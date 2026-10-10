#pragma once
#include "live_zone_membership50en.h"
#include "terrain_query_workspace48m.h"
#include "vegetation_raw_dispatch50jl.h"
#include <algorithm>
#include <cstring>

// Development-client live348 adapter. Source212090/212094 binds its limited
// list to vehicle488/48C, the SECOND source219AD4 collector output. This is
// separate from the spring detector's merged vehicle384/388 list.
namespace live_terrain_probe50fr {
using namespace rr::game;
struct Owner {
    Type2Ray49X pair{};
    Type2ResourceView49X resource{};
    bool initialized=false;
};
// Cold only. Keep Owner at its final address: resource borrows owner.pair.
inline bool initialize(Owner& owner,const LocalSegment48I& segment) noexcept {
    if(owner.initialized)return false;
    for(unsigned i=0;i<3;++i){owner.pair.first.v[i]=segment.first.v[i];
        owner.pair.second.v[i]=segment.second.v[i];}
    owner.pair.first.v[3]=owner.pair.second.v[3]=1.f;
    for(unsigned i=0;i<4;++i)owner.resource.localMatrix.v[5*i]=1.f;
    owner.resource.rays=&owner.pair;owner.resource.rayCount=1;
    owner.resource.root=type2Leaf49X;owner.initialized=true;return true;
}
inline bool matches(const Owner& owner,const TerrainPrepareState48M& prepared) noexcept {
    return owner.initialized&&owner.resource.rays==&owner.pair&&
        owner.resource.rayCount==1&&!owner.resource.nodeCount&&
        owner.resource.root==type2Leaf49X&&
        !std::memcmp(owner.pair.first.v,prepared.segment.first.v,12)&&
        !std::memcmp(owner.pair.second.v,prepared.segment.second.v,12);
}
inline LiveZoneBounds50EN bounds(const Owner& owner,
    const TerrainPrepareState48M& prepared) noexcept {
    LiveZoneBounds50EN out{};out.bounds.shapeTag=2;
    out.bounds.shapeWorld=prepared.segment.worldMatrix;
    for(unsigned i=0;i<3;++i)out.bounds.cachedCenter.v[i]=prepared.sphere.center.v[i];
    //11F680's transformed center W is the source vector-constructor lane.
    out.bounds.cachedCenter.v[3]=1.f;out.bounds.cachedRadius=prepared.sphere.radius;
    out.type2Resource=&owner.resource;return out;
}
// Cold setup may allocate. Every selected slot must accommodate every owner's
// filter lists; live copy below explicitly checks capacity before resizing.
inline bool reserveSelected(TerrainCandidate48J* selected,std::size_t capacity,
    TerrainCandidate48J* const* targets,std::size_t targetCount) {
    if((capacity&&!selected)||(targetCount&&!targets))return false;
    std::size_t categories=0,identities[2]{};
    for(std::size_t i=0;i<targetCount;++i){if(!targets[i])return false;
        categories=std::max(categories,targets[i]->filter.categoryMasks.size());
        for(unsigned j=0;j<2;++j)identities[j]=std::max(identities[j],
            targets[i]->filter.identityLists[j].identities.size());}
    for(std::size_t i=0;i<capacity;++i){
        selected[i].filter.categoryMasks.reserve(categories);
        for(unsigned j=0;j<2;++j)
            selected[i].filter.identityLists[j].identities.reserve(identities[j]);}
    return true;
}
struct Context {
    ZoneLifecycle50CL& lifecycle;
    LiveZoneWorld50EN& world;
    LiveZoneMembership50EN& membership;
    LiveZoneWorkspace50EN& membershipWork;
    CollectionRef50BX expectedObject=0;
    // Targets and the caller-supplied indices use the same stable native owner
    // namespace as vehicle488/48C. Selected is scratch, never a candidate source.
    TerrainCandidate48J* const* targets=nullptr;std::size_t targetCount=0;
    TerrainCandidate48J* selected=nullptr;std::size_t selectedCapacity=0;
    TerrainQueryWorkspace48M queryWork{};
    TerrainCandidateEffects48J* effects=nullptr;std::size_t effectCapacity=0;
    // Optional genuine raw full-table122660 branch. No recollection or flattening.
    const Type2StaticCandidate49X* rawCandidates=nullptr;std::size_t rawCandidateCount=0;
    const VegetationExpansion50JL* rawVegetation=nullptr;
};
enum class Status { Complete,InvalidInput,OwnerMismatch,InsufficientCapacity,
    PrefixRejected,QueryRejected };
struct Result {
    Status status=Status::InvalidInput;
    LiveZoneResult50EN prefix{};
    TerrainQueryStatus48M queryStatus=TerrainQueryStatus48M::Complete;
    bool prefixCommitted=false;
    std::size_t selectedCount=0;
    std::uint64_t membershipRevision=0;
};
namespace detail {
inline bool copyCandidate(const TerrainCandidate48J& source,
    TerrainCandidate48J& dest) noexcept {
    const auto& f=source.filter;auto& d=dest.filter;
    if(d.categoryMasks.capacity()<f.categoryMasks.size())return false;
    for(unsigned i=0;i<2;++i)if(d.identityLists[i].identities.capacity()<
        f.identityLists[i].identities.size())return false;
    dest.geometry=source.geometry;dest.sphere=source.sphere;
    dest.shapeType=source.shapeType;dest.compound=source.compound;
    dest.ownerIdentity9C=source.ownerIdentity9C;dest.typeTag=source.typeTag;
    dest.hasResponseCallbackE8=source.hasResponseCallbackE8;
    dest.copied40=source.copied40;dest.copied50=source.copied50;
    d.identity=f.identity;d.categoryA4=f.categoryA4;d.acceptMaskB8=f.acceptMaskB8;
    d.categoryModeD4=f.categoryModeD4;d.flagsF8=f.flagsF8;d.flags31=f.flags31;
    d.categoryMasks.resize(f.categoryMasks.size());
    std::copy(f.categoryMasks.begin(),f.categoryMasks.end(),d.categoryMasks.begin());
    for(unsigned i=0;i<2;++i){d.identityLists[i].mode=f.identityLists[i].mode;
        auto& list=d.identityLists[i].identities;const auto& sourceList=f.identityLists[i].identities;
        list.resize(sourceList.size());std::copy(sourceList.begin(),sourceList.end(),list.begin());}
    return true;
}
}
// Requires disjoint, stable caller-owned membership, target, selected-copy and
// query scratch spans, as do the underlying bounded APIs. The exact ordered
// indices/count represent vehicle488/48C, including a valid empty list. They
// must not be replaced with spring merged384/388 or recollected after prefix.
// Successful11FFA4 membership changes remain committed if122660 later rejects.
inline Result query(Owner& owner,const TerrainPrepareState48M& prepared,
    TerrainQuery48J& metadata,Context& c,const std::size_t* indices,
    std::size_t count,TerrainQueryResult48M& output) noexcept {
    Result r{};r.selectedCount=count;
    if(!matches(owner,prepared)||!c.membership.attached||
       c.membership.object!=c.expectedObject||!c.expectedObject||
       c.expectedObject>c.lifecycle.objectCount||
       c.membership.lastType2Resource!=&owner.resource||
       c.membership.lastShapeTag!=2){r.status=Status::OwnerMismatch;return r;}
    if((count&&(!indices||(!c.rawVegetation&&(!c.targets||!c.selected))))||!(metadata.filter.flagsF8&2u))return r;
    if(c.rawVegetation&&!c.rawCandidates)return r;
    if((!c.rawVegetation&&count>c.selectedCapacity)||count>c.effectCapacity||
       count>c.queryWork.effectCapacity){r.status=Status::InsufficientCapacity;return r;}
    for(std::size_t i=0;i<count;++i){
        if(c.rawVegetation){if(indices[i]>=c.rawCandidateCount)return r;continue;}
        if(indices[i]>=c.targetCount||!c.targets[indices[i]])return r;
        const auto& f=c.targets[indices[i]]->filter;const auto& d=c.selected[i].filter;
        if(d.categoryMasks.capacity()<f.categoryMasks.size()||
           d.identityLists[0].identities.capacity()<f.identityLists[0].identities.size()||
           d.identityLists[1].identities.capacity()<f.identityLists[1].identities.size()){
            r.status=Status::InsufficientCapacity;return r;}}
    const auto preparedBounds=bounds(owner,prepared);
    r.prefix=updateLiveZoneMembership50EN(c.lifecycle,c.world,c.membership,
        preparedBounds,c.membershipWork);
    if(r.prefix.status!=LiveZoneStatus50EN::Complete){r.status=Status::PrefixRejected;return r;}
    r.prefixCommitted=true;r.membershipRevision=c.membership.revision;
    if(!c.rawVegetation)for(std::size_t i=0;i<count;++i)
        if(!detail::copyCandidate(*c.targets[indices[i]],c.selected[i])){
            r.status=Status::InsufficientCapacity;return r;}
    const TerrainQueryView48M view{prepared.segment,prepared.sphere,metadata.filter,
        metadata.callbackMasks,prepared.terrain,metadata.copied40,metadata.copied50,
        metadata.collisionTypeTag,metadata.groupTypeTag,metadata.terrainCallbackEnabled,metadata.normalMode54,metadata.normalMode54Bound};
    TerrainQueryResult48M result{};
    r.queryStatus=c.rawVegetation?queryRawTerrainViewBounded50JL(view,c.rawCandidates,c.rawCandidateCount,indices,count,
        c.queryWork,c.effects,c.effectCapacity,*c.rawVegetation,result):
        queryTerrainViewBounded48M(view,c.selected,count,c.queryWork,c.effects,c.effectCapacity,result);
    if(r.queryStatus!=TerrainQueryStatus48M::Complete){r.status=Status::QueryRejected;return r;}
    // Preserve repeated entries and original effect order. No filter ownership
    // or vector copy is changed by this commit.
    // Raw mode leaves the effective-identity effects for the full-world owner
    // to commit with destinationShapes. It must call commitRaw immediately
    // after Complete, before another query. The old static adapter is unchanged.
    if(!c.rawVegetation)for(std::size_t i=0;i<count;++i){auto& target=*c.targets[indices[i]];
        const auto& effect=c.effects[i];target.filter.flagsF8=effect.flagsF8;
        target.copied40=effect.copied40;target.copied50=effect.copied50;}
    metadata.filter.flagsF8=result.queryFlagsF8;
    output=result;r.status=Status::Complete;return r;
}
}
