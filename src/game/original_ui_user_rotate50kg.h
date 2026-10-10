#pragma once
#include "graph_body49w.h"
#include <array>

namespace rr::game {
struct OriginalUiUserRotateParameters50KG {
    std::array<bool,3> enabled{};
    std::array<float,3> revolutionsPerSecond{};
    std::array<float,3> position{}; // widget+70, retained across rotations
};
// Native source1D38B0 loaded/unpaused numerical branch. The model root remains
// in the shared49P graph. This is not model acquisition, deferred loading,
// lighting, binder rendering, observer attachment or destination readiness.
// Caller invokes141ED0 children afterwards even when loaded=false/paused=true.
// Serial, strict binary32, FE_TONEAREST. No mutable aliases with graph storage.
// False leaves graph untouched. Uses actual source28B840 shared axis owner.
bool advanceOriginalUiUserRotate50KG(const OriginalUiUserRotateParameters50KG&,
    bool loaded,bool paused,float elapsedSeconds,const SceneGraph49P&,
    SceneNodeIndex49P modelRoot) noexcept;
// Source1D3B60 with t0=1,t1=0: correct look against up, normalize both,
// reconstruct right=up cross look, retain authored XYZ and homogeneous1.
// Native sqrt, no guest instructions or VU execution. Reject degenerate axes.
bool originalUiUserRotatePose50KG(const std::array<float,3>& position,
    const std::array<float,3>& look,const std::array<float,3>& up,
    BodyMatrix48G& output) noexcept;
}
