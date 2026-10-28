#include "spring_impulse48k.h"
#include "spring_contact48j.h"
#include <cmath>
#include <limits>

namespace rr::game {
namespace {
using V=Vector3_48C;
bool finite(const V& v) noexcept {for(float x:v.v)if(!std::isfinite(x))return false;return true;}
bool affine(const BodyMatrix48G& m) noexcept {
    for(float x:m.v)if(!std::isfinite(x))return false;
    return m.v[3]==0 && m.v[7]==0 && m.v[11]==0 && m.v[15]==1;
}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {volatile float r=a*b;return keep(r);}
    float div(float a,float b) noexcept {volatile float r=a/b;return keep(r);}
    V sum(const V& a,const V& b) noexcept {V r{};for(unsigned i=0;i<3;++i)r.v[i]=add(a.v[i],b.v[i]);return r;}
    V difference(const V& a,const V& b) noexcept {V r{};for(unsigned i=0;i<3;++i)r.v[i]=sub(a.v[i],b.v[i]);return r;}
    V scale(float s,const V& a) noexcept {V r{};for(unsigned i=0;i<3;++i)r.v[i]=mul(s,a.v[i]);return r;}
    V cross(const V& a,const V& b) noexcept {
        V r{};for(unsigned i=0;i<3;++i)r.v[i]=sub(mul(a.v[(i+1)%3],b.v[(i+2)%3]),mul(b.v[(i+1)%3],a.v[(i+2)%3]));return r;
    }
    float dot(const V& a,const V& b) noexcept {return add(add(mul(a.v[0],b.v[0]),mul(a.v[1],b.v[1])),mul(a.v[2],b.v[2]));}
    V inverse(const BodyMatrix48G& m,const V& a,bool helper=false) noexcept {
        V r{};for(unsigned i=0;i<3;++i){r.v[i]=add(add(mul(add(m.v[4*i],0),a.v[0]),mul(add(m.v[4*i+1],0),a.v[1])),mul(add(m.v[4*i+2],0),a.v[2]));if(helper)r.v[i]=add(r.v[i],0);}
        return r;
    }
    V transform(const BodyMatrix48G& m,const V& a,bool point=false) noexcept {
        V r{};for(unsigned i=0;i<3;++i)r.v[i]=add(add(add(mul(m.v[i],a.v[0]),mul(m.v[4+i],a.v[1])),mul(m.v[8+i],a.v[2])),point?m.v[12+i]:0);
        return r;
    }
    V inertia(const BodyParameters48C& p,const V& a) noexcept {
        V r{};for(unsigned i=0;i<3;++i)r.v[i]=add(add(add(mul(p.inverseInertiaColumns[0].v[i],a.v[0]),mul(p.inverseInertiaColumns[1].v[i],a.v[1])),mul(p.inverseInertiaColumns[2].v[i],a.v[2])),0);
        return r;
    }
    V correction(const V& old,const V& incoming) noexcept {
        const float square=dot(old,old);
        volatile float root=static_cast<float>(std::sqrt(static_cast<double>(square)));
        const float q=square==0?std::numeric_limits<float>::max():div(1,root);
        const V unit=scale(q,old);const float projection=dot(incoming,unit);
        const V perpendicular=difference(incoming,scale(projection,unit));
        const float excess=sub(projection,root);
        return sum(old,sum(perpendicular,scale(excess>0?excess:0,unit)));
    }
};
V yaw(V v,bool only) noexcept {if(only)v.v[0]=v.v[2]=0;return v;}
bool bodyFinite(const SpringImpactBody48K& b) noexcept {
    if(!affine(b.state.scene)||!std::isfinite(b.parameters.inverseMass))return false;
    for(const auto& c:b.parameters.inverseInertiaColumns)if(!finite(c))return false;
    return true;
}
}
bool prepareSpringImpactImpulsePrefix48K(const V& normal,const V& point,
    const SpringImpactParameters48K& p,const SpringImpactBody48K& primary,
    const SpringImpactBody48K* other,SpringImpactImpulsePrefix48K& output)noexcept {
    if(!finite(normal)||!finite(point)||!finite(p.contactVelocity120)||!bodyFinite(primary)||other==&primary)return false;
    if(other&&(!bodyFinite(*other)||!std::isfinite(other->parameters.mass)))return false;
    Math m;
    const float restitution=other?(other->parameters.mass>1000?.25f:p.restitutionC0):(normal.v[1]>.8f?p.restitutionBC:p.restitutionB8);
    V arm{},otherArm{};
    if(!p.disableLeverArms5C){
        arm=m.inverse(primary.state.scene,m.difference(point,p.worldCenter110));
        if(other)otherArm=m.inverse(other->state.scene,m.difference(point,m.transform(other->state.scene,other->localCenter,true)));
    }
    const auto localNormal=m.inverse(primary.state.scene,normal);
    const float primaryRot=m.dot(localNormal,m.cross(m.inertia(primary.parameters,m.cross(arm,localNormal)),arm));
    float otherRot=0,otherInv=0;
    if(other){
        const auto localOtherNormal=m.inverse(other->state.scene,normal);otherInv=other->parameters.inverseMass;
        otherRot=m.dot(localOtherNormal,m.cross(m.inertia(other->parameters,m.cross(otherArm,localOtherNormal)),otherArm));
    }
    const float effective=m.div(1,m.add(otherRot,m.add(otherInv,m.add(primary.parameters.inverseMass,primaryRot))));
    const float normalSpeed=m.dot(p.contactVelocity120,normal);
    SpringImpactImpulsePrefix48K result{};
    result.arm=arm;result.otherArm=otherArm;result.effectiveMass=effective;
    result.normalSpeed=normalSpeed;result.inward=normalSpeed<0;
    if(result.inward)result.impulse=m.scale(m.mul(effective,m.mul(-m.add(1,restitution),normalSpeed)),normal);
    if(!m.valid)return false;output=result;return true;
}
bool prepareSpringImpactAngularPrefix48K(const V& arm,const V& localImpulse,
    const BodyParameters48C& parameters,float scale,bool yawOnly,V& output)noexcept {
    if(!finite(arm)||!finite(localImpulse)||!std::isfinite(scale))return false;
    for(const auto& column:parameters.inverseInertiaColumns)if(!finite(column))return false;
    Math m;const auto value=m.scale(scale,yaw(m.inertia(parameters,m.cross(arm,localImpulse)),yawOnly));
    if(!m.valid)return false;output=value;return true;
}
bool prepareSpringImpactTorqueArm48K(const BodyParameters48C& parameters,const V& torque,
    const V& arm,V& output)noexcept {
    if(!finite(torque)||!finite(arm))return false;
    for(const auto& column:parameters.inverseInertiaColumns)if(!finite(column))return false;
    Math m;const auto value=m.cross(m.inertia(parameters,torque),arm);
    if(!m.valid)return false;output=value;return true;
}
bool prepareSpringCorrectionDelta48K(const V& current,const V& incoming,V& output)noexcept {
    if(!finite(current)||!finite(incoming))return false;
    Math m;const float square=m.dot(current,current);
    volatile float root=static_cast<float>(std::sqrt(static_cast<double>(square)));
    const float q=square==0?std::numeric_limits<float>::max():m.div(1,root);
    const V unit=m.scale(q,current);const float projection=m.dot(incoming,unit);
    const V perpendicular=m.difference(incoming,m.scale(projection,unit));
    const float excess=m.sub(projection,root);
    const auto delta=m.sum(perpendicular,m.scale(excess>0?excess:0,unit));
    if(!m.valid)return false;output=delta;return true;
}
bool applySpringImpact48K(const V& normal,const V& point,float penetration,
    const SpringImpactParameters48K& p,const SpringImpactBody48K& primary,
    const SpringImpactBody48K* other,SpringImpactResult48K& output) noexcept {
    if(!finite(normal)||!finite(point)||!finite(p.contactVelocity120)||!bodyFinite(primary)||other==&primary)return false;
    if(other&&(!bodyFinite(*other)||!std::isfinite(other->parameters.mass)))return false;
    Math m;SpringImpactResult48K r{};r.primary=primary.state;if(other)r.other=other->state;
    const float restitution=other?(other->parameters.mass>1000?.25f:p.restitutionC0):(normal.v[1]>.8f?p.restitutionBC:p.restitutionB8);
    V arm{},otherArm{};
    if(!p.disableLeverArms5C){
        arm=m.inverse(primary.state.scene,m.difference(point,p.worldCenter110));
        if(other)otherArm=m.inverse(other->state.scene,m.difference(point,m.transform(other->state.scene,other->localCenter,true)));
    }
    const auto localNormal=m.inverse(primary.state.scene,normal);
    const float primaryRot=m.dot(localNormal,m.cross(m.inertia(primary.parameters,m.cross(arm,localNormal)),arm));
    float otherRot=0,otherInv=0;
    if(other){
        const auto localOtherNormal=m.inverse(other->state.scene,normal);otherInv=other->parameters.inverseMass;
        otherRot=m.dot(localOtherNormal,m.cross(m.inertia(other->parameters,m.cross(otherArm,localOtherNormal)),otherArm));
    }
    const float effective=m.div(1,m.add(otherRot,m.add(otherInv,m.add(primary.parameters.inverseMass,primaryRot))));
    const float normalSpeed=m.dot(p.contactVelocity120,normal);
    if(normalSpeed<0){
        const auto impulse=m.scale(m.mul(effective,m.mul(-m.add(1,restitution),normalSpeed)),normal);
        const auto delta=m.scale(primary.parameters.inverseMass,impulse);
        r.primary.accumulator1c0=m.sum(r.primary.accumulator1c0,delta);r.primary.body.velocity=m.sum(r.primary.body.velocity,delta);
        const auto angular=m.scale(p.angularImpulseScaleB0,yaw(m.inertia(primary.parameters,m.cross(arm,m.inverse(primary.state.scene,impulse,true))),p.yawOnly6C));
        r.primary.accumulator1d0=m.sum(r.primary.accumulator1d0,angular);r.primary.body.angularVelocity=m.sum(r.primary.body.angularVelocity,angular);
        if(other){
            V negative{};for(unsigned i=0;i<3;++i)negative.v[i]=-impulse.v[i];
            const auto od=m.scale(otherInv,negative);
            r.other.accumulator1c0=m.sum(r.other.accumulator1c0,od);r.other.body.velocity=m.sum(r.other.body.velocity,od);
            const auto oa=m.scale(-p.angularImpulseScaleB0,yaw(m.inertia(other->parameters,m.cross(otherArm,m.inverse(other->state.scene,impulse,true))),p.yawOnly6C));
            r.other.accumulator1d0=m.sum(r.other.accumulator1d0,oa);r.other.body.angularVelocity=m.sum(r.other.body.angularVelocity,oa);
        }
    }
    const auto acceleration=m.sum(m.scale(primary.parameters.inverseMass,r.primary.loads.force),m.transform(primary.state.scene,m.cross(m.inertia(primary.parameters,r.primary.loads.torque),arm)));
    V otherAcceleration{};
    if(other)otherAcceleration=m.sum(other->state.body.linearAcceleration,m.transform(other->state.scene,m.cross(other->state.body.angularAcceleration,otherArm)));
    const float normalAcceleration=m.dot(m.difference(acceleration,otherAcceleration),normal);
    if(normalAcceleration<0){
        const auto force=m.scale(m.mul(effective,m.mul(-p.normalForceScale98,normalAcceleration)),normal);
        SpringPointForceParameters48J params{primary.localCenter,p.customLocalCenter100,p.forceTorqueScaleB4,p.customCenter70,p.yawOnly6C};
        if(!addSpringPointForce48J(primary.state.scene,r.primary.loads,params,force,point,r.primary.loads))return false;
    }
    if(p.correctPenetration64){
        if(!std::isfinite(penetration))return false;
        if(penetration>0){
            const auto impulse=m.scale(m.mul(penetration,effective),normal);
            const auto delta=m.scale(primary.parameters.inverseMass,m.scale(p.positionCorrectionScaleCC,impulse));
            r.primary.positionCorrection1a0=m.correction(r.primary.positionCorrection1a0,delta);
            const auto angular=yaw(m.scale(p.angularCorrectionScaleD0,m.inertia(primary.parameters,m.cross(arm,m.inverse(primary.state.scene,impulse,true)))),p.yawOnly6C);
            r.primary.rotationCorrection1b0=m.correction(r.primary.rotationCorrection1b0,angular);
        }
    }
    if(!m.valid)return false;
    output=r;return true;
}
}
