#pragma once
// PROPOSAL: cold numeric reference slice only; no renderer/graph registration API.
#include "original_ui_tournament_career_resources50kg.h"
#include "original_ui_tournament_model_construction50kg.h"
#include "../../../src/assets/original_standard_material_domain50la.h"
#include <cstring>
#include <exception>
namespace original_main_entry50KG { struct Application; }
namespace rr::platform {
class OriginalUiTournamentPodReferences50KG final {
    friend struct ::original_main_entry50KG::Application;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    using S=assets::OriginalStandardMaterialStatus50LA;
    // The private real Application factory establishes object ownership even
    // when stage rejects before numeric effects. This is not material or
    // retirement authority; all stage/context/domain checks remain required.
    explicit OriginalUiTournamentPodReferences50KG(
        const ::original_main_entry50KG::Application& issuer)noexcept:issuer_(&issuer){}
    const ::original_main_entry50KG::Application* const issuer_;
    std::shared_ptr<const OriginalUiTournamentCareerResources50KG> career_;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<assets::OriginalStandardModelResource50LA> model_;
    std::unique_ptr<OriginalUiTournamentModelConstruction50KG> modelConstruction_;
    std::array<std::array<std::int32_t,2>,2> endpoints_{{{{-1,-1}},{{-1,-1}}}};
    std::array<std::int32_t,2> binderRows_{{-1,-1}};
    std::array<bool,2> retained_{{false,false}};
    std::uint64_t providerRevision_=0;
    bool started_=false,complete_=false,released_=false;
    bool constructionEffects()const noexcept{return modelConstruction_&&modelConstruction_->effects();}
    static bool fail(D& d,S s,std::uint32_t pc)noexcept{d.status=s;d.sourcePC=pc;return false;}
    static bool equal(const char* a,const char* b)noexcept {
        for(;*a&&*b;++a,++b){const auto fold=[](char c){return c>='A'&&c<='Z'?char(c+32):c;};
            if(fold(*a)!=fold(*b))return false;}
        return *a==*b;
    }
    // Native original28C590 node,next-sibling,first-child name search. This
    // authenticated cold topology is NOT a registered/renderable model graph.
    bool deriveEndpoints()noexcept {
        const auto count=model_->nodeCount();const auto* rows=model_->sourceNodes();
        if(!rows||!count||count>assets::maxModelNodes49O)return false;
        std::array<std::int32_t,assets::maxModelNodes49O> first,next;
        first.fill(-1);next.fill(-1);
        for(std::uint32_t i=0;i<count;++i){
            const auto p=rows[i].parent;
            if(i==0){if(p!=-1)return false;continue;}
            if(p<0||std::uint32_t(p)>=i)return false;
            if(first[p]<0)first[p]=static_cast<std::int32_t>(i);
            else {auto n=first[p];while(next[n]>=0)n=next[n];next[n]=static_cast<std::int32_t>(i);}
        }
        const auto find=[&](auto&& self,std::int32_t n,const char* name)->std::int32_t {
            if(n<0)return -1;
            if(equal(rows[n].name,name))return n;
            const auto sibling=self(self,next[n],name);
            return sibling>=0?sibling:self(self,first[n],name);
        };
        endpoints_[0]={{find(find,0,"binderleft1"),find(find,0,"binderright1")}};
        endpoints_[1]={{find(find,0,"binderleft2"),find(find,0,"binderright2")}};
        return true;
    }
    bool stage(const ::original_main_entry50KG::Application* issuer,
        std::shared_ptr<const OriginalUiTournamentCareerResources50KG> career,
        std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context,
        std::shared_ptr<game::OriginalApplicationEntities50LA> entities,std::uint64_t generation,
        const std::string& actualDirectory,D& d)noexcept {
        d={};if(started_||released_)return fail(d,S::alreadyPrepared,0x1D3590);
        if(!issuer||issuer_!=issuer||!career||!career->current()||!career->pod()||!career->pod()->loaded()||
           !domain||!domain->prepared()||!context||!context->live()||
           !domain->materials382B38()->screenApplicationMatches(*context->materialApplication())||
           !domain->materials382B38()->canRetireNativeCollector29C300()||
           actualDirectory!="ui\\textures\\")return fail(d,S::phase,0x1D3590);
        const auto archive=career->pod()->archive();const auto member=career->pod()->modelSource();
        const auto provider=domain->archiveProvider3E1A70();
        if(!archive||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::tournament)||
           !member.identity||!member.data||member.size!=member.identity->bytes||!provider||!provider->sealed())
            return fail(d,S::source,0x1D3590);
        try {
            const auto authentic=[&](assets::OriginalUiMemberView50KG source)->bool {
                if(!source.identity||!source.data||source.size!=source.identity->bytes)return false;
                assets::OriginalStandardArchiveProvider50LA::MemberView resolved;
                return provider->lookup132820(source.identity->name,resolved)==assets::OriginalStandardArchiveProvider50LA::Lookup::present&&
                    resolved.data&&resolved.size==source.size&&!std::memcmp(source.data,resolved.data,source.size);
            };
            if(!authentic(member))return fail(d,S::source,0x132820);
            assets::OriginalStandardResourceDiagnostic50LA resource;
            model_=std::make_shared<assets::OriginalStandardModelResource50LA>();
            if(!model_->prepare(member.identity->name,std::vector<std::uint8_t>(member.data,member.data+member.size),resource))
                return fail(d,S::source,resource.sourcePC);
            if(!deriveEndpoints())return fail(d,S::source,0x28C590);
            const bool any=(endpoints_[0][0]>=0&&endpoints_[0][1]>=0)||(endpoints_[1][0]>=0&&endpoints_[1][1]>=0);
            if(any&&(!authentic(archive->find("ui\\textures\\binder.psm"))||
                     !authentic(archive->find("ui\\textures\\binder.atm"))))return fail(d,S::source,0x1D36FC);
            // The exact application domain is pinned before the first numeric effect.
            career_=std::move(career);domain_=std::move(domain);context_=std::move(context);
            providerRevision_=provider->revision();started_=true;
            modelConstruction_=std::unique_ptr<OriginalUiTournamentModelConstruction50KG>(
                new OriginalUiTournamentModelConstruction50KG(std::move(entities),generation));
            if(!modelConstruction_->prepare(model_,domain_->resolver3E1A70(),domain_->materials382B38(),actualDirectory,d))return false;
            const auto table=domain_->materials382B38();
            for(std::size_t pair=0;pair<2;++pair){
                if(endpoints_[pair][0]<0||endpoints_[pair][1]<0)continue;
                //1D36FC/1D3794 both pass genuine3AE4F8 "binder".
                //299BE0 joins actual36B970; this bounded directory has no dot.
                std::string requested=actualDirectory+"binder";
                if(!table->find298C80(requested,binderRows_[pair],d))return false;
                //150514 supplies DefaultMT, not ScreenMT. Every real pair
                //owns one reference even though both resolve the same row.
                d.sourceEffectsCommitted=true;d.storedSlots=pair+1;
                if(!table->retainDefault14D590(binderRows_[pair],d))return false;
                retained_[pair]=true;
            }
            if(!career_->current()||provider->revision()!=providerRevision_)return fail(d,S::phase,0x1D37D4);
            complete_=true;return true;
        }catch(...){return fail(d,S::allocation,0x1D3590);}
    }
    // Detached unpublished model construction and binder numeric refs only.
    // Actual model Entities, geometry cache and CPU packet allocations are
    // owned; no observer/search registration, binder primitive or renderer
    // export exists. This is NOT live1D3800 retirement completion.
    bool releaseUnpublished(const ::original_main_entry50KG::Application* issuer,D& d)noexcept {
        d={};
        if(!issuer||issuer_!=issuer)return fail(d,S::phase,0x1D3800);
        if(released_)return true;
        if(!started_){released_=true;return true;}
        if(!domain_)return fail(d,S::phase,0x1D3800);
        const auto table=domain_->materials382B38();
        // Model refs preflight alone does not check live native FIFO pins.
        if(!table->canRetireNativeCollector29C300()||
           (modelConstruction_&&!modelConstruction_->canClose()))return fail(d,S::phase,0x26E4C0);
        // Verify combined model + binder counts before any decrement. Duplicate
        // aliases must not pass independent insufficient-count preflights.
        const auto* count=modelConstruction_?modelConstruction_->materials_.count1C0():nullptr;
        for(std::size_t pair=0;pair<2;++pair)if(retained_[pair]){
            std::size_t needed=0;
            for(std::size_t p=0;p<2;++p)if(retained_[p]&&binderRows_[p]==binderRows_[pair])++needed;
            if(count)for(std::uint32_t i=0;i<*count;++i){const auto* h=modelConstruction_->materials_.handle1C4(i);if(h&&*h==binderRows_[pair])++needed;}
            assets::OriginalStandardMaterialRowView50LA row;
            if(!table->row(binderRows_[pair],row)||!row.references08||*row.references08<0||
               std::size_t(*row.references08)<needed)return fail(d,S::reference,0x14D500);
        }
        // Host abandoned-cold ownership order; not an asserted retail child
        // destructor order. Each actual numeric release uses original14D500.
        for(std::size_t p=2;p>0;--p)if(retained_[p-1]){
            if(!table->release14D500(binderRows_[p-1]))std::terminate();retained_[p-1]=false;
        }
        if(modelConstruction_&&!modelConstruction_->closeUnpublished(d))return false;
        released_=true;complete_=false;return true;
    }
public:
    ~OriginalUiTournamentPodReferences50KG(){
        // Keep partial real acquisitions until Application explicitly cleans
        // them. Never silently mint a drain or release while borrowed.
        if(started_&&!released_)std::terminate();
    }
    OriginalUiTournamentPodReferences50KG(const OriginalUiTournamentPodReferences50KG&)=delete;
    OriginalUiTournamentPodReferences50KG& operator=(const OriginalUiTournamentPodReferences50KG&)=delete;
    bool coldReferencesComplete()const noexcept {
        return complete_&&!released_&&career_&&career_->current()&&context_&&context_->live()&&
            domain_&&domain_->archiveProvider3E1A70()&&domain_->archiveProvider3E1A70()->sealed()&&
            domain_->archiveProvider3E1A70()->revision()==providerRevision_;
    }
    bool releaseRequired()const noexcept{return started_&&!released_;}
    std::int32_t binderMaterialRow(std::size_t pair)const noexcept{return pair<2&&retained_[pair]?binderRows_[pair]:-1;}
    std::uint32_t binderPrimitiveByte0C()const noexcept{return 7;} //2465CC; observation, not a primitive.
};
}
