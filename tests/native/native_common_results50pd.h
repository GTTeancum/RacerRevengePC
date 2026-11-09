#pragma once
// Native progress record for the original Results on the common race
// (docs/common-results-20261108.md). The original race load 1A8A40 and end
// entry 1A7DD0 are common to every course; RA reaches them through its own
// world join, the other consumed races through this record. It only says
// which original steps were reached this race; it is reset when a race is
// activated and never stands in for an original owner or acknowledgement.
#include <cstdint>
#include "../../src/game/original_race_ack_route50pd.h"
#include <functional>

namespace native_common_results50PD {
struct State {
    bool rootPublished=false;   // publishNativeHumanResultsRoot50LA succeeded
    bool endActive=false;       // 1A7DD0 end entry has been entered
    bool fallback=false;        // the end entry stopped; host panel shown
    bool replayActive=false;    // 1AB520 returned with the race78 playback on
    bool acknowledged=false;    // Results+20C nonzero observed (1AB5BC branch due)
    bool routeChosen=false;     // 1AB5BC chose its 17DE10 request
    rr::game::OriginalRaceAckDestination50PD route{};
    std::uint64_t endCalls=0;
    std::uint32_t lastPC=0;
    std::uint8_t lastStatus=0xffu;
    const char* lastOperation=nullptr;
    // Set by the loop for a course without the RA world join. 1AC8E0's
    // World74 leaves (trigger list 2DCC70, particles 161F60, world restart
    // 2702E0) and 1ABAE8..1ABB68 (racer +3C = 1, 229F80, VCR bit1/bit2) are
    // performed by the native course owners through these.
    std::function<bool(const char*&)> restartWorld,beginReplay;
};
inline State& state() noexcept { static State owner; return owner; }
inline void reset() noexcept { state()=State{}; }
} // namespace native_common_results50PD
