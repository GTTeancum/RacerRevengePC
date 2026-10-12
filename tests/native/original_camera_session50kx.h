#pragma once
// Isolated concrete ALL8 native session adapter. Intended install: tests/native.
// All camera/scene/selector state stays OUTSIDE the sealed50HN racer bank.
// This is an explicit native root policy, not recovered retail root scheduling.
#include "native_racer_bank50hn_fixture.h"
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
#include "../../src/game/original_camera_visibility_join.h"
#include "../../src/game/original_attached_race_frame.h"
#include <array>
#include <cstring>
#include <limits>

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
};
inline bool initializeFreshNativeCameraVideoAlpha0(OriginalVideoFadeState& video) noexcept {
    // Only a fresh native display; never reset an existing fade/history.
    if(video.alpha3E8||video.covered3DC||video.flags404Low6||
       video.toColor3E0!=0x80000000u||video.fromColor3E4||video.targetAlpha3EC!=128||
       video.millisecondsPerStep3F0||video.redSlope3F4||video.greenSlope3F8||
       video.blueSlope3FC||video.lastMilliseconds400)return false;
    video.alpha3E8=0;return true;
}
// All references are the existing50DU objects; there is no one-human manager,
// callback that claims a root completed, banked RNG copy, or replacement body.
struct ActualOwners {
    Bank& bank;
    VehicleFrameState49W& arena;
    VehicleFrameBindings49W& bindings;
    BaseTailState50X& baseTail;
    AiRuntimeState50IV& ai;
    LapTimingFields50ID& racerTiming;
    LapTimingManager50ID& all8Manager;
    race_session50HM::Session& hostRace;
    SharedRandomOwner50ID& sharedRandom;
    OriginalAttachedGlobals& attachedGlobals;
    HostMonotonicClock50ID& applicationClock;
    ControlPresets49D& preferences;
    const std::uint32_t& launchKind11258;
    lighting::Owner& visibility;
    const ZoneLifecycle50CL& collisionWorld;
    LiveZoneMembership50EN& mainMembership690;
    GameplayBlurPending& blur;
    original_follow_query_bridge::QueryBridge& followQueries;
    BasePoseHistoryState50Y& poseHistory;
    const BoostState50FK& boost;
    original_camera_cycle_audio50KX::Owner& cycleAudio;
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
    OriginalCameraVisibilityJoinResult visibilityJoin{};
    LiveZoneResult50EN spatial{};
};

