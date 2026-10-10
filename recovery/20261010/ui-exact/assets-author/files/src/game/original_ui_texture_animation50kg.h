#pragma once
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

namespace rr::game {
struct OriginalUiTextureAnimationConfig50KG {
    // Actual 29B950 constructor defaults, independent of any one UI material.
    float initialFrame = 0.f;
    float framesPerSecond = 1.f;
    bool looping = true;
    bool reverseLoopOnEnd = false;
    std::int32_t loopOnEndToFrame = -1;
};
inline OriginalUiTextureAnimationConfig50KG originalUiArrowAnimationConfig50KG() noexcept {
    // Authenticated UIMAIN arrwleft.psm/arrwrght.psm receipts supply these
    // three overrides; InitialFrame and Looping are omitted in both materials.
    OriginalUiTextureAnimationConfig50KG result;
    result.framesPerSecond = 16.f;
    result.reverseLoopOnEnd = true;
    result.loopOnEndToFrame = 0;
    return result;
}
struct OriginalUiTextureAnimation50KG {
    OriginalUiTextureAnimationConfig50KG config;
    std::uint32_t frameCount = 0;
    float phase = 0.f;
    // Both original numeric state and outer controller start with -1.
    std::uint32_t previousMilliseconds = 0xffffffffu;
    bool reverseDirection = false;
    bool stopped = false;
};
inline bool validOriginalUiTextureAnimationConfig50KG(
    const OriginalUiTextureAnimationConfig50KG& config, std::uint32_t count) noexcept {
    return count && count <= 64 && std::isfinite(config.initialFrame) &&
        config.initialFrame >= 0.f && config.initialFrame < float(count) &&
        std::isfinite(config.framesPerSecond) && config.framesPerSecond > 0.f &&
        config.framesPerSecond <= 60.f && config.loopOnEndToFrame >= -1 &&
        config.loopOnEndToFrame < static_cast<std::int32_t>(count);
}
inline bool initializeOriginalUiTextureAnimation50KG(OriginalUiTextureAnimation50KG& state,
    const OriginalUiTextureAnimationConfig50KG& config, std::uint32_t frameCount) noexcept {
    if (!validOriginalUiTextureAnimationConfig50KG(config, frameCount)) return false;
    OriginalUiTextureAnimation50KG initialized;
    initialized.config = config; initialized.frameCount = frameCount;
    initialized.phase = config.initialFrame;
    state = initialized;
    return true;
}
inline bool selectOriginalUiTextureAnimationFrame50KG(const OriginalUiTextureAnimation50KG& state,
    std::uint32_t& frame) noexcept {
    if (!validOriginalUiTextureAnimationConfig50KG(state.config, state.frameCount) ||
        !std::isfinite(state.phase) || state.phase <= -1.f || state.phase >= float(state.frameCount)) return false;
    // 29AFDC/29B028 EE CVT.W.S truncates toward zero. A source reverse-end
    // overshoot can leave phase in (-1,0); its selected frame is still zero.
    const auto selected = static_cast<std::int32_t>(state.phase);
    if (selected < 0 || static_cast<std::uint32_t>(selected) >= state.frameCount) return false;
    frame = static_cast<std::uint32_t>(selected);
    return true;
}
// One owner per authored "unique" material instance. The page/render owner
// supplies its existing sampled uint32 application-millisecond clock and gates
// calls with its original pause policy. No host/OS/audio clock is sampled here.
// Transactional on rejection; no allocation. Preserve binary32 operations,
// source timestamp sentinel, repeat-timestamp guard, and literal branch order.
inline bool advanceOriginalUiTextureAnimation50KG(OriginalUiTextureAnimation50KG& state,
    std::uint32_t sourceMilliseconds, std::uint32_t& frame) noexcept {
    static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559,
        "Original texture animation requires IEEE binary32");
    std::uint32_t selected;
    if (std::fegetround() != FE_TONEAREST || !selectOriginalUiTextureAnimationFrame50KG(state, selected) ||
        (!state.config.reverseLoopOnEnd && state.reverseDirection)) return false;
    // Original outer controller 29AC9C..29ACB0 suppresses repeat timestamps,
    // including an initial 0xffffffff sample. Do not re-reflect a negative
    // reverse-end phase on a second draw in the same source tick.
    if (sourceMilliseconds == state.previousMilliseconds) { frame = selected; return true; }
    auto next = state;
    float seconds = 0.f;
    if (next.previousMilliseconds != 0xffffffffu) {
        const std::uint32_t delta = sourceMilliseconds - next.previousMilliseconds;
        float milliseconds;
        if (delta & 0x80000000u) {
            volatile float half = static_cast<float>(static_cast<std::int32_t>((delta >> 1) | (delta & 1u)));
            volatile float doubled = half + half; milliseconds = doubled;
        } else {
            volatile float converted = static_cast<float>(static_cast<std::int32_t>(delta));
            milliseconds = converted;
        }
        const std::uint32_t scaleWord = 0x3a83126fu; float scale;
        std::memcpy(&scale, &scaleWord, sizeof(scale));
        volatile float scaled = scale * milliseconds; seconds = scaled;
    }
    next.previousMilliseconds = sourceMilliseconds;
    volatile float advance = seconds * next.config.framesPerSecond;
    volatile float phase = next.reverseDirection ? next.phase - advance : next.phase + advance;
    next.phase = phase;
    const float count = static_cast<float>(next.frameCount);
    if (next.phase >= count) {
        const float remainder = std::fmod(next.phase, count);
        if (next.config.reverseLoopOnEnd) {
            volatile float reflected = static_cast<float>(next.frameCount - 1) - remainder;
            next.phase = reflected;
            next.reverseDirection = true;
        } else if (next.config.loopOnEndToFrame != -1) {
            const float loopTo = static_cast<float>(next.config.loopOnEndToFrame);
            const float loopSpan = static_cast<float>(
                static_cast<std::int32_t>(next.frameCount) - next.config.loopOnEndToFrame);
            const float loopRemainder = std::fmod(remainder, loopSpan);
            volatile float looped = loopTo + loopRemainder;
            next.phase = looped;
        } else next.phase = remainder;
        if (!next.config.looping) next.stopped = true;
    } else if (next.phase < 0.f) {
        // Original 29A27C..29A2C8 is an ELSE branch: a negative value just
        // produced by the upper-end branch is not reflected again this tick.
        next.phase = std::fabs(std::fmod(next.phase, count));
        next.reverseDirection = false;
    }
    if (next.stopped) next.phase = static_cast<float>(next.frameCount - 1);
    if (!selectOriginalUiTextureAnimationFrame50KG(next, selected)) return false;
    state = next; frame = selected;
    return true;
}
}
