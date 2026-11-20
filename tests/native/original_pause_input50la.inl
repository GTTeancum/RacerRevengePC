// Included inside original_main_entry50KG by original_results104_input50la.cpp.
// Separate ALL8 Pause caller custody. Never adopts human Results/End authority.
struct NativePauseInputContinuation50LA final {
    enum class Phase : std::uint8_t { prepared,stopped,music,feedback,finishedAudio,closed,waitingVideo,videoReturned,closedAfterVideo,queued,failed };
    Phase phase=Phase::prepared;
    std::shared_ptr<rr::game::OriginalRaceConstructor50LA> race;
    std::shared_ptr<original_camera_session50KX::Session> camera;
    std::shared_ptr<Video> video;
    std::shared_ptr<RaceDisplayLifetime50LA> display;
    std::shared_ptr<original_race_hud_scope50LA::Registration> scope;
    std::shared_ptr<rr::game::OriginalPauseStorage50LA> pause;
    const original_pause_observation50LA::Owner* owner=nullptr;
    rr::platform::OriginalSpectatorMenuInput50LA* input=nullptr;
    rr::platform::OriginalVideoBlockingNativeBackend* backend=nullptr;
    original_camera_session50KX::DisplayInputs* inputs=nullptr;
    rr::platform::OriginalUiRiderProfileStamp50KG profile{};
    std::chrono::steady_clock::time_point epoch{};
    std::uint64_t generation=0,frame=0,publication=0,pollRevision=0;
    std::size_t eventOrdinal=0;
    std::uint32_t selection=0; //Actual same-Pause row, never a caller-selected destination.
    // Reuses only exact T0=1 numeric arithmetic. This is a distinct actual
    // Pause invocation, not a Results authority or fabricated guest RaceFC.
    rr::game::OriginalResultsBlockingFade2AED60 fade;
};
#include "original_pause_main_transfer50la.inl"
bool Application::pauseReturnInputPending50LA()const noexcept {
    return garageAudioOwnerThread_==std::this_thread::get_id()&&bool(pauseInputContinuation50LA_);
}
bool Application::pauseInputContinuationCurrent50LA()const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto& p=pauseInputContinuation50LA_;
    if(!p||p->phase==NativePauseInputContinuation50LA::Phase::failed||!p->owner||!p->input||!p->backend||
       sourceRace28COwner_!=p->race||videoOwner_!=p->video||!p->race||!p->pause||
       p->race->pause100_!=p->pause||raceConstructorColdOwner50LA_.lock()!=p->race||
       !p->camera||!p->display||raceDisplayOwner50LA_.lock()!=p->display||
       !p->display->canRetain()||p->display->application_!=this||p->display->video_!=p->video||
       !p->scope||p->race->endHud_.lock()!=p->scope||
       !consumedRaAll8ListenerCustodyNativeCurrent(*p->camera)||
       p->camera->generation_!=p->generation||p->camera->completedFrame_!=p->frame||
       p->camera->publicationCount_!=p->publication||p->camera->actual_.applicationClock.epoch!=p->epoch||
       p->camera->displayInputs_!=p->inputs||!p->inputs||
       &p->inputs->renderer!=&p->display->renderer_||
       &p->backend->renderer_!=&p->display->renderer_||&p->backend->host_!=&p->video->host||
       p->backend->use_!=rr::platform::OriginalVideoNativeUse::blockingSourceLoops||
       !riderProfile||p->race->profile_!=riderProfile||
       riderProfile->stamp().generation!=p->profile.generation||riderProfile->stamp().mode34!=p->profile.mode34||
       p->owner->application_!=this||p->owner->race_.lock()!=p->race||
       p->owner->pause_.lock()!=p->pause||p->owner->camera_.lock()!=p->camera||
       !p->owner->retainedAttachedLifecycle(*p->camera,*p->pause)||
       !p->input->currentPoll()||p->input->host_!=&p->video->host||
       !p->input->retainedForPauseReturn_||p->input->claimedOwner_!=p->owner||
       p->input->pollRevision_!=p->pollRevision||p->input->next_!=p->eventOrdinal||
       !p->input->eventStarted_||p->eventOrdinal>=p->input->batch_.count||
       !pauseMenuRequestTailCurrent50LA(*p->race))return false;
    using InputPhase=rr::platform::OriginalSpectatorMenuInput50LA::Phase;
    if(p->input->phase_!=InputPhase::dispatching&&p->input->phase_!=InputPhase::suspended)return false;
    const auto& event=p->input->batch_.events[p->eventOrdinal];
    if(event.code!=2u||event.category!=2u||event.transition!=1u||event.deviceId!=p->pause->controller884_)return false;
    if(p->selection!=9u&&p->selection!=10u)return false;
    if(p->selection==10u){
        rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection course;
        if(!riderProfile->courseRaceSnapshot(course)||course.mode34!=0u||
           (course.subtype!=1u&&course.subtype!=2u)||course.subtype!=p->camera->actual_.launchKind11258)return false;
    }
    const bool closed=p->selection==9u?p->phase>=NativePauseInputContinuation50LA::Phase::closed:
        p->phase>=NativePauseInputContinuation50LA::Phase::closedAfterVideo;
    std::uint32_t paused=0;
    return p->pause->selection880_==p->selection&&!p->pause->mode1F0_&&
        (p->pause->active200_!=0u)!=closed&&
        p->camera->actual_.applicationAnimation.readPause11C(paused)&&((paused!=0u)!=closed);
}
bool Application::pauseInputBatchReturned50LA(const NativePauseInputContinuation50LA& p)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||pauseInputContinuation50LA_.get()!=&p||
       p.phase!=NativePauseInputContinuation50LA::Phase::queued||!p.fade.complete()||!p.input||!p.owner||
       sourceRace28COwner_!=p.race||videoOwner_!=p.video||p.race->pause100_!=p.pause||
       p.owner->application_!=this||p.owner->race_.lock()!=p.race||p.owner->pause_.lock()!=p.pause||
       p.input->thread_!=std::this_thread::get_id()||
       p.input->phase_!=rr::platform::OriginalSpectatorMenuInput50LA::Phase::retired||
       p.input->host_||p.input->retainedForPauseReturn_||p.input->eventStarted_||
       p.input->claimedOwner_!=p.owner||p.input->pollRevision_!=p.pollRevision||
       p.input->next_!=p.input->batch_.count||p.eventOrdinal>=p.input->batch_.count||
       p.video->host.spectatorBatchOwner50LA_||p.video->host.sharedDialogPollRevision50KG_!=p.pollRevision)return false;
    return p.input->batch_.events[p.eventOrdinal].code==2u&&
        passPauseMenuProcessSuccessors50LA(*p.race);
}
Application::PauseReturnStart50LA Application::beginNativePauseReturn50LA(
    const original_pause_observation50LA::Owner& owner,rr::platform::OriginalSpectatorMenuInput50LA& input,
    rr::platform::OriginalVideoBlockingNativeBackend* backend,std::uint32_t& pc)const noexcept {
    using Start=PauseReturnStart50LA;using Phase=NativePauseInputContinuation50LA::Phase;
    pc=0x1BD8CC;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||owner.application_!=this||
       pauseInputContinuation50LA_||resultsInputContinuation50LA_||pauseMenuTransfer50LA_||resultsHallTransfer50LA_||
       input.thread_!=std::this_thread::get_id()||input.phase_!=rr::platform::OriginalSpectatorMenuInput50LA::Phase::dispatching||
       input.claimedOwner_!=&owner||!input.eventStarted_||input.next_>=input.batch_.count||
       !input.currentPoll()||input.retainedForPauseReturn_)return Start::failed;
    const auto race=owner.race_.lock();const auto pause=owner.pause_.lock();const auto observed=owner.camera_.lock();
    const auto& event=input.batch_.events[input.next_];
    if(!race||sourceRace28COwner_!=race||!pause||race->pause100_!=pause||!observed||
       !owner.attachedLifecycle(*observed,*pause)||!pause->active200_||pause->mode1F0_||(pause->selection880_!=9u&&pause->selection880_!=10u)||
       !(pause->entity_->component.flags29&1u)||event.deviceId!=pause->controller884_||
       event.category!=2u||event.code!=2u||event.transition!=1u)return Start::failed;
    const bool courseReturn=pause->selection880_==10u;
    pc=courseReturn?0x1BDA74u:0x1BD9B8u;
    if(!sourceUseProfile2E0_)return Start::unavailable;
    if(!*sourceUseProfile2E0_){pc=0x1BE804;return Start::sourceSkipped;}
    if(courseReturn){
        rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection course;
        rr::game::OriginalUiCourseRememberedProfile50KG remembered;
        // Actual source profile branch, BEFORE any effects. Mode0/subtype3
        // reaches qualification1ABFC0 and is not presumed false from missing
        // Results ownership. This bounded receiver admits only types1/2.
        if(!riderProfile||!riderProfile->courseRaceSnapshot(course)||course.mode34!=0u||
           (course.subtype!=1u&&course.subtype!=2u)||course.subtype!=observed->actual_.launchKind11258||
           !profile||!profile->known||riderProfile->commonProfile()!=profile.get()||!state||
           state->sharedDialogs||state->controllerPopupState.active3CC||state->controllerPopupState.closing3C8||
           state->uiObject3D8||
           !riderProfile->courseSnapshot(remembered)||!remembered.ownerReady||remembered.mode34!=0u||
           (remembered.cardFlag100BC&&!remembered.nativeBetaStorage)||
           riderProfile->stamp().generation>std::numeric_limits<std::uint64_t>::max()-2u)return Start::unavailable;
    }
    // Native capability admission BEFORE any source stop or close. Existing
    // framebuffer proof is real completed Pause, never a fallback underlay.
    if(!enabled||!retainPlayerWindow50DU_||!backend||!videoOwner_||input.host_!=&videoOwner_->host||
       !consumedRaAll8ListenerCustodyNativeCurrent(*observed)||!race->world74_||
       !riderProfile||race->profile_!=riderProfile||!race->raceAudio_||!race->music10C_||
       observed->pendingDisplayWork()||observed->blockingVideoPending()||observed->asyncVideoPending()||
       !observed->displayInputs_||!sourceAural00Owner_||!backend->pauseReturnSourceCurrent())return Start::unavailable;
    const auto display=raceDisplayOwner50LA_.lock();const auto scope=race->endHud_.lock();
    if(!display||!display->canRetain()||display->application_!=this||display->video_!=videoOwner_||
       &backend->renderer_!=&display->renderer_||&backend->host_!=&videoOwner_->host||
       &observed->displayInputs_->renderer!=&display->renderer_||
       !scope||!scope->music_||!scope->announcer_||!scope->gameplay_||!scope->engines_||
       !race->music10C_->nativeBackendCurrent(*scope)||!pauseMenuRequestTailCurrent50LA(*race))return Start::unavailable;
    auto& video=observed->displayInputs_->video;
    if(!video.alpha3E8||!std::isfinite(video.lastMilliseconds400)||
       std::fegetround()!=FE_TONEAREST||observed->displayInputs_->currentApplicationMode22C()!=0)
        return Start::unavailable; //No invented unknown alpha, EE rounding or source mode.
    try {
        // Every race: the Pause owner's own bound camera session (the race's
        // camera adapter34), not an RA aggregate borrow (common rewrite, R9).
        auto camera=observed;
        if(!camera||!race->ownsNativeCameraAdapter34(*camera))return Start::unavailable;
        auto p=std::make_shared<NativePauseInputContinuation50LA>();
        p->race=race;p->pause=pause;p->camera=std::move(camera);p->video=videoOwner_;p->display=display;p->scope=scope;
        p->owner=&owner;p->input=&input;p->backend=backend;p->inputs=p->camera->displayInputs_;
        p->profile=riderProfile->stamp();p->generation=p->camera->generation_;p->frame=p->camera->completedFrame_;
        p->publication=p->camera->publicationCount_;p->epoch=p->camera->actual_.applicationClock.epoch;
        p->pollRevision=input.pollRevision_;p->eventOrdinal=input.next_;p->selection=pause->selection880_;
        pauseInputContinuation50LA_=p;input.retainedForPauseReturn_=true;
        const auto stop=[&](std::uint32_t at){pc=at;p->phase=Phase::failed;return Start::failed;};
        // Count/enroll the genuine native request while effects are still
        // untouched. Any partial allocation remains owned for cancellation.
        if(!preparePauseMenuTransfer50LA(*p))return stop(courseReturn?0x1BDB6Cu:0x1BD9D4u);
        rr::platform::OriginalApplicationAural50KG::Diagnostic ad;
        if(!pauseInputContinuationCurrent50LA()||
           !sourceAural00Owner_->stopResultsNative26D070(*this,*race->raceAudio_,*scope->music_,
               p->camera->actual_.cycleAudio,*scope->announcer_,*scope->gameplay_,*scope->engines_,
               scope->countdown_,p->generation,ad))return stop(courseReturn?0x1BDB6Cu:0x1BD9D4u);
        p->phase=Phase::stopped;
        rr::game::OriginalRaceMusicManagerDiagnostic50LA music;
        using Music=rr::game::OriginalRaceMusicManager50LA;
        const auto call=courseReturn?Music::PauseReturnCall::course1BDB88:Music::PauseReturnCall::main1BD9F0;
        if(!pauseInputContinuationCurrent50LA()||
           !race->music10C_->pauseReturnNativeFade1A4AB0(*race,*scope,call,music))
            return stop(courseReturn?0x1BDB88u:0x1BD9F0u);
        p->phase=Phase::music;
        if(!pauseInputContinuationCurrent50LA())return stop(courseReturn?0x1BDBA0u:0x1BDA08u);
        const auto feedback=p->camera->actual_.cycleAudio.dispatchPauseFeedback(2);
        using Audio=original_camera_cycle_audio50KX::Owner::Result;
        if(feedback!=Audio::started&&feedback!=Audio::unavailable&&feedback!=Audio::muted)return stop(courseReturn?0x1BDBA0u:0x1BDA08u);
        p->phase=Phase::feedback;
        if(!pauseInputContinuationCurrent50LA()||
           !sourceAural00Owner_->finishResultsNative267BE0(*this,*race->raceAudio_,ad))return stop(courseReturn?0x1BDBB8u:0x1BDA20u);
        p->phase=Phase::finishedAudio;
        if(!pauseInputContinuationCurrent50LA())return stop(courseReturn?0x1BDBDCu:0x1BDA28u);
        if(!courseReturn){
            original_pause_observation50LA::Owner::Diagnostic close;
            if(!owner.closeLifecycle1BE830(close))return stop(close.sourcePC?close.sourcePC:0x1BDA28u);
            p->phase=Phase::closed;
            if(!pauseInputContinuationCurrent50LA())return stop(0x1BDA4C);
        }
        // Row10 deliberately retains active Pause/App11C through its actual
        // blocking video call; original1BDBE4 closes only AFTER that return.
        p->phase=Phase::waitingVideo;input.phase_=rr::platform::OriginalSpectatorMenuInput50LA::Phase::suspended;
        pc=courseReturn?0x1BDBDCu:0x1BDA4Cu;return Start::started;
    }catch(...){
        if(pauseInputContinuation50LA_)pauseInputContinuation50LA_->phase=Phase::failed;
        return Start::failed;
    }
}
bool Application::advanceNativePauseReturn50LA(std::uint32_t& pc)const noexcept {
    using Phase=NativePauseInputContinuation50LA::Phase;pc=0x1BDA4C;
    if(!pauseInputContinuationCurrent50LA())return false;
    const auto p=pauseInputContinuation50LA_;const bool courseReturn=p->selection==10u;
    pc=courseReturn?0x1BDBDCu:0x1BDA4Cu;
    if(p->phase!=Phase::waitingVideo||p->input->phase_!=rr::platform::OriginalSpectatorMenuInput50LA::Phase::suspended)return false;
    const auto stop=[&](std::uint32_t at){pc=at;p->phase=Phase::failed;return false;};
    try {
        const auto result=p->fade.advance(p->inputs->fadeManagerA8,p->inputs->currentApplicationMode22C(),
            p->inputs->video,p->camera->actual_.applicationClock,p->backend);
        if(!pauseInputContinuationCurrent50LA())return stop(result.sourcePC);
        if(result.status==rr::game::OriginalCameraStartStatus::videoClockWait){pc=result.sourcePC;return true;}
        if(result.status!=rr::game::OriginalCameraStartStatus::complete)return stop(result.sourcePC);
        p->phase=Phase::videoReturned;
        if(courseReturn){
            if(!pauseInputContinuationCurrent50LA())return stop(0x1BDBE4);
            original_pause_observation50LA::Owner::Diagnostic close;
            if(!p->owner->closeLifecycle1BE830(close))return stop(close.sourcePC?close.sourcePC:0x1BDBE4u);
            p->phase=Phase::closedAfterVideo;
            if(!pauseInputContinuationCurrent50LA())return stop(0x1BDBFC);
        }
        if(!commitPauseMenuTransfer50LA(*p))return stop(courseReturn?0x1BDBFCu:0x1BDA64u);
        p->phase=Phase::queued;
        original_pause_observation50LA::Owner::Diagnostic tail;
        if(!p->owner->finishNativePauseReturnInput(*p->input,tail))return stop(tail.sourcePC);
        if(!markPauseMenuBatchReturned50LA(*p))return stop(0x1BE80C);
        pauseInputContinuation50LA_.reset();pc=0x1BE814;return true;
    }catch(...){return stop(pc);}
}
bool Application::cancelNativePauseReturn50LA(rr::platform::OriginalVideoBlockingNativeBackend* backend)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto p=pauseInputContinuation50LA_;if(!p)return true;
    // Publication-local guard runs BEFORE its input arrays disappear. The
    // backend and pinned Display/Camera remain alive until actual retirement.
    if(!backend||backend!=p->backend||!p->input||p->input->thread_!=std::this_thread::get_id()||
       (p->input->host_&&(p->input->host_!=&p->video->host||
        p->video->host.spectatorBatchOwner50LA_!=p->input))||!backend->cancelAndRetire())return false;
    p->input->retainedForPauseReturn_=false;
    p->input->phase_=rr::platform::OriginalSpectatorMenuInput50LA::Phase::failed;
    if(p->input->host_){
        p->video->host.sharedDialogPollCurrent50KG_=false;p->video->host.loadingInputBarrier50DU_=true;
    }
    // Producer destruction, not a fake success, abandons the remaining batch.
    // The distinct menu request is cancelled by outer closeWindow afterward.
    pauseInputContinuation50LA_.reset();return true;
}
