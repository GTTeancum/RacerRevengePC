//Inside rr::assets, after actual shared source bank and retained-search services.
bool OriginalSourceCollisionFile50LA::continueFound132988(
    std::shared_ptr<OriginalSourcePsgMissingLookup50LA> search,const void* owner,
    std::shared_ptr<OriginalSourceCollisionFile50LA>& pending,D& d)noexcept {
    d={};if(!search||!search->resolver_||pending||!owner)return fail(d,E::phase,0x132988);
    auto resolver=search->resolver_;
    return resolver->continueSourceCollisionFound132988(std::move(search),owner,pending,d);
}
bool OriginalStandardTextureResolver50LA::continueSourceCollisionFound132988(
    std::shared_ptr<OriginalSourcePsgMissingLookup50LA> previous,const void* owner,
    std::shared_ptr<OriginalSourceCollisionFile50LA>& pending,D& d)noexcept try {
    d={};if(!previous||pending||!owner||!sourcePsgMissingCurrentNative(*previous)||previous->nativeClosed_||
       !previous->found_||previous->sourcePC_!=0x132988||previous->returnV0_||previous->observed170_||
       previous->continuationAttempted_||previous->fileBorrower_||previous->collisionSearchBorrower_!=owner||
       previous->collisionContinuationAttempted_||previous->collisionFileBorrower_||!freshFileCapacity_)
        return fail(d,E::phase,0x132988);
    auto& q=*previous;auto& s=*freshFileCapacity_;const auto& t=s.track;
    if(!s.members150||s.memberCount154!=q.memberCount_||s.cursor160!=q.after160_||s.cursor168!=q.after168_||
       s.cursor160<0||s.cursor160>=s.memberCount154||!q.root_->archive_||!q.root_->provider_)
        return fail(d,E::phase,0x132988);
    const auto index=static_cast<std::int32_t>(s.cursor160);auto& row=s.members150[index];
    if(row.originalMember!=q.foundMemberIdentity_||row.filename08!=q.foundFilenameIdentity_||
       !row.filename08||!courseSourceName308F00(q.name_,row.filename08)||row.deviceID00!=t.device||
       !row.canonicalData)return fail(d,E::source,0x132988);
    //The actual source row/bank is authority. No native ModelFileID, decoded
    //COL resource or provider-only match can authorize the interval.
    OriginalStandardArchiveProvider50LA::MemberView winner;
    if(q.root_->provider_->lookup132820(q.name_,winner)!=OriginalStandardArchiveProvider50LA::Lookup::present||
       !winner.member||!winner.data||winner.mount!=t.mount||winner.bankGeneration!=t.generation||
       winner.revision!=q.root_->provider_->revision())return fail(d,E::source,0x132988);
    const auto expected=q.root_->archive_->member(winner.ordinal);
    if(!expected.identity||!expected.data||row.originalMember!=expected.identity||row.length14!=expected.size||
       expected.size!=winner.size||!courseSourceName308F00(q.name_,expected.identity->name)||
       std::memcmp(expected.data,winner.data,winner.size)||std::memcmp(row.canonicalData,winner.data,winner.size))
        return fail(d,E::source,0x132988);
    auto attempt=std::shared_ptr<OriginalSourceCollisionFile50LA>(new OriginalSourceCollisionFile50LA);
    auto& p=*attempt;p.resolver_=q.resolver_;p.search_=std::move(previous);p.collisionOwner_=owner;
    p.expected_=expected;p.row_=index;p.mount_=t.mount;p.generation_=t.generation;p.pc_=0x1329a0;
    q.collisionContinuationAttempted_=true;q.collisionFileBorrower_=&p;pending=std::move(attempt);
    if(row.associatedObject1C){
        p.warmObserved_=true;
        //Source t1=1 would store signed CURRENT160 at1329B8, then actual row1C
        //at1329CC. There is no genuine typed source COL resource owner yet;
        //bound before either output, never substitute PSG -1/native weak cache.
        return fail(d,E::unsupported,p.pc_);
    }
    p.pc_=0x131380;
    if(!openSourceBankMember131380(q.root_.get(),&p,p.row_,p.handle_,d)){p.pc_=d.sourcePC;return false;}
    p.pc_=0x1329e0;p.sourceHandle148_=s.cursor160;p.outputHandleStored_=true;
    p.pc_=0x1329e4;p.sourceCache150_=nullptr;p.outputCacheStored_=true;
    p.returnedCold_=true;p.pc_=0x1329e8;d.sourcePC=p.pc_;d.sourceEffectsCommitted=true;return true;
}catch(...){return fail(d,E::allocation,0x132988);}
bool OriginalStandardTextureResolver50LA::sourceCollisionFileCurrentNative(
    const OriginalSourceCollisionFile50LA& p,bool active)const noexcept {
    if(p.resolver_.get()!=this||!p.search_||!p.collisionOwner_||p.thread_!=std::this_thread::get_id()||
       !p.expected_.identity||!p.expected_.data||p.row_<0||!sourcePsgMissingCurrentNative(*p.search_))return false;
    const auto& q=*p.search_;
    if(!q.collisionContinuationAttempted_||q.continuationAttempted_||q.fileBorrower_||
       p.row_!=q.after160_||p.mount_!=q.mount_||p.generation_!=q.generation_)return false;
    if(p.outputHandleStored_!=p.outputCacheStored_||p.returnedCold_!=p.outputHandleStored_||
       (p.returnedCold_&&(!p.sourceHandle148_||*p.sourceHandle148_!=p.row_||
        !p.sourceCache150_||*p.sourceCache150_||p.warmObserved_)))return false;
    if(p.associationAttempted_){
        if(!p.associatedResource_||!p.resourceOwner_||p.warmObserved_||!p.returnedCold_||
           p.associationGuards_>3u||
           (p.associationStored_&&(p.associationGuards_!=3u||p.associationReturnV0_!=1))||
           (p.associationReturned_&&!p.associationStored_))return false;
        //History after withdrawal must not dereference a subsequently freed resource.
        if(!p.associationWithdrawn_&&
           (p.payloadBorrower_!=p.resourceOwner_||
            p.associatedResource_->sourceAssociationBorrower_!=&p||
            !p.associatedResource_->sourceAssociationReady1326A0(&p,p.resourceOwner_)))return false;
    }else if(p.associatedResource_||p.resourceOwner_||p.associationStored_||p.associationReturned_||
             p.associationWithdrawn_||p.associationGuards_||p.associationReturnV0_)return false;
    if(active&&(p.associationWithdrawn_||p.sourceCloseAttempted_))return false;
    if(p.nativeClosed_)return !active&&p.handle_==-1&&!q.collisionFileBorrower_&&!p.payloadBorrower_&&
        (!p.associationAttempted_||p.associationWithdrawn_);
    if(q.nativeClosed_||q.collisionSearchBorrower_!=p.collisionOwner_||q.collisionFileBorrower_!=&p||
       !freshFileCapacity_||!q.root_)return false;
    const auto& s=*freshFileCapacity_;const auto& t=s.track;
    if(t.root!=q.root_.get()||!t.joined||t.mount!=p.mount_||t.generation!=p.generation_||
       !s.members150||p.row_>=s.memberCount154)return false;
    const auto& row=s.members150[p.row_];
    if(row.originalMember!=p.expected_.identity||row.filename08!=q.foundFilenameIdentity_||
       row.length14!=p.expected_.size||!row.canonicalData||std::memcmp(row.canonicalData,p.expected_.data,p.expected_.size)||row.deviceID00!=t.device)return false;
    if(!p.warmObserved_){
        const void* expected=p.associationStored_&&!p.associationWithdrawn_?p.associatedResource_:nullptr;
        if(row.associatedObject1C!=expected)return false;
    }
    if(p.sourceCloseReturned_)return !active&&p.sourceCloseAttempted_&&p.sourceCloseResult_==1&&p.handle_==-1;
    if(p.handle_!=-1)return p.handle_==p.row_&&sourceBankMemberCurrent(q.root_.get(),&p,p.handle_)&&
        (!active||p.returnedCold_);
    if(active||p.returnedCold_)return false;
    for(std::int32_t n=0;n<s.memberCount154;++n)if(s.members150[n].courseConfigOwner==&p)return false;
    return true; //Partialopen/warm bound without a fabricated returned handle.
}
bool OriginalSourceCollisionFile50LA::preflightNative()const noexcept {
    return resolver_&&resolver_->sourceCollisionFileCurrentNative(*this,false);
}
bool OriginalSourceCollisionFile50LA::current()const noexcept {
    return resolver_&&resolver_->sourceCollisionFileCurrentNative(*this,true);
}
bool OriginalSourceCollisionFile50LA::read12FB30(void* output,std::size_t count,ReadReceipt& r,D& d)noexcept {
    d={};if(r.entered||associationAttempted_||!current()||(!output&&count))return fail(d,E::phase,0x12fb30);
    auto& s=*resolver_->freshFileCapacity_;r.before=s.members150[handle_].cursor10;r.requested=count;r.entered=true;
    r.returned=resolver_->readSourceBankMember12FB30(search_->root_.get(),this,handle_,output,count,r.done,d);
    r.diagnostic=d;
    r.afterKnown=current();if(r.afterKnown)r.after=s.members150[handle_].cursor10;
    //A genuine EOF-short return is still a return. Caller controls per-byte
    //knownness and must ignore the source count, not synthesize missing bytes.
    return r.returned;
}
bool OriginalSourceCollisionFile50LA::closeSource131040(D& d)noexcept {
    d={};if(sourceCloseAttempted_||!current()||
       (payloadBorrower_&&(!associationReturned_||associationWithdrawn_||payloadBorrower_!=resourceOwner_)))
        return fail(d,E::phase,0x131040);
    sourceCloseAttempted_=true;
    if(!resolver_->closeSourceBankMember131040(search_->root_.get(),this,handle_,false,d))return false;
    handle_=-1;sourceCloseReturned_=true;sourceCloseResult_=1;return true;
}
bool OriginalSourceCollisionFile50LA::closeNative(D& d)noexcept {
    d={};if(payloadBorrower_||(associationAttempted_&&!associationWithdrawn_)||!preflightNative())
        return fail(d,E::phase,0);
    if(nativeClosed_)return true;
    if(handle_!=-1){
        if(!resolver_->closeSourceBankMember131040(search_->root_.get(),this,handle_,false,d))return false;
        handle_=-1;
    }
    search_->collisionFileBorrower_=nullptr;nativeClosed_=true;return true;
    //Search remains pinned by COL until its own explicit native close. No
    //source131040 receipt, stack/B0 reset, or source callee return is minted.
}

