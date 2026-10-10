#pragma once
#include "vehicle_offscreen_gl50dd.h"
#include "../../../src/game/vehicle_present50da.h"
namespace rr::platform {
enum class OffscreenPresentStage50DE { Complete,Assets,Draw,Renderer };
struct OffscreenPresentResult50DE {
    OffscreenPresentStage50DE stage=OffscreenPresentStage50DE::Assets;
    game::VehicleDrawBatchDiagnostic50CO draw{};
    VehicleOffscreenDiagnostic50DD renderer{};
};
// Same success-driven CPU-bank publication as50DA, targeting the owning50DD
// renderer instead of an exposed50CS backend. CPU and GPU published banks stay
// unchanged on failure. Assets and renderer must be exclusive to this state,
// stable, initialized together, and used serially on the renderer's owner thread.
// Camera/clear values and scene-preparation attestations remain caller choices.
// No context creation, input sampling, readback or per-frame heap allocation.
OffscreenPresentResult50DE presentVehicleOffscreen50DE(game::VehiclePresentState50DA&,
    assets::VehicleDrawAssets50CR&,const game::VehicleFrameState49W&,
    const game::VehicleFrameBindings49W&,game::ModelPalettePhase49S,
    game::ModelPaletteSceneState49S,VehicleOffscreenGl50DD&,
    const VehicleOffscreenFrame50DD&,game::VehicleDrawWorkspace50CM&)noexcept;
}
