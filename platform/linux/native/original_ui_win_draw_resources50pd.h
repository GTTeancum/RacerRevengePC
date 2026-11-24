#pragma once
// Retained UIWin (screen 7) cold resources: materials, combined image bank,
// text geometry, icon layouts, page prompts, background tiles, the six
// User_Rotate trophy/star models and the page's 3D camera/fog/light context.
// No GPU publication and no route admission happen here.
#include "original_ui_native_draw50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include "../../../src/assets/original_ui_atm50kg.h"
#include "../../../src/assets/original_ui_material50kg.h"
#include "../../../src/assets/original_ui_material_binding50kg.h"
#include "../../../src/game/original_ui_win_page50pd.h"
#include "../../../src/game/original_ui_win_scene50pd.h"
#include "../../../src/game/original_ui_text_geometry50kg.h"
#include "../../../src/game/original_ui_texture_animation50kg.h"
#include "../../../src/game/original_ui_exit_text50kg.h"
#include <memory>
#include <vector>

namespace rr::platform {
enum class OriginalUiWinDrawError50PD:std::uint8_t {
    none,alreadyLoaded,notLoaded,archive,videoOwner,input,page,scene,model,resource,
    material,animation,geometry,samplerConflict,registry,allocation
};
struct OriginalUiWinDrawDiagnostic50PD {
    OriginalUiWinDrawError50PD error=OriginalUiWinDrawError50PD::none;
    std::uint16_t widgetId=0;std::uint32_t materialIndex=0,imageIndex=0,sourcePC=0;
    assets::OriginalUiScreenAssetDiagnostic50KG resource{};assets::OriginalUiMaterialDiagnostic50KG material{};
    assets::DecodeError48C atm{};game::OriginalUiWinPageDiagnostic50PD page{};game::OriginalUiWinSceneDiagnostic50PD scene{};
    game::OriginalUiTextGeometryDiagnostic50KG text{};game::OriginalUiExitTextDiagnostic50KG exitText{};
};
struct OriginalUiWinVideoOwner50PD {std::shared_ptr<const void> lifetime;std::int32_t width=0,height=0,colorBits=0,packetZMaximum=0;std::uint32_t zBufferFormat=0;};
struct OriginalUiWinColdInput50PD {
    // UIBKG supplies the %02d_bkg PSTs for UIWin's authored %02d_bkg.psm;
    // CTRLPOPUP supplies overlay1.pst, OVERLAYS the cross/triangle prompts.
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,win,fonts,controllerPopup,overlays,backgrounds;
    const assets::OriginalUiEnglish50KG* english=nullptr;assets::OriginalUiMaterialRuntime50KG runtime{};OriginalUiWinVideoOwner50PD video{};
};
enum class OriginalUiWinMaterialKind50PD:std::uint8_t {screen,untextured,lit,specular};
// One registered material PASS. Trophy PSMs contribute two (lit, specular).
struct OriginalUiWinMaterialBinding50PD {
    std::string registeredName;assets::OriginalUiMaterial50KG material;std::uint32_t passIndex=0;
    OriginalUiWinMaterialKind50PD kind=OriginalUiWinMaterialKind50PD::screen;
    std::vector<std::uint32_t> images;
    std::vector<assets::OriginalUiNativeMaterial50KG> screenPolicies;
    std::vector<assets::OriginalUiLitMaterial50KM> litPolicies;
    assets::OriginalUiRider19SpecularMaterial50KM specular{};
    assets::OriginalUiNativeUntexturedMaterial50KG untextured{};
    game::OriginalUiTextureAnimationConfig50KG animation{};bool animated=false;
    // One-based Mat_Stbl.txt rank (29C490 registry order); 0 = unregistered.
    std::uint32_t registryToken=0, drawRank=0;
    const assets::OriginalUiMaterialPass50KG& pass()const noexcept{return material.passes[passIndex];}
};
struct OriginalUiWinBackground50PD {
    std::uint32_t career=0,material=assets::unresolvedMaterialTexture50EA;
    std::array<game::OriginalUiOverlayQuad50KG,4> tiles{};
};
struct OriginalUiWinPrompt50PD {std::uint8_t mask=0;std::uint32_t material=0;game::OriginalUiOverlayQuad50KG quad;};
// Per model surface: lit pass1 binding and (trophies only) specular pass2.
struct OriginalUiWinModelSurface50PD {std::uint32_t first=assets::unresolvedMaterialTexture50EA,second=assets::unresolvedMaterialTexture50EA;};
struct OriginalUiMaterialSpanBinding50KG;struct OriginalUiMaterialSpan50KG;
// Same bounded conversion as convertOriginalUiRider19SpecularMaterial50KM for
// the one depth-writing destination-alpha specular (UIWin trophy pass2).
bool convertOriginalUiWinTrophySpecularSpan50PD(const assets::OriginalUiRider19SpecularMaterial50KM&,
    const assets::OriginalUiScreenAssets50KG&,const OriginalUiMaterialSpanBinding50KG&,OriginalUiMaterialSpan50KG&) noexcept;
class OriginalUiWinDrawResources50PD final {
public:
    OriginalUiWinDrawResources50PD()noexcept;~OriginalUiWinDrawResources50PD();
    OriginalUiWinDrawResources50PD(const OriginalUiWinDrawResources50PD&)=delete;OriginalUiWinDrawResources50PD& operator=(const OriginalUiWinDrawResources50PD&)=delete;
    bool load(const OriginalUiWinColdInput50PD&,OriginalUiWinDrawDiagnostic50PD&)noexcept;
    bool loaded()const noexcept;
    game::OriginalUiWinPage50PD* page()noexcept;const game::OriginalUiWinPage50PD* page()const noexcept;
    game::OriginalUiWinModel50PD* model(std::uint16_t widget)noexcept;const game::OriginalUiWinModel50PD* model(std::uint16_t)const noexcept;
    const std::vector<OriginalUiWinModelSurface50PD>* modelSurfaces(std::uint16_t widget)const noexcept;
    const game::OriginalUiWinCamera50PD* camera()const noexcept;
    const game::OriginalUiWinRenderContext50PD* renderContext()const noexcept;
    const game::OriginalUiWinLighting50PD* lighting()const noexcept;
    const game::OriginalUiLitContext50KM* litContext()const noexcept;
    game::OriginalUiExitText50KG* exitText()noexcept;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images()const noexcept;
    const assets::OriginalUiImageBank50KG* sourceImageBank()const noexcept;
    const assets::MaterialTextureAssets50EA* textures()const noexcept;
    const std::vector<MaterialTextureSampler50EB>* samplers()const noexcept;
    const std::vector<OriginalUiWinMaterialBinding50PD>* materials()const noexcept;
    const OriginalUiWinVideoOwner50PD* video()const noexcept;
    std::uint32_t materialIndex(const char* registeredName,std::uint32_t passIndex=0)const noexcept;
    std::uint32_t widgetMaterial(std::uint16_t)const noexcept;
    const game::OriginalUiTextGeometry50KG* textGeometry(std::uint16_t)const noexcept;
    // Widget5's SetText geometry for the page's current dynamic revision.
    bool prepareDynamicText(std::uint16_t,OriginalUiWinDrawDiagnostic50PD&)noexcept;
    const OriginalUiWinBackground50PD* background(std::uint32_t career)const noexcept;
    const std::array<OriginalUiWinPrompt50PD,4>* prompts()const noexcept;
    // 208AA0's sprintf result for (circuit, place), built once from English.
    const std::string* message(game::OriginalUiWinCircuit50PD)const noexcept;
private:struct Storage;std::unique_ptr<Storage> storage_;
};
}
