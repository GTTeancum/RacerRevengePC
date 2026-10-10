#include "collection_bounds48s.h"
#include "scalar_minmax48o.h"
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using Vec=SpringVector4_48N;
struct Span {std::uintptr_t begin,end;};
template<class T>bool span(const T* p,std::size_t count,Span& out) noexcept {
    out={0,0};if(!count)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||
       count>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto address=reinterpret_cast<std::uintptr_t>(p),bytes=count*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-address)return false;
    out={address,address+bytes};return true;
}
bool overlap(Span a,Span b) noexcept {return a.begin<b.end&&b.begin<a.end;}
struct Math {
    bool valid=true;
    float keep(float f) noexcept {valid=valid&&std::isfinite(f);return f;}
    float add(float a,float b) noexcept {volatile float f=a+b;return keep(f);}
    float sub(float a,float b) noexcept {volatile float f=a-b;return keep(f);}
    float mul(float a,float b) noexcept {volatile float f=a*b;return keep(f);}
    float transform(const BodyMatrix48G& m,const Vec& p,unsigned axis,bool point) noexcept {
        float f=mul(keep(m.v[axis]),keep(p.v[0]));
        f=add(f,mul(keep(m.v[4+axis]),keep(p.v[1])));
        f=add(f,mul(keep(m.v[8+axis]),keep(p.v[2])));
        return add(f,point?keep(m.v[12+axis]):0.f);
    }
    float radius(const Vec& p) noexcept {
        const float x=mul(p.v[0],p.v[0]),y=mul(p.v[1],p.v[1]);
        const float xy=add(x,y),z=mul(p.v[2],p.v[2]);
        volatile float f=std::sqrt(add(xy,z));return keep(f);
    }
};
// VU MINI/MAX signed-word selectors107924/120740/11E314/12077C,
// independently audited in evidence/vuMinMax48P. In this ordinary finite
// domain the scalar ordering key is equivalent, including -0 below +0.
float vuMin(float a,float b) noexcept {
    return detail::scalarOrderKey48O(a)<detail::scalarOrderKey48O(b)?a:b;
}
float vuMax(float a,float b) noexcept {
    return detail::scalarOrderKey48O(a)>detail::scalarOrderKey48O(b)?a:b;
}
}
CollectionBoundsStatus48S prepareCollectionBounds48S(
    const CollectionBoundsInput48S& in,CollectionBoundsResult48S& out) noexcept {
    using Status=CollectionBoundsStatus48S;
    Span reads[4]{},write{};
    if(!span(&in,1,reads[0])||!span(in.endpoints,in.endpointCount,reads[1])||
       !span(in.endpointScenes,in.endpointSceneCount,reads[2])||
       !span(in.scenes,in.sceneCount,reads[3])||!span(&out,1,write))return Status::InvalidInput;
    for(auto read:reads)if(overlap(write,read))return Status::InvalidInput;
    if(in.shapeTag!=1&&in.shapeTag!=3&&in.shapeTag!=6)return Status::UnsupportedBounds;
    CollectionBoundsResult48S result{};
    std::memcpy(&result.cachedCenter,&in.cachedCenter,sizeof(result.cachedCenter));
    std::memcpy(&result.cachedRadius,&in.cachedRadius,sizeof(result.cachedRadius));
    Vec center{},half{};BodyMatrix48G matrix{};Math math;
    if(in.shapeTag==3){
        if(in.pairCount<=0||static_cast<std::uint32_t>(in.pairCount)>
           std::numeric_limits<std::size_t>::max()/2)return Status::InvalidInput;
        const auto count=static_cast<std::size_t>(in.pairCount)*2;
        if(in.endpointCount<count||((in.endpointScenes||in.endpointSceneCount)&&in.endpointSceneCount<count))
            return Status::InsufficientCapacity;
        float lower[3],upper[3];
        for(unsigned k=0;k<3;++k){lower[k]=std::numeric_limits<float>::max();upper[k]=-lower[k];}
        for(std::size_t i=0;i<count;i+=2){
            Vec endpoints[2]{};
            for(unsigned j=0;j<2;++j){
                const auto binding=in.endpointScenes?in.endpointScenes[i+j]:0u;
                if(binding&&static_cast<std::size_t>(binding)>in.sceneCount)return Status::InvalidInput;
                for(unsigned k=0;k<3;++k)endpoints[j].v[k]=binding?
                    math.transform(in.scenes[binding-1],in.endpoints[i+j],k,true):math.keep(in.endpoints[i+j].v[k]);
            }
            if(!math.valid)return Status::NonFinite;
            for(unsigned k=0;k<3;++k){
                lower[k]=vuMin(vuMin(lower[k],endpoints[1].v[k]),endpoints[0].v[k]);
                upper[k]=vuMax(vuMax(upper[k],endpoints[1].v[k]),endpoints[0].v[k]);
            }
        }
        //1207C8..120884 computes half first, then cached center. Identity
        // transformation is deliberately retained below rather than folded.
        for(unsigned k=0;k<3;++k)half.v[k]=math.mul(.5f,math.sub(upper[k],lower[k]));
        for(unsigned k=0;k<3;++k)center.v[k]=math.mul(.5f,math.add(upper[k],lower[k]));
        center.v[3]=half.v[3]=1.f;
        result.cachedCenter=center;result.cachedRadius=math.radius(half);
        matrix.v[0]=matrix.v[5]=matrix.v[10]=matrix.v[15]=1.f;
    }else{
        std::memcpy(&center,&in.localCenter,sizeof(center));
        std::memcpy(&half,&in.localHalf,sizeof(half));
        std::memcpy(&matrix,&in.shapeWorld,sizeof(matrix));
    }
    result.center.v[3]=1.f;
    const Vec x{{half.v[0],0,0,1}},y{{0,half.v[1],0,1}},z{{0,0,half.v[2],1}};
    for(unsigned k=0;k<3;++k){
        result.center.v[k]=math.transform(matrix,center,k,true);
        const float tx=math.transform(matrix,x,k,false),ty=math.transform(matrix,y,k,false),tz=math.transform(matrix,z,k,false);
        result.half.v[k]=math.add(std::fabs(tz),math.add(std::fabs(tx),std::fabs(ty)));
    }
    if(!math.valid)return Status::NonFinite;
    std::memcpy(&out,&result,sizeof(result));return Status::Complete;
}
}
