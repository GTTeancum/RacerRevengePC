#pragma once
#include "../../../src/game/original_application_animation_clock50kg.h"
#include "original_ui_rider_draw_resources50kg.h"
#include "original_ui_rider_stat_draw50kg.h"
#include "original_ui_lit_material_adapter50km.h"
#include "../../../src/game/original_ui_rider_selected_packet50kg.h"
#include "../../../src/game/original_ui_aperture_owner50kg.h"
namespace rr::platform {
class VehicleWindowGl50DS;
struct OriginalUiRiderDrawClock50KG {bool known=false,advanceAnimations=false;std::uint32_t milliseconds=0;
    game::OriginalApplicationAnimationClockView50KG applicationAnimation{};};
enum class OriginalUiRiderContributionError50KG : std::uint8_t {
    none,input,alreadyBound,resource,bank,publication,phase,clock,widget,stat,model,geometry,
    material,schedule,capacity,allocation,unfinished
};
struct OriginalUiRiderContributionDiagnostic50KG {
    OriginalUiRiderContributionError50KG error=OriginalUiRiderContributionError50KG::none;
    std::uint16_t widget=0;std::uint32_t material=0,pass=0,sourcePC=0;
    assets::OriginalUiMaterialDiagnostic50KG materialDetail;
    game::OriginalUiRiderWidgetDiagnostic50KG widgetDetail;
    game::OriginalUiRiderStatOwnerDiagnostic50KG stat;
    OriginalUiRiderStatDrawDiagnostic50KG statDraw;
    game::OriginalUiRiderSelectedPacketDiagnostic50KG selected;
    game::OriginalUiRiderModelDiagnostic50KG model;
    game::OriginalUiTextDiagnostic50KG text;
};
// Complete native Rider frame owner. One sealed OriginalUiSourceFrame50KM composer.
// It owns every projected vertex/W/fog stream and all pass/bank lifetime pins.
// Session must preserve this exact source order: ordinary widgets/models,
// page stats then four background sprites, aperture0/1, prompts, top widgets,
// and group drain. No partial contribution can obtain a source-frame seal.
class OriginalUiRiderDrawContribution50KG final : public OriginalUiSourceOwner50KM {
public:
    OriginalUiRiderDrawContribution50KG() noexcept;~OriginalUiRiderDrawContribution50KG();
    OriginalUiRiderDrawContribution50KG(const OriginalUiRiderDrawContribution50KG&)=delete;
    OriginalUiRiderDrawContribution50KG& operator=(const OriginalUiRiderDrawContribution50KG&)=delete;
private:
    friend class OriginalUiFrontend50KG;
    friend class OriginalUiRiderSession50KG;
    // Concrete frontend/session construction only. Resource presence alone
    // cannot certify the destination's real registry and manager lifetime.
    bool bind(std::shared_ptr<OriginalUiRiderDrawResources50KG>,
        std::shared_ptr<const assets::OriginalUiRiderStatAssets50KG>,
        std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG>,
        game::OriginalUiRiderBackend50KG&,game::OriginalUiRiderDynamicOwner50KG&,
        game::OriginalUiLightManagerLifetime50KG&,std::uint32_t staticOwner,std::uint32_t statOwner,
        std::shared_ptr<const game::OriginalUiTransitionOwner50KG>,
        const std::array<const game::OriginalUiApertureOwner50KG*,2>&,
        OriginalUiRiderContributionDiagnostic50KG&) noexcept;
public:
    // One-time immutable bank/sampler admission. Dynamic row instances are
    // constructed only by the backend parent+A4 issuer, never this cold path.
    bool prepareBank(OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank()const noexcept;
    const std::vector<MaterialTextureSampler50EB>* samplers()const noexcept;
    bool bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB>,OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    bool begin(const OriginalUiRiderDrawClock50KG&,const game::OriginalUiRiderRenderContext50KG&,
        const game::OriginalUiRiderLighting50KG&,OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    bool projectOrdinaryWidgets(bool ancestorsShown,OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    bool projectPage(const game::OriginalUiRiderStatInput50KG&,bool pageShown,OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    bool projectExitText(const game::OriginalUiApertureHook50KG&,OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    bool projectAperture(std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG&,OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    bool projectPrompts(std::uint32_t actualMask42C,OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    bool projectOnTopWidgets(bool ancestorsShown,OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    bool finish(OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    const OriginalUiSourceFrame50KM* frame()const noexcept;
    // Invalidates CPU publication; invoke before callbacks or bank replacement.
    // Renderer retirement is SEPARATE: call retireOriginalUiSourceOwner while
    // this instance lives, and keep it alive for retry if that operation fails.
    void drain() noexcept;
private:
    // Synchronous parent+A4 grouping or authenticated retirement consumption.
    // Lifecycle acceptance is independent from source-frame acceptance. Only
    // the backend acknowledges the prefix after both consumers accepted it.
    bool consumeMaterials(bool rebuild,OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    std::uint64_t materialSequence()const noexcept;
    std::uint64_t sourceSubmissionWatermark()const noexcept;
    bool hasSourceSubmission()const noexcept;
    const VehicleWindowGl50DS* sourceRenderer()const noexcept;
    friend class VehicleWindowGl50DS;
    // Actual renderer calls only. A partial failed submission invalidates the
    // binding history; accepted source publication commits the staged native
    // representatives even when a later presentation copy fails.
    bool beginSourceSubmission(std::uint64_t,const VehicleWindowGl50DS&) const noexcept override;
    void finishSourceSubmission(std::uint64_t,bool sourceAccepted,bool sourceWorkStarted) const noexcept override;
    bool widgets(bool,bool,OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    bool model(std::uint16_t,OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    bool append(const game::OriginalUiOverlayQuad50KG&,std::uint32_t material,float packetZ,
        OriginalUiRiderContributionDiagnostic50KG&) noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
    OriginalUiSourceFrame50KM frame_;
};
}
