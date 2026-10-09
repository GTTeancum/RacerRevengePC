#include "reset_visibility50ey.h"
#include <cstring>
#include <limits>

namespace rr::game { namespace {
using Status=ResetVisibilityStatus50EY;
struct Span {std::uintptr_t first=0,last=0;};
template<class T> bool span(const T* p,std::size_t count,Span& out) noexcept {
    out={};if(!count)return true;
    const auto address=reinterpret_cast<std::uintptr_t>(p);
    if(!p||address%alignof(T)||count>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto bytes=count*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-address)return false;
    out={address,address+bytes};return true;
}
bool overlap(Span a,Span b) noexcept {return a.first<b.last&&b.first<a.last;}
struct Guard {
    Span writes[24]{};std::size_t count=0;
    template<class T> bool write(T* p,std::size_t n) noexcept {
        Span s;if(count==24||!span(p,n,s))return false;
        for(std::size_t i=0;i<count;++i)if(overlap(writes[i],s))return false;
        writes[count++]=s;return true;
    }
    template<class T> bool read(const T* p,std::size_t n) const noexcept {
        Span s;if(!span(p,n,s))return false;
        for(std::size_t i=0;i<count;++i)if(overlap(writes[i],s))return false;
        return true;
    }
};
ResetVisibilityResult50EY fail(Status status,std::uint32_t pc) noexcept {
    ResetVisibilityResult50EY result{};result.status=status;result.sourcePC=pc;return result;
}
bool empty(const ZoneRegistration50CJ& r) noexcept {
    return !r.owner&&!r.slot&&!r.encodedObject;
}
bool layout(ResetVisibilityWorld50EY& space,ResetVisibilityOwner50EY& owner,
    LiveZoneWorkspace50EN& work,const VehicleFrameState49W* frame=nullptr,
    const VehicleFrameBindings49W* bindings=nullptr,
    const PodShapeSource49W* shape=nullptr) noexcept {
    const auto& life=space.lifecycle;const auto& world=space.portals;
    if(!owner.object30||!owner.object254Field50||!owner.storage.capacity||
       !life.perBlock||life.perBlock>1024u||!life.zoneCount||
       life.zoneCount>std::numeric_limits<std::size_t>::max()/4u||
       life.zoneCount>std::numeric_limits<CollectionRef50BX>::max()/2u||
       life.objectCount>(std::numeric_limits<CollectionRef50BX>::max()>>2u)||
       owner.object30>life.objectCount||world.zoneCount!=life.zoneCount||
       life.storage.registryCapacity<life.zoneCount*2u||
       life.storage.registrationCapacity<life.objectCount||
       life.storage.views.methodCapacity<life.zoneCount*2u||
       life.storage.views.zoneCapacity<life.zoneCount||
       life.storage.views.blockCapacity<life.zoneCount*4u)return false;
    Guard g;
    if(!g.write(&space,1)||!g.write(&owner,1)||!g.write(owner.object254Field50,1)||
       !g.write(&work,1)||
       !g.write(life.storage.registries,life.storage.registryCapacity)||
       !g.write(life.storage.registrations,life.storage.registrationCapacity)||
       !g.write(life.storage.views.methods,life.storage.views.methodCapacity)||
       !g.write(life.storage.views.zones,life.storage.views.zoneCapacity)||
       !g.write(life.storage.views.blocks,life.storage.views.blockCapacity)||
       !g.write(owner.storage.links,owner.storage.capacity)||
       !g.write(owner.storage.entries,owner.storage.capacity)||
       !g.write(world.portalLinks,world.portalLinkCount)||
       !g.write(work.registries,work.registryCapacity)||
       !g.write(work.links,work.membershipCapacity)||
       !g.write(work.entries,work.membershipCapacity)||
       !g.write(work.portalLinks,work.portalLinkCapacity)||
       !g.read(world.portalHeads,world.zoneCount)||
       !g.read(world.portals,world.portalCount)||
       !g.read(frame,frame?1u:0u)||!g.read(bindings,bindings?1u:0u)||
       !g.read(shape,shape?1u:0u))return false;
    return true;
}
bool bound(const ResetVisibilityWorld50EY& space,
    const ResetVisibilityOwner50EY& owner) noexcept {
    const auto& member=owner.membership7F0;
    if(!member.attached)return !member.object&&
        empty(space.lifecycle.storage.registrations[owner.object30-1u]);
    return member.object==owner.object30&&member.method==ZoneMethod50CL::Second&&
        member.storage.links==owner.storage.links&&
        member.storage.entries==owner.storage.entries&&
        member.storage.capacity==owner.storage.capacity;
}
} // namespace

ResetVisibilityResult50EY removeResetVisibility50EY(ResetVisibilityWorld50EY& space,
    ResetVisibilityOwner50EY& owner,LiveZoneWorkspace50EN& work) noexcept {
    if(!layout(space,owner,work))return fail(Status::Storage,0x216a90u);
    if(!bound(space,owner))return fail(Status::Binding,0x216a90u);
    if(!owner.membership7F0.attached)return fail(Status::SourceSliceComplete,0x216accu);
    auto result=fail(Status::Registry,0x216aacu);
    result.registry=removeLiveZoneMembership50EN(space.lifecycle,space.portals,
        owner.membership7F0,work);
    if(result.registry.status!=LiveZoneStatus50EN::Complete)return result;
    //50EN clears its nonnull native membership only after every281490 removal.
    // Retained backing replaces280920 free; the next280890/2809C0 uses it fresh.
    *owner.object254Field50=0;
    result.status=Status::SourceSliceComplete;result.sourcePC=0x216accu;
    return result;
}

ResetVisibilityResult50EY attachResetVisibility50EY(ResetVisibilityWorld50EY& space,
    ResetVisibilityOwner50EY& owner,const VehicleFrameState49W& frame,
    const VehicleFrameBindings49W& bindings,const PodShapeSource49W& resource,
    CollectionRef50BX visibilityZoneC30,LiveZoneWorkspace50EN& work) noexcept {
    if(!layout(space,owner,work,&frame,&bindings,&resource))return fail(Status::Storage,0x216874u);
    if(!bound(space,owner)||owner.membership7F0.attached||!visibilityZoneC30||
       visibilityZoneC30>space.lifecycle.zoneCount)return fail(Status::Binding,0x21687cu);
    const auto main=static_cast<std::int32_t>(bindings.lifecycle.main190);
    if(main<0||static_cast<std::uint32_t>(main)>=vehicleFrameNodeCount49W||
       !(frame.nodes[main].sceneFlags190&1u)||frame.shape.type88!=0u||
       frame.shape.registrationE0!=0u||
       std::memcmp(&frame.shape.world80,&frame.nodes[main].world100,sizeof(BodyMatrix48G)))
        return fail(Status::ShapeNotPrepared,0x21684cu);
    LiveZoneBounds50EN input{};input.bounds.shapeTag=0;
    input.bounds.shapeWorld=frame.shape.composed00;
    input.bounds.cachedCenter=frame.shape.center60;
    input.bounds.cachedRadius=frame.shape.radius80;input.type0Resource=&resource;
    auto result=fail(Status::Registry,0x21688cu);
    result.registry=attachLiveZoneMembership50EN(space.lifecycle,space.portals,
        owner.membership7F0,owner.storage,owner.object30,visibilityZoneC30,
        ZoneMethod50CL::Second,frame.shape.radius80,input,work);
    if(result.registry.status!=LiveZoneStatus50EN::Complete)return result;
    result.status=Status::SourceSliceComplete;result.sourcePC=0x21690cu;
    return result;
}
} // namespace rr::game
