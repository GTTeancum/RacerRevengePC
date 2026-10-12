#pragma once
#include "original_follow_camera.h"
#include "original_race_camera_owner.h"
#include "original_camera_scene_owner.h"
#include "original_camera_child_registry.h"
#include <cstring>
#include <limits>

namespace rr::game {
// Numeric scene projection at177AB0. The concrete transaction below owns its
// real1148D0/114770 visibility registration; these keys are not guest pointers.
struct OriginalFollowSceneBinding {
    std::uint64_t sceneKey=0,parentZoneKey=0,registrationKey=0;
    OriginalFollowVector position180{0,0,0,1};
    OriginalFollowVector registrationPosition20{0,0,0,1};
    std::int32_t parentMethod=0;
};
struct OriginalFollowSceneTransitionInput {
    std::uint64_t sessionGeneration=0,racerKey=0,mainBodyKey=0,sceneKey=0;
    //178180 obtains racer340->288 LOCAL F0, not racer194 world130.
    OriginalFollowVector mainBodyLocalF0{0,0,0,1};
    //race74->4C0[racer690->E0->4->48], and racer690->E0+20.
    std::uint64_t racerZoneKey=0;
    OriginalFollowVector racerRegistrationPosition20{0,0,0,1};
};
enum class OriginalFollowControllerStatus : std::uint8_t {
    complete,needsSceneRegistration,owner,requiresModeRestore,requiresOtherMode
};
struct OriginalFollowControllerPlan {
    OriginalRaceCameraOwner controllerAfter{};
    OriginalFollowCamera followAfter{};
    OriginalFollowSceneBinding sceneAfter{};
    std::uint64_t previousRegistrationKey=0,baseFollowRevision=0;
    std::int32_t oldMode44=0;
    bool pending=false;
};
// This is a genuine scene-owner receipt, not an injected camera pose. Produce
// it only after1148D0 removes the old link and114770/2809C0 bind the requested
// zone using real type2 spatial methods and all reached neighbor registrations.
struct OriginalFollowSceneRegistrationReceipt {
    std::uint64_t sceneKey=0,removedRegistrationKey=0,newRegistrationKey=0,parentZoneKey=0;
    bool removedOldAndAttachedType2=false;
};
// Request mode is explicit. This never chooses Follow in place of intro.
// Same-mode0 returns/changes preset immediately exactly as177B00..177B44.
// Different-mode0 produces the numeric plan in source order and stops at the
// real114770 spatial-registration boundary. Source flags62 bits5/6 restoration
// recursion and requests for other modes remain explicit controller paths.
inline OriginalFollowControllerStatus planOriginalFollowTransition177AB0(
    const OriginalRaceCameraOwner& controller,const OriginalFollowCamera& follow,
    const OriginalFollowSceneBinding& scene,std::int32_t requestedMode,
    const std::int32_t* requestedPresetId,const OriginalFollowSceneTransitionInput& in,
    OriginalFollowControllerPlan& out) noexcept {
    if(requestedMode!=0)return OriginalFollowControllerStatus::requiresOtherMode;
    if(!controller.bound||!follow.prepared||!follow.bank.records||follow.current100>=follow.bank.count||
        controller.binding.sessionGeneration!=follow.sessionGeneration||controller.binding.racerKey!=follow.racerKey||
        controller.binding.mainBodyKey!=follow.mainBodyKey||scene.sceneKey!=controller.binding.sceneKey)
        return OriginalFollowControllerStatus::owner;
    OriginalFollowControllerPlan p;p.controllerAfter=controller;p.followAfter=follow;p.sceneAfter=scene;
    p.oldMode44=controller.mode44;p.baseFollowRevision=follow.revision;
    if(controller.mode44==0){
        if(requestedPresetId&&*requestedPresetId!=follow.bank.records[follow.current100].id00){
            if(selectOriginalFollowPreset(p.followAfter,*requestedPresetId).status!=OriginalFollowStatus::ok)
                return OriginalFollowControllerStatus::owner;
        }
        out=p;return OriginalFollowControllerStatus::complete;
    }
    if(controller.flags62&0x60u)return OriginalFollowControllerStatus::requiresModeRestore;
    if(in.sessionGeneration!=follow.sessionGeneration||in.racerKey!=follow.racerKey||
        in.mainBodyKey!=follow.mainBodyKey||in.sceneKey!=scene.sceneKey||!in.racerZoneKey||!scene.registrationKey||!scene.parentZoneKey)
        return OriginalFollowControllerStatus::owner;
    //Source177C48 removes the actual old scene registration before activation.
    p.previousRegistrationKey=scene.registrationKey;
    if(activateOriginalFollowCamera(p.followAfter).status!=OriginalFollowStatus::ok)
        return OriginalFollowControllerStatus::owner;
    //115E60 selects the already-registered Follow64 child. This plan's commit
    //requires the caller to retain that actual controller child registration.
    if(requestedPresetId&&*requestedPresetId!=follow.bank.records[follow.current100].id00){
        if(selectOriginalFollowPreset(p.followAfter,*requestedPresetId).status!=OriginalFollowStatus::ok)
            return OriginalFollowControllerStatus::owner;
    }
    p.sceneAfter.position180=in.mainBodyLocalF0;p.sceneAfter.position180.w=1;
    p.sceneAfter.parentZoneKey=in.racerZoneKey;p.sceneAfter.parentMethod=2;
    //114770 passes position180 first; only AFTER its full2809C0 call returns
    //1781F8 overwrites registration+20 with this actual racer's parent record.
    p.sceneAfter.registrationPosition20=in.racerRegistrationPosition20;
    p.sceneAfter.registrationKey=0;p.controllerAfter.mode44=0;p.pending=true;
    out=p;return OriginalFollowControllerStatus::needsSceneRegistration;
}
inline OriginalFollowControllerStatus commitOriginalFollowTransition177AB0(
    OriginalRaceCameraOwner& controller,OriginalFollowCamera& follow,OriginalFollowSceneBinding& scene,
    OriginalFollowControllerPlan& plan,const OriginalFollowSceneRegistrationReceipt& receipt) noexcept {
    if(!plan.pending||follow.revision!=plan.baseFollowRevision||controller.mode44!=plan.oldMode44||
        follow.sessionGeneration!=plan.followAfter.sessionGeneration||follow.racerKey!=plan.followAfter.racerKey||
        scene.sceneKey!=plan.sceneAfter.sceneKey||scene.registrationKey!=plan.previousRegistrationKey||
        !receipt.removedOldAndAttachedType2||receipt.sceneKey!=scene.sceneKey||
        receipt.removedRegistrationKey!=plan.previousRegistrationKey||!receipt.newRegistrationKey||
        receipt.parentZoneKey!=plan.sceneAfter.parentZoneKey)return OriginalFollowControllerStatus::owner;
    plan.sceneAfter.registrationKey=receipt.newRegistrationKey;
    controller=plan.controllerAfter;follow=plan.followAfter;scene=plan.sceneAfter;plan.pending=false;
    return OriginalFollowControllerStatus::complete;
}
// The complete same-mode plan has no spatial-registration effect and cannot
// activate an inactive Follow. This explicit commit keeps planning read-only.
inline OriginalFollowControllerStatus commitOriginalFollowSameMode177AB0(
    OriginalRaceCameraOwner& controller,OriginalFollowCamera& follow,
    const OriginalFollowControllerPlan& plan) noexcept {
    if(plan.pending||controller.mode44!=0||plan.oldMode44!=0||follow.revision!=plan.baseFollowRevision||
        follow.sessionGeneration!=plan.followAfter.sessionGeneration||follow.racerKey!=plan.followAfter.racerKey)
        return OriginalFollowControllerStatus::owner;
    follow=plan.followAfter;return OriginalFollowControllerStatus::complete;
}
}

