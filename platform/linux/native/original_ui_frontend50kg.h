// Source-reviewed bounded Course/Tournament integration; runtime acceptance is pending.
#pragma once
#include "original_ui_options_session50kg.h"
#include "original_ui_hall_session50kg.h"
#include "../../../src/game/control_presets49d.h"
#include "original_ui_rider_session50kg.h"
#include "original_ui_course_session50kg.h"
#include "original_ui_tournament_session50kg.h"
#include "original_ui_garage_destination50kg.h"
#include "../../../src/assets/original_standard_material_table50la.h"
#include "vehicle_window_gl50ds.h"
#include "../../../src/assets/original_ui_localization50kg.h"
#include "../../../src/game/original_ui_light_manager_lifetime50kg.h"
#include "../../../src/game/original_course_load_selection50kg.h"
#include <chrono>
#include <memory>

namespace original_main_entry50KG { struct Application; struct Frontend; class RaceDisplayLifetime50LA; }
namespace rr::platform {
class OriginalUiMemoryCardConstructor50KG;
class OriginalSaveNativeReadStage50KG;
class OriginalSaveProfileOwner50KG;
class OriginalUiGarageSession50KG;
// Concrete native host gate admits either its already-prepared same-world
// destination or a supported cold-load request whose authentic source archive
// is retained. A queued request is not a loaded-world/completion receipt: the
// host must retire the old session and execute the target's real cold loader
// before any race activation. Archive names or caller-set ready flags alone
// cannot admit either path; actual source and lifecycle owners are required.
class OriginalUiCourseRaceDestination50KG {
public:
    virtual ~OriginalUiCourseRaceDestination50KG()=default;
    virtual bool admits(const OriginalUiRiderProfileOwner50KG::CourseRaceSelection&)const noexcept=0;
    virtual game::OriginalCourseLoadAvailability50KG availability(const OriginalUiRiderProfileOwner50KG::CourseRaceSelection& s)const noexcept {
        return admits(s)?game::OriginalCourseLoadAvailability50KG::ready:game::OriginalCourseLoadAvailability50KG::hostNotReady;
    }
};
enum class OriginalUiFrontendStatus50KG : std::uint8_t {
    ok,noChange,unavailableDestination,secondControllerRequired,routeAccepted,
    notPrepared,alreadyPrepared,notActive,busy,invalidInput,resource,localization,
    gpu,session,transition,renderer,cleanup,allocation,faulted,quit,courseLaunchReady
};
struct OriginalUiFrontendDiagnostic50KG {
    OriginalUiFrontendStatus50KG status=OriginalUiFrontendStatus50KG::ok;
    OriginalUiFrontendStatus50KG preparationStatus=OriginalUiFrontendStatus50KG::ok;
    std::uint32_t sourcePC=0;
    game::OriginalCourseLoadAvailability50KG courseLoad=game::OriginalCourseLoadAvailability50KG::hostNotReady;
    OriginalUiMainSessionDiagnostic50KG main{};
    OriginalUiOptionsSessionDiagnostic50KG options{};
    OriginalUiHallSessionDiagnostic50KG hall{};
    OriginalUiHallDrawDiagnostic50KG hallResources{};
    OriginalUiOptionsDrawDiagnostic50KG optionsResources{};
    OriginalUiRiderSessionDiagnostic50KG rider{};
    OriginalUiRiderDrawResourceDiagnostic50KG riderResources{};
    OriginalUiTournamentSessionDiagnostic50KG tournament{};
    OriginalUiGarageDestinationDiagnostic50KG garage{};
    assets::OriginalStandardMaterialDiagnostic50LA garageRuntime{};
    std::string garageDetail;
    OriginalUiTournamentCareerResourceDiagnostic50KG tournamentResources{};
    OriginalUiTournamentWidgetResourceDiagnostic50KG tournamentWidgets{};
    OriginalUiCourseSessionDiagnostic50KG course{};
    OriginalUiCourseDrawResourceDiagnostic50KG courseResources{};
    OriginalUiCourseHologramDrawDiagnostic50KG courseHolograms{};
    assets::OriginalUiRiderStatAssetDiagnostic50KG riderStats{};
    assets::OriginalUiRiderBinderAssetDiagnostic50KG riderBinder{};
    assets::OriginalUiLocalizationDiagnostic50KG localization{};
    game::OriginalUiLightManagerLifetimeDiagnostic50KG lightManager{};
    MaterialTextureGlDiagnostic50EB gpu{};
    VehicleWindowDiagnostic50DS renderer{};
    game::OriginalUiTransitionStatus50KG transition=game::OriginalUiTransitionStatus50KG::ok;
    // Race-return (1AB5BC receiver) and screen 7/10 sessions: reached source
    // PC and a static reason for the first refusal.
    std::uint32_t raceReturnSourcePC=0;
    const char* raceReturnDetail=nullptr;
};
// Native application presentation/audio owner. The window implementation uses
// verified driver swap-interval1, or an explicitly requested native PC deadline
// policy with truthful interval0. Neither policy may tick pages/RNG or invent
// timestamps, rendered-image completion, or physical scanout receipts.
class OriginalUiRiderPresentationHost50KG {
public:
    virtual ~OriginalUiRiderPresentationHost50KG()=default;
    virtual bool available()const noexcept=0;
    virtual bool begin()noexcept=0;
    virtual bool end()noexcept=0;
    // Called after the actual source sample-begin timestamp, before rendering.
    // Pacing only: a successful wait is not a render or presentation receipt.
    // Final whole-incoming promotion does not call this source-sample hook.
    virtual bool waitBeforeSample()noexcept=0;
    virtual bool present(VehicleWindowGl50DS&,const OriginalUiPrivatePublication50KP&,
        VehicleWindowDiagnostic50DS&)noexcept=0;
    virtual bool checkpointAudio()noexcept=0;
    virtual bool materialClock(OriginalUiMainFrameClock50KG&)noexcept=0;
};
struct OriginalUiFrontendCold50KG {
    // Already-authenticated immutable original archives, retained by this owner.
    // English is decoded once from localization and retained for every lifetime.
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,mainMenu,options,
        fonts,controllerPopup,overlays,localization,hall;
    // Optional concrete Rider inputs. Missing any leaves Main's genuine Rider
    // action unavailable; a cache flag or remembered rider is not readiness.
    std::shared_ptr<const assets::OriginalUiArchive50KG> rider,backgrounds,podTextures,course,tournament;
    // Optional original UIUNLOCK/UIWIN archives (screens 10 and 7), reached
    // only through a race acknowledgement 1AB5BC request or their successors.
    std::shared_ptr<const assets::OriginalUiArchive50KG> unlock,win;
    std::shared_ptr<const assets::OriginalUiIpumTextureCache50KG> riderIpumCache;
    std::shared_ptr<OriginalUiRiderProfileOwner50KG> riderProfile;
    std::shared_ptr<const OriginalUiCourseRaceDestination50KG> courseRaceDestination;
    OriginalUiMainVideoOwner50KG video{};
    assets::OriginalUiMaterialRuntime50KG runtime{};
    std::shared_ptr<OriginalUiMainProfile50KG> profile;
    std::shared_ptr<OriginalUiOptionsProfile50KG> optionsProfile;
    // One actual application owner, shared with the native gameplay/camera
    // binding. Page entry and Cancel never reset or replace these presets.
    std::shared_ptr<game::ControlPresets49D> controlsProfile;
    // Retained sole application profile for page4. Optional: without it only
    // Options menu1 is admitted. No reset or imported-save claim is made here.
    std::shared_ptr<game::OriginalUiGameSettingsProfile50KG> gameSettingsProfile;
    // The application's actual retained mode owner; Accept also proves that
    // the same pointer is attached to audio's current native mixer.
    std::shared_ptr<OriginalSoundMode50KG> soundMode;
    std::shared_ptr<OriginalUiMainApplication50KG> application;
    game::OriginalApplicationAnimationClockView50KG applicationAnimation{};
    // Constructed once by the native APPLICATION before initial Main, retained
    // across every screen/frontend lifetime. Never created by a destination.
    std::shared_ptr<game::OriginalUiLightManagerLifetime50KG> lightManager;
    OriginalUiMainAudioMode50KG audioMode=OriginalUiMainAudioMode50KG::enabled;
    // Same actual transport/output owner throughout Main/Options. Caller owns
    // mixer/device and advances PCM only for an accepted update receipt.
    std::shared_ptr<OriginalUiMainAudio50KG> audio;
    // Exact live context/functions and explicit per-bank GPU budget. Samplers
    // must be empty here: every fresh bank supplies its authenticated policies.
    // video.lifetime must retain the actual context/callback storage.
    MaterialTextureGlRequest50EB textureRequest{};
    // Borrowed stable application owner, outliving this frontend. Absence keeps
    // Left/Right unavailable before any model, image or profile mutation.
    OriginalUiRiderPresentationHost50KG* riderPresentation=nullptr;
    // Explicit application request, copied across manager rebuilds. False keeps
    // existing populations; true requires both authenticated Clegg cache members.
    bool prepareCleggLockedPreview=false;
    // Additive bit N = original rider N. Retained across Main/Course transitions
    // and manager rebuilds; resources only, never profile or Confirm admission.
    // Zero preserves the existing population and legacy Clegg request.
    std::uint32_t requestedPreviewRiders=0;
};
struct OriginalUiFrontendColdInterval50KG {
    // Host loading interval only; no source clock is sampled or advanced here.
    // Exclude this exact interval from the next elapsed source update and
    // material clock accumulation, including a rejected cold preparation.
    bool measured=false;
    std::chrono::steady_clock::time_point begin{},end{};
    std::uint64_t nanoseconds=0;
};
// Observation only: this interval remains active work. In particular, an
// outgoing GPU drain never supplies cold-time exclusion authority.
struct OriginalUiFrontendWorkObservation50KG {
    bool measured=false;
    std::chrono::steady_clock::time_point begin{},end{};
    std::uint64_t nanoseconds=0;
};
struct OriginalUiFrontendInputReceipt50KG {
    OriginalUiFrontendStatus50KG status=OriginalUiFrontendStatus50KG::noChange;
    game::OriginalUiTransitionRoute50KG source{},requested{};
    bool routeAccepted=false;
    bool garageInputReached=false,garageInputCompleted=false,garageRequestWritten=false;
    std::uint64_t garageInputSerial=0;std::uint32_t garageInputSourcePC=0;
    OriginalUiFrontendColdInterval50KG coldInterval{};
    OriginalUiMainSessionInputReceipt50KG main{};
    OriginalUiOptionsSessionInputReceipt50KG options{};
    OriginalUiHallSessionInputReceipt50KG hall{};
    OriginalUiRiderSessionInputReceipt50KG rider{};
    OriginalUiCourseSessionInputReceipt50KG course{};
    OriginalUiTournamentSessionInputReceipt50KG tournament{};
};
struct OriginalUiFrontendUpdateReceipt50KG {
    // True only after exactly one audio preparation, actual manager switch
    // case, and complete source child update. Caller advances its PCM output
    // once for this sequence using its corresponding accepted nanoseconds.
    bool accepted=false;
    // Observation of the actual Root transaction. A Garage update has already
    // committed SourceTiming; the host must not commit the generic menu tick.
    bool sourceTimingCommitted=false;
    std::uint64_t sourcePublication=0;
    std::uint64_t sequence=0;
    OriginalUiMainSessionUpdate50KG update{};
    game::OriginalUiTransitionRoute50KG source{};
    // Only the resident CPU/GPU bank validation/publication call is cold.
    // It occurs AFTER the host's accepted elapsed sample and belongs to the
    // next update/material intervals, never this already accepted update.
    OriginalUiFrontendColdInterval50KG coldInterval{};
    OriginalUiFrontendWorkObservation50KG bankDrain{};
};
struct OriginalUiFrontendDeferredReceipt50KG {
    OriginalUiFrontendColdInterval50KG coldInterval{};
    OriginalUiFrontendWorkObservation50KG bankDrain{};
};
struct OriginalUiFrontendSnapshot50KG {
    bool prepared=false,active=false,faulted=false,quitRequested=false;
    bool deferredQueue=false,frameSubmitted=false,courseLaunchReady=false;
    game::OriginalUiTransitionRoute50KG route{},preparedDestination{};
    game::OriginalUiTransitionSnapshot50KG transition{};
    game::OriginalUiApplicationRequest50KG queuedRequest{};
    OriginalUiMainSessionSnapshot50KG main{};
    OriginalUiOptionsSessionSnapshot50KG options{};
    OriginalUiHallSessionSnapshot50KG hall{};
    OriginalUiRiderSessionSnapshot50KG rider{};
    OriginalUiCourseSessionSnapshot50KG course{};
    OriginalUiTournamentSessionSnapshot50KG tournament{};
    std::uint64_t updates=0,renders=0,submissions=0,switches=0;
    game::OriginalUiLightCacheState50KG lightCacheState=game::OriginalUiLightCacheState50KG::notConstructed;
    std::uint64_t lightManagerGeneration=0;
    bool lightManagerEntered=false;
    bool garageLoaded=false,garageRootReturned=false,garageTimingCommitted=false,garageTargetPublished=false;
    std::uint64_t garagePublication=0;
    std::uint32_t garageManagerLifecycle=0,garageManagerTransition=0,garageSelectedStat=0;
    game::OriginalUiApertureSnapshot50KG garageAperture{};
};
// Concrete native Main4/1 <-> Options12/{1,2,3,4}, plus Rider5/1 for
// authenticated Rider selection: mode0 Confirm -> bounded Course6/1; mode2
// Confirm -> clean fresh-career Tournament9/1 and Back. Tournament Course/Garage
// use retained concrete destination owners. Course Confirm additionally requires the concrete native race lease.
// No guest execution, fallback UI or profile reset. Runtime acceptance is pending.
// One freshly prepared bank/session/manager per screen lifetime; shared process
// profile/application/audio/light manager remain the same. Options page changes
// retain that screen's one bank/session/manager. All calls serialized/non-reentrant.
class OriginalUiFrontend50KG final {
public:
    OriginalUiFrontend50KG() noexcept;
    // Explicit close while the retained video context is current is mandatory,
    // including after partial prepare/activation failures that own GPU names.
    ~OriginalUiFrontend50KG() noexcept;
    OriginalUiFrontend50KG(const OriginalUiFrontend50KG&)=delete;
    OriginalUiFrontend50KG& operator=(const OriginalUiFrontend50KG&)=delete;
    OriginalUiFrontendStatus50KG prepare(const OriginalUiFrontendCold50KG&,
        OriginalUiFrontendDiagnostic50KG&) noexcept;
    // Before renderer.create. Only sets original-UI fields; verifies the same
    // exact native context/currentness callbacks as the uploaded bank.
    OriginalUiFrontendStatus50KG configureRendererRequest(VehicleWindowRequest50DS&,
        OriginalUiFrontendDiagnostic50KG&) noexcept;
    // Renderer is borrowed until close; it must outlive this coordinator.
    // Initial notifications happen here, after the actual renderer accepts bank.
    OriginalUiFrontendStatus50KG activate(VehicleWindowGl50DS&,
        OriginalUiFrontendDiagnostic50KG&) noexcept;
    OriginalUiFrontendStatus50KG input(const OriginalUiMainSessionInput50KG&,
        OriginalUiFrontendInputReceipt50KG&,OriginalUiFrontendDiagnostic50KG&) noexcept;
    OriginalUiFrontendStatus50KG update(const OriginalUiMainSessionUpdate50KG&,
        OriginalUiFrontendUpdateReceipt50KG&,OriginalUiFrontendDiagnostic50KG&) noexcept;
    OriginalUiFrontendStatus50KG render(const OriginalUiMainFrameClock50KG&,
        OriginalUiFrontendDiagnostic50KG&) noexcept;
    OriginalUiFrontendStatus50KG submit(VehicleWindowGl50DS&,std::int32_t width,
        std::int32_t height,OriginalUiFrontendDiagnostic50KG&) noexcept;
    // Call after submit plus any swap/capture/readback consumers have returned.
    // Queued source's child/render work must be submitted first. Completes GPU
    // work, replaces bank, activates a fresh manager, then retires old owners.
    // Never invoked inside a manager callback or before the source frame ends.
    OriginalUiFrontendStatus50KG commitDeferred(VehicleWindowGl50DS&,
        OriginalUiFrontendDiagnostic50KG&) noexcept;
    // Additive receipt overload. No-change/rejected preconditions issue no
    // cold receipt; attempted bank publication retains its exact interval.
    OriginalUiFrontendStatus50KG commitDeferred(VehicleWindowGl50DS&,
        OriginalUiFrontendDeferredReceipt50KG&,OriginalUiFrontendDiagnostic50KG&) noexcept;
    // Retires source-frame owners through the live borrowed renderer, closes
    // that renderer, then releases sessions and GPU owners. May be retried only
    // for unresolved cleanup; no source work can resume after it.
    OriginalUiFrontendStatus50KG close(OriginalUiFrontendDiagnostic50KG&) noexcept;
    // Includes a retained borrow after failed activation or partial close. The
    // host must use close above before independently closing this renderer.
    // Read once after the actual source queue9 frame has completed on GPU.
    // Retains every UI/GPU bank through close; no native world is launched here.
    bool takeCourseLaunch(OriginalUiRiderProfileOwner50KG::CourseRaceSelection&)noexcept;
    bool borrowsRenderer(const VehicleWindowGl50DS&) const noexcept;
    // Native custody observation only, including a retained failed loader.
    bool ownsGarageCustody() const noexcept;
    OriginalUiFrontendStatus50KG retireGarageBeforeClose(OriginalUiFrontendDiagnostic50KG&) noexcept;
    void requestQuit() noexcept;
    OriginalUiFrontendSnapshot50KG snapshot() const noexcept;
    // Same values as snapshot() except that the garage* fields stay default:
    // the Garage runtime observation is skipped. For host callers that never
    // read a garage* field. route() is snapshot().route alone.
    // RR_GARAGE_PROOF_SHADOW=1 compares both against snapshot().
    OriginalUiFrontendSnapshot50KG snapshotWithoutGarageRuntime() const noexcept;
    game::OriginalUiTransitionRoute50KG route() const noexcept;
    const OriginalUiMainSession50KG* mainSession() const noexcept;
    const OriginalUiOptionsSession50KG* optionsSession() const noexcept;
    const OriginalUiRiderSession50KG* riderSession() const noexcept;
    const OriginalUiCourseSession50KG* courseSession() const noexcept;
    const OriginalUiTournamentSession50KG* tournamentSession() const noexcept;
    const OriginalUiHallOfFameSession50KG* hallSession() const noexcept;
    const OriginalUiCourseDrawResources50KG* courseResources() const noexcept;
    const OriginalUiMainDrawResources50KG* mainResources() const noexcept;
    const OriginalUiOptionsDrawResources50KG* optionsResources() const noexcept;
    const assets::MaterialTextureAssets50EA* textures() const noexcept;
    const MaterialTextureGl50EB* gpu() const noexcept;
    const OriginalUiNativeDraw50KG* draw() const noexcept;
private:
    friend struct ::original_main_entry50KG::Frontend;
    friend struct ::original_main_entry50KG::Application;
    // Read-only admission of the actual taken Course request. This does not
    // certify dispatcher destruction, source retirement or loading readiness.
    bool observeCourseRaHandoff50LA(const ::original_main_entry50KG::Application&,
        const ::original_main_entry50KG::Frontend&,
        OriginalUiRiderProfileOwner50KG::CourseRaceSelection&,
        std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA>&)const noexcept;
    // Uncalled native reverse-route receiver. Future exact Results flags6
    // transfer must establish origin/whole-poll completion/retirement first.
    // No Main construction history or source1B7BD0 receipt is synthesized.
    OriginalUiFrontendStatus50KG prepareNativeResultsHall(
        const OriginalUiFrontendCold50KG&,const ::original_main_entry50KG::Application&,
        OriginalUiFrontendDiagnostic50KG&)noexcept;
    // Only the genuine retained Pause row10 request can issue this origin.
    // It opens Course directly without a Main/Rider entry or profile replay.
    OriginalUiFrontendStatus50KG prepareNativePauseCourse(
        const OriginalUiFrontendCold50KG&,const std::shared_ptr<OriginalUiPauseCourseOrigin50LA>&,
        OriginalUiFrontendDiagnostic50KG&)noexcept;
    // 1AB5BC -> 17DE10(App,6,screen,1) receiver: the retired race's request
    // opens screen/page as the FIRST screen of this lifetime (17DB80 with no
    // UI manager builds one through ld_ui). The profile mode is retained.
    OriginalUiFrontendStatus50KG prepareNativeRaceReturn(
        const OriginalUiFrontendCold50KG&,const ::original_main_entry50KG::Application&,
        std::uint8_t screen,std::uint8_t page,OriginalUiFrontendDiagnostic50KG&)noexcept;
    OriginalUiFrontendStatus50KG prepareInitialNative(
        const OriginalUiFrontendCold50KG&,const ::original_main_entry50KG::Application*,
        const std::shared_ptr<OriginalUiPauseCourseOrigin50LA>&,
        OriginalUiFrontendDiagnostic50KG&)noexcept;
    friend class OriginalUiGarageSession50KG;
    bool queueGarageScreen17DE10(const OriginalUiGarageSession50KG&,std::int32_t,std::int32_t)noexcept;
    friend class OriginalUiGarageDrawContribution50KG;
    bool prepareGarageSourceImages12CEA0(const OriginalUiGarageSession50KG&,
        const std::shared_ptr<const assets::OriginalUiArchive50KG>&,
        std::shared_ptr<const assets::OriginalUiIpumTextureCache50KG>&,
        assets::OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool borrowGarageTextureServices50KG(OriginalUiGarageDrawContribution50KG&,
        MaterialTextureGlRequest50EB&,std::shared_ptr<const assets::OriginalUiArchive50KG>&,
        std::shared_ptr<const assets::OriginalUiIpumTextureCache50KG>&,VehicleWindowDiagnostic50DS&)noexcept;
    bool bindGarageTextureBank50KG(OriginalUiGarageDrawContribution50KG&,
        const assets::MaterialTextureAssets50EA&,const MaterialTextureGl50EB&,VehicleWindowDiagnostic50DS&)noexcept;
    bool releaseGarageTextureServices50KG(OriginalUiGarageDrawContribution50KG&,VehicleWindowDiagnostic50DS&)noexcept;
    bool canRearmGarageTextureServices50KG(const OriginalUiGarageDrawContribution50KG&)const noexcept;
    bool renewGarageTexturePublication140230(OriginalUiGarageDrawContribution50KG&,std::uint64_t)noexcept;

    // Private existing-owner preflight, uncalled. Output is private metadata,
    // NOT all-semantic readiness, a pending transaction or permission to load.
    // Uncalled private native install. Does not read a provider or admit a menu
    // action/card status. All failures precede the first live profile store.
    bool installNativeReadProfile50KG(const std::shared_ptr<const OriginalSaveNativeReadStage50KG>&)noexcept;
    bool prepareNativeReadProfileMetadata50KG(
        const std::shared_ptr<const OriginalSaveNativeReadStage50KG>&,
        std::unique_ptr<OriginalSaveProfileOwner50KG>& out)const noexcept;
    // Bind the actual enclosing native owner, never a callback/readiness flag.
    // Private source-custody handoff only; uncalled and not page438 admission.
    // Private counted base-constructor prefix in the existing Scope slot;
    // explicit native abort, not source completion or dispatcher publication.
    bool beginOptionsManagerPrefix50KG(const ::original_main_entry50KG::Application&)noexcept;
    bool abortOptionsManagerPrefix50KG()noexcept;
    bool writeOptionsVideo44Prefix50KG(const ::original_main_entry50KG::Application&)noexcept;
    bool adoptOptionsCameraPrefix50KG(const ::original_main_entry50KG::Application&)noexcept;
    bool stageOptionsCameraViewportPrefix50KG(const ::original_main_entry50KG::Application&)noexcept;
    bool continueOptionsViewportDrains50KG(const ::original_main_entry50KG::Application&)noexcept;
    bool continueOptionsCameraSuffix2361C0(const ::original_main_entry50KG::Application&)noexcept;
    // Non-counted input preparation; cannot mount/publish a source owner.
    bool prepareOptionsNativeInputs50KG(const ::original_main_entry50KG::Application&)noexcept;
    bool prepareOptionsOverlayTextStorage50KG(const ::original_main_entry50KG::Application&)noexcept;
    bool retainMemoryCardOptionsSources50KG(const ::original_main_entry50KG::Application&,
        OriginalUiMemoryCardConstructor50KG&)noexcept;
    bool bindGarageDestinationRoot50KG(::original_main_entry50KG::Frontend&)noexcept;
    //Exclusive same-Frontend request custody across one actual renderer reset.
    //The private caller supplies no readiness flag or replacement context.
    bool frontendTransferRequestCurrent50KG(
        const std::shared_ptr<const assets::OriginalNativeTextureUploadBinding50LA>&)const noexcept;
    bool beginFrontendTransferReset50KG(
        std::shared_ptr<const assets::OriginalNativeTextureUploadBinding50LA>&,
        OriginalUiFrontendDiagnostic50KG&)noexcept;
    bool finishFrontendTransferReset50KG(
        const std::shared_ptr<const assets::OriginalNativeTextureUploadBinding50LA>&,
        std::shared_ptr<const assets::OriginalNativeTextureUploadBinding50LA>,
        OriginalUiFrontendDiagnostic50KG&)noexcept;
    bool prepareSharedDialogsInitial(const ::original_main_entry50KG::Application&,
        VehicleWindowGl50DS&,OriginalUiFrontendDiagnostic50KG&)noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
