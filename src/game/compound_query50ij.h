#pragma once
#include "compound_collision50ij.h"
#include "collision_tree48i.h"

namespace rr::game {
// Single-pair121410 shape1 projection used by the live terrain query. The caller
// admits disjoint full compound/mesh spans before invoking this internal helper.
// One owner identity, parent coarse test, active child order, <= selected ties
// and strict pair ties. Callback/filter work remains outside the part loop.
// Scratch stack only: no allocation, independent outer candidates or trace.
inline CollisionQueryStatus48I queryCompoundPair50IJ(const LocalSegment48I& query,
    const CompoundCollisionView50IJ& compound,std::uint32_t ownerIdentity,
    const CollisionWorkspace48M& workspace,CollisionResult48I& output) noexcept {
    using Status=CollisionQueryStatus48I;using namespace compound50ij_detail;
    if(compound.count>maxCompoundMembers50IJ||(compound.count&&!compound.parts)||workspace.trace||workspace.traceCapacity)return Status::InvalidInput;
    Math m;Vector3_48C center{},half{};
    for(unsigned j=0;j<3;++j){
        center.v[j]=m.mul(.5f,m.add(query.second.v[j],query.first.v[j]));
        half.v[j]=m.mul(.5f,m.sub(query.second.v[j],query.first.v[j]));
    }
    bool coarse=false;if(!m.valid||!compoundCoarseOverlap50IJ(center,half,query.worldMatrix,compound,coarse))return Status::InvalidInput;
    if(!coarse)return Status::Miss;
    bool hit=false;CollisionResult48I result{};
    for(std::size_t i=0;i<compound.count;++i){const auto& part=compound.parts[i];if(!part.active)continue;
        const CollisionCandidate48I candidate{part.mesh,part.worldMatrix,ownerIdentity};
        CollisionResult48I found{};std::size_t traceUsed=0;
        const auto status=queryOrderedCollisionBounded48M(query,&candidate,1,workspace,found,traceUsed);
        if(status!=Status::Hit&&status!=Status::Miss)return status;
        if(status==Status::Miss)continue;
        if(!hit||found.selected.fraction<=result.selected.fraction)result.selected=found.selected;
        if(!hit||found.nearestPair.fraction<result.nearestPair.fraction)result.nearestPair=found.nearestPair;
        result.nodesVisited+=found.nodesVisited;result.nodesPruned+=found.nodesPruned;
        result.leavesTested+=found.leavesTested;result.leafHits+=found.leafHits;hit=true;
    }
    if(!hit)return Status::Miss;output=result;return Status::Hit;
}
}
