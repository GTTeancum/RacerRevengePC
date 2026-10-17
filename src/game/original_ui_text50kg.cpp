#include "original_ui_text50kg.h"
#include <cmath>

namespace rr::game {
namespace {
bool fail(OriginalUiTextDiagnostic50KG& d, OriginalUiTextError50KG error,
          std::uint32_t pc, std::size_t character = 0,
          std::int32_t ascii = -1) noexcept {
    d = {error, character, ascii, pc};
    return false;
}
bool finite4(float a, float b, float c, float d) noexcept {
    return std::isfinite(a) && std::isfinite(b) &&
           std::isfinite(c) && std::isfinite(d);
}
}

bool layoutOriginalUiText50KG(const OriginalUiTextInput50KG& input,
    OriginalUiTextLayout50KG& output, OriginalUiTextDiagnostic50KG& diagnostic) noexcept {
    diagnostic = {};
    if (!input.font || !input.font->decoded())
        return fail(diagnostic, OriginalUiTextError50KG::missingCel, 0x2817D0u);
    if (!input.atlas)
        return fail(diagnostic, OriginalUiTextError50KG::missingAtlas, 0x281F90u);
    const auto& atlas = *input.atlas;
    if (atlas.width == 0 || atlas.height == 0 ||
        atlas.width > assets::max_texture_dimension ||
        atlas.height > assets::max_texture_dimension ||
        atlas.rgba.size() != std::size_t(atlas.width) * atlas.height * 4u)
        return fail(diagnostic, OriginalUiTextError50KG::atlasShape, 0x2E0A88u);
    // Dimensions publishes output only after every layout check succeeds.
    if (!layoutOriginalUiTextDimensions50LA(input,atlas.width,atlas.height,output,diagnostic))return false;
    output.atlas=input.atlas;return true;
}

bool layoutOriginalUiTextDimensions50LA(const OriginalUiTextInput50KG& input,
    std::uint32_t atlasWidth,std::uint32_t atlasHeight,
    OriginalUiTextLayout50KG& output,OriginalUiTextDiagnostic50KG& diagnostic) noexcept {
    diagnostic={};
    if (!input.font || !input.font->decoded())
        return fail(diagnostic, OriginalUiTextError50KG::missingCel, 0x2817D0u);
    if (!atlasWidth || !atlasHeight || atlasWidth>assets::max_texture_dimension ||
        atlasHeight>assets::max_texture_dimension)
        return fail(diagnostic, OriginalUiTextError50KG::atlasShape, 0x2E0A88u);
    if ((!input.text && input.length != 0) ||
        !std::isfinite(input.localX) || !std::isfinite(input.localY))
        return fail(diagnostic, OriginalUiTextError50KG::input, 0x2E0A94u);
    if (input.length > originalUiTextCapacity50KG)
        return fail(diagnostic, OriginalUiTextError50KG::textCapacity, 0x2E0F00u);

    const float width = static_cast<float>(atlasWidth);
    const float height = static_cast<float>(atlasHeight);
    // Source multiplies UV by integer (dimension-1), then divides by dimension.
    // Retain those two operations instead of precomputing a scale factor.
    const float widthMinusOne = static_cast<float>(atlasWidth - 1u);
    const float heightMinusOne = static_cast<float>(atlasHeight - 1u);
    OriginalUiTextLayout50KG next;
    next.atlas = nullptr; // CPU dimensions are not a decoded-image receipt.
    next.endLocalX = input.localX;
    if (const auto* glyph = input.font->firstGlyph()) {
        const float uvHeight = glyph->bottom - glyph->top;
        next.fontLineHeight = height * uvHeight;
        if (!std::isfinite(next.fontLineHeight))
            return fail(diagnostic, OriginalUiTextError50KG::arithmetic, 0x2817A8u);
    }
    bool pixelMode = false;
    float measured = 0;
    float pen = input.localX;
    for (std::size_t i = 0; i < input.length; ++i) {
        const std::uint8_t ascii = input.text[i];
        const auto* g = input.font->glyph(ascii);
        if (!g)
            return fail(diagnostic, OriginalUiTextError50KG::missingGlyph,
                        0x2E0B18u, i, ascii);
        if (i == 0) pixelMode = g->hasPixelMetrics;
        else if (pixelMode != g->hasPixelMetrics)
            return fail(diagnostic, OriginalUiTextError50KG::mixedMetricRun,
                        0x2E18A8u, i, ascii);
        float left = 0, top = 0, right = 0, bottom = 0;
        float u0 = g->left, v0 = g->top, u1 = g->right, v1 = g->bottom;
        if (pixelMode) {
            // CEL metrics store extent minus one: -1 is a genuine zero-area
            // glyph. Keep its authored bearing/advance and degenerate corners;
            // only values below -1 describe a negative extent.
            if (g->widthMinusOne < -1 || g->heightMinusOne < -1)
                return fail(diagnostic, OriginalUiTextError50KG::glyphGeometry,
                            0x2E0B2Cu, i, ascii);
            // Source 0x2E0B2C..0x2E0BE4 and 0x2E0D00..0x2E0D14.
            pen = pen + static_cast<float>(g->leftBearing);
            left = pen - 0.5f;
            top = input.localY - 0.5f;
            right = left + static_cast<float>(int(g->widthMinusOne) + 1);
            bottom = top + static_cast<float>(int(g->heightMinusOne) + 1);
            u0 = (u0 * widthMinusOne) / width;
            u1 = (u1 * widthMinusOne) / width;
            v0 = (v0 * heightMinusOne) / height;
            v1 = (v1 * heightMinusOne) / height;
            pen = pen + static_cast<float>(g->advanceAfterBearing);
            measured = measured + static_cast<float>(
                int(g->leftBearing) + int(g->advanceAfterBearing));
            if (i == 0)
                next.firstGlyphHeightBeforeScaleAndRounding =
                    static_cast<float>(int(g->heightMinusOne) + 1);
        } else {
            // Actual original older CEL format, 0x2E0D18..0x2E0E68.
            const float uvWidth = g->right - g->left;
            const float uvHeight = g->bottom - g->top;
            left = pen;
            top = input.localY;
            const float advance = width * uvWidth;
            const float glyphHeight = height * uvHeight;
            pen = pen + advance;
            right = pen - 1.0f;
            bottom = top + glyphHeight;
            measured = measured + uvWidth;
            if (i == 0) next.firstGlyphHeightBeforeScaleAndRounding = glyphHeight;
        }
        if (!finite4(left, top, right, bottom) || !finite4(u0, v0, u1, v1) ||
            !std::isfinite(pen) || !std::isfinite(measured))
            return fail(diagnostic, OriginalUiTextError50KG::arithmetic,
                        0x2E0A30u, i, ascii);
        if (right < left || bottom < top)
            return fail(diagnostic, OriginalUiTextError50KG::glyphGeometry,
                        0x2E0DE8u, i, ascii);
        auto& quad = next.quads[i];
        quad.ascii = ascii;
        quad.hasPixelMetrics = pixelMode;
        quad.vertices[0] = {left, top, u0, v0};
        quad.vertices[1] = {right, top, u1, v0};
        quad.vertices[2] = {right, bottom, u1, v1};
        quad.vertices[3] = {left, bottom, u0, v1};
    }
    next.count = static_cast<std::uint16_t>(input.length);
    next.endLocalX = pen;
    next.widthBeforeScaleAndRounding = pixelMode ? measured : measured * width;
    if (!std::isfinite(next.widthBeforeScaleAndRounding) ||
        !std::isfinite(next.firstGlyphHeightBeforeScaleAndRounding))
        return fail(diagnostic, OriginalUiTextError50KG::arithmetic, 0x2E18C8u);
    output = next;
    return true;
}
}
