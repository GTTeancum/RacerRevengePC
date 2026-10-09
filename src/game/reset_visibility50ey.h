#pragma once
#include "live_zone_membership50en.h"
#include "vehicle_frame49w.h"

namespace rr::game {
// Actual application4C0 namespace. Initialize these owners once using50CL and
//50EN with separate backing from application4C4. Original272BC8/272C10 build
// these links from the SAME authored portals, but distinct27F3B0 allocations.
// Native zone refs retain authored ordinal+1. Current support requires the
// actual visibilityDynamic114 method to be List in every supplied zone; the
// cold caller must check readZoneSearchSelection50CN, not assume List from the
// collision method. No renderer or collision-object namespace is implied.
struct ResetVisibilityWorld50EY {
    ZoneLifecycle50CL lifecycle{};
    LiveZoneWorld50EN portals{};
};

struct ResetVisibilityOwner50EY {
    // Stable native identity of THIS vehicle's object+30 visibility base.
    // It indexes the visibility lifecycle, never a collision query table.
    CollectionRef50BX object30=0;
    LiveZoneStorage50EN storage{};
    LiveZoneMembership50EN membership7F0{};
    // Sole live projection of *(vehicle254)+50.216AC8 clears this only when
    //7F0 was nonnull. Attach does not invent a corresponding nonzero write.
    std::uint32_t* object254Field50=nullptr;
};
enum class ResetVisibilityStatus50EY {
    SourceSliceComplete,Storage,Binding,ShapeNotPrepared,Registry
};
struct ResetVisibilityResult50EY {
    ResetVisibilityStatus50EY status=ResetVisibilityStatus50EY::Binding;
    std::uint32_t sourcePC=0;
    LiveZoneResult50EN registry{};
};

//216A90..216AC8, BEFORE690/694/7A4/348 removals. Executes actual281490
// primary-then-secondary registry removal, clears7F0, then254+50. Caller-owned
// storage replaces allocation/destruction; it is retained for the next attach.
// A genuinely detached bound owner follows the original null7F0 skip and
// preserves254+50. A missing owner/registration cannot masquerade as that skip.
ResetVisibilityResult50EY removeResetVisibility50EY(ResetVisibilityWorld50EY&,
    ResetVisibilityOwner50EY&,LiveZoneWorkspace50EN&) noexcept;

//216874..21689C, AFTER actual21684C/21685C694 preparation and BEFORE690 attach.
// Reads the canonical frame.shape, verifies its detached type0/current ready
// main190 world, and derives1202E0 bounds from composed00 + authored root.
//2809C0 receives explicit C30, method2, and the cached694 radius (not zero).
// The same real694 owner may subsequently be prepared again at21698C.
// prepareUndertrackPodQuery50EW supplies the preceding source operation.
ResetVisibilityResult50EY attachResetVisibility50EY(ResetVisibilityWorld50EY&,
    ResetVisibilityOwner50EY&,const VehicleFrameState49W&,
    const VehicleFrameBindings49W&,const PodShapeSource49W&,
    CollectionRef50BX visibilityZoneC30,LiveZoneWorkspace50EN&) noexcept;

// Both functions perform real source registry operations; success covers ONLY
// the named slice. No228BBC/49W phase, collision query, camera scene/follow-query
// or host draw state is advanced. A distinct camera object ref can share this
// visibility lifecycle through its own actual114770/1148D0 owner, not vehicle7F0.
// Serial calls between collections, stable bounded caller-owned backing, no
// allocation. The two namespace backings, every live owner, the254 field, and
// workspace must be mutually disjoint. The full frame/bindings/source are
// read-only and disjoint from writable storage.50EN's topology/capacity and
// arithmetic boundaries remain explicit. Failure preserves all live state;
// workspace may change. Do not copy an attached owner or mutate it via50CL.
}
