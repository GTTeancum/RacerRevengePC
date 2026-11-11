#pragma once
// Bounded original player-engine damage modulation, recovered from retained
// SLUS words. This owns emitter-local numbers only, not a fictional D40 object.
// No samples, events, random generator, mixer voices or output clock are added.
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstdint>

namespace rr::game {
struct EngineDamageState50IA {
    // Constructor19EBCC/19EC88..19EC9C. Only low flags4A4 bits belong here;
    // warning transitions continue owning their separate existing high bits.
    float cooldown474=0.f,scale484=1.f,target490=1.f;
    float duration494=0.f,remaining498=0.f,velocity49C=0.f,acceleration4A0=0.f;
    std::uint8_t flags4A4=0;
};
struct EngineDamageInput50IA {
    float factor=0.f,damage7D0=0.f,damage7D4=0.f,damage7DC=0.f;
};
enum class EngineDamageStatus50IA : std::uint8_t {Complete,Input,RandomRange};
struct EngineDamageReceipt50IA {
    EngineDamageStatus50IA status=EngineDamageStatus50IA::Input;
    std::uint32_t randomDraws=0;
    bool damageDipStarted=false,returnStarted=false,dipFinished=false;
    bool okay()const noexcept{return status==EngineDamageStatus50IA::Complete;}
};
namespace engine_damage50IA_detail {
inline float add(float a,float b)noexcept{volatile float x=a+b;return x;}
inline float sub(float a,float b)noexcept{volatile float x=a-b;return x;}
inline float mul(float a,float b)noexcept{volatile float x=a*b;return x;}
inline float div(float a,float b)noexcept{volatile float x=a/b;return x;}
inline bool finite(float x)noexcept{return std::isfinite(x);}
inline bool valid(const EngineDamageState50IA&s)noexcept {
    return finite(s.cooldown474)&&s.cooldown474>=0.f&&finite(s.scale484)&&s.scale484>0.f&&
        finite(s.target490)&&s.target490>0.f&&finite(s.duration494)&&s.duration494>=0.f&&
        finite(s.remaining498)&&finite(s.velocity49C)&&finite(s.acceleration4A0)&&
        (s.flags4A4==0u||s.flags4A4==2u||s.flags4A4==3u)&&
        (!(s.flags4A4&2u)||s.duration494>0.f);
}
// Original shared308030 owner. Preserve raw32; mask only its returned value.
// Native callers already own the initialized RNG used by physics/materials/
// speech. Never seed or replace it with a private audio generator.
// The R5900 FPU rounds CVT.S.W and MUL.S toward zero, so the source's
// span*(2^-31*float(low31)) is always below span. Host nearest-even rounds 64
// of the 2^31 low31 values up to span (about 1 draw in 33 million), which
// breached the source assertion and stopped a GA race. This draw therefore
// narrows both steps toward zero, as the original hardware does. Every step
// is exact in binary64 first: low31 needs 31 bits, span*x at most 29.
inline float towardZero(double exact)noexcept {
    volatile float narrowed=static_cast<float>(exact);float value=narrowed;
    if(static_cast<double>(value)>exact)value=std::nextafter(value,0.f);
    return value;
}
inline bool draw(std::uint32_t&raw,unsigned span,unsigned base,unsigned&value)noexcept {
    raw=raw*0x41c64e6du+0x3039u;
    const float integer=towardZero(static_cast<double>(raw&0x7fffffffu));
    const float unit=mul(0x1p-31f,integer); // Power-of-two scale: exact.
    const float scaled=towardZero(static_cast<double>(span)*static_cast<double>(unit));
    // Unreachable with toward-zero narrowing; kept as the source assertion's
    // bound, refusing rather than choosing a different variant.
    if(!finite(scaled)||scaled<0.f||scaled>=static_cast<float>(span))return false;
    value=base+static_cast<unsigned>(scaled);return true;
}
//19FD20: start an outward or return curve. Do not clamp, snap to the target or
// substitute a conventional easing curve for the original scalar integration.
inline bool start(EngineDamageState50IA&s,float target,float duration)noexcept {
    if(!(duration>0.f))return false;
    s.flags4A4=2u;s.target490=target;s.velocity49C=0.f;
    const float denominator=div(mul(duration,duration),4.f);
    s.acceleration4A0=div(sub(target,s.scale484),denominator);
    s.duration494=s.remaining498=duration;
    return valid(s);
}
}
// Once at the existing selected-player engine subscriber boundary, before its
// pitch controls and warnings, using the live post-frame damage and SAME RNG.
// Source order:19EEC8 cooldown ->19FBF0 active curve ->19F6B0 selection.
// The native host's explicit post-frame subscriber phase remains a host phase;
// this does not reconstruct the full original emitter manager scheduler.
// No hot allocation. On any error, state and RNG remain unchanged. Prior/next
// may alias exactly. The receipt describes only this attempted update.
inline EngineDamageReceipt50IA updateEngineDamageAudio50IA(const EngineDamageInput50IA&in,
    const EngineDamageState50IA&before,EngineDamageState50IA&after,
    std::uint32_t&sharedRawRandomState)noexcept {
    using namespace engine_damage50IA_detail;
    EngineDamageReceipt50IA r{};
    if(std::fegetround()!=FE_TONEAREST||!finite(in.factor)||in.factor<0.f||
       !finite(in.damage7D0)||!finite(in.damage7D4)||!finite(in.damage7DC)||!valid(before))return r;
    auto s=before;auto raw=sharedRawRandomState;
    if(s.cooldown474!=0.f){
        s.cooldown474=sub(s.cooldown474,in.factor);
        if(s.cooldown474<0.f)s.cooldown474=0.f;
    }
    if(s.flags4A4&2u){
        s.remaining498=sub(s.remaining498,in.factor);
        const float delta=mul(s.acceleration4A0,in.factor);
        if(s.remaining498<div(s.duration494,2.f))s.velocity49C=sub(s.velocity49C,delta);
        else s.velocity49C=add(s.velocity49C,delta);
        s.scale484=add(s.scale484,mul(s.velocity49C,in.factor));
        if(s.remaining498<=0.f){
            if(s.flags4A4&1u){s.scale484=1.f;s.flags4A4=0u;r.dipFinished=true;}
            else{
                if(!start(s,1.f,s.duration494))return r;
                s.flags4A4|=1u;r.returnStarted=true;
            }
        }
    }
    if(s.cooldown474==0.f){
        //2234E0 max damage deliberately omits7D8. The source always draws the
        // two candidate values when cooldown is zero, even at damage<=0.3.
        const float damage=std::max(std::max(in.damage7D0,in.damage7D4),in.damage7DC);
        unsigned durationWord=0,targetWord=0;
        if(!draw(raw,26,50,durationWord)||!draw(raw,31,50,targetWord)){
            r.status=EngineDamageStatus50IA::RandomRange;return r;
        }
        r.randomDraws=2;
        if(damage>0.3f){
            if(!start(s,div(static_cast<float>(targetWord),100.f),
                div(static_cast<float>(durationWord),100.f)))return r;
            unsigned cooldownWord=0;
            const bool high=damage>0.7f;
            if(!draw(raw,high?3u:4u,high?1u:5u,cooldownWord)){
                r.status=EngineDamageStatus50IA::RandomRange;return r;
            }
            ++r.randomDraws;s.cooldown474=static_cast<float>(cooldownWord);
            r.damageDipStarted=true;
        }
    }
    if(!valid(s))return r;
    after=s;sharedRawRandomState=raw;r.status=EngineDamageStatus50IA::Complete;return r;
}
} // namespace rr::game
