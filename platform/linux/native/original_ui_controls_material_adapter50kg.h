#pragma once
#include "original_ui_lit_material_adapter50km.h"
namespace rr::platform {
inline bool convertOriginalUiControlsSpecularSpan50KG(const assets::OriginalUiControlsSpecularMaterial50KG& p,
    const assets::OriginalUiScreenAssets50KG& images,const OriginalUiMaterialSpanBinding50KG& range,
    OriginalUiMaterialSpan50KG& output) noexcept {
    const auto* bank=images.textures();const auto* image=bank?bank->texture(range.textureIndex):nullptr;
    if(p.images!=&images||!images.loaded()||!image||range.textureCount!=bank->textureCount()||
        p.imageIndex!=range.textureIndex||p.imageIdentity!=image->identity||
        p.material.program!=assets::OriginalUiLitProgram50KM::specularMapCompressed||
        p.material.raster.blend!=assets::OriginalUiMaterialBlend50KG::destinationAlphaAdditive||p.material.raster.depthWrite)return false;
    auto common=p.material.raster;common.blend=assets::OriginalUiMaterialBlend50KG::additive;
    OriginalUiMaterialSpan50KG next;if(!convertOriginalUiMaterialSpan50KG(common,range,next))return false;
    next.span.materialMode=OriginalUiMaterialMode50KG::sourceSpecularMapCompressed;
    next.span.blend=OriginalUiBlend50KG::destinationAlphaAdditive;next.span.alphaScale=1;
    next.native.sourceFogEnabled=p.material.fogEnabled;next.native.sourceAlphaTestGreaterZero=p.material.alphaTestGreaterZero;
    output=next;return true;
}
}
