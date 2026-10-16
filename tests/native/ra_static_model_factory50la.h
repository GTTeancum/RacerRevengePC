#pragma once
#include "original_ui_main_entry50kg.h"
#include "ra_cold_aggregate50la.h"

namespace ra_cold_aggregate50LA {
inline bool Owner::constructStaticModelPrefixes(
    const original_main_entry50KG::Application& application,
    Diagnostic& d)noexcept {
    if(!enter(d,Stage::staticWorld,modelPrefixesAttempted_,prefix_.staticWorld&&prefix_.trackParticleManager))return false;
    // Reuse the same retained session identity already consumed by the genuine
    // particle/camera owners. No independent caller generation or ready flag.
    const auto generation=trackParticleGeneration_;
    const auto entities=application.entities();const auto domain=application.standardMaterialDomain();
    if(!generation||!entities||!world_||!domain||domain!=world_->domain_||
       !world_->context_||world_->context_->materialApplication()!=application.materialApplication||
       trackParticleHostApplication_!=&application||entities!=trackParticleApplication_||
       domain!=trackParticleDomain_||!trackParticles_||!trackParticles_->prepared()||
       (particlesAttempted_&&(entities!=emitterApplication_||generation!=emitterGeneration_))||
       (rosterMountComplete_&&rosterMountApplication_!=&application))
        return fail(d,"static model prefixes require the same actual Application/domain/session");
    if(!world_->constructModelPrefixes(entities,generation,d.staticModels))
        return fail(d,"static model prefix failed; counted owners retained for checked close");
    prefix_.staticModelPrefixes=true;return true;
}
}
