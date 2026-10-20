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

// Session-owned fixed arrays never move or resize. Neither arbitrary counts nor
// a caller-supplied descriptor can select or expose the SC or TC backing.
class OriginalSelectedCameraVisibilityBacking50SC final {
    friend OriginalCameraVisibilityJoinResult attachFreshSelectedOriginalCamera0Visibility50SC(
        original_gc_gameplay_lighting_candidate::Owner&,std::uint64_t,std::uint64_t,
        OriginalCameraSceneOwner&,OriginalSelectedCameraVisibilityBacking50SC&,
        OriginalSelectedCameraVisibilityKeys50SC&,OriginalFollowSceneBinding&) noexcept;
    std::array<CollectionZoneLink50BX,original_gc_gameplay_lighting_candidate::maximumPortals+1> legacyLinks_{};
    std::array<LiveZoneEntry50EN,original_gc_gameplay_lighting_candidate::maximumPortals+1> legacyEntries_{};
    std::array<CollectionZoneLink50BX,35> scLinks_{};
    std::array<LiveZoneEntry50EN,35> scEntries_{};
    std::array<CollectionZoneLink50BX,27> tcLinks_{};
    std::array<LiveZoneEntry50EN,27> tcEntries_{};
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
    enum class Selected : std::uint8_t {none,legacy,sc,tc};
    Selected selected_=Selected::none;
    OriginalCameraVisibilityKeys legacy_{};
    OriginalScCameraVisibilityKeys50SC sc_{};
    OriginalCameraVisibilityKeys tc_{}; // TC has exactly25 canonical zone keys.
    OriginalSelectedCameraVisibilityKeys50SC(const OriginalSelectedCameraVisibilityKeys50SC&)=default;
    OriginalSelectedCameraVisibilityKeys50SC& operator=(const OriginalSelectedCameraVisibilityKeys50SC&)=default;
public:
    OriginalSelectedCameraVisibilityKeys50SC() noexcept=default;
    std::size_t count=0;
    OriginalFollowVisibilityZones view() const noexcept {
        if(selected_==Selected::legacy&&count==legacy_.count)return legacy_.view();
        if(selected_==Selected::sc&&count==sc_.count)return sc_.view();
        if(selected_==Selected::tc&&count==tc_.count&&count==25u)return tc_.view();
        return {};
    }
    bool matches(const original_gc_gameplay_lighting_candidate::Owner& owner,
        const original_gc_gameplay_lighting_candidate::CameraVisibilityLease& lease,
        std::uint64_t session,std::uint64_t scene) const noexcept {
        if(selected_==Selected::legacy)return !lease.isScSourceTopology()&&!lease.isTcSourceTopology()&&count==legacy_.count&&
            legacy_.matches(owner,lease,session,scene);
        if(selected_==Selected::sc)return count==sc_.count&&sc_.matches(owner,lease,session,scene);
        if(selected_==Selected::tc)return lease.isTcSourceTopology()&&count==25u&&count==tc_.count&&
            tc_.matches(owner,lease,session,scene);
        return false;
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
    using Selected=OriginalSelectedCameraVisibilityKeys50SC::Selected;
    OriginalCameraVisibilityJoinResult r;
    if(!sessionGeneration||!sceneKey)return r;
    if(spatial.sessionGeneration||spatial.sceneKey||spatial.object30||spatial.registration34.attached||
       keys.selected_!=Selected::none||keys.count||scene.sceneKey||scene.registrationKey){
        r.status=Status::freshness;return r;}
    original_gc_gameplay_lighting_candidate::CameraVisibilityLease lease;
    if(!actualWorldOwner.beginCameraVisibility(0u,lease,r.owner))return r;
    auto* world=lease.world();auto* workspace=lease.workspace();
    const auto* racer=lease.racerVisibility7F0();
    if(!world||!workspace||!racer||!lease.cameraObject30())return r;
    const bool sc=lease.isScSourceTopology(),tc=lease.isTcSourceTopology();
    if(sc&&tc)return r;
    OriginalSelectedCameraVisibilityKeys50SC nextKeys;
    nextKeys.selected_=tc?Selected::tc:sc?Selected::sc:Selected::legacy;
    nextKeys.count=lease.zoneCount();
    std::uint64_t* selectedKeys=nullptr;
    if(sc){
        auto& k=nextKeys.sc_;k.owner=&actualWorldOwner;k.sessionGeneration=sessionGeneration;k.sceneKey=sceneKey;
        k.count=nextKeys.count;if(k.count!=k.keys.size())return r;selectedKeys=k.keys.data();
    }else if(tc){
        auto& k=nextKeys.tc_;k.owner=&actualWorldOwner;k.sessionGeneration=sessionGeneration;k.sceneKey=sceneKey;
        k.count=nextKeys.count;if(k.count!=25u||k.count!=k.keys.size())return r;selectedKeys=k.keys.data();
    }else{
        auto& k=nextKeys.legacy_;k.owner=&actualWorldOwner;k.sessionGeneration=sessionGeneration;k.sceneKey=sceneKey;
        k.count=nextKeys.count;if(!k.count||k.count>k.keys.size())return r;selectedKeys=k.keys.data();
    }
    for(std::size_t i=0;i<nextKeys.count;++i){
        selectedKeys[i]=lease.zoneKey4C0(i);if(!selectedKeys[i])return r;
        for(std::size_t j=0;j<i;++j)if(selectedKeys[j]==selectedKeys[i])return r;
    }
    const LiveZoneStorage50EN legacyBacking{retainedBacking.legacyLinks_.data(),retainedBacking.legacyEntries_.data(),retainedBacking.legacyLinks_.size()};
    const LiveZoneStorage50EN scBacking{retainedBacking.scLinks_.data(),retainedBacking.scEntries_.data(),retainedBacking.scLinks_.size()};
    const LiveZoneStorage50EN tcBacking{retainedBacking.tcLinks_.data(),retainedBacking.tcEntries_.data(),retainedBacking.tcLinks_.size()};
    const LiveZoneStorage50EN backing=tc?tcBacking:sc?scBacking:legacyBacking;
    const LiveZoneStorage50EN inactive=sc||tc?legacyBacking:scBacking;
    const LiveZoneStorage50EN secondInactive=tc?scBacking:tcBacking;
    OriginalCameraSceneOwner next;
    r.status=Status::storage;
    if(!bindFreshOriginalCameraScene(next,sessionGeneration,sceneKey,lease.cameraObject30(),backing))return r;
    // Whole facade writes and full selected membership spans are admitted. The
    // unused typed backing is also protected against a malformed aliased call.
    const OriginalCameraSceneSpan writes[]={{&spatial,sizeof spatial},{&keys,sizeof keys},{&scene,sizeof scene}};
    const OriginalCameraSceneSpan reads[]={{&backing,sizeof backing},{&actualWorldOwner,sizeof actualWorldOwner},
        {racer,sizeof *racer},{racer->storage.links,racer->storage.capacity*sizeof(CollectionZoneLink50BX)},
        {racer->storage.entries,racer->storage.capacity*sizeof(LiveZoneEntry50EN)},
        {inactive.links,inactive.capacity*sizeof(CollectionZoneLink50BX)},
        {inactive.entries,inactive.capacity*sizeof(LiveZoneEntry50EN)},
        {secondInactive.links,secondInactive.capacity*sizeof(CollectionZoneLink50BX)},
        {secondInactive.entries,secondInactive.capacity*sizeof(LiveZoneEntry50EN)}};
    if(!admitOriginalCameraSceneStorage(*world,next,*workspace,writes,3,reads,9))return r;
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
