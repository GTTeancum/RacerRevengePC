#include "query_prepare48o.h"
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
struct Range { std::uintptr_t begin=0,end=0; };
bool range(const void* p,std::size_t count,std::size_t width,std::size_t alignment,Range& r) noexcept {
    r={};if(!count)return true;
    if(!p||count>std::numeric_limits<std::size_t>::max()/width)return false;
    const auto address=reinterpret_cast<std::uintptr_t>(p),bytes=count*width;
    if(address%alignment||bytes>std::numeric_limits<std::uintptr_t>::max()-address)return false;
    r={address,address+bytes};return true;
}
bool overlap(Range a,Range b) noexcept {return a.begin<b.end&&b.begin<a.end;}
bool finite3(const float* v) noexcept {
    return std::isfinite(v[0])&&std::isfinite(v[1])&&std::isfinite(v[2]);
}
bool affine(const BodyMatrix48G& m) noexcept {
    for(float v:m.v)if(!std::isfinite(v))return false;
    return m.v[3]==0.f&&m.v[7]==0.f&&m.v[11]==0.f&&m.v[15]==1.f;
}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {volatile float x=a*b;return keep(x);}
    float root(float a) noexcept {volatile float x=std::sqrt(a);return keep(x);}
};
}
QueryPairStagingStatus48O stageQueryPairs48O(const QueryPairStagingInput48O& input,
    SpringVector4_48N* destination,std::size_t destinationCapacity,
    std::uint32_t* destinationMetadata,std::size_t destinationMetadataCapacity,
    std::uint32_t& shapeType) noexcept {
    Range inputRange{},tagRange{};
    if(!range(&input,1,sizeof(input),alignof(QueryPairStagingInput48O),inputRange)||
       !range(&shapeType,1,sizeof(shapeType),alignof(std::uint32_t),tagRange)||overlap(inputRange,tagRange))
        return QueryPairStagingStatus48O::InvalidInput;
    if(!input.shapeLinked){shapeType=3;return QueryPairStagingStatus48O::NoShape;}
    if(input.pairCount>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()/2))
        return QueryPairStagingStatus48O::InvalidInput;
    const std::size_t count=static_cast<std::size_t>(input.pairCount)*2;
    if(!count){shapeType=3;return QueryPairStagingStatus48O::Staged;}
    if(input.sourceCapacity<count||destinationCapacity<count||destinationMetadataCapacity<count||
       (input.sourceMetadata&&input.sourceMetadataCapacity<count))
        return QueryPairStagingStatus48O::InsufficientCapacity;
    Range source{},sourceMeta{},dest{},destMeta{};
    if(!range(input.source,input.sourceCapacity,sizeof(*input.source),alignof(SpringVector4_48N),source)||
       !range(destination,destinationCapacity,sizeof(*destination),alignof(SpringVector4_48N),dest)||
       !range(destinationMetadata,destinationMetadataCapacity,sizeof(*destinationMetadata),alignof(std::uint32_t),destMeta)||
       (input.sourceMetadata&&!range(input.sourceMetadata,input.sourceMetadataCapacity,sizeof(*input.sourceMetadata),alignof(std::uint32_t),sourceMeta)))
        return QueryPairStagingStatus48O::InvalidInput;
    const Range readOnly[]={source,sourceMeta,inputRange,tagRange};
    if(overlap(dest,destMeta))return QueryPairStagingStatus48O::InvalidInput;
    for(const auto r:readOnly)if(overlap(dest,r)||overlap(destMeta,r))return QueryPairStagingStatus48O::InvalidInput;
    if(overlap(tagRange,source)||overlap(tagRange,sourceMeta))return QueryPairStagingStatus48O::InvalidInput;
    // All writes follow the complete preflight. Byte copies preserve payloads,
    // including NaNs and fourth lanes without floating-point interpretation.
    shapeType=3;
    for(std::size_t i=0;i<count;++i){
        destinationMetadata[i]=input.sourceMetadata?input.sourceMetadata[i]:0;
        std::memcpy(destination+i,input.source+i,sizeof(*destination));
    }
    return QueryPairStagingStatus48O::Staged;
}
QuerySpherePrepareStatus48O prepareQuerySphere48O(
    const QuerySpherePrepareInput48O& input,QuerySpherePrepareOutput48O& output) noexcept {
    if(!input.shapeLinked||input.shapeType!=2||!input.scenePrepared||
       (!input.hasPairTree&&input.pairCount!=1))return QuerySpherePrepareStatus48O::UnsupportedScope;
    if(!affine(input.preparedScene))return QuerySpherePrepareStatus48O::InvalidInput;
    Math math;float center[3]{},half[3]{};
    if(input.hasPairTree){
        if(!finite3(input.treeCenter.v)||!finite3(input.treeHalf.v))return QuerySpherePrepareStatus48O::InvalidInput;
        for(unsigned i=0;i<3;++i){center[i]=input.treeCenter.v[i];half[i]=input.treeHalf.v[i];}
    }else{
        if(!finite3(input.first.v)||!finite3(input.second.v))return QuerySpherePrepareStatus48O::InvalidInput;
        for(unsigned i=0;i<3;++i){
            center[i]=math.mul(.5f,math.add(input.second.v[i],input.first.v[i]));
            half[i]=math.mul(.5f,math.sub(input.second.v[i],input.first.v[i]));
        }
    }
    QuerySpherePrepareOutput48O result{};std::memcpy(&result.world,&input.preparedScene,sizeof(result.world));
    const float xy=math.add(math.mul(half[0],half[0]),math.mul(half[1],half[1]));
    result.radius=math.root(math.add(xy,math.mul(half[2],half[2])));
    for(unsigned i=0;i<3;++i){
        const float xyCenter=math.add(math.mul(input.preparedScene.v[i],center[0]),math.mul(input.preparedScene.v[4+i],center[1]));
        const float xyzCenter=math.add(xyCenter,math.mul(input.preparedScene.v[8+i],center[2]));
        result.center.v[i]=math.add(xyzCenter,input.preparedScene.v[12+i]);
    }
    result.center.v[3]=1.f;
    if(!math.valid)return QuerySpherePrepareStatus48O::InvalidInput;
    output=result;return QuerySpherePrepareStatus48O::Ready;
}
}
