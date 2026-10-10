#pragma once
#include "scene_graph49p.h"
#include <array>
namespace rr::game {
struct OriginalUiRiderBinderQuad50KG {
    // Source1D4280 vertex order: first-minus,first-plus,second-minus,second-plus.
    std::array<std::array<float,4>,4> position{};
    std::array<std::array<float,2>,4> uv{{{{0,0}},{{0,1}},{{1,0}},{{1,1}}}};
};
// Source1D3F90 +1D4280 numerical projection. cameraColumn is exactly
// source active camera object's matrix+C0 elements(0,2),(1,2),(2,2), not an
// invented eye vector. Named endpoints must be the real binderleftN/rightN.
// Source constructor width=2.25. Degenerate cross product rejects explicitly.
// False preserves graph/output. No texture clock/material/render submission;
// actual "binder" unique16fps ATM owner and source callbacks remain required.
bool projectOriginalUiRiderBinder50KG(const SceneGraph49P&,
    SceneNodeIndex49P first,SceneNodeIndex49P second,
    const std::array<float,3>& cameraColumn,OriginalUiRiderBinderQuad50KG&) noexcept;
}
