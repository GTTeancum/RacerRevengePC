#pragma once
#include "body_scene48g.h"
#include "collision_query48h.h"
#include "../assets/collision_mesh48i.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rr::game {
struct LocalSegment48I {
    Vector3_48C first,second;
    BodyMatrix48G worldMatrix;
};
struct CollisionCandidate48I {
    const assets::CollisionMesh48I* mesh;
    // Already composed COL-local/instance matrix, in original binary32 order.
    BodyMatrix48G worldMatrix;
    std::uint32_t id;
};
struct CollisionContact48I {
    Vector3_48C point,normal,correction;
    float fraction;
    std::uint32_t candidateId,face;
};
struct CollisionResult48I {
    // Original global <= selection and per-segment strictly-nearer selection.
    CollisionContact48I selected,nearestPair;
    std::uint32_t nodesVisited=0,nodesPruned=0,leavesTested=0,leafHits=0;
};
enum class CollisionQueryStatus48I { Miss,Hit,InvalidInput,UnsupportedInterpolationBoundary,InsufficientCapacity };
struct CollisionVisit48I {
    std::uint32_t candidateId,index;
    bool leaf,accepted;
};
// Caller-owned scratch, reusable across serial queries. No allocation occurs in
// the bounded API. A null trace with zero capacity disables diagnostics; a
// nonnull trace requests diagnostics even when its capacity is zero. Capacities
// are element counts; there is no implicit maximum tree depth or visit count.
struct CollisionWorkspace48M {
    std::uint32_t* stack=nullptr;
    std::size_t stackCapacity=0;
    CollisionVisit48I* trace=nullptr;
    std::size_t traceCapacity=0;
};
// Same numerical/query domain and traversal/tie order as the compatibility API
// below. Stack/trace exhaustion is InsufficientCapacity, never a truncated Hit.
// Only Hit commits output. Scratch may change on other results. traceUsed is
// reset to zero after validating disjoint writable/input extents, then reports
// the written trace prefix on every result. If these extents are invalid (in
// particular if traceUsed aliases output), no writable arguments are changed.
// Workspace storage, output and traceUsed must be mutually disjoint and must
// not overlap query/candidates/workspace descriptors or any input mesh storage.
CollisionQueryStatus48I queryOrderedCollisionBounded48M(const LocalSegment48I&,
    const CollisionCandidate48I*,std::size_t,const CollisionWorkspace48M&,
    CollisionResult48I&,std::size_t& traceUsed) noexcept;
// Single endpoint pair, mode1, float vertices. Candidate list must already have
// original eligibility/category filtering applied. Does not choose world
// candidates, modify callback state or update the spatial index.
// Preserves original transpose-relative matrices, 1067B0 pruning, child order,
// 1082B0 length/correction and 121410 world-result transforms.
// Only Hit writes output. An optional diagnostic trace records ordered visits.
// Compatibility wrapper: allocates temporary workspace. New serial runtime
// callers should use queryOrderedCollisionBounded48M with persistent scratch.
CollisionQueryStatus48I queryOrderedCollision48I(const LocalSegment48I&,
    const CollisionCandidate48I*,std::size_t,CollisionResult48I&,
    std::vector<CollisionVisit48I>* trace=nullptr) noexcept;
}
