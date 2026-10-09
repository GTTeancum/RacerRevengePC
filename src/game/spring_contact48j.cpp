#include "spring_contact48j.h"
#include <cmath>
#include <cstdint>
#include <cstring>

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
    float divide(float a,float b) noexcept {volatile float r=a/b;return keep(r);}
    float dot(const V& a,const V& b) noexcept {return add(add(mul(a.v[0],b.v[0]),mul(a.v[1],b.v[1])),mul(a.v[2],b.v[2]));}
    V inverse(const BodyMatrix48G& m,const V& v,bool point) noexcept {
        V input=v,out{};
        if(point)for(unsigned i=0;i<3;++i)input.v[i]=sub(input.v[i],add(m.v[12+i],0));
        for(unsigned i=0;i<3;++i)out.v[i]=add(add(mul(add(m.v[4*i],0),input.v[0]),mul(add(m.v[4*i+1],0),input.v[1])),mul(add(m.v[4*i+2],0),input.v[2]));
        return out;
    }
};
}
bool addSpringPointForce48J(const BodyMatrix48G& scene,const BodyLoads48C& input,
    const SpringPointForceParameters48J& parameters,const V& force,const V& point,
    BodyLoads48C& output) noexcept {
    if(!finite(input.force)||!finite(input.torque)||!finite(force)||!finite(point))return false;
    Math math;BodyLoads48C out=input;
    for(unsigned i=0;i<3;++i)out.force.v[i]=math.add(input.force.v[i],force.v[i]);
    if(point.v[0]!=0 || point.v[1]!=0 || point.v[2]!=0) {
        const auto& center=parameters.useCustomCenter?parameters.customLocalCenter:parameters.primaryLocalCenter;
        if(!affine(scene)||!finite(center)||!std::isfinite(parameters.torqueScale))return false;
        const auto localForce=math.inverse(scene,force,false);
        auto arm=math.inverse(scene,point,true);
        for(unsigned i=0;i<3;++i)arm.v[i]=math.sub(arm.v[i],center.v[i]);
        for(unsigned i=0;i<3;++i) {
            float torque=math.sub(math.mul(arm.v[(i+1)%3],localForce.v[(i+2)%3]),math.mul(localForce.v[(i+1)%3],arm.v[(i+2)%3]));
            if(parameters.yawTorqueOnly && i!=1)torque=0;
            out.torque.v[i]=math.add(input.torque.v[i],math.mul(parameters.torqueScale,torque));
        }
    }
    if(!math.valid)return false;
    output=out;return true;
}
bool applySpringFriction48J(const V& normal,const V& point,const V& velocity,float coefficient,
    const SpringPointForceParameters48J& parameters,const SpringFrictionBody48J& primary,
    const SpringFrictionBody48J* other,SpringFrictionResult48J& output) noexcept {
    if(!finite(normal)||!finite(velocity)||!std::isfinite(coefficient)||!std::isfinite(primary.mass))return false;
    Math math;const float projection=math.dot(velocity,normal);V tangent{},direction{};
    for(unsigned i=0;i<3;++i)tangent.v[i]=math.sub(velocity.v[i],math.mul(projection,normal.v[i]));
    const float square=math.dot(tangent,tangent);
    float reciprocal;
    if(square==0) {const std::uint32_t largest=0x7f7fffffu;std::memcpy(&reciprocal,&largest,4);}
    else {volatile float root=static_cast<float>(std::sqrt(static_cast<double>(square)));reciprocal=math.divide(1,root);}
    for(unsigned i=0;i<3;++i)direction.v[i]=-math.mul(tangent.v[i],reciprocal);
    float length=0;
    if(square!=0) {volatile float scalarReciprocal=static_cast<float>(1.0/std::sqrt(static_cast<double>(square)));length=math.divide(1,scalarReciprocal);}
    const float scale=math.mul(.1f,length);
    const float magnitude=math.mul(32.2f,math.mul(primary.mass,math.mul(scale<1?scale:1,coefficient)));
    SpringFrictionResult48J result{};
    for(unsigned i=0;i<3;++i)result.worldForce.v[i]=math.mul(magnitude,direction.v[i]);
    if(!math.valid||!addSpringPointForce48J(primary.scene,primary.loads,parameters,result.worldForce,point,result.primary))return false;
    if(other) {
        V opposite{};for(unsigned i=0;i<3;++i)opposite.v[i]=-result.worldForce.v[i];
        if(!addSpringPointForce48J(other->scene,other->loads,parameters,opposite,point,result.other))return false;
    }
    output=result;return true;
}
}
