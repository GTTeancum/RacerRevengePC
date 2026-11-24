#pragma once
// Native progress record for the original Results and the common race flow
// (docs/common-results-20261108.md; common rewrite R9, PLAN 2.6-2.8). The
// original race load 1A8A40, the end entry 1A7DD0 and the race start 1AC8E0
// are common to every course and every pod. This record only says which
// original steps were reached this race, plus the borrowed port-side owners
// the race-flow entries need; it is reset when a race is activated and never
// stands in for an original owner or acknowledgement.
#include <cstdint>
#include "../../src/game/original_race_ack_route50pd.h"
#include <functional>

namespace common_race_start50CR { struct Hooks; }
namespace original_camera_cycle_audio50KX { class Owner; }

namespace native_common_results50PD {
struct State {
    std::function<bool(std::uint32_t)> restartFromResults;
    bool rootPublished=false;   // publishNativeHumanResultsRoot50LA succeeded
    bool endActive=false;       // 1A7DD0 end entry has been entered
    // The end dispatch refused (common_race_end route=refused); the race
    // departs to the Course screen through step 18. (The host text panel
    // HostRaceResults50IL that this flag used to select is deleted.)
    bool fallback=false;
    bool replayActive=false;    // 1AB520 returned with the race78 playback on
    bool acknowledged=false;    // Results+20C nonzero observed (1AB5BC branch due)
    bool routeChosen=false;     // 1AB5BC chose its 17DE10 request
    bool endLogged=false;       // common_race_end printed for this 1A7DD0 dispatch
    bool endRefusedLogged=false;// common_race_end route=refused printed for this dispatch
    rr::game::OriginalRaceAckDestination50PD route{};
    std::uint64_t endCalls=0;
    std::uint32_t lastPC=0;
    std::uint8_t lastStatus=0xffu;
    const char* lastOperation=nullptr;
    // Set by the race publication/loop for every course (the race's one
    // World74). 1AC8E0's World74 leaves (trigger list 2DCC70, particles
    // 161F60, world restart 2702E0) and 1ABAE8..1ABB68 (racer +3C = 1,
    // 229F80, VCR bit1/bit2) are performed by the track's own owners.
    std::function<bool()> resetReplayGrid,fadeDeparture;
    std::function<void()> resetTriggers;
    std::function<bool(const char*&)> resetParticles,resetScene,beginReplay;
    // Common rewrite (R9): borrowed for the life of the counted race, bound by
    // I3 at race publication next to common_race_start50CR::Owner::bind and
    // cleared by reset(). Never owned here.
    //  raceStartHooks: the I3 Hooks the in-place 1AC8E0 runs with, for the
    //    callers that hold only the Application (Pause RESTART 1BD934,
    //    Results rows); the per-frame end dispatch receives them directly.
    //  raceCycleAudio: the counted race camera session's race10C+160 feedback
    //    emitter (1A4B90 keys, and the 26D070 stop of the step-18 departure).
    const common_race_start50CR::Hooks* raceStartHooks=nullptr;
    original_camera_cycle_audio50KX::Owner* raceCycleAudio=nullptr;
};
inline State& state() noexcept { static State owner; return owner; }
inline void reset() noexcept { state()=State{}; }
} // namespace native_common_results50PD
