#include "undertrack_reset50ew.h"
#include "chariot_scene49u.h"
#include "query_prepare48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <initializer_list>
#include <limits>

namespace rr::game {namespace {
using S=UndertrackResetStatus50EW;
using I=SceneNodeIndex49P;
UndertrackResetResult50EW result(S status,std::uint32_t pc=0) noexcept {
    UndertrackResetResult50EW r{};r.status=status;r.sourcePC=pc;return r;
}
UndertrackResetResult50EW missingOwner(std::uint32_t offset,std::uint32_t pc) noexcept {
    auto r=result(S::MissingQueryOwner,pc);r.ownerOffset=offset;return r;
}
struct Range {std::uintptr_t first=0,last=0;};
template<class T> bool range(const T* p,std::size_t count,Range& out) noexcept {
    out={};if(!count)return true;
    const auto a=reinterpret_cast<std::uintptr_t>(p);
    if(!p||a%alignof(T)||count>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto bytes=count*sizeof(T);if(bytes>std::numeric_limits<std::uintptr_t>::max()-a)return false;
    out={a,a+bytes};return true;
}
bool overlap(Range a,Range b) noexcept {return a.first<b.last&&b.first<a.last;}
struct Guard {
    Range writes[48]{};unsigned count=0;
    Range reads[64]{};unsigned readCount=0;
    template<class T> bool write(T* p,std::size_t n=1) noexcept {
        Range r;if(count==48||!range(p,n,r))return false;
        for(unsigned i=0;i<count;++i)if(overlap(r,writes[i]))return false;
        for(unsigned i=0;i<readCount;++i)if(overlap(r,reads[i]))return false;
        writes[count++]=r;return true;
    }
    template<class T> bool read(const T* p,std::size_t n=1) noexcept {
        Range r;if(readCount==64||!range(p,n,r))return false;
        for(unsigned i=0;i<count;++i)if(overlap(r,writes[i]))return false;
        reads[readCount++]=r;return true;
    }
};
std::uint32_t bits(float x) noexcept {std::uint32_t u;std::memcpy(&u,&x,4);return u;}
bool validIndex(I i) noexcept {const auto n=static_cast<std::int32_t>(i);return n>=0&&n<static_cast<std::int32_t>(vehicleFrameNodeCount49W);}
SceneNode49P& node(VehicleFrameState49W& s,I i) noexcept {return s.nodes[static_cast<unsigned>(i)];}
const SceneNode49P& node(const VehicleFrameState49W& s,I i) noexcept {return s.nodes[static_cast<unsigned>(i)];}
SceneGraph49P graph(VehicleFrameState49W& s) noexcept {return {s.nodes,s.sceneNodeCount};}
bool frameDomain(const VehicleFrameState49W& s,const VehicleFrameBindings49W& b) noexcept {
    if(std::fegetround()!=FE_TONEAREST||s.phase!=VehicleFramePhase49W::PostPackedBytes50S||
       s.fields.vehicle58||s.fields.vehicle3C||b.visual.extra1C0Present||b.visual.extra1C4Present||
       !s.bodies.main.domain.secondBodyPresent||!s.bodies.secondary.scenePresent||
       s.bodies.main.domain.sceneObserverCount||s.bodies.secondaryDomain.sceneObserverCount||
       b.mainDomain.rootObserverPresent19C||b.secondaryDomain.rootObserverPresent19C)return false;
    if(!validVehicleSceneCount49W(s,b)||!validateSceneGraph49P({const_cast<SceneNode49P*>(s.nodes),s.sceneNodeCount}))return false;
    const I ids[]={b.lifecycle.main190,b.lifecycle.hidden198,b.lifecycle.physical19C,
        b.lifecycle.model1B4,b.lifecycle.right1B8,b.lifecycle.left1BC};
    for(unsigned i=0;i<6;++i){if(!validIndex(ids[i])||static_cast<unsigned>(ids[i])>=s.sceneNodeCount||node(s,ids[i]).parent180!=I::none)return false;
        for(unsigned j=0;j<i;++j)if(ids[i]==ids[j])return false;}
    return true;
}
void project(VehicleFrameState49W& s,const VehicleFrameBindings49W& b) noexcept {
    const auto& main=node(s,b.lifecycle.main190);const auto& secondary=node(s,b.lifecycle.physical19C);
    s.bodies.main.components.root.scene=main.world100;
    s.bodies.mainSphere.sceneFlags190=(s.bodies.mainSphere.sceneFlags190&0xffffff00u)|main.sceneFlags190;
    s.bodies.secondary.root.scene=secondary.world100;
    s.bodies.secondary.sceneFlags190=(s.bodies.secondary.sceneFlags190&0xffffff00u)|secondary.sceneFlags190;
}
void orientationGlobal(VehicleFrameState49W& s) noexcept {
    auto& global=s.visualShared.orientationSentinel4433D8;
    if(!global.guard){global.word=0xffffffffu;global.guard=1;}
}
bool place(VehicleFrameState49W& s,I id,const BodyMatrix48G& pose,
    UndertrackPhysicsWorkspace50EW& w,UndertrackSceneOrder50ET order=UndertrackSceneOrder50ET::OrientationThenPosition) noexcept {
    UndertrackDiagnostic50ET d{};return setUndertrackResetScene50ET(graph(s),id,pose,w.scene,d,order);
}
bool poseFor(const VehicleFrameState49W& s,const UndertrackPlacement50ET& placement,
    const float* position,BodyMatrix48G& pose) noexcept {
    UndertrackDiagnostic50ET d{};
    return prepareUndertrackResetPose50ET(position,placement.headingBD0,s.lifecycleShared.up434650.v,pose,d);
}
bool readyMain(const VehicleFrameState49W& s,const VehicleFrameBindings49W& b) noexcept {
    return validIndex(b.lifecycle.main190)&&(node(s,b.lifecycle.main190).sceneFlags190&1u)&&
        node(s,b.lifecycle.main190).parent180==I::none&&
        std::memcmp(&node(s,b.lifecycle.main190).world100,&s.bodies.main.components.root.scene,sizeof(BodyMatrix48G))==0;
}
bool preparePod(const VehicleFrameState49W& s,const VehicleFrameBindings49W& b,
    const VehicleFrameParameters49W& p,PodShapeState49W& pod,LiveZoneBounds50EN& bounds) noexcept {
    if(!readyMain(s,b)||s.shape.registrationE0||s.shape.type88)return false;
    pod=s.shape;if(!transformPodShape49W(p.shape,node(s,b.lifecycle.main190).world100,pod))return false;
    bounds={};bounds.bounds.shapeTag=0;bounds.bounds.shapeWorld=pod.composed00;
    bounds.bounds.cachedCenter=pod.center60;bounds.bounds.cachedRadius=pod.radius80;
    bounds.type0Resource=&p.shape;return true;
}
bool queryOwners(const UndertrackQueryOwners50EW& owners,const ZoneLifecycle50CL& life,
    std::uint32_t& missing) noexcept {
    constexpr std::uint32_t offsets[]={0x690,0x694,0x7a4,0x348};
    for(unsigned i=0;i<4;++i){const auto& q=owners.query[i];missing=offsets[i];
        if(!q.membership||!q.object||q.object>life.objectCount||!q.storage.links||!q.storage.entries||!q.storage.capacity)return false;
        if(q.membership->attached?q.membership->object!=q.object:q.membership->object!=0)return false;
        if(q.membership->storage.capacity&&(q.membership->storage.links!=q.storage.links||
           q.membership->storage.entries!=q.storage.entries||q.membership->storage.capacity!=q.storage.capacity))return false;
        for(unsigned j=0;j<i;++j)if(q.object==owners.query[j].object||q.membership==owners.query[j].membership)return false;
    }
    missing=0;return true;
}
bool registryStorage(Guard& g,VehicleFrameState49W& frame,const UndertrackQueryOwners50EW& owners,
    ZoneLifecycle50CL& life,LiveZoneWorld50EN& world,LiveZoneWorkspace50EN& w,
    UndertrackRegistryProgress50EW& progress) noexcept {
    if(!g.write(&frame)||!g.write(&life)||!g.write(&w)||!g.write(&progress)||
       !g.write(life.storage.registries,life.storage.registryCapacity)||
       !g.write(life.storage.registrations,life.storage.registrationCapacity)||
       !g.write(life.storage.views.methods,life.storage.views.methodCapacity)||
       !g.write(life.storage.views.zones,life.storage.views.zoneCapacity)||
       !g.write(life.storage.views.blocks,life.storage.views.blockCapacity)||
       !g.write(world.portalLinks,world.portalLinkCount)||!g.write(w.registries,w.registryCapacity)||
       !g.write(w.links,w.membershipCapacity)||!g.write(w.entries,w.membershipCapacity)||
       !g.write(w.portalLinks,w.portalLinkCapacity))return false;
    for(const auto& q:owners.query){if(!g.write(q.membership)||!g.write(q.storage.links,q.storage.capacity)||
        !g.write(q.storage.entries,q.storage.capacity))return false;}
    return g.read(&owners)&&g.read(&world)&&g.read(world.portalHeads,world.zoneCount)&&g.read(world.portals,world.portalCount);
}
}

UndertrackResetResult50EW placeInitialUndertrackPhysics50EW(VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const UndertrackPlacement50ET& placement,
    const UndertrackPhysicsDomain50EW& domain,const SecondaryActivationGlobals49V& bodyGlobals,
    const ResetLifecycleGlobals50AT& lifecycleGlobals,UndertrackPhysicsWorkspace50EW& w) noexcept {
    Guard g;if(!g.write(&s)||!g.write(&w)||!g.read(&b)||!g.read(&placement)||!g.read(&domain)||
        !g.read(&bodyGlobals)||!g.read(&lifecycleGlobals))return result(S::Storage);
    if(!frameDomain(s,b))return result(S::Binding,0x216bd0);
    if(domain.terrain330!=UndertrackHeightOwner50EW::Absent)return result(S::UnsupportedHeightOwner,0x216b38);
    if(domain.water334!=UndertrackHeightOwner50EW::Absent)return result(S::UnsupportedHeightOwner,0x216b78);
    if(std::memcmp(lifecycleGlobals.zero434630,s.visualShared.zero434630.v,16))return result(S::Binding,0x216c4c);
    w.frame=s;auto& next=w.frame;BodyMatrix48G pose{};
    if(!poseFor(next,placement,placement.positionBC0,pose))return result(S::Arithmetic,0x216af0);
    orientationGlobal(next);
    if(!place(next,b.lifecycle.main190,pose,w)||!refreshSceneWorld49P(graph(next),b.lifecycle.main190))return result(S::Scene,0x216bd0);
    if(!place(next,b.lifecycle.hidden198,pose,w,UndertrackSceneOrder50ET::PositionThenOrientation))return result(S::Scene,0x216c08);
    if(!resetUndertrackMainBlock50ET(next.bodies.main,next.bodies.mainSphere,next.mainSupplement,
        next.bodies.vehicle838,next.lifecycle,bodyGlobals,lifecycleGlobals))return result(S::Arithmetic,0x216c38);
    if(!place(next,b.lifecycle.model1B4,pose,w,UndertrackSceneOrder50ET::PositionThenOrientation))return result(S::Scene,0x216cf0);
    if(!place(next,b.lifecycle.physical19C,pose,w,UndertrackSceneOrder50ET::PositionThenOrientation))return result(S::Scene,0x216d20);
    resetSecondaryActivation49V(next.bodies.secondary,next.secondarySupplement,bodyGlobals);
    project(next,b);s=next;return result(S::SourceSliceComplete,0x216d64);
}

UndertrackResetResult50EW finishUndertrackPhysics50EW(VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    const UndertrackPlacement50ET& placement,const ResetProbe50AG& probe,
    const VisualVector49R& offset,const UndertrackModelBounds50EW (&models)[3],
    const ResetEffectBindings50BK& effectBindings,EffectOwner50BE* owners,std::size_t ownerCount,
    std::uint32_t& vehicle850,UndertrackPhysicsWorkspace50EW& w) noexcept {
    Guard g;if(ownerCount>maxEffectOwners50BE||!g.write(&s)||!g.write(&w)||!g.write(&vehicle850)||
        !g.write(owners,ownerCount))return result(S::Storage);
    for(const auto& m:models)if(!m.result||!g.write(m.result))return result(S::Storage);
    if(!g.read(&b)||!g.read(&p)||!g.read(&placement)||!g.read(&probe)||!g.read(&offset)||
       !g.read(&effectBindings)||!g.read(models,3))return result(S::Storage);
    for(const auto& m:models)if(!m.count||!g.read(m.nodes,m.count))return result(S::Storage);
    if(!frameDomain(s,b))return result(S::Binding,0x216f6c);
    if(probe.phase!=ResetProbePhase50AG::Hit||probe.attempt>=20||probe.lift!=bits(p.visual.verticalLimit7EC)||
       probe.staged[0]!=bits(placement.positionBC0[0])||probe.staged[2]!=bits(placement.positionBC0[2])||
       probe.staged[3]!=bits(placement.positionBC0[3]))return result(S::GroundNotResolved,0x216f28);
    w.frame=s;w.vehicle850=vehicle850;if(ownerCount)std::memcpy(w.owners,owners,ownerCount*sizeof(*owners));
    auto& next=w.frame;float position[4];std::memcpy(position,probe.staged,16);BodyMatrix48G pose{};
    if(!poseFor(next,placement,position,pose))return result(S::Arithmetic,0x216f6c);
    orientationGlobal(next);
    if(!place(next,b.lifecycle.main190,pose,w)||!refreshSceneWorld49P(graph(next),b.lifecycle.main190))return result(S::Scene,0x216f6c);
    detail::SceneVectorContext49U context{0.f};VisualVector49R translated{};
    if(!detail::pointGraph49W(graph(next),b.lifecycle.main190,offset,context,translated))return result(S::Arithmetic,0x216fac);
    std::memcpy(pose.v+12,translated.v,16);
    if(!place(next,b.lifecycle.hidden198,pose,w))return result(S::Scene,0x216fc8);
    if(!place(next,b.lifecycle.model1B4,pose,w)||
       !refreshResetModelBounds50BA(graph(next),b.lifecycle.model1B4,models[0].nodes,models[0].count,w.bounds,w.modelResults[0])||
       !refreshSceneWorld49P(graph(next),b.lifecycle.model1B4))return result(S::ModelBounds,0x217030);
    if(!place(next,b.lifecycle.physical19C,pose,w))return result(S::Scene,0x21704c);
    const I ids[]={b.lifecycle.right1B8,b.lifecycle.left1BC};
    const VisualVector49R* offsets[]={&p.visual.rightBase2C0,&p.visual.leftBase2B0};
    constexpr std::uint32_t boundPC[]={0x2170dc,0x21714c};
    for(unsigned i=0;i<2;++i){
        if(!detail::pointGraph49W(graph(next),b.lifecycle.main190,*offsets[i],context,translated))return result(S::Arithmetic,boundPC[i]-0x4c);
        std::memcpy(pose.v+12,translated.v,16);
        if(!place(next,ids[i],pose,w)||!refreshResetModelBounds50BA(graph(next),ids[i],models[i+1].nodes,
           models[i+1].count,w.bounds,w.modelResults[i+1])||!refreshSceneWorld49P(graph(next),ids[i]))return result(S::ModelBounds,boundPC[i]);
    }
    resetMainRigidBody50AR(next.bodies.main,next.bodies.mainSphere,next.mainSupplement,p.activation);
    std::uint32_t zero[4];std::memcpy(zero,next.visualShared.zero434630.v,16);
    resetUndertrackPriorPoint50ET(next.bodies.main,w.vehicle850,zero);
    ResetEffectState50BK effectState{};std::memcpy(effectState.timers820,next.impactSupplement.timers820,16);
    const auto stopped=resetEffectStops50BK(effectBindings,w.owners,ownerCount,effectState,w.effects);
    if(stopped.status!=EffectDispatchStatus50BE::Complete){auto r=result(S::Effects,stopped.sourcePC);r.effects=stopped.status;return r;}
    std::memcpy(next.impactSupplement.timers820,effectState.timers820,16);
    finishUndertrackControlHistory50ET(next.bodies.main);project(next,b);
    s=next;vehicle850=w.vehicle850;
    for(unsigned i=0;i<3;++i)*models[i].result=w.modelResults[i];
    if(ownerCount)std::memcpy(owners,w.owners,ownerCount*sizeof(*owners));
    return result(S::SourceSliceComplete,0x2173f8);
}

UndertrackResetResult50EW prepareUndertrackPodQuery50EW(VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,LiveZoneBounds50EN& bounds) noexcept {
    Guard g;if(!g.write(&s)||!g.write(&bounds)||!g.read(&b)||!g.read(&p))return result(S::Storage);
    if(s.phase!=VehicleFramePhase49W::PostPackedBytes50S)return result(S::Phase,0x21684c);
    PodShapeState49W pod{};LiveZoneBounds50EN prepared{};
    if(!preparePod(s,b,p,pod,prepared))return result(S::QueryPreparation,0x21684c);
    CollectionBoundsResult48S checked{};Type0BoundsInput50EW input{};
    input.resource=prepared.type0Resource;input.composed00=prepared.bounds.shapeWorld;
    std::memcpy(input.cachedCenterWords,prepared.bounds.cachedCenter.v,16);std::memcpy(&input.cachedRadiusWord,&prepared.bounds.cachedRadius,4);
    if(prepareType0Bounds50EW(input,checked)!=CollectionBoundsStatus48S::Complete)return result(S::QueryPreparation,0x21685c);
    s.shape=pod;bounds=prepared;return result(S::SourceSliceComplete,0x216864);
}

UndertrackResetResult50EW removeUndertrackCollisionOwners50EW(VehicleFrameState49W& s,
    const UndertrackQueryOwners50EW& owners,ZoneLifecycle50CL& life,LiveZoneWorld50EN& world,
    LiveZoneWorkspace50EN& w,UndertrackRegistryProgress50EW& progress) noexcept {
    Guard g;if(!registryStorage(g,s,owners,life,world,w,progress))return result(S::Storage);
    if(s.phase!=VehicleFramePhase49W::PostPackedBytes50S||progress.removed>4||progress.attached)return result(S::Phase,0x216acc);
    std::uint32_t missing=0;if(!queryOwners(owners,life,missing))return missingOwner(missing,0x216acc);
    const auto& pod=owners.query[1];
    if(s.shape.registrationE0!=(pod.membership->attached?pod.object:0u))return result(S::Binding,0x216ad4);
    constexpr std::uint32_t pcs[]={0x216acc,0x216ad4,0x216adc,0x216ae4};
    while(progress.removed<4){const auto i=progress.removed;
        const auto removed=removeLiveZoneMembership50EN(life,world,*owners.query[i].membership,w);
        if(removed.status!=LiveZoneStatus50EN::Complete){auto r=result(S::Registry,pcs[i]);r.registry=removed;return r;}
        if(i==1)s.shape.registrationE0=0;++progress.removed;
    }
    return result(S::SourceSliceComplete,0x216aec);
}

UndertrackResetResult50EW attachUndertrackCollisionOwners50EW(VehicleFrameState49W& s,
    const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& p,
    const UndertrackPlacement50ET& placement,const UndertrackQueryOwners50EW& owners,
    ZoneLifecycle50CL& life,LiveZoneWorld50EN& world,LiveZoneWorkspace50EN& w,
    UndertrackRegistryProgress50EW& progress) noexcept {
    Guard g;if(!registryStorage(g,s,owners,life,world,w,progress))return result(S::Storage);
    if(!g.read(&b)||!g.read(&p)||!g.read(&placement))return result(S::Storage);
    if(s.phase!=VehicleFramePhase49W::PostPackedBytes50S||progress.removed!=4||progress.attached>4)
        return result(S::Phase,0x21690c);
    std::uint32_t missing=0;if(!queryOwners(owners,life,missing))return missingOwner(missing,0x21690c);
    if(!readyMain(s,b)||!placement.vehicleC34||placement.vehicleC34>life.zoneCount)return result(S::Binding,0x21691c);
    // Preflight actual prepared resource/backing owners before any attachment.
    for(unsigned i:{0u,3u}){const auto& q=owners.query[i];
        if(!q.type2||!q.type2->resource||!q.type2->queryScene.value)return missingOwner(i?0x348:0x690,i?0x216984:0x21691c);
        if(!g.write(q.type2)||!g.read(q.type2->resource)||
           !g.read(q.type2->resource->nodes,q.type2->resource->nodeCount)||
           !g.read(q.type2->resource->rays,q.type2->resource->rayCount))return result(S::Storage);
    }
    const auto& movement=owners.query[2];
    if(!movement.endpoints||!movement.metadata||!movement.shapeTag||!movement.segmentSphere)return missingOwner(0x7a4,0x21696c);
    if(!g.write(movement.endpoints,2)||!g.write(movement.metadata,2)||!g.write(movement.shapeTag)||!g.write(movement.segmentSphere))return result(S::Storage);
    constexpr unsigned order[]={0,2,3,1};constexpr std::uint32_t pcs[]={0x21691c,0x21696c,0x216984,0x21699c};
    while(progress.attached<4){const unsigned stage=progress.attached,index=order[stage];const auto& q=owners.query[index];
        if(q.membership->attached||q.membership->object)return result(S::Binding,pcs[stage]);
        LiveZoneBounds50EN bounds{};Type2StaticQuery49X prepared2{};PodShapeState49W prepared0{};
        SpringVector4_48N endpoints[2]{};std::uint32_t metadata[2]{},tag=3;
        if(index==0||index==3){prepared2=*q.type2;const auto& resource=*prepared2.resource;
            QuerySpherePrepareInput48O input{};input.preparedScene=node(s,b.lifecycle.main190).world100;
            input.hasPairTree=resource.nodeCount!=0;
            if(input.hasPairTree){if(!resource.nodes||(resource.root&type2Leaf49X)||resource.root>=resource.nodeCount)return result(S::QueryPreparation,pcs[stage]);
                for(unsigned k=0;k<3;++k){input.treeCenter.v[k]=resource.nodes[resource.root].center[k];input.treeHalf.v[k]=resource.nodes[resource.root].half[k];}}
            else {if(resource.root!=type2Leaf49X||!resource.rays||!resource.rayCount)return result(S::QueryPreparation,pcs[stage]);input.first=resource.rays[0].first;input.second=resource.rays[0].second;}
            QuerySpherePrepareOutput48O prepared{};
            if(prepareQuerySphere48O(input,prepared)!=QuerySpherePrepareStatus48O::Ready)return result(S::QueryPreparation,pcs[stage]);
            prepared2.worldMatrix=prepared.world;for(unsigned k=0;k<3;++k)prepared2.sphere.center.v[k]=prepared.center.v[k];prepared2.sphere.radius=prepared.radius;
            bounds.bounds.shapeTag=2;bounds.bounds.shapeWorld=prepared.world;bounds.bounds.cachedCenter=prepared.center;
            bounds.bounds.cachedRadius=prepared.radius;bounds.type2Resource=prepared2.resource;
        }else if(index==1){if(!preparePod(s,b,p,prepared0,bounds))return result(S::QueryPreparation,pcs[stage]);
        }else {
            //216934..216960:28BAB0(main190,body340+1E0), then11CA80 pair.
            const auto& m=node(s,b.lifecycle.main190).world100;
            for(unsigned k=0;k<3;++k){volatile float x=m.v[k]*p.bodies.main.centerOffset.v[0];
                volatile float y=m.v[4+k]*p.bodies.main.centerOffset.v[1];volatile float xy=x+y;
                volatile float z=m.v[8+k]*p.bodies.main.centerOffset.v[2];volatile float xyz=xy+z;
                volatile float point=xyz+m.v[12+k];if(!std::isfinite(point))return result(S::Arithmetic,0x216940);
                endpoints[0].v[k]=endpoints[1].v[k]=point;}
            endpoints[0].v[3]=endpoints[1].v[3]=1.f;
            bounds.bounds.shapeTag=3;bounds.bounds.pairCount=1;bounds.bounds.endpoints=endpoints;bounds.bounds.endpointCount=2;
            bounds.bounds.endpointScenes=metadata;bounds.bounds.endpointSceneCount=2;
        }
        const auto attached=attachLiveZoneMembership50EN(life,world,*q.membership,q.storage,q.object,
            placement.vehicleC34,ZoneMethod50CL::Second,0.f,bounds,w);
        if(attached.status!=LiveZoneStatus50EN::Complete){auto r=result(S::Registry,pcs[stage]);r.registry=attached;return r;}
        if(index==0||index==3)*q.type2=prepared2;
        else if(index==1){prepared0.registrationE0=q.object;s.shape=prepared0;}
        else {std::memcpy(q.endpoints,endpoints,sizeof endpoints);std::memcpy(q.metadata,metadata,sizeof metadata);*q.shapeTag=tag;
            for(unsigned k=0;k<3;++k)q.segmentSphere->center.v[k]=q.membership->bounds.cachedCenter.v[k];q.segmentSphere->radius=q.membership->bounds.cachedRadius;}
        ++progress.attached;
    }
    return result(S::SourceSliceComplete,0x2169a4);
}
}
