#pragma once
#include "original_ui_options_draw_contribution50kg.h"
#include "original_ui_main_session50kg.h"
namespace rr::platform {
// The source +6E4 word is separate from Main's +6E0. Import/reset it only from
// an authenticated profile owner. No implicit default or fabricated save file.
struct OriginalUiOptionsProfile50KG {bool known=false;std::uint32_t savedSlot6E4=0;};
// Explicit original reset186660/186668, not an automatic missing-save fallback.
// Does not assert the caller imported/created a valid profile lifecycle.
void resetOriginalUiOptionsProfileFields50KG(OriginalUiOptionsProfile50KG&) noexcept;
enum class OriginalUiOptionsSessionStatus50KG : std::uint8_t {
    ok,noChange,backProposed,pageProposed,unavailableDestination,secondControllerRequired,
    notBound,alreadyBound,invalidInput,unsupportedState,resource,widget,aperture,
    transition,particle,audio,draw,allocation,faulted,quit
};
struct OriginalUiOptionsSessionDiagnostic50KG {
    OriginalUiOptionsSessionStatus50KG status=OriginalUiOptionsSessionStatus50KG::ok;
    std::uint32_t sourcePC=0;
    OriginalUiOptionsContributionDiagnostic50KG draw{};
    game::OriginalUiOptionsWidgetDiagnostic50KG widget{};
    game::OriginalUiApertureDiagnostic50KG aperture{};
};
struct OriginalUiOptionsSessionCold50KG {
    std::shared_ptr<OriginalUiOptionsDrawResources50KG> resources;
    std::shared_ptr<const MaterialTextureGl50EB> gpu;
    std::shared_ptr<const assets::OriginalUiArchive50KG> optionsArchive;
    std::shared_ptr<OriginalUiOptionsProfile50KG> profile;
    // Optional sole application profile owner for page4 fields. Missing owner
    // keeps that page unavailable; known must come from real profile lifecycle.
    std::shared_ptr<game::OriginalUiGameSettingsProfile50KG> gameSettingsProfile;
    std::shared_ptr<OriginalUiMainProfile50KG> commonProfile;
    std::shared_ptr<OriginalUiMainApplication50KG> application;
    OriginalUiMainAudioMode50KG audioMode=OriginalUiMainAudioMode50KG::enabled;
    std::shared_ptr<OriginalUiMainAudio50KG> audio;
    // Sole actual transition manager, cold and unbound during page preparation.
    // Coordinator binds its callbacks afterwards and alone advances its tick.
    std::shared_ptr<game::OriginalUiTransitionOwner50KG> externalTransition;
};
class OriginalUiOptionsSession50KG;
struct OriginalUiOptionsRouteProposal50KG {
    const OriginalUiOptionsSession50KG* source=nullptr;
    std::uint64_t serial=0;
    game::OriginalUiRouteDecision50KG decision{};
};
struct OriginalUiOptionsSessionInputReceipt50KG {
    bool containerReached=false,pageCallbackRan=false,pageAccepted=false;
    game::OriginalUiOptionsMenuPageResult50KG page{};
    game::OriginalUiMovementNotification50KG movement{};
    game::OriginalUiGameOptionsPlan50KG gameOptions{};
    game::OriginalUiTransitionSnapshot50KG sourceLifecycle{};
    OriginalUiOptionsRouteProposal50KG routeProposal{};
    OriginalUiOptionsSessionStatus50KG status=OriginalUiOptionsSessionStatus50KG::noChange;
};
struct OriginalUiOptionsSessionSnapshot50KG {
    bool bound=false,faulted=false,quitRequested=false,pageShown=false,inputContainerShown=false;
    bool cameraActive=false,framePublished=false,backProposalPending=false;
    std::uint32_t packedBackground430=0,promptMask42C=0;
    std::uint64_t updateCount=0,renderCount=0;
    game::OriginalUiTransitionSnapshot50KG transition{};
    game::OriginalUiApertureSnapshot50KG aperture{};
    OriginalUiMainSessionAudioReceipt50KG audio{};
    game::OriginalUiMainFireReceipt50KG lastParticleAdvance{},lastFireEmit{};
};
// Options menu and bounded Game Options edit/cancel page/scene owner. Page4
// Accept needs a genuine sound-mode application owner and stays unavailable.
// Pages2/3/5/6 and HOF remain unavailable.
// Cross-screen admission, sole manager tick, deferred queue consumption and
// renderer-bank publication belong to the root coordinator, not this page.
class OriginalUiOptionsSession50KG final {
public:
    OriginalUiOptionsSession50KG() noexcept;
    ~OriginalUiOptionsSession50KG();
    OriginalUiOptionsSession50KG(const OriginalUiOptionsSession50KG&)=delete;
    OriginalUiOptionsSession50KG& operator=(const OriginalUiOptionsSession50KG&)=delete;
    OriginalUiOptionsSessionStatus50KG bind(const OriginalUiOptionsSessionCold50KG&,
        OriginalUiOptionsSessionDiagnostic50KG&) noexcept;
    OriginalUiOptionsSessionStatus50KG notifyTransition(const game::OriginalUiTransitionNotification50KG&,
        OriginalUiOptionsSessionDiagnostic50KG&) noexcept;
    game::OriginalUiTransitionCompletion50KG pollCompletion(game::OriginalUiTransitionRoute50KG,game::OriginalUiTransitionPhase50KG) const noexcept;
    const game::OriginalUiTransitionOwner50KG* externalLifecycleOwner() const noexcept;
    bool ownsPreparedPage(std::uint8_t page) const noexcept;
    OriginalUiOptionsSessionStatus50KG input(const OriginalUiMainSessionInput50KG&,
        OriginalUiOptionsSessionInputReceipt50KG&,OriginalUiOptionsSessionDiagnostic50KG&) noexcept;
    // Root admits the concrete destination, commits exact +6E4 or +6E8 from the
    // proposal in transition.commitRouteSelection, then immediately invokes
    // notifyRouteAccepted BEFORE ticking. This verifies the real manager's
    // actual screen/page request and profile write, then emits SELECT1 or BACK5
    // once. It writes
    // neither profile nor route. Intervening input/update/render/notice makes
    // the source proposal stale; its flags never certify a destination.
    bool routeProposalCurrent(const OriginalUiOptionsRouteProposal50KG&) const noexcept;
    OriginalUiOptionsSessionStatus50KG notifyRouteAccepted(const OriginalUiOptionsRouteProposal50KG&,
        OriginalUiOptionsSessionDiagnostic50KG&) noexcept;
    // Required one-use scheduler boundary: audio preparation, actual external
    // manager.tick, then update with identical arguments. No duplicate audio.
    OriginalUiOptionsSessionStatus50KG prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG&,
        OriginalUiOptionsSessionDiagnostic50KG&) noexcept;
    // Actual child traversal only; consumes the matching prepared update.
    OriginalUiOptionsSessionStatus50KG update(const OriginalUiMainSessionUpdate50KG&,
        OriginalUiOptionsSessionDiagnostic50KG&) noexcept;
    OriginalUiOptionsSessionStatus50KG render(const OriginalUiOptionsClock50KG&,
        OriginalUiOptionsSessionDiagnostic50KG&) noexcept;
    OriginalUiOptionsSessionSnapshot50KG snapshot() const noexcept;
    const OriginalUiNativeDraw50KG* draw() const noexcept;
    void requestQuit() noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
