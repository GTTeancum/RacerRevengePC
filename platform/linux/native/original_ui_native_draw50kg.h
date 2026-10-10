#pragma once
#include "original_ui_draw50kg.h"

namespace rr::platform {
// NEW reconstruction, not recovered historical bytes. No implicit material
// defaults: caller joins authenticated material, packet-depth and pass owners.
// Each span must first pass the recovered prepareOriginalUiNativeMaterial50KG
// admission. The caller must preserve its blend/depth policy and exact sampler,
// resolve pass-group/animation enablement, and exclude every rejected source
// state. This renderer cannot infer those owners from a texture-bank identity.
enum class OriginalUiDepth50KG : std::uint8_t { unspecified, never, always, greaterEqual, greater };
struct OriginalUiNativeSpan50KG {
    OriginalUiDepth50KG depth = OriginalUiDepth50KG::unspecified;
    bool depthWrite = false;
    // Exact source policy per span, even when images repeat. The renderer
    // verifies actual immutable texture parameters before target writes;
    // conflicts reject instead of inheriting the first image's sampler.
    MaterialTextureSampler50EB sampler{};
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
    // Vertex position[2] is the original integer packet Z in [0,16777215].
    // The vertex shader normalizes it once into native D24 reverse depth.
    // XY stays on the 640x448 source canvas; corners remain TL,TR,BR,BL.
    // colorScale must be 255/128. Nonopaque alphaScale must be (255/128)^2;
    // opaque retains the recovered API's alphaScale=1 (alpha is not stored).
};
}
