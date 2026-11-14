#include "vehicle_base_finish50y.h"
#include "vehicle_base_tail50x.h"
#include "vehicle_frame49w.h"
#include "pod_live_query50iz.h"
#include "chariot_scene49u.h"
#include "movement_hit_point50m.h"
#include "../assets/original_standard_material_animation50la.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <initializer_list>
#include <cstddef>
#include <limits>
#include <utility>
#include <exception>

#ifndef RR_VEHICLE_FRAME49W_STAGE
#define RR_VEHICLE_FRAME49W_STAGE(pc) ((void)0)
#endif
#ifndef RR_VEHICLE_FRAME49W_FORCE_CUT
#define RR_VEHICLE_FRAME49W_FORCE_CUT(pc,state,index) ((void)0)
#endif
namespace rr::game { namespace {
using I=SceneNodeIndex49P;using V=VisualVector49R;using S=VehicleFrameState49W;
using P=VehicleFramePhase49W;using E=VehicleFrameError49W;
float fp(std::uint32_t u) noexcept {float x;std::memcpy(&x,&u,4);return x;}
std::uint32_t bits(float x) noexcept {std::uint32_t u;std::memcpy(&u,&x,4);return u;}
struct Math {
 bool valid=true;
 float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
 float add(float a,float b) noexcept {keep(a);keep(b);volatile float x=a+b;return keep(x);}
 float sub(float a,float b) noexcept {keep(a);keep(b);volatile float x=a-b;return keep(x);}
 float mul(float a,float b) noexcept {keep(a);keep(b);volatile float x=a*b;return keep(x);}
 float div(float a,float b) noexcept {keep(a);keep(b);volatile float x=a/b;return keep(x);}
 float neg(float a) noexcept {return keep(fp(bits(a)^0x80000000u));}
 float squared(const V& v,bool vu=false) noexcept {const float x=mul(v.v[0],v.v[0]),y=mul(v.v[1],v.v[1]),z=mul(v.v[2],v.v[2]);return add(add(x,y),vu?mul(1,z):z);}
 float scalarRoot(float x) noexcept {if((bits(x)&0x7f800000u)==0)return fp(bits(x)&0x80000000u);volatile float r=static_cast<float>(std::sqrt(static_cast<double>(x)));return keep(r);}
 V scale(float k,const V& v) noexcept {V r{{0,0,0,1}};for(unsigned i=0;i<3;++i)r.v[i]=mul(k,v.v[i]);return r;}
 V normalize(const V& v) noexcept {const float x=squared(v,true);float q;if(x==0)q=fp((bits(x)&0x80000000u)|0x7f7fffffu);else {if((bits(x)&0x7f800000u)==0)valid=false;volatile float root=std::sqrt(std::fabs(x));q=div(1,root);}return scale(q,v);}
 V cross(const V&a,const V&b) noexcept {V r{};for(unsigned i=0;i<3;++i){const unsigned j=(i+1)%3,k=(i+2)%3;r.v[i]=sub(mul(a.v[j],b.v[k]),mul(b.v[j],a.v[k]));}return r;}
 void addLoad(Vector3_48C& to,const V&v) noexcept {for(unsigned i=0;i<3;++i)to.v[i]=add(to.v[i],v.v[i]);}
};
V vector(const Vector3_48C&v,float w=1) noexcept {return {{v.v[0],v.v[1],v.v[2],w}};}
SceneGraph49P graph(S&s) noexcept {return {s.nodes,s.sceneNodeCount};}
const SceneNode49P& node(const S&s,I i) noexcept {return s.nodes[static_cast<unsigned>(i)];}
SceneNode49P& node(S&s,I i) noexcept {return s.nodes[static_cast<unsigned>(i)];}
bool index(I i) noexcept {const auto n=static_cast<int>(i);return n>=0&&n<int(vehicleFrameNodeCount49W);}
bool overlap(const void*a,std::size_t an,const void*b,std::size_t bn) noexcept {const auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);return x<=y?y-x<an:x-y<bn;}
bool fail(VehicleFrameDiagnostic49W&d,E e,std::uint32_t pc) noexcept {d.error=e;d.sourceStage=pc;return false;}
bool same(const void*a,const void*b,std::size_t n) noexcept {return std::memcmp(a,b,n)==0;}
bool emptyRoute(const GraphBodyDomain49W&d) noexcept {return (!(d.componentFlags24&2u)||!d.firstComponentChildPresent18)&&!d.rootObserverPresent19C&&d.integrationMethod290==GraphBodyIntegrator49W::OriginalAdamsBashforth2&&d.callbackMethod9C==GraphBodyCallback49W::Clear238;}
unsigned children(const S&s,I i) noexcept {unsigned n=0;for(I k=node(s,i).firstChild184;k!=I::none;k=node(s,k).nextSibling188)++n;return n;}
void project(S&s,const VehicleFrameBindings49W&b) noexcept {
 auto& c=s.bodies;c.main.components.root.scene=node(s,b.lifecycle.main190).world100;
 c.mainSphere.sceneFlags190=(c.mainSphere.sceneFlags190&0xffffff00u)|node(s,b.lifecycle.main190).sceneFlags190;
 c.secondary.root.scene=node(s,b.lifecycle.physical19C).world100;
 c.secondary.sceneFlags190=(c.secondary.sceneFlags190&0xffffff00u)|node(s,b.lifecycle.physical19C).sceneFlags190;
}
bool admit(const S&s,const VehicleFrameBindings49W&b,VehicleFrameDiagnostic49W&d) noexcept {
 const auto&c=s.bodies;const auto& m=c.main.domain;const auto&z=c.secondaryDomain;
 // Preserve the old disabled admission; enabled348 requires explicit cold opt-in.
 if(!s.terrainProbe.enabled&&m.probe100)return fail(d,E::Admission,0);
 if(s.terrainProbe.enabled&&(m.probe100!=1u||s.terrainProbe.stage>3u||c.main.components.probe348.field1E8>16u))return fail(d,E::Admission,0x1b1800);
 if(m.probe124Present!=bool(b.terrainWater50JC)||
    (b.terrainWater50JC&&(!s.terrainProbe.enabled||!coherentTerrainWater50JC(*b.terrainWater50JC))))
  return fail(d,E::Admission,0x1b18b4);
 if(std::fegetround()!=FE_TONEAREST||s.fields.vehicle58>1u||(s.fields.vehicle58==1u&&!s.fields.nativeAiOwnersBound50IV)||s.fields.vehicle3C||s.fields.vehicle834||(c.main.vehicleState40&&(c.main.vehicleState40>2u||s.fields.vehicle58))||c.main.applicationActive11C||m.body320!=1||m.bodyChildCount||b.mainDomain.firstComponentChildPresent18||m.sceneObserverCount||!m.sceneAttached||m.sceneHasParent||!m.bodyEnabled||!m.secondBodyPresent||m.override3C||m.resetRequested||m.probe120Present||z.body320!=1||z.registrationMask!=2||z.bodyChildCount||b.secondaryDomain.firstComponentChildPresent18||z.sceneObserverCount||z.sceneHasParent||!c.secondary.scenePresent||!c.secondary.enabled||s.shape.type88||(s.shape.registrationE0&&!b.podLiveQuery50IZ)||!emptyRoute(b.mainDomain)||!emptyRoute(b.secondaryDomain))return fail(d,E::Admission,0);
 if(b.podLiveQuery50IZ&&!coherentPodLiveQuery50IZ(s.shape,*b.podLiveQuery50IZ))
  return fail(d,E::Admission,0x12023cu);
 const SceneGraph49P g{const_cast<SceneNode49P*>(s.nodes),s.sceneNodeCount};
 if(!validVehicleSceneCount49W(s,b)||!validateSceneGraph49P(g))return fail(d,E::Graph,0);
 for(unsigned i=0;i<s.sceneNodeCount;++i){const auto&n=s.nodes[i];const auto&t=b.topology[i];if(n.parent180!=t.parent180||n.firstChild184!=t.firstChild184||n.nextSibling188!=t.nextSibling188)return fail(d,E::Graph,0);}
 const auto&v=b.visual;const auto&l=b.lifecycle;const auto&a=b.animation;
 for(I i:{l.main190,l.container194,l.hidden198,l.physical19C,l.model1B4,l.left1BC,l.right1B8,l.leftCable1E0,l.rightCable1E4})if(!index(i)||static_cast<unsigned>(i)>=s.sceneNodeCount)return fail(d,E::Graph,0);
 if(l.main190==l.physical19C||node(s,l.main190).parent180!=I::none||node(s,l.physical19C).parent180!=I::none||node(s,l.container194).parent180!=l.main190||v.main190!=l.main190||v.container194!=l.container194||v.left1BC!=l.left1BC||v.right1B8!=l.right1B8||a.bodyScene288!=l.main190||a.chariot1B4!=l.model1B4||a.left1BC!=l.left1BC||a.right1B8!=l.right1B8||children(s,l.main190)!=m.sceneChildCount||children(s,l.physical19C)!=z.sceneChildCount)return fail(d,E::Graph,0);
 const bool terrainDirty=s.terrainProbe.enabled&&s.terrainProbe.stage==3u&&(s.phase==P::Post368||s.phase==P::MainIntegration);
 if((!(node(s,l.main190).sceneFlags190&1u)&&!terrainDirty)||!same(&c.main.components.root.scene,&node(s,l.main190).world100,sizeof(BodyMatrix48G))||!same(&c.secondary.root.scene,&node(s,l.physical19C).world100,sizeof(BodyMatrix48G))||(c.mainSphere.sceneFlags190&255u)!=node(s,l.main190).sceneFlags190||(c.secondary.sceneFlags190&255u)!=node(s,l.physical19C).sceneFlags190)return fail(d,E::Projection,0);
 return true;
}
bool phases(const S&s) noexcept {
 using M=MainBodyPhase49K;using C=CoupledBodyPhase49M;M m=M::AwaitingPostBase;C c=C::AwaitingPostBase;
 switch(s.phase){
 case P::Ready:m=M::FrameReady;c=C::FrameReady;break;
 case P::PostPlayer:m=M::AwaitingPostPlayer;c=C::AwaitingPostPlayer;break;
 case P::ImpactFeedbackPending50KZ:case P::ImpactFeedbackClearReturned50KZ:case P::ImpactFeedbackReturned50KZ:case P::BeforeGraphics:case P::PostGraphics:case P::PostImpactConsumers:case P::PostImpactRepair:case P::PostImpactEffects50O:case P::PostOptionalProbeEffects50P:case P::PostPlainEffects50Q:case P::PostPackedBytes50S:case P::PostEngineBase50X:case P::AfterImpactBlock:case P::BetweenBodyChildren:case P::AfterBodyChildren:break;
 case P::SubstepReady:m=M::SubstepReady;c=C::SubstepReady;break;
 case P::PostWorld:m=M::AwaitingWorldPreparation;c=C::AwaitingWorldPreparation;break;
 case P::Post354:m=M::AwaitingResponse354;c=C::AwaitingResponse354;break;
 case P::Post368:m=M::AwaitingResponse354;c=C::AwaitingResponse368;break;
 case P::MainIntegration:m=M::AwaitingResponse354;c=C::AwaitingMainIntegration;break;
 case P::Query219120:case P::PodLivePending50IZ:m=M::AwaitingQuery219120;c=C::AwaitingQuery219120;break;
 }
 return s.bodies.main.phase==m&&s.bodies.phase==c&&s.bodies.secondary.phase==(s.phase==P::Post368?SecondaryPhase49M::Response368Pending:SecondaryPhase49M::Ready);
}
bool start(S&s,const VehicleFrameBindings49W&b,P phase,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(overlap(&d,sizeof d,&s,sizeof s)||overlap(&d,sizeof d,&w,sizeof w)||overlap(&d,sizeof d,&b,sizeof b))return false;
 d={E::None,0,GraphBodyDiagnostic49W::None};
 if(overlap(&s,sizeof s,&w,sizeof w)||overlap(&s,sizeof s,&d,sizeof d)||overlap(&w,sizeof w,&d,sizeof d)||overlap(&w,sizeof w,&b,sizeof b))return fail(d,E::Storage,0);
 if(s.phase!=phase||!phases(s))return fail(d,E::Phase,0);
 if(!admit(s,b,d))return false;
 if(phase>=P::SubstepReady&&phase<=P::Query219120){Schedule49K q{};if(!selectSchedule49K(s.bodies.main.frameFactor,s.bodies.main.liveSubstepScale,q)||q.count!=s.bodies.main.schedule.count||bits(q.factorPerSubstep)!=bits(s.bodies.main.schedule.factorPerSubstep)||s.bodies.main.substepIndex>=q.count)return fail(d,E::Phase,0);}
 if(s.noiseOwner().noise.initializationIncomplete49M)return fail(d,E::Admission,0x14eec0);
 w.next=s;
 // Stage the real shared owner only for this transactional leaf. Failure must
 // preserve application RNG/tables just as it preserves the frame. This is
 // scratch, never a destination-owned stream or a cross-page seed transfer.
 w.next.noise=s.noiseOwner();w.next.borrowedNoise50KG=nullptr;return true;
}
bool preserving(const PreservingStage49W&x,PreservingCut49W p,VehicleFrameDiagnostic49W&d) noexcept {return (x.point==p&&x.representedStatePreserved&&x.omittedBB0WNotConsumed)||fail(d,E::ExternalCut,0);}
bool noiseContinuation(const S&live,const S&staged,VehicleFrameDiagnostic49W&d) noexcept {
 //Native serial/reentrant guard. Previously issued real effects are retained;
 //only the next dependent effect/callback is blocked after a partial prefix.
 return (!live.noiseOwner().noise.initializationIncomplete49M&&
         !staged.noiseOwner().noise.initializationIncomplete49M)||fail(d,E::Admission,0x14eec0);
}
bool commit(S&s,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d,P p) noexcept {
 //A live incomplete owner always wins over an earlier clean scratch copy.
 //This is serial/reentrant admission, not a synchronization mechanism.
 if(!noiseContinuation(s,w.next,d))return false;
 auto* shared=s.borrowedNoise50KG;w.next.phase=p;
 if(shared)*shared=w.next.noise;
 s=w.next;s.borrowedNoise50KG=shared;return true;
}
BodyContactEffects49W effects(const RootBodyState48H&r,float v,float a,float f,float t,const GraphBodyExtraLanes49W&e,std::uint32_t c) noexcept {return {r.body.velocity,r.body.angularVelocity,r.loads,r.positionCorrection1a0,r.rotationCorrection1b0,r.accumulator1c0,r.accumulator1d0,v,a,f,t,e.positionCorrection1A0W,e.rotationCorrection1B0W,e.accumulator1C0W,e.accumulator1D0W,c};}
void install(RootBodyState48H&r,float&v,float&a,float&f,float&t,GraphBodyExtraLanes49W&e,std::uint32_t&c,const BodyContactEffects49W&x) noexcept {r.body.velocity=x.velocity70;r.body.angularVelocity=x.angularVelocityB0;r.loads=x.loads;r.positionCorrection1a0=x.positionCorrection1A0;r.rotationCorrection1b0=x.rotationCorrection1B0;r.accumulator1c0=x.accumulator1C0;r.accumulator1d0=x.accumulator1D0;v=x.velocity70W;a=x.angularVelocityB0W;f=x.force180W;t=x.torque190W;e.positionCorrection1A0W=x.positionCorrection1A0W;e.rotationCorrection1B0W=x.rotationCorrection1B0W;e.accumulator1C0W=x.accumulator1C0W;e.accumulator1D0W=x.accumulator1D0W;c=x.body20C;}
bool sameEffects(const ContactEffects49W&a,const ContactEffects49W&b) noexcept {
 const auto&x=a.joint;const auto&y=b.joint;
 return same(&a.main,&b.main,sizeof a.main)&&same(&a.secondary,&b.secondary,sizeof a.secondary)&&
  same(&x.stiffness30,&y.stiffness30,offsetof(PodSphereJoint49L,bodyAPresent44))&&
  x.bodyAPresent44==y.bodyAPresent44&&x.bodyBPresent48==y.bodyBPresent48&&
  same(&x.length4C,&y.length4C,sizeof x-offsetof(PodSphereJoint49L,length4C))&&same(&a.impactBA0,&b.impactBA0,sizeof a.impactBA0);
}
void install(S&s,const ContactEffects49W&e) noexcept {auto&c=s.bodies;auto&m=c.mainSphere;auto&z=c.secondary;install(c.main.components.root,m.velocity70W,m.angularVelocityB0W,m.force180W,m.torque190W,s.mainExtra,c.main.components.body20C,e.main);install(z.root,z.velocity70W,z.angularVelocityB0W,z.force180W,z.torque190W,s.secondaryExtra,z.body20C,e.secondary);c.joint=e.joint;s.fields.impactBA0=e.impactBA0;}
ReadyBodyBasis48D basis(const BodyMatrix48G&m) noexcept {ReadyBodyBasis48D r{};for(unsigned c=0;c<3;++c)for(unsigned i=0;i<3;++i)r.columns[c].v[i]=m.v[4*c+i];return r;}
// Exact null-relative28A700 path: ready test/lazy world, then XYZ axes copies.
bool axes(S&s,I i,V&forward,V&up) noexcept {if(!refreshSceneWorld49P(graph(s),i))return false;for(unsigned j=0;j<3;++j){forward.v[j]=node(s,i).world100.v[8+j];up.v[j]=node(s,i).world100.v[4+j];}return true;}
void mainSphere(const S&s,const VehicleFrameParameters49W&p,PodSphereBody49L&out) noexcept {const auto&m=s.bodies.main;const auto&l=s.bodies.mainSphere;SecondaryBodyState49M view{};view.root=m.components.root;view.scenePresent=m.domain.sceneAttached;view.sceneFlags190=l.sceneFlags190;view.velocity70W=l.velocity70W;view.angularVelocityB0W=l.angularVelocityB0W;view.force180W=l.force180W;view.torque190W=l.torque190W;SecondaryBodyParameters49M par{};par.body=p.bodies.main.body;par.centerOffset=p.bodies.main.centerOffset;par.centerOffsetW=p.bodies.mainSphere.centerOffsetW;std::memcpy(par.inverseInertiaExtra,p.bodies.mainSphere.inverseInertiaExtra,sizeof par.inverseInertiaExtra);makeSecondarySphereBody49M(view,par,out);}
// Graph-aware source force path. Gravity/damping commit to scratch before
//218C60 lazy relative getter. Nonzero40 uses the genuine shared14EEC0 noise
//branch, never ordinary alignment with a missing engine. Whole-frame admission
//still requires the actual destruction owner before that route is reachable.
//No persistent49L body mirror exists.
bool secondaryForces(const S&live,S&s,const VehicleFrameBindings49W&b,const VehicleFrameParameters49W&p,const SecondaryInputs49M&x,VehicleFrameDiagnostic49W&d) noexcept {
 if(!noiseContinuation(live,s,d))return false;
 auto&z=s.bodies.secondary;const auto&m=s.bodies.main;const auto&par=p.bodies.secondary;
 if(z.phase!=SecondaryPhase49M::Ready)return false;
 Math f;const float g=m.components.probe348.field104?s.fields.gravity378:s.fields.gravity37C;
 if(!noiseContinuation(live,s,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x218a3c);if(!noiseContinuation(live,s,d))return false;RR_VEHICLE_FRAME49W_FORCE_CUT(0x218a3c,s,b.lifecycle.physical19C);if(!noiseContinuation(live,s,d))return false;f.addLoad(z.root.loads.force,{{0,f.mul(f.neg(par.body.mass),g),0,1}});
 const float lo=x.effective4346D0,hi=x.effective4346D8;
 float damping=f.add(lo,f.div(f.mul(f.sub(z.cachedSpeed34,fp(0x422fffcc)),f.sub(hi,lo)),fp(0x41eaaa64)));
 if(hi<=lo){if(damping<hi)damping=hi;else if(!(damping<=lo))damping=lo;}else{if(!(damping<=hi))damping=hi;else if(damping<lo)damping=lo;}
 f.addLoad(z.root.loads.torque,f.scale(f.neg(damping),vector(z.root.body.angularVelocity)));
 detail::SceneVectorContext49U scratch{0};V relative{},worldError{},upA{},upB{},forward{},torque{};
 if(!noiseContinuation(live,s,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x218c60);if(!noiseContinuation(live,s,d))return false;RR_VEHICLE_FRAME49W_FORCE_CUT(0x218c60,s,b.lifecycle.physical19C);if(!noiseContinuation(live,s,d))return false;
 if(!f.valid||!detail::position49U(graph(s),b.lifecycle.physical19C,b.lifecycle.main190,scratch,relative))return false;
 project(s,b);
 V error{{f.sub(x.targetRelative2F0.v[0],relative.v[0]),f.sub(x.targetRelative2F0.v[1],relative.v[1]),f.sub(x.targetRelative2F0.v[2],relative.v[2]),1}};error.v[1]=0;
 const float length=f.scalarRoot(f.squared(error));
 if(!noiseContinuation(live,s,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x218ce0);if(!noiseContinuation(live,s,d))return false;if(!detail::direction49U(graph(s),b.lifecycle.main190,error,scratch,worldError))return false;
 f.addLoad(z.root.loads.force,f.scale(f.mul(x.effective4346E0,length),f.normalize(worldError)));
 if(!noiseContinuation(live,s,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x218d90);if(!noiseContinuation(live,s,d))return false;
 if(!m.vehicleState40){
  f.addLoad(z.root.loads.force,f.scale(f.neg(x.effective4346C8),vector(z.root.body.velocity)));
  if(!noiseContinuation(live,s,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x218e30);if(!noiseContinuation(live,s,d))return false;if(!axes(s,b.lifecycle.physical19C,forward,upA))return false;
  if(!noiseContinuation(live,s,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x218e44);if(!noiseContinuation(live,s,d))return false;if(!axes(s,b.lifecycle.main190,forward,upB))return false;
  const auto worldTorque=f.scale(x.effective4346E8,f.cross(upA,upB));
  if(!noiseContinuation(live,s,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x218eb8);if(!noiseContinuation(live,s,d))return false;if(!detail::inverseDirection49U(graph(s),b.lifecycle.physical19C,worldTorque,scratch,torque))return false;
  f.addLoad(z.root.loads.torque,torque);
 }else{
  //218EFC..2190A0. s is this49W leaf's transaction scratch. Its noise is
  //the staged SAME application owner from start(), not a per-racer stream.
  const float coordinate=f.mul(x.effective434708,z.time864);float value=0.f;
  if(!noiseContinuation(live,s,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x218f08);if(!noiseContinuation(live,s,d))return false;
  if(!f.valid||!evaluateGradientNoise49M(s.noise.noise,s.noise.sharedRandomState,coordinate,value))return false;
  const float h=f.add(.5f,f.mul(.5f,value));
  f.addLoad(z.root.loads.torque,{{0.f,0.f,f.mul(m.cachedSpeed34,f.mul(h,x.effective4346F8)),1.f}});
  const V localForce{{f.mul(m.cachedSpeed34,f.mul(h,x.effective434700)),0.f,0.f,1.f}};
  z.time864=f.add(z.time864,m.schedule.factorPerSubstep);
  V worldForce{};if(!noiseContinuation(live,s,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x218fd8);if(!noiseContinuation(live,s,d))return false;
  if(!f.valid||!detail::direction49U(graph(s),b.lifecycle.physical19C,localForce,scratch,worldForce))return false;
  f.addLoad(z.root.loads.force,worldForce);
  f.addLoad(z.root.loads.force,f.scale(f.neg(x.effective4346F0),vector(z.root.body.velocity)));
 }
 if(!f.valid)return false;
 project(s,b);z.phase=SecondaryPhase49M::ConstraintPending;
 PodSphereBody49L main{};mainSphere(s,p,main);
 if(!noiseContinuation(live,s,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x2190ac);if(!noiseContinuation(live,s,d))return false;
 return applySecondaryConstraint49M(z,par,main,s.bodies.joint,z,s.bodies.joint,s.bodies.lastConstraintMagnitude);
}
TerrainFollowState48G terrainFollow50FP(const S& s) noexcept {
 const auto& p=s.bodies.main.components.probe348;const auto& e=s.terrainProbe;
 TerrainFollowState48G out{p.field104,p.field10C,p.field11C,e.previousPoint180,e.previousNormal190,{}};
 std::memcpy(out.largestVelocityChange1d0.v,p.vector1D0,12);return out;
}
PreparedTerrainState48J terrainSurface50FP(const S& s) noexcept {
 const auto& p=s.bodies.main.components.probe348;const auto& e=s.terrainProbe;
 PreparedTerrainState48J out{};out.surface={p.field108,p.field110,p.field114,{},e.normal170};
 std::memcpy(out.surface.point150.v,p.point150,12);out.collectedIdentities=e.collectedIdentities;
 out.collectedCount=p.field1E8;out.excludedOwnerIdentity=p.field12C;out.currentCandidateIdentity=e.currentCandidateIdentity;return out;
}
void installTerrainSurface50FP(S& s,const PreparedTerrainState48J& in) noexcept {
 auto& p=s.bodies.main.components.probe348;auto& e=s.terrainProbe;
 p.field108=in.surface.forceAngleTest108;p.field110=in.surface.hit110;p.field114=in.surface.hit114;
 std::memcpy(p.point150,in.surface.point150.v,12);e.normal170=in.surface.normal170;
 p.field1E8=in.collectedCount;p.field12C=in.excludedOwnerIdentity;
 e.collectedIdentities=in.collectedIdentities;e.currentCandidateIdentity=in.currentCandidateIdentity;
}
bool respondEnabledTerrain50FP(S& s,const VehicleFrameBindings49W& bindings) noexcept {
 auto& e=s.terrainProbe;auto& m=s.bodies.main;auto& c=m.components;auto& p=c.probe348;
 if(e.stage!=2u)return false;
 const auto initial=terrainFollow50FP(s);const auto surface=terrainSurface50FP(s);
 const auto& old=node(s,bindings.lifecycle.main190).world100;
 TerrainFollowBody48G input{};input.position60=c.root.body.position;input.worldVelocity70=c.root.body.velocity;
 input.cachedLocalVelocity80=c.root.localVelocity80;input.angleE8=m.cachedAnglesE0.v[2];input.loads=c.root.loads;
 for(unsigned axis=0;axis<3;++axis){std::memcpy(input.scene.basis.columns[axis].v,old.v+4*axis,12);input.scene.position.v[axis]=old.v[12+axis];}
 TerrainFollowResult48G result{};
 if(!applyTerrainFollowing48G(initial,e.response,surface.surface,input,result))return false;
 if(result.state.active104){
  //48G contains the original rounded28AF90 translation result. Apply its local
  //write/invalidation first. Parentless190 is required by frame admission.
  auto translated=node(s,bindings.lifecycle.main190).localC0;
  std::memcpy(translated.v+12,result.body.scene.position.v,12);
  if(!setLocalMatrix49P(graph(s),bindings.lifecycle.main190,translated))return false;
  //The reverse-motion force inverse getter occurs between translation and axes.
  if(input.cachedLocalVelocity80.v[2]<0.f&&
     !refreshSceneWorld49P(graph(s),bindings.lifecycle.main190))return false;
  //28A090(relative=null,t0=t1=0,t2=1): unnormalized basis, saved translation.
  auto oriented=node(s,bindings.lifecycle.main190).localC0;
  for(unsigned axis=0;axis<3;++axis){std::memcpy(oriented.v+4*axis,result.body.scene.basis.columns[axis].v,12);oriented.v[4*axis+3]=0.f;}
  oriented.v[15]=1.f;
  if(!setLocalMatrix49P(graph(s),bindings.lifecycle.main190,oriented))return false;
 }
 p.field104=result.state.active104;p.field10C=result.state.pending10c;p.field11C=result.state.detached11c;
 e.previousPoint180=result.state.previousPoint180;e.previousNormal190=result.state.previousNormal190;
 std::memcpy(p.vector1D0,result.state.largestVelocityChange1d0.v,12);
 c.root.body.velocity=result.body.worldVelocity70;c.root.localVelocity80=result.body.cachedLocalVelocity80;c.root.loads=result.body.loads;
 //The body's position/orientation/history are still owned by167C40 readback.
 e.stage=3;project(s,bindings);return true;
}
} // namespace
ContactEffects49W contactEffects49W(const S&s) noexcept {const auto&c=s.bodies;const auto&m=c.mainSphere;const auto&z=c.secondary;return {effects(c.main.components.root,m.velocity70W,m.angularVelocityB0W,m.force180W,m.torque190W,s.mainExtra,c.main.components.body20C),effects(z.root,z.velocity70W,z.angularVelocityB0W,z.force180W,z.torque190W,s.secondaryExtra,z.body20C),c.joint,s.fields.impactBA0};}
bool readAiVehicleFrame50IV(S&s,const VehicleFrameBindings49W&b,float factor,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(s.fields.vehicle58!=1u||!s.fields.nativeAiOwnersBound50IV)return fail(d,E::Admission,0x16e8c0);
 if(!start(s,b,P::Ready,w,d))return false;
 if(!std::isfinite(factor)||factor<0.f||factor>fp(0x3d75c28f))return fail(d,E::Input,0x16e8c0);
 auto&m=w.next.bodies.main;m.frameFactor=factor;m.substepIndex=0;
 m.phase=MainBodyPhase49K::AwaitingPostPlayer;w.next.bodies.phase=CoupledBodyPhase49M::AwaitingPostPlayer;
 return commit(s,w,d,P::PostPlayer);
}
bool bindAiVehicleFrame50IV(S&s,const VehicleFrameBindings49W&b,const VehicleFrameParameters49W&p,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(s.fields.vehicle58!=1u||!s.fields.nativeAiOwnersBound50IV)return fail(d,E::Admission,0x217de0);
 if(!start(s,b,P::PostPlayer,w,d))return false;
 if(!(s.bodies.main.controls.player.flagsB9C&0x80u)||p.rates.x!=4.f)return fail(d,E::Admission,0x16e8d4);
 auto&m=w.next.bodies.main;const auto&actions=m.controls.player;
 ControlBindingInput48W context{};context.mode58Nonzero=true;context.vehicleState40=m.vehicleState40;
 context.bodyCachedSpeed34=m.cachedSpeed34;context.actionB7C=actions.actionB7C;context.actionB80=actions.actionB80;
 context.flagsB9C=actions.flagsB9C;context.source7D0=s.fields.damage7D0;context.source7D4=s.fields.damage7D4;
 ControlBoundStep48W bound{};
 if(!stepAndBindControlInputs48W(actions.raw,m.controls.bound.control.shaping.smoothedChannel0D28,
     m.controls.bound.control.filtered,p.rates,m.frameFactor,context,p.control,m.components.bindings,bound))return fail(d,E::Input,0x217de0);
 m.controls.bound=bound;m.components.bindings=bound.bindings;
 m.phase=MainBodyPhase49K::AwaitingPostBase;w.next.bodies.phase=CoupledBodyPhase49M::AwaitingPostBase;
 return commit(s,w,d,P::BeforeGraphics);
}
bool readVehicleFrame49W(S&s,const VehicleFrameBindings49W&b,const FrameReader49K&x,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(s.fields.vehicle58)return fail(d,E::Admission,0x1a15c0);
 if(!start(s,b,P::Ready,w,d))return false;
 if(!std::isfinite(x.preparedFactor)||x.preparedFactor<0||x.preparedFactor>fp(0x3d75c28f))return fail(d,E::Input,0x1a15c0);
 auto&m=w.next.bodies.main;auto c=x.context;
 c.source7D0=s.fields.damage7D0;c.source7D4=s.fields.damage7D4;c.source7D8=s.fields.damage7D8;c.source7DC=s.fields.damage7DC;c.previousFlagsB9C=s.bodies.main.controls.player.flagsB9C;c.gateByte58=s.fields.applicationByte58;if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x1a15c0);if(!noiseContinuation(s,w.next,d))return false;
 if(!conditionDualShockPlayerAxes48X(x.packet,m.controls.components))return fail(d,E::Input,0x1a1a20);c.componentValues=m.controls.components;
 if(!readPlayerInput48X(c,x.codes,x.actions,m.controls.player))return fail(d,E::Input,0x1a1a20);
 m.frameFactor=x.preparedFactor;m.substepIndex=0;m.phase=MainBodyPhase49K::AwaitingPostPlayer;w.next.bodies.phase=CoupledBodyPhase49M::AwaitingPostPlayer;
 return commit(s,w,d,P::PostPlayer);
}
bool bindVehicleFrame49W(S&s,const VehicleFrameBindings49W&b,const VehicleFrameParameters49W&p,const PreservingStage49W&cut,const PostPlayerAt217DE0_49K&x,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(s.fields.vehicle58)return fail(d,E::Admission,0x217de0);
 if(!start(s,b,P::PostPlayer,w,d)||!preserving(cut,PreservingCut49W::PlayerAndBaseBefore217DE0,d))return false;
 if(!(s.bodies.main.controls.player.flagsB9C&0x80u)||!(x.player.flagsB9C&0x80u)||x.bindingContext.vehicleState40>2u||x.bindingContext.mode58Nonzero)return fail(d,E::Admission,0x217de0);
 auto&m=w.next.bodies.main;auto&c=m.controls.bound.control;if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x217de0);if(!noiseContinuation(s,w.next,d))return false;
 if(!shapePlayerControlChannels48X(x.player.raw,c.shaping.smoothedChannel0D28,ControlShapeMode48V::Nonlinear,c.shaping)||!stepControlRateFilter48B(c.shaping.targets,c.filtered,p.rates,m.frameFactor,c.filtered))return fail(d,E::Input,0x217de0);
 auto input=x.bindingContext;input.source7D0=s.fields.damage7D0;input.source7D4=s.fields.damage7D4;input.filtered=c.filtered;input.bodyCachedSpeed34=m.cachedSpeed34;input.actionB7C=x.player.actionB7C;input.actionB80=x.player.actionB80;input.flagsB9C=x.player.flagsB9C;
 if(!bindVehicleControls48W(input,p.control,m.components.bindings,m.components.bindings))return fail(d,E::Input,0x2181c4);
 m.controls.player=x.player;m.controls.bound.bindings=m.components.bindings;m.phase=MainBodyPhase49K::AwaitingPostBase;w.next.bodies.phase=CoupledBodyPhase49M::AwaitingPostBase;
 return commit(s,w,d,P::BeforeGraphics);
}
bool graphicsVehicleFrame49W(S&s,const VehicleFrameBindings49W&b,const VehicleFrameParameters49W&p,const PreservingStage49W&cut,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::BeforeGraphics,w,d)||!preserving(cut,PreservingCut49W::Base2176E0BeforeGraphics,d))return false;
 auto&r=w.next;auto&m=r.bodies.main;auto&c=m.components;const auto&f=r.fields;const float dt=m.frameFactor;
 constexpr std::uint32_t initializedZero[4]={0,0,0,0x3f800000u};if(!same(p.activation.vector433B80,initializedZero,16))return fail(d,E::Admission,0x167ad0);
 VehicleAnimationInput49T a{VehicleAnimationPhase49T::beforeAnimation21A000,dt,c.bindings.aeroDrive[0],c.bindings.aeroDrive[1],f.chariot191,f.chariot1B0,f.left191,f.right191,true,true,vector(c.root.body.linearAcceleration,r.mainSupplement.linearAccelerationC0W)};
 a.midRecordsAbsent=!b.animation.mid288&&!b.animation.rightMid294&&!b.animation.leftMid298;
 a.steeringB6C=m.controls.bound.control.filtered.values[0];
 VehicleAnimationDiagnostic49T ad{};if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x21a000);if(!noiseContinuation(s,w.next,d))return false;
 if(!advanceVehicleAnimation49T(a,b.animation,graph(r),r.animation,r.animationShared,r.visualShared,w.animation,ad))return fail(d,E::Animation,ad.sourceStage);
 project(r,b);
 VehicleVisualInput49R v{};v.phase=VehicleVisualPhase49R::afterAnimationBeforeChariotAndPhysics;v.vehicle834=f.vehicle834;v.vehicle844=f.vehicle844;v.vehicle40=m.vehicleState40;v.applicationByte58=f.applicationByte58;v.probe108=c.probe348.field108;v.aeroD8=std::uint32_t(c.bindings.aeroTriggerD8);v.aeroDC=c.aero.lowSpeedDc;v.vehicleCFC=f.vehicleCFC;v.vehicleD00=f.vehicleD00;v.frameFactor=dt;v.cachedBodySpeed34=m.cachedSpeed34;v.rawB5C=m.controls.player.raw.values[0];v.objectD34_10=f.objectD34_10;v.engine68=c.bindings.engineDrive[0];v.engine6C=c.bindings.engineDrive[1];v.engine70=c.engine.smoothedDrive[0];v.engine74=c.engine.smoothedDrive[1];v.aeroE4=c.bindings.aeroDrive[0];v.aeroE8=c.bindings.aeroDrive[1];v.damage7D0=f.damage7D0;v.damage7D8=f.damage7D8;v.damage7D4=f.damage7D4;v.damage7DC=f.damage7DC;std::memcpy(v.probeVector1D0.v,c.probe348.vector1D0,16);v.impactBA0=f.impactBA0;
 VehicleVisualDiagnostic49R vd{};if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x21c120);if(!noiseContinuation(s,w.next,d))return false;
 if(!updateVehicleVisual49R(v,p.visual,b.visual,graph(r),r.visual,r.visualShared,r.noise,w.visual,vd))return fail(d,E::Visual,vd.sourceStage);
 project(r,b);
 const ChariotLifecycleInput49V l{ChariotLifecyclePhase49V::afterEngineResponseBeforePhysics,f.vehicle58,std::uint32_t(c.bindings.aeroTriggerD8),f.vehicleCFC,f.vehicleD00,dt,f.objectD34_10,f.impactBA0,f.applicationByte58,true};
 ChariotLifecycleDiagnostic49V ld{};if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x21a660);if(!noiseContinuation(s,w.next,d))return false;
 if(!updateChariotLifecycle49V(l,p.lifecycle,b.lifecycle,graph(r),m,r.bodies.mainSphere,r.bodies.secondary,r.secondarySupplement,p.activation,r.bodies.vehicle838,r.lifecycle,r.lifecycleShared,r.visualShared.zero434630,r.noise,w.lifecycle,ld))return fail(d,E::Lifecycle,ld.sourceStage);
 project(r,b);return commit(s,w,d,P::PostGraphics);
}
bool consumeVehicleImpacts49Z(S&s,const VehicleFrameBindings49W&b,const ImpactConsumerInput49Z&input,VehicleImpactEvents49Z&events,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::PostGraphics,w,d))return false;
 if(overlap(&events,sizeof events,&s,sizeof s)||overlap(&events,sizeof events,&w,sizeof w)||overlap(&events,sizeof events,&b,sizeof b)||overlap(&events,sizeof events,&input,sizeof input)||overlap(&events,sizeof events,&d,sizeof d))return fail(d,E::Storage,0);
 auto&r=w.next;auto&f=r.fields;auto&z=r.impactSupplement;
 ImpactConsumerInput49Z in;std::memcpy(&in,&input,sizeof in);
 if(in.dynamicPartnerEventsBound&&!r.fields.dynamicOwnersBound50IW)return fail(d,E::Admission,0x222cec);
 auto copyFloat=[](float&to,const float&from){std::memcpy(&to,&from,sizeof(float));};
 copyFloat(in.factor,r.bodies.main.frameFactor);in.flagsCD8=f.impactFlagsCD8;
 in.vehicle3C=f.vehicle3C;in.vehicle40=r.bodies.main.vehicleState40;in.vehicle58=f.vehicle58;
 in.componentPresence=3u|(b.visual.extra1C0Present?4u:0u)|(b.visual.extra1C4Present?8u:0u);
 ImpactConsumerState49Z before{},after{};
 std::memcpy(before.directionalD08,f.impactDirectionalD08,sizeof before.directionalD08);
 std::memcpy(before.timers820,z.timers820,sizeof before.timers820);
 copyFloat(before.damage7D0[0],f.damage7D0);copyFloat(before.damage7D0[1],f.damage7D4);copyFloat(before.damage7D0[2],f.damage7D8);copyFloat(before.damage7D0[3],f.damage7DC);
 copyFloat(before.cooldown7E0,z.cooldown7E0);copyFloat(before.periodic7E8,z.periodic7E8);
 before.fieldCFC=f.vehicleCFC;before.fieldD00=f.vehicleD00;before.fieldD04=z.fieldD04;
 before.partnerId=z.partnerId;before.partnerState=z.partnerState;
 copyFloat(before.partnerTimer08,z.partnerTimer08);copyFloat(before.partnerAmount0C,z.partnerAmount0C);copyFloat(before.partnerDuration10,f.objectD34_10);
 copyFloat(before.environmentalD38,z.environmentalD38);copyFloat(before.environmentalD3C,z.environmentalD3C);
 VehicleImpactEvents49Z next{};
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x2288a8);if(!noiseContinuation(s,w.next,d))return false;
 const auto status=consumeImpacts49Z(in,before,after,next.values,2,next.count,
     in.dynamicPartnerEventsBound?&next.partners:nullptr);
 if(status==ImpactConsumerStatus49Z::unsupportedPartner)return fail(d,E::Admission,0x222cec);
 if(status==ImpactConsumerStatus49Z::unsupportedDynamicTuning)return fail(d,E::Admission,0x222a84);
 if(status==ImpactConsumerStatus49Z::arithmetic)return fail(d,E::Arithmetic,0x222b20);
 if(status==ImpactConsumerStatus49Z::capacity)return fail(d,E::Storage,0x222d8c);
 std::memcpy(f.impactDirectionalD08,after.directionalD08,sizeof after.directionalD08);
 std::memcpy(z.timers820,after.timers820,sizeof after.timers820);
 copyFloat(f.damage7D0,after.damage7D0[0]);copyFloat(f.damage7D4,after.damage7D0[1]);copyFloat(f.damage7D8,after.damage7D0[2]);copyFloat(f.damage7DC,after.damage7D0[3]);
 copyFloat(z.cooldown7E0,after.cooldown7E0);copyFloat(z.periodic7E8,after.periodic7E8);
 f.vehicleCFC=after.fieldCFC;f.vehicleD00=after.fieldD00;z.fieldD04=after.fieldD04;
 z.partnerId=after.partnerId;z.partnerState=after.partnerState;
 copyFloat(z.partnerTimer08,after.partnerTimer08);copyFloat(z.partnerAmount0C,after.partnerAmount0C);copyFloat(f.objectD34_10,after.partnerDuration10);
 copyFloat(z.environmentalD38,after.environmentalD38);copyFloat(z.environmentalD3C,after.environmentalD3C);
 if(status==ImpactConsumerStatus49Z::pendingControllerFeedback){
  // Source222B20 prefix is committed up to the missing player/controller
  // binding at222D70, or up to its first virtual call when identity is bound.
  // No2228D0 damage or later timer/clear writes were executed. Caller must
  // fulfill the preceding22ADE0 event before surfacing this terminal cut.
  r.pendingImpactInput50KZ=in;
  // This sum was already validated by the prefix and its inputs are unchanged.
  volatile float lateral=f.impactDirectionalD08[6]+f.impactDirectionalD08[7];
  r.pendingImpactLateral50KZ=lateral;r.pendingImpactDuration50KZ=0.f;
  if(!commit(s,w,d,P::ImpactFeedbackPending50KZ))return false;events=next;return fail(d,E::ExternalCut,next.values[0].sourcePC);
 }
 r.impactConsumerPending49Y=0;r.impactConsumersActive49Z=1;
 // The outer manager gate jumps to228B40, beyond repair/effects/transitions.
 // CD8's inner222B20 skip instead returns to2288BC and still reaches repair.
 if(!commit(s,w,d,(in.managerFlags58&2u)?P::PostImpactConsumers:P::AfterImpactBlock))return false;events=next;return true;
}
bool resumeVehicleImpactsWithControllers50KZ(S&s,const VehicleFrameBindings49W&b,
 ControllerFeedbackState50KZ* devices,std::size_t deviceCount,VehicleImpactEvents49Z&events,
 VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 // Reject aliasing before start clears the diagnostic or stages w.next.
 // An aliased diagnostic itself cannot safely receive a failure record.
 if(overlap(&d,sizeof d,&s,sizeof s)||overlap(&d,sizeof d,&w,sizeof w)||
    overlap(&d,sizeof d,&b,sizeof b)||overlap(&d,sizeof d,&events,sizeof events))return false;
 if(deviceCount>2u)return false; // Unknown extent cannot safely exclude d/device alias.
 if(devices&&overlap(devices,deviceCount*sizeof *devices,&d,sizeof d))return false;
 if((devices&&(overlap(devices,deviceCount*sizeof *devices,&s,sizeof s)||
    overlap(devices,deviceCount*sizeof *devices,&w,sizeof w)||
    overlap(devices,deviceCount*sizeof *devices,&events,sizeof events)||
    overlap(devices,deviceCount*sizeof *devices,&b,sizeof b)))||
    overlap(&events,sizeof events,&s,sizeof s)||overlap(&events,sizeof events,&w,sizeof w)||
    overlap(&events,sizeof events,&b,sizeof b))return fail(d,E::Storage,0x222d70u);
 if(s.phase!=P::ImpactFeedbackPending50KZ&&s.phase!=P::ImpactFeedbackClearReturned50KZ&&
    s.phase!=P::ImpactFeedbackReturned50KZ)return fail(d,E::Phase,0x222d70u);
 if(!start(s,b,s.phase,w,d))return false;
 const auto& saved=s.pendingImpactInput50KZ;
 if(!saved.player5CBound50IW||!devices||saved.player5C>=deviceCount)
  return fail(d,E::ExternalCut,0x222d70u);
 // This is the real native registry object for retained source5C. An absent
 // physical device is still an object, exactly as original237C10 constructs.
 auto& device=devices[saved.player5C];
 events={};
 if(s.phase!=P::ImpactFeedbackPending50KZ){
  events.values[0]={0x222d8cu,0x40u,saved.player5C,2u,0u,0.f};events.count=1;
  if(s.phase==P::ImpactFeedbackReturned50KZ){
   events.values[1]={0x222decu,0x44u,saved.player5C,2u,0u,s.pendingImpactDuration50KZ};events.count=2;
  }
 }
 if(s.phase==P::ImpactFeedbackPending50KZ){
  if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x222d8c);if(!noiseContinuation(s,w.next,d))return false;
  clearControllerFeedback50KZ(device,2);
  events={};events.values[0]={0x222d8cu,0x40u,saved.player5C,2u,0u,0.f};events.count=1;
  if(!commit(s,w,d,P::ImpactFeedbackClearReturned50KZ))return false;
 }
 if(s.phase==P::ImpactFeedbackClearReturned50KZ){
  if(!start(s,b,P::ImpactFeedbackClearReturned50KZ,w,d))return false;
  // Source222DB0/min.s, not an arbitrary rumble strength. Its result is a
  // duration in seconds;2388B0 owns the capability gate and timer writes.
  Math math;const auto scaled=math.mul(0.005f,s.pendingImpactLateral50KZ);
  const auto duration=scaled<0.5f?scaled:0.5f;
  if(!math.valid||((bits(scaled)&0x7f800000u)==0u&&(bits(scaled)&0x7fffffu)!=0u))
   return fail(d,E::Arithmetic,0x222db0u);
  if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x222dec);if(!noiseContinuation(s,w.next,d))return false;
  if(!requestControllerFeedback50KZ(device,2,0u,duration))return fail(d,E::Arithmetic,0x222decu);
  w.next.pendingImpactDuration50KZ=duration;
  events.values[1]={0x222decu,0x44u,s.pendingImpactInput50KZ.player5C,2u,0u,duration};events.count=2;
  if(!commit(s,w,d,P::ImpactFeedbackReturned50KZ))return false;
 }
 if(!start(s,b,P::ImpactFeedbackReturned50KZ,w,d))return false;
 auto&r=w.next;auto&f=r.fields;auto&z=r.impactSupplement;
 const auto& in=r.pendingImpactInput50KZ;
 auto copyFloat=[](float&to,const float&from){std::memcpy(&to,&from,sizeof(float));};
 ImpactConsumerState49Z before{},after{};
 std::memcpy(before.directionalD08,f.impactDirectionalD08,sizeof before.directionalD08);
 std::memcpy(before.timers820,z.timers820,sizeof before.timers820);
 copyFloat(before.damage7D0[0],f.damage7D0);copyFloat(before.damage7D0[1],f.damage7D4);copyFloat(before.damage7D0[2],f.damage7D8);copyFloat(before.damage7D0[3],f.damage7DC);
 copyFloat(before.cooldown7E0,z.cooldown7E0);copyFloat(before.periodic7E8,z.periodic7E8);
 before.fieldCFC=f.vehicleCFC;before.fieldD00=f.vehicleD00;before.fieldD04=z.fieldD04;
 before.partnerId=z.partnerId;before.partnerState=z.partnerState;
 copyFloat(before.partnerTimer08,z.partnerTimer08);copyFloat(before.partnerAmount0C,z.partnerAmount0C);copyFloat(before.partnerDuration10,f.objectD34_10);
 copyFloat(before.environmentalD38,z.environmentalD38);copyFloat(before.environmentalD3C,z.environmentalD3C);

 ImpactPartnerEvents49Z partners{};
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x222df4);if(!noiseContinuation(s,w.next,d))return false;
 const auto status=resumeImpactsAfterControllerFeedback50KZ(in,before,after,
     in.dynamicPartnerEventsBound?&partners:nullptr);
 if(status==ImpactConsumerStatus49Z::unsupportedPartner)return fail(d,E::Admission,0x222a84u);
 if(status==ImpactConsumerStatus49Z::unsupportedDynamicTuning)return fail(d,E::Admission,0x222a84u);
 if(status!=ImpactConsumerStatus49Z::applied)return fail(d,E::Arithmetic,0x222df4u);
 std::memcpy(f.impactDirectionalD08,after.directionalD08,sizeof after.directionalD08);
 std::memcpy(z.timers820,after.timers820,sizeof after.timers820);
 copyFloat(f.damage7D0,after.damage7D0[0]);copyFloat(f.damage7D4,after.damage7D0[1]);copyFloat(f.damage7D8,after.damage7D0[2]);copyFloat(f.damage7DC,after.damage7D0[3]);
 copyFloat(z.cooldown7E0,after.cooldown7E0);copyFloat(z.periodic7E8,after.periodic7E8);
 f.vehicleCFC=after.fieldCFC;f.vehicleD00=after.fieldD00;z.fieldD04=after.fieldD04;
 z.partnerId=after.partnerId;z.partnerState=after.partnerState;
 copyFloat(z.partnerTimer08,after.partnerTimer08);copyFloat(z.partnerAmount0C,after.partnerAmount0C);copyFloat(f.objectD34_10,after.partnerDuration10);
 copyFloat(z.environmentalD38,after.environmentalD38);copyFloat(z.environmentalD3C,after.environmentalD3C);

 r.impactConsumerPending49Y=0;r.impactConsumersActive49Z=1;
 r.pendingImpactInput50KZ={};r.pendingImpactLateral50KZ=0.f;r.pendingImpactDuration50KZ=0.f;
 if(!noiseContinuation(s,w.next,d))return false;
 events.partners=partners;
 return commit(s,w,d,P::PostImpactConsumers);
}
bool repairVehicleImpacts50A(S&s,const VehicleFrameBindings49W&b,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::PostImpactConsumers,w,d))return false;
 auto&r=w.next;auto&f=r.fields;auto&z=r.impactSupplement;
 auto copyFloat=[](float&to,const float&from){std::memcpy(&to,&from,sizeof(float));};
 ImpactRepairInput50A in{};in.flagsB9C=r.bodies.main.controls.player.flagsB9C;
 in.componentPresence=3u|(b.visual.extra1C0Present?4u:0u)|(b.visual.extra1C4Present?8u:0u);
 copyFloat(in.factor,r.bodies.main.frameFactor);copyFloat(in.repairCF4,f.repairCF4);
 ImpactRepairState50A before{},after{};
 copyFloat(before.damage7D0[0],f.damage7D0);copyFloat(before.damage7D0[1],f.damage7D4);
 copyFloat(before.damage7D0[2],f.damage7D8);copyFloat(before.damage7D0[3],f.damage7DC);
 copyFloat(before.timer7E4,z.repairTimer7E4);copyFloat(before.environmentalD38,z.environmentalD38);copyFloat(before.environmentalD3C,z.environmentalD3C);
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x2288c8);if(!noiseContinuation(s,w.next,d))return false;
 if(repairImpactDamage50A(in,before,after)==ImpactAftermathStatus50A::arithmetic)return fail(d,E::Arithmetic,0x224370);
 copyFloat(f.damage7D0,after.damage7D0[0]);copyFloat(f.damage7D4,after.damage7D0[1]);
 copyFloat(f.damage7D8,after.damage7D0[2]);copyFloat(f.damage7DC,after.damage7D0[3]);
 copyFloat(z.repairTimer7E4,after.timer7E4);copyFloat(z.environmentalD38,after.environmentalD38);copyFloat(z.environmentalD3C,after.environmentalD3C);
 return commit(s,w,d,P::PostImpactRepair);
}
bool refreshMainVehicleFrame49W(S&s,const VehicleFrameBindings49W&b,const PreservingStage49W&cut,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::PostGraphics,w,d)||!preserving(cut,PreservingCut49W::BaseAfterGraphicsThroughBA0ResetBeforeChildren,d))return false;
 //222B20/2228D0 may consume/reset newly owned maxima and change damage/CFC/D00.
 // An old all-preserving assertion is not proof of that source execution.
 if(s.impactConsumerPending49Y||s.impactConsumersActive49Z)return fail(d,E::ExternalCut,0x2288b4);
 auto&r=w.next;auto&m=r.bodies.main;if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x228b8c);if(!noiseContinuation(s,w.next,d))return false;r.fields.impactBA0=r.visualShared.zero434630;
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x167bb4);if(!noiseContinuation(s,w.next,d))return false;
 if(!quaternionToAngles48G(m.components.root.body.orientation,m.cachedAnglesE0)||!refreshCachedSpeed48E(m.components.root.body.velocity,m.cachedSpeed34))return fail(d,E::Arithmetic,0x167bb4);
 return commit(s,w,d,P::BetweenBodyChildren);
}
bool refreshSecondaryVehicleFrame49W(S&s,const VehicleFrameBindings49W&b,const PreservingStage49W&cut,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::BetweenBodyChildren,w,d)||!preserving(cut,PreservingCut49W::OtherChildrenBetween340And35C,d))return false;
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x167bb4);if(!noiseContinuation(s,w.next,d))return false;if(!refreshSecondaryFrameCache49M(w.next.bodies.secondary,w.next.bodies.secondary))return fail(d,E::Arithmetic,0x167bb4);
 return commit(s,w,d,P::AfterBodyChildren);
}
bool scheduleVehicleFrame49W(S&s,const VehicleFrameBindings49W&b,const PreservingStage49W&cut,float scale,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::AfterBodyChildren,w,d)||!preserving(cut,PreservingCut49W::OtherChildrenAfter35C,d))return false;
 auto&m=w.next.bodies.main;if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x1a8390);if(!noiseContinuation(s,w.next,d))return false;if(!selectSchedule49K(m.frameFactor,scale,m.schedule))return fail(d,E::Input,0x1a8390);
 m.liveSubstepScale=scale;m.phase=MainBodyPhase49K::SubstepReady;w.next.bodies.phase=CoupledBodyPhase49M::SubstepReady;return commit(s,w,d,P::SubstepReady);
}
bool beginVehicleSubstep49W(S&s,const VehicleFrameBindings49W&b,const SubstepSelectionContext49K&x,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::SubstepReady,w,d))return false;auto&r=w.next;auto&m=r.bodies.main;V forward{},up{};r.fields.gravity378=x.vehicle378;r.fields.gravity37C=x.vehicle37C;
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x21990c);if(!noiseContinuation(s,w.next,d))return false;
 const bool getter=m.components.probe348.field104&&!(m.components.bindings.aeroBrakeEC>0&&m.cachedSpeed34<fp(0x4212aa7f));
 if(getter){if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x2199b8);if(!noiseContinuation(s,w.next,d))return false;if(!axes(r,b.lifecycle.main190,forward,up))return fail(d,E::Arithmetic,0x2199b8);project(r,b);}
 const GravitySelection49K g{m.components.probe348.field104,m.components.bindings.aeroBrakeEC,m.cachedSpeed34,getter?up.v[1]:0,r.fields.gravity378,r.fields.gravity37C,x.effective434730,x.effective434738};
 if(!selectGravity49K(g,m.selectedGravity))return fail(d,E::Input,0x21990c);m.selectedEngineFlags=selectEngineFlags49K(x.vehicle64,m.controls.player.flagsB9C);
 if(r.terrainProbe.enabled)r.terrainProbe.stage=0;
 m.phase=MainBodyPhase49K::AwaitingWorldPreparation;r.bodies.phase=CoupledBodyPhase49M::AwaitingWorldPreparation;return commit(s,w,d,P::PostWorld);
}
bool enableVehicleTerrain50FP(S& s,const VehicleFrameBindings49W& b,
 const TerrainPrepareState48M& initial,const TerrainFollowParameters48G& parameters,
 VehicleFrameWorkspace49W& w,VehicleFrameDiagnostic49W& d) noexcept {
 if(overlap(&parameters,sizeof parameters,&s,sizeof s)||overlap(&parameters,sizeof parameters,&w,sizeof w)||
    overlap(&parameters,sizeof parameters,&d,sizeof d))return fail(d,E::Storage,0x1b1240);
 if(overlap(&initial,sizeof initial,&s,sizeof s)||overlap(&initial,sizeof initial,&w,sizeof w)||
    overlap(&initial,sizeof initial,&d,sizeof d))return fail(d,E::Storage,0x1b1240);
 if(!start(s,b,P::Ready,w,d))return false;
 if(s.terrainProbe.enabled||s.bodies.main.domain.probe100)return fail(d,E::Admission,0x1b1240);
 for(float value:parameters.localOffset1c0.v)if(!std::isfinite(value))return fail(d,E::Input,0x1b1240);
 if(!std::isfinite(parameters.airborneTorque204)||!std::isfinite(parameters.detachAngle208)||
    !std::isfinite(parameters.detachDistance20c))return fail(d,E::Input,0x1b1240);
 for(const auto* vector:{&initial.follow.previousPoint180,&initial.follow.previousNormal190,
     &initial.follow.largestVelocityChange1d0,&initial.terrain.surface.point150,&initial.terrain.surface.normal170})
  for(float value:vector->v)if(!std::isfinite(value))return fail(d,E::Input,0x1b1240);
 for(float value:initial.auxiliaryPoint160)if(!std::isfinite(value))return fail(d,E::Input,0x1b1240);
 if(initial.terrain.collectedCount>16u)return fail(d,E::Input,0x1b1240);
 auto& r=w.next;r.terrainProbe={};r.terrainProbe.enabled=true;r.terrainProbe.response=parameters;
 installTerrainSurface50FP(r,initial.terrain);
 auto& probe=r.bodies.main.components.probe348;probe.field104=initial.follow.active104;
 probe.field10C=initial.follow.pending10c;probe.field11C=initial.follow.detached11c;
 probe.field118=initial.auxiliaryAbove118;probe.point150[3]=1.f;probe.vector1D0[3]=1.f;
 std::memcpy(probe.vector1D0,initial.follow.largestVelocityChange1d0.v,12);
 r.terrainProbe.previousPoint180=initial.follow.previousPoint180;r.terrainProbe.previousNormal190=initial.follow.previousNormal190;
 std::memcpy(r.terrainProbe.waterPoint160.v,initial.auxiliaryPoint160,16);
 r.bodies.main.domain.probe100=1u;return commit(s,w,d,P::Ready);
}
bool bindVehicleTerrainWater50JC(S& s,VehicleFrameBindings49W& b,
 const TerrainWaterBinding50JC& water,VehicleFrameDiagnostic49W& d) noexcept {
 if(overlap(&s,sizeof s,&b,sizeof b)||overlap(&d,sizeof d,&s,sizeof s)||
    overlap(&d,sizeof d,&b,sizeof b)||overlap(&water,sizeof water,&s,sizeof s)||
    overlap(&water,sizeof water,&b,sizeof b)||overlap(&water,sizeof water,&d,sizeof d))return false;
 d={E::None,0,GraphBodyDiagnostic49W::None};
 if(s.phase!=P::Ready||!phases(s))return fail(d,E::Phase,0x21207c);
 if(!admit(s,b,d))return false;
 if(!s.terrainProbe.enabled||b.terrainWater50JC||s.bodies.main.domain.probe124Present||
    s.bodies.main.domain.probe120Present||!coherentTerrainWater50JC(water))
  return fail(d,E::Admission,0x1b120c);
 for(const auto& read:{std::pair<const void*,std::size_t>{water.probe124,sizeof(*water.probe124)},
      {water.globals,sizeof(*water.globals)}})
  if(overlap(read.first,read.second,&s,sizeof s)||overlap(read.first,read.second,&b,sizeof b)||
     overlap(read.first,read.second,&d,sizeof d))return fail(d,E::Storage,0x1b120c);
 b.terrainWater50JC=&water;s.bodies.main.domain.probe124Present=true;return true;
}
bool prepareVehicleTerrain50FP(S& s,const VehicleFrameBindings49W& b,const VehicleFrameParameters49W& parameters,
 const LocalSegment48I& segment,TerrainPrepareState48M& output,
 VehicleFrameWorkspace49W& w,VehicleFrameDiagnostic49W& d) noexcept {
 for(const auto& read:{std::pair<const void*,std::size_t>{&parameters,sizeof parameters},{&segment,sizeof segment}})
  if(overlap(read.first,read.second,&s,sizeof s)||overlap(read.first,read.second,&w,sizeof w)||
     overlap(read.first,read.second,&d,sizeof d)||overlap(read.first,read.second,&output,sizeof output))return fail(d,E::Storage,0x1b1800);
 if(overlap(&output,sizeof output,&s,sizeof s)||overlap(&output,sizeof output,&w,sizeof w)||
    overlap(&output,sizeof output,&b,sizeof b)||overlap(&output,sizeof output,&d,sizeof d))return fail(d,E::Storage,0x1b1800);
 if(!start(s,b,P::PostWorld,w,d))return false;
 if(!s.terrainProbe.enabled||s.terrainProbe.stage!=0u)return fail(d,E::Phase,0x1b1800);
 //48M clears the three represented lanes to the initialized434260 zero.
 constexpr std::uint32_t clear[4]={0,0,0,0x3f800000u};
 if(!same(parameters.bodies.main.effective434260,clear,16))return fail(d,E::Admission,0x1b1800);
 TerrainPrepareInput48M input{};input.state.follow=terrainFollow50FP(s);input.state.terrain=terrainSurface50FP(s);
 input.state.segment=segment;input.state.auxiliaryAbove118=s.bodies.main.components.probe348.field118;
 std::memcpy(input.state.auxiliaryPoint160,s.terrainProbe.waterPoint160.v,16);
 input.auxiliarySource120=s.bodies.main.domain.probe120Present;
 input.auxiliarySource124=s.bodies.main.domain.probe124Present;input.water124=b.terrainWater50JC;
 input.scene=node(s,b.lifecycle.main190).world100;
 std::memcpy(input.lowerEndpoint140.v,parameters.bodies.main.probeLocal140,12);
 TerrainPrepareState48M prepared{};
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x1b1800);if(!noiseContinuation(s,w.next,d))return false;
 WaterHeightResult50JC waterDiagnostic{};
 if(prepareTerrainSinglePair48M(input,prepared,&waterDiagnostic)!=TerrainPrepareStatus48M::QueryReady)
  return fail(d,E::Query,waterDiagnostic.sourcePC?waterDiagnostic.sourcePC:0x1b1800);
 auto& r=w.next;auto& probe=r.bodies.main.components.probe348;
 installTerrainSurface50FP(r,prepared.terrain);probe.point150[3]=1.f;
 probe.field118=prepared.auxiliaryAbove118;std::memcpy(probe.vector1D0,clear,16);
 std::memcpy(r.terrainProbe.waterPoint160.v,prepared.auxiliaryPoint160,16);
 r.terrainProbe.stage=1;if(!commit(s,w,d,P::PostWorld))return false;output=prepared;return true;
}
bool commitVehicleTerrainQuery50FP(S& s,const VehicleFrameBindings49W& b,const TerrainQueryResult48M& query,
 VehicleFrameWorkspace49W& w,VehicleFrameDiagnostic49W& d) noexcept {
 if(overlap(&query,sizeof query,&s,sizeof s)||overlap(&query,sizeof query,&w,sizeof w)||
    overlap(&query,sizeof query,&d,sizeof d))return fail(d,E::Storage,0x1b19b4);
 if(!start(s,b,P::PostWorld,w,d))return false;
 if(!s.terrainProbe.enabled||s.terrainProbe.stage!=1u)return fail(d,E::Phase,0x1b19b4);
 if(query.terrain.collectedCount>16u)return fail(d,E::Query,0x1b19b4);
 for(float value:query.terrain.surface.point150.v)if(!std::isfinite(value))return fail(d,E::Input,0x1b19b4);
 for(float value:query.terrain.surface.normal170.v)if(!std::isfinite(value))return fail(d,E::Input,0x1b19b4);
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x1b19b4);if(!noiseContinuation(s,w.next,d))return false;installTerrainSurface50FP(w.next,query.terrain);
 w.next.terrainProbe.stage=2;return commit(s,w,d,P::PostWorld);
}
bool produceVehicleForces49W(S&s,const VehicleFrameBindings49W&b,const VehicleFrameParameters49W&p,const PostWorld49W&x,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::PostWorld,w,d))return false;auto&r=w.next;auto&m=r.bodies.main;
 const auto expected=m.substepIndex==0?(r.bodies.vehicle838?WorldCut49W::First358Secondary36CAnd219420:WorldCut49W::First358And219420):(m.vehicle380?WorldCut49W::LaterRequired358And219420:WorldCut49W::LaterSkipped);
 if(x.point!=expected)return fail(d,E::ExternalCut,0x219810);
 if(expected==WorldCut49W::LaterSkipped){const auto before=contactEffects49W(s);if(!sameEffects(before,x.effects))return fail(d,E::ExternalCut,0x219c78);}
 install(r,x.effects);r.fields.applicationByte58=x.applicationGateByte58;auto&c=m.components;const auto&par=p.bodies.main;
 if(r.terrainProbe.enabled){if(r.terrainProbe.stage!=2u)return fail(d,E::Query,0x1b1800);}
 else{if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x1b1800);if(!noiseContinuation(s,w.next,d))return false;if(!prepareDisabledProbe49K(c.probe348,par.probeLocal140,node(r,b.lifecycle.main190).world100,par.effective434260,c.probe348))return fail(d,E::Arithmetic,0x1b1800);}
 EngineInput48D en{};AeroInput48E ai{};en.bodySpeed=m.cachedSpeed34;en.boost=m.selectedEngineFlags.engine34!=0;en.repair=m.selectedEngineFlags.engine38!=0;en.fullTorque=c.feedback.engine3C!=0;ai.bodyLocalVelocity=c.root.localVelocity80;ai.angularVelocity=c.root.body.angularVelocity;ai.bodyAngleE0=m.cachedAnglesE0.v[0];ai.subtractWorldFlow=x.aeroSubtractWorldFlow;ai.worldFlow40=x.aeroWorldFlow40;ai.triggerD0=c.feedback.aeroD0!=0;ai.fullSteeringE0=c.feedback.aeroE0!=0;
 if(!applyControlModelBindings48W(c.bindings,en,ai,en,ai))return fail(d,E::Input,0x219c84);
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x2e7720);if(!noiseContinuation(s,w.next,d))return false;if(!updateEngine48D(c.engine,par.engine,en,c.engine))return fail(d,E::Arithmetic,0x2e7720);
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x2827d0);if(!noiseContinuation(s,w.next,d))return false;if(!accumulateEngineGravity48D(c.root.loads,c.engine,basis(node(r,b.lifecycle.main190).world100),{m.selectedGravity,par.gravityForceX,par.gravityForceZ},par.body.mass,false,true,c.root.loads))return fail(d,E::Arithmetic,0x2827d0);
 Math f;detail::SceneVectorContext49U scratch{0};V v{};
 if(x.applicationGateByte58&2u){for(unsigned k=0;k<2;++k){if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(k?0x2e7c98:0x2e7c3c);if(!noiseContinuation(s,w.next,d))return false;if(!detail::direction49U(graph(r),b.lifecycle.main190,vector(c.engine.localThrust[k]),scratch,v))return fail(d,E::Arithmetic,0x2e7c10);f.addLoad(c.root.loads.force,v);}for(const auto&t:c.engine.torque)f.addLoad(c.root.loads.torque,vector(t));}
 V forward{},up{};if(ai.subtractWorldFlow){if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x16ce10);if(!noiseContinuation(s,w.next,d))return false;if(!detail::inverseDirection49U(graph(r),b.lifecycle.main190,vector(ai.worldFlow40),scratch,v))return fail(d,E::Arithmetic,0x16ce10);}
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x16cf58);if(!noiseContinuation(s,w.next,d))return false;if(!axes(r,b.lifecycle.main190,forward,up))return fail(d,E::Arithmetic,0x16cf58);
 AeroResult48E result{};if(!applyAerodynamics48E(c.aero,par.aero,ai,basis(node(r,b.lifecycle.main190).world100),c.root.loads,m.schedule.factorPerSubstep,result))return fail(d,E::Arithmetic,0x16cd90);
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x16d7c0);if(!noiseContinuation(s,w.next,d))return false;if(!detail::direction49U(graph(r),b.lifecycle.main190,vector(result.localForce),scratch,v)||!f.valid)return fail(d,E::Arithmetic,0x16d7c0);
 c.aero=result.state;c.root.loads=result.accumulatedLoads;project(r,b);m.phase=MainBodyPhase49K::AwaitingResponse354;r.bodies.phase=CoupledBodyPhase49M::AwaitingResponse354;return commit(s,w,d,P::Post354);
}
bool respondVehicle354_49W(S&s,const VehicleFrameBindings49W&b,const VehicleFrameParameters49W&p,const Post354_49W&x,const SecondaryInputs49M&secondary,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::Post354,w,d))return false;auto&r=w.next;install(r,x.effects);auto&m=r.bodies.main;auto&c=m.components;DisabledProbeResponse49K response{};const auto&par=p.bodies.main;
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x1b1a30);if(!noiseContinuation(s,w.next,d))return false;
 if(r.terrainProbe.enabled){if(!respondEnabledTerrain50FP(r,b))return fail(d,E::Arithmetic,0x1b1a30);}
 else{if(!respondDisabledProbe49K(c.probe348,m.cachedAnglesE0.v[2],par.probe204,par.effective4342C8,par.effective434290,c.root.loads.torque,response))return fail(d,E::Arithmetic,0x1b1a30);c.probe348=response.state;c.root.loads.torque=response.torque190;}
 m.response354_3C=x.response3C;m.response354_44=x.response44;r.bodies.vehicle838=x.vehicle838;
 if(x.vehicle838){if(!secondaryForces(s,r,b,p,secondary,d)){
   if(d.error!=E::None)return false;
   return fail(d,E::Arithmetic,0x2188b0);
  }r.bodies.phase=CoupledBodyPhase49M::AwaitingResponse368;return commit(s,w,d,P::Post368);}
 r.bodies.phase=CoupledBodyPhase49M::AwaitingMainIntegration;return commit(s,w,d,P::MainIntegration);
}
bool respondVehicle368_49W(S&s,const VehicleFrameBindings49W&b,const VehicleFrameParameters49W&p,const ContactEffects49W&x,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::Post368,w,d))return false;auto&r=w.next;install(r,x);auto&z=r.bodies.secondary;const auto&par=p.bodies.secondary;
 if(z.phase!=SecondaryPhase49M::Response368Pending)return fail(d,E::Phase,0x2190fc);
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x2190fc);if(!noiseContinuation(s,w.next,d))return false;if(!advanceGraphBody49W(z.root,r.secondarySupplement,r.secondaryExtra,{z.velocity70W,z.angularVelocityB0W,z.force180W,z.torque190W},z.sceneFlags190,par.body,par.centerOffset,b.lifecycle.physical19C,z.enabled,b.secondaryDomain,r.bodies.main.schedule.factorPerSubstep,graph(r),w.body,d.body))return fail(d,E::Body,0x2190fc);
 z.phase=SecondaryPhase49M::Ready;r.bodies.phase=CoupledBodyPhase49M::AwaitingMainIntegration;project(r,b);return commit(s,w,d,P::MainIntegration);
}

