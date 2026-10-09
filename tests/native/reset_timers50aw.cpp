#include "reset_timers50aw.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
using namespace rr::game;
static unsigned allocations=0;
void* operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned checks=0;for(const auto& row:j["cases"]){BaseTailState50X s;std::memset(&s,0xa5,sizeof s);s.flagsACC=row["pre"][11];s.flagsAD0=row["pre"][12];s.countAD4[0]=row["pre"][13];s.countAD4[1]=row["pre"][14];s.timerADC[0]=row["pre"][15];s.timerADC[1]=row["pre"][16];s.timerAE4=row["pre"][17];auto expected=s;expected.countAD4[0]=row["post"][13];expected.countAD4[1]=row["post"][14];expected.timerADC[0]=row["post"][15];expected.timerADC[1]=row["post"][16];expected.timerAE4=row["post"][17];unsigned n=allocations;resetBaseTimers50AW(s);if(allocations!=n||std::memcmp(&s,&expected,sizeof s))return 1;++checks;for(unsigned i=0;i<13;++i){if(row["pre"][i]!=row["post"][i])return 1;++checks;}}
std::cout<<nlohmann::json{{"cases",j["cases"].size()},{"checks",checks},{"hot_allocations",0},{"state_bytes",sizeof(BaseTailState50X)}}.dump()<<'\n';}
