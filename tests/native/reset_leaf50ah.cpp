#include "reset_leaf50ah.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
#include <cfenv>
#include <bit>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 2;std::fesetround(FE_TONEAREST);nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned cases=0,checks=0,hits=0;
for(const auto&row:j["cases"]){std::uint32_t u[39];for(unsigned i=0;i<39;++i)u[i]=row["input"][i];Type3Triangle48P t{};for(unsigned v=0;v<3;++v)std::memcpy(t.vertices[v].v,u+3*v,12);Type3Segment48P s{};std::memcpy(s.start.v,u+20,16);std::memcpy(s.direction.v,u+24,16);s.length=std::bit_cast<float>(u[28]);ResetLeafResult50AH out{};std::memcpy(&out,u+29,36);auto n=allocations;auto status=intersectResetFlatTriangle50AH(s,t,out);if(allocations!=n||unsigned(status!=Type3LeafStatus48P::Miss)!=row["hit"]||(status!=Type3LeafStatus48P::Miss&&status!=Type3LeafStatus48P::Hit)){std::cerr<<row["name"]<<" status "<<int(status)<<'\n';return 1;}hits+=status==Type3LeafStatus48P::Hit;std::uint32_t actual[9];std::memcpy(actual,&out,36);for(unsigned i=0;i<9;++i){if(actual[i]!=row["expected"][i]){std::cerr<<row["name"]<<" word "<<i<<" got "<<std::hex<<actual[i]<<" expected "<<row["expected"][i]<<'\n';return 1;}++checks;}++cases;}
Type3Segment48P s{};Type3Triangle48P t{};t.hasVertexNormals=true;ResetLeafResult50AH out{};auto saved=out;if(intersectResetFlatTriangle50AH(s,t,out)!=Type3LeafStatus48P::UnsupportedNormalMode||std::memcmp(&saved,&out,36))return 1;
std::cout<<nlohmann::json{{"cases",cases},{"hits",hits},{"word_checks",checks},{"hot_allocations",0},{"result_bytes",sizeof out}}.dump()<<'\n';}
