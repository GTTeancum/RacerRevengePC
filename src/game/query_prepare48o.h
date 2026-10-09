#pragma once
#include "body_scene48g.h"
#include "spring_records48n.h"
#include <cstddef>
#include <cstdint>

namespace rr::game {
struct QueryPairStagingInput48O {
    bool shapeLinked=true;
    std::uint32_t pairCount=0;
    const SpringVector4_48N* source=nullptr;
    std::size_t sourceCapacity=0; // Vec4 entries, not pairs.
    const std::uint32_t* sourceMetadata=nullptr;
    std::size_t sourceMetadataCapacity=0; // Optional per-Vec4 words.
};
enum class QueryPairStagingStatus48O { Staged,NoShape,InvalidInput,InsufficientCapacity };
// Original11CA80 sets tag3 and copies 2*pairCount complete Vec4 bit patterns.
// Missing sourceMetadata writes zero to each used destination metadata word.
// The untouched tail is retained. No sphere, matrix, registration or query runs.
// Null shape sets tag3 without consuming count/spans; zero pairs likewise uses
// no spans. For nonzero count, all declared spans must be aligned, representable
// and backed; mutable spans must be mutually disjoint and disjoint from source
// spans, input and shapeType. Read-only source spans may overlap one another.
// A count whose doubled value exceeds INT32_MAX is invalid. This is a native
// safety domain, not an assertion that malformed original metadata was safe.
// Any rejection is transactional. Capacity is caller-provided: no gameplay cap.
QueryPairStagingStatus48O stageQueryPairs48O(const QueryPairStagingInput48O&,
    SpringVector4_48N* destination,std::size_t destinationCapacity,
    std::uint32_t* destinationMetadata,std::size_t destinationMetadataCapacity,
    std::uint32_t& shapeType) noexcept;

struct QuerySpherePrepareInput48O {
    BodyMatrix48G preparedScene{};
    SpringVector4_48N first{},second{};
    Vector3_48C treeCenter{},treeHalf{};
    std::uint32_t shapeType=2,pairCount=1;
    bool shapeLinked=true,scenePrepared=true,hasPairTree=false;
};
struct QuerySpherePrepareOutput48O {
    BodyMatrix48G world{};
    SpringVector4_48N center{};
    float radius=0;
};
enum class QuerySpherePrepareStatus48O { Ready,UnsupportedScope,InvalidInput };
// Original11FF60 ->11FE50 ->prepared scene28B550 ->type2 setter11F680;
// boundary is BEFORE11FF74 calls registration120230. Tree root center/half or
// the sole float endpoint pair supplies the local sphere. W endpoint lanes and
// unused branch data are not consumed. Only center is transformed; radius is
// unscaled even for nonorthogonal affine matrices. Output center.W is 1.
// Strict ordinary binary32 finite affine domain; nonfinite consumed inputs or
// intermediate overflow reject transactionally. Dirty scenes and other shape
// types are unsupported. No allocations, guest pointers or fixture dependency.
QuerySpherePrepareStatus48O prepareQuerySphere48O(
    const QuerySpherePrepareInput48O&,QuerySpherePrepareOutput48O&) noexcept;
}
