#pragma once
#include "vehicle_draw50cm.h"
namespace rr::game {
struct VehicleDrawBatchInput50CO {
    // Fixed order: chariot, left, right, optional bottom-left, bottom-right.
    VehicleDrawSelection50CM selection[maxVehicleDrawComponents50CM]{};
    VehicleDrawMesh50CM mesh[maxVehicleDrawComponents50CM]{};
    VehicleDrawBuffer50CM staging[maxVehicleDrawComponents50CM]{};
    std::uint32_t componentCount=defaultVehicleDrawComponents50CM;
};
struct VehicleDrawBatchSourceColors50LA {
    VehicleDrawSourceColors50LA component[maxVehicleDrawComponents50CM]{};
};
struct VehicleDrawBatch50CO {
    VehicleDrawSubmission50CM component[maxVehicleDrawComponents50CM]{};
    assets::Bounds48B bounds{};
    std::uint32_t vertices=0,indices=0,draws=0;
    std::uint32_t componentCount=defaultVehicleDrawComponents50CM;
};
struct VehicleDrawBatchDiagnostic50CO {
    std::uint32_t component=0;
    VehicleDrawDiagnostic50CM detail{};
};
// Builds every active component from one immutable canonical frame. Reuses one
// palette/position workspace; caller staging vertex arrays must be distinct.
// Failure preserves batch descriptor but staging arrays may change. Keep current
// displayed buffers separate and publish/swap only after this function succeeds.
// No per-frame allocation, device command, camera policy or physical-pose rewrite.
// Empty source-color components retain ordinary behavior. Partial descriptors
// or nonempty inactive components reject; no requested projection is discarded.
bool buildVehicleDrawBatch50CO(const VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const VehicleDrawBatchInput50CO&,VehicleDrawBatch50CO&,
    VehicleDrawWorkspace50CM&,VehicleDrawBatchDiagnostic50CO&,
    const VehicleDrawBatchSourceColors50LA* sourceColors=nullptr)noexcept;
}
