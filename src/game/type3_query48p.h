#pragma once
#include "type3_state48p.h"
#include "collision_normal_mode50ja.h"
#include "compound_collision50ij.h"
#include "spring_type3_callback48p.h"
#include "terrain_query48j.h"

namespace rr::game {
enum class Type3QueryEntry48P { LimitedList,Dispatcher };
struct Type3QueryInput48P {
    Type3QueryState48P initial{};
    SpringRecordState48N spring{};
    CollisionFilter48J filter{};
    std::array<float,4> copied40{},copied50{};
    // Explicit fixture/caller constant; no value for BSS3E02C0 is inferred.
    SpringVector4_48N sentinel{};
    BodyMatrix48G bodyScene{};
    std::uint32_t pairCount=0,normalMode=0,initialQueryCurrent=0,initialSpringCurrentCategory=0;
    std::uint32_t skipMask=0,dynamicMask=0,materialMask=0;
    std::uint8_t collisionTypeTag=0,groupTypeTag=0;
    bool callbackEnabled=true,bodyScenePresent=true,resetSpring=false;
    Type3QueryEntry48P entry=Type3QueryEntry48P::LimitedList;
    bool normalModeBound50JA=false; // explicit source shape+44 binding
};
// 48Q additive composition seam: prepared scalars are separate from the borrowed
// immutable template's owning filter vectors. No filter clone/growth is needed.
struct Type3QueryContext48P {
    SpringRecordState48N spring{};
    BodyMatrix48G bodyScene{};
    std::uint32_t pairCount=0;
    bool resetSpring=false,bodyScenePresent=true;
    Type3QueryEntry48P entry=Type3QueryEntry48P::LimitedList;
};
struct Type3QuerySource48P {
    Type3RaySource48P rays{};
    const SpringVector4_48N* endpoints=nullptr;std::size_t endpointCount=0;
    const std::uint32_t* sceneMetadata=nullptr;std::size_t sceneMetadataCount=0;
    const SpringRecord48N* records=nullptr;std::size_t recordCount=0;
};
struct Type3Candidate48P {
    TerrainCandidate48J collision{};
    std::uint32_t shapeType=0; // Distinct from collision-list byte tags/categories.
    bool hasMaterialScene=false;
    const char* sceneName=nullptr;std::size_t sceneNameBytes=0;
    const CompoundCollisionView50IJ* compound=nullptr; // shape1 only; one outer owner
};
struct Type3QueryBuffers48P {
    Type3RayBuffers48P rays{};
    SpringRecord48N* records=nullptr;std::size_t recordCapacity=0;
    TerrainCandidateEffects48J* effects=nullptr;std::size_t effectCapacity=0;
};
struct Type3QueryWorkspace48P {
    Type3QueryBuffers48P buffers{};
    std::uint32_t* treeStack=nullptr;std::size_t treeStackCapacity=0;
};
struct Type3QueryResult48P {
    Type3QueryState48P state{};
    SpringRecordState48N spring{};
    std::uint32_t currentCandidate=0;
    std::uint8_t queryFlags=0;
    bool hasAnyHit=false;
    std::size_t candidateCount=0,geometryHits=0,callbacks=0,appended=0,saturated=0;
    CollisionNormalModeWrite50JA normalModeWrite{};
};
enum class Type3QueryStatus48P {
    Complete,InvalidInput,UnsupportedScope,UnsupportedInterpolationBoundary,InsufficientCapacity
};
// Exact supported projections of original122660 + type3 preparation126F40 and
// static294BB0 callback. First scope: positive pair count, zero endpoint scene
// metadata, float mesh shape0 or ordered compound1, flat/mode1 normals.
// Compound1 preserves stale localCenter, cumulative fractions/hits and repeated
// world conversion after every member, including misses; active is not consulted.
// Other unhandled shape tags preserve original miss resets. Higher normal modes reject only if interpolation is actually needed.
// Reached meshes require reverse_mode=0 and, when populated, a vertex-normal
// array covering exactly the float vertex array; other shapes remain unadapted.
// Per-candidate result resets/strict comparisons and stale selected transforms
// are retained. This is not the cumulative type2 query and no world is discovered.
// Dispatcher entry requires exactly one candidate, bypasses preparation/filter
// and callbacks, and projects the direct126F40 state transition.
// All supplied source ray slots/lengths/records are copied through scratch to
// destination; unused declared destination tails remain unchanged. Full declared
// mutable spans must be mutually disjoint and avoid all inputs, including nested
// filters/meshes/names. Errors may change scratch only. Every Complete (including
// misses) commits destination arrays/effects/result together. No heap/thread work.
// Mesh topology must already satisfy importer validation (a rooted tree with no
// shared or unreachable records). The visit limit bounds reached work/cycles;
// it is not a replacement for complete mesh topology validation.
// Candidate-local working XYZ is native numeric scratch; original unused local
// W register carry is deliberately not represented. All game-facing Vec4 lanes
// and copied record bytes are retained. Ordinary finite binary32 scope only.
Type3QueryStatus48P queryStaticType3Bounded48P(const Type3QueryInput48P&,
    const Type3QuerySource48P&,const Type3Candidate48P*,std::size_t,
    const Type3QueryWorkspace48P&,const Type3QueryBuffers48P& destination,
    Type3QueryResult48P&) noexcept;
// Same operation and full-span transactional contract; these six execution
// fields come exclusively from context. All other fields, especially filters,
// are borrowed from the immutable template. Context must avoid mutable spans.
Type3QueryStatus48P queryStaticType3WithContextBounded48P(const Type3QueryInput48P&,
    const Type3QueryContext48P&,const Type3QuerySource48P&,const Type3Candidate48P*,std::size_t,
    const Type3QueryWorkspace48P&,const Type3QueryBuffers48P& destination,
    Type3QueryResult48P&) noexcept;
}
