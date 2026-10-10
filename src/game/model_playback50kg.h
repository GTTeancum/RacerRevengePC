#pragma once
#include "model_motion49t.h"
#include "character_mode0_ik50kg.h"

namespace rr::game {
// Stable caller-owned descriptor. The MOT header supplies duration; looping is
// original resource B4 metadata, never inferred from a file name or MCF FPS.
struct ModelPlaybackMotion50KG {
    const assets::MotionView49O* motion = nullptr;
    bool looping = false;
};
struct ModelCapturedPose50KG {
    std::uint32_t count = 0;
    Quaternion48C rotation[assets::maxModelNodes49O]{};
    Vector3_48C position[assets::maxModelNodes49O]{};
};
struct ModelPlayback50KG {
    const ModelPlaybackMotion50KG* current18 = nullptr;
    const ModelPlaybackMotion50KG* previous1C = nullptr;
    bool stopped10 = false, blending30 = false;
    float time34 = 0, oldTime38 = 0, blendEnd3C = 0, blendStart40 = 0, blendDuration44 = 0;
    ModelCapturedPose50KG captured;
    std::uint32_t savedMatrixCount1F4 = 0;
    BodyMatrix48G savedMatrices1F4[assets::maxModelNodes49O]{};
};
enum class ModelPlaybackError50KG : std::uint32_t {
    none, input, topology, binding, arithmetic, sample, required_ik, ik
};
struct ModelPlaybackDiagnostic50KG {
    ModelPlaybackError50KG error = ModelPlaybackError50KG::none;
    std::uint32_t sourceStage = 0;
    ModelMotionDiagnostic49T sample{};
};
// Callback must implement the actual source mode0/1 chain for this ordered
// endpoint. It may modify only the staged graph and staged lazy records. Context is immutable
// prepared coefficients/identities; no externally visible side effects on false.
// A missing callback with enabled nonzero endpoints fails, never skips the solve.
using ModelPlaybackIkSolve50KG = bool (*)(const void* context,
    std::uint32_t chain, const Vector3_48C& worldTarget,
    ModelMode0IkShared50KG& stagedShared, const SceneGraph49P& stagedGraph) noexcept;
struct ModelPlaybackIk50KG {
    bool enabled204 = false;
    std::uint32_t count208 = 0;
    SceneNodeIndex49P endpoints20C[assets::maxModelNodes49O]{};
    std::uint32_t restoreCount214 = 0;
    std::uint8_t restoreRows218[assets::maxModelNodes49O]{};
    ModelPlaybackIkSolve50KG solve = nullptr;
    const void* context = nullptr;
};
struct ModelPlaybackWorkspace50KG {
    SceneNode49P nodes[maxSceneNodes49P];
    ModelMotionWorkspace49T motion;
    Vector3_48C targets[assets::maxModelNodes49O];
};
// All views, descriptors, bindings, graph, workspace and mutable records remain
// disjoint and alive. Serial access, nearest/even, strict binary32 operations.
// No allocation or guest execution. False leaves graph/playback/shared intact.
// Narrow original route: mirror=0, mask=null, preservePhase=0. Snapshot capture
// uses the existing original matrix->quaternion owner, not matrix interpolation.
bool captureModelPose50KG(const assets::MotionNodeBindings49O&, const SceneGraph49P&,
    ModelCapturedPose50KG&, ModelPlaybackDiagnostic50KG&) noexcept;
//155730 captures original local matrices after the entry .001 update. Retained
// even though this narrow nonmirrored playback route does not read them.
bool captureModelPlaybackMatrices50KG(const assets::MotionNodeBindings49O&,
    const SceneGraph49P&, ModelPlayback50KG&, ModelPlaybackDiagnostic50KG&) noexcept;
bool startModelPlayback50KG(const ModelPlaybackMotion50KG&, ModelPlayback50KG&,
    ModelPlaybackDiagnostic50KG&) noexcept;
bool transitionModelPlayback50KG(const ModelPlaybackMotion50KG&, float blendDuration,
    const assets::MotionNodeBindings49O&, const SceneGraph49P&, ModelPlayback50KG&,
    ModelPlaybackDiagnostic50KG&) noexcept;
// 156620: ordered endpoints and sorted unique bound ancestor rows, stopping
// before the top-level root. Does not construct or invent IK coefficients.
// Caller passes source-discovered endpoints in l_ik/r_ik registration order.
bool bindModelPlaybackIk50KG(const SceneNodeIndex49P* endpoints, std::uint32_t count,
    const assets::MotionNodeBindings49O&, const SceneGraph49P&,
    ModelPlaybackIk50KG&, ModelPlaybackDiagnostic50KG&) noexcept;
bool advanceModelPlayback50KG(float dt, const assets::MotionNodeBindings49O&,
    const SceneGraph49P&, const ModelPlaybackIk50KG&, ModelPlayback50KG&,
    ModelMotionSharedState49T&, ModelMode0IkShared50KG&, ModelPlaybackWorkspace50KG&,
    ModelPlaybackDiagnostic50KG&) noexcept;
// Shared49T numerical owner for original154A40/154FB0. Exactly one of previous
// and captured is nonnull. New and previous times are independent direct MOT
// times. Rotations then positions retain authored track order and setters.
// Position blend is new + (old-new)*(1-weight), as154D84..154DE0, not49T's
// fractional pair arithmetic. Output graph/shared are transactional.
bool sampleModelMotionTransition50KG(const assets::MotionView49O& current,
    const assets::MotionView49O* previous, const ModelCapturedPose50KG* captured,
    const assets::MotionNodeBindings49O&, float currentTime, float previousTime,
    float weight, const SceneGraph49P&, ModelMotionSharedState49T&,
    ModelMotionWorkspace49T&, ModelMotionDiagnostic49T&) noexcept;
}
