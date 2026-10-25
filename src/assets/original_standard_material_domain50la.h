#pragma once
#include "original_standard_material_table50la.h"
#include "original_standard_texture_resource50la.h"
#include "original_standard_archive_provider50la.h"
#include <cstring>
#include <utility>

namespace ra_static_world50LA {class Owner;}
namespace rr::game { class OriginalRaceConstructor50LA; class OriginalPauseStorage50LA; class OriginalTimeStats50LA; class OriginalSpeedBoost50LA; class OriginalDamage50LA; class OriginalPositionStatsResources50LA; }
namespace rr::platform {class OriginalUiGarageQuad50KG;class OriginalUiMemoryCardConstructor50KG;}
namespace rr::assets {
// The one Application-owned canonical382B38/382B70/383400 domain. Its tables
// retain their identities across native scene/Video close and reopen. This is
// composition of the real predecessor owners, not another name registry, a
// numeric projection of Rider's ledger, or a per-track replacement manager.
// AIDamage must borrow materials382B38() from this same object. Its separate
// sequential ScreenMT join and all manager-wide terminal guards remain intact.
class OriginalStandardMaterialDomain50LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class ::ra_static_world50LA::Owner;
    friend class OriginalApplicationFonts50LA;
    friend class rr::platform::OriginalUiGarageQuad50KG;
    friend class rr::platform::OriginalUiMemoryCardConstructor50KG;
    friend class OriginalTimeStatsMaterialReference50LA;
    friend class OriginalSpeedBoostMaterialReference50LA;
    friend class OriginalDamageMaterialReference50LA;
    friend class rr::game::OriginalSpeedBoost50LA;
    friend class rr::game::OriginalDamage50LA;
    friend class rr::game::OriginalTimeStats50LA;
    friend class rr::game::OriginalPositionStatsResources50LA;
    friend class rr::game::OriginalPauseStorage50LA;
    friend class rr::game::OriginalRaceConstructor50LA;
    std::shared_ptr<const OriginalUiMaterialApplication50LA> application_;
    std::shared_ptr<OriginalStandardProgramTable50LA> programs_;
    std::shared_ptr<OriginalStandardTextureTable50LA> textures_;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalStandardArchiveProvider50LA> archiveProvider_;
    // Actual original input mounted before Application template preparation.
    // Keeps failed material preparation retrying the SAME authenticated owner.
    // A native input/mount pin, never a source12CEA0 registration receipt.
    std::shared_ptr<const OriginalUiArchive50KG> serviceAppStart_;
    // The SAME canonical material table, with actual17CAD4 names reached
    // before templates; never exposed as a prepared domain during this prefix.
    std::shared_ptr<OriginalStandardMaterialTable50LA> earlyAppStartMaterials_;
    std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>> sources_;
    struct SourceAdmission {
        std::shared_ptr<const OriginalStandardMaterialSource50LA> source;
        OriginalStandardArchiveProvider50LA::RetainedMaterialMember member;
    };
    // Actual positive-lookup receipts, coupled to exact strong source owners.
    // Parsed bytes/name alone never prove a previously closed bank admission.
    std::vector<SourceAdmission> sourceAdmissions_;
    OriginalStandardMaterialCollectorReceipt50LA coldCollector_;
    // Last member: canonical materials release their pass/texture refs before
    // the remaining domain owners. Actual model refs independently pin this
    // same table until their ordered26E4C0 cleanup has finished.
    std::shared_ptr<OriginalStandardMaterialTable50LA> materials_;
    // Only the actual Application starts this phase before APPSTART/material
    // loading. Legacy standalone prepareFresh may still call it later; no
    // source startup chronology is inferred from either native allocation.
    bool prepareFreshNativeService(OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        d={};
        if(resolver_||archiveProvider_||serviceAppStart_||application_||prepared()){
            d.status=OriginalStandardMaterialStatus50LA::alreadyPrepared;return false;
        }
        try {
            auto resolver=std::make_shared<OriginalStandardTextureResolver50LA>();
            auto provider=std::make_shared<OriginalStandardArchiveProvider50LA>();
            if(!resolver->bootstrapFreshFileCapacity12CBA0(provider,d.texture)){
                d.status=OriginalStandardMaterialStatus50LA::allocation;return false;
            }
            resolver_=std::move(resolver);archiveProvider_=std::move(provider);return true;
        }catch(...){d.status=OriginalStandardMaterialStatus50LA::allocation;return false;}
    }
    bool retainNativeAppStart(std::shared_ptr<const OriginalUiArchive50KG> archive,
        OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        d={};
        if(!resolver_||!archiveProvider_||application_||prepared()||!archive||!archive->loaded()||
           archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::appStart)){
            d.status=OriginalStandardMaterialStatus50LA::source;return false;
        }
        if(serviceAppStart_){
            if(serviceAppStart_==archive)return true;
            d.status=OriginalStandardMaterialStatus50LA::source;return false;
        }
        // Never import a warm directory into this early single-input phase.
        if(archiveProvider_->mountCount()!=0||archiveProvider_->revision()!=0||archiveProvider_->sealed()){
            d.status=OriginalStandardMaterialStatus50LA::source;return false;
        }
        OriginalStandardArchiveProvider50LA::Diagnostic providerDiagnostic;
        if(!archiveProvider_->mountAuthenticatedUi(archive,providerDiagnostic)){
            d.status=OriginalStandardMaterialStatus50LA::source;return false;
        }
        // Join actual source rows only if the Application reached17CAB8.
        // Legacy standalone/externally prepared paths never synthesize them.
        if(!resolver_->joinAppStartCanonicalMount(d.texture)){
            d.status=OriginalStandardMaterialStatus50LA::source;return false;
        }
        serviceAppStart_=std::move(archive);return true;
    }
    bool prepareAppStartNames17CAD4(OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        d={};if(earlyAppStartMaterials_)return true; // Native continuation only.
        if(!resolver_||!serviceAppStart_||application_||prepared()){
            d.status=OriginalStandardMaterialStatus50LA::source;return false;
        }
        try {
            auto table=std::make_shared<OriginalStandardMaterialTable50LA>();
            std::vector<std::uint8_t> bytes;
            if(!resolver_->readAppStartNames17CAD4(bytes,d.texture)){
                d.status=OriginalStandardMaterialStatus50LA::source;return false;
            }
            // FClose already happened. These bytes came through the actual
            // encoded cache/decoded slot, not an immutable-member substitute.
            const auto readDiagnostic=d.texture;
            const bool parsed=table->prepareAppStartNames17CAD4(serviceAppStart_,bytes,d);
            d.texture=readDiagnostic; // Preserve reached source effects if parsing fails.
            if(!parsed)return false;
            earlyAppStartMaterials_=std::move(table);return true;
        }catch(...){d.status=OriginalStandardMaterialStatus50LA::allocation;return false;}
    }
    bool prepareAppStartPrograms17CAE8(OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        d={};if(programs_)return programs_->prepared();
        if(!earlyAppStartMaterials_||!resolver_||!serviceAppStart_||application_||prepared()){
            d.status=OriginalStandardMaterialStatus50LA::source;return false;
        }
        try {
            auto programs=std::make_shared<OriginalStandardProgramTable50LA>();
            if(!programs->prepareFresh(d.program)){d.status=OriginalStandardMaterialStatus50LA::program;return false;}
            programs_=std::move(programs);return true;
        }catch(...){d.status=OriginalStandardMaterialStatus50LA::allocation;return false;}
    }
