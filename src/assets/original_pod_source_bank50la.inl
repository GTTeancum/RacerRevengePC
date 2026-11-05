// Included inside rr::assets after the resolver's actual Fresh storage.
bool OriginalStandardTextureResolver50LA::registerPodBank12CEA0(OriginalPodSourceBank50LA& root,D& d)noexcept {
    if(freshFileCapacity_&&freshFileCapacity_->waterPoolFileBorrower)return fail(d,E::phase,0);
    d={};using C=FreshFileCapacity;
    if(!freshFileCapacity_||!scratch_||!root.archive_||!root.provider_||
       root.archive_->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::sourcePod01)||
       root.path_!="Pods\\pod01\\pod01.res")return fail(d,E::source,0x12CEA0);
    auto& s=*freshFileCapacity_;if(s.streamLibraryOwner||s.musicOwner)return fail(d,E::phase,0);auto& t=s.pod;
    if(t.attempted||t.root||s.provider.get()!=root.provider_.get()||s.registration!=C::Registration::complete||
       !s.devices14||!s.members150||s.deviceCount18<=0||s.memberCount154<=0||s.read174)
        return fail(d,E::phase,0x12CEA0);
    // Native preadmission only: the existing exact selected POD mount must
    // already own the whole authenticated directory. Never append, reactivate,
    // replace, or retrofit its absent encoded backing here.
    if(!root.provider_->sealed())return fail(d,E::phase,0x12CEA0);
    std::size_t nativeMount=static_cast<std::size_t>(-1);
    for(std::size_t n=0;n<root.provider_->mounts_.size();++n){const auto& bank=*root.provider_->mounts_[n];
        if(!OriginalStandardArchiveProvider50LA::sameName(bank.name,root.archive_->identity()->discPath))continue;
        if(nativeMount!=static_cast<std::size_t>(-1)||!bank.active||bank.digest!=root.archive_->identity()->sha256||
           bank.archiveBytes!=root.archive_->identity()->bytes||bank.rows.size()!=root.archive_->memberCount())return fail(d,E::source,0x12CEA0);
        for(std::size_t ordinal=0;ordinal<bank.rows.size();++ordinal){const auto actual=root.archive_->member(ordinal);const auto& row=bank.rows[ordinal];
            if(!actual.identity||row.name!=actual.identity->name||row.offset!=actual.identity->logicalOffset||row.size!=actual.size||
               std::memcmp(row.data,actual.data,actual.size))return fail(d,E::source,0x12CEA0);}
        nativeMount=n;
    }
    if(nativeMount==static_cast<std::size_t>(-1))return fail(d,E::source,0x12CEA0);
    // Native custody precedes every reached source mutation. No Course/Track root,
    // open member, loose device, or preexisting namespace lease is adopted.
    t.attempted=true;t.root=&root;t.archive=root.archive_;root.acquired_=true;
    t.mount=nativeMount;t.generation=root.provider_->mounts_[nativeMount]->generation;
    const std::string_view name=root.path_;
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
       !in.word(count)||!in.word(directoryBytes)||count!=96||directoryBytes!=3339||count!=t.archive->memberCount())return fail(d,E::source,0x12D51C);
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
    // Real1AE090 a2=1: source activation/refill precede12CEA0 return.
    // Failure retains all partial publication, never becomes caller false.
    if(!activateCourseDevice12E8B0(t.device,0,d))return false;
    if(!refillCourseWindow130340(t.device,0,d))return false;
    // Canonical reconciliation issues no extra numeric row.
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
bool OriginalStandardTextureResolver50LA::closePodBankNative(OriginalPodSourceBank50LA& root,D& d)noexcept {
    if(freshFileCapacity_&&freshFileCapacity_->waterPoolFileBorrower)return fail(d,E::phase,0);
    d={};if(!freshFileCapacity_||!scratch_)return fail(d,E::phase,0);auto& s=*freshFileCapacity_;if(s.streamLibraryOwner||s.musicOwner)return fail(d,E::phase,0);auto& t=s.pod;
    if(!t.root)return !root.acquired_;
    if(t.root!=&root)return fail(d,E::phase,0);
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
OriginalPodSourceBank50LA::~OriginalPodSourceBank50LA(){if(acquired_&&!nativeClosed_)std::terminate();}
bool OriginalPodSourceBank50LA::mount12CEA0(const original_racer_manager_entity::Owner& occurrence,
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider,
    std::shared_ptr<const OriginalUiArchive50KG> archive,const std::string& path,D& d)noexcept {
    d={};if(attempted_||nativeClosed_||thread_!=std::this_thread::get_id()||!resolver||!provider||!archive||!archive->loaded())return fail(d,E::phase,0x12CEA0);
    try{path_=path;}catch(...){return fail(d,E::allocation,0);}
    attempted_=true;occurrence_=&occurrence;resolver_=std::move(resolver);provider_=std::move(provider);archive_=std::move(archive);
    joined_=resolver_->registerPodBank12CEA0(*this,d);return joined_;
}
bool OriginalPodSourceBank50LA::enterArchiveOnly132A20(D& d)noexcept {
    d={};if(!joined_||nativeClosed_||mode1Reached_||thread_!=std::this_thread::get_id()||!resolver_||!resolver_->freshFileCapacity_||resolver_->freshFileCapacity_->pod.root!=this)return fail(d,E::phase,0x132A20);
    resolver_->freshFileCapacity_->sourceFileMode170=0;mode1Reached_=true;d.sourceEffectsCommitted=true;return true;
}
bool OriginalPodSourceBank50LA::closeNative(D& d)noexcept {
    d={};if(nativeClosed_)return true;if(thread_!=std::this_thread::get_id())return fail(d,E::phase,0);
    if(acquired_&&(!resolver_||!resolver_->closePodBankNative(*this,d)))return false;
    nativeClosed_=true;occurrence_=nullptr;archive_.reset();provider_.reset();resolver_.reset();return true;
}
