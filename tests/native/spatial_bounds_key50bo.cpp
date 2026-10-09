#include "spatial_bounds_key50bo.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <bit>
#include <cstdlib>
#include <new>
#include <cfenv>
using namespace rr::game;static unsigned allocations=0;
void*operator new(std::size_t n){++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}void operator delete(void*p)noexcept{std::free(p);}void operator delete(void*p,std::size_t)noexcept{std::free(p);}
int main(int argc,char**argv){if(argc!=2)return 1;nlohmann::json j;std::ifstream f(argv[1]);f>>j;unsigned cases=0,misses=0,checks=0,rejected=0;
 for(auto&row:j["cases"]){auto a=row["floats"].get<std::vector<std::uint32_t>>();SpatialBoundsKeyInput50BO in{std::bit_cast<float>(a[0]),std::bit_cast<float>(a[1]),std::bit_cast<float>(a[2]),std::bit_cast<float>(a[3]),std::bit_cast<float>(a[4]),std::bit_cast<float>(a[5]),std::bit_cast<float>(a[6]),std::bit_cast<float>(a[7]),std::bit_cast<float>(a[8]),std::bit_cast<float>(a[9]),std::bit_cast<float>(a[10])};auto quanta=row["quanta"].get<std::vector<std::uint32_t>>();in.maximumLevel54=quanta.size()-1;for(unsigned k=0;k<quanta.size();++k)in.quanta60[k]=quanta[k];std::uint32_t out=0x12345678;auto n=allocations;
 if(spatialBoundsKey50BO(in,out)!=SpatialBoundsKeyStatus50BO::Complete||allocations!=n||out!=row["expected"].get<std::uint32_t>()){std::cerr<<"case "<<cases<<" got "<<out<<" expected "<<row["expected"]<<'\n';return 1;}++cases;checks+=3;misses+=out==0;
 auto reject=[&](SpatialBoundsKeyInput50BO bad){std::uint32_t dest=0xa55aa55a;auto before=allocations;auto s=spatialBoundsKey50BO(bad,dest);checks+=3;if(s==SpatialBoundsKeyStatus50BO::Complete||dest!=0xa55aa55a||allocations!=before)return false;++rejected;return true;};
 auto bad=in;bad.quanta60[0]=3;if(!reject(bad))return 1;bad=in;bad.maximumLevel54=16;if(!reject(bad))return 1;bad=in;bad.centerX=std::bit_cast<float>(0x7fc01234u);if(!reject(bad))return 1;bad=in;bad.scale5C=std::bit_cast<float>(1u);if(!reject(bad))return 1;
 }
 SpatialBoundsKeyInput50BO in{};in.extentX48=in.extentZ4C=1;in.quanta60[0]=1;std::uint32_t out=5;std::fesetround(FE_DOWNWARD);auto status=spatialBoundsKey50BO(in,out);std::fesetround(FE_TONEAREST);if(status!=SpatialBoundsKeyStatus50BO::UnsupportedArithmetic||out!=5)return 1;
 std::cout<<nlohmann::json{{"original_cases",cases},{"checks",checks+2},{"outside_or_zero_keys",misses},{"rejections",rejected+1},{"hot_allocations",0}}.dump()<<'\n';}
