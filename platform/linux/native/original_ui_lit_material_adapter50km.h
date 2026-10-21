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
    switch(material.coverage){
    case assets::OriginalUiCoveragePolicy50KM::aa0: break;
    case assets::OriginalUiCoveragePolicy50KM::fourSampleAa1:
        if(material.program!=assets::OriginalUiLitProgram50KM::diffuseCompressed ||
           material.raster.blend!=assets::OriginalUiMaterialBlend50KG::sourceAlpha ||
           material.alphaTestGreaterZero)return false;
        next.native.coverage=OriginalUiCoverage50KG::fourSampleAa1;break;
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
    const auto* decoded=publishedImages.decodedTexture(binding.textureIndex);
    if(material.imagePolicy!=assets::OriginalUiNativeIpumImagePolicy50KM::highestResolutionBaseOnly ||
        material.images!=&publishedImages || !publishedImages.loaded() || !image || !decoded ||
        binding.textureIndex!=material.imageIndex || binding.textureCount!=bank->textureCount() ||
        image->identity!=material.imageIdentity || !material.sourceMipCount || material.sourceMipCount>5 ||
        decoded->kind!=2 || decoded->mips.size()!=material.sourceMipCount ||
        material.sourceStoredMipIndex!=material.sourceMipCount-1 ||
        material.material.program!=assets::OriginalUiLitProgram50KM::diffuseCompressed)return false;
    return convertOriginalUiLitMaterialSpan50KM(material.material,binding,output);
}
// Same concrete publication check for authentic indexed pod bodies.
// This converts their first pass only, and cannot discard or approve the later
// destination-alpha specular pass or assert an alpha-disposable frame.
inline bool convertOriginalUiRider19BodyMaterialSpan50KM(
    const assets::OriginalUiRider19BodyMaterial50KM& material,
    const assets::OriginalUiScreenAssets50KG& publishedImages,
    const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    const auto* bank=publishedImages.textures();
    const auto* image=bank?bank->texture(binding.textureIndex):nullptr;
    const auto* decoded=publishedImages.decodedTexture(binding.textureIndex);
    if(material.imagePolicy!=assets::OriginalUiNativeIndexedImagePolicy50KM::highestResolutionBaseOnly ||
        material.images!=&publishedImages || !publishedImages.loaded() || !image || !decoded ||
        binding.textureIndex!=material.imageIndex || binding.textureCount!=bank->textureCount() ||
        image->identity!=material.imageIdentity || !material.sourceMipCount || material.sourceMipCount>5 ||
        decoded->kind!=1 || decoded->mips.size()!=material.sourceMipCount ||
        material.sourceStoredMipIndex!=material.sourceMipCount-1 ||
        material.material.program!=assets::OriginalUiLitProgram50KM::diffuseCompressed)return false;
    return convertOriginalUiLitMaterialSpan50KM(material.material,binding,output);
}
// Only a prepared canonical17_3 cutout result on this exact immutable bank.
// Preserve source-alpha testing; no alpha replacement or shader change.
inline bool convertOriginalUiRider17CutoutMaterialSpan50KM(
    const assets::OriginalUiRider17CutoutMaterial50KM& material,
    const assets::OriginalUiScreenAssets50KG& publishedImages,
    const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    const auto* bank=publishedImages.textures();
    const auto* image=bank?bank->texture(binding.textureIndex):nullptr;
    const auto* decoded=publishedImages.decodedTexture(binding.textureIndex);
    const auto& raster=material.material.raster;
    if(material.imagePolicy!=assets::OriginalUiNativeIndexedImagePolicy50KM::highestResolutionBaseOnly ||
        material.images!=&publishedImages || !publishedImages.loaded() || !image || !decoded ||
        binding.textureIndex!=material.imageIndex || binding.textureCount!=bank->textureCount() ||
        image->identity!=material.imageIdentity || material.sourceMipCount!=4 || material.sourceStoredMipIndex!=3 ||
        decoded->kind!=1 || decoded->mips.size()!=4 ||
        decoded->mips.back().width!=128 || decoded->mips.back().height!=128 ||
        material.material.program!=assets::OriginalUiLitProgram50KM::diffuseCompressed ||
        raster.blend!=assets::OriginalUiMaterialBlend50KG::opaque || !raster.depthWrite || raster.depthMethod!=2 ||
        raster.environmentContext!=0 || raster.minification!=assets::OriginalUiMaterialFilter50KG::linear ||
        raster.magnification!=assets::OriginalUiMaterialFilter50KG::linear ||
        raster.u!=assets::OriginalUiMaterialWrap50KG::repeat || raster.v!=assets::OriginalUiMaterialWrap50KG::repeat ||
        !material.material.fogEnabled || !material.material.alphaTestGreaterZero)return false;
    return convertOriginalUiLitMaterialSpan50KM(material.material,binding,output);
}
// Exact bank/index/identity for original pod_wndw and12c_4 passes1/3.
// Caller must preserve both passes and the intervening authentic specular leaf
// in the source registry schedule; this local conversion cannot certify it.
inline bool convertOriginalUiRiderWindowMaterialSpan50KM(
    const assets::OriginalUiRiderWindowMaterial50KM& material,
    const assets::OriginalUiScreenAssets50KG& publishedImages,
    const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    const auto* bank=publishedImages.textures();
    const auto* image=bank?bank->texture(binding.textureIndex):nullptr;
    const auto* decoded=publishedImages.decodedTexture(binding.textureIndex);
    if(material.imagePolicy!=assets::OriginalUiNativeIndexedImagePolicy50KM::highestResolutionBaseOnly ||
        material.images!=&publishedImages || !publishedImages.loaded() || !image || !decoded ||
        decoded->kind!=1 || decoded->mips.size()!=material.sourceMipCount ||
        binding.textureIndex!=material.imageIndex || binding.textureCount!=bank->textureCount() ||
        image->identity!=material.imageIdentity || material.sourceStoredMipIndex!=material.sourceMipCount-1 ||
        (material.sourceMipCount!=3&&material.sourceMipCount!=4) || (material.sourcePass!=1 && material.sourcePass!=3) ||
        material.material.program!=assets::OriginalUiLitProgram50KM::diffuseCompressed ||
        material.material.raster.blend!=assets::OriginalUiMaterialBlend50KG::sourceAlpha ||
        !material.material.raster.depthWrite || material.material.raster.depthMethod!=2 ||
        !material.material.fogEnabled || !material.material.alphaTestGreaterZero)return false;
    return convertOriginalUiLitMaterialSpan50KM(material.material,binding,output);
}
// Exact original shadow image binding. Common conversion supplies only the
// unchanged sampler/depth fields; the dedicated attenuation blend never enters
// the ordinary RGB-only renderer or generic Diffuse admission.
inline bool convertOriginalUiRider19ShadowMaterialSpan50KM(
    const assets::OriginalUiRider19ShadowMaterial50KM& material,
    const assets::OriginalUiScreenAssets50KG& publishedImages,
    const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    const auto* bank=publishedImages.textures();
    const auto* image=bank?bank->texture(binding.textureIndex):nullptr;
    if(material.images!=&publishedImages || !publishedImages.loaded() || !image ||
        binding.textureIndex!=material.imageIndex || binding.textureCount!=bank->textureCount() ||
        image->identity!=material.imageIdentity || material.material.program!=assets::OriginalUiLitProgram50KM::diffuseCompressed ||
        material.material.raster.blend!=assets::OriginalUiMaterialBlend50KG::destinationSourceAlphaAttenuation ||
        !material.material.raster.depthWrite || material.material.raster.depthMethod!=2 ||
        !material.material.fogEnabled || !material.material.alphaTestGreaterZero)return false;
    auto commonRaster=material.material.raster;commonRaster.blend=assets::OriginalUiMaterialBlend50KG::sourceAlpha;
    OriginalUiMaterialSpan50KG next;
    if(!convertOriginalUiMaterialSpan50KG(commonRaster,binding,next))return false;
    next.span.materialMode=OriginalUiMaterialMode50KG::sourceDiffuseCompressed;
    next.span.blend=OriginalUiBlend50KG::destinationSourceAlphaAttenuation;
    next.span.alphaScale=1.f; // Explicit source byte coefficient in sealed RGBA route.
    next.native.sourceFogEnabled=material.material.fogEnabled;
    next.native.sourceAlphaTestGreaterZero=material.material.alphaTestGreaterZero;
    output=next;return true;
}
// Separate admission for source-RGBA composition only. The common converter
// still rejects both new blend kinds, and the RGB-only renderer rejects this
// program identity. Reusing its bounded sampler/depth conversion below does
// not relabel the original authored material or change any source field.
inline bool convertOriginalUiRider19SpecularMaterialSpan50KM(
    const assets::OriginalUiRider19SpecularMaterial50KM& material,
    const assets::OriginalUiScreenAssets50KG& publishedImages,
    const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    const auto* bank=publishedImages.textures();
    const auto* image=bank?bank->texture(binding.textureIndex):nullptr;
    const auto blend=material.material.raster.blend;
    if(material.images!=&publishedImages || !publishedImages.loaded() || !image ||
        binding.textureIndex!=material.imageIndex || binding.textureCount!=bank->textureCount() ||
        image->identity!=material.imageIdentity || material.material.program!=assets::OriginalUiLitProgram50KM::specularMapCompressed ||
        (blend!=assets::OriginalUiMaterialBlend50KG::destinationAlphaAdditive &&
         blend!=assets::OriginalUiMaterialBlend50KG::fixed255Additive&&
         blend!=assets::OriginalUiMaterialBlend50KG::fixed64Additive) ||
        material.material.raster.depthWrite!=(blend==assets::OriginalUiMaterialBlend50KG::fixed64Additive))return false;
    auto commonRaster=material.material.raster;commonRaster.blend=assets::OriginalUiMaterialBlend50KG::additive;
    OriginalUiMaterialSpan50KG next;
    if(!convertOriginalUiMaterialSpan50KG(commonRaster,binding,next))return false;
    next.span.materialMode=OriginalUiMaterialMode50KG::sourceSpecularMapCompressed;
    next.span.blend=blend==assets::OriginalUiMaterialBlend50KG::destinationAlphaAdditive ?
        OriginalUiBlend50KG::destinationAlphaAdditive :
        blend==assets::OriginalUiMaterialBlend50KG::fixed64Additive?OriginalUiBlend50KG::fixed64Additive:
        OriginalUiBlend50KG::fixed255Additive;
    next.span.alphaScale=1.f; // Typed programmable blend uses source byte units.
    next.native.sourceFogEnabled=material.material.fogEnabled;
    next.native.sourceAlphaTestGreaterZero=material.material.alphaTestGreaterZero;
    output=next;return true;
}
}
