#pragma once
#include <cstdint>

namespace rr::platform::native {

// Native gameplay pass integrated by VehicleWindowGl50DS. Not yet built or run.
// Source: authenticated original0x25DBB0..0x25E310 and0x25B150.
// This is the original displayed-history/rectangle/fixed-blend algorithm with
// explicit host raster policies. It is not a GS emulator or bit-perfect claim.
//
// Required owner contract before draw:
// - historyImage is the exact prior displayed/completed composition, including
//   earlier blur if present. It stays alive and unchanged until publication.
// - currentImage is the completed current scene through its original Render3D
//   tail. Do not silently exclude HUD; source blur is appended after Render3D.
// - output is a third, distinct color attachment. Neither sampled texture may be
//   attached as the draw target. All are RGBA8 with cold maximum-size storage;
//   nativeViewportSize/historyViewportSize name their actual valid rectangles.
// - No host render-attempt, simulation-only tick, or failed composition advances
//   history. Real native display/publication ownership is outside this header.
// - The request must come from a recovered producer. Do not substitute velocity,
//   boost flags, all-AI field offsets, or forced footage activation.
//
// Cold-create/link program, framebuffer/texture pair, and empty VAO. Hot draw:
// full-target viewport and filled polygons; disable GL blend/depth/stencil/scissor,
// CULL_FACE, RASTERIZER_DISCARD, COLOR_LOGIC_OP, DITHER, and FRAMEBUFFER_SRGB;
// enable RGBA color writes; bind immutable inputs to units0/1; upload uniforms;
// draw one fullscreen triangle. Restore state without clearing depth. A later
// original video effect/fade still follows this pass. Renderer storage owns GL calls.
//
// Native policies, explicitly not claimed as exact inherited GS state:
// - Original source viewport/UVs are logical pixels; a source one-pixel inset
//   stays one original pixel when the native target resolution changes.
// - A resized native viewport samples the prior displayed valid extent by
//   normalized logical position. Cold-allocation padding is never sampled.
// - Native pixel-center interpolation and nearest texelFetch implement the UV
//   rectangle. Exact GS fixed-point interpolation/tie handling is not emulated.
// - RGB is resolved to8-bit values and uses signed-floor GS blend arithmetic.
// - Current native alpha is preserved. Original alpha/FBA/FRAME-mask inheritance
//   is not yet joined. This policy is suitable only for the native RGB display
//   composition; later consumers requiring original framebuffer alpha need work.

struct GameplayBlurNativePassParameters {
    std::int32_t originalWidth = 0, originalHeight = 0;
    std::int32_t left = 0, top = 0, right = 0, bottom = 0; // Top-left convention.
    std::uint8_t fixed = 0; //0x2623E0's admitted0..128 fixed coefficient.
};

inline bool admitGameplayBlurNativePass(const GameplayBlurNativePassParameters& p) noexcept {
    // Safe native admission; source itself uses assertions rather than these bounds.
    return p.originalWidth > 0 && p.originalHeight > 0 && p.fixed <= 128 &&
        p.left >= 0 && p.top >= 0 && p.right <= p.originalWidth && p.bottom <= p.originalHeight &&
        p.left < p.right && p.top < p.bottom &&
        static_cast<std::int64_t>(p.right) - p.left >= 2 &&
        static_cast<std::int64_t>(p.bottom) - p.top >= 2;
}

inline constexpr char gameplayBlurNativeVertex[] = R"GLSL(#version 150 core
void main() {
    vec2 corner=vec2(float((gl_VertexID<<1)&2),float(gl_VertexID&2));
    gl_Position=vec4(corner*2.0-1.0,0.0,1.0);
}
)GLSL";

inline constexpr char gameplayBlurNativeFragment[] = R"GLSL(#version 150 core
uniform sampler2D historyImage;
uniform sampler2D currentImage;
uniform ivec2 originalSize;
uniform ivec2 nativeViewportSize;
uniform ivec2 historyViewportSize;
uniform ivec4 originalViewport;
uniform int fixedCoefficient;
out vec4 composedColor;

int signedFloor128(int value) {
    return value>=0 ? value/128 : -((-value+127)/128);
}

void main() {
    ivec2 nativeSize=nativeViewportSize;
    ivec2 destinationPixel=ivec2(gl_FragCoord.xy);
    vec4 destination=texelFetch(currentImage,destinationPixel,0);
    // FBO textures and gl_FragCoord use bottom-left. Source camera rectangles
    // use top-left; both the destination mapping and history lookup invert Y.
    vec2 logicalPosition=vec2(gl_FragCoord.x,float(nativeSize.y)-gl_FragCoord.y)
        *vec2(originalSize)/vec2(nativeSize);
    vec2 low=vec2(originalViewport.xy);
    vec2 high=vec2(originalViewport.zw);
    if (fixedCoefficient==0 || any(lessThan(logicalPosition,low)) ||
        any(greaterThanEqual(logicalPosition,high))) {
        composedColor=destination;
        return;
    }
    vec2 unit=(logicalPosition-low)/(high-low);
    // Original UV endpoints:(L+1,T+1) ->(R,B), destination:(L,T) ->(R,B).
    vec2 sourceLogical=low+vec2(1.0)+unit*(high-low-vec2(1.0));
    ivec2 sourceTopPixel=ivec2(floor(sourceLogical*vec2(historyViewportSize)/vec2(originalSize)));
    sourceTopPixel=clamp(sourceTopPixel,ivec2(0),historyViewportSize-ivec2(1));
    ivec2 sourcePixel=ivec2(sourceTopPixel.x,historyViewportSize.y-1-sourceTopPixel.y);
    ivec3 sourceByte=ivec3(floor(texelFetch(historyImage,sourcePixel,0).rgb*255.0+0.5));
    ivec3 destinationByte=ivec3(floor(destination.rgb*255.0+0.5));
    ivec3 product=(sourceByte-destinationByte)*fixedCoefficient;
    ivec3 result=destinationByte+ivec3(signedFloor128(product.r),
        signedFloor128(product.g),signedFloor128(product.b));
    composedColor=vec4(vec3(result)/255.0,destination.a);
}
)GLSL";

} // namespace rr::platform::native
