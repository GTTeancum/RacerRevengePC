#pragma once
#include "collection_bounds48s.h"
#include "group_shape48t.h"
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Every Ref is index+1 into its named table; zero is null. Identities are opaque
// nonzero output bits and do not act as references. Object identity must be
// unique; zero is reserved for null/current/list sentinel values.
using CollectionRef48S=std::uint32_t;
struct CollectionWords48S {
    const std::uint32_t* values=nullptr;std::size_t capacity=0;
    std::int32_t count=0,mode=1;
};
struct CollectionFilter48S {
    std::uint32_t categoryA4=0,acceptMaskB8=0xffffffffu;
    CollectionWords48S identities[2]{},categories{};
    std::uint8_t flags31=1;
};
struct CollectionObject48S {
    std::uint32_t identity=0;
    CollectionFilter48S filter{};
    std::uint8_t typeTag=0;
    CollectionRef48S group=0;
    std::uint16_t position[3]{};
    std::int8_t scale10=0,scale11=0;
    // The closed135CF0 paths require a nonnull shape. Types4/5 mutate the
    // compact shape fields below; other nonzero types return it unchanged.
    // For the query object, its already prepared bounds shape is authoritative.
    bool hasShape=true;std::uint32_t shapeTag=1;
    // Optional1-based shape0 table index. Zero retains the48S boundary.
    // Active only for effective tag0 (query tag comes from bounds). Repeated
    // active references share the SAME mutable shape state; inactive refs
    // do not assert physical shape identity and are never dereferenced.
    CollectionRef48S shape0Ref=0;
};
struct CollectionGroup48S {
    const CollectionRef48S* members=nullptr;std::size_t memberCapacity=0;
    std::int32_t memberCount=0;
    float base180=1,other184=1,base18C=1,other194=1;
};
struct CollectionNode48S {
    CollectionRef48S children[4]{};
    CollectionRef48S firstBlock=0;
};
struct CollectionItemBlock48S {
    CollectionRef48S next=0,objects[4]{};
};
struct CollectionTree48S {
    enum class Method : std::uint8_t { QuadTree,Unknown } method=Method::QuadTree;
    CollectionRef48S root=0,pool=0;
    float originX=0,originZ=0,extentX=0,extentZ=0,scale=1;
    std::int32_t quantum=1;
};
struct CollectionZone48S {
    CollectionRef48S firstMethod=0,secondMethod=0;
};
struct CollectionZoneLink48S { CollectionRef48S next=0,zone=0; };
struct CollectionPool48S { std::int32_t perBlockCapacity=0; };
// Distinct physical chunk descriptors have disjoint full entry ranges.
struct CollectionPoolBlock48S {
    CollectionRef48S firstEntry=0;std::size_t entryCount=0;
};
struct CollectionObjectState48S {
    std::uint16_t epoch=0;std::uint8_t flagsF8=0;
    SpringVector4_48N cachedCenter{};float cachedRadius=0;
    Vector3_48C memberTranslation{};
    float memberScale18C=0,memberScale180=0;
};
struct CollectionTreeState48S {
    std::uint16_t epoch=0;std::uint8_t flags16=0;
    std::int32_t blocked898=0,cache[4]{};
    CollectionRef48S savedHeadA4=0,currentHeadA8=0,currentBlockB0=0;
    std::int32_t slotAC=0;std::uint32_t auxiliaryB4=0;
};
struct CollectionPoolState48S {
    CollectionRef48S activeFirst14=0,activeLast18=0,unusedFirst1C=0,freeEntry2C=0;
    std::int32_t used20=0;std::uint32_t bytesUsed34=0;
};
struct CollectionPoolBlockState48S { CollectionRef48S next=0; };
struct CollectionPoolEntry48S { CollectionRef48S next=0,node=0; };
struct CandidateCollectionInput48S {
    const CollectionObject48S* objects=nullptr;std::size_t objectCount=0;
    const CollectionGroup48S* groups=nullptr;std::size_t groupCount=0;
    const CollectionNode48S* nodes=nullptr;std::size_t nodeCount=0;
    const CollectionItemBlock48S* itemBlocks=nullptr;std::size_t itemBlockCount=0;
    const CollectionTree48S* trees=nullptr;std::size_t treeCount=0;
    const CollectionZone48S* zones=nullptr;std::size_t zoneCount=0;
    const CollectionZoneLink48S* zoneLinks=nullptr;std::size_t zoneLinkCount=0;
    const CollectionPool48S* pools=nullptr;std::size_t poolCount=0;
    const CollectionPoolBlock48S* poolBlocks=nullptr;std::size_t poolBlockCount=0;
    std::size_t poolEntryCount=0;
    CollectionRef48S query=0,zoneHead=0,fallbackZone=0,initialActiveTree=0;
    std::uint32_t methodMask=3,initialCurrentIdentity=0;
    std::int32_t nominalCapacity=0;
    bool skipSphere=false,ignoreGroups=false;
    // Effective diagnostics for later-zone/fallback null method selections.
    // False requires both source paths to be initialized and disabled. Initial
    // method null remains a dereference boundary irrespective of this setting.
    bool laterMethodDiagnosticsEnabled=true;
    std::uint8_t directTypeTag=0,groupTypeTag=1;
    float groupPositionScale=1;
    CollectionBoundsInput48S bounds{};
    const GroupShapeSource48T* shape0Sources=nullptr;std::size_t shape0Count=0;
};
struct CandidateCollectionState48S {
    const CollectionObjectState48S* objects=nullptr;std::size_t objectCapacity=0;
    const CollectionTreeState48S* trees=nullptr;std::size_t treeCapacity=0;
    const CollectionPoolState48S* pools=nullptr;std::size_t poolCapacity=0;
    const CollectionPoolBlockState48S* poolBlocks=nullptr;std::size_t poolBlockCapacity=0;
    const CollectionPoolEntry48S* poolEntries=nullptr;std::size_t poolEntryCapacity=0;
    const GroupShapeState48T* shape0States=nullptr;std::size_t shape0Capacity=0;
};
struct CandidateCollectionBuffers48S {
    CollectionObjectState48S* objects=nullptr;std::size_t objectCapacity=0;
    CollectionTreeState48S* trees=nullptr;std::size_t treeCapacity=0;
    CollectionPoolState48S* pools=nullptr;std::size_t poolCapacity=0;
    CollectionPoolBlockState48S* poolBlocks=nullptr;std::size_t poolBlockCapacity=0;
    CollectionPoolEntry48S* poolEntries=nullptr;std::size_t poolEntryCapacity=0;
    std::uint32_t* identities=nullptr;std::size_t identityCapacity=0;
    GroupShapeState48T* shape0States=nullptr;std::size_t shape0Capacity=0;
};
// Scratch for iterative, source-order traversal. Fields are implementation
// state only; callers allocate storage and never have to initialize it.
struct CollectionTraversalFrame48S {
    CollectionRef48S node=0;std::int32_t x=0,z=0,half=0;
    std::uint8_t nextChild=0;bool all=false;
};
struct CandidateCollectionWorkspace48S {
    CandidateCollectionBuffers48S buffers{};
    CollectionTraversalFrame48S* stack=nullptr;std::size_t stackCapacity=0;
    // Optional borrowed validation orders; nullptr+0 independently selects the
    // original quadratic validation. Otherwise count must equal the complete
    // world table count. Every call revalidates all current values/references:
    // identityOrder has strictly increasing nonzero object identities;
    // blockOrder has strictly increasing (entryCount ? firstEntry : 0, ref)
    // keys and disjoint adjacent nonempty full ranges. Exact-count strict
    // order proves a complete permutation without owned memory or stale trust.
    // Spans are read-only, stable for this call, and fully alias-guarded.
    const CollectionRef48S* identityOrder=nullptr;std::size_t identityOrderCount=0;
    const CollectionRef48S* blockOrder=nullptr;std::size_t blockOrderCount=0;
};
struct CandidateCollectionResult48S {
    std::int32_t count=0;
    std::uint32_t currentIdentity=0;
    CollectionRef48S activeTree=0;
};
enum class CandidateCollectionStatus48S {
    Complete,InvalidInput,InsufficientCapacity,NonFinite,UnsupportedBounds,
    UnsupportedMethod,MissingMethod,AllocatorBoundary,UnsupportedMemberShape,
    InvalidTopology
};
// Bounded original122350 with authenticated294590/294600/294740 and QuadTree
// 265040/2655C0/265C40/265C60. Explicit synthetic world only: no live world,
// registrations, scene refresh, implicit default index or allocator fallback.
// The input's query ref denotes the SAME object/stamp as any spatial occurrence.
// bounds cached fields are replaced by that object's initial cache before1202E0.
// The supplied zoneHead must be explicit; fallbackZone=0 explicitly models the
// null optional global fallback. Unknown selected methods reject transactionally.
//
// Nominal capacity is the original signed outer-loop limit. Iterator next runs
// before its check; group expansion may exceed it and emits repeated GROUP IDs.
// Physical identity capacities separately bound actual writes; shortage rejects
// the whole transaction. No hard truncation or pre-enumerated candidate list.
//
// All state tables are compact caller-owned mutable projections. On Complete,
// declared world counts of destination state and identities[0,count) are copied
// from scratch; unused destination tails are preserved. Any other status leaves
// ALL destination buffers and result bytes unchanged; scratch may change.
// Full declared writable spans must be aligned, representable, mutually disjoint
// and disjoint from every input/descriptor/nested span, including inactive tails.
// Read-only spans may alias. All capacities describe real backed C++ storage.
// Optional48T shape0 sources/states are compact shape-indexed tables, absent
// by default. Their full capacities and each nonnull geometry object participate
// in the same alias checks and whole-destination transaction. A reached null
// shape0Ref/geometry remains UnsupportedMemberShape; an out-of-range nonzero
// reference is InvalidTopology. Query bounds shape remains authoritative.
// Runtime is serial, allocation-free, constant native stack; explicit traversal
// stack and pool capacities are caller-owned. See docs/candidate-collection48s.md.
CandidateCollectionStatus48S collectCandidatesBounded48S(
    const CandidateCollectionInput48S&,const CandidateCollectionState48S&,
    const CandidateCollectionWorkspace48S&,const CandidateCollectionBuffers48S& destination,
    CandidateCollectionResult48S&) noexcept;
}
