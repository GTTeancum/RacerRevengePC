#pragma once
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

namespace rr::game {
// Admitted original 29A020 forward-loop subset. All four GC ATM materials
// explicitly reverse=false, loopTo=0, and inherit looping=true from29B950.
// This is animation state, not a clock: the existing world/render owner supplies
// its already sampled uint32 millisecond timestamp. No OS/audio clock is read.
struct AuthoredTextureAnimation {
    float phase = 0.f;
    std::uint32_t previousMilliseconds = 0;
    bool hasTimestamp = false;
};
inline bool advanceAuthoredTextureAnimation(AuthoredTextureAnimation& state,
    std::uint32_t sourceMilliseconds, float framesPerSecond,
    std::uint32_t frameCount, std::uint32_t& frame) noexcept {
    static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559,
        "Original texture animation requires IEEE binary32");
    if (std::fegetround()!=FE_TONEAREST || !frameCount || frameCount>64 ||
        !std::isfinite(framesPerSecond) || framesPerSecond<=0.f || framesPerSecond>60.f ||
        !std::isfinite(state.phase) || state.phase<0.f || state.phase>=float(frameCount)) return false;
    auto next=state;
    if (next.hasTimestamp && next.previousMilliseconds!=0xffffffffu &&
        next.previousMilliseconds!=sourceMilliseconds) {
        const std::uint32_t delta=sourceMilliseconds-next.previousMilliseconds;
        // Source29A120..14C uses sticky-half conversion for high-bit uint32.
        float milliseconds;
        if (delta&0x80000000u) {
            volatile float half=static_cast<float>(static_cast<std::int32_t>((delta>>1)|(delta&1u)));
            volatile float doubled=half+half; milliseconds=doubled;
        } else { volatile float converted=static_cast<float>(static_cast<std::int32_t>(delta)); milliseconds=converted; }
        const std::uint32_t scaleWord=0x3a83126fu; float scale;
        std::memcpy(&scale,&scaleWord,sizeof(scale));
        // Keep the three original separately-rounded scalar operations. No FMA.
        volatile float seconds=scale*milliseconds;
        volatile float advance=seconds*framesPerSecond;
        volatile float phase=next.phase+advance;
        next.phase=phase;
        if (next.phase>=float(frameCount)) {
            // Original302CB0/3004A0 fmodf, then its explicit loopTo=0 route.
            next.phase=std::fmod(next.phase,float(frameCount));
            next.phase=std::fmod(next.phase,float(frameCount));
        }
    }
    if (!std::isfinite(next.phase) || next.phase<0.f || next.phase>=float(frameCount)) return false;
    next.previousMilliseconds=sourceMilliseconds; next.hasTimestamp=true;
    // Original29AFD8..E0/29B024..2C CVT.W.S; admitted phase is nonnegative.
    const auto selected=static_cast<std::uint32_t>(next.phase);
    if (selected>=frameCount) return false;
    state=next; frame=selected; return true;
}
}
