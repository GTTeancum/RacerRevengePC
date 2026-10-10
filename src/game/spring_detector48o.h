#pragma once
#include "query_prepare48o.h"

namespace rr::game {
struct SpringDetectorPoint48O {
    SpringVector4_48N local{};
    std::uint32_t sceneIndex=0; // Index in caller-owned scene-state span.
};
struct SpringDetectorScene48O {
    BodyMatrix48G world{},local{};
    std::uint32_t flags=1; // Original+190 byte; upper bytes preserved.
    bool parentPresent=false;
};
struct SpringDetectorState48O {
    SpringVector4_48N center{},half{};
    SpringRecordState48N spring{};
    std::uint32_t queryType=3;
};
struct SpringDetectorInput48O {
    SpringDetectorState48O state{};
    bool springPresent=true,pairsPresent=true,providerPresent=false,providerEnabled=false;
    bool queryPresent=true,queryEnabled=true,resetSpring=true,explicitList=true,shapePresent=true;
    std::uint32_t shapePairCount=0;
};
struct SpringDetectorSource48O {
    const SpringDetectorPoint48O* points=nullptr;std::size_t pointCapacity=0;
    const SpringDetectorScene48O* scenes=nullptr;std::size_t sceneCount=0;
    // Flat Vec4 span: first,second for every pair. Both counts are Vec4 counts.
    const SpringVector4_48N* pairs=nullptr;std::size_t pairVectorCount=0;
    const SpringVector4_48N* staged=nullptr;std::size_t stagedVectorCount=0;
    const std::uint32_t* stagedMetadata=nullptr;std::size_t stagedMetadataCount=0;
};
struct SpringDetectorBuffers48O {
    SpringDetectorScene48O* scenes=nullptr;std::size_t sceneCapacity=0;
    SpringVector4_48N* pairs=nullptr;std::size_t pairVectorCapacity=0;
    SpringVector4_48N* staged=nullptr;std::size_t stagedVectorCapacity=0;
    std::uint32_t* stagedMetadata=nullptr;std::size_t stagedMetadataCapacity=0;
};
enum class SpringDetectorStatus48O {
    NoSpring,QueryDisabled,RegistrationBoundary,LimitedQueryBoundary,WorldQueryBoundary,
    InvalidInput,UnsupportedScope,InsufficientCapacity
};
// Numeric295630 through a precise boundary, never registration/query execution.
// With pairs: transforms persistent local points, updates second endpoints/AABB,
// optionally resets spring, then (only when query enabled) fills first endpoints
// and stages the original TYPE3 shape via11CA80. This is not a48N type2 adapter.
// Without pairs: only initial type3 no-op preparation reaches11FF74 registration
// boundary; type2 sphere preparation is a separate query_prepare48o API/proof.
// Prepared scenes ignore parent presence. Dirty parentless scenes refresh world
// from local and set flag bit0. Dirty parented scenes/provider calls unsupported.
// Point count is the original signed interpretation of spring.pointCount7C;
// active pair prep requires positive count in the finite arithmetic domain.
// Shape count is independent; adequate initialized backing permits stale tails.
// Full declared writable spans must be disjoint from one another and every
// input span/descriptor. Scratch may change on error; destinations/state commit
// only on the five non-error statuses. Every supplied source array is projected
// into its destination, including unchanged slots; extra destination tails stay
// unchanged. Null pointers require zero corresponding count/capacity.
// Strict ordinary finite binary32 arithmetic, with signed-zero scalar MIN/MAX
// ordering; local/matrix W bits are ignored in point XYZ and preserved in cache.
// Fixed stack, caller-owned variable capacity, no allocation, threads or guest
// addresses. No saved live detector ownership or world registration established.
SpringDetectorStatus48O prepareSpringDetector48O(const SpringDetectorInput48O&,
    const SpringDetectorSource48O&,const SpringDetectorBuffers48O& scratch,
    const SpringDetectorBuffers48O& destination,SpringDetectorState48O&) noexcept;
}
