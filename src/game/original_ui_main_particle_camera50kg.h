#pragma once
#include "original_ui_main_camera50kg.h"
#include "original_ui_options_camera50kg.h"
#include <array>
#include <cstdint>

namespace rr::game {
// Shared native13BCA0 scalar tangent. Caller owns the source angle's exact
// arithmetic grouping. Finite nearest-even binary32; output unchanged on fail.
bool originalUiSourceTangent50KG(float radians,float& tangent) noexcept;
enum class OriginalUiMainParticleCameraStatus50KG : std::uint8_t {
    ok,invalidCamera,unsupportedArithmetic
};
struct OriginalUiMainParticleCamera50KG {
    // Original flat row-vector matrices. No perspective divide is baked in.
    // Third column of combinedSourceScreen250 is the input to source particle
    // depth calculation162994..162A24, before its separate bucket scaling.
    std::array<float,16> sourceScreen140{},combinedSourceScreen250{};
    float focalPixels1D8=0;
};
// Cold continuation of the prepared Main core camera into the source particle
// render boundary. Source2361C0 initializes screen140,236DD0 multiplies
// combined100 * screen140 into250,11548C..1154FC computes focalPixels1D8.
// Actual Main640x448 and ctor zoom328=32C=1 only. Screen depth1/8388591 is
// distinct from the projection's near/far1/100000. This is not an OpenGL depth
// conversion, a live camera update, scene culling or particle scheduling.
//
// Focal tangent uses source grouping radians*(0.5*FOV), not the projection's
// 0.5*(radians*FOV); retain separately rounded binary32 steps. Trig polynomial
// matches the already recovered camera_view49f numeric13BCA0 implementation.
// Strict nearest-even binary32, no contraction/fast math. Atomic on failure.
OriginalUiMainParticleCameraStatus50KG prepareOriginalUiMainParticleCamera50KG(
    const OriginalUiMainCameraPreparation50KG&,
    OriginalUiMainParticleCamera50KG&) noexcept;
// Same native236DD0/11548C numeric continuation, actual Options ui.dat identity.
OriginalUiMainParticleCameraStatus50KG prepareOriginalUiOptionsParticleCamera50KG(
    const OriginalUiOptionsCamera50KG&,OriginalUiMainParticleCamera50KG&) noexcept;
// Shared native source-screen continuation. Numeric only: each concrete caller
// must authenticate its own camera/page identity before reaching this boundary.
OriginalUiMainParticleCameraStatus50KG prepareOriginalUiParticleCameraFromView50KG(
    const CameraViewOutput49F&,float originalWidth,float originalHeight,
    OriginalUiMainParticleCamera50KG&) noexcept;

}
