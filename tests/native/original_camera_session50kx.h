#pragma once
#include <new>
#include <bit>
#include <cfenv>
#include <cstdio>
#include <xmmintrin.h>
#include "../../src/assets/original_ta_cut_authoring_candidate.h"
#include "../../src/game/original_ta_cut_resource_candidate.h"
#include <optional>
#include "original_replay_tv_camera50pd.h"
#include "source_grid50ig_fixture.h"
#include "source_grid50gb_fixture.h"
#include "source_grid50tc_fixture.h"
#include "source_grid50tb_fixture.h"
#include "source_grid50mc_fixture.h"
#include <string_view>
#include "../../platform/linux/native/race_audio_groups50ic.h"
#include "../../src/assets/original_ta_intro_authoring_candidate.h"
#include "../../src/game/pod_speech50hg.h"
#include "../../src/game/original_racer_diffuse_camera50la.h"
#include "../../src/game/track_zone_visibility50zv.h"
#include "track_navigation_view50tc.h"
#include "track_navigation_view50tb.h"
#include "track_navigation_view50gb.h"
#include "track_navigation_view50mc.h"
namespace original_subtype3_record { class Owner; }
namespace original_cable_selection50LA { class Binding; }
namespace original_race_hud_scope50LA { class Registration; }
namespace race_music50IE { class Owner; }
namespace source_announcer50IE { class Owner; }
namespace gb_particle_render50GB {class Owner;}
namespace mc_particle_render50MC {class Owner;}
namespace rr::game { class OriginalDamageObservation50LA; struct OriginalRaceF8Diagnostic50LA; }
namespace rr::platform { struct VehicleWindowFrame50DS; }
// Concrete original-AI / one-Player native session adapter. Intended install: tests/native.
// All camera/scene/selector state stays OUTSIDE the sealed50HN racer bank.
// This is an explicit native root policy, not recovered retail root scheduling.
#include "native_racer_bank50hn_fixture.h"
#include "native_racer_rig50hw_fixture.h"
#include "native_racer_model_lifetime50la_fixture.h"
#include "original_racer_cable_allocation50la.h"
#include "race_session50hm_fixture.h"
#include "original_camera_async_fade50kx.h"
#include "original_follow_query_bridge50kx.h"
#include "original_camera_cycle_audio50kx.h"
#include "../../src/game/boost_controls50fk.h"
#include "../../src/game/original_follow_race_frame.h"
#include "../../src/game/original_camera_mode_input.h"
#include "../../src/game/ai_runtime50iv.h"
#include "../../src/game/control_presets49d.h"
#include "../../src/game/frame_timing49i.h"
#include "../../src/game/original_camera_start_native.h"
#include "../../src/game/original_race_start_overlay50hu.h"
#include "../../src/game/original_camera_start_prelude.h"
#include "../../src/game/original_camera_visibility_join.h"
#include "../../src/game/original_attached_race_frame.h"
#include "../../src/game/original_human_finish50kg.h"
#include "../../src/game/original_finish_camera50kg.h"
#include "../../src/game/original_retirement_camera50kg.h"
#include "../../src/game/original_race_hud50kg.h"
#include "../../src/game/original_race_hud_blink50la.h"
#include "../../src/game/original_application_animation_clock50kg.h"
#include "../../src/game/undertrack_reposition50et.h"
#include "../../src/game/track_navigation50eu.h"
#include "../../src/game/original_racer_entity_cold50la.h"
#include "../../src/game/original_racer_world_borrow50la.h"
#include "../../src/game/original_racer_animation_cold50la.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include <array>
#include <cstring>
#include <limits>
#include <string>

namespace ra_source_camera50LA { class OwnershipBridge; }
namespace ra_cold_roster50LA { class FreshExecutionArena; }
namespace ra_cold_aggregate50LA { class Owner; }
namespace gc_original_world50LM { class Owner; }
namespace ga_original_world50LH { class Owner; }
namespace sa_original_world50LM { class Owner; }
namespace ma_original_world50LM { class Owner; }
namespace mb_original_world50LM { class Owner; }
namespace sc_original_world50LM { class Owner; }
namespace gb_original_world50LM { class Owner; }
namespace mc_cold_sources50LA { class Owner; }
namespace tb_original_world50LM { class Owner; }
namespace mc_original_world50LM { class Owner; }
namespace rc_original_world50LM { class Owner; }
namespace tc_original_world50LM { class Owner; }
namespace rr::game { class OriginalRaceConstructor50LA; class OriginalFrontendCameraCold50LA; }
namespace original_time_stats_adapter50LA { class Owner; }
namespace original_speed_boost_adapter50LA { class Owner; }
namespace original_position_stats_adapter50LA { class Owner; }
namespace original_pause_observation50LA { class Owner; }
namespace interactive_loop50DU { struct Context; }
namespace original_main_entry50KG { struct Application; class RaceDisplayLifetime50LA; class NativeF8BarPrefix50LA; }
namespace original_camera_session50KX {
// Explicit source course identity, never a TA default for an unknown route.
inline std::string_view introTrackId(rr::game::OriginalCameraStartCourse course)noexcept {
 using C=rr::game::OriginalCameraStartCourse;
 switch(course){
 case C::ta:return "TA";case C::ra:return "RA";case C::gc:return "GC";case C::ga:return "GA";case C::sa:return "SA";case C::ma:return "MA";case C::mb:return "MB";case C::sc:return "SC";case C::gb:return "GB";case C::rc:return "RC";case C::tc:return "TC";case C::mc:return "MC";case C::tb:return "TB";default:return {};

 }
}

using namespace rr::game;
using Bank=native_racer50HN::Bank;
using Phase=VehicleFramePhase49W;
namespace lighting=original_gc_gameplay_lighting_candidate;

// Immutable bytes borrowed from the actual permanent selected-pod/course
// owners. CAM/TOB are authenticated by the recovered exact-member catalogs.
// PHY is the unchanged decoded member already authenticated by the pod loader;
// its stable nonzero source identity is NOT an arbitrary parsed-offset receipt.
struct AuthoredInputs {
    const std::uint8_t* cam=nullptr;std::size_t camBytes=0;
    const char* phy=nullptr;std::size_t phyBytes=0;std::uint64_t phyIdentity=0;
    const std::uint8_t* tob=nullptr;std::size_t tobBytes=0;
    OriginalCameraStartCourse course=OriginalCameraStartCourse::unspecified;
    // Borrow of the very same MC source retained by actualRadarMapMc. The
    // source-specific AuthoredStorage issues it; never a generic course flag.
    const mc_cold_sources50LA::Owner* mcSourceIdentity=nullptr;
};
// These are genuine external application/display inputs. Keep one instance
// for the application's display lifetime, and borrow it here. The explicitly
// approved native cold-alpha policy is provided below; source259E00 itself
// leaves3E8 unwritten. Renderer queues/callbacks are never certified by a flag.
struct DisplayInputs {
    OriginalVideoFadeState& video;
    OriginalRendererDispatchFields& rendererDispatch;
    rr::platform::VehicleWindowGl50DS& renderer;
    rr::platform::VehicleWindowDiagnostic50DS& diagnostic;
    std::uint32_t rendererBufferCount0C;
    std::int32_t rendererWidth4,rendererHeight8;
    std::int32_t& fadeManagerA8;
    std::int32_t& fadeDuration98;
    std::int32_t applicationMode22C,application33C;
    std::uint8_t rootFlags228;
    OriginalCameraDrainPolicy drainPolicy;
    OriginalRaceFogFields* trackFog3EC=nullptr;
    // Optional actual Application borrow. Legacy native callers retain their
    // explicit copied policy; consumed source ownership reads its live scalar.
    const std::int32_t* applicationMode22CSource=nullptr;
    std::int32_t currentApplicationMode22C()const noexcept {
        return applicationMode22CSource?*applicationMode22CSource:applicationMode22C;
    }
};
inline bool initializeFreshNativeCameraVideoAlpha0(OriginalVideoFadeState& video) noexcept {
    // Only a fresh native display; never reset an existing fade/history.
    if(video.alpha3E8||video.covered3DC||video.flags404Low6||
       video.toColor3E0!=0x80000000u||video.fromColor3E4||video.targetAlpha3EC!=128||
       video.millisecondsPerStep3F0||video.redSlope3F4||video.greenSlope3F8||
       video.blueSlope3FC||video.lastMilliseconds400)return false;
    video.alpha3E8=0;return true;
}
// Legacy inputs can only borrow an actual Follow owner. The absent variant
// is private to the distinct fresh constructor-only factory, not a readiness flag.
class NativeFollowBorrow final {
    friend class ra_cold_aggregate50LA::Owner;
    original_follow_query_bridge::QueryBridge* owner_=nullptr;
    NativeFollowBorrow() noexcept=default;
public:
    NativeFollowBorrow(original_follow_query_bridge::QueryBridge& owner) noexcept:owner_(&owner){}
    original_follow_query_bridge::QueryBridge* get() const noexcept{return owner_;}
};
// All references are the existing50DU objects. The packed human prefix and
// the AI peers share their real manager, RNG, event tree and original bodies.
struct ActualOwners {
    Bank& bank;
    VehicleFrameState49W& arena;
    VehicleFrameBindings49W& bindings;
    BaseTailState50X& baseTail;
    AiRuntimeState50IV& ai;
    LapTimingFields50ID& racerTiming;
    LapTimingManager50ID& all8Manager;
    race_session50HM::SessionBorrow hostRace;
    SharedRandomOwner50ID& sharedRandom;
    OriginalAttachedGlobals& attachedGlobals;
    HostMonotonicClock50ID& applicationClock;
    ControlPresets49D& preferences;
    const std::uint32_t& launchKind11258;
    const std::uint32_t& configuration34;
    const std::int32_t& application350;
    lighting::Owner& visibility;
    const ZoneLifecycle50CL& collisionWorld;
    LiveZoneMembership50EN& mainMembership690;
    GameplayBlurPending& blur;
    NativeFollowBorrow followQueries;
    BasePoseHistoryState50Y& poseHistory;
    const BoostState50FK& boost;
    original_camera_cycle_audio50KX::Owner& cycleAudio;
    // Actual permanent rig identities, shared with native contact/audio actor
    // ownership. Player identity is never borrowed from an uninitialized AI.
    const std::array<std::uint32_t,8>& racer60;
    const std::uint32_t& humanCount34;
    float& race94;
    const ManagerHeldSources49A& humanHeld;
    OriginalRaceHud50KG* raceHud=nullptr;
    // Genuine ordered racer370/374 collections, rebound to this Session's
    // racer identity by the actual wreck owner. A chariot-only subset is not
    // a complete death-camera debris view.
    const OriginalDeathCameraDebrisView50KG* deathDebris=nullptr;
    // Optional concrete cold-component extension, admitted together only by
    // the current canonical Application/TA material caller. No second table,
    // directory, Entity counter, type registry or replacement vehicle exists.
    std::shared_ptr<OriginalApplicationEntities50LA> applicationEntities;
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> applicationMaterials;
    const std::string* actualMaterialDirectory36B970=nullptr;
    const std::array<std::unique_ptr<native_racer50HW::Rig>,8>* actualRigs=nullptr;
    OriginalTrackParticleCold50LA* actualTrackParticles4A0=nullptr;
    const UndertrackPhysicsDomain50EW* actualHeightDomain=nullptr;
    const std::array<std::unique_ptr<native_racer_model50LA::Owner>,8>* actualRightModels=nullptr;
    const std::array<std::unique_ptr<native_racer_model50LA::Owner>,8>* actualLeftModels=nullptr;
    const std::array<std::unique_ptr<native_racer_model50LA::Owner>,8>* actualChariotModels=nullptr;
    const std::array<std::unique_ptr<native_racer_model50LA::Owner>,8>* actualBottomRightModels=nullptr;
    const std::array<std::unique_ptr<native_racer_model50LA::Owner>,8>* actualBottomLeftModels=nullptr;
    OriginalApplicationAnimationClockView50KG applicationAnimation; //Same Application pause11C owner.
    // Legacy RA factory pins every concrete owner. The private earlier
    // constructor-only route requires absence, not an array of NULL owners.
    const std::array<native_racer50HW::Cables*,8>* actualCables=nullptr;
    const TrackNavigation50EU* actualRadarMapB8=nullptr; // Borrow of the same retained authored map; RA factory only.
    TrackNavigationView50EU actualRadarMapMb{};
    TrackNavigationView50TC actualRadarMapTc{};
    TrackNavigationView50TB actualRadarMapTb{};
    TrackNavigationView50GB actualRadarMapGb{};
    TrackNavigationView50MC actualRadarMapMc{};
    void* startMusicContext=nullptr;
    bool (*startMusicRequest)(void*,std::int32_t,std::int32_t)=nullptr;
    const original_subtype3_record::Owner* recordManager40=nullptr;
    rr::platform::RaceAudioGroups50IC* taIntroAudio=nullptr;
    std::shared_ptr<const rr::assets::OriginalTaIntroAuthoringCandidate> taIntroAuthoring;
    bool (*introMusicFadeRequest)(void*,float)=nullptr;
    // Borrowed from the SAME live native event dispatcher. It supplies its
    // configuration index5C; Bank slot and actionFFC are not substitutes.
    // No original1A02E0/210770 constructor/load completion is implied.
    const PlayerEventState49A* nativeHumanPlayer0=nullptr;

};
enum class Cut : std::uint8_t {
    // Native hold: use real current final-grid/last-published body state while
    // no racer/physics step runs. World animation/UI/audio remain root-owned.
    heldBeforeStart,
    // Source-ordered numeric camera placement after every actual racer prefix
    // and before the first1A8390/219810 physics suffix. Never claim prefixes ran
    // from a callback return: all eight actual49W phase/factor fields are read.
    afterAllRacerPrefixes,
    // Results replay (racer +3C = 1): every racer is Ready and posed from the
    // race78 playback; no racer frame or physics step runs this tick.
    replayPose
};
enum class Status : std::uint8_t {
    complete,sceneChildComplete,framePublished,startHeld,simulationAllowed,
    owner,order,phase,sourcePath,clock,authoring,numeric,visibility,renderer,
    exitPending,videoPending,terminal,abandoned
};
struct Result {
    Status status=Status::owner;std::uint32_t sourcePC=0;
    OriginalCameraStartResult start{};
    OriginalFollowResult follow{};
    OriginalAttachedResult attached{};
    // Failure diagnostics only. Valid flags distinguish this child attempt
    // from receipts retained from earlier successfully committed frames.
    OriginalFollowRaceFrameResult stableFollow{};
    OriginalAttachedRaceFrameResult stableAttached{};
    bool stableFollowAttempted=false,stableAttachedAttempted=false;
    bool cameraFrameMatches=false,cameraCutMatches=false,cameraCutAccepted=false;
    std::uint32_t cameraChildStage=0;
    OriginalRetirementCameraResult50KG retirement{};
    OriginalDeathCameraResult50KG death{};
    OriginalCameraVisibilityJoinResult visibilityJoin{};
    OriginalRacerEntityColdResult50LA entityCold{};
    OriginalRacerEntityColdCleanupResult50LA entityCleanup{};
    OriginalRacerAnimationColdResult50LA animationCold{},animationCleanup{};
    LiveZoneResult50EN spatial{};
};

class Session final {
    friend struct ::interactive_loop50DU::Context;
    friend class rr::game::OriginalSessionF8Intro50LA;
    friend class ::original_race_hud_scope50LA::Registration;
    friend class rr::game::OriginalRaceConstructor50LA; //TA counted Camera34 custody (issuance read only).
    friend struct ::original_main_entry50KG::Application;
    friend class ::original_main_entry50KG::RaceDisplayLifetime50LA;
    friend class ::gb_particle_render50GB::Owner;
    friend class ::mc_particle_render50MC::Owner;
    friend class ::original_cable_selection50LA::Binding;
    // Private same-RA factory verifies actual retirement, never a caller flag.
    friend class ra_source_camera50LA::OwnershipBridge;
    friend class ra_cold_aggregate50LA::Owner;
    // Read-only coherent TimeStats projection; no public gate setters or
    // mutable camera/Entity access are added by this narrow native adapter.
    friend class original_time_stats_adapter50LA::Owner;
    friend class original_speed_boost_adapter50LA::Owner;
    friend class original_position_stats_adapter50LA::Owner;
    friend class original_pause_observation50LA::Owner;
    friend class rr::game::OriginalDamageObservation50LA;
    template<class T> using Read=Bank::ReadHandle<T>;
    // Stable native binding descriptors identify actual (bank,managerSlot)
    // owners. Their addresses are key tokens ONLY because this Session cannot
    // move. The shared arena address never identifies distinct racers.
    struct Identity {
        std::uint32_t managerIndex=0,pod60=0;
        std::uint8_t racerTag=0,mainBodyTag=0,bodyNodeTag=0,modelTag=0,containerTag=0;
    };
    struct Reads {
        Read<std::uint32_t> class58,state40,pod60,managerIndex,retirementCD4,activeAE8,word844;
        Read<bool> aiInitialized,bottomRightPresent,bottomLeftPresent;
        Read<Phase> phase;
        Read<SceneNodeIndex49P> body288,model1B4,container194,rightModel1B8,leftModel1BC;
        Read<SceneNodeIndex49P> bottomRightModel1C0,bottomLeftModel1C4;
        Read<std::uint32_t> nodeCount;
        Read<float> a4,velocityY,frameFactor;
        std::array<Read<std::uint32_t>,5> timers;
        std::array<std::array<Read<float>,16>,vehicleFrameNodeCount49W> local;
        void bind(const ActualOwners& a){
            auto& b=a.bank;auto& s=a.arena;
            class58=b.bindRead(s.fields.vehicle58);state40=b.bindRead(s.bodies.main.vehicleState40);
            pod60=b.bindRead(a.ai.racer60);managerIndex=b.bindRead(a.ai.managerIndexFE0);
            aiInitialized=b.bindRead(a.ai.initialized);retirementCD4=b.bindRead(a.racerTiming.wordCD4);
            phase=b.bindRead(s.phase);nodeCount=b.bindRead(s.sceneNodeCount);
            body288=b.bindRead(a.bindings.animation.bodyScene288);
            model1B4=b.bindRead(a.bindings.lifecycle.model1B4);
            rightModel1B8=b.bindRead(a.bindings.visual.right1B8);
            leftModel1BC=b.bindRead(a.bindings.visual.left1BC);
            bottomRightModel1C0=b.bindRead(a.bindings.visual.bottomRight1C0);
            bottomLeftModel1C4=b.bindRead(a.bindings.visual.bottomLeft1C4);
            bottomRightPresent=b.bindRead(a.bindings.visual.extra1C0Present);
            bottomLeftPresent=b.bindRead(a.bindings.visual.extra1C4Present);
            container194=b.bindRead(a.bindings.visual.container194);
            a4=b.bindRead(s.bodies.main.components.root.worldAngularVelocityA0.v[1]);
            velocityY=b.bindRead(s.bodies.main.components.root.body.velocity.v[1]);
            frameFactor=b.bindRead(s.bodies.main.frameFactor);
            activeAE8=b.bindRead(a.baseTail.activeAE8);word844=b.bindRead(s.fields.vehicle844);
            timers={{b.bindRead(a.baseTail.countAD4[0]),b.bindRead(a.baseTail.countAD4[1]),
                b.bindRead(a.baseTail.timerADC[0]),b.bindRead(a.baseTail.timerADC[1]),b.bindRead(a.baseTail.timerAE4)}};
            for(std::size_t n=0;n<local.size();++n)for(unsigned lane=0;lane<16;++lane)
                local[n][lane]=b.bindRead(s.nodes[n].localC0.v[lane]);
        }
    } reads_;
    enum class FrameStage : std::uint8_t {
        idle,earlyDispatch,earlyTick,earlyMatrix,earlyVisibility,cameraReady,
        cameraDispatch,cameraUpdate,taSkipVideo,retirementHud,retirementPose,exit,exitPose,lateTick,lateMatrix,lateVisibility,
        stableMatrix,published,failed
    } frameStage_=FrameStage::idle;
    ActualOwners actual_;
    // Native typed borrower of this Session, never a source state initializer.
    OriginalSessionF8Intro50LA f8Intro_{*this};
    // Keep the actual F8 owner identity private; existing friends need only this check.
    bool onOwnerThread()const noexcept {
        return f8Intro_.ownerThread_==std::this_thread::get_id();
    }
    // Concrete native human Pause adapter only. The caller supplies its actual
    // Application clock/profile observations and proves their shared owner in
    // the loop. These three words already feed the live original HUD owners.
    // This is not an original Pause Entity/input/audio callback or a save.
    bool observeNativeHumanHud(const ControlPresets49D& expected,
        const OriginalApplicationAnimationClockView50KG& applicationClock,
        std::uint8_t playerBits1125C,std::array<std::uint32_t,4>& out)const noexcept {
        if(!onOwnerThread()||!coldBound_||closed_||abandoned_||!readsBound_||!hasView()||
           frameStage_!=FrameStage::published||pendingDisplayWork()||
           &actual_.preferences!=&expected||!actual_.applicationAnimation.sameOwner(applicationClock)||
           actual_.humanCount34!=1||((playerBits1125C>>5)&3u)!=1||
           !controller_.bound||controller_.binding.bootstrap!=CameraBootstrap::original_single_player||
           controller_.binding.racerClass!=CameraRacerClass::player||
           controller_.binding.managerIndex!=0||(raceFlags1B0_&3u)!=1||
           actual_.launchKind11258<1||actual_.launchKind11258>3)return false;
        try {if(!identitiesCurrent())return false;}catch(...){return false;}
        const auto& slot=actual_.preferences.slots[0];
        if(slot.field10F94>7||slot.field10F9C>7)return false;
        out={{slot.field10F94,slot.field10F9C,slot.field10FA4,actual_.launchKind11258}};
        return true;
    }
    bool adjustNativeHumanHud(ControlPresets49D& expected,
        const OriginalApplicationAnimationClockView50KG& applicationClock,
        std::uint8_t playerBits1125C,const std::array<std::uint32_t,4>& observed,
        std::uint32_t row,bool right)noexcept {
        std::array<std::uint32_t,4> current{};std::uint32_t paused=0;
        if(row>=3||!observeNativeHumanHud(expected,applicationClock,playerBits1125C,current)||
           current!=observed||!applicationClock.readPause11C(paused)||!paused)return false;
        auto& slot=actual_.preferences.slots[0];
        if(row==2){
            slot.field10FA4=slot.field10FA4?0u:1u; //1BDE98 /1BE490.
        }else if(row==1&&actual_.launchKind11258==3){
            // Count1 was read from actual profile1125C above. Retail subtype3
            // toggles NONE/MAP in either direction, including initial ALL->NONE.
            slot.field10F9C=slot.field10F9C?0u:6u; //1BDD4C/54,1BE344/34C.
        }else {
            auto& value=row==0?slot.field10F94:slot.field10F9C;
            // Exact bounded0..7 increment/decrement and source wrap order.
            // Native admission excludes malformed source values before writes.
            value+=right?1u:0xffffffffu;
            if(right){if(value>=8)value=0;}
            else if(std::bit_cast<std::int32_t>(value)<0)value=7;
        }
        // Native Pause has no original1A4B90 dispatch. Keep its existing quiet
        // UI policy: no skipped source-audio success, flush or lifecycle receipt.
        return true;
    }
    // Stable per-racer Entity extensions live outside the multiplexed bank.
    // The bank still owns every existing handling/body/rig field. These cold
    // constructors do not certify its missing full20F390/210770 history.
    OriginalAIDamageMaterialRow50LA aiDamageMaterial_;
    std::int32_t raceMode8C_=0;
    OriginalAIDamageRaceContext50LA aiDamageRace_;
    std::array<OriginalRacerEntityCold50LA,8> racerEntities_{};
    // Private actual RA owner hook. Never installed for standalone sessions.
    ra_source_camera50LA::OwnershipBridge* nativeWreckOwner_=nullptr;
    bool stageNativeRetainedWreckCold()noexcept;
    bool retireNativePlayerSpeechCold()noexcept;
    bool retireNativeRetainedWreckCold()noexcept;

