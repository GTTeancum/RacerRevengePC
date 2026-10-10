#pragma once
#include "candidate_collection50bx.h"
#include "terrain_query48j.h"
namespace rr::game {
struct CollectionProjection50CF {
    bool valid=false;
    CollectionObject50BX object{};
    CollectionObjectState50BX state{};
};
// Borrow immutable filter lists from a live native owner; no vector copy or
// allocation. Caller preserves owner/list lifetime and supplies raw cache W.
// This projects a direct object only; group ownership/geometry stays explicit.
CollectionProjection50CF projectCollectionObject50CF(const CollisionFilter48J&,
    const CollisionSphere48J&,std::uint8_t typeTag,std::uint32_t shapeTag,
    std::uint32_t cachedCenterW)noexcept;
}
