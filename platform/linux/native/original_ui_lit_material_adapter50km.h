#pragma once
#include "original_ui_material_adapter50kg.h"
#include "../../../src/assets/original_ui_screen_assets50kg.h"

namespace rr::platform {
// Successful prepareOriginalUiLitMaterial50KM output for the exact authored
// pass/image is required. Reuses only the bounded raster-policy conversion;
// never passes a Diffuse material through ScreenObject source admission.
// Source packet geometry/W/fog, original draw order, model phase, texture
// publication and opaquePresentationAlphaOnly proof remain caller-owned.
inline bool convertOriginalUiLitMaterialSpan50KM(
    const assets::OriginalUiLitMaterial50KM& material,
    const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    OriginalUiMaterialSpan50KG next;
    if(!convertOriginalUiMaterialSpan50KG(material.raster,binding,next))return false;
    switch(material.program){
    case assets::OriginalUiLitProgram50KM::diffuse:
        next.span.materialMode=OriginalUiMaterialMode50KG::sourceDiffuse;break;
    case assets::OriginalUiLitProgram50KM::diffuseCompressed:
        next.span.materialMode=OriginalUiMaterialMode50KG::sourceDiffuseCompressed;break;
    default:return false;
    }
    next.native.sourceFogEnabled=material.fogEnabled;
    next.native.sourceAlphaTestGreaterZero=material.alphaTestGreaterZero;
    output=next;return true;
}
// Policy comes from the exact bank that will be published, including any real
// combined-bank remapping. A policy prepared on a different bank/index/ID must
// be prepared again; do not relabel or silently substitute an uploaded image.
inline bool convertOriginalUiRider19IpumMaterialSpan50KM(
    const assets::OriginalUiRider19IpumMaterial50KM& material,
    const assets::OriginalUiScreenAssets50KG& publishedImages,
    const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    const auto* bank=publishedImages.textures();
    const auto* image=bank?bank->texture(binding.textureIndex):nullptr;
    if(material.imagePolicy!=assets::OriginalUiNativeIpumImagePolicy50KM::highestResolutionBaseOnly ||
        material.images!=&publishedImages || !publishedImages.loaded() || !image ||
        binding.textureIndex!=material.imageIndex || binding.textureCount!=bank->textureCount() ||
        image->identity!=material.imageIdentity || material.sourceStoredMipIndex!=4 ||
        material.sourceMipCount!=5 || material.material.program!=assets::OriginalUiLitProgram50KM::diffuseCompressed ||
        material.material.raster.blend!=assets::OriginalUiMaterialBlend50KG::opaque ||
        material.material.alphaTestGreaterZero)return false;
    return convertOriginalUiLitMaterialSpan50KM(material.material,binding,output);
}
}
