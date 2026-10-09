#include "movement_hit_point50m.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cfenv>
#include <limits>
using namespace rr::game;
static float fp(std::uint32_t x){float f;std::memcpy(&f,&x,4);return f;}
static std::uint32_t bits(float f){std::uint32_t x;std::memcpy(&x,&f,4);return x;}
int main(int argc,char**argv){if(argc!=2)return 2;std::fesetround(FE_TONEAREST);nlohmann::json data;std::ifstream in(argv[1]);in>>data;unsigned checks=0;
for(const auto&j:data["cases"]){SpringVector4_48N current{},point{},out{};for(unsigned i=0;i<4;++i){current.v[i]=fp(j["current"][i]);point.v[i]=fp(j["point"][i]);}
if(!computeMovementHitPoint50M(current,point,fp(j["push"]),out))return 1;
for(unsigned i=0;i<4;++i){if(bits(out.v[i])!=j["expected"][i].get<std::uint32_t>()){std::cerr<<"source mismatch "<<checks<<' '<<i<<'\n';return 1;}++checks;}
if(!computeMovementHitPoint50M(current,point,fp(j["push"]),current)||std::memcmp(&current,&out,sizeof out))return 1;++checks;
}
for(unsigned i=0;i<5;++i){SpringVector4_48N a{{1,2,3,4}},b{{2,3,4,5}},out{{6,7,8,9}};const auto old=out;float push=10;
if(i==0)a.v[0]=std::numeric_limits<float>::quiet_NaN();if(i==1)push=std::numeric_limits<float>::infinity();if(i==2)b.v[1]=std::numeric_limits<float>::max();if(i==3){a={{0,0,0,1}};b={{1e-20f,0,0,1}};}if(i==4)std::fesetround(FE_DOWNWARD);
const bool okay=computeMovementHitPoint50M(a,b,push,out);std::fesetround(FE_TONEAREST);if(okay||std::memcmp(&out,&old,sizeof out))return 1;++checks;}
std::cout<<"{\"original_cases\":24,\"checks\":"<<checks<<",\"rejections\":5}\n";
}
