#include "spring_query48n.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
struct Range48N { std::uintptr_t begin,end; };
template<class T>bool aligned48N(const T* p) noexcept {
    return reinterpret_cast<std::uintptr_t>(p)%alignof(T)==0;
}
bool range48N(const void* p,std::size_t count,std::size_t width,Range48N& r) noexcept {
    r={0,0};if(!count)return true;
    if(!p||count>std::numeric_limits<std::size_t>::max()/width)return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(p),bytes=count*width;
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-begin)return false;
    r={begin,begin+bytes};return true;
}
bool overlap48N(Range48N a,Range48N b) noexcept {return a.begin<b.end&&b.begin<a.end;}
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
SpringQueryStatus48N queryStaticSpringBounded48N(const SpringQueryView48N& query,
    const SpringStaticCandidate48N* candidates,std::size_t count,
    const SpringRecordState48N& initialState,const SpringRecord48N* initialRecords,
    std::size_t recordCount,const SpringQueryWorkspace48N& workspace,
    SpringRecord48N* destinationRecords,std::size_t destinationRecordCapacity,
    TerrainCandidateEffects48J* destinationEffects,std::size_t destinationEffectCapacity,
    SpringQueryResult48N& output) noexcept {
    using Status=SpringQueryStatus48N;
    if((count&&!aligned48N(candidates))||(workspace.effectCapacity&&!aligned48N(workspace.effects))||
       (destinationEffectCapacity&&!aligned48N(destinationEffects))||(recordCount&&!aligned48N(initialRecords))||
       (workspace.recordCapacity&&!aligned48N(workspace.records))||
       (destinationRecordCapacity&&!aligned48N(destinationRecords))||
       (workspace.treeStackCapacity&&!aligned48N(workspace.treeStack)))return Status::InvalidInput;
    if(initialState.generatedCapacity88<0||initialState.generatedCount84<0||!finite(query.sphere))return Status::InvalidInput;
    if(static_cast<std::size_t>(initialState.generatedCapacity88)>recordCount||workspace.recordCapacity<recordCount||
       destinationRecordCapacity<recordCount||workspace.effectCapacity<count||destinationEffectCapacity<count)
        return Status::InsufficientCapacity;
    Range48N recordScratch{},recordDest{},effectScratch{},effectDest{},stack{},meta{};
    Range48N input{},q{},descriptor{},initial{},recordInput{};
    if(!range48N(workspace.records,workspace.recordCapacity,sizeof(*workspace.records),recordScratch)||
       !range48N(destinationRecords,destinationRecordCapacity,sizeof(*destinationRecords),recordDest)||
       !range48N(workspace.effects,workspace.effectCapacity,sizeof(*workspace.effects),effectScratch)||
       !range48N(destinationEffects,destinationEffectCapacity,sizeof(*destinationEffects),effectDest)||
       !range48N(workspace.treeStack,workspace.treeStackCapacity,sizeof(*workspace.treeStack),stack)||
       !range48N(&output,1,sizeof(output),meta)||!range48N(candidates,count,sizeof(*candidates),input)||
       !range48N(&query,1,sizeof(query),q)||!range48N(&workspace,1,sizeof(workspace),descriptor)||
       !range48N(&initialState,1,sizeof(initialState),initial)||
       !range48N(initialRecords,recordCount,sizeof(*initialRecords),recordInput))return Status::InvalidInput;
    const Range48N writable[]={recordScratch,recordDest,effectScratch,effectDest,stack,meta};
    for(unsigned i=0;i<6;++i){
        for(unsigned j=0;j<i;++j)if(overlap48N(writable[i],writable[j]))return Status::InvalidInput;
        for(auto read:{input,q,descriptor,initial,recordInput})if(overlap48N(writable[i],read))return Status::InvalidInput;
    }
    const auto readSpan=[&](const void* p,std::size_t n,std::size_t width) noexcept {
        Range48N read{};if(!range48N(p,n,width,read))return false;
        for(auto write:writable)if(overlap48N(write,read))return false;
        return true;
    };
    if(!readSpan(&query.segment,1,sizeof(query.segment))||!readSpan(&query.sphere,1,sizeof(query.sphere))||
       !readSpan(&query.filter,1,sizeof(query.filter))||!readSpan(&query.copied40,1,sizeof(query.copied40))||
       !readSpan(&query.copied50,1,sizeof(query.copied50)))return Status::InvalidInput;
    const auto readVector=[&](const auto& values) noexcept {
        return readSpan(values.data(),values.size(),sizeof(*values.data()));
    };
    const auto readFilter=[&](const CollisionFilter48J& f) noexcept {
        return readVector(f.categoryMasks)&&readVector(f.identityLists[0].identities)&&readVector(f.identityLists[1].identities);
    };
    if(!readFilter(query.filter))return Status::InvalidInput;
    for(std::size_t i=0;i<count;++i){
        const auto& c=candidates[i];
        if(!readFilter(c.collision.filter)||!readSpan(c.sceneName,c.sceneNameBytes,1))return Status::InvalidInput;
        if(const auto* mesh=c.collision.geometry.mesh){
            if(!aligned48N(mesh)||!readSpan(mesh,1,sizeof(*mesh)))return Status::InvalidInput;
            if(!readVector(mesh->nodes)||!readVector(mesh->faces)||!readVector(mesh->vertices)||
               !readVector(mesh->vertex_normals))return Status::InvalidInput;
        }
    }
    SpringQueryResult48N result{};
    result.state=initialState;
    if(query.resetBefore)resetSpringRecords48N(result.state);
    if(recordCount)std::memcpy(workspace.records,initialRecords,recordCount*sizeof(*initialRecords));
    result.candidateCount=count;
    result.currentCandidateIdentity=query.initialQueryCurrentIdentity;
    result.queryFlagsF8=static_cast<std::uint8_t>((query.filter.flagsF8|0x10u)&~0x20u);
    auto currentCategory=query.initialCurrentCategory;
    for(std::size_t i=0;i<count;++i){
        const auto& c=candidates[i].collision;
        workspace.effects[i]={c.filter.identity,c.filter.flagsF8,c.copied40,c.copied50};
        auto& effect=workspace.effects[i];
        if(c.typeTag!=query.collisionTypeTag){
            if(c.typeTag==query.groupTypeTag&&!(result.queryFlagsF8&1u))return Status::UnsupportedScope;
            continue;
        }
        result.currentCandidateIdentity=c.filter.identity;
        effect.flagsF8=static_cast<std::uint8_t>(effect.flagsF8|0x10u);
        bool overlap=false;
        if(!finite(c.sphere)||!spheres(query.sphere,c.sphere,overlap))return Status::InvalidInput;
        if(!overlap||!filter(query.filter,c.filter))continue;
        if(c.geometry.id!=c.filter.identity)return Status::InvalidInput;
        CollisionResult48I hit{};
        std::size_t traceUsed=0;
        const CollisionWorkspace48M collision{workspace.treeStack,workspace.treeStackCapacity,nullptr,0};
        const auto status=queryOrderedCollisionBounded48M(query.segment,&c.geometry,1,collision,hit,traceUsed);
        if(status==CollisionQueryStatus48I::InsufficientCapacity)return Status::InsufficientCapacity;
        if(status==CollisionQueryStatus48I::InvalidInput)return Status::InvalidInput;
        if(status==CollisionQueryStatus48I::UnsupportedInterpolationBoundary)return Status::UnsupportedInterpolationBoundary;
        if(status!=CollisionQueryStatus48I::Hit)continue;
        ++result.geometryHits;
        result.queryFlagsF8=static_cast<std::uint8_t>(result.queryFlagsF8|0x20u);
        if(!result.hasGeometryHit||hit.selected.fraction<=result.collision.selected.fraction)
            result.collision.selected=hit.selected;
        if(!result.hasGeometryHit||hit.nearestPair.fraction<result.collision.nearestPair.fraction){
            result.collision.nearestPair=hit.nearestPair;
            // 10882C copies complete LOCAL endpoints. 121590 transforms only
            // fresh normal/point lanes; whole-query original proof establishes
            // vf9.W=1 here, then normal.W=2 and surface.W=1 for mode1.
            std::memcpy(result.pair.first.v,query.segment.first.v,3*sizeof(float));
            std::memcpy(result.pair.second.v,query.segment.second.v,3*sizeof(float));
            std::memcpy(&result.pair.first.v[3],&query.firstW,sizeof(float));
            std::memcpy(&result.pair.second.v[3],&query.secondW,sizeof(float));
            std::memcpy(result.pair.normal.v,hit.nearestPair.normal.v,3*sizeof(float));
            std::memcpy(result.pair.surfacePoint.v,hit.nearestPair.point.v,3*sizeof(float));
            result.pair.normal.v[3]=2.f;result.pair.surfacePoint.v[3]=1.f;
        }
        result.hasGeometryHit=true;
        effect.copied40=query.copied40;effect.copied50=query.copied50;
        if(query.callbackEnabled){
            ++result.callbacks;
            const auto& candidate=candidates[i];
            const SpringStaticCallbackInput48N callback{&result.pair,1,c.filter.identity,c.filter.categoryA4,
                currentCategory,query.skipMask,query.dynamicMask,query.materialMask,2,query.queryScene,
                candidate.hasMaterialScene,candidate.sceneName,candidate.sceneNameBytes};
            SpringStaticCallbackResult48N appended{};
            const auto before=result.state.currentCandidate130;
            const auto called=applySpringStaticCallback48N(callback,workspace.records,recordCount,result.state,appended);
            if(called==SpringCallbackStatus48N::InvalidInput)return Status::InvalidInput;
            if(called==SpringCallbackStatus48N::InsufficientCapacity)return Status::InsufficientCapacity;
            if(called==SpringCallbackStatus48N::UnsupportedScope)return Status::UnsupportedScope;
            if(called==SpringCallbackStatus48N::Complete){
                result.appended+=appended.appended;result.saturated+=appended.saturated;
                if(!before||(currentCategory&query.dynamicMask))currentCategory=c.filter.categoryA4;
            }
        }
        if(c.hasResponseCallbackE8)return Status::UnsupportedScope;
    }
    if(recordCount)std::memcpy(destinationRecords,workspace.records,recordCount*sizeof(*destinationRecords));
    if(count)std::copy_n(workspace.effects,count,destinationEffects);
    output=result;return Status::Complete;
}
}
