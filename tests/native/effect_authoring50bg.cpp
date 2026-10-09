#include "effect_authoring50bg.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <new>
#include <cstring>
#include <vector>
using namespace rr::assets;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
static std::uint32_t bits(float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;}
int main(){nlohmann::json rows;std::ifstream input("evidence/effectAuthoring50BG/actual-expected.json");input>>rows;unsigned checks=0;for(const auto&row:rows){std::ifstream f("evidence/effectAuthoring50BF/members/"+row["file"].get<std::string>());std::string text((std::istreambuf_iterator<char>(f)),{});EffectControlAuthoring50BG actual{};auto before=allocations;if(!readEffectControlAuthoring50BG(text.data(),text.size(),actual)||allocations!=before)return 1;if(actual.hasMode!=row["has_mode"]||actual.hasOneShot!=row["has_one_shot"]||actual.hasDelay!=row["has_delay"]||!actual.hasVelocity||actual.mode!=row["mode"]||bits(actual.oneShot)!=row["one_shot"]||bits(actual.delay)!=row["delay"])return 1;checks+=7;for(unsigned i=0;i<3;++i){if(bits(actual.velocity[i])!=row["velocity"][i])return 1;++checks;}std::uint32_t mode=99,duration=99;if(!prepareEffectTiming50BG(actual,mode,duration)||mode!=row["mode"]||duration!=(mode?row["one_shot"].get<std::uint32_t>():0))return 1;checks+=2;}
 unsigned rejects=0;for(const char*text:{"[General Info]\nEmitMode=9","[General Info]\nEmitMode=1\nEmitMode=0","[General Info]\nOneShotTime=nan","[General Info]\nOneShotTime=1e3","[General Info]\nVelocity=1,2","[General Info]\nVelocity=1,2,3,4","[General Info]\nVelocity=1,,3","[General Info]\nDelayTime=0\nDelayTime=1","[General Info]\n[General Info]\n","[Wrong]\nEmitMode=0"}){EffectControlAuthoring50BG out;std::memset(&out,0xa5,sizeof out);unsigned char saved[sizeof out];std::memcpy(saved,&out,sizeof out);auto before=allocations;if(readEffectControlAuthoring50BG(text,std::strlen(text),out)||std::memcmp(saved,&out,sizeof out)||allocations!=before)return 1;++rejects;}
 for(unsigned mode:{2u,3u}){EffectControlAuthoring50BG in{};in.hasMode=true;in.mode=mode;std::uint32_t a=99,b=88;if(prepareEffectTiming50BG(in,a,b)||a!=99||b!=88)return 1;++rejects;}
 std::cout<<nlohmann::json{{"actual_files",rows.size()},{"checks",checks},{"rejected",rejects},{"hot_allocations",0},{"output_bytes",sizeof(EffectControlAuthoring50BG)}}.dump()<<'\n';}
