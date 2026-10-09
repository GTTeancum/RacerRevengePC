#pragma once
#include "body_scene48g.h"
#include <cstdint>

namespace rr::game {
struct Schedule49K { std::uint32_t count; float factorPerSubstep; };
// Original 1A8384..1A83B0: binary32 product, EE CVT.W.S truncation, +1,
// CVT.S.W, then division. factor in [0,bits 3D75C28F], scale >= 0;
// only resulting counts 1..4 are admitted. No equal-substep approximation.
bool selectSchedule49K(float preparedFactor, float liveScale, Schedule49K&) noexcept;

struct GravitySelection49K {
    std::uint32_t incoming348_104;
    float brake350_EC, cachedSpeed340_34, sceneUpY;
    float vehicle378, vehicle37C;
    float effective434730, effective434738; // Initializer bits 43C13334,3F34FDF4.
};
// Run before CURRENT +348 preparation/response, using incoming +104 and the
// frame-phase cached +34. Globals are effective values after lazy initialization.
bool selectGravity49K(const GravitySelection49K&, float& gravity344_38) noexcept;
struct EngineFlags49K { std::uint32_t engine34, engine38; };
EngineFlags49K selectEngineFlags49K(std::uint32_t vehicle64, std::uint8_t vehicleB9C) noexcept;

// Only fields written on the admitted disabled-probe branch are represented.
// Other component fields remain caller-owned and unchanged by these leaves.
struct DisabledProbeState49K {
    std::uint32_t field104, field108, field10C, field110, field114;
    std::uint32_t field118, field11C, field12C, field1E8;
    float point150[4], vector1D0[4];
};
// Exact 1B1800 branch: linked body and ready root scene present, +120/+124 null,
// +100 zero. This is a disabled probe, NOT a no-hit result from an enabled probe.
// It clears 108/110/114/118/1E8/12C, copies effective434260 to 1D0, and transforms
// local140 into point150 (XYZ then W=1). No world query or callback is invented.
bool prepareDisabledProbe49K(const DisabledProbeState49K&, const float (&local140)[4],
    const BodyMatrix48G&, const float (&effective434260)[4], DisabledProbeState49K&) noexcept;
struct DisabledProbeResponse49K { DisabledProbeState49K state; Vector3_48C torque190; };
// Requires preparation's 110=114=0, same linked body/scene and disabled config.
// Effective4342C8 has original initialized bits 3E99999A. Response clears104,
// clears stale11C, sets10C=1, then adds cached E8*(global4342C8*component204)
// times global434290.xyz to body190.xyz in original scalar operation order.
bool respondDisabledProbe49K(const DisabledProbeState49K&, float cachedAngleE8,
    float component204, float effective4342C8, const Vector3_48C& effective434290,
    const Vector3_48C& priorTorque190, DisabledProbeResponse49K&) noexcept;

// Original main-body +94 callback 165420/165424, distinct from 48H's +238 clear.
void clearMainBodyPost20C49K(std::uint32_t& body20C) noexcept;
// Original219EA4..219EBC, after219120 and before optional vehicle+3C override.
std::uint32_t selectVehicle38049K(std::uint32_t component354_44) noexcept;
struct LateFeedback49K { std::uint32_t engine3C, aeroE0, aeroD0, aeroD4; };
LateFeedback49K selectLateFeedback49K(std::uint32_t component348_104,
    std::uint32_t component354_3C, std::uint32_t component354_44) noexcept;
// Every consumed float and intermediate must be finite. Boolean-returning
// leaves require FE_TONEAREST and are transactional, bounded, allocation-free.
// Strict binary32, no FMA. Integer-only leaves do not depend on rounding mode.
}
