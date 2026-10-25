#pragma once
#include <cstdint>
namespace rr::game {
// Native handoff diagnostics, NOT original unlocks or original rejection rules.
// Original decisions remain dispatchOriginalUiRider/Course50KG. A source-valid
// request can still lack a native mode, decoder, or authenticated source owner.
enum class OriginalCourseLoadAvailability50KG : std::uint8_t {
    ready, invalidSelection, unsupportedNativeMode, riderResourcesUnavailable,
    courseResourcesUnavailable, hostNotReady
};
template<class Selection> constexpr bool originalCourseSelectionEncoding50KG(const Selection& s) noexcept {
    return s.course<13&&s.planet<8&&s.rider<32&&s.laps<32&&
        (s.packed1125C&31u)==s.laps&&(s.packed1125C>>11)==s.rider;
}
// Mode0 writes at 1DED0C/1DED48 and 1DEDB4, preserving rider/human bits.
// This computes field values only; source lifecycle request ordering remains
// owned by the UI transition bridge, not simulated by this helper.
constexpr std::uint16_t originalCourseLaunchPackedMode050KG(std::uint16_t before,
    std::uint32_t subtype,std::uint32_t laps,std::uint32_t entrants) noexcept {
    const auto humans=(before>>5)&3u;
    const auto opponents=((subtype==2?entrants:8u)-humans)&15u;
    return static_cast<std::uint16_t>((before&~0x79fu)|(opponents<<7)|(laps&31u));
}
// Single-human native capability, separate from the authored UI transaction.
// Mode2 has its real fixed-three-lap eight-racer launch. Mode0 subtype2 uses
// its selected active extent. Subtype3's Course request initially packs eight,
// then its real1AD520 writer clears AI bits to leave one live human; both
// phases retain the same selected rider/course, never fabricated opponent rows.
template<class Selection> constexpr bool nativeSingleHumanCourseMode50KG(const Selection& s) noexcept {
    const auto humans=(s.packed1125C>>5)&3u;
    const auto opponents=(s.packed1125C>>7)&15u;
    if(humans!=1||s.laps<1||s.laps>25||s.entrants!=humans+opponents)return false;
    if(s.mode34==2)return s.subtype==1&&s.laps==3&&s.entrants==8;
    if(s.mode34!=0)return false;
    if(s.subtype==1)return s.entrants==8;
    if(s.subtype==2)return s.entrants>=1&&s.entrants<=8;
    if(s.subtype==3)return s.entrants==8||s.entrants==1;
    return false;
}
}
