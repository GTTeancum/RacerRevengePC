#pragma once
// UIWin 3D scene owners: page camera 1D7A90 (win.dat Camera*1), manager fog
// 1CE7B0 and the two ui.dat lights, and the six User_Rotate trophy/star models
// (Widgets.Dat 9..14). Authenticated UIWIN.RES only; native arithmetic only.
#include "original_ui_main_particle_camera50kg.h"
#include "original_ui_lit_geometry50km.h"
#include "original_ui_user_rotate50kg.h"
#include "model_palette49s.h"
#include "scene_graph49p.h"
#include "../assets/original_ui_pages50kg.h"
#include "../assets/model_geometry49o.h"
#include "../assets/import_scene48d.h"
#include <array>
#include <memory>
#include <string>

namespace rr::game {
struct OriginalUiWinRotateConfig50PD;
struct OriginalUiWinSceneDiagnostic50PD {
    assets::OriginalUiPageDiagnostic50KG parse{};
    assets::GeometryDiagnostic49O hierarchy{};
    assets::DecodeError48C geometry{};
    ModelPaletteDiagnostic49S palette{};
    OriginalUiLitGeometryStatus50KM lit=OriginalUiLitGeometryStatus50KM::complete;
    std::uint16_t widget=0;std::uint32_t sourcePC=0;
};
// Page vtable+C4 = shared 1D7A90 with win.dat PageInfo (same arithmetic as
// the Hall/Course page cameras, gated to the Win page file).
struct OriginalUiWinCamera50PD {
    const assets::OriginalUiMemberIdentity50KG* pageIdentity=nullptr;
    std::array<float,3> position{},look{{0,-1,0}},up{{0,0,1}};
    float fovDegrees=35,rollRadians=0,aspect1DC=0,aspect1F8=0;
    CameraViewOutput49F core{};
    OriginalUiMainParticleCamera50KG sourceScreen{};
};
// Manager 1CE7B0 fog + 13EA30 render-boundary camera, from UIWin ui.dat [Fog].
// UIWin authors the same "Visibilty" typo as Rider; the source reader looks up
// "Visibility" and takes its default 1. Haziness=0, ColorRGB=0,0,255.
struct OriginalUiWinRenderContext50PD {
    const assets::OriginalUiMemberIdentity50KG* fogIdentity=nullptr;
    float visibility=1,haziness=.5f;
    std::array<float,3> sourceFogRgb{};
    float fogStart=0,fogEnd=0,packetFogStart33Z=0,packetFogScale33W=0;
    OriginalUiWinCamera50PD cameraAfterFog;
    std::array<float,2> sourceCanvasOffset{{1728,1824}};
    std::array<float,16> clipProjection290{};
};
// Manager two-light population (Ambient 80,80,80; Directional 140,140,130
// from -1000,1000,1000 toward origin), source half-scale colours.
struct OriginalUiWinLighting50PD {
    const assets::OriginalUiMemberIdentity50KG* uiIdentity=nullptr;
    std::array<float,4> ambient50{},directional3C{};
    std::array<float,3> directionalLook78{};
};
bool prepareOriginalUiWinCamera50PD(const assets::OriginalUiArchive50KG&,OriginalUiWinCamera50PD&,OriginalUiWinSceneDiagnostic50PD&) noexcept;
bool prepareOriginalUiWinRenderContext50PD(const assets::OriginalUiArchive50KG&,const OriginalUiWinCamera50PD&,
    OriginalUiWinRenderContext50PD&,OriginalUiWinSceneDiagnostic50PD&) noexcept;
bool prepareOriginalUiWinLighting50PD(const assets::OriginalUiArchive50KG&,OriginalUiWinLighting50PD&,OriginalUiWinSceneDiagnostic50PD&) noexcept;
bool prepareOriginalUiWinLitContext50PD(const OriginalUiWinRenderContext50PD&,const OriginalUiWinLighting50PD&,
    OriginalUiLitContext50KM&) noexcept;

// One authored User_Rotate widget: authenticated ui/geometry/<SLT stem>.psg,
// its decoded single-node scene, and the live model-root pose. The pose is
// rebuilt from the authored Position/Look/Up on every DynamicLoad (select).
class OriginalUiWinModel50PD final {
public:
    OriginalUiWinModel50PD() noexcept;~OriginalUiWinModel50PD();
    OriginalUiWinModel50PD(const OriginalUiWinModel50PD&)=delete;OriginalUiWinModel50PD& operator=(const OriginalUiWinModel50PD&)=delete;
    bool load(std::shared_ptr<const assets::OriginalUiArchive50KG>,std::uint16_t widget,
        const OriginalUiWinRotateConfig50PD&,OriginalUiWinSceneDiagnostic50PD&) noexcept;
    bool loaded()const noexcept;
    std::uint16_t widgetId()const noexcept;
    const assets::Scene48D* scene()const noexcept;
    const assets::OriginalUiMemberIdentity50KG* geometrySource()const noexcept;
    // DynamicLoad on select (pose reset). Release on CleanFromMemory deselect.
    bool attach(OriginalUiWinSceneDiagnostic50PD&) noexcept;
    void release() noexcept;
    bool attached()const noexcept;
    // 1D38B0 loaded/unpaused branch with the actual update dt.
    bool advance(float dt,OriginalUiWinSceneDiagnostic50PD&) noexcept;
    bool projectPalette(ModelPalette49S&,OriginalUiWinSceneDiagnostic50PD&) noexcept;
    // Single-node PSG: source-index palette has one entry. Lit projection of
    // one surface/program. Specular uses the single-palette StripPreamble
    // modelView (33B918..33B928), i.e. nodeWorld[0]*view; see report.
    bool project(std::uint32_t surface,bool specular,const ModelPalette49S&,const OriginalUiLitContext50KM&,
        OriginalUiLitSurfaceOutput50KM&,OriginalUiLitGeometryWorkspace50KM&,OriginalUiWinSceneDiagnostic50PD&)const noexcept;
    // The model's 241320 radial-fog bit, admitted only where the bounds
    // sphere cannot reach fog start (then the source packet F is 255).
    bool radialFogClear(const OriginalUiWinRenderContext50PD&,OriginalUiWinSceneDiagnostic50PD&)const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
