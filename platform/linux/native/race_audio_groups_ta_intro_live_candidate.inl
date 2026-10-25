#pragma once
#include <cmath>
namespace rr::platform {
inline bool RaceAudioGroups50IC::beginTaIntro177750(
    std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate> authoring,
    game::OriginalWreckComponent50LA& controller,std::shared_ptr<const assets::AudioBank50HC> bank,
    std::size_t first,std::size_t count)noexcept {
    if(taIntroConstructionAdmission_||taIntroLive_||taIntroTransport_||coldTaIntroEmitter_||
       !current()||!authoring||!bank||count<3||first>=count_||count>count_-first||
       controller.kind!=game::OriginalWreckComponentKind50LA::cameraController174A30||
       controller.sessionGeneration!=coldSession_||controller.reference00!=1||
       !coldApplication_||!coldApplication_->entityAuthority_.canRetire(controller))return false;
    taIntroExpectedController_=&controller;
    taIntroFirstVoice_=first;taIntroVoiceCount_=count;taIntroConstructionAdmission_=true;
    if(!taIntroExistingBindingsCurrent())return false;
    if(constructTaIntroEvents177750(authoring)!=TaIntroConstructionResult::eventsComplete)return false;
    auto& group=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    if(!mixer_->canRecordControlChanges(count*2))return false;
    for(std::size_t n=0;n<count;++n){const auto slot=first+n;
        if(!mixer_->setVoiceGroupGain50IC(slot,normalized(groups_,game::RaceAudioGroup50IC::Announcer120))||
           !mixer_->setVoiceGroupPaused50IC(slot,groups_.state44[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)]!=0))return false;
        bindings_[slot]=game::RaceAudioGroup50IC::Announcer120;
    }
    taIntroTransport_=NativeRaceIntroTransport50LA::create(group.taIntroManager,*mixer_,first,count,
        std::move(authoring),std::move(bank));
    return bool(taIntroTransport_);
}
inline bool RaceAudioGroups50IC::finishTaIntro177750(game::OriginalWreckComponent50LA& controller)noexcept {
    // Only the concrete friend Session calls this after publishing and loading
    // its actual cue-map5C. No claimed callback result is consumed as proof.
    if(taIntroExpectedController_!=&controller||taIntroLive_||!taIntroTransport_||
       !taIntroTransport_->current()||!coldTaIntroEmitterIdentity()||
       coldTaIntroEmitter_->count200!=3||!attachTaIntro141960(controller))return false;
    taIntroLive_=true;return taIntroReady();
}
inline bool RaceAudioGroups50IC::taIntroReady()const noexcept {
    if(!taIntroLive_||!taIntroTransport_||!taIntroTransport_->current()||
       coldRetiring_||nativeBindingsRetired_||!coldTaIntroEmitterIdentity())return false;
    const auto& emitter=*coldTaIntroEmitter_;
    if(!emitter.controllerParent||!emitter.everAttached||emitter.count200!=3||emitter.pendingEvent)return false;
    bool selectedSoundKnown=emitter.sound1F0==nullptr;
    for(std::size_t n=0;n<3;++n){
        const auto& sound=*emitter.ownedEvents[n]->sound04.value();
        selectedSoundKnown=selectedSoundKnown||emitter.sound1F0==&sound;
        if(sound.channel2C>=0&&sound.channel2C!=static_cast<std::int32_t>(taIntroLatest_[n].serial))return false;
    }
    return selectedSoundKnown;
}
inline RaceAudioGroups50IC::TaIntroDispatchResult RaceAudioGroups50IC::dispatchTaIntro292360(
    unsigned eventId,unsigned restart,unsigned immediate)noexcept {
    if(!taIntroReady()||restart||immediate)return TaIntroDispatchResult::unavailable;
    auto& emitter=*coldTaIntroEmitter_;
    for(std::size_t n=0;n<emitter.count200;++n){auto& wrapper=*emitter.members204[n];
        if(wrapper.id00!=eventId)continue;
        if(!wrapper.sound04||!*wrapper.sound04)return TaIntroDispatchResult::sourceFalse;
        if(!taIntroTransport_->canPlay(eventId))return TaIntroDispatchResult::unavailable;
        auto& sound=**wrapper.sound04;
        emitter.sound1F0=&sound; //292810 before Sound virtual+1C.
        //233E90 returns immediately for stream1A8.233410 passes default -1
        // volume/angle/pitch to269650, NOT source spatial-factor producers.
        NativeRaceIntroTransport50LA::Playback next;
        const auto result=taIntroTransport_->play(eventId,next);
        if(result==NativeRaceIntroTransport50LA::Dispatch::unavailable)return TaIntroDispatchResult::unavailable;
        if(result==NativeRaceIntroTransport50LA::Dispatch::sourceFalse){
            sound.channel2C=taIntroTransport_->lastSourceResult(); //2334D0 before sign test.
            return TaIntroDispatchResult::sourceFalse;
        }
        taIntroLatest_[n]=next;
        sound.channel2C=static_cast<std::int32_t>(next.serial);
        sound.elapsedBits24=0;sound.word7C=1;
        return TaIntroDispatchResult::sourceTrue;
    }
    return TaIntroDispatchResult::sourceFalse;
}
inline bool RaceAudioGroups50IC::stopTaIntro2924A0()noexcept {
    if(!taIntroReady())return false;
    auto& emitter=*coldTaIntroEmitter_;
    //2924A0 scans the real wrapper list in order;1F0 survives this operation.
    for(std::size_t n=0;n<emitter.count200;++n){auto& wrapper=*emitter.members204[n];
        if(!wrapper.sound04||!*wrapper.sound04)continue;
        auto& sound=**wrapper.sound04;
        if(sound.channel2C==-1)continue; //233670 no writes in this arm.
        if(sound.channel2C<0){
            //Backend-issued269650 errors (-100/-107) reach26C180, NOT a
            //retail signed-negative early-out. Their decoded generationFFFF
            //cannot match the high16 of finite nonnegative source elapsed.
            //233710 is false;233670 writes only2C and never stops the older
            //native playback retained before this failed allocation.
            sound.channel2C=-1;continue;
        }
        bool active=false;
        if(!taIntroTransport_->stop(taIntroLatest_[n],active))return false;
        sound.channel2C=-1;
        if(active){sound.elapsedBits24=0;sound.word7C=0;}
        //Expired source reservation: only2C changes. PCM EOF is not this test.
    }
    return true;
}
inline bool RaceAudioGroups50IC::serviceTaIntro235720(
    const original_ui_source_timing50LH::PublicationView& publication)noexcept {
    if(!taIntroReady())return false;
    auto& group=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    if(group.entity.firstChild18||group.entity.descendants24)return false;
    float factor=0;
    if(!taIntroTransport_->service(publication,factor))return false;
    for(std::size_t n=0;n<group.count48;++n){auto& sound=*group.logicalMembers4C[n];
        //233E00 tests signed channel, not decoded PCM activity.
        if(sound.channel2C>=0){volatile float next=std::bit_cast<float>(sound.elapsedBits24)+factor;
            sound.elapsedBits24=std::bit_cast<std::uint32_t>(static_cast<float>(next));}
    }
    //235790 calls inherited141ED0 even for cold/expired Sounds. Its actual
    //group Entity has no children, so the reached loop has no dispatches.
    return !group.entity.firstChild18&&!group.entity.descendants24;
}
inline bool RaceAudioGroups50IC::retireTaIntro142590()noexcept {
    if(coldTaIntroEmitter_&&taIntroLive_&&!stopTaIntro2924A0())return false;
    //Drain every native overlapping instance, not merely the three latest
    //source handles. This is transport memory safety, not retail2924A0.
    if(taIntroTransport_&&!taIntroTransport_->drain())return false;
    if(coldTaIntroEmitter_){
        if(!retireNeverStartedTaIntro142590())return false;
    }else{
        // Retry a retained2353D0 temporary-allocation failure after292220.
        const auto& group=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
        if(group.taIntroAuthoring&&!clearColdTaIntroSounds2353D0())return false;
    }
    if(taIntroTransport_&&!taIntroTransport_->release())return false;
    taIntroTransport_.reset();taIntroLive_=false;return true;
}
}
