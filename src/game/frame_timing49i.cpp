#include "frame_timing49i.h"
#include <cfenv>
#include <cstring>
#include <limits>
#include <type_traits>

namespace rr::game {
namespace {
static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559 &&
    std::numeric_limits<float>::digits == 24, "49I requires IEEE binary32");
static_assert(sizeof(FrameTimingState49I) == 28 &&
    std::is_trivially_copyable<FrameTimingState49I>::value &&
    std::is_standard_layout<FrameTimingState49I>::value,
    "49I state must remain a seven-word typed projection");

float fromBits(std::uint32_t word) noexcept {
    float result;
    std::memcpy(&result, &word, sizeof(result));
    return result;
}

float originalUnsignedToFloat(std::uint32_t value) noexcept {
    // Original 140BEC..140C1C: high-bit words use a sticky low bit before
    // signed conversion and a separately rounded doubling operation.
    if (value & 0x80000000u) {
        const auto half = static_cast<std::int32_t>((value >> 1) | (value & 1u));
        volatile float converted = static_cast<float>(half);
        volatile float doubled = converted + converted;
        return doubled;
    }
    volatile float converted = static_cast<float>(static_cast<std::int32_t>(value));
    return converted;
}
}

bool beginFrameTiming49I(FrameTimingState49I& state,
    std::uint32_t sampledQuantizedMilliseconds, std::uint32_t capWord423ED8,
    FrameElapsed49I& output) noexcept {
    // Admission precedes even the source's zero-previous timestamp reset.
    if (std::fegetround() != FE_TONEAREST) return false;
    if (state.previous234 == 0)
        resetFrameTimestamps49I(state, sampledQuantizedMilliseconds);
    else
        state.current230 = sampledQuantizedMilliseconds;

    const std::uint32_t delta = state.current230 - state.previous234;
    volatile float raw = fromBits(0x3a83126fu) * originalUnsignedToFloat(delta);
    state.raw250 = raw;
    float elapsed = raw;
    const float ordinaryCap = fromBits(0x3d75c28fu);
    if (!(elapsed <= ordinaryCap)) elapsed = ordinaryCap;
    if (capWord423ED8 != 0) {
        const float optionalCap = fromBits(0x3d08850au);
        if (!(elapsed <= optionalCap)) elapsed = optionalCap;
    }
    output = {delta, elapsed};
    return true;
}

void resetFrameTimestamps49I(FrameTimingState49I& state,
    std::uint32_t sampledQuantizedMilliseconds) noexcept {
    state.previous234 = sampledQuantizedMilliseconds;
    state.current230 = sampledQuantizedMilliseconds;
}

FrameSimulationEntry49I enterFrameSimulation49I(FrameTimingState49I& state,
    std::uint32_t rootFlags228AfterManager, float elapsedSeconds) noexcept {
    if (rootFlags228AfterManager & 8u) return FrameSimulationEntry49I::Skipped;
    std::uint32_t word;
    std::memcpy(&word, &elapsedSeconds, sizeof(word));
    // Positive IEEE words are monotone. This avoids consuming a NaN through
    // floating comparison and admits both zero signs without canonicalization.
    const std::uint32_t magnitude = word & 0x7fffffffu;
    if (magnitude > 0x3d75c28fu || ((word & 0x80000000u) && magnitude != 0))
        return FrameSimulationEntry49I::InvalidElapsed;
    state.phase22C = 2;
    std::memcpy(&state.lastDispatched23C, &elapsedSeconds, sizeof(elapsedSeconds));
    return FrameSimulationEntry49I::Ready;
}

void leaveFrameSimulation49I(FrameTimingState49I& state) noexcept {
    state.phase22C = 0;
}

void finishFrameTiming49I(FrameTimingState49I& state) noexcept {
    state.previous234 = state.current230;
    if (state.pending120 != 0) {
        state.active11C = 1;
        state.pending120 = 0;
    }
}
}
