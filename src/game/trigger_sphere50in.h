#pragma once
#include "collection_bounds48s.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

namespace rr::game {
// Source shape5 payload: local point0, matrix10, radius50/radiusSquared54,
// radiusScale58 and centerScale5C. Static11C0B0 initializes both scales to1
// and matrix10 to identity. No implicit cached-center substitution.
struct TriggerSphereShape50IN {
    SpringVector4_48N localCenter{};
    BodyMatrix48G world{};
    float radius50=0.f,radiusSquared54=0.f;
    float radiusScale58=1.f,centerScale5C=1.f;
};
struct TriggerSphereBoundsInput50IN {
    TriggerSphereShape50IN sphere{};
    SpringVector4_48N cachedCenter60{};
    float cachedRadius80=0.f;
};
struct TriggerSphereRootExitInput50IN {
    TriggerSphereShape50IN sphere{};
    BodyMatrix48G type2PreparedWorld{};
    SpringVector4_48N rootCenter{},rootHalf{};
};
enum class TriggerSphereRootExitStatus50IN { Complete,InvalidInput,NonFinite };

namespace trigger_sphere50in_detail {
inline float add(float a,float b) noexcept {volatile float r=a+b;return r;}
inline float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
inline float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
inline bool finite3(const SpringVector4_48N& p) noexcept {
    return std::isfinite(p.v[0])&&std::isfinite(p.v[1])&&std::isfinite(p.v[2]);
}
inline bool affine(const BodyMatrix48G& m) noexcept {
    for(float f:m.v)if(!std::isfinite(f))return false;
    return m.v[3]==0.f&&m.v[7]==0.f&&m.v[11]==0.f&&m.v[15]==1.f;
}
inline bool disjoint(const void* a,std::size_t an,const void* b,std::size_t bn) noexcept {
    const auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);
    const auto max=std::numeric_limits<std::uintptr_t>::max();
    return an<=max-x&&bn<=max-y&&(x+an<=y||y+bn<=x);
}
inline float point(const BodyMatrix48G& m,const SpringVector4_48N& p,unsigned k) noexcept {
    return add(add(add(mul(m.v[k],p.v[0]),mul(m.v[4+k],p.v[1])),mul(m.v[8+k],p.v[2])),m.v[12+k]);
}
}

// Exact finite-affine shape5 projection1208E4..1209A4 into the already-recovered
// common120C4C bounds tail. Cached60/80 bits are preserved. This does not refresh
// scene or cached sphere, register ownership, perform a hit query, or mutate
// source state. Result must be disjoint from the whole input; all rejections
// preserve every result byte. Uses existing48S implementation, no allocation.
inline CollectionBoundsStatus48S prepareTriggerSphereBounds50IN(
    const TriggerSphereBoundsInput50IN& in,CollectionBoundsResult48S& out) noexcept {
    using S=CollectionBoundsStatus48S;
    using namespace trigger_sphere50in_detail;
    if(!disjoint(&in,sizeof in,&out,sizeof out)||std::fegetround()!=FE_TONEAREST)
        return S::InvalidInput;
    const auto& s=in.sphere;
    if(!finite3(s.localCenter)||!affine(s.world)||!std::isfinite(s.radius50)||
       !std::isfinite(s.radiusScale58)||!std::isfinite(s.centerScale5C))return S::NonFinite;
    CollectionBoundsInput48S p{};p.shapeTag=1;
    std::memcpy(&p.shapeWorld,&s.world,sizeof p.shapeWorld);
    std::memcpy(&p.cachedCenter,&in.cachedCenter60,sizeof p.cachedCenter);
    std::memcpy(&p.cachedRadius,&in.cachedRadius80,sizeof p.cachedRadius);
    const float radius=mul(s.radius50,s.radiusScale58);
    for(unsigned k=0;k<3;++k){
        p.localCenter.v[k]=mul(s.localCenter.v[k],s.centerScale5C);
        p.localHalf.v[k]=mul(radius,1.f); // explicit source unit-vector multiply
    }
    p.localCenter.v[3]=p.localHalf.v[3]=1.f;
    return prepareCollectionBounds48S(p,out);
}

// ONLY the recovered lifecycle coarse overlap:2A0B30 ->122BA0 ->106B50 for
// sphere5 and a type2 with a nonnull root-node tree. rootCenter/rootHalf must be
// that query's current root geometry. This is NOT121410/108E90 entry-hit parity.
// Uses sphere point transformed by its matrix10, unscaled radius50 and stored
// radiusSquared54, then TRANSPOSE of query basis after translation subtraction.
// No normalization/general inverse. Expanded-box checks are inclusive, final
// squared-distance comparison is strict (tangent excluded). Finite affine
// domain, nonnegative half/radius, explicit squared radius, FE_TONEAREST and
// separately-rounded binary32/no contraction required. No allocations or
// retained pointers. Unsupported leaf-format state is never read here.
inline TriggerSphereRootExitStatus50IN overlapTriggerSphereType2Root50IN(
    const TriggerSphereRootExitInput50IN& in,bool& overlapOut) noexcept {
    using S=TriggerSphereRootExitStatus50IN;
    using namespace trigger_sphere50in_detail;
    if(!disjoint(&in,sizeof in,&overlapOut,sizeof overlapOut)||std::fegetround()!=FE_TONEAREST)
        return S::InvalidInput;
    const auto& s=in.sphere;const auto& m=in.type2PreparedWorld;
    if(!finite3(s.localCenter)||!finite3(in.rootCenter)||!finite3(in.rootHalf)||
       !affine(s.world)||!affine(m)||!std::isfinite(s.radius50)||
       !std::isfinite(s.radiusSquared54))return S::NonFinite;
    if(s.radius50<0.f||s.radiusSquared54<0.f)return S::InvalidInput;
    for(unsigned k=0;k<3;++k)if(in.rootHalf.v[k]<0.f)return S::InvalidInput;
    SpringVector4_48N worldPoint{},translated{};
    for(unsigned k=0;k<3;++k){
        worldPoint.v[k]=point(s.world,s.localCenter,k);
        translated.v[k]=sub(worldPoint.v[k],add(m.v[12+k],0.f));
        if(!std::isfinite(worldPoint.v[k])||!std::isfinite(translated.v[k]))return S::NonFinite;
    }
    float residual[3]{};bool separated=false;
    for(unsigned k=0;k<3;++k){
        const float local=add(add(mul(add(m.v[k*4],0.f),translated.v[0]),
            mul(add(m.v[k*4+1],0.f),translated.v[1])),
            mul(add(m.v[k*4+2],0.f),translated.v[2]));
        const float distance=std::fabs(sub(local,in.rootCenter.v[k]));
        const float expandedGap=sub(sub(distance,in.rootHalf.v[k]),s.radius50);
        if(!std::isfinite(local)||!std::isfinite(distance)||!std::isfinite(expandedGap))return S::NonFinite;
        separated=separated||!(expandedGap<=0.f);
        const float closest=distance<in.rootHalf.v[k]?distance:in.rootHalf.v[k];
        residual[k]=sub(distance,closest);
    }
    if(separated){overlapOut=false;return S::Complete;}
    const float squared=add(add(mul(residual[0],residual[0]),mul(residual[1],residual[1])),
        mul(residual[2],residual[2]));
    if(!std::isfinite(squared))return S::NonFinite;
    overlapOut=!(s.radiusSquared54<=squared);return S::Complete;
}
}
