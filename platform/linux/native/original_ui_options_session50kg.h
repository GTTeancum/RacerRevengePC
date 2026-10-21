#pragma once
#include "original_ui_options_draw_contribution50kg.h"
#include "original_ui_main_session50kg.h"
#include "original_sound_mode50kg.h"
#include "original_ui_audio_options50kg.h"
#include "../../../src/game/original_ui_game_options_apply50kg.h"
#include "../../../src/game/original_ui_controls_controller50kg.h"
#include "../../../src/game/original_ui_memory_card50kg.h"
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
    transition,particle,audio,draw,allocation,faulted,quit,screenProposed
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
    std::shared_ptr<game::ControlPresets49D> controlsProfile;
    // Optional sole application profile owner for page4 fields. Missing owner
    // keeps that page unavailable; known must come from real profile lifecycle.
    std::shared_ptr<game::OriginalUiGameSettingsProfile50KG> gameSettingsProfile;
    // Same process owner attached to audio's actual frontend mixer. Optional;
    // without that exact concrete attachment, page4 Accept remains closed.
    std::shared_ptr<OriginalSoundMode50KG> soundMode;
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
    bool popupChanged=false;
    std::uint32_t popupSourcePC=0;
    game::OriginalUiOptionsMenuPageResult50KG page{};
    game::OriginalUiMovementNotification50KG movement{};
    game::OriginalUiGameOptionsPlan50KG gameOptions{};
    game::OriginalUiControlsPlan50KG controls{};
    game::OriginalUiTransitionSnapshot50KG sourceLifecycle{};
    OriginalUiOptionsRouteProposal50KG routeProposal{};
    OriginalUiOptionsSessionStatus50KG status=OriginalUiOptionsSessionStatus50KG::noChange;
};
struct OriginalUiOptionsSessionSnapshot50KG {
    bool bound=false,faulted=false,quitRequested=false,pageShown=false,inputContainerShown=false;
    bool cameraActive=false,framePublished=false,backProposalPending=false;
    game::OriginalUiControllerPopupState50KG controllerPopup{};
    std::uint32_t packedBackground430=0,promptMask42C=0;
    std::uint64_t updateCount=0,renderCount=0;
    std::uint64_t gameOptionsAcceptCount=0,lastGameOptionsSoundRevision=0,controlsAcceptCount=0;
    game::OriginalUiTransitionSnapshot50KG transition{};
    game::OriginalUiApertureSnapshot50KG aperture{};
    OriginalUiMainSessionAudioReceipt50KG audio{};
    game::OriginalUiMainFireReceipt50KG lastParticleAdvance{},lastFireEmit{};
};
// Options menu, authentic Controls and bounded Game Options scene owner. Page4 Accept needs
// the exact shared profile and actual frontend PCM sound-mode consumer.
// Page3 remains unadmitted despite the genuine absent-controller popup join.
// MemoryCard page5 admits only authenticated state4 entry/Back with a live direct-video owner.
// Storage actions remain unavailable before mutation; connected Controls2 stays closed.
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
    bool pendingAutomaticMemoryCardReturn(OriginalUiOptionsRouteProposal50KG&) const noexcept;
    bool pendingAutomaticCreditsReturn(OriginalUiOptionsRouteProposal50KG&) const noexcept;
    OriginalUiOptionsSessionStatus50KG input(const OriginalUiMainSessionInput50KG&,
        OriginalUiOptionsSessionInputReceipt50KG&,OriginalUiOptionsSessionDiagnostic50KG&) noexcept;
    // Root admits the concrete destination, commits the exact profile proposal
    // in transition.commitRouteSelection, then immediately invokes
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
    // Committed profile projection for the parent-owned race/profile bridge.
    // Never exposes local edits or claims save/race-group application.
    bool appliedAudioOptions(game::OriginalUiAudioOptions50KG&) const noexcept;
    const game::OriginalUiControlsState50KG* controlsState(std::uint8_t controllerIndex) const noexcept;
    const game::OriginalUiMemoryCardState50KG* memoryCardState() const noexcept;
    const OriginalUiNativeDraw50KG* draw() const noexcept;
    const OriginalUiSourceFrame50KM* sourceFrame() const noexcept;
    void requestQuit() noexcept;
private:
    friend class OriginalUiFrontend50KG;
    // Only the concrete coordinator's manager commit callback may call this,
    // after destination admission. A holder of a proposal cannot manually
    // promote its flags and invoke the transaction without that actual manager.
    // Revalidates both decisions, full profile/local snapshot, sound owner,
    // mixer epoch/revision and SELECT readiness. No user callback or allocation
    // occurs between source1F66F0 writes,188740 and group6E8. Manager publishes
    // its page request immediately after this returns; notify emits SELECT.
    bool commitGameOptionsAccept(const OriginalUiOptionsRouteProposal50KG&,
        const game::OriginalUiRouteDecision50KG& admittedDecision) noexcept;
    bool sourceTeardownPending() const noexcept;
    OriginalUiOptionsSessionStatus50KG teardownSource(VehicleWindowGl50DS*,OriginalUiOptionsSessionDiagnostic50KG&) noexcept;
    bool commitControlsReturn(const OriginalUiOptionsRouteProposal50KG&,
        const game::OriginalUiRouteDecision50KG& admittedDecision) noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
