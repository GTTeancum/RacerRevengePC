#pragma once
#include "../assets/original_ui_pages50kg.h"
#include "camera_view49f.h"
namespace rr::game {
struct OriginalUiOptionsCamera50KG {
    const assets::OriginalUiMemberIdentity50KG* uiIdentity=nullptr;
    bool hasCameraInfo=false;
    std::array<float,3> position{{0,50,35}},look{{0,0,1}},up{{0,1,0}};
    float fovDegrees=77,rollRadians=0,aspect1DC=0,aspect1F8=0;
    std::array<bool,5> authoredFields{};
    CameraViewOutput49F core{};
};
enum class OriginalUiOptionsCameraStatus50KG : std::uint8_t {ok,invalidArchive,parseFailure,unsupportedArithmetic};
struct OriginalUiOptionsCameraDiagnostic50KG {
    OriginalUiOptionsCameraStatus50KG status=OriginalUiOptionsCameraStatus50KG::ok;
    assets::OriginalUiPageDiagnostic50KG parse{};
};
// Genuine Options manager camera1D0580 ->2361C0(640,448,3FAAAAAB),
// CameraInfo reader1D07A0 and compose114F90. OptionsMenu+C4=1F9530 borrows
// manager+2B0; menu.dat CameraPosition1/etc are NOT the effective camera.
// Native CPU preparation only; no live registration, motion update or draw.
OriginalUiOptionsCameraStatus50KG prepareOriginalUiOptionsCamera50KG(
    const assets::OriginalUiArchive50KG&,OriginalUiOptionsCamera50KG&,
    OriginalUiOptionsCameraDiagnostic50KG&) noexcept;
}
