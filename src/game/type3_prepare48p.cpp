#include "type3_prepare48p.h"
#include "scalar_minmax48o.h"
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
struct Range {std::uintptr_t begin=0,end=0;};
bool range(const void* p,std::size_t count,std::size_t width,std::size_t alignment,Range& r) noexcept {
    r={};if(!count)return true;
    if(!p||count>std::numeric_limits<std::size_t>::max()/width)return false;
    const auto address=reinterpret_cast<std::uintptr_t>(p),bytes=count*width;
    if(address%alignment||bytes>std::numeric_limits<std::uintptr_t>::max()-address)return false;
    r={address,address+bytes};return true;
}
bool overlap(Range a,Range b) noexcept {return a.begin<b.end&&b.begin<a.end;}
template<class T>bool span(T* p,std::size_t count,Range& r) noexcept {
    return range(p,count,sizeof(T),alignof(T),r);
}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {volatile float x=a*b;return keep(x);}
    float divide(float a,float b) noexcept {volatile float x=a/b;return keep(x);}
    float root(float a) noexcept {volatile float x=std::sqrt(a);return keep(x);}
    float length(const float* v) noexcept {
        const float xx=mul(v[0],v[0]),yy=mul(v[1],v[1]);
        const float xy=add(xx,yy),zz=mul(v[2],v[2]);
        return root(add(xy,zz));
    }
};
// Independently audited VU MINI/MAX at 107924,11E120,11E15C,11E314:
// PCSX2 pcsx2/VUops.cpp 724..773 uses signed-word ordering with a reversed
// both-negative case. In this finite domain it equals this scalar order key,
// including -0 below +0, unlike host comparison ties. This reuse does not
// infer VU semantics from scalar MIN.S/MAX.S or change branch comparisons.
float vuMin(float a,float b) noexcept {
    return detail::scalarOrderKey48O(a)<detail::scalarOrderKey48O(b)?a:b;
}
float vuMax(float a,float b) noexcept {
    return detail::scalarOrderKey48O(a)>detail::scalarOrderKey48O(b)?a:b;
}
bool ray(const SpringVector4_48N& start,const SpringVector4_48N& end,
         const SpringVector4_48N& sentinel,float& length,SpringVector4_48N& direction) noexcept {
    Math math;float delta[3];
    for(unsigned k=0;k<3;++k){
        if(!std::isfinite(start.v[k])||!std::isfinite(end.v[k]))return false;
        delta[k]=math.sub(end.v[k],start.v[k]);
    }
    length=math.length(delta);
    if(length<=0.f)std::memcpy(&direction,&sentinel,sizeof(direction));
    else {
        const float reciprocal=math.divide(1.f,length);
        for(unsigned k=0;k<3;++k)direction.v[k]=math.mul(delta[k],reciprocal);
        direction.v[3]=1.f;
    }
    return math.valid;
}
}
Type3PrepareStatus48P prepareType3Rays48P(const Type3PrepareInput48P& input,
    Type3RayBuffers48P& output,Type3QueryState48P& state) noexcept {
    using Status=Type3PrepareStatus48P;
    Range descriptors[3]{};
    if(!span(&input,1,descriptors[0])||!span(&output,1,descriptors[1])||!span(&state,1,descriptors[2]))
        return Status::InvalidInput;
    for(unsigned i=0;i<3;++i)for(unsigned j=0;j<i;++j)
        if(overlap(descriptors[i],descriptors[j]))return Status::AliasedBuffers;
    if(!input.pairCount||input.pairCount>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())||
       input.pairCount>std::numeric_limits<std::size_t>::max()/2)return Status::InvalidInput;
    const auto count=static_cast<std::size_t>(input.pairCount),endpointCount=count*2;
    if(input.endpointCount<endpointCount||input.endpointSceneCount<endpointCount||
       output.capacity<count||output.lengthCapacity<count)return Status::CapacityExceeded;
    Range sources[2]{},buffers[9]{};
    if(!span(input.endpoints,input.endpointCount,sources[0])||
       !span(input.endpointScenes,input.endpointSceneCount,sources[1])||
       !span(output.worldStarts,output.capacity,buffers[0])||
       !span(output.worldDirections,output.capacity,buffers[1])||
       !span(output.lengths,output.lengthCapacity,buffers[2])||
       !span(output.localStarts,output.localStarts?output.capacity:0,buffers[3])||
       !span(output.localDirections,output.localDirections?output.capacity:0,buffers[4])||
       !span(output.normals,output.normals?output.capacity:0,buffers[5])||
       !span(output.points,output.points?output.capacity:0,buffers[6])||
       !span(output.hits,output.hits?output.capacity:0,buffers[7])||
       !span(output.fractions,output.fractions?output.capacity:0,buffers[8]))return Status::InvalidInput;
    for(unsigned i=0;i<9;++i){
        for(unsigned j=0;j<i;++j)if(overlap(buffers[i],buffers[j]))return Status::AliasedBuffers;
        for(const auto source:sources)if(overlap(buffers[i],source))return Status::AliasedBuffers;
        for(const auto descriptor:descriptors)if(overlap(buffers[i],descriptor))return Status::AliasedBuffers;
    }
    for(const auto source:sources)if(overlap(source,descriptors[2]))return Status::AliasedBuffers;
    for(std::size_t i=0;i<endpointCount;++i)if(input.endpointScenes[i])return Status::UnsupportedScope;
    float lower[3],upper[3];
    for(unsigned k=0;k<3;++k){lower[k]=std::numeric_limits<float>::max();upper[k]=-lower[k];}
    // First pass proves every ray and all aggregate arithmetic before any
    // output is touched. Endpoint W is never consumed by retained arithmetic.
    for(std::size_t i=0;i<count;++i){
        const auto& start=input.endpoints[2*i];const auto& end=input.endpoints[2*i+1];
        float length;SpringVector4_48N direction;
        if(!ray(start,end,input.sentinel,length,direction))return Status::NonFinite;
        for(unsigned k=0;k<3;++k){
            lower[k]=vuMin(vuMin(lower[k],end.v[k]),start.v[k]);
            upper[k]=vuMax(vuMax(upper[k],end.v[k]),start.v[k]);
        }
    }
    Math math;SpringVector4_48N center{};float half[3];
    for(unsigned k=0;k<3;++k){
        center.v[k]=math.mul(.5f,math.add(upper[k],lower[k]));
        half[k]=math.mul(.5f,math.sub(upper[k],lower[k]));
    }
    center.v[3]=1.f;const float radius=math.length(half);
    if(!math.valid)return Status::NonFinite;
    // The disjoint, immutable sources and successful first pass make commit
    // infallible in the supported serial, stable floating-environment domain.
    for(std::size_t i=0;i<count;++i){
        float length;SpringVector4_48N direction;
        (void)ray(input.endpoints[2*i],input.endpoints[2*i+1],input.sentinel,length,direction);
        std::memcpy(output.worldStarts+i,input.endpoints+2*i,sizeof(*output.worldStarts));
        std::memcpy(output.worldDirections+i,&direction,sizeof(direction));output.lengths[i]=length;
    }
    state.worldCenter=center;state.queryCenter=center;state.radius=radius;state.queryRadius=radius;
    std::memcpy(&state.result00,&input.sentinel,sizeof(state.result00));
    std::memcpy(&state.result10,&input.sentinel,sizeof(state.result10));
    state.selectedFraction=1.f;state.hitCount=0;return Status::Complete;
}
}
