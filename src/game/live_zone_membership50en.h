#pragma once
#include "portal_geometry50eo.h"
#include "zone_lifecycle50cl.h"
#include "type0_bounds50ew.h"

namespace rr::game {
// Input order is the original authored portal order. Vertices must be parsed
// with the source binary32 parser; no nearest-zone or geometry sorting occurs.
struct LiveZonePortalInput50EN {
    CollectionRef50BX zoneA=0,zoneB=0;
    Vector3_48C vertices[4]{};
};
struct LiveZonePortalLink50EN {
    CollectionRef50BX next=0,zone=0,portal=0;
    float minimumDistanceC=0,maximumDistance10=0;
};
struct LiveZoneWorld50EN {
    CollectionRef50BX* portalHeads=nullptr;std::size_t zoneCount=0;
    LiveZonePortalLink50EN* portalLinks=nullptr;std::size_t portalLinkCount=0;
    PortalGeometry50EO* portals=nullptr;std::size_t portalCount=0;
};
struct LiveZoneWorldStorage50EN {
    CollectionRef50BX* portalHeads=nullptr;std::size_t zoneCapacity=0;
    LiveZonePortalLink50EN* portalLinks=nullptr;std::size_t portalLinkCapacity=0;
    PortalGeometry50EO* portals=nullptr;std::size_t portalCapacity=0;
};
enum class LiveZoneStatus50EN {
    Complete,InvalidInput,InvalidTopology,InsufficientCapacity,BoundsRejected,
    GeometryRejected,RegistryRejected,TraversalBoundary,NonFinite,
    UnsupportedLinkRemoval
};

// Source272B3C..272C70 visits portal records in ascending authored order.
// Each27F3B0 call PREPENDS a directional link, so incident lists are reverse
// authored order. Builds collision links only, not the visibility/global head.
// Fresh storage only. Failure preserves all outputs; unused tails are untouched.
LiveZoneStatus50EN prepareLiveZoneWorld50EN(const LiveZonePortalInput50EN*,
    std::size_t portalCount,std::size_t zoneCount,
    const LiveZoneWorldStorage50EN&,LiveZoneWorld50EN&) noexcept;

struct LiveZoneEntry50EN {
    ZoneRegistration50CJ registration{};
    CollectionRef50BX portal=0;
    float distance10=0;
    bool used=false;
};
struct LiveZoneStorage50EN {
    CollectionZoneLink50BX* links=nullptr;
    LiveZoneEntry50EN* entries=nullptr;
    std::size_t capacity=0;
};
struct LiveZoneMembership50EN {
    LiveZoneStorage50EN storage{};
    CollectionRef50BX object=0;
    ZoneMethod50CL method=ZoneMethod50CL::First;
    bool attached=false; // Native projection of a nonnull object.E0.
    CollectionBoundsResult48S bounds{};
    // Successful-prefix receipt, owned by this adapter. A live narrowphase
    // admission must compare the actual current scene/resource, not just sphere
    // bounds, and retain its own expected object identity and update revision.
    std::uint32_t lastShapeTag=0;
    BodyMatrix48G lastShapeWorld{};
    const Type2ResourceView49X* lastType2Resource=nullptr;
    std::uint64_t revision=0;
};
struct LiveZoneWorkspace50EN {
    SearchStorage50BS* registries=nullptr;std::size_t registryCapacity=0;
    CollectionZoneLink50BX* links=nullptr;
    LiveZoneEntry50EN* entries=nullptr;std::size_t membershipCapacity=0;
    LiveZonePortalLink50EN* portalLinks=nullptr;
    std::size_t portalLinkCapacity=0;
    ZoneRegistrationScratch50CJ transfer{};
};
struct LiveZoneBounds50EN {
    CollectionBoundsInput48S bounds{};
    const Type2ResourceView49X* type2Resource=nullptr;
    // Tag0 consumes bounds.shapeWorld as actual shape.composed00, never world80.
    // Geometry is the original root projection already retained by49W.
    const PodShapeSource49W* type0Resource=nullptr;
};
struct LiveZoneResult50EN {
    LiveZoneStatus50EN status=LiveZoneStatus50EN::InvalidInput;
    CollectionBoundsStatus48S boundsStatus=CollectionBoundsStatus48S::Complete;
    PortalGeometryStatus50EO geometryStatus=PortalGeometryStatus50EO::Complete;
    SearchStorageStatus50BS registryStatus=SearchStorageStatus50BS::Complete;
    std::size_t crossedPortals=0;
};

// Source2809C0 bounded SearchList registration. Initial zone is an explicit
// caller decision backed by scene/reset ownership, never inferred from position.
// initialOverlapRadius is explicit: collision-object120070 passes ZERO at120170;
// vehicle visibility21688C instead passes cached radius. It does not overwrite
// the bounds cache used by subsequent120230 updates.
// The object Ref must exist in lifecycle and its primary record must be empty.
// Fresh membership state/storage only; storage is borrowed, stable and exclusive.
LiveZoneResult50EN attachLiveZoneMembership50EN(ZoneLifecycle50CL&,
    LiveZoneWorld50EN&,LiveZoneMembership50EN&,const LiveZoneStorage50EN&,
    CollectionRef50BX object,CollectionRef50BX initialZone,ZoneMethod50CL,
    float initialOverlapRadius,const LiveZoneBounds50EN&,LiveZoneWorkspace50EN&) noexcept;

// Source120230 ->1202E0 ->280D40, including portal crossing230BD0, primary
// transfer, portal-overlap231150 registration and saved-center publication.
// Detached objects return Complete without evaluating bounds, matching120230.
LiveZoneResult50EN updateLiveZoneMembership50EN(ZoneLifecycle50CL&,
    LiveZoneWorld50EN&,LiveZoneMembership50EN&,const LiveZoneBounds50EN&,
    LiveZoneWorkspace50EN&) noexcept;

// Source120290/281490 removal order: primary, then linked secondary entries;
// clear the native E0 projection last. Native backing is retained, not freed.
LiveZoneResult50EN removeLiveZoneMembership50EN(ZoneLifecycle50CL&,
    LiveZoneWorld50EN&,LiveZoneMembership50EN&,LiveZoneWorkspace50EN&) noexcept;

// Collector binding after a successful attach/update:
//   zoneLinks=membership.storage.links; zoneLinkCount=storage.capacity;
//   zoneHead=membership.attached ? 1 : explicitCallerFallbackHead.
// Link1 is primary; secondary links retain original head-insertion order.
// Do not append every authored zone or reuse the global443148 link as E0.
//
// Mutations are serial, BETWEEN collections. This adapter temporarily owns the
// lifecycle registries/views and this object's primary registration. Other50CL
// records remain untouched; do not mutate this object through50CL/50CQ while
// attached. Each membership needs its own live links/entries; workspace can be
// reused serially. All declared capacities, descriptors, state and scratch must
// be aligned, backed, mutually disjoint writable storage. Bounds source spans
// must be disjoint from every writable span. No allocation or observer callback.
//
// All non-Complete results preserve live state, registry words, records, views
// and portal caches; workspace may change. Existing first-block capacity and
// source LIFO registry slots are retained. Cyclic crossing walks are bounded by
// an explicit finite world-derived visit limit. Source281320 assigns head from
// the removed node even when a searched match is not head; that non-head removal
// is an explicit UnsupportedLinkRemoval boundary rather than orphaning live
// registrations or silently replacing original pointer semantics.
}