public:
    OriginalStandardMaterialDomain50LA() noexcept=default;
    OriginalStandardMaterialDomain50LA(const OriginalStandardMaterialDomain50LA&)=delete;
    OriginalStandardMaterialDomain50LA& operator=(const OriginalStandardMaterialDomain50LA&)=delete;
    bool prepared() const noexcept {return bool(materials_);}
    bool prepareFresh(std::shared_ptr<const OriginalUiMaterialApplication50LA> application,
        OriginalStandardMaterialDiagnostic50LA& d) noexcept {
        d={};
        if(prepared()){d.status=OriginalStandardMaterialStatus50LA::alreadyPrepared;return false;}
        if(!application||!application->prepared()){
            d.status=OriginalStandardMaterialStatus50LA::templates;return false;
        }
        try {
            auto programs=programs_?programs_:std::make_shared<OriginalStandardProgramTable50LA>();
            auto textures=std::make_shared<OriginalStandardTextureTable50LA>();
            auto materials=earlyAppStartMaterials_?earlyAppStartMaterials_:
                std::make_shared<OriginalStandardMaterialTable50LA>();
            // Reuse early Application custody. Standalone callers retain the
            // legacy late preparation route, explicitly without tracked history.
            if(!resolver_&&!archiveProvider_){if(!prepareFreshNativeService(d))return false;}
            if(!resolver_||!archiveProvider_){d.status=OriginalStandardMaterialStatus50LA::source;return false;}
            if(!retainNativeAppStart(application->appStart(),d))return false;
            if(!programs->prepared()&&!programs->prepareFresh(d.program)){
                d.status=OriginalStandardMaterialStatus50LA::program;return false;
            }
            if(!textures->prepareFresh(d.texture)){
                d.status=OriginalStandardMaterialStatus50LA::texture;return false;
            }
            if(earlyAppStartMaterials_){
                if(!materials->attachAppStartTemplates(application,d))return false;
            }else if(!materials->prepareFresh(application,d))return false;
            application_=std::move(application);programs_=std::move(programs);
            textures_=std::move(textures);
            materials_=std::move(materials);earlyAppStartMaterials_.reset();return true;
        }catch(...){d.status=OriginalStandardMaterialStatus50LA::allocation;return false;}
    }
    const std::shared_ptr<OriginalStandardMaterialTable50LA>& materials382B38() const noexcept {return materials_;}
    const OriginalStandardMaterialCollectorReceipt50LA& coldCollector14D440() const noexcept {
        return coldCollector_;
    }
    const std::shared_ptr<OriginalStandardProgramTable50LA>& programs382B70() const noexcept {return programs_;}
    const std::shared_ptr<OriginalStandardTextureTable50LA>& textures383400() const noexcept {return textures_;}
    const std::shared_ptr<OriginalStandardTextureResolver50LA>& resolver3E1A70() const noexcept {return resolver_;}
    const std::shared_ptr<OriginalStandardArchiveProvider50LA>& archiveProvider3E1A70()const noexcept {
        return archiveProvider_;
    }

    // Serial native archive/corpus growth. Existing names, row identities,
    // cached objects and source references survive a Course rider change.
    // Identical source members are shared; conflicting bytes never replace
    // a live application's source or impersonate original lookup precedence.
    bool appendOriginalSources(
        const std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>>& materials,
        const std::vector<OriginalStandardTextureSourceInput50LA>& textures,
        OriginalStandardMaterialDiagnostic50LA& d) noexcept {
        d={};
        if(!prepared()){d.status=OriginalStandardMaterialStatus50LA::unprepared;return false;}
        if(materials_->populationTerminal()||materials_->demandTerminal()||textures_->populationTerminal()){
            d.status=OriginalStandardMaterialStatus50LA::phase;return false;
        }
        if(!archiveProvider_||!archiveProvider_->sealed()){
            d.status=OriginalStandardMaterialStatus50LA::source;d.sourcePC=0x132820u;return false;
        }
        const auto providerRevision=archiveProvider_->revision();
        try {
            const auto same=[](const std::string& a,const std::string& b){
                if(a.size()!=b.size())return false;
                for(std::size_t i=0;i<a.size();++i){
                    const auto fold=[](unsigned char c){return c>='A'&&c<='Z'?c+32:c;};
                    if(fold(static_cast<unsigned char>(a[i]))!=fold(static_cast<unsigned char>(b[i])))return false;
                }
                return true;
            };
            auto next=sources_;
            for(const auto& source:materials){
                if(!source||!source->prepared()||!source->member()||!source->originalBytes()){
                    d.status=OriginalStandardMaterialStatus50LA::source;return false;
                }
                // Every incoming request still requires active namespace
                // authentication, even when it equals an old corpus member.
                OriginalStandardArchiveProvider50LA::RetainedMaterialMember incoming;
                if(!archiveProvider_->retainMaterialMember132820(*source->member(),*source->originalBytes(),incoming)){
                    d.status=OriginalStandardMaterialStatus50LA::source;d.sourcePC=0x29B628u;return false;
                }
                bool found=false;
                for(const auto& existing:next)if(same(*existing->member(),*source->member())){
                    if(*existing->originalBytes()!=*source->originalBytes()){
                        d.status=OriginalStandardMaterialStatus50LA::source;return false;
                    }
                    found=true;break;
                }
                if(!found)next.push_back(source);
            }
            // New ambiguity or unequal active bytes still reject. Genuine
            // absence after a temporary bank close is different: an already
            // admitted immutable corpus owner survives only with its exact
            // provider-issued receipt, never by filename or byte equality alone.
            std::vector<SourceAdmission> nextAdmissions;nextAdmissions.reserve(next.size());
            for(const auto& source:next){
                using Provider=OriginalStandardArchiveProvider50LA;
                Provider::MemberView member;
                const auto& bytes=*source->originalBytes();
                const auto lookup=archiveProvider_->lookup132820(*source->member(),member);
                SourceAdmission admission;admission.source=source;
                bool authenticated=false;
                if(lookup==Provider::Lookup::present){
                    authenticated=archiveProvider_->retainMaterialMember132820(*source->member(),bytes,admission.member);
                }else if(lookup==Provider::Lookup::absent){
                    for(const auto& old:sourceAdmissions_)if(old.source.get()==source.get()&&
                        !old.source.owner_before(source)&&!source.owner_before(old.source)){
                        if(archiveProvider_->retainedClosedMaterialMemberCurrent(old.member,*source->member(),bytes)){
                            admission.member=old.member;authenticated=true;
                        }
                        break;
                    }
                }
                if(!authenticated){
                    d.status=OriginalStandardMaterialStatus50LA::source;d.sourcePC=0x29B628u;return false;
                }
                nextAdmissions.push_back(std::move(admission));
            }
            if(archiveProvider_->revision()!=providerRevision){
                d.status=OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x132820u;return false;
            }
            // PSM registration precedes image population for an original world.
            // An empty texture batch has no resolver mutation and must not
            // require textures to have been prepared already. This does not
            // mark the resolver ready or permit unresolved texture demand.
            if(!textures.empty()&&!resolver_->appendOriginalSources(textures,d.texture)){
                d.status=OriginalStandardMaterialStatus50LA::texture;return false;
            }
            sources_=std::move(next);sourceAdmissions_=std::move(nextAdmissions);return true;
        }catch(...){d.status=OriginalStandardMaterialStatus50LA::allocation;return false;}
    }

    // Authenticated original members come from the actual archive provider.
    // First binding owns immutable input. A serial restart may present the
    // exact same bytes; it never replaces the resolver/cache or canonical
    // PSM owners. A different or incomplete corpus is explicitly unsupported
    // here, not evidence of source absence or permission to fabricate a file.
    bool bindOriginalSources(
        const std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>>& materials,
        const std::vector<OriginalStandardTextureSourceInput50LA>& textures,
        OriginalStandardMaterialDiagnostic50LA& d) noexcept {
        d={};
        if(!prepared()){d.status=OriginalStandardMaterialStatus50LA::unprepared;return false;}
        if(materials_->populationTerminal()||materials_->demandTerminal()||textures_->populationTerminal()){
            d.status=OriginalStandardMaterialStatus50LA::phase;return false;
        }
        try {
            if(materials.empty()||textures.empty()){
                d.status=OriginalStandardMaterialStatus50LA::source;return false;
            }
            for(const auto& source:materials)if(!source||!source->prepared()||
                !source->member()||!source->originalBytes()){
                d.status=OriginalStandardMaterialStatus50LA::source;return false;
            }
            if(resolver_->prepared()){
                if(sources_.size()!=materials.size()||resolver_->sourceCount()!=textures.size()){
                    d.status=OriginalStandardMaterialStatus50LA::source;return false;
                }
                for(std::size_t i=0;i<materials.size();++i)
                    if(*sources_[i]->member()!=*materials[i]->member()||
                       *sources_[i]->originalBytes()!=*materials[i]->originalBytes()){
                        d.status=OriginalStandardMaterialStatus50LA::source;return false;
                    }
                for(std::size_t i=0;i<textures.size();++i)
                    if(!resolver_->member(i)||!resolver_->originalBytes(i)||
                       *resolver_->member(i)!=textures[i].member||
                       *resolver_->originalBytes(i)!=textures[i].bytes){
                        d.status=OriginalStandardMaterialStatus50LA::source;return false;
                    }
                return true;
            }
            auto sources=materials;
            // The same3E1A70 may already own genuine model-file rows from
            // cold Rig construction. Admit the PST corpus without replacing
            // that cache or claiming an empty file/resource manager.
            if(!resolver_->prepareFresh(textures,d.texture)){
                d.status=OriginalStandardMaterialStatus50LA::texture;return false;
            }
            // This legacy binding does not issue positive provider receipts.
            // A later append must authenticate these actual owners while live.
            sources_=std::move(sources);sourceAdmissions_.clear();return true;
        }catch(...){d.status=OriginalStandardMaterialStatus50LA::allocation;return false;}
    }
