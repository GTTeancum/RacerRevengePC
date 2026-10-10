#pragma once
#include "vehicle_base_tail50x.h"
namespace rr::game {
// Original2179B0, including return delay slot. No flags, callback owners,
// opaque W words, activeAE8 or other state are initialized by this leaf.
inline void resetBaseTimers50AW(BaseTailState50X& state) noexcept {
 state.countAD4[1]=0; state.countAD4[0]=0;
 state.timerADC[1]=0; state.timerADC[0]=0; state.timerAE4=0;
}
}
