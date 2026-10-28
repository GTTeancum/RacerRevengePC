#pragma once
#include "static_type2_producer49x.h"
#include "candidate_collection50bx.h"
namespace rr::game {
// Pure existing native numerical policy for126EB0 and120E70, separated from
// owner publication and narrowphase. No scene/cache, flags, callbacks, normal
// mode or result binding is touched. False sphere status leaves output alone;
// unsupported rounding/nonfinite domain is not an inferred sphere rejection.
bool evaluateType2CandidateSphere50CC(const CollisionSphere48J&,const CollisionSphere48J&,
    bool& overlaps) noexcept;
bool passType2CandidateFilter50CC(const CollisionFilter48J&,const CollisionFilter48J&) noexcept;

// Borrowed immutable native filter lists, never a copied vector owner. The
// caller authenticates identity/F8 and const list custody against its target.
struct Type2BorrowedTargetFilter50CC {
    const CollectionFilter50BX* lists=nullptr;
    std::uint32_t identity=0;
    std::uint8_t flagsF8=0;
};
// False means invalid bounded storage (output untouched), not filter rejection.
// Successful evaluation writes pass with the same120E70 bilateral semantics.
// No allocation, owner lookup, flags write or category/default substitution.
bool passType2BorrowedCandidateFilter50CC(const CollisionFilter48J&,
    const Type2BorrowedTargetFilter50CC&,bool& pass) noexcept;

// Ordered borrowed owner indices, normally resolved from50CA's merged IDs by
//50CB. Avoids copying vector-owning candidate records in the hot path.
// Effects are emitted in list order. Repeated owner indices carry forward their
// earlier mutable effect state. Static0 and original compound1 owners share
// this indexed path: parent-box gate, active part order, cumulative pairs and
// exactly one callback/effect per outer index. Numerical owner/effect storage
// stays unchanged until the caller commits. Other49X numerical rules apply.
// Opt-in shape5 Gate uses the explicitly native segment/swept-surface policy.
// Its one uniquely bound GateTriggerBinding is a separately guarded writable
// owner: genuine hits commit E8 action state immediately, after E4's TRIGGER
// skip, even if a later candidate fails. Actions are never rolled back/replayed.
// External callback context must be stable, disjoint from numerical query
// storage and mutate only its admitted action owner; callbacks must not throw.
// The GC context activates sequence0 only. Visual/collision pose changes occur
// together in the later shared world-animation service, outside this query.
Type2StaticStatus49X produceIndexedStaticType2Contacts50CC(
    const Type2StaticQuery49X&,const Type2StaticCandidate49X* owners,std::size_t ownerCount,
    const std::size_t* orderedIndices,std::size_t count,const Type2StaticInitial49X&,
    const Type2StaticWorkspace49X&,const Type2StaticDestination49X&,
    Type2StaticResult49X&) noexcept;

// Internal actual121920/121E68 continuation. The world caller has already
// performed the applicable outer admission and canonical-owner state updates.
// Only the direct type-tag, sphere and bilateral-filter gates are bypassed.
// A nonnull previous is the last completed numerical state (or the caller's
// explicit11DD00 initialization). Initial slot spans contain its committed
// pairs, metadata and records; candidate carries the current canonical effects.
// currentCategory is the callback's retained category, separate from identity.
// targetWorldOverride is the current source-prepared shape0 world, when
// present. The caller authenticates it against its canonical world owner.
struct Type2WorldContinuation50CC {
    const Type2StaticResult49X* previous=nullptr;
    std::uint32_t currentCategory=0;
    const BodyMatrix48G* targetWorldOverride=nullptr;
    // Canonical current F8/copy40/copy50 effect, including prior visits in
    // this SAME world call. Identity must equal the actual candidate owner.
    const TerrainCandidateEffects48J* currentEffect=nullptr;
};
// One admitted static0/compound1 candidate, with the existing static callback
// and reached E8 guard. No synthetic callbacks or native Gate-policy extension.
// Previous selected/pair/record state and cumulative counters survive. Trace
// storage and traceUsed describe this visit only; slot/effect outputs commit
// only on Complete, as in the legacy entry. All inputs/outputs are disjoint.
Type2StaticStatus49X continueType2WorldCandidate50CC(
    const Type2StaticQuery49X&,const Type2StaticCandidate49X&,
    const Type2StaticInitial49X&,const Type2StaticWorkspace49X&,
    const Type2StaticDestination49X&,const Type2WorldContinuation50CC&,
    Type2StaticResult49X&,std::uint32_t& currentCategory) noexcept;
//Explicit native source-prefix contract, NOT a complete candidate/E4/E8 result.
//On a hit, identical pure geometry, cumulative pairs, query hit bits and target
//copy40/50 effects are produced BEFORE callbacks. The caller must publish these
//actual outputs, then execute each reached callback in original order. No query
//parameter, callbackEnabled or dynamicMask is rewritten to reach this boundary.
struct Type2WorldGeometryReached50CC {
    bool hit=false,queryE4Pending=false,targetE8Pending=false;
};
struct Type2WorldGeometryOutput50CC {
    Type2StaticResult49X numerical{};
    std::uint32_t retainedCategory=0;
    Type2WorldGeometryReached50CC reached{};
};
enum class Type2WorldGeometryPhase50CC { Rejected,GeometryPrefixReady };
struct Type2WorldGeometryReceipt50CC {
    Type2WorldGeometryPhase50CC phase=Type2WorldGeometryPhase50CC::Rejected;
    Type2StaticStatus49X numericalStatus=Type2StaticStatus49X::InvalidInput;
};
//One already-admitted direct shape0 native mesh target only (no groups,
//compound/trigger policy or callbacks). Same prior cumulative owner inputs as
//the full continuation. Failure preserves every output/destination byte;
//GeometryPrefixReady never proves E4/E8 completion or any source caller return.
Type2WorldGeometryReceipt50CC prepareType2WorldGeometryPrefix50CC(
    const Type2StaticQuery49X&,const Type2StaticCandidate49X&,
    const Type2StaticInitial49X&,const Type2StaticWorkspace49X&,
    const Type2StaticDestination49X&,const Type2WorldContinuation50CC&,
    Type2WorldGeometryOutput50CC&) noexcept;

// Geometry-only variant for an actual direct target with immutable borrowed
// lists. Candidate is an ephemeral numerical carrier: its scalar filter fields
// must exactly match borrowed; its vector lists must be empty and are NEVER
// treated as target list authority. The preceding outer gate uses borrowed.
// All borrowed capacities participate in the same output/scratch alias checks.
Type2WorldGeometryReceipt50CC prepareType2BorrowedWorldGeometryPrefix50CC(
    const Type2StaticQuery49X&,const Type2StaticCandidate49X&,
    const Type2BorrowedTargetFilter50CC&,const Type2StaticInitial49X&,
    const Type2StaticWorkspace49X&,const Type2StaticDestination49X&,
    const Type2WorldContinuation50CC&,Type2WorldGeometryOutput50CC&) noexcept;

}
