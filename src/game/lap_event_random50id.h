#pragma once
#include "lap_event6_subscriber50id.h"
#include "clock_quantization49j.h"

namespace rr::game {
// Real101A50 numeric provider, using the existing recovered49J conversion.
// The caller MUST supply the real selected clock owner's sampled16-bit timer
// and pending overflow state. This helper has no host-clock/raceSeconds fallback.
//101A50 shares timer globals435368/435370 and previous16-bit sample3D61C0
// with the existing clock owner; never construct a private clock for speech.
inline bool applyLapEventClock50ID(const LapEventRequest50ID& request,
    ClockCounterState49J& actualClock,std::uint16_t actualCurrentCount,
    std::uint32_t& actualV0) noexcept {
    if(request.kind!=LapEventCall50ID::Clock101A50)return false;
    auto next=actualClock;
    consumeTimerSample49J(next,actualCurrentCount);
    ClockQuantization49J output{};
    if(!quantizeCounter49J(next.accumulatedTicks,output))return false;
    actualClock=next;actualV0=output.uint32timestamp;return true;
}
//308020 stores the seed into actual C-runtime RNG owner+58.308030 advances
// that same raw32 state, stores raw32, and returns its low31 bits.
inline bool applyLapEventRandom50ID(const LapEventRequest50ID& request,
    std::uint32_t& actualSharedRawRandom,std::uint32_t& actualV0) noexcept {
    if(request.kind==LapEventCall50ID::Seed308020){
        actualSharedRawRandom=request.argumentA0;actualV0=0;return true;
    }
    if(request.kind==LapEventCall50ID::Draw308030){
        actualSharedRawRandom=actualSharedRawRandom*0x41c64e6du+0x3039u;
        actualV0=actualSharedRawRandom&0x7fffffffu;return true;
    }
    return false;
}
}
