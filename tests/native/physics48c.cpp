// Test-only execution of the exact original instruction words, with bounded
// memory and only the encountered instruction subset. No game VM in production.
#include "../../src/game/body_integration48c.h"
#include "../../tools/gameplay/physics_words48c.h"
#include "../../work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

using namespace rr::game;
static std::uint32_t bits(float f) { std::uint32_t b; std::memcpy(&b,&f,4); return b; }
static float fp(std::uint32_t b) { float f; std::memcpy(&f,&b,4); return f; }
static float add(float a,float b) { volatile float r=a+b; return r; }
static float sub(float a,float b) { volatile float r=a-b; return r; }
static float mul(float a,float b) { volatile float r=a*b; return r; }
static float divide(float a,float b) { volatile float r=a/b; return r; }

struct Original {
    std::array<std::array<std::uint32_t,4>,32> g{};
    std::array<float,32> f{};
    std::array<std::array<float,4>,32> v{};
    std::array<float,4> vacc{};
    std::array<std::uint32_t,0x2000/4> memory{};
    float acc=0;
    bool condition=false;
    std::uint32_t flags=0;
    unsigned steps=0;
    std::uint32_t read(std::uint32_t a) const {
        if (a>=0x382b60 && a<0x382b70 && !(a&3)) return 0;
        if (a>=0x433b80 && a<0x433b90 && !(a&3)) return a==0x433b8c ? 0x3f800000u:0u;
        if ((a&3) || a/4>=memory.size()) throw std::runtime_error("oracle read outside fixture");
        return memory[a/4];
    }
    void write(std::uint32_t a,std::uint32_t b) {
        if ((a&3) || a/4>=memory.size()) throw std::runtime_error("oracle write outside fixture");
        memory[a/4]=b;
    }
    float readFloat(std::uint32_t a) const { return fp(read(a)); }
    void store(std::uint32_t a,float x) { write(a,bits(x)); }
    void store(std::uint32_t a,const Vector3_48C& x) { for(unsigned i=0;i<3;++i) store(a+i*4,x.v[i]); }
    void store(std::uint32_t a,const Quaternion48C& x) { for(unsigned i=0;i<4;++i) store(a+i*4,x.v[i]); }
    Vector3_48C vec(std::uint32_t a) const { return {{readFloat(a),readFloat(a+4),readFloat(a+8)}}; }
    Quaternion48C quat(std::uint32_t a) const { return {{readFloat(a),readFloat(a+4),readFloat(a+8),readFloat(a+12)}}; }
    std::uint32_t word(std::uint32_t pc) const {
#define BLOCK(name) if (pc>=rr_##name##_48c_start && pc-rr_##name##_48c_start<sizeof(rr_##name##_48c)) return rr_##name##_48c[(pc-rr_##name##_48c_start)/4]
        BLOCK(body); BLOCK(integrator); BLOCK(quaternion_derivative); BLOCK(quaternion_normalize);
#undef BLOCK
        throw std::runtime_error("oracle PC outside original blocks: "+std::to_string(pc));
    }
    // Return true for control transfer (including a not-taken delayed branch).
    bool execute(std::uint32_t pc,std::uint32_t& target) {
        if (++steps>2000) throw std::runtime_error("oracle instruction budget");
        const auto w=word(pc), op=w>>26, rs=(w>>21)&31u, rt=(w>>16)&31u, rd=(w>>11)&31u;
        const auto imm=static_cast<std::int16_t>(w&65535u);
        auto address=[&](){return g[rs][0]+imm;};
        auto branch=[&](bool take){target=take ? pc+4+static_cast<std::int32_t>(imm)*4:pc+8;return true;};
        if(w==0) return false;
        if(op==9) g[rt][0]=g[rs][0]+imm;
        else if(op==15) g[rt][0]=(w&65535u)<<16;
        else if(op==0x23) g[rt][0]=read(address());
        else if(op==0x2b) write(address(),g[rt][0]);
        else if(op==0x31) f[rt]=readFloat(address());
        else if(op==0x39) store(address(),f[rt]);
        else if(op==0x1e) for(unsigned i=0;i<4;++i) g[rt][i]=read(address()+i*4);
        else if(op==0x1f) for(unsigned i=0;i<4;++i) write(address()+i*4,g[rt][i]);
        else if(op==0x36) for(unsigned i=0;i<4;++i) v[rt][i]=readFloat(address()+i*4);
        else if(op==0x3e) for(unsigned i=0;i<4;++i) store(address()+i*4,v[rt][i]);
        else if(op==4) return branch(g[rs][0]==g[rt][0]);
        else if(op==5) return branch(g[rs][0]!=g[rt][0]);
        else if(op==3) {g[31][0]=pc+8;target=(w&0x3ffffffu)*4;return true;}
        else if(op==0) {
            switch(w&63u) {
                case 0: g[rd][0]=g[rt][0]<<((w>>6)&31u); break;
                case 0x21: g[rd][0]=g[rs][0]+g[rt][0]; break;
                case 0x2a: g[rd][0]=static_cast<std::int32_t>(g[rs][0])<static_cast<std::int32_t>(g[rt][0]); break;
                case 8: target=g[rs][0]; return true;
                case 9: target=g[rs][0];g[rd][0]=pc+8;return true;
                default: throw std::runtime_error("oracle special opcode");
            }
        } else if(w==0x70003e28u) g[7]={}; // PADDUB a3,zero,zero.
        else if(op==0x11) {
            if(rs==4) f[rd]=fp(g[rt][0]);
            else if(rs==8) return branch(condition==((rt&1u)!=0));
            else if(rs==16) {
                const auto fd=(w>>6)&31u, fn=w&63u;
                switch(fn) {
                    case 0: f[fd]=add(f[rd],f[rt]);break;
                    case 1: f[fd]=sub(f[rd],f[rt]);break;
                    case 2: f[fd]=mul(f[rd],f[rt]);break;
                    case 3: f[fd]=divide(f[rd],f[rt]);break;
                    case 4: f[fd]=Ps2EeSqrt14(f[rd],flags);break;
                    case 6: f[fd]=f[rd];break;
                    case 0x16: f[fd]=Ps2EeRsqrt14(f[rd],f[rt],flags);break;
                    case 0x18: acc=add(f[rd],f[rt]);break;
                    case 0x1a: acc=mul(f[rd],f[rt]);break;
                    case 0x1c: f[fd]=add(acc,mul(f[rd],f[rt]));break;
                    case 0x1d: f[fd]=sub(acc,mul(f[rd],f[rt]));break;
                    case 0x32: condition=f[rd]==f[rt];break;
                    case 0x36: condition=f[rd]<=f[rt];break;
                    default: throw std::runtime_error("oracle scalar FPU opcode");
                }
            } else throw std::runtime_error("oracle COP1 opcode");
        } else if(op==0x12) {
            // Exact VU opcodes encountered in the original body, with individual
            // binary32 operations and no application-level integration formula.
            switch(w) {
                case 0x48222800: for(unsigned i=0;i<4;++i)g[2][i]=bits(v[5][i]);break;
                case 0x4be5233c: v[5]=v[4];break;
                case 0x4bc5216a: for(unsigned i=0;i<3;++i)v[5][i]=mul(v[4][i],v[5][i]);break;
                case 0x4b000183: v[6][0]=add(v[0][0],v[0][3]);break;
                case 0x4b05283d: vacc[0]=add(v[5][0],v[5][1]);break;
                case 0x4b05314a: v[5][0]=add(vacc[0],mul(v[6][0],v[5][2]));break;
                case 0x4bc821bc: for(unsigned i=0;i<3;++i)vacc[i]=mul(v[4][i],v[8][0]);break;
                case 0x4bc828bd: for(unsigned i=0;i<3;++i)vacc[i]=add(vacc[i],mul(v[5][i],v[8][1]));break;
                case 0x4bc8324a: for(unsigned i=0;i<3;++i)v[9][i]=add(vacc[i],mul(v[6][i],v[8][2]));break;
                case 0x4be04a68: for(unsigned i=0;i<4;++i)v[9][i]=add(v[9][i],v[0][i]);break;
                default: throw std::runtime_error("oracle VU opcode");
            }
        } else throw std::runtime_error("oracle unknown instruction: "+std::to_string(w));
        g[0]={};
        return false;
    }
    void run(float dt) {
        g[16][0]=0; g[29][0]=0x1800; f[20]=dt; f[0]=0; v[0][3]=1;
        std::uint32_t pc=rr_body_48c_start;
        while(pc!=0x14f4f0) {
            std::uint32_t target=0,unused=0;
            if(execute(pc,target)) {
                if(execute(pc+4,unused)) throw std::runtime_error("branch in delay slot");
                pc=target;
            } else pc+=4;
        }
        if(g[31][0]!=0x1680f4) throw std::runtime_error("unexpected core exit");
    }
    void set(const BodyState48C& s,const BodyParameters48C& p,const BodyLoads48C& loads) {
        store(0x60,s.position);store(0x70,s.velocity);store(0xb0,s.angularVelocity);store(0x1f0,s.orientation);
        store(0x2c4,s.previousPositionDerivative);store(0x2dc,s.previousLinearAcceleration);
        store(0x314,s.previousAngularAcceleration);store(0x2f8,s.previousOrientationDerivative);
        store(0x26c,p.mass);store(0x270,p.inverseMass);
        store(0x274,p.linearAccelerationLimitSquared);store(0x278,p.angularAccelerationLimitSquared);
        store(0x27c,p.linearVelocityLimitSquared);store(0x280,p.angularVelocityLimitSquared);
        for(unsigned i=0;i<3;++i) {
            store(0x140+i*16,p.inverseInertiaColumns[i]);
            write(0x2a0+i*4,p.translationEnabled[i]?1:0);write(0x2ac+i*4,p.rotationEnabled[i]?1:0);
        }
        store(0x180,loads.force);store(0x190,loads.torque);write(0x290,0x14ee40);
    }
    BodyState48C state() const {
        return {vec(0x60),vec(0x70),vec(0xb0),quat(0x1f0),vec(0x2c4),vec(0x2dc),vec(0x314),quat(0x2f8),
                vec(0xc0),vec(0xd0),quat(0x2e8)};
    }
};

