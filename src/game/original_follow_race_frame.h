#pragma once
#include "original_attached_race_frame.h"
#include "original_camera_query_owner.h"

namespace rr::game {
enum class OriginalFollowRaceFrameStatus : std::uint8_t {
    complete,owner,requiresObjectChildren,requiresOrdinaryScene,control,pose,sceneTick,visibility
};
struct OriginalFollowRaceFrameResult {
    OriginalFollowRaceFrameStatus status=OriginalFollowRaceFrameStatus::owner;
    RaceCameraControlReceipt control{};
    OriginalFollowResult pose{};
    OriginalFollowCollisionResult collision{};
    OriginalCameraSceneTickStatus tick=OriginalCameraSceneTickStatus::owner;
    LiveZoneResult50EN visibility{};
    bool controlCommitted=false,poseCommitted=false,sceneTickCommitted=false;
};
//175D70 ordinary mode0 prefix, then the ACTUAL selected17A050 child,
//115CB0/49F numeric scene,114AE0 and114C44. Both earlier query effects and
//completed stages remain committed on a later error: caller must stop rather
//than replay the frame. No substitute collision result or racer history exists.
inline OriginalFollowRaceFrameResult advanceOriginalStableFollowRaceFrame(
    OriginalRaceCameraOwner& controller,OriginalFollowCamera& follow,
    OriginalAttachedCamera& attached,const OriginalFollowChildRegistry& children,
    const OriginalCameraObjectChildHeads& objectChildren,const StableRaceCameraFrame& frame,
    const OriginalFollowSubject& subject,const SceneGraph49P& graph,
    const OriginalFollowCollisionSources& sources,OriginalCameraQueryPairOwner& queries,
    CoupledNoiseContext49M& random,GameplayBlurPending& blur,CameraSceneState49F& scene,
    TVCameraUpdateOutput49F& pose,CameraViewOutput49F& view,OriginalCameraSceneTick& tick,
    ResetVisibilityWorld50EY& visibility,OriginalCameraSceneOwner& spatial,
    const OriginalFollowVisibilityZones& zones,OriginalFollowSceneBinding& sceneBinding,
    LiveZoneWorkspace50EN& workspace,std::uint8_t rootFlags228,std::uint8_t sceneFlags29) noexcept {
    OriginalFollowRaceFrameResult r;
    if(!children.collection||!children.collection->bound)return r;
    const auto& collection=*children.collection;const auto& b=controller.binding;
    if(!controller.bound||controller.mode44!=0||!follow.prepared||!follow.active||!attached.prepared||
       collection.sessionGeneration!=b.sessionGeneration||collection.sceneKey!=b.sceneKey||
       !collection.controllerKey||objectChildren.controllerKey!=collection.controllerKey||
       children.follow64!=&follow||!children.follow64Key||!collection.registered38||
       !collection.count30||collection.count30>64||collection.selected34>=collection.count30||
       collection.registered38[collection.selected34]!=children.follow64Key||
       follow.sessionGeneration!=b.sessionGeneration||follow.racerKey!=b.racerKey||follow.mainBodyKey!=b.mainBodyKey||
       attached.binding.sessionGeneration!=b.sessionGeneration||attached.binding.racerKey!=b.racerKey||
       attached.binding.mainBodyKey!=b.mainBodyKey||frame.sessionGeneration!=b.sessionGeneration||
       frame.racerKey!=b.racerKey||frame.mainBodyKey!=b.mainBodyKey||
       subject.sessionGeneration!=b.sessionGeneration||subject.racerKey!=b.racerKey||
       subject.mainBodyKey!=b.mainBodyKey||subject.containerKey!=follow.containerKey||
       objectChildren.sessionGeneration!=b.sessionGeneration||objectChildren.sceneKey!=b.sceneKey||
       spatial.sessionGeneration!=b.sessionGeneration||spatial.sceneKey!=b.sceneKey||!spatial.registration34.attached||
       !tick.bound||tick.sessionGeneration!=b.sessionGeneration||tick.sceneKey!=b.sceneKey||
       !graph.nodes||graph.count>maxSceneNodes49P||sources.graph!=&graph||!sources.history||
       !zones.keys4C0||zones.count!=visibility.lifecycle.zoneCount)return r;
    if(!objectChildren.boundedChildTree()){
        r.status=OriginalFollowRaceFrameStatus::requiresObjectChildren;return r;}
    if((rootFlags228&4u)||(sceneFlags29&4u)){
        r.status=OriginalFollowRaceFrameStatus::requiresOrdinaryScene;return r;}
    if(zones.count>std::numeric_limits<std::size_t>::max()/sizeof(std::uint64_t))return r;
    const OriginalCameraSceneSpan writes[]={{&controller,sizeof controller},{&follow,sizeof follow},
        {&attached,sizeof attached},{graph.nodes,graph.count*sizeof(SceneNode49P)},
        {&random,sizeof random},{&blur,sizeof blur},{&scene,sizeof scene},{&pose,sizeof pose},
        {&view,sizeof view},{&tick,sizeof tick},{&sceneBinding,sizeof sceneBinding}};
    const OriginalCameraSceneSpan reads[]={{&children,sizeof children},{&collection,sizeof collection},
        {collection.registered38,collection.count30*sizeof(std::uint64_t)},
        {&objectChildren,sizeof objectChildren},{&frame,sizeof frame},{&subject,sizeof subject},
        {&graph,sizeof graph},{&zones,sizeof zones},{zones.keys4C0,zones.count*sizeof(std::uint64_t)},
        {&sources,sizeof sources},{sources.history,sizeof(*sources.history)}};
    if(!admitOriginalCameraSceneStorage(visibility,spatial,workspace,writes,11,reads,11))return r;
    OriginalFollowSceneBinding current;
    if(!readOriginalFollowSceneBinding(spatial,zones,current)||
       current.sceneKey!=sceneBinding.sceneKey||current.registrationKey!=sceneBinding.registrationKey||
       current.parentZoneKey!=sceneBinding.parentZoneKey||current.parentMethod!=sceneBinding.parentMethod||
       std::memcmp(&current.position180,&sceneBinding.position180,sizeof current.position180)||
       std::memcmp(&current.registrationPosition20,&sceneBinding.registrationPosition20,sizeof current.registrationPosition20))return r;
    OriginalCameraShakeChannels shake{follow.shakeScale174,follow.shakeAmplitude170,
        attached.shakeScale2C,attached.shakeAmplitude28};
    r.status=OriginalFollowRaceFrameStatus::control;
    r.control=advanceStableOriginalRaceCamera(controller,shake,frame,blur);
    if(r.control.result!=RaceCameraControlResult::advanced_needs_follow_pose)return r;
    follow.shakeScale174=shake.followMagnitude174;follow.shakeAmplitude170=shake.followFactor170;
    attached.shakeScale2C=shake.attachedMagnitude2C;attached.shakeAmplitude28=shake.attachedFactor28;
    r.controlCommitted=true;r.status=OriginalFollowRaceFrameStatus::pose;
    r.pose=advanceOriginalFollowCameraView(follow,subject,frame.elapsed,frame.raceFlags1B0,sources,queries,
        random,pose,scene,view,r.collision,rootFlags228,sceneFlags29);
    if(r.pose.status!=OriginalFollowStatus::ok)return r;
    r.poseCommitted=true;r.status=OriginalFollowRaceFrameStatus::sceneTick;
    r.tick=advanceOriginalCameraSceneTick114AE0(tick,b.sessionGeneration,b.sceneKey,
        view.position,scene.look,sceneFlags29);
    if(r.tick!=OriginalCameraSceneTickStatus::needsMatrixVisibilityAndChildren)return r;
    r.sceneTickCommitted=true;r.status=OriginalFollowRaceFrameStatus::visibility;
    const SpringVector4_48N point{{view.position.x,view.position.y,view.position.z,view.position.w}};
    r.visibility=updateOriginalCameraScene114C44(visibility,spatial,point,workspace);
    if(r.visibility.status!=LiveZoneStatus50EN::Complete||!readOriginalFollowSceneBinding(spatial,zones,sceneBinding))return r;
    r.status=OriginalFollowRaceFrameStatus::complete;return r;
}
}
