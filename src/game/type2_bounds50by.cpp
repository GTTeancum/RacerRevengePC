#include "type2_bounds50by.h"
#include <cstring>
#include <cmath>
#include <cfenv>
#include <limits>
namespace rr::game{namespace{
struct Span{std::uintptr_t b=0,e=0;};
template<class T>bool span(const T*p,std::size_t n,Span&v)noexcept{v={};if(!n)return true;if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;auto b=reinterpret_cast<std::uintptr_t>(p),size=n*sizeof(T);if(size>std::numeric_limits<std::uintptr_t>::max()-b)return false;v={b,b+size};return true;}
bool overlap(Span a,Span b)noexcept{return a.b<b.e&&b.b<a.e;}
}
CollectionBoundsStatus48S prepareType2Bounds50BY(const Type2BoundsInput50BY&in,CollectionBoundsResult48S&out)noexcept{
 using S=CollectionBoundsStatus48S;Span writes{},input{},resource{},nodes{},rays{};
 if(!span(&out,1,writes)||!span(&in,1,input)||!span(in.resource,1,resource)||overlap(writes,input)||overlap(writes,resource))return S::InvalidInput;
 const auto& view=*in.resource;
 if(!span(view.nodes,view.nodeCount,nodes)||!span(view.rays,view.rayCount,rays)||overlap(writes,nodes)||overlap(writes,rays)||std::fegetround()!=FE_TONEAREST)return S::InvalidInput;
 CollectionBoundsInput48S projection{};projection.shapeTag=1;std::memcpy(&projection.shapeWorld,&in.preparedWorld,sizeof in.preparedWorld);std::memcpy(&projection.cachedCenter,in.cachedCenterWords,16);std::memcpy(&projection.cachedRadius,&in.cachedRadiusWord,4);
 if(view.nodeCount){if((view.root&type2Leaf49X)||view.root>=view.nodeCount)return S::InvalidInput;const auto&root=view.nodes[view.root];for(unsigned k=0;k<3;++k){projection.localCenter.v[k]=root.center[k];projection.localHalf.v[k]=root.half[k];}}
 else{if(view.root!=type2Leaf49X||!view.rayCount)return S::InvalidInput;const auto&ray=view.rays[0];for(unsigned k=0;k<3;++k){const auto a=ray.first.v[k],b=ray.second.v[k];if(!std::isfinite(a)||!std::isfinite(b))return S::NonFinite;volatile float sum=b+a,difference=b-a;volatile float center=.5f*sum,half=.5f*difference;projection.localCenter.v[k]=center;projection.localHalf.v[k]=half;}}
 projection.localCenter.v[3]=projection.localHalf.v[3]=1.f;
 return prepareCollectionBounds48S(projection,out);
}
}
