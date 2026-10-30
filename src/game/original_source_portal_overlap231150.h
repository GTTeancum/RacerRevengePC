#pragma once
#include <array>
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <optional>
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Source231150 requires strict finite binary32 arithmetic"
#endif
namespace rr::game::source_portal_overlap231150 {
using Words=std::array<std::uint32_t,4>;
struct History {
    bool entered=false,returned=false,hit=false;std::uint32_t pc=0;
    std::array<std::optional<std::uint32_t>,16> stack;
    std::optional<std::uint32_t> outputDistance;
    unsigned edge=0;
};
inline float value(std::uint32_t w)noexcept{return std::bit_cast<float>(w);}
inline std::uint32_t bits(float f)noexcept{return std::bit_cast<std::uint32_t>(f);}
//Same explicitly native finite/nearest policy as source230690. No EE/VU
//exceptional or bit-equivalence claim, and no geometry preparation/rewrite.
struct Arithmetic {
    bool valid=true;
    float keep(float f)noexcept {valid=valid&&std::isfinite(f);return f;}
    bool admit(float a,float b)noexcept {valid=valid&&std::fegetround()==FE_TONEAREST&&std::isfinite(a)&&std::isfinite(b);return valid;}
    float sub(float a,float b)noexcept {if(!admit(a,b))return 0;volatile float f=a-b;return keep(f);}
    float add(float a,float b)noexcept {if(!admit(a,b))return 0;volatile float f=a+b;return keep(f);}
    float mul(float a,float b)noexcept {if(!admit(a,b))return 0;volatile float f=a*b;return keep(f);}
    float dot(const Words& a,const Words& b)noexcept {
        const auto x=mul(value(a[0]),value(b[0]));const auto y=mul(value(a[1]),value(b[1]));const auto z=mul(value(a[2]),value(b[2]));
        const auto one=add(0,1);const auto xy=add(x,y);return add(xy,mul(one,z));
    }
};
inline bool run(const std::array<Words,4>& vertices,const Words& normal,const std::array<Words,4>& edges,
    const Words& center,std::uint32_t radius,History& h)noexcept {
    if(h.entered)return false;h.entered=true;Arithmetic a;
    const auto at=[&](std::uint32_t pc){h.pc=pc;};
    const auto done=[&](bool hit){h.hit=hit;h.returned=true;at(0x2312f4);return true;};
    //The W of normal/edges is unknown and never consumed by the XYZ dot.
    at(0x23116c);float x=a.sub(value(center[0]),value(vertices[0][0]));if(!a.valid)return false;
    at(0x231178);float y=a.sub(value(center[1]),value(vertices[0][1]));if(!a.valid)return false;
    at(0x23117c);h.stack[4]=bits(x);
    at(0x231184);float z=a.sub(value(center[2]),value(vertices[0][2]));if(!a.valid)return false;
    at(0x231188);h.stack[5]=bits(y);at(0x23118c);h.stack[6]=bits(z);at(0x231190);h.stack[7]=0x3f800000;
    Words relative{{bits(x),bits(y),bits(z),0x3f800000}};
    at(0x231198);for(unsigned k=0;k<4;++k)h.stack[k]=relative[k];
    at(0x2311a4);const auto signedDistance=a.dot(relative,normal);if(!a.valid)return false;
    at(0x2311cc);h.stack[15]=bits(signedDistance)&0x7fffffffu;
    at(0x2311d4);h.outputDistance=h.stack[15]; //Caller stackAC is written before slab/edge tests.
    at(0x2311d8);if(!a.admit(signedDistance,value(radius)))return false;
    if(!(signedDistance<=value(radius)))return done(false);
    at(0x2311ec);if(signedDistance < -value(radius))return done(false);
    at(0x231218);auto distance=a.dot(relative,edges[0]);if(!a.valid)return false;
    at(0x231234);if(!(distance<=value(radius)))return done(false);
    for(h.edge=1;h.edge<4;++h.edge){const auto i=h.edge;
        at(0x23127c);x=a.sub(value(center[0]),value(vertices[i][0]));if(!a.valid)return false;
        at(0x231280);y=a.sub(value(center[1]),value(vertices[i][1]));if(!a.valid)return false;
        at(0x231284);h.stack[8]=bits(x);
        at(0x231288);z=a.sub(value(center[2]),value(vertices[i][2]));if(!a.valid)return false;
        at(0x23128c);h.stack[9]=bits(y);at(0x231290);h.stack[10]=bits(z);at(0x231294);h.stack[11]=0x3f800000;
        relative={{bits(x),bits(y),bits(z),0x3f800000}};
        at(0x23129c);for(unsigned k=0;k<4;++k)h.stack[k]=relative[k];
        at(0x2312a8);distance=a.dot(relative,edges[i]);if(!a.valid)return false;
        at(0x2312c4);if(!(distance<=value(radius)))return done(false);
    }
    return done(true);
}
inline bool historyCurrent(const History& h)noexcept {
    if(!h.entered){if(h.returned||h.hit||h.pc||h.edge||h.outputDistance)return false;
        for(const auto& w:h.stack)if(w)return false;return true;}
    if(!h.pc||h.edge>4||(h.hit&&!h.returned))return false;
    if(h.outputDistance&&(!h.stack[15]||h.outputDistance!=h.stack[15]||(*h.outputDistance&0x80000000u)))return false;
    return !h.returned||(h.outputDistance&&h.pc==0x2312f4);
}
}
