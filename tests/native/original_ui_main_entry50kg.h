#include "../../src/game/original_player_overlay_renderer_lease50la.h"
#include "../../src/game/original_application_input04_source50la.h"
#include "../../src/game/original_application_startup17c910_source50la.h"
#include "../../src/game/original_application_video_source50la.h"
#include "original_native_video_packet_invocation50la.h"
#include "../../src/game/original_audio_directory_source50la.h"
#pragma once
#include "../../src/game/original_player_race_music_borrow50la.h"
#include "../../platform/linux/native/native_beta_settings50kg.h"
#include "../../src/game/original_camera_start_effects.h"
#include "../../platform/linux/native/original_application_aural50kg.h"
namespace rr::platform {class OriginalUiCoursePopupPayload50KG;class OriginalUiCoursePagePayload50KG;}
namespace rr::platform { class OriginalUiPauseCourseOrigin50LA; class OriginalUiCourseSession50KG; }
namespace original_main_entry50KG { class OriginalLoadingCollector50LA; }
namespace original_pause_observation50LA { class Owner; }
namespace original_race_hud_scope50LA { class Registration; }
namespace live_audio50HF { class Owner; }
namespace rr::game { class OriginalLoadingScreen40Mode0LA; class OriginalSessionF8Intro50LA;class OriginalLoadingManager33050LA;}
namespace original_results_roster50LA { class Read; }
namespace interactive_publication50DU { struct Context; }
namespace ra_cold_roster50LA { class Operation; }
namespace ra_native_session50LA { class Join; }
namespace rr::game { class OriginalResults10450LA; struct InputEvent48Z; struct OriginalResults104Diagnostic50LA; struct OriginalRaceF8Diagnostic50LA; struct OriginalRaceEndEntryDiagnostic50LA; struct OriginalHumanEndClockResult50KG; }
namespace interactive_continuation50DU {struct Context;}
namespace original_course_targets50KX { class ConsumedLaunch; }
namespace ra_cold_aggregate50LA { class Owner; }
namespace ta_original_world50LE { class Owner; }
namespace gc_original_world50LM { class Owner; }
namespace ga_original_world50LH { class Owner; }
namespace ma_original_world50LM { class Owner; }
namespace sa_original_world50LM { class Owner; }
namespace ra_cold_sources50LA { class ColdNavigationLease; }
namespace rr::game { class OriginalRacerModelRenderGlobals50LA; class OriginalRaceConstructor50LA; class OriginalRaceTuning50LA; class OriginalTunnelTrack50LA; enum class OriginalTunnelTrackRole50LA : unsigned; class OriginalPauseStorage50LA; class OriginalPositionStatsPrimitiveOwner50LA; class OriginalPositionStatsResources50LA;
struct OriginalPositionStatsNumbers50LA; struct OriginalPositionStatsResourceDiagnostic50LA; }
namespace original_camera_session50KX { class Session; struct ActualOwners; struct AuthoredStorage; struct Result; }
namespace rr::assets {class OriginalUiGarageSceneAssets50KG; struct OriginalUiGaragePageData50KG;}
namespace rr::platform {class OriginalUiMemoryCardConstructor50KG; class OriginalUiSharedDialogs50KG; class OriginalUiGarageSharedPageDrawOwner50KG; class OriginalUiGarageSession50KG; struct OriginalUiGarageNativeInput50KG; struct OriginalUiGarageNativeState50KG; class OriginalUiGarageModelResources50KG; class OriginalUiGarageVisibility50KG; class OriginalUiGarageTextOverlay50KG; class OriginalUiGarageFogColorLease50KG; class OriginalUiGarageScreenText50KG; struct OriginalUiGarageScreenTextDiagnostic50KG; class OriginalUiGarageWidget1Owner50KG; struct OriginalUiGarageWidget1Diagnostic50KG; class OriginalUiGarageManagerFog50KG;}
namespace rr::platform { class OriginalUiMainDrawResources50KG; }
namespace rr::platform { class OriginalVideoBlockingNativeBackend; class OriginalSpectatorMenuInput50LA; }
namespace native_ai50IV { struct Session; }
#include "original_ui_main_output50kg.h"
#include "../../platform/linux/native/original_ui_garage_audio_backing50kg.h"
#include "original_ui_elapsed50lh.h"
#include "original_ui_source_timing50lh.h"
#include <optional>
#include "original_ui_rider_pacing50lb.h"
#include "../../platform/linux/native/original_ui_frontend50kg.h"
#include "../../src/game/original_frontend_camera_cold50la.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include "../../platform/linux/native/original_ui_tournament_pod_references50kg.h"
#include "../../platform/linux/native/original_ui_search_list_cold50kg.h"
#include "../../platform/linux/native/original_ui_light_manager_owner50kg.h"
#include "../../platform/linux/native/game_host50er.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../src/assets/original_ui_localization50kg.h"
#include "../../src/assets/original_ui_material_application50la.h"
#include "../../src/assets/original_standard_material_context50la.h"
#include "../../src/assets/original_native_texture_upload50la.h"
#include "../../src/assets/original_standard_material_domain50la.h"
#include "../../src/assets/original_application_fonts50la.h"
#include "../../src/assets/original_application_localization50kg.h"
#include "../../src/game/original_renderer_service_state50kg.h"
#include "../../src/game/original_video_preflip50kg.h"
#include "../../src/game/original_ui_entity_scope50kg.h"
#include "../../platform/linux/native/original_localization_renderer_lease50kg.h"
#include "../../src/game/lap_host_clock50id.h"
#include "../../src/game/original_application_animation_clock50kg.h"
#include "../../src/game/original_application_entities50la.h"
#include "../../platform/linux/native/original_audio_pending_waves50kg.h"
#include "../../src/game/original_motion_library50la.h"
#include "../../src/game/original_time_stats50la.h"
#include "../../src/game/original_speed_boost50la.h"
#include "../../src/game/original_damage50la.h"
#include "../../src/game/original_ui_light_manager_lifetime50kg.h"
#include <array>
#include <filesystem>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <thread>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

