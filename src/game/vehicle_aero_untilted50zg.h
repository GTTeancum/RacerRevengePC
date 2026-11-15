#pragma once
// PC ENHANCEMENT, NOT ORIGINAL BEHAVIOUR (native_zero_gravity50zg.h).
// Declaration only, so vehicle_aero48e.h (474 translation units) is untouched.
#include "vehicle_aero48e.h"

namespace rr::game {
// applyAerodynamics48E with the world-up "tilted" branch (up.y <= 0.5:
// InvertedSideSlipSideForce, TiltedSteering, TiltedYaw) never taken. Used only
// while a racer is in zero-g flight, where the N64 also skips its own tilt
// penalty (0x8006B6E4) and there is no world "up" to be tilted against.
// Same arithmetic, order and domain as applyAerodynamics48E otherwise.
bool applyAerodynamicsUntilted50ZG(const AeroState48E&,const AeroParameters48E&,
    const AeroInput48E&,const ReadyBodyBasis48D&,const BodyLoads48C&,
    float elapsedFactor,AeroResult48E& out) noexcept;
}
