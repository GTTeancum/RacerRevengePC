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

// The legacy25 key projection above remains unchanged. SC owns its separate
// exact30-row projection; only the actual current SC lighting lease selects it.
struct OriginalScCameraVisibilityKeys50SC {
    const original_gc_gameplay_lighting_candidate::Owner* owner=nullptr;
    std::uint64_t sessionGeneration=0,sceneKey=0;
    std::array<std::uint64_t,30> keys{};
    std::size_t count=0;
    OriginalFollowVisibilityZones view() const noexcept {return {keys.data(),count};}
    bool matches(const original_gc_gameplay_lighting_candidate::Owner& actualOwner,
        const original_gc_gameplay_lighting_candidate::CameraVisibilityLease& lease,
        std::uint64_t session,std::uint64_t scene) const noexcept {
        if(!lease.isScSourceTopology()||owner!=&actualOwner||sessionGeneration!=session||sceneKey!=scene||
           !session||!scene||count!=keys.size()||count!=lease.zoneCount())return false;
        for(std::size_t i=0;i<count;++i)if(!keys[i]||keys[i]!=lease.zoneKey4C0(i))return false;
        return true;
    }
};
class OriginalSelectedCameraVisibilityKeys50SC;
class OriginalSelectedCameraVisibilityBacking50SC;
inline OriginalCameraVisibilityJoinResult attachFreshSelectedOriginalCamera0Visibility50SC(
    original_gc_gameplay_lighting_candidate::Owner&,std::uint64_t,std::uint64_t,
    OriginalCameraSceneOwner&,OriginalSelectedCameraVisibilityBacking50SC&,
    OriginalSelectedCameraVisibilityKeys50SC&,OriginalFollowSceneBinding&) noexcept;

// Stable bounded backing for every supported topology. The maxima are the
// existing SC envelope; only the current lease's active span is exposed.
class OriginalSelectedCameraVisibilityBacking50SC final {
    friend OriginalCameraVisibilityJoinResult attachFreshSelectedOriginalCamera0Visibility50SC(
        original_gc_gameplay_lighting_candidate::Owner&,std::uint64_t,std::uint64_t,
        OriginalCameraSceneOwner&,OriginalSelectedCameraVisibilityBacking50SC&,
        OriginalSelectedCameraVisibilityKeys50SC&,OriginalFollowSceneBinding&) noexcept;
    std::array<CollectionZoneLink50BX,35> links_{};
    std::array<LiveZoneEntry50EN,35> entries_{};
public:
    OriginalSelectedCameraVisibilityBacking50SC() noexcept=default;
    OriginalSelectedCameraVisibilityBacking50SC(const OriginalSelectedCameraVisibilityBacking50SC&)=delete;
    OriginalSelectedCameraVisibilityBacking50SC& operator=(const OriginalSelectedCameraVisibilityBacking50SC&)=delete;
    OriginalSelectedCameraVisibilityBacking50SC(OriginalSelectedCameraVisibilityBacking50SC&&)=delete;
    OriginalSelectedCameraVisibilityBacking50SC& operator=(OriginalSelectedCameraVisibilityBacking50SC&&)=delete;
};
class OriginalSelectedCameraVisibilityKeys50SC final {
    friend OriginalCameraVisibilityJoinResult attachFreshSelectedOriginalCamera0Visibility50SC(
        original_gc_gameplay_lighting_candidate::Owner&,std::uint64_t,std::uint64_t,
        OriginalCameraSceneOwner&,OriginalSelectedCameraVisibilityBacking50SC&,
        OriginalSelectedCameraVisibilityKeys50SC&,OriginalFollowSceneBinding&) noexcept;
    const original_gc_gameplay_lighting_candidate::Owner* owner_=nullptr;
    std::uint64_t sessionGeneration_=0,sceneKey_=0;
    std::array<std::uint64_t,30> keys_{};
    OriginalSelectedCameraVisibilityKeys50SC(const OriginalSelectedCameraVisibilityKeys50SC&)=default;
    OriginalSelectedCameraVisibilityKeys50SC& operator=(const OriginalSelectedCameraVisibilityKeys50SC&)=default;
public:
    OriginalSelectedCameraVisibilityKeys50SC() noexcept=default;
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

// Same175224 transaction as the legacy join. Choice occurs only after borrowing
// the actual owner, and is committed with the real attachment. Subsequent views
// address the retained member arrays, never this function's staging object.
inline OriginalCameraVisibilityJoinResult attachFreshSelectedOriginalCamera0Visibility50SC(
    original_gc_gameplay_lighting_candidate::Owner& actualWorldOwner,
    std::uint64_t sessionGeneration,std::uint64_t sceneKey,
    OriginalCameraSceneOwner& spatial,OriginalSelectedCameraVisibilityBacking50SC& retainedBacking,
    OriginalSelectedCameraVisibilityKeys50SC& keys,OriginalFollowSceneBinding& scene) noexcept {
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
    OriginalSelectedCameraVisibilityKeys50SC nextKeys;
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
