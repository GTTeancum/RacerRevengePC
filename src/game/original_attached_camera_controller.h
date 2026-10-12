#pragma once
#include "original_attached_camera.h"
#include "original_follow_controller.h"

namespace rr::game {
struct OriginalAttachedChildRegistry {
    OriginalAttachedCamera* attached6C=nullptr;
    std::uint64_t attached6CKey=0;
    OriginalCameraChildRegistry* collection=nullptr;
};
//Actual177AB0 mode5 branch1783FC..178498. This does not choose the preference:
//the caller supplies the real configuration178700 mode, whose fresh value5
//comes from1864B0. Same-mode5 returns without activating/rebinding the child.
inline OriginalFollowTransitionResult transitionOriginalAttached177AB0(
    OriginalRaceCameraOwner& controller,OriginalAttachedCamera& attached,
    OriginalFollowSceneBinding& scene,OriginalAttachedChildRegistry& children,
    ResetVisibilityWorld50EY& visibility,OriginalCameraSceneOwner& spatial,
    const OriginalFollowVisibilityZones& zones,LiveZoneWorkspace50EN& workspace,
    std::int32_t requestedMode,const OriginalFollowSceneTransitionInput& in) noexcept {
    OriginalFollowTransitionResult result;
    if(!children.collection||!children.collection->bound)return result;
    auto& collection=*children.collection;
    if(collection.sessionGeneration!=controller.binding.sessionGeneration||
       collection.sceneKey!=controller.binding.sceneKey||!collection.controllerKey)return result;
    if(requestedMode!=5){result.status=OriginalFollowControllerStatus::requiresOtherMode;return result;}
    const auto& b=attached.binding;
    if(!controller.bound||!attached.prepared||controller.binding.sessionGeneration!=b.sessionGeneration||
       controller.binding.racerKey!=b.racerKey||controller.binding.mainBodyKey!=b.mainBodyKey||
       scene.sceneKey!=controller.binding.sceneKey)return result;
    //177B00!=0 exits before any source scene/child/input reads.
    if(controller.mode44==5){result.status=OriginalFollowControllerStatus::complete;result.sourcePC=0x17864cu;return result;}
    if(controller.flags62&0x60u){result.status=OriginalFollowControllerStatus::requiresModeRestore;return result;}
    if(children.attached6C!=&attached||!children.attached6CKey||!collection.registered38||
       !collection.count30||collection.count30>64||collection.selected34>=collection.count30||
       !zones.keys4C0||zones.count!=visibility.lifecycle.zoneCount||
       spatial.sceneKey!=scene.sceneKey||spatial.sessionGeneration!=b.sessionGeneration||
       !spatial.registration34.attached||!spatial.parent38||spatial.parent38>zones.count||
       spatial.allocationSerial!=scene.registrationKey||scene.parentMethod!=2||
       scene.parentZoneKey!=zones.keys4C0[spatial.parent38-1u]||
       in.sessionGeneration!=b.sessionGeneration||in.racerKey!=b.racerKey||
       in.mainBodyKey!=b.mainBodyKey||in.sceneKey!=scene.sceneKey||!in.racerZoneKey)return result;
    const auto& a=spatial.position180;const auto& s=spatial.registration34.bounds.center;
    const OriginalFollowVector prior180{a.v[0],a.v[1],a.v[2],a.v[3]},prior20{s.v[0],s.v[1],s.v[2],s.v[3]};
    if(std::memcmp(&prior180,&scene.position180,sizeof prior180)||
       std::memcmp(&prior20,&scene.registrationPosition20,sizeof prior20)||
       zones.count>std::numeric_limits<std::size_t>::max()/sizeof(std::uint64_t))return result;
    const OriginalCameraSceneSpan writes[]={{&controller,sizeof controller},{&attached,sizeof attached},
        {&scene,sizeof scene},{&children,sizeof children},{&collection,sizeof collection}};
    const OriginalCameraSceneSpan reads[]={{&zones,sizeof zones},{zones.keys4C0,zones.count*sizeof(std::uint64_t)},
        {collection.registered38,collection.count30*sizeof(std::uint64_t)},{&in,sizeof in}};
    if(!admitOriginalCameraSceneStorage(visibility,spatial,workspace,writes,5,reads,4))return result;
    std::size_t selected=collection.count30;
    for(std::size_t i=0;i<collection.count30;++i){
        if(!collection.registered38[i])return result;
        if(selected==collection.count30&&collection.registered38[i]==children.attached6CKey)selected=i;
    }
    if(selected==collection.count30)return result;
    CollectionRef50BX zone=0;
    for(std::size_t i=0;i<zones.count;++i){
        if(!zones.keys4C0[i])return result;
        for(std::size_t j=0;j<i;++j)if(zones.keys4C0[i]==zones.keys4C0[j])return result;
        if(zones.keys4C0[i]==in.racerZoneKey)zone=static_cast<CollectionRef50BX>(i+1u);
    }
    if(!zone)return result;
    auto next=attached;
    if(activateOriginalAttachedCamera(next).status!=OriginalAttachedStatus::ok)return result;
    const auto& p=in.mainBodyLocalF0;const auto& saved=in.racerRegistrationPosition20;
    const SpringVector4_48N point{{p.x,p.y,p.z,1}},oldCenter{{saved.x,saved.y,saved.z,saved.w}};
    result.status=OriginalFollowControllerStatus::needsSceneRegistration;result.sourcePC=0x177c50u;
    result.registration=rebindOriginalCameraScene177AB0(visibility,spatial,zone,point,oldCenter,workspace);
    if(result.registration.status!=LiveZoneStatus50EN::Complete)return result;
    //Source13E480 ignores prior-camera activation packet.115E60 chooses the
    //first actual registered6C match.178498 copies saved20;178648 writes5.
    attached=next;collection.selected34=selected;controller.mode44=5;
    scene.position180={p.x,p.y,p.z,1};scene.registrationPosition20=saved;
    scene.parentZoneKey=in.racerZoneKey;scene.parentMethod=2;scene.registrationKey=spatial.allocationSerial;
    result.status=OriginalFollowControllerStatus::complete;result.sourcePC=0x178648u;return result;
}
//The body-local/690 source reads are identical for mode0 and mode5; model1B4
//world is intentionally NOT read by this transition helper, only by13DF00.
inline bool readOriginalAttachedSceneTransitionInput(const OriginalAttachedBinding& b,
    std::uint64_t sceneKey,const SceneGraph49P& graph,const ZoneLifecycle50CL& collision,
    const LiveZoneMembership50EN& racer690,const OriginalFollowVisibilityZones& visibility,
    OriginalFollowSceneTransitionInput& out) noexcept {
    const OriginalFollowIdentity id{b.sessionGeneration,b.racerKey,b.mainBodyKey,b.model1B4Key,b.pod60};
    return readOriginalFollowSceneTransitionInput(id,sceneKey,graph,b.body288,collision,racer690,visibility,out);
}
}