namespace rr::game {
// Actual controller30/34/38 and child64 binding produced by1752D0 registration.
// The array contains stable native child identities in original insertion order.
struct OriginalFollowChildRegistry {
    OriginalFollowCamera* follow64=nullptr;
    std::uint64_t follow64Key=0;
    OriginalCameraChildRegistry* collection=nullptr;
};
struct OriginalFollowVisibilityZones {
    // Same source ordinal order as scene4C0; keys are actual bound zone owners.
    const std::uint64_t* keys4C0=nullptr;std::size_t count=0;
};
// Concrete reads for178180..1781F8. The graph node is the genuine340->288
//node; no refresh/world pose is substituted for its local translation. The
//collision690 primary-zone ordinal is mapped to the corresponding4C0 owner.
inline bool readOriginalFollowSceneTransitionInput(const OriginalFollowIdentity& identity,
    std::uint64_t sceneKey,const SceneGraph49P& graph,SceneNodeIndex49P body288,
    const ZoneLifecycle50CL& collision,const LiveZoneMembership50EN& racer690,
    const OriginalFollowVisibilityZones& visibility,OriginalFollowSceneTransitionInput& out) noexcept {
    const auto node=static_cast<std::int32_t>(body288);
    if(!identity.sessionGeneration||!identity.racerKey||!identity.mainBodyKey||!sceneKey||
       !graph.nodes||node<0||static_cast<std::uint32_t>(node)>=graph.count||!validateSceneGraph49P(graph)||
       !racer690.attached||!racer690.object||racer690.object>collision.objectCount||
       !collision.storage.registrations||collision.storage.registrationCapacity<collision.objectCount||
       !racer690.storage.capacity||!racer690.storage.links||!racer690.storage.entries||
       !visibility.keys4C0||visibility.count!=collision.zoneCount)return false;
    const auto zone=racer690.storage.links[0].zone;
    if(!zone||zone>visibility.count||!visibility.keys4C0[zone-1u])return false;
    const auto& record=racer690.storage.entries[0].registration;
    const auto& actual=collision.storage.registrations[racer690.object-1u];
    if(!record.owner||record.owner!=actual.owner||record.slot!=actual.slot||
       record.encodedObject!=actual.encodedObject||record.encodedObject!=(racer690.object<<2u))return false;
    const auto& local=graph.nodes[static_cast<std::uint32_t>(node)].localC0.v;
    const auto& saved=racer690.bounds.center;
    out={identity.sessionGeneration,identity.racerKey,identity.mainBodyKey,sceneKey,
        {local[12],local[13],local[14],1},visibility.keys4C0[zone-1u],
        {saved.v[0],saved.v[1],saved.v[2],saved.v[3]}};return true;
}
struct OriginalFollowTransitionResult {
    OriginalFollowControllerStatus status=OriginalFollowControllerStatus::owner;
    std::uint32_t sourcePC=0;
    LiveZoneResult50EN registration{};
};
// Complete explicit mode0 request. No externally supplied success receipt:
// actual source registration executes here before publishing the numeric plan.
// Child64 must already be present in the real1752D0 child registry. Intro7
// completion/config178700 remains the source of this request, not this helper.
inline OriginalFollowTransitionResult transitionOriginalFollow177AB0(
    OriginalRaceCameraOwner& controller,OriginalFollowCamera& follow,
    OriginalFollowSceneBinding& scene,OriginalFollowChildRegistry& children,
    ResetVisibilityWorld50EY& visibility,OriginalCameraSceneOwner& spatial,
    const OriginalFollowVisibilityZones& zones,LiveZoneWorkspace50EN& workspace,
    std::int32_t requestedMode,const std::int32_t* preset,
    const OriginalFollowSceneTransitionInput& in) noexcept {
    OriginalFollowTransitionResult result;
    if(!children.collection||!children.collection->bound)return result;
    auto& collection=*children.collection;
    if(collection.sessionGeneration!=controller.binding.sessionGeneration||
       collection.sceneKey!=controller.binding.sceneKey||!collection.controllerKey)return result;
    if(children.follow64!=&follow||!children.follow64Key||!collection.registered38||
       !collection.count30||collection.count30>64||collection.selected34>=collection.count30||
       !zones.keys4C0||zones.count!=visibility.lifecycle.zoneCount||
       spatial.sceneKey!=scene.sceneKey||spatial.sessionGeneration!=follow.sessionGeneration||
       !spatial.registration34.attached||!spatial.parent38||spatial.parent38>zones.count||
       spatial.allocationSerial!=scene.registrationKey||scene.parentMethod!=2||
       scene.parentZoneKey!=zones.keys4C0[spatial.parent38-1u])return result;
    const auto& prior=spatial.registration34.bounds.center;
    const OriginalFollowVector prior20{prior.v[0],prior.v[1],prior.v[2],prior.v[3]};
    const auto& p=spatial.position180;
    const OriginalFollowVector prior180{p.v[0],p.v[1],p.v[2],p.v[3]};
    if(std::memcmp(&prior20,&scene.registrationPosition20,sizeof prior20)||
       std::memcmp(&prior180,&scene.position180,sizeof prior180))return result;
    if(zones.count>std::numeric_limits<std::size_t>::max()/sizeof(std::uint64_t)||
       follow.bank.count>std::numeric_limits<std::size_t>::max()/sizeof(OriginalFollowPreset))return result;
    const OriginalCameraSceneSpan writes[]={{&controller,sizeof controller},{&follow,sizeof follow},
        {&scene,sizeof scene},{&children,sizeof children},{&collection,sizeof collection}};
    const OriginalCameraSceneSpan reads[]={{&zones,sizeof zones},{zones.keys4C0,zones.count*sizeof(std::uint64_t)},
        {collection.registered38,collection.count30*sizeof(std::uint64_t)},{&in,sizeof in},
        {preset,preset?sizeof(*preset):0},{follow.bank.records,follow.bank.count*sizeof(OriginalFollowPreset)}};
    if(!admitOriginalCameraSceneStorage(visibility,spatial,workspace,writes,5,reads,6))return result;
    std::size_t selected=collection.count30;
    for(std::size_t i=0;i<collection.count30;++i){
        if(!collection.registered38[i])return result;
        if(selected==collection.count30&&collection.registered38[i]==children.follow64Key)selected=i;
    }
    if(selected==collection.count30)return result;
    CollectionRef50BX nextZone=0;
    for(std::size_t i=0;i<zones.count;++i){
        if(!zones.keys4C0[i])return result;
        for(std::size_t j=0;j<i;++j)if(zones.keys4C0[i]==zones.keys4C0[j])return result;
        if(zones.keys4C0[i]==in.racerZoneKey)nextZone=static_cast<CollectionRef50BX>(i+1u);
    }
    OriginalFollowControllerPlan plan;
    result.status=planOriginalFollowTransition177AB0(controller,follow,scene,requestedMode,preset,in,plan);
    if(result.status==OriginalFollowControllerStatus::complete){
        result.status=commitOriginalFollowSameMode177AB0(controller,follow,plan);result.sourcePC=0x177b44u;return result;
    }
    if(result.status!=OriginalFollowControllerStatus::needsSceneRegistration)return result;
    if(!nextZone){result.status=OriginalFollowControllerStatus::owner;return result;}
    const auto& pp=plan.sceneAfter.position180;const auto& ss=plan.sceneAfter.registrationPosition20;
    const SpringVector4_48N position{{pp.x,pp.y,pp.z,pp.w}},saved{{ss.x,ss.y,ss.z,ss.w}};
    result.sourcePC=0x177c50u;
    result.registration=rebindOriginalCameraScene177AB0(visibility,spatial,nextZone,position,saved,workspace);
    if(result.registration.status!=LiveZoneStatus50EN::Complete)return result;
    //115E60 searches real registered children and publishes its first match.
    collection.selected34=selected;
    plan.sceneAfter.registrationKey=spatial.allocationSerial;
    controller=plan.controllerAfter;follow=plan.followAfter;scene=plan.sceneAfter;
    result.status=OriginalFollowControllerStatus::complete;result.sourcePC=0x178648u;return result;
}
// Refresh the value projection after actual initial scene attachment or after
// a source114C44 update. It cannot attach or alter the spatial namespace.
inline bool readOriginalFollowSceneBinding(const OriginalCameraSceneOwner& owner,
    const OriginalFollowVisibilityZones& zones,OriginalFollowSceneBinding& out) noexcept {
    if(!owner.sceneKey||!owner.allocationSerial||!owner.registration34.attached||
       !owner.parent38||owner.parent38>zones.count||!zones.keys4C0||!zones.keys4C0[owner.parent38-1u])return false;
    const auto& a=owner.position180;const auto& b=owner.registration34.bounds.center;
    out={owner.sceneKey,zones.keys4C0[owner.parent38-1u],owner.allocationSerial,
        {a.v[0],a.v[1],a.v[2],a.v[3]},{b.v[0],b.v[1],b.v[2],b.v[3]},2};return true;
}
}
