#pragma once
#include "original_ui_native_draw50kg.h"
#include "../../../src/assets/original_ui_material50kg.h"
#include <cstdint>

namespace rr::platform {
// NEW reconstruction, not recovered historical source. CPU conversion only.
struct OriginalUiMaterialSpanBinding50KG {
    std::uint32_t firstQuad = 0, quadCount = 0, drawQuadCount = 0;
    std::uint32_t textureIndex = assets::unresolvedMaterialTexture50EA;
    std::uint32_t textureCount = 0;
};
struct OriginalUiMaterialSpan50KG {
    OriginalUiSpan50KG span{};
    OriginalUiNativeSpan50KG native{};
};

// PRECONDITION: policy is the unchanged successful result of
// assets::prepareOriginalUiNativeMaterial50KG for this exact material/atlas.
// Shape checks below cannot prove that admission, authenticate those owners,
// or establish texture publication, original packet depth, pass-group or
// animation enablement. Binding counts are caller-supplied bounds, not proof
// of a real texture bank or geometry. Caller retains all those obligations.
//
// The native presentation renderer does not store source framebuffer alpha.
// Caller must independently establish its opaquePresentationAlphaOnly promise:
// no destination-alpha test/factor or downstream source-alpha consumer. This
// converter neither asserts that promise nor enables the frontend. The source
// stored-alpha scales are checked exactly, never substituted with a native
// framebuffer-alpha claim. Packet bytes and texture pixels remain untouched.
// Returns false for unsupported/inconsistent policy or invalid local bounds;
// output is unchanged on failure. Cross-span ordering/partition checks remain
// the draw caller's and renderer's responsibility.
inline bool convertOriginalUiMaterialSpan50KG(
    const assets::OriginalUiNativeMaterial50KG& policy,
    const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    if (!binding.drawQuadCount || binding.drawQuadCount > originalUiMaximumQuads50KG ||
        !binding.quadCount || binding.firstQuad >= binding.drawQuadCount ||
        binding.quadCount > binding.drawQuadCount - binding.firstQuad ||
        !binding.textureCount || binding.textureCount > assets::maxMaterialTextureAssets50EA ||
        binding.textureIndex >= binding.textureCount)
        return false;

    // Exact equality rejects nonfinite values as well as alternate scales.
    if (policy.environmentContext != 0 || policy.maximumVertexAlpha != 128 ||
        policy.colorScale != 255.f / 128.f ||
        policy.storedAlphaScale != 255.f / 128.f ||
        policy.blendAlphaScale != (255.f / 128.f) * (255.f / 128.f) ||
        policy.alphaStoreFromBlendFactor != 128.f / 255.f ||
        !policy.clampModulatedColorBeforeBlend ||
        policy.minification != policy.magnification ||
        policy.requiresOriginalDepthOwner != (policy.depthMethod != 1 || policy.depthWrite))
        return false;

    OriginalUiMaterialSpan50KG next{};
    next.span.firstQuad = binding.firstQuad;
    next.span.quadCount = binding.quadCount;
    next.span.textureIndex = binding.textureIndex;
    next.span.colorScale = policy.colorScale;
    next.span.materialMode = OriginalUiMaterialMode50KG::texturedModulate;
    next.native.depthWrite = policy.depthWrite;
    switch (policy.blend) {
    case assets::OriginalUiMaterialBlend50KG::opaque:
        if (policy.requiresSeparateAlphaStore) return false;
        next.span.blend = OriginalUiBlend50KG::opaque;
        // Recovered span API uses unity for opaque presentation alpha;
        // source alpha is not stored by this target.
        next.span.alphaScale = 1.f;
        break;
    case assets::OriginalUiMaterialBlend50KG::sourceAlpha:
        if (!policy.requiresSeparateAlphaStore) return false;
        next.span.blend = OriginalUiBlend50KG::sourceAlpha;
        next.span.alphaScale = policy.blendAlphaScale;
        break;
    case assets::OriginalUiMaterialBlend50KG::additive:
        if (!policy.requiresSeparateAlphaStore) return false;
        next.span.blend = OriginalUiBlend50KG::additive;
        next.span.alphaScale = policy.blendAlphaScale;
        break;
    default: return false;
    }
    // Source zero means NEVER; native zero is UNSPECIFIED. No enum casts.
    switch (policy.depthMethod) {
    case 0: next.native.depth = OriginalUiDepth50KG::never; break;
    case 1: next.native.depth = OriginalUiDepth50KG::always; break;
    case 2: next.native.depth = OriginalUiDepth50KG::greaterEqual; break;
    case 3: next.native.depth = OriginalUiDepth50KG::greater; break;
    default: return false;
    }
    const auto filter = [](assets::OriginalUiMaterialFilter50KG source,
        MaterialTextureFilter50EB& destination) noexcept {
        switch (source) {
        case assets::OriginalUiMaterialFilter50KG::nearest:
            destination = MaterialTextureFilter50EB::nearest; return true;
        case assets::OriginalUiMaterialFilter50KG::linear:
            destination = MaterialTextureFilter50EB::linear; return true;
        default: return false;
        }
    };
    const auto wrap = [](assets::OriginalUiMaterialWrap50KG source,
        MaterialTextureWrap50EB& destination) noexcept {
        switch (source) {
        case assets::OriginalUiMaterialWrap50KG::repeat:
            destination = MaterialTextureWrap50EB::repeat; return true;
        case assets::OriginalUiMaterialWrap50KG::clampToEdge:
            destination = MaterialTextureWrap50EB::clampToEdge; return true;
        default: return false;
        }
    };
    if (!filter(policy.minification, next.native.sampler.minification) ||
        !filter(policy.magnification, next.native.sampler.magnification) ||
        !wrap(policy.u, next.native.sampler.s) || !wrap(policy.v, next.native.sampler.t))
        return false;
    output = next;
    return true;
}

// A zero-map source has geometry bounds only. Texture indices/counts and
// samplers deliberately cannot be supplied through this separate binding.
struct OriginalUiUntexturedMaterialSpanBinding50KG {
    std::uint32_t firstQuad = 0, quadCount = 0, drawQuadCount = 0;
};
// PRECONDITION: unchanged successful result of the separate authenticated
// prepareOriginalUiNativeUntexturedMaterial50KG admission. Like the textured
// converter, this cannot prove source/pass/depth/alpha-consumer ownership.
// It neither constructs an atlas nor treats a missing real texture as valid
// untextured material. Output remains unchanged on failure.
inline bool convertOriginalUiUntexturedMaterialSpan50KG(
    const assets::OriginalUiNativeUntexturedMaterial50KG& policy,
    const OriginalUiUntexturedMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    if (!binding.drawQuadCount || binding.drawQuadCount > originalUiMaximumQuads50KG ||
        !binding.quadCount || binding.firstQuad >= binding.drawQuadCount ||
        binding.quadCount > binding.drawQuadCount - binding.firstQuad)
        return false;
    // TME=0 bypasses texture modulation. RGB and stored alpha are raw vertex
    // bytes; an active RGB blend coefficient uses A/128, not texture*A/128^2.
    if (policy.environmentContext != 0 || policy.maximumVertexAlpha != 128 ||
        policy.colorScale != 1.f || policy.storedAlphaScale != 1.f ||
        policy.blendAlphaScale != 255.f / 128.f ||
        policy.alphaStoreFromBlendFactor != 128.f / 255.f ||
        policy.requiresOriginalDepthOwner != (policy.depthMethod != 1 || policy.depthWrite))
        return false;

    OriginalUiMaterialSpan50KG next{};
    next.span.firstQuad = binding.firstQuad;
    next.span.quadCount = binding.quadCount;
    next.span.materialMode = OriginalUiMaterialMode50KG::untexturedVertexColor;
    next.span.colorScale = policy.colorScale;
    next.native.depthWrite = policy.depthWrite;
    switch (policy.blend) {
    case assets::OriginalUiMaterialBlend50KG::opaque:
        if (policy.requiresSeparateAlphaStore) return false;
        next.span.blend = OriginalUiBlend50KG::opaque;
        next.span.alphaScale = 1.f;
        break;
    case assets::OriginalUiMaterialBlend50KG::sourceAlpha:
        if (!policy.requiresSeparateAlphaStore) return false;
        next.span.blend = OriginalUiBlend50KG::sourceAlpha;
        next.span.alphaScale = policy.blendAlphaScale;
        break;
    case assets::OriginalUiMaterialBlend50KG::additive:
        if (!policy.requiresSeparateAlphaStore) return false;
        next.span.blend = OriginalUiBlend50KG::additive;
        next.span.alphaScale = policy.blendAlphaScale;
        break;
    default: return false;
    }
    switch (policy.depthMethod) {
    case 0: next.native.depth = OriginalUiDepth50KG::never; break;
    case 1: next.native.depth = OriginalUiDepth50KG::always; break;
    case 2: next.native.depth = OriginalUiDepth50KG::greaterEqual; break;
    case 3: next.native.depth = OriginalUiDepth50KG::greater; break;
    default: return false;
    }
    // Defaults retain unresolved textureIndex and unspecified sampler fields.
    output = next;
    return true;
}
}
