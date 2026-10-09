#pragma once
#include "pod_physics49n.h"
#include "../game/chariot_root49u.h"
#include "../game/pod_model_geometry49o.h"

namespace rr::assets {
enum class ChariotRootAuthoredKey49U : std::uint32_t {
    CableForce, CableRadiusForce, ChariotImpactForce, ChariotHeadOnImpactForce,
    ChariotRepulsorImpactForce, ChariotLandingImpulseScale, ChariotMaxImpulse,
    ChariotMaxRepulsorImpulse, ChariotVelocityRateForce, ChariotVelocityForce,
    ChariotPowerslideVelocityForce, ChariotVelocityDamping,
    ChariotAngularRateDamping, ChariotMaxYawRate, ChariotMaxYawAngle,
    ChariotMaxPowerslideYawAngle, MinChariotTurbulenceScale,
    MaxChariotTurbulenceScale, MinChariotTurbulenceTimeScale,
    MaxChariotTurbulenceTimeScale, ChariotOrientationScale, Count
};
constexpr std::size_t chariotRootAuthoredKeyCount49U =
    static_cast<std::size_t>(ChariotRootAuthoredKey49U::Count);
struct ChariotRootAuthoredValues49U { float values[chariotRootAuthoredKeyCount49U]; };
struct ChariotRootAuthoredProvenance49U { PodPhysicsToken49N tokens[chariotRootAuthoredKeyCount49U]; };
enum class ChariotRootAuthoringError49U : std::uint32_t {
    none, text, missing_key, rounding_mode, nonfinite, geometry, arithmetic
};
struct ChariotRootAuthoringDiagnostic49U {
    ChariotRootAuthoringError49U error;
    PodPhysicsDiagnostic49N text;
    ChariotRootAuthoredKey49U key;
};
// Exactly21 original [Pod Animation] Chariot/Cable keys. Missing keys reject;
// no default substitution. ChariotOrientationGain is unconsumed by the original
// loader and is not mapped toA94. Same bounded ASCII lexical/duplicate policy
// as49R and independently proved49N numeric conversion. Caller-owned bytes and
// fixed output/provenance must not overlap. False preserves both outputs.
bool readChariotRootAuthoring49U(const char*, std::size_t,
    ChariotRootAuthoredValues49U&, ChariotRootAuthoringDiagnostic49U&,
    ChariotRootAuthoredProvenance49U* = nullptr) noexcept;

// Explicit fresh binding. The caller supplies a successful authentic49O fresh
// geometry result and49R effective visual parameters. Copies49O length854 and
// already-converted49R A54; does not recompute either from guessed geometry.
// Original2158AC/C8/D4 separately multiply yaw fieldsA88/A8C/A90 by3C8EFA34.
// A94 comes from original constructorC3960000, not authoredOrientationGain.
// Optional output exposes convertedA90, which21A660 never consumes. All input
// scalars are finite;854 is nonnegative. Strict binary32 FE_TONEAREST. False
// preserves output and optionalA90. All inputs/outputs/diagnostic are disjoint.
bool bindFreshChariotRootParameters49U(const ChariotRootAuthoredValues49U&,
    const rr::game::FreshPodGeometryResult49O&,
    const rr::game::VehicleVisualParameters49R&,
    rr::game::ChariotRootParameters49U&, ChariotRootAuthoringDiagnostic49U&,
    float* convertedUnconsumedA90 = nullptr) noexcept;

// Constructor provenance only, for NEW caller-owned records.85C/864=+0;
// response870=(-0,-0,-1,1),880/890=(0,0,0,1),8A0=(1,0,0,0);838/83C/840=1/0/1.
// phase864 is the SAME vehicle clock supplied to49M; never a duplicate clock.
// Not a live-history repair/reset or scene/body initializer. Scene198 remains
// supplied until the complete original physical-to-virtual transition copies
// the original matrices and seeds its current response records.
void initializeChariotRootConstructorRecords49U(rr::game::ChariotRootState49U&,
    float& phase864) noexcept;
// Process-cold Chariot lazy records plus post-static-initializer434650up.
// Does not initialize49M noise/RNG or duplicate49R's shared434630zero vector.
void initializeChariotRootSharedColdRecords49U(rr::game::ChariotRootShared49U&) noexcept;
}
