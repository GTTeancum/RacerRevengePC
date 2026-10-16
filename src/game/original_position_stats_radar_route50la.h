#pragma once
#include "track_navigation50eu.h"
#include <cstddef>
#include <cstdint>

namespace rr::game::position_stats_detail50LA {
struct RadarRouteProgress {
    std::size_t visitedNodes=0,completedElements=0,pointsInCurrent=0;
    bool elementLimit=false;
    std::uint32_t sourcePC=0,node=0;
};
//Original2A26E0 selection, including the same-branch fast path and the
//descending search which excludes node0. The node0 query scans Next0 only.
inline bool radarPredecessor2A26E0(const TrackNavigation50EU& map,
    std::uint32_t node,std::uint32_t& previous)noexcept {
    if(node>=map.track.count)return false;
    if(node&&map.nodes[node].branch68==map.nodes[node-1].branch68){previous=node-1;return true;}
    if(!node){
        for(std::uint32_t i=0;i<map.track.count;++i)
            if(map.track.nodes[i].nextCount&&map.track.nodes[i].next[0]==0){previous=i;return true;}
        return false;
    }
    for(std::uint32_t i=node;i>0;--i)
        for(std::uint32_t n=0;n<map.track.nodes[i].nextCount;++n)
            if(map.track.nodes[i].next[n]==node){previous=i;return true;}
    return false;
}
//192568..1927E0, before the separate racer-marker loop. Callers supply the
//actual authored map. Emit writes an existing element/point and returns the
//192C70 alpha; Finish publishes its doubled type5 count/dirty state. Neither
//callback may reorder nodes, infer points, reenter, throw or submit guest work.
//Failure retains already emitted writes and reports the exact pending node.
//Completed elements are CPU geometry only; original per-strip GPU submission
//is deliberately a later native renderer responsibility.
template<class Emit,class Finish>
bool traverseRadar1924C0(const TrackNavigation50EU& map,Emit&& emit,Finish&& finish,
    RadarRouteProgress& progress)noexcept {
    progress={};
    const auto fail=[&](std::uint32_t pc,std::uint32_t node)noexcept{
        progress.sourcePC=pc;progress.node=node;return false;
    };
    if(!map.gatesPrepared||!map.track.count||map.track.count>maxUndertrackNodes50ET)
        return fail(0x192734,0);
    for(std::uint32_t i=0;i<map.track.count;++i){
        const auto& n=map.track.nodes[i];
        if(!n.nextCount||n.nextCount>maxUndertrackNext50ET)return fail(0x2a26c0,i);
        for(std::uint32_t j=0;j<n.nextCount;++j)
            if(n.next[j]>=map.track.count)return fail(0x2a26c0,i);
    }
    std::int32_t previous=-1;
    const auto append=[&](std::uint32_t node,std::uint32_t& alpha)noexcept{
        progress.node=node;
        if(progress.completedElements>=16||progress.pointsInCurrent>=118)
            return fail(0x192c70,node);
        const auto& p=map.track.nodes[node].position;
        if(!emit(progress.completedElements,progress.pointsInCurrent,p[0],p[2],alpha))
            return fail(0x192c70,node);
        return true;
    };
    const auto close=[&]()noexcept{
        if(previous<0)return fail(0x2a26c0,0);
        const auto node=map.track.nodes[static_cast<std::size_t>(previous)].next[0];
        std::uint32_t alpha=0;
        if(!append(node,alpha))return false;
        ++progress.pointsInCurrent;
        if(!finish(progress.completedElements,progress.pointsInCurrent))return fail(0x192624,node);
        ++progress.completedElements;return true;
    };
    for(std::uint32_t node=0;node<map.track.count;++node){
        progress.visitedNodes=node+1;
        //2A2600 extracts node79 bit5 (SecretBranch), not Pretty or Ugly.
        if(map.nodes[node].flags79&32u)continue;
        std::uint32_t alpha=0;
        if(!append(node,alpha))return false;
        if(!alpha)continue; //Same uncounted point is overwritten by the next candidate.
        std::uint32_t predecessor=0;
        if(!radarPredecessor2A26E0(map,node,predecessor))return fail(0x2a26e0,node);
        if(previous!=static_cast<std::int32_t>(predecessor)){
            if(progress.pointsInCurrent){
                if(!close())return false;
                if(progress.completedElements==16){progress.elementLimit=true;break;}
            }
            progress.pointsInCurrent=0;
            if(!append(predecessor,alpha))return false;
            ++progress.pointsInCurrent;
            if(!append(node,alpha))return false;
        }
        ++progress.pointsInCurrent;previous=static_cast<std::int32_t>(node);
    }
    if(progress.pointsInCurrent&&progress.completedElements<16){if(!close())return false;}
    progress.pointsInCurrent=0;return true;
}
}
