#include "vehicle_live_query50eq.h"
#include "query_prepare48o.h"
#include <cstring>
#include <limits>

namespace rr::game { namespace {
struct Span {std::uintptr_t first=0,last=0;};
template<class T> bool span(const T* p,std::size_t count,Span& s) noexcept {
    s={};if(!count)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||
       count>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto first=reinterpret_cast<std::uintptr_t>(p),bytes=count*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-first)return false;
    s={first,first+bytes};return true;
}
bool overlap(Span a,Span b) noexcept {return a.first<b.last&&b.first<a.last;}
struct Guard {
    Span writes[32]{};std::size_t count=0;
    template<class T> bool write(T* p,std::size_t n) noexcept {
        Span s;if(count==32||!span(p,n,s))return false;
        for(std::size_t i=0;i<count;++i)if(overlap(s,writes[i]))return false;
        writes[count++]=s;return true;
    }
    template<class T> bool read(const T* p,std::size_t n) const noexcept {
        Span s;if(!span(p,n,s))return false;
        for(std::size_t i=0;i<count;++i)if(overlap(s,writes[i]))return false;
        return true;
    }
    bool filter(const CollisionFilter48J& f) const noexcept {
        return read(f.categoryMasks.data(),f.categoryMasks.size())&&
            read(f.identityLists[0].identities.data(),f.identityLists[0].identities.size())&&
            read(f.identityLists[1].identities.data(),f.identityLists[1].identities.size());
    }
};

// Neither50EN nor49X sees the other's buffers. Check their combined ownership
// before either can write, including nested immutable query/mesh/filter data.
bool storage(const VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const MainStaticDetectorAdmission50H& admission,const VehicleLiveQueryContext50EQ& live,
    const Type2StaticQuery49X& source,const Type2StaticCandidate49X* owners,std::size_t ownerCount,
    const std::size_t* indices,std::size_t count,const Type2StaticInitial49X& initial,
    const Type2StaticWorkspace49X& w,const Type2StaticDestination49X& d,
    Type2StaticResult49X& result,Type2StaticQuery49X* sourcePublication=nullptr,
    const SourceTriggerActionHooks50MB* sourceHooks=nullptr) noexcept {
    Guard g;auto& life=live.lifecycle;auto& world=live.world;
    auto& membership=live.registrationE0;auto& work=live.workspace;
    if(!g.write(&life,1)||!g.write(&membership,1)||!g.write(&work,1)||
       !g.write(life.storage.registries,life.storage.registryCapacity)||
       !g.write(life.storage.registrations,life.storage.registrationCapacity)||
       !g.write(life.storage.views.methods,life.storage.views.methodCapacity)||
       !g.write(life.storage.views.zones,life.storage.views.zoneCapacity)||
       !g.write(life.storage.views.blocks,life.storage.views.blockCapacity)||
       !g.write(membership.storage.links,membership.storage.capacity)||
       !g.write(membership.storage.entries,membership.storage.capacity)||
       !g.write(work.registries,work.registryCapacity)||
       !g.write(work.links,work.membershipCapacity)||!g.write(work.entries,work.membershipCapacity)||
       !g.write(world.portalLinks,world.portalLinkCount)||!g.write(work.portalLinks,work.portalLinkCapacity)||
       !g.write(w.pairs,w.pairCapacity)||!g.write(w.metadata,w.metadataCapacity)||
       !g.write(w.records,w.recordCapacity)||!g.write(w.effects,w.effectCapacity)||
       !g.write(w.stack,w.stackCapacity)||!g.write(w.trace,w.traceCapacity)||
       !g.write(d.pairs,d.pairCapacity)||!g.write(d.metadata,d.metadataCapacity)||
       !g.write(d.records,d.recordCapacity)||!g.write(d.effects,d.effectCapacity)||!g.write(&result,1))return false;
    Span ownerSpan{},indexSpan{};GateTriggerBinding50KG* gateOwner=nullptr;NativeTriggerGroup50GA* triggerOwner=nullptr;NativeTriggerGroup50TC* tcOwner=nullptr;NativeTriggerGroup50TB* tbOwner=nullptr;
    if(!span(owners,ownerCount,ownerSpan)||!span(indices,count,indexSpan))return false;
    for(std::size_t i=0;i<count;++i){
        if(indices[i]>=ownerCount)return false;
        const auto& owner=owners[indices[i]];
        // Apply the same shape ownership rule before the live membership
        // prefix can mutate buffers or hand this owner to indexed50CC.
        if(((owner.nativeTriggers||owner.nativeTriggersTC||owner.nativeTriggersTB)&&owner.shapeType!=5&&owner.shapeType!=6)||
           (!owner.nativeTriggers&&!owner.nativeTriggersTC&&!owner.nativeTriggersTB&&owner.nativeTriggerIndex!=0))return false;
        if(owner.raOriginalTriggerSegments&&
           (owner.shapeType!=5||owner.gate||owner.nativeTriggersTC||owner.nativeTriggersTB||!owner.nativeTriggers||owner.nativeTriggerIndex>=2))return false;
        if(owner.shapeType==5||owner.shapeType==6){
            if((owner.shapeType==6&&owner.gate)||(unsigned(bool(owner.gate))+unsigned(bool(owner.nativeTriggers))+unsigned(bool(owner.nativeTriggersTC))+unsigned(bool(owner.nativeTriggersTB))!=1u))return false;
            if(owner.gate){
                if(tbOwner||tcOwner||triggerOwner||(gateOwner&&gateOwner!=owner.gate))return false;
                gateOwner=owner.gate;
            }else if(owner.nativeTriggersTC){
                if(tbOwner||gateOwner||triggerOwner||(tcOwner&&tcOwner!=owner.nativeTriggersTC))return false;
                tcOwner=owner.nativeTriggersTC;
            }else if(owner.nativeTriggersTB){
                if(tcOwner||gateOwner||triggerOwner||(tbOwner&&tbOwner!=owner.nativeTriggersTB))return false;
                tbOwner=owner.nativeTriggersTB;
            }else{
                if(tbOwner||tcOwner||gateOwner||(triggerOwner&&triggerOwner!=owner.nativeTriggers))return false;
                triggerOwner=owner.nativeTriggers;
            }
        }
    }
    if(gateOwner&&!g.write(gateOwner,1))return false;
    if(triggerOwner&&(!g.write(triggerOwner,1)||!(sourceHooks?validSourceTriggerGroup50MB(*triggerOwner,*sourceHooks):validNativeTriggerGroup50GA(*triggerOwner))))return false;
    if(tcOwner&&(!g.write(tcOwner,1)||sourceHooks||!validNativeTriggerGroup50TC(*tcOwner)))return false;
    if(tbOwner&&(!g.write(tbOwner,1)||sourceHooks||!validNativeTriggerGroup50TB(*tbOwner)))return false;
    if(sourcePublication&&(!sourceHooks||sourcePublication!=&source||!g.write(sourcePublication,1)))return false;
    if(!g.read(&frame,1)||!g.read(&bindings,1)||!g.read(&admission,1)||!g.read(&live,1)||
       (!sourcePublication&&!g.read(&source,1))||!g.read(&initial,1)||!g.read(&w,1)||!g.read(&d,1)||
       !g.read(&world,1)||!g.read(world.portalHeads,world.zoneCount)||!g.read(world.portals,world.portalCount)||
       !g.read(owners,ownerCount)||!g.read(indices,count)||!g.read(source.resource,1)||
       !g.read(source.filter,1)||!g.read(initial.pairs,initial.pairSlots)||
       !g.read(initial.metadata,initial.pairSlots)||!g.read(initial.recordSlots,initial.recordCount))return false;
    if(tcOwner&&!nativeTriggerQueryBound50TC(*tcOwner,source.filter->identity))return false;
    if(tbOwner&&!nativeTriggerQueryBound50TB(*tbOwner,source.filter->identity))return false;
    if(!g.read(source.resource->nodes,source.resource->nodeCount)||
       !g.read(source.resource->rays,source.resource->rayCount)||!g.filter(*source.filter))return false;
    // The write guard is complete, and this admission pass performs no mesh
    // mutations or callbacks. Reuse only complete successes within this call.
    struct MeshSpanAdmission {
        const assets::CollisionMesh48I* mesh;
        const float* radii;
        std::size_t radiusCount;
    };
    constexpr std::size_t meshSpanCapacity=8;
    MeshSpanAdmission admittedMeshes[meshSpanCapacity]{};
    std::size_t admittedMeshCount=0;
    for(std::size_t i=0;i<count;++i){
        if(indices[i]>=ownerCount)return false;
        const auto& owner=owners[indices[i]];const auto& candidate=owner.candidate;
        if(!g.filter(candidate.collision.filter)||!g.read(candidate.sceneName,candidate.sceneNameBytes))return false;
        const auto readMesh=[&](const assets::CollisionMesh48I* mesh,const float* radii,
            std::size_t radiusCount) noexcept {
            for(std::size_t j=0;j<admittedMeshCount;++j){
                const auto& admitted=admittedMeshes[j];
                if(admitted.mesh==mesh&&admitted.radii==radii&&
                   admitted.radiusCount==radiusCount)return true;
            }
            if(!g.read(mesh,1)||!g.read(radii,radiusCount))return false;
            if(!(g.read(mesh->nodes.data(),mesh->nodes.size())&&g.read(mesh->faces.data(),mesh->faces.size())&&
                 g.read(mesh->vertices.data(),mesh->vertices.size())&&
                 g.read(mesh->vertex_normals.data(),mesh->vertex_normals.size())))return false;
            // A full cache never rejects an otherwise valid mesh: misses still
            // execute every original span check, in the original order.
            if(admittedMeshCount<meshSpanCapacity)
                admittedMeshes[admittedMeshCount++]={mesh,radii,radiusCount};
            return true;
        };
        if(owner.shapeType==0){
            if(!readMesh(candidate.collision.geometry.mesh,owner.nodeRadii,owner.nodeRadiusCount))return false;
        }else if(owner.shapeType==1){
            // Admit every nested immutable span before the 50EN membership
            // prefix can mutate any of its buffers. No placeholder mesh read.
            const auto* compound=owner.compound;
            if(!g.read(compound,1)||compound->count>maxCompoundMembers50IJ||
               !g.read(compound->parts,compound->count))return false;
            for(std::size_t j=0;j<compound->count;++j){const auto& part=compound->parts[j];
                if(!readMesh(part.mesh,part.nodeRadii,part.nodeRadiusCount))return false;
            }
        }else if(owner.shapeType==5||owner.shapeType==6){
            if(!source.nativeGatePolicy||(owner.shapeType==6&&!validNativeTriggerBox50MA(owner.nativeTriggerBox)))return false;
            if(owner.gate){
                if(owner.gate!=gateOwner||!owner.gate->callbacks.activateAnimation)return false;
            }else if(owner.nativeTriggersTC){
                if(owner.nativeTriggersTC!=tcOwner||!tcOwner||owner.nativeTriggerIndex>=tcOwner->triggers.size()||
                   candidate.collision.filter.identity!=tcOwner->triggers[owner.nativeTriggerIndex].collider.identity||
                   !nativeTriggerQueryBound50TC(*tcOwner,source.filter->identity))return false;
            }else if(owner.nativeTriggersTB){
                if(owner.nativeTriggersTB!=tbOwner||!tbOwner||owner.nativeTriggerIndex>=tbOwner->triggers.size()||
                   candidate.collision.filter.identity!=tbOwner->triggers[owner.nativeTriggerIndex].collider.identity||
                   !nativeTriggerQueryBound50TB(*tbOwner,source.filter->identity))return false;
            }else if(owner.nativeTriggers!=triggerOwner||!triggerOwner||
                owner.nativeTriggerIndex>=triggerOwner->triggers.size()||
                candidate.collision.filter.identity!=triggerOwner->triggers[owner.nativeTriggerIndex].collider.identity||
                nativeTriggerCollider50GA(*triggerOwner,source.filter->identity)==triggerOwner->colliders.size())return false;
        }else return false;
    }
    return true;
}

VehicleLiveQueryResult50EQ query(bool secondary,const VehicleFrameState49W& frame,
    const VehicleFrameBindings49W& bindings,const MainStaticDetectorAdmission50H& admission,
    const VehicleLiveQueryContext50EQ& live,const Type2StaticQuery49X& source,
    const Type2StaticCandidate49X* owners,std::size_t ownerCount,const std::size_t* indices,
    std::size_t count,const Type2StaticInitial49X& initial,const Type2StaticWorkspace49X& w,
    const Type2StaticDestination49X& d,Type2StaticResult49X& result,Type2StaticQuery49X* sourcePublication=nullptr,
    const SourceTriggerActionHooks50MB* sourceHooks=nullptr) noexcept {
    using S=Type2StaticStatus49X;using L=VehicleLiveQueryStatus50EQ;
    VehicleLiveQueryResult50EQ outcome{};
    if(!storage(frame,bindings,admission,live,source,owners,ownerCount,indices,count,initial,w,d,result,sourcePublication,sourceHooks))return outcome;
    if(frame.phase!=VehicleFramePhase49W::PostWorld)return outcome;
    if((secondary&&(frame.bodies.main.substepIndex!=0||!frame.bodies.vehicle838))||
       !admission.springPresent||!admission.queryPresent||!admission.queryEnabled||admission.pairsPresent||
       (admission.providerPresent&&admission.pointContactsEnabled)||!admission.explicitCandidates||
       admission.queryType!=2||!admission.registrationE0){
        outcome.queryStatus=S::UnsupportedScope;return outcome;
    }
    auto& membership=live.registrationE0;
    if(!live.expectedObjectRef||!membership.attached||membership.object!=live.expectedObjectRef)return outcome;
    const auto scene=secondary?bindings.lifecycle.physical19C:bindings.lifecycle.main190;
    const auto index=static_cast<int>(scene);
    if(!validVehicleSceneCount49W(frame,bindings)||index<0||index>=static_cast<int>(frame.sceneNodeCount)||admission.queryScene!=scene||!source.queryScene.value)return outcome;
    const auto& node=frame.nodes[index];
    if(!(node.sceneFlags190&1u)){outcome.queryStatus=S::UnsupportedScope;return outcome;}
    const auto& bodyWorld=secondary?frame.bodies.secondary.root.scene:frame.bodies.main.components.root.scene;
    if(std::memcmp(&node.world100,&bodyWorld,sizeof node.world100))return outcome;

    // Same current canonical matrix/root preparation as the zero-E0 50CD paths.
    // Do not call those paths with a fabricated zero registration marker.
    outcome.status=L::PreparationRejected;
    const auto& resource=*source.resource;
    if((resource.root&0x8000u)||!resource.nodeCount){outcome.queryStatus=S::UnsupportedScope;return outcome;}
    if(resource.root>=resource.nodeCount)return outcome;
    const auto& root=resource.nodes[resource.root];QuerySpherePrepareInput48O input{};
    input.preparedScene=node.world100;input.shapeType=2;input.hasPairTree=true;
    for(unsigned i=0;i<3;++i){input.treeCenter.v[i]=root.center[i];input.treeHalf.v[i]=root.half[i];}
    QuerySpherePrepareOutput48O prepared{};
    const auto prep=prepareQuerySphere48O(input,prepared);
    if(prep!=QuerySpherePrepareStatus48O::Ready){
        outcome.queryStatus=prep==QuerySpherePrepareStatus48O::UnsupportedScope?S::UnsupportedScope:S::InvalidInput;
        return outcome;
    }
    Type2StaticQuery49X preparedQuery;std::memcpy(&preparedQuery,&source,sizeof preparedQuery);
    if(source.nativeGatePolicy){
        // Source worldMatrix is the previous accepted canonical query cache.
        // Racer-bank ownership already keeps it separate for every body/racer.
        preparedQuery.nativeGatePreviousWorld=source.worldMatrix;
        preparedQuery.nativeGateHasPrevious=true;
    }
    std::memcpy(&preparedQuery.worldMatrix,&prepared.world,sizeof preparedQuery.worldMatrix);
    for(unsigned i=0;i<3;++i)preparedQuery.sphere.center.v[i]=prepared.center.v[i];
    preparedQuery.sphere.radius=prepared.radius;preparedQuery.resetRecordsBefore=admission.resetSpring;

    LiveZoneBounds50EN bounds{};bounds.bounds.shapeTag=2;
    std::memcpy(&bounds.bounds.shapeWorld,&prepared.world,sizeof prepared.world);
    std::memcpy(&bounds.bounds.cachedCenter,&prepared.center,sizeof prepared.center);
    std::memcpy(&bounds.bounds.cachedRadius,&prepared.radius,sizeof prepared.radius);
    bounds.type2Resource=source.resource;
    const auto beforeRevision=membership.revision;
    outcome.prefix=updateLiveZoneMembership50EN(live.lifecycle,live.world,membership,bounds,live.workspace);
    outcome.membershipRevision=membership.revision;
    if(outcome.prefix.status!=LiveZoneStatus50EN::Complete){outcome.status=L::PrefixRejected;return outcome;}
    outcome.prefixCommitted=true;
    if(!membership.attached||membership.object!=live.expectedObjectRef||membership.lastShapeTag!=2||
       membership.lastType2Resource!=source.resource||beforeRevision==std::numeric_limits<std::uint64_t>::max()||
       membership.revision!=beforeRevision+1u||
       std::memcmp(&membership.lastShapeWorld,&prepared.world,sizeof prepared.world)||
       std::memcmp(&membership.bounds.cachedCenter,&prepared.center,sizeof prepared.center)||
       std::memcmp(&membership.bounds.cachedRadius,&prepared.radius,sizeof prepared.radius)){
        outcome.status=L::ReceiptRejected;return outcome;
    }
    if(sourcePublication){
        *sourcePublication=preparedQuery;outcome.queryStatus=S::Complete;outcome.status=L::Complete;return outcome;
    }
    // The collected owner list stays in its original order even after transfer.
    outcome.queryStatus=produceIndexedStaticType2Contacts50CC(preparedQuery,owners,ownerCount,indices,count,initial,w,d,result);
    outcome.status=outcome.queryStatus==S::Complete?L::Complete:L::QueryRejected;
    return outcome;
}
} // namespace

