#pragma once
#include "original_gc_gameplay_lighting_candidate.h"
#include "original_follow_controller.h"
#include <array>

namespace rr::game {
enum class OriginalCameraVisibilityJoinStatus : std::uint8_t {
    complete,owner,freshness,storage,attachment,binding
};
struct OriginalCameraVisibilityJoinResult {
    OriginalCameraVisibilityJoinStatus status=OriginalCameraVisibilityJoinStatus::owner;
    original_gc_gameplay_lighting_candidate::Diagnostic owner{};
    LiveZoneResult50EN registration{};
    bool sceneAttached=false;
};
class OriginalSelectedCameraVisibilityKeys;
class OriginalSelectedCameraVisibilityBacking;
inline OriginalCameraVisibilityJoinResult attachFreshSelectedOriginalCamera0Visibility(
    original_gc_gameplay_lighting_candidate::Owner&,std::uint64_t,std::uint64_t,
    OriginalCameraSceneOwner&,OriginalSelectedCameraVisibilityBacking&,
    OriginalSelectedCameraVisibilityKeys&,OriginalFollowSceneBinding&) noexcept;

// Stable bounded backing for every topology, sized from the one lighting
// owner's allocation ceilings (maximumZones keys, maximumPortals+1 memberships);
// only the current lease's active span is exposed.
class OriginalSelectedCameraVisibilityBacking final {
    friend OriginalCameraVisibilityJoinResult attachFreshSelectedOriginalCamera0Visibility(
        original_gc_gameplay_lighting_candidate::Owner&,std::uint64_t,std::uint64_t,
        OriginalCameraSceneOwner&,OriginalSelectedCameraVisibilityBacking&,
        OriginalSelectedCameraVisibilityKeys&,OriginalFollowSceneBinding&) noexcept;
    std::array<CollectionZoneLink50BX,original_gc_gameplay_lighting_candidate::maximumPortals+1u> links_{};
    std::array<LiveZoneEntry50EN,original_gc_gameplay_lighting_candidate::maximumPortals+1u> entries_{};
public:
    OriginalSelectedCameraVisibilityBacking() noexcept=default;
    OriginalSelectedCameraVisibilityBacking(const OriginalSelectedCameraVisibilityBacking&)=delete;
    OriginalSelectedCameraVisibilityBacking& operator=(const OriginalSelectedCameraVisibilityBacking&)=delete;
    OriginalSelectedCameraVisibilityBacking(OriginalSelectedCameraVisibilityBacking&&)=delete;
    OriginalSelectedCameraVisibilityBacking& operator=(OriginalSelectedCameraVisibilityBacking&&)=delete;
};
class OriginalSelectedCameraVisibilityKeys final {
    friend OriginalCameraVisibilityJoinResult attachFreshSelectedOriginalCamera0Visibility(
        original_gc_gameplay_lighting_candidate::Owner&,std::uint64_t,std::uint64_t,
        OriginalCameraSceneOwner&,OriginalSelectedCameraVisibilityBacking&,
        OriginalSelectedCameraVisibilityKeys&,OriginalFollowSceneBinding&) noexcept;
    const original_gc_gameplay_lighting_candidate::Owner* owner_=nullptr;
    std::uint64_t sessionGeneration_=0,sceneKey_=0;
    std::array<std::uint64_t,original_gc_gameplay_lighting_candidate::maximumZones> keys_{};
    OriginalSelectedCameraVisibilityKeys(const OriginalSelectedCameraVisibilityKeys&)=default;
    OriginalSelectedCameraVisibilityKeys& operator=(const OriginalSelectedCameraVisibilityKeys&)=default;
public:
    OriginalSelectedCameraVisibilityKeys() noexcept=default;
    std::size_t count=0;
    OriginalFollowVisibilityZones view() const noexcept {
        if(!owner_||!sessionGeneration_||!sceneKey_||!count||count>keys_.size())return {};
        return {keys_.data(),count};
    }
    bool matches(const original_gc_gameplay_lighting_candidate::Owner& owner,
        const original_gc_gameplay_lighting_candidate::CameraVisibilityLease& lease,
        std::uint64_t session,std::uint64_t scene) const noexcept {
        if(owner_!=&owner||!session||!scene||sessionGeneration_!=session||sceneKey_!=scene||
           !count||count>keys_.size()||count!=lease.zoneCount())return false;
        for(std::size_t i=0;i<count;++i)if(!keys_[i]||keys_[i]!=lease.zoneKey4C0(i))return false;
        return true;
    }
};

// Concrete fresh175224 join for camera0 -> actual manager racer0 visibility,
// one key projection for every topology. All real racer7F0 records must already
// exist from the fresh final grid. It borrows their canonical world, allocates
// no second registry, and invokes the actual point registration kernel. The
// keys are committed with the real attachment. Subsequent views
// address the retained member arrays, never this function's staging object.
inline OriginalCameraVisibilityJoinResult attachFreshSelectedOriginalCamera0Visibility(
    original_gc_gameplay_lighting_candidate::Owner& actualWorldOwner,
    std::uint64_t sessionGeneration,std::uint64_t sceneKey,
    OriginalCameraSceneOwner& spatial,OriginalSelectedCameraVisibilityBacking& retainedBacking,
    OriginalSelectedCameraVisibilityKeys& keys,OriginalFollowSceneBinding& scene) noexcept {
    using Status=OriginalCameraVisibilityJoinStatus;
    OriginalCameraVisibilityJoinResult r;
    if(!sessionGeneration||!sceneKey)return r;
    if(spatial.sessionGeneration||spatial.sceneKey||spatial.object30||spatial.registration34.attached||
       keys.owner_||keys.sessionGeneration_||keys.sceneKey_||keys.count||scene.sceneKey||scene.registrationKey){
        r.status=Status::freshness;return r;}
    original_gc_gameplay_lighting_candidate::CameraVisibilityLease lease;
    if(!actualWorldOwner.beginCameraVisibility(0u,lease,r.owner))return r;
    auto* world=lease.world();auto* workspace=lease.workspace();
    const auto* racer=lease.racerVisibility7F0();
    if(!world||!workspace||!racer||!lease.cameraObject30())return r;
    OriginalSelectedCameraVisibilityKeys nextKeys;
    nextKeys.owner_=&actualWorldOwner;nextKeys.sessionGeneration_=sessionGeneration;nextKeys.sceneKey_=sceneKey;
    nextKeys.count=lease.zoneCount();
    if(!nextKeys.count||nextKeys.count>nextKeys.keys_.size()||
       world->lifecycle.zoneCount!=nextKeys.count||world->portals.zoneCount!=nextKeys.count)return r;
    // Check before +1, including the zero-portal topology's one membership.
    if(world->portals.portalCount>=retainedBacking.links_.size())return r;
    const std::size_t activeCapacity=world->portals.portalCount+1u;
    for(std::size_t i=0;i<nextKeys.count;++i){
        nextKeys.keys_[i]=lease.zoneKey4C0(i);if(!nextKeys.keys_[i])return r;
        for(std::size_t j=0;j<i;++j)if(nextKeys.keys_[j]==nextKeys.keys_[i])return r;
    }
    const LiveZoneStorage50EN backing{retainedBacking.links_.data(),retainedBacking.entries_.data(),activeCapacity};
    OriginalCameraSceneOwner next;
    r.status=Status::storage;
    if(!bindFreshOriginalCameraScene(next,sessionGeneration,sceneKey,lease.cameraObject30(),backing))return r;
    // Admit the active membership writes and protect the unused tail against
    // malformed aliasing. The active capacity is not the maximum backing size.
    const OriginalCameraSceneSpan writes[]={{&spatial,sizeof spatial},{&keys,sizeof keys},{&scene,sizeof scene}};
    const OriginalCameraSceneSpan reads[]={{&backing,sizeof backing},{&actualWorldOwner,sizeof actualWorldOwner},
        {racer,sizeof *racer},{racer->storage.links,racer->storage.capacity*sizeof(CollectionZoneLink50BX)},
        {racer->storage.entries,racer->storage.capacity*sizeof(LiveZoneEntry50EN)},
        {retainedBacking.links_.data()+activeCapacity,(retainedBacking.links_.size()-activeCapacity)*sizeof(CollectionZoneLink50BX)},
        {retainedBacking.entries_.data()+activeCapacity,(retainedBacking.entries_.size()-activeCapacity)*sizeof(LiveZoneEntry50EN)}};
    const std::size_t readCount=activeCapacity<retainedBacking.links_.size()?7u:5u;
    if(!admitOriginalCameraSceneStorage(*world,next,*workspace,writes,3,reads,readCount))return r;
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
