// Included in resolver translation unit after Fresh and shared numeric I/O.
bool OriginalParticleSourceConfig50LA::current()const noexcept {
    return !nativeClosed_&&!sourceClosed_&&borrowed_&&thread_==std::this_thread::get_id()&&resolver_&&
        resolver_->freshFileCapacity_&&resolver_->freshFileCapacity_->particleConfigBorrower==this&&
        resolver_->freshFileCapacity_->provider==provider_&&source_->file00==handle_&&handle_>=0&&
        resolver_->sourceBankMemberCurrent(selectedRoot_,this,handle_);
}
bool OriginalParticleSourceConfig50LA::readLine(Line& line,D& d)noexcept {
    line.present=false;line.position=0;if(!current())return fail(d,E::phase,0x12EE90);
    const auto& row=resolver_->freshFileCapacity_->members150[handle_];
    const auto requested=std::min<std::size_t>(row.length14-row.cursor10,1022);
    for(std::size_t i=0;i<=requested;++i)line.known[i]=false;
    if(!resolver_->readSourceBankLine12EE90(selectedRoot_,this,handle_,line.bytes,line.present,line.position,d))return false;
    for(std::size_t i=0;i<=requested;++i)line.known[i]=true;
    return true;
}
bool OriginalParticleSourceConfig50LA::tell(std::int64_t& out,D& d)noexcept {
    if(!current())return fail(d,E::phase,0x12F520);
    std::size_t at=0;if(!resolver_->tellSourceBankMember12F520(selectedRoot_,this,handle_,at,d))return false;
    if(at>0xffffffffULL)return fail(d,E::phase,0x12F630);
    out=at<=0x7fffffffULL?std::int64_t(at):std::int64_t(at)-0x100000000LL;return true;
}
bool OriginalParticleSourceConfig50LA::begin1617A0(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider,const std::string& filename,D& d)noexcept {
    d={};if(attempted_||nativeClosed_||thread_!=std::this_thread::get_id()||!resolver||!provider||
        !provider->sealed()||!resolver->freshFileCapacity_||!resolver->scratch_)return fail(d,E::phase,0x161824);
    auto& s=*resolver->freshFileCapacity_;
    //Only the private coldWorld friend can call after proving its actual1617A0
    //filename and source phase. These checks retain the SAME live manager,
    //provider and Track registration; no mount/name bytes certify the caller.
    if(s.provider!=provider||!s.track.root||!s.track.joined||!s.track.cacheReturned||
       s.particleConfigBorrower||s.sourceFileMode170!=0||filename.empty()||filename.find('\0')!=std::string::npos)
        return fail(d,E::phase,0x161824);
    try {filename_=filename;}catch(...){return fail(d,E::allocation,0x161824);}
    resolver_=std::move(resolver);provider_=std::move(provider);attempted_=true;
    s.particleConfigBorrower=this;borrowed_=true;sourcePC_=0x1578D0;
    //131230 global rotating search: no caller-selected bank or forced ordinal.
    if(!s.members150||s.memberCount154<=0||s.cursor168< -1||s.cursor168>=s.memberCount154)
        return fail(d,E::phase,0x1315B0);
    ++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0;
    auto index=static_cast<std::int32_t>(s.cursor168);const auto start=index;bool found=false;
    do {const auto& row=s.members150[index];
        if(row.filename08&&courseSourceName308F00(filename_,row.filename08)){found=true;break;}
        if(++index==s.memberCount154)index=0;
    }while(index!=start);
    d.sourceEffectsCommitted=true;
    if(!found){sourceOpenReturned_=false;sourcePC_=0x157ACC;return true;}
    s.cursor168=index;
    //The established numeric reader currently has exact Course/Track custody.
    //A different globally selected bank is bounded AFTER search, not skipped
    //to force a later desired filename. No POD/nativeGarage admission is added.
    const auto device=s.members150[index].deviceID00;
    if(device==s.track.device&&s.track.joined)selectedRoot_=s.track.root;
    else if(device==s.courseDevice&&s.courseRootOwner&&
        s.courseStage==OriginalStandardTextureResolver50LA::FreshFileCapacity::CourseStage::joined)
        selectedRoot_=s.courseRootOwner;
    else {sourcePC_=0x131380;return fail(d,E::phase,sourcePC_);}
    if(!resolver_->openSourceBankMember131380(selectedRoot_,this,index,handle_,d))return false;
    source_->file00=handle_;sourcePC_=0x1578D8;
    source_->field42C=0;source_->opened0C=1;source_->mode10=0;
    std::array<std::uint8_t,7> magic;std::size_t done=0;sourcePC_=0x157914;
    if(!resolver_->readSourceBankMember12FB30(selectedRoot_,this,handle_,magic.data(),7,done,d))return false;
    sourcePC_=0x15791C;if(done!=7)return false; //Unwritten magic tail remains unknown.
    magic[6]=0;
    if(std::memcmp(magic.data(),"BINARY",6)==0){
        source_->binary08=1;sourcePC_=0x157950;return false;
        //Binary version/count/allocations not yet joined for this occurrence.
    }
    source_->binary08=0;sourcePC_=0x157AB0;
    if(!resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,0,d))return false;
    sourcePC_=0x157ABC;if(!parseText157500(d))return false;
    source_->field428=0;sourceOpenReturned_=true;sourcePC_=0x157ACC;return true;
}
bool OriginalParticleSourceConfig50LA::abortNative(D& d)noexcept {
    d={};if(thread_!=std::this_thread::get_id())return false;if(nativeClosed_)return true;
    if(borrowed_){
        if(!resolver_||!resolver_->freshFileCapacity_||
           resolver_->freshFileCapacity_->particleConfigBorrower!=this)return fail(d,E::phase,0);
        auto& s=*resolver_->freshFileCapacity_;
        //Native withdrawal is not source1573B0/1574D8 or a source handle store.
        //A partially opened handle belongs to this exact address even when
        //open did not return to publish Config.file00.
        if(handle_>=0){
            if(s.members150&&s.devices14){
                if(!resolver_->closeSourceBankMember131040(selectedRoot_,this,handle_,false,d))return false;
            }else {
                auto* rows=s.members150?s.members150:s.failedCourseMembers;
                const auto count=s.members150?s.memberCount154:s.failedCourseMemberCapacity;
                if(!rows||handle_>=count)return fail(d,E::phase,0);
                auto& row=rows[handle_];
                if(row.courseConfigOwner!=this||!(row.flags18&4)||row.associatedObject1C||!row.originalMember)
                    return fail(d,E::phase,0);
                if(row.auxiliarySlot04>=0&&std::size_t(row.auxiliarySlot04)<resolver_->scratch_->slots.size()){
                    auto& slot=resolver_->scratch_->slots[row.auxiliarySlot04];
                    if(slot.status08==handle_)resolver_->scratch_->selected148=row.auxiliarySlot04;
                }
                row.flags18&=~std::uint32_t(4);row.courseConfigOwner=nullptr;
            }
        }
        s.particleConfigBorrower=nullptr;borrowed_=false;
    }
    handle_=-1;selectedRoot_=nullptr;resolver_.reset();provider_.reset();nativeClosed_=true;return true;
}