bool OriginalSourceCollisionFile50LA::payloadCustodyCurrentNative(const void* owner)const noexcept {
    //Cleanup identity, not permission to read or resume source progress. In
    //particular, genuine closed handles and withdrawn association are allowed.
    return owner&&payloadBorrower_==owner&&!nativeClosed_&&preflightNative();
}
bool OriginalSourceCollisionFile50LA::associateSource1326A0(
    Resource& resource,const Payload* owner,D& d)noexcept {
    d={};if(associationAttempted_||!owner||payloadBorrower_!=owner||!current()||
       resource.sourceAssociationBorrower_||!resource.sourceAssociationReady1326A0(this,owner))
        return fail(d,E::phase,0x1326a0);
    //Native lifetime token precedes every source guard, including a refusal.
    associatedResource_=&resource;resourceOwner_=owner;associationAttempted_=true;
    resource.sourceAssociationBorrower_=this;
    auto& s=*resolver_->freshFileCapacity_;
    pc_=0x1326f8;
    if(!fileIoAssertions().cache827C_3E1BA8.reach(handle_>=0,d,pc_))return false;
    associationGuards_|=1u;pc_=0x132764;
    if(!fileIoAssertions().cache8280_3E1BB0.reach(handle_<s.memberCount154,d,pc_))return false;
    associationGuards_|=2u;
    //Original assertion-return behavior does not authorize host out-of-bounds.
    if(handle_<0||handle_>=s.memberCount154||!s.members150)return fail(d,E::unsupported,0x132798);
    pc_=0x1327a8;associationReturnV0_=1;
    pc_=0x1327b0;s.members150[handle_].associatedObject1C=&resource;associationStored_=true;
    pc_=0x1327c8;associationReturned_=true;d.sourcePC=pc_;d.sourceEffectsCommitted=true;return true;
}
bool OriginalSourceCollisionFile50LA::withdrawAssociationNative(D& d)noexcept {
    d={};if(!preflightNative())return fail(d,E::phase,0);
    if(!associationAttempted_||associationWithdrawn_)return true;
    if(associatedResource_->sourceResultBorrower_||associatedResource_->sourceDirectTransformBorrower_||associatedResource_->sourceUpdateBorrower_||associatedResource_->sourceContactBorrower_)return fail(d,E::phase,0);
    //All checks precede mutation. This is native cancellation, not a source
    //1326A0(NULL) call or ref60 decrement. Arrays/Resource remain owned next.
    auto& s=*resolver_->freshFileCapacity_;
    if(associationStored_)s.members150[row_].associatedObject1C=nullptr;
    associatedResource_->sourceAssociationBorrower_=nullptr;associationWithdrawn_=true;
    return true;
}
