#pragma once
#include "original_ui_rider_draw_contribution50kg.h"
#include "original_ui_rider_profile50kg.h"
#include "../../../src/game/original_ui_rider_panel50kg.h"
#include "../../../src/game/original_ui_rider_render_context50kg.h"
#include "../../../src/assets/original_ui_ipum_texture_cache50kg.h"
namespace rr::platform {
class OriginalUiMainDrawResources50KG;
class OriginalUiCourseDrawResources50KG;
class OriginalUiCourseHologramDraw50KG;
class VehicleWindowGl50DS;
struct VehicleWindowDiagnostic50DS;
enum class OriginalUiRiderSessionStatus50KG : std::uint8_t {
    ok,noChange,backProposed,confirmProposed,unavailableDestination,notBound,alreadyBound,
    invalidInput,unsupportedState,profile,resource,widget,backend,model,
    camera,lighting,aperture,transition,draw,audio,allocation,faulted,quit,moveProposed
};
struct OriginalUiRiderSessionDiagnostic50KG {
    OriginalUiRiderSessionStatus50KG status=OriginalUiRiderSessionStatus50KG::ok;
    std::uint32_t sourcePC=0;
    OriginalUiRiderContributionDiagnostic50KG draw{};
    game::OriginalUiRiderWidgetDiagnostic50KG widget{};
    game::OriginalUiRiderDynamicDiagnostic50KG dynamic{};
    game::OriginalUiRiderBackendDiagnostic50KG backend{};
    game::OriginalUiRiderCameraDiagnostic50KG camera{};
    game::OriginalUiRiderRenderContextDiagnostic50KG renderContext{};
    game::OriginalUiRiderLightingDiagnostic50KG lighting{};
    game::OriginalUiLightManagerLifetimeDiagnostic50KG lightManager{};
    game::OriginalUiApertureDiagnostic50KG aperture{};
    game::OriginalUiRiderVoiceChoiceDiagnostic50KG voice{};
};
struct OriginalUiRiderSessionCold50KG {
    std::shared_ptr<OriginalUiRiderDrawResources50KG> resources;
    // Actual Main4 predecessor records, copied to canonical row inventory
    // synchronously. Never extends the predecessor's native widget lifetime.
    const OriginalUiMainDrawResources50KG* predecessorResources=nullptr;
    // Course6 pair is mutually exclusive with Main4; copied synchronously.
    const OriginalUiCourseDrawResources50KG* predecessorCourseResources=nullptr;
    const OriginalUiCourseHologramDraw50KG* predecessorCourseHolograms=nullptr;
    std::shared_ptr<const assets::OriginalUiRiderStatAssets50KG> statAssets;
    std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> binderAssets;
    std::shared_ptr<const assets::OriginalUiArchive50KG> riderArchive,appStart,podTextures;
    // The retained real decode cache is borrowed by the combined bank loader.
    std::shared_ptr<const assets::OriginalUiIpumTextureCache50KG> ipumCache;
    assets::OriginalUiMaterialRuntime50KG materialRuntime{};
    // Copied synchronously at bind: real English LOC167/168/172 bytes only.
    const assets::OriginalUiEnglish50KG* english=nullptr;
    std::shared_ptr<OriginalUiRiderProfileOwner50KG> profile;
    std::shared_ptr<OriginalUiMainProfile50KG> commonProfile;
    std::shared_ptr<OriginalUiMainApplication50KG> application;
    std::shared_ptr<game::OriginalUiLightManagerLifetime50KG> lightManager;
    std::shared_ptr<game::OriginalUiTransitionOwner50KG> externalTransition;
    // Exactly the prospective source Main decision, before its mode commit.
    // It may override a copied cold snapshot ONLY; never writes the profile.
    std::uint32_t prospectiveMode=0;
    // Source1CCF90 predecessor screen. Main4 or genuine mode0 Course6 Back.
    std::uint8_t previousScreen=0;
    // Same concrete application frontend transport used by Main/Options.
    // Enabled initial19/Back needs its optional authenticated Song7. Muted
    // mode owns no transport. New muted Confirm retains real shared voice RNG
    // and suppresses only output; its independent review gate stays closed.
    OriginalUiMainAudioMode50KG audioMode=OriginalUiMainAudioMode50KG::enabled;
    std::shared_ptr<OriginalUiMainAudio50KG> audio;
};
class OriginalUiRiderSession50KG;
struct OriginalUiRiderRouteProposal50KG {
    const OriginalUiRiderSession50KG* source=nullptr;
    std::uint64_t serial=0;
    game::OriginalUiRouteDecision50KG decision{};
    OriginalUiRiderProfileStamp50KG profileStamp{};
};
struct OriginalUiRiderSessionInputReceipt50KG {
    bool containerReached=false,pageCallbackRan=false;
    // Source1EA460 always returns0, including a handled Back or direction.
    // Never use our native status to forward an accepted Movement_Control8.
    bool pageAccepted=false;
    std::uint64_t moveSerial=0;
    std::uint8_t moveEvent=0;
    game::OriginalUiRiderSelectionPlan50KG selection{};
    game::OriginalUiRouteDecision50KG decision{};
    OriginalUiRiderRouteProposal50KG routeProposal{};
    OriginalUiRiderSessionStatus50KG status=OriginalUiRiderSessionStatus50KG::noChange;
};
struct OriginalUiRiderSessionSnapshot50KG {
    bool bound=false,faulted=false,quitRequested=false,pageShown=false,inputContainerShown=false;
    bool cameraActive=false,selectionAttached=false,framePublished=false,backProposalPending=false;
    bool confirmProposalPending=false;
    bool blocked4E0=false,bypass4E4=false,fadeMusic4E8=true;
    std::uint32_t rider4C4=0,promptMask42C=0,packedBackground430=0,deferred4EC=0;
    std::uint16_t slot4C8=0;
    float panelY4DC=140;
    std::uint64_t updates=0,renders=0,bankGeneration=0,registryGeneration=0;
    game::OriginalUiTransitionSnapshot50KG transition{};
    game::OriginalUiApertureSnapshot50KG aperture{};
    OriginalUiMainSessionAudioReceipt50KG audio{};
    game::OriginalUiRiderVoiceChoiceReceipt50KG voice{};
};
// Actual bounded Rider5/1 lifecycle and frame owner. Exact19/20 entry,
// directional selection, normal Back and mode0 Confirm use real resource,
// renderer-retirement, profile, animation and shared voice-RNG owners. Other
// roster entries, page2 and reset popup remain unavailable before mutation.
// The frontend owns sole manager.tick, profile commit, GPU upload/publication,
// source-renderer retirement, and application queue consumption.
class OriginalUiRiderSession50KG final {
public:
    OriginalUiRiderSession50KG() noexcept;
    ~OriginalUiRiderSession50KG();
    OriginalUiRiderSession50KG(const OriginalUiRiderSession50KG&)=delete;
    OriginalUiRiderSession50KG& operator=(const OriginalUiRiderSession50KG&)=delete;
    OriginalUiRiderSessionStatus50KG bind(const OriginalUiRiderSessionCold50KG&,
        OriginalUiRiderSessionDiagnostic50KG&) noexcept;
    bool ownsPreparedPage(std::uint8_t) const noexcept;
    bool canEnterMode(std::uint32_t) const noexcept;
    std::uint32_t intendedMode() const noexcept;
    const game::OriginalUiTransitionOwner50KG* externalLifecycleOwner() const noexcept;
    OriginalUiRiderSessionStatus50KG notifyTransition(const game::OriginalUiTransitionNotification50KG&,
        OriginalUiRiderSessionDiagnostic50KG&) noexcept;
    game::OriginalUiTransitionCompletion50KG pollCompletion(game::OriginalUiTransitionRoute50KG,
        game::OriginalUiTransitionPhase50KG) const noexcept;
    OriginalUiRiderSessionStatus50KG input(const OriginalUiMainSessionInput50KG&,
        OriginalUiRiderSessionInputReceipt50KG&,OriginalUiRiderSessionDiagnostic50KG&) noexcept;
    bool routeProposalCurrent(const OriginalUiRiderRouteProposal50KG&) const noexcept;
    // Called immediately AFTER actual manager admission + private profile
    // commit. Revalidates the exact one-generation profile change and manager
    // request, then performs source route-specific effects once. Confirm never
    // borrows Back bypass; original selected-animation readiness remains real.
    OriginalUiRiderSessionStatus50KG notifyRouteAccepted(const OriginalUiRiderRouteProposal50KG&,
        OriginalUiRiderSessionDiagnostic50KG&) noexcept;
    // Same one-use audio scheduling boundary as Main/Options. Actual external
    // manager.tick occurs between these calls; shared PCM is mixed only by
    // the application after its accepted source interval, never by a page.
    OriginalUiRiderSessionStatus50KG prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG&,
        OriginalUiRiderSessionDiagnostic50KG&) noexcept;
    OriginalUiRiderSessionStatus50KG update(const OriginalUiMainSessionUpdate50KG&,
        OriginalUiRiderSessionDiagnostic50KG&) noexcept;
    OriginalUiRiderSessionStatus50KG render(const OriginalUiRiderDrawClock50KG&,
        OriginalUiRiderSessionDiagnostic50KG&) noexcept;
    // Call after cold preflight, and after source callbacks/update before
    // rendering. The actual bank may stay unchanged on visibility-only work.
    // prepareBank never uploads or certifies live selection/attachment.
    OriginalUiRiderSessionStatus50KG prepareBank(OriginalUiRiderSessionDiagnostic50KG&) noexcept;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank() const noexcept;
    const std::vector<MaterialTextureSampler50EB>* samplers() const noexcept;
    OriginalUiRiderSessionStatus50KG bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB>,
        OriginalUiRiderSessionDiagnostic50KG&) noexcept;
    const OriginalUiSourceFrame50KM* sourceFrame() const noexcept;
    OriginalUiRiderSessionSnapshot50KG snapshot() const noexcept;
    // CPU borrow release only. Frontend MUST additionally retire rendererOwner
    // while this object lives before destruction or bank replacement.
    void drain() noexcept;
    void requestQuit() noexcept;
