#pragma once
#include "collision_type3_leaf48p.h"
namespace rr::game {
struct ResetLeafResult50AH {float fraction=-1.f;SpringVector4_48N normal{},point{};};
// Original10B840 float-vertex, absent-vertex-normal branch. Coordinates are
// already in the mesh-local space, direction and length supplied by caller.
// This is only the triangle intersection, not1082B0,10AF70 or124CB0 traversal.
// Hit returns even on a non-improving tie; only strict/negative-prior fraction
// replaces output. No packed-plane normal substitution. W is unread input.
// Uses the separately verified common geometric path in48P; no scene mutation,
// guest execution, allocation, or manufactured misses. Unsupported normals
// reject rather than being silently flattened. Error/miss preserves output.
Type3LeafStatus48P intersectResetFlatTriangle50AH(const Type3Segment48P&,
 const Type3Triangle48P&,ResetLeafResult50AH&) noexcept;
}
