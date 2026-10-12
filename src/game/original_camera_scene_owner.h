#pragma once
#include "reset_visibility50ey.h"
#include <cstdint>

namespace rr::game {
// Actual scene+30 visibility base. This object is distinct from every vehicle
// visibility object and from Follow's two collision objects. Borrow the SAME
// application4C0 lifecycle/portal caches as other visibility owners, never4C4.
struct OriginalCameraSceneOwner {
    std::uint64_t sessionGeneration=0,sceneKey=0,allocationSerial=0;
    CollectionRef50BX object30=0;
    LiveZoneStorage50EN storage{};
    LiveZoneMembership50EN registration34{};
    SpringVector4_48N position180{{0,0,0,1}};
    CollectionRef50BX parent38=0;
};
struct OriginalCameraSceneSpan {const void* data=nullptr;std::size_t bytes=0;};
// Cross-owner guard for the complete controller transaction. These spans may
// not overlap the actual scene namespace, owner or scratch. No side effects.
bool admitOriginalCameraSceneStorage(ResetVisibilityWorld50EY&,OriginalCameraSceneOwner&,
    LiveZoneWorkspace50EN&,const OriginalCameraSceneSpan* writable,std::size_t writableCount,
    const OriginalCameraSceneSpan* readable,std::size_t readableCount) noexcept;
// Cold constructor+owner binding only; explicit stable native identities.
// Initial source114770 must still attach to its real source-selected zone.
bool bindFreshOriginalCameraScene(OriginalCameraSceneOwner&,
    std::uint64_t sessionGeneration,std::uint64_t sceneKey,
    CollectionRef50BX object30,const LiveZoneStorage50EN&) noexcept;

// Source114770/280890/2809C0: direct point180, source3E0060=(0,0,0,1),
// radius0, method bit2. No collision shape or synthetic identity transform.
LiveZoneResult50EN attachOriginalCameraScene114770(ResetVisibilityWorld50EY&,
    OriginalCameraSceneOwner&,CollectionRef50BX visibilityZone,
    const SpringVector4_48N& position180,LiveZoneWorkspace50EN&) noexcept;
//175224..175254: bind from the actual already-attached racer7F0 visibility
//record. Its primary zone and saved20 are copied, never collision690 here.
LiveZoneResult50EN attachOriginalCameraSceneFromRacer175224(ResetVisibilityWorld50EY&,
    OriginalCameraSceneOwner&,const LiveZoneMembership50EN& racer7F0,
    LiveZoneWorkspace50EN&) noexcept;
// Source1148D0/281490: primary then linked secondary removal, clear34 last.
// Source38 is retained. Backing is retained instead of guest allocation/free.
LiveZoneResult50EN removeOriginalCameraScene1148D0(ResetVisibilityWorld50EY&,
    OriginalCameraSceneOwner&,LiveZoneWorkspace50EN&) noexcept;
// Source114C44/280D40: portal crossing uses the actual saved registration20;
// position180 and parent38 publish only on complete native admission.
LiveZoneResult50EN updateOriginalCameraScene114C44(ResetVisibilityWorld50EY&,
    OriginalCameraSceneOwner&,const SpringVector4_48N& position180,
    LiveZoneWorkspace50EN&) noexcept;
// Complete remove+new registration, then1781F8 saved20 overwrite. This is a
// native transaction around the SAME source order. It does not choose Follow.
LiveZoneResult50EN rebindOriginalCameraScene177AB0(ResetVisibilityWorld50EY&,
    OriginalCameraSceneOwner&,CollectionRef50BX visibilityZone,
    const SpringVector4_48N& position180,const SpringVector4_48N& racerSaved20,
    LiveZoneWorkspace50EN&) noexcept;

// Source secondary-list order and all portal caches are preserved. Fresh or
// live visibility namespaces must have authenticated actual SearchType=List;
// both original GC and GA supply List in all12 zones. Both methods use actual
// initialized1024-slot source SearchLists; no quadtree substitution is allowed.
// All storage and full declared spans are stable, disjoint and caller-owned.
// Calls are serial between traversals. Rejection preserves live state; scratch
// may change. Original50EN bounded first-block and non-head-removal boundary
// remain explicit. No collision registry/scene graph/racer body is mutated.
}