namespace {
DynamicImpactState50IW dynamicImpactState50IW(const S&s,const BasePoseHistoryState50Y&h) noexcept {
 const auto&f=s.fields;const auto&z=s.impactSupplement;DynamicImpactState50IW n{};
 n.vehicleCFC=f.vehicleCFC;n.vehicleD00=f.vehicleD00;n.fieldD04=z.fieldD04;
 n.timerD2CBits=h.timerD2C;n.markerD30=h.markerD30;n.partnerId=z.partnerId;n.partnerState=z.partnerState;
 n.partnerTimer08=z.partnerTimer08;n.partnerAmount0C=z.partnerAmount0C;n.partnerDuration10=f.objectD34_10;
 n.flagB9D0=f.flagB9D0;return n;
}
void installDynamicImpactState50IW(S&s,BasePoseHistoryState50Y&h,const DynamicImpactState50IW&n) noexcept {
 auto&f=s.fields;auto&z=s.impactSupplement;
 f.vehicleCFC=n.vehicleCFC;f.vehicleD00=n.vehicleD00;z.fieldD04=n.fieldD04;
 h.timerD2C=n.timerD2CBits;h.markerD30=n.markerD30;z.partnerId=n.partnerId;z.partnerState=n.partnerState;
 z.partnerTimer08=n.partnerTimer08;z.partnerAmount0C=n.partnerAmount0C;f.objectD34_10=n.partnerDuration10;
 f.flagB9D0=n.flagB9D0;
}
DynamicVehicleImpactStatus50IW finishDynamicImpact50IW(S&s,BasePoseHistoryState50Y&h,
 DynamicVehicleImpactCursor50IW&call,const DynamicImpactResult50IW&result,
 const DynamicImpactState50IW&next,VehicleFrameDiagnostic49W&d) noexcept {
 using T=DynamicVehicleImpactStatus50IW;
 if(result.status==DynamicImpactStatus50IW::unsupported||result.status==DynamicImpactStatus50IW::arithmetic){
  fail(d,result.status==DynamicImpactStatus50IW::arithmetic?E::Arithmetic:E::Admission,result.sourcePC);return T::Rejected;
 }
 installDynamicImpactState50IW(s,h,next);call.prefix=next;
 if(result.status==DynamicImpactStatus50IW::needsFirstContact22A400){call.awaitingFirstContact=true;return T::NeedFirstContact;}
 call.awaitingFirstContact=false;call.source.vehicleCFC=s.fields.vehicleCFC;
 StaticImpactState49Y before{},after{};
 std::memcpy(before.directionalD08,s.fields.impactDirectionalD08,sizeof before.directionalD08);
 std::memcpy(before.impactBA0,s.fields.impactBA0.v,sizeof before.impactBA0);
 const auto applied=applyImpactSeverity50IW(call.source,call.geometry,result.severity,before,after);
 if(applied!=StaticImpactStatus49Y::applied){fail(d,E::Arithmetic,0x22313c);return T::Rejected;}
 std::memcpy(s.fields.impactDirectionalD08,after.directionalD08,sizeof after.directionalD08);
 std::memcpy(s.fields.impactBA0.v,after.impactBA0,sizeof after.impactBA0);
 s.impactConsumerPending49Y=1;s.dynamicImpactRecorded50IW=true;return T::Complete;
}
}
DynamicVehicleImpactStatus50IW beginVehicleDynamicImpact50IW(S&s,
 const VehicleFrameBindings49W&b,const VehicleFrameParameters49W&p,
 BasePoseHistoryState50Y&history,const DynamicImpactManagerView50IW&peers,
 const DynamicImpactTuning50IW&tuning,const DynamicImpactProfile50IW&profile,
 DynamicVehicleImpactCursor50IW&call,VehicleFrameDiagnostic49W&d) noexcept {
 using T=DynamicVehicleImpactStatus50IW;call={};d={};
 if(s.phase!=P::MainIntegration||!s.fields.dynamicOwnersBound50IW||s.dynamicImpactRecorded50IW){
  fail(d,E::Phase,0x219d98);return T::Rejected;}
 const auto&m=s.bodies.main;const auto&body=m.components;
 bool impact=false;
 for(unsigned axis=0;axis<3;++axis){
  if(!std::isfinite(s.visualShared.zero434630.v[axis])){fail(d,E::Admission,0x219d98);return T::Rejected;}
  if(body.root.accumulator1c0.v[axis]!=s.visualShared.zero434630.v[axis]){impact=true;break;}
 }
 if(!impact||(s.fields.impactFlagsCD8&1u)||m.vehicleState40==3u||m.vehicleState40==4u)return T::Complete;
 if(!body.body20C)return T::Complete;
 if(body.body20C>BodyContactCapacity48R){fail(d,E::Admission,0x223110);return T::Rejected;}
 if(s.terrainProbe.enabled){
  if(!refreshSceneWorld49P(graph(s),b.lifecycle.main190)){fail(d,E::Graph,0x223030);return T::Rejected;}
  project(s,b);
 }
 auto&in=call.source;in.vehicle40=m.vehicleState40;in.vehicle58=s.fields.vehicle58;
 in.bodyContactPartnerCount20C=body.body20C;in.flagsCD8=s.fields.impactFlagsCD8;
 in.sceneFlags190=node(s,b.lifecycle.main190).sceneFlags190;in.readyMainScene190=node(s,b.lifecycle.main190).world100;
 std::memcpy(in.accumulatedImpact1C0,body.root.accumulator1c0.v,12);
 std::memcpy(&in.accumulatedImpact1C0[3],&s.mainExtra.accumulator1C0W,4);
 in.reductionCF0=s.fields.defenseCF0;in.tuning=p.impactTuning;in.priorVf9W=0;
 const auto geometry=prepareImpactGeometry50IW(in,call.geometry);
 if(geometry!=StaticImpactStatus49Y::applied){fail(d,geometry==StaticImpactStatus49Y::arithmetic?E::Arithmetic:E::Admission,0x223030);return T::Rejected;}
 auto&dynamic=call.input;dynamic.manager338=peers;dynamic.managerB0=peers;
 dynamic.contacts.count20C=static_cast<std::int32_t>(body.body20C);
 std::memcpy(dynamic.contacts.slots210.data(),s.mainContactPartners210,sizeof s.mainContactPartners210);
 dynamic.self={s.fields.vehicle58,m.controls.player.raw.values[0],m.controls.bound.control.shaping.smoothedChannel0D28,
  body.root.worldAngularVelocityA0.v[1],m.cachedSpeed34,s.fields.defenseCF0,
  s.fields.damage7D0,s.fields.damage7D4,s.fields.damage7DC,true};
 dynamic.tuning=tuning;dynamic.profile=profile;dynamic.environmentTuning=p.impactTuning;
 dynamic.environmentTuningBound=p.impactTuningBound;dynamic.worldMagnitude=call.geometry.magnitude;
 dynamic.normalizedLocalX=call.geometry.normal[0];dynamic.normalizedLocalZ=call.geometry.normal[2];
 const auto before=dynamicImpactState50IW(s,history);DynamicImpactState50IW after{};
 const auto result=beginDynamicImpact50IW(dynamic,before,after);
 return finishDynamicImpact50IW(s,history,call,result,after,d);
}
DynamicVehicleImpactStatus50IW resumeVehicleDynamicImpact50IW(S&s,BasePoseHistoryState50Y&history,
 DynamicVehicleImpactCursor50IW&call,VehicleFrameDiagnostic49W&d) noexcept {
 using T=DynamicVehicleImpactStatus50IW;
 if(s.phase!=P::MainIntegration||!s.fields.dynamicOwnersBound50IW||!call.awaitingFirstContact||s.dynamicImpactRecorded50IW){
  fail(d,E::Phase,0x222fa0);return T::Rejected;}
 DynamicImpactState50IW after{};const auto result=resumeDynamicImpactAfter22A40050IW(call.input,call.prefix,after);
 return finishDynamicImpact50IW(s,history,call,result,after,d);
}

