#pragma once
#include "../assets/original_ui_icon50kg.h"
#include "original_ui_main_layout50kg.h"
#include "../assets/native_asset48b.h"

namespace rr::game {
enum class OriginalUiIconLayoutStatus50KG : std::uint8_t {
    complete,missingOwner,atlas,rectangle,depth,needsRotation,arithmetic
};
struct OriginalUiIconLayout50KG {
    OriginalUiOverlayQuad50KG quad;
    assets::OriginalUiIconRect50KG partialRect,mapRect;
    // Exact source packet Z after2ACA44. It must participate in the actual UI
    // depth/order owner, not be silently replaced by generic draw order.
    float packetDepth=0;
};
// Source Icon2AC590 binds actual ScreenTexture dimensions before rect defaults.
// actualAtlas is the initial material texture read by2DF180, not an arbitrary
// later ATM frame. Source reciprocal dimensions persist from that cold binding.
// This cold geometry admits the source's unrotated branch only (Angle<=0).
// Positive Angle needs the real sin/cos/pivot owner and is not erased.
// videoDepthMaximum is the actual source video owner+60; negative authored
// Depth uses its original max-300 special case. No fixed depth maximum guessed.
OriginalUiIconLayoutStatus50KG layoutOriginalUiIcon50KG(
    const assets::OriginalUiIcon50KG&,const assets::Texture48B& actualAtlas,
    std::int32_t videoDepthMaximum,OriginalUiIconLayout50KG&) noexcept;
}
