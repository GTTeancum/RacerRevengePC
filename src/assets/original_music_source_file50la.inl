// Included inside rr::assets after complete resolver storage and FileManager helpers.
struct OriginalMusicSourceFile50LA::Storage {
    std::unique_ptr<R::FreshFileCapacity::Handle> pendingHandle;
    // Retains even incompletely read/authenticated native input on refusal.
    std::shared_ptr<std::vector<std::uint8_t>> pendingInput;
    const char *memberNameIdentity=nullptr,*deviceNameIdentity=nullptr; //Exact native allocations, historical after cancellation.
    bool deviceBodyPublished=false; //Payload/table lanes remain unwritten if device-name allocation fails.

    //Native custody observations around the fixed read/seek, NOT receipts that
    //every inner source stage ran. These read/seek helpers cannot realloc rows.
    struct ProbeSnapshot {
        std::uint32_t memberCursor=0,deviceCursor=0,deviceFlags=0;
        const R::FreshFileCapacity::Handle* handleIdentity=nullptr;
        std::optional<std::size_t> physicalCursor;
        std::int32_t scratchIndex=-1,scratchOffset=0;
        std::int64_t scratchStatus=0;
        const std::uint8_t* scratchPointer=nullptr;
        std::optional<std::uint32_t> rawValidBytes;
    } beforeProbe,probe,beforeTextSeek,textSeek;
    struct LineSnapshot {
        ProbeSnapshot own;
        std::array<std::int32_t,4> active;
        std::int32_t current74,window7C,window80;
    } beforeLine,afterActivation,line,beforeSuffix,afterSuffix;
    struct Close {
        const void *handleIdentity=nullptr,*deviceNameIdentity=nullptr,*memberNameIdentity=nullptr;
        //Selected13D9F0 stores on the original handle, not a fabricated full
        //guest handle. The existing native Handle retains its physical input.
        std::optional<std::uint32_t> handle28,handle24;
        std::optional<std::int32_t> handleReturn;
        bool handleFreeEntered=false,handleFreeReturned=false;
        bool deviceEntered=false,deviceNameFreeEntered=false,deviceNameFreeReturned=false,deviceReturned=false;
        bool memberEntered=false,memberNameFreeEntered=false,memberNameFreeReturned=false,memberReturned=false;
    } close;
};
OriginalMusicSourceFile50LA::OriginalMusicSourceFile50LA(
    const original_main_entry50KG::Application* issuer,const rr::game::OriginalRaceMusicManager50LA* manager)noexcept
    :OriginalMusicSourceFile50LA(issuer,manager,Occurrence::raceMusic2C0B98){}
OriginalMusicSourceFile50LA::OriginalMusicSourceFile50LA(
    const original_main_entry50KG::Application* issuer,const rr::game::OriginalRaceMusicManager50LA* manager,Occurrence occurrence)noexcept
    :occurrence_(occurrence),issuer_(issuer),manager_(manager){}
