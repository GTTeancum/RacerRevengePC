#include "reset_lifecycle50at.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
#include <bit>
#include <cfenv>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned cases=0,checks=0;for(const auto&row:j["cases"]){ResetLifecycleGlobals50AT g{};for(unsigned i=0;i<4;++i){g.zero434630[i]=row["zero"][i];g.forward434660[i]=row["forward"][i];g.quaternion382B50[i]=row["quaternion"][i];}ChariotLifecycleState49V state{};state.vehicle840=0xdeadbeef;state.duration848=2.f;state.previous85C=3.f;std::uint32_t active=0;auto n=allocations;if(!resetLifecycleRecords50AT(active,state,g)||allocations!=n)return 1;std::uint32_t words[16];std::memcpy(words,&state.response,64);for(unsigned i=0;i<16;++i){if(words[i]!=row["response"][i])return 1;++checks;}if(active!=row["active"]||state.vehicle83C!=row["state"]||std::bit_cast<std::uint32_t>(state.elapsed84C)!=row["elapsed"]||state.vehicle840!=0xdeadbeef||state.duration848!=2.f||state.previous85C!=3.f)return 1;checks+=6;++cases;}
ResetLifecycleGlobals50AT g{};initializedResetLifecycleGlobals50AT(g);auto&first=j["cases"][0];for(unsigned i=0;i<4;++i)if(g.zero434630[i]!=first["zero"][i]||g.forward434660[i]!=first["forward"][i]||g.quaternion382B50[i]!=first["quaternion"][i])return 1;g.forward434660[0]=0x7fc00000;ChariotLifecycleState49V s{};auto saved=s;std::uint32_t active=23;if(resetLifecycleRecords50AT(active,s,g)||active!=23||std::memcmp(&s,&saved,sizeof s))return 1;
std::cout<<nlohmann::json{{"cases",cases},{"checks",checks},{"rejected",1},{"hot_allocations",0}}.dump()<<'\n';}
