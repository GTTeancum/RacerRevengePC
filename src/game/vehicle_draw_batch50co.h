#pragma once
#include "vehicle_draw50cm.h"
namespace rr::game {
struct VehicleDrawBatchInput50CO {
    // Fixed submission order: chariot, left engine, right engine.
    VehicleDrawSelection50CM selection[3]{};
    VehicleDrawMesh50CM mesh[3]{};
    VehicleDrawBuffer50CM staging[3]{};
};
struct VehicleDrawBatch50CO {
    VehicleDrawSubmission50CM component[3]{};
    assets::Bounds48B bounds{};
    std::uint32_t vertices=0,indices=0,draws=0;
};
struct VehicleDrawBatchDiagnostic50CO {
    std::uint32_t component=0;
    VehicleDrawDiagnostic50CM detail{};
};
// Builds all three components from one immutable canonical frame. Reuses one
// palette/position workspace; caller staging vertex arrays must be distinct.
// Failure preserves batch descriptor but staging arrays may change. Keep current
// displayed buffers separate and publish/swap only after this function succeeds.
// No per-frame allocation, device command, camera policy or physical-pose rewrite.
bool buildVehicleDrawBatch50CO(const VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const VehicleDrawBatchInput50CO&,VehicleDrawBatch50CO&,
    VehicleDrawWorkspace50CM&,VehicleDrawBatchDiagnostic50CO&)noexcept;
}
