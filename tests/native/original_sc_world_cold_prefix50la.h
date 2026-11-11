#pragma once
#include "original_race_constructor50la.h"
#include "sc_original_world50lm.h"

namespace rr::game {
inline bool OriginalRaceConstructor50LA::constructScWorld1A6BC0(
    const original_main_entry50KG::Application& app,
    std::shared_ptr<sc_original_world50LM::Owner> world,std::uint32_t& pc)noexcept {
    pc=0x1a6bc0;
    if(!counted_||rolledBack_||!constructorSuffix_||!loadPrefixReached_||!resourcesPrepared_||
       worldAttempted_||gbWorld74_||gbWorldCounted_||rcWorld74_||rcWorldCounted_||tcWorld74_||tcWorldCounted_||gaWorld74_||gaWorldCounted_||mbWorld74_||mbWorldCounted_||maWorld74_||maWorldCounted_||gcWorld74_||gcWorldCounted_||saWorld74_||saWorldCounted_||world74_||taWorld74_||taWorldCounted_||scWorld74_||scWorldCounted_||!world||!world->loaded()||
       !world->mesh()||!world->bank()||!application_||app.entities()!=application_||
       application_->attachmentLock423F58()||!raceAudio_||raceGroupCount_!=6||raceGroupsAttached_!=6)
        return false;
    const auto* selected=original_course_targets50KX::target(launchSelection_);
    if(!selected||selected->track!=rr::platform::HostTrack50IM::SC)return false;
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
    const auto key=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&scWorldEntity_));
    auto& authority=application_->entityAuthority_;
    if(application_->objectCount423F60()>std::numeric_limits<std::uint32_t>::max()-2u||
       !authority.reserveAdditional(2)||!authority.canEnroll(scWorldEntity_,key))return false;
    std::unique_ptr<OriginalWreckComponent50LA*[]> list;
    try {list.reset(new OriginalWreckComponent50LA*[10]);}catch(...){pc=0x1b5f64;return false;}
    // Prewarm does not increment any count. The same authenticated SC owner is
    // retained before the first actual constructor effect, including failure.
    worldAttempted_=true;scWorld74_=std::move(world);
    pc=0x141300;
    const auto result=initializeOriginalWreckComponent50LA(scWorldEntity_,
        OriginalWreckComponentKind50LA::entity141300,entity_.sessionGeneration,key,true,
        application_->objectCount423F60());
    if(result.status!=OriginalWreckComponentStatus50LA::complete||!authority.enroll(scWorldEntity_))std::terminate();
    scWorldCounted_=true;scWorldMode30_=1;scUseEnvironment35C_=0;
    pc=0x26fcf8;
    // Genuine World26FCBC constructor owns the seventh2350D0 call. Do not
    // initialize seven independent groups before this counted World Entity.
    if(!audio.constructNextCold2350D0(mixer,application_,entity_.sessionGeneration,RaceAudioGroup50IC::World4D8))return false;
    scWorldList500_=std::move(list);scWorldListCapacity504_=10;scWorldListGrowth508_=100000;scWorldListCount50C_=0;
    application_->worldRegistration434320_=0;scWorldMode524_=3;
    scWorldPrefixComplete_=true;
    worldInitStep_=WorldInitStep::soundReset; //Same caller continues 1A6BE0, as on RA/TA.
    pc=0x1b2918;return true;
}
inline bool OriginalRaceConstructor50LA::retireNativeScWorldPrefix(std::uint32_t& pc)noexcept {
    pc=0x1b2870;
    if(!scWorld74_&&!scWorldCounted_)return true;
    if(!counted_||rolledBack_||nativeTaController48_||!application_||application_->attachmentLock423F58()||!raceAudio_)return false;
    auto& audio=*raceAudio_;auto& e=scWorldEntity_;
    if(audio.coldApplication_!=application_||audio.coldSession_!=entity_.sessionGeneration||
       audio.coldThread_!=std::this_thread::get_id()||!audio.coldMixer_||
       audio.coldEpoch_!=audio.coldMixer_->groupEpoch50IC()||scWorldListCount50C_)return false;
    if(scWorldCounted_&&(e.reference00!=1||e.parent1C||e.previous10||e.next14||e.firstChild18||
       e.mask20||e.descendants24||e.sessionGeneration!=entity_.sessionGeneration||
       e.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e))||
       !application_->entityAuthority_.canRetire(e)))return false;
    const auto group=RaceAudioGroup50IC::World4D8;
    const bool groupCounted=audio.coldGroups_[static_cast<std::size_t>(group)].constructed;
    if(application_->objectCount423F60()<std::size_t(scWorldCounted_)+std::size_t(groupCounted)||
       (groupCounted&&!audio.canReleaseEmptyCold142590(group)))return false;
    // This leaf cannot clear live intro/audio children. Their real owners must
    // close them first; rejection retains the prefix for a later cleanup retry.
    if(groupCounted&&!audio.releaseEmptyCold142590(group)){pc=0x235150;return false;}
    if(scWorldCounted_){
        if(!application_->entityAuthority_.retire(e))std::terminate();
        --e.reference00;--application_->objectCount423F60();scWorldCounted_=false;
    }
    scWorldList500_.reset();scWorld74_.reset();scWorldPrefixComplete_=false;return true;
}
}
namespace original_main_entry50KG {
inline bool Application::constructConsumedScRaceWorld1A6BC0(rr::game::OriginalRaceConstructor50LA& owner,
    std::shared_ptr<sc_original_world50LM::Owner> world,std::uint32_t& pc)const noexcept {
    pc=0x1a6bc0;original_course_targets50KX::Selection selected;
    if(raceConstructorColdOwner50LA_.lock().get()!=&owner||owner.application_!=entitiesOwner_||
       owner.profile_!=riderProfile||!riderProfile||!riderProfile->courseRaceSnapshot(selected)||
       !original_course_targets50KX::same(selected,owner.launchSelection_))return false;
    const auto* target=original_course_targets50KX::target(selected);
    if(!target||target->track!=rr::platform::HostTrack50IM::SC)return false;
    // One race class (1A65C0) on every course: the world constructor 1A6BC0
    // continues into the same slot30 prefix 1A6BE0 (sound reset, profile
    // low2, counted Camera34) as on RA and TA.
    if(owner.worldAttempted_){
        // Native resumption never reconstructs or replays a reached prefix.
        if(!world||world.get()!=owner.scWorld74_.get())return false;
    }else if(!owner.constructScWorld1A6BC0(*this,std::move(world),pc))return false;
    return continueConsumedRaWorldInit1A6BE0(owner,pc);
}
}
