#include "spatial_items50bp.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
void load(const nlohmann::json&j,SpatialItemState50BP&s){s.nodeCount=4;s.blockCount=12;s.freeHead=j["free"];s.bytesUsed=j["bytes"];for(unsigned n=0;n<4;++n){s.nodes[n].firstBlock=j["heads"][n];for(unsigned k=0;k<4;++k)s.nodes[n].children[k]=j["children"][n][k];}for(unsigned i=0;i<12;++i){s.blocks[i].next=j["blocks"][i][0];for(unsigned k=0;k<4;++k)s.blocks[i].identities[k]=j["blocks"][i][k+1];}}
int main(int argc,char**argv){if(argc!=2)return 1;nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned sequences=0,steps=0,boundaries=0,rejected=0;std::uint64_t checks=0;SpatialItemWorkspace50BP work{};
 for(auto& sequence:j["cases"]){SpatialItemState50BP state;std::memset(&state,0xa5,sizeof state);load(sequence["initial"],state);
  for(auto& row:sequence["steps"]){const auto node=row["node"].get<std::uint32_t>(),id=row["identity"].get<std::uint32_t>();const bool add=row["add"],boundary=row["boundary"];auto before=state,expected=state;load(row["after"],expected);SpatialItemResult50BP result{true};auto n=allocations;auto status=editSpatialItems50BP(state,node,id,add,work,result);if(allocations!=n)return 1;++checks;
   if(boundary){if(status!=SpatialItemStatus50BP::AllocatorBoundary||std::memcmp(&state,&before,sizeof state)||!result.hasItemsOrChildren)return 1;++boundaries;checks+=3;}
   else{if(status!=SpatialItemStatus50BP::Complete||std::memcmp(&state,&expected,sizeof state)){std::cerr<<"state mismatch sequence "<<sequences<<" step "<<steps<<'\n';return 1;}checks+=2;if(!add){if(result.hasItemsOrChildren!=(row["alive"].get<unsigned>()!=0))return 1;++checks;}}
   ++steps;
  }
  auto reject=[&](SpatialItemState50BP bad,std::uint32_t node=0,std::uint32_t id=0x90004){auto saved=bad;SpatialItemResult50BP result{true};auto n=allocations;auto status=editSpatialItems50BP(bad,node,id,false,work,result);if(status==SpatialItemStatus50BP::Complete||std::memcmp(&saved,&bad,sizeof bad)||!result.hasItemsOrChildren||allocations!=n)return false;++rejected;checks+=4;return true;};
  if(!reject(state,state.nodeCount)||!reject(state,0,0))return 1;auto bad=state;++bad.bytesUsed;if(!reject(bad))return 1;
  for(unsigned n=0;n<4;++n)if(state.nodes[n].firstBlock){bad=state;auto ref=bad.nodes[n].firstBlock;for(auto&x:bad.blocks[ref-1].identities)x=0;if(!reject(bad))return 1;bad=state;bad.blocks[ref-1].next=ref;if(!reject(bad))return 1;bad=state;bad.freeHead=ref;if(!reject(bad))return 1;break;}
  work.staged=state;auto saved=work.staged;SpatialItemResult50BP result{true};if(editSpatialItems50BP(work.staged,0,0x90004,true,work,result)!=SpatialItemStatus50BP::InvalidInput||std::memcmp(&saved,&work.staged,sizeof saved)||!result.hasItemsOrChildren)return 1;++rejected;checks+=3;++sequences;
 }
 std::cout<<nlohmann::json{{"original_sequences",sequences},{"original_operations",steps},{"checks",checks},{"allocator_boundaries",boundaries},{"negative_controls",rejected},{"hot_allocations",0},{"state_bytes",sizeof(SpatialItemState50BP)},{"workspace_bytes",sizeof work}}.dump()<<'\n';}
