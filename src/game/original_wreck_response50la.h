#pragma once
#include "original_wreck_owner50la.h"
#include "chariot_scene49u.h"
#include "spring_force48l.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
enum class OriginalWreckResponseStatus50LA : std::uint8_t {
    complete,owner,storage,graph,pointContacts,dynamicBody,sceneBinding,sourceGlobal,arithmetic,impact,springForce,friction
};
struct OriginalWreckResponseResult50LA {
    OriginalWreckResponseStatus50LA status=OriginalWreckResponseStatus50LA::owner;
    std::uint32_t sourcePC=0;
    std::size_t recordsCompleted=0,positiveRecords=0;
    bool centerCommitted=false,velocityCacheCommitted=false,pendingCleared=false;
};
namespace original_wreck_response_detail50LA {
using V=Vector3_48C;
struct Math {
    bool valid=true;
    float keep(float x)noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b)noexcept {keep(a);keep(b);volatile float r=a+b;return keep(r);}
    float sub(float a,float b)noexcept {keep(a);keep(b);volatile float r=a-b;return keep(r);}
    float mul(float a,float b)noexcept {keep(a);keep(b);volatile float r=a*b;return keep(r);}
    V point(const BodyMatrix48G& m,const V& p)noexcept {
        V out{};for(unsigned i=0;i<3;++i)
            out.v[i]=add(add(add(mul(m.v[i],p.v[0]),mul(m.v[4+i],p.v[1])),mul(m.v[8+i],p.v[2])),m.v[12+i]);
        return out;
    }
    V cross(const V& a,const V& b)noexcept {
        V out{};for(unsigned i=0;i<3;++i)
            out.v[i]=sub(mul(a.v[(i+1)%3],b.v[(i+2)%3]),mul(a.v[(i+2)%3],b.v[(i+1)%3]));
        return out;
    }
    float dot(const V& a,const V& b)noexcept {
        return add(add(mul(a.v[0],b.v[0]),mul(a.v[1],b.v[1])),mul(1.f,mul(a.v[2],b.v[2])));
    }
};
inline V xyz(const SpringVector4_48N& a)noexcept {V v;std::memcpy(v.v,a.v,12);return v;}
inline V xyz(const VisualVector49R& a)noexcept {V v;std::memcpy(v.v,a.v,12);return v;}
inline VisualVector49R vector(const V& a)noexcept {return {{a.v[0],a.v[1],a.v[2],1.f}};}
inline void sentinel(VisualLazyWord49R& v)noexcept {if(!v.guard){v.word=0xffffffffu;v.guard=1;}}
struct Span {std::uintptr_t first=0,last=0;};
template<class T>inline bool span(const T* p,std::size_t count,Span& s)noexcept {
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||count>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto start=reinterpret_cast<std::uintptr_t>(p),bytes=count*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-start)return false;
    s={start,start+bytes};return true;
}
inline bool overlap(Span a,Span b)noexcept {return a.first<b.last&&b.first<a.last;}
inline bool sentinelStorage(OriginalOnePieceWreckOwner50LA& o,VisualLazyWord49R& point,
    VisualLazyWord49R& generated)noexcept {
    Span owner{},a{},b{};
    return span(&o,1,owner)&&span(&point,1,a)&&span(&generated,1,b)&&
        !overlap(owner,a)&&!overlap(owner,b)&&!overlap(a,b);
}
inline bool storage(OriginalOnePieceWreckOwner50LA& o,const SceneGraph49P& graph,
    VisualLazyWord49R& point,VisualLazyWord49R& generated)noexcept {
    Span ranges[4];
    if(!span(&o,1,ranges[0])||!span(graph.nodes,graph.count,ranges[1])||
       !span(&point,1,ranges[2])||!span(&generated,1,ranges[3]))return false;
    for(unsigned i=0;i<4;++i)for(unsigned j=0;j<i;++j)if(overlap(ranges[i],ranges[j]))return false;
    Span source{};if(!span(o.contactVector4435A0(),1,source))return false;
    for(const auto& range:ranges)if(overlap(source,range))return false;
    return true;
}
inline void projection(OriginalWreckPhysical50LA& p,const SceneGraph49P& graph)noexcept {
    const auto& n=graph.nodes[static_cast<std::uint32_t>(p.model288)];
    std::memcpy(&p.body.root.scene,&n.world100,sizeof p.body.root.scene);
    p.body.sceneFlags190=(p.body.sceneFlags190&0xffffff00u)|n.sceneFlags190;
}
inline SpringImpactBody48K primary(const OriginalWreckPhysical50LA& p)noexcept {
    SpringImpactBody48K out;
    std::memcpy(&out.state,&p.body.root,sizeof out.state);
    std::memcpy(&out.parameters,&p.parameters.body,sizeof out.parameters);
    std::memcpy(&out.localCenter,&p.parameters.centerOffset,sizeof out.localCenter);
    return out;
}
// Commit only source response fields. These are XYZ-only stores: D0 fourth
// lanes, histories, cached accelerations, transforms and callback238 survive.
inline void impact(OriginalWreckPhysical50LA& p,const RootBodyState48H& next)noexcept {
    auto& b=p.body.root;
    b.accumulator1c0=next.accumulator1c0;b.body.velocity=next.body.velocity;
    b.accumulator1d0=next.accumulator1d0;b.body.angularVelocity=next.body.angularVelocity;
    b.loads.force=next.loads.force;b.loads.torque=next.loads.torque;
    b.positionCorrection1a0=next.positionCorrection1a0;b.rotationCorrection1b0=next.rotationCorrection1b0;
}
}

