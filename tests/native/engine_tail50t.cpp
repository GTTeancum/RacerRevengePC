#include "engine_tail50t.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <new>
#include <cstring>
#include <cfenv>
static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void*p)noexcept{std::free(p);}
void operator delete(void*p,std::size_t)noexcept{std::free(p);}
using namespace rr::game;
int main(int argc,char**argv){if(argc!=2)return 2;std::ifstream file(argv[1]);nlohmann::json fixtures;file>>fixtures;unsigned checks=0;
for(const auto&row:fixtures["cases"]){const auto&i=row["input"];const auto&e=row["expected"];EngineTailState50T s{};s.flagsACC=i["acc"];s.flagsAD0=i["ad0"];s.timerAE4=i["timerAE4"];s.activeAE8=i["activeAE8"];s.field844=i["field844"];s.sharedRandomState=i["rng"];for(unsigned side=0;side<2;++side){s.countAD4[side]=i["counts"][side];s.timerADC[side]=i["timers"][side];for(unsigned k=0;k<4;++k)s.thrustA0[side][k]=row["initial_thrust"][side*4+k];}
EngineTailInput50T in{};in.flagsB9C=16;in.application58=i["application58"];in.state40=i["state40"];in.frameFactorBits=i["dt"];const auto before=s;const auto cold=allocations;auto out=advanceEngineTail50T(s,in);if(allocations!=cold||std::memcmp(&s,&before,sizeof s))return 1;if(out.status!=EngineTailStatus50T::Complete)return 1;
const std::uint32_t got[]={out.state.flagsACC,out.state.flagsAD0,out.state.countAD4[0],out.state.countAD4[1],out.state.timerADC[0],out.state.timerADC[1],out.state.timerAE4,out.state.activeAE8,out.state.field844,out.state.sharedRandomState};const char*keys[]={"0xacc","0xad0","0xad4","0xad8","0xadc","0xae0","0xae4","0xae8","0x844","rng"};for(unsigned k=0;k<10;++k){if(got[k]!=e[keys[k]].get<unsigned>()){std::cerr<<"case "<<checks/19<<" "<<keys[k]<<" got "<<got[k]<<" expected "<<e[keys[k]]<<'\n';return 1;}++checks;}
for(unsigned side=0;side<2;++side)for(unsigned k=0;k<4;++k){if(out.state.thrustA0[side][k]!=e["thrust"][side*4+k]){std::cerr<<"thrust mismatch\n";return 1;}++checks;}if(out.nullProbeResetExecuted!=(e["probeReset"].get<unsigned>()!=0))return 1;++checks;}
unsigned controls=0;EngineTailState50T s{};EngineTailInput50T in{};auto out=advanceEngineTail50T(s,in);if(out.status!=EngineTailStatus50T::NeedsFlameInitialization)return 1;++controls;
in.flagsB9C=16;in.frameFactorBits=0x7f800001u;out=advanceEngineTail50T(s,in);if(out.status!=EngineTailStatus50T::Complete)return 1;++controls; // unused timestep word
s.activeAE8=1;s.timerAE4=0x7f800001u;out=advanceEngineTail50T(s,in);if(out.status!=EngineTailStatus50T::Arithmetic||std::memcmp(&out.state,&s,sizeof s))return 1;++controls;
s.timerAE4=0x3c23d70au;in.frameFactorBits=0x3c83126fu;in.probe128Present=true;out=advanceEngineTail50T(s,in);if(out.status!=EngineTailStatus50T::NeedsProbeReset||std::memcmp(&out.state,&s,sizeof s))return 1;++controls;
in.probe128Present=false;out=advanceEngineTail50T(s,in);if(out.status!=EngineTailStatus50T::Complete||!out.nullProbeResetExecuted)return 1;++controls;
std::fesetround(FE_UPWARD);out=advanceEngineTail50T(s,in);std::fesetround(FE_TONEAREST);if(out.status!=EngineTailStatus50T::Arithmetic)return 1;++controls;
std::cout<<"{\"original_cases\":"<<fixtures["cases"].size()<<",\"checks\":"<<checks<<",\"admission_controls\":"<<controls<<",\"hot_allocations\":0}\n";}
