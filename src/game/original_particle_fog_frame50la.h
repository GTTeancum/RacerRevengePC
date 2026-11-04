#pragma once
#include <cstdint>
#include "original_particle_random_mode50la.h"
#include "original_particle_fog_multiply50la.h"

namespace rr::game::detail {
// Bounded source160A18 result-word inference from pinned scalar PS2Float
// with CHECK_FPU_SOFT_MUL selected. SPAN is LEFT, UNIT is RIGHT. Retain
// the full correction using both actual significands, including the unit's
// preceding scale correction. Span: literal+0 or any exact integer1..2^24.
// Unit: literal+0 or positive normal E96..126. Product: +0 or E96..150.
// No RNG, source custody, hardware equivalence or general EE backend claim.
// See FRAME-PROOF.md. Every false return leaves output unchanged.
inline bool sourceParticleFogFrameProduct50LA(std::uint32_t span,
        std::uint32_t unit,std::uint32_t& output) noexcept {
    if(span!=0u){
        if(span<0x3f800000u||span>0x4b800000u)return false;
        const auto exponent=(span>>23u)&0xffu;
        //The producer is signed integer CVT.S.W, not an arbitrary fraction.
        //For E127..149 its low150-E bits must be0; E150 is integral, and
        //the upper bound admits only exact2^24 at E151.
        if(exponent<150u&&(span&((std::uint32_t{1}<<(150u-exponent))-1u)))return false;
    }
    if(!sourceParticleMode2PositiveUnit(unit))return false;
    return sourceParticleFogMultiply50LA(span,unit,output);
}

// Source160A1C CVT.W.S: signed32 conversion toward zero, restricted to the
// genuine preceding product's +0 or positive-normal E96..150 value shape.
// Fractions below1 become0; all remaining conversions use right shifts0..23.
// Result is0..00FFFFFF. No negative, subnormal, exceptional or saturation
// behavior is admitted, and no host float/cast/rounding environment is used.
inline bool sourceParticleFogFrameOffset50LA(std::uint32_t product,
        std::uint32_t& output) noexcept {
    if(product==0u){output=0u;return true;}
    if(product&0x80000000u)return false;
    const auto exponent=(product>>23u)&0xffu;
    if(exponent<96u||exponent>150u)return false;
    const auto significand=0x00800000u|(product&0x007fffffu);
    const auto result=exponent<127u?0u:significand>>(150u-exponent);
    output=result;return true;
}
} // namespace rr::game::detail
