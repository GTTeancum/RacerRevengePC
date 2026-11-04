#pragma once
#include "original_camera_start_effects.h"
#include "original_session_f8_intro50la.h"
#include "original_wreck_components50la.h"
#include "../../platform/linux/native/race_audio_groups50ic.h"
#include "original_attached_camera_controller.h"
#include "original_gc_camera_start_lifecycle.h"
#include "original_camera_racer_fields.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"

namespace rr::game {
// Additional fields of the ONE real renderer442F70, shared by every141ED0
// child dispatch and the viewport calls. These are a BORROW of its queue
// state, not camera-private defaults. Cold preparation must supply the actual
// constructor/init projection; this aggregate intentionally has no defaults.
struct OriginalRendererDispatchFields {
    std::uint8_t flagsD8;
    std::int32_t currentQueue44,count54,count60;
    std::uint64_t pendingCallbackDC;
};
enum class OriginalCameraDrainPolicy : std::uint8_t {
    requireCommittedImage,
    //Explicit native direct-start policy. Requires the real separately
    //reviewed initialized-context drain API; never invents an image receipt.
    initializedContext
};
//Exact244D60 empty-queue path. a1=0 performs no GL drain. The source return
//value0 means no queue submitted, while status complete means this call ran.
//Nonempty source queues need their real245030/244F20/DMA-owner adaptation;
//glFinish on an unrelated native queue cannot discharge that obligation.
inline OriginalCameraStartResult serviceOriginalRenderer244D60(
    const OriginalRendererDispatchFields& fields,bool wait,
    platform::VehicleWindowGl50DS& renderer,platform::VehicleWindowDiagnostic50DS& diagnostic,
    std::int32_t& actualV0,
    OriginalCameraDrainPolicy policy=OriginalCameraDrainPolicy::requireCommittedImage) noexcept {
    using S=OriginalCameraStartStatus;
    if(!(fields.flagsD8&1u)){actualV0=0;return {S::complete,0x244d7c};}
    if(fields.pendingCallbackDC)return {S::rendererCallback,wait?0x244d98u:0x244e30u};
    const bool queued=(fields.currentQueue44==1&&fields.count60>0)||
                      (fields.currentQueue44==2&&fields.count54>0);
    if(queued)return {S::rendererQueue,0x244e10};
    if(wait){
        const bool completed=policy==OriginalCameraDrainPolicy::initializedContext?
            renderer.finishInitializedContextWork(diagnostic)&&diagnostic.contextWorkCompleted:
            renderer.finishSubmittedWork(diagnostic)&&diagnostic.submittedWorkCompleted;
        if(!completed)return {diagnostic.error==platform::VehicleWindowError50DS::noImage?
            S::rendererNoImage:S::rendererDrain,0x244dc4};
    }
    actualV0=0;return {S::complete,0x244e04};
}
// Real native platform adaptation of244D60(1),245090->242EE0(1,3).
// Calls the existing renderer, never accepts a fabricated completion receipt.
// A nonnull source render callbackDC is an additional owner, not a GL fence;
// it stays explicit. No-image direct startup remains a concrete boundary.
inline OriginalCameraStartResult drainOriginalCameraRenderer(
    platform::VehicleWindowGl50DS& renderer,const OriginalRendererDispatchFields& fields,
    platform::VehicleWindowDiagnostic50DS& diagnostic) noexcept {
    using S=OriginalCameraStartStatus;
    std::int32_t ignoredV0=0;
    const auto queued=serviceOriginalRenderer244D60(fields,true,renderer,diagnostic,ignoredV0);
    if(queued.status!=S::complete)return queued;
    //2458C0 explicitly calls245090 again after244D60(1), even when the
    //first call took the disabled-D8 return. Preserve both real drains.
    if(!renderer.finishSubmittedWork(diagnostic)||!diagnostic.submittedWorkCompleted)
        return {diagnostic.error==platform::VehicleWindowError50DS::noImage?S::rendererNoImage:S::rendererDrain,0x245910};
    return {S::complete,0x245918};
}
// Exact bounded2450A0: the actual243A80 renderer table3D2050 at+40
// has slot+24=245090, which calls242EE0(1,3). This is ONE completion,
// not244D60 queue submission and not the two-drain2458C0 destructor path.
// Admit only the represented empty queues/no callback. A real source callback
// or queue cannot be discharged by completing unrelated native GPU work.
// No image is required by245090; the actual initialized context is required.
inline OriginalCameraStartResult resetOriginalRenderer2450A0(
    OriginalRendererDispatchFields& fields,platform::VehicleWindowGl50DS& renderer,
    platform::VehicleWindowDiagnostic50DS& diagnostic) noexcept {
    using S=OriginalCameraStartStatus;
    if(fields.pendingCallbackDC)return {S::rendererCallback,0x242f78};
    if(fields.count54!=0||fields.count60!=0)return {S::rendererQueue,0x2450b8};
    // Even an already-clear D8 must perform the original virtual+24 call.
    if(!renderer.finishInitializedContextWork(diagnostic)||!diagnostic.contextWorkCompleted)
        return {S::rendererDrain,0x2450b8};
    //2450C8 is ANDI a0,zero,1 (zero), so the final OR does not set bit0.
    // Preserve all other bits. On a failed drain the original suffix has not run.
    fields.flagsD8&=static_cast<std::uint8_t>(~1u);
    return {S::complete,0x2450d4};
}
//Entity29 and children18 are not camera-child collection38/selected34.
//Cold174A30->141300 yields low29=3 and no generic entity children. The
//115DE0 registered camera array does not populate this entity child list.
struct OriginalCameraControllerEntityFields {
    std::uint64_t controllerKey=0,firstEntityChild18=0;
    std::uint8_t flags29=3;
    OriginalWreckComponent50LA* actualController=nullptr;
    OriginalWreckComponent50LA* actualIntroAudio=nullptr;
};
struct OriginalRaceCameraDisplayFields {
    std::uint64_t sessionGeneration=0,raceKey=0;
    std::array<std::uint64_t,4> applicationCameras254{};
    std::uint32_t cameraCount264=0;
    std::int32_t viewportMode8C=0;
    std::uint64_t extraScene44=0;
    std::array<OriginalCameraControllerEntityFields*,4> controllers48{};
    //Global renderer442F70 is separate from video443A60. These fields and
    //the actual GL renderer remain application-wide, outside racer snapshots.
    OriginalCameraViewportFields* rendererCurrentScene0=nullptr;
    std::uint32_t rendererBufferCount0C=0,rendererFogColor28=0;
    OriginalRendererDispatchFields* rendererDispatch=nullptr;
    OriginalCameraDrainPolicy drainPolicy=OriginalCameraDrainPolicy::requireCommittedImage;
    OriginalRaceFogFields* actualTrackFog3EC=nullptr;
};
struct OriginalRaceCameraSceneBorrow {
    OriginalCameraViewportFields* viewport=nullptr;
    CameraSceneState49F* scene=nullptr;
    OriginalCameraSceneTick* tick=nullptr;
    const assets::TVCameraVector49E* position180=nullptr;
    CameraViewOutput49F* matrices=nullptr;
};
//26F940 direct selection after real1146B0/1156E0. The admitted fresh scene
//has source640x448 dimensions already; the different-size114920 branch stays
//explicit rather than silently substituting host-window dimensions.
inline OriginalCameraStartResult selectOriginalRaceScene26F940(
    OriginalRaceCameraDisplayFields& display,OriginalCameraViewportFields& scene,
    std::int32_t actualRendererWidth4,std::int32_t actualRendererHeight8) noexcept {
    using S=OriginalCameraStartStatus;
    if(!scene.prepared||scene.sessionGeneration!=display.sessionGeneration)return {S::owner,0x26f948};
    if(scene.fullWidth204!=actualRendererWidth4||scene.fullHeight208!=actualRendererHeight8)
        return {S::sourcePath,0x26f978};
    display.rendererCurrentScene0=&scene;return {S::complete,0x26f998};
}
class OriginalRaceViewportCall final {
    unsigned phase_=0;
    unsigned drainPhase_=0;
    bool started_=false,completed_=false,cold_=false;
    OriginalRaceCameraDisplayFields* owner_=nullptr;
    OriginalCameraViewportFields* viewport_=nullptr;
    platform::VehicleWindowGl50DS* renderer_=nullptr;
    OriginalCameraDrainPolicy drainPolicy_=OriginalCameraDrainPolicy::requireCommittedImage;
    OriginalCameraStartResult applicationList(OriginalRaceCameraDisplayFields& d,
        const OriginalRaceCameraSceneBorrow& scene) noexcept {
        d.applicationCameras254={{scene.viewport->sceneKey,0,0,0}};d.cameraCount264=1;
        return {OriginalCameraStartStatus::complete,0x1401f8};
    }
    OriginalCameraStartResult fog(OriginalRaceCameraDisplayFields& d) noexcept {
        if(!d.actualTrackFog3EC)return {OriginalCameraStartStatus::complete,0x1a98e4};
        return restoreOriginalRaceFog1A98DC(*d.actualTrackFog3EC,d.rendererFogColor28);
    }
    OriginalCameraStartResult controllers(OriginalRaceCameraDisplayFields& d) noexcept {
        //The real fresh controller entity trees have no generic children.
        //Do not claim that unknown virtual notification descendants ran.
        for(unsigned i=0;i<d.controllers48.size();++i){
            auto* c=d.controllers48[i];if(!c||!c->firstEntityChild18)continue;
            auto* parent=c->actualController;auto* child=c->actualIntroAudio;
            if(!parent||!child||parent->firstChild18!=child||child->parent1C!=parent||
               child->next14||child->previous10||child->firstChild18||
               child->kind!=OriginalWreckComponentKind50LA::wattoEmitter2921C0||
               c->firstEntityChild18!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(child)))
                return {OriginalCameraStartStatus::sceneChildren,0x1414cc};
            OriginalWreckComponent50LA* nodes[]={parent,child};
            const OriginalWreckComponentDomain50LA domain{parent->sessionGeneration,nodes,2};
            const auto r=setOriginalWreckBaseActivity50LA(domain,*parent,i==0);
            if(r.status!=OriginalWreckComponentStatus50LA::complete)
                return {OriginalCameraStartStatus::sceneChildren,r.sourcePC};
            c->flags29=parent->flags29;
        }
        if(d.controllers48[0])d.controllers48[0]->flags29|=3u;
        for(unsigned i=1;i<4;++i)if(d.controllers48[i])d.controllers48[i]->flags29&=static_cast<std::uint8_t>(~3u);
        return {OriginalCameraStartStatus::complete,0x1a9938};
    }
public:
    bool complete() const noexcept {return completed_;}
    //One instance per source invocation, including cold1A9590(true). On a
    //blocked GL drain resume this SAME call; no source prefix is replayed.
    //The scene/display and their actual owner borrows stay fixed until done.
    OriginalCameraStartResult advance(bool cold,OriginalRaceCameraDisplayFields& display,
        const OriginalRaceCameraSceneBorrow& scene,platform::VehicleWindowGl50DS& renderer,
        platform::VehicleWindowDiagnostic50DS& diagnostic) noexcept {
        using S=OriginalCameraStartStatus;
        if(completed_)return {S::alreadyComplete,0x1a9ec8};
        if(!scene.viewport||!scene.scene||!scene.tick||!scene.position180||!scene.matrices||!display.rendererDispatch||
           !display.sessionGeneration||!display.raceKey||!scene.viewport->prepared||
           display.sessionGeneration!=scene.viewport->sessionGeneration||!scene.tick->bound||
           scene.tick->sessionGeneration!=display.sessionGeneration||scene.tick->sceneKey!=scene.viewport->sceneKey)
            return {S::owner,0x1a9750};
        if(!cold&&(display.viewportMode8C!=0||display.extraScene44))return {S::sourcePath,0x1a9840};
        if(started_&&(cold_!=cold||owner_!=&display||viewport_!=scene.viewport||renderer_!=&renderer||
            drainPolicy_!=display.drainPolicy))return {S::owner,0x1a9750};
        if(!started_){started_=true;cold_=cold;owner_=&display;viewport_=scene.viewport;
            renderer_=&renderer;drainPolicy_=display.drainPolicy;}
        for(;;){
            if(phase_==7){completed_=true;return {S::complete,0x1a9ec8};}
            //Cold: viewport/drain/matrix THEN camera list/fog/controller flags.
            //Exit: list/fog/controller flags THEN viewport/drain/matrix.
            const unsigned action=cold_?phase_:(phase_<3?phase_+4:phase_-3);
            OriginalCameraStartResult r{S::complete,0};
            switch(action){
            case 0:r=setOriginalFullRaceViewport1158A0(*scene.viewport,*scene.scene,*scene.tick,display.rendererBufferCount0C);break;
            case 1:
                //Each successful external barrier is retained separately. If
                //the second call blocks/fails, resuming must not repeat the
                //already-completed244D60(1) call (including disabled-D8 return).
                if(drainPhase_==0){
                    std::int32_t ignoredV0=0;
                    r=serviceOriginalRenderer244D60(*display.rendererDispatch,true,
                        renderer,diagnostic,ignoredV0,display.drainPolicy);
                    if(r.status!=S::complete)return r;
                    drainPhase_=1;
                }
                if(drainPhase_==1){
                    const bool completed=display.drainPolicy==OriginalCameraDrainPolicy::initializedContext?
                        renderer.finishInitializedContextWork(diagnostic)&&diagnostic.contextWorkCompleted:
                        renderer.finishSubmittedWork(diagnostic)&&diagnostic.submittedWorkCompleted;
                    if(!completed)
                        return {diagnostic.error==platform::VehicleWindowError50DS::noImage?
                            S::rendererNoImage:S::rendererDrain,0x245910};
                    drainPhase_=2;
                }
                break;
            case 2:
                //245918 tests actual renderer current scene0 before zoom.
                if(display.rendererCurrentScene0)r=updateOriginalViewportZoom2458C0(*scene.viewport,*display.rendererCurrentScene0);
                break;
            case 3:r=rebuildOriginalRaceScene236DD0(*scene.viewport,*scene.scene,*scene.position180,*scene.matrices);break;
            case 4:r=applicationList(display,scene);break;
            case 5:r=fog(display);break;
            case 6:r=controllers(display);break;
            default:completed_=true;return {S::complete,0x1a9ec8};
            }
            if(r.status!=S::complete)return r;
            ++phase_;
        }
    }
};

