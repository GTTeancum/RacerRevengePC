#include "search_list50br.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 1;nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned steps=0,checks=0,negative=0,sequences=0;
 for(auto&row:j["cases"]){auto words=row["blocks"].get<std::vector<std::vector<std::uint32_t>>>();std::vector<SearchListBlock50BR>blocks(words.size()+1);for(unsigned k=0;k<words.size();++k)blocks[k]={words[k].data(),words[k].size()};SearchListInput50BR in{row["null"].get<bool>()?nullptr:blocks.data(),row["null"].get<bool>()?0:blocks.size(),row["count"],row["capacity"],row["used"]};SearchListCursor50BR cursor{};cursor.flags16=row["flags"];
  for(auto&step:row["steps"]){unsigned entry=step["entry"];std::uint32_t value=0xdeadbeef;auto n=allocations;
   if(entry==0x27e6c0){if(nextSearchList50BR(in,cursor,value)!=SearchListStatus50BR::Complete||value!=step["value"].get<std::uint32_t>())return 1;checks+=2;}else resetSearchList50BR(cursor,entry!=0x27e850);
   if(allocations!=n||cursor.block44!=step["cursor"][0].get<int>()||cursor.slot48!=step["cursor"][1].get<int>()||cursor.flags16!=step["cursor"][2].get<unsigned>()){std::cerr<<"cursor mismatch "<<sequences<<" "<<steps<<'\n';return 1;}checks+=4;++steps;
  }
  if(in.blocks){auto before=cursor;auto reject=[&](SearchListInput50BR bad){std::uint32_t value=0x12345678;auto old=cursor;auto n=allocations;auto status=nextSearchList50BR(bad,cursor,value);if(status==SearchListStatus50BR::Complete||cursor.block44!=old.block44||cursor.slot48!=old.slot48||cursor.flags16!=old.flags16||value!=0x12345678||allocations!=n)return false;++negative;checks+=6;return true;};auto bad=in;bad.blockCapacity=in.blockCount;if(!reject(bad))return 1;bad=in;bad.lastUsed38=in.perBlock30+1;if(!reject(bad))return 1;bad=in;bad.blockCount=-1;if(!reject(bad))return 1;}
  ++sequences;
 }
 SearchListInput50BR null{};null.blockCount=-123;null.perBlock30=-45;null.lastUsed38=-67;SearchListCursor50BR c{-90,-100,0xab};std::uint32_t v=9;if(nextSearchList50BR(null,c,v)!=SearchListStatus50BR::Complete||v||c.block44!=-90||c.slot48!=-100||c.flags16!=0xab)return 1;checks+=5;
 std::cout<<nlohmann::json{{"original_sequences",sequences},{"original_operations",steps},{"checks",checks},{"negative_controls",negative},{"hot_allocations",0}}.dump()<<'\n';}
