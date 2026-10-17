#include "original_ui_font50kg.h"
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::assets {
namespace {
static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559,
              "Original CEL requires IEEE-754 binary32");
std::uint32_t u32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
           (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}
std::int16_t i16(const std::uint8_t* p) noexcept {
    const std::uint32_t value = std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8);
    return static_cast<std::int16_t>(value < 0x8000u ? static_cast<int>(value)
                                                   : static_cast<int>(value) - 0x10000);
}
float f32(const std::uint8_t* p) noexcept {
    const std::uint32_t bits = u32(p);
    float value = 0;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}
bool fail(OriginalUiCelDiagnostic50KG& d, OriginalUiCelError50KG error,
          std::size_t offset, std::uint32_t record = 0,
          std::int32_t ascii = -1) noexcept {
    d = {error, offset, record, ascii};
    return false;
}
}

bool decodeOriginalUiCel50KG(const std::uint8_t* bytes, std::size_t size,
    OriginalUiFont50KG& font, OriginalUiCelDiagnostic50KG& diagnostic) noexcept {
    diagnostic = {};
    if (!bytes && size != 0)
        return fail(diagnostic, OriginalUiCelError50KG::input, 0);
    if (!bytes || size == 0)
        return fail(diagnostic, OriginalUiCelError50KG::missingCel, 0);
    if (size > maxOriginalUiCelBytes50KG)
        return fail(diagnostic, OriginalUiCelError50KG::byteBudget, 0);
    if (size < 4)
        return fail(diagnostic, OriginalUiCelError50KG::truncated, 0);
    const std::uint32_t count = u32(bytes);
    if (count > originalUiFontGlyphCapacity50KG)
        return fail(diagnostic, OriginalUiCelError50KG::glyphCount, 0);
    if (count > (size - 4) / 20)
        return fail(diagnostic, OriginalUiCelError50KG::truncated, 4);
    OriginalUiFont50KG next;
    std::size_t offset = 4;
    for (std::uint32_t record = 0; record < count; ++record, offset += 20) {
        const std::uint32_t ascii = u32(bytes + offset);
        if (ascii >= originalUiFontGlyphCapacity50KG)
            return fail(diagnostic, OriginalUiCelError50KG::asciiRange, offset, record);
        if (next.present_[ascii])
            return fail(diagnostic, OriginalUiCelError50KG::duplicateGlyph,
                        offset, record, static_cast<std::int32_t>(ascii));
        auto& g = next.glyphs_[ascii];
        g.left = f32(bytes + offset + 4);
        g.top = f32(bytes + offset + 8);
        g.right = f32(bytes + offset + 12);
        g.bottom = f32(bytes + offset + 16);
        if (!std::isfinite(g.left) || !std::isfinite(g.top) ||
            !std::isfinite(g.right) || !std::isfinite(g.bottom))
            return fail(diagnostic, OriginalUiCelError50KG::nonFiniteUv,
                        offset + 4, record, static_cast<std::int32_t>(ascii));
        if (!(g.left < g.right) || !(g.top < g.bottom))
            return fail(diagnostic, OriginalUiCelError50KG::reversedUv,
                        offset + 4, record, static_cast<std::int32_t>(ascii));
        next.present_[ascii] = true;
        if (ascii < next.firstPresent_)
            next.firstPresent_ = static_cast<std::uint16_t>(ascii);
    }
    next.glyphCount_ = static_cast<std::uint16_t>(count);
    if (offset != size) {
        if (size - offset < 4)
            return fail(diagnostic, OriginalUiCelError50KG::truncated, offset);
        next.hasMetricSection_ = true;
        const std::uint32_t metricCount = u32(bytes + offset);
        if (metricCount > originalUiFontGlyphCapacity50KG)
            return fail(diagnostic, OriginalUiCelError50KG::metricCount, offset);
        offset += 4;
        if (metricCount > (size - offset) / 12)
            return fail(diagnostic, OriginalUiCelError50KG::truncated, offset);
        for (std::uint32_t record = 0; record < metricCount; ++record, offset += 12) {
            const std::uint32_t ascii = u32(bytes + offset);
            if (ascii >= originalUiFontGlyphCapacity50KG)
                return fail(diagnostic, OriginalUiCelError50KG::asciiRange, offset, record);
            if (!next.present_[ascii])
                return fail(diagnostic, OriginalUiCelError50KG::metricWithoutGlyph,
                            offset, record, static_cast<std::int32_t>(ascii));
            auto& g = next.glyphs_[ascii];
            if (g.hasPixelMetrics)
                return fail(diagnostic, OriginalUiCelError50KG::duplicateMetric,
                            offset, record, static_cast<std::int32_t>(ascii));
            g.leftBearing = i16(bytes + offset + 4);
            g.widthMinusOne = i16(bytes + offset + 6);
            g.advanceAfterBearing = i16(bytes + offset + 8);
            g.heightMinusOne = i16(bytes + offset + 10);
            g.hasPixelMetrics = true;
        }
        next.metricCount_ = static_cast<std::uint16_t>(metricCount);
    }
    if (offset != size)
        return fail(diagnostic, OriginalUiCelError50KG::trailingBytes, offset);
    next.decoded_ = true;
    font = next;
    return true;
}
}
