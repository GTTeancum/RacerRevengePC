#include <string>
#include "../../src/game/vehicle_forces48d.h"
#include "../../tools/gameplay/force_oracle48d.h"

static constexpr std::uint32_t engineAddress=0x400, gravityAddress=0x800, sceneAddress=0x1000;

static void callOriginal(Original48D& m,std::uint32_t entry,std::uint32_t object,float dt) {
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
static void putEngine(Original48D& m,const EngineState48D& s,const EngineParameters48D& p,const EngineInput48D& in) {
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
static EngineState48D getEngine(const Original48D& m) {
    return {{m.readFloat(engineAddress+0x70),m.readFloat(engineAddress+0x74)},
            {m.vec(engineAddress+0xa0),m.vec(engineAddress+0xb0)},
            {m.vec(engineAddress+0xc0),m.vec(engineAddress+0xd0)}};
}
static void putForceContext(Original48D& m,const ReadyBodyBasis48D& basis,GravityParameters48D gravity,bool hasScene) {
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

int main() {
    std::uint64_t checks=0;
    auto require=[&](bool pass,const char* why){++checks;if(!pass)throw std::runtime_error(why);};
    auto equalFloats=[&](const void* actual,const void* expected,unsigned count,const char* context) {
        const auto* a=static_cast<const unsigned char*>(actual);const auto* b=static_cast<const unsigned char*>(expected);
        for(unsigned i=0;i<count;++i) {
            std::uint32_t x,y;std::memcpy(&x,a+i*4,4);std::memcpy(&y,b+i*4,4);++checks;
            if(x!=y)throw std::runtime_error(std::string(context)+" float "+std::to_string(i)+
                " got "+std::to_string(x)+" expected "+std::to_string(y));
        }
    };
    auto compare=[&](const BodyState48C& body,const BodyParameters48C& bp,const BodyLoads48C& loads,
                     const EngineState48D& engine,const EngineParameters48D& ep,const EngineInput48D& in,
                     const ReadyBodyBasis48D& basis,GravityParameters48D gravity,bool apply,float dt) {
        Original48D m;m.set(body,bp,loads);putEngine(m,engine,ep,in);putForceContext(m,basis,gravity,true);
        callOriginal(m,rr_engine_update_48d_start,engineAddress,dt);
        const auto expectedEngine=getEngine(m);
        callOriginal(m,rr_gravity_48d_start,gravityAddress,dt);
        if(apply)callOriginal(m,rr_engine_apply_48d_start,engineAddress,dt);
        const BodyLoads48C expectedLoads{m.vec(0x180),m.vec(0x190)};
        m.steps=0;m.run(dt);
        const EngineGravityStep48D expected{expectedEngine,expectedLoads,m.state()};
        EngineGravityStep48D actual{};
        require(stepEngineGravityBody48D(body,bp,loads,engine,ep,in,basis,gravity,apply,dt,actual),"finite connected step rejected");
        static_assert(sizeof(EngineGravityStep48D)==56*sizeof(float));
        equalFloats(&actual,&expected,56,"original connected instruction sequence");
        return actual;
    };
    auto compareEngine=[&](const EngineState48D& s,const EngineParameters48D& p,const EngineInput48D& input,float arbitraryF12) {
        Original48D m;putEngine(m,s,p,input);callOriginal(m,rr_engine_update_48d_start,engineAddress,arbitraryF12);
        const auto expected=getEngine(m);EngineState48D actual{};
        require(updateEngine48D(s,p,input,actual),"finite engine rejected");
        static_assert(sizeof(EngineState48D)==14*sizeof(float));
        equalFloats(&actual,&expected,14,"engine producer");return actual;
    };
    std::uint32_t rng=0x48d2e772u;
    auto next=[&](){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;};
    auto randFloat=[&](float divisor){return static_cast<float>(static_cast<int>(next()&65535)-32768)/divisor;};
    try {
        for(int rounding:{FE_TONEAREST,FE_TOWARDZERO,FE_UPWARD,FE_DOWNWARD}) {
            require(std::fesetround(rounding)==0,"rounding setup");
            for(unsigned n=0;n<1600;++n) {
                auto ep=engineParameters();auto bp=bodyParameters();BodyState48C body{};
                BodyLoads48C loads{};EngineState48D engine{};ReadyBodyBasis48D basis{};
                EngineInput48D input{{randFloat(32768),randFloat(32768)},static_cast<float>(next()%1500),
                                     (next()&1)!=0,(next()&1)!=0,(next()&1)!=0};
                ep.maxThrust=static_cast<float>(next()%10000);ep.boostThrust=static_cast<float>(next()%3000);
                ep.repairThrust=static_cast<float>(next()%3000);ep.rampUpFilter=randFloat(32768);
                ep.rampDownFilter=randFloat(32768);ep.theoreticalTopSpeed=static_cast<float>(next()%1100);
                ep.rampDownSpeed=n%9==0 ? 0:static_cast<float>(next()%300);
                ep.boostSpeed=static_cast<float>(next()%400);
                ep.repairSpeed=n%11==0 ? ep.rampDownSpeed:static_cast<float>(next()%200);
                bp.mass=static_cast<float>(1+next()%50);bp.inverseMass=1.0f/bp.mass;
                for(unsigned i=0;i<2;++i)engine.smoothedDrive[i]=randFloat(32768);
                for(unsigned i=0;i<3;++i) {
                    body.position.v[i]=randFloat(4);body.velocity.v[i]=randFloat(512);
                    body.angularVelocity.v[i]=randFloat(65536);
                    body.previousPositionDerivative.v[i]=randFloat(512);
                    body.previousLinearAcceleration.v[i]=randFloat(128);
                    body.previousAngularAcceleration.v[i]=randFloat(8192);
                    loads.force.v[i]=randFloat(256);loads.torque.v[i]=randFloat(4096);
                    ep.torqueBasis[0].v[i]=randFloat(8192);ep.torqueBasis[1].v[i]=randFloat(8192);
                    for(unsigned j=0;j<3;++j)basis.columns[i].v[j]=randFloat(32768);
                }
                for(unsigned i=0;i<4;++i) {
                    body.orientation.v[i]=randFloat(32768);body.previousOrientationDerivative.v[i]=randFloat(65536);
                }
                compare(body,bp,loads,engine,ep,input,basis,{randFloat(128),randFloat(4096),randFloat(4096)},
                        n%5!=0,static_cast<float>(next()%512)/32768.0f);
            }
            // Real stopped runtime fields form the entire partial-step fixture.
            Original48D saved;
            for(unsigned i=0;i<sizeof(rr_stopped_body_48c)/4;++i)saved.memory[i]=rr_stopped_body_48c[i];
            for(unsigned i=0;i<sizeof(rr_saved_engine_48d)/4;++i)saved.memory[engineAddress/4+i]=rr_saved_engine_48d[i];
            auto ep=engineParameters();
            float* scalarFields[]={&ep.maxThrust,&ep.boostThrust,&ep.repairThrust,&ep.rampUpFilter,&ep.rampDownFilter,
                                   &ep.rampDownSpeed,&ep.theoreticalTopSpeed,&ep.boostSpeed,&ep.repairSpeed};
            const unsigned offsets[]={0x40,0x44,0x48,0x4c,0x50,0x54,0x58,0x60,0x64};
            for(unsigned i=0;i<9;++i)*scalarFields[i]=saved.readFloat(engineAddress+offsets[i]);
            for(unsigned i=0;i<2;++i)ep.torqueBasis[i]=saved.vec(engineAddress+0x80+i*16);
            const EngineInput48D input{{saved.readFloat(engineAddress+0x68),saved.readFloat(engineAddress+0x6c)},
                                       fp(rr_saved_body_speed_48d[0]),saved.read(engineAddress+0x34)!=0,
                                       saved.read(engineAddress+0x38)!=0,saved.read(engineAddress+0x3c)!=0};
            auto bp=bodyParameters();bp.mass=saved.readFloat(0x26c);bp.inverseMass=saved.readFloat(0x270);
            bp.linearAccelerationLimitSquared=saved.readFloat(0x274);bp.angularAccelerationLimitSquared=saved.readFloat(0x278);
            bp.linearVelocityLimitSquared=saved.readFloat(0x27c);bp.angularVelocityLimitSquared=saved.readFloat(0x280);
            ReadyBodyBasis48D basis{};
            for(unsigned i=0;i<3;++i) {
                bp.inverseInertiaColumns[i]=saved.vec(0x140+i*16);
                bp.translationEnabled[i]=saved.read(0x2a0+i*4)!=0;bp.rotationEnabled[i]=saved.read(0x2ac+i*4)!=0;
                for(unsigned j=0;j<3;++j)basis.columns[i].v[j]=fp(rr_saved_scene_basis_48d[i*4+j]);
            }
            const GravityParameters48D gravity{fp(rr_saved_gravity_48d[0x38/4]),fp(rr_saved_gravity_48d[0x40/4]),fp(rr_saved_gravity_48d[0x48/4])};
            compare(saved.state(),bp,{saved.vec(0x180),saved.vec(0x190)},getEngine(saved),ep,input,basis,gravity,true,1.0f/120.0f);

            // Controlled repeated partial steps: changing established model drive
            // channels, fixed basis and supplied speed. This is not a race replay.
            auto body=saved.state();auto engine=getEngine(saved);auto varying=input;
            for(unsigned i=0;i<256;++i) {
                varying.drive[0]=i%64<32 ? .75f:0;varying.drive[1]=i%64<32 ? 1.0f:0;
                varying.boost=i%47<8;varying.repair=i%73<5;
                auto result=compare(body,bp,{},engine,ep,varying,basis,gravity,true,i%19==0 ? 0:1.0f/120.0f);
                body=result.body;engine=result.engine;
            }
        }
        require(std::fesetround(FE_TONEAREST)==0,"rounding restore");
        auto ep=engineParameters();EngineState48D engine{};EngineInput48D input{{1,.5f},0,false,false,true};
        ep.rampUpFilter=.25f;ep.rampDownFilter=.5f;
        const auto a=compareEngine(engine,ep,input,0);const auto b=compareEngine(engine,ep,input,1000);
        equalFloats(&a,&b,14,"engine must ignore f12");require(a.smoothedDrive[0]==.25f,"ramp-up blend");
        engine=a;input.drive[0]=0;auto down=compareEngine(engine,ep,input,.01f);
        require(down.smoothedDrive[0]==.125f,"ramp-down blend");
        ep=engineParameters();input={{1,1},0,true,true,true};
        auto boost=compareEngine({},ep,input,0);input.repair=false;auto boostOnly=compareEngine({},ep,input,0);
        equalFloats(&boost,&boostOnly,14,"boost branch must precede repair");
        ep.rampDownSpeed=0;input.boost=false;auto noSpan=compareEngine({},ep,input,0);
        require(noSpan.localThrust[0].v[2]==0 && noSpan.torque[0].v[0]==0,"zero speed span fallback");
        ep=engineParameters();input={{1,.5f},0,false,false,false};
        auto half=compareEngine({},ep,input,0);input.fullTorque=true;auto full=compareEngine({},ep,input,0);
        require(half.torque[0].v[0]==mul(full.torque[0].v[0],.5f),"half-torque branch");
        require(half.localThrust[0].v[2]==full.localThrust[0].v[2],"half torque must not halve thrust");
        // Apply operation includes absent-scene branch: thrust skipped, torque
        // still accumulates; asymmetric basis must never rotate torque.
        for(bool hasScene:{false,true})for(bool apply:{false,true}) {
            auto basis=identityBasis();basis.columns[0]={{0,1,0}};basis.columns[1]={{-1,0,0}};
            const BodyLoads48C prior{{{3,4,5}},{{6,7,8}}};const GravityParameters48D gravity{2,.1f,-.2f};
            Original48D m;auto bp=bodyParameters();m.set({},bp,prior);putEngine(m,full,ep,input);
            putForceContext(m,basis,gravity,hasScene);callOriginal(m,rr_gravity_48d_start,gravityAddress,0);
            if(apply)callOriginal(m,rr_engine_apply_48d_start,engineAddress,0);
            const BodyLoads48C expected{m.vec(0x180),m.vec(0x190)};BodyLoads48C actual{};
            require(accumulateEngineGravity48D(prior,full,basis,gravity,1,apply,hasScene,actual),"load application rejected");
            equalFloats(&actual,&expected,6,"application and gravity");
        }
        for(float tiny:{0.0f,-0.0f,fp(1),fp(0x80000001),1e-20f}) {
            input={{tiny,-tiny},tiny,false,false,true};ep=engineParameters();ep.rampUpFilter=tiny;
            compareEngine({},ep,input,tiny);
        }
        const float bad=std::numeric_limits<float>::quiet_NaN();
        EngineGravityStep48D sentry{};sentry.body.position.v[0]=123;
        auto out=sentry;auto body=BodyState48C{};body.orientation.v[0]=1;
        input={{1,1},0,false,false,true};ep=engineParameters();ep.maxThrust=bad;
        require(!stepEngineGravityBody48D(body,bodyParameters(),{},engine,ep,input,identityBasis(),{},true,.01f,out),"NaN engine accepted");
        require(std::memcmp(&sentry,&out,sizeof out)==0,"bad producer mutated output");
        ep=engineParameters();auto bp=bodyParameters();bp.angularVelocityLimitSquared=-1;
        require(!stepEngineGravityBody48D(body,bp,{},engine,ep,input,identityBasis(),{},true,.01f,out),"bad body accepted");
        require(std::memcmp(&sentry,&out,sizeof out)==0,"late body failure mutated composed output");
        ep.maxThrust=std::numeric_limits<float>::max();input.drive[0]=2;
        EngineState48D engineSentry{};engineSentry.smoothedDrive[0]=123;auto engineOut=engineSentry;
        require(!updateEngine48D({},ep,input,engineOut),"overflow engine accepted");
        require(std::memcmp(&engineSentry,&engineOut,sizeof engineOut)==0,"overflow mutated engine");
        ep=engineParameters();input={{.4f,.7f},0,false,false,true};engine={};
        const auto expected=compareEngine(engine,ep,input,0);
        require(updateEngine48D(engine,ep,input,engine),"aliased engine failed");equalFloats(&engine,&expected,14,"aliased engine");
        std::cout<<"physics48d: "<<checks<<" checks passed; original engine/gravity/application + 48C body words; 4 rounding modes; saved connected snapshot and partial trajectories; no whole-vehicle claim\n";
        return 0;
    } catch(const std::exception& e) {
        std::fesetround(FE_TONEAREST);std::cerr<<"physics48d after "<<checks<<" checks: "<<e.what()<<'\n';return 1;
    }
}
