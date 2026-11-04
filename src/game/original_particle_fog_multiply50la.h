#pragma once
#include <cstdint>

namespace rr::game::detail {
// Bounded result-word inference from pinned scalar PS2Float Mul/DoMul with
// CHECK_FPU_SOFT_MUL selected. Not silicon equivalence, FCR31 emulation, VU
// arithmetic, source custody, or a general EE backend. See NUMERIC-PROOF.md.
// Both inputs must be literal+0 or positive normal E1..254. Nonzero results
// outside E1..254 refuse instead of flushing, saturating, or admitting E255.
// Preserve source fs/ft order: the correction circuit is operand-sensitive.
// Every false return leaves output unchanged; no host floating math or RNG.
inline bool sourceParticleFogMultiply50LA(std::uint32_t left,
        std::uint32_t right,std::uint32_t& output) noexcept {
    const auto leftExponent=(left>>23u)&0xffu;
    const auto rightExponent=(right>>23u)&0xffu;
    if((left|right)&0x80000000u)return false;
    if((left!=0u&&(leftExponent<1u||leftExponent>254u))||
       (right!=0u&&(rightExponent<1u||rightExponent>254u)))return false;
    // Validate BOTH inputs before this branch, including when one is zero.
    if(left==0u||right==0u){output=0u;return true;}

    const auto a=0x00800000u|(left&0x007fffffu);
    const auto b=0x00800000u|(right&0x007fffffu);
    const auto full=static_cast<std::uint64_t>(a)*static_cast<std::uint64_t>(b);
    std::uint32_t partial[8]{},negate[8]{};
    for(unsigned i=0;i<8u;++i){
        const auto shift=2u*i; //0..14; the alternate right shift is1..13.
        const auto selector=(i==0u?b<<1u:b>>(shift-1u))&7u;
        auto shifted=(a&0xffffu)<<shift;
        if(selector==3u||selector==4u)shifted+=shifted;
        const auto position=std::uint32_t{1}<<shift;
        // For an aligned q, q XOR(-position) == -q-position modulo2^32.
        // Only bits0..15 can reach correction15; discarded high bits cannot
        // feed any later bitwise operation, addition, or left shift downward.
        if(selector>=4u&&selector<=6u){
            partial[i]=(0u-shifted-position)&0xffffu;
            negate[i]=position;
        }else if(selector>=1u&&selector<=6u)partial[i]=shifted&0xffffu;
    }
    struct Pair{std::uint32_t lo,hi;};
    const auto carry=[](std::uint32_t x,std::uint32_t y,std::uint32_t z)noexcept{
        const auto u=x^y;
        return Pair{(u^z)&0xffffu,(((u&z)|(x&y))<<1u)&0xffffu};
    };
    const auto t0=carry(partial[1],partial[2],partial[3]);
    auto t1=carry(partial[4]&~0x7ffu,partial[5]&~0xfffu,partial[6]);
    t1.hi|=negate[6]|(partial[5]&0x800u);
    partial[7]|=(partial[5]&0x400u)+negate[5];
    const auto t2=carry(partial[0],t0.lo,t0.hi);
    const auto t3=carry(partial[7],t1.lo,t1.hi);
    const auto t4=carry(t2.hi,t3.lo,t3.hi);
    const auto t5=carry(t2.lo,t4.lo,t4.hi);
    // Original negation addition precedes clearing bits0..14. Once those
    // bits are cleared in both addends, sum bit15 is their bit15 XOR.
    // Unlike the LEFT-power specialization, full.bit15 need not be zero.
    const auto correction=(t5.lo^(t5.hi+negate[7])^
        static_cast<std::uint32_t>(full))&0x8000u;
    auto significand=static_cast<std::uint32_t>(
        (full-static_cast<std::uint64_t>(correction))>>23u);
    auto exponent=static_cast<std::int32_t>(leftExponent)+
        static_cast<std::int32_t>(rightExponent)-127;
    if(significand>0x00ffffffu){significand>>=1u;++exponent;}
    if(exponent<1||exponent>254)return false;
    output=(static_cast<std::uint32_t>(exponent)<<23u)|
        (significand&0x007fffffu);
    return true;
}
} // namespace rr::game::detail
