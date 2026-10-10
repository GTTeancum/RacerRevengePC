#pragma once
#include <cstdint>

namespace rr::game {
// A typed, caller-owned projection of original 140AC0 state, not its object
// layout. Initialize every lane explicitly. In particular, phase22C has no
// general constructor default recovered by this interface.
struct FrameTimingState49I {
    std::uint32_t current230;
    std::uint32_t previous234;
    float raw250;
    float lastDispatched23C;
    std::uint32_t phase22C;
    std::uint32_t active11C;
    std::uint32_t pending120;
};
struct FrameElapsed49I {
    std::uint32_t deltaMilliseconds;
    float elapsedSeconds;
};
enum class FrameSimulationEntry49I { Skipped, Ready, InvalidElapsed };

// The timestamp is the original already-quantized/saturating uint32 clock
// result, not ideal OS milliseconds. The complete post-clock numeric operation
// admits only host float32 nearest-even rounding. Unsupported rounding leaves
// both state and output unchanged. Delta subtraction wraps modulo 2^32; raw250
// stores the uncapped seconds before the ordinary and optional upper caps.
// A nonzero capWord423ED8 applies a second upper cap, never a fixed step.
// The caller must pass the returned factor to the original manager stage,
// then capture root flags immediately after that stage. A manager-stage change
// to the cap word affects a later begin, not this already-prepared factor.
bool beginFrameTiming49I(FrameTimingState49I& state,
    std::uint32_t sampledQuantizedMilliseconds, std::uint32_t capWord423ED8,
    FrameElapsed49I& output) noexcept;

// Original 140D70 field writes only: previous first, then current.
void resetFrameTimestamps49I(FrameTimingState49I& state,
    std::uint32_t sampledQuantizedMilliseconds) noexcept;

// Supply the root flags CAPTURED immediately after manager 128080 (source
// 140C98), without rereading them after any later action. If captured bit 3 is
// clear, the caller must first perform the admitted opaque 245030/conditional
// global virtual action, which precedes the source phase/factor writes. Then
// enter with those captured flags and the already-prepared factor. If bit 3 is
// set, skip that pre-dispatch action: enter skips before consuming elapsedSeconds
// and leaves phase/last-dispatched factor untouched. Otherwise the native factor
// must be finite and in [0, original 0.06f], with either signed zero preserved.
// Ready writes phase=2 before last-dispatched factor; the actual root callback
// belongs AFTER Ready. Neither opaque action nor root callback is invoked here.
// State and output objects must be live, initialized and disjoint.
FrameSimulationEntry49I enterFrameSimulation49I(FrameTimingState49I& state,
    std::uint32_t rootFlags228AfterManager, float elapsedSeconds) noexcept;

// Call only after the actual root callback. Does not emulate any callback.
void leaveFrameSimulation49I(FrameTimingState49I& state) noexcept;

// Call after the original opaque 267BE0 stage. Reads LIVE current/pending words,
// copies current to previous, and promotes nonzero pending to active=1/pending=0.
// The other state lanes are untouched. All five operations are allocation-free,
// serial numeric/state boundaries; scheduling and opaque effects stay external.
void finishFrameTiming49I(FrameTimingState49I& state) noexcept;
}
