#pragma once
// UIWin manager (screen 7, PodUICircuitWin 1D1550) with its single page
// PodUIPageCircuitWin (2076A0). Same session shape as the Hall of Fame.
#include "original_ui_win_draw_contribution50pd.h"
#include "original_ui_rider_profile50kg.h"
#include "original_ui_main_session50kg.h"
#include "../../../src/game/original_ui_transition_owner50kg.h"
namespace rr::platform {
// Menu-audio leaves this page uses, implemented on the frontend's sole
// OriginalUiMainAudio50KG transport. Each method is the native form of one
// original call; can* are read-only preflights of exactly that call.
class OriginalUiWinAudio50PD {
public:
    virtual ~OriginalUiWinAudio50PD()=default;
    // Song10 Music\UI\Screens\trphy1.vag and Song8 trphy2_3.vag prepared.
    virtual bool winMenuReady() const noexcept=0;
    // Opening 208810: 234880(stream,song,1). a2=1 first runs 234440(stream,0.0)
    // (immediate stop), then stores stream+40 = 1 (234900 stores 4 = loop) and
    // starts the song through 234170: a single, non-looping play.
    virtual bool canStartSong234880(std::uint32_t song) const noexcept=0;
    virtual bool startSong234880(std::uint32_t song) noexcept=0;
    // Closing 208950: 234440(stream,1.0).
    virtual bool canFadeMusic234440() const noexcept=0;
    virtual bool fadeMusic234440(float seconds) noexcept=0;
    // Input 2089D0: 292360(UImgr+40,1,1,0) SELECT1 (select.vag), then 267BE0.
    virtual bool canPlaySelect292360() const noexcept=0;
    virtual bool playSelect292360() noexcept=0;
    virtual void pause(bool) noexcept=0;
    virtual bool update(float sourceSeconds) noexcept=0;
};
enum class OriginalUiWinSessionStatus50PD:std::uint8_t {
    ok,noChange,routeProposed,unavailableDestination,notBound,alreadyBound,invalidInput,resource,profile,transition,
    aperture,widget,model,audio,draw,unsupportedState,busy,faulted,quit
};
struct OriginalUiWinSessionDiagnostic50PD {
    OriginalUiWinSessionStatus50PD status=OriginalUiWinSessionStatus50PD::ok;std::uint32_t sourcePC=0;
    OriginalUiWinContributionDiagnostic50PD draw{};game::OriginalUiWinPageDiagnostic50PD page{};
    game::OriginalUiWinSceneDiagnostic50PD scene{};game::OriginalUiApertureDiagnostic50KG aperture{};
};
struct OriginalUiWinSessionCold50PD {
    std::shared_ptr<OriginalUiWinDrawResources50PD> resources;
    std::shared_ptr<const MaterialTextureGl50EB> gpu; // published resources->textures()/samplers()
    std::shared_ptr<const assets::OriginalUiArchive50KG> winArchive;
    std::shared_ptr<OriginalUiRiderProfileOwner50KG> profile;
    std::shared_ptr<OriginalUiMainProfile50KG> commonProfile;
    std::shared_ptr<OriginalUiMainApplication50KG> application;
    OriginalUiMainAudioMode50KG audioMode=OriginalUiMainAudioMode50KG::enabled;
    std::shared_ptr<OriginalUiWinAudio50PD> audio; // null exactly when muted
    std::shared_ptr<game::OriginalUiTransitionOwner50KG> externalTransition; // unbound at bind
};
class OriginalUiWinSession50PD;
// Activate (event4): screen10/1 when 187E30 is pending, else screen13/1.
struct OriginalUiWinRouteProposal50PD {
    const OriginalUiWinSession50PD* source=nullptr;std::uint64_t serial=0;
    game::OriginalUiRouteDecision50KG decision{};OriginalUiRiderProfileStamp50KG profileStamp{};
};
struct OriginalUiWinSessionInputReceipt50PD {
    bool containerReached=false,pageCallbackRan=false,pageAccepted=false;
    OriginalUiWinRouteProposal50PD routeProposal{};
    OriginalUiWinSessionStatus50PD status=OriginalUiWinSessionStatus50PD::noChange;
};
struct OriginalUiWinSessionSnapshot50PD {
    bool bound=false,faulted=false,quitRequested=false,pageShown=false,inputContainerShown=false,cameraActive=false,framePublished=false;
    bool routeProposalPending=false;
    std::uint32_t packedBackground430=0,promptMask42C=0,career=0;
    game::OriginalUiWinCircuit50PD circuit{};std::uint16_t selectedPlace=0;
    std::uint64_t updateCount=0,renderCount=0;
    game::OriginalUiTransitionSnapshot50KG transition{};game::OriginalUiApertureSnapshot50KG aperture{};
    OriginalUiMainSessionAudioReceipt50KG audio{};
};
class OriginalUiWinSession50PD final {
public:
    OriginalUiWinSession50PD()noexcept;~OriginalUiWinSession50PD();
    OriginalUiWinSession50PD(const OriginalUiWinSession50PD&)=delete;OriginalUiWinSession50PD& operator=(const OriginalUiWinSession50PD&)=delete;
    OriginalUiWinSessionStatus50PD bind(const OriginalUiWinSessionCold50PD&,OriginalUiWinSessionDiagnostic50PD&)noexcept;
    OriginalUiWinSessionStatus50PD notifyTransition(const game::OriginalUiTransitionNotification50KG&,OriginalUiWinSessionDiagnostic50PD&)noexcept;
    game::OriginalUiTransitionCompletion50KG pollCompletion(game::OriginalUiTransitionRoute50KG,game::OriginalUiTransitionPhase50KG)const noexcept;
    const game::OriginalUiTransitionOwner50KG* externalLifecycleOwner()const noexcept;
    bool ownsPreparedPage(std::uint8_t)const noexcept;
    OriginalUiWinSessionStatus50PD input(const OriginalUiMainSessionInput50KG&,OriginalUiWinSessionInputReceipt50PD&,OriginalUiWinSessionDiagnostic50PD&)noexcept;
    bool routeProposalCurrent(const OriginalUiWinRouteProposal50PD&)const noexcept;
    // After the frontend's requestScreen(10|13,1) succeeded: 292360 + 267BE0.
    OriginalUiWinSessionStatus50PD notifyRouteAccepted(const OriginalUiWinRouteProposal50PD&,OriginalUiWinSessionDiagnostic50PD&)noexcept;
    OriginalUiWinSessionStatus50PD prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG&,OriginalUiWinSessionDiagnostic50PD&)noexcept;
    OriginalUiWinSessionStatus50PD update(const OriginalUiMainSessionUpdate50KG&,OriginalUiWinSessionDiagnostic50PD&)noexcept;
    OriginalUiWinSessionStatus50PD render(const OriginalUiMainFrameClock50KG&,OriginalUiWinSessionDiagnostic50PD&)noexcept;
    OriginalUiWinSessionSnapshot50PD snapshot()const noexcept;
    const OriginalUiNativeDraw50KG* draw()const noexcept;
    const OriginalUiSourceFrame50KM* sourceFrame()const noexcept;
    void requestQuit()noexcept;
private:
    friend class OriginalUiFrontend50KG;
    bool sourceTeardownPending()const noexcept;
    OriginalUiWinSessionStatus50PD teardownSource(VehicleWindowGl50DS*,OriginalUiWinSessionDiagnostic50PD&)noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
