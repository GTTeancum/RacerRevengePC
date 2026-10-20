#pragma once
#include "ai_speed_controls50is.h"
#include "boost_controls50fk.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdio>
#if defined(__SSE__)
#include <xmmintrin.h>
#endif

namespace rr::game {
// Native owner for the previously missing173180 cooldown only. Other AI state
// remains owned by its genuine state/avoidance/target producers, never defaulted.
struct AiBoostDecisionState50IT {
    float cooldown109C=0.f;
    bool initialized=false; // Native coverage marker, not an original field.
};
// Only at original constructor16E350 or actual reset16E878. Never per frame.
// These source stores both assign the exact6.0f word to vehicle109C.
inline void initializeAiBoostDecision50IT(AiBoostDecisionState50IT& out) noexcept {
    out.cooldown109C=6.f;out.initialized=true;
}
struct AiBoostDecisionInput50IT {
    float preparedFactor,desiredSpeed,heat7C,cachedSpeed34;
    std::uint32_t aiStateFE8,field1038,field103C,probe10C;
    bool target1008Present;
    std::uint8_t target1008FlagsB9C;
};
enum class AiBoostDecisionReason50IT : std::uint8_t {
    Heat, AboveDesiredSpeed, Steering, Field1038Or103C, State4,
    TargetBoosting, Below400, AlreadyBoosting, Probe10C, Cooldown,
    RandomDeclined, RandomAccepted
};
struct AiBoostDecisionReceipt50IT {
    bool handled=false,randomDrawn=false;
    AiBoostDecisionReason50IT reason=AiBoostDecisionReason50IT::Heat;
    float convertedSpeed=0.f,randomUnit=0.f,heatSquared=0.f;
};
enum class AiBoostDecisionError50IT : std::uint8_t {
    None, Uninitialized, RoundingMode, Input, MissingTarget, Arithmetic,
    MissingOwner, WrongBoundary, ChangedOwner
};
struct AiBoostDecisionDiagnostic50IT {
    AiBoostDecisionError50IT error=AiBoostDecisionError50IT::None;
    std::uint32_t sourcePC=0;
};
namespace ai_boost_detail50IT {
inline bool fail(AiBoostDecisionDiagnostic50IT& d,AiBoostDecisionError50IT e,
    std::uint32_t pc) noexcept {d={e,pc};return false;}
}
// Original173180..1734B0. Atomic admitted-domain numeric owner update. Input,
// diagnostic and receipt must be disjoint; before/out actions may alias exactly.
// Preserve the one genuine shared C-runtime RNG; never seed a private AI stream.
// Native strict binary32/FE_TONEAREST applies. Only actual cached-speed
// input/conversion admits finite subnormals; other50IR guards are unchanged.
// This scoped native policy does not establish guest FPU arithmetic/status parity.
inline bool decideAiBoost50IT(const AiBoostDecisionInput50IT& in,
    const PlayerReadResult48X& before,PlayerReadResult48X& out,
    AiBoostDecisionState50IT& state,std::uint32_t& actualSharedRandom,
    AiBoostDecisionReceipt50IT& receipt,AiBoostDecisionDiagnostic50IT& d) noexcept {
    using namespace ai_speed_detail50IR;
    using ai_boost_detail50IT::fail;
    d={};
    if(!state.initialized)return fail(d,AiBoostDecisionError50IT::Uninitialized,0x16e350);
    if(std::fegetround()!=FE_TONEAREST)return fail(d,AiBoostDecisionError50IT::RoundingMode,0x173180);
    if(!admitted(in.heat7C))return fail(d,AiBoostDecisionError50IT::Input,0x173194);
    Math f;auto next=state;auto actions=before;auto random=actualSharedRandom;
    AiBoostDecisionReceipt50IT result{};
    auto finish=[&](AiBoostDecisionReason50IT reason,bool handled,bool clearBoost) noexcept {
        result.reason=reason;result.handled=handled;
        if(clearBoost)actions.flagsB9C=static_cast<std::uint8_t>(actions.flagsB9C&0xfeu);
        if(handled){actions.flagsB9C=static_cast<std::uint8_t>(actions.flagsB9C|1u);
            actions.actionB7C=1.f;actions.actionB80=0.f;}
        state=next;actualSharedRandom=random;out=actions;receipt=result;return true;
    };
    using R=AiBoostDecisionReason50IT;
    const float threshold=word(0x3f666666u);
    if(!(in.heat7C<=threshold))return finish(R::Heat,false,true);
    bool cachedAccepted=false,desiredEvaluated=false,desiredAccepted=false;
    if(!(cachedAccepted=std::isfinite(in.cachedSpeed34))||
       !(desiredEvaluated=true,desiredAccepted=admitted(in.desiredSpeed))) {
        const auto bits=[](float v) noexcept {std::uint32_t b;std::memcpy(&b,&v,sizeof b);return b;};
        const int rounding=std::fegetround(),exceptions=std::fetestexcept(FE_ALL_EXCEPT);
        unsigned mxcsrAvailable=0,mxcsr=0;
#if defined(__SSE__)
        mxcsrAvailable=1;mxcsr=_mm_getcsr();
#endif
        std::fprintf(stderr,"AI_BOOST_SPEED_INPUT_REJECT pc=001731e8 cached_speed=%08x desired=%08x first_guard=%u cached_accepted=%u desired_evaluated=%u desired_accepted=%u rounding=%d exceptions=%x mxcsr_available=%u mxcsr=%08x\n",
            bits(in.cachedSpeed34),bits(in.desiredSpeed),cachedAccepted?2u:1u,unsigned(cachedAccepted),unsigned(desiredEvaluated),unsigned(desiredAccepted),rounding,unsigned(exceptions),mxcsrAvailable,mxcsr);
        return fail(d,AiBoostDecisionError50IT::Input,0x1731e8);
    }
    auto convertedCachedSpeed=[&]() noexcept {
        volatile float value=word(0x3f2e8bc1u)*in.cachedSpeed34;
        f.valid=f.valid&&std::isfinite(value);return float(value);
    };
    result.convertedSpeed=convertedCachedSpeed();
    if(!f.valid)return fail(d,AiBoostDecisionError50IT::Arithmetic,0x1731ec);
    if(!(result.convertedSpeed<=in.desiredSpeed))return finish(R::AboveDesiredSpeed,false,true);
    if(!admitted(before.raw.values[0]))return fail(d,AiBoostDecisionError50IT::Input,0x173220);
    //173244 returns0 WITHOUT clearing bit0. The caller's ordinary leaf may clear it later.
    if(!(std::fabs(before.raw.values[0])<=threshold))return finish(R::Steering,false,false);
    if(in.field1038||in.field103C)return finish(R::Field1038Or103C,false,true);
    if(in.aiStateFE8==4u)return finish(R::State4,false,true);
    if(in.aiStateFE8==1u){
        if(!in.target1008Present)return fail(d,AiBoostDecisionError50IT::MissingTarget,0x1732c0);
        if(in.target1008FlagsB9C&1u)return finish(R::TargetBoosting,false,true);
    }
    //173314 recomputes the same cached-speed product, preserving source order.
    result.convertedSpeed=convertedCachedSpeed();
    if(!f.valid)return fail(d,AiBoostDecisionError50IT::Arithmetic,0x173314);
    if(result.convertedSpeed<400.f)return finish(R::Below400,false,true);
    if(actions.flagsB9C&1u)return finish(R::AlreadyBoosting,true,false);
    if(in.probe10C)return finish(R::Probe10C,true,false);
    if(!admitted(next.cooldown109C))return fail(d,AiBoostDecisionError50IT::Input,0x1733fc);
    if(next.cooldown109C>0.f){
        if(!admitted(in.preparedFactor)||in.preparedFactor<0.f)
            return fail(d,AiBoostDecisionError50IT::Input,0x173418);
        next.cooldown109C=f.sub(next.cooldown109C,in.preparedFactor);
        if(!f.valid)return fail(d,AiBoostDecisionError50IT::Arithmetic,0x173418);
        // No clamp to zero. It is reconsidered only on the next eligible call.
        return finish(R::Cooldown,false,false);
    }
    next.cooldown109C=1.f; //17342C delay slot before the actual shared RNG call.
    //308030 advances/stores all32 bits and returns its low31 bits.
    random=random*0x41c64e6du+0x3039u;
    const float drawn=f.integer(static_cast<std::int32_t>(random&0x7fffffffu));
    result.randomUnit=f.mul(word(0x30000000u),drawn);
    result.heatSquared=f.mul(in.heat7C,in.heat7C);
    if(!f.valid)return fail(d,AiBoostDecisionError50IT::Arithmetic,0x173458);
    result.randomDrawn=true;
    // Source calls pure2A2580(node BoostScale) but never consumes its return.
    // No invented multiplier. Equality declines; strictly greater accepts.
    if(result.randomUnit<=result.heatSquared)return finish(R::RandomDeclined,false,false);
    return finish(R::RandomAccepted,true,false);
}

// Bind these borrowed pointers to genuine native owners, not copies/defaults.
// In particular, actualSharedRandom must be the same serial C-runtime stream
// across racers and other308030 consumers, outside a per-racer snapshot bank.
struct AiBoostNativeOwner50IT {
    const VehicleFrameState49W* frame=nullptr;
    const AiSpeedControlPending50IS* pending=nullptr;
    const BoostState50FK* thermal=nullptr;
    const std::uint32_t* aiStateFE8=nullptr;
    const std::uint32_t* field1038=nullptr;
    const std::uint32_t* field103C=nullptr;
    const PlayerReadResult48X* target1008Controls=nullptr;
    AiBoostDecisionState50IT* decision=nullptr;
    std::uint32_t* actualSharedRandom=nullptr;
    AiBoostDecisionReceipt50IT lastReceipt{};
    AiBoostDecisionDiagnostic50IT lastDiagnostic{};
};
inline bool completeNativeAiBoost50IT(void* context,float preparedFactor,float desiredSpeed,
    const PlayerReadResult48X& before,PlayerReadResult48X& after,bool& handled) noexcept {
    if(!context)return false;
    auto& owner=*static_cast<AiBoostNativeOwner50IT*>(context);
    using ai_boost_detail50IT::fail;
    auto& d=owner.lastDiagnostic;d={};
    if(!owner.frame||!owner.pending||!owner.thermal||!owner.aiStateFE8||
       !owner.field1038||!owner.field103C||!owner.decision||!owner.actualSharedRandom)
        return fail(d,AiBoostDecisionError50IT::MissingOwner,0x173180);
    const auto& pending=*owner.pending;
    if(&before!=&pending.afterDirection||pending.owner!=owner.frame||
       pending.phase!=AiSpeedControlPhase50IS::BoostIncomplete||
       owner.frame->fields.vehicle58!=1u)
        return fail(d,AiBoostDecisionError50IT::WrongBoundary,0x173180);
    if(!ai_controls_detail50IS::unchanged(*owner.frame,pending)||
       *owner.aiStateFE8!=pending.observedAiStateFE8||
       !ai_controls_detail50IS::sameFloat(preparedFactor,pending.preparedFactor)||
       !ai_controls_detail50IS::sameFloat(desiredSpeed,pending.target.desiredSpeed))
        return fail(d,AiBoostDecisionError50IT::ChangedOwner,0x173180);
    AiBoostDecisionInput50IT input{};
    input.preparedFactor=preparedFactor;input.desiredSpeed=desiredSpeed;
    input.heat7C=owner.thermal->heat7C;
    input.cachedSpeed34=owner.frame->bodies.main.cachedSpeed34;
    input.aiStateFE8=*owner.aiStateFE8;input.field1038=*owner.field1038;input.field103C=*owner.field103C;
    input.probe10C=owner.frame->bodies.main.components.probe348.field10C;
    input.target1008Present=owner.target1008Controls!=nullptr;
    if(owner.target1008Controls)input.target1008FlagsB9C=owner.target1008Controls->flagsB9C;
    PlayerReadResult48X result{};AiBoostDecisionReceipt50IT receipt{};
    if(!decideAiBoost50IT(input,before,result,*owner.decision,*owner.actualSharedRandom,receipt,d))return false;
    after=result;handled=receipt.handled;owner.lastReceipt=receipt;return true;
}
inline AiBoostOwner50IS nativeAiBoostProvider50IT(AiBoostNativeOwner50IT& owner) noexcept {
    return {&owner,&completeNativeAiBoost50IT};
}

// The shared2176E0 numeric thermal path is reusable for this bounded AI route:
// AI vtable3CE1E0+A8=173CB0 is JR RA/NOP, just like the retained player callback.
// Run AFTER217DE0 binding at source228788, never before this frame's173180.
// Requires genuine absent D40 and authentic state/parameters/engine histories;
// no AI frame admission, profile lookup, audio owner or cold setup is invented.
inline bool advanceAiThermal50IT(const BoostInput50FK& input,const BoostParameters50FK& parameters,
    const BoostState50FK& before,BoostState50FK& after,BoostDiagnostic50FK& diagnostic) noexcept {
    if(input.vehicle58!=1u){diagnostic={BoostError50FK::Input,0x2176e0};return false;}
    return advancePlayerBoost50FK(input,parameters,before,after,diagnostic);
}
} // namespace rr::game
