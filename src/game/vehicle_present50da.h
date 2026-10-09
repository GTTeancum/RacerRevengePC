#pragma once
#include "vehicle_draw_submit50cs.h"
#include "../assets/vehicle_draw_assets50cr.h"
namespace rr::game {
struct VehiclePresentState50DA {
    VehicleDrawBatch50CO published{};
    std::uint64_t committedFrames=0;
    std::uint8_t publishedCpuBank=0;
    bool hasPublished=false;
};
enum class VehiclePresentStage50DA { Complete,Assets,Draw,Submission };
struct VehiclePresentResult50DA {
    VehiclePresentStage50DA stage=VehiclePresentStage50DA::Assets;
    VehicleDrawBatchDiagnostic50CO draw{};
    VehicleDrawSubmitDiagnostic50CS submission{};
};
// Actual draw preparation+backend submission with success-driven CPU-bank choice.
// Unlike frame-number parity, rejected frames never select the displayed bank
// for overwrite on retry. Source frame remains unchanged; no timing, camera,
// material, visibility, polling or device-creation policy is supplied here.
// Assets/state/workspace/backend lifetimes are stable and disjoint. No heap.
// On failure state/published CPU bank and backend's prior commit remain intact;
// inactive staging and diagnostic/workspace may change. Backend50CS must obey
// its abortable transaction contract. A commit counts backend acceptance only.
VehiclePresentResult50DA presentVehicle50DA(VehiclePresentState50DA&,
    assets::VehicleDrawAssets50CR&,const VehicleFrameState49W&,
    const VehicleFrameBindings49W&,ModelPalettePhase49S,ModelPaletteSceneState49S,
    const VehicleDrawBackend50CS&,VehicleDrawWorkspace50CM&)noexcept;
}
