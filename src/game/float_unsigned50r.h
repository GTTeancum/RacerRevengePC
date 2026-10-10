#pragma once
#include <cstdint>
namespace rr::game {
// Original310F38 +3105A8 conversion, raw single-precision input. Subnormal/NaN
// and negative inputs become zero; positive infinity/overflow saturate UINT_MAX.
// Positive finite values truncate toward zero. No host FP conversion/rounding.
std::uint32_t sourceFloatUnsigned50R(std::uint32_t raw) noexcept;
}
