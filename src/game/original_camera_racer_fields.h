#pragma once
#include "vehicle_base_tail50x.h"
#include "vehicle_frame49w.h"
#include "reset_timers50aw.h"

namespace rr::game {
//Actual common-racer owners, borrowed inside that racer's real bank lease.
//1AEEA0 walks actual manager44 order and performs this store for every racer.
//Neither class identity nor input/control state is inferred or changed.
inline void setOriginalCameraRacerIntroActive(BaseTailState50X& actual) noexcept {
    actual.activeAE8=1u;
}
//1AEEE0: AE8=0,844=0, then2179B0. Preserve every other existing tail/frame
//field, including live history, AI state, damage, timers outsideAD4..AE4.
//Call once in actual manager order, inside the original intro-exit stage.
inline void resetOriginalCameraRacerStartFields(BaseTailState50X& actual,
    VehicleFrameFields49W& fields) noexcept {
    actual.activeAE8=0u;fields.vehicle844=0u;
    resetBaseTimers50AW(actual);
}
}
