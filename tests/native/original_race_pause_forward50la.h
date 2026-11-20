#pragma once
#include "original_race_world_join50la.h"
#include "../../src/game/original_pause_storage50la.h"

namespace rr::game {
// Canonical1BD7C0 rows5..8 under the retained Race/Pause. The caller binds
// the actual Application game-settings facet before entering this private leaf.
inline bool OriginalRaceConstructor50LA::adjustPauseAudio1BD7C0(
    const OriginalPauseStorage50LA& pause,OriginalUiGameSettingsProfile50KG& profile,
    const OriginalApplicationAnimationClockView50KG& clock,std::uint32_t row,
    bool increase,bool& changed)noexcept {
    changed=false;std::uint32_t paused=0;
    if(row>=4||!counted_||rolledBack_||!nativeRootAttached_||!application_||
       pause100_.get()!=&pause||!pause.active200_||pause.mode1F0_||
       !validOriginalUiGameSettingsProfile50KG(profile)||!clock.readPause11C(paused)||!paused)
        return false;
    const auto old=profile.volume11020[row];
    // Original bound branches do not reach a profile helper, floating-point
    // conversion or group setter. In particular they need no mixer revision.
    if((increase&&old==100u)||(!increase&&old==0u))return true;
    if(!raceAudio_||application_->attachmentLock423F58()||raceGroupCount_!=6||raceGroupsAttached_!=6||
       raceAudio_->coldApplication_!=application_||raceAudio_->coldSession_!=entity_.sessionGeneration||
       !raceAudio_->current()||!application_->entityAuthority_.canRetire(entity_)||
       !original_entity_chain_detail50LA::forest(application_->entityAuthority_.view())||
       entity_.firstChild18!=raceGroups_[0])return false;
    // These are the actual Race110/114/118/11C/120/124 constructor fields,
    // not enum labels standing in for unattached source objects.
    // 149p: a human race's attached F8 child follows Race124 (1A8D.. F8 attach,
    // as the human root publication proves); it is not a volume group.
    const auto* trailing=(f8Attached_&&f8Owner_&&f8Source_==f8Owner_.get())?f8Source_->entity():nullptr;
    if(trailing&&(trailing->parent1C!=&entity_||trailing->previous10!=raceGroups_[5]||trailing->next14))trailing=nullptr;
    for(std::size_t i=0;i<6;++i){
        const auto* child=raceGroups_[i];
        if(!child||child!=&raceAudio_->coldGroups_[static_cast<std::size_t>(groupOrder_[i])].entity||
           child->parent1C!=&entity_||child->previous10!=(i?raceGroups_[i-1]:nullptr)||
           child->next14!=(i+1<6?raceGroups_[i+1]:trailing))return false;
    }
    // Effects alone reaches Race74->World4D8 after Race110 and114.
    // World4D8 is owned by this real World, not a made-up seventh Race child.
    // Every course: the race's one counted World (common rewrite).
    if(row==0&&!nativeIntroWorldEntity())return false;
    const auto observed=profile.volume11020;
    // Shared native transport preflights same-profile configuration, actual
    // registered group slots, epoch/thread, voice gains/leases and aggregate
    // revision capacity before any profile store. Then profile, group38 and
    // native voice gains commit in original order. It does not issue26D200.
    if(!raceAudio_->adjustNativePauseAudio(profile,application_,*raceAudio_->mixer_,
        clock,observed,row,increase))return false;
    changed=true;return true;
}
// Actual table3CEE80+50=142040, separate from direct1ACF50 below.
inline bool OriginalRaceConstructor50LA::pauseEntity142040(bool requested)noexcept {
    // 149p: one plain line per distinct refusal step (logs-only tests).
    const auto refuse=[](int step)noexcept{static int last=0;
        if(step!=last){last=step;std::fprintf(stderr,"race_pause_142040 refused step=%d\n",step);}return false;};
    if(!counted_||rolledBack_||!application_||!raceAudio_)return refuse(1);
    if(application_->attachmentLock423F58()||raceGroupCount_!=6||raceGroupsAttached_!=6)return refuse(2);
    if(raceAudio_->coldApplication_!=application_||raceAudio_->coldSession_!=entity_.sessionGeneration||
       !raceAudio_->current())return refuse(3);
    if(!application_->entityAuthority_.canRetire(entity_)||
       !original_entity_chain_detail50LA::forest(application_->entityAuthority_.view())||
       entity_.firstChild18!=raceGroups_[0])return refuse(4);
    // 149p: the human race's attached F8 child follows Race124 (the human
    // root publication proves that chain). Its slot50 is not an audio group;
    // the port's 142040 reaches only the six race-audio groups (PORT
    // STAND-IN as before: F8 has no paused state of its own here).
    const auto* trailing=(f8Attached_&&f8Owner_&&f8Source_==f8Owner_.get())?f8Source_->entity():nullptr;
    if(trailing&&(trailing->parent1C!=&entity_||trailing->previous10!=raceGroups_[5]||trailing->next14))trailing=nullptr;
    std::array<bool,6> reached{};std::uint64_t changes=0;
    for(std::size_t i=0;i<6;++i){
        auto* child=raceGroups_[i];
        if(!child||child!=&raceAudio_->coldGroups_[static_cast<std::size_t>(groupOrder_[i])].entity||
           child->parent1C!=&entity_||child->previous10!=(i?raceGroups_[i-1]:nullptr)||
           child->next14!=(i+1<6?raceGroups_[i+1]:trailing))return refuse(10+int(i));
        reached[i]=(entity_.descendants24&0x20u)&&(child->flags29&1u)&&
            !(child->flags29&8u)&&(child->mask20&0x20u);
        if(!reached[i])continue;
        if(!raceAudio_->canPauseColdGroup2356A0(groupOrder_[i],requested))return refuse(20+int(i));
        if((raceAudio_->groups_.state44[static_cast<std::size_t>(groupOrder_[i])]!=0)!=requested)
            for(std::size_t slot=0;slot<raceAudio_->count_;++slot)
                if(raceAudio_->bindings_[slot]==groupOrder_[i])++changes;
    }
    // Reserve the admitted whole traversal's control-counter capacity before
    // its first source store; individual preflights alone could undercount.
    if(!raceAudio_->mixer_->canRecordControlChanges(changes))return refuse(30);
    entity_.flags29=static_cast<std::uint8_t>((entity_.flags29&~4u)|(requested?4u:0u));
    for(std::size_t i=0;i<6;++i)if(reached[i])
        if(!raceAudio_->pauseColdGroup2356A0(groupOrder_[i],requested))std::terminate();
    return true;
}
inline bool OriginalRaceConstructor50LA::pauseWorld1ACF50(bool requested)noexcept {
    if(!counted_||rolledBack_||!application_||!raceAudio_||application_->attachmentLock423F58()||
       raceAudio_->coldApplication_!=application_||raceAudio_->coldSession_!=entity_.sessionGeneration)
        return false;
    // Every course: the race's one counted World reaches the race-audio
    // World4D8 group (common rewrite).
    if(!nativeIntroWorldEntity())return false;
    //1ACF50..64 loads race74, then world4D8 and tailcalls that actual slot50.
    // It never calls base142040 on the race or walks its six group children.
    return raceAudio_->pauseColdGroup2356A0(RaceAudioGroup50IC::World4D8,requested);
}
}
namespace original_main_entry50KG {
inline bool Application::pauseConsumedRaceEntity142040(rr::game::OriginalRaceConstructor50LA& owner,
    bool requested)const noexcept {
    if(sourceRace28COwner_.get()!=&owner||raceConstructorColdOwner50LA_.lock().get()!=&owner||
       owner.application_!=entitiesOwner_||owner.profile_!=riderProfile)return false;
    return owner.pauseEntity142040(requested);
}
inline bool Application::pauseConsumedRaceWorld1ACF50(rr::game::OriginalRaceConstructor50LA& owner,
    bool requested)const noexcept {
    if(raceConstructorColdOwner50LA_.lock().get()!=&owner||owner.application_!=entitiesOwner_||
       owner.profile_!=riderProfile)return false;
    return owner.pauseWorld1ACF50(requested);
}
}
