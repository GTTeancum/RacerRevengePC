#pragma once
#include "terrain_query48j.h"

namespace rr::game {
// State touched or preserved at the original terrain preparation boundary.
// All storage is fixed-size. The collected array has the existing 16 slots;
// preparation clears only its count, not the slots or current candidate.
struct TerrainPrepareState48M {
    TerrainFollowState48G follow{};
    PreparedTerrainState48J terrain{};
    LocalSegment48I segment{};
    CollisionSphere48J sphere{};
    std::uint32_t auxiliaryAbove118=0;
};
struct TerrainPrepareInput48M {
    TerrainPrepareState48M state{};
    BodyMatrix48G scene{};
    // Component +140 is distinct from the collision shape's second endpoint.
    Vector3_48C lowerEndpoint140{};
    bool bodyLinked=true,queryEnabled100=true;
    bool auxiliarySource120=false,auxiliarySource124=false;
    std::uint32_t shapeType88=2,pairCount=1;
    bool hasPairTree=false;
};
enum class TerrainPrepareStatus48M {
    QueryReady,QueryDisabled,NoBody,UnsupportedScope,InvalidInput
};
// Numeric preparation only: 1B1800 prefix/matrix selection and the single-pair
// type-2 11F680 setter, stopping before 11FFA4 spatial registration. No world
// enumeration, candidates, auxiliary providers, contacts or spring detection.
// NoBody copies the input state unchanged. QueryDisabled performs the resets
// and full-scene lower-endpoint seed, preserving segment matrix and sphere.
// QueryReady also writes the selected matrix and local-pair midpoint sphere.
// Other statuses leave output untouched. Aliasing output with input.state is
// supported. Requires finite affine scenes and finite consumed intermediates,
// strict binary32 operations, round-to-nearest, no contraction/reassociation.
TerrainPrepareStatus48M prepareTerrainSinglePair48M(
    const TerrainPrepareInput48M&,TerrainPrepareState48M&) noexcept;
}
