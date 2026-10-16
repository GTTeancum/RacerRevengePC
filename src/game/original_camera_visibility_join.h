#pragma once
#include "original_gc_gameplay_lighting_candidate.h"
#include "original_follow_controller.h"
#include <array>

namespace rr::game {
// Retained identity projection only. Mutable spatial registration is the one
// OriginalCameraSceneOwner supplied by the actual camera session, never a copy
// in a racer bank. The keys identify canonical4C0 rows from the borrowed owner.
struct OriginalCameraVisibilityKeys {
    const original_gc_gameplay_lighting_candidate::Owner* owner=nullptr;
    std::uint64_t sessionGeneration=0,sceneKey=0;
    std::array<std::uint64_t,original_gc_gameplay_lighting_candidate::maximumZones> keys{};
    std::size_t count=0;
    OriginalFollowVisibilityZones view() const noexcept {return {keys.data(),count};}
    bool matches(const original_gc_gameplay_lighting_candidate::Owner& actualOwner,
        const original_gc_gameplay_lighting_candidate::CameraVisibilityLease& lease,
        std::uint64_t session,std::uint64_t scene) const noexcept {
        if(owner!=&actualOwner||sessionGeneration!=session||sceneKey!=scene||
           !session||!scene||!count||count>keys.size()||count!=lease.zoneCount())return false;
        for(std::size_t i=0;i<count;++i)if(!keys[i]||keys[i]!=lease.zoneKey4C0(i))return false;
        return true;
    }
};
enum class OriginalCameraVisibilityJoinStatus : std::uint8_t {
    complete,owner,freshness,storage,attachment,binding
};
struct OriginalCameraVisibilityJoinResult {
    OriginalCameraVisibilityJoinStatus status=OriginalCameraVisibilityJoinStatus::owner;
    original_gc_gameplay_lighting_candidate::Diagnostic owner{};
    LiveZoneResult50EN registration{};
    bool sceneAttached=false;
};
// Concrete fresh175224 join for camera0 -> actual manager racer0 visibility.
// All real racer7F0 records must already exist from the fresh final grid. This
// function borrows their canonical world, allocates no second registry, and
// invokes the actual point registration kernel. It does not reset a racer,
// select mode5, invent collision690 data, or claim a complete camera frame.
// Complete attachment remains committed if a later binding projection fails.
inline OriginalCameraVisibilityJoinResult attachFreshOriginalCamera0Visibility(
    original_gc_gameplay_lighting_candidate::Owner& actualWorldOwner,
    std::uint64_t sessionGeneration,std::uint64_t sceneKey,
    OriginalCameraSceneOwner& spatial,const LiveZoneStorage50EN& backing,
    OriginalCameraVisibilityKeys& keys,OriginalFollowSceneBinding& scene) noexcept {
    using Status=OriginalCameraVisibilityJoinStatus;
    OriginalCameraVisibilityJoinResult r;
    if(!sessionGeneration||!sceneKey)return r;
    if(spatial.sessionGeneration||spatial.sceneKey||spatial.object30||spatial.registration34.attached||
       keys.owner||keys.sessionGeneration||keys.sceneKey||keys.count||scene.sceneKey||scene.registrationKey){
        r.status=Status::freshness;return r;}
    original_gc_gameplay_lighting_candidate::CameraVisibilityLease lease;
    if(!actualWorldOwner.beginCameraVisibility(0u,lease,r.owner))return r;
    auto* world=lease.world();auto* workspace=lease.workspace();
    const auto* racer=lease.racerVisibility7F0();
    if(!world||!workspace||!racer||!lease.cameraObject30())return r;
    OriginalCameraVisibilityKeys nextKeys;
    nextKeys.owner=&actualWorldOwner;nextKeys.sessionGeneration=sessionGeneration;nextKeys.sceneKey=sceneKey;
    nextKeys.count=lease.zoneCount();
    if(!nextKeys.count||nextKeys.count>nextKeys.keys.size())return r;
    for(std::size_t i=0;i<nextKeys.count;++i){
        nextKeys.keys[i]=lease.zoneKey4C0(i);if(!nextKeys.keys[i])return r;
        for(std::size_t j=0;j<i;++j)if(nextKeys.keys[j]==nextKeys.keys[i])return r;
    }
    OriginalCameraSceneOwner next;
    r.status=Status::storage;
    if(!bindFreshOriginalCameraScene(next,sessionGeneration,sceneKey,lease.cameraObject30(),backing))return r;
    const OriginalCameraSceneSpan writes[]={{&spatial,sizeof spatial},{&keys,sizeof keys},{&scene,sizeof scene}};
    const OriginalCameraSceneSpan reads[]={{&backing,sizeof backing},{&actualWorldOwner,sizeof actualWorldOwner},
        {racer,sizeof *racer},{racer->storage.links,racer->storage.capacity*sizeof(CollectionZoneLink50BX)},
        {racer->storage.entries,racer->storage.capacity*sizeof(LiveZoneEntry50EN)}};
    if(!admitOriginalCameraSceneStorage(*world,next,*workspace,writes,3,reads,5))return r;
    r.status=Status::attachment;
    r.registration=attachOriginalCameraSceneFromRacer175224(*world,next,*racer,*workspace);
    if(r.registration.status!=LiveZoneStatus50EN::Complete)return r;
    spatial=next;keys=nextKeys;r.sceneAttached=true;
    r.status=Status::binding;
    OriginalFollowSceneBinding projected;
    if(!readOriginalFollowSceneBinding(spatial,keys.view(),projected))return r;
    scene=projected;r.status=Status::complete;return r;
}
}
