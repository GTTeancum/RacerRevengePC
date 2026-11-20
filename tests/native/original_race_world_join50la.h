#pragma once
// Common rewrite (R1, PLAN B-C05): the race's ONE World at race+74 for every
// course. 1A6BC0 -> 1B2870 -> 141300 -> 2350D0(World4D8) -> 1B2918, one intro
// identity rule and one retirement (1B2870 dtor -> 235150 -> 1B5F64). Replaces
// the 12 original_{ga,..,tc}_world_cold_prefix50la.h prefixes, the RA aggregate
// world slot and their per-course exclusion lists. The 1A9EE0 track load fills
// the World's Track afterwards (common_track50CR::Load, R3).
#include "original_race_constructor50la.h"
#include "common_race_world50cr.h"
#include "common_race_log50cr.h"

namespace rr::game {
// One rule (common_race_world50CR::introWorldEntity): the race's one World,
// counted, prefix complete, reference00==1, same session generation, retirable.
inline const OriginalWreckComponent50LA* OriginalRaceConstructor50LA::nativeIntroWorldEntity()const noexcept {
    if(!counted_||rolledBack_||!application_||!resourcesPrepared_||!constructorSuffix_||!world74_)return nullptr;
    const auto& w=*world74_;
    if(!w.counted||!w.prefixComplete||w.entity.reference00!=1||
       w.entity.sessionGeneration!=entity_.sessionGeneration||
       !application_->entityAuthority_.canRetire(w.entity))return nullptr;
    return &w.entity;
}
inline const OriginalWreckComponent50LA* OriginalRaceConstructor50LA::nativeIntroRaceEntity()const noexcept {
    return nativeIntroWorldEntity()?&entity_:nullptr;
}
inline bool OriginalRaceConstructor50LA::constructWorld1A6BC0(
    const original_main_entry50KG::Application& app,
    std::shared_ptr<common_track50CR::Track> track,std::uint32_t& pc)noexcept {
    pc=0x1a6bc0;
    if(!counted_||rolledBack_||!constructorSuffix_||!loadPrefixReached_||!resourcesPrepared_||
       worldAttempted_||world74_||!track||track->loaded()||!track->identity.code.valid()||
       !(track->identity.code==launchCourse_)||
       !application_||app.entities()!=application_||
       application_->attachmentLock423F58()||!raceAudio_||raceGroupCount_!=6||raceGroupsAttached_!=6)
        return false;
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
    // Host allocations first (no count effect): the 0x550 World and its +500
    // list. 1B2870 allocates the list with capacity 10 and growth 100000.
    std::shared_ptr<common_race_world50CR::World> world;
    std::unique_ptr<OriginalWreckComponent50LA*[]> list;
    try {world=std::make_shared<common_race_world50CR::World>();list.reset(new OriginalWreckComponent50LA*[10]{});}
    catch(...){pc=0x1b5f64;return false;}
    const auto key=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&world->entity));
    auto& authority=application_->entityAuthority_;
    if(application_->objectCount423F60()>std::numeric_limits<std::uint32_t>::max()-2u||
       !authority.reserveAdditional(2)||!authority.canEnroll(world->entity,key))return false;
    // Prewarm does not increment any count. The World (with its unloaded
    // Track) is retained before the first actual constructor effect, including failure.
    world->track=std::move(track);
    worldAttempted_=true;world74_=world;
    pc=0x141300;
    const auto result=initializeOriginalWreckComponent50LA(world->entity,
        OriginalWreckComponentKind50LA::entity141300,entity_.sessionGeneration,key,true,
        application_->objectCount423F60());
    if(result.status!=OriginalWreckComponentStatus50LA::complete||!authority.enroll(world->entity))std::terminate();
    world->counted=true;world->mode30=1;world->useEnvironment35C=0;
    pc=0x26fcf8;
    // Genuine World26FCBC constructor owns the seventh2350D0 call. Do not
    // initialize seven independent groups before this counted World Entity.
    if(!audio.constructNextCold2350D0(mixer,application_,entity_.sessionGeneration,RaceAudioGroup50IC::World4D8))return false;
    world->list500=std::move(list);world->listCapacity504=10;world->listGrowth508=100000;world->listCount50C=0;
    application_->worldRegistration434320_=0;world->mode524=3;
    world->prefixComplete=true;
    worldInitStep_=WorldInitStep::soundReset; //Same caller continues1A6BE0.
    pc=0x1b2918;return true;
}
inline bool OriginalRaceConstructor50LA::retireNativeWorldPrefix(std::uint32_t& pc)noexcept {
    pc=0x1b2870;
    if(!world74_)return true;
    auto& w=*world74_;
    if(!w.counted){world74_.reset();return true;} //Rejected preflight acquired no count.
    if(!counted_||rolledBack_||nativeController48_||!application_||application_->attachmentLock423F58()||!raceAudio_)return false;
    auto& audio=*raceAudio_;auto& e=w.entity;
    if(audio.coldApplication_!=application_||audio.coldSession_!=entity_.sessionGeneration||
       audio.coldThread_!=std::this_thread::get_id()||!audio.coldMixer_||
       audio.coldEpoch_!=audio.coldMixer_->groupEpoch50IC()||w.listCount50C)return false;
    if(e.reference00!=1||e.parent1C||e.previous10||e.next14||e.firstChild18||
       e.mask20||e.descendants24||e.sessionGeneration!=entity_.sessionGeneration||
       e.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e))||
       !application_->entityAuthority_.canRetire(e))return false;
    const auto group=RaceAudioGroup50IC::World4D8;
    const bool groupCounted=audio.coldGroups_[static_cast<std::size_t>(group)].constructed;
    if(application_->objectCount423F60()<1u+std::size_t(groupCounted)||
       (groupCounted&&!audio.canReleaseEmptyCold142590(group)))return false;
    // This leaf cannot clear live intro/audio children. Their real owners must
    // close them first; rejection retains the prefix for a later cleanup retry.
    if(groupCounted&&!audio.releaseEmptyCold142590(group)){pc=0x235150;return false;}
    if(!application_->entityAuthority_.retire(e))std::terminate();
    --e.reference00;--application_->objectCount423F60();w.counted=false;
    pc=0x1b5f64;
    w.list500.reset();w.listCapacity504=w.listCount50C=0;w.prefixComplete=false;
    world74_.reset();return true;
}
inline bool OriginalRaceConstructor50LA::rollbackNativeRaceWorldFailure(std::uint32_t& pc)noexcept {
    pc=0x1a6bc0;
    return retireNativeWorldPrefix(pc);
}
inline bool OriginalRaceConstructor50LA::worldResetPredecessor1A6BF0Current()const noexcept {
    // Same real seven empty2350D0 owners constructed before this exact call.
    // The Race's native provider mixer is not relabeled as Aural's interface.
    if(resultsFeedbackThread_!=std::this_thread::get_id()||worldInitStep_!=WorldInitStep::soundReset||
       !nativeIntroWorldEntity()||!raceAudio_||raceGroupCount_!=6||raceGroupsAttached_!=6)return false;
    // The one World's counted prefix: complete, an empty source list500 and no
    // controller in slot48 yet (the same state on every course).
    if(!world74_||!world74_->counted||!world74_->prefixComplete||!world74_->list500||
       world74_->listCount50C||nativeController48_)return false;
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
    std::shared_ptr<common_track50CR::Track> track,std::uint32_t& pc)const noexcept {
    namespace log=common_race_log50CR;
    pc=0x1a6bc0;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||
       owner.resultsFeedbackThread_!=std::this_thread::get_id())return false;
    original_course_targets50KX::Selection selected;
    if(sourceRace28COwner_.get()!=&owner||raceConstructorColdOwner50LA_.lock().get()!=&owner||owner.application_!=entitiesOwner_||
       owner.profile_!=riderProfile||!riderProfile||!riderProfile->courseRaceSnapshot(selected)||
       !original_course_targets50KX::same(selected,owner.launchSelection_)){
        log::load(log::Step::world1A6BC0,false,0,"race/profile/launch selection differs at 1A6BC0");return false;
    }
    if(owner.worldAttempted_){
        // Native resumption never reconstructs, replaces or replays a reached
        // World/reset. A failed World constructor remains unreached/closed.
        if(!owner.world74_||!track||owner.world74_->track!=track)return false;
    }else{
        std::shared_ptr<common_race_world50CR::World> world;
        if(!common_race_world50CR::construct1A6BC0(owner,*this,std::move(track),world,pc))return false;
    }
    if(!continueConsumedRaWorldInit1A6BE0(owner,pc)){
        log::load(log::Step::cameraRoot1A9240,false,0,"1A6BE0 sound reset / 1B0 low2 / 1A9240 camera root refused",nullptr,pc);
        return false;
    }
    return true;
}
}
