#pragma once
#include "../assets/original_ui_localization50kg.h"
#include <array>
#include <cstdint>

namespace rr::game {
// Original profile102F0 + course*E4. Native typed storage preserves the source
// lanes without claiming a packed guest object or persisted/earned records.
struct OriginalUiHallCourseRecords50KG {
    std::array<float,3> lap102F0{},race1033C{};
    std::array<std::uint32_t,3> knockouts10388{};
    std::array<std::array<char,21>,3> lapNames102FC{},raceNames10348{},knockoutNames10394{};
};
struct OriginalUiHallProfile50KG {
    bool ownerReady=false;
    std::uint32_t mode34=0,remembered6EC=0;
    std::uint8_t packed1125E=0,availability11018=0;
    bool bit0_7E4=false;
    std::array<std::uint8_t,13> courseOrder100C4{};
};
struct OriginalUiHallRecords50KG {
    std::array<OriginalUiHallCourseRecords50KG,13> courses{};
    const assets::OriginalUiMemberIdentity50KG* namesSource=nullptr;
    const assets::OriginalUiMemberIdentity50KG* textSource=nullptr;
};
struct OriginalUiHallRecordsDiagnostic50KG {
    enum class Error:std::uint8_t {none,localization,name,numeric} error=Error::none;
    std::uint32_t sourcePC=0,textID=0;
};
// Authenticated SLUS_202.68 initializer1801D0, invoked after actual localized
// text loading via17DE80 ->188870. Copies bounded original text bytes; output
// commits atomically. Does not import saves or synthesize post-race records.
bool initializeOriginalUiHallRecords50KG(const assets::OriginalUiEnglish50KG&,
    OriginalUiHallRecords50KG&,OriginalUiHallRecordsDiagnostic50KG&) noexcept;
// Source189000, ordinary finite binary32 inputs only. The separate multiply
// and add must not be contracted. Upper/nonpositive values use --:--.--.
bool originalUiHallTime18900050KG(float,std::array<char,9>&) noexcept;
}
