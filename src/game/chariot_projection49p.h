#pragma once
#include "scene_graph49p.h"
#include <cstdint>

namespace rr::game {
// Native admission tags, not recovered vehicle fields or a frame scheduler.
enum class ChariotProjectionPhase49P : std::uint8_t {
    unspecified,
    beforePhysicalCopy21B9A4,
    laterPhysics,
    afterPhysics
};
enum class ChariotSuppliedState49P : std::uint8_t {
    unavailable,
    localMatricesOnly,
    completeSceneStateThroughChariotPrefix
};
struct ChariotProjectionInput49P {
    ChariotProjectionPhase49P phase = ChariotProjectionPhase49P::unspecified;
    ChariotSuppliedState49P suppliedState = ChariotSuppliedState49P::unavailable;
    // Live words at this boundary. Constructor defaults are not admission.
    std::uint32_t vehicle838 = 0;
    std::uint32_t vehicle83C = 0;
    // none means absent. These are distinct, unparented roots in one forest.
    SceneNodeIndex49P secondaryPhysical19C = SceneNodeIndex49P::none;
    SceneNodeIndex49P chariotModel1B4 = SceneNodeIndex49P::none;
};

// Only the physical-mode branch21B9A4..21B9D0 of21A660, through setter28B490.
// Requires live838!=0 AND83C==0. Other modes are unsupported and return false;
// they are not successful no-ops. No constructor or whole-base-state initializer.
//
// The caller supplies the actual full scene state at pre21B9A4: every local,
// cached world, flag byte and link, after any first-base217440 nullcg relocation,
// controls217DE0/2176E0, animation21A000, engine/container21C120, and preceding
//21A660 effects. These producers are NOT run or replaced by callbacks here.
// Local matrices alone cannot certify their effects on ready flags/world caches.
// The current main child194 local and animated model-node locals remain supplied
// inputs. Tags attest this external boundary; they do not prove it was reached.
//
// Original228620 calls those graphics stages BEFORE later physics. A scene state
// taken after the later physics phase is not admitted as that same base phase.
// This function is not a49K/M bridge or a post-substep graphics callback.
//
// Copies all16 secondary19C LOCAL words to Chariot1B4 LOCAL, then applies exactly
// the setter's ancestor bit80 and ready01-gated descendant invalidation. Source
// cached world is not consumed/refreshed. No target2F0 offset is added. All other
// state, including cached destination/descendant worlds, remains unchanged.
// Later world/palette reads must explicitly use refreshSceneWorld49P at their
// own admitted boundary; no eager palette generation occurs in this branch.
//
// Uses the scene graph's bounded topology/rounding/storage contract. False leaves
// all represented graph state unchanged. Does not claim full21A660, all three
// rendered models, animation, observer behavior, race spawn, or FrameReady.
bool projectChariotPhysicalBranch49P(const SceneGraph49P& graph,
    const ChariotProjectionInput49P& input) noexcept;
}
