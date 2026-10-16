#pragma once
#include "original_camera_start_effects.h"
#include "control_presets49d.h"
#include <cstdint>
namespace rr::game {
// Synchronous read-only actual race/Bank projection. This is not an alternate
// racer, timing owner, source-global renderer flag or GPU-readiness receipt.
struct OriginalSpeedBoostNativeFrame50LA {
    const OriginalCameraViewportFields* viewport=nullptr;
    const ControlPresets49D* preferences=nullptr;
    std::uint64_t sessionGeneration=0,cameraPublication=0;
    std::uint32_t viewIndex=0,selectedSlot=0,viewVehicleState40=0;
    float selectedBodySpeed34=0,selectedHeat7C=0;
    std::uint32_t selectedBoost64=0,raceBlinkD8=0;
    std::int32_t raceMode8C=0;
    std::uint8_t raceFlags1B0=0,raceEntityFlags29=0,uiFlags1E4=0;
};
}
