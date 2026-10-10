#include "vehicle_forces48d.h"
#include <cmath>
#include <initializer_list>

namespace rr::game {
namespace {
struct FloatOps {
    bool valid = true;
    float keep(float x) noexcept { valid = valid && std::isfinite(x); return x; }
    float add(float a,float b) noexcept { volatile float r=a+b; return keep(r); }
    float sub(float a,float b) noexcept { volatile float r=a-b; return keep(r); }
    float mul(float a,float b) noexcept { volatile float r=a*b; return keep(r); }
    float div(float a,float b) noexcept { volatile float r=a/b; return keep(r); }
};
bool finite(const Vector3_48C& v) noexcept {
    for(float f:v.v) if(!std::isfinite(f)) return false;
    return true;
}
float speedFactor(float speed,float lower,float upper,FloatOps& f) noexcept {
    if(upper==lower) return 0.0f;
    const float numerator=f.mul(f.sub(speed,lower),-1.0f);
    const float factor=f.add(1.0f,f.div(numerator,f.sub(upper,lower)));
    if(factor<0.0f) return 0.0f;
    if(!(factor<=1.0f)) return 1.0f;
    return factor;
}
Vector3_48C transform(const ReadyBodyBasis48D& b,const Vector3_48C& v,FloatOps& f) noexcept {
    Vector3_48C out{};
    for(unsigned i=0;i<3;++i) {
        const float x=f.mul(b.columns[0].v[i],v.v[0]);
        const float y=f.mul(b.columns[1].v[i],v.v[1]);
        const float z=f.mul(b.columns[2].v[i],v.v[2]);
        out.v[i]=f.add(f.add(f.add(x,y),z),0.0f);
    }
    return out;
}
}

bool updateEngine48D(const EngineState48D& previous,const EngineParameters48D& p,
                     const EngineInput48D& input,EngineState48D& out) noexcept {
    for(float x:{p.maxThrust,p.boostThrust,p.repairThrust,p.rampUpFilter,p.rampDownFilter,
                 p.rampDownSpeed,p.theoreticalTopSpeed,p.boostSpeed,p.repairSpeed,input.bodySpeed,
                 input.drive[0],input.drive[1],previous.smoothedDrive[0],previous.smoothedDrive[1]})
        if(!std::isfinite(x)) return false;
    if(!finite(p.torqueBasis[0]) || !finite(p.torqueBasis[1])) return false;
    FloatOps f;
    EngineState48D result{};
    for(unsigned i=0;i<2;++i) {
        const float blend=input.drive[i]<=0.0f ? p.rampDownFilter:p.rampUpFilter;
        result.smoothedDrive[i]=f.add(f.mul(previous.smoothedDrive[i],f.sub(1.0f,blend)),
                                     f.mul(input.drive[i],blend));
    }
    float lower=f.sub(p.theoreticalTopSpeed,p.rampDownSpeed);
    float upper=p.theoreticalTopSpeed;
    // This subtraction is performed before the original mode branch, even when
    // its result is not consumed by the selected path.
    const float repairUpper=f.sub(p.theoreticalTopSpeed,p.repairSpeed);
    float thrust=p.maxThrust;
    if(input.boost) {
        lower=f.add(lower,p.boostSpeed);
        upper=f.add(p.theoreticalTopSpeed,p.boostSpeed);
        thrust=f.add(p.boostThrust,p.maxThrust);
    } else if(input.repair) {
        upper=repairUpper;
        thrust=f.sub(p.maxThrust,p.repairThrust);
    }
    const float factor=speedFactor(input.bodySpeed,lower,upper,f);
    for(unsigned i=0;i<2;++i) {
        const float scale=f.mul(result.smoothedDrive[i],factor);
        result.localThrust[i].v[2]=input.boost || input.repair ? f.mul(scale,thrust):f.mul(thrust,scale);
        for(unsigned j=0;j<3;++j) {
            result.torque[i].v[j]=f.mul(scale,p.torqueBasis[i].v[j]);
            if(!input.fullTorque) result.torque[i].v[j]=f.mul(result.torque[i].v[j],0.5f);
        }
    }
    if(!f.valid) return false;
    out=result;
    return true;
}

bool accumulateEngineGravity48D(const BodyLoads48C& previous,const EngineState48D& engine,
                                const ReadyBodyBasis48D& basis,GravityParameters48D gravity,
                                float mass,bool applyEngine,bool hasTransform,BodyLoads48C& out) noexcept {
    if(!finite(previous.force) || !finite(previous.torque) || !std::isfinite(mass) ||
       !std::isfinite(gravity.magnitude) || !std::isfinite(gravity.forceX) || !std::isfinite(gravity.forceZ)) return false;
    if(applyEngine) {
        for(const auto& torque:engine.torque) if(!finite(torque)) return false;
        if(hasTransform) {
            for(const auto& thrust:engine.localThrust) if(!finite(thrust)) return false;
            for(const auto& column:basis.columns) if(!finite(column)) return false;
        }
    }
    FloatOps f;
    BodyLoads48C result=previous;
    const Vector3_48C gravityForce{{gravity.forceX,f.mul(-mass,gravity.magnitude),gravity.forceZ}};
    for(unsigned i=0;i<3;++i) result.force.v[i]=f.add(result.force.v[i],gravityForce.v[i]);
    if(applyEngine) {
        if(hasTransform) {
            for(const auto& local:engine.localThrust) {
                const auto world=transform(basis,local,f);
                for(unsigned i=0;i<3;++i) result.force.v[i]=f.add(result.force.v[i],world.v[i]);
            }
        }
        for(const auto& torque:engine.torque)
            for(unsigned i=0;i<3;++i) result.torque.v[i]=f.add(result.torque.v[i],torque.v[i]);
    }
    if(!f.valid) return false;
    out=result;
    return true;
}

bool stepEngineGravityBody48D(const BodyState48C& body,const BodyParameters48C& bodyParameters,
                             const BodyLoads48C& priorLoads,const EngineState48D& engine,
                             const EngineParameters48D& engineParameters,const EngineInput48D& input,
                             const ReadyBodyBasis48D& basis,GravityParameters48D gravity,
                             bool applyEngine,float elapsedFactor,EngineGravityStep48D& out) noexcept {
    EngineGravityStep48D result{};
    if(!updateEngine48D(engine,engineParameters,input,result.engine) ||
       !accumulateEngineGravity48D(priorLoads,result.engine,basis,gravity,bodyParameters.mass,
                                   applyEngine,true,result.accumulatedLoads) ||
       !integrateBody48C(body,bodyParameters,result.accumulatedLoads,elapsedFactor,result.body)) return false;
    out=result;
    return true;
}
}
