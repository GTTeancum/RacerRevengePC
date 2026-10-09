#include "spring_model48k.h"
#include "spring_contact48j.h"
#include "spring_force48l.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include <type_traits>
namespace rr::game {
namespace {
using V=Vector3_48C;
static_assert(std::is_trivially_copyable<SpringModelBody48K>::value,"Body buffers must stay allocation-free");
static_assert(std::is_trivially_copyable<SpringGeneratedRecord48K>::value,"Record buffers must stay allocation-free");
struct Range { std::uintptr_t begin,end; };
bool range(const void* p,std::size_t count,std::size_t width,Range& r) noexcept {
    r={0,0};if(!count)return true;
    if(!p||count>std::numeric_limits<std::size_t>::max()/width)return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(p);const auto bytes=count*width;
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-begin)return false;
    r={begin,begin+bytes};return true;
}
bool overlaps(Range a,Range b) noexcept {return a.begin<b.end&&b.begin<a.end;}

bool finite(const V& v) noexcept {for(float x:v.v)if(!std::isfinite(x))return false;return true;}
bool affine(const BodyMatrix48G& m) noexcept {for(float x:m.v)if(!std::isfinite(x))return false;return m.v[3]==0&&m.v[7]==0&&m.v[11]==0&&m.v[15]==1;}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {volatile float r=a*b;return keep(r);}
    V minus(const V& a,const V& b) noexcept {V r{};for(unsigned i=0;i<3;++i)r.v[i]=sub(a.v[i],b.v[i]);return r;}
    V plus(const V& a,const V& b) noexcept {V r{};for(unsigned i=0;i<3;++i)r.v[i]=add(a.v[i],b.v[i]);return r;}
    V cross(const V& a,const V& b) noexcept {V r{};for(unsigned i=0;i<3;++i)r.v[i]=sub(mul(a.v[(i+1)%3],b.v[(i+2)%3]),mul(a.v[(i+2)%3],b.v[(i+1)%3]));return r;}
    V transform(const BodyMatrix48G& m,const V& v,bool point) noexcept {
        V r{};for(unsigned i=0;i<3;++i)r.v[i]=add(add(add(mul(m.v[i],v.v[0]),mul(m.v[4+i],v.v[1])),mul(m.v[8+i],v.v[2])),point?m.v[12+i]:0.f);return r;
    }
    V inverseDirection(const BodyMatrix48G& m,const V& v) noexcept {
        V r{};for(unsigned i=0;i<3;++i)r.v[i]=add(add(add(mul(add(m.v[4*i],0.f),v.v[0]),mul(add(m.v[4*i+1],0.f),v.v[1])),mul(add(m.v[4*i+2],0.f),v.v[2])),0.f);return r;
    }
    float dot(const V& a,const V& b) noexcept {return add(add(mul(a.v[0],b.v[0]),mul(a.v[1],b.v[1])),mul(1.f,mul(a.v[2],b.v[2])));}
};
V velocity(Math& m,const V& point,const V& center,const SpringImpactBody48K& primary,const SpringImpactBody48K* other) noexcept {
    const auto localArm=m.inverseDirection(primary.state.scene,m.minus(point,center));
    const auto rotational=m.transform(primary.state.scene,m.cross(primary.state.body.angularVelocity,localArm),false);
    const auto first=m.plus(rotational,primary.state.body.velocity);
    V second{};
    if(other){
        const auto otherCenter=m.transform(other->state.scene,other->localCenter,true);
        const auto omega=m.transform(other->state.scene,other->state.body.angularVelocity,false);
        second=m.plus(other->state.body.velocity,m.cross(omega,m.minus(point,otherCenter)));
    }
    return m.minus(first,second);
}
}
SpringGeneratedStatus48K updateSpringContacts48L(
    const SpringGeneratedParameters48K& parameters,const SpringResponseParameters48L& response,const SpringGeneratedState48K& state,
    const SpringModelBody48K* bodies,std::size_t bodyCount,const SpringPointRecord48L* pointRecords,std::size_t pointCount,
    const SpringGeneratedRecord48K* records,std::size_t count,
    const SpringGeneratedBuffers48K& scratch,const SpringGeneratedBuffers48K& destination,
    SpringGeneratedResult48K& output) noexcept {
    if(!bodies||!bodyCount||(count&&!records)||(pointCount&&!pointRecords))return SpringGeneratedStatus48K::InvalidInput;
    if(pointCount>std::numeric_limits<std::size_t>::max()-count)return SpringGeneratedStatus48K::InvalidInput;
    const auto total=pointCount+count;
    if(scratch.bodyCapacity<bodyCount||destination.bodyCapacity<bodyCount||
       scratch.recordCapacity<total||destination.recordCapacity<total)return SpringGeneratedStatus48K::InsufficientCapacity;
    Range inputBodies{},inputRecords{},inputPoints{},responseRange{},tempBodies{},tempFlags{},outBodies{},outFlags{},meta{},params{},flags{},scratchDescriptor{},destinationDescriptor{};
    if(!range(bodies,bodyCount,sizeof(*bodies),inputBodies)||!range(records,count,sizeof(*records),inputRecords)||
       !range(pointRecords,pointCount,sizeof(*pointRecords),inputPoints)||!range(&response,1,sizeof(response),responseRange)||
       !range(scratch.bodies,bodyCount,sizeof(*bodies),tempBodies)||!range(scratch.recordFlags50,total,1,tempFlags)||
       !range(destination.bodies,bodyCount,sizeof(*bodies),outBodies)||!range(destination.recordFlags50,total,1,outFlags)||
       !range(&output,1,sizeof(output),meta)||!range(&parameters,1,sizeof(parameters),params)||!range(&state,1,sizeof(state),flags)||
       !range(&scratch,1,sizeof(scratch),scratchDescriptor)||!range(&destination,1,sizeof(destination),destinationDescriptor))
        return SpringGeneratedStatus48K::InvalidInput;
    const Range inputs[]={inputBodies,inputRecords,inputPoints,responseRange,params,flags,meta,scratchDescriptor,destinationDescriptor};
    for(auto r:inputs)if(overlaps(tempBodies,r)||overlaps(tempFlags,r))return SpringGeneratedStatus48K::InvalidInput;
    if(overlaps(tempBodies,tempFlags)||overlaps(tempBodies,outBodies)||overlaps(tempBodies,outFlags)||
       overlaps(tempFlags,outBodies)||overlaps(tempFlags,outFlags)||overlaps(outBodies,outFlags)||
       overlaps(outBodies,meta)||overlaps(outFlags,meta))return SpringGeneratedStatus48K::InvalidInput;
    for(auto r:{inputRecords,inputPoints,responseRange,params,flags,scratchDescriptor,destinationDescriptor})if(overlaps(outBodies,r)||overlaps(outFlags,r))return SpringGeneratedStatus48K::InvalidInput;
    if((overlaps(outBodies,inputBodies)&&destination.bodies!=bodies)||overlaps(outFlags,inputBodies))return SpringGeneratedStatus48K::InvalidInput;
    for(auto r:{inputBodies,inputRecords,inputPoints,responseRange,params,flags,scratchDescriptor,destinationDescriptor})if(overlaps(meta,r))return SpringGeneratedStatus48K::InvalidInput;
    for(std::size_t i=0;i<bodyCount;++i){
        if(!bodies[i].identity)return SpringGeneratedStatus48K::InvalidInput;
        for(std::size_t j=0;j<i;++j)if(bodies[j].identity==bodies[i].identity)return SpringGeneratedStatus48K::InvalidInput;
    }
    {
        std::copy_n(bodies,bodyCount,scratch.bodies);
        SpringGeneratedResult48K out{};out.state=state;out.bodyCount=bodyCount;out.recordCount=total;
        out.worldCenter110=parameters.impact.worldCenter110;out.contactVelocity120=parameters.impact.contactVelocity120;
        Math math;auto& primary=scratch.bodies[0].body;
        if(parameters.pointContacts48||count){
            const auto& center=parameters.impact.customCenter70?parameters.impact.customLocalCenter100:primary.localCenter;
            if(!affine(primary.state.scene)||!finite(center))return SpringGeneratedStatus48K::InvalidInput;
            out.worldCenter110=math.transform(primary.state.scene,center,true);
        }
        for(std::size_t i=0;i<total;++i){
            const bool pointContact=i<pointCount;
            const auto& record=pointContact?pointRecords[i].contact:records[i-pointCount];
            if(pointContact&&!parameters.pointContacts48){scratch.recordFlags50[i]=record.flags50;continue;}
            if(pointContact&&!record.hasScene40)return SpringGeneratedStatus48K::InvalidInput;
            const auto& scene=record.hasScene40?record.scene40:primary.state.scene;
            if(!affine(scene)||!finite(record.localPoint10)||!finite(record.normal20)||!finite(record.surfacePoint30))return SpringGeneratedStatus48K::InvalidInput;
            const auto world=math.transform(scene,record.localPoint10,true);
            float depth=math.dot(math.minus(record.surfacePoint30,world),record.normal20);
            if(pointContact)depth=math.add(pointRecords[i].penetrationOffset48,depth);
            if(!math.valid)return SpringGeneratedStatus48K::InvalidInput;
            if(depth<=0.f){scratch.recordFlags50[i]=static_cast<std::uint8_t>(record.flags50&~2u);continue;}
            out.state.contact3C=1;
            if(pointContact)out.state.point74=1;
            else {
                out.state.generated78=1;
                if(record.otherBodyIdentity44)out.state.other44=1;
                else if(record.normal20.v[1]>.8f)out.state.upward40=1;
            }
            scratch.recordFlags50[i]=static_cast<std::uint8_t>(record.flags50|2u);
            SpringImpactBody48K* other=nullptr;
            if(!pointContact&&record.otherBodyIdentity44){
                for(std::size_t j=1;j<bodyCount;++j)if(scratch.bodies[j].identity==record.otherBodyIdentity44){other=&scratch.bodies[j].body;break;}
                if(!other||!affine(other->state.scene)||!finite(other->localCenter))return SpringGeneratedStatus48K::InvalidInput;
            }
            V point{};for(unsigned j=0;j<3;++j)point.v[j]=math.add(world.v[j],math.mul(depth,record.normal20.v[j]));
            out.contactVelocity120=velocity(math,point,out.worldCenter110,primary,other);
            if(!math.valid)return SpringGeneratedStatus48K::InvalidInput;
            const auto springForce=[&]() noexcept {
                const SpringPointForceParameters48J pointForce{primary.localCenter,parameters.impact.customLocalCenter100,parameters.impact.forceTorqueScaleB4,parameters.impact.customCenter70,parameters.impact.yawOnly6C};
                const SpringForceParameters48L force{out.contactVelocity120,pointForce,response.maxPenetration90,response.maxSpeed94,response.stiffness9C,response.dampingA0,response.impactStiffnessA4,response.impactDampingA8,parameters.impact54,parameters.otherSpring58};
                SpringFrictionResult48J result{};
                if(!applySpringForce48L(record.normal20,point,depth,force,primary,other,result))return false;
                primary.state.loads=result.primary;if(other)other->state.loads=result.other;return true;
            };
            const auto secondary=[&]() noexcept {
                return removeSpringInwardVelocity48L(record.normal20,out.contactVelocity120,primary.state.body.velocity,primary.state.body.velocity);
            };
            const bool alternateOther=other&&parameters.otherSpring58;
            if(alternateOther){if(!springForce())return SpringGeneratedStatus48K::InvalidInput;}
            else {
            if(parameters.impact54){
                auto input=parameters.impact;input.worldCenter110=out.worldCenter110;input.contactVelocity120=out.contactVelocity120;
                SpringImpactResult48K impact{};
                if(!applySpringImpact48K(record.normal20,point,depth,input,primary,other,impact))return SpringGeneratedStatus48K::InvalidInput;
                primary.state=impact.primary;if(other)other->state=impact.other;
            }
            if(parameters.secondary68&&!secondary())return SpringGeneratedStatus48K::InvalidInput;
            if(parameters.springForce4C){
                if(response.secondaryAtLimit60){
                    if(!std::isfinite(response.maxPenetration90))return SpringGeneratedStatus48K::InvalidInput;
                    if(depth>=response.maxPenetration90&&!parameters.impact54&&!secondary())return SpringGeneratedStatus48K::InvalidInput;
                }
                if(!springForce())return SpringGeneratedStatus48K::InvalidInput;
            }
            }
            if(parameters.friction50&&(!pointContact||(record.flags50&1u))){
                if(record.material4C>=parameters.frictionByMaterial.size())return SpringGeneratedStatus48K::InvalidInput;
                SpringPointForceParameters48J force{primary.localCenter,parameters.impact.customLocalCenter100,parameters.frictionTorqueScaleC4,parameters.impact.customCenter70,parameters.impact.yawOnly6C};
                SpringFrictionBody48J a{primary.state.scene,primary.state.loads,primary.parameters.mass},b{};
                if(other)b={other->state.scene,other->state.loads,other->parameters.mass};
                SpringFrictionResult48J friction{};
                if(!applySpringFriction48J(record.normal20,point,out.contactVelocity120,parameters.frictionByMaterial[record.material4C],force,a,other?&b:nullptr,friction))return SpringGeneratedStatus48K::InvalidInput;
                primary.state.loads=friction.primary;if(other)other->state.loads=friction.other;
            }
        }
        out.state.pending38=0;if(!math.valid)return SpringGeneratedStatus48K::InvalidInput;
        std::copy_n(scratch.bodies,bodyCount,destination.bodies);
        if(total)std::copy_n(scratch.recordFlags50,total,destination.recordFlags50);
        output=out;return SpringGeneratedStatus48K::Complete;
    }
}
SpringGeneratedStatus48K updateGeneratedSpringContacts48K(
    const SpringGeneratedParameters48K& parameters,const SpringGeneratedState48K& state,
    const SpringModelBody48K* bodies,std::size_t bodyCount,const SpringGeneratedRecord48K* records,std::size_t count,
    const SpringGeneratedBuffers48K& scratch,const SpringGeneratedBuffers48K& destination,
    SpringGeneratedResult48K& output) noexcept {
    // Preserve the original bounded48K API contract; expanded tuning/list inputs
    // are required through the explicit48L entry point.
    if(parameters.pointContacts48||parameters.springForce4C||parameters.otherSpring58||parameters.secondary68)return SpringGeneratedStatus48K::UnsupportedScope;
    return updateSpringContacts48L(parameters,SpringResponseParameters48L{},state,bodies,bodyCount,nullptr,0,records,count,scratch,destination,output);
}

}
