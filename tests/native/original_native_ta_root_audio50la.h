#pragma once
// Include after the genuine consumed TA race/world owner definitions.
#include "original_race_constructor50la.h"
#include <bit>
#include <cmath>
namespace original_main_entry50KG {
inline std::shared_ptr<const rr::assets::OriginalTaIntroAuthoringCandidate>
Application::retainNativeTaIntroAuthoring50LA()const noexcept {
    if(!sourceRace28COwner_||!sourceRace28COwner_->nativeTaRaceEntity()||
       !sourceRace28COwner_->nativeTaWorldBacking()||!sourceAural00Owner_||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())return {};
    auto metadata=sourceAural00Owner_->retainedStreamMetadata();
    auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    if(!metadata||!provider)return {};
    rr::assets::OriginalTaIntroAuthoringCandidate::Diagnostic diagnostic;
    auto authored=rr::assets::OriginalTaIntroAuthoringCandidate::create(provider,metadata,diagnostic);
    if(!authored)return {};
    try{return std::shared_ptr<const rr::assets::OriginalTaIntroAuthoringCandidate>(std::move(authored));}
    catch(...){return {};}
}
inline bool Application::beginNativeTaRaceRoot140CE0(rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::OriginalWreckComponent50LA& controller,
    const original_ui_source_timing50LH::PublicationView& timing,std::string& error)const noexcept {
    const auto fail=[&](const char* text){error=text;return false;};
    if(nativeTaRootEntered_||sourceRace28COwner_.get()!=&race||sourceUpdateRenderBypass228Bit3_||
       (sourceApplicationMode22C_!=0&&sourceApplicationMode22C_!=1)||!entitiesOwner_||
       entitiesOwner_->attachmentLock423F58()||!sourceAural00Owner_||!sourceAural00Owner_->current())
        return fail("TA Root140CE0 requires its actual idle Application/race/Aural authority");
    const auto* raceEntity=race.nativeTaRaceEntity();const auto* worldEntity=race.nativeTaWorldEntity();
    if(!raceEntity||!worldEntity||!race.nativeTaWorldBacking()||race.nativeTaControllerSlot48()!=&controller||
       controller.kind!=rr::game::OriginalWreckComponentKind50LA::cameraController174A30||controller.reference00!=1||
       controller.sessionGeneration!=raceEntity->sessionGeneration||
       !contains(entitiesOwner_->entityAuthority_.view(),raceEntity)||
       !contains(entitiesOwner_->entityAuthority_.view(),worldEntity)||
       !contains(entitiesOwner_->entityAuthority_.view(),&controller)||
       !rr::game::original_entity_chain_detail50LA::forest(entitiesOwner_->entityAuthority_.view()))
        return fail("TA Root140CE0 requires the real consumed race/world and retained controller48");
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!timing.readPrepared(sample,serial)||!std::isfinite(sample.updateSeconds)||sample.updateSeconds<0.f||
       sample.updateSeconds>std::bit_cast<float>(std::uint32_t{0x3d75c28f}))
        return fail("TA Root140CE4 requires a prepared capped source timing publication");
    const bool sameRace=nativeTaRootRace_==&race&&nativeTaRootGeneration_==raceEntity->sessionGeneration;
    if(sameRace&&nativeTaRootAborted_)return fail("Abandoned TA Root cannot resume source execution in the same race");
    if(sameRace&&nativeTaRootPublication_&&(!timing.sameOwner(nativeTaRootTiming_)||
       nativeTaRootPublication_==UINT64_MAX||serial!=nativeTaRootPublication_+1))
        return fail("TA Root source publication cannot repeat, skip or replace its retained timing owner");
    nativeTaRootTiming_=timing;nativeTaRootRace_=&race;nativeTaRootController_=&controller;
    nativeTaRootPublication_=serial;nativeTaRootGeneration_=raceEntity->sessionGeneration;
    nativeTaRootEntered_=true;nativeTaRootAborted_=false;sourceApplicationMode22C_=2;sourceApplicationElapsed23C_=sample.updateSeconds;
    error.clear();return true;
}
inline bool Application::finishNativeTaRaceRoot140CFC(rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::OriginalWreckComponent50LA& controller,
    const original_ui_source_timing50LH::PublicationView& timing,std::string& error)const noexcept {
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!nativeTaRootEntered_||nativeTaRootRace_!=&race||sourceRace28COwner_.get()!=&race||
       nativeTaRootController_!=&controller||race.nativeTaControllerSlot48()!=&controller||
       !race.nativeTaRaceEntity()||race.nativeTaRaceEntity()->sessionGeneration!=nativeTaRootGeneration_||
       !timing.sameOwner(nativeTaRootTiming_)||!timing.readPrepared(sample,serial)||serial!=nativeTaRootPublication_||
       sourceApplicationMode22C_!=2||!sourceApplicationElapsed23C_||
       std::bit_cast<std::uint32_t>(*sourceApplicationElapsed23C_)!=std::bit_cast<std::uint32_t>(sample.updateSeconds)||
       !sourceAural00Owner_||!sourceAural00Owner_->raceIntroPublicationServiced(timing,serial)){
        error="TA Root140CFC requires the same accepted publication and completed Aural267CB0 service";return false;
    }
    // Caller reaches this ONLY after its actual ordered camera/world/group
    // callbacks return. The numeric source publication is committed afterward.
    sourceApplicationMode22C_=0;nativeTaRootEntered_=false;error.clear();return true;
}
inline bool Application::abandonNativeTaRaceRootForTeardown50LA(rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::OriginalWreckComponent50LA& controller,
    const original_ui_source_timing50LH::PublicationView& timing,std::string& error)const noexcept {
    if(!nativeTaRootEntered_){error.clear();return true;}
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(nativeTaRootRace_!=&race||sourceRace28COwner_.get()!=&race||nativeTaRootController_!=&controller||
       race.nativeTaControllerSlot48()!=&controller||!timing.sameOwner(nativeTaRootTiming_)||
       !timing.readPrepared(sample,serial)||serial!=nativeTaRootPublication_||sourceApplicationMode22C_!=2){
        error="Native TA teardown cannot abandon a foreign or changed Root publication";return false;
    }
    // Explicit native terminal teardown, NOT140CFC success or rollback of
    // already performed callbacks. Keep23C and publication history. A new
    // consumed race may start; this same race can never resume source work.
    nativeTaRootEntered_=false;nativeTaRootAborted_=true;sourceApplicationMode22C_=0;
    error.clear();return true;
}

}
