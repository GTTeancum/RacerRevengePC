#pragma once
#include "body_scene48g.h"
#include "spring_records48n.h"
#include <cstddef>
#include <cstdint>

namespace rr::game {
struct CollectionBoundsInput48S {
    std::uint32_t shapeTag=1;
    BodyMatrix48G shapeWorld{};
    SpringVector4_48N localCenter{},localHalf{},cachedCenter{};
    float cachedRadius=0;
    std::int32_t pairCount=0;
    const SpringVector4_48N* endpoints=nullptr;std::size_t endpointCount=0;
    const std::uint32_t* endpointScenes=nullptr;std::size_t endpointSceneCount=0;
    const BodyMatrix48G* scenes=nullptr;std::size_t sceneCount=0;
};
struct CollectionBoundsResult48S {
    SpringVector4_48N center{};
    Vector3_48C half{};
    SpringVector4_48N cachedCenter{};
    float cachedRadius=0;
};
enum class CollectionBoundsStatus48S {
    Complete,UnsupportedBounds,InvalidInput,InsufficientCapacity,NonFinite
};
// Bounded original1202E0 tags1/6 and3. Tags1/6 use shapeWorld/localCenter/
// localHalf and preserve every cachedCenter/cachedRadius bit. Tag3 requires
// pairCount>0 and consumes two complete endpoint vectors per pair. Optional
// endpointScenes absent (nullptr,0) means all-zero source metadata; otherwise
// it covers every consumed endpoint. Zero means no transform, n>0 selects
// scenes[n-1]. Shared indices represent shared scene identity. Only the cached
// scene+100 matrix is consumed, never flags, parent, refresh, or live ownership.
// Tag3 recomputes cachedCenter/cachedRadius and then applies the original common
// identity bounds tail: returned center can differ from cachedCenter at -0.
// Original output half.W is untouched and is therefore absent from this API.
// Unsupported tags reject; pairCount<=0 is outside the finite native domain.
//
// Full declared array spans, including inactive tails, must be aligned,
// representable and backed. All read-only spans may alias; the entire result
// must be disjoint from the input and every declared span. Sources are unchanged;
// any rejection preserves every result byte. Only reached numeric XYZ/matrix
// XYZ lanes and intermediates must be finite. W lanes and inactive values remain
// raw. No orthogonality, normalization, positive-half or affine-W restriction.
// Strict separately rounded binary32, round-to-nearest, no contraction; no claim
// about all physical EE/VU exceptional modes. Serial, constant stack, no dynamic
// allocation, caller-owned capacities, no guest addresses or retained pointers.
CollectionBoundsStatus48S prepareCollectionBounds48S(
    const CollectionBoundsInput48S&,CollectionBoundsResult48S&) noexcept;
}
