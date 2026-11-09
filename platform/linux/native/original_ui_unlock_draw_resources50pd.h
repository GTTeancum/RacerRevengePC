#pragma once
// Exact retained UIUnlock 2D resources: widgets 1..7 and 39..53, the shared
// page prompts/exit text, the %02d_bkg quadrant sprites (2DF0B0 via 20A000)
// and the three raw gallery backgrounds (page vtable+8C). No GPU publication,
// route admission or 3D model ownership.
#include "original_ui_native_draw50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include "../../../src/assets/original_ui_atm50kg.h"
#include "../../../src/assets/original_ui_material50kg.h"
#include "../../../src/game/original_ui_unlock_page50pd.h"
#include "../../../src/game/original_ui_unlock_widgets50pd.h"
#include "../../../src/game/original_ui_text_geometry50kg.h"
#include "../../../src/game/original_ui_texture_animation50kg.h"
#include "../../../src/game/original_ui_exit_text50kg.h"
#include "../../../src/game/original_ui_main_layout50kg.h"
#include "../../../src/game/original_ui_unlock_models50pd.h"
#include "../../../src/game/original_ui_unlock_camera50pd.h"
#include <array>
#include <memory>
#include <vector>

namespace rr::platform {
enum class OriginalUiUnlockDrawError50PD:std::uint8_t {
    none,alreadyLoaded,notLoaded,archive,videoOwner,widgets,page,resource,material,animation,geometry,
    samplerConflict,registry,allocation,input
};
struct OriginalUiUnlockDrawDiagnostic50PD {
    OriginalUiUnlockDrawError50PD error=OriginalUiUnlockDrawError50PD::none;
    std::uint16_t widgetId=0;std::uint32_t materialIndex=0,imageIndex=0,sourcePC=0;
    assets::OriginalUiScreenAssetDiagnostic50KG resource{};assets::OriginalUiMaterialDiagnostic50KG material{};
    assets::DecodeError48C atm{};assets::OriginalUiPageDiagnostic50KG page{};
    game::OriginalUiUnlockWidgetDiagnostic50PD widgets{};game::OriginalUiTextGeometryDiagnostic50KG text{};
    game::OriginalUiExitTextDiagnostic50KG exitText{};
    game::OriginalUiUnlockModelDiagnostic50PD models{};game::OriginalUiUnlockCameraDiagnostic50PD camera{};
};
struct OriginalUiUnlockVideoOwner50PD {
    std::shared_ptr<const void> lifetime;
    std::int32_t width=0,height=0,colorBits=0,packetZMaximum=0;std::uint32_t zBufferFormat=0;
};
struct OriginalUiUnlockColdInput50PD {
    // backgrounds = UIBKG (%02d_bkg PSTs); controllerPopup = CONTPOP (overlay1 PST);
    // overlays = OVERLAYS (cross/triangle prompt PSTs).
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,unlock,fonts,backgrounds,controllerPopup,overlays;
    const assets::OriginalUiEnglish50KG* english=nullptr;
    assets::OriginalUiMaterialRuntime50KG runtime{};
    OriginalUiUnlockVideoOwner50PD video{};
    // 3D widgets: UIPODTEX pod textures, the retained IPUM decode cache, and
    // the pods whose pod+pilot may be shown (originalUiUnlockCandidatePods50PD).
    std::shared_ptr<const assets::OriginalUiArchive50KG> podTextures;
    const assets::OriginalUiIpumTextureCache50KG* ipumCache=nullptr;
    std::uint32_t candidatePods=0;
    // Exact original sprite set (209C2C..209CC4): pod built for each bit. Zero
    // keeps the native stand-in (every pod whose UIUnlock %02d_bkg.psm exists).
    std::uint32_t spritePods=0;
};
struct OriginalUiUnlockMaterialBinding50PD {
    std::string registeredName;assets::OriginalUiMaterial50KG material;std::vector<std::uint32_t> images;
    std::vector<assets::OriginalUiNativeMaterial50KG> policies;assets::OriginalUiNativeUntexturedMaterial50KG untextured{};
    game::OriginalUiTextureAnimationConfig50KG animation{};bool animated=false;
    std::uint32_t registryToken=0;const assets::OriginalUiMemberIdentity50KG* registryIdentity=nullptr;
    // 3D pass (pod/pilot DefaultMT pass, or the binder): passes[0] is that one
    // original pass; images are COMBINED bank indices; spans use the Rider
    // pass dispatch rather than the ScreenMT policy.
    bool model=false;
};
// 20A000: four 320x224 ScreenTextures at page+4C0+pod*180, depth15, sample
// endpoints 319/223 (tile1 mirrored X, tile2 mirrored Y, tile3 both).
struct OriginalUiUnlockSprites50PD {
    std::uint8_t pod=0;bool built=false;
    std::uint32_t material=assets::unresolvedMaterialTexture50EA;
    std::array<game::OriginalUiOverlayQuad50KG,4> quadrants{};
};
struct OriginalUiUnlockPrompt50PD {
    std::uint8_t mask=0;std::uint32_t material=assets::unresolvedMaterialTexture50EA;game::OriginalUiOverlayQuad50KG quad{};
};
class OriginalUiUnlockDrawResources50PD final {
public:
    OriginalUiUnlockDrawResources50PD() noexcept;~OriginalUiUnlockDrawResources50PD();
    OriginalUiUnlockDrawResources50PD(const OriginalUiUnlockDrawResources50PD&)=delete;
    OriginalUiUnlockDrawResources50PD& operator=(const OriginalUiUnlockDrawResources50PD&)=delete;
    bool load(const OriginalUiUnlockColdInput50PD&,OriginalUiUnlockDrawDiagnostic50PD&) noexcept;
    bool loaded() const noexcept;
    const game::OriginalUiUnlockPage50PD* page() const noexcept;
    game::OriginalUiUnlockWidgets50PD* widgets() noexcept;const game::OriginalUiUnlockWidgets50PD* widgets() const noexcept;
    game::OriginalUiExitText50KG* exitText() noexcept;
    const assets::OriginalUiScreenAssets50KG* images() const noexcept;
    const assets::OriginalUiImageBank50KG* sourceImageBank() const noexcept;
    const assets::MaterialTextureAssets50EA* textures() const noexcept;
    const std::vector<MaterialTextureSampler50EB>* samplers() const noexcept;
    const std::vector<OriginalUiUnlockMaterialBinding50PD>* materials() const noexcept;
    const OriginalUiUnlockVideoOwner50PD* video() const noexcept;
    std::uint32_t widgetMaterial(std::uint16_t) const noexcept;
    std::uint32_t materialIndex(const char*) const noexcept;
    // Static (cold) text geometry, and the live message widget4 geometry.
    const game::OriginalUiTextGeometry50KG* textGeometry(std::uint16_t) const noexcept;
    // Source SetDepth: negative explicit packet Z, else reversed from the video maximum.
    bool textPacketDepth(std::uint16_t,float&) const noexcept;
    // Applies 2A9FF0 on the live widget AND rebuilds its SetText geometry,
    // all or nothing.
    bool setDynamicText(std::uint16_t,const std::string&,OriginalUiUnlockDrawDiagnostic50PD&) noexcept;
    const OriginalUiUnlockSprites50PD* sprites(std::int32_t pod) const noexcept;
    const std::array<OriginalUiUnlockPrompt50PD,4>* prompts() const noexcept;
    // Raw page background image for a gallery tier, and the retained +8C one.
    std::uint32_t galleryImage(game::OriginalUiUnlockGallery50PD) const noexcept;
    std::uint32_t currentBackgroundImage() const noexcept;
    bool setPageBackground8C(game::OriginalUiUnlockGallery50PD) noexcept;
    // 3D owners and their material bindings.
    game::OriginalUiUnlockModels50PD* models() noexcept;
    const game::OriginalUiUnlockCamera50PD* camera() const noexcept;
    std::uint32_t modelBinding(const assets::OriginalUiRiderModelMaterials50KG*,std::uint32_t material,std::uint32_t pass) const noexcept;
    std::uint32_t modelImage(const assets::OriginalUiRiderModelMaterials50KG*,std::uint32_t localImage) const noexcept;
    std::uint32_t binderBinding() const noexcept;
    std::uint32_t binderImage(std::uint32_t localImage) const noexcept;
private:struct Storage;std::unique_ptr<Storage> storage_;
};
}