std::string_view OriginalMusicSourceFile50LA::sourceNameNative()const noexcept {
    return occurrence_==Occurrence::announcerPercentages2C5164?"audio\\spchpct.ini":"audio\\racemus.jub";
}
OriginalMusicSourceFile50LA::~OriginalMusicSourceFile50LA()noexcept {
    if(acquired_&&!nativeClosed_)std::terminate();
}
bool OriginalMusicSourceFile50LA::begin131230(std::shared_ptr<R> resolver,const void* config,
    const std::filesystem::path& root,std::string_view name,std::int64_t& returned,D& d)noexcept try {
    d={};using C=R::FreshFileCapacity;
    if(!issuer_||!manager_||!config||!resolver||thread_!=std::this_thread::get_id()||attempted_||nativeClosed_||
       name!=sourceNameNative()||root.empty()||!resolver->freshFileCapacity_||!resolver->scratch_)
        return fail(d,E::phase,0x131230);
    auto& s=*resolver->freshFileCapacity_;
    //Symmetric exclusion for BOTH Music and Announcer occurrences while the
    //independent Pool whole-table pin is live. No legacy branch changes.
    if(s.waterPoolFileBorrower)return fail(d,E::phase,0x131230);
    if(s.streamLibraryOwner||s.musicOwner||!s.members150||s.memberCount154<=0||!s.devices14||s.deviceCount18<=0||s.read174||
       s.registration!=C::Registration::complete||s.cursor168< -1||s.cursor168>=s.memberCount154||
       s.failedCourseDevices||s.failedCourseMembers)return fail(d,E::phase,0x131230);
    storage_=std::make_unique<Storage>();audioRoot_=root;resolver_=std::move(resolver);config_=config;
    attempted_=true;acquired_=true;s.musicOwner=this;
    //1315B0 always precedes170. No132820 cached160 shortcut or bank filter.
    searchCursorBefore_=s.cursor168;searchCount_=s.memberCount154;
    ++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0;
    auto n=static_cast<std::int32_t>(s.cursor168);const auto start=n;searchFirst_=start;
    do {
        ++searchProbes_;
        if(s.members150[n].filename08&&courseSourceName308F00(name,s.members150[n].filename08)){
            s.cursor168=n;winner_=n;break;
        }
        if(++n==s.memberCount154)n=0;
    }while(n!=start);
    searchReturn_=winner_;d.sourceEffectsCommitted=true;
    if(winner_!=-1)return fail(d,E::unsupported,0x131354); // Preserve foreign winner; never adopt/skip it.
    observedMode170_=s.sourceFileMode170;sourceMode170Read_=observedMode170_;
    if(!observedMode170_){returned_=true;returned=-1;return true;} // rb read-only miss131318/131348.
    // Native authentication admission is not a retail13C980 NULL return.
    // Immutable per-occurrence retail inputs: RACEMUS record16 at36C390,
    // LBA AC3/7 sectors/36DB bytes; SPCHPCT record2163 at381308,
    // pointer3A1C70, LBA C604/1 sector/FF bytes. Neither is a POD member.
    const bool percentages=occurrence_==Occurrence::announcerPercentages2C5164;
    const auto bytesExpected=percentages?255u:14043u;
    const char* localName=percentages?"SPCHPCT.INI":"RACEMUS.JUB";
    const char* hash=percentages?"24c2967388a31d04761fb11208faaae43a4ae7b9b88b40291bf64daf195c018c":
        "bff85d5a51a94157669be85bb90bb4d6be8bf4d802d95d08b1d2b235e9483bf1";
    storage_->pendingInput=std::make_shared<std::vector<std::uint8_t>>(bytesExpected);
    std::ifstream file(dlc50DL::host(audioRoot_/localName),std::ios::binary); // PC DLC overlay
    auto& bytes=*storage_->pendingInput;
    if(!file||!file.read(reinterpret_cast<char*>(bytes.data()),bytes.size())||file.peek()!=std::char_traits<char>::eof()||file.bad()||
       !OriginalLoadingTextureSource50LA::hashMatches(bytes.data(),bytes.size(),hash))
        return fail(d,E::source,0x13C980);
    input_=storage_->pendingInput;storage_->pendingInput.reset();
    //12DCB0 real device free-list or growth; count publication precedes realloc.
    if(s.freeDevice10==-1){
        if(s.deviceCount18==std::numeric_limits<std::int32_t>::max())return fail(d,E::unsupported,0x12DD30);
        const auto old=s.deviceCount18;++s.deviceCount18;
        auto* grown=new(std::nothrow) C::Device[s.deviceCount18];
        if(!grown){s.failedCourseDevices=s.devices14;s.failedCourseDeviceCapacity=old;s.devices14=nullptr;
            return fail(d,E::allocation,0x12DD30);}
        std::memcpy(grown,s.devices14,std::size_t(old)*sizeof(C::Device));delete[] s.devices14;s.devices14=grown;
        device_=old;s.freeDevice10=-1;auto& fresh=s.devices14[device_];
        fresh.flags0C=1;fresh.handle10=nullptr;fresh.id00=device_;fresh.name04=nullptr;
        fresh.cursor08=0;fresh.length14=0;fresh.byte24=0;
    }else{
        if(s.freeDevice10<0||s.freeDevice10>=s.deviceCount18)return fail(d,E::phase,0x12DCB0);
        device_=s.freeDevice10;s.freeDevice10=-1;
        for(std::int32_t k=1;k<s.deviceCount18;++k){const auto next=(std::int64_t(device_)+k)%s.deviceCount18;
            if(s.devices14[next].flags0C&1){s.freeDevice10=static_cast<std::int32_t>(next);break;}}
    }
    auto& device=s.devices14[device_];
    storage_->pendingHandle.reset(new(std::nothrow) C::Handle(input_));
    if(!storage_->pendingHandle)return fail(d,E::allocation,0x13C980);
    if(!storage_->pendingHandle->seek(input_->size()))return fail(d,E::source,0x13D8F0);
    const auto length=static_cast<std::uint32_t>(storage_->pendingHandle->cursor); //13D950.
    //12E2B0 duplicate/free-member/growth5 protocol on the SAME global tables.
    for(std::int32_t k=0;k<s.memberCount154;++k)
        if(s.members150[k].filename08&&courseSourceName308F00(name,s.members150[k].filename08))return fail(d,E::phase,0x12E324);
    if(s.freeMember14C==-1){
        const auto old=s.memberCount154;
        if(old>std::numeric_limits<std::int32_t>::max()-5)return fail(d,E::unsupported,0x12E2B0);
        auto* grown=new(std::nothrow) C::Member[std::size_t(old)+5];
        if(!grown){s.failedCourseMembers=s.members150;s.failedCourseMemberCapacity=old;s.members150=nullptr;
            return fail(d,E::allocation,0x12E2B0);}
        std::memcpy(grown,s.members150,std::size_t(old)*sizeof(C::Member));delete[] s.members150;s.members150=grown;
        s.freeMember14C=old;for(std::int32_t k=old;k<old+5;++k){grown[k].initialize();++s.memberCount154;}
    }
    const auto chosen=s.freeMember14C;s.freeMember14C=-1;
    if(chosen<0||chosen>=s.memberCount154)return fail(d,E::phase,0x12E458);
    for(std::int32_t k=1;k<s.memberCount154;++k){const auto next=(std::int64_t(chosen)+k)%s.memberCount154;
        if(s.members150[next].flags18&1){s.freeMember14C=static_cast<std::int32_t>(next);break;}}
    member_=chosen;auto& row=s.members150[member_];row.flags18=0;
    if(!memberNameAssertion12E4E8(row.filename08,d))return false;
    const auto filename=sourceNameNative(); //Both exact immutable names include a separately copied NUL.
    row.filename08=new(std::nothrow) char[filename.size()+1u];if(!row.filename08)return fail(d,E::allocation,0x12E578);
    storage_->memberNameIdentity=row.filename08;
    std::memcpy(row.filename08,filename.data(),filename.size());row.filename08[filename.size()]=0;
    row.deviceID00=device_;row.logicalStart0C=0;row.cursor10=0;row.length14=length;
    row.auxiliarySlot04=0;row.associatedObject1C=nullptr;
    //131230 passes its stack+48 out slot, NOT the132820 cached lookup160.
    // member_ retains that returned numeric index; shared cursor160 is untouched.
    row.originalMember=nullptr;row.loadingRaOwner=nullptr;row.musicOwner=this;row.canonicalName=nullptr;row.canonicalData=input_->data();
    if(!fileIoAssertions().looseName8208_3E1AC0.reach(device.name04==nullptr,d,0x12E080)||
       !fileIoAssertions().looseFree820C_3E1AC8.reach(device.flags0C&1,d,0x12E0EC))return false;
    device.flags0C=4;device.handle10=storage_->pendingHandle.release();device.cursor08=0;device.length14=length;
    device.name04=new(std::nothrow) char[filename.size()+1u];if(!device.name04)return fail(d,E::allocation,0x12E148);
    storage_->deviceNameIdentity=device.name04;
    std::memcpy(device.name04,filename.data(),filename.size());device.name04[filename.size()]=0;device.payload18=0;device.tableOffset1C=0;device.tableBytes20=0;device.byte24=0;device.id00=device_;storage_->deviceBodyPublished=true;
    registered_=true;
    if(!resolver_->openExactSourceMember131380(config_,member_,handle_,d))return false;
    returned_=true;returned=handle_;return true;
}catch(...){return fail(d,E::allocation,0x131230);}
bool OriginalMusicSourceFile50LA::current(const void* config,std::int64_t handle)const noexcept {
    if(!issuer_||!manager_||!resolver_||!config||config!=config_||thread_!=std::this_thread::get_id()||
       !returned_||closed_||nativeClosed_||handle!=handle_||!resolver_->freshFileCapacity_)return false;
    const auto& s=*resolver_->freshFileCapacity_;if(s.musicOwner!=this)return false;
    if(handle_==-1)return !registered_&&member_==-1&&winner_==-1;
    if(!registered_||!input_||!s.members150||!s.devices14||member_!=handle_||member_<0||member_>=s.memberCount154||
       device_<0||device_>=s.deviceCount18)return false;
    const auto& row=s.members150[member_];const auto& device=s.devices14[device_];
    return row.musicOwner==this&&row.courseConfigOwner==config_&&(row.flags18&4)&&row.deviceID00==device_&&
        !row.originalMember&&!row.loadingRaOwner&&row.canonicalData==input_->data()&&row.length14==input_->size()&&
        row.cursor10<=row.length14&&row.filename08&&device.flags0C==4&&device.name04&&device.length14==input_->size();
}
//Read-only admission for THIS occurrence, including failed open and its one
//fixed read and text seek. current() cannot cover partial native custody.
bool OriginalMusicSourceFile50LA::nativeAnnouncerCustodyCurrent(
    const original_main_entry50KG::Application* issuer,const rr::game::OriginalRaceMusicManager50LA* manager,
    const void* config,const R* resolver)const noexcept {
    if(occurrence_!=Occurrence::announcerPercentages2C5164||!issuer||issuer_!=issuer||!manager||manager_!=manager||
       !config||!resolver||thread_!=std::this_thread::get_id()||flushAttempted_||flushReturned_||flushHandleCall_||
       closeAttempted_||closed_||sourceLooseSlotsRetired_)return false; //No source destructor/close joined here.
    if(probeAttempted_){
        if(!acquired_||!returned_||!storage_||!probeOutputIdentity_||!probeSnapshotTaken_||probeCopied_>7u)return false;
    }else if(probeHelperReturned_||probeSnapshotTaken_||probeOutputIdentity_||probeCopied_)return false;
    if(textSeekAttempted_){
        if(!probeAttempted_||!probeHelperReturned_||probeCopied_!=7u||!textSeekSnapshotTaken_)return false;
        if(textSeekReturned_){
            if(!textSeekFirstStore_||!textSeekSecondStore_||textSeekReload_!=0u||textSeekReturnV0_!=0u)return false;
        }else if(textSeekFirstStore_||textSeekSecondStore_||textSeekReload_||textSeekReturnV0_)return false;
        const auto sameUnchangedLanes=[](const Storage::ProbeSnapshot& a,const Storage::ProbeSnapshot& b)noexcept {
            return a.deviceCursor==b.deviceCursor&&a.deviceFlags==b.deviceFlags&&a.handleIdentity==b.handleIdentity&&
                a.physicalCursor==b.physicalCursor&&a.scratchIndex==b.scratchIndex&&a.scratchStatus==b.scratchStatus&&
                a.scratchOffset==b.scratchOffset&&a.scratchPointer==b.scratchPointer&&a.rawValidBytes==b.rawValidBytes;
        };
        const auto& before=storage_->beforeTextSeek;const auto& after=storage_->textSeek;
        //Historical values/pointer equality only, also safe after native close.
        //Neither saved Handle nor scratch pointer is dereferenced here.
        if(before.memberCursor!=storage_->probe.memberCursor||!sameUnchangedLanes(before,storage_->probe)||
           !sameUnchangedLanes(before,after)||after.memberCursor!=(textSeekReturned_?0u:before.memberCursor))return false;
    }else if(textSeekFirstStore_||textSeekSecondStore_||textSeekReturned_||textSeekSnapshotTaken_||
             textSeekReload_||textSeekReturnV0_)return false;
    if(textLinePrefixAttempted_){
        if(!textSeekReturned_||!textLineOutputIdentity_)return false;
        if(textLineAssertionsReturned_){
            if(textLineLengthRead_!=255u||textLineCursorRead_!=storage_->textSeek.memberCursor||
               !textLineRemaining_||*textLineRemaining_!=std::uint32_t(*textLineLengthRead_-*textLineCursorRead_)||
               textLinePositiveBranch_!=(*textLineRemaining_!=0u&&(*textLineRemaining_&0x80000000u)==0u))return false;
        }else if(textLineLengthRead_||textLineCursorRead_||textLineRemaining_||textLinePositiveBranch_)return false;
    }else if(textLineAssertionsReturned_||textLineOutputIdentity_||textLineLengthRead_||textLineCursorRead_||
             textLineRemaining_||textLinePositiveBranch_)return false;
    if(!lineTransferHistoryCurrentNative()||!lineSuffixHistoryCurrentNative())return false;
    if(!acquired_){
        //Native bookkeeping allocation/root-copy can fail before pin/publication.
        return !attempted_&&!resolver_&&!config_&&!input_&&device_==-1&&member_==-1&&handle_==-1&&winner_==-1&&
            !registered_&&!returned_&&!searchCursorBefore_&&!searchCount_&&!searchFirst_&&!searchReturn_&&!searchProbes_&&
            !sourceMode170Read_&&(!storage_||(!storage_->pendingHandle&&!storage_->pendingInput&&
                                           !storage_->memberNameIdentity&&!storage_->deviceNameIdentity&&!storage_->deviceBodyPublished));
    }
    if(!attempted_||!resolver_||resolver_.get()!=resolver||config_!=config||!storage_||
       !resolver_->freshFileCapacity_||!resolver_->scratch_||!searchCursorBefore_||!searchCount_||*searchCount_<=0||
       *searchCursorBefore_<-1||*searchCursorBefore_>=*searchCount_||!searchFirst_||
       *searchFirst_!=((*searchCursorBefore_+1==*searchCount_)?0:*searchCursorBefore_+1)||
       !searchReturn_||*searchReturn_!=winner_||!searchProbes_||searchProbes_>std::uint32_t(*searchCount_))return false;
    if(winner_==-1){if(searchProbes_!=std::uint32_t(*searchCount_)||!sourceMode170Read_||
                         *sourceMode170Read_!=observedMode170_)return false;}
    else {
        if(winner_<0||winner_>=*searchCount_||sourceMode170Read_||input_||storage_->pendingInput||
           storage_->pendingHandle||device_!=-1||member_!=-1||returned_||registered_||handle_!=-1)return false;
        const auto distance=(std::int64_t(winner_)-*searchFirst_+*searchCount_)%*searchCount_;
        if(searchProbes_!=std::uint32_t(distance+1))return false;
    }
    const auto& s=*resolver_->freshFileCapacity_;
    if(nativeClosed_){
        //Indices and name addresses may be reused after pin withdrawal.
        //Never index their old arrays or dereference historical names here.
        return s.musicOwner!=this&&!registered_&&handle_==-1&&!storage_->pendingHandle;
    }
    if(s.musicOwner!=this||s.streamLibraryOwner)return false;
    //The probe observations remain history. Only this actual later seek's
    //separate post-state replaces them as the live cursor/handle/slot guard.
    const auto& latest=suffixSnapshotTaken_?storage_->afterSuffix.own:
        (lineSnapshotTaken_?storage_->line.own:(textSeekAttempted_?storage_->textSeek:storage_->probe));
    if(lineSnapshotTaken_){
        const auto& shared=suffixSnapshotTaken_?storage_->afterSuffix:storage_->line;
        if(s.active!=shared.active||s.current74!=shared.current74||s.window7C!=shared.window7C||s.window80!=shared.window80)return false;
    }
    if(winner_!=-1)return true; //Authentic foreign winner stays untouched by cancellation.
    if(!*sourceMode170Read_)
        return returned_&&!registered_&&device_==-1&&member_==-1&&handle_==-1&&!input_&&
            !storage_->pendingInput&&!storage_->pendingHandle; //Actual rb miss, not host OOM.
    if(input_&&input_->size()!=255u)return false;
    if(storage_->pendingInput&&(input_||device_!=-1||member_!=-1||storage_->pendingInput->size()!=255u))return false;
    if(storage_->pendingHandle&&(!input_||storage_->pendingHandle->musicInput!=input_||device_<0))return false;
    const auto* members=s.members150?s.members150:s.failedCourseMembers;
    const auto memberCount=s.members150?s.memberCount154:s.failedCourseMemberCapacity;
    const auto* devices=s.devices14?s.devices14:s.failedCourseDevices;
    const auto deviceCount=s.devices14?s.deviceCount18:s.failedCourseDeviceCapacity;
    if(device_>=0){
        if(!input_||!devices||device_>=deviceCount)return false;
        const auto& device=devices[device_];if(device.id00!=device_)return false;
        if(device.flags0C==1u){if(probeAttempted_||device.handle10||device.name04||storage_->deviceNameIdentity)return false;}
        else if(device.flags0C==4u){
            if((device.handle10&&device.handle10->musicInput!=input_)||device.name04!=storage_->deviceNameIdentity||
               device.length14!=255u||device.cursor08!=(probeAttempted_?latest.deviceCursor:0u))return false;
            if(probeAttempted_){
                const auto& observed=latest;
                if(device.flags0C!=observed.deviceFlags||device.handle10!=observed.handleIdentity)return false;
                if(device.handle10){if(!observed.physicalCursor||device.handle10->cursor!=*observed.physicalCursor)return false;}
                else if(observed.physicalCursor)return false;
            }else if(!device.handle10)return false;
            if(device.name04){
                if(!storage_->deviceBodyPublished||device.payload18||device.tableOffset1C||device.tableBytes20||device.byte24||
                   std::memcmp(device.name04,sourceNameNative().data(),sourceNameNative().size()+1u)!=0)return false;
            }else if(storage_->deviceBodyPublished)return false; //Do not read unwritten payload/table lanes.
        }else return false;
    }else if(member_!=-1||registered_||handle_!=-1||storage_->pendingHandle||storage_->deviceNameIdentity)return false;
    if(member_>=0){
        if(!input_||device_<0||!members||member_>=memberCount)return false;
        const auto& row=members[member_];
        if(row.originalMember||row.loadingRaOwner||row.associatedObject1C||
           (row.musicOwner&&row.musicOwner!=this)||(row.courseConfigOwner&&row.courseConfigOwner!=config_)||
           row.filename08!=storage_->memberNameIdentity)return false;
        if(row.musicOwner==this){
            if(!row.filename08||std::memcmp(row.filename08,sourceNameNative().data(),sourceNameNative().size()+1u)!=0||
               row.canonicalData!=input_->data()||row.canonicalName||row.deviceID00!=device_||
               row.logicalStart0C||row.length14!=255u||row.cursor10!=(probeAttempted_?latest.memberCursor:0u)||(row.flags18&~12u))return false;
        }else if(row.filename08||row.canonicalData||row.courseConfigOwner||row.flags18)return false;
        if(handle_>=0){if(handle_!=member_||row.courseConfigOwner!=config_||!(row.flags18&4u)||!registered_)return false;}
        else if(row.courseConfigOwner||(row.flags18&4u))return false;
    }else if(registered_||handle_!=-1||storage_->memberNameIdentity)return false;
    if(registered_){
        if(member_<0||device_<0||!devices||devices[device_].flags0C!=4u||!devices[device_].name04)return false;
    }
    if(returned_){
        if(!registered_||handle_<0||!s.members150||!s.devices14)return false;
        const auto& row=members[member_];
        if(row.auxiliarySlot04<0||std::size_t(row.auxiliarySlot04)>=resolver_->scratch_->slots.size()||
           resolver_->scratch_->slots[row.auxiliarySlot04].status08!=member_||
           !resolver_->scratch_->slots[row.auxiliarySlot04].pointer00)return false;
        if(probeAttempted_){
            const auto& observed=latest;const auto& slot=resolver_->scratch_->slots[row.auxiliarySlot04];
            if(row.auxiliarySlot04!=observed.scratchIndex||slot.pointer00!=observed.scratchPointer||
               slot.status08!=observed.scratchStatus||slot.offset10!=observed.scratchOffset||
               slot.nativeRawValidBytes!=observed.rawValidBytes)return false;
        }
    }
    return true;
}
bool OriginalMusicSourceFile50LA::readAnnouncerProbe12FB30(const void* config,std::int64_t handle,
    void* output,std::size_t& copied,D& d)noexcept {
    d={};copied=0;
    //This admission authenticates the fully opened, unread own rows before
    //the shared helper. No old RACEMUS read or caller-invented token is used.
    if(probeAttempted_||!output||!resolver_||handle<0||!current(config,handle)||
       !nativeAnnouncerCustodyCurrent(issuer_,manager_,config,resolver_.get()))return fail(d,E::phase,0x12FB30);
    //The helper cannot realloc/retire these preflighted rows or scratch slot.
    //Both observations are fixed-size and nonallocating. Older raw-valid data
    //is retained as observed metadata, never a synthetic new-read receipt.
    const auto capture=[&](Storage::ProbeSnapshot& observed)noexcept {
        const auto& s=*resolver_->freshFileCapacity_;const auto& row=s.members150[member_];
        const auto& device=s.devices14[device_];
        observed.memberCursor=row.cursor10;observed.deviceCursor=device.cursor08;observed.deviceFlags=device.flags0C;
        observed.handleIdentity=device.handle10;
        if(device.handle10)observed.physicalCursor=device.handle10->cursor;
        observed.scratchIndex=row.auxiliarySlot04;const auto& slot=resolver_->scratch_->slots[row.auxiliarySlot04];
        observed.scratchPointer=slot.pointer00;observed.scratchStatus=slot.status08;observed.scratchOffset=slot.offset10;
        observed.rawValidBytes=slot.nativeRawValidBytes;
    };
    capture(storage_->beforeProbe);probeAttempted_=true;probeOutputIdentity_=output;
    probeHelperReturned_=read12FB30(config,handle,output,7u,copied,d);probeCopied_=copied;
    //A false return can still reorder/evict active devices or publish a
    //scratch offset before refusal. Preserve those actual own post-fields.
    capture(storage_->probe);probeSnapshotTaken_=true;
    //copied is known output bytes, NEVER the size7*count1 source return.
    return probeHelperReturned_;
}
bool OriginalMusicSourceFile50LA::read12FB30(const void* config,std::int64_t handle,
    void* output,std::size_t requested,std::size_t& copied,D& d)noexcept {
    d={};copied=0;if(flushAttempted_||closeAttempted_||!current(config,handle)||handle_==-1)return fail(d,E::phase,0x12FB30);
    auto& s=*resolver_->freshFileCapacity_;auto& member=s.members150[handle_];
    if(!readAssertions12FB30(handle_,s.memberCount154,member.flags18,d))return false;
    const auto amount=std::min<std::size_t>(requested,member.length14-member.cursor10);
    if(!resolver_->activateCourseDevice12E8B0(member.deviceID00,member.logicalStart0C+member.cursor10,d))return false;
    auto& device=s.devices14[member.deviceID00];
    if(device.flags0C!=4||!device.handle10||device.handle10->musicInput!=input_||device.payload18)
        return fail(d,E::phase,0x13059C);
    if(!amount)return true; //12FEAC..12FEE0: no raw read or destination access at EOF.
    if(!output)return fail(d,E::unsupported,0x12FEF8);
    if(member.auxiliarySlot04==-1||amount>=R::Scratch::bytes){
        if(!device.handle10->seek(std::size_t(device.payload18)+device.cursor08))return fail(d,E::source,0x1305DC);
        if(!device.handle10->read(output,amount))return fail(d,E::source,0x1305F4);
        copied=amount;device.cursor08+=static_cast<std::uint32_t>(amount); //13061C only after full read.
    }else{
        if(member.auxiliarySlot04<0||std::size_t(member.auxiliarySlot04)>=resolver_->scratch_->slots.size())return fail(d,E::phase,0x130620);
        auto& slot=resolver_->scratch_->slots[member.auxiliarySlot04];
        if(!slot.pointer00||slot.status08!=handle_)return fail(d,E::phase,0x130620);
        auto within=std::int64_t(device.cursor08)-slot.offset10;
        if(within<0||within>=std::int64_t(R::Scratch::bytes)){
            slot.offset10=static_cast<std::int32_t>(device.cursor08); //130654 BEFORE physical read.
            const auto absolute=std::size_t(device.payload18)+device.cursor08;
            if(absolute>device.length14)return fail(d,E::source,0x130668);
            const auto bytes=std::min<std::size_t>(R::Scratch::bytes,device.length14-absolute);
            if(!device.handle10->seek(absolute))return fail(d,E::source,0x130690);
            if(!device.handle10->read(slot.pointer00,bytes))return fail(d,E::source,0x1306B0);
            slot.nativeRawValidBytes=static_cast<std::uint32_t>(bytes);within=0;
        }
        const auto n=std::min<std::size_t>(amount,R::Scratch::bytes-std::size_t(within));
        if(!slot.nativeRawValidBytes||std::size_t(within)>*slot.nativeRawValidBytes||n>*slot.nativeRawValidBytes-std::size_t(within))
            return fail(d,E::phase,0x130700);
        if(n)std::memcpy(output,slot.pointer00+within,n);
        copied=n;device.cursor08+=static_cast<std::uint32_t>(n); //130718.
        if(n!=amount)return fail(d,E::unsupported,0x130734); // Exact cohort never crosses this small-read window.
    }
    member.cursor10+=static_cast<std::uint32_t>(copied);d.sourceEffectsCommitted=true;return true;
}
bool OriginalMusicSourceFile50LA::seekAnnouncerText12F640(const void* config,std::int64_t handle,D& d)noexcept {
    d={};
    if(textSeekAttempted_||!probeAttempted_||!probeHelperReturned_||probeCopied_!=7u||!resolver_||handle<0||
       !current(config,handle)||!nativeAnnouncerCustodyCurrent(issuer_,manager_,config,resolver_.get()))
        return fail(d,E::phase,0x12F640);
    const auto capture=[&](Storage::ProbeSnapshot& observed)noexcept {
        const auto& s=*resolver_->freshFileCapacity_;const auto& row=s.members150[member_];
        const auto& device=s.devices14[device_];
        observed.memberCursor=row.cursor10;observed.deviceCursor=device.cursor08;observed.deviceFlags=device.flags0C;
        observed.handleIdentity=device.handle10;
        if(device.handle10)observed.physicalCursor=device.handle10->cursor;
        observed.scratchIndex=row.auxiliarySlot04;const auto& slot=resolver_->scratch_->slots[row.auxiliarySlot04];
        observed.scratchPointer=slot.pointer00;observed.scratchStatus=slot.status08;observed.scratchOffset=slot.offset10;
        observed.rawValidBytes=slot.nativeRawValidBytes;
    };
    capture(storage_->beforeTextSeek);textSeekAttempted_=true;
    auto& s=*resolver_->freshFileCapacity_;
    //Same shared process assertions, exactly once. Do not additionally invoke
    //seekSet12F640: its net store would omit these two reached source stores.
    if(!seekAssertions12F640(handle_,s.memberCount154,d)){
        capture(storage_->textSeek);textSeekSnapshotTaken_=true;return false;
    }
    auto& row=s.members150[member_];
    row.cursor10=0u;textSeekFirstStore_=true; //12F758 branch,12F75C ordinary delay: origin0.
    textSeekReload_=row.cursor10; //12F774 ACTUAL reload, not copied from argument.
    textSeekReturnV0_=0u; //12F778.
    const auto sum=std::uint32_t(*textSeekReload_+std::uint32_t(0)); //12F77C original32-bit add offset0.
    row.cursor10=sum;textSeekSecondStore_=true; //12F780 distinct second store.
    textSeekReturned_=true; //12F784..79C restores/returns; no numerical guest register image fabricated.
    d.sourceEffectsCommitted=true;
    capture(storage_->textSeek);textSeekSnapshotTaken_=true;
    //No physical seek, activation, scratch refill, device-cursor or mode write.
    return true;
}
bool OriginalMusicSourceFile50LA::beginAnnouncerTextLine12EE90(const void* config,std::int64_t handle,
    void* output,D& d)noexcept {
    d={};
    if(textLinePrefixAttempted_||!output||!textSeekReturned_||!resolver_||handle<0||
       !current(config,handle)||!nativeAnnouncerCustodyCurrent(issuer_,manager_,config,resolver_.get()))
        return fail(d,E::phase,0x12EEB8);
    textLinePrefixAttempted_=true;textLineOutputIdentity_=output;
    auto& s=*resolver_->freshFileCapacity_;
    //Join the EXISTING shared8220/8224 process history, exactly once.
    //No new Config/file-local guard, reset, guessed assertion word or diagnostic.
    //This valid own handle makes either133660 condition true WHEN its
    //assertion word enables the call. Original13366C reads mutable debug;
    //both possible debug branches return0 for true conditions. This existing
    //semantic helper assumes no debug value/read receipt and emits no output.
    if(!lineAssertions12EE90(handle_,s.memberCount154,d))return false;
    textLineAssertionsReturned_=true;
    //12EF50-or12EF94 then12EF98 select signed32 index;12EF9C
    //stride32,12EFA0/4 own row. No conditional guard/count read is invented.
    const auto& row=s.members150[handle_];
    textLineLengthRead_=row.length14; //12EFA8 actual field read.
    textLineCursorRead_=row.cursor10; //12EFAC actual field read, not expected0 substituted.
    textLineRemaining_=std::uint32_t(*textLineLengthRead_-*textLineCursorRead_); //12EFB0 SUBU.
    textLinePositiveBranch_=*textLineRemaining_!=0u&&(*textLineRemaining_&0x80000000u)==0u; //12EFB4 signed BGTZ,12EFB8 NOP.
    //No line bytes, activation, device/member/scratch/handle mutation or source
    //12EE90 return. Existing post-seek custody remains the actual latest state.
    return true; //Native prefix completion only, never source V0/present=true.
}
bool OriginalMusicSourceFile50LA::lineTransferHistoryCurrentNative()const noexcept {
    const auto& t=lineTransfer_;
    if(!t.historyCurrent())return false;
    if(!t.entered)return !lineSnapshotTaken_&&!activationSnapshotTaken_;
    if(!storage_||!textLineAssertionsReturned_||textLinePositiveBranch_!=true||
       t.outputIdentity!=textLineOutputIdentity_||t.deviceRead!=device_||!lineSnapshotTaken_||
       activationSnapshotTaken_!=t.activationCursorStored)return false;
    const auto sameUnchanged=[](const Storage::ProbeSnapshot& a,const Storage::ProbeSnapshot& b)noexcept {
        return a.memberCursor==b.memberCursor&&a.deviceFlags==b.deviceFlags&&a.handleIdentity==b.handleIdentity&&
            a.physicalCursor==b.physicalCursor&&a.scratchIndex==b.scratchIndex&&a.scratchStatus==b.scratchStatus&&
            a.scratchOffset==b.scratchOffset&&a.scratchPointer==b.scratchPointer&&a.rawValidBytes==b.rawValidBytes;
    };
    const auto sameShared=[](const Storage::LineSnapshot& a,const Storage::LineSnapshot& b)noexcept {
        return a.active==b.active&&a.current74==b.current74&&a.window7C==b.window7C&&a.window80==b.window80;
    };
    const auto& before=storage_->beforeLine;const auto& after=storage_->line;
    if(!sameUnchanged(before.own,storage_->textSeek)||before.own.deviceCursor!=storage_->textSeek.deviceCursor||
       !sameUnchanged(before.own,after.own)||!sameShared(before,after))return false;
    if(t.activeHeadRead&&*t.activeHeadRead!=before.active[0])return false;
    if(t.handleRead&&*t.handleRead!=before.own.handleIdentity)return false;
    if(t.scratchIndexRead&&*t.scratchIndexRead!=before.own.scratchIndex)return false;
    if(t.scratchOffsetRead&&*t.scratchOffsetRead!=before.own.scratchOffset)return false;
    const auto expectedCursor=t.deviceCursorStored?std::uint32_t(*t.deviceCursorReload+std::uint32_t(t.copied)):
        (t.activationCursorStored?*t.position:before.own.deviceCursor);
    if(after.own.deviceCursor!=expectedCursor)return false;
    if(activationSnapshotTaken_){
        const auto& activated=storage_->afterActivation;
        if(!sameUnchanged(before.own,activated.own)||!sameShared(before,activated)||
           activated.own.deviceCursor!=*t.position)return false;
    }
    //Only saved values and identity comparisons. No historical Handle, scratch,
    //old member/device row or parser output byte is dereferenced after retirement.
    return true;
}

