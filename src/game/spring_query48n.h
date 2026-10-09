#pragma once
#include "spring_callback48n.h"
#include "terrain_query48j.h"

namespace rr::game {
struct SpringQueryView48N {
    const LocalSegment48I& segment;
    const CollisionSphere48J& sphere;
    const CollisionFilter48J& filter;
    const std::array<float,4>& copied40;
    const std::array<float,4>& copied50;
    // Original endpoint fourth lanes are opaque bits, copied only after a hit.
    std::uint32_t firstW=0x3f800000u,secondW=0x3f800000u;
    SpringSceneId48N queryScene{0};
    std::uint32_t skipMask=0,dynamicMask=0,materialMask=0;
    std::uint32_t initialCurrentCategory=0;
    std::uint8_t collisionTypeTag=0,groupTypeTag=0;
    bool callbackEnabled=true,resetBefore=true;
    // In the supported static scope, query+F4 is unchanged when no
    // collision-tag candidate is enumerated. Group recursion is unsupported.
    std::uint32_t initialQueryCurrentIdentity=0;
};
struct SpringStaticCandidate48N {
    TerrainCandidate48J collision;
    bool hasMaterialScene=false;
    const char* sceneName=nullptr;
    std::size_t sceneNameBytes=0;
};
struct SpringQueryWorkspace48N {
    SpringRecord48N* records=nullptr;
    std::size_t recordCapacity=0;
    TerrainCandidateEffects48J* effects=nullptr;
    std::size_t effectCapacity=0;
    std::uint32_t* treeStack=nullptr;
    std::size_t treeStackCapacity=0;
};
struct SpringQueryResult48N {
    SpringRecordState48N state{};
    std::uint8_t queryFlagsF8=0;
    bool hasGeometryHit=false;
    std::uint32_t currentCandidateIdentity=0;
    struct { CollisionContact48I selected,nearestPair; } collision;
    SpringCumulativePair48N pair{};
    std::size_t candidateCount=0,geometryHits=0,callbacks=0,appended=0,saturated=0;
};
enum class SpringQueryStatus48N {
    Complete,InvalidInput,UnsupportedScope,UnsupportedInterpolationBoundary,InsufficientCapacity
};
// Original122660/121410 +294BB0/2963E0 for an EXPLICIT ordered candidate list,
// one endpoint pair, mode1, float triangle meshes, static type2 callback only.
// Per-hit callbacks see cumulative strict-nearest pair data; global selection
// uses <=. No world registration, detector ownership or live scheduling inferred.
// resetBefore performs296490 once before enumeration. Logical record saturation
// is successful original behavior. Workspace shortage never truncates the list.
// All storage is caller-owned. inputRecordCount slots (including stale bytes)
// are copied through scratch, and destination tails beyond that count are not
// touched. FULL declared scratch/destination capacities participate in alias
// validation, including unused tails. initialState capacity must fit inputRecordCount. Completed misses
// commit state/effects; errors leave every destination unchanged. Scratch may
// change on error. Writable spans must be disjoint from one another and all
// inputs, including nested mesh/filter/name storage. No allocations or threads.
SpringQueryStatus48N queryStaticSpringBounded48N(const SpringQueryView48N&,
    const SpringStaticCandidate48N*,std::size_t,
    const SpringRecordState48N& initialState,const SpringRecord48N* initialRecords,
    std::size_t inputRecordCount,const SpringQueryWorkspace48N&,
    SpringRecord48N* destinationRecords,std::size_t destinationRecordCapacity,
    TerrainCandidateEffects48J* destinationEffects,std::size_t destinationEffectCapacity,
    SpringQueryResult48N&) noexcept;
}
