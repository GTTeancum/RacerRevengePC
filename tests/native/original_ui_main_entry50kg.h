#pragma once
#include "../../src/game/original_camera_start_effects.h"
#include "../../platform/linux/native/original_application_aural50kg.h"
namespace original_pause_observation50LA { class Owner; }
namespace original_course_targets50KX { class ConsumedLaunch; }
namespace ra_cold_aggregate50LA { class Owner; }
namespace ra_cold_sources50LA { class ColdNavigationLease; }
namespace rr::game { class OriginalRacerModelRenderGlobals50LA; class OriginalRaceConstructor50LA; class OriginalRaceTuning50LA; class OriginalTunnelTrack50LA; enum class OriginalTunnelTrackRole50LA : unsigned; class OriginalPauseStorage50LA; class OriginalPositionStatsPrimitiveOwner50LA; class OriginalPositionStatsResources50LA;
struct OriginalPositionStatsNumbers50LA; struct OriginalPositionStatsResourceDiagnostic50LA; }
namespace original_camera_session50KX { class Session; }
namespace rr::assets {class OriginalUiGarageSceneAssets50KG; struct OriginalUiGaragePageData50KG;}
namespace rr::platform {class OriginalUiSharedDialogs50KG; class OriginalUiGarageSharedPageDrawOwner50KG; class OriginalUiGarageSession50KG; struct OriginalUiGarageNativeInput50KG; struct OriginalUiGarageNativeState50KG; class OriginalUiGarageModelResources50KG; class OriginalUiGarageVisibility50KG; class OriginalUiGarageTextOverlay50KG; class OriginalUiGarageFogColorLease50KG; class OriginalUiGarageScreenText50KG; struct OriginalUiGarageScreenTextDiagnostic50KG; class OriginalUiGarageWidget1Owner50KG; struct OriginalUiGarageWidget1Diagnostic50KG; class OriginalUiGarageManagerFog50KG;}
namespace native_ai50IV { struct Session; }
#include "original_ui_main_output50kg.h"
#include "../../platform/linux/native/original_ui_garage_audio_backing50kg.h"
#include "original_ui_elapsed50lh.h"
#include "original_ui_source_timing50lh.h"
#include <optional>
#include "original_ui_rider_pacing50lb.h"
#include "../../platform/linux/native/original_ui_frontend50kg.h"
#include "../../src/assets/original_standard_texture_transfer_cold50la.h"
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
#include "../../src/assets/original_archive_staged_reader50la.h"
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
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

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
inline std::shared_ptr<const rr::assets::OriginalUiArchive50KG> readArchive(
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
class RaceDisplayLifetime50LA;
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
    friend class RaceDisplayLifetime50LA;
    friend class rr::platform::OriginalUiLightManagerOwner50KG;
    friend class rr::assets::OriginalApplicationLocalization50KG;
    friend class rr::platform::OriginalLocalizationRendererLease50KG;
    friend class rr::game::OriginalVideoPreFlipList50KG;
    friend class rr::platform::OriginalUiGarageFogColorLease50KG;
    friend class rr::platform::OriginalUiGarageTextOverlay50KG;
    friend class rr::platform::OriginalUiGaragePageOwner50KG;
    friend class rr::platform::OriginalUiGarageSession50KG;
    friend class rr::platform::OriginalUiSharedDialogs50KG;
    friend class rr::platform::OriginalUiFrontend50KG;
    bool enabled=false;
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
    friend class ::original_pause_observation50LA::Owner;
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
    friend struct Video;
    friend struct Frontend;
    friend class NativeCollectorFifo50LA;
    // Opaque original process word3E1C10, included in entry100008..30 BSS
    // zeroing. Its gameplay meaning/nonzero producer is not established.
    // Retain actual storage once with Application, never per race/session.
    std::uint32_t sourceProcessWord3E1C10_=0;
    mutable std::int32_t sourceApplicationMode22C_=1; //13FDBC genuine constructor store.
    // Source23C is not constructor-written here. Only the actual140CE4
    // continuation publishes its passed/capped factor, never raw250.
    mutable std::optional<float> sourceApplicationElapsed23C_;
    void clearRaceProcessWord1A6884() noexcept { sourceProcessWord3E1C10_=0; }

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
    // Bounded last-completed dispatcher receipt. This keeps the old address
    // generation distinct while its genuine successor is constructed.
    mutable std::shared_ptr<rr::platform::OriginalUiSharedDialogs50KG> retiredSharedUiDialogs_;
    mutable rr::game::OriginalUiControllerPopupState50KG retiredControllerPopupState_{};
    bool sharedUiRetirementReceiptCurrent17C1D8()const noexcept;
    enum class SharedUiBootResult { unavailable,complete,failed };
    SharedUiBootResult prepareSharedDialogsInitial1B7BD0(
        std::shared_ptr<rr::platform::OriginalUiControllerPopupResources50KG>,
        std::shared_ptr<const rr::assets::OriginalUiArchive50KG>,
        rr::platform::VehicleWindowGl50DS&,std::string&)const noexcept;
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
    // Each serial frontend/race still closes its GL resources and host. A later
    // session reopens this SAME host owner only after that cleanup completed.
    // CPU material resources are not GL objects and retain this stable owner;
    // no renderer, texture name or GPU readiness survives session teardown.
    mutable std::shared_ptr<Video> videoOwner_;
    // Retain the exact construction capability, not just equivalent fields.
    // Its dependencies pin Video/templates/runtime/list, never Application,
    // so these two application-owned handles do not form a reference cycle.
    mutable std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA> materialContextOwner_;
    mutable std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> standardMaterialDomainOwner_;
    // Native identity of each actual detached Tournament construction attempt.
    // No source clock/profile/result meaning. Never reset on page return.
    mutable std::uint64_t tournamentPodConstructionGeneration_=0;
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
    //17C710/764/808..814 copies3A85E8's exact particle-directory string.
    const std::string particleDirectory36B530_="particle\\";
    //17C848/84C copies the six original bytes at3A8600 into36B420.
    const std::string fontDirectory36B420_="font\\";
public:
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
    bool sampleAnimationTimestamp(std::uint32_t& out)const noexcept {
        std::uint32_t sampled=0;
        if(!animationClockBackend_||!rr::game::sampleHostTimestamp50ID(*animationClockBackend_,sampled)||
            !publishAnimationTimestamp(*animationClockBackend_,sampled))return false;
        out=sampled;return true;
    }
    // Inactive source boundary APIs: the real outer Application/UI scheduler
    // must call these, not each fixed world tick. No delta/ready flag supplied.
    bool beginUpdateClock140AC0()const noexcept {
        auto& clock=*animationClockOwner_;
        if(!clock.applicationAlive_||clock.updateInProgress_||clock.updatePublication_==std::numeric_limits<std::uint64_t>::max())return false;
        std::uint32_t sampled=0;
        if(!sampleAnimationTimestamp(sampled))return false;
        //140B10..28 calls140D70 when234==0. That reset samples once into
        //both230/234 and this first update therefore computes zero elapsed.
        if(!clock.previous234_)clock.previous234_=sampled;
        clock.elapsed250_=rr::game::OriginalApplicationAnimationClock50KG::delta140BEC(
            clock.value_.current230,clock.previous234_);
        ++clock.updatePublication_;clock.updateInProgress_=true;return true;
    }
    bool commitUpdateClock140D30()const noexcept {
        auto& clock=*animationClockOwner_;
        if(!clock.applicationAlive_||!clock.updateInProgress_)return false;
        //Source reloads230 here AFTER callbacks. Do not commit a cached entry
        //timestamp if an intervening authentic source sampler updated230.
        clock.previous234_=clock.value_.current230;clock.updateInProgress_=false;return true;
    }
    bool resetUpdateClock140D70()const noexcept {
        auto& clock=*animationClockOwner_;
        if(!clock.applicationAlive_||clock.updateInProgress_)return false;
        std::uint32_t sampled=0;
        if(!sampleAnimationTimestamp(sampled))return false;
        clock.previous234_=sampled; //140E38/3C same sampled value into234/230.
        //140D70 does not write250. Preserve it, with no active publication.
        return true;
    }
    bool animationPaused()const noexcept {return animationClockOwner_->value_.pause11C!=0;}
    // Native explicit pause/resume mapping, not17CC64 input-matcher parity.
    // No single-step120 writer is joined here; constructor120 remains zero.
    void setExplicitAnimationPause(bool paused)const noexcept {
        animationClockOwner_->value_.pause11C=paused?1u:0u;
    }
    std::shared_ptr<Video> videoForSession() const;
    std::shared_ptr<RaceDisplayLifetime50LA> raceDisplayForSession50LA() const;
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
    const std::int32_t& applicationMode22C()const noexcept{return sourceApplicationMode22C_;}
    // Exact conditional443848 typed destruction leaf; never an implicit drain.
    bool retireNativeCollector1A6948(rr::assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    // Source-only detached ownership. Definitions are in original_pause_entity50la.h.
    bool prewarmPauseCold50LA(original_pause_cold50LA::Owner&)const noexcept;
    bool loadConsumedPauseCold50LA(rr::game::OriginalRaceConstructor50LA&,original_pause_cold50LA::Owner&)const noexcept;
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
    bool stageArchiveReader(std::shared_ptr<rr::assets::OriginalArchiveStagedReader50LA>& out) const noexcept {
        if(out||!standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())return false;
        const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
        return provider&&provider->issueStagedReader(*this,provider,out);
    }
    // Uncalled actual-Application camera constructor prefix. Stops before2361C0;
    // no page attachment, observer slot, renderer or visibility service exists here.
    rr::game::OriginalFrontendCameraCold50LA::Result stageFrontendCameraCold(
        std::shared_ptr<rr::game::OriginalFrontendCameraCold50LA>& out)const noexcept {
        return rr::game::OriginalFrontendCameraCold50LA::construct(this,entitiesOwner_,out);
    }
    // Partial scalar initialization only, stopped before renderer114A00 entry.
    rr::game::OriginalFrontendCameraCold50LA::Result stageFrontendCameraViewportPrefix1149D4(
        rr::game::OriginalFrontendCameraCold50LA& owner)const noexcept {
        return owner.stageViewportPrefix1149D4(this,entitiesOwner_);
    }
    rr::game::OriginalFrontendCameraCold50LA::Result releaseFrontendCameraCold(
        rr::game::OriginalFrontendCameraCold50LA& owner)const noexcept {
        return owner.release(this,entitiesOwner_);
    }
    // Uncalled Tournament cold-reference proposal. This does not mount archives,
    // construct a new canonical domain/Video, or admit a Tournament page.
    bool stageUiSearchListCold(
        std::shared_ptr<rr::platform::OriginalUiSearchListCold50KG>& out,
        rr::platform::OriginalUiSearchListColdDiagnostic50KG& d)const noexcept {
        using S=rr::platform::OriginalUiSearchListColdStatus50KG;d={};
        if(out||!entitiesOwner_||!entitiesOwner_->application10()){
            d={S::input,0x27E260,false};return false;
        }
        try {
            out=std::shared_ptr<rr::platform::OriginalUiSearchListCold50KG>(
                new rr::platform::OriginalUiSearchListCold50KG(*this,entitiesOwner_));
            return out->construct27E260(this,entitiesOwner_,d);
        }catch(...){d={S::allocation,0x1CCD14,false};return false;}
    }
    bool releaseUiSearchListCold(rr::platform::OriginalUiSearchListCold50KG& owner,
        rr::platform::OriginalUiSearchListColdDiagnostic50KG& d)const noexcept {
        return owner.releaseCold(this,entitiesOwner_,d);
    }
    // Uncalled staged252C70 owner. Never performs2542F0 or publishes13C.
    bool stageTextureTransferCold(
        std::shared_ptr<rr::assets::OriginalStandardTextureTransferCold50LA>& out,
        rr::assets::OriginalStandardTextureTransferColdDiagnostic50LA& d)const noexcept {
        using S=rr::assets::OriginalStandardTextureTransferColdStatus50LA;d={};
        if(out||!textureLifetimeOwner_||!entitiesOwner_||!entitiesOwner_->application10()){
            d={S::input,0x252C70,false};return false;
        }
        try {
            out=std::shared_ptr<rr::assets::OriginalStandardTextureTransferCold50LA>(
                new rr::assets::OriginalStandardTextureTransferCold50LA(*this,textureLifetimeOwner_));
            return out->construct252C70(this,textureLifetimeOwner_,d);
        }catch(...){d={S::allocation,0x2543AC,false};return false;}
    }
    bool releaseTextureTransferCold(rr::assets::OriginalStandardTextureTransferCold50LA& owner,
        rr::assets::OriginalStandardTextureTransferColdDiagnostic50LA& d)const noexcept {
        return owner.releaseCold(this,textureLifetimeOwner_,d);
    }
    bool stageTournamentPodReferencesCold(
        std::shared_ptr<const rr::platform::OriginalUiTournamentCareerResources50KG>,
        std::shared_ptr<rr::platform::OriginalUiTournamentPodReferences50KG>&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool releaseTournamentPodReferencesUnpublished(
        rr::platform::OriginalUiTournamentPodReferences50KG&,
        rr::assets::OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> standardMaterialDomain() const {
        require(materialApplication&&materialApplication->prepared(),
            "canonical domain requires actual application APPSTART owners");
        if(!standardMaterialDomainOwner_){
            auto domain=std::make_shared<rr::assets::OriginalStandardMaterialDomain50LA>();
            rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
            require(domain->prepareFresh(materialApplication,diagnostic),
                "application canonical material/program/texture domain rejected");
            standardMaterialDomainOwner_=std::move(domain);
        }
        return standardMaterialDomainOwner_;
    }
    // Cold factory only. Video must already have minted this application's
    // actual construction context; no caller-provided replacement or ready
    // flag can create a font domain. This does not load fonts or activate HUD.
    std::shared_ptr<rr::assets::OriginalApplicationFonts50LA> fonts328() const {
        require(materialContextOwner_&&materialContextOwner_->live()&&
            materialContextOwner_->materialApplication()==materialApplication,
            "font registry requires actual live application construction context");
        if(!fonts328Owner_){
            const auto domain=standardMaterialDomain();
            fonts328Owner_=std::shared_ptr<rr::assets::OriginalApplicationFonts50LA>(
                new rr::assets::OriginalApplicationFonts50LA(domain,materialContextOwner_));
        }
        return fonts328Owner_;
    }
    // Cold caller only, before renderer publication. The font registry cannot
    // be populated through a detached directory or public texture setter.
    bool loadTimeStatsFonts193A0C(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        rr::assets::OriginalApplicationFontDiagnostic50LA& diagnostic) const {
        return fonts328()->loadTimeStats193A0C(std::move(overlays),
            materialDirectory36B970_,fontDirectory36B420_,diagnostic);
    }
    // Source-only Speed/Boost font pair; no startup caller activates this.
    bool loadSpeedBoostFonts18A99C(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        rr::assets::OriginalApplicationFontDiagnostic50LA& diagnostic) const {
        return fonts328()->loadSpeedBoost18A99C(std::move(overlays),
            materialDirectory36B970_,fontDirectory36B420_,diagnostic);
    }
    //Uncalled PositionStats one-font route through this SAME Application328.
    //Authentic PST/provider corpus must precede190E64 in the future full loader.
    bool loadPositionStatsFont190E64(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        rr::assets::OriginalApplicationFontDiagnostic50LA& diagnostic) const {
        return fonts328()->loadPositionStats190E64(std::move(overlays),
            materialDirectory36B970_,fontDirectory36B420_,diagnostic);
    }
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
        const std::filesystem::path& originalRoot) const {
        require(bool(materialApplication),"application material owner missing");
        if(!materialApplication->prepared()){
            auto archive=readArchive(originalRoot,rr::assets::OriginalUiArchiveId50KG::appStart);
            rr::assets::OriginalUiMaterialDiagnostic50KG diagnostic;
            require(materialApplication->prepareFreshApplication(std::move(archive),diagnostic),
                "application authentic DefaultMT/ScreenMT owners rejected");
        }
        return materialApplication->appStart();
    }
    std::shared_ptr<rr::platform::OriginalUiMainProfile50KG> profile;
    // One fresh application profile owner; destinations only borrow it.
    std::shared_ptr<rr::platform::OriginalUiRiderProfileOwner50KG> riderProfile;
    std::shared_ptr<rr::platform::OriginalUiOptionsProfile50KG> optionsProfile;
    std::shared_ptr<rr::game::OriginalUiGameSettingsProfile50KG> gameSettingsProfile;
    std::shared_ptr<rr::game::ControlPresets49D> controlsProfile;
    std::shared_ptr<rr::platform::OriginalSoundMode50KG> soundMode;
    std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> state;
    //140084/147FF8 application lifetime, shared by every frontend incarnation.
    std::shared_ptr<rr::game::OriginalUiLightManagerLifetime50KG> lightManager;
    Application(const Application&)=delete;
    Application& operator=(const Application&)=delete;
    ~Application() {
        if((state&&state->sharedDialogs)||(sharedUiScopeOwner_&&!sharedUiScopeOwner_->closed_))std::terminate();
        // A live source slot may still own renderer/context and counted state.
        // Application destruction cannot substitute for its checked retirement.
        if(sourceAural00Owner_||sourceRace28COwner_||sourceLightManager426F60Owner_||sourceLocalization32COwner_||localizationRendererCall14C5A0_)std::terminate();
        // Fail closed before invalidating the actual clock. This is not
        // cleanup: failed-prefix consumers and their mixer remain owned.
        if(garageAudioOwnerThread_!=std::this_thread::get_id()||garageAudioBackingOwner_||garageCustodyOwner_)std::terminate();
        if(!rr::game::OriginalVideoPreFlipList50KG::applicationMayRetire(*this))std::terminate();
        // Checked audio retirement must precede lifetime invalidation. This
        // assertion does not clear rows, flush registrations, or drain owners.
        if(!rr::platform::OriginalAudioPendingWaves50KG::applicationMayRetire(*this))std::terminate();
        animationClockOwner_->applicationAlive_=false;
    }
    explicit Application(std::shared_ptr<rr::game::HostMonotonicClock50ID> clock,
        std::shared_ptr<rr::game::ControlPresets49D> controls={})
        :animationClockBackend_(clock),controlsProfile(std::move(controls)) {
        // Retain the one application epoch even on the disabled-Main branch.
        require(bool(animationClockBackend_),"application animation clock owner missing");
        const char* originalRoot=std::getenv("RR_ORIGINAL_ASSET_ROOT");
        require(!originalRoot||*originalRoot,"RR_ORIGINAL_ASSET_ROOT must name a nonempty directory");
        decodeRiderImagesInProcess=originalRoot&&*originalRoot;
        const char* enabledSetting=std::getenv("RR_ORIGINAL_MAIN_MENU");
        require(!enabledSetting || std::string(enabledSetting)=="0" || std::string(enabledSetting)=="1",
            "RR_ORIGINAL_MAIN_MENU must be 0 or 1");
        enabled=enabledSetting && std::string(enabledSetting)=="1";
        if(const char* previews=std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")){
            require(std::string(previews)=="3,7,9,23,24",
                "RR_ORIGINAL_RIDER_PREVIEWS must be exactly 3,7,9,23,24");
            require(enabled,"RR_ORIGINAL_RIDER_PREVIEWS requires RR_ORIGINAL_MAIN_MENU=1");
            requestedPreviewRiders=(1u<<3)|(1u<<7)|(1u<<9)|(1u<<23)|(1u<<24);
        }
        const char* visibleRiderSetting=std::getenv("RR_ORIGINAL_TEST_VISIBLE_RIDER");
        std::uint32_t visibleRiderTest=0;
        if(visibleRiderSetting){
            const std::string value(visibleRiderSetting);
            require(value=="3"||value=="8"||value=="22"||value=="14"||value=="21"||value=="25"||value=="26",
                "RR_ORIGINAL_TEST_VISIBLE_RIDER must be exactly 3, 8, 22, 14, 21, 25 or 26; other gameplay owners are not admitted");
            visibleRiderTest=value=="3"?3u:value=="8"?8u:value=="14"?14u:value=="21"?21u:value=="25"?25u:value=="26"?26u:22u;
            require(enabled&&!std::getenv("RR_ORIGINAL_TEST_PROFILE"),
                "Visible Rider test override requires Main and excludes other test profiles");
            require(decodeRiderImagesInProcess||std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE"),
                "Visible Rider test override requires original-disc Rider decoding or an authenticated pixel cache");
        }
        const char* wanMbSetting=std::getenv("RR_ORIGINAL_TEST_WAN_MB");
        const bool wanMbTest=wanMbSetting!=nullptr;
        if(wanMbTest){
            require(std::string(wanMbSetting)=="1",
                "RR_ORIGINAL_TEST_WAN_MB must be exactly 1 when present");
            require(enabled&&!visibleRiderSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE"),
                "Wan/MB test profile requires Main and excludes other test profiles");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Wan/MB test profile requires the normal human-controlled race");
            require(decodeRiderImagesInProcess||std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE"),
                "Wan/MB test profile requires original-disc Rider decoding or an authenticated pixel cache");
        }
        const char* gasganoScSetting=std::getenv("RR_ORIGINAL_TEST_GASGANO_SC");
        const bool gasganoScTest=gasganoScSetting!=nullptr;
        if(gasganoScTest){
            require(std::string(gasganoScSetting)=="1",
                "RR_ORIGINAL_TEST_GASGANO_SC must be exactly 1 when present");
            require(enabled&&!visibleRiderSetting&&!wanMbSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE")&&
                !std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")&&!std::getenv("RR_UNATTENDED_UI_ROUTE"),
                "Gasgano/SC test profile requires human Main and excludes other test overrides and unattended routes");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Gasgano/SC test profile requires the normal human-controlled race");
            const char* cache=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
            require(decodeRiderImagesInProcess||(cache&&*cache),
                "Gasgano/SC test profile requires original-disc Rider decoding or an authenticated pixel cache");
            // Explicit resource request precedes bind()'s population decision.
            // Distinct SC provenance never impersonates the legacy Gasgano fixture.
            requestedPreviewRiders|=1u<<7;
        }
        const char* tzidikTcSetting=std::getenv("RR_ORIGINAL_TEST_TZIDIK_TC");
        const bool tzidikTcTest=tzidikTcSetting!=nullptr;
        if(tzidikTcTest){
            require(std::string(tzidikTcSetting)=="1",
                "RR_ORIGINAL_TEST_TZIDIK_TC must be exactly 1 when present");
            require(enabled&&!gasganoScSetting&&!visibleRiderSetting&&!wanMbSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE")&&
                !std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")&&!std::getenv("RR_UNATTENDED_UI_ROUTE"),
                "Tzidik/TC test profile requires human Main and excludes other test overrides and unattended routes");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Tzidik/TC test profile requires the normal human-controlled race");
            const char* cache=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
            require(decodeRiderImagesInProcess||(cache&&*cache),
                "Tzidik/TC test profile requires original-disc Rider decoding or an authenticated pixel cache");
            // Explicit resource request precedes bind()'s population decision.
            // Distinct TC provenance never impersonates any earlier test fixture.
            requestedPreviewRiders|=1u<<24;
        }
        const char* scorchRcSetting=std::getenv("RR_ORIGINAL_TEST_SCORCH_RC");
        const bool scorchRcTest=scorchRcSetting!=nullptr;
        if(scorchRcTest){
            require(std::string(scorchRcSetting)=="1",
                "RR_ORIGINAL_TEST_SCORCH_RC must be exactly 1 when present");
            require(enabled&&!tzidikTcSetting&&!gasganoScSetting&&!visibleRiderSetting&&!wanMbSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE")&&
                !std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")&&!std::getenv("RR_UNATTENDED_UI_ROUTE"),
                "Scorch/RC test profile requires human Main and excludes other test overrides and unattended routes");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Scorch/RC test profile requires the normal human-controlled race");
            const char* cache=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
            require(decodeRiderImagesInProcess||(cache&&*cache),
                "Scorch/RC test profile requires original-disc Rider decoding or an authenticated pixel cache");
            // Explicit resource request precedes bind()'s population decision.
            // Distinct RC provenance never impersonates any earlier test fixture.
            requestedPreviewRiders|=1u<<17;
        }
        const char* sebulbaTbSetting=std::getenv("RR_ORIGINAL_TEST_SEBULBA_TB");
        const bool sebulbaTbTest=sebulbaTbSetting!=nullptr;
        if(sebulbaTbTest){
            require(std::string(sebulbaTbSetting)=="1",
                "RR_ORIGINAL_TEST_SEBULBA_TB must be exactly 1 when present");
            require(enabled&&!scorchRcSetting&&!tzidikTcSetting&&!gasganoScSetting&&!visibleRiderSetting&&!wanMbSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE")&&
                !std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")&&!std::getenv("RR_UNATTENDED_UI_ROUTE"),
                "Sebulba/TB test profile requires human Main and excludes other test overrides and unattended routes");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Sebulba/TB test profile requires the normal human-controlled race");
            const char* cache=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
            require(decodeRiderImagesInProcess||(cache&&*cache),
                "Sebulba/TB test profile requires original-disc Rider decoding or an authenticated pixel cache");
            // Explicit resource request precedes bind()'s population decision.
            // Distinct TB provenance never impersonates any earlier test fixture.
            requestedPreviewRiders|=1u<<18;
        }
        const char* kraidGbSetting=std::getenv("RR_ORIGINAL_TEST_KRAID_GB");
        const bool kraidGbTest=kraidGbSetting!=nullptr;
        if(kraidGbTest){
            require(std::string(kraidGbSetting)=="1",
                "RR_ORIGINAL_TEST_KRAID_GB must be exactly 1 when present");
            require(enabled&&!sebulbaTbSetting&&!scorchRcSetting&&!tzidikTcSetting&&!gasganoScSetting&&!visibleRiderSetting&&!wanMbSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE")&&
                !std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")&&!std::getenv("RR_UNATTENDED_UI_ROUTE"),
                "Kraid/GB test profile requires human Main and excludes other test overrides and unattended routes");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Kraid/GB test profile requires the normal human-controlled race");
            const char* cache=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
            require(decodeRiderImagesInProcess||(cache&&*cache),
                "Kraid/GB test profile requires original-disc Rider decoding or an authenticated pixel cache");
            // Explicit resource request precedes bind()'s population decision.
            // Distinct GB provenance never impersonates any earlier test fixture.
            requestedPreviewRiders|=1u<<23;
        }
        const char* youngAnakinMcSetting=std::getenv("RR_ORIGINAL_TEST_YOUNG_ANAKIN_MC");
        const bool youngAnakinMcTest=youngAnakinMcSetting!=nullptr;
        const std::uint32_t mcSelectedRider=visibleRiderTest?visibleRiderTest:1u;
        if(youngAnakinMcTest){
            require(std::string(youngAnakinMcSetting)=="1",
                "RR_ORIGINAL_TEST_YOUNG_ANAKIN_MC must be exactly 1 when present");
            require(enabled&&!kraidGbSetting&&!sebulbaTbSetting&&!scorchRcSetting&&!tzidikTcSetting&&!gasganoScSetting&&(!visibleRiderSetting||mcSelectedRider==14||mcSelectedRider==21||mcSelectedRider==25||mcSelectedRider==26)&&!wanMbSetting&&!std::getenv("RR_ORIGINAL_TEST_PROFILE")&&
                !std::getenv("RR_ORIGINAL_RIDER_PREVIEWS")&&!std::getenv("RR_UNATTENDED_UI_ROUTE"),
                "Young Anakin/MC test profile requires human Main and excludes other test overrides and unattended routes");
            const char* allAi=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
            require(!allAi||std::string(allAi)=="0",
                "Young Anakin/MC test profile requires the normal human-controlled race");
            const char* cache=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
            require(decodeRiderImagesInProcess||(cache&&*cache),
                "Young Anakin/MC test profile requires original-disc Rider decoding or an authenticated pixel cache");
            // Explicit resource request precedes bind()'s population decision.
            // Distinct MC provenance never impersonates any earlier test fixture.
            requestedPreviewRiders|=1u<<mcSelectedRider;
        }
        const char* testProfileSetting=std::getenv("RR_ORIGINAL_TEST_PROFILE");
        require(!testProfileSetting||std::string(testProfileSetting)=="gasgano"||std::string(testProfileSetting)=="mawhonic-ryloth",
            "unsupported explicit test profile");
        const bool gasganoTest=testProfileSetting&&std::string(testProfileSetting)=="gasgano";
        const bool mawhonicRylothTest=testProfileSetting&&std::string(testProfileSetting)=="mawhonic-ryloth";
        if(testProfileSetting){
            const auto exact=[](const char* name,const char* value){
                const auto* setting=std::getenv(name);return setting&&std::string(setting)==value;
            };
            const auto* captureDirectory=std::getenv("RR_WINDOW_CAPTURE_DIR");
            require(enabled&&exact("RR_UNATTENDED_UI_ROUTE",mawhonicRylothTest?"main-mawhonic09-course-ra":"main-gasgano07-course-sa")&&
                exact("RR_WINDOW_CAPTURE_AUTO","1")&&exact("RR_UNATTENDED_CAPTURE","1")&&
                exact("RR_ORIGINAL_AI_ALL_RACERS","1")&&exact("RR_TRACK_FPS_BASELINE","1")&&
                captureDirectory&&*captureDirectory,
                "Explicit test profile requires the explicit complete original-AI window evidence route");
            require(decodeRiderImagesInProcess||std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE"),
                "Explicit test profile requires original-disc Rider decoding or an authenticated pixel cache");
        }
        if(!enabled)return;
        const char* root=std::getenv("RR_ORIGINAL_UI_ROOT");
        require(root && *root,"RR_ORIGINAL_MAIN_MENU requires explicit RR_ORIGINAL_UI_ROOT");
        assetRoot=std::filesystem::path(root);
        const char* audio=std::getenv("RR_ORIGINAL_MAIN_AUDIO");
        require(audio && (std::string(audio)=="muted" || std::string(audio)=="enabled"),
            "Main entry requires explicit RR_ORIGINAL_MAIN_AUDIO=muted or enabled");
        if(std::string(audio)=="enabled"){
            audioMode=rr::platform::OriginalUiMainAudioMode50KG::enabled;
            const char* audioRootSetting=std::getenv("RR_ORIGINAL_MAIN_AUDIO_ROOT");
            require(audioRootSetting && *audioRootSetting,
                "enabled Main audio requires explicit RR_ORIGINAL_MAIN_AUDIO_ROOT pointing to AUDIO");
            audioRoot=std::filesystem::path(audioRootSetting);
        }else{
            require(!std::getenv("RR_AUDIO_WAV"),"muted original Main cannot produce an audio proof recording");
        }
        profile=std::make_shared<rr::platform::OriginalUiMainProfile50KG>();
        optionsProfile=std::make_shared<rr::platform::OriginalUiOptionsProfile50KG>();
        gameSettingsProfile=std::make_shared<rr::game::OriginalUiGameSettingsProfile50KG>();
        soundMode=std::make_shared<rr::platform::OriginalSoundMode50KG>();
        state=std::make_shared<rr::platform::OriginalUiMainApplication50KG>();
        require(bool(clock),"original application source clock owner missing");
        state->clock101A50=clock; // Same retained epoch for startup, materials and Rider voice seed.
        // The last shared owner performs application destruction. A frontend
        // pins this same object until its consumers/cleanup have finished, so
        // even exceptional unwinding cannot destroy it under live borrowers.
        using LightManager=rr::game::OriginalUiLightManagerLifetime50KG;
        lightManager=std::shared_ptr<LightManager>(new LightManager,
            [](LightManager* manager) noexcept {manager->destroyApplication();delete manager;});
        rr::game::OriginalUiLightManagerLifetimeDiagnostic50KG lightDiagnostic;
        require(lightManager->constructAtApplicationStartup(lightDiagnostic),
            "original shared LightManager startup rejected");
        lightManagerStartupWitness_=lightManager; // Exact existing constructor owner, not a new concrete startup call.
        // Fresh native backing starts with no imported unlocks or debug bits.
        // Source1864B0/186EF0 reset only its proven Main fields.
        rr::platform::resetOriginalUiMainProfileFields50KG(*profile);
        rr::platform::resetOriginalUiOptionsProfileFields50KG(*optionsProfile);
        rr::game::resetOriginalUiGameSettingsProfileFields50KG(*gameSettingsProfile);
        // These are explicit fresh process-owned profile fields. Construct and
        // reset once here, never on page entry, Defaults, Cancel or bank switch.
        profile->known=true;optionsProfile->known=true;gameSettingsProfile->known=true;
        //17F970/1870B0: read the authenticated authored ability tables once
        // at explicit new-profile startup, sharing the actual Main profile.
        // No missing-save fallback and no reset on Rider entry or Back.
        const char* cacheRoot=std::getenv("RR_ORIGINAL_RIDER_IPUM_CACHE");
        if(decodeRiderImagesInProcess||cacheRoot){
            if(cacheRoot){
                require(*cacheRoot,"RR_ORIGINAL_RIDER_IPUM_CACHE must name a nonempty directory");
                riderCacheRoot=std::filesystem::path(cacheRoot);
            }
            riderProfile=std::make_shared<rr::platform::OriginalUiRiderProfileOwner50KG>();
            rr::platform::OriginalUiRiderProfileDiagnostic50KG riderProfileDiagnostic;
            const auto physics=readArchive(assetRoot,rr::assets::OriginalUiArchiveId50KG::podPhysics);
            require(youngAnakinMcTest?
                riderProfile->initializeYoungAnakinMcTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic,mcSelectedRider):kraidGbTest?
                riderProfile->initializeKraidGbTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):sebulbaTbTest?
                riderProfile->initializeSebulbaTbTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):scorchRcTest?
                riderProfile->initializeScorchRcTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):tzidikTcTest?
                riderProfile->initializeTzidikTcTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):gasganoScTest?
                riderProfile->initializeGasganoScTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):wanMbTest?
                riderProfile->initializeWanMbTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):visibleRiderTest?
                riderProfile->initializeVisibleRiderTestProfileAtApplicationStartup(profile,physics,visibleRiderTest,riderProfileDiagnostic):mawhonicRylothTest?
                riderProfile->initializeMawhonicRylothTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):gasganoTest?
                riderProfile->initializeGasganoTestProfileAtApplicationStartup(profile,physics,riderProfileDiagnostic):
                riderProfile->initializeAtApplicationStartup(profile,physics,riderProfileDiagnostic),
                "Rider startup profile initialization rejected");
            // Original17DE80/1801D0 fresh-profile record producer. Populate
            // once at APPLICATION startup, never Hall entry or missing-save recovery.
            const auto recordLocalization=readArchive(assetRoot,rr::assets::OriginalUiArchiveId50KG::localization);
            rr::assets::OriginalUiEnglish50KG recordEnglish;
            rr::assets::OriginalUiLocalizationDiagnostic50KG recordLocalizationDiagnostic;
            require(rr::assets::readOriginalUiEnglish50KG(*recordLocalization,recordEnglish,recordLocalizationDiagnostic)&&
                riderProfile->initializeHallRecordsAtApplicationStartup(recordEnglish,riderProfileDiagnostic),
                "Original fresh Hall record initialization rejected");
            require(riderProfile->gasganoTestProfile()==gasganoTest&&
                riderProfile->mawhonicRylothTestProfile()==mawhonicRylothTest&&
                riderProfile->visibleRiderTestProfile()==(youngAnakinMcTest?0u:visibleRiderTest)&&
                riderProfile->wanMbTestProfile()==wanMbTest&&
                riderProfile->gasganoScTestProfile()==gasganoScTest&&
                riderProfile->tzidikTcTestProfile()==tzidikTcTest&&
                riderProfile->scorchRcTestProfile()==scorchRcTest&&
                riderProfile->sebulbaTbTestProfile()==sebulbaTbTest&&
                riderProfile->kraidGbTestProfile()==kraidGbTest&&riderProfile->youngAnakinMcTestProfile()==youngAnakinMcTest,"Rider test profile provenance differs");
            if(youngAnakinMcTest)std::cerr<<"TEST UNLOCK OVERRIDE: MC rider="<<mcSelectedRider<<" initially remembered; original course prefix10 and MC slot10 (course6/planet5). Process-local human testing only; no earned progression or save write. Actual MC gameplay owners remain mandatory.\n";
            if(kraidGbTest)std::cerr<<"TEST UNLOCK OVERRIDE: Kraid23 initially remembered; original course prefix8 and GB slot8 (course9/planet1). Process-local human testing only; no earned progression or save write. GB gameplay capability remains separately gated.\n";
            if(sebulbaTbTest)std::cerr<<"TEST UNLOCK OVERRIDE: Sebulba18 initially remembered; original course prefix6 and TB slot6 (course1/planet0). Process-local human testing only; no earned progression or save write. TB gameplay capability remains separately gated.\n";
            if(scorchRcTest)std::cerr<<"TEST UNLOCK OVERRIDE: Scorch17 initially remembered; original course prefix12 and RC slot12 (course3/planet2). Process-local human testing only; no earned progression or save write. RC gameplay capability remains separately gated.\n";
            if(tzidikTcTest)std::cerr<<"TEST UNLOCK OVERRIDE: Tzidik24 initially remembered; original course prefix13 and TC slot13 (course2/planet0). Process-local human testing only; no earned progression or save write. TC gameplay capability remains separately gated.\n";
            if(gasganoScTest)std::cerr<<"TEST UNLOCK OVERRIDE: Gasgano7 initially remembered; original course prefix9 and SC slot9 (course12/planet4). "
                "Process-local human testing only; no earned progression or save write. SC gameplay capability remains separately gated.\n";
            if(wanMbTest)std::cerr<<"TEST UNLOCK OVERRIDE: Wan16 initially remembered; original course prefix7 and MB slot7. "
                "Process-local testing only; no earned progression or save write. Real Wan/MB cold-loader checks remain mandatory.\n";
            if(visibleRiderTest&&!youngAnakinMcTest)std::cerr<<"TEST UNLOCK OVERRIDE: rider="<<visibleRiderTest
                <<" available and initially remembered for this process only. Native test patch, not retail cheat execution; "
                "no earned unlock, career progress, saved-profile import or save write. All gameplay loader gates remain active.\n";
            if(mawhonicRylothTest)std::cerr<<"TEST PROFILE: Mawhonic9 available and initially remembered; original course prefix availability5 for Brightlands, process-local only. "
                "No earned unlock, qualification, career progress or save write is claimed. RA race admission remains subject to genuine runtime readiness.\n";
            if(gasganoTest)std::cerr<<"TEST PROFILE: Gasgano available and initially remembered for this process only; "
                "no earned unlock, qualification, saved-profile import or save write is claimed.\n";
        }
        require(rr::game::validOriginalUiGameSettingsProfile50KG(*gameSettingsProfile),
            "fresh original Game Options profile rejected");
        require(soundMode->initializeFromProfile1101C(gameSettingsProfile->soundMode1101C),
            "fresh original sound mode initialization rejected");
        // One native startup seed from the existing process clock, never per
        // frame, capture or focus change. Exact original boot seeding unclaimed.
        require(rr::game::sampleHostTimestamp50ID(*state->clock101A50,state->rawRandom308030),
            "Main startup shared application timestamp unavailable");
        state->known=true;
        std::cerr<<"ORIGINAL MAIN: "<<(youngAnakinMcTest?"explicit Young Anakin/MC human test profile":kraidGbTest?"explicit Kraid/GB human test profile":sebulbaTbTest?"explicit Sebulba/TB human test profile":scorchRcTest?"explicit Scorch/RC human test profile":tzidikTcTest?"explicit Tzidik/TC human test profile":gasganoScTest?"explicit Gasgano/SC human test profile":wanMbTest?"explicit Wan/MB test unlock profile":visibleRiderTest?"explicit visible-Rider test unlock profile":mawhonicRylothTest?"explicit Mawhonic/Ryloth test profile":gasganoTest?"explicit Gasgano test profile":"fresh native profile")
            <<"/application ownership; "
            "one clock-derived native RNG seed="<<state->rawRandom308030
            <<"; Main audio="<<audio<<". Main, Options menu and Game Options routing is wired; "
            "Game Options Accept requires enabled concrete audio and commits process-owned settings. "
            "Prepared default-unlocked Riders 2/4/10/11/12/15/19/20 use their authenticated original resources and image caches; enabled audio uses original Song7. "
            "Mode0 Rider Confirm to Course and Back supports explicit mute or the original Confirm voice/Song11 owner. Course Left/Right selects the four fresh unlocked tracks; "
            "Controls uses original presets and its animated controller: Left/Right chooses, Tab changes Normal/Advanced, R rotates, Enter/Space accepts and Escape cancels. Accept applies only controller0; Cancel preserves the saved preset. "
            "Second-controller Controls remains unavailable until native menu-controller/hotplug and the original missing-controller popup are joined. Gallery, Memory Card and Credits remain unavailable. "
            "Course admits bounded TA human driving with one actual Player and seven original AI, or authenticated TA/SA/MA/GA ALL8 driving, using prepared Riders2/4/10/11/12/15/19/20. Human selections always use the typed whole-world cold-load boundary, including unchanged selections. Other human tracks remain unavailable. No original boot/save parity is claimed.\n";
    }
};
// Owns the actual host/context and its native source-canvas mode together.
// shared_ptr conversions retain this object and use its real destructor.
// RGBA8/D24 private targets are owned by the actual VehicleWindowGl renderer;
// source Z format0x31 is the admitted original24-bit packet representation.
struct Video {
    friend struct Application;
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
    // The ordinary RA aggregate still owns its separate recovered effect
    // continuation. Do not pretend its writes transferred back into these
    // same-Video frontend fields after a real race display use.
    void beginRaceDisplay1A7658() noexcept { sourceMode44_=1;sourceFrontendEndEffectsKnown_=false; }
    struct EndView25B110 {
        std::uint32_t overlay40C=0; //25A1E8, four real slots.
        std::uint8_t overlay494=0; //25A250.
        std::uint8_t covered2F4=0,increasing2F8=0,decreasing2FC=0,completed300=0;
        float lastMilliseconds3C4=0; //25A098.
    };
    std::array<EndView25B110,4> sourceEndViews_{};
    rr::game::OriginalVideoFadeState sourceEndFade_; //Actual259E00 initialized fields;3E8 remains unknown.
    bool sourceFrontendEndEffectsKnown_=true;

