#pragma once
#include "lap_completion50gg.h"
#include <cstdint>

namespace rr::game {
// Concrete native scene fields, already selected by this racer's vehicle5C:
// app+28C+134+28*5C, +130+28*5C, and +164+4*5C. The native scene
// may bind concrete OriginalRaceHud50KG objects for the human view. The native
// overlay remains a separate presentation owner.
// known=false means unrepresented, never an inferred null object.
struct LapHudFields50ID {
    std::uintptr_t lap134=0,race130=0,lastLap164=0;
    bool known=false;
};
enum class LapHudResult50ID : std::uint8_t {NullReturned,NeedNonNullOwner,Unknown,OtherCall};
inline LapHudResult50ID applyLapHudNullBranch50ID(const LapHudFields50ID& fields,
    const LapCompletionRequest50GG& request) noexcept {
    if(!fields.known)return LapHudResult50ID::Unknown;
    std::uintptr_t object=0;
    switch(request.kind){
    case LapCompletionCall50GG::LapRecord22B170:object=fields.lap134;break;
    case LapCompletionCall50GG::RaceRecord22B370:object=fields.race130;break;
    case LapCompletionCall50GG::LastLap22B760:object=fields.lastLap164;break;
    default:return LapHudResult50ID::OtherCall;
    }
    // Actual null branches22B1B0 ->22B354,22B3B0 ->22B554,
    //22B78C ->22B8A0 bypass all text/animation/subordinate effects.
    return object?LapHudResult50ID::NeedNonNullOwner:LapHudResult50ID::NullReturned;
}
}
