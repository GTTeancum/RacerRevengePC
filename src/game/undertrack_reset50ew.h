#pragma once
#include "undertrack_reposition50et.h"
#include "vehicle_frame49w.h"
#include "live_zone_membership50en.h"
#include "reset_model_bounds50ba.h"
#include "reset_effects50bk.h"
#include "reset_probe50ag.h"
#include "static_type2_producer49x.h"
#include "terrain_prepare48m.h"
#include "water_height50jc.h"

namespace rr::game {
// Distinct source callers, never a temporary phase/type impersonation. The
// undertrack entry retains its post-packed admission;1A17A4 controller input
// uses Ready. Original AI16EAA0 stays PostPlayer with its scalar continuation.
enum class UndertrackResetDomain50EW : std::uint8_t {
    PlayerPostPackedBytes50S, OriginalAiPostPlayer, PlayerControllerReady50LC,
    SourceTriggerAiPostWorld
};
enum class UndertrackResetStatus50EW {
    SourceSliceComplete,Storage,Phase,Binding,Arithmetic,Scene,ModelBounds,
    Effects,MissingQueryOwner,QueryPreparation,Registry,GroundNotResolved,
    UnsupportedHeightOwner,WaterHeight
};
struct UndertrackResetResult50EW {
    UndertrackResetStatus50EW status=UndertrackResetStatus50EW::Binding;
    std::uint32_t sourcePC=0;
    std::uint32_t ownerOffset=0;
    LiveZoneResult50EN registry{};
    EffectDispatchStatus50BE effects=EffectDispatchStatus50BE::Complete;
    WaterHeightResult50JC water{};
};
enum class UndertrackHeightOwner50EW : std::uint8_t {Unbound,Absent,Present};
struct UndertrackPhysicsDomain50EW {
    UndertrackHeightOwner50EW terrain330=UndertrackHeightOwner50EW::Unbound;
    UndertrackHeightOwner50EW water334=UndertrackHeightOwner50EW::Unbound;
    // Borrowed actual334 query owner and its shared globals. Present never
    // means that merely loading a resource constructed this live state.
    const WaterHeightProvider50JC* waterProvider334=nullptr;
    const WaterHeightGlobals50JC* waterGlobals=nullptr;
    // Actual348+1F0, initialized by1B1240 at1B1270. The214944..214950
    // setup supplies authored7EC, but a receipt/copy of7EC is not this owner.
    // Read only when water height exceeds the current BC4.
    const float* terrain348Lift1F0=nullptr;
};
struct UndertrackModelBounds50EW {
    const ResetBoundNode50BA* nodes=nullptr;
    std::size_t count=0;
    ModelBoundsResult49O* result=nullptr;
};
struct UndertrackPhysicsWorkspace50EW {
    VehicleFrameState49W frame{};
    UndertrackSceneWorkspace50ET scene{};
    ResetBoundsWorkspace50BA bounds{};
    ModelBoundsResult49O modelResults[5]{};
    EffectOwner50BE owners[maxEffectOwners50BE]{};
    ResetEffectWorkspace50BK effects{};
    std::uint32_t vehicle850=0;
    // Outer transaction scratch for optional canonical19CC50 spray controls.
    // Inner50BK commits only these copies; actual controls publish with frame.
    EffectControlState50BC controls81C[2]{};
};

// Concrete216B38..216D64, after the caller-specific prefix and registration
// removal prefix. Terrain330 must be explicitly ABSENT. Present334 consumes
// its actual50JC owner and globals, then stores waterY+348->1F0 only when
// waterY-BC4>0 (216BAC..216BCC). It never uses max(BC4,waterY+lift).
// Placement and frame commit together; later ground probing must use the
// adjusted placement. The query's optional effect-owner argument is null.
// Applies190/198 scenes, main/lifecycle reset, then
// initial1B4/19C scenes and secondary reset. Only canonical49W scene/body owners
// and the supplied placement change. Pose integrator fields, damage, timers,
// control and phase are preserved.
// Native atomic slice, not a receipt that missing earlier source effects ran.
UndertrackResetResult50EW placeInitialUndertrackPhysics50EW(VehicleFrameState49W&,
    const VehicleFrameBindings49W&,UndertrackPlacement50ET&,
    const UndertrackPhysicsDomain50EW&,const SecondaryActivationGlobals49V&,
    const ResetLifecycleGlobals50AT&,UndertrackPhysicsWorkspace50EW&,
    UndertrackResetDomain50EW=UndertrackResetDomain50EW::PlayerPostPackedBytes50S) noexcept;

// Legacy immutable-placement admission remains explicitly ABSENT330/334.
// It cannot discard a present water provider's persistent BC4 publication.
UndertrackResetResult50EW placeInitialUndertrackPhysics50EW(VehicleFrameState49W&,
    const VehicleFrameBindings49W&,const UndertrackPlacement50ET&,
    const UndertrackPhysicsDomain50EW&,const SecondaryActivationGlobals49V&,
    const ResetLifecycleGlobals50AT&,UndertrackPhysicsWorkspace50EW&,
    UndertrackResetDomain50EW=UndertrackResetDomain50EW::PlayerPostPackedBytes50S) noexcept;

// Concrete216F6C..2173F8 for reset(a1=0,a2=1), after the ACTUAL ground
// query. Both actual source callers admit Hit or actual20-query Exhausted
// after their continuation emits the216F48 diagnostic;
// it never relabels exhaustion as a hit or invents a floor height.
// Offsets are real2F0 and canonical parameters2C0/2B0, not fresh grid poses.
// Models are [chariot1B4,right1B8,left1BC, optional bottomRight1C0,bottomLeft1C4],
// each with its complete authored box
// span/output. Uses50BA's real dirty-descendant domain; never coerces cache bits.
// Applies source scene/bounds order, main reset, prior7B0/850, real50BK effect
// stops/timer clears and final canonicalD28 clear. Secondary pose stays lazy.
// a1=0 skips2173E8..2173F4: canonical fields.flagB9D0 is preserved.
// Requires unchanged X/Z from placement and probe's actual lift/terminal state.
// All outputs, including effectBindings.controls81C, commit together. Workspace
// may change on error. No frame advance. External controls must be disjoint
// from every other writable/read-only input and from each other.
UndertrackResetResult50EW finishUndertrackPhysics50EW(VehicleFrameState49W&,
    const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
    const UndertrackPlacement50ET&,const ResetProbe50AG&,
    const VisualVector49R& targetRelative2F0,const UndertrackModelBounds50EW*,std::uint32_t modelCount,
    const ResetEffectBindings50BK&,EffectOwner50BE*,std::size_t ownerCount,
    std::uint32_t& vehicle850,UndertrackPhysicsWorkspace50EW&,
    UndertrackResetDomain50EW=UndertrackResetDomain50EW::PlayerPostPackedBytes50S) noexcept;
// Legacy three-model callers retain their original admission.
UndertrackResetResult50EW finishUndertrackPhysics50EW(VehicleFrameState49W&,
    const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
    const UndertrackPlacement50ET&,const ResetProbe50AG&,
    const VisualVector49R& targetRelative2F0,const UndertrackModelBounds50EW (&)[3],
    const ResetEffectBindings50BK&,EffectOwner50BE*,std::size_t ownerCount,
    std::uint32_t& vehicle850,UndertrackPhysicsWorkspace50EW&,
    UndertrackResetDomain50EW=UndertrackResetDomain50EW::PlayerPostPackedBytes50S) noexcept;

// These are the four ACTUAL source vehicle query owners, in removal order.
// The secondary spring query is not a substitute for any of these slots.
enum class UndertrackQuerySlot50EW : std::uint8_t {Main690=0,Pod694=1,Movement7A4=2,Probe348=3};
struct UndertrackQueryOwner50EW {
    LiveZoneMembership50EN* membership=nullptr;
    LiveZoneStorage50EN storage{};
    CollectionRef50BX object=0;
    //690 and348: actual type2 query/cache/resource.348 can use the supported
    //rootless one-pair view; neither resource is manufactured by this adapter.
    Type2StaticQuery49X* type2=nullptr;
    //7A4: actual single-pair shape backing and cache. All four lanes and both
    //metadata words are written by the source11CA80 call. These must be the
    //same owners later consumed by the live movement query.
    SpringVector4_48N* endpoints=nullptr;
    std::uint32_t* metadata=nullptr;
    std::uint32_t* shapeTag=nullptr;
    CollisionSphere48J* segmentSphere=nullptr;
    //348 may be the existing enabled terrain owner's canonical preparation
    // state. This is an alternative to type2, never a second query/cache.
    TerrainPrepareState48M* terrain=nullptr;
    const Type2ResourceView49X* terrainResource=nullptr;
};
struct UndertrackQueryOwners50EW {UndertrackQueryOwner50EW query[4]{};};
struct UndertrackRegistryProgress50EW {
    std::uint8_t removed=0,attached=0; // Native source-order cursor, not game data.
};
// Source21684C/21685C: prepare actual694 while detached, then expose its tag0
//bounds for visibility7F0. Requires canonical main190 already source-ready.
// Commits shape matrix history/sphere; output borrows parameters.shape. A later
//694 collision reattach performs its second source11FE50 preparation again.
UndertrackResetResult50EW prepareUndertrackPodQuery50EW(VehicleFrameState49W&,
    const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
    LiveZoneBounds50EN&,
    UndertrackResetDomain50EW=UndertrackResetDomain50EW::PlayerPostPackedBytes50S) noexcept;
//216ACC..216AE8:690,694,7A4,348. Missing owner is rejected BEFORE any removal.
// Each actual50EN removal commits separately; progress records successful calls
// so a rejected later call can be retried without replaying earlier mutations.
// Canonical shape694.registrationE0 is cleared only with its real removal.
UndertrackResetResult50EW removeUndertrackCollisionOwners50EW(VehicleFrameState49W&,
    const UndertrackQueryOwners50EW&,ZoneLifecycle50CL&,LiveZoneWorld50EN&,
    LiveZoneWorkspace50EN&,UndertrackRegistryProgress50EW&,
    UndertrackResetDomain50EW=UndertrackResetDomain50EW::PlayerPostPackedBytes50S) noexcept;

//21690C..2169A0, after the separate REQUIRED visibility7F0 attach:690,7A4,348,
//694. Derives query matrices/spheres/segment endpoints from canonical scenes,
// native resources and main1E0. Actual120070 mode2 uses zero overlap radius.
// Each preparation+attach commits together; earlier successful owners survive
// a later rejection, tracked by progress.694 gains its actual nonzero E0 marker.
// Existing49W ownership guards are not bypassed: subsequent frames must bind
// their real registered694 preparation/update route through PodLiveQuery50IZ.
UndertrackResetResult50EW attachUndertrackCollisionOwners50EW(VehicleFrameState49W&,
    const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
    const UndertrackPlacement50ET&,const UndertrackQueryOwners50EW&,
    ZoneLifecycle50CL&,LiveZoneWorld50EN&,LiveZoneWorkspace50EN&,
    UndertrackRegistryProgress50EW&,
    UndertrackResetDomain50EW=UndertrackResetDomain50EW::PlayerPostPackedBytes50S) noexcept;

// All calls are serial with stable, correctly typed caller-owned storage and
// no active collection. Full descriptors/backing capacities, immutable source
// spans, all model result records, effects and workspace must be disjoint from
// writable frame/registry storage. The canonical shape is frame.shape, never
// a second body/shape copy. Use strict binary32 FE_TONEAREST/no contraction.
//
// SourceSliceComplete means ONLY the named operations completed. Visibility
//7F0, camera-scene/follow-query reset, the real ground collector, player prefix
//and optional height owners are not filled by success booleans or callbacks.
// No API here clears228BBC, changes49W phase, or reports2255B0 complete.
}
