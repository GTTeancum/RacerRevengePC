#pragma once
#include <cstdint>
#include <limits>

// Host deadline arithmetic only. This type owns no clock, window, renderer,
// audio or source authority. Runtime supplies actual monotonic observations;
// tests may supply numbers without creating any runtime completion receipt.
namespace original_ui_rider_pacing50LB {
inline constexpr std::uint64_t periodNanoseconds=16666667ull; // Native PC60Hz policy, rounded up.
inline constexpr std::uint64_t maximumSleepSliceNanoseconds=1000000ull;
enum class Error : std::uint8_t {none,phase,backwards,overflow,early};
struct Observation {
    Error error=Error::none;
    bool due=false;
    std::uint64_t sleepNanoseconds=0;
};
class Deadline final {
public:
    Error beginWait(std::uint64_t now)noexcept {
        if(waiting_)return Error::phase;
        if(started_&&now<lastObserved_)return Error::backwards;
        if(!started_&&now>maximum()-periodNanoseconds)return Error::overflow;
        if(!started_)deadline_=now+periodNanoseconds;
        started_=true;waiting_=true;lastObserved_=now;return Error::none;
    }
    Observation observe(std::uint64_t now)noexcept {
        if(!waiting_)return {Error::phase,false,0};
        if(now<lastObserved_)return {Error::backwards,false,0};
        lastObserved_=now;
        if(now>=deadline_)return {Error::none,true,0};
        const auto remaining=deadline_-now;
        return {Error::none,false,remaining<maximumSleepSliceNanoseconds?remaining:maximumSleepSliceNanoseconds};
    }
    Error finishWait(std::uint64_t now)noexcept {
        if(!waiting_)return Error::phase;
        if(now<lastObserved_)return Error::backwards;
        if(now<deadline_)return Error::early;
        if(now>maximum()-periodNanoseconds)return Error::overflow;
        // Re-anchor after actual service, including oversleep/late frames.
        // Never produce catch-up submissions or synthetic elapsed intervals.
        deadline_=now+periodNanoseconds;lastObserved_=now;waiting_=false;return Error::none;
    }
    void reset()noexcept {*this=Deadline{};}
private:
    static constexpr std::uint64_t maximum()noexcept {return std::numeric_limits<std::uint64_t>::max();}
    bool started_=false,waiting_=false;
    std::uint64_t deadline_=0,lastObserved_=0;
};
}
