#pragma once
#include "graph_body49w.h"
#include <array>

namespace rr::game {
// Value-only snapshot of source28A530 local look/up. It retains no graph,
// asset, material, registry, renderer packet or source-lifetime ownership.
// W is deliberately absent: Rider initializes both saved W lanes to1, and
// source289EC0 later explicitly writes basis W=0 and translation W=1.
struct OriginalUiRiderPodOrientation50KG {
    std::array<float,3> look{};
    std::array<float,3> up{};
};
// Directional Rider restore ONLY: source289EC0(a3=0,t0=1,t1=0).
// Normalize Up then Look independently and form Right=Up cross Look. Do not
// cross-correct Look or normalize Right. This pure builder can preflight the
// captured axes before destructive outgoing callbacks. False leaves output
// untouched. Native sqrt/binary32 policy, FE_TONEAREST, no guest/VU execution;
// finite XYZ and nonzero finite squared axis lengths are native admission.
// Nonzero parallel axes are not orthogonalized or rejected by this branch.
bool originalUiRiderPodOrientationPose50KG(const std::array<float,3>& position,
    const OriginalUiRiderPodOrientation50KG&,BodyMatrix48G& output) noexcept;
// Capture copies local+C0 look/up XYZ exactly, without refreshing world caches
// or doing arithmetic. False leaves output unchanged. Restore reads position
// from the INCOMING local matrix, writes its complete local matrix, then uses
// source28C4F0/ready-gated289A20 propagation; cached worlds stay lazy.
// Coherent caller-owned graph, valid node, serial access and disjoint snapshot
// storage are required. No attachment, visibility/cache membership, material
// lifetime, navigation, renderer fence or source callback is supplied here.
bool captureOriginalUiRiderPodOrientation50KG(const SceneGraph49P&,
    SceneNodeIndex49P,OriginalUiRiderPodOrientation50KG& output) noexcept;
bool restoreOriginalUiRiderPodOrientation50KG(const SceneGraph49P&,
    SceneNodeIndex49P,const OriginalUiRiderPodOrientation50KG&) noexcept;
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
