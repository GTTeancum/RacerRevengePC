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
        p->flags30==6u&&p->screen34==11u&&p->page38==1u&&p->request.reference00==1u&&
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
        p->flags30==6u&&p->screen34==11u&&p->page38==1u&&
        riderProfile->stamp().generation==p->stamp.generation&&riderProfile->stamp().mode34==p->stamp.mode34;
}
bool Application::acceptNativeResultsHallReceiver50LA()const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    const auto p=resultsHallTransfer50LA_;
    // The real coordinator's mode-entry commit is the ONLY permitted change.
    if(!p||p->phase!=NativeResultsHallTransfer50LA::Phase::receiver||p->profile!=riderProfile||p->common!=profile||
       p->video!=videoOwner_||sourceRace28COwner_||p->request.reference00||p->enrolled||
       !profile||profile->previousMode38!=0u||profile->mode34!=5u||
       riderProfile->stamp().mode34!=5u||riderProfile->stamp().generation!=p->stamp.generation)return false;
    p->flags30=0u;resultsHallTransfer50LA_.reset();return true;
}