    void clearRaceDisplay1A6870() noexcept { sourceMode44_=0; }
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
        const Application& application) const {
        require(owner.get()==this&&application.videoOwner_.get()==this&&
            materialConstructionPinLive(*this),
            "standard material construction requires the actual current original video owner");
        require(application.materialRuntimeOwner_&&
            &application.materialRuntime==application.materialRuntimeOwner_.get()&&
            application.materialRuntime.overrideKnown&&
            !application.materialRuntime.override435670&&
            application.materialApplication&&application.materialApplication->prepared()&&
            application.textureLifetimeOwner_,
            "standard material construction requires actual application templates, runtime and texture lifetime");
        using Context=rr::assets::OriginalStandardMaterialContext50LA;
        if(application.materialContextOwner_){
            const auto& context=application.materialContextOwner_;
            require(context->videoIdentity()==this&&context->live()&&
                context->materialApplication().get()==application.materialApplication.get(),
                "standard material construction cannot replace its application ownership domain");
            return context;
        }
        // Raw identity alone cannot prove that a caller's shared_ptr owns its
        // pointee. Retain the application's authoritative control block.
        application.materialContextOwner_=std::shared_ptr<const Context>(new Context(application.videoOwner_,
            application.materialApplication,application.materialRuntimeOwner_,
            application.textureLifetimeOwner_,&Video::materialConstructionPinLive,applicationAnimation_));
        return application.materialContextOwner_;
    }
    rr::platform::OriginalUiMainVideoOwner50KG publication(
        const std::shared_ptr<Video>& owner) const {
        require(owner.get()==this && originalMain && host.created() && host.current(),
            "Main video publication requires its actual current native owner");
        return {owner,width,height,colorBits,packetZMaximum,zBufferFormat};
    }
};
inline std::shared_ptr<Video> Application::videoForSession() const {
    require(rr::game::OriginalVideoPreFlipList50KG::applicationVideoEntryCurrent(*this),
        "Video factory requires its live Application owner thread");
    require(raceDisplayOwner50LA_.expired(),"previous race display lifetime still retained");
    if(!videoOwner_){
        videoOwner_=std::shared_ptr<Video>(new Video(enabled,animationClock(),*this));
        videoOwner_->sourcePreFlip18_.bindPublishedVideo(videoOwner_);
    }
    require(videoOwner_->originalMain==enabled,
        "native application cannot replace its original/direct video mode");
    require(!videoOwner_->host.created()&&!videoOwner_->host.ownsResources(),
        "native session requires completed outgoing video/host cleanup");
    return videoOwner_;
}
inline bool Application::stageTournamentPodReferencesCold(
    std::shared_ptr<const rr::platform::OriginalUiTournamentCareerResources50KG> resource,
    std::shared_ptr<rr::platform::OriginalUiTournamentPodReferences50KG>& out,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    using S=rr::assets::OriginalStandardMaterialStatus50LA;d={};
    const auto reject=[&]()noexcept{d.status=S::phase;d.sourcePC=0x1D3590;return false;};
    if(out||!resource||!resource->current()||!riderProfile||!profile||
       riderProfile->commonProfile()!=profile.get()||!resource->sameProfileOwner(*riderProfile)||
       !videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
       !materialContextOwner_||!materialContextOwner_->live()||
       materialContextOwner_->videoIdentity()!=videoOwner_.get()||
       materialContextOwner_->materialApplication()!=materialApplication||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())return reject();
    std::uint8_t packed=0;std::uint32_t selected=0;
    const auto* career=resource->career();
    if(!career||!riderProfile->riderConfirmSnapshot(packed,selected)||selected!=(packed>>3)||
       career->career100C0!=selected||riderProfile->stamp().mode34!=2)return reject();
    try {
        // Publish owner before acquiring any canonical numeric reference: a
        // failure leaves its actual retained prefix available for cleanup.
        if(tournamentPodConstructionGeneration_==std::numeric_limits<std::uint64_t>::max()){
            d.status=S::overflow;d.sourcePC=0x1D3590;return false;
        }
        out=std::shared_ptr<rr::platform::OriginalUiTournamentPodReferences50KG>(
            new rr::platform::OriginalUiTournamentPodReferences50KG(*this));
        const auto generation=++tournamentPodConstructionGeneration_;
        const bool okay=out->stage(this,std::move(resource),standardMaterialDomainOwner_,
                                  materialContextOwner_,entitiesOwner_,generation,materialDirectory36B970_,d);
        d.sourceEffectsCommitted=d.sourceEffectsCommitted||
            out->constructionEffects();
        return okay;
    }catch(...){d.status=S::allocation;return false;}
}
inline bool Application::releaseTournamentPodReferencesUnpublished(
    rr::platform::OriginalUiTournamentPodReferences50KG& owner,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    return owner.releaseUnpublished(this,d);
}
inline bool Application::stageTimeStatsNativeCold50LA(const std::filesystem::path& originalRoot,
    std::uint64_t actualSessionGeneration,TimeStatsColdResult50LA& out,bool deferPopulation)const noexcept {
    using namespace rr::assets;using Phase=TimeStatsColdResult50LA::Phase;
    // Never erase a prior partial result or replace an outstanding owner.
    if(out.phase!=Phase::fresh||out.owner||out.pixels)return false;
    const auto fail=[&](const char* message)noexcept {
        out.failedAt=out.phase;out.phase=Phase::failed;
        out.sourceEffectsCommitted=out.sourceEffectsCommitted||out.object.sourceEffectsCommitted||
            out.font.sourceEffectsCommitted||out.material.sourceEffectsCommitted;
        try{out.failure=message;}catch(...){}
        return false;
    };
    try {
        if(!actualSessionGeneration||!timeStatsColdOwner_.expired()||!entitiesOwner_||
           !entitiesOwner_->application10()||!videoOwner_||
           !Video::materialConstructionPinLive(*videoOwner_)||
           !materialContextOwner_||!materialContextOwner_->live()||
           materialContextOwner_->videoIdentity()!=videoOwner_.get()||
           materialContextOwner_->materialApplication()!=materialApplication||
           !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())
            return fail("TimeStats requires actual live Application cold owners");
        const auto domain=standardMaterialDomainOwner_;
        const auto context=materialContextOwner_;
        out.phase=Phase::archives;
        const auto overlays=readArchive(originalRoot,OriginalUiArchiveId50KG::overlays);
        const auto localization=readArchive(originalRoot,OriginalUiArchiveId50KG::localization);
        auto english=std::make_shared<OriginalUiEnglish50KG>();
        if(!readOriginalUiEnglish50KG(*localization,*english,out.localization))
            return fail("TimeStats authentic English localization rejected");
        out.phase=Phase::sources;
        std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>> materials;
        for(const char* name:{"ui\\textures\\stats.psm","font\\eurostileboldbold13ingame.psm"}){
            const auto member=overlays->find(name);
            if(!member.identity||!member.data||member.size!=member.identity->bytes)
                return fail("TimeStats authentic PSM missing");
            auto source=std::make_shared<OriginalStandardMaterialSource50LA>();
            const std::vector<std::uint8_t> bytes(member.data,member.data+member.size);
            if(!source->prepare(member.identity->name,bytes,out.source))
                return fail("TimeStats original PSM parse rejected");
            materials.push_back(std::move(source));
        }
        std::vector<OriginalStandardTextureSourceInput50LA> textures;
        for(const char* name:{"ui\\textures\\stats.pst","font\\eurostileboldbold13ingame.pst",
                             "font\\denmarkbold16ingame.pst"}){
            const auto member=overlays->find(name);
            if(!member.identity||!member.data||member.size!=member.identity->bytes)
                return fail("TimeStats authentic PST missing");
            textures.push_back({member.identity->name,
                std::vector<std::uint8_t>(member.data,member.data+member.size)});
        }
        // Construct the genuine detached base before resource loader effects.
        out.phase=Phase::owner;
        out.owner=std::shared_ptr<rr::game::OriginalTimeStats50LA>(new rr::game::OriginalTimeStats50LA(
            entitiesOwner_,actualSessionGeneration,domain,context,overlays,localization,english));
        timeStatsColdOwner_=out.owner;out.sourceEffectsCommitted=true;
        const auto provider=domain->archiveProvider3E1A70();
        if(!provider||!provider->mountAuthenticatedUi(overlays,out.provider)||
           !provider->sealNativeMounts(out.provider))return fail("TimeStats canonical archive mount rejected");
        if(!domain->appendOriginalSources(materials,textures,out.material))
            return fail("TimeStats canonical corpus append rejected");
        {
            // Actual1A8A6C/1A8AA0 outer path, restored at native boundary
            // corresponding to1A90CC even on partial failure/exception.
            struct DirectoryScope {
                std::string& actual;std::string previous;
                explicit DirectoryScope(std::string& value):actual(value),previous(value){actual="ui\\textures\\";}
                ~DirectoryScope(){actual.swap(previous);}
            } directory(materialDirectory36B970_);
            out.phase=Phase::stats;
            if(!out.owner->stageStats193240(materialDirectory36B970_,
                static_cast<std::uint32_t>(videoOwner_->packetZMaximum),out.object))
                return fail("TimeStats stats resource stage rejected");
            out.phase=Phase::fonts;
            if(!loadTimeStatsFonts193A0C(overlays,out.font)||
               !out.owner->borrowFonts193A24(*fonts328Owner_,out.object))
                return fail("TimeStats Application font stage rejected");
            out.phase=Phase::text;
            if(!out.owner->stageTextReferences193A58(materialDirectory36B970_,out.object))
                return fail("TimeStats text-reference stage rejected");
        }
        out.phase=Phase::pixels;
        const auto stats=overlays->find("ui\\textures\\stats.pst");
        const auto font=overlays->find("font\\eurostileboldbold13ingame.pst");
        const auto cel=overlays->find("font\\eurostileboldbold13ingame.cel");
        if(!cel.identity||!cel.data||cel.size!=cel.identity->bytes)
            return fail("TimeStats original13ingame CEL missing");
        // IDs0/1 are local bank slots only, not original resource identities.
        // Exact member provenance is retained separately by the existing owner.
        std::vector<OriginalUiImageSource50KG> images{
            {0,stats,0,stats.size,{}},{1,font,0,font.size,cel}};
        auto pixels=std::make_shared<OriginalUiScreenAssets50KG>();
        if(!pixels->load(images,out.image))return fail("TimeStats original pixel bridge rejected");
        out.pixels=pixels;
        if(!out.owner->borrowTextPixels(out.pixels,1,out.object))
            return fail("TimeStats canonical font/pixel bridge mismatch");
        if(deferPopulation){out.phase=Phase::awaitingFinalVerification;return true;}
        out.phase=Phase::population;
        // Explicit native cold policy corresponding to the later original
        //1AA5A0 resource pair and1A7B14 collector. This is not six per-text
        //population calls and does not certify full retail race chronology.
        if(!domain->populate(context,out.material))
            return fail("TimeStats later canonical population/collector rejected");
        out.sourceEffectsCommitted=true;
        out.phase=Phase::awaitingFinalVerification;
        if(!out.owner->finishNativeColdPreparation50LA(out.pixels,0,1,
            videoOwner_->width,videoOwner_->height,out.object))
            return fail("TimeStats canonical cold verification rejected");
        // CPU/source resource preparation only. Actual view binding, frame
        //projection, GPU upload and renderer retirement are separate joins.
        out.phase=Phase::cpuPrepared;return true;
    }catch(const std::exception& e){return fail(e.what());}
    catch(...){return fail("TimeStats cold preparation exception");}
}
inline bool Application::finishTimeStatsNativeCold50LA(TimeStatsColdResult50LA& out)const noexcept {
    using Phase=TimeStatsColdResult50LA::Phase;
    if(out.phase!=Phase::awaitingFinalVerification||!out.owner||!out.pixels||
       timeStatsColdOwner_.lock()!=out.owner||!videoOwner_||
       !Video::materialConstructionPinLive(*videoOwner_))return false;
    if(!out.owner->finishNativeColdPreparation50LA(out.pixels,0,1,
        videoOwner_->width,videoOwner_->height,out.object)){
        out.failedAt=out.phase;out.phase=Phase::failed;
        out.failure="TimeStats final shared collector verification rejected";return false;
    }
    out.sourceEffectsCommitted=true;out.phase=Phase::cpuPrepared;return true;
}
inline bool Application::stageDamageNativeCold50LA(const std::filesystem::path& originalRoot,
    std::uint64_t actualSessionGeneration,DamageColdResult50LA& out)const noexcept {
    using namespace rr::assets;using Phase=DamageColdResult50LA::Phase;
    if(out.phase!=Phase::fresh||out.owner||out.pixels)return false;
    const auto fail=[&](const char* message)noexcept {
        out.failedAt=out.phase;out.phase=Phase::failed;
        out.sourceEffectsCommitted=out.sourceEffectsCommitted||out.object.sourceEffectsCommitted||
            out.material.sourceEffectsCommitted;
        try{out.failure=message;}catch(...){}return false;
    };
    try {
        if(!actualSessionGeneration||!damageColdOwner_.expired()||!entitiesOwner_||
           !entitiesOwner_->application10()||!videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
           !materialContextOwner_||!materialContextOwner_->live()||
           materialContextOwner_->videoIdentity()!=videoOwner_.get()||
           materialContextOwner_->materialApplication()!=materialApplication||
           !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())
            return fail("Damage requires actual live Application cold owners");
        const auto domain=standardMaterialDomainOwner_;const auto context=materialContextOwner_;
        out.phase=Phase::archives;
        const auto overlays=readArchive(originalRoot,OriginalUiArchiveId50KG::overlays);
        out.phase=Phase::sources;
        const auto pst=overlays->find("ui\\textures\\dmgradar.pst");
        if(!pst.identity||!pst.data||pst.size!=pst.identity->bytes||
           overlays->find("ui\\textures\\dmgradar.psm").identity)
            return fail("Damage authentic PST/archive absence proof rejected");
        std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>> materials;
        std::vector<OriginalStandardTextureSourceInput50LA> textures;
        textures.push_back({pst.identity->name,std::vector<std::uint8_t>(pst.data,pst.data+pst.size)});
        out.phase=Phase::owner;
        out.owner=std::shared_ptr<rr::game::OriginalDamage50LA>(new rr::game::OriginalDamage50LA(
            entitiesOwner_,actualSessionGeneration,domain,context,overlays));
        damageColdOwner_=out.owner;out.sourceEffectsCommitted=true;
        const auto provider=domain->archiveProvider3E1A70();
        if(!provider||!provider->mountAuthenticatedUi(overlays,out.provider)||
           !provider->sealNativeMounts(out.provider))return fail("Damage canonical archive mount rejected");
        if(!domain->appendOriginalSources(materials,textures,out.material))
            return fail("Damage canonical source append rejected");
        {
            struct DirectoryScope {
                std::string& actual;std::string previous;
                explicit DirectoryScope(std::string& value):actual(value),previous(value){actual="ui\\textures\\";}
                ~DirectoryScope(){actual.swap(previous);}
            } directory(materialDirectory36B970_);
            out.phase=Phase::textures;
            if(!out.owner->stageTextures18C6F0(materialDirectory36B970_,
                static_cast<std::uint32_t>(videoOwner_->packetZMaximum),out.object))
                return fail("Damage texture stage rejected");
        }
        out.phase=Phase::pixels;
        std::vector<OriginalUiImageSource50KG> images{{0u,pst,0,pst.size,{}}};
        auto pixels=std::make_shared<OriginalUiScreenAssets50KG>();
        if(!pixels->load(images,out.image))return fail("Damage authentic pixel bridge rejected");
        out.pixels=pixels;
        // Root's later real population/collector join owns final sealing.
        // This owner never rebuilds the collector to manufacture readiness.
        out.phase=Phase::awaitingFinalVerification;return true;
    }catch(const std::exception& e){return fail(e.what());}
    catch(...){return fail("Damage cold preparation exception");}
}
inline bool Application::finishDamageNativeCold50LA(DamageColdResult50LA& out)const noexcept {
    using Phase=DamageColdResult50LA::Phase;
    if(out.phase!=Phase::awaitingFinalVerification||!out.owner||!out.pixels||
       damageColdOwner_.lock()!=out.owner)return false;
    try {
        if(!videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
           !materialContextOwner_||!materialContextOwner_->live()||
           materialContextOwner_->videoIdentity()!=videoOwner_.get()||
           out.owner->context_!=materialContextOwner_||
           out.owner->domain_!=standardMaterialDomainOwner_||
           !out.owner->finishNativeColdPreparation50LA(out.pixels,0,out.object)){
            out.sourceEffectsCommitted=out.sourceEffectsCommitted||out.object.sourceEffectsCommitted;
            out.failedAt=out.phase;out.phase=Phase::failed;
            out.failure="Damage final canonical collector verification rejected";return false;
        }
        out.sourceEffectsCommitted=true;out.phase=Phase::cpuPrepared;return true;
    }catch(...){out.failedAt=out.phase;out.phase=Phase::failed;return false;}
}
inline bool Application::stageSpeedBoostNativeCold50LA(const std::filesystem::path& originalRoot,
    std::uint64_t actualSessionGeneration,SpeedBoostColdResult50LA& out)const noexcept {
    using namespace rr::assets;using Phase=SpeedBoostColdResult50LA::Phase;
    if(out.phase!=Phase::fresh||out.owner||out.pixels)return false;
    const auto fail=[&](const char* message)noexcept {
        out.failedAt=out.phase;out.phase=Phase::failed;
        out.sourceEffectsCommitted=out.sourceEffectsCommitted||out.object.sourceEffectsCommitted||
            out.font.sourceEffectsCommitted||out.material.sourceEffectsCommitted;
        try{out.failure=message;}catch(...){}return false;
    };
    try {
        if(!actualSessionGeneration||!speedBoostColdOwner_.expired()||!entitiesOwner_||
           !entitiesOwner_->application10()||!videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
           !materialContextOwner_||!materialContextOwner_->live()||
           materialContextOwner_->videoIdentity()!=videoOwner_.get()||
           materialContextOwner_->materialApplication()!=materialApplication||
           !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())
            return fail("Speed/Boost requires actual live Application cold owners");
        const auto domain=standardMaterialDomainOwner_;const auto context=materialContextOwner_;
        out.phase=Phase::archives;
        const auto overlays=readArchive(originalRoot,OriginalUiArchiveId50KG::overlays);
        const auto localization=readArchive(originalRoot,OriginalUiArchiveId50KG::localization);
        auto english=std::make_shared<OriginalUiEnglish50KG>();
        if(!readOriginalUiEnglish50KG(*localization,*english,out.localization))
            return fail("Speed/Boost authentic English localization rejected");
        out.phase=Phase::sources;
        std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>> materials;
        for(const char* name:{"font\\eurostileboldbolditalic26ingame.psm","font\\eurostileboldbold13ingame.psm"}){
            const auto member=overlays->find(name);
            if(!member.identity||!member.data||member.size!=member.identity->bytes)
                return fail("Speed/Boost authentic font PSM missing");
            auto source=std::make_shared<OriginalStandardMaterialSource50LA>();
            const std::vector<std::uint8_t> bytes(member.data,member.data+member.size);
            if(!source->prepare(member.identity->name,bytes,out.source))return fail("Speed/Boost original font PSM rejected");
            materials.push_back(std::move(source));
        }
        constexpr const char* names[]={"ui\\textures\\spdbost1","ui\\textures\\boost_a1",
            "ui\\textures\\boost_a2","ui\\textures\\boost_a3","ui\\textures\\boost_a4",
            "ui\\textures\\boost_a5","ui\\textures\\boost_a6","ui\\textures\\boost_a7",
            "ui\\textures\\boost_a8","ui\\textures\\boost_a9",
            "font\\eurostileboldbolditalic26ingame","font\\eurostileboldbold13ingame"};
        std::vector<OriginalStandardTextureSourceInput50LA> textures;
        std::array<OriginalUiMemberView50KG,12> sourceImages{};
        for(std::size_t i=0;i<12;++i){
            const auto member=overlays->find(std::string(names[i])+".pst");
            if(!member.identity||!member.data||member.size!=member.identity->bytes||
               (i<10&&overlays->find(std::string(names[i])+".psm").identity))
                return fail("Speed/Boost authentic texture/absence proof rejected");
            sourceImages[i]=member;
            textures.push_back({member.identity->name,std::vector<std::uint8_t>(member.data,member.data+member.size)});
        }
        out.phase=Phase::owner;
        out.owner=std::shared_ptr<rr::game::OriginalSpeedBoost50LA>(new rr::game::OriginalSpeedBoost50LA(
            entitiesOwner_,actualSessionGeneration,domain,context,overlays,localization,english));
        speedBoostColdOwner_=out.owner;out.sourceEffectsCommitted=true;
        const auto provider=domain->archiveProvider3E1A70();
        if(!provider||!provider->mountAuthenticatedUi(overlays,out.provider)||
           !provider->sealNativeMounts(out.provider))return fail("Speed/Boost canonical archive mount rejected");
        if(!domain->appendOriginalSources(materials,textures,out.material))
            return fail("Speed/Boost canonical source append rejected");
        {
            struct DirectoryScope {
                std::string& actual;std::string previous;
                explicit DirectoryScope(std::string& value):actual(value),previous(value){actual="ui\\textures\\";}
                ~DirectoryScope(){actual.swap(previous);}
            } directory(materialDirectory36B970_);
            out.phase=Phase::textures;
            if(!out.owner->stageTextures1895B0(materialDirectory36B970_,
                static_cast<std::uint32_t>(videoOwner_->packetZMaximum),out.object))
                return fail("Speed/Boost texture stage rejected");
            out.phase=Phase::triangle;
            if(!out.owner->stageTriangle18A77C(materialDirectory36B970_,out.object))
                return fail("Speed/Boost triangle stage rejected");
            out.phase=Phase::fonts;
            if(!loadSpeedBoostFonts18A99C(overlays,out.font)||
               !out.owner->borrowFonts18A9B4(*fonts328Owner_,out.object))
                return fail("Speed/Boost actual font stage rejected");
            out.phase=Phase::text;
            if(!out.owner->stageTextReferences18A9DC(materialDirectory36B970_,out.object))
                return fail("Speed/Boost text-reference stage rejected");
        }
        out.phase=Phase::pixels;
        std::vector<OriginalUiImageSource50KG> images;
        for(std::size_t i=0;i<12;++i){
            OriginalUiMemberView50KG cel{};
            if(i>=10){cel=overlays->find(std::string(names[i])+".cel");
                if(!cel.identity||!cel.data||cel.size!=cel.identity->bytes)
                    return fail("Speed/Boost authentic CEL missing");}
            images.push_back({static_cast<std::uint32_t>(i),sourceImages[i],0,sourceImages[i].size,cel});
        }
        auto pixels=std::make_shared<OriginalUiScreenAssets50KG>();
        if(!pixels->load(images,out.image))return fail("Speed/Boost authentic pixel bridge rejected");
        out.pixels=pixels;
        // No final population/collector here. Native root stages TimeStats
        // next; its later actual1AA5A0/1A7B14 join resolves BOTH sets of refs.
        out.phase=Phase::awaitingFinalVerification;return true;
    }catch(const std::exception& e){return fail(e.what());}
    catch(...){return fail("Speed/Boost cold preparation exception");}
}
inline bool Application::finishSpeedBoostNativeCold50LA(SpeedBoostColdResult50LA& out)const noexcept {
    using Phase=SpeedBoostColdResult50LA::Phase;
    if(out.phase!=Phase::awaitingFinalVerification||!out.owner||!out.pixels||
       speedBoostColdOwner_.lock()!=out.owner)return false;
    try {
        constexpr std::array<std::uint32_t,12> images{{0,1,2,3,4,5,6,7,8,9,10,11}};
        if(!videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
           !out.owner->finishNativeColdPreparation50LA(out.pixels,images,
                videoOwner_->width,videoOwner_->height,out.object)){
            out.failedAt=out.phase;out.phase=Phase::failed;
            out.failure="Speed/Boost final canonical population/collector verification rejected";return false;
        }
        out.sourceEffectsCommitted=true;out.phase=Phase::cpuPrepared;return true;
    }catch(...){out.failedAt=out.phase;out.phase=Phase::failed;return false;}
}
inline std::shared_ptr<const rr::assets::OriginalUiArchive50KG> readArchive(
    const std::filesystem::path& root,rr::assets::OriginalUiArchiveId50KG id) {
    const auto* identity=rr::assets::originalUiArchiveIdentity50KG(id);
    require(identity,"original UI archive catalog identity missing");
    // The compiled authenticated archive catalog is the only path authority.
    // No private workspace path, recursive search, basename fallback or cache.
    const auto path=root/std::filesystem::path(identity->discPath);
    std::ifstream input(path,std::ios::binary|std::ios::ate);
    if(!input)throw std::runtime_error("cannot open original UI archive: "+path.string());
    const std::streamoff bytes=input.tellg();
    if(bytes<0 || static_cast<std::uint64_t>(bytes)!=identity->bytes)
        throw std::runtime_error("original UI archive size mismatch: "+path.string());
    std::vector<std::uint8_t> raw(identity->bytes);
    input.seekg(0);
    input.read(reinterpret_cast<char*>(raw.data()),static_cast<std::streamsize>(raw.size()));
    if(!input)throw std::runtime_error("cannot read complete original UI archive: "+path.string());
    auto archive=std::make_shared<rr::assets::OriginalUiArchive50KG>();
    rr::assets::OriginalUiArchiveDiagnostic50KG diagnostic;
    if(!archive->loadOwned(id,std::move(raw),diagnostic))
        throw std::runtime_error("original UI archive authentication/decode rejected: "+path.string()+
            " status="+std::to_string(static_cast<unsigned>(diagnostic.status))+
            " member="+std::to_string(diagnostic.member));
    return archive;
}
struct Frontend : rr::platform::OriginalUiRiderPresentationHost50KG {
private:
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

