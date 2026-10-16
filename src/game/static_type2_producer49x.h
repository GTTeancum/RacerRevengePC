#pragma once
#include "type2_resource49x.h"
#include "collision_normal_mode50ja.h"
#include "compound_collision50ij.h"
#include "spring_query48n.h"
#include "gate_trigger_action50kg.h"
#include "native_trigger_group50ga.h"
#include "native_trigger_box_geometry50ma.h"

namespace rr::game {
struct Type2PairMetadata49X {
    float fraction;
    std::uint32_t worldConverted;
    std::uint16_t queryTag;
    std::uint16_t reserved;
    std::uint32_t candidateIdentity,face;
};
struct Type2Selected49X {
    SpringVector4_48N first,second,normal,surfacePoint,correction;
    float fraction;
    std::uint8_t flags;
    std::uint8_t preserved[3];
    std::uint32_t candidateIdentity,face;
    std::uint16_t queryTag=0,reserved=0;
};
struct Type2StaticQuery49X {
    const Type2ResourceView49X* resource=nullptr;
    BodyMatrix48G worldMatrix{}; // already prepared; no scene/registration work
    const CollisionFilter48J* filter=nullptr;
    CollisionSphere48J sphere{};
    std::array<float,4> copied40{},copied50{};
    SpringSceneId48N queryScene{0};
    std::uint32_t skipMask=0,dynamicMask=0,materialMask=0;
    std::uint32_t initialCurrentCategory=0,initialQueryCurrentIdentity=0;
    std::uint8_t collisionTypeTag=0,groupTypeTag=0;
    bool callbackEnabled=true,resetRecordsBefore=true;
    // Explicit opt-in native Gate geometry policy; the live wrapper supplies
    // the preceding accepted per-racer matrix before preparing the current one.
    bool nativeGatePolicy=false,nativeGateHasPrevious=false;
    BodyMatrix48G nativeGatePreviousWorld{};
    // Actual shape+54. The retained producer admits only its existing mode0 seam.
    std::uint32_t normalMode54=0;
    bool normalMode54Bound=false; // legacy numeric-only calls publish no source owner
};
struct Type2StaticCandidate49X {
    SpringStaticCandidate48N candidate;
    const float* nodeRadii=nullptr;
    std::size_t nodeRadiusCount=0,maxDepth=0;
    // Prepared by prepareStaticTargetRadii49X; mesh and radii remain immutable.
    std::uint32_t shapeType=0;
    const CompoundCollisionView50IJ* compound=nullptr; // shape1 only; one callback owner
    // Only indexed50CC supports this bounded registered shape5 Gate owner.
    // Shared track lifetime, never per-racer copied or rolled back.
    GateTriggerBinding50KG* gate=nullptr;
    // Alternative shared native trigger set. Exactly one of gate/group is used
    // for shape5/6. Original action/reciprocal state lives in this shared owner.
    NativeTriggerGroup50GA* nativeTriggers=nullptr;
    std::size_t nativeTriggerIndex=0;
    NativeTriggerBox50MA nativeTriggerBox{}; // shape6 only, inline immutable geometry
};
struct Type2WorkItem49X { std::uint16_t queryTag,targetTag; };
struct Type2Visit49X {
    std::uint32_t candidateIdentity;
    std::uint16_t queryTag,targetTag;
    // 0=pruned/miss,1=leaf hit,2=query split,3=target split.
    std::uint8_t outcome;
};
struct Type2StaticWorkspace49X {
    SpringCumulativePair48N* pairs=nullptr; std::size_t pairCapacity=0;
    Type2PairMetadata49X* metadata=nullptr; std::size_t metadataCapacity=0;
    SpringRecord48N* records=nullptr; std::size_t recordCapacity=0;
    TerrainCandidateEffects48J* effects=nullptr; std::size_t effectCapacity=0;
    Type2WorkItem49X* stack=nullptr; std::size_t stackCapacity=0;
    // null/zero disables trace; a nonnull zero-capacity trace requests tracing.
    Type2Visit49X* trace=nullptr; std::size_t traceCapacity=0;
};
struct Type2StaticInitial49X {
    Type2Selected49X selected{};
    SpringRecordState48N records{};
    const SpringCumulativePair48N* pairs=nullptr;
    const Type2PairMetadata49X* metadata=nullptr;
    std::size_t pairSlots=0;
    const SpringRecord48N* recordSlots=nullptr; std::size_t recordCount=0;
};
struct Type2StaticDestination49X {
    SpringCumulativePair48N* pairs=nullptr; std::size_t pairCapacity=0;
    Type2PairMetadata49X* metadata=nullptr; std::size_t metadataCapacity=0;
    SpringRecord48N* records=nullptr; std::size_t recordCapacity=0;
    TerrainCandidateEffects48J* effects=nullptr; std::size_t effectCapacity=0;
};
struct Type2StaticResult49X {
    Type2Selected49X selected{};
    SpringRecordState48N records{};
    std::size_t pairCount=0,candidateCount=0,geometryHits=0,leafHits=0;
    std::size_t callbacks=0,appended=0,saturated=0,visits=0,traceUsed=0,stackHighWater=0;
    std::uint32_t currentCandidateIdentity=0;
    std::uint8_t queryFlagsF8=0;
    bool hasGeometryHit=false;
    CollisionNormalModeWrite50JA normalModeWrite{};
};
enum class Type2StaticStatus49X { Complete,InvalidInput,UnsupportedScope,InsufficientCapacity };
// Original type2/mode0 numerical seam, explicit ordered static0/compound1 owners and
// prepared affine matrices. Resources must be cold-prepared/validated and stay
// immutable. Compound1 keeps the parent SAT gate, active member order and one
// callback per owner. No discovery, scene update, registration, E8 or dynamic callbacks.
// Reset selected endpoints to(+0,+0,+0,1), fraction to1 and cumulative count to0; preserve other
// initial selected bytes and stale pair slots until original stores replace them.
// Complete commits declared initial slot prefixes and every candidate effect.
// Errors leave destinations/result untouched; scratch may change. Full declared
// writable capacities must be disjoint from each other and every nested input.
// Count/ray and cumulative tree-work bounds must be size_t-representable and
// are checked before scratch mutation. The source arithmetic contract is the
// retained ordinary-finite-intermediate model; this is not a universal checker
// for every original exceptional float/device behavior.
// No allocation, recursion, guest execution, threads, global mutable state.
Type2StaticStatus49X produceStaticType2Contacts49X(const Type2StaticQuery49X&,
    const Type2StaticCandidate49X*,std::size_t,const Type2StaticInitial49X&,
    const Type2StaticWorkspace49X&,const Type2StaticDestination49X&,
    Type2StaticResult49X&) noexcept;
}