    DisplayInputs* displayInputs_=nullptr;
    // Native observation of the SAME Display0 selector, not a second camera
    // or a strong cycle. No original Camera reference/destructor is inferred.
    std::weak_ptr<original_main_entry50KG::RaceDisplayLifetime50LA> selectedGameplayDisplay_;
    void observeSelectedGameplayCamera26F940()noexcept;
    void invalidateSelectedGameplayCamera50LA()noexcept;
    std::uint64_t generation_=0,frameNumber_=0,completedFrame_=0,publicationCount_=0;
    std::array<Identity,8> identity_{};
    unsigned coldPhase_=0,authoredPhase_=0;
    // One native-only owned preparation interval, not a source Race-load
    // return or a source30-null assertion. Only the actual Application can
    // open it after its retained RA loader tail. It NEVER grants coldBound.
    // Kept after every partial failure/close: a second prepare cannot replay
    // the already reached cut-data/Follow/Death/Attached preparation effects.
    // Separate native constructor admission, never camera or prepared348 readiness.
    std::weak_ptr<ra_cold_roster50LA::FreshExecutionArena> constructorArena_;
    std::shared_ptr<const OriginalRacerWorldBorrow50LA> constructorWorld_;
    bool constructorOnly_=false,constructorPrefixAttempted_=false,constructorPrefixComplete_=false;
    Session(ActualOwners,const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>&,
        std::shared_ptr<const OriginalRacerWorldBorrow50LA>) noexcept;
    static bool constructorArenaMatches(
        const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>&,
        const Bank&,const VehicleFrameState49W&,const VehicleFrameBindings49W&,
        const AiRuntimeState50IV&,const BasePoseHistoryState50Y&,
        const std::array<std::unique_ptr<native_racer50HW::Rig>,8>&) noexcept;
    bool admitConstructorOnly(const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>&,
        std::uint64_t,DisplayInputs&) noexcept;
    bool constructConstructorOnlyPrefixes() noexcept;
    Result closeConstructorOnlyNative(bool abandon) noexcept;
    bool nativeRaOwnedPreparationAttempted_=false;
    bool coldBound_=false,readsBound_=false,countdownInstalled_=false,goPublished_=false,closed_=false,abandoned_=false;
    Cut cut_=Cut::heldBeforeStart;
    float frameDelta_=0;
    // Additional represented race/UI fields owned by this native race session.
    // One real registered camera establishes low2=1. This is RACE1B0, not
    // APPLICATION1B0's unrelated modal/event gate. UI1E4 is a native HUD gate:
    // hidden during intro. Genuine issued RA intro reads canonical199770's
    // resulting low5 here; unsupported legacy routes retain their numeric write.
    // This projection is never authoritative canonical Race/F8 state or time.
    std::uint8_t raceFlags1B0_=1,uiFlags1E4_=1,sceneFlags29_=3;
    std::uint8_t raceEntityFlags29_=1; //Enabled native race entity, distinct from scene flags29.
    OriginalRaceHudBlink50LA raceHudBlink_; //Actual race D8/E8, no separate clock.
    // Immutable native custody, not source camera fields or a readiness grant.
    // Root still owns all Rig/particle borrows; closed/prepared sessions cannot
    // use this record to re-enter the private fresh constructor.
    struct NativeTaIssuance {
        const original_main_entry50KG::Application* issuer=nullptr;
        std::shared_ptr<const rr::assets::OriginalStandardArchiveProvider50LA> provider;
        std::uint64_t generation=0,providerRevision=0,bankGeneration=0;
        std::size_t mount=0,ordinal=0;
        const native_racer50HW::Rig* selectedRig=nullptr;
        OriginalTrackParticleCold50LA* particles=nullptr;
        std::uint32_t selectedPod=0;
        OriginalCameraStartCourse course=OriginalCameraStartCourse::unspecified;
        // GC issuance records the actual consumed race/world and root-owned map.
        // Comparison-only: the root closes Session before releasing these owners.
        const OriginalRaceConstructor50LA* gcRace=nullptr;
        const gc_original_world50LM::Owner* gcWorld=nullptr;
        const TrackNavigation50EU* gcMap=nullptr;
        const OriginalRaceConstructor50LA* saRace=nullptr;
        const sa_original_world50LM::Owner* saWorld=nullptr;
        const TrackNavigation50EU* saMap=nullptr;
        const OriginalRaceConstructor50LA* maRace=nullptr;
        const ma_original_world50LM::Owner* maWorld=nullptr;
        const TrackNavigation50EU* maMap=nullptr;
        // MB keeps its retained radar/start map as a view (no B8 pointer).
        const OriginalRaceConstructor50LA* mbRace=nullptr;
        const mb_original_world50LM::Owner* mbWorld=nullptr;
        TrackNavigationView50EU mbMap{};
        // SC's start-lane/radar map is its own retained authored B8 map.
        const OriginalRaceConstructor50LA* scRace=nullptr;
        const sc_original_world50LM::Owner* scWorld=nullptr;
        const TrackNavigation50EU* scMap=nullptr;
        // GB keeps its retained radar/start map as its own view (no B8 pointer).
        const OriginalRaceConstructor50LA* gbRace=nullptr;
        const gb_original_world50LM::Owner* gbWorld=nullptr;
        TrackNavigationView50GB gbMap{};
        // RC uses its retained B8 navigation map, as SC/GC do.
        const OriginalRaceConstructor50LA* rcRace=nullptr;
        const rc_original_world50LM::Owner* rcWorld=nullptr;
        const TrackNavigation50EU* rcMap=nullptr;
        // TC keeps its retained 268-node map as its own view (no B8 pointer).
        const OriginalRaceConstructor50LA* tcRace=nullptr;
        const tc_original_world50LM::Owner* tcWorld=nullptr;
        TrackNavigationView50TC tcMap{};
        // TB keeps its retained 270-node map as its own view (no B8 pointer).
        const OriginalRaceConstructor50LA* tbRace=nullptr;
        const tb_original_world50LM::Owner* tbWorld=nullptr;
        TrackNavigationView50TB tbMap{};
        // MC: its own 173-node map view, bound to the same MC cold source as its camera assets.
        const OriginalRaceConstructor50LA* mcRace=nullptr;
        const mc_original_world50LM::Owner* mcWorld=nullptr;
        TrackNavigationView50MC mcMap{};
        const mc_cold_sources50LA::Owner* mcSource=nullptr;
        const OriginalRaceConstructor50LA* gaRace=nullptr;
        const ga_original_world50LH::Owner* gaWorld=nullptr;
        const TrackNavigation50EU* gaMap=nullptr;
        std::vector<std::uint8_t> cam,tob,phy;
    };
    std::unique_ptr<const NativeTaIssuance> nativeTaIssuance_;
    // Optional native immutable input custody only. No source child/registry,
    // global443200 value, numeric array or source readiness is represented.
    // Every retained field is owned: cleanup never reads issuer/Rig/particles.
    struct NativeTaCutAuthoringCustody {
        struct Member {
            std::string name;
            std::size_t ordinal=0,bytes=0;
        };
        std::shared_ptr<const rr::assets::OriginalStandardArchiveProvider50LA> provider;
        std::uint64_t generation=0,providerRevision=0,bankGeneration=0;
        std::size_t mount=0;
        std::array<Member,8> members; // Authenticated TOB plus at most seven authored cuts.
        std::unique_ptr<const rr::assets::OriginalTaCutAuthoringCandidate> authoring;
    };
    std::unique_ptr<const NativeTaCutAuthoringCustody> nativeTaCutAuthoring_;
    // One real object-local constructor, held only in unpublished NATIVE
    // custody. Not source74, not a registry member, not a loaded/active camera.
    struct NativeUnpublishedTaCutChild final {
        std::uint32_t vtable00=0;
        std::array<std::uint32_t,4> offset10{};
        // Source field order20,24,28,50,5C,58,60. Unloaded constructor makes all null.
        std::array<const void*,7> arrays{};
        std::uint32_t rate2C=0,elapsed38=0,field3C=0,mode40=0,lower44=0,active4C=0,loop54=0;
        // Unwritten constructor30/34/48 stay absent until actual resource publication.
        std::optional<std::uint64_t> frameCount30;
        std::optional<std::int32_t> upper48;
        using ResourceArrays=original_ta_cut_storage_candidate_detail::Arrays;
        enum class ResourcePhase : std::uint8_t {unloaded,published,derived,terminal,retired};
        ResourcePhase resourcePhase=ResourcePhase::unloaded;
        std::unique_ptr<ResourceArrays> resourceArrays;
        std::uint32_t resourceFailurePC=0,resourceFrame=0; //Native diagnostic only.
        bool constructed=false,retired=false;
        explicit NativeUnpublishedTaCutChild(const std::array<std::uint32_t,4>& actualOrigin)noexcept {
            vtable00=0x3cca90u; //115AE0, no ref/Entity/count constructor.
            vtable00=0x3d2ee0u; //28287C.
            offset10={{0,0,0,0x3f800000u}}; //282884 SQC2 vf0.
            field3C=0x42a00000u;rate2C=0x41f00000u;
            elapsed38=0;active4C=1;mode40=0;loop54=0;lower44=0;
            for(auto& pointer:arrays)pointer=nullptr; //2828B0..C8 exact field order.
            offset10=actualOrigin; //2828D0/2828D8 copy the actual process443200.
            constructed=true; //Native history, not source field or readiness.
        }
        NativeUnpublishedTaCutChild(const NativeUnpublishedTaCutChild&)=delete;
        NativeUnpublishedTaCutChild& operator=(const NativeUnpublishedTaCutChild&)=delete;
        NativeUnpublishedTaCutChild(NativeUnpublishedTaCutChild&&)=delete;
        NativeUnpublishedTaCutChild& operator=(NativeUnpublishedTaCutChild&&)=delete;
        bool setupNull282990()const noexcept {
            //282994 branches before2829A0: null file changes NO field,
            //including54. It returns1, not a load/activation certificate.
            return constructed&&!retired;
        }
        bool deriveReservedTa01(const rr::assets::OriginalTaCutAuthoringCandidate& source,
            const std::array<std::uint32_t,4>& actualLoadOrigin,
            std::unique_ptr<ResourceArrays> reserved)noexcept {
            namespace r=original_ta_cut_resource_candidate_detail;
            if(!constructed||retired||vtable00!=0x3d2ee0u||
               resourcePhase!=ResourcePhase::unloaded||resourceArrays||frameCount30||upper48||
               !reserved||!r::ta01Reservation(*reserved)||source.frameCount(1)!=461||
               !r::arithmeticAdmitted())return false;
            for(const auto* pointer:arrays)if(pointer)return false;
            //282D18 is a fresh load-local process read, NOT child.offset10.
            const auto loadOffset=actualLoadOrigin;
            //Reached283460 sees only the authenticated all-null first-load state.
            //No allocator effects or source fields change at that empty cleanup.
            resourceArrays=std::move(reserved);resourcePhase=ResourcePhase::published;
            auto& a=*resourceArrays;
            //Native backing was reserved earlier. These are the actual child
            //source-field publications, in282ECC..282FD8 order.
            arrays[4]=a.frameHeaders5C.data(); //5C
            arrays[5]=a.nodeIdentifiers58.data(); //58
            arrays[6]=a.nodes60.data(); //60
            frameCount30=std::uint64_t{461}; //282F08 sign-extended N stored at30/34.
            r::vectorConstructor11C090(a.look20);arrays[0]=a.look20.data();
            r::vectorConstructor11C090(a.up24);arrays[1]=a.up24.data();
            r::vectorConstructor11C090(a.position28);arrays[2]=a.position28.data();
            arrays[3]=a.scalar50.data(); //50
            upper48=461; //N, not N-1. No activation/loop/mode/cursor/time writes.
            for(std::uint32_t frame=0;frame<461;++frame){
                rr::assets::OriginalTaCutFrameBitsCandidate raw;
                resourceFrame=frame;
                if(!source.frameBits(1,frame,raw))resourceFailurePC=0x282fdcu;
                else resourceFailurePC=r::deriveFrameTa01(raw,loadOffset,a,frame);
                if(resourceFailurePC){resourcePhase=ResourcePhase::terminal;return false;}
            }
            resourcePhase=ResourcePhase::derived;
            return true; //Only native copied-input derivation; NOT282CD0/service return.
        }
        bool retireOwned2828F0()noexcept {
            if(!constructed||retired||vtable00!=0x3d2ee0u)return false;
            if(resourceArrays){
                const auto& a=*resourceArrays;
                if((resourcePhase!=ResourcePhase::derived&&resourcePhase!=ResourcePhase::terminal)||
                   !original_ta_cut_resource_candidate_detail::ta01Reservation(a)||
                   !frameCount30||*frameCount30!=461||!upper48||*upper48!=461||
                   arrays[0]!=a.look20.data()||arrays[1]!=a.up24.data()||
                   arrays[2]!=a.position28.data()||arrays[3]!=a.scalar50.data()||
                   arrays[4]!=a.frameHeaders5C.data()||arrays[5]!=a.nodeIdentifiers58.data()||
                   arrays[6]!=a.nodes60.data())return false;
            }else{
                if(resourcePhase!=ResourcePhase::unloaded||frameCount30||upper48)return false;
                for(const auto* pointer:arrays)if(pointer)return false;
            }
            vtable00=0x3d2ee0u; //282918, before283460.
            if(resourceArrays){
                auto& a=*resourceArrays;
                //Concrete native allocations released in283460 source order.
                //Retail's vector header/delete helper is NOT a native ABI claim.
                std::vector<OriginalTaCutVector3Candidate>().swap(a.look20);
                std::vector<OriginalTaCutVector3Candidate>().swap(a.up24);
                std::vector<OriginalTaCutVector3Candidate>().swap(a.position28);
                std::vector<float>().swap(a.scalar50);arrays[3]=nullptr;
                std::vector<std::array<std::uint32_t,2>>().swap(a.frameHeaders5C);arrays[4]=nullptr;
                std::vector<std::uint32_t>().swap(a.nodeIdentifiers58);arrays[5]=nullptr;
                std::vector<original_ta_cut_storage_candidate_detail::Node>().swap(a.nodes60);arrays[6]=nullptr;
                //Source leaves20/24/28 dangling. Native custody invalidates these
                //only AFTER the source-ordered frees, then deletes empty backing.
                arrays[0]=nullptr;arrays[1]=nullptr;arrays[2]=nullptr;
                resourceArrays.reset();
            }
            resourcePhase=ResourcePhase::retired;
            vtable00=0x3cca90u; //115B2C base reset; no ref/count decrement.
            retired=true;
            return true;
        }
        ~NativeUnpublishedTaCutChild()noexcept {
            // Completed unpublished objects still need their local destructor;
            // this is not retail controller174C20 or registry removal.
            if(constructed&&!retired&&!retireOwned2828F0())std::terminate();
        }
    };
    enum class NativeTaCutChildPhase : std::uint8_t {empty,constructed,aborted};
    NativeTaCutChildPhase nativeTaCutChildPhase_=NativeTaCutChildPhase::empty;
    std::unique_ptr<NativeUnpublishedTaCutChild> nativeUnpublishedTaCutChild_;
    bool constructUnpublishedTaCutChild282860()noexcept {
        const auto& app=actual_.applicationEntities;
        if(nativeTaCutChildPhase_!=NativeTaCutChildPhase::empty||nativeUnpublishedTaCutChild_||
           !app||!app->application10()||app->nativeOwnerThread_!=std::this_thread::get_id())return false;
        try {
            // Only native allocation can fail: it precedes every282860 write.
            // No phantom2828F0 call occurs if operator new fails.
            auto next=std::unique_ptr<NativeUnpublishedTaCutChild>(
                new NativeUnpublishedTaCutChild(app->cutOrigin443200_));
            if(!next->setupNull282990())std::terminate();
            nativeUnpublishedTaCutChild_=std::move(next);
            nativeTaCutChildPhase_=NativeTaCutChildPhase::constructed;
            return true;
        }catch(...){return false;}
    }
    bool abortUnpublishedTaCutChildNative()noexcept {
        if(nativeTaCutChildPhase_==NativeTaCutChildPhase::aborted)return !nativeUnpublishedTaCutChild_;
        if(nativeTaCutChildPhase_!=NativeTaCutChildPhase::constructed||!nativeUnpublishedTaCutChild_||
           !nativeUnpublishedTaCutChild_->retireOwned2828F0())return false;
        nativeUnpublishedTaCutChild_.reset(); //Native allocation delete; local source destructor already ran ONCE.
        nativeTaCutChildPhase_=NativeTaCutChildPhase::aborted; //Never rearm/erase construction history.
        return true;
    }
    std::unique_ptr<OriginalTaCutChildrenCandidate> taCuts_;
    unsigned taCutCursor_=0;
    bool taPositive_=false,taFirstSelected_=false;
    // 174A30 constructed for a counted race whose 174E48 cut count is zero
    // (no +74 / +80). Native history; the Tournament route never sets it.
    bool zeroCutController_=false;
    std::array<TrackIntroLine50HG,7> taLines_{};
    std::size_t taLineCount_=0; // Actual script span, not storage capacity.
    OriginalRaceCameraOwner controller_;
    // ONE noncopyable Entity counterpart for this existing copyable numeric
    // payload. No second controller, borrowed parent, or additional authority.
    struct ColdController174A30 {
        OriginalWreckComponent50LA entity;
        std::uint64_t issuerGeneration=0;
        bool started=false,released=false;
        std::uint32_t vtable08=0;
        const void* scene3C=nullptr;
        // Actual retained loading operands, published only by the same
        // Application/Race/Camera34 native preparation handoff. scene3C is
        // its existing Session projection, not authority for another camera.
        const OriginalFrontendCameraCold50LA* nativeScene3COwner=nullptr;
        const OriginalRaceConstructor50LA* race88=nullptr;
        bool nativeLoadOperandsPublished=false,nativeLoadOperandsRevoked=false;
        bool nativeLoadPrefixEntered=false,nativeLoadPrefixComplete=false;
        std::uint32_t nativeLoadPrefixPC=0x174df0;
        const void* racer40=nullptr;
        std::array<const void*,4> queries48{}; //48,4C,50,54.
        std::uint32_t word58=0; //Constructor zero; no pointer/ownership meaning inferred.
        const void* script5C=nullptr;
        std::array<const void*,8> children64{}; //64..80, including intro emitter80.
        std::uint32_t mode84=0; //174DF0 later receives the actual race8C integer.
        // Constructor does NOT establish88; only the published actual loading
        // operands above may establish its native borrow. Unknown masks below
        // are never filled from safe native backing.
        std::uint32_t word60Value=0,word60Known=0;
        std::uint16_t halfA8Value=0,halfA8Known=0;
    } coldController_;
    OriginalFollowCamera follow_;
    OriginalAttachedCamera attached_;
    OriginalDeathCamera50KG death_;
    OriginalDeathCameraGlobals50KG deathGlobals_;
    OriginalRetirementCameraCursor50KG retirementCursor_;
    float uiTransition1E0_=0;
    OriginalRaceStartCamera start_;
    std::array<OriginalFollowPreset,32> presets_{};
    OriginalFollowPresetBank presetBank_;
    OriginalAttachedOffset attachedOffset_;
    OriginalCourseStartActions startActions_;
    OriginalCameraStartPrelude startPrelude_;
    RaceStartPreparationReceipt preparation_;
    std::array<std::uint32_t,16> seedSamples_{};std::size_t seedCount_=0;
    OriginalCameraChildRegistry collection_;
    std::array<std::uint64_t,12> childOrder_{}; // Three fixed children, up to seven authored cuts, actual7C, TV78.
    OriginalStartChildBinding introChild_;
    OriginalAttachedChildRegistry attachedChild_;
    OriginalFollowChildRegistry followChild_;
    OriginalDeathChildRegistry50KG deathChild_;
    OriginalCameraObjectChildHeads objectChildren_;
    OriginalCameraControllerEntityFields controllerEntity_;
    OriginalCameraSceneOwner spatial_;
    OriginalSelectedCameraVisibilityBacking50SC visibilityBacking_;
    OriginalSelectedCameraVisibilityKeys50SC visibilityKeys_;
    OriginalFollowSceneBinding sceneBinding_;
    CameraSceneState49F scene_;
    OriginalCameraSceneTick tick_;
    OriginalCameraViewportFields viewport_;
    bool coldCameraSelected26F940_=false;
    bool bindCountedRaceCameraNative34()noexcept;
    bool canonicalCourseEndRoute()const noexcept;
    bool completeCountedRaceCameraNative34()noexcept;
    bool completeCountedRaceMusicNative10C(std::uint32_t&)noexcept;
    bool completeCountedRaceWorldNativePrefix1A9EE0(std::uint32_t&)noexcept;
    bool revokeCountedRaceCameraNative34()noexcept;
    TVCameraUpdateOutput49F pose_{};
    // race48+78 (175040/175650): the course TV camera; absent when the course
    // file does not load (then 1ACE14/1A81E4/17623C skip every TV step).
    original_replay_tv50PD::Owner replayTv_;
    CameraViewOutput49F matrices_;
    OriginalRaceCameraDisplayFields display_;
    OriginalRaceViewportCall coldViewport_;
    OriginalGcGaIntroExitOwner exit_;
    OriginalDirectRaceStartOwner directStart_;
    bool directStartPending_=false;
    OriginalRaceStartCountdown countdown_;
    // race+D4 PodRaceStartOverlay (196F20), one per race on every course.
    // Shared with the renderer, which may outlive this Session.
    std::shared_ptr<OriginalRaceStartOverlay50HU> startOverlay_;
    bool goLogged_=false,goEdgePending_=false;
    OriginalVideoBlockingFade blockingFade_;
    AsyncRetainedFade asyncFade_,endingFade_,taSkipAsyncFade_;
    OriginalVideoAsyncRequest50LA fadeInRequest_,taSkipFadeInRequest_,endingFadeRequest_;
    enum class FadeInRequestConsumer : std::uint8_t {none,normal,taSkip};
    FadeInRequestConsumer fadeInRequestConsumer_=FadeInRequestConsumer::none;
    bool retainedFadeInRequestPending50LA()const noexcept {
        return fadeInRequestConsumer_==FadeInRequestConsumer::normal?fadeInRequest_.pending():
            fadeInRequestConsumer_==FadeInRequestConsumer::taSkip&&taSkipFadeInRequest_.pending();
    }
    bool blockingRequestPending50LA()const noexcept{return exit_.requestPending()||taSkipFadeRequest_.pending();}
    bool videoRequestPending50LA()const noexcept {
        return blockingRequestPending50LA()||fadeInRequest_.pending()||taSkipFadeInRequest_.pending()||endingFadeRequest_.pending();
    }
    OriginalCameraStartResult serviceAsyncFadeInRequest260610()noexcept {
        using S=OriginalCameraStartStatus;
        if(!displayInputs_||displayInputs_->fadeManagerA8!=8||blockingRequestPending50LA()||endingFadeRequest_.pending())return {S::owner,0x2ae93c};
        auto& app=*displayInputs_;
        if(fadeInRequestConsumer_==FadeInRequestConsumer::none){
            const bool skip=taSkipFadeFinished_&&!exit_.complete()&&!directStart_.complete();
            auto& selected=skip?taSkipFadeInRequest_:fadeInRequest_;
            // A later source invocation cannot borrow a completed request's return.
            if(selected.attempted())return {S::owner,0x2ae93c};
            fadeInRequestConsumer_=skip?FadeInRequestConsumer::taSkip:FadeInRequestConsumer::normal;
        }
        auto& request=fadeInRequestConsumer_==FadeInRequestConsumer::taSkip?taSkipFadeInRequest_:fadeInRequest_;
        const auto r=request.invoke(OriginalVideoFadeDirection::in261630,app.video,actual_.applicationClock,
            app.currentApplicationMode22C(),app.fadeDuration98);
        if(r.status!=S::complete)return r;
        app.fadeManagerA8=0; //2AE958 only after the actual request returns.
        if(fadeInRequestConsumer_==FadeInRequestConsumer::taSkip)taSkipFadeInRequested_=true;
        else fadeInRequested_=true;
        fadeInRequestConsumer_=FadeInRequestConsumer::none;return r;
    }
    //2255B0 camera tail belongs to this one genuine Player camera, outside
    // all racer snapshots. It is separate from the ordinary camera-frame cursor.
    struct HumanResetCamera {
        enum class Stage : std::uint8_t {idle,motion,remove,position,attach,project,queries,finalMotion,complete};
        Stage stage=Stage::idle;
        std::uint64_t serial=0;
        bool input=false;
        CollectionRef50BX visibilityZone=0,collisionZone=0;
        SpringVector4_48N anchor{};
        OriginalCameraPairResetCursor queries{};
    } humanResetCamera_;
    OriginalHumanEndClock50KG endingClock_;
    OriginalHumanEndClockCursor50KG endingClockCursor_;
    OriginalHumanEndClockResult50KG advanceCanonicalHumanEndRoot()noexcept;
    Result advanceCanonicalRaceFadeInPrefix()noexcept;
    bool fadeInRequested_=false,endingFadeRequested_=false,taSkipFadeInRequested_=false;
    Result result_;
    OriginalCameraModeInputResult lastInput_{};

