#include "original_results104_storage50la.h"
#include <bit>
#include <cfenv>
#include <cmath>

namespace rr::game {
namespace {
bool updateOrdinary(float value)noexcept {
    return std::isfinite(value)&&(value==0.f||std::fpclassify(value)==FP_NORMAL);
}
float updateAdd(float a,float b)noexcept {volatile float result=a+b;return result;}
float updateSub(float a,float b)noexcept {volatile float result=a-b;return result;}
}
bool OriginalResults10450LA::updateCpuPrefix1C8AE0(float actual23C,std::uint64_t publication,
    OriginalResults104Diagnostic50LA& d)noexcept {
    d={};if(!impl_){d.status=Impl::S::owner;d.sourcePC=0x1c8ae0;return false;}
    auto& p=*impl_;
    if(!constructed()||!p.loadComplete||!p.derived||!retirementCurrent())
        return p.stop(d,Impl::S::owner,0x1c8ae0,"Results actual loaded update owner absent");
    if(!publication||publication<=p.lastUpdateAttemptPublication)
        return p.stop(d,Impl::S::phase,0x1c8ae0,"Results Root publication cannot repeat or precede an attempted update");
    // Root itself proves consecutive actual publications. This Entity can be
    // skipped by141ED0 activity/mask gates, or by an unsupported native return
    // path: never synthesize attempts/deltas for those gaps or catch up later.
    // A failed prefix cannot replay in the same publication; a live failure
    // stops before Root return. This is attempt custody, not update success.
    p.lastUpdateAttemptPublication=publication;p.updateCpu.emplace();
    auto& out=*p.updateCpu;out.publication=publication;
    const auto stop=[&](Impl::S status,std::uint32_t pc,const char* operation){
        out.nextSourcePC=pc;return p.stop(d,status,pc,operation);
    };
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
        std::numeric_limits<float>::digits==24,"Results source binary32 timers");
#if defined(__FAST_MATH__) || defined(_M_FP_FAST) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
    return stop(Impl::S::source,0x1c8af8,"Results strict native float update required");
#else
    if(std::fegetround()!=FE_TONEAREST||!updateOrdinary(actual23C)||actual23C<0.f||
       actual23C>std::bit_cast<float>(std::uint32_t{0x3d75c28f}))
        return stop(Impl::S::source,0x1c8af8,"Results actual capped23C/nearest native domain");
    if(!p.nameEntry||!p.nameEntry->fieldAB8)
        return stop(Impl::S::incomplete,0x1c8ae8,"Results blinkAB8 has no reached name-entry initializer");
    const float blink=std::bit_cast<float>(*p.nameEntry->fieldAB8);
    if(!updateOrdinary(blink))return stop(Impl::S::source,0x1c8ae8,"Results blink timer operand domain");
    const float sum=updateAdd(blink,actual23C);
    if(!updateOrdinary(sum))return stop(Impl::S::source,0x1c8af8,"Results blink timer addition domain");
    p.displayCpu.reset(); //Invalidate existing CPU borrows before timer effects.
    p.nameEntry->fieldAB8=std::bit_cast<std::uint32_t>(sum); //1C8B08 delay slot, ALWAYS.
    out.sourceEffectsCommitted=true;
    if(!(sum<.5f)){
        const float remainder=updateSub(sum,.5f); //ONE subtraction, not modulo/a catch-up loop.
        if(!updateOrdinary(remainder))return stop(Impl::S::source,0x1c8b10,"Results blink subtraction domain");
        p.nameEntry->fieldAB8=std::bit_cast<std::uint32_t>(remainder); //BeforeAB4 read.
        if(!p.nameEntry->fieldAB4)
            return stop(Impl::S::incomplete,0x1c8b18,"Results cursorAB4 has no reached initializer");
        p.nameEntry->fieldAB4=*p.nameEntry->fieldAB4?0u:1u;
    }
    out.prefix=UpdateCpuPrefix::blink;
    if(!p.direction398)
        return stop(Impl::S::incomplete,0x1c8b2c,"Results pulse direction398 initialization remains unknown");
    const bool rising=*p.direction398!=0;
    const auto pulseReadPC=rising?0x1c8b38u:0x1c8b6cu;
    if(!p.pulse394)return stop(Impl::S::incomplete,pulseReadPC,"Results pulse394 initialization remains unknown");
    const float pulse=std::bit_cast<float>(*p.pulse394);
    if(!updateOrdinary(pulse))return stop(Impl::S::source,pulseReadPC,"Results pulse operand domain");
    if(rising){
        const float next=updateAdd(pulse,actual23C);
        if(!updateOrdinary(next))return stop(Impl::S::source,0x1c8b4c,"Results rising pulse addition domain");
        p.pulse394=std::bit_cast<std::uint32_t>(next); //1C8B5C delay slot.
        if(!(next<std::bit_cast<float>(std::uint32_t{0x3e99999a}))){
            p.direction398=0u;p.pulse394=0x3e99999au; //No reflection/carry of overshoot.
        }
    }else{
        const float next=updateSub(pulse,actual23C);
        if(!updateOrdinary(next))return stop(Impl::S::source,0x1c8b78,"Results falling pulse subtraction domain");
        p.pulse394=std::bit_cast<std::uint32_t>(next); //1C8B88 delay slot.
        if(next<=0.f){p.direction398=1u;p.pulse394=0u;}
    }
    out.prefix=UpdateCpuPrefix::pulse;
    const float cheat=std::bit_cast<float>(p.derived->fieldADC);
    if(!updateOrdinary(cheat))return stop(Impl::S::source,0x1c8b98,"Results cheat timer operand domain");
    if(!(cheat<=0.f)){
        const float next=updateSub(cheat,actual23C);
        if(!updateOrdinary(next))return stop(Impl::S::source,0x1c8bb4,"Results cheat timer subtraction domain");
        p.derived->fieldADC=std::bit_cast<std::uint32_t>(next); //1C8BC4 before clamp.
        if(next<=0.f){p.derived->fieldADC=0u;p.derived->fieldAE0=1u;}
    }
    const float transition=std::bit_cast<float>(p.derived->field390);
    if(!updateOrdinary(transition))return stop(Impl::S::source,0x1c8bd4,"Results transition timer operand domain");
    if(!(transition<=0.f)){
        const float next=updateSub(transition,actual23C);
        if(!updateOrdinary(next))return stop(Impl::S::source,0x1c8bf0,"Results transition subtraction domain");
        p.derived->field390=std::bit_cast<std::uint32_t>(next); //1C8BF8 beforeMAX.S.
        p.derived->field390=std::bit_cast<std::uint32_t>(next<0.f?0.f:next);
    }
    out.prefix=UpdateCpuPrefix::timers;
    // Mandatory1B9FE0 reads actual controller connection/state and may call
    //1BA120 before141ED0. Do not skip it with a generic empty-child success.
    return stop(Impl::S::incomplete,0x1c8c10,"Results timer prefix only; actual1B9FE0 input/base tail absent");
#endif
}
}
