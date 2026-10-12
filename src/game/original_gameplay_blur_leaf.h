#pragma once
#include <array>
#include <cstdint>

namespace rr::game {

// SOURCE-RECOVERED LEAF CANDIDATE, NOT WIRED OR EXECUTED.
// Original ELF SHA256:
// c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30
// This leaf intentionally has no camera/speed/boost activation, scene scheduler,
// framebuffer capture, UV sampler, GL code, or image-history lifecycle.
// Call only with requests from a separately recovered original effect producer.

struct GameplayBlurRect {
    std::int32_t left = 0, top = 0, right = 0, bottom = 0;
};

struct GameplayBlurPending {
    std::array<std::int32_t, 4> strength{}; // Manager +0x78+4*view.
    std::array<bool, 4> requested{};         // Manager +0x88+4*view == 1.
};

struct GameplayBlurVideoView {
    std::uint8_t fixed = 0;                 // Video +0x494+physicalView.
    GameplayBlurRect rectangle{};           // Video +0x4A4+16*physicalView.
};

struct GameplayBlurVideoState {
    std::array<GameplayBlurVideoView, 4> view{};
};

enum class GameplayBlurRequestResult {
    queued,
    already_pending,
    unavailable_view
};

// 0x2AED10..0x2AED50. First request wins until consumed; even an ignored later
// request returns success in the original. This enum exposes that distinction.
// Explicit view -1 selects the application's current logical render view.
// Rejecting out-of-range views is a native admission, not an original check.
inline GameplayBlurRequestResult requestGameplayBlur(
    GameplayBlurPending& owner, std::int32_t strength,
    std::int32_t explicitView, std::int32_t currentLogicalView) noexcept {
    const std::int32_t selected = explicitView == -1 ? currentLogicalView : explicitView;
    if (selected < 0 || selected >= 4) return GameplayBlurRequestResult::unavailable_view;
    const auto index = static_cast<std::uint32_t>(selected);
    if (owner.requested[index]) return GameplayBlurRequestResult::already_pending;
    owner.requested[index] = true;
    owner.strength[index] = strength;
    return GameplayBlurRequestResult::queued;
}

// 0x262504..0x26255C: signed clamp0..100, convert to binary32, divide by100,
// multiply by128, unsigned truncation0x310F38, store low byte.
// For this closed101-integer domain the exact rational floor agrees with the
// source float sequence: nonintegral results are >=0.04 from an integer and
// float rounding error is <0.00002; multiples25 are represented exactly.
inline std::uint8_t gameplayBlurFixedFromStrength(std::int32_t strength) noexcept {
    if (strength < 0) strength = 0;
    if (strength > 100) strength = 100;
    return static_cast<std::uint8_t>((strength * 32) / 25);
}

enum class GameplayBlurConsumeResult {
    consumed,
    no_request,
    unavailable_view,
    unavailable_rectangle
};

// 0x2AEACC..0x2AEAF8 ->0x2623E0, neutral-color a3=-1 and implicit active
// camera rectangle. This candidate admits the active-camera subset: the caller
// supplies that exact rectangle. Original no-camera fallback is full video size.
// Logical and physical view identities are separate in the source. The engine
// assigns them equal at0x14042C in ordinary per-view rendering, but this API
// does not silently conflate them. Native admission requires a proper rectangle.
// Invoke at the original Render3D control-consumption phase, not simulation time.
inline GameplayBlurConsumeResult consumeGameplayBlur(
    GameplayBlurPending& pending, std::int32_t logicalView,
    std::int32_t physicalView, GameplayBlurRect cameraRectangle,
    GameplayBlurVideoState& video) noexcept {
    if (logicalView < 0 || logicalView >= 4 || physicalView < 0 || physicalView >= 4)
        return GameplayBlurConsumeResult::unavailable_view;
    const auto logical = static_cast<std::uint32_t>(logicalView);
    if (!pending.requested[logical]) return GameplayBlurConsumeResult::no_request;
    if (cameraRectangle.left >= cameraRectangle.right || cameraRectangle.top >= cameraRectangle.bottom)
        return GameplayBlurConsumeResult::unavailable_rectangle;
    auto& destination = video.view[static_cast<std::uint32_t>(physicalView)];
    destination.fixed = gameplayBlurFixedFromStrength(pending.strength[logical]);
    destination.rectangle = cameraRectangle;
    pending.requested[logical] = false;
    return GameplayBlurConsumeResult::consumed;
}

// 0x2AE6A8..0x2AE6BC calls video vtable+0x50 =0x25B9D0. It clears all
// four strength bytes only. Rectangle and prior image contents are untouched.
inline void clearGameplayBlurVideoStrengths(GameplayBlurVideoState& video) noexcept {
    for (auto& channel : video.view) channel.fixed = 0;
}

struct GameplayBlurRgb {
    std::uint8_t red = 0, green = 0, blue = 0;
};

// ALPHA_1=(FIX<<32)|0x64 at0x25E088..0x25E098,0x25E184..0x25E18C:
// (Cs-Cd)*FIX/128+Cd. Neutral MODULATE RGB128 leaves source sample RGB intact.
// Inputs are already resolved source/destination color bytes, not normalized
// floats. Signed floor follows the existing reviewed native GS byte arithmetic.
// This does not establish raster sample selection, target alpha, or FRAME masks.
inline std::int32_t gameplayBlurSignedFloor128(std::int32_t product) noexcept {
    return product >= 0 ? product / 128 : -((-product + 127) / 128);
}

inline bool blendGameplayBlurRgb(
    GameplayBlurRgb sourceSample, GameplayBlurRgb destination,
    std::uint8_t fixed, GameplayBlurRgb& output) noexcept {
    if (fixed > 128) return false; // Native boundary of setter's admitted range.
    const auto channel = [fixed](std::uint8_t source, std::uint8_t dest) noexcept {
        const std::int32_t difference = static_cast<std::int32_t>(source) - dest;
        const std::int32_t result = dest + gameplayBlurSignedFloor128(difference * fixed);
        return static_cast<std::uint8_t>(result); // Convex blend remains0..255.
    };
    output = {channel(sourceSample.red, destination.red),
              channel(sourceSample.green, destination.green),
              channel(sourceSample.blue, destination.blue)};
    return true;
}

} // namespace rr::game