private:
    // Same source population pair, not Garage/TimeStats caller authority.
    // Only the actual private MemoryCard owner invokes this after its whole
    // pending-set admission. No collector rebuild or page publication.
    bool populateMemoryCardResources2DF144(std::shared_ptr<const OriginalStandardMaterialContext50LA> context,
        OriginalStandardMaterialDiagnostic50LA& d) noexcept {
        return populateTimeStatsResources2DF144(std::move(context),d);
    }
    bool populateGarageResources2DF144(std::shared_ptr<const OriginalStandardMaterialContext50LA> context,
        OriginalStandardMaterialDiagnostic50LA& d) noexcept {
        return populateTimeStatsResources2DF144(std::move(context),d);
    }
    bool populateDamageResources2DF144(std::shared_ptr<const OriginalStandardMaterialContext50LA> context,
        OriginalStandardMaterialDiagnostic50LA& d) noexcept {
        return populateTimeStatsResources2DF144(std::move(context),d);
    }
    bool populateSpeedBoostResources2DF144(std::shared_ptr<const OriginalStandardMaterialContext50LA> context,
        OriginalStandardMaterialDiagnostic50LA& d) noexcept {
        return populateTimeStatsResources2DF144(std::move(context),d);
    }
    // Source2DF144/2DF150 pair for each actual TimeStats ScreenTexture.
    // No collector rebuild occurs inside2DF0B0. The later source population
    // phase must rebuild before any caller borrows a collector publication.
    bool populateTimeStatsResources2DF144(std::shared_ptr<const OriginalStandardMaterialContext50LA> context,
        OriginalStandardMaterialDiagnostic50LA& d) noexcept {
        d={};
        if(!prepared()||!resolver_){
            d.status=OriginalStandardMaterialStatus50LA::unprepared;return false;
        }
        if(!context||!context->live()||context->materialApplication()!=application_){
            d.status=OriginalStandardMaterialStatus50LA::video;return false;
        }
        OriginalStandardMaterialPopulationInput50LA input;
        input.sources=&sources_;input.actualPrograms382B70=programs_;
        input.actualTextures383400=textures_;input.construction=context;
        input.positiveConstructionProvider_=archiveProvider_;
        if(!materials_->populate14D370(input,d))return false;
        const bool texturesOkay=textures_->populate29E340(resolver_,std::move(context),d.texture);
        d.sourceEffectsCommitted=d.sourceEffectsCommitted||d.texture.sourceEffectsCommitted;
        if(!texturesOkay){
            d.status=OriginalStandardMaterialStatus50LA::texture;return false;
        }
        return true;
    }
