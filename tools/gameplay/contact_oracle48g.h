#pragma once
// Generated test-only oracle extension; source hash in recovery manifest.
// Test-only execution of the exact original instruction words, with bounded
// memory and only the encountered instruction subset. No game VM in production.
#include "../../src/game/body_integration48c.h"
#include "../../tools/gameplay/physics_words48c.h"
#include "forces_words48d.h"
#include "aero_words48e.h"
#include "contact_words48g.h"
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

struct Original48G {
    std::array<std::array<std::uint32_t,4>,32> g{};
    std::array<float,32> f{};
    std::array<std::array<float,4>,32> v{};
    std::array<float,4> vacc{};
    std::array<std::uint32_t,0x4000/4> memory{};
    float acc=0, q=0;
    bool annulDelay=false;
    bool condition=false;
    std::uint32_t flags=0;
    unsigned steps=0;
    std::uint32_t currentPc=0;
    std::array<bool,5> modeCoverage{};
    std::uint32_t read(std::uint32_t a) const {
        if(a>=0x434260&&a<0x4342e0&&!(a&3))return rr_terrain_constants_48g[(a-0x434260)/4];
        if(a==0x433c10)return 0x3f000000u;
        if(a==0x433c18)return 0x3e800000u;
        if(a==0x433c20)return 0x40800000u;
        if (a>=0x446800 && a<0x446810 && !(a&3)) return a==0x44680c ? 0x3f800000u:0u;
        if (a>=0x382b60 && a<0x382b70 && !(a&3)) return 0;
        if (a>=0x433b80 && a<0x433b90 && !(a&3)) return a==0x433b8c ? 0x3f800000u:0u;
        if ((a&3) || a/4>=memory.size()) throw std::runtime_error("oracle read outside fixture address="+std::to_string(a)+" PC="+std::to_string(currentPc));
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
#define AERO_BLOCK(name) if(pc>=rr_##name##_48e_start && pc-rr_##name##_48e_start<sizeof(rr_##name##_48e)) return rr_##name##_48e[(pc-rr_##name##_48e_start)/4]
        AERO_BLOCK(aero);AERO_BLOCK(angle);AERO_BLOCK(axes);AERO_BLOCK(inverse_direction);AERO_BLOCK(cached_speed);
#undef AERO_BLOCK
#define CONTACT_BLOCK(name) if(pc>=rr_##name##_48g_start&&pc-rr_##name##_48g_start<sizeof(rr_##name##_48g))return rr_##name##_48g[(pc-rr_##name##_48g_start)/4]
        CONTACT_BLOCK(terrain);CONTACT_BLOCK(terrain_angle);CONTACT_BLOCK(point);CONTACT_BLOCK(translate);CONTACT_BLOCK(set_axes);CONTACT_BLOCK(scene_dirty);CONTACT_BLOCK(dot);CONTACT_BLOCK(make_vector);CONTACT_BLOCK(sqrt);
#undef CONTACT_BLOCK
        throw std::runtime_error("oracle PC outside original blocks: "+std::to_string(pc));
    }
    // Return true for control transfer (including a not-taken delayed branch).
    bool execute(std::uint32_t pc,std::uint32_t& target) {
        currentPc=pc;annulDelay=false;
        const std::uint32_t modePc[]={0x16d10c,0x16d130,0x16d154,0x16d390,0x16d450};
        for(unsigned i=0;i<5;++i)if(pc==modePc[i])modeCoverage[i]=true;
        if (++steps>20000) throw std::runtime_error("oracle instruction budget");
        const auto w=word(pc), op=w>>26, rs=(w>>21)&31u, rt=(w>>16)&31u, rd=(w>>11)&31u;
        const auto imm=static_cast<std::int16_t>(w&65535u);
        auto address=[&](){return g[rs][0]+imm;};
        auto branch=[&](bool take){target=take ? pc+4+static_cast<std::int32_t>(imm)*4:pc+8;return true;};
        if(w==0) return false;
        if(op==0x19)g[rt][0]=g[rs][0]+imm;
        else if(op==0x14){const bool take=g[rs][0]==g[rt][0];annulDelay=!take;return branch(take);}
        else if(op==0x28){const auto a=address();auto w0=read(a&~3u);const auto s=(a&3u)*8;write(a&~3u,(w0&~(255u<<s))|((g[rt][0]&255u)<<s));}
        else if(op==9) g[rt][0]=g[rs][0]+imm;
        else if(op==13) g[rt][0]=g[rs][0]|(w&65535u);
        else if(op==7) return branch(static_cast<std::int32_t>(g[rs][0])>0);
        else if(op==0x20) {const auto a=address();g[rt][0]=static_cast<std::int8_t>((read(a&~3u)>>((a&3u)*8))&255u);}
        else if(op==12) g[rt][0]=g[rs][0]&(w&65535u);
        else if(op==0x24) { const auto a=address(); g[rt][0]=(read(a&~3u)>>((a&3u)*8))&255u; }
        else if(op==15) g[rt][0]=(w&65535u)<<16;
        else if(op==0x23) g[rt][0]=read(address());
        else if(op==0x2b) write(address(),g[rt][0]);
        else if(op==0x31) f[rt]=readFloat(address());
        else if(op==0x39) store(address(),f[rt]);
        else if(op==0x1e) {const auto a=address();for(unsigned i=0;i<4;++i) g[rt][i]=read(a+i*4);}
        else if(op==0x1f) for(unsigned i=0;i<4;++i) write(address()+i*4,g[rt][i]);
        else if(op==0x36) for(unsigned i=0;i<4;++i) v[rt][i]=readFloat(address()+i*4);
        else if(op==0x3e) for(unsigned i=0;i<4;++i) store(address()+i*4,v[rt][i]);
        else if(op==4) return branch(g[rs][0]==g[rt][0]);
        else if(op==5) return branch(g[rs][0]!=g[rt][0]);
        else if(op==3) {g[31][0]=pc+8;target=(w&0x3ffffffu)*4;return true;}
        else if(op==0) {
            switch(w&63u) {
                case 0: g[rd][0]=g[rt][0]<<((w>>6)&31u); break;
                case 0x24:g[rd][0]=g[rs][0]&g[rt][0];break;
                case 0x25:g[rd][0]=g[rs][0]|g[rt][0];break;
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
            else if(rs==20 && (w&63u)==0x20) f[(w>>6)&31u]=static_cast<float>(static_cast<std::int32_t>(bits(f[rd])));
            else if(rs==16) {
                const auto fd=(w>>6)&31u, fn=w&63u;
                switch(fn) {
                    case 0: f[fd]=add(f[rd],f[rt]);break;
                    case 1: f[fd]=sub(f[rd],f[rt]);break;
                    case 2: f[fd]=mul(f[rd],f[rt]);break;
                    case 3: f[fd]=divide(f[rd],f[rt]);break;
                    case 4: f[fd]=Ps2EeSqrt14(f[rt],flags);break;
                    case 5: f[fd]=std::fabs(f[rd]);break;
                    case 6: f[fd]=f[rd];break;
                    case 7: f[fd]=-f[rd];break;
                    case 0x16: f[fd]=Ps2EeRsqrt14(f[rd],f[rt],flags);break;
                    case 0x18: acc=add(f[rd],f[rt]);break;
                    case 0x19: acc=sub(f[rd],f[rt]);break;
                    case 0x28: case 0x29: {
                        // Independent signed-word ordering from supplied PCSX2
                        // FPU.cpp fp_min/fp_max, including opposite signed zero.
                        const auto a=static_cast<std::int32_t>(bits(f[rd])), b=static_cast<std::int32_t>(bits(f[rt]));
                        const bool chooseMin=(fn==0x29) != (a<0 && b<0);
                        f[fd]=fp(static_cast<std::uint32_t>(chooseMin ? (a<b?a:b):(a>b?a:b)));break;
                    }
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
            if((w&0x7ffu)==0x2feu) {
                for(unsigned i=0;i<3;++i)vacc[i]=mul(v[rd][(i+1)%3],v[rt][(i+2)%3]);
            } else if((w&63u)==0x2eu) {
                auto result=v[(w>>6)&31u];for(unsigned i=0;i<3;++i)result[i]=sub(vacc[i],mul(v[rd][(i+1)%3],v[rt][(i+2)%3]));v[(w>>6)&31u]=result;
            } else if((w&63u)==0x2cu && ((w>>21)&15u)==1u) {
                v[(w>>6)&31u][3]=sub(v[rd][3],v[rt][3]);
            } else switch(w) {
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
                case 0x4bfef7ac: for(unsigned i=0;i<4;++i)v[30][i]=sub(v[30][i],v[30][i]);break;
                case 0x4bfe0928: for(unsigned i=0;i<4;++i)v[4][i]=add(v[1][i],v[30][i]);break;
                case 0x4bfe1168: for(unsigned i=0;i<4;++i)v[5][i]=add(v[2][i],v[30][i]);break;
                case 0x4bfe19a8: for(unsigned i=0;i<4;++i)v[6][i]=add(v[3][i],v[30][i]);break;
                case 0x4b010141: v[5][0]=add(v[0][0],v[1][1]);break;
                case 0x4b010182: v[6][0]=add(v[0][0],v[1][2]);break;
                case 0x4a820182: v[6][1]=add(v[0][1],v[2][2]);break;
                case 0x4a820100: v[4][1]=add(v[0][1],v[2][0]);break;
                case 0x4a430100: v[4][2]=add(v[0][2],v[3][0]);break;
                case 0x4a430141: v[5][2]=add(v[0][2],v[3][1]);break;
                case 0x4beb5aec: for(unsigned i=0;i<4;++i)v[11][i]=sub(v[11][i],v[11][i]);break;
                case 0x4beb52e8: for(unsigned i=0;i<4;++i)v[11][i]=add(v[10][i],v[11][i]);break;
                case 0x4bca21bc: for(unsigned i=0;i<3;++i)vacc[i]=mul(v[4][i],v[10][0]);break;
                case 0x4bca28bd: for(unsigned i=0;i<3;++i)vacc[i]=add(vacc[i],mul(v[5][i],v[10][1]));break;
                case 0x4bca32ca: for(unsigned i=0;i<3;++i)v[11][i]=add(vacc[i],mul(v[6][i],v[10][2]));break;
                case 0x4bec632c: for(unsigned i=0;i<4;++i)v[12][i]=sub(v[12][i],v[12][i]);break;
                case 0x4a20632c: v[12][3]=sub(v[12][3],v[0][3]);break;
                case 0x4bcc5adb: for(unsigned i=0;i<3;++i)v[11][i]=mul(v[11][i],v[12][3]);break;
                case 0x4bc4216a:for(unsigned i=0;i<3;++i)v[5][i]=mul(v[4][i],v[4][i]);break;
                case 0x4a6503be:{const float d=v[5][0];if(d==0)q=fp((bits(d)&0x80000000u)|0x7f7fffffu);else {volatile float temp=std::sqrt(std::fabs(d));q=divide(1.0f,temp);}break;}
                case 0x4a202180:v[6][3]=add(v[4][3],v[0][0]);break;
                case 0x4a0003bf:break;
                case 0x4bc0219c:for(unsigned i=0;i<3;++i)v[6][i]=mul(v[4][i],q);break;
                case 0x4a20025b:v[9][3]=mul(v[0][3],v[0][3]);break;
                case 0x4bc74a68:for(unsigned i=0;i<3;++i)v[9][i]=add(v[9][i],v[7][i]);break;
                case 0x4a2701e8:v[7][3]=add(v[0][3],v[7][3]);break;
                default:throw std::runtime_error("oracle VU opcode="+std::to_string(w)+" pc="+std::to_string(pc));
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
