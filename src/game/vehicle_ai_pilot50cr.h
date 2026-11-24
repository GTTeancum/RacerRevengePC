#pragma once
#include "vehicle_frame49w.h"

namespace rr::game {
bool readPilotedPlayerFrame50CR(VehicleFrameState49W&,const VehicleFrameBindings49W&,float,
    VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
bool bindPilotedPlayerFrame50CR(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const VehicleFrameParameters49W&,VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
}
