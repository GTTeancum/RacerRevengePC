#pragma once
#include "collision_tree48i.h"
#include "vehicle_contact48g.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rr::game {
struct CollisionIdentityList48J {
    std::int32_t mode=1;
    std::vector<std::uint32_t> identities;
};
struct CollisionFilter48J {
    std::uint32_t identity=0,categoryA4=0,acceptMaskB8=0xffffffffu;
    CollisionIdentityList48J identityLists[2]; // BC/C0/C4 and C8/CC/D0
    std::int32_t categoryModeD4=1;
    std::vector<std::uint32_t> categoryMasks;
    std::uint8_t flagsF8=0,flags31=0;
};
struct CollisionSphere48J { Vector3_48C center;float radius; };
struct TerrainCandidate48J {
    CollisionCandidate48I geometry;
    CollisionFilter48J filter;
    CollisionSphere48J sphere;
    std::uint32_t ownerIdentity9C=0;
    std::uint8_t typeTag=0;
    bool hasResponseCallbackE8=false;
    std::array<float,4> copied40{},copied50{};
};
struct TerrainCallbackMasks48J {
    std::uint32_t collect=0,forceAngle=0,excludeOwner=0;
};
struct PreparedTerrainState48J {
    TerrainSurface48G surface;
    std::array<std::uint32_t,16> collectedIdentities{};
    std::uint32_t collectedCount=0,excludedOwnerIdentity=0,currentCandidateIdentity=0;
};
struct TerrainQuery48J {
    LocalSegment48I segment;
    CollisionSphere48J sphere;
    CollisionFilter48J filter;
    TerrainCallbackMasks48J callbackMasks;
    PreparedTerrainState48J prepared;
    std::array<float,4> copied40{},copied50{};
    std::uint8_t collisionTypeTag=0,groupTypeTag=0;
    bool terrainCallbackEnabled=true;
};
struct TerrainCandidateEffects48J {
    std::uint32_t identity=0;
    std::uint8_t flagsF8=0;
    std::array<float,4> copied40{},copied50{};
};
struct TerrainQueryResult48J {
    PreparedTerrainState48J terrain;
    std::uint8_t queryFlagsF8=0;
    bool hasGeometryHit=false;
    struct { CollisionContact48I selected,nearestPair; } collision;
    std::vector<TerrainCandidateEffects48J> candidates;
};
enum class TerrainQueryStatus48J { Complete,InvalidInput,UnsupportedScope,UnsupportedInterpolationBoundary };
// Original prepared limited-list phase: 122660 / 120E70 / 126EB0,
// identified 1B0E50 terrain callback, then 1B19B4 surface selection.
// Input preparation (1B1800), world enumeration, group objects and arbitrary
// response callbacks are separate. No guest addresses: identities are native IDs.
// Complete writes all state even on a geometric miss; every other status leaves
// output unchanged. Candidate order and list contents are preserved.
TerrainQueryStatus48J queryTerrainLimited48J(const TerrainQuery48J&,
    const TerrainCandidate48J*,std::size_t,TerrainQueryResult48J&) noexcept;
}
