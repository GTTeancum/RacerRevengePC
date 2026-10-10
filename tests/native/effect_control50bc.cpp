#include "effect_control50bc.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <new>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned checks=0;for(const auto&row:j["cases"]){EffectControlState50BC s{};for(unsigned i=0;i<4;++i){s.position30[i]=row["pre"][i];s.previous40[i]=row["pre"][4+i];s.velocity60[i]=row["pre"][8+i];}s.word19C=row["pre"][12];s.intensity204=row["pre"][13];s.flags1D4=row["pre"][14].get<std::uint32_t>()&255u;std::uint32_t v[4];for(unsigned i=0;i<4;++i)v[i]=row["value"][i];const std::string op=row["op"];const unsigned alias=row["alias"];auto before=allocations;
 if(op=="start")startEffect50BC(s);else if(op=="stop")stopEffect50BC(s);else if(op=="position")positionEffect50BC(s,alias==1?s.position30:alias==2?s.previous40:v);else if(op=="velocity")velocityEffect50BC(s,v);else if(op=="intensity")intensityEffect50BC(s,v[0]);else return 1;
 if(before!=allocations)return 1;for(unsigned i=0;i<4;++i){if(s.position30[i]!=row["post"][i]||s.previous40[i]!=row["post"][i+4]||s.velocity60[i]!=row["post"][i+8])return 1;checks+=3;}if(s.word19C!=row["post"][12]||s.intensity204!=row["post"][13]||s.flags1D4!=(row["post"][14].get<std::uint32_t>()&255u)||(row["post"][14].get<std::uint32_t>()&0xffffff00)!=(row["pre"][14].get<std::uint32_t>()&0xffffff00))return 1;checks+=4;}
std::cout<<nlohmann::json{{"cases",j["cases"].size()},{"checks",checks},{"hot_allocations",0},{"state_bytes",sizeof(EffectControlState50BC)}}.dump()<<'\n';}
