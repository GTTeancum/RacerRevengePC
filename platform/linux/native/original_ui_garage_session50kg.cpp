#include "../../../src/game/original_loading_manager33050la.h"
#include <cstdio>
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_garage_audio_backing50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_garage_renderer_lease50kg.h"
#include "../../../src/game/original_camera_start_native.h"
#include "../../../src/assets/original_ui_garage_watto_assets50kg.h"
#include "../../../src/assets/original_ui_garage_scene_assets50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include <bit>
#include <cmath>
#include <exception>
#include <new>
#include <cstring>
#include <chrono>
#include <cstdio>

namespace {
// Diagnostic-only host elapsed time. Never feeds source clocks, state or admission.
struct GarageColdStageTiming {
    using Clock=std::chrono::steady_clock;
    const char* name;
    const rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic;
    const Clock::time_point start=Clock::now();
    GarageColdStageTiming(const char* phase,
        const rr::assets::OriginalStandardMaterialDiagnostic50LA& d)noexcept:name(phase),diagnostic(d){
        std::fprintf(stderr,"original_garage_cold_stage phase=%s edge=begin mono_ms=%.3f\n",
            name,std::chrono::duration<double,std::milli>(start.time_since_epoch()).count());
        std::fflush(stderr);
    }
    ~GarageColdStageTiming()noexcept {
        const auto end=Clock::now();
        std::fprintf(stderr,"original_garage_cold_stage phase=%s edge=scope_exit elapsed_ms=%.3f status=%u source_pc=%u\n",
            name,std::chrono::duration<double,std::milli>(end-start).count(),
            static_cast<unsigned>(diagnostic.status),static_cast<unsigned>(diagnostic.sourcePC));
        std::fflush(stderr);
    }
};
template<class A,class B> bool sameCustodyShared(const std::shared_ptr<A>& a,
    const std::shared_ptr<B>& b)noexcept {
    return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
bool custodyFailure(std::string& error,const char* text)noexcept {
    try{error=text;}catch(...){error.clear();}return false;
}
//Native immutable source corpus only. This does not retain canonical rows,
//populate resources or emulate a bank handle. The real source lookup must be
//open here; material refs and14D370/29E340/14D440 happen at their actual calls.
bool appendGarageBankSources(const std::shared_ptr<const rr::assets::OriginalUiArchive50KG>& archive,
    const std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA>& domain,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d,bool* domainAppendStarted=nullptr,
    const std::vector<std::string>* residentTextures=nullptr)noexcept {
    namespace a=rr::assets;using Provider=a::OriginalStandardArchiveProvider50LA;
    if(domainAppendStarted)*domainAppendStarted=false;
    d={};const auto provider=domain?domain->archiveProvider3E1A70():nullptr;
    auto fail=[&](std::uint32_t pc){d.status=a::OriginalStandardMaterialStatus50LA::source;d.sourcePC=pc;return false;};
    if(!archive||!archive->loaded()||!provider||!provider->sealed())return fail(0x132820);
    try {
        std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials;
        std::vector<a::OriginalStandardTextureSourceInput50LA> textures;
        auto texture=[&](const std::string& name)->bool {
            //Exact24DADC precedence. A present unsupported IPU is not replaced
            //by a lower-priority image; its real resolver call remains closed.
            for(const char* extension:{".pst",".ipu",".tga",".atm"}){
                Provider::MemberView view;const auto result=provider->lookup132820(name+extension,view);
                if(result==Provider::Lookup::absent)continue;
                if(result!=Provider::Lookup::present||!view.member||!view.data)return fail(0x132820);
                if(!std::strcmp(extension,".ipu"))return fail(0x24DB60);
                textures.push_back({*view.member,std::vector<std::uint8_t>(view.data,view.data+view.size)});return true;
            }
            return fail(0x24DADC);
        };
        for(std::size_t i=0;i<archive->memberCount();++i){
            const auto original=archive->member(i);if(!original.identity||!original.data)return fail(0x132820);
            const std::string name=original.identity->name;
            if(name.size()<4)continue;
            std::string extension=name.substr(name.size()-4);
            for(char& c:extension)if(c>='A'&&c<='Z')c=char(c-'A'+'a');
            if(extension!=".psm")continue;
            Provider::MemberView view;
            if(provider->lookup132820(name,view)!=Provider::Lookup::present||!view.member||!view.data||
               view.size!=original.size||std::memcmp(view.data,original.data,view.size))return fail(0x132820);
            auto source=std::make_shared<a::OriginalStandardMaterialSource50LA>();
            if(!source->prepare(*view.member,std::vector<std::uint8_t>(view.data,view.data+view.size),d.document))return fail(0x29B628);
            const std::string stem=view.member->substr(0,view.member->size()-4);
            //29B720..79C /2472D4: texture names inherit THIS material directory.
            //Font and particle PSMs are not in the model ui\\textures namespace.
            const auto separator=stem.find_last_of('\\');
            const std::string texturePrefix=separator==std::string::npos?std::string{}:stem.substr(0,separator+1);
            const auto* general=source->section("General");std::int32_t passes=0;
            if(!general||!a::originalUiInteger50KG(*general,"TotalShaderPasses",0,passes,d.document,0x29B718)||
               passes<1||passes>4)return fail(0x29B718);
            for(std::int32_t pass=0;pass<passes;++pass){
                const auto* section=source->section("ShaderPass_"+std::to_string(pass+1));
                if(!section)return fail(0x24718C);
                //247214 defaults1, explicitly0 is genuine NoTexture. Shader
                //enable flags do not skip the source texture construction.
                std::int32_t maps=1;
                if(!a::originalUiInteger50KG(*section,"totalTextureMaps",1,maps,d.document,0x247214)||maps<0||maps>2)return fail(0x247214);
                for(std::int32_t map=0;map<maps;++map){
                    const auto* field=section->find(map?"TextureMap_2":"TextureMap_1");
                    auto image=field?field->text:std::string{};
                    if(image.empty())image=stem.substr(stem.find_last_of("\\/")+1);
                    if(image.find_first_of("\\/.:\"")!=std::string::npos)return fail(0x24DADC);
                    if(!texture(texturePrefix+image))return false;
                }
            }
            materials.push_back(std::move(source));
        }
        //Missing-PSM fallback rows (2475C0) read the image of the same name;
        //for the selected pod it is the boot-resident UIPODTEX image.
        if(residentTextures)for(const auto& name:*residentTextures)if(!texture(name))return false;
        //All source gathering above owns only local vectors. This records the
        //actual call boundary, not readiness or presumed atomic publication.
        if(domainAppendStarted)*domainAppendStarted=true;
        return domain->appendOriginalSources(materials,textures,d);
    }catch(...){d.status=a::OriginalStandardMaterialStatus50LA::allocation;d.sourcePC=0x29B628;return false;}
}
}
namespace rr::platform {
OriginalUiGarageSession50KG::OriginalUiGarageSession50KG(const Application& root,
    std::shared_ptr<Grant> grant,std::shared_ptr<Resources> resources,
    std::shared_ptr<const SceneAssets> assets,
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility)noexcept
    :issuer_(&root),issuerLifetime_(root.animationClock()),authority_(root.entities()),
     grant_(std::move(grant)),scope_(grant_->scope_),resources_(std::move(resources)),
     sceneAssets_(std::move(assets)),visibility_(std::move(visibility)),
     application_(root.state),motions_(root.motionLibrary4333C0()){}
OriginalUiGarageSession50KG::OriginalUiGarageSession50KG(const Application& root,
    std::shared_ptr<const SceneAssets> assets)noexcept
    :issuer_(&root),issuerLifetime_(root.animationClock()),authority_(root.entities()),
     sceneAssets_(std::move(assets)),application_(root.state),motions_(root.motionLibrary4333C0()){}
bool OriginalUiGarageSession50KG::prepareManagerSelected1D1CD0(D& d)noexcept {
    d={};
    if(!issuer_||!sameRoot(*issuer_)||failed_||retiring_||!entryFrontend_||
       managerEntryPhase_!=ManagerEntryPhase::selectedBankOpened1D1D68||!selectedBankLease_||
       !selectedArchiveOnlyScope1D1D88_||entrySelected_||
       !constructionInputsCurrent(*issuer_,grant_,resources_,sceneAssets_)){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x1D1CD0;return false;
    }
    try {
        entrySelected_=std::make_shared<OriginalUiGarageSelectedModel50KG>();
        //Retain manager2B0 before load240740, enrollment and actual attachment.
        //No visibility publication or Page borrow is inferred from this return.
        if(!entrySelected_->prepare1D1CD0(grant_,*resources_,sceneAssets_,d))return false;
        managerEntryPhase_=ManagerEntryPhase::selectedModelConstructed1D1E80;return true;
    }catch(...){d.status=assets::OriginalStandardMaterialStatus50LA::allocation;d.sourcePC=0x1D1CD0;return false;}
}
bool OriginalUiGarageSession50KG::prepareManagerSearch1CCD14(D& d)noexcept {
    d={};
    if(!issuer_||!constructionCurrent(*issuer_)||!entryFrontend_||entrySearch_||
       managerConstructorPhase_!=ManagerConstructorPhase::rendererCallsReturned1CCD14||
       !scope_||managerDispatcher2AC_!=&scope_->dispatcher_||scope_->managerPublished_){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x1CCD14;return false;
    }
    try {
        // The existing owner creates the one1024 List, actual SearchNode and
        // both aliases in source order. It retains any partial constructor.
        entrySearch_=std::make_shared<OriginalUiGarageManagerSearch50KG>();
        OriginalUiGarageManagerSearch50KG::Diagnostic search;
        if(!entrySearch_->prepare1CCD14(grant_,entrySelected_,search)){
            d.status=assets::OriginalStandardMaterialStatus50LA::source;d.sourcePC=search.sourcePC;return false;
        }
        managerConstructorPhase_=ManagerConstructorPhase::searchReturned1CCD78;return true;
    }catch(...){d.status=assets::OriginalStandardMaterialStatus50LA::allocation;d.sourcePC=0x1CCD14;return false;}
}
OriginalUiGarageSession50KG::~OriginalUiGarageSession50KG()noexcept {
    if(published_&&!closed_)std::terminate();
}
bool OriginalUiGarageSession50KG::constructionInputsCurrent(const Application& root,
    const std::shared_ptr<Grant>& grant,const std::shared_ptr<Resources>& resources,
    const std::shared_ptr<const SceneAssets>& assets)noexcept {
    return grant&&grant->current()&&grant->issuer()==&root&&
        sameCustodyShared(grant->application(),root.entities())&&resources&&
        resources->issuer_==&root&&resources->generation_==grant->generation_&&
        sameCustodyShared(resources->application_,root.entities())&&assets&&assets->loaded();
}
bool OriginalUiGarageSession50KG::managerArchiveScopeCurrent()const noexcept {
    try {
    if(!issuer_||!sceneAssets_||!managerArchiveLease_||!managerArchiveLease_->loaded()||
       !managerProviderLease_||!managerProviderLease_->sealed()||!managerDomainLease_||
       managerDomainLease_->archiveProvider3E1A70()!=managerProviderLease_||
       issuer_->standardMaterialDomainOwner_!=managerDomainLease_||
       managerArchiveLease_!=sceneAssets_->archive())return false;
    const auto* identity=managerArchiveLease_->identity();
    const auto original=managerArchiveLease_->find("UI/UIGarage/UI.DAT");
    assets::OriginalStandardArchiveProvider50LA::MemberView member;
    assets::OriginalStandardArchiveProvider50LA::MountView mount;
    if(!identity||!original.identity||
       managerProviderLease_->lookup132820(original.identity->name,member)!=
         assets::OriginalStandardArchiveProvider50LA::Lookup::present||
       !managerProviderLease_->mount(member.mount,mount)||!mount.archiveSha256||
       *mount.archiveSha256!=identity->sha256||mount.archiveBytes!=identity->bytes||
       member.size!=original.size||!member.data||!original.data)return false;
    // Actual authenticated provider row, not a positive-ready flag. Later
    //nonconflicting mounts may advance its revision; changed bytes/ambiguity
    //still reject. The entry revision is retained as historical evidence.
    return std::memcmp(member.data,original.data,member.size)==0;
    }catch(...){return false;}
}
bool OriginalUiGarageSession50KG::releaseManagerPageTable2A3B04(D& d)noexcept {
    d={};
    if(!scope_||!grant_||!managerPage144Known_||!managerPageTable144_||
       managerPageTableReleased2A3B04_||
       !scope_->managerPageTableRetirementCurrent(grant_->generation_,this)){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x2A3B04;return false;
    }
    managerPageTable144_.reset(); //Actual native array delete at source2A3B04.
    const auto step=scope_->noteGarageManagerPageTableReleased(grant_->generation_,this);
    if(!step.okay())std::terminate();
    managerPageTableReleased2A3B04_=true;d.sourcePC=0x2A3B04;return true;
}
bool OriginalUiGarageSession50KG::renderCustodyCurrent140230(std::uint64_t serial)const noexcept {rr::game::EntityForestReadScope50LA forestReadScope50LA;
    std::uint32_t pause=0;
    if(!issuerLifetime_.readPause11C(pause)||!issuer_||!sameRoot(*issuer_)||!serial||
       !scope_||!grant_||!scope_->publishedPopulatedGarageManagerCurrent(grant_->generation_)||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||runtimeRootPublication_!=serial||
       runtimeRoot10ReturnPC_!=0x141F88||runtimeOuter26CReturnPC_!=0x141F88||
       !page_||page_->grant_!=grant_||!grant_->garageAttachedPageCurrent(*page_,page_->entity_)||
       page_->entryPhase_!=OriginalUiGaragePageOwner50KG::EntryPhase::returned||
       !page_->updateReturned_||page_->updateFailed_||page_->updatePublication_!=serial)return false;
    return true;
}
bool OriginalUiGarageSession50KG::sameRoot(const Application& root)const noexcept {
    if(ownerThread_!=std::this_thread::get_id()||issuer_!=&root||!published_||closed_)return false;
    std::uint32_t pause=0;
    return issuerLifetime_.readPause11C(pause)&&issuerLifetime_.sameOwner(root.animationClock())&&
        sameCustodyShared(authority_,root.entities());
}
bool OriginalUiGarageSession50KG::hasUnenrolledSelectedFailure()const noexcept {
    return failed_&&entrySelected_&&entrySelected_->enrollmentFailed2403A0_;
}
bool OriginalUiGarageSession50KG::unenrolledSelectedFailurePrefixCurrent(const Application& root)const noexcept {
    using P=UnenrolledSelectedAbortPhase;
    if(!sameRoot(root)||!failed_||!entryFrontend_||entryFailureStatus_!=assets::OriginalStandardMaterialStatus50LA::phase||
       entryFailureSourcePC_!=0x2403A0||managerPublished1B8274_||
       managerEntryPhase_!=ManagerEntryPhase::selectedBankOpened1D1D68||
       managerConstructorPhase_!=ManagerConstructorPhase::notStarted||
       managerDrivePhase_!=ManagerDrivePhase::notStarted||managerLoadPhase_!=ManagerLoadPhase::notStarted||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||entryAbortPhase_!=EntryAbortPhase::notStarted||
       !scope_||!grant_||!sameCustodyShared(grant_->scope_,scope_)||scope_->issuer_!=&root||
       !sameCustodyShared(scope_->application_,authority_)||!scope_->live().okay()||
       grant_->generation_!=entryManagerGeneration_||grant_->epoch_!=scope_->managerEpoch_||
       !entryManagerGeneration_||!scope_->managerEpoch_||scope_->managerPublished_||
       scope_->managerForestRetiring_||scope_->managerRetiring_||scope_->managerSearchOwner_||
       scope_->dispatcherCurrentManager30_||!scope_->garageChildren_.empty()||
       scope_->manager_.firstChild18||!resources_||resources_->issuer_!=&root||
       resources_->generation_!=entryManagerGeneration_||!sameCustodyShared(resources_->application_,authority_)||
       resources_->domain_!=root.standardMaterialDomainOwner_||resources_->context_!=root.materialContextOwner_||
       !entrySelected_||!entrySelected_->attempted_||!entrySelected_->enrollmentFailed2403A0_||
       entrySelected_->grant_!=grant_||entrySelected_->assets_!=sceneAssets_||
       entrySelected_->enrolled_||entrySelected_->attached_||entrySelected_->placed_||
       !entrySelected_->model_||resources_->population_.size()!=1||
       resources_->population_[0]!=entrySelected_->model_||entrySelected_->visibility_||
       entrySelected_->managerSearchBorrower_||entrySelected_->managerRetiring_||
       !sceneAssets_||!sceneAssets_->selectedPod()||
       entrySelected_->model_->source_!=sceneAssets_->selectedPod()->model||
       entrySearch_||entryLights_||entryFog_||widgets_||watto_||actors_||visibility_||page_||podScene_||
       audio_||pending_||actorInputs_||wattoInputs_||wattoAssets_||
       managerDat298_||managerArchiveOnlyScope170_||managerBankLease_||
       managerArchiveLease_||managerProviderLease_||managerDomainLease_||
       managerPageTable144_||managerPage144Known_||manager150Known_||managerCurrentPage14C_||
       managerPageCount148_||!managerDeferred190Empty_||runtimeFrame_||drawContribution_||
       pageAttempted_||pageSceneAttempted_||actorsAttempted_||wattoAttempted_)return false;
    if(unenrolledSelectedAbortPhase_<P::bankRetired){
        if(!selectedBankLease_||!selectedArchiveOnlyScope1D1D88_)return false;
    }else if(selectedBankLease_||selectedArchiveOnlyScope1D1D88_)return false;
    if(unenrolledSelectedAbortPhase_<P::managerRetired){
        if(!grant_->current()||!scope_->managerLive_||scope_->managerGeneration_!=entryManagerGeneration_||
           scope_->manager_.reference00!=1||scope_->manager_.parent1C||scope_->manager_.previous10||
           scope_->manager_.next14)return false;
    }else if(scope_->managerLive_||scope_->managerGeneration_||scope_->manager_.reference00)return false;
    if(unenrolledSelectedAbortPhase_<P::selectedRetired){
        if(entrySelected_->closed_||!entrySelected_->model_->current())return false;
    }else if(!entrySelected_->closed_||!entrySelected_->model_->closed_||
             entrySelected_->model_->resourceHeld_||entrySelected_->model_->nativeGarageFifoPins_)return false;
    return true;
}
bool OriginalUiGarageSession50KG::preflightUnenrolledSelectedAbort(D& d)const noexcept {
    d={};
    if(!issuer_||!unenrolledSelectedFailurePrefixCurrent(*issuer_)||retiring_||
       unenrolledSelectedAbortPhase_!=UnenrolledSelectedAbortPhase::notStarted){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x2403A0;return false;
    }
    return entrySelected_->model_->preflightUnpublished142590(d);
}
bool OriginalUiGarageSession50KG::retireUnenrolledSelectedAbort(D& d)noexcept {
    d={};
    if(!issuer_||!unenrolledSelectedFailurePrefixCurrent(*issuer_)||!retiring_||
       unenrolledSelectedAbortPhase_!=UnenrolledSelectedAbortPhase::notStarted){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x2403A0;return false;
    }
    //The actual selected owner chooses closeUnpublished because enrollment
    //never happened. No Scope/Authority removal or phantom attach is used.
    return entrySelected_->close1D18D0(d);
}
bool OriginalUiGarageSession50KG::nativeAudioConstructionIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t& generation,std::uint64_t& epoch)const noexcept {
    if(!sameRoot(root)||failed_||actorSampleFailed10179C_||unsealedRenderRetirement_||retiring_||!audio_||audio_.get()!=&main||
       !constructionInputsCurrent(root,grant_,resources_,sceneAssets_))return false;
    generation=grant_->generation_;epoch=grant_->epoch_;return generation&&epoch;
}
bool OriginalUiGarageSession50KG::baseImageFailurePrefixCurrent(const Application& root)const noexcept {
    return managerBaseImagesFailed12CEA0_&&!managerBaseCorpusBeforeAppendFailed_&&
        baseInputsOwnerPrefixCurrent(root);
}
bool OriginalUiGarageSession50KG::baseCorpusFailurePrefixCurrent(const Application& root)const noexcept {
    return !managerBaseImagesFailed12CEA0_&&managerBaseCorpusBeforeAppendFailed_&&
        managerBaseCorpusFailureStatus_!=assets::OriginalStandardMaterialStatus50LA::complete&&
        managerBaseCorpusFailurePC_&&entryFailureStatus_==managerBaseCorpusFailureStatus_&&
        entryFailureSourcePC_==managerBaseCorpusFailurePC_&&baseInputsOwnerPrefixCurrent(root);
}
bool OriginalUiGarageSession50KG::baseInputsFailurePrefixCurrent(const Application& root)const noexcept {
    //Two distinct failure-site receipts share only the already-owned empty
    //base prefix. A failure after calling Domain append is not either receipt.
    return baseImageFailurePrefixCurrent(root)||baseCorpusFailurePrefixCurrent(root);
}
bool OriginalUiGarageSession50KG::baseInputsOwnerPrefixCurrent(const Application& root)const noexcept {
    if(!sameRoot(root)||!failed_||!entryFrontend_||
       entryFailureStatus_==assets::OriginalStandardMaterialStatus50LA::complete||!entryFailureSourcePC_||
       managerPublished1B8274_||managerEntryPhase_!=ManagerEntryPhase::selectedReturned1D1CD0||
       managerConstructorPhase_!=ManagerConstructorPhase::notStarted||
       managerDrivePhase_!=ManagerDrivePhase::notStarted||managerLoadPhase_!=ManagerLoadPhase::notStarted||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||entryAbortPhase_!=EntryAbortPhase::notStarted||
       !scope_||!grant_||!sameCustodyShared(grant_->scope_,scope_)||scope_->issuer_!=&root||
       !sameCustodyShared(scope_->application_,authority_)||!scope_->live().okay()||
       grant_->generation_!=entryManagerGeneration_||grant_->epoch_!=scope_->managerEpoch_||
       !entryManagerGeneration_||!scope_->managerEpoch_||scope_->managerPublished_||
       scope_->managerForestRetiring_||scope_->managerRetiring_||scope_->managerSearchOwner_||
       scope_->dispatcherCurrentManager30_||!resources_||resources_->issuer_!=&root||
       resources_->generation_!=entryManagerGeneration_||
       !sameCustodyShared(resources_->application_,authority_)||
       resources_->domain_!=root.standardMaterialDomainOwner_||resources_->context_!=root.materialContextOwner_||
       !entrySelected_||entrySelected_->grant_!=grant_||entrySelected_->assets_!=sceneAssets_||
       !entrySelected_->model_||resources_->population_.size()!=1||
       resources_->population_[0]!=entrySelected_->model_||entrySelected_->visibility_||
       entrySelected_->managerSearchBorrower_||entrySelected_->managerRetiring_||
       selectedBankLease_||selectedArchiveOnlyScope1D1D88_||
       entrySearch_||entryLights_||entryFog_||widgets_||watto_||actors_||visibility_||page_||podScene_||
       !audio_||!pending_||actorInputs_||wattoInputs_||wattoAssets_||
       managerDat298_||managerPageTable144_||managerPage144Known_||manager150Known_||
       managerCurrentPage14C_||managerPageCount148_||!managerDeferred190Empty_||
       runtimeFrame_||drawContribution_||pageAttempted_||pageSceneAttempted_||actorsAttempted_||wattoAttempted_||
       !sceneAssets_||managerArchiveLease_!=sceneAssets_->archive()||
       managerDomainLease_!=root.standardMaterialDomainOwner_||!managerDomainLease_||
       managerProviderLease_!=managerDomainLease_->archiveProvider3E1A70()||!managerProviderLease_)
        return false;
    if(baseEntryAbortPhase_<BaseEntryAbortPhase::bankRetired){
        if(!managerBankLease_||!managerArchiveOnlyScope170_||!managerArchiveScopeCurrent())return false;
    }else if(managerBankLease_||managerArchiveOnlyScope170_)return false;
    if(baseEntryAbortPhase_<BaseEntryAbortPhase::managerRetired){
        if(!grant_->current()||!scope_->managerLive_||scope_->managerGeneration_!=entryManagerGeneration_||
           scope_->manager_.reference00!=1||scope_->manager_.parent1C||scope_->manager_.previous10||
           scope_->manager_.next14)return false;
    }else if(scope_->managerLive_||scope_->managerGeneration_||scope_->manager_.reference00||
             scope_->manager_.firstChild18||!scope_->garageChildren_.empty())return false;
    if(baseEntryAbortPhase_>=BaseEntryAbortPhase::selectedRetired){
        if(!entrySelected_->closed_||entrySelected_->enrolled_||entrySelected_->attached_||
           entrySelected_->placed_||!entrySelected_->model_->closed_||entrySelected_->model_->resourceHeld_||
           !scope_->garageChildren_.empty()||scope_->manager_.firstChild18)return false;
    }
    return true;
}
bool OriginalUiGarageSession50KG::hasSharedPromptFailure()const noexcept {
    return failed_&&page_&&page_->failedFirstPrompt29B638_&&entryFailureSourcePC_==0x29B638;
}
bool OriginalUiGarageSession50KG::hasPartialSceneAttachmentFailure()const noexcept {
    return failed_&&page_&&page_->failedSceneAttachment1B4A38_;
}
bool OriginalUiGarageSession50KG::partialSceneFailurePrefixCurrent(const Application& root)const noexcept {
    using P=PartialSceneAbortPhase;
    if(!sameRoot(root)||!failed_||closed_||!published_||!entryFrontend_||!grant_||!scope_||!authority_||
       !entryManagerGeneration_||scope_->issuer_!=&root||grant_->scope_.get()!=scope_.get()||
       grant_->generation_!=entryManagerGeneration_||grant_->epoch_!=scope_->managerEpoch_||
       !sameCustodyShared(authority_,root.entities())||scope_->application_!=authority_||
       entryFailureStatus_!=assets::OriginalStandardMaterialStatus50LA::source||entryFailureSourcePC_!=0x1B4A38||
       managerEntryPhase_!=ManagerEntryPhase::baseInputsReturned1CCC84||
       managerConstructorPhase_!=ManagerConstructorPhase::returned1CCDE4||
       managerDrivePhase_!=ManagerDrivePhase::pageForest||managerLoadPhase_!=ManagerLoadPhase::tableAllocated||
       managerPublished1B8274_||scope_->managerPublished_||scope_->managerForestRetiring_||
       scope_->dispatcherCurrentManager30_||managerPage0Stored_||managerCurrentPage14C_||manager150Known_||
       !managerDeferred190Empty_||!managerPage144Known_||managerPageCount148_!=1||
       !managerDat298_||!managerDrivePageData_||!managerDriveProfile_||!entryFonts_||
       !page_||!page_->failedSceneAttachment1B4A38_||
       page_->failedSceneAttachmentGeneration_!=entryManagerGeneration_||page_->failedSceneAttachmentEpoch_!=grant_->epoch_||
       !pageAttempted_||!pageSceneAttempted_||!actorsAttempted_||!wattoAttempted_||
       !actors_||!podScene_||!visibility_||runtimeFrame_||drawContribution_||
       !entrySelected_||!entrySearch_||!entryLights_||!entryFog_||!widgets_||!watto_||
       !actorInputs_||!wattoInputs_||!wattoAssets_||!audio_||!pending_||!resources_||
       resources_->issuer_!=&root||resources_->generation_!=entryManagerGeneration_||resources_->application_!=authority_||
       !sceneAssets_||managerArchiveLease_!=sceneAssets_->archive()||
       !managerDomainLease_||managerDomainLease_!=root.standardMaterialDomainOwner_||
       !managerProviderLease_||managerProviderLease_!=managerDomainLease_->archiveProvider3E1A70()||
       selectedBankLease_||selectedArchiveOnlyScope1D1D88_||managerBaseImagesFailed12CEA0_||
       managerBaseCorpusBeforeAppendFailed_||pendingRetainFailed98A4_||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||
       entryAbortPhase_!=EntryAbortPhase::notStarted||baseEntryAbortPhase_!=BaseEntryAbortPhase::notStarted||
       unenrolledSelectedAbortPhase_!=UnenrolledSelectedAbortPhase::notStarted||
       pendingRetainAbortPhase_!=PendingRetainAbortPhase::notStarted||sharedPageAbortPhase_!=SharedPageAbortPhase::notStarted||
       (partialSceneAbortPhase_==PartialSceneAbortPhase::notStarted?retiring_:!retiring_)||
       (partialSceneAbortFrontend_&&partialSceneAbortFrontend_!=entryFrontend_))return false;
    if(partialSceneAbortPhase_<P::bankRetired){
        if(!managerBankLease_||!managerArchiveOnlyScope170_||!managerArchiveScopeCurrent())return false;
    }else if(!managerBankLease_||managerArchiveOnlyScope170_)return false;
    if(partialSceneAbortPhase_<P::pageTableRetired){
        if(!managerPageTable144_||managerPageTableReleased2A3B04_)return false;
    }else if(managerPageTable144_||!managerPageTableReleased2A3B04_)return false;
    if(partialSceneAbortPhase_<P::managerRefReleased){
        if(!grant_->current()||!scope_->managerLive_||scope_->managerRetiring_||
           scope_->managerGeneration_!=entryManagerGeneration_||scope_->manager_.reference00!=1)return false;
    }else if(partialSceneAbortPhase_<P::managerRetired){
        if(!scope_->managerPayloadRetirementCurrent(entryManagerGeneration_,entrySearch_.get()))return false;
    }else if(scope_->managerLive_||scope_->managerGeneration_||scope_->manager_.reference00||
             scope_->manager_.firstChild18||!scope_->garageChildren_.empty())return false;
    if(scope_->manager_.parent1C||scope_->manager_.previous10||scope_->manager_.next14)return false;
    if(partialSceneAbortPhase_<P::pageRetired){
        if(!page_->partialSceneAttachmentFailureCurrent())return false;
        const auto* actualScene=page_->scene4C0_?page_->scene4C0_:page_->retiredSceneBacking_.get();
        if(actualScene!=podScene_||podScene_->visibility_!=visibility_)return false;
    }else{
        if(page_->failedSceneRetirement_!=OriginalUiGaragePageOwner50KG::FailedSceneRetirement::returned||
           !page_->entityCountReleased_||page_->entityLive_||!page_->entityRefReleased_||
           page_->stage_!=OriginalUiGaragePageOwner50KG::Stage::pagePayloadRetired||
           page_->scene4C0_||page_->sceneBacking_||page_->retiredSceneBacking_.get()!=podScene_||
           podScene_->sceneLive_||podScene_->retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete||
           podScene_->failedPageRetirementOwner_!=page_.get())return false;
        if(partialSceneAbortPhase_<P::managerRetired&&
           (scope_->completedPartialPageOwner_!=page_.get()||
            scope_->completedPartialPageGeneration_!=entryManagerGeneration_||
            scope_->completedPartialPageEpoch_!=grant_->epoch_))return false;
    }
    if(partialSceneAbortPhase_>=P::selectedRetired&&
       (!entrySelected_->closed_||entrySelected_->enrolled_||entrySelected_->attached_||
        !entrySelected_->model_->closed_||entrySelected_->model_->resourceHeld_))return false;
    return true;
}
bool OriginalUiGarageSession50KG::nativeAudioPartialSceneFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,bool sceneRetiring)const noexcept {
    if(!partialSceneFailurePrefixCurrent(root)||audio_.get()!=&main||generation!=entryManagerGeneration_||
       epoch!=grant_->epoch_||!podScene_||!page_)return false;
    using S=OriginalUiGaragePodScene50KG::RetirementStage;
    if(sceneRetiring)return partialSceneAbortPhase_==PartialSceneAbortPhase::pageRetiring&&
        page_->entityRefReleased_&&page_->pendingPC_==0x1FA1B4&&podScene_->retirementStage_==S::configClosed;
    //Readonly preflight is genuine both before child teardown and immediately
    //before source Scene last-ref, after Page's real ref0. No cached readiness.
    return podScene_->retirementStage_==S::none&&
        (partialSceneAbortPhase_==PartialSceneAbortPhase::notStarted||
         (partialSceneAbortPhase_==PartialSceneAbortPhase::pageRetiring&&
          ((!page_->entityRefReleased_&&
            page_->failedSceneRetirement_==OriginalUiGaragePageOwner50KG::FailedSceneRetirement::notStarted)||
           (page_->entityRefReleased_&&page_->pendingPC_==0x1FA1B4))));
}
bool OriginalUiGarageSession50KG::nativeAudioPartialSceneTerminalFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,bool managerRetired)const noexcept {
    return partialSceneFailurePrefixCurrent(root)&&audio_.get()==&main&&generation==entryManagerGeneration_&&
        epoch==grant_->epoch_&&(managerRetired?
            partialSceneAbortPhase_==PartialSceneAbortPhase::managerRetired:
            partialSceneAbortPhase_==PartialSceneAbortPhase::notStarted);
}
bool OriginalUiGarageSession50KG::hasWattoBindingFailure()const noexcept {
    return failed_&&page_&&page_->failedWattoBinding1D4800_;
}
bool OriginalUiGarageSession50KG::wattoBindingFailurePrefixCurrent(const Application& root)const noexcept {
    using P=WattoBindingAbortPhase;
    if(!sameRoot(root)||!failed_||closed_||!published_||!entryFrontend_||!grant_||!scope_||!authority_||
       !entryManagerGeneration_||scope_->issuer_!=&root||grant_->scope_.get()!=scope_.get()||
       grant_->generation_!=entryManagerGeneration_||grant_->epoch_!=scope_->managerEpoch_||
       !sameCustodyShared(authority_,root.entities())||scope_->application_!=authority_||
       entryFailureStatus_!=assets::OriginalStandardMaterialStatus50LA::source||entryFailureSourcePC_!=0x1D4800||
       managerEntryPhase_!=ManagerEntryPhase::baseInputsReturned1CCC84||
       managerConstructorPhase_!=ManagerConstructorPhase::returned1CCDE4||
       managerDrivePhase_!=ManagerDrivePhase::managerReturn||managerLoadPhase_!=ManagerLoadPhase::failed||
       managerPublished1B8274_||scope_->managerPublished_||scope_->managerForestRetiring_||
       scope_->dispatcherCurrentManager30_||!managerPage0Stored_||!manager150Known_||managerSelected150_!=1||
       !page_||managerCurrentPage14C_!=&page_->entity_||
       !managerDeferred190Empty_||!managerPage144Known_||managerPageCount148_!=1||
       managerDat298_||!managerDrivePageData_||!managerDriveProfile_||!entryFonts_||
       !page_||!page_->failedWattoBinding1D4800_||
       page_->failedWattoBindingGeneration_!=entryManagerGeneration_||page_->failedWattoBindingEpoch_!=grant_->epoch_||
       !pageAttempted_||!pageSceneAttempted_||!actorsAttempted_||!wattoAttempted_||
       !actors_||!podScene_||!visibility_||runtimeFrame_||drawContribution_||
       !entrySelected_||!entrySearch_||!entryLights_||!entryFog_||!widgets_||!watto_||
       !actorInputs_||!wattoInputs_||!wattoAssets_||!audio_||!pending_||!resources_||
       resources_->issuer_!=&root||resources_->generation_!=entryManagerGeneration_||resources_->application_!=authority_||
       !sceneAssets_||managerArchiveLease_!=sceneAssets_->archive()||
       !managerDomainLease_||managerDomainLease_!=root.standardMaterialDomainOwner_||
       !managerProviderLease_||managerProviderLease_!=managerDomainLease_->archiveProvider3E1A70()||
       selectedBankLease_||selectedArchiveOnlyScope1D1D88_||managerBaseImagesFailed12CEA0_||
       managerBaseCorpusBeforeAppendFailed_||pendingRetainFailed98A4_||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||
       entryAbortPhase_!=EntryAbortPhase::notStarted||baseEntryAbortPhase_!=BaseEntryAbortPhase::notStarted||
       unenrolledSelectedAbortPhase_!=UnenrolledSelectedAbortPhase::notStarted||
       pendingRetainAbortPhase_!=PendingRetainAbortPhase::notStarted||sharedPageAbortPhase_!=SharedPageAbortPhase::notStarted||
       partialSceneAbortPhase_!=PartialSceneAbortPhase::notStarted||lateBorrowAbortPhase_!=LateBorrowAbortPhase::notStarted||
       loadedPageAbortPhase_!=LoadedPageAbortPhase::notStarted||
       (wattoBindingAbortPhase_==WattoBindingAbortPhase::notStarted?retiring_:!retiring_)||
       (wattoBindingAbortFrontend_&&wattoBindingAbortFrontend_!=entryFrontend_))return false;
    if(wattoBindingAbortPhase_<P::bankRetired){
        if(!managerBankLease_||!managerArchiveOnlyScope170_||!managerArchiveScopeCurrent())return false;
    }else if(!managerBankLease_||managerArchiveOnlyScope170_)return false;
    if(wattoBindingAbortPhase_<P::pageTableRetired){
        if(!managerPageTable144_||managerPageTableReleased2A3B04_||managerPageTable144_[0]!=&page_->entity_)return false;
    }else if(managerPageTable144_||!managerPageTableReleased2A3B04_)return false;
    if(wattoBindingAbortPhase_<P::managerRefReleased){
        if(!grant_->current()||!scope_->managerLive_||scope_->managerRetiring_||
           scope_->managerGeneration_!=entryManagerGeneration_||scope_->manager_.reference00!=1)return false;
    }else if(wattoBindingAbortPhase_<P::managerRetired){
        if(!scope_->managerPayloadRetirementCurrent(entryManagerGeneration_,entrySearch_.get()))return false;
    }else if(scope_->managerLive_||scope_->managerGeneration_||scope_->manager_.reference00||
             scope_->manager_.firstChild18||!scope_->garageChildren_.empty())return false;
    if(scope_->manager_.parent1C||scope_->manager_.previous10||scope_->manager_.next14)return false;
    if(wattoBindingAbortPhase_<P::pageRetired){
        if(!page_->wattoBindingFailureOwnerCurrent())return false;
        const auto* actualScene=page_->scene4C0_?page_->scene4C0_:page_->retiredSceneBacking_.get();
        if(actualScene!=podScene_||podScene_->visibility_!=visibility_)return false;
    }else{
        if(page_->failedWattoBindingRetirement_!=OriginalUiGaragePageOwner50KG::FailedWattoBindingRetirement::returned||
           !page_->entityCountReleased_||page_->entityLive_||!page_->entityRefReleased_||
           page_->stage_!=OriginalUiGaragePageOwner50KG::Stage::pagePayloadRetired||
           page_->scene4C0_||page_->sceneBacking_||page_->retiredSceneBacking_.get()!=podScene_||
           podScene_->sceneLive_||podScene_->retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete||
           podScene_->failedWattoBindingRetirementOwner_!=page_.get())return false;
        if(wattoBindingAbortPhase_<P::managerRetired&&
           (scope_->completedWattoBindingPageOwner_!=page_.get()||
            scope_->completedWattoBindingPageGeneration_!=entryManagerGeneration_||
            scope_->completedWattoBindingPageEpoch_!=grant_->epoch_))return false;
    }
    if(wattoBindingAbortPhase_>=P::selectedRetired&&
       (!entrySelected_->closed_||entrySelected_->enrolled_||entrySelected_->attached_||
        !entrySelected_->model_->closed_||entrySelected_->model_->resourceHeld_))return false;
    return true;
}
bool OriginalUiGarageSession50KG::nativeAudioWattoBindingFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,bool sceneRetiring)const noexcept {
    if(!wattoBindingFailurePrefixCurrent(root)||audio_.get()!=&main||generation!=entryManagerGeneration_||
       epoch!=grant_->epoch_||!podScene_||!page_)return false;
    using S=OriginalUiGaragePodScene50KG::RetirementStage;
    if(sceneRetiring)return wattoBindingAbortPhase_==WattoBindingAbortPhase::pageRetiring&&
        page_->entityRefReleased_&&page_->pendingPC_==0x1FA1B4&&podScene_->retirementStage_==S::configClosed;
    //Readonly preflight is genuine both before child teardown and immediately
    //before source Scene last-ref, after Page's real ref0. No cached readiness.
    return podScene_->retirementStage_==S::none&&
        (wattoBindingAbortPhase_==WattoBindingAbortPhase::notStarted||
         (wattoBindingAbortPhase_==WattoBindingAbortPhase::pageRetiring&&
          ((!page_->entityRefReleased_&&
            page_->failedWattoBindingRetirement_==OriginalUiGaragePageOwner50KG::FailedWattoBindingRetirement::notStarted)||
           (page_->entityRefReleased_&&page_->pendingPC_==0x1FA1B4))));
}
bool OriginalUiGarageSession50KG::nativeAudioWattoBindingTerminalFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,bool managerRetired)const noexcept {
    return wattoBindingFailurePrefixCurrent(root)&&audio_.get()==&main&&generation==entryManagerGeneration_&&
        epoch==grant_->epoch_&&(managerRetired?
            wattoBindingAbortPhase_==WattoBindingAbortPhase::managerRetired:
            wattoBindingAbortPhase_==WattoBindingAbortPhase::notStarted);
}
bool OriginalUiGarageSession50KG::hasLoadedPageHideFailure()const noexcept {
    return failed_&&page_&&page_->failedLoadedPageHide1415A0_;
}
bool OriginalUiGarageSession50KG::loadedPageFailurePrefixCurrent(const Application& root)const noexcept {
    using P=LoadedPageAbortPhase;
    if(!sameRoot(root)||!failed_||closed_||!published_||!entryFrontend_||!grant_||!scope_||!authority_||
       !entryManagerGeneration_||scope_->issuer_!=&root||grant_->scope_.get()!=scope_.get()||
       grant_->generation_!=entryManagerGeneration_||grant_->epoch_!=scope_->managerEpoch_||
       !sameCustodyShared(authority_,root.entities())||scope_->application_!=authority_||
       entryFailureStatus_!=assets::OriginalStandardMaterialStatus50LA::source||entryFailureSourcePC_!=0x1415A0||
       managerEntryPhase_!=ManagerEntryPhase::baseInputsReturned1CCC84||
       managerConstructorPhase_!=ManagerConstructorPhase::returned1CCDE4||
       managerDrivePhase_!=ManagerDrivePhase::managerReturn||managerLoadPhase_!=ManagerLoadPhase::failed||
       managerPublished1B8274_||scope_->managerPublished_||scope_->managerForestRetiring_||
       scope_->dispatcherCurrentManager30_||!managerPage0Stored_||managerCurrentPage14C_||manager150Known_||
       !managerDeferred190Empty_||!managerPage144Known_||managerPageCount148_!=1||
       !managerDat298_||!managerDrivePageData_||!managerDriveProfile_||!entryFonts_||
       !page_||!page_->failedLoadedPageHide1415A0_||
       page_->failedLoadedPageHideGeneration_!=entryManagerGeneration_||page_->failedLoadedPageHideEpoch_!=grant_->epoch_||
       !pageAttempted_||!pageSceneAttempted_||!actorsAttempted_||!wattoAttempted_||
       !actors_||!podScene_||!visibility_||runtimeFrame_||drawContribution_||
       !entrySelected_||!entrySearch_||!entryLights_||!entryFog_||!widgets_||!watto_||
       !actorInputs_||!wattoInputs_||!wattoAssets_||!audio_||!pending_||!resources_||
       resources_->issuer_!=&root||resources_->generation_!=entryManagerGeneration_||resources_->application_!=authority_||
       !sceneAssets_||managerArchiveLease_!=sceneAssets_->archive()||
       !managerDomainLease_||managerDomainLease_!=root.standardMaterialDomainOwner_||
       !managerProviderLease_||managerProviderLease_!=managerDomainLease_->archiveProvider3E1A70()||
       selectedBankLease_||selectedArchiveOnlyScope1D1D88_||managerBaseImagesFailed12CEA0_||
       managerBaseCorpusBeforeAppendFailed_||pendingRetainFailed98A4_||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||
       entryAbortPhase_!=EntryAbortPhase::notStarted||baseEntryAbortPhase_!=BaseEntryAbortPhase::notStarted||
       unenrolledSelectedAbortPhase_!=UnenrolledSelectedAbortPhase::notStarted||
       pendingRetainAbortPhase_!=PendingRetainAbortPhase::notStarted||sharedPageAbortPhase_!=SharedPageAbortPhase::notStarted||
       partialSceneAbortPhase_!=PartialSceneAbortPhase::notStarted||lateBorrowAbortPhase_!=LateBorrowAbortPhase::notStarted||
       (loadedPageAbortPhase_==LoadedPageAbortPhase::notStarted?retiring_:!retiring_)||
       (loadedPageAbortFrontend_&&loadedPageAbortFrontend_!=entryFrontend_))return false;
    if(loadedPageAbortPhase_<P::bankRetired){
        if(!managerBankLease_||!managerArchiveOnlyScope170_||!managerArchiveScopeCurrent())return false;
    }else if(!managerBankLease_||managerArchiveOnlyScope170_)return false;
    if(loadedPageAbortPhase_<P::pageTableRetired){
        if(!managerPageTable144_||managerPageTableReleased2A3B04_||managerPageTable144_[0]!=&page_->entity_)return false;
    }else if(managerPageTable144_||!managerPageTableReleased2A3B04_)return false;
    if(loadedPageAbortPhase_<P::managerRefReleased){
        if(!grant_->current()||!scope_->managerLive_||scope_->managerRetiring_||
           scope_->managerGeneration_!=entryManagerGeneration_||scope_->manager_.reference00!=1)return false;
    }else if(loadedPageAbortPhase_<P::managerRetired){
        if(!scope_->managerPayloadRetirementCurrent(entryManagerGeneration_,entrySearch_.get()))return false;
    }else if(scope_->managerLive_||scope_->managerGeneration_||scope_->manager_.reference00||
             scope_->manager_.firstChild18||!scope_->garageChildren_.empty())return false;
    if(scope_->manager_.parent1C||scope_->manager_.previous10||scope_->manager_.next14)return false;
    if(loadedPageAbortPhase_<P::pageRetired){
        if(!page_->loadedHideFailureOwnerCurrent())return false;
        const auto* actualScene=page_->scene4C0_?page_->scene4C0_:page_->retiredSceneBacking_.get();
        if(actualScene!=podScene_||podScene_->visibility_!=visibility_)return false;
    }else{
        if(page_->failedLoadedHideRetirement_!=OriginalUiGaragePageOwner50KG::FailedLoadedHideRetirement::returned||
           !page_->entityCountReleased_||page_->entityLive_||!page_->entityRefReleased_||
           page_->stage_!=OriginalUiGaragePageOwner50KG::Stage::pagePayloadRetired||
           page_->scene4C0_||page_->sceneBacking_||page_->retiredSceneBacking_.get()!=podScene_||
           podScene_->sceneLive_||podScene_->retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete||
           podScene_->failedLoadedHideRetirementOwner_!=page_.get())return false;
        if(loadedPageAbortPhase_<P::managerRetired&&
           (scope_->completedLoadedHidePageOwner_!=page_.get()||
            scope_->completedLoadedHidePageGeneration_!=entryManagerGeneration_||
            scope_->completedLoadedHidePageEpoch_!=grant_->epoch_))return false;
    }
    if(loadedPageAbortPhase_>=P::selectedRetired&&
       (!entrySelected_->closed_||entrySelected_->enrolled_||entrySelected_->attached_||
        !entrySelected_->model_->closed_||entrySelected_->model_->resourceHeld_))return false;
    return true;
}
bool OriginalUiGarageSession50KG::nativeAudioLoadedPageFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,bool sceneRetiring)const noexcept {
    if(!loadedPageFailurePrefixCurrent(root)||audio_.get()!=&main||generation!=entryManagerGeneration_||
       epoch!=grant_->epoch_||!podScene_||!page_)return false;
    using S=OriginalUiGaragePodScene50KG::RetirementStage;
    if(sceneRetiring)return loadedPageAbortPhase_==LoadedPageAbortPhase::pageRetiring&&
        page_->entityRefReleased_&&page_->pendingPC_==0x1FA1B4&&podScene_->retirementStage_==S::configClosed;
    //Readonly preflight is genuine both before child teardown and immediately
    //before source Scene last-ref, after Page's real ref0. No cached readiness.
    return podScene_->retirementStage_==S::none&&
        (loadedPageAbortPhase_==LoadedPageAbortPhase::notStarted||
         (loadedPageAbortPhase_==LoadedPageAbortPhase::pageRetiring&&
          ((!page_->entityRefReleased_&&
            page_->failedLoadedHideRetirement_==OriginalUiGaragePageOwner50KG::FailedLoadedHideRetirement::notStarted)||
           (page_->entityRefReleased_&&page_->pendingPC_==0x1FA1B4))));
}
bool OriginalUiGarageSession50KG::nativeAudioLoadedPageTerminalFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,bool managerRetired)const noexcept {
    return loadedPageFailurePrefixCurrent(root)&&audio_.get()==&main&&generation==entryManagerGeneration_&&
        epoch==grant_->epoch_&&(managerRetired?
            loadedPageAbortPhase_==LoadedPageAbortPhase::managerRetired:
            loadedPageAbortPhase_==LoadedPageAbortPhase::notStarted);
}
bool OriginalUiGarageSession50KG::hasLateSceneBorrowFailure()const noexcept {
    return failed_&&page_&&page_->failedLateBorrow1B4D24_;
}
bool OriginalUiGarageSession50KG::lateBorrowFailurePrefixCurrent(const Application& root)const noexcept {
    using P=LateBorrowAbortPhase;
    if(!sameRoot(root)||!failed_||closed_||!published_||!entryFrontend_||!grant_||!scope_||!authority_||
       !entryManagerGeneration_||scope_->issuer_!=&root||grant_->scope_.get()!=scope_.get()||
       grant_->generation_!=entryManagerGeneration_||grant_->epoch_!=scope_->managerEpoch_||
       !sameCustodyShared(authority_,root.entities())||scope_->application_!=authority_||
       entryFailureStatus_!=assets::OriginalStandardMaterialStatus50LA::source||entryFailureSourcePC_!=0x1B4D24||
       managerEntryPhase_!=ManagerEntryPhase::baseInputsReturned1CCC84||
       managerConstructorPhase_!=ManagerConstructorPhase::returned1CCDE4||
       managerDrivePhase_!=ManagerDrivePhase::lateForest||managerLoadPhase_!=ManagerLoadPhase::tableAllocated||
       managerPublished1B8274_||scope_->managerPublished_||scope_->managerForestRetiring_||
       scope_->dispatcherCurrentManager30_||managerPage0Stored_||managerCurrentPage14C_||manager150Known_||
       !managerDeferred190Empty_||!managerPage144Known_||managerPageCount148_!=1||
       !managerDat298_||!managerDrivePageData_||!managerDriveProfile_||!entryFonts_||
       !page_||!page_->failedLateBorrow1B4D24_||
       page_->failedLateBorrowGeneration_!=entryManagerGeneration_||page_->failedLateBorrowEpoch_!=grant_->epoch_||
       !pageAttempted_||!pageSceneAttempted_||!actorsAttempted_||!wattoAttempted_||
       !actors_||!podScene_||!visibility_||runtimeFrame_||drawContribution_||
       !entrySelected_||!entrySearch_||!entryLights_||!entryFog_||!widgets_||!watto_||
       !actorInputs_||!wattoInputs_||!wattoAssets_||!audio_||!pending_||!resources_||
       resources_->issuer_!=&root||resources_->generation_!=entryManagerGeneration_||resources_->application_!=authority_||
       !sceneAssets_||managerArchiveLease_!=sceneAssets_->archive()||
       !managerDomainLease_||managerDomainLease_!=root.standardMaterialDomainOwner_||
       !managerProviderLease_||managerProviderLease_!=managerDomainLease_->archiveProvider3E1A70()||
       selectedBankLease_||selectedArchiveOnlyScope1D1D88_||managerBaseImagesFailed12CEA0_||
       managerBaseCorpusBeforeAppendFailed_||pendingRetainFailed98A4_||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||
       entryAbortPhase_!=EntryAbortPhase::notStarted||baseEntryAbortPhase_!=BaseEntryAbortPhase::notStarted||
       unenrolledSelectedAbortPhase_!=UnenrolledSelectedAbortPhase::notStarted||
       pendingRetainAbortPhase_!=PendingRetainAbortPhase::notStarted||sharedPageAbortPhase_!=SharedPageAbortPhase::notStarted||
       partialSceneAbortPhase_!=PartialSceneAbortPhase::notStarted||
       (lateBorrowAbortPhase_==LateBorrowAbortPhase::notStarted?retiring_:!retiring_)||
       (lateBorrowAbortFrontend_&&lateBorrowAbortFrontend_!=entryFrontend_))return false;
    if(lateBorrowAbortPhase_<P::bankRetired){
        if(!managerBankLease_||!managerArchiveOnlyScope170_||!managerArchiveScopeCurrent())return false;
    }else if(!managerBankLease_||managerArchiveOnlyScope170_)return false;
    if(lateBorrowAbortPhase_<P::pageTableRetired){
        if(!managerPageTable144_||managerPageTableReleased2A3B04_)return false;
    }else if(managerPageTable144_||!managerPageTableReleased2A3B04_)return false;
    if(lateBorrowAbortPhase_<P::managerRefReleased){
        if(!grant_->current()||!scope_->managerLive_||scope_->managerRetiring_||
           scope_->managerGeneration_!=entryManagerGeneration_||scope_->manager_.reference00!=1)return false;
    }else if(lateBorrowAbortPhase_<P::managerRetired){
        if(!scope_->managerPayloadRetirementCurrent(entryManagerGeneration_,entrySearch_.get()))return false;
    }else if(scope_->managerLive_||scope_->managerGeneration_||scope_->manager_.reference00||
             scope_->manager_.firstChild18||!scope_->garageChildren_.empty())return false;
    if(scope_->manager_.parent1C||scope_->manager_.previous10||scope_->manager_.next14)return false;
    if(lateBorrowAbortPhase_<P::pageRetired){
        if(!page_->lateBorrowFailureOwnerCurrent())return false;
        const auto* actualScene=page_->scene4C0_?page_->scene4C0_:page_->retiredSceneBacking_.get();
        if(actualScene!=podScene_||podScene_->visibility_!=visibility_)return false;
    }else{
        if(page_->failedLateBorrowRetirement_!=OriginalUiGaragePageOwner50KG::FailedLateBorrowRetirement::returned||
           !page_->entityCountReleased_||page_->entityLive_||!page_->entityRefReleased_||
           page_->stage_!=OriginalUiGaragePageOwner50KG::Stage::pagePayloadRetired||
           page_->scene4C0_||page_->sceneBacking_||page_->retiredSceneBacking_.get()!=podScene_||
           podScene_->sceneLive_||podScene_->retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete||
           podScene_->failedLateBorrowRetirementOwner_!=page_.get())return false;
        if(lateBorrowAbortPhase_<P::managerRetired&&
           (scope_->completedLateBorrowPageOwner_!=page_.get()||
            scope_->completedLateBorrowPageGeneration_!=entryManagerGeneration_||
            scope_->completedLateBorrowPageEpoch_!=grant_->epoch_))return false;
    }
    if(lateBorrowAbortPhase_>=P::selectedRetired&&
       (!entrySelected_->closed_||entrySelected_->enrolled_||entrySelected_->attached_||
        !entrySelected_->model_->closed_||entrySelected_->model_->resourceHeld_))return false;
    return true;
}
bool OriginalUiGarageSession50KG::nativeAudioLateBorrowFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,bool sceneRetiring)const noexcept {
    if(!lateBorrowFailurePrefixCurrent(root)||audio_.get()!=&main||generation!=entryManagerGeneration_||
       epoch!=grant_->epoch_||!podScene_||!page_)return false;
    using S=OriginalUiGaragePodScene50KG::RetirementStage;
    if(sceneRetiring)return lateBorrowAbortPhase_==LateBorrowAbortPhase::pageRetiring&&
        page_->entityRefReleased_&&page_->pendingPC_==0x1FA1B4&&podScene_->retirementStage_==S::configClosed;
    //Readonly preflight is genuine both before child teardown and immediately
    //before source Scene last-ref, after Page's real ref0. No cached readiness.
    return podScene_->retirementStage_==S::none&&
        (lateBorrowAbortPhase_==LateBorrowAbortPhase::notStarted||
         (lateBorrowAbortPhase_==LateBorrowAbortPhase::pageRetiring&&
          ((!page_->entityRefReleased_&&
            page_->failedLateBorrowRetirement_==OriginalUiGaragePageOwner50KG::FailedLateBorrowRetirement::notStarted)||
           (page_->entityRefReleased_&&page_->pendingPC_==0x1FA1B4))));
}
bool OriginalUiGarageSession50KG::nativeAudioLateBorrowTerminalFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,bool managerRetired)const noexcept {
    return lateBorrowFailurePrefixCurrent(root)&&audio_.get()==&main&&generation==entryManagerGeneration_&&
        epoch==grant_->epoch_&&(managerRetired?
            lateBorrowAbortPhase_==LateBorrowAbortPhase::managerRetired:
            lateBorrowAbortPhase_==LateBorrowAbortPhase::notStarted);
}
bool OriginalUiGarageSession50KG::sharedPageFailurePrefixCurrent(const Application& root)const noexcept {
    using P=SharedPageAbortPhase;
    if(!sameRoot(root)||!failed_||closed_||!published_||!entryFrontend_||
       !grant_||!scope_||!authority_||!entryManagerGeneration_||
       scope_->issuer_!=&root||grant_->scope_.get()!=scope_.get()||
       grant_->generation_!=entryManagerGeneration_||grant_->epoch_!=scope_->managerEpoch_||
       !sameCustodyShared(authority_,root.entities())||scope_->application_!=authority_||
       entryFailureStatus_!=assets::OriginalStandardMaterialStatus50LA::source||entryFailureSourcePC_!=0x29B638||
       managerEntryPhase_!=ManagerEntryPhase::baseInputsReturned1CCC84||
       managerConstructorPhase_!=ManagerConstructorPhase::returned1CCDE4||
       managerDrivePhase_!=ManagerDrivePhase::sharedLoader||managerLoadPhase_!=ManagerLoadPhase::tableAllocated||
       managerPublished1B8274_||scope_->managerPublished_||scope_->managerForestRetiring_||
       scope_->dispatcherCurrentManager30_||managerPage0Stored_||managerCurrentPage14C_||manager150Known_||
       !managerDeferred190Empty_||!managerPage144Known_||managerPageCount148_!=1||
       !managerDat298_||!managerDrivePageData_||!managerDriveProfile_||!entryFonts_||
       !page_||!page_->failedFirstPrompt29B638_||page_->failedPromptGeneration_!=entryManagerGeneration_||
       page_->failedPromptEpoch_!=grant_->epoch_||!pageAttempted_||pageSceneAttempted_||actorsAttempted_||
       !wattoAttempted_||actors_||podScene_||visibility_||runtimeFrame_||drawContribution_||
       !entrySelected_||!entrySearch_||!entryLights_||!entryFog_||!widgets_||!watto_||
       actorInputs_||!wattoInputs_||!wattoAssets_||!audio_||!pending_||!resources_||
       resources_->issuer_!=&root||resources_->generation_!=entryManagerGeneration_||
       resources_->application_!=authority_||resources_->population_.size()!=2||
       !sceneAssets_||managerArchiveLease_!=sceneAssets_->archive()||
       !managerDomainLease_||managerDomainLease_!=root.standardMaterialDomainOwner_||
       managerProviderLease_!=managerDomainLease_->archiveProvider3E1A70()||!managerProviderLease_||
       selectedBankLease_||selectedArchiveOnlyScope1D1D88_||managerBaseImagesFailed12CEA0_||
       managerBaseCorpusBeforeAppendFailed_||pendingRetainFailed98A4_||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||
       entryAbortPhase_!=EntryAbortPhase::notStarted||baseEntryAbortPhase_!=BaseEntryAbortPhase::notStarted||
       unenrolledSelectedAbortPhase_!=UnenrolledSelectedAbortPhase::notStarted||
       pendingRetainAbortPhase_!=PendingRetainAbortPhase::notStarted||
       (sharedPageAbortPhase_==P::notStarted?retiring_:!retiring_)||
       (sharedPageAbortFrontend_&&sharedPageAbortFrontend_!=entryFrontend_))return false;
    if(sharedPageAbortPhase_<P::bankRetired){
        if(!managerBankLease_||!managerArchiveOnlyScope170_||!managerArchiveScopeCurrent())return false;
    }else if(!managerBankLease_||managerArchiveOnlyScope170_)return false;
    if(sharedPageAbortPhase_<P::pageTableRetired){
        if(!managerPageTable144_||managerPageTableReleased2A3B04_)return false;
    }else if(managerPageTable144_||!managerPageTableReleased2A3B04_)return false;
    if(sharedPageAbortPhase_<P::managerRefReleased){
        if(!grant_->current()||!scope_->managerLive_||scope_->managerRetiring_||
           scope_->managerGeneration_!=entryManagerGeneration_||scope_->manager_.reference00!=1)return false;
    }else if(sharedPageAbortPhase_<P::managerRetired){
        if(!scope_->managerPayloadRetirementCurrent(entryManagerGeneration_,entrySearch_.get()))return false;
    }else if(scope_->managerLive_||scope_->managerGeneration_||scope_->manager_.reference00||
             scope_->manager_.firstChild18||!scope_->garageChildren_.empty())return false;
    if(scope_->manager_.parent1C||scope_->manager_.previous10||scope_->manager_.next14)return false;
    if(sharedPageAbortPhase_<P::pageRetired){
        if(!page_->sharedPromptFailurePrefixCurrent())return false;
    }else if(page_->failedSharedRetirement_!=OriginalUiGaragePageOwner50KG::FailedSharedRetirement::returned||
             !page_->entityCountReleased_||page_->entityLive_||page_->stage_!=OriginalUiGaragePageOwner50KG::Stage::pagePayloadRetired||page_->sceneBacking_)return false;
    if(sharedPageAbortPhase_>=P::selectedRetired&&
       (!entrySelected_->closed_||entrySelected_->enrolled_||entrySelected_->attached_||
        !entrySelected_->model_->closed_||entrySelected_->model_->resourceHeld_))return false;
    return true;
}
bool OriginalUiGarageSession50KG::preflightSharedPageFailure(D& d)const noexcept {
    d={};
    auto fail=[&](std::uint32_t pc){d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=pc;return false;};
    if(!issuer_||!sharedPageFailurePrefixCurrent(*issuer_)||sharedPageAbortPhase_!=SharedPageAbortPhase::notStarted||
       !scope_->populatedGarageManagerForestCurrent(entryManagerGeneration_,false)||
       !entrySelected_->current()||!entrySearch_->current()||!entryLights_->current()||
       !entryFog_->currentAttached()||!widgets_->currentColdComplete())return fail(0x29B638);
    const auto& selected=entrySelected_->model_;
    OriginalUiGarageWattoOwner50KG::VisibilityBinding watto;
    if(!watto_->visibilityBinding(watto)||watto.grant!=grant_||watto.visibility||watto.primary4C0||
       !watto.model||resources_->population_[0]!=selected||resources_->population_[1]!=watto.model||
       selected->scene_.model()->observer19C||!selected->canRetireResources(nullptr)||
       !watto_->preflightColdRetirement142590(d))return fail(d.sourcePC?d.sourcePC:0x142590);
    if(!entryLights_->manager_||!entryLights_->manager_->preflightRemoveUiLight1491F0(*entryLights_,1))
        return fail(0x1491F0);
    // The actual tail preflight cannot yet remove row0 while row1 is live.
    // Inspect its same retained registry entry and all later removal operands
    // without changing count94 or pretending that row1 already returned.
    const auto& ambient=*entryLights_->lights_[0];
    const auto& lightManager=*entryLights_->manager_;
    if(lightManager.count94_!=2||!lightManager.registry9C_||lightManager.registry9C_[0]!=&ambient.entity||
       ambient.observerD0||ambient.fieldD8||ambient.fieldC0||ambient.fieldC4||ambient.type34!=4)
        return fail(0x1491F0);
    OriginalUiGaragePageOwner50KG::Diagnostic page;
    if(!page_->preflightFailedSharedPrompt(page))return fail(page.sourcePC);
    OriginalUiGarageWidget1Diagnostic50KG widget;
    if(!widgets_->preflightFailedSharedPageRows(*page_,widget))return fail(widget.sourcePC);
    return true;
}
bool OriginalUiGarageSession50KG::nativeAudioSharedPageFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,bool managerRetired)const noexcept {
    return sharedPageFailurePrefixCurrent(root)&&audio_.get()==&main&&generation==entryManagerGeneration_&&
        epoch==grant_->epoch_&&(managerRetired?
            sharedPageAbortPhase_==SharedPageAbortPhase::managerRetired:
            sharedPageAbortPhase_==SharedPageAbortPhase::notStarted);
}
bool OriginalUiGarageSession50KG::preflightBaseSelectedAbort(D& d)const noexcept {
    d={};
    if(!issuer_||!baseInputsFailurePrefixCurrent(*issuer_)||
       baseEntryAbortPhase_!=BaseEntryAbortPhase::notStarted||retiring_||!entrySelected_->current()){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x1D18D0;return false;
    }
    return preflightAttachedSelectedAbort(d);
}
bool OriginalUiGarageSession50KG::preflightAttachedSelectedAbort(D& d)const noexcept {
    d={};
    if(!scope_||!grant_||!entrySelected_||!entrySelected_->current()){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x1D18D0;return false;
    }
    const auto& model=entrySelected_->model_;const auto* node=model->scene_.modelEntity8();
    if(!node||node->previous10||node->next14||node->firstChild18||node->parent1C!=&scope_->manager_||
       scope_->manager_.firstChild18!=node||scope_->garageChildren_.size()!=1||
       scope_->garageChildren_[0].node!=node||!scope_->garageChildren_[0].model||
       model->scene_.model()->observer19C||!model->canRetireResources(nullptr)){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x142590;return false;
    }
    const auto step=scope_->preflightGarageModelRetirement(entryManagerGeneration_,model->scene_);
    if(!step.okay()){d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=step.sourcePC;return false;}
    return true;
}
bool OriginalUiGarageSession50KG::retireBaseSelectedAbort(D& d)noexcept {
    d={};if(baseEntryAbortPhase_!=BaseEntryAbortPhase::notStarted||!retiring_||
       !baseEntryAbortFrontend_||!issuer_||!baseInputsFailurePrefixCurrent(*issuer_)){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x1D18D0;return false;
    }
    // The actual existing selected owner performs Scope142590 followed by
    // packet/material/last Geometry release. No detached stand-in is created.
    if(!entrySelected_->close1D18D0(d))return false;
    baseEntryAbortPhase_=BaseEntryAbortPhase::selectedRetired;return true;
}
bool OriginalUiGarageSession50KG::pendingRetainFailurePrefixCurrent(const Application& root)const noexcept {
    using P=PendingRetainAbortPhase;
    if(!sameRoot(root)||!failed_||!pendingRetainFailed98A4_||!entryFrontend_||
       entryFailureStatus_!=assets::OriginalStandardMaterialStatus50LA::source||entryFailureSourcePC_!=0x1CCC84||
       managerPublished1B8274_||managerEntryPhase_!=ManagerEntryPhase::selectedReturned1D1CD0||
       managerConstructorPhase_!=ManagerConstructorPhase::notStarted||
       managerDrivePhase_!=ManagerDrivePhase::notStarted||managerLoadPhase_!=ManagerLoadPhase::notStarted||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||entryAbortPhase_!=EntryAbortPhase::notStarted||
       baseEntryAbortPhase_!=BaseEntryAbortPhase::notStarted||managerBaseImagesFailed12CEA0_||
       unenrolledSelectedAbortPhase_!=UnenrolledSelectedAbortPhase::notStarted||hasUnenrolledSelectedFailure()||
       !scope_||!grant_||!sameCustodyShared(grant_->scope_,scope_)||scope_->issuer_!=&root||
       !sameCustodyShared(scope_->application_,authority_)||!scope_->live().okay()||
       grant_->generation_!=entryManagerGeneration_||grant_->epoch_!=scope_->managerEpoch_||
       !entryManagerGeneration_||!scope_->managerEpoch_||scope_->managerPublished_||
       scope_->managerForestRetiring_||scope_->managerRetiring_||scope_->managerSearchOwner_||
       scope_->dispatcherCurrentManager30_||!resources_||resources_->issuer_!=&root||
       resources_->generation_!=entryManagerGeneration_||!sameCustodyShared(resources_->application_,authority_)||
       resources_->domain_!=root.standardMaterialDomainOwner_||resources_->context_!=root.materialContextOwner_||
       !entrySelected_||entrySelected_->grant_!=grant_||entrySelected_->assets_!=sceneAssets_||
       !entrySelected_->model_||resources_->population_.size()!=1||
       resources_->population_[0]!=entrySelected_->model_||entrySelected_->visibility_||
       entrySelected_->managerSearchBorrower_||entrySelected_->managerRetiring_||
       selectedBankLease_||selectedArchiveOnlyScope1D1D88_||
       entrySearch_||entryLights_||entryFog_||widgets_||watto_||actors_||visibility_||page_||podScene_||
       !audio_||pending_||actorInputs_||wattoInputs_||wattoAssets_||
       managerDat298_||managerArchiveOnlyScope170_||managerBankLease_||
       managerArchiveLease_||managerProviderLease_||managerDomainLease_||
       managerPageTable144_||managerPage144Known_||manager150Known_||managerCurrentPage14C_||
       managerPageCount148_||!managerDeferred190Empty_||runtimeFrame_||drawContribution_||
       pageAttempted_||pageSceneAttempted_||actorsAttempted_||wattoAttempted_)return false;
    if(pendingRetainAbortPhase_<P::managerRetired){
        if(!grant_->current()||!scope_->managerLive_||scope_->managerGeneration_!=entryManagerGeneration_||
           scope_->manager_.reference00!=1||scope_->manager_.parent1C||scope_->manager_.previous10||
           scope_->manager_.next14)return false;
    }else if(scope_->managerLive_||scope_->managerGeneration_||scope_->manager_.reference00)return false;
    if(pendingRetainAbortPhase_<P::selectedRetired){
        if(!entrySelected_->current())return false;
    }else if(!entrySelected_->closed_||entrySelected_->enrolled_||entrySelected_->attached_||
             entrySelected_->placed_||!entrySelected_->model_->closed_||entrySelected_->model_->resourceHeld_||
             scope_->manager_.firstChild18||!scope_->garageChildren_.empty())return false;
    return true;
}
bool OriginalUiGarageSession50KG::retirePendingSelectedAbort(D& d)noexcept {
    d={};
    if(!issuer_||!pendingRetainFailurePrefixCurrent(*issuer_)||!retiring_||!pendingRetainAbortFrontend_||
       pendingRetainAbortPhase_!=PendingRetainAbortPhase::notStarted){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x1D18D0;return false;
    }
    if(!entrySelected_->close1D18D0(d))return false;
    pendingRetainAbortPhase_=PendingRetainAbortPhase::selectedRetired;return true;
}
bool OriginalUiGarageSession50KG::nativeAudioEmptyEntryFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,
    bool managerRetired)const noexcept {
    if(!baseInputsFailurePrefixCurrent(root)||audio_.get()!=&main||!generation||!epoch||
       generation!=entryManagerGeneration_||generation!=grant_->generation_||epoch!=grant_->epoch_)return false;
    if(managerRetired)return retiring_&&baseEntryAbortFrontend_==entryFrontend_&&
        baseEntryAbortPhase_==BaseEntryAbortPhase::managerRetired;
    return !retiring_&&baseEntryAbortPhase_==BaseEntryAbortPhase::notStarted;
}
bool OriginalUiGarageSession50KG::nativeAudioRetirementIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch)const noexcept {
    if(actorSampleFailed10179C_||publishedObservationFailed1B8274_||unsealedRenderRetirement_||!sameRoot(root)||!retiring_||!audio_||audio_.get()!=&main||!grant_||!scope_||!generation||!epoch||
       !sameCustodyShared(grant_->scope_,scope_)||scope_->issuer_!=&root||
       !sameCustodyShared(scope_->application_,authority_)||scope_->managerEpoch_!=epoch||
       grant_->generation_!=generation||grant_->epoch_!=epoch||normalManagerGeneration_!=generation)
        return false;
    if(normalRetirementPhase_==NormalRetirementPhase::currentCleared){
        // Actual finishGarageManagerPayloadRetirement clears the current
        // generation to0. Only its retained completed Page/epoch plus the
        // exhaustive final capsule proof can identify that historical owner.
        if(scope_->managerGeneration_||!page_||scope_->completedManagerPage1413E0_!=page_.get()||
           scope_->completedManagerPageGeneration_!=generation||scope_->completedManagerPageEpoch_!=epoch)
            return false;
        D retired;std::string detail;return normalSourceOwnersRetired(retired,detail);
    }
    // An in-progress retirement is NOT a new grant or a completion receipt.
    // Never accept generation0 just because a destructor was entered.
    return normalRetirementPhase_!=NormalRetirementPhase::notStarted&&scope_->managerLive_&&
        scope_->managerGeneration_==generation&&scope_->dispatcherCurrentManager30_==&scope_->manager_&&
        scope_->dispatcherCurrentManagerGeneration30_==generation;
}
bool OriginalUiGarageSession50KG::constructionCurrent(const Application& root)const noexcept {
    return sameRoot(root)&&!failed_&&!unsealedRenderRetirement_&&!retiring_&&
        (!entryFrontend_||(managerEntryPhase_==ManagerEntryPhase::baseInputsReturned1CCC84&&
          entrySelected_&&entrySelected_->current()))&&
        sameCustodyShared(application_,root.state)&&
        sameCustodyShared(motions_,root.motionLibrary4333C0())&&
        constructionInputsCurrent(root,grant_,resources_,sceneAssets_)&&pending_&&pending_->current()&&
        (!podScene_? !visibility_ : (page_&&page_->borrowScene4C0()==podScene_&&
        podScene_->currentPrefix()&&podScene_->visibility_==visibility_&&
        (!visibility_?!actorsAttempted_:visibility_->matchesColdConstruction(grant_,sceneAssets_))));
}
bool OriginalUiGarageSession50KG::publishedOwnerCurrent(const Application& root)const noexcept {rr::game::EntityForestReadScope50LA forestReadScope50LA;
    // Native retained custody, not a new load or frame-return receipt. The
    // real Search/render histories survive their calls and must not be zeroed
    // to reuse the detached-construction predicate after manager publication.
    return sameRoot(root)&&!failed_&&!actorSampleFailed10179C_&&!unsealedRenderRetirement_&&!retiring_&&entryFrontend_&&
        managerEntryPhase_==ManagerEntryPhase::baseInputsReturned1CCC84&&
        entrySelected_&&entrySelected_->current()&&
        sameCustodyShared(application_,root.state)&&
        sameCustodyShared(motions_,root.motionLibrary4333C0())&&
        constructionInputsCurrent(root,grant_,resources_,sceneAssets_)&&pending_&&pending_->current()&&
        managerDrivePhase_==ManagerDrivePhase::returned&&
        managerLoadPhase_==ManagerLoadPhase::published&&managerPublished1B8274_&&scope_&&
        scope_->publishedPopulatedGarageManagerCurrent(grant_->generation_)&&
        page_&&podScene_&&visibility_&&page_->borrowScene4C0()==podScene_&&
        podScene_->currentPrefix()&&podScene_->visibility_==visibility_&&
        page_->sourceEntryReturned1FAE00()&&
        visibility_->matchesRetainedSceneOwnership(grant_,sceneAssets_,*page_,*podScene_);
}
bool OriginalUiGarageSession50KG::widgetPredecessorCurrent(
    const std::shared_ptr<OriginalUiGarageWidget1Owner50KG>& widgets)const noexcept {
    if(!widgets||!widgets->currentColdPrefix9()||widgets->issuer_!=issuer_||
        !sameCustodyShared(widgets->authority_,authority_)||
        !sameCustodyShared(widgets->grant_,grant_)||
        !sameCustodyShared(widgets->archive_,sceneAssets_->archive())||!widgets->fog_)return false;
    const auto& fog=*widgets->fog_;
    if(!sameCustodyShared(fog.grant_,grant_)||!fog.lights_)return false;
    const auto& lights=*fog.lights_;
    if(!sameCustodyShared(lights.grant_,grant_)||!sameCustodyShared(lights.assets_,sceneAssets_)||
        !lights.search_)return false;
    const auto& search=*lights.search_;
    if(!sameCustodyShared(search.grant_,grant_)||!search.selected_)return false;
    if(entryFrontend_&&(!sameCustodyShared(entrySelected_,search.selected_)||
       !sameCustodyShared(entrySearch_,lights.search_)||!sameCustodyShared(entryLights_,fog.lights_)||
       !sameCustodyShared(entryFog_,widgets->fog_)||!sameCustodyShared(widgets_,widgets)))return false;
    const auto& selected=*search.selected_;
    if(!sameCustodyShared(selected.grant_,grant_)||!sameCustodyShared(selected.assets_,sceneAssets_))return false;
    // The retained selected predecessor must be a member of THIS exact shared
    // model population, not a parallel resource container with equal bytes.
    for(const auto& model:resources_->population_)
        if(sameCustodyShared(model,selected.model_))return true;
    return false;
}
bool OriginalUiGarageSession50KG::retiredEnclosingInputs()const noexcept {
    // Observation only: cleanup of these predecessors belongs to their real
    // enclosing source stages. This capsule must not manufacture it.
    if(entryFrontend_||page_||podScene_||managerDat298_||managerArchiveOnlyScope170_||managerPageTable144_||
        !consumersClosed_||!backingRetired_||(visibility_&&!visibility_->closed_)||
        !scope_||!scope_->closed_||scope_->managerLive_||!scope_->garageChildren_.empty())return false;
    if(widgets_&&(!widgets_->detachedReleased_||!widgets_->tableReleased_))return false;
    for(const auto& resource:resources_->population_)
        if(resource&&!resource->closed_)return false;
    return true;
}
bool OriginalUiGarageSession50KG::normalRetirementFrameCurrent(const Application& root)const noexcept {
    using Draw=OriginalUiGarageDrawContribution50KG;
    using Native=Draw::NativeRetirementPhase;
    using Page=OriginalUiGaragePageOwner50KG;
    if(!page_||normalRetirementPhase_!=NormalRetirementPhase::notStarted||retiring_||
       managerLifecycleCallbackPending_||page_->updateFailed_||page_->drawPending_||
       page_->runtimeWidgetPending_||page_->sharedHookPending_||page_->timerCallbackPending_||
       page_->speechStopPending_||
       (page_->inputPhase_!=Page::InputPhase::idle&&page_->inputPhase_!=Page::InputPhase::returned))return false;
    if(!drawContribution_){
        //Completed manager entry may close before its first Root frame. An
        //absent Draw cannot erase an already-started update/audio/render prefix.
        return runtimeRootPhase_==RootRuntimePhase::notStarted&&runtimePhase_==RuntimePhase::notStarted&&
            runtimeTailPhase_==RootTailPhase::notStarted&&!runtimeFrame_&&!runtimeRootPublication_&&
            !runtimeTailPublication_&&!runtimeTargetPublished140A68_&&!runtimeRootReturnPC_&&
            !runtimeRoot10ReturnPC_&&!runtimeOuter26CReturnPC_&&!runtimeManagerReturnPC_&&
            !runtimeDispatcherReturnPC_&&!page_->updatePublication_&&!page_->updateReturned_&&
            !page_->drawPublication_&&!page_->completedUpdateFrames_&&!page_->lastCompletedUpdatePublication_;
    }
    const auto& draw=*drawContribution_;
    if(runtimeRootPhase_!=RootRuntimePhase::completed||runtimePhase_!=RuntimePhase::dispatcherReturned||
       runtimeTailPhase_!=RootTailPhase::timingCommitted||runtimeRootReturnPC_!=0x140D60||
       !runtimeRootPublication_||runtimeTailPublication_!=runtimeRootPublication_||
       !runtimeTargetPublished140A68_||!runtimeFrame_||root.sourceApplicationMode22C_!=0||
       !page_->updateReturned_||page_->updatePublication_!=runtimeRootPublication_||
       page_->lastCompletedUpdatePublication_!=runtimeRootPublication_||
       draw.session_.lock().get()!=this||draw.issuer_!=&root||draw.page_!=page_||
       draw.publication_!=runtimeRootPublication_||!draw.timing_.sameOwner(entryTiming_)||
       draw.phase_!=Draw::Phase::renderReturned140230||draw.sourcePC_!=0x140A68||
       !draw.targetCompleted140A38_||!draw.targetPublished140A68_||!draw.targetPublishAttempted_||
       !draw.finalPassReturned140844_||!draw.rendererEndReturned2442CC_||
       !draw.terminalQueue105990Current()||draw.activeText_||draw.activePageQuad_||
       draw.activePageCostText_)return false;
    if(draw.nativeRetirementPhase_==Native::notStarted)return draw.renderReturned140230Current(root);
    //A prior native release attempt may have released the renderer prelude
    //before GPU closure refused. Preserve its exact native retry cursor.
    return (draw.nativeRetirementPhase_==Native::admitted||
            draw.nativeRetirementPhase_==Native::preludeReleased)&&
        draw.nativeRetirementPublication_==runtimeRootPublication_&&draw.nativeRetirementOwnerCurrent();
}
bool OriginalUiGarageSession50KG::unsealedRenderReturnedFailureCurrent(const Application& root)const noexcept {
    using Draw=OriginalUiGarageDrawContribution50KG;
    if(!sameRoot(root)||failed_||actorSampleFailed10179C_||publishedObservationFailed1B8274_||retiring_||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||!page_||!runtimeFrame_||!drawContribution_||
       runtimeRootPhase_!=RootRuntimePhase::suffixUnavailable||runtimeTailPhase_!=RootTailPhase::notStarted||
       runtimePhase_!=RuntimePhase::dispatcherReturned||!runtimeRootPublication_||!runtimeRootSourcePC_||
       runtimeTailPublication_||runtimeTargetPublished140A68_||runtimeRootReturnPC_||
       runtimeRoot10ReturnPC_!=0x141F88||runtimeOuter26CReturnPC_!=0x141F88||
       runtimeManagerReturnPC_!=0x2A3FF0||runtimeDispatcherReturnPC_!=0x141F88||
       runtimeManagerChild_||runtimeRootChild_||runtimeOuterChild_||
       !page_->updateReturned_||page_->updateFailed_||page_->updateChild_||
       page_->updatePublication_!=runtimeRootPublication_||
       page_->lastCompletedUpdatePublication_!=runtimeRootPublication_||!page_->completedUpdateFrames_)return false;
    const auto& draw=*drawContribution_;
    // suffixUnavailable is written AFTER continue140230 returned false.
    // During the call Root is outerReturned. A later Root-tail failure has
    // Draw renderReturned, so cannot satisfy this exact returned-failure pair.
    return draw.session_.lock().get()==this&&draw.issuer_==&root&&draw.page_==page_&&
        draw.publication_==runtimeRootPublication_&&draw.timing_.sameOwner(entryTiming_)&&
        draw.attempted_&&draw.phase_==Draw::Phase::unavailable&&draw.sourcePC_==runtimeRootSourcePC_&&
        draw.nativeRetirementPhase_==Draw::NativeRetirementPhase::notStarted&&
        !draw.targetCompleted140A38_&&
        !draw.targetPublishAttempted_&&!draw.targetPublished140A68_&&draw.identityCurrent();
}
bool OriginalUiGarageSession50KG::hasUnsealedRenderFailure()const noexcept {
    return unsealedRenderRetirement_||
        (issuer_&&unsealedRenderReturnedFailureCurrent(*issuer_));
}
bool OriginalUiGarageSession50KG::unsealedRenderFailureHistoryCurrent(const Application& root)const noexcept {
    using Native=OriginalUiGarageDrawContribution50KG::NativeRetirementPhase;
    if(!unsealedRenderRetirement_||failed_||actorSampleFailed10179C_||publishedObservationFailed1B8274_||
       !sameRoot(root)||!page_||page_.get()!=unsealedRenderPage_||!drawContribution_||
       drawContribution_.get()!=unsealedRenderDraw_||!unsealedRenderGeneration_||!unsealedRenderEpoch_||
       !unsealedRenderPublication_||!unsealedRenderSourcePC_||!unsealedRenderFrame_||
       runtimeRootPhase_!=RootRuntimePhase::suffixUnavailable||runtimeRootSourcePC_!=unsealedRenderSourcePC_||
       runtimeRootPublication_!=unsealedRenderPublication_||runtimePhase_!=RuntimePhase::dispatcherReturned||
       runtimeTailPhase_!=RootTailPhase::notStarted||runtimeTailPublication_||runtimeTargetPublished140A68_||
       runtimeRootReturnPC_||runtimeRoot10ReturnPC_!=0x141F88||runtimeOuter26CReturnPC_!=0x141F88||
       runtimeManagerReturnPC_!=0x2A3FF0||runtimeDispatcherReturnPC_!=0x141F88||
       runtimeManagerChild_||runtimeRootChild_||runtimeOuterChild_||
       !page_->updateReturned_||page_->updateFailed_||page_->updateChild_||
       page_->updatePublication_!=unsealedRenderPublication_||
       page_->lastCompletedUpdatePublication_!=unsealedRenderPublication_||
       page_->completedUpdateFrames_!=unsealedRenderCompletedUpdates_||
       root.sourceApplicationMode22C_!=unsealedRenderMode_||!root.sourceApplicationElapsed23C_)return false;
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!entryTiming_.readPrepared(sample,serial)||serial!=unsealedRenderPublication_||
       sample.updateSeconds!=*root.sourceApplicationElapsed23C_)return false;
    const auto& draw=*drawContribution_;
    if(draw.publication_!=unsealedRenderPublication_||draw.sourcePC_!=unsealedRenderSourcePC_||
       draw.phase_!=OriginalUiGarageDrawContribution50KG::Phase::unavailable||!draw.attempted_)return false;
    if(draw.nativeRetirementPhase_==Native::released){
        if(draw.nativeRetirementPublication_!=unsealedRenderPublication_||draw.issuer_||draw.page_||draw.camera_||
           draw.renderer_||!draw.session_.expired()||draw.textureCoordinator_)return false;
    }else if(retiring_||normalRetirementPhase_!=NormalRetirementPhase::notStarted||
             draw.session_.lock().get()!=this||draw.issuer_!=&root||draw.page_!=page_)return false;
    if(unsealedRenderFrameReleased_)return unsealedRenderAudioRetired_&&
        normalRetirementPhase_==NormalRetirementPhase::currentCleared&&!runtimeFrame_&&
        draw.nativeRetirementPhase_==Native::released;
    return runtimeFrame_&&runtimeFrame_.get()==unsealedRenderFrame_;
}
bool OriginalUiGarageSession50KG::unsealedRenderFailureInitialIdentity(const Application& root)const noexcept {
    using Page=OriginalUiGaragePageOwner50KG;
    using Native=OriginalUiGarageDrawContribution50KG::NativeRetirementPhase;
    if(!unsealedRenderFailureHistoryCurrent(root)||retiring_||unsealedRenderAudioRetired_||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||!entryFrontend_||!scope_||!grant_||
       grant_->scope_.get()!=scope_.get()||scope_->issuer_!=&root||!sameCustodyShared(scope_->application_,authority_)||
       grant_->generation_!=unsealedRenderGeneration_||grant_->epoch_!=unsealedRenderEpoch_||
       scope_->managerEpoch_!=unsealedRenderEpoch_||entryManagerGeneration_!=unsealedRenderGeneration_||
       managerEntryPhase_!=ManagerEntryPhase::baseInputsReturned1CCC84||managerDrivePhase_!=ManagerDrivePhase::returned||
       managerLoadPhase_!=ManagerLoadPhase::published||!managerPublished1B8274_||
       !scope_->publishedPopulatedGarageManagerCurrent(grant_->generation_)||
       page_->issuer_!=&root||page_->grant_!=grant_||!page_->managerAttached1D1C40_||
       page_->loadReturnPhase_!=Page::LoadReturnPhase::returned||page_->activationPhase_!=Page::ActivationPhase::returned||
       page_->entryPhase_!=Page::EntryPhase::returned||!podScene_||page_->scene4C0_!=podScene_||
       page_->sceneBacking_.get()!=podScene_||page_->scenePageAttachCursor_!=10||page_->sceneLateAttachCursor_!=3||
       page_->cameraHandoffPhase_!=3||page_->entityRefReleased_||page_->entityCountReleased_||!page_->entityLive_||
       page_->cameraRetirementStep_||page_->sceneInitialRetiredCount_||page_->drawPending_||
       page_->timerCallbackPending_||page_->speechStopPending_||page_->runtimeWidgetPending_||
       page_->sharedHookPending_||managerLifecycleCallbackPending_||
       (page_->inputPhase_!=Page::InputPhase::idle&&page_->inputPhase_!=Page::InputPhase::returned)||
       managerPageCount148_!=1||!managerPageTable144_||managerPageTable144_[0]!=&page_->entity_||
       managerCurrentPage14C_!=&page_->entity_||managerSelected150_!=1||!manager150Known_||
       managerDat298_||managerArchiveOnlyScope170_||!managerBankLease_||!managerDeferred190Empty_)return false;
    const auto& draw=*drawContribution_;
    if(draw.activeText_||draw.activePageQuad_||draw.activePageCostText_)return false;
    // Readonly and nonrecursive: Main can authenticate its exact entry before
    // Widget/source preflights. Actual native release remains a later effect.
    return draw.nativeRetirementPhase_==Native::released||draw.unsealedNativeRendererQuiescent();
}
bool OriginalUiGarageSession50KG::unsealedRenderFailurePreflightCurrent(const Application& root)const noexcept {
    float dt=0;std::uint32_t pause=0;
    return unsealedRenderFailureInitialIdentity(root)&&entrySelected_&&entrySelected_->current()&&
        sameCustodyShared(application_,root.state)&&sameCustodyShared(motions_,root.motionLibrary4333C0())&&
        constructionInputsCurrent(root,grant_,resources_,sceneAssets_)&&pending_&&pending_->current()&&
        actors_&&runtimeFrame_&&actors_->currentRuntimeFrame(*runtimeFrame_,dt,pause)&&
        visibility_&&podScene_->currentPrefix()&&podScene_->visibility_==visibility_&&
        visibility_->matchesRetainedSceneOwnership(grant_,sceneAssets_,*page_,*podScene_)&&
        visibility_->nativeTerminalRenderQuiescent(*page_,unsealedRenderPublication_)&&
        page_->sourceEntryReturned1FAE00()&&page_->entryProfileCurrent()&&page_->entryProfile_&&
        page_->entryProfile_->mode==2&&sceneAssets_&&page_->entryProfile_->career100C0==sceneAssets_->selectedCareer()&&
        widgets_&&widgets_->pageEntryCustodyCurrent(*page_)&&widgets_->fog_&&widgets_->fog_->lights_&&
        widgets_->fog_->lights_->search_&&widgets_->fog_->lights_->search_->selected_;
}
bool OriginalUiGarageSession50KG::unsealedRenderFailureRetirementCurrent(const Application& root)const noexcept {
    using P=NormalRetirementPhase;
    if(!unsealedRenderFailureHistoryCurrent(root)||!retiring_||!entryFrontend_||normalFrontend_!=entryFrontend_||
       normalRetirementPhase_==P::notStarted||!scope_||!grant_||grant_->scope_.get()!=scope_.get()||
       scope_->issuer_!=&root||!sameCustodyShared(scope_->application_,authority_)||
       grant_->generation_!=unsealedRenderGeneration_||grant_->epoch_!=unsealedRenderEpoch_||
       normalManagerGeneration_!=unsealedRenderGeneration_||scope_->managerEpoch_!=unsealedRenderEpoch_||
       drawContribution_->nativeRetirementPhase_!=OriginalUiGarageDrawContribution50KG::NativeRetirementPhase::released)return false;
    if(normalRetirementPhase_<P::managerCountRetired)return scope_->managerLive_&&
        scope_->managerGeneration_==unsealedRenderGeneration_&&scope_->dispatcherCurrentManager30_==&scope_->manager_&&
        scope_->dispatcherCurrentManagerGeneration30_==unsealedRenderGeneration_;
    if(scope_->managerLive_||scope_->managerGeneration_||scope_->manager_.reference00||
       scope_->completedManagerPage1413E0_!=page_.get()||
       scope_->completedManagerPageGeneration_!=unsealedRenderGeneration_||
       scope_->completedManagerPageEpoch_!=unsealedRenderEpoch_)return false;
    if(normalRetirementPhase_==P::managerCountRetired)return scope_->dispatcherCurrentManager30_==&scope_->manager_&&
        scope_->dispatcherCurrentManagerGeneration30_==unsealedRenderGeneration_;
    if(normalRetirementPhase_!=P::currentCleared)return false;
    D d;std::string error;return normalSourceOwnersRetired(d,error);
}
bool OriginalUiGarageSession50KG::nativeAudioUnsealedRenderFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,bool managerRetired)const noexcept {
    if(!audio_||audio_.get()!=&main||!generation||!epoch||generation!=unsealedRenderGeneration_||
       epoch!=unsealedRenderEpoch_||unsealedRenderAudioRetired_)return false;
    if(managerRetired)return normalRetirementPhase_==NormalRetirementPhase::currentCleared&&
        unsealedRenderFailureRetirementCurrent(root);
    if(normalRetirementPhase_==NormalRetirementPhase::notStarted)return unsealedRenderFailureInitialIdentity(root);
    return normalRetirementPhase_<NormalRetirementPhase::currentCleared&&unsealedRenderFailureRetirementCurrent(root);
}
bool OriginalUiGarageSession50KG::prepareUnsealedRenderFailureRetirement(const Application& root,
    D& d,std::string& error)noexcept {
    d={};
    if(!unsealedRenderRetirement_){
        if(!unsealedRenderReturnedFailureCurrent(root)||!scope_||!grant_||
           !entryManagerGeneration_||grant_->generation_!=entryManagerGeneration_||grant_->epoch_!=scope_->managerEpoch_)
            return custodyFailure(error,"Native render cleanup requires its actual returned failed unsealed Draw");
        // Capture native cleanup identity only. The existing post-return Root
        // state is the failure producer; no source completion/phase is written.
        unsealedRenderGeneration_=grant_->generation_;unsealedRenderEpoch_=grant_->epoch_;
        unsealedRenderPublication_=runtimeRootPublication_;unsealedRenderSourcePC_=runtimeRootSourcePC_;
        unsealedRenderCompletedUpdates_=page_->completedUpdateFrames_;unsealedRenderMode_=root.sourceApplicationMode22C_;
        unsealedRenderPage_=page_.get();unsealedRenderDraw_=drawContribution_.get();unsealedRenderFrame_=runtimeFrame_.get();
        unsealedRenderRetirement_=true;
    }
    if(!unsealedRenderFailurePreflightCurrent(root)){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=unsealedRenderSourcePC_;
        return custodyFailure(error,"Native render cleanup retains pending source/native render ownership");
    }
    const auto owner=root.garageCustodyOwner_;
    if(!owner||owner.get()!=this||!audio_||!audio_->preflightUnsealedRenderEntryFailure(owner,error)){
        d.status=assets::OriginalStandardMaterialStatus50LA::source;d.sourcePC=0x235150;return false;
    }
    // Actual owner/prelude/FIFO/GPU release precedes every manager effect.
    // Its admitted/preludeReleased cursor retains a failed native release.
    if(!drawContribution_->releaseUnsealedBeforeRetirement()){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=unsealedRenderSourcePC_;
        return custodyFailure(error,"Native unsealed Draw release is pending before manager retirement");
    }
    normalSelected_=widgets_->fog_->lights_->search_->selected_;
    normalManagerGeneration_=grant_->generation_;return true;
}
void OriginalUiGarageSession50KG::retainActorSampleFailure10179C(const Application& root)noexcept {
    using Page=OriginalUiGaragePageOwner50KG;
    // Called only at the actual Root return from the failed dispatcher. The
    // Actor receipt itself was minted after its synchronous callback unwound.
    if(actorSampleFailed10179C_||publishedObservationFailed1B8274_||failed_||retiring_||
       !sameRoot(root)||!scope_||!grant_||!page_||!actors_||!runtimeFrame_||drawContribution_||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||
       runtimeRootPhase_!=RootRuntimePhase::unavailable||runtimeRootSourcePC_!=0x10179C||
       runtimePhase_!=RuntimePhase::unavailable||runtimeSourcePC_!=0x10179C||
       runtimeTailPhase_!=RootTailPhase::notStarted||!runtimeRootPublication_||runtimeTailPublication_||
       runtimeRootReturnPC_||runtimeRoot10ReturnPC_||runtimeOuter26CReturnPC_||
       runtimeManagerReturnPC_||runtimeDispatcherReturnPC_||runtimeTargetPublished140A68_||
       runtimeManagerChild_!=&page_->entity_||!page_->updateFailed_||page_->updateSourcePC_!=0x10179C||
       page_->updatePublication_!=runtimeRootPublication_||page_->updateReturned_||
       page_->completedUpdateFrames_||page_->lastCompletedUpdatePublication_||page_->updatedPageChildren_!=8||
       page_->updateChild_!=page_->scenePageChildren_[7]||!page_->updateChild_||
       !actors_->failedSampleFrameCurrent(*runtimeFrame_,runtimeRootPublication_)||
       !scope_->publishedPopulatedGarageManagerCurrent(grant_->generation_)||
       page_->loadReturnPhase_!=Page::LoadReturnPhase::returned||
       page_->activationPhase_!=Page::ActivationPhase::returned||page_->entryPhase_!=Page::EntryPhase::returned||
       page_->scenePageAttachCursor_!=10||page_->sceneLateAttachCursor_!=3||page_->cameraHandoffPhase_!=3||
       page_->drawPublication_||page_->drawPending_||page_->timerCallbackPending_||page_->speechStopPending_||
       page_->runtimeWidgetPending_||page_->sharedHookPending_||managerLifecycleCallbackPending_||
       !entryManagerGeneration_||grant_->generation_!=entryManagerGeneration_||grant_->epoch_!=scope_->managerEpoch_)
        return;
    actorSampleGeneration_=grant_->generation_;actorSampleEpoch_=grant_->epoch_;
    actorSamplePublication_=runtimeRootPublication_;actorSamplePage_=page_.get();
    actorSampleManager_=page_->updateChild_;actorSampleFailed10179C_=true;
}
bool OriginalUiGarageSession50KG::hasActorSampleFailure()const noexcept {
    return actorSampleFailed10179C_&&!publishedObservationFailed1B8274_&&!failed_&&
        actorSampleGeneration_&&actorSampleEpoch_&&actorSamplePublication_&&actorSamplePage_&&actorSampleManager_;
}
bool OriginalUiGarageSession50KG::actorSampleFailureHistoryCurrent(const Application& root)const noexcept {
    if(!hasActorSampleFailure()||!sameRoot(root)||!page_||page_.get()!=actorSamplePage_||!actors_||
       runtimeRootPhase_!=RootRuntimePhase::unavailable||runtimeRootSourcePC_!=0x10179C||
       runtimePhase_!=RuntimePhase::unavailable||runtimeSourcePC_!=0x10179C||
       runtimeRootPublication_!=actorSamplePublication_||runtimeTailPhase_!=RootTailPhase::notStarted||
       runtimeTailPublication_||runtimeTargetPublished140A68_||drawContribution_||
       runtimeRootReturnPC_||runtimeRoot10ReturnPC_||runtimeOuter26CReturnPC_||
       runtimeManagerReturnPC_||runtimeDispatcherReturnPC_||runtimeManagerChild_!=&page_->entity_||
       !page_->updateFailed_||page_->updateSourcePC_!=0x10179C||page_->updatePublication_!=actorSamplePublication_||
       page_->updateReturned_||page_->completedUpdateFrames_||page_->lastCompletedUpdatePublication_||
       page_->updatedPageChildren_!=8||page_->updateChild_!=actorSampleManager_||
       page_->drawPublication_||root.sourceApplicationMode22C_!=2||!root.sourceApplicationElapsed23C_)
        return false;
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!entryTiming_.readPrepared(sample,serial)||serial!=actorSamplePublication_||
       sample.updateSeconds!=*root.sourceApplicationElapsed23C_)return false;
    if(actorSampleFrameReleased_)return actorSampleAudioRetired_&&
        normalRetirementPhase_==NormalRetirementPhase::currentCleared&&!runtimeFrame_&&
        actors_->failedSampleFrameReleasedCurrent(actorSamplePublication_);
    return runtimeFrame_&&actors_->failedSampleFrameCurrent(*runtimeFrame_,actorSamplePublication_);
}
bool OriginalUiGarageSession50KG::actorSampleFailureInitialIdentity(const Application& root)const noexcept {
    using Page=OriginalUiGaragePageOwner50KG;
    // No Widget/Main currentness call here: Main consumes this readonly
    // identity before those full owner preflights, without a recursion cycle.
    return actorSampleFailureHistoryCurrent(root)&&!retiring_&&!actorSampleAudioRetired_&&
        normalRetirementPhase_==NormalRetirementPhase::notStarted&&entryFrontend_&&scope_&&grant_&&
        grant_->scope_.get()==scope_.get()&&scope_->issuer_==&root&&sameCustodyShared(scope_->application_,authority_)&&
        grant_->generation_==actorSampleGeneration_&&grant_->epoch_==actorSampleEpoch_&&
        scope_->managerEpoch_==actorSampleEpoch_&&entryManagerGeneration_==actorSampleGeneration_&&
        managerEntryPhase_==ManagerEntryPhase::baseInputsReturned1CCC84&&
        managerDrivePhase_==ManagerDrivePhase::returned&&managerLoadPhase_==ManagerLoadPhase::published&&
        managerPublished1B8274_&&scope_->publishedPopulatedGarageManagerCurrent(grant_->generation_)&&
        page_->issuer_==&root&&page_->grant_==grant_&&page_->managerAttached1D1C40_&&
        page_->loadReturnPhase_==Page::LoadReturnPhase::returned&&
        page_->activationPhase_==Page::ActivationPhase::returned&&page_->entryPhase_==Page::EntryPhase::returned&&
        page_->scene4C0_==podScene_&&podScene_&&page_->sceneBacking_.get()==podScene_&&
        page_->scenePageAttachCursor_==10&&page_->sceneLateAttachCursor_==3&&page_->cameraHandoffPhase_==3&&
        !page_->entityRefReleased_&&!page_->entityCountReleased_&&page_->entityLive_&&
        !page_->cameraRetirementStep_&&!page_->sceneInitialRetiredCount_&&
        !page_->timerCallbackPending_&&!page_->speechStopPending_&&!page_->runtimeWidgetPending_&&
        !page_->sharedHookPending_&&!managerLifecycleCallbackPending_&&
        (page_->inputPhase_==Page::InputPhase::idle||page_->inputPhase_==Page::InputPhase::returned)&&
        managerPageCount148_==1&&managerPageTable144_&&managerPageTable144_[0]==&page_->entity_&&
        managerCurrentPage14C_==&page_->entity_&&managerSelected150_==1&&manager150Known_&&
        !managerDat298_&&!managerArchiveOnlyScope170_&&managerBankLease_&&managerDeferred190Empty_;
}
bool OriginalUiGarageSession50KG::actorSampleFailurePreflightCurrent(const Application& root)const noexcept {
    return actorSampleFailureInitialIdentity(root)&&entrySelected_&&entrySelected_->current()&&
        sameCustodyShared(application_,root.state)&&sameCustodyShared(motions_,root.motionLibrary4333C0())&&
        constructionInputsCurrent(root,grant_,resources_,sceneAssets_)&&pending_&&pending_->current()&&
        visibility_&&podScene_->currentPrefix()&&podScene_->visibility_==visibility_&&
        visibility_->matchesRetainedSceneOwnership(grant_,sceneAssets_,*page_,*podScene_)&&
        page_->sourceEntryReturned1FAE00()&&page_->entryProfileCurrent()&&page_->entryProfile_&&
        page_->entryProfile_->mode==2&&sceneAssets_&&page_->entryProfile_->career100C0==sceneAssets_->selectedCareer()&&
        widgets_&&widgets_->pageEntryCustodyCurrent(*page_)&&widgets_->fog_&&widgets_->fog_->lights_&&
        widgets_->fog_->lights_->search_&&widgets_->fog_->lights_->search_->selected_;
}
bool OriginalUiGarageSession50KG::actorSampleFailureRetirementCurrent(const Application& root)const noexcept {
    using P=NormalRetirementPhase;
    if(!actorSampleFailureHistoryCurrent(root)||!retiring_||!entryFrontend_||normalFrontend_!=entryFrontend_||
       normalRetirementPhase_==P::notStarted||!scope_||!grant_||grant_->scope_.get()!=scope_.get()||
       scope_->issuer_!=&root||!sameCustodyShared(scope_->application_,authority_)||
       grant_->generation_!=actorSampleGeneration_||grant_->epoch_!=actorSampleEpoch_||
       normalManagerGeneration_!=actorSampleGeneration_||scope_->managerEpoch_!=actorSampleEpoch_)return false;
    if(normalRetirementPhase_<P::managerCountRetired)
        return scope_->managerLive_&&scope_->managerGeneration_==actorSampleGeneration_&&
            scope_->dispatcherCurrentManager30_==&scope_->manager_&&
            scope_->dispatcherCurrentManagerGeneration30_==actorSampleGeneration_;
    if(scope_->managerLive_||scope_->managerGeneration_||scope_->manager_.reference00||
       scope_->completedManagerPage1413E0_!=page_.get()||
       scope_->completedManagerPageGeneration_!=actorSampleGeneration_||
       scope_->completedManagerPageEpoch_!=actorSampleEpoch_)return false;
    if(normalRetirementPhase_==P::managerCountRetired)return scope_->dispatcherCurrentManager30_==&scope_->manager_&&
        scope_->dispatcherCurrentManagerGeneration30_==actorSampleGeneration_;
    if(normalRetirementPhase_!=P::currentCleared)return false;
    D d;std::string error;return normalSourceOwnersRetired(d,error);
}
bool OriginalUiGarageSession50KG::nativeAudioActorSampleFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,bool managerRetired)const noexcept {
    if(!audio_||audio_.get()!=&main||!generation||!epoch||generation!=actorSampleGeneration_||
       epoch!=actorSampleEpoch_||actorSampleAudioRetired_)return false;
    if(managerRetired)return normalRetirementPhase_==NormalRetirementPhase::currentCleared&&
        actorSampleFailureRetirementCurrent(root);
    if(normalRetirementPhase_==NormalRetirementPhase::notStarted)return actorSampleFailureInitialIdentity(root);
    return normalRetirementPhase_<NormalRetirementPhase::currentCleared&&actorSampleFailureRetirementCurrent(root);
}
bool OriginalUiGarageSession50KG::prepareActorSampleFailureRetirement(const Application& root,
    D& d,std::string& error)noexcept {
    d={};
    if(!actorSampleFailurePreflightCurrent(root)){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x10179C;
        return custodyFailure(error,"Actor sample failure requires its exact first partial-frame source custody");
    }
    const auto owner=root.garageCustodyOwner_;
    if(!owner||owner.get()!=this||!audio_||!audio_->preflightActorSampleEntryFailure(owner,error)){
        d.status=assets::OriginalStandardMaterialStatus50LA::source;d.sourcePC=0x235150;return false;
    }
    normalSelected_=widgets_->fog_->lights_->search_->selected_;
    normalManagerGeneration_=grant_->generation_;return true;
}
void OriginalUiGarageSession50KG::retainPublishedObservationFailure1B8274(const Application& root)noexcept {
    using Page=OriginalUiGaragePageOwner50KG;
    if(publishedObservationFailed1B8274_||failed_||retiring_||!sameRoot(root)||!entryFrontend_||
       managerDrivePhase_!=ManagerDrivePhase::returned||managerLoadPhase_!=ManagerLoadPhase::published||
       !managerPublished1B8274_||!scope_||!grant_||!page_||!podScene_||
       !scope_->publishedPopulatedGarageManagerCurrent(grant_->generation_)||
       grant_->scope_.get()!=scope_.get()||grant_->generation_!=entryManagerGeneration_||
       grant_->epoch_!=scope_->managerEpoch_||!entryManagerGeneration_||!grant_->epoch_||
       page_->issuer_!=&root||page_->grant_!=grant_||!page_->managerAttached1D1C40_||
       page_->loadReturnPhase_!=Page::LoadReturnPhase::returned||
       page_->activationPhase_!=Page::ActivationPhase::returned||page_->entryPhase_!=Page::EntryPhase::returned||
       page_->entryAppearanceStep_!=18||page_->entryStatCount_!=6||page_->entryExplicitWidget_!=6||
       page_->entryCameraStep_!=5||page_->scene4C0_!=podScene_||page_->sceneBacking_.get()!=podScene_||
       page_->scenePageAttachCursor_!=10||page_->sceneLateAttachCursor_!=3||
       managerPageCount148_!=1||!managerPageTable144_||managerPageTable144_[0]!=&page_->entity_||
       managerCurrentPage14C_!=&page_->entity_||managerSelected150_!=1||!manager150Known_||
       managerDat298_||managerArchiveOnlyScope170_||!managerBankLease_||
       !normalRetirementFrameCurrent(root)||runtimeFrame_||drawContribution_)return;
    publishedObservationGeneration_=grant_->generation_;publishedObservationEpoch_=grant_->epoch_;
    publishedObservationPage_=page_.get();publishedObservationFailed1B8274_=true;
}
bool OriginalUiGarageSession50KG::hasPublishedObservationFailure()const noexcept {
    return failed_&&publishedObservationFailed1B8274_&&publishedObservationGeneration_&&
        publishedObservationEpoch_&&publishedObservationPage_&&
        entryFailureStatus_==assets::OriginalStandardMaterialStatus50LA::source&&entryFailureSourcePC_==0x1B8274;
}
bool OriginalUiGarageSession50KG::publishedObservationFailureInitialIdentity(const Application& root)const noexcept {
    using Page=OriginalUiGaragePageOwner50KG;
    // Nonrecursive identity for Main's read-only failure preflight. Do not
    // enter Widget/Watto/audio currentness or recertify the failed observer.
    return hasPublishedObservationFailure()&&sameRoot(root)&&!retiring_&&!publishedObservationAudioRetired_&&
        normalRetirementPhase_==NormalRetirementPhase::notStarted&&entryFrontend_&&page_&&
        page_.get()==publishedObservationPage_&&scope_&&grant_&&grant_->scope_.get()==scope_.get()&&
        scope_->issuer_==&root&&sameCustodyShared(scope_->application_,authority_)&&
        grant_->generation_==publishedObservationGeneration_&&grant_->epoch_==publishedObservationEpoch_&&
        scope_->managerEpoch_==publishedObservationEpoch_&&entryManagerGeneration_==publishedObservationGeneration_&&
        managerEntryPhase_==ManagerEntryPhase::baseInputsReturned1CCC84&&
        managerDrivePhase_==ManagerDrivePhase::returned&&managerLoadPhase_==ManagerLoadPhase::published&&
        managerPublished1B8274_&&scope_->publishedPopulatedGarageManagerCurrent(grant_->generation_)&&
        page_->issuer_==&root&&page_->grant_==grant_&&page_->managerAttached1D1C40_&&
        page_->loadReturnPhase_==Page::LoadReturnPhase::returned&&
        page_->activationPhase_==Page::ActivationPhase::returned&&page_->entryPhase_==Page::EntryPhase::returned&&
        page_->entryAppearanceStep_==18&&page_->entryStatCount_==6&&page_->entryExplicitWidget_==6&&
        page_->entryCameraStep_==5&&page_->scene4C0_==podScene_&&podScene_&&page_->sceneBacking_.get()==podScene_&&
        page_->scenePageAttachCursor_==10&&page_->sceneLateAttachCursor_==3&&
        managerPageCount148_==1&&managerPageTable144_&&managerPageTable144_[0]==&page_->entity_&&
        managerCurrentPage14C_==&page_->entity_&&managerSelected150_==1&&manager150Known_&&
        !managerDat298_&&!managerArchiveOnlyScope170_&&managerBankLease_&&
        !runtimeFrame_&&!drawContribution_&&normalRetirementFrameCurrent(root);
}
bool OriginalUiGarageSession50KG::publishedObservationFailurePreflightCurrent(const Application& root)const noexcept {
    if(!publishedObservationFailureInitialIdentity(root)||!sameRoot(root)||retiring_||publishedObservationAudioRetired_||
       normalRetirementPhase_!=NormalRetirementPhase::notStarted||!entryFrontend_||
       !page_||page_.get()!=publishedObservationPage_||!scope_||!grant_||
       grant_->scope_.get()!=scope_.get()||scope_->issuer_!=&root||
       grant_->generation_!=publishedObservationGeneration_||grant_->epoch_!=publishedObservationEpoch_||
       scope_->managerEpoch_!=publishedObservationEpoch_||entryManagerGeneration_!=publishedObservationGeneration_||
       managerEntryPhase_!=ManagerEntryPhase::baseInputsReturned1CCC84||
       managerDrivePhase_!=ManagerDrivePhase::returned||managerLoadPhase_!=ManagerLoadPhase::published||
       !managerPublished1B8274_||!scope_->publishedPopulatedGarageManagerCurrent(grant_->generation_)||
       !entrySelected_||!entrySelected_->current()||!sameCustodyShared(application_,root.state)||
       !sameCustodyShared(motions_,root.motionLibrary4333C0())||
       !constructionInputsCurrent(root,grant_,resources_,sceneAssets_)||!pending_||!pending_->current()||
       !podScene_||!visibility_||page_->borrowScene4C0()!=podScene_||!podScene_->currentPrefix()||
       podScene_->visibility_!=visibility_||!visibility_->matchesRetainedSceneOwnership(grant_,sceneAssets_,*page_,*podScene_)||
       !page_->sourceEntryReturned1FAE00()||!page_->entryProfileCurrent()||!page_->entryProfile_||
       page_->entryProfile_->mode!=2||!sceneAssets_||page_->entryProfile_->career100C0!=sceneAssets_->selectedCareer()||
       !widgets_||!widgets_->pageEntryCustodyCurrent(*page_)||!widgets_->fog_||!widgets_->fog_->lights_||
       !widgets_->fog_->lights_->search_||!widgets_->fog_->lights_->search_->selected_||!managerBankLease_||
       managerPageCount148_!=1||!managerPageTable144_||managerPageTable144_[0]!=&page_->entity_||
       managerCurrentPage14C_!=&page_->entity_||managerSelected150_!=1||!managerDeferred190Empty_||
       managerDat298_||managerArchiveOnlyScope170_||runtimeFrame_||drawContribution_||
       !normalRetirementFrameCurrent(root))return false;
    return true;
}
bool OriginalUiGarageSession50KG::publishedObservationFailureRetirementCurrent(const Application& root)const noexcept {
    using P=NormalRetirementPhase;
    if(!hasPublishedObservationFailure()||!sameRoot(root)||!retiring_||!entryFrontend_||normalFrontend_!=entryFrontend_||
       normalRetirementPhase_==P::notStarted||!page_||page_.get()!=publishedObservationPage_||
       !scope_||!grant_||grant_->scope_.get()!=scope_.get()||scope_->issuer_!=&root||
       !sameCustodyShared(scope_->application_,authority_)||
       grant_->generation_!=publishedObservationGeneration_||grant_->epoch_!=publishedObservationEpoch_||
       normalManagerGeneration_!=publishedObservationGeneration_||scope_->managerEpoch_!=publishedObservationEpoch_||
       runtimeRootPhase_!=RootRuntimePhase::notStarted||runtimePhase_!=RuntimePhase::notStarted||
       runtimeTailPhase_!=RootTailPhase::notStarted||runtimeRootPublication_||runtimeTailPublication_||
       runtimeTargetPublished140A68_||runtimeFrame_||drawContribution_)return false;
    if(normalRetirementPhase_<P::managerCountRetired)
        return scope_->managerLive_&&scope_->managerGeneration_==publishedObservationGeneration_&&
            scope_->dispatcherCurrentManager30_==&scope_->manager_&&
            scope_->dispatcherCurrentManagerGeneration30_==publishedObservationGeneration_;
    if(scope_->managerLive_||scope_->managerGeneration_||scope_->manager_.reference00||
       scope_->completedManagerPage1413E0_!=page_.get()||
       scope_->completedManagerPageGeneration_!=publishedObservationGeneration_||
       scope_->completedManagerPageEpoch_!=publishedObservationEpoch_)return false;
    if(normalRetirementPhase_==P::managerCountRetired)
        return scope_->dispatcherCurrentManager30_==&scope_->manager_&&
            scope_->dispatcherCurrentManagerGeneration30_==publishedObservationGeneration_;
    if(normalRetirementPhase_!=P::currentCleared)return false;
    D d;std::string error;return normalSourceOwnersRetired(d,error);
}
bool OriginalUiGarageSession50KG::nativeAudioPublishedObservationFailureIdentity(const Application& root,
    const OriginalUiMainAudio50KG& main,std::uint64_t generation,std::uint64_t epoch,bool managerRetired)const noexcept {
    if(!audio_||audio_.get()!=&main||!generation||!epoch||generation!=publishedObservationGeneration_||
       epoch!=publishedObservationEpoch_||publishedObservationAudioRetired_)return false;
    if(managerRetired)return normalRetirementPhase_==NormalRetirementPhase::currentCleared&&
        publishedObservationFailureRetirementCurrent(root);
    if(normalRetirementPhase_==NormalRetirementPhase::notStarted)
        return publishedObservationFailureInitialIdentity(root);
    return normalRetirementPhase_<NormalRetirementPhase::currentCleared&&
        publishedObservationFailureRetirementCurrent(root);
}
bool OriginalUiGarageSession50KG::preparePublishedObservationFailureRetirement(const Application& root,
    D& d,std::string& error)noexcept {
    d={};
    if(!publishedObservationFailurePreflightCurrent(root)){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x1B8274;
        return custodyFailure(error,"Published Garage observation failure requires its exact complete, never-framed source owners");
    }
    // Resolved Sounds may own genuine immediate-play voices. Only Main's
    // exact readonly source-custody preflight may authorize their later real
    // destructor stops. Never cancel unresolved rows or manufacture silence.
    const auto owner=root.garageCustodyOwner_;
    if(!owner||owner.get()!=this||!audio_||!audio_->preflightPublishedObservationEntryFailure(owner,error)){
        d.status=assets::OriginalStandardMaterialStatus50LA::source;d.sourcePC=0x235150;return false;
    }
    normalSelected_=widgets_->fog_->lights_->search_->selected_;
    normalManagerGeneration_=grant_->generation_;return true;
}
bool OriginalUiGarageSession50KG::prepareNormalRetirement(const Application& root,
    D& d,std::string& error)noexcept {
    d={};
    if(normalRetirementPhase_!=NormalRetirementPhase::notStarted||!publishedOwnerCurrent(root)||
       !managerPublished1B8274_||managerLoadPhase_!=ManagerLoadPhase::published||
       !page_||!podScene_||!page_->sourceEntryReturned1FAE00()||
       !page_->entryProfileCurrent()||!page_->entryProfile_||
       page_->entryProfile_->mode!=2||!sceneAssets_||page_->entryProfile_->career100C0!=sceneAssets_->selectedCareer()||
       !widgets_||!widgets_->pageEntryCustodyCurrent(*page_)||!widgets_->fog_||
       !widgets_->fog_->lights_||!widgets_->fog_->lights_->search_||
       !widgets_->fog_->lights_->search_->selected_||!managerBankLease_||
       !scope_->publishedPopulatedGarageManagerCurrent(grant_->generation_)||
       managerPageCount148_!=1||!managerPageTable144_||
       managerPageTable144_[0]!=&page_->entity_||managerCurrentPage14C_!=&page_->entity_||
       managerSelected150_!=1||!managerDeferred190Empty_||managerDat298_||managerArchiveOnlyScope170_)
        return custodyFailure(error,"Normal Garage Back requires the exact published unchanged Page and live owner chain");
    if(!normalRetirementFrameCurrent(root))
        return custodyFailure(error,"Normal Garage retirement requires no started frame or the exact published, timing-committed, retired-FIFO target");
    if(drawContribution_&&!drawContribution_->releaseUnsealedBeforeRetirement())
        return custodyFailure(error,"Garage frame ownership must discharge its real renderer lifetime before manager retirement");
    // SearchNode retirement drops its selected shared borrow. Preserve the
    // exact predecessor now; never reconstruct it from a resource ordinal.
    normalSelected_=widgets_->fog_->lights_->search_->selected_;
    normalManagerGeneration_=grant_->generation_;
    return true;
}
bool OriginalUiGarageSession50KG::retireNormalChildren142590(D& d,std::string& error)noexcept {
    using P=NormalRetirementPhase;d={};
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        d.status=assets::OriginalStandardMaterialStatus50LA::source;d.sourcePC=pc;
        return custodyFailure(error,text);
    };
    if(!widgets_||!widgets_->fog_||!widgets_->fog_->lights_||
       !widgets_->fog_->lights_->search_||!normalSelected_)
        return fail(0x142590,"Normal Garage retirement lost its exact retained owner chain");
    auto& fog=*widgets_->fog_;auto& lights=*fog.lights_;auto& search=*lights.search_;
    if(normalRetirementPhase_==P::managerDetached){
        OriginalUiGaragePageOwner50KG::Diagnostic pd;
        if(!page_||!page_->beginRetirement142590(pd)){
            std::fprintf(stderr,"garage_page_retirement_pending page=%d status=%u pc=%x stage=%u\n",int(page_!=nullptr),
                unsigned(pd.status),unsigned(pd.sourcePC),page_?unsigned(page_->stage_):0u);
            return fail(pd.sourcePC?pd.sourcePC:0x142590,"Normal Garage Page/Scene retirement is pending; exact forest retained");}
        // Scene4C0 was freed by its Page owner. Clear only our native borrow.
        podScene_=nullptr;normalRetirementPhase_=P::pageRetired;
    }
    if(normalRetirementPhase_==P::pageRetired){
        OriginalUiGarageManagerFog50KG::Diagnostic fd;
        if(!fog.retireAttached1CD080(fd))return fail(fd.sourcePC,"Normal Garage Fog tail retirement is pending");
        normalRetirementPhase_=P::fogRetired;
    }
    if(normalRetirementPhase_==P::fogRetired){
        OriginalUiGarageWidget1Diagnostic50KG wd;
        if(!widgets_->retireManagerRows2A8F70(wd))return fail(wd.sourcePC,"Normal Garage Widget22..1 retirement is pending");
        normalRetirementPhase_=P::widgetsRetired;
    }
    if(normalRetirementPhase_==P::widgetsRetired){
        // Every Fog entity already returned before Widgets. This existing
        // owner now drops ONLY its native aliases and records their release;
        // none of its detached-entity callbacks can be reached on this route.
        for(std::size_t i=0;i<3;++i)if(!fog.attached_[i]||!fog.retired_[i])
            return fail(0x13E700,"Normal Garage Fog aliases require all actual attached callbacks");
        OriginalUiGarageManagerFog50KG::Diagnostic fd;
        if(!fog.retireDetached1CCFA0(fd))return fail(fd.sourcePC,"Normal Garage Fog aliases remain retained");
        normalRetirementPhase_=P::fogAliasesReleased;
    }
    if(normalRetirementPhase_==P::fogAliasesReleased){
        OriginalUiGarageUiLights50KG::Diagnostic ld;
        if(!lights.retireLight146980(1,ld))return fail(ld.sourcePC,"Normal Garage directional Light retirement is pending");
        normalRetirementPhase_=P::directionalRetired;
    }
    if(normalRetirementPhase_==P::directionalRetired){
        OriginalUiGarageUiLights50KG::Diagnostic ld;
        if(!lights.retireLight146980(0,ld))return fail(ld.sourcePC,"Normal Garage ambient Light retirement is pending");
        normalRetirementPhase_=P::ambientRetired;
    }
    if(normalRetirementPhase_==P::ambientRetired){
        OriginalUiGarageManagerSearch50KG::Diagnostic sd;
        if(!search.retireNode27F180(sd))return fail(sd.sourcePC,"Normal Garage SearchNode retirement is pending");
        normalRetirementPhase_=P::searchNodeRetired;
    }
    if(normalRetirementPhase_==P::searchNodeRetired){
        if(!normalSelected_->closeManager142590(d))
            return custodyFailure(error,"Normal Garage selected-model retirement is pending");
        normalRetirementPhase_=P::selectedRetired;
    }
    if(normalRetirementPhase_==P::selectedRetired){
        const auto step=grant_->beginGarageManagerPayloadRetirement(&search);
        if(!step.okay())return fail(step.sourcePC,"Normal Garage manager ref0 dispatch refused");
        normalRetirementPhase_=P::managerRefReleased;
    }
    if(normalRetirementPhase_!=P::managerRefReleased)return fail(0x1D18FC,"Normal Garage child retirement is out of order");
    error.clear();return true;
}
bool OriginalUiGarageSession50KG::retireNormalManagerPayload1CCADC(D& d,std::string& error)noexcept {
    using P=NormalRetirementPhase;d={};
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        d.status=assets::OriginalStandardMaterialStatus50LA::source;d.sourcePC=pc;
        return custodyFailure(error,text);
    };
    if(!widgets_||!widgets_->fog_||!widgets_->fog_->lights_||!widgets_->fog_->lights_->search_)
        return fail(0x1CCADC,"Normal Garage manager payload owner chain changed");
    auto& lights=*widgets_->fog_->lights_;auto& search=*lights.search_;
    if(normalRetirementPhase_==P::transfersRetired){
        OriginalUiGarageManagerSearch50KG::Diagnostic sd;
        if(!search.retireManagerList1CCADC(sd))return fail(sd.sourcePC,"Normal Garage SearchList payload retirement is pending");
        normalRetirementPhase_=P::searchPayloadRetired;
    }
    if(normalRetirementPhase_==P::searchPayloadRetired){
        // Genuine constructor null190; positive OnTop producers were refused.
        if(!managerDeferred190Empty_)return fail(0x1CCAF0,"Normal Garage deferred190 is not genuinely empty");
        normalRetirementPhase_=P::deferredReturned;
    }
    if(normalRetirementPhase_==P::deferredReturned){
        OriginalUiGarageUiLights50KG::Diagnostic ld;
        if(!lights.retireManagerDescriptors1CCB20(ld))return fail(ld.sourcePC,"Normal Garage shared Light zones/descriptors retirement is pending");
        normalRetirementPhase_=P::lightPayloadRetired;
    }
    if(normalRetirementPhase_!=P::lightPayloadRetired)return fail(0x12E5E0,"Normal Garage manager payload retirement is out of order");
    error.clear();return true;
}
bool OriginalUiGarageSession50KG::finishNormalManager2A3B04(D& d,std::string& error)noexcept {
    using P=NormalRetirementPhase;d={};
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        d.status=assets::OriginalStandardMaterialStatus50LA::source;d.sourcePC=pc;
        return custodyFailure(error,text);
    };
    if(normalRetirementPhase_==P::bankRetired){
        if(!releaseManagerPageTable2A3B04(d))return custodyFailure(error,"Normal Garage Page table release refused");
        normalRetirementPhase_=P::pageTableRetired;
    }
    if(normalRetirementPhase_==P::pageTableRetired){
        OriginalUiGarageWidget1Diagnostic50KG wd;
        if(!widgets_->retireManagerTable2A3B18(wd))return fail(wd.sourcePC,"Normal Garage Widget table release refused");
        normalRetirementPhase_=P::widgetTableRetired;
    }
    if(normalRetirementPhase_==P::widgetTableRetired){
        const auto step=scope_->finishGarageManagerPayloadRetirement(normalManagerGeneration_,widgets_->fog_->lights_->search_.get());
        if(!step.okay())return fail(step.sourcePC,"Normal Garage manager count release refused");
        normalRetirementPhase_=P::managerCountRetired;
    }
    if(normalRetirementPhase_==P::managerCountRetired){
        const auto step=scope_->clearRetiredCurrentManager2A35C0(normalManagerGeneration_);
        if(!step.okay())return fail(step.sourcePC,"Normal Garage dispatcher current30 clear refused");
        managerPublished1B8274_=false;normalRetirementPhase_=P::currentCleared;
        // The shared dispatcher/Application/audio remain alive for the next
        // menu. No cold-abort backing close or invented source pointer stores.
    }
    if(normalRetirementPhase_!=P::currentCleared)return fail(0x2A3B04,"Normal Garage manager suffix is out of order");
    error.clear();return true;
}

bool OriginalUiGarageSession50KG::normalSourceOwnersRetired(D& d,std::string& error)const noexcept {
    d={};
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        d.status=assets::OriginalStandardMaterialStatus50LA::source;d.sourcePC=pc;
        return custodyFailure(error,text);
    };
    if(ownerThread_!=std::this_thread::get_id()||normalRetirementPhase_!=NormalRetirementPhase::currentCleared||
       !retiring_||!published_||closed_||!scope_||!grant_||!page_||!resources_||
       scope_->closed_||!scope_->dispatcherLive_||scope_->managerLive_||scope_->managerRetiring_||
       scope_->managerForestRetiring_||scope_->dispatcherCurrentManager30_||!scope_->garageChildren_.empty()||
       scope_->completedManagerPage1413E0_!=page_.get()||
       scope_->completedManagerPageGeneration_!=normalManagerGeneration_||
       scope_->completedManagerPageEpoch_!=grant_->epoch_||grant_->epoch_!=scope_->managerEpoch_)
        return fail(0x2A35C0,"Normal Garage native release requires this exact completed manager epoch");
    if(page_->stage_!=OriginalUiGaragePageOwner50KG::Stage::pagePayloadRetired||
       page_->entityLive_||!page_->entityRefReleased_||!page_->entityCountReleased_||
       page_->entity_.reference00||page_->scene4C0_||page_->sceneBacking_||podScene_||page_->quadCount_||page_->textCount_||
       page_->router_||page_->controlConstructionCount_||page_->groupConstructionCount_||
       page_->sharedCamera_||page_->sharedCameraDisplayPinned_)
        return fail(0x1FA2AC,"Normal Garage native release retains an unfinished Page payload");
    // The Page may still own a *retired* Scene tombstone until the actual
    // Main routing command alias is released. Its audio owner checks that
    // tombstone before this capsule is allowed to drop the Page itself.
    OriginalUiGarageActors50GN::Observation actors;
    if(!actors_||!actors_->inspect(actors)||actors.stage!=OriginalUiGarageActors50GN::Stage::pageManagerRetired||
       !actors.toyList3C8||actors.toyList3C8->reference00||actors.toyList3C8->firstChild18||
       !actors.manager49C||actors.manager49C->entity.reference00)
        return fail(0x101FE0,"Normal Garage native release retains unfinished Actor/Toy owners");
    OriginalUiGarageWattoOwner50KG::Observation watto;
    if(!watto_||!watto_->inspect(watto)||watto.stage!=OriginalUiGarageWattoOwner50KG::Stage::closed)
        return fail(0x1D4480,"Normal Garage native release retains an unfinished Watto owner");
    if(!normalSelected_||!normalSelected_->closed_||!visibility_||!visibility_->closed_||
       !visibility_->searchEntitiesRetired_)
        return fail(0x28976C,"Normal Garage native release retains selected/visibility lifetime callbacks");
    // Every canonical model allocation must have completed its own exact
    // packet/material/geometry/base callbacks, not just disappeared from Scope.
    for(const auto& model:resources_->population_)if(!model||!model->closed_)
        return fail(0x26E410,"Normal Garage native release retains a model resource payload");
    if(!widgets_||!widgets_->detachedReleased_||!widgets_->tableReleased_||
       !widgets_->fog_||!widgets_->fog_->payloadReleased_||!widgets_->fog_->lights_||
       !widgets_->fog_->lights_->payloadReleased_||!widgets_->fog_->lights_->search_)
        return fail(0x2A3B18,"Normal Garage native release retains manager Widget/Fog/Light payloads");
    const auto& fog=*widgets_->fog_;const auto& lights=*fog.lights_;const auto& search=*lights.search_;
    if(entryFrontend_&&(!sameCustodyShared(entrySelected_,normalSelected_)||
       !sameCustodyShared(entrySearch_,lights.search_)||!sameCustodyShared(entryLights_,fog.lights_)||
       !sameCustodyShared(entryFog_,widgets_->fog_)||managerDispatcher2AC_!=&scope_->dispatcher_))
        return fail(0x1413E0,"Normal Garage native release does not own its original constructor chain");
    if(!search.payloadReleased_||search.nodeLive_||search.selectedBorrow_||search.camera_||search.cameraObserver_||
       grant_->stillBorrowingGarageWidget1Owner(widgets_.get())||
       grant_->stillBorrowingGarageManagerFogOwner(&fog)||
       grant_->stillBorrowingGarageUiLightsOwner(&lights)||grant_->stillBorrowingManagerSearchOwner(&search)||
       (lights.manager_&&lights.manager_->uiLightsOwner_==&lights)||managerPageTable144_||
       !managerPageTableReleased2A3B04_||managerDat298_||managerArchiveOnlyScope170_||managerPublished1B8274_)
        return fail(0x1413E0,"Normal Garage native release still has a counted manager payload borrow");
    error.clear();return true;
}

}
namespace original_main_entry50KG {
bool Application::beginNativeFrontendRootAudio140CE0(Frontend& frontend,
    const rr::platform::OriginalUiMainSessionUpdate50KG& input,
    rr::platform::OriginalUiFrontendDiagnostic50KG& d,std::string& error)const noexcept {
    using Phase=Frontend::NativeRootAudioPhase;
    using C=rr::game::OriginalWreckComponent50LA;
    using K=rr::game::OriginalWreckComponentKind50LA;
    const auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        d.status=rr::platform::OriginalUiFrontendStatus50KG::session;d.sourcePC=pc;
        return custodyFailure(error,text);
    };
    if(frontend.nativeRootAudioPhase_==Phase::failed||frontend.nativeRootAudioPhase_==Phase::entered||
       frontend.nativeRootAudioPhase_==Phase::awaitingDispatcher)
        return fail(frontend.nativeRootAudioSourcePC_,"Native frontend retains an unfinished Root audio prefix; source service cannot replay");
    const auto backing=garageAudioBackingOwner_;const auto light=sourceLightManager426F60Owner_;
    const auto aural=sourceAural00Owner_;const auto scope=sharedUiScopeOwner_;
    const auto display=raceDisplayOwner50LA_.lock();const auto before=frontend.coordinator.snapshot();
    const auto rejected=[](bool failed,const char* prerequisite) noexcept {
        if(failed)std::fprintf(stderr,"frontend_root_audio_rejected prerequisite=%s\n",prerequisite);
        return failed;
    };
    if(rejected(frontend.materialApplicationOwner!=this,"frontend.materialApplicationOwner!=this")||
       rejected(!garageAudioApplicationInputsCurrent(),"!garageAudioApplicationInputsCurrent()")||
       rejected(!backing,"!backing")||
       rejected(!backing->inputsCurrent(*this),"!backing->inputsCurrent(*this)")||
       rejected(backing->failed_,"backing->failed_")||
       rejected(backing->closed_,"backing->closed_")||
       rejected(backing->frontendOutputOwner_!=&frontend.audioOutput,"backing->frontendOutputOwner_!=&frontend.audioOutput")||
       rejected(backing->frontendOutputClosed_,"backing->frontendOutputClosed_")||
       rejected(!aural,"!aural")||
       rejected(!light,"!light")||
       rejected(!scope,"!scope")||
       rejected(!display,"!display")||
       rejected(display->application_!=this,"display->application_!=this")||
       rejected(!display->canRetain(),"!display->canRetain()")||
       rejected(!frontend.coordinator.borrowsRenderer(display->renderer_),"!frontend.coordinator.borrowsRenderer(display->renderer_)")||
       rejected(!sameCustodyShared(aural->entities_,entitiesOwner_),"!sameCustodyShared(aural->entities_,entitiesOwner_)")||
       rejected(!sameCustodyShared(light->entities_,entitiesOwner_),"!sameCustodyShared(light->entities_,entitiesOwner_)")||
       rejected(!sameCustodyShared(scope->application_,entitiesOwner_),"!sameCustodyShared(scope->application_,entitiesOwner_)")||
       rejected(light->issuer_!=this,"light->issuer_!=this")||
       rejected(!light->current(),"!light->current()")||
       rejected(!aural->current(),"!aural->current()")||
       rejected(!aural->backendCurrent(),"!aural->backendCurrent()")||
       rejected(!nativeSourceAudioDispatcherCurrent(*aural),"!nativeSourceAudioDispatcherCurrent(*aural)")||
       rejected(!before.active,"!before.active")||
       rejected(before.faulted,"before.faulted")||
       rejected(before.quitRequested,"before.quitRequested")||
       rejected(before.deferredQueue,"before.deferredQueue")||
       rejected(before.updates!=frontend.lastAudioSequence,"before.updates!=frontend.lastAudioSequence")||
       rejected(before.submissions!=before.updates,"before.submissions!=before.updates")||
       rejected(before.updates==std::numeric_limits<std::uint64_t>::max(),"before.updates==std::numeric_limits<std::uint64_t>::max()"))
        return fail(0x140CE0,"Native frontend audio prefix lost its actual Root, display, output or dispatcher owner");
    // The concrete native coordinator below owns only these ordinary managers.
    // Garage has its own complete retained Root cursor and must never be serviced twice.
    if((before.route.screen!=4&&before.route.screen!=5&&before.route.screen!=6&&
        before.route.screen!=9&&before.route.screen!=12&&before.route.screen!=11&&
        before.route.screen!=7&&before.route.screen!=10)||   // UIWin 7, UIUnlock 10
       (garageCustodyOwner_&&garageCustodyOwner_->runtimeRootPhase_!=
          rr::platform::OriginalUiGarageSession50KG::RootRuntimePhase::notStarted))
        return fail(0x140CE0,"Native ordinary manager cannot consume a Garage Root publication");
    if((sourceApplicationMode22C_!=0&&sourceApplicationMode22C_!=1)||sourceUpdateRenderBypass228Bit3_)
        return fail(0x140CA4,"Actual Root phase or bypass excludes this native frontend update prefix");
    original_ui_source_timing50LH::PublicationView actual;
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!frontend.sourceDispatchTiming.retainPublication(actual)||!actual.readPrepared(sample,serial)||
       (frontend.nativeRootAudioPublication_&& !actual.sameOwner(frontend.nativeRootAudioTiming_))||
       frontend.nativeRootAudioPublication_==std::numeric_limits<std::uint64_t>::max()||
       serial!=frontend.nativeRootAudioPublication_+1||
       std::bit_cast<std::uint32_t>(input.elapsedSeconds)!=std::bit_cast<std::uint32_t>(sample.updateSeconds)||
       !std::isfinite(input.elapsedSeconds)||input.elapsedSeconds<0.f||(input.paused&&input.elapsedSeconds!=0.f))
        return fail(0x140CE4,"Native frontend audio prefix requires this exact prepared capped source factor");
    C* const outer=entitiesOwner_->application26C();C* const root=entitiesOwner_->application10();
    C* const lightNode=&light->entity_;C* const auralNode=&aural->entity_;C* const dispatcher=&scope->dispatcher_;
    // nativeSourceAudioDispatcherCurrent authenticated the optional, inactive
    // process-owned loading manager and its exact links before this traversal.
    const C* const loadingNode=sourceLoading330Owner_?&sourceLoading330Owner_->manager_:nullptr;
    const C* const afterAural=loadingNode?loadingNode:dispatcher;
    const C* const beforeDispatcher=loadingNode?loadingNode:auralNode;
    const auto eligible=[](const C& node)noexcept{return (node.flags29&0x0du)==1u&&(node.mask20&2u);};
    if(!outer||!root||outer->kind!=K::entity141300||root->kind!=K::entity141300||
       !scope->linked(*outer,nullptr)||outer->previous10||outer->next14||outer->firstChild18!=root||
       !(outer->descendants24&2u)||!scope->linked(*root,outer)||root->previous10||root->next14||
       !eligible(*root)||!(root->descendants24&2u)||root->firstChild18!=lightNode||
       !scope->linked(*lightNode,root)||lightNode->previous10||lightNode->next14!=auralNode||
       !scope->linked(*auralNode,root)||auralNode->previous10!=lightNode||auralNode->next14!=afterAural||
       !scope->linked(*dispatcher,root)||dispatcher->previous10!=beforeDispatcher||dispatcher->next14||!eligible(*dispatcher))
        return fail(0x141F00,"Native frontend audio prefix does not own the actual eligible Light/Aural/dispatcher chain");
    // Native bookkeeping begins only after all nonmutating ownership checks.
    // Real source effects below are never rolled back or retried after failure.
    frontend.nativeRootAudioTiming_=actual;frontend.nativeRootAudioPublication_=serial;
    frontend.nativeRootAudioExpectedUpdate_=before.updates+1;
    frontend.nativeRootAudioPhase_=Phase::entered;frontend.nativeRootAudioSourcePC_=0x140CE4;
    sourceApplicationMode22C_=2;sourceApplicationElapsed23C_=sample.updateSeconds;
    const auto stop=[&](std::uint32_t pc,const char* text)noexcept {
        frontend.nativeRootAudioPhase_=Phase::failed;frontend.nativeRootAudioSourcePC_=pc;return fail(pc,text);
    };
    const auto service=[&]()noexcept {
        rr::platform::VehicleWindowDiagnostic50DS rd;std::int32_t value=0;
        const auto result=rr::game::serviceOriginalRenderer244D60(display->dispatch_,false,display->renderer_,rd,value);
        if(result.status!=rr::game::OriginalCameraStartStatus::complete)
            return stop(result.sourcePC,"Actual preceding Entity renderer service failed; Root prefix retained");
        return true;
    };
    if(!service())return false; //Outer141ED0 -> eligible root10.
    if(eligible(*lightNode)){
        if(!service())return false;
        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic ld;
        if(!light->update1483E0(sample.updateSeconds,ld))return stop(ld.sourcePC,"Actual Light callback rejected its reached prefix");
    }
    // Inactive Aural takes the real skip branch, never a manufactured update.
    if(eligible(*auralNode)){
        if(!service())return false;
        rr::platform::OriginalApplicationAural50KG::Diagnostic ad;
        if(!aural->update103720(sample.updateSeconds,ad))return stop(ad.sourcePC,"Actual Aural callback rejected its reached source service");
    }
    if(!service())return false; //Immediately precedes existing native dispatcher.
    frontend.nativeRootAudioPhase_=Phase::awaitingDispatcher;frontend.nativeRootAudioSourcePC_=0x141F60;
    error.clear();return true;
}
bool Application::finishNativeFrontendRootAudio140CFC(Frontend& frontend,
    const rr::platform::OriginalUiFrontendUpdateReceipt50KG& receipt,
    rr::platform::OriginalUiFrontendDiagnostic50KG& d,std::string& error)const noexcept {
    using Phase=Frontend::NativeRootAudioPhase;
    const auto fail=[&](const char* text)noexcept {
        frontend.nativeRootAudioPhase_=Phase::failed;frontend.nativeRootAudioSourcePC_=0x140CFC;
        d.status=rr::platform::OriginalUiFrontendStatus50KG::session;d.sourcePC=0x140CFC;
        return custodyFailure(error,text);
    };
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    const auto snapshot=frontend.coordinator.snapshot();
    if(frontend.materialApplicationOwner!=this||frontend.nativeRootAudioPhase_!=Phase::awaitingDispatcher||
       !frontend.nativeRootAudioTiming_.readPrepared(sample,serial)||serial!=frontend.nativeRootAudioPublication_||
       !receipt.accepted||receipt.sequence!=frontend.nativeRootAudioExpectedUpdate_||
       snapshot.updates!=receipt.sequence||snapshot.faulted||sourceApplicationMode22C_!=2||!sourceApplicationElapsed23C_||
       std::bit_cast<std::uint32_t>(sample.updateSeconds)!=std::bit_cast<std::uint32_t>(*sourceApplicationElapsed23C_)||
       std::bit_cast<std::uint32_t>(receipt.update.elapsedSeconds)!=std::bit_cast<std::uint32_t>(sample.updateSeconds))
        return fail("Native dispatcher did not return for the same actual Root audio publication");
    // Existing coordinator.update is the bounded native manager equivalent,
    // not an original full-tree141ED0 receipt. Its actual accepted return is
    // required before this native enclosing continuation restores update mode.
    sourceApplicationMode22C_=0;frontend.nativeRootAudioPhase_=Phase::returned;
    frontend.nativeRootAudioSourcePC_=0x140CFC;error.clear();return true;
}
bool Application::retainGarageUpdateFrame140CF4(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using Actors=rr::platform::OriginalUiGarageActors50GN;
    using Renderer=rr::platform::OriginalUiGarageRendererLease50KG;
    using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    using C=rr::game::OriginalWreckComponent50LA;
    using P=Custody::RuntimePhase;
    d={};error.clear();const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        d.status=Status::source;d.sourcePC=pc;return custodyFailure(error,text);
    };
    if(!owner)return fail(0x141ED0,"Garage dispatcher has no actual retained manager");
    auto& s=*owner;
    // A reached incomplete source callback is retained, never replayed using
    // a different elapsed sample or represented as an accepted frame.
    if(s.runtimePhase_==P::unavailable)
        return fail(s.runtimeSourcePC_,"Garage update stopped at its retained required callback; no frame accepted");
    if(s.runtimePhase_==P::dispatcherReturned)
        return fail(0x141F68,"Garage dispatcher already returned for its retained publication; enclosing Application Entity traversal and later manager lifecycle are not joined");
    if(!s.publishedOwnerCurrent(*this)||s.entryFrontend_!=&frontend||
       s.managerDrivePhase_!=Custody::ManagerDrivePhase::returned||
       s.managerLoadPhase_!=Custody::ManagerLoadPhase::published||!s.managerPublished1B8274_||
       !s.managerUpdateFieldsConstructed_||!s.scope_||!s.actorInputs_||!s.actors_||!s.widgets_||
       !s.page_||!state||!state->sharedDialogs||
       !frontend.garageTransferOwnerCurrent(s.grant_)||
       !s.scope_->publishedPopulatedGarageManagerCurrent(s.entryManagerGeneration_))
        return fail(0x141ED0,"Garage update needs its genuine complete source load and same Frontend, not cold seam custody");
    auto& scope=*s.scope_;auto& shared=*state->sharedDialogs;
    if(shared.scope_.get()!=&scope||!shared.loadedCurrent()||!s.widgets_->pageEntryCustodyCurrent(*s.page_))
        return fail(0x141ED0,"Garage dispatcher payload identities are not current");
    //Fresh1D1950 calls1B86D0 while manager is DETACHED; only1B8268
    //subsequently appends manager. The current real chain is470,47C,manager.
    const C* first=&shared.dialog470_.entity;const C* second=&shared.controller47C_.entity;
    const C* manager=&scope.manager_;
    if(scope.dispatcher_.firstChild18!=first||first->previous10||first->next14!=second||
       second->previous10!=first||second->next14!=manager||manager->previous10!=second||manager->next14||
       first->parent1C!=&scope.dispatcher_||second->parent1C!=&scope.dispatcher_||
       manager->parent1C!=&scope.dispatcher_||!(scope.dispatcher_.descendants24&2u))
        return fail(0x141F00,"Garage dispatcher topology differs from the reached first-entry lifetime");
    original_ui_source_timing50LH::PublicationView actualTiming;
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!frontend.sourceDispatchTiming.retainPublication(actualTiming)||
       !actualTiming.sameOwner(s.entryTiming_)||!actualTiming.readPrepared(sample,serial))
        return fail(0x140AC0,"Garage update requires the actual already-prepared Frontend source timestamp");
    if(!s.runtimeFrame_){
        if(s.runtimePhase_!=P::notStarted)
            return fail(0x140CF4,"Garage callback history lost its original frame borrow");
        auto renderer=Renderer::retain(this,raceDisplayOwner50LA_.lock());
        if(!renderer)return fail(0x141F3C,"Garage update cannot borrow its actual live renderer");
        try{s.runtimeFrame_=std::shared_ptr<const Actors::RuntimeFrame>(new Actors::RuntimeFrame(s.actorInputs_,renderer));}
        catch(...){return fail(0x141ED0,"Garage native runtime-frame custody allocation failed before dispatch");}
        float dt=0;std::uint32_t pause=0;
        if(!s.actors_->currentRuntimeFrame(*s.runtimeFrame_,dt,pause)||dt!=sample.updateSeconds||
           s.runtimeFrame_->publication_!=serial){s.runtimeFrame_.reset();
            return fail(0x140AC0,"Garage RuntimeFrame has no exact prepared source/pause/clock ownership");}
    }
    if(!s.runtimeFrame_||s.runtimeFrame_->publication_!=serial||
       !s.runtimeFrame_->timing_.sameOwner(actualTiming)||!s.runtimeFrame_->renderer_||
       !s.runtimeFrame_->renderer_->currentFor(this))
        return fail(0x140AC0,"Garage pending update lost its exact publication or renderer; no replay");
    return true;
}

bool Application::continueGarageRootUpdate140CE0(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using RP=Custody::RootRuntimePhase;
    using DP=Custody::RuntimePhase;
    using C=rr::game::OriginalWreckComponent50LA;
    using K=rr::game::OriginalWreckComponentKind50LA;
    using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    d={};error.clear();const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        d.status=Status::source;d.sourcePC=pc;return custodyFailure(error,text);
    };
    if(!owner)return fail(0x140CE0,"Garage enclosing Root has no actual retained manager");
    auto& s=*owner;
    if(s.runtimeRootPhase_==RP::unavailable||s.runtimeRootPhase_==RP::suffixUnavailable)
        return fail(s.runtimeRootSourcePC_,"Garage enclosing Root stopped at its retained required operation; no replay or frame acceptance");
    // Never insert earlier Root services after a dispatcher-only invocation.
    if(s.runtimeRootPhase_!=RP::notStarted||s.runtimePhase_!=DP::notStarted||
       s.runtimeFrame_||s.runtimeVisitedChildren_||s.runtimeManagerReturnPC_||s.runtimeDispatcherReturnPC_)
        return fail(0x140CE0,"Garage Root cursor must originate before its actual dispatcher publication is consumed");
    if(sourceApplicationMode22C_!=0&&sourceApplicationMode22C_!=1)
        return fail(0x140CE0,"Garage Root is already inside an unsupported Application update/render phase");
    // Preserve the actual constructor-backed entry envelope. This check is
    //not a receipt for128080/245030 or for execution of the earlier branch.
    if(sourceUpdateRenderBypass228Bit3_)
        return fail(0x140CA4,"Garage Root's actual bit3 excludes this update/render continuation");
    if(!retainGarageUpdateFrame140CF4(frontend,d,error)){
        // A failed issued native borrow is kept as this invocation's exact
        //prefix, even if no source child has run. Do not hide its first PC
        //behind a later generic "cursor already issued" rejection.
        if(s.runtimeFrame_){s.runtimeRootPhase_=RP::unavailable;
            s.runtimeRootSourcePC_=d.sourcePC?d.sourcePC:0x140CF4;}
        return false;
    }
    auto stop=[&](std::uint32_t pc,const char* text)noexcept {
        s.runtimeRootPhase_=RP::unavailable;s.runtimeRootSourcePC_=pc;return fail(pc,text);
    };
    auto& scope=*s.scope_;
    const auto light=sourceLightManager426F60Owner_;
    const auto aural=sourceAural00Owner_;
    if(!scope.live().okay()||!sameCustodyShared(scope.application_,entitiesOwner_)||
       !light||light->issuer_!=this||!sameCustodyShared(light->entities_,entitiesOwner_)||!light->current()||
       !aural||aural->issuer_!=this||!sameCustodyShared(aural->entities_,entitiesOwner_)||!aural->current()||!aural->attached_)
        return stop(0x140CF4,"Garage Root lacks its actual same-Application Light/Aural/dispatcher owners");
    C* const outer=entitiesOwner_->application26C();
    C* const root=entitiesOwner_->application10();
    C* const lightNode=&light->entity_;C* const auralNode=&aural->entity_;
    C* const dispatcher=&scope.dispatcher_;
    const auto generation=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(entitiesOwner_.get()));
    auto actualRoot=[&](const C* c)noexcept {
        return c&&c->kind==K::entity141300&&c->sessionGeneration==generation&&c->reference00==1&&
            c->objectKey==static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(c));
    };
    // The process loading manager 330 (17CE2C) is a later Root sibling than
    // Aural. A first-boot dispatcher precedes it; a successor dispatcher
    // (rebuilt after a race, 17C1D8) is appended after it. Exactly that one
    // retained sibling may sit between Aural and the dispatcher.
    C* const loading=sourceLoading330Owner_&&sourceLoading330Owner_->managerAttached_?
        &sourceLoading330Owner_->manager_:nullptr;
    const bool auralThenDispatcher=auralNode->next14==dispatcher||
        (loading&&auralNode->next14==loading&&loading->next14==dispatcher&&scope.linked(*loading,root));
    if(!actualRoot(outer)||!actualRoot(root)||!scope.linked(*outer,nullptr)||
       outer->previous10||outer->next14||outer->firstChild18!=root||
       !scope.linked(*root,outer)||root->previous10||root->firstChild18!=lightNode||
       !scope.linked(*lightNode,root)||lightNode->previous10||lightNode->next14!=auralNode||
       !scope.linked(*auralNode,root)||!auralThenDispatcher||!scope.linked(*dispatcher,root)){
        std::fprintf(stderr,"garage_root_links outer=%d root=%d outer_linked=%d outer_sib=%d outer_first_root=%d root_linked=%d root_prev=%d root_first_light=%d light_linked=%d light_prev=%d light_next_aural=%d aural_linked=%d aural_next_dispatcher=%d dispatcher_linked=%d\n",
            int(actualRoot(outer)),int(actualRoot(root)),int(outer&&scope.linked(*outer,nullptr)),int(outer&&(outer->previous10||outer->next14)),
            int(outer&&outer->firstChild18==root),int(root&&outer&&scope.linked(*root,outer)),int(root&&root->previous10!=nullptr),
            int(root&&root->firstChild18==lightNode),int(root&&scope.linked(*lightNode,root)),int(lightNode->previous10!=nullptr),
            int(lightNode->next14==auralNode),int(root&&scope.linked(*auralNode,root)),int(auralNode->next14==dispatcher),
            int(root&&scope.linked(*dispatcher,root)));
        if(root)for(const C* c=root->firstChild18;c;c=c->next14)
            std::fprintf(stderr,"garage_root_child kind=%u light=%d aural=%d dispatcher=%d refs=%u gen_root=%d\n",unsigned(c->kind),
                int(c==lightNode),int(c==auralNode),int(c==dispatcher),unsigned(c->reference00),int(c->sessionGeneration==generation));
        return stop(0x140CF4,"Garage Root's actual enrolled intrusive links do not match its retained native owners");}
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!s.runtimeFrame_->timing_.readPrepared(sample,serial)||serial!=s.runtimeFrame_->publication_)
        return stop(0x140CE4,"Garage Root lost the same prepared source factor before publishing23C");
    s.runtimeRootPublication_=serial;
    // The API is explicitly scoped at140CE0, not a claim that the earlier
    //input/245030 nonblocking-status producer has executed. These are actual
    //source stores on this Root, not readiness flags or copied raw250.
    sourceApplicationMode22C_=2;              //140CE0
    sourceApplicationElapsed23C_=sample.updateSeconds; //140CE4
    s.runtimeRootSourcePC_=0x140CF4;
    s.runtimeRootPhase_=RP::outerChildren;
    s.runtimeOuterChild_=(outer->descendants24&2u)?outer->firstChild18:nullptr;
    auto& renderer=*s.runtimeFrame_->renderer_;
    auto eligible=[](const C& c)noexcept {return (c.flags29&0x0du)==1u&&(c.mask20&2u);};
    auto service=[&]()noexcept {
        rr::platform::VehicleWindowDiagnostic50DS rd;rr::game::OriginalCameraStartResult result;
        if(renderer.serviceEntity141F3C(rd,result))return true;
        return stop(result.sourcePC,"Garage enclosing Entity renderer service is unavailable; prior source effects remain retained");
    };
    while(s.runtimeOuterChild_){
        C* const child=s.runtimeOuterChild_;
        if(!scope.live().okay()||!scope.linked(*child,outer))
            return stop(0x141F00,"Garage outer Entity cursor lost its actual live child");
        if(eligible(*child)){
            if(!service())return false;
            if(child!=root)
                return stop(0x141F60,"Garage outer Entity reached an unsupported actual sibling callback");
            s.runtimeRootPhase_=RP::rootChildren;
            s.runtimeRootChild_=(root->descendants24&2u)?root->firstChild18:nullptr;
            while(s.runtimeRootChild_){
                C* const inner=s.runtimeRootChild_;
                if(!scope.live().okay()||!scope.linked(*inner,root))
                    return stop(0x141F00,"Garage root10 cursor lost its actual live child");
                if(eligible(*inner)){
                    if(!service())return false;
                    if(inner==lightNode){
                        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic ld;
                        if(!light->update1483E0(*sourceApplicationElapsed23C_,ld))
                            return stop(ld.sourcePC,"Garage preceding Light update requires an unsupported actual branch");
                        s.runtimeLightReturnPC_=ld.sourcePC;
                    }else if(inner==auralNode){
                        rr::platform::OriginalApplicationAural50KG::Diagnostic ad;
                        // Actual eligible child after its preceding renderer
                        // service. Consume THIS Root's published capped23C,
                        // never Main fade dt, wall time or DSP samples.
                        if(!aural->update103720(*sourceApplicationElapsed23C_,ad))
                            return stop(ad.sourcePC,"Garage actual Aural source service rejected its reached body");
                    }else if(inner==dispatcher){
                        // Ordinary native managers service Main's music
                        // envelope in prepareUpdateAudio before their Page
                        // callbacks. Garage owns this Root instead, so service
                        // ONLY that retained envelope at the same boundary.
                        // The actual capped23C/publication was established
                        // above; Aural ages already ran and PCM runs later once.
                        // This Root cursor cannot restart after any failure.
                        if(!s.audio_||!s.page_||
                           !s.audio_->advanceGarageNativeMusicControl(*s.page_,*sourceApplicationElapsed23C_))
                            return stop(0x234440,"Garage retained native music-control service refused its exact Page/time");
                        s.runtimeRootPhase_=RP::dispatcher;
                        const bool returned=continueGarageDispatcherUpdate141ED0(frontend,d,error);
                        (void)returned; // A scoped return receipt, not bool, discharges this exact child.
                        if(s.runtimePhase_!=DP::dispatcherReturned||s.runtimeManagerReturnPC_!=0x2A3FF0||
                           s.runtimeDispatcherReturnPC_!=0x141F88||s.runtimeRootPublication_!=serial||
                           s.runtimeFrame_->publication_!=serial){
                            s.runtimeRootPhase_=RP::unavailable;
                            s.runtimeRootSourcePC_=d.sourcePC?d.sourcePC:0x141F60;
                            if(d.sourcePC==0x10179C)s.retainActorSampleFailure10179C(*this);
                            if(!d.sourcePC)return fail(s.runtimeRootSourcePC_,"Garage dispatcher did not establish its exact scoped callback return");
                            return false; // Preserve its concrete failed callback/diagnostic.
                        }
                        d={};error.clear();s.runtimeRootPhase_=RP::rootChildren;
                    }else return stop(0x141F60,"Garage root10 reached an unsupported actual child callback");
                }
                // Inactive Aural/other actual nodes take the original skip,
                //not an invented callback receipt. Read next14 after return.
                s.runtimeRootChild_=inner->next14;
            }
            s.runtimeRoot10ReturnPC_=0x141F88;s.runtimeRootPhase_=RP::rootReturned;
        }
        s.runtimeOuterChild_=child->next14;
        s.runtimeRootPhase_=RP::outerChildren;
    }
    s.runtimeOuter26CReturnPC_=0x141F88;s.runtimeRootPhase_=RP::outerReturned;
    sourceApplicationMode22C_=0; //140CFC, ONLY after genuine outer26C return.
    //Actual140D08->237970->140230, using the SAME retained Frontend
    //publication and real Root/Page/Display owners. Never retrofit a cursor
    //after dispatcher dispatch or publish a partially composed SourceFrame.
    using Draw=rr::platform::OriginalUiGarageDrawContribution50KG;
    // Later publications reuse the same real renderer/bank owner only after
    // prepareNextGarageFrame140CE0 consumed its published prior target.
    if(!s.drawContribution_){
        try{s.drawContribution_=std::shared_ptr<Draw>(new Draw(*this,owner,s.runtimeFrame_->renderer_,
            s.runtimeFrame_->timing_,serial,&scope.manager_));}
        catch(...){return stop(0x140D08,"Garage real render-owner custody allocation failed");}
    }
    Draw::Diagnostic render;
    if(!s.drawContribution_->continue140230(render)){
        s.runtimeRootPhase_=RP::suffixUnavailable;s.runtimeRootSourcePC_=render.sourcePC;
        d.sourceEffectsCommitted=true;
        return fail(render.sourcePC,"Garage live render prefix retained its exact pending callback; no complete SourceFrame or Root acceptance exists");
    }
    using TP=Custody::RootTailPhase;
    auto stopTail=[&](std::uint32_t pc,const char* text)noexcept {
        s.runtimeRootPhase_=RP::suffixUnavailable;s.runtimeRootSourcePC_=pc;
        d.sourceEffectsCommitted=true;return fail(pc,text);
    };
    // Bool alone is not a render receipt. The actual Draw's completed source
    // phase and exact Application/publication must still authenticate it.
    if(!s.drawContribution_->renderReturned140230Current(*this)||
       s.runtimeTailPhase_!=TP::notStarted||sourceApplicationMode22C_!=0)
        return stopTail(0x140D10,"Garage render did not retain its exact complete140230 return");
    s.runtimeTailPhase_=TP::renderReturned;
    const auto display=raceDisplayOwner50LA_.lock();
    if(!display||!sameCustodyShared(display,renderer.display_)||display->application_!=this||
       !display->frontendOwnerCurrent50KG())
        return stopTail(0x140D14,"Garage post-render tail lost its same real renderer owner");
    rr::platform::VehicleWindowDiagnostic50DS frameDiagnostic;
    if(!display->advanceGarageFrame26F910(*s.drawContribution_,frameDiagnostic))
        return stopTail(0x26F910,"Garage real renderer buffer/frame advance refused; source tail retained");
    s.runtimeTailPhase_=TP::rendererAdvanced;s.runtimeRootSourcePC_=0x140D24;
    rr::platform::OriginalApplicationAural50KG::Diagnostic audioDiagnostic;
    if(!aural->finishNativeControls267BE0(audioDiagnostic))
        return stopTail(audioDiagnostic.sourcePC,"Garage actual Aural command boundary refused after renderer advance");
    s.runtimeTailPhase_=TP::audioReturned;
    const auto& audioInterface=**aural->interfaceA0_;
    s.runtimeTailAudioBoundary_=audioInterface.controlBoundaryCount;
    s.runtimeTailAudioFrame_=audioInterface.controlBoundaryFrame;
    s.runtimeTailMainRevision_=audioInterface.controlBoundaryMainRevision;
    s.runtimeTailMixerRevision_=audioInterface.controlBoundaryMixerRevision;
    s.runtimeTailGroupSequence_=audioInterface.controlBoundaryGroupSequence;
    // This is the existing native source-time owner, not the distinct PCM
    // timeline or a fabricated Application clock snapshot. Revalidate the SAME
    // prepared publication after callbacks, then its numeric140D2C..48 commit.
    original_ui_source_timing50LH::PublicationView liveTiming;
    original_ui_source_timing50LH::Sample liveSample;std::uint64_t liveSerial=0;
    if(!frontend.sourceDispatchTiming.retainPublication(liveTiming)||
       !liveTiming.sameOwner(s.runtimeFrame_->timing_)||!liveTiming.readPrepared(liveSample,liveSerial)||
       liveSerial!=serial||serial!=s.runtimeRootPublication_||!sourceApplicationElapsed23C_||
       std::bit_cast<std::uint32_t>(liveSample.updateSeconds)!=std::bit_cast<std::uint32_t>(*sourceApplicationElapsed23C_))
        return stopTail(0x140D2C,"Garage end-frame source timing is not its original prepared publication");
    s.runtimeTailSample_=liveSample;s.runtimeTailPublication_=liveSerial;
    if(!frontend.sourceDispatchTiming.commit())
        return stopTail(0x140D30,"Garage exact source timing commit refused after its completed audio boundary");
    s.runtimeTailPhase_=TP::timingCommitted;s.runtimeRootPhase_=RP::completed;
    s.runtimeRootReturnPC_=0x140D60;s.runtimeRootSourcePC_=0;
    // Caller must NOT run generic Frontend::commitSourceUpdateTiming again.
    // No PCM advancement, GL submission, frame sealing or per-frame rearm is
    // implied. Those require the retained real Draw and enclosing route owner.
    d={};d.sourceEffectsCommitted=true;d.sourcePC=0x140D60;error.clear();return true;
}

bool Application::continueGarageDispatcherUpdate141ED0(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using P=Custody::RuntimePhase;
    using RP=Custody::RootRuntimePhase;
    using C=rr::game::OriginalWreckComponent50LA;
    using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    d={};error.clear();const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        d.status=Status::source;d.sourcePC=pc;return custodyFailure(error,text);
    };
    if(!owner)return fail(0x141ED0,"Garage dispatcher has no actual retained manager");
    auto& s=*owner;
    if(s.runtimeRootPhase_!=RP::notStarted&&s.runtimeRootPhase_!=RP::dispatcher)
        return fail(s.runtimeRootSourcePC_?s.runtimeRootSourcePC_:0x140CF4,
            "Garage dispatcher cannot bypass its enclosing Root cursor");
    if(!retainGarageUpdateFrame140CF4(frontend,d,error))return false;
    auto& scope=*s.scope_;auto& shared=*state->sharedDialogs;
    const C* first=&shared.dialog470_.entity;const C* second=&shared.controller47C_.entity;
    const C* manager=&scope.manager_;
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    const auto& actualTiming=s.runtimeFrame_->timing_;
    if(!s.runtimeFrame_->timing_.readPrepared(sample,serial)||serial!=s.runtimeFrame_->publication_)
        return fail(0x140AC0,"Garage dispatcher lost its exact already-retained publication");
    if(s.runtimePhase_==P::notStarted)s.runtimePhase_=P::shared470;
    auto& renderer=*s.runtimeFrame_->renderer_;
    auto stop=[&](std::uint32_t pc,const char* text)noexcept {
        s.runtimePhase_=P::unavailable;s.runtimeSourcePC_=pc;return fail(pc,text);
    };
    auto eligible=[](const C& c)noexcept {return (c.flags29&0x0du)==1u&&(c.mask20&2u);};
    auto service=[&]()noexcept {
        rr::platform::VehicleWindowDiagnostic50DS rd;rr::game::OriginalCameraStartResult result;
        if(renderer.serviceEntity141F3C(rd,result))return true;
        return stop(result.sourcePC,"Garage Entity renderer service is incomplete; actual source prefix retained");
    };
    if(s.runtimePhase_==P::shared470){
        if(eligible(*first)){
            if(!service())return false;
            rr::platform::OriginalUiSharedDialogs50KG::Diagnostic sd;
            if(!shared.advance4702048F0(sample.updateSeconds,sd))
                return stop(sd.sourcePC,"Garage preceding shared470 update is incomplete");
        }
        s.runtimePhase_=P::shared47C;
    }
    if(s.runtimePhase_==P::shared47C){
        if(eligible(*second)){
            if(!service())return false;
            rr::game::OriginalUiControllerPopupPlan50KG plan;
            if(rr::game::OriginalUiControllerPopup50KG::update(state->controllerPopupState,sample.updateSeconds,plan)!=
               rr::game::OriginalUiControllerPopupStatus50KG::ok)
                return stop(0x206420,"Garage preceding real47C state update is incomplete");
            state->controllerPopupState=plan.state;
        }
        s.runtimePhase_=P::manager;
    }
    if(s.runtimePhase_==P::manager){
        if(!eligible(*manager))return stop(0x141F60,"Garage manager is not source-update eligible");
        if(!service())return false;
        if(!s.updateManagerState2A3C80(d))
            return stop(d.sourcePC,"Garage actual manager transition retained its required callback");
        //15C constructor-null skips virtual80;2A3FDC then calls141ED0.
        s.runtimeManagerChild_=manager->firstChild18;s.runtimePhase_=P::managerChildren;
    }
    while(s.runtimeManagerChild_){
        const C* child=s.runtimeManagerChild_;
        if(child->parent1C!=manager)return stop(0x141F00,"Garage manager child link changed during update");
        if(eligible(*child)){
            if(!service())return false;
            if(child==scope.managerSearchSelected_||child==scope.managerSearchNode_){
                //The actual inherited141ED0 takes its zero-descendant-mask
                //return; neither model geometry nor an observer is updated.
                if(child->descendants24&2u)return stop(0x141ED0,"Garage inherited child has a real unsupported update descendant");
            }else{
                bool textOuter=false;
                for(const auto& row:s.widgets_->rows_)if(row.outer && &row.outer->entity==child){textOuter=true;break;}
                if(textOuter){
                    rr::platform::OriginalUiGarageWidget1Diagnostic50KG wd;
                    if(!s.widgets_->updateTextOuter141ED0(*child,actualTiming,serial,renderer,wd))
                        return stop(wd.sourcePC,"Garage actual TextOverlay callback is incomplete; completed lines retained");
                    ++s.runtimeReturnedTextOuters_;
                }else if(child==s.widgets_->rows_[9].wattoEntity){
                    if(!s.watto_||s.widgets_->rows_[9].wattoEntity!=child)
                        return stop(0x1D4B10,"Garage Watto row lost its exact retained owner");
                    rr::game::OriginalUiGarageWattoReceipt50KG receipt;
                    rr::game::OriginalUiGarageWattoLifecycleDiagnostic50KG wd;
                    if(!s.watto_->advance1D4B10(*s.page_,actualTiming,serial,renderer,receipt,wd))
                        return stop(wd.sourcePC,"Garage Watto retained its reached motion/observer/audio/child prefix; callback incomplete");
                }else if(child==scope.managerFog_[0]||child==scope.managerFog_[1]||child==scope.managerFog_[2]){
                    if(!s.entryFog_||scope.managerFogOwner_!=s.entryFog_.get()||!s.entryFog_->currentAttached())
                        return stop(0x13E9B0,"Garage manager Fog callback lost its actual retained payload");
                    std::size_t index=0;while(index<3&&s.entryFog_->managerFields_[index]!=child)++index;
                    if(index==3)return stop(0x13E9B0,"Garage Fog source field and child identity disagree");
                    //Fog virtual38=13E9B0 is the literal return1, with NO
                    //child traversal. On/Off virtual38=141ED0 have no children.
                    if(index&&(child->descendants24&2u)&&child->firstChild18)
                        return stop(0x141F60,"Garage FogOn/Off acquired an unsupported real update child");
                }else if(child==&s.page_->entity_){
                    //All preceding actual children returned. Page owns its
                    //already-seeded34B4/34C8 fields; never activate a detached
                    //TimedActions copy or redraw Page-entry RNG here.
                    rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic pd;
                    if(!s.page_->continueUpdate1FB6A0(s,renderer,pd)){
                        d=pd.loadResources;
                        return stop(pd.sourcePC,"Garage Page retained its canonical timer/child prefix at the actual required callback");
                    }
                }else return stop(0x141F60,"Garage reached a later source child without its concrete update callback");
            }
        }
        ++s.runtimeVisitedChildren_;
        s.runtimeManagerChild_=child->next14; //Only after its callback returns.
    }
    //The actual manager child walk has reached its null next14.141F84
    //returns1;2A3FE4..2A3FF4 only restore registers/return, preserving v0.
    //There is no further manager effect or readiness operation to invent.
    if(s.runtimePhase_!=P::managerChildren)
        return stop(0x2A3FE4,"Garage manager lost its actual completed child-walk phase");
    s.runtimeManagerReturnPC_=0x2A3FF0;
    s.runtimePhase_=P::managerReturned;
    //Dispatcher3CF310+38 is141ED0. Read the real next14 only AFTER
    //the manager callback returned, even though initial topology was checked.
    //A newly appended sibling cannot be silently omitted by this bounded walk.
    if(manager->next14)
        return stop(0x141F68,"Garage dispatcher acquired a later actual sibling without its update owner");
    s.runtimeDispatcherReturnPC_=0x141F88;
    s.runtimeSourcePC_=0x141F68;
    s.runtimePhase_=P::dispatcherReturned;
    //The two real callback returns above are retained separately. This Root
    //continuation still cannot return success: it has no authentic enclosing
    //outer26C/root10 cursor or preceding sibling-return receipts. Preserve
    //the exact frame/publication and stop at that next enclosing Entity step.
    //No SourceTiming.commit, PCM advance or renderer submission follows.
    return fail(0x141F68,"Garage manager and dispatcher returned; enclosing Application Entity traversal is not joined and no Root frame is accepted");
}

void Application::prepareGarageDestinationResources1F241C(Frontend& frontend,
    const rr::platform::OriginalUiTournamentRouteProposal50KG& proposal,
    rr::platform::OriginalUiGarageDestinationDiagnostic50KG& d,
    rr::platform::OriginalUiFrontendColdInterval50KG& cold)const noexcept {
    using S=rr::platform::OriginalUiGarageDestinationStatus50KG;
    d={};d.proposalSerial=proposal.serial;d.requestSourcePC=0x1F241C;
    try {
        if(cold.measured||garageAudioOwnerThread_!=std::this_thread::get_id()||
           frontend.materialApplicationOwner!=this||!frontend.uploadLifetime_||
           !Frontend::uploadLifetimeLive(*frontend.uploadLifetime_)){
            d.status=S::rootOwner;d.pendingSourcePC=0x1B823C;return;
        }
        const auto* source=frontend.coordinator.tournamentSession();
        const auto& decision=proposal.decision;const auto& request=decision.request;
        if(!source||proposal.source!=source||!source->routeProposalCurrent(proposal)||
           !riderProfile||!source->ownsProfile(*riderProfile)||
           decision.admitted||decision.action!=rr::game::OriginalUiRouteAction50KG::unavailable||
           decision.blockers!=static_cast<std::uint32_t>(rr::game::OriginalUiRouteBlocker50KG::destinationOwner)||
           !decision.profileProposalComplete||decision.profileChanges.count||decision.inputPC!=0x1F2310||
           request.kind!=rr::game::OriginalUiRouteKind50KG::changeScreen||request.sourceScreen!=9||
           request.sourcePage!=1||request.targetScreen!=13||request.targetPage!=1||request.sourcePC!=0x1F241C){
            d.status=S::origin;d.pendingSourcePC=0x1F241C;return;
        }
        // Tournament-only: the proposal's rider must be the selected career.
        rr::platform::OriginalUiRiderProfileOwner50KG::GarageProfileSnapshot selected;
        if(riderProfile->commonProfile()!=profile.get()||!riderProfile->selectedGarageProfileSnapshot(selected)||
           decision.selectedRider!=selected.career.career100C0){
            d.status=S::profile;d.pendingSourcePC=0x1D1CD0;return;
        }
    }catch(...){d.status=S::allocation;return;}
    prepareGarageCareerResources1D1950(frontend,d,cold);
}
void Application::prepareGarageCareerResources1D1950(Frontend& frontend,
    rr::platform::OriginalUiGarageDestinationDiagnostic50KG& d,
    rr::platform::OriginalUiFrontendColdInterval50KG& cold)const noexcept {
    using S=rr::platform::OriginalUiGarageDestinationStatus50KG;
    using SceneAssets=rr::assets::OriginalUiGarageSceneAssets50KG;
    using Id=rr::assets::OriginalUiArchiveId50KG;
    // Caller owns origin admission and has set proposalSerial/requestSourcePC.
    const auto serial=d.proposalSerial;const auto requestPC=d.requestSourcePC;
    d.status=S::notRequested;d.pendingSourcePC=0;
    // Host-only cold observation. No SourceTiming sample, callback service or
    // drain occurs here. An already measured receipt cannot be overwritten.
    struct ColdSpan {
        rr::platform::OriginalUiFrontendColdInterval50KG& out;bool active=false;
        void start()noexcept{active=true;out.measured=true;out.begin=std::chrono::steady_clock::now();}
        ~ColdSpan(){if(active){out.end=std::chrono::steady_clock::now();
            const auto n=std::chrono::duration_cast<std::chrono::nanoseconds>(out.end-out.begin).count();
            out.nanoseconds=n>0?static_cast<std::uint64_t>(n):0;}}
    } interval{cold};
    try {
        if(cold.measured||garageAudioOwnerThread_!=std::this_thread::get_id()||
           frontend.materialApplicationOwner!=this||!frontend.uploadLifetime_||
           !Frontend::uploadLifetimeLive(*frontend.uploadLifetime_)){
            d.status=S::rootOwner;d.pendingSourcePC=0x1B823C;return;
        }
        rr::platform::OriginalUiRiderProfileOwner50KG::GarageProfileSnapshot selected;
        if(!riderProfile||riderProfile->commonProfile()!=profile.get()||!riderProfile->selectedGarageProfileSnapshot(selected)||
           selected.career.career100C0>=frontend.garageCareerResources_.size()||
           !rr::assets::originalUiGaragePodArchive50KG(selected.career.career100C0)||
           selected.career.stamp.mode34!=2){
            d.status=S::profile;d.pendingSourcePC=0x1D1CD0;return;
        }
        if(garageCustodyOwner_||garageCustodyAttempted_){
            // A real failed or completed source prefix cannot be adopted or
            // reset by another menu request. Report its genuine retained PC.
            d.status=S::retainedSourcePrefix;d.pendingSourcePC=0x1D1950;
            if(garageCustodyOwner_&&garageCustodyOwner_->entryFailureSourcePC_)
                d.pendingSourcePC=garageCustodyOwner_->entryFailureSourcePC_;
            return;
        }
        auto& retained=frontend.garageCareerResources_[selected.career.career100C0];
        frontend.garageDestinationAssetsAttempted_=retained.attempted;
        frontend.garageDestinationAssets_=retained.assets;
        frontend.garageDestinationFailure_=retained.failure;
        frontend.garageDestinationPod19_=retained.assets?retained.assets->selectedArchive():nullptr;
        // Keep the exact cold resource result for this career on every return,
        // including an allocation failure. Never replay a failed source attempt.
        struct RetainCareer {
            Frontend::GarageCareerResources& slot;Frontend& frontend;
            rr::platform::OriginalUiGarageDestinationDiagnostic50KG& diagnostic;
            ~RetainCareer()noexcept {
                slot.attempted=frontend.garageDestinationAssetsAttempted_;
                slot.assets=frontend.garageDestinationAssets_;
                try{slot.failure=slot.assets?frontend.garageDestinationFailure_:diagnostic;}
                catch(...){slot.failure.status=S::allocation;}
            }
        } retain{retained,frontend,d};
        if(frontend.garageDestinationAssetsAttempted_){
            if(!frontend.garageDestinationAssets_){
                d=frontend.garageDestinationFailure_;d.proposalSerial=serial;d.requestSourcePC=requestPC;return;
            }
        }else{
            interval.start();frontend.garageDestinationAssetsAttempted_=true;
            // The catalog is the sole path/digest authority. Reuse the native
            // Root's exact assetRoot, never an environment/test supplied bank.
            d.status=S::archive;d.pendingSourcePC=0x1D1950;d.archive=Id::garage;
            if(!frontend.garageDestinationArchive_)
                frontend.garageDestinationArchive_=readArchive(assetRoot,Id::garage);
            d.archive=rr::assets::originalUiGaragePodArchive50KG(selected.career.career100C0)->id;
            frontend.garageDestinationPod19_=readArchive(assetRoot,d.archive);
            d.status=S::sceneAssets;d.pendingSourcePC=0x2710F0;
            auto assets=std::make_shared<SceneAssets>();
            if(!assets->load(frontend.garageDestinationArchive_,frontend.garageDestinationPod19_,d.scene)){
                if(d.scene.sourcePC)d.pendingSourcePC=d.scene.sourcePC;
                frontend.garageDestinationFailure_=d;return;
            }
            frontend.garageDestinationAssets_=std::move(assets);
        }
        if(!frontend.garageDestinationAssets_->loaded()||
           frontend.garageDestinationAssets_->selectedCareer()!=static_cast<int>(selected.career.career100C0)||
           frontend.garageDestinationAssets_->archive()!=frontend.garageDestinationArchive_){
            d.status=S::sceneAssets;d.pendingSourcePC=0x2710F0;return;
        }
        // This is immutable resource admission only. The frontend must retire
        // its predecessor, run the actual manager/Page load, and accept only
        // actual returned Root/render/audio frames. No cold ready frame exists.
        d.status=S::resourcesPrepared;d.pendingSourcePC=0x1D1950;
        return;
    }catch(const std::bad_alloc&){d.status=S::allocation;}
    catch(...){if(d.status!=S::archive&&d.status!=S::sceneAssets)d.status=S::allocation;}
    if(frontend.garageDestinationAssetsAttempted_&&!frontend.garageDestinationAssets_){
        try{frontend.garageDestinationFailure_=d;}
        catch(...){frontend.garageDestinationFailure_.status=S::allocation;
            frontend.garageDestinationFailure_.requestSourcePC=requestPC;
            frontend.garageDestinationFailure_.pendingSourcePC=d.pendingSourcePC;}
    }
}
bool Application::loadGarageManager1D1950(Frontend& frontend,
    std::shared_ptr<const rr::assets::OriginalUiGarageSceneAssets50KG> assets,
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> fonts,std::int32_t requestedPage,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using P=Custody::ManagerDrivePhase;
    using CP=Custody::ManagerConstructorPhase;
    using Page=rr::platform::OriginalUiGaragePageOwner50KG;
    using PS=Page::Stage;
    using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    diagnostic={};
    auto inputFailure=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=Status::input;diagnostic.sourcePC=pc;return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||requestedPage!=1||
       !assets||!assets->loaded()||!fonts||!fonts->loaded()||
       fonts->identity()!=rr::assets::originalUiArchiveIdentity50KG(rr::assets::OriginalUiArchiveId50KG::fonts))
        return inputFailure(0x1B8258,"Garage normal loader requires authenticated source inputs and actual requested Page1");
    if(!garageCustodyOwner_){
        //This is the real production caller of the transfer/selected entry,
        //not an initialized-context test aperture or a detached seam recipe.
        if(!prepareGarageManagerEntry1D1950(frontend,assets,diagnostic,error))return false;
    }
    const auto& owner=garageCustodyOwner_;
    if(!owner||!owner->sameRoot(*this)||owner->entryFrontend_!=&frontend||
       !sameCustodyShared(owner->sceneAssets_,assets)||owner->retiring_)
        return inputFailure(0x1D1950,"Garage normal loader cannot adopt another or detached-cold entry");
    if(owner->failed_){
        diagnostic.status=owner->entryFailureStatus_==Status::complete?Status::source:owner->entryFailureStatus_;
        diagnostic.sourcePC=owner->entryFailureSourcePC_?owner->entryFailureSourcePC_:0x1D1950;
        return custodyFailure(error,"Garage reached source failure remains retained; the normal loader will not replay it");
    }
    auto& custody=*owner;
    if(!frontend.garageTransferOwnerCurrent(custody.grant_)||!custody.constructionCurrent(*this))
        return inputFailure(0x1D1950,"Garage normal loader lost its genuine Root/Frontend/manager lifetime");
    auto stopped=[&](std::uint32_t fallbackPC,const char* text)noexcept {
        if(diagnostic.status==Status::complete)diagnostic.status=Status::source;
        if(!diagnostic.sourcePC)diagnostic.sourcePC=fallbackPC;
        custody.failed_=true;custody.entryFailureStatus_=diagnostic.status;
        custody.entryFailureSourcePC_=diagnostic.sourcePC;
        //Keep the original helper's more specific native failure when present.
        if(error.empty())return custodyFailure(error,text);
        return false;
    };
    const auto sharedResumeSupported=[&]()noexcept {
        if(!custody.page_||!custody.page_->currentConstructor())return false;
        switch(custody.page_->stage_){
        case PS::sharedCameraViewportPending: //Real two-drain cursor.
        case PS::sharedCameraCallbackPending: //Real callback-count/Camera phase.
        case PS::sharedCameraObserverPending: //Actual observer construction cursor.
        case PS::sharedCameraPoseSet:         //Intentional source caller handoff.
        case PS::sharedPromptsLoaded:         //Before actual Text load has begun.
            return true;
        default:return false; //No replay of a partial Quad/Text/router constructor.
        }
    };
    std::uint32_t sourceCall=0x1CEC10;
    try {
        if(custody.managerDrivePhase_==P::notStarted){
            if(custody.pageAttempted_||custody.page_||custody.podScene_||custody.actorsAttempted_||
               !profile||!riderProfile||riderProfile->commonProfile()!=profile.get()||
               (custody.entryFonts_&&!sameCustodyShared(custody.entryFonts_,fonts)))
                return inputFailure(0x1CEC10,"Garage normal driver needs its real current profile and unstarted Page load");
            custody.managerDriveRequestedPage_=requestedPage; //Native caller argument, NOT source150.
            custody.managerDriveProfile_=profile; //Actual live Root profile, never a seeded copy.
            custody.entryFonts_=fonts; //Immutable native input pin, not a source Font constructor.
            custody.managerDrivePhase_=P::constructors;
        }else if(custody.managerDriveRequestedPage_!=requestedPage||
                 !sameCustodyShared(custody.entryFonts_,fonts)||
                 !sameCustodyShared(custody.managerDriveProfile_,profile)||
                 !riderProfile||riderProfile->commonProfile()!=profile.get())
            return inputFailure(0x1CEC10,"Garage normal driver cannot replace its retained source arguments/profile");
        if(custody.managerDrivePhase_==P::constructors){
            sourceCall=0x1CCD14;
            if(custody.managerConstructorPhase_!=CP::returned1CCDE4&&
               !continueGarageManagerConstructors1CCD14(frontend,custody.entryFonts_,diagnostic,error))
                return false; //Its real pending/failed prefix already remains owned.
            custody.managerDrivePhase_=P::pageMetadata;
        }
        if(custody.managerDrivePhase_==P::pageMetadata){
            sourceCall=0x1CEC10;
            //Read immutable metadata from the SAME retained actual manager
            //archive. The Page still opens/closes its own loader DAT later.
            auto metadata=std::make_shared<rr::assets::OriginalUiGaragePageData50KG>();
            custody.managerDrivePageData_=metadata;
            rr::assets::OriginalUiGaragePageDiagnostic50KG read;
            if(!metadata->load(custody.managerArchiveLease_,read)){
                diagnostic.status=Status::source;diagnostic.sourcePC=read.sourcePC;
                return stopped(0x1CEC10,"Garage authenticated Page metadata failed; manager prefix retained");
            }
            custody.managerDrivePhase_=P::sharedLoader;
            //The immutable SharedPageDraw projection is optional input and
            //unused by the genuine canonical Quad/Text constructor path. Do
            //not manufacture a detached draw owner to stand in for it.
            const bool returned=continueGaragePageCustodyCold({},metadata,{},diagnostic,error);
            if(!returned&&!sharedResumeSupported())
                return stopped(0x1FA2EC,"Garage Page/shared constructor stopped at a non-resumable source operation");
            if(returned){
                if(!custody.page_||custody.page_->stage_!=PS::sharedLoaderReturned)
                    return stopped(0x1D715C,"Garage Page helper did not return its actual shared loader");
                custody.managerDrivePhase_=P::sceneConstructor;
            }else if(custody.page_->stage_!=PS::sharedCameraPoseSet)return false;
            //PoseSet is the helper's intentional source handoff, not a missing
            //operation. Continue only that reached next call in this invocation.
        }
        if(custody.managerDrivePhase_==P::sharedLoader){
            sourceCall=0x1D63B0;
            if(!sharedResumeSupported())return stopped(sourceCall,"Garage shared loader has no supported retained continuation");
            Page::Diagnostic page;
            bool returned=custody.page_->beginSharedLoad1D63B0(page);
            if(!returned&&custody.page_->stage_==PS::sharedCameraPoseSet){
                //The prior continuation reached114F90. Its next source call
                //is1D682C; this does not repeat the Camera or either drain.
                returned=custody.page_->beginSharedLoad1D63B0(page);
            }
            if(!returned){
                diagnostic.status=Status::source;diagnostic.sourcePC=page.sourcePC;
                custodyFailure(error,"Garage shared loader stopped at its exact retained native owner boundary");
                if(!sharedResumeSupported())return stopped(sourceCall,"Garage shared loader partial constructor cannot replay");
                return false;
            }
            if(custody.page_->stage_!=PS::sharedLoaderReturned)
                return stopped(0x1D715C,"Garage shared loader has not actually returned");
            custody.managerDrivePhase_=P::sceneConstructor;
        }
        if(custody.managerDrivePhase_==P::sceneConstructor){
            sourceCall=0x1FA308;
            if(!continueGaragePageSceneCustodyCold(diagnostic,error))return stopped(sourceCall,"Garage actual Scene constructor stopped");
            custody.managerDrivePhase_=P::pageSpecific;
        }
        if(custody.managerDrivePhase_==P::pageSpecific){
            sourceCall=0x1FA318;
            if(!continueGaragePageSpecificCustodyCold(diagnostic,error))return stopped(sourceCall,"Garage Page-specific Text/Quad constructor stopped");
            custody.managerDrivePhase_=P::scenePrefix;
        }
        if(custody.managerDrivePhase_==P::scenePrefix){
            sourceCall=0x1FB0F0;
            if(!continueGarageScenePrefixCustodyCold(custody.managerDriveProfile_,diagnostic,error))
                return stopped(sourceCall,"Garage actual Scene Config/TOY/AUD prefix stopped");
            custody.managerDrivePhase_=P::scenePopulation;
        }
        if(custody.managerDrivePhase_==P::scenePopulation){
            sourceCall=0x1FB310;
            if(!continueGarageScenePopulationCustodyCold(diagnostic,error))
                return stopped(sourceCall,"Garage actual Scene population/load-return suffix stopped");
            custody.managerDrivePhase_=P::pageForest;
        }
        if(custody.managerDrivePhase_==P::pageForest){
            sourceCall=0x1FB5C0;
            if(!continueGaragePageForestCustodyCold(diagnostic,error))return stopped(sourceCall,"Garage actual initial Page forest/Camera handoff stopped");
            custody.managerDrivePhase_=P::lateForest;
        }
        if(custody.managerDrivePhase_==P::lateForest){
            sourceCall=0x1FB648;
            if(!continueGaragePageLateForestCustodyCold(diagnostic,error))return stopped(sourceCall,"Garage actual shared-dialog/late forest handoff stopped");
            custody.managerDrivePhase_=P::managerReturn;
        }
        if(custody.managerDrivePhase_==P::managerReturn){
            sourceCall=0x1FAA88;
            if(!continueGarageManagerLoadReturnCold(custody.managerDriveRequestedPage_,diagnostic,error))
                return stopped(sourceCall,"Garage actual Page return/activation/entry/manager suffix stopped");
            custody.managerDrivePhase_=P::returned;
        }
        if(custody.managerDrivePhase_!=P::returned||
           custody.managerLoadPhase_!=Custody::ManagerLoadPhase::published||!custody.managerPublished1B8274_||
           !custody.page_||!custody.page_->sourceEntryReturned1FAE00()||
           !custody.scope_->publishedPopulatedGarageManagerCurrent(custody.grant_->generation_)){
            // Only the actually completed loader/publication may retain this
            // native observation failure. Earlier source failures cannot mint it.
            custody.retainPublishedObservationFailure1B8274(*this);
            return stopped(0x1B8274,"Garage source manager publication has not actually completed");
        }
        //This is only the original LOAD return/publication. No coordinator
        //route capability is minted. Real frame/draw/event-port producers,
        //wet playback and Fire's small-carry caller argc/register remain
        //unadmitted; no timer/RNG/frame or particle branch is fabricated.
        diagnostic={};diagnostic.sourcePC=0x1B8274;error.clear();return true;
    }catch(...){diagnostic.status=Status::allocation;diagnostic.sourcePC=sourceCall;
        return stopped(sourceCall,"Garage normal driver allocation failed; reached source prefix retained");}
}
bool Application::prepareGarageManagerEntry1D1950(Frontend& frontend,
    std::shared_ptr<const rr::assets::OriginalUiGarageSceneAssets50KG> assets,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using Backing=rr::platform::OriginalUiGarageAudioBacking50KG;
    using Resources=rr::platform::OriginalUiGarageModelResources50KG;
    using Scope=rr::game::OriginalUiEntityScope50KG;
    using Phase=Custody::ManagerEntryPhase;
    using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    diagnostic={};
    auto fail=[&](Status status,std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=status;diagnostic.sourcePC=pc;return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return fail(Status::phase,0x1B823C,"Garage entry requires the actual Application creator thread");
    if(garageCustodyAttempted_||garageCustodyOwner_){
        if(garageCustodyOwner_&&garageCustodyOwner_->entryFrontend_==&frontend&&garageCustodyOwner_->failed_)
            return fail(garageCustodyOwner_->entryFailureStatus_,garageCustodyOwner_->entryFailureSourcePC_,
                "Garage entry failed prefix remains owned; transfer and constructors will not replay");
        return fail(Status::phase,0x1B823C,"Garage entry already has retained custody; no second manager is constructed");
    }
    //These are genuine retained native predecessors, not a context prepared
    //by a test aperture. The dispatcher and shared dialogs are never rebuilt
    //here. Missing boot/Aural/history or menu-transition authority stays shut.
    const auto& scope=sharedUiScopeOwner_;
    if(!garageAudioApplicationInputsCurrent()||!garageAudioBackingOwner_||
       garageAudioBackingOwner_->failed_||garageAudioBackingOwner_->stage_!=Backing::Stage::transportPrepared||
       frontend.materialApplicationOwner!=this||!assets||!assets->loaded()||!assets->selectedPod()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !materialContextOwner_||!materialContextOwner_->live()||materialContextOwner_->videoIdentity()!=videoOwner_.get()||
       !sameCustodyShared(materialContextOwner_->materialApplication(),materialApplication)||
       !scope||scope->issuer_!=this||!sameCustodyShared(scope->application_,entitiesOwner_)||
       !scope->live().okay()||!scope->dispatcherLive_||scope->managerLive_||scope->dispatcherCurrentManager30_||
       scope->globalWidgetLoadPhase_!=Scope::GlobalWidgetLoadPhase::absentReturned||
       scope->dispatcherGlobalCount34_||scope->dispatcherGlobalTable38_||
       !sameCustodyShared(scope->globalWidgetProvider_,standardMaterialDomainOwner_->archiveProvider3E1A70())||
       !state->sharedDialogs||!state->sharedDialogs->loadedCurrent()||state->sharedDialogs->scope_!=scope||
       !sourceLightManager426F60Owner_||!sourceLightManager426F60Owner_->sourceMutationAvailable()||
       sourceBuildUiResFiles344_!=0)
        return fail(Status::input,0x1B823C,"Garage entry needs actual shared dispatcher/dialogs/LightManager, native Frontend, material namespace and prepared audio");
    // Read the actual retained career and its exact authenticated model before
    // any mutation. Do not seed a profile or substitute another racer.
    rr::platform::OriginalUiRiderProfileOwner50KG::GarageProfileSnapshot selected;
    if(!riderProfile||riderProfile->commonProfile()!=profile.get()||
       !riderProfile->selectedGarageProfileSnapshot(selected)||
       !assets||assets->selectedCareer()!=static_cast<int>(selected.career.career100C0)||
       selected.career.stamp.mode34!=2)
        return fail(Status::input,0x1D1CD0,"Garage entry is bounded to the actual unchanged selected-career profile");
    try {
        auto next=std::shared_ptr<Custody>(new Custody(*this,std::move(assets)));
        garageCustodyOwner_=std::move(next);garageCustodyAttempted_=true;
        auto& custody=*garageCustodyOwner_;custody.published_=true;
        custody.entryFrontend_=&frontend;custody.scope_=scope;
        custody.entryManagerGeneration_=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&custody));
        auto retainedFailure=[&](Status status,std::uint32_t pc,const char* text)noexcept {
            custody.failed_=true;custody.entryFailureStatus_=status;custody.entryFailureSourcePC_=pc;
            return fail(status,pc,text);
        };
        //1B823C constructs/enrolls the real manager while DETACHED. Its
        //literal194 and epoch are produced by this existing Scope constructor.
        auto step=scope->prepareGarageManager(custody.entryManagerGeneration_);
        if(!step.okay())return retainedFailure(Status::source,step.sourcePC,"Garage actual detached manager constructor refused; prefix retained");
        //Base2A39FC precedes the derived1D1860 fields and all loader work.
        //15C/16C/170/17C are not the unrelated archive-only scope native flag.
        custody.managerRestart17C_=1;custody.managerLifecycle16C_=0;
        custody.managerTransition170_=0;custody.managerField15C_=0;custody.managerField174_=0;
        custody.managerUpdateFieldsConstructed_=true;
        custody.managerEntryPhase_=Phase::managerConstructed1D1860;
        step=scope->issueGarageConstruction(scope,custody.entryManagerGeneration_,custody.grant_);
        if(!step.okay())return retainedFailure(Status::source,step.sourcePC,"Garage exact constructor Grant could not be issued; manager retained");
        custody.managerEntryPhase_=Phase::grantIssued1D1950;
        //Native ledger backing, before the first selected-model enrollment.
        //This bounds ALL200 admitted constructor insertions, including rows
        //retired during construction; it is not a simultaneous-live count or
        //a change to any original model/search/particle capacity. Camera and
        //persistent shared dialogs use their own existing authority owners.
        constexpr auto statics=rr::assets::originalUiGarageStaticModelCount50KG;
        constexpr auto actors=rr::assets::originalUiGarageActorCount50KG;
        constexpr std::size_t garageConstructionLedgerBound=
            (2+statics+actors)+       //selected, Watto, static and Actor models
            (17+37+1+3)+             //text outers/leaves, Movement, Watto trio
            (1+2+3)+                 //manager Search, Lights and Fog
            (1+1+8+3)+               //Page, router, controls and groups
            (1+4+1+2+3+1)+           //Pod, Visibility, LightList, lights/fog/pool
            (1+1+1+1+1)+             //static, collision, Toy, animation, particles
            (actors+statics+7)+      //characters, collision queries, emitters
            (2+4)+                   //Watto/Scene sound groups and audio lists
            (actors+1);              //temporary Actor LoopSoundLists and Trigger
        static_assert(garageConstructionLedgerBound==200);
        step=custody.grant_->reservePopulation(garageConstructionLedgerBound);
        if(!step.okay())return retainedFailure(
            step.status==Scope::Status::allocation?Status::allocation:Status::phase,
            step.sourcePC,"Garage native child-ledger reservation failed before resource construction; manager retained");
        custody.resources_=std::shared_ptr<Resources>(new Resources(this,entitiesOwner_,
            standardMaterialDomainOwner_,materialContextOwner_,custody.entryManagerGeneration_));
        rr::platform::OriginalUiFrontendDiagnostic50KG transfer;
        if(!frontend.prepareGarageTransfers2542F0(custody.grant_,transfer))
            return retainedFailure(Status::source,transfer.sourcePC?transfer.sourcePC:0x1D1980,
                "Garage native transfer entry failed; exact Frontend drain/renewal and manager prefix retained");
        custody.managerEntryPhase_=Phase::transfersReturned2542F0;
        //Ordinary344=0: actual1D1D68 opens19E before the selected2403A0.
        //The authenticated archive was decoded earlier, but was not an active
        //132820 lookup bank. Retain exact custody before every fallible step.
        const auto selectedArchive=custody.sceneAssets_->selectedArchive();
        const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
        const auto resolver=standardMaterialDomainOwner_->resolver3E1A70();
        rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic archiveDiagnostic;
        rr::assets::OriginalStandardTextureDiagnostic50LA bank;
        //1B7648..1B7834 left every UIPODTEX "%s%02d_%d" image resident in
        //texture383400 for the whole UI lifetime; the native table has no
        //boot-resident rows. The SAME authenticated UIPODTEX bytes join THIS
        //selected namespace only when the selected PSG names one of them and
        //neither Garage archive carries it (of 23 careers, only 26E.PSG: 26_3).
        std::shared_ptr<const rr::assets::OriginalUiArchive50KG> residentPodTextures;
        std::vector<std::string> residentPodTextureNames;
        if(selectedArchive){
            const auto* pod=custody.sceneAssets_->selectedPod();
            const auto* scene=pod&&pod->model?pod->model->scene():nullptr;
            const auto garageArchive=custody.sceneAssets_->archive();
            if(scene&&garageArchive)for(const auto& name:scene->materials){
                if(name.empty()||name.size()>63)continue;
                const std::string base="ui\\textures\\"+name;
                if(selectedArchive->find(base+".psm").identity||garageArchive->find(base+".psm").identity||
                   selectedArchive->find(base+".pst").identity)continue;
                if(!residentPodTextures)residentPodTextures=readArchive(assetRoot,rr::assets::OriginalUiArchiveId50KG::podTextures);
                if(!residentPodTextures||!residentPodTextures->find(base+".pst").identity){residentPodTextures.reset();residentPodTextureNames.clear();break;}
                residentPodTextureNames.push_back(base);
            }
        }
        if(!selectedArchive||!provider||!resolver||
           !provider->mountAuthenticatedUi(selectedArchive,archiveDiagnostic)||
           (residentPodTextures&&!provider->mountAuthenticatedUi(residentPodTextures,archiveDiagnostic))||
           !provider->sealNativeMounts(archiveDiagnostic)||!resolver->bindProvider3E1A70(provider,bank)){
            diagnostic.texture=bank;
            return retainedFailure(Status::source,bank.sourcePC?bank.sourcePC:0x1D1D68,
                "Garage selected19E source bank opening failed; actual prefix retained");
        }
        custody.residentPodTextures_=residentPodTextures;
        custody.selectedBankLease_=std::make_unique<rr::assets::OriginalStandardTextureResolver50LA::GarageBankLease50LA>();
        if(!resolver->issueGarageSelectedBank1D1D68(provider,selectedArchive,*custody.selectedBankLease_,bank)){
            diagnostic.texture=bank;return retainedFailure(Status::source,bank.sourcePC?bank.sourcePC:0x1D1D68,
                "Garage selected19E native bank custody failed; actual open bank retained");
        }
        custody.selectedArchiveOnlyScope1D1D88_=true;
        custody.managerEntryPhase_=Phase::selectedBankOpened1D1D68;
        std::shared_ptr<const rr::assets::OriginalUiIpumTextureCache50KG> sourceImages;
        if(!frontend.coordinator.prepareGarageSourceImages12CEA0(custody,selectedArchive,sourceImages,diagnostic))
            return retainedFailure(diagnostic.status,diagnostic.sourcePC,"Garage selected source-image preparation failed; bank retained");
        if(!resolver->bindGarageStaticIpumCache24E880(sourceImages,selectedArchive,bank)){
            diagnostic.texture=bank;return retainedFailure(Status::texture,bank.sourcePC?bank.sourcePC:0x24E880,
                "Garage selected exact static IPUM source binding failed; bank retained");
        }
        if(!appendGarageBankSources(selectedArchive,standardMaterialDomainOwner_,diagnostic,nullptr,
               residentPodTextures?&residentPodTextureNames:nullptr))
            return retainedFailure(diagnostic.status,diagnostic.sourcePC,"Garage selected immutable source corpus failed; bank retained");
        if(!custody.prepareManagerSelected1D1CD0(diagnostic))
            return retainedFailure(diagnostic.status,diagnostic.sourcePC,"Garage selected model constructor failed after actual transfer entry; prefix retained");
        //1D1E90->2A4B30: same retained14D370 /29E340 /14D440 owners.
        //No second transfer renewal, GL upload, or guessed ready flag.
        if(!standardMaterialDomainOwner_->populate(materialContextOwner_,diagnostic))
            return retainedFailure(diagnostic.status,diagnostic.sourcePC,"Garage selected manager resource completion failed; actual partial resources retained");
        if(!standardMaterialDomainOwner_->materials382B38()->coldCollectorCurrent(standardMaterialDomainOwner_->coldCollector14D440()))
            return retainedFailure(Status::source,0x14D440,"Garage selected resource completion lacks its actual collector receipt; bank retained");
        custody.managerEntryPhase_=Phase::selectedPopulationReturned1D1E90;
        if(!resolver->retireGarageBank12E5E0(*custody.selectedBankLease_,bank)){
            diagnostic.texture=bank;return retainedFailure(Status::source,bank.sourcePC?bank.sourcePC:0x1D1EA4,
                "Garage selected source bank close failed after real resource completion; prefix retained");
        }
        // Native copied-source cleanup uses the real just-closed selected bank
        // receipt while it still exists. Any row ever borrowed by a Texture
        // remains under its actual destructor; decoded pixels are untouched.
        if(!resolver->reclaimNeverBorrowedSelectedPstAfterBankClose(*custody.selectedBankLease_,bank)){
            diagnostic.texture=bank;return retainedFailure(Status::source,bank.sourcePC?bank.sourcePC:0x1D1EA4,
                "Garage selected closed-bank unused source cleanup failed; exact closure receipt retained");
        }
        custody.selectedBankLease_.reset();
        custody.selectedArchiveOnlyScope1D1D88_=false; //Actual1D1EBC132A20(0).
        custody.managerEntryPhase_=Phase::selectedReturned1D1CD0;
        //1D19B4 calls the base manager loader only AFTER selected1D1CD0.
        //Reuse its real DAT/archive owner; this does not issue Page or Widgets.
        //1CCBE0 writes the retained Application22C before archive/DAT work.
        //Do not assume an earlier Main/Rider/CareerInfo route left it at1.
        sourceApplicationMode22C_=1;
        if(!continueGarageCustodyInputsCold(diagnostic,error)){
            custody.failed_=true;custody.entryFailureStatus_=diagnostic.status==Status::complete?Status::source:diagnostic.status;
            custody.entryFailureSourcePC_=diagnostic.sourcePC?diagnostic.sourcePC:0x1CCC84;
            diagnostic.status=custody.entryFailureStatus_;diagnostic.sourcePC=custody.entryFailureSourcePC_;return false;
        }
        custody.managerEntryPhase_=Phase::baseInputsReturned1CCC84;
        error.clear();return true;
    }catch(...){
        if(garageCustodyOwner_){garageCustodyOwner_->failed_=true;
            garageCustodyOwner_->entryFailureStatus_=Status::allocation;
            garageCustodyOwner_->entryFailureSourcePC_=0x1D1950;}
        return fail(Status::allocation,0x1D1950,"Garage native entry allocation failed; any actual manager/transfer prefix remains retained");
    }
}
bool Application::continueGarageManagerConstructors1CCD14(Frontend& frontend,
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> fonts,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using P=Custody::ManagerConstructorPhase;
    using Fog=rr::platform::OriginalUiGarageManagerFog50KG;
    using Lights=rr::platform::OriginalUiGarageUiLights50KG;
    using Widget=rr::platform::OriginalUiGarageWidget1Owner50KG;
    using WS=rr::platform::OriginalUiGarageWidget1Status50KG;
    using Watto=rr::platform::OriginalUiGarageWattoOwner50KG;
    using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    diagnostic={};const auto& owner=garageCustodyOwner_;
    auto fail=[&](Status status,std::uint32_t pc,const char* text,bool terminal=false)noexcept {
        diagnostic.status=status;diagnostic.sourcePC=pc;
        if(terminal&&owner){owner->failed_=true;owner->entryFailureStatus_=status;owner->entryFailureSourcePC_=pc;}
        return custodyFailure(error,text);
    };
    if(!owner||!owner->sameRoot(*this)||owner->entryFrontend_!=&frontend)
        return fail(Status::phase,0x1CCCD0,"Garage constructor continuation requires its exact retained Root and entry Frontend");
    if(owner->failed_)
        return fail(owner->entryFailureStatus_==Status::complete?Status::source:owner->entryFailureStatus_,
            owner->entryFailureSourcePC_?owner->entryFailureSourcePC_:0x1CCD14,
            "Garage constructor failed prefix remains owned; no source call is replayed");
    if(!garageAudioApplicationInputsCurrent()||!owner->constructionCurrent(*this)||
       owner->managerEntryPhase_!=Custody::ManagerEntryPhase::baseInputsReturned1CCC84||
       !sameCustodyShared(owner->scope_,sharedUiScopeOwner_)||owner->scope_->managerPublished_||
       owner->scope_->dispatcherCurrentManager30_||owner->scope_->manager_.parent1C||
       !owner->managerDat298_||!owner->managerArchiveOnlyScope170_||!owner->managerArchiveScopeCurrent()||
       owner->pageAttempted_||owner->podScene_||owner->actorsAttempted_||
       !frontend.garageTransferOwnerCurrent(owner->grant_))
        return fail(Status::phase,0x1CCCD0,"Garage constructor continuation lost its actual detached manager, DAT or native transfer owner");
    if(!fonts||!fonts->loaded()||!fonts->identity()||
       fonts->identity()->id!=rr::assets::OriginalUiArchiveId50KG::fonts)
        return fail(Status::input,0x2A9540,"Garage static Text requires the authenticated immutable Fonts archive");
    auto& custody=*owner;
    if(custody.managerConstructorPhase_!=P::notStarted&&
       (!sameCustodyShared(custody.entryFonts_,fonts)||
        custody.managerDispatcher2AC_!=&custody.scope_->dispatcher_||
        !custody.entryTiming_.sameOwner(custody.entryTiming_)))
        return fail(Status::phase,0x1CCCD0,"Garage constructor continuation cannot replace its dispatcher, font source or Frontend clock");
    std::uint32_t allocationPC=0x1CCCD0;
    try {
        if(custody.managerConstructorPhase_==P::notStarted){
            const auto display=raceDisplayOwner50LA_.lock();
            const auto transfers=frontend.garageTransferDisplay_.lock();
            if(!sameCustodyShared(display,transfers))
                return fail(Status::phase,0x1CCCEC,"Garage renderer no-op calls require the actual same live native Display");
            // Native lifetime binding, not a source clock sample. The real
            // Frontend prepare/commit path remains the only timing producer.
            if(!frontend.sourceDispatchTiming.retainPublication(custody.entryTiming_))
                return fail(Status::allocation,0x1CCCD0,"Garage could not retain its actual Frontend timing publication");
            custody.entryFonts_=std::move(fonts);
            custody.managerDispatcher2AC_=&custody.scope_->dispatcher_; //1CCCD0, no retain in source.
            // Source243A80 stores table3D2050. Its slot2C is245130, literally
            // jr ra/nop. Calls1CCCEC(0,13,0) and1CCD0C(0,14,0) have no observable
            // effects or consumed result. Do not invent native GL state13/14.
            custody.managerConstructorPhase_=P::rendererCallsReturned1CCD14;
        }
        if(custody.managerConstructorPhase_==P::rendererCallsReturned1CCD14){
            if(!custody.prepareManagerSearch1CCD14(diagnostic))
                return fail(diagnostic.status,diagnostic.sourcePC,"Garage actual Search constructor stopped; prefix retained",true);
        }
        if(custody.managerConstructorPhase_==P::searchReturned1CCD78){
            if(!sourceLightManager426F60Owner_)
                return fail(Status::input,0x1CCD88,"Garage requires the real pre-existing Application LightManager140074, not a replacement manager");
            allocationPC=0x1CDB10;custody.entryLights_=std::make_shared<Lights>();
            Lights::Diagnostic lights;
            //This owner already performs actual shared148B60(1) at1CCD88,
            //then both authored1CDB10 lights. Never reset it a second time.
            if(!custody.entryLights_->prepare1CDB10(custody.grant_,custody.entrySearch_,
                 sourceLightManager426F60Owner_,*custody.resources_,custody.sceneAssets_,lights))
                return fail(Status::source,lights.sourcePC,"Garage actual shared Light reset/load stopped; prefix retained",true);
            custody.managerConstructorPhase_=P::lightsReturned1CCD98;
        }
        if(custody.managerConstructorPhase_==P::lightsReturned1CCD98){
            allocationPC=0x1CCFA0;custody.entryFog_=std::make_shared<Fog>();Fog::Diagnostic fog;
            if(!custody.entryFog_->prepare1CCFA0(custody.grant_,custody.entryLights_,
                 retainGarageFogColor13E8A8(),fog)){
                if(custody.entryFog_->stage_==Fog::Stage::awaitingRenderer&&fog.status==Fog::Status::renderer){
                    custody.managerConstructorPhase_=P::fogAwaitingRenderer13E8A8;
                    return fail(Status::source,fog.sourcePC,"Garage Fog retains its exact renderer-write continuation");
                }
                return fail(Status::source,fog.sourcePC,"Garage actual Fog constructor stopped; prefix retained",true);
            }
            custody.managerConstructorPhase_=P::fogReturned1CCDA8;
        }
        if(custody.managerConstructorPhase_==P::fogAwaitingRenderer13E8A8){
            Fog::Diagnostic fog;
            if(!custody.entryFog_->continue1CCFA0(retainGarageFogColor13E8A8(),fog))
                return fail(Status::source,fog.sourcePC,"Garage Fog renderer continuation has not returned",
                    custody.entryFog_->stage_!=Fog::Stage::awaitingRenderer||fog.status!=Fog::Status::renderer);
            custody.managerConstructorPhase_=P::fogReturned1CCDA8;
        }
        if(custody.managerConstructorPhase_==P::fogReturned1CCDA8){
            // Garage table3CF5A0+B0 is1CCF30: jr ra / v0=0. The1CCDBC
            // caller ignores v0. DAT298 stays owned and no hidden load occurs.
            allocationPC=0x1CD7A0;custody.widgets_=std::make_shared<Widget>();
            custody.managerConstructorPhase_=P::widgetsStarted1CD7A0;
            rr::platform::OriginalUiGarageWidget1Diagnostic50KG widgets;
            if(!prepareGarageWidget1_1CD7A0(*custody.widgets_,custody.grant_,custody.entryFog_,widgets)&&
               widgets.status!=WS::selectedTextPending)
                return fail(Status::source,widgets.sourcePC,"Garage actual Widget table/first constructor stopped; prefix retained",true);
        }
        if(custody.managerConstructorPhase_==P::widgetsStarted1CD7A0){
            rr::platform::OriginalUiGarageWidget1Diagnostic50KG widgets;
            // Resumes the same real row0 if its selected text was pending,
            // then constructs rows1..8. No duplicate table or localization.
            if(!continueGarageWidgetsThrough9_1CD860(*custody.widgets_,custody.entryFonts_,widgets))
                return fail(Status::source,widgets.sourcePC,"Garage actual Widget prefix9 has not returned",
                    widgets.status!=WS::selectedTextPending);
            custody.managerConstructorPhase_=P::widgets9Returned;
        }
        if(custody.managerConstructorPhase_==P::widgets9Returned){
            if(!continueGarageWattoCustodyCold(custody.widgets_,diagnostic,error)){
                // The existing constructor can return before its *native*
                // silent routing service succeeds. Prove the real source
                // return with the actual retained subtree, then retry ONLY
                // native service. Never replay motion/wing/reverb enqueue.
                Watto::ColdPublication returned;
                if(!custody.failed_&&custody.watto_&&custody.watto_->constructorPublication(returned)){
                    custody.managerConstructorPhase_=P::wattoRoutingPending;
                    diagnostic.status=Status::source;diagnostic.sourcePC=0x1D46B8;return false;
                }
                return fail(diagnostic.status==Status::complete?Status::source:diagnostic.status,
                    diagnostic.sourcePC?diagnostic.sourcePC:0x1D4510,
                    "Garage Watto constructor stopped; its actual attempted prefix will not replay",true);
            }
            custody.managerConstructorPhase_=P::wattoReturned1D46B8;
        }
        if(custody.managerConstructorPhase_==P::wattoRoutingPending){
            Watto::ColdPublication returned;
            if(!custody.watto_||!custody.watto_->constructorPublication(returned))
                return fail(Status::source,0x1D46B8,"Garage lost its real Watto source return while native routing was pending",true);
            if(!custody.audio_->serviceColdGarageRouting(error)){
                diagnostic.status=Status::source;diagnostic.sourcePC=0x1D46B8;return false;
            }
            custody.managerConstructorPhase_=P::wattoReturned1D46B8;
        }
        if(custody.managerConstructorPhase_==P::wattoReturned1D46B8){
            rr::platform::OriginalUiGarageWidget1Diagnostic50KG widgets;
            if(!continueGarageWidgetsThrough22_1CD860(*custody.widgets_,custody.watto_,custody.entryFonts_,widgets))
                return fail(Status::source,widgets.sourcePC,"Garage actual Widget10..22 constructor loop has not returned",
                    widgets.status!=WS::selectedTextPending);
            custody.managerConstructorPhase_=P::widgetsReturned1CCDD4;
        }
        if(custody.managerConstructorPhase_==P::widgetsReturned1CCDD4){
            Fog::Diagnostic fog;
            if(!custody.entryFog_->attachAfterWidgets1CD080(*custody.widgets_,fog))
                return fail(Status::source,fog.sourcePC,"Garage post22 Fog0/On2/Off1 append stopped; real prefix retained",true);
            custody.managerConstructorPhase_=P::returned1CCDE4;
        }
        if(custody.managerConstructorPhase_!=P::returned1CCDE4||
           !custody.widgets_||!custody.widgets_->currentColdComplete()||!custody.entryFog_->currentAttached())
            return fail(Status::phase,0x1CCDE4,"Garage constructor prefix has not reached its actual post-Widget return");
        //1CCDF4 Page load is later. No Page, Scene, dispatcher publication,
        //menu admission, frame, fire-carry or wet playback claim is made here.
        diagnostic.sourcePC=0x1CCDE4;error.clear();return true;
    }catch(...){return fail(Status::allocation,allocationPC,"Garage native constructor allocation failed; all reached owners retained",true);}
}
bool Application::prepareGarageCustodyCold(
    std::shared_ptr<rr::game::OriginalUiEntityScope50KG::GarageConstructionGrant> grant,
    std::shared_ptr<rr::platform::OriginalUiGarageModelResources50KG> resources,
    std::shared_ptr<const rr::assets::OriginalUiGarageSceneAssets50KG> assets,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    const GarageColdStageTiming stageTiming{"prepareGarageCustodyCold",diagnostic};
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using Backing=rr::platform::OriginalUiGarageAudioBacking50KG;
    diagnostic={};
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return custodyFailure(error,"Garage cold custody requires actual Application creator thread");
    if(garageCustodyAttempted_||garageCustodyOwner_)
        return custodyFailure(error,"Garage cold custody is one retained attempt, never replayed");
    if(!garageAudioApplicationInputsCurrent()||!garageAudioBackingOwner_||
        garageAudioBackingOwner_->failed_||
        garageAudioBackingOwner_->stage_!=Backing::Stage::transportPrepared||
        !Custody::constructionInputsCurrent(*this,grant,resources,assets)||
        !sameCustodyShared(resources->context_,materialContextOwner_)||
        !sameCustodyShared(resources->domain_,standardMaterialDomainOwner_)||
        !materialContextOwner_->live()||materialContextOwner_->videoIdentity()!=videoOwner_.get()||
        !sameCustodyShared(materialContextOwner_->materialApplication(),materialApplication))
        return custodyFailure(error,"Garage cold custody requires real current enclosing grant/resources and root audio");
    // Manager widgets precede Page/Scene construction. Retain native backing
    // and authentic immutable data now, without any early counted Scene owner.
    try {
        auto next=std::shared_ptr<Custody>(new Custody(*this,std::move(grant),
            std::move(resources),std::move(assets),{}));
        garageCustodyOwner_=std::move(next);garageCustodyAttempted_=true;
        auto& custody=*garageCustodyOwner_;custody.published_=true;
        return continueGarageCustodyInputsCold(diagnostic,error);
    }catch(...){
        if(garageCustodyOwner_)garageCustodyOwner_->failed_=true;
        return custodyFailure(error,"Garage native custody allocation failed; any published prefix retained");
    }
}
bool Application::continueGarageCustodyInputsCold(
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    const GarageColdStageTiming stageTiming{"continueGarageCustodyInputsCold",diagnostic};
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    diagnostic={};const auto& owner=garageCustodyOwner_;
    if(!owner||!owner->sameRoot(*this)||owner->failed_||owner->retiring_||
       !Custody::constructionInputsCurrent(*this,owner->grant_,owner->resources_,owner->sceneAssets_)||
       owner->managerDat298_||owner->managerArchiveOnlyScope170_||owner->wattoAssets_||
       (owner->entryFrontend_&&owner->managerEntryPhase_!=Custody::ManagerEntryPhase::selectedReturned1D1CD0))
        return custodyFailure(error,"Garage base loader requires its exact retained, not-yet-read constructor prefix");
    auto& custody=*owner;
    try {
        if(!retainGarageAudioTransportCold(custody.audio_,error)){
            custody.failed_=true;return false;
        }
        if(!retainPendingWaves98A4(custody.pending_,error)){
            //This failed read creates no namespace and precedes Main's entry
            //state transition. Retain the actual successful audio alias only.
            custody.pendingRetainFailed98A4_=true;
            custody.failed_=true;return false;
        }
        // This exact new grant/capsule enters the retained source-construction
        // interval. Ordinary music/effects remain the same root-owned voices.
        if(!custody.audio_->beginRetainedGarageEntry(garageCustodyOwner_,error)){
            custody.failed_=true;return false;
        }
        // Native immutable assets may be decoded ahead of time; the active
        //manager DAT298 and archive-only load scope are retained here BEFORE
        //the actual Widget constructor loop, as in1CCC64..1CCDCC.
        if(sourceBuildUiResFiles344_!=0||!standardMaterialDomainOwner_){
            custody.failed_=true;return custodyFailure(error,"Garage resource-builder mode is not admitted");
        }
        custody.managerArchiveLease_=custody.sceneAssets_->archive();
        custody.managerDomainLease_=standardMaterialDomainOwner_;
        custody.managerProviderLease_=standardMaterialDomainOwner_->archiveProvider3E1A70();
        if(custody.entryFrontend_){
            //Actual later base1CCBA012CEA0; not a pre-mounted substitute for
            //the earlier selected bank. Detached component callers retain
            //their existing explicit namespace precondition unchanged.
            rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic mounted;
            rr::assets::OriginalStandardTextureDiagnostic50LA binding;
            if(!custody.managerProviderLease_||!custody.managerArchiveLease_||
               !custody.managerProviderLease_->mountAuthenticatedUi(custody.managerArchiveLease_,mounted)||
               !custody.managerProviderLease_->sealNativeMounts(mounted)||
               !custody.managerDomainLease_->resolver3E1A70()->bindProvider3E1A70(custody.managerProviderLease_,binding)){
                custody.failed_=true;diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
                diagnostic.texture=binding;diagnostic.sourcePC=binding.sourcePC?binding.sourcePC:0x12CEA0;
                return custodyFailure(error,"Garage actual base archive opening failed; source prefix retained");
            }
        }
        if(!custody.managerArchiveScopeCurrent()){
            custody.failed_=true;return custodyFailure(error,"Garage actual mounted archive namespace is unavailable");
        }
        // Retain an exact native retirement authority for this already-mounted
        // authenticated bank. Issuance neither unloads it nor manufactures a
        // guest bank handle; normal12E5E0 consumes this lease after all children.
        custody.managerBankLease_=std::make_unique<rr::assets::OriginalStandardTextureResolver50LA::GarageBankLease50LA>();
        rr::assets::OriginalStandardTextureDiagnostic50LA bank;
        if(!custody.managerDomainLease_->resolver3E1A70()->issueGarageBank12E5E0(
             custody.managerProviderLease_,custody.managerArchiveLease_,*custody.managerBankLease_,bank)){
            custody.failed_=true;diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
            diagnostic.sourcePC=bank.sourcePC?bank.sourcePC:0x12CEA0;
            return custodyFailure(error,"Garage exact retained bank ownership is unavailable; constructor prefix retained");
        }
        //Existing12CEA0 bank success still yields s0/2A8=1.132A20(1)
        //means170=0 (archive-only), not a newly allocated guest bank.
        custody.managerScopeEntryRevision_=custody.managerProviderLease_->revision();
        custody.managerArchiveOnlyScope170_=true;
        if(custody.entryFrontend_){
            std::shared_ptr<const rr::assets::OriginalUiIpumTextureCache50KG> sourceImages;
            if(!custody.entryFrontend_->coordinator.prepareGarageSourceImages12CEA0(custody,custody.managerArchiveLease_,sourceImages,diagnostic)){
                custody.managerBaseImagesFailed12CEA0_=true; // Exact reached failure, before24E880/DAT.
                custody.failed_=true;return custodyFailure(error,"Garage source-image input preparation failed; active base bank retained");
            }
            if(!custody.managerDomainLease_->resolver3E1A70()->bindGarageStaticIpumCache24E880(sourceImages,custody.managerArchiveLease_,bank)){
                custody.failed_=true;diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::texture;
                diagnostic.texture=bank;diagnostic.sourcePC=bank.sourcePC?bank.sourcePC:0x24E880;
                return custodyFailure(error,"Garage exact base static IPUM source binding failed; active bank retained");
            }
            bool domainAppendStarted=false;
            if(!appendGarageBankSources(custody.managerArchiveLease_,custody.managerDomainLease_,diagnostic,&domainAppendStarted)){
                if(!domainAppendStarted&&diagnostic.status!=rr::assets::OriginalStandardMaterialStatus50LA::complete&&diagnostic.sourcePC){
                    custody.managerBaseCorpusBeforeAppendFailed_=true;
                    custody.managerBaseCorpusFailureStatus_=diagnostic.status;
                    custody.managerBaseCorpusFailurePC_=diagnostic.sourcePC;
                }
                custody.failed_=true;return custodyFailure(error,"Garage immutable model-material source corpus failed; active bank retained");
            }
        }
        const auto ui=custody.managerArchiveLease_->find("UI/UIGarage/UI.DAT");
        custody.managerDat298_=std::make_unique<rr::assets::OriginalUiDat50KG>();
        rr::assets::OriginalUiPageDiagnostic50KG dat;
        if(!ui.identity||!rr::assets::readOriginalUiDat50KG(*custody.managerArchiveLease_,
             ui.identity->name,*custody.managerDat298_,dat)){
            custody.failed_=true;diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
            diagnostic.sourcePC=dat.sourcePC?dat.sourcePC:0x1CCC84;
            return custodyFailure(error,"Garage actual manager temporary DAT298 failed; prefix retained");
        }
        auto wattoAssets=std::make_shared<rr::assets::OriginalUiGarageWattoAssets50KG>();
        custody.wattoAssets_=wattoAssets;
        rr::assets::OriginalUiGarageWattoAssetDiagnostic50KG assetDiagnostic;
        if(!wattoAssets->load(custody.sceneAssets_->archive(),garageAudioBackingOwner_->metadata_,assetDiagnostic)){
            custody.failed_=true;diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
            diagnostic.sourcePC=assetDiagnostic.sourcePC;
            return custodyFailure(error,"Garage authenticated Watto assets failed; custody retained");
        }
        error.clear();return true;
    }catch(...){
        custody.failed_=true;diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::allocation;
        diagnostic.sourcePC=0x1CCC84;
        return custodyFailure(error,"Garage native base-loader allocation failed; actual prefix retained");
    }
}
bool Application::continueGarageWidgetsCustodyCold(
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> fonts,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    const GarageColdStageTiming stageTiming{"continueGarageWidgetsCustodyCold",diagnostic};
    diagnostic={};const auto& owner=garageCustodyOwner_;
    if(!garageAudioApplicationInputsCurrent()||!owner||!owner->constructionCurrent(*this)||
       !owner->widgets_||!owner->watto_||owner->actorsAttempted_||owner->podScene_)
        return custodyFailure(error,"Garage widget continuation requires this retained pre-Page custody");
    rr::platform::OriginalUiGarageWidget1Diagnostic50KG widgets;
    if(!continueGarageWidgetsThrough22_1CD860(*owner->widgets_,owner->watto_,std::move(fonts),widgets)){
        diagnostic.sourcePC=widgets.sourcePC;diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
        return custodyFailure(error,"Garage actual widget constructor/tail has not returned; no Page creation");
    }
    rr::platform::OriginalUiGarageManagerFog50KG::Diagnostic fog;
    if(!owner->widgets_->fog_||!owner->widgets_->fog_->attachAfterWidgets1CD080(*owner->widgets_,fog)){
        diagnostic.sourcePC=fog.sourcePC;diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
        return custodyFailure(error,"Garage post-widget fog attachment stopped; exact prefix retained");
    }
    error.clear();return true;
}
bool Application::continueGaragePageCustodyCold(
    std::shared_ptr<rr::platform::OriginalUiGarageManagerSearch50KG> managerSearch,
    std::shared_ptr<const rr::assets::OriginalUiGaragePageData50KG> metadata,
    std::shared_ptr<const rr::platform::OriginalUiGarageSharedPageDrawOwner50KG> sharedDraw,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    const GarageColdStageTiming stageTiming{"continueGaragePageCustodyCold",diagnostic};
    using Page=rr::platform::OriginalUiGaragePageOwner50KG;
    diagnostic={};const auto& owner=garageCustodyOwner_;
    if(!garageAudioApplicationInputsCurrent()||!owner||!owner->constructionCurrent(*this)||
       owner->pageAttempted_||owner->page_||owner->actorsAttempted_||owner->podScene_||
       !owner->widgets_||!owner->widgets_->currentColdComplete()||
       !owner->widgets_->fog_||!owner->widgets_->fog_->currentAttached())
        return custodyFailure(error,"Garage Page factory requires real completed widgets and post-widget Fog");
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    if(owner->entryFrontend_){
        if(owner->managerConstructorPhase_!=Custody::ManagerConstructorPhase::returned1CCDE4||
           !owner->entrySearch_||!sameCustodyShared(owner->entryFog_,owner->widgets_->fog_)||
           (managerSearch&&!sameCustodyShared(managerSearch,owner->entrySearch_)))
            return custodyFailure(error,"Garage Page requires this normal entry's actual completed manager constructor chain");
        // The production entry supplies its own Search. A null argument is
        // not a missing/empty list: it selects this exact retained owner.
        // Historical detached callers still supply their existing real owner.
        managerSearch=owner->entrySearch_;
    }
    auto& custody=*owner;custody.pageAttempted_=true;
    Page::Diagnostic page;
    try {
        if(!metadata||!metadata->loaded()||metadata->archive()!=custody.managerArchiveLease_||
           !metadata->uiDat()||!metadata->uiDat()->identity()||!custody.managerDat298_||
           custody.managerDat298_->identity()!=metadata->uiDat()->identity()||
           !custody.managerArchiveOnlyScope170_||!custody.managerArchiveScopeCurrent())
            return custodyFailure(error,"Garage Page table requires the actual still-open manager DAT298/scope");
        rr::assets::OriginalUiPageIndex50KG index;
        if(!rr::assets::readOriginalUiPageIndex50KG(*custody.managerDat298_,index,page.page)||
           index.pageFiles.size()!=1)
            return custodyFailure(error,"Garage manager requires its genuine one-page table");
        custody.managerPageCount148_=static_cast<std::int32_t>(index.pageFiles.size());
        custody.managerPageTable144_.reset(new Custody::Component*[index.pageFiles.size()]);
        custody.managerPage144Known_=true;
        const auto table=custody.scope_->bindGarageManagerPageTable144(
            custody.grant_->generation_,&custody,custody.managerPageTable144_.get(),custody.managerPageCount148_);
        if(!table.okay()){
            diagnostic.sourcePC=table.sourcePC;diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
            custody.failed_=true;return custodyFailure(error,"Garage manager table ownership failed");
        }
        custody.managerLoadPhase_=Custody::ManagerLoadPhase::tableAllocated;
        custody.page_=std::make_shared<Page>();
        const Page::RootInputs inputs(this,entities(),custody.grant_,custody.widgets_,custody.widgets_->fog_);
        if(!custody.page_->prepare1F9FF0(inputs,page)||
           !custody.page_->retainManagerSearch1D7B3C(std::move(managerSearch),page)||
           !custody.page_->retainSharedColdResources(std::move(metadata),std::move(sharedDraw),page)||
           !custody.page_->beginSharedLoad1D63B0(page)){
            diagnostic.sourcePC=page.sourcePC;
            diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
            //Retain actual constructor/loader effects. A metadata projection
            //cannot stand in for the reached shared loader or allow a Scene.
            return custodyFailure(error,"Garage reached Page/shared-loader boundary remains pending; Scene not constructed");
        }
        error.clear();return true;
    }catch(...){custody.failed_=true;return custodyFailure(error,"Garage Page native allocation failed; actual prefix retained");}
}
bool Application::continueGaragePageSceneCustodyCold(
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    const GarageColdStageTiming stageTiming{"continueGaragePageSceneCustodyCold",diagnostic};
    diagnostic={};const auto& owner=garageCustodyOwner_;
    if(!garageAudioApplicationInputsCurrent()||!owner||!owner->constructionCurrent(*this)||
       owner->pageSceneAttempted_||owner->podScene_||!owner->page_||
       !owner->page_->currentConstructor()||
       owner->page_->stage_!=rr::platform::OriginalUiGaragePageOwner50KG::Stage::sharedLoaderReturned)
        return custodyFailure(error,"Scene4C0 requires actual Page shared-loader return");
    auto& custody=*owner;custody.pageSceneAttempted_=true;
    auto* droids=custody.audio_->garageDroidAudioCold();
    rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
    if(!droids||!custody.page_->constructScene1FA308(custody.sceneAssets_,
            custody.audio_,droids,custody.pending_,page)){
        diagnostic.sourcePC=page.sourcePC;
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
        custody.failed_=true;return custodyFailure(error,"Page-owned Scene constructor stopped; prefix retained");
    }
    custody.podScene_=custody.page_->borrowScene4C0();
    if(!custody.podScene_){custody.failed_=true;return custodyFailure(error,"Page did not publish its genuine Scene4C0");}
    // Root alone issues this native lifetime identity. The owning control
    // block is exactly page_, whose unique sceneBacking_ is podScene_.
    // Do not increment source Entity/Sound refs or create a new control block.
    const std::weak_ptr<rr::platform::OriginalUiGaragePodScene50KG> unissued;
    auto& lifetime=custody.podScene_->nativeRoutingLifetime_;
    if(custody.page_->sceneBacking_.get()!=custody.podScene_||
       custody.page_->scene4C0_!=custody.podScene_||
       lifetime.owner_before(unissued)||unissued.owner_before(lifetime)){
        custody.failed_=true;return custodyFailure(error,"Page/Scene native routing lifetime was replaced or already issued");
    }
    lifetime=std::shared_ptr<rr::platform::OriginalUiGaragePodScene50KG>(custody.page_,custody.podScene_);
    //Cost Text3340 and Page-specific quads still precede1FB0F0. This is
    //constructor custody only, not permission to populate Actors or statics.
    error.clear();return true;
}
bool Application::continueGaragePageSpecificCustodyCold(
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    const GarageColdStageTiming stageTiming{"continueGaragePageSpecificCustodyCold",diagnostic};
    diagnostic={};const auto& owner=garageCustodyOwner_;
    if(!garageAudioApplicationInputsCurrent()||!owner||!owner->constructionCurrent(*this)||
       !owner->pageSceneAttempted_||!owner->podScene_||!owner->page_||
       owner->actorsAttempted_||!owner->page_->currentConstructor())
        return custodyFailure(error,"Page-specific loader requires its retained real Scene constructor prefix");
    rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
    if(!owner->page_->continuePageSpecific1FA318(page)){
        diagnostic.sourcePC=page.sourcePC;
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
        return custodyFailure(error,"Page-specific Widget10/Text/quads stopped; exact prefix retained");
    }
    error.clear();return true;
}
bool Application::continueGarageScenePrefixCustodyCold(
    std::shared_ptr<const rr::platform::OriginalUiMainProfile50KG> profile,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    const GarageColdStageTiming stageTiming{"continueGarageScenePrefixCustodyCold",diagnostic};
    using Actors=rr::platform::OriginalUiGarageActors50GN;
    diagnostic={};const auto& owner=garageCustodyOwner_;
    if(!profile||!garageAudioApplicationInputsCurrent()||!owner||!owner->constructionCurrent(*this)||
       owner->actorsAttempted_||!owner->podScene_||!owner->page_||!owner->page_->currentConstructor()||
       !owner->widgets_||!owner->widgets_->currentColdComplete()||
       !owner->widgets_->fog_||!owner->widgets_->fog_->currentAttached())
        return custodyFailure(error,"Scene prefix requires actual retained profile, Page and completed widgets/Fog");
    rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
    if(!owner->page_->preflightSceneLoad1FB0F0(page)){
        diagnostic.sourcePC=page.sourcePC;
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
        return custodyFailure(error,"Actual Page Text/quads have not reached Scene load1FB0F0");
    }
    auto& custody=*owner;custody.actorsAttempted_=true;
    auto& scene=*custody.podScene_;
    try {
        // The actual mutable Config owners, closes and dispatch branches are
        // performed in source order. Immutable assets only authenticate bytes.
        if(!scene.captureModelDirectory1FB178(diagnostic)){
            custody.failed_=true;return custodyFailure(error,"Actual Scene namespace/local capture stopped before load");
        }
        materialDirectory36B970_="ui\\textures\\"; //1FB1F0/1F4/1F8, before slot30.
        if(!scene.beginLoad1B2AB0(custody.application_,std::move(profile),custody.resources_,diagnostic)||
           !scene.loadSceneConfig1B3880(diagnostic)||!scene.readScenePrefix270900(diagnostic)||
           !scene.prepareStaticDescriptors2710F0(custody.resources_,diagnostic)||
           !scene.finishSceneRead270900(diagnostic)||!scene.closeSceneConfig271C10(diagnostic)||
           !scene.loadToyConfig1B4330(diagnostic)||!scene.prepareToyDispatch271CF0Prefix(diagnostic)){
            custody.failed_=true;return custodyFailure(error,"Garage actual Scene/TOY prefix stopped; retained exact phase");
        }
        //1FB280 constructs only the real TOY list and copied actor rows here.
        //AnimationManager and Actor/Sound constructors occur much later.
        custody.actors_=std::make_unique<Actors>();
        if(!custody.actors_->prepareToyList2713F8(scene,custody.grant_,*custody.resources_,custody.sceneAssets_,diagnostic)||
           !scene.finishToyDispatch271CF0(diagnostic)||!scene.closeToyConfig272800(diagnostic)||
           !scene.loadAudioConfig1B4720(diagnostic)||!scene.readAudioDispatch2728E0(diagnostic)||
           !scene.closeAudioConfig272930(diagnostic)){
            custody.failed_=true;return custodyFailure(error,"Garage TOY/AUD dispatch stopped; no later phase skipped");
        }
        custody.visibility_=std::make_shared<rr::platform::OriginalUiGarageVisibility50KG>();
        if(!scene.prepareVisibility2729E0(custody.visibility_,diagnostic)){
            custody.failed_=true;return custodyFailure(error,"Garage distinct Visibility construction failed; prefix retained");
        }
        // This is an explicit prefix return, NOT loader completion. Real
        //Lights/Fog/ParticleManager/Instances precede statics/collisions/actors;
        //their successor joins must execute before any flush or page admission.
        error.clear();return true;
    }catch(...){custody.failed_=true;return custodyFailure(error,"Garage Scene prefix allocation failed; retained");}
}
bool Application::continueGarageScenePopulationCustodyCold(
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    const GarageColdStageTiming stageTiming{"continueGarageScenePopulationCustodyCold",diagnostic};
    using Actors=rr::platform::OriginalUiGarageActors50GN;
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using Scene=rr::platform::OriginalUiGaragePodScene50KG;
    diagnostic={};const auto& owner=garageCustodyOwner_;
    if(!garageAudioApplicationInputsCurrent()||!owner||!owner->constructionCurrent(*this)||
       !owner->actorsAttempted_||owner->actorsFinished_||!owner->actors_||!owner->podScene_||
       !owner->widgets_||!owner->widgets_->fog_||!owner->widgets_->fog_->lights_||
       owner->podScene_->populationStage_!=Scene::PopulationStage::visibility)
        return custodyFailure(error,"Scene population requires its exact retained visibility predecessor");
    auto& custody=*owner;auto& scene=*custody.podScene_;
    // Native preflight before later source population mutates its prefix.
    // Source timing remains the actual retained Frontend publication; pause
    // and expected clock101A50 come from the real Application in RootInputs.
    if(custody.entryFrontend_&&
       (custody.managerConstructorPhase_!=Custody::ManagerConstructorPhase::returned1CCDE4||
        !custody.entryTiming_.sameOwner(custody.entryTiming_))){
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::phase;diagnostic.sourcePC=0x1FB3E4;
        return custodyFailure(error,"Scene actors lack this entry's live Frontend timing publication");
    }
    const auto& fog=*custody.widgets_->fog_;
    try {
        //1FB310 supplies0x457A0000 (4000), not the unrelated particle capacity5000.
        //1FB328 divides that actual far distance by3. No new fog owner is made.
        if(!scene.populateLights273AA0(fog.lights_->manager_,diagnostic)||
           !scene.prepareFog273C30(4000.0f,4000.0f/3.0f,fog.renderer_,diagnostic)||
           !scene.prepareParticleManager1B4DC0(diagnostic)){
            custody.failed_=true;return custodyFailure(error,"Scene light/fog/particle source phase stopped; exact owners retained");
        }
        //Actual shared Root namespace writes, not private model path substitutes.
        modelDirectory36B860_="ui\\geometry\\"; //1FB370/374/378, before Instances/statics.
        if(!scene.populateInstances274080(diagnostic)||!scene.populateStaticModels274160(diagnostic)||
           !scene.populateCollisionModels2743B0(diagnostic)){
            custody.failed_=true;return custodyFailure(error,"Scene Instance/static/collision population stopped; exact prefix retained");
        }
        modelDirectory36B860_="ui\\anim\\"; //1FB3D0/3D4, before Actor slot120 at1FB3E4.
        // Only the genuine normal entry binds its real Frontend publication.
        // No prepared sample is manufactured here. Legacy cold seams retain
        // their deliberately empty timing view and cannot issue RuntimeFrame.
        custody.actorInputs_=std::shared_ptr<const Actors::RootInputs>(new Actors::RootInputs(
            this,custody.grant_,state,motionLibraryOwner_,custody.audio_,animationClock(),
            custody.entryFrontend_?custody.entryTiming_:original_ui_source_timing50LH::PublicationView{}));
        if(!custody.actors_->prepareAnimationManager2742F0(custody.actorInputs_,custody.visibility_,diagnostic)){
            custody.failed_=true;return custodyFailure(error,"Scene actual animation manager preparation stopped");
        }
        for(std::size_t ordinal=0;ordinal<rr::assets::originalUiGarageActorCount50KG;++ordinal){
            if(!custody.actors_->constructNext2D9060(diagnostic)){
                custody.failed_=true;return custodyFailure(error,"Scene actual Actor registration stopped; prior rows retained");
            }
        }
        if(!scene.finishActorPopulation2742F0(diagnostic)){
            custody.failed_=true;return custodyFailure(error,"Actual PodScene camdroid scan1B40B0 stopped");
        }
        custody.actorsFinished_=true;
        if(!scene.populateEmitters274370(diagnostic)){
            custody.failed_=true;return custodyFailure(error,"Actual seven-row Scene emitter load stopped; prior owners retained");
        }
        if(!scene.populateDynamicObjects274E90(diagnostic)||
           !scene.populateAmbientAudio275140(diagnostic)||!scene.populateSoultreeAudio275180(diagnostic)||
           !scene.populateWater274570(diagnostic)||!scene.populateWaterRenderer2747C0(diagnostic)||
           !scene.dispatchEcosystem1FB47C(diagnostic)||
           !scene.finishSceneLoad1FB594(materialDirectory36B970_,diagnostic)){
            custody.failed_=true;return custodyFailure(error,"Actual Scene source-return suffix stopped; reached owners retained");
        }
        //The final source235520 call enqueues reverb; it is NOT a sample
        //flush, wet-playback capability or Page/public menu completion.
        error.clear();return true;
    }catch(...){custody.failed_=true;return custodyFailure(error,"Scene native allocation stopped; exact prefix retained");}
}
bool Application::continueGaragePageForestCustodyCold(
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    const GarageColdStageTiming stageTiming{"continueGaragePageForestCustodyCold",diagnostic};
    diagnostic={};const auto& owner=garageCustodyOwner_;
    if(!garageAudioApplicationInputsCurrent()||!owner||!owner->constructionCurrent(*this)||
       !owner->actorsFinished_||!owner->page_||!owner->podScene_||
       !owner->podScene_->sourceLoadReturned1FB59C())
        return custodyFailure(error,"Page forest requires the genuine completed Scene load return");
    rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
    if(!owner->page_->continueSceneAttachments1FB5C0(page)||
       !owner->page_->continueCameraHandoff1FB5E4(page)){
        diagnostic.sourcePC=page.sourcePC;
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
        owner->failed_=true;return custodyFailure(error,"Actual Page Scene/Camera handoff stopped; exact prefix retained");
    }
    //Later1B4CA0 children, Page scene callback and runtime effects are not
    //implicitly completed by publishing this exact initial forest prefix.
    error.clear();return true;
}
bool Application::rewriteGarageCameraSlots1401B0(
    const rr::platform::OriginalUiGaragePageOwner50KG& page)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||page.issuer_!=this||
       !page.grant_||page.grant_->issuer()!=this||!page.grant_->current()||
       !sameCustodyShared(page.grant_->application(),entities())||
       !page.entryCameraActivationReturned115724()||!page.sharedCamera_||
       page.shared_.camera4BC!=page.sharedCamera_.get())return false;
    // Actual Application virtual44 at1FADC0 receives the SAME Camera4BC,
    // then three nulls. Reuse the existing13FC80/1401B0 process authority;
    // no duplicate Root array, renderer selection substitute, or default.
    return entities()->rewriteCameras1401B0({&page.sharedCamera_->entity_,nullptr,nullptr,nullptr});
}
bool Application::observeGarageLoaderApplication344(
    const rr::platform::OriginalUiGaragePageOwner50KG& page,std::int32_t& out)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||page.issuer_!=this||
       !page.grant_||page.grant_->issuer()!=this||
       !sameCustodyShared(page.grant_->application(),entities())||!page.currentConstructor())return false;
    // Borrow this process owner's constructor-defined normal startup state.
    // No read is certified for a future BuildUIResFiles producer or reload.
    out=sourceBuildUiResFiles344_;return true;
}
bool Application::continueGaragePageLateForestCustodyCold(
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    const GarageColdStageTiming stageTiming{"continueGaragePageLateForestCustodyCold",diagnostic};
    diagnostic={};const auto& owner=garageCustodyOwner_;
    if(!garageAudioApplicationInputsCurrent()||!owner||!owner->constructionCurrent(*this)||
       !owner->actorsFinished_||!owner->page_||!owner->podScene_||!state||!state->sharedDialogs||
       !owner->podScene_->sourceLoadReturned1FB59C())
        return custodyFailure(error,"Late Page forest requires the actual shared dialogs and retained Scene");
    rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
    if(!owner->page_->continueLateSceneAttachments1FB648(*state->sharedDialogs,page)){
        diagnostic.sourcePC=page.sourcePC;
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
        owner->failed_=true;return custodyFailure(error,"Actual shared-dialog/late Page child join stopped; exact prefix retained");
    }
    //Source attachments alone are not Page callbacks, applied wet effects,
    //draw readiness or normal Garage admission.
    error.clear();return true;
}
bool Application::continueGarageManagerLoadReturnCold(std::int32_t requestedPage,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    const GarageColdStageTiming stageTiming{"continueGarageManagerLoadReturnCold",diagnostic};
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using Phase=Custody::ManagerLoadPhase;
    using Pending=rr::platform::OriginalAudioPendingWaves50KG;
    diagnostic={};const auto& owner=garageCustodyOwner_;
    if(!garageAudioApplicationInputsCurrent()||!owner||!owner->constructionCurrent(*this)||
       !owner->actorsFinished_||!owner->page_||!owner->podScene_||!state||!state->sharedDialogs||
       !owner->podScene_->sourceLoadReturned1FB59C()||requestedPage!=1||
       owner->managerPageCount148_!=1||!owner->managerPage144Known_||!owner->managerPageTable144_||
       owner->managerLoadPhase_==Phase::notStarted||owner->managerLoadPhase_==Phase::failed||
       (owner->manager150Known_&&owner->managerSelected150_!=requestedPage))
        return custodyFailure(error,"Manager return requires actual Page/Scene, table and exact requested Page1");
    if(owner->managerPublished1B8274_){
        if(owner->managerLoadPhase_!=Phase::published||!owner->page_->sourceEntryReturned1FAE00()||
           !owner->scope_->publishedPopulatedGarageManagerCurrent(owner->grant_->generation_))
            return custodyFailure(error,"Published Garage manager ownership changed");
        error.clear();return true;
    }
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        if(!diagnostic.sourcePC)diagnostic.sourcePC=pc;
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
        owner->failed_=true;owner->managerLoadPhase_=Phase::failed;
        return custodyFailure(error,text);
    };
    auto pageFailure=[&](const rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic& page,
        const char* text)noexcept {
        diagnostic=page.loadResources;diagnostic.sourcePC=page.sourcePC;
        return fail(page.sourcePC,text);
    };
    rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
    if(owner->managerLoadPhase_==Phase::tableAllocated){
        const GarageColdStageTiming phaseTiming{"managerReturn.tableAllocated",diagnostic};
        if(!owner->managerArchiveOnlyScope170_||!owner->managerArchiveScopeCurrent()||!owner->managerDat298_)
            return fail(0x1CEC10,"Garage manager archive-only scope changed");
        if(!owner->page_->sourceLoadReturned1FAAC8()&&
           !owner->page_->continueLoadReturn1FAA88(*state->sharedDialogs,page))
            return pageFailure(page,"Actual Garage Page load return stopped; prefix retained");
        if(!owner->page_->managerAttached1D1C40_&&!owner->page_->attachToManager1D1C40(page))
            return pageFailure(page,"Actual loaded Garage Page attachment failed");
        //The real one-page table is written BEFORE the source virtual1C hide.
        owner->managerPageTable144_[0]=&owner->page_->entity_; //1CECE0
        owner->managerPage0Stored_=true;
        const auto hidden=owner->grant_->hideGaragePageForest1415A0(*owner->page_,owner->page_->entity_);
        if(!hidden.okay()){
            // Exact native exception receipt: the complete visibility preflight
            // returned before its recursive flag mutation. This is not entry.
            if(hidden.status==rr::game::OriginalUiEntityScope50KG::Status::topology&&
               hidden.sourcePC==0x1415A0&&!owner->page_->failedLoadedPageHide1415A0_){
                owner->page_->failedLoadedPageHide1415A0_=true;
                owner->page_->failedLoadedPageHideGeneration_=owner->entryManagerGeneration_;
                owner->page_->failedLoadedPageHideEpoch_=owner->grant_->epoch_;
            }
            return fail(hidden.sourcePC,"Garage loaded Page recursive hide failed");
        }
        owner->managerLoadPhase_=Phase::pageHidden;
    }
    if(owner->managerLoadPhase_==Phase::pageHidden){
        const GarageColdStageTiming phaseTiming{"managerReturn.pageHidden",diagnostic};
        //1CDA70 sees the original1D1890 null190. Actual Widget constructors
        //read every OnTop value and refuse positives, so no deferred row is skipped.
        if(!owner->managerDeferred190Empty_||!owner->widgets_->currentColdComplete())
            return fail(0x1CDA70,"Garage deferred widget list is not the supported genuine empty list");
        owner->managerLoadPhase_=Phase::deferredReturned;
    }
    if(owner->managerLoadPhase_==Phase::deferredReturned){
        const GarageColdStageTiming phaseTiming{"managerReturn.deferredReturned",diagnostic};
        if(!sourceTextureCallback228Bit5_||!textureLifetimeOwner_||!materialContextOwner_||
           !materialContextOwner_->live()||materialContextOwner_->materialApplication()!=materialApplication||
           !materialContextOwner_->applicationAnimationClock().sameOwner(animationClock()))
            return fail(0x1CCE40,"Actual Application texture-manager callback owner is unavailable");
        //Genuine bit5 is true. The exact initialized owner's255800 returns1
        //without effects; do not substitute a GPU flush or omit the reached call.
        (void)textureLifetimeOwner_->loadedResources255800();
        owner->managerLoadPhase_=Phase::textureCallbackReturned;
    }
    if(owner->managerLoadPhase_==Phase::textureCallbackReturned){
        const GarageColdStageTiming phaseTiming{"managerReturn.textureCallbackReturned",diagnostic};
        if(!owner->managerDat298_)return fail(0x1CCE4C,"Garage temporary DAT298 owner is missing");
        owner->managerDat298_.reset(); //1573B0 then source298=null at1CCE54.
        owner->managerLoadPhase_=Phase::datReleased;
    }
    if(owner->managerLoadPhase_==Phase::datReleased){
        const GarageColdStageTiming phaseTiming{"managerReturn.datReleased",diagnostic};
        if(!owner->managerPage0Stored_||owner->managerPageTable144_[requestedPage-1]!=&owner->page_->entity_)
            return fail(0x2A3C30,"Garage selected Page table identity changed");
        owner->managerSelected150_=requestedPage;owner->manager150Known_=true; //2A3C1C
        owner->managerCurrentPage14C_=owner->managerPageTable144_[requestedPage-1]; //2A3C34
        owner->managerLoadPhase_=Phase::pageSelected;
    }
    if(owner->managerLoadPhase_==Phase::pageSelected){
        const GarageColdStageTiming phaseTiming{"managerReturn.pageSelected",diagnostic};
        if(!owner->page_->continueActivation1D7390(page))
            return pageFailure(page,"Garage actual Page90 activation stopped; prefix retained");
        owner->managerLoadPhase_=Phase::pageActivated;
    }
    if(owner->managerLoadPhase_==Phase::pageActivated){
        const GarageColdStageTiming phaseTiming{"managerReturn.pageActivated",diagnostic};
        if(!owner->page_->continueEntry1FAAF0(page))
            return pageFailure(page,"Garage actual Page74 entry stopped; prefix retained");
        owner->managerLoadPhase_=Phase::pageEntered;
    }
    if(owner->managerLoadPhase_==Phase::pageEntered){
        const GarageColdStageTiming phaseTiming{"managerReturn.pageEntered",diagnostic};
        if(!owner->page_->sourceEntryReturned1FAE00()||!owner->managerArchiveScopeCurrent()||
           !standardMaterialDomainOwner_||!materialContextOwner_||!materialContextOwner_->live())
            return fail(0x1CCE74,"Garage second manager population lacks real completed entry/context");
        //Distinct original1CCE74→2A4B30, AFTER Page entry and before returning.
        //The earlier Page1FAAC0 population is historical, never a replacement.
        if(!standardMaterialDomainOwner_->populate(materialContextOwner_,diagnostic))
            return fail(0x1CCE74,"Garage second canonical material/texture population failed");
        const auto collector=standardMaterialDomainOwner_->coldCollector14D440();
        if(!standardMaterialDomainOwner_->materials382B38()->coldCollectorCurrent(collector))
            return fail(0x2A4B54,"Garage second canonical collector is not current");
        owner->managerLoadPhase_=Phase::resourcesReturned;
    }
    if(owner->managerLoadPhase_==Phase::resourcesReturned){
        const GarageColdStageTiming phaseTiming{"managerReturn.resourcesReturned",diagnostic};
        if(!owner->managerArchiveOnlyScope170_||!owner->managerArchiveScopeCurrent())
            return fail(0x1CCE94,"Garage resource scope changed before its source exit");
        //Existing-bank success gave s0=1, t0=0.132A20(0) exits archive-only
        //scope. Native backing stays retained until actual bank retirement;
        //this does NOT claim an implemented post-scope filesystem fallback.
        owner->managerArchiveOnlyScope170_=false;
        owner->managerLoadPhase_=Phase::scopeReturned;
    }
    if(owner->managerLoadPhase_==Phase::scopeReturned){
        const GarageColdStageTiming phaseTiming{"managerReturn.scopeReturned",diagnostic};
        const Pending::RootOperation operation(*this,entities(),Pending::RootOperation::Kind::nativeSceneResolve1D1B9C);
        if(!owner->pending_->flushNativeScene26D9F0(operation,*owner->podScene_,error)){
            // Preserve the already-produced nested refusal before the public
            // manager-stage diagnostic replaces it. No partial owner reads.
            std::fprintf(stderr,"original_garage_pending_sound_resolution_failed detail=%s\n",error.c_str());
            std::fflush(stderr);
            return fail(0x1D1B9C,"Garage final genuine pending-Sound resolution failed; prefix retained");
        }
        owner->managerLoadPhase_=Phase::soundReturned;
    }
    if(owner->managerLoadPhase_!=Phase::soundReturned||!owner->page_->sourceEntryReturned1FAE00())
        return fail(0x1D1BBC,"Garage whole manager loader has not returned");
    //Only now1B8268 appends the actual populated forest;1B8274 stores30.
    const auto published=owner->scope_->publishPopulatedGarageManager1B8268(owner->grant_->generation_);
    if(!published.okay())return fail(published.sourcePC,"Populated Garage manager publication failed");
    owner->managerPublished1B8274_=true;owner->managerLoadPhase_=Phase::published;
    //Not normal menu admission, frame rendering, or wet-DSP playback readiness.
    error.clear();return true;
}
bool Application::continueGarageWattoCustodyCold(
    std::shared_ptr<rr::platform::OriginalUiGarageWidget1Owner50KG> widgets,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    const GarageColdStageTiming stageTiming{"continueGarageWattoCustodyCold",diagnostic};
    using Watto=rr::platform::OriginalUiGarageWattoOwner50KG;
    diagnostic={};
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return custodyFailure(error,"Garage Watto custody requires actual Application creator thread");
    const auto& owner=garageCustodyOwner_;
    if(!garageAudioApplicationInputsCurrent()||!owner||!owner->constructionCurrent(*this)||
        owner->actorsAttempted_||owner->podScene_||owner->wattoAttempted_||
        !sameCustodyShared(owner->resources_->context_,materialContextOwner_)||
        !sameCustodyShared(owner->resources_->domain_,standardMaterialDomainOwner_)||
        !materialContextOwner_->live()||materialContextOwner_->videoIdentity()!=videoOwner_.get()||
        !sameCustodyShared(materialContextOwner_->materialApplication(),materialApplication)||
        !owner->widgetPredecessorCurrent(widgets))
        return custodyFailure(error,"Garage Watto factory requires actual same-loop Widget1 prefix9 before Page/Scene creation");
    std::shared_ptr<rr::platform::OriginalUiMainAudio50KG> actual;
    if(!retainGarageAudioTransportCold(actual,error)||!sameCustodyShared(actual,owner->audio_))
        return custodyFailure(error,"Garage Watto factory requires this root's exact retained audio alias");
    auto& custody=*owner;
    // Pin the real preceding rows and their Fog/Lights/Search/selected chain
    // BEFORE allocating RootInputs, owner, or any counted widget10 prefix.
    custody.widgets_=std::move(widgets);custody.wattoAttempted_=true;
    try {
        custody.wattoInputs_=std::shared_ptr<const Watto::RootInputs>(new Watto::RootInputs(
            this,custody.grant_,state,motionLibraryOwner_,custody.audio_,custody.entryTiming_));
        custody.watto_=std::make_shared<Watto>();
        const bool made=custody.watto_->prepare1D4510(custody.grant_,*custody.resources_,
                custody.sceneAssets_,custody.wattoAssets_,custody.wattoInputs_,diagnostic);
        if(!made&&diagnostic.sourcePC!=0x26DAD0){
            custody.failed_=true;return custodyFailure(error,"Garage Watto constructor stopped; prefix retained");
        }
        if(!custody.watto_->continueWingRegistration1D464C(custody.pending_,diagnostic)&&
           !custody.watto_->pendingFlush232EC0(custody.pending_)){
            custody.failed_=true;return custodyFailure(error,"Garage Watto registration/emitter continuation stopped; prefix retained");
        }
        if(!custody.watto_->pendingFlush232EC0(custody.pending_)){
            custody.failed_=true;return custodyFailure(error,"Garage Watto did not reach its exact source flush");
        }
        // Lazily prepare actual native cache storage at the first real sample
        // resolution, never use a per-widget namespace or a guessed sample ID.
        if(!custody.pending_->nativeSamples_&&!preparePendingWaveNativeBacking(error)){
            custody.failed_=true;return false;
        }
        using Pending=rr::platform::OriginalAudioPendingWaves50KG;
        const Pending::RootOperation operation(*this,entities(),Pending::RootOperation::Kind::nativeWingResolve26D9F0);
        if(!custody.pending_->flushNativeWing26D9F0(operation,error)){
            custody.failed_=true;return false;
        }
        if(!custody.watto_->continueAfterFlush1D4688(custody.pending_,diagnostic))
            return custodyFailure(error,"Garage Watto source reverb/constructor suffix is not complete; genuine prefix retained");
        // Native cold service, not a fictitious source flush PC: the actual
        // constructor has enqueued its routing control. Apply it to the real
        // silent mixer owner before later widgets/Scene continue. No sample
        // history advances and no wet playback readiness follows from this.
        if(!custody.audio_->serviceColdGarageRouting(error))return false;
        error.clear();return true;
    }catch(...){custody.failed_=true;
        return custodyFailure(error,"Garage Watto custody allocation failed; attempted prefix retained");}
}
bool Application::retireGaragePartialSceneAttachmentFailure(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;using P=Custody::PartialSceneAbortPhase;
    using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    diagnostic={};const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=Status::phase;diagnostic.sourcePC=pc;return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||
       !owner->partialSceneFailurePrefixCurrent(*this)||owner->entryFrontend_!=&frontend||
       frontend.materialApplicationOwner!=this||!sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x1B4A38,"Garage cleanup requires the exact count6 Scene attachment failure");
    auto& s=*owner;const auto resolver=s.managerDomainLease_->resolver3E1A70();
    rr::assets::OriginalStandardTextureDiagnostic50LA bank;
    if(s.partialSceneAbortPhase_<P::bankRetired&&
       (!resolver||!s.managerBankLease_||!resolver->canRetireGarageBank12E5E0(*s.managerBankLease_,bank))){
        diagnostic.texture=bank;return fail(bank.sourcePC?bank.sourcePC:0x12E5E0,"Failed partial Scene lost its real releasable base bank");
    }
    if(s.partialSceneAbortPhase_==P::notStarted){
        rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
        if(!s.page_->preflightFailedSceneAttachment(page))
            return fail(page.sourcePC,"Failed partial Scene exact Page/Scene preflight refused");
        if(!frontend.garageTransferOwnerCurrent(s.grant_))return fail(0x2543F0,"Failed partial Scene transfer owner changed");
        if(!s.audio_->preflightPartialSceneEntryFailure(owner,error)){
            diagnostic.status=Status::phase;diagnostic.sourcePC=0x235150;return false;
        }
        s.partialSceneAbortFrontend_=&frontend;s.retiring_=true;s.partialSceneAbortPhase_=P::pageRetiring;
    }
    if(s.partialSceneAbortPhase_==P::pageRetiring){
        rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
        if(!s.page_->retireFailedSceneAttachmentPrefix(page))return fail(page.sourcePC,"Failed partial Scene exact payload retirement remains pending");
        s.partialSceneAbortPhase_=P::pageRetired;
    }
    if(s.partialSceneAbortPhase_==P::pageRetired){
        rr::platform::OriginalUiGarageManagerFog50KG::Diagnostic fog;
        if(!s.entryFog_->retireAttached1CD080(fog))return fail(fog.sourcePC,"Failed partial Scene Fog tail retirement refused");
        s.partialSceneAbortPhase_=P::fogRetired;
    }
    if(s.partialSceneAbortPhase_==P::fogRetired){
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG widget;
        if(!s.widgets_->retireDetached2A8F70(widget))return fail(widget.sourcePC,"Failed partial Scene Widget/Watto retirement remains pending");
        s.partialSceneAbortPhase_=P::widgetsRetired;
    }
    if(s.partialSceneAbortPhase_==P::widgetsRetired){
        rr::platform::OriginalUiGarageManagerFog50KG::Diagnostic fog;
        if(!s.entryFog_->retireDetached1CCFA0(fog))return fail(fog.sourcePC,"Failed partial Scene Fog aliases remain retained");
        s.partialSceneAbortPhase_=P::fogAliasesReleased;
    }
    if(s.partialSceneAbortPhase_==P::fogAliasesReleased){
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireLight146980(1,light))return fail(light.sourcePC,"Failed partial Scene directional Light retirement refused");
        s.partialSceneAbortPhase_=P::directionalRetired;
    }
    if(s.partialSceneAbortPhase_==P::directionalRetired){
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireLight146980(0,light))return fail(light.sourcePC,"Failed partial Scene ambient Light retirement refused");
        s.partialSceneAbortPhase_=P::ambientRetired;
    }
    if(s.partialSceneAbortPhase_==P::ambientRetired){
        rr::platform::OriginalUiGarageManagerSearch50KG::Diagnostic search;
        if(!s.entrySearch_->retireNode27F180(search))return fail(search.sourcePC,"Failed partial Scene Search node retirement refused");
        s.partialSceneAbortPhase_=P::searchNodeRetired;
    }
    if(s.partialSceneAbortPhase_==P::searchNodeRetired){
        if(!s.preflightAttachedSelectedAbort(diagnostic))return custodyFailure(error,"Failed partial Scene selected preflight refused");
        // This existing destructor is not restartable after a partial failure.
        s.partialSceneAbortPhase_=P::selectedRetiring;
        if(!s.entrySelected_->close1D18D0(diagnostic))return custodyFailure(error,"Failed partial Scene selected destructor stopped; no replay");
        s.partialSceneAbortPhase_=P::selectedRetired;
    }
    if(s.partialSceneAbortPhase_==P::selectedRetiring)return fail(0x142590,"Failed selected destructor retains its exact irreversible prefix");
    if(s.partialSceneAbortPhase_==P::selectedRetired){
        const auto step=s.grant_->beginGarageManagerPayloadRetirement(s.entrySearch_.get());
        if(!step.okay())return fail(step.sourcePC,"Failed partial Scene empty manager last-reference release refused");
        s.partialSceneAbortPhase_=P::managerRefReleased;
    }
    if(s.partialSceneAbortPhase_==P::managerRefReleased){
        rr::platform::OriginalUiFrontendDiagnostic50KG transfer;
        if(!frontend.retireFailedPartialSceneTransfers(owner,transfer))
            return fail(transfer.sourcePC,"Failed partial Scene actual transfer retirement remains pending");
        s.partialSceneAbortPhase_=P::transfersRetired;
    }
    if(s.partialSceneAbortPhase_==P::transfersRetired){
        rr::platform::OriginalUiGarageManagerSearch50KG::Diagnostic search;
        if(!s.entrySearch_->retireManagerList1CCADC(search))return fail(search.sourcePC,"Failed partial Scene Search payload retirement refused");
        s.partialSceneAbortPhase_=P::searchPayloadRetired;
    }
    if(s.partialSceneAbortPhase_==P::searchPayloadRetired){
        // Constructor190 is actually null; no invented positive OnTop work.
        if(!s.managerDeferred190Empty_)return fail(0x1CCAF0,"Failed partial Scene deferred owner is not empty");
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireManagerDescriptors1CCB20(light))return fail(light.sourcePC,"Failed partial Scene Light payload retirement refused");
        s.partialSceneAbortPhase_=P::lightPayloadRetired;
    }
    if(s.partialSceneAbortPhase_==P::lightPayloadRetired){
        if(!s.scope_->failedGaragePartialScenePayloadCurrent(s.entryManagerGeneration_,&s))
            return fail(0x12E5E0,"Failed partial Scene payload receipt is stale before bank close");
        if(!resolver->retireGarageBank12E5E0(*s.managerBankLease_,bank)){
            diagnostic.texture=bank;return fail(bank.sourcePC,"Failed partial Scene bank close refused");
        }
        const auto step=s.scope_->noteFailedPartialSceneBankRetired12E5E0(s.entryManagerGeneration_,&s);
        if(!step.okay())std::terminate();
        s.managerArchiveOnlyScope170_=false;s.partialSceneAbortPhase_=P::bankRetired;
    }
    if(s.partialSceneAbortPhase_==P::bankRetired){
        if(!s.releaseManagerPageTable2A3B04(diagnostic))return custodyFailure(error,"Failed partial Scene native Page table release refused");
        s.partialSceneAbortPhase_=P::pageTableRetired;
    }
    if(s.partialSceneAbortPhase_==P::pageTableRetired){
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG widget;
        if(!s.widgets_->retireManagerTable2A3B18(widget))return fail(widget.sourcePC,"Failed partial Scene Widget table release refused");
        s.partialSceneAbortPhase_=P::widgetTableRetired;
    }
    if(s.partialSceneAbortPhase_==P::widgetTableRetired){
        const auto step=s.scope_->finishGarageManagerPayloadRetirement(s.entryManagerGeneration_,s.entrySearch_.get());
        if(!step.okay())return fail(step.sourcePC,"Failed partial Scene counted manager release refused");
        s.partialSceneAbortPhase_=P::managerRetired;
    }
    if(s.partialSceneAbortPhase_==P::managerRetired){
        if(!s.audio_->retirePartialSceneEntryFailure(owner,error)){
            diagnostic.status=Status::phase;diagnostic.sourcePC=0x235150;return false;
        }
        s.partialSceneAbortPhase_=P::audioRetired;
    }
    if(s.partialSceneAbortPhase_!=P::audioRetired)return fail(0x1413E0,"Failed partial Scene retirement cursor is not terminal");
    // Actual dead Watto/source records stayed alive through Main's typed commit.
    // This failed constructor remains terminal-only, never a normal Back receipt.
    // Main consumed the last retired Scene alias. Do not dereference raw podScene_ after that commit.
    s.podScene_=nullptr;s.visibility_.reset();s.actors_.reset();
    s.page_.reset();s.watto_.reset();s.widgets_.reset();s.entryFog_.reset();s.entryLights_.reset();s.entrySearch_.reset();
    s.entrySelected_.reset();s.actorInputs_.reset();s.wattoInputs_.reset();s.resources_.reset();
    s.managerDat298_.reset();s.managerDrivePageData_.reset();s.managerDriveProfile_.reset();s.entryFonts_.reset();s.entryTiming_={};
    s.wattoAssets_.reset();s.sceneAssets_.reset();s.managerBankLease_.reset();
    s.managerArchiveLease_.reset();s.managerProviderLease_.reset();s.managerDomainLease_.reset();
    s.audio_.reset();s.pending_.reset();s.application_.reset();s.motions_.reset();s.authority_.reset();
    s.grant_.reset();s.scope_.reset();s.entryFrontend_=nullptr;
    s.partialSceneAbortPhase_=P::closed;s.closed_=true;garageCustodyOwner_.reset();error.clear();return true;
}
bool Application::retireGarageWattoBindingFailure(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;using P=Custody::WattoBindingAbortPhase;
    using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    diagnostic={};const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=Status::phase;diagnostic.sourcePC=pc;return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||
       !owner->wattoBindingFailurePrefixCurrent(*this)||owner->entryFrontend_!=&frontend||
       frontend.materialApplicationOwner!=this||!sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x1D4800,"Garage cleanup requires the exact activated Page pre-bind entry failure");
    auto& s=*owner;const auto resolver=s.managerDomainLease_->resolver3E1A70();
    rr::assets::OriginalStandardTextureDiagnostic50LA bank;
    if(s.wattoBindingAbortPhase_<P::bankRetired&&
       (!resolver||!s.managerBankLease_||!resolver->canRetireGarageBank12E5E0(*s.managerBankLease_,bank))){
        diagnostic.texture=bank;return fail(bank.sourcePC?bank.sourcePC:0x12E5E0,"Failed Watto binding Page lost its real releasable base bank");
    }
    if(s.wattoBindingAbortPhase_==P::notStarted){
        rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
        if(!s.page_->preflightFailedWattoBinding1D4800(page))
            return fail(page.sourcePC,"Failed Watto binding Page exact Page/Scene preflight refused");
        if(!frontend.garageTransferOwnerCurrent(s.grant_))return fail(0x2543F0,"Failed Watto binding Page transfer owner changed");
        if(!s.audio_->preflightWattoBindingEntryFailure(owner,error)){
            diagnostic.status=Status::phase;diagnostic.sourcePC=0x235150;return false;
        }
        s.wattoBindingAbortFrontend_=&frontend;s.retiring_=true;s.wattoBindingAbortPhase_=P::pageRetiring;
    }
    if(s.wattoBindingAbortPhase_==P::pageRetiring){
        rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
        if(!s.page_->retireFailedWattoBinding1D4800(page))return fail(page.sourcePC,"Failed Watto binding Page exact payload retirement remains pending");
        s.wattoBindingAbortPhase_=P::pageRetired;
    }
    if(s.wattoBindingAbortPhase_==P::pageRetired){
        rr::platform::OriginalUiGarageManagerFog50KG::Diagnostic fog;
        if(!s.entryFog_->retireAttached1CD080(fog))return fail(fog.sourcePC,"Failed Watto binding Page Fog tail retirement refused");
        s.wattoBindingAbortPhase_=P::fogRetired;
    }
    if(s.wattoBindingAbortPhase_==P::fogRetired){
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG widget;
        if(!s.widgets_->retireDetached2A8F70(widget))return fail(widget.sourcePC,"Failed Watto binding Page Widget/Watto retirement remains pending");
        s.wattoBindingAbortPhase_=P::widgetsRetired;
    }
    if(s.wattoBindingAbortPhase_==P::widgetsRetired){
        rr::platform::OriginalUiGarageManagerFog50KG::Diagnostic fog;
        if(!s.entryFog_->retireDetached1CCFA0(fog))return fail(fog.sourcePC,"Failed Watto binding Page Fog aliases remain retained");
        s.wattoBindingAbortPhase_=P::fogAliasesReleased;
    }
    if(s.wattoBindingAbortPhase_==P::fogAliasesReleased){
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireLight146980(1,light))return fail(light.sourcePC,"Failed Watto binding Page directional Light retirement refused");
        s.wattoBindingAbortPhase_=P::directionalRetired;
    }
    if(s.wattoBindingAbortPhase_==P::directionalRetired){
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireLight146980(0,light))return fail(light.sourcePC,"Failed Watto binding Page ambient Light retirement refused");
        s.wattoBindingAbortPhase_=P::ambientRetired;
    }
    if(s.wattoBindingAbortPhase_==P::ambientRetired){
        rr::platform::OriginalUiGarageManagerSearch50KG::Diagnostic search;
        if(!s.entrySearch_->retireNode27F180(search))return fail(search.sourcePC,"Failed Watto binding Page Search node retirement refused");
        s.wattoBindingAbortPhase_=P::searchNodeRetired;
    }
    if(s.wattoBindingAbortPhase_==P::searchNodeRetired)s.wattoBindingAbortPhase_=P::selectedRetiring;
    if(s.wattoBindingAbortPhase_==P::selectedRetiring){
        // The real1FB67C observer outlives Search. Consume this distinct
        // completed-Page proof in the actual restartable manager/model tail,
        // never the observer-absent close1D18D0 or normal Page receipt.
        if(!s.entrySelected_->closeFailedWattoBindingManager142590(*s.page_,diagnostic))
            return custodyFailure(error,"Failed Watto binding Page selected observer/model retirement remains pending");
        s.wattoBindingAbortPhase_=P::selectedRetired;
    }
    if(s.wattoBindingAbortPhase_==P::selectedRetired){
        const auto step=s.grant_->beginGarageManagerPayloadRetirement(s.entrySearch_.get());
        if(!step.okay())return fail(step.sourcePC,"Failed Watto binding Page empty manager last-reference release refused");
        s.wattoBindingAbortPhase_=P::managerRefReleased;
    }
    if(s.wattoBindingAbortPhase_==P::managerRefReleased){
        rr::platform::OriginalUiFrontendDiagnostic50KG transfer;
        if(!frontend.retireFailedWattoBindingTransfers(owner,transfer))
            return fail(transfer.sourcePC,"Failed Watto binding Page actual transfer retirement remains pending");
        s.wattoBindingAbortPhase_=P::transfersRetired;
    }
    if(s.wattoBindingAbortPhase_==P::transfersRetired){
        rr::platform::OriginalUiGarageManagerSearch50KG::Diagnostic search;
        if(!s.entrySearch_->retireManagerList1CCADC(search))return fail(search.sourcePC,"Failed Watto binding Page Search payload retirement refused");
        s.wattoBindingAbortPhase_=P::searchPayloadRetired;
    }
    if(s.wattoBindingAbortPhase_==P::searchPayloadRetired){
        // Constructor190 is actually null; no invented positive OnTop work.
        if(!s.managerDeferred190Empty_)return fail(0x1CCAF0,"Failed Watto binding Page deferred owner is not empty");
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireManagerDescriptors1CCB20(light))return fail(light.sourcePC,"Failed Watto binding Page Light payload retirement refused");
        s.wattoBindingAbortPhase_=P::lightPayloadRetired;
    }
    if(s.wattoBindingAbortPhase_==P::lightPayloadRetired){
        if(!s.scope_->failedGarageWattoBindingPayloadCurrent(s.entryManagerGeneration_,&s))
            return fail(0x12E5E0,"Failed Watto binding Page payload receipt is stale before bank close");
        if(!resolver->retireGarageBank12E5E0(*s.managerBankLease_,bank)){
            diagnostic.texture=bank;return fail(bank.sourcePC,"Failed Watto binding Page bank close refused");
        }
        const auto step=s.scope_->noteFailedWattoBindingBankRetired12E5E0(s.entryManagerGeneration_,&s);
        if(!step.okay())std::terminate();
        s.managerArchiveOnlyScope170_=false;s.wattoBindingAbortPhase_=P::bankRetired;
    }
    if(s.wattoBindingAbortPhase_==P::bankRetired){
        if(!s.releaseManagerPageTable2A3B04(diagnostic))return custodyFailure(error,"Failed Watto binding Page native Page table release refused");
        s.wattoBindingAbortPhase_=P::pageTableRetired;
    }
    if(s.wattoBindingAbortPhase_==P::pageTableRetired){
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG widget;
        if(!s.widgets_->retireManagerTable2A3B18(widget))return fail(widget.sourcePC,"Failed Watto binding Page Widget table release refused");
        s.wattoBindingAbortPhase_=P::widgetTableRetired;
    }
    if(s.wattoBindingAbortPhase_==P::widgetTableRetired){
        const auto step=s.scope_->finishGarageManagerPayloadRetirement(s.entryManagerGeneration_,s.entrySearch_.get());
        if(!step.okay())return fail(step.sourcePC,"Failed Watto binding Page counted manager release refused");
        s.wattoBindingAbortPhase_=P::managerRetired;
    }
    if(s.wattoBindingAbortPhase_==P::managerRetired){
        if(!s.audio_->retireWattoBindingEntryFailure(owner,error)){
            diagnostic.status=Status::phase;diagnostic.sourcePC=0x235150;return false;
        }
        s.wattoBindingAbortPhase_=P::audioRetired;
    }
    if(s.wattoBindingAbortPhase_!=P::audioRetired)return fail(0x1413E0,"Failed Watto binding Page retirement cursor is not terminal");
    // Actual dead Watto/source records stayed alive through Main's typed commit.
    // This failed constructor remains terminal-only, never a normal Back receipt.
    // Main consumed the last retired Scene alias. Do not dereference raw podScene_ after that commit.
    s.podScene_=nullptr;s.visibility_.reset();s.actors_.reset();
    s.page_.reset();s.watto_.reset();s.widgets_.reset();s.entryFog_.reset();s.entryLights_.reset();s.entrySearch_.reset();
    s.entrySelected_.reset();s.actorInputs_.reset();s.wattoInputs_.reset();s.resources_.reset();
    s.managerDat298_.reset();s.managerDrivePageData_.reset();s.managerDriveProfile_.reset();s.entryFonts_.reset();s.entryTiming_={};
    s.wattoAssets_.reset();s.sceneAssets_.reset();s.managerBankLease_.reset();
    s.managerArchiveLease_.reset();s.managerProviderLease_.reset();s.managerDomainLease_.reset();
    s.audio_.reset();s.pending_.reset();s.application_.reset();s.motions_.reset();s.authority_.reset();
    s.grant_.reset();s.scope_.reset();s.entryFrontend_=nullptr;
    s.wattoBindingAbortPhase_=P::closed;s.closed_=true;garageCustodyOwner_.reset();error.clear();return true;
}
bool Application::retireGarageLoadedPageHideFailure(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;using P=Custody::LoadedPageAbortPhase;
    using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    diagnostic={};const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=Status::phase;diagnostic.sourcePC=pc;return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||
       !owner->loadedPageFailurePrefixCurrent(*this)||owner->entryFrontend_!=&frontend||
       frontend.materialApplicationOwner!=this||!sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x1415A0,"Garage cleanup requires the exact attached loaded-Page hide preflight failure");
    auto& s=*owner;const auto resolver=s.managerDomainLease_->resolver3E1A70();
    rr::assets::OriginalStandardTextureDiagnostic50LA bank;
    if(s.loadedPageAbortPhase_<P::bankRetired&&
       (!resolver||!s.managerBankLease_||!resolver->canRetireGarageBank12E5E0(*s.managerBankLease_,bank))){
        diagnostic.texture=bank;return fail(bank.sourcePC?bank.sourcePC:0x12E5E0,"Failed loaded Page lost its real releasable base bank");
    }
    if(s.loadedPageAbortPhase_==P::notStarted){
        rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
        if(!s.page_->preflightFailedLoadedHide1415A0(page))
            return fail(page.sourcePC,"Failed loaded Page exact Page/Scene preflight refused");
        if(!frontend.garageTransferOwnerCurrent(s.grant_))return fail(0x2543F0,"Failed loaded Page transfer owner changed");
        if(!s.audio_->preflightLoadedPageEntryFailure(owner,error)){
            diagnostic.status=Status::phase;diagnostic.sourcePC=0x235150;return false;
        }
        s.loadedPageAbortFrontend_=&frontend;s.retiring_=true;s.loadedPageAbortPhase_=P::pageRetiring;
    }
    if(s.loadedPageAbortPhase_==P::pageRetiring){
        rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
        if(!s.page_->retireFailedLoadedHide1415A0(page))return fail(page.sourcePC,"Failed loaded Page exact payload retirement remains pending");
        s.loadedPageAbortPhase_=P::pageRetired;
    }
    if(s.loadedPageAbortPhase_==P::pageRetired){
        rr::platform::OriginalUiGarageManagerFog50KG::Diagnostic fog;
        if(!s.entryFog_->retireAttached1CD080(fog))return fail(fog.sourcePC,"Failed loaded Page Fog tail retirement refused");
        s.loadedPageAbortPhase_=P::fogRetired;
    }
    if(s.loadedPageAbortPhase_==P::fogRetired){
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG widget;
        if(!s.widgets_->retireDetached2A8F70(widget))return fail(widget.sourcePC,"Failed loaded Page Widget/Watto retirement remains pending");
        s.loadedPageAbortPhase_=P::widgetsRetired;
    }
    if(s.loadedPageAbortPhase_==P::widgetsRetired){
        rr::platform::OriginalUiGarageManagerFog50KG::Diagnostic fog;
        if(!s.entryFog_->retireDetached1CCFA0(fog))return fail(fog.sourcePC,"Failed loaded Page Fog aliases remain retained");
        s.loadedPageAbortPhase_=P::fogAliasesReleased;
    }
    if(s.loadedPageAbortPhase_==P::fogAliasesReleased){
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireLight146980(1,light))return fail(light.sourcePC,"Failed loaded Page directional Light retirement refused");
        s.loadedPageAbortPhase_=P::directionalRetired;
    }
    if(s.loadedPageAbortPhase_==P::directionalRetired){
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireLight146980(0,light))return fail(light.sourcePC,"Failed loaded Page ambient Light retirement refused");
        s.loadedPageAbortPhase_=P::ambientRetired;
    }
    if(s.loadedPageAbortPhase_==P::ambientRetired){
        rr::platform::OriginalUiGarageManagerSearch50KG::Diagnostic search;
        if(!s.entrySearch_->retireNode27F180(search))return fail(search.sourcePC,"Failed loaded Page Search node retirement refused");
        s.loadedPageAbortPhase_=P::searchNodeRetired;
    }
    if(s.loadedPageAbortPhase_==P::searchNodeRetired)s.loadedPageAbortPhase_=P::selectedRetiring;
    if(s.loadedPageAbortPhase_==P::selectedRetiring){
        // The real1FB67C observer outlives Search. Consume this distinct
        // completed-Page proof in the actual restartable manager/model tail,
        // never the observer-absent close1D18D0 or normal Page receipt.
        if(!s.entrySelected_->closeFailedLoadedPageManager142590(*s.page_,diagnostic))
            return custodyFailure(error,"Failed loaded Page selected observer/model retirement remains pending");
        s.loadedPageAbortPhase_=P::selectedRetired;
    }
    if(s.loadedPageAbortPhase_==P::selectedRetired){
        const auto step=s.grant_->beginGarageManagerPayloadRetirement(s.entrySearch_.get());
        if(!step.okay())return fail(step.sourcePC,"Failed loaded Page empty manager last-reference release refused");
        s.loadedPageAbortPhase_=P::managerRefReleased;
    }
    if(s.loadedPageAbortPhase_==P::managerRefReleased){
        rr::platform::OriginalUiFrontendDiagnostic50KG transfer;
        if(!frontend.retireFailedLoadedHideTransfers(owner,transfer))
            return fail(transfer.sourcePC,"Failed loaded Page actual transfer retirement remains pending");
        s.loadedPageAbortPhase_=P::transfersRetired;
    }
    if(s.loadedPageAbortPhase_==P::transfersRetired){
        rr::platform::OriginalUiGarageManagerSearch50KG::Diagnostic search;
        if(!s.entrySearch_->retireManagerList1CCADC(search))return fail(search.sourcePC,"Failed loaded Page Search payload retirement refused");
        s.loadedPageAbortPhase_=P::searchPayloadRetired;
    }
    if(s.loadedPageAbortPhase_==P::searchPayloadRetired){
        // Constructor190 is actually null; no invented positive OnTop work.
        if(!s.managerDeferred190Empty_)return fail(0x1CCAF0,"Failed loaded Page deferred owner is not empty");
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireManagerDescriptors1CCB20(light))return fail(light.sourcePC,"Failed loaded Page Light payload retirement refused");
        s.loadedPageAbortPhase_=P::lightPayloadRetired;
    }
    if(s.loadedPageAbortPhase_==P::lightPayloadRetired){
        if(!s.scope_->failedGarageLoadedHidePayloadCurrent(s.entryManagerGeneration_,&s))
            return fail(0x12E5E0,"Failed loaded Page payload receipt is stale before bank close");
        if(!resolver->retireGarageBank12E5E0(*s.managerBankLease_,bank)){
            diagnostic.texture=bank;return fail(bank.sourcePC,"Failed loaded Page bank close refused");
        }
        const auto step=s.scope_->noteFailedLoadedHideBankRetired12E5E0(s.entryManagerGeneration_,&s);
        if(!step.okay())std::terminate();
        s.managerArchiveOnlyScope170_=false;s.loadedPageAbortPhase_=P::bankRetired;
    }
    if(s.loadedPageAbortPhase_==P::bankRetired){
        if(!s.releaseManagerPageTable2A3B04(diagnostic))return custodyFailure(error,"Failed loaded Page native Page table release refused");
        s.loadedPageAbortPhase_=P::pageTableRetired;
    }
    if(s.loadedPageAbortPhase_==P::pageTableRetired){
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG widget;
        if(!s.widgets_->retireManagerTable2A3B18(widget))return fail(widget.sourcePC,"Failed loaded Page Widget table release refused");
        s.loadedPageAbortPhase_=P::widgetTableRetired;
    }
    if(s.loadedPageAbortPhase_==P::widgetTableRetired){
        const auto step=s.scope_->finishGarageManagerPayloadRetirement(s.entryManagerGeneration_,s.entrySearch_.get());
        if(!step.okay())return fail(step.sourcePC,"Failed loaded Page counted manager release refused");
        s.loadedPageAbortPhase_=P::managerRetired;
    }
    if(s.loadedPageAbortPhase_==P::managerRetired){
        if(!s.audio_->retireLoadedPageEntryFailure(owner,error)){
            diagnostic.status=Status::phase;diagnostic.sourcePC=0x235150;return false;
        }
        s.loadedPageAbortPhase_=P::audioRetired;
    }
    if(s.loadedPageAbortPhase_!=P::audioRetired)return fail(0x1413E0,"Failed loaded Page retirement cursor is not terminal");
    // Actual dead Watto/source records stayed alive through Main's typed commit.
    // This failed constructor remains terminal-only, never a normal Back receipt.
    // Main consumed the last retired Scene alias. Do not dereference raw podScene_ after that commit.
    s.podScene_=nullptr;s.visibility_.reset();s.actors_.reset();
    s.page_.reset();s.watto_.reset();s.widgets_.reset();s.entryFog_.reset();s.entryLights_.reset();s.entrySearch_.reset();
    s.entrySelected_.reset();s.actorInputs_.reset();s.wattoInputs_.reset();s.resources_.reset();
    s.managerDat298_.reset();s.managerDrivePageData_.reset();s.managerDriveProfile_.reset();s.entryFonts_.reset();s.entryTiming_={};
    s.wattoAssets_.reset();s.sceneAssets_.reset();s.managerBankLease_.reset();
    s.managerArchiveLease_.reset();s.managerProviderLease_.reset();s.managerDomainLease_.reset();
    s.audio_.reset();s.pending_.reset();s.application_.reset();s.motions_.reset();s.authority_.reset();
    s.grant_.reset();s.scope_.reset();s.entryFrontend_=nullptr;
    s.loadedPageAbortPhase_=P::closed;s.closed_=true;garageCustodyOwner_.reset();error.clear();return true;
}
bool Application::retireGarageLateSceneBorrowFailure(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;using P=Custody::LateBorrowAbortPhase;
    using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    diagnostic={};const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=Status::phase;diagnostic.sourcePC=pc;return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||
       !owner->lateBorrowFailurePrefixCurrent(*this)||owner->entryFrontend_!=&frontend||
       frontend.materialApplicationOwner!=this||!sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x1B4D24,"Garage cleanup requires the exact count10 late Scene borrow failure");
    auto& s=*owner;const auto resolver=s.managerDomainLease_->resolver3E1A70();
    rr::assets::OriginalStandardTextureDiagnostic50LA bank;
    if(s.lateBorrowAbortPhase_<P::bankRetired&&
       (!resolver||!s.managerBankLease_||!resolver->canRetireGarageBank12E5E0(*s.managerBankLease_,bank))){
        diagnostic.texture=bank;return fail(bank.sourcePC?bank.sourcePC:0x12E5E0,"Failed late-borrow Scene lost its real releasable base bank");
    }
    if(s.lateBorrowAbortPhase_==P::notStarted){
        rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
        if(!s.page_->preflightFailedLateBorrow1B4D24(page))
            return fail(page.sourcePC,"Failed late-borrow Scene exact Page/Scene preflight refused");
        if(!frontend.garageTransferOwnerCurrent(s.grant_))return fail(0x2543F0,"Failed late-borrow Scene transfer owner changed");
        if(!s.audio_->preflightLateBorrowEntryFailure(owner,error)){
            diagnostic.status=Status::phase;diagnostic.sourcePC=0x235150;return false;
        }
        s.lateBorrowAbortFrontend_=&frontend;s.retiring_=true;s.lateBorrowAbortPhase_=P::pageRetiring;
    }
    if(s.lateBorrowAbortPhase_==P::pageRetiring){
        rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
        if(!s.page_->retireFailedLateBorrow1B4D24(page))return fail(page.sourcePC,"Failed late-borrow Scene exact payload retirement remains pending");
        s.lateBorrowAbortPhase_=P::pageRetired;
    }
    if(s.lateBorrowAbortPhase_==P::pageRetired){
        rr::platform::OriginalUiGarageManagerFog50KG::Diagnostic fog;
        if(!s.entryFog_->retireAttached1CD080(fog))return fail(fog.sourcePC,"Failed late-borrow Scene Fog tail retirement refused");
        s.lateBorrowAbortPhase_=P::fogRetired;
    }
    if(s.lateBorrowAbortPhase_==P::fogRetired){
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG widget;
        if(!s.widgets_->retireDetached2A8F70(widget))return fail(widget.sourcePC,"Failed late-borrow Scene Widget/Watto retirement remains pending");
        s.lateBorrowAbortPhase_=P::widgetsRetired;
    }
    if(s.lateBorrowAbortPhase_==P::widgetsRetired){
        rr::platform::OriginalUiGarageManagerFog50KG::Diagnostic fog;
        if(!s.entryFog_->retireDetached1CCFA0(fog))return fail(fog.sourcePC,"Failed late-borrow Scene Fog aliases remain retained");
        s.lateBorrowAbortPhase_=P::fogAliasesReleased;
    }
    if(s.lateBorrowAbortPhase_==P::fogAliasesReleased){
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireLight146980(1,light))return fail(light.sourcePC,"Failed late-borrow Scene directional Light retirement refused");
        s.lateBorrowAbortPhase_=P::directionalRetired;
    }
    if(s.lateBorrowAbortPhase_==P::directionalRetired){
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireLight146980(0,light))return fail(light.sourcePC,"Failed late-borrow Scene ambient Light retirement refused");
        s.lateBorrowAbortPhase_=P::ambientRetired;
    }
    if(s.lateBorrowAbortPhase_==P::ambientRetired){
        rr::platform::OriginalUiGarageManagerSearch50KG::Diagnostic search;
        if(!s.entrySearch_->retireNode27F180(search))return fail(search.sourcePC,"Failed late-borrow Scene Search node retirement refused");
        s.lateBorrowAbortPhase_=P::searchNodeRetired;
    }
    if(s.lateBorrowAbortPhase_==P::searchNodeRetired){
        if(!s.preflightAttachedSelectedAbort(diagnostic))return custodyFailure(error,"Failed late-borrow Scene selected preflight refused");
        // This existing destructor is not restartable after a partial failure.
        s.lateBorrowAbortPhase_=P::selectedRetiring;
        if(!s.entrySelected_->close1D18D0(diagnostic))return custodyFailure(error,"Failed late-borrow Scene selected destructor stopped; no replay");
        s.lateBorrowAbortPhase_=P::selectedRetired;
    }
    if(s.lateBorrowAbortPhase_==P::selectedRetiring)return fail(0x142590,"Failed selected destructor retains its exact irreversible prefix");
    if(s.lateBorrowAbortPhase_==P::selectedRetired){
        const auto step=s.grant_->beginGarageManagerPayloadRetirement(s.entrySearch_.get());
        if(!step.okay())return fail(step.sourcePC,"Failed late-borrow Scene empty manager last-reference release refused");
        s.lateBorrowAbortPhase_=P::managerRefReleased;
    }
    if(s.lateBorrowAbortPhase_==P::managerRefReleased){
        rr::platform::OriginalUiFrontendDiagnostic50KG transfer;
        if(!frontend.retireFailedLateBorrowTransfers(owner,transfer))
            return fail(transfer.sourcePC,"Failed late-borrow Scene actual transfer retirement remains pending");
        s.lateBorrowAbortPhase_=P::transfersRetired;
    }
    if(s.lateBorrowAbortPhase_==P::transfersRetired){
        rr::platform::OriginalUiGarageManagerSearch50KG::Diagnostic search;
        if(!s.entrySearch_->retireManagerList1CCADC(search))return fail(search.sourcePC,"Failed late-borrow Scene Search payload retirement refused");
        s.lateBorrowAbortPhase_=P::searchPayloadRetired;
    }
    if(s.lateBorrowAbortPhase_==P::searchPayloadRetired){
        // Constructor190 is actually null; no invented positive OnTop work.
        if(!s.managerDeferred190Empty_)return fail(0x1CCAF0,"Failed late-borrow Scene deferred owner is not empty");
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireManagerDescriptors1CCB20(light))return fail(light.sourcePC,"Failed late-borrow Scene Light payload retirement refused");
        s.lateBorrowAbortPhase_=P::lightPayloadRetired;
    }
    if(s.lateBorrowAbortPhase_==P::lightPayloadRetired){
        if(!s.scope_->failedGarageLateBorrowPayloadCurrent(s.entryManagerGeneration_,&s))
            return fail(0x12E5E0,"Failed late-borrow Scene payload receipt is stale before bank close");
        if(!resolver->retireGarageBank12E5E0(*s.managerBankLease_,bank)){
            diagnostic.texture=bank;return fail(bank.sourcePC,"Failed late-borrow Scene bank close refused");
        }
        const auto step=s.scope_->noteFailedLateBorrowBankRetired12E5E0(s.entryManagerGeneration_,&s);
        if(!step.okay())std::terminate();
        s.managerArchiveOnlyScope170_=false;s.lateBorrowAbortPhase_=P::bankRetired;
    }
    if(s.lateBorrowAbortPhase_==P::bankRetired){
        if(!s.releaseManagerPageTable2A3B04(diagnostic))return custodyFailure(error,"Failed late-borrow Scene native Page table release refused");
        s.lateBorrowAbortPhase_=P::pageTableRetired;
    }
    if(s.lateBorrowAbortPhase_==P::pageTableRetired){
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG widget;
        if(!s.widgets_->retireManagerTable2A3B18(widget))return fail(widget.sourcePC,"Failed late-borrow Scene Widget table release refused");
        s.lateBorrowAbortPhase_=P::widgetTableRetired;
    }
    if(s.lateBorrowAbortPhase_==P::widgetTableRetired){
        const auto step=s.scope_->finishGarageManagerPayloadRetirement(s.entryManagerGeneration_,s.entrySearch_.get());
        if(!step.okay())return fail(step.sourcePC,"Failed late-borrow Scene counted manager release refused");
        s.lateBorrowAbortPhase_=P::managerRetired;
    }
    if(s.lateBorrowAbortPhase_==P::managerRetired){
        if(!s.audio_->retireLateBorrowEntryFailure(owner,error)){
            diagnostic.status=Status::phase;diagnostic.sourcePC=0x235150;return false;
        }
        s.lateBorrowAbortPhase_=P::audioRetired;
    }
    if(s.lateBorrowAbortPhase_!=P::audioRetired)return fail(0x1413E0,"Failed late-borrow Scene retirement cursor is not terminal");
    // Actual dead Watto/source records stayed alive through Main's typed commit.
    // This failed constructor remains terminal-only, never a normal Back receipt.
    // Main consumed the last retired Scene alias. Do not dereference raw podScene_ after that commit.
    s.podScene_=nullptr;s.visibility_.reset();s.actors_.reset();
    s.page_.reset();s.watto_.reset();s.widgets_.reset();s.entryFog_.reset();s.entryLights_.reset();s.entrySearch_.reset();
    s.entrySelected_.reset();s.actorInputs_.reset();s.wattoInputs_.reset();s.resources_.reset();
    s.managerDat298_.reset();s.managerDrivePageData_.reset();s.managerDriveProfile_.reset();s.entryFonts_.reset();s.entryTiming_={};
    s.wattoAssets_.reset();s.sceneAssets_.reset();s.managerBankLease_.reset();
    s.managerArchiveLease_.reset();s.managerProviderLease_.reset();s.managerDomainLease_.reset();
    s.audio_.reset();s.pending_.reset();s.application_.reset();s.motions_.reset();s.authority_.reset();
    s.grant_.reset();s.scope_.reset();s.entryFrontend_=nullptr;
    s.lateBorrowAbortPhase_=P::closed;s.closed_=true;garageCustodyOwner_.reset();error.clear();return true;
}
bool Application::retireGarageSharedLoaderFailure(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;using P=Custody::SharedPageAbortPhase;
    using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    diagnostic={};const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=Status::phase;diagnostic.sourcePC=pc;return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||
       !owner->sharedPageFailurePrefixCurrent(*this)||owner->entryFrontend_!=&frontend||
       frontend.materialApplicationOwner!=this||!sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x29B638,"Garage cleanup requires the exact failed first shared prompt, not a later Page");
    auto& s=*owner;const auto resolver=s.managerDomainLease_->resolver3E1A70();
    rr::assets::OriginalStandardTextureDiagnostic50LA bank;
    if(s.sharedPageAbortPhase_<P::bankRetired&&
       (!resolver||!s.managerBankLease_||!resolver->canRetireGarageBank12E5E0(*s.managerBankLease_,bank))){
        diagnostic.texture=bank;return fail(bank.sourcePC?bank.sourcePC:0x12E5E0,"Failed shared Page lost its real releasable base bank");
    }
    if(s.sharedPageAbortPhase_==P::notStarted){
        // Every actual owner is inspected before Camera/embedded payload teardown.
        if(!s.preflightSharedPageFailure(diagnostic))
            return custodyFailure(error,"Failed shared Page all-owner retirement preflight refused");
        if(!frontend.garageTransferOwnerCurrent(s.grant_))return fail(0x2543F0,"Failed shared Page transfer owner changed");
        if(!s.audio_->preflightSharedPageEntryFailure(owner,error)){
            diagnostic.status=Status::phase;diagnostic.sourcePC=0x235150;return false;
        }
        const auto receipt=s.scope_->beginFailedGarageSharedLoaderRetirement(
            s.entryManagerGeneration_,&s,s.page_.get(),s.page_->entity_);
        if(!receipt.okay())return fail(receipt.sourcePC,"Failed shared Page custody receipt refused before destruction");
        s.sharedPageAbortFrontend_=&frontend;s.retiring_=true;s.sharedPageAbortPhase_=P::pageRetiring;
    }
    if(s.sharedPageAbortPhase_==P::pageRetiring){
        rr::platform::OriginalUiGaragePageOwner50KG::Diagnostic page;
        if(!s.page_->retireFailedSharedPrompt(page))return fail(page.sourcePC,"Failed shared Page exact payload retirement remains pending");
        s.sharedPageAbortPhase_=P::pageRetired;
    }
    if(s.sharedPageAbortPhase_==P::pageRetired){
        rr::platform::OriginalUiGarageManagerFog50KG::Diagnostic fog;
        if(!s.entryFog_->retireAttached1CD080(fog))return fail(fog.sourcePC,"Failed shared Page Fog tail retirement refused");
        s.sharedPageAbortPhase_=P::fogRetired;
    }
    if(s.sharedPageAbortPhase_==P::fogRetired){
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG widget;
        if(!s.widgets_->retireDetached2A8F70(widget))return fail(widget.sourcePC,"Failed shared Page Widget/Watto retirement remains pending");
        s.sharedPageAbortPhase_=P::widgetsRetired;
    }
    if(s.sharedPageAbortPhase_==P::widgetsRetired){
        rr::platform::OriginalUiGarageManagerFog50KG::Diagnostic fog;
        if(!s.entryFog_->retireDetached1CCFA0(fog))return fail(fog.sourcePC,"Failed shared Page Fog aliases remain retained");
        s.sharedPageAbortPhase_=P::fogAliasesReleased;
    }
    if(s.sharedPageAbortPhase_==P::fogAliasesReleased){
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireLight146980(1,light))return fail(light.sourcePC,"Failed shared Page directional Light retirement refused");
        s.sharedPageAbortPhase_=P::directionalRetired;
    }
    if(s.sharedPageAbortPhase_==P::directionalRetired){
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireLight146980(0,light))return fail(light.sourcePC,"Failed shared Page ambient Light retirement refused");
        s.sharedPageAbortPhase_=P::ambientRetired;
    }
    if(s.sharedPageAbortPhase_==P::ambientRetired){
        rr::platform::OriginalUiGarageManagerSearch50KG::Diagnostic search;
        if(!s.entrySearch_->retireNode27F180(search))return fail(search.sourcePC,"Failed shared Page Search node retirement refused");
        s.sharedPageAbortPhase_=P::searchNodeRetired;
    }
    if(s.sharedPageAbortPhase_==P::searchNodeRetired){
        if(!s.preflightAttachedSelectedAbort(diagnostic))return custodyFailure(error,"Failed shared Page selected preflight refused");
        // This existing destructor is not restartable after a partial failure.
        s.sharedPageAbortPhase_=P::selectedRetiring;
        if(!s.entrySelected_->close1D18D0(diagnostic))return custodyFailure(error,"Failed shared Page selected destructor stopped; no replay");
        s.sharedPageAbortPhase_=P::selectedRetired;
    }
    if(s.sharedPageAbortPhase_==P::selectedRetiring)return fail(0x142590,"Failed selected destructor retains its exact irreversible prefix");
    if(s.sharedPageAbortPhase_==P::selectedRetired){
        const auto step=s.grant_->beginGarageManagerPayloadRetirement(s.entrySearch_.get());
        if(!step.okay())return fail(step.sourcePC,"Failed shared Page empty manager last-reference release refused");
        s.sharedPageAbortPhase_=P::managerRefReleased;
    }
    if(s.sharedPageAbortPhase_==P::managerRefReleased){
        rr::platform::OriginalUiFrontendDiagnostic50KG transfer;
        if(!frontend.retireFailedSharedPageTransfers(owner,transfer))
            return fail(transfer.sourcePC,"Failed shared Page actual transfer retirement remains pending");
        s.sharedPageAbortPhase_=P::transfersRetired;
    }
    if(s.sharedPageAbortPhase_==P::transfersRetired){
        rr::platform::OriginalUiGarageManagerSearch50KG::Diagnostic search;
        if(!s.entrySearch_->retireManagerList1CCADC(search))return fail(search.sourcePC,"Failed shared Page Search payload retirement refused");
        s.sharedPageAbortPhase_=P::searchPayloadRetired;
    }
    if(s.sharedPageAbortPhase_==P::searchPayloadRetired){
        // Constructor190 is actually null; no invented positive OnTop work.
        if(!s.managerDeferred190Empty_)return fail(0x1CCAF0,"Failed shared Page deferred owner is not empty");
        rr::platform::OriginalUiGarageUiLights50KG::Diagnostic light;
        if(!s.entryLights_->retireManagerDescriptors1CCB20(light))return fail(light.sourcePC,"Failed shared Page Light payload retirement refused");
        s.sharedPageAbortPhase_=P::lightPayloadRetired;
    }
    if(s.sharedPageAbortPhase_==P::lightPayloadRetired){
        if(!s.scope_->failedGarageSharedLoaderPayloadCurrent(s.entryManagerGeneration_,&s))
            return fail(0x12E5E0,"Failed shared Page payload receipt is stale before bank close");
        if(!resolver->retireGarageBank12E5E0(*s.managerBankLease_,bank)){
            diagnostic.texture=bank;return fail(bank.sourcePC,"Failed shared Page bank close refused");
        }
        const auto step=s.scope_->noteFailedGarageBankRetired12E5E0(s.entryManagerGeneration_,&s);
        if(!step.okay())std::terminate();
        s.managerArchiveOnlyScope170_=false;s.sharedPageAbortPhase_=P::bankRetired;
    }
    if(s.sharedPageAbortPhase_==P::bankRetired){
        if(!s.releaseManagerPageTable2A3B04(diagnostic))return custodyFailure(error,"Failed shared Page native Page table release refused");
        s.sharedPageAbortPhase_=P::pageTableRetired;
    }
    if(s.sharedPageAbortPhase_==P::pageTableRetired){
        rr::platform::OriginalUiGarageWidget1Diagnostic50KG widget;
        if(!s.widgets_->retireManagerTable2A3B18(widget))return fail(widget.sourcePC,"Failed shared Page Widget table release refused");
        s.sharedPageAbortPhase_=P::widgetTableRetired;
    }
    if(s.sharedPageAbortPhase_==P::widgetTableRetired){
        const auto step=s.scope_->finishGarageManagerPayloadRetirement(s.entryManagerGeneration_,s.entrySearch_.get());
        if(!step.okay())return fail(step.sourcePC,"Failed shared Page counted manager release refused");
        s.sharedPageAbortPhase_=P::managerRetired;
    }
    if(s.sharedPageAbortPhase_==P::managerRetired){
        if(!s.audio_->retireSharedPageEntryFailure(owner,error)){
            diagnostic.status=Status::phase;diagnostic.sourcePC=0x235150;return false;
        }
        s.sharedPageAbortPhase_=P::audioRetired;
    }
    if(s.sharedPageAbortPhase_!=P::audioRetired)return fail(0x1413E0,"Failed shared Page retirement cursor is not terminal");
    // Actual dead Watto/source records stayed alive through Main's typed commit.
    // This failed constructor remains terminal-only, never a normal Back receipt.
    s.page_.reset();s.watto_.reset();s.widgets_.reset();s.entryFog_.reset();s.entryLights_.reset();s.entrySearch_.reset();
    s.entrySelected_.reset();s.actorInputs_.reset();s.wattoInputs_.reset();s.resources_.reset();
    s.managerDat298_.reset();s.managerDrivePageData_.reset();s.managerDriveProfile_.reset();s.entryFonts_.reset();s.entryTiming_={};
    s.wattoAssets_.reset();s.sceneAssets_.reset();s.managerBankLease_.reset();
    s.managerArchiveLease_.reset();s.managerProviderLease_.reset();s.managerDomainLease_.reset();
    s.audio_.reset();s.pending_.reset();s.application_.reset();s.motions_.reset();s.authority_.reset();
    s.grant_.reset();s.scope_.reset();s.entryFrontend_=nullptr;
    s.sharedPageAbortPhase_=P::closed;s.closed_=true;garageCustodyOwner_.reset();error.clear();return true;
}
bool Application::retireGarageEntryBeforeDat(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using P=Custody::BaseEntryAbortPhase;using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    diagnostic={};const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=Status::phase;diagnostic.sourcePC=pc;return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||
       !owner->baseInputsFailurePrefixCurrent(*this)||owner->entryFrontend_!=&frontend||
       frontend.materialApplicationOwner!=this||!sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x1CCC84,"Garage base-input cleanup requires its exact selected-complete, before-DAT failed prefix");
    auto& custody=*owner;
    if(custody.baseEntryAbortFrontend_&&custody.baseEntryAbortFrontend_!=&frontend)
        return fail(0x2543F0,"Garage base-input cleanup cannot change Frontend owners");
    const auto resolver=custody.managerDomainLease_->resolver3E1A70();
    rr::assets::OriginalStandardTextureDiagnostic50LA bank;
    if(custody.baseEntryAbortPhase_<=P::transfersRetired&&
       (!resolver||!custody.managerBankLease_||
        !resolver->canRetireGarageBank12E5E0(*custody.managerBankLease_,bank))){
        diagnostic.texture=bank;
        return fail(bank.sourcePC?bank.sourcePC:0x12E5E0,"Garage base-input cleanup lost its exact releasable base bank");
    }
    if(custody.baseEntryAbortPhase_==P::notStarted){
        // Exhaustive reads precede the first destructive selected callback.
        // Main authenticates its actual empty constructing entry independently.
        if(!custody.preflightBaseSelectedAbort(diagnostic))
            return custodyFailure(error,"Garage base-input selected retirement preflight refused; no owner discarded");
        if(!frontend.garageTransferOwnerCurrent(custody.grant_))
            return fail(0x2543F0,"Garage base-input cleanup lacks its actual ready transfer generation");
        if(!custody.audio_->preflightEmptyGarageEntryFailure(owner,error)){
            diagnostic.status=Status::phase;diagnostic.sourcePC=0x235150;return false;
        }
        custody.baseEntryAbortFrontend_=&frontend;custody.retiring_=true;
        if(!custody.retireBaseSelectedAbort(diagnostic))return false;
    }
    if(custody.baseEntryAbortPhase_==P::selectedRetired){
        rr::platform::OriginalUiFrontendDiagnostic50KG transfer;
        if(!frontend.retireFailedGarageEntryTransfers(custody.grant_,transfer))
            return fail(transfer.sourcePC?transfer.sourcePC:0x2543F0,"Garage base-input native transfer cleanup pending; prefix retained");
        custody.baseEntryAbortPhase_=P::transfersRetired;
    }
    if(custody.baseEntryAbortPhase_==P::transfersRetired){
        if(!resolver->retireGarageBank12E5E0(*custody.managerBankLease_,bank)){
            diagnostic.texture=bank;
            return fail(bank.sourcePC?bank.sourcePC:0x12E5E0,"Garage base-input bank cleanup pending; manager retained");
        }
        custody.managerBankLease_.reset();custody.managerArchiveOnlyScope170_=false;
        custody.baseEntryAbortPhase_=P::bankRetired;
    }
    if(custody.baseEntryAbortPhase_==P::bankRetired){
        const auto step=custody.scope_->releaseGarageManager(custody.entryManagerGeneration_);
        if(!step.okay())return fail(step.sourcePC,"Garage base-input empty manager cleanup pending");
        custody.baseEntryAbortPhase_=P::managerRetired;
    }
    if(custody.baseEntryAbortPhase_==P::managerRetired){
        if(!custody.audio_->retireEmptyGarageEntryFailure(owner,error)){
            diagnostic.status=Status::phase;diagnostic.sourcePC=0x235150;return false;
        }
        custody.baseEntryAbortPhase_=P::audioRetired;
    }
    if(custody.baseEntryAbortPhase_!=P::audioRetired)
        return fail(0x1413E0,"Garage base-input cleanup is out of order");
    // The selected source owner, bank, counted manager and empty Main entry
    // have genuinely returned. Only now remove native capsule aliases.
    custody.entrySelected_.reset();custody.resources_.reset();
    custody.managerProviderLease_.reset();custody.managerDomainLease_.reset();custody.managerArchiveLease_.reset();
    custody.audio_.reset();custody.pending_.reset();custody.sceneAssets_.reset();
    custody.application_.reset();custody.motions_.reset();custody.authority_.reset();
    custody.grant_.reset();custody.scope_.reset();custody.entryFrontend_=nullptr;
    custody.baseEntryAbortPhase_=P::closed;custody.closed_=true;garageCustodyOwner_.reset();
    error.clear();return true;
}
bool Application::retireGarageEntryBeforeSelected(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using P=Custody::EntryAbortPhase;using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    diagnostic={};const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=Status::phase;diagnostic.sourcePC=pc;return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||!owner->sameRoot(*this)||
       owner->entryFrontend_!=&frontend||frontend.materialApplicationOwner!=this||
       !sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x1D1CD0,"Failed Garage entry cleanup requires its exact live Application, Frontend and capsule");
    auto& custody=*owner;
    // These two observed cache boundaries precede resolver binding, source
    // corpus append and selected2403A0. Later partial resources need their own
    // authentic cleanup and are deliberately NOT admitted by this route.
    const bool cacheFailure=(custody.entryFailureSourcePC_==0x12CEA0&&custody.entryFailureStatus_==Status::phase)||
        (custody.entryFailureSourcePC_==0x24F178&&custody.entryFailureStatus_==Status::texture);
    if(!custody.failed_||!cacheFailure||custody.managerPublished1B8274_||
       custody.managerEntryPhase_!=Custody::ManagerEntryPhase::selectedBankOpened1D1D68||
       custody.managerConstructorPhase_!=Custody::ManagerConstructorPhase::notStarted||
       custody.managerDrivePhase_!=Custody::ManagerDrivePhase::notStarted||
       custody.managerLoadPhase_!=Custody::ManagerLoadPhase::notStarted||
       custody.normalRetirementPhase_!=Custody::NormalRetirementPhase::notStarted||
       !custody.resources_||custody.resources_->issuer_!=this||
       custody.resources_->generation_!=custody.entryManagerGeneration_||
       custody.resources_->domain_!=standardMaterialDomainOwner_||
       custody.resources_->context_!=materialContextOwner_||!custody.resources_->population_.empty()||
       custody.entrySelected_||custody.entrySearch_||custody.entryLights_||custody.entryFog_||
       custody.widgets_||custody.watto_||custody.actors_||custody.visibility_||custody.page_||custody.podScene_||
       custody.audio_||custody.pending_||custody.actorInputs_||custody.wattoInputs_||
       custody.managerDat298_||custody.managerArchiveOnlyScope170_||custody.managerBankLease_||
       custody.managerArchiveLease_||custody.managerProviderLease_||custody.managerDomainLease_||
       custody.managerPageTable144_||custody.managerPage144Known_||custody.manager150Known_||
       custody.managerCurrentPage14C_||custody.managerPageCount148_||!custody.managerDeferred190Empty_||
       custody.runtimeFrame_||custody.drawContribution_||custody.pageAttempted_||custody.pageSceneAttempted_||
       custody.actorsAttempted_||custody.wattoAttempted_||!custody.scope_->live().okay()||
       custody.scope_->managerPublished_||custody.scope_->managerForestRetiring_||
       custody.scope_->managerRetiring_||custody.scope_->managerSearchOwner_||
       custody.scope_->dispatcherCurrentManager30_||!custody.scope_->garageChildren_.empty())
        return fail(0x1D1CD0,"Failed Garage prefix is not the exact empty pre-selected cache boundary; no owner discarded");
    if(custody.entryAbortFrontend_&&custody.entryAbortFrontend_!=&frontend)
        return fail(0x2543F0,"Failed Garage cleanup cannot change Frontend owners");
    if(!custody.grant_||!custody.grant_->current()||
       custody.grant_->generation_!=custody.entryManagerGeneration_||
       custody.scope_->managerGeneration_!=custody.entryManagerGeneration_||
       !custody.scope_->managerLive_||custody.scope_->manager_.reference00!=1||
       custody.scope_->manager_.parent1C||custody.scope_->manager_.previous10||
       custody.scope_->manager_.next14||custody.scope_->manager_.firstChild18)
        return fail(0x142590,"Failed Garage cleanup lost its actual detached childless manager");
    const auto resolver=standardMaterialDomainOwner_?standardMaterialDomainOwner_->resolver3E1A70():nullptr;
    rr::assets::OriginalStandardTextureDiagnostic50LA bank;
    if(custody.entryAbortPhase_<=P::transfersRetired&&
       (!resolver||!custody.selectedBankLease_||!custody.selectedArchiveOnlyScope1D1D88_||
        !resolver->canRetireGarageBank12E5E0(*custody.selectedBankLease_,bank))){
        diagnostic.texture=bank;
        return fail(bank.sourcePC?bank.sourcePC:0x12E5E0,"Failed Garage cleanup lacks its exact releasable selected19E lease");
    }
    custody.retiring_=true;custody.entryAbortFrontend_=&frontend;
    if(custody.entryAbortPhase_==P::notStarted){
        rr::platform::OriginalUiFrontendDiagnostic50KG transfer;
        if(!frontend.retireFailedGarageEntryTransfers(custody.grant_,transfer))
            return fail(transfer.sourcePC?transfer.sourcePC:0x2543F0,"Failed Garage native transfer cleanup pending; exact prefix retained");
        custody.entryAbortPhase_=P::transfersRetired;
    }
    if(custody.entryAbortPhase_==P::transfersRetired){
        if(!resolver->retireGarageBank12E5E0(*custody.selectedBankLease_,bank)){
            diagnostic.texture=bank;
            return fail(bank.sourcePC?bank.sourcePC:0x12E5E0,"Failed Garage selected19E bank cleanup pending; manager retained");
        }
        custody.selectedBankLease_.reset();custody.selectedArchiveOnlyScope1D1D88_=false;
        custody.entryAbortPhase_=P::selectedBankRetired;
    }
    if(custody.entryAbortPhase_==P::selectedBankRetired){
        const auto step=custody.scope_->releaseGarageManager(custody.entryManagerGeneration_);
        if(!step.okay())return fail(step.sourcePC,"Failed Garage empty manager Entity cleanup pending");
        custody.entryAbortPhase_=P::managerRetired;
    }
    if(custody.entryAbortPhase_!=P::managerRetired)
        return fail(0x1413E0,"Failed Garage native cleanup is out of order");
    // No counted children or per-entry audio aliases were ever created.
    // Release ONLY this capsule's empty native backing. Shared Main/Aural,
    // renderer/FIFO, canonical domain and immutable cache remain with their
    // genuine enclosing owners. Failed entry remains failed and unreplayable.
    custody.resources_.reset();custody.sceneAssets_.reset();
    custody.application_.reset();custody.motions_.reset();custody.authority_.reset();
    custody.grant_.reset();custody.scope_.reset();custody.entryFrontend_=nullptr;
    custody.entryAbortPhase_=P::closed;custody.closed_=true;garageCustodyOwner_.reset();
    error.clear();return true;
}
bool Application::retireGaragePendingRetainFailure(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using P=Custody::PendingRetainAbortPhase;using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    diagnostic={};const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=Status::phase;diagnostic.sourcePC=pc;return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||
       !owner->pendingRetainFailurePrefixCurrent(*this)||owner->entryFrontend_!=&frontend||
       frontend.materialApplicationOwner!=this||!sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x1CCC84,"Garage Pending-retain cleanup requires its exact before-Main-entry failure");
    auto& custody=*owner;
    if(custody.pendingRetainAbortFrontend_&&custody.pendingRetainAbortFrontend_!=&frontend)
        return fail(0x2543F0,"Garage Pending-retain cleanup cannot change Frontend");
    //This existing read-only borrow authenticates the same persistent backing.
    //It deliberately does not require or create the missing Pending namespace,
    //nor enter/retire Main's per-Garage source-construction state.
    std::shared_ptr<rr::platform::OriginalUiMainAudio50KG> actual;
    if(!retainGarageAudioTransportCold(actual,error)||!sameCustodyShared(actual,custody.audio_))
        return fail(0x1CCC84,"Garage Pending-retain cleanup lost its original persistent Main alias");
    if(custody.pendingRetainAbortPhase_==P::notStarted){
        if(custody.retiring_)
            return fail(0x1D18D0,"Garage Pending-retain selected destructor already attempted; failed prefix retained");
        if(!custody.preflightAttachedSelectedAbort(diagnostic))
            return custodyFailure(error,"Garage Pending-retain selected destructor preflight rejected; owners retained");
        if(!frontend.garageTransferOwnerCurrent(custody.grant_))
            return fail(0x2543F0,"Garage Pending-retain transfer owner is not current; selected model retained");
        custody.retiring_=true;custody.pendingRetainAbortFrontend_=&frontend;
        if(!custody.retirePendingSelectedAbort(diagnostic))
            return custodyFailure(error,"Garage Pending-retain selected destructor failed; exact prefix retained");
    }
    if(custody.pendingRetainAbortPhase_==P::selectedRetired){
        rr::platform::OriginalUiFrontendDiagnostic50KG transfer;
        if(!frontend.retireFailedGarageEntryTransfers(custody.grant_,transfer))
            return fail(transfer.sourcePC?transfer.sourcePC:0x2543F0,"Garage Pending-retain transfer cleanup pending");
        custody.pendingRetainAbortPhase_=P::transfersRetired;
    }
    if(custody.pendingRetainAbortPhase_==P::transfersRetired){
        //The real selected19E close already returned before this failure;
        //no base bank was opened. Do not fabricate another12E5E0 call.
        const auto step=custody.scope_->releaseGarageManager(custody.entryManagerGeneration_);
        if(!step.okay())return fail(step.sourcePC,"Garage Pending-retain empty manager release pending");
        custody.pendingRetainAbortPhase_=P::managerRetired;
    }
    if(custody.pendingRetainAbortPhase_!=P::managerRetired||
       !custody.pendingRetainFailurePrefixCurrent(*this))
        return fail(0x1413E0,"Garage Pending-retain native cleanup is out of order");
    //Only this Session's shared alias is dropped. Main's source lifetime,
    //persistent backing, namespace publication, Aural and PCM are untouched.
    custody.audio_.reset();custody.entrySelected_.reset();custody.resources_.reset();custody.sceneAssets_.reset();
    custody.application_.reset();custody.motions_.reset();custody.authority_.reset();
    custody.grant_.reset();custody.scope_.reset();custody.entryFrontend_=nullptr;
    custody.pendingRetainAbortPhase_=P::closed;custody.closed_=true;garageCustodyOwner_.reset();
    error.clear();return true;
}
bool Application::retireGarageUnenrolledSelectedEntry(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using P=Custody::UnenrolledSelectedAbortPhase;using Status=rr::assets::OriginalStandardMaterialStatus50LA;
    diagnostic={};const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=Status::phase;diagnostic.sourcePC=pc;return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||
       !owner->unenrolledSelectedFailurePrefixCurrent(*this)||owner->entryFrontend_!=&frontend||
       frontend.materialApplicationOwner!=this||!sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x2403A0,"Garage failed enrollment cleanup requires its exact complete unpublished selected model");
    auto& custody=*owner;
    if(custody.unenrolledSelectedAbortFrontend_&&custody.unenrolledSelectedAbortFrontend_!=&frontend)
        return fail(0x2543F0,"Garage failed enrollment cleanup cannot change Frontend");
    const auto resolver=standardMaterialDomainOwner_?standardMaterialDomainOwner_->resolver3E1A70():nullptr;
    rr::assets::OriginalStandardTextureDiagnostic50LA bank;
    if(custody.unenrolledSelectedAbortPhase_<P::bankRetired&&
       (!resolver||!custody.selectedBankLease_||
        !resolver->canRetireGarageBank12E5E0(*custody.selectedBankLease_,bank))){
        diagnostic.texture=bank;
        return fail(bank.sourcePC?bank.sourcePC:0x12E5E0,"Garage failed enrollment lost its actual selected19E bank");
    }
    if(custody.unenrolledSelectedAbortPhase_==P::notStarted){
        //All owners are preflighted before the first model reference release.
        if(!custody.preflightUnenrolledSelectedAbort(diagnostic))
            return custodyFailure(error,"Garage unpublished selected destructor preflight rejected; all owners retained");
        if(!frontend.garageTransferOwnerCurrent(custody.grant_))
            return fail(0x2543F0,"Garage failed enrollment transfer owner is not current; model retained");
        custody.retiring_=true;custody.unenrolledSelectedAbortFrontend_=&frontend;
        if(!custody.retireUnenrolledSelectedAbort(diagnostic))
            return custodyFailure(error,"Garage unpublished selected destructor failed; exact prefix retained");
        custody.unenrolledSelectedAbortPhase_=P::selectedRetired;
    }
    if(custody.unenrolledSelectedAbortPhase_==P::selectedRetired){
        rr::platform::OriginalUiFrontendDiagnostic50KG transfer;
        if(!frontend.retireFailedGarageEntryTransfers(custody.grant_,transfer))
            return fail(transfer.sourcePC?transfer.sourcePC:0x2543F0,"Garage failed enrollment transfer cleanup pending");
        custody.unenrolledSelectedAbortPhase_=P::transfersRetired;
    }
    if(custody.unenrolledSelectedAbortPhase_==P::transfersRetired){
        if(!resolver->retireGarageBank12E5E0(*custody.selectedBankLease_,bank)){
            diagnostic.texture=bank;
            return fail(bank.sourcePC?bank.sourcePC:0x12E5E0,"Garage failed enrollment selected19E close pending");
        }
        custody.selectedBankLease_.reset();custody.selectedArchiveOnlyScope1D1D88_=false;
        custody.unenrolledSelectedAbortPhase_=P::bankRetired;
    }
    if(custody.unenrolledSelectedAbortPhase_==P::bankRetired){
        const auto step=custody.scope_->releaseGarageManager(custody.entryManagerGeneration_);
        if(!step.okay())return fail(step.sourcePC,"Garage failed enrollment empty manager release pending");
        custody.unenrolledSelectedAbortPhase_=P::managerRetired;
    }
    if(custody.unenrolledSelectedAbortPhase_!=P::managerRetired||
       !custody.unenrolledSelectedFailurePrefixCurrent(*this))
        return fail(0x1413E0,"Garage failed enrollment native cleanup is out of order");
    //No per-entry Main state was created. Leave persistent Main/Aural/PCM and
    //the canonical Domain alone. The Geometry died before this bank closed;
    //its retired compressed PSG bytes intentionally remain until actual Domain
    //destruction on this terminal failure, not a forged normal-manager receipt.
    custody.entrySelected_.reset();custody.resources_.reset();custody.sceneAssets_.reset();
    custody.application_.reset();custody.motions_.reset();custody.authority_.reset();
    custody.grant_.reset();custody.scope_.reset();custody.entryFrontend_=nullptr;
    custody.unenrolledSelectedAbortPhase_=P::closed;custody.closed_=true;garageCustodyOwner_.reset();
    error.clear();return true;
}
bool Application::retireGarageUnsealedRenderFailure(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;using P=Custody::NormalRetirementPhase;
    diagnostic={};const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;diagnostic.sourcePC=pc;
        return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||!owner->sameRoot(*this)||
       !owner->hasUnsealedRenderFailure()||owner->entryFrontend_!=&frontend||
       !sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x140230,"Unsealed render cleanup cannot adopt another failed Draw or publication");
    auto& s=*owner;
    if(s.normalRetirementPhase_==P::notStarted){
        if(!frontend.garageTransferOwnerCurrent(s.grant_)||
           !s.prepareUnsealedRenderFailureRetirement(*this,diagnostic,error))return false;
        s.normalFrontend_=&frontend;s.retiring_=true;s.normalRetirementPhase_=P::dispatcherPrefix;
    }
    if(!s.unsealedRenderFailureRetirementCurrent(*this))
        return fail(s.unsealedRenderSourcePC_,"Unsealed render cleanup lost its actual native release/manager cursor");
    if(s.normalRetirementPhase_==P::dispatcherPrefix&&
       (!s.audio_||!s.audio_->drainUnsealedRenderEntryFailure(owner,error))){
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
        diagnostic.sourcePC=s.unsealedRenderSourcePC_;return false;
    }
    return retireGarageManagerNormal2A3510(frontend,diagnostic,error);
}
bool Application::retireGarageActorSampleFailure(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;using P=Custody::NormalRetirementPhase;
    diagnostic={};const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;diagnostic.sourcePC=pc;
        return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||!owner->sameRoot(*this)||
       !owner->hasActorSampleFailure()||owner->entryFrontend_!=&frontend||
       !sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x10179C,"Partial Actor cleanup cannot adopt a different failed callback or publication");
    auto& s=*owner;
    if(s.normalRetirementPhase_==P::notStarted){
        if(!frontend.garageTransferOwnerCurrent(s.grant_)||
           !s.prepareActorSampleFailureRetirement(*this,diagnostic,error))return false;
        s.normalFrontend_=&frontend;s.retiring_=true;s.normalRetirementPhase_=P::dispatcherPrefix;
    }
    if(!s.actorSampleFailureRetirementCurrent(*this))
        return fail(0x10179C,"Partial Actor cleanup lost its exact entered owner retirement cursor");
    if(s.normalRetirementPhase_==P::dispatcherPrefix){
        // Native terminal-only service of already accepted ordered controls.
        // No guest267BE0, time/age/slot reset, sample flush, or PCM advancement.
        if(!s.audio_||!s.audio_->drainActorSampleEntryFailure(owner,error)){
            diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
            diagnostic.sourcePC=0x10179C;return false;
        }
    }
    return retireGarageManagerNormal2A3510(frontend,diagnostic,error);
}
bool Application::retireGaragePublishedObservationFailure(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;using P=Custody::NormalRetirementPhase;
    diagnostic={};const auto owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;diagnostic.sourcePC=pc;
        return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||!owner->sameRoot(*this)||
       !owner->hasPublishedObservationFailure()||owner->entryFrontend_!=&frontend||
       !sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x1B8274,"Published observation cleanup cannot adopt a different failure or manager");
    auto& s=*owner;
    if(s.normalRetirementPhase_==P::notStarted){
        if(!frontend.garageTransferOwnerCurrent(s.grant_)||
           !s.preparePublishedObservationFailureRetirement(*this,diagnostic,error))return false;
        // Only native cleanup admission advances here. Failed load/frame and
        // Back/reentry histories remain unchanged. The complete source owner
        // topology permits the existing real dispatcher/destructor sequence.
        s.normalFrontend_=&frontend;s.retiring_=true;s.normalRetirementPhase_=P::dispatcherPrefix;
    }
    if(!s.publishedObservationFailureRetirementCurrent(*this))
        return fail(0x1B8274,"Published observation cleanup lost its exact entered source retirement cursor");
    if(s.normalRetirementPhase_==P::dispatcherPrefix){
        // Native terminal-failure cleanup only: service the real accepted
        // ordered controls after admission, before any child teardown. This
        // is not a guest267BE0 call, sample flush, clock advance or PCM drain.
        // Main validates actual applied rows on retry, never replays them.
        if(!s.audio_||!s.audio_->drainPublishedObservationEntryFailure(owner,error)){
            diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
            diagnostic.sourcePC=0x1B8274;return false;
        }
    }
    return retireGarageManagerNormal2A3510(frontend,diagnostic,error);
}
bool Application::retireGarageManagerNormal2A3510(Frontend& frontend,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    using Custody=rr::platform::OriginalUiGarageSession50KG;
    using P=Custody::NormalRetirementPhase;diagnostic={};
    const auto& owner=garageCustodyOwner_;
    auto fail=[&](std::uint32_t pc,const char* text)noexcept {
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
        diagnostic.sourcePC=pc;return custodyFailure(error,text);
    };
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||!owner->sameRoot(*this)||
       !sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return fail(0x2A3510,"Normal Garage Back requires this live Application and its actual shared dispatcher");
    auto& custody=*owner;
    if(custody.unsealedRenderRetirement_&&!custody.unsealedRenderFailureRetirementCurrent(*this))
        return fail(custody.unsealedRenderSourcePC_,"Unsealed render retirement requires its distinct entered native-release identity");
    if(custody.actorSampleFailed10179C_&&!custody.actorSampleFailureRetirementCurrent(*this))
        return fail(0x10179C,"Actor sample retirement requires its distinct entered failure identity");
    if(custody.publishedObservationFailed1B8274_&&
       !custody.publishedObservationFailureRetirementCurrent(*this))
        return fail(0x1B8274,"Published observation retirement requires its distinct entered failure identity");
    if(custody.normalRetirementPhase_==P::notStarted){
        if((custody.entryFrontend_&&custody.entryFrontend_!=&frontend)||
           !frontend.garageTransferOwnerCurrent(custody.grant_))
            return fail(0x2543F0,"Normal Garage Back requires the same actual Frontend transfer owner before detach");
        if(!custody.prepareNormalRetirement(*this,diagnostic,error))return false;
        custody.normalFrontend_=&frontend;custody.retiring_=true;
        custody.normalRetirementPhase_=P::dispatcherPrefix;
    }
    if(custody.normalFrontend_!=&frontend)
        return fail(0x2543F0,"Normal Garage Back cannot switch Frontend owners during retirement");
    if(custody.normalRetirementPhase_==P::currentCleared)
        return releaseGarageCustodyNormal(diagnostic,error);
    if(custody.normalRetirementPhase_==P::dispatcherPrefix){
        rr::platform::VehicleWindowDiagnostic50DS display;
        //Global row history was issued at startup, before the shared dialogs.
        //Do not rerun that loader here or substitute the popup widget table.
        auto step=continueDispatcherTransitionPrefix2A3510(custody.normalManagerGeneration_,display);
        if(!step.okay())return fail(step.sourcePC,"Normal Garage dispatcher Display/global-widget prefix is pending");
        step=custody.scope_->detachPopulatedGarageManager2A35A4(custody.normalManagerGeneration_);
        if(!step.okay())return fail(step.sourcePC,"Normal Garage actual manager detach refused");
        custody.normalRetirementPhase_=P::managerDetached;
    }
    if(custody.normalRetirementPhase_>=P::managerDetached&&custody.normalRetirementPhase_<P::managerRefReleased)
        if(!custody.retireNormalChildren142590(diagnostic,error))return false;
    if(custody.normalRetirementPhase_==P::managerRefReleased){
        rr::platform::OriginalUiFrontendDiagnostic50KG transfer;
        if(!frontend.retireGarageTransfers2543F0(custody.grant_,transfer))
            return fail(transfer.sourcePC?transfer.sourcePC:0x2543F0,"Normal Garage native transfer retirement is pending; manager payload retained");
        custody.normalRetirementPhase_=P::transfersRetired;
    }
    if(custody.normalRetirementPhase_>=P::transfersRetired&&custody.normalRetirementPhase_<P::lightPayloadRetired)
        if(!custody.retireNormalManagerPayload1CCADC(diagnostic,error))return false;
    if(custody.normalRetirementPhase_==P::lightPayloadRetired){
        if(!custody.scope_->garageBankRetirement12E5E0Current(custody.normalManagerGeneration_,&custody)||
           !custody.managerBankLease_||!custody.managerDomainLease_||
           custody.managerDomainLease_!=standardMaterialDomainOwner_||
           !custody.managerDomainLease_->resolver3E1A70())
            return fail(0x12E5E0,"Normal Garage bank retirement lacks its exact lease or manager payload interval");
        rr::assets::OriginalStandardTextureDiagnostic50LA bank;
        if(!custody.managerDomainLease_->resolver3E1A70()->retireGarageBank12E5E0(*custody.managerBankLease_,bank))
            return fail(bank.sourcePC?bank.sourcePC:0x12E5E0,"Normal Garage bank retirement is pending; Page and Widget arrays retained");
        const auto note=custody.scope_->noteGarageBankRetired12E5E0(custody.normalManagerGeneration_,&custody);
        if(!note.okay())std::terminate(); //Same-thread prevalidated exact native release.
        custody.normalRetirementPhase_=P::bankRetired;
    }
    if(!custody.finishNormalManager2A3B04(diagnostic,error))return false;
    return releaseGarageCustodyNormal(diagnostic,error);
}
bool Application::releaseGarageCustodyNormal(
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    diagnostic={};
    // Own this native capsule across removal from the root slot. Native alias
    // release below cannot invalidate the active function's reference.
    const auto owner=garageCustodyOwner_;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!owner||!owner->sameRoot(*this)||
       !sameCustodyShared(owner->scope_,sharedUiScopeOwner_))
        return custodyFailure(error,"Normal Garage native release requires this exact live root capsule");
    if(!owner->normalSourceOwnersRetired(diagnostic,error))return false;
    // Native frame/display/RootInputs aliases outlive incomplete callbacks,
    // but not the exact completed manager epoch. This does not commit or
    // reset SourceTiming, nor invent acceptance of a failed source frame.
    if(!owner->actorSampleFailed10179C_&&!owner->unsealedRenderRetirement_){
        owner->runtimeFrame_.reset();owner->runtimeManagerChild_=nullptr;
    }
    // Real audio owner continuation: release retired Scene routing custody,
    // then rearm only its retired native per-entry state. Application Main,
    // mixer, shared sample namespace and canonical material domain survive.
    if(owner->unsealedRenderRetirement_){
        if(!owner->unsealedRenderFailureRetirementCurrent(*this)||!owner->audio_)
            return custodyFailure(error,"Unsealed render terminal audio lost actual completed source owners");
        if(!owner->unsealedRenderAudioRetired_){
            if(!owner->audio_->retireUnsealedRenderEntryFailure(owner,error)){
                diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
                diagnostic.sourcePC=0x235150;return false;
            }
            owner->unsealedRenderAudioRetired_=true;
            owner->runtimeFrame_.reset();owner->unsealedRenderFrameReleased_=true;
        }
    }else if(owner->actorSampleFailed10179C_){
        if(!owner->actorSampleFailureRetirementCurrent(*this)||!owner->audio_)
            return custodyFailure(error,"Actor sample terminal audio lost actual completed source owners");
        if(!owner->actorSampleAudioRetired_){
            if(!owner->audio_->retireActorSampleEntryFailure(owner,error)){
                diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
                diagnostic.sourcePC=0x235150;return false;
            }
            owner->actorSampleAudioRetired_=true;
            // The Actor copy ended only at101FE0. The final retained Frame
            //and its Display pin now end AFTER actual terminal audio release.
            owner->runtimeFrame_.reset();owner->actorSampleFrameReleased_=true;
        }
    }else if(owner->publishedObservationFailed1B8274_){
        if(!owner->publishedObservationFailureRetirementCurrent(*this)||!owner->audio_)
            return custodyFailure(error,"Published observation terminal audio lost actual completed source owners");
        if(!owner->publishedObservationAudioRetired_){
            if(!owner->audio_->retirePublishedObservationEntryFailure(owner,error)){
                diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
                diagnostic.sourcePC=0x235150;return false;
            }
            owner->publishedObservationAudioRetired_=true;
        }
    }else if(!rearmGarageAudioAfterManagerRetirementCold(owner->page_,owner->normalManagerGeneration_,error)){
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
        diagnostic.sourcePC=0x235150;return false;
    }
    if(owner->page_->sceneBacking_||owner->page_->retiredSceneBacking_||
       !owner->audio_||owner->audio_->nativeGroupControlCount_){
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::phase;
        diagnostic.sourcePC=0x1FA2AC;
        return custodyFailure(error,"Normal Garage audio still retains its retired Page/Scene routing owner");
    }
    // Native inverse-order cleanup: actual Geometry destruction may precede
    // the base bank close. Source callbacks, transfers/FIFO/renderer and the
    // audio aliases above are already retired; require BOTH exact historical
    // receipts now, never reclaim merely because12E5E0 removed an association.
    if(!owner->managerBankLease_||!owner->managerDomainLease_||
       owner->managerDomainLease_!=standardMaterialDomainOwner_||
       !owner->managerDomainLease_->resolver3E1A70()){
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::phase;
        diagnostic.sourcePC=0x12EB60;
        return custodyFailure(error,"Normal Garage PSG cleanup lacks its exact closed-bank owner");
    }
    rr::assets::OriginalStandardTextureDiagnostic50LA backing;
    if(!owner->managerDomainLease_->resolver3E1A70()->reclaimGaragePsgBackingAfterManagerRetirement(
        *owner->managerBankLease_,backing)){
        diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
        diagnostic.sourcePC=backing.sourcePC;diagnostic.texture=backing;
        return custodyFailure(error,"Normal Garage PSG backing cleanup retained after failed lifetime/accounting preflight");
    }
    // Source callbacks were proved before ANY native destruction. These are
    // shared/unique backing aliases only, never substitutes for142590, bank
    // retirement or the existing cold-abort cleanup path.
    if(owner->actorSampleFailed10179C_||owner->unsealedRenderRetirement_){
        owner->page_->updateChild_=nullptr;owner->runtimeManagerChild_=nullptr;
        owner->runtimeRootChild_=nullptr;owner->runtimeOuterChild_=nullptr;
    }
    if(owner->unsealedRenderRetirement_)owner->drawContribution_.reset();
    owner->page_.reset();owner->actors_.reset();owner->watto_.reset();owner->widgets_.reset();
    owner->entryFog_.reset();owner->entryLights_.reset();owner->entrySearch_.reset();
    owner->entryFonts_.reset();owner->entryTiming_={};
    owner->managerDrivePageData_.reset();owner->managerDriveProfile_.reset();
    owner->normalSelected_.reset();owner->entrySelected_.reset();owner->entryFrontend_=nullptr;
    owner->visibility_.reset();owner->resources_.reset();
    owner->actorInputs_.reset();owner->wattoInputs_.reset();
    owner->sceneAssets_.reset();owner->wattoAssets_.reset();owner->managerBankLease_.reset();
    owner->managerArchiveLease_.reset();owner->managerProviderLease_.reset();owner->managerDomainLease_.reset();
    owner->audio_.reset();owner->pending_.reset();owner->application_.reset();owner->motions_.reset();
    owner->grant_.reset();owner->scope_.reset();
    owner->closed_=true;garageCustodyOwner_.reset();
    // A completed normal epoch, not a failed constructor retry. A new entry
    // must still acquire a new authenticated Scope grant and bank lease.
    // A failed final native observation is terminal-close-only, even after
    // every real source owner retired. It never enables a fresh entry retry.
    if(!owner->publishedObservationFailed1B8274_&&!owner->actorSampleFailed10179C_&&!owner->unsealedRenderRetirement_)garageCustodyAttempted_=false;
    error.clear();return true;
}
bool Application::retireGarageCustodyConsumersCold(
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic,std::string& error)const noexcept {
    diagnostic={};
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return custodyFailure(error,"Garage custody retirement requires actual Application creator thread");
    const auto& owner=garageCustodyOwner_;
    if(!owner||!owner->sameRoot(*this))
        return custodyFailure(error,"Garage custody retirement requires this exact live root");
    auto& custody=*owner;
    if(custody.entryFrontend_)
        return custodyFailure(error,"Source-owned Garage entry cannot use detached cold abort in place of ordered manager/transfer retirement");
    custody.retiring_=true;
    // The successful Page owns its own tail-first child/zone retirement.
    // This cold-prefix path must not mutate those children without that owner.
    if(custody.page_||custody.podScene_)
        return custodyFailure(error,"Garage PodScene Page/config/group retirement is unjoined; whole populated prefix retained");
    if(!custody.ownersClosed_){
        // Discharge accepted native cold controls through their real backend
        // before any source child teardown. Failure retains the whole graph;
        // this is not queue clearing, cancellation or a DSP-history claim.
        if(custody.audio_&&!custody.audio_->serviceColdGarageRouting(error))return false;
        // These children were appended AFTER all widgets. Retire their actual
        // attached suffix before any row22..1 release; payload stays retained.
        if(custody.widgets_&&custody.widgets_->fog_){
            rr::platform::OriginalUiGarageManagerFog50KG::Diagnostic fog;
            if(!custody.widgets_->fog_->retireAttached1CD080(fog)){
                diagnostic.sourcePC=fog.sourcePC;
                diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
                return custodyFailure(error,"Garage appended fog retirement refused; widget rows unchanged");
            }
        }
        if(custody.widgets_&&custody.widgets_->ownsWattoRow(custody.watto_.get())){
            // Once row10 was adopted, the actual manager loop owns the order:
            // tail22..11, Watto10, then prefix9..1. Never close Watto first.
            rr::platform::OriginalUiGarageWidget1Diagnostic50KG widgetDiagnostic;
            if(!custody.widgets_->retireDetached2A8F70(widgetDiagnostic)){
                diagnostic.sourcePC=widgetDiagnostic.sourcePC;
                diagnostic.status=rr::assets::OriginalStandardMaterialStatus50LA::source;
                return custodyFailure(error,"Garage widget-tail retirement refused; custody retained");
            }
        }else if(custody.watto_&&!custody.watto_->closeCold142590(diagnostic))
            return custodyFailure(error,"Garage unadopted Watto retirement refused; whole custody retained");
        custody.ownersClosed_=true;
    }
    if(!custody.consumersClosed_){
        if(custody.managerPage144Known_||custody.managerPageTable144_||custody.manager150Known_)
            return custodyFailure(error,"Cold abort cannot retire a real Page-table/entry prefix");
        //Actual temporary DAT disposal and archive-only scope exit. No bank
        //unmount, widget teardown or completed manager destructor is implied.
        custody.managerDat298_.reset();
        custody.managerArchiveOnlyScope170_=false;
        // Actors close does NOT retire Scene+4D8's counted group or Sounds.
        // No concrete scene-group witness is joined. Keep every populated
        // prefix, even after aggregate native stop/logical close (retired_).
        if(!garageAudioBackingOwner_||
            !garageAudioBackingOwner_->transport_.garageSceneNativeRowsNeverPopulatedCold())
            return custodyFailure(error,"Garage source scene Sound retirement is unresolved; closed consumers and backing retained");
        // No retire2353D0 call or invented witness here. Only genuinely
        // never-populated native rows permit releasing this slice's aliases.
        // The future counted scene owner must establish populated retirement.
        custody.watto_.reset();custody.actors_.reset();
        custody.wattoInputs_.reset();custody.actorInputs_.reset();custody.audio_.reset();
        custody.consumersClosed_=true;
    }
    if(!custody.backingRetired_){
        if(!retireGarageAudioBackingCold(error))return false;
        custody.backingRetired_=true;
    }
    // No source-group retirement claim follows native cleanup of this empty
    // prefix. Visibility/static/selected/widget/late-manager cleanup remains
    // independently required before the capsule itself can be released.
    error.clear();return true;
}
bool Application::releaseRetiredGarageCustodyCold(std::string& error)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return custodyFailure(error,"Garage custody release requires actual Application creator thread");
    const auto& owner=garageCustodyOwner_;
    if(!owner||!owner->sameRoot(*this)||!owner->retiredEnclosingInputs())
        return custodyFailure(error,"Garage custody still retains unresolved enclosing owners; no implicit cleanup");
    owner->closed_=true;garageCustodyOwner_.reset();error.clear();return true;
}
}
