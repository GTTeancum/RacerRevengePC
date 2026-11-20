#pragma once
namespace original_pause_observation50LA {
inline bool Owner::retainedAttachedLifecycle(const Camera& camera,
    const rr::game::OriginalPauseStorage50LA& pause)const noexcept {
    if(lifecycleRejected_||!application_||!pause.entity_||!pause.entity_->initialized||pause.entity_->releasing||
       camera.generation_!=cameraGeneration_)return false;
    const auto controls=controls_.lock();
    const auto& race=application_->sourceRace28COwner_;
    if(!controls||application_->controlsProfile!=controls)return false;
    if(counted_){
        // 149p: the counted race of every course (its own camera adapter34).
        if(!countedIdentity(camera,pause,*controls,false))return false;
    }else{
        const auto track=track_.lock();
        if(camera.actual_.humanCount34!=0||!track||!retainedRaceIdentity(camera,*track,pause,*controls))return false;
    }
    if(!race||race->pause100_.get()!=&pause||
       !race->nativeRootAttached_||race->application_!=application_->entities()||
       race->entity_.sessionGeneration!=cameraGeneration_)return false;
    const auto& c=pause.entity_->component;
    return pause.entity_->application==application_->entities()&&
        c.parent1C==application_->entities()->application10()&&c.mask20==0x1402u&&
        !(c.flags29&8u)&&!c.firstChild18&&!c.descendants24&&
        pause.entity_->currentForest();
}
inline bool Owner::attachedLifecycle(const Camera& camera,
    const rr::game::OriginalPauseStorage50LA& pause)const noexcept {
    if(!retainedAttachedLifecycle(camera,pause))return false;
    if(counted_){const auto& race=application_->sourceRace28COwner_;return race&&race->ownsNativeCameraAdapter34(camera);}
    return application_->consumedRaAll8CameraCustodyNative34Current(camera);
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
    // 149p: a counted race whose world 4D8 group the open could not pause
    // (logged there) has nothing to resume here; RA ALL8 always requires it.
    if((!counted_||countedWorldPaused_)&&!application_->pauseConsumedRaceWorld1ACF50(race,false))return false;
    countedWorldPaused_=false;
    d.sourcePC=0x1BE878;
    if(!application_->pauseConsumedRaceEntity142040(race,false))return false;
    d.sourcePC=0x1BE888;
    if(!pause->entity_->setActivity141630(false))return false;
    countedOwnsOpen_=false;
    lifecycleRejected_=false;d={};return true;
}
inline bool Owner::closeNativeSpectatorMenuForShutdown(Diagnostic& d)const noexcept {
    d={};const auto pause=pause_.lock();const auto camera=camera_.lock();
    // A gameplay failure can invalidate live Bank/Display admission while this
    // exact world still owns its Session. An already inactive Pause has no
    // close callback left to run; retain the original cleanup path without
    // claiming that failed native camera work completed.
    if(!pause||!camera||!retainedAttachedLifecycle(*camera,*pause))return false;
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
inline bool Owner::dispatchNativeInputBatch(rr::platform::OriginalSpectatorMenuInput50LA& input,
    rr::platform::OriginalVideoBlockingNativeBackend* backend,Diagnostic& d)const noexcept {
    d={};if(!input.beginDispatch(*this))return false;
    return dispatchNativeInputRemainder(input,backend,d);
}
inline bool Owner::dispatchNativeInputRemainder(rr::platform::OriginalSpectatorMenuInput50LA& input,
    rr::platform::OriginalVideoBlockingNativeBackend* backend,Diagnostic& d)const noexcept {
    using Phase=rr::platform::OriginalSpectatorMenuInput50LA::Phase;
    while(input.next_<input.batch_.count){
        auto& event=input.batch_.events[input.next_];
        auto& record=input.batch_.records[input.next_];
        input.eventStarted_=true; //Claim before this input callback can mutate source state or audio.
        const bool open=nativeSpectatorMenuOpen();
        if(!open&&event.code==8){
            if(!openNativeAll8SpectatorMenu(event,d)){
                input.phase_=Phase::failed;return false;
            }
        }else if(open&&(event.code==12||event.code==14||event.code==2||event.code==15||event.code==13)){
            const auto pause=pause_.lock();
            if(event.code==2&&pause&&(pause->selection880_==9u||pause->selection880_==10u)){
                using Start=original_main_entry50KG::Application::PauseReturnStart50LA;
                const auto started=application_->beginNativePauseReturn50LA(*this,input,backend,d.sourcePC);
                if(started==Start::started)return true; //Same event remains started and the whole batch stays owned.
                if(started==Start::failed){input.phase_=Phase::failed;return false;}
                if(started==Start::sourceSkipped){
                    //Actual App2E0==0 skips the body, but still calls the real child tail.
                    const rr::game::EventChildRouter48Z children{pause->entity_->component.descendants24,
                        rr::game::noEventChild48Z,nullptr,0};
                    if(rr::game::routeChildInputEvent48Z(children,true,event,record,nullptr,nullptr,0).status!=
                        rr::game::EventRouteStatus48Z::Complete){input.phase_=Phase::failed;return false;}
                }
                input.eventStarted_=false;++input.next_;continue;
            }
            const auto result=event.code==2?continueRow0(event,record):pressNavigation1BD7C0(event,record);
            using S=NavigationResult::Status;
            if(result.status!=S::complete&&result.status!=S::actionUnavailable){
                d.sourcePC=result.sourcePC;input.phase_=Phase::failed;return false;
            }
            // Other unavailable actions retain their original bounded behavior.
            // Never invoke a generic extra child tail or fabricate handled input.
        }
        input.eventStarted_=false;++input.next_;
    }
    if(!input.retire(*this)){input.phase_=Phase::failed;return false;}
    return true;
}
inline bool Owner::finishNativePauseReturnInput(rr::platform::OriginalSpectatorMenuInput50LA& input,
    Diagnostic& d)const noexcept {
    using Phase=rr::platform::OriginalSpectatorMenuInput50LA::Phase;
    d={};d.sourcePC=0x1BE80C;
    const auto pause=pause_.lock();const auto camera=camera_.lock();
    if(!pause||!camera||!retainedAttachedLifecycle(*camera,*pause)||pause->active200_||
       input.phase_!=Phase::suspended||!input.retainedForPauseReturn_||!input.currentPoll()||
       input.claimedOwner_!=this||!input.eventStarted_||input.next_>=input.batch_.count)return false;
    auto& event=input.batch_.events[input.next_];auto& record=input.batch_.records[input.next_];
    const rr::game::EventChildRouter48Z children{pause->entity_->component.descendants24,
        rr::game::noEventChild48Z,nullptr,0};
    if(rr::game::routeChildInputEvent48Z(children,true,event,record,nullptr,nullptr,0).status!=
        rr::game::EventRouteStatus48Z::Complete)return false;
    input.eventStarted_=false;++input.next_;input.phase_=Phase::dispatching;
    // Complete the SAME original physical batch after its source child tail.
    // In this fixed order only Left/Right can follow Confirm; closed Pause
    // consumes them without reopening or invoking another row9 body.
    return dispatchNativeInputRemainder(input,nullptr,d);
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
// ---------------------------------------------------------------------------
// 149p: the original Pause owner on the counted race of every course.
// Race input 1A87A0 (Start on a human pad: F8 199770(0,1,1,1,1), HUD stop
// 1AD230, race1A4 = device, race1B1 bit0), the race update 1A7C1C (race slot50
// 142040 then 1BE8A0), and the Pause input 1BD7C0/1BD8CC rows. One rule for
// every course, add-on courses included: no course, pod or track key.
inline bool Owner::countedPressRace1A87A0(rr::game::InputEvent48Z& event,CountedInput& out)const noexcept {
    if(!counted_||event.category!=2u||event.code!=8u||event.transition!=1u)return true;
    const auto pause=pause_.lock();const auto camera=camera_.lock();
    if(!pause||!camera||!attachedLifecycle(*camera,*pause)){
        out.refused=true;out.gate=1;out.sourcePC=0x1A87A0;return false;
    }
    auto& race=*application_->sourceRace28COwner_;
    // 1A87B8..1A87F0: race B4 (racer manager) +58: not finished (bit0) and
    // racing (1B0B80: bit1 and not bit0) or bit2. 1A87F8: device < race1B0&3.
    // 1A8810: race1B1 bit0 (pending) clear. 1A883C: Pause+200 == 0.
    const auto flags58=camera->shadowManagerFlags58();
    const bool admitted=!(flags58&1u)&&(((flags58&2u)!=0u)||((flags58&4u)!=0u));
    const auto humans=static_cast<std::uint32_t>(race.flags1B0KnownValue_&3u);
    if(!pause->active200_){
        // Finished (bit0): 1A87A0 refuses the Start (common rewrite R9, 2.8);
        // it is consumed and logged, never handed to a host menu.
        if(flags58&1u){
            out.startConsumed=true;out.gate=2;out.sourcePC=0x1A87B8;
            common_race_log50CR::pauseRefused(0x1A87B8,"race B4+58 bit0 set (finished)");
            return true;
        }
        out.startConsumed=true;
        if(!admitted||event.deviceId>=humans||countedPending_){
            out.gate=3;out.sourcePC=0x1A8890;
            std::fprintf(stderr,"pause_request site=1A8890 refused flags58=%u device=%u humans=%u pending=%d\n",
                unsigned(flags58),unsigned(event.deviceId),unsigned(humans),int(countedPending_));
            return true;
        }
        // 1A8860 199770(F8,0,1,1,1,1) and 1A8868 1AD230(race) through the
        // Application's private F8/HUD-scope borrows (each logged).
        bool f8=false,hud=false;
        application_->countedPauseRequest1A87A0_149p(race,f8,hud);
        std::fprintf(stderr,"pause_request site=1A87A0 device=%u f8_199770=%d hud_1AD230=%d\n",
            unsigned(event.deviceId),int(f8),int(hud));
        countedDevice_=event.deviceId;countedPending_=true; //1A887C race1A4, 1A888C race1B1 bit0.
        out.sourcePC=0x1A8890;return true;
    }
    // 1A8890..1A88E4: Start on Pause+884's device while Pause+200 == 1:
    // 1BE830 close, then 1BA100(Pause,2).
    if(!countedOwnsOpen_||event.deviceId!=pause->controller884_||pause->active200_!=1u)return true;
    out.startConsumed=true;
    Diagnostic close;
    if(!closeLifecycle1BE830(close)){out.refused=true;out.gate=4;out.sourcePC=close.sourcePC?close.sourcePC:0x1A88D4u;return false;}
    camera->actual_.cycleAudio.dispatchPauseFeedback(2);
    out.closed=true;out.sourcePC=0x1A88E4;
    std::fprintf(stderr,"pause_close site=1A88D4 row=start\n");
    return true;
}
inline bool Owner::updateCountedRace1A7C1C(std::uint32_t& gate)const noexcept {
    gate=0;if(!counted_||!countedPending_)return true;
    const auto pause=pause_.lock();const auto camera=camera_.lock();
    if(!pause||!camera||!attachedLifecycle(*camera,*pause)||pause->active200_||pause->mode1F0_||
       (pause->entity_->component.flags29&3u)){countedPending_=false;gate=1;return false;}
    auto& race=*application_->sourceRace28COwner_;
    // 1A7C2C: race1B0 bit3 set skips the open (and keeps the request pending).
    if(race.flags1B0KnownValue_&8u){gate=2;return false;}
    // 1A7C40 race vtable+50 = 142040(race,1): preflighted, no store on refusal.
    if(!application_->pauseConsumedRaceEntity142040(race,true)){countedPending_=false;gate=3;return false;}
    countedPending_=false; //1A7C74 race1B1 bit0 clear (after 1BE8A0 below).
    // 1BE8A0: App11C = 1, Pause+880 = 0, Pause+30 = .25, 1A4B90(race10C,2),
    // Pause vtable+24 (activity), Pause+200 = 1, Pause+884 = device,
    // 1ACF50(race), 1AF770(race B0).
    application_->setExplicitAnimationPause(true);
    pause->selection880_=0;pause->opening30_=.25f;
    camera->actual_.cycleAudio.dispatchPauseFeedback(2);
    if(!pause->entity_->setActivity141630(true)){
        application_->pauseConsumedRaceEntity142040(race,false);application_->setExplicitAnimationPause(false);
        gate=4;return false;
    }
    pause->active200_=1;pause->controller884_=countedDevice_;countedOwnsOpen_=true;
    // 1ACF50: the race world's 4D8 audio group. PORT STAND-IN (flagged): a
    // course world whose 4D8 group the port does not own leaves it unpaused
    // (logged) instead of failing the open; 1BE830 then skips its resume.
    countedWorldPaused_=application_->pauseConsumedRaceWorld1ACF50(race,true);
    // 1AF770 human-device iteration (pad vibration stop): the loop's own
    // stopFeedbackTransport on the paused frame (PORT STAND-IN, flagged).
    rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection course;
    const bool known=application_->riderProfile&&application_->riderProfile->courseRaceSnapshot(course);
    std::fprintf(stderr,"pause_open owner=original site=1BE8A0 device=%u course=%u subtype=%u mode34=%u world_1ACF50=%d humans=%u\n",
        unsigned(countedDevice_),known?unsigned(course.course):99u,unsigned(camera->actual_.launchKind11258),
        application_->riderProfile?unsigned(application_->riderProfile->stamp().mode34):99u,
        int(countedWorldPaused_),unsigned(camera->actual_.humanCount34));
    return true;
}
inline bool Owner::countedConfirm1BD8CC(rr::game::InputEvent48Z& event,rr::game::DeviceButtonRecord48Y& record,
    CountedInput& out)const noexcept {
    const auto pause=pause_.lock();const auto camera=camera_.lock();
    if(!pause||!camera||!attachedLifecycle(*camera,*pause)){out.refused=true;out.gate=5;return false;}
    // 1BD8BC: 1277D0 on Pause+884's device, Cross (code 2).
    if(pause->mode1F0_||event.deviceId!=pause->controller884_||event.category!=2u||event.transition!=1u)return true;
    const auto row=pause->selection880_;out.selection=row;
    using A=original_camera_cycle_audio50KX::Owner::Result;
    const auto feedback=[&](){const auto a=camera->actual_.cycleAudio.dispatchPauseFeedback(2);
        return a==A::started||a==A::unavailable||a==A::muted;};
    if(row==0u){ //1BD908 1BE830, 1BD920 1A4B90(2).
        const auto result=continueRow0(event,record);
        out.sourcePC=result.sourcePC;out.closed=result.status==NavigationResult::Status::complete;
        std::fprintf(stderr,"pause_row row=0 site=1BD908 closed=%d\n",int(out.closed));
        if(!out.closed){out.refused=true;out.gate=9;}
        return out.closed;
    }
    if(row==1u){
        // 1BD934 1BE830; 1BD93C..1BD980 race1B0 bit4 (replay) cleared when set;
        // 1BD990 1AC8E0(race,1) in place (common_race_start50CR, no reload);
        // 1BD9A8 1A4B90(2). Common rewrite R9 (F01 retired).
        Diagnostic close;
        if(!closeLifecycle1BE830(close)){out.refused=true;out.gate=6;out.sourcePC=close.sourcePC;return false;}
        out.closed=true;out.row=CountedInput::Row::restart;
        auto& race=*application_->sourceRace28COwner_;
        if((race.flags1B0KnownMask_&0x10u)&&(race.flags1B0KnownValue_&0x10u))
            race.flags1B0KnownValue_=std::uint8_t(race.flags1B0KnownValue_&~0x10u);
        const auto* hooks=native_common_results50PD::state().raceStartHooks;
        const common_race_start50CR::Request request{common_race_start50CR::Reason::restart,1u,
            common_race_start50CR::callerPauseRestart1BD934,camera->actual_.launchKind11258};
        common_race_start50CR::Receipt receipt;
        if(hooks)out.restarted=application_->restartConsumedRace1AC8E0(race,request,*hooks,receipt);
        else{
            common_race_log50CR::RaceStart line;line.restart=true;line.a1=1;line.caller=request.caller;
            line.ok=false;line.refusedPc=0x1AC8E0u;line.reason="race start hooks not bound (I3 publication)";
            common_race_log50CR::raceStart(line);
        }
        feedback();out.sourcePC=0x1BD9A8;
        std::fprintf(stderr,"pause_row row=1 site=1BD990 restart=in_place ok=%d\n",int(out.restarted));
        if(!out.restarted){out.refused=true;out.gate=10;}
        return true;
    }
    if(row==9u||row==10u){
        // 1BD9B8 / 1BDA74: App2E0 == 0 skips the row body (1BE804).
        if(!application_->sourceUseProfile2E0_||!*application_->sourceUseProfile2E0_){
            out.sourcePC=0x1BE804;std::fprintf(stderr,"pause_row row=%u site=1BE804 app2E0=0\n",unsigned(row));return true;}
        // Common rewrite R9 (F03, F05): the rows run in the original order.
        // Row 10 in a Time Trial (11258==3, P+34==0) first asks 1BDAB4
        // 1ABFC0(race,1): a record opens Results page 3 over the race; no
        // record takes 1BDB5C. Otherwise row 9 (1BD9B8) / row 10 (1BDB5C) run
        // step 18 (26D070, 1A4AB0(0.5), 1A4B90(2), 267BE0, 2AED60, 1BE830,
        // 17DE10(6,4|6,1)) through Application::departRace50CR. The loop only
        // carries the queued 17DE10 request to the frontend receiver.
        auto& race=*application_->sourceRace28COwner_;
        const auto finishClose=[&](){countedOwnsOpen_=false;countedWorldPaused_=false;lifecycleRejected_=false;};
        rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection course;
        const bool timeTrial=row==10u&&camera->actual_.launchKind11258==3u&&application_->riderProfile&&
            application_->riderProfile->courseRaceSnapshot(course)&&course.mode34==0u;
        if(timeTrial){
            bool qualifies=false;std::uint32_t pc=0;
            const bool ok=application_->quitTimeTrialFromPause1BDAB4(race,qualifies,pc);
            out.sourcePC=pc;out.closed=true;
            if(ok){
                finishClose();
                out.row=qualifies?CountedInput::Row::record:CountedInput::Row::course;out.departed=!qualifies;
                std::fprintf(stderr,"pause_quit row=10 site=1BDAB4 qualify_1ABFC0=%d pc=%X\n",int(qualifies),unsigned(pc));
                return true;
            }
            out.refused=true;out.gate=11;out.row=CountedInput::Row::course;
            std::fprintf(stderr,"pause_quit row=10 site=1BDAB4 refused pc=%X\n",unsigned(pc));
            return false;
        }
        common_race_end50CR::Departure departure;
        const bool departed=application_->departRace50CR(race,
            row==9u?common_race_end50CR::Destination::mainMenu:common_race_end50CR::Destination::course,departure);
        if(departed)finishClose();
        out.closed=departed;out.departed=departed;
        out.row=row==9u?CountedInput::Row::main:CountedInput::Row::course;
        out.sourcePC=departure.reachedPc?departure.reachedPc:(row==9u?0x1BD9B8u:0x1BDB5Cu);
        std::fprintf(stderr,"pause_row row=%u site=%s departed=%d pc=%X\n",unsigned(row),row==9u?"1BD9B8":"1BDB5C",
            int(departed),unsigned(out.sourcePC));
        if(!departed){out.refused=true;out.gate=7;return false;}
        return true;
    }
    out.sourcePC=0x1BE804;return true; //Rows 2..8: Cross has no body.
}
inline bool Owner::dispatchCountedRaceInput(rr::game::InputEvent48Z* events,rr::game::DeviceButtonRecord48Y* records,
    std::size_t count,CountedInput& out)const noexcept {
    out={};if(!counted_||!events||!records)return false;
    for(std::size_t i=0;i<count;++i){
        auto& event=events[i];auto& record=records[i];
        const bool wasOpen=countedOpen();
        // Race (App10 first child) before its Pause100 sibling.
        if(!countedPressRace1A87A0(event,out))return false;
        if(out.closed)return true;
        if(!wasOpen||!countedOpen())continue; //Pause 1BD7C0 runs only on an active Pause.
        if(event.code==2u){
            if(!countedConfirm1BD8CC(event,record,out))return false;
            if(out.closed||out.row!=CountedInput::Row::none)return true;
        }else if(event.code==12u||event.code==14u||event.code==15u||event.code==13u){
            const auto result=pressNavigation1BD7C0(event,record);
            using S=NavigationResult::Status;
            out.navigated=out.navigated||result.selectionChanged||result.settingsChanged;
            if(result.status!=S::complete&&result.status!=S::actionUnavailable){
                std::fprintf(stderr,"pause_navigation refused status=%u pc=%x\n",unsigned(result.status),unsigned(result.sourcePC));
                out.refused=true;out.gate=8;out.sourcePC=result.sourcePC;return false;
            }
            if(result.settingsChanged){const auto p=pause_.lock();
                std::fprintf(stderr,"pause_row row=%u site=%x setting=1\n",p?unsigned(p->selection880_):99u,unsigned(result.sourcePC));}
        }
    }
    return true;
}
}
