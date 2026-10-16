#pragma once
#include "original_race_constructor50la.h"
#include "original_world_cold_prefix50la.h"

namespace rr::game {
inline bool OriginalRaceConstructor50LA::constructWorld1A6BC0(
    const original_main_entry50KG::Application& app,std::shared_ptr<ra_cold_aggregate50LA::Owner> world,
    std::uint32_t& pc)noexcept {
    pc=0x1a6bc0;
    if(!counted_||rolledBack_||!constructorSuffix_||!loadPrefixReached_||!resourcesPrepared_||
       worldAttempted_||world74_||!world||!raceAudio_||raceGroupCount_!=6||raceGroupsAttached_!=6||
       world->worldColdAttempted_||world->worldColdConstructed_||world->worldApplication_||
       raceAudio_->coldApplication_!=application_||raceAudio_->coldSession_!=entity_.sessionGeneration||
       raceAudio_->coldThread_!=std::this_thread::get_id()||!raceAudio_->coldMixer_||
       raceAudio_->coldEpoch_!=raceAudio_->coldMixer_->groupEpoch50IC())return false;
    for(std::size_t i=0;i<6;++i){
        const auto* child=raceGroups_[i];
        if(!child||child->parent1C!=&entity_||child->previous10!=(i?raceGroups_[i-1]:nullptr)||
           child->next14!=(i+1<6?raceGroups_[i+1]:nullptr))return false;
    }
    // Retain the SAME decoded/query/static aggregate before a fallible counted
    // constructor. This is native failure backing, not successful source74
    // publication or a second world. Already-acquired worlds were rejected.
    worldAttempted_=true;world74_=std::move(world);
    if(!world74_->constructWorldCold1B2870(app,*raceAudio_,*raceAudio_->coldMixer_,
        entity_.sessionGeneration,pc))return false;
    pc=0x1a6bdc;return true; //Only world constructor prefix, no slot30 load.
}
inline bool OriginalRaceConstructor50LA::rollbackNativeRaceWorldFailure(std::uint32_t& pc)noexcept {
    pc=0x1a6bc0;
    if(!world74_)return true;
    if(!counted_||rolledBack_||!application_||application_->attachmentLock423F58())return false;
    // A rejected world preflight may have acquired no count. Never invoke a
    // destructor for that case. A reached count uses its real typed rollback,
    // including actual3CC/3D4 resources if subsequently acquired.
    if(world74_->worldColdConstructed_&&
       !world74_->rollbackNativeWorldConstructionFailure(pc))return false;
    world74_.reset();return true;
}
}
namespace original_main_entry50KG {
inline bool Application::constructConsumedRaceWorld1A6BC0(rr::game::OriginalRaceConstructor50LA& owner,
    std::shared_ptr<ra_cold_aggregate50LA::Owner> world,std::uint32_t& pc)const noexcept {
    pc=0x1a6bc0;
    original_course_targets50KX::Selection selected;
    if(raceConstructorColdOwner50LA_.lock().get()!=&owner||owner.application_!=entitiesOwner_||
       owner.profile_!=riderProfile||!riderProfile||!riderProfile->courseRaceSnapshot(selected)||
       !original_course_targets50KX::same(selected,owner.launchSelection_))return false;
    const auto* target=original_course_targets50KX::target(selected);
    if(!target||target->track!=rr::platform::HostTrack50IM::RA)return false;
    return owner.constructWorld1A6BC0(*this,std::move(world),pc);
}
}
