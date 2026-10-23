#pragma once
#include "original_ui_garage_panel50kg.h"
#include "../../../src/assets/original_ui_garage_prompt_material50kg.h"
#include "../../../src/game/original_ui_exit_text50kg.h"
namespace rr::platform {
struct OriginalUiGarageSharedColdInput50KG {
    std::shared_ptr<const OriginalUiGaragePanel50KG> panel;
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,overlays;
    const assets::OriginalUiEnglish50KG* english=nullptr;
    assets::OriginalUiMaterialRuntime50KG runtime{};
};
enum class OriginalUiGarageSharedError50KG:std::uint8_t {none,alreadyLoaded,notLoaded,owner,resource,material,bank,registry,aperture,mask,allocation};
struct OriginalUiGarageSharedDiagnostic50KG {
    OriginalUiGarageSharedError50KG error=OriginalUiGarageSharedError50KG::none;
    std::uint32_t sourcePC=0,material=0;
    assets::OriginalUiMaterialDiagnostic50KG materialDetail{};
    assets::OriginalUiGaragePromptDiagnostic50KG prompt{};
    assets::OriginalUiScreenAssetDiagnostic50KG image{};
    assets::OriginalUiImageBankDiagnostic50KG bank{};
    game::OriginalUiExitTextDiagnostic50KG exitText{};
};
struct OriginalUiGarageSharedMaterial50KG {
    std::string registeredName;
    assets::OriginalUiMaterial50KG authored;
    std::unique_ptr<assets::OriginalUiGaragePromptFallback50KG> prompt;
    assets::OriginalUiNativeMaterial50KG textured;
    assets::OriginalUiNativeUntexturedMaterial50KG untextured;
    std::uint32_t image=assets::unresolvedMaterialTexture50EA,registryToken=0;
    const assets::OriginalUiMemberIdentity50KG* registryIdentity=nullptr;
};
struct OriginalUiGarageSharedPrompt50KG {
    game::OriginalUiOverlayQuad50KG quad;
    std::uint32_t material=0,image=0,mask=0,objectOffset=0,positionPC=0;
};
struct OriginalUiGarageSharedPacket50KG {
    const game::OriginalUiOverlayQuad50KG* quad=nullptr;
    std::uint32_t material=0,image=assets::unresolvedMaterialTexture50EA,sourcePC=0,objectOffset=0;
    float packetZ=0;
};
// Retains actual Garage13/1 shared constructor objects: four prompts, two
// NoTexture aperture primitives, and localized+440 ScreenText. Shares genuine
// panel font/video/archive lifetimes; final image bank preserves exact origins.
// This is NOT a manager/session/collector or complete draw readiness owner.
// It deliberately cannot construct OriginalUiSourceFrame50KM. A future COMPLETE
// Garage composer must consume these raw source-RGBA packets with real registry
// history/alpha target, after ordinary widgets + Garage panel/scene, followed by
// shared child141D70 and OnTop traversal. No RGB-only alpha-disposable promise.
class OriginalUiGarageSharedPageDrawOwner50KG final {
public:
    OriginalUiGarageSharedPageDrawOwner50KG() noexcept;
    ~OriginalUiGarageSharedPageDrawOwner50KG();
    OriginalUiGarageSharedPageDrawOwner50KG(const OriginalUiGarageSharedPageDrawOwner50KG&)=delete;
    OriginalUiGarageSharedPageDrawOwner50KG& operator=(const OriginalUiGarageSharedPageDrawOwner50KG&)=delete;
    bool load(const OriginalUiGarageSharedColdInput50KG&,OriginalUiGarageSharedDiagnostic50KG&) noexcept;
    bool loaded()const noexcept;
    const OriginalUiGaragePanel50KG* panel()const noexcept;
    const std::array<OriginalUiGarageSharedMaterial50KG,6>* materials()const noexcept;
    const std::array<OriginalUiGarageSharedPrompt50KG,4>* prompts()const noexcept;
    game::OriginalUiExitText50KG* exitText()noexcept;
    const game::OriginalUiExitText50KG* exitText()const noexcept;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images()const noexcept;
    const assets::OriginalUiImageBank50KG* sourceImageBank()const noexcept;
    const assets::OriginalUiImageBankInput50KG* bankInput()const noexcept;
    const std::vector<MaterialTextureSampler50EB>* samplers()const noexcept;
    // Pure local projection after actual shared1D7550 render. Requires the real
    // Garage-bound numeric owner and its prepared geometry, not caller quads.
    // Actual+42C is supplied by session hooks, and+420 suppresses prompts only.
    // Does not invoke any callback or infer completion. Any reached+440 exit
    // packet MUST already be collected before these two aperture primitives.
    // Order is424,428,cross,triangle,square,circle; children remain external.
    bool projectAfterAperture(const game::OriginalUiApertureOwner50KG&,
        std::uint32_t actualMask42C,std::uint32_t actualSuppression420,
        OriginalUiGarageSharedDiagnostic50KG&) noexcept;
    const OriginalUiGarageSharedPacket50KG* packets()const noexcept;
    std::size_t packetCount()const noexcept;
private:struct Storage;std::unique_ptr<Storage> storage_;
};
}
