#include <string>
#include "../../src/game/vehicle_aero48e.h"
#include "../../tools/gameplay/aero_oracle48e.h"

static constexpr std::uint32_t engineAddress=0x400, gravityAddress=0x800, sceneAddress=0x1000;

static void callOriginal(Original48E& m,std::uint32_t entry,std::uint32_t object,float dt) {
    m.steps=0;m.g[4][0]=object;m.g[29][0]=0x1800;m.g[31][0]=0xf00df00d;m.f[12]=dt;m.v[0][3]=1;
    auto pc=entry;
    while(pc!=0xf00df00d) {
        std::uint32_t target=0,unused=0;
        if(m.execute(pc,target)) {
            if(m.execute(pc+4,unused)) throw std::runtime_error("force oracle branch in delay slot");
            pc=target;
        } else pc+=4;
    }
    if(m.g[29][0]!=0x1800) throw std::runtime_error("force oracle stack did not return");
}
static void putEngine(Original48E& m,const EngineState48D& s,const EngineParameters48D& p,const EngineInput48D& in) {
    m.write(engineAddress+0x30,0); // Relocated body fixture starts at zero.
    m.write(engineAddress+0x34,in.boost);m.write(engineAddress+0x38,in.repair);m.write(engineAddress+0x3c,in.fullTorque);
    m.store(0x34,in.bodySpeed);
    const float parameters[]={p.maxThrust,p.boostThrust,p.repairThrust,p.rampUpFilter,p.rampDownFilter,
                              p.rampDownSpeed,p.theoreticalTopSpeed,0,p.boostSpeed,p.repairSpeed};
    for(unsigned i=0;i<10;++i)m.store(engineAddress+0x40+i*4,parameters[i]);
    for(unsigned i=0;i<2;++i) {
        m.store(engineAddress+0x68+i*4,in.drive[i]);m.store(engineAddress+0x70+i*4,s.smoothedDrive[i]);
        m.store(engineAddress+0x80+i*16,p.torqueBasis[i]);
        m.store(engineAddress+0xa0+i*16,s.localThrust[i]);m.store(engineAddress+0xc0+i*16,s.torque[i]);
    }
}
static EngineState48D getEngine(const Original48E& m) {
    return {{m.readFloat(engineAddress+0x70),m.readFloat(engineAddress+0x74)},
            {m.vec(engineAddress+0xa0),m.vec(engineAddress+0xb0)},
            {m.vec(engineAddress+0xc0),m.vec(engineAddress+0xd0)}};
}
static void putForceContext(Original48E& m,const ReadyBodyBasis48D& basis,GravityParameters48D gravity,bool hasScene) {
    m.write(0x288,hasScene ? sceneAddress:0);
    for(unsigned i=0;i<3;++i)m.store(sceneAddress+0x100+i*16,basis.columns[i]);
    m.write(sceneAddress+0x190,1); // Explicit supported original cached-matrix route.
    m.write(gravityAddress+0x30,1);m.write(gravityAddress+0x50,0x900);m.write(0x900,0);
    m.store(gravityAddress+0x38,gravity.magnitude);
    m.store(gravityAddress+0x40,gravity.forceX);m.store(gravityAddress+0x48,gravity.forceZ);
}
static BodyParameters48C bodyParameters() {
    return {1,1,{{{.001f,0,0}},{{0,.001f,0}},{{0,0,.001f}}},1e10f,1e10f,1e10f,1e10f,
            {true,true,true},{true,true,true}};
}
static ReadyBodyBasis48D identityBasis() { return {{{{1,0,0}},{{0,1,0}},{{0,0,1}}}}; }
static EngineParameters48D engineParameters() {
    return {4800,1800,1600,1,1,161.3325958251953f,879.9960327148438f,293.3320007324219f,
            36.666500091552734f,{{{-.3f,0,0}},{{.3f,0,0}}}};
}

static constexpr std::uint32_t aeroAddress=0xa00, aeroBody=0xc00;
static constexpr unsigned coefficientOffsets[]={0x68,0x6c,0x78,0x84,0x88,0x90,0xb0,0xb8,0xc8,
    0xf8,0xfc,0x100,0x104,0x108,0x10c,0x110,0x114,0x118,0x11c,0x120};
