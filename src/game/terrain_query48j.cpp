#include "terrain_query48j.h"
#include <algorithm>
#include <cmath>
#include <new>

namespace rr::game {
namespace {
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
bool finite(const Vector3_48C& v) noexcept {for(float x:v.v)if(!std::isfinite(x))return false;return true;}
bool finite(const CollisionSphere48J& s) noexcept {return finite(s.center)&&std::isfinite(s.radius)&&s.radius>=0.f;}
bool member(const std::vector<std::uint32_t>& values,std::uint32_t id) noexcept {
    return std::find(values.begin(),values.end(),id)!=values.end();
}
bool identitiesPass(const CollisionIdentityList48J& list,std::uint32_t other) noexcept {
    // Empty identity lists bypass the mode test in the original.
    if(list.identities.empty())return true;
    const bool found=member(list.identities,other);
    if(found&&list.mode==1)return false;
    if(!found&&list.mode==0)return false;
    return true;
}
bool categoriesPass(const CollisionFilter48J& f,std::uint32_t other) noexcept {
    bool found=false;for(auto mask:f.categoryMasks)if(mask&other){found=true;break;}
    // Unlike identity lists, an empty category allow-list rejects.
    return f.categoryModeD4==1?!found:found;
}
bool filter(const CollisionFilter48J& query,const CollisionFilter48J& candidate) noexcept {
    if(query.identity==candidate.identity||!(candidate.flagsF8&4u)||!(candidate.flags31&1u))return false;
    if(!(candidate.categoryA4&query.acceptMaskB8))return false;
    for(unsigned i=0;i<2;++i)
        if(!identitiesPass(query.identityLists[i],candidate.identity)||
           !identitiesPass(candidate.identityLists[i],query.identity))return false;
    return categoriesPass(query,candidate.categoryA4)&&categoriesPass(candidate,query.categoryA4);
}
bool spheres(const CollisionSphere48J& a,const CollisionSphere48J& b,bool& overlap) noexcept {
    const float x=sub(a.center.v[0],b.center.v[0]);
    const float y=sub(a.center.v[1],b.center.v[1]);
    const float z=sub(a.center.v[2],b.center.v[2]);
    const float squared=add(add(mul(x,x),mul(y,y)),mul(z,z));
    const float radius=add(a.radius,b.radius);
    volatile float distance=std::sqrt(squared);
    const float gap=sub(distance,radius);
    if(!std::isfinite(gap))return false;
    overlap=gap<=0.f;return true;
}
}
TerrainQueryStatus48J queryTerrainLimited48J(const TerrainQuery48J& query,
    const TerrainCandidate48J* candidates,std::size_t count,TerrainQueryResult48J& output) noexcept {
    if((count&&!candidates)||query.prepared.collectedCount>16||!finite(query.sphere)||
       !finite(query.prepared.surface.point150)||!finite(query.prepared.surface.normal170))return TerrainQueryStatus48J::InvalidInput;
    TerrainQueryResult48J result{};
    try {
        result.terrain=query.prepared;result.queryFlagsF8=static_cast<std::uint8_t>((query.filter.flagsF8|0x10u)&~0x20u);
        result.candidates.reserve(count);
        bool pairFound=false;
        for(std::size_t i=0;i<count;++i){
            const auto& c=candidates[i];
            result.candidates.push_back({c.filter.identity,c.filter.flagsF8,c.copied40,c.copied50});
            auto& effect=result.candidates.back();
            if(c.typeTag!=query.collisionTypeTag){
                if(c.typeTag==query.groupTypeTag&&!(result.queryFlagsF8&1u))return TerrainQueryStatus48J::UnsupportedScope;
                continue;
            }
            result.terrain.currentCandidateIdentity=c.filter.identity;
            effect.flagsF8=static_cast<std::uint8_t>(effect.flagsF8|0x10u);
            bool overlap=false;
            if(!finite(c.sphere)||!spheres(query.sphere,c.sphere,overlap))return TerrainQueryStatus48J::InvalidInput;
            if(!overlap||!filter(query.filter,c.filter))continue;
            if(c.geometry.id!=c.filter.identity)return TerrainQueryStatus48J::InvalidInput;
            if(c.shapeType!=0)return TerrainQueryStatus48J::UnsupportedScope; // bounded48M supplies compound1
            CollisionResult48I hit{};
            const auto status=queryOrderedCollision48I(query.segment,&c.geometry,1,hit);
            if(status==CollisionQueryStatus48I::InvalidInput)return TerrainQueryStatus48J::InvalidInput;
            if(status==CollisionQueryStatus48I::UnsupportedInterpolationBoundary)return TerrainQueryStatus48J::UnsupportedInterpolationBoundary;
            if(status!=CollisionQueryStatus48I::Hit)continue;
            result.queryFlagsF8=static_cast<std::uint8_t>(result.queryFlagsF8|0x20u);
            if(!result.hasGeometryHit||hit.selected.fraction<=result.collision.selected.fraction)result.collision.selected=hit.selected;
            if(!pairFound||hit.nearestPair.fraction<result.collision.nearestPair.fraction){result.collision.nearestPair=hit.nearestPair;pairFound=true;}
            result.hasGeometryHit=true;
            // 11E330 does not change query+40/+50 for type2; 122848 copies them.
            effect.copied40=query.copied40;effect.copied50=query.copied50;
            if(query.terrainCallbackEnabled){
                auto& state=result.terrain;const auto category=c.filter.categoryA4;
                if((category&query.callbackMasks.collect)&&state.collectedCount<16)
                    state.collectedIdentities[state.collectedCount++]=c.filter.identity;
                if(category&query.callbackMasks.forceAngle)state.surface.forceAngleTest108=1;
                if(category&query.callbackMasks.excludeOwner)state.excludedOwnerIdentity=c.ownerIdentity9C;
            }
            if(c.hasResponseCallbackE8)return TerrainQueryStatus48J::UnsupportedScope;
        }
        auto& surface=result.terrain.surface;
        if(result.hasGeometryHit&&!result.terrain.excludedOwnerIdentity&&
           (!surface.hit110||!(result.collision.selected.point.v[1]<=surface.point150.v[1]))){
            surface.point150=result.collision.selected.point;surface.normal170=result.collision.selected.normal;
            surface.hit110=0;surface.hit114=1;
        }
        output=std::move(result);return TerrainQueryStatus48J::Complete;
    }catch(const std::bad_alloc&){return TerrainQueryStatus48J::InvalidInput;}
}
}
