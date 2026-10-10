#include "effect_stop50bd.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <new>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned checks=0,deferred=0;for(const auto&row:j["cases"]){EffectControlState50BC s{};for(unsigned i=0;i<4;++i){s.position30[i]=row["pre"][i];s.previous40[i]=row["pre"][4+i];s.velocity60[i]=row["pre"][8+i];}s.word19C=row["pre"][12];s.intensity204=row["pre"][13];s.flags1D4=row["pre"][14].get<std::uint32_t>()&255u;const auto mode=row["mode"].get<std::uint32_t>(),duration=row["duration"].get<std::uint32_t>();auto before=allocations;const auto status=stopDeferredEffect50BD(s,mode,duration);if(before!=allocations||status!=(row["stopped"].get<bool>()?EffectStopResult50BD::Stopped:EffectStopResult50BD::Deferred))return 1;++checks;if(status==EffectStopResult50BD::Deferred)++deferred;
 for(unsigned i=0;i<4;++i){if(s.position30[i]!=row["post"][i]||s.previous40[i]!=row["post"][i+4]||s.velocity60[i]!=row["post"][i+8])return 1;checks+=3;}if(s.word19C!=row["post"][12]||s.intensity204!=row["post"][13]||s.flags1D4!=(row["post"][14].get<std::uint32_t>()&255u))return 1;checks+=3;}
 unsigned rejected=0;for(std::uint32_t value:{0x7f800000u,0xff800000u,0x7fc01234u,0x7f801234u,0xffc01234u,1u,0x80000001u}){EffectControlState50BC s;std::memset(&s,0xa5,sizeof s);s.flags1D4|=2u;auto saved=s;auto before=allocations;if(stopDeferredEffect50BD(s,1,value)!=EffectStopResult50BD::UnsupportedDuration||std::memcmp(&s,&saved,sizeof s)||allocations!=before)return 1;++rejected;s=saved;if(stopDeferredEffect50BD(s,2,value)!=EffectStopResult50BD::Stopped)return 1;s=saved;s.flags1D4&=~2u;if(stopDeferredEffect50BD(s,1,value)!=EffectStopResult50BD::Stopped)return 1;}
 std::cout<<nlohmann::json{{"cases",j["cases"].size()},{"checks",checks},{"deferred",deferred},{"rejected_consumed",rejected},{"unconsumed_controls",2*rejected},{"hot_allocations",0}}.dump()<<'\n';}
