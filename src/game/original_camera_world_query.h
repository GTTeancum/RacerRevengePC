#pragma once
#include "candidate_collection50bx.h"
#include "original_camera_sphere_query.h"
#include "type3_state48p.h"
#include "compound_collision50ij.h"

namespace rr::game {
// Actual1792D0 owner fields used by Follow. Copied E0/F0 XYZ are retained;
// their unused source W/register-carry lanes are not invented by this adapter.
struct OriginalCameraCollisionCallback {
    std::uint32_t hit114=0;
    std::array<float,3> pointE0{},normalF0{};
};
// Exact query-object9C/E4 binding. Zero E4 means the original nullable call
// is skipped; a nonzero callback is admitted only for actual Follow1792D0.
// owner9C is the native key of the bound camera, never a fabricated guest PC.
struct OriginalCameraWorldCallbackBinding {
    std::uint64_t owner9C=0;
    std::uint32_t callbackE4=0;
    OriginalCameraCollisionCallback* follow1792D0=nullptr;
};
struct OriginalCameraSingleRay {
    Vector3_48C localStart{},localDirection{};
    SpringVector4_48N worldStart{},worldDirection{},normal{},point{};
    std::uint32_t hit=0;
    float fraction=-1.f,length=0.f;
};
struct OriginalCameraWorldQueryState {
    std::uint8_t flagsF8=0;
    std::uint32_t currentCandidateIdentity=0;
    SpringVector4_48N previousCenter70{},copied40{},copied50{};
    float distance84=0.f;
    OriginalCameraSphereContact sphere{};
    Type3QueryState48P type3{};
    OriginalCameraSingleRay ray{};
};
struct OriginalCameraWorldGeometry {
    const assets::CollisionMesh48I* mesh=nullptr;
    BodyMatrix48G world{};
    const CompoundCollisionView50IJ* compound=nullptr;
    bool hasResponseCallbackE8=false;
};
struct OriginalCameraWorldObjectCopies {
    SpringVector4_48N copied40{},copied50{};
};
struct OriginalCameraWorldInput {
    const CollectionObject50BX* objects=nullptr;
    const OriginalCameraWorldGeometry* geometry=nullptr;
    std::size_t objectCount=0;
    const CollectionTree50BX* methods=nullptr;std::size_t methodCount=0;
    const CollectionZone50BX* zones=nullptr;std::size_t zoneCount=0;
    // SAME camera query's E0 linked registration chain, in source order.
    const CollectionZoneLink50BX* zoneLinks=nullptr;std::size_t zoneLinkCount=0;
    CollectionRef50BX zoneHead=0,queryObject=0;
    bool registrationChainPresent=false; // true only for actual nonnull camera E0
    const CollectionRef50BX* globalFallbackZone443108=nullptr;
    const std::uint8_t* directTypeTagGp8010=nullptr;
    const std::uint32_t* staticObjectMask3E0720=nullptr;
    std::uint32_t shapeTag=0; // only genuine3 or5
    // Actual1202E0 inputs after11FF60/live registration; not selected candidates.
    CollectionBoundsInput48S bounds{};
    float sphereRadius50=0.f,sphereRadiusSquared54=0.f;
    SpringVector4_48N sentinel3E02C0{};
    std::uint32_t type3NormalMode44=0;
};
struct OriginalCameraWorldMutable {
    CollectionObjectState50BX* objects=nullptr;std::size_t objectCapacity=0;
    CollectionTreeState50BX* methods=nullptr;std::size_t methodCapacity=0;
    OriginalCameraWorldObjectCopies* copies=nullptr;std::size_t copyCapacity=0;
};
struct OriginalCameraWorldWorkspace {
    OriginalCameraWorldMutable staging{};
    std::uint32_t* meshStack=nullptr;std::size_t meshStackCapacity=0;
};
enum class OriginalCameraWorldStatus {
    Complete,Storage,Binding,Topology,Capacity,UnsupportedMethod,
    UnsupportedQuery,UnsupportedCallback,Arithmetic,Narrowphase
};
struct OriginalCameraWorldResult {
    OriginalCameraWorldStatus status=OriginalCameraWorldStatus::Complete;
    std::uint32_t sourcePC=0;
    CollectionRef50BX object=0,method=0;
    std::size_t visited=0,geometryHits=0,callbacks=0;
    float returnedDistance=0.f;
    bool resetGlobalsWritten=false; // enabled121920 reached11DD00; publish only on Complete
    bool normalModeWritten=false;
    std::uint32_t normalMode3DF5B0=0;
    // Genuine final VU VF9.W for the completed nullable-E4 shape3 route.
    // The producer tracks actual ordered dispatches, including misses after
    //a hit. Unknown for shape5, a skipped disabled query, or invoked Follow.
    bool type3VectorScratchKnown=false;
    float type3VectorScratchVF9W=0.f;
};
// Exact camera121920(a1=0) SearchList route. First pass method1 with global
// fallback; second pass method2 WITHOUT fallback iff a registered zone has
// distinct first/second methods. Uses actual filter/coarse sphere, shape0/1
// narrowphase and1792D0 callback in iterator order. Other shape tags produce
// their original dispatcher misses. No predeclared collision receipts.
// Camera constructor's F8 bit0 must be set; grouped indirect objects are then
// genuinely skipped. Reached response E8 remains an explicit boundary.
// Original121920 does NOT call294B40: do not close its final methods here.
// Query preparation/live registration11FF60 precedes this API; its owner must
// bind the current bounds/cache/E0 chain and retain each camera query separately.
// Only Complete commits mutable world states, query and camera callback; staging
// and diagnostic result may change on failure. Existing finite arithmetic,
// no-contraction/FE_TONEAREST, float-mesh/rooted-tree limits apply. Serial,
// allocation-free; caller-owned backing is stable and disjoint as documented
// by the reused50BX/48P structures. Output metadata reports, never hides, gaps.
OriginalCameraWorldStatus queryOriginalCameraWorld(
    const OriginalCameraWorldInput&,const OriginalCameraWorldMutable&,
    const OriginalCameraWorldWorkspace&,OriginalCameraWorldQueryState&,
    OriginalCameraCollisionCallback&,OriginalCameraWorldResult&) noexcept;
// Caller-specific nullable E4 path used by death query120. All world/result
// side effects are shared with the Follow entry, including candidate40/50
// copies and a reached target E8 boundary. Null E4 never invokes1792D0 and
// never interprets shape3 result90 as the incompatible Follow shape5 layout.
OriginalCameraWorldStatus queryOriginalCameraWorld(
    const OriginalCameraWorldInput&,const OriginalCameraWorldMutable&,
    const OriginalCameraWorldWorkspace&,OriginalCameraWorldQueryState&,
    const OriginalCameraWorldCallbackBinding&,OriginalCameraWorldResult&) noexcept;
}
