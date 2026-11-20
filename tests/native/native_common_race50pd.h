#pragma once
// The one common race (docs/common-results-20261108.md; common rewrite R1,
// PLAN B-C04). The original has one race class (1A65C0) with one race+74 World
// on every course; the port now holds exactly one consumed stage
// (original_consumed_stage50DU::Owner) and one World (common_race_world50CR).
// These helpers select THE consumed race and check it against the running
// Track. They read only; nothing here creates or writes state. No course is
// named: a caller that needs the course code for a file name reads
// track.code().
#include "original_race_world_join50la.h"
#include "original_consumed_stage50du.h"
#include <memory>

namespace native_common_race50PD {
using Race=rr::game::OriginalRaceConstructor50LA;

// The consumed race of the running course (one stage for every course).
template<class Context>
const std::shared_ptr<Race>& race(const Context& a) noexcept {
    return a.consumedStage.race();
}

// The race's one World holds the Context's running Track (the same object the
// stage began with), on every course.
template<class Context>
bool worldCurrent(const Context& a,const Race& r) noexcept {
    const auto* world=r.world74();
    return world&&world->track&&world->track.get()==&a.track&&
        a.consumedStage.track().get()==&a.track;
}
} // namespace native_common_race50PD
