#pragma once
// Host-side admission cache for the immutable part of the indexed type2 mesh
// check (every BVH node, vertex and face). Port-side safety pass only; the
// original never re-validates meshes per query. Host memory only: nothing here
// reads or writes game/guest memory. Results are identical to running the full
// check on every query:
//  - only meshes that PASSED the full check are recorded; a failing mesh is
//    re-checked every time and fails the same way;
//  - the key is the mesh address plus the address and size of every span the
//    full check reads (nodes, faces, vertices, vertex normals, node radii) and
//    reverse_mode, and the entry carries the scope generation;
//  - the table is live only inside an open scope (one race session, opened by
//    CollisionAdmissionScope50CC in terrain_interactive_setup50du.cpp) and only
//    on the thread that opened it; outside it every query runs the full check;
//  - opening and closing a scope both bump the generation and clear the table,
//    so a mesh freed with the session can never alias a new one;
//  - fixed capacity, every insert bounds-checked; when full the entry is not
//    stored and that mesh simply keeps taking the full check (slower, never wrong).
// RR_COLLISION_ADMISSION_CHECK=1 (test-only) runs the full check even on a
// cache hit, uses the full result, and counts disagreements.
#include "../assets/collision_mesh48i.h"
#include <cstddef>
#include <cstdint>

namespace rr::game {

struct CollisionAdmissionStats50CC {
    std::uint64_t checks=0;        // admission requests (one per candidate mesh per query)
    std::uint64_t cachedHits=0;    // requests answered by the table
    std::uint64_t fullChecks=0;    // requests that ran the node/vertex/face walk
    std::uint64_t inserts=0;       // meshes recorded as admitted
    std::uint64_t tableFull=0;     // admitted meshes not stored because the table was full
    std::uint64_t disagreements=0; // check switch only: cache said admitted, full check said no
    std::uint64_t entries=0;       // entries currently stored
    std::uint32_t generation=0;
};

// Run the node/vertex/face part of the mesh check, cached per scope. The caller
// keeps every per-query check (span/overlap reads, sizes, reverse_mode,
// radiusCount==nodes.size(), affine world, stack capacity, visit bound) and calls
// this only after all of those passed, exactly where the loops used to run.
// Returns true when the mesh contents pass (same answer as the full walk).
bool collisionMeshContentsAdmitted50CC(const assets::CollisionMesh48I& mesh,
    const float* radii,std::size_t radiusCount) noexcept;

// The uncached walk itself (same loops, same order, same answer as the code it
// replaces in indexed_type2_contact50cc.cpp validate()).
bool collisionMeshContentsValid50CC(const assets::CollisionMesh48I& mesh,
    const float* radii) noexcept;

// Scope control. Both bump the generation and clear the table. close also
// prints one `collision_admission ...` line when anything was checked.
void openCollisionAdmissionScope50CC() noexcept;
void closeCollisionAdmissionScope50CC() noexcept;
CollisionAdmissionStats50CC collisionAdmissionStats50CC() noexcept;
// Prints the same `collision_admission at=<when> ...` line now (no reset), so a
// run that is stopped before the session closes still reports. Called by the
// race frame recorder at each race end. Owner thread only; no-op otherwise.
void reportCollisionAdmission50CC(const char* when) noexcept;

// One race session's lifetime. Declare it before any owner of collision
// meshes or candidates in that session so it is destroyed after them.
class CollisionAdmissionScope50CC {
public:
    CollisionAdmissionScope50CC() noexcept {openCollisionAdmissionScope50CC();}
    ~CollisionAdmissionScope50CC() {closeCollisionAdmissionScope50CC();}
    CollisionAdmissionScope50CC(const CollisionAdmissionScope50CC&)=delete;
    CollisionAdmissionScope50CC& operator=(const CollisionAdmissionScope50CC&)=delete;
};

} // namespace rr::game
