#pragma once
#include "spring_detector48o.h"
#include "type3_query48p.h"

namespace rr::game {
struct SpringDetectorPipelineInput48Q {
    SpringDetectorInput48O detector{};
    Type3QueryInput48P query{};
    // -1: use query.bodyScene as an independent cached value. Nonnegative:
    // explicit identity binding to source.detector.scenes[index], including
    // a world matrix refreshed by detector preparation in this same call.
    // No live ownership or relationship to candidate transforms is inferred.
    // Invalid bindings become absent callback scenes, rejected only if used.
    // query.bodyScenePresent remains authoritative even for a valid index;
    // selecting a matrix never creates a missing callback scene.
    std::int32_t bodySceneIndex=-1;
};
struct SpringDetectorPipelineSource48Q {
    SpringDetectorSource48O detector{};
    Type3RaySource48P rays{};
    const SpringRecord48N* records=nullptr;
    std::size_t recordCount=0;
};
struct SpringDetectorPipelineBuffers48Q {
    SpringDetectorBuffers48O detector{};
    Type3QueryBuffers48P query{};
};
struct SpringDetectorPipelineWorkspace48Q {
    SpringDetectorBuffers48O detectorScratch{},detectorPrepared{};
    Type3QueryWorkspace48P queryScratch{};
    Type3QueryBuffers48P queryPrepared{};
};
struct SpringDetectorPipelineResult48Q {
    SpringDetectorState48O detector{};
    Type3QueryResult48P query{};
    bool detectorReturned=false,queryExecuted=false;
    // Original295630 returns1 on every completed supported path. Query hit
    // is separately query.hasAnyHit and never becomes the detector return.
    std::uint32_t detectorReturnValue=0;
};
enum class SpringDetectorPipelineStatus48Q {
    Complete,NoSpring,QueryDisabled,RegistrationBoundary,WorldQueryBoundary,
    InvalidInput,UnsupportedScope,UnsupportedInterpolationBoundary,InsufficientCapacity
};
// Bounded supplied-list composition of295630 -> 11CA80 -> 122660 -> 294BB0.
// Detector preparation executes first; only LimitedQueryBoundary runs48P.
// Query endpoints/metadata come from detectorPrepared, pairCount from the
// independent detector.shapePairCount, and spring from prepared detector state.
// query.spring/pairCount/resetSpring/entry are intentionally superseded; query
// reset never runs twice. Candidate matrices remain explicit supplied values.
// NoSpring/QueryDisabled preserve exact48O effects and skip the query. The two
// named registration/world boundaries commit only the supported detector prefix
// and do not claim that the original detector returned. Missing shape, reached
// providers, dirty parented scenes, compounds and other48P boundaries stay
// unsupported. No live registration, enumeration, type2 adapter or scheduling.
//
// Source arrays are projected in full, including stale tails. Query rays and
// records pass through unchanged if query execution is skipped; candidate
// effects are untouched in that case. Unused destination tails are untouched.
// All full declared mutable capacities, output and nested readable backing
// (including vector capacities, meshes, names and descriptors) are validated
// and mutually disjoint across BOTH stages before any scratch write. Read-only
// inputs may alias one another. Zero counts ignore their pointers; a nonzero
// count/capacity requires aligned, representable, caller-backed storage.
// Errors may mutate workspace only; final arrays and whole result are unchanged.
// Complete and the four named prefix statuses commit all final arrays/result.
// Caller meshes must already be validated rooted trees (no shared/unreachable
// records);48P's visit limit bounds traversal, not full importer validation.
// Constant stack, no allocation, vector copy/growth, recursion, threads, cache
// growth, guest pointers or desktop dependencies. Same finite binary32 scope
// and payload/signed-zero semantics as48O/48P. Keep strict FP compilation.
SpringDetectorPipelineStatus48Q runSpringDetectorPipeline48Q(
    const SpringDetectorPipelineInput48Q&,const SpringDetectorPipelineSource48Q&,
    const Type3Candidate48P*,std::size_t candidateCount,
    const SpringDetectorPipelineWorkspace48Q&,
    const SpringDetectorPipelineBuffers48Q& destination,
    SpringDetectorPipelineResult48Q&) noexcept;
}
