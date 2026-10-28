#include "vehicle_contact48r.h"
#include <cmath>
#include <cfenv>
#include <cstring>
#include <limits>
#include <type_traits>

namespace rr::game {
namespace {
using Status=VehicleContactStatus48R;
using Vec=SpringVector4_48N;
static_assert(sizeof(float)==4,"Original arithmetic requires binary32");
static_assert(sizeof(SpringRecord48N)==96,"Preserve every original record byte");
static_assert(std::is_trivially_copyable<VehicleContactScene48R>::value&&
              std::is_trivially_copyable<VehicleContactBody48R>::value,
              "Transactional projections must be allocation-free");
struct Span {std::uintptr_t first,last;};
bool overlaps(Span a,Span b) noexcept {return a.first<b.last&&b.first<a.last;}
template<class T>bool span(const T* p,std::size_t n,Span& out) noexcept {
    out={0,0};if(!n)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||
       n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-begin)return false;
    out={begin,begin+bytes};return true;
}
template<class T>void copy(T* destination,const T* source,std::size_t count) noexcept {
    if(count)std::memcpy(destination,source,count*sizeof(T));
}
float fromBits(std::uint32_t bits) noexcept {
    float value;std::memcpy(&value,&bits,sizeof(value));return value;
}
std::uint32_t bits(float value) noexcept {
    std::uint32_t result;std::memcpy(&result,&value,sizeof(result));return result;
}
struct Arithmetic {
    bool valid=true;
    float keep(float value) noexcept {valid=valid&&std::isfinite(value);return value;}
    float add(float a,float b) noexcept {volatile float value=a+b;return keep(value);}
    float sub(float a,float b) noexcept {volatile float value=a-b;return keep(value);}
    float mul(float a,float b) noexcept {volatile float value=a*b;return keep(value);}
    float div(float a,float b) noexcept {volatile float value=a/b;return keep(value);}
    float root(float a) noexcept {
        // The frozen48H/48J original-word oracle's ordinary finite policy.
        // This does not borrow48G's separate exponent-zero flush behavior.
        volatile float value=static_cast<float>(std::sqrt(std::fabs(static_cast<double>(a))));
        return keep(value);
    }
    float square(const Vec& a) noexcept {
        const float x=mul(a.v[0],a.v[0]),y=mul(a.v[1],a.v[1]);
        const float acc=add(x,y);
        return add(acc,mul(a.v[2],a.v[2]));
    }
    float dot(const Vec& a,const Vec& b) noexcept {
        //106E70's VU dot includes an explicit1*Z product after X+Y.
        const float x=mul(a.v[0],b.v[0]),y=mul(a.v[1],b.v[1]);
        const float z=mul(a.v[2],b.v[2]),one=add(0.f,1.f);
        return add(add(x,y),mul(one,z));
    }
    Vec normal(const Vec& a) noexcept {
        const float d=dot(a,a);
        const float q=d==0.f?fromBits((bits(d)&0x80000000u)|0x7f7fffffu):div(1.f,root(d));
        Vec result{{0,0,0,add(a.v[3],0.f)}};
        for(unsigned i=0;i<3;++i)result.v[i]=mul(a.v[i],q);
        return result;
    }
    float transform(const BodyMatrix48G& m,const Vec& p,unsigned axis,bool point) noexcept {
        float r=mul(m.v[axis],p.v[0]);
        r=add(r,mul(m.v[4+axis],p.v[1]));
        r=add(r,mul(m.v[8+axis],p.v[2]));
        return add(r,point?m.v[12+axis]:0.f);
    }
    Vec boundsHalf(const VehicleContactCandidate48R& c) noexcept {
        //1202E0 always computes its center, although219420 discards it.
        // Every directional transform retains its two explicit zero terms
        // and final add-zero rather than collapsing to a column product.
        for(unsigned i=0;i<3;++i)(void)transform(c.shapeWorld,c.localCenter,i,true);
        const Vec x{{c.localHalf.v[0],0,0,1}};
        const Vec y{{0,c.localHalf.v[1],0,1}};
        const Vec z{{0,0,c.localHalf.v[2],1}};
        Vec result{{0,0,0,1}};
        for(unsigned i=0;i<3;++i){
            const float tx=transform(c.shapeWorld,x,i,false);
            const float ty=transform(c.shapeWorld,y,i,false);
            const float tz=transform(c.shapeWorld,z,i,false);
            result.v[i]=add(std::fabs(tz),add(std::fabs(tx),std::fabs(ty)));
        }
        return result;
    }
    Vec inversePoint(const BodyMatrix48G& m,const Vec& p) noexcept {
        Vec result{{0,0,0,1}};
        for(unsigned i=0;i<3;++i){
            //28BD50 constructs the transposed matrix, retaining add-zero,
            // then its negative translated column. Point and translation
            // are transformed separately; transpose(point-T) is different.
            float row[3]{};
            for(unsigned j=0;j<3;++j)
                row[j]=i==j?add(m.v[4*i+j],0.f):add(0.f,m.v[4*i+j]);
            float translation=mul(row[0],m.v[12]);
            translation=add(translation,mul(row[1],m.v[13]));
            translation=add(translation,mul(row[2],m.v[14]));
            translation=mul(translation,sub(0.f,1.f));
            float point=mul(row[0],p.v[0]);
            point=add(point,mul(row[1],p.v[1]));
            point=add(point,mul(row[2],p.v[2]));
            result.v[i]=add(point,translation);
        }
        return result;
    }
};
Status prepareScene(VehicleContactScene48R& scene) noexcept {
    if(scene.flags&1u)return Status::Complete;
    if(scene.parentPresent)return Status::UnsupportedSceneParent;
    std::memcpy(&scene.world,&scene.local,sizeof(scene.world));
    scene.flags|=1u;return Status::Complete;
}
}
Status runVehicleContacts48R(const VehicleContactInput48R& input,
    const VehicleContactSource48R& source,const VehicleContactBuffers48R& scratch,
    const VehicleContactBuffers48R& destination,VehicleContactResult48R& output) noexcept {
    if(scratch.sceneCapacity<source.sceneCount||destination.sceneCapacity<source.sceneCount||
       scratch.bodyCapacity<source.bodyCount||destination.bodyCapacity<source.bodyCount||
       scratch.recordCapacity<source.recordCount||destination.recordCapacity<source.recordCount)
        return Status::InsufficientCapacity;
    Span writes[7]{},reads[9]{};
    if(!span(scratch.scenes,scratch.sceneCapacity,writes[0])||
       !span(scratch.bodies,scratch.bodyCapacity,writes[1])||
       !span(scratch.records,scratch.recordCapacity,writes[2])||
       !span(destination.scenes,destination.sceneCapacity,writes[3])||
       !span(destination.bodies,destination.bodyCapacity,writes[4])||
       !span(destination.records,destination.recordCapacity,writes[5])||!span(&output,1,writes[6])||
       !span(&input,1,reads[0])||!span(&source,1,reads[1])||!span(&scratch,1,reads[2])||
       !span(&destination,1,reads[3])||!span(source.scenes,source.sceneCount,reads[4])||
       !span(source.bodies,source.bodyCount,reads[5])||
       !span(source.candidates,source.candidateCount,reads[6])||
       !span(source.candidateOrder,source.candidateOrderCount,reads[7])||
       !span(source.records,source.recordCount,reads[8]))return Status::InvalidInput;
    for(unsigned i=0;i<7;++i){
        for(unsigned j=0;j<i;++j)if(overlaps(writes[i],writes[j]))return Status::InvalidInput;
        for(auto read:reads)if(overlaps(writes[i],read))return Status::InvalidInput;
    }
    // Each body index denotes one mutable object. Duplicate identity labels
    // at distinct indices would give two divergent states for that object.
    for(std::size_t i=0;i<source.bodyCount;++i)
        for(std::size_t j=0;j<i;++j)
            if(source.bodies[i].identity.value==source.bodies[j].identity.value)return Status::InvalidInput;
    copy(scratch.scenes,source.scenes,source.sceneCount);
    copy(scratch.bodies,source.bodies,source.bodyCount);
    copy(scratch.records,source.records,source.recordCount);
    VehicleContactResult48R result{input.spring,input.statics};
    if(!result.statics.flags[0]){result.statics.words[0]=0x3f000000u;result.statics.flags[0]=1;}
    if(!result.statics.flags[1]){result.statics.words[1]=0x3f800000u;result.statics.flags[1]=1;}
    if(input.positionSceneIndex>=source.sceneCount)return Status::InvalidInput;
    auto& positionScene=scratch.scenes[input.positionSceneIndex];
    // Both original scene calls precede the signed loop-count test.
    auto status=prepareScene(positionScene);if(status!=Status::Complete)return status;
    Vec position{{positionScene.world.v[12],positionScene.world.v[13],positionScene.world.v[14],1}};
    status=prepareScene(positionScene);if(status!=Status::Complete)return status;
    Vec up{{positionScene.world.v[4],positionScene.world.v[5],positionScene.world.v[6],1}};
    if(input.candidateCount>0&&static_cast<std::size_t>(input.candidateCount)>source.candidateOrderCount)
        return Status::InsufficientCapacity;
    Arithmetic math;
    for(std::int32_t index=0;index<input.candidateCount;++index){
        const auto candidateIndex=source.candidateOrder[static_cast<std::size_t>(index)];
        if(candidateIndex>=source.candidateCount)return Status::InvalidInput;
        const auto& candidate=source.candidates[candidateIndex];
        if(!(candidate.category&input.categoryMask))continue;
        if(candidate.shapeTag!=1&&candidate.shapeTag!=6)return Status::UnsupportedBounds;
        Vec half=math.boundsHalf(candidate);half.v[1]=0.f;
        const float radius=math.mul(fromBits(result.statics.words[0]),math.root(math.square(half)));
        Vec lateral{{0,0,0,1}};
        for(unsigned i=0;i<3;++i)lateral.v[i]=math.sub(position.v[i],candidate.cachedCenter.v[i]);
        const float projection=math.dot(lateral,up);
        for(unsigned i=0;i<3;++i)lateral.v[i]=math.sub(lateral.v[i],math.mul(projection,up.v[i]));
        const float distance=math.root(math.square(lateral));
        const float threshold=math.add(input.vehicleRadius,radius);
        if(!math.valid)return Status::InvalidInput;
        if(!(distance<threshold))continue;
        SpringRecordAppend48N record{};
        record.normal20=math.normal(lateral);
        Vec surface{{0,0,0,1}};
        for(unsigned i=0;i<3;++i)
            surface.v[i]=math.sub(position.v[i],math.mul(input.vehicleRadius,record.normal20.v[i]));
        if(!math.valid)return Status::InvalidInput;
        if(input.localSceneIndex>=source.sceneCount)return Status::InvalidInput;
        auto& localScene=scratch.scenes[input.localSceneIndex];
        status=prepareScene(localScene);if(status!=Status::Complete)return status;
        record.field00=math.inversePoint(localScene.world,position);
        status=prepareScene(localScene);if(status!=Status::Complete)return status;
        record.localPoint10=math.inversePoint(localScene.world,surface);
        record.surfacePoint30.v[3]=1.f;
        for(unsigned i=0;i<3;++i)
            record.surfacePoint30.v[i]=math.add(candidate.cachedCenter.v[i],math.mul(radius,record.normal20.v[i]));
        if(!math.valid)return Status::InvalidInput;
        if(input.primaryBodyIndex>=source.bodyCount||candidate.bodyIndex>=source.bodyCount)return Status::InvalidInput;
        auto& primary=scratch.bodies[input.primaryBodyIndex];
        auto& other=scratch.bodies[candidate.bodyIndex];
        // Both original body objects are dereferenced on this hit. Native
        // SpringBodyId48N reserves zero for absence, so it cannot label either.
        // Standalone165430 still accepts zero in its by-value identity payload.
        if(!primary.identity.value||!other.identity.value)return Status::InvalidInput;
        if(primary.callback!=VehicleContactCallback48R::RigidBody165430)return Status::UnsupportedBodyCallback;
        if(applyBodyContactList48R(other.identity,primary.contacts)==BodyContactListStatus48R::InvalidInput)
            return Status::InvalidInput;
        if(other.callback!=VehicleContactCallback48R::RigidBody165430)return Status::UnsupportedBodyCallback;
        if(applyBodyContactList48R(primary.identity,other.contacts)==BodyContactListStatus48R::InvalidInput)
            return Status::InvalidInput;
        record.other44.value=other.identity.value; // scene40/material4C remain0.
        const auto appended=appendSpringRecord48N(record,scratch.records,source.recordCount,result.spring);
        if(appended==SpringRecordStatus48N::InvalidInput)return Status::InvalidInput;
        if(appended==SpringRecordStatus48N::InsufficientCapacity)return Status::InsufficientCapacity;
    }
    copy(destination.scenes,scratch.scenes,source.sceneCount);
    copy(destination.bodies,scratch.bodies,source.bodyCount);
    copy(destination.records,scratch.records,source.recordCount);
    output=result;return Status::Complete;
}
bool prepareVehicleContactGeometry48R(const VehicleContactGeometryInput48R& input,
    VehicleContactGeometryPrefix48R& output) noexcept {
    Span in{},out{};
    if(!span(&input,1,in)||!span(&output,1,out)||overlaps(in,out)||
       std::fegetround()!=FE_TONEAREST)return false;
    Arithmetic math;Vec half{{input.boundsHalf.v[0],0.f,input.boundsHalf.v[2],1.f}};
    //21952C clears Y after1202E0; no candidate cachedRadius read occurs.
    const float radius=math.mul(fromBits(input.radiusScaleWord),math.root(math.square(half)));
    Vec lateral{{0,0,0,1}};
    for(unsigned i=0;i<3;++i)lateral.v[i]=math.sub(input.position.v[i],input.cachedCenter.v[i]);
    const float projection=math.dot(lateral,input.up);
    for(unsigned i=0;i<3;++i)lateral.v[i]=math.sub(lateral.v[i],math.mul(projection,input.up.v[i]));
    const float distance=math.root(math.square(lateral));
    const float threshold=math.add(input.vehicleRadius,radius);
    if(!math.valid)return false;
    VehicleContactGeometryPrefix48R result{};result.candidateRadius=radius;
    result.hit=distance<threshold;
    if(result.hit){
        result.normal=math.normal(lateral);result.vehicleSurface={{0,0,0,1}};
        for(unsigned i=0;i<3;++i)
            result.vehicleSurface.v[i]=math.sub(input.position.v[i],math.mul(input.vehicleRadius,result.normal.v[i]));
        if(!math.valid)return false;
    }
    output=result;return true;
}
bool prepareVehicleContactLocalPoint48R(const BodyMatrix48G& world,
    const SpringVector4_48N& point,SpringVector4_48N& output) noexcept {
    Span matrix{},in{},out{};
    if(!span(&world,1,matrix)||!span(&point,1,in)||!span(&output,1,out)||
       overlaps(matrix,out)||overlaps(in,out)||std::fegetround()!=FE_TONEAREST)return false;
    Arithmetic math;const auto result=math.inversePoint(world,point);
    if(!math.valid)return false;output=result;return true;
}
bool prepareVehicleContactCandidateSurface48R(const SpringVector4_48N& center,
    const SpringVector4_48N& normal,float radius,SpringVector4_48N& output) noexcept {
    Span first{},second{},out{};
    if(!span(&center,1,first)||!span(&normal,1,second)||!span(&output,1,out)||
       overlaps(first,out)||overlaps(second,out)||std::fegetround()!=FE_TONEAREST)return false;
    Arithmetic math;SpringVector4_48N result{{0,0,0,1}};
    for(unsigned i=0;i<3;++i)result.v[i]=math.add(center.v[i],math.mul(radius,normal.v[i]));
    if(!math.valid)return false;output=result;return true;
}

}
