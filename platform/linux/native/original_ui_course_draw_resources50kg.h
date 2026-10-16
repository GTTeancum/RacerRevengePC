#pragma once
#include "original_ui_material_adapter50kg.h"
#include "../../../src/assets/original_ui_atm50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include "../../../src/game/original_ui_course_widgets50kg.h"
#include "../../../src/game/original_ui_text_geometry50kg.h"
#include "../../../src/game/original_ui_texture_animation50kg.h"
#include "../../../src/game/original_ui_exit_text50kg.h"
#include <array>
#include <memory>
#include <vector>
namespace rr::platform {
enum class OriginalUiCourseDrawResourceError50KG :std::uint8_t {none,alreadyLoaded,notLoaded,archive,videoOwner,widgets,resource,material,animation,geometry,samplerConflict,registry,allocation,input};
struct OriginalUiCourseDrawResourceDiagnostic50KG {
 OriginalUiCourseDrawResourceError50KG error=OriginalUiCourseDrawResourceError50KG::none;std::uint16_t widgetId=0;std::uint32_t materialIndex=0,imageIndex=0,sourcePC=0;
 assets::OriginalUiScreenAssetDiagnostic50KG resource{};assets::OriginalUiMaterialDiagnostic50KG material{};assets::DecodeError48C atm{};
 game::OriginalUiCourseWidgetDiagnostic50KG widgets{};game::OriginalUiTextGeometryDiagnostic50KG text{};game::OriginalUiExitTextDiagnostic50KG exitText{};
};
struct OriginalUiCourseVideoOwner50KG {std::shared_ptr<const void> lifetime;std::int32_t width=0,height=0,colorBits=0,packetZMaximum=0;std::uint32_t zBufferFormat=0;};
struct OriginalUiCourseColdInput50KG {std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,course,fonts,backgrounds,controllerPopup,overlays;const assets::OriginalUiEnglish50KG* english=nullptr;assets::OriginalUiMaterialRuntime50KG runtime{};OriginalUiCourseVideoOwner50KG video{};bool prepareRylothBackground=false;};
struct OriginalUiCourseMaterialBinding50KG {
 std::string registeredName;assets::OriginalUiMaterial50KG material;std::vector<std::uint32_t> images;
 std::vector<assets::OriginalUiNativeMaterial50KG> policies;assets::OriginalUiNativeUntexturedMaterial50KG untextured{};
 // Retained authored unique-controller metadata, including disabled-arrow
 // PST declarations. Only animated (stored ATM frames) receives a live native
 // numeric controller; static PST controller-phase parity is not claimed.
 game::OriginalUiTextureAnimationConfig50KG animation{};bool animated=false;std::uint32_t registryToken=0;std::size_t registryByteOffset=0;const assets::OriginalUiMemberIdentity50KG* registryIdentity=nullptr;
};
struct OriginalUiCoursePrompt50KG {std::uint8_t mask=0;std::uint32_t material=assets::unresolvedMaterialTexture50EA;game::OriginalUiOverlayQuad50KG quad{};};
// CPU resource ownership only. Unified composer must remap owner-local images.
class OriginalUiCourseDrawResources50KG final {
public:
 OriginalUiCourseDrawResources50KG()noexcept;~OriginalUiCourseDrawResources50KG();
 OriginalUiCourseDrawResources50KG(const OriginalUiCourseDrawResources50KG&)=delete;OriginalUiCourseDrawResources50KG& operator=(const OriginalUiCourseDrawResources50KG&)=delete;
 bool load(const OriginalUiCourseColdInput50KG&,OriginalUiCourseDrawResourceDiagnostic50KG&)noexcept;bool loaded()const noexcept;
 game::OriginalUiCourseWidgets50KG* widgets()noexcept;const game::OriginalUiCourseWidgets50KG* widgets()const noexcept;
 std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images()const noexcept;const assets::OriginalUiImageBankInput50KG* bankInput()const noexcept;
 const assets::MaterialTextureAssets50EA* textures()const noexcept;
 // Raw-background index is UNASSIGNED; composer owns its actual raw policy.
 const std::vector<MaterialTextureSampler50EB>* samplers()const noexcept;
 const std::vector<OriginalUiCourseMaterialBinding50KG>* materials()const noexcept;const OriginalUiCourseVideoOwner50KG* video()const noexcept;
 std::uint32_t widgetMaterial(std::uint16_t,bool selected=false)const noexcept;std::uint32_t materialIndex(const char*)const noexcept;
 const game::OriginalUiTextGeometry50KG* textGeometry(std::uint16_t)const noexcept;const assets::OriginalUiFont50KG* textFont(std::uint16_t)const noexcept;const assets::Texture48B* textAtlas(std::uint16_t)const noexcept;
 std::uint32_t backgroundImage(std::uint32_t planet=0)const noexcept;const std::array<OriginalUiCoursePrompt50KG,4>* prompts()const noexcept;
 game::OriginalUiExitText50KG* exitText()noexcept;bool textPacketDepth(std::uint16_t,float&)const noexcept;
private:struct Storage;std::unique_ptr<Storage> storage_;
};
}
