#pragma once
#include "native_directional_normalize106ea0_proposal.h"
// Strict-native EE-FPU attenuation adaptation for the authentic second RA
// Directional only. No PS2 rounding/ACC/exception parity or execution claim.
namespace rr::game::directional_native_proposal {
inline Status range147268(const Words& attenuation, std::uint32_t sourceLimit,
                         std::uint32_t& output) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return Status::nativeEnvironment;
    //Actual277674 default or authored RA Spotlight tuple, not a general attenuation policy.
    if((attenuation!=Words{{0x3cf5c28fu,0x3b03126fu,0x3a83126fu,0x3f800000u}}&&
        attenuation!=Words{{0x3f800000u,0x358637bdu,0x360637bdu,0x3f800000u}})||
       !detail::normalOrZeroWord(sourceLimit))return Status::unsupportedDomain;
    const float a=detail::fromWord(attenuation[0]),b=detail::fromWord(attenuation[1]),c=detail::fromWord(attenuation[2]);
    const float limit=detail::fromWord(sourceLimit);
    if(!(limit>0.f))return Status::unsupportedDomain;
    float acc,fourC;
    //147280 MULA.S ACC=b*b;14728C MUL.S=4*c.
    if(!detail::product(b,b,acc)||!detail::product(4.f,c,fourC))return Status::unsupportedDomain;
    volatile float aMinusTwo=a-2.f; //147294 SUB.S; original constant2, not an epsilon.
    if(!detail::normalOrZero(aMinusTwo))return Status::unsupportedDomain;
    float product;
    if(!detail::product(fourC,aMinusTwo,product))return Status::unsupportedDomain;
    //147298 MSUB.S native policy: separate rounded product, then subtract.
    //Not FMA, not a claim about original EE internal ACC precision.
    volatile float discriminant=acc-product;
    if(!detail::normalOrZero(discriminant)||!(discriminant>0.f))return Status::unsupportedDomain;
    volatile float root=std::sqrt(discriminant); //14729C; materialized1472A0.
    if(!detail::normalOrZero(root)||!(root>0.f))return Status::unsupportedDomain;
    volatile float negativeB=-b; //1472B0 NEG.S.
    float denominator;
    if(!detail::product(2.f,c,denominator)||!(denominator>0.f))return Status::unsupportedDomain;
    const float numerator=detail::add(negativeB,root); //1472B8, after2*c.
    if(!detail::normalOrZero(numerator))return Status::unsupportedDomain;
    volatile float range=numerator/denominator; //1472BC DIV.S.
    if(!detail::normalOrZero(range)||range<0.f)return Status::unsupportedDomain;
    if(range==0.f&&numerator!=0.f)return Status::unsupportedDomain; //No native flush-to-zero admission.
    //1472C8 zero branch and1472F8 source C.LE.S clamp. Caller supplies the
    //existing exact source-software-double sqrt/narrow limit, not host sqrt.
    if(range==0.f||!(range<=limit))range=limit;
    output=detail::word(range);return Status::complete;
}
}
