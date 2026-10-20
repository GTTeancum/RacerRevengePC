#pragma once
#include "player_input48x.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdio>
#if defined(__SSE__)
#include <xmmintrin.h>
#endif

namespace rr::game {
// Native supplied-owner projection of original AI speed selection172C50..172DA8
// and ordinary non-boost actions173770..1737D8. This is NOT a complete AI owner,
// dispatcher, boost decision, direction planner, or HOST-course-driver policy.
// Source-only stage: not built, executed, integrated, or runtime validated.
struct AiSpeedTargetInput50IR {
    // Resolve BOTH from vehicle.FF4's actual node indexed by vehicle.C80.
    // 2A2520 reads node+5C; 2A25E0 reads bit4 of node+79 (DriveFullSpeed).
    float currentNodeSpeedLimit5C;
    std::uint8_t currentNodeFlags79;
    // Live output of171C60, not a hard-coded1: its constructor/reset value1
    // can be replaced by the original packing calculation each AI frame.
    float vehicleSpeedScaleFFC;
    std::int32_t vehicleMinimumSpeed108C; // Signed MyMinSpeed loader result.

    std::uint8_t trackControlFlags58; // app28C -> B4 -> byte58, NOT vehicle58.
    std::uint8_t vehicleFlagsCD8;
    std::uint32_t vehicleAiStateFE8;
    // The following owners are read only when track bit6 is set, CD8 bit0 is
    // clear, and AI stateFE8 is not1. Names intentionally retain source fields.
    float trackRacePace7C;              // app28C -> C0 -> float7C.
    bool settings188F60Nonzero;          // Actual188F60(app2DC,0) return.
    std::int32_t vehicleCCC;
    float vehicleCB0;
    float vehicleCC4;
    float aiTrackLength3C;              // Same vehicle.FF4 -> float3C.
};
struct AiSpeedTargetReceipt50IR {
    float desiredSpeed;
    bool racePaceBranch;
    bool racePaceCapApplied;
    bool minimumApplied;
    bool driveFullSpeedOverride;
};
enum class AiSpeedError50IR : std::uint8_t {
    None, RoundingMode, Input, Arithmetic
};
struct AiSpeedDiagnostic50IR {
    AiSpeedError50IR error=AiSpeedError50IR::None;
    std::uint32_t sourcePC=0;
};
namespace ai_speed_detail50IR {
inline float word(std::uint32_t bits) noexcept {
    float value;std::memcpy(&value,&bits,sizeof value);return value;
}
// Deliberately bounded native arithmetic, not a guest-FPU implementation.
// Exclude nonzero subnormals, NaNs, infinities, and non-nearest host rounding.
inline bool admitted(float value) noexcept {
    return std::isfinite(value)&&(value==0.f||std::isnormal(value));
}
inline bool fail(AiSpeedDiagnostic50IR& d,AiSpeedError50IR error,
                 std::uint32_t pc) noexcept {d={error,pc};return false;}
struct Math {
    bool valid=true;
    float keep(float value) noexcept {valid=valid&&admitted(value);return value;}
    float add(float a,float b) noexcept {volatile float value=a+b;return keep(value);}
    float sub(float a,float b) noexcept {volatile float value=a-b;return keep(value);}
    float mul(float a,float b) noexcept {volatile float value=a*b;return keep(value);}
    float div(float a,float b) noexcept {
        if(b==0.f){valid=false;return 0.f;}
        volatile float value=a/b;return keep(value);
    }
    float integer(std::int32_t source) noexcept {
        volatile float value=static_cast<float>(source);return keep(value);
    }
};
} // namespace ai_speed_detail50IR

// Exact selected source operations with separately rounded native binary32.
// Preserve operation order: scaled node limit, conditional race-pace cap,
// signed minimum-speed floor, then DriveFullSpeed's unscaled node override.
// Require strict binary32 compilation, no fast-math/contraction, and gradual
// host underflow (FTZ/DAZ disabled). FE_TONEAREST alone cannot establish these.
// No lookahead minimum, curvature cap, planar-speed estimate, or invented pace.
// Failure preserves out. Diagnostic is replaced. No allocation or callbacks.
inline bool selectAiTargetSpeed50IR(const AiSpeedTargetInput50IR& in,
    AiSpeedTargetReceipt50IR& out,AiSpeedDiagnostic50IR& d) noexcept {
    using namespace ai_speed_detail50IR;
    d={};
    if(std::fegetround()!=FE_TONEAREST)
        return fail(d,AiSpeedError50IR::RoundingMode,0x172c50);
    if(!admitted(in.currentNodeSpeedLimit5C)||!admitted(in.vehicleSpeedScaleFFC))
        return fail(d,AiSpeedError50IR::Input,0x172ca4);
    Math f;
    AiSpeedTargetReceipt50IR next{};
    float target=f.mul(in.currentNodeSpeedLimit5C,in.vehicleSpeedScaleFFC);
    if(!f.valid)return fail(d,AiSpeedError50IR::Arithmetic,0x172ca4);
    if((in.trackControlFlags58&0x40u)&&!(in.vehicleFlagsCD8&1u)&&
       in.vehicleAiStateFE8!=1u) {
        next.racePaceBranch=true;
        if(!admitted(in.trackRacePace7C))
            return fail(d,AiSpeedError50IR::Input,0x172cd0);
        float pace=in.trackRacePace7C;
        if(in.settings188F60Nonzero)pace=f.sub(pace,10.f); //172CE4..172CF0.
        if(!admitted(in.vehicleCB0))
            return fail(d,AiSpeedError50IR::Input,0x172cfc);
        // These add/sub operations also run when pace<=0: the source branch's
        // delay slot computes the remaining interval before skipping the cap.
        const float interval=f.add(pace,f.integer(in.vehicleCCC));
        const float remaining=f.sub(interval,in.vehicleCB0);
        if(!f.valid)return fail(d,AiSpeedError50IR::Arithmetic,0x172d10);
        if(pace>0.f) {
            if(!admitted(in.vehicleCC4)||!admitted(in.aiTrackLength3C))
                return fail(d,AiSpeedError50IR::Input,0x172d14);
            const float units=word(0x3f2e8bc1u);
            const float remainingRate=f.div(f.mul(units,in.vehicleCC4),remaining);
            const float wholeRate=f.div(f.mul(units,in.aiTrackLength3C),interval);
            if(!f.valid)return fail(d,AiSpeedError50IR::Arithmetic,0x172d3c);
            //172D40 is scalar MAX.S, followed by the ordinary target<=cap test.
            const float cap=detail::scalarMax48O(remainingRate,wholeRate);
            if(!(target<=cap)){target=cap;next.racePaceCapApplied=true;}
        }
    }
    const float minimum=f.integer(in.vehicleMinimumSpeed108C);
    if(target<minimum){target=minimum;next.minimumApplied=true;}
    if(in.currentNodeFlags79&0x10u) {
        target=in.currentNodeSpeedLimit5C;
        next.driveFullSpeedOverride=true;
    }
    if(!f.valid)return fail(d,AiSpeedError50IR::Arithmetic,0x172d68);
    next.desiredSpeed=target;out=next;return true;
}

struct AiNonBoostSpeedReceipt50IR {
    float convertedCachedSpeed;
    float desiredMinusCurrent;
    bool braking;
};
// Source173770 is called by172C50 ONLY AFTER the real173180 boost decision
// returns0. Caller must preserve that scheduling; this function does not decide
// whether boost should be attempted, and must not replace it with a guessed flag.
// bodyCachedSpeed34 must come from vehicle340->34 at that point, with its sign.
// The existing native field view preserves every raw steering/control channel;
// only B7C, B80 and B9C bit0 change. It does not run the player/device reader.
// Both outputs remain unchanged on failure. No vehicle/route/physics writes.
inline bool applyAiNonBoostSpeed50IR(float desiredSpeed,float bodyCachedSpeed34,
    const PlayerReadResult48X& previous,PlayerReadResult48X& out,
    AiNonBoostSpeedReceipt50IR& receipt,AiSpeedDiagnostic50IR& d) noexcept {
    using namespace ai_speed_detail50IR;
    d={};
    if(std::fegetround()!=FE_TONEAREST)
        return fail(d,AiSpeedError50IR::RoundingMode,0x173770);
    bool desiredAccepted=false,cachedEvaluated=false,cachedAccepted=false;
    if(!(desiredAccepted=admitted(desiredSpeed))||
       !(cachedEvaluated=true,cachedAccepted=std::isfinite(bodyCachedSpeed34))) {
        // Capture each actual predicate once, retaining short-circuit order.
        // Additional word classification neither replays nor substitutes it.
        const auto bits=[](float v) noexcept {std::uint32_t b;std::memcpy(&b,&v,sizeof b);return b;};
        const auto wordAdmitted=[](std::uint32_t b) noexcept {
            const auto magnitude=b&0x7fffffffu,exponent=b&0x7f800000u;
            return magnitude==0u||(exponent!=0u&&exponent!=0x7f800000u);
        };
        const auto desiredBits=bits(desiredSpeed),cachedBits=bits(bodyCachedSpeed34);
        const bool desiredWordAdmitted=wordAdmitted(desiredBits),cachedWordAdmitted=wordAdmitted(cachedBits);
        const int rounding=std::fegetround(),exceptions=std::fetestexcept(FE_ALL_EXCEPT);
        unsigned mxcsrAvailable=0,mxcsr=0;
#if defined(__SSE__)
        mxcsrAvailable=1;mxcsr=_mm_getcsr();
#endif
        std::fprintf(stderr,"AI_NONBOOST_INPUT_REJECT pc=0017379c desired=%08x cached_speed=%08x first_guard=%u desired_guard_accepted=%u cached_guard_accepted=%u desired_word_admitted=%u cached_guard_evaluated=%u cached_word_admitted=%u rounding=%d exceptions=%x mxcsr_available=%u mxcsr=%08x\n",
            desiredBits,cachedBits,desiredAccepted?2u:1u,unsigned(desiredAccepted),unsigned(cachedAccepted),unsigned(desiredWordAdmitted),unsigned(cachedEvaluated),unsigned(cachedWordAdmitted),rounding,unsigned(exceptions),mxcsrAvailable,mxcsr);
        return fail(d,AiSpeedError50IR::Input,0x17379c);
    }
    Math f;
    AiNonBoostSpeedReceipt50IR next{};
    // Scoped native gradual-underflow policy for the actual cached-speed field.
    // Preserve the source MUL then SUB and the exact rounded product bits.
    // This does not establish guest FPU arithmetic/status parity. All other
    // inputs/results, including the live difference, retain their old guards.
    volatile float converted=word(0x3f2e8bc1u)*bodyCachedSpeed34;
    next.convertedCachedSpeed=converted;
    f.valid=f.valid&&std::isfinite(next.convertedCachedSpeed);
    next.desiredMinusCurrent=f.sub(desiredSpeed,next.convertedCachedSpeed);
    if(!f.valid)return fail(d,AiSpeedError50IR::Arithmetic,0x1737a4);
    next.braking=next.desiredMinusCurrent<0.f; // Equality, including -0, accelerates.
    auto result=previous;
    result.flagsB9C=static_cast<std::uint8_t>(result.flagsB9C&0xfeu);
    result.actionB7C=next.braking?0.f:1.f;
    result.actionB80=next.braking?1.f:0.f;
    out=result;receipt=next;return true;
}
} // namespace rr::game