class Session final {
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
        Read<bool> aiInitialized;
        Read<Phase> phase;
        Read<SceneNodeIndex49P> body288,model1B4,container194;
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
        cameraDispatch,cameraUpdate,exit,exitPose,lateTick,lateMatrix,lateVisibility,
        stableMatrix,published,failed
    } frameStage_=FrameStage::idle;
    ActualOwners actual_;
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
    OriginalRaceCameraOwner controller_;
    OriginalFollowCamera follow_;
    OriginalAttachedCamera attached_;
    OriginalRaceStartCamera start_;
    std::array<OriginalFollowPreset,32> presets_{};
    OriginalFollowPresetBank presetBank_;
    OriginalAttachedOffset attachedOffset_;
    OriginalCourseStartActions startActions_;
    RaceStartPreparationReceipt preparation_;
    std::array<std::uint32_t,16> seedSamples_{};std::size_t seedCount_=0;
    OriginalCameraChildRegistry collection_;
    std::array<std::uint64_t,3> childOrder_{};
    OriginalStartChildBinding introChild_;
    OriginalAttachedChildRegistry attachedChild_;
    OriginalFollowChildRegistry followChild_;
    OriginalCameraObjectChildHeads objectChildren_;
    OriginalCameraControllerEntityFields controllerEntity_;
    OriginalCameraSceneOwner spatial_;
    std::array<CollectionZoneLink50BX,lighting::maximumPortals+1> links_{};
    std::array<LiveZoneEntry50EN,lighting::maximumPortals+1> entries_{};
    OriginalCameraVisibilityKeys visibilityKeys_;
    OriginalFollowSceneBinding sceneBinding_;
    CameraSceneState49F scene_;
    OriginalCameraSceneTick tick_;
    OriginalCameraViewportFields viewport_;
    TVCameraUpdateOutput49F pose_{};
    CameraViewOutput49F matrices_;
    OriginalRaceCameraDisplayFields display_;
    OriginalRaceViewportCall coldViewport_;
    OriginalGcGaIntroExitOwner exit_;
    OriginalRaceStartCountdown countdown_;
    OriginalVideoBlockingFade blockingFade_;
    AsyncRetainedFade asyncFade_;
    bool fadeInRequested_=false;
    Result result_;
    OriginalCameraModeInputResult lastInput_{};

    template<class T> static std::uint64_t key(const T& owner) noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&owner));
    }
    static bool same(float a,float b) noexcept{return std::memcmp(&a,&b,sizeof a)==0;}
    Result set(Status s,std::uint32_t pc=0) noexcept {result_.status=s;result_.sourcePC=pc;return result_;}
    Result fail(Status s,std::uint32_t pc=0) noexcept {frameStage_=FrameStage::failed;return set(s,pc);}
    OriginalRaceCameraSceneBorrow sceneBorrow() noexcept {
        return {&viewport_,&scene_,&tick_,&pose_.position,&matrices_};
    }
    bool identitiesCurrent() const {
        if(actual_.bank.count()!=8)return false;
        for(std::size_t i=0;i<8;++i){
            if(!actual_.bank.active(i)||!actual_.bank.read(i,reads_.aiInitialized)||
               actual_.bank.read(i,reads_.managerIndex)!=i||actual_.bank.read(i,reads_.class58)!=1u||
               actual_.bank.read(i,reads_.pod60)!=identity_[i].pod60)return false;
        }
        return true;
    }
    bool cutCurrent(Cut cut) const {
        if(!identitiesCurrent())return false;
        if(cut==Cut::heldBeforeStart&&(actual_.all8Manager.flags58&2u))return false;
        for(std::size_t i=0;i<8;++i){
            const auto expected=cut==Cut::heldBeforeStart?Phase::Ready:Phase::AfterBodyChildren;
            if(actual_.bank.read(i,reads_.phase)!=expected)return false;
            if(cut==Cut::afterAllRacerPrefixes&&
               !same(actual_.bank.read(i,reads_.frameFactor),frameDelta_))return false;
        }
        return true;
    }
    bool readRacers(RaceStartRacerBank& out,GcStartRacerEffectBank& effects) const {
        if(!identitiesCurrent())return false;
        out.sessionGeneration=generation_;
        for(std::size_t i=0;i<8;++i){
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
        if(!identitiesCurrent()||(actual_.all8Manager.flags58&1u))return false;
        const auto state=actual_.bank.read(0,reads_.state40),retirement=actual_.bank.read(0,reads_.retirementCD4);
        if(state>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())||
           retirement>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return false;
        frame={generation_,key(identity_[0].racerTag),key(identity_[0].mainBodyTag),frameDelta_,
            actual_.all8Manager.flags58,raceFlags1B0_,static_cast<std::int32_t>(state),
            static_cast<std::int32_t>(retirement),0};
        //race94 is deliberately not represented: bit0 special branch rejects
        //above before that field could be consumed. Do not copy manager30 here.
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
        OriginalGcGaIntroExitBindings bindings{controller_,start_,introChild_,attached_,attachedChild_,follow_,
            sceneBinding_,*world,spatial_,zones,*work,transition,display_,exitScene,app.video,
            actual_.applicationClock,countdown_,actual_.all8Manager,startActions_,raceFlags1B0_,uiFlags1E4_,
            app.fadeManagerA8,app.applicationMode22C,app.application33C,
            static_cast<std::int32_t>(actual_.launchKind11258),
            static_cast<std::int32_t>(preference.field11030),static_cast<std::int32_t>(preference.field11038),
            0,static_cast<std::int32_t>(actual_.bank.read(0,reads_.state40))};
        //Source80 null is proved by the authenticated GC/GA zero-cut route;
        //it is not a muted-audio success override. Native existing suffix audio
        //continues in its real caller, outside this camera owner.
        result_.start=exit_.advance(bindings,actual_.bank,actual_.baseTail,actual_.arena.fields,
            app.renderer,app.diagnostic);
        if(result_.start.status!=OriginalCameraStartStatus::complete)
            return set(Status::exitPending,result_.start.sourcePC);
        if(!countdownInstalled_){
            auto& host=actual_.hostRace;
            //The already initialized route/standings owner is retained. This
            //is the ONE advancing countdown; source34 is only its start input.
            if(host.phase!=race_session50HM::Phase::Countdown||host.racerCount!=8||host.raceSeconds!=0||
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
    Session(const Session&)=delete;Session& operator=(const Session&)=delete;
    Session(Session&&)=delete;Session& operator=(Session&&)=delete;
    const Result& diagnostic()const noexcept{return result_;}
    bool ready()const noexcept{return coldBound_;}
    bool abandoned()const noexcept{return abandoned_;}
    bool countdownInstalled()const noexcept{return countdownInstalled_;}
    bool introExitReady()const noexcept{return controller_.mode44==7&&start_.complete240;}
    std::int32_t cameraMode()const noexcept{return controller_.mode44;}
    std::uint64_t sessionGeneration()const noexcept{return generation_;}
    std::uint64_t racerKey(std::size_t i)const noexcept{return i<8?key(identity_[i].racerTag):0;}
    std::uint64_t mainBodyKey(std::size_t i)const noexcept{return i<8?key(identity_[i].mainBodyTag):0;}
    const CameraViewOutput49F& view()const noexcept{return matrices_;}
    const OriginalCameraViewportFields& viewport()const noexcept{return viewport_;}
    const TVCameraUpdateOutput49F& pose()const noexcept{return pose_;}
    bool nativeHudVisible()const noexcept{return !(uiFlags1E4_&1u);}
    bool hasView()const noexcept{return publicationCount_!=0;}
    std::uint64_t publicationCount()const noexcept{return publicationCount_;}
    bool blockingVideoPending()const noexcept {
        return !abandoned_&&displayInputs_&&(displayInputs_->video.flags404Low6&3u)&&
            !(displayInputs_->video.flags404Low6&4u);
    }
    bool asyncVideoPending()const noexcept {
        //Numeric25B5D0 may clear direction bits before the final staged GPU
        //sample/restore retires. Retained owner state, not flags alone, wins.
        return !abandoned_&&fadeInRequested_&&!asyncFade_.complete();
    }
    bool pendingDisplayWork()const noexcept {
        return displayInputs_&&((displayInputs_->video.flags404Low6&3u)||
            (raceFlags1B0_&8u)||displayInputs_->fadeManagerA8==8||
            (asyncFade_.started()&&!asyncFade_.complete()));
    }
    std::uint64_t publishedFrame()const noexcept{return completedFrame_;}
    const RaceStartPreparationReceipt& introPreparation()const noexcept{return preparation_;}
    const OriginalRaceCameraOwner& controller()const noexcept{return controller_;}
    //Only an actual authored trigger enter may call this. No event is created
    //from speed/contact or elapsed camera time. Same original camera0 owner.
    bool applyAuthoredShakeEnter(bool entering,const OriginalCameraShakeAction& action) noexcept {
        if(!coldBound_||(frameStage_!=FrameStage::idle&&frameStage_!=FrameStage::published&&
            frameStage_!=FrameStage::cameraReady))return false;
        return applyOriginalCameraShakeEnter(entering,action,&controller_,1);
    }

    //Cold after actual ALL8 roster/final-grid/7F0 setup and existing neutral
    //setup tick, before native countdown/GO. The live50Y history is NOT reset.
    //authored, app and every borrowed owner/backing must outlive this Session.
    //generation is the native race lifetime key; it does not assert a guest ID.
    Result prepare(std::uint64_t generation,const AuthoredInputs& authored,DisplayInputs& app){
        if(coldBound_||closed_)return set(Status::order);
        if(frameStage_==FrameStage::failed)return set(Status::terminal);
        if(!generation||actual_.bank.count()!=8||actual_.all8Manager.flags58&2u||
           actual_.launchKind11258!=1u||
           actual_.hostRace.phase!=race_session50HM::Phase::Countdown||actual_.hostRace.racerCount!=8||
           actual_.hostRace.raceSeconds!=0||actual_.hostRace.finishCount)return set(Status::owner);
        if(displayInputs_&&displayInputs_!=&app)return set(Status::owner);
        if(generation_&&generation_!=generation)return set(Status::owner);
        generation_=generation;displayInputs_=&app;
        if(!readsBound_){
            reads_.bind(actual_);readsBound_=true;
            for(std::size_t i=0;i<8;++i){
                identity_[i].managerIndex=static_cast<std::uint32_t>(i);
                identity_[i].pod60=actual_.bank.read(i,reads_.pod60);
            }
        }
        if(!identitiesCurrent())return set(Status::owner);
        for(std::size_t i=0;i<8;++i)if(actual_.bank.read(i,reads_.phase)!=Phase::Ready)return set(Status::phase);
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
                    0,CameraRacerClass::ai,CameraBootstrap::native_all_ai};
                if(!bindFreshOriginalRaceCamera(controller_,b))return set(Status::owner);
                //Explicit reduced NATIVE registry: actual fully owned children
                //in source relative order. Source unused68/70/78 are omitted;
                //count/ordinals are not retail parity. No dummy success owners.
                childOrder_={{key(follow_),key(attached_),key(start_)}};
                if(!bindFreshOriginalCameraChildRegistry(collection_,generation_,key(controller_),key(scene_),
                    childOrder_.data(),childOrder_.size()))return set(Status::owner);
                introChild_={&start_,key(start_),&collection_};attachedChild_={&attached_,key(attached_),&collection_};
                followChild_={&follow_,key(follow_),&collection_};
                objectChildren_={generation_,key(controller_),key(scene_),0,0};
                controllerEntity_.controllerKey=key(controller_);display_.controllers48[0]=&controllerEntity_;
                ++coldPhase_;break;}
            case 7:
                result_.visibilityJoin=attachFreshOriginalCamera0Visibility(actual_.visibility,generation_,key(scene_),
                    spatial_,{links_.data(),entries_.data(),links_.size()},visibilityKeys_,sceneBinding_);
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
        if(!coldBound_||!displayInputs_)return set(Status::owner);
        if(frameStage_==FrameStage::failed)return set(Status::terminal);
        if(frame!=frameNumber_||cut!=cut_||!cutCurrent(cut))return set(Status::order);
        if(frameStage_==FrameStage::cameraReady)frameStage_=FrameStage::cameraDispatch;
        for(;;){switch(frameStage_){
        case FrameStage::cameraDispatch:
            result_.start=serviceBeforeChild();
            if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::renderer,result_.start.sourcePC);
            frameStage_=FrameStage::cameraUpdate;break;
        case FrameStage::cameraUpdate:{
            StableRaceCameraFrame current;if(!stableFrame(current))return fail(Status::sourcePath,0x175d70);
            if(controller_.mode44==4||controller_.mode44==7){
                RaceStartRacerBank bank;GcStartRacerEffectBank fields;if(!readRacers(bank,fields))return fail(Status::owner);
                const auto& p=actual_.preferences.slots[0];
                if(actual_.launchKind11258!=1u)return fail(Status::sourcePath,0x175d70);
                const GcCameraStartContext context{0,static_cast<std::int32_t>(actual_.launchKind11258),static_cast<std::int32_t>(p.field11030),
                    static_cast<std::int32_t>(p.field11038),displayInputs_->rootFlags228,sceneFlags29_};
                const auto r=advanceOriginalGcCameraStartFrame(controller_,start_,introChild_,fields,bank,current,
                    context,actual_.blur,scene_,pose_,matrices_);
                if(r.status==GcStartFrameStatus::needsOriginalExit){
                    if(!completionPrefix())return fail(Status::numeric,0x175d70);
                    frameStage_=FrameStage::exit;break;
                }
                if(r.status!=GcStartFrameStatus::posePublished)return fail(Status::sourcePath,0x175d70);
                if(r.activatedStartCamera){
                    //Temporary value projection is discarded. The actual AE8
                    //entry stores land in all eight real banked base tails.
                    for(std::size_t i=0;i<8;++i)actual_.bank.visit(i,[&](){setOriginalCameraRacerIntroActive(actual_.baseTail);});
                }
                frameStage_=FrameStage::lateTick;break;
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
            result_.attached=r.pose;
            if(r.status!=OriginalAttachedRaceFrameStatus::complete)return fail(Status::sourcePath,r.pose.sourcePC);
            frameStage_=FrameStage::stableMatrix;break;}
        case FrameStage::exit:{
            const auto r=advanceExit();if(r.status!=Status::complete)return r;break;}
        case FrameStage::exitPose:{
            //Same175D70 handoff frame reaches115CB0 after177AB0. Its common
            //timer prefix already ran; do not run stable mode5 control again.
            const auto body=attachedFrame();
            result_.attached=actual_.bank.visit(0,[&](){return advanceOriginalAttachedCameraView(attached_,body,
                {actual_.arena.nodes,actual_.arena.sceneNodeCount},actual_.attachedGlobals,actual_.arena.noiseOwner(),
                pose_,scene_,matrices_,displayInputs_->rootFlags228,sceneFlags29_);});
            if(result_.attached.status!=OriginalAttachedStatus::ok)return fail(Status::numeric,result_.attached.sourcePC);
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
        if(!coldBound_||abandoned_||pendingDisplayWork()||
           (frameStage_!=FrameStage::idle&&frameStage_!=FrameStage::published))return {false,0};
        std::array<std::uint32_t,8> viewState{};
        for(std::size_t i=0;i<viewState.size();++i)viewState[i]=actual_.bank.read(i,reads_.retirementCD4);
        OriginalCameraModeInputEnvironment environment{held,actual_.preferences,viewState.data(),viewState.size(),
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
        if(!race_session50HM::advanceCountdown(actual_.hostRace,static_cast<double>(actualPreparedSeconds))){
            if(actual_.hostRace.error!=race_session50HM::Error::None)return fail(Status::numeric);
            return set(Status::startHeld);
        }
        if(!goPublished_){
            actual_.all8Manager.flags58=static_cast<std::uint8_t>(actual_.all8Manager.flags58|2u);
            for(std::size_t i=0;i<8;++i)actual_.bank.visit(i,[&](){actual_.arena.fields.applicationByte58=actual_.all8Manager.flags58;});
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
    //Attached base image. Root owns that real renderer publication, not this
    //numeric camera counter. The backend will capture/validate the image.
    Result advanceRaceFadeInPrefix(){
        if(abandoned_)return set(Status::abandoned);
        if(!coldBound_||!displayInputs_||!exit_.complete()||!hasView())return set(Status::owner);
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
                app.applicationMode22C,app.fadeDuration98,true);
            if(result_.start.status!=OriginalCameraStartStatus::complete)return set(Status::videoPending,result_.start.sourcePC);
            app.fadeManagerA8=0;fadeInRequested_=true;
        }else if(app.fadeManagerA8)return set(Status::sourcePath,0x2ae7fc);
        return set(Status::complete,0x2ae958);
    }
    //Native held-underlay presentation policy. Call repeatedly from the real
    //presentation service, not once per simulation tick. No sleep, fake fence,
    //manufactured overlay or guessed source-clock milliseconds exist here.
    Result advanceAsyncVideo(OriginalVideoBlockingBackend* backend){
        if(abandoned_)return set(Status::abandoned);
        if(!coldBound_||!displayInputs_||!fadeInRequested_)return set(Status::owner);
        result_.start=asyncFade_.advance(displayInputs_->video,actual_.applicationClock,
            publicationCount_,backend);
        return set(result_.start.status==OriginalCameraStartStatus::complete?Status::complete:Status::videoPending,
            result_.start.sourcePC);
    }
    //Release before the Session's backing/keys can be destroyed while the
    //visibility owner remains usable. A failed removal must not be hidden by
    //a destructor success; retain this Session until the real owner is closed.
    //If the whole application world is destroyed together, its owner must
    //still outlive this Session and must never traverse these records again.
    Result close(){
        if(closed_)return set(Status::complete);
        if(pendingDisplayWork())return set(Status::videoPending);
        if(frameStage_!=FrameStage::idle&&frameStage_!=FrameStage::published&&
           frameStage_!=FrameStage::failed)return set(Status::order);
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
    //the camera permanently and remove ONLY its CPU visibility registration.
    //This operation needs no fake video-completed receipt: the GL backends do
    //not borrow these spatial buffers. The root must independently perform
    //their real cancelAndRetire before releasing renderer/display resources.
    //A removal failure retains this owner for retry; video flags/alpha, race
    //flags, countdown, RNG and racer history remain exactly as abandoned.
    Result abandonAndCloseSpatial(){
        abandoned_=true;frameStage_=FrameStage::failed;
        if(closed_)return set(Status::abandoned);
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