    template<class T> static std::uint64_t key(const T& owner) noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&owner));
    }
    static bool same(float a,float b) noexcept{return std::memcmp(&a,&b,sizeof a)==0;}
    #include "original_camera_controller_cold_candidate.inl"
    #include "original_camera_unused_children50la.inl"
    #include "original_camera_follow_death_cold50la.inl"
#include "original_camera_attached_cold50la.inl"
    #include "original_camera_intro_source50la.inl"
    #include "original_camera_racer_binding50la.inl"
    #include "original_camera_fog_publication50la.inl"
    rr::game::OriginalRacerDiffuseCamera50LA racerDiffuseCamera_;
    Result set(Status s,std::uint32_t pc=0) noexcept {result_.status=s;result_.sourcePC=pc;return result_;}
    Result fail(Status s,std::uint32_t pc=0) noexcept {invalidateFogPreparation();frameStage_=FrameStage::failed;return set(s,pc);}
    bool constructRacerEntityPrefixes()noexcept {
        const bool entities=bool(actual_.applicationEntities),materials=bool(actual_.applicationMaterials);
        if(!entities&&!materials&&!actual_.actualMaterialDirectory36B970)return true;
        if(!entities||!materials||!actual_.actualMaterialDirectory36B970||!displayInputs_||
           (raceFlags1B0_&3u)!=1u){result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
            result_.entityCold.sourcePC=0x1a9240u;return false;}
        // Source1A6C80/88 passes the actual one-view1B0 low bits to1A9240;
        //1A9284 writes numeric race8C=0. Zero is not an absent scene pointer.
        if(!nativeTaIssuance_||nativeTaIssuance_->course!=OriginalCameraStartCourse::ra)raceMode8C_=0;
        // Issued RA binds its actual canonical8C before entering this native
        // racer-prefix adapter; never invent its mode from one-view appearance.
        aiDamageRace_={generation_,key(*this),&raceMode8C_};
        if(!aiDamageMaterial_.prepared()){
            rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
            if(!aiDamageMaterial_.bindApplication(actual_.applicationMaterials,diagnostic)){
                result_.entityCold.status=OriginalRacerEntityColdStatus50LA::material;
                result_.entityCold.sourcePC=diagnostic.sourcePC;return false;
            }
        }
        const auto& directory=*actual_.actualMaterialDirectory36B970;
        auto& application=*actual_.applicationEntities;
        for(std::size_t i=0;i<actual_.bank.count();++i){
            auto& owner=racerEntities_[i];
            if(owner.prepared())continue; // Actual completed constructor, not bank phase.
            OriginalRacerEntityColdBinding50LA binding;
            binding.sessionGeneration=generation_;binding.racerKey=racerKey(i);
            binding.detached7F8Key=key(owner.detached7F8);binding.overlay240Key=key(owner.overlay240.component);
            binding.actualObjectCount423F60=&application.objectCount423F60();
            binding.actualAttachmentLock423F58=&application.attachmentLock423F58();
            binding.actualTypes383408=&application.types383408();
            binding.actualRace28C=&aiDamageRace_;binding.actualAIDamageMaterial=&aiDamageMaterial_;
            binding.actualMaterialDirectory36B970=directory.c_str();
            binding.materialDirectoryCapacity=directory.size()+1u;
            binding.renderer442F70=&displayInputs_->rendererDispatch;
            binding.actualRenderer=&displayInputs_->renderer;
            binding.rendererDiagnostic=&displayInputs_->diagnostic;
            if(constructorOnly_){
                // This strong Session lease outlives every retained Racer. The
                // actual constructor reads it at2107FC, AFTER earlier effects.
                binding.sourceConstructorOnly=true;
                binding.actualWorld2107FC=constructorWorld_.get();
                if(!constructorWorld_||!constructorWorld_->readIncoming210770(generation_,
                    binding.actualObjectCount423F60,binding.actualIncomingVisibility4C0,
                    binding.actualIncomingCollision4C4)){
                    result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                    result_.entityCold.sourcePC=0x210770u;return false;
                }
                binding.actualLoadPod60=&actual_.racer60[i];
                binding.actualAnchorOrigin434630=&application.detectorFrame434630_;
            }
            result_.entityCold=owner.constructBeforeParticleParent210A18(binding);
            if(!result_.entityCold.okay())return false;
        }
        return true;
    }
    bool constructRacerModelPrefixes()noexcept {
        if(!actual_.applicationEntities)return !actual_.actualRigs&&
            !actual_.actualTrackParticles4A0&&!actual_.actualHeightDomain&&
            !actual_.actualRightModels&&!actual_.actualLeftModels&&!actual_.actualChariotModels&&
            !actual_.actualBottomRightModels&&!actual_.actualBottomLeftModels;
        if(!actual_.actualRigs||!actual_.actualTrackParticles4A0||!actual_.actualHeightDomain||
           !actual_.actualTrackParticles4A0->particles()||!actual_.actualRightModels||
           !actual_.actualLeftModels||!actual_.actualChariotModels||
           bool(actual_.actualBottomRightModels)!=bool(actual_.actualBottomLeftModels)){
            result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
            result_.entityCold.sourcePC=0x2109b4u;return false;
        }
        for(std::size_t i=0;i<actual_.bank.count();++i){
            const auto* rig=(*actual_.actualRigs)[i].get();
            constexpr auto right=native_racer50HW::Rig::right;
            constexpr auto left=native_racer50HW::Rig::left;
            constexpr auto bottomRight=native_racer50HW::Rig::bottomRight;
            constexpr auto bottomLeft=native_racer50HW::Rig::bottomLeft;
            constexpr auto chariot=native_racer50HW::Rig::chariot;
            if(!rig||(rig->modelCount!=3&&rig->modelCount!=5)){
                result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                result_.entityCold.sourcePC=0x210bc8u;return false;
            }
            const bool lower=rig->modelCount==5;
            const auto* bottomRightModel=actual_.actualBottomRightModels?
                (*actual_.actualBottomRightModels)[i].get():nullptr;
            const auto* bottomLeftModel=actual_.actualBottomLeftModels?
                (*actual_.actualBottomLeftModels)[i].get():nullptr;
            if(bool(bottomRightModel)!=lower||bool(bottomLeftModel)!=lower||
               actual_.bank.read(i,reads_.bottomRightPresent)!=lower||
               actual_.bank.read(i,reads_.bottomLeftPresent)!=lower||
               (!lower&&(actual_.bank.read(i,reads_.bottomRightModel1C0)!=SceneNodeIndex49P::none||
                         actual_.bank.read(i,reads_.bottomLeftModel1C4)!=SceneNodeIndex49P::none))){
                result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                result_.entityCold.sourcePC=0x211210u;return false;
            }
            // All authenticated inputs come from this exact Rig and share
            // its one installed numeric graph. Session appends the owners;
            // it never reconstructs or re-samples any source subtree. The
            // separate21191C branch is reached because the engine owners exist;
            // the no-engine211840 combined-model194 branch is not substituted.
            for(const auto role:{right,left,bottomRight,bottomLeft,chariot}){
                if(role>=rig->modelCount)continue;
                const auto root=actual_.bank.read(i,role==right?reads_.rightModel1B8:
                    role==left?reads_.leftModel1BC:role==bottomRight?reads_.bottomRightModel1C0:
                    role==bottomLeft?reads_.bottomLeftModel1C4:reads_.model1B4);
                if(rig->mcf[role].empty()||
                   rig->mcfMembers[role].empty()||rig->psg[role].empty()||
                   rig->hierarchy[role].data!=rig->psg[role].data()||rig->roots[role]!=root){
                    result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                    result_.entityCold.sourcePC=role==right?0x210bc8u:
                        role==left?0x210e70u:role==bottomRight?0x211210u:
                        role==bottomLeft?0x21150cu:0x21191cu;return false;
                }
            }
            auto& entity=racerEntities_[i];
            if(!entity.postOverlayConstructed210A20()){
                result_.entityCold=entity.constructPostOverlay210890(
                    *actual_.actualTrackParticles4A0->particles(),*actual_.actualHeightDomain,
                    actual_.launchKind11258,key(entity.particle7FC));
                if(!result_.entityCold.okay())return false;
            }
            for(const auto role:{right,left,bottomRight,bottomLeft,chariot}){
                if(role>=rig->modelCount)continue;
                auto* model=(role==right?*actual_.actualRightModels:
                    role==left?*actual_.actualLeftModels:role==bottomRight?*actual_.actualBottomRightModels:
                    role==bottomLeft?*actual_.actualBottomLeftModels:*actual_.actualChariotModels)[i].get();
                const auto setupPC=role==right?0x210c5cu:role==left?0x210f20u:
                    role==bottomRight?0x211224u:role==bottomLeft?0x211520u:0x211950u;
                if(!model||model->actualRig()!=rig||model->modelRole()!=role||
                   model->sessionGeneration()!=generation_){
                    result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                    result_.entityCold.sourcePC=setupPC;return false;
                }
                try {
                    // Resume reuses each actual completed owner. Represented
                    // joins follow R,L,BR,BL,C: the optional lower owners are
                    // 1AC/1C0 at211210/211254 and1B0/1C4 at21150C/211550.
                    // Intervening non-model SceneObjects remain outside this
                    // bounded Entity projection. finish1538B0 now reaches each
                    // actual213380 material pass; the following nullcg/physics/
                    // effect sequence is still not declared completed.
                    if(!model->finishedFor(entity)&&
                       !(constructorOnly_&&model->sourceModelPrefixFor(entity)))
                        model->finish1538B0(entity,*actual_.actualMaterialDirectory36B970);
                    if(constructorOnly_&&role==chariot){
                        if(!model->sourceModelPrefixFor(entity)){
                            result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                            result_.entityCold.sourcePC=model->failurePC()?model->failurePC():0x21204cu;
                            return false;
                        }
                        if(!nativeWreckOwner_||model->sourceTerrainWithdrawn_){
                            result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                            result_.entityCold.sourcePC=0x21204cu;return false;
                        }
                        if(!model->sourceTerrainComplete_)model->finishSourceTerrain21204C(*nativeWreckOwner_);
                        if(model->sourceEngineQueryWithdrawn_){
                            result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                            result_.entityCold.sourcePC=0x2120b4u;return false;
                        }
                        if(!model->sourceEngineQueryComplete_)model->finishSourceEngineQuery2120B4(*nativeWreckOwner_);
                        if(model->sourceQuery694Withdrawn_||model->sourceContact354Withdrawn_){
                            result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                            result_.entityCold.sourcePC=model->failurePC()?model->failurePC():0x2121d0u;return false;
                        }
                        if(!model->sourceQuery694Complete_)model->finishSourceQuery6942121D0(*nativeWreckOwner_);
                        if(!model->sourceContact354Complete_)model->finishSourceContact354212278(*nativeWreckOwner_);
                        if(model->sourceDetector358Withdrawn_){
                            result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                            result_.entityCold.sourcePC=model->failurePC()?model->failurePC():0x212348u;return false;
                        }
                        if(!model->sourceDetector358Complete_)model->finishSourceDetector358212348(*nativeWreckOwner_);
                        if(model->sourceAuxiliaryWithdrawn_){
                            result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                            result_.entityCold.sourcePC=model->failurePC()?model->failurePC():0x2123c8u;return false;
                        }
                        if(!model->sourceAuxiliaryComplete_)model->finishSourceAuxiliary2123C8(*nativeWreckOwner_);
                        if(model->sourceSecondaryWithdrawn_){
                            result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                            result_.entityCold.sourcePC=model->failurePC()?model->failurePC():0x2124dcu;return false;
                        }
                        if(!model->sourceSecondaryComplete_)model->finishSourceSecondary2124DC();
                        if(model->sourceSecondarySetupWithdrawn_){
                            result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                            result_.entityCold.sourcePC=model->failurePC()?model->failurePC():0x2125c4u;return false;
                        }
                        if(!model->sourceSecondarySetupComplete_)model->finishSourceSecondarySetup2125C4();
                        if(model->sourceSecondaryContactsWithdrawn_){
                            result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                            result_.entityCold.sourcePC=model->failurePC()?model->failurePC():0x212720u;return false;
                        }
                        if(!model->sourceSecondaryContactsComplete_)model->finishSourceSecondaryContacts212720(*nativeWreckOwner_);
                        if(!model->sourceWreckConfigComplete_)model->finishSourceWreckConfig2129A4(*nativeWreckOwner_);
                        if(!model->sourceWreckPieceComplete_)model->finishSourceWreckPiece212A50(*nativeWreckOwner_);
                        if(!model->sourceWreckBodyComplete_)model->finishSourceWreckBody165E90(*nativeWreckOwner_);
                        if(!model->sourceWreckE0Complete_)model->finishSourceWreckE0166018(*nativeWreckOwner_);
                        if(!model->sourceWreckE0LoadComplete_)model->finishSourceWreckE0Shape166074(*nativeWreckOwner_);
                        if(!model->sourceWreckE0PrepareComplete_)model->finishSourceWreckE0Prepare16607C(*nativeWreckOwner_);
                        if(!model->sourceWreckDCComplete_)model->finishSourceWreckDC1660A0(*nativeWreckOwner_);
                        if(!model->sourceWreckD8Complete_)model->finishSourceWreckD816616C(*nativeWreckOwner_);
                        if(!model->sourceWreckE4Complete_)model->finishSourceWreckE41661D8(*nativeWreckOwner_);
                        if(!model->sourceWreckE4PrepareComplete_)model->finishSourceWreckE4Prepare16629C(*nativeWreckOwner_);
                        if(!model->sourceWreckE8Complete_)model->finishSourceWreckE81662E4(*nativeWreckOwner_);
                        if(!model->sourceWreckMutualComplete_)model->finishSourceWreckMutual166390(*nativeWreckOwner_);
                        if(!model->sourceWreckArraysComplete_)model->finishSourceWreckArrays2CB628(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb6ccu&&!model->sourceWreckSmokeComplete_)
                            model->finishSourceWreckSmoke2CB6CC(*nativeWreckOwner_);
                        if(model->sourceWreckSmokeComplete_&&model->sourceConstructorPC_==0x19a7d0u&&!model->sourceWreckSmokeReadComplete_)
                            model->finishSourceWreckSmokeRead19A7D0(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb720u&&!model->sourceWreckFireComplete_)
                            model->finishSourceWreckFire2CB720(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x15e024u&&!model->sourceWreckFireReadComplete_)
                            model->finishSourceWreckFireRead15E024(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb778u&&!model->sourceWreckFirstPairComplete_)
                            model->finishSourceWreckFirstPair2CB778(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb6ccu&&model->sourceWreckFirstPairComplete_&&
                           !model->sourceWreckRemainingPairsComplete_)
                            model->finishSourceWreckRemainingPairs2CB6CC(*nativeWreckOwner_);
                        if((model->sourceConstructorPC_==0x2cb7e0u||model->sourceConstructorPC_==0x2cb7e4u)&&
                           !model->sourceWreckDustComplete_)
                            model->finishSourceWreckDust2CB7E0(*nativeWreckOwner_);
                        if(model->sourceWreckDustComplete_&&model->sourceConstructorPC_==0x19a7d0u&&
                           !model->sourceWreckDustReadComplete_)
                            model->finishSourceWreckDustRead19A7D0(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb854u&&!model->sourceWreckSplashComplete_)
                            model->finishSourceWreckSplash2CB854(*nativeWreckOwner_);
                        if(model->sourceWreckSplashComplete_&&model->sourceConstructorPC_==0x19ddc8u&&
                           !model->sourceWreckSplashReadComplete_)
                            model->finishSourceWreckSplashRead19DDC8(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb8c4u&&!model->sourceWreckPieceAppendComplete_)
                            model->finishSourceWreckPieceAppend22C1B8(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x22c21cu&&!model->sourceWreckWaterComplete_)
                            model->finishSourceWreckWater22C21C(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x22c25cu&&!model->sourceWreckPieceCursorComplete_)
                            model->finishSourceWreckPieceCursor22C25C(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x22c0a0u&&model->sourceWreckPieceCursorComplete_&&
                           !model->sourceWreckSecondPieceComplete_)
                            model->finishSourceWreckSecondPiece22C0A0(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x165e90u&&model->sourceWreckSecondPieceComplete_&&
                           !model->sourceWreckSecondBodyComplete_)
                            model->finishSourceWreckSecondBody165E90(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x166018u&&model->sourceWreckSecondBodyComplete_&&
                           !model->sourceWreckSecondE0Complete_)
                            model->finishSourceWreckSecondE0166018(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x166074u&&model->sourceWreckSecondE0Complete_&&
                           !model->sourceWreckSecondE0LoadComplete_)
                            model->finishSourceWreckSecondE0Shape166074(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x16607cu&&model->sourceWreckSecondE0LoadComplete_&&
                           !model->sourceWreckSecondE0PrepareComplete_)
                            model->finishSourceWreckSecondE0Prepare16607C(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x1660a0u&&model->sourceWreckSecondE0PrepareComplete_&&
                           !model->sourceWreckSecondDCComplete_)
                            model->finishSourceWreckSecondDC1660A0(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x16616cu&&model->sourceWreckSecondDCComplete_&&
                           !model->sourceWreckSecondD8Complete_)
                            model->finishSourceWreckSecondD816616C(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x1661d8u&&model->sourceWreckSecondD8Complete_&&
                           !model->sourceWreckSecondE4Complete_)
                            model->finishSourceWreckSecondE41661D8(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x16629cu&&model->sourceWreckSecondE4Complete_&&
                           !model->sourceWreckSecondE4PrepareComplete_)
                            model->finishSourceWreckSecondE4Prepare16629C(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x1662e4u&&model->sourceWreckSecondE4PrepareComplete_&&
                           !model->sourceWreckSecondE8Complete_)
                            model->finishSourceWreckSecondE81662E4(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x166390u&&model->sourceWreckSecondE8Complete_&&
                           !model->sourceWreckSecondMutualComplete_)
                            model->finishSourceWreckSecondMutual166390(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb618u&&model->sourceWreckSecondMutualComplete_&&
                           !model->sourceWreckSecondEffectInputsComplete_)
                            model->bindSourceWreckSecondEffectInputs2CB618(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb628u&&model->sourceWreckSecondEffectInputsComplete_&&
                           !model->sourceWreckSecondArraysComplete_)
                            model->finishSourceWreckSecondArrays2CB628(*nativeWreckOwner_);
                        if((model->sourceConstructorPC_==0x2cb6ccu||model->sourceConstructorPC_==0x2cb7e4u)&&
                           model->sourceWreckSecondArraysComplete_&&!model->sourceWreckSecondInitialChildComplete_)
                            model->constructSourceWreckSecondInitialChild(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x19a7d0u&&model->sourceWreckSecondInitialChildComplete_&&
                           !model->sourceWreckSecondInitialReaderComplete_)
                            model->readSourceWreckSecondInitialChild19A7D0(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x19a7f8u&&model->sourceWreckSecondInitialReaderComplete_&&
                           !model->sourceWreckSecondInitialCallerComplete_)
                            model->completeSourceWreckSecondInitialChild19A7F8(*nativeWreckOwner_);
                        if((model->sourceConstructorPC_==0x2cb720u||model->sourceConstructorPC_==0x2cb854u)&&
                           model->sourceWreckSecondInitialCallerComplete_&&!model->sourceWreckSecondSuccessorComplete_)
                            model->constructSourceWreckSecondSuccessorChild(*nativeWreckOwner_);
                        if((model->sourceConstructorPC_==0x15e024u||model->sourceConstructorPC_==0x19ddc8u)&&
                           model->sourceWreckSecondSuccessorComplete_&&!model->sourceWreckSecondSuccessorReaderComplete_)
                            model->readSourceWreckSecondSuccessorChild(*nativeWreckOwner_);
                        if((model->sourceConstructorPC_==0x15e0b0u||model->sourceConstructorPC_==0x19de7cu)&&
                           model->sourceWreckSecondSuccessorReaderComplete_&&!model->sourceWreckSecondSuccessorCallerComplete_)
                            model->completeSourceWreckSecondSuccessorChild(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb778u&&model->sourceWreckSecondSuccessorCallerComplete_&&
                           !model->sourceWreckSecondFirstPairComplete_)
                            model->finishSourceWreckSecondFirstPair2CB778(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb6ccu&&model->sourceWreckSecondFirstPairComplete_&&
                           !model->sourceWreckSecondRemainingPairsComplete_)
                            model->finishSourceWreckSecondRemainingPairs2CB6CC(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb7e0u&&model->sourceWreckSecondFirstPairComplete_&&
                           !model->sourceWreckSecondPositiveDustComplete_)
                            model->constructSourceWreckSecondPositiveDust(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x19a7d0u&&model->sourceWreckSecondPositiveDustComplete_&&
                           !model->sourceWreckSecondPositiveDustReaderComplete_)
                            model->finishSourceWreckSecondPositiveDustRead19A7D0(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb854u&&model->sourceWreckSecondPositiveDustReaderComplete_&&
                           !model->sourceWreckSecondPositiveSplashComplete_)
                            model->constructSourceWreckSecondPositiveSplash(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x19ddc8u&&model->sourceWreckSecondPositiveSplashComplete_&&
                           !model->sourceWreckSecondPositiveSplashReaderComplete_)
                            model->finishSourceWreckSecondPositiveSplashRead19DDC8(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cb8c4u&&model->sourceWreckPieceCursorComplete_&&
                           (model->sourceWreckSecondPositiveSplashReaderComplete_||model->sourceWreckSecondSuccessorCallerComplete_)&&
                           !model->sourceWreckSecondPieceAppendComplete_)
                            model->finishSourceWreckSecondPieceAppend22C1B8(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x22c21cu&&model->sourceWreckSecondPieceAppendComplete_&&
                           !model->sourceWreckSecondWaterComplete_)
                            model->finishSourceWreckSecondWater22C21C(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x22c25cu&&model->sourceWreckSecondWaterComplete_&&
                           !model->sourceWreckSecondPieceCursorComplete_)
                            model->finishSourceWreckSecondPieceCursor22C25C(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x22c278u&&model->sourceWreckPieceCursorComplete_&&
                           !model->sourceWreckGroupConstructorComplete_)
                            model->constructSourceWreckGroup22C278(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x22c298u&&model->sourceWreckGroupConstructorComplete_&&
                           !model->sourceWreckGroupPrimaryComplete_)
                            model->bindSourceWreckGroupPrimary22C298(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cc088u&&model->sourceWreckGroupPrimaryComplete_&&
                           !model->sourceWreckGroupContextComplete_)
                            model->consumeSourceWreckGroupContext2CC088(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cc0b4u&&model->sourceWreckGroupContextComplete_&&
                           !model->sourceWreckGroupSparksComplete_)
                            model->constructSourceWreckGroupSparks2CC0B4(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x2cc0d4u&&model->sourceWreckGroupSparksComplete_&&
                           !model->sourceWreckGroupSparksSetupComplete_)
                            model->setupSourceWreckGroupSparks2CC0D4(*nativeWreckOwner_);
                        if(model->sourceConstructorPC_==0x15f2f0u&&model->sourceWreckGroupSparksSetupComplete_&&
                           !model->sourceWreckGroupSparksConfigComplete_)
                            model->beginSourceWreckGroupSparksConfig15F2F0(*nativeWreckOwner_);
                        //KnownNULL remains2CC124; nonnull stackConfig ctor stops15AE2C beforeload.
                        //No FileManager open/read, emitter reader, Stop/append or readiness grant.
                        result_.animationCold={OriginalRacerAnimationColdStatus50LA::complete,model->sourceConstructorPC_};
                        continue;
                    }
                    result_.animationCold={OriginalRacerAnimationColdStatus50LA::complete,
                        role==right?0x210cbcu:role==left?0x210f80u:
                        role==bottomRight?0x211284u:role==bottomLeft?0x211580u:0x2119b0u};
                }catch(...){
                    result_.entityCold.status=OriginalRacerEntityColdStatus50LA::binding;
                    result_.entityCold.sourcePC=model->failurePC()?model->failurePC():setupPC;
                    return false;
                }
            }
        }
        return true;
    }
    bool releaseRacerEntityPrefixes()noexcept {
        if(!onOwnerThread())return false;

        //No public intro work may survive a failed dependent-owner cleanup.
        if(actual_.taIntroAudio&&actual_.taIntroAudio->taIntroNativeReserved_)
            actual_.taIntroAudio->taIntroNativeWithdrawn_=true;
        if(!retireNativePlayerSpeechCold()){
            result_.entityCleanup={OriginalRacerEntityColdCleanupStatus50LA::topology,0x2c10b0u};
            return false;
        }
        if(!retireNativeRetainedWreckCold()){
            result_.entityCleanup={OriginalRacerEntityColdCleanupStatus50LA::topology,0x22ce10u};
            return false;
        }
        if(!revokeNativeRacerCameraBindings()){
            result_.entityCleanup={OriginalRacerEntityColdCleanupStatus50LA::topology,nativeRacerCamera1750A0_.pc};
            return false;
        }
        // The TV20 source association is a borrow of an actual retained340.
        // Withdraw it BEFORE the first chariot/model can retire that body,
        // including failure/abandonment and a later repeated cleanup.
        if(!revokeNativeRaTvBody20()){
            result_.entityCleanup={OriginalRacerEntityColdCleanupStatus50LA::topology,0x2d209cu};
            return false;
        }
        // Native storage lifetime releases only the represented cold prefixes.
        // No manager/race/root Entity list has been invented for these racers.
        for(std::size_t i=actual_.bank.count();i>0;--i){
            // Actual Cables allocation owns a raw backlink to this racer. Close
            // it while Session/racer/models and canonical Bank still exist.
            if(actual_.actualCables){
                auto* cables=(*actual_.actualCables)[i-1u];
                if(!cables||cables->closeSelectionAllocationPrefix().status!=
                   OriginalPodCableAllocationStatus50LA::closed){
                    result_.entityCleanup={OriginalRacerEntityColdCleanupStatus50LA::topology,0x173d40u};
                    return false;
                }
            }
            // Release C,BL,BR,L,R, preserving each
            // animation18 alias until that owner's model release has repaired
            // the actual7F8 list. Each visits this racer's own bank graph.
            for(const auto* models:{actual_.actualChariotModels,actual_.actualBottomLeftModels,
                actual_.actualBottomRightModels,actual_.actualLeftModels,actual_.actualRightModels}){
                auto* model=models?(*models)[i-1].get():nullptr;
                if(model&&!model->close()){
                    result_.entityCleanup.status=OriginalRacerEntityColdCleanupStatus50LA::topology;
                    result_.entityCleanup.sourcePC=model->failurePC();return false;
                }
            }
            result_.animationCleanup={OriginalRacerAnimationColdStatus50LA::complete,0x154130u};
            result_.entityCleanup=racerEntities_[i-1].releaseColdPrefix();
            if(!result_.entityCleanup.okay())return false;
        }
        return true;
    }
    OriginalRaceCameraSceneBorrow sceneBorrow() noexcept {
        invalidateFogPreparation();
        return {&viewport_,&scene_,&tick_,&pose_.position,&matrices_};
    }
    bool identitiesCurrent() const {
        if((!actual_.bank.count()||actual_.bank.count()>8)||actual_.humanCount34>1u)return false;
        for(std::size_t i=0;i<actual_.bank.count();++i){
            const bool human=i<actual_.humanCount34;
            if(!actual_.bank.active(i)||!actual_.racer60[i]||actual_.racer60[i]!=identity_[i].pod60||
               actual_.bank.read(i,reads_.class58)!=(human?0u:1u))return false;
            if(!human&&(!actual_.bank.read(i,reads_.aiInitialized)||
               actual_.bank.read(i,reads_.managerIndex)!=i||
               actual_.bank.read(i,reads_.pod60)!=identity_[i].pod60))return false;
        }
        return true;
    }
    bool cutCurrent(Cut cut) const {
        if(!identitiesCurrent())return false;
        if(cut==Cut::heldBeforeStart&&(actual_.all8Manager.flags58&2u))return false;
        for(std::size_t i=0;i<actual_.bank.count();++i){
            const auto expected=cut==Cut::afterAllRacerPrefixes?Phase::AfterBodyChildren:Phase::Ready;
            if(actual_.bank.read(i,reads_.phase)!=expected)return false;
            if(cut==Cut::afterAllRacerPrefixes&&
               !same(actual_.bank.read(i,reads_.frameFactor),frameDelta_))return false;
        }
        return true;
    }
    bool readRacers(RaceStartRacerBank& out,GcStartRacerEffectBank& effects) const {
        if(!identitiesCurrent())return false;
        out.sessionGeneration=generation_;out.racerCount=actual_.bank.count();
        for(std::size_t i=0;i<actual_.bank.count();++i){
            const auto node=static_cast<std::int32_t>(actual_.bank.read(i,reads_.body288));
            const auto count=actual_.bank.read(i,reads_.nodeCount);
            if(node<0||!count||count>reads_.local.size()||static_cast<std::uint32_t>(node)>=count)return false;
            const auto& lanes=reads_.local[static_cast<std::size_t>(node)];
            auto vector=[&](unsigned first){return RaceStartVector{
                actual_.bank.read(i,lanes[first]),actual_.bank.read(i,lanes[first+1]),
                actual_.bank.read(i,lanes[first+2]),actual_.bank.read(i,lanes[first+3])};};
            out.racers[i]={key(identity_[i].racerTag),key(identity_[i].bodyNodeTag),vector(12),vector(8),vector(4)};
            auto& effect=effects.racers[i];effect.racerKey=out.racers[i].racerKey;
            effect.wordAE8=actual_.bank.read(i,reads_.activeAE8);effect.word844=actual_.bank.read(i,reads_.word844);
            for(std::size_t j=0;j<5;++j)effect.wordsAD4ThroughAE4[j]=actual_.bank.read(i,reads_.timers[j]);
        }
        return true;
    }
    bool stableFrame(StableRaceCameraFrame& frame) const {
        if(!nativeRacerCameraBindingsCurrent())return false;
        if(!identitiesCurrent()||((actual_.all8Manager.flags58&1u)&&!actual_.humanCount34))return false;
        const auto state=actual_.bank.read(0,reads_.state40),retirement=actual_.bank.read(0,reads_.retirementCD4);
        if(state>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())||
           retirement>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return false;
        frame={generation_,key(identity_[0].racerTag),key(identity_[0].mainBodyTag),frameDelta_,
            actual_.all8Manager.flags58,raceFlags1B0_,static_cast<std::int32_t>(state),
            static_cast<std::int32_t>(retirement),actual_.humanCount34?actual_.race94:0.f};
        // Human ending borrows the actual race94 lifetime. It is neither
        // source manager30 nor an estimated presentation duration.
        return true;
    }
    OriginalAttachedFrame attachedFrame() const {
        if(!nativeRacerCameraBindingsCurrent())return {}; //Invalid identity, never fallback.
        return {generation_,key(identity_[0].racerTag),key(identity_[0].mainBodyTag),key(identity_[0].modelTag),
            actual_.bank.read(0,reads_.a4),actual_.bank.read(0,reads_.velocityY),frameDelta_};
    }
    OriginalCameraStartResult serviceBeforeChild() noexcept {
        std::int32_t ignored=0;
        return serviceOriginalRenderer244D60(displayInputs_->rendererDispatch,false,
            displayInputs_->renderer,displayInputs_->diagnostic,ignored);
    }
    bool updateSpatial() noexcept {
        lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
        if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
           !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return false;
        auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return false;
        const SpringVector4_48N position{{pose_.position.x,pose_.position.y,pose_.position.z,pose_.position.w}};
        result_.spatial=updateOriginalCameraScene114C44(*world,spatial_,position,*work);
        return result_.spatial.status==LiveZoneStatus50EN::Complete&&
            readOriginalFollowSceneBinding(spatial_,visibilityKeys_.view(),sceneBinding_);
    }
    bool completionPrefix() noexcept {
        //175D70 common prefix on the frame whose already-completed7C exits.
        //The intro helper deliberately returns before consuming this prefix.
        namespace d=original_race_camera_detail;
        if(controller_.mode44!=7||!start_.complete240||
           !d::scalar(frameDelta_)||frameDelta_<0||!d::scalar(controller_.cooldown8C)||
           !d::scalar(controller_.cutRemaining90))return false;
        auto next=controller_;auto blur=actual_.blur;
        if(next.cooldown8C>0){next.cooldown8C=d::sub(next.cooldown8C,frameDelta_);if(!d::scalar(next.cooldown8C))return false;}
        if(next.cutRemaining90>=0){
            const float amount=d::mul(100,d::div(next.cutRemaining90,2));std::int32_t strength=0;
            if(!d::strength(amount,strength))return false;requestGameplayBlur(blur,strength,0,0);
            if(next.cutRemaining90==0)next.cutRemaining90=-1;
            else {const float remaining=d::sub(next.cutRemaining90,frameDelta_);
                if(!d::scalar(remaining))return false;next.cutRemaining90=remaining<=0?0:remaining;}
        }else if(next.shakeRemaining94>=0){
            // 175E44..175F4C shake blur timer, as in the intro timer prefix.
            // Mode 7 reaches neither mode0/5 terminal call.
            if(!d::scalar(next.shakeDelayA0))return false;
            if(next.shakeDelayA0>0){next.shakeDelayA0=d::sub(next.shakeDelayA0,frameDelta_);if(!d::scalar(next.shakeDelayA0))return false;}
            else{
                std::int32_t strength=75;
                if(next.shakeDecayingA9){
                    if(!d::scalar(next.shakeDuration98)||next.shakeDuration98==0)return false;
                    if(!d::strength(d::mul(80,d::div(next.shakeRemaining94,next.shakeDuration98)),strength))return false;
                }
                requestGameplayBlur(blur,strength,0,0);
                if(next.shakeRemaining94==0){next.shakeRemaining94=-1;next.shakeDuration98=0;}
                else{const float remaining=d::sub(next.shakeRemaining94,frameDelta_);
                    if(!d::scalar(remaining))return false;next.shakeRemaining94=remaining<=0?0:remaining;}
            }
        }
        next.flags63Low5=static_cast<std::uint8_t>(next.flags63Low5&~0x18u);
        controller_=next;actual_.blur=blur;return true;
    }
    Result advanceExit(){
        lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
        if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
           !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return fail(Status::visibility,0x177ab0);
        auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return fail(Status::owner);
        auto zones=visibilityKeys_.view();OriginalFollowSceneTransitionInput transition;
        const bool projected=actual_.bank.visit(0,[&](){
            return readOriginalAttachedSceneTransitionInput(attached_.binding,key(scene_),
                {actual_.arena.nodes,actual_.arena.sceneNodeCount},actual_.collisionWorld,
                actual_.mainMembership690,zones,transition);
        });
        if(!projected)return fail(Status::owner,0x177b18);
        const auto& preference=actual_.preferences.slots[0];
        if(preference.field11030>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())||
           preference.field11038>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return fail(Status::sourcePath,0x178700);
        auto& app=*displayInputs_;
        auto exitScene=sceneBorrow();
        //177AB0 writes actual scene180 before the exit viewport rebuild.
        //That is the spatial/numeric scene binding, not the prior intro output.
        exitScene.position180=&sceneBinding_.position180;
        OriginalGcGaIntroExitBindings bindings{controller_,start_,introChild_,attached_,attachedChild_,follow_,followChild_,
            sceneBinding_,*world,spatial_,zones,*work,transition,display_,exitScene,app.video,
            actual_.applicationClock,countdown_,actual_.all8Manager,startActions_,raceFlags1B0_,uiFlags1E4_,
            app.fadeManagerA8,app.currentApplicationMode22C(),app.application33C,
            static_cast<std::int32_t>(actual_.launchKind11258),
            static_cast<std::int32_t>(preference.field11030),static_cast<std::int32_t>(preference.field11038),
            taPositive_&&actual_.taIntroAudio->taIntroEntity80()?key(*actual_.taIntroAudio->taIntroEntity80()):0,
            static_cast<std::int32_t>(actual_.bank.read(0,reads_.state40)),
            &actual_.arena.noiseOwner(),actual_.startMusicContext,actual_.startMusicRequest,actual_.taIntroAudio,taExitReason_,actual_.introMusicFadeRequest,&f8Intro_};
        // Effective zero-cut paths retain actual80=null. Positive TA/RA passes
        // the real retained emitter and stops it at the ordered2924A0 stage;
        // stop does not retire its Entity or race-owned Sounds.
        result_.start=directStartPending_?directStart_.advance(bindings,app.renderer,app.diagnostic):
            exit_.advance(bindings,actual_.bank,actual_.baseTail,actual_.arena.fields,app.renderer,app.diagnostic);
        if(result_.start.status!=OriginalCameraStartStatus::complete)
            return set(Status::exitPending,result_.start.sourcePC);
        if(!countdownInstalled_){
            auto& host=actual_.hostRace.rows();
            //The already initialized route/standings owner is retained. This
            //is the ONE advancing countdown; source34 is only its start input.
            if(host.phase!=race_session50HM::Phase::Countdown||host.racerCount!=actual_.bank.count()||host.raceSeconds!=0||
               host.finishCount||host.resultsClosed||(actual_.all8Manager.flags58&2u)||
               !std::isfinite(countdown_.countdown34)||countdown_.countdown34<0)
                return fail(Status::sourcePath,0x1afd80);
            host.config.countdownSeconds=static_cast<double>(countdown_.countdown34);
            host.countdownRemaining=host.config.countdownSeconds;
            countdownInstalled_=true;
        }
        frameStage_=FrameStage::exitPose;return set(Status::complete,0x1767c4);
    }
