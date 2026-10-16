#pragma once
#include "player_finish50ia.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Native HUD observation of original1A1EB0. This owns NO Player/AI object,
// controller, collision state or simulation fields. The caller supplies real
// manager-order state40 and scene340->288 observations for one publication.
struct OriginalDamageProximityRacer50LA {
    std::array<float,3> worldPosition{}; //28B180, with actual lazy cache refresh.
    std::uint32_t state40=0;
};
struct OriginalDamageProximityObservation50LA {
    // Original10A8..10C8 result order: near3, middle3, far3. Values are
    // manager indices, not nearest-racer distances or material/mesh handles.
    std::array<std::int32_t,9> nearby{{-1,-1,-1,-1,-1,-1,-1,-1,-1}};
};
// Complete finite scalar scan, preserving manager visitation and delay-slot
// writes. This is an explicit native presentation adaptation of a Player
// producer, NOT a claim that an AI allocation contains the Player extension.
// No existing output is changed on unsupported input; no runtime is scheduled.
inline bool observeOriginalDamageProximity1A1EB0(
    const OriginalDamageProximityRacer50LA* racers,std::size_t count,
    std::uint32_t viewManagerIndex,const std::array<float,3>& selfLocalForward,
    OriginalDamageProximityObservation50LA& output)noexcept {
    using namespace player_finish_detail50IA;
    if(!racers||count==0||count>8||viewManagerIndex>=count||std::fegetround()!=FE_TONEAREST)return false;
    for(float v:selfLocalForward)if(!std::isfinite(v))return false;
    const auto& self=racers[viewManagerIndex];
    for(float v:self.worldPosition)if(!std::isfinite(v))return false;
    float ownAngle=0;if(!directionAngle(selfLocalForward.data(),ownAngle)||!std::isfinite(ownAngle))return false;
    OriginalDamageProximityObservation50LA next;Math f;
    const float outer=word(0x48ef4200u),middle=word(0x481c4000u),inner=word(0x46afc800u);
    const float angle1=word(0x3f96cbe4u),angle2=word(0x401d1463u);
    const float tau=word(0x40c90fdbu),negativePi=word(0xc0490fdbu);
    for(std::size_t slot=0;slot<count;++slot){
        if(slot==viewManagerIndex||racers[slot].state40!=0)continue;
        const auto& other=racers[slot];for(float v:other.worldPosition)if(!std::isfinite(v))return false;
        const float dx=f.sub(self.worldPosition[0],other.worldPosition[0]);
        const float dy=f.sub(self.worldPosition[1],other.worldPosition[1]);
        const float dz=f.sub(self.worldPosition[2],other.worldPosition[2]);
        const float xx=f.mul(dx,dx),yy=f.mul(dy,dy),zz=f.mul(dz,dz);
        const float distanceSquared=f.add(f.add(xx,yy),zz);
        if(!f.valid)return false;
        if(!(distanceSquared<outer))continue; //1A1FD4/1A1FDC.
        float direction[3];for(unsigned axis=0;axis<3;++axis)
            direction[axis]=f.sub(other.worldPosition[axis],self.worldPosition[axis]);
        float otherAngle=0;if(!directionAngle(direction,otherAngle)||!std::isfinite(otherAngle))return false;
        //1A2024..2090: only positive own/negative candidate wraps candidate;
        //then relative angles below -pi receive one +2pi. No generic atan2.
        if(ownAngle>0.f&&otherAngle<0.f)otherAngle=f.add(otherAngle,tau);
        float relative=f.sub(ownAngle,otherAngle);
        if(relative<negativePi)relative=f.add(relative,tau);
        if(!f.valid)return false;
        std::size_t sector=0;
        if(relative>angle1&&relative<angle2)sector=2;
        else if(relative>=angle2||relative<=-angle2)sector=1;
        else if(relative< -angle1&&relative> -angle2)sector=0;
        else continue;
        const auto oldOuter=next.nearby[sector+6];
        next.nearby[sector+6]=static_cast<std::int32_t>(slot); //branch delay store.
        if(!(distanceSquared<middle))continue;
        const auto oldMiddle=next.nearby[sector+3];
        next.nearby[sector+3]=static_cast<std::int32_t>(slot); //branch delay store.
        if(distanceSquared<inner){
            next.nearby[sector]=static_cast<std::int32_t>(slot); //BNE delay store.
            if(oldMiddle==-1)next.nearby[sector+3]=-1;
        }
        if(oldOuter==-1)next.nearby[sector+6]=-1;
    }
    output=next;return true;
}
}
