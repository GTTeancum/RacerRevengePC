#include "vehicle_offscreen_present50de.h"
namespace rr::platform {
OffscreenPresentResult50DE presentVehicleOffscreen50DE(game::VehiclePresentState50DA&state,
    assets::VehicleDrawAssets50CR&assets,const game::VehicleFrameState49W&frame,
    const game::VehicleFrameBindings49W&bindings,game::ModelPalettePhase49S phase,
    game::ModelPaletteSceneState49S sceneState,VehicleOffscreenGl50DD&renderer,
    const VehicleOffscreenFrame50DD&view,game::VehicleDrawWorkspace50CM&workspace)noexcept{
    OffscreenPresentResult50DE result{};
    if(state.publishedCpuBank>1)return result;
    const std::uint8_t bank=state.hasPublished?static_cast<std::uint8_t>(state.publishedCpuBank^1u):0;
    game::VehicleDrawBatchInput50CO input{};
    if(!assets.batchInput(bank,phase,sceneState,input))return result;
    game::VehicleDrawBatch50CO next{};result.stage=OffscreenPresentStage50DE::Draw;
    if(!game::buildVehicleDrawBatch50CO(frame,bindings,input,next,workspace,result.draw))return result;
    result.stage=OffscreenPresentStage50DE::Renderer;
    if(!renderer.submit(next,view,result.renderer))return result;
    state.published=next;state.publishedCpuBank=bank;state.hasPublished=true;
    ++state.committedFrames;result.stage=OffscreenPresentStage50DE::Complete;return result;
}
}
