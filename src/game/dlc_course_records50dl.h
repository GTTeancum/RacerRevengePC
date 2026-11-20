#pragma once
// Add-on course records (DLC, helper 148r). PORT EXTENSION, NOT ORIGINAL BEHAVIOUR.
// The ranks of the last 1AC440 insertion into an add-on course's own Hall rows
// (DLC side file, src\assets\dlc_progress50dl.h). The original keeps the
// first-human ranks in profile 11260/64/68 and the race-return Hall recolours
// those rows (1E2010/1E2860/1E30A8); an add-on course's ranks are kept here
// instead, so the stock profile is never written for an add-on course.
// Owner: the rider profile (written at the 1AC440 moment, cleared at the race
// start 1A66CC as 11260.. are); read by the Hall session. Process memory only.
// Narrow on purpose (standard headers only); implemented in dlc_course_entries50dl.cpp.
#include <cstdint>

namespace rr::game::dlc50DL {
struct AddonCourseRecordRanks {
    unsigned number=0;                        // add-on course number (32..), 0 = none
    std::uint32_t lap=0,race=0,knockouts=0;   // 1-based rank, 0 = no entry
};
// Bounds: number must be an admitted add-on course, ranks 0..3; otherwise refused.
bool noteAddonCourseRecord(const AddonCourseRecordRanks&) noexcept;
AddonCourseRecordRanks lastAddonCourseRecord() noexcept;
void clearAddonCourseRecord() noexcept;
} // namespace rr::game::dlc50DL
