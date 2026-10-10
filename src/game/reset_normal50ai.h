#pragma once
#include "reset_leaf50ah.h"
namespace rr::game {
// Original10B840 float-vertex path with optional vertex normals, modes0/1.
// Source edge-pointer holes return UnsupportedInterpolationBoundary and preserve
// output. Other normal modes reject explicitly. Mesh traversal is not included.
Type3LeafStatus48P intersectResetTriangle50AI(const Type3Segment48P&,
 const Type3Triangle48P&,ResetLeafResult50AH&) noexcept;
}
