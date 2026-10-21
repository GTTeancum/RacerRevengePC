#pragma once
#include "original_ui_tournament_widgets50kg.h"
#include "original_ui_tournament_draw_contribution50kg.h"
#include "original_ui_tournament_career_resources50kg.h"
#include "original_ui_rider_session50kg.h"
#include "original_ui_course_session50kg.h"
#include <variant>
#include "../../../src/game/original_ui_tournament_camera50kg.h"
#include "../../../src/game/original_ui_tournament_clean_lifecycle50kg.h"
#include "../../../src/game/original_ui_tournament_input50kg.h"
#include "../../../src/game/original_ui_tournament_pod_owner50kg.h"
namespace rr::platform {
class OriginalUiTournamentDrawContribution50KG;

enum class OriginalUiTournamentSessionStatus50KG:std::uint8_t {ok,noChange,backProposed,unavailableDestination,notBound,alreadyBound,invalidInput,resource,profile,transition,aperture,widget,audio,camera,model,draw,unsupportedState,busy,faulted,quit,continueProposed};
struct OriginalUiTournamentSessionDiagnostic50KG {
 OriginalUiTournamentSessionStatus50KG status=OriginalUiTournamentSessionStatus50KG::ok;
 std::uint32_t sourcePC=0;
 OriginalUiTournamentCareerResourceDiagnostic50KG resource{};
 OriginalUiTournamentWidgetsDiagnostic50KG widget{};
 game::OriginalUiApertureDiagnostic50KG aperture{};
 game::OriginalUiTournamentCameraDiagnostic50KG camera{};
 game::OriginalUiTournamentPodDiagnostic50KG model{};
 OriginalUiTournamentContributionDiagnostic50KG draw{};
 game::OriginalUiLightManagerLifetimeDiagnostic50KG light{};
};
struct OriginalUiTournamentSessionCold50KG {
 std::shared_ptr<OriginalUiTournamentCareerResources50KG> resources;
 std::shared_ptr<OriginalUiTournamentWidgetResources50KG> widgets;
 std::shared_ptr<const MaterialTextureGl50EB> gpu;
 std::shared_ptr<const assets::OriginalUiArchive50KG> tournamentArchive,appStart,podTextures;
 std::shared_ptr<const assets::OriginalUiIpumTextureCache50KG> ipumCache;
 assets::OriginalUiMaterialRuntime50KG materialRuntime{};
 const assets::OriginalUiEnglish50KG* english=nullptr;
 std::shared_ptr<game::OriginalUiLightManagerLifetime50KG> lightManager;
 std::shared_ptr<OriginalUiRiderProfileOwner50KG> profile;
 std::shared_ptr<OriginalUiMainProfile50KG> commonProfile;
 std::shared_ptr<OriginalUiMainApplication50KG> application;
 // Same actual Application clock capability used by the frontend update/render.
 game::OriginalApplicationAnimationClockView50KG applicationAnimation{};
 OriginalUiMainAudioMode50KG audioMode=OriginalUiMainAudioMode50KG::enabled;
 std::shared_ptr<OriginalUiMainAudio50KG> audio;
 std::shared_ptr<game::OriginalUiTransitionOwner50KG> externalTransition;
 // Actual typed predecessor. Course Back never reuses a retired Rider proposal.
 std::variant<std::monostate,OriginalUiRiderRouteProposal50KG,OriginalUiCourseRouteProposal50KG> sourceProposal{};
};
class OriginalUiTournamentSession50KG;
struct OriginalUiTournamentRouteProposal50KG {
 const OriginalUiTournamentSession50KG* source=nullptr;std::uint64_t serial=0;
 game::OriginalUiRouteDecision50KG decision{};OriginalUiRiderProfileStamp50KG profileStamp{};
};
struct OriginalUiTournamentSessionInputReceipt50KG {
 bool containerReached=false,pageCallbackRan=false,pageAccepted=false;
 OriginalUiTournamentRouteProposal50KG routeProposal{};
 OriginalUiTournamentSessionStatus50KG status=OriginalUiTournamentSessionStatus50KG::noChange;
};
struct OriginalUiTournamentSessionSnapshot50KG {
 bool bound=false,faulted=false,quitRequested=false,pageShown=false,inputContainerShown=false,cameraActive=false,framePublished=false,backProposalPending=false;
 std::uint32_t promptMask42C=0,career=0;
 float timer4064=0;
 std::uint64_t updateCount=0,renderCount=0;
 game::OriginalUiTransitionSnapshot50KG transition{};game::OriginalUiApertureSnapshot50KG aperture{};
 OriginalUiMainSessionAudioReceipt50KG audio{};
};
// Clean fresh-career navigation only. Course Continue/Back is bounded to
// fresh slot 1; Garage, race launch, storage and progression stay unadmitted. Destination readiness requires concrete owned rendering, not
// this declaration, numeric plan or the archive registry entry.
class OriginalUiTournamentSession50KG final {
public:
 OriginalUiTournamentSession50KG() noexcept;~OriginalUiTournamentSession50KG();
 OriginalUiTournamentSession50KG(const OriginalUiTournamentSession50KG&)=delete;
 OriginalUiTournamentSession50KG& operator=(const OriginalUiTournamentSession50KG&)=delete;
 OriginalUiTournamentSessionStatus50KG bind(const OriginalUiTournamentSessionCold50KG&,OriginalUiTournamentSessionDiagnostic50KG&) noexcept;
 OriginalUiTournamentSessionStatus50KG notifyTransition(const game::OriginalUiTransitionNotification50KG&,OriginalUiTournamentSessionDiagnostic50KG&) noexcept;
 game::OriginalUiTransitionCompletion50KG pollCompletion(game::OriginalUiTransitionRoute50KG,game::OriginalUiTransitionPhase50KG) const noexcept;
 const game::OriginalUiTransitionOwner50KG* externalLifecycleOwner() const noexcept;
 bool ownsPreparedPage(std::uint8_t) const noexcept;
 OriginalUiTournamentSessionStatus50KG prepareBank(OriginalUiTournamentSessionDiagnostic50KG&) noexcept;
 std::shared_ptr<const assets::OriginalUiImageBank50KG> bank() const noexcept;
 const std::vector<MaterialTextureSampler50EB>* samplers() const noexcept;
 OriginalUiTournamentSessionStatus50KG bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB>,OriginalUiTournamentSessionDiagnostic50KG&) noexcept;
 OriginalUiTournamentSessionStatus50KG input(const OriginalUiMainSessionInput50KG&,OriginalUiTournamentSessionInputReceipt50KG&,OriginalUiTournamentSessionDiagnostic50KG&) noexcept;
 bool ownsProfile(const OriginalUiRiderProfileOwner50KG&) const noexcept;
 bool routeProposalCurrent(const OriginalUiTournamentRouteProposal50KG&) const noexcept;
 // Exact accepted Continue after real renderer, pod and material retirement.
 // The consuming Course entry authenticates and consumes this origin once.
 bool routeProposalRetired(const OriginalUiTournamentRouteProposal50KG&) const noexcept;
 OriginalUiTournamentSessionStatus50KG notifyRouteAccepted(const OriginalUiTournamentRouteProposal50KG&,OriginalUiTournamentSessionDiagnostic50KG&) noexcept;
 OriginalUiTournamentSessionStatus50KG prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG&,OriginalUiTournamentSessionDiagnostic50KG&) noexcept;
 OriginalUiTournamentSessionStatus50KG update(const OriginalUiMainSessionUpdate50KG&,OriginalUiTournamentSessionDiagnostic50KG&) noexcept;
 OriginalUiTournamentSessionStatus50KG render(const OriginalUiMainFrameClock50KG&,OriginalUiTournamentSessionDiagnostic50KG&) noexcept;
 OriginalUiTournamentSessionSnapshot50KG snapshot() const noexcept;
 const game::OriginalUiNavigation50KG* navigation() const noexcept;
 const OriginalUiNativeDraw50KG* draw() const noexcept;
 const OriginalUiSourceFrame50KM* sourceFrame() const noexcept;
 std::shared_ptr<const game::OriginalUiTournamentPredecessorInventory50KG> predecessorInventory(OriginalUiTournamentSessionDiagnostic50KG&) const noexcept;
 void requestQuit() noexcept;
private:
 friend class OriginalUiFrontend50KG;
 bool acceptEntryProfileCommit(const OriginalUiRiderRouteProposal50KG&) noexcept;
 bool acceptEntryProfileCommit(const OriginalUiCourseRouteProposal50KG&) noexcept;
 bool commitRoute(const OriginalUiTournamentRouteProposal50KG&,const game::OriginalUiRouteDecision50KG&) noexcept;
 bool retirePredecessorInventory(std::shared_ptr<const game::OriginalUiTournamentPredecessorInventory50KG>,game::OriginalUiTournamentPredecessorRetirement50KG&,OriginalUiTournamentSessionDiagnostic50KG&) const noexcept;
 bool sourceTeardownPending() const noexcept;
 OriginalUiTournamentSessionStatus50KG teardownSource(VehicleWindowGl50DS*,OriginalUiTournamentSessionDiagnostic50KG&) noexcept;
 struct Storage;std::unique_ptr<Storage> storage_;
};
}
