#pragma once
#include "player_events49a.h"
#include "original_gameplay_blur_leaf.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>

namespace rr::game {
// Sole represented Player1090/1094/1098 owner is the actual49A event state.
// Constructor1A0368/70 writes1094=0/1090=-1. The accepted cold full-reset
// boundary1A2588 writes1098=5. Neither2255B0(a1=0) caller resets1098.
inline void initializeOriginalHumanResetTimers50LC(PlayerEventState49A& player) noexcept {
    player.value1090=-1.f;player.value1094=0.f;player.value1098=5.f;
}
// Source1A1294..1A12D0 and normal1090<0 branch1A14D8..1A1590.
// Run before the same Player's1A1A20 input reader. It borrows the renderer's
// real2AED10 first-request-wins owner, never a receipt/no-op blur provider.
// Other damage-state1090 branches remain explicit unsupported source owners.
// Completed1098 prefix survives a rejected later numeric/effect operation.
inline bool advanceOriginalHumanResetTimers50LC(PlayerEventState49A& player,
    std::uint32_t vehicle3C,std::uint8_t manager58,float elapsed,
    GameplayBlurPending& blur,std::uint32_t& sourcePC) noexcept {
    sourcePC=0;
    if(vehicle3C||!(manager58&2u))return true;
    auto scalar=[](float x) noexcept {
        return std::isfinite(x)&&(x==0.f||std::fabs(x)>=std::numeric_limits<float>::min());
    };
    auto fail=[&](std::uint32_t pc) noexcept {sourcePC=pc;return false;};
    if(std::fegetround()!=FE_TONEAREST||!scalar(elapsed)||elapsed<0.f||!scalar(player.value1098))
        return fail(0x1a12b8u);
    if(player.value1098>=0.f){
        volatile float next=player.value1098-elapsed;
        if(!scalar(next))return fail(0x1a12ccu);
        player.value1098=next;
    }
    if(!scalar(player.value1090)||player.value1090>=0.f)return fail(0x1a12d4u);
    if(!scalar(player.value1094))return fail(0x1a14d8u);
    if(player.value1094<0.f)return true;
    volatile float ratio=player.value1094/2.5f;
    volatile float percent=100.f*ratio;
    volatile float scaled=80.f*percent;
    if(!scalar(ratio)||!scalar(percent)||!scalar(scaled)||scaled<0.f||scaled>=2147483648.f)
        return fail(0x1a151cu);
    // Original CVT.W.S truncation, followed by signed constant divide100.
    const auto strength=static_cast<std::int32_t>(scaled)/100;
    volatile float next=player.value1094-elapsed;
    if(!scalar(next))return fail(0x1a152cu);
    player.value1094=next; //1A154C delay slot precedes the manager request.
    if(player.source5C>=4u)return fail(0x1a1564u);
    const auto result=requestGameplayBlur(blur,next<=0.f?0:strength,
        static_cast<std::int32_t>(player.source5C),0);
    if(result==GameplayBlurRequestResult::unavailable_view)return fail(0x1a1564u);
    if(next<=0.f)player.value1094=-1.f;
    return true;
}
}
