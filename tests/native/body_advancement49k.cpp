#include <string>
#include "../../src/game/body_advancement49k.h"
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
static void originalForces(MainBodyState49K& s,const MainBodyParameters49K& p,const PostWorldPreparation49K& w){
    auto& c=s.components;EngineInput48D e{{c.bindings.engineDrive[0],c.bindings.engineDrive[1]},s.cachedSpeed34,s.selectedEngineFlags.engine34!=0,s.selectedEngineFlags.engine38!=0,c.feedback.engine3C!=0};
    AeroInput48E a{c.root.localVelocity80,c.root.body.angularVelocity,s.cachedAnglesE0.v[0],w.aeroSubtractWorldFlow,w.aeroWorldFlow40,
        c.feedback.aeroD0!=0,c.bindings.aeroTriggerD8!=0,c.feedback.aeroE0!=0,c.bindings.aeroDrive[0],c.bindings.aeroDrive[1],c.bindings.aeroBrakeEC};
    OriginalWrapper48H m;m.set(c.root.body,p.body,c.root.loads);putEngine(m,c.engine,p.engine,e);
    const auto b=rootBasis(c.root);putForceContext(m,b,{s.selectedGravity,p.gravityForceX,p.gravityForceZ},true);
    callOriginal(m,rr_engine_update_48d_start,engineAddress,s.schedule.factorPerSubstep);c.engine=getEngine(m);originalInstructions+=m.steps;
    callOriginal(m,rr_gravity_48d_start,gravityAddress,s.schedule.factorPerSubstep);originalInstructions+=m.steps;
    if(w.liveApplicationGateByte58&2){callOriginal(m,rr_engine_apply_48d_start,engineAddress,s.schedule.factorPerSubstep);originalInstructions+=m.steps;}
    putAero(m,c.aero,p.aero,a,b,{m.vec(0x180),m.vec(0x190)});
    callOriginal(m,rr_aero_48e_start,aeroAddress,s.schedule.factorPerSubstep);const auto result=getAero(m);originalInstructions+=m.steps;
    c.aero=result.state;c.root.loads=result.accumulatedLoads;
}
static void originalRoot(MainBodyState49K& s,const MainBodyParameters49K& p){OriginalWrapper48H m;putRoot(m,s.components.root,p.body,p.centerOffset);callOriginal(m,rr_body_wrapper_48h_start,0,s.schedule.factorPerSubstep);s.components.root=getRoot(m);originalInstructions+=m.steps;}
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
    c.root.loads.torque.v[1]=add(c.root.loads.torque.v[1],.003f);
    c.root.positionCorrection1a0={{frame%19==0?.002f:0,0,0}};
    c.root.rotationCorrection1b0={{0,step==1?.00001f:0,0}};
    c.root.accumulator1d0={{.1f,.2f,.3f}};c.root.callbackState238=1;c.body20C=17;
}
static void jsonArray(std::ostream& o,const float* p,unsigned n){o<<'[';for(unsigned i=0;i<n;++i){if(i)o<<',';o<<std::setprecision(9)<<p[i];}o<<']';}
static void jsonBits(std::ostream& o,const float* p,unsigned n){o<<'[';for(unsigned i=0;i<n;++i){if(i)o<<',';o<<bits(p[i]);}o<<']';}
int main(int argc,char** argv){try{
    static_assert(std::is_trivially_copyable_v<MainBodyState49K>);
    require(std::fesetround(FE_TONEAREST)==0,"nearest mode");
    auto native=initial(),reference=native;const auto p=parameters();const ControlBindingParameters48W bp{0,1,100,.2f,.5f,.25f};const ControlRateLimits48B rates{3,2};
    std::ofstream trace;if(argc>1){trace.open(argv[1]);require(bool(trace),"trace open");trace<<"{\"kind\":\"synthetic bounded main-body diagnostic\",\"external_stages\":\"explicit post-player, post-base, first-world preparation, nonzero354 response, no-hit219120, scene observation\",\"frames\":[";}
    FrameTimingState49I timing{};ClockCounterState49J clock{};std::uint64_t syntheticTicks=0;std::uint32_t timestamp=0;unsigned substeps=0;bool allCounts[4]{};bool speedChanged=false,orientationChanged=false,lagObserved=false;
    const auto starting=native.components.root;
    for(unsigned frame=0;frame<180;++frame){
        const unsigned deltas[]={0,16,33,60,80,9,25,40};
        const auto oldTicks=syntheticTicks;syntheticTicks+=std::uint64_t(deltas[frame%8])*576u;
        clock.pendingOverflows=static_cast<std::uint32_t>((syntheticTicks>>16)-(oldTicks>>16));
        const auto pendingOverflows=clock.pendingOverflows;ClockFrameOutput49J clockFrame{};
        hot([&]{return beginFrameFromTimerSample49J(clock,timing,static_cast<std::uint16_t>(syntheticTicks),0,clockFrame);},"supplied timer49J begin");
        const auto elapsed=clockFrame.elapsed;timestamp=clockFrame.quantization.uint32timestamp;
        require(enterFrameSimulation49I(timing,0,elapsed.elapsedSeconds)==FrameSimulationEntry49I::Ready,"timing ready");
        Provider provider{frame%17,0};FrameReader49K f{{std::uint8_t(80+frame%91),std::uint8_t(50+frame%71),std::uint8_t(180+frame%61),std::uint8_t(40+frame%61)},
            {{},0,0,0,0,2,native.controls.player.flagsB9C},{3,5,7,9,11},{&provider,Provider::query},elapsed.elapsedSeconds};
        originalRead(reference,f);hot([&]{return beginMainBodyFrame49K(native,f,native);},"frame reader");
        equalWords(&native.controls.components,&reference.controls.components,4,"original axis");equalWords(&native.controls.player.raw,&reference.controls.player.raw,6,"original reader");require(native.controls.player.flagsB9C==reference.controls.player.flagsB9C,"reader flags");
        PostPlayerAt217DE0_49K post{reference.controls.player,{{},999,frame%4==3?5u:frame%4,false,frame%8<4,0,0,0,0,0}};
        // A named synthetic post-player effect changes raw0 and an action at a
        // few frames. This is supplied to both sides, not claimed as recovered.
        if(frame%29==0){post.player.raw.values[0]=.25f;post.player.actionB7C=1;}
        originalBind(reference,post,rates,bp);hot([&]{return bindMainBodyFrame49K(native,post,rates,bp,native);},"frame binding");
        equalWords(&native.controls.bound.control,&reference.controls.bound.control,sizeof(ControlInputStep48V)/4,"original shape/filter");equalWords(&native.components.bindings,&reference.components.bindings,sizeof(ControlModelBindings48W)/4,"original binding oldspeed");
        const auto beforeRefresh=native.components.root;const float oldSpeed=native.cachedSpeed34;
        originalRefresh(reference);reference.schedule=selectorOriginal::schedule(reference.frameFactor,60);
        hot([&]{return refreshMainBodyFrame49K(native,{native.components,60,post.bindingContext.vehicleState40,timing.active11C},native);},"frame cache refresh");
        equalWords(&native.components.root,&beforeRefresh,80,"childphase must not integrate");compareComponents(native,reference);allCounts[native.schedule.count-1]=true;speedChanged|=bits(oldSpeed)!=bits(native.cachedSpeed34);
        const auto frozenAngles=native.cachedAnglesE0;const float frozenSpeed=native.cachedSpeed34;
        if(trace){if(frame)trace<<',';trace<<"{\"frame\":"<<frame<<",\"milliseconds\":"<<timestamp<<",\"synthetic_count\":"<<static_cast<std::uint16_t>(syntheticTicks)<<",\"supplied_pending_overflows\":"<<pendingOverflows<<",\"factor\":"<<std::setprecision(9)<<native.frameFactor<<",\"factor_bits\":"<<bits(native.frameFactor)<<",\"binding_cached_speed_bits\":"<<bits(oldSpeed)<<",\"count\":"<<native.schedule.count<<",\"steps\":[";}
        for(unsigned step=0;step<native.schedule.count;++step){
            SubstepSelectionContext49K select{9.8f,12,fp(0x43c13334),fp(0x3f34fdf4),frame%5,post.player.flagsB9C};
            reference.selectedGravity=selectorOriginal::gravity({reference.components.probe348.field104,reference.components.bindings.aeroBrakeEC,reference.cachedSpeed34,reference.components.root.scene.v[5],select.vehicle378,select.vehicle37C,select.effective434730,select.effective434738});reference.selectedEngineFlags=selectorOriginal::flags(select.vehicle64,select.vehicleB9C,step);
            const auto incoming104=native.components.probe348.field104;hot([&]{return beginMainBodySubstep49K(native,select,native);},"substep selectors");
            equalWords(&native.selectedGravity,&reference.selectedGravity,1,"gravity input order");equalWords(&native.selectedEngineFlags,&reference.selectedEngineFlags,2,"engine flags order");
            PostWorldPreparation49K prep{step==0?WorldPreparationPoint49K::FirstSubstep358And219420:(native.vehicle380?WorldPreparationPoint49K::LaterRequired358And219420:WorldPreparationPoint49K::LaterSkipped358And219420),native.components,frame%2==0,{{.1f,0,.2f}},std::uint8_t(frame%11==0?0:2)};
            // Synthetic first-world preparation has a visible load effect.
            if(step==0){prep.components.root.loads.force.v[0]=add(prep.components.root.loads.force.v[0],.2f);reference.components.root.loads.force.v[0]=add(reference.components.root.loads.force.v[0],.2f);}
            const auto priorFeedback=native.components.feedback;
            reference.components.probe348=selectorOriginal::prepare(reference.components.probe348,p.probeLocal140,reference.components.root.scene,p.effective434260);originalForces(reference,p,prep);
            hot([&]{return produceMainBodyForces49K(native,p,prep,native);},"force stages");compareComponents(native,reference);
            auto response=native.components;synthetic354(response,frame,step);synthetic354(reference.components,frame,step);
            const auto probe=selectorOriginal::response(reference.components.probe348,reference.cachedAnglesE0.v[2],p.probe204,p.effective4342C8,p.effective434290,reference.components.root.loads.torque);reference.components.probe348=probe.state;reference.components.root.loads.torque=probe.torque190;
            originalRoot(reference,p);reference.components.body20C=selectorOriginal::clear(reference.components.body20C);
            const unsigned response3C=(frame+step)%3==0?1:0,response44=(frame+step)%7;
            hot([&]{return completeMainBodyResponse49K(native,p,{response,response3C,response44},native);},"body response integration");compareComponents(native,reference);
            require(native.components.probe348.field104==0&&native.components.probe348.field10C==1,"disabled probe response flags");
            equalWords(&native.cachedSpeed34,&frozenSpeed,1,"no per-step speed refresh");equalWords(&native.cachedAnglesE0,&frozenAngles,3,"no per-step angle refresh");
            SuppliedQuery219120_49K query{std::uint8_t(frame%4),{native.components.root.scene.v[12],native.components.root.scene.v[13],native.components.root.scene.v[14],1}};
            hot([&]{return completeMainBodyQuery49K(native,query,native);},"required supplied query");reference.vehicle380=selectorOriginal::vehicle380(response44);require(native.vehicle380==reference.vehicle380,"live380 original-word write");reference.components.feedback=selectorOriginal::late(reference.components.probe348.field104,response3C,response44);
            compareComponents(native,reference);equalWords(native.priorPoint7B0,query.currentScenePoint7C0,4,"nohit point copy");
            lagObserved|=priorFeedback.aeroD0!=native.components.feedback.aeroD0;
            if(trace){if(step)trace<<',';trace<<"{\"step\":"<<step<<",\"factor\":"<<native.schedule.factorPerSubstep<<",\"incoming348_104\":"<<incoming104<<",\"gravity\":"<<native.selectedGravity<<",\"engine_flags\":["<<native.selectedEngineFlags.engine34<<','<<native.selectedEngineFlags.engine38<<"],\"previous_feedback\":["<<priorFeedback.engine3C<<','<<priorFeedback.aeroD0<<"],\"next_feedback\":["<<native.components.feedback.engine3C<<','<<native.components.feedback.aeroD0<<"],\"scene\":";jsonArray(trace,native.components.root.scene.v,16);trace<<",\"velocity\":";jsonArray(trace,native.components.root.body.velocity.v,3);trace<<",\"orientation\":";jsonArray(trace,native.components.root.body.orientation.v,4);trace<<",\"cached_angles\":";jsonArray(trace,native.cachedAnglesE0.v,3);trace<<",\"cached_speed\":"<<native.cachedSpeed34<<",\"cached_speed_bits\":"<<bits(native.cachedSpeed34)<<",\"factor_bits\":"<<bits(native.schedule.factorPerSubstep)<<",\"gravity_bits\":"<<bits(native.selectedGravity)<<",\"world_preparation\":"<<static_cast<unsigned>(prep.point)<<",\"vehicle380\":"<<native.vehicle380<<",\"engine_application_gate\":"<<unsigned(prep.liveApplicationGateByte58)<<",\"scene_bits\":";jsonBits(trace,native.components.root.scene.v,16);trace<<",\"velocity_bits\":";jsonBits(trace,native.components.root.body.velocity.v,3);trace<<",\"orientation_bits\":";jsonBits(trace,native.components.root.body.orientation.v,4);trace<<",\"cached_angles_bits\":";jsonBits(trace,native.cachedAnglesE0.v,3);trace<<'}';}
            hot([&]{return completeMainBodySceneConsumer49K(native,{native.components.root.scene},native);},"scene consumer observation");++substeps;
        }
        if(trace)trace<<"]}";
        require(native.phase==MainBodyPhase49K::FrameReady,"frame closes");leaveFrameSimulation49I(timing);finishFrameTiming49I(timing);
    }
    if(trace)trace<<"]}\n";
    orientationChanged=std::memcmp(&starting.body.orientation,&native.components.root.body.orientation,sizeof(Quaternion48C))!=0;
    require(orientationChanged&&speedChanged&&lagObserved,"persistent response evidence");require(native.components.root.scene.v[14]!=starting.scene.v[14],"actual pose advancement");for(bool b:allCounts)require(b,"substep count coverage");
    // Rejections preserve all output bytes; missing mandatory boundaries cannot
    // be bypassed by calling the next stage. External reads are checked separately.
    auto reject=[&](auto f,const char* label){auto out=native;const auto old=out;require(!f(out),label);require(std::memcmp(&old,&out,sizeof out)==0,"rejection atomicity");};
    Provider provider{};FrameReader49K f{{128,128,128,128},{},{-1,-1,1,2,3},{&provider,Provider::query},.016f};
    for(unsigned k=0;k<13;++k){auto bad=native;switch(k){case 0:bad.domain.body320=0;break;case 1:bad.domain.bodyChildCount=1;break;case 2:bad.domain.sceneChildCount=1;break;case 3:bad.domain.sceneObserverCount=1;break;case 4:bad.domain.sceneAttached=false;break;case 5:bad.domain.sceneHasParent=true;break;case 6:bad.domain.bodyEnabled=false;break;case 7:bad.domain.secondBodyPresent=true;break;case 8:bad.domain.override3C=true;break;case 9:bad.domain.resetRequested=true;break;case 10:bad.domain.probe100=1;break;case 11:bad.domain.probe120Present=true;break;case 12:bad.domain.probe124Present=true;break;}
        provider.count=0;reject([&](MainBodyState49K& out){return beginMainBodyFrame49K(bad,f,out);},"unsupported domain");require(provider.count==0,"unsupported context read device");}
    for(unsigned blockedState:{3u,4u}){auto state=native;state.phase=MainBodyPhase49K::AwaitingPostPlayer;
        PostPlayerAt217DE0_49K post{state.controls.player,{}};post.bindingContext.vehicleState40=blockedState;
        reject([&](MainBodyState49K& out){return bindMainBodyFrame49K(state,post,rates,bp,out);},"binding branch skipped state");
        state.phase=MainBodyPhase49K::AwaitingPostBase;
        reject([&](MainBodyState49K& out){return refreshMainBodyFrame49K(state,{state.components,60,blockedState,0},out);},"scheduler skipped vehicle state");}
    {auto state=native;state.phase=MainBodyPhase49K::AwaitingPostBase;
        reject([&](MainBodyState49K& out){return refreshMainBodyFrame49K(state,{state.components,60,0,1},out);},"active app child-only branch");}
    reject([&](MainBodyState49K& out){return completeMainBodyQuery49K(native,{},out);},"query before integration");
    auto blocked=native;blocked.substepIndex=0;blocked.phase=MainBodyPhase49K::AwaitingResponse354;
    auto response=blocked.components;response.root.accumulator1c0={{0,1,0}};
    reject([&](MainBodyState49K& out){return completeMainBodyResponse49K(blocked,p,{response,0,0},out);},"impact branch unsupported");
    blocked.phase=MainBodyPhase49K::AwaitingQuery219120;
    reject([&](MainBodyState49K& out){return completeMainBodyQuery49K(blocked,{0x20,{0,0,0,1}},out);},"query hit unsupported");
    blocked.phase=MainBodyPhase49K::AwaitingSceneConsumer;auto wrongScene=blocked.components.root.scene;wrongScene.v[12]+=1;
    reject([&](MainBodyState49K& out){return completeMainBodySceneConsumer49K(blocked,{wrongScene},out);},"wrong consumer transform");
    blocked=native;blocked.substepIndex=0;blocked.phase=MainBodyPhase49K::AwaitingWorldPreparation;
    const PostWorldPreparation49K validPrep{WorldPreparationPoint49K::FirstSubstep358And219420,blocked.components,false,{},2};
    for(unsigned k=0;k<4;++k){auto bad=blocked;if(k==0)bad.schedule.count=0;if(k==1)bad.schedule.count=5;if(k==2)bad.schedule.factorPerSubstep=-1;if(k==3)bad.substepIndex=bad.schedule.count;
        reject([&](MainBodyState49K& out){return produceMainBodyForces49K(bad,p,validPrep,out);},"forged schedule rejected");}
    blocked.substepIndex=1;blocked.vehicle380=0;auto skipped=validPrep;skipped.point=WorldPreparationPoint49K::LaterSkipped358And219420;skipped.components.root.loads.force.v[0]+=1;
    reject([&](MainBodyState49K& out){return produceMainBodyForces49K(blocked,p,skipped,out);},"skipped world stage cannot add effects");
    for(int rounding:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){
        require(std::fesetround(rounding)==0,"alternate round setup");provider.count=0;
        reject([&](MainBodyState49K& out){return beginMainBodyFrame49K(native,f,out);},"nonnearest reader");require(provider.count==0,"nonnearest must not read device");
        auto state=native;state.substepIndex=0;state.phase=MainBodyPhase49K::AwaitingPostPlayer;
        reject([&](MainBodyState49K& out){return bindMainBodyFrame49K(state,{state.controls.player,{}},rates,bp,out);},"nonnearest binding");
        state.phase=MainBodyPhase49K::AwaitingPostBase;
        reject([&](MainBodyState49K& out){return refreshMainBodyFrame49K(state,{state.components,60},out);},"nonnearest refresh");
        state.phase=MainBodyPhase49K::SubstepReady;
        reject([&](MainBodyState49K& out){return beginMainBodySubstep49K(state,{9.8f,12,.7f,.7f,0,0},out);},"nonnearest selectors");
        state.phase=MainBodyPhase49K::AwaitingWorldPreparation;
        reject([&](MainBodyState49K& out){return produceMainBodyForces49K(state,p,validPrep,out);},"nonnearest producers");
        state.phase=MainBodyPhase49K::AwaitingResponse354;
        reject([&](MainBodyState49K& out){return completeMainBodyResponse49K(state,p,{state.components,0,0},out);},"nonnearest response");
        state.phase=MainBodyPhase49K::AwaitingQuery219120;
        reject([&](MainBodyState49K& out){return completeMainBodyQuery49K(state,{0,{0,0,0,1}},out);},"nonnearest query");
        state.phase=MainBodyPhase49K::AwaitingSceneConsumer;
        reject([&](MainBodyState49K& out){return completeMainBodySceneConsumer49K(state,{state.components.root.scene},out);},"nonnearest scene acknowledgement");
    }
    require(std::fesetround(FE_TONEAREST)==0,"restore nearest");
    f.preparedFactor=std::numeric_limits<float>::quiet_NaN();reject([&](MainBodyState49K& out){return beginMainBodyFrame49K(native,f,out);},"invalid frame factor");
    std::cout<<"body_advancement49k: "<<checks<<" checks; 180 persistent synthetic frames; "<<substeps<<" substeps; "<<(originalInstructions+advancement49k_oracle::instructionCount)<<" retained original-word instructions; "<<hotTransitions<<" allocation-free native transitions; state "<<sizeof(MainBodyState49K)<<" bytes; all4counts; persistent scene/velocity/orientation; explicit external boundaries\n";
    return 0;
}catch(const std::exception& e){std::cerr<<"body_advancement49k after "<<checks<<" checks: "<<e.what()<<'\n';return 1;}}