    static bool uploadLifetimeLive(const UploadLifetime& token)noexcept {
        const auto* owner=token.frontend_;
        return owner&&owner->uploadLifetime_.get()==&token&&owner->materialApplicationOwner&&
            token.video_&&owner->materialApplicationOwner->videoOwner_.get()==token.video_.get()&&
            token.video_->originalMain&&token.video_->host.created()&&token.video_->host.current();
    }
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
        const rr::platform::VehicleWindowDiagnostic50DS* renderer=nullptr) const {
        const auto elapsed=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
        const auto snapshot=coordinator.snapshot();const auto& route=snapshot.route;
        stageHistory[nextStageObservation]={stage,start,end,frameCount,snapshot.updates,route.screen,route.page,okay};
        nextStageObservation=(nextStageObservation+1)%stageHistory.size();
        if(stageObservationCount<stageHistory.size())++stageObservationCount;
        if(!traceStages)return;
        // End the observation before formatting/output; logging overhead still
        // remains real work in the next accepted update interval.
        std::cerr<<"original_ui_stage {\"stage\":\""<<stage<<"\",\"completed_frame\":"<<frameCount
            <<",\"accepted_update_sequence\":"<<snapshot.updates<<",\"screen\":"<<unsigned(route.screen)
            <<",\"page\":"<<unsigned(route.page)<<",\"wall_nanoseconds\":"<<elapsed
            <<",\"steady_begin_nanoseconds\":"<<std::chrono::duration_cast<std::chrono::nanoseconds>(start.time_since_epoch()).count()
            <<",\"steady_end_nanoseconds\":"<<std::chrono::duration_cast<std::chrono::nanoseconds>(end.time_since_epoch()).count()
            <<",\"okay\":"<<(okay?"true":"false");
        if(renderer)std::cerr<<",\"source_coherent_framebuffer_fetch\":"<<(renderer->originalUiCoherentFramebufferFetch?"true":"false")
            <<",\"draws\":"<<renderer->originalUiDraws<<",\"source_triangles\":"<<renderer->originalUiSourceTriangles
            <<",\"source_texture_barriers\":"<<renderer->originalUiTextureBarriers;
        std::cerr<<"}\n";
    }
    void reportStage(const char* stage,StageClock::time_point start,bool okay=true,
        const rr::platform::VehicleWindowDiagnostic50DS* renderer=nullptr) const {
        reportStageRange(stage,start,StageClock::now(),okay,renderer);
    }
    void accountColdInterval(const rr::platform::OriginalUiFrontendColdInterval50KG& interval) {
        if(!interval.measured)return;
        require(interval.end>=interval.begin,"UI measured cold clock moved backwards");
        const auto actual=std::chrono::duration_cast<std::chrono::nanoseconds>(interval.end-interval.begin).count();
        require(static_cast<std::uint64_t>(actual)==interval.nanoseconds,"UI measured cold duration mismatch");
        require(original_ui_elapsed50LH::creditCold(interval.nanoseconds,
            pendingUpdateColdNanoseconds,pendingMaterialColdNanoseconds),"UI cold interval overflow");
    }
    void accountBankPublication(const rr::platform::OriginalUiFrontendColdInterval50KG& interval,
        const rr::platform::OriginalUiFrontendWorkObservation50KG& drain,bool okay) {
        // Diagnostic stage ranges never feed the clocks. Only the coordinator's
        // exact owned bank-publication receipt credits the two consumers.
        if(drain.measured)reportStageRange("bank_gpu_drain",drain.begin,drain.end,okay);
        if(interval.measured)reportStageRange("cold_bank_publication",interval.begin,interval.end,okay);
        accountColdInterval(interval);
    }
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
    const char* presentationPolicyName()const noexcept {
        switch(riderPresentationPolicy){
        case RiderPresentationPolicy::driverSwapInterval1:return "driver_swap_interval1";
        case RiderPresentationPolicy::nativePcDeadline60Hz:return "native_pc_deadline60hz";
        case RiderPresentationPolicy::restoring:return "restoring";
        default:return "inactive";
        }
    }
    bool available()const noexcept override {
        if(!presentationHost||!presentationNow||!presentationHost->hasWindow()||!presentationHost->current()||hostPaused())return false;
        const auto& info=presentationHost->info();
        if(!info.focused||info.minimized||info.drawableWidth<=0||info.drawableHeight<=0)return false;
        if(!presentationActive)return riderPresentationPolicy==RiderPresentationPolicy::inactive;
        if(info.drawableWidth!=presentationWidth||info.drawableHeight!=presentationHeight)return false;
        return (riderPresentationPolicy==RiderPresentationPolicy::driverSwapInterval1&&info.swapInterval==1)||
            (riderPresentationPolicy==RiderPresentationPolicy::nativePcDeadline60Hz&&info.swapInterval==0);
    }
    bool begin()noexcept override {
        if(presentationActive||!available())return false;
        savedSwapInterval=presentationHost->info().swapInterval;
        presentationWidth=presentationHost->info().drawableWidth;
        presentationHeight=presentationHost->info().drawableHeight;
        presentationBeginSwaps=riderPresentationSwaps;
        presentationCompletedWaits=0;presentationCompletedWaitNanoseconds=0;riderDeadline.reset();
        // Any failed setter leaves an explicit restoration obligation until
        // the exact saved native setting is verified; no assumed rollback.
        presentationActive=true;riderPresentationPolicy=RiderPresentationPolicy::restoring;
        rr::platform::GameHostDiagnostic50ER d;
        if(presentationHost->setSwapInterval(1,d))riderPresentationPolicy=RiderPresentationPolicy::driverSwapInterval1;
        else {
            const bool swapControlFailure=d.error==rr::platform::GameHostError50ER::Window&&
                d.window.error==rr::platform::WindowHostError50DQ::SwapIntervalFailure;
            if(!presentationHost->setSwapInterval(savedSwapInterval,d))return false;
            // Optional native-PC policy, never a fabricated interval1 result.
            // Wrong context/thread, nonzero saved intervals and unaccepted
            // audio/clock baselines cannot activate this alternate owner.
            if(!allowNativePcRiderDeadline||!swapControlFailure||savedSwapInterval!=0||
               presentationHost->info().swapInterval!=0||!lastUpdateNanoseconds||
               original_ui_elapsed50LH::project(lastUpdateNanoseconds,presentationNow(),pendingUpdateColdNanoseconds).error!=
                   original_ui_elapsed50LH::Error::none){
                presentationActive=false;riderPresentationPolicy=RiderPresentationPolicy::inactive;return false;
            }
            riderPresentationPolicy=RiderPresentationPolicy::nativePcDeadline60Hz;
        }
        if(!available())return false;
        std::fprintf(stderr,"original_rider_presentation_begin policy=%s actual_interval=%d saved_interval=%d physical_scanout_measured=0\n",
            presentationPolicyName(),presentationHost->info().swapInterval,savedSwapInterval);
        return true;
    }
    bool end()noexcept override {
        if(!presentationActive)return true;
        rr::platform::GameHostDiagnostic50ER d;
        if(!presentationHost||!presentationHost->setSwapInterval(savedSwapInterval,d))return false;
        std::fprintf(stderr,"original_rider_presentation_end policy=%s restored_interval=%d swap_calls=%llu completed_waits=%llu completed_wait_scope_ns=%llu physical_scanout_measured=0\n",
            presentationPolicyName(),presentationHost->info().swapInterval,
            static_cast<unsigned long long>(riderPresentationSwaps-presentationBeginSwaps),
            static_cast<unsigned long long>(presentationCompletedWaits),
            static_cast<unsigned long long>(presentationCompletedWaitNanoseconds));
        presentationActive=false;riderPresentationPolicy=RiderPresentationPolicy::inactive;
        riderDeadline.reset();presentationWidth=0;presentationHeight=0;return true;
    }
    bool pollPresentationInput()noexcept {
        if(!presentationActive||!available())return false;
        rr::platform::GameHostInput50ER input;rr::platform::GameHostDiagnostic50ER d;
        if(!presentationHost->poll(input,d)||input.keys.quitRequested||input.keys.activityLost||
           input.keys.resized||input.keys.eventBudgetExhausted||!available())return false;
        return true;
    }
    bool waitBeforeSample()noexcept override {
        if(!pollPresentationInput())return false;
        // Driver policy retains its existing real swap wait. The opt-in
        // interval0 host policy waits BEFORE render, inside actual sample dt.
        if(riderPresentationPolicy==RiderPresentationPolicy::driverSwapInterval1)return true;
        if(riderPresentationPolicy!=RiderPresentationPolicy::nativePcDeadline60Hz)return false;
        using Error=original_ui_rider_pacing50LB::Error;
        const auto begin=presentationNow();
        if(riderDeadline.beginWait(begin)!=Error::none)return false;
        for(;;){
            if(!pollPresentationInput()||!checkpointAudio())return false;
            const auto now=presentationNow();const auto observed=riderDeadline.observe(now);
            if(observed.error!=Error::none)return false;
            if(observed.due){
                if(now<begin||now-begin>std::numeric_limits<std::uint64_t>::max()-presentationCompletedWaitNanoseconds||
                   presentationCompletedWaits==std::numeric_limits<std::uint64_t>::max()||
                   riderDeadline.finishWait(now)!=Error::none)return false;
                presentationCompletedWaitNanoseconds+=now-begin;++presentationCompletedWaits;return true;
            }
            // Sleep requests are at most1ms; oversleep remains real elapsed
            // work. All measured active time remains in the PCM accounting domain.
            try{std::this_thread::sleep_for(std::chrono::nanoseconds(observed.sleepNanoseconds));}
            catch(...){return false;}
        }
    }
    bool materialClock(rr::platform::OriginalUiMainFrameClock50KG& out)noexcept override {
        if(!presentationClock)return false;
        try{out=sampleClock(*presentationClock,true);return true;}catch(...){return false;}
    }
    bool checkpointAudio()noexcept override {
        if(!presentationActive||!available())return false;
        const auto now=presentationNow();
        // Blocking presentation requires an already accepted active baseline;
        // it cannot use the first/resumed-page zero-delta convention.
        if(!lastUpdateNanoseconds)return false;
        const auto interval=original_ui_elapsed50LH::project(lastUpdateNanoseconds,now,pendingUpdateColdNanoseconds);
        if(interval.error!=original_ui_elapsed50LH::Error::none)return false;
        rr::platform::AudioOutputDiagnostic50HD d;
        if(!audioOutput.advanceAcceptedNanoseconds(interval.activeNanoseconds,d))return false;
        // These actual steady-clock intervals are mixed exactly once during
        // blocking native presentation. They are excluded from next page dt;
        // material timestamps retain their normal advancing application clock.
        // A successful checkpoint consumes update credit exactly once. The
        // separately sampled material clock still owns its own cold credit.
        pendingUpdateColdNanoseconds=0;lastUpdateNanoseconds=now;return true;
    }
    bool present(rr::platform::VehicleWindowGl50DS& renderer,
        const rr::platform::OriginalUiPrivatePublication50KP& image,
        rr::platform::VehicleWindowDiagnostic50DS& d)noexcept override {
        if(!presentationActive||!available()||!image.completed()||!pollPresentationInput())return false;
        rr::platform::GameHostDiagnostic50ER hostDiagnostic;
        const auto& info=presentationHost->info();
        if(info.drawableWidth!=image.width()||info.drawableHeight!=image.height())return false;
        rr::platform::OriginalUiWindowBackBufferCopy copy;
        if(!renderer.copyOriginalUiPrivatePublicationToWindow(image,copy,d)||!copy.commandsAccepted()||
           !presentationHost->swapWindow(hostDiagnostic))return false;
        ++riderPresentationSwaps;
        return checkpointAudio();
    }
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
        rr::game::HostMonotonicClock50ID& applicationClock,bool active) {
        std::uint32_t rawSample=0;
        require(rr::game::sampleHostTimestamp50ID(applicationClock,rawSample),
            "original UI shared material timestamp unavailable");
        require(materialApplicationOwner&&materialApplicationOwner->publishAnimationTimestamp(applicationClock,rawSample),
            "original UI canonical time requires the same application backend");
        const auto now=applicationClock.lastNanoseconds;
        if(!sourceSampleKnown)activeSourceNanoseconds=now;
        else {
            require(now>=lastSourceNanoseconds,"original UI material clock moved backwards");
            const auto elapsed=now-lastSourceNanoseconds;
            require(pendingMaterialColdNanoseconds<=elapsed,"UI cold interval exceeds sampled wall interval");
            if(active&&previousSourceActive){
                const auto accepted=elapsed-pendingMaterialColdNanoseconds;
                require(accepted<=std::numeric_limits<std::uint64_t>::max()-activeSourceNanoseconds,
                    "UI active material clock overflow");
                activeSourceNanoseconds+=accepted;
            }
        }
        pendingMaterialColdNanoseconds=0;lastSourceNanoseconds=now;
        sourceSampleKnown=true;previousSourceActive=active;
        // Keep the real application clock/RNG owner unchanged. Quantize only
        // this measured active presentation timeline through the existing
        // native source timer conversion, excluding focus and cold loading.
        require(rr::game::hostNanosecondsToSourceTimestamp50ID(activeSourceNanoseconds,sourceMilliseconds),
            "original UI active material timestamp conversion failed");
        return {true,active,sourceMilliseconds,materialApplicationOwner->animationClock()};
    }
    void prepare(const Application& application,const std::shared_ptr<Video>& video,
        const rr::platform::MaterialTextureGlRequest50EB& textureRequest,
        rr::platform::VehicleWindowRequest50DS& rendererRequest,
        std::shared_ptr<const rr::platform::OriginalUiCourseRaceDestination50KG> raceDestination={},
        std::uint64_t (*nativeNow)()noexcept=nullptr,std::uint64_t nativeSessionIndex=0) {
        require(!materialApplicationOwner,"frontend material directory already entered");
        materialApplicationOwner=&application;
        application.enterUiMaterialDirectory17DB80();
        using Id=rr::assets::OriginalUiArchiveId50KG;
        rr::platform::OriginalUiFrontendCold50KG cold;
        cold.appStart=application.appStart(application.assetRoot);
        cold.mainMenu=readArchive(application.assetRoot,Id::mainMenu);
        cold.options=readArchive(application.assetRoot,Id::options);
        cold.hall=readArchive(application.assetRoot,Id::hallOfFame);
        cold.fonts=readArchive(application.assetRoot,Id::fonts);
        cold.controllerPopup=readArchive(application.assetRoot,Id::controllerPopup);
        cold.overlays=readArchive(application.assetRoot,Id::overlays);
        cold.localization=readArchive(application.assetRoot,Id::localization);
        languageArchive=cold.localization;fontArchive=cold.fonts;
        cold.runtime=application.materialRuntime;cold.video=video->publication(video);
        cold.profile=application.profile;cold.application=application.state;cold.applicationAnimation=application.animationClock();cold.courseRaceDestination=std::move(raceDestination);
        presentationHost=&video->host;presentationNow=nativeNow;
        // Absent retains strict driver-only admission. This request changes
        // host pacing only, never Rider profile/resource/selection guards.
        if(const char* policy=std::getenv("RR_ORIGINAL_RIDER_PRESENTATION")){
            require(std::string(policy)=="allow-native-pc-deadline60hz",
                "RR_ORIGINAL_RIDER_PRESENTATION must be allow-native-pc-deadline60hz");
            allowNativePcRiderDeadline=true;
        }
        presentationClock=application.state->clock101A50.get();cold.riderPresentation=this;
        cold.lightManager=application.lightManager;
        cold.riderProfile=application.riderProfile;
        // Original-disc mode decodes in process, with no external image pack.
        // Both paths preserve the same source and decoded-image hash admission.
        if(application.decodeRiderImagesInProcess||!application.riderCacheRoot.empty()){
            require(application.riderProfile && application.riderProfile->initialized(),
                "original Rider application profile owner unavailable");
            using Cache=rr::assets::OriginalUiIpumTextureCache50KG;
            require(Cache::catalogAvailable(),"original Rider IPUM output catalog is unavailable");
            cold.rider=readArchive(application.assetRoot,Id::rider);
            // Course is an optional destination. A missing archive must not
            // remove existing Main/Options/Rider Back capability. If supplied,
            // its bytes still authenticate normally; malformed input is fatal.
            const auto* courseIdentity=rr::assets::originalUiArchiveIdentity50KG(Id::course);
            require(courseIdentity,"original Course archive identity missing");
            if(std::filesystem::exists(application.assetRoot/std::filesystem::path(courseIdentity->discPath)))
                cold.course=readArchive(application.assetRoot,Id::course);
            else
                std::cerr<<"Original Course archive absent; Course destination remains unavailable.\n";
            // Optional genuine Tournament archive, independent of Course. A
            // missing owner keeps only that destination unavailable.
            const auto* tournamentIdentity=rr::assets::originalUiArchiveIdentity50KG(Id::tournament);
            require(tournamentIdentity,"original Tournament archive identity missing");
            if(std::filesystem::exists(application.assetRoot/std::filesystem::path(tournamentIdentity->discPath)))
                cold.tournament=readArchive(application.assetRoot,Id::tournament);
            else std::cerr<<"Original Tournament archive absent; Tournament destination remains unavailable.\n";
            cold.backgrounds=readArchive(application.assetRoot,Id::backgrounds);
            cold.podTextures=readArchive(application.assetRoot,Id::podTextures);
            auto cache=std::make_shared<Cache>();
            rr::assets::OriginalUiIpumCacheDiagnostic50KG cacheDiagnostic;
            if(application.decodeRiderImagesInProcess){
                if(!cache->loadOriginal(cold.rider,cacheDiagnostic))
                    throw std::runtime_error("original Rider IPUM native decode/admission rejected: "+
                        cacheDiagnostic.detail+" error="+std::to_string(static_cast<unsigned>(cacheDiagnostic.error))+
                        " mip="+std::to_string(cacheDiagnostic.mip));
                for(std::size_t source=0;source<Cache::additionalCatalogSourceCount();++source)
                    if(!cache->loadAdditionalOriginal(source,cold.rider,cacheDiagnostic))
                        throw std::runtime_error("additional Rider IPUM native decode/admission rejected: "+
                            cacheDiagnostic.detail+" source="+std::to_string(source)+
                            " error="+std::to_string(static_cast<unsigned>(cacheDiagnostic.error))+
                            " mip="+std::to_string(cacheDiagnostic.mip));
            }else{
            std::vector<std::vector<std::uint8_t>> mips;
            mips.reserve(rr::assets::originalUiIpumMipCount50KG);
            for(std::size_t mip=0;mip<rr::assets::originalUiIpumMipCount50KG;++mip){
                const char* filename=Cache::retainedMipFilename(mip);
                require(filename && *filename,"original Rider IPUM retained filename unavailable");
                const auto path=application.riderCacheRoot/filename;
                std::ifstream input(path,std::ios::binary|std::ios::ate);
                if(!input)throw std::runtime_error("cannot open original Rider IPUM mip: "+path.string());
                const std::streamoff size=input.tellg();
                require(size>0 && static_cast<std::uint64_t>(size)<=16u*1024u*1024u,
                    "original Rider IPUM retained mip size rejected");
                std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
                input.seekg(0);input.read(reinterpret_cast<char*>(bytes.data()),
                    static_cast<std::streamsize>(bytes.size()));
                require(static_cast<bool>(input),"original Rider IPUM retained mip read failed");
                mips.push_back(std::move(bytes));
            }
            require(cache->load(cold.rider,mips,cacheDiagnostic),
                "original Rider IPUM retained mip authentication rejected");
            // A compiled additional catalog requires its real retained bytes.
            // With no extra catalog, no additional images are fabricated.
            for(std::size_t source=0;source<Cache::additionalCatalogSourceCount();++source){
                std::vector<std::vector<std::uint8_t>> additionalMips;
                const auto count=Cache::additionalMipCount(source);
                additionalMips.reserve(count);
                for(std::size_t mip=0;mip<count;++mip){
                    const char* filename=Cache::additionalRetainedMipFilename(source,mip);
                    require(filename && *filename,"additional Rider IPUM filename unavailable");
                    const auto path=application.riderCacheRoot/filename;
                    std::ifstream input(path,std::ios::binary|std::ios::ate);
                    if(!input)throw std::runtime_error("cannot open additional Rider IPUM mip: "+path.string());
                    const std::streamoff size=input.tellg();
                    require(size>0 && static_cast<std::uint64_t>(size)<=16u*1024u*1024u,
                        "additional Rider IPUM retained mip size rejected");
                    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
                    input.seekg(0);input.read(reinterpret_cast<char*>(bytes.data()),
                        static_cast<std::streamsize>(bytes.size()));
                    require(static_cast<bool>(input),"additional Rider IPUM retained mip read failed");
                    additionalMips.push_back(std::move(bytes));
                }
                require(cache->loadAdditional(source,cold.rider,additionalMips,cacheDiagnostic),
                    "additional Rider IPUM retained mip authentication rejected");
            }
            }
            cold.riderIpumCache=std::move(cache);
        }
        // Reconstructed on every frontend lifetime from the SAME Application.
        // coordinator.prepare requires genuine inputs; Rider bind prepares every requested pair.
        cold.prepareCleggLockedPreview=application.prepareCleggLockedPreview;
        cold.requestedPreviewRiders=application.requestedPreviewRiders;
        cold.optionsProfile=application.optionsProfile;cold.gameSettingsProfile=application.gameSettingsProfile;
        cold.soundMode=application.soundMode;cold.controlsProfile=application.controlsProfile;
        cold.audioMode=application.audioMode;
        require(video&&application.videoOwner_==video&&textureRequest.context==&video->host&&
            rendererRequest.context==&video->host&&textureRequest.isCurrent&&
            textureRequest.isCurrent==rendererRequest.isCurrent&&!uploadLifetime_,
            "frontend upload requires its actual Application Video and context callbacks");
        uploadLifetime_=std::shared_ptr<UploadLifetime>(new UploadLifetime(*this,video,&uploadLifetimeLive));
        using UploadBinding=rr::assets::OriginalNativeTextureUploadBinding50LA;
        cold.textureRequest=textureRequest;
        cold.textureRequest.originalUploadBinding=std::shared_ptr<const UploadBinding>(new UploadBinding(
            uploadLifetime_,application.textureLifetimeOwner_,&video->host,textureRequest.isCurrent));
        cold.textureRequest.samplers=nullptr;cold.textureRequest.samplerCount=0;
        cold.textureRequest.maximumGpuBytes=32u*1024u*1024u;
        if(application.audioMode==rr::platform::OriginalUiMainAudioMode50KG::enabled){
            rr::platform::AudioOutputRequest50HD outputRequest;
            outputRequest.tryDevice=video->host.hasWindow();outputRequest.wavPath=std::getenv("RR_AUDIO_WAV");
            if(outputRequest.wavPath&&*outputRequest.wavPath&&nativeSessionIndex){
                const std::filesystem::path requested(outputRequest.wavPath);
                sessionWavPath=(requested.parent_path()/(requested.stem().string()+"-menu-session-"+
                    std::to_string(nativeSessionIndex+1)+requested.extension().string())).string();
                outputRequest.wavPath=sessionWavPath.c_str();
            }
            if(const char* setting=std::getenv("RR_AUDIO_DEVICE")){
                require(std::string(setting)=="0"||std::string(setting)=="1","UI RR_AUDIO_DEVICE must be 0 or 1");
                outputRequest.tryDevice=std::string(setting)=="1";
            }
            std::string error;rr::platform::AudioOutputDiagnostic50HD outputDiagnostic;
            if(!application.prepareFrontendAudioOutput(*this,outputRequest,error,outputDiagnostic))throw std::runtime_error(error);
            require(audioOutput.soundModeOwner50KG()==application.soundMode.get(),
                "original UI output did not retain the application sound mode owner");
            cold.audio=audioOutput.transport();
            if(!application.prepareFrontendMenuAudio(*this,cold,error))throw std::runtime_error(error);
            const auto info=audioOutput.info();
            std::cerr<<"Original UI audio device="<<rr::platform::audioDeviceStatusName50HD(info.deviceStatus)
                <<" capture="<<info.captureActive<<" rate="<<info.sampleRate<<" detail="<<info.deviceDetail.data()<<'\n';
        }
        if(coordinator.prepare(cold,diagnostic)!=Status::ok)fail("preparation");
        require(coordinator.bindGarageDestinationRoot50KG(*this),"original UI same-Root destination binding failed");
        if(coordinator.configureRendererRequest(rendererRequest,diagnostic)!=Status::ok)fail("renderer configuration");
    }
    void activate(rr::platform::VehicleWindowGl50DS& renderer) {
        std::string languageError;
        if(!materialApplicationOwner||!materialApplicationOwner->prepareFrontendLanguageServices(*this,renderer,languageError))
            throw std::runtime_error("original UI language startup rejected: "+languageError);
        if(!materialApplicationOwner||!coordinator.prepareSharedDialogsInitial(*materialApplicationOwner,renderer,diagnostic))
            fail("shared UI initial ownership");
        if(coordinator.activate(renderer,diagnostic)!=Status::ok)fail("activation");
    }
    std::string garageRuntimeFailureText() const {
        const auto& g=diagnostic.garageRuntime;
        if(g.status==rr::assets::OriginalStandardMaterialStatus50LA::complete&&!g.sourcePC&&diagnostic.garageDetail.empty())return {};
        return " garage_runtime="+std::to_string(static_cast<unsigned>(g.status))+
            " garage_runtime_pc="+std::to_string(g.sourcePC)+" garage_runtime_row="+std::to_string(g.row)+
            " garage_runtime_slot="+std::to_string(g.slot)+" garage_runtime_pass="+std::to_string(g.pass)+
            " garage_runtime_map="+std::to_string(g.map)+" garage_runtime_field="+(g.field?g.field:"")+
            " garage_source_effects="+(g.sourceEffectsCommitted?"true":"false")+
            " garage_document="+std::to_string(static_cast<unsigned>(g.document.error))+
            " garage_document_pc="+std::to_string(g.document.sourcePC)+
            " garage_document_offset="+std::to_string(g.document.byteOffset)+
            " garage_program="+std::to_string(static_cast<unsigned>(g.program.status))+
            " garage_program_pc="+std::to_string(g.program.sourcePC)+" garage_program_row="+std::to_string(g.program.row)+
            " garage_texture="+std::to_string(static_cast<unsigned>(g.texture.status))+
            " garage_texture_pc="+std::to_string(g.texture.sourcePC)+" garage_texture_row="+std::to_string(g.texture.row)+
            " garage_texture_offset="+std::to_string(g.texture.byteOffset)+" garage_detail="+diagnostic.garageDetail;
    }
    [[noreturn]] void fail(const char* operation) const {
        throw std::runtime_error(std::string("original UI ")+operation+" rejected: status="+
            std::to_string(static_cast<unsigned>(diagnostic.status))+" source="+std::to_string(diagnostic.sourcePC)+
            " main="+std::to_string(static_cast<unsigned>(diagnostic.main.status))+
            " options="+std::to_string(static_cast<unsigned>(diagnostic.options.status))+
            " hall="+std::to_string(static_cast<unsigned>(diagnostic.hall.status))+
            " hall_pc="+std::to_string(diagnostic.hall.sourcePC)+
            " hall_draw="+std::to_string(static_cast<unsigned>(diagnostic.hall.draw.error))+
            " hall_draw_pc="+std::to_string(diagnostic.hall.draw.sourcePC)+
            " hall_draw_widget="+std::to_string(diagnostic.hall.draw.widgetId)+
            " hall_widget="+std::to_string(static_cast<unsigned>(diagnostic.hall.widget.error))+
            " hall_resource="+std::to_string(static_cast<unsigned>(diagnostic.hallResources.error))+
            " options_draw="+std::to_string(static_cast<unsigned>(diagnostic.options.draw.error))+
            " options_draw_pc="+std::to_string(diagnostic.options.draw.sourcePC)+
            " options_draw_widget="+std::to_string(diagnostic.options.draw.widgetId)+
            " options_resource="+std::to_string(static_cast<unsigned>(diagnostic.options.draw.resource.error))+
            " options_model="+std::to_string(static_cast<unsigned>(diagnostic.options.draw.resource.controlsModel.error))+
            " options_model_pc="+std::to_string(diagnostic.options.draw.resource.controlsModel.sourcePC)+
            " rider="+std::to_string(static_cast<unsigned>(diagnostic.rider.status))+
            " rider_pc="+std::to_string(diagnostic.rider.sourcePC)+
            " rider_widget="+std::to_string(static_cast<unsigned>(diagnostic.rider.widget.error))+
            " rider_dynamic="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.error))+
            " rider_dynamic_widget="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.widget))+
            " rider_model="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.error))+
            " rider_model_pc="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.sourcePC))+
            " rider_playback="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.playback.error))+
            " rider_playback_pc="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.playback.sourceStage))+
            " rider_sample="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.playback.sample.error))+
            " rider_sample_track="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.playback.sample.track))+
            " rider_sample_channel="+std::to_string(static_cast<unsigned>(diagnostic.rider.dynamic.model.playback.sample.channel))+
            " rider_backend="+std::to_string(static_cast<unsigned>(diagnostic.rider.backend.error))+
            " rider_draw="+std::to_string(static_cast<unsigned>(diagnostic.rider.draw.error))+
            " rider_draw_pc="+std::to_string(diagnostic.rider.draw.sourcePC)+
            " rider_draw_widget="+std::to_string(diagnostic.rider.draw.widget)+
            " rider_draw_surface="+std::to_string(diagnostic.rider.draw.geometrySurface)+
            " rider_draw_component="+std::to_string(diagnostic.rider.draw.geometryComponent)+
            " rider_draw_colors="+std::to_string(diagnostic.rider.draw.geometryStoredColors)+
            " rider_draw_geometry="+std::to_string(diagnostic.rider.draw.geometryStatus)+
            " rider_draw_material_index="+std::to_string(diagnostic.rider.draw.material)+
            " rider_draw_pass="+std::to_string(diagnostic.rider.draw.pass)+
            " tournament="+std::to_string(static_cast<unsigned>(diagnostic.tournament.status))+
            " tournament_pc="+std::to_string(diagnostic.tournament.sourcePC)+
            " tournament_resource="+std::to_string(static_cast<unsigned>(diagnostic.tournamentResources.error))+
            " tournament_widgets="+std::to_string(static_cast<unsigned>(diagnostic.tournamentWidgets.error))+
            " tournament_draw="+std::to_string(static_cast<unsigned>(diagnostic.tournament.draw.error))+
            " tournament_model="+std::to_string(static_cast<unsigned>(diagnostic.tournament.model.error))+
            " course="+std::to_string(static_cast<unsigned>(diagnostic.course.status))+
            " course_pc="+std::to_string(diagnostic.course.sourcePC)+
            " renderer="+std::to_string(static_cast<unsigned>(diagnostic.renderer.error))+garageRuntimeFailureText());
    }
    void input(std::uint8_t sourceInput,rr::platform::WindowControllerPresence50DQ presence=rr::platform::WindowControllerPresence50DQ::Unknown,
        std::uint8_t controller=0) {
        rr::platform::OriginalUiMainSessionInput50KG in;in.controller=controller;in.originalInput=sourceInput;in.secondControllerObservation=presence;
        rr::platform::OriginalUiFrontendInputReceipt50KG receipt;
        const auto inputStart=stageStart();
        const auto status=coordinator.input(in,receipt,diagnostic);++inputCount;
        reportStage("source_input",inputStart,status==Status::ok||status==Status::noChange||
            status==Status::routeAccepted||status==Status::unavailableDestination||status==Status::secondControllerRequired);
        accountColdInterval(receipt.coldInterval);
        if(receipt.options.popupChanged||receipt.main.popupChanged){
            const auto ui=coordinator.snapshot();const auto& popup=receipt.main.popupChanged?ui.main.controllerPopup:ui.options.controllerPopup;
            std::cerr<<"original_controller_popup {\"source_pc\":"<<(receipt.main.popupChanged?receipt.main.popupSourcePC:receipt.options.popupSourcePC)
                <<",\"device_observation\":"<<unsigned(presence)<<",\"active\":"<<(popup.active3CC?"true":"false")
                <<",\"closing\":"<<(popup.closing3C8?"true":"false")<<",\"result\":"<<popup.result3D8
                <<",\"timer\":"<<popup.timer3C4<<",\"cancel_label\":"<<(popup.hasCancel3D4?"true":"false")<<"}\n";
        }
        if(status==Status::unavailableDestination||status==Status::secondControllerRequired){
            ++unavailableCount;std::cerr<<"original_ui_destination unavailable screen="<<unsigned(receipt.source.screen)
                <<" page="<<unsigned(receipt.source.page)
                <<" status="<<unsigned(status)<<" original_input="<<unsigned(sourceInput)<<" device_observation="<<unsigned(presence)<<" source="<<diagnostic.sourcePC
                <<" native_course_load="<<unsigned(diagnostic.courseLoad)
                <<" main="<<unsigned(diagnostic.main.status)<<" options="<<unsigned(diagnostic.options.status)
                <<" garage_phase="<<rr::platform::originalUiGarageDestinationStatusName50KG(diagnostic.garage.status)
                <<" garage_request_pc="<<diagnostic.garage.requestSourcePC<<" garage_pending_pc="<<diagnostic.garage.pendingSourcePC
                <<" garage_asset_error="<<unsigned(diagnostic.garage.scene.error)<<" garage_asset_pc="<<diagnostic.garage.scene.sourcePC
                <<garageRuntimeFailureText()
                <<" tournament_status="<<unsigned(diagnostic.tournament.status)<<" tournament_pc="<<diagnostic.tournament.sourcePC
                <<" tournament_resource="<<unsigned(diagnostic.tournamentResources.error)<<" tournament_resource_pc="<<diagnostic.tournamentResources.sourcePC
                <<" tournament_widgets="<<unsigned(diagnostic.tournamentWidgets.error)<<" tournament_widgets_pc="<<diagnostic.tournamentWidgets.sourcePC
                <<" tournament_model="<<unsigned(diagnostic.tournament.model.error)<<" tournament_draw="<<unsigned(diagnostic.tournament.draw.error)
                <<" hall_status="<<unsigned(diagnostic.hall.status)<<" hall_pc="<<diagnostic.hall.sourcePC
                <<" hall_resource="<<unsigned(diagnostic.hallResources.error)<<" hall_resource_pc="<<diagnostic.hallResources.sourcePC
                <<" options_resource="<<unsigned(diagnostic.optionsResources.error)
                <<" options_resource_widget="<<diagnostic.optionsResources.widgetId
                <<" options_resource_pc="<<diagnostic.optionsResources.sourcePC
                <<" options_camera="<<unsigned(diagnostic.optionsResources.camera.status)
                <<" options_controls_model="<<unsigned(diagnostic.optionsResources.controlsModel.error)
                <<" options_controls_model_pc="<<diagnostic.optionsResources.controlsModel.sourcePC
                <<" options_controls_asset="<<unsigned(diagnostic.optionsResources.controlsModel.asset.error)
                <<" options_controls_geometry="<<diagnostic.optionsResources.controlsModel.asset.geometry.code
                <<" options_controls_geometry_offset="<<diagnostic.optionsResources.controlsModel.asset.geometry.offset
                <<" options_controls_geometry_detail="<<diagnostic.optionsResources.controlsModel.asset.geometry.detail
                <<" options_fire="<<unsigned(diagnostic.optionsResources.fire)
                <<" options_lighting="<<unsigned(diagnostic.optionsResources.lighting)
                <<" options_render_state="<<unsigned(diagnostic.optionsResources.renderState)
                <<" options_widget_status="<<unsigned(diagnostic.optionsResources.widgets.status)
                <<" options_widget_id="<<diagnostic.optionsResources.widgets.widgetId
                <<" options_widget_controller="<<unsigned(diagnostic.optionsResources.widgets.controller)
                <<" options_widget_parse="<<unsigned(diagnostic.optionsResources.widgets.parse.error)
                <<" options_widget_parse_offset="<<diagnostic.optionsResources.widgets.parse.byteOffset
                <<" options_widget_parse_pc="<<diagnostic.optionsResources.widgets.parse.sourcePC
                <<" options_widget_text="<<unsigned(diagnostic.optionsResources.widgets.text)
                <<" options_widget_icon="<<unsigned(diagnostic.optionsResources.widgets.icon)
                <<" preparation_status="<<unsigned(diagnostic.preparationStatus)
                <<" rider="<<unsigned(diagnostic.rider.status)<<" rider_pc="<<diagnostic.rider.sourcePC
                <<" course="<<unsigned(diagnostic.course.status)<<" course_pc="<<diagnostic.course.sourcePC
                <<" course_resource="<<unsigned(diagnostic.courseResources.error)
                <<" course_resource_pc="<<diagnostic.courseResources.sourcePC
                <<" course_resource_widget="<<diagnostic.courseResources.widgetId
                <<" course_resource_material="<<diagnostic.courseResources.materialIndex
                <<" course_resource_image="<<diagnostic.courseResources.imageIndex
                <<" course_material="<<unsigned(diagnostic.courseResources.material.error)
                <<" course_material_field="<<diagnostic.courseResources.material.field
                <<" course_material_pc="<<diagnostic.courseResources.material.sourcePC
                <<" course_material_parse="<<unsigned(diagnostic.courseResources.material.document.error)
                <<" course_material_parse_offset="<<diagnostic.courseResources.material.document.byteOffset
                <<" course_material_parse_pc="<<diagnostic.courseResources.material.document.sourcePC
                <<" course_images="<<unsigned(diagnostic.courseResources.resource.error)
                <<" course_images_image="<<diagnostic.courseResources.resource.image
                <<" course_image_decode="<<diagnostic.courseResources.resource.texture.code
                <<" course_image_decode_offset="<<diagnostic.courseResources.resource.texture.offset
                <<" course_atm="<<diagnostic.courseResources.atm.code
                <<" course_atm_offset="<<diagnostic.courseResources.atm.offset
                <<" course_font="<<unsigned(diagnostic.courseResources.resource.font.error)
                <<" course_font_offset="<<diagnostic.courseResources.resource.font.byteOffset
                <<" course_font_record="<<diagnostic.courseResources.resource.font.record
                <<" course_font_ascii="<<diagnostic.courseResources.resource.font.ascii
                <<" course_widget="<<unsigned(diagnostic.courseResources.widgets.error)
                <<" course_widget_id="<<diagnostic.courseResources.widgets.widget
                <<" course_widget_controller="<<unsigned(diagnostic.courseResources.widgets.controller)
                <<" course_widget_parse="<<unsigned(diagnostic.courseResources.widgets.parse.error)
                <<" course_widget_parse_offset="<<diagnostic.courseResources.widgets.parse.byteOffset
                <<" course_widget_parse_pc="<<diagnostic.courseResources.widgets.parse.sourcePC
                <<" course_text="<<unsigned(diagnostic.courseResources.text.error)
                <<" course_text_line="<<diagnostic.courseResources.text.line
                <<" course_text_glyph="<<unsigned(diagnostic.courseResources.text.glyph.error)
                <<" course_text_character="<<diagnostic.courseResources.text.glyph.character
                <<" course_text_ascii="<<diagnostic.courseResources.text.glyph.ascii
                <<" course_text_pc="<<diagnostic.courseResources.text.glyph.sourcePC
                <<" course_exit_text="<<unsigned(diagnostic.courseResources.exitText.status)
                <<" course_exit_text_pc="<<diagnostic.courseResources.exitText.sourcePC
                <<" course_hologram_pc="<<diagnostic.courseHolograms.sourcePC
                <<" rider_resource="<<unsigned(diagnostic.riderResources.error)
                <<" rider_resource_widget="<<diagnostic.riderResources.widgetId
                <<" rider_resource_material="<<diagnostic.riderResources.materialIndex
                <<" rider_resource_image="<<diagnostic.riderResources.imageIndex
                <<" rider_widget="<<unsigned(diagnostic.riderResources.widgets.error)
                <<" rider_widget_id="<<diagnostic.riderResources.widgets.widget
                <<" rider_parse="<<unsigned(diagnostic.riderResources.widgets.parse.error)
                <<" rider_parse_offset="<<diagnostic.riderResources.widgets.parse.byteOffset
                <<" rider_parse_pc="<<diagnostic.riderResources.widgets.parse.sourcePC
                <<" rider_selection="<<unsigned(diagnostic.riderResources.widgets.selection.status)
                <<" rider_selection_parse="<<unsigned(diagnostic.riderResources.widgets.selection.parse.error)
                <<" rider_selection_parse_offset="<<diagnostic.riderResources.widgets.selection.parse.byteOffset
                <<" rider_selection_parse_pc="<<diagnostic.riderResources.widgets.selection.parse.sourcePC
                <<" rider_material="<<unsigned(diagnostic.riderResources.material.error)
                <<" rider_material_field="<<diagnostic.riderResources.material.field
                <<" rider_material_parse="<<unsigned(diagnostic.riderResources.material.document.error)
                <<" rider_material_parse_offset="<<diagnostic.riderResources.material.document.byteOffset
                <<" rider_material_parse_pc="<<diagnostic.riderResources.material.document.sourcePC
                <<" rider_images="<<unsigned(diagnostic.riderResources.resource.error)
                <<" rider_images_image="<<diagnostic.riderResources.resource.image
                <<" rider_font="<<unsigned(diagnostic.riderResources.resource.font.error)
                <<" rider_font_offset="<<diagnostic.riderResources.resource.font.byteOffset
                <<" rider_font_record="<<diagnostic.riderResources.resource.font.record
                <<" rider_font_ascii="<<diagnostic.riderResources.resource.font.ascii
                <<" rider_text="<<unsigned(diagnostic.riderResources.text.error)
                <<" rider_text_line="<<diagnostic.riderResources.text.line
                <<" rider_text_glyph="<<unsigned(diagnostic.riderResources.text.glyph.error)
                <<" rider_text_character="<<diagnostic.riderResources.text.glyph.character
                <<" rider_text_ascii="<<diagnostic.riderResources.text.glyph.ascii
                <<" rider_text_pc="<<diagnostic.riderResources.text.glyph.sourcePC
                <<" rider_exit_text="<<unsigned(diagnostic.riderResources.exitText.status)
                <<" rider_exit_text_pc="<<diagnostic.riderResources.exitText.sourcePC
                <<" rider_exit_parse="<<unsigned(diagnostic.riderResources.exitText.parse.error)
                <<" rider_exit_parse_offset="<<diagnostic.riderResources.exitText.parse.byteOffset
                <<" rider_exit_parse_pc="<<diagnostic.riderResources.exitText.parse.sourcePC
                <<" rider_exit_glyph="<<unsigned(diagnostic.riderResources.exitText.text.error)
                <<" rider_exit_character="<<diagnostic.riderResources.exitText.text.character
                <<" rider_exit_ascii="<<diagnostic.riderResources.exitText.text.ascii
                <<" rider_exit_glyph_pc="<<diagnostic.riderResources.exitText.text.sourcePC
                <<" rider_stats="<<unsigned(diagnostic.riderStats.error)
                <<" rider_binder="<<unsigned(diagnostic.riderBinder.error)
                <<" rider_draw="<<unsigned(diagnostic.rider.draw.error)
                <<" rider_draw_pc="<<diagnostic.rider.draw.sourcePC
                <<" rider_draw_surface="<<diagnostic.rider.draw.geometrySurface
                <<" rider_draw_component="<<diagnostic.rider.draw.geometryComponent
                <<" rider_draw_colors="<<diagnostic.rider.draw.geometryStoredColors
                <<" rider_draw_geometry="<<unsigned(diagnostic.rider.draw.geometryStatus)
                <<" rider_draw_material="<<unsigned(diagnostic.rider.draw.materialDetail.error)
                <<" rider_draw_material_index="<<diagnostic.rider.draw.material
                <<" rider_draw_material_pass="<<diagnostic.rider.draw.materialDetail.pass
                <<" rider_draw_widget="<<diagnostic.rider.draw.widget
                <<" rider_draw_field="<<diagnostic.rider.draw.materialDetail.field
                <<" rider_dynamic="<<unsigned(diagnostic.rider.dynamic.error)
                <<" rider_dynamic_widget="<<diagnostic.rider.dynamic.widget
                <<" rider_pod="<<unsigned(diagnostic.rider.dynamic.pod.error)
                <<" rider_pod_pc="<<diagnostic.rider.dynamic.pod.sourcePC
                <<" rider_pod_text="<<unsigned(diagnostic.rider.dynamic.pod.text.error)
                <<" rider_pod_text_offset="<<diagnostic.rider.dynamic.pod.text.byteOffset
                <<" rider_pod_hierarchy="<<unsigned(diagnostic.rider.dynamic.pod.hierarchy.code)
                <<" rider_pod_hierarchy_offset="<<diagnostic.rider.dynamic.pod.hierarchy.byteOffset
                <<" rider_pod_geometry="<<diagnostic.rider.dynamic.pod.geometry.code
                <<" rider_pod_geometry_offset="<<diagnostic.rider.dynamic.pod.geometry.offset
                <<" rider_pod_geometry_detail="<<diagnostic.rider.dynamic.pod.geometry.detail
                <<" rider_character="<<unsigned(diagnostic.rider.dynamic.character.error)
                <<" rider_character_pc="<<diagnostic.rider.dynamic.character.sourcePC
                <<" rider_character_motion="<<diagnostic.rider.dynamic.character.motionOrdinal
                <<" rider_character_model="<<unsigned(diagnostic.rider.dynamic.character.model.code)
                <<" rider_character_model_offset="<<diagnostic.rider.dynamic.character.model.byteOffset
                <<" rider_character_binding="<<unsigned(diagnostic.rider.dynamic.character.binding.error)
                <<" rider_character_binding_offset="<<diagnostic.rider.dynamic.character.binding.offset
                <<" rider_character_geometry="<<diagnostic.rider.dynamic.character.geometry.code
                <<" rider_character_geometry_offset="<<diagnostic.rider.dynamic.character.geometry.offset
                <<" rider_backend="<<unsigned(diagnostic.rider.backend.error)
                <<" gpu="<<unsigned(diagnostic.gpu.error)
                <<" gpu_operation="<<(diagnostic.gpu.operation?diagnostic.gpu.operation:"")
                <<" gpu_texture="<<diagnostic.gpu.texture
                <<" gpu_bank_texture_count="<<diagnostic.gpu.bankTextureCount
                <<" gpu_bank_rgba_bytes="<<diagnostic.gpu.bankRgbaBytes
                <<" gpu_maximum_bytes="<<diagnostic.gpu.maximumGpuBytes
                <<" gpu_bank_maximum_bytes="<<diagnostic.gpu.maximumBankRgbaBytes
                <<" gpu_validated_prefix_bytes="<<diagnostic.gpu.validatedPrefixRgbaBytes
                <<" gpu_candidate_bytes="<<diagnostic.gpu.candidateRgbaBytes
                <<" gpu_candidate_identity="<<diagnostic.gpu.candidateIdentity
                <<" gpu_candidate_width="<<diagnostic.gpu.candidateWidth
                <<" gpu_candidate_height="<<diagnostic.gpu.candidateHeight
                <<" renderer="<<unsigned(diagnostic.renderer.error)
                <<"; no destination or gameplay entered.\n";
        }else if(status!=Status::ok&&status!=Status::noChange&&status!=Status::routeAccepted)fail("input");
    }
    void reportElapsedFailure(const original_ui_elapsed50LH::Interval& interval,std::uint64_t before,
        std::uint64_t now,std::uint64_t pending,StageClock::time_point observedNow) const {
        const auto snapshot=coordinator.snapshot();
        std::cerr<<"original_ui_interval_failure {\"error\":"<<unsigned(interval.error)
            <<",\"frame\":"<<frameCount<<",\"accepted_update_sequence\":"<<snapshot.updates
            <<",\"screen\":"<<unsigned(snapshot.route.screen)<<",\"page\":"<<unsigned(snapshot.route.page)
            <<",\"course\":"<<snapshot.course.selectedCourse44<<",\"planet\":"<<snapshot.course.selectedPlanet40
            <<",\"course_refresh_updates\":"<<snapshot.course.deferred14D0
            <<",\"update_clock_domain\":\"monotonicNow50DU_relative\",\"stage_clock_domain\":\"steady_clock_since_epoch\""
            <<",\"host_sample_observed_steady_nanoseconds\":"<<std::chrono::duration_cast<std::chrono::nanoseconds>(observedNow.time_since_epoch()).count()
            <<",\"update_begin_nanoseconds\":"<<before<<",\"update_end_nanoseconds\":"<<now
            <<",\"wall_nanoseconds\":"<<interval.wallNanoseconds<<",\"pending_cold_nanoseconds\":"<<pending
            <<",\"excluded_cold_nanoseconds\":"<<interval.excludedColdNanoseconds
            <<",\"active_nanoseconds\":"<<interval.activeNanoseconds<<",\"recent_stages\":[";
        const auto oldest=(nextStageObservation+stageHistory.size()-stageObservationCount)%stageHistory.size();
        for(std::size_t i=0;i<stageObservationCount;++i){const auto& stage=stageHistory[(oldest+i)%stageHistory.size()];
            if(i)std::cerr<<',';
            std::cerr<<"{\"stage\":\""<<stage.name<<"\",\"frame\":"<<stage.frame<<",\"sequence\":"<<stage.sequence
                <<",\"screen\":"<<unsigned(stage.screen)<<",\"page\":"<<unsigned(stage.page)
                <<",\"steady_begin_nanoseconds\":"<<std::chrono::duration_cast<std::chrono::nanoseconds>(stage.begin.time_since_epoch()).count()
                <<",\"steady_end_nanoseconds\":"<<std::chrono::duration_cast<std::chrono::nanoseconds>(stage.end.time_since_epoch()).count()
                <<",\"wall_nanoseconds\":"<<std::chrono::duration_cast<std::chrono::nanoseconds>(stage.end-stage.begin).count()
                <<",\"okay\":"<<(stage.okay?"true":"false")<<'}';
        }
        std::cerr<<"]}\n";
    }
    std::uint64_t acceptedElapsed(std::uint64_t now) {
        // Observation paired immediately after the caller's relative host
        // sample. It labels clock domains; it never advances either clock.
        const auto observedNow=StageClock::now();
        const auto before=lastUpdateNanoseconds,pending=pendingUpdateColdNanoseconds;
        const auto interval=original_ui_elapsed50LH::consume(lastUpdateNanoseconds,now,pendingUpdateColdNanoseconds);
        if(interval.error!=original_ui_elapsed50LH::Error::none)reportElapsedFailure(interval,before,now,pending,observedNow);
        require(interval.error!=original_ui_elapsed50LH::Error::backwards,"original UI update clock moved backwards");
        require(interval.error!=original_ui_elapsed50LH::Error::coldExceedsWall,"UI cold interval exceeds update interval");
        // Native wall accounting is not the source-dispatched simulation factor.
        // Original140C50 preserves raw250, then140C54..8C caps the factor passed
        // through the real root/UI routers. Keep PCM/material time uncapped.
        std::uint64_t active=now;
        if(before){
            require(interval.activeNanoseconds<=std::numeric_limits<std::uint64_t>::max()-sourceDispatchActiveNanoseconds,
                "UI source dispatch active timeline overflow");
            active=sourceDispatchActiveNanoseconds+interval.activeNanoseconds;
        }
        require(sourceDispatchTiming.prepare(active,!before,sourceDispatchSample),
            "UI source dispatch timestamp/factor preparation rejected");
        sourceDispatchActiveNanoseconds=active;
        if(interval.longActiveInterval)
            std::cerr<<"original_ui_long_interval active_nanoseconds="<<interval.activeNanoseconds
                <<" source_delta_milliseconds="<<sourceDispatchSample.deltaMilliseconds
                <<" source_raw250="<<sourceDispatchSample.raw250
                <<" source_update_seconds="<<sourceDispatchSample.updateSeconds
                <<" policy=source_capped_dispatch_pcm_full_interval\n";
        return interval.activeNanoseconds;
    }
    float preparedSourceUpdateSeconds() const noexcept {return sourceDispatchSample.updateSeconds;}
    void commitSourceUpdateTiming() {
        require(sourceDispatchTiming.commit(),"UI source dispatch timing commit rejected");
    }
    rr::platform::OriginalUiFrontendUpdateReceipt50KG update(
        const rr::platform::OriginalUiMainSessionUpdate50KG& in,std::uint64_t elapsedNanoseconds) {
        rr::platform::OriginalUiFrontendUpdateReceipt50KG receipt;
        const auto updateStart=stageStart();
        const bool garageRoot=coordinator.snapshot().route.screen==13;
        const bool sourceAudio=materialApplicationOwner&&materialApplicationOwner->garageAudioBackingOwner_;
        const bool ordinarySourceAudio=sourceAudio&&!garageRoot;
        original_ui_source_timing50LH::PublicationView garageTiming;
        original_ui_source_timing50LH::Sample garageSample;std::uint64_t garagePublication=0;
        if(garageRoot){
            // Root13 owns its real Light/Aural/dispatcher traversal. Capture
            // THIS prepared publication before it commits; do not invoke the
            // ordinary-manager prefix or manufacture another audio sample.
            require(sourceAudio&&nativeRootAudioPhase_==NativeRootAudioPhase::returned&&
                nativeRootAudioPublication_!=std::numeric_limits<std::uint64_t>::max()&&
                lastAudioSequence!=std::numeric_limits<std::uint64_t>::max()&&
                sourceDispatchTiming.retainPublication(garageTiming)&&
                garageTiming.sameOwner(nativeRootAudioTiming_)&&
                garageTiming.readPrepared(garageSample,garagePublication)&&
                garagePublication==nativeRootAudioPublication_+1&&
                garageSample.updateSeconds==in.elapsedSeconds,
                "Garage host lost its exact preceding Root/audio/timing publication");
        }
        std::string rootAudioError;
        if(ordinarySourceAudio&&!materialApplicationOwner->beginNativeFrontendRootAudio140CE0(*this,in,diagnostic,rootAudioError)){
            reportStage("source_update",updateStart,false);throw std::runtime_error(rootAudioError);
        }
        bool updated=coordinator.update(in,receipt,diagnostic)==Status::ok&&receipt.accepted&&receipt.sequence==lastAudioSequence+1;
        if(ordinarySourceAudio&&updated)
            updated=materialApplicationOwner->finishNativeFrontendRootAudio140CFC(*this,receipt,diagnostic,rootAudioError);
        if(updated){
            if(garageRoot){
                original_ui_source_timing50LH::Sample stillPrepared;std::uint64_t pending=0;
                updated=receipt.source.screen==13&&receipt.source.page==1&&receipt.sourceTimingCommitted&&
                    receipt.sourcePublication==garagePublication&&
                    receipt.update.elapsedSeconds==garageSample.updateSeconds&&receipt.update.paused==in.paused&&
                    !garageTiming.readPrepared(stillPrepared,pending);
            }else updated=!receipt.sourceTimingCommitted&&!receipt.sourcePublication&&receipt.source.screen!=13;
            if(!updated)rootAudioError="Native frontend returned an inconsistent Root timing/audio receipt; source effects retained";
        }
        reportStage("source_update",updateStart,updated);
        accountBankPublication(receipt.coldInterval,receipt.bankDrain,updated);
        if(!updated){
            if(!rootAudioError.empty())throw std::runtime_error(rootAudioError);
            fail("update");
        }
        // One PCM advance for this exact accepted source update; never for
        // loading, rejected updates, or a second screen's copied clock.
        rr::platform::AudioOutputDiagnostic50HD outputDiagnostic;
        const auto audioStart=stageStart();
        const bool advanced=audioOutput.advanceAcceptedNanoseconds(elapsedNanoseconds,outputDiagnostic);
        reportStage("pcm_advance",audioStart,advanced);
        if(!advanced)
            throw std::runtime_error(std::string("original UI PCM advance failed: ")+outputDiagnostic.detail.data());
        lastAudioSequence=receipt.sequence;
        if(garageRoot){
            // PCM above is still required: Root267BE0 drains controls, not
            // samples. Observe its actual completed publication only after
            // this one PCM drain succeeds. Preserve chronology for a later
            // ordinary manager instead of resetting its prefix cursor.
            nativeRootAudioTiming_=garageTiming;nativeRootAudioPublication_=garagePublication;
            nativeRootAudioExpectedUpdate_=receipt.sequence;nativeRootAudioPhase_=NativeRootAudioPhase::returned;
            nativeRootAudioSourcePC_=0x140D60;
        }
        return receipt;
    }
    void render(const rr::platform::OriginalUiMainFrameClock50KG& clock,
        rr::platform::VehicleWindowGl50DS& renderer,std::int32_t width,std::int32_t height) {
        const auto buildStart=stageStart();
        const bool rendered=coordinator.render(clock,diagnostic)==Status::ok;
        reportStage("source_build",buildStart,rendered);
        if(!rendered)fail("render");
        const auto submitStart=stageStart();
        const bool submitted=coordinator.submit(renderer,width,height,diagnostic)==Status::ok;
        reportStage("native_submit",submitStart,submitted,&diagnostic.renderer);
        if(!submitted)fail("submission");
        const auto submittedState=coordinator.snapshot();submittedScreen=submittedState.route.screen;
        submittedGaragePublication=submittedScreen==13&&submittedState.route.page==1&&
            submittedState.garageTargetPublished?submittedState.garagePublication:0;
        submittedPublication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
        submittedDraws=diagnostic.renderer.originalUiDraws;submittedQuads=diagnostic.renderer.originalUiQuads;
        submittedTriangles=diagnostic.renderer.originalUiSourceTriangles;
        submittedTextureBarriers=diagnostic.renderer.originalUiTextureBarriers;
        submittedCoherentFramebufferFetch=diagnostic.renderer.originalUiCoherentFramebufferFetch;
        sourceFrameGeneration=diagnostic.renderer.originalUiSourceFrameGeneration;
        sourceTargetGeneration=diagnostic.renderer.originalUiSourceTargetGeneration;
    }
    void commitDeferred(rr::platform::VehicleWindowGl50DS& renderer) {
        const auto deferredStart=stageStart();
        rr::platform::OriginalUiFrontendDeferredReceipt50KG receipt;
        const auto status=coordinator.commitDeferred(renderer,receipt,diagnostic);
        const bool okay=status==Status::ok||status==Status::noChange||status==Status::courseLaunchReady;
        reportStage("deferred_commit",deferredStart,okay);
        accountBankPublication(receipt.coldInterval,receipt.bankDrain,okay);
        if(status==Status::courseLaunchReady){
            require(materialApplicationOwner,"Course material directory owner missing");
            materialApplicationOwner->enterGameplayMaterialDirectory17D7A0();
            imagePublished=false;return;
        }
        if(status==Status::ok){imagePublished=false;std::cerr<<"original_ui_screen_activated "<<unsigned(coordinator.snapshot().route.screen)<<'\n';}
        else if(status!=Status::noChange)fail("deferred activation");
    }
    void requestQuit() noexcept {coordinator.requestQuit();}
    bool closeUi() noexcept {
        if(uploadLifetime_&&(uploadLifetime_->active_||
            uploadLifetime_->thread_!=std::this_thread::get_id()))return false;
        const auto status=coordinator.close(diagnostic);
        if(status!=Status::ok&&status!=Status::noChange)return false;
        if(uploadLifetime_){uploadLifetime_->retired_=true;uploadLifetime_.reset();}
        return true;
    }
    bool closeAudio() noexcept {
        rr::platform::AudioOutputDiagnostic50HD d;const bool okay=audioOutput.close(d);
        if(!okay)std::cerr<<"original UI audio close error="<<unsigned(d.error)<<'\n';return okay;
    }
    const rr::game::OriginalUiNavigation50KG* navigation() const noexcept {
        const auto route=coordinator.snapshot().route;
        if(route.screen==4&&route.page==1){
            const auto* resources=coordinator.mainResources();
            return resources&&resources->manager()?resources->manager()->navigation(1):nullptr;
        }
        if(route.screen==6&&route.page==1){
            const auto* resources=coordinator.courseResources();
            return resources&&resources->widgets()?resources->widgets()->navigation():nullptr;
        }
        if(route.screen==9&&route.page==1){
            const auto* session=coordinator.tournamentSession();return session?session->navigation():nullptr;
        }
        if(route.screen==11&&route.page==1){
            const auto* session=coordinator.hallSession();const auto* state=session?session->navigationState():nullptr;
            return state?&state->navigation:nullptr;
        }
        if(route.screen==12){
            const auto* resources=coordinator.optionsResources();
            const auto* widgets=resources?resources->widgets():nullptr;
            if(!widgets)return nullptr;
            if(route.page==1)return widgets->navigation();
            if(route.page==5)return widgets->memoryCardNavigation();
            if(route.page==6&&widgets->creditsState())return &widgets->creditsState()->navigation;
            if(route.page==4){const auto* state=widgets->gameOptionsState();return state?&state->navigation:nullptr;}
            if(route.page==2||route.page==3){const auto* session=coordinator.optionsSession();
                const auto* state=session?session->controlsState(route.page-2):nullptr;return state?&state->navigation:nullptr;}
        }
        return nullptr;
    }
    void report(const char* reason,const Application& application,std::uint64_t worldTicks,std::uint64_t mixedFrames) const {
        const auto snapshot=coordinator.snapshot();const bool main=snapshot.route.screen==4;
        // Observation only: do not force a Rider-readable mode while Main owns
        // mode4. The public read-only snapshot legitimately returns false then.
        const auto reportRiderProfile=[&]() {
            if(!application.riderProfile)return;
            rr::game::OriginalUiRiderSelectionProfile50KG profile;
            const bool readable=application.riderProfile->selectionSnapshot(profile);
            const auto stamp=application.riderProfile->stamp();
            const auto& common=*application.profile;
            const auto& settings=*application.gameSettingsProfile;
            std::cerr<<",\"rider_profile_initialized\":"<<(application.riderProfile->initialized()?"true":"false")
                <<",\"rider_profile_readable\":"<<(readable?"true":"false")
                <<",\"rider_profile_common_link\":"<<(application.riderProfile->commonProfile()==application.profile.get()?"true":"false")
                <<",\"rider_profile_generation\":"<<stamp.generation<<",\"rider_profile_mode\":"<<stamp.mode34
                <<",\"rider_profile_remembered\":";
            if(readable)std::cerr<<'['<<profile.remembered6F0<<','<<profile.remembered6F4<<','
                <<profile.remembered6F8<<','<<profile.remembered6FC<<']';
            else std::cerr<<"null";
            std::cerr<<",\"main_profile_mode\":"<<common.mode34<<",\"main_profile_previous_mode\":"<<common.previousMode38
                <<",\"main_profile_saved_slot\":"<<common.savedSlot6E0
                <<",\"main_profile_availability\":"<<unsigned(common.availability11018)
                <<",\"main_profile_flags\":"<<unsigned(common.flags11054)
                <<",\"main_profile_player_bits\":"<<unsigned(common.playerBits1125C)
                <<",\"application_game_profile\":{\"sound_mode\":"<<settings.soundMode1101C
                <<",\"volumes\":["<<settings.volume11020[0]<<','<<settings.volume11020[1]<<','
                <<settings.volume11020[2]<<','<<settings.volume11020[3]<<']'
                <<",\"flags\":"<<unsigned(settings.flags11050)<<",\"saved_group\":"<<settings.savedGroup6E8
                <<",\"sound_revision\":"<<application.soundMode->revision()<<'}'
                <<",\"light_manager_generation\":"<<application.lightManager->managerGeneration()
                <<",\"light_cache_state\":"<<unsigned(application.lightManager->cacheState());
        };
        if(snapshot.route.screen==13){
            const auto& aperture=snapshot.garageAperture;
            const bool accepted=snapshot.route.page==1&&snapshot.frameSubmitted&&snapshot.garageTargetPublished&&
                snapshot.garageRootReturned&&snapshot.garageTimingCommitted&&submittedScreen==13&&
                snapshot.garagePublication!=0&&submittedGaragePublication==snapshot.garagePublication;
            std::cerr<<"original_garage_state {\"reason\":\""<<reason<<"\",\"screen\":13,\"page\":"<<unsigned(snapshot.route.page)
                <<",\"presentation\":\"native_source_rgba_target\",\"switches\":"<<snapshot.switches
                <<",\"updates\":"<<snapshot.updates<<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<snapshot.submissions
                <<",\"loaded\":"<<(snapshot.garageLoaded?"true":"false")<<",\"selected_stat\":"<<(snapshot.garageLoaded?std::to_string(snapshot.garageSelectedStat):"null")
                <<",\"source_publication\":"<<snapshot.garagePublication
                <<",\"root_returned\":"<<(snapshot.garageRootReturned?"true":"false")
                <<",\"timing_committed\":"<<(snapshot.garageTimingCommitted?"true":"false")
                <<",\"target_published\":"<<(snapshot.garageTargetPublished?"true":"false")
                <<",\"accepted_native_target\":"<<(accepted?"true":"false")
                <<",\"lifecycle\":"<<snapshot.garageManagerLifecycle<<",\"phase\":"<<snapshot.garageManagerTransition
                <<",\"aperture_bound\":"<<(aperture.bound?"true":"false")<<",\"aperture_state\":"<<unsigned(aperture.state)
                <<",\"aperture_top\":"<<aperture.top284<<",\"aperture_bottom\":"<<aperture.bottom28C
                <<",\"aperture_input_enabled\":"<<(aperture.inputEnabled268?"true":"false")
                <<",\"selected_group\":null,\"selected_slot\":null,\"gpu_publication\":null"
                <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount
                <<",\"source_rng\":"<<application.state->rawRandom308030<<",\"source_milliseconds\":"<<sourceMilliseconds
                <<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0)
                <<",\"audio_source_mixed_frames\":"<<audioOutput.sourceMixedFrames()
                <<",\"audio_output_serial\":"<<audioOutput.retainedOutputSerial()
                <<",\"audio_mixed_frames\":"<<audioOutput.info().mixedFrames
                <<",\"world_ticks\":"<<worldTicks<<",\"mixed_frames\":"<<mixedFrames;
            reportRiderProfile();std::cerr<<"}\n";return;
        }
        if(snapshot.route.screen==6){
            require(snapshot.route.page==1&&coordinator.courseSession(),"original Course session report unavailable");
            const auto& course=snapshot.course;const auto& aperture=course.aperture;
            const auto publication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
            const bool accepted=submittedScreen==6&&submittedPublication==publication&&snapshot.frameSubmitted;
            std::cerr<<"original_course_state {\"reason\":\""<<reason<<"\",\"screen\":6,\"page\":1"
                <<",\"presentation\":\"native_host_rgb\",\"switches\":"<<snapshot.switches
                <<",\"gpu_publication\":"<<publication<<",\"updates\":"<<snapshot.updates
                <<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<snapshot.submissions
                <<",\"mode34\":"<<course.mode34<<",\"tier14E8\":"<<course.tier14E8
                <<",\"entry_writes_laps_entrants\":"<<(course.mode34==0?"true":"false")
                <<",\"course_slot\":"<<course.courseSlot14E4<<",\"subtype_slot\":"<<course.subtypeSlot14F4
                <<",\"selected_planet\":"<<course.selectedPlanet40<<",\"selected_course_index\":"<<course.selectedCourse44
                <<",\"selection_refresh_updates\":"<<course.deferred14D0
                <<",\"laps\":"<<course.laps14EC<<",\"entrants\":"<<course.entrants14F0
                <<",\"input_shown\":"<<(course.inputContainerShown?"true":"false")
                <<",\"popup_enabled\":"<<(course.popupEnabled438?"true":"false")
                <<",\"prompt_mask\":"<<course.promptMask42C
                <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount
                <<",\"lifecycle\":"<<unsigned(snapshot.transition.lifecycle)<<",\"phase\":"<<unsigned(snapshot.transition.phase)
                <<",\"requested_screen\":"<<unsigned(snapshot.transition.requested.screen)
                <<",\"requested_page\":"<<unsigned(snapshot.transition.requested.page)
                <<",\"aperture_state\":"<<unsigned(aperture.state)
                <<",\"aperture_top\":"<<aperture.top284<<",\"aperture_bottom\":"<<aperture.bottom28C
                <<",\"aperture_input_enabled\":"<<(aperture.inputEnabled268?"true":"false")
                <<",\"accepted_native_frame\":"<<(accepted?"true":"false")
                <<",\"quads\":"<<(accepted?submittedQuads:0)<<",\"draws\":"<<(accepted?submittedDraws:0)
                <<",\"source_rng\":"<<application.state->rawRandom308030<<",\"source_milliseconds\":"<<sourceMilliseconds
                <<",\"audio_muted\":"<<(audioOutput.enabled()?"false":"true")
                <<",\"audio_suppressed\":"<<course.audio.suppressedByMutedMode
                <<",\"audio_submitted\":"<<course.audio.submitted<<",\"audio_failed\":"<<course.audio.failed
                <<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0)
                <<",\"audio_source_mixed_frames\":"<<audioOutput.sourceMixedFrames()
                <<",\"audio_source_frame_origin\":"<<audioOutput.sourceFrameOrigin()
                <<",\"audio_output_source_mixed_frames\":"<<audioOutput.outputSourceMixedFrames()
                <<",\"audio_output_serial\":"<<audioOutput.retainedOutputSerial()
                <<",\"audio_mixed_frames\":"<<audioOutput.info().mixedFrames
                <<",\"audio_captured_frames\":"<<audioOutput.info().capturedFrames
                <<",\"audio_rate\":"<<audioOutput.info().sampleRate
                <<",\"audio_accepted_nanoseconds\":"<<audioOutput.acceptedNanoseconds()
                <<",\"audio_device_status\":\""<<rr::platform::audioDeviceStatusName50HD(audioOutput.info().deviceStatus)<<"\""
                <<",\"world_ticks\":"<<worldTicks<<",\"mixed_frames\":"<<mixedFrames;
            reportRiderProfile();std::cerr<<"}\n";return;
        }
        if(snapshot.route.screen==5){
            require(snapshot.route.page==1&&coordinator.riderSession(),"original Rider session report unavailable");
            const auto& rider=snapshot.rider;const auto& aperture=rider.aperture;
            const auto publication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
            const bool accepted=submittedScreen==5&&submittedPublication==publication&&sourceFrameGeneration!=0;
            std::cerr<<"original_rider_state {\"reason\":\""<<reason<<"\",\"screen\":5,\"page\":1"
                <<",\"switches\":"<<snapshot.switches<<",\"gpu_publication\":"<<publication
                <<",\"updates\":"<<snapshot.updates<<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<snapshot.submissions
                <<",\"selected_slot\":"<<rider.slot4C8<<",\"selected_rider\":"<<rider.rider4C4
                <<",\"selection_attached\":"<<(rider.selectionAttached?"true":"false")
                <<",\"bank_generation\":"<<rider.bankGeneration<<",\"registry_generation\":"<<rider.registryGeneration
                <<",\"input_shown\":"<<(rider.inputContainerShown?"true":"false")
                <<",\"input_blocked\":"<<(rider.blocked4E0?"true":"false")<<",\"panel_y\":"<<rider.panelY4DC
                <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount
                <<",\"lifecycle\":"<<unsigned(snapshot.transition.lifecycle)<<",\"phase\":"<<unsigned(snapshot.transition.phase)
                <<",\"requested_screen\":"<<unsigned(snapshot.transition.requested.screen)
                <<",\"requested_page\":"<<unsigned(snapshot.transition.requested.page)
                <<",\"prompt_mask\":"<<rider.promptMask42C<<",\"aperture_state\":"<<unsigned(aperture.state)
                <<",\"aperture_top\":"<<aperture.top284<<",\"aperture_bottom\":"<<aperture.bottom28C
                <<",\"aperture_input_enabled\":"<<(aperture.inputEnabled268?"true":"false")
                <<",\"source_rng\":"<<application.state->rawRandom308030<<",\"source_milliseconds\":"<<sourceMilliseconds
                <<",\"accepted_source_frame\":"<<(accepted?"true":"false")
                <<",\"source_frame_generation\":"<<(accepted?sourceFrameGeneration:0)
                <<",\"source_target_generation\":"<<(accepted?sourceTargetGeneration:0)
                <<",\"quads\":"<<(accepted?submittedQuads:0)<<",\"draws\":"<<(accepted?submittedDraws:0)
                <<",\"source_triangles\":"<<(accepted?submittedTriangles:0)
                <<",\"source_coherent_framebuffer_fetch\":"<<(accepted&&submittedCoherentFramebufferFetch?"true":"false")
                <<",\"source_texture_barriers\":"<<(accepted?submittedTextureBarriers:0)
                <<",\"audio_muted\":"<<(audioOutput.enabled()?"false":"true")
                <<",\"audio_suppressed\":"<<rider.audio.suppressedByMutedMode
                <<",\"audio_submitted\":"<<rider.audio.submitted<<",\"audio_failed\":"<<rider.audio.failed
                <<",\"audio_source_gate_suppressed\":"<<rider.audio.suppressedBySourceGate
                <<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0)
                <<",\"audio_source_mixed_frames\":"<<audioOutput.sourceMixedFrames()
                <<",\"audio_source_frame_origin\":"<<audioOutput.sourceFrameOrigin()
                <<",\"audio_output_source_mixed_frames\":"<<audioOutput.outputSourceMixedFrames()
                <<",\"audio_output_serial\":"<<audioOutput.retainedOutputSerial()
                <<",\"audio_mixed_frames\":"<<audioOutput.info().mixedFrames
                <<",\"audio_captured_frames\":"<<audioOutput.info().capturedFrames
                <<",\"audio_rate\":"<<audioOutput.info().sampleRate
                <<",\"audio_accepted_nanoseconds\":"<<audioOutput.acceptedNanoseconds()
                <<",\"audio_clock_microseconds\":"<<audioOutput.info().simulationMicroseconds
                <<",\"audio_remainder_nanoseconds\":"<<audioOutput.remainderNanoseconds()
                <<",\"audio_device_status\":\""<<rr::platform::audioDeviceStatusName50HD(audioOutput.info().deviceStatus)<<"\""
                <<",\"audio_device_consumed_frames\":"<<audioOutput.info().deviceConsumedFrames
                <<",\"audio_device_queued_frames\":"<<audioOutput.info().deviceQueuedFrames
                <<",\"audio_device_underrun_frames\":"<<audioOutput.info().deviceUnderrunFrames
                <<",\"audio_device_overflow_frames\":"<<audioOutput.info().deviceOverflowFrames
                <<",\"world_ticks\":"<<worldTicks<<",\"mixed_frames\":"<<mixedFrames;
            reportRiderProfile();std::cerr<<"}\n";
            return;
        }
        if(snapshot.route.screen==9){
            require(snapshot.route.page==1&&coordinator.tournamentSession(),"original Tournament session report unavailable");
            const auto& tournament=snapshot.tournament;const auto publication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
            const bool accepted=snapshot.frameSubmitted&&submittedScreen==9&&submittedPublication==publication&&sourceFrameGeneration!=0;
            const auto* nav=navigation();require(nav,"original Tournament navigation report unavailable");
            const auto group=nav->controllerGroup[0];
            const auto slot=group>0&&static_cast<std::size_t>(group)<nav->groups.size()?nav->groups[group].selectedSlot:0;
            std::cerr<<"original_tournament_state {\"reason\":\""<<reason<<"\",\"screen\":9,\"page\":1"
                <<",\"career\":"<<tournament.career<<",\"selected_group\":"<<group<<",\"selected_slot\":"<<slot
                <<",\"page_shown\":"<<(tournament.pageShown?"true":"false")<<",\"input_shown\":"<<(tournament.inputContainerShown?"true":"false")
                <<",\"camera_active\":"<<(tournament.cameraActive?"true":"false")<<",\"timer4064\":"<<tournament.timer4064
                <<",\"audio_submitted\":"<<tournament.audio.submitted<<",\"audio_failed\":"<<tournament.audio.failed
                <<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0)
                <<",\"prompt_mask\":"<<tournament.promptMask42C<<",\"gpu_publication\":"<<publication
                <<",\"updates\":"<<snapshot.updates<<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<snapshot.submissions
                <<",\"aperture_state\":"<<unsigned(tournament.aperture.state)
                <<",\"source_rgba_accepted\":"<<(accepted?"true":"false")
                <<",\"source_frame\":"<<sourceFrameGeneration<<",\"source_target\":"<<sourceTargetGeneration
                <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount;
            reportRiderProfile();std::cerr<<"}\n";return;
        }
        if(snapshot.route.screen==11){
            require(snapshot.route.page==1&&coordinator.hallSession(),"original Hall session report unavailable");
            const auto& hall=snapshot.hall;const auto publication=coordinator.gpu()?coordinator.gpu()->publicationToken():0;
            const bool accepted=snapshot.frameSubmitted&&submittedScreen==11&&submittedPublication==publication&&sourceFrameGeneration!=0;
            const auto* nav=navigation();require(nav,"original Hall navigation report unavailable");
            const auto group=nav->controllerGroup[0];
            const auto slot=group>0&&static_cast<std::size_t>(group)<nav->groups.size()?nav->groups[group].selectedSlot:0;
            std::cerr<<"original_hall_state {\"reason\":\""<<reason<<"\",\"screen\":11,\"page\":1"
                <<",\"course_index\":"<<hall.courseIndex<<",\"planet_index\":"<<hall.planetIndex
                <<",\"selected_group\":"<<group<<",\"selected_slot\":"<<slot
                <<",\"audio_submitted\":"<<hall.audio.submitted<<",\"audio_failed\":"<<hall.audio.failed
                <<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0)
                <<",\"deferred1460\":"<<hall.deferred1460<<",\"video_paused\":"<<(hall.videoPaused84?"true":"false")
                <<",\"presentation_pending\":"<<(hall.movementPresentationPending?"true":"false")
                <<",\"prompt_mask\":"<<hall.promptMask42C<<",\"gpu_publication\":"<<publication
                <<",\"updates\":"<<snapshot.updates<<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<snapshot.submissions
                <<",\"aperture_state\":"<<unsigned(hall.aperture.state)
                <<",\"source_rgba_accepted\":"<<(accepted?"true":"false")
                <<",\"source_frame\":"<<sourceFrameGeneration<<",\"source_target\":"<<sourceTargetGeneration
                <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount;
            reportRiderProfile();std::cerr<<"}\n";return;
        }
        const auto* optionsResources=coordinator.optionsResources();
        const auto* nav=navigation();require(nav,"original UI live navigation missing");
        const auto group=nav->controllerGroup[0];
        const auto slot=group>0&&static_cast<std::size_t>(group)<nav->groups.size()?nav->groups[group].selectedSlot:0;
        const auto* gameOptions=snapshot.route.screen==12&&snapshot.route.page==4&&optionsResources?
            optionsResources->widgets()->gameOptionsState():nullptr;
        const auto* mainSession=coordinator.mainSession();
        const auto* fire=main?(mainSession?mainSession->fire():nullptr):(optionsResources?optionsResources->fire():nullptr);
        require(fire,"original UI live particle owner missing");
        const auto aperture=main?snapshot.main.aperture:snapshot.options.aperture;
        const auto audio=main?snapshot.main.audio:snapshot.options.audio;
        const auto* draw=coordinator.draw();const auto audioInfo=audioOutput.info();
        std::cerr<<"original_main_state {\"reason\":\""<<reason<<"\",\"screen\":"<<unsigned(snapshot.route.screen)
            <<",\"page\":"<<unsigned(snapshot.route.page)<<",\"switches\":"<<snapshot.switches
            <<",\"requested_screen\":"<<unsigned(snapshot.transition.requested.screen)
            <<",\"requested_page\":"<<unsigned(snapshot.transition.requested.page)
            <<",\"gpu_publication\":"<<(coordinator.gpu()?coordinator.gpu()->publicationToken():0)
            <<",\"updates\":"<<snapshot.updates<<",\"renders\":"<<snapshot.renders<<",\"submitted\":"<<frameCount
            <<",\"selected_group\":"<<group<<",\"selected_slot\":"<<slot
            <<",\"saved_slot\":"<<(main?application.profile->savedSlot6E0:application.optionsProfile->savedSlot6E4)
            <<",\"saved_group\":"<<application.gameSettingsProfile->savedGroup6E8
            <<",\"game_options_accepts\":"<<snapshot.options.gameOptionsAcceptCount
            <<",\"game_sound_apply_revision\":"<<snapshot.options.lastGameOptionsSoundRevision
            <<",\"application_sound_revision\":"<<application.soundMode->revision()
            <<",\"input_events\":"<<inputCount<<",\"unavailable_destinations\":"<<unavailableCount
            <<",\"source_rng\":"<<application.state->rawRandom308030<<",\"fire_serial\":"<<application.state->fireSerial433970
            <<",\"input_shown\":"<<((main?snapshot.main.inputContainerShown:snapshot.options.inputContainerShown)?"true":"false")
            <<",\"lifecycle\":"<<unsigned(snapshot.transition.lifecycle)<<",\"phase\":"<<unsigned(snapshot.transition.phase)
            <<",\"prompt_mask\":"<<(main?snapshot.main.promptMask42C:snapshot.options.promptMask42C)
            <<",\"aperture_state\":"<<unsigned(aperture.state)
            <<",\"aperture_top\":"<<aperture.top284<<",\"aperture_bottom\":"<<aperture.bottom28C
            <<",\"aperture_input_enabled\":"<<(aperture.inputEnabled268?"true":"false")
            <<",\"particles\":"<<fire->particleCount()<<",\"source_packets\":"<<fire->packetCount()
            <<",\"quads\":"<<(draw?draw->draw.quadCount:0)<<",\"spans\":"<<(draw?draw->draw.spanCount:0)
            <<",\"source_milliseconds\":"<<sourceMilliseconds<<",\"audio_muted\":"<<(audioOutput.enabled()?"false":"true")
            <<",\"audio_suppressed\":"<<audio.suppressedByMutedMode<<",\"audio_submitted\":"<<audio.submitted
            <<",\"audio_source_gate_suppressed\":"<<audio.suppressedBySourceGate<<",\"audio_failed\":"<<audio.failed
            <<",\"audio_source_mixed_frames\":"<<audioOutput.sourceMixedFrames()
                <<",\"audio_source_frame_origin\":"<<audioOutput.sourceFrameOrigin()
                <<",\"audio_output_source_mixed_frames\":"<<audioOutput.outputSourceMixedFrames()
                <<",\"audio_output_serial\":"<<audioOutput.retainedOutputSerial()<<",\"audio_mixed_frames\":"<<audioInfo.mixedFrames
            <<",\"audio_captured_frames\":"<<audioInfo.capturedFrames<<",\"audio_rate\":"<<audioInfo.sampleRate
            <<",\"audio_accepted_nanoseconds\":"<<audioOutput.acceptedNanoseconds()<<",\"audio_clock_microseconds\":"<<audioInfo.simulationMicroseconds
            <<",\"audio_remainder_nanoseconds\":"<<audioOutput.remainderNanoseconds()
            <<",\"audio_device_status\":\""<<rr::platform::audioDeviceStatusName50HD(audioInfo.deviceStatus)<<"\""
            <<",\"audio_device_consumed_frames\":"<<audioInfo.deviceConsumedFrames<<",\"audio_device_queued_frames\":"<<audioInfo.deviceQueuedFrames
            <<",\"audio_device_underrun_frames\":"<<audioInfo.deviceUnderrunFrames<<",\"audio_device_overflow_frames\":"<<audioInfo.deviceOverflowFrames
            <<",\"world_ticks\":"<<worldTicks<<",\"mixed_frames\":"<<mixedFrames;
        const auto& popup=main?snapshot.main.controllerPopup:snapshot.options.controllerPopup;
        std::cerr<<",\"controller_popup_active\":"<<(popup.active3CC?"true":"false")
            <<",\"controller_popup_closing\":"<<(popup.closing3C8?"true":"false")
            <<",\"controller_popup_result\":"<<popup.result3D8<<",\"controller_popup_timer\":"<<popup.timer3C4;
        const auto& saved=*application.gameSettingsProfile;
        std::cerr<<",\"game_profile_sound_mode\":"<<saved.soundMode1101C
            <<",\"game_profile_volumes\":["<<saved.volume11020[0]<<','<<saved.volume11020[1]<<','<<saved.volume11020[2]<<','<<saved.volume11020[3]<<']'
            <<",\"game_profile_flags\":"<<unsigned(saved.flags11050);
        if(gameOptions&&gameOptions->loaded){
            const auto& local=gameOptions->local;
            std::cerr<<",\"game_local_auto_save\":"<<(local.autoSave?"true":"false")
                <<",\"game_local_sound_mode\":"<<local.soundMode
                <<",\"game_local_volumes\":["<<local.volume[0]<<','<<local.volume[1]<<','<<local.volume[2]<<','<<local.volume[3]<<']'
                <<",\"game_local_vertical_split\":"<<(local.verticalSplit?"true":"false")
                <<",\"game_local_vibration\":"<<(local.vibration?"true":"false");
        }
        if(snapshot.route.screen==12&&snapshot.route.page==5){
            const auto* session=coordinator.optionsSession();const auto* card=session?session->memoryCardState():nullptr;
            if(card)std::cerr<<",\"memory_card_dialog_state\":"<<card->dialogState30
                <<",\"memory_card_active\":"<<(card->dialogActive?"true":"false")
                <<",\"memory_card_closing\":"<<(card->closing3AC?"true":"false")
                <<",\"memory_card_complete\":"<<(card->completed3A4?"true":"false")
                <<",\"memory_card_timer\":"<<card->timer398
                <<",\"memory_card_storage_supported\":false";
        }
        if(snapshot.route.screen==12&&snapshot.route.page==6&&optionsResources){
            const auto* credits=optionsResources->widgets()->creditsState();
            if(credits)std::cerr<<",\"credits_slot\":"<<credits->navigation.groups[1].selectedSlot
                <<",\"credits_phase\":"<<unsigned(credits->phase7D4)
                <<",\"credits_remaining\":"<<credits->remaining7D8
                <<",\"credits_input_flags\":"<<credits->inputFlags42C;
        }
        if(application.controlsProfile){
            std::cerr<<",\"controls_accepts\":"<<snapshot.options.controlsAcceptCount
                <<",\"controls_saved_presets\":["<<application.controlsProfile->slots[0].preset11048<<','
                <<application.controlsProfile->slots[1].preset11048<<']';
            if(snapshot.route.screen==12&&(snapshot.route.page==2||snapshot.route.page==3)){
                const auto* session=coordinator.optionsSession();
                const auto* control=session?session->controlsState(snapshot.route.page-2):nullptr;
                if(control)std::cerr<<",\"controls_local_preset\":"<<control->preset4D8
                    <<",\"controls_local_configuration\":"<<control->configuration4D4
                    <<",\"controls_front_view\":"<<(control->frontView4CC?"true":"false")
                    <<",\"controls_model_blocked\":"<<(control->modelBlocked4D0?"true":"false");
            }
        }
        if(application.riderProfile)std::cerr<<",\"audio_music_song\":"<<(audioOutput.transport()?audioOutput.transport()->activeMusicSong():0);
        reportRiderProfile();std::cerr<<"}\n";
    }
};
}
