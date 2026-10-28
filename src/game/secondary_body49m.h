#pragma once
#include "body_wrapper48h.h"
#include "pod_sphere_constraint49l.h"
#include "gradient_noise49m.h"
#include <cstdint>

namespace rr::game {
enum class SecondaryPhase49M { Ready, ConstraintPending, Response368Pending };
// Canonical body state, not an independently mutable duplicate sphere body.
// Frame caches are refreshed once by167B80; 48H does not refresh them.
struct SecondaryBodyState49M {
    RootBodyState48H root;
    float cachedSpeed34;
    Vector3_48C cachedAnglesE0;
    std::uint32_t sceneFlags190, body20C;
    bool scenePresent, enabled;
    float velocity70W, angularVelocityB0W, force180W, torque190W;
    float time864; // vehicle+864, persistent only on the nonzero-state route
    SecondaryPhase49M phase;
};
struct SecondaryBodyParameters49M {
    BodyParameters48C body;
    Vector3_48C centerOffset;
    float centerOffsetW;
    // Original inverse-inertia matrix lanes3,7,11,12,13,14,15. Its3x3
    // entries come exclusively from body.inverseInertiaColumns.
    float inverseInertiaExtra[7];
};
struct SecondaryForceContext49M {
    std::uint32_t vehicleState40, probe104;
    float gravity378, gravity37C;
    PodSphereVector49L targetRelative2F0;
    PodSphereMatrix49L mainSceneWorld100;
    std::uint32_t mainSceneFlags190;
    float mainCachedSpeed34;
    float effective4346C8, effective4346D0, effective4346D8;
    float effective4346E0, effective4346E8, effective4346F0;
    float effective4346F8, effective434700, effective434708;
};
// Separately identified source-initialized values, not inferred live defaults.
void initializedSecondaryGlobals49M(SecondaryForceContext49M&) noexcept;
bool refreshSecondaryFrameCache49M(const SecondaryBodyState49M&,
                                   SecondaryBodyState49M&) noexcept;
// Copies canonical fields into the original49L full-lane view. No arithmetic.
void makeSecondarySphereBody49M(const SecondaryBodyState49M&,
    const SecondaryBodyParameters49M&, PodSphereBody49L&) noexcept;
//Pure218B28 speed-conditioned angular damping scratch only. No body,
//source-global, phase or constraint completion is supplied by this leaf.
bool prepareSecondaryDampingPrefix49M(float cachedSpeed,float actualLow,
    float actualHigh,const Vector3_48C& angularVelocity,Vector3_48C& torque) noexcept;
// 218A3C..2190A0: gravity, speed-conditioned angular damping, restoration;
// state0 damping/alignment, or original gradient-noise route. Ready scenes only.
// Noise/RNG are untouched in state0 and are atomically committed otherwise.
bool produceSecondaryForces49M(const SecondaryBodyState49M&,
    const SecondaryBodyParameters49M&, const SecondaryForceContext49M&,
    float elapsedFactor, GradientNoiseState49M&, std::uint32_t& sharedRandomState,
    SecondaryBodyState49M&) noexcept;
// 2190AC,2190C0,2190D4:49L prepare/correct/apply in that order. Main is read-only
// and must be derived at this exact stage from the canonical main state.
bool applySecondaryConstraint49M(const SecondaryBodyState49M&,
    const SecondaryBodyParameters49M&, const PodSphereBody49L& main,
    const PodSphereJoint49L&, SecondaryBodyState49M&, PodSphereJoint49L&,
    float& correctionMagnitude) noexcept;
// Supplied post368 state -> original secondary48H2190FC. The composition owns
// the complete external post368 main/secondary/joint snapshot. This leaf clears
// callback238 through48H; it does NOT invoke the main-only20C callback.
bool completeSecondaryResponse49M(const SecondaryBodyState49M& suppliedPost368,
    const SecondaryBodyParameters49M&, float elapsedFactor,
    SecondaryBodyState49M&) noexcept;
// FE_TONEAREST; finite consumed values and intermediates; fixed storage; no
// original query/contact/child graph execution. All bool failures preserve
// represented outputs, noise and RNG. State/output may alias. Distinct joint,
// body, noise, RNG and magnitude storage is the caller's ordinary API contract.
}
