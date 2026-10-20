#pragma once
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "RC environment projection requires strict finite arithmetic"
#endif
namespace rr::game::original_track_environment50RC {
using Vec3 = std::array<float,3>;
using Uv = std::array<float,2>;
struct Vertex {
    std::array<std::int32_t,3> position{};
    std::array<std::int8_t,3> normal{};
};
struct Inputs {
    // Camera in the SAME source quantized-position coordinate frame. The
    // actual model/placement producer owns its basis and quantization origin.
    // A world camera, native normalized normal or guessed identity is invalid.
    Vec3 sourceLocalCamera{};
    float qPosition = 0.f, qNormal = 0.f;
};
namespace detail {
inline float add(float a,float b) noexcept {volatile float v=a+b;return v;}
inline float sub(float a,float b) noexcept {volatile float v=a-b;return v;}
inline float mul(float a,float b) noexcept {volatile float v=a*b;return v;}
inline float div(float a,float b) noexcept {volatile float v=a/b;return v;}
inline bool finite(float x) noexcept {return std::isfinite(x);}
inline bool ready() noexcept {return sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&std::fegetround()==FE_TONEAREST;}
}
// EnvironmentMapCompressed 33AB78..33AD58, non-clipped single-palette path.
// e=normalize(rawPosition*qPosition-sourceLocalCamera), n=rawNormal*qNormal.
// dot order is (e.x*n.x + e.z*n.z) + e.y*n.y. The original flips BOTH
// e.y and n.y after the dot, then forms .5+.5*e.xy-n.xy*dot.
// It does not normalize n, clamp UV, use camera view axes or apply shininess.
// Native binary32 and std::sqrt are an explicit functional-port policy, not
// VU EFU approximation/flags, clipped-packet arithmetic or raster parity.
// Pure numerical leaf: callers authenticate actual source/camera ownership.
// A zero/underflowed view length or nonfinite intermediate rejects; output is
// unchanged. No callbacks, allocation, FP-mode changes or source-state writes.
inline bool projectUv(const Vertex& v,const Inputs& in,Uv& output) noexcept {
    if(!detail::ready()||!detail::finite(in.qPosition)||in.qPosition<=0.f||
       !detail::finite(in.qNormal)||in.qNormal<=0.f)return false;
    Vec3 e{},n{},products{};
    for(unsigned a=0;a<3;++a){
        if(v.position[a]<-8388608||v.position[a]>8388607||!detail::finite(in.sourceLocalCamera[a]))return false;
        const float p=detail::mul(static_cast<float>(v.position[a]),in.qPosition);
        e[a]=detail::sub(p,in.sourceLocalCamera[a]);
        n[a]=detail::mul(static_cast<float>(v.normal[a]),in.qNormal);
        products[a]=detail::mul(e[a],e[a]);
        if(!detail::finite(p)||!detail::finite(e[a])||!detail::finite(n[a])||!detail::finite(products[a]))return false;
    }
    const float xy=detail::add(products[0],products[1]);
    const float squared=detail::add(xy,products[2]);
    if(!detail::finite(xy)||!detail::finite(squared)||squared<=0.f)return false;
    const float length=std::sqrt(squared);
    const float reciprocal=detail::div(1.f,length);
    if(!detail::finite(length)||length<=0.f||!detail::finite(reciprocal))return false;
    for(unsigned a=0;a<3;++a){
        e[a]=detail::mul(e[a],reciprocal);
        products[a]=detail::mul(e[a],n[a]);
        if(!detail::finite(e[a])||!detail::finite(products[a]))return false;
    }
    const float xz=detail::add(products[0],products[2]);
    const float dot=detail::add(xz,products[1]);
    if(!detail::finite(xz)||!detail::finite(dot))return false;
    e[1]=detail::sub(0.f,e[1]);n[1]=detail::sub(0.f,n[1]);
    Uv next{};
    for(unsigned a=0;a<2;++a){
        const float half=detail::mul(e[a],.5f);
        const float biased=detail::add(.5f,half);
        const float reflected=detail::mul(n[a],dot);
        next[a]=detail::sub(biased,reflected);
        if(!detail::finite(half)||!detail::finite(biased)||!detail::finite(reflected)||!detail::finite(next[a]))return false;
    }
    output=next;return true;
}
// The environment program uses vf12, loaded from actual VU row27 by the
// preamble at340328. 33ABB8/33ABC0 and33AC08 store row27+2^23 raw low bytes;
// these are NOT the PSG prelit vertex colors. Native admission here is the
// nonnegative byte-range ambient subset, with no invented ambient owner.
inline bool projectAmbient(const std::array<float,4>& sourceRow27,
    std::array<std::uint8_t,4>& output) noexcept {
    if(!detail::ready())return false;
    std::array<std::uint8_t,4> next{};
    for(unsigned c=0;c<4;++c){
        if(!detail::finite(sourceRow27[c])||sourceRow27[c]<0.f||sourceRow27[c]>255.f)return false;
        const float packed=detail::add(sourceRow27[c],8388608.f);
        std::uint32_t bits=0;std::memcpy(&bits,&packed,sizeof(bits));
        next[c]=static_cast<std::uint8_t>(bits&255u);
    }
    output=next;return true;
}
// A0/B2/C1/D1 and A0/B2/C0/D1 byte equations, after texture MODULATE and
// alpha test. The source alpha is independently stored, never blended.
// This pure reference does not certify the current native framebuffer order.
inline std::array<std::uint8_t,4> blendBytes(
    const std::array<std::uint8_t,4>& source,const std::array<std::uint8_t,4>& destination,
    bool destinationAlpha) noexcept {
    const unsigned alpha=destinationAlpha?destination[3]:source[3];
    std::array<std::uint8_t,4> result=source;
    for(unsigned c=0;c<3;++c){
        const unsigned v=(unsigned(source[c])*alpha)/128u+destination[c];
        result[c]=static_cast<std::uint8_t>(v>255u?255u:v);
    }
    return result;
}
} // namespace rr::game::original_track_environment50RC
