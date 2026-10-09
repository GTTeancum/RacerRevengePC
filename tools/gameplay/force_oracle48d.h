#pragma once
// Generated test-only oracle extension; source hash in recovery manifest.
// Test-only execution of the exact original instruction words, with bounded
// memory and only the encountered instruction subset. No game VM in production.
#include "../../src/game/body_integration48c.h"
#include "../../tools/gameplay/physics_words48c.h"
#include "forces_words48d.h"
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

struct Original48D {
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
        if (a>=0x446800 && a<0x446810 && !(a&3)) return a==0x44680c ? 0x3f800000u:0u;
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
#define FORCE_BLOCK(name) if (pc>=rr_##name##_48d_start && pc-rr_##name##_48d_start<sizeof(rr_##name##_48d)) return rr_##name##_48d[(pc-rr_##name##_48d_start)/4]
        FORCE_BLOCK(engine_update); FORCE_BLOCK(engine_apply); FORCE_BLOCK(direction); FORCE_BLOCK(gravity);
#undef FORCE_BLOCK
        throw std::runtime_error("oracle PC outside original blocks: "+std::to_string(pc));
    }
    // Return true for control transfer (including a not-taken delayed branch).
    bool execute(std::uint32_t pc,std::uint32_t& target) {
        if (++steps>20000) throw std::runtime_error("oracle instruction budget");
        const auto w=word(pc), op=w>>26, rs=(w>>21)&31u, rt=(w>>16)&31u, rd=(w>>11)&31u;
        const auto imm=static_cast<std::int16_t>(w&65535u);
        auto address=[&](){return g[rs][0]+imm;};
        auto branch=[&](bool take){target=take ? pc+4+static_cast<std::int32_t>(imm)*4:pc+8;return true;};
        if(w==0) return false;
        if(op==9) g[rt][0]=g[rs][0]+imm;
        else if(op==12) g[rt][0]=g[rs][0]&(w&65535u);
        else if(op==0x24) { const auto a=address(); g[rt][0]=(read(a&~3u)>>((a&3u)*8))&255u; }
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
        } else if((w&0xfc0007ffu)==0x70000628u && rt==0) g[rd]=g[rs]; // PADDUB register,zero copies all lanes.
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
                    case 7: f[fd]=-f[rd];break;
                    case 0x16: f[fd]=Ps2EeRsqrt14(f[rd],f[rt],flags);break;
                    case 0x18: acc=add(f[rd],f[rt]);break;
                    case 0x1a: acc=mul(f[rd],f[rt]);break;
                    case 0x1c: f[fd]=add(acc,mul(f[rd],f[rt]));break;
                    case 0x1d: f[fd]=sub(acc,mul(f[rd],f[rt]));break;
                    case 0x32: condition=f[rd]==f[rt];break;
                    case 0x34: condition=f[rd]<f[rt];break;
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
