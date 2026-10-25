#pragma once
// Isolated Garage proposal. Actual CPU model/resource ownership, not a draw,
// visibility registration, manager publication or page-admission capability.
#include "../../../src/assets/original_ui_garage_scene_assets50kg.h"
#include "../../../src/assets/original_standard_material_domain50la.h"
#include "../../../src/assets/original_standard_material_context50la.h"
#include "../../../src/game/original_racer_model_scene50la.h"
#include "../../../src/game/original_racer_model_packets50la.h"
#include <vector>
#include <cstring>
namespace original_main_entry50KG {struct Application;}
namespace rr::platform {
class OriginalUiGarageSession50KG;
class OriginalUiGarageStaticModels50KG;
class OriginalUiGarageActors50GN;
class OriginalUiGarageWattoOwner50KG;
class OriginalUiGarageSelectedModel50KG;
class OriginalUiGarageVisibility50KG;
class OriginalUiGarageDrawContribution50KG;
class OriginalUiGarageModelResources50KG;
class OriginalUiGarageModelResource50KG final {
    friend class OriginalUiGarageModelResources50KG;
    friend class OriginalUiGarageSession50KG;
    friend class OriginalUiGarageUiLights50KG;
    friend class OriginalUiGarageActors50GN;
    friend class OriginalUiGarageStaticModels50KG;
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGarageSceneCollision50KG;
    friend class OriginalUiGarageWattoOwner50KG;
    friend class OriginalUiGarageSelectedModel50KG;
    friend class OriginalUiGarageVisibility50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    using S=assets::OriginalStandardMaterialStatus50LA;
    using I=game::SceneNodeIndex49P;
public:
    enum class RootMode : std::uint8_t {direct,separate};
private:
    std::shared_ptr<game::OriginalApplicationEntities50LA> application_;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<const assets::OriginalUiGarageModelAsset50KG> source_;
    // A warm user's real geometry prototype1C cannot outlive its owner.
    std::shared_ptr<OriginalUiGarageModelResource50KG> prototype_;
    std::uint64_t generation_=0,providerRevision_=0;
    std::array<game::SceneNode49P,assets::maxModelNodes49O+1u> nodes_{};
    std::array<I,assets::maxModelNodes49O> indices_{};
    std::uint32_t nodeCount_=0;
    RootMode mode_=RootMode::direct;
    game::OriginalRacerModelSceneOwner50LA scene_;
    assets::OriginalStandardMaterialReferences50LA materials_;
    std::shared_ptr<assets::OriginalStandardModelGeometry50LA> geometry_;
    game::OriginalRacerModelPackets50LA packets_;
    //Native ownership of the actual source1C8 leaf. The source word remains
    //stale after registry death, but the receipt/weak owner prevent reuse.
    std::weak_ptr<OriginalUiGarageModelResource50KG> nativeSharedSelf_;
    std::weak_ptr<::original_main_entry50KG::NativeCollectorFifo50LA> nativeCollector1C8Owner_;
    assets::OriginalStandardMaterialCollectorReceipt50LA nativeCollector1C8Receipt_;
    std::int32_t nativeCollector1C8Registration_=-1;
    std::uint32_t nativeGarageFifoPins_=0;
    std::uint64_t nativeGaragePacketRevision()const noexcept {
        return current()&&packets_.constructed240740()&&packets_.matricesPrepared_?packets_.matrixRevision_:0;
    }
    bool attempted_=false,complete_=false,resourceHeld_=false,closed_=false;
    bool fileBound_=false;
    OriginalUiGarageModelResource50KG(
        std::shared_ptr<game::OriginalApplicationEntities50LA> application,
        std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context,
        std::uint64_t generation)noexcept
        :application_(std::move(application)),domain_(std::move(domain)),context_(std::move(context)),generation_(generation){}
    // Static failure context only. Never replace a deeper authored-field
    // diagnostic, allocate storage, or alter status/PC/row/effect history.
    static bool failureStage(D& d,const char* stage)noexcept {
        if(!d.field||!*d.field)d.field=stage;
        return false;
    }
    static bool fail(D& d,S s,std::uint32_t pc,const char* stage=nullptr)noexcept {
        d.status=s;d.sourcePC=pc;
        return stage?failureStage(d,stage):false;
    }
    void publishMaterialFields()noexcept {
        if(materials_.started()){
            scene_.fields().materialCount1C0=static_cast<std::int32_t>(*materials_.count1C0());
            scene_.fields().materials1C4=materials_.allocationBase1C4();
        }
    }
    bool prepare(std::shared_ptr<const assets::OriginalUiGarageModelAsset50KG> source,
        RootMode mode,bool lighting,const std::vector<std::shared_ptr<OriginalUiGarageModelResource50KG>>& population,D& d)noexcept {
        d={};if(attempted_||closed_)return fail(d,S::alreadyPrepared,0x26E820,"garage.model.prepare.reentry");
        if(!application_||!application_->application10()||!generation_||application_->attachmentLock423F58()||
           !domain_||!domain_->prepared()||!context_||!context_->live()||
           !domain_->materials382B38()->screenApplicationMatches(*context_->materialApplication())||
           !domain_->materials382B38()->canRetireNativeCollector29C300()||
           !source||!source->archive||!source->payload||!source->payload->prepared()||
           !source->source.identity||!source->source.data||source->source.size!=source->source.identity->bytes||
           !source->payload->nodeCount()||source->payload->nodeCount()>indices_.size()||
           !source->payload->member()||!source->payload->originalBytes())return fail(d,S::input,0x26E820,"garage.model.prepare.inputs");
        const auto provider=domain_->archiveProvider3E1A70();
        assets::OriginalStandardArchiveProvider50LA::MemberView original;
        if(!provider||!provider->sealed()||provider->lookup132820(*source->payload->member(),original)!=
           assets::OriginalStandardArchiveProvider50LA::Lookup::present||!original.data||
           original.size!=source->source.size||std::memcmp(original.data,source->source.data,original.size))
            return fail(d,S::source,0x132820,"garage.model.provider.lookup132820");
        // These are exact archive identities, not arbitrary supplied PSGs.
        const auto* archive=source->archive->identity();
        const int selectedCareer=archive?assets::originalUiGaragePodCareer50KG(archive->id):-1;
        const bool selectedArchive=selectedCareer>=0&&
            archive==assets::originalUiGaragePodArchive50KG(static_cast<std::uint32_t>(selectedCareer));
        if(!source->archive->loaded()||
           (archive!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::garage)&&
            !selectedArchive))
            return fail(d,S::source,0x132820,"garage.model.archive.identity");
        // Every authenticated selected E archive has its exact single PSG at
        // ordinal1. Do not admit a different catalog member just because its
        // archive belongs to the selected-pod family.
        if(selectedArchive&&source->source.identity!=assets::originalUiMemberIdentity50KG(archive->id,1))
            return fail(d,S::source,0x132820,"garage.model.selected.geometry.identity");
        // This public immutable descriptor must still retain the exact archive
        // view and its matching decoded payload/hierarchy. A genuine identity
        // alone cannot authenticate a substituted PSG or a foreign row view.
        const auto member=source->archive->find(*source->payload->member());
        const auto& bytes=*source->payload->originalBytes();
        if(member.identity!=source->source.identity||member.data!=source->source.data||
           member.size!=source->source.size||*source->payload->member()!=source->source.identity->name||
           bytes.size()!=source->source.size||std::memcmp(bytes.data(),source->source.data,bytes.size())||
           source->hierarchy.data!=source->source.data||source->hierarchy.bytes!=source->source.size||
           source->hierarchy.nodeCount!=source->payload->nodeCount())
            return fail(d,S::source,0x28CAC0,"garage.model.source.hierarchy28CAC0");
        attempted_=true;source_=std::move(source);mode_=mode;providerRevision_=provider->revision();
        try {
            const auto payload=source_->payload;
            nodeCount_=payload->nodeCount()+(mode==RootMode::separate?1u:0u);
            for(std::uint32_t i=0;i<payload->nodeCount();++i)
                indices_[i]=static_cast<I>(i+(mode==RootMode::separate?1u:0u));
            game::OriginalRacerModelSceneBinding50LA binding;
            binding.sessionGeneration=generation_;binding.actualObjectCount423F60=&application_->objectCount423F60();
            binding.actualAttachmentLock423F58=&application_->attachmentLock423F58();binding.actualTypes383408=&application_->types383408();
            binding.modelIndex=static_cast<I>(0);binding.sourceRowIndices=indices_.data();binding.sourceRowCount=payload->nodeCount();
            auto step=mode==RootMode::direct?scene_.constructDirectRoot2403A0(binding,graph(),true):
                scene_.construct2403A0(binding,graph(),true);
            if(!step.okay())return fail(d,S::source,step.sourcePC,"garage.model.entity2403A0");
            step=mode==RootMode::direct?scene_.prepareDirectRoot26E934(graph(),*payload->member()):
                scene_.constructSeparateRoot26E944(graph(),*payload->member());
            if(!step.okay())return fail(d,S::source,step.sourcePC,"garage.model.root26E934_26E944");
            const auto resolver=domain_->resolver3E1A70();
            assets::OriginalStandardModelFileId50LA file;
            assets::OriginalStandardTextureDiagnostic50LA cache;
            if(!resolver->bindModelMember(*payload->member(),assets::OriginalStandardModelFileKind50LA::psg,
                *payload->originalBytes(),file,cache))return fail(d,S::source,cache.sourcePC,"garage.model.file.bind132820");
            fileBound_=true;
            assets::OriginalStandardModelFileView50LA found;
            if(!resolver->modelFile(file,found,cache))return fail(d,S::source,cache.sourcePC,"garage.model.file.borrow132820");
            assets::OriginalStandardResourceDiagnostic50LA rd;
            if(found.geometry240110){
                // Resolve the ACTUAL cache prototype; no supplied callback,
                // bind-pose reconstruction or fallback on an unknown cache hit.
                for(const auto& candidate:population)if(candidate&&candidate.get()!=this&&
                    &candidate->scene_==found.geometry240110->prototype1C())prototype_=candidate;
                if(!prototype_||!prototype_->current()||prototype_->application_!=application_||
                   prototype_->generation_!=generation_||prototype_->domain_!=domain_||
                   prototype_->context_!=context_||prototype_->mode_!=mode_||
                   prototype_->geometry_!=found.geometry240110||
                   *prototype_->source_->payload->originalBytes()!=*payload->originalBytes())
                    return fail(d,S::phase,0x26EA10,"garage.model.warm.prototype26EA10");
                geometry_=found.geometry240110;
                if(!geometry_->retain104960(&scene_,rd))return fail(d,S::source,rd.sourcePC,"garage.model.warm.retain104960");
                resourceHeld_=true;scene_.fields().geometry1B4=geometry_.get();
                step=mode==RootMode::direct&&payload->nodeCount()==1?
                    scene_.loadWarmChildlessRows26EAC4(graph(),prototype_->scene_,prototype_->graph()):
                    scene_.loadWarmHierarchy26EA28(graph(),prototype_->scene_,prototype_->graph());
                if(!step.okay())return fail(d,S::source,step.sourcePC,"garage.model.warm.hierarchy26EA28");
                const bool cloned=materials_.clone26EB44(prototype_->materials_,d);publishMaterialFields();
                if(!cloned)return failureStage(d,"garage.model.material.clone26EB44");
                step=scene_.copyParameters2406E0(prototype_->scene_);
                if(!step.okay())return fail(d,S::source,step.sourcePC,"garage.model.warm.parameters2406E0");
            }else {
                step=mode==RootMode::direct?scene_.loadDirectRootSourceRows28CAC0(graph(),source_->hierarchy):
                    scene_.loadSourceRows28CAC0(graph(),source_->hierarchy);
                if(!step.okay())return fail(d,S::source,step.sourcePC,"garage.model.cold.sourceRows28CAC0");
                const std::string directory="ui\\textures\\"; // actual Garage36B970
                assets::OriginalStandardMaterialPath50LA path{&directory};
                const bool acquired=materials_.acquire26E640(*payload,domain_->materials382B38(),path,d);
                publishMaterialFields();if(!acquired)return failureStage(d,"garage.model.material.acquire26E640");
                step=scene_.loadQuantization240570(*payload->scene());
                if(!step.okay())return fail(d,S::source,step.sourcePC,"garage.model.quantization240570");
                geometry_=std::make_shared<assets::OriginalStandardModelGeometry50LA>();
                if(!geometry_->construct240110(payload,resolver,file,&scene_,rd))return fail(d,S::source,rd.sourcePC,"garage.model.geometry240110");
                resourceHeld_=true;scene_.fields().geometry1B4=geometry_.get();
                if(!resolver->publishGeometry1326A0(file,geometry_,cache))return fail(d,S::source,cache.sourcePC,"garage.model.geometry.publish1326A0");
            }
            step=scene_.allocateSourceRowTable26EE9C();if(!step.okay())return fail(d,S::source,step.sourcePC,"garage.model.rowTable26EE9C");
            const auto packet=packets_.construct240740(scene_,*geometry_,lighting);
            if(!packet.okay())return fail(d,S::source,packet.sourcePC,"garage.model.packets240740");
            if(provider->revision()!=providerRevision_)return fail(d,S::phase,0x26EEB4,"garage.model.provider.revision26EEB4");
            complete_=true;return true;
        }catch(...){return fail(d,S::allocation,0x26E820,"garage.model.prepare.allocation");}
    }
    // Additional real Entity/observer ownership is checked by its typed caller.
    bool canRetireResources(const void* actualObserver)const noexcept {
        if(closed_||!scene_.model())return true;
        if(nativeGarageFifoPins_||!domain_||!domain_->materials382B38()->canRetireNativeCollector29C300()||
           !scene_.preflightRetainedScene2896D0(graph(),actualObserver).okay()||
           !materials_.canRelease26E4C0())return false;
        if(packets_.constructed240740()&&!packets_.preflight240450().okay())return false;
        return !resourceHeld_||(geometry_&&geometry_->holdsModelReference(&scene_)&&geometry_->canReleaseForModel(&scene_));
    }
    bool referenceReleased142590()const noexcept{return scene_.referenceReleased_;}
    enum class PageRetirement {none,packetsReleased,materialsReleased,geometryReleased,
        rowsReleased,payloadsReleased,childrenReleased,observerReleased,complete};
    PageRetirement pageRetirement_=PageRetirement::none;
    const void* pageObserverIdentity_=nullptr; //Comparison-only after280920.
    bool retirePagePayloads240450(D& d)noexcept {
        d={};
        if(!scene_.referenceReleased_||closed_||pageRetirement_>PageRetirement::payloadsReleased)
            return fail(d,S::phase,0x240450);
        if(pageRetirement_==PageRetirement::none){
            pageObserverIdentity_=scene_.model_.observer19C;
            if(!canRetireResources(pageObserverIdentity_))return fail(d,S::phase,0x240450);
            if(packets_.constructed240740()&&!packets_.release240450().okay())
                return fail(d,S::phase,0x2404DC);
            pageRetirement_=PageRetirement::packetsReleased;
        }
        if(pageRetirement_==PageRetirement::packetsReleased){
            if(!materials_.release26E4C0(d))return false;
            pageRetirement_=PageRetirement::materialsReleased;
        }
        if(pageRetirement_==PageRetirement::materialsReleased){
            if(resourceHeld_){assets::OriginalStandardResourceDiagnostic50LA rd;
                if(!geometry_->release26E598(&scene_,materials_,rd))return fail(d,S::phase,rd.sourcePC);
                resourceHeld_=false;
            }
            pageRetirement_=PageRetirement::geometryReleased;
        }
        if(pageRetirement_==PageRetirement::geometryReleased){
            const auto step=scene_.freeSourceRowTable26E5CC();
            if(!step.okay())return fail(d,S::phase,step.sourcePC);
            pageRetirement_=PageRetirement::rowsReleased;
        }
        if(pageRetirement_==PageRetirement::rowsReleased){
            if(materials_.started()&&!materials_.freeAllocation26E5EC())return fail(d,S::phase,0x26E5EC);
            pageRetirement_=PageRetirement::payloadsReleased;
        }
        return true;
    }
    bool retirePageSceneChildren2896D0(D& d)noexcept {
        d={};if(pageRetirement_!=PageRetirement::payloadsReleased||closed_)
            return fail(d,S::phase,0x2896D0);
        const auto step=scene_.retireGarageSceneChildren2896D0(graph(),pageObserverIdentity_);
        if(!step.okay())return fail(d,S::phase,step.sourcePC);
        pageRetirement_=PageRetirement::childrenReleased;return true;
    }
    bool finishPageSceneBase1413E0(D& d)noexcept {
        d={};if(pageRetirement_!=PageRetirement::observerReleased||closed_)
            return fail(d,S::phase,0x289778);
        const auto step=scene_.finishGarageSceneBase1413E0(graph(),pageObserverIdentity_);
        if(!step.okay())return fail(d,S::phase,step.sourcePC);
        pageRetirement_=PageRetirement::complete;
        closed_=true;complete_=false;prototype_.reset();return true;
    }
    // Called after genuine142590 by the enclosing scoped Entity owner. A real
    // observer, if any, must have been removed by its owner before2896D0.
    bool releaseAfterEntity142590(D& d)noexcept {
        d={};if(closed_)return true;
        if(!scene_.referenceReleased_||!canRetireResources(nullptr))return fail(d,S::phase,0x240450);
        if(packets_.constructed240740()&&!packets_.release240450().okay())std::terminate();
        if(!materials_.release26E4C0(d))std::terminate();
        if(resourceHeld_){assets::OriginalStandardResourceDiagnostic50LA rd;
            if(!geometry_->release26E598(&scene_,materials_,rd))return fail(d,S::phase,rd.sourcePC);
            resourceHeld_=false;}
        if(!scene_.freeSourceRowTable26E5CC().okay())std::terminate();
        if(materials_.started()&&!materials_.freeAllocation26E5EC())std::terminate();
        if(!scene_.releaseSceneBase2896D0(graph()).okay())std::terminate();
        closed_=true;complete_=false;prototype_.reset();return true;
    }
    // Failure recovery only while this model's actual Entity is detached and
    // no19C has ever been installed. Attached cleanup cannot enter here.
    //Read-only factor of the resource-bearing detached142590 preflight.
    //No fresh/partial constructor success or Authority enrollment is inferred.
    bool preflightUnpublished142590(D& d)const noexcept {
        d={};const auto* entity=scene_.modelEntity8();
        if(!scene_.model()||!entity||entity->parent1C||entity->previous10||entity->next14||
           entity->firstChild18||scene_.model()->observer19C)return fail(d,S::phase,0x142590);
        const game::OriginalWreckComponentDomain50LA detached{generation_,nullptr,0};
        if(!canRetireResources(nullptr)||!scene_.preflightModelReference142590(detached).okay())
            return fail(d,S::phase,0x142590);
        return true;
    }
    bool closeUnpublished(D& d)noexcept {
        d={};if(closed_)return true;
        if(!scene_.model()){closed_=true;return true;}
        const auto* entity=scene_.modelEntity8();
        if(!entity||entity->parent1C||entity->previous10||entity->next14||entity->firstChild18||
           scene_.model()->observer19C)return fail(d,S::phase,0x142590);
        if(!materials_.started()&&!resourceHeld_){
            const auto step=scene_.closeFreshUnlinked(graph());
            if(!step.okay())return fail(d,S::phase,step.sourcePC);
            closed_=true;complete_=false;return true;
        }
        const game::OriginalWreckComponentDomain50LA detached{generation_,nullptr,0};
        if(!preflightUnpublished142590(d))return false;
        const auto step=scene_.releaseModelReference142590(detached);
        if(!step.okay())return fail(d,S::phase,step.sourcePC);
        return releaseAfterEntity142590(d);
    }
public:
    ~OriginalUiGarageModelResource50KG(){if(scene_.model()&&!closed_)std::terminate();}
    OriginalUiGarageModelResource50KG(const OriginalUiGarageModelResource50KG&)=delete;
    OriginalUiGarageModelResource50KG& operator=(const OriginalUiGarageModelResource50KG&)=delete;
    bool current()const noexcept {
        //132820 and the atomic constructor revision check stay above. After
        //source12E5E0, this model owns cached geometry/material references,
        //not a still-open bank lookup. Another genuine mount/close cannot
        //invalidate those retained immutable payloads or reopen their file ID.
        if(!complete_||closed_||!application_||!application_->application10()||
           !context_||!context_->live()||!domain_||!domain_->prepared()||
           !domain_->archiveProvider3E1A70()||!domain_->archiveProvider3E1A70()->sealed()||
           !domain_->materials382B38()->screenApplicationMatches(*context_->materialApplication())||
           !source_||!source_->archive||!source_->payload||!source_->payload->prepared()||
           !source_->payload->member()||!geometry_||!resourceHeld_||
           !geometry_->constructed240110()||!geometry_->holdsModelReference(&scene_)||
           geometry_->fileId().resolver!=domain_->resolver3E1A70().get()||
           !geometry_->payload()||!geometry_->payload()->prepared()||
           !materials_.complete()||materials_.released26E4C0()||
           materials_.table382B38()!=domain_->materials382B38().get())return false;
        const auto member=source_->archive->find(*source_->payload->member());
        return member.identity==source_->source.identity&&member.data==source_->source.data&&
            member.size==source_->source.size&&member.data&&member.size;
    }
    game::SceneGraph49P graph()noexcept{return {nodes_.data(),nodeCount_};}
    game::SceneGraph49P graph()const noexcept{return {const_cast<game::SceneNode49P*>(nodes_.data()),nodeCount_};}
    game::OriginalRacerModelSceneOwner50LA& scene()noexcept{return scene_;}
    const game::OriginalRacerModelSceneOwner50LA& scene()const noexcept{return scene_;}
    const game::OriginalRacerModelPackets50LA& packets()const noexcept{return packets_;}
    const assets::OriginalStandardMaterialReferences50LA& materials()const noexcept{return materials_;}
    const assets::OriginalStandardModelGeometry50LA* geometry()const noexcept{return geometry_.get();}
    const assets::OriginalUiGarageModelAsset50KG* source()const noexcept{return source_.get();}
    std::uint64_t generation()const noexcept{return generation_;}
};

