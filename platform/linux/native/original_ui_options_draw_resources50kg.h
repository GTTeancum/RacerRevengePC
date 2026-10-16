#pragma once
#include "original_ui_controls_material_adapter50kg.h"
#include "../../../src/game/original_ui_controls_model50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include "../../../src/game/original_ui_light_manager_lifetime50kg.h"
#include "../../../src/assets/original_ui_atm50kg.h"
#include "../../../src/game/original_ui_options_menu_widgets50kg.h"
#include "../../../src/game/original_ui_options_menu_layout50kg.h"
#include "../../../src/game/original_ui_options_camera50kg.h"
#include "../../../src/game/original_ui_options_particle_render_state50kg.h"
#include "../../../src/assets/original_ui_main_particle_material50kg.h"
#include "../../../src/game/original_ui_text_geometry50kg.h"
#include "../../../src/game/original_ui_texture_animation50kg.h"
#include "../../../src/game/original_ui_exit_text50kg.h"
#include <memory>
#include <vector>

namespace rr::platform {
struct OriginalUiMainApplication50KG;
enum class OriginalUiOptionsDrawError50KG : std::uint8_t {
    none,alreadyLoaded,notLoaded,archive,videoOwner,widgets,resource,material,
    animation,geometry,samplerConflict,registry,allocation,input
};
struct OriginalUiOptionsDrawDiagnostic50KG {
    OriginalUiOptionsDrawError50KG error=OriginalUiOptionsDrawError50KG::none;
    std::uint16_t widgetId=0;
    std::uint32_t materialIndex=0,imageIndex=0,sourcePC=0;
    assets::OriginalUiScreenAssetDiagnostic50KG resource{};
    assets::OriginalUiMaterialDiagnostic50KG material{};
    assets::DecodeError48C atm{};
    game::OriginalUiOptionsWidgetDiagnostic50KG widgets{};
    game::OriginalUiTextGeometryDiagnostic50KG text{};
    game::OriginalUiOptionsCameraDiagnostic50KG camera{};
    game::OriginalUiControlsModelDiagnostic50KG controlsModel{};
    game::OriginalUiExitTextDiagnostic50KG exitText{};
    game::OriginalUiMainSceneDiagnostic50KG scene{};
    game::OriginalUiMainFireStatus50KG fire=game::OriginalUiMainFireStatus50KG::unprepared;
    game::OriginalUiMainParticleLightingStatus50KG lighting=game::OriginalUiMainParticleLightingStatus50KG::unprepared;
    game::OriginalUiOptionsParticleRenderStateStatus50KG renderState=game::OriginalUiOptionsParticleRenderStateStatus50KG::input;
};
struct OriginalUiOptionsVideoOwner50KG {
    // Genuine ownership of original-mode native video; no no-op alias.
    std::shared_ptr<const void> lifetime;
    std::int32_t width=0,height=0,colorBits=0,packetZMaximum=0;
    std::uint32_t zBufferFormat=0;
};
struct OriginalUiOptionsColdInput50KG {
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,options,fonts,controllerPopup,overlays;
    const assets::OriginalUiEnglish50KG* english=nullptr;
    assets::OriginalUiMaterialRuntime50KG runtime{};
    OriginalUiOptionsVideoOwner50KG video{};
    std::shared_ptr<OriginalUiMainApplication50KG> application;
    std::shared_ptr<game::OriginalUiLightManagerLifetime50KG> lightManager;
};
struct OriginalUiOptionsMaterialBinding50KG {
    std::string registeredName;
    assets::OriginalUiMaterial50KG material;
    std::vector<std::uint32_t> images;
    std::vector<assets::OriginalUiNativeMaterial50KG> policies;
    assets::OriginalUiNativeUntexturedMaterial50KG untextured{};
    game::OriginalUiTextureAnimationConfig50KG animation{};
    bool animated=false;
    bool model=false,specular=false;
    assets::OriginalUiLitMaterial50KM lit{};
    assets::OriginalUiControlsSpecularMaterial50KG controlsSpecular{};
    // Mat_Stbl source ordinal, not a guessed draw order or native group address.
    std::uint32_t registryToken=0;
    const assets::OriginalUiMemberIdentity50KG* registryIdentity=nullptr;
};
// Native cold resource owner for menu, Controls and Game Options widgets.
// Exact Options PSMs, Options background/ATMs, real shared fonts/overlay/prompts.
// No GPU publication, page/scene admission, source route or save effect implied.
class OriginalUiOptionsDrawResources50KG final {
public:
    OriginalUiOptionsDrawResources50KG() noexcept;
    ~OriginalUiOptionsDrawResources50KG();
    OriginalUiOptionsDrawResources50KG(const OriginalUiOptionsDrawResources50KG&)=delete;
    OriginalUiOptionsDrawResources50KG& operator=(const OriginalUiOptionsDrawResources50KG&)=delete;
    bool load(const OriginalUiOptionsColdInput50KG&,OriginalUiOptionsDrawDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    game::OriginalUiExitText50KG* exitText() noexcept;
    game::OriginalUiExitText50KG* gameOptionsExitText() noexcept;
    game::OriginalUiExitText50KG* controlsExitText(std::uint8_t controllerIndex) noexcept;
    bool controlsModelReady() const noexcept;
    bool publishControlsRegistry(OriginalUiOptionsDrawDiagnostic50KG&) noexcept;
    bool prepareControlsModelPacket(game::OriginalUiControlsModelPacket50KG&,OriginalUiOptionsDrawDiagnostic50KG&) noexcept;
    bool currentControlsModelSelection(const game::OriginalUiControlsModelSelection50KG&) const noexcept;
    bool applyControlsModelEffect(const game::OriginalUiControlsEffect50KG&,OriginalUiOptionsDrawDiagnostic50KG&) noexcept;
    bool advanceControlsModel(float elapsedSeconds,OriginalUiOptionsDrawDiagnostic50KG&) noexcept;
    bool controlsAnimationObservation(bool& animationWord10Nonzero) const noexcept;
    game::OriginalUiOptionsMenuWidgets50KG* widgets() noexcept;
    const game::OriginalUiOptionsMenuWidgets50KG* widgets() const noexcept;
    const assets::OriginalUiScreenAssets50KG* images() const noexcept;
    const assets::OriginalUiImageBank50KG* sourceImageBank() const noexcept;
    std::uint32_t controlsMaterial(const std::string& sourceName,std::uint32_t sourcePass) const noexcept;
    const assets::MaterialTextureAssets50EA* textures() const noexcept;
    const std::vector<MaterialTextureSampler50EB>* samplers() const noexcept;
    const std::vector<OriginalUiOptionsMaterialBinding50KG>* materials() const noexcept;
    const OriginalUiOptionsVideoOwner50KG* video() const noexcept;
    const game::OriginalUiTextGeometry50KG* textGeometry(std::uint16_t) const noexcept;
    const game::OriginalUiPositionedLine50KG* textGeometryLine(std::uint16_t,std::size_t) const noexcept;
    std::uint32_t widgetMaterial(std::uint16_t) const noexcept;
    std::uint32_t materialIndex(const char*) const noexcept;
    const game::OriginalUiOptionsBackground50KG* background() const noexcept;
    const game::OriginalUiOptionsBackground50KG* gameOptionsBackground() const noexcept;
    const game::OriginalUiOptionsBackground50KG* controlsBackground() const noexcept;
    const game::OriginalUiOptionsCamera50KG* camera() const noexcept;
    const game::OriginalUiMainSceneOwner50KG* scene() const noexcept;
    game::OriginalUiMainFireOwner50KG* fire() noexcept;
    const game::OriginalUiMainFireOwner50KG* fire() const noexcept;
    const game::OriginalUiOptionsParticleRenderState50KG* particleRenderState() const noexcept;
    const assets::OriginalUiMainParticleMaterial50KG* particleMaterial() const noexcept;
    std::uint32_t particleImageIndex() const noexcept;
    // Current selection's original first-line coordinates/width and cold arrow
    // dimensions. Does not change the animated frame or sample a clock.
    bool selection(game::OriginalUiOptionsSelection50KG&,OriginalUiOptionsDrawDiagnostic50KG&) const noexcept;
    bool gameOptionsSelection(game::OriginalUiOptionsSelection50KG&,OriginalUiOptionsDrawDiagnostic50KG&) const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
