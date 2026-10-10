#pragma once
#include "candidate_collection50bx.h"
#include "indexed_type2_contact50cc.h"
#include "gate_query_dispatch50kg.h"
#include "terrain_query_workspace48m.h"
namespace rr::game {
struct VegetationShapeState50JL {
    GroupShapeState48T matrices{};
    CollisionSphere48J sphere{};
};
// Full object-indexed world; candidate indices and CollectionObject indices
// agree. Group member refs and shape0Ref are one-based. Source GC groups have
// zero or one member. Larger groups remain an explicit unsupported boundary.
struct VegetationExpansion50JL {
    const CollectionObject50BX* objects=nullptr;std::size_t objectCount=0;
    const CollectionGroup50BX* groups=nullptr;std::size_t groupCount=0;
    float groupPositionScale=1;
    const GroupShapeSource48T* shapeSources=nullptr;
    const VegetationShapeState50JL* initialShapes=nullptr;
    VegetationShapeState50JL* scratchShapes=nullptr;
    VegetationShapeState50JL* destinationShapes=nullptr;
    std::size_t shapeCount=0;
};
// Actual122660 raw Vegetation branch layered on the retained type2 producer.
// Raw instances are resolved only at their ordered dispatch position. The XZ
// gate precedes135CF0; shared shape setter precedes subtype geometry, then
// callback completes before the next raw entry. No per-instance prototype or
// direct126EB0/120E70 filter is substituted for that branch.
// One effect per raw entry: a reached member effect names its shared prototype;
// an unexpanded entry retains its raw identity. Apply only actual collision
// effects to canonical collision owners. Repeated prototype effects carry on.
// On Complete, destinationShapes and existing49X destinations commit together;
// errors leave all numerical destinations/result unchanged, while scratch may
// change. With opt-in Gate context, actual registered shape5 hits evaluate the
// defined native segment/sphere policy at their ordered position. E4 skips the
// TRIGGER category, then E8 consumes its genuine receipt synchronously. Gate
// actions commit immediately and survive a later numerical/query boundary.
// No combined transaction or retry/replay guarantee is claimed. Null preserves
// the legacy shape/E8 scope. No trigger writes pair or spring-contact records.
Type2StaticStatus49X produceRawIndexedType2Contacts50JL(
    const Type2StaticQuery49X&,const Type2StaticCandidate49X*,std::size_t,
    const std::size_t*,std::size_t,const Type2StaticInitial49X&,
    const Type2StaticWorkspace49X&,const Type2StaticDestination49X&,
    const VegetationExpansion50JL&,Type2StaticResult49X&,
    const GateQueryDispatchContext50KG* gate=nullptr) noexcept;
TerrainQueryStatus48M queryRawTerrainViewBounded50JL(
    const TerrainQueryView48M&,const Type2StaticCandidate49X*,std::size_t,
    const std::size_t*,std::size_t,const TerrainQueryWorkspace48M&,
    TerrainCandidateEffects48J*,std::size_t,const VegetationExpansion50JL&,
    TerrainQueryResult48M&) noexcept;
}