bool integrateVehicleMain49W(S&s,const VehicleFrameBindings49W&b,const VehicleFrameParameters49W&p,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::MainIntegration,w,d))return false;auto&r=w.next;auto&m=r.bodies.main;auto&c=m.components;auto&lanes=r.bodies.mainSphere;const auto&par=p.bodies.main;
 bool impact=false;
 //219D1C..219D90 short-circuits X,Y,Z against the canonical shared sentinel.
 // Numeric equality intentionally treats either sign of zero as equal.
 for(unsigned i=0;i<3;++i){
  if(!std::isfinite(r.visualShared.zero434630.v[i]))return fail(d,E::Admission,0x219d98);
  if(c.root.accumulator1c0.v[i]!=r.visualShared.zero434630.v[i]){impact=true;break;}
 }
 if(impact){
  if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x219d98);if(!noiseContinuation(s,w.next,d))return false;
  if(r.dynamicImpactRecorded50IW){
   if(!r.fields.dynamicOwnersBound50IW)return fail(d,E::Admission,0x223110);
   r.dynamicImpactRecorded50IW=false;
  }else{
  // The entry skip consumes neither tuning nor defense nor the partner list.
  if(!(r.fields.impactFlagsCD8&1u)&&m.vehicleState40!=3&&m.vehicleState40!=4){
   if(!p.impactTuningBound)return fail(d,E::Admission,0x223030);
   //223030 calls the real28BBB0 getter. Enabled terrain may have dirtied190.
   if(r.terrainProbe.enabled){if(!refreshSceneWorld49P(graph(r),b.lifecycle.main190))return fail(d,E::Graph,0x223030);project(r,b);}
   StaticImpactInput49Y in{};in.vehicle40=m.vehicleState40;in.vehicle58=r.fields.vehicle58;
   in.vehicleCFC=r.fields.vehicleCFC;in.partnerState=r.impactSupplement.partnerState;
   in.damage7D0=r.fields.damage7D0;in.damage7D4=r.fields.damage7D4;in.damage7DC=r.fields.damage7DC;
   in.aiOwnersBound=r.fields.nativeAiOwnersBound50IV;
   in.bodyContactPartnerCount20C=c.body20C;in.flagsCD8=r.fields.impactFlagsCD8;
   in.sceneFlags190=node(r,b.lifecycle.main190).sceneFlags190;
   in.readyMainScene190=node(r,b.lifecycle.main190).world100;
   std::memcpy(in.accumulatedImpact1C0,c.root.accumulator1c0.v,12);
   std::memcpy(&in.accumulatedImpact1C0[3],&r.mainExtra.accumulator1C0W,4);
   in.reductionCF0=r.fields.defenseCF0;in.tuning=p.impactTuning;
   // Only XYZ/state is projected. This finite scratch representative cannot
   // affect it; true source VF9.W and late BB0.W are not invented game state.
   in.priorVf9W=0;float scratchW=0;StaticImpactState49Y previous{},next{};
   std::memcpy(previous.directionalD08,r.fields.impactDirectionalD08,32);
   std::memcpy(previous.impactBA0,r.fields.impactBA0.v,16);
   const auto status=recordStaticImpact49Y(in,previous,next,scratchW);
   if(status==StaticImpactStatus49Y::unsupported)return fail(d,E::Admission,0x223030);
   if(status==StaticImpactStatus49Y::arithmetic)return fail(d,E::Arithmetic,0x223030);
   std::memcpy(r.fields.impactDirectionalD08,next.directionalD08,32);
   std::memcpy(r.fields.impactBA0.v,next.impactBA0,16);
   r.impactConsumerPending49Y=1;
  }
  }
 }
 if(r.dynamicImpactRecorded50IW)return fail(d,E::Phase,0x219d98);
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x219e80);if(!noiseContinuation(s,w.next,d))return false;if(!advanceGraphBody49W(c.root,r.mainSupplement,r.mainExtra,{lanes.velocity70W,lanes.angularVelocityB0W,lanes.force180W,lanes.torque190W},lanes.sceneFlags190,par.body,par.centerOffset,b.lifecycle.main190,m.domain.bodyEnabled,b.mainDomain,m.schedule.factorPerSubstep,graph(r),w.body,d.body))return fail(d,E::Body,0x219e80);
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x219e94);if(!noiseContinuation(s,w.next,d))return false;clearMainBodyPost20C49K(c.body20C);
 //28B1A0(null) writes XYZ only.7CC survives, including copy-only payloads.
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x219170);if(!noiseContinuation(s,w.next,d))return false;if(!refreshSceneWorld49P(graph(r),b.lifecycle.main190))return fail(d,E::Graph,0x219170);
 for(unsigned i=0;i<3;++i)m.currentPoint7C0[i]=node(r,b.lifecycle.main190).world100.v[12+i];
 if(m.priorPoint7B0[0]==r.visualShared.zero434630.v[0]&&m.priorPoint7B0[1]==r.visualShared.zero434630.v[1]&&m.priorPoint7B0[2]==r.visualShared.zero434630.v[2])std::memcpy(m.priorPoint7B0,m.currentPoint7C0,16);
 project(r,b);m.phase=MainBodyPhase49K::AwaitingQuery219120;r.bodies.phase=CoupledBodyPhase49M::AwaitingQuery219120;return commit(s,w,d,P::Query219120);
}
static bool finishMovementTail50M(S&s,const VehicleFrameBindings49W&b,const VehicleFrameParameters49W&p,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 auto&r=w.next;auto&m=r.bodies.main;m.vehicle380=selectVehicle38049K(m.response354_44);
 // Only XYZ is represented/consumed in this admission. The scratch W seed is
 // immaterial to XYZ; no outputW claim or invented BB0.W=1 is made.
 detail::SceneVectorContext49U scratch{0};V inverse{};if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x219f80);if(!noiseContinuation(s,w.next,d))return false;
 if(!detail::inverseDirection49U(graph(r),b.lifecycle.main190,r.fields.impactBA0,scratch,inverse))return fail(d,E::Arithmetic,0x219f80);
 for(unsigned i=0;i<3;++i)r.fields.inverseImpactBB0XYZ.v[i]=inverse.v[i];
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x219f90);if(!noiseContinuation(s,w.next,d))return false;m.components.feedback=selectLateFeedback49K(m.components.probe348.field104,m.response354_3C,m.response354_44);
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x219fd8);if(!noiseContinuation(s,w.next,d))return false;
 if(r.shape.registrationE0){
  // Actual typed694 owner, after original movement/feedback prefix. The shape
  // numeric preparation and live registry have distinct source commits.
  if(!b.podLiveQuery50IZ||b.podLiveQuery50IZ->source!=&p.shape){
   project(r,b);
   if(!commit(s,w,d,P::PodLivePending50IZ))return false;
   return fail(d,E::Admission,0x219fd8u);
  }
  const auto updated=updateRegisteredPodShape50IZ(r.shape,
      node(r,b.lifecycle.main190).world100,*b.podLiveQuery50IZ);
  if(updated.status!=PodLiveQueryStatus50IZ::Complete){
   // Retain completed source-prefix work in an explicit pending phase. There
   // is no represented resume yet: a full movement-tail replay is rejected.
   project(r,b);
   if(!commit(s,w,d,P::PodLivePending50IZ))return false;
   return fail(d,E::Shape,updated.sourcePC?updated.sourcePC:0x219fd8u);
  }
 }else if(!transformPodShape49W(p.shape,node(r,b.lifecycle.main190).world100,r.shape))
  return fail(d,E::Shape,0x219fd8);
 project(r,b);++m.substepIndex;const bool done=m.substepIndex==m.schedule.count;
 m.phase=done?MainBodyPhase49K::FrameReady:MainBodyPhase49K::SubstepReady;r.bodies.phase=done?CoupledBodyPhase49M::FrameReady:CoupledBodyPhase49M::SubstepReady;return commit(s,w,d,done?P::Ready:P::SubstepReady);
}
bool finishVehicleQuery49W(S&s,const VehicleFrameBindings49W&b,const VehicleFrameParameters49W&p,const NoHitQuery49W&x,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::Query219120,w,d))return false;
 if((x.resultFlagsF8&0x20u)||!x.representedStatePreserved||!x.omittedBB0WNotConsumed)return fail(d,E::Query,0x219228);
 auto&m=w.next.bodies.main;if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x219400);if(!noiseContinuation(s,w.next,d))return false;std::memcpy(m.priorPoint7B0,m.currentPoint7C0,16);
 return finishMovementTail50M(s,b,p,w,d);
}
bool finishVehicleMovementQuery50M(S&s,const VehicleFrameBindings49W&b,const VehicleFrameParameters49W&p,const MovementQueryCompletion50M&x,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(overlap(&x,sizeof x,&s,sizeof s)||overlap(&x,sizeof x,&w,sizeof w)||overlap(&x,sizeof x,&d,sizeof d)||overlap(&p,sizeof p,&s,sizeof s)||overlap(&p,sizeof p,&w,sizeof w)||overlap(&p,sizeof p,&d,sizeof d))return false;
 if(!start(s,b,P::Query219120,w,d))return false;
 if(!x.representedStatePreserved||!x.omittedBB0WNotConsumed)return fail(d,E::Query,0x219228);
 auto&r=w.next;auto&m=r.bodies.main;
 if(x.resultFlagsF8&0x20u){
  SpringVector4_48N current{},selected{},corrected{};std::memcpy(&current,m.currentPoint7C0,16);std::memcpy(&selected,&x.selectedPoint,16);
  if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x219230);if(!noiseContinuation(s,w.next,d))return false;
  if(!computeMovementHitPoint50M(current,selected,x.pushDistance434710,corrected))return fail(d,E::Arithmetic,0x2192a4);
  auto local=node(r,b.lifecycle.main190).localC0;for(unsigned i=0;i<3;++i)local.v[12+i]=corrected.v[i];
  if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x219344);if(!noiseContinuation(s,w.next,d))return false;
  if(!setLocalMatrix49P(graph(r),b.lifecycle.main190,local))return fail(d,E::Graph,0x219344);
  // The existing frame admission proves parentless main190 and null19C observer.
  // The original28ADD0 writes XYZ only, with the same mark/invalidation order;
  // all copied basis and W words above retain their exact old representations.
  if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x219350);if(!noiseContinuation(s,w.next,d))return false; // actual28EA20 null-observer branch
  if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x2193fc);if(!noiseContinuation(s,w.next,d))return false;std::memcpy(m.priorPoint7B0,&corrected,16);
  // The original returned distance ratio is not consumed by219EA4 onward.
 }else {if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x219400);if(!noiseContinuation(s,w.next,d))return false;std::memcpy(m.priorPoint7B0,m.currentPoint7C0,16);}
 return finishMovementTail50M(s,b,p,w,d);
}
bool effectsVehicleImpacts50O(S&s,const VehicleFrameBindings49W&b,const ImpactEffectsInput50B&owners,
 ImpactEffectsWorkspace50B&effectsWorkspace,ImpactEffectEvent50B*events,std::size_t capacity,std::size_t&count,
 VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 struct Span {std::uintptr_t a,z;};Span writes[6]{};unsigned used=0;
 auto write=[&](const void*p,std::size_t n,std::size_t size,std::size_t align){
  if(!n)return true;const auto a=reinterpret_cast<std::uintptr_t>(p);
  if(!p||a%align||n>std::numeric_limits<std::size_t>::max()/size)return false;const auto bytes=n*size;
  if(bytes>std::numeric_limits<std::uintptr_t>::max()-a)return false;Span span{a,a+bytes};
  for(unsigned i=0;i<used;++i)if(span.a<writes[i].z&&writes[i].a<span.z)return false;writes[used++]=span;return true;
 };
 if(!write(&s,1,sizeof s,alignof(S))||!write(&w,1,sizeof w,alignof(VehicleFrameWorkspace49W))||
    !write(&d,1,sizeof d,alignof(VehicleFrameDiagnostic49W))||!write(&effectsWorkspace,1,sizeof effectsWorkspace,alignof(ImpactEffectsWorkspace50B))||
    !write(events,capacity,sizeof(*events),alignof(ImpactEffectEvent50B))||!write(&count,1,sizeof count,alignof(std::size_t)))return false;
 auto read=[&](const void*p,std::size_t n,std::size_t size){if(!n)return true;const auto a=reinterpret_cast<std::uintptr_t>(p);if(!p||n>std::numeric_limits<std::size_t>::max()/size)return false;const auto bytes=n*size;if(bytes>std::numeric_limits<std::uintptr_t>::max()-a)return false;for(unsigned i=0;i<used;++i)if(a<writes[i].z&&writes[i].a<a+bytes)return false;return true;};
 if(!read(&b,1,sizeof b)||!read(&owners,1,sizeof owners)||!read(owners.damage00Matches,owners.nameCount,1))return false;
 if(!start(s,b,P::PostImpactRepair,w,d))return false;
 //223540 visits 1BC,1B8,1C4,1C0 in that order. Optional presence must
 // name the actual cold model owner, never a primary-model alias.
 if(b.visual.extra1C0Present!=(b.visual.bottomRight1C0!=I::none)||
    b.visual.extra1C4Present!=(b.visual.bottomLeft1C4!=I::none)||
    owners.models[0].node!=b.visual.left1BC||owners.models[1].node!=b.visual.right1B8||
    owners.models[2].node!=b.visual.bottomLeft1C4||owners.models[3].node!=b.visual.bottomRight1C0)
  return fail(d,E::Admission,0x223540);
 for(unsigned i=0;i<4;++i){
  const auto& model=owners.models[i];
  if(model.node==I::none){
   if(model.effect800.id||model.effect804.id||model.effect80C.id)return fail(d,E::Admission,0x223540);
  }else for(unsigned j=0;j<i;++j)if(model.node==owners.models[j].node)
   return fail(d,E::Admission,0x223540);
 }
 auto&r=w.next;ImpactEffectsInput50B input;std::memcpy(&input,&owners,sizeof input);
 const float*damage[]={&r.fields.damage7D0,&r.fields.damage7D4,&r.fields.damage7D8,&r.fields.damage7DC};for(unsigned i=0;i<4;++i)std::memcpy(&input.damage7D0[i],damage[i],4);
 std::memcpy(input.timers820,r.impactSupplement.timers820,sizeof input.timers820);
 std::memcpy(input.bodyVelocity70,r.bodies.main.components.root.body.velocity.v,12);std::memcpy(input.bodyVelocity70+3,&r.bodies.mainSphere.velocity70W,4);
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x2288d8);if(!noiseContinuation(s,w.next,d))return false;
 const auto status=updateImpactEffects50B(graph(r),input,r.noise.sharedRandomState,effectsWorkspace,events,capacity,count);
 if(status!=ImpactEffectsStatus50B::applied)return fail(d,status==ImpactEffectsStatus50B::arithmetic?E::Arithmetic:status==ImpactEffectsStatus50B::graph?E::Graph:status==ImpactEffectsStatus50B::binding?E::Admission:E::Storage,0x223540);
 project(r,b);return commit(s,w,d,P::PostImpactEffects50O);
}

