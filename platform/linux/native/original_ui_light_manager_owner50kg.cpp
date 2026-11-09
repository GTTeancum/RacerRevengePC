#include "original_ui_light_manager_owner50kg.h"
#include "original_ui_course_owned_lifecycle50kg.h"
#include "original_application_aural50kg.h"
#include "original_ui_garage_ui_lights50kg.h"
#include "original_ui_garage_scene_lighting50kg.h"
#include "original_ui_garage_visibility50kg.h"
#include "../../../src/game/original_ui_lit_geometry50km.h"
#include <cstring>
#include <cmath>
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_constructor50la.h"
#include "../../../tests/native/ra_cold_aggregate50la.h"
#include "../../../src/game/original_gc_gameplay_lighting_candidate.h"
#include <limits>
#include <initializer_list>
#include "original_ui_light_manager_source_portal_guards50la.h"
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
       ((nativeProjectionUi_||nativeProjectionGameplayGeneration_)&&
        !(raZones_?raZonesReleasePreflight():nativeProjectionInputsEmpty())))return false;
    const auto view=entities_->entityAuthority_.view();
    return contains(view,&entity_)&&game::original_entity_chain_detail50LA::forest(view);
}
bool OriginalUiLightManagerOwner50KG::sourceMutationAvailable()const noexcept {
    return current()&&!nativeProjectionUi_&&!nativeProjectionGameplayGeneration_&&!raZones_&&!courseZoneOwner_;
}
bool OriginalUiLightManagerOwner50KG::nativeProjectionInputsEmpty()const noexcept {
    // Root custody excludes even a Garage prefix before its first Light row.
    // Dead registry identities/defaults may persist after real source teardown.
    // No scalar projection mutates them or fabricates their source destruction.
    return issuer_&&!issuer_->garageCustodyOwner_&&!courseZoneOwner_&&!raZones_&&
        !zonesAttempted_&&!zonesComplete_&&!count90_&&!nodes88_&&!lists8C_&&
        !ambient60_&&!ambient80_&&!directional44_&&!zone_&&!list_&&!listPublished_&&
        !uiLightsOwner_&&!sceneLightsOwner_&&!count94_&&!directional40Live_&&!registryFailed_;
}
bool OriginalUiLightManagerOwner50KG::nativeProjectionBridge(OriginalUiLightManagerOwner50KG* owner,
    const Scalar& producer,ProjectionOperation operation,
    const assets::OriginalUiArchiveIdentity50KG* identity,std::uint64_t generation)noexcept {
    if(!owner||owner->scalar_.get()!=&producer||producer.concreteBorrower_!=owner||
       producer.concreteProjection_!=&nativeProjectionBridge||!owner->current()){
        // Diagnostic only: which term of current() refused the projection.
        if(owner){const auto& o=*owner;
            std::fprintf(stderr,"light_projection refused op=%u constructed=%d retired=%d source=%d registry=%d cacheA4=%d fieldA8=%d ui=%d gameplay=%llu ra=%d inputsEmpty=%d issuer=%d garageCustody=%d courseZone=%d zones=%d/%d count90=%u lights=%d/%d\n",
                unsigned(operation),int(o.constructed_),int(o.retired_),int(bool(o.sourceIdentity())),int(o.registryCurrent()),int(bool(o.cacheA4_)),int(o.fieldA8_),
                int(bool(o.nativeProjectionUi_)),static_cast<unsigned long long>(o.nativeProjectionGameplayGeneration_),int(bool(o.raZones_)),
                int(o.nativeProjectionInputsEmpty()),int(bool(o.issuer_)),int(o.issuer_&&o.issuer_->garageCustodyOwner_),int(bool(o.courseZoneOwner_)),
                int(o.zonesAttempted_),int(o.zonesComplete_),unsigned(o.count90_),int(bool(o.uiLightsOwner_)),int(bool(o.sceneLightsOwner_)));}
        return false;}
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
        // UIUnlock 10 / UIWin 7 are ordinary UI managers too (the scalar
        // producer already admits both archives at 1CCD88).
        for(const auto id:{Id::mainMenu,Id::options,Id::rider,Id::course,Id::hallOfFame,Id::tournament,Id::unlock,Id::win})
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
    if(courseZoneOwner_||sceneLightsOwner_||!sourceMutationAvailable()||count90_!=(zonesAttempted_?1u:0u))return false;
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
    if(count94_>185||registryWritten_>190||count94_>registryWritten_||fieldA0_>1)return false;
    if(!raZones_&&count94_>4u)return false; //Dead cohort backing/history survives World retirement.
    if(raRegistryGrowthFailed_&&(capacity98_<20u||capacity98_>190u||capacity98_%10u||
       registryWritten_>capacity98_-10u||count94_>capacity98_-10u||!registry9C_))return false;
    if(capacity98_>190u||capacity98_%10u||registryWritten_>capacity98_)return false;
    if(registryFailed_){if(capacity98_!=10||registry9C_||count94_||registryWritten_)return false;}
    else if(bool(registry9C_)!=(capacity98_!=0))return false;
    for(std::size_t i=0;i<registryWritten_;++i)
        if(!registry9C_||registry9C_[i]!=registryPublished_[i])return false;
    if(raZones_&&raZones_->sourceMode&&raZones_->sourceAmbientRegistered){
        const auto& r=*raZones_;
        const bool ambientLive=!r.sourceAmbientWithdrawn;
        const bool secondLive=r.sourceSecondRegistered&&!r.sourceSecondWithdrawn;
        if((!r.world&&(ambientLive||secondLive||!r.sourceNativeClosed))||
           count94_!=std::uint32_t(ambientLive)+std::uint32_t(secondLive)||capacity98_!=10||
           registryWritten_!=1u+std::size_t(r.sourceSecondRegistered)||!registry9C_||
           r.firstAmbient||r.firstDirectional||directional40Live_||directional44Known_||uiLightsOwner_||sceneLightsOwner_)
            return false;
        if(ambientLive){
            if(!r.sourceAmbient||registry9C_[0]!=r.sourceAmbient||registryPublished_[0]!=r.sourceAmbient||
               !r.world->sourceFirstLight_||&r.world->sourceFirstLight_->entity!=r.sourceAmbient||
               !r.world->sourceFirstLight_->registered||!r.world->preflightSourceFirstLightNative())return false;
        }else if(r.sourceAmbient||secondLive)return false;
        if(r.sourceSecondRegistered){
            if(!r.sourceSecondRegistryReceipt||registry9C_[1]!=r.sourceSecondRegistryReceipt||
               registryPublished_[1]!=r.sourceSecondRegistryReceipt||field40_!=r.sourceSecondRegistryReceipt)return false;
            if(secondLive&&(!r.world->sourceSecondLight_||&r.world->sourceSecondLight_->entity!=r.sourceSecondRegistryReceipt||
               !r.world->sourceSecondLight_->registered||!r.world->preflightSourceSecondLightNative()))return false;
            //Withdrawn receipts are address-only history, never dereferenced.
        }else if(r.sourceSecondRegistryReceipt||r.sourceSecondWithdrawn)return false;
        return true; //Source registry custody only, not generic renderer readiness.
    }
    if(raZones_&&raZones_->firstAmbient){
        const auto& r=*raZones_;const auto* c=r.firstAmbient;
        if(uiLightsOwner_||sceneLightsOwner_||count94_!=(1u+bool(r.firstDirectional)+r.localRegisteredCount())||
           directional40Live_!=bool(r.firstDirectional)||!registry9C_||registry9C_[0]!=c||!r.world||
           !r.world->nativeAmbientLightCurrent(c))return false;
        if(r.firstDirectional&&(registry9C_[1]!=r.firstDirectional||field40_!=r.firstDirectional||
           !r.world->nativeDirectionalLightCurrent(r.firstDirectional)))return false;
        bool gap=false;for(std::size_t i=0;i<183u;++i){auto* light=r.localRegistered[i];
            if(!light){gap=true;continue;}
            if(gap||!r.firstDirectional||registry9C_[i+2]!=light||!r.world->nativeLocalLightCurrent(light))return false;}
        return true;
    }
    if(courseZoneOwner_){
        const auto& owner=*courseZoneOwner_;
        if(uiLightsOwner_||sceneLightsOwner_||raZones_||owner.issuer_!=issuer_||
           owner.courseLight_.get()!=this||count94_>2)return false;
        bool directional=false;
        for(std::size_t i=0;i<count94_;++i){
            const auto& p=owner.courseUiLights_[i];
            if(!p||!owner.courseLightEntityCurrent(p->entity,i)||!p->constructed||!p->typed||
               !p->configured||!p->registered||p->retired||p->type34!=(i?2u:4u)||
               p->observerD0||p->fieldD8||!registry9C_||registry9C_[i]!=&p->entity)return false;
            if(p->type34==2&&field40_==&p->entity)directional=true;
        }
        return directional40Live_==directional;
    }
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
        if(i<2)o.registryIdentities[i]=value;else if(i-2<o.sceneRegistryIdentities.size())o.sceneRegistryIdentities[i-2]=value;
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

namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::raZonesScopeCurrent()const noexcept {
    if(!raZones_||raZones_->sourceMode||!sourceIdentity()||!nativeProjectionGameplayGeneration_||nativeProjectionUi_||
       issuer_->garageCustodyOwner_||raZones_->gameplayGeneration!=nativeProjectionGameplayGeneration_||
       !raZones_->world||!raZones_->canonical)return false;
    const auto& race=issuer_->sourceRace28COwner_;
    if(!race||!race->counted_||race->rolledBack_||race->application_!=entities_||race->world74_.get()!=raZones_->world||
       race->resultsFeedbackThread_!=std::this_thread::get_id()||
       race->nativeTrackLoadStep_!=game::OriginalRaceConstructor50LA::NativeTrackLoadStep::aubRetired)return false;
    const auto* world=raZones_->canonical->world();
    return world&&world->zones.size()==10u;
}
bool OriginalUiLightManagerOwner50KG::raZonesReleasePreflight()const noexcept {
    if(!raZonesScopeCurrent()||zone_||list_||listPublished_||uiLightsOwner_||sceneLightsOwner_||
       count94_!=(std::uint32_t(bool(raZones_->firstAmbient))+std::uint32_t(bool(raZones_->firstDirectional))+raZones_->localRegisteredCount())||
       directional40Live_!=bool(raZones_->firstDirectional)||registryFailed_||raZones_->next>10u||
       count90_!=(zonesAttempted_?10u:0u)||(zonesComplete_&&raZones_->next!=10u))return false;
    const auto& r=*raZones_;std::uint32_t counted=0;
    for(std::size_t i=0;i<183u;++i)if(auto* entity=r.localRegistered[i]){
        const auto* light=r.world->nativeLocalLightFor(entity);
        if(!r.firstDirectional||!light||light->ordinalC8!=static_cast<std::int32_t>(i+2u))return false;
        if(light->type34==4u){if(i+2u<172u||i+2u>174u||r.localPrimary[i]||light->observerOwner||light->observerD0)return false;}
        else if((light->type34!=1u&&light->type34!=3u)||entity!=r.localPrimary[i])return false;
    }
    if(r.portalLinksPublished>20u)return false;
    const auto* authored=r.canonical->world();
    if(!authored||authored->portals.size()!=10u)return false;
    bool gap=false;std::size_t portalCount=0;
    for(std::size_t i=0;i<r.portals.size();++i){const auto* p=r.portals[i];
        if(!p){gap=true;continue;}
        if(gap||i>r.portalLinksPublished/2u||!zonesComplete_||!r.portalParent||
           p->parent1C!=r.portalParent||p->mask20||p->firstChild18||
           !entities_->entityAuthority_.canRetire(*p))return false;
        //A next actual source Portal can precede its148D00 call. Validate
        //against SAME private World-owned ordered tail, not arbitrary extra
        //children that merely happen to share parent/count authority.
        if(!r.world->nativePortalTailCurrent148D00(r.portalParent,p)||
           (p->next14&&!r.world->nativePortalTailCurrent148D00(r.portalParent,p->next14)))return false;
        ++portalCount;
    }
    if((portalCount&&!entities_->entityAuthority_.canRetire(*r.portalParent))||
       (!portalCount&&r.portalParent)||portalCount<(r.portalLinksPublished+1u)/2u)return false;
    std::array<const RaPortalLink27F3B0*,10> heads{};
    for(std::size_t i=0;i<r.portalLinks.size();++i){
        if(i>=r.portalLinksPublished){if(r.portalLinks[i])return false;continue;}
        const auto ordinal=i/2u;const auto& labels=authored->portals[ordinal].zones;
        if(!labels[0]||labels[0]>10u||!labels[1]||labels[1]>10u||labels[0]==labels[1])return false;
        const auto from=labels[i%2u]-1u,to=labels[1u-i%2u]-1u;
        const auto* link=r.portalLinks[i].get();
        if(!link||!r.zones[from]||!r.zones[to]||link->next00!=heads[from]||
           link->neighbor04!=&r.zones[to]->entity||link->portal08!=r.portals[ordinal]||
           (link->distanceWritten?(link->field0C!=link->field10||link->field0C>0x7f7fffffu):
                                  (link->field0C||link->field10!=0x7f7fffff)))return false;
        heads[from]=link;
    }
    for(std::size_t i=0;i<10;++i){const auto& zone=r.zones[i];const auto& list=r.lists[i];
        if(i>r.next&&(zone||list||r.listPublished[i]||r.ambientKnown[i]))return false;
        if(!zone){if(list||r.listPublished[i]||r.ambientKnown[i]||i<r.next)return false;continue;}
        const auto& z=*zone;const auto& c=z.entity;++counted;
        if(!zonesAttempted_||!nodes88_||!lists8C_||!ambient60_||!ambient80_||!directional44_||
           !z.constructed||!z.published||nodes88_[i]!=zone.get()||
           c.kind!=K::searchNode27F120||c.objectKey!=key(&c)||c.sessionGeneration!=generation_||
           c.reference00!=1||c.previous10||c.next14||c.firstChild18||c.parent1C||c.mask20||
           c.descendants24||c.category28!=0xff||(c.flags29&15u)!=3u||c.nativeFlags29||
           !entities_->entityAuthority_.canRetire(c)||
           z.field30!=heads[i]||
           z.field3C||z.field40||z.field44||z.fieldA0||
           z.first34!=(list&&list->lightAliases_>=1?list.get():nullptr)||
           z.second38!=(list&&list->lightAliases_>=2?list.get():nullptr))return false;
        if(list){if(!r.listPublished[i]||lists8C_[i]!=list.get()||
            !list->lightHistoryCurrent(this,issuer_,entities_)||list->lightReleasedAliases_)return false;
            for(auto* member:list->lightMembers_)if(member){bool found=false;for(auto* p:r.localPrimary)if(p==member)found=true;if(!found)return false;}}
        else if(r.listPublished[i]||r.ambientKnown[i])return false;
        if(i<r.next&&(!list||!list->complete_||list->lightAliases_!=2||!r.ambientKnown[i]||z.index48!=i))return false;
    }
    return counted<=entities_->objectCount423F60()&&localMembershipsCurrent();
}
bool OriginalUiLightManagerOwner50KG::bindRaWorldZones272A48(
    const ::original_main_entry50KG::Application& root,const ::ra_cold_aggregate50LA::Owner& world,
    const std::shared_ptr<game::original_gc_gameplay_lighting_candidate::Owner>& canonical,
    std::shared_ptr<OriginalUiLightManagerOwner50KG>& output,Diagnostic& d)noexcept {
    d={};const auto manager=root.sourceLightManager426F60Owner_;
    if(!manager||!manager->current()||!canonical||!canonical->world()||canonical->world()->zones.size()!=10u||
       !root.sourceRace28COwner_||root.sourceRace28COwner_->world74_.get()!=&world||
       !manager->nativeProjectionGameplayGeneration_||manager->nativeProjectionUi_||
       (output&&output!=manager))return fail(d,Status::identity,0x272a48);
    if(manager->raZones_){
        if(manager->raZones_->world!=&world||manager->raZones_->canonical!=canonical||
           !manager->raZonesReleasePreflight())return fail(d,Status::phase,0x272a48);
    }else{
        if(!manager->nativeProjectionInputsEmpty())return fail(d,Status::phase,0x272a48);
        try{
            auto scope=std::make_unique<RaZones148B60>();scope->world=&world;scope->canonical=canonical;
            scope->gameplayGeneration=manager->nativeProjectionGameplayGeneration_;
            manager->raZones_=std::move(scope);
        }catch(...){return fail(d,Status::allocation,0x272a48);}
    }
    output=manager; //Retain failed source setup prefix for explicit World cleanup.
    return manager->setupRaZones148B60(d);
}
bool OriginalUiLightManagerOwner50KG::setupRaZones148B60(Diagnostic& d)noexcept {
    d={};if(!raZonesReleasePreflight())return fail(d,Status::phase,0x148b60);
    if(zonesComplete_){d={Status::complete,0x148cf4};return true;}
    auto& r=*raZones_;
    if(!zonesAttempted_){
        if(entities_->objectCount423F60()>std::numeric_limits<std::uint32_t>::max()-10u||
           !entities_->entityAuthority_.reserveAdditional(10))return fail(d,Status::allocation,0x148c08);
        //Exact empty prior concrete payload proved at private scope issuance;
        //148110 has no nodes/lists/arrays to release. This is NOT a scalar reset
        //or proof that canonical native gameplay lighting has no authored rows.
        constexpr Words defaults{{0x42480000u,0x42480000u,0x42480000u,0x42800000u}};
        default70_=defaults;default50_=defaults;defaultsKnown_=true;
        zonesAttempted_=true;count90_=10; //148BB8 before first array allocation.
    }
    std::uint32_t pc=0x148bb4;
    try{
        if(!nodes88_)nodes88_.reset(new Zone*[10]);
        pc=0x148bc0;if(!lists8C_)lists8C_.reset(new L*[10]);
        pc=0x148bd0;if(!ambient60_)ambient60_.reset(new Words[10]);
        pc=0x148bdc;if(!ambient80_)ambient80_.reset(new Words[10]);
        pc=0x148be8;if(!directional44_)directional44_.reset(new Words[10]); //UNWRITTEN/UNKNOWN.
        while(r.next<10){const auto i=r.next;
            if(!r.zones[i]){
                pc=0x148c08;auto zone=std::make_unique<Zone>();auto& c=zone->entity;
                if(!entities_->entityAuthority_.canEnroll(c,key(&c)))return fail(d,Status::topology,0x148c1c);
                const auto initialized=game::initializeOriginalWreckComponent50LA(c,K::searchNode27F120,
                    generation_,key(&c),true,entities_->objectCount423F60_);
                if(initialized.status!=game::OriginalWreckComponentStatus50LA::complete||
                   !entities_->entityAuthority_.enroll(c))std::terminate();
                zone->constructed=true;zone->published=true;r.zones[i]=std::move(zone);
                pc=0x148c38;nodes88_[i]=r.zones[i].get(); //No141960 parent attachment.
            }
            auto& z=*r.zones[i];
            if(!r.lists[i]){
                pc=0x148c34;auto list=std::unique_ptr<L>(new L(*issuer_,entities_));
                list->lightManagerOwner_=this;L::D ld;
                const bool built=list->construct27E260(issuer_,entities_,ld);
                r.lists[i]=std::move(list);lists8C_[i]=r.lists[i].get();r.listPublished[i]=true;
                if(!built)return fail(d,Status::list,ld.sourcePC); //Published real partial list, not null replacement.
            }
            auto& list=*r.lists[i];L::D ld;
            if(!list.complete_)return fail(d,Status::list,0x27e260); //No constructor replay after failed prefix.
            pc=0x148c70;
            if(list.lightAliases_<1&&!list.assignLightAlias(this,issuer_,entities_,1,z.first34,ld))
                return fail(d,Status::list,ld.sourcePC);
            pc=0x148c8c;
            if(list.lightAliases_<2&&!list.assignLightAlias(this,issuer_,entities_,2,z.second38,ld))
                return fail(d,Status::list,ld.sourcePC);
            pc=0x148ca4;z.index48=static_cast<std::uint32_t>(i);
            pc=0x148cbc;ambient80_[i]=default70_;
            pc=0x148cd0;ambient60_[i]=default50_;r.ambientKnown[i]=true;++r.next;
        }
        ambientKnown_=true;zonesComplete_=true;d={Status::complete,0x148cf4};return true;
    }catch(...){return fail(d,Status::allocation,pc);}
}
bool OriginalUiLightManagerOwner50KG::releaseRaZones148110(
    const ::ra_cold_aggregate50LA::Owner& world,Diagnostic& d)noexcept {
    d={};if(!raZones_||raZones_->world!=&world||!raZonesReleasePreflight()||count94_)return fail(d,Status::phase,0x148110);
    for(auto* member:raZones_->localPrimary)if(member)return fail(d,Status::phase,0x148110);
    auto& r=*raZones_;
    //Original148110: ALL node releases (each alias34 then38) precede88free,
    //then ALL list final payload releases and8Cfree; never interleave pairs.
    for(std::size_t i=0;i<10;++i){if(!r.zones[i])continue;auto& z=*r.zones[i];auto& c=z.entity;
        if(!entities_->entityAuthority_.retire(c))std::terminate();--c.reference00;
        //27F180 frees node30 links BEFORE aliases34/38. Actual Portal remains
        //live until later aggregate topology teardown. Previously retired
        //neighbor nodes are NEVER dereferenced by these raw-link deletions.
        while(z.field30){
            const auto* dying=static_cast<const RaPortalLink27F3B0*>(z.field30);
            const auto* next=dying->next00;bool found=false;
            for(auto& link:r.portalLinks)if(link.get()==dying){link.reset();found=true;break;}
            if(!found)std::terminate();z.field30=next;
        }
        L::D ld;
        if(z.first34&&!r.lists[i]->releaseLightAlias(this,issuer_,entities_,1,z.first34,ld))std::terminate();
        if(z.second38&&!r.lists[i]->releaseLightAlias(this,issuer_,entities_,2,z.second38,ld))std::terminate();
        --entities_->objectCount423F60_;z.constructed=false;r.zones[i].reset();
    }
    r.portals.fill(nullptr);r.portalParent=nullptr;r.portalLinksPublished=0;
    nodes88_.reset();
    for(std::size_t i=0;i<10;++i)if(r.lists[i]){L::D ld;
        if(!r.lists[i]->releaseLightPayload(this,issuer_,entities_,ld))std::terminate();r.lists[i].reset();
    }
    lists8C_.reset();count90_=0;
    ambient60_.reset();ambient80_.reset();directional44_.reset();
    zonesAttempted_=false;zonesComplete_=false;ambientKnown_=false;directional44Known_=false;
    raZones_.reset(); //Canonical native lighting/World stay alive; only private custody borrow ends.
    d={Status::complete,0x148234};return true;
}
}

namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::linkRaPortal148D00(
    const ::ra_cold_aggregate50LA::Owner& world,std::size_t ordinal,const C& portal,const C& parent,
    std::uint32_t zoneA,std::uint32_t zoneB,Diagnostic& d)noexcept {
    d={};if(!raZones_||raZones_->world!=&world||!raZonesReleasePreflight()||!zonesComplete_||
       ordinal>=10u||zoneA>=10u||zoneB>=10u||zoneA==zoneB||portal.parent1C!=&parent||
       portal.mask20||portal.firstChild18||!entities_->entityAuthority_.canRetire(portal)||
       !entities_->entityAuthority_.canRetire(parent)||!world.nativePortalTailCurrent148D00(&parent,&portal))
        return fail(d,Status::identity,0x148d00);
    auto& r=*raZones_;const auto& labels=r.canonical->world()->portals[ordinal].zones;
    if(labels[0]!=zoneA+1u||labels[1]!=zoneB+1u||r.portalLinksPublished<ordinal*2u||
       (r.portalParent&&r.portalParent!=&parent)||(r.portals[ordinal]&&r.portals[ordinal]!=&portal))
        return fail(d,Status::identity,0x148d00);
    if(r.portalLinksPublished>=ordinal*2u+2u){
        if(r.portals[ordinal]!=&portal)return fail(d,Status::identity,0x148d00);
        d={Status::complete,0x148f20};return true;
    }
    // Genuine admitted legacy invocation begins here. It bypasses original
    //148D18 guard reads; invalidate shared history before its first effects.
    source_portal_guards50LA::invalidateNative();
    r.portals[ordinal]=&portal;r.portalParent=&parent;
    //Native bounds/non-null admission, no fabricated debug-global writes.
    try{
        while(r.portalLinksPublished<ordinal*2u+2u){const auto i=r.portalLinksPublished;
            const auto from=i%2u==0u?zoneA:zoneB,to=i%2u==0u?zoneB:zoneA;
            d.sourcePC=i%2u==0u?0x148ee0:0x148f04;
            auto link=std::make_unique<RaPortalLink27F3B0>();
            link->next00=static_cast<RaPortalLink27F3B0*>(const_cast<void*>(r.zones[from]->field30));
            link->neighbor04=&r.zones[to]->entity;link->portal08=&portal;
            r.portalLinks[i]=std::move(link);r.zones[from]->field30=r.portalLinks[i].get();
            ++r.portalLinksPublished;
        }
    }catch(...){d.status=Status::allocation;return false;}
    d={Status::complete,0x148f20};return true;
}
}

namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::incrementRaAmbient149300(
    const ::ra_cold_aggregate50LA::Owner& world,const C& light,Diagnostic& d)noexcept {
    d={};
    if(!raZonesReleasePreflight()||raZones_->world!=&world||raZones_->firstAmbient||count94_||
       !world.nativeAmbientLightCurrent(&light)||!world.nativeAmbientLight_->colorsWritten||
       world.nativeAmbientLight_->revisionWritten||field84_==std::numeric_limits<std::uint32_t>::max())
        return fail(d,Status::phase,0x149300);
    ++field84_;d.sourcePC=0x149310;return true;
}
bool OriginalUiLightManagerOwner50KG::appendRaAmbient148F30(
    const ::ra_cold_aggregate50LA::Owner& world,const C& light,Diagnostic& d)noexcept {
    d={};
    if(!raZonesReleasePreflight()||raZones_->world!=&world||!registryCurrent()||raZones_->firstAmbient||count94_||
       !zonesComplete_||count90_!=10u||(!capacity98_||capacity98_>190u||capacity98_%10u)||raRegistryGrowthFailed_||!registry9C_||
       !world.nativeAmbientLightCurrent(&light)||!world.nativeAmbientLight_->configured||
       world.nativeAmbientLight_->registered||light.parent1C)return fail(d,Status::phase,0x148f30);
    //First actual Ambient: source half RGB50, alpha overwritten to64 in manager.
    //149000 zone0 propagates both real arrays. Canonical rendering rows untouched.
    const Words ambient{{0x42480000u,0x42480000u,0x42480000u,0x42800000u}};
    default50_=default70_=ambient;
    for(std::size_t i=0;i<10u;++i){ambient60_[i]=ambient;ambient80_[i]=ambient;}
    ambientKnown_=true;
    //Actual retained UI registry has capacity10,count0. Reuse its allocation;
    //overwrite only reached slot0, preserve dead history slots1..3 and capacity.
    registry9C_[0]=&light;registryPublished_[0]=&light;++count94_;
    if(registryWritten_<1u)registryWritten_=1u;
    raZones_->firstAmbient=&light;d.sourcePC=0x1491cc;return true;
}
bool OriginalUiLightManagerOwner50KG::removeRaAmbient1491F0(
    const ::ra_cold_aggregate50LA::Owner& world,const C& light,Diagnostic& d)noexcept {
    d={};
    if(!raZonesScopeCurrent()||raZones_->world!=&world||!world.nativeAmbientLightCurrent(&light)||
       !world.nativeAmbientLight_->retiring||light.reference00||
       (raZones_->firstAmbient&&raZones_->firstAmbient!=&light)||
       count94_!=(raZones_->firstAmbient?1u:0u)||
       (count94_&&(!registry9C_||registry9C_[0]!=&light)))return fail(d,Status::phase,0x1491f0);
    //146980 invokes removal even for a never-registered partial constructor.
    //149230 decrements without clearing/compacting9C;149290 clearsA0.
    if(raZones_->firstAmbient)--count94_;
    raZones_->firstAmbient=nullptr;fieldA0_=0;d.sourcePC=0x1492e4;return true;
}
}

namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::incrementRaDirectional149300(const ::ra_cold_aggregate50LA::Owner& world,
    const C& light,std::uint32_t site,Diagnostic& d)noexcept {
    d={};
    if(!raZonesReleasePreflight()||raZones_->world!=&world||!registryCurrent()||!raZones_->firstAmbient||
       raZones_->firstDirectional||count94_!=1u||!world.nativeDirectionalLightCurrent(&light)||
       field84_==std::numeric_limits<std::uint32_t>::max())return fail(d,Status::phase,site);
    const auto& l=*world.nativeDirectionalLight_;
    if(!((site==0x146e30u&&l.colorsWritten&&!l.revisionWritten)||
         (site==0x147184u&&l.positionWritten&&!l.positionRevision)||
         (site==0x147230u&&l.lookWritten&&!l.lookRevision)||
         (site==0x147354u&&l.rangeWritten&&!l.rangeRevision)))return fail(d,Status::phase,site);
    ++field84_;d.sourcePC=0x149310;return true;
}
bool OriginalUiLightManagerOwner50KG::appendRaDirectional148F30(const ::ra_cold_aggregate50LA::Owner& world,
    const C& light,Diagnostic& d)noexcept {
    d={};
    if(!raZonesReleasePreflight()||raZones_->world!=&world||!registryCurrent()||!raZones_->firstAmbient||
       raZones_->firstDirectional||count94_!=1u||(!capacity98_||capacity98_>190u||capacity98_%10u)||raRegistryGrowthFailed_||!registry9C_||
       !world.nativeDirectionalLightCurrent(&light)||!world.nativeDirectionalLight_->configured||
       world.nativeDirectionalLight_->registered||light.parent1C)return fail(d,Status::phase,0x148f30);
    //1490A4..BC RGB source103/103/92.5, W0;1490D0 actual directional owner.
    vector30_={{0x42ce0000u,0x42ce0000u,0x42b90000u,0}};field40_=&light;
    for(std::size_t i=0;i<10u;++i)directional44_[i]=vector30_;
    directional44Known_=true;directional40Live_=true;
    registry9C_[1]=&light;registryPublished_[1]=&light;++count94_;
    if(registryWritten_<2u)registryWritten_=2u;
    raZones_->firstDirectional=&light;d.sourcePC=0x1491cc;return true;
}
bool OriginalUiLightManagerOwner50KG::removeRaDirectional1491F0(const ::ra_cold_aggregate50LA::Owner& world,
    const C& light,Diagnostic& d)noexcept {
    d={};
    if(!raZonesScopeCurrent()||raZones_->world!=&world||!raZones_->firstAmbient||
       !world.nativeDirectionalLightCurrent(&light)||!world.nativeDirectionalLight_->retiring||light.reference00||
       (raZones_->firstDirectional&&raZones_->firstDirectional!=&light)||
       count94_!=(raZones_->firstDirectional?2u:1u)||!registry9C_||registry9C_[0]!=raZones_->firstAmbient||
       (raZones_->firstDirectional&&registry9C_[1]!=&light))return fail(d,Status::phase,0x1491f0);
    if(raZones_->firstDirectional)--count94_;
    raZones_->firstDirectional=nullptr;if(field40_==&light)directional40Live_=false;
    fieldA0_=0; //Remaining actual row0 is Ambient; no directional left.
    //Source removal does NOT restore field40/vector30/44 or compact9C.
    d.sourcePC=0x1492e4;return true;
}
}

namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::incrementRaLocal149300(const ::ra_cold_aggregate50LA::Owner& world,
    const C& light,std::uint32_t site,Diagnostic& d)noexcept {
    const auto* actual=world.nativeLocalLightFor(&light);
    const auto zoneIndex=world.nativeLocalZoneIndex(&light);
    const auto authored=world.nativeLocalLightAuthoredIndex(&light);
    if(!actual||!authored||*authored<2u||*authored>184u||!zoneIndex||*zoneIndex>=10u||
       (actual->type34!=1u&&actual->type34!=3u)||actual->ordinalC8!=static_cast<std::int32_t>(*authored))return fail(d,Status::identity,0x148f30);
    const auto index=actual->ordinalC8-2u;
    d={};if(!raZonesReleasePreflight()||raZones_->world!=&world||!registryCurrent()||!raZones_->firstDirectional||
       raZones_->localPrimary[index]||count94_!=actual->ordinalC8||!world.nativeLocalLightCurrent(&light)||
       field84_==std::numeric_limits<std::uint32_t>::max())return fail(d,Status::phase,site);
    const auto& l=*world.nativeLocalLightFor(&light);
    if(!((site==0x146e30u&&l.colorsWritten&&!l.revisionWritten)||
         (site==0x147184u&&l.positionWritten&&!l.positionRevision)||
         (site==0x147230u&&l.type34==3u&&l.lookWritten&&!l.lookRevision)||
         (site==0x147354u&&l.rangeWritten&&!l.rangeRevision)))return fail(d,Status::phase,site);
    ++field84_;d.sourcePC=0x149310;return true;
}
bool OriginalUiLightManagerOwner50KG::bindRaLocalPrimary2809C0(const ::ra_cold_aggregate50LA::Owner& world,
    const C& light,Diagnostic& d)noexcept {
    const auto* actual=world.nativeLocalLightFor(&light);
    const auto zoneIndex=world.nativeLocalZoneIndex(&light);
    const auto authored=world.nativeLocalLightAuthoredIndex(&light);
    if(!actual||!authored||*authored<2u||*authored>184u||!zoneIndex||*zoneIndex>=10u||
       (actual->type34!=1u&&actual->type34!=3u)||actual->ordinalC8!=static_cast<std::int32_t>(*authored))return fail(d,Status::identity,0x148f30);
    const auto index=actual->ordinalC8-2u;
    d={};if(!raZonesReleasePreflight()||raZones_->world!=&world||!registryCurrent()||count94_!=actual->ordinalC8||
       !raZones_->firstDirectional||!world.nativeLocalLightCurrent(&light)||
       !world.nativeLocalLightFor(&light)->configured)return fail(d,Status::phase,0x149080);
    game::OriginalGarageObserverShared50KG::invalidateNativeLightAssertions();
    auto& l=*world.nativeLocalLightFor(&light);auto& zone=*raZones_->zones[*zoneIndex];auto* list=zone.first34;
    if(!list||list!=zone.second38||list!=raZones_->lists[*zoneIndex].get()||
       !list->lightHistoryCurrent(this,issuer_,entities_))return fail(d,Status::phase,0x27f590);
    if(l.observerOwner&&l.observerOwner->primaryRegistered){
        if(raZones_->localPrimary[index]!=&light||!list->lightMemberAt(&light,l.observerOwner->slot14))return fail(d,Status::identity,0x280b90);
        d.sourcePC=0x280bb8;return true;
    }
    if(raZones_->localPrimary[index])return fail(d,Status::phase,0x27e3b0);
    try {
        if(!l.observerOwner)l.observerOwner=std::make_unique<::ra_pod_scene50LA::Owner::NativeLightObserver280890>();
        auto& observer=*l.observerOwner;
        if(!observer.sharedConstructed){
            if(!entities_->garageObserverShared_.construct280890(&observer))return fail(d,Status::allocation,0x147540);
            observer.sharedConstructed=true;
        }
        l.observerD0=&observer;l.fieldD4=&zone.entity; //147548/554 same actual node.
        L::D ld;std::uint32_t slot=0;
        if(!list->insertRaFirstLight(this,issuer_,entities_,light,4u*(l.ordinalC8+1u),slot,ld))return fail(d,Status::phase,ld.sourcePC);
        //280B90..BBC actual primary publication AFTER slot20/24 service.
        observer.child04=&zone.entity;observer.method08=list;observer.slot14=slot;
        observer.next00=nullptr;observer.center20=l.positionA0;observer.primaryRegistered=true;
        raZones_->localPrimary[index]=&light;d.sourcePC=0x280bb8;return true;
    }catch(...){return fail(d,Status::allocation,0x147530);}
}
bool OriginalUiLightManagerOwner50KG::removeRaLocalPrimary1491F0(const ::ra_cold_aggregate50LA::Owner& world,
    const C& light,Diagnostic& d)noexcept {
    const auto* actual=world.nativeLocalLightFor(&light);
    const auto authored=world.nativeLocalLightAuthoredIndex(&light);
    const auto zoneIndex=world.nativeLocalZoneIndex(&light);
    //A counted failed-before146950 owner has NOT received type34/ordinalC8.
    //Use actual native custody for cleanup, never fabricate those source stores.
    if(!actual||!authored||*authored<2u||*authored>184u||!zoneIndex||*zoneIndex>=10u||
       (actual->colorsWritten&&((actual->type34!=1u&&actual->type34!=3u&&actual->type34!=4u)||
         actual->ordinalC8!=static_cast<std::int32_t>(*authored)))||
       (actual->type34==4u&&(*authored<172u||*authored>174u||actual->observerOwner||actual->observerD0)))
        return fail(d,Status::identity,0x1491f0);
    const auto index=*authored-2u;
    d={};if(!raZonesScopeCurrent()||raZones_->world!=&world||!world.nativeLocalLightCurrent(&light)||
       !world.nativeLocalLightFor(&light)->retiring||light.reference00||count94_!=(*authored+std::uint32_t(bool(raZones_->localRegistered[index])))||
       !raZones_->firstAmbient||!raZones_->firstDirectional||!registry9C_||
       registry9C_[0]!=raZones_->firstAmbient||registry9C_[1]!=raZones_->firstDirectional||
       (raZones_->localRegistered[index]&&(raZones_->localRegistered[index]!=&light||registry9C_[*authored]!=&light)))
        return fail(d,Status::phase,0x1491f0);
    auto& l=*world.nativeLocalLightFor(&light);
    //149214..230 removes the matching final registry row BEFORE observer cleanup.
    if(raZones_->localRegistered[index])--count94_;
    raZones_->localRegistered[index]=nullptr;
    //Real type1/D0 removal invokes281490 only after method08 exists.
    if(l.observerOwner){auto& observer=*l.observerOwner;
        if(observer.primaryRegistered){
            if(raZones_->localPrimary[index]!=&light||!observer.method08)return fail(d,Status::phase,0x281514);
            L::D ld;if(!observer.method08->removeRaFirstLight(this,issuer_,entities_,light,observer.slot14,ld))
                return fail(d,Status::phase,ld.sourcePC);
            //281534..568: source head order, remove each exact same List slot
            //before returning its actual shared-pool record through105830.
            while(observer.next00){auto* link=observer.next00;
                if(!entities_->garageObserverShared_.ownsLink(&observer,link)||!link->method08||
                   !link->method08->removeRaFirstLight(this,issuer_,entities_,light,link->slot14,ld))
                    return fail(d,Status::phase,0x281544);
                auto* next=link->next00;
                if(!entities_->garageObserverShared_.release105830(&observer,link))return fail(d,Status::phase,0x105830);
                observer.next00=next;
            }
            if(observer.pending){const auto& pending=*observer.pending;
                //Native abort custody after neighbor insertion but before105720
                //publication; retail280CD0 has no null-allocation recovery.
                if(!pending.method->removeRaFirstLight(this,issuer_,entities_,light,pending.slot,ld))
                    return fail(d,Status::phase,0x280cc8);
                observer.pending.reset();
            }
        }else if(raZones_->localPrimary[index])return fail(d,Status::phase,0x281490);
        //Native failed-before-primary abort is deliberately NOT a claim that
        //retail281490 could dereference null08 safely. No membership to remove.
        if(observer.sharedConstructed&&!entities_->garageObserverShared_.release280920(&observer))
            return fail(d,Status::phase,0x280920);
        l.observerOwner.reset();l.observerD0=nullptr; //Actual allocation, then14928C.
    }
    raZones_->localPrimary[index]=nullptr;fieldA0_=1; //Remaining actual row1 is Directional1492C8.
    d.sourcePC=0x1492e4;return true;
}
}

namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::localMembershipsCurrent()const noexcept {
    if(!raZones_||!raZones_->world)return false;const auto& r=*raZones_;
    std::array<std::array<bool,1024>,10> occupied{};
    for(std::size_t ordinal=0;ordinal<183u;++ordinal){const auto* light=r.localPrimary[ordinal];if(!light)continue;
        const auto* l=r.world->nativeLocalLightFor(light);const auto zoneIndex=r.world->nativeLocalZoneIndex(light);
        if(!l||!zoneIndex||*zoneIndex>=10u||!r.world->nativeLocalLightCurrent(light)||!l->observerOwner||l->ordinalC8!=ordinal+2u)return false;
        const auto& o=*l->observerOwner;const auto z=*zoneIndex;
        auto mark=[&](const C* child,const L* method,std::uint32_t slot){
            std::size_t n=10;for(std::size_t i=0;i<10;++i)if(r.zones[i]&&child==&r.zones[i]->entity)n=i;
            if(n==10||method!=r.lists[n].get()||!slot||slot>1024u||occupied[n][slot-1]||!method->lightMemberAt(light,slot)||
               method->lightTokens_[slot-1]!=4u*(l->ordinalC8+1u))return false;
            occupied[n][slot-1]=true;return true;
        };
        if(!o.primaryRegistered||!r.zones[z]||!mark(o.child04,o.method08,o.slot14)||o.child04!=&r.zones[z]->entity)return false;
        auto admit=[&](const C* child,const L* method,const C* portal,std::uint32_t slot,std::uint32_t distance){
            if(distance>0x7f7fffffu||!mark(child,method,slot))return false;
            // Edge cache is shared and later lights overwrite distance. Each
            // observer retains its own actual overlap result; don't equate it
            // to the most recent other owner's edge distance.
            for(auto* q=static_cast<const RaPortalLink27F3B0*>(r.zones[z]->field30);q;q=q->next00)
                if(q->neighbor04==child&&q->portal08==portal&&q->distanceWritten)return true;
            return false;
        };
        std::size_t visited=0;for(auto* link=o.next00;link;link=link->next00)
            if(++visited>20u||!entities_->garageObserverShared_.ownsLink(&o,link)||
               !admit(link->child04,link->method08,link->portal0C,link->slot14,link->distance10))return false;
        if(o.pending&&!admit(o.pending->child,o.pending->method,o.pending->portal,o.pending->slot,o.pending->distance))return false;
        if(o.portalLoopStarted){bool method=false;for(const auto& list:r.lists)if(list.get()==o.cursorMethod)method=true;if(!method)return false;
            if(o.portalCursor){bool cursor=false;for(auto* q=static_cast<const RaPortalLink27F3B0*>(r.zones[z]->field30);q;q=q->next00)if(q==o.portalCursor)cursor=true;if(!cursor)return false;}}
    }
    for(std::size_t z=0;z<10u;++z)if(r.lists[z])for(std::size_t slot=0;slot<1024u;++slot)
        if(bool(r.lists[z]->lightMembers_[slot])!=occupied[z][slot])return false;
    return true;
}
bool OriginalUiLightManagerOwner50KG::continueRaLocalPortals280BF0(
    const ::ra_cold_aggregate50LA::Owner& world,const C& light,Diagnostic& d)noexcept {
    const auto* actual=world.nativeLocalLightFor(&light);
    const auto zoneIndex=world.nativeLocalZoneIndex(&light);
    const auto authored=world.nativeLocalLightAuthoredIndex(&light);
    if(!actual||!authored||*authored<2u||*authored>184u||!zoneIndex||*zoneIndex>=10u||
       (actual->type34!=1u&&actual->type34!=3u)||actual->ordinalC8!=static_cast<std::int32_t>(*authored))return fail(d,Status::identity,0x148f30);
    const auto index=actual->ordinalC8-2u;
    d={};if(!raZonesReleasePreflight()||raZones_->world!=&world||!registryCurrent()||count94_!=actual->ordinalC8||
       raZones_->localPrimary[index]!=&light||!world.nativeLocalLightCurrent(&light)||
       !world.nativeLocalLightFor(&light)->observerOwner)return fail(d,Status::phase,0x280bf0);
    auto& r=*raZones_;auto& l=*world.nativeLocalLightFor(&light);auto& o=*l.observerOwner;
    if(!o.portalLoopStarted){o.portalCursor=r.zones[*zoneIndex]->field30;o.cursorMethod=o.method08;o.portalLoopStarted=true;}
    if(o.portalLoopComplete){d.sourcePC=0x14757c;return true;}
    auto& pool=entities_->garageObserverShared_;
    while(o.portalCursor){
        RaPortalLink27F3B0* edge=nullptr;std::size_t edgeIndex=0;
        for(std::size_t i=0;i<r.portalLinks.size();++i)if(r.portalLinks[i].get()==o.portalCursor){edge=r.portalLinks[i].get();edgeIndex=i;break;}
        if(!edge||!world.nativePortals_[edgeIndex/2u]||!world.nativePortals_[edgeIndex/2u]->geometry)
            return fail(d,Status::identity,0x280c40);
        if(!o.pending){
            game::Vector3_48C center{};for(std::size_t i=0;i<3;++i)std::memcpy(&center.v[i],&l.positionA0[i],4);
            float radius;std::memcpy(&radius,&l.range84,4);float distance=0;bool overlaps=false;
            if(game::overlapPortal231150_50EO(*world.nativePortals_[edgeIndex/2u]->geometry,
                center,radius,overlaps,distance)!=game::PortalGeometryStatus50EO::Complete)
                return fail(d,Status::phase,0x231150);
            if(!overlaps){o.portalCursor=edge->next00;continue;}
            std::uint32_t bits;std::memcpy(&bits,&distance,4);
            edge->field10=bits;edge->field0C=bits;edge->distanceWritten=true; //280C60/64.
            Zone* neighbor=nullptr;for(const auto& z:r.zones)if(z && &z->entity==edge->neighbor04)neighbor=z.get();
            if(!neighbor||!o.cursorMethod)return fail(d,Status::identity,0x27f590);
            //Source s2 changes after each overlap: consume THAT method's10.
            const auto usage=o.cursorMethod->usage10_;
            L* method=(usage&1u)?neighbor->first34:(usage&2u)?neighbor->second38:nullptr;
            if(!method)return fail(d,Status::phase,0x27f590);
            o.cursorMethod=method;
            L::D ld;std::uint32_t slot=0;
            if(!method->insertRaFirstLight(this,issuer_,entities_,light,4u*(l.ordinalC8+1u),slot,ld))return fail(d,Status::phase,ld.sourcePC);
            //Native custody of a REAL reached membership across105720 failure.
            //No fabricated shared record or source null-allocation return.
            o.pending=::ra_pod_scene50LA::Owner::NativeLightObserver280890::PendingMembership{
                edge->neighbor04,method,edge->portal08,slot,bits};
        }
        auto* link=pool.allocate105720(&o);if(!link)return fail(d,Status::allocation,0x280cc8);
        const auto pending=*o.pending;
        link->slot14=pending.slot;link->child04=pending.child;link->method08=pending.method;
        link->portal0C=pending.portal;link->distance10=pending.distance;
        link->next00=o.next00;o.next00=link; //280CD0..CF4, actual shared record head insertion.
        o.pending.reset();o.portalCursor=edge->next00;
    }
    o.portalLoopComplete=true;d.sourcePC=0x14757c;return true;
}
bool OriginalUiLightManagerOwner50KG::appendRaLocal148F30(
    const ::ra_cold_aggregate50LA::Owner& world,const C& light,Diagnostic& d)noexcept {
    const auto* actual=world.nativeLocalLightFor(&light);
    const auto zoneIndex=world.nativeLocalZoneIndex(&light);
    const auto authored=world.nativeLocalLightAuthoredIndex(&light);
    if(!actual||!authored||*authored<2u||*authored>184u||!zoneIndex||*zoneIndex>=10u||
       (actual->type34!=1u&&actual->type34!=3u)||actual->ordinalC8!=static_cast<std::int32_t>(*authored))return fail(d,Status::identity,0x148f30);
    const auto index=actual->ordinalC8-2u;
    d={};if(!raZonesReleasePreflight()||raZones_->world!=&world||!registryCurrent()||count94_!=actual->ordinalC8||
       raZones_->localPrimary[index]!=&light||raZones_->localRegistered[index]||!registry9C_||raRegistryGrowthFailed_||
       !world.nativeLocalLightCurrent(&light)||!world.nativeLocalLightFor(&light)->observerOwner||
       !world.nativeLocalLightFor(&light)->observerOwner->portalLoopComplete||world.nativeLocalLightFor(&light)->registered)
        return fail(d,Status::phase,0x149088);
    //1490A4 and149178 are type1 only here; actual Spot3 leaves them intact.
    if(actual->type34==1u){for(std::size_t i=0;i<3u;++i)std::memcpy(&vector30_[i],&actual->diffuse3C[i],4);
        vector30_[3]=0;fieldA0_=1;}
    if(!growRaRegistry149194(d))return false;
    registry9C_[count94_]=&light;registryPublished_[count94_]=&light;++count94_;
    if(registryWritten_<count94_)registryWritten_=count94_;
    raZones_->localRegistered[index]=&light;d.sourcePC=0x1491cc;return true;
}
}

namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::growRaRegistry149194(Diagnostic& d)noexcept {
    if(raRegistryGrowthFailed_||!raZones_||!registry9C_||!capacity98_||capacity98_>190u||capacity98_%10u||count94_>=185u)
        return fail(d,Status::phase,0x149194);
    if(capacity98_>=count94_+1u)return true;
    if(capacity98_!=count94_)return fail(d,Status::phase,0x149194);
    capacity98_=count94_+10u; //149198 BEFORE actual realloc service.
    try {auto grown=std::make_unique<const C*[]>(capacity98_);
        for(std::size_t i=0;i<registryWritten_;++i)grown[i]=registry9C_[i];
        registry9C_=std::move(grown);
    }catch(...){raRegistryGrowthFailed_=true;return fail(d,Status::allocation,0x1491a4);}
    // Explicit native terminal failure custody retains old allocation; never
    // claims source failed realloc/null9C survived, never retries its service.
    return true;
}
bool OriginalUiLightManagerOwner50KG::incrementRaLaterAmbient149300(
    const ::ra_cold_aggregate50LA::Owner& world,const C& light,Diagnostic& d)noexcept {
    d={};const auto* l=world.nativeLocalLightFor(&light);const auto authored=world.nativeLocalLightAuthoredIndex(&light);
    if(!l||!authored||*authored<172u||*authored>174u||l->type34!=4u||l->ordinalC8!=static_cast<std::int32_t>(*authored)||
       !raZonesReleasePreflight()||raZones_->world!=&world||!registryCurrent()||raRegistryGrowthFailed_||count94_!=*authored||
       !world.nativeLocalLightCurrent(&light)||!l->colorsWritten||l->revisionWritten||
       field84_==std::numeric_limits<std::uint32_t>::max())return fail(d,Status::phase,0x149300);
    ++field84_;d.sourcePC=0x149310;return true;
}
bool OriginalUiLightManagerOwner50KG::appendRaLaterAmbient148F30(
    const ::ra_cold_aggregate50LA::Owner& world,const C& light,Diagnostic& d)noexcept {
    d={};const auto* l=world.nativeLocalLightFor(&light);const auto authored=world.nativeLocalLightAuthoredIndex(&light);
    const auto z=world.nativeLocalZoneIndex(&light);
    if(!l||!authored||*authored<172u||*authored>174u||!z||*z!=(*authored==172u?0u:*authored==173u?1u:7u)||
       l->type34!=4u||l->ordinalC8!=static_cast<std::int32_t>(*authored)||!world.nativeLocalLightCurrent(&light)||
       !l->configured||l->registered||l->observerOwner||l->observerD0||light.parent1C||
       !raZonesReleasePreflight()||raZones_->world!=&world||!registryCurrent()||raRegistryGrowthFailed_||count94_!=*authored||
       raZones_->localRegistered[*authored-2u]||raZones_->localPrimary[*authored-2u]||
       !ambient60_||!ambient80_||!zonesComplete_||count90_!=10u)return fail(d,Status::phase,0x148f30);
    Words ambient{};for(std::size_t i=0;i<3;++i)std::memcpy(&ambient[i],&l->ambient5C[i],4);
    ambient[3]=0x42800000u; //148F74/FE4..FFC actual64, not source Light alpha127.5.
    ambient60_[*z]=ambient80_[*z]=ambient;default50_=default70_=ambient;
    if(*z==0u)for(std::size_t i=0;i<10u;++i)ambient60_[i]=ambient80_[i]=ambient; //149000 loop.
    ambientKnown_=true;
    // Source type4 bypasses observer, vector30, directional40/44 andA0 stores.
    if(!growRaRegistry149194(d))return false;
    registry9C_[count94_]=&light;registryPublished_[count94_]=&light;++count94_;
    if(registryWritten_<count94_)registryWritten_=count94_;
    raZones_->localRegistered[*authored-2u]=&light;d.sourcePC=0x1491cc;return true;
}
}

namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::applyRaDirectionalColor1498A0(
    const ::ra_cold_aggregate50LA::Owner& world,std::size_t zone,Diagnostic& d)noexcept {
    d={};
    if((zone!=0u&&zone!=1u&&zone!=7u)||!raZonesReleasePreflight()||raZones_->world!=&world||
       !registryCurrent()||count94_!=185u||raZones_->localRegisteredCount()!=183u||raRegistryGrowthFailed_||
       !directional40Live_||!directional44Known_||!directional44_||count90_!=10u||
       world.nativePointNext_!=15u||world.nativeRemainderNext_!=168u||
       !world.nativeLightList3A8_||!world.canRetireNativeLightWrappers())return fail(d,Status::phase,0x273bf8);
    const auto& list=*world.nativeLightList3A8_;
    if(!list.counted||list.loaded!=185u||list.count30!=185u||!list.zoneCount34||*list.zoneCount34!=10u||
       list.colorsLoaded!=10u||!list.colors3C||!list.wrappers38)return fail(d,Status::identity,0x273bd4);
    for(std::size_t i=0;i<185u;++i)
        if(list.wrappers38[i].live00!=registry9C_[i]||!list.wrappers38[i].descriptor04)
            return fail(d,Status::identity,0x273bd4);
    // Only the three authenticated authored DirectionalLightColorForZone rows
    // reach1498A0. Other seven source sentinel X lanes are skipped by273BE8.
    const auto& lanes=list.colors3C[zone].lanes;
    for(std::size_t i=0;i<3u;++i)if(!lanes[i]||*lanes[i]!=20.0f)
        return fail(d,Status::identity,0x273bdc);
    // Source1498D0/E0/F0 writes RGB ONLY. Preserve the actual prior W payload;
    // no initialization, inferred W store, revision bump or canonical replay.
    for(std::size_t i=0;i<3u;++i)std::memcpy(&directional44_[zone][i],&*lanes[i],4);
    d.sourcePC=0x1498f4;return true;
}
}

namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::beginCourseZone148B60(
    const ::original_main_entry50KG::Application& issuer,
    const OriginalUiCourseOwnedLifecycle50KG& owner,Diagnostic& d)noexcept {
    d={};
    if(issuer_!=&issuer||owner.issuer_!=&issuer||!owner.lightBoundaryCurrent148B60()||
       owner.courseLight_.get()!=this||!sourceMutationAvailable()||
       !nativeProjectionInputsEmpty()||!zoneReleasePreflight())
        return fail(d,Status::identity,0x1CCD88);
    // Native global count/table capacity admission precedes the source interval;
    // this is NOT a substitute success for any source heap allocation below.
    if(entities_->objectCount423F60_==std::numeric_limits<std::uint32_t>::max()||
       !entities_->entityAuthority_.reserveAdditional(1))return fail(d,Status::allocation,0x148B60);
    constexpr Words defaults{{0x42480000u,0x42480000u,0x42480000u,0x42800000u}};
    default70_=defaults;default50_=defaults;defaultsKnown_=true; //148BA0/A8.
    if(!releaseZones148110(d))std::terminate(); // Authenticated actually-empty source call.
    courseZoneOwner_=&owner;
    zonesAttempted_=true;count90_=1; //148BB8 before allocation return88 publication.
    // NULL is a published source allocation result, not an early C++ exception.
    // All five allocation calls and stores precede the first slot dereference.
    nodes88_.reset(new(std::nothrow) Zone*[1]); //148BB4/BC.
    lists8C_.reset(new(std::nothrow) L*[1]); //148BC0/CC.
    ambient60_.reset(new(std::nothrow) Words[1]); //148BD0/D8.
    ambient80_.reset(new(std::nothrow) Words[1]); //148BDC/E4.
    directional44_.reset(new(std::nothrow) Words[1]); //148BE8/F4; contents UNKNOWN.
    zone_.reset(new(std::nothrow) Zone); //148C08; NULL skips27F120.
    if(zone_){
        auto& z=*zone_;
        if(!entities_->entityAuthority_.canEnroll(z.entity,key(&z.entity)))std::terminate();
        const auto init=game::initializeOriginalWreckComponent50LA(z.entity,K::searchNode27F120,
            generation_,key(&z.entity),true,entities_->objectCount423F60_);
        if(init.status!=game::OriginalWreckComponentStatus50LA::complete||
           !entities_->entityAuthority_.enroll(z.entity))std::terminate();
        z.constructed=true;
    }
    // The node slot store is the delay slot of the next allocator call.
    // If88 is NULL, the list allocator has not run yet.
    if(!nodes88_)return fail(d,Status::allocation,0x148C38);
    nodes88_[0]=zone_.get();courseNodeSlotKnown_=true;
    if(zone_)zone_->published=true;
    list_=std::unique_ptr<L>(new(std::nothrow) L(*issuer_,entities_)); //148C34 after delay-slot store.
    if(list_){
        list_->lightManagerOwner_=this;L::D ld;
        if(!list_->construct27E260(issuer_,entities_,ld)&&
           !(ld.status==L::S::allocation&&ld.sourcePC==0x27EE38&&
             list_->lightEmptyCurrent(this,issuer_,entities_)&&list_->derivedConstructed_&&
             !list_->complete_&&!list_->selected1C_&&list_->soultreeKnown_&&list_->searchableKnown_))
            return fail(d,Status::list,ld.sourcePC);
        //27EA14 ignores selected-allocation NULL; actual constructor still
        // returns. Slot publication and alias setters do not read selected1C.
    }
    if(!lists8C_)return fail(d,Status::allocation,0x148C58);
    lists8C_[0]=list_.get();courseListSlotKnown_=true;listPublished_=bool(list_);
    if(!zone_)return fail(d,Status::allocation,0x27F434); // First receiver34 load.
    if(!list_)return fail(d,Status::allocation,0x27F454); // After actual old34=NULL branch.
    auto& z=*zone_;
    if(!list_->lightEmptyCurrent(this,issuer_,entities_)||!list_->derivedConstructed_||
       z.first34||z.second38||list_->lightAliases_)return fail(d,Status::phase,0x27F420);
    // Actual setters write List usage then Node alias and retain. Neither
    // dereferences selected1C, so its genuine NULL result is preserved.
    list_->usage10_=1;z.first34=list_.get();++list_->reference00_;++list_->lightAliases_;
    list_->usage10_=2;z.second38=list_.get();++list_->reference00_;++list_->lightAliases_;
    z.index48=0; //148CA4.
    if(!ambient80_)return fail(d,Status::allocation,0x148CBC);
    ambient80_[0]=defaults;courseAmbient80Known_=true; // Survives a later NULL60 stop.
    if(!ambient60_)return fail(d,Status::allocation,0x148CD0);
    ambient60_[0]=defaults;ambientKnown_=true;
    //44 may legally be NULL:148B60 does not dereference it in this interval.
    zonesComplete_=true;d={Status::complete,0x1CCD90};return true;
}
bool OriginalUiLightManagerOwner50KG::courseZoneReleasePreflight(
    const OriginalUiCourseOwnedLifecycle50KG& owner)const noexcept {
    if(courseZoneOwner_!=&owner||owner.issuer_!=issuer_||owner.courseLight_.get()!=this||!current()||
       nativeProjectionUi_||nativeProjectionGameplayGeneration_||raZones_||
       uiLightsOwner_||sceneLightsOwner_||count94_||directional40Live_||
       !zonesAttempted_||count90_!=1)return false;
    if(courseNodeSlotKnown_&&(!nodes88_||nodes88_[0]!=zone_.get()))return false;
    if(courseListSlotKnown_&&(!lists8C_||lists8C_[0]!=list_.get()))return false;
    if(listPublished_!=(courseListSlotKnown_&&bool(list_)))return false;
    if(zone_){
        const auto& z=*zone_;const auto& c=z.entity;
        if(!z.constructed||z.published!=courseNodeSlotKnown_||
           c.kind!=K::searchNode27F120||c.objectKey!=key(&c)||c.sessionGeneration!=generation_||
           c.reference00!=1||c.previous10||c.next14||c.firstChild18||c.parent1C||c.mask20||
           !entities_->entityAuthority_.canRetire(c)||z.field30||z.field3C||z.field40||z.field44||z.fieldA0||z.index48)
            return false;
        if(z.first34!=(list_&&list_->lightAliases_>=1?list_.get():nullptr)||
           z.second38!=(list_&&list_->lightAliases_>=2?list_.get():nullptr))return false;
    }else if(list_&&list_->lightAliases_)return false;
    return !list_||(list_->lightEmptyCurrent(this,issuer_,entities_)&&!list_->lightReleasedAliases_);
}
bool OriginalUiLightManagerOwner50KG::abortCourseZoneNative(
    const OriginalUiCourseOwnedLifecycle50KG& owner,Diagnostic& d)noexcept {
    d={};
    // A retained candidate may have rejected admission while another Course
    // owns the singleton. That never-acquired borrow has nothing to release.
    if(courseZoneOwner_!=&owner)return issuer_==owner.issuer_&&owner.courseLight_.get()==this;
    if(!courseZoneReleasePreflight(owner))return fail(d,Status::phase,0x148110);
    // Native partial unwind, NOT a certificate that Course manager teardown
    // has run. Retire actual constructed objects even if a NULL array stopped
    // before their source publication. Never read an unwritten array slot.
    if(zone_){auto& z=*zone_;auto& c=z.entity;
        if(!entities_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;L::D ld;
        if(z.first34&&!list_->releaseLightAlias(this,issuer_,entities_,1,z.first34,ld))std::terminate();
        if(z.second38&&!list_->releaseLightAlias(this,issuer_,entities_,2,z.second38,ld))std::terminate();
        --entities_->objectCount423F60_;z.constructed=false;zone_.reset();
    }
    nodes88_.reset();
    if(list_){L::D ld;if(!list_->releaseLightPayload(this,issuer_,entities_,ld))std::terminate();list_.reset();}
    lists8C_.reset();listPublished_=false;count90_=0;
    ambient60_.reset();ambient80_.reset();directional44_.reset();
    zonesAttempted_=false;zonesComplete_=false;ambientKnown_=false;directional44Known_=false;
    courseNodeSlotKnown_=false;courseListSlotKnown_=false;courseAmbient80Known_=false;courseZoneOwner_=nullptr;
    // Defaults50/70, registry94/98/9C, absent cacheA4 and singleton retained.
    d={Status::complete,0};return true; // Native unwind, no source148110 return receipt.
}
}

namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::incrementCourseRevision149300(
    OriginalUiCourseOwnedLifecycle50KG& owner,Diagnostic& d)noexcept {
    d={};
    if(courseZoneOwner_!=&owner||owner.issuer_!=issuer_||owner.courseLight_.get()!=this||
       !current()||!zonesComplete_||count90_!=1)return fail(d,Status::identity,0x149300);
    ++field84_; //149304/addiu and14930C store: real unsigned32 wrap.
    d={Status::complete,0x149310};return true;
}
bool OriginalUiLightManagerOwner50KG::appendCourseLight148F30(
    OriginalUiCourseOwnedLifecycle50KG& owner,C& entity,Diagnostic& d)noexcept {
    d={};
    if(courseZoneOwner_!=&owner||owner.issuer_!=issuer_||owner.courseLight_.get()!=this||
       !current()||!zonesComplete_||count90_!=1||registryFailed_||count94_>=2)
        return fail(d,Status::phase,0x148F30);
    const std::size_t ordinal=count94_;
    auto& p=owner.courseUiLights_[ordinal];
    if(!p||&p->entity!=&entity||!owner.courseLightEntityCurrent(entity,ordinal)||
       !p->constructed||!p->typed||!p->configured||p->registered||p->retired||
       entity.reference00!=1||entity.parent1C||entity.previous10||entity.next14||
       p->type34!=(ordinal?2u:4u)||p->observerD0||p->fieldD8)
        return fail(d,Status::identity,0x148F30);
    auto& l=*p;
    const auto word=[](float f)noexcept {std::uint32_t u;std::memcpy(&u,&f,4);return u;};
    if(l.type34==4){
        // Source148F50 skips Ambient copy if60 is NULL. A completed one-zone
        // Course setup proves both arrays exist, but retain the real branch.
        if(ambient60_){
            if(!ambient80_)return fail(d,Status::phase,0x148F7C);
            for(std::size_t i=0;i<3;++i){const auto value=word(l.ambient5C[i]);
                ambient80_[0][i]=value;ambient60_[0][i]=value;
                default70_[i]=value;default50_[i]=value;
            }
            ambient80_[0][3]=0x42800000u;ambient60_[0][3]=0x42800000u;
            default70_[3]=0x42800000u;default50_[3]=0x42800000u;
            ambient60_[0]=default50_;ambient80_[0]=default50_; //14900C..40 zone0 loop.
            ambientKnown_=true;courseAmbient80Known_=true;defaultsKnown_=true;
        }
    }else {
        vector30_={{word(l.diffuse3C[0]),word(l.diffuse3C[1]),word(l.diffuse3C[2]),0}};
        field40_=&entity; //1490D0; historical pointer survives later native release.
        if(directional44_){directional44_[0]=vector30_;directional44Known_=true;}
        // NULL44 is explicitly checked1490D8 and is not a failure or zero row.
    }
    if(capacity98_<count94_+1){
        // Only the first row can grow in this bounded two-light cohort. Any
        // retained existing registry has capacity>=10 and is reused in place.
        if(count94_||capacity98_||registry9C_)std::terminate();
        capacity98_=count94_+10; //149198 before realloc call.
        registry9C_.reset(new(std::nothrow) const C*[capacity98_]); //1491AC publishes even NULL.
        if(!registry9C_){registryFailed_=true;return fail(d,Status::allocation,0x1491C0);}
    }
    registry9C_[count94_]=&entity;registryPublished_[count94_]=&entity; //1491C0.
    ++count94_;if(registryWritten_<count94_)registryWritten_=count94_;
    l.registered=true;if(l.type34==2)directional40Live_=true;
    d={Status::complete,0x1491E8};return true;
}
bool OriginalUiLightManagerOwner50KG::removeCourseLight1491F0(
    OriginalUiCourseOwnedLifecycle50KG& owner,C& entity,Diagnostic& d)noexcept {
    d={};
    if(courseZoneOwner_!=&owner||owner.issuer_!=issuer_||owner.courseLight_.get()!=this||!current())
        return fail(d,Status::identity,0x1491F0);
    std::size_t ordinal=2;
    for(std::size_t i=0;i<2;++i)if(owner.courseUiLights_[i] && &owner.courseUiLights_[i]->entity==&entity)ordinal=i;
    if(ordinal==2||!owner.courseLightEntityCurrent(entity,ordinal))return fail(d,Status::identity,0x1491F0);
    auto& l=*owner.courseUiLights_[ordinal];
    if(!l.constructed||l.retired||l.observerD0||l.fieldD8||l.fieldC0||l.fieldC4||
       (l.type34!=0&&l.type34!=(ordinal?2u:4u)))return fail(d,Status::phase,0x1491F0);
    // Native teardown invokes the actual function with its still-counted row;
    //1491F0 does not inspect Entity reference00. No146980 completion implied.
    if(l.registered){
        if(count94_!=ordinal+1||!registry9C_||registry9C_[ordinal]!=&entity)
            return fail(d,Status::phase,0x1491F0);
    }else {
        if(count94_>ordinal)return fail(d,Status::phase,0x1491F0);
        for(std::size_t i=0;i<count94_;++i)if(registry9C_[i]==&entity)
            return fail(d,Status::identity,0x1491F0);
    }
    for(std::size_t i=0;i<count94_;++i)if(registry9C_[i]==&entity)--count94_;
    l.registered=false;if(field40_==&entity)directional40Live_=false;
    fieldA0_=0;
    for(std::size_t i=0;i<count94_;++i)if(owner.courseUiLights_[i]->type34==2){fieldA0_=1;break;}
    // Neither stale source9C slots nor40 are cleared by1491F0.
    d={Status::complete,0x1492F8};return true;
}
}

#include "original_ui_light_manager_source_world_zones50la.inl"
#include "original_ui_light_manager_source_ambient50la.inl"

#include "original_ui_light_manager_source_portals50la.inl"
