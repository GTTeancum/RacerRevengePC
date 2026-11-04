#pragma once
#include "lap_event_random50id.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <limits>

namespace rr::game {
// Explicit HOST backend, application lifetime. This supplies native elapsed
// wall time, not a claim to reproduce the original console's sampled values.
// Use ONE backend for source101A50 callers, including Dax's shared lazy seed.
struct HostMonotonicClock50ID {
    std::chrono::steady_clock::time_point epoch=std::chrono::steady_clock::now();
    std::uint64_t lastNanoseconds=0;
};
// Shared HOST nanosecond-to49J numeric projection. Neither this nor its
// callers sample an EE timer, consume guest overflows or certify IRQ custody.
inline bool hostNanosecondsToSourceClockQuantization50ID(std::uint64_t nanoseconds,
    ClockQuantization49J& output) noexcept {
    // Original timer selector yields576000 ticks/second;49J multiplies by
    // binary32 bits3AE38E39 (~1/576). Preserve the full quantization: fades
    // need FLOAT milliseconds, while101A50 uses the uint32 conversion.
    // Splitting seconds/remainder avoids nanoseconds*576000 overflow.
    constexpr std::uint64_t billion=1000000000u,ticksPerSecond=576000u;
    const auto seconds=nanoseconds/billion,remainder=nanoseconds%billion;
    if(seconds>std::numeric_limits<std::uint64_t>::max()/ticksPerSecond)return false;
    const auto ticks=seconds*ticksPerSecond+remainder*ticksPerSecond/billion;
    return quantizeCounter49J(ticks,output);
}
inline bool hostNanosecondsToSourceTimestamp50ID(std::uint64_t nanoseconds,
    std::uint32_t& timestamp) noexcept {
    ClockQuantization49J output{};
    if(!hostNanosecondsToSourceClockQuantization50ID(nanoseconds,output))return false;
    timestamp=output.uint32timestamp;return true;
}
// One actual host sample and one shared lastNanoseconds commit. Callers must
// borrow the application's live clock, not a copied or per-effect clock. The
// output must be separate from that clock. Failure leaves both unchanged.
inline bool sampleHostClockQuantization50ID(HostMonotonicClock50ID& clock,
    ClockQuantization49J& output) noexcept {
    const auto elapsed=std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now()-clock.epoch).count();
    if(elapsed<0)return false;
    const auto ns=static_cast<std::uint64_t>(elapsed);
    ClockQuantization49J next{};
    if(ns<clock.lastNanoseconds||!hostNanosecondsToSourceClockQuantization50ID(ns,next))return false;
    clock.lastNanoseconds=ns;output=next;return true;
}
inline bool sampleHostTimestamp50ID(HostMonotonicClock50ID& clock,
    std::uint32_t& timestamp) noexcept {
    ClockQuantization49J output{};
    if(!sampleHostClockQuantization50ID(clock,output))return false;
    timestamp=output.uint32timestamp;return true;
}
inline bool sampleHostFloatMilliseconds50ID(HostMonotonicClock50ID& clock,
    float& milliseconds) noexcept {
    ClockQuantization49J output{};
    if(!sampleHostClockQuantization50ID(clock,output))return false;
    // Preserve49J's separately rounded absolute binary32 sample. Never cast
    // its saturated uint32 timestamp or substitute ideal/delta milliseconds.
    std::memcpy(&milliseconds,&output.scaledMillisecondBits,sizeof(milliseconds));
    return true;
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