bool OriginalMusicSourceFile50LA::transferAnnouncerLine12EFC8(const void* config,std::int64_t handle,
    void* output,std::uint32_t capacity,D& d)noexcept {
    d={};auto& t=lineTransfer_;
    if(!t.unissued()||!textLineAssertionsReturned_||textLinePositiveBranch_!=true||
       output!=textLineOutputIdentity_||!output||capacity!=0x400u||!resolver_||handle<0||
       !current(config,handle)||!nativeAnnouncerCustodyCurrent(issuer_,manager_,config,resolver_.get()))
        return fail(d,E::phase,0x12EFC8);
    auto& s=*resolver_->freshFileCapacity_;auto& member=s.members150[handle_];auto& device=s.devices14[device_];
    const auto capture=[&](Storage::LineSnapshot& observed)noexcept {
        auto& own=observed.own;own.memberCursor=member.cursor10;own.deviceCursor=device.cursor08;own.deviceFlags=device.flags0C;
        own.handleIdentity=device.handle10;if(device.handle10)own.physicalCursor=device.handle10->cursor;
        own.scratchIndex=member.auxiliarySlot04;const auto& slot=resolver_->scratch_->slots[member.auxiliarySlot04];
        own.scratchPointer=slot.pointer00;own.scratchStatus=slot.status08;own.scratchOffset=slot.offset10;
        own.rawValidBytes=slot.nativeRawValidBytes;
        //Native observation only, not unissued source reads of every shared lane.
        observed.active=s.active;observed.current74=s.current74;observed.window7C=s.window7C;observed.window80=s.window80;
    };
    const auto finish=[&](bool result)noexcept {capture(storage_->line);lineSnapshotTaken_=true;return result;};
    const auto stop=[&](E error,std::uint32_t pc)noexcept {fail(d,error,pc,t.copied);return finish(false);};
    capture(storage_->beforeLine);t.entered=true;t.outputIdentity=output;
    t.deviceRead=member.deviceID00;t.logicalStartRead=member.logicalStart0C; //12EFC8/CC; same actual devices14 selected12EFD0.
    t.position=std::uint32_t(*t.logicalStartRead+*textLineCursorRead_); //12EFE0 uses retained12EFAC A3, NOT device cursor7.
    //Same process guard8214/word3E1AD8. This semantic join does not invent
    //granular guard/debug read receipts, and is NOT a source JAL12E8B0.
    //A valid own row makes133660 return0 through either mutable-debug branch.
    if(!fileIoAssertions().active8214_3E1AD8.reach(true,d,0x12F018))return finish(false);
    t.activeAssertionReturned=true;
    t.deviceIDRead=device.id00;t.activeHeadRead=s.active[0]; //12F044 selects actual head,12F048/04C reads.
    t.ownHeadBranch=*t.activeHeadRead==*t.deviceIDRead; //12F050 BEQ,12F054 ordinary A1=0 delay.
    if(!*t.ownHeadBranch)return stop(E::unsupported,0x12F058); //No reorder, eviction or foreign-handle close.
    t.handleRead=static_cast<const void*>(device.handle10); //12F144 actual handle identity, nullable not a readiness substitute.
    t.nonnullHandleBranch=*t.handleRead!=nullptr;t.activationV0=1u; //12F148 branch and12F14C ordinary delay.
    if(!*t.nonnullHandleBranch)return stop(E::unsupported,0x12F150); //No implicit reopen/allocation.
    device.cursor08=*t.position;t.activationCursorStored=true; //12F17C genuine inline activation store.
    d.sourceEffectsCommitted=true;
    capture(storage_->afterActivation);activationSnapshotTaken_=true;
    //12F180 sees the selected1, does not return;12F184 clears V0 in its delay.
    t.requestLimit=capacity-2u; //12F188.
    t.request=*textLineRemaining_<*t.requestLimit?*textLineRemaining_:*t.requestLimit; //12F18C/190 signed min; admitted positives.
    t.scratchIndexRead=member.auxiliarySlot04; //12F194 actual slot index.
    //12F198/19C sign-extend request; A0=FileManager,A1=own device,A2=slot,
    //T0=request;12F1B0 delay supplies this same output identity.
    t.rawCallEntered=true;t.rawSavedReturnPC=0x12f1b4u;t.rawFrameBytes=0xf0u; //12F1AC ->130560.
    for(auto& saved:t.rawSavesIssued)saved=true; //130564..588 RA,FP,S7,S6,S5,S4,S3,S2,S1,S0.
    //Known caller identities/values remain in the enclosing trace; unknown FP,
    //S7 and upper saved lanes are not fabricated numeric register images.
    t.flagsRead=device.flags0C; //13058C;130590..598 select receiver and save request/output.
    t.compressedBranch=(*t.flagsRead&0x100u)!=0u; //13059C/5A0, ordinary5A4 selects own device.
    if(*t.compressedBranch)return stop(E::unsupported,0x1307E4);
    t.directBranch=*t.scratchIndexRead==-1||*t.request>=0x6000u; //1305A8..5C0.
    if(*t.directBranch)return stop(E::unsupported,0x1305C4); //No physical seek/read path issued.
    const auto& slot=resolver_->scratch_->slots[*t.scratchIndexRead]; //Preflight exact own live slot, no realloc in this prefix.
    t.deviceCursorRead=device.cursor08;t.scratchOffsetRead=slot.offset10; //130624/634 actual reads.
    t.withinBits=std::uint32_t(*t.deviceCursorRead-std::uint32_t(*t.scratchOffsetRead)); //130638 SUBU.
    t.withinSigned=(*t.withinBits&0x80000000u)?std::int64_t(*t.withinBits)-0x100000000ll:std::int64_t(*t.withinBits); //13063C/640.
    t.cacheHitBranch=*t.withinSigned>=0&&*t.withinSigned<0x6000; //130644..650, preserves ordinary delays.
    if(!*t.cacheHitBranch)return stop(E::unsupported,0x130654); //Before source offset stamp, seek or refill.
    const auto available=0x6000u-static_cast<std::uint32_t>(*t.withinSigned); //1306C4..6D0.
    t.copyBytes=*t.request<available?*t.request:available; //1306D4..6E0 then selected signed amount.
    if(!slot.pointer00||slot.status08!=handle_||!slot.nativeRawValidBytes||
       std::uint64_t(*t.withinSigned)+*t.copyBytes>*slot.nativeRawValidBytes||*t.copyBytes>capacity)
        return stop(E::phase,0x130700); //Native byte-provenance refusal, not a source read0.
    //130700 ->3062C0 uses the actual selected warm scratch bytes. No input-file
    //shortcut, read12FB30 assertions, physical read or member cursor advance.
    std::memcpy(output,slot.pointer00+static_cast<std::size_t>(*t.withinSigned),*t.copyBytes);
    t.copied=*t.copyBytes;
    t.deviceCursorReload=device.cursor08; //130708 actual reload after copy.
    device.cursor08=std::uint32_t(*t.deviceCursorReload+std::uint32_t(t.copied));t.deviceCursorStored=true; //130714/718.
    if(t.copied!=*t.request)return stop(E::unsupported,0x130734); //No second-window/refill work invented.
    t.returnV0=*t.request; //13071C/720,130728 DSLL32 and130730 DSRL32 zero-extend the actual count.
    t.releasedFrameBytes=0xf0u;t.rawReturned=true; //131008..034 restores selected saves and returns12F1B4.
    return finish(true); //ONLY raw130560 return. No12F1B4 branch/delay, NUL, newline scan or member update.
}

