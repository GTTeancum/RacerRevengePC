#pragma once
#include "../../src/game/track_navigation50eu.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>

// EXPLICIT HOST-ONLY INPUT ROUTING ADAPTER. Not original AI/navigation.
// Copy only into the opt-in HOST overlap stage,not the authentic hit producer.
// Canonical LostAI changes C80 only. This chooses a temporary predecessor for
// CourseDriverInput50GK.previousNode,never a replacement source C84 value.
namespace gc_host_route50IN {
namespace g=rr::game;
inline constexpr std::uint32_t originalAgcNodeCount=195;
inline bool lostAiTarget(std::uint32_t node)noexcept {
    switch(node){
        case 22:case 172:case 181:case 134:case 186:case 137:case 193:case 49:return true;
        default:return false;
    }
}
// CALLER AUTHORIZATION/LIFETIME CONTRACT:
// Call only after this opt-in HOST backend accepted LostAI for this racer AND
// current remains that exact recorded action target. Store that pending-event
// condition separately in HOST state;clear it once canonical C80 changes.
// Generic InvalidRoute or proximity alone does not authorize this fallback.
// The immutable route is the authenticated AGC.SPL195-node graph. actualPose
// is the actual current body/query pose. Do not supply an invented position.
//
// Policy: keep canonical previous if it has an authored edge to current.
// Otherwise choose the incoming authored segment with smallest clamped XZ
// point-to-segment distance from actualPose. Exact ties choose lower authored
// predecessor index. This is a deterministic HOST policy,not source AI logic.
// All195 graph nodes and all16 pose components are validated before selection.
// output must be a HOST temporary,never an alias of canonical C84/C80.
// Failure leaves output unchanged. Success writes only output. No graph,
// C80/C84,C88/C8C,body,velocity,lap,control packet or RNG effects occur here.
inline bool selectPrevious(const g::TrackNavigation50EU&route,
    std::uint32_t current,std::uint32_t previous,const g::BodyMatrix48G&actualPose,
    std::uint32_t&output)noexcept {
    if(!route.gatesPrepared||route.track.count!=originalAgcNodeCount||
       current>=originalAgcNodeCount||previous>=originalAgcNodeCount||
       !lostAiTarget(current)||std::fegetround()!=FE_TONEAREST)return false;
    for(const float value:actualPose.v)if(!std::isfinite(value))return false;
    for(std::uint32_t i=0;i<originalAgcNodeCount;++i){
        const auto&node=route.track.nodes[i];
        if(!node.nextCount||node.nextCount>g::maxUndertrackNext50ET)return false;
        for(const float value:node.position)if(!std::isfinite(value))return false;
        for(std::uint32_t j=0;j<node.nextCount;++j)
            if(node.next[j]>=originalAgcNodeCount)return false;
    }
    const auto&old=route.track.nodes[previous];
    if(previous!=current)for(std::uint32_t j=0;j<old.nextCount;++j)
        if(old.next[j]==current){output=previous;return true;}
    const auto&target=route.track.nodes[current];
    const double px=actualPose.v[12],pz=actualPose.v[14];
    const double bx=target.position[0],bz=target.position[2];
    double best=std::numeric_limits<double>::infinity();
    std::uint32_t chosen=originalAgcNodeCount;
    for(std::uint32_t i=0;i<originalAgcNodeCount;++i){
        const auto&node=route.track.nodes[i];bool incoming=false;
        for(std::uint32_t j=0;j<node.nextCount;++j)incoming|=node.next[j]==current;
        if(!incoming||i==current)continue;
        const double ax=node.position[0],az=node.position[2];
        const double dx=bx-ax,dz=bz-az;
        const double extent=dx*dx+dz*dz;
        if(!std::isfinite(extent)||extent<=0.0)return false;
        const double raw=((px-ax)*dx+(pz-az)*dz)/extent;
        if(!std::isfinite(raw))return false;
        const double t=raw<0.0?0.0:(raw>1.0?1.0:raw);
        const double ex=px-(ax+t*dx),ez=pz-(az+t*dz);
        const double distanceSquared=ex*ex+ez*ez;
        if(!std::isfinite(distanceSquared))return false;
        // Ascending original node order plus strict < keeps lower-index ties.
        if(distanceSquared<best){best=distanceSquared;chosen=i;}
    }
    if(chosen==originalAgcNodeCount)return false;
    output=chosen;return true;
}
}
