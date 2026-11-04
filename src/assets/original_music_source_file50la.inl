// Included inside rr::assets after complete resolver storage and FileManager helpers.
struct OriginalMusicSourceFile50LA::Storage {
    std::unique_ptr<R::FreshFileCapacity::Handle> pendingHandle;
    // Retains even incompletely read/authenticated native input on refusal.
    std::shared_ptr<std::vector<std::uint8_t>> pendingInput;
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
    :issuer_(issuer),manager_(manager){}
OriginalMusicSourceFile50LA::~OriginalMusicSourceFile50LA()noexcept {
    if(acquired_&&!nativeClosed_)std::terminate();
}
bool OriginalMusicSourceFile50LA::begin131230(std::shared_ptr<R> resolver,const void* config,
    const std::filesystem::path& root,std::string_view name,std::int64_t& returned,D& d)noexcept try {
    d={};using C=R::FreshFileCapacity;
    if(!issuer_||!manager_||!config||!resolver||thread_!=std::this_thread::get_id()||attempted_||nativeClosed_||
       name!="audio\\racemus.jub"||root.empty()||!resolver->freshFileCapacity_||!resolver->scratch_)
        return fail(d,E::phase,0x131230);
    auto& s=*resolver->freshFileCapacity_;
    if(s.streamLibraryOwner||s.musicOwner||!s.members150||s.memberCount154<=0||!s.devices14||s.deviceCount18<=0||s.read174||
       s.registration!=C::Registration::complete||s.cursor168< -1||s.cursor168>=s.memberCount154||
       s.failedCourseDevices||s.failedCourseMembers)return fail(d,E::phase,0x131230);
    storage_=std::make_unique<Storage>();audioRoot_=root;resolver_=std::move(resolver);config_=config;
    attempted_=true;acquired_=true;s.musicOwner=this;
    //1315B0 always precedes170. No132820 cached160 shortcut or bank filter.
    ++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0;
    auto n=static_cast<std::int32_t>(s.cursor168);const auto start=n;
    do {
        if(s.members150[n].filename08&&courseSourceName308F00(name,s.members150[n].filename08)){
            s.cursor168=n;winner_=n;break;
        }
        if(++n==s.memberCount154)n=0;
    }while(n!=start);
    d.sourceEffectsCommitted=true;
    if(winner_!=-1)return fail(d,E::unsupported,0x131354); // Preserve foreign winner; never adopt/skip it.
    observedMode170_=s.sourceFileMode170;
    if(!observedMode170_){returned_=true;returned=-1;return true;} // rb read-only miss131318/131348.
    // Native authentication admission is not a retail13C980 NULL return.
    // Retail static directory record16 at36C390 names \\AUDIO\\RACEMUS.JUB;1,
    // LBA0xAC3, seven sectors, exact byte length0x36DB. No POD is involved.
    storage_->pendingInput=std::make_shared<std::vector<std::uint8_t>>(14043u);
    std::ifstream file(audioRoot_/"RACEMUS.JUB",std::ios::binary);
    auto& bytes=*storage_->pendingInput;
    if(!file||!file.read(reinterpret_cast<char*>(bytes.data()),bytes.size())||file.peek()!=std::char_traits<char>::eof()||file.bad()||
       !OriginalLoadingTextureSource50LA::hashMatches(bytes.data(),bytes.size(),
          "bff85d5a51a94157669be85bb90bb4d6be8bf4d802d95d08b1d2b235e9483bf1"))
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
    constexpr char filename[]="audio\\racemus.jub";
    row.filename08=new(std::nothrow) char[sizeof(filename)];if(!row.filename08)return fail(d,E::allocation,0x12E578);
    std::memcpy(row.filename08,filename,sizeof(filename));
    row.deviceID00=device_;row.logicalStart0C=0;row.cursor10=0;row.length14=length;
    row.auxiliarySlot04=0;row.associatedObject1C=nullptr;
    //131230 passes its stack+48 out slot, NOT the132820 cached lookup160.
    // member_ retains that returned numeric index; shared cursor160 is untouched.
    row.originalMember=nullptr;row.loadingRaOwner=nullptr;row.musicOwner=this;row.canonicalName=nullptr;row.canonicalData=input_->data();
    if(!fileIoAssertions().looseName8208_3E1AC0.reach(device.name04==nullptr,d,0x12E080)||
       !fileIoAssertions().looseFree820C_3E1AC8.reach(device.flags0C&1,d,0x12E0EC))return false;
    device.flags0C=4;device.handle10=storage_->pendingHandle.release();device.cursor08=0;device.length14=length;
    device.name04=new(std::nothrow) char[sizeof(filename)];if(!device.name04)return fail(d,E::allocation,0x12E148);
    std::memcpy(device.name04,filename,sizeof(filename));device.payload18=0;device.tableOffset1C=0;device.tableBytes20=0;device.byte24=0;device.id00=device_;
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
