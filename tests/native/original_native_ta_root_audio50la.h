#pragma once
// Include after the genuine consumed TA/RA typed race/world owner definitions.
// Legacy method names remain stable; actual consumed track/world identity owns admission.
#include "original_race_constructor50la.h"
#include <bit>
#include <cmath>
#include <thread>
namespace original_main_entry50KG {
inline std::shared_ptr<const rr::assets::OriginalTaIntroAuthoringCandidate>
Application::retainNativeTaIntroAuthoring50LA()const noexcept {
    if(!sourceRace28COwner_||!sourceRace28COwner_->nativeIntroRaceEntity()||
       !sourceRace28COwner_->nativeIntroWorldEntity()||!sourceAural00Owner_||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())return {};
    auto metadata=sourceAural00Owner_->retainedStreamMetadata();
    auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
    if(!metadata||!provider)return {};
    rr::assets::OriginalTaIntroAuthoringCandidate::Diagnostic diagnostic;
    auto authored=rr::assets::OriginalTaIntroAuthoringCandidate::create(provider,metadata,diagnostic);
    const auto* target=original_course_targets50KX::target(sourceRace28COwner_->launchSelection_);
    // Only tracks with a real typed consumed world reached the guard above.
    const char* expected=target?rr::platform::hostTrackCode50IM(target->track):nullptr;
    if(!authored||!expected||authored->trackId()!=expected)return {};
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
        return fail("Intro Root140CE0 requires its actual idle Application/race/Aural authority");
    const auto* raceEntity=race.nativeIntroRaceEntity();const auto* worldEntity=race.nativeIntroWorldEntity();
    if(!raceEntity||!worldEntity||!race.nativeIntroWorldEntity()||race.nativeIntroControllerSlot48()!=&controller||
       controller.kind!=rr::game::OriginalWreckComponentKind50LA::cameraController174A30||controller.reference00!=1||
       controller.sessionGeneration!=raceEntity->sessionGeneration||
       !contains(entitiesOwner_->entityAuthority_.view(),raceEntity)||
       !contains(entitiesOwner_->entityAuthority_.view(),worldEntity)||
       !contains(entitiesOwner_->entityAuthority_.view(),&controller)||
       !rr::game::original_entity_chain_detail50LA::forest(entitiesOwner_->entityAuthority_.view()))
        return fail("Intro Root140CE0 requires the real consumed race/world and retained controller48");
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!timing.readPrepared(sample,serial)||!std::isfinite(sample.updateSeconds)||sample.updateSeconds<0.f||
       sample.updateSeconds>std::bit_cast<float>(std::uint32_t{0x3d75c28f}))
        return fail("Intro Root140CE4 requires a prepared capped source timing publication");
    const bool sameRace=nativeTaRootRace_==&race&&nativeTaRootGeneration_==raceEntity->sessionGeneration;
    if(sameRace&&nativeTaRootAborted_)return fail("Abandoned Intro Root cannot resume source execution in the same race");
    if(sameRace&&nativeTaRootPublication_&&(!timing.sameOwner(nativeTaRootTiming_)||
       nativeTaRootPublication_==UINT64_MAX||serial!=nativeTaRootPublication_+1))
        return fail("Intro Root source publication cannot repeat, skip or replace its retained timing owner");
    nativeTaRootTiming_=timing;nativeTaRootRace_=&race;nativeTaRootController_=&controller;
    nativeTaRootPublication_=serial;nativeTaRootGeneration_=raceEntity->sessionGeneration;
    nativeTaRootEntered_=true;nativeTaRootAborted_=false;sourceApplicationMode22C_=2;sourceApplicationElapsed23C_=sample.updateSeconds;
    error.clear();return true;
}
inline bool Application::updateNativeTaRaceF8BeforeCamera50LA(rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::OriginalWreckComponent50LA& controller,
    const original_ui_source_timing50LH::PublicationView& timing,std::string& error)const noexcept {
    // Existing accepted-root pre-Camera cut, not a new root admission or the
    // full141ED0 dispatcher. Affinity precedes mutable Application/Race state.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()){
        error="F8 pre-Camera update requires the Application owner thread";return false;
    }
    const auto retained=sourceRace28COwner_;
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!nativeTaRootEntered_||nativeTaRootAborted_||nativeTaRootRace_!=&race||retained.get()!=&race||
       nativeTaRootController_!=&controller||race.nativeIntroControllerSlot48()!=&controller||
       !race.nativeIntroRaceEntity()||race.nativeIntroRaceEntity()->sessionGeneration!=nativeTaRootGeneration_||
       !timing.sameOwner(nativeTaRootTiming_)||!timing.readPrepared(sample,serial)||serial!=nativeTaRootPublication_||
       sourceUpdateRenderBypass228Bit3_||sourceApplicationMode22C_!=2||!sourceApplicationElapsed23C_||
       std::bit_cast<std::uint32_t>(*sourceApplicationElapsed23C_)!=std::bit_cast<std::uint32_t>(sample.updateSeconds)||
       !sourceAural00Owner_||!sourceAural00Owner_->raceIntroPublicationServiced(timing,serial)){
        error="F8 pre-Camera update requires the same active Root23C and serviced Aural publication";return false;
    }
    // Other-track/ALL8 roots have no F8. Partial actual custody is not absence;
    // the existing bridge authenticates the attached leaf before any store.
    if(race.f8Owner_||race.f8Source_||race.f8Attached_){
        rr::game::OriginalRaceF8Diagnostic50LA diagnostic;
        if(!updateConsumedF8199320(diagnostic)){
            error=diagnostic.operation?diagnostic.operation:"F8 actual Root23C timer rejected";
            return false;
        }
    }
    // The existing F8 publication history prevents replay. Keep this root
    // open for its real following native work and possible suspended skip.
    // No renderer20(0), complete forest traversal or root completion is claimed.
    error.clear();return true;
}
inline bool Application::finishNativeTaRaceRoot140CFC(rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::OriginalWreckComponent50LA& controller,
    const original_ui_source_timing50LH::PublicationView& timing,std::string& error)const noexcept {
    original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    if(!nativeTaRootEntered_||nativeTaRootRace_!=&race||sourceRace28COwner_.get()!=&race||
       nativeTaRootController_!=&controller||race.nativeIntroControllerSlot48()!=&controller||
       !race.nativeIntroRaceEntity()||race.nativeIntroRaceEntity()->sessionGeneration!=nativeTaRootGeneration_||
       !timing.sameOwner(nativeTaRootTiming_)||!timing.readPrepared(sample,serial)||serial!=nativeTaRootPublication_||
       sourceApplicationMode22C_!=2||!sourceApplicationElapsed23C_||
       std::bit_cast<std::uint32_t>(*sourceApplicationElapsed23C_)!=std::bit_cast<std::uint32_t>(sample.updateSeconds)||
       !sourceAural00Owner_||!sourceAural00Owner_->raceIntroPublicationServiced(timing,serial)){
        error="Intro Root140CFC requires the same accepted publication and completed Aural267CB0 service";return false;
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
       race.nativeIntroControllerSlot48()!=&controller||!timing.sameOwner(nativeTaRootTiming_)||
       !timing.readPrepared(sample,serial)||serial!=nativeTaRootPublication_||sourceApplicationMode22C_!=2){
        error="Native intro teardown cannot abandon a foreign or changed Root publication";return false;
    }
    // Explicit native terminal teardown, NOT140CFC success or rollback of
    // already performed callbacks. Keep23C and publication history. A new
    // consumed race may start; this same race can never resume source work.
    nativeTaRootEntered_=false;nativeTaRootAborted_=true;sourceApplicationMode22C_=0;
    error.clear();return true;
}

}
