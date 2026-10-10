#pragma once
#include "lap_records50id.h"
#include "lap_completion50gg.h"
#include <cstring>

namespace rr::game {
// A new native profile's actual TA numeric record projection. This neither
// imports nor overwrites a saved user profile. Other track rows/names are not
// represented. The caller explicitly chooses the race configuration.
struct LapConfigTa50ID {
    std::uint32_t mode11258=2;
    std::uint16_t packed1125C=0;
    std::uint8_t selection1125F=0;
    LapRecordRow50ID row{};
    bool initialized=false;
};
inline bool initializeFreshTaLapConfig50ID(LapConfigTa50ID& out,
    std::uint32_t mode,std::uint8_t lapCount,std::uint8_t humans,
    std::uint8_t opponents) noexcept {
    if(out.initialized||(mode!=1u&&mode!=2u)||!lapCount||lapCount>31u||
       humans>3u||opponents>15u||humans+opponents==0u)return false;
    LapConfigTa50ID next;
    next.mode11258=mode;
    next.packed1125C=static_cast<std::uint16_t>(lapCount|(humans<<5u)|(opponents<<7u));
    // Constructor17E118..138 initializes selected low nibble to0. Its first
    // track row11278 receives "ta\0" from3A89E0 at17E9D8..17E9F8.
    next.selection1125F=0;
    //Original language/default initialization17DF3C ->188870 ->1801D0.
    //1801D0's row0 direct-slot calls185910 at18030C/1803CC/18048C;
    //185BC0 at18054C/18060C/1806CC. Exact original f12 words, no decimal
    // re-rounding, fabricated sentinel, or guessed record-query answer.
    constexpr std::uint32_t lapWords[3]{0x423f51ecu,0x4250a3d7u,0x426e0a3du};
    constexpr std::uint32_t raceWords[3]{0x432b2148u,0x43328a3du,0x4336a666u};
    for(unsigned i=0;i<3;++i){
        std::memcpy(&next.row.lap102F0[i],&lapWords[i],4);
        std::memcpy(&next.row.race1033C[i],&raceWords[i],4);
    }
    next.initialized=true;out=next;return true;
}
inline LapCompletionConfig50GG completionConfig50ID(const LapConfigTa50ID& config) noexcept {
    return {config.mode11258,static_cast<std::uint8_t>(config.packed1125C),
        config.selection1125F};
}
inline LapRecordConfig50ID recordConfig50ID(const LapConfigTa50ID& config) noexcept {
    return {config.mode11258,static_cast<std::uint8_t>(config.packed1125C),config.packed1125C};
}
inline LapRecordTables50ID recordTables50ID(const LapConfigTa50ID& config) noexcept {
    return {config.initialized?&config.row:nullptr,config.initialized?1u:0u};
}
}