bool optionalProbeVehicleImpacts50P(S&s,const VehicleFrameBindings49W&b,const OptionalProbeOwners50P&owners,
 OptionalProbeResult50P&output,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(overlap(&output,sizeof output,&s,sizeof s)||overlap(&output,sizeof output,&w,sizeof w)||overlap(&output,sizeof output,&d,sizeof d)||overlap(&output,sizeof output,&b,sizeof b)||overlap(&output,sizeof output,&owners,sizeof owners)||overlap(&owners,sizeof owners,&s,sizeof s)||overlap(&owners,sizeof owners,&w,sizeof w)||overlap(&owners,sizeof owners,&d,sizeof d))return false;
 if(!start(s,b,P::PostImpactEffects50O,w,d))return false;
 const auto&main=w.next.bodies.main;const auto&probe=main.components.probe348;
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x2288e4);if(!noiseContinuation(s,w.next,d))return false;
 const auto result=inspectOptionalProbeEffects50P(owners,probe.field104,probe.field118,main.components.root.localVelocity80.v[2]);
 if(result.status!=OptionalProbeStatus50P::Complete)return fail(d,result.status==OptionalProbeStatus50P::Arithmetic?E::Arithmetic:result.status==OptionalProbeStatus50P::Binding?E::Admission:E::ExternalCut,result.sourcePC);
 if(!commit(s,w,d,P::PostOptionalProbeEffects50P))return false;output=result;return true;
}
bool decideVehicleImpacts50P(S&s,const VehicleFrameBindings49W&b,std::uint32_t thresholdBits,
 ImpactTransitionResult50C&decision,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(overlap(&decision,sizeof decision,&s,sizeof s)||overlap(&decision,sizeof decision,&w,sizeof w)||overlap(&decision,sizeof decision,&d,sizeof d)||overlap(&decision,sizeof decision,&b,sizeof b))return false;
 if(!start(s,b,P::PostOptionalProbeEffects50P,w,d))return false;
 auto&r=w.next;ImpactTransitionInput50C input{};
 const float*damage[]={&r.fields.damage7D0,&r.fields.damage7D4,&r.fields.damage7D8,&r.fields.damage7DC};for(unsigned i=0;i<4;++i)std::memcpy(input.damage7D0+i,damage[i],4);
 std::memcpy(&input.impactBA8,&r.fields.impactBA0.v[2],4);std::memcpy(&input.threshold50,&thresholdBits,4);
 //225CF8 consumes bit0 only: this is a bit representative, not a raw B9D byte.
 input.flagsB9D=static_cast<std::uint32_t>(r.fields.flagB9D0);
 //2288EC: 225C50 runs only at state0; states1/2 keep their 227370/227A40 result.
 if(r.bodies.main.vehicleState40){decision={ImpactTransitionStatus50C::noTransition,0x2288ecu,0,0};return commit(s,w,d,P::AfterImpactBlock);}
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x228904);if(!noiseContinuation(s,w.next,d))return false;decision=inspectImpactTransition50C(input);
 if(decision.status!=ImpactTransitionStatus50C::noTransition)return fail(d,decision.status==ImpactTransitionStatus50C::arithmetic?E::Arithmetic:E::ExternalCut,decision.sourcePC);
 return commit(s,w,d,P::AfterImpactBlock);
}

