#pragma once
#include "original_camera_query_owner.h"
#include <memory>

namespace rr::game {
// One actual22D810 query120. These fields never alias Follow sphere24/ray190.
struct OriginalDeathCameraQueryState {
    OriginalCameraWorldQueryState query120{};
    SpringVector4_48N endpoints00[2]{};
    std::uint32_t endpointMetadata[2]{};
    std::uint32_t rayNormalMode44=0;
    SpringVector4_48N sentinel3E02C0{};
    std::uint64_t owner9C=0;
    std::uint32_t callbackE4=0,staticCategoryMaskD8=0,objectIdentity120=0;
};
enum class OriginalDeathCameraQueryStatus {Complete,Storage,Binding,Arithmetic,LiveRegistration,WorldQuery};
struct OriginalDeathCameraQueryColdInput {
    std::uint64_t owner9C=0;
    SpringVector4_48N initialPosition{},sentinel3E02C0{};
    std::uint8_t directTypeTagGp8010=0;
    std::uint32_t staticObjectMask3E0720=0;
    // Stable, separately backed actual one-word D8 allocation. Caller copies
    // the initialized application static mask here before cold projection.
    const std::uint32_t* categoryD8=nullptr;
};
struct OriginalDeathCameraQueryColdProjection {
    OriginalDeathCameraQueryStatus status=OriginalDeathCameraQueryStatus::Binding;
    std::uint32_t sourcePC=0;
    OriginalDeathCameraQueryState state{};
    CollectionObject50BX object{};
    CollectionObjectState50BX collection{};
    OriginalCameraWorldObjectCopies copies{};
};
// Fresh11B190(1)/11B570(2),9C=death/E4=null,124160(0),124170(static mask),
// F8 bit0=1,11C710(1,0,0),11CA80(position,position). Keeps source-unwritten
// scalar/prior bytes. Only a successful projection can be published to the
// same actual object/state/copy/cache owners. categoryD8 stays separately
// backed for the full query lifetime. No allocator/scene/bootstrap is claimed.
OriginalDeathCameraQueryColdProjection projectOriginalDeathCameraQueryCold(
    const OriginalDeathCameraQueryColdInput&,const OriginalDeathCameraQueryState&,
    const CollectionObject50BX&,const CollectionObjectState50BX&) noexcept;
struct OriginalDeathCameraQueryOwner {
    ZoneLifecycle50CL* lifecycle=nullptr;
    LiveZoneWorld50EN* liveWorld=nullptr;
    LiveZoneWorkspace50EN* liveWorkspace=nullptr;
    LiveZoneMembership50EN* membershipE0=nullptr;
    CollectionRef50BX object120=0;
    OriginalDeathCameraQueryState* state=nullptr;
    std::uint64_t owner9C=0;
    OriginalCameraWorldInput world{};
    OriginalCameraWorldMutable mutableWorld{};
    OriginalCameraWorldWorkspace queryWorkspace{};
    CollisionNormalModeOwner50JA* normalMode3DF5B0=nullptr;
};
// Native cold allocation owner. The object slot and membership backing may be
// reserved with the collision world, but query120 and its D8 allocation remain
// absent until the real22D894/22D8A8 path. No Follow object is reused.
struct OriginalDeathCameraQueryColdOwner {
    std::unique_ptr<OriginalDeathCameraQueryState> allocation120;
    std::unique_ptr<std::uint32_t> categoryD8;
    CollectionObject50BX* object120=nullptr;
    OriginalCameraWorldGeometry* geometry120=nullptr;
    std::uint8_t* globalValid3E0730=nullptr;
    std::uint32_t identity120=0,sourcePC=0;
    bool attempted=false,complete=false;
};
// Includes absent-state admission without preconstructing or projecting it.
// Allocation itself occurs only after the scene/binding source prefix.
bool admitOriginalDeathCameraQueryColdStorage(const OriginalDeathCameraQueryOwner&,
    const OriginalDeathCameraQueryColdOwner&,const OriginalCameraOwnerSpan*,std::size_t,
    const OriginalCameraOwnerSpan*,std::size_t) noexcept;
OriginalDeathCameraQueryStatus constructOriginalDeathCameraQuery22D8A8(
    OriginalDeathCameraQueryOwner&,OriginalDeathCameraQueryColdOwner&) noexcept;
struct OriginalDeathCameraQueryResult {
    OriginalDeathCameraQueryStatus status=OriginalDeathCameraQueryStatus::Complete;
    std::uint32_t sourcePC=0;
    LiveZoneResult50EN registration{};
    OriginalCameraWorldResult world{};
    unsigned removalsCommitted=0,updatesCommitted=0,queriesCommitted=0;
};
// Cross-owner admission before the caller publishes a source prefix. Extra
// spans are bounded identically to the Follow admission API.
bool admitOriginalDeathCameraQueryStorage(const OriginalDeathCameraQueryOwner&,
    const OriginalCameraOwnerSpan* writable,std::size_t writableCount,
    const OriginalCameraOwnerSpan* readable,std::size_t readableCount) noexcept;
// Verifies the exact bound death68 key, actual object120 definition/cache,
// callbackE4=null and attached Second-method E0. Does not mutate or allocate.
bool originalDeathCameraQueryAttached(const OriginalDeathCameraQueryOwner&,
    std::uint64_t death68Key) noexcept;
//22D950..22D970: setter,120290,120070(zone,Second). Reusing the actual query
// detaches its existing E0. This stops before the intervening camera writes.
OriginalDeathCameraQueryStatus attachOriginalDeathCameraQuery120(
    OriginalDeathCameraQueryOwner&,CollectionRef50BX initialZone,
    const SpringVector4_48N& position,OriginalDeathCameraQueryResult&) noexcept;
//22DC50 final120230 after the caller's original intervening camera writes.
OriginalDeathCameraQueryStatus refreshOriginalDeathCameraQuery120(
    OriginalDeathCameraQueryOwner&,OriginalDeathCameraQueryResult&) noexcept;
//22F72C/22FE64:11CA80 ->11FF60 (includes120230) ->121920(a1=0).
// Complete retains F8 bit5, result90+20 selectedNormal and+30 selectedPoint,
// iterator/cache/candidate40/50 writes and global normal mode. E4 is NULL;
// no Follow1792D0 callback is constructed or invoked. Caller consumes actual
// query120.flagsF8/type3 fields, never a host collision receipt or forced miss.
// Source-prefix live registration persists if the subsequent world query fails.
OriginalDeathCameraQueryStatus updateOriginalDeathCameraQuery120(
    OriginalDeathCameraQueryOwner&,const SpringVector4_48N& start,
    const SpringVector4_48N& end,OriginalDeathCameraQueryResult&) noexcept;
//22D0C4 destructor120290; removes only this query's genuine membership.
OriginalDeathCameraQueryStatus removeOriginalDeathCameraQuery120(
    OriginalDeathCameraQueryOwner&,OriginalDeathCameraQueryResult&) noexcept;
}
