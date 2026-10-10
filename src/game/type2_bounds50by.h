#pragma once
#include "type2_resource49x.h"
#include "collection_bounds48s.h"
namespace rr::game{
struct Type2BoundsInput50BY{
 const Type2ResourceView49X* resource=nullptr;
 BodyMatrix48G preparedWorld{};
 // Explicit opaque cached words. Source type2 bounds preserves every bit.
 std::uint32_t cachedCenterWords[4]{},cachedRadiusWord=0;
};
// Original1202E0 tag2, including rootless first endpoint pair and shared tail.
// Does not change actual query type/state. Reuses the already proved common
// numeric bounds tail internally. Endpoint/matrix W words are not consumed.
// Rootful view uses its admitted runtime root; rootless view must be leaf0.
// Output/input/full declared source spans must be disjoint. Rejection preserves
// all output bytes. No positive-half/orthogonality assumption or hot allocation.
CollectionBoundsStatus48S prepareType2Bounds50BY(const Type2BoundsInput50BY&,
 CollectionBoundsResult48S&)noexcept;
}