bool plainEffectsVehicleTail50Q(S&s,const VehicleFrameBindings49W&b,const PlainEffectOwners50Q&owners,
 PlainEffectResult50Q&output,VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(overlap(&output,sizeof output,&s,sizeof s)||overlap(&output,sizeof output,&w,sizeof w)||overlap(&output,sizeof output,&d,sizeof d)||overlap(&output,sizeof output,&b,sizeof b)||overlap(&output,sizeof output,&owners,sizeof owners)||overlap(&owners,sizeof owners,&s,sizeof s)||overlap(&owners,sizeof owners,&w,sizeof w)||overlap(&owners,sizeof owners,&d,sizeof d))return false;
 if(!start(s,b,P::AfterImpactBlock,w,d))return false;
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x228b40);if(!noiseContinuation(s,w.next,d))return false;
 const auto result=inspectPlainEffectTail50Q(owners);
 if(result.status!=PlainEffectStatus50Q::Complete)return fail(d,result.status==PlainEffectStatus50Q::UnsupportedAttached?E::ExternalCut:E::Admission,result.status==PlainEffectStatus50Q::UnsupportedAttached?0x228b50:0x228390);
 if(!commit(s,w,d,P::PostPlainEffects50Q))return false;output=result;return true;
}

bool packVehicleFrame50S(S&s,const VehicleFrameBindings49W&b,bool configurationEntryNonzero,
 VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(!start(s,b,P::PostPlainEffects50Q,w,d))return false;
 const auto&main=w.next.bodies.main;const auto&c=main.components;const auto&f=w.next.fields;
 PackedVehicleInput50R in{};
 in.drive[0]=c.engine.smoothedDrive[0];in.drive[1]=c.engine.smoothedDrive[1];in.drive[2]=c.bindings.aeroDrive[0];in.drive[3]=c.bindings.aeroDrive[1];
 std::memcpy(in.impactBA0,f.impactBA0.v,12);in.damage7D0[0]=f.damage7D0;in.damage7D0[1]=f.damage7D4;in.damage7D0[2]=f.damage7D8;in.damage7D0[3]=f.damage7DC;
 in.filteredB70=main.controls.bound.control.filtered.values[1];in.filteredB78=main.controls.bound.control.filtered.values[3];in.actionB80=main.controls.player.actionB80;in.flagsB9C=main.controls.player.flagsB9C;in.state40=static_cast<std::uint8_t>(main.vehicleState40);in.vehicle58=f.vehicle58;in.configurationEntryNonzero=configurationEntryNonzero;
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x228b78);if(!noiseContinuation(s,w.next,d))return false;const auto result=packVehicleTail50R(in);if(!result.complete)return fail(d,E::Arithmetic,0x2291d0);
 std::memcpy(w.next.packedC00,result.bytesC00,7);return commit(s,w,d,P::PostPackedBytes50S);
}

