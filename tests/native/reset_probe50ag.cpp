#include "reset_probe50ag.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
#include <cfenv>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;std::fesetround(FE_TONEAREST);nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned cases=0,checks=0;
for(const auto&row:j["cases"]){std::uint32_t p[4];for(unsigned i=0;i<4;++i)p[i]=row["staged"][i];ResetProbe50AG s{};auto a=allocations;auto lift=row["lift"].get<std::uint32_t>();if(!beginResetProbe50AG(p,lift,s)||allocations!=a)return 1;unsigned attempt=0;
for(const auto&q:row["requests"]){for(unsigned i=0;i<8;++i){if((i<4?s.start[i]:s.end[i-4])!=q[i]){std::cerr<<cases<<" request "<<attempt<<" word "<<i<<'\n';return 1;}++checks;}a=allocations;bool hit=int(attempt)==row["hit_at"].get<int>();auto hy=row["hit_y"].get<std::uint32_t>();if(!acceptResetProbe50AG(s,hit,hy)||allocations!=a)return 1;++attempt;}
for(unsigned i=0;i<4;++i){if(s.staged[i]!=row["final"][i])return 1;++checks;}if(s.phase!=(row["hit_at"].get<int>()<0?ResetProbePhase50AG::Exhausted:ResetProbePhase50AG::Hit))return 1;auto saved=s;if(acceptResetProbe50AG(s,false,0)||std::memcmp(&s,&saved,sizeof s))return 1;++cases;}
unsigned rejects=0;for(unsigned k=0;k<4;++k){std::uint32_t p[4]={0,0x3f800000,0,0x7fc12345};ResetProbe50AG s{};if(!beginResetProbe50AG(p,0x40a00000,s))return 1;if(k==0)s.attempt=20;if(k==1)s.phase=ResetProbePhase50AG::Hit;if(k==2)s.staged[0]=0x7f800000;auto saved=s;if(acceptResetProbe50AG(s,k==3,0x7fc00000)||std::memcmp(&saved,&s,sizeof s))return 1;++rejects;}
std::cout<<nlohmann::json{{"cases",cases},{"word_checks",checks},{"rejects",rejects},{"state_bytes",sizeof(ResetProbe50AG)},{"hot_allocations",0}}.dump()<<'\n';}