bool OriginalMusicSourceFile50LA::lineSuffixHistoryCurrentNative()const noexcept {
    const auto& s=lineSuffix_;
    if(!s.historyCurrent())return false;
    if(!s.entered)return !suffixSnapshotTaken_;
    if(!storage_||!lineTransfer_.rawReturned||lineTransfer_.copied!=255u||!lineSnapshotTaken_||!suffixSnapshotTaken_||
       s.outputIdentity!=lineTransfer_.outputIdentity||s.rawV0Read!=lineTransfer_.returnV0||s.requestRead!=lineTransfer_.request||
       s.restoredS2!=config_||s.restoredS1!=std::int64_t(member_))return false;
    const auto sameExceptMember=[](const Storage::LineSnapshot& a,const Storage::LineSnapshot& b)noexcept {
        const auto& x=a.own;const auto& y=b.own;
        return x.deviceCursor==y.deviceCursor&&x.deviceFlags==y.deviceFlags&&x.handleIdentity==y.handleIdentity&&
            x.physicalCursor==y.physicalCursor&&x.scratchIndex==y.scratchIndex&&x.scratchStatus==y.scratchStatus&&
            x.scratchOffset==y.scratchOffset&&x.scratchPointer==y.scratchPointer&&x.rawValidBytes==y.rawValidBytes&&
            a.active==b.active&&a.current74==b.current74&&a.window7C==b.window7C&&a.window80==b.window80;
    };
    const auto& before=storage_->beforeSuffix;const auto& after=storage_->afterSuffix;
    return sameExceptMember(before,storage_->line)&&before.own.memberCursor==storage_->line.own.memberCursor&&
        sameExceptMember(before,after)&&s.memberCursorRead==before.own.memberCursor&&after.own.memberCursor==s.memberCursorSum;
    //Historical value/identity checks only, including after native close.
}