VehicleLiveQueryResult50EQ prepareSourceLiveQuery50MB(bool secondary,const VehicleFrameState49W& frame,
    const VehicleFrameBindings49W& bindings,const MainStaticDetectorAdmission50H& admission,
    const VehicleLiveQueryContext50EQ& live,Type2StaticQuery49X& queryOwner,
    const Type2StaticCandidate49X* owners,std::size_t ownerCount,const std::size_t* indices,std::size_t count,
    const Type2StaticInitial49X& initial,const Type2StaticWorkspace49X& w,const Type2StaticDestination49X& d,
    Type2StaticResult49X& result,const SourceTriggerActionHooks50MB& hooks) noexcept {
    return query(secondary,frame,bindings,admission,live,queryOwner,owners,ownerCount,indices,count,initial,w,d,result,&queryOwner,&hooks);
}

SecondaryQuerySceneStatus50J prepareSecondaryLiveQueryScene50EQ(
    VehicleFrameState49W& s,const VehicleFrameBindings49W& b,
    const MainStaticDetectorAdmission50H& a,const VehicleLiveQueryContext50EQ& live,
    VehicleFrameWorkspace49W& w) noexcept {
    using S=SecondaryQuerySceneStatus50J;
    Guard g;const auto& life=live.lifecycle;const auto& world=live.world;
    const auto& membership=live.registrationE0;const auto& work=live.workspace;
    if(!g.write(&s,1)||!g.write(&w,1)||!g.read(&b,1)||!g.read(&a,1)||!g.read(&live,1)||
       !g.read(&life,1)||!g.read(&world,1)||!g.read(&membership,1)||!g.read(&work,1)||
       !g.read(life.storage.registries,life.storage.registryCapacity)||
       !g.read(life.storage.registrations,life.storage.registrationCapacity)||
       !g.read(life.storage.views.methods,life.storage.views.methodCapacity)||
       !g.read(life.storage.views.zones,life.storage.views.zoneCapacity)||
       !g.read(life.storage.views.blocks,life.storage.views.blockCapacity)||
       !g.read(membership.storage.links,membership.storage.capacity)||
       !g.read(membership.storage.entries,membership.storage.capacity)||
       !g.read(work.registries,work.registryCapacity)||!g.read(work.links,work.membershipCapacity)||
       !g.read(work.entries,work.membershipCapacity)||!g.read(work.portalLinks,work.portalLinkCapacity)||
       !g.read(world.portalHeads,world.zoneCount)||!g.read(world.portals,world.portalCount)||
       !g.read(world.portalLinks,world.portalLinkCount))return S::InvalidInput;
    if(membership.lastType2Resource){
        if(!g.read(membership.lastType2Resource,1))return S::InvalidInput;
        const auto& resource=*membership.lastType2Resource;
        if(!g.read(resource.nodes,resource.nodeCount)||!g.read(resource.rays,resource.rayCount))return S::InvalidInput;
    }
    if(s.phase!=VehicleFramePhase49W::PostWorld||s.bodies.main.substepIndex!=0||!s.bodies.vehicle838)return S::UnsupportedScope;
    if(!a.springPresent||!a.queryPresent||!a.queryEnabled||a.pairsPresent||(a.providerPresent&&a.pointContactsEnabled)||
       !a.explicitCandidates||a.queryType!=2||!a.registrationE0)return S::UnsupportedScope;
    if(!live.expectedObjectRef||!membership.attached||membership.object!=live.expectedObjectRef)return S::InvalidInput;
    const auto index=static_cast<int>(b.lifecycle.physical19C);
    if(!validVehicleSceneCount49W(s,b)||index<0||index>=static_cast<int>(s.sceneNodeCount)||a.queryScene!=b.lifecycle.physical19C)return S::InvalidInput;
    const auto& node=s.nodes[index];const auto& body=s.bodies.secondary;
    if(node.parent180!=SceneNodeIndex49P::none||s.bodies.secondaryDomain.sceneHasParent||
       s.bodies.secondaryDomain.sceneObserverCount||b.secondaryDomain.rootObserverPresent19C)return S::UnsupportedScope;
    if(std::memcmp(&node.world100,&body.root.scene,sizeof node.world100)||
       (body.sceneFlags190&255u)!=node.sceneFlags190)return S::InvalidInput;
    std::memcpy(&w.next,&s,sizeof s);
    if(!refreshSceneWorld49P({w.next.nodes,w.next.sceneNodeCount},b.lifecycle.physical19C))return S::GraphFailure;
    const auto& ready=w.next.nodes[index];auto& next=w.next.bodies.secondary;
    std::memcpy(&next.root.scene,&ready.world100,sizeof next.root.scene);
    next.sceneFlags190=(next.sceneFlags190&0xffffff00u)|ready.sceneFlags190;
    std::memcpy(&s,&w.next,sizeof s);return S::Ready;
}

