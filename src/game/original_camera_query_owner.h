#pragma once
#include "original_camera_world_query.h"
#include "live_zone_membership50en.h"
#include "collision_normal_mode50ja.h"

namespace rr::game {
// One Follow camera's two genuinely distinct collision objects and shapes.
// Cold values come from original_camera_query_cold, never from host misses.
struct OriginalCameraQueryPairState {
    OriginalCameraWorldQueryState sphere24{},ray190{};
    SpringVector4_48N sphereCenter00{},endpoints00[2]{};
    std::uint32_t endpointMetadata[2]{};
    float sphereRadius50=0.f,sphereRadiusSquared54=0.f;
    std::uint32_t rayNormalMode44=0;
    SpringVector4_48N sentinel3E02C0{};
};
struct OriginalCameraQueryPairOwner {
    ZoneLifecycle50CL* lifecycle=nullptr;
    LiveZoneWorld50EN* liveWorld=nullptr;
    LiveZoneWorkspace50EN* liveWorkspace=nullptr;
    LiveZoneMembership50EN* sphereE0=nullptr;
    LiveZoneMembership50EN* rayE0=nullptr;
    CollectionRef50BX sphereObject=0,rayObject=0;
    OriginalCameraQueryPairState* state=nullptr;
    // Only these world table/global fields are borrowed; query-specific fields
    // are constructed from the actual per-object state/membership each call.
    OriginalCameraWorldInput world{};
    OriginalCameraWorldMutable mutableWorld{};
    OriginalCameraWorldWorkspace queryWorkspace{};
    // SAME application owner written by the other original type3 dispatches.
    CollisionNormalModeOwner50JA* normalMode3DF5B0=nullptr;
};
enum class OriginalCameraPairStatus {Complete,Storage,Binding,Arithmetic,LiveRegistration,WorldQuery};
struct OriginalCameraPairResult {
    OriginalCameraPairStatus status=OriginalCameraPairStatus::Complete;
    std::uint32_t sourcePC=0;
    LiveZoneResult50EN registration{};
    OriginalCameraWorldResult world{};
    unsigned updatesCommitted=0,queriesCommitted=0;
};
struct OriginalCameraOwnerSpan {const void* data=nullptr;std::size_t bytes=0;};
// Cross-owner admission used by17A870 before any live-registration prefix.
// The extra spans describe its own state/output and borrowed racer/scene data;
// they cannot hide a collision or substitute for the concrete world query.
bool admitOriginalCameraQueryPairStorage(const OriginalCameraQueryPairOwner&,
    const OriginalCameraOwnerSpan* writable,std::size_t writableCount,
    const OriginalCameraOwnerSpan* readable,std::size_t readableCount) noexcept;
//17A870 pair order: clear114; sphere11C1F0/11FF60, ray11CA80/11FF60;
// ray121920 first, sphere121920 only while114==0. executeQueries=false is the
// final origin reset: prepare/register both but preserve114 and do not query.
// The two actual E0 memberships must already be attached by179A80/120070 to
// method Second. Bind the exact same lifecycle methods/zones and object table.
// Source-prefix side effects are retained on a later error, explicitly counted;
// no frame-wide rollback, anonymous collision callbacks or synthetic misses.
OriginalCameraPairStatus updateOriginalCameraQueryPair(
    OriginalCameraQueryPairOwner&,const SpringVector4_48N& target,
    const SpringVector4_48N& endpoint,const SpringVector4_48N& sphereCenter,
    bool executeQueries,OriginalCameraCollisionCallback&,OriginalCameraPairResult&) noexcept;

// Live179A80 continuation. Neither actual query may be absent: cold creation
// belongs to attachOriginalCameraQueryPair. Successful source operations remain
// committed on a later rejection, and the cursor resumes that exact operation.
// The original cached sphere center/radius survive its11C1F0 setter:179A80 has
// NO11FF60 between the live setter and120070. Ray metadata is cleared by11CA80.
enum class OriginalCameraPairResetStage : std::uint8_t {
    idle,sphereRemove,sphereSet,sphereAttach,rayRemove,raySet,rayAttach,complete
};
struct OriginalCameraPairResetCursor {
    OriginalCameraPairResetStage stage=OriginalCameraPairResetStage::idle;
    OriginalCameraQueryPairOwner* owner=nullptr;
    CollectionRef50BX zone=0;
    SpringVector4_48N position{};
    float radius=0.f;
};
OriginalCameraPairStatus resetOriginalCameraQueryPair179A80(
    OriginalCameraQueryPairOwner&,CollectionRef50BX collisionZoneC34,
    const SpringVector4_48N& originalAnchor,float actualRadius178,
    OriginalCameraPairResetCursor&,OriginalCameraPairResult&) noexcept;

// Fresh179A80 attachment after cold query projection, sphere first then ray.
// This deliberately does not insert a missing11FF60 between the setter and
//120070. The retained sphere cache can differ from its new local center.
// Caller-owned membership backing is already assigned to each detached E0;
// initialZone is the original scene/reset owner, never guessed from position.
OriginalCameraPairStatus attachOriginalCameraQueryPair(
    OriginalCameraQueryPairOwner&,CollectionRef50BX initialZone,
    const SpringVector4_48N& position,OriginalCameraPairResult&) noexcept;
}
