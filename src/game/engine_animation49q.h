#pragma once
#include "model_motion49q.h"
#include "control_binding48w.h"
#include <cstdint>

namespace rr::game {
struct AnimationFilter49Q {
    float value0, time4, alpha8;
};
struct EngineAnimationState49Q {
    // Shared lazy animation-enable global, not a new per-engine policy.
    std::uint8_t enableGuard8784;
    std::uint32_t enable4748;
    AnimationFilter49Q leftB00, rightB08;
    // Source mirrors are separately clamped; filter.value0 stays unclamped.
    float leftB04, rightB0C;
    ModelMotionState49Q leftModel, rightModel;
};
enum class EngineAnimationPhase49Q : std::uint32_t {
    unspecified, baseAnimation21A000, laterPhysics
};
struct EngineAnimationInput49Q {
    EngineAnimationPhase49Q phase;
    // Reloaded global application+23C at2287B8. Not the physics substep dt.
    float animationDelta23C;
    float aeroE4, aeroE8;
    // Earlier Chariot brake gates affect B04/B0C even if an engine is hidden.
    bool chariotModelPresent;
    std::uint8_t chariotFlags191;
    bool brakeLeftMotionPresent, brakeRightMotionPresent;
    // Required source-selection boundary: actual Pod01 MCF lookup has neither
    // enginerightmid nor engineleftmid. A foreign matching record is excluded.
    bool engineMidRecordsAbsent;
};
struct EngineMotionModel49Q {
    const assets::MotionView49O* motion;
    const assets::MotionNodeBindings49O* bindings;
    SceneNodeIndex49P modelRoot;
    std::uint8_t sceneFlags191;
};
struct EngineAnimationModels49Q {
    EngineMotionModel49Q left, right;
    SceneNodeIndex49P chariotModelRoot;
};
struct EngineAnimationWorkspace49Q {
    SceneNode49P nodes[maxSceneNodes49P];
    ModelMotionWorkspace49Q motion;
};
enum class EngineAnimationError49Q : std::uint32_t {
    none, context, rounding_mode, graph, input, arithmetic, binding, motion, records
};
struct EngineAnimationDiagnostic49Q {
    EngineAnimationError49Q error;
    std::uint32_t engine; // 0=left,1=right when a model operation fails.
    ModelMotionDiagnostic49Q motion;
};

// Numeric initialization from2134A0..213538: preserve supplied B04/B0C seeds,
// set filter values to those seeds, time4=0x3E4CCCCD and alpha8=1. All other
// state, including shared enable and model18, survives unchanged. This does
// not construct a vehicle, initialize the other two Chariot filters, or claim
// that an already-running vehicle has the constructor's zero-valued seeds.
bool initializeEngineAnimationFilters49Q(float seedB04, float seedB0C,
    EngineAnimationState49Q& state) noexcept;

// Complete engine-node projection of21A000 for selected non-mirrored Pod01
// engine motions with absent engine-Mid records. Original order: lazy enable;
// left/right filters; earlier Chariot brake mirror clamps when gated; left
// engine clamp+1559F0; right engine clamp+1559F0. Remaining21A000 work targets
// the separate Chariot hierarchy and cannot change these engine locals under
// the admitted distinct-root binding. No Chariot matrix/IK effects are claimed.
//
// A null motion or modelRoot==none skips that engine, as does clear bit0 of
// sceneFlags191. Active models require distinct unparented roots and binding
// rows confined to their respective subtrees. Bindings contain explicit graph
// node indices, not guessed PSG-row or hierarchy-base arithmetic. Model18 and
// all scene local/world/flag state follow the single-motion source helper.
//
// Caller supplies current scene state, immutable asset views, enable/filter
// history and separate model18 values. The phase is an admission assertion,
// not a scheduler. Engine root placement21C120, Chariot animation/IK, bounds,
// rendering, observer callbacks and shared later noise/RNG are not executed.
// Positive filter times, nonnegative finite dt and finite consumed arithmetic
// are native admission. FE_TONEAREST, strict separate binary32 operations.
// Graph, state, workspace, diagnostic and immutable inputs are distinct live
// caller-owned records. Defensive checks are partial: graph/state/workspace
// spans are checked against each other; workspace is also checked against the
// input/model descriptors and active immutable motion/binding data. Diagnostic
// is assigned before those checks; graph/state versus immutable inputs remains
// a caller precondition. Checks do not establish pointer lifetime or validate
// arbitrary caller-forged asset views. Workspace may
// change on false; graph/state remain unchanged. No allocation or I/O.
bool advanceEngineAnimation49Q(const EngineAnimationInput49Q&,
    const EngineAnimationModels49Q&, const SceneGraph49P&,
    EngineAnimationState49Q&, EngineAnimationWorkspace49Q&,
    EngineAnimationDiagnostic49Q&) noexcept;

// Exact existing control-output association: aeroDrive[0]/[1] are the values
// written to aero+E4/+E8 by48W. Other context fields are preserved. This does
// not change original control/animation sequencing or choose animation dt.
bool engineAnimationInputFromBindings49Q(const ControlModelBindings48W&,
    const EngineAnimationInput49Q& context, EngineAnimationInput49Q& out) noexcept;
}
