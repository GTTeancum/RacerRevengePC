#include "spring_detector48o.h"
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
struct Span {std::uintptr_t first,last;};
bool overlap(Span a,Span b) noexcept {return a.first<b.last&&b.first<a.last;}
template<class T>bool span(const T* p,std::size_t n,Span& s) noexcept {
    s={0,0};if(!n)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-a)return false;
    s={a,a+bytes};return true;
}
struct Arithmetic {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {volatile float r=a*b;return keep(r);}
};
std::uint32_t orderedKey(float x) noexcept {
    std::uint32_t b;std::memcpy(&b,&x,sizeof(b));
    // Monotonic finite binary32 ordering, including -0 below +0. This expresses
    // the scalar MIN.S/MAX.S signed-word rule without host min/max zero ties.
    return (b&0x80000000u)?~b:(b|0x80000000u);
}
std::int32_t signedWord(std::uint32_t x) noexcept {std::int32_t y;std::memcpy(&y,&x,sizeof(y));return y;}
template<class T>void copy(T* d,const T* s,std::size_t count) noexcept {
    if(count)std::memcpy(d,s,count*sizeof(T));
}
}
SpringDetectorStatus48O prepareSpringDetector48O(const SpringDetectorInput48O& input,
    const SpringDetectorSource48O& source,const SpringDetectorBuffers48O& scratch,
    const SpringDetectorBuffers48O& destination,SpringDetectorState48O& output) noexcept {
    using Status=SpringDetectorStatus48O;
    if(scratch.sceneCapacity<source.sceneCount||destination.sceneCapacity<source.sceneCount||
       scratch.pairVectorCapacity<source.pairVectorCount||destination.pairVectorCapacity<source.pairVectorCount||
       scratch.stagedVectorCapacity<source.stagedVectorCount||destination.stagedVectorCapacity<source.stagedVectorCount||
       scratch.stagedMetadataCapacity<source.stagedMetadataCount||destination.stagedMetadataCapacity<source.stagedMetadataCount)
        return Status::InsufficientCapacity;
    Span writes[9]{},reads[9]{};
    if(!span(scratch.scenes,scratch.sceneCapacity,writes[0])||!span(scratch.pairs,scratch.pairVectorCapacity,writes[1])||
       !span(scratch.staged,scratch.stagedVectorCapacity,writes[2])||!span(scratch.stagedMetadata,scratch.stagedMetadataCapacity,writes[3])||
       !span(destination.scenes,destination.sceneCapacity,writes[4])||!span(destination.pairs,destination.pairVectorCapacity,writes[5])||
       !span(destination.staged,destination.stagedVectorCapacity,writes[6])||!span(destination.stagedMetadata,destination.stagedMetadataCapacity,writes[7])||
       !span(&output,1,writes[8])||!span(&input,1,reads[0])||!span(&source,1,reads[1])||
       !span(&scratch,1,reads[2])||!span(&destination,1,reads[3])||!span(source.points,source.pointCapacity,reads[4])||
       !span(source.scenes,source.sceneCount,reads[5])||!span(source.pairs,source.pairVectorCount,reads[6])||
       !span(source.staged,source.stagedVectorCount,reads[7])||!span(source.stagedMetadata,source.stagedMetadataCount,reads[8]))return Status::InvalidInput;
    for(unsigned i=0;i<9;++i){
        for(unsigned j=0;j<i;++j)if(overlap(writes[i],writes[j]))return Status::InvalidInput;
        for(auto read:reads)if(overlap(writes[i],read))return Status::InvalidInput;
    }
    SpringDetectorState48O result=input.state;
    copy(scratch.scenes,source.scenes,source.sceneCount);
    copy(scratch.pairs,source.pairs,source.pairVectorCount);
    copy(scratch.staged,source.staged,source.stagedVectorCount);
    copy(scratch.stagedMetadata,source.stagedMetadata,source.stagedMetadataCount);
    const auto commit=[&](Status status) noexcept {
        copy(destination.scenes,scratch.scenes,source.sceneCount);
        copy(destination.pairs,scratch.pairs,source.pairVectorCount);
        copy(destination.staged,scratch.staged,source.stagedVectorCount);
        copy(destination.stagedMetadata,scratch.stagedMetadata,source.stagedMetadataCount);
        output=result;return status;
    };
    if(!input.springPresent)return commit(Status::NoSpring);
    const auto signedCount=signedWord(result.spring.pointCount7C);
    if(signedCount>0&&input.providerPresent&&input.providerEnabled)return Status::UnsupportedScope;
    std::size_t pointCount=0;
    if(input.pairsPresent){
        // Original empty/negative active lists produce -infinite half extents
        // in the bounded IEEE model. No invented empty-box fallback is used.
        if(signedCount<=0)return Status::InvalidInput;
        pointCount=static_cast<std::size_t>(signedCount);
        if(pointCount>source.pointCapacity||pointCount>source.pairVectorCount/2)return Status::InsufficientCapacity;
        float minimum[3]={std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max()};
        float maximum[3]={-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max()};
        Arithmetic math;
        for(std::size_t i=0;i<pointCount;++i){
            const auto& point=source.points[i];
            if(point.sceneIndex>=source.sceneCount)return Status::InvalidInput;
            auto& scene=scratch.scenes[point.sceneIndex];
            if(!(scene.flags&1u)){
                if(scene.parentPresent)return Status::UnsupportedScope;
                std::memcpy(&scene.world,&scene.local,sizeof(scene.world));scene.flags|=1u;
            }
            SpringVector4_48N world{};world.v[3]=1.f;
            for(unsigned axis=0;axis<3;++axis){
                if(!std::isfinite(point.local.v[axis]))return Status::InvalidInput;
                for(unsigned column=0;column<4;++column)if(!std::isfinite(scene.world.v[column*4+axis]))return Status::InvalidInput;
            }
            for(unsigned axis=0;axis<3;++axis){
                const auto& m=scene.world.v;
                float value=math.mul(m[axis],point.local.v[0]);
                value=math.add(value,math.mul(m[4+axis],point.local.v[1]));
                value=math.add(value,math.mul(m[8+axis],point.local.v[2]));
                world.v[axis]=math.add(value,m[12+axis]);
            }
            if(!math.valid)return Status::InvalidInput;
            scratch.pairs[2*i+1]=world;
            for(unsigned axis=0;axis<3;++axis){
                const auto key=orderedKey(world.v[axis]);
                if(key<orderedKey(minimum[axis]))minimum[axis]=world.v[axis];
                if(key>orderedKey(maximum[axis]))maximum[axis]=world.v[axis];
            }
        }
        for(unsigned axis=0;axis<3;++axis){
            result.center.v[axis]=math.mul(.5f,math.add(maximum[axis],minimum[axis]));
            result.half.v[axis]=math.mul(.5f,math.sub(maximum[axis],minimum[axis]));
        }
        result.center.v[3]=1.f;result.half.v[3]=1.f;
        if(!math.valid)return Status::InvalidInput;
    }
    if(input.resetSpring)resetSpringRecords48N(result.spring);
    if(!input.queryPresent||!input.queryEnabled)return commit(Status::QueryDisabled);
    if(!input.pairsPresent){
        if(result.queryType!=3)return Status::UnsupportedScope;
        return commit(Status::RegistrationBoundary);
    }
    for(std::size_t i=0;i<pointCount;++i)scratch.pairs[2*i]=result.center;
    // Keep the staging source count separate from pointCount: untouched source
    // tail pairs are legitimate initialized input when the shape count is larger.
    const QueryPairStagingInput48O staging{input.shapePresent,input.shapePairCount,
        scratch.pairs,source.pairVectorCount,nullptr,0};
    const auto staged=stageQueryPairs48O(staging,scratch.staged,source.stagedVectorCount,
        scratch.stagedMetadata,source.stagedMetadataCount,result.queryType);
    if(staged==QueryPairStagingStatus48O::InsufficientCapacity)return Status::InsufficientCapacity;
    if(staged==QueryPairStagingStatus48O::InvalidInput)return Status::InvalidInput;
    return commit(input.explicitList?Status::LimitedQueryBoundary:Status::WorldQueryBoundary);
}
}
