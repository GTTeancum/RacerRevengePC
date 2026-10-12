#pragma once
#include "base_pose_history50y.h"
#include <array>
#include <cstdint>

namespace rr::game {
//20F390 constructor write-set210434..21047C, common PodVehicle storage.
// Apply exactly once to a genuinely fresh racer BEFORE the50Y producer is
// scheduled. This is not a reset operation for a running racer or permission
// to replace a diagnostic/live ring. actual434630 comes from the original
// application initializer3C9F70. Unwritten slot words5..7 and all other fields
// are preserved from prior storage; no whole-owner zeroing occurs.
BasePoseHistoryState50Y projectOriginalBaseHistoryConstructor(
    const BasePoseHistoryState50Y& prior,
    const std::array<std::uint32_t,4>& actual434630) noexcept;
enum class OriginalCameraRacerHistoryStatus {
    Complete, InvalidInput, UnsupportedArithmetic, InsufficientHistory
};
struct OriginalCameraRacerHistoryResult {
    OriginalCameraRacerHistoryStatus status=OriginalCameraRacerHistoryStatus::InvalidInput;
    std::array<float,4> point180{};
    std::uint32_t selectedSlot=0, traversedSlots=0;
    float precedingDistance=0.f, fraction=0.f;
};
// Source17AE34..17AFDC only. Reuses the base PodVehicle's actual50Y owner;
// does not create a second ring or advance it. The caller supplies camera+04
// and admits the enclosing17AAC8 branch and original frame ordering separately.
// Previous point starts at (0,0,0,1), even when zero slots are traversed. Source
// distances and target are not clamped; slot order is currentFD0,+1,wrap20.
// nextFD4, caches and unused slot words are not consumed. No pointer is retained.
// FE_TONEAREST, ordinary finite normal/zero binary32, separately rounded ops.
// A selected zero denominator is UnsupportedArithmetic. Exhausting20 slots is
// InsufficientHistory (source assertion at17AEE4..17AF18), not fabricated pose.
// Only a Complete result contains a valid point/receipt. No state is modified.
OriginalCameraRacerHistoryResult readOriginalCameraRacerHistory(
    const BasePoseHistoryState50Y& history, float cameraDistance04) noexcept;
}
