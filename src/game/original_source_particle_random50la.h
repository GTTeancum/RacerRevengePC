#pragma once
#include <cstdint>

namespace rr::game::detail {
// Narrow source1605EC/160608/160624 CVT.S.W domain: the actual308030
// return is nonnegative low31. EE conversion rounds toward zero. Construct
// its binary32 word directly, dropping only integer bits below24 significant
// bits. This follows the existing Rider/Watto source-native conversion; it is
// not a host-nearest cast, a new random generator, or a general EE interpreter.
// On refusal, output is unchanged. Zero produces source positive zero.
inline bool sourceParticleTruncateInteger50LA(std::uint32_t low31,
    std::uint32_t& output) noexcept {
    if(low31>0x7fffffffu)return false;
    if(low31==0){output=0;return true;}
    unsigned exponent=0;
    for(auto remaining=low31;remaining>1u;remaining>>=1u)++exponent;
    // exponent is0..30. Both shifts are0..23 or1..7; no sign extension,
    // shift-by-width, signed overflow, or host floating arithmetic occurs.
    const auto significand=exponent<=23u
        ?low31<<(23u-exponent):low31>>(exponent-23u);
    const auto result=(static_cast<std::uint32_t>(exponent+127u)<<23u)|(significand&0x007fffffu);
    output=result;return true;
}

// Source1609F8 CVT.S.W is explicitly a32-bit SIGNED conversion (Sony EE
// Instruction Set Manual6.0,p355). Admit only |signedWord|<=2^24, whose
// binary32 magnitude is exact with no discarded nonzero significand bits.
// This narrow deduction uses the specified conversion and representation,
// not a measured-silicon table or general rounded signed arithmetic. Raw32
// magnitude formation avoids signed overflow; zero always becomes+0.
// Failure leaves output unchanged, including INT_MIN and wider spans.
inline bool sourceParticleExactSignedSpan50LA(std::uint32_t signedWord,
    std::uint32_t& output) noexcept {
    const auto sign=signedWord&0x80000000u;
    const auto magnitude=sign?0u-signedWord:signedWord;
    if(magnitude>0x01000000u)return false;
    std::uint32_t bits=0;
    if(!sourceParticleTruncateInteger50LA(magnitude,bits))return false;
    output=bits|sign;return true;
}

// Exact source1605FC/160618/160644 multiplication by bits30000000
// (2^-31). Input must be the positive, exactly integral binary32 result of
// the bounded conversion above: +0 or a value in1..(2^31-128). Validate that
// domain before adjusting the exponent; no rounding or underflow is possible.
// Negative zero, fractions, subnormals, infinities, NaNs and2^31 are refused.
// On refusal, output is unchanged. This does not accept authored range math.
inline bool sourceParticleScaleRandom50LA(std::uint32_t convertedBits,
    std::uint32_t& output) noexcept {
    if(convertedBits==0){output=0;return true;}
    if(convertedBits&0x80000000u)return false;
    const auto biasedExponent=(convertedBits>>23u)&0xffu;
    if(biasedExponent<127u||biasedExponent>157u)return false;
    const auto exponent=biasedExponent-127u; //0..30.
    const auto significand=0x00800000u|(convertedBits&0x007fffffu);
    if(exponent<23u){
        const auto fractionalBits=23u-exponent; //1..23.
        if(significand&((std::uint32_t{1}<<fractionalBits)-1u))return false;
    }
    // Biased output exponent96..126: finite normal, exact power-of-two
    // rescaling. Fraction bits are unchanged. Largest result is3F7FFFFF.
    const auto result=((biasedExponent-31u)<<23u)|(convertedBits&0x007fffffu);
    output=result;return true;
}
// Deliberately tiny160664/160680/16069C domain: both authored endpoints
// must be literal+0. EE User Manual6.0 section8.6 explicitly gives+0-+0=+0.
// Negative-zero endpoints and every nonzero range remain pending; this is not
// general subtraction, cancellation, interpolation or EE arithmetic emulation.
inline bool sourceParticleZeroRangeDifference50LA(std::uint32_t maximum,
    std::uint32_t minimum,std::uint32_t& output) noexcept {
    if(maximum!=0||minimum!=0)return false;
    output=0;return true;
}
// Source160668/160684/1606A0: the actual scaled RNG unit is+0 or positive
// normal in[2^-31,1), and the admitted width is literal+0. MUL.S is defined
// to return the product (EE Instruction Manual6.0,p372). Its exact product
// is+0, with no discarded nonzero significand or range exception. This is a
// narrow deduction from that source operation, not a separately enumerated
// normal-times-zero table or a claim of general EE low-bit equivalence.
inline bool sourceParticleUnitTimesPositiveZero50LA(std::uint32_t unit,
    std::uint32_t width,std::uint32_t& output) noexcept {
    if(width!=0||(unit&0x80000000u))return false;
    if(unit!=0){const auto exponent=(unit>>23u)&0xffu;
        if(exponent<96u||exponent>126u)return false;}
    output=0;return true;
}
// Only BOTH operands zero. Sony's explicit section8.6 signed-zero ADD table
// gives negative zero only for(-0)+(-0). No normal-plus-zero identity, host
// floating environment, general exact ADD or VU semantics is assumed here.
inline bool sourceParticleBothZeroAdd50LA(std::uint32_t left,
    std::uint32_t right,std::uint32_t& output) noexcept {
    if((left&0x7fffffffu)||(right&0x7fffffffu))return false;
    output=(left&right)&0x80000000u;return true;
}
// Sony EE User Manual6.0,section8.6,p163 explicitly enumerates all four
// zero*zero sign combinations. Admit ONLY those cases at160A18, not a
// mathematically exact nonzero product or inferred power-of-two exception to
// the documented low-bit caveat. The Fog span/unit producers yield+0 here.
inline bool sourceParticleBothZeroMultiply50LA(std::uint32_t left,
    std::uint32_t right,std::uint32_t& output) noexcept {
    if((left&0x7fffffffu)||(right&0x7fffffffu))return false;
    output=(left^right)&0x80000000u;return true;
}
// CVT.W.S converts single to signed integer toward zero (Sony Instruction
// Set Manual6.0,p356). Either signed zero has integer result0. No nonzero,
// exceptional, saturating or host floating conversion is admitted here.
inline bool sourceParticleZeroToWord50LA(std::uint32_t bits,
    std::uint32_t& output) noexcept {
    if(bits&0x7fffffffu)return false;
    output=0;return true;
}
// Exact exceptional1608DC domain, not general division. Its authenticated
// numerator is+1 and actual record30 denominator must be literal+0 or-0.
// Sony EE User Manual6.0,section8.6,p163 explicitly gives positive nonzero
// fs divided by+0/-0 as7FFFFFFF/FFFFFFFF, with D/SD asserted. These are EE
// saturation words; never evaluate them as host IEEE floats or normalize
// their payload. This helper records only the result word, not whole FCR31
// or sticky history. The caller retains comparison C separately because DIV
// does not replace it. On refusal output is unchanged.
inline bool sourceParticleDivideOneBySignedZero50LA(std::uint32_t denominator,
    std::uint32_t& output) noexcept {
    if(denominator&0x7fffffffu)return false;
    output=denominator|0x7fffffffu;return true;
}
} // namespace rr::game::detail
