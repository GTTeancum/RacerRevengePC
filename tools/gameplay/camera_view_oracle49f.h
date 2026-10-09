#pragma once
// TEST ONLY: bounded execution of frozen, original camera numeric instructions.
// Finite ordinary host binary32 arithmetic, explicit operation rounding. This is
// not a complete PS2 all-bit/cycle-accurate emulator and is never a game backend.
// No production camera source/types, saved RAM, or query/world helpers are used.
#include "../../evidence/cameraView49F/consumer-oracle/camera_view_words49f.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>

namespace rr { namespace test49f {
using Vec4 = std::array<float,4>;
using Mat4 = std::array<float,16>;
// Optional test-only observer. Null by default; receives each executed original
// instruction address, including delay slots. Set/reset only outside execution.
inline void (*instructionTrace49F)(std::uint32_t) = nullptr;
struct Scene {
    Vec4 position{{0,0,0,1}}, direction{{0,0,1,1}}, up{{0,1,0,1}};
    float roll=0, fov=60;
    std::uint32_t stamp=0;
};
inline std::uint32_t floatBits(float x) { std::uint32_t w; std::memcpy(&w,&x,4); return w; }
inline float wordFloat(std::uint32_t w) { float x; std::memcpy(&x,&w,4); return x; }
namespace detail {
inline float add(float a,float b) { volatile float r=a+b; return r; }
inline float sub(float a,float b) { volatile float r=a-b; return r; }
inline float mul(float a,float b) { volatile float r=a*b; return r; }
inline float div(float a,float b) { volatile float r=a/b; return r; }
inline float root(float a) { volatile float r=std::sqrt(a); return r; }
struct Machine {
    std::array<std::array<std::uint32_t,4>,32> g{};
    std::array<float,32> f{};
    std::array<Vec4,32> v{};
    Vec4 acc{};
    std::array<std::uint32_t,0x10000/4> memory{};
    float q=0;
    bool condition=false;
    std::uint32_t current=0;
    unsigned steps=0;
    static constexpr std::uint32_t sceneAddress=0x1000, frameAddress=0x7000;
    Machine() { g[29][0]=0xf000; v[0][3]=1; }
    [[noreturn]] void fail(const char* what) const { throw std::runtime_error(std::string("camera original-word oracle: ")+what+" at "+std::to_string(current)); }
    std::uint32_t read(std::uint32_t a) const {
        if(a==0x442f70) return frameAddress;
        if((a&3u)||a/4>=memory.size()) fail("read outside synthetic fixture");
        return memory[a/4];
    }
    void write(std::uint32_t a,std::uint32_t x) {
        if((a&3u)||a/4>=memory.size()) fail("write outside synthetic fixture");
        memory[a/4]=x;
    }
    void store(std::uint32_t a,float x) { write(a,floatBits(x)); }
    float scalar(std::uint32_t a) const { return wordFloat(read(a)); }
    template<std::size_t N> void store(std::uint32_t a,const std::array<float,N>& x) { for(unsigned i=0;i<N;++i)store(a+4*i,x[i]); }
    template<std::size_t N> std::array<float,N> array(std::uint32_t a) const { std::array<float,N> r{}; for(unsigned i=0;i<N;++i)r[i]=scalar(a+4*i); return r; }
    void putScene(const Scene& s,std::uint32_t frame) {
        store(sceneAddress+0x180,s.position); store(sceneAddress+0x190,s.direction); store(sceneAddress+0x1a0,s.up);
        store(sceneAddress+0x1d0,s.fov); store(sceneAddress+0x1d4,s.roll); write(sceneAddress+0x210,s.stamp);
        write(frameAddress+12,frame);
    }
    Scene getScene() const {
        Scene s; s.position=array<4>(sceneAddress+0x180);s.direction=array<4>(sceneAddress+0x190);s.up=array<4>(sceneAddress+0x1a0);
        s.fov=scalar(sceneAddress+0x1d0);s.roll=scalar(sceneAddress+0x1d4);s.stamp=read(sceneAddress+0x210);return s;
    }
    std::uint32_t word(std::uint32_t pc) const {
        if(pc&3u) fail("unaligned instruction address");
#define CAMERA_BLOCK(NAME) if(pc>=frozen::NAME##_start && pc-frozen::NAME##_start<sizeof(frozen::NAME)) return frozen::NAME[(pc-frozen::NAME##_start)/4]
        CAMERA_BLOCK(scene_set);CAMERA_BLOCK(look_target);CAMERA_BLOCK(view);CAMERA_BLOCK(projection);
        CAMERA_BLOCK(roll);CAMERA_BLOCK(tan);CAMERA_BLOCK(sincos);CAMERA_BLOCK(zero_matrix);CAMERA_BLOCK(multiply);
#undef CAMERA_BLOCK
        fail("instruction outside camera numeric closure");
    }
    void vectorInstruction(std::uint32_t w) {
        // Exact encountered opcode words; each lane uses a separately rounded
        // host binary32 operation. VU pipeline stalls have no numeric effect.
        switch(w) {
        case 0x48222800: for(unsigned i=0;i<4;++i)g[2][i]=floatBits(v[5][i]);return; // qmfc2
        case 0x4bc4216a: for(unsigned i=0;i<3;++i)v[5][i]=mul(v[4][i],v[4][i]);return;
        case 0x4bc5216a: for(unsigned i=0;i<3;++i)v[5][i]=mul(v[4][i],v[5][i]);return;
        case 0x4b000183: v[6][0]=add(v[0][0],v[0][3]);return;
        case 0x4b05283d: acc[0]=add(v[5][0],v[5][1]);return;
        case 0x4b05314a: v[5][0]=add(acc[0],mul(v[6][0],v[5][2]));return;
        case 0x4a6503be: q=v[5][0]==0 ? wordFloat(0x7f7fffffu) : div(v[0][3],root(v[5][0]));return; // vrsqrt Q,vf0.w,vf5.x; finite zero-divide result
        case 0x4a202180: v[6][3]=add(v[4][3],v[0][0]);return; // original W + +0; -0 becomes +0
        case 0x4a0003bf: return; // vwaitq
        case 0x4bc0219c: for(unsigned i=0;i<3;++i)v[6][i]=mul(v[4][i],q);return;
        case 0x4a2631ac: v[6][3]=sub(v[6][3],v[6][3]);return;
        case 0x4bc522fe: // vopmula.xyz ACC,vf4,vf5
            acc[0]=mul(v[4][1],v[5][2]);acc[1]=mul(v[4][2],v[5][0]);acc[2]=mul(v[4][0],v[5][1]);return;
        case 0x4bc429ae: // vopmsub.xyz vf6,vf5,vf4
            v[6][0]=sub(acc[0],mul(v[5][1],v[4][2]));v[6][1]=sub(acc[1],mul(v[5][2],v[4][0]));v[6][2]=sub(acc[2],mul(v[5][0],v[4][1]));return;
        default: break;
        }
        // vmulax/vmadday/vmaddaz/vmaddw in the frozen 4x4 product.
        // Every accepted word is one of the exact four-column instruction forms.
        for(unsigned src=5;src<=8;++src) {
            if(w==(0x4be009bcu|(src<<16))) { for(unsigned i=0;i<4;++i)acc[i]=mul(v[1][i],v[src][0]);return; }
            if(w==(0x4be010bdu|(src<<16))) { for(unsigned i=0;i<4;++i)acc[i]=add(acc[i],mul(v[2][i],v[src][1]));return; }
            if(w==(0x4be018beu|(src<<16))) { for(unsigned i=0;i<4;++i)acc[i]=add(acc[i],mul(v[3][i],v[src][2]));return; }
            const unsigned dst=src+4;
            if(w==(0x4be0200bu|(src<<16)|(dst<<6))) { for(unsigned i=0;i<4;++i)v[dst][i]=add(acc[i],mul(v[4][i],v[src][3]));return; }
        }
        fail("unimplemented vector instruction");
    }
    bool execute(std::uint32_t pc,std::uint32_t& target) {
        current=pc;if(++steps>10000)fail("instruction budget");
        const auto w=word(pc),op=w>>26,rs=(w>>21)&31u,rt=(w>>16)&31u,rd=(w>>11)&31u;
        if(instructionTrace49F) instructionTrace49F(pc);
        const auto imm=static_cast<std::int16_t>(w&65535u);
        auto address=[&](){return g[rs][0]+static_cast<std::int32_t>(imm);};
        auto branch=[&](bool take){target=take ? pc+4+static_cast<std::int32_t>(imm)*4 : pc+8;return true;};
        if(w==0)return false;
        if(op==9||op==0x19)g[rt][0]=g[rs][0]+static_cast<std::int32_t>(imm);
        else if(op==12)g[rt][0]=g[rs][0]&(w&65535u);
        else if(op==13)g[rt][0]=g[rs][0]|(w&65535u);
        else if(op==15)g[rt][0]=(w&65535u)<<16;
        else if(op==0x23)g[rt][0]=read(address());
        else if(op==0x2b)write(address(),g[rt][0]);
        else if(op==0x31)f[rt]=scalar(address());
        else if(op==0x39)store(address(),f[rt]);
        else if(op==0x1e){auto a=address();for(unsigned i=0;i<4;++i)g[rt][i]=read(a+i*4);}
        else if(op==0x1f){auto a=address();for(unsigned i=0;i<4;++i)write(a+i*4,g[rt][i]);}
        else if(op==0x36)v[rt]=array<4>(address());
        else if(op==0x3e)store(address(),v[rt]);
        else if(op==4)return branch(g[rs][0]==g[rt][0]);
        else if(op==7)return branch(static_cast<std::int32_t>(g[rs][0])>0);
        else if(op==3){g[31][0]=pc+8;target=(w&0x3ffffffu)*4;return true;}
        else if(op==0&&(w&63u)==8){target=g[rs][0];return true;}
        else if((w&0xfc0007ffu)==0x70000628u&&rt==0)g[rd]=g[rs]; // POR register,zero
        else if(op==0x11) {
            if(rs==4)f[rd]=wordFloat(g[rt][0]);
            else if(rs==8)return branch(condition==((rt&1u)!=0));
            else if(rs==16) {
                const auto fd=(w>>6)&31u,fn=w&63u;
                switch(fn) {
                case 0:f[fd]=add(f[rd],f[rt]);break;
                case 1:f[fd]=sub(f[rd],f[rt]);break;
                case 2:f[fd]=mul(f[rd],f[rt]);break;
                case 3:f[fd]=div(f[rd],f[rt]);break;
                case 6:f[fd]=f[rd];break;
                case 7:f[fd]=-f[rd];break;
                case 0x32:condition=f[rd]==f[rt];break;
                case 0x34:condition=f[rd]<f[rt];break;
                case 0x36:condition=f[rd]<=f[rt];break;
                default:fail("unimplemented scalar floating instruction");
                }
            } else fail("unimplemented COP1 instruction");
        } else if(op==0x12)vectorInstruction(w);
        else fail("unimplemented integer instruction");
        g[0]={};v[0]={{0,0,0,1}};return false;
    }
    void run(std::uint32_t start,std::uint32_t stop=0) {
        g[31][0]=0;
        for(std::uint32_t pc=start;pc!=stop;) {
            std::uint32_t target=0,ignored=0;
            if(execute(pc,target)) {
                if(execute(pc+4,ignored))fail("control transfer in delay slot");
                pc=target;
            } else pc+=4;
        }
    }
};
} // detail
inline void sceneSet(Scene& scene,const Vec4* position,const Vec4* direction,const Vec4* up,const float* roll,const float* fov,std::uint32_t frame=0) {
    detail::Machine m;m.putScene(scene,frame);m.g[4][0]=m.sceneAddress;
    if(position){m.store(0x2000,*position);m.g[5][0]=0x2000;}
    if(direction){m.store(0x2010,*direction);m.g[6][0]=0x2010;}
    if(up){m.store(0x2020,*up);m.g[7][0]=0x2020;}
    if(roll){m.store(0x2030,*roll);m.g[8][0]=0x2030;}
    if(fov){m.store(0x2034,*fov);m.g[9][0]=0x2034;}
    m.run(frozen::scene_set_start);scene=m.getScene();
}
inline bool lookTarget(Scene& scene,const Vec4& target,std::uint32_t frame=0) {
    detail::Machine m;m.putScene(scene,frame);m.store(0x2000,target);m.g[4][0]=m.sceneAddress;m.g[5][0]=0x2000;
    m.run(frozen::look_target_start);scene=m.getScene();return m.g[2][0]!=0;
}
inline Mat4 view(const Vec4& position,const Vec4& direction,const Vec4& up,float roll) {
    detail::Machine m;m.store(0x2000,position);m.store(0x2010,direction);m.store(0x2020,up);
    m.g[4][0]=0x1000;m.g[5][0]=0x2000;m.g[6][0]=0x2010;m.g[7][0]=0x2020;m.f[12]=roll;
    m.run(frozen::view_start);return m.array<16>(0x1000);
}
inline Mat4 projection(float nearClip,float farClip,float fovRadians,float inverseAspect) {
    detail::Machine m;m.g[4][0]=0x1000;m.f[12]=nearClip;m.f[13]=farClip;m.f[14]=fovRadians;m.f[15]=inverseAspect;
    m.run(frozen::projection_start);return m.array<16>(0x1000);
}
inline Mat4 rollMatrix(float angle) {
    detail::Machine m;m.g[4][0]=0x1000;m.f[12]=angle;m.run(frozen::roll_start);return m.array<16>(0x1000);
}
inline Mat4 zeroMatrix() {
    detail::Machine m;for(unsigned i=0;i<16;++i)m.store(0x1000+4*i,91.0f);m.g[4][0]=0x1000;
    m.run(frozen::zero_matrix_start);return m.array<16>(0x1000);
}
inline Mat4 multiply(const Mat4& viewMatrix,const Mat4& projectionMatrix) {
    detail::Machine m;m.g[16][0]=m.sceneAddress;m.store(m.sceneAddress+0xc0,viewMatrix);m.store(m.sceneAddress+0x80,projectionMatrix);
    m.run(frozen::multiply_start,0x11548c);return m.array<16>(m.sceneAddress+0x100);
}
inline float tanSource(float angle) {
    detail::Machine m;m.f[12]=angle;m.run(frozen::tan_start);return m.f[0];
}
inline std::array<float,2> sincosSource(float angle) {
    detail::Machine m;m.f[12]=angle;m.g[4][0]=0x1000;m.g[5][0]=0x1004;m.run(frozen::sincos_start);
    return m.array<2>(0x1000); // sine, cosine
}
} } // rr::test49f
