#pragma once
#include "original_ui_course_selection50kg.h"

namespace rr::game {
// Pure proposal for source1DA558..1DA618. No owner-ready/generation token,
// runtime admission, profile mutation, or implication of an earned result.
// Caller must obtain progress from the SAME actual profile.career[100C0].
struct OriginalUiCourseMode2EntryInput50KG {
    std::uint32_t mode34=0,career100C0=0xffffffffu;
    std::uint8_t progress219C=0;
};
// Only source-written lanes are represented. Mode2 entry does NOT write
// laps14EC or entrants14F0. Their later launch semantics are separate owners.
struct OriginalUiCourseMode2EntryPlan50KG {
    std::uint16_t courseSlot14E4=0,subtypeSlot14F4=0;
    std::uint32_t tier14E8=0;
    OriginalUiCourseControl50KG selectedControl{};
    OriginalUiProfileChange50KG planetProfileChange{};
    std::uint32_t sourcePC=0x1da558u;
};
// Requires actual loaded Course metadata; an unbound data owner is rejected.
// All256 progress bytes retain original mask/add/cap behavior. career<27 is
// the represented profile-bank bound, not proof of selection or unlock.
// Failure leaves out unchanged. The bounded fresh Course Session and private
// profile commit consume this pure plan; it cannot authorize a transaction.
OriginalUiCourseSelectionStatus50KG planOriginalUiCourseMode2Entry50KG(
    const OriginalUiCourseSelectionData50KG&,
    const OriginalUiCourseMode2EntryInput50KG&,
    OriginalUiCourseMode2EntryPlan50KG&) noexcept;
}
