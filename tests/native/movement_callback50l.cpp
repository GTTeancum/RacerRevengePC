#include "movement_callback50l.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <limits>
#include <cfenv>
#include <cstdlib>
#include <new>
static std::size_t allocations=0;
void* operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void operator delete(void*p) noexcept{std::free(p);}
void operator delete(void*p,std::size_t) noexcept{std::free(p);}
using namespace rr::game;
int main(int argc,char**argv){if(argc!=2)return 2;std::fesetround(FE_TONEAREST);std::ifstream in(argv[1]);nlohmann::json corpus;in>>corpus;unsigned checks=0;
for(const auto&row:corpus["cases"]){Vector3_48C other{},self{};for(unsigned i=0;i<3;++i){auto a=row["other"][i].get<std::uint32_t>(),b=row["self"][i].get<std::uint32_t>();std::memcpy(&other.v[i],&a,4);std::memcpy(&self.v[i],&b,4);}const bool dynamic=row["dynamic"];auto flag=row["initial"].get<std::uint32_t>();const auto before=allocations;auto status=applyMovementCallback50L(dynamic?2:4,2,dynamic?&other:nullptr,dynamic?&self:nullptr,flag);if(status!=MovementCallbackStatus50L::Complete||flag!=row["expected"].get<std::uint32_t>()||allocations!=before){std::cerr<<"original callback mismatch "<<checks<<'\n';return 1;}++checks;}
for(unsigned bad=0;bad<5;++bad){Vector3_48C other{{1,2,3}},self{{4,5,6}};const Vector3_48C*p=&other;std::uint32_t flag=0x89abcdef;
if(bad==0)p=nullptr;if(bad==1)other.v[0]=std::numeric_limits<float>::infinity();if(bad==2)other.v[1]=std::numeric_limits<float>::quiet_NaN();if(bad==3)other.v[0]=self.v[0]=std::numeric_limits<float>::max();if(bad==4)std::fesetround(FE_DOWNWARD);
const auto before=allocations;const auto status=applyMovementCallback50L(2,2,p,&self,flag);std::fesetround(FE_TONEAREST);if(status!=MovementCallbackStatus50L::InvalidInput||flag!=0x89abcdef||allocations!=before)return 1;++checks;}
std::cout<<"{\"original_cases\":"<<corpus["cases"].size()<<",\"rejections\":5,\"checks\":"<<checks<<",\"hot_allocations\":0}\n";
}
