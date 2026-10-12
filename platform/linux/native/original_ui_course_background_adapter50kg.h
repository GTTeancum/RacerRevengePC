#pragma once
#include "original_ui_native_draw50kg.h"
#include "../../../src/assets/original_ui_screen_assets50kg.h"
#include "../../../src/assets/original_ui_course_background50kg.h"
#include <cstring>
namespace rr::platform {
// HOST presentation adapter for raw2A6E90/25CC60 dispatch, not a PSM.
// Collector supplies full640x448 quad first, RGB128/A64 and source UV inset.
// Clamp-to-edge is explicit host policy; inherited source CLAMP is unclosed.
inline bool prepareOriginalUiCourseBackgroundSpan50KG(
    const assets::OriginalUiScreenAssets50KG& images,std::uint32_t image,
    OriginalUiSpan50KG& span,OriginalUiNativeSpan50KG& state)noexcept{
    const auto* source=images.sourceProvenance(image);const auto* decoded=images.decodedTexture(image);
    const auto* bank=images.textures();const auto* texture=bank?bank->texture(image):nullptr;
    if(!source||!decoded||!texture||!source->memberIdentity||source->celIdentity||source->pstOffset||
       source->pstBytes!=287772||!assets::isOriginalUiCourseBackgroundIdentity50KG(source->memberIdentity)||
       decoded->kind!=1||decoded->mips.size()!=1||texture->texture.width!=640||texture->texture.height!=448)return false;
    bool canonical=false;const auto* archive=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course);
    if(!archive)return false;for(std::size_t i=0;i<archive->memberCount;++i)
        canonical|=assets::originalUiMemberIdentity50KG(archive->id,i)==source->memberIdentity;
    if(!canonical)return false;
    OriginalUiSpan50KG s;s.firstQuad=0;s.quadCount=1;s.textureIndex=image;
    s.blend=OriginalUiBlend50KG::sourceAlpha;s.colorScale=255.f/128.f;s.alphaScale=(255.f/128.f)*(255.f/128.f);
    s.materialMode=OriginalUiMaterialMode50KG::texturedModulate;
    OriginalUiNativeSpan50KG n;n.depth=OriginalUiDepth50KG::always;n.depthWrite=false;
    n.sampler.minification=MaterialTextureFilter50EB::linear;n.sampler.magnification=MaterialTextureFilter50EB::linear;
    n.sampler.s=MaterialTextureWrap50EB::clampToEdge;n.sampler.t=MaterialTextureWrap50EB::clampToEdge;
    span=s;state=n;return true;
}
}
