// Inside rr::assets after complete Resolver storage and shared FileManager helpers.
struct OriginalLoadingRaFile50LA::Storage {
    std::unique_ptr<OriginalStandardTextureResolver50LA::FreshFileCapacity::Handle> pendingHandle;
    bool nativeCanonicalAdmitted=false,devicePublished=false,memberPublished=false;
};
OriginalLoadingRaFile50LA::OriginalLoadingRaFile50LA()noexcept=default;
OriginalLoadingRaFile50LA::~OriginalLoadingRaFile50LA()noexcept {
    if(acquired_&&!nativeClosed_)std::terminate();
}
std::shared_ptr<OriginalLoadingRaFile50LA> OriginalLoadingRaFile50LA::admitNative(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<const OriginalLoadingTextureSource50LA> source,D& d)noexcept try {
    d={};if(!resolver||!source){fail(d,E::phase,0x24DAF8);return {};}
    auto out=std::shared_ptr<OriginalLoadingRaFile50LA>(new OriginalLoadingRaFile50LA);
    out->storage_=std::make_unique<Storage>();out->resolver_=std::move(resolver);out->source_=std::move(source);
    out->self_=out;return out;
}catch(...){d.status=OriginalStandardTextureStatus50LA::allocation;d.sourcePC=0x24DAF8;return {};}
bool OriginalLoadingRaFile50LA::begin132820(std::size_t& index,
    OriginalStandardTextureResolver50LA::UiSelection& selected,std::shared_ptr<OriginalLoadingPstFile50LA>& pending,
    std::shared_ptr<OriginalStandardTextureResource50LA>& cached,D& d)noexcept {
    if(!resolver_)return fail(d,E::phase,0x132820);
    return resolver_->beginLoadingRa132820(*this,index,selected,pending,cached,d);
}
bool OriginalLoadingRaFile50LA::finishLoad24E094(const std::shared_ptr<OriginalLoadingPstFile50LA>& p,
    const std::shared_ptr<OriginalStandardTextureResource50LA>& resource,D& d)noexcept {
    d={};if(!resolver_||!p||p->raRoot_.get()!=this||p->root_)return fail(d,E::phase,0x24E094);
    return resolver_->associateLoadingPst1326A0(*p,resource,d)&&resolver_->closeLoadingPst131040(*p,false,d);
}
bool OriginalLoadingRaFile50LA::abortPendingNative(const std::shared_ptr<OriginalLoadingPstFile50LA>& p,D& d)noexcept {
    d={};if(!p)return true;
    if(!resolver_||p->raRoot_.get()!=this||p->root_)return fail(d,E::phase,0);
    return resolver_->closeLoadingPst131040(*p,true,d);
}
bool OriginalLoadingRaFile50LA::closeNative(D& d)noexcept {
    d={};if(nativeClosed_)return true;
    if(!resolver_||thread_!=std::this_thread::get_id())return fail(d,E::phase,0);
    return resolver_->closeLoadingRaNative(*this,d);
}
bool OriginalStandardTextureResolver50LA::beginLoadingRa132820(OriginalLoadingRaFile50LA& owner,
    std::size_t& index,UiSelection& selected,std::shared_ptr<OriginalLoadingPstFile50LA>& pending,
    std::shared_ptr<OriginalStandardTextureResource50LA>& cached,D& d)noexcept try {
    d={};index=static_cast<std::size_t>(-1);cached.reset();selected=UiSelection{};
    if(owner.resolver_.get()!=this||owner.thread_!=std::this_thread::get_id()||owner.attempted_||owner.nativeClosed_||
       !owner.storage_||!owner.source_||pending||unjoinedLoadingRaFileHistory_||!freshFileCapacity_||!scratch_)
        return fail(d,E::phase,0x132820);
    auto& s=*freshFileCapacity_;
    if(s.streamLibraryOwner||s.musicOwner||s.loadingRaOwner||s.courseRootOwner||s.courseStage!=FreshFileCapacity::CourseStage::none||
       s.registration!=FreshFileCapacity::Registration::complete||s.templateStage!=2||s.templateMember!=-1||
       s.templateFailed||!s.namesReadAttempted||!s.devices14||s.deviceCount18!=1||s.freeDevice10!=-1||
       !s.members150||s.memberCount154<=0||s.sourceFileMode170!=1||s.read174||
       s.cursor160< -1||s.cursor160>=s.memberCount154||s.cursor168< -1||s.cursor168>=s.memberCount154)
        return fail(d,E::phase,0x132820);
    for(std::int32_t n=0;n<s.memberCount154;++n)if(s.members150[n].flags18&4)return fail(d,E::phase,0x132820);
    // Native immutable source/row and attempt admission, not source FileManager
    // registration or premature Resource allocation. No generic marker is set.
    if(!admitLoadingRaCanonicalNative(owner.source_,owner.sourceIndex_,selected,d))return false;
    owner.storage_->nativeCanonicalAdmitted=true;index=owner.sourceIndex_;
    pending=std::shared_ptr<OriginalLoadingPstFile50LA>(new OriginalLoadingPstFile50LA);
    auto& p=*pending;p.raRoot_=owner.self_.lock();p.sourceIndex_=index;
    if(!p.raRoot_)return fail(d,E::phase,0x132820);
    owner.attempted_=true;p.searchReached_=true;
    constexpr const char* name="UI\\Textures\\LD_RA.PST";
    bool found=false;
    if(s.cursor160!=-1)found=s.members150[s.cursor160].filename08&&courseSourceName308F00(name,s.members150[s.cursor160].filename08);
    if(!found){
        ++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0;
        auto n=static_cast<std::int32_t>(s.cursor168);const auto start=n;
        do{if(s.members150[n].filename08&&courseSourceName308F00(name,s.members150[n].filename08)){s.cursor168=n;found=true;break;}
            if(++n==s.memberCount154)n=0;
        }while(n!=start);
        s.cursor160=found?s.cursor168:-1;
    }
    d.sourceEffectsCommitted=true;
    if(found)return fail(d,E::phase,0x1329A0); // Foreign loose row/cache cannot be adopted from equal bytes.
    owner.acquired_=true;s.loadingRaOwner=&owner;
    // Exact12DCB0 no-free-device branch. Count is stored BEFORE source realloc14.
    const auto oldCount=s.deviceCount18;++s.deviceCount18;
    auto* grown=new(std::nothrow) FreshFileCapacity::Device[s.deviceCount18];
    if(!grown){s.failedCourseDevices=s.devices14;s.failedCourseDeviceCapacity=oldCount;s.devices14=nullptr;
        return fail(d,E::allocation,0x12DD30);}
    std::memcpy(grown,s.devices14,std::size_t(oldCount)*sizeof(FreshFileCapacity::Device));delete[] s.devices14;s.devices14=grown;
    owner.device_=s.deviceCount18-1;s.freeDevice10=-1;auto& device=s.devices14[owner.device_];
    device.flags0C=1;device.handle10=nullptr;device.id00=owner.device_;device.name04=nullptr;
    device.cursor08=0;device.length14=0;device.byte24=0;owner.storage_->devicePublished=true;
    // Original rb mode and authenticated immutable-input backend, with a real
    // per-handle physical cursor. No compressed archive view is fabricated.
    owner.storage_->pendingHandle.reset(new(std::nothrow) FreshFileCapacity::Handle(owner.source_,owner.source_->bytes_.data(),owner.source_->bytes_.size()));
    if(!owner.storage_->pendingHandle){s.retireDevice12EB60(device);return fail(d,E::allocation,0x13296C);}
    auto& input=*owner.storage_->pendingHandle;
    if(!input.seek(input.size()))return fail(d,E::source,0x13D8F0);
    const auto length=static_cast<std::uint32_t>(input.cursor); //13D950 after seekend.
    //12E2B0 scans actual existing filenames, then uses the actual free-member
    // slot and wrap scan. Growth5 is used ONLY if no free row exists.
    for(std::int32_t n=0;n<s.memberCount154;++n)if(s.members150[n].filename08&&courseSourceName308F00(name,s.members150[n].filename08))
        return fail(d,E::phase,0x12E324);
    if(s.freeMember14C==-1){
        const auto old=s.memberCount154;auto* grownMembers=new(std::nothrow) FreshFileCapacity::Member[std::size_t(old)+5];
        if(!grownMembers){s.failedCourseMembers=s.members150;s.failedCourseMemberCapacity=old;s.members150=nullptr;
            owner.storage_->pendingHandle.reset();s.retireDevice12EB60(device);return fail(d,E::allocation,0x13296C);}
        std::memcpy(grownMembers,s.members150,std::size_t(old)*sizeof(FreshFileCapacity::Member));delete[] s.members150;s.members150=grownMembers;
        s.freeMember14C=old;for(std::int32_t n=old;n<old+5;++n){grownMembers[n].initialize();++s.memberCount154;}
    }
    const auto member=s.freeMember14C;s.freeMember14C=-1;
    if(member<0||member>=s.memberCount154)return fail(d,E::phase,0x12E458);
    for(std::int32_t n=1;n<s.memberCount154;++n){const auto next=(member+n)%s.memberCount154;if(s.members150[next].flags18&1){s.freeMember14C=next;break;}}
    owner.member_=member;auto& row=s.members150[member];row.flags18=0;
    if(!memberNameAssertion12E4E8(row.filename08,d))return false;
    constexpr auto nameBytes=sizeof("UI\\Textures\\LD_RA.PST");
    row.filename08=new(std::nothrow) char[nameBytes];
    if(!row.filename08)return fail(d,E::allocation,0x12E578);
    std::memcpy(row.filename08,name,nameBytes);
    row.deviceID00=owner.device_;row.logicalStart0C=0;row.cursor10=0;row.length14=length;
    row.auxiliarySlot04=0;row.associatedObject1C=nullptr;s.cursor160=member; //12E5A4 exact out-param.
    row.originalMember=nullptr;row.loadingRaOwner=&owner;
    row.canonicalName=&storage_->entries[index].file->member;row.canonicalData=owner.source_->bytes_.data();
    owner.storage_->memberPublished=true;p.row_=member;
    if(!fileIoAssertions().looseName8208_3E1AC0.reach(device.name04==nullptr,d,0x12E080)||
       !fileIoAssertions().looseFree820C_3E1AC8.reach(device.flags0C&1,d,0x12E0EC))return false;
    device.flags0C=4;device.handle10=owner.storage_->pendingHandle.release();device.cursor08=0;device.length14=length;
    device.name04=new(std::nothrow) char[nameBytes];if(!device.name04)return fail(d,E::allocation,0x12E148);
    std::memcpy(device.name04,name,nameBytes);
    device.payload18=0;device.tableBytes20=0;device.tableOffset1C=0;device.byte24=0;device.id00=owner.device_;
    owner.registered_=true;
    //132820 itself opens the numeric row after12DCB0. Never131230 again.
    if(!openExactSourceMember131380(&p,member,p.handle_,d))return false;
    p.returned132820_=true;return true;
}catch(...){return fail(d,E::allocation,0x132820);}
bool OriginalStandardTextureResolver50LA::loadingRaFileCurrent(const OriginalLoadingPstFile50LA& p,bool open)const noexcept {
    const auto& owner=p.raRoot_;
    if(!owner||p.root_||owner->resolver_.get()!=this||owner->thread_!=std::this_thread::get_id()||owner->nativeClosed_||
       !owner->registered_||!p.returned132820_||p.nativeAborted_||!freshFileCapacity_||!storage_||
       p.sourceIndex_!=owner->sourceIndex_||p.sourceIndex_>=storage_->entries.size())return false;
    const auto& s=*freshFileCapacity_;
    if(s.loadingRaOwner!=owner.get()||!s.members150||!s.devices14||p.row_!=owner->member_||p.row_<0||p.row_>=s.memberCount154||
       owner->device_<0||owner->device_>=s.deviceCount18)return false;
    const auto& row=s.members150[p.row_];const auto& file=*storage_->entries[p.sourceIndex_].file;
    if(row.loadingRaOwner!=owner.get()||row.deviceID00!=owner->device_||row.originalMember||!row.filename08||
       row.canonicalData!=owner->source_->bytes_.data()||row.length14!=owner->source_->bytes_.size()||
       file.loadingRa!=owner->source_||file.retired||row.cursor10>row.length14)return false;
    return !open||(!p.closed_&&!p.hitReturned_&&p.handle_==p.row_&&row.courseConfigOwner==&p&&(row.flags18&4));
}
bool OriginalStandardTextureResolver50LA::readLoadingRa130560(OriginalLoadingPstFile50LA& p,
    void* output,std::size_t requested,std::size_t& copied,D& d)noexcept {
    d={};copied=0;if(!loadingRaFileCurrent(p,true)||(!output&&requested))return fail(d,E::phase,0x130560);
    auto& s=*freshFileCapacity_;auto& member=s.members150[p.handle_];
    const auto amount=std::min<std::size_t>(requested,member.length14-member.cursor10);
    if(!activateCourseDevice12E8B0(member.deviceID00,member.logicalStart0C+member.cursor10,d))return false;
    auto& device=s.devices14[member.deviceID00];
    if(device.flags0C!=4||!device.handle10||device.handle10->looseOwner!=p.raRoot_->source_||device.payload18)
        return fail(d,E::phase,0x13059C);
    if(member.auxiliarySlot04==-1||amount>=Scratch::bytes){
        if(!device.handle10->seek(std::size_t(device.payload18)+device.cursor08))return fail(d,E::source,0x1305DC);
        if(!device.handle10->read(output,amount))return fail(d,E::source,0x1305F4);
        copied=amount;device.cursor08+=static_cast<std::uint32_t>(amount); //13061C only after full read.
    }else{
        if(member.auxiliarySlot04<0||std::size_t(member.auxiliarySlot04)>=scratch_->slots.size())return fail(d,E::phase,0x130620);
        auto& slot=scratch_->slots[member.auxiliarySlot04];
        if(!slot.pointer00||slot.status08!=p.handle_)return fail(d,E::phase,0x130620);
        auto within=std::int64_t(device.cursor08)-slot.offset10;
        if(within<0||within>=std::int64_t(Scratch::bytes)){
            slot.offset10=static_cast<std::int32_t>(device.cursor08); //130654 BEFORE physical read.
            const auto absolute=std::size_t(device.payload18)+device.cursor08;
            if(absolute>device.length14)return fail(d,E::source,0x130668);
            const auto bytes=std::min<std::size_t>(Scratch::bytes,device.length14-absolute);
            if(!device.handle10->seek(absolute))return fail(d,E::source,0x130690);
            if(!device.handle10->read(slot.pointer00,bytes))return fail(d,E::source,0x1306B0);
            slot.nativeRawValidBytes=static_cast<std::uint32_t>(bytes);within=0;
        }
        const auto n=std::min<std::size_t>(amount,Scratch::bytes-std::size_t(within));
        if(!slot.nativeRawValidBytes||std::size_t(within)>*slot.nativeRawValidBytes||n>*slot.nativeRawValidBytes-std::size_t(within))
            return fail(d,E::phase,0x130700);
        if(n)std::memcpy(output,slot.pointer00+within,n);
        copied=n;device.cursor08+=static_cast<std::uint32_t>(n); //130718.
        if(n!=amount)return fail(d,E::unsupported,0x130734); // Exact cohort never crosses this small-read window.
    }
    member.cursor10+=static_cast<std::uint32_t>(copied);d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResolver50LA::closeLoadingRa131040(OriginalLoadingPstFile50LA& p,bool nativeAbort,D& d)noexcept {
    d={};auto owner=p.raRoot_;
    if(!owner||p.root_||owner->resolver_.get()!=this||owner->thread_!=std::this_thread::get_id())return fail(d,E::phase,0x131040);
    if(p.closed_||p.nativeAborted_)return true;
    if(p.handle_!=-1){
        if(!freshFileCapacity_||!scratch_)return fail(d,E::phase,0x131040);
        auto& s=*freshFileCapacity_;
        if(!closeAssertions131040(p.handle_,s.memberCount154,d))return false;
        if(!s.members150||!s.devices14||s.loadingRaOwner!=owner.get()||p.handle_!=owner->member_||
           p.handle_<0||p.handle_>=s.memberCount154)return fail(d,E::phase,0x131040);
        auto& member=s.members150[p.handle_];
        if(member.loadingRaOwner!=owner.get()||member.courseConfigOwner!=&p||!(member.flags18&4)||
           member.deviceID00!=owner->device_||owner->device_<0||owner->device_>=s.deviceCount18)
            return fail(d,E::phase,0x131040);
        auto& device=s.devices14[owner->device_];
        member.cursor10=0;member.flags18&=~std::uint32_t(4);
        if(member.auxiliarySlot04>=0&&std::size_t(member.auxiliarySlot04)<scratch_->slots.size()&&
           scratch_->slots[member.auxiliarySlot04].status08==p.handle_)scratch_->selected148=member.auxiliarySlot04;
        // Actual1311D8 loose close; source1C nonNULL retains registration.
        delete device.handle10;device.handle10=nullptr;member.courseConfigOwner=nullptr;p.handle_=-1;
        if(!member.associatedObject1C&&!(member.flags18&8)){
            s.retireDevice12EB60(device);s.retireMember12EAA0(owner->member_);owner->registered_=false;
        }
        d.sourceEffectsCommitted=true;
    }else if(!nativeAbort&&!p.hitReturned_)return fail(d,E::phase,0x131040);
    if(nativeAbort)p.nativeAborted_=true;else p.closed_=true;
    return true;
}
bool OriginalStandardTextureResolver50LA::closeLoadingRaNative(OriginalLoadingRaFile50LA& owner,D& d)noexcept {
    d={};if(owner.resolver_.get()!=this||owner.thread_!=std::this_thread::get_id()||!owner.storage_)return fail(d,E::phase,0);
    if(owner.nativeClosed_)return true;
    if(owner.storage_->nativeCanonicalAdmitted){
        if(!storage_||owner.sourceIndex_>=storage_->entries.size())return fail(d,E::phase,0);
        const auto& file=*storage_->entries[owner.sourceIndex_].file;
        if(file.loadingRa!=owner.source_||file.pstBorrowers||file.cacheIdentity||!file.cached.expired())return fail(d,E::phase,0);
    }
    if(owner.acquired_){
        if(freshFileCapacity_&&(freshFileCapacity_->streamLibraryOwner||freshFileCapacity_->musicOwner))return fail(d,E::phase,0);
        if(!freshFileCapacity_||!scratch_||freshFileCapacity_->loadingRaOwner!=&owner)return fail(d,E::phase,0);
        auto& s=*freshFileCapacity_;
        auto* members=s.members150?s.members150:s.failedCourseMembers;
        const auto count=s.members150?s.memberCount154:s.failedCourseMemberCapacity;
        if(owner.member_>=0){
            if(!members||owner.member_>=count)return fail(d,E::phase,0);
            const auto& row=members[owner.member_];
            if(row.courseConfigOwner||(row.flags18&4)||row.associatedObject1C)return fail(d,E::phase,0);
            if(owner.registered_&&row.loadingRaOwner!=&owner)return fail(d,E::phase,0);
        }
        auto* devices=s.devices14?s.devices14:s.failedCourseDevices;
        const auto deviceCapacity=s.devices14?s.deviceCount18:s.failedCourseDeviceCapacity;
        if(owner.device_>=0&&(!devices||owner.device_>=deviceCapacity))return fail(d,E::phase,0);
        // All gates precede native withdrawal. Failed realloc keeps sourceNULL
        // and old native allocation in the manager, never a repaired source table.
        owner.storage_->pendingHandle.reset();
        if(owner.member_>=0&&!(members[owner.member_].flags18&1)){
            if(s.members150)s.retireMember12EAA0(owner.member_);
            else { // Native-only disposal of this exact retained old allocation.
                auto& row=members[owner.member_];
                if(row.auxiliarySlot04>=0&&std::size_t(row.auxiliarySlot04)<scratch_->slots.size()&&
                   scratch_->slots[row.auxiliarySlot04].status08==owner.member_){
                    scratch_->slots[row.auxiliarySlot04].status08=-1;scratch_->selected148=row.auxiliarySlot04;}
                delete[] row.filename08;row.filename08=nullptr;row.loadingRaOwner=nullptr;
                row.canonicalName=nullptr;row.canonicalData=nullptr;row.flags18=1;row.deviceID00=0;
            }
        }
        if(owner.device_>=0&&!(devices[owner.device_].flags0C&1))
            s.retireDevice12EB60(devices[owner.device_]); // Native withdrawal, including retained old device14 backing.
        s.loadingRaOwner=nullptr;
    }
    if(owner.storage_->nativeCanonicalAdmitted){auto& file=*storage_->entries[owner.sourceIndex_].file;
        file.retired=true; // Exact immutable backing remains stable, no source device claim.
    }
    owner.registered_=false;owner.nativeClosed_=true;return true;
}
bool OriginalStandardTextureResolver50LA::loadingRaRetainedCurrent()const noexcept {
    if(!freshFileCapacity_||!storage_)return false;const auto& s=*freshFileCapacity_;const auto* owner=s.loadingRaOwner;
    if(!owner||owner->resolver_.get()!=this||owner->thread_!=std::this_thread::get_id()||!owner->registered_||owner->nativeClosed_||
       !owner->source_||!s.devices14||!s.members150||owner->device_<0||owner->device_>=s.deviceCount18||
       owner->member_<0||owner->member_>=s.memberCount154||owner->sourceIndex_>=storage_->entries.size())return false;
    const auto& device=s.devices14[owner->device_];const auto& member=s.members150[owner->member_];
    const auto& file=*storage_->entries[owner->sourceIndex_].file;const auto resource=file.cached.lock();
    return device.flags0C==4&&!device.handle10&&device.id00==owner->device_&&device.name04&&
        device.length14==owner->source_->bytes_.size()&&!device.payload18&&!device.tableOffset1C&&!device.tableBytes20&&
        member.loadingRaOwner==owner&&member.deviceID00==owner->device_&&!(member.flags18&5)&&!member.courseConfigOwner&&
        member.cursor10==0&&member.associatedObject1C&&resource&&resource.get()==member.associatedObject1C&&
        file.cacheIdentity==resource.get()&&file.loadingRa==owner->source_&&!file.retired&&resource->constructed24DA30()&&
        resource->loadingPstFile_&&resource->loadingPstFile_->raRoot_.get()==owner&&resource->loadingPstFile_->associated_&&
        resource->loadingPstFile_->closed_&&!resource->loadingPstFile_->associationCleared_;
}
bool OriginalStandardTextureResolver50LA::canWithdrawLoadingRaAssociationNative(
    const OriginalLoadingPstFile50LA& p,const OriginalStandardTextureResource50LA& resource)const noexcept {
    const auto& owner=p.raRoot_;
    if(!owner||p.root_||owner->resolver_.get()!=this||owner->thread_!=std::this_thread::get_id()||owner->nativeClosed_||
       p.boundResource_!=&resource||!p.associated_||p.handle_!=-1||!freshFileCapacity_)return false;
    if(p.nativeAssociationWithdrawn_)return true;
    const auto& s=*freshFileCapacity_;
    if(s.loadingRaOwner!=owner.get()||s.members150||!s.failedCourseMembers||p.row_!=owner->member_||
       p.row_<0||p.row_>=s.failedCourseMemberCapacity)return false;
    if(!storage_||p.sourceIndex_!=owner->sourceIndex_||p.sourceIndex_>=storage_->entries.size())return false;
    const auto& file=*storage_->entries[p.sourceIndex_].file;
    if(file.loadingRa!=owner->source_||file.cacheIdentity!=&resource||file.cached.lock().get()!=&resource)return false;
    const auto& row=s.failedCourseMembers[p.row_];
    return row.loadingRaOwner==owner.get()&&row.deviceID00==owner->device_&&!row.courseConfigOwner&&!(row.flags18&4)&&
        row.associatedObject1C==&resource&&row.canonicalData==owner->source_->bytes_.data();
}
bool OriginalStandardTextureResolver50LA::withdrawLoadingRaAssociationNative(
    OriginalLoadingPstFile50LA& p,const OriginalStandardTextureResource50LA& resource,D& d)noexcept {
    d={};if(!canWithdrawLoadingRaAssociationNative(p,resource))return fail(d,E::phase,0);
    if(p.nativeAssociationWithdrawn_)return true;
    // The source150 remains NULL. Only the independently retained old native
    // allocation loses this exact reference; never certify source1327D0 here.
    freshFileCapacity_->failedCourseMembers[p.row_].associatedObject1C=nullptr;
    p.nativeAssociationWithdrawn_=true;return true;
}
