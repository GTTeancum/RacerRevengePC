#pragma once
namespace original_pause_observation50LA {
inline bool Owner::attachedLifecycle(const Camera& camera,
    const rr::game::OriginalPauseStorage50LA& pause)const noexcept {
    if(lifecycleRejected_||!application_||!pause.entity_||!pause.entity_->initialized||pause.entity_->releasing||
       camera.generation_!=cameraGeneration_||camera.actual_.humanCount34!=0)return false;
    const auto controls=controls_.lock();const auto track=track_.lock();
    const auto& race=application_->sourceRace28COwner_;
    if(!controls||!track||application_->controlsProfile!=controls||
       !raceIdentity(camera,*track,pause,*controls)||!race||race->pause100_.get()!=&pause||
       !race->nativeRootAttached_||race->application_!=application_->entities()||
       race->entity_.sessionGeneration!=cameraGeneration_)return false;
    const auto& c=pause.entity_->component;
    return pause.entity_->application==application_->entities()&&
        c.parent1C==application_->entities()->application10()&&c.mask20==0x1402u&&
        !(c.flags29&8u)&&!c.firstChild18&&!c.descendants24&&
        pause.entity_->currentForest();
}
inline bool Owner::nativeSpectatorMenuOpen()const noexcept {
    const auto pause=pause_.lock();const auto camera=camera_.lock();
    return pause&&camera&&attachedLifecycle(*camera,*pause)&&pause->active200_!=0;
}
inline bool Owner::openNativeAll8SpectatorMenu(const rr::game::InputEvent48Z& event,
    Diagnostic& d)const noexcept {
    d={};d.sourcePC=0x1BE8A0;
    const auto pause=pause_.lock();const auto camera=camera_.lock();
    if(!pause||!camera||!attachedLifecycle(*camera,*pause)||pause->active200_||
       pause->mode1F0_||event.category!=2||event.code!=8||event.transition!=1||
       event.deviceId>=2||(pause->entity_->component.flags29&3u))return false;
    auto& race=*application_->sourceRace28COwner_;
    // Original opening caller1A7C40 first dispatches actual race slot50.
    if(!application_->pauseConsumedRaceEntity142040(race,true))return false;
    lifecycleRejected_=true; //Cleared only after the complete reached callback.
    application_->setExplicitAnimationPause(true); //1BE8C8
    pause->selection880_=0;pause->opening30_=.25f;
    const auto audio=camera->actual_.cycleAudio.dispatchPauseFeedback(2);
    using A=original_camera_cycle_audio50KX::Owner::Result;
    d.sourcePC=0x1BE8E4;
    if(audio!=A::started&&audio!=A::unavailable&&audio!=A::muted)return false;
    if(!pause->entity_->setActivity141630(true))return false;
    pause->active200_=1;pause->controller884_=event.deviceId;
    d.sourcePC=0x1BE910;
    if(!application_->pauseConsumedRaceWorld1ACF50(race,true))return false;
    //1AF770 sees this SAME actual humanCount34==0: no device iteration.
    lifecycleRejected_=false;d={};return true;
}
inline bool Owner::closeLifecycle1BE830(Diagnostic& d)const noexcept {
    d={};const auto pause=pause_.lock();const auto camera=camera_.lock();
    if(!pause||!camera||!attachedLifecycle(*camera,*pause)||!pause->active200_)return false;
    auto& race=*application_->sourceRace28COwner_;
    lifecycleRejected_=true;
    application_->setExplicitAnimationPause(false); //1BE848
    pause->active200_=0; //1BE84C; preserve reached writes on failure.
    d.sourcePC=0x1BE85C;
    if(!application_->pauseConsumedRaceWorld1ACF50(race,false))return false;
    d.sourcePC=0x1BE878;
    if(!application_->pauseConsumedRaceEntity142040(race,false))return false;
    d.sourcePC=0x1BE888;
    if(!pause->entity_->setActivity141630(false))return false;
    lifecycleRejected_=false;d={};return true;
}
inline bool Owner::closeNativeSpectatorMenuForShutdown(Diagnostic& d)const noexcept {
    d={};const auto pause=pause_.lock();const auto camera=camera_.lock();
    if(!pause||!camera||!attachedLifecycle(*camera,*pause))return false;
    if(!pause->active200_)return true;
    //Explicit native shutdown invokes actual close directly, regardless of row.
    //No invented confirm edge, selection mutation, or row-confirm feedback.
    return closeLifecycle1BE830(d);
}
inline Owner::NavigationResult Owner::continueRow0(rr::game::InputEvent48Z& event,
    rr::game::DeviceButtonRecord48Y& record)const noexcept {
    NavigationResult out;using S=NavigationResult::Status;
    const auto pause=pause_.lock();const auto camera=camera_.lock();
    if(!pause||!camera||!attachedLifecycle(*camera,*pause))return out;
    if(!pause->active200_||!(pause->entity_->component.flags29&1u)||pause->mode1F0_||
       pause->selection880_||event.deviceId!=pause->controller884_||
       event.category!=2||event.code!=2||event.transition!=1){
        out.status=S::actionUnavailable;out.sourcePC=0x1BD8BC;return out;
    }
    Diagnostic close;
    if(!closeLifecycle1BE830(close)){out.sourcePC=close.sourcePC;return out;}
    out.audio=camera->actual_.cycleAudio.dispatchPauseFeedback(2); //1BD924 after close.
    using A=original_camera_cycle_audio50KX::Owner::Result;
    if(out.audio!=A::started&&out.audio!=A::unavailable&&out.audio!=A::muted){
        lifecycleRejected_=true;out.status=S::audioUnavailable;out.sourcePC=0x1BD924;return out;
    }
    const rr::game::EventChildRouter48Z children{pause->entity_->component.descendants24,
        rr::game::noEventChild48Z,nullptr,0};
    const auto routed=rr::game::routeChildInputEvent48Z(children,true,event,record,nullptr,nullptr,0);
    out.status=routed.status==rr::game::EventRouteStatus48Z::Complete?S::complete:S::childRoute;
    out.sourcePC=0x1BE80C;out.originalReturn=routed.handled?1u:0u;return out;
}
inline bool Owner::resetNativePresentationClock()const noexcept {
    const auto pause=pause_.lock();const auto camera=camera_.lock();
    return pause&&camera&&attachedLifecycle(*camera,*pause)&&application_->resetUpdateClock140D70();
}
inline bool Owner::prepareNativePresentation(rr::game::OriginalPauseNativeDraw50LA& native,
    Diagnostic& observation,rr::game::OriginalPauseLayout50LA::Diagnostic& layout,
    rr::game::OriginalPauseNativeResources50LA::Diagnostic& draw)const noexcept {
    observation={};layout={};draw={};if(native.frame_)native.frame_->prepared=false;
    const auto pause=pause_.lock();const auto camera=camera_.lock();
    if(!pause||!camera||!attachedLifecycle(*camera,*pause))return false;
    // Actual applicationList producer currently publishes ONE retained scene.
    // Verify that producer, rather than manufacturing count from humanCount.
    const auto& display=camera->display_;
    if(display.cameraCount264!=1||display.applicationCameras254[0]!=camera->sceneBinding_.sceneKey||
       !display.applicationCameras254[0]||display.applicationCameras254[1]||
       display.applicationCameras254[2]||display.applicationCameras254[3])return false;
    if(!application_->beginUpdateClock140AC0())return false;
    bool complete=true;
    //140414 initializes0C;1407E0 advances across all FOUR actual slots.
    for(application_->sourceCameraIndex0C_=0;application_->sourceCameraIndex0C_<4;
        ++application_->sourceCameraIndex0C_){
        const auto index=application_->sourceCameraIndex0C_;
        if(!display.applicationCameras254[index])continue;
        if(!(pause->entity_->component.flags29&1u))continue;
        if(index!=display.cameraCount264-1u)continue; //1B9308..1B931C
        complete=pause->demandResources1BAE00(index,display.cameraCount264,observation)&&
            pause->advanceGeometry1B9324(application_->animationClock(),observation)&&
            captureOuterNativeFrame(native,observation,layout,draw);
        if(!complete)break;
    }
    // Commit even if an actual callback rejected after source effects. Caller
    // reports failure, never retries this presentation or supplies catchup delta.
    const bool committed=application_->commitUpdateClock140D30();
    if(!complete||!committed)lifecycleRejected_=true;
    return complete&&committed;
}
}
