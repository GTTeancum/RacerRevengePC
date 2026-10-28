#pragma once
#include "lap_records50id.h"
#include "original_ui_hall_records50kg.h"

namespace rr::game {
// Source1863B0. Pure read-only query of the actual Hall table, not insertion or
// proof of earned CD0. This uses the same status/return convention as existing
//1861A0/186280; missing storage is never a source no-record answer.
inline LapRecordQueryResult50ID queryOriginalHallKnockoutRecord50KG(
    const LapRecordConfig50ID& config,const OriginalUiHallRecords50KG& records,
    std::uint32_t selection,std::uint32_t countCD0) noexcept {
    using S=LapRecordQueryStatus50ID;
    using lap_record_detail50ID::result;
    if(config.currentMode11258!=1u||(config.currentLapCount1125C&31u)!=3u)
        return result(S::Complete,0,0x1864a8u);
    const auto humans=(config.targetPacked1125C>>5u)&3u;
    const auto opponents=(config.targetPacked1125C>>7u)&15u;
    if(humans+opponents!=8u)return result(S::Complete,0,0x1864a8u);
    if(selection>=records.courses.size())return result(S::Storage,0,0x18644cu);
    const auto& values=records.courses[selection].knockouts10388;
    //186450/186470/186490 SLTU, first strictly lower stored slot. No sorted
    //row requirement, signed conversion, cap7 or nonzero-candidate gate.
    for(std::uint32_t i=0;i<values.size();++i)
        if(values[i]<countCD0)return result(S::Complete,i+1u,0x1864a8u);
    return result(S::Complete,0,0x1864a8u);
}
}
