#include "reset_probe50ag.h"
#include <bit>
#include <cmath>
namespace rr::game { namespace {
float f(std::uint32_t x) noexcept{return std::bit_cast<float>(x);}
std::uint32_t u(float x) noexcept{return std::bit_cast<std::uint32_t>(x);}
float add(float a,float b) noexcept{volatile float x=a+b;return x;}
float mul(float a,float b) noexcept{volatile float x=a*b;return x;}
bool valid(const ResetProbe50AG&s) noexcept{
 if(s.phase!=ResetProbePhase50AG::AwaitingQuery||s.attempt>=20||!std::isfinite(f(s.lift)))return false;
 for(unsigned i=0;i<3;++i)if(!std::isfinite(f(s.staged[i]))||!std::isfinite(f(s.start[i]))||!std::isfinite(f(s.end[i])))return false;
 return true;
}
}
bool beginResetProbe50AG(const std::uint32_t p[4],std::uint32_t lift,ResetProbe50AG&out) noexcept{
 if(!p||!std::isfinite(f(lift)))return false;
 ResetProbe50AG s{};s.lift=lift;
 for(unsigned i=0;i<4;++i){if(i<3&&!std::isfinite(f(p[i])))return false;s.staged[i]=s.start[i]=s.end[i]=p[i];}
 s.end[1]=0;out=s;return true;
}
bool acceptResetProbe50AG(ResetProbe50AG&out,bool hit,std::uint32_t hitY) noexcept{
 if(!valid(out)||(hit&&!std::isfinite(f(hitY))))return false;
 auto s=out;
 if(hit||s.attempt==19){
  const float y=add(f(hit?hitY:s.staged[1]),f(s.lift));if(!std::isfinite(y))return false;
  s.staged[1]=u(y);s.phase=hit?ResetProbePhase50AG::Hit:ResetProbePhase50AG::Exhausted;
 }else{
  ++s.attempt;for(unsigned i=0;i<4;++i)s.start[i]=s.end[i]=s.staged[i];
  const float y=add(f(s.staged[1]),mul(5.f,static_cast<float>(s.attempt)));if(!std::isfinite(y))return false;
  s.start[1]=u(y);
 }
 out=s;return true;
}
}
