#include "spatial_point_key50bn.h"
#include <cmath>
#include <cfenv>
#include <initializer_list>
namespace rr::game {
namespace {bool ordinary(float x)noexcept{return std::isfinite(x)&&(x==0||std::fpclassify(x)!=FP_SUBNORMAL);}}
SpatialPointKeyStatus50BN spatialPointKey50BN(const SpatialPointKeyInput50BN& input,std::uint32_t& output)noexcept{
 const auto in=input;using S=SpatialPointKeyStatus50BN;
 if(!in.quantum58||in.quantum58>32768||(in.quantum58&(in.quantum58-1))||in.level54>15||in.extent48<0||in.scale5C<=0)return S::InvalidInput;
 for(float v:{in.originX30,in.originZ38,in.extent48,in.scale5C,in.pointX,in.pointZ})if(!ordinary(v))return S::UnsupportedArithmetic;
 if(std::fegetround()!=FE_TONEAREST)return S::UnsupportedArithmetic;
 volatile float dx=in.pointX-in.originX30,dz=in.pointZ-in.originZ38;
 if(!ordinary(dx)||!ordinary(dz))return S::UnsupportedArithmetic;
 if(dx<0||dx>in.extent48||dz<0||dz>in.extent48){output=0;return S::Complete;}
 volatile float sx=dx*in.scale5C,sz=dz*in.scale5C;
 if(!ordinary(sx)||!ordinary(sz)||sx>=2147483648.0f||sz>=2147483648.0f)return S::UnsupportedArithmetic;
 const auto x=static_cast<std::uint32_t>(sx),z=static_cast<std::uint32_t>(sz);
 const auto fineMask=~(in.quantum58-1),coarseMask=~(2*in.quantum58-1);
 auto coarseX=x&coarseMask,coarseZ=z&coarseMask;
 const auto selector=(coarseX!=(x&fineMask)?2u:0u)|(coarseZ!=(z&fineMask)?1u:0u);
 const auto flags=16u<<selector;
 if(coarseX>=32768)coarseX=32767;if(coarseZ>=32768)coarseZ=32767;
 output=(coarseX<<17)|(coarseZ<<5)|flags|in.level54;return S::Complete;
}
}
