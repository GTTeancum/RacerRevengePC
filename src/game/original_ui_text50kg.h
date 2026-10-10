#pragma once
#include "../assets/original_ui_font50kg.h"
#include "../assets/native_asset48b.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Original ScreenText::characterCount is read with lbu at 0x2E0F00.
constexpr std::size_t originalUiTextCapacity50KG = 255;
struct OriginalUiTextVertex50KG { float x = 0, y = 0, u = 0, v = 0; };
struct OriginalUiTextQuad50KG {
    // Top-left, top-right, bottom-right, bottom-left. These expand the source's
    // two opposite sprite corners; there is no host atlas remap or Y flip.
    std::array<OriginalUiTextVertex50KG, 4> vertices{};
    std::uint8_t ascii = 0;
    bool hasPixelMetrics = false;
};
struct OriginalUiTextInput50KG {
    const assets::OriginalUiFont50KG* font = nullptr;
    // Must be the genuine decoded same-name PST atlas. This module neither
    // decodes PST nor invents a texture. Its lifetime covers all output use.
    const assets::Texture48B* atlas = nullptr;
    const std::uint8_t* text = nullptr;
    std::size_t length = 0;
    // Source local coordinates entering 0x2E0A30, after any positioning and
    // inverse-scale step. Text scale, rotation and justification are outside
    // this leaf. A caller must not silently treat screen coordinates as local.
    float localX = 0, localY = 0;
};
struct OriginalUiTextLayout50KG {
    std::array<OriginalUiTextQuad50KG, originalUiTextCapacity50KG> quads{};
    std::uint16_t count = 0;
    const assets::Texture48B* atlas = nullptr;
    float endLocalX = 0;
    // Pre-scale, pre-cvt.w.s values of 0x2E16F0 and 0x2E15B0. Width uses
    // source summation order, not bounding-box width or last corner position.
    float widthBeforeScaleAndRounding = 0;
    float firstGlyphHeightBeforeScaleAndRounding = 0;
    // 0x281730: first present ASCII slot's UV height * real atlas height.
    // This source line-leading metric differs from the first text glyph's
    // optional pixel height. Empty font has the original -1 sentinel.
    float fontLineHeight = -1;
};
enum class OriginalUiTextError50KG : std::uint8_t {
    none, missingCel, missingAtlas, atlasShape, input, textCapacity,
    missingGlyph, mixedMetricRun, glyphGeometry, arithmetic
};
struct OriginalUiTextDiagnostic50KG {
    OriginalUiTextError50KG error = OriginalUiTextError50KG::none;
    std::size_t character = 0;
    std::int32_t ascii = -1;
    std::uint32_t sourcePC = 0;
};

// Bounded native translation of glyph setup 0x2E0A30..0x2E0F0C, plus
// unrounded measurement operands. Byte text is preserved verbatim: no UTF-8
// decoding, invented glyph, newline, tab, kerning or fallback policy.
// Missing glyphs reject the whole run rather than hitting the guest assertion.
// The CEL loader allows partial pixel-metric suffixes. Aggregate width's final
// mode depends on its last visited glyph (0x2E18A8), so this strict native
// contract rejects runs mixing those two modes instead of guessing a width.
// The actual original UV-only CEL branch IS supported when all used glyphs
// lack pixel metrics. It always requires the original decoded atlas.
// Fixed storage, no allocation, IO, renderer calls, guest memory or execution.
// Failure preserves output. Input/output/diagnostic storage must be distinct.
bool layoutOriginalUiText50KG(const OriginalUiTextInput50KG& input,
    OriginalUiTextLayout50KG& output, OriginalUiTextDiagnostic50KG& diagnostic) noexcept;
}