#include "original_ta_camera_lifecycle.inl"
public:
    explicit Session(ActualOwners actual) noexcept;
    OriginalWreckComponent50LA* taControllerEntity174A30()noexcept {
        return coldController_.started&&!coldController_.released&&!closed_?&coldController_.entity:nullptr;
    }
    bool constructTaIntro177750(std::shared_ptr<const rr::assets::AudioBank50HC> bank,
        std::size_t firstVoice,std::size_t voiceCount)noexcept {
        return constructTaIntroAudio177750(std::move(bank),firstVoice,voiceCount);
    }
    // Counted race with a zero 174E48 cut count: 174A30 only (174F9C skips
    // 174FA4 and 177750). Caller first proves fresh Application admission.
    bool constructCountedControllerWithoutCuts()noexcept {
        return constructCountedControllerWithoutCuts174A30();
    }
    const rr::assets::OriginalTaCutAuthoringCandidate* taCutAuthoring()const noexcept {
        return nativeTaCutAuthoring_?nativeTaCutAuthoring_->authoring.get():nullptr;
    }
    ~Session();
    Session(const Session&)=delete;Session& operator=(const Session&)=delete;
    Session(Session&&)=delete;Session& operator=(Session&&)=delete;
    const Result& diagnostic()const noexcept{return result_;}
    bool ready()const noexcept{return coldBound_;}
    bool abandoned()const noexcept{return abandoned_;}
    bool countdownInstalled()const noexcept{return countdownInstalled_;}
    // 1A8C40 / 197040 / 1A8C78: build, load and attach the race's start
    // overlay. Called once from the common race load for every course.
    bool loadStartOverlay(std::shared_ptr<const rr::assets::OriginalUiArchive50KG> appStart,
        std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays,
        const rr::assets::OriginalUiMaterialRuntime50KG& runtime,
        OriginalRaceStartOverlayDiagnostic50HU& d)noexcept{
        if(startOverlay_)return false;
        try{auto owner=std::make_shared<OriginalRaceStartOverlay50HU>();
            if(!owner->load197040(std::move(appStart),std::move(overlays),runtime,d))return false;
            //1976B0 reads race+8C; the port runs one full-screen view (mode 0).
            if(!owner->attach1976B0(0))return false;
            startOverlay_=std::move(owner);return true;
        }catch(...){return false;}
    }
    std::shared_ptr<const OriginalRaceStartOverlay50HU> startOverlay()const noexcept{return startOverlay_;}
    // 1982C0/197C10 for the drawn view; returns the draw (with its 1A4B80
    // sound events) or null when the overlay is absent.
    const OriginalRaceStartOverlayDraw50HU* drawStartOverlay(std::uint32_t drawnView268)noexcept{
        if(!startOverlay_)return nullptr;
        OriginalRaceStartOverlayInputs50HU in{countdown_.countdown34,actual_.all8Manager.flags58,
            raceFlags1B0_,raceEntityFlags29_,drawnView268};
        if(!startOverlay_->draw198300(in))return nullptr;
        return &startOverlay_->nativeDraw();
    }
    float countdown34()const noexcept{return countdown_.countdown34;}
    // True once, on the step whose 1AFE30 ran. Root then performs 1AEDD0's
    // per-racer starts and the 287B60 record start before any racer step.
    bool takeGoEdge1AFE30()noexcept{const bool edge=goEdgePending_;goEdgePending_=false;return edge;}
    bool goPublished()const noexcept{return goPublished_;}
    // Actual permanent rig identity (racer+60) of a race slot; 0 if absent.
    std::uint32_t racer60(std::size_t slot)const noexcept{return slot<actual_.racer60.size()?actual_.racer60[slot]:0u;}
    bool introExitReady()const noexcept{return controller_.mode44==7&&start_.complete240;}
    // Read-only smoke progress; this neither authorizes nor advances a cut.
    std::array<std::int32_t,4> nativeStartProgress()const noexcept {
        return {{static_cast<std::int32_t>(controller_.mode44),start_.nextRoute1F8,
            start_.routeCount1FC,countdownInstalled_?1:0}};
    }
    bool willUseFollowPose()const noexcept {
        // The first ending Follow transition can start from Attached mode5.
        // Its genuine17A870 spatial query needs the same live animated
        // candidates as an already selected Follow pose, before this child.
        const bool endingFollow=actual_.humanCount34&&(actual_.all8Manager.flags58&1u)&&
            !(raceFlags1B0_&0x10u)&&actual_.race94>0.f&&controller_.specialDelayA4<=0.f&&
            actual_.bank.read(0,reads_.retirementCD4)==0u;
        // Death120 traverses the same animated collision world, including
        // its first transition from Attached. Refresh those owners as well.
        const auto racerState=actual_.bank.read(0,reads_.state40);
        return controller_.mode44==0||controller_.mode44==1||racerState==3u||racerState==4u||endingFollow||
            (introExitReady()&&actual_.preferences.slots[0].field11030==0);
    }
    // Called with the actual bank0 arena, immediately after genuine
    //1A24F0/216A30 returned. There is no nested bank visit, frame update,
    // Follow-mode selection or view substitution. Even Attached mode resets
    // the real controller64 Follow object, exactly as225B28/1788B0 require.
    Result resetHumanUndertrack225B0C(const UndertrackSelection50ET& selected,
        const UndertrackPlacement50ET& placed,std::uint64_t serial) noexcept {
        return resetHuman225B0C(selected,placed,serial,false);
    }
    Result resetHumanController225B0C(const UndertrackSelection50ET& selected,
        const UndertrackPlacement50ET& placed,std::uint64_t serial) noexcept {
        return resetHuman225B0C(selected,placed,serial,true);
    }
    Result resetHuman225B0C(const UndertrackSelection50ET& selected,
        const UndertrackPlacement50ET& placed,std::uint64_t serial,bool input) noexcept {
        invalidateFogPreparation();
        using Stage=HumanResetCamera::Stage;
        if(!actual_.followQueries.get()||!coldBound_||closed_||abandoned_||pendingDisplayWork()||!serial||
           actual_.humanCount34!=1u||actual_.arena.fields.vehicle58!=0u||
           actual_.arena.phase!=(input?Phase::Ready:Phase::PostPackedBytes50S)||
           (frameStage_!=FrameStage::idle&&frameStage_!=FrameStage::published)||
           !controller_.bound||controller_.binding.racerClass!=CameraRacerClass::player||
           controller_.binding.managerIndex!=0u||!follow_.prepared||
           follow_.sessionGeneration!=generation_||follow_.racerKey!=controller_.binding.racerKey||
           follow_.mainBodyKey!=controller_.binding.mainBodyKey||
           spatial_.sceneKey!=controller_.binding.sceneKey||spatial_.sessionGeneration!=generation_)
            return set(Status::owner,0x225b24u);
        auto& cursor=humanResetCamera_;
        if(cursor.serial!=serial||cursor.input!=input){
            if(cursor.stage!=Stage::idle&&cursor.stage!=Stage::complete)return set(Status::order,0x225b28u);
            cursor={};cursor.serial=serial;cursor.input=input;cursor.visibilityZone=placed.vehicleC30;
            cursor.collisionZone=placed.vehicleC34;
            std::memcpy(cursor.anchor.v,selected.anchorPosition,16);cursor.stage=Stage::motion;
        }else if(cursor.visibilityZone!=placed.vehicleC30||cursor.collisionZone!=placed.vehicleC34||
            std::memcmp(cursor.anchor.v,selected.anchorPosition,16))return set(Status::owner,0x178874u);
        lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
        if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
           !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))
            return set(Status::visibility,0x178888u);
        auto* world=lease.world();auto* workspace=lease.workspace();
        if(!world||!workspace||!cursor.visibilityZone||cursor.visibilityZone>lease.zoneCount()||
           !cursor.collisionZone||cursor.collisionZone>actual_.collisionWorld.zoneCount)
            return set(Status::owner,0x1788a8u);
        const auto& zero=actual_.followQueries.get()->sharedZero433DB0();
        for(;;)switch(cursor.stage){
        case Stage::motion:
        case Stage::finalMotion:
            if(follow_.revision==std::numeric_limits<std::uint64_t>::max())return set(Status::owner,0x17bce0u);
            //17BCE0 and179C70 copy the same initialized application packet.
            // Filters,114 hit state, presets, activation and timer state survive.
            std::memcpy(&follow_.scratch180,&zero,16);
            std::memcpy(&follow_.collision.localPoint1A0,&zero,16);++follow_.revision;
            cursor.stage=cursor.stage==Stage::motion?Stage::remove:Stage::complete;break;
        case Stage::remove:
            result_.spatial=removeOriginalCameraScene1148D0(*world,spatial_,*workspace);
            if(result_.spatial.status!=LiveZoneStatus50EN::Complete)return set(Status::visibility,0x17888cu);
            cursor.stage=Stage::position;break;
        case Stage::position:
            //1788A0 stores the ORIGINAL pre-lift spline anchor, never BC0's
            // later terrain/water/ground adjusted position.
            spatial_.position180=cursor.anchor;std::memcpy(&pose_.position,&cursor.anchor,16);
            cursor.stage=Stage::attach;break;
        case Stage::attach:
            result_.spatial=attachOriginalCameraScene114770(*world,spatial_,cursor.visibilityZone,
                cursor.anchor,*workspace);
            if(result_.spatial.status!=LiveZoneStatus50EN::Complete)return set(Status::visibility,0x1788a8u);
            cursor.stage=Stage::project;break;
        case Stage::project:
            if(!readOriginalFollowSceneBinding(spatial_,visibilityKeys_.view(),sceneBinding_))
                return set(Status::owner,0x1788a8u);
            cursor.stage=Stage::queries;break;
        case Stage::queries:{
            if(!actual_.followQueries.get()->sync())return set(Status::owner,0x179a80u);
            OriginalCameraPairResult result;
            const auto status=resetOriginalCameraQueryPair179A80(actual_.followQueries.get()->pairOwner(),
                cursor.collisionZone,cursor.anchor,follow_.queryRadius178,cursor.queries,result);
            // Publish actual successful registry prefixes even after rejection.
            const bool committed=actual_.followQueries.get()->commit();
            if(!committed)return set(Status::owner,0x179a80u);
            if(status!=OriginalCameraPairStatus::Complete)return set(Status::sourcePath,result.sourcePC);
            cursor.stage=Stage::finalMotion;break;}
        case Stage::complete:return set(Status::complete,0x225b74u);
        default:return set(Status::order,0x225b28u);
        }
    }
    std::int32_t cameraMode()const noexcept{return controller_.mode44;}
    // Original projected shadows (1B32E0): race +B4 settings flags +58, managerB0
    // +34 human count, and this view camera's follow record index (+64 -> +100).
    std::uint8_t shadowManagerFlags58()const noexcept{return actual_.all8Manager.flags58;}
    std::uint32_t shadowHumanCount34()const noexcept{return static_cast<std::uint32_t>(actual_.humanCount34);}
    std::uint32_t shadowFollowRecord100()const noexcept{return static_cast<std::uint32_t>(follow_.current100);}
    std::int32_t authoredSceneCutCount3E4()const noexcept{return startActions_.sourceSceneCutCount3E4();}
    std::int32_t effectiveControllerCutCountA8()const noexcept{return startPrelude_.controllerCutCountA8();}
    std::uint64_t sessionGeneration()const noexcept{return generation_;}
    std::uint64_t racerKey(std::size_t i)const noexcept{return i<actual_.bank.count()?key(identity_[i].racerTag):0;}
    const OriginalRacerAnimationCold50LA* rightAnimation1A4(std::size_t i)const noexcept {
        const auto* model=actual_.actualRightModels&&i<actual_.actualRightModels->size()?
            (*actual_.actualRightModels)[i].get():nullptr;
        return model&&model->animation1A4().constructed1536F0()?&model->animation1A4():nullptr;
    }
    const OriginalRacerAnimationCold50LA* leftAnimation1A8(std::size_t i)const noexcept {
        const auto* model=actual_.actualLeftModels&&i<actual_.actualLeftModels->size()?
            (*actual_.actualLeftModels)[i].get():nullptr;
        return model&&model->animation1A8().constructed1536F0()?&model->animation1A8():nullptr;
    }
    const native_racer50HW::Rig* actualRacerRig(std::size_t i)const noexcept {
        return actual_.actualRigs&&i<actual_.actualRigs->size()?(*actual_.actualRigs)[i].get():nullptr;
    }
    std::uint64_t mainBodyKey(std::size_t i)const noexcept{return i<actual_.bank.count()?key(identity_[i].mainBodyTag):0;}
    // Bound once from an actual cold Session when a selected render consumer
    // requires the source fog phase. This is a nonowning scheduling capability,
    // never a prepared camera publication or substitute renderer authority.
    const rr::game::OriginalRacerDiffuseCamera50LA* racerDiffuseCamera() noexcept {
        racerDiffuseCamera_.owner_=this;
        racerDiffuseCamera_.read_=[](const void* opaque,rr::game::OriginalRacerDiffuseCameraSnapshot50LA& out) noexcept {
            const auto& session=*static_cast<const Session*>(opaque);
            FogPreparation p;if(!session.readFogPreparation(p))return false;
            rr::game::OriginalRacerDiffuseCameraSnapshot50LA next;
            next.owner=reinterpret_cast<std::uintptr_t>(&session);next.revision=p.revision;
            next.generation=p.generation;next.publication=p.publication;next.frame=p.frame;
            next.camera=p.camera;next.guardCombined=p.guardCombined;next.fog=p.source;
            out=next;return true;
        };
        return &racerDiffuseCamera_;
    }
    class GameplayFogRenderPhase final {
        friend class Session;
        Session* owner_=nullptr;
        const OriginalRaceFogFields* fog_=nullptr;
        std::uint64_t generation_=0;
    public:
        bool bound()const noexcept{return owner_!=nullptr;}
        Result prepare(Session* actual)const noexcept {
            if(!actual||actual!=owner_||actual->generation_!=generation_||
               !actual->displayInputs_||actual->displayInputs_->trackFog3EC!=fog_||
               actual->display_.actualTrackFog3EC!=fog_){
                Result rejected;rejected.status=Status::phase;rejected.sourcePC=0x13ea30;return rejected;
            }
            return actual->prepareGameplayFogRender();
        }
    };
    bool bindGameplayFogRenderPhase(GameplayFogRenderPhase& phase)noexcept {
        if(phase.bound()||!actual_.followQueries.get()||!coldBound_||closed_||abandoned_||!generation_||!displayInputs_||
           !displayInputs_->trackFog3EC||display_.actualTrackFog3EC!=displayInputs_->trackFog3EC)return false;
        phase.owner_=this;phase.fog_=displayInputs_->trackFog3EC;phase.generation_=generation_;return true;
    }
    // Source fog13EA30/13EA60 changes the SAME active camera range before
    // world/particle draw. Native scheduling invokes this after the published
    // camera pose, before either world49F or ParticleCode projection. It does
    // not dispatch a child, tick visibility, sample RNG or publish a new pose.
    Result prepareGameplayFogRender() noexcept {
        invalidateFogPreparation();
        if(!actual_.followQueries.get()||!coldBound_||closed_||abandoned_||!hasView()||!displayInputs_||
           !displayInputs_->trackFog3EC||frameStage_!=FrameStage::published)
            return set(Status::phase,0x13ea30);
        if(fogPreparationRevision_==std::numeric_limits<std::uint64_t>::max())
            return fail(Status::order,0x13ea30);
        const auto& fog=*displayInputs_->trackFog3EC;
        if(!original_race_camera_detail::scalar(fog.start34)||
           !original_race_camera_detail::scalar(fog.end38)||fog.end38<=1||fog.end38<=fog.start34)
            return fail(Status::numeric,0x13ea30);
        //114F20 source clamp: near>=1 and far<=100000. Authenticated GC
        //scene has near1/end2666.6499, so neither value needs an invented cap.
        auto next=scene_;next.nearPlane=1;
        next.farPlane=fog.end38>100000.f?100000.f:fog.end38;
        result_.start=rebuildOriginalRaceScene236DD0(viewport_,next,pose_.position,matrices_);
        if(result_.start.status!=OriginalCameraStartStatus::complete)
            return fail(Status::numeric,result_.start.sourcePC);
        scene_=next;publishFogPreparation(fog);return set(Status::complete,0x13ea60);
    }
    const CameraViewOutput49F& view()const noexcept{return matrices_;}
    const OriginalCameraViewportFields& viewport()const noexcept{return viewport_;}
    const TVCameraUpdateOutput49F& pose()const noexcept{return pose_;}
    // 175650 load + 1752D0 115DE0 registration, one call for every course and
    // both session routes (after prepare). The native collection order is not
    // retail ordinal parity (existing note at cold phase 6); 115E60 selects by
    // identity, so appending the TV key is equivalent.
    bool loadReplayTv175650(const std::vector<std::uint8_t>& archive,std::size_t archiveBytes,const char* code)noexcept{
        if(replayTv_.present()||!collection_.bound||collection_.registered38!=childOrder_.data())return false;
        if(!replayTv_.load175650(archive,code,archiveBytes))return false;
        if(collection_.count30>=childOrder_.size())return false;
        childOrder_[collection_.count30]=replayTv_.childKey();++collection_.count30;
        return true;
    }
    bool replayTvPresent()const noexcept{return replayTv_.present();}
    const rr::assets::TVCameraDiagnostic49E& replayTvLoadError()const noexcept{return replayTv_.lastError();}
    // 1AB520 stores race1B0 bit4 before 1AC8E0; 1ACE14 (and 1A81DC..1A8204 at
    // every replay loop): with race48+78, 2D30B0 then 177AB0(race48,6,0,-1).
    bool enterResultsReplayCamera1ACE14()noexcept{
        raceFlags1B0_=static_cast<std::uint8_t>(raceFlags1B0_|0x10u);
        if(!replayTv_.present())return true;
        replayTv_.reset2D30B0();
        return requestReplayTvMode177AB0();
    }
