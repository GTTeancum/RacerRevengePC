#pragma once
#include <cstdint>
#include "original_particle_random_mode50la.h"
#include "original_particle_fog_add50la.h"
#include "original_particle_fog_multiply50la.h"
#include "original_source_particle_random50la.h"

namespace rr::game::detail {
// Bounded result-word inference from the pinned scalar PS2Float model with
// CHECK_FPU_SOFT_MUL and CHECK_FPU_SOFT_ADDSUB selected. Not a hardware claim or a
// general EE backend. See NUMERIC-PROOF.md for source operands and bounds.
// Every false return leaves output unchanged; no helper draws or seeds RNG.

// Source1605FC/160618/160644/160734/160758/16077C/1607BC/16080C/16085C/
// 160A14 all put constant2^-31 on the LEFT and actual CVT.S.W on the RIGHT.
// Their exact operand domain matches15B16C. Reuse its proven correction,
// rather than the legacy Fog exponent-only helper. Other operations retain
// their existing guards and independently authenticated source chronology.
inline bool sourceParticleFogScale50LA(std::uint32_t converted,
        std::uint32_t& output) noexcept {
    return sourceParticleMode2Scale(converted,output);
}

// Source160664/160680/16069C: fs=maximum,ft=minimum. Admit signed
// zero and ordinary finite normals through the bounded model SUB. Preserve
// its actual difference even if the later MUL refuses a non-power width;
// no whole-range preflight may erase this completed source operation.
inline bool sourceParticleFogRangeDifference50LA(std::uint32_t maximum,
        std::uint32_t minimum,std::uint32_t& output) noexcept {
    return sourceParticleFogSubtract50LA(maximum,minimum,output);
}

// Source160668/160684/1606A0: fs=UNIT,ft=WIDTH, unlike the velocity
// magnitude products. Width keeps the old value interval[2^-95,2^127],
// now including ordinary fractions, plus literal+0. The complete pinned
// correction uses BOTH actual significands in this exact operand order.
// Previously admitted RIGHT-power cases retain their zero correction.
inline bool sourceParticleFogRangeProduct50LA(std::uint32_t unit,
        std::uint32_t width,std::uint32_t& output) noexcept {
    if(!sourceParticleMode2PositiveUnit(unit)||
       (width!=0u&&(width<0x10000000u||width>0x7f000000u)))return false;
    return sourceParticleFogMultiply50LA(unit,width,output);
}

// Source16066C/160688/1606A4: fs=MINIMUM,ft=PRODUCT. Keep the actual
// preceding MUL image: literal+0 or positive normal E1..253. Minimum may
// have either sign; the model ADD may produce either sign or cancellation.
// Nonzero result exponents outside1..254 refuse without publishing a sum.
inline bool sourceParticleFogRangeSum50LA(std::uint32_t minimum,
        std::uint32_t product,std::uint32_t& output) noexcept {
    if(product&0x80000000u)return false;
    if(product!=0u){const auto exponent=(product>>23u)&0xffu;
        if(exponent<1u||exponent>253u)return false;}
    return sourceParticleFogAdd50LA(minimum,product,output);
}

// Source1606C8/1606D8/1606E0: fs=emitter Z/Y/X,ft=local Z/Y/X.
// Both operands may now be genuinely nonzero, with either sign, while
// remaining signed zero or finite normal E1..254. This uses the pinned
// scalar model's masked alignment and truncation, not host addition or a
// mathematical translation identity. Unsupported result words remain pending
// at this actual ADD; earlier reads, range stores and RNG effects survive.
inline bool sourceParticleFogPositionAdd50LA(std::uint32_t emitter,
        std::uint32_t local,std::uint32_t& output) noexcept {
    return sourceParticleFogAdd50LA(emitter,local,output);
}
} // namespace rr::game::detail