bool OriginalMusicSourceFile50LA::finishAnnouncerLine12F1B4(const void* config,std::int64_t handle,
    void* output,std::uint32_t capacity,std::uint32_t savedOrdinal,std::uint32_t savedReturnPC,D& d)noexcept {
    d={};auto& suffix=lineSuffix_;const auto& transfer=lineTransfer_;
    if(!suffix.unissued()||!transfer.rawReturned||transfer.returnV0!=255u||transfer.request!=255u||transfer.copied!=255u||
       !output||output!=transfer.outputIdentity||capacity!=0x400u||savedOrdinal!=0u||savedReturnPC!=0x157768u||
       !resolver_||handle<0||!current(config,handle)||!nativeAnnouncerCustodyCurrent(issuer_,manager_,config,resolver_.get()))
        return fail(d,E::phase,0x12F1B4);
    auto& state=*resolver_->freshFileCapacity_;auto& member=state.members150[handle_];const auto& device=state.devices14[device_];
    const auto capture=[&](Storage::LineSnapshot& observed)noexcept {
        auto& own=observed.own;own.memberCursor=member.cursor10;own.deviceCursor=device.cursor08;own.deviceFlags=device.flags0C;
        own.handleIdentity=device.handle10;if(device.handle10)own.physicalCursor=device.handle10->cursor;
        own.scratchIndex=member.auxiliarySlot04;const auto& slot=resolver_->scratch_->slots[member.auxiliarySlot04];
        own.scratchPointer=slot.pointer00;own.scratchStatus=slot.status08;own.scratchOffset=slot.offset10;own.rawValidBytes=slot.nativeRawValidBytes;
        observed.active=state.active;observed.current74=state.current74;observed.window7C=state.window7C;observed.window80=state.window80;
    };
    capture(storage_->beforeSuffix);
    //Every fallible native admission is above. The remaining own-buffer/source
    //block is nonallocating, nonthrowing and bounded by its actual NUL255 store.
    //No callback, transport, assertion, table relocation or foreign action occurs.
    auto* bytes=static_cast<std::uint8_t*>(output);
    const auto signedByte=[](std::uint8_t b)noexcept {return b<128u?std::int32_t(b):std::int32_t(b)-256;};
    suffix.entered=true;suffix.outputIdentity=output;suffix.rawV0Read=*transfer.returnV0;
    suffix.nonzeroBranch=*suffix.rawV0Read!=0u; //12F1B4 BNE, authentic positive raw return.
    suffix.requestRead=*transfer.request; //12F1B8 ordinary delay discards V0, DSLL32 from retained requested S0.
    suffix.requestTerminatorOffset=*suffix.requestRead; //12F1C8/CC signed request+same symbolic output.
    bytes[*suffix.requestTerminatorOffset]=0u;suffix.requestTerminatorWritten=true; //12F1D0, first known byte255.
    suffix.consumed=*suffix.requestRead; //Carry the original S0; overwrite ONLY if LF is found below.
    suffix.scanOffset=0u; //12F1D4 LF;12F1D8 branch,12F1DC ordinary A0=output delay.
    for(;;){
        const auto byte=bytes[*suffix.scanOffset]; //12F1E0 actual LB, bounded by the just-written terminator.
        suffix.scanBytes[suffix.scanReads++]=byte;suffix.lastSignedByte=signedByte(byte);
        if(*suffix.lastSignedByte==0)break; //12F1E4 taken,12F1E8 ordinary NOP.
        if(*suffix.lastSignedByte==10)break; //12F1EC BNEL NOT taken;12F1F0 increment ANNULLED.
        ++*suffix.scanOffset;++suffix.scanIncrements; //Taken BNEL,12F1F0 delay increments to next actual byte.
    }
    suffix.rereadByte=signedByte(bytes[*suffix.scanOffset]); //12F1F8 ACTUAL reload, before any CRLF normalization.
    suffix.foundLF=*suffix.rereadByte==10;
    suffix.pointerCleared=!*suffix.foundLF; //12F200 BNEL: only taken path executes12F204 A0=0.
    suffix.differenceIssued=true; //12F208 BEQ and ordinary12F20C SUBU always execute.
    if(*suffix.foundLF){
        suffix.difference=*suffix.scanOffset; //Same-buffer difference is known; no numerical EE pointer invented.
        suffix.consumed=std::uint32_t(*suffix.difference+1u); //12F210/214, signed-low32 value selected at12F220.
        suffix.previousByteInRange=*suffix.scanOffset>0u; //12F218 SLTU,12F21C branch tests its zero/nonzero.
        auto terminator=*suffix.scanOffset;
        if(*suffix.previousByteInRange){
            suffix.precedingRead=signedByte(bytes[*suffix.scanOffset-1u]); //12F224 actual preceding byte.
            suffix.precedingNotCRBranch=*suffix.precedingRead!=13; //12F22C BNEL against CR.
            if(*suffix.precedingNotCRBranch){
                ++terminator; //Taken12F230 increment delay; branch goes straight to12F244.
            }else {
                --terminator; //12F230 ANNULLED;12F234 actual decrement.
                suffix.crWriteOffset=terminator;bytes[terminator]=10u;suffix.crStored=true; //12F238/23C.
                ++terminator; //12F240 actual increment after CR->LF store.
            }
        }else ++terminator; //12F21C taken to12F240, after ordinary12F220 delay.
        suffix.lineTerminatorOffset=terminator;bytes[terminator]=0u;suffix.lineTerminatorWritten=true; //12F244.
    }
    //No-LF path skips12F210..244.12F20C's null-minus-output value is unknown
    //and unused; difference stays absent, while its issued receipt remains true.
    suffix.memberCursorRead=member.cursor10; //12F248 NEW actual read, not the old prefix's cursor receipt.
    suffix.sourcePointerV0=output; //12F24C POR: source pointer identity, never bool1 or byte count.
    suffix.memberCursorSum=std::uint32_t(*suffix.memberCursorRead+*suffix.consumed); //12F250 ADDU.
    member.cursor10=*suffix.memberCursorSum;suffix.memberStored=true; //12F254 distinct line-reader advance.
    suffix.restoresIssued[0]=true;suffix.restoredReturnPC=savedReturnPC; //12F258 actual saved RA157768.
    for(std::size_t n=1;n<=5u;++n)suffix.restoresIssued[n]=true; //12F25C..26C S7..S3 stay unknown, not zeroed.
    suffix.restoresIssued[6]=true;suffix.restoredS2=config; //12F270, actual same saved Config identity.
    suffix.restoresIssued[7]=true;suffix.restoredS1=handle; //12F274, actual saved parser handle.
    suffix.restoresIssued[8]=true;suffix.restoredS0=savedOrdinal; //12F278, actual saved parser ordinal0.
    suffix.releasedFrameBytes=0x90u;suffix.lineReturned=true; //12F27C JR RA and12F280 ordinary frame release.
    d.sourceEffectsCommitted=true;capture(storage_->afterSuffix);suffixSnapshotTaken_=true;
    return true; //12EE90 pointer return only. Parser157768 BNE/delay and159440 remain unissued.
}