// ONE parent-issued model population, using the real Application-wide cache.
//42 is the authored30 static +10 actors +selected19E +Watto extent, not a new
// global parser/table capacity or a fabricated number of visible draws.
class OriginalUiGarageModelResources50KG final {
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    friend class OriginalUiGarageSceneCollision50KG;
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiGarageSession50KG;
    friend class OriginalUiGarageUiLights50KG;
    friend class OriginalUiGarageActors50GN;
    friend class OriginalUiGarageStaticModels50KG;
    friend class OriginalUiGarageWattoOwner50KG;
    friend class OriginalUiGarageSelectedModel50KG;
    using R=OriginalUiGarageModelResource50KG;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    using S=assets::OriginalStandardMaterialStatus50LA;
    std::shared_ptr<game::OriginalApplicationEntities50LA> application_;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context_;
    const ::original_main_entry50KG::Application* issuer_=nullptr;
    std::uint64_t generation_=0;
    std::vector<std::shared_ptr<R>> population_;
    OriginalUiGarageModelResources50KG(const ::original_main_entry50KG::Application* issuer,
        std::shared_ptr<game::OriginalApplicationEntities50LA> application,
        std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context,std::uint64_t generation)noexcept
        :application_(std::move(application)),domain_(std::move(domain)),context_(std::move(context)),issuer_(issuer),generation_(generation){}
    bool load(std::shared_ptr<const assets::OriginalUiGarageModelAsset50KG> source,R::RootMode mode,
        bool lighting,std::shared_ptr<R>& out,D& d)noexcept {
        d={};if(out||!issuer_||!generation_||population_.size()>=42u)return R::fail(d,S::input,0x26E820,"garage.models.load.inputs");
        try {
            // Reserve native owner backing before any counted constructor.
            if(population_.empty())population_.reserve(42u);
            auto next=std::shared_ptr<R>(new R(application_,domain_,context_,generation_));
            next->nativeSharedSelf_=next;
            population_.push_back(next);out=next; // Retain a partial prefix on failure.
            return next->prepare(std::move(source),mode,lighting,population_,d);
        }catch(...){return R::fail(d,S::allocation,0x26E820,"garage.models.load.allocation");}
    }
public:
    OriginalUiGarageModelResources50KG(const OriginalUiGarageModelResources50KG&)=delete;
    OriginalUiGarageModelResources50KG& operator=(const OriginalUiGarageModelResources50KG&)=delete;
    std::size_t size()const noexcept{return population_.size();}
};
}
