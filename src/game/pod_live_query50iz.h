#pragma once
#include "vehicle_live_query50eq.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

namespace rr::game {
// Stable caller-owned binding of the real vehicle694, not a presence receipt.
// source must be the canonical VehicleFrameParameters49W::shape owner used by
// the frame. live.expectedObjectRef identifies694 in its collision namespace.
// The context and referenced backing must outlive every native bank visit.
struct PodLiveQueryContext50IZ {
    VehicleLiveQueryContext50EQ live;
    const PodShapeSource49W* source=nullptr;
};
enum class PodLiveQueryStatus50IZ {Complete,Storage,Admission,Preparation,Registry,Receipt};
struct PodLiveQueryResult50IZ {
    PodLiveQueryStatus50IZ status=PodLiveQueryStatus50IZ::Admission;
    std::uint32_t sourcePC=0;
    bool preparationCommitted=false,registryCommitted=false;
    LiveZoneResult50EN registry{};
};
// This tests the actual attached object; it never erases a nonnull marker to
// enter the old null branch. Detached coherence is useful between reset calls.
inline bool coherentPodLiveQuery50IZ(const PodShapeState49W&shape,
    const PodLiveQueryContext50IZ&context) noexcept {
    const auto&live=context.live;const auto&membership=live.registrationE0;
    if(!context.source||!live.expectedObjectRef||live.expectedObjectRef>live.lifecycle.objectCount||shape.type88)return false;
    if(!shape.registrationE0)return !membership.attached&&!membership.object;
    return shape.registrationE0==live.expectedObjectRef&&membership.attached&&
        membership.object==live.expectedObjectRef&&membership.method==ZoneMethod50CL::Second&&
        membership.lastShapeTag==0u&&
        membership.storage.links&&membership.storage.entries&&membership.storage.capacity;
}
namespace pod_live_detail50IZ {
struct Span {std::uintptr_t first=0,last=0;};
template<class T> inline bool span(const T*p,std::size_t count,Span&out) noexcept {
    out={};if(!count)return true;
    const auto first=reinterpret_cast<std::uintptr_t>(p);
    if(!p||first%alignof(T)||count>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto bytes=count*sizeof(T);
    if(first>std::numeric_limits<std::uintptr_t>::max()-bytes)return false;
    out={first,first+bytes};return true;
}
inline bool overlap(Span a,Span b) noexcept {return a.first<b.last&&b.first<a.last;}
struct Guard {
    Span writes[24]{};std::size_t count=0;
    template<class T> bool write(T*p,std::size_t n=1) noexcept {
        Span value;if(count==24||!span(p,n,value))return false;
        for(std::size_t i=0;i<count;++i)if(overlap(value,writes[i]))return false;
        writes[count++]=value;return true;
    }
    template<class T> bool read(const T*p,std::size_t n=1) const noexcept {
        Span value;if(!span(p,n,value))return false;
        for(std::size_t i=0;i<count;++i)if(overlap(value,writes[i]))return false;
        return true;
    }
};
inline bool storage(PodShapeState49W&shape,const BodyMatrix48G&worldMatrix,
    const PodLiveQueryContext50IZ&context) noexcept {
    auto&live=context.live;auto&life=live.lifecycle;auto&world=live.world;
    auto&membership=live.registrationE0;auto&work=live.workspace;Guard g;
    if(!g.write(&shape)||!g.write(&life)||!g.write(&membership)||!g.write(&work)||
       !g.write(life.storage.registries,life.storage.registryCapacity)||
       !g.write(life.storage.registrations,life.storage.registrationCapacity)||
       !g.write(life.storage.views.methods,life.storage.views.methodCapacity)||
       !g.write(life.storage.views.zones,life.storage.views.zoneCapacity)||
       !g.write(life.storage.views.blocks,life.storage.views.blockCapacity)||
       !g.write(membership.storage.links,membership.storage.capacity)||
       !g.write(membership.storage.entries,membership.storage.capacity)||
       !g.write(work.registries,work.registryCapacity)||
       !g.write(work.links,work.membershipCapacity)||!g.write(work.entries,work.membershipCapacity)||
       !g.write(world.portalLinks,world.portalLinkCount)||!g.write(work.portalLinks,work.portalLinkCapacity))return false;
    return g.read(&context)&&g.read(context.source)&&g.read(&worldMatrix)&&g.read(&world)&&
        g.read(world.portalHeads,world.zoneCount)&&g.read(world.portals,world.portalCount);
}
}
// Concrete11FF90 ->11F680 ->120230 ->1202E0 ->280D40. Preparation and live
// update commit in source order. On registry/receipt failure the successfully
// prepared canonical shape remains, and no phase or fake Ready is published.
// Source, input world and all mutable registry/backing spans are disjoint.
// No allocation, scene refresh, static collection, E0 clearing or callback.
inline PodLiveQueryResult50IZ updateRegisteredPodShape50IZ(PodShapeState49W&shape,
    const BodyMatrix48G&mainCachedWorld100,const PodLiveQueryContext50IZ&context) noexcept {
    PodLiveQueryResult50IZ result{};
    if(!pod_live_detail50IZ::storage(shape,mainCachedWorld100,context)){
        result.status=PodLiveQueryStatus50IZ::Storage;return result;
    }
    if(!shape.registrationE0||!coherentPodLiveQuery50IZ(shape,context))return result;
    if(!preparePodShape50IZ(*context.source,mainCachedWorld100,shape)){
        result.status=PodLiveQueryStatus50IZ::Preparation;result.sourcePC=0x11f680u;return result;
    }
    result.preparationCommitted=true;
    LiveZoneBounds50EN bounds{};bounds.bounds.shapeTag=0;
    bounds.bounds.shapeWorld=shape.composed00;bounds.bounds.cachedCenter=shape.center60;
    bounds.bounds.cachedRadius=shape.radius80;bounds.type0Resource=context.source;
    auto&live=context.live;const auto revision=live.registrationE0.revision;
    result.registry=updateLiveZoneMembership50EN(live.lifecycle,live.world,
        live.registrationE0,bounds,live.workspace);
    if(result.registry.status!=LiveZoneStatus50EN::Complete){
        result.status=PodLiveQueryStatus50IZ::Registry;result.sourcePC=0x120230u;return result;
    }
    result.registryCommitted=true;const auto&membership=live.registrationE0;
    if(!coherentPodLiveQuery50IZ(shape,context)||membership.revision!=revision+1u||
       membership.lastShapeTag!=0u||membership.lastType2Resource||
       std::memcmp(&membership.lastShapeWorld,&shape.composed00,sizeof(shape.composed00))){
        result.status=PodLiveQueryStatus50IZ::Receipt;result.sourcePC=0x280d40u;return result;
    }
    result.status=PodLiveQueryStatus50IZ::Complete;return result;
}
} //namespace rr::game