BaseTailState50X coldBaseTailState50X(std::uint32_t leftW,std::uint32_t rightW) noexcept {
 BaseTailState50X out{};out.flagsACC=out.flagsAD0=1;out.engineThrustW[0]=leftW;out.engineThrustW[1]=rightW;return out;
}
static bool advanceVehicleBaseThroughEngineImpl50X(S&s,const VehicleFrameBindings49W&b,const FrameTimingState49I&clock,
 const BaseTailBindings50X&binding,BaseTailState50X&tail,BaseTailGlobals50X&globals,
 rr::assets::PodFlameOwners50W&flame,BaseTailWorkspace50X&scratch,VehicleFrameWorkspace49W&w,
 VehicleFrameDiagnostic49W&d,BaseUndertrackContinuation50X* undertrack,bool resuming) noexcept {
 struct Span{const void*p;std::size_t n;};const Span writes[]={{&s,sizeof s},{&tail,sizeof tail},{&globals,sizeof globals},{&flame,sizeof flame},{&scratch,sizeof scratch},{&w,sizeof w},{&d,sizeof d},{undertrack,undertrack?sizeof(*undertrack):0}};
 for(unsigned i=0;i<8;++i){for(unsigned j=0;j<i;++j)if(overlap(writes[i].p,writes[i].n,writes[j].p,writes[j].n))return false;for(const auto&read: {Span{&b,sizeof b},Span{&clock,sizeof clock},Span{&binding,sizeof binding}})if(overlap(writes[i].p,writes[i].n,read.p,read.n))return false;}
 if(!start(s,b,P::PostPackedBytes50S,w,d))return false;
 // 228BBC calls 2255B0 for every racer; 2255B0 itself branches on +58. Player
 // (58=0) and original AI (58=1 with its bound native owners) share this cut.
 const bool undertrackRacer=s.fields.vehicle58==0u||
     (s.fields.vehicle58==1u&&s.fields.nativeAiOwnersBound50IV);
 if(resuming){
  if(!undertrack||!undertrack->pending||undertrack->frame!=&s||undertrack->bindings!=&b||
     !undertrackRacer||std::memcmp(&undertrack->frameFactor,&s.bodies.main.frameFactor,4)||
     !std::isfinite(undertrack->position[1])||!(undertrack->position[1]<0.f))
   return fail(d,E::Admission,0x228bc4);
 }else if(undertrack&&undertrack->pending)return fail(d,E::Admission,0x228bbc);
 if(!binding.vehicleOwnerId||static_cast<unsigned>(binding.probe128)>2)return fail(d,E::Admission,0x2208a0);
 const SceneNodeIndex49P flameRoots[]={b.visual.left1BC,b.visual.right1B8,
  b.visual.bottomLeft1C4,b.visual.bottomRight1C0};
 if(flame.modelCount<2||flame.modelCount>rr::assets::podFlameModelCapacity50W)
  return fail(d,E::Admission,0x2208d0);
 for(unsigned model=0;model<rr::assets::podFlameModelCapacity50W;++model){
  const auto root=flameRoots[model];const auto& prepared=flame.models[model];
  if(model>=flame.modelCount){
   if(root!=SceneNodeIndex49P::none||prepared.modelId)return fail(d,E::Admission,0x2208d0);
   continue;
  }
  if(root==SceneNodeIndex49P::none||prepared.modelId!=static_cast<std::uint32_t>(root)+1u||
     prepared.flameFilter!=(model&1u)+1u||!prepared.referenceCount||
     prepared.referenceCount>rr::assets::podFlameReferencesPerModel50W)
   return fail(d,E::Admission,0x2208d0);
  for(unsigned earlier=0;earlier<model;++earlier)
   if(flame.models[earlier].modelId==prepared.modelId)return fail(d,E::Admission,0x2208d0);
 }
 // Each actual model retains four flame references: 8 callback installations
 // for legacy two-engine pods, 16 for POD02's four independent model instances.
 scratch.next=tail;scratch.globals=globals;scratch.flame=flame;auto&r=w.next;auto&m=r.bodies.main;
 V position{{0,0,0,1}};
 if(resuming){
  if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x228bc4);if(!noiseContinuation(s,w.next,d))return false;std::memcpy(position.v,undertrack->position,16);
 }else{
  if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x228b8c);if(!noiseContinuation(s,w.next,d))return false;r.fields.impactBA0=r.visualShared.zero434630;
  // Original28B180 copies local F0/F4/F8 only; no world refresh occurs here.
  if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x228b98);if(!noiseContinuation(s,w.next,d))return false;std::memcpy(position.v,node(r,b.lifecycle.main190).localC0.v+12,12);
  if(!std::isfinite(position.v[1]))return fail(d,E::Arithmetic,0x228bac);
  if(position.v[1]<0.f){
   if(undertrack){
    if(!undertrackRacer||undertrack->serial==std::numeric_limits<std::uint64_t>::max())
     return fail(d,E::Admission,0x228bbc);
    if(!noiseContinuation(s,w.next,d))return false;
    ++undertrack->serial;undertrack->frame=&s;undertrack->bindings=&b;undertrack->frameFactor=s.bodies.main.frameFactor;
    std::memcpy(undertrack->position,position.v,16);undertrack->pending=true;
    // This represented source store precedes the real2255B0 call. It must
    // remain committed while that call owns a later source continuation.
    s.fields.impactBA0=r.fields.impactBA0;
   }
   return fail(d,E::ExternalCut,0x228bbc);
  }
 }
 std::memcpy(scratch.next.positionAt228B98,position.v,12);scratch.next.positionAt228B98[3]=1.f;
 if(!scratch.globals.initialized434810){scratch.globals.value434810=0xffffffffu;scratch.globals.initialized434810=true;}
 auto&flags=m.controls.player.flagsB9C;
 bool flameBindingPrepared=false;
 if(!(flags&16u)){
  const auto result=bindPreparedFlameModels50V(scratch.flame.models,binding.vehicleOwnerId,rr::assets::flameViews50W(scratch.flame),scratch.flame.records,2+scratch.flame.modelCount,scratch.binding);
  if(result.status!=FlameStatus50V::Complete||result.writes!=4*scratch.flame.modelCount)return fail(d,E::ExternalCut,result.sourcePC?result.sourcePC:0x2208d0);
  flags=static_cast<std::uint8_t>(flags|16u);
  flameBindingPrepared=true;
 }
 for(unsigned model=0;model<scratch.flame.modelCount;++model){
  const auto& record=scratch.flame.records[model+2];
  if(record.words[0]!=((model&1u)?0x20ee90u:0x20f110u)||record.words[1]!=binding.vehicleOwnerId)
   return fail(d,E::Admission,0x220a48);
 }
 EngineTailState50T engine{};auto&t=scratch.next;engine.flagsACC=t.flagsACC;engine.flagsAD0=t.flagsAD0;for(unsigned i=0;i<2;++i){engine.countAD4[i]=t.countAD4[i];engine.timerADC[i]=t.timerADC[i];std::memcpy(engine.thrustA0[i],m.components.engine.localThrust[i].v,12);engine.thrustA0[i][3]=t.engineThrustW[i];}
 engine.timerAE4=t.timerAE4;engine.activeAE8=t.activeAE8;engine.field844=r.fields.vehicle844;engine.sharedRandomState=r.noise.sharedRandomState;
 EngineTailInput50T input{};input.flagsB9C=flags;input.application58=r.fields.applicationByte58;input.state40=m.vehicleState40;std::memcpy(&input.frameFactorBits,&clock.lastDispatched23C,4);input.probe128Present=binding.probe128!=Probe128Binding50X::Null;
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x228c0c);if(!noiseContinuation(s,w.next,d))return false;const auto result=advanceEngineTail50T(engine,input);
 if(result.status!=EngineTailStatus50T::Complete)return fail(d,result.status==EngineTailStatus50T::Arithmetic?E::Arithmetic:E::ExternalCut,result.status==EngineTailStatus50T::NeedsProbeReset?0x1b1560:0x2208a0);
 const auto&next=result.state;t.flagsACC=next.flagsACC;t.flagsAD0=next.flagsAD0;for(unsigned i=0;i<2;++i){t.countAD4[i]=next.countAD4[i];t.timerADC[i]=next.timerADC[i];std::memcpy(m.components.engine.localThrust[i].v,next.thrustA0[i],12);t.engineThrustW[i]=next.thrustA0[i][3];}t.timerAE4=next.timerAE4;t.activeAE8=next.activeAE8;r.fields.vehicle844=next.field844;r.noise.sharedRandomState=next.sharedRandomState;
 // The source binder above writes scratch records transactionally. Do not
 // install its callback in a canonical material clone on any abandoned base
 // frame. All actual clone targets are preflighted before the no-fail commit.
 for(unsigned model=0;model<scratch.flame.modelCount;++model){
  auto* instance=scratch.flame.canonicalInstances50LA[model];
  if(instance!=flame.canonicalInstances50LA[model])return fail(d,E::Admission,0x29B1D0);
  if(!instance)continue;
  const auto& record=scratch.flame.records[model+2];
  if(!instance->prototype()||!instance->prototype()->constructed29B890()||!instance->model()||
     instance->prototype()->instance29B1D0(*instance->model())!=instance||
     instance->nativeModelId()!=scratch.flame.models[model].modelId)
   return fail(d,E::Admission,0x29B1D0);
  if(flameBindingPrepared){
   if(!instance->canBindEngine29BD80(record.words[0],record.words[1]))return fail(d,E::Admission,0x29BDE4);
  }else if(instance->callback00()!=record.words[0]||instance->callbackOwner04()!=record.words[1])
   return fail(d,E::Admission,0x29BDE4);
 }
 if(!commit(s,w,d,P::PostEngineBase50X))return false;tail=scratch.next;globals=scratch.globals;flame=scratch.flame;
 if(flameBindingPrepared)for(unsigned model=0;model<flame.modelCount;++model)
  if(auto* instance=flame.canonicalInstances50LA[model]){
   const auto& record=flame.records[model+2];
   if(!noiseContinuation(s,w.next,d))return false;
   if(!instance->bindEngine29BD80(record.words[0],record.words[1]))std::terminate();
  }
 if(!noiseContinuation(s,w.next,d))return false;
 if(resuming){const auto serial=undertrack->serial;*undertrack={};undertrack->serial=serial;}return true;
}
bool advanceVehicleBaseThroughEngine50X(S&s,const VehicleFrameBindings49W&b,const FrameTimingState49I&clock,
 const BaseTailBindings50X&binding,BaseTailState50X&tail,BaseTailGlobals50X&globals,
 rr::assets::PodFlameOwners50W&flame,BaseTailWorkspace50X&scratch,VehicleFrameWorkspace49W&w,
 VehicleFrameDiagnostic49W&d,BaseUndertrackContinuation50X* undertrack) noexcept {
 return advanceVehicleBaseThroughEngineImpl50X(s,b,clock,binding,tail,globals,flame,scratch,w,d,undertrack,false);
}
bool resumeVehicleBaseAfterUndertrack50X(S&s,const VehicleFrameBindings49W&b,const FrameTimingState49I&clock,
 const BaseTailBindings50X&binding,BaseTailState50X&tail,BaseTailGlobals50X&globals,
 rr::assets::PodFlameOwners50W&flame,BaseTailWorkspace50X&scratch,VehicleFrameWorkspace49W&w,
 VehicleFrameDiagnostic49W&d,BaseUndertrackContinuation50X& undertrack) noexcept {
 return advanceVehicleBaseThroughEngineImpl50X(s,b,clock,binding,tail,globals,flame,scratch,w,d,&undertrack,true);
}

