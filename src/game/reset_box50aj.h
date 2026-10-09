#pragma once
#include "body_scene48g.h"
#include "collision_type3_node48p.h"
namespace rr::game {
// Original1063F0 endpoint-to-mesh transform and ordered segment/box SAT.
// Explicit prepared matrix; no scene refresh, inverse construction or traversal.
// Finite consumed XYZ/matrix lanes, nonnegative half extents. Input W and matrix
// fourth lanes are unconsumed. Intermediate overflow follows original ordered
// comparisons; no replacement slab test. Serial fixed storage, no allocations.
Type3NodeStatus48P resetSegmentBox50AJ(const SpringVector4_48N& start,
 const SpringVector4_48N& end,const BodyMatrix48G& worldToLocal,
 const Vector3_48C& center,const Vector3_48C& half) noexcept;
}
