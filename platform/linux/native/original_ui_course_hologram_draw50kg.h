#pragma once
#include "../../../src/assets/original_ui_course_hologram_assets50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include "../../../src/assets/original_ui_material50kg.h"
#include "../../../src/game/original_ui_course_hologram_scene50kg.h"
#include "../../../src/game/original_ui_lit_geometry50km.h"
#include "../../../src/game/original_ui_texture_animation50kg.h"
#include "../../../src/game/original_application_animation_clock50kg.h"
namespace rr::platform {
class OriginalUiCourseDrawContribution50KG;
struct OriginalUiCourseHologramDrawDiagnostic50KG {
 std::uint32_t sourcePC=0;std::uint16_t widget=0;
 assets::OriginalUiCourseHologramAssetDiagnostic50KG asset{};
 game::OriginalUiCourseHologramSceneDiagnostic50KG scene{};
 assets::OriginalUiMaterialDiagnostic50KG material{};
 assets::OriginalUiScreenAssetDiagnostic50KG images{};assets::DecodeError48C texture{};
};
class OriginalUiCourseHologramDraw50KG final {
public:
 OriginalUiCourseHologramDraw50KG()noexcept;~OriginalUiCourseHologramDraw50KG();
 bool load(std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,std::shared_ptr<const assets::OriginalUiArchive50KG> course,const assets::OriginalUiMaterialRuntime50KG&,OriginalUiCourseHologramDrawDiagnostic50KG&)noexcept;
 bool loaded()const noexcept;
 std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images()const noexcept;
 const assets::OriginalUiImageBankInput50KG* bankInput()const noexcept;
 game::OriginalUiCourseHologramScene50KG* model(std::uint16_t)noexcept;
 const assets::OriginalUiCourseHologramAssets50KG* modelAssets(std::uint16_t)const noexcept;
 const assets::OriginalUiMaterial50KG* material(std::uint16_t)const noexcept;
 const std::vector<std::uint32_t>* images(std::uint16_t,std::uint32_t pass)const noexcept;
 const std::vector<assets::OriginalUiLitMaterial50KM>* policies(std::uint16_t,std::uint32_t pass)const noexcept;
 std::uint32_t currentImage(std::uint16_t,std::uint32_t pass)const noexcept;
 bool advanceTextureAnimations(std::uint32_t,OriginalUiCourseHologramDrawDiagnostic50KG&)noexcept;
 bool project(std::uint16_t,std::uint32_t surface,const game::ModelPalette49S&,const game::OriginalUiLitContext50KM&,bool radialFog,game::OriginalUiLitSurfaceOutput50KM&,game::OriginalUiLitGeometryWorkspace50KM&,OriginalUiCourseHologramDrawDiagnostic50KG&)const noexcept;
private:
 friend class OriginalUiCourseDrawContribution50KG;
 // New mode2 only: the collector has projected this actual shown surface and
 // is about to append its enabled pass. No caller timestamp or frame receipt.
 // Mode0 retains advanceTextureAnimations and its existing scheduling.
 bool serviceTextureAnimation24A740(std::uint16_t,std::uint32_t pass,
     const game::OriginalApplicationAnimationClockView50KG&,
     OriginalUiCourseHologramDrawDiagnostic50KG&)noexcept;
 bool animationFaulted()const noexcept;
 std::uint64_t textureAnimationMutationSerial()const noexcept;
 struct Storage;std::unique_ptr<Storage> storage_;
};
}
