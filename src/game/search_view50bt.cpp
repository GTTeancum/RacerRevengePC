#include "search_view50bt.h"
#include <limits>
namespace rr::game{namespace{
bool overlap(const void*a,std::size_t n,const void*b,std::size_t m)noexcept{auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);return n&&m&&(x<=y?y-x<n:x-y<m);}
}
SearchStorageStatus50BS prepareSearchView50BT(const SearchStorage50BS&s,SearchListBlock50BR*desc,std::size_t capacity,SearchListInput50BR&out)noexcept{
 using S=SearchStorageStatus50BS;
 if(capacity>std::numeric_limits<std::size_t>::max()/sizeof(*desc)||(capacity&&(!desc||reinterpret_cast<std::uintptr_t>(desc)%alignof(SearchListBlock50BR))))return S::InvalidInput;
 auto bytes=capacity*sizeof(*desc);auto address=reinterpret_cast<std::uintptr_t>(desc);if(bytes>std::numeric_limits<std::uintptr_t>::max()-address||overlap(desc,bytes,&s,sizeof s)||overlap(desc,bytes,&out,sizeof out)||overlap(&s,sizeof s,&out,sizeof out))return S::InvalidInput;
 std::uint32_t unused=0;auto status=selectSearchSlot50BS(s,0,unused);if(status!=S::Complete&&status!=S::GrowthBoundary)return status;
 if(!s.blockCount){out={nullptr,0,0,static_cast<std::int32_t>(s.perBlock),static_cast<std::int32_t>(s.lastUsed)};return S::Complete;}
 if(capacity<=s.blockCount)return S::InvalidInput;
 for(unsigned i=0;i<s.blockCount;++i)desc[i]={s.words+i*s.perBlock,s.perBlock};desc[s.blockCount]={};
 out={desc,static_cast<std::size_t>(s.blockCount)+1,static_cast<std::int32_t>(s.blockCount),static_cast<std::int32_t>(s.perBlock),static_cast<std::int32_t>(s.lastUsed)};return S::Complete;
}
}
