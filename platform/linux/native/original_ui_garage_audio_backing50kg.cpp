#include "original_ui_garage_audio_backing50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_audio_pending_waves50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <exception>
#include <limits>

namespace {
template<class T> bool sameGarageShared(const std::shared_ptr<T>& a,
    const std::shared_ptr<T>& b)noexcept {
    return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
bool garageBackingFailure(std::string& error,const char* text)noexcept {
    try{error=text;}catch(...){error.clear();}return false;
}
}
namespace rr::platform {
OriginalUiGarageAudioBacking50KG::OriginalUiGarageAudioBacking50KG(
    const Application& application)noexcept
    :issuer_(&application),issuerLifetime_(application.animationClock()),
     authority_(application.entities()),soundMode_(application.soundMode){}
OriginalUiGarageAudioBacking50KG::~OriginalUiGarageAudioBacking50KG()noexcept {
    // Refusal, never implicit failure rollback/source cleanup. The sole root
    // owner must explicitly retire, after all aliasing consumers are gone.
    if(published_&&!closed_)std::terminate();
}
bool OriginalUiGarageAudioBacking50KG::sameRoot(const Application& application)const noexcept {
    // Thread before mutable clock/root reads; lifetime before issuer dereference.
    if(ownerThread_!=std::this_thread::get_id()||issuer_!=&application||!published_||closed_)return false;
    std::uint32_t pause=0;
    return issuerLifetime_.readPause11C(pause)&&
        issuerLifetime_.sameOwner(application.animationClock())&&
        sameGarageShared(authority_,application.entities());
}
bool OriginalUiGarageAudioBacking50KG::inputsCurrent(const Application& application)const noexcept {
    return sameRoot(application)&&authority_->application10()&&
        assetRoot_==application.assetRoot&&audioRoot_==application.audioRoot&&
        sameGarageShared(soundMode_,application.soundMode)&&soundMode_->ready()&&
        application.materialApplication&&
        sameGarageShared(appStart_,application.materialApplication->appStart())&&
        metadata_&&metadata_->prepared();
}
}
namespace original_main_entry50KG {
bool Application::prepareFrontendAudioOutput(Frontend& frontend,
    const rr::platform::AudioOutputRequest50HD& request,std::string& error,
    rr::platform::AudioOutputDiagnostic50HD& diagnostic)const {
    auto& output=frontend.audioOutput;
    using Main=rr::platform::OriginalUiMainAudio50KG;
    using Aural=rr::platform::OriginalApplicationAural50KG;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||frontendAudioShutdownComplete_||
       frontend.materialApplicationOwner!=this||
       !garageAudioApplicationInputsCurrent()||output.attempted_||output.closed_||output.typedBound_||
       output.legacyAdvanced_||output.transport_||output.sourceMixer_!=&output.mixer_||
       !output.retainedBacking_.expired()||output.acceptedNanoseconds_||output.remainderNanoseconds_||
       output.sourceFrameOrigin_||output.retainedOutputSerial_||output.output_.info().sampleRate)
        return garageBackingFailure(error,"Frontend output requires its genuine fresh owner and current Root inputs");
    if(!garageAudioBackingOwner_&&!garageAudioBackingAttempted_&&!sourceAural00Owner_&&
       !sourceLightManager426F60Owner_&&sameGarageShared(lightManager,lightManagerStartupWitness_)&&
       lightManager->cacheState()==rr::game::OriginalUiLightCacheState50KG::absentSinceApplicationConstruction&&
       !lightManager->managerGeneration()&&!lightManager->activeUiIdentity()&&!lightManager->nativeGameplayGeneration()&&
       Aural::firstStartupInputsCurrent(*this)){
        // Actual first frontend preparation, before coordinator.prepare/activate
        // can enter Main lighting. Reuse the source-owned Aural -> backing ->
        // native interface -> Light -> attach chain, never replay any constructor.
        Aural::Diagnostic prefix;
        if(!stageAuralLightNativePrefix13FE24(request.sampleRate,prefix,error)){
            error="Aural/Light startup rejected: status="+
                std::to_string(static_cast<unsigned>(prefix.status))+" sourcePC="+
                std::to_string(prefix.sourcePC)+(error.empty()?std::string{}:" detail="+error);
            return false;
        }
        // Actual17CD84 invokes232EA0 on the initialized Aural interface,
        // before fonts328/dispatcher330 and every later Garage consumer.
        // Publish the existing authenticated APPSTART directory once at this
        // real first Root startup, never lazily from retain98A4 or a manager.
        std::shared_ptr<rr::platform::OriginalAudioPendingWaves50KG> pending;
        if(!preparePendingWaves17CD84(pending,error))return false;
    }
    const auto& backing=garageAudioBackingOwner_;
    if(!backing){
        if(garageAudioBackingAttempted_||sourceAural00Owner_||frontendAudioShutdownComplete_)
            return garageBackingFailure(error,"Frontend output cannot bypass an attempted or retired root audio startup prefix");
        // Already-used native roots without the source startup prefix retain
        // their previous ordinary8 behavior. No lazy Aural/Light reconstruction.
        return output.prepare(audioRoot,request,error,diagnostic,soundMode);
    }
    // A later retained output may only borrow that same live publication.
    // Missing/failed/retired startup cannot be repaired by output reattachment.
    std::shared_ptr<rr::platform::OriginalAudioPendingWaves50KG> pending;
    if(!retainPendingWaves98A4(pending,error))return false;
    if(!backing->inputsCurrent(*this)||backing->failed_||
       backing->stage_!=rr::platform::OriginalUiGarageAudioBacking50KG::Stage::transportPrepared||
       backing->frontendOutputOwner_||backing->frontendOutputCloseFailed_||backing->frontendOutputAccountingFailed_||
       backing->frontendOutputSerial_==std::numeric_limits<std::uint64_t>::max()||garageCustodyOwner_||
       !sourceAural00Owner_||!sourceAural00Owner_->attached_||!sourceAural00Owner_->nativeInitialized_||
       !sourceAural00Owner_->current()||!sourceAural00Owner_->backendCurrent()||
       !sourceAural00Owner_->nativeOutputSourceQuiescent()||backing.use_count()!=2||
       backing->nativeOutputRate_!=request.sampleRate)
        return garageBackingFailure(error,"Frontend output requires its exact quiescent Root/Aural backing with no outstanding consumer aliases");
    auto& main=backing->transport_;
    if(backing->frontendOutputAttempted_){
        std::uint32_t pause=0;
        if(backing->frontendOutputAuralEntity_!=&sourceAural00Owner_->entity_||
           !frontendAudioClosedReceiptCurrent()||!animationClock().readPause11C(pause)||
           !main.rebindOrdinaryFrontendOutput(pause))
            return garageBackingFailure(error,"Frontend output rebind requires the unchanged genuine device-close history");
    }else{
        if(backing->mixer_.mixedFrames()||main.garageSourceLifetime_!=Main::GarageSourceLifetime::initialPrefix||
           !main.canRetireOrdinaryFrontendOutput())
            return garageBackingFailure(error,"First retained output cannot adopt an already-used Main transport");
        for(std::size_t ordinal=0;ordinal<Main::garageOrdinaryVoiceCount;++ordinal){
            const auto slot=Main::garageOrdinaryVoice(ordinal);
            rr::platform::AudioVoiceObservation50HD v;
            if(!backing->mixer_.inspectVoice(slot,v)||v.voice.active||v.playbackRevision)
                return garageBackingFailure(error,"First retained output cannot adopt an already-used ordinary voice");
        }
    }
    // Pin the SAME transport. At this actual new device-open boundary seal the
    // cumulative mixer origin; no process-wide clock/history or PCM is reset.
    output.transport_=std::shared_ptr<Main>(backing,&main);
    output.sourceMixer_=&backing->mixer_;output.retainedBacking_=backing;
    output.sourceFrameOrigin_=backing->mixer_.mixedFrames();
    output.retainedOutputSerial_=++backing->frontendOutputSerial_;
    output.attempted_=true;backing->frontendOutputAttempted_=true;backing->frontendOutputOwner_=&output;
    backing->frontendOutputClosed_=false;backing->frontendOutputAuralEntity_=&sourceAural00Owner_->entity_;
    if(!output.output_.open(request,diagnostic))
        return garageBackingFailure(error,"Retained Main output open failed; exact constructor prefix remains owned");
    output.enabled_=true;error.clear();return true;
}
bool Application::prepareFrontendMenuAudio(Frontend& frontend,
    const rr::platform::OriginalUiFrontendCold50KG& cold,std::string& error)const noexcept {
    const auto& main=cold.audio;const auto& output=frontend.audioOutput;
    if(!main||main!=output.transport()||frontend.materialApplicationOwner!=this)
        return garageBackingFailure(error,"Frontend menu PCM requires its actual output transport");
    const auto& backing=garageAudioBackingOwner_;
    const bool retained=bool(backing);
    if(retained&&(!backing->inputsCurrent(*this)||backing->frontendOutputOwner_!=&output||
       output.retainedOutputSerial_!=backing->frontendOutputSerial_||!output.enabled_||
       main.get()!=&backing->transport_||main.owner_before(backing)||backing.owner_before(main)||
       output.sourceMixedFrames()!=output.sourceFrameOrigin_||output.acceptedNanoseconds_||
       output.remainderNanoseconds_||output.output_.info().mixedFrames||!main->current()))
        return garageBackingFailure(error,"Retained menu PCM reuse requires the exact fresh output borrow and authentic source transport");
    // These are authenticated PCM resources, not per-Frontend constructors.
    // Reuse only genuinely prepared/valid same-Root storage; failed attempts
    // still fail and missing resources retain their existing cold-only guards.
    if((decodeRiderImagesInProcess||!riderCacheRoot.empty())&&
       !(retained&&main->riderPrepared_?main->riderMenuReady():main->prepareRiderMenu(audioRoot,error)))return false;
    if((cold.course||cold.tournament)&&
       !(retained&&main->coursePrepared_?main->courseMenuReady():main->prepareCourseMenu(audioRoot,error)))return false;
    if(cold.options&&
       !(retained&&main->creditsPrepared_?main->creditsMenuReady():main->prepareCreditsMenu(audioRoot,error)))return false;
    if(cold.hall&&
       !(retained&&main->hallOfFamePrepared_?main->hallOfFameMenuReady():main->prepareHallOfFameMenu(audioRoot,error)))return false;
    // Optional UIWin Song10/Song8. A retained prepared copy is reused as is;
    // readiness (which also refuses a wet-bound transport) is checked by Win.
    if(cold.win&&!(retained&&main->winPrepared_)&&!main->prepareWinMenu(audioRoot,error))return false;
    error.clear();return true;
}
bool Application::frontendAudioClosedReceiptCurrent()const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto& b=garageAudioBackingOwner_;
    if(!b||!b->inputsCurrent(*this)||b->failed_||!b->frontendOutputAttempted_||!b->frontendOutputClosed_||
       b->frontendOutputOwner_||b->frontendOutputCloseFailed_||!b->frontendOutputSerial_||
       !b->transport_.retiredOrdinaryFrontendOutputCurrent()||
       b->mixer_.mixedFrames()!=b->frontendClosedSourceFrames_)return false;
    const bool resetDescendant=b->frontendClosedReset268300MainRevision_||
        b->frontendClosedReset268300MixerRevision_;
    if(resetDescendant&&
       (b->frontendClosedReset268300MainRevision_<=b->frontendClosedMainRevision_||
        b->frontendClosedReset268300MixerRevision_<=b->frontendClosedMixerRevision_))return false;
    // Only same-Aural268300 can publish these two descendant identities,
    // after validating this exact prior close/current descendant. Staging or
    // unrelated control edits cannot make the historical close current.
    const auto priorMain=resetDescendant?b->frontendClosedReset268300MainRevision_:b->frontendClosedMainRevision_;
    const auto priorMixer=resetDescendant?b->frontendClosedReset268300MixerRevision_:b->frontendClosedMixerRevision_;
    const bool stopDescendant=b->frontendClosedStop26D070MainRevision_||b->frontendClosedStop26D070MixerRevision_;
    if(stopDescendant&&(b->frontendClosedStop26D070MainRevision_<=priorMain||
        b->frontendClosedStop26D070MixerRevision_<=priorMixer))return false;
    if(b->transport_.controlRevision_!=(stopDescendant?b->frontendClosedStop26D070MainRevision_:priorMain)||
       b->mixer_.controlRevision()!=(stopDescendant?b->frontendClosedStop26D070MixerRevision_:priorMixer))return false;
    for(std::size_t ordinal=0;ordinal<rr::platform::OriginalUiMainAudio50KG::garageOrdinaryVoiceCount;++ordinal){
        const auto slot=rr::platform::OriginalUiMainAudio50KG::garageOrdinaryVoice(ordinal);
        rr::platform::AudioVoiceObservation50HD v;
        if(!b->mixer_.inspectVoice(slot,v)||v.voice.active||
           v.playbackRevision!=b->frontendClosedPlaybackRevisions_[slot]||
           v.voice.sourceFrame!=b->frontendClosedSourceFramesByVoice_[slot])return false;
    }
    return true;
}
bool AudioOutput::closeRetained(rr::platform::AudioOutputDiagnostic50HD& diagnostic)noexcept {
    const auto backing=retainedBacking_.lock();std::uint32_t pause=0;
    // Read actual root lifetime before its raw Application pointer.
    if(!backing||!backing->issuer_||!backing->issuerLifetime_.readPause11C(pause)){
        diagnostic={};diagnostic.error=rr::platform::AudioOutputError50HD::Clock;
        diagnostic.operation="Retained frontend output root lifetime";return false;
    }
    return backing->issuer_->closeFrontendAudioOutput(*this,backing,diagnostic);
}
bool Application::closeFrontendAudioOutput(AudioOutput& output,
    const std::shared_ptr<rr::platform::OriginalUiGarageAudioBacking50KG>& closeBorrow,
    rr::platform::AudioOutputDiagnostic50HD& diagnostic)const noexcept {
    using Error=rr::platform::AudioOutputError50HD;
    auto reject=[&](const char* why)noexcept {diagnostic={};diagnostic.error=Error::Clock;diagnostic.operation=why;return false;};
    if(output.retainedCloseFailed_){diagnostic=output.retainedCloseDiagnostic_;return false;}
    const auto& backing=garageAudioBackingOwner_;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!backing||!backing->inputsCurrent(*this)||backing->failed_||
       !backing->frontendOutputAttempted_||backing->frontendOutputOwner_!=&output||
       output.retainedOutputSerial_!=backing->frontendOutputSerial_||!output.retainedOutputSerial_||
       backing->frontendOutputClosed_||backing->frontendOutputCloseFailed_||garageCustodyOwner_||
       !sameGarageShared(closeBorrow,backing)||!output.transport_||output.transport_.get()!=&backing->transport_||
       output.transport_.owner_before(backing)||backing.owner_before(output.transport_)||
       output.sourceMixer_!=&backing->mixer_||!sourceAural00Owner_||
       backing->frontendOutputAuralEntity_!=&sourceAural00Owner_->entity_||
       (!sourceAural00Owner_->current()&&!sourceAural00Owner_->listenerTerminalCleanupCurrent())||
       !sourceAural00Owner_->backendCurrent()||
       !backing->transport_.canRetireOrdinaryFrontendOutput())
        return reject("Retained output close requires exact idle source owners, Aural, epoch and ordinary revisions");
    // Exactly Root, output's Main alias, actual Aural interface Main alias,
    // and closeRetained's pinned closeBorrow. Any coordinator/consumer alias
    // requires its genuine release before this native output can stop voices.
    if(backing.use_count()!=4)
        return reject("Retained output close still has an external Main/source consumer alias");
    // The actual Aural owner proves its lists/registry; retain the exact
    // Pending namespace, not an invented empty namespace or readiness bit.
    if(!sourceAural00Owner_->nativeOutputSourceQuiescent())
        return reject("Retained output close still has registered source audio owners");
    if(const auto pending=sourceAural00Owner_->nativeOutputPendingNamespace()){
        if(pending->issuer_!=this||!pending->current())return reject("Retained output close has a changed Pending namespace");
        for(const auto& borrow:pending->groupBorrows_)if(!borrow.empty())
            return reject("Retained output close still has a source Pending group borrow");
        if(!pending->nativeOutputRegistrationRowsQuiescent())
            return reject("Retained output close still has queued source sample registrations");
    }
    const auto outputInfo=output.output_.info();
    if(backing->mixer_.mixedFrames()<output.sourceFrameOrigin_)
        return reject("Retained output has lost its exact cumulative mixer origin");
    if(output.enabled_){
        if(outputInfo.sampleRate!=backing->nativeOutputRate_)
            return reject("Retained output close has a changed native sample clock");
        if(!output.advancementFault_){
            pcm_drain_detail50KG::Plan noAdvance;
            const pcm_drain_detail50KG::Counters counters{outputInfo.sampleRate,outputInfo.simulationMicroseconds,
                outputInfo.mixedFrames,backing->mixer_.mixedFrames()-output.sourceFrameOrigin_,outputInfo.capturedFrames,outputInfo.captureActive};
            if(pcm_drain_detail50KG::prepare(0,output.acceptedNanoseconds_,output.remainderNanoseconds_,counters,noAdvance)!=
               pcm_drain_detail50KG::Error::none)
                return reject("Retained output close has unaccounted source/output clocks");
        }
    }else if(backing->mixer_.mixedFrames()!=output.sourceFrameOrigin_||output.acceptedNanoseconds_||output.remainderNanoseconds_||outputInfo.mixedFrames)
        return reject("Retained output constructor failure has unexpected source/output advancement");
    // A faulted irreversible PCM attempt may still close the device. This is
    // only a shutdown receipt; leave advancementFault and failed-prefix drain
    // observations unchanged, never turn partial output into accounted time.
    // This call really joins the callback/finalizes capture. A failed close is
    // terminal even though low-level state may already have been deleted; a
    // second harmless close must never launder its failure into a receipt.
    if(!output.output_.close(diagnostic)){
        output.retainedCloseFailed_=true;output.retainedCloseDiagnostic_=diagnostic;
        backing->frontendOutputCloseFailed_=true;return false;
    }
    auto& main=backing->transport_;
    // The callback only consumed copied PCM; it cannot mutate Main or mixer.
    // Exact preflight preceded device close. Preserve clip/cursor/PCM storage.
    if(!main.retireOrdinaryFrontendOutputAfterDeviceClose())std::terminate();
    backing->frontendClosedSourceFrames_=backing->mixer_.mixedFrames();
    backing->frontendClosedMainRevision_=main.controlRevision_;
    backing->frontendClosedMixerRevision_=backing->mixer_.controlRevision();
    backing->frontendClosedReset268300MainRevision_=0;
    backing->frontendClosedReset268300MixerRevision_=0;
    backing->frontendClosedStop26D070MainRevision_=0;
    backing->frontendClosedStop26D070MixerRevision_=0;
    for(std::size_t ordinal=0;ordinal<rr::platform::OriginalUiMainAudio50KG::garageOrdinaryVoiceCount;++ordinal){
        const auto slot=rr::platform::OriginalUiMainAudio50KG::garageOrdinaryVoice(ordinal);
        rr::platform::AudioVoiceObservation50HD v;
        if(!backing->mixer_.inspectVoice(slot,v)||v.voice.active)std::terminate();
        backing->frontendClosedPlaybackRevisions_[slot]=v.playbackRevision;
        backing->frontendClosedSourceFramesByVoice_[slot]=v.voice.sourceFrame;
    }
    backing->frontendOutputAccountingFailed_=backing->frontendOutputAccountingFailed_||output.advancementFault_;
    backing->frontendOutputOwner_=nullptr;backing->frontendOutputClosed_=true;
    output.closedSourceMixedFrames_=backing->frontendClosedSourceFrames_;
    output.sourceMixer_=nullptr;output.transport_.reset();output.retainedBacking_.reset();
    output.enabled_=false;output.closed_=true;
    // Aural/cache/Main PCM remain genuinely retained until their own source
    // releases. Device close itself neither invokes102CD0 nor reclaims backing.
    diagnostic={};return true;
}
bool Application::retireFrontendAudioBackingClosed(std::string& error)const noexcept {
    const auto& backing=garageAudioBackingOwner_;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!backing||!backing->inputsCurrent(*this)||
       backing->failed_||!backing->frontendOutputAttempted_||!backing->frontendOutputClosed_||
       backing->frontendOutputOwner_||backing->frontendOutputCloseFailed_||backing.use_count()!=1||
       garageCustodyOwner_||!rr::platform::OriginalApplicationAural50KG::nativeOutputSourceRetired(
           *this,backing->frontendOutputAuralEntity_))
        return garageBackingFailure(error,"Used frontend backing needs its real device-close receipt, source Aural retirement and no remaining native aliases");
    // Application00 deliberately retains a stale identity after102CD0. Compare
    // it only with the captured receipt; never dereference that released Entity.
    auto& main=backing->transport_;
    if(!frontendAudioClosedReceiptCurrent())
        return garageBackingFailure(error,"Used frontend backing changed after its exact native ordinary shutdown");
    // This is final native storage destruction, not a source destructor or
    // transport restart. All output/source/native consumers already released.
    main.reset();backing->closed_=true;
    backing->stage_=rr::platform::OriginalUiGarageAudioBacking50KG::Stage::closed;
    garageAudioBackingOwner_.reset();error.clear();return true;
}
bool Application::finishNativeFrontendAudio(std::string& error)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||garageCustodyOwner_||sourceRace28COwner_||
       !raceDisplayOwner50LA_.expired()||(state&&state->sharedDialogs)||
       (sharedUiScopeOwner_&&!sharedUiScopeOwner_->closed_)||
       (lightManager&&(lightManager->activeUiIdentity()||lightManager->nativeGameplayGeneration())))
        return garageBackingFailure(error,"Terminal Root services require actual UI/race/dispatcher and Garage owner closure");
    // Terminal failure/quit can abandon an already-consumed Hall transfer,
    // but never while a live Race/input/renderer owner remains above.
    if(!cancelNativeResultsHallTransfer50LA())
        return garageBackingFailure(error,"Terminal Results Hall request retirement incomplete");
    const auto& backing=garageAudioBackingOwner_;
    if(frontendAudioShutdownComplete_){
        if(backing||sourceAural00Owner_||sourceLightManager426F60Owner_||
           sourceLocalization32COwner_||localizationRendererCall14C5A0_)
            return garageBackingFailure(error,"Retired Root services cannot reacquire a source prefix");
        error.clear();return true;
    }
    if(backing&&backing->frontendOutputAttempted_&&!frontendAudioClosedReceiptCurrent())
        return garageBackingFailure(error,"Terminal Root services require the real unchanged device-close receipt");
    if(sourceLightManager426F60Owner_&&!sourceLightManager426F60Owner_->sourceMutationAvailable())
        return garageBackingFailure(error,"Terminal Root services still have an ordinary/gameplay Light projection");
    // The actual caller is after the terminal session stack, never the Course
    // handoff. Release later source siblings first: font/text leases and the
    // selected localization, then Pending's real cache/strong-sample owners.
    // Supplied by the coordinated native language-services production join.
    if(!retireFrontendLanguageServices17C8A4(error))return false;
    // Loading330 is a later Root sibling than Aural. Its real camera/list
    // aliases must retire before102CD0, whose tail precondition is deliberate.
    // Waiting for Application's destructor leaves Aural blocked by next14.
    if(!retireNativeLoading330Prefix())
        return garageBackingFailure(error,"Terminal loading-manager330 prefix still has live borrowers");
    if(sourceAural00Owner_){
        if(const auto pending=sourceAural00Owner_->nativeOutputPendingNamespace())
            if(!retirePendingWaves2668E0(pending,error))return false;
        rr::platform::OriginalApplicationAural50KG::Diagnostic aural;
        if(!releaseAuralCold102CD0(aural))
            return garageBackingFailure(error,"Terminal Aural102CD0 still owns a real emitter/listener/group/backend prefix");
    }
    if(sourceLightManager426F60Owner_){
        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic light;
        if(!releaseLightManagerCold148020(*sourceLightManager426F60Owner_,light))
            return garageBackingFailure(error,"Terminal Light148020 still owns a real zone/registry/entity prefix");
    }
    // Aural has genuinely dropped its Main alias and master borrower. Only
    // now may Root reclaim native PCM/backing, using the saved close receipt.
    if(backing&&!retireGarageAudioBackingCold(error))return false;
    frontendAudioShutdownComplete_=true;error.clear();return true;
}
bool Application::garageAudioApplicationInputsCurrent()const noexcept {
    // Private root predicate: validate its initialized state/profile and exact
    // clock backend without exposing that backend or accepting a ready flag.
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    std::uint32_t pause=0;
    return animationClock().readPause11C(pause)&&entitiesOwner_&&entitiesOwner_->application10()&&
        state&&state->known&&sameGarageShared(state->clock101A50,animationClockBackend_)&&
        profile&&profile->known&&gameSettingsProfile&&gameSettingsProfile->known&&
        rr::game::validOriginalUiGameSettingsProfile50KG(*gameSettingsProfile)&&
        soundMode&&soundMode->ready()&&
        static_cast<std::uint32_t>(soundMode->fields().profile1101C)==gameSettingsProfile->soundMode1101C;
}
bool Application::prepareGarageAudioBackingCold(std::uint32_t nativeOutputRate,
    std::string& error)const noexcept {
    using Backing=rr::platform::OriginalUiGarageAudioBacking50KG;
    // This immutable native sidecar is born with THIS Application, not with
    // whichever caller first attempts audio. It is not an original source field.
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return garageBackingFailure(error,"Garage cold backing requires actual Application creator thread");
    if(!garageAudioApplicationInputsCurrent())
        return garageBackingFailure(error,"Garage cold backing requires actual initialized Application/state/profile/clock and consistent sound mode");
    if(garageAudioBackingAttempted_||garageAudioBackingOwner_)
        return garageBackingFailure(error,"Garage cold backing is one native attempt; retain or retire without replay");
    try {
        // Allocation precedes publication; every later failure keeps this exact
        // attempted backing rooted. No partially prepared Main escapes alone.
        auto next=std::shared_ptr<Backing>(new Backing(*this));
        garageAudioBackingOwner_=std::move(next);garageAudioBackingAttempted_=true;
        auto& backing=*garageAudioBackingOwner_;backing.published_=true;
        backing.assetRoot_=assetRoot;backing.audioRoot_=audioRoot;
        if(!backing.mixer_.initialize(backing.voices_.data(),backing.voices_.size(),nativeOutputRate)){
            backing.failed_=true;
            return garageBackingFailure(error,"Garage native cold mixer configuration rejected; attempted backing retained");
        }
        backing.mixerEpoch_=backing.mixer_.groupEpoch50IC();
        backing.nativeOutputRate_=backing.mixer_.sampleRate();
        backing.stage_=Backing::Stage::mixerInitialized;
        if(!backing.mixer_.bindOriginalSoundMode50KG(backing.soundMode_)){
            backing.failed_=true;
            return garageBackingFailure(error,"Garage actual sound-mode binding rejected; attempted backing retained");
        }
        backing.stage_=Backing::Stage::soundModeBound;
        backing.appStart_=appStart(assetRoot);
        auto metadata=std::make_shared<rr::assets::OriginalStreamMetadata50KZ>();
        rr::assets::StreamMetadataDiagnostic50KZ diagnostic;
        if(!backing.appStart_||!metadata->prepare(*backing.appStart_,diagnostic)){
            backing.failed_=true;
            return garageBackingFailure(error,"Garage actual APPSTART stream metadata rejected; attempted backing retained");
        }
        backing.metadata_=std::move(metadata);backing.stage_=Backing::Stage::metadataPrepared;
        // Existing Application friendship is the only cold Main aperture.
        // No eager row population and no actor/model/bounds completion is made.
        if(!backing.transport_.prepareGarageSceneColdTransport(audioRoot,
                backing.metadata_,backing.mixer_,error)){
            backing.failed_=true;return false;
        }
        backing.stage_=Backing::Stage::transportPrepared;
        // Only this actual Application allocation can issue persistent Main
        // ownership. Never adopt another live mixer or the ordinary8 output.
        if(!backing.transport_.bindRetainedGarageTransport(garageAudioBackingOwner_,error)){
            backing.failed_=true;return false;
        }
        error.clear();return true;
    }catch(...){
        if(garageAudioBackingOwner_)garageAudioBackingOwner_->failed_=true;
        return garageBackingFailure(error,"Garage cold backing preparation failed; any published prefix remains rooted");
    }
}
bool Application::retainGarageAudioTransportCold(
    std::shared_ptr<rr::platform::OriginalUiMainAudio50KG>& out,std::string& error)const noexcept {
    using Backing=rr::platform::OriginalUiGarageAudioBacking50KG;
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return garageBackingFailure(error,"Garage cold transport retain requires actual Application creator thread");
    const auto& backing=garageAudioBackingOwner_;
    if(!garageAudioApplicationInputsCurrent()||!backing||!backing->inputsCurrent(*this)||backing->failed_||
        (backing->stage_!=Backing::Stage::transportPrepared&&backing->stage_!=Backing::Stage::transportFinished)||
        backing->transport_.mixer_!=&backing->mixer_||!backing->transport_.prepared_||
        backing->mixer_.count()!=rr::platform::OriginalUiMainAudio50KG::garageSceneRequiredVoices||
        backing->transport_.mixerEpoch_!=backing->mixer_.groupEpoch50IC()||
        backing->transport_.mixerRate_!=backing->mixer_.sampleRate()||
        backing->mixer_.originalSoundMode50KG()!=backing->soundMode_.get())
        return garageBackingFailure(error,"Garage cold transport is not this root's retained exact native backing");
    // No independent Main allocation. Existing shared Main consumers now pin
    // all raw pointer dependencies. A failed retain preserves caller output.
    out=std::shared_ptr<rr::platform::OriginalUiMainAudio50KG>(backing,&backing->transport_);
    error.clear();return true;
}
bool Application::finishGarageAudioBackingCold(std::string& error)const noexcept {
    using Backing=rr::platform::OriginalUiGarageAudioBacking50KG;
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return garageBackingFailure(error,"Garage cold finish requires actual Application creator thread");
    const auto& backing=garageAudioBackingOwner_;
    if(!garageAudioApplicationInputsCurrent()||!backing||!backing->inputsCurrent(*this)||backing->failed_||
        backing->stage_!=Backing::Stage::transportPrepared)
        return garageBackingFailure(error,"Garage cold finish requires this root's exact unreplayed transport");
    // Existing Main preflight waits for the REAL interleaved actor Sound rows.
    // An early refusal does not consume finish; a mutating failed attempt does.
    if(!backing->transport_.finishGarageSceneColdTransport(error)){
        if(backing->transport_.garageColdFinishAttempted_)backing->failed_=true;
        return false;
    }
    backing->stage_=Backing::Stage::transportFinished;error.clear();return true;
}
bool Application::rearmGarageAudioAfterManagerRetirementCold(
    const std::shared_ptr<rr::platform::OriginalUiGaragePageOwner50KG>& page,
    std::uint64_t generation,std::string& error)const noexcept {
    using Backing=rr::platform::OriginalUiGarageAudioBacking50KG;
    using Session=rr::platform::OriginalUiGarageSession50KG;
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return garageBackingFailure(error,"Garage audio reentry requires the original Application thread");
    const auto& custody=garageCustodyOwner_;const auto& backing=garageAudioBackingOwner_;
    // These identities are retained through real manager destruction, not
    // grant.current() (which correctly becomes false at manager1413E0).
    if(!garageAudioApplicationInputsCurrent()||!custody||!custody->sameRoot(*this)||
       custody->normalRetirementPhase_!=Session::NormalRetirementPhase::currentCleared||
       custody->normalManagerGeneration_!=generation||!sameGarageShared(custody->page_,page)||
       !sameGarageShared(custody->scope_,sharedUiScopeOwner_)||
       !backing||!backing->inputsCurrent(*this)||backing->failed_||
       (backing->stage_!=Backing::Stage::transportFinished&&backing->stage_!=Backing::Stage::transportPrepared)||
       !custody->audio_||custody->audio_.get()!=&backing->transport_||
       custody->audio_.owner_before(backing)||backing.owner_before(custody->audio_))
        return garageBackingFailure(error,"Garage audio reentry requires exact completed normal manager custody and retained Application backing");
    rr::assets::OriginalStandardMaterialDiagnostic50LA retired;
    if(!custody->normalSourceOwnersRetired(retired,error))return false;
    const auto& pending=custody->pending_;
    if(!pending||!pending->current()||pending->issuer_!=this||!pending->nativeSamples_||
       !pending->nativeSamples_->current())
        return garageBackingFailure(error,"Garage audio reentry requires its unchanged Application pending namespace and sample cache");
    for(const auto& borrow:pending->groupBorrows_)if(!borrow.empty())
        return garageBackingFailure(error,"Garage audio reentry refuses an outstanding source-group namespace borrow");
    for(const auto& row:pending->rows_)
        if(row.count8||row.hasRepresentative()||(row.resolvedNativeSample&&!row.resolvedNativeSample->current()))
            return garageBackingFailure(error,"Garage audio reentry refuses pending or stale sample registrations");
    // No per-manager268300 or26D960 source boundary was reached. Keep the
    // exact namespace/cache and positive handles; the next genuine2332A0
    // registration replaces only its own row, and later26D9F0 resolves it.
    auto& main=backing->transport_;
    const bool rearmed=main.garageSourceLifetime_==rr::platform::OriginalUiMainAudio50KG::GarageSourceLifetime::unbound?
        main.rearmGarageAfterManagerRetirementCold(page,generation,backing->metadata_,error):
        main.rearmGarageAfterManagerRetirementRetainingTransport(page,generation,backing->metadata_,error);
    if(!rearmed)return false;
    backing->stage_=Backing::Stage::transportPrepared;error.clear();return true;
}
bool Application::retireGarageAudioBackingCold(std::string& error)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return garageBackingFailure(error,"Garage cold retirement requires actual Application creator thread");
    const auto& backing=garageAudioBackingOwner_;
    if(!backing||!backing->sameRoot(*this))
        return garageBackingFailure(error,"Garage cold retirement requires this exact live Application backing");
    // A used root-backed output has a distinct real shutdown contract. Do not
    // route mixed history through cold-abort preconditions or silently reset it.
    if(backing->frontendOutputAttempted_)return retireFrontendAudioBackingClosed(error);
    // Existing actor/Watto RootInputs and source SoundGroup own Main aliases.
    // Even a failed/closed-but-retained consumer must release its native borrow
    // first. No source row is cleared/flushed, no consumer is forged retired.
    if(backing.use_count()!=1)
        return garageBackingFailure(error,"Garage cold backing still has external transport or consumer borrows");
    // A private future caller must not reset a reinitialized/replaced mixer
    // while old voice storage can still borrow PCM. Refuse BEFORE Main/Watto
    // reset; their independent deeper native-voice checks remain fail-stop.
    using Stage=rr::platform::OriginalUiGarageAudioBacking50KG::Stage;
    if(backing->stage_!=Stage::published&&
        (backing->mixer_.groupEpoch50IC()!=backing->mixerEpoch_||
         backing->mixer_.count()!=backing->voices_.size()||
         backing->mixer_.sampleRate()!=backing->nativeOutputRate_))
        return garageBackingFailure(error,"Garage cold retirement refuses changed native mixer epoch/count/rate");
    const auto& main=backing->transport_;
    if((main.prepared_&&!main.mixer_)||(main.mixer_&&
        (main.mixer_!=&backing->mixer_||main.mixerEpoch_!=backing->mixerEpoch_||
         main.mixerCount_!=backing->voices_.size()||main.mixerRate_!=backing->nativeOutputRate_||
         main.preparedSoundMode_!=backing->soundMode_.get())))
        return garageBackingFailure(error,"Garage cold retirement refuses mismatched published Main backing");
    // This is still a COLD-abort API, not the source shutdown of a running
    // shared frontend transport. Newly supported ordinary playback cannot be
    // silently stopped just because the failed Garage capsule dropped aliases.
    if(main.garageSourceLifetime_!=rr::platform::OriginalUiMainAudio50KG::GarageSourceLifetime::unbound){
        if(backing->mixer_.mixedFrames()||main.ownsMusic_||main.fading_||main.ownsRiderConfirm_)
            return garageBackingFailure(error,"Cold Garage backing retirement cannot consume a used retained ordinary transport");
        for(std::size_t ordinal=0;ordinal<rr::platform::OriginalUiMainAudio50KG::garageOrdinaryVoiceCount;++ordinal){
            const auto slot=rr::platform::OriginalUiMainAudio50KG::garageOrdinaryVoice(ordinal);
            rr::platform::AudioVoiceObservation50HD voice;
            if(!backing->mixer_.inspectVoice(slot,voice)||voice.voice.active||voice.playbackRevision)
                return garageBackingFailure(error,"Cold Garage backing retirement cannot erase ordinary voice history");
        }
    }
    // A legitimately already-reset Main has no mixer pointer/prepared flag;
    // it can retire with its still-exact backing, without reconstructed rows.
    backing->transport_.reset(); // Backing still owns all raw dependencies.
    backing->closed_=true;backing->stage_=rr::platform::OriginalUiGarageAudioBacking50KG::Stage::closed;
    garageAudioBackingOwner_.reset();error.clear();return true;
}
}
