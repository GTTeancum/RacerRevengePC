#pragma once
#include "../assets/original_ui_archive50kg.h"
#include "../assets/original_ui_localization50kg.h"
#include "../assets/original_ui_material50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include "original_camera_start_effects.h"
#include "lap_completion50gg.h"
#include "lap_hud_null50id.h"
#include <array>
#include <memory>

namespace rr::game {
// Concrete native TextOverlay29C850/29CA40, not a UI Overlay_Text widget.
// Every active view owns all eleven distinct objects with40-character backing.
// Source constructor leaves overlays disabled until an actual source setter.
struct OriginalRaceHudText50KG {
    std::array<char,41> text{};
    std::uint8_t textLength=0,fontIndex=0,materialIndex=0;
    const OriginalCameraViewportFields* viewport=nullptr;
    std::array<std::int32_t,4> position{},positionFrom{},positionTarget{};
    float positionElapsed=0,positionDuration=0,viewportScaleX=1,viewportScaleY=1;
    std::uint32_t enabled70=0,justification74=3,blinkOn80=1,indefinite88=0,lockedCC=0;
    float blinkRemaining78=0,blinkPeriod7C=0,hold84=-1;
    std::int32_t alphaFrom90=255,alphaTarget94=255,alphaIntermediate98=0;
    float alphaElapsed9C=0,alphaDurationA0=0;
    float scaleFromA4=1,scaleTargetA8=1,scaleElapsedAC=0,scaleDurationB0=0;
    float rotationTargetB4=0,rotationFromB8=0,rotationElapsedBC=0,rotationDurationC0=0;
    float secondDelayD0=-1,secondScaleD8=1,secondRotationDC=0;
    std::int32_t secondAlphaD4=255;
    std::uint32_t color170=0xffffffff;
    float alpha174=255,scale178=1,rotation17C=0;
    bool colorDirty180=false,alphaDirty184=false,scaleDirty188=false,rotationDirty18C=false;
    // Real ScreenText state consumed by update and width queries, separately
    // from the pending TextOverlay setters. Rendering must not overwrite it
    // from a host HUD string or infer a source draw merely from an update.
    std::uint32_t screenColor54=0xffffffff;
    std::int16_t screenAlpha58=255;
    float screenScale40=1,screenRotation44=0;
    std::uint32_t screenJustification50=1;
};
struct OriginalRaceHudGleam50KG {
    const OriginalCameraViewportFields* viewport=nullptr;
    // Actual embedded ScreenTextures+40/+A0. Authenticated materials live in
    // the containing owner. Four native vertex-color lanes per sprite.
    std::array<std::array<std::uint8_t,4>,4> brightColor{},clearColor{};
    float y100=0,x104=0,width108=0,height10C=0,travel110=0,offset114=0;
    float angle118=0,duration120=0,elapsed124=0;
    std::uint32_t active11C=0;
};
struct OriginalRaceHudDiagnostic50KG {
    enum class Error:std::uint8_t {none,alreadyLoaded,archive,material,images,localization,viewport,allocation} error=Error::none;
    std::uint32_t sourcePC=0;
    assets::OriginalUiMaterialDiagnostic50KG material;
    assets::OriginalUiScreenAssetDiagnostic50KG images;
    assets::OriginalUiLocalizationDiagnostic50KG localization;
};
// Cold resources and mutable overlays are one fixed-address race-lifetime
// owner. No externally writable nullable array, callback receipt, guest pointer
// or ability to mark missing resources as loaded is exposed.
class OriginalRaceHud50KG final {
public:
    OriginalRaceHud50KG() noexcept;
    ~OriginalRaceHud50KG();
    OriginalRaceHud50KG(const OriginalRaceHud50KG&)=delete;
    OriginalRaceHud50KG& operator=(const OriginalRaceHud50KG&)=delete;
    // Bind the real APPSTART/OVERLAYS/LOC archives and process material flag.
    // This executes existing native cold resource readers when the application
    // calls it; no resource decoder was run to prepare this source packet.
    bool load(std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,
        std::shared_ptr<const assets::OriginalUiArchive50KG> overlays,
        std::shared_ptr<const assets::OriginalUiArchive50KG> localization,
        const assets::OriginalUiMaterialRuntime50KG&,OriginalRaceHudDiagnostic50KG&) noexcept;
    bool resourcesLoaded() const noexcept;
    // Call after actual viewports exist and before publishing race HUD fields.
    // Inactive second-view slots remain null by the source race1B0 low2 loop.
    bool construct1A8CC4(std::uint8_t raceFlags1B0,
        const std::array<const OriginalCameraViewportFields*,2>& views,
        std::int32_t actualRendererWidth,std::int32_t actualRendererHeight) noexcept;
    bool loaded() const noexcept;
    std::uint32_t viewCount() const noexcept;
    LapHudFields50ID lapFields(std::uint32_t view) const noexcept;
    // Only the three source text callbacks; other completion effects return
    // false for the existing player-end/reset owner. group34 is the actual
    // player's338->34; split11050 is the actual config11050, not a HUD choice.
    bool applyLapCompletion(std::uint32_t view,std::uint32_t group34,
        std::uint8_t split11050,const LapCompletionRequest50GG&) noexcept;
    bool stopAll1AD230() noexcept;
    // Native world scheduling: once before this step's racer prefixes, so
    // setters made by the lap callback survive until accepted presentation.
    bool advanceAnimations29CDA0(float seconds) noexcept;
    // Explicit bounded native HUD presentation policy. Call only after the
    // actual gameplay display completes, for its matching selected viewport.
    // Commits ScreenText caches used by later source animation/width reads;
    // does not claim original glyph, gleam-group or GPU draw submission.
    bool commitNativePresentation(std::uint32_t view,
        const OriginalCameraViewportFields& displayedViewport) noexcept;
    const OriginalRaceHudText50KG* textAt(std::uint32_t view,std::uint32_t raceOffset) const noexcept;
    const OriginalRaceHudGleam50KG* gleamAt(std::uint32_t view,std::uint32_t raceOffset) const noexcept;
    const assets::OriginalUiScreenAssets50KG* images() const noexcept;
    const assets::OriginalUiMaterial50KG* material(std::size_t) const noexcept;
    // Borrowed authentic APPSTART registration order, never a caller receipt.
    assets::OriginalUiMemberView50KG materialRegistry() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
