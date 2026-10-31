#pragma once
#include "vehicle_visual49r.h"
#include <array>

namespace rr::game::detail {
// Exact t0=0,t1=1,t2=0 numerical basis branch; no Scene or guard effects.
// On success, rows0..2 contain right/up/look, translation is VF0 placeholder.
// Caller MUST replace translationXYZ with the saved original scratch30 XYZ
// before source matrix stores. False is a native numeric-domain bound at
// refusalPC, not a source check; output remains unchanged on false.
bool sourceParentlessBasis28A090(const std::array<std::uint32_t,4>& look,
    const std::array<std::uint32_t,4>& up,
    std::array<std::uint32_t,16>& scratch,std::uint32_t& refusalPC) noexcept;
// Internal source-phase stages operate on the wrapper's admitted transaction
// scratch. They may leave scratch partially updated on false; they are not
// standalone public atomic transitions. No concurrent or aliased records.
struct VisualFrame49R {
    // Exact stack outputs at21C440. B0 was overwritten by the two crosses;
    // it is correctedForward withW=0, not the original main-world forward.
    // Other five getter vectors haveW=1. mainUp retains original main up.
    VisualVector49R mainPosition, correctedForward, mainUp;
    VisualVector49R containerPosition, containerForward, containerUp;
};
bool prepareVisualContainer49R(const SceneGraph49P&, const VisualSceneBindings49R&,
    std::uint32_t probe108, VehicleVisualState49R&, VisualShared49R&,
    VisualFrame49R&, VehicleVisualDiagnostic49R&) noexcept;
bool updateVisualResponses49R(const VehicleVisualInput49R&,
    const VehicleVisualParameters49R&, const VisualSceneBindings49R&,
    const VisualFrame49R&, const SceneGraph49P&, VehicleVisualState49R&,
    VisualShared49R&, CoupledNoiseContext49M&, VehicleVisualDiagnostic49R&) noexcept;
}
