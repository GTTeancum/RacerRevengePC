#pragma once
#include "original_ui_material_adapter50kg.h"
#include "original_ui_controller_popup_resources50kg.h"
#include "../../../src/assets/original_ui_atm50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include "../../../src/game/original_ui_rider_widgets50kg.h"
#include "../../../src/game/original_ui_text_geometry50kg.h"
#include "../../../src/game/original_ui_texture_animation50kg.h"
#include "../../../src/game/original_ui_exit_text50kg.h"
#include <array>
#include <memory>
#include <vector>

namespace rr::platform {
enum class OriginalUiRiderDrawResourceError50KG : std::uint8_t {
    none,alreadyLoaded,notLoaded,archive,videoOwner,widgets,resource,material,
    animation,geometry,samplerConflict,registry,allocation,input
};
struct OriginalUiRiderDrawResourceDiagnostic50KG {
    OriginalUiRiderDrawResourceError50KG error=OriginalUiRiderDrawResourceError50KG::none;
    std::uint16_t widgetId=0;
    std::uint32_t materialIndex=0,imageIndex=0,sourcePC=0;
    assets::OriginalUiScreenAssetDiagnostic50KG resource{};
    assets::OriginalUiMaterialDiagnostic50KG material{};
    assets::DecodeError48C atm{};
    game::OriginalUiRiderWidgetDiagnostic50KG widgets{};
    game::OriginalUiTextGeometryDiagnostic50KG text{};
    game::OriginalUiExitTextDiagnostic50KG exitText{};
    OriginalUiControllerPopupResourceDiagnostic50KG controllerPopup{};
};
struct OriginalUiRiderVideoOwner50KG {
    // Real original-mode native video lifetime, not a synthetic ownership tag.
    std::shared_ptr<const void> lifetime;
    std::int32_t width=0,height=0,colorBits=0,packetZMaximum=0;
    std::uint32_t zBufferFormat=0;
};
struct OriginalUiRiderColdInput50KG {
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,rider,fonts,backgrounds,overlays;
    const assets::OriginalUiEnglish50KG* english=nullptr;
    assets::OriginalUiMaterialRuntime50KG runtime{};
    OriginalUiRiderVideoOwner50KG video{};
    // Optional only for a route retaining the real root SharedDialogs47C.
    // Both absent preserves direct Course-back Rider; one alone is invalid.
    // Rider may rejoin this owner, never construct a replacement47C/history.
    std::shared_ptr<const assets::OriginalUiArchive50KG> controllerPopup;
    std::shared_ptr<OriginalUiControllerPopupResources50KG> retainedControllerPopup;
};
struct OriginalUiRiderMaterialBinding50KG {
    std::string registeredName;
    assets::OriginalUiMaterial50KG material;
    // Exact stored ATM order or one PST. All IDs belong to images(), owner0
    // in bankInput(); the complete composer must use the backend's remapping.
    std::vector<std::uint32_t> images;
    std::vector<assets::OriginalUiNativeMaterial50KG> policies;
    assets::OriginalUiNativeUntexturedMaterial50KG untextured{};
    game::OriginalUiTextureAnimationConfig50KG animation{};
    bool animated=false;
    std::uint32_t registryToken=0;
    std::size_t registryByteOffset=0;
    const assets::OriginalUiMemberIdentity50KG* registryIdentity=nullptr;
};
struct OriginalUiRiderBackground50KG {
    std::uint8_t rider=0;
    std::uint32_t material=assets::unresolvedMaterialTexture50EA;
    std::array<game::OriginalUiOverlayQuad50KG,4> quadrants{};
};
struct OriginalUiRiderPrompt50KG {
    std::uint8_t mask=0;
    std::uint32_t material=assets::unresolvedMaterialTexture50EA;
    game::OriginalUiOverlayQuad50KG quad{};
};
// Authentic cold widget/font/icon, background and shared aperture resource
// population. Does not claim model/stat, page/session, GPU, or route readiness.
// One physical Rider manager owns its 119 widgets; the two page exits remain
// distinct +440 objects. No fabricated DAT widgets or substitute images exist.
class OriginalUiRiderDrawResources50KG final {
public:
    OriginalUiRiderDrawResources50KG() noexcept;
    ~OriginalUiRiderDrawResources50KG();
    OriginalUiRiderDrawResources50KG(const OriginalUiRiderDrawResources50KG&)=delete;
    OriginalUiRiderDrawResources50KG& operator=(const OriginalUiRiderDrawResources50KG&)=delete;
    bool load(const OriginalUiRiderColdInput50KG&,OriginalUiRiderDrawResourceDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    std::shared_ptr<OriginalUiControllerPopupResources50KG> controllerPopup() const noexcept;
    game::OriginalUiRiderWidgets50KG* widgets() noexcept;
    const game::OriginalUiRiderWidgets50KG* widgets() const noexcept;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images() const noexcept;
    const assets::OriginalUiImageBankInput50KG* bankInput() const noexcept;
    const assets::MaterialTextureAssets50EA* textures() const noexcept;
    const std::vector<MaterialTextureSampler50EB>* samplers() const noexcept;
    const std::vector<OriginalUiRiderMaterialBinding50KG>* materials() const noexcept;
    const OriginalUiRiderVideoOwner50KG* video() const noexcept;
    std::uint32_t widgetMaterial(std::uint16_t) const noexcept;
    std::uint32_t materialIndex(const char*) const noexcept;
    // Cold static text only. Dynamic content/position comes from the live
    // widget owner and must be measured/relaid at its current source position.
    const game::OriginalUiTextGeometry50KG* textGeometry(std::uint16_t) const noexcept;
    const assets::OriginalUiFont50KG* textFont(std::uint16_t) const noexcept;
    const assets::Texture48B* textAtlas(std::uint16_t) const noexcept;
    const OriginalUiRiderBackground50KG* background(std::uint8_t rider) const noexcept;
    const std::array<OriginalUiRiderPrompt50KG,4>* prompts() const noexcept;
    game::OriginalUiExitText50KG* exitText(std::uint8_t player) noexcept;
    // Source SetDepth for ScreenText/ScreenTexture: negative means explicit
    // packet Z; nonnegative is reversed against the actual video maximum.
    // This keeps Rider Huttese depth -5 distinct from ordinary depth1 text.
    bool textPacketDepth(std::uint16_t,float&) const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
