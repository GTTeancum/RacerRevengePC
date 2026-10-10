#include "type0_bounds50ew.h"
#include <cfenv>
#include <cstring>
#include <limits>
namespace rr::game {namespace {
bool overlap(const void*a,std::size_t an,const void*b,std::size_t bn) noexcept {
    const auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);
    return x<=y?y-x<an:x-y<bn;
}
}
CollectionBoundsStatus48S prepareType0Bounds50EW(const Type0BoundsInput50EW& in,
    CollectionBoundsResult48S& out) noexcept {
    using S=CollectionBoundsStatus48S;
    const auto address=reinterpret_cast<std::uintptr_t>(in.resource);
    if(!in.resource||address%alignof(PodShapeSource49W)||
       sizeof(PodShapeSource49W)>std::numeric_limits<std::uintptr_t>::max()-address||
       overlap(&in,sizeof in,&out,sizeof out)||
       overlap(in.resource,sizeof(*in.resource),&out,sizeof out)||std::fegetround()!=FE_TONEAREST)return S::InvalidInput;
    CollectionBoundsInput48S projection{};projection.shapeTag=1;
    projection.shapeWorld=in.composed00;
    for(unsigned i=0;i<3;++i){projection.localCenter.v[i]=in.resource->rootCenter.v[i];
        projection.localHalf.v[i]=in.resource->rootHalf.v[i];}
    projection.localCenter.v[3]=projection.localHalf.v[3]=1.f;
    std::memcpy(projection.cachedCenter.v,in.cachedCenterWords,16);
    std::memcpy(&projection.cachedRadius,&in.cachedRadiusWord,4);
    return prepareCollectionBounds48S(projection,out);
}
}
