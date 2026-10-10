#include "original_ui_text_position50kg.h"
#include <cmath>
#include <limits>

namespace rr::game {
namespace {
bool truncateToInt32(float value,std::int32_t& out) noexcept {
    if (!std::isfinite(value)) return false;
    // EE CVT.W.S truncates toward zero. This native path admits only finite,
    // representable inputs; rejection outside that domain is not EE saturation.
    if (double(value)<double(std::numeric_limits<std::int32_t>::min()) ||
        double(value)>double(std::numeric_limits<std::int32_t>::max())) return false;
    out=static_cast<std::int32_t>(value);return true;
}
// Keep source mul.s/add.s as separate host float operations; forbid contraction.
// This materialization is not a claim of all-input EE floating-point parity.
float mul(float a,float b) noexcept { volatile float f=a*b;return f; }
float add(float a,float b) noexcept { volatile float f=a+b;return f; }
}
bool roundOriginalUiTextMetric50KG(float beforeScale,float scale,std::int32_t& output) noexcept {
    if (!std::isfinite(beforeScale) || !std::isfinite(scale)) return false;
    std::int32_t result=0;if (!truncateToInt32(mul(beforeScale,scale),result)) return false;
    output=result;return true;
}
bool positionOriginalUiText50KG(const OriginalUiTextPositionInput50KG& input,
    OriginalUiTextPosition50KG& output) noexcept {
    if (!input.widget || input.line>=input.widget->lines.size() ||
        !std::isfinite(input.fontLineHeight) || input.measuredWidth<0 || input.measuredHeight<0) return false;
    const auto& w=*input.widget;OriginalUiTextPosition50KG result;
    result.authoredLineX=w.x;
    if (w.usesExplicitHeight) {
        const auto y=std::int64_t(w.y)+std::int64_t(input.line)*w.height;
        if (y<std::numeric_limits<std::int32_t>::min() || y>std::numeric_limits<std::int32_t>::max()) return false;
        result.authoredLineY=static_cast<std::int32_t>(y);
    } else {
        const float advance=mul(w.leadingFactor,input.fontLineHeight);
        const float distance=mul(float(input.line),advance);
        if (!truncateToInt32(add(float(w.y),distance),result.authoredLineY)) return false;
    }
    result.positionedX=float(result.authoredLineX);result.positionedY=float(result.authoredLineY);
    if (!w.lines[input.line].empty()) {
        const auto j=w.justification;std::int32_t coordinate=0;
        if (j==assets::OriginalUiTextJustification50KG::centerHorizontal ||
            j==assets::OriginalUiTextJustification50KG::centerBoth) {
            if (!truncateToInt32(add(result.positionedX,-mul(float(input.measuredWidth),0.5f)),coordinate)) return false;
            result.positionedX=float(coordinate);
        } else if (j==assets::OriginalUiTextJustification50KG::right) {
            if (!truncateToInt32(add(result.positionedX,-float(input.measuredWidth)),coordinate)) return false;
            result.positionedX=float(coordinate);
        }
        if (j==assets::OriginalUiTextJustification50KG::centerVertical ||
            j==assets::OriginalUiTextJustification50KG::centerBoth) {
            if (!truncateToInt32(add(result.positionedY,-mul(float(input.measuredHeight),0.5f)),coordinate)) return false;
            result.positionedY=float(coordinate);
        }
    }
    output=result;return true;
}
std::array<std::uint8_t,4> originalUiTextPacketColor50KG(std::uint32_t color,
    std::int16_t alpha) noexcept {
    // Division with explicit floor reproduces arithmetic shift even for the
    // signed-short negative domain without relying on implementation shifts.
    const std::int32_t a=alpha;const auto shifted=a>=0?a/4:-((-a+3)/4);
    return {{std::uint8_t(((color>>16)&255u)>>1),std::uint8_t(((color>>8)&255u)>>1),
        std::uint8_t((color&255u)>>1),std::uint8_t(std::uint32_t(shifted)&255u)}};
}
}
