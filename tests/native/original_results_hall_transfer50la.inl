// Included only by original_results104_input50la.cpp, after the retained input
// definition. Native later adaptation of App2E8/2C6C20, not17C684 boot history.
struct NativeResultsHallTransfer50LA final {
    enum class Phase { prepared,committing,queued,pollReturned,departing,receiver };
    Phase phase=Phase::prepared;
    std::shared_ptr<rr::game::OriginalRaceConstructor50LA> race;
    std::shared_ptr<rr::game::OriginalApplicationEntities50LA> entities;
    std::shared_ptr<rr::platform::OriginalUiRiderProfileOwner50KG> profile;
    std::shared_ptr<rr::platform::OriginalUiMainProfile50KG> common;
    std::shared_ptr<Video> video;
    rr::platform::OriginalUiRiderProfileStamp50KG stamp{};
    rr::game::OriginalWreckComponent50LA request;
    std::uint32_t flags30=0,screen34=0,page38=0;
    bool enrolled=false;
    // 1AB5BC request (common race acknowledgement) instead of 1CA6F0's Hall.
    bool acknowledgement1AB5BC=false;
    rr::game::OriginalRaceAckRoute50PD route=rr::game::OriginalRaceAckRoute50PD::hall;
    bool routeCurrent()const noexcept {
        if(flags30!=6u||page38!=1u)return false;
        if(!acknowledgement1AB5BC)return screen34==11u;
        // 13 Garage, 10 Unlock, 11 Hall, 7 Win, 6 Course (1AB5BC rows); 4 Main
        // menu: Pause row 9 1BDA64 17DE10(App,6,4,1) (common_race_end50CR depart).
        return screen34==13u||screen34==10u||screen34==11u||screen34==7u||screen34==6u||screen34==4u;
    }
    ~NativeResultsHallTransfer50LA(){if(request.reference00||enrolled)std::terminate();}
};
bool Application::prepareResultsHallTransfer50LA(rr::game::OriginalRaceConstructor50LA& race)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||resultsHallTransfer50LA_||
       resultsInputContinuation50LA_||sourceRace28COwner_.get()!=&race||!enabled||
       !retainPlayerWindow50DU_||!videoOwner_||!riderProfile||!profile||
       riderProfile->commonProfile()!=profile.get()||race.profile_!=riderProfile||
       (race.flags1B0KnownMask_&3u)!=3u||(race.flags1B0KnownValue_&3u)!=1u||
       riderProfile->stamp().mode34!=0u||!riderProfile->hallRecordsReady())return false;
    std::uint32_t subtype=0;
    rr::game::OriginalResults104Diagnostic50LA d;
    if(!riderProfile->resultsSubtype11258(subtype)||subtype!=3u||!resultsPublishedRoot50LA(race,d))return false;
    try {
        auto p=std::make_shared<NativeResultsHallTransfer50LA>();
        const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&p->request));
        auto& authority=entitiesOwner_->entityAuthority_;
        if(entitiesOwner_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max()||
           !authority.reserveAdditional(1)||!authority.canEnroll(p->request,key))return false;
        p->race=sourceRace28COwner_;p->entities=entitiesOwner_;p->profile=riderProfile;p->common=profile;p->video=videoOwner_;
        p->stamp=riderProfile->stamp();
        // Retain the actual constructed prefix even if enrollment fails.
        resultsHallTransfer50LA_=p;
        if(rr::game::initializeOriginalWreckComponent50LA(p->request,
            rr::game::OriginalWreckComponentKind50LA::applicationRequest2C6C20,
            race.entity_.sessionGeneration,key,true,entitiesOwner_->objectCount423F60()).status!=
            rr::game::OriginalWreckComponentStatus50LA::complete)return false;
        p->enrolled=authority.enroll(p->request);
        return p->enrolled;
    }catch(...){return false;}
}
bool Application::commitResultsHallTransfer50LA(rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::OriginalRaceRecordInsertionStage50KG& stage,
    rr::platform::OriginalSaveProfileOwner50KG& prepared)const noexcept {
    if(!resultsInputContinuationCurrent50LA())return false;
    const auto p=resultsHallTransfer50LA_;const auto input=resultsInputContinuation50LA_;
    if(!p||p->phase!=NativeResultsHallTransfer50LA::Phase::prepared||!p->enrolled||
       p->race.get()!=&race||p->entities!=entitiesOwner_||p->profile!=riderProfile||p->common!=profile||p->video!=videoOwner_||
       input->sourceCallPC!=0x1ca678u||!input->fade.complete()||
       input->profile.generation!=p->stamp.generation||input->profile.mode34!=p->stamp.mode34||
       stage.secondHumanRanks()||stage.profileMode34()!=0u||
       (race.endEntryProfile_&&(race.endEntryProfile_->generation!=p->stamp.generation||
        race.endEntryProfile_->mode34!=p->stamp.mode34)))return false;
    // No callback or foreign mutation between either private write and the
    // corresponding exact immediate-descendant stamp reconciliation.
    p->phase=NativeResultsHallTransfer50LA::Phase::committing;
    rr::platform::OriginalUiRiderProfileStamp50KG after;
    if(!p->profile->commitSingleHumanHallRecords1AC440(stage,prepared,p->stamp,after))return false;
    if(after.generation!=p->stamp.generation+1u||after.mode34!=p->stamp.mode34)return false;
    p->stamp=after;input->profile=after;if(race.endEntryProfile_)race.endEntryProfile_=after;
    if(!p->profile->markResultsHallDirty1CA6E4(p->stamp))return false;
    after=p->profile->stamp();
    if(after.generation!=p->stamp.generation+1u||after.mode34!=p->stamp.mode34)return false;
    p->stamp=after;input->profile=after;if(race.endEntryProfile_)race.endEntryProfile_=after;
    //17DE10 writes the three actual request lanes BEFORE141960 append.
    p->flags30=6u;p->screen34=11u;p->page38=1u;
    if(entitiesOwner_->entityAuthority_.attachChildren(*entitiesOwner_->application10(),p->request,
        0xffffffffu,entitiesOwner_->attachmentLock423F58()).status!=
        rr::game::OriginalWreckComponentStatus50LA::complete)return false;
    p->phase=NativeResultsHallTransfer50LA::Phase::queued;
    return resultsRequestTailCurrent50LA(race);
}
bool Application::resultsRequestTailCurrent50LA(const rr::game::OriginalRaceConstructor50LA& race)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto p=resultsHallTransfer50LA_;
    if(!p)return race.results104_&&race.results104_->entity()&&!race.results104_->entity()->next14;
    const auto* results=race.results104_?race.results104_->entity():nullptr;
    if(!results||p->race.get()!=&race||p->entities!=entitiesOwner_||!p->enrolled)return false;
    if(p->phase==NativeResultsHallTransfer50LA::Phase::prepared||
       p->phase==NativeResultsHallTransfer50LA::Phase::committing)
        return !results->next14&&!p->request.parent1C&&!p->request.previous10&&!p->request.next14;
    return (p->phase==NativeResultsHallTransfer50LA::Phase::queued||
        p->phase==NativeResultsHallTransfer50LA::Phase::pollReturned)&&
        p->routeCurrent()&&p->request.reference00==1u&&
        results->next14==&p->request&&p->request.previous10==results&&
        p->request.parent1C==entitiesOwner_->application10()&&!p->request.next14&&
        p->request.mask20==0xffffffffu&&!p->request.firstChild18&&!p->request.descendants24;
}
bool Application::passResultsProcessSuccessors50LA(const rr::game::OriginalRaceConstructor50LA& race)const noexcept {
    if(!resultsRequestTailCurrent50LA(race))return false;
    const auto p=resultsHallTransfer50LA_;
    // Real request vtable3D4E20+64=142350: its owned child list is empty.
    // No callback38/departure is executed inside this per-event traversal.
    return !p||p->phase!=NativeResultsHallTransfer50LA::Phase::queued||
        ((p->request.flags29&9u)==1u&&!p->request.firstChild18&&!p->request.descendants24);
}
bool Application::beginNativeResultsHallDeparture50LA(bool& selected)const noexcept {
    selected=false;
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto p=resultsHallTransfer50LA_;if(!p)return true;
    if(p->phase!=NativeResultsHallTransfer50LA::Phase::pollReturned||resultsInputContinuation50LA_||
       sourceRace28COwner_!=p->race||videoOwner_!=p->video||riderProfile!=p->profile||profile!=p->common||
       riderProfile->stamp().generation!=p->stamp.generation||riderProfile->stamp().mode34!=p->stamp.mode34||
       !resultsRequestTailCurrent50LA(*p->race))return false;
    // Explicit native callback38 scheduling: AFTER the entire retained poll,
    // BEFORE new input/update. Consume/detach before the existing native close
    // sequence so its proven Race/Pause/Results topology remains unchanged.
    // Real GPU/resource drains and Race destruction still run in closeWindow;
    // this is not a claim that retail17D8A0 or a global flush returned.
    if(entitiesOwner_->entityAuthority_.detach(p->request,entitiesOwner_->attachmentLock423F58()).status!=
        rr::game::OriginalWreckComponentStatus50LA::complete)return false;
    p->phase=NativeResultsHallTransfer50LA::Phase::departing;selected=true;return true;
}
bool Application::nativeResultsHallDeparting50LA()const noexcept {
    return garageAudioOwnerThread_==std::this_thread::get_id()&&resultsHallTransfer50LA_&&
        resultsHallTransfer50LA_->phase==NativeResultsHallTransfer50LA::Phase::departing;
}
bool Application::cancelNativeResultsHallTransfer50LA()const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||resultsInputContinuation50LA_)return false;
    const auto p=resultsHallTransfer50LA_;if(!p)return true;
    auto& n=p->request;
    if(n.firstChild18||n.descendants24||!p->entities||p->entities!=entitiesOwner_)return false;
    if(n.parent1C&&entitiesOwner_->entityAuthority_.detach(n,entitiesOwner_->attachmentLock423F58()).status!=
       rr::game::OriginalWreckComponentStatus50LA::complete)return false;
    if(n.previous10||n.next14)return false;
    if(n.reference00&&(n.reference00!=1u||!entitiesOwner_->objectCount423F60()))return false;
    if(p->enrolled){if(!entitiesOwner_->entityAuthority_.retire(n))return false;p->enrolled=false;}
    if(n.reference00){
        if(n.reference00!=1u||!entitiesOwner_->objectCount423F60())return false;
        n.reference00=0;--entitiesOwner_->objectCount423F60();
    }
    resultsHallTransfer50LA_.reset();return true;
}
bool Application::finishNativeResultsHallDeparture50LA()const noexcept {
    if(!nativeResultsHallDeparting50LA()||sourceRace28COwner_||resultsInputContinuation50LA_||
       !raceDisplayOwner50LA_.expired()||!lightManager||lightManager->nativeGameplayGeneration())return false;
    const auto p=resultsHallTransfer50LA_;
    if(!p->race||!p->race->rolledBack_||p->race->entity_.reference00||p->profile!=riderProfile||p->video!=videoOwner_||
       !p->enrolled||p->request.parent1C||p->request.previous10||p->request.next14||
       p->request.firstChild18||p->request.descendants24||
       riderProfile->stamp().generation!=p->stamp.generation||riderProfile->stamp().mode34!=p->stamp.mode34||
       !entitiesOwner_->objectCount423F60())return false;
    if(!entitiesOwner_->entityAuthority_.retire(p->request))return false;
    p->enrolled=false;p->request.reference00=0;--entitiesOwner_->objectCount423F60();
    p->race.reset(); // No Race survives into the receiver; no ownership cycle.
    sourceApplicationMode22C_=1u; // Actual flags2 native UI entry boundary.
    p->phase=NativeResultsHallTransfer50LA::Phase::receiver;return true;
}
bool Application::nativeResultsHallReceiverCurrent50LA()const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto p=resultsHallTransfer50LA_;
    return p&&p->phase==NativeResultsHallTransfer50LA::Phase::receiver&&!p->race&&!p->enrolled&&
        !p->request.reference00&&!sourceRace28COwner_&&!resultsInputContinuation50LA_&&
       p->profile==riderProfile&&p->common==profile&&p->video==videoOwner_&&p->entities==entitiesOwner_&&
        p->routeCurrent()&&
        riderProfile->stamp().generation==p->stamp.generation&&riderProfile->stamp().mode34==p->stamp.mode34;
}
// acceptNativeResultsHallReceiver50LA (the separate Results Hall entrance's mode5 entry
// commit) is deleted with that entrance; acceptNativeRaceReturnReceiver50PD consumes every
// retained race departure, the Results Hall included.
bool Application::commitRaceAcknowledgementTransfer50PD(rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::OriginalRaceAckDestination50PD& route)const noexcept {
    // 1AB5BC -> 17DE10(App,6,screen,1): store the three request lanes, then
    // 141960 appends the launcher to App10. Its tick (2C6CD0) runs after this
    // update: flag4 destroys the race, flag2 changes screen through ld_ui.
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||resultsHallTransfer50LA_||
       resultsInputContinuation50LA_||sourceRace28COwner_.get()!=&race||!enabled||
       !retainPlayerWindow50DU_||!videoOwner_||!riderProfile||!profile||
       riderProfile->commonProfile()!=profile.get()||race.profile_!=riderProfile||
       route.flags!=6u||route.page!=1u){
        std::fprintf(stderr,"race_ack_transfer rejected=owners thread=%d prior=%d input=%d race=%d\n",
            int(garageAudioOwnerThread_!=std::this_thread::get_id()),int(bool(resultsHallTransfer50LA_)),
            int(bool(resultsInputContinuation50LA_)),int(sourceRace28COwner_.get()!=&race));
        return false;}
    rr::game::OriginalResults104Diagnostic50LA d;
    // Results is confirmed and inactive (flags29 bit0 clear) by 1AB5BC, but
    // still attached after Pause on App10: same topology, no update admission.
    if(!resultsPublishedRoot50LA(race,d,false)){
        std::fprintf(stderr,"race_ack_transfer rejected=results_root pc=%x op=%s\n",d.sourcePC,d.operation?d.operation:"");
        return false;}
    const auto reject=[](const char* why)noexcept{std::fprintf(stderr,"race_ack_transfer rejected=%s\n",why);return false;};
    try {
        auto p=std::make_shared<NativeResultsHallTransfer50LA>();
        p->acknowledgement1AB5BC=true;p->route=route.route;
        p->flags30=route.flags;p->screen34=route.screen;p->page38=route.page;
        if(!p->routeCurrent()){p->flags30=0;return false;}
        p->flags30=0;p->screen34=0;p->page38=0;
        const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&p->request));
        auto& authority=entitiesOwner_->entityAuthority_;
        if(entitiesOwner_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max()||
           !authority.reserveAdditional(1)||!authority.canEnroll(p->request,key))return reject("enroll_capacity");
        p->race=sourceRace28COwner_;p->entities=entitiesOwner_;p->profile=riderProfile;p->common=profile;p->video=videoOwner_;
        p->stamp=riderProfile->stamp();
        resultsHallTransfer50LA_=p; // Retain the constructed prefix even if a later step fails.
        if(rr::game::initializeOriginalWreckComponent50LA(p->request,
            rr::game::OriginalWreckComponentKind50LA::applicationRequest2C6C20,
            race.entity_.sessionGeneration,key,true,entitiesOwner_->objectCount423F60()).status!=
            rr::game::OriginalWreckComponentStatus50LA::complete)return reject("component_init");
        p->enrolled=authority.enroll(p->request);
        if(!p->enrolled)return reject("enroll");
        p->flags30=route.flags;p->screen34=route.screen;p->page38=route.page;
        if(authority.attachChildren(*entitiesOwner_->application10(),p->request,
            0xffffffffu,entitiesOwner_->attachmentLock423F58()).status!=
            rr::game::OriginalWreckComponentStatus50LA::complete)return reject("attach");
        // No Results input poll is in flight here: the request is due at the
        // launcher's tick, i.e. the next native departure point.
        p->phase=NativeResultsHallTransfer50LA::Phase::pollReturned;
        if(resultsRequestTailCurrent50LA(race))return true;
        const auto* results=race.results104_?race.results104_->entity():nullptr;
        std::fprintf(stderr,"race_ack_transfer rejected=tail results=%d next_is_request=%d request_prev_is_results=%d parent=%d next=%d ref=%u\n",
            int(results!=nullptr),int(results&&results->next14==&p->request),int(p->request.previous10==results),
            int(p->request.parent1C==entitiesOwner_->application10()),int(p->request.next14!=nullptr),unsigned(p->request.reference00));
        return false;
    }catch(...){return reject("exception");}
}
// One receiver for every 17DE10(App,6,screen,1) race departure. The 1CA6F0 Results
// Hall (Time Trial record, incl. Pause QUIT -> 1ABFC0 records page) and the 1AB5BC
// acknowledgement rows write the same request lanes (flags 6, screen, page 1), and
// 17DB80/ld_ui writes no mode, so both enter the same race-return first screen
// (here Hall11 with P+34 unchanged), with the same shared-dialog/dispatcher boot.
// The former separate 1CA6F0 receiver (prepareNativeResultsHall: Hall entered in
// Options mode5, no 1B7BD0 boot; cr-26: its frontend reached the Root audio prefix with
// the previous lifetime's closed dispatcher) is deleted.
bool Application::nativeRaceReturnRoute50PD(std::uint32_t& screen,std::uint32_t& page)const noexcept {
    screen=0;page=0;
    const auto p=resultsHallTransfer50LA_;
    if(!p||!nativeResultsHallReceiverCurrent50LA())return false;
    screen=p->screen34;page=p->page38;return true;
}
bool Application::acceptNativeRaceReturnReceiver50PD()const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto p=resultsHallTransfer50LA_;
    if(!p||p->phase!=NativeResultsHallTransfer50LA::Phase::receiver||
       p->profile!=riderProfile||p->common!=profile||p->video!=videoOwner_||sourceRace28COwner_||
       p->request.reference00||p->enrolled||!riderProfile||
       riderProfile->stamp().mode34!=p->stamp.mode34)return false;
    p->flags30=0u;resultsHallTransfer50LA_.reset();return true;
}
// 1AB5BC record row: tournament 1AB680 / other modes 1AB838 call
// 1AC440(App28C) immediately before 17DE10(App,6,11,1). Same single-human
// stage/provenance/commit as the 1CA6F0 Hall (1CA678), but its operands are
// this race's own manager row0 read through the race's lexical Camera/Bank/
// manager borrow, as Registration::continuePreAck1AB520 binds it. Call ONCE,
// right after the destination is decided and before
// commitRaceAcknowledgementTransfer50PD; there is no replay guard beyond the
// absent transfer, so a false return must stop the caller.
bool Application::commitRaceAcknowledgementRecords1AC440_50PD(rr::game::OriginalRaceConstructor50LA& race,
    const rr::game::OriginalRaceAckDestination50PD& route)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||resultsHallTransfer50LA_||
       resultsInputContinuation50LA_||sourceRace28COwner_.get()!=&race||!enabled||
       !riderProfile||!profile||riderProfile->commonProfile()!=profile.get()||race.profile_!=riderProfile||
       route.route!=rr::game::OriginalRaceAckRoute50PD::hall||!route.insertRecords1AC440||
       !race.endAcknowledgement20C_||!*race.endAcknowledgement20C_||!race.counted_||race.rolledBack_||
       !race.results104_||!standardMaterialDomainOwner_||!riderProfile->hallRecordsReady()){
        std::fprintf(stderr,"ack_1ac440_refused line=%d thread=%d hallTransfer=%d inputContinuation=%d race=%d enabled=%d profile=%d route=%d ack=%d counted=%d results=%d domain=%d hall=%d\n",__LINE__,
            int(garageAudioOwnerThread_==std::this_thread::get_id()),int(bool(resultsHallTransfer50LA_)),int(bool(resultsInputContinuation50LA_)),
            int(sourceRace28COwner_.get()==&race),int(enabled),int(riderProfile&&profile&&riderProfile->commonProfile()==profile.get()&&race.profile_==riderProfile),
            int(route.route==rr::game::OriginalRaceAckRoute50PD::hall&&route.insertRecords1AC440),int(race.endAcknowledgement20C_&&*race.endAcknowledgement20C_),
            int(race.counted_&&!race.rolledBack_),int(bool(race.results104_)),int(bool(standardMaterialDomainOwner_)),int(riderProfile&&riderProfile->hallRecordsReady()));
        return false;}
    // The row must still be the live 1AB5BC decision (P+34, 102CC, 7E4 bit0).
    rr::game::OriginalRaceAckFacts50PD facts;
    if(!riderProfile->raceAcknowledgementFacts1AB5BC(facts)||!facts.record7E4){std::fprintf(stderr,"ack_1ac440_refused line=%d\n",__LINE__);return false;}
    const auto live=rr::game::originalRaceAckDestination1AB5BC(facts);
    if(live.route!=route.route||live.sourcePC!=route.sourcePC||!live.insertRecords1AC440){std::fprintf(stderr,"ack_1ac440_refused line=%d\n",__LINE__);return false;}
    //1AC4EC: race1B0 low2 == 1. The two-human 1AC5B4 branch is not reached natively.
    if((race.flags1B0KnownMask_&3u)!=3u||(race.flags1B0KnownValue_&3u)!=1u){std::fprintf(stderr,"ack_1ac440_refused line=%d\n",__LINE__);return false;}
    try {
        const auto scope=race.endHud_.lock();
        if(!scope||scope->thread_!=std::this_thread::get_id()||!scope->active_||!scope->published_||
           !scope->hud_||!scope->manager_||!scope->bank_||!scope->camera_||scope->race_.lock().get()!=&race||
           scope->application_!=this||race.entity_.sessionGeneration!=scope->generation_||!race.world74_){std::fprintf(stderr,"ack_1ac440_refused line=%d\n",__LINE__);return false;}
        // Every race: the counted race's own camera adapter34 (no RA aggregate borrow).
        const original_camera_session50KX::Session* camera=race.cameraNativeAdapter34_;
        if(!camera||camera!=scope->camera_||&camera->actual_.bank!=scope->bank_||camera->actual_.raceHud!=scope->hud_||
           camera->generation_!=scope->generation_||camera->actual_.applicationEntities!=race.application_){std::fprintf(stderr,"ack_1ac440_refused line=%d\n",__LINE__);return false;}
        using Observation=original_position_stats_adapter50LA::Owner;
        Observation observer;Observation::Diagnostic observed;
        if(!observer.bind(*camera,*scope->manager_,observed)||
           !race.results104_->endDispatchOwnerCurrent1AB520(race,observer)){std::fprintf(stderr,"ack_1ac440_refused line=%d\n",__LINE__);return false;}
        // Results itself was loaded from this same canonical domain (1BEEB0).
        const auto* materials=standardMaterialDomainOwner_->materials382B38().get();
        const auto generation=race.entity_.sessionGeneration;
        Observation::EndObservation current;Observation::ResultsRecordIdentity row;
        rr::game::OriginalRaceRecordRacerObservation50KG values{};
        //1AC4F0..: 1AF330(managerB0,0) +CB4 / +CB8 / +CD0.
        if(!observer.captureEndObservation1AB520(race.application_.get(),materials,generation,current,observed)||
           !observer.captureResultsRecordIdentity(race.application_.get(),materials,generation,0,row,observed)||
           row.publication!=current.publication||
           !observer.readResultsBestLapCB4(row,values.bestLapCB4,observed)||
           !observer.readResultsRaceTimeCB8(row,values.timeCB8,observed)||
           !observer.readResultsObservedTimingCD0(row,values.countCD0,observed)){std::fprintf(stderr,"ack_1ac440_refused line=%d\n",__LINE__);return false;}
        // Rows still current; nothing runs between this check and the store.
        if(!observer.resultsRowsCurrent(race.application_.get(),materials,generation,current.publication)){std::fprintf(stderr,"ack_1ac440_refused line=%d\n",__LINE__);return false;}
        rr::game::OriginalRaceRecordInsertionStage50KG::Diagnostic insertion;
        const auto before=riderProfile->stamp();
        if(before.mode34!=facts.mode34){std::fprintf(stderr,"ack_1ac440_refused line=%d\n",__LINE__);return false;}
        rr::platform::OriginalUiRiderProfileStamp50KG after;
        if(!riderProfile->commitSingleHumanRaceRecords1AC440(values,1u,before,after,insertion)||
           after.generation!=before.generation+1u||after.mode34!=before.mode34){std::fprintf(stderr,"ack_1ac440_refused line=%d insertion=%u/%x\n",__LINE__,
            unsigned(insertion.status),insertion.sourcePC);return false;}
        if(race.endEntryProfile_&&race.endEntryProfile_->generation==before.generation&&
           race.endEntryProfile_->mode34==before.mode34)race.endEntryProfile_=after;
        //1AB848..1AB878: non-tournament 100BC bit0 store AFTER 1AC440. The
        // tournament row's 1AB648 store already ran in commitTournamentRaceResult.
        if(facts.mode34!=2u&&route.savePending1AB648){
            rr::platform::OriginalUiRiderProfileStamp50KG saved;
            if(!riderProfile->markAcknowledgementSavePending1AB648(after,saved)){std::fprintf(stderr,"ack_1ac440_refused line=%d\n",__LINE__);return false;}
        }
        return true;
    }catch(...){return false;}
}
