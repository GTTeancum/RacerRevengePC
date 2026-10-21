#pragma once
#include "original_ui_hall_draw_contribution50kg.h"
#include "original_ui_rider_profile50kg.h"
#include "../../../src/game/original_ui_hall_controller50kg.h"
namespace rr::platform {
struct OriginalUiHallContributionDiagnostic50KG;
enum class OriginalUiHallSessionStatus50KG:std::uint8_t {ok,noChange,backProposed,unavailableDestination,notBound,alreadyBound,invalidInput,resource,profile,transition,aperture,widget,audio,draw,unsupportedState,busy,faulted,quit};
struct OriginalUiHallSessionDiagnostic50KG {OriginalUiHallSessionStatus50KG status=OriginalUiHallSessionStatus50KG::ok;std::uint32_t sourcePC=0;OriginalUiHallDrawDiagnostic50KG resource{};OriginalUiHallContributionDiagnostic50KG draw{};game::OriginalUiHallWidgetDiagnostic50KG widget{};game::OriginalUiApertureDiagnostic50KG aperture{};};
struct OriginalUiHallSessionCold50KG {
 std::shared_ptr<OriginalUiHallDrawResources50KG> resources;
 std::shared_ptr<const MaterialTextureGl50EB> gpu;
 std::shared_ptr<const assets::OriginalUiArchive50KG> hallArchive;
 std::shared_ptr<OriginalUiRiderProfileOwner50KG> profile;
 std::shared_ptr<OriginalUiMainProfile50KG> commonProfile;
 std::shared_ptr<OriginalUiMainApplication50KG> application;
 OriginalUiMainAudioMode50KG audioMode=OriginalUiMainAudioMode50KG::enabled;
 std::shared_ptr<OriginalUiMainAudio50KG> audio;
 std::shared_ptr<game::OriginalUiTransitionOwner50KG> externalTransition;
};
class OriginalUiHallOfFameSession50KG;
struct OriginalUiHallRouteProposal50KG {const OriginalUiHallOfFameSession50KG* source=nullptr;std::uint64_t serial=0;game::OriginalUiRouteDecision50KG decision{};OriginalUiRiderProfileStamp50KG profileStamp{};};
struct OriginalUiHallSessionInputReceipt50KG {bool containerReached=false,pageCallbackRan=false,pageAccepted=false,movementAccepted=false;OriginalUiHallRouteProposal50KG routeProposal{};OriginalUiHallSessionStatus50KG status=OriginalUiHallSessionStatus50KG::noChange;};
struct OriginalUiHallSessionSnapshot50KG {
 bool bound=false,faulted=false,quitRequested=false,pageShown=false,inputContainerShown=false,cameraActive=false,framePublished=false,backProposalPending=false;
 bool videoPaused84=false,movementPresentationPending=false;
 float deferred1460=0;std::uint32_t packedBackground430=0,promptMask42C=0,courseIndex=0,planetIndex=0;
 std::uint64_t updateCount=0,renderCount=0;
 game::OriginalUiTransitionSnapshot50KG transition{};game::OriginalUiApertureSnapshot50KG aperture{};
 OriginalUiMainSessionAudioReceipt50KG audio{};
};
// Mode5-only Hall manager. Original post-race Records paths remain closed.
class OriginalUiHallOfFameSession50KG final {
public:
 OriginalUiHallOfFameSession50KG()noexcept;~OriginalUiHallOfFameSession50KG();
 OriginalUiHallOfFameSession50KG(const OriginalUiHallOfFameSession50KG&)=delete;OriginalUiHallOfFameSession50KG& operator=(const OriginalUiHallOfFameSession50KG&)=delete;
 OriginalUiHallSessionStatus50KG bind(const OriginalUiHallSessionCold50KG&,OriginalUiHallSessionDiagnostic50KG&)noexcept;
 OriginalUiHallSessionStatus50KG notifyTransition(const game::OriginalUiTransitionNotification50KG&,OriginalUiHallSessionDiagnostic50KG&)noexcept;
 game::OriginalUiTransitionCompletion50KG pollCompletion(game::OriginalUiTransitionRoute50KG,game::OriginalUiTransitionPhase50KG)const noexcept;
 const game::OriginalUiTransitionOwner50KG* externalLifecycleOwner()const noexcept;
 bool ownsPreparedPage(std::uint8_t)const noexcept;
 OriginalUiHallSessionStatus50KG input(const OriginalUiMainSessionInput50KG&,OriginalUiHallSessionInputReceipt50KG&,OriginalUiHallSessionDiagnostic50KG&)noexcept;
 bool routeProposalCurrent(const OriginalUiHallRouteProposal50KG&)const noexcept;
 OriginalUiHallSessionStatus50KG notifyRouteAccepted(const OriginalUiHallRouteProposal50KG&,OriginalUiHallSessionDiagnostic50KG&)noexcept;
 OriginalUiHallSessionStatus50KG prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG&,OriginalUiHallSessionDiagnostic50KG&)noexcept;
 OriginalUiHallSessionStatus50KG update(const OriginalUiMainSessionUpdate50KG&,OriginalUiHallSessionDiagnostic50KG&)noexcept;
 OriginalUiHallSessionStatus50KG render(const OriginalUiMainFrameClock50KG&,OriginalUiHallSessionDiagnostic50KG&)noexcept;
 OriginalUiHallSessionSnapshot50KG snapshot()const noexcept;
 const game::OriginalUiHallState50KG* navigationState()const noexcept;
 const OriginalUiNativeDraw50KG* draw()const noexcept;
 const OriginalUiSourceFrame50KM* sourceFrame()const noexcept;
 void requestQuit()noexcept;
private:
 friend class OriginalUiFrontend50KG;
 // Prepared against the exact old common profile, only after source17DE50
 // mode/previous-mode writes. Options remembered slot lives in its own owner.
 bool acceptEntryProfileCommit()noexcept;
 bool commitBack(const OriginalUiHallRouteProposal50KG&,const game::OriginalUiRouteDecision50KG&)noexcept;
 bool canBeginMovement()const noexcept;
 bool movementInProgress()const noexcept;
 // Called only after concrete renderer retain/blend/publish/retire receipts.
 bool completeMovementPresentation()noexcept;
 bool sourceTeardownPending()const noexcept;
 OriginalUiHallSessionStatus50KG teardownSource(VehicleWindowGl50DS*,OriginalUiHallSessionDiagnostic50KG&)noexcept;
 struct Storage;std::unique_ptr<Storage> storage_;
};
}
