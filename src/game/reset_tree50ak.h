#pragma once
#include "reset_normal50ai.h"
#include "reset_box50aj.h"
#include "../assets/collision_mesh48i.h"
#include <cstddef>
namespace rr::game {
struct ResetTreeResult50AK {float fraction=1.f;SpringVector4_48N normal{};std::uint32_t hit=0;};
enum class ResetTreeStatus50AK { Complete,InvalidInput,InsufficientCapacity,UnsupportedInterpolationBoundary,UnsupportedScope };
// Original10B700/10AF70/1082B0 single-segment mesh query with result+50=null.
// Supplied world-to-local matrix, float mesh, explicit normal mode0/1. Imported
// topology must be a rooted tree; visit budget also bounds malformed cycles.
// Child0 before child1, strict nearest ties; hit counts even when not nearer.
// No object eligibility/sphere test, inverse construction, collection or scene
// registration. Caller owns bounded stack; errors preserve result, scratch may
// change. All writable spans must be disjoint from each other and all inputs.
ResetTreeStatus50AK queryResetMesh50AK(const SpringVector4_48N& start,
 const SpringVector4_48N& end,const BodyMatrix48G& worldToLocal,
 const assets::CollisionMesh48I&,std::uint32_t normalMode,
 std::uint32_t* stack,std::size_t stackCapacity,ResetTreeResult50AK&) noexcept;
}
