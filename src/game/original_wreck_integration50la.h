#pragma once
#include "original_wreck_physical50la.h"
#include "original_wreck_visibility50la.h"
#include <cmath>

namespace rr::game {
namespace original_wreck_integration_detail50LA {
using V=VisualVector49R;
inline V vector(const Vector3_48C& v,float w=0.f) noexcept {return {{v.v[0],v.v[1],v.v[2],w}};}
inline void xyz(Vector3_48C& out,const V& in) noexcept {std::memcpy(out.v,in.v,12);}
struct Math {
    bool valid=true;
    float keep(float v) noexcept {valid=valid&&std::isfinite(v);return v;}
    float add(float a,float b) noexcept {keep(a);keep(b);volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {keep(a);keep(b);volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {keep(a);keep(b);volatile float r=a*b;return keep(r);}
    float div(float a,float b) noexcept {keep(a);keep(b);volatile float r=a/b;return keep(r);}
    float length(const V& v) noexcept {
        const float xx=mul(v.v[0],v.v[0]),yy=mul(v.v[1],v.v[1]);
        const float n=add(add(xx,yy),mul(v.v[2],v.v[2]));std::uint32_t bits;
        std::memcpy(&bits,&n,4);
        if((bits&0x7f800000u)==0u){bits&=0x80000000u;float z;std::memcpy(&z,&bits,4);return z;}
        volatile float r=static_cast<float>(std::sqrt(static_cast<double>(n)));return keep(r);
    }
};
inline void clear(OriginalWreckPhysical50LA& p) noexcept {
    auto& r=p.body.root;
    r.loads.force={};p.body.force180W=1.f;
    r.loads.torque={};p.body.torque190W=1.f;
    r.accumulator1c0={};p.extra.accumulator1C0W=1.f;
    r.accumulator1d0={};p.extra.accumulator1D0W=1.f;
    r.positionCorrection1a0={};p.extra.positionCorrection1A0W=1.f;
    r.rotationCorrection1b0={};p.extra.rotationCorrection1B0W=1.f;
    r.callbackState238=0;
}
}
enum class OriginalWreckIntegrationStatus50LA : std::uint8_t {
    complete,owner,component,graph,arithmetic,visibility
};
struct OriginalWreckIntegrationResult50LA {
    OriginalWreckIntegrationStatus50LA status=OriginalWreckIntegrationStatus50LA::owner;
    std::uint32_t sourcePC=0;
    bool numericalPrefixCommitted=false,observerCommitted=false,suffixCommitted=false;
    OriginalWreckVisibilityResult50LA visibility{};
};
// Genuine167C40 body route for this concrete167920 body and its actual model
// observer. No GraphBodyDomain observer=false substitution is used. The existing
//49W public leaf and its observer guard are unchanged. Same source kernels are
//reused through1682C0; the canonical graph/body prefix is then published before
//the real28EA20/280D40 visibility owner, followed by the two direction getters
//and exact1654B0 callback. A late failure retains the completed source prefix.
//
//Actual body children must take the empty141ED0 route. The cold constructor
//fixes slot290=14EE40 and callback9C=1654B0 for this167920 owner. Visibility's
//native provider admits the preallocated dynamic List methods; portal helpers
//230BD0/231150 useVF4..6 and integer List/pool operations preserveVF9.W.
//28E428 setsVF9.W=1;28E46C/4A0/4D4 increment it three times. Hence an actual
//attached observer leaves4, yielding local80.W=5/worldA0.W=6. Null19C leaves
//the second point getter's1, yielding2/3. No caller supplied scratch receipt.
//Strict FE_TONEAREST, finite consumed arithmetic, serial nonaliasing scratch;
//workspace is temporary transaction storage, never a persistent body owner.
inline OriginalWreckIntegrationResult50LA integrateOriginalWreckBody50LA(
    OriginalWreckPhysical50LA& p,const OriginalWreckComponent50LA& bodyComponent,
    OriginalWreckAssets50LA& assets,OriginalWreckVisibility50LA& visibility,
    float dt,GraphBodyWorkspace49W& work) noexcept {
    using namespace original_wreck_integration_detail50LA;
    using S=OriginalWreckIntegrationStatus50LA;OriginalWreckIntegrationResult50LA result;
    auto fail=[&](S s,std::uint32_t pc){result.status=s;result.sourcePC=pc;return result;};
    if(!p.bodyConstructorFieldsApplied||bodyComponent.kind!=OriginalWreckComponentKind50LA::body167920||
       !assets.prepared()||!visibility.boundTo(assets)||p.model288!=assets.model14C())return fail(S::owner,0x167c40u);
    if((bodyComponent.descendants24&2u)&&bodyComponent.firstChild18)return fail(S::component,0x141ed0u);
    if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,0x167c40u);
    const auto graph=assets.graph();const auto raw=static_cast<std::int32_t>(p.model288);
    if(!validateSceneGraph49P(graph)||raw<0||static_cast<std::uint32_t>(raw)>=graph.count||
       graph.nodes[raw].parent180!=SceneNodeIndex49P::none)return fail(S::graph,0x167c58u);
    if(!p.body.enabled){clear(p);result.suffixCommitted=true;result.status=S::complete;return result;}
    work.body=p.body.root;work.supplement=p.supplement;work.extra=p.extra;
    std::memcpy(work.nodes,graph.nodes,graph.count*sizeof(SceneNode49P));
    const SceneGraph49P staged{work.nodes,graph.count};auto& root=staged.nodes[raw];
    auto& b=work.body;auto& s=work.supplement;auto& x=work.extra;
    detail::SceneVectorContext49U context{0.f};
    b.previousVelocity90=b.body.velocity;s.previousVelocity90W=p.body.velocity70W;
    V point{};
    if(!detail::pointGraph49W(staged,p.model288,vector(p.parameters.centerOffset),context,point))return fail(S::arithmetic,0x167c80u);
    xyz(b.body.position,point);x.position60W=point.v[3];
    std::memcpy(&b.scene,&root.world100,sizeof b.scene);
    if(!matrixToQuaternion48G(root.world100,b.body.orientation)||
       !integrateBody48C(b.body,p.parameters.body,b.loads,dt,b.body))return fail(S::arithmetic,0x167cb8u);
    s.positionDerivative2B8=b.body.velocity;s.linearAcceleration2D0=b.body.linearAcceleration;
    s.angularAcceleration308=b.body.angularAcceleration;s.linearAccelerationC0W=1.f;s.angularAccelerationD0W=2.f;
    BodyMatrix48G orientation{};
    if(!quaternionToMatrix48G(b.body.orientation,orientation)||
       !setParentlessWorldMatrix49P(staged,p.model288,orientation))return fail(S::arithmetic,0x168110u);
    Math f;const auto& correction=b.rotationCorrection1b0.v;
    for(float v:correction)f.keep(v);
    if(!f.valid)return fail(S::arithmetic,0x168128u);
    if(correction[0]!=0.f||correction[1]!=0.f||correction[2]!=0.f){
        V axis{};
        if(!detail::direction49U(staged,p.model288,vector(b.rotationCorrection1b0),context,axis))return fail(S::arithmetic,0x168170u);
        const float length=f.length(axis);
        if(!f.valid)return fail(S::arithmetic,0x1681dcu);
        if(length!=0.f){const float inverse=f.div(1.f,length);
            for(unsigned i=0;i<3;++i)axis.v[i]=f.mul(axis.v[i],inverse);
            if(!f.valid||!detail::rotateAxisGraph49W(staged,p.model288,axis,length))return fail(S::arithmetic,0x168224u);}
    }
    if(!detail::pointGraph49W(staged,p.model288,vector(p.parameters.centerOffset),context,point))return fail(S::arithmetic,0x168234u);
    float delta[3];for(unsigned i=0;i<3;++i)delta[i]=f.sub(b.positionCorrection1a0.v[i],point.v[i]);
    for(unsigned i=0;i<3;++i)b.body.position.v[i]=f.add(b.body.position.v[i],delta[i]);
    const SceneDisplacement49P displacement{{b.body.position.v[0],b.body.position.v[1],b.body.position.v[2],x.position60W}};
    if(!f.valid||!addParentlessTranslation49P(staged,p.model288,displacement))return fail(S::arithmetic,0x1682b8u);
    //1682C4 must observe the genuinely changed model and canonical body.
    p.body.root=b;p.supplement=s;p.extra=x;
    std::memcpy(graph.nodes,work.nodes,graph.count*sizeof(SceneNode49P));
    std::memcpy(&p.body.root.scene,&graph.nodes[raw].world100,sizeof p.body.root.scene);
    p.body.sceneFlags190=(p.body.sceneFlags190&0xffffff00u)|graph.nodes[raw].sceneFlags190;
    result.numericalPrefixCommitted=true;
    const bool attached=visibility.attached();result.visibility=visibility.update();
    if(result.visibility.status!=OriginalWreckVisibilityStatus50LA::complete)return fail(S::visibility,0x1682c4u);
    result.observerCommitted=true;context.vf9W=attached?4.f:1.f;
    V local{},angular{};
    if(!detail::inverseDirection49U(graph,p.model288,vector(p.body.root.body.velocity,p.body.velocity70W),context,local))return fail(S::arithmetic,0x1682d4u);
    xyz(p.body.root.localVelocity80,local);p.supplement.localVelocity80W=local.v[3];
    if(!detail::direction49U(graph,p.model288,vector(p.body.root.body.angularVelocity,p.body.angularVelocityB0W),context,angular))return fail(S::arithmetic,0x1682ecu);
    xyz(p.body.root.worldAngularVelocityA0,angular);p.extra.worldAngularVelocityA0W=angular.v[3];
    std::memcpy(&p.body.root.scene,&graph.nodes[raw].world100,sizeof p.body.root.scene);
    p.body.sceneFlags190=(p.body.sceneFlags190&0xffffff00u)|graph.nodes[raw].sceneFlags190;
    clear(p);result.suffixCommitted=true;result.status=S::complete;result.sourcePC=0x16836cu;return result;
}
}
