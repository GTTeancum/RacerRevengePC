#pragma once
// PS2 EE FPU arithmetic never produces or consumes subnormal floats: a
// subnormal operand is read as zero and a subnormal result is flushed to a
// signed zero. Recovered code throughout this port therefore rejects subnormal
// values as impossible ("scalar" checks). IEEE hardware with gradual underflow
// produces them from ordinary decays (camera smoothing, flame decay, timers),
// which then stopped races. Run the game thread with SSE flush-to-zero (FTZ)
// and denormals-are-zero (DAZ) so every float operation follows the console's
// rule. Rounding stays round-to-nearest; exception masks stay set.
#include <cstdint>
#include <cstdlib>
#if defined(__SSE2__) || defined(_M_X64) || defined(__x86_64__)
#include <xmmintrin.h>
#define RR_PS2_FLOAT_MODE_SSE50LA 1
#endif

namespace rr::game {
inline constexpr std::uint32_t ps2FlushBits50LA = 0x8040u; // FTZ | DAZ.

// True when this thread's SSE mode flushes/zeroes subnormals.
inline bool ps2FloatModeActive50LA() noexcept {
#if defined(RR_PS2_FLOAT_MODE_SSE50LA)
    return (static_cast<std::uint32_t>(_mm_getcsr()) & ps2FlushBits50LA) == ps2FlushBits50LA;
#else
    return true;
#endif
}
// Sets FTZ|DAZ on the calling thread only. Returns true if the mode had to be
// (re)applied. Other MXCSR fields are preserved.
inline bool enforcePs2FloatMode50LA() noexcept {
    // Unattended reproduction only: RR_TEST_IEEE_SUBNORMALS=1 keeps the old
    // (pre-fix) IEEE behaviour so the subnormal failures can be shown.
    static const bool ieeeSubnormals=[]{const char* v=std::getenv("RR_TEST_IEEE_SUBNORMALS");return v&&v[0]=='1'&&!v[1];}();
    if(ieeeSubnormals)return false;
#if defined(RR_PS2_FLOAT_MODE_SSE50LA)
    const auto csr = static_cast<std::uint32_t>(_mm_getcsr());
    if ((csr & ps2FlushBits50LA) == ps2FlushBits50LA) return false;
    _mm_setcsr(csr | ps2FlushBits50LA);
    return true;
#else
    return false;
#endif
}
}
