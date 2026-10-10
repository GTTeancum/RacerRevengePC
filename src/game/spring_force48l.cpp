#include "spring_force48l.h"
#include "scalar_minmax48o.h"
#include <cmath>

namespace rr::game {
namespace {
using V=Vector3_48C;
bool finite(const V& v) noexcept {for(float x:v.v)if(!std::isfinite(x))return false;return true;}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {volatile float r=a*b;return keep(r);}
    float dot(const V& a,const V& b) noexcept {
        return add(add(mul(a.v[0],b.v[0]),mul(a.v[1],b.v[1])),mul(a.v[2],b.v[2]));
    }
};
}
bool applySpringForce48L(const V& normal,const V& point,float penetration,
    const SpringForceParameters48L& p,const SpringImpactBody48K& primary,
    const SpringImpactBody48K* other,SpringFrictionResult48J& output) noexcept {
    if(other==&primary||!finite(normal)||!finite(p.contactVelocity120)||
       !std::isfinite(penetration)||!std::isfinite(p.maxPenetration90)||!std::isfinite(p.maxSpeed94))return false;
    const bool impact=p.impact54&&(!other||!p.otherSpring58);
    const float stiffness=impact?p.impactStiffnessA4:p.stiffness9C;
    const float damping=impact?p.impactDampingA8:p.dampingA0;
    if(!std::isfinite(stiffness)||!std::isfinite(damping))return false;
    Math math;float ratio=1;
    if(other) {
        if(!std::isfinite(primary.parameters.mass)||!std::isfinite(other->parameters.mass))return false;
        if(primary.parameters.mass<=other->parameters.mass) {
            if(!std::isfinite(other->parameters.inverseMass))return false;
            ratio=math.mul(primary.parameters.mass,other->parameters.inverseMass);
        } else {
            if(!std::isfinite(primary.parameters.inverseMass))return false;
            ratio=math.mul(primary.parameters.inverseMass,other->parameters.mass);
        }
    }
    // Scalar MIN.S/MAX.S use signed-word ordering, including -0 below +0.
    // This is an upper-only penetration clamp and a two-sided speed
    // clamp, followed by a nonnegative magnitude clamp, in original order.
    const float depth=detail::scalarMin48O(penetration,p.maxPenetration90);
    const float spring=math.mul(stiffness,depth);
    const float speed=math.dot(p.contactVelocity120,normal);
    const float lower=detail::scalarMax48O(speed,-p.maxSpeed94);
    const float clamped=detail::scalarMin48O(lower,p.maxSpeed94);
    const float damped=math.sub(spring,math.mul(damping,clamped));
    const float magnitude=math.mul(ratio,detail::scalarMax48O(damped,0.f));
    SpringFrictionResult48J result{};
    for(unsigned i=0;i<3;++i)result.worldForce.v[i]=math.mul(magnitude,normal.v[i]);
    if(!math.valid||!addSpringPointForce48J(primary.state.scene,primary.state.loads,
        p.pointForce,result.worldForce,point,result.primary))return false;
    if(other) {
        V opposite{};for(unsigned i=0;i<3;++i)opposite.v[i]=-result.worldForce.v[i];
        if(!addSpringPointForce48J(other->state.scene,other->state.loads,
            p.pointForce,opposite,point,result.other))return false;
    }
    output=result;return true;
}
bool removeSpringInwardVelocity48L(const V& normal,const V& cached,
    const V& primaryVelocity,V& output) noexcept {
    if(!finite(normal)||!finite(cached)||!finite(primaryVelocity))return false;
    Math math;const float speed=math.dot(cached,normal);
    const float correction=-detail::scalarMin48O(speed,0.f);
    V result{};
    for(unsigned i=0;i<3;++i)result.v[i]=math.add(primaryVelocity.v[i],math.mul(correction,normal.v[i]));
    if(!math.valid)return false;
    output=result;return true;
}
}