static AeroParameters48E savedParameters() {
    AeroParameters48E p{};float* fields[]={&p.offset68,&p.lift6c,&p.drag78,&p.pitch84,&p.pitchDamping88,
        &p.lateral90,&p.rollDampingB0,&p.timerYawB8,&p.yawDampingC8,&p.driveDragF8,&p.brakeDragFc,
        &p.steering100,&p.steeringRamp104,&p.timerSteering108,&p.timerLateral10c,&p.timerYaw110,
        &p.timerDrag114,&p.tiltedLateral118,&p.tiltedSteering11c,&p.tiltedYaw120};
    for(unsigned i=0;i<20;++i)*fields[i]=fp(rr_saved_aero_48e[coefficientOffsets[i]/4]);
    for(unsigned i=0;i<3;++i)p.velocityDamping130.v[i]=fp(rr_saved_aero_48e[0x130/4+i]);
    return p;
}
static void putAero(Original48E& m,const AeroState48E& s,const AeroParameters48E& p,
                    const AeroInput48E& in,const ReadyBodyBasis48D& basis,const BodyLoads48C& loads) {
    // A non-null linked body is necessary for the original producer. The body
    // core oracle uses zero as its relocated object, so the fixture mirrors
    // fields explicitly and copies resulting loads back before that core.
    m.write(aeroAddress+0x30,aeroBody);m.write(aeroBody+0x288,sceneAddress);
    m.store(aeroBody+0x80,in.bodyLocalVelocity);m.store(aeroBody+0xb0,in.angularVelocity);
    m.store(aeroBody+0xe0,in.bodyAngleE0);m.store(aeroBody+0x180,loads.force);m.store(aeroBody+0x190,loads.torque);
    m.write(aeroAddress+0x34,in.subtractWorldFlow);m.store(aeroAddress+0x40,in.worldFlow40);
    const float fields[]={p.offset68,p.lift6c,p.drag78,p.pitch84,p.pitchDamping88,p.lateral90,
        p.rollDampingB0,p.timerYawB8,p.yawDampingC8,p.driveDragF8,p.brakeDragFc,p.steering100,
        p.steeringRamp104,p.timerSteering108,p.timerLateral10c,p.timerYaw110,p.timerDrag114,
        p.tiltedLateral118,p.tiltedSteering11c,p.tiltedYaw120};
    for(unsigned i=0;i<20;++i)m.store(aeroAddress+coefficientOffsets[i],fields[i]);
    m.store(aeroAddress+0x130,p.velocityDamping130);
    m.write(aeroAddress+0xd0,in.triggerD0);m.write(aeroAddress+0xd8,in.triggerD8);
    m.write(aeroAddress+0xe0,in.fullSteeringE0);m.write(aeroAddress+0xdc,s.lowSpeedDc);
    m.store(aeroAddress+0xe4,in.driveE4);m.store(aeroAddress+0xe8,in.driveE8);m.store(aeroAddress+0xec,in.brakeEc);
    m.store(aeroAddress+0x140,s.timer140);m.store(aeroAddress+0x144,s.timer144);
    for(unsigned i=0;i<3;++i)m.store(sceneAddress+0x100+i*16,basis.columns[i]);
    m.write(sceneAddress+0x190,1);m.g[28][0]=0x96c0;
    m.write(0x1c00,1);m.write(0x1c04,1);m.write(0x1c08,1); // Original statics initialized.
}
static AeroResult48E getAero(const Original48E& m) {
    // Original producer locals remain in its returned stack frame (SP-0x130).
    return {{m.readFloat(aeroAddress+0x140),m.readFloat(aeroAddress+0x144),m.read(aeroAddress+0xdc)!=0},
        m.vec(0x1800-0x130+0x90),m.vec(0x1800-0x130+0xa0),
        {m.vec(aeroBody+0x180),m.vec(aeroBody+0x190)}};
}

