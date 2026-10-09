#include "engine_tail50t.h"
#include <cstring>
#include <cfenv>
namespace rr::game {
namespace {
float fp(std::uint32_t b)noexcept{float f;std::memcpy(&f,&b,4);return f;}
std::uint32_t bits(float f)noexcept{std::uint32_t b;std::memcpy(&b,&f,4);return b;}
std::int32_t signedWord(std::uint32_t b)noexcept{std::int32_t n;std::memcpy(&n,&b,4);return n;}
struct Math{bool okay=true;
 float read(std::uint32_t b)noexcept{const auto e=b&0x7f800000u;if(e==0x7f800000u||(!e&&(b&0x7fffffu))){okay=false;return 0;}return fp(b);}
 std::uint32_t sub(std::uint32_t a,std::uint32_t b)noexcept{volatile float v=read(a)-read(b);auto out=bits(v);read(out);return out;}
 std::uint32_t mul(float a,float b)noexcept{volatile float v=a*b;auto out=bits(v);read(out);return out;}
};
}
EngineTailResult50T advanceEngineTail50T(const EngineTailState50T&previous,const EngineTailInput50T&in)noexcept{
 EngineTailResult50T out{};out.state=previous;
 if(!(in.flagsB9C&16u)){out.status=EngineTailStatus50T::NeedsFlameInitialization;return out;}
 if(std::fegetround()!=FE_TONEAREST){out.status=EngineTailStatus50T::Arithmetic;return out;}
 auto&s=out.state;Math m;
 auto thrust=[&](unsigned side,std::uint32_t raw){for(unsigned axis=0;axis<3;++axis)s.thrustA0[side][axis]=raw;s.thrustA0[side][3]=0x3f800000u;out.thrustWriteMask|=static_cast<std::uint8_t>(1u<<side);};
 if(s.activeAE8){
  bool complete=true,both;
  if(!(m.read(s.timerAE4)<=0.f)){s.timerAE4=m.sub(s.timerAE4,in.frameFactorBits);both=true;}
  else{both=signedWord(s.countAD4[0])>=4&&signedWord(s.countAD4[1])>=4;if(both){s.timerAE4=0x3faccccdu;s.field844=0;}}
  for(unsigned side=0;side<2;++side){auto&flag=side?s.flagsACC:s.flagsAD0;
   if(both){flag=1;if(!(m.read(s.timerAE4)<0.f)){thrust(side,0x45bb8000u);complete=false;}}
   else if(signedWord(s.countAD4[side])<5){
    complete=false;s.sharedRandomState=s.sharedRandomState*0x41c64e6du+0x3039u;++out.randomDraws;
    volatile float converted=static_cast<float>(s.sharedRandomState&0x7fffffffu);const auto draw=m.mul(fp(0x30000000u),converted);
    if(m.read(draw)<fp(0x3d99999au)&&m.read(s.timerADC[side])<=0.f){flag=0;++s.countAD4[side];volatile float magnitude=static_cast<float>(signedWord(s.countAD4[side]*2000u));thrust(side,bits(magnitude));volatile float count=static_cast<float>(signedWord(s.countAD4[side]));s.timerADC[side]=m.mul(.25f,count);}
    else if(!(m.read(s.timerADC[side])<=0.f))s.timerADC[side]=m.sub(s.timerADC[side],in.frameFactorBits);
   }
  }
  if(complete){s.activeAE8=0;s.flagsAD0=s.flagsACC=1;if(in.probe128Present){out.status=EngineTailStatus50T::NeedsProbeReset;out.state=previous;out.thrustWriteMask=out.randomDraws=0;return out;}out.nullProbeResetExecuted=true;}
 }else{
  if(((in.application58&4u)||s.countAD4[1]||s.countAD4[0])&&!in.state40)s.flagsAD0=s.flagsACC=1;
  else{thrust(1,0);thrust(0,0);s.flagsAD0=s.flagsACC=0;}
 }
 if(!m.okay){out={};out.state=previous;out.status=EngineTailStatus50T::Arithmetic;}
 return out;
}
}
