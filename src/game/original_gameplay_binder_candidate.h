#pragma once
// Source-only candidate, not built or tested. Ordinary two-engine gameplay.
#include "vehicle_frame49w.h"
#include "original_ui_rider_binder50kg.h"
#include "../assets/native_asset48b.h"
#include <array>

namespace rr::game::original_gameplay_binder_candidate {
struct Bindings {
    SceneNodeIndex49P left230=SceneNodeIndex49P::none;
    SceneNodeIndex49P right238=SceneNodeIndex49P::none;
    //215BC8..215BFC: [Binder] BinderMeshHalfHeight default2.25. The cold
    // owner must read the selected PHY; never use a physical spring parameter.
    float halfHeight25C=2.25f;
};
struct Mesh {
    std::array<assets::Vertex48B,4> vertices{};
};
struct Workspace { std::array<SceneNode49P,vehicleFrameNodeCount49W> nodes; };
enum class Status : std::uint8_t {
    complete, phase, unsupportedVehicle, graph, binding, input, arithmetic
};
//213A40/220E70/229D20: named binderleft1/right1 under the actual selected
// engine models, not physical enginecable or chariotcable attachments.
// cameraColumn is active view matrix column2 in WORLD coordinates. No camera
// is sampled here. Output is the exact four world positions/UV order expected
// by the existing generic Diffuse projector, despite that type's UI name.
// Copies/refreshes the current graphics graph in caller workspace only.
// Scope: vehicle40=0, ordinary vehicle3C=0, one primary engine pair. Ghost,
// four-engine, detached states and scene/render scheduling are not substituted.
// Finite binary32/nearest arithmetic, portable native sqrt. No guest execution,
// VU implementation, generated texture, RNG, allocation, or state mutation.
// Degenerate/subnormal cross products reject, never invent a facing axis.
Status project(const VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const Bindings&,const std::array<float,3>& cameraColumn,
    OriginalUiRiderBinderQuad50KG&,Workspace&) noexcept;
//23CB20's Diffuse quad overrides light directions with identity, directional
// colors with zero, and copies CURRENT LightManager+50 into color row27.
// All four packed lanes use323E68..323ED8's +2^23 then upper-clamp operation.
// Caller supplies a source-owned +50 observation; no white/zone fallback.
Status shade(const OriginalUiRiderBinderQuad50KG&,
    const std::array<float,4>& currentAmbient50,Mesh&) noexcept;
}
