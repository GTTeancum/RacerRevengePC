#pragma once
#include "scene_graph49p.h"
#include "vehicle_visual49r.h"
#include "source_acos49t.h"
#include <cstdint>

namespace rr::game {
struct ChariotIkVector49T { float v[4]; };
struct ChariotIkChainBinding49T {
    SceneNodeIndex49P shoulder, elbow, wrist, target;
};
struct ChariotIkBindings49T { ChariotIkChainBinding49T left, right; };
// Exactly the source-written28EB90 fields. No claim is made about unwritten
// source padding0C/44/48/4C, allocation, or the guest pointer representation.
struct ChariotIkChain49T {
    SceneNodeIndex49P shoulder0, elbow4, wrist8;
    std::uint32_t mode10;
    float lengthSquared14, inverseLengthProduct18;
    float projectedSquared1C, inverseProjectedProduct20;
    float restCosine24, restSine28, restAngle2C;
    ChariotIkVector49T savedElbowLocal30;
    std::uint32_t debug40;
    ChariotIkVector49T axis50;
};
struct ChariotIkPair49T { ChariotIkChain49T left, right; };
struct ChariotIkShared49T {
    VisualLazyWord49R sentinel443450, sentinel443458;
};
struct ChariotIkWorkspace49T {
    SceneNode49P nodes[maxSceneNodes49P];
    ChariotIkPair49T records;
    SourceLibmState49T libm;
    ChariotIkShared49T shared;
    VisualShared49R visual;
};
enum class ChariotIkError49T : std::uint32_t {
    none, rounding_mode, graph, bindings, records, context, arithmetic
};
struct ChariotIkDiagnostic49T {
    ChariotIkError49T error;
    std::uint32_t sourceStage, hand;
};
// Original28EB90 twice, at the supplied pre-motion initialization boundary.
// Coefficients depend on the complete original constructor scene; never derive
// them from a later animated pose. Graph lazy world-cache effects and source
// libm errno effects are represented. Left axis(+0,+1,+0,1), right(-0,-1,-0,1).
// Source mode1/default matherr/cold NaN record, FE_TONEAREST, finite consumed
// arithmetic and retained scalar EE roots/VU zero-or-normal norms required.
// All mutable records/workspace/diagnostic and immutable descriptors must be
// distinct; pointer lifetime and serial access remain caller preconditions.
// False leaves graph, records and libm untouched. Fixed caller memory, no I/O,
// allocation, lookup, lifecycle, ownership, profiling or implicit reset.
bool initializeChariotIk49T(const ChariotIkBindings49T&, const SceneGraph49P&,
    ChariotIkPair49T&, SourceLibmState49T&, ChariotIkWorkspace49T&,
    ChariotIkDiagnostic49T&) noexcept;
// Original left290A00 then right290A00: target position, fixed-axis solve,
// target world orientation, wrist parent-relative orientation assignment.
// Requires persistent source-initialized records with mode10=1/debug40=0.
// Records are immutable. target==none skips that hand; bothnone is a no-op
// without consuming coefficient/libm/lazy state. Complete graph/caches remain.
// Source libm mode1 domain results/errno are preserved, including acos>1.
// visual.orientationSentinel4433D8 is the SAME shared record used by49R;
// callers must pass the same VisualShared49R across animation then21C120.
// False leaves graph, libm, shared and visual untouched; workspace may change.
bool solveChariotHands49T(const ChariotIkBindings49T&, const ChariotIkPair49T&,
    const SceneGraph49P&, SourceLibmState49T&, ChariotIkShared49T&,
    VisualShared49R&, ChariotIkWorkspace49T&, ChariotIkDiagnostic49T&) noexcept;
}
