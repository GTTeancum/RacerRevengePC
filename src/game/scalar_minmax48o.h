#pragma once
#include <cstdint>
#include <cstring>

namespace rr::game::detail {
// Scalar MIN.S/MAX.S ordering for the native ordinary finite binary32 domain,
// including -0 below +0. This is distinct from host floating comparison ties.
// It does not establish EE NaN/denormal/overflow modes or VU instruction rules.
inline std::uint32_t scalarOrderKey48O(float value) noexcept {
    std::uint32_t word;std::memcpy(&word,&value,sizeof(word));
    return word&0x80000000u?~word:word|0x80000000u;
}
inline float scalarMin48O(float first,float second) noexcept {
    return scalarOrderKey48O(first)<scalarOrderKey48O(second)?first:second;
}
inline float scalarMax48O(float first,float second) noexcept {
    return scalarOrderKey48O(first)>scalarOrderKey48O(second)?first:second;
}
}
