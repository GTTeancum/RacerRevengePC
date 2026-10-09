#pragma once
#include "terrain_query48j.h"
#include "terrain_prepare48m.h"

namespace rr::game {
struct TerrainQueryWorkspace48M {
    TerrainCandidateEffects48J* effects;
    std::size_t effectCapacity;
    std::uint32_t* treeStack;
    std::size_t treeStackCapacity;
};
struct TerrainQueryResult48M {
    PreparedTerrainState48J terrain;
    std::uint8_t queryFlagsF8=0;
    bool hasGeometryHit=false;
    struct { CollisionContact48I selected,nearestPair; } collision;
    std::size_t candidateCount=0;
};
enum class TerrainQueryStatus48M {
    Complete,InvalidInput,UnsupportedScope,UnsupportedInterpolationBoundary,InsufficientCapacity
};
// Non-owning view permits new numeric preparation without copying persistent
// filter vectors or mutating the caller's previous query object.
struct TerrainQueryView48M {
    const LocalSegment48I& segment;
    const CollisionSphere48J& sphere;
    const CollisionFilter48J& filter;
    const TerrainCallbackMasks48J& callbackMasks;
    const PreparedTerrainState48J& prepared;
    const std::array<float,4>& copied40;
    const std::array<float,4>& copied50;
    std::uint8_t collisionTypeTag,groupTypeTag;
    bool terrainCallbackEnabled;
};
TerrainQueryStatus48M queryTerrainViewBounded48M(const TerrainQueryView48M&,
    const TerrainCandidate48J*,std::size_t,const TerrainQueryWorkspace48M&,
    TerrainCandidateEffects48J*,std::size_t,TerrainQueryResult48M&) noexcept;
// Exact48J prepared limited-list semantics using caller-owned reusable scratch.
// Input vectors/meshes are persistent, read-only assets/metadata; no per-query
// allocation or filter copying occurs. Workspace and destination spans must be
// disjoint from each other and inputs. Capacity failure never truncates a list.
// Scratch may change on any outcome; destination effects/result only change on
// Complete, including a completed miss. Spatial registration/candidate production
// and unknown callbacks/group objects remain outside this numerical boundary.
TerrainQueryStatus48M queryTerrainLimitedBounded48M(const TerrainQuery48J&,
    const TerrainCandidate48J*,std::size_t,const TerrainQueryWorkspace48M&,
    TerrainCandidateEffects48J* destination,std::size_t destinationCapacity,
    TerrainQueryResult48M&) noexcept;
struct TerrainPreparedQueryResult48M {
    TerrainPrepareState48M state;
    TerrainQueryResult48M query;
    bool queryExecuted=false;
};
enum class TerrainPreparedQueryStatus48M {
    Queried,QueryDisabled,NoBody,InvalidInput,UnsupportedScope,
    UnsupportedInterpolationBoundary,InsufficientCapacity
};
// Compose numeric preparation with an EXPLICIT caller-provided ordered list.
// Does not execute or suppress the original spatial-registration system: that
// independent owner is outside this API. queryMetadata supplies only filters,
// callbacks/type tags and copied40/50; its numeric segment/sphere/state are ignored.
// NoBody/QueryDisabled return prepared state with queryExecuted=false, zero query
// result and unchanged destination effects. Errors leave all output untouched.
TerrainPreparedQueryStatus48M prepareAndQueryTerrain48M(const TerrainPrepareInput48M&,
    const TerrainQuery48J& queryMetadata,const TerrainCandidate48J*,std::size_t,
    const TerrainQueryWorkspace48M&,TerrainCandidateEffects48J*,std::size_t,
    TerrainPreparedQueryResult48M&) noexcept;

}
