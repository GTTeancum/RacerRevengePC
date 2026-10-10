#pragma once
#include "scene_graph49p.h"
#include "coupled_body_advancement49m.h"
#include <cstdint>

namespace rr::game {
struct VisualVector49R { float v[4]; };
struct VisualResponse49R {
    VisualVector49R position, linearVelocity, angularVelocity;
    Quaternion48C orientation; // Scalar-first w,x,y,z, all four words.
};
struct VehicleVisualState49R {
    VisualVector49R upC20, publishedUpAF0;
    VisualResponse49R right, left;
    float phase8B0, phase900, phase950, phase9A0;
};
struct VisualLazyWord49R { std::uint8_t guard; std::uint32_t word; };
struct VisualShared49R {
    // Ascending source addresses434760,768,770,778,780,788,790,798,7A0.
    // Preserve warm effective words, including unconsumed store-only values.
    VisualLazyWord49R globals[9];
    VisualLazyWord49R orientationSentinel4433D8;
    VisualVector49R zero434630;
};
struct VehicleVisualParameters49R {
    float impact9F0, groundImpact9F4, separation9F8, thrust9FC;
    float liftA00, dragA04, springA08, dampingA0C, binderSpringA10;
    float yawA14, slideYawA18, slidePitchA1C, pitchA20, rollA24;
    float angleReturnA28, angularDampingA2C, binderTorqueA30;
    float minTimeA34, maxTimeA38, minAmplitudeA44, maxAmplitudeA48;
    float effectiveSpeedSquaredA54, damageTranslationA58, damageOrientationA5C;
    float orientationScaleA64, effectiveInputSpeedA68, verticalLimit7EC;
    VisualVector49R rightBase2C0, leftBase2B0;
};
enum class VehicleVisualPhase49R : std::uint32_t {
    unspecified, afterAnimationBeforeChariotAndPhysics, laterPhysics
};
struct VehicleVisualInput49R {
    VehicleVisualPhase49R phase;
    std::uint32_t vehicle834, vehicle844, vehicle40;
    std::uint8_t applicationByte58;
    std::uint32_t probe108, aeroD8, aeroDC, vehicleCFC, vehicleD00;
    float frameFactor, cachedBodySpeed34, rawB5C, objectD34_10;
    float engine68, engine6C, engine70, engine74, aeroE4, aeroE8;
    float damage7D0, damage7D8, damage7D4, damage7DC;
    VisualVector49R probeVector1D0, impactBA0;
};
struct VisualSceneBindings49R {
    SceneNodeIndex49P main190, container194, right1B8, left1BC;
    bool extra1C0Present, extra1C4Present;
    std::uint32_t rotatingNodeCountB54;
};
struct VehicleVisualWorkspace49R {
    SceneNode49P nodes[maxSceneNodes49P];
    VehicleVisualState49R state;
    VisualShared49R shared;
    CoupledNoiseContext49M noise;
};
enum class VehicleVisualError49R : std::uint32_t {
    none, context, rounding_mode, graph, records, input, arithmetic, noise
};
struct VehicleVisualDiagnostic49R {
    VehicleVisualError49R error;
    std::uint32_t sourceStage;
};

// Selected original21C120 normal834==0 call through its real return. Requires
// distinct parentless main/right/left roots, container194.parent==main190,
// both primary engines present, no optional roots, and B54==0. This is an
// explicit native admission, not a discovery of scene/resource identities.
// Runs after21A000 node animation and before21A660/later physical substeps.
// Produces container local/world/cache state, C20/AF0, all four clocks, both
// primary response records and actual engine-root LOCAL matrices. Root worlds
// stay lazy. Application/body/component/probe/impact values are stable supplied
// phase inputs; no contact/query/body update or Chariot IK is executed.
//
// Shared lazy graphics globals and the existing49M noise/RNG object remain
// caller-owned and serially shared across consumers/vehicles. The noise flag
// and amplitudes do not authorize skipping source calls. This API does not
// widen49M's existing supplied PostBase or physical scene-domain contracts.
//
// FE_TONEAREST and strict separate binary32 arithmetic; nonnegative finite
// frame factor and finite consumed values/intermediates, with retained
// source-reference square-root behavior. VU vector normalization admits true
// zero but excludes positive subnormal squared norms; no fallback axis is used.
// Live graph/state/shared/noise/workspace/diagnostic and immutable inputs must
// be distinct. Defensive span checks cover mutable records and scratch versus
// input descriptors. Diagnostic is assigned first; mutable records versus
// immutable inputs remain a caller precondition, as does pointer lifetime.
// False preserves represented graph/state/shared/noise; workspace may change.
// Fixed storage, no
// allocation, callbacks, I/O, scheduling, model ownership or implicit reset.
bool updateVehicleVisual49R(const VehicleVisualInput49R&,
    const VehicleVisualParameters49R&, const VisualSceneBindings49R&,
    const SceneGraph49P&, VehicleVisualState49R&, VisualShared49R&,
    CoupledNoiseContext49M&, VehicleVisualWorkspace49R&,
    VehicleVisualDiagnostic49R&) noexcept;
}
