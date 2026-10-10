// NEW49M-only common phases copied verbatim from49K except two-body domain.
// Evidence checks each copied body; original49K files and public rejection stay unchanged.
#include "coupled_body_advancement49m.h"
#include <cmath>
#include <cfenv>
#include <cstring>
#include <cstddef>

namespace rr::game {
namespace {
namespace mainPhases49M {
namespace {
bool supported(const MainBodyDomain49K& d) noexcept {
    return d.body320==1 && d.bodyChildCount==0 && d.sceneChildCount==0 &&
        d.sceneObserverCount==0 && d.sceneAttached && !d.sceneHasParent &&
        d.bodyEnabled && d.secondBodyPresent && !d.override3C && !d.resetRequested &&
        d.probe100==0 && !d.probe120Present && !d.probe124Present;
}
bool at(const MainBodyState49K& s, MainBodyPhase49K p) noexcept {
    if(std::fegetround()!=FE_TONEAREST || !supported(s.domain) || s.phase!=p ||
       !std::isfinite(s.frameFactor) || s.frameFactor<0 || s.frameFactor>0.06f)return false;
    if(p==MainBodyPhase49K::FrameReady || p==MainBodyPhase49K::AwaitingPostPlayer ||
       p==MainBodyPhase49K::AwaitingPostBase)return true;
    if(s.applicationActive11C!=0 || s.vehicleState40==3 || s.vehicleState40==4)return false;
    Schedule49K expected{};
    return selectSchedule49K(s.frameFactor,s.liveSubstepScale,expected) &&
        s.schedule.count==expected.count && s.substepIndex<s.schedule.count &&
        std::memcmp(&s.schedule.factorPerSubstep,&expected.factorPerSubstep,sizeof(float))==0;
}
bool sameComponents(const MainBodyComponents49K& a,const MainBodyComponents49K& b) noexcept {
    return std::memcmp(&a.root,&b.root,sizeof(a.root))==0 &&
        std::memcmp(&a.engine,&b.engine,sizeof(a.engine))==0 &&
        std::memcmp(&a.aero.timer140,&b.aero.timer140,2*sizeof(float))==0 &&
        a.aero.lowSpeedDc==b.aero.lowSpeedDc &&
        std::memcmp(&a.bindings,&b.bindings,sizeof(a.bindings))==0 &&
        std::memcmp(&a.probe348,&b.probe348,sizeof(a.probe348))==0 &&
        std::memcmp(&a.feedback,&b.feedback,sizeof(a.feedback))==0 && a.body20C==b.body20C;
}
ReadyBodyBasis48D basis(const BodyMatrix48G& m) noexcept {
    ReadyBodyBasis48D b{};
    for(unsigned c=0;c<3;++c)for(unsigned r=0;r<3;++r)b.columns[c].v[r]=m.v[4*c+r];
    return b;
}
}
bool beginMainBodyFrame49K(const MainBodyState49K& in, const FrameReader49K& f,
    MainBodyState49K& out) noexcept {
    if(!at(in,MainBodyPhase49K::FrameReady) || !std::isfinite(f.preparedFactor) ||
       f.preparedFactor<0 || f.preparedFactor>0.06f)return false;
    auto r=in; auto context=f.context;
    if(!conditionDualShockPlayerAxes48X(f.packet,r.controls.components))return false;
    context.componentValues=r.controls.components;
    if(!readPlayerInput48X(context,f.codes,f.actions,r.controls.player))return false;
    r.frameFactor=f.preparedFactor;r.substepIndex=0;
    r.phase=MainBodyPhase49K::AwaitingPostPlayer;out=r;return true;
}
bool bindMainBodyFrame49K(const MainBodyState49K& in,const PostPlayerAt217DE0_49K& p,
    ControlRateLimits48B rates,const ControlBindingParameters48W& parameters,
    MainBodyState49K& out) noexcept {
    if(!at(in,MainBodyPhase49K::AwaitingPostPlayer) || p.bindingContext.vehicleState40==3 ||
       p.bindingContext.vehicleState40==4)return false;
    auto r=in;auto& control=r.controls.bound.control;
    if(!shapePlayerControlChannels48X(p.player.raw,in.controls.bound.control.shaping.smoothedChannel0D28,
        p.bindingContext.mode58Nonzero?ControlShapeMode48V::BypassNonlinear:ControlShapeMode48V::Nonlinear,
        control.shaping) || !stepControlRateFilter48B(control.shaping.targets,
        in.controls.bound.control.filtered,rates,in.frameFactor,control.filtered))return false;
    auto b=p.bindingContext;b.filtered=control.filtered;b.bodyCachedSpeed34=in.cachedSpeed34;
    b.actionB7C=p.player.actionB7C;b.actionB80=p.player.actionB80;b.flagsB9C=p.player.flagsB9C;
    if(!bindVehicleControls48W(b,parameters,in.components.bindings,r.components.bindings))return false;
    r.controls.bound.bindings=r.components.bindings;
    r.vehicleState40=p.bindingContext.vehicleState40;
    r.phase=MainBodyPhase49K::AwaitingPostBase;out=r;return true;
}
bool refreshMainBodyFrame49K(const MainBodyState49K& in,const PostBaseAtChildPhase49K& p,
    MainBodyState49K& out) noexcept {
    if(!at(in,MainBodyPhase49K::AwaitingPostBase) || p.applicationActive11C!=0 ||
       p.liveVehicleState40==3 || p.liveVehicleState40==4)return false;
    auto r=in;r.components=p.components;
    if(!quaternionToAngles48G(r.components.root.body.orientation,r.cachedAnglesE0) ||
       !refreshCachedSpeed48E(r.components.root.body.velocity,r.cachedSpeed34) ||
       !selectSchedule49K(in.frameFactor,p.liveSubstepScale,r.schedule))return false;
    r.liveSubstepScale=p.liveSubstepScale;
    r.vehicleState40=p.liveVehicleState40;r.applicationActive11C=p.applicationActive11C;
    r.phase=MainBodyPhase49K::SubstepReady;out=r;return true;
}
bool beginMainBodySubstep49K(const MainBodyState49K& in,const SubstepSelectionContext49K& p,
    MainBodyState49K& out) noexcept {
    if(!at(in,MainBodyPhase49K::SubstepReady) || in.substepIndex>=in.schedule.count ||
       in.schedule.count==0 || in.schedule.count>4)return false;
    auto r=in;
    const GravitySelection49K g{in.components.probe348.field104,in.components.bindings.aeroBrakeEC,
        in.cachedSpeed34,in.components.root.scene.v[5],p.vehicle378,p.vehicle37C,
        p.effective434730,p.effective434738};
    if(!selectGravity49K(g,r.selectedGravity))return false;
    r.selectedEngineFlags=selectEngineFlags49K(p.vehicle64,p.vehicleB9C);
    r.phase=MainBodyPhase49K::AwaitingWorldPreparation;out=r;return true;
}
bool produceMainBodyForces49K(const MainBodyState49K& in,const MainBodyParameters49K& p,
    const PostWorldPreparation49K& supplied,MainBodyState49K& out) noexcept {
    if(!at(in,MainBodyPhase49K::AwaitingWorldPreparation) ||
       supplied.point!=(in.substepIndex==0?WorldPreparationPoint49K::FirstSubstep358And219420:
                                          (in.vehicle380!=0?WorldPreparationPoint49K::LaterRequired358And219420:
                                                             WorldPreparationPoint49K::LaterSkipped358And219420)))return false;
    if(supplied.point==WorldPreparationPoint49K::LaterSkipped358And219420 &&
       !sameComponents(supplied.components,in.components))return false;
    auto r=in;r.components=supplied.components;auto& c=r.components;
    if(!prepareDisabledProbe49K(c.probe348,p.probeLocal140,c.root.scene,p.effective434260,c.probe348))return false;
    EngineInput48D engineInput{};AeroInput48E aeroInput{};
    engineInput.bodySpeed=in.cachedSpeed34;
    engineInput.boost=in.selectedEngineFlags.engine34!=0;
    engineInput.repair=in.selectedEngineFlags.engine38!=0;
    engineInput.fullTorque=c.feedback.engine3C!=0;
    aeroInput.bodyLocalVelocity=c.root.localVelocity80;
    aeroInput.angularVelocity=c.root.body.angularVelocity;
    aeroInput.bodyAngleE0=in.cachedAnglesE0.v[0];
    aeroInput.subtractWorldFlow=supplied.aeroSubtractWorldFlow;
    aeroInput.worldFlow40=supplied.aeroWorldFlow40;
    aeroInput.triggerD0=c.feedback.aeroD0!=0;
    aeroInput.fullSteeringE0=c.feedback.aeroE0!=0;
    if(!applyControlModelBindings48W(c.bindings,engineInput,aeroInput,engineInput,aeroInput))return false;
    const auto b=basis(c.root.scene);
    if(!updateEngine48D(c.engine,p.engine,engineInput,c.engine) ||
       !accumulateEngineGravity48D(c.root.loads,c.engine,b,{in.selectedGravity,p.gravityForceX,p.gravityForceZ},
           p.body.mass,(supplied.liveApplicationGateByte58&2u)!=0,true,c.root.loads))return false;
    AeroResult48E aero{};
    if(!applyAerodynamics48E(c.aero,p.aero,aeroInput,b,c.root.loads,in.schedule.factorPerSubstep,aero))return false;
    c.aero=aero.state;c.root.loads=aero.accumulatedLoads;
    r.phase=MainBodyPhase49K::AwaitingResponse354;out=r;return true;
}
bool completeMainBodyQuery49K(const MainBodyState49K& in,const SuppliedQuery219120_49K& p,
    MainBodyState49K& out) noexcept {
    if(!at(in,MainBodyPhase49K::AwaitingQuery219120) || (p.resultFlagsF8&0x20u)!=0)return false;
    for(float x:p.currentScenePoint7C0)if(!std::isfinite(x))return false;
    auto r=in;
    for(unsigned i=0;i<4;++i)r.priorPoint7B0[i]=r.currentPoint7C0[i]=p.currentScenePoint7C0[i];
    r.vehicle380=selectVehicle38049K(r.response354_44);
    r.components.feedback=selectLateFeedback49K(r.components.probe348.field104,r.response354_3C,r.response354_44);
    r.phase=MainBodyPhase49K::AwaitingSceneConsumer;out=r;return true;
}
bool completeMainBodySceneConsumer49K(const MainBodyState49K& in,
    const SceneConsumerObservation49K& p,MainBodyState49K& out) noexcept {
    if(!at(in,MainBodyPhase49K::AwaitingSceneConsumer) ||
       std::memcmp(&p.consumedScene,&in.components.root.scene,sizeof(p.consumedScene))!=0)return false;
    auto r=in;++r.substepIndex;
    r.phase=r.substepIndex==r.schedule.count?MainBodyPhase49K::FrameReady:MainBodyPhase49K::SubstepReady;
    out=r;return true;
}
} // namespace mainPhases49M
} // private49M phase copies
namespace {
MainBodyPhase49K mainPhase(CoupledBodyPhase49M p) noexcept {
    switch(p) {
    case CoupledBodyPhase49M::FrameReady:return MainBodyPhase49K::FrameReady;
    case CoupledBodyPhase49M::AwaitingPostPlayer:return MainBodyPhase49K::AwaitingPostPlayer;
    case CoupledBodyPhase49M::AwaitingPostBase:return MainBodyPhase49K::AwaitingPostBase;
    case CoupledBodyPhase49M::SubstepReady:return MainBodyPhase49K::SubstepReady;
    case CoupledBodyPhase49M::AwaitingWorldPreparation:return MainBodyPhase49K::AwaitingWorldPreparation;
    case CoupledBodyPhase49M::AwaitingQuery219120:return MainBodyPhase49K::AwaitingQuery219120;
    case CoupledBodyPhase49M::AwaitingSceneConsumer:return MainBodyPhase49K::AwaitingSceneConsumer;
    default:return MainBodyPhase49K::AwaitingResponse354;
    }
}
bool at(const CoupledBodyState49M& s,CoupledBodyPhase49M p) noexcept {
    const auto& d=s.secondaryDomain;
    return s.phase==p && mainPhases49M::at(s.main,mainPhase(p)) &&
        d.body320==1 && d.registrationMask==2 && d.bodyChildCount==0 &&
        d.sceneChildCount==0 && d.sceneObserverCount==0 && !d.sceneHasParent &&
        s.secondary.scenePresent && s.secondary.enabled &&
        s.secondary.phase==(p==CoupledBodyPhase49M::AwaitingResponse368?
            SecondaryPhase49M::Response368Pending:SecondaryPhase49M::Ready);
}
bool sameSecondary(const SecondaryBodyState49M& a,const SecondaryBodyState49M& b) noexcept {
    return std::memcmp(&a.root,&b.root,sizeof a.root)==0 &&
        std::memcmp(&a.cachedSpeed34,&b.cachedSpeed34,4*sizeof(float))==0 &&
        a.sceneFlags190==b.sceneFlags190 && a.body20C==b.body20C &&
        a.scenePresent==b.scenePresent && a.enabled==b.enabled &&
        std::memcmp(&a.velocity70W,&b.velocity70W,5*sizeof(float))==0 && a.phase==b.phase;
}
bool sameJoint(const PodSphereJoint49L& a,const PodSphereJoint49L& b) noexcept {
    return std::memcmp(&a.stiffness30,&b.stiffness30,2*sizeof(float))==0 &&
        a.enabled38==b.enabled38 && a.unilateral3C==b.unilateral3C && a.suppressed40==b.suppressed40 &&
        a.bodyAPresent44==b.bodyAPresent44 && a.bodyBPresent48==b.bodyBPresent48 &&
        std::memcmp(&a.length4C,&b.length4C,sizeof(PodSphereJoint49L)-offsetof(PodSphereJoint49L,length4C))==0;
}
bool accept(CoupledBodyState49M& r,const CoupledComponents49M& c) noexcept {
    if(c.secondary.phase!=r.secondary.phase || !c.secondary.scenePresent || !c.secondary.enabled)return false;
    r.main.components=c.main;r.secondary=c.secondary;r.mainSphere=c.mainSphere;r.joint=c.joint;return true;
}
void mainSphere(const CoupledBodyState49M& s,const CoupledBodyParameters49M& p,
                PodSphereBody49L& out) noexcept {
    SecondaryBodyState49M view{};view.root=s.main.components.root;
    view.scenePresent=s.main.domain.sceneAttached;view.sceneFlags190=s.mainSphere.sceneFlags190;
    view.velocity70W=s.mainSphere.velocity70W;view.angularVelocityB0W=s.mainSphere.angularVelocityB0W;
    view.force180W=s.mainSphere.force180W;view.torque190W=s.mainSphere.torque190W;
    SecondaryBodyParameters49M parameters{};parameters.body=p.main.body;
    parameters.centerOffset=p.main.centerOffset;parameters.centerOffsetW=p.mainSphere.centerOffsetW;
    for(unsigned i=0;i<7;++i)parameters.inverseInertiaExtra[i]=p.mainSphere.inverseInertiaExtra[i];
    makeSecondarySphereBody49M(view,parameters,out);
}
}
CoupledComponents49M coupledComponents49M(const CoupledBodyState49M& s) noexcept {
    return {s.main.components,s.secondary,s.mainSphere,s.joint};
}
bool beginCoupledBodyFrame49M(const CoupledBodyState49M& in,const FrameReader49K& f,
    CoupledBodyState49M& out) noexcept {
    if(!at(in,CoupledBodyPhase49M::FrameReady))return false;
    auto r=in;if(!mainPhases49M::beginMainBodyFrame49K(in.main,f,r.main))return false;
    r.phase=CoupledBodyPhase49M::AwaitingPostPlayer;out=r;return true;
}
bool bindCoupledBodyFrame49M(const CoupledBodyState49M& in,const PostPlayerAt217DE0_49K& p,
    ControlRateLimits48B rates,const ControlBindingParameters48W& parameters,
    CoupledBodyState49M& out) noexcept {
    if(!at(in,CoupledBodyPhase49M::AwaitingPostPlayer))return false;
    auto r=in;if(!mainPhases49M::bindMainBodyFrame49K(in.main,p,rates,parameters,r.main))return false;
    r.phase=CoupledBodyPhase49M::AwaitingPostBase;out=r;return true;
}
bool refreshCoupledBodyFrame49M(const CoupledBodyState49M& in,const PostBase49M& p,
    CoupledBodyState49M& out) noexcept {
    if(!at(in,CoupledBodyPhase49M::AwaitingPostBase))return false;
    auto r=in;if(!accept(r,p.components))return false;
    if(!mainPhases49M::refreshMainBodyFrame49K(in.main,
        {p.components.main,p.liveSubstepScale,p.liveVehicleState40,p.applicationActive11C},r.main) ||
       !refreshSecondaryFrameCache49M(r.secondary,r.secondary))return false;
    r.vehicle838=p.liveVehicle838;r.phase=CoupledBodyPhase49M::SubstepReady;out=r;return true;
}
bool beginCoupledBodySubstep49M(const CoupledBodyState49M& in,const SubstepSelectionContext49K& p,
    CoupledBodyState49M& out) noexcept {
    if(!at(in,CoupledBodyPhase49M::SubstepReady))return false;
    auto r=in;if(!mainPhases49M::beginMainBodySubstep49K(in.main,p,r.main))return false;
    r.phase=CoupledBodyPhase49M::AwaitingWorldPreparation;out=r;return true;
}
bool produceCoupledBodyForces49M(const CoupledBodyState49M& in,const CoupledBodyParameters49M& p,
    const PostWorld49M& supplied,CoupledBodyState49M& out) noexcept {
    if(!at(in,CoupledBodyPhase49M::AwaitingWorldPreparation))return false;
    if(supplied.point==WorldPreparationPoint49K::LaterSkipped358And219420 &&
       (!sameSecondary(supplied.components.secondary,in.secondary) ||
        !sameJoint(supplied.components.joint,in.joint) ||
        std::memcmp(&supplied.components.mainSphere,&in.mainSphere,sizeof in.mainSphere)!=0))return false;
    auto r=in;if(!accept(r,supplied.components))return false;
    const PostWorldPreparation49K world{supplied.point,supplied.components.main,supplied.aeroSubtractWorldFlow,
        supplied.aeroWorldFlow40,supplied.liveApplicationGateByte58};
    if(!mainPhases49M::produceMainBodyForces49K(in.main,p.main,world,r.main))return false;
    r.phase=CoupledBodyPhase49M::AwaitingResponse354;out=r;return true;
}
bool completeCoupledResponse354_49M(const CoupledBodyState49M& in,const CoupledBodyParameters49M& p,
    const PostResponse354_49M& supplied,CoupledBodyState49M& out) noexcept {
    if(!at(in,CoupledBodyPhase49M::AwaitingResponse354))return false;
    auto r=in;if(!accept(r,supplied.components))return false;
    auto& c=r.main.components;DisabledProbeResponse49K response{};
    if(!respondDisabledProbe49K(c.probe348,in.main.cachedAnglesE0.v[2],p.main.probe204,
        p.main.effective4342C8,p.main.effective434290,c.root.loads.torque,response))return false;
    c.probe348=response.state;c.root.loads.torque=response.torque190;
    r.main.response354_3C=supplied.response3C;r.main.response354_44=supplied.response44;
    r.vehicle838=supplied.liveVehicle838;
    r.phase=CoupledBodyPhase49M::AwaitingSecondary;out=r;return true;
}
bool advanceCoupledSecondary49M(const CoupledBodyState49M& in,const CoupledBodyParameters49M& p,
    const SecondaryInputs49M& supplied,CoupledNoiseContext49M& shared,CoupledBodyState49M& out) noexcept {
    if(!at(in,CoupledBodyPhase49M::AwaitingSecondary))return false;
    auto r=in;
    if(r.vehicle838==0){r.phase=CoupledBodyPhase49M::AwaitingMainIntegration;out=r;return true;}
    auto nextShared=shared;
    SecondaryForceContext49M context{};
    context.vehicleState40=r.main.vehicleState40;context.probe104=r.main.components.probe348.field104;
    context.gravity378=supplied.gravity378;context.gravity37C=supplied.gravity37C;
    context.targetRelative2F0=supplied.targetRelative2F0;
    for(unsigned i=0;i<16;++i)context.mainSceneWorld100.v[i]=r.main.components.root.scene.v[i];
    context.mainSceneFlags190=r.mainSphere.sceneFlags190;context.mainCachedSpeed34=r.main.cachedSpeed34;
    context.effective4346C8=supplied.effective4346C8;context.effective4346D0=supplied.effective4346D0;
    context.effective4346D8=supplied.effective4346D8;context.effective4346E0=supplied.effective4346E0;
    context.effective4346E8=supplied.effective4346E8;context.effective4346F0=supplied.effective4346F0;
    context.effective4346F8=supplied.effective4346F8;context.effective434700=supplied.effective434700;
    context.effective434708=supplied.effective434708;
    if(!produceSecondaryForces49M(r.secondary,p.secondary,context,r.main.schedule.factorPerSubstep,
        nextShared.noise,nextShared.sharedRandomState,r.secondary))return false;
    PodSphereBody49L main{};mainSphere(r,p,main);
    if(!applySecondaryConstraint49M(r.secondary,p.secondary,main,r.joint,r.secondary,r.joint,
        r.lastConstraintMagnitude))return false;
    r.phase=CoupledBodyPhase49M::AwaitingResponse368;shared=nextShared;out=r;return true;
}
bool completeCoupledResponse368_49M(const CoupledBodyState49M& in,const CoupledBodyParameters49M& p,
    const CoupledComponents49M& supplied,CoupledBodyState49M& out) noexcept {
    if(!at(in,CoupledBodyPhase49M::AwaitingResponse368))return false;
    auto r=in;if(!accept(r,supplied))return false;
    if(!completeSecondaryResponse49M(r.secondary,p.secondary,r.main.schedule.factorPerSubstep,r.secondary))return false;
    r.phase=CoupledBodyPhase49M::AwaitingMainIntegration;out=r;return true;
}
bool integrateCoupledMain49M(const CoupledBodyState49M& in,const CoupledBodyParameters49M& p,
    CoupledBodyState49M& out) noexcept {
    if(!at(in,CoupledBodyPhase49M::AwaitingMainIntegration))return false;
    auto r=in;auto& c=r.main.components;
    for(unsigned i=0;i<3;++i)if(!std::isfinite(p.main.effectiveImpactZero434630.v[i]) ||
        c.root.accumulator1c0.v[i]!=p.main.effectiveImpactZero434630.v[i])return false;
    if(!updateRootBody48H(c.root,p.main.body,p.main.centerOffset,true,true,
        r.main.schedule.factorPerSubstep,c.root))return false;
    // Original48H full-lane clears1682FC..168340 copy433B80=(0,0,0,1).
    r.mainSphere.force180W=1;r.mainSphere.torque190W=1;
    // Root scene dirty helper marksbit7; the no-parent ready reflection retainsbit0.
    r.mainSphere.sceneFlags190|=0x80u;
    clearMainBodyPost20C49K(c.body20C);
    r.main.phase=MainBodyPhase49K::AwaitingQuery219120;
    r.phase=CoupledBodyPhase49M::AwaitingQuery219120;out=r;return true;
}
bool completeCoupledQuery49M(const CoupledBodyState49M& in,const SuppliedQuery219120_49K& p,
    CoupledBodyState49M& out) noexcept {
    if(!at(in,CoupledBodyPhase49M::AwaitingQuery219120))return false;
    auto r=in;if(!mainPhases49M::completeMainBodyQuery49K(in.main,p,r.main))return false;
    r.phase=CoupledBodyPhase49M::AwaitingSceneConsumer;out=r;return true;
}
bool completeCoupledSceneConsumer49M(const CoupledBodyState49M& in,const SceneConsumerObservation49K& p,
    CoupledBodyState49M& out) noexcept {
    if(!at(in,CoupledBodyPhase49M::AwaitingSceneConsumer))return false;
    auto r=in;if(!mainPhases49M::completeMainBodySceneConsumer49K(in.main,p,r.main))return false;
    r.phase=r.main.phase==MainBodyPhase49K::FrameReady?CoupledBodyPhase49M::FrameReady:CoupledBodyPhase49M::SubstepReady;
    out=r;return true;
}
}
