// RECONSTRUCTED FROM VISIBLE TASK CONTEXT AFTER WORKSPACE ROLLBACK.
// Not byte-verified against lost candidate. Source-only integration review pending.
#pragma once
#include "original_ui_course_draw_contribution50kg.h"
#include "original_ui_rider_profile50kg.h"
#include "../../../src/game/original_ui_light_manager_lifetime50kg.h"
namespace rr::platform {
enum class OriginalUiCourseSessionStatus50KG : std::uint8_t {
    ok,noChange,backProposed,unavailableDestination,notBound,alreadyBound,invalidInput,
    unsupportedState,profile,resource,widget,model,camera,lighting,aperture,
    transition,draw,audio,allocation,faulted,quit,confirmProposed
};
struct OriginalUiCourseSessionDiagnostic50KG {
    OriginalUiCourseSessionStatus50KG status=OriginalUiCourseSessionStatus50KG::ok;
    std::uint32_t sourcePC=0;
    OriginalUiCourseContributionDiagnostic50KG draw{};
    game::OriginalUiCourseWidgetDiagnostic50KG widget{};
    game::OriginalUiCourseHologramSceneDiagnostic50KG model{};
    game::OriginalUiCourseSelectionDiagnostic50KG selection{};
    game::OriginalUiCourseCameraDiagnostic50KG camera{};
    game::OriginalUiCourseRenderContextDiagnostic50KG renderContext{};
    game::OriginalUiCourseLightingDiagnostic50KG lighting{};
    game::OriginalUiLightManagerLifetimeDiagnostic50KG lightManager{};
    game::OriginalUiApertureDiagnostic50KG aperture{};
};
struct OriginalUiCourseSessionCold50KG {
    std::shared_ptr<OriginalUiCourseDrawResources50KG> resources;
    std::shared_ptr<OriginalUiCourseHologramDraw50KG> holograms;
    std::shared_ptr<const assets::OriginalUiArchive50KG> courseArchive;
    const assets::OriginalUiEnglish50KG* english=nullptr;
    std::shared_ptr<OriginalUiRiderProfileOwner50KG> profile;
    std::shared_ptr<OriginalUiMainProfile50KG> commonProfile;
    std::shared_ptr<OriginalUiMainApplication50KG> application;
    std::shared_ptr<game::OriginalUiLightManagerLifetime50KG> lightManager;
    std::shared_ptr<game::OriginalUiTransitionOwner50KG> externalTransition;
    OriginalUiMainAudioMode50KG audioMode=OriginalUiMainAudioMode50KG::enabled;
    std::shared_ptr<OriginalUiMainAudio50KG> audio;
    std::uint8_t previousScreen=0;
};
class OriginalUiCourseSession50KG;
struct OriginalUiCourseRouteProposal50KG {
    const OriginalUiCourseSession50KG* source=nullptr;
    std::uint64_t serial=0;
    game::OriginalUiRouteDecision50KG decision{};
    OriginalUiRiderProfileStamp50KG profileStamp{};
};
struct OriginalUiCourseSessionInputReceipt50KG {
    bool containerReached=false,pageCallbackRan=false,pageAccepted=false,selectionChanged=false;
    game::OriginalUiRouteDecision50KG decision{};
    OriginalUiCourseRouteProposal50KG routeProposal{};
    OriginalUiCourseSessionStatus50KG status=OriginalUiCourseSessionStatus50KG::noChange;
};
struct OriginalUiCourseSessionSnapshot50KG {
    bool bound=false,faulted=false,quitRequested=false,pageShown=false;
    bool inputContainerShown=false,cameraActive=false,framePublished=false;
    bool backProposalPending=false,backAccepted=false,launchAccepted=false;
    bool popupEnabled438=false;
    std::uint32_t courseSlot14E4=0,subtypeSlot14F4=0,laps14EC=0,entrants14F0=0;
    std::uint32_t promptMask42C=0,selectedPlanet40=0,selectedCourse44=0,deferred14D0=0;
    std::uint64_t updates=0,renders=0,registryGeneration=0;
    game::OriginalUiTransitionSnapshot50KG transition{};
    game::OriginalUiApertureSnapshot50KG aperture{};
    OriginalUiMainSessionAudioReceipt50KG audio{};
};
// Bounded mode0/subtype1/laps3/entrants8, fresh unlocked Course slots1..4.
// Left/right uses genuine authored controls/models/text/backgrounds. Original
// Back creates a fresh Rider owner; Confirm proposes only the source launch transaction.
// The frontend independently admits its actual cold native race destination.
class OriginalUiCourseSession50KG final {
public:
    OriginalUiCourseSession50KG()noexcept;~OriginalUiCourseSession50KG();
    OriginalUiCourseSession50KG(const OriginalUiCourseSession50KG&)=delete;
    OriginalUiCourseSession50KG& operator=(const OriginalUiCourseSession50KG&)=delete;
    OriginalUiCourseSessionStatus50KG bind(const OriginalUiCourseSessionCold50KG&,OriginalUiCourseSessionDiagnostic50KG&)noexcept;
    bool ownsPreparedPage(std::uint8_t)const noexcept;
    bool canEnterMode(std::uint32_t)const noexcept;
    std::uint32_t intendedMode()const noexcept{return 0;}
    const game::OriginalUiTransitionOwner50KG* externalLifecycleOwner()const noexcept;
    OriginalUiCourseSessionStatus50KG notifyTransition(const game::OriginalUiTransitionNotification50KG&,OriginalUiCourseSessionDiagnostic50KG&)noexcept;
    game::OriginalUiTransitionCompletion50KG pollCompletion(game::OriginalUiTransitionRoute50KG,game::OriginalUiTransitionPhase50KG)const noexcept;
    OriginalUiCourseSessionStatus50KG input(const OriginalUiMainSessionInput50KG&,OriginalUiCourseSessionDiagnostic50KG&)noexcept;
    OriginalUiCourseSessionStatus50KG input(const OriginalUiMainSessionInput50KG&,OriginalUiCourseSessionInputReceipt50KG&,OriginalUiCourseSessionDiagnostic50KG&)noexcept;
    bool routeProposalCurrent(const OriginalUiCourseRouteProposal50KG&)const noexcept;
    bool routeContext(const OriginalUiCourseRouteProposal50KG&,game::OriginalUiCourseContext50KG&)const noexcept;
    OriginalUiCourseSessionStatus50KG notifyRouteAccepted(const OriginalUiCourseRouteProposal50KG&,OriginalUiCourseSessionDiagnostic50KG&)noexcept;
    OriginalUiCourseSessionStatus50KG prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG&,OriginalUiCourseSessionDiagnostic50KG&)noexcept;
    OriginalUiCourseSessionStatus50KG update(const OriginalUiMainSessionUpdate50KG&,OriginalUiCourseSessionDiagnostic50KG&)noexcept;
    OriginalUiCourseSessionStatus50KG render(const OriginalUiCourseDrawClock50KG&,OriginalUiCourseSessionDiagnostic50KG&)noexcept;
    OriginalUiCourseSessionStatus50KG prepareBank(OriginalUiCourseSessionDiagnostic50KG&)noexcept;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank()const noexcept;
    const std::vector<MaterialTextureSampler50EB>* samplers()const noexcept;
    OriginalUiCourseSessionStatus50KG bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB>,OriginalUiCourseSessionDiagnostic50KG&)noexcept;
    const OriginalUiNativeDraw50KG* draw()const noexcept;
    OriginalUiCourseSessionSnapshot50KG snapshot()const noexcept;
    void drain()noexcept;void requestQuit()noexcept;
private:
    friend class OriginalUiFrontend50KG;
    // Called only by the frontend after actual finishSubmittedWork and exact
    // immutable-bank rebind. No public callback can mint completion.
    OriginalUiCourseSessionStatus50KG completeSelectionRefresh(OriginalUiCourseSessionDiagnostic50KG&)noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
