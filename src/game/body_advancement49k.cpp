#include "body_advancement49k.h"
#include <cmath>
#include <cfenv>
#include <cstring>

namespace rr::game {
namespace {
bool supported(const MainBodyDomain49K& d) noexcept {
    return d.body320==1 && d.bodyChildCount==0 && d.sceneChildCount==0 &&
        d.sceneObserverCount==0 && d.sceneAttached && !d.sceneHasParent &&
        d.bodyEnabled && !d.secondBodyPresent && !d.override3C && !d.resetRequested &&
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
bool completeMainBodyResponse49K(const MainBodyState49K& in,const MainBodyParameters49K& p,
    const PostResponse354_49K& supplied,MainBodyState49K& out) noexcept {
    if(!at(in,MainBodyPhase49K::AwaitingResponse354))return false;
    auto r=in;r.components=supplied.components;auto& c=r.components;
    DisabledProbeResponse49K response{};
    if(!respondDisabledProbe49K(c.probe348,in.cachedAnglesE0.v[2],p.probe204,p.effective4342C8,
        p.effective434290,c.root.loads.torque,response))return false;
    c.probe348=response.state;c.root.loads.torque=response.torque190;
    for(unsigned i=0;i<3;++i)if(!std::isfinite(p.effectiveImpactZero434630.v[i]) ||
        c.root.accumulator1c0.v[i]!=p.effectiveImpactZero434630.v[i])return false;
    if(!updateRootBody48H(c.root,p.body,p.centerOffset,true,true,in.schedule.factorPerSubstep,c.root))return false;
    clearMainBodyPost20C49K(c.body20C);
    r.response354_3C=supplied.response3C;r.response354_44=supplied.response44;
    r.phase=MainBodyPhase49K::AwaitingQuery219120;out=r;return true;
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
}
