#pragma once
#include <array>
#include <cstdint>
namespace rr::game {
// Native cold-destination capability domain, not source unlock/profile state.
// These IDs have actual original rig/COL/visual/PHY/audio owners in the staged
// eight-racer field (4/7/10/11) or authenticated supplemental POD02/12/15/19/20. The real
// UI unlock owner must independently admit selection, and cold loading must
// authenticate that selected pod. Never replace an unsupported rider with19.
inline constexpr std::array<std::uint32_t,9> nativeCourseRacers50KG{{2,4,7,10,11,12,15,19,20}};
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
        (course==10&&planet==1&&sourceSlot==11);
}

}
