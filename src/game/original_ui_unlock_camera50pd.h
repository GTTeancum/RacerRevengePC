#pragma once
// UIUnlock 3D camera, fog and lights, read from the authenticated UIUNLOCK
// unlock.dat [PageInfo] (CameraPosition1/Look1/Up1/FOV1/Roll1) and ui.dat
// [Fog]/[Lights]. Same numeric owners as the Tournament camera (1D7A90 pose,
// 1CE7B0 fog, 13EA30 near/far, 237000 clip, 280720 membership, 1CDB10 lights).
// The pose is static (no camera animation), so the published frame is fixed.
#include "original_ui_rider_render_context50kg.h"
#include "original_ui_rider_visibility50kg.h"
#include "original_ui_rider_lighting50kg.h"
#include "original_ui_lit_geometry50km.h"
#include "original_racer_light_selection50km.h"
#include <memory>

namespace rr::game {
enum class OriginalUiUnlockCameraError50PD : std::uint8_t {none,alreadyBound,archive,page,fog,lights,arithmetic,allocation};
struct OriginalUiUnlockCameraDiagnostic50PD {
    OriginalUiUnlockCameraError50PD error=OriginalUiUnlockCameraError50PD::none;
    assets::OriginalUiPageDiagnostic50KG parse{};std::uint32_t sourcePC=0;
};
struct OriginalUiUnlockCameraFrame50PD {
    OriginalUiRiderRenderContext50KG render{};     // cameraAfterFog + fog terms
    OriginalUiRiderClipCamera50KG clip{};
    // Authored ui.dat population in Ambient,Directional registration order:
    // half-scale colours (1CE094/146FDC), W 64 / 127.5, normalized look.
    OriginalUiRiderLighting50KG lighting{};
    // Renderer-global 1499A0 rows (pod: User_Rotate model1B3 bit0 clear).
    OriginalUiLitContext50KM podContext{};
    // 14A0E8 small registry -> 149BB0 rows for the Soultree_Character.
    original_racer_lighting50KM::LightMatrices characterLights{};
    OriginalUiLitContext50KM characterContext{};
};
class OriginalUiUnlockCamera50PD final {
public:
    OriginalUiUnlockCamera50PD() noexcept=default;
    OriginalUiUnlockCamera50PD(const OriginalUiUnlockCamera50PD&)=delete;
    OriginalUiUnlockCamera50PD& operator=(const OriginalUiUnlockCamera50PD&)=delete;
    bool bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,OriginalUiUnlockCameraDiagnostic50PD&) noexcept;
    const OriginalUiUnlockCameraFrame50PD* frame() const noexcept {return archive_?&frame_:nullptr;}
    const assets::OriginalUiArchive50KG* archive() const noexcept {return archive_.get();}
private:
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive_;
    OriginalUiUnlockCameraFrame50PD frame_{};
};
}
