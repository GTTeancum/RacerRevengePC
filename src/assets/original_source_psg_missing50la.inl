//Included inside rr::assets with actual Fresh source FileManager services.
bool OriginalSourcePsgMissingLookup50LA::capture132820(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalRaTrackSourceFile50LA> root,const std::string& exactName,
    std::shared_ptr<OriginalSourcePsgMissingLookup50LA>& pending,D& d)noexcept {
    d={};if(!resolver||!root||pending)return fail(d,E::phase,0x132820);
    return resolver->captureSourcePsgMissing132820(std::move(root),exactName,pending,d);
}
bool OriginalStandardTextureResolver50LA::captureSourcePsgMissing132820(
    std::shared_ptr<OriginalRaTrackSourceFile50LA> root,const std::string& name,
    std::shared_ptr<OriginalSourcePsgMissingLookup50LA>& pending,D& d)noexcept try {
    d={};if(pending||!root||root->resolver_.get()!=this||root->nativeClosed_||!root->joined_||
       root->thread_!=std::this_thread::get_id()||!freshFileCapacity_||name.find('\0')!=std::string::npos)
        return fail(d,E::phase,0x132820);
    auto& s=*freshFileCapacity_;const auto& t=s.track;
    if(t.root!=root.get()||!t.joined||!s.members150||s.memberCount154<=0||
       s.cursor160< -1||s.cursor160>=s.memberCount154||s.cursor168< -1||s.cursor168>=s.memberCount154||
       root->nativeMissingLookupBorrowers_==std::numeric_limits<std::size_t>::max())
        return fail(d,E::phase,0x132820);
    //All fallible bookkeeping precedes source search effects. Provider is NOT
    //queried: even a sealed provider's absence cannot replace actual rows.
    std::string retainedName=name;
    auto attempt=std::shared_ptr<OriginalSourcePsgMissingLookup50LA>(new OriginalSourcePsgMissingLookup50LA);
    auto& p=*attempt;p.resolver_=root->resolver_;p.root_=std::move(root);p.name_=std::move(retainedName);
    p.before160_=s.cursor160;p.before168_=s.cursor168;p.memberCount_=s.memberCount154;
    p.mount_=t.mount;p.generation_=t.generation;
    ++p.root_->nativeMissingLookupBorrowers_;p.nativeBorrowed_=true;
    pending=std::move(attempt);p.searchReached_=true;p.sourcePC_=0x132820;
    if(s.cursor160!=-1){const auto& row=s.members150[s.cursor160];
        p.fastMatch_=row.filename08&&courseSourceName308F00(p.name_,row.filename08);}
    p.found_=p.fastMatch_;
    if(!p.found_){
        ++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0;
        auto index=static_cast<std::int32_t>(s.cursor168);const auto start=index;
        do {const auto& row=s.members150[index];
            if(row.filename08&&courseSourceName308F00(p.name_,row.filename08)){
                s.cursor168=index;p.found_=true;break;}
            if(++index==s.memberCount154)index=0;
        }while(index!=start);
        s.cursor160=p.found_?s.cursor168:-1; //132938 including genuine miss.
    }
    p.after160_=s.cursor160;p.after168_=s.cursor168;p.searchReturned_=true;
    d.sourceEffectsCommitted=true;
    if(p.found_){
        const auto& row=s.members150[p.after160_];
        p.foundMemberIdentity_=row.originalMember;p.foundFilenameIdentity_=row.filename08;
        p.sourcePC_=0x132988;return fail(d,E::unsupported,p.sourcePC_);
    }
    //Actual13294C branch. Mode0 authenticates return0, not filesystem absence.
    p.observed170_=s.sourceFileMode170;
    if(*p.observed170_){p.sourcePC_=0x12dcb0;return fail(d,E::unsupported,p.sourcePC_);}
    p.returnV0_=0;p.sourcePC_=0x132a0c;d.sourcePC=p.sourcePC_;return true;
}catch(...){return fail(d,E::allocation,0x132820);}
bool OriginalStandardTextureResolver50LA::sourcePsgMissingCurrentNative(
    const OriginalSourcePsgMissingLookup50LA& p)const noexcept {
    if(p.resolver_.get()!=this||!p.root_||p.thread_!=std::this_thread::get_id()||
       !p.searchReached_||!p.searchReturned_||p.memberCount_<=0||
       p.before160_< -1||p.before160_>=p.memberCount_||p.before168_< -1||p.before168_>=p.memberCount_||
       p.after160_< -1||p.after160_>=p.memberCount_||p.after168_< -1||p.after168_>=p.memberCount_)return false;
    if(p.fastMatch_){if(!p.found_||p.before160_==-1||p.after160_!=p.before160_||p.after168_!=p.before168_)return false;}
    else if(p.found_){if(p.after160_!=p.after168_||p.after160_<0)return false;}
    else {
        const auto next=p.before168_+1==p.memberCount_?0:p.before168_+1;
        if(p.after160_!=-1||p.after168_!=next)return false;
    }
    if(p.found_){if(p.observed170_||p.returnV0_||p.sourcePC_!=0x132988||!p.foundFilenameIdentity_)return false;}
    else {
        if(!p.observed170_||p.foundMemberIdentity_||p.foundFilenameIdentity_||p.continuationAttempted_||p.fileBorrower_||p.collisionContinuationAttempted_||p.collisionFileBorrower_)return false;
        if(*p.observed170_){if(p.returnV0_||p.sourcePC_!=0x12dcb0)return false;}
        else if(!p.returnedMissing132820())return false;
    }
    //Historical sharedcursor/mode observations remain valid after other real
    //lookups. Never restore them or require they still equal current globals.
    if(p.fileBorrower_&&!p.continuationAttempted_)return false;
    if(p.collisionSearchBorrower_&&(p.continuationAttempted_||p.fileBorrower_))return false;
    if(p.collisionFileBorrower_&&(!p.collisionContinuationAttempted_||!p.collisionSearchBorrower_||
       p.continuationAttempted_||p.fileBorrower_))return false;
    if(p.nativeClosed_)return !p.nativeBorrowed_&&!p.fileBorrower_&&!p.collisionSearchBorrower_&&!p.collisionFileBorrower_;
    if(!p.nativeBorrowed_||p.root_->nativeClosed_||!p.root_->joined_||
       !p.root_->nativeMissingLookupBorrowers_||p.root_->resolver_.get()!=this||!freshFileCapacity_)return false;
    const auto& t=freshFileCapacity_->track;
    return t.root==p.root_.get()&&t.joined&&t.mount==p.mount_&&t.generation==p.generation_;
}
bool OriginalSourcePsgMissingLookup50LA::preflightNative()const noexcept {
    return resolver_&&resolver_->sourcePsgMissingCurrentNative(*this);
}
bool OriginalStandardTextureResolver50LA::closeSourcePsgMissingNative(
    OriginalSourcePsgMissingLookup50LA& p,D& d)noexcept {
    d={};if(!sourcePsgMissingCurrentNative(p))return fail(d,E::phase,0);
    if(p.nativeClosed_)return true;
    if(p.fileBorrower_||p.collisionSearchBorrower_||p.collisionFileBorrower_)return fail(d,E::phase,0);
    --p.root_->nativeMissingLookupBorrowers_;p.nativeBorrowed_=false;p.nativeClosed_=true;
    return true; //No source131040, no handle/resource output, no cursor reset.
}
bool OriginalSourcePsgMissingLookup50LA::closeNative(D& d)noexcept {
    d={};return resolver_?resolver_->closeSourcePsgMissingNative(*this,d):fail(d,E::phase,0);
}
