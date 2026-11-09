#pragma once
// UIUnlock screen 10 / page 1 session (PodUIUnlock, page PodUIPageUnlock).
// Same frontend shape as the Hall session: bind from a cold struct, manager
// transition notifications, aperture completion polling, input returning a
// typed route proposal (re-show page 1, or leave to Garage 13/1 / Course 6/1),
// update, render to one sealed source frame, and renderer-first teardown.
// The profile grant of 20B480 is performed inside the admitted route commit.
#include "original_ui_unlock_draw_contribution50pd.h"
#include "original_ui_rider_profile50kg.h"
#include "../../../src/game/original_ui_unlock_page50pd.h"
#include <optional>
namespace rr::platform {
class OriginalUiFrontend50KG;
enum class OriginalUiUnlockSessionStatus50PD:std::uint8_t {
    ok,noChange,routeProposed,unavailableDestination,notBound,alreadyBound,invalidInput,resource,profile,
    transition,aperture,widget,audio,draw,unsupportedState,busy,faulted,quit
};
struct OriginalUiUnlockSessionDiagnostic50PD {
    OriginalUiUnlockSessionStatus50PD status=OriginalUiUnlockSessionStatus50PD::ok;std::uint32_t sourcePC=0;
    OriginalUiUnlockDrawDiagnostic50PD resource{};OriginalUiUnlockContributionDiagnostic50PD draw{};
    game::OriginalUiUnlockWidgetDiagnostic50PD widget{};game::OriginalUiApertureDiagnostic50KG aperture{};
    assets::OriginalUiPageDiagnostic50KG page{};
};
struct OriginalUiUnlockSessionCold50PD {
    std::shared_ptr<OriginalUiUnlockDrawResources50PD> resources;
    std::shared_ptr<const MaterialTextureGl50EB> gpu;
    std::shared_ptr<const assets::OriginalUiArchive50KG> unlockArchive;
    // Copied synchronously at bind (loc 0x151..0x154, pod names, rival P+3C names).
    const assets::OriginalUiEnglish50KG* english=nullptr;
    std::shared_ptr<OriginalUiRiderProfileOwner50KG> profile;
    std::shared_ptr<OriginalUiMainProfile50KG> commonProfile;
    std::shared_ptr<OriginalUiMainApplication50KG> application;
    // Actual Application+344 (argv producer; 209CCC / 20AE68 read it). The
    // nonzero debug branches (texture export, P+1125F nibble=7) are not
    // implemented; bind requires the real value and refuses nonzero.
    std::optional<std::int32_t> application344;
    OriginalUiMainAudioMode50KG audioMode=OriginalUiMainAudioMode50KG::enabled;
    // Enabled mode needs Song7 (riderMenuReady) and UIEVENTS select 1.
    std::shared_ptr<OriginalUiMainAudio50KG> audio;
    // Fresh, unbound sole manager owner; the frontend binds it (bindInitialPage
    // 10/1 after a race: this screen has no predecessor page).
    std::shared_ptr<game::OriginalUiTransitionOwner50KG> externalTransition;
};
class OriginalUiUnlockSession50PD;
struct OriginalUiUnlockRouteProposal50PD {
    const OriginalUiUnlockSession50PD* source=nullptr;std::uint64_t serial=0;
    game::OriginalUiUnlockNext50PD next=game::OriginalUiUnlockNext50PD::none;
    // reshow: changePage 10/1 -> 10/1 (vtable+A4, manager page request);
    // exit: changeScreen 10/1 -> 13/1 (mode34==2) or 6/1 (17DE10(App,2,s,1) at 2A3F70).
    game::OriginalUiRouteDecision50KG decision{};
    // 20B4B0..20B5E4 grant made inside the admitted commit (first pending flag).
    bool grant=false;std::size_t grantCheck=0;std::uint32_t grantTier=0;
    OriginalUiRiderProfileStamp50KG profileStamp{};
};
struct OriginalUiUnlockSessionInputReceipt50PD {
    bool containerReached=false,pageCallbackRan=false,pageAccepted=false;
    OriginalUiUnlockRouteProposal50PD routeProposal{};
    OriginalUiUnlockSessionStatus50PD status=OriginalUiUnlockSessionStatus50PD::noChange;
};
struct OriginalUiUnlockSessionSnapshot50PD {
    bool bound=false,faulted=false,quitRequested=false,pageShown=false,inputContainerShown=false,framePublished=false;
    bool routeProposalPending=false,routeCommitted=false;
    std::int32_t pod2D40=-1;std::uint32_t promptMask42C=0,packedBackground430=0,activations=0;
    game::OriginalUnlockChecks50PD pending{};
    game::OriginalUiUnlockGallery50PD pageBackground=game::OriginalUiUnlockGallery50PD::none;
    std::uint64_t updateCount=0,renderCount=0,grants=0;
    game::OriginalUiTransitionSnapshot50KG transition{};game::OriginalUiApertureSnapshot50KG aperture{};
    OriginalUiMainSessionAudioReceipt50KG audio{};
};
class OriginalUiUnlockSession50PD final {
public:
    OriginalUiUnlockSession50PD() noexcept;~OriginalUiUnlockSession50PD();
    OriginalUiUnlockSession50PD(const OriginalUiUnlockSession50PD&)=delete;
    OriginalUiUnlockSession50PD& operator=(const OriginalUiUnlockSession50PD&)=delete;
    OriginalUiUnlockSessionStatus50PD bind(const OriginalUiUnlockSessionCold50PD&,OriginalUiUnlockSessionDiagnostic50PD&) noexcept;
    OriginalUiUnlockSessionStatus50PD notifyTransition(const game::OriginalUiTransitionNotification50KG&,OriginalUiUnlockSessionDiagnostic50PD&) noexcept;
    game::OriginalUiTransitionCompletion50KG pollCompletion(game::OriginalUiTransitionRoute50KG,game::OriginalUiTransitionPhase50KG) const noexcept;
    const game::OriginalUiTransitionOwner50KG* externalLifecycleOwner() const noexcept;
    bool ownsPreparedPage(std::uint8_t) const noexcept;
    OriginalUiUnlockSessionStatus50PD input(const OriginalUiMainSessionInput50KG&,OriginalUiUnlockSessionInputReceipt50PD&,
        OriginalUiUnlockSessionDiagnostic50PD&) noexcept;
    bool routeProposalCurrent(const OriginalUiUnlockRouteProposal50PD&) const noexcept;
    // After manager admission + commitRoute: plays UI sound 1 (292360 + 267BE0).
    OriginalUiUnlockSessionStatus50PD notifyRouteAccepted(const OriginalUiUnlockRouteProposal50PD&,OriginalUiUnlockSessionDiagnostic50PD&) noexcept;
    OriginalUiUnlockSessionStatus50PD prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG&,OriginalUiUnlockSessionDiagnostic50PD&) noexcept;
    OriginalUiUnlockSessionStatus50PD update(const OriginalUiMainSessionUpdate50KG&,OriginalUiUnlockSessionDiagnostic50PD&) noexcept;
    OriginalUiUnlockSessionStatus50PD render(const OriginalUiMainFrameClock50KG&,OriginalUiUnlockSessionDiagnostic50PD&) noexcept;
    OriginalUiUnlockSessionSnapshot50PD snapshot() const noexcept;
    const game::OriginalUiUnlockState50PD* pageState() const noexcept;
    const OriginalUiNativeDraw50KG* draw() const noexcept;
    const OriginalUiSourceFrame50KM* sourceFrame() const noexcept;
    void requestQuit() noexcept;
private:
    friend class OriginalUiFrontend50KG;
    // Frontend commitRouteSelection callback: revalidates the proposal against
    // the admitted decision, then performs the original grant (profile
    // grantUnlock20B480) and clears that flag. False leaves everything unchanged.
    bool commitRoute(const OriginalUiUnlockRouteProposal50PD&,const game::OriginalUiRouteDecision50KG& admitted) noexcept;
    bool sourceTeardownPending() const noexcept;
    OriginalUiUnlockSessionStatus50PD teardownSource(VehicleWindowGl50DS*,OriginalUiUnlockSessionDiagnostic50PD&) noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
