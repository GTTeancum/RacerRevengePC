#include "../../src/game/body_wrapper48h.h"
#include "../../tools/gameplay/body_wrapper_oracle48h.h"
#include <algorithm>

static constexpr std::uint32_t scene=0x1000;
static void put(OriginalWrapper48H&m,const RootBodyState48H&s,const BodyParameters48C&p,const Vector3_48C&offset,bool attached=true,bool enabled=true) {
    m.set(s.body,p,s.loads);m.store(0xc0,s.body.linearAcceleration);m.store(0xd0,s.body.angularAcceleration);m.store(0x2e8,s.body.orientationDerivative);
    m.store(0x90,s.previousVelocity90);m.store(0x80,s.localVelocity80);m.store(0xa0,s.worldAngularVelocityA0);
    m.store(0x1a0,s.positionCorrection1a0);m.store(0x1b0,s.rotationCorrection1b0);m.store(0x1c0,s.accumulator1c0);m.store(0x1d0,s.accumulator1d0);
    m.store(0x1e0,offset);m.write(0x238,s.callbackState238);m.write(0x288,attached?scene:0);m.write(0x204,enabled?1:0);
    m.write(8,0x380);m.write(0x380+0x9c,0x1654b0);
    for(unsigned i=0;i<16;++i){m.store(scene+0xc0+i*4,s.scene.v[i]);m.store(scene+0x100+i*4,s.scene.v[i]);}
    // Actual saved body has no component children and root scene has no parent
    // or spatial observer. Only the render child notification graph is omitted.
    m.write(scene+0x190,1);m.g[28][0]=0x9600;
    m.write(0x1c40,1);m.write(0x1c44,1);m.write(0x1c48,1);m.write(0x222c,1);
}
static void run(OriginalWrapper48H&m,float dt) {
    m.steps=0;m.g[4][0]=0;m.g[29][0]=0x3000;m.g[31][0]=0xf00df00d;m.f[12]=dt;m.v[0][3]=1;
    auto pc=rr_body_wrapper_48h_start;
    while(pc!=0xf00df00d){std::uint32_t target=0,unused=0;
        if(m.execute(pc,target)){if(!m.annulDelay&&m.execute(pc+4,unused))throw std::runtime_error("branch delay transfer");pc=target;}else pc+=4;
    }
    if(m.g[29][0]!=0x3000||m.g[2][0]!=1)throw std::runtime_error("wrapper return failed");
}
static RootBodyState48H get(const OriginalWrapper48H&m) {
    RootBodyState48H s{};s.body=m.state();for(unsigned i=0;i<16;++i)s.scene.v[i]=m.readFloat(scene+0xc0+i*4);
    s.previousVelocity90=m.vec(0x90);s.localVelocity80=m.vec(0x80);s.worldAngularVelocityA0=m.vec(0xa0);
    s.loads={m.vec(0x180),m.vec(0x190)};s.positionCorrection1a0=m.vec(0x1a0);s.rotationCorrection1b0=m.vec(0x1b0);
    s.accumulator1c0=m.vec(0x1c0);s.accumulator1d0=m.vec(0x1d0);s.callbackState238=m.read(0x238);return s;
}
static BodyParameters48C parameters() {return {1,1,{{{1,0,0}},{{0,1,0}},{{0,0,1}}},1e10f,1e10f,1e10f,1e10f,{true,true,true},{true,true,true}};}
int main(){
    std::uint64_t checks=0;unsigned cases=0;std::array<bool,8>coverage{};
    auto require=[&](bool ok,const char*why){++checks;if(!ok)throw std::runtime_error(why);};
    auto equal=[&](const RootBodyState48H&a,const RootBodyState48H&b){
        static_assert(sizeof(a)==80*4);std::array<std::uint32_t,80>x{},y{};std::memcpy(x.data(),&a,sizeof(a));std::memcpy(y.data(),&b,sizeof(b));
        for(unsigned i=0;i<x.size();++i){++checks;if(x[i]!=y[i])throw std::runtime_error("wrapper mismatch case="+std::to_string(cases)+" lane="+std::to_string(i)+" got="+std::to_string(x[i])+" expected="+std::to_string(y[i]));}
    };
    auto compare=[&](const RootBodyState48H&s,const BodyParameters48C&p,const Vector3_48C&o,float dt,bool attached=true,bool enabled=true){
        ++cases;OriginalWrapper48H m;put(m,s,p,o,attached,enabled);run(m,dt);const auto expected=get(m);RootBodyState48H actual{};
        require(updateRootBody48H(s,p,o,attached,enabled,dt,actual),"native wrapper rejected");equal(actual,expected);
        auto aliased=s;require(updateRootBody48H(aliased,p,o,attached,enabled,dt,aliased),"alias rejected");equal(aliased,expected);
        for(unsigned i=0;i<coverage.size();++i)coverage[i]=coverage[i]||m.wrapperCoverage[i];return actual;
    };
    std::uint32_t rng=0x48f0167c;auto next=[&](){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;};
    auto random=[&](float divisor){return static_cast<float>(static_cast<int>(next()&65535)-32768)/divisor;};
    try {
        OriginalWrapper48H saved;for(unsigned i=0;i<std::size(rr_saved_body_48h);++i)saved.write(i*4,rr_saved_body_48h[i]);
        for(unsigned i=0;i<std::size(rr_saved_scene_48h);++i)saved.write(scene+i*4,rr_saved_scene_48h[i]);
        auto s=get(saved);auto p=parameters();p.mass=saved.readFloat(0x26c);p.inverseMass=saved.readFloat(0x270);
        p.linearAccelerationLimitSquared=saved.readFloat(0x274);p.angularAccelerationLimitSquared=saved.readFloat(0x278);p.linearVelocityLimitSquared=saved.readFloat(0x27c);p.angularVelocityLimitSquared=saved.readFloat(0x280);
        for(unsigned i=0;i<3;++i)p.inverseInertiaColumns[i]=saved.vec(0x140+i*16);
        auto center=saved.vec(0x1e0);compare(s,p,center,1.f/60);
        for(int rounding:{FE_TONEAREST,FE_TOWARDZERO,FE_UPWARD,FE_DOWNWARD}){
            require(std::fesetround(rounding)==0,"round mode");
            for(unsigned n=0;n<500;++n){
                RootBodyState48H r{};p=parameters();Quaternion48C q{{random(32768),random(32768),random(32768),random(32768)}};
                require(quaternionToMatrix48G(q,r.scene),"fixture matrix");
                for(unsigned i=0;i<3;++i){r.scene.v[12+i]=random(4);r.body.velocity.v[i]=random(512);r.body.angularVelocity.v[i]=random(32768);
                    r.body.previousPositionDerivative.v[i]=random(512);r.body.previousLinearAcceleration.v[i]=random(8192);r.body.previousAngularAcceleration.v[i]=random(32768);
                    r.loads.force.v[i]=random(1024);r.loads.torque.v[i]=random(32768);r.positionCorrection1a0.v[i]=random(32768);
                    if(n%3)r.rotationCorrection1b0.v[i]=random(32768);center.v[i]=random(1024);
                    r.previousVelocity90.v[i]=random(128);r.localVelocity80.v[i]=random(128);r.worldAngularVelocityA0.v[i]=random(128);
                    r.accumulator1c0.v[i]=random(128);r.accumulator1d0.v[i]=random(128);
                    p.translationEnabled[i]=(next()%5)!=0;p.rotationEnabled[i]=(next()%5)!=0;
                }
                for(float&x:r.body.previousOrientationDerivative.v)x=random(32768);
                r.callbackState238=next();if(n%5==0)p.mass=0;
                if(n%7==0){p.linearAccelerationLimitSquared=1;p.angularAccelerationLimitSquared=.05f;p.linearVelocityLimitSquared=4;p.angularVelocityLimitSquared=.25f;}
                compare(r,p,center,n%4==0?0.f:1.f/60);if(n<4){compare(r,p,center,.02f,false,true);compare(r,p,center,.02f,true,false);}
            }
        }
        std::fesetround(FE_TONEAREST);p=parameters();s={};s.scene.v[0]=s.scene.v[5]=s.scene.v[10]=s.scene.v[15]=1;
        center={{0,-8,0}};s.rotationCorrection1b0={{1e-25f,0,0}};compare(s,p,center,0);
        // Keep the original scene cache, histories and register state between
        // calls. Only external force/torque inputs are refreshed, never repaired
        // from native output after the initial seed.
        OriginalWrapper48H chain;put(chain,s,p,center);
        for(unsigned n=0;n<180;++n){
            s.loads.force={{4,-1,2}};s.loads.torque={{.01f,.03f,-.02f}};
            chain.store(0x180,s.loads.force);chain.store(0x190,s.loads.torque);
            run(chain,1.f/60);++cases;RootBodyState48H nextState{};
            require(updateRootBody48H(s,p,center,true,true,1.f/60,nextState),"chained wrapper rejected");
            equal(nextState,get(chain));s=nextState;
        }
        for(float correction:{-4.f,-2.f,-.5f,0.f,.5f,2.f,4.f}){
            auto r=s;r.scene.v[0]*=.7f;r.scene.v[5]*=1.2f;r.rotationCorrection1b0={{correction,.2f,-.1f}};compare(r,p,center,.01f);
        }
        for(bool b:coverage)require(b,"wrapper branch missing");
        auto invalid=s;invalid.rotationCorrection1b0.v[0]=std::numeric_limits<float>::infinity();auto out=s;
        require(!updateRootBody48H(invalid,p,center,true,true,.01f,out),"nonfinite accepted");equal(out,s);
        invalid=s;invalid.loads.force.v[0]=std::numeric_limits<float>::max();p.inverseMass=std::numeric_limits<float>::max();
        require(!updateRootBody48H(invalid,p,center,true,true,.01f,out),"overflow accepted");equal(out,s);
        std::cout<<checks<<" root body wrapper checks passed; "<<cases<<" original-word calls; 8 branch gates; four rounding modes\n";return 0;
    }catch(const std::exception&e){std::fesetround(FE_TONEAREST);std::cerr<<"FAIL after "<<checks<<" checks: "<<e.what()<<"\n";return 1;}
}
