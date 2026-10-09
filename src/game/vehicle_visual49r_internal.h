#pragma once
#include "vehicle_visual49r.h"

namespace rr::game::detail {
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
