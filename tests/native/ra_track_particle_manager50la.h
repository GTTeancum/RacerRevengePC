#pragma once
#include "original_ui_main_entry50kg.h"
#include "ra_cold_aggregate50la.h"
#include <cstring>

// Application-aware definition kept out of the aggregate's include graph.
// No SessionAssets admission, archive re-decode, renderer atlas or emission.
namespace ra_cold_aggregate50LA {
inline bool Owner::constructRuntimeTrackParticleManager(
    const original_main_entry50KG::Application& application,
    std::uint64_t generation,Diagnostic& d)noexcept {
    if(!enter(d,Stage::trackParticleManager,trackParticleAttempted_,
        prefix_.particleAuthoring&&prefix_.staticWorld&&!worldImagesAttempted_))return false;
    try {
        const auto domain=application.standardMaterialDomain();
        const auto entities=application.entities();
        const auto* source=source_?source_->sourcesDecoded():nullptr;
        if(!generation||!source||!world_||!world_->staged_||world_->imagesAttempted_||
           !domain||domain!=world_->domain_||!domain->prepared()||!entities||
           !world_->context_||!world_->context_->live()||
           world_->context_->materialApplication()!=application.materialApplication||
           !domain->materials382B38()||!domain->resolver3E1A70()||
           !domain->archiveProvider3E1A70()||!domain->archiveProvider3E1A70()->sealed()||
           domain->archiveProvider3E1A70()->revision()!=world_->providerRevision_||
           domain->materials382B38()->populationTerminal()||domain->materials382B38()->demandTerminal()||
           domain->materials382B38()->coldCollectorCurrent(domain->coldCollector14D440()))
            return fail(d,"RA particle manager requires its same cold Application/domain before collector publication");
        if(particlesAttempted_&&(emitterApplication_!=entities||emitterGeneration_!=generation))
            return fail(d,"RA particle manager differs from the actual detached-emitter Application/session");
        // This bounded fresh route never adopts a surviving scene pool or a
        // material already retained/populated by an earlier particle manager.
        std::int32_t row=-1;a::OriginalStandardMaterialRowView50LA material;
        if(!domain->materials382B38()->find298C80("particle\\demoprt",row,d.trackParticles.material)||
           !domain->materials382B38()->row(row,material)||!material.references08||
           *material.references08!=0||material.object04)
            return fail(d,"RA particle manager requires fresh canonical demoprt reference ownership");
        const std::string* scbName=nullptr;
        for(const auto& block:source->world.parameters.blocks)if(block.name=="environment")
            for(const auto& field:block.fields)if(field.name=="particletexturemapname"){
                if(scbName)return fail(d,"duplicate actual RA particle atlas selector");
                scbName=&field.value;
            }
        if(!scbName||*scbName!="demoprt.tga"||application.particleDirectory36B530()!="particle\\")
            return fail(d,"actual RA SCB/Application particle directory selection differs");
        const auto* prt=particles_->originalMember(2);
        const auto* psm=particles_->originalMember(3);
        const auto* pst=particles_->originalMember(4);
        if(!prt||!psm||!pst)return fail(d,"retained authenticated RA particle member closure missing");
        const auto& provider=domain->archiveProvider3E1A70();
        const auto matches=[&](const char* member,const std::vector<std::uint8_t>& bytes){
            a::OriginalStandardArchiveProvider50LA::MemberView selected;
            a::OriginalStandardArchiveProvider50LA::MountView mount;
            return provider->lookup132820(member,selected)==a::OriginalStandardArchiveProvider50LA::Lookup::present&&
                provider->mount(selected.mount,mount)&&mount.archiveSha256&&
                *mount.archiveSha256==ra_cold_sources50LA::archiveSha&&mount.archiveBytes==4604561u&&
                mount.memberCount==1010u&&selected.member&&selected.data&&
                ra_compressed_scene::detail::normalized(*selected.member)==ra_compressed_scene::detail::normalized(member)&&
                selected.revision==world_->providerRevision_&&selected.size==bytes.size()&&
                !std::memcmp(selected.data,bytes.data(),bytes.size());
        };
        if(!matches("particle\\demoprt.prt",*prt)||!matches("particle\\demoprt.psm",*psm)||
           !matches("particle\\demoprt.pst",*pst))
            return fail(d,"RA particle source differs from the same mounted provider");
        auto materialSource=std::make_shared<a::OriginalStandardMaterialSource50LA>();
        if(!materialSource->prepare("particle\\demoprt.psm",*psm,d.trackParticles.material.document))
            return fail(d,"actual RA particle PSM parsing failed");
        // Retain actual identities before any corpus/constructor effects. No
        // separate manager or directory state is introduced on a retry.
        trackParticleDomain_=domain;trackParticleApplication_=entities;
        trackParticleHostApplication_=&application;trackParticleGeneration_=generation;
        trackParticles_=std::make_shared<g::OriginalTrackParticleCold50LA>();
        if(!domain->appendOriginalSources({materialSource},{{"particle\\demoprt.pst",*pst}},d.trackParticles.material))
            return fail(d,"RA particle source append failed; actual corpus prefix retained");
        const bool prepared=application.withParticleMaterialDirectory160F88([&](const std::string& actualDirectory){
            return trackParticles_->prepareNativeTrack(actualDirectory,*scbName,*prt,entities,
                domain->materials382B38(),generation,d.trackParticles);
        });
        if(!prepared)return fail(d,"RA scene particle constructor failed; counter/depth/reference prefix retained");
        if(domain->archiveProvider3E1A70()->revision()!=world_->providerRevision_||
           trackParticles_->material150()->table382B38()!=domain->materials382B38().get())
            return fail(d,"RA particle construction changed its canonical source ownership");
        prefix_.trackParticleManager=true;return true;
    }catch(...){return fail(d,"RA scene particle stage failed; partial owners retained for checked cleanup");}
}
} // namespace ra_cold_aggregate50LA
