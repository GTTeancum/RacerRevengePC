#pragma once
#include "scene_graph49p.h"
#include "model_bounds49o.h"
namespace rr::game {
struct ResetBoundNode50BA {SceneNodeIndex49P node;Vector3_48C center140,halfExtent150;};
struct ResetBoundsWorkspace50BA {SceneNode49P nodes[maxSceneNodes49P];ModelBoundsNode49O flat[maxModelBoundsNodes49O];ModelBoundsWorkspace49O numeric;ModelBoundsResult49O result;};
// Prepared fresh-model box route26F4B0. Parentless root; complete, unique,
// parent-first source flat order of all descendants; all descendants dirty
// (ready01 clear) and authored-box04 set. Root own04 and sphere10 clear.
// A ready root retains its supplied cached world. Dirty root refreshes normally.
// Unsupported cache/topology domains reject atomically. Caller-owned disjoint
// graph, bounds, result and scratch; no live model discovery or hot allocation.
bool refreshResetModelBounds50BA(const SceneGraph49P&,SceneNodeIndex49P root,
 const ResetBoundNode50BA*,std::size_t count,ResetBoundsWorkspace50BA&,
 ModelBoundsResult49O&) noexcept;
}
