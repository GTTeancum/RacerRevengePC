#pragma once
#include <cstdint>

namespace rr::game::detail {
// Value-only inference from the pinned scalar PS2Float model, not silicon
// equivalence or a general EE arithmetic backend. The left significand is
// exactly800000 at BOTH original15B16C and admitted15B174 operations.
// See NUMERIC-PROOF.md for the reduced bit15 correction circuit and bounds.
inline std::uint32_t sourceParticleMode2LeftPowerCorrection(
        std::uint32_t rightSignificand) noexcept {
    // Internal precondition:800000<=rightSignificand<=FFFFFF. Only the
    // original Booth partial products' low16 bits can affect correction15.
    // With left significand800000 those bits are0 or-negatePosition.
    std::uint32_t partial[8],negate[8];
    for(unsigned i=0;i<8u;++i){
        const auto selector=(i==0u?rightSignificand<<1u:
            rightSignificand>>(2u*i-1u))&7u;
        negate[i]=(selector>=4u&&selector<=6u)?
            (std::uint32_t{1}<<(2u*i)):0u;
        partial[i]=0u-negate[i];
    }
    struct Pair{std::uint32_t lo,hi;};
    const auto carry=[](std::uint32_t a,std::uint32_t b,std::uint32_t c)noexcept{
        const auto u=a^b;
        return Pair{u^c,((u&c)|(a&b))<<1u};
    };
    const auto t0=carry(partial[1],partial[2],partial[3]);
    auto t1=carry(partial[4]&~0x7ffu,partial[5]&~0xfffu,partial[6]);
    t1.hi|=negate[6]|(partial[5]&0x800u);
    partial[7]|=(partial[5]&0x400u)+negate[5];
    const auto t2=carry(partial[0],t0.lo,t0.hi);
    const auto t3=carry(partial[7],t1.lo,t1.hi);
    const auto t4=carry(t2.hi,t3.lo,t3.hi);
    const auto t5=carry(t2.lo,t4.lo,t4.hi);
    // The model clears both addends' bits0..14 before summing. Therefore
    // the resulting bit15 is their bit15 XOR. Its final negate add occurs
    // BEFORE that clearing and must not be omitted or moved afterward.
    return ((t5.lo^(t5.hi+negate[7]))>>15u)&1u;
}

inline bool sourceParticleMode2PositiveUnit(std::uint32_t unit) noexcept {
    if(unit==0u)return true;
    if(unit&0x80000000u)return false;
    const auto exponent=(unit>>23u)&0xffu;
    return exponent>=96u&&exponent<=126u;
}

inline bool sourceParticleMode2Width(std::uint32_t width) noexcept {
    if(width==0u)return true;
    if(width&0x807fffffu)return false;
    const auto exponent=(width>>23u)&0xffu;
    return exponent>=32u&&exponent<=254u;
}

// Original15B16C: fs=2^-31,ft=the actual CVT.S.W result. Admit precisely
// +0 or positive integral binary32 values1..(2^31-128), the image of the
// bounded low31 conversion. Preserve the model's possible one-ULP decrement;
// do not use the earlier Fog exponent-only scale helper for this new slice.
inline bool sourceParticleMode2Scale(std::uint32_t convertedBits,
        std::uint32_t& output) noexcept {
    if(convertedBits==0u){output=0u;return true;}
    if(convertedBits&0x80000000u)return false;
    const auto exponent=(convertedBits>>23u)&0xffu;
    if(exponent<127u||exponent>157u)return false;
    const auto significand=0x00800000u|(convertedBits&0x007fffffu);
    const auto integerExponent=exponent-127u;
    if(integerExponent<23u){
        const auto fractionalBits=23u-integerExponent;
        if(significand&((std::uint32_t{1}<<fractionalBits)-1u))return false;
    }
    const auto corrected=significand-sourceParticleMode2LeftPowerCorrection(significand);
    const auto result=((exponent-31u)<<23u)|(corrected&0x007fffffu);
    output=result;return true;
}

// Original15B170: maximum-minus-minimum. Literal+0 minimum only; maximum
// is literal+0 or2^k,-95<=k<=127. The model's denormal/zero SUB branch
// returns the maximum word unchanged. Negative-zero endpoints are refused.
inline bool sourceParticleMode2Difference(std::uint32_t maximum,
        std::uint32_t minimum,std::uint32_t& output) noexcept {
    if(minimum!=0u||!sourceParticleMode2Width(maximum))return false;
    output=maximum;return true;
}

// Original15B174: fs=WIDTH,ft=UNIT, not the reverse. The bounds keep every
// nonzero result normal with biased exponent1..253. Zero operands give+0.
inline bool sourceParticleMode2Product(std::uint32_t width,
        std::uint32_t unit,std::uint32_t& output) noexcept {
    if(!sourceParticleMode2Width(width)||!sourceParticleMode2PositiveUnit(unit))return false;
    if(width==0u||unit==0u){output=0u;return true;}
    const auto widthExponent=(width>>23u)&0xffu;
    const auto unitExponent=(unit>>23u)&0xffu;
    const auto significand=0x00800000u|(unit&0x007fffffu);
    const auto corrected=significand-sourceParticleMode2LeftPowerCorrection(significand);
    const auto exponent=widthExponent+unitExponent-127u; //1..253.
    const auto result=(exponent<<23u)|(corrected&0x007fffffu);
    output=result;return true;
}

// Original15B178: fs=MINIMUM,ft=PRODUCT. The model's zero/normal ADD branch
// returns the positive normal product, or+0 for both+0. No normal ADD is
// inferred. The admitted product range excludes exponent254 and255.
inline bool sourceParticleMode2Sum(std::uint32_t minimum,
        std::uint32_t product,std::uint32_t& output) noexcept {
    if(minimum!=0u||(product&0x80000000u))return false;
    if(product!=0u){const auto exponent=(product>>23u)&0xffu;
        if(exponent<1u||exponent>253u)return false;}
    output=product;return true;
}
} // namespace rr::game::detail
