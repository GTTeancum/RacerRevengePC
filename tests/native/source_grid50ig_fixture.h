#pragma once
#include "track_navigation50eu.h"
#include "grid_center50ac.h"
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>

namespace source_grid50IG {
using namespace rr::game;
// Native projection of2A2880..2A2D74 for the authored node0 start and ranks0..7.
// The original first two rows use gate-direction offsets40/0/40; the final
// two-racer row uses20/20. Row distance is integer(rank/3)*100. The caller
// assigns racers to ranks explicitly; no claim is made about original ranking.
// Gate10/20 are produced by the real M spline loader, never world X/Z guesses.
inline bool prepare(const TrackNavigation50EU& map,std::uint32_t rank,
    GridCenterResult50AC& result) noexcept {
    if(rank>7||!map.gatesPrepared||map.track.count<2||
       map.track.count>maxUndertrackNodes50ET||std::fegetround()!=FE_TONEAREST)return false;
    std::uint32_t previous=0;
    for(;previous<map.track.count;++previous){const auto& node=map.track.nodes[previous];
        if(!node.nextCount||node.nextCount>maxUndertrackNext50ET)return false;
        if(node.next[0]==0)break;}
    if(previous==map.track.count||!map.track.nodes[0].nextCount)return false;
    const auto* current=map.track.nodes[0].position;
    GridCenterInput50AC center{};
    for(unsigned k=0;k<4;++k){center.current[k]=std::bit_cast<std::uint32_t>(current[k]);
        center.previous[k]=std::bit_cast<std::uint32_t>(map.track.nodes[previous].position[k]);}
    center.rank=1;center.nextNode=map.track.nodes[0].next[0];
    GridCenterResult50AC staged{};
    if(!prepareGridCenter50AC(center,staged))return false;
    const auto add=[](float a,float b){volatile float x=a+b;return x;};
    const auto sub=[](float a,float b){volatile float x=a-b;return x;};
    const auto mul=[](float a,float b){volatile float x=a*b;return x;};
    float lateral[2][3]{};
    const float* gates[]={map.nodes[0].gate10,map.nodes[0].gate20};
    //2A299C..2A2AD4 performs both scalar normalizations, even for center ranks.
    // Original zero-vector fallback443910 is deliberately not guessed here.
    for(unsigned side=0;side<2;++side){
        for(unsigned k=0;k<3;++k)lateral[side][k]=sub(gates[side][k],current[k]);
        const float square=add(add(mul(lateral[side][0],lateral[side][0]),
            mul(lateral[side][1],lateral[side][1])),mul(lateral[side][2],lateral[side][2]));
        if(!(square>0.f)||!std::isfinite(square)||(std::bit_cast<std::uint32_t>(square)&0x7f800000u)==0)return false;
        volatile float root=static_cast<float>(std::sqrt(static_cast<double>(square)));
        volatile float reciprocal=1.f/root;
        for(unsigned k=0;k<3;++k){lateral[side][k]=mul(lateral[side][k],reciprocal);
            if(!std::isfinite(lateral[side][k]))return false;}
    }
    const auto row=rank/3u,column=rank%3u;
    const bool lateralColumn=row==2u||column!=1u;
    const unsigned side=column==0u?0u:1u;
    const float across=row==2u?20.f:40.f;
    const float back=static_cast<float>(row*100u);
    for(unsigned k=0;k<3;++k){
        const float at=lateralColumn?add(current[k],mul(across,lateral[side][k])):current[k];
        const float value=sub(at,mul(back,std::bit_cast<float>(staged.heading[k])));
        if(!std::isfinite(value))return false;staged.position[k]=std::bit_cast<std::uint32_t>(value);
    }
    staged.position[3]=lateralColumn?0x3f800000u:center.current[3];
    result=staged;return true;
}
} // namespace source_grid50IG
