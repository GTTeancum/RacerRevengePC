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
    // Tournament (mode34 2) race-return facts; all zero for the mode5 view.
    // Read live from the same profile on every snapshot, never cached.
    std::uint32_t order187B10=0;        // 1E4764 opening slot = order+1; 1E4000 confirm
    std::uint32_t boundary102CC=0;      // 1E3FF4
    std::uint8_t finalMedal1938=0;      // 1E4094 M[100C0][100C0][order]
    bool unlockPending187E30=false;     // 1E416C
    std::uint8_t raceCourse1125F=0;     // 1E2038 low nibble of 1125F
    // 1E2050/1E28A0/1E30E8 first-human ranks 11260/64/68 and 1E2068/1E28B8/
    // 1E3100 second-human ranks 1126C/70/74 (lap, race, knockouts).
    std::array<std::uint32_t,3> ranks11260{},ranks1126C{};
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
