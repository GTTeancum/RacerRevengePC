#include "search_list50br.h"
#include <limits>
namespace rr::game{
namespace{
struct Span{std::uintptr_t b=0,e=0;};
template<class T>bool span(const T*p,std::size_t n,Span&v)noexcept{v={};if(!n)return true;if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;auto b=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);if(bytes>std::numeric_limits<std::uintptr_t>::max()-b)return false;v={b,b+bytes};return true;}
bool overlap(Span a,Span b)noexcept{return a.b<b.e&&b.b<a.e;}
}
void resetSearchList50BR(SearchListCursor50BR&c,bool open)noexcept{c.block44=0;c.slot48=0;c.flags16=static_cast<std::uint8_t>((c.flags16&0xfeu)|(open?1u:0u));}
SearchListStatus50BR nextSearchList50BR(const SearchListInput50BR&in,SearchListCursor50BR&cursor,std::uint32_t&output)noexcept{
 using S=SearchListStatus50BR;Span c{},o{},i{},table{};
 if(!span(&cursor,1,c)||!span(&output,1,o)||!span(&in,1,i)||overlap(c,o)||overlap(c,i)||overlap(o,i))return S::InvalidInput;
 if(!in.blocks){if(in.blockCapacity)return S::InvalidInput;output=0;return S::Complete;}
 if(in.blockCount<0||in.perBlock30<=0||in.lastUsed38<0||in.lastUsed38>in.perBlock30||cursor.block44<0||cursor.block44>in.blockCount||cursor.slot48<0||cursor.slot48>in.perBlock30)return S::InvalidInput;
 if(in.blockCapacity<=static_cast<std::size_t>(in.blockCount))return S::InsufficientCapacity;
 if(!span(in.blocks,in.blockCapacity,table)||overlap(table,c)||overlap(table,o))return S::InvalidInput;
 for(std::size_t k=0;k<in.blockCapacity;++k){const auto&b=in.blocks[k];Span data{};if(!span(b.words,b.capacity,data)||overlap(data,c)||overlap(data,o))return S::InvalidInput;if(k<static_cast<std::size_t>(in.blockCount)&&b.capacity<static_cast<std::size_t>(k+1<static_cast<std::size_t>(in.blockCount)?in.perBlock30:in.lastUsed38))return S::InsufficientCapacity;}
 auto next=cursor;std::uint32_t value=0;
 while(next.block44<in.blockCount){const auto limit=next.block44+1<in.blockCount?in.perBlock30:in.lastUsed38;const auto&b=in.blocks[next.block44];
  while(next.slot48<limit&&(b.words[next.slot48]&2u))++next.slot48;
  if(next.slot48<limit){value=b.words[next.slot48++];break;}
  ++next.block44;if(next.block44<in.blockCount)next.slot48=0;
 }
 cursor=next;output=value;return S::Complete;
}
}
