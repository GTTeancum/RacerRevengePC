#include "clock_quantization49j.h"
#include <cfenv>
#include <cstring>
#include <limits>
#include <type_traits>

namespace rr::game {
namespace {
static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559 &&
    std::numeric_limits<float>::digits == 24, "49J requires IEEE binary32");
static_assert(sizeof(ClockCounterState49J) == 16 &&
    std::is_trivially_copyable<ClockCounterState49J>::value &&
    std::is_standard_layout<ClockCounterState49J>::value,
    "49J counter state must remain a compact typed projection");
static_assert(sizeof(ClockQuantization49J) == 12 &&
    sizeof(ClockFrameOutput49J) == 24, "49J outputs must remain compact");

unsigned highestBit(std::uint64_t value) noexcept {
    unsigned result = 0;
    while (value >>= 1) ++result;
    return result;
}

std::uint64_t nearestEvenShift(std::uint64_t value, unsigned shift) noexcept {
    if (!shift) return value;
    const std::uint64_t retained = value >> shift;
    const std::uint64_t remainder = value & ((std::uint64_t{1} << shift) - 1);
    const std::uint64_t half = std::uint64_t{1} << (shift - 1);
    return retained + (remainder > half || (remainder == half && (retained & 1)));
}

std::uint32_t nonnegativeHelperBits(std::uint64_t value) noexcept {
    if (!value) return 0;
    // Original 30E4E8 does not directly round the integer to binary32. Above
    // 2^53 it ORs bit 11 when any low 11 bits are set, then constructs a
    // software binary64 value and narrows it. Preserve the resulting rare
    // double-rounding quirk instead of replacing it with an ideal cast.
    if (value >= (std::uint64_t{1} << 53) && (value & 0x7ffu)) value |= 0x800u;
    unsigned top = highestBit(value);
    const unsigned doubleDiscard = top > 52 ? top - 52 : 0;
    // Helper input is <=2^63-1, so the rounded integer is at most 2^63.
    value = nearestEvenShift(value, doubleDiscard) << doubleDiscard;
    top = highestBit(value);
    std::uint64_t significand = top > 23 ? nearestEvenShift(value, top - 23)
                                          : value << (23 - top);
    if (significand == (std::uint64_t{1} << 24)) {
        significand >>= 1;
        ++top;
    }
    return ((top + 127u) << 23) | static_cast<std::uint32_t>(significand & 0x7fffffu);
}

std::uint32_t roundedCounterBits(std::uint64_t value) noexcept {
    // Original 2EAE10 uses the signed helper directly only for positive signed
    // inputs. Its zero/high-bit branch sticky-halves, converts, then doubles.
    // The latter doubling is exact for this zero/positive-normal domain.
    if (value && !(value & (std::uint64_t{1} << 63))) return nonnegativeHelperBits(value);
    const auto halfBits = nonnegativeHelperBits((value >> 1) | (value & 1u));
    return halfBits ? halfBits + 0x00800000u : 0u;
}

float fromBits(std::uint32_t bits) noexcept {
    float result;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}

std::uint32_t nonnegativeTimestamp(std::uint32_t bits) noexcept {
    // Original 310F38 restricted to the proven counter-derived finite domain.
    // Zero and positive normal values are the only possible inputs here.
    const int exponent = static_cast<int>((bits >> 23) & 255u) - 127;
    if (exponent < 0) return 0;
    if (exponent >= 32) return 0xffffffffu;
    const auto significand = (bits & 0x7fffffu) | 0x800000u;
    return exponent >= 23 ? significand << static_cast<unsigned>(exponent - 23)
                          : significand >> static_cast<unsigned>(23 - exponent);
}
}

std::uint32_t consumeTimerSample49J(ClockCounterState49J& state,
    std::uint16_t currentCount) noexcept {
    const auto pending = state.pendingOverflows;
    state.pendingOverflows = 0;
    std::uint32_t delta;
    if (currentCount >= state.previousCount) {
        delta = (pending << 16) + (std::uint32_t{currentCount} - state.previousCount);
    } else {
        const auto effective = pending == 0 ? 1u : pending;
        delta = (effective << 16) - (std::uint32_t{state.previousCount} - currentCount);
    }
    const std::uint64_t extended = (delta & 0x80000000u)
        ? (0xffffffff00000000ull | delta) : delta;
    state.accumulatedTicks += extended;
    state.previousCount = currentCount;
    return delta;
}

bool quantizeCounter49J(std::uint64_t accumulatedTicks,
    ClockQuantization49J& output) noexcept {
    if (std::fegetround() != FE_TONEAREST) return false;
    const auto rounded = roundedCounterBits(accumulatedTicks);
    volatile float scaled = fromBits(rounded) * fromBits(0x3ae38e39u);
    const float saved = scaled;
    std::uint32_t scaledBits;
    std::memcpy(&scaledBits, &saved, sizeof(scaledBits));
    output = {rounded, scaledBits, nonnegativeTimestamp(scaledBits)};
    return true;
}

bool beginFrameFromTimerSample49J(ClockCounterState49J& clockState,
    FrameTimingState49I& frameState, std::uint16_t currentCount,
    std::uint32_t capWord423ED8, ClockFrameOutput49J& output) noexcept {
    if (std::fegetround() != FE_TONEAREST) return false;
    ClockCounterState49J nextClock{clockState.accumulatedTicks,
        clockState.pendingOverflows, clockState.previousCount};
    FrameTimingState49I nextFrame;
    // Preserve every unconsumed float lane, including arbitrary NaN payloads.
    std::memcpy(&nextFrame, &frameState, sizeof(nextFrame));
    ClockFrameOutput49J next{};
    next.deltaCounterBits = consumeTimerSample49J(nextClock, currentCount);
    if (!quantizeCounter49J(nextClock.accumulatedTicks, next.quantization) ||
        !beginFrameTiming49I(nextFrame, next.quantization.uint32timestamp,
            capWord423ED8, next.elapsed)) return false;
    clockState.pendingOverflows = nextClock.pendingOverflows;
    clockState.accumulatedTicks = nextClock.accumulatedTicks;
    clockState.previousCount = nextClock.previousCount;
    std::memcpy(&frameState, &nextFrame, sizeof(frameState));
    output = next;
    return true;
}
}
