//Inside rr::assets, after the SAME Fresh/Scratch and encoded archive services.
bool OriginalStandardTextureResolver50LA::waterPoolTrackRelationNative(
    const OriginalRaTrackSourceFile50LA* root,const OriginalStandardArchiveProvider50LA* provider,
    const void*& identity,std::size_t& mount,std::uint64_t& generation)const noexcept {
    if(!root||!provider||!freshFileCapacity_||!scratch_||root->resolver_.get()!=this||
       root->provider_.get()!=provider||root->nativeClosed_||!root->joined_||
       root->thread_!=std::this_thread::get_id())return false;
    const auto& s=*freshFileCapacity_;const auto& t=s.track;
    if(s.provider.get()!=provider||s.registration!=FreshFileCapacity::Registration::complete||
       t.root!=root||!t.joined||!t.registered||!t.cacheReturned||t.archive!=root->archive_||
       t.mount>=provider->mounts_.size()||!t.generation)return false;
    const auto& bank=*provider->mounts_[t.mount];
    if(!bank.active||bank.generation!=t.generation)return false;
    identity=&bank;mount=t.mount;generation=t.generation;return true;
    //No sealed/revision/ctype/registry-provenance requirement for cleanup.
}
OriginalWaterPoolSourceFile50LA::~OriginalWaterPoolSourceFile50LA()noexcept {
    if(pinOwned_||nativeHandle_!=-1)std::terminate(); //Explicit aggregate withdrawal owns ordering.
}
bool OriginalWaterPoolSourceFile50LA::compare308F00(Comparison& c,bool& equal)noexcept {
    using G=OriginalConfigProcessGlobals50LA;const G::WaterSprintfAccess key;
    const char* left=c.fast?c.filenameIdentity:name_.c_str();
    const char* right=c.fast?name_.c_str():c.filenameIdentity;
    for(std::size_t i=0;;++i){
        c.left=static_cast<unsigned char>(left[i]);c.right=static_cast<unsigned char>(right[i]);
        c.foldedMask=0;c.folded={};
        const auto fold=[&](unsigned lane,unsigned char byte,std::uint32_t at)noexcept {
            c.pc=pc_=at;
            if(!G::waterFold308F00(key,byte,c.folded[lane],pc_)){c.pc=pc_;return false;}
            c.foldedMask|=std::uint8_t(1u<<lane);++c.foldReads;return true;
        };
        if(c.left){
            if(!fold(0,c.left,0x308f34)||!fold(1,c.right,0x308f3c))return false;
            if(c.folded[0]==c.folded[1]){++c.equalPrefix;continue;}
        }
        if(!fold(2,c.left,0x308f90)||!fold(3,c.right,0x308f94))return false;
        equal=c.equal=c.folded[2]==c.folded[3];c.returned=true;return true;
    }
}
bool OriginalWaterPoolSourceFile50LA::begin132820(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider,
    std::shared_ptr<OriginalRaTrackSourceFile50LA> track,const void* poolIdentity,
    const std::string& name,std::shared_ptr<OriginalWaterPoolSourceFile50LA>& pending,D& d)noexcept try {
    d={};if(pending||!resolver||!provider||!track||!poolIdentity||name.empty()||
       name.find('\0')!=std::string::npos||!provider->sealed()||!resolver->freshFileCapacity_||!resolver->scratch_)
        return fail(d,E::phase,0x132820);
    auto& s=*resolver->freshFileCapacity_;
    const void* mountIdentity=nullptr;std::size_t mount=0;std::uint64_t generation=0;
    if(s.waterPoolFileBorrower||s.musicOwner||s.streamLibraryOwner||s.loadingRaOwner||
       !s.members150||s.memberCount154<=0||!s.devices14||s.deviceCount18<=1||
       s.failedCourseMembers||s.failedCourseDevices||
       s.cursor160< -1||s.cursor160>=s.memberCount154||s.cursor168< -1||s.cursor168>=s.memberCount154||
       !resolver->waterPoolTrackRelationNative(track.get(),provider.get(),mountIdentity,mount,generation))
        return fail(d,E::phase,0x132820);
    //All allocations precede the first shared search effect. Publish any
    //allocated partial bookkeeping for aggregate cancellation on refusal.
    pending=std::shared_ptr<OriginalWaterPoolSourceFile50LA>(new OriginalWaterPoolSourceFile50LA);
    auto& p=*pending;p.poolIdentity_=poolIdentity;p.resolver_=std::move(resolver);
    p.provider_=std::move(provider);p.track_=std::move(track);p.pc_=0x132820;
    p.freshIdentity_=&s;p.membersIdentity_=s.members150;p.devicesIdentity_=s.devices14;
    p.memberCount_=s.memberCount154;p.deviceCount_=s.deviceCount18;
    p.serviceMountIdentity_=mountIdentity;p.serviceTrackMount_=mount;p.serviceTrackGeneration_=generation;
    p.before160_=p.after160_=s.cursor160;p.before168_=p.after168_=s.cursor168;
    p.name_=name;p.comparisons_.reserve(std::size_t(p.memberCount_)+1u);
    p.read_.before.devices.resize(std::size_t(p.deviceCount_));
    p.read_.after.devices.resize(std::size_t(p.deviceCount_));
    //The gate is whole-table, including misses and unsupported foreign wins.
    //It is not Water Config's pin and does not identify a winning bank yet.
    s.waterPoolFileBorrower=&p;p.pinOwned_=true;p.searchReached_=true;
    const auto compare=[&](std::int32_t index,bool fast,bool& equal)noexcept {
        p.comparisons_.emplace_back();auto& c=p.comparisons_.back();
        c.row=index;c.fast=fast;c.filenameIdentity=s.members150[index].filename08;
        equal=false;
        if(!c.filenameIdentity){c.returned=true;return true;}
        return p.compare308F00(c,equal);
    };
    const auto stop=[&](E why,std::uint32_t pc)noexcept {
        p.after160_=s.cursor160;p.after168_=s.cursor168;p.pc_=pc;
        d.sourceEffectsCommitted=p.rotatingAdvanced_||p.cachedStored_||p.openAttempted_;
        return fail(d,why,pc);
    };
    bool equal=false;
    if(s.cursor160!=-1){
        if(!compare(static_cast<std::int32_t>(s.cursor160),true,equal))return stop(E::unsupported,p.pc_);
        p.fastMatch_=equal;
    }
    p.found_=p.fastMatch_;
    if(!p.found_){
        p.pc_=0x1328b0;++s.cursor168;p.rotatingAdvanced_=true;
        if(s.cursor168==s.memberCount154){p.pc_=0x1328c4;s.cursor168=0;p.rotatingWrapped_=true;}
        auto index=static_cast<std::int32_t>(s.cursor168);const auto start=index;
        do {
            if(!compare(index,false,equal))return stop(E::unsupported,p.pc_);
            if(equal){p.pc_=0x132904;s.cursor168=index;p.rotatingMatchStored_=true;p.found_=true;break;}
            if(++index==s.memberCount154)index=0;
        }while(index!=start);
        p.pc_=0x132938;s.cursor160=p.found_?s.cursor168:-1;p.cachedStored_=true;
    }
    p.after160_=s.cursor160;p.after168_=s.cursor168;p.searchReturned_=true;
    if(!p.found_){
        p.pc_=0x13294c;p.missMode170_=s.sourceFileMode170;
        if(*p.missMode170_)return stop(E::unsupported,0x12dcb0);
        p.sourceReturnV0_=0;p.pc_=0x132a0c;d.sourcePC=p.pc_;d.sourceEffectsCommitted=true;return true;
    }
    //Immediate132988 tail proves the current selected row before its next
    //source read. This check is deliberately NOT a historical cleanup gate.
    p.pc_=0x132988;
    if(s.cursor160!=p.after160_||s.cursor160<0||s.cursor160>=p.memberCount_||
       p.comparisons_.empty()||p.comparisons_.back().row!=s.cursor160||
       s.members150[s.cursor160].filename08!=p.comparisons_.back().filenameIdentity)
        return stop(E::phase,p.pc_);
    p.row_=static_cast<std::int32_t>(s.cursor160);const auto& row=s.members150[p.row_];
    p.device_=row.deviceID00;p.memberIdentity_=row.originalMember;
    p.filenameIdentity_=row.filename08;p.selectedLength_=row.length14;
    p.pc_=0x1329a0;p.associationObserved_=true;p.opaqueAssociation_=row.associatedObject1C;
    //Selected-root metadata is assigned only AFTER the genuine global search.
    if(row.deviceID00==s.track.device){
        p.selectedRoot_=s.track.root;p.selectedMountIdentity_=mountIdentity;
        p.selectedMount_=s.track.mount;p.selectedGeneration_=s.track.generation;
    }
    if(p.opaqueAssociation_){
        p.warmObserved_=true;p.warmT1ZeroBranch_=true;p.warmDelayV0_=-1;
        //t1=0 and1329B0's v0=-1 are reached, but1329C0's output store is NOT.
        //No typed custodian, source retain, alias, association clear or fallback.
        return stop(E::unsupported,0x1329c0);
    }
    p.pc_=0x131380;
    if(!p.selectedRoot_||!p.memberIdentity_||!row.canonicalData)
        return stop(E::unsupported,p.pc_); //Honest other-bank/unsupported winner.
    p.openAttempted_=true;
    p.openReturned_=p.resolver_->openSourceBankMember131380(p.selectedRoot_,&p,p.row_,p.nativeHandle_,d);
    p.openDiagnostic_=d;
    if(!p.openReturned_)return stop(d.status,d.sourcePC?d.sourcePC:0x131380);
    //Open's return register is ignored by source. It cannot turn native refusal
    //into source0. Successful open reloads CURRENT160 for its output store.
    if(s.cursor160!=p.row_)return stop(E::phase,0x1329dc);
    p.pc_=0x1329e0;p.sourceHandle1E0_=s.cursor160;p.outputHandleStored_=true;
    p.pc_=0x1329e4;p.sourceAssociated1E8_=nullptr;p.outputAssociationStored_=true;
    p.pc_=0x1329e8;p.sourceReturnV0_=1;
    p.pc_=0x132a0c;d.sourcePC=p.pc_;d.sourceEffectsCommitted=true;return true;
}catch(...){
    if(pending)pending->pc_=0x132820;
    return fail(d,E::allocation,0x132820);
}
bool OriginalWaterPoolSourceFile50LA::observeServiceNative(ServiceObservation& out)const noexcept {
    if(!resolver_||!resolver_->freshFileCapacity_||!resolver_->scratch_||nativeHandle_<0)return false;
    const auto& s=*resolver_->freshFileCapacity_;const auto& scratch=*resolver_->scratch_;
    if(!s.members150||nativeHandle_>=s.memberCount154||!s.devices14||
       out.devices.size()!=std::size_t(s.deviceCount18))return false;
    const auto& row=s.members150[nativeHandle_];
    out.memberCursor10=row.cursor10;out.memberFlags18=row.flags18;out.memberAuxiliary04=row.auxiliarySlot04;
    out.selected148=scratch.selected148;out.active=s.active;out.current74=s.current74;
    out.window7C=s.window7C;out.window80=s.window80;out.encodedWindowDevice=s.encodedWindowDevice;
    out.read174=s.read174;out.windowBytes=s.courseWindowBytes;out.lengthTableIdentity=s.lengthTable84;
    for(std::size_t i=0;i<scratch.slots.size();++i){out.scratchStatus[i]=scratch.slots[i].status08;out.scratchOffset[i]=scratch.slots[i].offset10;}
    for(std::size_t i=0;i<out.devices.size();++i){const auto& device=s.devices14[i];auto& observed=out.devices[i];
        observed.cursor08=device.cursor08;observed.flags0C=device.flags0C;observed.handleIdentity=device.handle10;
        observed.encodedCursor=device.handle10?device.handle10->cursor:0;
    }
    return true;
}
std::optional<std::uint32_t> OriginalWaterPoolSourceFile50LA::versionWordNative()const noexcept {
    if(!read_.attempted||!read_.nativeReturned||!read_.resultAdmitted)return {};
    for(const auto known:read_.known)if(!known)return {};
    std::uint32_t value=0;for(unsigned i=0;i<4;++i)value|=std::uint32_t(read_.bytes[i])<<(8u*i);
    return value;
}
bool OriginalWaterPoolSourceFile50LA::readVersion12FB30(D& d)noexcept {
    d={};if(read_.attempted||nativeClosed_||sourceCloseAttempted_||!openReturned_||
       sourceReturnV0_!=1||!preflightNative(poolIdentity_)||nativeHandle_==-1)
        return fail(d,E::phase,0x12fb30);
    if(!observeServiceNative(read_.before))return fail(d,E::phase,0x12fb30);
    const auto& row=resolver_->freshFileCapacity_->members150[nativeHandle_];
    if(row.length14-row.cursor10>std::uint32_t(std::numeric_limits<std::int32_t>::max()))
        return fail(d,E::unsupported,0x12fb30); //Native preadmission for the source signed clamp.
    //Native admitted extent, not a claim that the source clamp preceded its
    //guards/activation. The helper performs those reached effects in order.
    read_.clampedAmount=std::min<std::size_t>(4u,row.length14-row.cursor10);
    read_.attempted=true;pc_=0x12fb30;
    read_.nativeReturned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,nativeHandle_,
        read_.bytes.data(),4u,read_.copied,d);
    read_.diagnostic=d;
    for(std::size_t i=0;i<read_.known.size();++i)read_.known[i]=i<read_.copied;
    read_.afterObserved=observeServiceNative(read_.after);
    if(!read_.nativeReturned){pc_=d.sourcePC?d.sourcePC:0x12fb30;return false;}
    //This synchronous encoded service returns true only for the entire clamped
    //amount. Positive1..3 bytes still select the ORIGINAL caller count1.
    if(!read_.afterObserved||read_.copied!=read_.clampedAmount||
       read_.after.memberCursor10!=read_.before.memberCursor10+read_.copied)
        return fail(d,E::phase,pc_=0x12fb30);
    read_.sourceItems=read_.copied?1:0;read_.resultAdmitted=true;pc_=0x2bb0e4;return true;
    //Caller ignores v0 but must refuse LW2BB0E4 unless all four lanes are known.
}
bool OriginalWaterPoolSourceFile50LA::readChildCount12FB30(const void* resourceIdentity,D& d)noexcept {
    d={};const auto version=versionWordNative();
    if(!resourceIdentity||countRead_.entered||nativeClosed_||nativeCloseAttempted_||
       sourceCloseAttempted_||!version||*version!=2u||!read_.afterObserved||
       !openReturned_||sourceReturnV0_!=1||nativeHandle_==-1||
       !preflightNative(poolIdentity_))return fail(d,E::phase,0x2bb19c);
    auto& c=countRead_;c.entered=true;c.resourceIdentity=resourceIdentity;c.pc=0x2bb19c;
    //No service call or stream effect can precede these native allocations.
    //An allocation refusal still consumes this descendant's one-shot entry,
    //but leaves the immutable version.after as the live member-cursor tail.
    try {
        c.before.devices.resize(std::size_t(deviceCount_));
        c.after.devices.resize(std::size_t(deviceCount_));
    }catch(...){fail(d,E::allocation,c.pc);c.diagnostic=d;return false;}
    c.prepared=true;c.pc=0x12fb30;
    c.beforeObserved=observeServiceNative(c.before);
    if(!c.beforeObserved||c.before.memberCursor10!=read_.after.memberCursor10){
        fail(d,E::phase,c.pc);c.diagnostic=d;return false;
    }
    //The fresh observation contains current shared device/window state. Only
    //the exact owned member's cursor chains to the prior authenticated tail;
    //other legitimate readers need not leave the old shared window current.
    const auto& row=resolver_->freshFileCapacity_->members150[nativeHandle_];
    if(row.cursor10>row.length14||
       row.length14-row.cursor10>std::uint32_t(std::numeric_limits<std::int32_t>::max())){
        fail(d,E::unsupported,c.pc);c.diagnostic=d;return false;
    }
    c.clampedAmount=std::min<std::size_t>(4u,row.length14-row.cursor10);
    c.attempted=true;
    c.nativeReturned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,nativeHandle_,
        c.bytes.data(),4u,c.copied,d);
    c.diagnostic=d;c.afterObserved=observeServiceNative(c.after);
    //The actual helper overlays only its copied prefix. The constructor-zero
    //suffix remains fully known, including after EOF or a partial refusal.
    //Do not derive a member cursor from copied bytes: failure may advance the
    //encoded device/window while leaving the member cursor unchanged.
    if(!c.nativeReturned){c.pc=d.sourcePC?d.sourcePC:0x12fb30;return false;}
    if(!c.afterObserved||c.copied!=c.clampedAmount||
       c.after.memberCursor10!=c.before.memberCursor10+c.copied)
        return fail(d,E::phase,c.pc=0x12fb30);
    c.sourceItems=c.copied?1:0;c.resultAdmitted=true;c.pc=0x2bb1a4;return true;
    //Even completed1..3 byte reads return source items1; EOF returns0. The
    //aggregate, not this file custodian, may now issue LW and signed BLEZ.
}
const std::array<OriginalWaterPoolSourceFile50LA::MaterialReadSpec,18>&
OriginalWaterPoolSourceFile50LA::materialReadScheduleNative()noexcept {
    //Exact occurrences, including the unusual +9C BEFORE +98 order. The
    //first service load belongs to the immutable aggregate predecessor.
    static constexpr std::array<MaterialReadSpec,18> schedule{{
        {0x2bf718,0x2bf71c,0x2bf724,0x2bf728,0x2bf72c,0x2bf730,0x2bf734,0x2bf738,0x30,64,1,0},
        {0x2bf738,0x2bf73c,0x2bf740,0x2bf744,0x2bf748,0x2bf74c,0x2bf750,0x2bf754,0x70,4,0,0},
        {0x2bf754,0x2bf758,0x2bf75c,0x2bf760,0x2bf764,0x2bf768,0x2bf76c,0x2bf770,0x74,4,4,0x00ff00ff},
        {0x2bf770,0x2bf774,0x2bf778,0x2bf77c,0x2bf780,0x2bf784,0x2bf788,0x2bf78c,0x78,4,0,0},
        {0x2bf78c,0x2bf790,0x2bf794,0x2bf798,0x2bf79c,0x2bf7a0,0x2bf7a4,0x2bf7a8,0x7c,4,0,0},
        {0x2bf7a8,0x2bf7ac,0x2bf7b0,0x2bf7b4,0x2bf7b8,0x2bf7bc,0x2bf7c0,0x2bf7c4,0x80,4,4,0},
        {0x2bf7c4,0x2bf7c8,0x2bf7cc,0x2bf7d0,0x2bf7d4,0x2bf7d8,0x2bf7dc,0x2bf7e0,0x84,4,4,0},
        {0x2bf7e0,0x2bf7e4,0x2bf7e8,0x2bf7ec,0x2bf7f0,0x2bf7f4,0x2bf7f8,0x2bf7fc,0x88,4,4,1},
        {0x2bf7fc,0x2bf800,0x2bf804,0x2bf808,0x2bf80c,0x2bf810,0x2bf814,0x2bf818,0x8c,4,4,0},
        {0x2bf818,0x2bf81c,0x2bf820,0x2bf824,0x2bf828,0x2bf82c,0x2bf830,0x2bf834,0x90,4,4,0},
        {0x2bf834,0x2bf838,0x2bf83c,0x2bf840,0x2bf844,0x2bf848,0x2bf84c,0x2bf850,0x94,4,4,0},
        {0x2bf850,0x2bf854,0x2bf858,0x2bf85c,0x2bf860,0x2bf864,0x2bf868,0x2bf86c,0x9c,4,4,1},
        {0x2bf86c,0x2bf870,0x2bf874,0x2bf878,0x2bf87c,0x2bf880,0x2bf884,0x2bf888,0x98,4,4,0},
        {0x2bf888,0x2bf88c,0x2bf890,0x2bf894,0x2bf898,0x2bf89c,0x2bf8a0,0x2bf8a4,0xa0,4,4,0},
        {0x2bf8a4,0x2bf8a8,0x2bf8ac,0x2bf8b0,0x2bf8b4,0x2bf8b8,0x2bf8bc,0x2bf8c0,0xa4,4,4,0x3f800000},
        {0x2bf8c0,0x2bf8c4,0x2bf8c8,0x2bf8cc,0x2bf8d0,0x2bf8d4,0x2bf8d8,0x2bf8dc,0xa8,4,4,0},
        {0x2bf8dc,0x2bf8e0,0x2bf8e4,0x2bf8e8,0x2bf8ec,0x2bf8f0,0x2bf8f4,0x2bf8f8,0xac,2,2,0},
        {0x2bf8f8,0x2bf8fc,0x2bf900,0x2bf904,0x2bf908,0x2bf90c,0x2bf910,0x2bf914,0xb0,4,4,0}
    }};return schedule;
}
bool OriginalWaterPoolSourceFile50LA::beginMaterialReaderNative(
    const void* material,const void* array,const void* data,std::uint64_t materialEpoch,
    std::uint64_t arrayEpoch,std::uint64_t dataEpoch,D& d)noexcept {
    d={};auto& m=materialReader_;
    if(m.entered||!material||!array||!data||materialEpoch!=1u||arrayEpoch!=1u||dataEpoch!=1u||
       countRead_.resourceIdentity!=data||!countRead_.resultAdmitted||!countRead_.afterObserved||
       nativeClosed_||nativeCloseAttempted_||sourceCloseAttempted_||nativeHandle_<0||
       !preflightNative(poolIdentity_))return fail(d,E::phase,0x2bf730);
    m.entered=true;m.materialIdentity=material;m.arrayIdentity=array;m.dataIdentity=data;
    m.materialEpoch=materialEpoch;m.arrayEpoch=arrayEpoch;m.dataEpoch=dataEpoch;
    //Fixed18 receipts; all36 device vectors are allocated BEFORE any source
    //call. Even an after-observation of a failed helper allocates nothing.
    try {for(auto& r:m.reads){r.before.devices.resize(std::size_t(deviceCount_));
        r.after.devices.resize(std::size_t(deviceCount_));}}
    catch(...){fail(d,E::allocation,0x2bf730);m.preparationDiagnostic=d;return false;}
    m.prepared=true;return true;
}
bool OriginalWaterPoolSourceFile50LA::readMaterial12FB30(std::size_t ordinal,D& d)noexcept {
    d={};auto& m=materialReader_;const auto& schedule=materialReadScheduleNative();
    if(!m.prepared||ordinal>=m.reads.size()||m.reads[ordinal].entered||
       (ordinal&&!m.reads[ordinal-1u].resultAdmitted))return fail(d,E::phase,0x12fb30);
    auto& r=m.reads[ordinal];r.entered=true;r.source=schedule[ordinal];r.pc=r.source.callPC;
    for(std::size_t i=0;i<r.source.knownPrefix;++i){
        r.preimageMask[i]=r.knownMask[i]=255u;
        r.preimageBytes[i]=r.bytes[i]=std::uint8_t(r.source.preimageWord>>(8u*i));
    }
    const auto reject=[&](E why,std::uint32_t pc)noexcept {
        fail(d,why,pc);r.admissionDiagnostic=d;r.pc=pc;return false;
    };
    if(nativeClosed_||nativeCloseAttempted_||sourceCloseAttempted_||nativeHandle_<0||
       !preflightNative(poolIdentity_))return reject(E::phase,r.source.callPC);
    r.beforeObserved=observeServiceNative(r.before);
    const auto& prior=ordinal?m.reads[ordinal-1u].after:countRead_.after;
    if(!r.beforeObserved||r.before.memberCursor10!=prior.memberCursor10)
        return reject(E::phase,r.source.callPC);
    if(r.before.memberCursor10>selectedLength_||
       selectedLength_-r.before.memberCursor10>std::uint32_t(std::numeric_limits<std::int32_t>::max()))
        return reject(E::unsupported,r.source.callPC);
    r.clampedAmount=std::min<std::size_t>(r.source.size,selectedLength_-r.before.memberCursor10);
    //The real call/delay occur only after native observation/clamp admission.
    //The first delay saves a1 in s2; later delays pass that exact same handle.
    r.pc=r.source.callPC;r.callIssued=true;r.pc=r.source.delayPC;r.delayIssued=true;
    if(!ordinal)m.savedHandleS2=*sourceHandle1E0_;
    r.argumentHandle=m.savedHandleS2;r.helperEntry=r.pc=0x12fb30u;r.attempted=true;
    r.nativeReturned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,nativeHandle_,
        r.bytes.data(),r.source.size,r.copied,d);
    r.helperDiagnostic=d;r.afterObserved=observeServiceNative(r.after);
    for(std::size_t i=0;i<std::min(r.copied,std::size_t(r.source.size));++i)r.knownMask[i]=255u;
    //Preserve helper Boolean/diagnostic even if this later admission fails.
    //No caller return, reset or replay can follow an unadmitted outcome.
    if(!r.nativeReturned){r.pc=d.sourcePC?d.sourcePC:0x12fb30u;return false;}
    if(!r.afterObserved||r.copied!=r.clampedAmount||
       r.after.memberCursor10!=r.before.memberCursor10+r.copied)return reject(E::phase,0x12fb30);
    r.sourceItems=r.copied?1:0;r.resultAdmitted=true;r.pc=r.source.returnPC;return true;
}
bool OriginalWaterPoolSourceFile50LA::materialReaderHistoryNative()const noexcept {
    const auto& m=materialReader_;const auto& schedule=materialReadScheduleNative();
    if(m.entered){
        if(!m.materialIdentity||!m.arrayIdentity||!m.dataIdentity||m.materialEpoch!=1u||m.arrayEpoch!=1u||
           m.dataEpoch!=1u||m.dataIdentity!=countRead_.resourceIdentity||!countRead_.resultAdmitted||
           !countRead_.afterObserved||sourceCloseAttempted_||sourceHandle1E0_!=std::int64_t(row_))return false;
    }else if(m.prepared||m.materialIdentity||m.arrayIdentity||m.dataIdentity||m.materialEpoch||
             m.arrayEpoch||m.dataEpoch||m.savedHandleS2)return false;
    bool previousAdmitted=true,anyAttempted=false;const ServiceObservation* tail=&countRead_.after;
    for(std::size_t index=0;index<m.reads.size();++index){const auto& r=m.reads[index];const auto& s=schedule[index];
        if(m.prepared){if(r.before.devices.size()!=std::size_t(deviceCount_)||
            r.after.devices.size()!=std::size_t(deviceCount_))return false;}
        else if(r.entered||r.before.devices.size()>std::size_t(deviceCount_)||
                r.after.devices.size()>std::size_t(deviceCount_))return false;
        if(!m.entered&&(!r.before.devices.empty()||!r.after.devices.empty()))return false;
        const auto& v=r.source;
        if(r.entered){
            if(!m.prepared||!previousAdmitted||!r.pc||v.serviceLuiPC!=s.serviceLuiPC||v.serviceLoadPC!=s.serviceLoadPC||
               v.destinationPC!=s.destinationPC||v.sizePC!=s.sizePC||v.countPC!=s.countPC||v.callPC!=s.callPC||
               v.delayPC!=s.delayPC||v.returnPC!=s.returnPC||v.offset!=s.offset||v.size!=s.size||
               v.knownPrefix!=s.knownPrefix||v.preimageWord!=s.preimageWord||r.copied>s.size||
               r.clampedAmount>s.size||r.copied>r.clampedAmount)return false;
        }else if(v.serviceLuiPC||v.serviceLoadPC||v.destinationPC||v.sizePC||v.countPC||v.callPC||v.delayPC||
                 v.returnPC||v.offset||v.size||v.knownPrefix||v.preimageWord||r.pc||r.beforeObserved||r.attempted||
                 r.callIssued||r.delayIssued||r.nativeReturned||r.afterObserved||r.resultAdmitted||r.copied||
                 r.clampedAmount||r.argumentHandle||r.sourceItems||r.helperEntry)return false;
        for(std::size_t i=0;i<64u;++i){
            const bool known=r.entered&&i<s.knownPrefix;
            const auto byte=known?std::uint8_t(s.preimageWord>>(8u*i)):std::uint8_t(0);
            if(r.preimageMask[i]!=(known?255u:0u)||r.preimageBytes[i]!=byte||
               r.knownMask[i]!=((known||i<r.copied)?255u:0u)||(i>=r.copied&&r.bytes[i]!=byte))return false;
        }
        if(r.beforeObserved&&(!r.entered||!tail||r.before.memberCursor10!=tail->memberCursor10))return false;
        if(r.attempted){
            anyAttempted=true;
            if(!r.beforeObserved||!r.callIssued||!r.delayIssued||r.helperEntry!=0x12fb30u||
               r.argumentHandle!=m.savedHandleS2||r.argumentHandle!=sourceHandle1E0_||
               r.before.memberCursor10>selectedLength_||
               selectedLength_-r.before.memberCursor10>std::uint32_t(std::numeric_limits<std::int32_t>::max())||
               r.clampedAmount!=std::min<std::size_t>(s.size,selectedLength_-r.before.memberCursor10))return false;
            const bool admitted=r.nativeReturned&&r.afterObserved&&r.copied==r.clampedAmount&&
                r.after.memberCursor10==r.before.memberCursor10+r.copied;
            if(r.resultAdmitted!=admitted)return false;
            if(r.resultAdmitted){if(!r.nativeReturned||!r.afterObserved||r.copied!=r.clampedAmount||
                r.sourceItems!=(r.copied?1:0)||r.pc!=s.returnPC||
                r.after.memberCursor10!=r.before.memberCursor10+r.copied)return false;}
            else if(r.sourceItems||r.pc!=(r.nativeReturned?0x12fb30u:
                r.helperDiagnostic.sourcePC?r.helperDiagnostic.sourcePC:0x12fb30u))return false;
            if(!r.nativeReturned&&r.afterObserved&&r.after.memberCursor10!=r.before.memberCursor10)return false;
            tail=r.afterObserved?&r.after:nullptr; //Missing after MUST NOT reuse an older tail.
        }else if(r.callIssued||r.delayIssued||r.helperEntry||r.argumentHandle||r.nativeReturned||
                 r.afterObserved||r.resultAdmitted||r.sourceItems||r.copied||r.clampedAmount||
                 (r.entered&&r.pc!=s.callPC))return false;
        previousAdmitted=r.resultAdmitted;
    }
    return bool(m.savedHandleS2)==anyAttempted&&(!anyAttempted||m.savedHandleS2==sourceHandle1E0_);
}
bool OriginalWaterPoolSourceFile50LA::readTopologyCount12FB30(
    const void* data,std::uint64_t dataEpoch,TopologyEntryKind kind,D& d)noexcept {
    d={};auto& r=topologyRead_;
    if(r.entered||!data||dataEpoch!=1u||data!=countRead_.resourceIdentity||
       !countRead_.resultAdmitted||!countRead_.afterObserved||nativeClosed_||nativeCloseAttempted_||
       sourceCloseAttempted_||nativeHandle_<0||!sourceHandle1E0_||*sourceHandle1E0_<0||
       !preflightNative(poolIdentity_))return fail(d,E::phase,0x2bab40u);
    const auto childWord=std::uint32_t(countRead_.bytes[0])|(std::uint32_t(countRead_.bytes[1])<<8u)|
        (std::uint32_t(countRead_.bytes[2])<<16u)|(std::uint32_t(countRead_.bytes[3])<<24u);
    const ServiceObservation* prior=nullptr;
    if(kind==TopologyEntryKind::returnedFirstMaterial){
        if(childWord!=1u||!materialReader_.prepared||materialReader_.dataIdentity!=data||
           materialReader_.dataEpoch!=dataEpoch)return fail(d,E::phase,0x2bab40u);
        for(const auto& read:materialReader_.reads)
            if(!read.resultAdmitted||!read.afterObserved)return fail(d,E::phase,0x2bab40u);
        prior=&materialReader_.reads.back().after;
    }else if(kind==TopologyEntryKind::nonpositiveCount){
        if(materialReader_.entered||(childWord!=0u&&!(childWord&0x80000000u)))
            return fail(d,E::phase,0x2bab40u);
        prior=&countRead_.after;
    }else return fail(d,E::phase,0x2bab40u);
    r.entered=true;r.entryKind=kind;r.dataIdentity=data;r.dataEpoch=dataEpoch;r.fileIdentity=this;
    r.sourceHandle=sourceHandle1E0_;r.pc=0x2bab40u;r.preimagePC=0x2bb164u;r.itemCount=1u;
    r.source={0x2bab2c,0x2bab30,0x2bab34,0x2bab38,0x2bab3c,0x2bab40,0x2bab44,0x2bab48,0x1c,4,4,0};
    r.preimageMask.fill(255u);r.knownMask.fill(255u);
    //Bookkeeping zero is justified by the caller's genuine SW2BB164. Both
    //observation vectors must exist before the helper can affect any state.
    try {r.before.devices.resize(std::size_t(deviceCount_));r.after.devices.resize(std::size_t(deviceCount_));}
    catch(...){fail(d,E::allocation,r.pc);r.preparationDiagnostic=d;return false;}
    r.prepared=true;
    const auto reject=[&](E why,std::uint32_t pc)noexcept {
        fail(d,why,pc);r.admissionDiagnostic=d;r.pc=pc;return false;
    };
    r.beforeObserved=observeServiceNative(r.before);
    if(!r.beforeObserved||r.before.memberCursor10!=prior->memberCursor10)return reject(E::phase,r.source.callPC);
    if(r.before.memberCursor10>selectedLength_||
       selectedLength_-r.before.memberCursor10>std::uint32_t(std::numeric_limits<std::int32_t>::max()))
        return reject(E::unsupported,r.source.callPC);
    r.clampedAmount=std::min<std::size_t>(4u,selectedLength_-r.before.memberCursor10);
    r.pc=r.source.callPC;r.callIssued=true;r.pc=r.source.delayPC;r.delayIssued=true;
    r.argumentHandle=r.sourceHandle;r.savedHandleS2=r.sourceHandle;
    r.helperEntry=r.pc=0x12fb30u;r.attempted=true;
    r.nativeReturned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,nativeHandle_,
        r.bytes.data(),4u,r.copied,d);
    r.helperDiagnostic=d;r.afterObserved=observeServiceNative(r.after);
    //Keep actual copied prefix and actual after even on native refusal. Owner
    //synchronizes the prefix into Data before calling any current-byte check.
    if(!r.nativeReturned){r.pc=d.sourcePC?d.sourcePC:0x12fb30u;return false;}
    if(!r.afterObserved||r.copied!=r.clampedAmount||
       r.after.memberCursor10!=r.before.memberCursor10+r.copied)return reject(E::phase,0x12fb30u);
    r.sourceItems=r.copied?1:0;r.resultAdmitted=true;r.pc=r.source.returnPC;return true;
}
bool OriginalWaterPoolSourceFile50LA::topologyReadHistoryNative()const noexcept {
    const auto& r=topologyRead_;const auto& s=r.source;
    if(!r.entered){
        if(r.entryKind!=TopologyEntryKind::none||r.dataIdentity||r.fileIdentity||r.dataEpoch||r.prepared||
           r.preimagePC||r.itemCount||r.sourceHandle||r.argumentHandle||r.savedHandleS2||r.sourceItems||
           r.pc||r.helperEntry||r.beforeObserved||r.attempted||r.callIssued||r.delayIssued||r.nativeReturned||
           r.afterObserved||r.resultAdmitted||r.copied||r.clampedAmount||
           !r.before.devices.empty()||!r.after.devices.empty()||s.serviceLuiPC||s.serviceLoadPC||s.destinationPC||
           s.sizePC||s.countPC||s.callPC||s.delayPC||s.returnPC||s.offset||s.size||s.knownPrefix||s.preimageWord)
            return false;
        return r.preimageMask==std::array<std::uint8_t,4>{}&&r.knownMask==std::array<std::uint8_t,4>{}&&
            r.preimageBytes==std::array<std::uint8_t,4>{}&&r.bytes==std::array<std::uint8_t,4>{};
    }
    if(!r.dataIdentity||r.dataEpoch!=1u||r.fileIdentity!=this||r.dataIdentity!=countRead_.resourceIdentity||
       !countRead_.resultAdmitted||!countRead_.afterObserved||sourceCloseAttempted_||
       !r.sourceHandle||*r.sourceHandle<0||r.sourceHandle!=sourceHandle1E0_||r.sourceHandle!=std::int64_t(row_)||
       r.preimagePC!=0x2bb164u||r.itemCount!=1u||s.serviceLuiPC!=0x2bab2cu||s.serviceLoadPC!=0x2bab30u||
       s.destinationPC!=0x2bab34u||s.sizePC!=0x2bab38u||s.countPC!=0x2bab3cu||s.callPC!=0x2bab40u||
       s.delayPC!=0x2bab44u||s.returnPC!=0x2bab48u||s.offset!=0x1cu||s.size!=4u||s.knownPrefix!=4u||
       s.preimageWord||r.copied>4u||r.clampedAmount>4u||r.copied>r.clampedAmount)return false;
    const auto childWord=std::uint32_t(countRead_.bytes[0])|(std::uint32_t(countRead_.bytes[1])<<8u)|
        (std::uint32_t(countRead_.bytes[2])<<16u)|(std::uint32_t(countRead_.bytes[3])<<24u);
    const ServiceObservation* prior=nullptr;
    if(r.entryKind==TopologyEntryKind::returnedFirstMaterial){
        if(childWord!=1u||!materialReader_.prepared||materialReader_.dataIdentity!=r.dataIdentity||
           materialReader_.dataEpoch!=r.dataEpoch)return false;
        for(const auto& read:materialReader_.reads)if(!read.resultAdmitted||!read.afterObserved)return false;
        prior=&materialReader_.reads.back().after;
    }else if(r.entryKind==TopologyEntryKind::nonpositiveCount){
        if(materialReader_.entered||(childWord!=0u&&!(childWord&0x80000000u)))return false;
        prior=&countRead_.after;
    }else return false;
    for(std::size_t i=0;i<4u;++i)if(r.preimageMask[i]!=255u||r.knownMask[i]!=255u||r.preimageBytes[i]||
        (i>=r.copied&&r.bytes[i]))return false;
    if(r.prepared){
        if(r.before.devices.size()!=std::size_t(deviceCount_)||r.after.devices.size()!=std::size_t(deviceCount_))return false;
    }else if(r.before.devices.size()>std::size_t(deviceCount_)||r.after.devices.size()>std::size_t(deviceCount_)||
             r.beforeObserved||r.attempted)return false;
    if(r.beforeObserved&&(!r.prepared||r.before.memberCursor10!=prior->memberCursor10))return false;
    if(!r.attempted)return !r.callIssued&&!r.delayIssued&&!r.helperEntry&&!r.argumentHandle&&!r.savedHandleS2&&
        !r.nativeReturned&&!r.afterObserved&&!r.resultAdmitted&&!r.sourceItems&&!r.copied&&!r.clampedAmount&&r.pc==s.callPC;
    if(!r.prepared||!r.beforeObserved||!r.callIssued||!r.delayIssued||r.helperEntry!=0x12fb30u||
       r.argumentHandle!=r.sourceHandle||r.savedHandleS2!=r.sourceHandle||r.before.memberCursor10>selectedLength_||
       selectedLength_-r.before.memberCursor10>std::uint32_t(std::numeric_limits<std::int32_t>::max())||
       r.clampedAmount!=std::min<std::size_t>(4u,selectedLength_-r.before.memberCursor10))return false;
    const bool admitted=r.nativeReturned&&r.afterObserved&&r.copied==r.clampedAmount&&
        r.after.memberCursor10==r.before.memberCursor10+r.copied;
    if(r.resultAdmitted!=admitted)return false;
    if(admitted)return r.sourceItems==(r.copied?1:0)&&r.pc==s.returnPC;
    //The existing130560 adapter advances this member only at its successful
    //return. Validate that invariant against the ACTUAL observation, without
    //replacing device/window effects or manufacturing a missing after.
    if(!r.nativeReturned&&r.afterObserved&&r.after.memberCursor10!=r.before.memberCursor10)return false;
    return !r.sourceItems&&r.pc==(r.nativeReturned?0x12fb30u:
        r.helperDiagnostic.sourcePC?r.helperDiagnostic.sourcePC:0x12fb30u);
    //No retired role dereferences, aggregate callbacks, cursor reconstruction
    //or source-return inference from initialized destination bytes.
}
#include "original_water_pool_index_reads50la.inl"
#include "original_water_pool_record_read50la.inl"
#include "original_water_pool_element_read50la.inl"
bool OriginalWaterPoolSourceFile50LA::closeSource131040(D& d)noexcept {
    d={};const auto version=versionWordNative();
    if(sourceCloseAttempted_||nativeClosed_||!version||*version==2u||
       !canCloseNative(poolIdentity_)||nativeHandle_==-1||!sourceHandle1E0_)
        return fail(d,E::phase,0x131040);
    sourceCloseAttempted_=true;pc_=0x131040;
    sourceCloseReturned_=resolver_->closeSourceBankMember131040(selectedRoot_,this,nativeHandle_,false,d);
    sourceCloseDiagnostic_=d;
    if(!sourceCloseReturned_){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    nativeHandle_=-1;sourceCloseV0_=1;pc_=0x2bb110;return true;
    //P+1E0 retains the numeric source output. No Entity release/loader return.
}
bool OriginalWaterPoolSourceFile50LA::historyNative(const void* poolIdentity)const noexcept {
    if(!poolIdentity||poolIdentity_!=poolIdentity||thread_!=std::this_thread::get_id()||
       !resolver_||!provider_||!track_||(nativeClosed_&&pinOwned_)||!materialReaderHistoryNative()||
       !topologyReadHistoryNative()||!indexReadHistoryNative()||!firstTopologyRecordHistoryNative()||
       !firstTopologyElementHistoryNative())return false;
    if(!searchReached_)return !pinOwned_&&nativeHandle_==-1&&!openAttempted_&&!openReturned_&&
        !searchReturned_&&!found_&&!sourceReturnV0_&&!outputHandleStored_&&!outputAssociationStored_&&
        !read_.attempted&&!countRead_.entered&&!sourceCloseAttempted_; //Bookkeeping allocation refusal.
    if(!freshIdentity_||!membersIdentity_||!devicesIdentity_||!serviceMountIdentity_||
       !serviceTrackGeneration_||memberCount_<=0||deviceCount_<=1||name_.empty()||
       before160_< -1||before160_>=memberCount_||before168_< -1||before168_>=memberCount_||
       after160_< -1||after160_>=memberCount_||after168_< -1||after168_>=memberCount_||
       comparisons_.size()>std::size_t(memberCount_)+1u)return false;
    const auto start=before168_+1==memberCount_?0:before168_+1;
    std::size_t scanned=0;bool comparisonPartial=false,matched=false,fastReturned=before160_==-1;
    for(std::size_t n=0;n<comparisons_.size();++n){const auto& c=comparisons_[n];
        if(c.row<0||c.row>=memberCount_||comparisonPartial||matched)return false;
        if(c.fast){if(n||before160_==-1||c.row!=before160_)return false;fastReturned=c.returned;}
        else {if(!fastReturned||!rotatingAdvanced_||c.row!=(start+std::int64_t(scanned))%memberCount_)return false;++scanned;}
        if(!c.filenameIdentity){if(!c.returned||c.equal||c.foldReads||c.foldedMask)return false;}
        else {
            if(c.equalPrefix>name_.size()||c.foldReads<2u*c.equalPrefix||c.foldedMask>15u)return false;
            if(c.returned&&((c.foldedMask&12u)!=12u||c.equal!=(c.folded[2]==c.folded[3])))return false;
        }
        comparisonPartial=!c.returned;matched=c.returned&&c.equal;
    }
    if(scanned>std::size_t(memberCount_)||fastMatch_!=(matched&&!rotatingAdvanced_)||
       rotatingWrapped_!=(rotatingAdvanced_&&before168_+1==memberCount_)||
       rotatingMatchStored_!=(rotatingAdvanced_&&found_)||cachedStored_!=(rotatingAdvanced_&&searchReturned_))return false;
    if(searchReturned_){
        if(comparisonPartial||found_!=matched||(!found_&&scanned!=std::size_t(memberCount_)))return false;
    }else if(found_||matched||cachedStored_||associationObserved_||sourceReturnV0_)return false;
    if(after160_!=(cachedStored_?(found_?after168_:-1):before160_)||
       after168_!=(rotatingMatchStored_?comparisons_.back().row:rotatingAdvanced_?start:before168_))return false;
    if(missMode170_){if(!searchReturned_||found_||associationObserved_)return false;
        if(*missMode170_){if(sourceReturnV0_||pc_!=0x12dcb0)return false;}
        else if(sourceReturnV0_!=0||pc_!=0x132a0c)return false;
    }else if(searchReturned_&&!found_)return false;
    if(associationObserved_){if(!searchReturned_||!found_||row_!=after160_||!filenameIdentity_||
           warmObserved_!=(opaqueAssociation_!=nullptr))return false;}
    else if(warmObserved_||opaqueAssociation_||openAttempted_||openReturned_)return false;
    if(warmT1ZeroBranch_!=warmObserved_||bool(warmDelayV0_)!=warmObserved_)return false;
    if(warmObserved_&&(warmDelayV0_!=-1||pc_!=0x1329c0||openAttempted_||sourceReturnV0_||sourceHandle1E0_||sourceAssociated1E8_))return false;
    if(selectedRoot_&&(selectedRoot_!=track_.get()||selectedMountIdentity_!=serviceMountIdentity_||
       selectedMount_!=serviceTrackMount_||selectedGeneration_!=serviceTrackGeneration_))return false;
    if(openAttempted_&&(!selectedRoot_||!memberIdentity_||warmObserved_||!associationObserved_))return false;
    if(openReturned_&&!openAttempted_)return false;
    if(outputHandleStored_!=bool(sourceHandle1E0_)||outputAssociationStored_!=bool(sourceAssociated1E8_)||
       outputHandleStored_!=outputAssociationStored_||(outputHandleStored_&&
       (!openReturned_||sourceHandle1E0_!=std::int64_t(row_)||*sourceAssociated1E8_!=nullptr||sourceReturnV0_!=1)))return false;
    if(sourceReturnV0_&&*sourceReturnV0_==1&&!outputHandleStored_)return false;
    if(read_.attempted){
        if(!openReturned_||!outputHandleStored_||read_.copied>4u||read_.clampedAmount>4u||
           read_.before.devices.size()!=std::size_t(deviceCount_)||
           read_.after.devices.size()!=std::size_t(deviceCount_))return false;
        for(std::size_t i=0;i<4u;++i)if(read_.known[i]!=(i<read_.copied))return false;
        if(read_.resultAdmitted){if(!read_.nativeReturned||!read_.afterObserved||
               read_.copied!=read_.clampedAmount||read_.sourceItems!=(read_.copied?1:0)||
               read_.after.memberCursor10!=read_.before.memberCursor10+read_.copied)return false;}
        else if(read_.sourceItems)return false;
        if(!read_.nativeReturned&&read_.afterObserved&&read_.after.memberCursor10!=read_.before.memberCursor10)return false;
    }else if(read_.nativeReturned||read_.resultAdmitted||read_.copied||read_.sourceItems||sourceCloseAttempted_)return false;
    const auto& count=countRead_;
    for(std::size_t i=0;i<4u;++i)
        if(!count.known[i]||(i>=count.copied&&count.bytes[i]))return false;
    if(count.entered){
        const auto version=versionWordNative();
        if(!count.resourceIdentity||!version||*version!=2u||!read_.afterObserved||
           sourceCloseAttempted_||count.copied>4u||count.clampedAmount>4u||
           count.copied>count.clampedAmount||!count.pc)return false;
        if(count.prepared){
            if(count.before.devices.size()!=std::size_t(deviceCount_)||
               count.after.devices.size()!=std::size_t(deviceCount_))return false;
        }else if(count.beforeObserved||count.attempted)return false;
        if(count.beforeObserved&&count.before.memberCursor10!=read_.after.memberCursor10)return false;
        if(count.attempted){
            if(!count.prepared||!count.beforeObserved||count.before.memberCursor10>selectedLength_||
               selectedLength_-count.before.memberCursor10>std::uint32_t(std::numeric_limits<std::int32_t>::max())||
               count.clampedAmount!=std::min<std::size_t>(4u,selectedLength_-count.before.memberCursor10))return false;
            if(count.resultAdmitted){
                if(!count.nativeReturned||!count.afterObserved||count.copied!=count.clampedAmount||
                   count.sourceItems!=(count.copied?1:0)||count.pc!=0x2bb1a4||
                   count.after.memberCursor10!=count.before.memberCursor10+count.copied)return false;
            }else if(count.sourceItems)return false;
            if(!count.nativeReturned&&count.afterObserved&&
               count.after.memberCursor10!=count.before.memberCursor10)return false;
        }else if(count.nativeReturned||count.afterObserved||count.resultAdmitted||
                 count.copied||count.clampedAmount||count.sourceItems)return false;
    }else if(count.resourceIdentity||count.prepared||count.beforeObserved||count.attempted||
             count.nativeReturned||count.afterObserved||count.resultAdmitted||count.copied||
             count.clampedAmount||count.sourceItems||count.pc||
             !count.before.devices.empty()||!count.after.devices.empty())return false;
    if(sourceCloseAttempted_){const auto word=versionWordNative();if(!word||*word==2u)return false;}
    if(sourceCloseReturned_&&(!sourceCloseAttempted_||sourceCloseV0_!=1||nativeHandle_!=-1||pc_!=0x2bb110))return false;
    if(bool(sourceCloseV0_)!=sourceCloseReturned_||(nativeHandleClosed_&&!nativeCloseAttempted_))return false;
    if(nativeHandle_!=-1&&(!openAttempted_||nativeHandle_!=row_||sourceCloseReturned_||nativeClosed_))return false;
    return true; //Only values/opaque identities. Never dereference retired roles.
}
bool OriginalWaterPoolSourceFile50LA::preflightNative(const void* poolIdentity)const noexcept {
    if(!historyNative(poolIdentity))return false;
    if(nativeClosed_)return !pinOwned_&&nativeHandle_==-1;
    if(!searchReached_)return !pinOwned_&&nativeHandle_==-1;
    if(!pinOwned_||!resolver_->freshFileCapacity_||!resolver_->scratch_)return false;
    const auto& s=*resolver_->freshFileCapacity_;
    if(&s!=freshIdentity_||s.waterPoolFileBorrower!=this||s.provider!=provider_||
       s.members150!=membersIdentity_||s.memberCount154!=memberCount_||
       s.devices14!=devicesIdentity_||s.deviceCount18!=deviceCount_||
       s.musicOwner||s.streamLibraryOwner||s.loadingRaOwner)return false;
    const void* identity=nullptr;std::size_t mount=0;std::uint64_t generation=0;
    if(!resolver_->waterPoolTrackRelationNative(track_.get(),provider_.get(),identity,mount,generation)||
       identity!=serviceMountIdentity_||mount!=serviceTrackMount_||generation!=serviceTrackGeneration_)return false;
    if(nativeHandle_!=-1){
        if(!resolver_->sourceBankMemberCurrent(selectedRoot_,this,nativeHandle_))return false;
        const auto& row=s.members150[nativeHandle_];
        if(row.originalMember!=memberIdentity_||row.filename08!=filenameIdentity_||
           row.deviceID00!=device_||row.length14!=selectedLength_)return false;
        //No reset/replay of a failed read; cursor is whatever the real helper
        //left, not before+copied. Other files may freely change manager windows.
        const MaterialRead* latestMaterial=nullptr;
        for(const auto& r:materialReader_.reads)if(r.attempted)latestMaterial=&r;
        const IndexRead* latestIndex=nullptr;
        for(const auto& r:indexReader_.reads)if(r.attempted)latestIndex=&r;
        if(firstTopologyElement_.read.attempted){
            const auto& r=firstTopologyElement_.read;
            if(!r.afterObserved||row.cursor10!=r.after.memberCursor10)return false;
        }else if(firstTopologyRecord_.read.attempted){
            const auto& r=firstTopologyRecord_.read;
            if(!r.afterObserved||row.cursor10!=r.after.memberCursor10)return false;
        }else if(latestIndex){
            if(!latestIndex->afterObserved||row.cursor10!=latestIndex->after.memberCursor10)return false;
        }else if(topologyRead_.attempted){
            //An attempted topology read supersedes all earlier tails, even
            //on helper refusal. Missing actual after means no close admission.
            if(!topologyRead_.afterObserved||row.cursor10!=topologyRead_.after.memberCursor10)return false;
        }else if(latestMaterial){
            if(!latestMaterial->afterObserved||row.cursor10!=latestMaterial->after.memberCursor10)return false;
        }else if(countRead_.attempted){
            //historyNative authenticated count.before against version.after.
            //A failed helper's actual after cursor is still the latest tail.
            //Without an after observation, never silently fall back to the
            //older version cursor or infer one from the destination prefix.
            if(!countRead_.afterObserved||row.cursor10!=countRead_.after.memberCursor10)return false;
        }else if(read_.attempted&&read_.afterObserved&&row.cursor10!=read_.after.memberCursor10)return false;
        if(!read_.attempted&&row.cursor10!=0)return false;
    }else for(std::int32_t n=0;n<s.memberCount154;++n)if(s.members150[n].courseConfigOwner==this)return false;
    return true;
    //No present160/168, row1C, provider revision, ctype or Config pin equality.
}
bool OriginalWaterPoolSourceFile50LA::canCloseNative(const void* poolIdentity)const noexcept {
    if(!preflightNative(poolIdentity))return false;
    if(nativeHandle_==-1)return true;
    const auto& s=*resolver_->freshFileCapacity_;const auto device=s.members150[nativeHandle_].deviceID00;
    return s.devices14&&device>=0&&device<s.deviceCount18&&(s.devices14[device].flags0C&2u);
}
bool OriginalWaterPoolSourceFile50LA::closeNative(D& d)noexcept {
    d={};if(!canCloseNative(poolIdentity_))return fail(d,E::phase,0);
    if(nativeClosed_)return true;
    nativeCloseAttempted_=true;
    if(nativeHandle_!=-1){
        const bool closed=resolver_->closeSourceBankMember131040(selectedRoot_,this,nativeHandle_,false,d);
        nativeCloseDiagnostic_=d;if(!closed)return false;
        nativeHandle_=-1;nativeHandleClosed_=true;
    }
    if(pinOwned_){resolver_->freshFileCapacity_->waterPoolFileBorrower=nullptr;pinOwned_=false;}
    nativeClosed_=true;return true;
    //No fake close for miss/warm/no-open. No source PC/outputs/count changed.
}
