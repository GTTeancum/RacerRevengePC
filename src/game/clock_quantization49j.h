#pragma once
#include "frame_timing49i.h"
#include <cstdint>

namespace rr::game {
// Typed, caller-owned software state, not the original global/MMIO layout.
// All member bit patterns are admitted. Initialize every member explicitly;
// padding is not part of this interface. No hardware sampling or IRQ atomicity
// is provided: the caller supplies the count and the pending-overflow word.
struct ClockCounterState49J {
    std::uint64_t accumulatedTicks;
    std::uint32_t pendingOverflows;
    std::uint16_t previousCount;
};
struct ClockQuantization49J {
    std::uint32_t roundedTickBits;
    std::uint32_t scaledMillisecondBits;
    std::uint32_t uint32timestamp;
};
struct ClockFrameOutput49J {
    std::uint32_t deltaCounterBits;
    ClockQuantization49J quantization;
    FrameElapsed49I elapsed;
};

// Consumes pendingOverflows, then accumulates the sign-extended low-word delta
// modulo 2^64, then stores currentCount. Returns that delta's raw uint32 bits.
// A decreasing count substitutes one overflow only when the FULL pending word
// was zero. Equal samples do not imply a wrap. No host rounding mode is needed.
std::uint32_t consumeTimerSample49J(ClockCounterState49J& state,
    std::uint16_t currentCount) noexcept;

// Original unsigned64 wrapper, sticky-adjusted software-double/narrowing
// conversion, separately rounded binary32 scale 0x3ae38e39, then nonnegative
// uint32 truncation/saturation. This is not a direct uint64-to-float cast.
// FE_TONEAREST is required for the host binary32 multiply. An unsupported
// rounding mode returns false and leaves output unchanged.
bool quantizeCounter49J(std::uint64_t accumulatedTicks,
    ClockQuantization49J& output) noexcept;

// Allocation-free numeric composition: consume -> quantize -> 49I begin.
// Unsupported rounding rejects before any state/output change, including the
// pending-overflow clear. All three objects must be live, initialized, disjoint.
// This ends at the prepared elapsed factor. The existing 49I manager-stage,
// late-captured flags, phase entry/exit and live-tail operations stay external.
// No clock source, scheduling, catch-up policy or callback is supplied here.
bool beginFrameFromTimerSample49J(ClockCounterState49J& clockState,
    FrameTimingState49I& frameState, std::uint16_t currentCount,
    std::uint32_t capWord423ED8, ClockFrameOutput49J& output) noexcept;
}
