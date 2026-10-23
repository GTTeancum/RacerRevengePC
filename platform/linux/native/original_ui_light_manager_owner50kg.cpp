#include "original_ui_light_manager_owner50kg.h"
#include "original_application_aural50kg.h"
#include "original_ui_garage_ui_lights50kg.h"
#include "original_ui_garage_scene_lighting50kg.h"
#include "original_ui_garage_visibility50kg.h"
#include "../../../src/game/original_ui_lit_geometry50km.h"
#include <cstring>
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <limits>
#include <initializer_list>
namespace rr::platform {
using K=game::OriginalWreckComponentKind50LA;
//24BD7C consumes these renderer-global rows once from the actual singleton.
// Source1499A0 does not perform per-model spatial selection or copy zone60.
bool OriginalUiLightManagerOwner50KG::readGarageGlobalLightRows1499A0(
    const ::original_main_entry50KG::Application& issuer,
    game::original_racer_lighting50KM::LightMatrices& out,Diagnostic& d)const noexcept {
    d={};
    if(issuer_!=&issuer||!current()||!sourceMutationAvailable()||!registryCurrent()||!defaultsKnown_)
        return fail(d,Status::identity,0x24bd7cu);
    std::array<float,4> ambient,color{};std::array<float,3> look{};
    std::memcpy(ambient.data(),default50_.data(),sizeof ambient);
    bool found=false;
    const auto read=[&](const auto& light)noexcept {
        if(field40_!=&light.entity)return true;
        if(found||!directional40Live_||!light.constructed||!light.configured||!light.registered||
           light.retiring||light.retired||light.type34!=2u||light.entity.reference00!=1)
            return false;
        // Do not read source Light3C.w:149A18..149A34 copies only RGB.
        // Its output alpha words are actual integer64 bit patterns1499B0.
        look=light.look78;for(unsigned i=0;i<3;++i)color[i]=light.diffuse3C[i];
        found=true;return true;
    };
    if(field40_){
        // Resolve only within the same retained registry owner. A dead/stale
        // source40 is a failure, not a null branch or a fallback UI light.
        if(!directional40Live_)return fail(d,Status::phase,0x1499dcu);
        if(uiLightsOwner_)for(const auto& light:uiLightsOwner_->lights_)
            if(light&&!read(*light))return fail(d,Status::identity,0x1499e8u);
        if(sceneLightsOwner_)for(const auto& light:sceneLightsOwner_->lights_)
            if(light&&!read(*light))return fail(d,Status::identity,0x1499e8u);
        if(!found)return fail(d,Status::identity,0x1499e8u);
    }else if(directional40Live_)return fail(d,Status::identity,0x1499dcu);
    if(game::projectOriginalUiGlobalLightRows50KM(found,look,color,ambient,out)!=
       game::original_racer_lighting50KM::Status::complete)return fail(d,Status::phase,0x1499a0u);
    d={Status::complete,0x149a40u};return true;
}
//149310 scans the actual full registry and retains its LAST type2 source
// Light. Its positionA0 operand is distinct from1499A0's selected look78.
bool OriginalUiLightManagerOwner50KG::readGarageParticleLight149310(
    const ::original_main_entry50KG::Application& issuer,ParticleLight149310& out,
    Diagnostic& d)const noexcept {
    d={};
    if(issuer_!=&issuer||!current()||!sourceMutationAvailable()||!registryCurrent()||
       !defaultsKnown_||count94_!=4||!registry9C_||
       !uiLightsOwner_||!sceneLightsOwner_)return fail(d,Status::identity,0x1493CC);
    ParticleLight149310 next;
    for(unsigned c=0;c<3;++c)std::memcpy(&next.ambient50[c],&default50_[c],sizeof(float));
    for(std::size_t ordinal=0;ordinal<count94_;++ordinal){
        bool found=false;
        const auto read=[&](const auto& light)noexcept {
            if(registry9C_[ordinal]!=&light.entity)return true;
            if(found||!light.constructed||!light.configured||!light.registered||
               light.retiring||light.retired||light.entity.reference00!=1||
               (light.type34!=2u&&light.type34!=4u))return false;
            found=true;
            if(light.type34==2u){
                next.lastType2=&light.entity;next.positionA0=light.positionA0;
                for(unsigned c=0;c<3;++c)next.diffuse3C[c]=light.diffuse3C[c];
            }
            return true;
        };
        for(const auto& light:uiLightsOwner_->lights_)
            if(light&&!read(*light))return fail(d,Status::identity,0x1493DC);
        for(const auto& light:sceneLightsOwner_->lights_)
            if(light&&!read(*light))return fail(d,Status::identity,0x1493DC);
        if(!found)return fail(d,Status::identity,0x1493D8);
    }
    out=next;d.sourcePC=0x149468;return true;
}
// Reached14A0A0 ->149BB0 small registry path. Read the actual manager9C
// order, actual zone ambient60 and real Light fields after their callbacks.
// No authored substitute, point-light omission or cacheA4 null fabrication.
bool OriginalUiLightManagerOwner50KG::selectGarageModel14A0A0(
    const OriginalUiGarageVisibility50KG& visibility,const OriginalUiGarageModelResource50KG& model,
    game::original_racer_lighting50KM::LightMatrices& out,Diagnostic& d)const noexcept {
    d={};std::uint32_t zone=0;
    if(!current()||!sourceMutationAvailable()||!registryCurrent()||
       !visibility.modelLightZone14A110(*this,model,zone))return fail(d,Status::identity,0x14a110u);
    if(cacheA4_)return fail(d,Status::phase,0x14a0e0u);
    if(count94_!=4||!registry9C_||!uiLightsOwner_||!sceneLightsOwner_||
       zone>=count90_||!ambient60_||!ambientKnown_)return fail(d,Status::phase,0x14a130u);
    game::original_racer_lighting50KM::SmallDirectionalRegistry input;
    input.count=count94_;std::memcpy(input.selectedZoneAmbient.data(),ambient60_[zone].data(),4u*sizeof(float));
    const auto append=[&](const auto& light,std::size_t ordinal)noexcept {
        if(!light.constructed||!light.configured||!light.registered||light.retiring||light.retired||
           registry9C_[ordinal]!=&light.entity||light.entity.reference00!=1||
           (light.type34!=2u&&light.type34!=4u))return false;
        auto& row=input.registered[ordinal];row.type=light.type34;row.direction78=light.look78;
        for(unsigned lane=0;lane<3;++lane){row.diffuse3C[lane]=light.diffuse3C[lane];row.specular4C[lane]=light.specular4C[lane];}
        return true;
    };
    for(std::size_t i=0;i<2;++i){
        if(!uiLightsOwner_->lights_[i]||!sceneLightsOwner_->lights_[i]||
           !append(*uiLightsOwner_->lights_[i],i)||!append(*sceneLightsOwner_->lights_[i],i+2u))
            return fail(d,Status::identity,0x149bb0u);
    }
    if(game::original_racer_lighting50KM::projectSmallDirectionalRegistry(input,out)!=
       game::original_racer_lighting50KM::Status::complete)return fail(d,Status::phase,0x149bb0u);
    d.sourcePC=0x14a184u;return true;
}
OriginalUiLightManagerOwner50KG::OriginalUiLightManagerOwner50KG(
    const ::original_main_entry50KG::Application* issuer,std::shared_ptr<E> entities,
    std::shared_ptr<game::OriginalUiLightManagerLifetime50KG> scalar,
    game::OriginalApplicationAnimationClockView50KG clock)noexcept
    :issuer_(issuer),entities_(std::move(entities)),scalar_(std::move(scalar)),clock_(std::move(clock)),generation_(key(entities_.get())){}
OriginalUiLightManagerOwner50KG::~OriginalUiLightManagerOwner50KG()noexcept {
    if(constructed_&&!retired_)std::terminate();
}
bool OriginalUiLightManagerOwner50KG::sourceIdentity()const noexcept {
    // Same-owner-thread-only, just like the shared Entity authority and clock.
    if(!entities_||entities_->nativeOwnerThread_!=std::this_thread::get_id())return false;
    std::uint32_t pause=0;
    if(!issuer_||!clock_.readPause11C(pause))return false; // Before raw Root reads.
    const auto issued=issuedOwner_.lock();
    const auto& retained=issuer_->sourceLightManager426F60Owner_;
    const auto& rootScalar=issuer_->lightManager;
    if(!issued||issued.get()!=this||!retained||retained.get()!=this||
       retained.owner_before(issued)||issued.owner_before(retained)||
       !scalar_||rootScalar!=scalar_||rootScalar.owner_before(scalar_)||scalar_.owner_before(rootScalar)||
       issuer_->entities()!=entities_||issuer_->entities().owner_before(entities_)||
       entities_.owner_before(issuer_->entities())||!clock_.sameOwner(issuer_->animationClock()))return false;
    return entities_->constructed_&&!entities_->released_&&!entities_->attachmentLock423F58_&&
        generation_==key(entities_.get())&&entities_->lightManagerNativeOwner_==this&&
        entities_->lightManager426F60_==&entity_&&scalar_->concreteBorrower_==this&&
        scalar_->concreteProjection_==&nativeProjectionBridge&&!scalar_->concreteProjectionRetired_&&
        scalar_->cacheState_==game::OriginalUiLightCacheState50KG::absentSinceApplicationConstruction&&
        scalar_->enteredFirstMain_==(nativeProjectionUiGeneration_!=0)&&
        scalar_->managerGeneration_==nativeProjectionUiGeneration_&&
        scalar_->activeUi_==nativeProjectionUi_&&
        scalar_->nativeGameplayGeneration_==nativeProjectionGameplayGeneration_&&
        !(nativeProjectionUi_&&nativeProjectionGameplayGeneration_);
}
bool OriginalUiLightManagerOwner50KG::current()const noexcept {
    if(!constructed_||retired_||!sourceIdentity()||entity_.kind!=K::lightManager147ED0||
       entity_.sessionGeneration!=generation_||entity_.objectKey!=key(&entity_)||entity_.reference00!=1||
       entity_.parent1C!=&entities_->root10_||entity_.mask20!=0x402u||entity_.firstChild18||
       !registryCurrent()||cacheA4_||fieldA8_!=-1||
       ((nativeProjectionUi_||nativeProjectionGameplayGeneration_)&&!nativeProjectionInputsEmpty()))return false;
    const auto view=entities_->entityAuthority_.view();
    return contains(view,&entity_)&&game::original_entity_chain_detail50LA::forest(view);
}
bool OriginalUiLightManagerOwner50KG::sourceMutationAvailable()const noexcept {
    return current()&&!nativeProjectionUi_&&!nativeProjectionGameplayGeneration_;
}
bool OriginalUiLightManagerOwner50KG::nativeProjectionInputsEmpty()const noexcept {
    // Root custody excludes even a Garage prefix before its first Light row.
    // Dead registry identities/defaults may persist after real source teardown.
    // No scalar projection mutates them or fabricates their source destruction.
    return issuer_&&!issuer_->garageCustodyOwner_&&
        !zonesAttempted_&&!zonesComplete_&&!count90_&&!nodes88_&&!lists8C_&&
        !ambient60_&&!ambient80_&&!directional44_&&!zone_&&!list_&&!listPublished_&&
        !uiLightsOwner_&&!sceneLightsOwner_&&!count94_&&!directional40Live_&&!registryFailed_;
}
bool OriginalUiLightManagerOwner50KG::nativeProjectionBridge(OriginalUiLightManagerOwner50KG* owner,
    const Scalar& producer,ProjectionOperation operation,
    const assets::OriginalUiArchiveIdentity50KG* identity,std::uint64_t generation)noexcept {
    if(!owner||owner->scalar_.get()!=&producer||producer.concreteBorrower_!=owner||
       producer.concreteProjection_!=&nativeProjectionBridge||!owner->current())return false;
    if(operation==ProjectionOperation::observe)return !identity&&!generation;
    if(!owner->nativeProjectionInputsEmpty())return false;
    // The callback is private to the scalar's real producer methods. Its exact
    // pre-state must agree with the Root-retained receipt. Each successful
    // mutation is followed only by nonthrowing scalar stores, with no callbacks.
    switch(operation){
    case ProjectionOperation::beginUi:{
        if(owner->nativeProjectionUi_||owner->nativeProjectionGameplayGeneration_||!identity||
           owner->nativeProjectionUiGeneration_==std::numeric_limits<std::uint64_t>::max()||
           generation!=owner->nativeProjectionUiGeneration_+1)return false;
        using Id=assets::OriginalUiArchiveId50KG;
        bool admitted=false;
        for(const auto id:{Id::mainMenu,Id::options,Id::rider,Id::course,Id::hallOfFame,Id::tournament})
            if(identity==assets::originalUiArchiveIdentity50KG(id)){admitted=true;break;}
        // Concrete Garage uses its own counted UI/Scene source producers. It
        // cannot borrow the ordinary rendering projection as those producers.
        if(!admitted||(!owner->nativeProjectionUiGeneration_&&
           identity!=assets::originalUiArchiveIdentity50KG(Id::mainMenu)))return false;
        owner->nativeProjectionUi_=identity;owner->nativeProjectionUiGeneration_=generation;return true;
    }
    case ProjectionOperation::endUi:
        if(!identity||identity!=owner->nativeProjectionUi_||generation!=owner->nativeProjectionUiGeneration_||
           owner->nativeProjectionGameplayGeneration_)return false;
        owner->nativeProjectionUi_=nullptr;return true;
    case ProjectionOperation::beginGameplay:
        if(identity||!generation||owner->nativeProjectionUi_||owner->nativeProjectionGameplayGeneration_||
           !owner->nativeProjectionUiGeneration_)return false;
        owner->nativeProjectionGameplayGeneration_=generation;return true;
    case ProjectionOperation::endGameplay:
        if(identity||!generation||owner->nativeProjectionUi_||generation!=owner->nativeProjectionGameplayGeneration_)return false;
        owner->nativeProjectionGameplayGeneration_=0;return true;
    case ProjectionOperation::observe:break;
    }
    return false;
}
bool OriginalUiLightManagerOwner50KG::constructFresh140074(const ::original_main_entry50KG::Application& root,
    std::shared_ptr<OriginalUiLightManagerOwner50KG>& output,Diagnostic& d)noexcept {
    d={};if(output)return fail(d,Status::phase,0x140074);
    const auto entities=root.entities();const auto scalar=root.lightManager;std::uint32_t pause=0;
    if(!entities||!scalar||scalar!=root.lightManagerStartupWitness_||
       scalar.owner_before(root.lightManagerStartupWitness_)||root.lightManagerStartupWitness_.owner_before(scalar)||
       !root.animationClock().readPause11C(pause))return fail(d,Status::owner,0x140074);
    if(entities->nativeOwnerThread_!=std::this_thread::get_id())return fail(d,Status::affinity,0x140074);
    // This private proposal can start only BEFORE any dispatcher or other
    // counted owner. Existing scalar menu history cannot mint source absence.
    if(!entities->constructed_||entities->released_||entities->attachmentLock423F58_||
       entities->objectCount423F60_!=2||entities->entityAuthority_.count_!=2||entities->entityAuthority_.nonRootEnrollmentObserved_||entities->root10_.firstChild18||
       entities->lightManager426F60_||entities->lightManagerNativeOwner_||entities->lightManagerGuard83A8_||
       scalar->concreteBorrower_||scalar->concreteProjection_||scalar->concreteProjectionRetired_||scalar->cacheState_!=game::OriginalUiLightCacheState50KG::absentSinceApplicationConstruction||
       scalar->enteredFirstMain_||scalar->activeUi_||scalar->managerGeneration_||scalar->nativeGameplayGeneration_)
        return fail(d,Status::phase,0x147F34);
    std::shared_ptr<OriginalUiLightManagerOwner50KG> next;
    try{next=std::shared_ptr<OriginalUiLightManagerOwner50KG>(new OriginalUiLightManagerOwner50KG(&root,entities,scalar,root.animationClock()));}
    catch(...){return fail(d,Status::allocation,0x140074);}
    next->issuedOwner_=next; // Native exact root-issued shared ownership witness.
    auto& authority=entities->entityAuthority_;
    if(!authority.reserveAdditional(1)||!authority.canEnroll(next->entity_,key(&next->entity_)))return fail(d,Status::allocation,0x141300);
    const auto init=game::initializeOriginalWreckComponent50LA(next->entity_,K::lightManager147ED0,
        next->generation_,key(&next->entity_),true,entities->objectCount423F60_);
    if(init.status!=game::OriginalWreckComponentStatus50LA::complete)std::terminate();
    next->constructed_=true;
    //147F10..1C lazy guard, then actual singleton publication and constructor
    // scalar/vector stores. All zero source fields have their own typed backing.
    entities->lightManagerAssertion426F88_=1;entities->lightManagerGuard83A8_=1;
    entities->lightManager426F60_=&next->entity_;next->vector30_=entities->lightVector426F10_;
    entities->lightManagerNativeOwner_=next.get();scalar->concreteBorrower_=next.get();
    scalar->concreteProjection_=&nativeProjectionBridge;
    if(!authority.enroll(next->entity_))std::terminate();
    const auto attach=authority.attachChildren(entities->root10_,next->entity_,0x402u,0);
    if(attach.status!=game::OriginalWreckComponentStatus50LA::complete)std::terminate();
    output=std::move(next);d={Status::complete,0x140094};return true;
}
bool OriginalUiLightManagerOwner50KG::constructAfterAuralNative140074(const ::original_main_entry50KG::Application& root,
    const OriginalApplicationAural50KG& aural,
    std::shared_ptr<OriginalUiLightManagerOwner50KG>& output,Diagnostic& d)noexcept {
    d={};if(output)return fail(d,Status::phase,0x140074);
    const auto entities=root.entities();const auto scalar=root.lightManager;std::uint32_t pause=0;
    if(!entities||!scalar||scalar!=root.lightManagerStartupWitness_||
       scalar.owner_before(root.lightManagerStartupWitness_)||root.lightManagerStartupWitness_.owner_before(scalar)||
       !root.animationClock().readPause11C(pause))return fail(d,Status::owner,0x140074);
    if(entities->nativeOwnerThread_!=std::this_thread::get_id())return fail(d,Status::affinity,0x140074);
    // Separate typed bounded native prefix, NOT a claim that retail virtual94
    // and texture-manager253D80 executed. Exactly the retained original Aural
    // may precede this Light; neither count3 nor a reset boolean is authority.
    if(root.sourceAural00Owner_.get()!=&aural||aural.issuer_!=&root||
       aural.entities_!=entities||!aural.exactAuralOnlyStartupPrefix()||
       !aural.nativeInitialized_||!aural.backendCurrent())return fail(d,Status::owner,0x140074);
    if(!entities->constructed_||entities->released_||entities->attachmentLock423F58_||
       entities->lightManager426F60_||entities->lightManagerNativeOwner_||entities->lightManagerGuard83A8_||
       scalar->concreteBorrower_||scalar->concreteProjection_||scalar->concreteProjectionRetired_||scalar->cacheState_!=game::OriginalUiLightCacheState50KG::absentSinceApplicationConstruction||
       scalar->enteredFirstMain_||scalar->activeUi_||scalar->managerGeneration_||scalar->nativeGameplayGeneration_)
        return fail(d,Status::phase,0x147F34);
    std::shared_ptr<OriginalUiLightManagerOwner50KG> next;
    try{next=std::shared_ptr<OriginalUiLightManagerOwner50KG>(new OriginalUiLightManagerOwner50KG(&root,entities,scalar,root.animationClock()));}
    catch(...){return fail(d,Status::allocation,0x140074);}
    next->issuedOwner_=next; // Native exact root-issued shared ownership witness.
    auto& authority=entities->entityAuthority_;
    if(!authority.reserveAdditional(1)||!authority.canEnroll(next->entity_,key(&next->entity_)))return fail(d,Status::allocation,0x141300);
    const auto init=game::initializeOriginalWreckComponent50LA(next->entity_,K::lightManager147ED0,
        next->generation_,key(&next->entity_),true,entities->objectCount423F60_);
    if(init.status!=game::OriginalWreckComponentStatus50LA::complete)std::terminate();
    next->constructed_=true;
    //147F10..1C lazy guard, then actual singleton publication and constructor
    // scalar/vector stores. All zero source fields have their own typed backing.
    entities->lightManagerAssertion426F88_=1;entities->lightManagerGuard83A8_=1;
    entities->lightManager426F60_=&next->entity_;next->vector30_=entities->lightVector426F10_;
    entities->lightManagerNativeOwner_=next.get();scalar->concreteBorrower_=next.get();
    scalar->concreteProjection_=&nativeProjectionBridge;
    if(!authority.enroll(next->entity_))std::terminate();
    const auto attach=authority.attachChildren(entities->root10_,next->entity_,0x402u,0);
    if(attach.status!=game::OriginalWreckComponentStatus50LA::complete)std::terminate();
    output=std::move(next);d={Status::complete,0x140094};return true;
}
bool OriginalUiLightManagerOwner50KG::zoneReleasePreflight()const noexcept {
    if(sceneLightsOwner_||!sourceMutationAvailable()||count90_!=(zonesAttempted_?1u:0u))return false;
    if(!zonesAttempted_)return !nodes88_&&!lists8C_&&!ambient60_&&!ambient80_&&!directional44_&&!zone_&&!list_;
    if(zone_){
        const auto& z=*zone_;const auto& c=z.entity;
        if(!z.constructed||!z.published||!nodes88_||nodes88_[0]!=zone_.get()||
           c.kind!=K::searchNode27F120||c.objectKey!=key(&c)||c.sessionGeneration!=generation_||
           c.reference00!=1||c.previous10||c.next14||c.firstChild18||c.parent1C||c.mask20||
           !entities_->entityAuthority_.canRetire(c)||z.field30||z.field3C||z.field40||z.field44||z.fieldA0||z.index48)
            return false;
        if(z.first34!=(list_&&list_->lightAliases_>=1?list_.get():nullptr)||
           z.second38!=(list_&&list_->lightAliases_>=2?list_.get():nullptr))return false;
    }else if(list_)return false;
    if(list_&&(!listPublished_||!lists8C_||lists8C_[0]!=list_.get()||
        !list_->lightEmptyCurrent(this,issuer_,entities_)||list_->lightReleasedAliases_))return false;
    return true;
}
bool OriginalUiLightManagerOwner50KG::setupOneZone148B60(Diagnostic& d)noexcept {
    d={};if(!zoneReleasePreflight()||(zonesAttempted_&&!zonesComplete_))return fail(d,Status::phase,0x148B60);
    if(entities_->objectCount423F60_==std::numeric_limits<std::uint32_t>::max()||
       !entities_->entityAuthority_.reserveAdditional(1))return fail(d,Status::allocation,0x148C08);
    constexpr Words defaults{{0x42480000u,0x42480000u,0x42480000u,0x42800000u}};
    default70_=defaults;default50_=defaults;defaultsKnown_=true; //148BA0/A8, before148110.
    if(!releaseZones148110(d))std::terminate(); // Complete preflight, no callbacks.
    zonesAttempted_=true;count90_=1; //148BB8 precedes first array allocation.
    std::uint32_t pc=0x148BB4;
    try {
        nodes88_.reset(new Zone*[1]);
        pc=0x148BC0;lists8C_.reset(new L*[1]);
        pc=0x148BD0;ambient60_.reset(new Words[1]);
        pc=0x148BDC;ambient80_.reset(new Words[1]);
        pc=0x148BE8;directional44_.reset(new Words[1]); // Contents remain UNKNOWN.
        pc=0x148C08;zone_=std::make_unique<Zone>();
        auto& z=*zone_;
        if(!entities_->entityAuthority_.canEnroll(z.entity,key(&z.entity)))std::terminate();
        const auto init=game::initializeOriginalWreckComponent50LA(z.entity,K::searchNode27F120,
            generation_,key(&z.entity),true,entities_->objectCount423F60_);
        if(init.status!=game::OriginalWreckComponentStatus50LA::complete||!entities_->entityAuthority_.enroll(z.entity))std::terminate();
        z.constructed=true;nodes88_[0]=&z;z.published=true; //148C38; no141960.
        pc=0x148C34;list_=std::unique_ptr<L>(new L(*issuer_,entities_));
        list_->lightManagerOwner_=this;L::D ld;
        const bool built=list_->construct27E260(issuer_,entities_,ld);
        lists8C_[0]=list_.get();listPublished_=true; //148C58, actual retained prefix.
        if(!built)return fail(d,Status::list,ld.sourcePC);
        if(!list_->assignLightAlias(this,issuer_,entities_,1,z.first34,ld)||
           !list_->assignLightAlias(this,issuer_,entities_,2,z.second38,ld))return fail(d,Status::list,ld.sourcePC);
        z.index48=0;ambient80_[0]=defaults;ambient60_[0]=defaults;ambientKnown_=true;
        zonesComplete_=true;d={Status::complete,0x148CD0};return true;
    }catch(...){return fail(d,Status::allocation,pc);}
}
bool OriginalUiLightManagerOwner50KG::releaseZones148110(Diagnostic& d)noexcept {
    d={};if(!zoneReleasePreflight())return fail(d,Status::phase,0x148110);
    // Native failed-prefix unwind touches only actually published slots. An
    // allocated but unwritten array slot is never read as a source null pointer.
    if(zone_){auto& z=*zone_;auto& c=z.entity;
        if(!entities_->entityAuthority_.retire(c))std::terminate();
        --c.reference00; //148150→104970.
        L::D ld;
        if(z.first34&&!list_->releaseLightAlias(this,issuer_,entities_,1,z.first34,ld))std::terminate();
        if(z.second38&&!list_->releaseLightAlias(this,issuer_,entities_,2,z.second38,ld))std::terminate();
        --entities_->objectCount423F60_;z.constructed=false;zone_.reset(); //27F23C after both edges.
    }
    nodes88_.reset(); //148170/178.
    if(list_){L::D ld;if(!list_->releaseLightPayload(this,issuer_,entities_,ld))std::terminate();list_.reset();}
    lists8C_.reset();listPublished_=false;count90_=0; //1481C8/1D0/1D4.
    ambient60_.reset();ambient80_.reset();directional44_.reset(); //1481E4/1FC/214.
    zonesAttempted_=false;zonesComplete_=false;ambientKnown_=false;directional44Known_=false;
    // Registry94/98/9C, cacheA4, source defaults50/70 and singleton untouched.
    d={Status::complete,0x148234};return true;
}
bool OriginalUiLightManagerOwner50KG::retireEmpty148020(Diagnostic& d)noexcept {
    d={};if(retired_)return true;
    if(!zoneReleasePreflight()||issuer_->garageCustodyOwner_||zonesAttempted_||uiLightsOwner_||count94_||directional40Live_||entity_.next14||entity_.firstChild18||
       !entities_->entityAuthority_.canRetire(entity_))return fail(d,Status::phase,0x148020);
    // Required empty-zone envelope avoids fabricating a safe replacement for
    //148020's source60/80/44 frees BEFORE its call148110. No hidden reset.
    if(entity_.previous10)entity_.previous10->next14=entity_.next14;
    else entities_->root10_.firstChild18=entity_.next14;
    if(!entities_->entityAuthority_.retire(entity_))std::terminate();
    --entity_.reference00;
    //148054 frees the actual registered-pointer backing, whose live prefix is empty.
    // Preserve stale source fields40/98 and dead row identities; do not dereference.
    registry9C_.reset();
    // Zone/cache payloads60/80/44/A4/88/8C are genuinely absent here.
    entities_->lightManager426F60_=nullptr; //1480D8, before1413E0 completes.
    --entities_->objectCount423F60_;
    entities_->lightManagerNativeOwner_=nullptr;scalar_->concreteBorrower_=nullptr;
    scalar_->concreteProjection_=nullptr;scalar_->concreteProjectionRetired_=true;retired_=true;
    d={Status::complete,0x1480D4};return true;
}
bool OriginalUiLightManagerOwner50KG::update1483E0(float elapsed,Diagnostic& d)noexcept {
    d={};
    if(!current()||(entity_.flags29&0x0du)!=1u||!(entity_.mask20&2u))
        return fail(d,Status::identity,0x1483E0);
    // The shared words are actual Application-lifetime BSS. Their source
    //key-event writer requires mask80 and therefore debug228 bit2; retain
    //that envelope rather than re-zeroing prior history for this callback.
    if(issuer_->sourceDebugCamera228Bit2_)
        return fail(d,Status::phase,0x1483E0);
    if(issuer_->sourceLightDebug426F50_)
        return fail(d,Status::phase,0x148408);
    if(issuer_->sourceLightDebug426F58_)
        return fail(d,Status::phase,0x1486EC);
    if(entities_->diagnostic14_)
        return fail(d,Status::phase,0x1489B8);
    //148B40 calls141ED0 with this SAME f12. The actual current Light has
    //no Entity children (its zones/registry are detached payload owners).
    //Thus141F84 returns1 without any child service or numeric operation.
    //No generic no-op assertion substitutes for the three reached branches.
    (void)elapsed;
    if(entity_.firstChild18)return fail(d,Status::topology,0x141F60);
    d.sourcePC=0x148B58;return true;
}
bool OriginalUiLightManagerOwner50KG::registryCurrent()const noexcept {
    if(count94_>4||registryWritten_>4||count94_>registryWritten_||fieldA0_>1)return false;
    if(capacity98_!=0&&capacity98_!=10)return false;
    if(registryFailed_){if(capacity98_!=10||registry9C_||count94_||registryWritten_)return false;}
    else if(bool(registry9C_)!=(capacity98_==10))return false;
    for(std::size_t i=0;i<registryWritten_;++i)
        if(!registry9C_||registry9C_[i]!=registryPublished_[i])return false;
    if(!uiLightsOwner_)return !count94_&&!directional40Live_&&!sceneLightsOwner_;
    const auto& ui=*uiLightsOwner_;
    if(!ui.bound_||!ui.scopeBorrowed()||ui.manager_.get()!=this||!ui.grant_||ui.issuer_!=issuer_||
       ui.application_!=entities_||ui.application_.owner_before(entities_)||entities_.owner_before(ui.application_))return false;
    bool currentDirectional=false;
    for(std::size_t i=0;i<count94_&&i<2;++i){
        if(!ui.lights_[i])return false;
        const auto& light=*ui.lights_[i];
        if(!ui.lightIdentity(light)||registry9C_[i]!=&light.entity||!light.constructed||light.retired||!light.registered||
           light.type34!=(i?2u:4u)||light.observerD0||light.fieldD8||
           light.entity.reference00!=(light.retiring?0u:1u))return false;
        if(light.type34==2&&field40_==&light.entity)currentDirectional=true;
    }
    if(sceneLightsOwner_){
        const auto& scene=*sceneLightsOwner_;
        if(!scene.managerBound_||!scene.identityCurrent()||scene.manager_.get()!=this||scene.application_!=entities_||
           scene.application_.owner_before(entities_)||entities_.owner_before(scene.application_)||scene.issuer_!=issuer_||count94_<2)
            return false;
        for(std::size_t i=2;i<count94_;++i){
            const auto& row=scene.lights_[i-2];if(!row)return false;const auto& light=*row;
            if(!scene.lightIdentity(light)||registry9C_[i]!=&light.entity||!light.constructed||!light.typed||!light.configured||
               !light.registered||light.retired||light.type34!=(i==2?4u:2u)||light.observerD0||light.fieldD8||
               light.entity.reference00!=(light.retiring?0u:1u))return false;
            if(light.type34==2&&field40_==&light.entity)currentDirectional=true;
        }
    }else if(count94_>2)return false;
    return !directional40Live_||currentDirectional;
}
bool OriginalUiLightManagerOwner50KG::bindUiLights(OriginalUiGarageUiLights50KG& owner,Diagnostic& d)noexcept {
    d={};if(!sourceMutationAvailable()||!zoneReleasePreflight()||(zonesAttempted_&&!zonesComplete_)||uiLightsOwner_||count94_||registryFailed_||
       !owner.bound_||owner.manager_.get()!=this||!owner.grant_||
       owner.issuer_!=issuer_||owner.application_!=entities_||
       owner.application_.owner_before(entities_)||entities_.owner_before(owner.application_))
        return fail(d,Status::identity,0x1CDB10);
    uiLightsOwner_=&owner;return true;
}
bool OriginalUiLightManagerOwner50KG::incrementUiRevision149300(OriginalUiGarageUiLights50KG& owner,Diagnostic& d)noexcept {
    d={};if(uiLightsOwner_!=&owner||!sourceMutationAvailable()||!owner.scopeCurrent()||field84_==std::numeric_limits<std::uint32_t>::max())
        return fail(d,Status::phase,0x149300);
    ++field84_;return true;
}
bool OriginalUiLightManagerOwner50KG::appendUiLight148F30(OriginalUiGarageUiLights50KG& owner,std::size_t ordinal,Diagnostic& d)noexcept {
    d={};if(uiLightsOwner_!=&owner||sceneLightsOwner_||!sourceMutationAvailable()||!zonesComplete_||registryFailed_||ordinal>=2||count94_!=ordinal||
       !owner.scopeCurrent()||!owner.lights_[ordinal])return fail(d,Status::phase,0x148F30);
    auto& l=*owner.lights_[ordinal];
    if(!owner.lightIdentity(l)||!l.constructed||!l.typed||!l.configured||l.registered||l.retired||l.retiring||l.entity.reference00!=1||
       l.entity.parent1C||l.entity.previous10||l.entity.next14||l.type34!=(ordinal?2u:4u)||l.observerD0||l.fieldD8)
        return fail(d,Status::identity,0x148F30);
    const auto word=[](float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;};
    if(l.type34==4){Words ambient{{word(l.ambient5C[0]),word(l.ambient5C[1]),word(l.ambient5C[2]),0x42800000u}};
        ambient80_[0]=ambient;ambient60_[0]=ambient;default70_=ambient;default50_=ambient;ambientKnown_=true;
    }else {
        vector30_={{word(l.diffuse3C[0]),word(l.diffuse3C[1]),word(l.diffuse3C[2]),0}};
        field40_=&l.entity;directional44_[0]=vector30_;directional44Known_=true;
        // Native liveness becomes true with actual registry publication below.
    }
    if(capacity98_<count94_+1){
        capacity98_=count94_+10; //149198 is BEFORE realloc, preserved on failure.
        try{registry9C_.reset(new const C*[capacity98_]);}
        catch(...){registryFailed_=true;return fail(d,Status::allocation,0x1491A4);}
    }
    registry9C_[count94_]=&l.entity;registryPublished_[count94_]=&l.entity;
    ++count94_;if(registryWritten_<count94_)registryWritten_=count94_;l.registered=true;
    if(l.type34==2)directional40Live_=true;
    d={Status::complete,0x1491CC};return true;
}
bool OriginalUiLightManagerOwner50KG::preflightRemoveUiLight1491F0(const OriginalUiGarageUiLights50KG& owner,std::size_t ordinal)const noexcept {
    if(uiLightsOwner_!=&owner||sceneLightsOwner_||!sourceMutationAvailable()||ordinal>=2||!owner.lights_[ordinal])return false;
    const auto& l=*owner.lights_[ordinal];
    if(!l.constructed||l.retired||l.observerD0||l.fieldD8||l.fieldC0||l.fieldC4)return false;
    if(l.type34!=0&&l.type34!=(ordinal?2u:4u))return false;
    if(l.registered)return count94_==ordinal+1&&registry9C_&&registry9C_[ordinal]==&l.entity;
    for(std::size_t i=0;i<count94_;++i)if(registry9C_[i]==&l.entity)return false;
    return count94_<=ordinal;
}
bool OriginalUiLightManagerOwner50KG::removeUiLight1491F0(OriginalUiGarageUiLights50KG& owner,std::size_t ordinal,Diagnostic& d)noexcept {
    d={};if(!preflightRemoveUiLight1491F0(owner,ordinal)||!owner.lights_[ordinal]->retiring||
       owner.lights_[ordinal]->entity.reference00)return fail(d,Status::phase,0x1491F0);
    auto& l=*owner.lights_[ordinal];
    // Source decrements the live bound while scanning, with NO compaction or
    // nulling. Preflight restricts this native owner to safe reverse-tail close.
    for(std::size_t i=0;i<count94_;++i)if(registry9C_[i]==&l.entity)--count94_;
    l.registered=false;if(field40_==&l.entity)directional40Live_=false;
    fieldA0_=0;
    for(std::size_t i=0;i<count94_;++i)if(owner.lights_[i]->type34==2){fieldA0_=1;break;}
    d={Status::complete,0x1492E4};return true;
}
bool OriginalUiLightManagerOwner50KG::releaseUiLightsBorrow(OriginalUiGarageUiLights50KG& owner,Diagnostic& d)noexcept {
    d={};if(uiLightsOwner_!=&owner||sceneLightsOwner_||!sourceMutationAvailable()||count94_||directional40Live_||count90_||nodes88_||lists8C_||
       ambient60_||ambient80_||directional44_||!owner.payloadCurrent())
        return fail(d,Status::phase,0x1CCB20);
    for(const auto& light:owner.lights_)if(light&&light->constructed&&!light->retired)return fail(d,Status::phase,0x1CCB20);
    uiLightsOwner_=nullptr;return true;
}
bool OriginalUiLightManagerOwner50KG::bindSceneLights(OriginalUiGarageSceneLighting50KG& owner,Diagnostic& d)noexcept {
    d={};
    if(!sourceMutationAvailable()||!zonesComplete_||count90_!=1||count94_!=2||!uiLightsOwner_||!uiLightsOwner_->current()||
       sceneLightsOwner_||registryFailed_||!owner.identityCurrent()||owner.manager_.get()!=this||
       owner.issuer_!=issuer_||owner.application_!=entities_||owner.application_.owner_before(entities_)||
       entities_.owner_before(owner.application_))return fail(d,Status::identity,0x273AA0);
    sceneLightsOwner_=&owner;owner.managerBound_=true;return true;
}
bool OriginalUiLightManagerOwner50KG::incrementSceneRevision149300(OriginalUiGarageSceneLighting50KG& owner,Diagnostic& d)noexcept {
    d={};
    if(sceneLightsOwner_!=&owner||!sourceMutationAvailable()||!owner.identityCurrent()||field84_==std::numeric_limits<std::uint32_t>::max())
        return fail(d,Status::phase,0x149300);
    ++field84_;return true;
}
bool OriginalUiLightManagerOwner50KG::appendSceneLight148F30(OriginalUiGarageSceneLighting50KG& owner,std::size_t ordinal,Diagnostic& d)noexcept {
    d={};
    if(sceneLightsOwner_!=&owner||!sourceMutationAvailable()||!zonesComplete_||count90_!=1||ordinal>=2||count94_!=ordinal+2||
       !owner.identityCurrent()||!owner.lights_[ordinal]||capacity98_!=10||!registry9C_)
        return fail(d,Status::phase,0x148F30);
    auto& light=*owner.lights_[ordinal];
    if(!owner.lightIdentity(light)||!light.constructed||!light.typed||!light.configured||light.registered||light.retired||light.retiring||
       light.entity.reference00!=1||light.entity.parent1C||light.entity.previous10||light.entity.next14||
       light.type34!=(ordinal?2u:4u)||light.observerD0||light.fieldD8)return fail(d,Status::identity,0x148F30);
    const auto word=[](float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;};
    if(light.type34==4){
        const Words ambient{{word(light.ambient5C[0]),word(light.ambient5C[1]),word(light.ambient5C[2]),0x42800000u}};
        ambient80_[0]=ambient;ambient60_[0]=ambient;default70_=ambient;default50_=ambient;ambientKnown_=true;
    }else{
        vector30_={{word(light.diffuse3C[0]),word(light.diffuse3C[1]),word(light.diffuse3C[2]),0}};
        field40_=&light.entity;directional44_[0]=vector30_;directional44Known_=true;
    }
    //Actual original registry backing already has capacity10 from UI. These
    //two Scene rows append to it; there is no second light registry/container.
    registry9C_[count94_]=&light.entity;registryPublished_[count94_]=&light.entity;
    ++count94_;if(registryWritten_<count94_)registryWritten_=count94_;
    light.registered=true;if(light.type34==2)directional40Live_=true;
    d={Status::complete,0x1491CC};return true;
}
bool OriginalUiLightManagerOwner50KG::preflightRemoveSceneLight1491F0(const OriginalUiGarageSceneLighting50KG& owner,std::size_t ordinal)const noexcept {
    if(sceneLightsOwner_!=&owner||!sourceMutationAvailable()||ordinal>=2||!owner.lights_[ordinal])return false;
    const auto& light=*owner.lights_[ordinal];
    if(!light.constructed||light.retired||light.observerD0||light.fieldD8||light.fieldC0||light.fieldC4)return false;
    if(light.type34!=0&&light.type34!=(ordinal?2u:4u))return false;
    if(light.registered)return count94_==ordinal+3&&registry9C_&&registry9C_[ordinal+2]==&light.entity;
    for(std::size_t i=0;i<count94_;++i)if(registry9C_[i]==&light.entity)return false;
    return count94_<=ordinal+2;
}
bool OriginalUiLightManagerOwner50KG::removeSceneLight1491F0(OriginalUiGarageSceneLighting50KG& owner,std::size_t ordinal,Diagnostic& d)noexcept {
    d={};
    if(!preflightRemoveSceneLight1491F0(owner,ordinal)||!owner.lights_[ordinal]->retiring||owner.lights_[ordinal]->entity.reference00)
        return fail(d,Status::phase,0x1491F0);
    auto& light=*owner.lights_[ordinal];
    for(std::size_t i=0;i<count94_;++i)if(registry9C_[i]==&light.entity)--count94_;
    light.registered=false;if(field40_==&light.entity)directional40Live_=false;
    fieldA0_=0;
    for(std::size_t i=0;i<count94_;++i){
        const auto type=i<2?uiLightsOwner_->lights_[i]->type34:owner.lights_[i-2]->type34;
        if(type==2){fieldA0_=1;break;}
    }
    //1491F0 does NOT restore UI field40, ambient colors or directional colors.
    d={Status::complete,0x1492E4};return true;
}
bool OriginalUiLightManagerOwner50KG::releaseSceneLightsBorrow(OriginalUiGarageSceneLighting50KG& owner,Diagnostic& d)noexcept {
    d={};
    if(sceneLightsOwner_!=&owner||!sourceMutationAvailable()||count94_!=2)return fail(d,Status::phase,0x2781D0);
    for(const auto& row:owner.lights_)if(row&&row->constructed&&!row->retired)return fail(d,Status::phase,0x2781D0);
    sceneLightsOwner_=nullptr;owner.managerBound_=false;return true;
}
OriginalUiLightManagerOwner50KG::Observation OriginalUiLightManagerOwner50KG::inspect()const noexcept {
    Observation o;o.entity=constructed_?&entity_:nullptr;o.constructed=constructed_;o.retired=retired_;
    o.nativeProjectionUi=nativeProjectionUi_;o.nativeProjectionUiGeneration=nativeProjectionUiGeneration_;
    o.nativeProjectionGameplayGeneration=nativeProjectionGameplayGeneration_;
    o.vector30=vector30_;o.default50=default50_;o.default70=default70_;o.count90=count90_;
    o.arrays88=bool(nodes88_);o.arrays8C=bool(lists8C_);o.array60=bool(ambient60_);o.array80=bool(ambient80_);o.array44=bool(directional44_);
    o.zone=zone_&&zone_->published?&zone_->entity:nullptr;o.zonePublished=zone_&&zone_->published;
    o.list=listPublished_?list_.get():nullptr;o.listPublished=listPublished_;if(list_)o.listState=list_->snapshot();
    o.zonesComplete=zonesComplete_;o.ambientKnown=ambientKnown_;o.defaultsKnown=defaultsKnown_;
    if(ambientKnown_){o.ambient60=ambient60_[0];o.ambient80=ambient80_[0];}
    o.directional44Known=directional44Known_;if(directional44Known_)o.directional44=directional44_[0];
    o.revision84=field84_;o.count94=count94_;o.capacity98=capacity98_;o.fieldA0=fieldA0_;
    o.directional40Identity=reinterpret_cast<std::uintptr_t>(field40_);o.directional40Live=directional40Live_;
    o.registryBacking=bool(registry9C_);
    for(std::size_t i=0;i<registryWritten_;++i){
        const auto value=reinterpret_cast<std::uintptr_t>(registryPublished_[i]);
        if(i<2)o.registryIdentities[i]=value;else o.sceneRegistryIdentities[i-2]=value;
    }
    return o;
}
}
namespace original_main_entry50KG {
bool Application::stageLightManagerCold140074(std::shared_ptr<rr::platform::OriginalUiLightManagerOwner50KG>& out,
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalUiLightManagerOwner50KG;
    if(out||sourceLightManager426F60Owner_){d={O::Status::phase,0x140074};return false;}
    if(!O::constructFresh140074(*this,out,d))return false;
    sourceLightManager426F60Owner_=out;return true;
}
bool Application::releaseLightManagerCold148020(rr::platform::OriginalUiLightManagerOwner50KG& owner,
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalUiLightManagerOwner50KG;
    if(sourceLightManager426F60Owner_.get()!=&owner||owner.issuer_!=this||owner.entities_!=entitiesOwner_||
       owner.scalar_!=lightManager||owner.scalar_.owner_before(lightManager)||lightManager.owner_before(owner.scalar_)||
       !owner.clock_.sameOwner(animationClock())){d={O::Status::identity,0x148020};return false;}
    if(!owner.retireEmpty148020(d))return false;
    sourceLightManager426F60Owner_.reset();return true;
}
}

