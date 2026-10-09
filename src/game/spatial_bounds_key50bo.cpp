#include "spatial_bounds_key50bo.h"
#include <cmath>
#include <cfenv>
#include <initializer_list>
namespace rr::game {
namespace{bool ordinary(float x)noexcept{return std::isfinite(x)&&(x==0||std::fpclassify(x)!=FP_SUBNORMAL);}float add(float a,float b)noexcept{volatile float x=a+b;return x;}float sub(float a,float b)noexcept{volatile float x=a-b;return x;}float mul(float a,float b)noexcept{volatile float x=a*b;return x;}}
SpatialBoundsKeyStatus50BO spatialBoundsKey50BO(const SpatialBoundsKeyInput50BO& input,std::uint32_t& output)noexcept{
 const auto in=input;using S=SpatialBoundsKeyStatus50BO;
 if(in.maximumLevel54>15||in.maximumX34<in.originX30||in.maximumZ3C<in.originZ38||in.extentX48<0||in.extentZ4C<0||in.scale5C<=0||in.halfX<0||in.halfZ<0)return S::InvalidInput;
 for(unsigned i=0;i<=in.maximumLevel54;++i){auto q=in.quanta60[i];if(!q||q>32768||(q&(q-1))||(i&&q*2!=in.quanta60[i-1]))return S::InvalidInput;}
 for(float v:{in.originX30,in.maximumX34,in.originZ38,in.maximumZ3C,in.extentX48,in.extentZ4C,in.scale5C,in.centerX,in.centerZ,in.halfX,in.halfZ})if(!ordinary(v))return S::UnsupportedArithmetic;
 if(std::fegetround()!=FE_TONEAREST)return S::UnsupportedArithmetic;
 auto lx=sub(in.centerX,in.halfX),hx=add(in.centerX,in.halfX),lz=sub(in.centerZ,in.halfZ),hz=add(in.centerZ,in.halfZ);
 for(float v:{lx,hx,lz,hz})if(!ordinary(v))return S::UnsupportedArithmetic;
 if(lx<in.originX30)lx=in.originX30;if(!(hx<=in.maximumX34))hx=in.maximumX34;
 if(lz<in.originZ38)lz=in.originZ38;if(!(hz<=in.maximumZ3C))hz=in.maximumZ3C;
 hx=sub(hx,in.originX30);lx=sub(lx,in.originX30);lz=sub(lz,in.originZ38);hz=sub(hz,in.originZ38);
 for(float v:{lx,hx,lz,hz})if(!ordinary(v))return S::UnsupportedArithmetic;
 if(hx<0||lx>in.extentX48||hz<0||lz>in.extentZ4C){output=0;return S::Complete;}
 lx=mul(lx,in.scale5C);lz=mul(lz,in.scale5C);hx=mul(hx,in.scale5C);hz=mul(hz,in.scale5C);
 for(float v:{lx,hx,lz,hz})if(!ordinary(v)||v<0||double(v)>=2147483647.0)return S::UnsupportedArithmetic;
 auto x=static_cast<std::uint32_t>(lx),z=static_cast<std::uint32_t>(lz),endX=static_cast<std::uint32_t>(hx)+1,endZ=static_cast<std::uint32_t>(hz)+1;
 if(endX<x||endZ<z)return S::UnsupportedArithmetic;
 const auto widthX=endX-x,widthZ=endZ-z;unsigned level=0;
 while(level<=in.maximumLevel54&&widthX<in.quanta60[level]&&widthZ<in.quanta60[level])++level;
 if(level>in.maximumLevel54)level=in.maximumLevel54;
 auto quantum=in.quanta60[level],mask=~(quantum-1);x&=mask;endX&=mask;z&=mask;endZ&=mask;
 if((endX-x>quantum||endZ-z>quantum)&&level){quantum+=quantum;mask=~(quantum-1);--level;x&=mask;endX&=mask;z&=mask;endZ&=mask;}
 auto flags=16u;if(endZ!=z)flags|=32;if(endX!=x)flags|=64;if((flags&96)==96)flags|=128;
 if(x>=32768)x=32767;if(z>=32768)z=32767;output=(x<<17)|(z<<5)|flags|level;return S::Complete;
}
}
