#pragma once
// PROPOSAL ONLY: real detached2403A0/240740 cold model; no UI admission.
#include "../../../src/game/original_racer_model_scene50la.h"
#include "../../../src/game/original_racer_model_packets50la.h"
#include "../../../src/assets/original_standard_material_table50la.h"
namespace rr::platform {
class OriginalUiTournamentPodReferences50KG;
class OriginalUiTournamentModelConstruction50KG final {
    friend class OriginalUiTournamentPodReferences50KG;
    using G=game::OriginalRacerModelSceneOwner50LA;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    using S=assets::OriginalStandardMaterialStatus50LA;
    std::shared_ptr<game::OriginalApplicationEntities50LA> application_;
    std::shared_ptr<const assets::OriginalStandardModelResource50LA> payload_;
    std::shared_ptr<assets::OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<assets::OriginalStandardMaterialTable50LA> table_;
    std::uint64_t generation_=0;
    std::array<game::SceneNode49P,assets::maxModelNodes49O> nodes_{};
    std::array<game::SceneNodeIndex49P,assets::maxModelNodes49O> indices_{};
    std::uint32_t nodeCount_=0;
    G scene_;
    assets::OriginalStandardMaterialReferences50LA materials_;
    std::shared_ptr<assets::OriginalStandardModelGeometry50LA> geometry_;
    game::OriginalRacerModelPackets50LA packets_;
    bool attempted_=false,complete_=false,resourceHeld_=false,referenceReleased_=false,closed_=false;
    bool fileBound_=false;
    OriginalUiTournamentModelConstruction50KG(
        std::shared_ptr<game::OriginalApplicationEntities50LA> app,std::uint64_t generation)noexcept
        :application_(std::move(app)),generation_(generation){}
    game::SceneGraph49P graph()noexcept{return {nodes_.data(),nodeCount_};}
    game::SceneGraph49P graph()const noexcept{return {const_cast<game::SceneNode49P*>(nodes_.data()),nodeCount_};}
    static bool fail(D& d,S s,std::uint32_t pc)noexcept{d.status=s;d.sourcePC=pc;return false;}
    bool effects()const noexcept{return fileBound_||scene_.model()!=nullptr||materials_.started();}
    bool prepare(std::shared_ptr<const assets::OriginalStandardModelResource50LA> payload,
        std::shared_ptr<assets::OriginalStandardTextureResolver50LA> resolver,
        std::shared_ptr<assets::OriginalStandardMaterialTable50LA> table,
        const std::string& actualDirectory,D& d)noexcept {
        d={};if(attempted_||closed_)return fail(d,S::alreadyPrepared,0x1D3590);
        if(!application_||!application_->application10()||!generation_||application_->attachmentLock423F58()||
           !payload||!payload->prepared()||!payload->originalBytes()||!payload->member()||
           !resolver||!table||!table->prepared()||!payload->nodeCount()||payload->nodeCount()>nodes_.size())
            return fail(d,S::input,0x1D3590);
        attempted_=true;payload_=std::move(payload);resolver_=std::move(resolver);table_=std::move(table);
        try {
            nodeCount_=payload_->nodeCount();
            for(std::uint32_t i=0;i<nodeCount_;++i)indices_[i]=static_cast<game::SceneNodeIndex49P>(i);
            game::OriginalRacerModelSceneBinding50LA b;
            b.sessionGeneration=generation_;b.actualObjectCount423F60=&application_->objectCount423F60();
            b.actualAttachmentLock423F58=&application_->attachmentLock423F58();b.actualTypes383408=&application_->types383408();
            b.modelIndex=indices_[0];b.sourceRowIndices=indices_.data();b.sourceRowCount=nodeCount_;
            //1D35C4 a1=1;1D35D8 loader a2=0, so PSG row0 IS this model.
            auto step=scene_.constructDirectRoot2403A0(b,graph(),true);
            if(!step.okay())return fail(d,S::source,step.sourcePC);
            step=scene_.prepareDirectRoot26E934(graph(),*payload_->member());
            if(!step.okay())return fail(d,S::source,step.sourcePC);
            assets::OriginalStandardModelFileId50LA file;
            assets::OriginalStandardTextureDiagnostic50LA cache;
            if(!resolver_->bindModelMember(*payload_->member(),assets::OriginalStandardModelFileKind50LA::psg,
                                          *payload_->originalBytes(),file,cache))return fail(d,S::source,cache.sourcePC);
            fileBound_=true;
            assets::OriginalStandardModelFileView50LA found;
            if(!resolver_->modelFile(file,found,cache))return fail(d,S::source,cache.sourcePC);
            // No warm clone, cross-owner alias or reset of a genuine cache hit.
            if(found.geometry240110)return fail(d,S::phase,0x26E9F4);
            assets::PsgHierarchyView49O hierarchy{};assets::GeometryDiagnostic49O hd{};
            const auto& bytes=*payload_->originalBytes();
            if(!assets::readPsgHierarchy49O(bytes.data(),bytes.size(),hierarchy,hd))return fail(d,S::source,0x28CAC0);
            step=scene_.loadDirectRootSourceRows28CAC0(graph(),hierarchy);
            if(!step.okay())return fail(d,S::source,step.sourcePC);
            assets::OriginalStandardMaterialPath50LA path{&actualDirectory};
            const bool acquired=materials_.acquire26E640(*payload_,table_,path,d);
            if(materials_.started()){
                scene_.fields().materialCount1C0=static_cast<std::int32_t>(*materials_.count1C0());
                scene_.fields().materials1C4=materials_.allocationBase1C4();
            }
            if(!acquired)return false;
            step=scene_.loadQuantization240570(*payload_->scene());
            if(!step.okay())return fail(d,S::source,step.sourcePC);
            geometry_=std::make_shared<assets::OriginalStandardModelGeometry50LA>();
            assets::OriginalStandardResourceDiagnostic50LA rd;
            if(!geometry_->construct240110(payload_,resolver_,file,&scene_,rd))return fail(d,S::source,rd.sourcePC);
            resourceHeld_=true;scene_.fields().geometry1B4=geometry_.get();
            if(!resolver_->publishGeometry1326A0(file,geometry_,cache))return fail(d,S::source,cache.sourcePC);
            step=scene_.allocateSourceRowTable26EE9C();
            if(!step.okay())return fail(d,S::source,step.sourcePC);
            //1D35E8 a3=0 forwarded by240740: dynamic lighting off.
            const auto packet=packets_.construct240740(scene_,*geometry_,false);
            if(!packet.okay())return fail(d,S::source,packet.sourcePC);
            complete_=true;return true;
        }catch(...){return fail(d,S::allocation,0x1D3590);}
    }
    bool canClose()const noexcept {
        if(closed_||!scene_.model())return true;
        if(!materials_.started())return !resourceHeld_&&!packets_.constructed240740();
        const game::OriginalWreckComponentDomain50LA domain{generation_,nullptr,0};
        if(!scene_.preflightScene2896D0(graph()).okay()||
           (!referenceReleased_&&!scene_.preflightModelReference142590(domain).okay())||
           !materials_.canRelease26E4C0())return false;
        if(packets_.constructed240740()&&!packets_.preflight240450().okay())return false;
        return !resourceHeld_||(geometry_&&geometry_->prototype1C()==&scene_&&
               geometry_->holdsModelReference(&scene_)&&geometry_->canReleaseForModel(&scene_));
    }
    // Only called by the private parent after SAME table no-FIFO-pin check and
    // combined model/binder retain-count preflight. No observer/Entity append
    // or renderer export exists here, so there is no fake retirement receipt.
    bool closeUnpublished(D& d)noexcept {
        d={};if(closed_)return true;
        if(!scene_.model()){closed_=true;return true;}
        if(!canClose())return fail(d,S::phase,0x240450);
        if(!materials_.started()){
            const auto step=scene_.closeFreshUnlinked(graph());
            if(!step.okay())return fail(d,S::phase,step.sourcePC);
            closed_=true;complete_=false;return true;
        }
        const game::OriginalWreckComponentDomain50LA domain{generation_,nullptr,0};
        if(!referenceReleased_){const auto step=scene_.releaseModelReference142590(domain);
            if(!step.okay())return fail(d,S::phase,step.sourcePC);referenceReleased_=true;}
        if(packets_.constructed240740()&&!packets_.release240450().okay())std::terminate();
        if(!materials_.release26E4C0(d))std::terminate();
        if(resourceHeld_){assets::OriginalStandardResourceDiagnostic50LA rd;
            if(!geometry_->release26E598(&scene_,materials_,rd))std::terminate();resourceHeld_=false;}
        if(!scene_.freeSourceRowTable26E5CC().okay())std::terminate();
        if(!materials_.freeAllocation26E5EC())std::terminate();
        if(!scene_.releaseSceneBase2896D0(graph()).okay())std::terminate();
        closed_=true;complete_=false;return true;
    }
public:
    ~OriginalUiTournamentModelConstruction50KG(){if(scene_.model()&&!closed_)std::terminate();}
    OriginalUiTournamentModelConstruction50KG(const OriginalUiTournamentModelConstruction50KG&)=delete;
    OriginalUiTournamentModelConstruction50KG& operator=(const OriginalUiTournamentModelConstruction50KG&)=delete;
};
}
