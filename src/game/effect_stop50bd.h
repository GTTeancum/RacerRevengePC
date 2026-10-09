#pragma once
#include "effect_control50bc.h"
namespace rr::game {
enum class EffectStopResult50BD {Stopped,Deferred,UnsupportedDuration};
// Concrete15E800-created owner (table3CDA70), original15F070 override.
// Duration is consumed only for mode1/enabled. Other inputs remain opaque.
// Consumed nonfinite/subnormal durations reject unchanged pending EE proof.
inline EffectStopResult50BD stopDeferredEffect50BD(EffectControlState50BC&s,
 std::uint32_t mode1D8,std::uint32_t duration1E4) noexcept {
 if(mode1D8==1&&(s.flags1D4&2u)){
  const auto magnitude=duration1E4&0x7fffffffu;const auto exponent=magnitude&0x7f800000u;
  if(exponent==0x7f800000u||(!exponent&&magnitude))return EffectStopResult50BD::UnsupportedDuration;
  if(!(duration1E4&0x80000000u)&&magnitude)return EffectStopResult50BD::Deferred;
 }
 stopEffect50BC(s);return EffectStopResult50BD::Stopped;
}
}