public:
    // Existing domain caller preserves its population pair plus collector.
    // Called only after genuine model26E640 refs and current native host
    // creation. Ascending14D370 and29E340 scans precede14D440's complete
    // canonical registry rebuild. The stored receipt is a borrowed cold seal;
    // the table invalidates it before any pass publication or destruction.
    bool populate(std::shared_ptr<const OriginalStandardMaterialContext50LA> context,
        OriginalStandardMaterialDiagnostic50LA& d) noexcept {
        if(!populateTimeStatsResources2DF144(std::move(context),d))return false;
        OriginalStandardMaterialDiagnostic50LA collectorDiagnostic;
        if(!materials_->rebuildColdCollector14D440(coldCollector_,collectorDiagnostic)){
            // Keep the failing collector location without losing successful
            // earlier publication counts, texture details or committed effects.
            const auto populated=d;d=collectorDiagnostic;
            d.populationVisited=populated.populationVisited;
            d.populationPublished=populated.populationPublished;
            d.program=populated.program;d.texture=populated.texture;
            d.sourceEffectsCommitted=d.sourceEffectsCommitted||populated.sourceEffectsCommitted;
            return false;
        }
        d.sourceEffectsCommitted=d.sourceEffectsCommitted||collectorDiagnostic.sourceEffectsCommitted;
        return true;
    }
};
}