int main() {
    std::uint64_t checks=0;std::array<bool,5> coverage{};
    auto require=[&](bool ok,const char* why){++checks;if(!ok)throw std::runtime_error(why);};
    auto equal=[&](const void* actual,const void* expected,unsigned count,const char* why) {
        const auto* a=static_cast<const unsigned char*>(actual);const auto* b=static_cast<const unsigned char*>(expected);
        for(unsigned i=0;i<count;++i) {
            std::uint32_t x,y;std::memcpy(&x,a+i*4,4);std::memcpy(&y,b+i*4,4);++checks;
            if(x!=y)throw std::runtime_error(std::string(why)+" float "+std::to_string(i)+
                " got "+std::to_string(x)+" expected "+std::to_string(y));
        }
    };
    auto equalAero=[&](const AeroResult48E& a,const AeroResult48E& b) {
        equal(&a.state.timer140,&b.state.timer140,2,"timers");require(a.state.lowSpeedDc==b.state.lowSpeedDc,"low speed flag");
        equal(&a.localForce,&b.localForce,3,"local aero force");equal(&a.directTorque,&b.directTorque,3,"direct aero torque");
        equal(&a.accumulatedLoads,&b.accumulatedLoads,6,"aero accumulated loads");
    };
    auto compare=[&](const AeroState48E& state,const AeroParameters48E& p,const AeroInput48E& in,
                     const ReadyBodyBasis48D& basis,const BodyLoads48C& loads,float dt) {
        Original48E m;putAero(m,state,p,in,basis,loads);callOriginal(m,rr_aero_48e_start,aeroAddress,dt);
        auto expected=getAero(m);AeroResult48E actual{};
        require(applyAerodynamics48E(state,p,in,basis,loads,dt,actual),"finite aero rejected");equalAero(actual,expected);
        for(unsigned i=0;i<5;++i)coverage[i]=coverage[i]||m.modeCoverage[i];return actual;
    };
    auto connected=[&](const BodyState48C& body,const BodyParameters48C& bp,const BodyLoads48C& prior,
                       const EngineState48D& engine,const EngineParameters48D& ep,const EngineInput48D& ei,
                       const AeroState48E& aero,const AeroParameters48E& ap,const AeroInput48E& ai,
                       const ReadyBodyBasis48D& basis,GravityParameters48D gravity,bool apply,float dt) {
        Original48E m;m.set(body,bp,prior);putEngine(m,engine,ep,ei);putForceContext(m,basis,gravity,true);
        callOriginal(m,rr_engine_update_48d_start,engineAddress,dt);auto expectedEngine=getEngine(m);
        callOriginal(m,rr_gravity_48d_start,gravityAddress,dt);if(apply)callOriginal(m,rr_engine_apply_48d_start,engineAddress,dt);
        putAero(m,aero,ap,ai,basis,{m.vec(0x180),m.vec(0x190)});callOriginal(m,rr_aero_48e_start,aeroAddress,dt);
        auto expectedAero=getAero(m);m.store(0x180,expectedAero.accumulatedLoads.force);m.store(0x190,expectedAero.accumulatedLoads.torque);
        m.steps=0;m.run(dt);const auto expectedBody=m.state();VehiclePartialStep48E actual{};
        require(stepEngineAeroBody48E(body,bp,prior,engine,ep,ei,aero,ap,ai,basis,gravity,apply,dt,actual),"connected aero step rejected");
        equal(&actual.engine,&expectedEngine,14,"connected engine");equalAero(actual.aero,expectedAero);
        equal(&actual.body,&expectedBody,36,"connected body");return actual;
    };
    std::uint32_t rng=0x48e16cd9u;
    auto next=[&](){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;};
    auto random=[&](float div){return static_cast<float>(static_cast<int>(next()&65535)-32768)/div;};
    try {
        for(int rounding:{FE_TONEAREST,FE_TOWARDZERO,FE_UPWARD,FE_DOWNWARD}) {
            require(std::fesetround(rounding)==0,"rounding setup");
            for(unsigned n=0;n<2500;++n) {
                auto p=savedParameters();AeroInput48E in{};auto basis=identityBasis();BodyLoads48C loads{};
                for(unsigned i=0;i<3;++i) {
                    in.bodyLocalVelocity.v[i]=random(n%2 ? 64.0f:2048.0f);in.angularVelocity.v[i]=random(65536);
                    in.worldFlow40.v[i]=random(512);loads.force.v[i]=random(16);loads.torque.v[i]=random(8192);
                    p.velocityDamping130.v[i]=random(1024);
                    for(unsigned j=0;j<3;++j)basis.columns[i].v[j]=random(32768);
                }
                p.lateral90=random(128);p.tiltedLateral118=random(128);p.timerLateral10c=random(128);
                p.timerYaw110=random(128);p.timerDrag114=random(128);p.steeringRamp104=random(65536);
                p.tiltedSteering11c=random(16);p.timerSteering108=random(16);
                in.bodyAngleE0=random(32768);in.subtractWorldFlow=(next()&1)!=0;
                in.triggerD0=n%7==0;in.triggerD8=n%5==0;in.fullSteeringE0=(next()&1)!=0;
                in.driveE4=random(32768);in.driveE8=random(32768);in.brakeEc=random(32768);
                AeroState48E state{n%3==0 ? random(8192):-1,n%3==1 ? random(8192):-1,(next()&1)!=0};
                const float dt=static_cast<float>(next()%1024)/32768.0f;
                compare(state,p,in,basis,loads,dt);
                if(n%4==0) {
                    BodyState48C body{};body.orientation.v[0]=1;body.velocity=in.bodyLocalVelocity;body.angularVelocity=in.angularVelocity;
                    EngineInput48D ei{{in.driveE4,in.driveE8},static_cast<float>(next()%1100),(next()&1)!=0,(next()&1)!=0,(next()&1)!=0};
                    connected(body,bodyParameters(),loads,{},engineParameters(),ei,state,p,in,basis,{32,.1f,-.2f},n%5!=0,dt);
                }
            }
            // Exact preserved runtime aero coefficients and inputs. elapsedFactor
            // is a supplied test value, not an observed snapshot-to-snapshot tick.
            AeroInput48E saved{};AeroState48E state{};auto basis=identityBasis();
            for(unsigned i=0;i<3;++i) {
                saved.bodyLocalVelocity.v[i]=fp(rr_saved_body_local_48e[i]);
                saved.angularVelocity.v[i]=fp(rr_stopped_body_48c[0xb0/4+i]);
                saved.worldFlow40.v[i]=fp(rr_saved_aero_48e[0x40/4+i]);
                for(unsigned j=0;j<3;++j)basis.columns[i].v[j]=fp(rr_saved_scene_basis_48d[i*4+j]);
            }
            saved.bodyAngleE0=fp(rr_saved_body_angle_48e[0]);saved.subtractWorldFlow=rr_saved_aero_48e[0x34/4]!=0;
            saved.triggerD0=rr_saved_aero_48e[0xd0/4]!=0;saved.triggerD8=rr_saved_aero_48e[0xd8/4]!=0;
            saved.fullSteeringE0=rr_saved_aero_48e[0xe0/4]!=0;
            saved.driveE4=fp(rr_saved_aero_48e[0xe4/4]);saved.driveE8=fp(rr_saved_aero_48e[0xe8/4]);saved.brakeEc=fp(rr_saved_aero_48e[0xec/4]);
            state={fp(rr_saved_aero_48e[0x140/4]),fp(rr_saved_aero_48e[0x144/4]),rr_saved_aero_48e[0xdc/4]!=0};
            compare(state,savedParameters(),saved,basis,{},1.0f/120);
            // Repeated updates exercise persistent timer countdown/retrigger and
            // changing drive. Local velocity/basis remain explicitly supplied.
            for(unsigned i=0;i<320;++i) {
                saved.triggerD0=i%83==0;saved.triggerD8=i%37==0;saved.driveE4=i%5*.25f;
                saved.bodyLocalVelocity.v[2]=static_cast<float>(i%80);
                auto result=compare(state,savedParameters(),saved,basis,{},i%31==0 ? 0:1.0f/120);state=result.state;
            }
            for(unsigned i=0;i<1000;++i) {
                Vector3_48C velocity{{random(64),random(64),random(64)}};
                if(i<4)velocity={{i==0 ? 0:fp(i),-0.0f,0}};
                Original48E m;m.store(0x70,velocity);m.g[16][0]=0;m.g[29][0]=0x1800;m.f[3]=0;
                auto pc=rr_cached_speed_48e_start;
                while(pc!=0x167c1c) {
                    std::uint32_t target=0,unused=0;
                    if(m.execute(pc,target)){if(m.execute(pc+4,unused))throw std::runtime_error("speed delayed branch");pc=target;}
                    else pc+=4;
                }
                float actual=-1,expected=m.readFloat(0x34);require(refreshCachedSpeed48E(velocity,actual),"speed rejected");equal(&actual,&expected,1,"RSQRT speed");
            }
        }
        require(std::fesetround(FE_TONEAREST)==0,"rounding restore");
        for(bool covered:coverage)require(covered,"original aerodynamic mode branch missed");
        auto p=savedParameters();AeroInput48E in{};in.fullSteeringE0=true;in.driveE4=1;in.driveE8=0;
        auto basis=identityBasis();
        // Boundaries distinguish the original strict speed/forward/up compares.
        for(float z:{-50.0f,-0.0f,0.0f,20.0f,std::nextafter(20.0f,21.0f),49.999996f,50.0f,50.000004f,100.0f})
            for(float y:{-150.0f,-20.0f,0.0f,20.0f,150.0f}) {
                in.bodyLocalVelocity={{0,y,z}};compare({},p,in,basis,{},.01f);
            }
        in.bodyLocalVelocity={{0,0,100}};
        for(float up:{-.01f,-1.0f,0.0f,.5f,std::nextafter(.5f,1.0f),1.0f}) {
            basis.columns[1].v[1]=up;compare({},p,in,basis,{},.01f);
        }
        basis=identityBasis();in.triggerD0=true;in.triggerD8=true;
        auto both=compare({},p,in,basis,{},.5f);
        require(both.state.timer140==-.25f && both.state.timer144==0,"timer priority/overshoot changed");
        in.triggerD0=false;auto later=compare(both.state,p,in,basis,{},.5f);
        require(later.state.timer144==3.5f,"second trigger after first expired");
        in.triggerD8=false;in.bodyLocalVelocity={{0,0,50}};auto threshold=compare({},p,in,basis,{},0);
        require(!threshold.state.lowSpeedDc,"speed 50 must take high-speed path");
        const float bad=std::numeric_limits<float>::quiet_NaN();auto invalid=in;invalid.driveE4=bad;
        AeroResult48E sentry{};sentry.state.timer140=123;auto output=sentry;
        require(!applyAerodynamics48E({},p,invalid,basis,{},.01f,output),"NaN accepted");require(std::memcmp(&output,&sentry,sizeof output)==0,"invalid mutated output");
        invalid=in;invalid.bodyLocalVelocity.v[0]=std::numeric_limits<float>::max();
        require(!applyAerodynamics48E({},p,invalid,basis,{},.01f,output),"overflow accepted");require(std::memcmp(&output,&sentry,sizeof output)==0,"overflow mutated output");
        require(!applyAerodynamics48E({},p,in,basis,{},-.01f,output),"negative dt accepted");
        float speed=123;require(!refreshCachedSpeed48E({{bad,0,0}},speed)&&speed==123,"invalid speed mutation");
        auto expected=compare({},p,in,basis,{},.01f);AeroResult48E alias{};
        require(applyAerodynamics48E(alias.state,p,in,basis,alias.accumulatedLoads,.01f,alias),"alias rejected");equalAero(alias,expected);
        VehiclePartialStep48E combined{};combined.body.position.v[0]=123;auto oldCombined=combined;
        auto bp=bodyParameters();bp.linearAccelerationLimitSquared=-1;
        require(!stepEngineAeroBody48E({},bp,{}, {},engineParameters(),{{1,1},0,false,false,true},{},p,in,basis,{},true,.01f,combined),"late body rejection missing");
        require(std::memcmp(&combined,&oldCombined,sizeof combined)==0,"late failure mutated composed output");
        std::cout<<"physics48e: "<<checks<<" checks passed; complete ready-scene aerodynamic producer; 5 original mode branches; original angle/transform/force words; 4 rounding modes; 2500 connected partial steps; separate cached-speed writer; no full-vehicle claim\n";
        return 0;
    }catch(const std::exception& e){std::fesetround(FE_TONEAREST);std::cerr<<"physics48e after "<<checks<<" checks: "<<e.what()<<'\n';return 1;}
}
