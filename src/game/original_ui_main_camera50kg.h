#pragma once
#include "../assets/original_ui_main_camera_config50kg.h"
#include "camera_view49f.h"
#include <cstdint>

namespace rr::game {
enum class OriginalUiMainCameraStatus50KG : std::uint8_t {
    ok,invalidDescriptor,unsupportedArithmetic
};
struct OriginalUiMainCameraPreparation50KG {
    const assets::OriginalUiMemberIdentity50KG* configurationIdentity=nullptr;
    CameraViewOutput49F core{};
    std::uint16_t originalWidth=0,originalHeight=0;
    float aspect1DC=0,aspect1F8=0;
    bool cameraInfoApplied=false;
};
// Cold numeric preparation for the actual manager-owned Main camera:
// 1CF2C0 ->2361C0(640,448,exact bits3FAAAAAB) ->1CF4F0 ->114F90.
// Source236238..23628C supplies the aspect arithmetic, not desktop dimensions.
// Existing recovered49F supplies normalization, view, original projection and
// source-order V*P. Source near/far remain1143F0 defaults1/100000.
// Output is before PS2 viewport/screen adjustment; use the existing native GL
// camera adapter for world rendering. Never apply core matrices to authored
// overlay XY. Page1 must BORROW this one native camera, as1E5CF0 does.
//
// This function does not register a camera with an entity/scene, update its
// motion metrics, process debug pose controls, manage render passes or claim
// that the live camera ran. Native composition owns those explicit boundaries.
// Do not repeatedly re-normalize its core.sceneAfter as if it were raw DAT.
// Strict FE_TONEAREST, no contraction/fast math. Failure preserves output.
OriginalUiMainCameraStatus50KG prepareOriginalUiMainCamera50KG(
    const assets::OriginalUiMainCameraConfig50KG&,
    OriginalUiMainCameraPreparation50KG&) noexcept;
}
