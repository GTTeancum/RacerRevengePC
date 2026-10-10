#pragma once
#include "original_ui_material_adapter50kg.h"
#include "../../../src/assets/original_ui_main_particle_material50kg.h"

namespace rr::platform {
// PRECONDITION: unchanged successful cold admission for the actual Main
// mmprt PSM, DefaultMT and exact mmprt atlas. State0 and state1 are selected by
// the source run sign; they are alternatives, not two passes over each quad.
// This conversion does not certify the full draw's alpha-consumer promise.
inline bool convertOriginalUiMainParticleSpan50KG(
    const assets::OriginalUiMainParticleMaterial50KG& material,
    std::uint8_t stateIndex,const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    if(stateIndex>1 || !material.identity || !material.textureIdentity ||
        !material.templateIdentity || !material.fogEnabled ||
        !material.alphaTestGreaterZero || !material.noDestinationAlphaConsumer)
        return false;
    const auto& policy=material.states[stateIndex];
    if(policy.environmentContext!=stateIndex || policy.depthMethod!=2 || policy.depthWrite ||
        policy.blend!=(stateIndex?assets::OriginalUiMaterialBlend50KG::additive:
            assets::OriginalUiMaterialBlend50KG::sourceAlpha) ||
        policy.minification!=assets::OriginalUiMaterialFilter50KG::linear ||
        policy.magnification!=assets::OriginalUiMaterialFilter50KG::linear ||
        policy.u!=assets::OriginalUiMaterialWrap50KG::repeat ||
        policy.v!=assets::OriginalUiMaterialWrap50KG::repeat)
        return false;
    // Both original contexts target the same Main framebuffer/depth owner.
    // Only after authenticating the actual pair can the ordinary native
    // sampler/blend/depth conversion be reused with its context0 requirement.
    auto sameTarget=policy;sameTarget.environmentContext=0;
    OriginalUiMaterialSpan50KG next;
    if(!convertOriginalUiMaterialSpan50KG(sameTarget,binding,next))return false;
    next.span.materialMode=OriginalUiMaterialMode50KG::mainParticle;
    output=next;return true;
}
}
