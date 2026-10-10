#pragma once
#include "candidate_collection50bx.h"
#include "type3_raw_cursor50jn.h"

namespace rr::game {
struct FullMovementWorld50JN {
    CandidateCollectionInput50BX collection{};
    CandidateCollectionState50BX initial{};
    CandidateCollectionWorkspace50BX workspace{};
    CandidateCollectionBuffers50BX destination{};
    GroupQueryInput48U groups{};
    GroupQueryState48U groupInitial{};
    GroupQueryWorkspace48U groupWorkspace{};
    GroupQueryBuffers48U groupDestination{};
};
struct FullMovementCall50JN {
    bool callbackIs20EE10=false;
    // Actual global3E0718 PHYSICSBODY for20EE10. input.dynamicMask is the
    // same named binding for294BB0;126F40 itself does not consume that field.
    std::uint32_t callbackPhysicsBodyMask=0,initialVehicle7A0=0;
};
enum class FullMovementStatus50JN {Complete,Binding,Iterator,Numeric};
struct FullMovementResult50JN {
    FullMovementStatus50JN status=FullMovementStatus50JN::Binding;
    CandidateCollectionStatus50BX iteratorStatus=CandidateCollectionStatus50BX::InvalidInput;
    Type3QueryStatus48P numericStatus=Type3QueryStatus48P::InvalidInput;
    std::uint32_t sourcePC=0,vehicle7A0=0;
    CollectionRef50BX activeTree=0;
    SpringVector4_48N query70{};
    float query84=0;
    Type3QueryResult48P query{};
    bool secondPass=false;
};
// Exact enabled one-pair, mode0,219210 ->121920(a1=0) physical projection.
// Uses actual E0 zone links and the PRESENT canonical fallback; no precollected
// static list. Each294740 return is dispatched before requesting the next.
// The second pass is source-conditional, CollisionObject-only, without fallback.
// No294B40/265C60 close is performed:121920 has no such call.
//
// Both destination families are borrowed scratch leases, not canonical owners.
// On failure they may contain a source-ordered partial prefix and must NOT be
// committed or replayed. Complete permits the owner's joint commit: collection
// stamps/cursors/pool, numeric objects, numeric shapes, rays, query/70/84/7A0,
// and normal-mode receipt. Collection shape0States are never authoritative here.
// Separate family spans and every descriptor must remain stable and disjoint
// through this call; the canonical cold owner establishes that cross-family
// lifetime contract. Each reused family also retains its local span checks.
//
// Original floating return f20 is ignored by219210 and deliberately absent.
// Its diagnostic lazy3E0860 writer and virtual margin outputs are excluded;
// no admitted consumer may observe those omitted diagnostic/cache-only lanes.
// The overwritten84 distance arithmetic has no retained a1=0 effect. Retained
//70 is copied from pre-11DD00 query60 and changed to the source sentinel on hit.
// Disabled queries, null E0 requiring443148, nonzero mode/pair metadata, reached
// dynamic20EE10 bodies and response callbacks retain explicit boundaries.
FullMovementResult50JN queryFullMovement50JN(const FullMovementCall50JN&,
    const Type3QueryInput48P&,const Type3QueryContext48P&,const Type3QuerySource48P&,
    const FullMovementWorld50JN&) noexcept;
}
