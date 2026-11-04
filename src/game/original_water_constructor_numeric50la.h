#pragma once
#include "original_particle_fog_lifetime50la.h"
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
} //namespace rr::game::detail
