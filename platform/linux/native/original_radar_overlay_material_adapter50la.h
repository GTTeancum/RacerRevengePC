#pragma once
#include "original_ui_material_adapter50kg.h"
#include <cmath>
#include "../../../src/assets/original_radar_overlay_material_reference50la.h"

namespace rr::platform {
// Synchronous preparation at the genuine primitive's ordered draw position.
// This is CPU pass conversion only; it does not append, reorder, upload or
// submit geometry. The caller must have recovered ScreenObject/type5 vertex
// lowering before supplying span bounds. STQ is not ready host XYZ.
struct OriginalRadarOverlayNativeSpan50LA {
    assets::OriginalRadarOverlayNativePass50LA material{};
    OriginalUiMaterialSpan50KG span{};
};
inline bool prepareOriginalRadarOverlayNativeSpan50LA(
    const assets::OriginalRadarOverlayMaterialReference50LA& retained,
    const assets::OriginalStandardMaterialCollectorReceipt50LA& collector,
    const OriginalUiUntexturedMaterialSpanBinding50KG& bounds,
    OriginalRadarOverlayNativeSpan50LA& output,
    assets::OriginalStandardMaterialDiagnostic50LA& d)noexcept {
    OriginalRadarOverlayNativeSpan50LA next;
    if(!retained.prepareNativeZeroMap(collector,next.material,d))return false;
    // A disabled pass consumes no renderer span. Keep this distinct from a
    // successful enabled pass and preserve the exact FINAL collector identity.
    if(next.material.enabled&&!convertOriginalUiUntexturedMaterialSpan50KG(
           next.material.policy,bounds,next.span)){
        d.status=assets::OriginalStandardMaterialStatus50LA::unsupportedPass;
        d.sourcePC=0x2e21d0;d.field="native zero-map span bounds/policy";return false;
    }
    output=next;return true;
}
// Native depth-independent lowering for a genuinely expanded ScreenObject
// point. Input XY is already in source screen pixels, after the actual owner
// applies any recovered fixed-point conversion. Never guess a /16 scale, a
// half-pixel offset, viewport transform or source depth from packet aliases.
// The owning geometry keeps expandedScreenStq (including original Z/W).
// Successful output uses inert native Z0 because this exact effective pass
// neither tests a relative depth nor writes depth; its RGB result is Z-invariant.
inline bool convertOriginalRadarOverlayDepthIndependentVertex50LA(
    const assets::OriginalRadarOverlayNativePass50LA& material,
    const std::array<float,4>& expandedScreenStq,
    const std::array<std::uint32_t,4>& rawRgba,assets::Vertex48B& output)noexcept {
    using F=assets::OriginalUiGsField50KG;
    const auto value=[&](F f)noexcept {
        return material.effectiveState.fields[static_cast<std::size_t>(f)].value;
    };
    if(!material.enabled||material.program<0||
       material.policy.depthMethod!=1||material.policy.depthWrite||
       material.policy.requiresOriginalDepthOwner||
       value(F::depthTestMethod)!=1||value(F::zWriteDisabled)!=1||
       value(F::textureMappingOnOff)!=0)return false;
    for(float v:expandedScreenStq)if(!std::isfinite(v))return false;
    for(auto c:rawRgba)if(c>255)return false;
    if(rawRgba[3]>material.policy.maximumVertexAlpha)return false;
    assets::Vertex48B next{};
    next.position[0]=expandedScreenStq[0];next.position[1]=expandedScreenStq[1];
    next.position[2]=0.f; // Native raster carrier only; original STQ.z is retained by owner.
    for(std::size_t i=0;i<4;++i)next.color[i]=static_cast<std::uint8_t>(rawRgba[i]);
    output=next;return true;
}
// Retain source pass/primitive order when appending these spans. Keep the real
// primitive/material and table live through submission and both owner drains.
// On ordinary submitOriginalUi the caller must establish the actual
// opaquePresentationAlphaOnly contract; this helper does NOT assert it. A
// source-alpha consumer instead needs the renderer's genuine source-RGBA owner.
// Depth and alpha storage are not inferred from depthTestMethod==1 alone.
}