struct OriginalGcIntroExitBindings {
    OriginalRaceCameraOwner& controller;
    OriginalRaceStartCamera& start7C;
    OriginalStartChildBinding& introChild;
    OriginalAttachedCamera& attached6C;
    OriginalAttachedChildRegistry& attachedChild;
    OriginalFollowCamera& follow64;
    OriginalFollowChildRegistry& followChild;
    OriginalFollowSceneBinding& numericScene;
    ResetVisibilityWorld50EY& visibility;
    OriginalCameraSceneOwner& spatialScene;
    const OriginalFollowVisibilityZones& zoneKeys;
    LiveZoneWorkspace50EN& workspace;
    const OriginalFollowSceneTransitionInput& transitionInput;
    OriginalRaceCameraDisplayFields& display;
    OriginalRaceCameraSceneBorrow scene;
    OriginalVideoFadeState& video;
    HostMonotonicClock50ID& sharedClock;
    OriginalRaceStartCountdown& countdown;
    LapTimingManager50ID& actualManager;
    const OriginalCourseStartActions& startActions;
    std::uint8_t& raceFlags1B0;
    std::uint8_t& actualUiFlags1E4;
    std::int32_t actualFadeManagerA8,actualApplicationMode22C,actualApplication33C;
    std::int32_t actualLaunch11258,actualPreference11030,actualVariant11038;
    //Actual source80.177750(effective A8 intro count0) never allocates it.
    std::uint64_t actualIntroAudio80;
    //Current common racer40, not class58 or an input-role inference.
    std::int32_t actualRacerState40;
    CoupledNoiseContext49M* startRandom=nullptr;
    void* startMusicContext=nullptr;
    bool (*startMusicRequest)(void*,std::int32_t,std::int32_t)=nullptr;
    platform::RaceAudioGroups50IC* introAudioOwner=nullptr;
    //0 natural mode7 completion;1 genuine mode7 skip;2 no7C skip;3 no7C exhaustion.
    unsigned taExitReason=0;
    bool (*introMusicFadeRequest)(void*,float)=nullptr;
    // Optional concrete Session owner; legacy standalone callers keep their
    // existing numeric projection. This is not a generic callback or receipt.
    const OriginalSessionF8Intro50LA* sessionF8Intro=nullptr;
};
//1AFD80 returns from1A9750 before types2/3 call1AD3C0. The one
//308030 draw is retained even if the actual music consumer cannot yet finish.
class OriginalEventStartMusic final {
    bool drawn_=false,complete_=false;std::int32_t line_=0;
public:
    OriginalCameraStartResult advance(OriginalGcIntroExitBindings& b) noexcept {
        using S=OriginalCameraStartStatus;
        if(complete_)return {S::complete,0x1afe20};
        if(b.actualLaunch11258==1){complete_=true;return {S::complete,0x1afe20};}
        if((b.actualLaunch11258!=2&&b.actualLaunch11258!=3)||!b.startRandom||!b.startMusicRequest)
            return {S::owner,0x1ad3c0};
        if(!drawn_){
            if(std::fegetround()!=FE_TONEAREST)return {S::arithmetic,0x1ad3dc};
            auto& raw=b.startRandom->sharedRandomState;raw=raw*0x41c64e6du+0x3039u;
            volatile float converted=static_cast<float>(static_cast<std::int32_t>(raw&0x7fffffffu));
            volatile float probability=converted*0x1p-31f;
            line_=probability<0.5f?45:44;drawn_=true;
        }
        if(!b.startMusicRequest(b.startMusicContext,line_,1))return {S::sourcePath,0x1a46e0};
        complete_=true;return {S::complete,0x1afe20};
    }
};
//1762F4, not the completed mode7 exit: no fade, racer AE8 reset, raceFlag8,
//intro-finished flag or cooldown. Real saved preference, viewport and music
//owners are still executed in the original order.
class OriginalDirectRaceStartOwner final {
    unsigned phase_=0;bool complete_=false;std::uint64_t generation_=0;
    OriginalRaceViewportCall viewport_;OriginalEventStartMusic music_;
public:
    bool complete()const noexcept{return complete_;}
    OriginalCameraStartResult advance(OriginalGcIntroExitBindings& b,
        platform::VehicleWindowGl50DS& renderer,platform::VehicleWindowDiagnostic50DS& diagnostic) noexcept {
        using S=OriginalCameraStartStatus;
        auto* registry=b.followChild.collection;
        if(complete_)return {S::alreadyComplete,0x176728};
        if(!registry||!registry->bound||registry!=b.introChild.collection||registry!=b.attachedChild.collection||
           !registry->registered38||!registry->count30||registry->selected34>=registry->count30||
           !b.controller.bound||registry->sessionGeneration!=b.controller.binding.sessionGeneration||
           registry->sceneKey!=b.controller.binding.sceneKey||!b.follow64.prepared||
           b.follow64.sessionGeneration!=registry->sessionGeneration||
           (generation_&&generation_!=registry->sessionGeneration))return {S::owner,0x1762f4};
        if(b.actualLaunch11258<1||b.actualLaunch11258>3||
           (b.actualLaunch11258!=2&&b.start7C.route[0]!=-1)||
           (b.actualPreference11030!=0&&b.actualPreference11030!=5)||
           b.actualRacerState40!=0||b.actualIntroAudio80||(b.raceFlags1B0&3u)!=1u||
           b.controller.flags62&0xe0u||!b.startActions.authenticated())return {S::sourcePath,0x1762f4};
        if(!generation_){if(b.controller.mode44!=4)return {S::owner,0x1762f4};generation_=registry->sessionGeneration;}
        for(;;){OriginalCameraStartResult r{S::complete,0};switch(phase_){
        case 0:{
            std::size_t index=0;for(;index<registry->count30;++index)if(registry->registered38[index]==b.followChild.follow64Key)break;
            if(index==registry->count30)return {S::owner,0x115e60};
            registry->selected34=index;b.controller.mode44=0;
            //Native active is a dispatch-lifetime guard, not a source field.
            //This direct selection uses the already initialized constructor
            //values and must NOT replay179F40 or reset snap/filter state.
            b.follow64.active=true;break;}
        case 1:{
            b.controller.flags63Low5&=static_cast<std::uint8_t>(~2u);b.controller.specialDelayA4=1;
            const auto switched=b.actualPreference11030==0?
                transitionOriginalFollow177AB0(b.controller,b.follow64,b.numericScene,b.followChild,
                    b.visibility,b.spatialScene,b.zoneKeys,b.workspace,0,&b.actualVariant11038,b.transitionInput):
                transitionOriginalAttached177AB0(b.controller,b.attached6C,b.numericScene,b.attachedChild,
                    b.visibility,b.spatialScene,b.zoneKeys,b.workspace,5,b.transitionInput);
            if(switched.status!=OriginalFollowControllerStatus::complete)return {S::sourcePath,switched.sourcePC};
            b.follow64.distanceOffset58=b.follow64.yawOffset5C=b.follow64.pitchOffset60=b.follow64.fovOffset64=0;break;}
        case 2:r=beginOriginalRaceCountdown1AFD80(b.countdown,b.actualManager,b.actualApplication33C,b.actualLaunch11258,b.startActions);break;
        case 3:r=viewport_.advance(false,b.display,b.scene,renderer,diagnostic);break;
        case 4:r=music_.advance(b);break;
        case 5:
            if(b.sessionF8Intro){
                if(!b.sessionF8Intro->reveal176330(b.actualUiFlags1E4))return {S::owner,0x176330};
            }else revealOriginalRaceUi199770(b.actualUiFlags1E4);
            break;
        default:complete_=true;return {S::complete,0x176728};
        }if(r.status!=S::complete)return r;++phase_;}
    }
};
class OriginalGcIntroExitOwner final {
    unsigned phase_=0;std::size_t nextRacer_=0;
    bool begun_=false,completed_=false;
    OriginalCameraExitFadeRequest2AED60 fadeRequest_;
    std::uint64_t generation_=0,controllerKey_=0;
    OriginalRaceViewportCall viewportCall_;
    OriginalEventStartMusic music_;
public:
    bool complete() const noexcept {return completed_;}
    bool requestPending()const noexcept{return fadeRequest_.pending();}
    //RacerBank must be the actual stable50HN Bank. The references point to
    //its existing live arena, whose registered slots contain BaseTail/fields.
    //No GcStartRacerEffectBank shadow state or pretend Player owner is used.
    template<class RacerBank>
    OriginalCameraStartResult advance(OriginalGcIntroExitBindings& b,RacerBank& bank,
        BaseTailState50X& liveTail,VehicleFrameFields49W& liveFields,
        platform::VehicleWindowGl50DS& renderer,platform::VehicleWindowDiagnostic50DS& d) {
        using S=OriginalCameraStartStatus;
        if(completed_)return {S::alreadyComplete,0x176728};
        const auto* registry=b.introChild.collection;
        if(!b.controller.bound||!b.start7C.prepared||(!b.start7C.active&&b.taExitReason<2)||(!b.start7C.complete240&&!b.taExitReason)||
           b.introChild.start7C!=&b.start7C||!registry||!registry->bound||
           registry!=b.attachedChild.collection||registry!=b.followChild.collection||registry->sessionGeneration!=b.controller.binding.sessionGeneration||
           registry->sceneKey!=b.controller.binding.sceneKey||b.start7C.sessionGeneration!=registry->sessionGeneration||
           !registry->controllerKey||!registry->registered38||registry->selected34>=registry->count30||bank.count()!=8||
           b.display.sessionGeneration!=registry->sessionGeneration||b.display.controllers48[0]==nullptr||
           b.display.controllers48[0]->controllerKey!=registry->controllerKey||!b.follow64.prepared||
           b.follow64.sessionGeneration!=registry->sessionGeneration||b.follow64.racerKey!=b.controller.binding.racerKey)
            return {S::owner,0x176654};
        if((b.actualLaunch11258!=1&&b.actualLaunch11258!=3)||(b.actualPreference11030!=5&&b.actualPreference11030!=0)||
           b.actualRacerState40!=0||(b.actualIntroAudio80&&(!b.introAudioOwner||
               b.introAudioOwner->taIntroEntity80()==nullptr))||b.controller.flags62&0xe0u||
           (b.raceFlags1B0&3u)!=1u||!b.startActions.authenticated())return {S::sourcePath,0x176664};
        if(!begun_){
            if((b.taExitReason<2&&(b.controller.mode44!=7||registry->registered38[registry->selected34]!=b.introChild.start7CKey))||
               (b.taExitReason>=2&&b.controller.mode44!=4))return {S::owner,0x176648};
            generation_=registry->sessionGeneration;controllerKey_=registry->controllerKey;begun_=true;
        }else if(generation_!=registry->sessionGeneration||controllerKey_!=registry->controllerKey)
            return {S::owner,0x176664};
        for(;;){
            OriginalCameraStartResult r{S::complete,0};
            switch(phase_){
            case 0:b.controller.flags63Low5|=1u;break;
            case 1:
                for(;nextRacer_<8;++nextRacer_)bank.visit(nextRacer_,[&](){
                    resetOriginalCameraRacerStartFields(liveTail,liveFields);
                });
                break;
            case 2:r=fadeRequest_.invoke(b.actualFadeManagerA8,b.actualApplicationMode22C,b.video,b.sharedClock);break;
            case 3:
                if(b.taExitReason>=2&&(!b.introMusicFadeRequest||!b.introMusicFadeRequest(b.startMusicContext,.5f)))
                    return {S::sourcePath,0x1a4ab0};
                b.raceFlags1B0|=8u;break;
            case 4:
                //178700 reads the actual saved mode AND Follow preset ID.
                //Single view skips17878C's destructive multi-view clamp.
                //Mode7 reaches178810, with the real ID pointer passed to177AB0.
                b.controller.flags63Low5&=static_cast<std::uint8_t>(~2u);b.controller.specialDelayA4=1;
                {const auto switched=b.actualPreference11030==0?
                    transitionOriginalFollow177AB0(b.controller,b.follow64,b.numericScene,b.followChild,
                        b.visibility,b.spatialScene,b.zoneKeys,b.workspace,0,&b.actualVariant11038,b.transitionInput):
                    transitionOriginalAttached177AB0(b.controller,b.attached6C,b.numericScene,b.attachedChild,
                        b.visibility,b.spatialScene,b.zoneKeys,b.workspace,5,b.transitionInput);
                 if(switched.status!=OriginalFollowControllerStatus::complete)return {S::sourcePath,switched.sourcePC};}
                break;
            case 5:
                if(!b.taExitReason)b.follow64.distanceOffset58=b.follow64.yawOffset5C=b.follow64.pitchOffset60=b.follow64.fovOffset64=0;
                break;
            case 6:r=beginOriginalRaceCountdown1AFD80(b.countdown,b.actualManager,b.actualApplication33C,
                    b.actualLaunch11258,b.startActions);break;
            case 7:r=viewportCall_.advance(false,b.display,b.scene,renderer,d);break;
            case 8:
                r=music_.advance(b);if(r.status!=S::complete)return r;
                // On rejection retain this cut, including completed music;
                // do not run cooldown/flags/emitter stop or replay earlier work.
                if(b.sessionF8Intro){
                    // The existing reason selects the represented original
                    // natural/skip/no7C call site, never a new source phase.
                    const std::uint32_t call=b.taExitReason==0?0x1766ecu:
                        b.taExitReason==1?0x1765d8u:b.taExitReason==2?0x17652cu:
                        b.taExitReason==3?0x175d30u:0x176664u;
                    if(!b.sessionF8Intro->revealIntroExit(b.actualUiFlags1E4))return {S::owner,call};
                }else revealOriginalRaceUi199770(b.actualUiFlags1E4);
                break;
            case 9:if(!b.taExitReason)b.controller.cooldown8C=1;break;
            case 10:if(!b.taExitReason)b.controller.flags63Low5|=1u;break;
            case 11:
                if(b.actualIntroAudio80&&!b.introAudioOwner->stopTaIntro2924A0())return {S::sourcePath,0x2924a0};
                // Current racer40=0, mode0/5: no mode correction follows.
                completed_=true;return {S::complete,0x1767c4};
            default:return {S::owner,0x176664};
            }
            if(r.status!=S::complete)return r;++phase_;
        }
    }
};
//The reached exit contains no course switch. Authenticated GC, GA and TA
//have the same absent321-action list; the LIVE completed procedural child,
//null80 and actual configuration predicates above remain mandatory.
using OriginalGcGaIntroExitOwner=OriginalGcIntroExitOwner;
using OriginalGcGaIntroExitBindings=OriginalGcIntroExitBindings;
}