private:
    bool requestReplayTvMode177AB0()noexcept{
        lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
        if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
           !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return false;
        auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return false;
        const auto zones=visibilityKeys_.view();
        const auto r=original_replay_tv50PD::transitionOriginalTv177AB0(controller_,replayTv_,collection_,
            sceneBinding_,*world,spatial_,zones,*work);
        if(r.status!=OriginalFollowControllerStatus::complete)return false;
        pose_.position=sceneBinding_.position180;   // 177AB0 scene180 write
        return true;
    }
    // 28B180 / 28B1A0 of racer340->288 for the bank slot being visited.
    rr::assets::TVCameraVector49E bodyLocal288()const noexcept{
        const auto& m=actual_.arena.nodes[static_cast<std::uint32_t>(actual_.bindings.animation.bodyScene288)].localC0;
        return {m.v[12],m.v[13],m.v[14],1.f};
    }
    rr::assets::TVCameraVector49E bodyWorld288()const noexcept{
        const auto& m=actual_.arena.nodes[static_cast<std::uint32_t>(actual_.bindings.animation.bodyScene288)].world100;
        return {m.v[12],m.v[13],m.v[14],1.f};
    }
public:
    bool nativeHudVisible()const noexcept{return !(uiFlags1E4_&1u);}
    // The prepared 174EF4 death-camera identity the wreck owner rebinds its
    // racer370/374 debris view to.
    const OriginalDeathCameraIdentity50KG& deathIdentity()const noexcept{return death_.identity;}
    bool hasView()const noexcept{return publicationCount_!=0;}
    // Explicit shared-mixer consumed-step listener adaptation, not the omitted
    // retail music-update prefix/suffix. Never a render or End callback.
    bool advanceNativeMusicListenerConsumedStep(std::uint64_t beforeMix,
        bool heldSourceScene,std::uint32_t& sourcePC)const noexcept;

    // Concrete native presentation join for the consumed human RA route only.
    // Rejects unissued/foreign Sessions; no fallback success or source callback
    // acknowledgment. Frame must outlive output and its synchronous submit.
    bool prepareNativeF8Frame50LA(rr::platform::VehicleWindowFrame50DS&,
        std::unique_ptr<original_main_entry50KG::NativeF8BarPrefix50LA>&,
        rr::game::OriginalRaceF8Diagnostic50LA&)const noexcept;
    // Exact camera scene38 parent, projected to the original zero-based zone48.
    // This is never a racer collision zone, AI map node or visible-zone guess.
    bool readCameraPrimaryZone48(std::uint32_t& out) noexcept {
        if(!actual_.followQueries.get()||!coldBound_||closed_||abandoned_||!hasView()||!spatial_.registration34.attached||
           !spatial_.parent38||spatial_.parent38>visibilityKeys_.count)return false;
        lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
        if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
           !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return false;
        OriginalFollowSceneBinding actual;
        if(!readOriginalFollowSceneBinding(spatial_,visibilityKeys_.view(),actual)||
           actual.sceneKey!=sceneBinding_.sceneKey||actual.parentZoneKey!=sceneBinding_.parentZoneKey||
           actual.registrationKey!=sceneBinding_.registrationKey)return false;
        out=spatial_.parent38-1u;return true;
    }
    // Original per-frame zone (section) visibility for the track renderer,
    // the same for every course: this camera's scene38 zone, the course's
    // authored portals (272B3C) and that zone's ExclusionList (27632C), walked
    // by 27F650/27FEE0 (track_zone_visibility50zv). false = not available this
    // frame; the caller then supplies no zones (frustum-only, as before).
    // Reads the lease's portal world and authoring only; writes nothing in it.
    bool prepareTrackZones50ZV(rr::platform::VehicleWindowTrackZone50DS* out,std::size_t capacity,
        std::uint32_t& count,float widescreenScaleX=1.f) noexcept {
        count=0;std::uint32_t root=0;
        if(!out||!readCameraPrimaryZone48(root)||!viewport_.prepared)return false;
        lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
        if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
           !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return false;
        const auto* world=lease.world();
        const auto* authored=lease.authoredRenderZone(root);
        if(!world||!authored||authored->exclusionCount>authored->exclusions.size()||
           world->portals.zoneCount>capacity)return false;
        rr::game::TrackZoneVisibilityInput50ZV in{};
        in.portals=&world->portals;in.rootZone=root;
        in.rootExclusions=authored->exclusions.data();in.rootExclusionCount=authored->exclusionCount;
        in.camera=&view();in.widescreenScaleX=widescreenScaleX;
        // 230450's constant quad, built locally (the Application 434940 copy is
        // RA-owned today); nothing is written into any original structure.
        if(!rr::game::constructPortalViewport230450_50EO(in.viewportQuad))return false;
        in.viewportX=viewport_.x1E0;in.viewportY=viewport_.y1E4;
        in.viewportWidth=viewport_.width1E8;in.viewportHeight=viewport_.height1EC;
        std::array<rr::game::TrackZoneRow50ZV,rr::game::maxTrackZones50ZV> rows{};
        if(rr::game::prepareTrackZoneVisibility50ZV(in,rows.data(),rows.size())!=
           rr::game::TrackZoneVisibilityStatus50ZV::complete)return false;
        for(std::size_t z=0;z<world->portals.zoneCount;++z){
            auto& o=out[z];const auto& r=rows[z];
            o.drawn=r.drawn;o.sourceClip=r.view.matrix60;o.scissor=r.scissor;
            o.viewport={{in.viewportX,in.viewportY,in.viewportWidth,in.viewportHeight}};
        }
        count=static_cast<std::uint32_t>(world->portals.zoneCount);return true;
    }
    std::uint64_t publicationCount()const noexcept{return publicationCount_;}
    bool blockingVideoPending()const noexcept {
        return !abandoned_&&!videoRequestPending50LA()&&displayInputs_&&(displayInputs_->video.flags404Low6&3u)&&
            !(displayInputs_->video.flags404Low6&4u);
    }
    bool asyncVideoPending()const noexcept {
        //Numeric25B5D0 may clear direction bits before the final staged GPU
        //sample/restore retires. Retained owner state, not flags alone, wins.
        return !abandoned_&&!videoRequestPending50LA()&&((taSkipFadeInRequested_&&!taSkipAsyncFade_.complete())||
            (fadeInRequested_&&!asyncFade_.complete())||
            (endingFadeRequested_&&!endingFade_.complete()));
    }
    bool pendingDisplayWork()const noexcept {
        return displayInputs_&&(videoRequestPending50LA()||(displayInputs_->video.flags404Low6&3u)||
            (raceFlags1B0_&8u)||displayInputs_->fadeManagerA8==8||
            (asyncFade_.started()&&!asyncFade_.complete())||
            (taSkipAsyncFade_.started()&&!taSkipAsyncFade_.complete())||
            (endingFadeRequested_&&!endingFade_.complete()));
    }
    std::uint64_t publishedFrame()const noexcept{return completedFrame_;}
    const RaceStartPreparationReceipt& introPreparation()const noexcept{return preparation_;}
    const OriginalRaceCameraOwner& controller()const noexcept{return controller_;}
    //Only an actual authored trigger enter may call this. No event is created
    //from speed/contact or elapsed camera time. Same original camera0 owner.
    bool applyAuthoredShakeEnter(bool entering,const OriginalCameraShakeAction& action) noexcept {
        invalidateFogPreparation();
        if(!actual_.followQueries.get()||!coldBound_||(frameStage_!=FrameStage::idle&&frameStage_!=FrameStage::published&&
            frameStage_!=FrameStage::cameraReady))return false;
        return applyOriginalCameraShakeEnter(entering,action,&controller_,1);
    }

    //Cold after actual ALL8 roster/final-grid/7F0 setup and existing neutral
    //setup tick, before native countdown/GO. The live50Y history is NOT reset.
    //authored, app and every borrowed owner/backing must outlive this Session.
    //generation is the native race lifetime key; it does not assert a guest ID.
    // 1A93B0 Camera34 -> 1A9EE0 track load, the part of the race load that
    // precedes the racers (1AA510): viewport, scene selection and renderer
    // drain, Camera34, music and the track's world load. No racer may be
    // required here; the racer-dependent camera parts run in prepare(), after
    // the racers. prepare() runs this itself when no caller did it first.
    Result prepareTrackCold(std::uint64_t generation,const AuthoredInputs& authored,DisplayInputs& app){
        if(f8Intro_.ownerThread_!=std::this_thread::get_id())return {Status::owner,0x1a93b0};
        if(!actual_.followQueries.get()||constructorOnly_||nativeRaOwnedPreparationAttempted_)return set(Status::terminal,0x1a6d98);
        if(coldBound_||closed_)return set(Status::order);
        if(frameStage_==FrameStage::failed)return set(Status::terminal);
        if(coldPhase_>=2)return set(Status::complete,0x1a6d98);
        if(nativeTaIssuance_&&(generation!=nativeTaIssuance_->generation||authored.course!=nativeTaIssuance_->course))return set(Status::owner);
        if(!generation||!actual_.hostRace.runtimeValid())return set(Status::owner);
        if(displayInputs_&&displayInputs_!=&app)return set(Status::owner);
        if(generation_&&generation_!=generation)return set(Status::owner);
        if(!coldTrackStarted_)invalidateFogPreparation();
        coldTrackStarted_=true;
        generation_=generation;displayInputs_=&app;
        // PORT STAND-IN (flagged): the port's counted Camera34 binding checks
        // the racer identity rows (readsBound_/identitiesCurrent). Where the
        // roster already exists (RA constructs its racers before this point)
        // they are bound here exactly as prepare() binds them; a course that
        // runs this before its racers needs that binding check moved.
        if(!readsBound_&&actual_.bank.count()&&actual_.bank.count()<=8){
            reads_.bind(actual_);readsBound_=true;
            for(std::size_t i=0;i<actual_.bank.count();++i){
                identity_[i].managerIndex=static_cast<std::uint32_t>(i);
                identity_[i].pod60=actual_.racer60[i];
            }
        }
        return runTrackCold(app);
    }
private:
    bool coldTrackStarted_=false;
    Result runTrackCold(DisplayInputs& app){
        if(!bindCountedRaceCameraNative34())return set(Status::owner,0x1a93b0);
        while(coldPhase_<2){
            switch(coldPhase_){
            case 0:
                if(!tick_.bound&&!bindFreshOriginalCameraSceneTick(tick_,generation_,key(scene_)))return set(Status::owner);
                if(!viewport_.prepared){
                    result_.start=prepareOriginalRaceViewport2361C0(viewport_,generation_,key(scene_),scene_);
                    if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::numeric,result_.start.sourcePC);
                }
                pose_.mode=0;pose_.position={0,50,35,1};pose_.look=scene_.look;pose_.up=scene_.up;pose_.fov=scene_.fovDegrees;
                //target is unconsumed on this mode0-only route. Its native
                //scratch initialization does not claim a source stack write.
                display_.sessionGeneration=generation_;display_.raceKey=key(*this);
                display_.rendererBufferCount0C=app.rendererBufferCount0C;
                display_.rendererDispatch=&app.rendererDispatch;display_.actualTrackFog3EC=app.trackFog3EC;
                display_.drainPolicy=app.drainPolicy;
                ++coldPhase_;break;
            default:
                //2361C0 selects26F940 before1A9240 reaches1A9590(true).
                // Select this actual native scene BEFORE the separate cold
                //1158A0/drain/zoom/matrix call.245918 must see its real target,
                // not an empty current-scene slot. This is not source114A00
                // completion or publication of the counted Camera34.
                if(!coldCameraSelected26F940_){
                    result_.start=selectOriginalRaceScene26F940(display_,viewport_,app.rendererWidth4,app.rendererHeight8);
                    //26F948 precedes the dimension branch. Only the completed
                    //equal-dimension path can publish its actual observation.
                    observeSelectedGameplayCamera26F940();
                    if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::sourcePath,result_.start.sourcePC);
                    coldCameraSelected26F940_=true;
                }
                if(!coldViewport_.complete()){
                    // Pending/failing work retains this same selection and
                    // cursor. Resume its reached phases, never select again.
                    result_.start=coldViewport_.advance(true,display_,sceneBorrow(),app.renderer,app.diagnostic);
                    if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::renderer,result_.start.sourcePC);
                }
                // Actual renderer work and selection precede this bounded
                // native adapter acceptance; numeric preparation alone cannot.
                if(!completeCountedRaceCameraNative34())return set(Status::owner,0x1a93b0);
                {
                    std::uint32_t musicPC=0;
                    if(!completeCountedRaceMusicNative10C(musicPC))return set(Status::owner,musicPC);
                }
                {
                    std::uint32_t worldPC=0;
                    if(!completeCountedRaceWorldNativePrefix1A9EE0(worldPC))return set(Status::owner,worldPC);
                }
                ++coldPhase_;break;
            }
        }
        return set(Status::complete,0x1a6d98);
    }
