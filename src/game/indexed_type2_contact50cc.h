#pragma once
#include "static_type2_producer49x.h"
namespace rr::game {
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
}
