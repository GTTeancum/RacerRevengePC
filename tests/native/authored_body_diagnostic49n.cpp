#include <string>
#include "../../evidence/authoredInitialization49N/connected/parameter_oracle_bridge49n.h"
#include <array>
#include "../../src/game/authored_parameters49n.h"
#include "../../src/assets/pod_physics49n.h"
#include "../../src/game/control_presets49d.h"
#include "../../src/game/coupled_body_advancement49m.h"
#include "../../evidence/secondaryWrapper49M/composition/secondary_oracle_helpers49m.h"
#include "../../src/game/clock_quantization49j.h"
#include "../../evidence/bodyAdvancement49K/composition/oracle_helpers49k.h"
namespace sceneOracle49K {
#include "../../tools/gameplay/body_scene_oracle48g.h"
}
#include "../../tools/gameplay/player_input_oracle48x.h"
#include "../../tools/gameplay/control_binding_oracle48w.h"
#include "../../evidence/bodyAdvancement49K/composition/selector_oracle_helpers49k.h"
namespace selectorOriginal=compositionSelectorOracle49K;
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <new>
#include <type_traits>

static std::uint64_t allocations=0,checks=0,originalInstructions=0,hotTransitions=0;
void* operator new(std::size_t n){++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept {std::free(p);}
void operator delete[](void* p) noexcept {std::free(p);}
void operator delete(void* p,std::size_t) noexcept {std::free(p);}
void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
static void require(bool b,const char* s){++checks;if(!b)throw std::runtime_error(s);}
static void equalWords(const void* a,const void* b,unsigned n,const char* s){
    const auto* x=static_cast<const unsigned char*>(a);const auto* y=static_cast<const unsigned char*>(b);
    for(unsigned i=0;i<n;++i){std::uint32_t p,q;std::memcpy(&p,x+4*i,4);std::memcpy(&q,y+4*i,4);++checks;
        if(p!=q)throw std::runtime_error(std::string(s)+" lane="+std::to_string(i)+" actual="+std::to_string(p)+" expected="+std::to_string(q));}
}
template<class F> static void hot(F f,const char* s){const auto old=allocations;const bool ok=f();require(ok,s);require(allocations==old,"native hot allocation");++hotTransitions;}
struct Provider {
    unsigned salt=0,count=0;
    bool answer(std::int32_t code,std::uint32_t mask) noexcept {++count;return mask==0x3f&&code>=0&&((unsigned(code)+salt)%13==0);}
    static PlayerActionResult48X query(void* p,std::int32_t c,std::uint32_t m) noexcept {return static_cast<Provider*>(p)->answer(c,m)?PlayerActionResult48X::Match:PlayerActionResult48X::NoMatch;}
};
static ReadyBodyBasis48D rootBasis(const RootBodyState48H& r){ReadyBodyBasis48D b{};for(unsigned c=0;c<3;++c)for(unsigned i=0;i<3;++i)b.columns[c].v[i]=r.scene.v[4*c+i];return b;}
static void originalRead(MainBodyState49K& s,const FrameReader49K& f){
    original48x::ReaderInput in{};in.codes={f.codes.action694,f.codes.action690,f.codes.flag698,f.codes.flag69C,f.codes.flag6B4};
    in.gateByte=f.context.gateByte58;in.previousFlags=f.context.previousFlagsB9C;
    in.source7D0=f.context.source7D0;in.source7D4=f.context.source7D4;in.source7D8=f.context.source7D8;in.source7DC=f.context.source7DC;
    const float nums[]={float(f.packet.packet6),float(f.packet.packet7),float(255-f.packet.packet4),float(f.packet.packet5)};
    for(unsigned i=0;i<4;++i){original48x::Stats st{};in.components[i]=original48x::axis(nums[i],255,{-1,1,0,fp(0x3e4ccccd),fp(0xbe4ccccd)},&st);originalInstructions+=st.steps;s.controls.components.values[i]=in.components[i];}
    Provider p=*static_cast<Provider*>(f.actions.context);p.count=0;original48x::Stats st{};
    const auto got=original48x::reader(in,[&](std::int32_t c,std::uint32_t m){return p.answer(c,m);},&st);originalInstructions+=st.steps;
    for(unsigned i=0;i<4;++i)s.controls.player.raw.values[i]=got.raw[i];
    s.controls.player.actionB7C=got.actionB7C;s.controls.player.actionB80=got.actionB80;s.controls.player.flagsB9C=got.flags;
    s.frameFactor=f.preparedFactor;s.substepIndex=0;
}
static void originalBind(MainBodyState49K& s,const PostPlayerAt217DE0_49K& p,ControlRateLimits48B rates,const ControlBindingParameters48W& parameters){
    auto context=p.bindingContext;context.bodyCachedSpeed34=s.cachedSpeed34;context.actionB7C=p.player.actionB7C;context.actionB80=p.player.actionB80;context.flagsB9C=p.player.flagsB9C;
    original48w::Machine m;m.prepareBinding(context,parameters,s.components.bindings,0);
    s.controls.bound.control=m.run(p.player.raw,s.controls.bound.control.shaping.smoothedChannel0D28,context.mode58Nonzero?1u:0u,s.controls.bound.control.filtered,rates,s.frameFactor,3);
    s.components.bindings=m.continueBinding();s.controls.bound.bindings=s.components.bindings;originalInstructions+=m.steps;
}
static void originalRefresh(MainBodyState49K& s){
    sceneOracle49K::OriginalScene48G angles;angles.store(0x400,s.components.root.body.orientation);
    angles.g[4][0]=0x600;angles.g[5][0]=0x400;angles.g[29][0]=0x1800;angles.g[31][0]=0xf00df00d;angles.v[0][3]=1;
    auto anglePc=0x14fc00u;while(anglePc!=0xf00df00d){std::uint32_t target=0,unused=0;if(angles.execute(anglePc,target)){if(angles.execute(anglePc+4,unused))throw std::runtime_error("angle delay");anglePc=target;}else anglePc+=4;}
    s.cachedAnglesE0=angles.vec(0x600);originalInstructions+=angles.steps;
    OriginalWrapper48H m;m.store(0x70,s.components.root.body.velocity);m.g[16][0]=0;m.g[29][0]=0x1800;m.f[3]=0;m.v[0][3]=1;
    auto pc=rr_cached_speed_48e_start;
    while(pc!=0x167c1c){std::uint32_t target=0,unused=0;if(m.execute(pc,target)){if(!m.annulDelay&&m.execute(pc+4,unused))throw std::runtime_error("speed delay branch");pc=target;}else pc+=4;}
    s.cachedSpeed34=m.readFloat(0x34);originalInstructions+=m.steps;
}
static void originalForces(MainBodyState49K& s,const MainBodyParameters49K& p,const PostWorldPreparation49K& w,MainSphereLanes49M& lanes){
    auto& c=s.components;EngineInput48D e{{c.bindings.engineDrive[0],c.bindings.engineDrive[1]},s.cachedSpeed34,s.selectedEngineFlags.engine34!=0,s.selectedEngineFlags.engine38!=0,c.feedback.engine3C!=0};
    AeroInput48E a{c.root.localVelocity80,c.root.body.angularVelocity,s.cachedAnglesE0.v[0],w.aeroSubtractWorldFlow,w.aeroWorldFlow40,
        c.feedback.aeroD0!=0,c.bindings.aeroTriggerD8!=0,c.feedback.aeroE0!=0,c.bindings.aeroDrive[0],c.bindings.aeroDrive[1],c.bindings.aeroBrakeEC};
    OriginalWrapper48H m;m.set(c.root.body,p.body,c.root.loads);putEngine(m,c.engine,p.engine,e);m.store(0x18c,lanes.force180W);m.store(0x19c,lanes.torque190W);
    const auto b=rootBasis(c.root);putForceContext(m,b,{s.selectedGravity,p.gravityForceX,p.gravityForceZ},true);
    callOriginal(m,rr_engine_update_48d_start,engineAddress,s.schedule.factorPerSubstep);c.engine=getEngine(m);originalInstructions+=m.steps;
    callOriginal(m,rr_gravity_48d_start,gravityAddress,s.schedule.factorPerSubstep);originalInstructions+=m.steps;
    if(w.liveApplicationGateByte58&2){callOriginal(m,rr_engine_apply_48d_start,engineAddress,s.schedule.factorPerSubstep);originalInstructions+=m.steps;}
    putAero(m,c.aero,p.aero,a,b,{m.vec(0x180),m.vec(0x190)});
    m.store(aeroBody+0x18c,m.readFloat(0x18c));m.store(aeroBody+0x19c,m.readFloat(0x19c));
    callOriginal(m,rr_aero_48e_start,aeroAddress,s.schedule.factorPerSubstep);const auto result=getAero(m);originalInstructions+=m.steps;
    c.aero=result.state;c.root.loads=result.accumulatedLoads;lanes.force180W=m.readFloat(aeroBody+0x18c);lanes.torque190W=m.readFloat(aeroBody+0x19c);
}

static MainBodyParameters49K parameters(){
    MainBodyParameters49K p{};p.body=oracleDefaultBodyParameters();p.body.mass=2;p.body.inverseMass=.5f;
    p.body.inverseInertiaColumns[0].v[0]=p.body.inverseInertiaColumns[1].v[1]=p.body.inverseInertiaColumns[2].v[2]=.05f;
    p.engine={80,20,10,.4f,.2f,45,65,15,5,{{{.03f,.04f,.07f}},{{-.02f,.03f,-.04f}}}};
    p.aero={.1f,.002f,.002f,.01f,.01f,.005f,.01f,.01f,.01f,.003f,.003f,.01f,.2f,.01f,.01f,.01f,.001f,.002f,.01f,.01f,{{.001f,.002f,.001f}}};
    p.centerOffset={{0,-.25f,0}};p.probeLocal140[1]=-.5f;p.probeLocal140[3]=1;p.effective434260[3]=1;
    p.probe204=.2f;p.effective4342C8=fp(0x3e99999a);p.effective434290={{0,0,1}};return p;
}
static MainBodyState49K initial(){
    MainBodyState49K s{};s.domain={1,0,0,0,true,false,true,false,false,false,0,false,false};
    s.components.root.body.orientation={{.98f,.08f,.04f,.15f}};
    require(quaternionToMatrix48G(s.components.root.body.orientation,s.components.root.scene),"initial scene");s.components.root.scene.v[13]=10;
    s.components.root.body.velocity={{1,0,30}};s.components.root.localVelocity80={{1,0,30}};
    s.components.root.body.previousPositionDerivative=s.components.root.body.velocity;
    s.cachedSpeed34=75;s.cachedAnglesE0={{.01f,.02f,.03f}};s.components.feedback={1,1,1,7};
    s.components.probe348.field104=1;s.components.probe348.field11C=9;s.components.body20C=0x12345678;
    s.phase=MainBodyPhase49K::FrameReady;return s;
}
static void compareComponents(const MainBodyState49K& a,const MainBodyState49K& b){
    equalWords(&a.components.root,&b.components.root,80,"persistent48H");equalWords(&a.components.engine,&b.components.engine,14,"persistent48D");
    equalWords(&a.components.aero.timer140,&b.components.aero.timer140,2,"persistent48E timers");require(a.components.aero.lowSpeedDc==b.components.aero.lowSpeedDc,"persistent low speed flag");
    equalWords(&a.components.bindings,&b.components.bindings,sizeof(ControlModelBindings48W)/4,"persistent48W");
    equalWords(&a.components.probe348,&b.components.probe348,sizeof(DisabledProbeState49K)/4,"probe state");
    equalWords(&a.components.feedback,&b.components.feedback,4,"lagged flags");require(a.components.body20C==b.components.body20C,"20C clear");
    equalWords(&a.cachedSpeed34,&b.cachedSpeed34,1,"frame cached speed");equalWords(&a.cachedAnglesE0,&b.cachedAnglesE0,3,"full frame angles");
}
static void synthetic354(MainBodyComponents49K& c,unsigned frame,unsigned step){
    // Explicit synthetic external response fixture, not a354 implementation:
    // nonzero load and correction effects demonstrate they are not skipped.
    c.root.loads.force.v[1]=add(c.root.loads.force.v[1],1.25f);
    c.root.body.velocity.v[1]=add(c.root.body.velocity.v[1],.002f);
    c.root.scene.v[11]=frame%31==0?.0001f:0.0f;
    c.root.loads.torque.v[1]=add(c.root.loads.torque.v[1],.003f);
    c.root.positionCorrection1a0={{frame%19==0?.002f:0,0,0}};
    c.root.rotationCorrection1b0={{0,step==1?.00001f:0,0}};
    c.root.accumulator1d0={{.1f,.2f,.3f}};c.root.callbackState238=1;c.body20C=17;
}
static void jsonBits(std::ostream& o,const float* p,unsigned n){o<<'[';for(unsigned i=0;i<n;++i){if(i)o<<',';o<<bits(p[i]);}o<<']';}
namespace fixture=secondary49m_fixture;
static void compareSecondary(const SecondaryBodyState49M& a,const SecondaryBodyState49M& b){
    equalWords(&a.root,&b.root,80,"secondary persistent48H");
    equalWords(&a.cachedSpeed34,&b.cachedSpeed34,4,"secondary frame cache");
    require(a.sceneFlags190==b.sceneFlags190&&a.body20C==b.body20C,"secondary scene/20C");
    equalWords(&a.velocity70W,&b.velocity70W,5,"secondary full vector W/time");
    require(a.phase==b.phase,"secondary phase");
}
static void compareJoint(const PodSphereJoint49L& a,const PodSphereJoint49L& b){
    equalWords(&a.stiffness30,&b.stiffness30,5,"joint tuning/flags");
    require(a.bodyAPresent44==b.bodyAPresent44&&a.bodyBPresent48==b.bodyBPresent48,"joint presence");
    equalWords(&a.length4C,&b.length4C,(sizeof(a)-offsetof(PodSphereJoint49L,length4C))/4,"joint persistent all lanes");
}
static SecondaryBodyState49M mainView(const CoupledBodyState49M& s){
    SecondaryBodyState49M v{};v.root=s.main.components.root;v.scenePresent=true;v.enabled=true;v.sceneFlags190=s.mainSphere.sceneFlags190;
    v.body20C=s.main.components.body20C;v.velocity70W=s.mainSphere.velocity70W;v.angularVelocityB0W=s.mainSphere.angularVelocityB0W;
    v.force180W=s.mainSphere.force180W;v.torque190W=s.mainSphere.torque190W;return v;
}
static SecondaryBodyParameters49M mainParameters(const CoupledBodyParameters49M& p){
    SecondaryBodyParameters49M v{};v.body=p.main.body;v.centerOffset=p.main.centerOffset;v.centerOffsetW=p.mainSphere.centerOffsetW;
    for(unsigned i=0;i<7;++i)v.inverseInertiaExtra[i]=p.mainSphere.inverseInertiaExtra[i];
    return v;
}
static SecondaryForceContext49M secondaryContext(const CoupledBodyState49M& s,const SecondaryInputs49M& p){
    SecondaryForceContext49M c{};c.vehicleState40=s.main.vehicleState40;c.probe104=s.main.components.probe348.field104;
    c.gravity378=p.gravity378;c.gravity37C=p.gravity37C;c.targetRelative2F0=p.targetRelative2F0;
    for(unsigned i=0;i<16;++i)c.mainSceneWorld100.v[i]=s.main.components.root.scene.v[i];
    c.mainSceneFlags190=s.mainSphere.sceneFlags190;c.mainCachedSpeed34=s.main.cachedSpeed34;
    c.effective4346C8=p.effective4346C8;c.effective4346D0=p.effective4346D0;c.effective4346D8=p.effective4346D8;
    c.effective4346E0=p.effective4346E0;c.effective4346E8=p.effective4346E8;c.effective4346F0=p.effective4346F0;
    c.effective4346F8=p.effective4346F8;c.effective434700=p.effective434700;c.effective434708=p.effective434708;return c;
}
static void originalSecondaryCache(SecondaryBodyState49M& s){MainBodyState49K fake{};fake.components.root=s.root;originalRefresh(fake);s.cachedAnglesE0=fake.cachedAnglesE0;s.cachedSpeed34=fake.cachedSpeed34;}
static void originalMainRoot(CoupledBodyState49M& s,const CoupledBodyParameters49M& p){
    auto v=mainView(s);const auto result=fixture::integrateRoot(v,mainParameters(p),s.main.schedule.factorPerSubstep);originalInstructions+=result.instructions;
    s.main.components.root=v.root;s.main.components.body20C=selectorOriginal::clear(v.body20C);
    s.mainSphere={v.sceneFlags190,v.velocity70W,v.angularVelocityB0W,v.force180W,v.torque190W};
}
static void synthetic368(CoupledComponents49M& c,unsigned frame,unsigned step){
    // Supplied contact effects deliberately affect BOTH bodies, including a
    // main impact accumulator cleared here. These are fixtures, never contacts.
    c.main.root.loads.force.v[0]=add(c.main.root.loads.force.v[0],.075f);
    c.main.root.loads.torque.v[2]=add(c.main.root.loads.torque.v[2],.0002f);
    c.main.root.accumulator1c0={};
    c.secondary.root.loads.force.v[2]=add(c.secondary.root.loads.force.v[2],-.4f);
    c.secondary.root.loads.torque.v[0]=add(c.secondary.root.loads.torque.v[0],.003f);
    c.secondary.root.positionCorrection1a0={{step==1?.001f:0,0,0}};
    c.secondary.root.rotationCorrection1b0={{0,0,frame%23==0?.00001f:0}};
    c.secondary.root.callbackState238=0x93;c.secondary.body20C=0xabcdef00u+step;
    c.secondary.force180W=.75f;c.secondary.torque190W=-.5f;
}
static void assign(CoupledBodyState49M& s,const CoupledComponents49M& c){s.main.components=c.main;s.secondary=c.secondary;s.mainSphere=c.mainSphere;s.joint=c.joint;}
// This connected diagnostic mechanically retains the original49M oracle
// helpers, synthetic state and external effects. Only authenticated parameters,
// recovered49D preset rows and explicit selection context replace fixture tuning.
// The separate identity checker records the permitted differences.
static std::uint64_t totalFrames=0,allSubsteps=0,allSecondarySteps=0,allSkipped=0;
struct Outcome49N {RootBodyState48H main,secondary;};
static Outcome49N runScenario(const char* name,unsigned preset,const AuthoredBinding49N& binding,
    const AuthoredSelectionContext49N& context,const SelectedControlPreset49D& selectedPreset,std::ofstream& trace){
    CoupledBodyParameters49M p{};p.main=parameters();p.mainSphere.centerOffsetW=1;p.mainSphere.inverseInertiaExtra[6]=1;
    p.secondary.body=oracleDefaultBodyParameters();p.secondary.body.mass=200;p.secondary.body.inverseMass=.005f;
    for(unsigned i=0;i<3;++i)p.secondary.body.inverseInertiaColumns[i].v[i]=.0005f;
    p.secondary.centerOffset={{0,.1f,0}};p.secondary.centerOffsetW=1;p.secondary.inverseInertiaExtra[2]=.0001f;p.secondary.inverseInertiaExtra[6]=1;
    const auto supplied=p;
    hot([&]{return projectAuthoredCoupledParameters49N(binding,p,p);},"authored49N projection");
    equalWords(&p.secondary,&supplied.secondary,sizeof p.secondary/4,"secondary parameters stay supplied");
    equalWords(&p.main.aero.velocityDamping130,&supplied.main.aero.velocityDamping130,3,"unproved aero130 preserved");
    CoupledBodyState49M native{};native.main=initial();native.main.domain.secondBodyPresent=true;
    native.secondaryDomain={1,2,0,0,0,false};native.mainSphere={1,1,.5f,.25f,-.25f};
    native.secondary.root=native.main.components.root;native.secondary.root.scene.v[14]=-3;native.secondary.root.body.angularVelocity={{.001f,.002f,-.001f}};
    native.secondary.root.body.velocity={{.8f,.1f,29.5f}};native.secondary.root.body.previousPositionDerivative=native.secondary.root.body.velocity;
    native.secondary.scenePresent=true;native.secondary.enabled=true;native.secondary.sceneFlags190=1;native.secondary.body20C=0xabcdef00;
    native.secondary.velocity70W=.75f;native.secondary.angularVelocityB0W=.25f;native.secondary.force180W=.5f;native.secondary.torque190W=-.5f;
    CoupledNoiseContext49M shared{};shared.sharedRandomState=0x49abc123;native.vehicle838=1;native.phase=CoupledBodyPhase49M::FrameReady;
    require(initializePodSphere49L(native.joint),"joint suffix");
    auto sphereA=fixture::sphereBody(native.secondary,p.secondary),sphereB=fixture::sphereBody(mainView(native),mainParameters(p));
    require(setupPodSphereA49L(native.joint,sphereA,{{.1f,0,0,1}}),"joint setup A");
    require(setupPodSphereB49L(native.joint,&sphereB,{{0,0,-3,1}},1),"joint setup B");native.joint.stiffness30=.2f;native.joint.damping34=.5f;
    auto reference=native;auto referenceShared=shared;const auto initialMain=native.main.components.root,initialSecondary=native.secondary.root;
    const auto bp=binding.controls;const auto rates=binding.rates;
    SecondaryForceContext49M defaults{};initializedSecondaryGlobals49M(defaults);
    SecondaryInputs49M si{9.8f,12,{{0,0,-3,1}},defaults.effective4346C8,defaults.effective4346D0,defaults.effective4346D8,defaults.effective4346E0,defaults.effective4346E8,defaults.effective4346F0,defaults.effective4346F8,defaults.effective434700,defaults.effective434708};
    SubstepSelectionContext49K selectedGravity{};
    hot([&]{return projectAuthoredGravity49N(binding,selectedGravity,si,selectedGravity,si);},"authored gravity projection");
    const float traceProfile[]={context.profile.thrust1920,context.profile.speed1924,context.profile.steering1928};
    const float traceRates[]={rates.x,rates.y};
    if(trace){trace<<"{\"name\":\""<<name<<"\",\"preset\":"<<preset<<",\"mode58_nonzero\":"<<(context.mode58Nonzero?"true":"false")<<",\"profile_overwrite\":"<<(context.applyProfileOverwrite?"true":"false")<<",\"mass_bits\":"<<bits(p.main.body.mass)<<",\"inverse_mass_bits\":"<<bits(p.main.body.inverseMass)<<",\"gravity_bits\":["<<bits(si.gravity378)<<','<<bits(si.gravity37C)<<"],\"max_thrust_bits\":"<<bits(p.main.engine.maxThrust)<<",\"steering_bits\":"<<bits(p.main.aero.steering100)<<",\"torque_basis_bits\":";trace<<'[';jsonBits(trace,p.main.engine.torqueBasis[0].v,3);trace<<',';jsonBits(trace,p.main.engine.torqueBasis[1].v,3);trace<<']';trace<<",\"inferred_class\":"<<unsigned(context.inferredClass)<<",\"selected_class\":"<<unsigned(binding.selectedClass)<<",\"profile_abilities_bits\":";jsonBits(trace,traceProfile,3);trace<<",\"joystick_rate_bits\":";jsonBits(trace,traceRates,2);trace<<",\"frames\":[";}
    FrameTimingState49I timing{};ClockCounterState49J clock{};std::uint64_t ticks=0;unsigned totalSubsteps=0,secondarySteps=0;bool counts[4]{},modes[3]{};unsigned skipped=0;
    for(unsigned frame=0;frame<120;++frame){
        const unsigned deltas[]={0,16,33,60,80,9,25,40};auto previousTicks=ticks;ticks+=std::uint64_t(deltas[frame%8])*576;
        clock.pendingOverflows=std::uint32_t((ticks>>16)-(previousTicks>>16));ClockFrameOutput49J cf{};
        hot([&]{return beginFrameFromTimerSample49J(clock,timing,std::uint16_t(ticks),0,cf);},"timer49J");
        require(enterFrameSimulation49I(timing,0,cf.elapsed.elapsedSeconds)==FrameSimulationEntry49I::Ready,"timing ready");
        Provider provider{frame%17,0};FrameReader49K f{{std::uint8_t(80+frame%91),std::uint8_t(50+frame%71),std::uint8_t(180+frame%61),std::uint8_t(40+frame%61)},{{},0,0,0,0,2,native.main.controls.player.flagsB9C},selectedPreset.actions,{&provider,Provider::query},cf.elapsed.elapsedSeconds};
        require(std::isfinite(f.preparedFactor)&&f.preparedFactor>=0&&f.preparedFactor<=.080001f,"bounded synthetic timing admission");
        originalRead(reference.main,f);hot([&]{return beginCoupledBodyFrame49M(native,f,native);},"coupled frame reader");
        equalWords(&native.main.controls.components,&reference.main.controls.components,4,"reader axes");
        equalWords(&native.main.controls.player.raw,&reference.main.controls.player.raw,6,"reader result");
        // Supplied binding states3/4 are outside the admitted route.
        const unsigned mode=frame%3; modes[mode]=true;
        PostPlayerAt217DE0_49K post{reference.main.controls.player,{{},999,mode,context.mode58Nonzero,selectedPreset.configurationEntryNonzero,0,0,0,0,0}};
        if(frame%29==0){post.player.raw.values[0]=.25f;post.player.actionB7C=1;}
        originalBind(reference.main,post,rates,bp);reference.main.vehicleState40=mode;
        hot([&]{return bindCoupledBodyFrame49M(native,post,rates,bp,native);},"coupled frame bind");
        equalWords(&native.main.controls.bound.control,&reference.main.controls.bound.control,sizeof(ControlInputStep48V)/4,"shape filter");
        const auto beforeMain=native.main.components.root,beforeSecondary=native.secondary.root;
        originalRefresh(reference.main);originalSecondaryCache(reference.secondary);reference.main.schedule=selectorOriginal::schedule(reference.main.frameFactor,60);
        native.vehicle838=reference.vehicle838=(frame%13==0?0:1);
        hot([&]{return refreshCoupledBodyFrame49M(native,{coupledComponents49M(native),60,mode,timing.active11C,native.vehicle838},native);},"both frame caches");
        equalWords(&native.main.components.root,&beforeMain,80,"main cache only");equalWords(&native.secondary.root,&beforeSecondary,80,"secondary cache only");
        compareComponents(native.main,reference.main);compareSecondary(native.secondary,reference.secondary);
        counts[native.main.schedule.count-1]=true;const auto mainAngles=native.main.cachedAnglesE0,secondaryAngles=native.secondary.cachedAnglesE0;const float mainSpeed=native.main.cachedSpeed34,secondarySpeed=native.secondary.cachedSpeed34;
        if(trace){if(frame)trace<<',';trace<<"{\"frame\":"<<frame<<",\"milliseconds\":"<<cf.quantization.uint32timestamp<<",\"mode40\":"<<mode<<",\"live838\":"<<native.vehicle838<<",\"factor_bits\":"<<bits(native.main.frameFactor)<<",\"substeps\":[";}
        for(unsigned step=0;step<native.main.schedule.count;++step){
            reference.main.substepIndex=step;
            SubstepSelectionContext49K select{si.gravity378,si.gravity37C,fp(0x43c13334),fp(0x3f34fdf4),frame%5,post.player.flagsB9C};
            reference.main.selectedGravity=selectorOriginal::gravity({reference.main.components.probe348.field104,reference.main.components.bindings.aeroBrakeEC,reference.main.cachedSpeed34,reference.main.components.root.scene.v[5],select.vehicle378,select.vehicle37C,select.effective434730,select.effective434738});
            reference.main.selectedEngineFlags=selectorOriginal::flags(select.vehicle64,select.vehicleB9C,step);
            hot([&]{return beginCoupledBodySubstep49M(native,select,native);},"selectors");
            PostWorld49M world{step==0?WorldPreparationPoint49K::FirstSubstep358And219420:(native.main.vehicle380?WorldPreparationPoint49K::LaterRequired358And219420:WorldPreparationPoint49K::LaterSkipped358And219420),coupledComponents49M(native),frame%2==0,{{.1f,0,.2f}},std::uint8_t(frame%11==0?0:2)};
            if(step==0){world.components.main.root.loads.force.v[0]=add(world.components.main.root.loads.force.v[0],.2f);reference.main.components.root.loads.force.v[0]=add(reference.main.components.root.loads.force.v[0],.2f);}
            reference.main.components.probe348=selectorOriginal::prepare(reference.main.components.probe348,p.main.probeLocal140,reference.main.components.root.scene,p.main.effective434260);
            originalForces(reference.main,p.main,{world.point,reference.main.components,world.aeroSubtractWorldFlow,world.aeroWorldFlow40,world.liveApplicationGateByte58},reference.mainSphere);
            hot([&]{return produceCoupledBodyForces49M(native,p,world,native);},"main force phases");compareComponents(native.main,reference.main);equalWords(&native.mainSphere,&reference.mainSphere,5,"forceW source effects");
            auto contact=coupledComponents49M(native),originalContact=coupledComponents49M(reference);
            synthetic354(contact.main,frame,step);synthetic354(originalContact.main,frame,step);
            contact.secondary.root.body.velocity.v[0]=add(contact.secondary.root.body.velocity.v[0],.003f);
            originalContact.secondary.root.body.velocity.v[0]=add(originalContact.secondary.root.body.velocity.v[0],.003f);
            // A nonzero impact accumulator can be changed by368. Testing the
            // main gate before368 would reject a valid represented sequence.
            if(native.vehicle838){contact.main.root.accumulator1c0.v[0]=.1f;originalContact.main.root.accumulator1c0.v[0]=.1f;}
            assign(reference,originalContact);
            const auto response=selectorOriginal::response(reference.main.components.probe348,reference.main.cachedAnglesE0.v[2],p.main.probe204,p.main.effective4342C8,p.main.effective434290,reference.main.components.root.loads.torque);
            reference.main.components.probe348=response.state;reference.main.components.root.loads.torque=response.torque190;
            const unsigned r3=(frame+step)%3==0,r44=(frame+step)%7;
            hot([&]{return completeCoupledResponse354_49M(native,p,{contact,r3,r44,native.vehicle838},native);},"354 then348");compareComponents(native.main,reference.main);
            const auto mainBeforeJoint=native.main;const auto secondaryBefore=native.secondary;
            fixture::O original;
            if(native.vehicle838){
                fixture::putWrapper(original,reference.secondary,p.secondary,secondaryContext(reference,si),reference.joint,fixture::sphereBody(mainView(reference),mainParameters(p)));
                original.beginWrapper(fixture::vehicle,reference.main.schedule.factorPerSubstep);
                const bool wasCold=!referenceShared.noise.initialized;
                const auto noiseExecution=fixture::runPrefix(original,referenceShared.noise,referenceShared.sharedRandomState);originalInstructions+=noiseExecution.instructions;
                require(noiseExecution.draws==(mode!=0&&wasCold?1791u:0u),"original cold1791/readyzero RNG draws");
                fixture::prefixState(original,reference.secondary);original.until(0x1b6ff0);reference.lastConstraintMagnitude=original.f[0];original.until(fixture::suppliedContactEntry);
                fixture::constraintState(original,reference.secondary,reference.joint);originalInstructions+=original.steps;
                require(original.callCount==4&&original.callTargets[0]==0x1b66d0&&original.callTargets[1]==0x1b6ba0&&original.callTargets[2]==0x1b6ff0&&original.callTargets[3]==fixture::suppliedContactEntry,"original prepare/correct/apply/368 order");
            }
            hot([&]{return advanceCoupledSecondary49M(native,p,si,shared,native);},"secondary wrapper and49L");
            equalWords(&native.main.components.root,&mainBeforeJoint.components.root,80,"main read-only during49L");
            compareSecondary(native.secondary,reference.secondary);compareJoint(native.joint,reference.joint);
            equalWords(&native.lastConstraintMagnitude,&reference.lastConstraintMagnitude,1,"49L correction magnitude");
            require(shared.sharedRandomState==referenceShared.sharedRandomState,"shared RNG persistent");
            equalWords(shared.noise.permutation,referenceShared.noise.permutation,256,"persistent noise permutation");
            equalWords(shared.noise.gradient1D,referenceShared.noise.gradient1D,256,"persistent noise gradients");
            require(shared.noise.scratchCell==referenceShared.noise.scratchCell&&shared.noise.initialized==referenceShared.noise.initialized,"noise scratch/readiness");
            if(native.vehicle838){
                auto supplied=coupledComponents49M(native),expected=coupledComponents49M(reference);synthetic368(supplied,frame,step);synthetic368(expected,frame,step);assign(reference,expected);
                original.resumeSuppliedCallback();original.until(0x167c40);
                require(original.callCount==5&&original.callTargets[4]==0x167c40,"original secondary48H after368");
                originalInstructions+=fixture::integrateRoot(reference.secondary,p.secondary,reference.main.schedule.factorPerSubstep).instructions;
                hot([&]{return completeCoupledResponse368_49M(native,p,supplied,native);},"368 then secondary48H");
                original.resumeSuppliedCallback();original.until(fixture::O::stop);
                compareComponents(native.main,reference.main);compareSecondary(native.secondary,reference.secondary);
                require(native.secondary.body20C==0xabcdef00u+step&&native.secondary.root.callbackState238==0,"secondary callback clears238 not20C");++secondarySteps;
            }else{require(std::memcmp(&secondaryBefore,&native.secondary,sizeof secondaryBefore)==0,"838 skip body unchanged");++skipped;}
            originalMainRoot(reference,p);hot([&]{return integrateCoupledMain49M(native,p,native);},"current impact gate then main48H");
            compareComponents(native.main,reference.main);equalWords(&native.mainSphere,&reference.mainSphere,5,"main W lanes");
            equalWords(&native.main.cachedSpeed34,&mainSpeed,1,"main cache frozen");equalWords(&native.main.cachedAnglesE0,&mainAngles,3,"main angles frozen");
            equalWords(&native.secondary.cachedSpeed34,&secondarySpeed,1,"secondary cache frozen");equalWords(&native.secondary.cachedAnglesE0,&secondaryAngles,3,"secondary angles frozen");
            SuppliedQuery219120_49K query{std::uint8_t(frame%4),{native.main.components.root.scene.v[12],native.main.components.root.scene.v[13],native.main.components.root.scene.v[14],1}};
            hot([&]{return completeCoupledQuery49M(native,query,native);},"supplied nohit219120");reference.main.vehicle380=selectorOriginal::vehicle380(r44);reference.main.components.feedback=selectorOriginal::late(reference.main.components.probe348.field104,r3,r44);compareComponents(native.main,reference.main);
            if(trace){if(step)trace<<',';trace<<"{\"step\":"<<step<<",\"factor_bits\":"<<bits(native.main.schedule.factorPerSubstep)<<",\"main_scene_bits\":";jsonBits(trace,native.main.components.root.scene.v,16);trace<<",\"secondary_scene_bits\":";jsonBits(trace,native.secondary.root.scene.v,16);trace<<",\"main_orientation_bits\":";jsonBits(trace,native.main.components.root.body.orientation.v,4);trace<<",\"secondary_orientation_bits\":";jsonBits(trace,native.secondary.root.body.orientation.v,4);trace<<",\"main_velocity_bits\":";jsonBits(trace,native.main.components.root.body.velocity.v,3);trace<<",\"secondary_velocity_bits\":";jsonBits(trace,native.secondary.root.body.velocity.v,3);trace<<",\"cached_speed_bits\":["<<bits(mainSpeed)<<','<<bits(secondarySpeed)<<"],\"timer864_bits\":"<<bits(native.secondary.time864)<<",\"secondary20C\":"<<native.secondary.body20C<<'}';}
            hot([&]{return completeCoupledSceneConsumer49M(native,{native.main.components.root.scene},native);},"scene consumer");++totalSubsteps;
        }
        if(trace)trace<<"]}";
        require(native.phase==CoupledBodyPhase49M::FrameReady&&native.main.domain.secondBodyPresent,"honest persistent frame closes");leaveFrameSimulation49I(timing);finishFrameTiming49I(timing);
    }
    if(trace)trace<<"]}";
    for(bool c:counts)require(c,"all schedule counts");
    for(bool c:modes)require(c,"all supported binding states");
    require(secondarySteps&&skipped,"wrapper branch coverage");
    require(std::memcmp(&initialMain.scene,&native.main.components.root.scene,sizeof initialMain.scene)!=0,"main pose moved");
    require(std::memcmp(&initialSecondary.scene,&native.secondary.root.scene,sizeof initialSecondary.scene)!=0,"secondary pose moved");
    require(std::memcmp(&initialSecondary.body.orientation,&native.secondary.root.body.orientation,sizeof(Quaternion48C))!=0,"secondary orientation moved");
    totalFrames+=120;allSubsteps+=totalSubsteps;allSecondarySteps+=secondarySteps;allSkipped+=skipped;
    return {native.main.components.root,native.secondary.root};
}

static AuthoredBinding49N prepare(const char* bytes,std::size_t length,const char* track,unsigned preset,bool mode58,bool profile,
    AuthoredSelectionContext49N& context,SelectedControlPreset49D& selected) {
    AuthoredParameters49N raw{};rr::assets::PodPhysicsDiagnostic49N diagnosis{};
    hot([&]{return rr::assets::readPodPhysics49N(bytes,length,raw,diagnosis);},"actual PHY reader");
    AuthoredClass49N category{};
    hot([&]{return selectAuthoredClass49N(track,std::strlen(track),category);},"supplied track class");
    ControlPresets49D presets{};initializeControlPresets49D(presets);
    hot([&]{return selectControlPreset49D(presets,0,preset);},"explicit preset choice");
    hot([&]{return resolveControlPreset49D(presets,0,0,selected);},"selected49D actions/configuration");
    context={category,mode58,selected.configurationEntryNonzero,profile,profile,{.2f,1.5f,.25f}};
    AuthoredBinding49N previous{};previous.main=parameters();previous.controls={0,1,100,.2f,.5f,.25f};
    AuthoredBinding49N actual{};hot([&]{return bindAuthoredParameters49N(raw,context,previous,actual);},"actual authored parameters");
    const auto expected=connectedParameterOriginal49N::bind(raw,context,previous,originalInstructions);
    equalWords(&actual.main.body,&expected.main.body,15,"authoring original body");
    equalWords(&actual.main.engine,&expected.main.engine,15,"authoring original engine");
    equalWords(&actual.main.aero,&expected.main.aero,23,"authoring original aero");
    equalWords(&actual.controls,&expected.controls,6,"authoring original joystick");
    equalWords(&actual.rates,&expected.rates,2,"authoring original rates");
    equalWords(&actual.gravity378,&expected.gravity378,1,"authoring original gravity");
    equalWords(&actual.airborneGravity37C,&expected.airborneGravity37C,1,"authoring original airborne gravity");
    equalWords(&actual.inertia100,&expected.inertia100,16,"authoring original inertia");
    equalWords(&actual.inverseInertia140,&expected.inverseInertia140,16,"authoring original inverse");
    equalWords(&actual.engineDifference5C,&expected.engineDifference5C,1,"authoring original difference");
    require(actual.selectedClass==expected.selectedClass&&actual.usedAuthoredClassValues==expected.usedAuthoredClassValues&&actual.massSetterWrote==expected.massSetterWrote,"source selection metadata");
    return actual;
}
static void changedBodies(const Outcome49N& baseline,const Outcome49N& changed,const char* why){
    require(std::memcmp(&baseline.main.scene,&changed.main.scene,sizeof baseline.main.scene)!=0,why);
    require(std::memcmp(&baseline.secondary.scene,&changed.secondary.scene,sizeof baseline.secondary.scene)!=0,"changed tuning propagates to secondary pose");
}
static std::string changedText(const std::string& baseline,const char* from,const char* to){
    std::string result=baseline;const auto at=result.find(from);require(at!=std::string::npos,"sensitivity source token");
    require(result.find(from,at+1)==std::string::npos,"sensitivity token unique");result.replace(at,std::strlen(from),to);return result;
}
int main(int argc,char** argv){try{
    require(std::fesetround(FE_TONEAREST)==0,"nearest");
    require(argc>=2,"usage: authored_body_diagnostic49n Pod01.phy [trace.json]");
    std::ifstream input(argv[1],std::ios::binary);require(bool(input),"actual PHY fixture open");
    std::array<char,rr::assets::maxPodPhysicsBytes49N+1> bytes{};
    input.read(bytes.data(),bytes.size());const auto length=static_cast<std::size_t>(input.gcount());
    require(length==6584&&input.eof(),"authenticated fixture byte count; SHA checked by runner");
    const std::string baseline(bytes.data(),length);
    std::ofstream trace;if(argc>2){trace.open(argv[2]);require(bool(trace),"trace open");trace<<"{\"kind\":\"actual Pod01 PHY parameters with supplied two-body state and external effects\",\"phy_sha256\":\"47fcfbaab278d10506b302520adc51af4e3388ceb601daa3dbd588aea1f39a1b\",\"profile_is_supplied_not_live\":true,\"scenarios\":[";}
    AuthoredSelectionContext49N context{};SelectedControlPreset49D selected{};
    auto binding=prepare(baseline.data(),baseline.size(),"ta",0,false,false,context,selected);
    require(binding.massSetterWrote,"actual positive Pod01 mass setter took source write branch");
    require(binding.main.body.mass>30&&binding.main.body.mass<32,"actual main mass replaces tiny fixture mass");
    require(binding.main.engine.maxThrust==8475&&binding.rates.x==4&&binding.rates.y==3,"actual engine and joystick values");
    const auto base=runScenario("actual-class-A-preset0-profile-bypass",0,binding,context,selected,trace);
    if(trace)trace<<',';
    auto presetBinding=prepare(baseline.data(),baseline.size(),"ra",4,false,false,context,selected);
    require(presetBinding.selectedClass==AuthoredClass49N::B,"ra source class exception");
    require(presetBinding.main.aero.steering100==-4000,"preset halves actual authored steering");
    runScenario("actual-class-B-preset4-profile-bypass",4,presetBinding,context,selected,trace);
    if(trace)trace<<',';
    auto profileBinding=prepare(baseline.data(),baseline.size(),"sb",4,false,true,context,selected);
    require(profileBinding.selectedClass==AuthoredClass49N::C,"sb source class exception");
    require(profileBinding.main.aero.steering100==-5750&&profileBinding.main.engine.maxThrust==4800,"explicit profile overwrite");
    equalWords(profileBinding.main.engine.torqueBasis,presetBinding.main.engine.torqueBasis,6,"profile does not rebuild earlier preset vectors");
    const auto profile=runScenario("actual-class-C-preset4-supplied-profile",4,profileBinding,context,selected,trace);
    if(trace)trace<<',';
    auto bypassBinding=prepare(baseline.data(),baseline.size(),"ta",4,true,true,context,selected);
    equalWords(bypassBinding.main.engine.torqueBasis,binding.main.engine.torqueBasis,6,"mode58 bypasses preset vector replacement");
    runScenario("actual-class-A-preset4-mode58-profile",4,bypassBinding,context,selected,trace);
    const char* names[]={"changed-authored-main-weight","changed-authored-engine-thrust","changed-authored-aero-drag","changed-authored-joystick-rate","changed-authored-airborne-gravity"};
    const char* from[]={"Weight = 1000\r\n","MaxThrustA = 8475\r\n","BasicDrag = 0.02\r\n","XAxisRateLimit = 4\r\n","AirborneGravity = 10\r\n"};
    const char* to[]={"Weight = 1200\r\n","MaxThrustA = 8000\r\n","BasicDrag = 0.03\r\n","XAxisRateLimit = 2\r\n","AirborneGravity = 11\r\n"};
    for(unsigned i=0;i<5;++i){
        const auto variant=changedText(baseline,from[i],to[i]);
        auto tuned=prepare(variant.data(),variant.size(),"ta",0,false,false,context,selected);
        if(trace)trace<<',';
        const auto result=runScenario(names[i],0,tuned,context,selected,trace);
        changedBodies(base,result,"changed authenticated parameter alters real main pose");
    }
    const auto groundVariant=changedText(baseline,"Gravity = 6\r\n","Gravity = 7\r\n");
    const auto groundBinding=prepare(groundVariant.data(),groundVariant.size(),"ta",0,false,false,context,selected);
    require(bits(groundBinding.gravity378)!=bits(binding.gravity378),"ground gravity text projects differently");
    if(trace)trace<<',';
    const auto ground=runScenario("negative-control-unselected-ground-gravity",0,groundBinding,context,selected,trace);
    equalWords(&ground.main,&base.main,sizeof ground.main/4,"unselected ground gravity leaves main unchanged");
    equalWords(&ground.secondary,&base.secondary,sizeof ground.secondary/4,"unselected ground gravity leaves secondary unchanged");
    // Deliberately wrong source-order control, not an admitted authoring path.
    // Rebuilding bases from the final profile steering must change both bodies.
    prepare(baseline.data(),baseline.size(),"sb",4,false,true,context,selected);
    auto wrongOrder=profileBinding;wrongOrder.main.engine.torqueBasis[0].v[1]=-wrongOrder.main.aero.steering100;
    wrongOrder.main.engine.torqueBasis[1].v[1]=wrongOrder.main.aero.steering100;
    if(trace)trace<<',';
    const auto wrong=runScenario("sensitivity-only-wrong-recomputed-profile-vectors",4,wrongOrder,context,selected,trace);
    changedBodies(profile,wrong,"source-order vector trap changes main pose");
    if(trace)trace<<"],\"summary\":{\"frames\":"<<totalFrames<<",\"substeps\":"<<allSubsteps<<",\"secondary_steps\":"<<allSecondarySteps<<",\"live838_skips\":"<<allSkipped<<",\"checks\":"<<checks<<",\"hot_transitions\":"<<hotTransitions<<",\"hot_allocations\":0,\"original_instructions\":"<<(originalInstructions+advancement49k_oracle::instructionCount)<<"}}\n";
    std::cout<<"authored_body_diagnostic49n: "<<checks<<" checks; "<<totalFrames<<" persistent supplied frames; "<<allSubsteps<<" substeps; "<<allSecondarySteps<<" secondary steps; "<<allSkipped<<" live838 skips; "<<(originalInstructions+advancement49k_oracle::instructionCount)<<" original-word instructions; "<<hotTransitions<<" allocation-free native calls; actual Pod01 PHY +11 bounded scenarios; retained synthetic state/contact geometry\n";
    return 0;
}catch(const std::exception& e){std::cerr<<"authored_body_diagnostic49n after "<<checks<<" checks: "<<e.what()<<'\n';return 1;}}
