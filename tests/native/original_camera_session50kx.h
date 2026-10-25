#pragma once
#include "../../src/assets/original_ta_cut_authoring_candidate.h"
#include "../../src/game/original_ta_cut_resource_candidate.h"
#include <optional>
#include "../../src/game/original_racer_diffuse_camera50la.h"
#include "track_navigation_view50tc.h"
#include "track_navigation_view50tb.h"
#include "track_navigation_view50gb.h"
#include "track_navigation_view50mc.h"
namespace original_subtype3_record { class Owner; }
namespace original_cable_selection50LA { class Binding; }
namespace gb_particle_render50GB {class Owner;}
namespace mc_particle_render50MC {class Owner;}
namespace rr::game { class OriginalDamageObservation50LA; }
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
#include "../../src/game/original_racer_animation_cold50la.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include <array>
#include <cstring>
#include <limits>
#include <string>

namespace ra_source_camera50LA { class OwnershipBridge; }
namespace original_time_stats_adapter50LA { class Owner; }
namespace original_speed_boost_adapter50LA { class Owner; }
namespace original_position_stats_adapter50LA { class Owner; }
namespace original_pause_observation50LA { class Owner; }
namespace original_main_entry50KG { struct Application; }
namespace original_camera_session50KX {
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
    original_follow_query_bridge::QueryBridge& followQueries;
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
    const std::array<native_racer50HW::Cables*,8>* actualCables=nullptr; // RA factory pins every concrete owner.
    const TrackNavigation50EU* actualRadarMapB8=nullptr; // Borrow of the same retained authored map; RA factory only.
    TrackNavigationView50EU actualRadarMapMb{};
    TrackNavigationView50TC actualRadarMapTc{};
    TrackNavigationView50TB actualRadarMapTb{};
    TrackNavigationView50GB actualRadarMapGb{};
    TrackNavigationView50MC actualRadarMapMc{};
    void* startMusicContext=nullptr;
    bool (*startMusicRequest)(void*,std::int32_t,std::int32_t)=nullptr;
    const original_subtype3_record::Owner* recordManager40=nullptr;
};
enum class Cut : std::uint8_t {
    // Native hold: use real current final-grid/last-published body state while
    // no racer/physics step runs. World animation/UI/audio remain root-owned.
    heldBeforeStart,
    // Source-ordered numeric camera placement after every actual racer prefix
    // and before the first1A8390/219810 physics suffix. Never claim prefixes ran
    // from a callback return: all eight actual49W phase/factor fields are read.
    afterAllRacerPrefixes
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
    friend struct ::original_main_entry50KG::Application;
    friend class ::gb_particle_render50GB::Owner;
    friend class ::mc_particle_render50MC::Owner;
    friend class ::original_cable_selection50LA::Binding;
    // Private same-RA factory verifies actual retirement, never a caller flag.
    friend class ra_source_camera50LA::OwnershipBridge;
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
        cameraDispatch,cameraUpdate,retirementHud,retirementPose,exit,exitPose,lateTick,lateMatrix,lateVisibility,
        stableMatrix,published,failed
    } frameStage_=FrameStage::idle;
    ActualOwners actual_;
    // Stable per-racer Entity extensions live outside the multiplexed bank.
    // The bank still owns every existing handling/body/rig field. These cold
    // constructors do not certify its missing full20F390/210770 history.
    OriginalAIDamageMaterialRow50LA aiDamageMaterial_;
    std::int32_t raceMode8C_=0;
    OriginalAIDamageRaceContext50LA aiDamageRace_;
    std::array<OriginalRacerEntityCold50LA,8> racerEntities_{};
    DisplayInputs* displayInputs_=nullptr;
    std::uint64_t generation_=0,frameNumber_=0,completedFrame_=0,publicationCount_=0;
    std::array<Identity,8> identity_{};
    unsigned coldPhase_=0,authoredPhase_=0;
    bool coldBound_=false,readsBound_=false,countdownInstalled_=false,goPublished_=false,closed_=false,abandoned_=false;
    Cut cut_=Cut::heldBeforeStart;
    float frameDelta_=0;
    // Additional represented race/UI fields owned by this native race session.
    // One real registered camera establishes low2=1. This is RACE1B0, not
    // APPLICATION1B0's unrelated modal/event gate. UI1E4 is a native HUD gate:
    // hidden during intro, revealed by the actual199770 numeric write.
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
        std::array<Member,4> members;
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
    OriginalRaceCameraOwner controller_;
    // ONE noncopyable Entity counterpart for this existing copyable numeric
    // payload. No second controller, borrowed parent, or additional authority.
    struct ColdController174A30 {
        OriginalWreckComponent50LA entity;
        std::uint64_t issuerGeneration=0;
        bool started=false,released=false;
        std::uint32_t vtable08=0;
        const void* scene3C=nullptr;
        const void* racer40=nullptr;
        std::array<const void*,4> queries48{}; //48,4C,50,54.
        std::uint32_t word58=0; //Constructor zero; no pointer/ownership meaning inferred.
        const void* script5C=nullptr;
        std::array<const void*,8> children64{}; //64..80, including intro emitter80.
        std::uint32_t mode84=0; //174DF0 later receives the actual race8C integer.
        // Constructor-unwritten88 is deliberately absent. Native safe backing
        // is not source knowledge: only these masks become established.
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
    std::array<std::uint64_t,4> childOrder_{};
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
    TVCameraUpdateOutput49F pose_{};
    CameraViewOutput49F matrices_;
    OriginalRaceCameraDisplayFields display_;
    OriginalRaceViewportCall coldViewport_;
    OriginalGcGaIntroExitOwner exit_;
    OriginalDirectRaceStartOwner directStart_;
    bool directStartPending_=false;
    OriginalRaceStartCountdown countdown_;
    OriginalVideoBlockingFade blockingFade_;
    AsyncRetainedFade asyncFade_,endingFade_;
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
    bool fadeInRequested_=false,endingFadeRequested_=false;
    Result result_;
    OriginalCameraModeInputResult lastInput_{};

    template<class T> static std::uint64_t key(const T& owner) noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&owner));
    }
    static bool same(float a,float b) noexcept{return std::memcmp(&a,&b,sizeof a)==0;}
    #include "original_camera_controller_cold_candidate.inl"
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
        raceMode8C_=0;aiDamageRace_={generation_,key(*this),&raceMode8C_};
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
                    if(!model->finishedFor(entity))
                        model->finish1538B0(entity,*actual_.actualMaterialDirectory36B970);
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
            const auto expected=cut==Cut::heldBeforeStart?Phase::Ready:Phase::AfterBodyChildren;
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
        if(controller_.mode44!=7||!start_.complete240||controller_.shakeRemaining94>=0||
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
            0,static_cast<std::int32_t>(actual_.bank.read(0,reads_.state40)),
            &actual_.arena.noiseOwner(),actual_.startMusicContext,actual_.startMusicRequest};
        //Source80 null is proved by the retained effective controllerA8=0;
        //it is not a muted-audio success override. Native existing suffix audio
        //continues in its real caller, outside this camera owner.
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
public:
    explicit Session(ActualOwners actual) noexcept:actual_(actual){}
    ~Session(){
        if(coldController_.entity.reference00)std::terminate();
        if(nativeTaCutChildPhase_==NativeTaCutChildPhase::constructed&&
           !abortUnpublishedTaCutChildNative())std::terminate();
    }
    Session(const Session&)=delete;Session& operator=(const Session&)=delete;
    Session(Session&&)=delete;Session& operator=(Session&&)=delete;
    const Result& diagnostic()const noexcept{return result_;}
    bool ready()const noexcept{return coldBound_;}
    bool abandoned()const noexcept{return abandoned_;}
    bool countdownInstalled()const noexcept{return countdownInstalled_;}
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
        if(!coldBound_||closed_||abandoned_||pendingDisplayWork()||!serial||
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
        const auto& zero=actual_.followQueries.sharedZero433DB0();
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
            if(!actual_.followQueries.sync())return set(Status::owner,0x179a80u);
            OriginalCameraPairResult result;
            const auto status=resetOriginalCameraQueryPair179A80(actual_.followQueries.pairOwner(),
                cursor.collisionZone,cursor.anchor,follow_.queryRadius178,cursor.queries,result);
            // Publish actual successful registry prefixes even after rejection.
            const bool committed=actual_.followQueries.commit();
            if(!committed)return set(Status::owner,0x179a80u);
            if(status!=OriginalCameraPairStatus::Complete)return set(Status::sourcePath,result.sourcePC);
            cursor.stage=Stage::finalMotion;break;}
        case Stage::complete:return set(Status::complete,0x225b74u);
        default:return set(Status::order,0x225b28u);
        }
    }
    std::int32_t cameraMode()const noexcept{return controller_.mode44;}
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
        if(phase.bound()||!coldBound_||closed_||abandoned_||!generation_||!displayInputs_||
           !displayInputs_->trackFog3EC||display_.actualTrackFog3EC!=displayInputs_->trackFog3EC)return false;
        phase.owner_=this;phase.fog_=displayInputs_->trackFog3EC;phase.generation_=generation_;return true;
    }
    // Source fog13EA30/13EA60 changes the SAME active camera range before
    // world/particle draw. Native scheduling invokes this after the published
    // camera pose, before either world49F or ParticleCode projection. It does
    // not dispatch a child, tick visibility, sample RNG or publish a new pose.
    Result prepareGameplayFogRender() noexcept {
        invalidateFogPreparation();
        if(!coldBound_||closed_||abandoned_||!hasView()||!displayInputs_||
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
    bool nativeHudVisible()const noexcept{return !(uiFlags1E4_&1u);}
    bool hasView()const noexcept{return publicationCount_!=0;}
    // Exact camera scene38 parent, projected to the original zero-based zone48.
    // This is never a racer collision zone, AI map node or visible-zone guess.
    bool readCameraPrimaryZone48(std::uint32_t& out) noexcept {
        if(!coldBound_||closed_||abandoned_||!hasView()||!spatial_.registration34.attached||
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
    std::uint64_t publicationCount()const noexcept{return publicationCount_;}
    bool blockingVideoPending()const noexcept {
        return !abandoned_&&displayInputs_&&(displayInputs_->video.flags404Low6&3u)&&
            !(displayInputs_->video.flags404Low6&4u);
    }
    bool asyncVideoPending()const noexcept {
        //Numeric25B5D0 may clear direction bits before the final staged GPU
        //sample/restore retires. Retained owner state, not flags alone, wins.
        return !abandoned_&&((fadeInRequested_&&!asyncFade_.complete())||
            (endingFadeRequested_&&!endingFade_.complete()));
    }
    bool pendingDisplayWork()const noexcept {
        return displayInputs_&&((displayInputs_->video.flags404Low6&3u)||
            (raceFlags1B0_&8u)||displayInputs_->fadeManagerA8==8||
            (asyncFade_.started()&&!asyncFade_.complete())||
            (endingFadeRequested_&&!endingFade_.complete()));
    }
    std::uint64_t publishedFrame()const noexcept{return completedFrame_;}
    const RaceStartPreparationReceipt& introPreparation()const noexcept{return preparation_;}
    const OriginalRaceCameraOwner& controller()const noexcept{return controller_;}
    //Only an actual authored trigger enter may call this. No event is created
    //from speed/contact or elapsed camera time. Same original camera0 owner.
    bool applyAuthoredShakeEnter(bool entering,const OriginalCameraShakeAction& action) noexcept {
        invalidateFogPreparation();
        if(!coldBound_||(frameStage_!=FrameStage::idle&&frameStage_!=FrameStage::published&&
            frameStage_!=FrameStage::cameraReady))return false;
        return applyOriginalCameraShakeEnter(entering,action,&controller_,1);
    }

    //Cold after actual ALL8 roster/final-grid/7F0 setup and existing neutral
    //setup tick, before native countdown/GO. The live50Y history is NOT reset.
    //authored, app and every borrowed owner/backing must outlive this Session.
    //generation is the native race lifetime key; it does not assert a guest ID.
    Result prepare(std::uint64_t generation,const AuthoredInputs& authored,DisplayInputs& app){
        invalidateFogPreparation();
        // Unactivated constructor branch cannot enter the reduced native
        // initialization order, even after typed retirement. No fresh receipt.
        if(coldController_.started)return set(Status::sourcePath,0x174df0u);
        // Unpublished constructor staging is a different private lifetime,
        // not evidence for the existing reduced native prepare chronology.
        if(nativeTaCutChildPhase_!=NativeTaCutChildPhase::empty)return set(Status::sourcePath,0x282860u);
        if(nativeTaIssuance_&&generation!=nativeTaIssuance_->generation)return set(Status::owner);
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
        // At this native cold caller, genuinely construct the represented
        // racer27E130/7F8/240 prefix, then concrete7FC/model lifetime below.
        // No physics/collection or outer manager attachment is inferred.
        if(!constructRacerEntityPrefixes())return fail(Status::sourcePath,result_.entityCold.sourcePC);
        if(!constructRacerModelPrefixes())return fail(Status::sourcePath,result_.entityCold.sourcePC);
        for(;;){
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
            case 1:
                if(!coldViewport_.complete()){
                    result_.start=coldViewport_.advance(true,display_,sceneBorrow(),app.renderer,app.diagnostic);
                    if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::renderer,result_.start.sourcePC);
                }
                result_.start=selectOriginalRaceScene26F940(display_,viewport_,app.rendererWidth4,app.rendererHeight8);
                if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::sourcePath,result_.start.sourcePC);
                ++coldPhase_;break;
            case 2:{
                if(authoredPhase_==0){
                    if(!startActions_.prepare(authored.course,authored.tob,authored.tobBytes))return set(Status::authoring,0x1ad330);
                    authoredPhase_=1;
                }else if(startActions_.course()!=authored.course)return set(Status::authoring,0x1ad330);
                if(!startPrelude_.prepared()&&!startPrelude_.prepare174DF0(startActions_,
                    actual_.launchKind11258,actual_.configuration34,actual_.application350))
                    return set(Status::authoring,0x174e48);
                // A source-positive count reaches the real74 cut collection.
                // Never substitute the procedural7C path for those children.
                if(startPrelude_.controllerCutCountA8()!=0)return set(Status::sourcePath,0x174fa4);
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
                const auto& i=identity_[0];
                const OriginalFollowIdentity f{generation_,key(i.racerTag),key(i.mainBodyTag),key(i.containerTag),
                    static_cast<std::int32_t>(i.pod60)};
                result_.follow=prepareOriginalFollowCamera(follow_,f,presetBank_);
                if(result_.follow.status!=OriginalFollowStatus::ok)return set(Status::numeric,result_.follow.sourcePC);
                if(!actual_.bank.visit(0,[&](){return actual_.followQueries.prepare(follow_.queryRadius178);}))
                    return fail(Status::owner,0x179a80);
                ++coldPhase_;break;}
            case 4:{
                const auto& i=identity_[0];
                const OriginalAttachedBinding b{generation_,key(i.racerTag),key(i.mainBodyTag),key(i.modelTag),
                    actual_.bank.read(0,reads_.body288),actual_.bank.read(0,reads_.model1B4),static_cast<std::int32_t>(i.pod60)};
                result_.attached=prepareOriginalAttachedCamera(attached_,b,attachedOffset_);
                if(result_.attached.status!=OriginalAttachedStatus::ok)return set(Status::numeric,result_.attached.sourcePC);
                const OriginalDeathCameraIdentity50KG deathIdentity{generation_,key(i.racerTag),key(i.mainBodyTag),
                    key(death_),actual_.bank.read(0,reads_.body288)};
                result_.death=prepareOriginalDeathCamera50KG(death_,deathIdentity,deathGlobals_);
                if(result_.death.status!=OriginalDeathCameraStatus50KG::complete)return set(Status::numeric,result_.death.sourcePC);
                if(!actual_.followQueries.bindDeathChild(key(death_)))return set(Status::owner,0x1752d0u);
                ++coldPhase_;break;}
            case 5:{
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
                if(!bindFreshOriginalRaceCamera(controller_,b))return set(Status::owner);
                //Explicit reduced NATIVE registry: actual fully owned children
                //in source relative order. Source unused70/78 are omitted;
                //count/ordinals are not retail parity. No dummy success owners.
                childOrder_={{key(follow_),key(death_),key(attached_),key(start_)}};
                if(!bindFreshOriginalCameraChildRegistry(collection_,generation_,key(controller_),key(scene_),
                    childOrder_.data(),childOrder_.size()))return set(Status::owner);
                introChild_={&start_,key(start_),&collection_};attachedChild_={&attached_,key(attached_),&collection_};
                followChild_={&follow_,key(follow_),&collection_};
                deathChild_={&death_,key(death_),&collection_};
                objectChildren_={generation_,key(controller_),key(scene_),0,0};
                controllerEntity_.controllerKey=key(controller_);display_.controllers48[0]=&controllerEntity_;
                ++coldPhase_;break;}
            case 7:
                result_.visibilityJoin=attachFreshSelectedOriginalCamera0Visibility50SC(actual_.visibility,generation_,key(scene_),
                    spatial_,visibilityBacking_,visibilityKeys_,sceneBinding_);
                if(result_.visibilityJoin.status!=OriginalCameraVisibilityJoinStatus::complete){
                    if(result_.visibilityJoin.sceneAttached)return fail(Status::visibility,0x175224);
                    return set(Status::visibility,0x175224);
                }
                pose_.position=sceneBinding_.position180;
                ++coldPhase_;break;
            default:coldBound_=true;return set(Status::complete,0x1752c0);
            }
        }
    }

    //First actual scene34 child, before the real manager timing/rank child and
    //camera48. It executes114AE0 +236DD0 +114C44 using the prior camera pose.
    //A successful return is only this child, never the complete application.
    Result advanceSceneChild(std::uint64_t frame,Cut cut,float actualRootPreparedDelta){
        invalidateFogPreparation();
        if(!coldBound_||!displayInputs_)return set(Status::owner);
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
        if(!coldBound_||!displayInputs_)return set(Status::owner);
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
            StableRaceCameraFrame current;if(!stableFrame(current))return fail(Status::sourcePath,0x175d70);
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
                if(r.status!=GcStartFrameStatus::posePublished)return fail(Status::sourcePath,0x175d70);
                if(r.activatedStartCamera){
                    //Temporary value projection is discarded. The actual AE8
                    //entry stores land in all eight real banked base tails.
                    for(std::size_t i=0;i<actual_.bank.count();++i)actual_.bank.visit(i,[&](){setOriginalCameraRacerIntroActive(actual_.baseTail);});
                }
                frameStage_=FrameStage::lateTick;break;
            }
            if(actual_.humanCount34&&(current.raceStateFlags58&1u)&&current.racerCD4==0&&current.raceTime94>0.f){
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
            if(current.racerState40==3||current.racerState40==4){
                result_.retirement=retirementCursor_.begin({controller_,follow_,attached_,death_,deathChild_,
                    followChild_,attachedChild_,actual_.blur},current);
                if(result_.retirement.status==OriginalRetirementCameraStatus50KG::needsDeathTransition177AB0){
                    lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
                    if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic)||
                       !visibilityKeys_.matches(actual_.visibility,lease,generation_,key(scene_)))return fail(Status::visibility);
                    auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return fail(Status::owner);
                    const auto zones=visibilityKeys_.view();
                    result_.retirement=actual_.bank.visit(0,[&](){
                        OriginalRetirementCameraResult50KG failed;
                        if(!actual_.followQueries.sync())return failed;
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
                            actual_.followQueries.commit();return failed;
                        }
                        auto result=transitionOriginalDeath177AB0(controller_,death_,sceneBinding_,deathChild_,
                            followChild_,attachedChild_,actual_.followQueries.deathOwner(),subject,
                            actual_.collisionWorld,actual_.mainMembership690,*world,spatial_,zones,*work,transition,
                            &actual_.followQueries.deathColdOwner());
                        if(!actual_.followQueries.commit())result.status=OriginalRetirementCameraStatus50KG::owner;
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
                    if(!actual_.followQueries.sync())return failed;
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
                        current,subject,graph,sources,actual_.followQueries.pairOwner(),actual_.arena.noiseOwner(),
                        actual_.blur,scene_,pose_,matrices_,tick_,*world,spatial_,zones,sceneBinding_,*work,
                        displayInputs_->rootFlags228,sceneFlags29_);
                    // Publish every completed world-query prefix to its actual
                    // banked filter/copy aliases, even when a later pose fails.
                    if(!actual_.followQueries.commit())result.status=OriginalFollowRaceFrameStatus::owner;
                    return result;
                });
                result_.stableFollow=r;result_.stableFollowAttempted=true;
                result_.follow=r.pose;
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
                if(!actual_.followQueries.sync())return failed;
                const SceneGraph49P graph{actual_.arena.nodes,actual_.arena.sceneNodeCount};
                const auto& velocity=actual_.arena.bodies.main.components.root.body.velocity.v;
                const OriginalDeathCameraSubject50KG subject{death_.identity,&graph,
                    static_cast<std::int32_t>(actual_.arena.bodies.main.vehicleState40),
                    actual_.arena.bodies.main.components.probe348.field118,
                    {velocity[0],velocity[1],velocity[2],1.f}};
                auto result=advanceOriginalDeathCamera22DC90(death_,subject,*actual_.deathDebris,
                    frameDelta_,deathGlobals_,actual_.followQueries.deathOwner(),pose_);
                if(!actual_.followQueries.commit())result.status=OriginalDeathCameraStatus50KG::owner;
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
                    if(!actual_.followQueries.sync())return result;
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
                        actual_.followQueries.pairOwner(),actual_.arena.noiseOwner(),pose_,scene_,matrices_,collision,
                        displayInputs_->rootFlags228,sceneFlags29_);
                    if(!actual_.followQueries.commit())return OriginalFollowResult{OriginalFollowStatus::owner,0x17a870,0};
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
            frameStage_=FrameStage::published;completedFrame_=frameNumber_;++publicationCount_;return set(Status::framePublished,0x115dd0);
        case FrameStage::stableMatrix:
            //Stable composition already did the second actual tick/visibility.
            //Complete the derived236DD0 screen/guard caches for this same pose.
            result_.start=rebuildOriginalRaceScene236DD0(viewport_,scene_,pose_.position,matrices_);
            if(result_.start.status!=OriginalCameraStartStatus::complete)return fail(Status::numeric,result_.start.sourcePC);
            frameStage_=FrameStage::published;completedFrame_=frameNumber_;++publicationCount_;return set(Status::framePublished,0x115dd0);
        default:return set(Status::order);
        }}
    }

    // Native camera-only device press. Root dispatches real237DF0 transitions
    // through12815C/application routing before the next fixed simulation step.
    // This does not route driving input or sample a Player device for ALL8.
    EventCallbackResult48Z press(const ManagerHeldSources49A& held,
        InputEvent48Z& event,DeviceButtonRecord48Y& record) noexcept {
        invalidateFogPreparation();
        if(!coldBound_||abandoned_||pendingDisplayWork()||
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
        lastInput_=advanceOriginalCameraCyclePress176DF0({controller_,follow_,attached_,sceneBinding_,followChild_,
            attachedChild_,*world,spatial_,zones,*work},environment,transition);
        if(lastInput_.status==OriginalCameraModeInputStatus::needsCycleAudio1A4B90){
            pose_.position=sceneBinding_.position180; //Actual177AB0 scene180 write.
            const auto sound=actual_.cycleAudio.dispatchEvent4();
            using AudioResult=original_camera_cycle_audio50KX::Owner::Result;
            if(sound!=AudioResult::started&&sound!=AudioResult::unavailable&&sound!=AudioResult::muted)return {false,0};
            if(!saveOriginalCameraModePreference178670(controller_,follow_,actual_.preferences,0))return {false,0};
        }else if(lastInput_.status!=OriginalCameraModeInputStatus::needsChildRoute142350)return {false,0};
        //Actual reduced native object graph has no controller18 descendants;
        //these are NOT the camera collection38 whose selected child owns pose.
        if(objectChildren_.controllerChild18)return {false,0};
        const EventChildRouter48Z children{0x600u,noEventChild48Z,nullptr,0};
        const auto routed=routeChildInputEvent48Z(children,true,event,record,nullptr,nullptr,0);
        return {routed.status==EventRouteStatus48Z::Complete,routed.handled?1u:0u};
    }
    EventCallbackResult48Z release(InputEvent48Z& event,DeviceButtonRecord48Y& record) noexcept {
        if(objectChildren_.controllerChild18)return {false,0};
        const EventChildRouter48Z children{0x600u,noEventChild48Z,nullptr,0};
        const auto routed=routeChildInputEvent48Z(children,false,event,record,nullptr,nullptr,0);
        return {routed.status==EventRouteStatus48Z::Complete,routed.handled?1u:0u};
    }
    const OriginalCameraModeInputResult& lastInput()const noexcept{return lastInput_;}

    //ONE existing host countdown. Root calls once per unpaused planned native
    //tick instead of its former advanceCountdown call; never call both. Intro
    //does not consume it. Its crossing-zero hold and subsequent GO edge retain
    //the existing native schedule; no source1AFE30 event/sound parity is claimed.
    Result advanceNativeStartGate(float actualPreparedSeconds){
        if(!coldBound_||frameStage_==FrameStage::failed)return set(Status::owner);
        if(pendingDisplayWork())return set(Status::videoPending);
        if(!countdownInstalled_)return set(Status::startHeld);
        if(!actual_.hostRace.advanceCountdown(static_cast<double>(actualPreparedSeconds))){
            if(actual_.hostRace.rows().error!=race_session50HM::Error::None)return fail(Status::numeric);
            return set(Status::startHeld);
        }
        if(!goPublished_){
            actual_.all8Manager.flags58=static_cast<std::uint8_t>(actual_.all8Manager.flags58|2u);
            for(std::size_t i=0;i<actual_.bank.count();++i)actual_.bank.visit(i,[&](){actual_.arena.fields.applicationByte58=actual_.all8Manager.flags58;});
            goPublished_=true;
        }
        return set(Status::simulationAllowed);
    }
    //Presentation cut only. Root must call after the real video frame drain
    //and registered callbacks, before accepting another simulation tick. The
    //backend performs actual retained-image samples/retirement. Null blocks.
    Result advanceBlockingVideo(OriginalVideoBlockingBackend* backend){
        if(abandoned_)return set(Status::abandoned);
        if(!coldBound_||!displayInputs_)return set(Status::owner);
        auto& v=displayInputs_->video;
        if(!(v.flags404Low6&3u))return set(Status::complete);
        if(v.flags404Low6&4u)return set(Status::sourcePath,0x25aa38);
        result_.start=blockingFade_.advance(v,actual_.applicationClock,backend);
        return set(result_.start.status==OriginalCameraStartStatus::complete?Status::complete:Status::videoPending,
            result_.start.sourcePC);
    }
    //Next reached race-prefix1A7BD0..1A7C18 and real manager-state8 child.
    //Call only after fade-out retirement and publication of the actual first
    //restored camera base image. Root owns that real renderer publication, not this
    //numeric camera counter. The backend will capture/validate the image.
    Result advanceRaceFadeInPrefix(){
        if(abandoned_)return set(Status::abandoned);
        if(!coldBound_||!displayInputs_||(!exit_.complete()&&!directStart_.complete())||!hasView())return set(Status::owner);
        auto& app=*displayInputs_;auto& video=app.video;
        if((video.flags404Low6&3u)&&!(video.flags404Low6&4u))return set(Status::videoPending);
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
            result_.start=requestOriginalVideoFade260610(video,actual_.applicationClock,
                app.currentApplicationMode22C(),app.fadeDuration98,true);
            if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::videoPending,result_.start.sourcePC);
            app.fadeManagerA8=0;fadeInRequested_=true;
        }else if(app.fadeManagerA8)return set(Status::sourcePath,0x2ae7fc);
        return set(Status::complete,0x2ae958);
    }
    // Genuine human root1A7DA0 precedes the racer child/physics schedule.
    // The same clock94/flags1B0 then reach this Session's ending camera.
    OriginalHumanEndClockResult50KG advanceHumanEndRoot(float delta,bool musicActive,
        bool musicFading,bool announcerActive) noexcept {
        if(!actual_.humanCount34||!coldBound_||abandoned_||pendingDisplayWork()||
           (frameStage_!=FrameStage::idle&&frameStage_!=FrameStage::published))
            return {OriginalHumanEndClockStatus50KG::callerBoundary,0x1a7da0u,0};
        return endingClockCursor_.begin(endingClock_,actual_.race94,raceFlags1B0_,actual_.all8Manager.flags58,
            delta,musicActive,musicFading,announcerActive,true);
    }
    OriginalHumanEndClockResult50KG resumeHumanEndMusic(bool musicActive,bool announcerActive) noexcept {
        return endingClockCursor_.afterMusicFade(musicActive,announcerActive,true);
    }
    Result requestHumanEndFade(std::int32_t milliseconds) noexcept {
        if(!actual_.humanCount34||!displayInputs_||!endingClockCursor_.pending())return set(Status::owner);
        auto& app=*displayInputs_;
        result_.start=requestOriginalHumanEndFade2AED60(app.video,app.fadeManagerA8,app.fadeDuration98,milliseconds);
        if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::sourcePath,result_.start.sourcePC);
        const auto accepted=endingClockCursor_.afterVideoFadeRequest();
        return set(accepted.status==OriginalHumanEndClockStatus50KG::advanced?Status::complete:Status::sourcePath,accepted.sourcePC);
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
        if(!actual_.humanCount34||!coldBound_||!displayInputs_)return set(Status::owner);
        auto& app=*displayInputs_;
        if(app.fadeManagerA8==7){
            result_.start=requestOriginalVideoFade260320(app.video,actual_.applicationClock,
                app.currentApplicationMode22C(),app.fadeDuration98,true,false);
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
        if(!coldBound_||!displayInputs_)return set(Status::owner);
        if(endingVideoPending())result_.start=endingFade_.advance(displayInputs_->video,actual_.applicationClock,
            publicationCount_,backend,OriginalVideoFadeDirection::out260850);
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
        invalidateFogPreparation();
        if(closed_)return set(Status::complete);
        if(pendingDisplayWork())return set(Status::videoPending);
        if(frameStage_!=FrameStage::idle&&frameStage_!=FrameStage::published&&
           frameStage_!=FrameStage::failed)return set(Status::order);
        // Once source ownership starts retiring, a partial failure can only
        // resume cleanup. It must not re-enter normal simulation/constructor work.
        coldBound_=false;frameStage_=FrameStage::failed;
        // Unpublished native custody closes independently of fresh issuer
        // admission; root borrows may already be terminal during abandonment.
        if(nativeTaCutChildPhase_!=NativeTaCutChildPhase::empty&&
           !abortUnpublishedTaCutChildNative())return set(Status::owner,0x2828f0u);
        if(!releaseColdController174C20())return set(Status::owner,0x174c20u);
        if(!releaseRacerEntityPrefixes())return set(Status::renderer,result_.entityCleanup.sourcePC);
        if(!actual_.bank.visit(0,[&](){return actual_.followQueries.close();}))return set(Status::owner,0x120290);
        if(spatial_.registration34.attached){
            lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
            if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic))return set(Status::visibility);
            auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return set(Status::owner);
            result_.spatial=removeOriginalCameraScene1148D0(*world,spatial_,*work);
            if(result_.spatial.status!=LiveZoneStatus50EN::Complete)return set(Status::visibility,0x1148d0);
        }
        coldBound_=false;closed_=true;return set(Status::complete,0x1148d0);
    }
    //Quit/error cleanup is separate from successful numeric completion. Stop
    //the camera permanently, retire the actual cold Entity/material prefixes
    //through the still-live renderer, and remove its CPU visibility registration.
    //The root independently runs real fade cancelAndRetire before this call.
    //A cleanup failure retains this owner for retry; video flags/alpha, race
    //flags, countdown, RNG and racer history remain exactly as abandoned.
    Result abandonAndCloseSpatial(){
        invalidateFogPreparation();
        abandoned_=true;coldBound_=false;frameStage_=FrameStage::failed;
        if(closed_)return set(Status::abandoned);
        // Unpublished native custody closes independently of fresh issuer
        // admission; root borrows may already be terminal during abandonment.
        if(nativeTaCutChildPhase_!=NativeTaCutChildPhase::empty&&
           !abortUnpublishedTaCutChildNative())return set(Status::owner,0x2828f0u);
        if(!releaseColdController174C20())return set(Status::owner,0x174c20u);
        if(!releaseRacerEntityPrefixes())return set(Status::renderer,result_.entityCleanup.sourcePC);
        if(!actual_.bank.visit(0,[&](){return actual_.followQueries.close();}))return set(Status::owner,0x120290);
        if(spatial_.registration34.attached){
            lighting::CameraVisibilityLease lease;lighting::Diagnostic diagnostic;
            if(!actual_.visibility.beginCameraVisibility(0,lease,diagnostic))return set(Status::visibility);
            auto* world=lease.world();auto* work=lease.workspace();if(!world||!work)return set(Status::owner);
            result_.spatial=removeOriginalCameraScene1148D0(*world,spatial_,*work);
            if(result_.spatial.status!=LiveZoneStatus50EN::Complete)return set(Status::visibility,0x1148d0);
        }
        coldBound_=false;closed_=true;return set(Status::abandoned,0x1148d0);
    }
};
} // namespace original_camera_session50KX
