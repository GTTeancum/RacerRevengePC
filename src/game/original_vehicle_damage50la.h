#pragma once
#include "effect_dispatch50be.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace rr::game {
// Native source continuations only. None of these functions constructs a
// wreck, activates a piece, changes vehicle40/CD4, or advances a frame phase.
// In particular a selected virtual call is not a completed destruction event.
enum class OriginalVehicleDamageStatus50LA : std::uint8_t {
    complete,destructionRequired,context,arithmetic,binding,unsupportedDuration
};
struct OriginalVehicleDamageResult50LA {
    OriginalVehicleDamageStatus50LA status=OriginalVehicleDamageStatus50LA::context;
    std::uint32_t sourcePC=0,vtableSlot=0,originalTarget=0;
    bool timerWritten=false;
    std::size_t stopCalls=0,deferredStops=0;
    bool okay()const noexcept{return status==OriginalVehicleDamageStatus50LA::complete;}
};
namespace vehicle_damage_detail50LA {
inline std::uint32_t bits(float value) noexcept {
    std::uint32_t word;std::memcpy(&word,&value,4);return word;
}
inline float value(std::uint32_t word) noexcept {
    float result;std::memcpy(&result,&word,4);return result;
}
inline bool scalar(float value) noexcept {
    const auto word=bits(value),exponent=word&0x7f800000u;
    return exponent!=0x7f800000u&&(exponent||(word&0x7fffffu)==0u);
}
inline OriginalVehicleDamageResult50LA result(OriginalVehicleDamageStatus50LA status,
    std::uint32_t pc,bool timerWritten=false) noexcept {
    OriginalVehicleDamageResult50LA out{};out.status=status;out.sourcePC=pc;
    out.timerWritten=timerWritten;return out;
}
}

// Original2289BC..228A90, reached AFTER2288B4/C8/D8/E4 with manager58 bit1
// set, vehicle40=1/2 and vehicle3C=0. The timer is the SAME canonical vehicle4C
// written to2 by2273CC/227A9C; this function never seeds or resets it.
//
// Source order matters: old4C<=0 skips every following read. Otherwise4C is
// decremented BEFORE impact/threshold tests. The positive-timer prefix remains
// committed at a later arithmetic boundary or actual virtual call. Do not
// replay this function while that call is pending. A destructionRequired
// result must run the real2261B0/226750 owner; it cannot authorize a state
// assignment, retirement, a checkpoint return, or an AfterImpactBlock receipt.
//
// Consumed scalars are finite normal/signed-zero, FE_TONEAREST with separate
// binary32 arithmetic. factor is the actual nonnegative dispatched frame
// factor. Input floats must not alias mutable4C. No shared RNG is consumed.
inline OriginalVehicleDamageResult50LA advanceOriginalDetachedDamage50LA(
    std::uint32_t vehicle40,std::uint32_t vehicle3C,float factor,
    float impactBA8,float threshold50,float& remaining4C) noexcept {
    using S=OriginalVehicleDamageStatus50LA;namespace D=vehicle_damage_detail50LA;
    if((vehicle40!=1u&&vehicle40!=2u)||vehicle3C)
        return D::result(S::context,0x2289bcu);
    if(std::fegetround()!=FE_TONEAREST||!D::scalar(remaining4C))
        return D::result(S::arithmetic,0x2289dcu);
    if(remaining4C<=0.f)return D::result(S::complete,0x228b40u);
    if(!D::scalar(factor)||factor<0.f)return D::result(S::arithmetic,0x2289f8u);
    volatile float next=remaining4C-factor;
    if(!D::scalar(next))return D::result(S::arithmetic,0x2289f8u);
    remaining4C=next; //228A00, before either subsequent handler selection.
    if(!D::scalar(impactBA8))return D::result(S::arithmetic,0x228a08u,true);
    const float magnitude=D::value(D::bits(impactBA8)&0x7fffffffu);
    if(!D::scalar(threshold50))return D::result(S::arithmetic,0x228a18u,true);
    if(!(magnitude<=threshold50)){
        auto out=D::result(S::destructionRequired,0x228a30u,true);
        out.vtableSlot=0x88u;out.originalTarget=0x2261b0u;return out;
    }
    volatile float half=threshold50/2.f; //228A4C, no multiply-by-reciprocal rewrite.
    if(!D::scalar(half))return D::result(S::arithmetic,0x228a4cu,true);
    if(!(magnitude<=half)||remaining4C<=0.f){
        auto out=D::result(S::destructionRequired,0x228a84u,true);
        out.vtableSlot=0x8cu;out.originalTarget=0x226750u;return out;
    }
    return D::result(S::complete,0x228b40u,true);
}

