#pragma once
#include "original_ui_draw50kg.h"

namespace rr::platform {
// NEW reconstruction, not recovered historical bytes. No implicit material
// defaults: caller joins authenticated material, packet-depth and pass owners.
// Each span must first pass prepareOriginalUiNativeMaterial50KG (textured) or
// prepareOriginalUiNativeUntexturedMaterial50KG (zero-map vertex color).
// The caller must preserve its blend/depth policy and any exact sampler,
// resolve pass-group/animation enablement, and exclude every rejected source
// state. This renderer cannot infer those owners from a texture-bank identity.
enum class OriginalUiDepth50KG : std::uint8_t { unspecified, never, always, greaterEqual, greater };
struct OriginalUiNativeSpan50KG {
    OriginalUiDepth50KG depth = OriginalUiDepth50KG::unspecified;
    bool depthWrite = false;
    // Exact source policy per textured span, even when images repeat. The renderer
    // verifies actual immutable texture parameters before target writes;
    // conflicts reject instead of inheriting the first image's sampler.
    // Untextured spans must leave every sampler field unspecified. No texture
    // is bound as a surrogate and no irrelevant sampler policy is invented.
    MaterialTextureSampler50EB sampler{};
    // Only the two explicit source Diffuse modes consume these fields. The
    // material owner supplies FGE and the admitted GREATER/AREF0/KEEP test.
    bool sourceFogEnabled = false;
    bool sourceAlphaTestGreaterZero = false;
};
struct OriginalUiNativeDraw50KG {
    OriginalUiDraw50KG draw{};
    const OriginalUiNativeSpan50KG* states = nullptr;
    std::uint32_t stateCount = 0; // Exactly one per ordered draw span.
    // Must explicitly prove no destination-alpha test/factor or downstream
    // consumer expects source framebuffer alpha. This presentation target is
    // cleared to alpha 1 and RGB-only writes preserve that value. RGBA readback
    // is presentation data, NOT source framebuffer-alpha recovery.
    bool opaquePresentationAlphaOnly = false;
    // Source fog byte /256, parallel to all draw vertices. Required for Main
    // particles or a fog-enabled Diffuse span. Main admits 0..254, Diffuse
    // 0..255. Unused slots may be 0; per-span policy controls consumption.
    // This is a separate native stream, never a fabricated texture channel.
    const float* particleFog = nullptr;
    std::uint32_t particleFogCount = 0;
    float particleFogColor[3] = {0.f,0.f,0.f}; // authored source RGB /255
    // Original pre-divide projected W, parallel to all vertices. Required for
    // Diffuse/Compressed spans; consumed only by those modes. Ordinary screen
    // and particle spans retain W=1. Preserves source perspective ST/Q while
    // packet RGB and fog remain screen-linear. No reconstruction from depth.
    const float* sourceRasterW = nullptr;
    std::uint32_t sourceRasterWCount = 0;
    // Vertex position[2] is the original integer packet Z in [0,16777215].
    // The vertex shader normalizes it once into native D24 reverse depth.
    // XY stays on the 640x448 source canvas. UI corners are TL,TR,BR,BL;
    // particle corners BL,TL,TR,BR preserve the source strip's BL--TR diagonal.
    // texturedModulate: colorScale=255/128, blended alphaScale=(255/128)^2.
    // untexturedVertexColor: colorScale=1, blended alphaScale=255/128.
    // Opaque spans retain alphaScale=1 (source framebuffer alpha is not stored).
    // Both modes retain raw vertex alpha <=128 as a bounded native admission.
    // Blended textured spans additionally require each vertex alpha times the
    // real texture's maximum alpha <=16384. Full-byte atlases (including actual
    // Main alpha255) are valid with source packet alpha63/64. This joint bound
    // prevents native blend-factor clamping from hiding a source factor >1.
};
}
