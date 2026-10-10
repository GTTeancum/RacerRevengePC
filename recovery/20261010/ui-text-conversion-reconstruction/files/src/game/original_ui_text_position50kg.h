#pragma once
#include "../assets/original_ui_overlay_text50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
struct OriginalUiTextPositionInput50KG {
    const assets::OriginalUiOverlayText50KG* widget=nullptr;
    std::size_t line=0;
    float fontLineHeight=0;
    // Source ScreenText::GetWidth/GetHeight integer results, after source
    // metric truncation. Font layout's unconverted values are not interchangeable.
    std::int32_t measuredWidth=0,measuredHeight=0;
};
struct OriginalUiTextPosition50KG {
    std::int32_t authoredLineX=0,authoredLineY=0;
    float positionedX=0,positionedY=0;
    // Creation 0x29DA40 initializes scale=1, rotation=0. These positioned
    // coordinates may therefore enter the glyph leaf as local coordinates
    // for a settled, unanimated original main text line. Animated transforms
    // must be separately source-updated; this function does not erase them.
};
// Source GetWidth/GetHeight final multiply and CVT.W.S truncation toward zero.
// The converted product must be finite and int32-representable; out-of-domain
// inputs are rejected, not source-saturated. Conversion does not use host fenv.
bool roundOriginalUiTextMetric50KG(float beforeScale,float sourceScale,
    std::int32_t& output) noexcept;
// Line leading 0x2A9684/0x2A98A8/0x2A9D20 and justification 0x2E13A0.
// Explicit Height only affects the localized/split branch (0x2A9CD4).
// CVT.W.S conversion truncates toward zero in the finite int32 domain;
// out-of-domain inputs are rejected before casting. Never host rounding mode.
bool positionOriginalUiText50KG(const OriginalUiTextPositionInput50KG&,
    OriginalUiTextPosition50KG&) noexcept;
// Independent ScreenText packet route: 29DA10->29DCB0 stores packed color;
// 29D484/29D6C8 ->133F20 extracts RGB and shifts each right one. Alpha
// 29D4C4/29D70C converts float to signed16, then1D8850 arithmetic>>2,
// masked to byte. Input here is the already-converted source signed16 alpha,
// not authored Alpha nor an assumed final opacity. This preserves the exact
// short-width contract; normal clamped0..255 yields0..63. GS/material
// normalization belongs to the renderer and is not hidden in these bytes.
std::array<std::uint8_t,4> originalUiTextPacketColor50KG(
    std::uint32_t sourcePackedColor,std::int16_t sourceAlpha) noexcept;
}
