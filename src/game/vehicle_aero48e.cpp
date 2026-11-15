#include "vehicle_aero48e.h"
#include "vehicle_aero_untilted50zg.h"
#include <cmath>
#include <cstdint>
#include <cstring>

namespace rr::game {
namespace {
std::uint32_t bits(float x) noexcept { std::uint32_t b; std::memcpy(&b,&x,4); return b; }
float fp(std::uint32_t b) noexcept { float x; std::memcpy(&x,&b,4); return x; }
struct Arithmetic {
    bool valid=true;
    float keep(float x) noexcept { valid=valid && std::isfinite(x);return x; }
    float add(float a,float b) noexcept {volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {volatile float r=a*b;return keep(r);}
    float div(float a,float b) noexcept {volatile float r=a/b;return keep(r);}
    float root(float x) noexcept {
        if((bits(x)&0x7f800000u)==0)return fp(bits(x)&0x80000000u);
        volatile float r=static_cast<float>(std::sqrt(static_cast<double>(x)));return keep(r);
    }
    float reciprocalRoot(float x) noexcept {
        if((bits(x)&0x7f800000u)==0)return fp(0x7f7fffffu);
        volatile float r=static_cast<float>(1.0/std::sqrt(static_cast<double>(x)));
        if((bits(r)&0x7f800000u)==0)return 0;return keep(r);
    }
};
bool finite(const Vector3_48C& v) noexcept {for(float x:v.v)if(!std::isfinite(x))return false;return true;}
// Supplied PCSX2 FPU.cpp:92-100 orders signed zero by its sign bit. Ordinary
// finite comparisons agree; avoid inheriting std::min/max's equal-zero tie.
float minimum(float a,float b) noexcept {return a==0 && b==0 ? fp(bits(a)|bits(b)):(a<b ? a:b);}
float maximum(float a,float b) noexcept {return a==0 && b==0 ? fp(bits(a)&bits(b)):(a>b ? a:b);}
float angle(float x,Arithmetic& f) noexcept {
    const float a=std::fabs(x),c=fp(0x3e8f5c29u);
    const float y=a<=1 ? f.div(a,f.add(1,f.mul(f.mul(c,a),a)))
                       : f.sub(fp(0x3fc90fdbu),f.div(a,f.add(c,f.mul(a,a))));
    return x<0 ? -y:y;
}
// Original repeated bounded interpolation sequence. Endpoint order matters.
float interpolate(float x,float low,float high,float first,float last,Arithmetic& f) noexcept {
    if(high==low)return low;
    const float v=f.add(first,f.div(f.mul(f.sub(x,low),f.sub(last,first)),f.sub(high,low)));
    if(!(last<=first)) {if(!(v<=last))return last;if(v<first)return first;}
    else {if(v<last)return last;if(!(v<=first))return first;}
    return v;
}
Vector3_48C transform(const ReadyBodyBasis48D& b,const Vector3_48C& v,Arithmetic& f) noexcept {
    Vector3_48C out{};
    for(unsigned i=0;i<3;++i)out.v[i]=f.add(f.add(f.add(f.mul(b.columns[0].v[i],v.v[0]),
        f.mul(b.columns[1].v[i],v.v[1])),f.mul(b.columns[2].v[i],v.v[2])),0);
    return out;
}
Vector3_48C inverseDirection(const ReadyBodyBasis48D& b,const Vector3_48C& v,Arithmetic& f) noexcept {
    ReadyBodyBasis48D t{};
    const float zero=f.sub(0,0); // VU subtract-self and transpose add-zero order.
    for(unsigned c=0;c<3;++c)for(unsigned r=0;r<3;++r)
        t.columns[c].v[r]=c==r ? f.add(b.columns[r].v[c],zero):f.add(0,b.columns[r].v[c]);
    return transform(t,v,f);
}
void calculateForce(Arithmetic& f,const AeroMotionPrefix48E& motion,
    const AeroParameters48E& p,const AeroInput48E& in,const AeroState48E& state,
    float up,bool firstTimer,bool secondTimer,bool tilted,AeroForcePrefix48E& out) noexcept {
    const auto& v=motion.velocity;const float speed=motion.speed,pitch=motion.pitch;
    const float factor=motion.factor,signedFactor=motion.signedFactor,speed2=motion.speedSquared;
    const float steeringScale=in.fullSteeringE0 ? 1.0f:.5f;
    float yaw=0,steering=0,extraSteering=0,extraDrag=0,lateral=0;
    if(state.lowSpeedDc) {
        float scale=f.add(1.5f,f.div(f.mul(f.sub(speed,25),-1.5f),25));
        if(scale<0)scale=0;else if(!(scale<=1.5f))scale=1.5f;
        steering=f.mul(p.steering100,scale);lateral=p.lateral90;
    } else if(firstTimer) {yaw=p.timerYawB8;lateral=p.lateral90;}
    else if(secondTimer) {
        steering=p.steering100;
        lateral=interpolate(state.timer144,0,4,p.lateral90,p.timerLateral10c,f);
        extraSteering=interpolate(state.timer144,0,4,0,p.timerYaw110,f);
        extraDrag=interpolate(state.timer144,0,4,0,f.mul(p.timerDrag114,std::fabs(v.v[0])),f);
    } else if(tilted) {
        lateral=interpolate(up,.5f,0,p.lateral90,p.tiltedLateral118,f);
        yaw=p.tiltedYaw120;
    } else {steering=p.steering100;lateral=p.lateral90;}
    auto& force=out.localForce;auto& torque=out.directTorque;
    force.v[0]=f.add(f.mul(factor,f.mul(p.velocityDamping130.v[0],v.v[0])),f.mul(lateral,v.v[0]));
    force.v[1]=f.add(f.mul(speed2,f.mul(p.lift6c,pitch)),f.mul(factor,f.mul(p.velocityDamping130.v[1],v.v[1])));
    float z=f.add(extraDrag,f.mul(factor,f.mul(p.velocityDamping130.v[2],v.v[2])));
    z=f.sub(z,f.mul(signedFactor,f.mul(p.driveDragF8,f.add(in.driveE4,in.driveE8))));
    z=f.sub(z,f.mul(signedFactor,f.mul(p.brakeDragFc,in.brakeEc)));
    force.v[2]=f.sub(z,f.mul(f.mul(p.drag78,speed2),v.v[2]<0 ? -1.0f:1.0f));
    torque.v[0]=f.add(f.mul(p.pitchDamping88,in.angularVelocity.v[0]),
        f.mul(f.mul(p.pitch84,f.add(pitch,p.offset68)),maximum(signedFactor,0)));
    torque.v[1]=f.add(f.mul(p.yawDampingC8,in.angularVelocity.v[1]),f.mul(yaw,v.v[0]));
    torque.v[2]=f.mul(p.rollDampingB0,in.angularVelocity.v[2]);
    if(state.lowSpeedDc)torque.v[1]=f.add(torque.v[1],f.mul(steering,f.sub(in.driveE4,in.driveE8)));
    else {
        const float ramp=minimum(f.mul(p.steeringRamp104,speed2),1);
        if(firstTimer) force.v[0]=f.add(force.v[0],f.mul(f.mul(f.mul(steeringScale,p.timerSteering108),f.sub(in.driveE4,in.driveE8)),ramp));
        else if(secondTimer)torque.v[1]=f.add(torque.v[1],f.mul(f.mul(f.mul(steeringScale,f.add(p.steering100,extraSteering)),f.sub(in.driveE4,in.driveE8)),ramp));
        else if(tilted) {
            // Original maps up 0..-1 from tiltedSteering to zero, then clamps.
            const float scale=interpolate(up,0,-1,p.tiltedSteering11c,0,f);
            force.v[0]=f.add(force.v[0],f.mul(f.mul(f.mul(steeringScale,scale),f.sub(in.driveE4,in.driveE8)),ramp));
        } else torque.v[1]=f.add(torque.v[1],f.mul(f.mul(f.mul(steeringScale,f.add(p.steering100,extraSteering)),f.sub(in.driveE4,in.driveE8)),ramp));
    }
 }
}
namespace {
// Shared body of 16CD90. allowTilt is true on the original route; only the PC
// zero-g enhancement (vehicle_aero_untilted50zg.h) passes false.
bool applyAerodynamicsWith50ZG(const AeroState48E& previous,const AeroParameters48E& p,
                          const AeroInput48E& in,const ReadyBodyBasis48D& basis,
                          const BodyLoads48C& loads,float dt,bool allowTilt,AeroResult48E& out) noexcept {
    if(!std::isfinite(dt)||dt<0||!finite(in.bodyLocalVelocity)||!finite(in.angularVelocity)||
       !std::isfinite(in.bodyAngleE0)||!std::isfinite(in.driveE4)||!std::isfinite(in.driveE8)||
       !std::isfinite(in.brakeEc)||!std::isfinite(previous.timer140)||!std::isfinite(previous.timer144)||
       !finite(loads.force)||!finite(loads.torque))return false;
    for(const auto& c:basis.columns)if(!finite(c))return false;
    // All runtime coefficient fields are part of this API's finite domain.
    const float coefficients[]={p.offset68,p.lift6c,p.drag78,p.pitch84,p.pitchDamping88,p.lateral90,
        p.rollDampingB0,p.timerYawB8,p.yawDampingC8,p.driveDragF8,p.brakeDragFc,p.steering100,
        p.steeringRamp104,p.timerSteering108,p.timerLateral10c,p.timerYaw110,p.timerDrag114,
        p.tiltedLateral118,p.tiltedSteering11c,p.tiltedYaw120};
    for(float x:coefficients)if(!std::isfinite(x))return false;
    if(!finite(p.velocityDamping130)||(in.subtractWorldFlow&&!finite(in.worldFlow40)))return false;
    Arithmetic f;AeroResult48E result{};result.state=previous;result.accumulatedLoads=loads;
    auto v=in.bodyLocalVelocity;
    if(in.subtractWorldFlow) {const auto flow=inverseDirection(basis,in.worldFlow40,f);
        for(unsigned i=0;i<3;++i)v.v[i]=f.sub(v.v[i],flow.v[i]);}
    const float speed=f.root(f.add(f.add(f.mul(v.v[0],v.v[0]),f.mul(v.v[1],v.v[1])),f.mul(v.v[2],v.v[2])));
    const float pitch=v.v[2]<=20 ? in.bodyAngleE0:angle(f.div(-v.v[1],v.v[2]),f);
    const float factor=minimum(f.mul(fp(0x3ca3d70au),speed),1);
    const float signedFactor=v.v[2]<0 ? f.mul(factor,-1):factor;
    const float speed2=f.mul(speed,speed),up=basis.columns[1].v[1];
    const bool tilted=allowTilt&&up<=.5f;
    if(in.triggerD0){result.state.timer140=.25f;result.state.timer144=0;}
    const bool firstTimer=!(result.state.timer140<=0);
    if(firstTimer)result.state.timer140=f.sub(result.state.timer140,dt);
    if(in.triggerD8&&!firstTimer)result.state.timer144=4;
    const bool secondTimer=!(result.state.timer144<=0);
    if(secondTimer)result.state.timer144=f.sub(result.state.timer144,dt);
    result.state.lowSpeedDc=speed<50;
    AeroForcePrefix48E forces{};
    calculateForce(f,{v,speed,pitch,factor,signedFactor,speed2},p,in,result.state,
        up,firstTimer,secondTimer,tilted,forces);
    result.localForce=forces.localForce;result.directTorque=forces.directTorque;
    const auto& force=result.localForce;const auto& torque=result.directTorque;
    const auto worldForce=transform(basis,force,f);
    for(unsigned i=0;i<3;++i) {
        result.accumulatedLoads.force.v[i]=f.add(loads.force.v[i],worldForce.v[i]);
        result.accumulatedLoads.torque.v[i]=f.add(loads.torque.v[i],torque.v[i]);
    }
    if(!f.valid)return false;out=result;return true;
}
} // namespace
bool applyAerodynamics48E(const AeroState48E& previous,const AeroParameters48E& p,
                          const AeroInput48E& in,const ReadyBodyBasis48D& basis,
                          const BodyLoads48C& loads,float dt,AeroResult48E& out) noexcept {
    return applyAerodynamicsWith50ZG(previous,p,in,basis,loads,dt,true,out);
}
// PC ENHANCEMENT, NOT ORIGINAL (vehicle_aero_untilted50zg.h): zero-g flight only.
bool applyAerodynamicsUntilted50ZG(const AeroState48E& previous,const AeroParameters48E& p,
                          const AeroInput48E& in,const ReadyBodyBasis48D& basis,
                          const BodyLoads48C& loads,float dt,AeroResult48E& out) noexcept {
    return applyAerodynamicsWith50ZG(previous,p,in,basis,loads,dt,false,out);
}
bool prepareAeroMotionPrefix48E(const AeroInput48E& in,const ReadyBodyBasis48D& basis,
    AeroMotionPrefix48E& out)noexcept {
    if(!finite(in.bodyLocalVelocity)||!std::isfinite(in.bodyAngleE0))return false;
    Arithmetic f;auto v=in.bodyLocalVelocity;
    if(in.subtractWorldFlow){
        if(!finite(in.worldFlow40))return false;
        for(const auto& column:basis.columns)if(!finite(column))return false;
        const auto flow=inverseDirection(basis,in.worldFlow40,f);
        for(unsigned k=0;k<3;++k)v.v[k]=f.sub(v.v[k],flow.v[k]);
    }
    const float speed=f.root(f.add(f.add(f.mul(v.v[0],v.v[0]),f.mul(v.v[1],v.v[1])),f.mul(v.v[2],v.v[2])));
    const float pitch=v.v[2]<=20?in.bodyAngleE0:angle(f.div(-v.v[1],v.v[2]),f);
    const float factor=minimum(f.mul(fp(0x3ca3d70au),speed),1);
    const float signedFactor=v.v[2]<0?f.mul(factor,-1):factor;
    const float speed2=f.mul(speed,speed);
    if(!f.valid)return false;out={v,speed,pitch,factor,signedFactor,speed2};return true;
}
bool calculateAeroForcePrefix48E(const AeroMotionPrefix48E& motion,const AeroParameters48E& p,
    const AeroInput48E& in,const AeroState48E& state,float up,bool firstTimer,bool secondTimer,
    bool tilted,AeroForcePrefix48E& out)noexcept {
    if(!finite(motion.velocity)||!finite(in.angularVelocity)||!finite(p.velocityDamping130))return false;
    const float values[]={motion.speed,motion.pitch,motion.factor,motion.signedFactor,motion.speedSquared,
        state.timer140,state.timer144,up,in.driveE4,in.driveE8,in.brakeEc,p.offset68,p.lift6c,p.drag78,
        p.pitch84,p.pitchDamping88,p.lateral90,p.rollDampingB0,p.timerYawB8,p.yawDampingC8,
        p.driveDragF8,p.brakeDragFc,p.steering100,p.steeringRamp104,p.timerSteering108,p.timerLateral10c,
        p.timerYaw110,p.timerDrag114,p.tiltedLateral118,p.tiltedSteering11c,p.tiltedYaw120};
    for(float value:values)if(!std::isfinite(value))return false;
    Arithmetic f;AeroForcePrefix48E result{};
    calculateForce(f,motion,p,in,state,up,firstTimer,secondTimer,tilted,result);
    if(!f.valid)return false;out=result;return true;
}
bool stepEngineAeroBody48E(const BodyState48C& body,const BodyParameters48C& bp,const BodyLoads48C& prior,
                          const EngineState48D& engine,const EngineParameters48D& ep,const EngineInput48D& ei,
                          const AeroState48E& aero,const AeroParameters48E& ap,const AeroInput48E& ai,
                          const ReadyBodyBasis48D& basis,GravityParameters48D gravity,bool apply,float dt,
                          VehiclePartialStep48E& out) noexcept {
    VehiclePartialStep48E result{};BodyLoads48C loads{};
    if(!updateEngine48D(engine,ep,ei,result.engine)||
       !accumulateEngineGravity48D(prior,result.engine,basis,gravity,bp.mass,apply,true,loads)||
       !applyAerodynamics48E(aero,ap,ai,basis,loads,dt,result.aero)||
       !integrateBody48C(body,bp,result.aero.accumulatedLoads,dt,result.body))return false;
    out=result;return true;
}
bool refreshCachedSpeed48E(const Vector3_48C& velocity,float& out) noexcept {
    if(!finite(velocity))return false;Arithmetic f;
    const float square=f.add(f.add(f.mul(velocity.v[0],velocity.v[0]),f.mul(velocity.v[1],velocity.v[1])),f.mul(velocity.v[2],velocity.v[2]));
    const float speed=square==0 ? 0:f.div(1,f.reciprocalRoot(square));
    if(!f.valid)return false;out=speed;return true;
}
}
