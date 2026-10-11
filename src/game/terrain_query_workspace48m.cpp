#include "terrain_query_workspace48m.h"
#include "compound_query50ij.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace rr::game {
namespace {
struct Range48M { std::uintptr_t begin,end; };
template<class T>bool aligned48M(const T* p) noexcept {
    return reinterpret_cast<std::uintptr_t>(p)%alignof(T)==0;
}
bool range48M(const void* p,std::size_t count,std::size_t width,Range48M& r) noexcept {
    r={0,0};if(!count)return true;
    if(!p||count>std::numeric_limits<std::size_t>::max()/width)return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(p),bytes=count*width;
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-begin)return false;
    r={begin,begin+bytes};return true;
}
bool overlap48M(Range48M a,Range48M b) noexcept {return a.begin<b.end&&b.begin<a.end;}
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
TerrainQueryStatus48M queryTerrainViewBounded48M(const TerrainQueryView48M& query,
    const TerrainCandidate48J* candidates,std::size_t count,const TerrainQueryWorkspace48M& workspace,
    TerrainCandidateEffects48J* destination,std::size_t destinationCapacity,TerrainQueryResult48M& output) noexcept {
    if((count&&(!aligned48M(candidates)||!aligned48M(workspace.effects)||!aligned48M(destination)))||
       (workspace.treeStackCapacity&&!aligned48M(workspace.treeStack)))return TerrainQueryStatus48M::InvalidInput;
    if((count&&!candidates)||query.prepared.collectedCount>16||!finite(query.sphere)||
       !finite(query.prepared.surface.point150)||!finite(query.prepared.surface.normal170))return TerrainQueryStatus48M::InvalidInput;
    if(workspace.effectCapacity<count||destinationCapacity<count)return TerrainQueryStatus48M::InsufficientCapacity;
    Range48M scratch{},dest{},stack{},meta{},input{},q{},descriptor{};
    if(!range48M(workspace.effects,count,sizeof(*workspace.effects),scratch)||!range48M(destination,count,sizeof(*destination),dest)||
       !range48M(workspace.treeStack,workspace.treeStackCapacity,sizeof(*workspace.treeStack),stack)||
       !range48M(&output,1,sizeof(output),meta)||!range48M(candidates,count,sizeof(*candidates),input)||
       !range48M(&query,1,sizeof(query),q)||!range48M(&workspace,1,sizeof(workspace),descriptor))return TerrainQueryStatus48M::InvalidInput;
    const Range48M writable[]={scratch,dest,stack,meta};
    for(unsigned i=0;i<4;++i){
        for(unsigned j=0;j<i;++j)if(overlap48M(writable[i],writable[j]))return TerrainQueryStatus48M::InvalidInput;
        for(auto read:{input,q,descriptor})if(overlap48M(writable[i],read))return TerrainQueryStatus48M::InvalidInput;
    }
    const auto readSpan=[&](const void* p,std::size_t bytes) noexcept {
        Range48M read{};if(!range48M(p,bytes,1,read))return false;
        for(auto write:writable)if(overlap48M(write,read))return false;
        return true;
    };
    if(!readSpan(&query.segment,sizeof(query.segment))||!readSpan(&query.sphere,sizeof(query.sphere))||
       !readSpan(&query.filter,sizeof(query.filter))||!readSpan(&query.callbackMasks,sizeof(query.callbackMasks))||
       !readSpan(&query.prepared,sizeof(query.prepared))||!readSpan(&query.copied40,sizeof(query.copied40))||
       !readSpan(&query.copied50,sizeof(query.copied50)))return TerrainQueryStatus48M::InvalidInput;
    const auto readOnly=[&](const auto& values) noexcept {
        Range48M r{};if(!range48M(values.data(),values.size(),sizeof(*values.data()),r))return false;
        for(auto write:writable)if(overlap48M(write,r))return false;
        return true;
    };
    const auto readFilter=[&](const CollisionFilter48J& f) noexcept {
        return readOnly(f.categoryMasks)&&readOnly(f.identityLists[0].identities)&&readOnly(f.identityLists[1].identities);
    };
    if(!readFilter(query.filter))return TerrainQueryStatus48M::InvalidInput;
    for(std::size_t i=0;i<count;++i){
        if(!readFilter(candidates[i].filter))return TerrainQueryStatus48M::InvalidInput;
        if(const auto* compound=candidates[i].compound){
            if(!aligned48M(compound)||!readSpan(compound,sizeof(*compound))||compound->count>maxCompoundMembers50IJ||
               (compound->count&&!aligned48M(compound->parts))||!readSpan(compound->parts,compound->count*sizeof(*compound->parts)))return TerrainQueryStatus48M::InvalidInput;
            for(std::size_t j=0;j<compound->count;++j){const auto* mesh=compound->parts[j].mesh;
                if(!mesh||!aligned48M(mesh)||!readSpan(mesh,sizeof(*mesh))||!readOnly(mesh->nodes)||!readOnly(mesh->faces)||
                   !readOnly(mesh->vertices)||!readOnly(mesh->vertex_normals))return TerrainQueryStatus48M::InvalidInput;
            }
        }
        if(const auto* mesh=candidates[i].geometry.mesh){
            if(!aligned48M(mesh))return TerrainQueryStatus48M::InvalidInput;
            Range48M meshObject{};if(!range48M(mesh,1,sizeof(*mesh),meshObject))return TerrainQueryStatus48M::InvalidInput;
            for(auto write:writable)if(overlap48M(write,meshObject))return TerrainQueryStatus48M::InvalidInput;
            if(!readOnly(mesh->nodes)||!readOnly(mesh->faces)||!readOnly(mesh->vertices)||!readOnly(mesh->vertex_normals))return TerrainQueryStatus48M::InvalidInput;
        }
    }
    if(query.normalMode54!=1u)return TerrainQueryStatus48M::UnsupportedScope;
    TerrainQueryResult48M result{};result.normalModeWrite.bound=query.normalMode54Bound;
    {
        result.candidateCount=count;result.terrain=query.prepared;result.queryFlagsF8=static_cast<std::uint8_t>((query.filter.flagsF8|0x10u)&~0x20u);
        bool pairFound=false;
        for(std::size_t i=0;i<count;++i){
            const auto& c=candidates[i];
            workspace.effects[i]={c.filter.identity,c.filter.flagsF8,c.copied40,c.copied50};
            auto& effect=workspace.effects[i];
            if(c.typeTag!=query.collisionTypeTag){
                if(c.typeTag==query.groupTypeTag&&!(result.queryFlagsF8&1u))return TerrainQueryStatus48M::UnsupportedScope;
                continue;
            }
            result.terrain.currentCandidateIdentity=c.filter.identity;
            effect.flagsF8=static_cast<std::uint8_t>(effect.flagsF8|0x10u);
            bool overlap=false;
            if(!finite(c.sphere)||!spheres(query.sphere,c.sphere,overlap))return TerrainQueryStatus48M::InvalidInput;
            if(!overlap||!filter(query.filter,c.filter))continue;
            if(c.geometry.id!=c.filter.identity)return TerrainQueryStatus48M::InvalidInput;
            //121438 writes even if the shape/triangle subsequently misses.
            recordCollisionNormalMode50JA(result.normalModeWrite,query.normalMode54);
            CollisionResult48I hit{};
            std::size_t traceUsed=0;
            const CollisionWorkspace48M collision{workspace.treeStack,workspace.treeStackCapacity,nullptr,0};
            // Original126F40 dispatch admits no shape5 terrain hit (1270BC).
            // Do not give the terrain probe the new racer type2 Gate policy.
            if(c.shapeType==5)continue;
            if(c.shapeType>1||(c.shapeType==1&&!c.compound))return TerrainQueryStatus48M::UnsupportedScope;
            const auto status=c.shapeType==1?queryCompoundPair50IJ(query.segment,*c.compound,c.geometry.id,collision,hit):
                queryOrderedCollisionBounded48M(query.segment,&c.geometry,1,collision,hit,traceUsed);
            if(status==CollisionQueryStatus48I::InsufficientCapacity)return TerrainQueryStatus48M::InsufficientCapacity;
            if(status==CollisionQueryStatus48I::InvalidInput)return TerrainQueryStatus48M::InvalidInput;
            if(status==CollisionQueryStatus48I::UnsupportedInterpolationBoundary)return TerrainQueryStatus48M::UnsupportedInterpolationBoundary;
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
            if(c.hasResponseCallbackE8)return TerrainQueryStatus48M::UnsupportedScope;
        }
        auto& surface=result.terrain.surface;
        if(result.hasGeometryHit&&!result.terrain.excludedOwnerIdentity&&
           (!surface.hit110||!(result.collision.selected.point.v[1]<=surface.point150.v[1]))){
            surface.point150=result.collision.selected.point;surface.normal170=result.collision.selected.normal;
            surface.hit110=0;surface.hit114=1;
        }
        if(count)std::copy_n(workspace.effects,count,destination);
        output=result;return TerrainQueryStatus48M::Complete;
    }
}
TerrainQueryStatus48M queryTerrainLimitedBounded48M(const TerrainQuery48J& query,
    const TerrainCandidate48J* candidates,std::size_t count,const TerrainQueryWorkspace48M& workspace,
    TerrainCandidateEffects48J* destination,std::size_t destinationCapacity,TerrainQueryResult48M& output) noexcept {
    const TerrainQueryView48M view{query.segment,query.sphere,query.filter,query.callbackMasks,query.prepared,
        query.copied40,query.copied50,query.collisionTypeTag,query.groupTypeTag,query.terrainCallbackEnabled,query.normalMode54,query.normalMode54Bound};
    return queryTerrainViewBounded48M(view,candidates,count,workspace,destination,destinationCapacity,output);
}
TerrainPreparedQueryStatus48M prepareAndQueryTerrain48M(const TerrainPrepareInput48M& preparation,
    const TerrainQuery48J& metadata,const TerrainCandidate48J* candidates,std::size_t count,
    const TerrainQueryWorkspace48M& workspace,TerrainCandidateEffects48J* destination,std::size_t capacity,
    TerrainPreparedQueryResult48M& output) noexcept {
    Range48M resultRange{},prepRange{},queryRange{},candidateRange{},workspaceRange{},scratch{},stack{},dest{};
    if(!range48M(&output,1,sizeof(output),resultRange)||!range48M(&preparation,1,sizeof(preparation),prepRange)||
       !range48M(&metadata,1,sizeof(metadata),queryRange)||!range48M(&workspace,1,sizeof(workspace),workspaceRange))
        return TerrainPreparedQueryStatus48M::InvalidInput;
    for(auto read:{prepRange,queryRange,workspaceRange})if(overlap48M(resultRange,read))return TerrainPreparedQueryStatus48M::InvalidInput;
    // Even skipped queries must not write result metadata over effect output.
    if(destination&&(!range48M(destination,count,sizeof(*destination),dest)||overlap48M(resultRange,dest)))
        return TerrainPreparedQueryStatus48M::InvalidInput;
    if(candidates&&(!range48M(candidates,count,sizeof(*candidates),candidateRange)||overlap48M(resultRange,candidateRange)))
        return TerrainPreparedQueryStatus48M::InvalidInput;
    if((count&&(!candidates||!aligned48M(candidates)||!aligned48M(workspace.effects)||!aligned48M(destination)))||
       (workspace.treeStackCapacity&&!aligned48M(workspace.treeStack)))return TerrainPreparedQueryStatus48M::InvalidInput;
    const auto avoidOutput=[&](const void* data,std::size_t n,std::size_t width) noexcept {
        Range48M span{};return range48M(data,n,width,span)&&!overlap48M(resultRange,span);
    };
    const auto vectorAvoidsOutput=[&](const auto& values) noexcept {
        return avoidOutput(values.data(),values.size(),sizeof(*values.data()));
    };
    const auto filterAvoidsOutput=[&](const CollisionFilter48J& f) noexcept {
        return vectorAvoidsOutput(f.categoryMasks)&&vectorAvoidsOutput(f.identityLists[0].identities)&&vectorAvoidsOutput(f.identityLists[1].identities);
    };
    if(!filterAvoidsOutput(metadata.filter))return TerrainPreparedQueryStatus48M::InvalidInput;
    for(std::size_t i=0;i<count;++i){
        const auto& candidate=candidates[i];
        if(!filterAvoidsOutput(candidate.filter))return TerrainPreparedQueryStatus48M::InvalidInput;
        if(const auto* compound=candidate.compound){
            if(!aligned48M(compound)||!avoidOutput(compound,1,sizeof(*compound))||compound->count>maxCompoundMembers50IJ||
               (compound->count&&!aligned48M(compound->parts))||!avoidOutput(compound->parts,compound->count,sizeof(*compound->parts)))return TerrainPreparedQueryStatus48M::InvalidInput;
            for(std::size_t j=0;j<compound->count;++j){const auto* mesh=compound->parts[j].mesh;
                if(!mesh||!aligned48M(mesh)||!avoidOutput(mesh,1,sizeof(*mesh))||!vectorAvoidsOutput(mesh->nodes)||
                   !vectorAvoidsOutput(mesh->faces)||!vectorAvoidsOutput(mesh->vertices)||!vectorAvoidsOutput(mesh->vertex_normals))return TerrainPreparedQueryStatus48M::InvalidInput;
            }
        }
        if(const auto* mesh=candidate.geometry.mesh){
            if(!aligned48M(mesh)||!avoidOutput(mesh,1,sizeof(*mesh)))return TerrainPreparedQueryStatus48M::InvalidInput;
            if(!vectorAvoidsOutput(mesh->nodes)||!vectorAvoidsOutput(mesh->faces)||!vectorAvoidsOutput(mesh->vertices)||
               !vectorAvoidsOutput(mesh->vertex_normals))return TerrainPreparedQueryStatus48M::InvalidInput;
        }
    }
    TerrainPreparedQueryResult48M result{};
    const auto prepared=prepareTerrainSinglePair48M(preparation,result.state);
    if(prepared==TerrainPrepareStatus48M::InvalidInput)return TerrainPreparedQueryStatus48M::InvalidInput;
    if(prepared==TerrainPrepareStatus48M::UnsupportedScope)return TerrainPreparedQueryStatus48M::UnsupportedScope;
    if(prepared==TerrainPrepareStatus48M::NoBody){output=result;return TerrainPreparedQueryStatus48M::NoBody;}
    if(prepared==TerrainPrepareStatus48M::QueryDisabled){output=result;return TerrainPreparedQueryStatus48M::QueryDisabled;}
    if(!range48M(candidates,count,sizeof(*candidates),candidateRange)||
       !range48M(workspace.effects,count,sizeof(*workspace.effects),scratch)||
       !range48M(workspace.treeStack,workspace.treeStackCapacity,sizeof(*workspace.treeStack),stack)||
       !range48M(destination,count,sizeof(*destination),dest))return TerrainPreparedQueryStatus48M::InvalidInput;
    for(auto span:{candidateRange,scratch,stack,dest})if(overlap48M(resultRange,span))return TerrainPreparedQueryStatus48M::InvalidInput;
    for(auto span:{scratch,stack,dest})if(overlap48M(prepRange,span)||overlap48M(queryRange,span))return TerrainPreparedQueryStatus48M::InvalidInput;
    const TerrainQueryView48M view{result.state.segment,result.state.sphere,metadata.filter,metadata.callbackMasks,result.state.terrain,
        metadata.copied40,metadata.copied50,metadata.collisionTypeTag,metadata.groupTypeTag,metadata.terrainCallbackEnabled,metadata.normalMode54,metadata.normalMode54Bound};
    const auto queried=queryTerrainViewBounded48M(view,candidates,count,workspace,destination,capacity,result.query);
    switch(queried){
        case TerrainQueryStatus48M::InvalidInput:return TerrainPreparedQueryStatus48M::InvalidInput;
        case TerrainQueryStatus48M::UnsupportedScope:return TerrainPreparedQueryStatus48M::UnsupportedScope;
        case TerrainQueryStatus48M::UnsupportedInterpolationBoundary:return TerrainPreparedQueryStatus48M::UnsupportedInterpolationBoundary;
        case TerrainQueryStatus48M::InsufficientCapacity:return TerrainPreparedQueryStatus48M::InsufficientCapacity;
        case TerrainQueryStatus48M::Complete:break;
    }
    result.state.terrain=result.query.terrain;result.queryExecuted=true;output=result;
    return TerrainPreparedQueryStatus48M::Queried;
}

}