bool OriginalParticleSourceConfig50LA::seek(std::size_t at,D& d)noexcept {
    if(!current())return fail(d,E::phase,0x12F640);
    return resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,at,d);
}

bool OriginalParticleSourceConfig50LA::missingOpenCurrent()const noexcept {
    if(!attempted_||nativeClosed_||sourceClosed_||!borrowed_||
       thread_!=std::this_thread::get_id()||!resolver_||!provider_||
       !resolver_->freshFileCapacity_||resolver_->freshFileCapacity_->particleConfigBorrower!=this||
       resolver_->freshFileCapacity_->provider!=provider_||!sourceOpenReturned_||*sourceOpenReturned_||
       sourcePC_!=0x157acc||handle_!=-1||selectedRoot_||textAttempted_||textReturned_)return false;
    const auto& s=*source_;
    return s.file00==-1&&!s.binary08&&!s.opened0C&&!s.mode10&&!s.count14&&!s.index18&&
        !s.text1C&&!s.child24&&!s.selected20&&!s.cacheCount438&&!s.names4CC&&!s.index4BC&&!s.block4C4;
}
bool OriginalParticleSourceConfig50LA::closeStack1573B0(D& d)noexcept {
    d={};
    const bool missing=missingOpenCurrent();
    if(sourceDestructorAttempted_||(!missing&&(!current()||!textReturned_||!sourceOpenReturned_||
       !*sourceOpenReturned_||source_->binary08||!glowGetterReturned_||
       integerGetterBounded_||frameGetterBounded_||textSelectBounded_)))return false;
    auto& s=*source_;
    //All native custody guards precede the actual handle close. This is the
    //opened TEXT occurrence or the genuine unopened missing-member branch.
    if(s.child24!=textChildTable_.get()||s.text1C!=textTable_.sourceStorage||
       s.count14!=textTable_.count14||s.index18!=textTable_.capacity18||
       s.cacheCount438||s.names4CC||s.index4BC||s.block4C4)return false;
    sourceDestructorAttempted_=true;
    if(!missing){
    sourcePC_=0x1573E8u;
    //Actual same numeric handle:12F290 archive flush13D9E0 is a literal no-op,
    //followed by131040 closing that occurrence, NOT its bank or sharedwindow.
    if(!resolver_->closeSourceBankMember131040(selectedRoot_,this,handle_,true,d))return false;
    sourcePC_=0x157404u;s.file00=-1;
    handle_=-1; //Native custody withdrawn only AFTER real close+sourceSD.
    } //Missing file00=-1 takes1573DC directly to15740C: no flush/close/store.
    sourcePC_=0x15740Cu;
    if(!retireTextChild157F90())return false;
    s.selected20=0;s.child24=nullptr; //157CE4/E8 even when childwasNULL.
    const auto cleanup=Text::cleanupText157414(textTable_);sourcePC_=cleanup.pc;
    syncTextFields();if(cleanup.stop!=Text::Stop::complete)return false;
    //This actual text occurrence never populated438 cache or binary arrays.
    //Do not invent releasecalls for them, nor reset source capacity18.
    s.cacheCount438=0;sourcePC_=0x157478u;
    //Actual161AAC a1=-1 is signed16 negative:1574C8 skips2E8B00. Config was
    //on source stack; native embedded Source remains for historical evidence.
    sourceClosed_=true;sourcePC_=0x1574F4u;return true;
}
