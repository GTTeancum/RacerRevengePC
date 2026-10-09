#include "search_cold50bu.h"
#include "search_view50bt.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 1;nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned cases=0,checks=0,negative=0,registered=0;for(auto&row:j["cases"]){SearchStorage50BS state{};state.perBlock=row["pre_header"][0];state.blockCount=row["pre_header"][1];state.lastUsed=row["pre_header"][2];state.freeHead=row["pre_header"][3];for(unsigned i=0;i<1024;++i)state.words[i]=row["words"][i];auto expected=state;const unsigned cap=row["capacity"];auto n=allocations;
 if(initializeColdSearchStorage50BU(state,cap)!=SearchStorageStatus50BS::Complete||allocations!=n)return 1;expected.perBlock=row["constructor"][0];expected.blockCount=row["constructor"][1];expected.lastUsed=row["constructor"][2];expected.freeHead=row["constructor"][3];if(std::memcmp(&state,&expected,sizeof state))return 1;checks+=3;
 std::uint32_t slot=0xdeadbeef;n=allocations;if(adoptFirstSearchBlock50BU(state,slot)!=SearchStorageStatus50BS::Complete||slot!=row["selected"].get<unsigned>()||allocations!=n)return 1;expected.perBlock=row["adopted"][0];expected.blockCount=row["adopted"][1];expected.lastUsed=row["adopted"][2];expected.freeHead=row["adopted"][3];if(std::memcmp(&state,&expected,sizeof state))return 1;checks+=4;
 // Joined capacity/control exercise.318 is the observed authored model count,
 // not a claim that these opaque synthetic IDs are actual live registrations.
 const unsigned count=cap<318?cap:318;n=allocations;
 for(unsigned i=0;i<count;++i){if(selectSearchSlot50BS(state,0,slot)!=SearchStorageStatus50BS::Complete||slot!=i+1||insertSearchSlot50BS(state,slot,0x80000+i*4)!=SearchStorageStatus50BS::Complete)return 1;checks+=3;++registered;}
 SearchListBlock50BR descriptors[2]{};SearchListInput50BR view{};SearchListCursor50BR cursor{};if(prepareSearchView50BT(state,descriptors,2,view)!=SearchStorageStatus50BS::Complete)return 1;resetSearchList50BR(cursor,true);for(unsigned i=0;i<count;++i){std::uint32_t value=0;if(nextSearchList50BR(view,cursor,value)!=SearchListStatus50BR::Complete||value!=0x80000+i*4)return 1;checks+=2;}std::uint32_t value=0xdeadbeef;if(nextSearchList50BR(view,cursor,value)!=SearchListStatus50BR::Complete||value||allocations!=n)return 1;checks+=3;
 auto saved=state;slot=0xdeadbeef;if(adoptFirstSearchBlock50BU(state,slot)!=SearchStorageStatus50BS::InvalidInput||slot!=0xdeadbeef||initializeColdSearchStorage50BU(state,0)!=SearchStorageStatus50BS::InvalidInput||initializeColdSearchStorage50BU(state,1025)!=SearchStorageStatus50BS::InvalidInput||std::memcmp(&state,&saved,sizeof state))return 1;negative+=3;checks+=5;++cases;
 }
 std::cout<<nlohmann::json{{"original_fragment_cases",cases},{"checks",checks},{"joined_registrations",registered},{"negative_controls",negative},{"factory_capacity",1024},{"largest_joined_list",318},{"hot_allocations",0},{"state_bytes",sizeof(SearchStorage50BS)}}.dump()<<'\n';}
