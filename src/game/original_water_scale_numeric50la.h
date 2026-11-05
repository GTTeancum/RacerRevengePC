#pragma once
#include <cstdint>

namespace rr::game::detail {
// Reconstructed from docs/recovery/water-native-range-scale-20261105.md.
// Bounded soft-MUL specialization with RIGHT operand exactly +16. Neither
// general multiplication nor live FCR31 emulation. Refusal preserves outputs.
inline bool sourceWaterScale16Native50LA(std::uint32_t left,std::uint32_t right,
        std::uint32_t& output,std::uint32_t& localClearMask)noexcept {
    if(right!=0x41800000u)return false;
    const auto exponent=(left>>23u)&0xffu;
    if(left!=0u&&(exponent<1u||exponent>250u))return false;
    const auto result=left==0u?0u:(left&0x807fffffu)|((exponent+4u)<<23u);
    output=result;localClearMask=0x8000u;return true;
}
}
