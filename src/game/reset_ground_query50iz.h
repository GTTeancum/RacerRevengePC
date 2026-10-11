#pragma once
#include "candidate_collection50bx.h"
#include "collision_tree48i.h"
#include "reset_candidate50am.h"

namespace rr::game {
// One actual216EA8/216F04 invocation of124CB0: staticOnly=1,
// exclusionCount=0, method mask3, fallback enabled. The temporary280890
// owner has next=0 and zone=the actualC34 (216E84/216E88). This is not a
// vehicle's overlap chain or a HOST all-world scan.
struct ResetGroundInput50IZ {
    SpringVector4_48N start{},end{};
    CollectionRef50BX zoneC34=0;
    // Bound native projection of the actual optional global zone pointer.
    // *owner==0 is authentic absence; owner==nullptr is unbound and rejects.
    const CollectionRef50BX* globalFallbackZone443108=nullptr;
    std::uint32_t floorMask=0;
    const std::uint8_t* directTypeTagGp8010=nullptr;
    // Same application-wide owner written by121438/126FA0.124CB0/124580
    // preserve it. A null pointer is unbound, never an assumed flat mode.
    const std::uint32_t* normalMode3DF5B0=nullptr;
};
struct ResetGroundGeometry50IZ {
    // Index corresponds to the SAME world object Ref-1. Null is permitted
    // for an object which never reaches an enabled shape0 narrowphase.
    // The candidate's id must match the corresponding object's identity.
    const CollisionCandidate48I* shape0=nullptr;
};
struct ResetGroundWorld50IZ {
    const CollectionObject50BX* objects=nullptr;
    const ResetGroundGeometry50IZ* geometry=nullptr;
    std::size_t objectCount=0;
    const CollectionTree50BX* methods=nullptr;std::size_t methodCount=0;
    const CollectionZone50BX* zones=nullptr;std::size_t zoneCount=0;
};
// Reuse the actual collection source-state owners. Only flagsF8 and the
// SearchList cursor/flags16 fields can change; epochs/cache/pool data survive.
// If these are live projections rather than direct owners, caller must commit
// accepted flagsF8 back to the SAME original collision owners before continuing.
struct ResetGroundMutable50IZ {
    CollectionObjectState50BX* objects=nullptr;std::size_t objectCapacity=0;
    CollectionTreeState50BX* methods=nullptr;std::size_t methodCapacity=0;
};
struct ResetGroundWorkspace50IZ {
    CollectionObjectState50BX* objects=nullptr;std::size_t objectCapacity=0;
    CollectionTreeState50BX* methods=nullptr;std::size_t methodCapacity=0;
    std::uint32_t* meshStack=nullptr;std::size_t meshStackCapacity=0;
};
struct ResetGroundResult50IZ {
    // Caller supplies actual prior output-point/output-normal words. On a
    // complete miss they remain bit-preserved; end becomes the supplied end.
    // Every hit writes point/normal then copies point over the segment end.
    SpringVector4_48N end{},point{},normal{};
    bool hit=false;
    // Native observations only; no original field is replaced by these.
    std::size_t visited=0,eligible=0,queried=0,hits=0;
    CollectionRef50BX lastHitObject=0,finalZone=0;
    SpringVector4_48N searchCenter{},searchHalf{};
};
enum class ResetGroundStatus50IZ : std::uint8_t {
    Complete,Storage,Binding,RoundingMode,Arithmetic,Topology,Capacity,
    UnsupportedMethod,UnsupportedShape,Narrowphase
};
struct ResetGroundDiagnostic50IZ {
    ResetGroundStatus50IZ status=ResetGroundStatus50IZ::Complete;
    std::uint32_t sourcePC=0;
    CollectionRef50BX object=0,method=0,zone=0;
    ResetTreeStatus50AK narrowphase=ResetTreeStatus50AK::Complete;
};
// Exact294600/294740/294B40 SearchList route, then124CB0's type/category
// eligibility, F8 bit4 effect and124580 shape0 leaf. Groups are genuinely
// skipped by this call's staticOnly=1; no query-object122350 filtering occurs.
// Preserve raw order/repeats/deletions, the one-new-method read rule (an empty
// second method ends this call), and close BOTH distinct final-zone methods.
// No object-epoch deduplication is present in the SearchList source path.
// Reached enabled shape1/other shapes and non-SearchList methods reject;
// flags31-disabled candidates retain their genuine early no-hit return.
//
// On Complete, mutable states and result commit together, even for a real
// miss. All other statuses preserve them; scratch may change. This bounded
// atomic query must not be reported as a miss or a source-completed call on
// rejection. Diagnostic reports the unrepresented/rejected source boundary.
// Serial, stable current world/geometry/cache publication; no open collector,
// registration mutation, callbacks, heap, scene refresh, or source globals
// changed by this API. FE_TONEAREST/separate binary32 arithmetic and inherited
//50AM numeric/topology/interpolation boundaries. Full mutable/scratch/result/
//diagnostic spans and descriptors are disjoint from one another and all read
//inputs, including borrowed list and mesh arrays. Read-only spans may alias.
ResetGroundStatus50IZ queryResetGround50IZ(const ResetGroundInput50IZ&,
    const ResetGroundWorld50IZ&,const ResetGroundMutable50IZ&,
    const ResetGroundWorkspace50IZ&,ResetGroundResult50IZ&,
    ResetGroundDiagnostic50IZ&) noexcept;
} // namespace rr::game
