#pragma once
#include <cstdint>
#include <limits>

namespace rr::platform {
// Host pacing only; does not replace the game's native timing conversion.
// Call plan once per monotonic host timestamp and execute exactly plan.steps.
struct FixedStepPlan50DT {
    std::uint32_t steps = 0;
    std::uint64_t waitNanoseconds = 0;
    std::uint64_t discardedNanoseconds = 0;
    bool timestampRegressed = false;
};
class FixedStepClock50DT {
public:
    bool initialize(std::uint64_t now, std::uint64_t stepNanoseconds,
                    std::uint32_t maximumCatchupSteps) noexcept {
        if (!stepNanoseconds || !maximumCatchupSteps ||
            stepNanoseconds > std::numeric_limits<std::uint64_t>::max()/maximumCatchupSteps)
            return false;
        previous_=now; step_=stepNanoseconds; maximum_=maximumCatchupSteps;
        accumulated_=0; ready_=true; return true;
    }
    // Focus loss/minimization: caller pauses game advancement and rebases time.
    void pause(std::uint64_t now) noexcept { previous_=now; accumulated_=0; }
    bool plan(std::uint64_t now, FixedStepPlan50DT& result) noexcept {
        result={}; if(!ready_) return false;
        if(now<previous_) {
            result.timestampRegressed=true; previous_=now; accumulated_=0;
            result.waitNanoseconds=step_; return true;
        }
        const auto elapsed=now-previous_; previous_=now;
        const auto ceiling=step_*maximum_;
        // Saturating catch-up drops oldest host time explicitly, never performs
        // an unbounded burst of native simulation steps after a stalled window.
        const auto room=ceiling-accumulated_;
        if(elapsed>room) {
            result.discardedNanoseconds=elapsed-room; accumulated_=ceiling;
        } else accumulated_+=elapsed;
        result.steps=static_cast<std::uint32_t>(accumulated_/step_);
        accumulated_%=step_;
        result.waitNanoseconds=step_-accumulated_;
        return true;
    }
private:
    std::uint64_t previous_=0,step_=0,accumulated_=0;
    std::uint32_t maximum_=0;
    bool ready_=false;
};
}
