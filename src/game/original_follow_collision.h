#pragma once
#include "original_camera_world_query.h"
#include "original_camera_racer_history.h"
#include "../assets/tv_camera49e.h"
#include "scene_graph49p.h"
#include <array>

namespace rr::game {
using OriginalFollowCollisionVector=assets::TVCameraVector49E;
struct OriginalFollowCollisionFilter {float value=0.f,tau=0.f,blend=1.f;};
struct OriginalFollowCollisionState {
    OriginalFollowCollisionVector localPoint1A0{};
    std::array<OriginalFollowCollisionFilter,3> contact13C{{{0.f,.05f,1.f},{0.f,.05f,1.f},{0.f,.05f,1.f}}};
    std::array<OriginalFollowCollisionFilter,3> clear148{{{0.f,.2f,1.f},{0.f,.2f,1.f},{0.f,.2f,1.f}}};
    OriginalCameraCollisionCallback callback{};
};
struct OriginalFollowCollisionInput {
    OriginalFollowCollisionVector position70{},rightF0{},target80{},flatForwardA0{},upB0{};
    float elapsed=0.f;
};
struct OriginalFollowCollisionSources {
    // Exact racer158->190 graph node after source refresh. World cache must
    // already be ready; no substitution of rendered/cockpit bounds is allowed.
    const SceneGraph49P* graph=nullptr;
    SceneNodeIndex49P racerScene190=SceneNodeIndex49P::none;
    const BasePoseHistoryState50Y* history=nullptr;
    OriginalFollowCollisionVector mainBodyVelocity70{};
    float cameraDistance04=0.f;
    std::uint32_t selectedProfile34=0;
};
struct OriginalCameraQueryPairOwner;
enum class OriginalFollowCollisionStatus {Complete,Binding,Arithmetic,History,WorldQuery};
struct OriginalFollowCollisionResult {
    OriginalFollowCollisionStatus status=OriginalFollowCollisionStatus::Complete;
    std::uint32_t sourcePC=0;
    OriginalFollowCollisionVector position70{};
    std::size_t queryPairs=0;
    OriginalCameraWorldResult world{};
};
// Complete17A870 using the concrete original camera pair-query owner. Right,
// flatForward and Up are source copies but unused by this original helper;
// target80 is not mutated. State/position commit only on Complete; genuine
// preceding live-registration/query side effects remain owned by pairOwner.
OriginalFollowCollisionStatus advanceOriginalFollowCollision17A870(
    const OriginalFollowCollisionInput&,const OriginalFollowCollisionSources&,
    OriginalFollowCollisionState&,OriginalCameraQueryPairOwner&,
    OriginalFollowCollisionResult&) noexcept;
}
