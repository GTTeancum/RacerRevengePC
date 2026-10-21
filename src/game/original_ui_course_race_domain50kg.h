#pragma once
#include <array>
#include <cstdint>
namespace rr::game {
// Native cold-destination request capability, not a successful load or unlock.
// Supplemental racers and TB belong only to their complete test composition;
// the actual canonical loader must authenticate and construct every owner.
// These IDs have actual original rig/COL/visual/PHY/audio owners in the staged
// eight-racer field (1/3/4/7/8/9/10/11) or authenticated supplemental POD02/12/15/19/20/22. The real
// UI unlock owner must independently admit selection, and cold loading must
// authenticate that selected pod. Never replace an unsupported rider with19.
inline constexpr std::array<std::uint32_t,23> nativeCourseRacers50KG{{1,2,3,4,7,8,9,10,11,12,15,16,19,20,22,24,17,18,23,14,21,25,26}};
inline constexpr bool nativeCourseRacerOwned50KG(std::uint32_t racer60) noexcept {
    for(const auto owned:nativeCourseRacers50KG)if(owned==racer60)return true;
    return false;
}
// Original fresh unlocks remain TA, SA, MA, GA. GC is a separately admitted
// native destination at authentic later slot11; this capability never unlocks it.
inline constexpr bool nativeCourseTrackOwned50KG(std::uint32_t course,
    std::uint32_t planet,std::uint32_t sourceSlot) noexcept {
    return (course==0&&planet==0&&sourceSlot==1)||
        (course==11&&planet==4&&sourceSlot==2)||
        (course==4&&planet==5&&sourceSlot==3)||
        (course==8&&planet==1&&sourceSlot==4)||
        (course==7&&planet==2&&sourceSlot==5)||
        (course==5&&planet==5&&sourceSlot==7)||
        (course==12&&planet==4&&sourceSlot==9)||
        (course==10&&planet==1&&sourceSlot==11)||
        (course==2&&planet==0&&sourceSlot==13)||
        (course==3&&planet==2&&sourceSlot==12)||
        (course==1&&planet==0&&sourceSlot==6)||
        (course==9&&planet==1&&sourceSlot==8)||
        (course==6&&planet==5&&sourceSlot==10);
}

}
