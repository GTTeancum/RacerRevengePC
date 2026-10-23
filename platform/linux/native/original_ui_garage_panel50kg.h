#pragma once
#include "original_ui_material_adapter50kg.h"
#include "../../../src/assets/original_ui_garage_page50kg.h"
#include "../../../src/assets/original_ui_garage_panel_material50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include "../../../src/game/original_ui_garage_state50kg.h"
#include "../../../src/game/original_ui_main_layout50kg.h"
#include "../../../src/game/original_ui_text_geometry50kg.h"
#include "../../../src/game/original_ui_text_widget50kg.h"
#include "../../../src/game/original_ui_texture_animation50kg.h"
#include "../../../src/game/original_application_animation_clock50kg.h"
#include <memory>
namespace rr::platform {
inline constexpr std::size_t originalUiGarageWidgetSlots50KG=23;
inline constexpr std::size_t originalUiGaragePanelMaterialCount50KG=9;
class OriginalUiGarageWattoWidgetOwner50KG;
class OriginalUiGarageSceneDrawOwner50KG;
class OriginalUiGarageSharedPageDrawOwner50KG;
// Intentionally incomplete required joins. They cannot be replaced by boolean
// readiness, an opaque lifetime, an empty draw, or this panel's cold success.
struct OriginalUiGaragePanelRequiredJoins50KG {
    std::shared_ptr<OriginalUiGarageWattoWidgetOwner50KG> watto10;
    std::shared_ptr<OriginalUiGarageSceneDrawOwner50KG> scene;
    std::shared_ptr<OriginalUiGarageSharedPageDrawOwner50KG> promptsExitAperture;
};
struct OriginalUiGaragePanelVideo50KG {
    std::shared_ptr<const void> lifetime;
    std::int32_t width=0,height=0,colorBits=0,packetZMaximum=0;
    std::uint32_t zBufferFormat=0;
};
struct OriginalUiGaragePanelColdInput50KG {
    std::shared_ptr<const assets::OriginalUiGaragePageData50KG> page;
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,fonts;
    const assets::OriginalUiEnglish50KG* english=nullptr;
    assets::OriginalUiMaterialRuntime50KG runtime{};
    OriginalUiGaragePanelVideo50KG video;
};
enum class OriginalUiGaragePanelError50KG:std::uint8_t {none,alreadyLoaded,notLoaded,owner,population,descriptor,material,image,registry,sampler,text,input,clock,requiredJoin,allocation};
struct OriginalUiGaragePanelDiagnostic50KG {
    OriginalUiGaragePanelError50KG error=OriginalUiGaragePanelError50KG::none;
    std::uint32_t sourcePC=0,material=0,image=0;std::uint16_t widget=0;
    assets::OriginalUiPageDiagnostic50KG descriptor{};
    assets::OriginalUiMaterialDiagnostic50KG materialDetail{};
    assets::OriginalUiGaragePanelFallbackDiagnostic50KG fallback{};
    assets::OriginalUiScreenAssetDiagnostic50KG imageDetail{};
    assets::OriginalUiImageBankDiagnostic50KG bank{};
    game::OriginalUiTextGeometryDiagnostic50KG geometry{};
    game::OriginalUiTextDiagnostic50KG glyph{};
    game::OriginalUiTextWidgetStatus50KG callback=game::OriginalUiTextWidgetStatus50KG::ok;
};
struct OriginalUiGaragePanelMaterial50KG {
    std::string registeredName;
    // Authored pass OR separate real fallback owner, never both.
    assets::OriginalUiMaterial50KG authored;
    std::unique_ptr<assets::OriginalUiGaragePanelFallback50KG> fallback;
    std::vector<std::uint32_t> images;
    std::vector<assets::OriginalUiNativeMaterial50KG> policies;
    std::uint32_t registryToken=0;
    const assets::OriginalUiMemberIdentity50KG* registryIdentity=nullptr;
    game::OriginalUiTextureAnimationConfig50KG animation{};
    bool animated=false;
};
struct OriginalUiGaragePanelInput50KG {
    const game::OriginalUiGarageState50KG* state=nullptr;
    // Actual retained page4D8; do not assume opened0 during entry/close.
    bool panelOffsetKnown=false;float panelOffset4D8=0;
    // Source selected-profile color pair at A8+8*career / AC+8*career.
    bool colorsKnown=false;std::uint32_t primaryA8=0,secondaryAC=0;
};
struct OriginalUiGaragePanelPacket50KG {
    const game::OriginalUiOverlayQuad50KG* quad=nullptr;
    const game::OriginalUiTextLayout50KG* text=nullptr;
    std::array<std::uint8_t,4> color{{127,127,127,63}};
    std::uint32_t material=0,image=0,frame=0,sourcePC=0,objectOffset=0;
    float packetZ=0;
};
struct OriginalUiGarageWidgetTextProjection50KG {
    const game::OriginalUiPositionedLine50KG* geometry=nullptr;
    game::OriginalUiTextLineProjection50KG line{};
    std::uint32_t material=0,image=0;float packetZ=0;
};
// Retained genuine Garage2D resources and source1FBE10 panel projection only.
// All17 text descriptors/fonts/materials and all120 notch objects are owned.
// Fresh numeric navigation only; no purchase/progression/storage mutation.
// Ordinary widget draws precede page1FDE40, so its SetText/SetPosition changes
// are visible on the NEXT manager traversal, not redrawn early by this class.
// Watto10, scene/selected model/particles, shared prompts/exit/aperture, native
// source collector and canonical material animation joins remain REQUIRED.
// No session, complete-frame seal, route or overall-page-ready API is exposed.
class OriginalUiGaragePanel50KG final {
public:
    OriginalUiGaragePanel50KG()noexcept;~OriginalUiGaragePanel50KG();
    OriginalUiGaragePanel50KG(const OriginalUiGaragePanel50KG&)=delete;
    OriginalUiGaragePanel50KG& operator=(const OriginalUiGaragePanel50KG&)=delete;
    bool load(const OriginalUiGaragePanelColdInput50KG&,OriginalUiGaragePanelDiagnostic50KG&)noexcept;
    bool loaded()const noexcept;
    const assets::OriginalUiGaragePageData50KG* pageData()const noexcept;
    const assets::OriginalUiOverlayText50KG* descriptor(std::uint16_t)const noexcept;
    const game::OriginalUiTextWidget50KG* widget(std::uint16_t)const noexcept;
    const std::vector<std::uint16_t>* ordinaryOrder()const noexcept;
    // Individual text leaf only. No manager callback batch may silently skip
    // Watto10 or nontext owners; those IDs return requiredJoin.
    bool applyTextCallback(std::uint16_t,game::OriginalUiControlOperation50KG,bool,
        OriginalUiGaragePanelDiagnostic50KG&)noexcept;
    bool updateText(std::uint16_t,bool ancestorsShown,OriginalUiGaragePanelDiagnostic50KG&)noexcept;
    bool projectText(std::uint16_t,std::size_t,bool ancestorsShown,
        OriginalUiGarageWidgetTextProjection50KG&,OriginalUiGaragePanelDiagnostic50KG&)noexcept;
    bool setEntryTheme(std::uint32_t primaryA8,std::uint32_t secondaryAC,OriginalUiGaragePanelDiagnostic50KG&)noexcept;
    bool projectFreshPanel(const OriginalUiGaragePanelInput50KG&,OriginalUiGaragePanelDiagnostic50KG&)noexcept;
    // Numeric unique-arrow leaf only. Real app clock and pause are required;
    // this does not impersonate canonical StandardMaterial animation callbacks.
    bool advanceArrows(const game::OriginalApplicationAnimationClockView50KG&,
        OriginalUiGaragePanelDiagnostic50KG&)noexcept;
    const OriginalUiGaragePanelPacket50KG* packets()const noexcept;
    std::size_t packetCount()const noexcept;
    const game::OriginalUiOverlayQuad50KG* notch(std::size_t row,std::size_t slot)const noexcept;
    const std::string* costText()const noexcept;
    const std::vector<std::string>* liveText(std::uint16_t)const noexcept;
    const std::array<OriginalUiGaragePanelMaterial50KG,originalUiGaragePanelMaterialCount50KG>* materials()const noexcept;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images()const noexcept;
    const assets::OriginalUiImageBankInput50KG* bankInput()const noexcept;
    const assets::OriginalUiImageBank50KG* sourceImageBank()const noexcept;
    const std::vector<MaterialTextureSampler50EB>* samplers()const noexcept;
    const OriginalUiGaragePanelVideo50KG* video()const noexcept;
private:struct Storage;std::unique_ptr<Storage> storage_;
};
}
