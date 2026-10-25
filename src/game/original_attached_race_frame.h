#pragma once
#include "original_attached_camera_controller.h"
#include "original_camera_scene_tick.h"
#include "original_wreck_components50la.h"

namespace rr::game {
//Actual141300 object-child heads, not the115DE0 camera collection38. The
//closed ordinary composition admits empty lists, for which both reached
//141ED0 calls are real no-ops. Nonempty lists require their actual dispatch.
struct OriginalCameraObjectChildHeads {
    std::uint64_t sessionGeneration=0,controllerKey=0,sceneKey=0;
    std::uint64_t controllerChild18=0,sceneChild18=0;
    const OriginalWreckComponent50LA* actualController=nullptr;
    const OriginalWreckComponent50LA* actualIntroAudio=nullptr;
    bool boundedChildTree()const noexcept {
        if(sceneChild18)return false;
        if(!controllerChild18)return !actualController&&!actualIntroAudio;
        const auto* c=actualController;const auto* e=actualIntroAudio;
        return c&&e&&c->reference00&&e->reference00&&c->firstChild18==e&&e->parent1C==c&&
            !e->previous10&&!e->next14&&!e->firstChild18&&!e->descendants24&&
            e->kind==OriginalWreckComponentKind50LA::wattoEmitter2921C0&&
            controllerChild18==static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(e));
    }
};
enum class OriginalAttachedRaceFrameStatus : std::uint8_t {
    complete,owner,requiresObjectChildren,requiresOrdinaryScene,
    control,pose,sceneTick,visibility
};
struct OriginalAttachedRaceFrameResult {
    OriginalAttachedRaceFrameStatus status=OriginalAttachedRaceFrameStatus::owner;
    RaceCameraControlReceipt control{};
    OriginalAttachedResult pose{};
    OriginalCameraSceneTickStatus tick=OriginalCameraSceneTickStatus::owner;
    LiveZoneResult50EN visibility{};
    bool controlCommitted=false,poseCommitted=false,sceneTickCommitted=false;
};
//Complete stable mode5 ordinary controller prefix -> actual6C13DF00 ->
//115CB0/49F scene composition ->114AE0 history/counters ->114C44 visibility
//update, for the explicitly empty source object-child lists. Call at the real
//race-child cut, AFTER all ordered racer prefixes and BEFORE1A8390 physics
//schedule. This does not choose mode5 or perform the prior intro transition.
//The earlier race-child scene114AE0 call is separately scheduled and MUST NOT
//be elided: it also updates histories/counters/registration before this call.
//
//Existing real child shake values are projected through the source control
//leaf then committed back to those exact owners. No new timer, event, Player
//field or RNG exists. Prefix and later pose/scene successes remain committed
//if a subsequent native stage rejects; the returned stage tells the owner
//where to stop. A rejection must not cause this whole frame to be retried.
inline OriginalAttachedRaceFrameResult advanceOriginalStableAttachedRaceFrame(
    OriginalRaceCameraOwner& controller,OriginalFollowCamera& follow,
    OriginalAttachedCamera& attached,const OriginalAttachedChildRegistry& children,
    const OriginalCameraObjectChildHeads& objectChildren,const StableRaceCameraFrame& frame,
    const OriginalAttachedFrame& actualBody,const SceneGraph49P& graph,
    OriginalAttachedGlobals& globals,CoupledNoiseContext49M& random,
    GameplayBlurPending& blur,CameraSceneState49F& scene,TVCameraUpdateOutput49F& pose,
    CameraViewOutput49F& view,OriginalCameraSceneTick& tick,
    ResetVisibilityWorld50EY& visibility,OriginalCameraSceneOwner& spatial,
    const OriginalFollowVisibilityZones& zones,OriginalFollowSceneBinding& sceneBinding,
    LiveZoneWorkspace50EN& workspace,std::uint8_t rootFlags228,std::uint8_t sceneFlags29) noexcept {
    OriginalAttachedRaceFrameResult r;
    if(!children.collection||!children.collection->bound)return r;
    const auto& collection=*children.collection;
    const auto& b=controller.binding;const auto& a=attached.binding;
    if(collection.sessionGeneration!=b.sessionGeneration||collection.sceneKey!=b.sceneKey||
       !collection.controllerKey||objectChildren.controllerKey!=collection.controllerKey||
       !controller.bound||controller.mode44!=5||!attached.prepared||!attached.active||
       !follow.prepared||follow.sessionGeneration!=b.sessionGeneration||follow.racerKey!=b.racerKey||
       follow.mainBodyKey!=b.mainBodyKey||a.sessionGeneration!=b.sessionGeneration||
       a.racerKey!=b.racerKey||a.mainBodyKey!=b.mainBodyKey||
       frame.sessionGeneration!=b.sessionGeneration||frame.racerKey!=b.racerKey||frame.mainBodyKey!=b.mainBodyKey||
       actualBody.sessionGeneration!=b.sessionGeneration||actualBody.racerKey!=b.racerKey||
       actualBody.mainBodyKey!=b.mainBodyKey||actualBody.model1B4Key!=a.model1B4Key||
       children.attached6C!=&attached||!children.attached6CKey||!collection.registered38||
       !collection.count30||collection.count30>64||collection.selected34>=collection.count30||
       collection.registered38[collection.selected34]!=children.attached6CKey||
       objectChildren.sessionGeneration!=b.sessionGeneration||!objectChildren.controllerKey||
       objectChildren.sceneKey!=b.sceneKey||spatial.sessionGeneration!=b.sessionGeneration||
       spatial.sceneKey!=b.sceneKey||!spatial.registration34.attached||
       !tick.bound||tick.sessionGeneration!=b.sessionGeneration||tick.sceneKey!=b.sceneKey||
       !graph.nodes||graph.count>maxSceneNodes49P||!zones.keys4C0||
       zones.count!=visibility.lifecycle.zoneCount)return r;
    if(!objectChildren.boundedChildTree()){
        r.status=OriginalAttachedRaceFrameStatus::requiresObjectChildren;return r;}
    if((rootFlags228&4u)||(sceneFlags29&4u)){
        r.status=OriginalAttachedRaceFrameStatus::requiresOrdinaryScene;return r;}
    if(zones.count>std::numeric_limits<std::size_t>::max()/sizeof(std::uint64_t))return r;
    const OriginalCameraSceneSpan writes[]={{&controller,sizeof controller},{&follow,sizeof follow},
        {&attached,sizeof attached},{graph.nodes,graph.count*sizeof(SceneNode49P)},
        {&globals,sizeof globals},{&random,sizeof random},{&blur,sizeof blur},{&scene,sizeof scene},
        {&pose,sizeof pose},{&view,sizeof view},{&tick,sizeof tick},{&sceneBinding,sizeof sceneBinding}};
    const OriginalCameraSceneSpan reads[]={{&children,sizeof children},{&collection,sizeof collection},
        {collection.registered38,collection.count30*sizeof(std::uint64_t)},
        {&objectChildren,sizeof objectChildren},{&frame,sizeof frame},{&actualBody,sizeof actualBody},
        {&graph,sizeof graph},{&zones,sizeof zones},{zones.keys4C0,zones.count*sizeof(std::uint64_t)}};
    if(!admitOriginalCameraSceneStorage(visibility,spatial,workspace,writes,12,reads,9))return r;
    OriginalFollowSceneBinding current;
    if(!readOriginalFollowSceneBinding(spatial,zones,current)||
       current.sceneKey!=sceneBinding.sceneKey||current.registrationKey!=sceneBinding.registrationKey||
       current.parentZoneKey!=sceneBinding.parentZoneKey||current.parentMethod!=sceneBinding.parentMethod||
       std::memcmp(&current.position180,&sceneBinding.position180,sizeof current.position180)||
       std::memcmp(&current.registrationPosition20,&sceneBinding.registrationPosition20,
           sizeof current.registrationPosition20))return r;
    OriginalCameraShakeChannels channels{follow.shakeScale174,follow.shakeAmplitude170,
        attached.shakeScale2C,attached.shakeAmplitude28};
    r.status=OriginalAttachedRaceFrameStatus::control;
    r.control=advanceStableOriginalRaceCamera(controller,channels,frame,blur);
    if(r.control.result!=RaceCameraControlResult::advanced_needs_follow_pose)return r;
    follow.shakeScale174=channels.followMagnitude174;follow.shakeAmplitude170=channels.followFactor170;
    attached.shakeScale2C=channels.attachedMagnitude2C;attached.shakeAmplitude28=channels.attachedFactor28;
    r.controlCommitted=true;
    auto body=actualBody;body.elapsed=frame.elapsed;
    r.status=OriginalAttachedRaceFrameStatus::pose;
    r.pose=advanceOriginalAttachedCameraView(attached,body,graph,globals,random,pose,scene,view,
        rootFlags228,sceneFlags29);
    if(r.pose.status!=OriginalAttachedStatus::ok)return r;
    r.poseCommitted=true;
    //49F stages the source pose and matrix numeric operations together. The
    //intervening114AE0 history/counter writes have no matrix/RNG dependency;
    //they publish here before the first external spatial operation. There is
    //no draw/child callback between these numerical stages.
    r.status=OriginalAttachedRaceFrameStatus::sceneTick;
    r.tick=advanceOriginalCameraSceneTick114AE0(tick,b.sessionGeneration,b.sceneKey,
        view.position,scene.look,sceneFlags29);
    if(r.tick!=OriginalCameraSceneTickStatus::needsMatrixVisibilityAndChildren)return r;
    r.sceneTickCommitted=true;
    const SpringVector4_48N point{{view.position.x,view.position.y,view.position.z,view.position.w}};
    r.status=OriginalAttachedRaceFrameStatus::visibility;
    r.visibility=updateOriginalCameraScene114C44(visibility,spatial,point,workspace);
    if(r.visibility.status!=LiveZoneStatus50EN::Complete)return r;
    if(!readOriginalFollowSceneBinding(spatial,zones,sceneBinding))return r;
    r.status=OriginalAttachedRaceFrameStatus::complete;return r;
}
}
