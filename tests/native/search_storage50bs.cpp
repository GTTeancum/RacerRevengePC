#include "search_storage50bs.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
void load(const nlohmann::json&j,SearchStorage50BS&s){s.lastUsed=j["used"];s.freeHead=j["free"];for(unsigned i=0;i<j["words"].size();++i)s.words[i]=j["words"][i];}
int main(int argc,char**argv){if(argc!=2)return 1;nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned sequences=0,steps=0,boundaries=0,rejected=0,checks=0;
 for(auto&row:j["cases"]){SearchStorage50BS state;std::memset(&state,0xa5,sizeof state);state.perBlock=row["capacity"];state.blockCount=row["count"];load(row["initial"],state);
  for(auto&step:row["steps"]){auto op=step["op"].get<std::string>();auto expected=state,before=state;load(step["after"],expected);SearchStorageStatus50BS status;std::uint32_t selected=0xdeadbeef;unsigned slot=step.value("slot",0u),identity=step.value("identity",0u),previous=step.value("previous",0u);auto n=allocations;
   if(op=="select")status=selectSearchSlot50BS(state,previous,selected);else if(op=="insert")status=insertSearchSlot50BS(state,slot,identity);else status=removeSearchSlot50BS(state,slot,identity);
   if(allocations!=n)return 1;++checks;
   if(op=="select"&&step["selected"].is_null()){if(status!=SearchStorageStatus50BS::GrowthBoundary||selected!=0xdeadbeef||std::memcmp(&state,&before,sizeof state))return 1;++boundaries;checks+=3;}
   else{if(status!=SearchStorageStatus50BS::Complete||std::memcmp(&state,&expected,sizeof state)){std::cerr<<"state mismatch "<<sequences<<' '<<steps<<'\n';return 1;}checks+=2;if(op=="select"){if(selected!=step["selected"].get<unsigned>())return 1;++checks;}}
   ++steps;
  }
  auto reject=[&](SearchStorage50BS bad){auto saved=bad;std::uint32_t selected=0xa55aa55a;auto n=allocations;auto status=selectSearchSlot50BS(bad,0,selected);if(status==SearchStorageStatus50BS::Complete||status==SearchStorageStatus50BS::GrowthBoundary||std::memcmp(&saved,&bad,sizeof bad)||selected!=0xa55aa55a||allocations!=n)return false;++rejected;checks+=5;return true;};
  auto bad=state;bad.perBlock=0;if(!reject(bad))return 1;bad=state;bad.blockCount=1025;if(!reject(bad))return 1;bad=state;bad.lastUsed=state.perBlock+1;if(!reject(bad))return 1;bad=state;bad.freeHead=1025;if(!reject(bad))return 1;
  const auto total=(state.blockCount-1)*state.perBlock+state.lastUsed;if(total){bad=state;bad.freeHead=1;bad.words[0]=(1u<<2)|2;if(!reject(bad))return 1;bad=state;bad.words[0]=0;if(!reject(bad))return 1;}
  auto saved=state;auto n=allocations;if(insertSearchSlot50BS(state,1,0)!=SearchStorageStatus50BS::InvalidInput||removeSearchSlot50BS(state,0,0x90004)!=SearchStorageStatus50BS::InvalidInput||selectSearchSlot50BS(state,0,state.freeHead)!=SearchStorageStatus50BS::InvalidInput||std::memcmp(&saved,&state,sizeof state)||allocations!=n)return 1;rejected+=3;checks+=5;++sequences;
 }
 std::cout<<nlohmann::json{{"original_sequences",sequences},{"original_operations",steps},{"checks",checks},{"growth_boundaries",boundaries},{"negative_controls",rejected},{"hot_allocations",0},{"state_bytes",sizeof(SearchStorage50BS)},{"validation_bitset_bytes",128}}.dump()<<'\n';}
