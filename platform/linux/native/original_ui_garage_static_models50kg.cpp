#include "original_ui_garage_static_models50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_collision50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "../../../src/game/original_ui_user_rotate50kg.h"
namespace rr::platform {
OriginalUiGarageStaticModels50KG::~OriginalUiGarageStaticModels50KG()noexcept {
    if(listLive_||retiring_)std::terminate();
    for(const auto& model:models_)if(model&&!model->closed_)std::terminate();
}
bool OriginalUiGarageStaticModels50KG::belongsTo(const OriginalUiGaragePodScene50KG& scene,
    const std::shared_ptr<Grant>& grant,
    const std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>& assets)const noexcept {
    return sceneOwner_==&scene&&grant&&grant_==grant&&!grant_.owner_before(grant)&&
        !grant.owner_before(grant_)&&assets&&assets_==assets&&!assets_.owner_before(assets)&&
        !assets.owner_before(assets_)&&!closed_;
}
bool OriginalUiGarageStaticModels50KG::descriptorIdentityCurrent()const noexcept {
    if(!descriptorsComplete_||!listLive_||closed_||retiring_||!grant_||!grant_->current()||
       !assets_||!assets_->loaded()||!resources_||!sceneOwner_||
       !sceneOwner_->staticZoneLists3C4_||sceneOwner_->staticZoneLists3C4_[0]!=&list3C4_||
       count30_!=count||!entries34_||
       list3C4_.kind!=K::staticModels27B610||list3C4_.reference00!=1||
       list3C4_.sessionGeneration!=grant_->generation_||
       list3C4_.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&list3C4_))||
       list3C4_.parent1C||list3C4_.previous10||list3C4_.next14||list3C4_.mask20||list3C4_.descendants24)
        return false;
    return true;
}
bool OriginalUiGarageStaticModels50KG::prepare2710F0(OriginalUiGaragePodScene50KG& scene,
    std::shared_ptr<Grant> grant,std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets,
    std::shared_ptr<OriginalUiGarageModelResources50KG> resources,D& d)noexcept {
    d={};if(attempted_||closed_)return R::fail(d,S::alreadyPrepared,0x2710F0);
    if(!grant||!grant->current()||!assets||!assets->loaded()||!assets->sceneDat()||
       !resources||resources->issuer_!=grant->issuer()||resources->generation_!=grant->generation_||
       resources->application_!=grant->application()||
       resources->application_.owner_before(grant->application())||grant->application().owner_before(resources->application_)||
       !scene.currentPrefix()||scene.visibility_||scene.grant_!=grant||scene.assets_!=assets)
        return R::fail(d,S::input,0x2710F0);
    const auto* config=scene.liveSceneConfig34();
    if(!config||scene.loadStage_!=OriginalUiGaragePodScene50KG::LoadStage::sceneStructure||
       scene.numZones34C_!=1||!scene.staticZoneLists3C4_||scene.staticZoneLists3C4_[0])
        return R::fail(d,S::phase,0x2710F0);
    const auto* statics=config->section("StaticModels");
    assets::OriginalUiPageDiagnostic50KG text;std::int32_t n=0;
    if(!statics||config->section("Zones")||config->section("Portals")||
       !assets::originalUiInteger50KG(*statics,"NumberOfStaticModels",0,n,text,0x275F6C)||n!=int(count)||
       !assets::originalUiBoolean50KG(*statics,"EnvironmentNodeVisible",true,environmentNodeVisible_,text,0x275EF4))
        return R::fail(d,S::source,text.sourcePC?text.sourcePC:0x275F6C);
    attempted_=true;grant_=std::move(grant);assets_=std::move(assets);resources_=std::move(resources);sceneOwner_=&scene;
    //271150/27B61C: a1=1; a real StaticModels Entity, not a search node.
    const auto made=grant_->constructPrefix(list3C4_,K::staticModels27B610);
    if(!made.okay())return R::fail(d,S::phase,made.sourcePC);
    listLive_=true;
    scene.staticZoneLists3C4_[0]=&list3C4_; //27116C, after constructor, before reader.
    count30_=static_cast<std::uint32_t>(count); //27B780 before allocation.
    try {
        entries34_=std::make_unique<Wrapper*[]>(count);
        for(std::size_t i=0;i<count;++i){
            const auto* source=assets_->staticModel(i);
            const auto* row=config->section("Model"+std::to_string(i+1));
            if(!source||source->ordinal!=i+1||!source->model||!source->model->payload||
               source->model->payload->nodeCount()!=1||!row)
                return R::fail(d,S::source,0x27B804);
            const auto* type=row->find("StaticType");
            if(type&&type->text!="Standard")return R::fail(d,S::source,0x27B844);
            auto wrapper=std::make_unique<Wrapper>();
            wrappers_[i]=std::move(wrapper);entries34_[i]=wrappers_[i].get(); //27B878.
            wrappers_[i]->descriptor10=std::make_unique<Descriptor>(); //27AB90.
            auto& descriptor=*wrappers_[i]->descriptor10;descriptor.source=source;
            const auto* slt=row->find("SLT");
            const auto dot=slt?slt->text.find('.'):std::string::npos;
            if(!slt||slt->text!=source->slt||dot==std::string::npos||!dot||dot>50||
               !assets::originalUiTriple50KG(*row,"Position",{},descriptor.position110,text,0x27A304)||
               !assets::originalUiTriple50KG(*row,"LookVector",{0,0,1},descriptor.look120,text,0x27A320)||
               !assets::originalUiTriple50KG(*row,"UpVector",{0,1,0},descriptor.up130,text,0x27A33C)||
               !assets::originalUiBoolean50KG(*row,"UseLighting",true,descriptor.useLighting140,text,0x27A354)||
               !assets::originalUiBoolean50KG(*row,"UseDynamicLighting",false,descriptor.useDynamicLighting144,text,0x27A36C)||
               !assets::originalUiBoolean50KG(*row,"IsSmoker",false,descriptor.isSmoker14C,text,0x27A384)||
               !assets::originalUiTriple50KG(*row,"SmokeOffset",{},descriptor.smokeOffset150,text,0x27A3A0)||
               !assets::originalUiTriple50KG(*row,"SmokeVelocity",{},descriptor.smokeVelocity160,text,0x27A3BC)||
               descriptor.position110!=source->position||descriptor.look120!=source->look)
                return R::fail(d,S::source,text.sourcePC?text.sourcePC:0x27A524);
            // This corpus has no required288C30 smoker. A newly authored one
            // must acquire its real Scene4A0 owner, never become a skipped draw.
            if(descriptor.isSmoker14C)return R::fail(d,S::source,0x27AE50);
            descriptor.modelName180=slt->text.substr(0,dot)+".slt";
        }
    }catch(...){return R::fail(d,S::allocation,0x27B798);}
    descriptorsComplete_=true;return true;
}
bool OriginalUiGarageStaticModels50KG::populate274160(std::shared_ptr<OriginalUiGarageVisibility50KG> visibility,D& d)noexcept {
    d={};if(populationAttempted_||!descriptorIdentityCurrent()||!visibility||
       !visibility->matchesColdConstruction(grant_,assets_)||visibility->sourceSceneOwner_!=sceneOwner_)
        return R::fail(d,S::phase,0x274160);
    populationAttempted_=true;visibility_=std::move(visibility);
    // Real275EF4 ->2741C0 zone44 publication, before first27B930 entry.
    if(!visibility_->beginStaticPopulation2741C0(*this,d))return false;
    try {
        for(std::size_t i=0;i<count;++i){auto& wrapper=*wrappers_[i];
            if(entries34_[i]!=&wrapper||wrapper.type00!=1||!wrapper.descriptor10||wrapper.model8||models_[i])
                return R::fail(d,S::phase,0x27B95C);
            const auto& descriptor=*wrapper.descriptor10;
            // Each authored placement receives a DIFFERENT counted root,
            // rows, packets and material references. No lightray pass omission.
            const bool loaded=resources_->load(descriptor.source->model,R::RootMode::direct,
                descriptor.useDynamicLighting144,models_[i],d);
            wrapper.model8=models_[i].get(); //Retains actual partial27AC38 result.
            if(!loaded)return false;
            auto& model=*models_[i];auto& scene=model.scene_;
            const auto enrolled=grant_->enrollConstructedModel(scene);
            if(!enrolled.okay())return R::fail(d,S::phase,enrolled.sourcePC);
            enrolled_[i]=true;
            scene.fields_.flags1B3=static_cast<std::uint8_t>((scene.fields_.flags1B3&~0x10u)|((descriptor.field148&1u)<<4));
            scene.model_.name38=descriptor.modelName180.substr(0,127); //27AD34/44.
            const auto root=scene.model_.sceneIndex;
            const game::SceneDisplacement49P position{{descriptor.position110[0],descriptor.position110[1],descriptor.position110[2],1}};
            if(!game::setLocalTranslation49P(model.graph(),root,position))return R::fail(d,S::source,0x27ADCC);
            game::BodyMatrix48G local;
            // The absent UpVector is the REAL27A224 constructor default +Y.
            //289EC0(1,0,0) corrects look, normalizes up/look, crosses right.
            if(!game::originalUiUserRotatePose50KG(descriptor.position110,descriptor.look120,descriptor.up130,local)||
               !game::setLocalMatrix49P(model.graph(),root,local))return R::fail(d,S::source,0x27ADEC);
            if(!visibility_->attachStatic27AE0C(*this,i,models_[i],d))return false;
            const auto attached=grant_->attachModel(list3C4_,scene,0u); //AFTER28E8A0.
            if(!attached.okay())return R::fail(d,S::phase,attached.sourcePC);
            attached_[i]=true;
            wrapper.descriptor10.reset(); //27AF6C, only after the entire row.
            ++populated_;
        }
    }catch(...){return R::fail(d,S::allocation,0x27ABC0);}
    populationComplete_=true;return populationCurrent()?true:R::fail(d,S::phase,0x274234);
}
bool OriginalUiGarageStaticModels50KG::populationCurrent()const noexcept {
    if(!populationComplete_||populated_!=count||!descriptorIdentityCurrent()||!visibility_||
       !visibility_->matchesColdConstruction(grant_,assets_))return false;
    const C* previous=nullptr;const C* child=list3C4_.firstChild18;
    for(std::size_t i=0;i<count;++i){const auto& wrapper=wrappers_[i];const auto& model=models_[i];
        if(!wrapper||entries34_[i]!=wrapper.get()||wrapper->type00!=1||wrapper->descriptor10||
           !model||wrapper->model8!=model.get()||!model->current()||!enrolled_[i]||!attached_[i]||
           model->source()!=assets_->staticModel(i)->model.get())return false;
        const auto* entity=model->scene_.modelEntity8();
        if(!entity||child!=entity||entity->parent1C!=&list3C4_||entity->previous10!=previous||
           entity->mask20||entity->descendants24||entity->firstChild18||entity->reference00!=1||
           !visibility_->staticMembershipCurrent(*this,i,*model))return false;
        previous=entity;child=entity->next14;
    }
    // Before the actual collision pass there is no suffix. After it starts,
    // only that exact owner's authored queries may follow these30 visuals.
    if(sceneOwner_&&sceneOwner_->sceneCollision_&&sceneOwner_->sceneCollision_->attempted_)
        return sceneOwner_->sceneCollision_->staticSuffixCurrent(*this,previous,child);
    return child==nullptr;
}
OriginalUiGarageStaticModels50KG::C* OriginalUiGarageStaticModels50KG::pageEntity3C4()noexcept {
    return populationCurrent()?&list3C4_:nullptr;
}
bool OriginalUiGarageStaticModels50KG::retirePage27B650(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(failedPageRetirement_)return R::fail(d,S::phase,0x27B650);
    return retirePageBody27B650(page,false,d);
}
bool OriginalUiGarageStaticModels50KG::retireFailedPageStatic27B650(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!page.partialSceneAttachmentFailureCurrent()||!sceneOwner_||!sceneOwner_->sceneCollision_||
       !sceneOwner_->sceneCollision_->failedRawListRetired27BB80(page)||
       (pageRetirement_!=PageRetirement::none&&!failedPageRetirement_))
        return R::fail(d,S::phase,0x27B650);
    return retirePageBody27B650(page,true,d);
}
bool OriginalUiGarageStaticModels50KG::retirePageBody27B650(
    const OriginalUiGaragePageOwner50KG& page,bool failedPrefix,D& d)noexcept {
    d={};
    if(pageRetirement_==PageRetirement::complete){
        if(pageRetirementOwner_==&page&&closed_&&!listLive_&&!retiring_&&
           !entries34_&&!list3C4_.reference00&&!list3C4_.firstChild18)return true;
        return R::fail(d,S::phase,0x27B700);
    }
    if(!grant_||!grant_->current()||!sceneOwner_||sceneOwner_->staticModels_.get()!=this||
       sceneOwner_->grant_!=grant_||sceneOwner_->assets_!=assets_||
       !sceneOwner_->sourceLoadReturned1FB59C()||sceneOwner_->numZones34C_!=1||
       !sceneOwner_->staticZoneLists3C4_||sceneOwner_->staticZoneLists3C4_[0]!=&list3C4_||
       !visibility_||visibility_!=sceneOwner_->visibility_||!sceneOwner_->sceneCollision_||
       closed_||(pageRetirementOwner_&&pageRetirementOwner_!=&page))
        return R::fail(d,S::phase,0x27B650);
    if(pageRetirement_<=PageRetirement::listReference){
        if(!listLive_||retiring_||
           !grant_->garagePageStaticListRetirementCurrent(page,*sceneOwner_,list3C4_))
            return R::fail(d,S::phase,0x142590);
    }else if(!retiring_||!grant_->garagePageStaticListBaseRetirementCurrent(page,*sceneOwner_,list3C4_))
        return R::fail(d,S::phase,0x27B680);
    if(pageRetirement_==PageRetirement::none){
        if(!descriptorsComplete_||!populationComplete_||populated_!=count||count30_!=count||
           !entries34_||(failedPrefix?!sceneOwner_->sceneCollision_->failedRawListRetired27BB80(page):
               !sceneOwner_->sceneCollision_->rawListRetired27BB80()))
            return R::fail(d,S::phase,0x27BB80);
        for(std::size_t i=0;i<count;++i){const auto& wrapper=wrappers_[i];const auto& model=models_[i];
            if(!wrapper||entries34_[i]!=wrapper.get()||wrapper->type00!=1||wrapper->descriptor10||
               !model||wrapper->model8!=model.get()||!model->current()||
               !enrolled_[i]||!attached_[i]||model->source()!=assets_->staticModel(i)->model.get())
                return R::fail(d,S::phase,0x1425D8);
        }
        failedPageRetirement_=failedPrefix;
        pageRetirementOwner_=&page;pageRetirement_=PageRetirement::queries;
    }
    if(pageRetirement_==PageRetirement::queries){
        if(failedPrefix){if(!sceneOwner_->sceneCollision_->retireFailedPageQueries1424E0(page,*this,d))return false;}
        else if(!sceneOwner_->sceneCollision_->retirePageQueries1424E0(page,*this,d))return false;
        pageRetirement_=PageRetirement::models;
    }
    if(pageRetirement_==PageRetirement::models){
        for(std::size_t end=count;end>0;--end){const auto i=end-1;auto& model=*models_[i];
            const auto& entity=model.scene_.model_.entity();
            if(model.closed_){
                if(model.pageRetirement_!=R::PageRetirement::complete||enrolled_[i]||attached_[i]||
                   entity.reference00)return R::fail(d,S::phase,0x289778);
                continue;
            }
            if(entity.parent1C!=&list3C4_||entity.next14||entity.firstChild18||entity.mask20||
               entity.previous10!=(i?models_[i-1]->scene_.modelEntity8():nullptr))
                return R::fail(d,S::phase,0x142564);
            if(!model.scene_.referenceReleased_){
                if(!enrolled_[i]||!attached_[i]||entity.reference00!=1||
                   !visibility_->preflightPageStaticObserver28976C(page,*this,i,model)||
                   !model.canRetireResources(model.scene_.model_.observer19C))
                    return R::fail(d,S::phase,0x240450);
                const auto step=grant_->retireModel(model.scene_);
                if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
                enrolled_[i]=false;attached_[i]=false;
            }else if(enrolled_[i]||attached_[i]||entity.reference00)
                return R::fail(d,S::phase,0x104970);
            // Normal source order: ref0,240450 packets,26E460 materials/
            //geometry/arrays,2896D0 graph children,280920 observer,1413E0.
            if(model.pageRetirement_<=R::PageRetirement::payloadsReleased&&
               !model.retirePagePayloads240450(d))return false;
            if(model.pageRetirement_==R::PageRetirement::payloadsReleased&&
               !model.retirePageSceneChildren2896D0(d))return false;
            if(model.pageRetirement_==R::PageRetirement::childrenReleased&&
               !visibility_->retirePageStaticObserver28976C(page,*this,i,model,d))return false;
            if(model.pageRetirement_==R::PageRetirement::observerReleased&&
               !model.finishPageSceneBase1413E0(d))return false;
            if(!model.closed_)return R::fail(d,S::phase,0x289778);
        }
        if(list3C4_.firstChild18)return R::fail(d,S::phase,0x1425B8);
        pageRetirement_=PageRetirement::listReference;
    }
    if(pageRetirement_==PageRetirement::listReference){
        const auto step=grant_->beginGaragePageStaticListRetirement142590(page,*sceneOwner_,list3C4_);
        if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
        retiring_=true;pageRetirement_=PageRetirement::wrappers;
    }
    if(pageRetirement_==PageRetirement::wrappers){
        //27B698 forward wrapper destruction, then27B6F0 entries34 release.
        for(std::size_t i=0;i<count;++i){wrappers_[i].reset();if(entries34_)entries34_[i]=nullptr;}
        entries34_.reset();pageRetirement_=PageRetirement::base;
    }
    if(pageRetirement_==PageRetirement::base){
        const auto step=grant_->finishGaragePageStaticListRetirement1413E0(page,*sceneOwner_,list3C4_);
        if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
        retiring_=false;listLive_=false;closed_=true;populationComplete_=false;
        pageRetirement_=PageRetirement::complete;
    }
    //The Visibility owner intentionally retains dead primary/collision rows.
    //Its separate real SearchList destruction, not this callback, releases them.
    return true;
}
bool OriginalUiGarageStaticModels50KG::closeCold142590(D& d)noexcept {
    d={};if(closed_)return true;
    if(pageRetirement_!=PageRetirement::none)return R::fail(d,S::phase,0x27B650);
    if(!grant_||!grant_->current()||list3C4_.parent1C||list3C4_.previous10||list3C4_.next14)
        return R::fail(d,S::phase,0x142590);
    //142590 ->1424E0 destroys the sibling tail first, then first child. Keep
    // real memberships/resources and the list parent alive through this walk.
    for(std::size_t end=count;end>0;--end){const auto i=end-1;auto& model=models_[i];
        if(!model||model->closed_)continue;
        if(!enrolled_[i]){
            if(model->scene_.referenceReleased_){if(!model->releaseAfterEntity142590(d))return false;}
            else if(!model->closeUnpublished(d))return false;
            continue;
        }
        const auto* actual=model->scene_.model();const auto* observer=actual?actual->observer19C:nullptr;
        if(!model->canRetireResources(observer))return R::fail(d,S::phase,0x240450);
        if(observer&&(!visibility_||!visibility_->removeStatic28EA80(i,*model,d)))return false;
        if(!model->canRetireResources(nullptr))return R::fail(d,S::phase,0x240450);
        const auto step=grant_->retireModel(model->scene_);
        if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
        enrolled_[i]=false;attached_[i]=false;
        if(!model->releaseAfterEntity142590(d))return false;
    }
    if(listLive_&&!retiring_){
        const auto step=grant_->beginStaticListRetirement(list3C4_);
        if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
        retiring_=true;
    }
    //27B650 releases wrapper payloads/entries34 after104970, before1413E0.
    for(std::size_t i=0;i<count;++i){wrappers_[i].reset();if(entries34_)entries34_[i]=nullptr;}
    entries34_.reset();
    if(retiring_){
        const auto step=grant_->finishStaticListRetirement(list3C4_);
        if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
        retiring_=false;listLive_=false;
    }
    if(visibility_&&!visibility_->endStaticPopulationRetirement(*this,d))return false;
    closed_=true;populationComplete_=false;return true;
}
}
