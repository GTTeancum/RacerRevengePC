#pragma once
#include "type3_query48p.h"

namespace rr::game::detail {
// Internal borrowed numeric seam. Callers must guard every reachable backing
// span and stage mutations before this function. It has no public transaction
// guarantee; the48P and48U public wrappers own that guarantee.
Type3QueryStatus48P dispatchType3Borrowed48U(
    const Type3QueryInput48P&,const Type3QueryContext48P&,const Type3Candidate48P&,
    const CollisionSphere48J&,const assets::CollisionMesh48I*,const BodyMatrix48G&,bool shapePresent,
    const Type3RayBuffers48P&,std::uint32_t*,std::size_t,Type3QueryState48P&,bool&) noexcept;
// Actual ephemeral VU scratch effect of the reached126F40 mesh transforms.
// No persistent physics owner and no caller-supplied post-query receipt. A
// filtered/coarse/unsupported-shape miss preserves the preceding context.
// This is meaningful only after Complete; the caller owns atomic publication.
struct Type3VectorScratch48U { float vf9W=0.f;bool written=false; };
Type3QueryStatus48P dispatchType3BorrowedWithScratch48U(
    const Type3QueryInput48P&,const Type3QueryContext48P&,const Type3Candidate48P&,
    const CollisionSphere48J&,const assets::CollisionMesh48I*,const BodyMatrix48G&,bool shapePresent,
    const Type3RayBuffers48P&,std::uint32_t*,std::size_t,Type3QueryState48P&,bool&,
    Type3VectorScratch48U&) noexcept;
bool eligibleType3Borrowed48U(const CollisionFilter48J&,const CollisionFilter48J&,
    std::uint8_t candidateFlagsF8) noexcept;
Type3QueryStatus48P outerSphereType3Borrowed48U(const Type3QueryState48P&,
    const CollisionSphere48J&,bool&) noexcept;
}