public:
    //Cold after actual ALL8 roster/final-grid/7F0 setup and existing neutral
    //setup tick, before native countdown/GO. The live50Y history is NOT reset.
    //authored, app and every borrowed owner/backing must outlive this Session.
    //generation is the native race lifetime key; it does not assert a guest ID.
    Result prepare(std::uint64_t generation,const AuthoredInputs& authored,DisplayInputs& app){
        if(f8Intro_.ownerThread_!=std::this_thread::get_id())return {Status::owner,0x1a93b0};
        if(!actual_.followQueries.get()||constructorOnly_||nativeRaOwnedPreparationAttempted_)return set(Status::terminal,0x1a6d98);
        if(!coldTrackStarted_)invalidateFogPreparation();
        // The consumed TA/RA constructor has one retained, generation-bound
        // real74 collection. Other unpublished constructor experiments do not
        // admit this source lifecycle, and retired owners cannot be reused.
        // A zero-cut counted controller has no74 by the original 174F9C rule.
        if(coldController_.started&&((!taCuts_&&!zeroCutController_)||coldController_.released||
           coldController_.issuerGeneration!=generation))return set(Status::sourcePath,0x174df0u);
        // Unpublished constructor staging is a different private lifetime,
        // not evidence for the existing reduced native prepare chronology.
        if(nativeTaCutChildPhase_!=NativeTaCutChildPhase::empty)return set(Status::sourcePath,0x282860u);
        if(nativeTaIssuance_&&(generation!=nativeTaIssuance_->generation||authored.course!=nativeTaIssuance_->course))return set(Status::owner);
        if(coldBound_||closed_)return set(Status::order);
        if(frameStage_==FrameStage::failed)return set(Status::terminal);
        const bool mc=authored.course==OriginalCameraStartCourse::mc;
        if(mc?(!authored.mcSourceIdentity||!actual_.actualRadarMapMc.runtimeValid()||
                actual_.actualRadarMapMc.role()!=NavigationRole50MC::Map||actual_.actualRadarMapMc.count()!=173u||
                actual_.actualRadarMapMc.sourceIdentity()!=authored.mcSourceIdentity||
                actual_.actualRadarMapB8||actual_.actualRadarMapMb.gatesPrepared()||actual_.actualRadarMapTc.gatesPrepared()||
                actual_.actualRadarMapTb.gatesPrepared()||actual_.actualRadarMapGb.gatesPrepared()):
              (authored.mcSourceIdentity||actual_.actualRadarMapMc.gatesPrepared()))return set(Status::owner);
        if(!actual_.hostRace.runtimeValid())return set(Status::owner);
        if(!generation||(!actual_.bank.count()||actual_.bank.count()>8)||actual_.all8Manager.flags58&2u||
           (actual_.launchKind11258<1u||actual_.launchKind11258>3u)||
           actual_.hostRace.rows().phase!=race_session50HM::Phase::Countdown||actual_.hostRace.rows().racerCount!=actual_.bank.count()||
           actual_.hostRace.rows().raceSeconds!=0||actual_.hostRace.rows().finishCount)return set(Status::owner);
        if(displayInputs_&&displayInputs_!=&app)return set(Status::owner);
        if(generation_&&generation_!=generation)return set(Status::owner);
        generation_=generation;displayInputs_=&app;
        if(!readsBound_){
            reads_.bind(actual_);readsBound_=true;
            for(std::size_t i=0;i<actual_.bank.count();++i){
                identity_[i].managerIndex=static_cast<std::uint32_t>(i);
                identity_[i].pod60=actual_.racer60[i];
            }
        }
        if(!identitiesCurrent())return set(Status::owner);
        for(std::size_t i=0;i<actual_.bank.count();++i)if(actual_.bank.read(i,reads_.phase)!=Phase::Ready)return set(Status::phase);
        // 1A93B0 -> 1A9EE0 precede the racers (1AA510) in the original race
        // load. A course that did not run them before its racers runs them
        // here, before every racer-dependent camera part below.
        if(coldPhase_<2){
            coldTrackStarted_=true;
            const auto track=runTrackCold(app);
            if(track.status!=Status::complete)return track;
        }else if(!bindCountedRaceCameraNative34())return set(Status::owner,0x1a93b0);
        // At this native cold caller, genuinely construct the represented
        // racer27E130/7F8/240 prefix, then concrete7FC/model lifetime below.
        // No physics/collection or outer manager attachment is inferred.
        if(!constructRacerEntityPrefixes())return fail(Status::sourcePath,result_.entityCold.sourcePC);
        // Native cold query backing follows the genuine racer identities and
        // prior QueryBridge admission, but precedes any Follow prepare.
        if(!stageNativeRetainedWreckCold())return fail(Status::owner,0x2129c0u);

        if(!constructRacerModelPrefixes())return fail(Status::sourcePath,result_.entityCold.sourcePC);
        for(;;){
            switch(coldPhase_){
            case 2:{
                if(!nativeRaOwnedPreparationAttempted_&&!publishNeverAdoptedNativeIntro())
                    return fail(Status::owner,0x177750);
                //Only never-adopted native preparation may use legacy staging.
                //After source adoption starts, no fallback can publish aliases.
                if(!nativeRaOwnedPreparationAttempted_&&nativeCinematographer70_&&
                   !nativeCinematographer70Retired_&&!nativeCinematographer70Published_){
                    if(coldController_.children64[3]||coldController_.children64[4])
                        return fail(Status::owner,0x174f84);
                    coldController_.children64[3]=nativeCinematographer70_.get();
                    nativeCinematographer70Published_=true;
                    coldController_.children64[4]=taCuts_.get();
                }
                if(authoredPhase_==0){
                    if(!startActions_.prepare(authored.course,authored.tob,authored.tobBytes))return set(Status::authoring,0x1ad330);
                    authoredPhase_=1;
                }else if(startActions_.course()!=authored.course)return set(Status::authoring,0x1ad330);
                if(nativeRaOwnedPreparationAttempted_){
                    //The real World3E4 producer already wrote the SAME counted
                    //controllerA8. Authoring remains corroboration, not authority.
                    if(!coldController_.nativeLoadPrefixComplete||
                       coldController_.nativeLoadOperandsRevoked||!startPrelude_.prepared()||
                       !startPrelude_.currentControllerA8(coldController_.halfA8Value,coldController_.halfA8Known)||
                       startActions_.sourceSceneCutCount3E4()!=startPrelude_.sourceSceneCutCount3E4())
                        return set(Status::owner,coldController_.nativeLoadPrefixPC);
                }else if(!startPrelude_.prepared()&&!startPrelude_.prepare174DF0(startActions_,
                    actual_.launchKind11258,actual_.configuration34,actual_.application350))
                    return set(Status::authoring,0x174e48);
                // A source-positive count reaches the real74 cut collection.
                // Never substitute the procedural7C path for those children.
                if(startPrelude_.controllerCutCountA8()!=0){
                    if(!nativeCamera74ConsumersCurrent())return fail(Status::owner,nativeAttached6C_.pc);
                    if(!taCuts_||startPrelude_.controllerCutCountA8()!=taCuts_->cutCount()||!coldController_.started||
                       !nativeTaIssuance_||nativeTaIssuance_->course!=authored.course||
                       taCuts_->trackId()!=introTrackId(authored.course)||
                       !actual_.taIntroAudio||!actual_.taIntroAuthoring||
                       actual_.taIntroAuthoring->trackId()!=introTrackId(authored.course)||
                       !actual_.startMusicRequest||!actual_.introMusicFadeRequest)
                        return fail(Status::sourcePath,0x174fa4);
                    taPositive_=true;
                    if(!bindTaIntroEntity())return fail(Status::owner,0x177750);
                    if(!taCuts_->loaded()){
                        if(!original_ta_cut_resource_candidate_detail::arithmeticAdmitted())return fail(Status::numeric,0x282cd0);
                        if(actual_.applicationEntities->cutOrigin443200_!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}})
                            return fail(Status::sourcePath,0x282d18);
                        const auto loaded=taCuts_->load175510();
                        if(!loaded.okay())return fail(Status::authoring,loaded.sourcePC);
                    }
                    // Existing native staging already owns controller/audio.
                    // Complete its uncounted RA78 only after actual74 load;
                    // no live registry, selector or phase5 activation follows.
                    if(!constructNativeRaTv78())return fail(Status::owner,taFailurePC_);
                    if(!constructNativeRaStart7C())return fail(Status::owner,taFailurePC_);
                }
                const auto pod=identity_[0].pod60;
                if(pod>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return set(Status::owner);
                if(authoredPhase_==1){
                    result_.follow=loadOriginalFollowCam(static_cast<std::int32_t>(pod),0,authored.cam,authored.camBytes,
                        presets_.data(),presets_.size(),presetBank_);
                    if(result_.follow.status!=OriginalFollowStatus::ok)return set(Status::authoring,result_.follow.sourcePC);
                    authoredPhase_=2;
                }
                if(authoredPhase_==2){
                    if(!readOriginalAttachedOffset215CC0(static_cast<std::int32_t>(pod),authored.phyIdentity,
                        authored.phy,authored.phyBytes,attachedOffset_))return set(Status::authoring,0x215cc0);
                    authoredPhase_=3;
                }
                ++coldPhase_;break;}
            case 3:{
                //A source-adopted64 must eventually use a split179580 loader,
                //not the old standalone composite which repeats its ctor.
                if(nativeCamera6468_.attempted)return fail(Status::sourcePath,
                    nativeAttached6C_.attempted?nativeAttached6C_.pc:0x174f50);
                const auto& i=identity_[0];
                const OriginalFollowIdentity f{generation_,key(i.racerTag),key(i.mainBodyTag),key(i.containerTag),
                    static_cast<std::int32_t>(i.pod60)};
                result_.follow=prepareOriginalFollowCamera(follow_,f,presetBank_);
                if(result_.follow.status!=OriginalFollowStatus::ok)return set(Status::numeric,result_.follow.sourcePC);
                if(!actual_.bank.visit(0,[&](){return actual_.followQueries.get()->prepare(follow_.queryRadius178);}))
                    return fail(Status::owner,0x179a80);
                ++coldPhase_;break;}
            case 4:{
                if(nativeCamera6468_.attempted)return fail(Status::sourcePath,
                    nativeAttached6C_.attempted?nativeAttached6C_.pc:0x174f50);
                const auto& i=identity_[0];
                const OriginalAttachedBinding b{generation_,key(i.racerTag),key(i.mainBodyTag),key(i.modelTag),
                    actual_.bank.read(0,reads_.body288),actual_.bank.read(0,reads_.model1B4),static_cast<std::int32_t>(i.pod60)};
                result_.attached=prepareOriginalAttachedCamera(attached_,b,attachedOffset_);
                if(result_.attached.status!=OriginalAttachedStatus::ok)return set(Status::numeric,result_.attached.sourcePC);
                const OriginalDeathCameraIdentity50KG deathIdentity{generation_,key(i.racerTag),key(i.mainBodyTag),
                    key(death_),actual_.bank.read(0,reads_.body288)};
                result_.death=prepareOriginalDeathCamera50KG(death_,deathIdentity,deathGlobals_);
                if(result_.death.status!=OriginalDeathCameraStatus50KG::complete)return set(Status::numeric,result_.death.sourcePC);
                if(!actual_.followQueries.get()->bindDeathChild(key(death_)))return set(Status::owner,0x1752d0u);
                if(nativeRaOwnedPreparationAttempted_&&!bindNativeRaTvBody1752A0())
                    return fail(Status::owner,taFailurePC_);
                ++coldPhase_;break;}
            case 5:{
                // First-cut selection below reparents visibility, activates
                // the child and dispatches speech/music. The bounded native
                // loader preparation must stop BEFORE any of those effects,
                // not merely before eventual coldBound/gameplay publication.
                if(nativeRaOwnedPreparationAttempted_)
                    return fail(Status::sourcePath,0x1a6d98);
                if(taPositive_&&!taFirstSelected_){
                    if(!bindTaRegistryBefore7C())return fail(Status::owner,0x1752d0);
                    result_.visibilityJoin=attachFreshSelectedOriginalCamera0Visibility50SC(actual_.visibility,generation_,key(scene_),
                        spatial_,visibilityBacking_,visibilityKeys_,sceneBinding_);
                    if(result_.visibilityJoin.status!=OriginalCameraVisibilityJoinStatus::complete)
                        return fail(Status::visibility,0x175224);
                    pose_.position=sceneBinding_.position180;
                    if(!nextTaCut1756D0())return fail(Status::sourcePath,taFailurePC_);
                    taFirstSelected_=true;
                }
                RaceStartRacerBank bank;GcStartRacerEffectBank unused;
                if(!readRacers(bank,unused))return set(Status::owner);
                const auto status=prepareOriginalRaceStartCamera(start_,bank,actual_.sharedRandom,actual_.arena.noiseOwner(),
                    {seedSamples_.data(),seedCount_},preparation_);
                if(status==RaceStartStatus::missingClockSample){
                    //Preparation stages RNG changes atomically. Only a newly
                    //reached missing101A50 requests a real clock sample. Retry
                    //reuses earlier samples and does not replay real RNG draws.
                    if(seedCount_==seedSamples_.size()||
                       !sampleHostTimestamp50ID(actual_.applicationClock,seedSamples_[seedCount_]))return set(Status::clock,0x101a50);
                    ++seedCount_;break;
                }
                if(status!=RaceStartStatus::ok)return set(Status::numeric,0x2d4dc0);
                ++coldPhase_;break;}
            case 6:{
                const RaceCameraBinding b{generation_,key(scene_),key(identity_[0].racerTag),key(identity_[0].mainBodyTag),
                    0,actual_.humanCount34?CameraRacerClass::player:CameraRacerClass::ai,
                    actual_.humanCount34?CameraBootstrap::original_single_player:CameraBootstrap::native_all_ai};
                if(!controller_.bound&&!bindFreshOriginalRaceCamera(controller_,b))return set(Status::owner);
                //Explicit reduced NATIVE registry: actual fully owned children
                //in source relative order. Cold-only70/78 are NOT registered;
                //count/ordinals are not retail parity. No dummy success owners.
                if(taPositive_){
                    //1753CC follows the real first1756D0. Append actual7C only now.
                    childOrder_[3+taCuts_->cutCount()]=key(start_);collection_.count30=4+taCuts_->cutCount();
                    coldController_.children64[6]=&start_;
                }else{
                    childOrder_={{key(follow_),key(death_),key(attached_),key(start_),0,0,0}};
                    if(!bindFreshOriginalCameraChildRegistry(collection_,generation_,key(controller_),key(scene_),
                        childOrder_.data(),4))return set(Status::owner);
                }
                introChild_={&start_,key(start_),&collection_};attachedChild_={&attached_,key(attached_),&collection_};
                followChild_={&follow_,key(follow_),&collection_};
                deathChild_={&death_,key(death_),&collection_};
                objectChildren_={generation_,key(controller_),key(scene_),taPositive_?key(*coldController_.entity.firstChild18):0,0};
                controllerEntity_.controllerKey=key(controller_);display_.controllers48[0]=&controllerEntity_;
                if(taPositive_){
                    if(!bindTaIntroEntity())return fail(Status::owner,0x177750);
                    objectChildren_.actualController=&coldController_.entity;
                    objectChildren_.actualIntroAudio=coldController_.entity.firstChild18;
                }
                ++coldPhase_;break;}
            case 7:
                if(taPositive_){++coldPhase_;break;}
                result_.visibilityJoin=attachFreshSelectedOriginalCamera0Visibility50SC(actual_.visibility,generation_,key(scene_),
                    spatial_,visibilityBacking_,visibilityKeys_,sceneBinding_);
                if(result_.visibilityJoin.status!=OriginalCameraVisibilityJoinStatus::complete){
                    if(result_.visibilityJoin.sceneAttached)return fail(Status::visibility,0x175224);
                    return set(Status::visibility,0x175224);
                }
                pose_.position=sceneBinding_.position180;
                ++coldPhase_;break;
            default:
                if(nativeRaOwnedPreparationAttempted_)
                    return fail(Status::sourcePath,0x1a6d98);
                coldBound_=true;return set(Status::complete,0x1752c0);
            }
        }
    }

    //First actual scene34 child, before the real manager timing/rank child and
    //camera48. It executes114AE0 +236DD0 +114C44 using the prior camera pose.
    //A successful return is only this child, never the complete application.
    Result advanceSceneChild(std::uint64_t frame,Cut cut,float actualRootPreparedDelta){
        invalidateFogPreparation();
        if(!actual_.followQueries.get()||!coldBound_||!displayInputs_)return set(Status::owner);
        if(frameStage_==FrameStage::failed)return set(Status::terminal);
        if(frameStage_==FrameStage::idle||frameStage_==FrameStage::published){
            if(pendingDisplayWork())return set(Status::videoPending);
            if(publicationCount_==std::numeric_limits<std::uint64_t>::max())return fail(Status::order);
            if(!frame||frame<=completedFrame_)return set(Status::order);
            //Held racers have no newly dispatched clock. Use the actual root
            //planned fixed-step delta, never change a racer clock to supply it.
            frameDelta_=actualRootPreparedDelta;
            if(!original_race_camera_detail::scalar(frameDelta_)||frameDelta_<0||frameDelta_>.06f)return set(Status::numeric);
            frameNumber_=frame;cut_=cut;if(!cutCurrent(cut))return set(Status::phase);
            // Explicit native accepted-world scheduling: exactly once per new
            // real sourceCameraTick, including held-start world updates. This
            // recovers the race numeric prefix, not full retail parent order.
            std::uint32_t actualPause11C=0;
            if(!actual_.applicationAnimation.readPause11C(actualPause11C))return set(Status::owner,0x1A7B94);
            if(!raceHudBlink_.advance1A7C78(frameDelta_,raceFlags1B0_,actualPause11C))
                return set(Status::numeric,0x1A7C90);
            frameStage_=FrameStage::earlyDispatch;
        }
        if(frame!=frameNumber_||cut!=cut_||!same(actualRootPreparedDelta,frameDelta_)||!cutCurrent(cut))return set(Status::order);
        for(;;){switch(frameStage_){
        case FrameStage::earlyDispatch:
            result_.start=serviceBeforeChild();
            if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::renderer,result_.start.sourcePC);
            frameStage_=FrameStage::earlyTick;break;
        case FrameStage::earlyTick:
            if((displayInputs_->rootFlags228&4u)||(sceneFlags29_&4u))return fail(Status::sourcePath,0x2363b0);
            if(advanceOriginalCameraSceneTick114AE0(tick_,generation_,key(scene_),pose_.position,scene_.look,sceneFlags29_)!=
                OriginalCameraSceneTickStatus::needsMatrixVisibilityAndChildren)return fail(Status::numeric,0x114ae0);
            frameStage_=FrameStage::earlyMatrix;break;
        case FrameStage::earlyMatrix:
            result_.start=rebuildOriginalRaceScene236DD0(viewport_,scene_,pose_.position,matrices_);
            if(result_.start.status!=OriginalCameraStartStatus::complete)return fail(Status::numeric,result_.start.sourcePC);
            frameStage_=FrameStage::earlyVisibility;break;
        case FrameStage::earlyVisibility:
            if(!updateSpatial())return fail(Status::visibility,0x114c44);
            frameStage_=FrameStage::cameraReady;return set(Status::sceneChildComplete,0x114ca0);
        default:return set(Status::order);
        }}
    }

    //Call after the real native world/timing/UI services chosen by the root,
    //at this SAME cut. Existing host-selected Player-derived audio/RNG suffix
    //runs in its existing caller later; this does not suppress/reproduce it.
    Result advanceCameraChild(std::uint64_t frame,Cut cut){
        result_.stableFollowAttempted=false;result_.stableAttachedAttempted=false;
        result_.cameraFrameMatches=false;result_.cameraCutMatches=false;result_.cameraCutAccepted=false;
        result_.cameraChildStage=static_cast<std::uint32_t>(frameStage_);
        invalidateFogPreparation();
        if(!actual_.followQueries.get()||!coldBound_||!displayInputs_)return set(Status::owner);
        if(frameStage_==FrameStage::failed)return set(Status::terminal);
        result_.cameraFrameMatches=frame==frameNumber_;result_.cameraCutMatches=cut==cut_;
        if(!result_.cameraFrameMatches||!result_.cameraCutMatches)return set(Status::order);
        result_.cameraCutAccepted=cutCurrent(cut);
        if(!result_.cameraCutAccepted)return set(Status::order);
        if(frameStage_==FrameStage::cameraReady)frameStage_=FrameStage::cameraDispatch;
        for(;;){result_.cameraChildStage=static_cast<std::uint32_t>(frameStage_);switch(frameStage_){
        case FrameStage::cameraDispatch:
            result_.start=serviceBeforeChild();
            if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::renderer,result_.start.sourcePC);
            frameStage_=FrameStage::cameraUpdate;break;
        case FrameStage::cameraUpdate:{
            StableRaceCameraFrame current;if(!stableFrame(current)){std::fputs("intro_camera_stable_frame_refused\n",stderr);return fail(Status::sourcePath,0x175d70);}
            if(taPositive_&&(controller_.mode44==4||(controller_.mode44==7&&(controller_.flags62&0x80u)))){
                if(current.racerState40!=0||(current.raceFlags1B0&3u)!=1u||
                   (current.raceFlags1B0&0x10u)||((current.raceStateFlags58&1u)&&current.racerCD4==0)){
                    std::fprintf(stderr,"intro_camera_frame_refused racer40=%u race1B0=%u flags58=%u racerCD4=%u mode44=%u\n",
                        unsigned(current.racerState40),unsigned(current.raceFlags1B0),unsigned(current.raceStateFlags58),
                        unsigned(current.racerCD4),unsigned(controller_.mode44));
                    return fail(Status::sourcePath,0x175d70);}
                if(!advanceTaCut175D70()){std::fprintf(stderr,"intro_camera_cut_refused pc=%x\n",taFailurePC_);
                    return fail(Status::sourcePath,taFailurePC_);}
                if(frameStage_==FrameStage::exit)break;
                if(frameStage_==FrameStage::taSkipVideo)return set(Status::videoPending,0x260850);
                frameStage_=FrameStage::lateTick;break;
            }
            if(controller_.mode44==4||controller_.mode44==7){
                RaceStartRacerBank bank;GcStartRacerEffectBank fields;if(!readRacers(bank,fields))return fail(Status::owner);
                const auto& p=actual_.preferences.slots[0];
                if((actual_.launchKind11258<1u||actual_.launchKind11258>3u))return fail(Status::sourcePath,0x175d70);
                const GcCameraStartContext context{startPrelude_.controllerCutCountA8(),static_cast<std::int32_t>(actual_.launchKind11258),static_cast<std::int32_t>(p.field11030),
                    static_cast<std::int32_t>(p.field11038),displayInputs_->rootFlags228,sceneFlags29_};
                const auto r=advanceOriginalGcCameraStartFrame(controller_,start_,introChild_,fields,bank,current,
                    context,actual_.blur,scene_,pose_,matrices_);
                if(r.status==GcStartFrameStatus::needsDirectStart){
                    directStartPending_=true;frameStage_=FrameStage::exit;break;
                }
                if(r.status==GcStartFrameStatus::needsOriginalExit){
                    if(!completionPrefix())return fail(Status::numeric,0x175d70);
                    frameStage_=FrameStage::exit;break;
                }
                if(r.status!=GcStartFrameStatus::posePublished){std::fprintf(stderr,"intro_camera_start_frame status=%u mode44=%u flags62=%u\n",unsigned(r.status),unsigned(controller_.mode44),unsigned(controller_.flags62));return fail(Status::sourcePath,0x175d70);}
                if(r.activatedStartCamera){
                    //Temporary value projection is discarded. The actual AE8
                    //entry stores land in all eight real banked base tails.
                    for(std::size_t i=0;i<actual_.bank.count();++i)actual_.bank.visit(i,[&](){setOriginalCameraRacerIntroActive(actual_.baseTail);});
                }
                frameStage_=FrameStage::lateTick;break;
            }
            // 175D70 with race1B0 bit4 and race48+78 (17622C..17624C): common
            // prefix 175D90..17601C, then the TV director 176924..176D48 in any
            // mode, then 115CB0's selected child. The ending orbit branch below
            // refuses on bit4 (17601C), as in the original.
            // 17692C: a racer in state 3/4 (destroyed, e.g. a wreck recorded
            // before the finish) gets the death camera, not the TV director:
            // that is this session's existing destroyed-racer path below.
            if((raceFlags1B0_&0x10u)&&replayTv_.present()&&current.racerState40!=3&&current.racerState40!=4){
                {   OriginalCameraShakeChannels shake{follow_.shakeScale174,follow_.shakeAmplitude170,
                        attached_.shakeScale2C,attached_.shakeAmplitude28};
                    auto next=controller_;auto blur=actual_.blur;bool attempted=false;std::int32_t strength=0;
                    if(!original_race_camera_detail::advanceCommonPrefix175D70(next,shake,blur,frameDelta_,attempted,strength))
                        return fail(Status::numeric,0x175d90u);
                    if(current.racerState40>=0&&current.racerState40<=2){
                        const std::uint8_t bits=current.racerState40==1?8u:current.racerState40==2?16u:0u;
                        next.flags63Low5=static_cast<std::uint8_t>((next.flags63Low5&~0x18u)|bits);
                    }
                    controller_=next;actual_.blur=blur;
                    follow_.shakeScale174=shake.followMagnitude174;follow_.shakeAmplitude170=shake.followFactor170;
                    attached_.shakeScale2C=shake.attachedMagnitude2C;attached_.shakeAmplitude28=shake.attachedFactor28;}
                original_replay_tv50PD::Decision decision;
                if(!actual_.bank.visit(0,[&](){return replayTv_.direct176924(bodyLocal288(),frameDelta_,
                    actual_.arena.noiseOwner(),decision);}))return fail(Status::numeric,0x2d30d0u);
                if(decision.switched){
                    const auto oldMode=controller_.mode44;bool changed=false;
                    if(decision.tv){
                        if(!requestReplayTvMode177AB0())return fail(Status::sourcePath,0x176a40u);
                        changed=true;
                    }else{
                        lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
                        if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
                           !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return fail(Status::visibility);
                        auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return fail(Status::owner);
                        const auto zones=visibilityKeys_.view();OriginalFollowSceneTransitionInput transition;
                        const OriginalFollowIdentity identity{generation_,key(identity_[0].racerTag),key(identity_[0].mainBodyTag),
                            key(identity_[0].containerTag),static_cast<std::int32_t>(identity_[0].pod60)};
                        if(!actual_.bank.visit(0,[&](){return readOriginalFollowSceneTransitionInput(identity,key(scene_),
                            {actual_.arena.nodes,actual_.arena.sceneNodeCount},actual_.bindings.animation.bodyScene288,
                            actual_.collisionWorld,actual_.mainMembership690,zones,transition);}))return fail(Status::owner);
                        // 176A60: 177AB0(race48, 0, no preset, -1).
                        const auto t=transitionOriginalFollow177AB0(controller_,follow_,sceneBinding_,followChild_,
                            *world,spatial_,zones,*work,0,nullptr,transition);
                        if(t.status!=OriginalFollowControllerStatus::complete)return fail(Status::sourcePath,t.sourcePC);
                        pose_.position=sceneBinding_.position180;
                        changed=oldMode!=0;
                        // 176A74 2843C0 current id, then 17BD00(id,0,0) and +10C = 1.
                        const auto before=follow_.bank.records[follow_.current100].id00;
                        if(selectOriginalFollowPreset(follow_,decision.preset,false,false).status!=OriginalFollowStatus::ok)
                            return fail(Status::sourcePath,0x17bd00u);
                        follow_.snap10C=true;++follow_.revision;
                        changed=changed||before!=decision.preset;
                    }
                    // 176D48..176D78.
                    if(decision.blurIfChanged&&changed){
                        controller_.cutRemaining90=2.f;requestGameplayBlur(actual_.blur,100,0,0);
                    }
                }
                if(controller_.mode44==6){
                    // 115CB0 -> TV vtable+14 = 2D3400 (body-attached, 2D2090 values).
                    std::array<TVCameraSceneSample49F,8> samples{};std::size_t count=actual_.bank.count();
                    if(count>samples.size())return fail(Status::owner,0x2d38c0u);
                    rr::game::TVCameraUpdateInput49F input;
                    for(std::size_t i=0;i<count;++i){   // 2D38C0: racer manager order
                        samples[i].inactive40=static_cast<std::int32_t>(actual_.bank.read(i,reads_.state40));
                        actual_.bank.visit(i,[&](){const auto& v=actual_.arena.bodies.main.components.root.body.velocity.v;
                            samples[i].localPosition=bodyLocal288();samples[i].velocity={v[0],v[1],v[2],0.f};
                            if(i==0){input.resolvedPlayerPosition=bodyWorld288();input.bodyVelocity={v[0],v[1],v[2],0.f};}});
                    }
                    input.targetYOffset=original_replay_tv50PD::targetYOffset2D2090;
                    input.elapsed=frameDelta_;input.sceneSamples={samples.data(),count};
                    CameraViewOutput49F projected;
                    if(!replayTv_.update2D3400(input,actual_.arena.noiseOwner(),scene_,pose_,projected,
                        {displayInputs_->rootFlags228,sceneFlags29_}))return fail(Status::numeric,0x2d3400u);
                    scene_=projected.sceneAfter;matrices_=projected;
                    frameStage_=FrameStage::lateTick;break;
                }
                // Mode 0 (director Follow variant): the selected Follow pose only.
                frameStage_=FrameStage::exitPose;break;
            }
            // 17601C: with race1B0 bit4 (Results replay) the ending orbit is
            // never entered; control passes on (director or death camera).
            if(actual_.humanCount34&&!(raceFlags1B0_&0x10u)&&(current.raceStateFlags58&1u)&&current.racerCD4==0&&current.raceTime94>0.f){
                lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
                if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
                   !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return fail(Status::visibility);
                auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return fail(Status::owner);
                const auto zones=visibilityKeys_.view();OriginalFollowSceneTransitionInput transition;
                const OriginalFollowIdentity identity{generation_,key(identity_[0].racerTag),key(identity_[0].mainBodyTag),
                    key(identity_[0].containerTag),static_cast<std::int32_t>(identity_[0].pod60)};
                if(!actual_.bank.visit(0,[&](){return readOriginalFollowSceneTransitionInput(identity,key(scene_),
                    {actual_.arena.nodes,actual_.arena.sceneNodeCount},actual_.bindings.animation.bodyScene288,
                    actual_.collisionWorld,actual_.mainMembership690,zones,transition);}))return fail(Status::owner);
                const auto ending=advanceOriginalEndingRaceCamera175D70({controller_,follow_,attached_,sceneBinding_,
                    followChild_,attachedChild_,*world,spatial_,zones,*work,actual_.arena.noiseOwner(),actual_.blur,actual_.humanHeld},
                    current,transition);
                if(ending.status!=OriginalEndingCameraStatus50KG::needsSelectedPose115CB0)
                    return fail(Status::sourcePath,ending.sourcePC);
                if(ending.transitionCommitted)pose_.position=sceneBinding_.position180;
                frameStage_=FrameStage::exitPose;break;
            }
            // States3/4 enter the original death camera (177AB0, 22DC90), whose
            // every update aggregates the racer's real debris (22D4B0). Until
            // the wreck owner has bound this racer's debris view (the world
            // boundary after the destroy commit) the camera keeps its current
            // mode on the frozen, hidden body (docs/pod-destruction-20261108.md).
            if((current.racerState40==3||current.racerState40==4)&&(!actual_.deathDebris||!actual_.deathDebris->count370)){
                // No wreck provider yet: hold the current pose (published
                // through the ordinary late tick/matrix/visibility stages).
                frameStage_=FrameStage::lateTick;break;
            }
            if((current.racerState40==3||current.racerState40==4)&&actual_.deathDebris&&actual_.deathDebris->count370){
                result_.retirement=retirementCursor_.begin({controller_,follow_,attached_,death_,deathChild_,
                    followChild_,attachedChild_,actual_.blur,replayTv_.present(),
                    replayTv_.present()?replayTv_.childKey():0u},current);
                if(result_.retirement.status==OriginalRetirementCameraStatus50KG::needsDeathTransition177AB0){
                    lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
                    if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
                       !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return fail(Status::visibility);
                    auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return fail(Status::owner);
                    const auto zones=visibilityKeys_.view();
                    result_.retirement=actual_.bank.visit(0,[&](){
                        OriginalRetirementCameraResult50KG failed;
                        if(!actual_.followQueries.get()->sync())return failed;
                        const SceneGraph49P graph{actual_.arena.nodes,actual_.arena.sceneNodeCount};
                        const auto& velocity=actual_.arena.bodies.main.components.root.body.velocity.v;
                        const OriginalDeathCameraSubject50KG subject{death_.identity,&graph,current.racerState40,
                            actual_.arena.bodies.main.components.probe348.field118,
                            {velocity[0],velocity[1],velocity[2],1.f}};
                        const OriginalFollowIdentity identity{generation_,key(identity_[0].racerTag),
                            key(identity_[0].mainBodyTag),key(identity_[0].containerTag),static_cast<std::int32_t>(identity_[0].pod60)};
                        OriginalFollowSceneTransitionInput transition;
                        if(!readOriginalFollowSceneTransitionInput(identity,key(scene_),graph,
                            actual_.bindings.animation.bodyScene288,actual_.collisionWorld,actual_.mainMembership690,zones,transition)){
                            actual_.followQueries.get()->commit();return failed;
                        }
                        auto result=transitionOriginalDeath177AB0(controller_,death_,sceneBinding_,deathChild_,
                            followChild_,attachedChild_,actual_.followQueries.get()->deathOwner(),subject,
                            actual_.collisionWorld,actual_.mainMembership690,*world,spatial_,zones,*work,transition,
                            &actual_.followQueries.get()->deathColdOwner(),replayTv_.present()?replayTv_.childKey():0u);
                        if(!actual_.followQueries.get()->commit())result.status=OriginalRetirementCameraStatus50KG::owner;
                        return result;
                    });
                    if(result_.retirement.status!=OriginalRetirementCameraStatus50KG::complete)
                        return fail(Status::sourcePath,result_.retirement.sourcePC);
                    pose_.position=sceneBinding_.position180;
                    result_.retirement=retirementCursor_.afterDeathTransition();
                }
                if(result_.retirement.status==OriginalRetirementCameraStatus50KG::needsHudAnimationStop1AD230){
                    frameStage_=FrameStage::retirementHud;break;
                }
                if(result_.retirement.status!=OriginalRetirementCameraStatus50KG::needsSelectedPose115CB0)
                    return fail(Status::sourcePath,result_.retirement.sourcePC);
                frameStage_=FrameStage::retirementPose;break;
            }
            if(controller_.mode44==0){
                lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
                if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
                   !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return fail(Status::visibility);
                auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return fail(Status::owner);
                auto zones=visibilityKeys_.view();
                const auto r=actual_.bank.visit(0,[&](){
                    OriginalFollowRaceFrameResult failed;
                    if(!actual_.followQueries.get()->sync())return failed;
                    const auto& velocity=actual_.arena.bodies.main.components.root.body.velocity.v;
                    const OriginalFollowIdentity identity{generation_,key(identity_[0].racerTag),
                        key(identity_[0].mainBodyTag),key(identity_[0].containerTag),static_cast<std::int32_t>(identity_[0].pod60)};
                    const SceneGraph49P graph{actual_.arena.nodes,actual_.arena.sceneNodeCount};
                    OriginalFollowSubject subject;
                    const auto subjectResult=readOriginalFollowSubject(identity,graph,
                        actual_.bindings.visual.container194,static_cast<std::int32_t>(actual_.boost.vehicle64),
                        {velocity[0],velocity[1],velocity[2],0.f}, //W is unconsumed by the source velocity paths.
                        actual_.arena.bodies.main.components.root.worldAngularVelocityA0.v[1],subject);
                    if(subjectResult.status!=OriginalFollowStatus::ok)return failed;
                    const OriginalFollowCollisionSources sources{&graph,actual_.bindings.lifecycle.main190,&actual_.poseHistory};
                    auto result=advanceOriginalStableFollowRaceFrame(controller_,follow_,attached_,followChild_,objectChildren_,
                        current,subject,graph,sources,actual_.followQueries.get()->pairOwner(),actual_.arena.noiseOwner(),
                        actual_.blur,scene_,pose_,matrices_,tick_,*world,spatial_,zones,sceneBinding_,*work,
                        displayInputs_->rootFlags228,sceneFlags29_);
                    // Publish every completed world-query prefix to its actual
                    // banked filter/copy aliases, even when a later pose fails.
                    if(!actual_.followQueries.get()->commit())result.status=OriginalFollowRaceFrameStatus::owner;
                    return result;
                });
                result_.stableFollow=r;result_.stableFollowAttempted=true;
                result_.follow=r.pose;
                {   // Diagnostic: per-frame follow camera (the lap-2 crash site).
                    static std::uint64_t frames=0,blocked=0,subnormalSquares=0;static float minBlockedSquare=-1;
                    ++frames;
                    if(r.collision.cameraBlocked){++blocked;
                        if(r.collision.speedSquaredSubnormal)++subnormalSquares;
                        if(minBlockedSquare<0||r.collision.speedSquared<minBlockedSquare)minBlockedSquare=r.collision.speedSquared;}
                    if(frames%312==0)std::fprintf(stderr,"follow_collision_stats frames=%llu camera_blocked=%llu"
                        " blocked_speed_sq_subnormal=%llu min_blocked_speed_sq=%g\n",
                        static_cast<unsigned long long>(frames),static_cast<unsigned long long>(blocked),
                        static_cast<unsigned long long>(subnormalSquares),static_cast<double>(minBlockedSquare));
                    if(r.status!=OriginalFollowRaceFrameStatus::complete)
                        std::fprintf(stderr,"follow_camera_failure path=race_frame race_status=%u pose_status=%u pc=0x%x"
                            " collision_status=%u collision_pc=0x%x bad_op=%u bad_role=%u bad_bits=0x%08x bad_ordinal=%u"
                            " operations=%u rounding=%d mxcsr=0x%x follow_query=%s\n",
                            static_cast<unsigned>(r.status),static_cast<unsigned>(r.pose.status),r.pose.sourcePC,
                            static_cast<unsigned>(r.collision.status),r.collision.sourcePC,r.collision.firstBadOp,
                            r.collision.firstBadRole,r.collision.firstBadBits,r.collision.firstBadOrdinal,
                            r.collision.operations,std::fegetround(),static_cast<unsigned>(_mm_getcsr()),
                            actual_.followQueries.get()&&actual_.followQueries.get()->diagnostic().message?
                                actual_.followQueries.get()->diagnostic().message:"");
                }
                if(r.status!=OriginalFollowRaceFrameStatus::complete)return fail(Status::sourcePath,r.pose.sourcePC);
                frameStage_=FrameStage::stableMatrix;break;
            }
            if(controller_.mode44!=5)return fail(Status::sourcePath,0x177ab0);
            lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
            if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
               !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return fail(Status::visibility);
            auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return fail(Status::owner);
            const auto body=attachedFrame();auto zones=visibilityKeys_.view();
            const auto r=actual_.bank.visit(0,[&](){
                return advanceOriginalStableAttachedRaceFrame(controller_,follow_,attached_,attachedChild_,objectChildren_,
                    current,body,{actual_.arena.nodes,actual_.arena.sceneNodeCount},actual_.attachedGlobals,actual_.arena.noiseOwner(),
                    actual_.blur,scene_,pose_,matrices_,tick_,*world,spatial_,zones,sceneBinding_,*work,
                    displayInputs_->rootFlags228,sceneFlags29_);
            });
            result_.stableAttached=r;result_.stableAttachedAttempted=true;
            result_.attached=r.pose;
            if(r.status!=OriginalAttachedRaceFrameStatus::complete)return fail(Status::sourcePath,r.pose.sourcePC);
            frameStage_=FrameStage::stableMatrix;break;}
        case FrameStage::taSkipVideo:
            if(!taSkipBlockingFade_.complete())return set(Status::videoPending,0x260850);
            if(!taFinishSkipFade())return fail(Status::sourcePath,taFailurePC_);
            {RaceStartRacerBank bank;GcStartRacerEffectBank unused;
            if(!readRacers(bank,unused)||advanceOriginalRaceStartCameraView(start_,bank,frameDelta_,scene_,pose_,matrices_,
                displayInputs_->rootFlags228,sceneFlags29_)!=RaceStartStatus::ok)return fail(Status::numeric,0x2d6f10);}
            frameStage_=FrameStage::lateTick;break;
        case FrameStage::retirementHud:
            if(!actual_.raceHud||!actual_.raceHud->loaded()||!actual_.raceHud->stopAll1AD230())
                return fail(Status::owner,0x1ad230u);
            result_.retirement=retirementCursor_.afterHudAnimationStop(uiFlags1E4_,uiTransition1E0_);
            if(result_.retirement.status!=OriginalRetirementCameraStatus50KG::needsSelectedPose115CB0)
                return fail(Status::sourcePath,result_.retirement.sourcePC);
            frameStage_=FrameStage::retirementPose;break;
        case FrameStage::retirementPose:{
            if(!actual_.deathDebris)return fail(Status::owner,0x22d4b0u);
            if((displayInputs_->rootFlags228&4u)||(sceneFlags29_&4u))return fail(Status::sourcePath,0x115cb0u);
            result_.death=actual_.bank.visit(0,[&](){
                OriginalDeathCameraResult50KG failed;failed.status=OriginalDeathCameraStatus50KG::owner;
                if(!actual_.followQueries.get()->sync())return failed;
                const SceneGraph49P graph{actual_.arena.nodes,actual_.arena.sceneNodeCount};
                const auto& velocity=actual_.arena.bodies.main.components.root.body.velocity.v;
                const OriginalDeathCameraSubject50KG subject{death_.identity,&graph,
                    static_cast<std::int32_t>(actual_.arena.bodies.main.vehicleState40),
                    actual_.arena.bodies.main.components.probe348.field118,
                    {velocity[0],velocity[1],velocity[2],1.f}};
                auto result=advanceOriginalDeathCamera22DC90(death_,subject,*actual_.deathDebris,
                    frameDelta_,deathGlobals_,actual_.followQueries.get()->deathOwner(),pose_);
                if(!actual_.followQueries.get()->commit())result.status=OriginalDeathCameraStatus50KG::owner;
                return result;
            });
            if(result_.death.status!=OriginalDeathCameraStatus50KG::complete)return fail(Status::numeric,result_.death.sourcePC);
            CameraViewOutput49F projected;
            if(!composeCameraView49F(scene_,pose_,projected))return fail(Status::numeric,0x115350u);
            scene_=projected.sceneAfter;matrices_=projected;
            frameStage_=FrameStage::lateTick;break;}
        case FrameStage::exit:{
            const auto r=advanceExit();if(r.status!=Status::complete)return r;break;}
        case FrameStage::exitPose:{
            //Same175D70 handoff frame reaches115CB0 after177AB0. The common
            //timer prefix already ran; execute only the selected pose child.
            if(controller_.mode44==0){
                result_.follow=actual_.bank.visit(0,[&](){
                    OriginalFollowResult result{OriginalFollowStatus::owner,0x17a870,0};
                    if(!actual_.followQueries.get()->sync())return result;
                    const auto& velocity=actual_.arena.bodies.main.components.root.body.velocity.v;
                    const OriginalFollowIdentity identity{generation_,key(identity_[0].racerTag),
                        key(identity_[0].mainBodyTag),key(identity_[0].containerTag),static_cast<std::int32_t>(identity_[0].pod60)};
                    const SceneGraph49P graph{actual_.arena.nodes,actual_.arena.sceneNodeCount};
                    OriginalFollowSubject subject;
                    result=readOriginalFollowSubject(identity,graph,actual_.bindings.visual.container194,
                        static_cast<std::int32_t>(actual_.boost.vehicle64),{velocity[0],velocity[1],velocity[2],0.f},
                        actual_.arena.bodies.main.components.root.worldAngularVelocityA0.v[1],subject);
                    if(result.status!=OriginalFollowStatus::ok)return result;
                    const OriginalFollowCollisionSources sources{&graph,actual_.bindings.lifecycle.main190,&actual_.poseHistory};
                    OriginalFollowCollisionResult collision;
                    result=advanceOriginalFollowCameraView(follow_,subject,frameDelta_,raceFlags1B0_,sources,
                        actual_.followQueries.get()->pairOwner(),actual_.arena.noiseOwner(),pose_,scene_,matrices_,collision,
                        displayInputs_->rootFlags228,sceneFlags29_);
                    {   // Diagnostic summary every 312 follow frames (about 5 s).
                        static std::uint64_t frames=0,blocked=0,subnormalSquares=0;static float minBlockedSquare=-1;
                        ++frames;
                        if(collision.cameraBlocked){++blocked;
                            if(collision.speedSquaredSubnormal)++subnormalSquares;
                            if(minBlockedSquare<0||collision.speedSquared<minBlockedSquare)minBlockedSquare=collision.speedSquared;}
                        if(frames%312==0)std::fprintf(stderr,"follow_collision_stats frames=%llu camera_blocked=%llu"
                            " blocked_speed_sq_subnormal=%llu min_blocked_speed_sq=%g\n",
                            static_cast<unsigned long long>(frames),static_cast<unsigned long long>(blocked),
                            static_cast<unsigned long long>(subnormalSquares),static_cast<double>(minBlockedSquare));
                    }
                    if(result.status!=OriginalFollowStatus::ok)
                        std::fprintf(stderr,"follow_camera_failure status=%u pc=0x%x collision_status=%u collision_pc=0x%x"
                            " bad_op=%u bad_role=%u bad_bits=0x%08x bad_ordinal=%u operations=%u rounding=%d mxcsr=0x%x\n",
                            static_cast<unsigned>(result.status),result.sourcePC,static_cast<unsigned>(collision.status),
                            collision.sourcePC,collision.firstBadOp,collision.firstBadRole,collision.firstBadBits,
                            collision.firstBadOrdinal,collision.operations,std::fegetround(),static_cast<unsigned>(_mm_getcsr()));
                    if(!actual_.followQueries.get()->commit())return OriginalFollowResult{OriginalFollowStatus::owner,0x17a870,0};
                    return result;
                });
                if(result_.follow.status!=OriginalFollowStatus::ok)return fail(Status::numeric,result_.follow.sourcePC);
            }else if(controller_.mode44==5){
                const auto body=attachedFrame();
                result_.attached=actual_.bank.visit(0,[&](){return advanceOriginalAttachedCameraView(attached_,body,
                    {actual_.arena.nodes,actual_.arena.sceneNodeCount},actual_.attachedGlobals,actual_.arena.noiseOwner(),
                    pose_,scene_,matrices_,displayInputs_->rootFlags228,sceneFlags29_);});
                if(result_.attached.status!=OriginalAttachedStatus::ok)return fail(Status::numeric,result_.attached.sourcePC);
            }else return fail(Status::sourcePath,0x115cb0);
            frameStage_=FrameStage::lateTick;break;}
        case FrameStage::lateTick:
            if(advanceOriginalCameraSceneTick114AE0(tick_,generation_,key(scene_),pose_.position,scene_.look,sceneFlags29_)!=
                OriginalCameraSceneTickStatus::needsMatrixVisibilityAndChildren)return fail(Status::numeric,0x114ae0);
            frameStage_=FrameStage::lateMatrix;break;
        case FrameStage::lateMatrix:
            result_.start=rebuildOriginalRaceScene236DD0(viewport_,scene_,pose_.position,matrices_);
            if(result_.start.status!=OriginalCameraStartStatus::complete)return fail(Status::numeric,result_.start.sourcePC);
            frameStage_=FrameStage::lateVisibility;break;
        case FrameStage::lateVisibility:
            if(!updateSpatial())return fail(Status::visibility,0x114c44);
            if(!dispatchTaEntity141ED0())return fail(Status::sourcePath,taFailurePC_);
            frameStage_=FrameStage::published;completedFrame_=frameNumber_;++publicationCount_;return set(Status::framePublished,0x115dd0);
        case FrameStage::stableMatrix:
            //Stable composition already did the second actual tick/visibility.
            //Complete the derived236DD0 screen/guard caches for this same pose.
            result_.start=rebuildOriginalRaceScene236DD0(viewport_,scene_,pose_.position,matrices_);
            if(result_.start.status!=OriginalCameraStartStatus::complete)return fail(Status::numeric,result_.start.sourcePC);
            if(!dispatchTaEntity141ED0())return fail(Status::sourcePath,taFailurePC_);
            frameStage_=FrameStage::published;completedFrame_=frameNumber_;++publicationCount_;return set(Status::framePublished,0x115dd0);
        default:return set(Status::order);
        }}
    }

    // Native camera-only device press. Root dispatches real237DF0 transitions
    // through12815C/application routing before the next fixed simulation step.
    // This does not route driving input or sample a Player device for ALL8.
    EventCallbackResult48Z cameraInput(const ManagerHeldSources49A& held,
        InputEvent48Z& event,DeviceButtonRecord48Y& record,bool pressed) noexcept {
        invalidateFogPreparation();
        if(!actual_.followQueries.get()||!coldBound_||abandoned_||pendingDisplayWork()||
           (frameStage_!=FrameStage::idle&&frameStage_!=FrameStage::published))return {false,0};
        std::array<std::uint32_t,8> viewState{};
        for(std::size_t i=0;i<actual_.bank.count();++i)viewState[i]=actual_.bank.read(i,reads_.retirementCD4);
        OriginalCameraModeInputEnvironment environment{held,actual_.preferences,viewState.data(),actual_.bank.count(),
            actual_.bank.read(0,reads_.class58),0,raceEntityFlags29_,actual_.all8Manager.flags58,raceFlags1B0_};
        lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
        if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
           !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return {false,0};
        auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return {false,0};
        auto zones=visibilityKeys_.view();OriginalFollowSceneTransitionInput transition;
        const OriginalFollowIdentity identity{generation_,key(identity_[0].racerTag),key(identity_[0].mainBodyTag),
            key(identity_[0].containerTag),static_cast<std::int32_t>(identity_[0].pod60)};
        if(!actual_.bank.visit(0,[&](){return readOriginalFollowSceneTransitionInput(identity,key(scene_),
            {actual_.arena.nodes,actual_.arena.sceneNodeCount},actual_.bindings.animation.bodyScene288,
            actual_.collisionWorld,actual_.mainMembership690,zones,transition);}))return {false,0};
        const auto oldMode=controller_.mode44;
        OriginalCameraModeInputOwners owners{controller_,follow_,attached_,sceneBinding_,followChild_,
            attachedChild_,*world,spatial_,zones,*work};
        lastInput_=pressed?advanceOriginalCameraCyclePress176DF0(owners,environment,transition):
            advanceOriginalCameraLookRelease1774A0(owners,environment,transition);
        if(oldMode!=controller_.mode44)pose_.position=sceneBinding_.position180;
        if(lastInput_.status==OriginalCameraModeInputStatus::needsCycleAudio1A4B90){
            pose_.position=sceneBinding_.position180; //Actual177AB0 scene180 write.
            const auto sound=actual_.cycleAudio.dispatchEvent4();
            using AudioResult=original_camera_cycle_audio50KX::Owner::Result;
            if(sound!=AudioResult::started&&sound!=AudioResult::unavailable&&sound!=AudioResult::muted)return {false,0};
            if(!saveOriginalCameraModePreference178670(controller_,follow_,actual_.preferences,0))return {false,0};
        }else if(lastInput_.status!=OriginalCameraModeInputStatus::needsChildRoute142350)return {false,0};
        // Generic Entity18 is distinct from camera collection38. Positive TA
        // traverses the real emitter; zero-cut paths retain empty Entity lists.
        if(objectChildren_.controllerChild18)return routeTaEntityInput(event,record,pressed);
        const EventChildRouter48Z children{0x600u,noEventChild48Z,nullptr,0};
        const auto routed=routeChildInputEvent48Z(children,pressed,event,record,nullptr,nullptr,0);
        return {routed.status==EventRouteStatus48Z::Complete,routed.handled?1u:0u};
    }
    EventCallbackResult48Z press(const ManagerHeldSources49A& held,
        InputEvent48Z& event,DeviceButtonRecord48Y& record) noexcept {
        return cameraInput(held,event,record,true);
    }
    EventCallbackResult48Z release(const ManagerHeldSources49A& held,
        InputEvent48Z& event,DeviceButtonRecord48Y& record) noexcept {
        //With neither temporary flag set,1774A0's held queries cannot write
        //camera state. Preserve the existing effect-free release route even
        //during intro/display work, which must not acquire a scene lease.
        if(!(controller_.flags62&0x60u)){
            if(objectChildren_.controllerChild18)return routeTaEntityInput(event,record,false);
            const EventChildRouter48Z children{0x600u,noEventChild48Z,nullptr,0};
            const auto routed=routeChildInputEvent48Z(children,false,event,record,nullptr,nullptr,0);
            return {routed.status==EventRouteStatus48Z::Complete,routed.handled?1u:0u};
        }
        return cameraInput(held,event,record,false);
    }
    const OriginalCameraModeInputResult& lastInput()const noexcept{return lastInput_;}
    bool taSkipVideoPending()const noexcept{return frameStage_==FrameStage::taSkipVideo&&!taSkipBlockingFade_.complete();}
    bool taSkipVideoResumePending()const noexcept{return frameStage_==FrameStage::taSkipVideo;}


    //Original countdown clock. Root calls once per unpaused planned native
    //tick (Application+11C paused ticks never reach it). This is the counting
    //branch of the lap manager step 1AF9C0 (1AFA70..1AFAB4): countdown34 -= dt
    //while flags58 bit2 and not bit1; on the first step that finds
    //countdown34 <= 0 it runs 1AFE30 (GO). The racing branch (clock +30,
    //1AFF50, 1B0CB0, 1B0A70) stays with the existing lap timing owner. The
    //start overlay update 198190 follows, as race+D4 follows race+B4.
    //The host race rows only mirror this clock; they no longer own one.
    Result advanceNativeStartGate(float actualPreparedSeconds){
        if(!actual_.followQueries.get()||!coldBound_||frameStage_==FrameStage::failed)return set(Status::owner);
        if(pendingDisplayWork())return set(Status::videoPending);
        if(!countdownInstalled_)return set(Status::startHeld);
        auto& host=actual_.hostRace.rows();
        bool held=true;
        if(!goPublished_){
            const auto step=advanceOriginalRaceCountdown1AFA84(countdown_,actual_.all8Manager,actualPreparedSeconds);
            if(step==OriginalCountdownStep::arithmetic)return fail(Status::numeric);
            if(step==OriginalCountdownStep::inactive)return fail(Status::sourcePath,0x1afa70);
            if(step==OriginalCountdownStep::needs1AFE30){
                //1AFE30. Its 1AEDD0 per-racer starts (body axis enables
                //167A30/167A40 on racer+340, D40 +80/+8C) and the 287B60
                //record start belong to owners outside this Session; root
                //runs them on takeGoEdge1AFE30() in this same step, before
                //any racer update. The flags58 copy into every racer below is
                //the port's existing per-racer view of the manager word.
                //2C4FA0(announcer, 7.0) is empty in retail: no call.
                actual_.all8Manager.flags58=static_cast<std::uint8_t>(actual_.all8Manager.flags58|2u); //1AFF38.
                for(std::size_t i=0;i<actual_.bank.count();++i)actual_.bank.visit(i,[&](){actual_.arena.fields.applicationByte58=actual_.all8Manager.flags58;});
                goPublished_=true;held=false;goEdgePending_=true;
                //Host rows follow the original GO; they keep no clock of their own.
                host.countdownRemaining=0;host.phase=race_session50HM::Phase::Racing;host.error=race_session50HM::Error::None;
                if(!goLogged_){std::fprintf(stderr,"race_start_clock go1AFE30 flags58=%u\n",unsigned(actual_.all8Manager.flags58));goLogged_=true;}
            }else{
                //Mirror only: race-audio validity and logs read these rows.
                host.countdownRemaining=countdown_.countdown34>0?static_cast<double>(countdown_.countdown34):0.0;
            }
        }else held=false;
        if(startOverlay_){
            OriginalRaceStartOverlayInputs50HU in{countdown_.countdown34,actual_.all8Manager.flags58,
                raceFlags1B0_,raceEntityFlags29_,0};
            if(!startOverlay_->update198190(in,actualPreparedSeconds))return fail(Status::numeric);
        }
        return set(held?Status::startHeld:Status::simulationAllowed);
    }
    //Presentation cut only. Root must call after the real video frame drain
    //and registered callbacks, before accepting another simulation tick. The
    //backend performs actual retained-image samples/retirement. Null blocks.
    Result advanceBlockingVideo(OriginalVideoBlockingBackend* backend){
        if(abandoned_)return set(Status::abandoned);
        if(!actual_.followQueries.get()||!coldBound_||!displayInputs_)return set(Status::owner);
        if(videoRequestPending50LA())return set(Status::sourcePath,0x25aa4c);
        auto& v=displayInputs_->video;
        if(!(v.flags404Low6&3u))return set(Status::complete);
        if(v.flags404Low6&4u)return set(Status::sourcePath,0x25aa38);
        result_.start=frameStage_==FrameStage::taSkipVideo?
            taSkipBlockingFade_.advance(v,actual_.applicationClock,backend):
            blockingFade_.advance(v,actual_.applicationClock,backend);
        return set(result_.start.status==OriginalCameraStartStatus::complete?Status::complete:Status::videoPending,
            result_.start.sourcePC);
    }
    //Next reached race-prefix1A7BD0..1A7C18 and real manager-state8 child.
    //Call only after fade-out retirement and publication of the actual first
    //restored camera base image. Root owns that real renderer publication, not this
    //numeric camera counter. The backend will capture/validate the image.
    Result advanceRaceFadeInPrefix(){
        if(f8Intro_.ownerThread_!=std::this_thread::get_id())return {Status::owner,0x1a7bd0};
        if(abandoned_)return set(Status::abandoned);
        if(!actual_.followQueries.get()||!coldBound_||!displayInputs_||(!exit_.complete()&&!directStart_.complete()&&!taSkipFadeFinished_)||!hasView())return set(Status::owner);
        if(blockingRequestPending50LA())return set(Status::sourcePath,0x2aed60);
        if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::ra)
            return advanceCanonicalRaceFadeInPrefix();
        auto& app=*displayInputs_;auto& video=app.video;
        if(endingFadeRequest_.pending()||(retainedFadeInRequestPending50LA()&&app.fadeManagerA8!=8))
            return set(Status::owner,0x2ae93c);
        // A partial async request has direction bits but may not yet have
        // bit2. Only its same retained A8=8 invocation bypasses this gate.
        if((video.flags404Low6&3u)&&!(video.flags404Low6&4u)&&
           !(app.fadeManagerA8==8&&retainedFadeInRequestPending50LA()))return set(Status::videoPending);
        if(raceFlags1B0_&8u){
            //Original2AEE90 returns0 if manager busy or video not covered.
            //Only an actual accepted scheduling write clears the race flag.
            if(app.fadeManagerA8||!(video.covered3DC||(video.flags404Low6&16u)))
                return set(Status::videoPending,0x1a7bf8);
            result_.start=requestOriginalFirstMainFade1B7D50(video,app.fadeManagerA8,app.fadeDuration98);
            if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::videoPending,result_.start.sourcePC);
            raceFlags1B0_=static_cast<std::uint8_t>(raceFlags1B0_&~8u);
        }
        if(app.fadeManagerA8==8){
            //2AE93C..2AE958: actual duration98,+38=260610,a2=1;clear A8.
            result_.start=serviceAsyncFadeInRequest260610();
            if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::videoPending,result_.start.sourcePC);
        }else if(app.fadeManagerA8)return set(Status::sourcePath,0x2ae7fc);
        return set(Status::complete,0x2ae958);
    }
    // Genuine human root1A7DA0 precedes the racer child/physics schedule.
    // The same clock94/flags1B0 then reach this Session's ending camera.
    OriginalHumanEndClockResult50KG advanceHumanEndRootWithOwners(float legacyDelta,
        race_music50IE::Owner*,source_announcer50IE::Owner*)noexcept;
    OriginalHumanEndClockResult50KG advanceHumanEndRoot(float delta,bool musicActive,
        bool musicFading,bool announcerActive) noexcept {
        if(f8Intro_.ownerThread_!=std::this_thread::get_id())
            return {OriginalHumanEndClockStatus50KG::callerBoundary,0x1a7da0u,0};
        if(!actual_.humanCount34||!actual_.followQueries.get()||!coldBound_||abandoned_||pendingDisplayWork()||
           (frameStage_!=FrameStage::idle&&frameStage_!=FrameStage::published))
            return {OriginalHumanEndClockStatus50KG::callerBoundary,0x1a7da0u,0};
        if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::ra)
            return advanceCanonicalHumanEndRoot(); //Actual Root/audio, never these supplied observations.
        return endingClockCursor_.begin(endingClock_,actual_.race94,raceFlags1B0_,actual_.all8Manager.flags58,
            delta,musicActive,musicFading,announcerActive,true);
    }
    OriginalHumanEndClockResult50KG resumeHumanEndMusic(bool musicActive,bool announcerActive) noexcept {
        if(f8Intro_.ownerThread_!=std::this_thread::get_id()||
           (nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::ra))
            return {OriginalHumanEndClockStatus50KG::callerBoundary,0x1a7e84u,0};
        return endingClockCursor_.afterMusicFade(musicActive,announcerActive,true);
    }
    Result requestHumanEndFade(std::int32_t milliseconds) noexcept {
        if(f8Intro_.ownerThread_!=std::this_thread::get_id())return {Status::owner,0x1a7f24};
        if(nativeTaIssuance_&&nativeTaIssuance_->course==OriginalCameraStartCourse::ra)
            return set(Status::sourcePath,0x1a7f24); //Canonical request is consumed by its exact owner.
        if(!actual_.humanCount34||!displayInputs_||!endingClockCursor_.pending())return set(Status::owner);
        auto& app=*displayInputs_;
        result_.start=requestOriginalHumanEndFade2AED60(app.video,app.fadeManagerA8,app.fadeDuration98,milliseconds);
        if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::sourcePath,result_.start.sourcePC);
        const auto accepted=endingClockCursor_.afterVideoFadeRequest();
        return set(accepted.status==OriginalHumanEndClockStatus50KG::advanced?Status::complete:Status::sourcePath,accepted.sourcePC);
    }
    // Common-race restart behind the Results (1AC8E0 sets race1B0 bit3, the
    // next 1A7BD0 fades the restarted scene in again). The original repeats
    // its fade-in; rearm the one-shot fade-in request/async owners once every
    // earlier fade (intro fade-in, ending fade-out) has fully finished.
    bool rearmFadeInAfterRestart50PD()noexcept {
        if(f8Intro_.ownerThread_!=std::this_thread::get_id()||abandoned_||!displayInputs_)return false;
        if(fadeInRequestConsumer_!=FadeInRequestConsumer::none||fadeInRequest_.pending()||
           taSkipFadeInRequest_.pending()||endingFadeRequest_.pending()||
           (asyncFade_.started()&&!asyncFade_.complete())||(taSkipAsyncFade_.started()&&!taSkipAsyncFade_.complete())||
           (endingFadeRequested_&&!endingFade_.complete())||displayInputs_->fadeManagerA8==8)return false;
        fadeInRequest_.~OriginalVideoAsyncRequest50LA();::new(static_cast<void*>(&fadeInRequest_))OriginalVideoAsyncRequest50LA();
        asyncFade_.~AsyncRetainedFade();::new(static_cast<void*>(&asyncFade_))AsyncRetainedFade();
        fadeInRequested_=false;return true;
    }
    bool endingFadeQueued()const noexcept {
        return actual_.humanCount34&&displayInputs_&&displayInputs_->fadeManagerA8==7;
    }
    bool endingVideoPending()const noexcept {
        return !abandoned_&&endingFadeRequested_&&!endingFade_.complete();
    }
    bool endingVideoCovered()const noexcept {
        return endingFadeRequested_&&endingFade_.complete()&&displayInputs_&&
            (displayInputs_->video.covered3DC||(displayInputs_->video.flags404Low6&16u));
    }
    // Service real2AE7D0 after the last completed gameplay image. Source7
    // starts asynchronous OUT; source4 clears only the actual covered result.
    Result advanceHumanEndFadeManager() noexcept {
        if(!actual_.humanCount34||!actual_.followQueries.get()||!coldBound_||!displayInputs_)return set(Status::owner);
        auto& app=*displayInputs_;
        if(blockingRequestPending50LA()||retainedFadeInRequestPending50LA()||(endingFadeRequest_.pending()&&app.fadeManagerA8!=7))
            return set(Status::owner,0x2ae938);
        if(app.fadeManagerA8==7){
            result_.start=endingFadeRequest_.invoke(OriginalVideoFadeDirection::out260850,app.video,actual_.applicationClock,
                app.currentApplicationMode22C(),app.fadeDuration98);
            if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::sourcePath,result_.start.sourcePC);
            app.fadeManagerA8=4;endingFadeRequested_=true;
        }else if(app.fadeManagerA8==4){
            if(!(app.video.covered3DC||(app.video.flags404Low6&16u)))return set(Status::videoPending,0x2ae89cu);
            app.fadeManagerA8=0;
        }else if(app.fadeManagerA8)return set(Status::sourcePath,0x2ae7fcu);
        return set(Status::complete,0x2ae7fcu);
    }
    //Native held-underlay presentation policy. Call repeatedly from the real
    //presentation service, not once per simulation tick. No sleep, fake fence,
    //manufactured overlay or guessed source-clock milliseconds exist here.
    Result advanceAsyncVideo(OriginalVideoBlockingBackend* backend){
        if(abandoned_)return set(Status::abandoned);
        if(!actual_.followQueries.get()||!coldBound_||!displayInputs_)return set(Status::owner);
        if(videoRequestPending50LA())return set(Status::sourcePath,0x25e610);
        if(endingVideoPending())result_.start=endingFade_.advance(displayInputs_->video,actual_.applicationClock,
            publicationCount_,backend,OriginalVideoFadeDirection::out260850);
        else if(taSkipFadeInRequested_&&!taSkipAsyncFade_.complete())result_.start=taSkipAsyncFade_.advance(
            displayInputs_->video,actual_.applicationClock,publicationCount_,backend);
        else if(fadeInRequested_)result_.start=asyncFade_.advance(displayInputs_->video,actual_.applicationClock,
            publicationCount_,backend);
        else return set(Status::owner);
        return set(result_.start.status==OriginalCameraStartStatus::complete?Status::complete:Status::videoPending,
            result_.start.sourcePC);
    }
    //Release before the Session's backing/keys can be destroyed while the
    //visibility owner remains usable. A failed removal must not be hidden by
    //a destructor success; retain this Session until the real owner is closed.
    //If the whole application world is destroyed together, its owner must
    //still outlive this Session and must never traverse these records again.
    Result close(){
        if(constructorOnly_)return closeConstructorOnlyNative(false);
        if(!actual_.followQueries.get())return set(Status::owner);
        invalidateFogPreparation();
        if(closed_)return set(Status::complete);
        if(pendingDisplayWork())return set(Status::videoPending);
        if(frameStage_!=FrameStage::idle&&frameStage_!=FrameStage::published&&
           frameStage_!=FrameStage::failed)return set(Status::order);
        if(!withdrawNativeRaLoadingOperands()||!revokeCountedRaceCameraNative34())return set(Status::owner,0x1a93b0);
        // Once source ownership starts retiring, a partial failure can only
        // resume cleanup. It must not re-enter normal simulation/constructor work.
        coldBound_=false;frameStage_=FrameStage::failed;
        invalidateSelectedGameplayCamera50LA();
        // Unpublished native custody closes independently of fresh issuer
        // admission; root borrows may already be terminal during abandonment.
        if(nativeTaCutChildPhase_!=NativeTaCutChildPhase::empty&&
           !abortUnpublishedTaCutChildNative())return set(Status::owner,0x2828f0u);

        if(!releaseRacerEntityPrefixes())return set(Status::renderer,result_.entityCleanup.sourcePC);
        if(!actual_.bank.visit(0,[&](){return actual_.followQueries.get()->close();}))return set(Status::owner,0x120290);
        if(spatial_.registration34.attached){
            lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
            if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic))return set(Status::visibility);
            auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return set(Status::owner);
            result_.spatial=removeOriginalCameraScene1148D0(*world,spatial_,*work);
            if(result_.spatial.status!=LiveZoneStatus50EN::Complete)return set(Status::visibility,0x1148d0);
        }
        if(!retireTaController())return set(Status::owner,0x174c20u);
        coldBound_=false;closed_=true;return set(Status::complete,0x1148d0);
    }
    //Quit/error cleanup is separate from successful numeric completion. Stop
    //the camera permanently, retire the actual cold Entity/material prefixes
    //through the still-live renderer, and remove its CPU visibility registration.
    //The root independently runs real fade cancelAndRetire before this call.
    //A cleanup failure retains this owner for retry; video flags/alpha, race
    //flags, countdown, RNG and racer history remain exactly as abandoned.
    // Original 1A4558..1A4588 (race music manager update 1A4440): each view's
    // AuralScape listener is set through 1030B0 from that view's CAMERA scene
    // (+180 position, +190 look, +1A0 up), not from the player's body. Read-only.
    bool nativeAudioListenerPose(float (&position)[3],float (&look)[3])const noexcept {
        if(closed_||abandoned_||!hasView())return false;
        const float p[3]{pose_.position.x,pose_.position.y,pose_.position.z};
        const float l[3]{scene_.look.x,scene_.look.y,scene_.look.z};
        for(int i=0;i<3;++i)if(!std::isfinite(p[i])||!std::isfinite(l[i]))return false;
        for(int i=0;i<3;++i){position[i]=p[i];look[i]=l[i];}
        return true;
    }
    Result abandonAndCloseSpatial(){
        if(constructorOnly_)return closeConstructorOnlyNative(true);
        if(!actual_.followQueries.get())return set(Status::owner);
        invalidateFogPreparation();
        if(!withdrawNativeRaLoadingOperands()||!revokeCountedRaceCameraNative34())return set(Status::owner,0x1a93b0);
        abandoned_=true;coldBound_=false;frameStage_=FrameStage::failed;
        invalidateSelectedGameplayCamera50LA();
        if(closed_)return set(Status::abandoned);
        // Unpublished native custody closes independently of fresh issuer
        // admission; root borrows may already be terminal during abandonment.
        if(nativeTaCutChildPhase_!=NativeTaCutChildPhase::empty&&
           !abortUnpublishedTaCutChildNative())return set(Status::owner,0x2828f0u);

        if(!releaseRacerEntityPrefixes())return set(Status::renderer,result_.entityCleanup.sourcePC);
        if(!actual_.bank.visit(0,[&](){return actual_.followQueries.get()->close();}))return set(Status::owner,0x120290);
        if(spatial_.registration34.attached){
            lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
            if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic))return set(Status::visibility);
            auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return set(Status::owner);
            result_.spatial=removeOriginalCameraScene1148D0(*world,spatial_,*work);
            if(result_.spatial.status!=LiveZoneStatus50EN::Complete)return set(Status::visibility,0x1148d0);
        }
        if(!retireTaController())return set(Status::owner,0x174c20u);
        coldBound_=false;closed_=true;return set(Status::abandoned,0x1148d0);
    }
};
} // namespace original_camera_session50KX
