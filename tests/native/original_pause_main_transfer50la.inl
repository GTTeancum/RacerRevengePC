// Included only by original_pause_input50la.inl AFTER its private retained
// NativePauseInputContinuation50LA definition. No independent caller receipt.
// Native late App2E8/2C6C20 ownership for actual row9 Main or row10 Course.
// The historical filename does not imply a fabricated Main predecessor.
struct NativePauseMenuTransfer50LA final {
    enum class Phase : std::uint8_t { prepared,committing,queued,batchReturned,departing,receiver,accepting };
    Phase phase=Phase::prepared;
    std::shared_ptr<rr::game::OriginalRaceConstructor50LA> race;
    std::shared_ptr<rr::game::OriginalApplicationEntities50LA> entities;
    std::shared_ptr<rr::platform::OriginalUiRiderProfileOwner50KG> profile;
    std::shared_ptr<rr::platform::OriginalUiMainProfile50KG> common;
    std::shared_ptr<Video> video;
    // The retained input owns these strongly until its whole batch returns.
    // Transfer must not keep publication-local Camera/Display/Scope/Pause alive
    // across real native retirement. No raw producer/observer is dereferenced.
    std::weak_ptr<rr::game::OriginalPauseStorage50LA> pause;
    std::weak_ptr<original_camera_session50KX::Session> camera;
    std::weak_ptr<RaceDisplayLifetime50LA> display;
    std::weak_ptr<original_race_hud_scope50LA::Registration> scope;
    const NativePauseInputContinuation50LA* origin=nullptr;
    const Frontend* receiver=nullptr;
    rr::platform::OriginalUiRiderProfileStamp50KG stamp{};
    rr::platform::OriginalUiMainProfile50KG commonBefore{};
    rr::game::OriginalUiCourseRememberedProfile50KG courseBefore{};
    std::shared_ptr<rr::platform::OriginalUiPauseCourseOrigin50LA> courseOrigin;
    std::uint32_t selection=0,destinationScreen=0;
    bool destinationCurrent()const noexcept {
        return (selection==9u&&destinationScreen==4u)||(selection==10u&&destinationScreen==6u);
    }
    bool courseProfileCurrent(std::uint8_t packed)const noexcept {
        if(selection!=10u)return selection==9u;
        rr::game::OriginalUiCourseRememberedProfile50KG now;
        if(!profile||!profile->courseSnapshot(now))return false;
        const auto& before=courseBefore;
        return now.ownerReady==before.ownerReady&&now.mode34==before.mode34&&
            now.course700==before.course700&&now.laps704==before.laps704&&now.entrants708==before.entrants708&&
            now.subtype70C==before.subtype70C&&now.course710==before.course710&&
            now.entrants714==before.entrants714&&now.laps718==before.laps718&&now.packed1125E==packed&&
            now.cardFlag100BC==before.cardFlag100BC&&now.nativeBetaStorage==before.nativeBetaStorage&&
            now.availability11018==before.availability11018&&now.planet0Primary79C==before.planet0Primary79C&&
            now.planet0Secondary7A0==before.planet0Secondary7A0&&now.courseOrder100C4==before.courseOrder100C4&&
            now.mawhonicRylothTest==before.mawhonicRylothTest&&now.wanMbTest==before.wanMbTest&&
            now.sebulbaTbTest==before.sebulbaTbTest&&now.scorchRcTest==before.scorchRcTest&&
            now.tzidikTcTest==before.tzidikTcTest&&now.gasganoScTest==before.gasganoScTest&&
            now.kraidGbTest==before.kraidGbTest&&now.youngAnakinMcTest==before.youngAnakinMcTest;
    }
    rr::game::OriginalWreckComponent50LA request;
    std::uint64_t generation=0,pollRevision=0;
    std::size_t eventOrdinal=0;
    std::uint32_t flags30=0,screen34=0,page38=0;
    bool enrolled=false;
    bool sameCommon(const rr::platform::OriginalUiMainProfile50KG& expected)const noexcept {
        return common&&common->known==expected.known&&common->mode34==expected.mode34&&
            common->previousMode38==expected.previousMode38&&common->savedSlot6E0==expected.savedSlot6E0&&
            common->availability11018==expected.availability11018&&common->flags11054==expected.flags11054&&
            common->playerBits1125C==expected.playerBits1125C;
    }
    bool detachedRequestCurrent()const noexcept {
        return enrolled&&request.reference00==1u&&
            request.kind==rr::game::OriginalWreckComponentKind50LA::applicationRequest2C6C20&&
            request.objectKey==std::uint64_t(reinterpret_cast<std::uintptr_t>(&request))&&
            request.sessionGeneration==generation&&!request.nativeFlags29&&request.category28==0xffu&&
            (request.flags29&15u)==3u&&request.mask20==0xffffffffu&&
            !request.parent1C&&!request.previous10&&!request.next14&&!request.firstChild18&&!request.descendants24&&
            destinationCurrent()&&flags30==6u&&screen34==destinationScreen&&page38==1u;
    }
    ~NativePauseMenuTransfer50LA(){if(request.reference00||enrolled)std::terminate();}
};
bool Application::preparePauseMenuTransfer50LA(const NativePauseInputContinuation50LA& input)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||pauseMenuTransfer50LA_||resultsHallTransfer50LA_||
       resultsInputContinuation50LA_||pauseInputContinuation50LA_.get()!=&input||
       input.phase!=NativePauseInputContinuation50LA::Phase::prepared||!pauseInputContinuationCurrent50LA()||
       !enabled||!retainPlayerWindow50DU_||!entitiesOwner_||!entitiesOwner_->application10()||
       entitiesOwner_->attachmentLock423F58()||!input.race||sourceRace28COwner_!=input.race||
       input.race->application_!=entitiesOwner_||!input.pause||input.race->pause100_!=input.pause||
       !input.camera||input.camera->actual_.humanCount34!=0u||!input.scope||
       !input.display||raceDisplayOwner50LA_.lock()!=input.display||input.race->loadDisplay_!=input.display||
       !videoOwner_||input.video!=videoOwner_||!riderProfile||!profile||!profile->known||
       riderProfile->commonProfile()!=profile.get()||input.race->profile_!=riderProfile||
       input.profile.generation!=riderProfile->stamp().generation||input.profile.mode34!=riderProfile->stamp().mode34||
       input.generation!=input.race->entity_.sessionGeneration||!input.pollRevision||
       !pauseMenuRequestTailCurrent50LA(*input.race))return false;
    try {
        auto p=std::make_shared<NativePauseMenuTransfer50LA>();
        const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&p->request));
        auto& authority=entitiesOwner_->entityAuthority_;
        if(entitiesOwner_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max()||
           !authority.reserveAdditional(1)||!authority.canEnroll(p->request,key))return false;
        p->race=input.race;p->entities=entitiesOwner_;p->profile=riderProfile;p->common=profile;p->video=input.video;
        p->pause=input.pause;p->camera=input.camera;p->display=input.display;p->scope=input.scope;
        p->origin=&input;p->generation=input.generation;p->pollRevision=input.pollRevision;p->eventOrdinal=input.eventOrdinal;
        p->stamp=input.profile;p->commonBefore=*profile;p->selection=input.selection;
        p->destinationScreen=input.selection==9u?4u:6u;
        if(!p->destinationCurrent()||(input.selection==10u&&
           (!riderProfile->courseSnapshot(p->courseBefore)||!p->courseBefore.ownerReady||p->courseBefore.mode34)))return false;
        // Publish before the actual counted constructor. A partially reached
        // prefix remains owned for terminal cancellation, never silently freed.
        pauseMenuTransfer50LA_=p;
        if(rr::game::initializeOriginalWreckComponent50LA(p->request,
            rr::game::OriginalWreckComponentKind50LA::applicationRequest2C6C20,
            p->generation,key,true,entitiesOwner_->objectCount423F60()).status!=
            rr::game::OriginalWreckComponentStatus50LA::complete)return false;
        //2C6C2C calls146520, whose141300 prefix above precedes the three
        // actual146540/44/48 request-lane clears. The concrete request kind
        // represents the final2C6C3C table3D4E20 identity, not a raw vtable.
        p->flags30=0u;p->screen34=0u;p->page38=0u;
        p->enrolled=authority.enroll(p->request);
        return p->enrolled&&pauseMenuRequestTailCurrent50LA(*p->race);
    }catch(...){return false;}
}
bool Application::commitPauseMenuTransfer50LA(const NativePauseInputContinuation50LA& input)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||pauseInputContinuation50LA_.get()!=&input||
       !pauseInputContinuationCurrent50LA())return false;
    const auto p=pauseMenuTransfer50LA_;
    if(!p||p->phase!=NativePauseMenuTransfer50LA::Phase::prepared||!p->enrolled||p->origin!=&input||
       !((input.selection==9u&&input.phase==NativePauseInputContinuation50LA::Phase::videoReturned)||
         (input.selection==10u&&input.phase==NativePauseInputContinuation50LA::Phase::closedAfterVideo))||
       p->selection!=input.selection||!p->destinationCurrent()||!input.fade.complete()||
       p->race!=input.race||p->pause.lock()!=input.pause||p->camera.lock()!=input.camera||
       p->display.lock()!=input.display||p->scope.lock()!=input.scope||p->video!=input.video||
       p->entities!=entitiesOwner_||p->profile!=riderProfile||p->common!=profile||
       p->generation!=input.generation||p->pollRevision!=input.pollRevision||p->eventOrdinal!=input.eventOrdinal||
       input.profile.generation!=p->stamp.generation||input.profile.mode34!=p->stamp.mode34||
       !p->sameCommon(p->commonBefore)||!p->courseProfileCurrent(p->courseBefore.packed1125E)||
       !pauseMenuRequestTailCurrent50LA(*p->race))return false;
    // Reached only after the SAME retained caller's actual close and real
    // blocking2AED60/260850 return, in its distinct row9/10 ordering. No boolean 'fade ready' authority.
    p->phase=NativePauseMenuTransfer50LA::Phase::committing;
    //17DE18/1C/24 write App318/31C/320, i.e. request2E8+30/+34/+38,
    // BEFORE17DE2C invokes141960(App10,request,-1). No Hall/profile writes.
    p->flags30=6u;p->screen34=p->destinationScreen;p->page38=1u;
    if(entitiesOwner_->entityAuthority_.attachChildren(*entitiesOwner_->application10(),p->request,
        0xffffffffu,entitiesOwner_->attachmentLock423F58()).status!=
        rr::game::OriginalWreckComponentStatus50LA::complete)return false;
    p->phase=NativePauseMenuTransfer50LA::Phase::queued;
    return pauseMenuRequestTailCurrent50LA(*p->race);
}
bool Application::pauseMenuRequestTailCurrent50LA(const rr::game::OriginalRaceConstructor50LA& race)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!entitiesOwner_||
       sourceRace28COwner_.get()!=&race||race.application_!=entitiesOwner_||!race.nativeRootAttached_||
       !race.nativeRootMaskReached_||!race.pause100_||!race.pause100_->entity_||
       entitiesOwner_->attachmentLock423F58()||!entitiesOwner_->entityAuthority_.canRetire(race.entity_)||
       !rr::game::original_entity_chain_detail50LA::forest(entitiesOwner_->entityAuthority_.view()))return false;
    const auto& pause=*race.pause100_->entity_;const auto& n=pause.component;
    const auto* root=entitiesOwner_->application10();
    auto* results=race.results104_?race.results104_->entity():nullptr;
    if(!root||pause.application!=entitiesOwner_||!pause.initialized||pause.releasing||
       !pause.currentForest()||n.sessionGeneration!=race.entity_.sessionGeneration||
       n.mask20!=0x1402u||n.firstChild18||n.descendants24||(n.flags29&8u)||
       race.entity_.parent1C!=root||race.entity_.mask20!=0xffffffffu||race.entity_.next14!=&n||
       n.parent1C!=root||n.previous10!=&race.entity_)return false;
    if(race.results104_&&(!results||!race.results104_->belongsTo(entitiesOwner_,race.entity_.sessionGeneration)||
       !race.results104_->loadedDisabled()||!race.results104_->retirementCurrent()||
       n.next14!=results||results->parent1C!=root||results->previous10!=&n||
       results->mask20!=0xfffffff7u))return false;
    const auto* tail=results?results:&n;
    const auto p=pauseMenuTransfer50LA_;
    if(!p)return !tail->next14;
    const auto& request=p->request;
    if(!p->destinationCurrent()||p->race.get()!=&race||p->pause.lock()!=race.pause100_||p->entities!=entitiesOwner_||!p->enrolled||
       request.kind!=rr::game::OriginalWreckComponentKind50LA::applicationRequest2C6C20||
       request.sessionGeneration!=p->generation||request.sessionGeneration!=race.entity_.sessionGeneration||
       request.objectKey!=std::uint64_t(reinterpret_cast<std::uintptr_t>(&request))||request.reference00!=1u||
       !entitiesOwner_->entityAuthority_.canRetire(request)||request.nativeFlags29||request.category28!=0xffu||
       (request.flags29&15u)!=3u||request.firstChild18||request.descendants24)return false;
    if(p->phase==NativePauseMenuTransfer50LA::Phase::prepared||p->phase==NativePauseMenuTransfer50LA::Phase::committing)
        return !tail->next14&&!request.parent1C&&!request.previous10&&!request.next14&&!request.mask20;
    return (p->phase==NativePauseMenuTransfer50LA::Phase::queued||
        p->phase==NativePauseMenuTransfer50LA::Phase::batchReturned)&&
        p->destinationCurrent()&&p->flags30==6u&&p->screen34==p->destinationScreen&&p->page38==1u&&tail->next14==&request&&
        request.parent1C==root&&request.previous10==tail&&!request.next14&&request.mask20==0xffffffffu;
}
bool Application::passPauseMenuProcessSuccessors50LA(const rr::game::OriginalRaceConstructor50LA& race)const noexcept {
    // The exact disabled Results sibling above is skipped by142350 activity.
    // Real request vtable3D4E20+64=142350 reaches its genuinely empty child
    // list. This per-event pass NEVER executes request callback38/departure.
    return pauseMenuRequestTailCurrent50LA(race);
}
bool Application::markPauseMenuBatchReturned50LA(const NativePauseInputContinuation50LA& input)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||pauseInputContinuation50LA_.get()!=&input||
       !pauseInputBatchReturned50LA(input))return false;
    const auto p=pauseMenuTransfer50LA_;
    if(!p||p->phase!=NativePauseMenuTransfer50LA::Phase::queued||p->origin!=&input||
       input.phase!=NativePauseInputContinuation50LA::Phase::queued||p->selection!=input.selection||!input.fade.complete()||
       p->race!=input.race||p->pause.lock()!=input.pause||p->camera.lock()!=input.camera||
       p->display.lock()!=input.display||p->scope.lock()!=input.scope||p->video!=input.video||
       p->pollRevision!=input.pollRevision||p->eventOrdinal!=input.eventOrdinal||
       !p->sameCommon(p->commonBefore)||!p->courseProfileCurrent(p->courseBefore.packed1125E)||
       !pauseMenuRequestTailCurrent50LA(*p->race))return false;
    // Authenticated producer retirement is after the same-event child tail
    // AND every remaining original batch event. Retained caller resets only
    // after this mark; the request does not retain raw producer/Owner pointers.
    p->origin=nullptr;p->phase=NativePauseMenuTransfer50LA::Phase::batchReturned;return true;
}
bool Application::beginNativePauseMenuDeparture50LA(bool& selected)const noexcept {
    selected=false;
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto p=pauseMenuTransfer50LA_;if(!p)return true;
    const auto display=p->display.lock();const auto camera=p->camera.lock();
    if(p->phase!=NativePauseMenuTransfer50LA::Phase::batchReturned||pauseInputContinuation50LA_||
       resultsInputContinuation50LA_||resultsHallTransfer50LA_||p->origin||
       sourceRace28COwner_!=p->race||videoOwner_!=p->video||riderProfile!=p->profile||profile!=p->common||
       p->entities!=entitiesOwner_||!display||raceDisplayOwner50LA_.lock()!=display||
       !display->canRetain()||display->application_!=this||display->video_!=p->video||
       !camera||camera->generation_!=p->generation||!p->scope.lock()||
       riderProfile->stamp().generation!=p->stamp.generation||riderProfile->stamp().mode34!=p->stamp.mode34||
       !p->sameCommon(p->commonBefore)||!p->courseProfileCurrent(p->courseBefore.packed1125E)||!p->video->host.ownerThread()||!p->video->host.created()||
       p->video->host.spectatorBatchOwner50LA_||p->video->host.retainedResultsPollOwner50LA_||
       p->video->host.sharedDialogPollRevision50KG_!=p->pollRevision||
       !pauseMenuRequestTailCurrent50LA(*p->race))return false;
    // Explicit NATIVE callback38 scheduling, like the separate native Hall
    // transfer: AFTER the entire original poll and BEFORE fresh input/update.
    // Retail2C6CD0 retains the request through17D8A0 and17DB80 then clears/
    // detaches it at2C6E70. Native ordinary Race/Pause/Results retirement proves
    // a no-extra-sibling chain, so detach NOW and retain this counted owner.
    // Real session/audio/GPU drains still run normally. This is NOT recovered
    // retail17D8A0/global-flush completion, and is not its original schedule.
    if(entitiesOwner_->entityAuthority_.detach(p->request,entitiesOwner_->attachmentLock423F58()).status!=
        rr::game::OriginalWreckComponentStatus50LA::complete)return false;
    p->phase=NativePauseMenuTransfer50LA::Phase::departing;selected=true;return true;
}
bool Application::nativePauseMenuDeparting50LA()const noexcept {
    return garageAudioOwnerThread_==std::this_thread::get_id()&&pauseMenuTransfer50LA_&&
        pauseMenuTransfer50LA_->phase==NativePauseMenuTransfer50LA::Phase::departing;
}
bool Application::cancelNativePauseMenuTransfer50LA()const noexcept {
    // Cancellation is terminal, not a fade return or source-effect rollback.
    // Actual backend cancellation and input release MUST have happened first.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||pauseInputContinuation50LA_)return false;
    const auto p=pauseMenuTransfer50LA_;if(!p)return true;
    auto& n=p->request;
    if(!p->entities||p->entities!=entitiesOwner_||n.firstChild18||n.descendants24)return false;
    if(n.parent1C&&(n.parent1C!=entitiesOwner_->application10()||
       entitiesOwner_->entityAuthority_.detach(n,entitiesOwner_->attachmentLock423F58()).status!=
       rr::game::OriginalWreckComponentStatus50LA::complete))return false;
    if(n.previous10||n.next14)return false;
    if(n.reference00&&(n.reference00!=1u||!entitiesOwner_->objectCount423F60()||
       n.kind!=rr::game::OriginalWreckComponentKind50LA::applicationRequest2C6C20||
       n.objectKey!=std::uint64_t(reinterpret_cast<std::uintptr_t>(&n))||n.sessionGeneration!=p->generation))return false;
    if(p->enrolled){if(!entitiesOwner_->entityAuthority_.retire(n))return false;p->enrolled=false;}
    if(n.reference00){n.reference00=0;--entitiesOwner_->objectCount423F60();}
    // Do not dereference origin, producer, observer or frontend after unwind.
    pauseMenuTransfer50LA_.reset();return true;
}
bool Application::finishNativePauseMenuDeparture50LA()const noexcept {
    if(!nativePauseMenuDeparting50LA()||sourceRace28COwner_||pauseInputContinuation50LA_||
       resultsInputContinuation50LA_||!raceDisplayOwner50LA_.expired()||
       !lightManager||lightManager->nativeGameplayGeneration())return false;
    const auto p=pauseMenuTransfer50LA_;const auto& n=p->request;
    if(!p->race||!p->race->rolledBack_||p->race->entity_.reference00||p->race->pause100_||p->race->results104_||
       !p->pause.expired()||!p->camera.expired()||!p->display.expired()||!p->scope.expired()||
       p->profile!=riderProfile||p->common!=profile||p->video!=videoOwner_||p->entities!=entitiesOwner_||
       !p->detachedRequestCurrent()||!entitiesOwner_->entityAuthority_.canRetire(n)||
       riderProfile->stamp().generation!=p->stamp.generation||riderProfile->stamp().mode34!=p->stamp.mode34||
       !p->sameCommon(p->commonBefore)||!p->courseProfileCurrent(p->courseBefore.packed1125E)||
       !entitiesOwner_->objectCount423F60())return false;
    p->race.reset(); // No Race/Camera/Display/Scope/Pause survives into the incoming menu.
    sourceApplicationMode22C_=1u; //2C6E10 flags2 mode store, before native UI entry.
    // Keep the real counted, enrolled but detached request until the actual destination returns.
    p->phase=NativePauseMenuTransfer50LA::Phase::receiver;return true;
}
bool Application::nativePauseMenuReceiverCurrent50LA()const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto p=pauseMenuTransfer50LA_;if(!p)return false;
    const auto& n=p->request;
    return p->phase==NativePauseMenuTransfer50LA::Phase::receiver&&!p->race&&!p->origin&&
        p->pause.expired()&&p->camera.expired()&&p->display.expired()&&p->scope.expired()&&
        !sourceRace28COwner_&&!pauseInputContinuation50LA_&&!resultsInputContinuation50LA_&&!resultsHallTransfer50LA_&&
        sourceApplicationMode22C_==1u&&p->profile==riderProfile&&p->common==profile&&
        p->video==videoOwner_&&p->entities==entitiesOwner_&&p->detachedRequestCurrent()&&
        entitiesOwner_->entityAuthority_.canRetire(n)&&
        riderProfile->stamp().generation==p->stamp.generation&&riderProfile->stamp().mode34==p->stamp.mode34&&
        p->sameCommon(p->commonBefore)&&p->courseProfileCurrent(p->courseBefore.packed1125E)&&
        lightManager&&!lightManager->nativeGameplayGeneration();
}
bool Application::nativePauseMainReceiverCurrent50LA()const noexcept {
    return nativePauseMenuReceiverCurrent50LA()&&pauseMenuTransfer50LA_->selection==9u;
}
bool Application::nativePauseCourseReceiverCurrent50LA()const noexcept {
    return nativePauseMenuReceiverCurrent50LA()&&pauseMenuTransfer50LA_->selection==10u&&
        pauseMenuTransfer50LA_->stamp.mode34==0u;
}
bool Application::bindNativePauseMenuReceiver50LA(const Frontend& frontend,
    const std::shared_ptr<Video>& video)const noexcept {
    if(!nativePauseMenuReceiverCurrent50LA()||video!=videoOwner_||frontend.materialApplicationOwner!=this||
       frontend.nativeResultsHallEntry_||frontend.nativePauseMainEntry_==frontend.nativePauseCourseEntry_)return false;
    const auto p=pauseMenuTransfer50LA_;
    if(p->receiver||frontend.nativePauseCourseEntry_!=(p->selection==10u))return false;
    p->receiver=&frontend;
    if(p->selection==10u){
        try { p->courseOrigin=std::shared_ptr<rr::platform::OriginalUiPauseCourseOrigin50LA>(
            new rr::platform::OriginalUiPauseCourseOrigin50LA(
                rr::platform::OriginalUiPauseCourseOrigin50LA::Issuance{},*this,frontend,p->stamp)); }
        catch(...){return false;}
    }
    return true;
}
std::shared_ptr<rr::platform::OriginalUiPauseCourseOrigin50LA>
Application::pauseCourseReceiverOrigin50LA(const Frontend& frontend)const noexcept {
    if(!nativePauseCourseReceiverCurrent50LA()||pauseMenuTransfer50LA_->receiver!=&frontend)return {};
    return pauseMenuTransfer50LA_->courseOrigin;
}
bool Application::nativePauseCourseOriginCurrent50LA(const rr::platform::OriginalUiPauseCourseOrigin50LA& origin,
    const rr::platform::OriginalUiCourseSession50KG& session)const noexcept {
    if(!nativePauseCourseReceiverCurrent50LA())return false;
    const auto p=pauseMenuTransfer50LA_;
    // This is the exact allocated incoming Course, even during bind before its
    // own storage is installed. Neither a copied Cold input nor previousScreen
    // can manufacture this request or redirect its one-shot session binding.
    return p->courseOrigin.get()==&origin&&origin.root_==this&&origin.frontend_==p->receiver&&
        p->receiver&&p->receiver->materialApplicationOwner==this&&p->receiver->nativePauseCourseEntry_&&
        !p->receiver->nativePauseMainEntry_&&!p->receiver->nativeResultsHallEntry_&&
        p->receiver->coordinator.courseSession()==&session&&
        (!origin.session_||origin.session_==&session)&&!origin.entryCommitted_&&
        origin.preimage_.generation==p->stamp.generation&&origin.preimage_.mode34==p->stamp.mode34;
}
bool Application::acceptNativePauseMenuReceiver50LA(const Frontend& frontend,
    const rr::platform::VehicleWindowGl50DS& renderer)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto p=pauseMenuTransfer50LA_;const auto display=raceDisplayOwner50LA_.lock();
    if(!p||p->phase!=NativePauseMenuTransfer50LA::Phase::receiver||p->receiver!=&frontend||
       frontend.materialApplicationOwner!=this||frontend.nativeResultsHallEntry_||
       frontend.nativePauseMainEntry_==frontend.nativePauseCourseEntry_||
       frontend.nativePauseCourseEntry_!=(p->selection==10u)||
       p->race||p->origin||!p->pause.expired()||!p->camera.expired()||!p->display.expired()||!p->scope.expired()||
       sourceRace28COwner_||pauseInputContinuation50LA_||resultsInputContinuation50LA_||resultsHallTransfer50LA_||
       sourceApplicationMode22C_!=1u||p->profile!=riderProfile||p->common!=profile||
       p->video!=videoOwner_||p->entities!=entitiesOwner_||!p->detachedRequestCurrent()||
       !display||!display->frontendOwnerCurrent50KG()||
       display->application_!=this||display->video_!=p->video||&display->renderer_!=&renderer||
       !frontend.coordinator.borrowsRenderer(renderer)||
       !entitiesOwner_->entityAuthority_.canRetire(p->request)||!entitiesOwner_->objectCount423F60())return false;
    const auto actual=frontend.coordinator.snapshot();
    if(!actual.prepared||!actual.active||actual.faulted||actual.quitRequested||actual.deferredQueue||
       actual.route.screen!=p->destinationScreen||actual.route.page!=1u||!actual.transition.bound||
       actual.transition.current.screen!=p->destinationScreen||actual.transition.current.page!=1u||
       !actual.lightManagerEntered)return false;
    if(p->selection==9u){
        if(!frontend.coordinator.mainSession()||p->courseOrigin)return false;
        auto expected=p->commonBefore;
        // Actual Main pageEntry74 changes only previous mode and mode4.
        if(expected.mode34!=4u){expected.previousMode38=expected.mode34;expected.mode34=4u;}
        if(!p->sameCommon(expected)||riderProfile->stamp().mode34!=4u||
           riderProfile->stamp().generation!=p->stamp.generation)return false;
    }else{
        const auto* course=frontend.coordinator.courseSession();const auto& origin=p->courseOrigin;
        if(!course||!origin||origin->root_!=this||origin->frontend_!=&frontend||origin->session_!=course||
           !origin->entryCommitted_||origin->preimage_.generation!=p->stamp.generation||
           origin->preimage_.mode34!=0u||p->stamp.mode34!=0u||!p->sameCommon(p->commonBefore)||
           riderProfile->stamp().mode34!=0u||p->stamp.generation==std::numeric_limits<std::uint64_t>::max()||
           riderProfile->stamp().generation!=p->stamp.generation+1u)return false;
        const auto entry=course->snapshot();
        if(!entry.bound||entry.faulted||entry.quitRequested||!entry.pageShown||!entry.cameraActive||
           entry.mode34!=0u||entry.popupEnabled438||entry.selectedPlanet40>=6u||
           entry.courseSlot14E4!=p->courseBefore.course700||entry.subtypeSlot14F4!=p->courseBefore.subtype70C||
           entry.laps14EC!=p->courseBefore.laps704||entry.entrants14F0!=p->courseBefore.entrants708)return false;
        const auto packed=static_cast<std::uint8_t>((p->courseBefore.packed1125E&0x1fu)|(entry.selectedPlanet40<<5));
        if(!p->courseProfileCurrent(packed))return false;
    }
    p->phase=NativePauseMenuTransfer50LA::Phase::accepting;
    p->flags30=0u; //2C6E74 follows the actual bound destination, never preparation.
    if(!entitiesOwner_->entityAuthority_.retire(p->request))return false;
    p->enrolled=false;p->request.reference00=0;--entitiesOwner_->objectCount423F60();
    pauseMenuTransfer50LA_.reset();return true;
}
