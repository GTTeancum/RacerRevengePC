#pragma once
#include "original_particle_fog_add50la.h"
#include <cstdint>

namespace rr::game::detail {
// Reconstructed from docs/recovery/water-opacity-subtract-scalar-tail-20261105.md.
// Bounded soft-ADDSUB value inference, not live FCR31 or hardware equivalence.
// Refusal leaves both outputs unchanged; the mask describes only this operation.
inline bool sourceWaterOpacitySubtract50LA(std::uint32_t left,std::uint32_t right,
        std::uint32_t& output,std::uint32_t& localClearMask)noexcept {
    const auto admitted=[](std::uint32_t word)noexcept {
        const auto exponent=(word>>23u)&0xffu;
        return word==0u||(exponent>=107u&&exponent<=156u);
    };
    if(!admitted(left)||!admitted(right))return false;
    std::uint32_t result=0;
    if(!sourceParticleFogSubtract50LA(left,right,result))return false;
    const auto exponent=(result>>23u)&0xffu;
    if(result!=0u&&(exponent<83u||exponent>157u))return false;
    output=result;localClearMask=0x8000u;return true;
}
}