private:
    friend class OriginalUiFrontend50KG;
    OriginalUiRiderSessionStatus50KG commitMove(const OriginalUiRiderSessionInputReceipt50KG&,
        VehicleWindowGl50DS&,VehicleWindowDiagnostic50DS&,OriginalUiRiderSessionDiagnostic50KG&) noexcept;
    // Public bind prepares the exact19+20 immutable union. A cold population
    // never selects or registers a model; real callbacks and renderer-first
    // retirement remain required at each entry/direction boundary.
    OriginalUiRiderSessionStatus50KG bindCold(const OriginalUiRiderSessionCold50KG&,
        game::OriginalUiRiderColdPopulation50KG,OriginalUiRiderSessionDiagnostic50KG&) noexcept;
    const OriginalUiRiderDrawContribution50KG* rendererOwner() const noexcept;
    bool sourceTeardownPending()const noexcept;
    // Frontend-only: obtains genuine completion directly from its renderer,
    // then destroys dynamic domains and consumes their exact retirements while
    // both ledger and collector still live. Failure retains unfinished owners.
    OriginalUiRiderSessionStatus50KG teardownSource(VehicleWindowGl50DS*,
        VehicleWindowDiagnostic50DS&,OriginalUiRiderSessionDiagnostic50KG&) noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
