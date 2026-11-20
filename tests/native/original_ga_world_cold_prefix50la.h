#pragma once
#include "original_race_constructor50la.h"
#include "ga_original_world50lh.h"

namespace rr::game {
inline bool OriginalRaceConstructor50LA::constructGaWorld1A6BC0(
    const original_main_entry50KG::Application& app,
    std::shared_ptr<ga_original_world50LH::Owner> world,std::uint32_t& pc)noexcept {
    pc=0x1a6bc0;
    if(!counted_||rolledBack_||!constructorSuffix_||!loadPrefixReached_||!resourcesPrepared_||
       worldAttempted_||gbWorld74_||gbWorldCounted_||rcWorld74_||rcWorldCounted_||tcWorld74_||tcWorldCounted_||mcWorld74_||mcWorldCounted_||tbWorld74_||tbWorldCounted_||gcWorld74_||gcWorldCounted_||maWorld74_||maWorldCounted_||mbWorld74_||mbWorldCounted_||scWorld74_||scWorldCounted_||commonRouteWorld74_||commonRouteWorldCounted_||world74_||taWorld74_||taWorldCounted_||gaWorld74_||gaWorldCounted_||!world||!world->loaded()||
       !world->mesh()||!world->bank()||!application_||app.entities()!=application_||
       application_->attachmentLock423F58()||!raceAudio_||raceGroupCount_!=6||raceGroupsAttached_!=6)
        return false;
    const auto* selected=original_course_targets50KX::target(launchSelection_);
    if(!selected||selected->track!=rr::platform::HostTrack50IM::GA)return false;
    auto& audio=*raceAudio_;
    if(!audio.coldStarted_||audio.coldRetiring_||audio.coldNext_!=6||
       audio.coldApplication_!=application_||audio.coldSession_!=entity_.sessionGeneration||
       audio.coldThread_!=std::this_thread::get_id()||!audio.coldMixer_||
       audio.coldEpoch_!=audio.coldMixer_->groupEpoch50IC()||audio.mixer_||audio.groups_.initialized)
        return false;
    if(entity_.firstChild18!=raceGroups_[0])return false;
    for(std::size_t i=0;i<6;++i){const auto* child=raceGroups_[i];
        if(!child||child!=&audio.coldGroups_[static_cast<std::size_t>(groupOrder_[i])].entity||
           child->parent1C!=&entity_||child->previous10!=(i?raceGroups_[i-1]:nullptr)||
           child->next14!=(i+1<6?raceGroups_[i+1]:nullptr))return false;
    }
    auto& mixer=*audio.coldMixer_;
    for(std::size_t i=0;i<mixer.count();++i)if(mixer.active(i)||mixer.hasVoiceGroupGain50IC(i))return false;
    const auto key=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&gaWorldEntity_));
    auto& authority=application_->entityAuthority_;
    if(application_->objectCount423F60()>std::numeric_limits<std::uint32_t>::max()-2u||
       !authority.reserveAdditional(2)||!authority.canEnroll(gaWorldEntity_,key))return false;
    std::unique_ptr<OriginalWreckComponent50LA*[]> list;
    try {list.reset(new OriginalWreckComponent50LA*[10]);}catch(...){pc=0x1b5f64;return false;}
    // Prewarm does not increment any count. The same authenticated GA owner is
    // retained before the first actual constructor effect, including failure.
    worldAttempted_=true;gaWorld74_=std::move(world);
    pc=0x141300;
    const auto result=initializeOriginalWreckComponent50LA(gaWorldEntity_,
        OriginalWreckComponentKind50LA::entity141300,entity_.sessionGeneration,key,true,
        application_->objectCount423F60());
    if(result.status!=OriginalWreckComponentStatus50LA::complete||!authority.enroll(gaWorldEntity_))std::terminate();
    gaWorldCounted_=true;gaWorldMode30_=1;gaUseEnvironment35C_=0;
    pc=0x26fcf8;
    // Genuine World26FCBC constructor owns the seventh2350D0 call. Do not
    // initialize seven independent groups before this counted World Entity.
    if(!audio.constructNextCold2350D0(mixer,application_,entity_.sessionGeneration,RaceAudioGroup50IC::World4D8))return false;
    gaWorldList500_=std::move(list);gaWorldListCapacity504_=10;gaWorldListGrowth508_=100000;gaWorldListCount50C_=0;
    application_->worldRegistration434320_=0;gaWorldMode524_=3;
    gaWorldPrefixComplete_=true;
    worldInitStep_=WorldInitStep::soundReset; //Same caller continues 1A6BE0, as on RA/TA.
    pc=0x1b2918;return true;
}
inline bool OriginalRaceConstructor50LA::retireNativeGaWorldPrefix(std::uint32_t& pc)noexcept {
    pc=0x1b2870;
    if(!gaWorld74_&&!gaWorldCounted_)return true;
    if(!counted_||rolledBack_||nativeTaController48_||!application_||application_->attachmentLock423F58()||!raceAudio_)return false;
    auto& audio=*raceAudio_;auto& e=gaWorldEntity_;
    if(audio.coldApplication_!=application_||audio.coldSession_!=entity_.sessionGeneration||
       audio.coldThread_!=std::this_thread::get_id()||!audio.coldMixer_||
       audio.coldEpoch_!=audio.coldMixer_->groupEpoch50IC()||gaWorldListCount50C_)return false;
    if(gaWorldCounted_&&(e.reference00!=1||e.parent1C||e.previous10||e.next14||e.firstChild18||
       e.mask20||e.descendants24||e.sessionGeneration!=entity_.sessionGeneration||
       e.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e))||
       !application_->entityAuthority_.canRetire(e)))return false;
    const auto group=RaceAudioGroup50IC::World4D8;
    const bool groupCounted=audio.coldGroups_[static_cast<std::size_t>(group)].constructed;
    if(application_->objectCount423F60()<std::size_t(gaWorldCounted_)+std::size_t(groupCounted)||
       (groupCounted&&!audio.canReleaseEmptyCold142590(group)))return false;
    // This leaf cannot clear live intro/audio children. Their real owners must
    // close them first; rejection retains the prefix for a later cleanup retry.
    if(groupCounted&&!audio.releaseEmptyCold142590(group)){pc=0x235150;return false;}
    if(gaWorldCounted_){
        if(!application_->entityAuthority_.retire(e))std::terminate();
        --e.reference00;--application_->objectCount423F60();gaWorldCounted_=false;
    }
    gaWorldList500_.reset();gaWorld74_.reset();gaWorldPrefixComplete_=false;return true;
}
}
namespace original_main_entry50KG {
inline bool Application::constructConsumedGaRaceWorld1A6BC0(rr::game::OriginalRaceConstructor50LA& owner,
    std::shared_ptr<ga_original_world50LH::Owner> world,std::uint32_t& pc)const noexcept {
    pc=0x1a6bc0;original_course_targets50KX::Selection selected;
    if(raceConstructorColdOwner50LA_.lock().get()!=&owner||owner.application_!=entitiesOwner_||
       owner.profile_!=riderProfile||!riderProfile||!riderProfile->courseRaceSnapshot(selected)||
       !original_course_targets50KX::same(selected,owner.launchSelection_))return false;
    const auto* target=original_course_targets50KX::target(selected);
    if(!target||target->track!=rr::platform::HostTrack50IM::GA)return false;
    // One race class (1A65C0) on every course: the world constructor 1A6BC0
    // continues into the same slot30 prefix 1A6BE0 (sound reset, profile
    // low2, counted Camera34) as on RA and TA.
    if(owner.worldAttempted_){
        // Native resumption never reconstructs or replays a reached prefix.
        if(!world||world.get()!=owner.gaWorld74_.get())return false;
    }else if(!owner.constructGaWorld1A6BC0(*this,std::move(world),pc))return false;
    return continueConsumedRaWorldInit1A6BE0(owner,pc);
}
}
