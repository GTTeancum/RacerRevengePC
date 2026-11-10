#pragma once
// The common race outside RA (docs/common-results-20261108.md). The original
// has one race class (1A65C0) with one race+74 world on every course; the
// port keeps one consumed stage and one typed world owner per course. These
// helpers select THE consumed race of the running course and check it
// against its own course world, so the Results path is the same code on
// every course with a consumed race (TA, GA, SA, MA, GC). They read the
// race's public world backings only; nothing here creates or writes state.
#include "original_race_world_join50la.h"
#include "../../src/game/original_camera_start_effects.h"
#include <memory>

namespace native_common_race50PD {
using Race=rr::game::OriginalRaceConstructor50LA;
using Course=rr::game::OriginalCameraStartCourse;

// The non-RA consumed race of the running course (at most one is live).
template<class Context>
const std::shared_ptr<Race>& race(const Context& a) noexcept {
    if(a.consumedTaStage.race())return a.consumedTaStage.race();
    if(a.consumedGaStage.race())return a.consumedGaStage.race();
    if(a.consumedSaStage.race())return a.consumedSaStage.race();
    if(a.consumedMaStage.race())return a.consumedMaStage.race();
    return a.consumedGcStage.race();
}

// The course whose counted world this race holds (never RA).
inline Course course(const Race& r) noexcept {
    if(r.nativeTaWorldBacking())return Course::ta;
    if(r.nativeGaWorldBacking())return Course::ga;
    if(r.nativeSaWorldBacking())return Course::sa;
    if(r.nativeMaWorldBacking())return Course::ma;
    if(r.nativeGcWorldBacking())return Course::gc;
    return Course::unspecified;
}

// The race's own course world is the Context's retained world of that course.
// TA callers keep their own (older, stricter) TA expression; this covers the
// other courses through the race's identity-checked backings.
template<class Context>
bool worldCurrent(const Context& a,const Race& r) noexcept {
    if(const auto* w=r.nativeGaWorldBacking())return a.gaOriginalWorld&&w==a.gaOriginalWorld.get();
    if(const auto* w=r.nativeSaWorldBacking())return a.saOriginalWorld&&w==a.saOriginalWorld.get();
    if(const auto* w=r.nativeMaWorldBacking())return a.maOriginalWorld&&w==a.maOriginalWorld.get();
    if(const auto* w=r.nativeGcWorldBacking())return a.gcOriginalWorld&&w==a.gcOriginalWorld.get();
    if(const auto* w=r.nativeTaWorldBacking())return a.taOriginalWorld&&w==a.taOriginalWorld.get();
    return false;
}
} // namespace native_common_race50PD
