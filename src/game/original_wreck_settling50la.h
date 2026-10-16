#pragma once
#include "body_integration48c.h"
#include "scalar_minmax48o.h"
#include "spring_records48n.h"
#include <cfenv>
#include <cmath>
#include <cstdint>

namespace rr::game {

// A borrowed view of ONE real PhysicsObject/PodPhysicsObject and its D0/DC.
// This descriptor owns no state and must not be retained across replacement of
// any canonical owner. The parameter/member names identify original fields.
struct OriginalWreckSettlingBinding50LA {
    const std::uint32_t& flight34;
    const std::uint32_t& flight38;
    const SpringRecordState48N& springDC;
    const BodyState48C& bodyD0;
    const float& bodyCachedSpeed34;
    bool& bodyEnabled204;
    float& contactTime12C;
    float& dragEC;
    float& rateDampingF0;
    const float& baseDrag100;
    const float& baseRateDamping104;
    const float& finalRateDamping108;
    const float& finalDrag10C;
    const float& changeWindow130;
    const float& stopSpeed134;
    const float& stopAngularSpeed138;
    const float& stopLinearAcceleration13C;
    const float& stopAngularAcceleration140;
    const float& measuredLinearAcceleration124;
    const float& measuredAngularAcceleration128;
};
enum class OriginalWreckSettlingStatus50LA : std::uint8_t {
    complete,roundingMode,input,arithmetic
};
struct OriginalWreckSettlingResult50LA {
    OriginalWreckSettlingStatus50LA status=OriginalWreckSettlingStatus50LA::input;
    std::uint32_t sourcePC=0;
};
namespace original_wreck_settling_detail50LA {
inline bool admitted(float value) noexcept {
    return std::isfinite(value)&&(value==0.f||std::isnormal(value));
}
inline float add(float a,float b) noexcept {volatile float r=a+b;return r;}
inline float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
inline float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
inline float divide(float a,float b) noexcept {volatile float r=a/b;return r;}
//1676EC..1677A4, repeated1677A8..167860. Preserve the separate operations,
// including2*window-window; the final clamp is source floating comparisons,
// deliberately NOT scalarMin/Max's signed-zero ordering.
inline bool interpolate(float time,float window,float first,float last,
    float& out) noexcept {
    const float end=mul(2.f,window);
    if(!admitted(end))return false;
    if(end==window){out=0.f;return true;}
    const float distance=sub(time,window);
    const float span=sub(end,window);
    const float delta=sub(last,first);
    if(!admitted(distance)||!admitted(span)||!admitted(delta)||span==0.f)return false;
    const float product=mul(distance,delta);
    if(!admitted(product))return false;
    const float fraction=divide(product,span);
    if(!admitted(fraction))return false;
    float value=add(first,fraction);
    if(!admitted(value))return false;
    if(last<=first){
        if(value<last)value=last;
        else if(!(value<=first))value=first;
    }else{
        if(!(value<=last))value=last;
        else if(value<first)value=first;
    }
    out=value;return true;
}
inline bool angularLength(const Vector3_48C& angular,float& out) noexcept {
    //16786C..167888: multiplyY,Y; multiplyX,X; ADDA(X²,Y²),
    // MADD(Z,Z), SQRT.S. This leaf admits only ordinary finite arithmetic.
    for(unsigned i=0;i<3;++i)if(!admitted(angular.v[i]))return false;
    const float yy=mul(angular.v[1],angular.v[1]);
    const float xx=mul(angular.v[0],angular.v[0]);
    const float xy=add(xx,yy);
    const float zz=mul(angular.v[2],angular.v[2]);
    const float squared=add(xy,zz);
    if(!admitted(yy)||!admitted(xx)||!admitted(xy)||!admitted(zz)||!admitted(squared))return false;
    volatile float root=static_cast<float>(std::sqrt(static_cast<double>(squared)));
    out=root;return admitted(out);
}
}

// Complete reached167640..167918 numerical/state leaf. Call at the actual
// PhysicsObject166E40/PhysicsObjectGroup2CA840 settling position, after their
// real detection, force/integration, scene/query updates and source124/128
// producers. This function does not claim any of those operations completed.
// The borrowed spring.contact3C and body cachedSpeed34 are used as they stand;
// neither is recomputed, defaulted, inferred from a miss, or replaced by a flag.
//
// It can only DISABLE the real body's existing enabled204 flag. Source1653C0
// with a1=0 stores204=0 then returns: no linked-body or component traversal is
// omitted. The enable1 path has a different linked-body268/28C/208 contract
// and is intentionally not implemented here. No model/activity29 is changed.
//
//130<=0 skips all damping/time accesses, but source always computes |B0| and
// performs the short-circuit settling comparisons. When130>0, time increases
// toward min(time+factor,2*130) only when either34 or38 is zero and actual
// spring.contact3C is nonzero; otherwise max(time-factor,+0) is used. Scalar
// MIN.S/MAX.S reuse48O including signed-zero ordering. Both damping lerps
// retain their ordinary comparison clamps and execute in F0-then-EC order.
//
// Disable requires strict '<' for cached speed, angular length and both source
// acceleration magnitudes, then a nonzero DC.contact3C. Failed earlier gates
// leave later thresholds/magnitudes unconsumed, even if those values are NaN.
// Body position/velocity/orientation/derivatives and old outputEC/F0 are never
// consumed. No private persistent copy of a body, force, or query is created.
//
// Native admission: FE_TONEAREST, normal-or-zero finite consumed operands and
// intermediates, strict binary32/no contraction/fast math. An error preserves
// time, both damping fields and enabled204. Source scalar operands are checked
// only at their reached reads. Output fields may not alias each other or any
// source field; bindings/backing remain alive and serially exclusive. No
// allocation, RNG, callbacks, effects, world query, registry or rendering.
inline OriginalWreckSettlingResult50LA settleOriginalWreckBody50LA(
    const OriginalWreckSettlingBinding50LA& b,float elapsedFactor) noexcept {
    using namespace original_wreck_settling_detail50LA;
    using S=OriginalWreckSettlingStatus50LA;
    if(std::fegetround()!=FE_TONEAREST)return {S::roundingMode,0x167640u};
    if(!admitted(b.changeWindow130))return {S::input,0x16764cu};
    const bool changeDamping=b.changeWindow130>0.f;
    float time=0.f,rate=0.f,drag=0.f;
    if(changeDamping){
        if(!admitted(b.contactTime12C)||!admitted(elapsedFactor))return {S::input,0x167688u};
        if((b.flight34==0u||b.flight38==0u)&&b.springDC.contact3C!=0u){
            const float increased=add(b.contactTime12C,elapsedFactor);
            const float maximum=mul(2.f,b.changeWindow130);
            if(!admitted(increased)||!admitted(maximum))return {S::arithmetic,0x167698u};
            time=detail::scalarMin48O(increased,maximum);
        }else{
            const float decreased=sub(b.contactTime12C,elapsedFactor);
            if(!admitted(decreased))return {S::arithmetic,0x1676d0u};
            time=detail::scalarMax48O(decreased,0.f);
        }
        if(!admitted(b.baseRateDamping104)||!admitted(b.finalRateDamping108))
            return {S::input,0x1676f8u};
        if(!interpolate(time,b.changeWindow130,b.baseRateDamping104,b.finalRateDamping108,rate))
            return {S::arithmetic,0x167700u};
        if(!admitted(b.baseDrag100)||!admitted(b.finalDrag10C))return {S::input,0x1677b4u};
        if(!interpolate(time,b.changeWindow130,b.baseDrag100,b.finalDrag10C,drag))
            return {S::arithmetic,0x1677bcu};
    }
    float angularSpeed=0.f;
    if(!angularLength(b.bodyD0.angularVelocity,angularSpeed))return {S::arithmetic,0x16786cu};
    if(!admitted(b.bodyCachedSpeed34)||!admitted(b.stopSpeed134))return {S::input,0x167894u};
    bool disable=false;
    if(b.bodyCachedSpeed34<b.stopSpeed134){
        if(!admitted(b.stopAngularSpeed138))return {S::input,0x1678acu};
        if(angularSpeed<b.stopAngularSpeed138){
            if(!admitted(b.measuredLinearAcceleration124)||!admitted(b.stopLinearAcceleration13C))
                return {S::input,0x1678c0u};
            if(b.measuredLinearAcceleration124<b.stopLinearAcceleration13C){
                if(!admitted(b.measuredAngularAcceleration128)||!admitted(b.stopAngularAcceleration140))
                    return {S::input,0x1678d8u};
                if(b.measuredAngularAcceleration128<b.stopAngularAcceleration140)
                    disable=b.springDC.contact3C!=0u;
            }
        }
    }
    if(changeDamping){
        b.contactTime12C=time;b.rateDampingF0=rate;b.dragEC=drag;
    }
    if(disable)b.bodyEnabled204=false; //1653C0 a1=0, exact store-only branch
    return {S::complete,0x167910u};
}
}
