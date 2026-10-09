#pragma once
#include "lap_event_random50id.h"
#include <chrono>
#include <cstdint>
#include <limits>

namespace rr::game {
// Explicit HOST backend, application lifetime. This supplies native elapsed
// wall time, not a claim to reproduce the original console's sampled values.
// Use ONE backend for source101A50 callers, including Dax's shared lazy seed.
struct HostMonotonicClock50ID {
    std::chrono::steady_clock::time_point epoch=std::chrono::steady_clock::now();
    std::uint64_t lastNanoseconds=0;
};
inline bool hostNanosecondsToSourceTimestamp50ID(std::uint64_t nanoseconds,
    std::uint32_t& timestamp) noexcept {
    // Original timer selector yields576000 ticks/second;49J multiplies by
    // binary32 bits3AE38E39 (~1/576), returning uint32 milliseconds. Splitting
    // seconds/remainder avoids nanoseconds*576000 overflow. No MMIO/IRQ is
    // simulated, and no ideal-millisecond shortcut bypasses49J rounding.
    constexpr std::uint64_t billion=1000000000u,ticksPerSecond=576000u;
    const auto seconds=nanoseconds/billion,remainder=nanoseconds%billion;
    if(seconds>std::numeric_limits<std::uint64_t>::max()/ticksPerSecond)return false;
    const auto ticks=seconds*ticksPerSecond+remainder*ticksPerSecond/billion;
    ClockQuantization49J output{};
    if(!quantizeCounter49J(ticks,output))return false;
    timestamp=output.uint32timestamp;return true;
}
inline bool sampleHostTimestamp50ID(HostMonotonicClock50ID& clock,
    std::uint32_t& timestamp) noexcept {
    const auto elapsed=std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now()-clock.epoch).count();
    if(elapsed<0)return false;
    const auto ns=static_cast<std::uint64_t>(elapsed);
    if(ns<clock.lastNanoseconds||!hostNanosecondsToSourceTimestamp50ID(ns,timestamp))return false;
    clock.lastNanoseconds=ns;return true;
}
inline bool applyHostLapClockOrRandom50ID(const LapEventRequest50ID& request,
    HostMonotonicClock50ID& clock,std::uint32_t& sharedRawRandom,
    std::uint32_t& actualV0) noexcept {
    if(request.kind==LapEventCall50ID::Clock101A50)
        return sampleHostTimestamp50ID(clock,actualV0);
    return applyLapEventRandom50ID(request,sharedRawRandom,actualV0);
}
// Race lap timing uses the actual accepted native step's binary32 seconds
// (.016f in50DU), not this wall clock or the HOST observer's interpolated
// finishSeconds. Paused/countdown/abandoned steps do not advance lap timing.
}
