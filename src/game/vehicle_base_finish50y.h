#pragma once
#include "vehicle_base_tail50x.h"
#include "base_pose_history50y.h"
namespace rr::game {
// Native source continuation through pose/history and the main body's ordered
// per-frame cache callback. Sidecar history is initialized by the caller.
// Diagnostic-manager absence and omitted earlier-child preservation are explicit.
bool finishVehicleBaseAndMainCache50Y(VehicleFrameState49W&,const VehicleFrameBindings49W&,
 const BaseTailState50X&,bool diagnosticsAbsent,BasePoseHistoryState50Y&,
 const PreservingStage49W&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
}
