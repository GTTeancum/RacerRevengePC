// Included inside rr::assets after the resolver's actual Fresh storage.
bool OriginalStandardTextureResolver50LA::registerTrackBank12CEA0(OriginalRaTrackSourceFile50LA& root,D& d)noexcept {
    if(freshFileCapacity_&&freshFileCapacity_->waterPoolFileBorrower)return fail(d,E::phase,0);
    d={};using C=FreshFileCapacity;
    if(!freshFileCapacity_||!scratch_||!root.archive_||!root.provider_||
       root.archive_->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::trackRa))return fail(d,E::source,0x12CEA0);
    auto& s=*freshFileCapacity_;if(s.streamLibraryOwner||s.musicOwner)return fail(d,E::phase,0);auto& t=s.track;
    if(t.attempted||t.root||s.provider.get()!=root.provider_.get()||s.registration!=C::Registration::complete||
       !s.devices14||!s.members150||s.deviceCount18<=0||s.memberCount154<=0||s.read174)
        return fail(d,E::phase,0x12CEA0);
    // Native preadmission only: the existing exact canonical RA mount must
    // already own the whole authenticated directory. Never append, reactivate,
    // replace, or retrofit its absent encoded backing here.
    if(!root.provider_->sealed())return fail(d,E::phase,0x12CEA0);
    std::size_t nativeMount=static_cast<std::size_t>(-1);
    for(std::size_t n=0;n<root.provider_->mounts_.size();++n){const auto& bank=*root.provider_->mounts_[n];
        if(!OriginalStandardArchiveProvider50LA::sameName(bank.name,"TRACKS/RA.RES"))continue;
        if(nativeMount!=static_cast<std::size_t>(-1)||!bank.active||bank.digest!=root.archive_->identity()->sha256||
           bank.archiveBytes!=root.archive_->identity()->bytes||bank.rows.size()!=root.archive_->memberCount())return fail(d,E::source,0x12CEA0);
        for(std::size_t ordinal=0;ordinal<bank.rows.size();++ordinal){const auto actual=root.archive_->member(ordinal);const auto& row=bank.rows[ordinal];
            if(!actual.identity||row.name!=actual.identity->name||row.offset!=actual.identity->logicalOffset||row.size!=actual.size||
               std::memcmp(row.data,actual.data,actual.size))return fail(d,E::source,0x12CEA0);}
        nativeMount=n;
    }
    if(nativeMount==static_cast<std::size_t>(-1))return fail(d,E::source,0x12CEA0);
    // Native custody precedes every reached source mutation. No Course root,
    // open member, loose RA device, or preexisting namespace lease is adopted.
    t.attempted=true;t.root=&root;t.archive=root.archive_;root.acquired_=true;
    t.mount=nativeMount;t.generation=root.provider_->mounts_[nativeMount]->generation;
    constexpr std::string_view name="tracks\\ra.res";
    for(std::int32_t i=0;i<s.deviceCount18;++i)if(s.devices14[i].name04&&courseSourceName308F00(name,s.devices14[i].name04))
        return fail(d,E::unsupported,0x12CF58); // Exact duplicate/flag20 retirement capability not issued by this owner.
    auto selected=scratch_->selected148;
    if(selected!=-1){
        if(selected<0||std::size_t(selected)>=scratch_->slots.size()||!scratch_->slots[selected].pointer00)return fail(d,E::phase,0x12CF98);
        auto& slot=scratch_->slots[selected];slot.status08=-1;slot.offset10=-0x6000;scratch_->selected148=-1;
    }else for(std::size_t n=0;n<scratch_->slots.size();++n){auto& slot=scratch_->slots[n];
        if(slot.status08==-2)continue;
        if(!slot.pointer00)return fail(d,E::phase,0x12D004);
        if(slot.status08!=-1){if(slot.status08<0||slot.status08>=s.memberCount154)return fail(d,E::phase,0x12D01C);
            if(s.members150[slot.status08].flags18&4)continue;}
        slot.offset10=-0x6000;slot.status08=-1;selected=static_cast<std::int32_t>(n);break;
    }
    // No-slot diagnostic12D078 does not terminate source registration: the
    // original12D8FC branch reads each entry through local stack buffers.
    t.registrationSlot=selected;d.sourceEffectsCommitted=true;
    if(s.freeDevice10==-1){const auto old=s.deviceCount18;++s.deviceCount18;
        auto* grown=new(std::nothrow) C::Device[s.deviceCount18];
        if(!grown){s.failedCourseDevices=s.devices14;s.failedCourseDeviceCapacity=old;s.devices14=nullptr;t.failed=true;return fail(d,E::allocation,0x12D0C4);}
        std::memcpy(grown,s.devices14,std::size_t(old)*sizeof(C::Device));delete[] s.devices14;s.devices14=grown;
        t.device=old;s.freeDevice10=-1;auto& row=s.devices14[old];row.flags0C=1;row.handle10=nullptr;row.id00=old;
        row.name04=nullptr;row.cursor08=0;row.length14=0;row.byte24=0;
    }else{
        if(!fileIoAssertions().reuse81E8_3E1A80.reach(s.freeDevice10<s.deviceCount18,d,0x12D1BC))return false;
        if(s.freeDevice10<0||s.freeDevice10>=s.deviceCount18)return fail(d,E::phase,0x12D1E8);
        t.device=s.freeDevice10;s.freeDevice10=-1;
        for(std::int32_t n=1;n<s.deviceCount18;++n){const auto next=(t.device+n)%s.deviceCount18;if(s.devices14[next].flags0C&1){s.freeDevice10=next;break;}}
        if(!fileIoAssertions().reuseNext81EC_3E1A88.reach(s.freeDevice10<s.deviceCount18,d,0x12D2A8))return false;
    }
    d.sourceEffectsCommitted=true;
    t.registrationInput.reset(new(std::nothrow) C::Handle(OriginalUiEncodedArchiveView50KG(s.provider.get(),t.archive)));
    if(!t.registrationInput)return fail(d,E::allocation,0x13C980);
    auto& in=*t.registrationInput;
    std::uint32_t version=0,meta=0,flags=0,user=0,tableBytes=0,count=0,directoryBytes=0;
    if(!in.word(version)||!in.word(meta)||!in.word(flags)||!in.word(user)||!in.word(tableBytes)||version!=3||flags!=0x340||user||
       tableBytes<2||(tableBytes&1)||std::uint64_t(tableBytes)+20!=std::uint64_t(meta)+8||!in.seek(std::size_t(tableBytes)+20)||
       !in.word(count)||!in.word(directoryBytes)||count!=1010||directoryBytes!=38132||count!=t.archive->memberCount())return fail(d,E::source,0x12D51C);
    const auto payload=static_cast<std::uint32_t>(in.cursor+directoryBytes),fileLength=static_cast<std::uint32_t>(in.input.size());
    std::size_t remaining=directoryBytes,at=0,carried=0;
    std::uint8_t* buffer=selected<0?nullptr:scratch_->slots[selected].pointer00;
    const auto refill=[&]()noexcept{const auto amount=std::min<std::size_t>(remaining,Scratch::bytes-carried-1);
        remaining-=amount;if(!in.read(buffer+carried,amount))return false;at=0;return true;};
    if(buffer&&!refill())return fail(d,E::source,0x12D780);
    const auto localWord=[](const std::uint8_t* b)noexcept{return std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|(std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);};
    for(std::uint32_t ordinal=0;ordinal<count;){
        std::array<char,1025> localName{};std::uint32_t nameBytes=0,offset=0,length=0;std::size_t entryStart=at;
        if(buffer){
            bool fits=Scratch::bytes-at>=5;
            if(fits){nameBytes=localWord(buffer+at);fits=nameBytes<=1024&&std::size_t(nameBytes)+8<Scratch::bytes-at-4;}
            if(!fits){carried=Scratch::bytes-entryStart-1;std::memmove(buffer,buffer+entryStart,carried);
                if(!remaining||!refill())return fail(d,E::source,0x12D780);continue;}
            at+=4;std::memcpy(localName.data(),buffer+at,nameBytes);at+=nameBytes;
            offset=localWord(buffer+at);at+=4;length=localWord(buffer+at);at+=4;
        }else{
            if(!in.word(nameBytes)||!nameBytes||nameBytes>1024||!in.read(localName.data(),nameBytes)||!in.word(offset)||!in.word(length))return fail(d,E::source,0x12D9B8);
        }
        const auto original=t.archive->member(ordinal);const std::string_view requested(localName.data(),nameBytes);
        if(!original.identity||!nameBytes||requested!=original.identity->name||offset!=original.identity->logicalOffset||length!=original.size)return fail(d,E::source,0x12D890);
        bool duplicate=false;for(std::int32_t n=0;n<s.memberCount154;++n)if(s.members150[n].filename08&&courseSourceName308F00(requested,s.members150[n].filename08)){duplicate=true;break;}
        if(!duplicate){
            if(s.freeMember14C==-1){const auto old=s.memberCount154;const auto growth=static_cast<std::int32_t>(count-ordinal);
                auto* rows=new(std::nothrow) C::Member[std::size_t(old)+growth];
                if(!rows){s.failedCourseMembers=s.members150;s.failedCourseMemberCapacity=old;s.members150=nullptr;t.failed=true;return fail(d,E::allocation,buffer?0x12D898:0x12D9E0);}
                std::memcpy(rows,s.members150,std::size_t(old)*sizeof(C::Member));delete[] s.members150;s.members150=rows;s.freeMember14C=old;
                for(std::int32_t n=old;n<old+growth;++n){rows[n].initialize();++s.memberCount154;}
            }
            const auto index=s.freeMember14C;if(index<0||index>=s.memberCount154)return fail(d,E::phase,0x12E2B0);s.freeMember14C=-1;
            for(std::int32_t n=1;n<s.memberCount154;++n){const auto next=(index+n)%s.memberCount154;if(s.members150[next].flags18&1){s.freeMember14C=next;break;}}
            auto& row=s.members150[index];row.flags18=0;t.pendingMember=index;
            if(!memberNameAssertion12E4E8(row.filename08,d))return false;
            row.filename08=new(std::nothrow) char[std::size_t(nameBytes)+1];if(!row.filename08)return fail(d,E::allocation,0x12E578);
            std::memcpy(row.filename08,localName.data(),nameBytes+1);row.deviceID00=t.device;row.logicalStart0C=offset;row.cursor10=0;row.length14=length;
            row.auxiliarySlot04=0;row.associatedObject1C=nullptr;row.originalMember=original.identity;t.pendingMember=-1;
        }
        ++ordinal;
        if(buffer&&ordinal==count){ // Source's final reached carry copy precedes loop exit.
            carried=Scratch::bytes-entryStart-1;std::memmove(buffer,buffer+entryStart,carried);
        }
    }
    if(in.cursor!=payload)return fail(d,E::source,0x12DAB0);
    if(!in.seek(fileLength))return fail(d,E::source,0x12DAB8);t.registrationInput.reset();scratch_->selected148=selected;t.registrationSlot=-1;
    auto& device=s.devices14[t.device];if(!deviceAssertions12DAFC(device.name04,device.flags0C,d))return false;
    device.flags0C=flags|2;device.handle10=nullptr;device.name04=new(std::nothrow) char[name.size()+1];
    if(!device.name04)return fail(d,E::allocation,0x12DBF4);
    std::memcpy(device.name04,name.data(),name.size());device.name04[name.size()]=0;device.cursor08=0;device.length14=fileLength;
    device.payload18=payload;device.tableOffset1C=20;device.tableBytes20=tableBytes;device.byte24=0;t.registered=true;
    // Real a2=0: do not activate/preload. Canonical byte custody is reconciled
    // separately and does not issue any FileManager numeric row.
    const auto& canonicalBank=*root.provider_->mounts_[t.mount];
    for(std::int32_t n=0;n<s.memberCount154;++n){auto& row=s.members150[n];if(row.deviceID00!=t.device||!row.filename08)continue;
        bool found=false;
        for(std::size_t ordinal=0;ordinal<canonicalBank.rows.size();++ordinal){const auto original=t.archive->member(ordinal);
            if(original.identity!=row.originalMember)continue;
            const auto& canonical=canonicalBank.rows[ordinal];row.canonicalName=&canonical.name;row.canonicalData=canonical.data;found=true;break;
        }
        if(!found)return fail(d,E::source,0);
    }
    t.joined=true;return true;
}
bool OriginalStandardTextureResolver50LA::resizeTrackCache132E40(OriginalRaTrackSourceFile50LA& root,D& d)noexcept {
    if(freshFileCapacity_&&freshFileCapacity_->waterPoolFileBorrower)return fail(d,E::phase,0);
    d={};if(!freshFileCapacity_||!scratch_)return fail(d,E::phase,0x132E40);auto& s=*freshFileCapacity_;if(s.streamLibraryOwner||s.musicOwner)return fail(d,E::phase,0);auto& t=s.track;
    if(t.root!=&root||!t.registered||!s.devices14||s.read174||t.cacheAttempted)return fail(d,E::phase,0x132E40);
    std::uint32_t length=0;for(std::int32_t n=0;n<s.deviceCount18;++n)if(s.devices14[n].name04&&courseSourceName308F00("tracks\\ra.res",s.devices14[n].name04)){length=s.devices14[n].length14;break;}
    std::size_t budget=0x280000;
    constexpr std::array<std::size_t,10> ceilings{{0x10000,0x20000,0x40000,0x80000,0x100000,0x140000,0x180000,0x1c0000,0x200000,0x280000}};
    for(const auto ceiling:ceilings)if(length<=ceiling&&budget>=ceiling){budget=ceiling;break;}
    t.cacheAttempted=true;
    //132CD0(-1): synchronize only when current changes, then invalidate the
    // encoded resident window. No device physical handle or open member dies.
    if(s.current74!=-1){s.current74=-1;s.window7C=s.window80=-1;}
    s.active.fill(-1); //133068..7C; scratch slots/selected148 are unchanged.
    for(auto& input:s.inputs)if(input.pointer){::operator delete(input.pointer,std::align_val_t(FreshFileCapacity::alignment));input.pointer=nullptr;}
    s.total78=0; //133208. Length lanes retain history until each actual write.
    bool returned=true;const auto each=budget/8;
    for(std::size_t i=0;i<s.inputs.size();++i){auto& input=s.inputs[i];
        input.pointer=static_cast<std::uint8_t*>(::operator new(each,std::align_val_t(FreshFileCapacity::alignment),std::nothrow));
        if(i==0&&!input.pointer){input.length=each;input.lengthWritten=true;returned=false;}
        if(input.pointer){s.total78+=each;input.length=each;input.lengthWritten=true;}
    }
    //132E40 returns1330F0's first-allocation result, preserving later allocated
    // inputs even when first NULL. Do not substitute the old cache on failure.
    t.cacheReturned=returned;d.sourceEffectsCommitted=true;
    return returned?true:fail(d,E::allocation,0x13318C);
}
bool OriginalStandardTextureResolver50LA::closeTrackBankNative(OriginalRaTrackSourceFile50LA& root,D& d)noexcept {
    if(freshFileCapacity_&&freshFileCapacity_->waterPoolFileBorrower)return fail(d,E::phase,0);
    d={};if(!freshFileCapacity_||!scratch_)return fail(d,E::phase,0);auto& s=*freshFileCapacity_;if(s.streamLibraryOwner||s.musicOwner)return fail(d,E::phase,0);auto& t=s.track;
    if(!t.root)return !root.acquired_;
    if(t.root!=&root||root.occurrence_||root.nativeMissingLookupBorrowers_||s.particleConfigBorrower||s.sourceSkyFileBorrower||s.waterConfigBorrower)return fail(d,E::phase,0);
    auto* members=s.members150?s.members150:s.failedCourseMembers;const auto memberCount=s.members150?s.memberCount154:s.failedCourseMemberCapacity;
    auto* devices=s.devices14?s.devices14:s.failedCourseDevices;const auto deviceCount=s.devices14?s.deviceCount18:s.failedCourseDeviceCapacity;
    if((t.device>=0&&(!devices||t.device>=deviceCount))||(!members&&t.pendingMember>=0))return fail(d,E::phase,0);
    if(members)for(std::int32_t n=0;n<memberCount;++n)if((t.device>=0&&members[n].deviceID00==t.device)||n==t.pendingMember)
        if(members[n].courseConfigOwner||(members[n].flags18&4)||members[n].associatedObject1C)return fail(d,E::phase,0);
    // All gates precede withdrawals. Lost public arrays stay NULL: exact old
    // backing is native-only custody, never restored to manufacture source.
    t.registrationInput.reset();if(t.registrationSlot>=0)scratch_->selected148=t.registrationSlot;t.registrationSlot=-1;
    if(members)for(std::int32_t n=0;n<memberCount;++n)if((t.device>=0&&members[n].deviceID00==t.device)||n==t.pendingMember){
        if(s.members150)s.retireMember12EAA0(n);
        else {auto& row=members[n];if(row.auxiliarySlot04>=0&&std::size_t(row.auxiliarySlot04)<scratch_->slots.size()){
            auto& slot=scratch_->slots[row.auxiliarySlot04];if(slot.status08==n){slot.status08=-1;scratch_->selected148=row.auxiliarySlot04;}}
            delete[] row.filename08;row.filename08=nullptr;row.originalMember=nullptr;row.canonicalName=nullptr;row.canonicalData=nullptr;
            row.flags18=1;row.deviceID00=0;row.associatedObject1C=nullptr;}
    }
    if(t.device>=0){if(s.devices14)s.retireDevice12EB60(devices[t.device]);
        else {auto& row=devices[t.device];for(std::size_t at=0;at<s.active.size();++at)if(s.active[at]==t.device){for(std::size_t donor=s.active.size();donor>0;--donor)if(s.active[donor-1]!=-1){s.active[at]=s.active[donor-1];s.active[donor-1]=-1;break;}break;}
            if(s.current74==t.device){s.current74=-1;s.window7C=s.window80=-1;}
            delete row.handle10;row.handle10=nullptr;delete[] row.name04;row.name04=nullptr;row.flags0C=1;}
    }
    // This owner only withdraws its source/native transport capability. The
    // retained canonical namespace is separate caller/Domain custody.
    t.archive.reset();t.root=nullptr;t.device=t.pendingMember=-1;t.joined=false;t.registered=false;return true;
}
OriginalRaTrackSourceFile50LA::~OriginalRaTrackSourceFile50LA(){if(acquired_&&!nativeClosed_)std::terminate();}
bool OriginalRaTrackSourceFile50LA::mount12CEA0(std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider,std::shared_ptr<const OriginalUiArchive50KG> archive,D& d)noexcept {
    d={};if(attempted_||nativeClosed_||thread_!=std::this_thread::get_id()||!resolver||!provider||!archive||!archive->loaded())return fail(d,E::phase,0x12CEA0);
    attempted_=true;resolver_=std::move(resolver);provider_=std::move(provider);archive_=std::move(archive);
    joined_=resolver_->registerTrackBank12CEA0(*this,d);return joined_;
}
bool OriginalRaTrackSourceFile50LA::enterArchiveOnly132A20(D& d)noexcept {
    d={};if(!joined_||nativeClosed_||thread_!=std::this_thread::get_id()||!resolver_||!resolver_->freshFileCapacity_||resolver_->freshFileCapacity_->track.root!=this)return fail(d,E::phase,0x132A20);
    resolver_->freshFileCapacity_->sourceFileMode170=0;d.sourceEffectsCommitted=true;return true;
}
bool OriginalRaTrackSourceFile50LA::resizeCache132E40(D& d)noexcept {
    if(nativeClosed_||thread_!=std::this_thread::get_id()||!resolver_)return fail(d,E::phase,0x132E40);return resolver_->resizeTrackCache132E40(*this,d);
}
bool OriginalRaTrackSourceFile50LA::open131230(const rr::platform::OriginalWorldSourceConfig50LA& config,
    std::string_view name,std::int64_t& source00,D& d)noexcept {
    d={};if(!joined_||nativeClosed_||thread_!=std::this_thread::get_id()||source00!=-1||handle_!=-1||(occurrence_&&occurrence_!=&config)||
       !resolver_->freshFileCapacity_||!resolver_->freshFileCapacity_->track.cacheReturned)return fail(d,E::phase,0x131230);
    // Native name storage is admitted before the actual search; occurrence
    // custody is retained even on absent lookup or failure before numeric open.
    try {const std::string requested(name);occurrence_=&config;std::int32_t numeric=-1;
        const bool okay=resolver_->openSourceBankMember131230(this,&config,requested,numeric,d);
        handle_=numeric;
        if(!okay&&d.status==E::unresolved&&d.sourcePC==0x1315B0){source00=-1;d={};d.sourceEffectsCommitted=true;return true;}
        if(okay)source00=numeric;
        return okay;
    }catch(...){return fail(d,E::allocation,0x131230);}
}
bool OriginalRaTrackSourceFile50LA::current(const rr::platform::OriginalWorldSourceConfig50LA& config,std::int64_t source00)const noexcept {
    return !nativeClosed_&&thread_==std::this_thread::get_id()&&resolver_&&occurrence_==&config&&handle_>=0&&source00==handle_&&
        handle_<=std::numeric_limits<std::int32_t>::max()&&resolver_->sourceBankMemberCurrent(this,&config,static_cast<std::int32_t>(handle_));
}
bool OriginalRaTrackSourceFile50LA::read12FB30(const rr::platform::OriginalWorldSourceConfig50LA& config,std::int64_t handle,
    void* output,std::size_t bytes,std::size_t& done,D& d)noexcept {
    done=0;if(!current(config,handle))return fail(d,E::phase,0x12FB30);
    return resolver_->readSourceBankMember12FB30(this,&config,static_cast<std::int32_t>(handle_),output,bytes,done,d);
}
bool OriginalRaTrackSourceFile50LA::read12EE90(const rr::platform::OriginalWorldSourceConfig50LA& config,
    std::int64_t handle,Line12EE90& line,D& d)noexcept {
    line.present=false;line.position=0;
    if(!current(config,handle))return fail(d,E::phase,0x12EE90);
    // current proves SAME root, exact Config occurrence/numeric member, live
    // mount generation, open flag, thread and cursor<=length. Never remount,
    // borrow another Config's handle or broaden SCB/TOB/AUB open admission.
    const auto& member=resolver_->freshFileCapacity_->members150[static_cast<std::int32_t>(handle_)];
    const auto remaining=std::size_t(member.length14-member.cursor10);
    const auto requested=remaining<line.bytes.size()-2?remaining:line.bytes.size()-2;
    // Backend may commit partial destination/cache/device state before bounding.
    // Conservatively forget the potentially overwritten prefix; no unknown byte
    // becomes known or becomes EOF just because the method returns false.
    for(std::size_t i=0;i<=requested;++i)line.known[i]=false;
    if(!resolver_->readSourceBankLine12EE90(this,&config,static_cast<std::int32_t>(handle_),
        line.bytes,line.present,line.position,d))return false;
    // Genuine backend guarantees complete requested bytes before true, followed
    // by original requested-count terminator/CRLF normalization. At EOF only
    // byte0 is produced. The untouched tail retains its prior knownness.
    for(std::size_t i=0;i<=requested;++i)line.known[i]=true;
    return true;
}
bool OriginalRaTrackSourceFile50LA::tell12F520(const rr::platform::OriginalWorldSourceConfig50LA& config,
    std::int64_t handle,std::int64_t& sourceV0,D& d)noexcept {
    if(!current(config,handle))return fail(d,E::phase,0x12F520);
    std::size_t actual=0;
    if(!resolver_->tellSourceBankMember12F520(this,&config,static_cast<std::int32_t>(handle_),actual,d))return false;
    // Original12F630 LW sign-extends member10 into64-bit v0. Do not silently
    // zero-extend a high-bit cursor before the Config row's157654 SD.
    const auto low=static_cast<std::uint32_t>(actual);
    sourceV0=(low&0x80000000u)?std::int64_t(low)-0x100000000ll:std::int64_t(low);
    return true;
}
bool OriginalRaTrackSourceFile50LA::seek12F640(const rr::platform::OriginalWorldSourceConfig50LA& config,std::int64_t handle,std::size_t position,D& d)noexcept {
    if(!current(config,handle))return fail(d,E::phase,0x12F640);return resolver_->seekSourceBankMember12F640(this,&config,static_cast<std::int32_t>(handle_),position,d);
}
bool OriginalRaTrackSourceFile50LA::close131040(const rr::platform::OriginalWorldSourceConfig50LA& config,std::int64_t handle,bool flush,D& d)noexcept {
    if(!current(config,handle))return fail(d,E::phase,0x131040);
    if(!resolver_->closeSourceBankMember131040(this,&config,static_cast<std::int32_t>(handle_),flush,d))return false;
    handle_=-1;occurrence_=nullptr;return true;
}
bool OriginalRaTrackSourceFile50LA::abortOccurrenceNative(const rr::platform::OriginalWorldSourceConfig50LA& config,D& d)noexcept {
    d={};if(thread_!=std::this_thread::get_id()||nativeClosed_||(occurrence_&&occurrence_!=&config))return fail(d,E::phase,0);
    if(!occurrence_)return true;
    if(handle_!=-1){
        if(!resolver_||!resolver_->freshFileCapacity_||!resolver_->scratch_)return fail(d,E::phase,0);
        auto& state=*resolver_->freshFileCapacity_;auto& track=state.track;
        if(track.root!=this)return fail(d,E::phase,0);
        if(!state.members150||!state.devices14){
            auto* rows=state.members150?state.members150:state.failedCourseMembers;
            const auto count=state.members150?state.memberCount154:state.failedCourseMemberCapacity;
            if(!rows||handle_<0||handle_>=count)return fail(d,E::phase,0);
            auto& row=rows[static_cast<std::int32_t>(handle_)];
            if(row.courseConfigOwner!=&config||row.deviceID00!=track.device||!(row.flags18&4)||row.associatedObject1C||!row.originalMember)
                return fail(d,E::phase,0);
            // Native-only lost-shared-table disposal. No source131040 receipt,
            // no restoration of public150/14, no source Config00 rewrite.
            if(row.auxiliarySlot04>=0&&std::size_t(row.auxiliarySlot04)<resolver_->scratch_->slots.size()){
                auto& slot=resolver_->scratch_->slots[row.auxiliarySlot04];
                if(slot.status08==handle_)resolver_->scratch_->selected148=row.auxiliarySlot04;
            }
            row.flags18&=~std::uint32_t(4);row.courseConfigOwner=nullptr;
        }else if(!resolver_->closeSourceBankMember131040(this,&config,static_cast<std::int32_t>(handle_),false,d))return false;
    }
    handle_=-1;occurrence_=nullptr;d={};return true;
}
bool OriginalRaTrackSourceFile50LA::closeNative(D& d)noexcept {
    d={};if(nativeClosed_)return true;if(thread_!=std::this_thread::get_id()||occurrence_||nativeMissingLookupBorrowers_)return fail(d,E::phase,0);
    if(acquired_&&(!resolver_||!resolver_->closeTrackBankNative(*this,d)))return false;
    nativeClosed_=true;archive_.reset();provider_.reset();resolver_.reset();return true;
}