static BodyParameters48C defaultParameters() {
    return {1,1,{{{1,0,0}},{{0,1,0}},{{0,0,1}}},1e10f,1e10f,1e10f,1e10f,{true,true,true},{true,true,true}};
}
int main() {
    std::uint64_t checks=0;
    auto require=[&](bool pass,const char* why){++checks;if(!pass)throw std::runtime_error(why);};
    std::uint32_t rng=0x48c167c4u;
    auto next=[&](){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;};
    auto randomFloat=[&](float scale){return static_cast<float>(static_cast<int>(next()&65535u)-32768)/scale;};
    auto compare=[&](const BodyState48C& s,const BodyParameters48C& p,const BodyLoads48C& loads,float dt) {
        Original oracle;oracle.set(s,p,loads);oracle.run(dt);const auto expected=oracle.state();
        BodyState48C actual{};
        require(integrateBody48C(s,p,loads,dt,actual),"valid core input rejected");
        static_assert(sizeof(BodyState48C)==36*sizeof(float));
        std::array<std::uint32_t,36> a{},b{};
        std::memcpy(a.data(),&actual,sizeof actual);std::memcpy(b.data(),&expected,sizeof expected);
        for(unsigned i=0;i<a.size();++i) {
            ++checks;
            if(a[i]!=b[i]) throw std::runtime_error("original-word mismatch float "+std::to_string(i)+
                " got "+std::to_string(a[i])+" expected "+std::to_string(b[i]));
        }
        return actual;
    };
    try {
        for(int rounding:{FE_TONEAREST,FE_TOWARDZERO,FE_UPWARD,FE_DOWNWARD}) {
            require(std::fesetround(rounding)==0,"rounding setup");
            for(unsigned n=0;n<1500;++n) {
                BodyState48C s{}; auto p=defaultParameters();BodyLoads48C loads{};
                for(unsigned i=0;i<3;++i) {
                    s.position.v[i]=randomFloat(8);s.velocity.v[i]=randomFloat(512);
                    s.angularVelocity.v[i]=randomFloat(16384);
                    s.previousPositionDerivative.v[i]=randomFloat(512);
                    s.previousLinearAcceleration.v[i]=randomFloat(256);
                    s.previousAngularAcceleration.v[i]=randomFloat(4096);
                    loads.force.v[i]=randomFloat(16);loads.torque.v[i]=randomFloat(512);
                    p.translationEnabled[i]=(next()%4)!=0;p.rotationEnabled[i]=(next()%4)!=0;
                    for(unsigned j=0;j<3;++j)p.inverseInertiaColumns[i].v[j]=randomFloat(65536);
                }
                for(unsigned i=0;i<4;++i) {
                    s.orientation.v[i]=randomFloat(32768);
                    s.previousOrientationDerivative.v[i]=randomFloat(8192);
                }
                p.mass=n%7==0 ? 0.0f : n%11==0 ? -1.0f:1.0f;
                p.inverseMass=static_cast<float>(next()%100)/100.0f;
                p.linearAccelerationLimitSquared=static_cast<float>(next()%6000);
                p.angularAccelerationLimitSquared=static_cast<float>(next()%20);
                p.linearVelocityLimitSquared=static_cast<float>(next()%5000);
                p.angularVelocityLimitSquared=static_cast<float>(next()%8);
                compare(s,p,loads,static_cast<float>(next()%1024)/32768.0f);
            }
            // Nonzero histories survive zero dt and influence the next step.
            BodyState48C s{};s.orientation.v[0]=1;
            auto p=defaultParameters();BodyLoads48C loads{{{2,-3,1}},{{.25f,-.5f,.1f}}};
            for(unsigned i=0;i<512;++i) s=compare(s,p,loads,i%17==0 ? 0.0f:1.0f/120.0f);
            // Saved actual body values, including subnormal angular components.
            // This is synthetic continuation, not an observed before/after tick.
            Original saved;
            for(unsigned i=0;i<sizeof(rr_stopped_body_48c)/4;++i)saved.memory[i]=rr_stopped_body_48c[i];
            p.mass=saved.readFloat(0x26c);p.inverseMass=saved.readFloat(0x270);
            p.linearAccelerationLimitSquared=saved.readFloat(0x274);p.angularAccelerationLimitSquared=saved.readFloat(0x278);
            p.linearVelocityLimitSquared=saved.readFloat(0x27c);p.angularVelocityLimitSquared=saved.readFloat(0x280);
            for(unsigned i=0;i<3;++i) {
                p.inverseInertiaColumns[i]=saved.vec(0x140+i*16);
                p.translationEnabled[i]=saved.read(0x2a0+i*4)!=0;p.rotationEnabled[i]=saved.read(0x2ac+i*4)!=0;
            }
            compare(saved.state(),p,{saved.vec(0x180),saved.vec(0x190)},1.0f/120.0f);
            for(float small:{0.0f,-0.0f,fp(1),fp(0x80000001),1e-20f,1e-19f}) {
                s={};for(float& q:s.orientation.v)q=small;
                compare(s,defaultParameters(),{},0);
            }
        }
        require(std::fesetround(FE_TONEAREST)==0,"rounding restore");
        auto p=defaultParameters();BodyState48C s{};s.orientation.v[0]=1;
        // AB2 history, not Euler: 1 + dt/2*(3*2-(-4)) = 1.5.
        s.velocity.v[0]=1;s.previousLinearAcceleration.v[0]=-4;
        auto result=compare(s,p,{{{2,0,0}},{}},.1f);
        require(result.velocity.v[0]==1.5f,"AB2 velocity semantics");
        // Translation lock clears acceleration history, but not position history.
        p.translationEnabled[0]=false;s.previousPositionDerivative.v[0]=10;
        result=compare(s,p,{},.1f);
        require(result.velocity.v[0]==0 && result.position.v[0]==-.5f,"original lock-history drift");
        // Nonpositive mass skips translation locks and consumes old acceleration.
        p.mass=0;result=compare(s,p,{},.1f);
        require(result.velocity.v[0]==1.2f,"nonpositive mass retains integration history");
        s={};result=compare(s,defaultParameters(),{},0);
        require(result.orientation.v[0]==0,"zero quaternion fallback changed");

        const BodyState48C sentry=result;
        for(float bad:{-1.0f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}) {
            auto out=sentry;
            require(!integrateBody48C(s,defaultParameters(),{},bad,out),"invalid dt accepted");
            require(std::memcmp(&out,&sentry,sizeof out)==0,"invalid dt mutated output");
            p=defaultParameters();p.angularVelocityLimitSquared=bad;
            require(!integrateBody48C(s,p,{},.01f,out),"invalid cap accepted");
        }
        auto out=sentry;p=defaultParameters();s.velocity.v[2]=std::numeric_limits<float>::max();
        require(!integrateBody48C(s,p,{},.1f,out),"overflowing squared speed accepted");
        require(std::memcmp(&out,&sentry,sizeof out)==0,"overflow mutated output");
        s={};s.orientation.v[0]=1;p=defaultParameters();
        const auto expected=compare(s,p,{{{1,2,3}},{{4,5,6}}},.01f);
        require(integrateBody48C(s,p,{{{1,2,3}},{{4,5,6}}},.01f,s),"alias input/output failed");
        require(std::memcmp(&s,&expected,sizeof s)==0,"alias mismatch");
        std::cout<<"physics48c: "<<checks<<" checks passed; exact ELF core + three original helper blocks; 4 rounding modes; AB2 trajectories; snapshot/subnormal/zero/lock/history cases\n";
        return 0;
    } catch(const std::exception& e) {
        std::fesetround(FE_TONEAREST);
        std::cerr<<"physics48c after "<<checks<<" checks: "<<e.what()<<'\n';return 1;
    }
}
