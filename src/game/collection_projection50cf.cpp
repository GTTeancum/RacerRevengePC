#include "collection_projection50cf.h"
#include <cstring>
#include <limits>
namespace rr::game {
CollectionProjection50CF projectCollectionObject50CF(const CollisionFilter48J&f,
    const CollisionSphere48J&s,std::uint8_t typeTag,std::uint32_t shapeTag,std::uint32_t w)noexcept{
 CollectionProjection50CF p{};
 if(!f.identity)return p;
 const auto limit=static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max());
 if(f.categoryMasks.size()>limit||f.identityLists[0].identities.size()>limit||f.identityLists[1].identities.size()>limit)return p;
 p.object.identity=f.identity;p.object.typeTag=typeTag;p.object.shapeTag=shapeTag;
 auto& d=p.object.filter;d.categoryA4=f.categoryA4;d.acceptMaskB8=f.acceptMaskB8;d.flags31=f.flags31;
 for(unsigned i=0;i<2;++i){const auto&v=f.identityLists[i];d.identities[i]={v.identities.data(),v.identities.size(),static_cast<std::int32_t>(v.identities.size()),v.mode};}
 d.categories={f.categoryMasks.data(),f.categoryMasks.size(),static_cast<std::int32_t>(f.categoryMasks.size()),f.categoryModeD4};
 p.state.flagsF8=f.flagsF8;std::memcpy(p.state.cachedCenter.v,s.center.v,12);std::memcpy(&p.state.cachedCenter.v[3],&w,4);p.state.cachedRadius=s.radius;p.valid=true;return p;
}
}