namespace original_wreck_response_detail50LA {
// Legacy adapter preserves its existing single-owner admission and identity
// provider. The common kernel never substitutes a response-only scene map.
struct SingleContactContext {
    OriginalOnePieceWreckOwner50LA& owner;
    OriginalWreckPhysical50LA& contact()noexcept{return owner.physical;}
    OriginalWreckPhysical50LA& body()noexcept{return owner.physical;}
    bool bound()const noexcept{return owner.physical.contactConstructorFieldsApplied&&
        owner.physical.contactBody30==&owner.physical.body;}
    bool ready()const noexcept{return owner.prepared()&&owner.physical.bodyConstructorFieldsApplied&&
        owner.physical.model288==owner.assets.model14C();}
    SceneGraph49P graph()noexcept{return owner.assets.graph();}
    const SpringVector4_48N* zero()const noexcept{return owner.contactVector4435A0();}
    bool sentinels(VisualLazyWord49R& a,VisualLazyWord49R& b)noexcept{return sentinelStorage(owner,a,b);}
    bool storageBound(const SceneGraph49P& g,VisualLazyWord49R& a,VisualLazyWord49R& b)noexcept {
        return storage(owner,g,a,b);
    }
    bool recordFrame(SpringSceneId48N id,SceneGraph49P& g,SceneNodeIndex49P& model)noexcept {
        if(id.value&&id.value!=owner.modelSceneIdentity().value)return false;
        g=owner.assets.graph();model=owner.physical.model288;return true;
    }
};
// Actual2964C0 static generated-record route, borrowing the SAME DC8C records,
// D0 body and graph288 as295630. No caller contacts, response receipt or second
// mutable physics owner. The no-record route retains its original non-reading
// behavior. Point-terrain and reached dynamic bodies remain explicit guards.
//
// Raw world100 is intentionally read before any scene refresh.296F10 refreshes
// through the actual28BBB0/28B9B0 graph helpers only after positive depth/flags.
// Positive records run serially: prepare cached120 once, impact, secondary,
// spring force, friction with that SAME pre-impact velocity. Next records and
// substeps see earlier canonical writes. No record is removed or synthesized.
//
// Strict FE_TONEAREST and the existing48J/K/L finite binary32 numeric domain,
// separate operations/no contraction. A helper's numerical failure is atomic
// for that helper; earlier source stages and records stay committed. The caller
// must stop, not replay this call or call the integration suffix after failure.
template<class Context>
inline OriginalWreckResponseResult50LA advanceContact2964C0(
    Context& context,VisualLazyWord49R& pointSentinel4435E0,
    VisualLazyWord49R& generatedSentinel4435E8)noexcept {
    using namespace original_wreck_response_detail50LA;
    using S=OriginalWreckResponseStatus50LA;OriginalWreckResponseResult50LA r;
    auto fail=[&](S s,std::uint32_t pc){r.status=s;r.sourcePC=pc;return r;};
    auto& p=context.contact();auto& b=context.body();
    auto& state=p.spring;auto& settings=p.generated;
    if(!context.bound())return fail(S::owner,0x2964c0u);
    if(!context.sentinels(pointSentinel4435E0,generatedSentinel4435E8))return fail(S::storage,0x2964c0u);
    if(settings.pointContacts48)return fail(S::pointContacts,0x2965bcu);
    if(state.generatedCount84<=0){
        sentinel(pointSentinel4435E0);sentinel(generatedSentinel4435E8);
        state.pending38=0;r.pendingCleared=true;r.status=S::complete;r.sourcePC=0x296bf8u;return r;
    }
    if(state.generatedCapacity88<state.generatedCount84||
       static_cast<std::size_t>(state.generatedCapacity88)>p.records.size())return fail(S::storage,0x296500u);
    if(!context.ready())return fail(S::owner,0x296518u);
    const auto graph=context.graph();const auto model=static_cast<std::int32_t>(b.model288);
    if(!validateSceneGraph49P(graph)||model<0||static_cast<std::uint32_t>(model)>=graph.count)
        return fail(S::graph,0x296524u);
    if(!context.storageBound(graph,pointSentinel4435E0,generatedSentinel4435E8))return fail(S::storage,0x2964c0u);
    if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,0x29652cu);
    Math math;
    const V center=math.point(graph.nodes[model].world100,settings.impact.customCenter70?
        settings.impact.customLocalCenter100:b.parameters.centerOffset);
    if(!math.valid)return fail(S::arithmetic,0x296598u);
    settings.impact.worldCenter110=center;p.worldCenter110W=1.f;r.centerCommitted=true;
    sentinel(pointSentinel4435E0);sentinel(generatedSentinel4435E8);
    for(std::int32_t i=0;i<state.generatedCount84;++i){
        auto& record=p.records[static_cast<std::size_t>(i)];
        SceneGraph49P recordGraph{};SceneNodeIndex49P recordModel=SceneNodeIndex49P::none;
        if(!context.recordFrame(record.scene40,recordGraph,recordModel))
            return fail(S::sceneBinding,0x296884u);
        //296884 nonzero scene40 and2968CC zero fallback are distinct owners.
        // Neither path refreshes a scene before this cached-matrix read.
        const auto recordIndex=static_cast<std::int32_t>(recordModel);
        if(!recordGraph.nodes||recordIndex<0||static_cast<std::uint32_t>(recordIndex)>=recordGraph.count)
            return fail(S::sceneBinding,0x296884u);
        const V world=math.point(recordGraph.nodes[recordIndex].world100,xyz(record.localPoint10));
        const V normal=xyz(record.normal20),surface=xyz(record.surfacePoint30);
        V separation{};for(unsigned axis=0;axis<3;++axis)separation.v[axis]=math.sub(surface.v[axis],world.v[axis]);
        const float depth=math.dot(separation,normal);
        if(!math.valid)return fail(S::arithmetic,0x296980u);
        if(depth<=0.f){record.flags50=static_cast<std::uint8_t>(record.flags50&~2u);++r.recordsCompleted;continue;}
        state.contact3C=1;state.generated78=1;
        if(record.other44.value)state.other44=1;
        else if(normal.v[1]>.8f)state.upward40=1;
        record.flags50=static_cast<std::uint8_t>(record.flags50|2u);++r.positiveRecords;
        V point{};for(unsigned axis=0;axis<3;++axis)point.v[axis]=math.add(world.v[axis],math.mul(depth,normal.v[axis]));
        if(!math.valid)return fail(S::arithmetic,0x296a50u);
        if(record.other44.value)return fail(S::dynamicBody,0x296a58u);
        //296F10 uses actual graph owners, and publishes lazy scene refresh even
        //if a later numeric operation fails. Stack direction W is overwritten
        //by the final scalar cache construction and never enters body state.
        V arm{};for(unsigned axis=0;axis<3;++axis)arm.v[axis]=math.sub(point.v[axis],center.v[axis]);
        if(!math.valid)return fail(S::arithmetic,0x296fa8u);
        detail::SceneVectorContext49U sceneContext{1.f};VisualVector49R local{},rotational{};
        const bool localOkay=detail::inverseDirection49U(graph,b.model288,vector(arm),sceneContext,local);
        projection(b,graph);
        if(!localOkay)return fail(S::graph,0x296fb4u);
        const V spin=math.cross(b.body.root.body.angularVelocity,xyz(local));
        if(!math.valid)return fail(S::arithmetic,0x296fe8u);
        const bool worldOkay=detail::direction49U(graph,b.model288,vector(spin),sceneContext,rotational);
        projection(b,graph);
        if(!worldOkay)return fail(S::graph,0x296ff4u);
        // The released static leaves use source4435A0's initialized XYZ +0
        //for no-other velocity/acceleration and their zero-point sentinel.
        //Authenticate that SAME application binding before consuming it.
        std::uint32_t zero[3];std::memcpy(zero,context.zero()->v,sizeof zero);
        if(zero[0]||zero[1]||zero[2])return fail(S::sourceGlobal,0x29703cu);
        V velocity{};for(unsigned axis=0;axis<3;++axis)
            velocity.v[axis]=math.sub(math.add(rotational.v[axis],b.body.root.body.velocity.v[axis]),0.f);
        if(!math.valid)return fail(S::arithmetic,0x297178u);
        settings.impact.contactVelocity120=velocity;p.contactVelocity120W=1.f;r.velocityCacheCommitted=true;
        if(settings.impact54){
            const auto body=primary(b);SpringImpactResult48K next{};
            if(!applySpringImpact48K(normal,point,depth,settings.impact,body,nullptr,next))return fail(S::impact,0x296ae0u);
            impact(b,next.primary);
        }
        auto secondary=[&]()noexcept {
            V next{};if(!removeSpringInwardVelocity48L(normal,velocity,b.body.root.body.velocity,next))return false;
            b.body.root.body.velocity=next;return true;
        };
        if(settings.secondary68&&!secondary())return fail(S::arithmetic,0x296b00u);
        if(settings.springForce4C){
            if(p.response.secondaryAtLimit60){
                if(!std::isfinite(p.response.maxPenetration90))return fail(S::arithmetic,0x296b24u);
                if(depth>=p.response.maxPenetration90&&!settings.impact54&&!secondary())return fail(S::arithmetic,0x296b4cu);
            }
            const auto body=primary(b);const auto& a=settings.impact;const auto& response=p.response;
            const SpringPointForceParameters48J force{b.parameters.centerOffset,a.customLocalCenter100,a.forceTorqueScaleB4,a.customCenter70,a.yawOnly6C};
            const SpringForceParameters48L input{velocity,force,response.maxPenetration90,response.maxSpeed94,response.stiffness9C,response.dampingA0,
                response.impactStiffnessA4,response.impactDampingA8,settings.impact54,settings.otherSpring58};
            SpringFrictionResult48J next{};
            if(!applySpringForce48L(normal,point,depth,input,body,nullptr,next))return fail(S::springForce,0x296b68u);
            b.body.root.loads.force=next.primary.force;b.body.root.loads.torque=next.primary.torque;
        }
        if(settings.friction50){
            if(record.material4C>=settings.frictionByMaterial.size())return fail(S::storage,0x296b94u);
            const auto& a=settings.impact;
            const SpringPointForceParameters48J force{b.parameters.centerOffset,a.customLocalCenter100,settings.frictionTorqueScaleC4,a.customCenter70,a.yawOnly6C};
            const SpringFrictionBody48J body{b.body.root.scene,b.body.root.loads,b.parameters.body.mass};
            SpringFrictionResult48J next{};
            if(!applySpringFriction48J(normal,point,velocity,settings.frictionByMaterial[record.material4C],force,body,nullptr,next))
                return fail(S::friction,0x296b98u);
            b.body.root.loads.force=next.primary.force;b.body.root.loads.torque=next.primary.torque;
        }
        ++r.recordsCompleted;
    }
    state.pending38=0;r.pendingCleared=true;r.status=S::complete;r.sourcePC=0x296bf8u;return r;
}
} // namespace original_wreck_response_detail50LA
inline OriginalWreckResponseResult50LA advanceOriginalWreckContact2964C0_50LA(
    OriginalOnePieceWreckOwner50LA& owner,VisualLazyWord49R& pointSentinel4435E0,
    VisualLazyWord49R& generatedSentinel4435E8)noexcept {
    original_wreck_response_detail50LA::SingleContactContext context{owner};
    return original_wreck_response_detail50LA::advanceContact2964C0(context,pointSentinel4435E0,generatedSentinel4435E8);
}
} // namespace rr::game