bool finishVehicleBaseAndMainCache50Y(S&s,const VehicleFrameBindings49W&b,const BaseTailState50X&tail,
 bool diagnosticsAbsent,BasePoseHistoryState50Y&history,const PreservingStage49W&cut,
 VehicleFrameWorkspace49W&w,VehicleFrameDiagnostic49W&d) noexcept {
 if(overlap(&history,sizeof history,&s,sizeof s)||overlap(&history,sizeof history,&w,sizeof w)||overlap(&history,sizeof history,&b,sizeof b)||overlap(&history,sizeof history,&d,sizeof d)||overlap(&history,sizeof history,&tail,sizeof tail)||overlap(&history,sizeof history,&cut,sizeof cut)||overlap(&tail,sizeof tail,&s,sizeof s)||overlap(&tail,sizeof tail,&w,sizeof w)||overlap(&tail,sizeof tail,&d,sizeof d))return false;
 if(!start(s,b,P::PostEngineBase50X,w,d)||!preserving(cut,PreservingCut49W::OtherChildrenBeforeMain340,d))return false;
 if(s.impactConsumerPending49Y)return fail(d,E::ExternalCut,0x2288b4);
 auto&r=w.next;auto&m=r.bodies.main;BasePoseHistoryInput50Y input{};input.readyWorld100=node(r,b.lifecycle.main190).world100;input.worldReady=(node(r,b.lifecycle.main190).sceneFlags190&1u)!=0;input.diagnosticsAbsent=diagnosticsAbsent;
 std::memcpy(input.velocity70,m.components.root.body.velocity.v,12);std::memcpy(input.velocity70+3,&r.bodies.mainSphere.velocity70W,4);std::memcpy(input.positionAt228B98,tail.positionAt228B98,16);std::memcpy(&input.outerDeltaBits,&m.frameFactor,4);
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x228c14);if(!noiseContinuation(s,w.next,d))return false;const auto result=advanceBasePoseHistory50Y(history,input);
 if(result.status!=BasePoseHistoryStatus50Y::Complete)return fail(d,result.status==BasePoseHistoryStatus50Y::Arithmetic?E::Arithmetic:E::ExternalCut,result.status==BasePoseHistoryStatus50Y::NeedsDiagnostics?0x228c28:0x228d68);
 if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x228f04);if(!noiseContinuation(s,w.next,d))return false;RR_VEHICLE_FRAME49W_STAGE(0x167bb4);if(!noiseContinuation(s,w.next,d))return false;
 if(!quaternionToAngles48G(m.components.root.body.orientation,m.cachedAnglesE0)||!refreshCachedSpeed48E(m.components.root.body.velocity,m.cachedSpeed34))return fail(d,E::Arithmetic,0x167bb4);
 if(!commit(s,w,d,P::BetweenBodyChildren))return false;history=result.state;return true;
}

}
