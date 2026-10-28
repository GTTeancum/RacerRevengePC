#pragma once
#include "camera_view49f.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <optional>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "LightFlare numeric adaptation requires strict finite-aware floating point"
#endif

namespace rr::game::light_flare_numeric50LA {
static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559 &&
    std::numeric_limits<float>::radix==2 && std::numeric_limits<float>::digits==24 &&
    std::numeric_limits<float>::max_exponent==128 &&
    std::numeric_limits<float>::min_exponent==-125,
    "LightFlare numeric adaptation requires IEEE binary32");
// DRAFT numeric leaf only. Actual camera/controls/owner/currentness are caller
// obligations. No selectors, resource acquisition, draw or global publication.
// Native binary32 instruction-step adaptation: FE_TONEAREST is admission, not
// evidence of original FCSR. No universal EE ACC/Q/denormal equivalence claim.
struct Inputs {
    const CameraViewOutput49F* actualCamera;
    const std::array<float,4>* actualPosition40;
    float size98;
    std::uint32_t turning9C, enabled382B28, distance427040;
    // Actual lazy producer values 427050 and 427058, only reached if distance.
    std::optional<std::array<float,2>> distanceGlobals7050_7058;
};
struct Corners {
    std::array<float,4> field50, field60, field70, field80;
};
namespace detail {
inline float fp(std::uint32_t w) noexcept { float f; std::memcpy(&f,&w,4); return f; }
struct Math {
    bool valid=true;
    float keep(float x) noexcept {
        // Subnormal arithmetic/flush modes are outside this bounded leaf.
        if(!std::isfinite(x)||(x!=0.0f&&std::fpclassify(x)==FP_SUBNORMAL)) valid=false;
        return x;
    }
    float add(float a,float b) noexcept {keep(a);keep(b);volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {keep(a);keep(b);volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {keep(a);keep(b);volatile float x=a*b;return keep(x);}
    float div(float a,float b) noexcept {keep(a);keep(b);if(b==0){valid=false;return 0;}volatile float x=a/b;return keep(x);}
    float root(float a) noexcept {keep(a);if(a<0){valid=false;return 0;}volatile float x=std::sqrt(a);return keep(x);}
};
// 14AD10, not 13C1F0/atan2: exact strict near-axis and quadrant branches.
inline float angle(float y,float x,Math& f) noexcept {
    f.keep(y);f.keep(x);
    const float a=std::fabs(x),b=std::fabs(y),eps=fp(0x3727c5ac);
    const float h=fp(0x3fc90fdb),p=fp(0x40490fdb);
    if(a<eps&&b<eps)return f.div(y,x);
    if(a<eps&&b>eps)return y<=0?-h:h;
    if(b<eps&&a>eps)return x<=0?p:0.0f;
    auto poly=[&](float r) noexcept {
        const float t=f.mul(r,r);
        float q=f.mul(fp(0x3d1fb705),t);
        q=f.sub(fp(0x3e15c988),q);q=f.mul(t,q);
        q=f.add(fp(0xbea471f4),q);q=f.mul(t,q);
        q=f.add(fp(0x3f7fcc8e),q);return f.mul(r,q);
    };
    if(x>0&&y>0)return b<a?poly(f.div(b,a)):f.sub(h,poly(f.div(a,b)));
    if(x<0&&y>0)return b<a?f.sub(p,poly(f.div(b,a))):f.add(h,poly(f.div(a,b)));
    if(x<0&&y<0)return b<a?f.add(p,poly(f.div(b,a))):f.sub(fp(0x4096cbe4),poly(f.div(a,b)));
    if(x>0&&y<0)return b<a?-poly(f.div(b,a)):f.add(-h,poly(f.div(a,b)));
    return 0.0f;
}
// 13BAD0/13BBA0; private chariot_ik49t helpers are not callable here.
inline float sine(float a,Math& f) noexcept {
    if(!(a<=fp(0x3fc90fdb)))a=f.sub(fp(0x40490fdb),a);
    else if(a<fp(0xbfc90fdb))a=f.sub(fp(0xc0490fdb),a);
    const float s=f.mul(a,a);float r=f.mul(fp(0x3638ef1d),s);
    r=f.add(fp(0xb9500d01),r);r=f.mul(s,r);r=f.add(fp(0x3c088889),r);
    r=f.mul(s,r);r=f.add(fp(0xbe2aaaab),r);
    return f.add(a,f.mul(f.mul(a,s),r));
}
inline float cosine(float a,Math& f) noexcept {
    float sign=1;
    if(!(a<=fp(0x3fc90fdb))){a=f.sub(fp(0x40490fdb),a);sign=-1;}
    else if(a<fp(0xbfc90fdb)){a=f.sub(fp(0xc0490fdb),a);sign=-1;}
    const float s=f.mul(a,a);float r=f.mul(fp(0xb493f27e),s);
    r=f.add(fp(0x37d00d01),r);r=f.mul(s,r);r=f.add(fp(0xbab60b61),r);
    r=f.mul(s,r);r=f.add(fp(0x3d2aaaab),r);r=f.mul(s,r);
    r=f.add(-0.5f,r);r=f.mul(s,r);r=f.add(1,r);return f.mul(sign,r);
}
using Vec=std::array<float,3>;
inline float square(const Vec& a,Math& f) noexcept {
    const float x=f.mul(a[0],a[0]),y=f.mul(a[1],a[1]),z=f.mul(a[2],a[2]);
    return f.add(f.add(x,y),z);
}
inline std::array<float,16> axis(Vec a,float c,float s,Math& f) noexcept {
    const float n=square(a,f);
    if(n<=0){f.valid=false;return {};}// no fabricated zero-axis rotation
    const float q=f.div(1,f.root(n));for(float& v:a)v=f.mul(v,q);
    const float om=f.sub(1,c);Vec sa{{f.mul(a[0],s),f.mul(a[1],s),f.mul(a[2],s)}};
    std::array<float,16> r{};
    for(unsigned row=0;row<3;++row)for(unsigned lane=0;lane<3;++lane)
        r[row*4+lane]=f.mul(f.mul(a[lane],a[row]),om);
    r[0]=f.add(r[0],c);r[1]=f.add(r[1],sa[2]);r[2]=f.sub(r[2],sa[1]);
    r[4]=f.sub(r[4],sa[2]);r[5]=f.add(r[5],c);r[6]=f.add(r[6],sa[0]);
    r[8]=f.add(r[8],sa[1]);r[9]=f.sub(r[9],sa[0]);r[10]=f.add(r[10],c);r[15]=1;
    return r;
}
inline Vec point(const std::array<float,16>& m,const Vec& p,Math& f) noexcept {
    Vec out{};
    for(unsigned j=0;j<3;++j){float a=f.mul(m[j],p[0]);a=f.add(a,f.mul(m[4+j],p[1]));
        a=f.add(a,f.mul(m[8+j],p[2]));out[j]=f.add(a,m[12+j]);}
    return out;
}
}
// On unsupported numeric inputs, outputs remain unchanged (native atomic
// admission, not original partial-store behavior). On disabled source gate,
// updated=false and corners are untouched; no child/material selection occurs.
// Success with updated=true stops at 14B87C BEFORE material/141C30 service.
inline bool update14B2C0(const Inputs& in,Corners& out,bool& updated,std::uint32_t& pc) noexcept {
    pc=0x14b2e4;
    if(!in.enabled382B28){updated=false;pc=0x14b8b4;return true;}
    pc=0x14b320;
    if(!in.actualCamera||!in.actualPosition40||std::fegetround()!=FE_TONEAREST)return false;
    detail::Math f;
    const auto& cam=*in.actualCamera;const auto& v=cam.view;
    const detail::Vec p{{(*in.actualPosition40)[0],(*in.actualPosition40)[1],(*in.actualPosition40)[2]}};
    detail::Vec up{};
    up[2]=f.mul(in.size98,v[9]);up[1]=f.mul(in.size98,v[5]);up[0]=f.mul(in.size98,v[1]);
    detail::Vec a{{f.sub(p[0],cam.position.x),f.sub(p[1],cam.position.y),f.sub(p[2],cam.position.z)}};
    float factor=1;
    if(in.distance427040){
        pc=0x14b404;if(!in.distanceGlobals7050_7058)return false;
        const auto& g=*in.distanceGlobals7050_7058;
        const float len=f.root(detail::square(a,f));
        const float t=f.mul(g[0],len);
        const float q=f.div(g[1],f.root(t));factor=f.div(1,q);
    }
    detail::Vec right{};
    if(in.turning9C){
        pc=0x14b540;
        const auto projected=detail::point(cam.combined,p,f);
        pc=0x14ad10;const float angle=detail::angle(projected[1],projected[0],f);
        const float c=detail::cosine(angle,f),s=detail::sine(angle,f);
        pc=0x112c50;const auto rotation=detail::axis(a,c,s,f);
        up=detail::point(rotation,up,f);
        const detail::Vec forward{{v[2],v[6],v[10]}};
        for(unsigned i=0;i<3;++i){const unsigned j=(i+1)%3,k=(i+2)%3;
            right[i]=f.sub(f.mul(forward[j],up[k]),f.mul(up[j],forward[k]));}
    }else{
        pc=0x14b610;right[2]=f.mul(in.size98,v[8]);right[1]=f.mul(in.size98,v[4]);right[0]=f.mul(in.size98,v[0]);
    }
    if(!f.valid)return false;
    pc=0x14b6a4;Corners result{};detail::Vec delta{};
    for(unsigned i=0;i<3;++i)delta[i]=f.sub(right[i],up[i]);
    if(in.distance427040)for(float& d:delta)d=f.mul(d,factor);
    for(unsigned i=0;i<3;++i)result.field50[i]=f.sub(p[i],delta[i]);result.field50[3]=1;
    for(unsigned i=0;i<3;++i)result.field80[i]=f.add(p[i],delta[i]);result.field80[3]=1;
    for(unsigned i=0;i<3;++i)delta[i]=f.add(right[i],up[i]);
    if(in.distance427040)for(float& d:delta)d=f.mul(d,factor);
    for(unsigned i=0;i<3;++i)result.field60[i]=f.sub(p[i],delta[i]);result.field60[3]=1;
    for(unsigned i=0;i<3;++i)result.field70[i]=f.add(p[i],delta[i]);result.field70[3]=1;
    if(!f.valid)return false;
    out=result;updated=true;pc=0x14b87c;return true;
}
}
