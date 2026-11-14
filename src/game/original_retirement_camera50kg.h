#pragma once
#include "original_attached_camera_controller.h"
#include "original_death_camera50kg.h"
#include <cstring>
#include <limits>

namespace rr::game {
// The actual additional1752D0 child68 registration, in the same collection as
// Follow64, Attached6C and Intro7C. No spare/fake child or private selector.
struct OriginalDeathChildRegistry50KG {
    OriginalDeathCamera50KG* death68=nullptr;
    std::uint64_t death68Key=0;
    OriginalCameraChildRegistry* collection=nullptr;
};
enum class OriginalRetirementCameraStatus50KG : std::uint8_t {
    complete,needsDeathTransition177AB0,needsHudAnimationStop1AD230,needsSelectedPose115CB0,
    owner,sourcePath,numeric,query,visibility,continuation
};
struct OriginalRetirementCameraResult50KG {
    OriginalRetirementCameraStatus50KG status=OriginalRetirementCameraStatus50KG::owner;
    std::uint32_t sourcePC=0;
    bool prefixCommitted=false,sceneRemoved=false,queryAttached=false,
        childActivated=false,childSelected=false,sceneAttached=false;
    OriginalDeathCameraResult50KG death{};
    OriginalDeathCameraQueryResult query{};
    LiveZoneResult50EN visibility{};
};
// Ordinary mode0/5 -> death mode1 branch177AB0/1781FC..1782A4 for an
// distinct query120 with actual source-ordered first-use construction when
// absent, or the retained query when present. Its E0 may be detached.
// No Follow callback is consumed. WORLD and LOCAL positions differ.
// Earlier effects survive rejection; caller must terminate, never replay.
inline OriginalRetirementCameraResult50KG transitionOriginalDeath177AB0(
    OriginalRaceCameraOwner& controller,OriginalDeathCamera50KG& death,
    OriginalFollowSceneBinding& scene,OriginalDeathChildRegistry50KG& children,
    OriginalFollowChildRegistry& followChildren,OriginalAttachedChildRegistry& attachedChildren,
    OriginalDeathCameraQueryOwner& query,const OriginalDeathCameraSubject50KG& subject,
    const ZoneLifecycle50CL& racerCollision,const LiveZoneMembership50EN& racer690,
    ResetVisibilityWorld50EY& visibility,OriginalCameraSceneOwner& spatial,
    const OriginalFollowVisibilityZones& zones,LiveZoneWorkspace50EN& workspace,
    const OriginalFollowSceneTransitionInput& in,
    OriginalDeathCameraQueryColdOwner* cold=nullptr,std::uint64_t tvChildKey=0) noexcept {
    using S=OriginalRetirementCameraStatus50KG;
    OriginalRetirementCameraResult50KG r;
    const auto& b=controller.binding;const auto& identity=death.identity;
    if(!controller.bound||!death.prepared||identity.sessionGeneration!=b.sessionGeneration||
       identity.racerKey!=b.racerKey||identity.mainBodyKey!=b.mainBodyKey||
       scene.sceneKey!=b.sceneKey)return r;
    //177B00 nonzero same-mode return precedes collection and subject reads.
    if(controller.mode44==1){r.status=S::complete;r.sourcePC=0x17864cu;return r;}
    // Mode 6 is the Results-replay TV child (race48+78), reached only with a
    // registered TV key: 177AB0's old-pose export is not consumed by 22D810.
    if(controller.mode44!=0&&controller.mode44!=5&&!(controller.mode44==6&&tvChildKey)){
        r.status=S::sourcePath;r.sourcePC=0x177b48u;return r;
    }
    if(controller.flags62&0x60u){
        if(!followChildren.follow64||!attachedChildren.attached6C)return r;
        const auto restored=restoreOriginalTemporaryLook1775D0(controller,*followChildren.follow64,
            *attachedChildren.attached6C,scene,attachedChildren,visibility,spatial,zones,workspace,in,
            (controller.flags62&0x40u)?0x40u:0x20u);
        if(restored.status!=OriginalFollowControllerStatus::complete){
            r.status=S::sourcePath;r.sourcePC=restored.sourcePC;return r;
        }
    }
    if(!children.collection||!children.collection->bound)return r;
    auto& collection=*children.collection;
    if(children.death68!=&death||!children.death68Key||children.death68Key!=identity.death68Key||
       collection.sessionGeneration!=b.sessionGeneration||collection.sceneKey!=b.sceneKey||
       !collection.controllerKey||!collection.registered38||!collection.count30||collection.count30>64||
       collection.selected34>=collection.count30||followChildren.collection!=&collection||
       attachedChildren.collection!=&collection||!followChildren.follow64||!followChildren.follow64Key||
       !attachedChildren.attached6C||!attachedChildren.attached6CKey||
       (controller.mode44==0&&(!followChildren.follow64->active||
        collection.registered38[collection.selected34]!=followChildren.follow64Key))||
       (controller.mode44==5&&(!attachedChildren.attached6C->active||
        collection.registered38[collection.selected34]!=attachedChildren.attached6CKey))||
       (controller.mode44==6&&collection.registered38[collection.selected34]!=tvChildKey)||
       subject.identity.sessionGeneration!=identity.sessionGeneration||
       subject.identity.racerKey!=identity.racerKey||subject.identity.mainBodyKey!=identity.mainBodyKey||
       subject.identity.death68Key!=identity.death68Key||subject.identity.body288!=identity.body288||
       (subject.racerState40!=3&&subject.racerState40!=4)||!subject.graph||
       !subject.graph->nodes||!validateSceneGraph49P(*subject.graph)||
       in.sessionGeneration!=b.sessionGeneration||in.racerKey!=b.racerKey||
       in.mainBodyKey!=b.mainBodyKey||in.sceneKey!=b.sceneKey||
       !zones.keys4C0||zones.count!=visibility.lifecycle.zoneCount||
       zones.count!=racerCollision.zoneCount||!in.racerZoneKey||
       !racer690.attached||!racer690.storage.links||!racer690.storage.entries||!racer690.storage.capacity||
       !racer690.object||racer690.object>racerCollision.objectCount||
       !racerCollision.storage.registrations||
       racerCollision.storage.registrationCapacity<racerCollision.objectCount||
       query.lifecycle!=&racerCollision||query.owner9C!=identity.death68Key||
       spatial.sessionGeneration!=b.sessionGeneration||spatial.sceneKey!=b.sceneKey||
       !spatial.registration34.attached||!spatial.parent38||spatial.parent38>zones.count||
       spatial.allocationSerial!=scene.registrationKey||scene.parentMethod!=2||
       scene.parentZoneKey!=zones.keys4C0[spatial.parent38-1u])return r;
    const auto node=static_cast<std::int32_t>(identity.body288);
    if(node<0||static_cast<std::uint32_t>(node)>=subject.graph->count)return r;
    const auto zone=racer690.storage.links[0].zone;
    if(!zone||zone>zones.count||zones.keys4C0[zone-1u]!=in.racerZoneKey)return r;
    const auto& registration=racer690.storage.entries[0].registration;
    const auto& actual=racerCollision.storage.registrations[racer690.object-1u];
    if(!registration.owner||registration.owner!=actual.owner||registration.slot!=actual.slot||
       registration.encodedObject!=actual.encodedObject||
       registration.encodedObject!=(racer690.object<<2u))return r;
    const auto& currentPosition=spatial.position180;const auto& currentCenter=spatial.registration34.bounds.center;
    const OriginalFollowVector prior180{currentPosition.v[0],currentPosition.v[1],currentPosition.v[2],currentPosition.v[3]},
        prior20{currentCenter.v[0],currentCenter.v[1],currentCenter.v[2],currentCenter.v[3]};
    if(std::memcmp(&prior180,&scene.position180,sizeof prior180)||
       std::memcmp(&prior20,&scene.registrationPosition20,sizeof prior20))return r;
    const auto& local=subject.graph->nodes[static_cast<std::uint32_t>(node)].localC0.v;
    const OriginalFollowVector localF0{local[12],local[13],local[14],1};
    const auto& sourceCenter=racer690.bounds.center;
    const OriginalFollowVector saved20{sourceCenter.v[0],sourceCenter.v[1],sourceCenter.v[2],sourceCenter.v[3]};
    if(std::memcmp(&localF0,&in.mainBodyLocalF0,sizeof localF0)||
       std::memcmp(&saved20,&in.racerRegistrationPosition20,sizeof saved20))return r;
    std::size_t selected=collection.count30;
    for(std::size_t i=0;i<collection.count30;++i){
        if(!collection.registered38[i])return r;
        if(selected==collection.count30&&collection.registered38[i]==children.death68Key)selected=i;
    }
    if(selected==collection.count30||zones.count>std::numeric_limits<std::size_t>::max()/sizeof(std::uint64_t)||
       subject.graph->count>std::numeric_limits<std::size_t>::max()/sizeof(SceneNode49P)||
       racer690.storage.capacity>std::numeric_limits<std::size_t>::max()/sizeof(CollectionZoneLink50BX)||
       racer690.storage.capacity>std::numeric_limits<std::size_t>::max()/sizeof(LiveZoneEntry50EN))return r;
    const OriginalCameraSceneSpan writes[]={{&controller,sizeof controller},{&death,sizeof death},
        {&scene,sizeof scene},{&children,sizeof children},{&collection,sizeof collection},
        {subject.graph->nodes,subject.graph->count*sizeof(SceneNode49P)}};
    const OriginalCameraSceneSpan reads[]={{&zones,sizeof zones},{zones.keys4C0,zones.count*sizeof(std::uint64_t)},
        {collection.registered38,collection.count30*sizeof(std::uint64_t)},{&in,sizeof in},
        {&subject,sizeof subject},{subject.graph,sizeof *subject.graph},{&racer690,sizeof racer690},
        {racer690.storage.links,racer690.storage.capacity*sizeof(CollectionZoneLink50BX)},
        {racer690.storage.entries,racer690.storage.capacity*sizeof(LiveZoneEntry50EN)}};
    if(!admitOriginalCameraSceneStorage(visibility,spatial,workspace,writes,6,reads,9))return r;
    OriginalCameraOwnerSpan queryWrites[6],queryReads[11];
    for(std::size_t i=0;i<6;++i)queryWrites[i]={writes[i].data,writes[i].bytes};
    for(std::size_t i=0;i<9;++i)queryReads[i]={reads[i].data,reads[i].bytes};
    queryReads[9]={visibility.portals.portalHeads,visibility.portals.zoneCount*sizeof(CollectionRef50BX)};
    queryReads[10]={visibility.portals.portals,visibility.portals.portalCount*sizeof(*visibility.portals.portals)};
    if(cold?!admitOriginalDeathCameraQueryColdStorage(query,*cold,queryWrites,6,queryReads,11):
       !admitOriginalDeathCameraQueryStorage(query,queryWrites,6,queryReads,11))return r;
    // The scene and query use genuinely different lifecycle namespaces. Check
    // every scene allocation against the complete query allocation BEFORE the
    // first irreversible source effect. Scene admission above already checked
    // these allocations against each other and against the borrowed inputs.
    const auto& life=visibility.lifecycle;const auto& portals=visibility.portals;
    const OriginalCameraOwnerSpan sceneWrites[]={
        {&visibility,sizeof visibility},{&spatial,sizeof spatial},{&workspace,sizeof workspace},
        {life.storage.registries,life.storage.registryCapacity*sizeof(*life.storage.registries)},
        {life.storage.registrations,life.storage.registrationCapacity*sizeof(*life.storage.registrations)},
        {life.storage.views.methods,life.storage.views.methodCapacity*sizeof(*life.storage.views.methods)},
        {life.storage.views.zones,life.storage.views.zoneCapacity*sizeof(*life.storage.views.zones)},
        {life.storage.views.blocks,life.storage.views.blockCapacity*sizeof(*life.storage.views.blocks)},
        {spatial.storage.links,spatial.storage.capacity*sizeof(*spatial.storage.links)},
        {spatial.storage.entries,spatial.storage.capacity*sizeof(*spatial.storage.entries)},
        {workspace.registries,workspace.registryCapacity*sizeof(*workspace.registries)},
        {workspace.links,workspace.membershipCapacity*sizeof(*workspace.links)},
        {workspace.entries,workspace.membershipCapacity*sizeof(*workspace.entries)},
        {portals.portalLinks,portals.portalLinkCount*sizeof(*portals.portalLinks)},
        {workspace.portalLinks,workspace.portalLinkCapacity*sizeof(*workspace.portalLinks)}};
    if(cold?!admitOriginalDeathCameraQueryColdStorage(query,*cold,sceneWrites,15,nullptr,0):
       !admitOriginalDeathCameraQueryStorage(query,sceneWrites,15,nullptr,0))return r;
    r.sourcePC=0x177c50u;
    r.visibility=removeOriginalCameraScene1148D0(visibility,spatial,workspace);
    if(r.visibility.status!=LiveZoneStatus50EN::Complete){r.status=S::visibility;return r;}
    r.sceneRemoved=true;scene.registrationKey=0;
    r.death=beginOriginalDeathCamera22D810(death,subject);
    if(r.death.status!=OriginalDeathCameraStatus50KG::complete){
        r.status=S::numeric;r.sourcePC=r.death.sourcePC;return r;
    }
    if(!query.state){
        r.sourcePC=0x22d8a8u;
        const auto status=cold?constructOriginalDeathCameraQuery22D8A8(query,*cold):OriginalDeathCameraQueryStatus::Binding;
        if(status!=OriginalDeathCameraQueryStatus::Complete){
            r.query.status=status;r.query.sourcePC=cold&&cold->sourcePC?cold->sourcePC:0x22d8a8u;
            r.status=S::query;r.sourcePC=r.query.sourcePC;return r;
        }
    }
    //177AB0 exports the previous selected camera, then115060 overwrites its
    //scene fields.22D810 copies that packet but286F90 ignores every word.
    // Do not invent values for the source-unwritten packet tail.
    r.sourcePC=0x22d928u;
    if(!refreshSceneWorld49P(*subject.graph,identity.body288)){r.status=S::owner;return r;}
    const auto& world=subject.graph->nodes[static_cast<std::uint32_t>(node)].world100.v;
    const SpringVector4_48N worldPoint{{world[12],world[13],world[14],1}};
    if(attachOriginalDeathCameraQuery120(query,zone,worldPoint,r.query)!=OriginalDeathCameraQueryStatus::Complete){
        r.status=S::query;r.sourcePC=r.query.sourcePC;return r;
    }
    r.queryAttached=true;
    r.death=selectOriginalDeathCamera22D810(death,subject,query);
    if(r.death.status!=OriginalDeathCameraStatus50KG::complete){
        r.status=S::query;r.sourcePC=r.death.sourcePC;return r;
    }
    r.childActivated=true;collection.selected34=selected;r.childSelected=true; //178224/115E60
    //178238 reads LOCAL F0;178280 binds scene in the racer's actual primary
    //visibility zone.1782A4 overwrites registration20 only after attach returns.
    const SpringVector4_48N localPoint{{localF0.x,localF0.y,localF0.z,1}};
    r.sourcePC=0x178280u;
    r.visibility=attachOriginalCameraScene114770(visibility,spatial,zone,localPoint,workspace);
    if(r.visibility.status!=LiveZoneStatus50EN::Complete){r.status=S::visibility;return r;}
    r.sceneAttached=true;
    spatial.registration34.bounds.center=sourceCenter;
    spatial.registration34.bounds.cachedCenter=sourceCenter;
    scene.position180=localF0;scene.registrationPosition20=saved20;
    scene.parentZoneKey=in.racerZoneKey;scene.parentMethod=2;scene.registrationKey=spatial.allocationSerial;
    controller.mode44=1;r.status=S::complete;r.sourcePC=0x178648u;return r;
}

// Source199770(1,0,0,0,1), reached ONLY after the actual1AD230 returns at
//176808. The source writes transition1E0=.25 only on the visible->hidden edge.
inline void hideOriginalRetirementUi199770(std::uint8_t& flags1E4,float& transition1E0) noexcept {
    flags1E4=static_cast<std::uint8_t>((flags1E4&~0x1cu)|0x10u);
    if(!(flags1E4&1u)){flags1E4=static_cast<std::uint8_t>(flags1E4|1u);transition1E0=.25f;}
}

struct OriginalRetirementCameraOwners50KG {
    OriginalRaceCameraOwner& controller;
    OriginalFollowCamera& follow;
    OriginalAttachedCamera& attached;
    OriginalDeathCamera50KG& death;
    OriginalDeathChildRegistry50KG& deathChildren;
    const OriginalFollowChildRegistry& followChildren;
    const OriginalAttachedChildRegistry& attachedChildren;
    GameplayBlurPending& blur;
    // Results replay with race1B0 bit4 and race48+78 (17622C..17624C): the
    // death branch is 17692C..17698C, entered from any camera mode.
    bool replayDirector=false;
    std::uint64_t tvChildKey=0;
};
// Typed continuation around175D70's actual death mode1 branch. This never
// pretends that missing race HUD text owners executed1AD230. Caller retains
// a pending cut, invokes those genuine owners, then resumes once. The native
// session presently lacks those two-view HUD arrays; that is an explicit join.
class OriginalRetirementCameraCursor50KG final {
    enum class Phase : std::uint8_t {idle,transition,hud};
    Phase phase_=Phase::idle;
    OriginalRaceCameraOwner* controller_=nullptr;
    OriginalDeathCamera50KG* death_=nullptr;
    OriginalCameraChildRegistry* collection_=nullptr;
    std::uint64_t deathKey_=0;
    CameraRacerClass racerClass_=CameraRacerClass::unavailable;
    std::uint8_t raceFlags1B0_=0;
    bool replay_=false;GameplayBlurPending* blur_=nullptr;
    OriginalRetirementCameraResult50KG finish() noexcept {
        controller_->flags62=static_cast<std::uint8_t>(controller_->flags62&~0x80u);
        phase_=Phase::idle;
        return {OriginalRetirementCameraStatus50KG::needsSelectedPose115CB0,0x176dbcu,true};
    }
    OriginalRetirementCameraResult50KG afterModeSelected() noexcept {
        if(controller_->mode44!=1||!death_||!death_->active||!collection_||
           collection_->selected34>=collection_->count30||
           collection_->registered38[collection_->selected34]!=deathKey_)return {OriginalRetirementCameraStatus50KG::continuation,0x1767c8u,true};
        // 176954 (replay): mode 1 returns at 176D9C with no 1AD230 HUD stop.
        if(!replay_&&racerClass_==CameraRacerClass::player&&(raceFlags1B0_&3u)==1u){
            phase_=Phase::hud;
            return {OriginalRetirementCameraStatus50KG::needsHudAnimationStop1AD230,0x176800u,true};
        }
        return finish();
    }
public:
    bool pending()const noexcept{return phase_!=Phase::idle;}
    OriginalRetirementCameraResult50KG begin(OriginalRetirementCameraOwners50KG o,
        const StableRaceCameraFrame& frame) noexcept {
        using S=OriginalRetirementCameraStatus50KG;
        OriginalRetirementCameraResult50KG r;const auto& b=o.controller.binding;
        const auto& identity=o.death.identity;auto* collection=o.deathChildren.collection;
        if(pending()){r.status=S::continuation;return r;}
        if(!o.controller.bound||!o.follow.prepared||!o.attached.prepared||!o.death.prepared||
           b.sessionGeneration!=frame.sessionGeneration||b.racerKey!=frame.racerKey||
           b.mainBodyKey!=frame.mainBodyKey||identity.sessionGeneration!=b.sessionGeneration||
           identity.racerKey!=b.racerKey||identity.mainBodyKey!=b.mainBodyKey||
           o.follow.sessionGeneration!=b.sessionGeneration||o.follow.racerKey!=b.racerKey||
           o.follow.mainBodyKey!=b.mainBodyKey||o.attached.binding.sessionGeneration!=b.sessionGeneration||
           o.attached.binding.racerKey!=b.racerKey||o.attached.binding.mainBodyKey!=b.mainBodyKey||
           o.deathChildren.death68!=&o.death||!o.deathChildren.death68Key||
           o.deathChildren.death68Key!=identity.death68Key||!collection||!collection->bound||
           collection->sessionGeneration!=b.sessionGeneration||collection->sceneKey!=b.sceneKey||
           !collection->registered38||!collection->count30||collection->count30>64||
           collection->selected34>=collection->count30||!collection->controllerKey||
           o.followChildren.collection!=collection||o.followChildren.follow64!=&o.follow||!o.followChildren.follow64Key||
           o.attachedChildren.collection!=collection||o.attachedChildren.attached6C!=&o.attached||!o.attachedChildren.attached6CKey||
           (o.controller.mode44==0&&(!o.follow.active||
            collection->registered38[collection->selected34]!=o.followChildren.follow64Key))||
           (o.controller.mode44==5&&(!o.attached.active||
            collection->registered38[collection->selected34]!=o.attachedChildren.attached6CKey)))return r;
        // 17622C: race1B0 bit4 with race48+78 skips the ending orbit and, for a
        // racer in 3/4, reaches 17692C from any mode (the TV mode 6 included).
        const bool replay=o.replayDirector&&(frame.raceFlags1B0&0x10u);
        if((frame.racerState40!=3&&frame.racerState40!=4)||
           (!replay&&o.controller.mode44!=0&&o.controller.mode44!=5&&o.controller.mode44!=1)||
           (replay&&o.controller.mode44!=0&&o.controller.mode44!=5&&o.controller.mode44!=1&&o.controller.mode44!=6)||
           (!replay&&(frame.raceFlags1B0&0x10u))||(!replay&&(frame.raceStateFlags58&1u)&&frame.racerCD4==0)){
            r.status=S::sourcePath;r.sourcePC=0x17601cu;return r;
        }
        if(o.controller.mode44==6&&(!o.tvChildKey||collection->registered38[collection->selected34]!=o.tvChildKey))return r;
        if(o.controller.mode44==1&&(!o.death.active||
           collection->registered38[collection->selected34]!=o.deathChildren.death68Key))return r;
        namespace d=original_race_camera_detail;
        if(std::fegetround()!=FE_TONEAREST||!d::scalar(frame.elapsed)||frame.elapsed<0.f||
           !d::scalar(o.controller.cooldown8C)||!d::scalar(o.controller.cutRemaining90)){
            r.status=S::numeric;r.sourcePC=0x175d90u;return r;
        }
        auto next=o.controller;auto blur=o.blur;
        OriginalCameraShakeChannels shake{o.follow.shakeScale174,o.follow.shakeAmplitude170,
            o.attached.shakeScale2C,o.attached.shakeAmplitude28};
        bool attempted=false;std::int32_t strength=0;
        if(!d::advanceCommonPrefix175D70(next,shake,blur,frame.elapsed,attempted,strength)){
            r.status=S::numeric;r.sourcePC=0x175d90u;return r;
        }
        //States3/4 deliberately leave63 bits3/4 as previously stored.
        o.controller=next;o.blur=blur;
        o.follow.shakeScale174=shake.followMagnitude174;o.follow.shakeAmplitude170=shake.followFactor170;
        o.attached.shakeScale2C=shake.attachedMagnitude2C;o.attached.shakeAmplitude28=shake.attachedFactor28;
        controller_=&o.controller;death_=&o.death;collection_=collection;deathKey_=o.deathChildren.death68Key;
        racerClass_=b.racerClass;raceFlags1B0_=frame.raceFlags1B0;replay_=replay;blur_=&o.blur;
        if(o.controller.mode44!=1){phase_=Phase::transition;r.status=S::needsDeathTransition177AB0;r.sourcePC=0x176770u;r.prefixCommitted=true;return r;}
        return afterModeSelected();
    }
    // Call ONLY after transitionOriginalDeath177AB0 returns complete. This
    // does not certify the transition by accepting an arbitrary boolean.
    OriginalRetirementCameraResult50KG afterDeathTransition() noexcept {
        if(phase_!=Phase::transition||!controller_)return {OriginalRetirementCameraStatus50KG::continuation,0x176770u};
        // 176970..176984 (replay): cut90 = 2.0 and 2AED10(blur, 100, 0).
        if(replay_&&blur_){controller_->cutRemaining90=2.f;requestGameplayBlur(*blur_,100,0,0);}
        return afterModeSelected();
    }
    // The actual race HUD animation-stop helper has returned. Its source
    // pointers are not present in this package and must never be synthesized.
    OriginalRetirementCameraResult50KG afterHudAnimationStop(
        std::uint8_t& actualUiFlags1E4,float& actualUiTransition1E0) noexcept {
        if(phase_!=Phase::hud||!controller_)return {OriginalRetirementCameraStatus50KG::continuation,0x176800u};
        hideOriginalRetirementUi199770(actualUiFlags1E4,actualUiTransition1E0);
        return finish();
    }
};
} // namespace rr::game
