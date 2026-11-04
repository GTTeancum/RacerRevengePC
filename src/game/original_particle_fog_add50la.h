#pragma once
#include <cstdint>

namespace rr::game::detail {
// Bounded raw-result inference from pinned PS2Float Add/Sub/DoAdd with
// CHECK_FPU_SOFT_ADDSUB selected. Not hardware equivalence, flag emulation,
// source custody, or a general EE backend. See NUMERIC-PROOF.md.
// Inputs: either signed zero or either sign of a normal with exponent1..254.
// Nonzero arithmetic results outside exponent1..254 refuse, without flushing
// or saturating them. Every false return leaves output unchanged.
inline bool sourceParticleFogAdd50LA(std::uint32_t left,
        std::uint32_t right,std::uint32_t& output) noexcept {
    const auto leftMagnitude=left&0x7fffffffu;
    const auto rightMagnitude=right&0x7fffffffu;
    auto leftExponent=(left>>23u)&0xffu;
    auto rightExponent=(right>>23u)&0xffu;
    if((leftMagnitude!=0u&&(leftExponent<1u||leftExponent>254u))||
       (rightMagnitude!=0u&&(rightExponent<1u||rightExponent>254u)))return false;

    // The model's explicit exponent-zero path: only -0+-0 produces -0.
    // A normal has greater CompareOperands magnitude than either signed zero.
    if(leftMagnitude==0u&&rightMagnitude==0u){
        output=(left&right)&0x80000000u;return true;
    }
    if(leftMagnitude==0u){output=right;return true;}
    if(rightMagnitude==0u){output=left;return true;}

    // Match DoAdd's exponent ordering, preserving each word's effective sign.
    if(leftExponent<rightExponent){
        const auto word=left;left=right;right=word;
        const auto exponent=leftExponent;leftExponent=rightExponent;rightExponent=exponent;
    }
    const auto difference=leftExponent-rightExponent;
    if(difference>=25u){output=left;return true;}

    const auto largeSignificand=(left&0x007fffffu)|0x00800000u;
    auto smallSignificand=(right&0x007fffffu)|0x00800000u;
    if(difference!=0u)smallSignificand&=0xffffffffu<<(difference-1u);
    const auto large=largeSignificand<<6u;
    const auto small=(smallSignificand<<6u)>>difference;
    const auto largeSign=left&0x80000000u;
    const auto smallSign=right&0x80000000u;
    std::uint32_t magnitude=0u;
    std::uint32_t sign=largeSign;
    if(largeSign==smallSign)magnitude=large+small;
    else if(large>=small)magnitude=large-small;
    else{magnitude=small-large;sign=smallSign;}
    if(magnitude==0u){output=0u;return true;}

    // Nonzero magnitude is a multiple of32 and is below2^31: highest5..30.
    std::uint32_t highest=0u;
    for(auto remaining=magnitude;remaining>1u;remaining>>=1u)++highest;
    const auto exponent=static_cast<std::int32_t>(leftExponent)+
        static_cast<std::int32_t>(highest)-29;
    if(exponent<1||exponent>254)return false;
    const auto significand=highest>23u?magnitude>>(highest-23u):
        magnitude<<(23u-highest);
    output=sign|(static_cast<std::uint32_t>(exponent)<<23u)|
        (significand&0x007fffffu);
    return true;
}

// On the admitted zero/normal domain, Sub(a,b) equals Add(a,Negate(b))
// including all four signed-zero pairs and either zero/normal orientation.
// Sign flipping changes neither input admission nor false-output atomicity.
inline bool sourceParticleFogSubtract50LA(std::uint32_t left,
        std::uint32_t right,std::uint32_t& output) noexcept {
    return sourceParticleFogAdd50LA(left,right^0x80000000u,output);
}
} // namespace rr::game::detail
