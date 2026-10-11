#pragma once
#include "material_texture_gl50eb.h"
#include "../../../src/assets/native_asset48b.h"
#include <cstdint>

namespace rr::platform {
// Ordered, authored 2D UI draw data. The source screen/widget owner supplies
// all geometry, text, material state and navigation. This interface neither
// substitutes a screen nor generates glyphs from a host font.
inline constexpr std::uint32_t originalUiMaximumQuads50KG = 8192;
inline constexpr std::uint32_t originalUiMaximumSpans50KG = 2048;
enum class OriginalUiBlend50KG : std::uint8_t {
    opaque, sourceAlpha, additive, destinationAlphaAdditive, fixed255Additive
};
enum class OriginalUiMaterialMode50KG : std::uint8_t {
    unspecified, texturedModulate, untexturedVertexColor,
    // Actual Main ParticleCode: MODULATE, alpha GREATER 0, source fog.
    mainParticle,
    // Actual DefaultMT Diffuse identities. CPU owner supplies original packet
    // RGB, source fixed-point XY/Z and perspective W; no generic relighting.
    sourceDiffuse, sourceDiffuseCompressed, sourceSpecularMapCompressed
};
struct OriginalUiSpan50KG {
    // Contiguous quad partition in the caller's source-resolved draw order.
    // Textured/untextured spans and repeated images are never regrouped here.
    // Each vertex carries real source XY/RGBA and textured spans' authored UV.
    std::uint32_t firstQuad = 0, quadCount = 0;
    std::uint32_t textureIndex = assets::unresolvedMaterialTexture50EA;
    OriginalUiBlend50KG blend = OriginalUiBlend50KG::sourceAlpha;
    // Explicit decoded material policy. No alpha reinterpretation is inferred.
    float alphaScale = 1.f;
    // Explicit source material modulation normalization; packet bytes remain exact.
    float colorScale = 1.f;
    // Explicit source admission, never inferred from a missing texture. The
    // untextured mode requires the unresolved index; it samples no image.
    OriginalUiMaterialMode50KG materialMode = OriginalUiMaterialMode50KG::unspecified;
};
struct OriginalUiDraw50KG {
    // Exact published original texture bank; its owner outlives this call.
    // Raw native GLuints and fallback textures cannot enter the interface.
    // An all-untextured renderer may omit both bank and publication (null/0).
    // Mixed draws retain the same authenticated bank as their textured spans.
    const MaterialTextureGl50EB* textures = nullptr;
    std::uint64_t texturePublication = 0;
    const assets::Vertex48B* vertices = nullptr; // four vertices per quad
    std::uint32_t quadCount = 0;
    const OriginalUiSpan50KG* spans = nullptr;
    std::uint32_t spanCount = 0;
    // Source canvas in pixels, positive X right and positive Y down. Authored
    // coordinates and CEL half-pixel offsets arrive unchanged. Native viewport
    // adaptation scales uniformly and letterboxes; it never stretches artwork.
    float canvasWidth = 0.f, canvasHeight = 0.f;
    float clearRgba[4] = {0.f,0.f,0.f,1.f};
    // Pause/results overlay can retain the exact last published race image.
    // Startup uses the supplied original UI background geometry instead.
    bool retainRaceImage = false;
};
}
