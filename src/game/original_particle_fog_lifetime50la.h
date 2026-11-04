#pragma once
#include <cstdint>

namespace rr::game::detail {
// Bounded numerator-one, raw-result specialization of pinned PS2Float::Div
// with CHECK_FPU_SOFT_DIVSQRT selected. The 25 quotient steps, including the
// low24 OR in quotientSelect and its prior-state zero-digit case, are retained.
// This is configured-model result equivalence, not hardware/IEEE division,
// complete FCR31 emulation, or proof of a source operand's ownership.
//
// Signed zeros keep the separate Sony-documented legacy saturation mapping.
// Otherwise admit only positive normal operands (exponent1..254), and refuse
// a model result whose exponent is outside1..254 instead of flushing it.
// False leaves BOTH outputs unchanged. false assertedDivideByZero on success
// means this DIV did not assert D/SD; it does not clear earlier cause/sticky
// state. Source comparison C remains separately owned. See LIFETIME-PROOF.md.
inline bool sourceParticleFogReciprocal50LA(std::uint32_t denominator,
        std::uint32_t& output,bool& assertedDivideByZero) noexcept {
    if((denominator&0x7fffffffu)==0u){
        output=denominator|0x7fffffffu;
        assertedDivideByZero=true;
        return true;
    }
    const auto exponent=(denominator>>23u)&0xffu;
    if((denominator&0x80000000u)||exponent<1u||exponent>254u)return false;

    // Actual numerator3F800000 has significand800000. Every state word and
    // carry operation below uses uint32 modulo2^32, as in the pinned source.
    const std::uint32_t divisor=((denominator&0x007fffffu)|0x00800000u)<<2u;
    std::uint32_t sum=0x02000000u,carry=0u,quotient=0u;
    int digit=1; //Only -1,0,+1, never a signed word representation.
    for(unsigned step=0;step<25u;++step){
        quotient<<=1u;
        if(digit>0)++quotient;
        else if(digit<0)--quotient;
        const std::uint32_t add=digit>0?~divisor:(digit<0?divisor:0u);
        if(digit>0)++carry;
        const std::uint32_t exclusive=sum^carry;
        const std::uint32_t nextSum=exclusive^add;
        const std::uint32_t nextCarry=((sum&carry)|(exclusive&add))<<1u;

        // Select from CSA when the OLD digit is nonzero; otherwise select
        // from the prior pair, not the new CSA pair. Preserve the source OR.
        const auto selectSum=digit?nextSum:sum;
        const auto selectCarry=digit?nextCarry:carry;
        const std::uint32_t test=((selectSum&0xff000000u)+selectCarry)|
            (selectSum&0x00ffffffu);
        // Raw equivalents of signed test>=2^23 and test<-2^24. Avoid a
        // uint32-to-int32 conversion or a shift of a negative signed value.
        digit=(test>=0x00800000u&&test<0x80000000u)?1:
            ((test>=0x80000000u&&test<0xff000000u)?-1:0);
        sum=nextSum<<1u;
        carry=nextCarry<<1u;
    }

    // Pinned exponent(a)-exponent(b)+126, with exponent(a)=127.
    // The explicit small signed subtraction is in[-1,252], with no overflow.
    int resultExponent=253-static_cast<int>(exponent);
    if(quotient>=0x01000000u){++resultExponent;quotient>>=1u;}
    if(resultExponent<1||resultExponent>254)return false;
    const std::uint32_t result=(quotient&0x007fffffu)|
        (static_cast<std::uint32_t>(resultExponent)<<23u);
    output=result;
    assertedDivideByZero=false;
    return true;
}

// Shape only, never denominator custody or proof that an operation happened.
// A normal successful result has exponent at most253 (253-E plus at most1,
// E>=1). Every accepted word still needs exact reciprocal recomputation from
// the actual retained record30 read before it can authenticate a source store.
inline bool sourceParticleFogReciprocalWord50LA(std::uint32_t word) noexcept {
    if(word==0x7fffffffu||word==0xffffffffu)return true;
    const auto exponent=(word>>23u)&0xffu;
    return !(word&0x80000000u)&&exponent>=1u&&exponent<=253u;
}
} // namespace rr::game::detail
