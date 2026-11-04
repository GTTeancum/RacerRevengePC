#pragma once
#include <array>
#include <cstdint>
#include "original_particle_random_mode50la.h"
#include "original_particle_fog_multiply50la.h"

namespace rr::game::detail {
// Bounded result-word inference from the pinned scalar PS2Float model with
// CHECK_FPU_SOFT_MUL selected. Not silicon equivalence, general EE arithmetic,
// source custody or runtime-particle admission. See NUMERIC-PROOF.md.
// Every numeric refusal leaves output unchanged; no helper reads or draws RNG.

// Source16073C/160760/160780: fs=MAGNITUDE,ft=UNIT. The public argument
// order follows the existing Fog caller; the delegated model order must not.
// Magnitude is+0 or any positive ordinary word in[2^-95,2^127]; unit is+0
// or positive normal E96..126. Use BOTH actual significands, including the
// unit's preceding scale correction. Prior LEFT-power results are unchanged.
// The same value interval retains result+0 or positive E1..253.
inline bool sourceParticleFogVelocityProduct50LA(std::uint32_t unit,
        std::uint32_t magnitude,std::uint32_t& output) noexcept {
    if(!sourceParticleMode2PositiveUnit(unit)||
       (magnitude!=0u&&(magnitude<0x10000000u||magnitude>0x7f000000u)))return false;
    return sourceParticleFogMultiply50LA(magnitude,unit,output);
}

// Value-shape predicate only. A published velocity lane may have either sign,
// including-0, but no subnormal or exponent254/255 is in this product image.
// Source occurrence, exact products, branch decisions and stores remain the
// caller's separately authenticated obligations.
inline bool sourceParticleFogVelocityWord50LA(std::uint32_t word) noexcept {
    if((word&0x7fffffffu)==0u)return true;
    const auto exponent=(word>>23u)&0xffu;
    return exponent>=1u&&exponent<=253u;
}

// Source1607E0/160830/160880: fs=the actual positive stack lane,ft=-1.
// For RIGHT significand800000 every Booth selector is0, so the correction
// is0 and prior fraction bits survive unchanged. E+127-127 remains E.
// The model's explicit zero branch uses sign XOR: +0 times-1 is-0.
// Only the genuine pre-negation image(+0 or positive E1..253) is admitted.
inline bool sourceParticleFogVelocityNegate50LA(std::uint32_t input,
        std::uint32_t& output) noexcept {
    if((input&0x80000000u)||!sourceParticleFogVelocityWord50LA(input))return false;
    output=input^0x80000000u;return true;
}

// Source16088C stores all four words. W comes from the literal1 at160790;
// it is never a sign-selected lane or an inferred constructor replacement.
inline bool sourceParticleFogVelocityVector50LA(
        const std::array<std::uint32_t,4>& words) noexcept {
    return sourceParticleFogVelocityWord50LA(words[0])&&
        sourceParticleFogVelocityWord50LA(words[1])&&
        sourceParticleFogVelocityWord50LA(words[2])&&words[3]==0x3f800000u;
}
} // namespace rr::game::detail