// Original228BC4..228BE0. Only a reached state4 frame decrements canonical268;
// there is no zero clamp, no explosion production and no retirement write.
// Invoke after the real228BBC undertrack branch has returned, not as a
// substitute for that branch or the preceding destruction handler.
inline OriginalVehicleDamageResult50LA advanceOriginalExplosionTimer50LA(
    std::uint32_t vehicle40,float factor,float& remaining268) noexcept {
    using S=OriginalVehicleDamageStatus50LA;namespace D=vehicle_damage_detail50LA;
    if(vehicle40!=4u)return D::result(S::context,0x228bc4u);
    if(std::fegetround()!=FE_TONEAREST||!D::scalar(remaining268)||
       !D::scalar(factor)||factor<0.f)return D::result(S::arithmetic,0x228bd4u);
    volatile float next=remaining268-factor;
    if(!D::scalar(next))return D::result(S::arithmetic,0x228bd8u);
    remaining268=next;return D::result(S::complete,0x228be0u,true);
}

// Source arrays exist even when one of their four entries is genuinely null.
// Every nonnull pointer borrows the actual source50BE effect control owner.
// These are not IDs or copies that can drift from the effect/particle runtime.
struct OriginalDestroyedEffectBindings50LA {
    std::array<EffectOwner50BE*,4> owners800{},owners804{},owners80C{};
};
// Original228390..22861C, the actual228764/228770 state3/4 frame branch.
// Walk800,804,80C in that order and call Stop84 only if current1D4 bit1 is set.
// A deferred stop is a genuine returned virtual84 call, not an invented stop.
// No timer820,808/81C, vehicle state, gameplay phase or particle pool is reset.
// Source-ordered commits are retained on failure; retrying from the beginning
// is not supported. Bindings, result storage and every owner must be disjoint.
// Repeated owner pointers are legal: a later slot rereads its current flag.
inline OriginalVehicleDamageResult50LA stopOriginalDestroyedEffects50LA(
    std::uint32_t vehicle40,const OriginalDestroyedEffectBindings50LA& bindings) noexcept {
    using S=OriginalVehicleDamageStatus50LA;namespace D=vehicle_damage_detail50LA;
    if(vehicle40!=3u&&vehicle40!=4u)return D::result(S::context,0x228390u);
    constexpr std::uint32_t pc[3][4]={
        {0x2283ccu,0x228400u,0x228434u,0x228468u},
        {0x22849cu,0x2284d0u,0x228504u,0x228538u},
        {0x22856cu,0x2285a0u,0x2285d4u,0x228608u}};
    const std::array<EffectOwner50BE*,4>* tables[]={
        &bindings.owners800,&bindings.owners804,&bindings.owners80C};
    auto out=D::result(S::complete,0x228618u);
    for(unsigned table=0;table<3;++table)for(unsigned slot=0;slot<4;++slot){
        auto* owner=(*tables[table])[slot];
        if(!owner||!(owner->state.flags1D4&2u))continue;
        if(!owner->id||static_cast<std::uint32_t>(owner->profile)>3u){
            out.status=S::binding;out.sourcePC=pc[table][slot];return out;
        }
        if(owner->profile==EffectProfile50BE::Deferred15E800){
            const auto stopped=stopDeferredEffect50BD(owner->state,owner->mode1D8,owner->duration1E4);
            if(stopped==EffectStopResult50BD::UnsupportedDuration){
                out.status=S::unsupportedDuration;out.sourcePC=pc[table][slot];return out;
            }
            if(stopped==EffectStopResult50BD::Deferred)++out.deferredStops;
        }else stopEffect50BC(owner->state);
        ++out.stopCalls;
    }
    return out;
}
} // namespace rr::game