VehicleLiveQueryResult50EQ queryMainLiveIndexedStaticVehicle50EQ(
    const VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const MainStaticDetectorAdmission50H& admission,const VehicleLiveQueryContext50EQ& live,
    const Type2StaticQuery49X& source,const Type2StaticCandidate49X* owners,std::size_t ownerCount,
    const std::size_t* indices,std::size_t count,const Type2StaticInitial49X& initial,
    const Type2StaticWorkspace49X& w,const Type2StaticDestination49X& d,Type2StaticResult49X& result) noexcept {
    return query(false,frame,bindings,admission,live,source,owners,ownerCount,indices,count,initial,w,d,result);
}
VehicleLiveQueryResult50EQ querySecondaryLiveIndexedStaticVehicle50EQ(
    const VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const MainStaticDetectorAdmission50H& admission,const VehicleLiveQueryContext50EQ& live,
    const Type2StaticQuery49X& source,const Type2StaticCandidate49X* owners,std::size_t ownerCount,
    const std::size_t* indices,std::size_t count,const Type2StaticInitial49X& initial,
    const Type2StaticWorkspace49X& w,const Type2StaticDestination49X& d,Type2StaticResult49X& result) noexcept {
    return query(true,frame,bindings,admission,live,source,owners,ownerCount,indices,count,initial,w,d,result);
}
} // namespace rr::game
