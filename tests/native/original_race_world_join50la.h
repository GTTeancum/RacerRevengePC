#pragma once
#include "original_race_constructor50la.h"
#include "original_world_cold_prefix50la.h"
#include "original_ta_world_cold_prefix50la.h"
#include "original_gc_world_cold_prefix50la.h"
#include "original_ga_world_cold_prefix50la.h"
#include "original_ma_world_cold_prefix50la.h"
#include "original_mb_world_cold_prefix50la.h"
#include "original_sc_world_cold_prefix50la.h"
#include "original_gb_world_cold_prefix50la.h"
#include "original_rc_world_cold_prefix50la.h"
#include "original_tc_world_cold_prefix50la.h"
#include "original_sa_world_cold_prefix50la.h"

namespace rr::game {
inline const OriginalWreckComponent50LA* OriginalRaceConstructor50LA::nativeIntroWorldEntity()const noexcept {
    if(!counted_||rolledBack_||!application_||!resourcesPrepared_||!constructorSuffix_)return nullptr;
    const auto* selected=original_course_targets50KX::target(launchSelection_);
    if(!selected)return nullptr;
    if(selected->track==rr::platform::HostTrack50IM::TA){
        if(gaWorld74_||gaWorldCounted_||maWorld74_||maWorldCounted_||mbWorld74_||mbWorldCounted_||scWorld74_||scWorldCounted_||gbWorld74_||gbWorldCounted_||rcWorld74_||rcWorldCounted_||tcWorld74_||tcWorldCounted_||saWorld74_||saWorldCounted_||gcWorld74_||gcWorldCounted_||world74_||!taWorld74_||!taWorldCounted_||!taWorldPrefixComplete_)return nullptr;
        return &taWorldEntity_;
    }
    if(selected->track==rr::platform::HostTrack50IM::RA){
        if(gaWorld74_||gaWorldCounted_||maWorld74_||maWorldCounted_||mbWorld74_||mbWorldCounted_||scWorld74_||scWorldCounted_||gbWorld74_||gbWorldCounted_||rcWorld74_||rcWorldCounted_||tcWorld74_||tcWorldCounted_||saWorld74_||saWorldCounted_||gcWorld74_||gcWorldCounted_||taWorld74_||taWorldCounted_||!world74_||!world74_->worldColdConstructed_||
           world74_->worldApplication_!=application_||world74_->topologyClosed_||
           world74_->worldEntity_.reference00!=1||
           world74_->worldEntity_.sessionGeneration!=entity_.sessionGeneration)return nullptr;
        return &world74_->worldEntity_;
    }
    if(selected->track==rr::platform::HostTrack50IM::GC){
        if(gaWorld74_||gaWorldCounted_||maWorld74_||maWorldCounted_||mbWorld74_||mbWorldCounted_||scWorld74_||scWorldCounted_||gbWorld74_||gbWorldCounted_||rcWorld74_||rcWorldCounted_||tcWorld74_||tcWorldCounted_||saWorld74_||saWorldCounted_||world74_||taWorld74_||taWorldCounted_||!gcWorld74_||!gcWorldCounted_||
           !gcWorldPrefixComplete_||gcWorldEntity_.reference00!=1||
           gcWorldEntity_.sessionGeneration!=entity_.sessionGeneration||
           !application_->entityAuthority_.canRetire(gcWorldEntity_))return nullptr;
        return &gcWorldEntity_;
    }
    if(selected->track==rr::platform::HostTrack50IM::MA){
        if(gaWorld74_||gaWorldCounted_||world74_||taWorld74_||taWorldCounted_||gcWorld74_||gcWorldCounted_||
           saWorld74_||saWorldCounted_||mbWorld74_||mbWorldCounted_||scWorld74_||scWorldCounted_||gbWorld74_||gbWorldCounted_||rcWorld74_||rcWorldCounted_||tcWorld74_||tcWorldCounted_||!maWorld74_||!maWorldCounted_||
           !maWorldPrefixComplete_||maWorldEntity_.reference00!=1||
           maWorldEntity_.sessionGeneration!=entity_.sessionGeneration||
           !application_->entityAuthority_.canRetire(maWorldEntity_))return nullptr;
        return &maWorldEntity_;
    }
    if(selected->track==rr::platform::HostTrack50IM::SA){
        if(gaWorld74_||gaWorldCounted_||world74_||taWorld74_||taWorldCounted_||gcWorld74_||gcWorldCounted_||
           maWorld74_||maWorldCounted_||mbWorld74_||mbWorldCounted_||scWorld74_||scWorldCounted_||gbWorld74_||gbWorldCounted_||rcWorld74_||rcWorldCounted_||tcWorld74_||tcWorldCounted_||!saWorld74_||!saWorldCounted_||
           !saWorldPrefixComplete_||saWorldEntity_.reference00!=1||
           saWorldEntity_.sessionGeneration!=entity_.sessionGeneration||
           !application_->entityAuthority_.canRetire(saWorldEntity_))return nullptr;
        return &saWorldEntity_;
    }
    if(selected->track==rr::platform::HostTrack50IM::GA){
        if(world74_||taWorld74_||taWorldCounted_||gcWorld74_||gcWorldCounted_||
           maWorld74_||maWorldCounted_||mbWorld74_||mbWorldCounted_||scWorld74_||scWorldCounted_||gbWorld74_||gbWorldCounted_||rcWorld74_||rcWorldCounted_||tcWorld74_||tcWorldCounted_||saWorld74_||saWorldCounted_||
           !gaWorld74_||!gaWorldCounted_||!gaWorldPrefixComplete_||gaWorldEntity_.reference00!=1||
           gaWorldEntity_.sessionGeneration!=entity_.sessionGeneration||
           !application_->entityAuthority_.canRetire(gaWorldEntity_))return nullptr;
        return &gaWorldEntity_;
    }
    if(selected->track==rr::platform::HostTrack50IM::MB){
        if(gaWorld74_||gaWorldCounted_||world74_||taWorld74_||taWorldCounted_||gcWorld74_||gcWorldCounted_||
           saWorld74_||saWorldCounted_||maWorld74_||maWorldCounted_||scWorld74_||scWorldCounted_||gbWorld74_||gbWorldCounted_||rcWorld74_||rcWorldCounted_||tcWorld74_||tcWorldCounted_||!mbWorld74_||!mbWorldCounted_||
           !mbWorldPrefixComplete_||mbWorldEntity_.reference00!=1||
           mbWorldEntity_.sessionGeneration!=entity_.sessionGeneration||
           !application_->entityAuthority_.canRetire(mbWorldEntity_))return nullptr;
        return &mbWorldEntity_;
    }
    if(selected->track==rr::platform::HostTrack50IM::SC){
        if(gaWorld74_||gaWorldCounted_||world74_||taWorld74_||taWorldCounted_||gcWorld74_||gcWorldCounted_||
           saWorld74_||saWorldCounted_||maWorld74_||maWorldCounted_||mbWorld74_||mbWorldCounted_||gbWorld74_||gbWorldCounted_||rcWorld74_||rcWorldCounted_||tcWorld74_||tcWorldCounted_||!scWorld74_||!scWorldCounted_||
           !scWorldPrefixComplete_||scWorldEntity_.reference00!=1||
           scWorldEntity_.sessionGeneration!=entity_.sessionGeneration||
           !application_->entityAuthority_.canRetire(scWorldEntity_))return nullptr;
        return &scWorldEntity_;
    }
    if(selected->track==rr::platform::HostTrack50IM::GB){
        if(gaWorld74_||gaWorldCounted_||world74_||taWorld74_||taWorldCounted_||gcWorld74_||gcWorldCounted_||
           saWorld74_||saWorldCounted_||maWorld74_||maWorldCounted_||mbWorld74_||mbWorldCounted_||scWorld74_||scWorldCounted_||rcWorld74_||rcWorldCounted_||tcWorld74_||tcWorldCounted_||
           !gbWorld74_||!gbWorldCounted_||!gbWorldPrefixComplete_||gbWorldEntity_.reference00!=1||
           gbWorldEntity_.sessionGeneration!=entity_.sessionGeneration||
           !application_->entityAuthority_.canRetire(gbWorldEntity_))return nullptr;
        return &gbWorldEntity_;
    }
    if(selected->track==rr::platform::HostTrack50IM::RC){
        if(gaWorld74_||gaWorldCounted_||world74_||taWorld74_||taWorldCounted_||gcWorld74_||gcWorldCounted_||
           saWorld74_||saWorldCounted_||maWorld74_||maWorldCounted_||mbWorld74_||mbWorldCounted_||scWorld74_||scWorldCounted_||gbWorld74_||gbWorldCounted_||tcWorld74_||tcWorldCounted_||
           !rcWorld74_||!rcWorldCounted_||!rcWorldPrefixComplete_||rcWorldEntity_.reference00!=1||
           rcWorldEntity_.sessionGeneration!=entity_.sessionGeneration||
           !application_->entityAuthority_.canRetire(rcWorldEntity_))return nullptr;
        return &rcWorldEntity_;
    }
    if(selected->track==rr::platform::HostTrack50IM::TC){
        if(gaWorld74_||gaWorldCounted_||world74_||taWorld74_||taWorldCounted_||gcWorld74_||gcWorldCounted_||
           saWorld74_||saWorldCounted_||maWorld74_||maWorldCounted_||mbWorld74_||mbWorldCounted_||scWorld74_||scWorldCounted_||gbWorld74_||gbWorldCounted_||rcWorld74_||rcWorldCounted_||
           !tcWorld74_||!tcWorldCounted_||!tcWorldPrefixComplete_||tcWorldEntity_.reference00!=1||
           tcWorldEntity_.sessionGeneration!=entity_.sessionGeneration||
           !application_->entityAuthority_.canRetire(tcWorldEntity_))return nullptr;
        return &tcWorldEntity_;
    }
    return nullptr;
}
inline const OriginalWreckComponent50LA* OriginalRaceConstructor50LA::nativeIntroRaceEntity()const noexcept {
    return nativeIntroWorldEntity()?&entity_:nullptr;
}
inline bool OriginalRaceConstructor50LA::constructWorld1A6BC0(
    const original_main_entry50KG::Application& app,std::shared_ptr<ra_cold_aggregate50LA::Owner> world,
    std::uint32_t& pc)noexcept {
    pc=0x1a6bc0;
    if(!counted_||rolledBack_||!constructorSuffix_||!loadPrefixReached_||!resourcesPrepared_||
       worldAttempted_||gaWorld74_||gaWorldCounted_||maWorld74_||maWorldCounted_||mbWorld74_||mbWorldCounted_||scWorld74_||scWorldCounted_||gbWorld74_||gbWorldCounted_||rcWorld74_||rcWorldCounted_||tcWorld74_||tcWorldCounted_||saWorld74_||saWorldCounted_||world74_||taWorld74_||taWorldCounted_||gcWorld74_||gcWorldCounted_||!world||!raceAudio_||raceGroupCount_!=6||raceGroupsAttached_!=6||
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
    worldInitStep_=WorldInitStep::soundReset;
    pc=0x1a6bdc;return true; //Same-owner caller continues; no slot30 load.
}
inline bool OriginalRaceConstructor50LA::rollbackNativeRaceWorldFailure(std::uint32_t& pc)noexcept {
    pc=0x1a6bc0;
    if(gaWorld74_||gaWorldCounted_)return retireNativeGaWorldPrefix(pc);
    if(maWorld74_||maWorldCounted_)return retireNativeMaWorldPrefix(pc);
    if(mbWorld74_||mbWorldCounted_)return retireNativeMbWorldPrefix(pc);
    if(scWorld74_||scWorldCounted_)return retireNativeScWorldPrefix(pc);
    if(gbWorld74_||gbWorldCounted_)return retireNativeGbWorldPrefix(pc);
    if(rcWorld74_||rcWorldCounted_)return retireNativeRcWorldPrefix(pc);
    if(tcWorld74_||tcWorldCounted_)return retireNativeTcWorldPrefix(pc);
    if(saWorld74_||saWorldCounted_)return retireNativeSaWorldPrefix(pc);
    if(gcWorld74_||gcWorldCounted_)return retireNativeGcWorldPrefix(pc);
    if(taWorld74_||taWorldCounted_)return retireNativeTaWorldPrefix(pc);
    if(!world74_)return true;
    if(!counted_||rolledBack_||!application_||application_->attachmentLock423F58())return false;
    // A rejected world preflight may have acquired no count. Never invoke a
    // destructor for that case. A reached count uses its real typed rollback,
    // including actual3CC/3D4 resources if subsequently acquired.
    nativeWater70Domain_.reset();nativeWater70Object_.reset();nativeWater70State_=NativeWater70State::withdrawn;
    if(world74_->worldColdConstructed_&&
       !world74_->rollbackNativeWorldConstructionFailure(pc))return false;
    world74_.reset();return true;
}
inline bool OriginalRaceConstructor50LA::worldResetPredecessor1A6BF0Current()const noexcept {
    // Same real seven empty2350D0 owners constructed before this exact call.
    // The Race's native provider mixer is not relabeled as Aural's interface.
    if(resultsFeedbackThread_!=std::this_thread::get_id()||worldInitStep_!=WorldInitStep::soundReset||
       !nativeIntroWorldEntity()||!raceAudio_||raceGroupCount_!=6||raceGroupsAttached_!=6)return false;
    // World-specific half: RA aggregate, or the TA counted prefix (which owns
    // no topology/physics/collision objects and an empty source list500).
    if(world74_){
        if(world74_->worldAudio_!=raceAudio_||world74_->topologyAttempted_||world74_->worldPhysics3D4_||
           world74_->worldCollisionObjects3CC_||!world74_->worldList500_||world74_->worldListCount50C_)return false;
    }else{
        // The non-RA course world prefix (TA/GA/SA/MA/GC): counted, complete,
        // an empty source list500 and no controller in slot48 yet.
        const bool prefix=taWorld74_?taWorldCounted_&&taWorldPrefixComplete_&&taWorldList500_&&!taWorldListCount50C_:
            gaWorld74_?gaWorldCounted_&&gaWorldPrefixComplete_&&gaWorldList500_&&!gaWorldListCount50C_:
            saWorld74_?saWorldCounted_&&saWorldPrefixComplete_&&saWorldList500_&&!saWorldListCount50C_:
            maWorld74_?maWorldCounted_&&maWorldPrefixComplete_&&maWorldList500_&&!maWorldListCount50C_:
            mbWorld74_?mbWorldCounted_&&mbWorldPrefixComplete_&&mbWorldList500_&&!mbWorldListCount50C_:
            scWorld74_?scWorldCounted_&&scWorldPrefixComplete_&&scWorldList500_&&!scWorldListCount50C_:
            gbWorld74_?gbWorldCounted_&&gbWorldPrefixComplete_&&gbWorldList500_&&!gbWorldListCount50C_:
            rcWorld74_?rcWorldCounted_&&rcWorldPrefixComplete_&&rcWorldList500_&&!rcWorldListCount50C_:
            tcWorld74_?tcWorldCounted_&&tcWorldPrefixComplete_&&tcWorldList500_&&!tcWorldListCount50C_:
            gcWorld74_?gcWorldCounted_&&gcWorldPrefixComplete_&&gcWorldList500_&&!gcWorldListCount50C_:false;
        if(!prefix||nativeTaController48_)return false;
    }
    if(
       raceAudio_->coldThread_!=std::this_thread::get_id()||!raceAudio_->coldStarted_||
       raceAudio_->coldRetiring_||raceAudio_->nativeBindingsRetired_||raceAudio_->coldNext_!=7||
       raceAudio_->coldApplication_!=application_||raceAudio_->coldSession_!=entity_.sessionGeneration||
       !raceAudio_->coldMixer_||raceAudio_->coldEpoch_!=raceAudio_->coldMixer_->groupEpoch50IC()||
       raceAudio_->mixer_||raceAudio_->groups_.initialized||raceAudio_->coldTaIntroEmitter_)return false;
    for(std::size_t n=0;n<raceAudio_->coldMixer_->count();++n)
        if(raceAudio_->coldMixer_->active(n)||raceAudio_->coldMixer_->hasVoiceGroupGain50IC(n)||
           raceAudio_->bindings_[n]!=RaceAudioGroup50IC::Unrepresented)return false;
    for(std::size_t n=0;n<raceAudio_->coldGroups_.size();++n){
        const auto& group=raceAudio_->coldGroups_[n];const auto& e=group.entity;
        if(!group.constructed||!group.loaded||group.count48||!group.logicalMembers4C||
           group.taIntroAuthoring||group.taIntroManager||e.reference00!=1||e.firstChild18||e.descendants24||
           e.sessionGeneration!=entity_.sessionGeneration||group.slot40<0||
           static_cast<std::size_t>(group.slot40)>=application_->soundGroups434A10_.size()||
           application_->soundGroups434A10_[group.slot40]!=&e||
           !application_->entityAuthority_.canRetire(e))return false;
        for(const auto& sound:group.taIntroSounds)if(sound)return false;
    }
    for(std::size_t n=0;n<raceGroups_.size();++n){const auto* child=raceGroups_[n];
        const auto& actual=raceAudio_->coldGroups_[static_cast<std::size_t>(groupOrder_[n])].entity;
        if(child!=&actual||child->parent1C!=&entity_||child->previous10!=(n?raceGroups_[n-1]:nullptr)||
           child->next14!=(n+1<raceGroups_.size()?raceGroups_[n+1]:nullptr))return false;
    }
    const auto& worldGroup=raceAudio_->coldGroups_[static_cast<std::size_t>(RaceAudioGroup50IC::World4D8)].entity;
    return !worldGroup.parent1C&&!worldGroup.previous10&&!worldGroup.next14;
}
}
namespace original_main_entry50KG {
inline bool Application::continueConsumedRaWorldInit1A6BE0(
    rr::game::OriginalRaceConstructor50LA& owner,std::uint32_t& pc)const noexcept {
    using Step=rr::game::OriginalRaceConstructor50LA::WorldInitStep;
    pc=0x1a6be0;
    // Immutable creator fences precede root/weak/cursor reads. This private
    // caller is synchronous and can never be issued from a foreign thread.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||
       owner.resultsFeedbackThread_!=std::this_thread::get_id())return false;
    if(sourceRace28COwner_.get()!=&owner||raceConstructorColdOwner50LA_.lock().get()!=&owner||
       owner.application_!=entitiesOwner_||owner.profile_!=riderProfile||!riderProfile||
       !owner.counted_||owner.rolledBack_||!owner.worldAttempted_||!owner.anyWorld()||
       !owner.nativeIntroWorldEntity()||owner.worldInitStep_==Step::unreached||
       !entitiesOwner_||entitiesOwner_->attachmentLock423F58()||
       !entitiesOwner_->entityAuthority_.canRetire(owner.entity_))return false;
    if(owner.worldInitStep_==Step::soundReset){
        pc=0x1a6bf0;
        if(!owner.worldResetPredecessor1A6BF0Current())return false;
        pc=0x1a6be8;
        if(!sourceAural00Owner_)return false;
        rr::platform::OriginalApplicationAural50KG::Diagnostic reset;
        // Actual Application00 -> retained AuralA0, native replacement of its
        // manager+40 call. Rejection is NOT a zero/positive source RPC result.
        if(!sourceAural00Owner_->resetNativeMemory268300(*this,reset)){
            pc=reset.sourcePC;return false;
        }
        owner.worldInitStep_=Step::defaultLow2;
    }
    if(owner.worldInitStep_==Step::defaultLow2){
        owner.flags1B0KnownValue_=static_cast<std::uint8_t>(
            (owner.flags1B0KnownValue_&0xfcu)|1u); //1A6C10, before2E0 read.
        owner.flags1B0KnownMask_=static_cast<std::uint8_t>(owner.flags1B0KnownMask_|3u);
        owner.worldInitStep_=Step::profileLow2;
    }
    if(owner.worldInitStep_==Step::profileLow2){
        pc=0x1a6c18;
        if(!sourceUseProfile2E0_)return false; // Prior reset/default store retained.
        if(!*sourceUseProfile2E0_){
            // No native original-argv/track298 producer admits this branch.
            // Do not synthesize1A6C7C Profile bits or silently choose normal UI.
            pc=0x1a6c54;return false;
        }
        pc=0x1a6c34;
        original_course_targets50KX::Selection current;
        if(!riderProfile->courseRaceSnapshot(current)||
           !original_course_targets50KX::same(current,owner.launchSelection_))return false;
        // Actual byte1125C operand, not inferred native humanCount, Session
        // flags or an earned-result receipt. Preserve every upper1B0 lane.
        const auto low2=static_cast<std::uint8_t>((current.packed1125C>>5u)&3u);
        owner.flags1B0KnownValue_=static_cast<std::uint8_t>(
            (owner.flags1B0KnownValue_&0xfcu)|low2); //1A6C50 branch delay store.
        owner.worldInitStep_=Step::camera;
    }
    if(owner.worldInitStep_!=Step::camera)return false;
    return continueConsumedRaCamera1A9240(owner,pc);
}
inline bool Application::constructConsumedRaceWorld1A6BC0(rr::game::OriginalRaceConstructor50LA& owner,
    std::shared_ptr<ra_cold_aggregate50LA::Owner> world,std::uint32_t& pc)const noexcept {
    pc=0x1a6bc0;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||
       owner.resultsFeedbackThread_!=std::this_thread::get_id())return false;
    original_course_targets50KX::Selection selected;
    if(sourceRace28COwner_.get()!=&owner||raceConstructorColdOwner50LA_.lock().get()!=&owner||owner.application_!=entitiesOwner_||
       owner.profile_!=riderProfile||!riderProfile||!riderProfile->courseRaceSnapshot(selected)||
       !original_course_targets50KX::same(selected,owner.launchSelection_))return false;
    const auto* target=original_course_targets50KX::target(selected);
    if(!target||target->track!=rr::platform::HostTrack50IM::RA)return false;
    if(owner.worldAttempted_){
        // Native resumption never reconstructs, replaces or replays a reached
        // World/reset. A failed World constructor remains unreached/closed.
        if(!world||world.get()!=owner.world74_.get()||world.owner_before(owner.world74_)||
           owner.world74_.owner_before(world))return false;
    }else if(!owner.constructWorld1A6BC0(*this,std::move(world),pc))return false;
    return continueConsumedRaWorldInit1A6BE0(owner,pc);
}
}
