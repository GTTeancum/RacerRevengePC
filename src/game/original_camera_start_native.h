#pragma once
#include "original_camera_start_effects.h"
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
//Entity29 and children18 are not camera-child collection38/selected34.
//Cold174A30->141300 yields low29=3 and no generic entity children. The
//115DE0 registered camera array does not populate this entity child list.
struct OriginalCameraControllerEntityFields {
    std::uint64_t controllerKey=0,firstEntityChild18=0;
    std::uint8_t flags29=3;
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
        for(const auto* c:d.controllers48)if(c&&c->firstEntityChild18)
            return {OriginalCameraStartStatus::sceneChildren,0x1414cc};
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
    //Actual source80.177750(a8 intro count0) never allocates it on GC.
    std::uint64_t actualIntroAudio80;
    //Current common racer40, not class58 or an input-role inference.
    std::int32_t actualRacerState40;
};
class OriginalGcIntroExitOwner final {
    unsigned phase_=0;std::size_t nextRacer_=0;
    bool begun_=false,completed_=false;
    std::uint64_t generation_=0,controllerKey_=0;
    OriginalRaceViewportCall viewportCall_;
public:
    bool complete() const noexcept {return completed_;}
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
        if(!b.controller.bound||!b.start7C.prepared||!b.start7C.active||!b.start7C.complete240||
           b.introChild.start7C!=&b.start7C||!registry||!registry->bound||
           registry!=b.attachedChild.collection||registry!=b.followChild.collection||registry->sessionGeneration!=b.controller.binding.sessionGeneration||
           registry->sceneKey!=b.controller.binding.sceneKey||b.start7C.sessionGeneration!=registry->sessionGeneration||
           !registry->controllerKey||!registry->registered38||registry->selected34>=registry->count30||bank.count()!=8||
           b.display.sessionGeneration!=registry->sessionGeneration||b.display.controllers48[0]==nullptr||
           b.display.controllers48[0]->controllerKey!=registry->controllerKey||!b.follow64.prepared||
           b.follow64.sessionGeneration!=registry->sessionGeneration||b.follow64.racerKey!=b.controller.binding.racerKey)
            return {S::owner,0x176654};
        if(b.actualLaunch11258!=1||(b.actualPreference11030!=5&&b.actualPreference11030!=0)||
           b.actualRacerState40!=0||b.actualIntroAudio80||b.controller.flags62&0xe0u||
           (b.raceFlags1B0&3u)!=1u||!b.startActions.authenticated())return {S::sourcePath,0x176664};
        if(!begun_){
            if(b.controller.mode44!=7||registry->registered38[registry->selected34]!=b.introChild.start7CKey)
                return {S::owner,0x176648};
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
            case 2:r=requestOriginalCameraExitFade2AED60(b.actualFadeManagerA8,b.actualApplicationMode22C,b.video,b.sharedClock);break;
            case 3:b.raceFlags1B0|=8u;break;
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
                b.follow64.distanceOffset58=b.follow64.yawOffset5C=b.follow64.pitchOffset60=b.follow64.fovOffset64=0;
                break;
            case 6:r=beginOriginalRaceCountdown1AFD80(b.countdown,b.actualManager,b.actualApplication33C,
                    b.actualLaunch11258,b.startActions);break;
            case 7:r=viewportCall_.advance(false,b.display,b.scene,renderer,d);break;
            case 8:revealOriginalRaceUi199770(b.actualUiFlags1E4);break;
            case 9:b.controller.cooldown8C=1;break;
            case 10:b.controller.flags63Low5|=1u;break;
            case 11:
                //Actual80==null and racer40==0,mode44==0/5:2924A0 and
                //176728..1767C4's mode correction both genuinely skip.
                completed_=true;return {S::complete,0x1767c4};
            default:return {S::owner,0x176664};
            }
            if(r.status!=S::complete)return r;++phase_;
        }
    }
};
//The reached exit contains no course switch. Both authenticated GC and GA
//have the same absent321-action list; the LIVE completed procedural child,
//null80 and actual configuration predicates above remain mandatory.
using OriginalGcGaIntroExitOwner=OriginalGcIntroExitOwner;
using OriginalGcGaIntroExitBindings=OriginalGcIntroExitBindings;
}
