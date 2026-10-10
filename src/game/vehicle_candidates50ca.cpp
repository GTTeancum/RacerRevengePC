#include "vehicle_candidates50ca.h"
#include <limits>
namespace rr::game { namespace {
struct Span {std::uintptr_t begin=0,end=0;};
template<class T> bool span(const T* p,std::size_t n,Span& r) noexcept {
    r={};if(!n)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto b=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-b)return false;
    r={b,b+bytes};return true;
}
bool overlap(Span a,Span b) noexcept {return a.begin<b.end&&b.begin<a.end;}
struct Guard {
    Span writes[32]{};std::size_t used=0;
    template<class T>bool write(T* p,std::size_t n) noexcept {
        Span r;if(used>=32||!span(p,n,r))return false;
        for(std::size_t i=0;i<used;++i)if(overlap(r,writes[i]))return false;
        writes[used++]=r;return true;
    }
    template<class T>bool read(const T* p,std::size_t n) const noexcept {
        Span r;if(!span(p,n,r))return false;
        for(std::size_t i=0;i<used;++i)if(overlap(r,writes[i]))return false;
        return true;
    }
};
bool writes(Guard& g,const CandidateCollectionBuffers50BX& b) noexcept {
    return g.write(b.objects,b.objectCapacity)&&g.write(b.trees,b.treeCapacity)&&
        g.write(b.pools,b.poolCapacity)&&g.write(b.poolBlocks,b.poolBlockCapacity)&&
        g.write(b.poolEntries,b.poolEntryCapacity)&&g.write(b.identities,b.identityCapacity)&&
        g.write(b.shape0States,b.shape0Capacity);
}

CandidateCollectionState50BX state(const CandidateCollectionBuffers50BX& b) noexcept {
    return {b.objects,b.objectCapacity,b.trees,b.treeCapacity,b.pools,b.poolCapacity,
        b.poolBlocks,b.poolBlockCapacity,b.poolEntries,b.poolEntryCapacity,
        b.shape0States,b.shape0Capacity};
}
}
VehicleCandidateResult50CA collectVehicleCandidates50CA(
    const CandidateCollectionInput50BX& in,const CandidateCollectionState50BX& initial,
    const VehicleCandidateWorkspace50CA& stages) noexcept {
    VehicleCandidateResult50CA result{};
    const auto& work=stages.collector;
    Guard guard;
    if(!writes(guard,work.buffers)||!writes(guard,stages.first)||!writes(guard,stages.second)||
       !guard.write(work.stack,work.stackCapacity)||
       !guard.write(stages.mergeScratch.identities,stages.mergeScratch.capacity)||
       !guard.write(stages.merged.identities,stages.merged.capacity)||
       !guard.read(&stages,1))return result;
    if(!guard.read(&in,1)||!guard.read(&initial,1)||!guard.read(&work,1)||
       !guard.read(in.objects,in.objectCount)||!guard.read(in.groups,in.groupCount)||!guard.read(in.nodes,in.nodeCount)||
       !guard.read(in.itemBlocks,in.itemBlockCount)||!guard.read(in.trees,in.treeCount)||!guard.read(in.zones,in.zoneCount)||
       !guard.read(in.zoneLinks,in.zoneLinkCount)||!guard.read(in.pools,in.poolCount)||!guard.read(in.poolBlocks,in.poolBlockCount)||
       !guard.read(initial.objects,initial.objectCapacity)||!guard.read(initial.trees,initial.treeCapacity)||
       !guard.read(initial.pools,initial.poolCapacity)||!guard.read(initial.poolBlocks,initial.poolBlockCapacity)||
       !guard.read(initial.poolEntries,initial.poolEntryCapacity)||
       !guard.read(in.bounds.endpoints,in.bounds.endpointCount)||!guard.read(in.bounds.endpointScenes,in.bounds.endpointSceneCount)||
       !guard.read(in.bounds.scenes,in.bounds.sceneCount)||!guard.read(in.shape0Sources,in.shape0Count)||
       !guard.read(initial.shape0States,initial.shape0Capacity)||
       !guard.read(work.identityOrder,work.identityOrderCount)||!guard.read(work.blockOrder,work.blockOrderCount))return result;
    if(!guard.read(in.type2Resource,in.type2Resource?1:0))return result;
    if(in.type2Resource&&(!guard.read(in.type2Resource->nodes,in.type2Resource->nodeCount)||!guard.read(in.type2Resource->rays,in.type2Resource->rayCount)))return result;
    for(std::size_t k=0;k<in.treeCount;++k)if(in.trees[k].method==CollectionTree50BX::Method::SearchList){
        const auto& list=in.trees[k].searchList;
        if(!guard.read(list.blocks,list.blockCapacity))return result;
        for(std::size_t j=0;j<list.blockCapacity;++j)if(!guard.read(list.blocks[j].words,list.blocks[j].capacity))return result;
    }

    for(std::size_t j=0;j<in.objectCount;++j){
        const auto& f=in.objects[j].filter;
        if(!guard.read(f.identities[0].values,f.identities[0].capacity)||
           !guard.read(f.identities[1].values,f.identities[1].capacity)||
           !guard.read(f.categories.values,f.categories.capacity))return result;
    }
    for(std::size_t j=0;j<in.groupCount;++j)
        if(!guard.read(in.groups[j].members,in.groups[j].memberCapacity))return result;
    for(std::size_t j=0;j<in.shape0Count;++j)
        if(!guard.read(in.shape0Sources[j].geometry,in.shape0Sources[j].geometry?1:0))return result;
    auto input=in;
    input.methodMask=2;input.skipSphere=true;input.ignoreGroups=true;input.nominalCapacity=64;
    result.phase=VehicleCandidateResult50CA::Phase::First;
    result.collectionStatus=collectCandidatesBounded50BX(input,initial,work,stages.first,result.first);
    if(result.collectionStatus!=CandidateCollectionStatus50BX::Complete)return result;
    input.methodMask=1;input.skipSphere=false;input.ignoreGroups=false;
    input.initialCurrentIdentity=result.first.currentIdentity;
    input.initialActiveTree=result.first.activeTree;
    const auto afterFirst=state(stages.first);
    result.phase=VehicleCandidateResult50CA::Phase::Second;
    result.collectionStatus=collectCandidatesBounded50BX(input,afterFirst,work,stages.second,result.second);
    if(result.collectionStatus!=CandidateCollectionStatus50BX::Complete)return result;
    const CandidateMergeInput48S merge{
        {result.first.count,stages.first.identities,stages.first.identityCapacity},
        {result.second.count,stages.second.identities,stages.second.identityCapacity}};
    CandidateMergeResult48S merged{};
    result.phase=VehicleCandidateResult50CA::Phase::Merge;
    result.mergeStatus=mergeCandidates48S(merge,stages.mergeScratch,stages.merged,merged);
    if(result.mergeStatus!=CandidateMergeStatus48S::Complete)return result;
    result.count=merged.count;result.phase=VehicleCandidateResult50CA::Phase::Complete;
    return result;
}
}
