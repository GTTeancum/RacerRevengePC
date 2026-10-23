#pragma once
#include "original_ui_material_adapter50kg.h"
#include "../../../src/assets/original_ui_garage_model_materials50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
namespace rr::platform {
// Exact immutable policy lowering only. Actual canonical row/pass history and
// real current model/light/camera packet producers remain separately required.
inline bool convertOriginalUiGarageLitMaterialSpan50KG(
    const assets::OriginalUiGarageModelMaterials50KG& owner,std::uint32_t slot,std::uint32_t pass,
    const assets::OriginalUiImageBank50KG& bank,std::uint32_t imageOwner,
    const OriginalUiMaterialSpanBinding50KG& binding,OriginalUiMaterialSpan50KG& output,
    assets::OriginalUiMaterialDiagnostic50KG& d)noexcept {
    d={};try {
        const auto reject=[&](const char* field){d.error=assets::OriginalUiMaterialError50KG::texture;d.field=field;return false;};
        assets::OriginalUiGarageLitMaterial50KG policy;
        if(!assets::prepareOriginalUiGarageLitMaterial50KG(owner,slot,pass,policy,d))return false;
        const auto images=bank.images();const auto* map=bank.mapping(imageOwner,policy.imageIndex);
        const auto* textures=images?images->textures():nullptr;
        const auto* image=textures?textures->texture(binding.textureIndex):nullptr;
        const auto* original=policy.images?policy.images->sourceProvenance(policy.imageIndex):nullptr;
        const auto* combined=images?images->sourceProvenance(binding.textureIndex):nullptr;
        const auto* decoded=images?images->decodedTexture(binding.textureIndex):nullptr;
        if(!bank.loaded()||!images||!images->loaded()||!map||!image||!original||!combined||!decoded||
           map->sourceIdentity!=policy.imageIdentity||map->combinedIndex!=binding.textureIndex||
           map->combinedIdentity!=image->identity||binding.textureCount!=textures->textureCount()||
           original->memberIdentity!=combined->memberIdentity||original->celIdentity!=combined->celIdentity||
           original->pstOffset!=combined->pstOffset||original->pstBytes!=combined->pstBytes||
           decoded->mips.size()!=policy.sourceMipCount||policy.sourceStoredMipIndex+1!=policy.sourceMipCount)
            return reject("exact Garage diffuse/specular image-bank remapping");
        if(!binding.drawQuadCount||binding.drawQuadCount>originalUiMaximumSourceQuads50KM||!binding.quadCount||
           binding.firstQuad>=binding.drawQuadCount||binding.quadCount>binding.drawQuadCount-binding.firstQuad)
            return reject("bounded actual Garage model packet span");
        using P=assets::OriginalUiGarageProgram50KG;
        using B=assets::OriginalUiMaterialBlend50KG;
        const bool specular=policy.program==P::specularMapCompressed;
        auto commonRaster=policy.raster;
        if(specular){
            // The authenticated Garage producer admits exactly these two GS
            // specular blends, not Rider fixed64 or shadow. Reuse only the
            // ordinary scalar sampler/depth conversion, then restore the
            // real source-RGBA blend below; never change the authored policy.
            if((policy.raster.blend!=B::destinationAlphaAdditive&&policy.raster.blend!=B::fixed255Additive)||
               policy.raster.depthWrite||policy.raster.depthMethod!=2||
               !policy.fogEnabled||!policy.alphaTestGreaterZero)
                return reject("actual Garage source-RGBA specular policy");
            commonRaster.blend=B::additive;
        }
        auto scalar=binding;scalar.firstQuad=0;scalar.quadCount=scalar.drawQuadCount=1;
        OriginalUiMaterialSpan50KG next;
        if(!convertOriginalUiMaterialSpan50KG(commonRaster,scalar,next))return reject("actual Garage model raster policy");
        if(specular){
            next.span.blend=policy.raster.blend==B::destinationAlphaAdditive?
                OriginalUiBlend50KG::destinationAlphaAdditive:OriginalUiBlend50KG::fixed255Additive;
            next.span.alphaScale=1.f; // Typed programmable blend uses source byte units.
        }
        switch(policy.program){
            case P::diffuse:next.span.materialMode=OriginalUiMaterialMode50KG::sourceDiffuse;break;
            case P::diffuseCompressed:next.span.materialMode=OriginalUiMaterialMode50KG::sourceDiffuseCompressed;break;
            case P::specularMapCompressed:next.span.materialMode=OriginalUiMaterialMode50KG::sourceSpecularMapCompressed;break;
            default:return reject("PreLit cannot masquerade as a lit program");
        }
        next.span.firstQuad=binding.firstQuad;next.span.quadCount=binding.quadCount;
        next.native.coverage=policy.coverage==assets::OriginalUiCoveragePolicy50KM::fourSampleAa1?
            OriginalUiCoverage50KG::fourSampleAa1:OriginalUiCoverage50KG::aa0;
        next.native.sourceFogEnabled=policy.fogEnabled;next.native.sourceAlphaTestGreaterZero=policy.alphaTestGreaterZero;
        // New Garage packets must retain their actual separately rounded raw
        // STQ. Do not reconstruct it from old semantic-UV-only numeric paths.
        // No such producer is asserted merely because this material exists.
        next.native.sourcePacketStq=true;
        output=next;return true;
    }catch(...){d.error=assets::OriginalUiMaterialError50KG::allocation;return false;}
}
}
