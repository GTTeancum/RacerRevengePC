#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::assets {
constexpr std::size_t originalUiFontGlyphCapacity50KG = 256;
constexpr std::size_t maxOriginalUiCelBytes50KG = 8 + 256 * (20 + 12);

// Native values decoded from an ORIGINAL <FontName>.cel. Disk order is
// ASCII, left, top, right, bottom; the original heap object reorders the UVs.
struct OriginalUiGlyph50KG {
    float left = 0, top = 0, right = 0, bottom = 0;
    // CEL's optional signed four-short record, in exact disk order. Names
    // describe their uses in SLUS_202.68 0x2E0A30, not inferred typography.
    std::int16_t leftBearing = 0;
    std::int16_t widthMinusOne = 0;
    std::int16_t advanceAfterBearing = 0;
    std::int16_t heightMinusOne = 0;
    bool hasPixelMetrics = false;
};

enum class OriginalUiCelError50KG : std::uint8_t {
    none, missingCel, input, byteBudget, truncated, glyphCount, asciiRange,
    duplicateGlyph, nonFiniteUv, reversedUv, metricCount, duplicateMetric,
    metricWithoutGlyph, trailingBytes
};
struct OriginalUiCelDiagnostic50KG {
    OriginalUiCelError50KG error = OriginalUiCelError50KG::none;
    std::size_t byteOffset = 0;
    std::uint32_t record = 0;
    std::int32_t ascii = -1;
};

class OriginalUiFont50KG final {
public:
    bool decoded() const noexcept { return decoded_; }
    std::uint16_t glyphCount() const noexcept { return glyphCount_; }
    std::uint16_t metricCount() const noexcept { return metricCount_; }
    bool hasMetricSection() const noexcept { return hasMetricSection_; }
    const OriginalUiGlyph50KG* glyph(std::uint8_t ascii) const noexcept {
        return decoded_ && present_[ascii] ? &glyphs_[ascii] : nullptr;
    }
private:
    friend bool decodeOriginalUiCel50KG(const std::uint8_t*, std::size_t,
        OriginalUiFont50KG&, OriginalUiCelDiagnostic50KG&) noexcept;
    std::array<OriginalUiGlyph50KG, originalUiFontGlyphCapacity50KG> glyphs_{};
    std::array<bool, originalUiFontGlyphCapacity50KG> present_{};
    std::uint16_t glyphCount_ = 0, metricCount_ = 0;
    bool decoded_ = false, hasMetricSection_ = false;
};

// Source: original font load 0x2817D0..0x281F84. Little-endian u32 count,
// count * {i32 ASCII, f32 left, top, right, bottom}; optional i32 count,
// count * {i32 ASCII, i16 bearing, widthMinusOne, advanceAfterBearing,
// heightMinusOne}. No magic, version, atlas bytes, or guessed ASCII table.
// Strict native envelope rejects duplicate/out-of-domain records rather than
// reproducing guest assertions, overwrite leaks, or unchecked reads. It does
// not assume UVs are in [0,1]: the source asserts only ordered rectangles.
// No allocation or IO. Failure preserves the existing font in full. Source
// storage and output/diagnostic must be distinct, with valid stable extents.
// Decoded metrics alone do NOT make a font renderable: resolve and decode the
// original same-name PST atlas, including its real dimensions and pixels.
bool decodeOriginalUiCel50KG(const std::uint8_t* bytes, std::size_t size,
    OriginalUiFont50KG& font, OriginalUiCelDiagnostic50KG& diagnostic) noexcept;
}