bool OriginalMusicSourceFile50LA::seekSet12F640(const void* config,std::int64_t handle,
    std::uint32_t position,D& d)noexcept {
    d={};if(flushAttempted_||closeAttempted_||!current(config,handle)||handle_==-1)return fail(d,E::phase,0x12F640);
    auto& s=*resolver_->freshFileCapacity_;auto& row=s.members150[handle_];
    if(position>row.length14)return fail(d,E::unsupported,0x12F640);
    if(!seekAssertions12F640(handle_,s.memberCount154,d))return false;
    // Actual seek-origin0 changes logical member cursor only. The next read
    // activates the device at that position; physical/scratch state stays here.
    row.cursor10=position;d.sourceEffectsCommitted=true;return true;
}
bool OriginalMusicSourceFile50LA::flush12F290(const void* config,std::int64_t handle,
    std::int32_t& returned,D& d)noexcept {
    d={};if(flushAttempted_||closeAttempted_||!current(config,handle)||handle_==-1)
        return fail(d,E::phase,0x12f290);
    flushAttempted_=true;auto& s=*resolver_->freshFileCapacity_;
    d.sourceEffectsCommitted=true;
    if(!fileAssertions12F290(handle_,s.memberCount154,d))return false;
    const auto deviceID=s.members150[handle_].deviceID00;
    if(!s.devices14||deviceID<0||deviceID>=s.deviceCount18||deviceID!=device_)
        return fail(d,E::phase,0x12f3ac);
    //Actual12F3AC reads this member's device handle. A non-NULL handle calls
    //13D9E0, exactly JR RA with zero in its delay slot; NULL also returns0.
    //No host flush, callback, file close, mode change or device reset occurs.
    flushHandleCall_=s.devices14[deviceID].handle10!=nullptr;
    flushReturned_=true;returned=0;return true;
}
bool OriginalMusicSourceFile50LA::close131040(const void* config,std::int64_t handle,D& d)noexcept {
    d={};if(closeAttempted_||(flushAttempted_&&!flushReturned_)||!current(config,handle)||handle_==-1)
        return fail(d,E::phase,0x131040);
    closeAttempted_=true;auto& s=*resolver_->freshFileCapacity_;auto& retired=storage_->close;
    d.sourceEffectsCommitted=true;
    if(!closeAssertions131040(handle_,s.memberCount154,d))return false;
    auto& row=s.members150[handle_];auto& device=s.devices14[device_];
    row.cursor10=0;row.flags18&=~std::uint32_t(4); //131164/174.
    //131178..8C indexes this slot unconditionally, unlike12EAA0's -1 skip.
    if(row.auxiliarySlot04<0||std::size_t(row.auxiliarySlot04)>=resolver_->scratch_->slots.size())
        return fail(d,E::phase,0x13118c); //Native custody bound before the original indexed read.
    if(resolver_->scratch_->slots[row.auxiliarySlot04].status08==handle_)
        resolver_->scratch_->selected148=row.auxiliarySlot04; //131198.
    //This source occurrence owns a loose flags4 device, not an archive branch.
    using G=OriginalConfigProcessGlobals50LA;
    if(device.handle10){
        retired.handleIdentity=device.handle10;
        retired.handle28=0; //13D9FC, before the free's reentrancy read.
        retired.handle24=0;device.handle10->cursor=0; //13DA04; native logical cursor adapter.
        retired.handleFreeEntered=true;
        if(!G::descriptorDelete2E8B00AdmittedNative())return fail(d,E::phase,0x305734);
        //Matched native delete adapts2E8B00->305728. No guest heap bins/locks.
        delete device.handle10;retired.handleFreeReturned=true;retired.handleReturn=-1;
        device.handle10=nullptr; //1311D8 only after13D9F0 returned.
    }
    if(row.associatedObject1C)return fail(d,E::unsupported,0x1311dc);
    if(row.flags18&8u)return fail(d,E::unsupported,0x1311e8);
    //Actual1311FC ->12EB60. Split at its reached source frees instead of
    //claiming monolithic retirement when shared reentrancy is unknown.
    retired.deviceEntered=true;
    for(std::size_t at=0;at<s.active.size();++at)if(s.active[at]==device.id00){
        for(std::size_t donor=s.active.size();donor>0;--donor)if(s.active[donor-1]!=-1){
            s.active[at]=s.active[donor-1];s.active[donor-1]=-1;break;}
        break;
    }
    if(s.current74==device.id00){
        //132CD0(-1) observes handleNULL after the earlier13D9F0. Therefore
        //13D840 is skipped, not replaced by a synthetic synchronization.
        s.window7C=-1;s.window80=-1;s.current74=-1; //132D28/2C/30.
    }
    s.freeDevice10=device.id00; //12EC28, before device-name free.
    if(device.handle10)return fail(d,E::phase,0x12ec2c); //No unjoined replacement.
    if(device.name04){
        retired.deviceNameIdentity=device.name04;retired.deviceNameFreeEntered=true;
        if(!G::descriptorDelete2E8B00AdmittedNative())return fail(d,E::phase,0x305734);
        delete[] device.name04;retired.deviceNameFreeReturned=true;
        device.name04=nullptr; //12EC58 only after305728 returned.
    }
    device.flags0C=1;device.cursor08=0;device.length14=0;device.payload18=0;
    device.tableBytes20=0;device.tableOffset1C=0;device.byte24=0; //12EC60..78.
    retired.deviceReturned=true;
    //Actual131208 ->12EAA0. Scratch invalidation precedes filename free.
    retired.memberEntered=true;
    if(row.auxiliarySlot04!=-1){
        if(row.auxiliarySlot04<0||std::size_t(row.auxiliarySlot04)>=resolver_->scratch_->slots.size())
            return fail(d,E::phase,0x12eaf4); //Keep earlier device retirement on a custody bound.
        auto& slot=resolver_->scratch_->slots[row.auxiliarySlot04];
        if(slot.status08==member_){slot.status08=-1;resolver_->scratch_->selected148=row.auxiliarySlot04;}
    }
    if(row.filename08){
        retired.memberNameIdentity=row.filename08;retired.memberNameFreeEntered=true;
        if(!G::descriptorDelete2E8B00AdmittedNative())return fail(d,E::phase,0x305734);
        delete[] row.filename08;retired.memberNameFreeReturned=true;
        row.filename08=nullptr; //12EB20 only after305728 returned.
    }
    row.auxiliarySlot04=0;row.cursor10=0;row.deviceID00=0;row.flags18=1;
    row.associatedObject1C=nullptr;row.length14=0;row.logicalStart0C=0;
    s.freeMember14C=member_; //12EB24..44, exact source field order.
    //Native authentication/borrow fields retire only after those source stores.
    row.originalMember=nullptr;row.canonicalName=nullptr;row.canonicalData=nullptr;
    row.courseConfigOwner=nullptr;row.loadingRaOwner=nullptr;row.musicOwner=nullptr;
    retired.memberReturned=true;registered_=false;sourceLooseSlotsRetired_=true;
    handle_=-1;closed_=true;return true; //Actual131040 return, no root-pin release yet.

}
bool OriginalMusicSourceFile50LA::closeNative(D& d)noexcept {
    d={};if(nativeClosed_)return true;
    if(thread_!=std::this_thread::get_id())return fail(d,E::phase,0);
    if(!acquired_){nativeClosed_=true;return true;}
    if(!resolver_||!resolver_->freshFileCapacity_||!resolver_->scratch_||!storage_)return fail(d,E::phase,0);
    auto& s=*resolver_->freshFileCapacity_;if(s.musicOwner!=this)return fail(d,E::phase,0);
    if(closed_&&sourceLooseSlotsRetired_&&!registered_){
        //Original131040 already retired both numeric slots. Do not read or
        //retire them again: their indices are historical and may be reused.
        //Only end this adapter's native root pin; no source call is implied.
        s.musicOwner=nullptr;nativeClosed_=true;return true;
    }
    auto* members=s.members150?s.members150:s.failedCourseMembers;
    const auto memberCount=s.members150?s.memberCount154:s.failedCourseMemberCapacity;
    auto* devices=s.devices14?s.devices14:s.failedCourseDevices;
    const auto deviceCount=s.devices14?s.deviceCount18:s.failedCourseDeviceCapacity;
    const bool memberRetired=storage_->close.memberReturned,deviceRetired=storage_->close.deviceReturned;
    if(member_>=0&&!memberRetired){
        if(!members||member_>=memberCount)return fail(d,E::phase,0);
        const auto& row=members[member_];
        if((row.musicOwner&&row.musicOwner!=this)||(row.courseConfigOwner&&row.courseConfigOwner!=config_)||row.associatedObject1C)
            return fail(d,E::phase,0);
    }
    if(device_>=0&&!deviceRetired&&(!devices||device_>=deviceCount))return fail(d,E::phase,0);
    // Native cancellation retains original NULL realloc history and never
    // claims131040 returned. Foreign winner_ is never touched.
    storage_->pendingHandle.reset();
    if(member_>=0&&!memberRetired&&!(members[member_].flags18&1)){
        if(s.members150)s.retireMember12EAA0(member_);
        else {
            auto& row=members[member_];
            if(row.auxiliarySlot04>=0&&std::size_t(row.auxiliarySlot04)<resolver_->scratch_->slots.size()&&
               resolver_->scratch_->slots[row.auxiliarySlot04].status08==member_){
                resolver_->scratch_->slots[row.auxiliarySlot04].status08=-1;resolver_->scratch_->selected148=row.auxiliarySlot04;
            }
            delete[] row.filename08;row.filename08=nullptr;row.musicOwner=nullptr;row.courseConfigOwner=nullptr;
            row.canonicalData=nullptr;row.canonicalName=nullptr;row.flags18=1;row.deviceID00=0;
        }
    }
    if(device_>=0&&!deviceRetired){
        auto& device=devices[device_];
        // Even an unpublished/free device can own the exact reserved slot.
        s.retireDevice12EB60(device);
    }
    handle_=-1;registered_=false;s.musicOwner=nullptr;nativeClosed_=true;return true;
}
