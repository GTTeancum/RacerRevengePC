#pragma once
#include "original_ma_shield_uv.h"
namespace rr::game {
// Same renderer row shape, distinct MC controller/state and authenticated rate.
using OriginalMcShieldUvRows=OriginalMaShieldUvRows;
struct OriginalMcShieldUv {
 OriginalMcShieldUvRows matrix{};
 std::uint32_t frame60=0,controllerTimestamp8=0xffffffffu;
};
inline bool validOriginalMcShieldUv(const OriginalMcShieldUv& state)noexcept{
 const auto& m=state.matrix.rows;
 return m[0][0]==1.f&&m[0][1]==0.f&&m[0][2]==0.f&&m[0][3]==0.f&&
  m[1][0]==0.f&&m[1][1]==1.f&&m[1][2]==0.f&&m[1][3]==0.f&&
  std::isfinite(m[2][0])&&m[2][0]>=-5.f&&m[2][0]<=0.f&&
  m[2][1]==0.f&&m[2][2]==1.f&&m[2][3]==0.f;
}
inline OriginalMcShieldUv cloneOriginalMcShieldUv(const OriginalMcShieldUv& prototype)noexcept{
 auto clone=prototype;clone.controllerTimestamp8=0xffffffffu;return clone;
}
// Authenticated MC_SHIELD.PSM: identity basis, U=.05, V=0, omitted scale=1,
// center/angle/shear=0, frames=1, looping/reverse/paused=0. Source 29A68C..6B0
// constructs U translation as basis[2][0]-u_velocity; 29A978..9D8 updates rows.
// This is the existing declared binary32 native policy, not R5900/VU parity.
inline bool serviceOriginalMcShieldUv(OriginalMcShieldUv& state,std::uint32_t timestamp,
 bool paused,bool callbackAllowsUpdate=true)noexcept{
 static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559);
 if(std::fegetround()!=FE_TONEAREST||!validOriginalMcShieldUv(state))return false;
 if(timestamp==state.controllerTimestamp8||paused)return true;
 auto next=state;
 if(callbackAllowsUpdate){
  if(!next.frame60)next.matrix=OriginalMcShieldUvRows{};
  else{
   volatile float translated=next.matrix.rows[2][0]-0x1.99999ap-5f;
   float u=translated;
   //29A9DC..29AA18: strict abs(U)>5, truncation and subtraction.
   if(std::fabs(u)>5.f){volatile float integral=static_cast<float>(static_cast<std::int32_t>(u));volatile float reduced=u-integral;u=reduced;}
   next.matrix.rows[2][0]=u;
  }
  ++next.frame60;
 }
 //29AD48..58 stamps even when the installed callback rejects service.
 next.controllerTimestamp8=timestamp;
 if(!validOriginalMcShieldUv(next))return false;state=next;return true;
}
}
