#pragma once
#include "original_particle_fog_lifetime50la.h"
#include "original_particle_fog_multiply50la.h"
#include <cstdint>
namespace rr::game::detail {
//Result-word inference for the authenticated small positive constructor words,
//not a host float conversion, arbitrary CVT.S.W or source operand custody.
//Every admitted signed32 integer is exactly representable. False preserves out.
inline bool sourceWaterSmallWordFloat50LA(std::uint32_t word,std::uint32_t& out)noexcept {
    if(!word||word>258u)return false;
    unsigned exponent=0;for(auto remaining=word;remaining>1u;remaining>>=1u)++exponent;
    const auto result=((127u+exponent)<<23u)|((word<<(23u-exponent))&0x007fffffu);
    out=result;return true;
}
//Bounded configured PS2Float soft-DIV result, not silicon/IEEE equivalence or
//live FCR31 emulation. Numerators +/-2 have the SAME800000 significand as the
//existing +1 helper. Quotient/normalization are unchanged; only exponent+1
//and sign differ. Calling that native value helper is NOT a source1/x event.
//Admit precisely positive integral denominators86..255, covering the actual
//factory C-1/C-2 operands. No zero, denormal, exceptional or saturation path.
//False leaves BOTH outputs unchanged. false assertedD means no D/SD assertion,
//not clearing earlier flags. The pinned normal DIV clears O only, preserves U
//and all other cause/sticky bits; callers retain that local effect separately.
inline bool sourceWaterTwoDivision50LA(std::uint32_t numerator,std::uint32_t denominator,
        std::uint32_t& out,bool& assertedD)noexcept {
    if(numerator!=0x40000000u&&numerator!=0xc0000000u)return false;
    const auto exponent=(denominator>>23u)&0xffu;
    if((denominator&0x80000000u)||exponent<133u||exponent>134u)return false;
    const auto shift=150u-exponent; //16 or17; exact integer decoding.
    const auto significand=0x00800000u|(denominator&0x007fffffu);
    if(significand&((std::uint32_t{1}<<shift)-1u))return false;
    const auto word=significand>>shift;if(word<86u||word>255u)return false;
    std::uint32_t reciprocal=0;bool helperD=false;
    if(!sourceParticleFogReciprocal50LA(denominator,reciprocal,helperD)||helperD)return false;
    const auto resultExponent=(reciprocal>>23u)&0xffu;
    if((reciprocal&0x80000000u)||resultExponent<119u||resultExponent>121u)return false;
    out=(numerator&0x80000000u)|(reciprocal+0x00800000u);assertedD=false;return true;
}
//Actual2B8550 ratio only: converted(17*B+2)/(17*B), B=2,3,4.
//These words describe the helper's value domain, not caller/table authority.
//Unlike +/-2, this requires the actual numerator significand. Preserve the
//same pinned25-step quotient circuit, including the old-digit selection and
//low24 OR. No reciprocal multiplication, host division or supplied quotient.
//Both outputs remain unchanged on refusal. Same local normal-DIV flags as above.
inline bool sourceWaterBandRatio50LA(std::uint32_t numerator,std::uint32_t denominator,
        std::uint32_t& out,bool& assertedD)noexcept {
    if(!((numerator==0x42100000u&&denominator==0x42080000u)||
         (numerator==0x42540000u&&denominator==0x424c0000u)||
         (numerator==0x428c0000u&&denominator==0x42880000u)))return false;
    const auto divisor=((denominator&0x007fffffu)|0x00800000u)<<2u;
    std::uint32_t sum=((numerator&0x007fffffu)|0x00800000u)<<2u;
    std::uint32_t carry=0,quotient=0;int digit=1;
    for(unsigned step=0;step<25u;++step){
        quotient<<=1u;if(digit>0)++quotient;else if(digit<0)--quotient;
        const auto add=digit>0?~divisor:(digit<0?divisor:0u);
        if(digit>0)++carry;
        const auto exclusive=sum^carry;
        const auto nextSum=exclusive^add;
        const auto nextCarry=((sum&carry)|(exclusive&add))<<1u;
        const auto selectSum=digit?nextSum:sum;
        const auto selectCarry=digit?nextCarry:carry;
        const auto selected=((selectSum&0xff000000u)+selectCarry)|(selectSum&0x00ffffffu);
        digit=(selected>=0x00800000u&&selected<0x80000000u)?1:
            ((selected>=0x80000000u&&selected<0xff000000u)?-1:0);
        sum=nextSum<<1u;carry=nextCarry<<1u;
    }
    //Actual pairs have equal positive normal exponents. Result exponent126
    //or127 is normal in either normalization branch; all local flags stay false.
    std::uint32_t exponent=126u;
    if(quotient>=0x01000000u){++exponent;quotient>>=1u;}
    out=(quotient&0x007fffffu)|(exponent<<23u);assertedD=false;return true;
}
} //namespace rr::game::detail
