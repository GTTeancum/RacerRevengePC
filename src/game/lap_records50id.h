#pragma once
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace rr::game {
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
    std::numeric_limits<float>::digits==24,"50ID records require binary32");
// Source-only, uncompiled recovery of 1861A0 and 186280. These views must
// project actual configuration-owned records. Zero/default/invented record
// rows are not a replacement for recovering their configuration owner.
struct LapRecordRow50ID {
    float lap102F0[3];
    float race1033C[3];
};
struct LapRecordTables50ID {
    const LapRecordRow50ID* rows;
    std::size_t rowCount;
};
struct LapRecordConfig50ID {
    // Original helpers read mode/lap count from CURRENT global->2DC, but the
    // record table and participant counts from the actual a0 configuration.
    std::uint32_t currentMode11258;
    std::uint8_t currentLapCount1125C;
    std::uint16_t targetPacked1125C;
};
enum class LapRecordQueryStatus50ID : std::uint8_t {
    Complete, Storage, UnsupportedNumeric
};
struct LapRecordQueryResult50ID {
    LapRecordQueryStatus50ID status;
    std::uint32_t originalV0;
    std::uint32_t sourcePC;
    bool completed() const noexcept {
        return status==LapRecordQueryStatus50ID::Complete;
    }
};
namespace lap_record_detail50ID {
inline bool ordinary(float x) noexcept {
    return std::isfinite(x)&&(x==0.0f||std::isnormal(x));
}
inline LapRecordQueryResult50ID result(LapRecordQueryStatus50ID status,
    std::uint32_t value,std::uint32_t pc) noexcept {return {status,value,pc};}
inline LapRecordQueryResult50ID compare(const LapRecordTables50ID& tables,
    std::uint32_t selection,float seconds,bool race) noexcept {
    using S=LapRecordQueryStatus50ID;
    const auto firstPC=race?0x18633cu:0x186204u;
    if(!tables.rows||selection>=tables.rowCount)
        return result(S::Storage,0,firstPC);
    const auto& row=tables.rows[selection];
    const float* values=race?row.race1033C:row.lap102F0;
    const std::uint32_t pcsLap[3]{0x186204u,0x186228u,0x18624cu};
    const std::uint32_t pcsRace[3]{0x18633cu,0x186360u,0x186384u};
    // Source is an ordered first-match search, not a minimum/sortedness test.
    // A matching earlier slot does not read or validate a later slot.
    for(std::uint32_t i=0;i<3;++i){
        const auto pc=race?pcsRace[i]:pcsLap[i];
        if(!ordinary(values[i]))return result(S::UnsupportedNumeric,0,pc);
        if(seconds<values[i])return result(S::Complete,i+1u,
            race?0x1863a4u:0x18626cu);
    }
    return result(S::Complete,0,race?0x1863a4u:0x18626cu);
}
}
// 50GG masks selection1125F to 15 before calling. The helper does not mask
// its own a1. Invalid/missing backing is an error, never a false query result.
inline LapRecordQueryResult50ID queryLapRecord50ID(
    const LapRecordConfig50ID& config,const LapRecordTables50ID& tables,
    std::uint32_t selection,float seconds) noexcept {
    using S=LapRecordQueryStatus50ID;
    using namespace lap_record_detail50ID;
    if(config.currentMode11258==2u)return result(S::Complete,0,0x18626cu);
    if(!ordinary(seconds))return result(S::UnsupportedNumeric,0,0x1861ccu);
    if(seconds<=0.0f)return result(S::Complete,0,0x18626cu);
    return compare(tables,selection,seconds,false);
}
inline LapRecordQueryResult50ID queryRaceRecord50ID(
    const LapRecordConfig50ID& config,const LapRecordTables50ID& tables,
    std::uint32_t selection,float seconds) noexcept {
    using S=LapRecordQueryStatus50ID;
    using namespace lap_record_detail50ID;
    if(config.currentMode11258!=1u||(config.currentLapCount1125C&31u)!=3u)
        return result(S::Complete,0,0x1863a4u);
    const auto humans=(config.targetPacked1125C>>5u)&3u;
    const auto opponents=(config.targetPacked1125C>>7u)&15u;
    if(humans+opponents!=8u)return result(S::Complete,0,0x1863a4u);
    if(!ordinary(seconds))return result(S::UnsupportedNumeric,0,0x186304u);
    if(seconds<=0.0f)return result(S::Complete,0,0x1863a4u);
    return compare(tables,selection,seconds,true);
}
// Both helpers are read-only. originalV0 is the actual 0/1/2/3 source result.
// Only completed() permits an acknowledgement to 50GG's pending query.
// Typed backing, its bounds, lifetimes, non-aliasing and serial ownership are
// caller contracts, as with the existing 50GG projections.
}
