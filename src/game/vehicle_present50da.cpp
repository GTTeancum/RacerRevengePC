#include "vehicle_present50da.h"
namespace rr::game {
VehiclePresentResult50DA presentVehicle50DA(VehiclePresentState50DA&state,
    assets::VehicleDrawAssets50CR&assets,const VehicleFrameState49W&frame,
    const VehicleFrameBindings49W&bindings,ModelPalettePhase49S phase,
    ModelPaletteSceneState49S sceneState,const VehicleDrawBackend50CS&backend,
    VehicleDrawWorkspace50CM&workspace)noexcept{
    VehiclePresentResult50DA result{};
    if(state.publishedCpuBank>1)return result;
    const std::uint8_t bank=state.hasPublished?static_cast<std::uint8_t>(state.publishedCpuBank^1u):0;
    VehicleDrawBatchInput50CO input{};
    if(!assets.batchInput(bank,phase,sceneState,input))return result;
    VehicleDrawBatch50CO next{};result.stage=VehiclePresentStage50DA::Draw;
    if(!buildVehicleDrawBatch50CO(frame,bindings,input,next,workspace,result.draw))return result;
    result.stage=VehiclePresentStage50DA::Submission;
    if(!submitVehicleDrawBatch50CS(next,backend,result.submission))return result;
    state.published=next;state.publishedCpuBank=bank;state.hasPublished=true;
    ++state.committedFrames;result.stage=VehiclePresentStage50DA::Complete;return result;
}
}
