#pragma once
// Isolated cold Garage owner. No Page admission, visibility selection, draw
// seal, renderer completion or normal-startup predicate is manufactured here.
#include "original_ui_garage_model_resource50kg.h"
#include "../../../src/game/original_ui_entity_scope50kg.h"

namespace rr::platform {
class OriginalUiGarageVisibility50KG;
class OriginalUiGarageManagerSearch50KG;
class OriginalUiGaragePageOwner50KG;
class OriginalUiGarageSelectedModel50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiGarageSession50KG;
    friend class OriginalUiGarageVisibility50KG;
    friend class OriginalUiGarageManagerSearch50KG;
    using R=OriginalUiGarageModelResource50KG;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    using S=assets::OriginalStandardMaterialStatus50LA;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    std::shared_ptr<Grant> grant_;
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets_;
    std::shared_ptr<R> model_;
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility_; //Native external-Observer custody only.
    bool managerRetiring_=false;
    bool closeManager142590(D&)noexcept;
    //GUI78 has a real live1FB67C Observer after actual Page/Search retirement.
    //Keep its completed failed Page authority distinct from normal entry.
    const OriginalUiGaragePageOwner50KG* failedLoadedPageManagerRetirementOwner_=nullptr;
    bool preflightFailedLoadedPageManager142590(const OriginalUiGaragePageOwner50KG&,D&)const noexcept;
    bool closeFailedLoadedPageManager142590(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    const OriginalUiGaragePageOwner50KG* failedWattoBindingManagerRetirementOwner_=nullptr;
    bool preflightFailedWattoBindingManager142590(const OriginalUiGaragePageOwner50KG&,D&)const noexcept;
    bool closeFailedWattoBindingManager142590(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool closeAttachedManager142590(D&,const OriginalUiGaragePageOwner50KG*,
        const OriginalUiGaragePageOwner50KG* failedWattoBindingPage=nullptr)noexcept;
    bool attempted_=false,enrolled_=false,attached_=false,placed_=false,closed_=false;
    //Native receipt minted ONLY after the real model resource constructor
    //returned and the actual Scope enrollment rejected before any mutation.
    bool enrollmentFailed2403A0_=false;
    // Native lifetime edge only. Source selected model keeps reference1.
    // Manager search is constructed after this model and dies before it.
    const OriginalUiGarageManagerSearch50KG* managerSearchBorrower_=nullptr;
    const game::OriginalWreckComponent50LA* borrowManagerSearchPredecessor(
        const OriginalUiGarageManagerSearch50KG* owner,const std::shared_ptr<Grant>& grant)noexcept {
        if(!owner||managerSearchBorrower_||!current()||grant!=grant_||
           grant.owner_before(grant_)||grant_.owner_before(grant))return nullptr;
        managerSearchBorrower_=owner;return model_->scene_.modelEntity8();
    }
    bool managerSearchPredecessorCurrent(const OriginalUiGarageManagerSearch50KG* owner,
        const std::shared_ptr<Grant>& grant)const noexcept {
        return owner&&managerSearchBorrower_==owner&&current()&&grant==grant_&&
            !grant.owner_before(grant_)&&!grant_.owner_before(grant);
    }
    bool releaseManagerSearchPredecessor(const OriginalUiGarageManagerSearch50KG* owner,
        const std::shared_ptr<Grant>& grant)noexcept {
        if(!managerSearchPredecessorCurrent(owner,grant))return false;
        managerSearchBorrower_=nullptr;return true;
    }

    bool prepare1D1CD0(std::shared_ptr<Grant> grant,
        OriginalUiGarageModelResources50KG& resources,
        std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets,D& d)noexcept {
        d={};
        if(attempted_||closed_)return R::fail(d,S::alreadyPrepared,0x1D1CD0,"garage.selected.prepare.reentry");
        if(!grant||!grant->current()||grant->issuer()!=resources.issuer_||
           grant->application()!=resources.application_||
           grant->application().owner_before(resources.application_)||
           resources.application_.owner_before(grant->application())||
           grant->generation_!=resources.generation_||!assets||!assets->loaded()||
           !assets->selectedPod()||!assets->selectedPod()->model||
           !assets->selectedPod()->model->payload||
           assets->selectedPod()->model->payload->nodeCount()!=1||
           assets->selectedPod()->look!=std::array<float,3>{{0,0,1}}||
           assets->selectedPod()->up!=std::array<float,3>{{0,1,0}})
            return R::fail(d,S::input,0x1D1CD0,"garage.selected.prepare.inputs");
        attempted_=true;grant_=std::move(grant);assets_=std::move(assets);
        // Manager+2B0 retains the actual partially constructed object too.
        //240740 receives a2=a3=0: no additional root and no local light block.
        if(!resources.load(assets_->selectedPod()->model,R::RootMode::direct,false,model_,d))
            return R::failureStage(d,"garage.selected.model.load");
        auto step=grant_->enrollConstructedModel(model_->scene_);
        if(!step.okay()){
            enrollmentFailed2403A0_=true;
            return R::fail(d,S::phase,step.sourcePC,"garage.selected.model.enrollment");
        }
        enrolled_=true;
        auto* manager=grant_->manager();
        if(!manager)return R::fail(d,S::phase,0x1D1E38,"garage.selected.manager.borrow");
        step=grant_->attachModel(*manager,model_->scene_,2u);
        if(!step.okay())return R::fail(d,S::phase,step.sourcePC,"garage.selected.model.attach");
        attached_=true;
        const auto index=model_->scene_.model()->sceneIndex;
        const auto& source=*assets_->selectedPod();
        game::SceneDisplacement49P position{{source.position[0],source.position[1],source.position[2],1.f}};
        if(!game::setLocalTranslation49P(model_->graph(),index,position))return R::fail(d,S::source,0x1D1E64,"garage.selected.placement.translation");
        // Exact +Z look/+Y up289EC0 branch. Row0 IS this model, not a child
        // retaining the PSG's independently stored translation.
        game::BodyMatrix48G local{{1,0,0,0,0,1,0,0,0,0,1,0,
            source.position[0],source.position[1],source.position[2],1}};
        if(!game::setLocalMatrix49P(model_->graph(),index,local))return R::fail(d,S::source,0x1D1E80,"garage.selected.placement.matrix");
        placed_=true;return true;
    }
    bool publishBounds28E110(D& d)noexcept {
        d={};if(!current())return R::fail(d,S::phase,0x28E110,"garage.selected.bounds.current");
        const auto step=model_->scene_.publishDirectRootBounds28E110(model_->graph());
        return step.okay()?true:R::fail(d,S::source,step.sourcePC,"garage.selected.bounds.publish28E110");
    }
    //Detached-construction abort only. Normal Page/Scene retirement leaves
    //the source Observer alive; closeManager142590 consumes it later28976C.
    // A submitted draw will additionally need the real renderer retirement
    // join; this cold-only owner cannot be used to assert that completion.
    bool close1D18D0(D& d)noexcept {
        d={};if(failedLoadedPageManagerRetirementOwner_||failedWattoBindingManagerRetirementOwner_)return R::fail(d,S::phase,0x142590);
        if(closed_)return true;
        if(managerSearchBorrower_||managerRetiring_)return R::fail(d,S::phase,0x142590);
        if(!model_){closed_=true;return true;}
        if(!enrolled_){
            if(!model_->closeUnpublished(d))return false;
        }else {
            if(!grant_||!grant_->current()||!model_->canRetireResources(nullptr))
                return R::fail(d,S::phase,0x142590);
            const auto step=grant_->retireModel(model_->scene_);
            if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
            enrolled_=false;attached_=false;
            if(!model_->releaseAfterEntity142590(d))return false;
        }
        closed_=true;placed_=false;return true;
    }
public:
    OriginalUiGarageSelectedModel50KG()noexcept=default;
    ~OriginalUiGarageSelectedModel50KG(){if(model_&&!closed_)std::terminate();}
    OriginalUiGarageSelectedModel50KG(const OriginalUiGarageSelectedModel50KG&)=delete;
    OriginalUiGarageSelectedModel50KG& operator=(const OriginalUiGarageSelectedModel50KG&)=delete;
    bool current()const noexcept {
        if(!placed_||!enrolled_||!attached_||closed_||!grant_||!grant_->current()||!model_||!model_->current())return false;
        const auto* entity=model_->scene_.modelEntity8();
        return entity&&entity->parent1C==grant_->manager()&&entity->mask20==2u&&entity->reference00==1u;
    }
    const R* model()const noexcept{return current()?model_.get():nullptr;}
};
}