namespace original_main_entry50KG {
bool Application::stageLightManagerAfterAuralNative140074(std::shared_ptr<rr::platform::OriginalUiLightManagerOwner50KG>& out,
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic& d)const noexcept {
    using O=rr::platform::OriginalUiLightManagerOwner50KG;
    if(out||sourceLightManager426F60Owner_||!sourceAural00Owner_){d={O::Status::phase,0x140074};return false;}
    if(!O::constructAfterAuralNative140074(*this,*sourceAural00Owner_,out,d))return false;
    sourceLightManager426F60Owner_=out;return true;
}
}

namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::serviceSceneLightShared147A00(
    OriginalUiGarageSceneLighting50KG& owner,const C& node,Diagnostic& d)noexcept {
    d={};
    if(sceneLightsOwner_!=&owner||!sourceMutationAvailable()||!owner.identityCurrent()||
       owner.application_.get()!=entities_.get()||owner.application_.owner_before(entities_)||
       entities_.owner_before(owner.application_)||!entities_->entityAuthority_.canRetire(node))
        return fail(d,Status::identity,0x147A00);
    bool actual=false;
    for(const auto& row:owner.lights_)if(row && &row->entity==&node){
        actual=owner.lightIdentity(*row)&&row->constructed&&row->configured&&row->registered&&
            row->attached&&!row->retiring&&!row->retired&&!row->fieldD8;break;
    }
    if(!actual)return fail(d,Status::identity,0x147A00);
    //Actual startup100008 BSS ->147A14/18 first use. This shared state is
    //not LightManager payload:148020 and menu reentry leave it untouched.
    if(!entities_->lightUpdateGuard83A0_){
        entities_->lightUpdateGeometry426F78_=0;
        entities_->lightUpdateGuard83A0_=1;
    }
    //No source enable producer was recovered. Preserve any nonzero state;
    //do not replace its camera/geometry path with a fabricated reset.
    if(entities_->lightUpdateGeometry426F78_)return fail(d,Status::phase,0x147A24);
    d.sourcePC=0x147EB0;return true;
}
}
