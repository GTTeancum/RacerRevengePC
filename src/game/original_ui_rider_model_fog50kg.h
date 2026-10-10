#pragma once
#include "model_bounds49o.h"
#include "scene_graph49p.h"
#include "../assets/model_geometry49o.h"
#include <array>
namespace rr::game {
struct OriginalUiRiderModelFog50KG {
    ModelBoundsResult49O bounds;
    float distance1BC=0;
    bool computeRadialFog1B2=false;
};
// Actual multi-node Rider model route: PSG count1A0>1 selects aggregate box
// center160/radius198, then26F070 distance and241320 fog-computation flag.
// graph is the concrete Rider scene layout: additional model root0, original
// PSG nodes at1..N. Uses current post-motion locals and original PSG bounds.
// Snapshot only: no observer, live scene cache or renderer state is published.
// Single-node sphere194 route is deliberately not admitted by this adapter.
// Material FGE is INDEPENDENT. False computeRadialFog means source packet F254,
// not FGE0; callers must retain real inherited fog and original FOGCOL.
bool prepareOriginalUiRiderModelFog50KG(const assets::PsgHierarchyView49O&,
    const SceneGraph49P&,const std::array<float,3>& activeCameraPosition180,
    float actualFogStartB0,OriginalUiRiderModelFog50KG&) noexcept;
}