namespace rr::assets {
class OriginalPauseSourceData50LA;
class OriginalArchiveStagedReader50LA;
class OriginalStandardTextureTransferCold50LA;
struct OriginalStandardTextureTransferColdDiagnostic50LA;
}
namespace original_pause_cold50LA { class Owner; }
namespace rr::platform { class OriginalUiGarageManagerSearch50KG; }
namespace original_main_entry50KG {
// Concrete host lease: the predicate captures stable native owner references,
// performs read-only exact identity/state checks. An authenticated cold-load
// plan admits only queueing; it does not fabricate a loaded world or activate
// physics from a UI callback. The containing session outlives this lease.
template<class Predicate> class CourseRaceDestination final:public rr::platform::OriginalUiCourseRaceDestination50KG {
    Predicate predicate_;
public:
    explicit CourseRaceDestination(Predicate predicate):predicate_(std::move(predicate)){}
    bool admits(const rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection& selection)const noexcept override {
        try{return predicate_(selection);}catch(...){return false;}
    }
};
template<class Predicate> auto courseRaceDestination(Predicate predicate){
    return std::make_shared<CourseRaceDestination<Predicate>>(std::move(predicate));
}
template<class Predicate,class Availability> class DiagnosedCourseRaceDestination final:public rr::platform::OriginalUiCourseRaceDestination50KG {
    Predicate predicate_;Availability availability_;
public:
    DiagnosedCourseRaceDestination(Predicate p,Availability a):predicate_(std::move(p)),availability_(std::move(a)){}
    bool admits(const rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection& s)const noexcept override {
        return availability(s)==rr::game::OriginalCourseLoadAvailability50KG::ready;
    }
    rr::game::OriginalCourseLoadAvailability50KG availability(const rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection& s)const noexcept override {
        using A=rr::game::OriginalCourseLoadAvailability50KG;
        try{const auto a=availability_(s);return a!=A::ready?a:predicate_(s)?A::ready:A::hostNotReady;}
        catch(...){return A::hostNotReady;}
    }
};
template<class Predicate,class Availability> auto courseRaceDestination(Predicate predicate,Availability availability){
    return std::make_shared<DiagnosedCourseRaceDestination<Predicate,Availability>>(std::move(predicate),std::move(availability));
}
inline void require(bool accepted,const char* message) {
    if(!accepted)throw std::runtime_error(message);
}
std::shared_ptr<const rr::assets::OriginalUiArchive50KG> readArchive(
    const std::filesystem::path&,rr::assets::OriginalUiArchiveId50KG);
// NEW native application startup for the original-UI opt-in. This is a real
// process owner, not an imported save or a reconstruction of retail boot.
// The cold gameplay world stays unadvanced while the frontend owns input.
// An admitted Course destination borrows this same application RNG/selector
// owner after source exit and GPU drain. A changed track/rider is carried to
// the next cold world only after actual outgoing-world teardown.
// Native whole-race restart/return keeps this same process owner. No copied
// seed or replacement selected racer is used.
struct Video;
struct NativeResultsInputContinuation50LA;
struct NativeResultsHallTransfer50LA;
struct NativePauseInputContinuation50LA;
struct NativePauseMenuTransfer50LA;
class RaceDisplayLifetime50LA;
class RaceF8RendererLease50LA;
class NativeF8BarPrefix50LA;
class NativeResultsFrame50LA;
class NativeCollectorFifo50LA;
struct PositionStatsPrimitiveResult50LA;
struct PositionStatsResourceResult50LA;
// Cold staging result, never a drawable/renderer-ready receipt. On failure
// retain this result through diagnosis: its genuine partial owner and the
// Application's committed canonical table effects are not rolled back.
struct TimeStatsColdResult50LA {
    enum class Phase : std::uint8_t {fresh,archives,sources,owner,stats,fonts,text,pixels,
        population,awaitingFinalVerification,cpuPrepared,failed};
    Phase phase=Phase::fresh,failedAt=Phase::fresh;
    bool sourceEffectsCommitted=false;
    std::shared_ptr<rr::game::OriginalTimeStats50LA> owner;
    // Fixed image order:0 stats,1 authentic13ingame PST+CEL.
    std::shared_ptr<const rr::assets::OriginalUiScreenAssets50KG> pixels;
    rr::game::OriginalTimeStatsDiagnostic50LA object;
    rr::assets::OriginalApplicationFontDiagnostic50LA font;
    rr::assets::OriginalStandardMaterialDiagnostic50LA material;
    rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic provider;
    rr::assets::OriginalUiLocalizationDiagnostic50KG localization;
    rr::assets::OriginalUiPageDiagnostic50KG source;
    rr::assets::OriginalUiScreenAssetDiagnostic50KG image;
    std::string failure;
};
// Staged source resources only. Awaiting verification is not GPU readiness.
struct SpeedBoostColdResult50LA {
    enum class Phase:std::uint8_t {fresh,archives,sources,owner,textures,triangle,fonts,text,
        pixels,awaitingFinalVerification,cpuPrepared,failed};
    Phase phase=Phase::fresh,failedAt=Phase::fresh;
    bool sourceEffectsCommitted=false;
    std::shared_ptr<rr::game::OriginalSpeedBoost50LA> owner;
    std::shared_ptr<const rr::assets::OriginalUiScreenAssets50KG> pixels;
    rr::game::OriginalSpeedBoostDiagnostic50LA object;
    rr::assets::OriginalApplicationFontDiagnostic50LA font;
    rr::assets::OriginalStandardMaterialDiagnostic50LA material;
    rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic provider;
    rr::assets::OriginalUiLocalizationDiagnostic50KG localization;
    rr::assets::OriginalUiPageDiagnostic50KG source;
    rr::assets::OriginalUiScreenAssetDiagnostic50KG image;
    std::string failure;
};
// Uncalled cold-only Damage staging. Never a full loader/draw readiness receipt.
struct DamageColdResult50LA {
    enum class Phase:std::uint8_t {fresh,archives,sources,owner,textures,pixels,
        awaitingFinalVerification,cpuPrepared,failed};
    Phase phase=Phase::fresh,failedAt=Phase::fresh;
    bool sourceEffectsCommitted=false;
    std::shared_ptr<rr::game::OriginalDamage50LA> owner;
    std::shared_ptr<const rr::assets::OriginalUiScreenAssets50KG> pixels;
    rr::game::OriginalDamageDiagnostic50LA object;
    rr::assets::OriginalStandardMaterialDiagnostic50LA material;
    rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic provider;
    rr::assets::OriginalUiScreenAssetDiagnostic50KG image;
    std::string failure;
};
struct Application {
    friend class ::ra_cold_aggregate50LA::Owner; //Same-App material admission and actual ordered source directory stores; no lazy prepare.
    friend class ::original_camera_session50KX::Session;
    friend class rr::game::OriginalSessionF8Intro50LA;
    friend class RaceDisplayLifetime50LA;
    friend class RaceF8RendererLease50LA;
    friend class ResultsRendererLease50LA;
    friend class NativeResultsFrame50LA;
    friend class NativeF8BarPrefix50LA;
    friend class rr::platform::OriginalUiLightManagerOwner50KG;
    friend class rr::assets::OriginalApplicationLocalization50KG;
    friend class rr::platform::OriginalLocalizationRendererLease50KG;
    friend class rr::game::OriginalVideoPreFlipList50KG;
    friend class rr::platform::OriginalUiGarageFogColorLease50KG;
    friend class rr::platform::OriginalUiGarageTextOverlay50KG;
    friend class rr::platform::OriginalUiGarageScreenText50KG; //Authenticated Course text loaders borrow this App's retained resources and source directories.
    friend class rr::platform::OriginalUiGaragePageOwner50KG;
    friend class rr::platform::OriginalUiCoursePagePayload50KG;
    friend class rr::platform::OriginalUiCoursePopupPayload50KG;
    friend class rr::platform::OriginalUiGarageSession50KG;
    friend class rr::platform::OriginalUiSharedDialogs50KG;
    friend class rr::platform::OriginalUiMemoryCardConstructor50KG;
    friend class rr::platform::OriginalUiFrontend50KG;
    friend class rr::game::OriginalLoadingScreen40Mode0LA;
    friend class OriginalLoadingCollector50LA;
    bool enabled=false;
    // Host-only device choice from an accepted Course Confirm, not save/profile
    // data. A new SDL window must authenticate and neutral-arm its own device;
    // the old window's instance ID and button levels are never reused.
    mutable bool courseHumanController=false;
    // Persistent native preview request only; never a profile unlock or race grant.
    // Legacy Clegg request remains separate; each frontend authenticates its caches.
    bool prepareCleggLockedPreview=false;
    // Additive original rider resource pairs only, retained across frontend rebuilds.
    std::uint32_t requestedPreviewRiders=0;
    std::filesystem::path assetRoot,audioRoot,riderCacheRoot;
    bool decodeRiderImagesInProcess=false;
    rr::platform::OriginalUiMainAudioMode50KG audioMode=
        rr::platform::OriginalUiMainAudioMode50KG::muted;
    // One process-owned fresh normal-material setting for Main AND gameplay.
    // Original100008..100030 clears435670 AND435678 in BSS. Native debug overrides are
    // not admitted; never reinitialize this owner on page/race/session entry.
private:
    friend struct ::interactive_loop50DU::Context; //Actual successful-poll dispatch, before native clients.
    friend struct ::interactive_continuation50DU::Context; //Actual same-owner Root finish/capture/commit closure.
    friend struct ::interactive_publication50DU::Context; //Actual accepted-world human publication only.
    friend class rr::game::OriginalResults10450LA; //Per-event concrete process predecessors, no public input API.
    bool publishNativeHumanResultsRoot50LA(::interactive_publication50DU::Context&,
        rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    bool passResultsProcessPredecessors50LA(const rr::game::OriginalRaceConstructor50LA&,
        const rr::game::InputEvent48Z*,rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    friend class ::original_pause_observation50LA::Owner;
    friend class ::original_race_hud_scope50LA::Registration; //Separate ALL8 listener camera borrow, never End/HUD authority.
    friend class ::original_results_roster50LA::Read;
    friend class ::ra_native_session50LA::Join;
    const std::shared_ptr<rr::assets::OriginalPauseSourceData50LA>&
        pauseSourceDataForResults50LA(const original_pause_cold50LA::Owner&)const noexcept;
    bool loadConsumedPauseSourceCold50LA(rr::game::OriginalRaceConstructor50LA&,
        original_pause_cold50LA::Owner&,bool beforeCollector)const noexcept;
    bool loadConsumedPauseResultsBeforeCollector50LA(rr::game::OriginalRaceConstructor50LA&,
        original_pause_cold50LA::Owner&,std::shared_ptr<ra_cold_roster50LA::Operation>,
        rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    bool loadHumanTournamentPauseResultsBeforeCollector50LA(rr::game::OriginalRaceConstructor50LA&,
        original_pause_cold50LA::Owner&,std::shared_ptr<ra_cold_roster50LA::Operation>,
        const std::array<std::uint32_t,8>&,rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    bool loadPauseResultsBeforeCollectorImpl50LA(rr::game::OriginalRaceConstructor50LA&,
        original_pause_cold50LA::Owner&,std::shared_ptr<ra_cold_roster50LA::Operation>,
        const std::array<std::uint32_t,8>*,rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    // Private F8 slot38 bridge, called at the native accepted-Root pre-Camera
    // cut with this actual140CE4 factor and Race/F8. No new root admission.
    bool updateConsumedF8199320(rr::game::OriginalRaceF8Diagnostic50LA&)const noexcept;
    // Reached only by the actual retained Session's two ordered intro cuts.
    // Fixed199770 tuple; no end invocation, Session flag input or renderer gate.
    bool revealConsumedSessionF8Intro50LA(original_camera_session50KX::Session&,bool direct)const noexcept;
    // Results slot38 body, reached by the private native ordered-return join.
    // It still uses only the same retained Root23C, never a supplied delta.
    // Native visual provenance remains separate; exact base state chronology
    // uses the explicitly native successful-Window-poll availability adapter.
    bool updateConsumedResults1C8AE0(rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    // Native Loop calls before other clients, at original input-before-update
    // chronology. Same current Video poll; exact eligible sibling predecessors.
    // No supplied keys, originalCBC claim, input ack or Root-mode write.
    bool pressConsumedResults1C8C30(rr::platform::GameHost50ER&,
        rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    mutable std::shared_ptr<NativeResultsInputContinuation50LA> resultsInputContinuation50LA_;
    mutable std::shared_ptr<NativeResultsHallTransfer50LA> resultsHallTransfer50LA_;
    // Distinct private ALL8 Pause origin and counted App17DE10(6,4/6,1)
    // transfer. No human Results/Hall entry or public readiness authority.
    mutable std::shared_ptr<NativePauseInputContinuation50LA> pauseInputContinuation50LA_;
    enum class PauseReturnStart50LA { unavailable,sourceSkipped,started,failed };
    PauseReturnStart50LA beginNativePauseReturn50LA(const original_pause_observation50LA::Owner&,
        rr::platform::OriginalSpectatorMenuInput50LA&,rr::platform::OriginalVideoBlockingNativeBackend*,
        std::uint32_t&)const noexcept;
    bool pauseInputContinuationCurrent50LA()const noexcept;
    bool pauseInputBatchReturned50LA(const NativePauseInputContinuation50LA&)const noexcept;
    bool pauseReturnInputPending50LA()const noexcept;
    bool advanceNativePauseReturn50LA(std::uint32_t&)const noexcept;
    bool cancelNativePauseReturn50LA(rr::platform::OriginalVideoBlockingNativeBackend*)const noexcept;
    mutable std::shared_ptr<NativePauseMenuTransfer50LA> pauseMenuTransfer50LA_;
    bool preparePauseMenuTransfer50LA(const NativePauseInputContinuation50LA&)const noexcept;
    bool commitPauseMenuTransfer50LA(const NativePauseInputContinuation50LA&)const noexcept;
    bool markPauseMenuBatchReturned50LA(const NativePauseInputContinuation50LA&)const noexcept;
    bool pauseMenuRequestTailCurrent50LA(const rr::game::OriginalRaceConstructor50LA&)const noexcept;
    bool passPauseMenuProcessSuccessors50LA(const rr::game::OriginalRaceConstructor50LA&)const noexcept;
    bool beginNativePauseMenuDeparture50LA(bool& selected)const noexcept;
    bool nativePauseMenuDeparting50LA()const noexcept;
    bool cancelNativePauseMenuTransfer50LA()const noexcept;
    friend class rr::platform::OriginalUiPauseCourseOrigin50LA;
    bool nativePauseMenuReceiverCurrent50LA()const noexcept;
    bool nativePauseMainReceiverCurrent50LA()const noexcept;
    bool nativePauseCourseReceiverCurrent50LA()const noexcept;
    std::shared_ptr<rr::platform::OriginalUiPauseCourseOrigin50LA> pauseCourseReceiverOrigin50LA(const Frontend&)const noexcept;
    bool nativePauseCourseOriginCurrent50LA(const rr::platform::OriginalUiPauseCourseOrigin50LA&,
        const rr::platform::OriginalUiCourseSession50KG&)const noexcept;
    bool bindNativePauseMenuReceiver50LA(const Frontend&,const std::shared_ptr<Video>&)const noexcept;
    bool acceptNativePauseMenuReceiver50LA(const Frontend&,const rr::platform::VehicleWindowGl50DS&)const noexcept;
    bool prepareResultsHallTransfer50LA(rr::game::OriginalRaceConstructor50LA&)const noexcept;
    bool commitResultsHallTransfer50LA(rr::game::OriginalRaceConstructor50LA&,
        const rr::game::OriginalRaceRecordInsertionStage50KG&,
        rr::platform::OriginalSaveProfileOwner50KG&)const noexcept;
    bool resultsRequestTailCurrent50LA(const rr::game::OriginalRaceConstructor50LA&)const noexcept;
    bool passResultsProcessSuccessors50LA(const rr::game::OriginalRaceConstructor50LA&)const noexcept;
    bool beginNativeResultsHallDeparture50LA(bool& selected)const noexcept;
    bool cancelNativeResultsHallTransfer50LA()const noexcept;
    bool nativeResultsHallDeparting50LA()const noexcept;
    bool nativeResultsHallReceiverCurrent50LA()const noexcept;
    bool acceptNativeResultsHallReceiver50LA()const noexcept;
    // 1AB5BC: the common race's acknowledgement request 17DE10(App,6,screen,1),
    // appended to App10 like 1CA6F0's Hall request and consumed by the same
    // departure/receiver lifecycle. The route is the original's decision.
    bool commitRaceAcknowledgementTransfer50PD(rr::game::OriginalRaceConstructor50LA&,
        const rr::game::OriginalRaceAckDestination50PD&)const noexcept;
    // Receiver route of a retired-session 1AB5BC request; false otherwise.
    bool nativeRaceReturnRoute50PD(std::uint32_t& screen,std::uint32_t& page)const noexcept;
    bool acceptNativeRaceReturnReceiver50PD()const noexcept;
    // 1AB680 / 1AB838: 1AC440(App28C) for the 1AB5BC record row, before 17DE10.
    bool commitRaceAcknowledgementRecords1AC440_50PD(rr::game::OriginalRaceConstructor50LA&,
        const rr::game::OriginalRaceAckDestination50PD&)const noexcept;
    bool resultsInputContinuationCurrent50LA()const noexcept;
    enum class ResultsBlockingInputOrigin50LA : std::uint32_t {
        summary=0x1c8f5c,tournament=0x1c98bc,nameDone=0x1ca778,hall=0x1ca678};
    bool suspendResultsBlockingInput50LA(rr::game::OriginalResults10450LA&,
        rr::game::OriginalRaceConstructor50LA&,const original_camera_session50KX::Session&,
        rr::platform::GameHost50ER&,ResultsBlockingInputOrigin50LA)const noexcept;
    std::uint32_t resultsBlockingInputReturnPC50LA(const rr::game::OriginalResults10450LA&)const noexcept;
    bool resultsDonePending50LA()const noexcept;
    bool advanceResultsDone50LA(rr::platform::OriginalVideoBlockingNativeBackend&,
        rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    bool cancelResultsDone50LA(rr::platform::OriginalVideoBlockingNativeBackend*)const noexcept;
    // Called only at Loop's two successful native Camera/world callback cuts,
    // BEFORE its existing Root return. Not complete source1A7B40/141ED0.
    bool updateNativeResultsAfterRace50LA(const original_camera_session50KX::Session&,
        rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    // Offered only by the successful Loop return path, AFTER real Root finish
    // and BEFORE that same timing owner's commit. No arbitrary clock receipt.
    bool captureNativeResultsAfterRoot50LA(const original_camera_session50KX::Session&,
        rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    bool prepareNativeResultsFrame50LA(const original_camera_session50KX::Session&,
        rr::platform::VehicleWindowFrame50DS&,std::unique_ptr<NativeResultsFrame50LA>&,
        rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    bool nativeResultsFrameCurrent50LA(const NativeResultsFrame50LA&,
        const rr::platform::VehicleWindowGl50DS&,const rr::platform::VehicleWindowFrame50DS&)const noexcept;
    // Direct existing-owner observation, not an original last-camera receipt.
    bool nativeResultsFinalCameraCurrent50LA(const rr::game::OriginalRaceConstructor50LA&,
        const original_camera_session50KX::Session&)const noexcept;
    bool resultsPublishedRoot50LA(const rr::game::OriginalRaceConstructor50LA&,
        rr::game::OriginalResults104Diagnostic50LA&,bool requireUpdateEligible=true)const noexcept;
    // UNCALLED CPU-only slot44 bridge; no scheduler/render/input admission.
    bool layoutConsumedF81994B0(rr::game::OriginalRaceF8Diagnostic50LA&)const noexcept;
    // Native resource custody; no original gameplay render-phase admission.
    bool retainConsumedF8Renderer50LA(rr::game::OriginalRaceF8Diagnostic50LA&)const noexcept;
    bool f8RendererFifoCurrent50LA(const NativeCollectorFifo50LA&)const noexcept;
    bool resultsRendererFifoCurrent50LA(const NativeCollectorFifo50LA&)const noexcept;
    bool retainConsumedResultsRenderer50LA(rr::game::OriginalResults104Diagnostic50LA&)const noexcept;
    // Actual same-Session synchronous native bars/icon/text contribution.
    // No original mode/index assignment, FIFO or complete199500 return.
    bool prepareNativeF8Bars50LA(const original_camera_session50KX::Session&,
        rr::platform::VehicleWindowFrame50DS&,
        std::unique_ptr<NativeF8BarPrefix50LA>&,rr::game::OriginalRaceF8Diagnostic50LA&)const noexcept;
    bool nativeF8BarsCurrent50LA(const NativeF8BarPrefix50LA&,
        const rr::platform::VehicleWindowGl50DS&,const rr::platform::VehicleWindowFrame50DS&)const noexcept;
    // UNCALLED actual-owner1A7DD0 body, not an end-ready setter. It executes
    // reached F8/Results/HUD and owned1AB520 pre-ack effects per suspended
    // invocation, stopping at actual missing reads/after20C. No live trigger,
    // input acknowledgment writer, reward or Hall/save continuation follows.
    void continueConsumedRaceEndEntry1A7DD0(rr::game::OriginalRaceEndEntryDiagnostic50LA&)const noexcept;
    // Actual issued-RA state producers only. Inputs are this retained Root23C,
    // registered real audio/display owners and canonical Race fields, never
    // caller booleans or a copied Session clock. End dispatch remains separate.
    rr::game::OriginalHumanEndClockResult50KG advanceConsumedSessionEndClock1A7DA0(
        original_camera_session50KX::Session&)const noexcept;
    original_camera_session50KX::Result advanceConsumedSessionFadeIn1A7BD0(
        original_camera_session50KX::Session&)const noexcept;
    friend class rr::platform::OriginalUiGarageDrawContribution50KG;
    mutable std::uint32_t sourceCameraIndex0C_=0;
    //13FC90..13FC9C. These remain this actual process Root's fields across
    //Garage reentry. Ordinary menu projections do not manufacture this cursor.
    mutable std::uint32_t sourceRenderedCamera268_=0,sourcePreviousModels270_=0,sourceRenderedModels274_=0;
    mutable float sourceSecondMapDistance382FA0_=130.f; //ELF data43020000, actual2410E0 writer only.
    const void* sourceDebugOverlay14_=nullptr; //13FBF8; no bounded native debug-overlay creation writer.
    std::uint32_t sourceModelPassOverride4354F8_=0; //Actual process BSS, no native bounded writer.
    struct RenderHistory140230 final {
        std::array<std::int32_t,8> diagnostic{};std::array<bool,8> diagnosticGuard{};
        std::array<std::uint32_t,4> words{};std::array<bool,4> wordGuard{};
    };
    mutable RenderHistory140230 sourceRenderHistory140230_;
    // Actual process BSS and the reached 1407A4/244270/24427C stores. These
    // are not host presentation counters and never reset on Garage entry.
    mutable std::uint32_t sourceRenderDispatch423F40_=0,sourceRenderStats4355F0_=0,sourceRenderStats4355F8_=0;
    struct RenderPeak164CB0 { std::uint32_t timestamp00=0;std::int32_t value04=0;std::uint32_t lifetime08=0; };
    mutable std::array<RenderPeak164CB0,2> sourceRenderPeaks4355C0_{};

    //Actual data word36ADA8=1. No native debug writer is inferred from GL.
    std::uint32_t sourceCameraSearch36ADA8_=1;
    //Actual process BSS435600. No bounded native input/race writer exists;
    //retain it across Garage entries, independent of GL debug flags.
    std::uint32_t sourceCameraSynchronous435600_=0;
    mutable std::array<bool,4> sourceCameraRenderLazyGuards_{};
    mutable std::array<std::int32_t,4> sourceCameraRenderLazyWords_{}; //3E00B0/B8/C0/C8.
    friend class rr::platform::OriginalApplicationAural50KG;
    // The retained stream owner verifies this same Application resolver's
    // shared ownership at lookup; raw pointer equality is not sufficient.
    friend class rr::platform::OriginalStreamLibrarySource50LA;
    friend struct Video;
    friend struct Frontend;
    friend class rr::platform::OriginalUiCourseOwnedLifecycle50KG;
    enum class CourseLoadingPhase50LA : std::uint8_t {
        absent,uiRetirement,audioRetirement,audioReset,pendingClear,activity,videoRequest,terminal
    };
    struct CourseLoadingPrefix50LA {
        bool attempted=false;
        CourseLoadingPhase50LA phase=CourseLoadingPhase50LA::absent;
        Frontend* frontend=nullptr;
        std::shared_ptr<RaceDisplayLifetime50LA> display;
        std::shared_ptr<Video> video;
        std::shared_ptr<const rr::platform::OriginalUiRiderProfileOwner50KG> profile;
        rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection selection{};
        std::uint32_t stopPC=0x17d7a0;
    };
    mutable CourseLoadingPrefix50LA courseLoadingPrefix50LA_;
    bool advanceCourseRaLoadingPrefix50LA(Frontend&,std::uint32_t&)const noexcept;
    bool withdrawCourseRaLoadingPrefix50LA(Frontend&)const noexcept;

    friend class NativeCollectorFifo50LA;
    // Opaque original process word3E1C10, included in entry100008..30 BSS
    // zeroing. Its gameplay meaning/nonzero producer is not established.
    // Retain actual storage once with Application, never per race/session.
    std::uint32_t sourceProcessWord3E1C10_=0;
    // Actual source startup13FD40 owner. Separate from native SDL/presentation
    // input and uncalled until its true startup dispatcher predecessor joins.
    friend class rr::game::OriginalPlayerConstructorPrefix50LA;
    mutable std::shared_ptr<rr::game::OriginalApplicationInput04Source50LA> sourceInput04_;
    //Declare caller before body so the body is destroyed first. Both are
    //embedded one-shot identities; close revokes caller before descendant.
    mutable rr::game::OriginalApplicationStartupCall23A0DCNative50LA nativeStartupCall23A0DC_;
    mutable rr::game::OriginalApplicationStartup17C910Source50LA sourceStartup17C910_;
    //Private and uncalled: source entry stops at the unowned argument read.
    //The separate native normal-UI continuation never fabricates guest argv.
    rr::game::OriginalApplicationStartup17C910Source50LA::Result enterSourceStartup17C910()const noexcept;
    rr::game::OriginalApplicationStartup17C910Source50LA::Result continueNativeNormalStartup17C910()const noexcept;
    rr::game::OriginalApplicationStartup17C910Source50LA::Result continueSourceStartupDirectories17CA88()const noexcept;
    rr::game::OriginalApplicationStartup17C910Source50LA::Result continueSourceStartupAppStart17CAB8()const noexcept;
    rr::game::OriginalApplicationStartup17C910Source50LA::Result continueSourceStartupMaterialRead17CAD4()const noexcept;
    rr::game::OriginalApplicationStartup17C910Source50LA::Result continueSourceStartupMaterialRows29871C()const noexcept;
    rr::game::OriginalApplicationStartup17C910Source50LA::Result continueSourceStartupMaterialPopulation2987E8()const noexcept;
    rr::game::OriginalApplicationStartup17C910Source50LA::Result continueSourceStartupDefaultTemplate17CAE8()const noexcept;
    rr::game::OriginalApplicationStartup17C910Source50LA::Result continueSourceStartupScreenTemplate17CAFC()const noexcept;
    rr::game::OriginalApplicationStartup17C910Source50LA::Result continueSourceStartupInputMappings17CB04()const noexcept;
    rr::game::OriginalApplicationStartup17C910Source50LA::Result captureNativeStartupBodyResult17CB9C()const noexcept;
    rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result enterNativeStartupCall23A0DC()const noexcept;
    rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result returnNativeStartupCall23A0E4()const noexcept;
    rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result continueNativeStartupInitialize23A0E4()const noexcept;
    rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result enterNativeInitialize237640()const noexcept;
    rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result continueNativeStartupVideo2376A0()const noexcept;
    bool nativeStartupVideoInitializationOwnerCurrent()const noexcept;
    bool nativeStartupVideoIntcBoundaryCurrent()const noexcept;
    rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result continueNativeStartupVideoIntc25A68C()const noexcept;
    rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result holdNativeStartupVideoIntcCustody()const noexcept;
    rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result continueNativeStartupModeTable2376C0()const noexcept;
    rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Result continueNativeStartupVideoDescendants25A5EC()const noexcept;
    bool constructSourceInput04_13FD40()const noexcept {
        if(sourceInput04_||!enabled||!entities())return false;
        try{sourceInput04_.reset(new(std::nothrow) rr::game::OriginalApplicationInput04Source50LA);}
        catch(...){return false;}
        return sourceInput04_&&sourceInput04_->begin13FD40(*this);
    }
    mutable std::shared_ptr<rr::platform::OriginalSourcePadHostLease50LA> sourcePadHostLease_;
    bool bindSourceInput04NativeTransport()const noexcept;
    bool continueSourceInput04NativeActivation()const noexcept;
    friend class rr::game::OriginalPlayerOverlayRendererLease50LA;
    std::shared_ptr<rr::game::OriginalPlayerOverlayRendererLease50LA> borrowSourcePlayerOverlayRenderer199990(
        const rr::game::OriginalRaceConstructor50LA*)const noexcept;
    bool readSourcePlayerRaceMode199980(const rr::game::OriginalRaceConstructor50LA*,std::int32_t&)const noexcept;
    std::shared_ptr<const rr::game::OriginalPlayerRaceMusicBorrow50LA> borrowSourcePlayerMusic210810(
        const rr::game::OriginalRaceConstructor50LA*)const noexcept;
    bool sourceInput04NativeBorrowCurrent(const rr::game::OriginalApplicationInput04Source50LA* input)const noexcept;
    bool closeSourceInput04Native()const noexcept {
        if(!nativeStartupCall23A0DC_.closeNative(*this))return false;
        if(!sourceStartup17C910_.closeNative(*this))return false;
        if(sourcePadHostLease_)sourcePadHostLease_->revoke();
        sourcePadHostLease_.reset();
        return !sourceInput04_||sourceInput04_->closeNative(*this);
    }
    mutable std::int32_t sourceApplicationMode22C_=1; //13FDBC genuine constructor store.
    // Source23C is not constructor-written here. Only the actual140CE4
    // continuation publishes its passed/capped factor, never raw250.
    mutable std::optional<float> sourceApplicationElapsed23C_;
    mutable original_ui_source_timing50LH::PublicationView nativeTaRootTiming_;
    mutable const rr::game::OriginalRaceConstructor50LA* nativeTaRootRace_=nullptr;
    mutable const rr::game::OriginalWreckComponent50LA* nativeTaRootController_=nullptr;
    mutable std::uint64_t nativeTaRootPublication_=0,nativeTaRootGeneration_=0;
    mutable bool nativeTaRootEntered_=false,nativeTaRootAborted_=false;
    void clearRaceProcessWord1A6884() noexcept { sourceProcessWord3E1C10_=0; }

    // Weak custody of the ONE Application-created canonical TA native Session.
    // Root/consumers own its lifetime; this never prolongs borrowed stack owners.
    mutable std::weak_ptr<original_camera_session50KX::Session> nativeTaCameraSession50LA_;
    // Uncalled private source prefix. Selects only the retained factory-issued
    // Session; no caller Session, key, generation, clock or readiness parameter.
    std::shared_ptr<original_camera_session50KX::Session> freshNativeTaCameraSession50LA()const noexcept;
    // Uncalled native custody leaf, not a source loader or child constructor.
    bool retainNativeTaCutAuthoring50LA()const noexcept;
    bool constructNativeTaController174A30()const noexcept;
    // Private, uncalled object-local unloaded child lifetime. No source74
    // publication, registry key, VUB loader or replacement process input.
    bool constructUnpublishedNativeTaCutChild282860()const noexcept;
    bool abortUnpublishedNativeTaCutChild()const noexcept;
    // Private, UNCALLED first-TA01 copied-input resource prerequisite only.
    // No caller child/ordinal/file/FP promise or source IO completion authority.
    std::shared_ptr<original_camera_session50KX::Session> resourceNativeTaCameraSession50LA()const noexcept;
    bool deriveUnpublishedNativeTa01Resources()const noexcept;
    mutable std::weak_ptr<RaceDisplayLifetime50LA> raceDisplayOwner50LA_;
    // The actual persistent dispatcher is Root-owned, never manufactured on
    // Garage entry. The two counted global dialog children share this scope.
    mutable std::shared_ptr<rr::platform::OriginalApplicationAural50KG> sourceAural00Owner_;
    bool stageAuralLightNativePrefix13FE24(std::uint32_t nativeOutputRate,
        rr::platform::OriginalApplicationAural50KG::Diagnostic&,std::string&)const noexcept;
    bool stageAuralCold13FE24(std::shared_ptr<rr::platform::OriginalApplicationAural50KG>&,
        rr::platform::OriginalApplicationAural50KG::Diagnostic&)const noexcept;
    bool stageLightManagerAfterAuralNative140074(std::shared_ptr<rr::platform::OriginalUiLightManagerOwner50KG>&,
        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic&)const noexcept;
    bool initializeAuralNative102DE0(rr::platform::OriginalApplicationAural50KG::Diagnostic&)const noexcept;
    bool attachAuralNativePrefix140108(rr::platform::OriginalApplicationAural50KG::Diagnostic&)const noexcept;
    bool nativeSourceAudioDispatcherCurrent(const rr::platform::OriginalApplicationAural50KG&)const noexcept;
    bool beginNativeFrontendRootAudio140CE0(Frontend&,const rr::platform::OriginalUiMainSessionUpdate50KG&,
        rr::platform::OriginalUiFrontendDiagnostic50KG&,std::string&)const noexcept;
    bool finishNativeFrontendRootAudio140CFC(Frontend&,const rr::platform::OriginalUiFrontendUpdateReceipt50KG&,
        rr::platform::OriginalUiFrontendDiagnostic50KG&,std::string&)const noexcept;
    friend class rr::platform::OriginalUiGaragePodScene50KG;
    bool clearAuralRegistrations103DD0(rr::platform::OriginalApplicationAural50KG::Diagnostic&)const noexcept;
    bool releaseAuralCold102CD0(rr::platform::OriginalApplicationAural50KG::Diagnostic&)const noexcept;
    mutable std::shared_ptr<rr::game::OriginalUiEntityScope50KG> sharedUiScopeOwner_;
    mutable std::shared_ptr<rr::platform::OriginalUiMemoryCardConstructor50KG> memoryCardConstructorOwner_;
    // Bounded last-completed dispatcher receipt. This keeps the old address
    // generation distinct while its genuine successor is constructed.
    mutable std::shared_ptr<rr::platform::OriginalUiSharedDialogs50KG> retiredSharedUiDialogs_;
    mutable rr::game::OriginalUiControllerPopupState50KG retiredControllerPopupState_{};
    bool sharedUiRetirementReceiptCurrent17C1D8()const noexcept;
    enum class SharedUiBootResult { unavailable,complete,failed };
    SharedUiBootResult prepareSharedDialogsInitial1B7BD0(
        const rr::platform::OriginalUiMainDrawResources50KG&,
        std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,
        rr::platform::VehicleWindowGl50DS&,std::string&,
        // True only for a race-return lifetime whose FIRST manager is Garage13:
        // its own Page1FB648 performs the 1B86D0 reorder, not Main1D7CF4.
        bool firstManagerGarage=false)const noexcept;
    bool finishSharedUiDispatcherAfterBanks1B7140(std::string&)const noexcept;
    rr::game::OriginalUiEntityScope50KG::Result loadDispatcherGlobalWidgets2A35E0()const noexcept;
    // Executes only Display20/24 and proven zero global-row hide. The caller
    // must still execute actual manager detach/142590/clear30, in that order.
    rr::game::OriginalUiEntityScope50KG::Result continueDispatcherTransitionPrefix2A3510(
        std::uint64_t,rr::platform::VehicleWindowDiagnostic50DS&)const noexcept;

    // Uncalled scalar-only issuance; not a renderer-service or startup grant.
    std::shared_ptr<rr::platform::OriginalUiGarageFogColorLease50KG> retainGarageFogColor13E8A8()const noexcept;
    mutable std::weak_ptr<rr::game::OriginalPauseStorage50LA> pauseColdOwner50LA_;
    mutable std::array<std::weak_ptr<rr::game::OriginalTunnelTrack50LA>,2> tunnelTrackColdOwners50LA_;
    mutable std::weak_ptr<rr::game::OriginalRaceTuning50LA> raceTuningColdOwner50LA_;
    mutable std::weak_ptr<rr::game::OriginalRaceConstructor50LA> raceConstructorColdOwner50LA_;
    // Actual Application28C publication. Native backing is retained through
    // partial load/cleanup; only explicit completed native retirement clears it.
    mutable std::shared_ptr<rr::game::OriginalRaceConstructor50LA> sourceRace28COwner_;
    // Actual constructor17C714 scalar, not a manager pointer or enabled flag.
    // The native normal-UI constructor writes1 before consumers. Original
    // predefined-track17C9EC writes0 only AFTER its actual argv/298 producer;
    // native host CLI/environment are not that input and do not enable it.
    std::optional<std::uint32_t> sourceUseProfile2E0_;
    rr::game::OriginalAudioDirectorySource50LA sourceAudioDirectory36AFE0_;
    // Genuine single-view1A9240 prefix and concrete Windows/Linux adapter.
    // Native accepted initialization is distinct from a retail2361C0 receipt.
    bool continueConsumedRaCamera1A9240(rr::game::OriginalRaceConstructor50LA&,std::uint32_t&)const noexcept;
    // Private read-only native ALL8 custody, not Camera34 completion/issuance.
    // Used by the actual Pause borrower while the original1A9240 stays pending.
    bool consumedRaAll8CameraCustodyNative34Current(const original_camera_session50KX::Session&)const noexcept;
    bool bindConsumedRaCameraNative34(original_camera_session50KX::Session&)const noexcept;
    // TA slot30: the counted Camera34 route is engaged only for the human
    // course race whose race94 this Application-issued TA Session borrows.
    // Any other TA Session keeps its previous (unbound) route unchanged.
    bool consumedTaCamera34Engaged(const original_camera_session50KX::Session&,bool requireBound)const noexcept;
    bool completeConsumedRaCameraNative34(original_camera_session50KX::Session&)const noexcept;
    enum class All8MusicStatus:std::uint8_t {unavailable,nativeChildrenReady,failed};
    // Separate native ALL8 entry: actual counted10C and prepared PCM/Dax join,
    // real listener registration and shared FIFO. No human End/HUD authority,
    // original child IO, camera completion or retail1A3C00 return is implied.
    All8MusicStatus stageConsumedRaAll8MusicNative10C(
        original_camera_session50KX::Session&,std::uint32_t&)const noexcept;
    bool consumedRaAll8ListenerCustodyNativeCurrent(const original_camera_session50KX::Session&)const noexcept;
    bool advanceConsumedRaAll8MusicListenerNative(const original_camera_session50KX::Session&,
        std::uint64_t beforeMix,bool heldSourceScene,std::uint32_t&)const noexcept;
    bool serviceConsumedRaAll8MenuAudioNative(const original_camera_session50KX::Session&,live_audio50HF::Owner&,
        std::uint32_t elapsedMicroseconds,rr::platform::AudioOutputDiagnostic50HD&,std::uint32_t&)const noexcept;
    bool completeConsumedRaMusicNative10C(original_camera_session50KX::Session&,std::uint32_t&)const noexcept;
    bool advanceConsumedRaMusicListenerNative(const original_camera_session50KX::Session&,
        std::uint64_t beforeMix,bool heldSourceScene,std::uint32_t&)const noexcept;
    bool consumedRaMusicNative10CCurrent(const original_camera_session50KX::Session&)const noexcept;
    bool consumedRaMusicNativeOnlyNoPreparation(const original_camera_session50KX::Session&)const noexcept;
    //Explicit native progress/archive/scene/TOB prefix: pc1AA0C0 stops BEFORE
    //World slotA4/271CF0. pc1A6CC8 preserves the never-prepared native-only
    //audio route without ANY new effects. Neither is full1A9EE0 completion.
    // Private conditional source segment; no caller joins the missing music1A3C00 interval.
    bool loadRetainedSourceTrackSegment1A6CF4(std::uint32_t&)const noexcept;
    bool completeConsumedRaWorldNativePrefix1A9EE0(
        original_camera_session50KX::Session&,std::uint32_t&)const noexcept;
    bool retireConsumedRaMusicNative10C(rr::game::OriginalRaceConstructor50LA&,std::uint32_t&)const noexcept;
    bool revokeConsumedRaCameraNative34(original_camera_session50KX::Session&)const noexcept;
    bool retireConsumedRaCameraNative34(rr::game::OriginalRaceConstructor50LA&,std::uint32_t&)const noexcept;
    // Explicit private source-backed Display route; does not bind a Session or
    // grant nativeWorld images, metadata, or the remainder of1A9240.
    bool initializeConsumedRaLoadingCamera2361C0(rr::game::OriginalRaceConstructor50LA&,std::uint32_t&)const noexcept;
    bool abortConsumedRaLoadingCameraNative34(rr::game::OriginalRaceConstructor50LA&,std::uint32_t&)const noexcept;
    bool finishConsumedRaLoadingCamera1A9750(rr::game::OriginalRaceConstructor50LA&,std::uint32_t&)const noexcept;

    bool continueConsumedRaWorldInit1A6BE0(rr::game::OriginalRaceConstructor50LA&,
        std::uint32_t&)const noexcept;
    // Native-only creator affinity and one cold allocation. Ordinary8 remains
    // unchanged unless actual early Aural/Light startup already issued this
    // retained backing before the fresh frontend output's first publication.
    const std::thread::id garageAudioOwnerThread_=std::this_thread::get_id();
    mutable std::shared_ptr<rr::platform::OriginalUiGarageAudioBacking50KG> garageAudioBackingOwner_;
    mutable bool garageAudioBackingAttempted_=false;
    bool garageAudioApplicationInputsCurrent()const noexcept;
    bool prepareGarageAudioBackingCold(std::uint32_t nativeOutputRate,std::string&)const noexcept;
    bool retainGarageAudioTransportCold(std::shared_ptr<rr::platform::OriginalUiMainAudio50KG>&,
        std::string&)const noexcept;
    bool finishGarageAudioBackingCold(std::string&)const noexcept;
    bool rearmGarageAudioAfterManagerRetirementCold(
        const std::shared_ptr<rr::platform::OriginalUiGaragePageOwner50KG>&,
        std::uint64_t retiredManagerGeneration,std::string&)const noexcept;
    bool retireGarageAudioBackingCold(std::string&)const noexcept;
    friend class AudioOutput;
    bool prepareFrontendAudioOutput(Frontend&,const rr::platform::AudioOutputRequest50HD&,
        std::string&,rr::platform::AudioOutputDiagnostic50HD&)const;
    bool closeFrontendAudioOutput(AudioOutput&,
        const std::shared_ptr<rr::platform::OriginalUiGarageAudioBacking50KG>&,
        rr::platform::AudioOutputDiagnostic50HD&)const noexcept;
    bool retireFrontendAudioBackingClosed(std::string&)const noexcept;
    bool frontendAudioClosedReceiptCurrent()const noexcept;
    bool prepareFrontendMenuAudio(Frontend&,const rr::platform::OriginalUiFrontendCold50KG&,
        std::string&)const noexcept;
    mutable bool frontendAudioShutdownComplete_=false;
    // Actual Tournament input5 resource transaction. No counted manager is
    // created until a complete Garage frame destination can own its lifetime.
    void prepareGarageDestinationResources1F241C(Frontend&,
        const rr::platform::OriginalUiTournamentRouteProposal50KG&,
        rr::platform::OriginalUiGarageDestinationDiagnostic50KG&,
        rr::platform::OriginalUiFrontendColdInterval50KG&)const noexcept;
    // Shared selected-career resource admission (archives, pod, scene assets).
    // Caller has already proved its origin: Tournament proposal or the retired
    // race's 17DE10(App,6,13,1) request. Creates no counted manager.
    void prepareGarageCareerResources1D1950(Frontend&,
        rr::platform::OriginalUiGarageDestinationDiagnostic50KG&,
        rr::platform::OriginalUiFrontendColdInterval50KG&)const noexcept;
    bool loadPreparedGarageManager1D1950(Frontend&,
        std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,
        std::shared_ptr<rr::platform::OriginalUiGarageSession50KG>&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool garageManagerLoadedCurrent(const Frontend&,const rr::platform::OriginalUiGarageSession50KG&)const noexcept;
    bool observeGarageRuntime50KG(const Frontend&,const rr::platform::OriginalUiGarageSession50KG&,
        rr::platform::OriginalUiGarageNativeState50KG&)const noexcept;
    bool inputGaragePage1FDE70(Frontend&,std::uint32_t,std::uint32_t,
        rr::platform::OriginalUiGarageNativeInput50KG&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool publishGarageTarget140A68(Frontend&,int,int,
        rr::platform::VehicleWindowDiagnostic50DS&,std::string&)const noexcept;
    bool prepareNextGarageFrame140CE0(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    // Source-owned entry prefix, still not a normal menu-route capability.
    // The Root retains every actual manager/transfer/model prefix on failure.
    mutable std::shared_ptr<rr::platform::OriginalUiGarageSession50KG> garageCustodyOwner_;
    mutable bool garageCustodyAttempted_=false;
    //Actual private normal LOAD driver. It owns sequencing and all native
    //inputs; successful source load is not normal frame/menu admission.
    bool loadGarageManager1D1950(Frontend&,
        std::shared_ptr<const rr::assets::OriginalUiGarageSceneAssets50KG>,
        std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,std::int32_t requestedPage,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    //Bounded actual dispatcher callback, not frontend route/frame admission.
    //Scoped manager/dispatcher returns have separate retained state. This
    //continuation still refuses success at the missing outer26C/root10 walk.
    //Its actual RuntimeFrame remains retained, preventing callback replay.
    bool continueGarageDispatcherUpdate141ED0(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    // Authentic same-Frontend frame borrow, with no source callback effects.
    // Shared by the explicit legacy dispatcher seam and enclosing Root path.
    bool retainGarageUpdateFrame140CF4(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    // Scoped140CE0/CE4 -> outer26C/root10 invocation. It does not claim the
    // earlier245030/input prefix or later140230 render/audio/frame completion.
    bool continueGarageRootUpdate140CE0(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool prepareGarageManagerEntry1D1950(Frontend&,
        std::shared_ptr<const rr::assets::OriginalUiGarageSceneAssets50KG>,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    // Continues the same entry's real Search/Light/Fog/22-Widget owners.
    // Fonts is authenticated immutable input, never an initialized context.
    bool continueGarageManagerConstructors1CCD14(Frontend&,
        std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool continueGarageCustodyInputsCold(
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    // Historical detached downstream seam. The normal entry above never
    // accepts a caller-issued Grant, model population or constructor receipt.
    bool prepareGarageCustodyCold(
        std::shared_ptr<rr::game::OriginalUiEntityScope50KG::GarageConstructionGrant>,
        std::shared_ptr<rr::platform::OriginalUiGarageModelResources50KG>,
        std::shared_ptr<const rr::assets::OriginalUiGarageSceneAssets50KG>,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool continueGarageWidgetsCustodyCold(
        std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool continueGaragePageCustodyCold(
        std::shared_ptr<rr::platform::OriginalUiGarageManagerSearch50KG>,
        std::shared_ptr<const rr::assets::OriginalUiGaragePageData50KG>,
        std::shared_ptr<const rr::platform::OriginalUiGarageSharedPageDrawOwner50KG>,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool continueGaragePageSceneCustodyCold(
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool continueGaragePageSpecificCustodyCold(
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    // Actual Application17C650/17C76C initializes344 to0. The only
    // recovered startup write1 is exact argv[1]=BuildUIResFiles at17C98C.
    // This native normal-UI owner does not admit the resource-builder mode;
    // do not derive this field from host argc or a Page-local ready boolean.
    const std::int32_t sourceBuildUiResFiles344_=0;
    //13FC74 constructs Application228 bit5 true;17CD7C reinforces it.
    //The admitted normal native path has no producer clearing this bit.
    //This owns only that proven flag, not an invented whole guest byte.
    const bool sourceTextureCallback228Bit5_=true;
    //13FBF4 clears this actual Application bit;140008 clears it again.
    //Keep process history separate from native renderer/current Camera state.
    //No bounded normal UI writer enables it; debug input is unsupported.
    bool sourceDebugCamera228Bit2_=false;
    //13FC20 clears bit3 independently.140CA4 bypasses update AND render
    //when set;140EA8/140F04 also gate source key-event traversal. No current
    //native normal UI writer changes it. This is not Camera debug bit2.
    bool sourceUpdateRenderBypass228Bit3_=false;
    // Process BSS100008..100030. Original148250 toggles these only via
    //1277D0(mask80), rejected by143E30 while the retained bit2 is clear.
    // No Garage entry/reset writes them; enabled debug history is unsupported.
    std::int32_t sourceLightDebug426F50_=0,sourceLightDebug426F58_=0;
    bool rewriteGarageCameraSlots1401B0(const rr::platform::OriginalUiGaragePageOwner50KG&)const noexcept;
    bool observeGarageLoaderApplication344(
        const rr::platform::OriginalUiGaragePageOwner50KG&,std::int32_t&)const noexcept;
    bool continueGarageManagerLoadReturnCold(std::int32_t requestedPage,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool continueGaragePageLateForestCustodyCold(
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool continueGaragePageForestCustodyCold(
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool continueGarageScenePopulationCustodyCold(
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool continueGarageScenePrefixCustodyCold(
        std::shared_ptr<const rr::platform::OriginalUiMainProfile50KG>,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool continueGarageWidgetsThrough22_1CD860(rr::platform::OriginalUiGarageWidget1Owner50KG&,
        std::shared_ptr<rr::platform::OriginalUiGarageWattoOwner50KG>,
        std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG&)const noexcept;
    bool continueGarageWattoCustodyCold(
        std::shared_ptr<rr::platform::OriginalUiGarageWidget1Owner50KG>,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    // Exact selected-complete base-cache or pre-Domain corpus failure receipt:
    // retire selected, transfer/bank/manager, then Main's empty entry borrow.
    // Never a failure after Domain append or any later constructor operation.
    //Exact failed first shared prompt with constructed Widgets/Watto, no Scene.
    bool retireGarageWattoBindingFailure(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool retireGarageLoadedPageHideFailure(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool retireGarageLateSceneBorrowFailure(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool retireGaragePartialSceneAttachmentFailure(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool retireGarageSharedLoaderFailure(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool retireGarageEntryBeforeDat(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    // Exact native failed-entry unwind before any selected model is created.
    // Does not claim a completed source manager or retire shared Main/Aural.
    bool retireGarageEntryBeforeSelected(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    //Exact failed read of Pending after persistent Main borrow, before its
    //per-entry begin call. Does not retire/create any namespace or Main entry.
    bool retireGaragePendingRetainFailure(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    //Exact terminal failure after selected resource construction, before its
    //first successful Scope enrollment. No per-entry audio or Page existed.
    bool retireGarageUnenrolledSelectedEntry(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    // Private exact-owner normal Back continuation; not normal admission.
    bool retireGarageUnsealedRenderFailure(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool retireGarageActorSampleFailure(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool retireGaragePublishedObservationFailure(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool retireGarageManagerNormal2A3510(Frontend&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool releaseGarageCustodyNormal(
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool retireGarageCustodyConsumersCold(
        rr::assets::OriginalStandardMaterialDiagnostic50LA&,std::string&)const noexcept;
    bool releaseRetiredGarageCustodyCold(std::string&)const noexcept;
    // Private isolated root audio stages. These retain the ONE namespace in
    // entitiesOwner_; no manager/widget constructs a replacement. Ordinary
    // startup and Garage construction do not call these unjoined stages.
    bool preparePendingWaves17CD84(
        std::shared_ptr<rr::platform::OriginalAudioPendingWaves50KG>&,std::string&)const noexcept;
    bool retainPendingWaves98A4(
        std::shared_ptr<rr::platform::OriginalAudioPendingWaves50KG>&,std::string&)const noexcept;
    bool preparePendingWaveNativeBacking(std::string&)const noexcept;
    bool retirePendingWaves2668E0(
        const std::shared_ptr<rr::platform::OriginalAudioPendingWaves50KG>&,std::string&)const noexcept;
    // Private cold-only issuance. Ordinary startup does not call this staged leaf.
    mutable std::shared_ptr<rr::platform::OriginalUiLightManagerOwner50KG> sourceLightManager426F60Owner_;
    std::shared_ptr<rr::game::OriginalUiLightManagerLifetime50KG> lightManagerStartupWitness_;
    bool stageLightManagerCold140074(std::shared_ptr<rr::platform::OriginalUiLightManagerOwner50KG>&,
        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic&)const noexcept;
    bool releaseLightManagerCold148020(rr::platform::OriginalUiLightManagerOwner50KG&,
        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic&)const noexcept;
    // Source-shaped frontend one-font loader; no ordinary startup issuance.
    bool loadGarageTextFont29CB74(std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,
        rr::assets::OriginalApplicationFontDiagnostic50LA&)const noexcept;
    bool loadGarageTextFont29CB74(std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,
        rr::assets::OriginalApplicationFonts50LA::Name,
        rr::assets::OriginalApplicationFontDiagnostic50LA&)const noexcept;
    // This staged seam remains closed before genuine selected-text ownership.
    bool prepareGarageScreenText29CBD8(std::shared_ptr<rr::platform::OriginalUiGarageScreenText50KG>&,
        rr::platform::OriginalUiGarageScreenTextDiagnostic50KG&)const noexcept;
    // Only the genuinely reached detached Widget1 constructor prefix is admitted.
    bool prepareGarageWidget1_1CD7A0(rr::platform::OriginalUiGarageWidget1Owner50KG&,
        std::shared_ptr<rr::game::OriginalUiEntityScope50KG::GarageConstructionGrant>,
        std::shared_ptr<rr::platform::OriginalUiGarageManagerFog50KG>,
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG&)const noexcept;
    // Reached2A9540 continuation: actual typed selected text and same-root FONTS.
    bool continueGarageWidget1_2A9540(rr::platform::OriginalUiGarageWidget1Owner50KG&,
        std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG&)const noexcept;
    bool continueGarageWidgetsThrough9_1CD860(rr::platform::OriginalUiGarageWidget1Owner50KG&,
        std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG&)const noexcept;
    bool clearNativeRetiredRace28C(rr::game::OriginalRaceConstructor50LA&)const noexcept;
    // Process443848 FIFO identity. Scene/display close cannot silently reset it.
    mutable std::shared_ptr<NativeCollectorFifo50LA> nativeCollectorFifoOwner_;
    mutable bool uiCollector1B73BCAttempted_=false;
    bool constructUiCollector1B73BC(rr::assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    mutable std::uint64_t raceDisplayGeneration50LA_=0;
    mutable std::weak_ptr<rr::game::OriginalPositionStatsPrimitiveOwner50LA> positionStatsPrimitivesOwner50LA_;
    mutable std::weak_ptr<rr::game::OriginalPositionStatsResources50LA> positionStatsResourcesOwner50LA_;
    const std::shared_ptr<rr::game::HostMonotonicClock50ID> animationClockBackend_;
    const std::shared_ptr<rr::game::OriginalApplicationAnimationClock50KG> animationClockOwner_=
        std::shared_ptr<rr::game::OriginalApplicationAnimationClock50KG>(
            new rr::game::OriginalApplicationAnimationClock50KG);
    // Native application startup13FDC0..13FE0C, before UI/race ownership.
    // This SAME base Entity lifetime and globals survive every session.
    // Consumers pin it; no race entry creates replacement root ancestry.
    const std::shared_ptr<rr::game::OriginalApplicationEntities50LA> entitiesOwner_=
        std::make_shared<rr::game::OriginalApplicationEntities50LA>();
    //3C8D9C..3C8DC0's real process motion library, separate from Entity
    // counts and the file cache. Racer153610/153F50 share its ordered list.
    const std::shared_ptr<rr::game::OriginalMotionLibrary50LA> motionLibraryOwner_=
        std::make_shared<rr::game::OriginalMotionLibrary50LA>();
    // Retained immutable backing for the SAME process setting. The existing
    // public read name below aliases it; material consumers do not own a
    // separately resettable normal-mode copy.
    const std::shared_ptr<const rr::assets::OriginalUiMaterialRuntime50KG> materialRuntimeOwner_=
        std::make_shared<const rr::assets::OriginalUiMaterialRuntime50KG>(
            rr::assets::OriginalUiMaterialRuntime50KG{true,false,true,0u});
    // Original140030/14004C creates443870, a texture manager DISTINCT from
    // rendering Video443A60. Own its genuine CPU resource list once here.
    // The sealed TA construction context below pins this exact owner. No native GPU
    // memory manager or254AD0/254B20 frame caller is claimed or activated.
    const std::shared_ptr<rr::assets::OriginalStandardTextureLifetime50LA> textureLifetimeOwner_=
        std::shared_ptr<rr::assets::OriginalStandardTextureLifetime50LA>(
            new rr::assets::OriginalStandardTextureLifetime50LA);
    // One real native Video identity per application, including direct entry.
    // Normal player entry retains its actual native window/context through
    // serial frontend/race generations. Each generation still drains and
    // retires every renderer and GPU borrower before the next can begin.
    // Explicit diagnostic/headless entry keeps its original close/reopen policy.
    const bool retainPlayerWindow50DU_;
    mutable std::shared_ptr<Video> videoOwner_;
    // Original2376A0/259E00/2376BC is a DISTINCT source producer. Native
    // videoForSession(), Window creation, and immutable640x448/480 constants
    // do not enter it or certify it. A private native initializer may issue
    // this exact fresh producer; neither path claims guest outer startup.
    mutable rr::game::OriginalApplicationVideoSource50LA sourceVideoProduction_;
    friend class NativeVideoPacketInvocation50LA;
    mutable std::unique_ptr<NativeVideoPacketInvocation50LA> nativeVideoPacketInvocation50LA_;
    mutable std::uint64_t nativeVideoPacketInvocationSerial50LA_=0;
    rr::game::OriginalApplicationVideoSource50LA::Result constructSourceVideo2376A0()const noexcept {
        if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this))
            return rr::game::OriginalApplicationVideoSource50LA::Result::authority;
        if(sourceVideoProduction_.nativeInitializerCaller_){
            const auto& call=nativeStartupCall23A0DC_;
            if(sourceVideoProduction_.nativeInitializerCaller_!=&call||!call.current(*this)||
               call.phase_!=rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Phase::videoConstructionIssuing||
               call.initialization_.video!=&sourceVideoProduction_||!call.initialization_.videoCallIssued||
               call.initialization_.pc!=0x2376a0)
                return rr::game::OriginalApplicationVideoSource50LA::Result::authority;
        }
        return sourceVideoProduction_.begin2376A0(*this);
    }
    // Private, uncalled successor of the SAME source publication. Produces
    // only25A3E0's table prefix and stops before its descendant allocation;
    // it must not be invoked by a native renderer/session readiness query.
    rr::game::OriginalApplicationVideoSource50LA::ModeTableResult continueSourceVideoModeTable25A3E0()const noexcept {
        if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this))
            return rr::game::OriginalApplicationVideoSource50LA::ModeTableResult::authority;
        if(sourceVideoProduction_.nativeInitializerCaller_){
            const auto& call=nativeStartupCall23A0DC_;const auto& init=call.initialization_;
            if(sourceVideoProduction_.nativeInitializerCaller_!=&call||!call.current(*this)||
               call.phase_!=rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Phase::modeTableIssuing||
               init.video!=&sourceVideoProduction_||!init.videoPublicationObserved||
               !init.modeTableCallIssued||init.modeTablePrefixObserved||init.pc!=0x2376c0)
                return rr::game::OriginalApplicationVideoSource50LA::ModeTableResult::phase;
        }
        return sourceVideoProduction_.continueModeTable25A3E0(*this);
    }
    // Private, uncalled CPU successor. Retains original allocation/publication
    //ordering and stops before installing any platform callback or MMIO state.
    rr::game::OriginalApplicationVideoSource50LA::DescendantResult continueSourceVideoDescendants25A5EC()const noexcept {
        if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this))
            return rr::game::OriginalApplicationVideoSource50LA::DescendantResult::authority;
        if(sourceVideoProduction_.nativeInitializerCaller_){
            const auto& call=nativeStartupCall23A0DC_;const auto& init=call.initialization_;
            if(sourceVideoProduction_.nativeInitializerCaller_!=&call||!call.current(*this)||
               call.phase_!=rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Phase::descendantsIssuing||
               init.video!=&sourceVideoProduction_||!init.modeTableCallIssued||!init.modeTablePrefixObserved||
               !init.descendantsContinuationIssued||init.descendantsPrefixObserved||init.pc!=0x25a5ec)
                return rr::game::OriginalApplicationVideoSource50LA::DescendantResult::phase;
        }
        return sourceVideoProduction_.continueModeDescendants25A5EC(*this);
    }
    // Private, uncalled entry to the exact source syscall boundary only. A
    //pending call image is not native registration or an interrupt producer.
    rr::game::OriginalApplicationVideoSource50LA::RegistrationResult enterSourceVideoIntcRegistration25A68C()const noexcept {
        if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this))
            return rr::game::OriginalApplicationVideoSource50LA::RegistrationResult::authority;
        if(sourceVideoProduction_.nativeInitializerCaller_){
            const auto& call=nativeStartupCall23A0DC_;const auto& init=call.initialization_;
            if(sourceVideoProduction_.nativeInitializerCaller_!=&call||!call.current(*this)||
               call.phase_!=rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Phase::intcRegistrationIssuing||
               init.video!=&sourceVideoProduction_||!init.descendantsContinuationIssued||!init.descendantsPrefixObserved||
               !init.intcRegistrationCallIssued||init.intcSyscallBoundaryReached||init.pc!=0x25a68c)
                return rr::game::OriginalApplicationVideoSource50LA::RegistrationResult::phase;
        }
        return sourceVideoProduction_.enterIntcRegistration25A68C(*this);
    }
    // Native-only custody of the same pending source request. No host hook,
    //source return, IRQ, callback delivery, or mode admission is issued.
    rr::game::OriginalApplicationVideoSource50LA::NativeIntcCustodyResult holdSourceVideoPendingIntcNativeCustody()const noexcept {
        if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this))
            return rr::game::OriginalApplicationVideoSource50LA::NativeIntcCustodyResult::authority;
        if(sourceVideoProduction_.nativeInitializerCaller_){
            const auto& call=nativeStartupCall23A0DC_;const auto& init=call.initialization_;
            if(sourceVideoProduction_.nativeInitializerCaller_!=&call||!call.current(*this)||
               call.phase_!=rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Phase::intcCustodyIssuing||
               init.video!=&sourceVideoProduction_||!init.intcRegistrationCallIssued||!init.intcSyscallBoundaryReached||
               !init.intcCustodyAttempted||init.pc!=0x2f5884)
                return rr::game::OriginalApplicationVideoSource50LA::NativeIntcCustodyResult::phase;
        }
        return sourceVideoProduction_.holdPendingIntcNativeCustody(*this);
    }
    // Retain the exact construction capability, not just equivalent fields.
    // Its dependencies pin Video/templates/runtime/list, never Application,
    // so these two application-owned handles do not form a reference cycle.
    //17C6F4/6FC and17C7F0..804 copy exact literal3A85D8. Separate
    //actual process directory from mutable normal-material36B970.
    const std::string loadingTextureDirectory36BCA0_="UI\\Textures\\";
    mutable std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA> materialContextOwner_;
    // Created by this Application before original APPSTART/material loading.
    // Canonical tables remain lazy; the same service pair is never replaced.
    mutable std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> standardMaterialDomainOwner_;
    // Native identity of each actual detached Tournament construction attempt.
    // No source clock/profile/result meaning. Never reset on page return.
    mutable std::uint64_t tournamentPodConstructionGeneration_=0;
    // Explicit native late staging of actual330; never a replacement overlay.
    mutable std::shared_ptr<rr::game::OriginalLoadingManager33050LA> sourceLoading330Owner_;
    mutable bool nativeLoading330Attempted_=false;
    bool stageNativeLoading330Prefix(std::uint32_t&)const noexcept;
    bool retireNativeLoading330Prefix()const noexcept;
    // First bounded Application328 registry. Declared after its canonical
    // domain/context so owned fonts release their texture leases first.
    // Scene teardown must retire every ScreenText/renderer Entry borrow;
    // the process registry itself survives serial Video close/reopen.
    mutable std::shared_ptr<rr::assets::OriginalApplicationFonts50LA> fonts328Owner_;
    // Process-owned dictionary selected by the real initialized frontend below.
    mutable std::shared_ptr<rr::assets::OriginalApplicationLocalization50KG> sourceLocalization32COwner_;
    mutable std::shared_ptr<rr::platform::OriginalLocalizationRendererLease50KG> localizationRendererCall14C5A0_;
    // Native one-invocation envelope, not an original per-dictionary prohibition.
    // Future independent language calls require their own genuine caller join.
    mutable std::weak_ptr<rr::assets::OriginalApplicationLocalization50KG> localizationRendererAttempted32C_;
    mutable std::uint64_t localizationRendererInvocationSerial_=0;
    bool stageLocalizationRenderer14C5A0(std::shared_ptr<rr::platform::OriginalLocalizationRendererLease50KG>&,
        rr::platform::OriginalLocalizationRendererDiagnostic50KG&)const noexcept;
    bool advanceLocalizationRenderer14C5A0(rr::platform::OriginalLocalizationRendererLease50KG&,
        rr::platform::OriginalLocalizationRendererDiagnostic50KG&)const noexcept;
    bool cancelLocalizationRenderer14C5A0(rr::platform::OriginalLocalizationRendererLease50KG&,
        rr::platform::OriginalLocalizationRendererDiagnostic50KG&)const noexcept;
    bool stageLocalizationIndex17DEF4(std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,
        std::shared_ptr<rr::assets::OriginalApplicationLocalization50KG>&,
        rr::assets::OriginalApplicationLocalizationDiagnostic50KG&)const noexcept;
    bool selectLocalizationEnglish14C5F0(rr::assets::OriginalApplicationLocalization50KG&,
        rr::assets::OriginalApplicationLocalizationDiagnostic50KG&)const noexcept;
    bool borrowLocalizationText18AC60(rr::assets::OriginalApplicationLocalization50KG&,std::int32_t,
        std::shared_ptr<const rr::assets::OriginalApplicationLocalization50KG::SelectedText>&,
        rr::assets::OriginalApplicationLocalizationDiagnostic50KG&)const noexcept;
    bool retireLocalization17C8B8(rr::assets::OriginalApplicationLocalization50KG&,
        rr::assets::OriginalApplicationLocalizationDiagnostic50KG&)const noexcept;
    // Native startup/retirement bookkeeping is separate from original32C.
    // Repeated frontend incarnations borrow the SAME selected bytes; neither
    // native window recreation nor a failed attempt replays language selection.
    enum class FrontendLanguagePhase { untouched, preparing, selected, failed, retired };
    mutable FrontendLanguagePhase frontendLanguagePhase_=FrontendLanguagePhase::untouched;
    mutable std::shared_ptr<const rr::assets::OriginalUiArchive50KG> frontendLanguageArchive_,frontendFontsArchive_;
    bool prepareFrontendLanguageServices(Frontend&,rr::platform::VehicleWindowGl50DS&,std::string&)const;
    bool retireFrontendLanguageServices17C8A4(std::string&)const noexcept;
    // At most one live native-host-scheduled TimeStats cold owner. Neither
    // this weak lifetime guard nor the factory activates presentation.
    mutable std::weak_ptr<rr::game::OriginalTimeStats50LA> timeStatsColdOwner_;
    mutable std::weak_ptr<rr::game::OriginalSpeedBoost50LA> speedBoostColdOwner_;
    mutable std::weak_ptr<rr::game::OriginalDamage50LA> damageColdOwner_;
    // Actual mutable application36B970. Startup17CA6C/70 copies3A85C0.
    // UI17DC14 and gameplay17D814 write this SAME field below; canonical
    // model acquisition borrows it, never derives a path from asset metadata.
    mutable std::string materialDirectory36B970_="textures\\";
    // Startup17CA4C/50 copies3A85B0; UI/gameplay write this same field.
    //154140 borrows it for a real save/clear/restore around model setup.
    mutable std::string modelDirectory36B860_="geometry\\";
    //17CA98/AC copies the exact eight bytes3A8648;1A9FD0/1AA010 later
    //selects tracks again. Not derived from native asset-root/CLI paths.
    mutable std::string trackDirectory36B640_="tracks\\";
    //UI-resource startup remains unrepresented. Animation stays unknown until
    //the reached17CA1C/20 copy or existing1B8330 selection writes this SAME field.
    mutable std::optional<std::string> uiResourceDirectory36BB90_;
    mutable std::optional<std::string> animationDirectory36AED0_;

    //17C710/764/808..814 copies3A85E8's exact particle-directory string.
    const std::string particleDirectory36B530_="particle\\";
    // Startup17C724/734/818..82C copies3A85F8's seven bytes. Native
    // application directory custody, not a renderer/resource load receipt.
    const std::string skyDirectory36B750_="skies\\";
    //17C848/84C copies the six original bytes at3A8600 into36B420.
    const std::string fontDirectory36B420_="font\\";
public:
    // Borrow of the one Application-issued course Session (TA/MA/SA/GA/GC).
    // Its root owner closes it explicitly; this never extends that lifetime
    // beyond the synchronous caller.
    std::shared_ptr<original_camera_session50KX::Session> issuedCourseCameraSession50LA()const noexcept {
        return nativeTaCameraSession50LA_.lock();
    }
    // Native allocation/custody only, replacing the existing canonical-TA
    // make_shared at its real root. Does not call the source constructor.
    std::shared_ptr<original_camera_session50KX::Session> createNativeTaCameraSession50LA(
        original_camera_session50KX::ActualOwners,
        const original_camera_session50KX::AuthoredStorage&)const noexcept;
    // Same-root GC custody requires the genuine consumed GC race/world owner.
    std::shared_ptr<original_camera_session50KX::Session> createNativeGcCameraSession50LA(
        original_camera_session50KX::ActualOwners,
        const original_camera_session50KX::AuthoredStorage&)const noexcept;
    // Same-root GA custody requires the genuine consumed GA race/world owner.
    std::shared_ptr<original_camera_session50KX::Session> createNativeGaCameraSession50LA(
        original_camera_session50KX::ActualOwners,
        const original_camera_session50KX::AuthoredStorage&)const noexcept;
    // Same-root SA custody requires the genuine consumed SA race/world owner.
    std::shared_ptr<original_camera_session50KX::Session> createNativeSaCameraSession50LA(
        original_camera_session50KX::ActualOwners,
        const original_camera_session50KX::AuthoredStorage&)const noexcept;
    // Same-root MA custody requires the genuine consumed MA race/world owner.
    std::shared_ptr<original_camera_session50KX::Session> createNativeMaCameraSession50LA(
        original_camera_session50KX::ActualOwners,
        const original_camera_session50KX::AuthoredStorage&)const noexcept;
    const rr::assets::OriginalUiMaterialRuntime50KG& materialRuntime=*materialRuntimeOwner_;
    rr::game::OriginalApplicationAnimationClockView50KG animationClock()const noexcept {
        return rr::game::OriginalApplicationAnimationClockView50KG(animationClockOwner_);
    }
    bool publishAnimationTimestamp(const rr::game::HostMonotonicClock50ID& backend,
        std::uint32_t sampled)const noexcept {
        if(animationClockBackend_.get()!=&backend)return false;
        animationClockOwner_->value_.current230=sampled;
        animationClockOwner_->published_=true;return true;
    }
    bool sampleAnimationTimestamp(std::uint32_t& out)const noexcept;
    // Inactive source boundary APIs: the real outer Application/UI scheduler
    // must call these, not each fixed world tick. No delta/ready flag supplied.
    bool beginUpdateClock140AC0()const noexcept;
    bool commitUpdateClock140D30()const noexcept;
    bool resetUpdateClock140D70()const noexcept;
    bool animationPaused()const noexcept {return animationClockOwner_->value_.pause11C!=0;}
    // Native explicit pause/resume mapping, not17CC64 input-matcher parity.
    // No single-step120 writer is joined here; constructor120 remains zero.
    void setExplicitAnimationPause(bool paused)const noexcept {
        animationClockOwner_->value_.pause11C=paused?1u:0u;
    }
    std::shared_ptr<Video> videoForSession() const;
    // Diagnostic only. Constructor-published0x510 backing is not an admitted
    // display mode; the private source producer is still deliberately uncalled.
    bool inspectSourceVideo2376A0(rr::game::OriginalApplicationVideoSource50LA::Observation& out)const noexcept {
        return sourceVideoProduction_.inspect(*this,out);
    }
    bool inspectSourceStartup17C910(rr::game::OriginalApplicationStartup17C910Source50LA::Observation& out)const noexcept {
        return sourceStartup17C910_.inspect(*this,out);
    }
    bool inspectNativeStartupCall23A0DC(rr::game::OriginalApplicationStartupCall23A0DCNative50LA::Observation& out)const noexcept {
        return nativeStartupCall23A0DC_.inspect(*this,out);
    }
    // Native read-only lookup using25A900's data algorithm and the actual
    // owned source table. No source call, mode selection, or readiness grant.
    rr::game::OriginalApplicationVideoSource50LA::NativeModeQuery queryNativeSourceVideoMode25A900(
        std::uint32_t width,std::uint32_t height,std::uint32_t colorBits)const noexcept {
        return sourceVideoProduction_.queryNativeMode25A900(*this,width,height,colorBits);
    }
    //Explicit native CPU packet artifact from25CAD0's algorithm. Independent
    //immutable backing, not a clear operation or source mode/IRQ continuation.
    rr::game::OriginalApplicationVideoSource50LA::NativeClearPacket encodeNativeSourceVideoClearPacket25CAD0(
        std::uint32_t red,std::uint32_t green,std::uint32_t blue,std::uint32_t alpha)const noexcept {
        if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this))return {};
        return sourceVideoProduction_.encodeNativeClearPacket25CAD0(*this,red,green,blue,alpha);
    }
    //Independent CPU FINISH packet bytes. Encoding is not GPU submission,
    //a CSR FINISH event, an interrupt-counter tick or a source frame return.
    rr::game::OriginalApplicationVideoSource50LA::NativeFinishPacket encodeNativeSourceVideoFinishPacket25AB2C()const noexcept {
        if(!rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this))return {};
        return sourceVideoProduction_.encodeNativeFinishPacket25AB2C(*this);
    }
    // Explicit native private-target work only; not a startup/frame/DMA call.
    // A false result after issuance retains the invocation until cancellation.
    bool beginNativeVideoPacketInvocation50LA(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,
        rr::platform::OriginalVideoPacketCpuRenderer50LA::NativePolicies,std::size_t,
        NativeVideoPacketInvocationInfo50LA&,rr::platform::VehicleWindowDiagnostic50DS&)const noexcept;
    bool pollNativeVideoPacketInvocation50LA(NativeVideoPacketInvocationInfo50LA&,
        rr::platform::VehicleWindowDiagnostic50DS&)const noexcept;
    bool cancelNativeVideoPacketInvocation50LA(NativeVideoPacketInvocationInfo50LA&,
        rr::platform::VehicleWindowDiagnostic50DS&)const noexcept;
    bool retireNativeVideoPacketInvocation50LA(NativeVideoPacketInvocationInfo50LA&,
        rr::platform::VehicleWindowDiagnostic50DS&)const noexcept;
    bool retainsPlayerWindow50DU()const noexcept{return retainPlayerWindow50DU_;}
    // Read-only host access for the root loading presentation; not a display
    // factory, source readiness capability, or renderer/GPU lease.
    std::shared_ptr<Video> playerVideo50DU()const noexcept {
        return retainPlayerWindow50DU_?videoOwner_:std::shared_ptr<Video>{};
    }
    bool closePlayerWindow50DU(rr::platform::GameHostDiagnostic50ER&)const noexcept;
    // Called only after the real session stack and its gameplay Light lease
    // retire. Requires the exact consumed request; never selects a route.
    bool finishNativeResultsHallDeparture50LA()const noexcept;
    bool finishNativePauseMenuDeparture50LA()const noexcept;
    std::shared_ptr<RaceDisplayLifetime50LA> raceDisplayForSession50LA() const;
    // Exact frontend create successor only; success is a bounded114A00 leaf,
    // never loading-manager initialization or gameplay readiness.
    bool prepareFrontendLoading330Viewport(const std::shared_ptr<RaceDisplayLifetime50LA>&,
        rr::platform::VehicleWindowDiagnostic50DS&,std::uint32_t&)const noexcept;
    bool stagePositionStatsPrimitives50LA(const std::filesystem::path& originalRoot,
        PositionStatsPrimitiveResult50LA&) const noexcept;
    bool stagePositionStatsResources50LA(const std::filesystem::path& originalRoot,
        PositionStatsResourceResult50LA&) const noexcept;
    // Synchronous same-owner observation only; no retained camera borrow or draw grant.
    bool capturePositionStatsNumbers50LA(const rr::game::OriginalPositionStatsResources50LA&,
        const original_camera_session50KX::Session&,const native_ai50IV::Session&,
        rr::game::OriginalPositionStatsNumbers50LA&,
        rr::game::OriginalPositionStatsResourceDiagnostic50LA&) const noexcept;
    bool preparePositionStatsLayout50LA(rr::game::OriginalPositionStatsResources50LA&,
        const original_camera_session50KX::Session&,const native_ai50IV::Session&,
        rr::game::OriginalPositionStatsResourceDiagnostic50LA&) const noexcept;
    bool preparePositionStatsRadar50LA(rr::game::OriginalPositionStatsResources50LA&,
        const original_camera_session50KX::Session&,const native_ai50IV::Session&,
        rr::game::OriginalPositionStatsResourceDiagnostic50LA&) const noexcept;
    bool preparePositionStatsText50LA(rr::game::OriginalPositionStatsResources50LA&,
        const original_camera_session50KX::Session&,const native_ai50IV::Session&,
        rr::game::OriginalPositionStatsResourceDiagnostic50LA&) const noexcept;
    // Detached source-stage TunnelTrack ownership; no race publication/activation.
    std::shared_ptr<rr::game::OriginalTunnelTrack50LA> constructTunnelTrackCold2A1160(
        std::uint64_t,rr::game::OriginalTunnelTrackRole50LA,
        std::shared_ptr<const ra_cold_sources50LA::ColdNavigationLease>)const noexcept;
    bool bindTunnelTrackColdProjection2A1260(rr::game::OriginalTunnelTrack50LA&)const noexcept;
    std::shared_ptr<rr::game::OriginalRaceTuning50LA> retainRaceTuningCold1A7184()const noexcept;
    std::shared_ptr<rr::game::OriginalRaceConstructor50LA> constructConsumedRaceCold1A65C0(
        original_course_targets50KX::ConsumedLaunch&,std::uint64_t session,std::uint32_t& sourcePC)const noexcept;
    bool populateConsumedRaceResources1AA5A0(rr::game::OriginalRaceConstructor50LA&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool publishConsumedRace28C(const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>&)const noexcept;
    const rr::game::OriginalRaceConstructor50LA* race28C()const noexcept{return sourceRace28COwner_.get();}
    bool beginConsumedRaceLoad1A6AA0(rr::game::OriginalRaceConstructor50LA&,
        rr::game::OriginalRacerModelRenderGlobals50LA&,rr::assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool closeNativeConsumedRace(rr::game::OriginalRaceConstructor50LA&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool rollbackNativeRaceLoadPrefix(rr::game::OriginalRaceConstructor50LA&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool pauseConsumedRaceEntity142040(rr::game::OriginalRaceConstructor50LA&,bool)const noexcept;
    bool pauseConsumedRaceWorld1ACF50(rr::game::OriginalRaceConstructor50LA&,bool)const noexcept;
    bool constructConsumedRaceWorld1A6BC0(rr::game::OriginalRaceConstructor50LA&,
        std::shared_ptr<ra_cold_aggregate50LA::Owner>,std::uint32_t& sourcePC)const noexcept;
    bool constructConsumedTaRaceWorld1A6BC0(rr::game::OriginalRaceConstructor50LA&,
        std::shared_ptr<ta_original_world50LE::Owner>,std::uint32_t& sourcePC)const noexcept;
    bool constructConsumedGcRaceWorld1A6BC0(rr::game::OriginalRaceConstructor50LA&,
        std::shared_ptr<gc_original_world50LM::Owner>,std::uint32_t& sourcePC)const noexcept;
    bool constructConsumedMaRaceWorld1A6BC0(rr::game::OriginalRaceConstructor50LA&,
        std::shared_ptr<ma_original_world50LM::Owner>,std::uint32_t& sourcePC)const noexcept;
    bool constructConsumedSaRaceWorld1A6BC0(rr::game::OriginalRaceConstructor50LA&,
        std::shared_ptr<sa_original_world50LM::Owner>,std::uint32_t& sourcePC)const noexcept;
    bool constructConsumedGaRaceWorld1A6BC0(rr::game::OriginalRaceConstructor50LA&,
        std::shared_ptr<ga_original_world50LH::Owner>,std::uint32_t& sourcePC)const noexcept;
    const std::int32_t& applicationMode22C()const noexcept{return sourceApplicationMode22C_;}
    bool constructNativeTaIntroCamera50LA()const noexcept;
    std::shared_ptr<const rr::assets::OriginalTaIntroAuthoringCandidate> retainNativeTaIntroAuthoring50LA()const noexcept;
    bool beginNativeTaRaceRoot140CE0(rr::game::OriginalRaceConstructor50LA&,
        const rr::game::OriginalWreckComponent50LA&,const original_ui_source_timing50LH::PublicationView&,
        std::string&)const noexcept;
    // Native scheduling only: after this accepted Root's real Aural service,
    // before Camera175D70. No arbitrary delta or source dispatcher receipt.
    bool updateNativeTaRaceF8BeforeCamera50LA(rr::game::OriginalRaceConstructor50LA&,
        const rr::game::OriginalWreckComponent50LA&,const original_ui_source_timing50LH::PublicationView&,
        std::string&)const noexcept;
    bool abandonNativeTaRaceRootForTeardown50LA(rr::game::OriginalRaceConstructor50LA&,
        const rr::game::OriginalWreckComponent50LA&,const original_ui_source_timing50LH::PublicationView&,
        std::string&)const noexcept;
    bool finishNativeTaRaceRoot140CFC(rr::game::OriginalRaceConstructor50LA&,
        const rr::game::OriginalWreckComponent50LA&,const original_ui_source_timing50LH::PublicationView&,
        std::string&)const noexcept;
    // Exact conditional443848 typed destruction leaf; never an implicit drain.
    bool retireNativeCollector1A6948(rr::assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    // Source-only detached ownership. Definitions are in original_pause_entity50la.h.
    bool prewarmPauseCold50LA(original_pause_cold50LA::Owner&)const noexcept;
    bool loadConsumedPauseCold50LA(rr::game::OriginalRaceConstructor50LA&,original_pause_cold50LA::Owner&)const noexcept;
    // Every-track human race: the same Pause storage/load sequence without a
    // consumed RA race owner (session generation and launch subtype supplied).
    bool loadHostRacePauseCold50LA(original_pause_cold50LA::Owner&,std::uint64_t sessionGeneration,
        std::int32_t launchSubtype)const noexcept;
    bool finishPauseDrawCold50LA(original_pause_cold50LA::Owner&)const noexcept;
    std::shared_ptr<rr::game::OriginalPauseStorage50LA> constructPauseCold1A8AB0(
        std::uint64_t actualSessionGeneration)const noexcept;
    std::shared_ptr<rr::game::OriginalPauseStorage50LA> constructConsumedRacePause1A8AB0(
        rr::game::OriginalRaceConstructor50LA&)const noexcept;
    bool attachNativeConsumedRacePause(rr::game::OriginalRaceConstructor50LA&,
        const std::shared_ptr<original_camera_session50KX::Session>&)const noexcept;
    bool finishPauseEntityCold1A8ADC(rr::game::OriginalPauseStorage50LA&)const noexcept;
    // Terminal caller only, after the real frontend/race stack and gameplay
    // Light lease have closed. Course handoff closes its output, NOT Root.
    bool finishNativeFrontendAudio(std::string&)const noexcept;
    const std::shared_ptr<rr::game::OriginalApplicationEntities50LA>& entities() const noexcept {
        return entitiesOwner_;
    }
    const std::shared_ptr<rr::game::OriginalMotionLibrary50LA>& motionLibrary4333C0()const noexcept {
        return motionLibraryOwner_;
    }
    const std::string& materialDirectory36B970() const noexcept {return materialDirectory36B970_;}
    std::string& modelDirectory36B860() const noexcept {return modelDirectory36B860_;}
    const std::string& particleDirectory36B530() const noexcept {return particleDirectory36B530_;}
    const std::string& skyDirectory36B750() const noexcept {return skyDirectory36B750_;}
    //160F74/88 saves the actual live path then selects36B530.161014 restores
    //that same path. Native exceptions also restore it; no shadow directory.
    template<class Operation> bool withParticleMaterialDirectory160F88(Operation&& operation) const {
        std::string previous=materialDirectory36B970_;
        materialDirectory36B970_=particleDirectory36B530_;
        try {const bool result=operation(materialDirectory36B970_);
            materialDirectory36B970_.swap(previous);return result;}
        catch(...){materialDirectory36B970_.swap(previous);throw;}
    }
    void enterUiMaterialDirectory17DB80() const {
        modelDirectory36B860_="ui\\geometry\\";materialDirectory36B970_="ui\\textures\\";
    }
    void enterGameplayMaterialDirectory17D7A0() const {
        modelDirectory36B860_="geometry\\";materialDirectory36B970_="textures\\";
    }
    // Empty staging may be issued after real canonical-domain startup. No
    // historical mounts/rows/cache state is inferred and no service is activated.
    bool stageArchiveReader(std::shared_ptr<rr::assets::OriginalArchiveStagedReader50LA>& out) const noexcept;
    // Uncalled actual-Application camera constructor prefix. Stops before2361C0;
    // no page attachment, observer slot, renderer or visibility service exists here.
    rr::game::OriginalFrontendCameraCold50LA::Result stageFrontendCameraCold(
        std::shared_ptr<rr::game::OriginalFrontendCameraCold50LA>& out)const noexcept;
    // Partial scalar initialization only, stopped before renderer114A00 entry.
    rr::game::OriginalFrontendCameraCold50LA::Result stageFrontendCameraViewportPrefix1149D4(
        rr::game::OriginalFrontendCameraCold50LA& owner)const noexcept;
    rr::game::OriginalFrontendCameraCold50LA::Result releaseFrontendCameraCold(
        rr::game::OriginalFrontendCameraCold50LA& owner)const noexcept;
    // Uncalled Tournament cold-reference proposal. This does not mount archives,
    // construct a new canonical domain/Video, or admit a Tournament page.
    bool stageUiSearchListCold(
        std::shared_ptr<rr::platform::OriginalUiSearchListCold50KG>& out,
        rr::platform::OriginalUiSearchListColdDiagnostic50KG& d)const noexcept;
    bool releaseUiSearchListCold(rr::platform::OriginalUiSearchListCold50KG& owner,
        rr::platform::OriginalUiSearchListColdDiagnostic50KG& d)const noexcept;
    // Uncalled staged252C70 owner. Never performs2542F0 or publishes13C.
    bool stageTextureTransferCold(
        std::shared_ptr<rr::assets::OriginalStandardTextureTransferCold50LA>& out,
        rr::assets::OriginalStandardTextureTransferColdDiagnostic50LA& d)const noexcept;
    bool releaseTextureTransferCold(rr::assets::OriginalStandardTextureTransferCold50LA& owner,
        rr::assets::OriginalStandardTextureTransferColdDiagnostic50LA& d)const noexcept;
    bool stageTournamentPodReferencesCold(
        std::shared_ptr<const rr::platform::OriginalUiTournamentCareerResources50KG>,
        std::shared_ptr<rr::platform::OriginalUiTournamentPodReferences50KG>&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool releaseTournamentPodReferencesUnpublished(
        rr::platform::OriginalUiTournamentPodReferences50KG&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> standardMaterialDomain() const;
    const rr::game::OriginalAudioDirectorySource50LA& nativeAudioDirectory36AFE0()const noexcept {
        return sourceAudioDirectory36AFE0_;
    }
    // Cold factory only. Video must already have minted this application's
    // actual construction context; no caller-provided replacement or ready
    // flag can create a font domain. This does not load fonts or activate HUD.
    std::shared_ptr<rr::assets::OriginalApplicationFonts50LA> fonts328() const;
    // Cold caller only, before renderer publication. The font registry cannot
    // be populated through a detached directory or public texture setter.
    bool loadTimeStatsFonts193A0C(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        rr::assets::OriginalApplicationFontDiagnostic50LA& diagnostic) const;
    // Source-only Speed/Boost font pair; no startup caller activates this.
    bool loadSpeedBoostFonts18A99C(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        rr::assets::OriginalApplicationFontDiagnostic50LA& diagnostic) const;
    //Uncalled PositionStats one-font route through this SAME Application328.
    //Authentic PST/provider corpus must precede190E64 in the future full loader.
    bool loadPositionStatsFont190E64(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        rr::assets::OriginalApplicationFontDiagnostic50LA& diagnostic) const;
    // Explicit whole-world COLD policy only, before any renderer borrows or
    // submissions. Caller supplies the real native session generation. No
    // caller boolean can certify the cold boundary. Result must retire before
    // Application/font-manager destruction. The native ALL8 root calls this.
    bool stageTimeStatsNativeCold50LA(const std::filesystem::path& originalRoot,
        std::uint64_t actualSessionGeneration,TimeStatsColdResult50LA&,bool deferPopulation=false)const noexcept;
    bool finishTimeStatsNativeCold50LA(TimeStatsColdResult50LA&)const noexcept;
    // Unactivated Speed/Boost stages. Stage references before TimeStats,
    // then finish against the actual final whole-table population/collector.
    bool stageSpeedBoostNativeCold50LA(const std::filesystem::path& originalRoot,
        std::uint64_t actualSessionGeneration,SpeedBoostColdResult50LA&)const noexcept;
    bool finishSpeedBoostNativeCold50LA(SpeedBoostColdResult50LA&)const noexcept;
    bool stageDamageNativeCold50LA(const std::filesystem::path& originalRoot,
        std::uint64_t actualSessionGeneration,DamageColdResult50LA&)const noexcept;
    bool finishDamageNativeCold50LA(DamageColdResult50LA&)const noexcept;
    // Present even when Original Main is disabled: direct gameplay shares the
    // same process lifetime. Authentic bytes/templates load only at first use.
    // Frontend teardown, Course handoff and race restart never replace it.
    std::shared_ptr<rr::assets::OriginalUiMaterialApplication50LA> materialApplication=
        std::make_shared<rr::assets::OriginalUiMaterialApplication50LA>();
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> appStart(
        const std::filesystem::path& originalRoot) const;
    std::shared_ptr<rr::platform::OriginalUiMainProfile50KG> profile;
    // One fresh application profile owner; destinations only borrow it.
    std::shared_ptr<rr::platform::OriginalUiRiderProfileOwner50KG> riderProfile;
    std::shared_ptr<rr::platform::OriginalUiOptionsProfile50KG> optionsProfile;
    std::shared_ptr<rr::game::OriginalUiGameSettingsProfile50KG> gameSettingsProfile;
    std::shared_ptr<rr::game::ControlPresets49D> controlsProfile;
    mutable rr::platform::native_beta50kg::SettingsStore betaSettings;
    mutable rr::platform::native_beta50kg::Result betaSettingsStatus=rr::platform::native_beta50kg::Result::unavailable;
    void showBetaPersistenceStatus(const char* normalTitle=nullptr) const noexcept;
    void saveAcceptedSettings() const noexcept;
    std::shared_ptr<rr::platform::OriginalSoundMode50KG> soundMode;
    std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> state;
    //140084/147FF8 application lifetime, shared by every frontend incarnation.
    std::shared_ptr<rr::game::OriginalUiLightManagerLifetime50KG> lightManager;
    Application(const Application&)=delete;
    Application& operator=(const Application&)=delete;
    ~Application();
    explicit Application(std::shared_ptr<rr::game::HostMonotonicClock50ID> clock,
        std::shared_ptr<rr::game::ControlPresets49D> controls={},bool retainPlayerWindow=false);
};
// Owns the actual host/context and its native source-canvas mode together.
// shared_ptr conversions retain this object and use its real destructor.
// RGBA8/D24 private targets are owned by the actual VehicleWindowGl renderer;
// source Z format0x31 is the admitted original24-bit packet representation.
struct Video {
    friend struct Application;
    friend class rr::game::OriginalLoadingScreen40Mode0LA;
    friend class rr::platform::OriginalUiFrontend50KG;
    friend class rr::platform::OriginalLocalizationRendererLease50KG;
    friend class rr::game::OriginalVideoPreFlipList50KG;
    friend class rr::platform::OriginalUiGarageDrawContribution50KG;
    rr::platform::GameHost50ER host;
    const bool originalMain;
    const std::int32_t width=640,height,colorBits=32,packetZMaximum=0xffffff;
    const std::uint32_t zBufferFormat=0x31;
    ~Video()=default;
private:
    // Actual base2AE080 list precedes the later Video40/44 constructor writes.
    rr::game::OriginalVideoPreFlipList50KG sourcePreFlip18_;
    rr::game::OriginalVideoServiceFields50KG sourceRendererServiceVideo_; //259E9C precedes259F58.
    // Actual Video443A60 scalar, initialized by259F58. It is not a
    // resource pointer, renderer queue44, or a claim that race load ran.
    std::uint32_t sourceMode44_=0;
    friend class rr::platform::OriginalUiCourseOwnedLifecycle50KG;
    class CourseModeEffect;
    CourseModeEffect* courseModeEffect_=nullptr;
    class OptionsModeEffect;
    OptionsModeEffect* optionsModeEffect_=nullptr; // Exclusive actual scalar producer.
    // The ordinary RA aggregate still owns its separate recovered effect
    // continuation. Do not pretend its writes transferred back into these
    // same-Video frontend fields after a real race display use.
    bool beginRaceDisplay1A7658() noexcept {
        if(optionsModeEffect_||courseModeEffect_)return false;
        sourceMode44_=1;sourceFrontendEndEffectsKnown_=false;return true;
    }
    struct EndView25B110 {
        std::uint32_t overlay40C=0; //25A1E8, four real slots.
        std::uint8_t overlay494=0; //25A250.
        std::uint8_t covered2F4=0,increasing2F8=0,decreasing2FC=0,completed300=0;
        float lastMilliseconds3C4=0; //25A098.
    };
    std::array<EndView25B110,4> sourceEndViews_{};
    rr::game::OriginalVideoFadeState sourceEndFade_; //Actual259E00 initialized fields;3E8 remains unknown.
    bool sourceFrontendEndEffectsKnown_=true;

    bool clearRaceDisplay1A6870() noexcept {
        if(optionsModeEffect_||courseModeEffect_)return false;
        sourceMode44_=0;return true;
    }
    // Actual2A3A90 ownership, not a scalar-equality readiness test. This
    // private native effect is separate from source manager completion.
    class CourseModeEffect final {
        friend struct Video;
        friend class rr::platform::OriginalUiCourseOwnedLifecycle50KG;
        using Construction=rr::game::OriginalUiEntityScope50KG::CourseConstruction;
        std::shared_ptr<Video> video_;
        std::shared_ptr<rr::game::OriginalUiEntityScope50KG> scope_;
        Construction* construction_=nullptr;
        const Application* issuer_=nullptr;
        const rr::platform::OriginalUiFrontend50KG* frontend_=nullptr;
        const void* manager_=nullptr;
        std::thread::id thread_;
        std::uint32_t before_=0;
        bool acquired_=false,nativeAborted_=false;
        CourseModeEffect(std::shared_ptr<Video> video,Construction& construction,
            const Application& issuer,const rr::platform::OriginalUiFrontend50KG* frontend,
            const void* manager)noexcept
            :video_(std::move(video)),scope_(construction.scope_),construction_(&construction),issuer_(&issuer),
             frontend_(frontend),manager_(manager),thread_(std::this_thread::get_id()){}
        bool pending()const noexcept {return acquired_&&!nativeAborted_;}
        bool effectCurrent()const noexcept {
            // No issuer dereference or live host/clock requirement on native
            // cancellation. The exact retained Video and registration survive.
            return pending()&&std::this_thread::get_id()==thread_&&video_&&construction_&&
                scope_==construction_->scope_&&video_->courseModeEffect_==this&&construction_->videoEffect_==this&&
                construction_->videoOwnedPhase()&&
                construction_->frontend_==frontend_&&construction_->managerOwner_==manager_&&
                construction_->registrationCurrent();
        }
        bool abortCurrent()const noexcept {
            return effectCurrent()&&!construction_->pageAcquired_&&!construction_->searchList_&&
                !construction_->searchNodeAcquired_;
        }
        bool current(const Application& root,const std::shared_ptr<Video>& video,
            Construction& construction,const rr::platform::OriginalUiFrontend50KG* frontend,
            const void* manager)const noexcept {
            return issuer_==&root&&video_==video&&!video_.owner_before(video)&&!video.owner_before(video_)&&
                construction_==&construction&&frontend_==frontend&&manager_==manager&&effectCurrent()&&
                construction.current(frontend,manager);
        }
        bool abortNative()noexcept {
            if(!acquired_||nativeAborted_)return true;
            if(!abortCurrent())return false;
            // Exclusive producer custody, NOT equality with1, proves which
            // write is being undone. Restore only actual observed Video44.
            video_->sourceMode44_=before_;
            video_->courseModeEffect_=nullptr;
            construction_->videoEffect_=nullptr;
            construction_->phase_=Construction::Phase::nativeVideoAborted;
            nativeAborted_=true;return true;
        }
    public:
        CourseModeEffect(const CourseModeEffect&)=delete;
        CourseModeEffect& operator=(const CourseModeEffect&)=delete;
        ~CourseModeEffect()noexcept {
            // Even terminal effects release retained native Video/Scope pins.
            if(std::this_thread::get_id()!=thread_)std::terminate();
            if(pending()&&!abortNative())std::terminate();
        }
    };
    bool acquireCourseMode2A3A90(const Application& root,const std::shared_ptr<Video>& retained,
        rr::game::OriginalUiEntityScope50KG::CourseConstruction& construction,
        const rr::platform::OriginalUiFrontend50KG* frontend,const void* manager,
        std::unique_ptr<CourseModeEffect>& output)noexcept;
    class OptionsModeEffect final {
        friend struct Video;
        friend class rr::platform::OriginalUiFrontend50KG;
        using Construction=rr::game::OriginalUiEntityScope50KG::OptionsConstruction;
        std::shared_ptr<Video> video_;
        std::shared_ptr<rr::game::OriginalUiEntityScope50KG> scope_;
        Construction* construction_=nullptr;
        const Application* issuer_=nullptr;
        const rr::platform::OriginalUiFrontend50KG* frontend_=nullptr;
        const void* manager_=nullptr;
        std::thread::id thread_;
        std::uint32_t before_=0;
        bool acquired_=false,nativeAborted_=false;
        OptionsModeEffect(std::shared_ptr<Video> video,Construction& construction,
            const Application& issuer,const rr::platform::OriginalUiFrontend50KG* frontend,
            const void* manager)noexcept
            :video_(std::move(video)),scope_(construction.scope_),construction_(&construction),issuer_(&issuer),
             frontend_(frontend),manager_(manager),thread_(std::this_thread::get_id()){}
        bool pending()const noexcept {return acquired_&&!nativeAborted_;}
        bool effectCurrent()const noexcept {
            // No issuer dereference or live host/clock requirement on native
            // cancellation. The exact retained Video and registration survive.
            return pending()&&std::this_thread::get_id()==thread_&&video_&&construction_&&
                scope_==construction_->scope_&&video_->optionsModeEffect_==this&&construction_->videoEffect_==this&&
                construction_->videoOwnedPhase()&&
                construction_->frontend_==frontend_&&construction_->managerOwner_==manager_&&
                construction_->registrationCurrent();
        }
        bool abortCurrent()const noexcept {
            return effectCurrent()&&!construction_->cameraAcquired_;
        }
        bool current(const Application& root,const std::shared_ptr<Video>& video,
            Construction& construction,const rr::platform::OriginalUiFrontend50KG* frontend,
            const void* manager)const noexcept {
            return issuer_==&root&&video_==video&&!video_.owner_before(video)&&!video.owner_before(video_)&&
                construction_==&construction&&frontend_==frontend&&manager_==manager&&effectCurrent()&&
                construction.current(frontend,manager);
        }
        bool abortNative()noexcept {
            if(!acquired_||nativeAborted_)return true;
            if(!abortCurrent())return false;
            // Exclusive producer custody, NOT equality with1, proves which
            // write is being undone. Restore only actual observed Video44.
            video_->sourceMode44_=before_;
            video_->optionsModeEffect_=nullptr;
            construction_->videoEffect_=nullptr;
            construction_->phase_=Construction::Phase::nativeVideoAborted;
            nativeAborted_=true;return true;
        }
    public:
        OptionsModeEffect(const OptionsModeEffect&)=delete;
        OptionsModeEffect& operator=(const OptionsModeEffect&)=delete;
        ~OptionsModeEffect()noexcept {
            // Even terminal effects release retained native Video/Scope pins.
            if(std::this_thread::get_id()!=thread_)std::terminate();
            if(pending()&&!abortNative())std::terminate();
        }
    };
    bool acquireOptionsMode2A3A90(const Application& root,const std::shared_ptr<Video>& retained,
        rr::game::OriginalUiEntityScope50KG::OptionsConstruction& construction,
        const rr::platform::OriginalUiFrontend50KG* frontend,const void* manager,
        std::unique_ptr<OptionsModeEffect>& output)noexcept;
    rr::game::OriginalApplicationAnimationClockView50KG applicationAnimation_;
    explicit Video(bool main,rr::game::OriginalApplicationAnimationClockView50KG animation,
        const Application& application)
        :originalMain(main),height(main?448:480),sourcePreFlip18_(*this,application),
         applicationAnimation_(std::move(animation)){}
    static bool materialConstructionPinLive(const Video& owner) noexcept {
        return owner.originalMain&&owner.width==640&&owner.height==448&&
            owner.colorBits==32&&owner.packetZMaximum==0xffffff&&
            owner.zBufferFormat==0x31&&owner.host.created()&&owner.host.current();
    }
public:
    // Canonical CPU construction capability. The bounded TA source-provider
    // join calls this only after native Video ownership is actually live.
    // The context pins this same actual host owner and the application's
    // authentic APPSTART/templates and distinct texture lifetime; no
    // shared_ptr<void> or numeric video
    // fields supplied by another caller can mint an equivalent capability.
    std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA>
    standardMaterialConstructionContext(const std::shared_ptr<Video>& owner,
        const Application& application) const;
    rr::platform::OriginalUiMainVideoOwner50KG publication(
        const std::shared_ptr<Video>& owner) const;
};

inline bool Application::bindSourceInput04NativeTransport()const noexcept {
    if(sourcePadHostLease_||!sourceInput04_||!enabled||!videoOwner_||
       sourceInput04_->pc_!=0x2374bc||sourceInput04_->completedControllers_!=8||sourceInput04_->nativeClosed_)return false;
    //No caller GameHost parameter: borrow ONLY this App's already-published
    //canonical Video owner. Binding never invokes videoForSession/factory.
    const auto retained=videoOwner_;
    auto lease=retained->host.bindSourcePadNative(*this,*sourceInput04_);
    if(!lease)return false;
    lease->video_=retained;sourcePadHostLease_=std::move(lease);return true;
    //Native transport contract only; source313000/237430 remains pending.
}


inline bool Application::sourceInput04NativeBorrowCurrent(
    const rr::game::OriginalApplicationInput04Source50LA* input)const noexcept {
    const auto& lease=sourcePadHostLease_;
    return enabled&&input&&sourceInput04_.get()==input&&!input->nativeClosed_&&
        input->application_==this&&input->thread_==std::this_thread::get_id()&&
        input->startupReturnedWithNativeTransport_&&lease&&lease->live_&&
        lease->thread_==std::this_thread::get_id()&&lease->app_==this&&lease->source_==input&&
        videoOwner_&&lease->video_==videoOwner_&&lease->host_==&videoOwner_->host&&
        lease->window_&&lease->state_&&lease->nativeCallsReturned_==8;
    // No connected-device requirement. Revoked Window/GameHost epoch is
    // nevertheless unusable for issuing any NEW logical-controller borrower.
}

inline bool Application::continueSourceInput04NativeActivation()const noexcept {
    if(!enabled||!sourceInput04_||sourceInput04_->nativeClosed_||
       sourceInput04_->application_!=this||sourceInput04_->thread_!=std::this_thread::get_id()||
       sourceInput04_->pc_!=0x2374bc||sourceInput04_->completedControllers_!=8)return false;
    // Disabled/headless/no-Video backends remain pending. Never turn on
    // gameControllers or open devices to satisfy this source continuation.
    if(!sourcePadHostLease_&&!bindSourceInput04NativeTransport())return false;
    const auto& lease=sourcePadHostLease_;
    if(!lease||!lease->live_||lease->thread_!=std::this_thread::get_id()||
       lease->app_!=this||lease->source_!=sourceInput04_.get()||!videoOwner_||
       lease->video_!=videoOwner_||lease->host_!=&videoOwner_->host||!lease->window_||!lease->state_||
       lease->nativeCallsReturned_!=0)return false;
    static constexpr std::uint32_t calls[8]={0x2374bc,0x2374d0,0x2374e4,0x2374f8,
        0x23750c,0x237520,0x237534,0x237548};
    for(unsigned i=0;i<8;++i){sourceInput04_->pc_=calls[i];
        if(!lease->returnNativeOpen(i,calls[i]))return false;}
    sourceInput04_->pc_=0x237550;
    return sourceInput04_->finishNativePadActivation(*this);
    // Conditional adapted 13FD40 return ONLY. No outer App startup/Race
    // admission, Player FF0 borrow, sampled-input bridge or gameplay claim.
}

#include "original_application_startup17c910_source50la.inl"

struct Frontend : rr::platform::OriginalUiRiderPresentationHost50KG {
private:
    bool nativeResultsHallEntry_=false;
    bool nativePauseMainEntry_=false,nativePauseCourseEntry_=false;
    // 1AB5BC request receiver: first screen/page of this lifetime (0 = none).
    std::uint8_t nativeRaceReturnScreen_=0,nativeRaceReturnPage_=0;
    using UploadLifetime=rr::assets::OriginalNativeFrontendUploadLifetime50LA;
    std::shared_ptr<UploadLifetime> uploadLifetime_;
    friend struct Application;
    friend class rr::platform::OriginalUiGarageDrawContribution50KG;
    // Exact native prefix/return custody for THIS prepared timing publication.
    // This is not a source tick flag or a claim that the native coordinator
    // executes every original dispatcher descendant. Failures retain the prefix.
    enum class NativeRootAudioPhase : std::uint8_t { absent,entered,awaitingDispatcher,returned,failed };
    NativeRootAudioPhase nativeRootAudioPhase_=NativeRootAudioPhase::absent;
    original_ui_source_timing50LH::PublicationView nativeRootAudioTiming_;
    std::uint64_t nativeRootAudioPublication_=0,nativeRootAudioExpectedUpdate_=0;
    std::uint32_t nativeRootAudioSourcePC_=0;
    // Immutable catalog resources only. They own no Entity/material/GPU/audio
    // references and can be released normally even when the destination is
    // unavailable. A failed native resource attempt is retained, not replayed.
    struct GarageCareerResources {
        bool attempted=false;
        std::shared_ptr<const rr::assets::OriginalUiGarageSceneAssets50KG> assets;
        rr::platform::OriginalUiGarageDestinationDiagnostic50KG failure{};
    };
    std::array<GarageCareerResources,27> garageCareerResources_{};
    bool garageDestinationAssetsAttempted_=false;
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> garageDestinationArchive_,garageDestinationPod19_;
    std::shared_ptr<const rr::assets::OriginalUiGarageSceneAssets50KG> garageDestinationAssets_;
    rr::platform::OriginalUiGarageDestinationDiagnostic50KG garageDestinationFailure_{};
    using GarageTransferGrant=rr::game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    using GarageTransferDiagnostic=rr::platform::OriginalUiFrontendDiagnostic50KG;
    using GarageTransferBinding=rr::assets::OriginalNativeTextureUploadBinding50LA;
    enum class GarageTransferPhase : std::uint8_t {
        absent,entryDrainAttempted,entryDrained,ready,exitDrainAttempted,exitDrained,retired,failed,failedEntryRetired
    };
    GarageTransferPhase garageTransferPhase_=GarageTransferPhase::absent;
    std::shared_ptr<GarageTransferGrant> garageTransferGrant_;
    std::weak_ptr<RaceDisplayLifetime50LA> garageTransferDisplay_;
    std::shared_ptr<const GarageTransferBinding> garageTransferBefore_,garageTransferActive_,garageTransferAfter_;
    const Application* garageTransferApplication_=nullptr;
    const rr::game::OriginalWreckComponent50LA* garageTransferManager_=nullptr;
    std::uint64_t garageTransferGeneration_=0,garageTransferEpoch_=0;
    GarageTransferDiagnostic garageTransferFailure_{},garageTransferEntryDrain_{},garageTransferExitDrain_{};
    //Exact native OriginalCameraStartStatus values;255 means no call occurred.
    std::uint8_t garageTransferEntryResetStatus_=255,garageTransferExitResetStatus_=255;
    bool prepareGarageTransfers2542F0(const std::shared_ptr<GarageTransferGrant>&,
        GarageTransferDiagnostic&)noexcept;
    bool retireGarageTransfers2543F0(const std::shared_ptr<GarageTransferGrant>&,
        GarageTransferDiagnostic&)noexcept;
    bool retireFailedSharedPageTransfers(const std::shared_ptr<rr::platform::OriginalUiGarageSession50KG>&,
        GarageTransferDiagnostic&)noexcept;
    bool retireFailedWattoBindingTransfers(const std::shared_ptr<rr::platform::OriginalUiGarageSession50KG>&,
        GarageTransferDiagnostic&)noexcept;
    bool retireFailedLoadedHideTransfers(const std::shared_ptr<rr::platform::OriginalUiGarageSession50KG>&,
        GarageTransferDiagnostic&)noexcept;
    bool retireFailedLateBorrowTransfers(const std::shared_ptr<rr::platform::OriginalUiGarageSession50KG>&,
        GarageTransferDiagnostic&)noexcept;
    bool retireFailedPartialSceneTransfers(const std::shared_ptr<rr::platform::OriginalUiGarageSession50KG>&,
        GarageTransferDiagnostic&)noexcept;
    bool retireGarageTransfersAtPayload(const std::shared_ptr<GarageTransferGrant>&,
        const rr::platform::OriginalUiGarageSession50KG*,GarageTransferDiagnostic&,bool failedPartial=false,bool failedLateBorrow=false,bool failedLoadedHide=false,bool failedWattoBinding=false)noexcept;
    bool retireFailedGarageEntryTransfers(const std::shared_ptr<GarageTransferGrant>&,
        GarageTransferDiagnostic&)noexcept;
    bool garageTransferOwnerCurrent(const std::shared_ptr<GarageTransferGrant>&)const noexcept;
    bool garageTransferIdentityCurrent(const std::shared_ptr<GarageTransferGrant>&)const noexcept;

    static bool uploadLifetimeLive(const UploadLifetime& token)noexcept;
public:

    const Application* materialApplicationOwner=nullptr;
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> languageArchive,fontArchive;
    using Status=rr::platform::OriginalUiFrontendStatus50KG;
    using StageClock=std::chrono::steady_clock;
    // Host observation only. These samples never feed a source/accepted clock,
    // cold-time exclusion, frame delta or the unchanged one-second stall gate.
    const bool traceStages=[](){const char* v=std::getenv("RR_TRACE_ORIGINAL_UI_STAGES");
        return v&&v[0]=='1'&&v[1]=='\0';}();
    struct StageObservation {
        const char* name=nullptr;
        StageClock::time_point begin{},end{};
        std::uint64_t frame=0,sequence=0;
        std::uint8_t screen=0,page=0;
        bool okay=false;
    };
    // Fixed bounded history is always retained, without per-frame output.
    // Observations can overlap (e.g. source_update contains bank publication)
    // and must NEVER be summed or credited as cold-time authority.
    mutable std::array<StageObservation,24> stageHistory{};
    mutable std::size_t nextStageObservation=0,stageObservationCount=0;
    StageClock::time_point stageStart() const noexcept {return StageClock::now();}
    void reportStageRange(const char* stage,StageClock::time_point start,StageClock::time_point end,bool okay=true,
        const rr::platform::VehicleWindowDiagnostic50DS* renderer=nullptr) const;
    void reportStage(const char* stage,StageClock::time_point start,bool okay=true,
        const rr::platform::VehicleWindowDiagnostic50DS* renderer=nullptr) const;
    void accountColdInterval(const rr::platform::OriginalUiFrontendColdInterval50KG& interval);
    void accountBankPublication(const rr::platform::OriginalUiFrontendColdInterval50KG& interval,
        const rr::platform::OriginalUiFrontendWorkObservation50KG& drain,bool okay);
    rr::platform::GameHost50ER* presentationHost=nullptr;
    std::uint64_t (*presentationNow)()noexcept=nullptr;
    enum class RiderPresentationPolicy : std::uint8_t {inactive,restoring,driverSwapInterval1,nativePcDeadline60Hz};
    RiderPresentationPolicy riderPresentationPolicy=RiderPresentationPolicy::inactive;
    bool presentationActive=false,allowNativePcRiderDeadline=false;
    rr::game::HostMonotonicClock50ID* presentationClock=nullptr;
    std::int32_t savedSwapInterval=0,presentationWidth=0,presentationHeight=0;
    std::uint64_t riderPresentationSwaps=0,presentationBeginSwaps=0;
    std::uint64_t presentationCompletedWaits=0,presentationCompletedWaitNanoseconds=0;
    original_ui_rider_pacing50LB::Deadline riderDeadline;
    const char* presentationPolicyName()const noexcept;
    bool available()const noexcept override;
    bool begin()noexcept override;
    bool end()noexcept override;
    bool pollPresentationInput()noexcept;
    bool waitBeforeSample()noexcept override;
    bool materialClock(rr::platform::OriginalUiMainFrameClock50KG& out)noexcept override;
    bool checkpointAudio()noexcept override;
    bool present(rr::platform::VehicleWindowGl50DS& renderer,
        const rr::platform::OriginalUiPrivatePublication50KP& image,
        rr::platform::VehicleWindowDiagnostic50DS& d)noexcept override;
    AudioOutput audioOutput;
    rr::platform::OriginalUiFrontend50KG coordinator;
    rr::platform::OriginalUiFrontendDiagnostic50KG diagnostic{};
    std::uint64_t lastUpdateNanoseconds=0,inputCount=0,unavailableCount=0,frameCount=0;
    original_ui_source_timing50LH::Owner sourceDispatchTiming;
    original_ui_source_timing50LH::Sample sourceDispatchSample;
    std::uint64_t sourceDispatchActiveNanoseconds=0;
    std::uint64_t captureSequence=0,lastAudioSequence=0;
    std::string sessionWavPath;
    // Actual accepted renderer receipt; diagnostic is reused by later calls.
    std::uint8_t submittedScreen=0;
    std::uint64_t submittedPublication=0,sourceFrameGeneration=0,sourceTargetGeneration=0;
    std::uint64_t submittedGaragePublication=0; // Actual completed Root target, never a GPU token.
    std::uint32_t submittedDraws=0,submittedQuads=0,submittedTriangles=0;
    std::uint32_t submittedTextureBarriers=0;
    bool submittedCoherentFramebufferFetch=false;
    std::uint64_t lastSourceNanoseconds=0,activeSourceNanoseconds=0;
    std::uint64_t pendingUpdateColdNanoseconds=0,pendingMaterialColdNanoseconds=0;
    std::uint32_t sourceMilliseconds=0;
    bool sourceSampleKnown=false,previousSourceActive=false;
    bool imagePublished=false;
    bool hostPaused()const noexcept {
        return materialApplicationOwner&&materialApplicationOwner->animationPaused();
    }
    void setHostPaused(bool paused) {
        require(materialApplicationOwner,"explicit UI pause requires application owner");
        materialApplicationOwner->setExplicitAnimationPause(paused);
    }
    rr::platform::OriginalUiMainFrameClock50KG sampleClock(
        rr::game::HostMonotonicClock50ID& applicationClock,bool active);
    void prepare(const Application& application,const std::shared_ptr<Video>& video,
        const rr::platform::MaterialTextureGlRequest50EB& textureRequest,
        rr::platform::VehicleWindowRequest50DS& rendererRequest,
        std::shared_ptr<const rr::platform::OriginalUiCourseRaceDestination50KG> raceDestination={},
        std::uint64_t (*nativeNow)()noexcept=nullptr,std::uint64_t nativeSessionIndex=0);
    void showBetaSettingsStatus() const noexcept;
    void activate(rr::platform::VehicleWindowGl50DS& renderer);
    std::string garageRuntimeFailureText() const;
    [[noreturn]] void fail(const char* operation) const;
    bool nativeBetaLoadPending=false;
    Status input(std::uint8_t sourceInput,rr::platform::WindowControllerPresence50DQ presence=rr::platform::WindowControllerPresence50DQ::Unknown,
        std::uint8_t controller=0);
    void reportElapsedFailure(const original_ui_elapsed50LH::Interval& interval,std::uint64_t before,
        std::uint64_t now,std::uint64_t pending,StageClock::time_point observedNow) const;
    std::uint64_t acceptedElapsed(std::uint64_t now);
    float preparedSourceUpdateSeconds() const noexcept {return sourceDispatchSample.updateSeconds;}
    void commitSourceUpdateTiming() {
        require(sourceDispatchTiming.commit(),"UI source dispatch timing commit rejected");
    }
    rr::platform::OriginalUiFrontendUpdateReceipt50KG update(
        const rr::platform::OriginalUiMainSessionUpdate50KG& in,std::uint64_t elapsedNanoseconds);
    void render(const rr::platform::OriginalUiMainFrameClock50KG& clock,
        rr::platform::VehicleWindowGl50DS& renderer,std::int32_t width,std::int32_t height);
    void commitDeferred(rr::platform::VehicleWindowGl50DS& renderer);
    void requestQuit() noexcept {coordinator.requestQuit();}
    // Actual taken Course route only. This bounded source prefix never
    //publishes a native cold-load request or claims loading/Screen completion.
    bool advanceCourseRaLoadingPrefix50LA(std::uint32_t& sourcePC)noexcept;
    bool closeUi() noexcept;
    bool closeAudio() noexcept;
    const rr::game::OriginalUiNavigation50KG* navigation() const noexcept;
    void report(const char* reason,const Application& application,std::uint64_t worldTicks,std::uint64_t mixedFrames) const;
};
}
