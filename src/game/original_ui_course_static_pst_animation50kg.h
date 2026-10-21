#pragma once
#include "original_ui_texture_animation50kg.h"
#include <string_view>

namespace rr::game {
// Distinct from an ATM: 29A10C reads the first word of the PST resource's
// name20, not a count of stored images. The original UI prefix17DBF8 and
// copy24DF10 produce bytes 75 69 5C 74 ("ui\\t") for these actual resources.
// No image capacity or ordinal is represented by this signed numeric bound.
struct OriginalUiCourseStaticPstAnimation50KG {
    OriginalUiTextureAnimationConfig50KG config{};
    std::int32_t sourceNameWord20=0;
    float phase=0.f;
    std::uint32_t previousMilliseconds=0xffffffffu;
    bool reverseDirection=false,stopped=false;
};
inline bool validOriginalUiCourseStaticPstAnimation50KG(
    const OriginalUiCourseStaticPstAnimation50KG& state) noexcept {
    const auto& c=state.config;
    return state.sourceNameWord20==0x745c6975 && c.initialFrame==0.f &&
        c.framesPerSecond==8.f && c.looping &&
        c.reverseLoopOnEnd && c.loopOnEndToFrame==0 && std::isfinite(state.phase);
}
inline bool initializeOriginalUiCourseStaticPstAnimation50KG(
    OriginalUiCourseStaticPstAnimation50KG& state,
    const OriginalUiTextureAnimationConfig50KG& config,
    std::string_view originalResourceName20) noexcept {
    if(originalResourceName20!="ui\\textures\\arrwleft_d.pst" &&
       originalResourceName20!="ui\\textures\\arrwrght_d.pst")return false;
    std::uint32_t word=0;
    for(unsigned i=0;i<4;++i)word|=std::uint32_t(static_cast<unsigned char>(originalResourceName20[i]))<<(8u*i);
    OriginalUiCourseStaticPstAnimation50KG next;
    next.config=config;next.sourceNameWord20=static_cast<std::int32_t>(word);
    next.phase=config.initialFrame;
    if(!validOriginalUiCourseStaticPstAnimation50KG(next))return false;
    state=next;return true;
}
// Numeric leaf29A020 only. Its owning material controller services the real
// clock at29AC90 and separately commits leaf10 and outer08. This function
// deliberately has no duplicate/pause gate: those belong to the outer owner.
// The PST virtual18 getter29C6D0 ignores the resulting phase and returns the
// retained same PST. Never call the ATM frame selector on this state.
inline bool advanceOriginalUiCourseStaticPstAnimation50KG(
    OriginalUiCourseStaticPstAnimation50KG& state,
    std::uint32_t sourceMilliseconds) noexcept {
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559,
        "Original texture animation requires IEEE binary32");
    if(std::fegetround()!=FE_TONEAREST||!validOriginalUiCourseStaticPstAnimation50KG(state))return false;
    auto next=state;
    float seconds=0.f;
    if(next.previousMilliseconds!=0xffffffffu){
        const std::uint32_t delta=sourceMilliseconds-next.previousMilliseconds;
        float milliseconds;
        if(delta&0x80000000u){
            volatile float half=static_cast<float>(static_cast<std::int32_t>((delta>>1)|(delta&1u)));
            volatile float doubled=half+half;milliseconds=doubled;
        }else{
            volatile float converted=static_cast<float>(static_cast<std::int32_t>(delta));
            milliseconds=converted;
        }
        const std::uint32_t scaleWord=0x3a83126fu;float scale;
        std::memcpy(&scale,&scaleWord,sizeof(scale));
        volatile float scaled=scale*milliseconds;seconds=scaled;
    }
    next.previousMilliseconds=sourceMilliseconds; //29A174
    volatile float advance=seconds*next.config.framesPerSecond;
    volatile float phase=next.reverseDirection?next.phase-advance:next.phase+advance;
    next.phase=phase;
    //29A1B4 is signed CVT.S.W. In particular, binary32(bound-1) may equal
    //binary32(bound). That source result is legal for a static PST getter.
    const float bound=static_cast<float>(next.sourceNameWord20);
    if(next.phase>=bound){
        const float remainder=std::fmod(next.phase,bound);
        volatile float reflected=static_cast<float>(next.sourceNameWord20-1)-remainder;
        next.phase=reflected;next.reverseDirection=true;
    }else if(next.phase<0.f){
        next.phase=std::fabs(std::fmod(next.phase,bound));next.reverseDirection=false;
    }
    if(next.stopped)next.phase=static_cast<float>(next.sourceNameWord20-1);
    if(!std::isfinite(next.phase))return false;
    state=next;return true;
}
}
