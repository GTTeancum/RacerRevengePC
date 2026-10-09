#include "vehicle_indexed_query50cd.h"
#include "query_prepare48o.h"
#include <cstring>
#include <limits>
namespace rr::game {
namespace {
struct Span {std::uintptr_t a=0,z=0;};
template<class T>bool span(const T*p,std::size_t n,Span&s) noexcept {
    s={};if(!n)return true;if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);if(bytes>std::numeric_limits<std::uintptr_t>::max()-a)return false;s={a,a+bytes};return true;
}
struct Protect {
    Span reads[4]{};
    template<class T>bool write(T*p,std::size_t n) const noexcept {Span s;if(!span(p,n,s))return false;for(auto r:reads)if(s.a<r.z&&r.a<s.z)return false;return true;}
};
}
Type2StaticStatus49X querySecondaryIndexedStaticVehicle50CD(const VehicleFrameState49W&frame,
    const VehicleFrameBindings49W&bindings,const MainStaticDetectorAdmission50H&admission,
    const Type2StaticQuery49X&source,const Type2StaticCandidate49X*candidates,std::size_t ownerCount,const std::size_t*indices,std::size_t count,
    const Type2StaticInitial49X&initial,const Type2StaticWorkspace49X&w,
    const Type2StaticDestination49X&d,Type2StaticResult49X&result) noexcept {
    using S=Type2StaticStatus49X;
    Protect protect;
    if(!span(&frame,1,protect.reads[0])||!span(&bindings,1,protect.reads[1])||!span(&admission,1,protect.reads[2])||!span(&source,1,protect.reads[3])||
       !protect.write(w.pairs,w.pairCapacity)||!protect.write(w.metadata,w.metadataCapacity)||
       !protect.write(w.records,w.recordCapacity)||!protect.write(w.effects,w.effectCapacity)||
       !protect.write(w.stack,w.stackCapacity)||!protect.write(w.trace,w.traceCapacity)||
       !protect.write(d.pairs,d.pairCapacity)||!protect.write(d.metadata,d.metadataCapacity)||
       !protect.write(d.records,d.recordCapacity)||!protect.write(d.effects,d.effectCapacity)||!protect.write(&result,1))return S::InvalidInput;
    if(frame.phase!=VehicleFramePhase49W::PostWorld)return S::InvalidInput;
    if(frame.bodies.main.substepIndex!=0||!frame.bodies.vehicle838)return S::UnsupportedScope;
    if(!admission.springPresent||!admission.queryPresent||!admission.queryEnabled||admission.pairsPresent||
       (admission.providerPresent&&admission.pointContactsEnabled)||!admission.explicitCandidates||
       admission.queryType!=2||admission.registrationE0)return S::UnsupportedScope;
    const auto index=static_cast<int>(bindings.lifecycle.physical19C);
    if(index<0||index>=static_cast<int>(vehicleFrameNodeCount49W)||admission.queryScene!=bindings.lifecycle.physical19C||!source.queryScene.value)return S::InvalidInput;
    const auto&node=frame.nodes[index];
    if(!(node.sceneFlags190&1u))return S::UnsupportedScope;
    if(std::memcmp(&node.world100,&frame.bodies.secondary.root.scene,sizeof node.world100))return S::InvalidInput;
    const auto*resource=source.resource;
    Span resourceSpan{},nodeSpan{};
    if(!span(resource,1,resourceSpan))return S::InvalidInput;
    if((resource->root&0x8000u)||!resource->nodeCount)return S::UnsupportedScope;
    if(!span(resource->nodes,resource->nodeCount,nodeSpan)||resource->root>=resource->nodeCount)return S::InvalidInput;
    const auto&root=resource->nodes[resource->root];QuerySpherePrepareInput48O input{};
    input.preparedScene=node.world100;input.shapeType=2;input.hasPairTree=true;
    for(unsigned i=0;i<3;++i){input.treeCenter.v[i]=root.center[i];input.treeHalf.v[i]=root.half[i];}
    QuerySpherePrepareOutput48O prepared{};
    const auto prep=prepareQuerySphere48O(input,prepared);
    if(prep!=QuerySpherePrepareStatus48O::Ready)return prep==QuerySpherePrepareStatus48O::UnsupportedScope?S::UnsupportedScope:S::InvalidInput;
    Type2StaticQuery49X query;std::memcpy(&query,&source,sizeof query);
    std::memcpy(&query.worldMatrix,&prepared.world,sizeof query.worldMatrix);
    for(unsigned i=0;i<3;++i)query.sphere.center.v[i]=prepared.center.v[i];
    query.sphere.radius=prepared.radius;query.resetRecordsBefore=admission.resetSpring;
    return produceIndexedStaticType2Contacts50CC(query,candidates,ownerCount,indices,count,initial,w,d,result);
}
}
