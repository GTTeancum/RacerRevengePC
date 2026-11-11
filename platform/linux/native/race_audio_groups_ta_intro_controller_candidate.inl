#pragma once
namespace rr::platform {
inline bool RaceAudioGroups50IC::taIntroExistingBindingsCurrent()const noexcept {
    if(!taIntroConstructionAdmission_||!mixer_||coldMixer_!=mixer_||coldEpoch_!=epoch_||
       mixer_->groupEpoch50IC()!=epoch_||mixer_->count()!=count_||
       !mixer_->requiresVoiceGroupGains50IC()||!taIntroVoiceCount_||
       taIntroFirstVoice_>=count_||taIntroVoiceCount_>count_-taIntroFirstVoice_)return false;
    for(std::size_t slot=0;slot<count_;++slot){
        AudioVoiceObservation50HD v;
        if(!mixer_->inspectVoice(slot,v)||v.voice.active)return false;
        const auto group=bindings_[slot];
        if(represented(group)){
            if(!v.groupKnown||v.groupGain!=normalized(groups_,group)||
               v.groupPaused!=(groups_.state44[static_cast<std::size_t>(group)]!=0))return false;
        }else if(group!=game::RaceAudioGroup50IC::Unrepresented||v.groupKnown)return false;
        if(slot>=taIntroFirstVoice_&&slot-taIntroFirstVoice_<taIntroVoiceCount_&&
           (v.groupKnown||v.playbackRevision||group!=game::RaceAudioGroup50IC::Unrepresented))return false;
    }
    return true;
}
inline RaceAudioGroups50IC::TaIntroConstructionResult
RaceAudioGroups50IC::constructTaIntroEvents177750(
    std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate> authoring)noexcept {
    if(!authoring||!authoring->eventCount()||authoring->eventCount()>3||(authoring->trackId()!="TA"&&authoring->trackId()!="RA"&&authoring->trackId()!="GC"&&authoring->trackId()!="SA"&&authoring->trackId()!="MA"&&authoring->trackId()!="GA"&&authoring->trackId()!="MB"&&authoring->trackId()!="SC"&&authoring->trackId()!="GB"))return TaIntroConstructionResult::rejected;
    const auto events=authoring->eventCount();
    if(!coldTaIntroEmitter_){
        if(!constructDetachedColdTaIntroEmitter177750(std::move(authoring)))
            return TaIntroConstructionResult::rejected;
    }else{
        const auto& group=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
        if(group.taIntroAuthoring!=authoring||!coldTaIntroEmitterIdentity()||
           coldTaIntroEmitter_->everAttached||coldTaIntroEmitter_->nativeAppendAborted)
            return TaIntroConstructionResult::rejected;
    }
    // A retained allocation prefix may continue. A completed call is not replayed.
    if(coldTaIntroEmitter_->count200==events)return TaIntroConstructionResult::rejected;
    while(coldTaIntroEmitter_->count200<events){
        const auto result=appendNextColdTaIntroEvent292650();
        if(result==ColdTaEmitterAppendResult::allocationPending)
            return TaIntroConstructionResult::allocationPending;
        if(result==ColdTaEmitterAppendResult::ownerRejected)
            return TaIntroConstructionResult::rejected;
    }
    return TaIntroConstructionResult::eventsComplete;
}
inline bool RaceAudioGroups50IC::attachTaIntro141960(
    game::OriginalWreckComponent50LA& controller)noexcept {
    if(!coldTaIntroEmitterIdentity()||coldTaIntroEmitter_->count200!=coldTaIntroEmitter_->group30->taIntroAuthoring->eventCount()||
       coldTaIntroEmitter_->pendingEvent||coldTaIntroEmitter_->everAttached||
       controller.kind!=game::OriginalWreckComponentKind50LA::cameraController174A30||
       controller.sessionGeneration!=coldSession_||controller.reference00!=1||
       !coldApplication_->entityAuthority_.canRetire(controller))return false;
    auto& emitter=*coldTaIntroEmitter_;
    // Caller has already published/initialized controller5C and read script.
    // Actual generic-child splice; no camera registry/selected34 insertion.
    const auto result=coldApplication_->entityAuthority_.attachChildren(controller,
        emitter.entity,0xffffffffu,coldApplication_->attachmentLock423F58());
    if(result.status!=game::OriginalWreckComponentStatus50LA::complete)return false;
    emitter.controllerParent=&controller;emitter.everAttached=true;return true;
}
inline bool RaceAudioGroups50IC::retireNeverStartedTaIntro142590()noexcept {
    if(!coldTaIntroEmitterIdentity())return false;
    auto& emitter=*coldTaIntroEmitter_;
    if(emitter.pendingEvent&&!abortPendingColdTaIntroEventNative())return false;
    // This exact leaf is childless and last in the surviving sibling chain.
    // Retire other generic sibling tails first; never silently sever them.
    if(emitter.entity.next14||emitter.entity.firstChild18)return false;
    if(emitter.controllerParent){
        if(coldApplication_->entityAuthority_.detach(emitter.entity,
            coldApplication_->attachmentLock423F58()).status!=
            game::OriginalWreckComponentStatus50LA::complete)return false;
        emitter.controllerParent=nullptr;
    }
    //104970/292220 retires wrapper storage only. Group-owned Sounds remain.
    if(!retireDetachedColdTaIntroEmitter142590())return false;
    return clearColdTaIntroSounds2353D0();
}
}
