#pragma once
#include "original_ui_material_adapter50kg.h"
#include "../../../src/assets/original_ui_garage_model_materials50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"

namespace rr::platform {
// Construction-policy/image lowering only. It does not replace the actual
// StandardMaterial1C4 reference, final-leaf registration, dynamic249610 state,
// originating parameter owner, accepted binding history or source-frame seal.
// The complete collector must join those real owners before using this span.
// Image-bank canonicalization is checked separately from local numeric IDs.
inline bool convertOriginalUiGaragePrelitMaterialSpan50KG(
    const assets::OriginalUiGarageModelMaterials50KG& owner,
    std::uint32_t materialSlot,std::uint32_t passIndex,
    const assets::OriginalUiImageBank50KG& bank,std::uint32_t imageOwnerIndex,
    const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output,assets::OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};try {
    const auto reject=[&](const char* field) {
        d.error=assets::OriginalUiMaterialError50KG::texture;d.field=field;return false;
    };
    assets::OriginalUiGaragePrelitMaterial50KG policy;
    if(!assets::prepareOriginalUiGaragePrelitMaterial50KG(owner,materialSlot,passIndex,policy,d))return false;
    const auto images=bank.images();
    const auto* map=bank.mapping(imageOwnerIndex,policy.imageIndex);
    const auto* textures=images?images->textures():nullptr;
    const auto* image=textures?textures->texture(binding.textureIndex):nullptr;
    const auto* source=policy.images?policy.images->sourceProvenance(policy.imageIndex):nullptr;
    const auto* combined=images?images->sourceProvenance(binding.textureIndex):nullptr;
    const auto* decoded=images?images->decodedTexture(binding.textureIndex):nullptr;
    if(!bank.loaded() || !images || !images->loaded() || !map || !image || !source || !combined || !decoded ||
       map->sourceIdentity!=policy.imageIdentity || map->combinedIndex!=binding.textureIndex ||
       map->combinedIdentity!=image->identity || binding.textureCount!=textures->textureCount() ||
       source->memberIdentity!=combined->memberIdentity || source->celIdentity!=combined->celIdentity ||
       source->pstOffset!=combined->pstOffset || source->pstBytes!=combined->pstBytes ||
       decoded->mips.size()!=policy.sourceMipCount || policy.sourceStoredMipIndex+1!=policy.sourceMipCount)
        return reject("exact Garage PreLit image-bank remapping");
    if(!binding.drawQuadCount || binding.drawQuadCount>originalUiMaximumSourceQuads50KM ||
       !binding.quadCount || binding.firstQuad>=binding.drawQuadCount ||
       binding.quadCount>binding.drawQuadCount-binding.firstQuad)
        return reject("bounded original source packet span");
    // Reuse only scalar raster/sampler conversion. Its ordinary8192 canvas
    // bound is not a whole source-frame bound. Validate the real source range
    // above against the UNCHANGED20480 source capacity, convert one local unit,
    // then publish that already-validated original range. No dummy geometry is
    // emitted, no source slot is dropped and no existing guard is broadened.
    auto scalar=binding;scalar.firstQuad=0;scalar.quadCount=scalar.drawQuadCount=1;
    OriginalUiMaterialSpan50KG next;
    if(!convertOriginalUiMaterialSpan50KG(policy.raster,scalar,next))
        return reject("original Garage PreLit raster policy");
    next.span.firstQuad=binding.firstQuad;next.span.quadCount=binding.quadCount;
    next.span.materialMode=OriginalUiMaterialMode50KG::sourcePrelitCompressed;
    next.native.sourcePacketStq=true;
    next.native.sourceFogEnabled=policy.fogEnabled;
    next.native.sourceAlphaTestGreaterZero=policy.alphaTestGreaterZero;
    output=next;return true;
    }catch(...){d.error=assets::OriginalUiMaterialError50KG::allocation;return false;}
}
}
