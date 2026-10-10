#pragma once
#include "original_ui_material_adapter50kg.h"
#include "../../../src/assets/original_ui_main_particle_material50kg.h"
#include <cstring>
namespace rr::platform {
// Unchanged successful Options ParticleCode policy only. Historical backend
// mainParticle enum denotes the shared numeric shader, not a Main asset alias.
inline bool convertOriginalUiOptionsParticleSpan50KG(const assets::OriginalUiMainParticleMaterial50KG& material,
    std::uint8_t stateIndex,const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    if(stateIndex>1||!material.templateIdentity||!material.fogEnabled||!material.alphaTestGreaterZero||!material.noDestinationAlphaConsumer)return false;
    const auto* archive=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::options);
    if(!archive)return false;
    bool psm=false,pst=false;
    for(std::size_t i=0;i<archive->memberCount;++i) {
        const auto* m=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::options,i);
        if(m==material.identity)psm=std::strcmp(m->name,"particle\\mmprt.psm")==0;
        if(m==material.textureIdentity)pst=std::strcmp(m->name,"particle\\mmprt.pst")==0;
    }
    if(!psm||!pst)return false;
    const auto& policy=material.states[stateIndex];
    if(policy.environmentContext!=stateIndex||policy.depthMethod!=2||policy.depthWrite||
        policy.blend!=(stateIndex?assets::OriginalUiMaterialBlend50KG::additive:assets::OriginalUiMaterialBlend50KG::sourceAlpha)||
        policy.minification!=assets::OriginalUiMaterialFilter50KG::linear||policy.magnification!=assets::OriginalUiMaterialFilter50KG::linear||
        policy.u!=assets::OriginalUiMaterialWrap50KG::repeat||policy.v!=assets::OriginalUiMaterialWrap50KG::repeat)return false;
    auto sameTarget=policy;sameTarget.environmentContext=0;OriginalUiMaterialSpan50KG result;
    if(!convertOriginalUiMaterialSpan50KG(sameTarget,binding,result))return false;
    result.span.materialMode=OriginalUiMaterialMode50KG::mainParticle;output=result;return true;
}
}
