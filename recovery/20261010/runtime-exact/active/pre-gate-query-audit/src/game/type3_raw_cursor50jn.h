#pragma once
#include "group_query48u.h"
namespace rr::game {
// Incremental borrowed numeric lease for full121920(a1=0). The wrapper owns
// iterator open/next ordering and canonical commits. Cold descriptors/backing
// must stay stable and disjoint from this cursor until finish or terminal error.
struct Type3RawCursor50JN {
    const Type3QueryInput48P* input=nullptr;
    const Type3QueryContext48P* context=nullptr;
    GroupQueryInput48U groups{};
    GroupQueryBuffers48U buffers{};
    std::uint32_t* treeStack=nullptr;std::size_t treeStackCapacity=0;
    Type3QueryResult48P result{};
    std::uint32_t vehicle7A0=0,callbackPhysicsBodyMask=0;
    Type3QueryStatus48P status=Type3QueryStatus48P::InvalidInput;
    bool started=false,active=false,finished=false;
};
// Full48U span/admission validation and one11DD00 preparation. The supplied
// outerOrder is ignored: no object dispatch occurs until consume. Destination
// is the mutable lease for subsequent calls, not an irrevocable canonical
// commit. Cursor must be a separate owner disjoint from ALL supplied spans.
Type3QueryStatus48P beginType3Raw50JN(
    const Type3QueryInput48P&,const Type3QueryContext48P&,const Type3QuerySource48P&,
    const GroupQueryInput48U&,const GroupQueryState48U&,const GroupQueryWorkspace48U&,
    const GroupQueryBuffers48U& destination,bool callbackIs20EE10,
    std::uint32_t callbackPhysicsBodyMask,
    std::uint32_t initialVehicle7A0,Type3RawCursor50JN&) noexcept;
// One wrapper-returned1-based raw reference. Direct object F4/F8 precede gates;
// Vegetation placement and callbacks finish synchronously before returning.
// Second pass admits only collision-tag objects. Any failure terminates this
// lease; partial scratch effects are retained for source ordering, never replayed.
Type3QueryStatus48P consumeType3Raw50JN(Type3RawCursor50JN&,
    GroupQueryRef48U object,bool collisionOnlySecondPass) noexcept;
// No extra11DD00 or dispatch. Type3 11E470 is a no-op. Outputs on Complete only.
Type3QueryStatus48P finishType3Raw50JN(Type3RawCursor50JN&,
    Type3QueryResult48P&,std::uint32_t& vehicle7A0) noexcept;
}
