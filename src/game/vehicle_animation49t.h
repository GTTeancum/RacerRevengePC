#pragma once
#include "engine_animation49q.h"
#include "model_motion49t.h"
#include "chariot_ik49t.h"

namespace rr::game {
struct VehicleAnimationState49T {
    AnimationFilter49Q leftB00, rightB08, poseB18, blendB10;
    float leftB04, rightB0C, poseB1C, blendB14;
    ModelMotionState49T chariotModel, leftModel, rightModel;
};
struct VehicleAnimationShared49T {
    std::uint8_t enableGuard3D60F4;
    std::uint32_t enable434748;
    ModelMotionSharedState49T motion;
    ChariotIkShared49T ik;
    SourceLibmState49T libm;
};
enum class VehicleAnimationPhase49T : std::uint32_t {
    unspecified, beforeAnimation21A000, laterPhysics
};
struct VehicleAnimationInput49T {
    VehicleAnimationPhase49T phase;
    // Actual reloaded application+23C at2287B8, after controls, before21C120.
    float animationDelta23C, aeroE4, aeroE8;
    std::uint8_t chariot191, chariot1B0, left191, right191;
    bool body340Present, midRecordsAbsent;
    // Current body+C0 linear acceleration, not velocity or a later substep.
    VisualVector49R bodyAccelerationC0;
    // Actual filtered steering+B6C consumed only by present Mid records.
    float steeringB6C=0;
};
struct VehicleAnimationBindings49T {
    SceneNodeIndex49P chariot1B4 = SceneNodeIndex49P::none;
    SceneNodeIndex49P left1BC = SceneNodeIndex49P::none;
    SceneNodeIndex49P right1B8 = SceneNodeIndex49P::none;
    SceneNodeIndex49P bodyScene288 = SceneNodeIndex49P::none;
    const assets::MotionNodeBindings49O* chariotRows = nullptr;
    const assets::MotionNodeBindings49O* leftRows = nullptr;
    const assets::MotionNodeBindings49O* rightRows = nullptr;
    const assets::MotionView49O* brakeLeft28C = nullptr;
    const assets::MotionView49O* brakeRight290 = nullptr;
    const assets::MotionView49O* forward280 = nullptr;
    const assets::MotionView49O* back284 = nullptr;
    const assets::MotionView49O* left2A0 = nullptr;
    const assets::MotionView49O* right29C = nullptr;
    ChariotIkBindings49T hands;
    const ChariotIkPair49T* handRecords = nullptr;
    const assets::MotionView49O* mid288 = nullptr;
    const assets::MotionView49O* rightMid294 = nullptr;
    const assets::MotionView49O* leftMid298 = nullptr;
};
struct VehicleAnimationWorkspace49T {
    SceneNode49P nodes[maxSceneNodes49P];
    VehicleAnimationState49T state;
    VehicleAnimationShared49T shared;
    VisualShared49R visual;
    ModelMotionWorkspace49T motion;
    ChariotIkWorkspace49T ik;
};
enum class VehicleAnimationError49T : std::uint32_t {
    none, context, rounding_mode, graph, records, bindings, input, arithmetic,
    motion, ik
};
struct VehicleAnimationDiagnostic49T {
    VehicleAnimationError49T error;
    std::uint32_t sourceStage;
    ModelMotionDiagnostic49T motion;
    ChariotIkDiagnostic49T ik;
};
// Numeric successful-record initialization2134C4..2135CC. Values forB18/B10
// come from B04/B0C, not B1C/B14. Preserves supplied B1C/B14/model18 records.
// No vehicle construction, allocation, lifecycle or reset is performed.
bool initializeVehicleAnimationFilters49T(float seedB04, float seedB0C,
    VehicleAnimationState49T&) noexcept;

// Complete admitted original21A000 internal-animation call through real return.
// Resolved per-model MCF motions use their actual49O views/bindings. Nullable
// Mid records follow21A218/21A388/21A3F8 and consume filtered steering+B6C.
// midRecordsAbsent must agree with all three supplied Mid pointers.
// Presence pointers and gate bytes retain source skips.
// The caller attests selected resource identity/no foreign matching registry
// records, valid persistent source-initialized IK coefficients and source body
// binding. Model roots are distinct unparented roots with rows confined to each
// original subtree. All other current scene state is supplied, not inferred.
//
// Order: lazy enable; B00/B08; Chariot brakes; left/right engines; body inverse
// direction/B18/B10/Forward+Back; left/right IK. No filter runs twice. Gate1B0
// skips only pair/IK. Missing pair/body still reaches target-gated IK. Body
// inverse direction performs exact28BBB0 lazy graph refresh and VU ordering.
// Model18, lazy sampler/IK words and source libm errno remain persistent.
// visual is the SAME caller VisualShared49R passed to later21C120/49R, including
//4433D8. No hidden reset, noise, root21A660, physics, palette or rendering call.
//
// Requires current complete coherent graph1..maxSceneNodes49P with finite local/world words,
// FE_TONEAREST, finite consumed arithmetic, nonnegative finite frame factor,
// positive used filter times and subordinate sampler/IK numeric admissions.
// A phase tag attests a supplied boundary; it does not execute prior controls.
// Distinct live graph/state/shared/visual/workspace/diagnostic and immutable
// descriptors/assets/IK records are caller preconditions. Fixed scratch span
// checks are defensive, not pointer-lifetime or ownership validation. Serial use.
// False preserves graph/state/shared/visual; scratch may change. No allocation,
// I/O, guest execution, generic callback or environment access in production.
bool advanceVehicleAnimation49T(const VehicleAnimationInput49T&,
    const VehicleAnimationBindings49T&, const SceneGraph49P&,
    VehicleAnimationState49T&, VehicleAnimationShared49T&, VisualShared49R&,
    VehicleAnimationWorkspace49T&, VehicleAnimationDiagnostic49T&) noexcept;

bool vehicleAnimationInputFromBindings49T(const ControlModelBindings48W&,
    const VehicleAnimationInput49T&, VehicleAnimationInput49T&) noexcept;
}
