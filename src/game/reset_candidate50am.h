#pragma once
#include "reset_tree50ak.h"
#include "collision_result_binding50mb.h"
namespace rr::game {
struct ResetCandidateResult50AM {SpringVector4_48N point{},normal{};std::uint32_t hit=0;};
// Original124580 static shape0 boundary. Cached sphere, composed COL matrix,
// flags31 and normal mode are supplied by caller, not invented from geometry.
// Does not perform124CB0 collection or effects. Hit pointW=1; normalW=2 is the
// original VF9.W=1 then VADD.W +VF0.W path, not a normalized-vector convention.
// Zero-length segments explicitly unsupported here pending scalar clamp proof.
ResetTreeStatus50AK queryResetCandidate50AM(const SpringVector4_48N& start,
 const SpringVector4_48N& end,const BodyMatrix48G& world,
 const Vector3_48C& sphereCenter,float sphereRadius,std::uint8_t flags31,
 const assets::CollisionMesh48I&,std::uint32_t normalMode,
 std::uint32_t* stack,std::size_t capacity,ResetCandidateResult50AM&) noexcept;
// Native one-child result lifetime, entered only at the actual reached local
// selected-result setup. Earlier enabled/sphere exits do not bind a child.
ResetTreeStatus50AK queryResetCandidateSourceBound50MB(const SpringVector4_48N& start,
 const SpringVector4_48N& end,const BodyMatrix48G& world,
 const Vector3_48C& sphereCenter,float sphereRadius,std::uint8_t flags31,
 const assets::CollisionMesh48I&,std::uint32_t normalMode,
 std::uint32_t* stack,std::size_t capacity,ResetCandidateResult50AM&,
 CollisionResultBindingOwner50MB&,const SourceOuterResultLease50MB&) noexcept;
}
