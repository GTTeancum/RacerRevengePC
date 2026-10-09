#pragma once
#include "static_type2_producer49x.h"
namespace rr::game {
// Ordered borrowed owner indices, normally resolved from50CA's merged IDs by
//50CB. Avoids copying vector-owning candidate records in the hot path.
// Effects are emitted in list order. Repeated owner indices carry forward their
// earlier mutable effect state. Owner storage remains unchanged until caller
// commits effects in order. Other49X transactional/finite-input rules apply.
Type2StaticStatus49X produceIndexedStaticType2Contacts50CC(
    const Type2StaticQuery49X&,const Type2StaticCandidate49X* owners,std::size_t ownerCount,
    const std::size_t* orderedIndices,std::size_t count,const Type2StaticInitial49X&,
    const Type2StaticWorkspace49X&,const Type2StaticDestination49X&,
    Type2StaticResult49X&) noexcept;
}
