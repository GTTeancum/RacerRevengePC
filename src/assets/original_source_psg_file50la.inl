// Included inside rr::assets after the existing SourceBank services.
bool OriginalSourcePsgFile50LA::capture132820(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalRaTrackSourceFile50LA> root,const std::string& name,
    OriginalStandardModelFileId50LA id,std::shared_ptr<OriginalSourcePsgFile50LA>& pending,D& d)noexcept {
    d={};if(!resolver||!root||pending)return fail(d,E::phase,0x132820);
    return resolver->captureSourcePsg132820(std::move(root),name,id,pending,d);
}
bool OriginalStandardTextureResolver50LA::captureSourcePsg132820(
    std::shared_ptr<OriginalRaTrackSourceFile50LA> root,const std::string& name,
    OriginalStandardModelFileId50LA id,std::shared_ptr<OriginalSourcePsgFile50LA>& pending,D& d,
    std::shared_ptr<OriginalSourcePsgFile50LA> warmCreatorFile,const OriginalStandardModelGeometry50LA* warmGeometry,
    const rr::game::OriginalRacerModelSceneOwner50LA* warmCreator)noexcept try {
    d={};if(pending||!root||root->resolver_.get()!=this||root->nativeClosed_||!root->joined_||
       root->thread_!=std::this_thread::get_id()||!freshFileCapacity_||!scratch_||!storage_||
       id.resolver!=this||id.row>=storage_->files.size()||!root->provider_||!root->archive_)
        return fail(d,E::phase,0x132820);
    auto& s=*freshFileCapacity_;const auto& t=s.track;auto& file=*storage_->files[id.row];
    if(t.root!=root.get()||!t.joined||s.provider!=root->provider_||
       !s.members150||s.memberCount154<=0||s.cursor160< -1||s.cursor160>=s.memberCount154||
       s.cursor168< -1||s.cursor168>=s.memberCount154||file.retired||file.kind!=Storage::FileKind::psg||
       !courseSourceName308F00(name,file.member.c_str()))return fail(d,E::phase,0x132820);
    OriginalStandardArchiveProvider50LA::MemberView winner;
    if(root->provider_->lookup132820(name,winner)!=OriginalStandardArchiveProvider50LA::Lookup::present||
       !winner.member||!winner.data||winner.mount!=t.mount||winner.bankGeneration!=t.generation||
       winner.revision!=root->provider_->revision()||winner.size!=file.bytes.size()||
       std::memcmp(winner.data,file.bytes.data(),winner.size))return fail(d,E::source,0x132820);
    const auto expected=root->archive_->member(winner.ordinal);
    if(!expected.identity||!expected.data||expected.size!=winner.size||
       !courseSourceName308F00(name,expected.identity->name)||
       std::memcmp(expected.data,winner.data,winner.size))return fail(d,E::source,0x132820);
    // Allocate/publish native attempt before first represented search mutation.
    std::string retainedName=name; // Fallible native metadata before attempt publication.
    pending=std::shared_ptr<OriginalSourcePsgFile50LA>(new OriginalSourcePsgFile50LA);
    auto& p=*pending;p.resolver_=root->resolver_;p.root_=std::move(root);p.modelId_=id;
    p.expected_=expected;p.name_=std::move(retainedName);p.mount_=winner.mount;p.ordinal_=winner.ordinal;
    p.generation_=winner.bankGeneration;p.revision_=winner.revision;p.searchReached_=true;
    bool found=false;
    if(s.cursor160!=-1){const auto& row=s.members150[s.cursor160];
        found=row.filename08&&courseSourceName308F00(name,row.filename08);}
    if(!found){
        ++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0;
        auto index=static_cast<std::int32_t>(s.cursor168);const auto start=index;
        do {const auto& row=s.members150[index];
            if(row.filename08&&courseSourceName308F00(name,row.filename08)){s.cursor168=index;found=true;break;}
            if(++index==s.memberCount154)index=0;
        }while(index!=start);
        s.cursor160=found?s.cursor168:-1; //132938, including actual unsuccessful search.
    }
    d.sourceEffectsCommitted=true;
    // No provider-only absent return and no invented12DCB0 fallback success.
    if(!found)return fail(d,E::unresolved,0x13294C);
    p.row_=static_cast<std::int32_t>(s.cursor160);auto& row=s.members150[p.row_];
    if(row.originalMember!=p.expected_.identity||row.deviceID00!=t.device||row.length14!=p.expected_.size||
       !row.canonicalData||std::memcmp(row.canonicalData,p.expected_.data,p.expected_.size))
        return fail(d,E::source,0x1329A0);
    if(row.associatedObject1C){
        // Actual t1=0 branch returns this row1C and signed64 handle -1.
        // Supplied native custody is authority only after this real search.
        if(!warmCreatorFile||warmCreatorFile.get()==&p||!warmGeometry||!warmCreator||
           warmCreatorFile->resolver_.get()!=this||warmCreatorFile->root_!=p.root_||
           warmCreatorFile->row_!=p.row_||warmCreatorFile->modelId_.resolver!=id.resolver||
           warmCreatorFile->modelId_.row!=id.row||row.associatedObject1C!=warmGeometry||
           !warmCreatorFile->sourceCloseReturned_||warmCreatorFile->nativeClosed_||
           !sourcePsgGeometryCurrent(*warmCreatorFile,warmGeometry,warmCreator)||
           warmCreatorFile->warmLookupBorrowers_==std::numeric_limits<std::size_t>::max())
            return fail(d,E::unsupported,0x1329A0);
        p.warmCreatorFile_=std::move(warmCreatorFile);p.warmGeometry_=warmGeometry;p.warmCreator_=warmCreator;
        ++p.warmCreatorFile_->warmLookupBorrowers_; // Native lifetime, NOT104960.
        p.returnedWarm_=true;d.sourcePC=0x1329E8;return true;
    }
    // Explicit warm admission cannot silently become an unrequested cold open.
    if(warmCreatorFile||warmGeometry||warmCreator||file.geometryIdentity||!file.geometry.expired()||!file.payloadGeometry.expired())
        return fail(d,E::unsupported,0x1329A0);
    if(!openSourceBankMember131380(p.root_.get(),&p,p.row_,p.handle_,d))return false;
    p.returnedCold_=true;d.sourcePC=0x1329E8;return true;
}catch(...){return fail(d,E::allocation,0x132820);}
bool OriginalStandardTextureResolver50LA::sourcePsgFileCurrent(const OriginalSourcePsgFile50LA& p)const noexcept {
    if(!sourcePsgFoundPredecessorCurrentNative(p))return false;
    if(p.resolver_.get()!=this||!p.root_||p.thread_!=std::this_thread::get_id()||p.nativeClosed_||
       !p.returnedCold_||!freshFileCapacity_||!storage_||p.modelId_.resolver!=this||
       p.modelId_.row>=storage_->files.size()||!sourceBankMemberCurrent(p.root_.get(),&p,p.handle_))return false;
    const auto& s=*freshFileCapacity_;const auto& t=s.track;const auto& row=s.members150[p.handle_];
    const auto& file=*storage_->files[p.modelId_.row];
    if(t.root!=p.root_.get()||!t.joined||t.mount!=p.mount_||t.generation!=p.generation_||
       !p.root_->provider_||p.root_->provider_->revision()!=p.revision_||
       p.row_!=p.handle_||row.originalMember!=p.expected_.identity||
       row.length14!=p.expected_.size||!row.canonicalData||file.retired||file.kind!=Storage::FileKind::psg||
       (p.associationStored_&&!p.associationWithdrawn_ ?
        !sourcePsgGeometryCurrent(p,p.associatedGeometry_,p.associatedModel_) :
        (row.associatedObject1C||file.geometryIdentity||!file.geometry.expired()||!file.payloadGeometry.expired())))return false;
    OriginalStandardArchiveProvider50LA::MemberView winner;
    return p.root_->provider_->lookup132820(p.name_,winner)==OriginalStandardArchiveProvider50LA::Lookup::present&&
        winner.mount==p.mount_&&winner.ordinal==p.ordinal_&&winner.bankGeneration==p.generation_&&
        winner.revision==p.revision_&&winner.size==p.expected_.size&&winner.data&&
        file.bytes.size()==winner.size&&!std::memcmp(file.bytes.data(),winner.data,winner.size)&&
        !std::memcmp(row.canonicalData,winner.data,winner.size);
}
bool OriginalSourcePsgFile50LA::current()const noexcept {
    return resolver_&&resolver_->sourcePsgFileCurrent(*this);
}
bool OriginalSourcePsgFile50LA::read12FB30(void* destination,std::size_t count,std::size_t& done,D& d)noexcept {
    done=0;d={};if(!current())return fail(d,E::phase,0x12FB30);
    // The existing reader owns cursor/scratch effects. On failure retain its
    // exact copied count and partial destination; never promote it to success.
    return resolver_->readSourceBankMember12FB30(root_.get(),this,handle_,destination,count,done,d);
}
bool OriginalSourcePsgFile50LA::cursor12F520(std::size_t& out,D& d)const noexcept {
    d={};if(!current())return fail(d,E::phase,0x12F520);
    return resolver_->sourcePsgCursorNative(*this,out,d);
}
bool OriginalSourcePsgFile50LA::closeNative(D& d)noexcept {
    d={};if(thread_!=std::this_thread::get_id()||!resolver_||!root_)return fail(d,E::phase,0);
    if(nativeClosed_&&returnedWarm_)
        return preflightNative()?true:fail(d,E::phase,0); //Pure warm tombstone, before live bank admission.
    if(!preflightNative())return fail(d,E::phase,0);
    if(nativeClosed_)return true;
    if(returnedWarm_){
        if(warmResourceBorrower_||!warmCreatorFile_||!warmCreatorFile_->warmLookupBorrowers_)
            return fail(d,E::phase,0);
        --warmCreatorFile_->warmLookupBorrowers_;warmLookupWithdrawn_=true;
        warmCreatorFile_.reset();
        if(!resolver_->withdrawSourcePsgFoundNative(*this,d))return false;
        nativeClosed_=true;return true; // No131040 call.
    }
    if(warmLookupBorrowers_)return fail(d,E::phase,0);
    if(associationStored_&&!associationWithdrawn_)return fail(d,E::phase,0);
    // A partial131380 may have acquired handle before scratch admission failed.
    // Do not require returnedCold_ or payload currentness for exact withdrawal.
    if(handle_!=-1){if(!resolver_->closeSourceBankMember131040(root_.get(),this,handle_,false,d))return false;
        handle_=-1;}
    if(!resolver_->withdrawSourcePsgFoundNative(*this,d))return false;
    nativeClosed_=true;return true; // Native cleanup, never source26EE90 completion.
}

bool OriginalSourcePsgFile50LA::readExactNative(void* destination,std::size_t count,
    std::size_t& logicalCursor,D& d)noexcept {
    d={};if(readTerminal_||!current()||(!destination&&count)||logicalCursor>expected_.size)
        return fail(d,E::phase,0);
    std::size_t before=0;if(!cursor12F520(before,d))return false;
    if(before!=logicalCursor)return fail(d,E::phase,0);
    // Native bookkeeping allocation is before any represented IO mutation.
    ReadReceipt next;
    //Original12FB30 may return a genuine EOF-short prefix. Do not reject
    //request>remaining before its cursor/destination effects. Bookkeeping
    //only needs the maximum prefix the authenticated member can supply.
    const auto available=std::min<std::size_t>(count,expected_.size-before);
    try {next.copiedPrefix.resize(available);}catch(...){return fail(d,E::allocation,0);}
    lastRead_=std::move(next);auto& receipt=lastRead_;
    receipt.requested=count;receipt.before=before;receipt.entered=true;
    D readDiagnostic;receipt.returned=read12FB30(destination,count,receipt.done,readDiagnostic);
    receipt.diagnostic=readDiagnostic;
    if(receipt.done>count||receipt.done>receipt.copiedPrefix.size()){readTerminal_=true;return fail(d,E::source,0,receipt.done);}
    if(receipt.done)std::memcpy(receipt.copiedPrefix.data(),destination,receipt.done);
    receipt.copiedPrefix.resize(receipt.done); // Shrink only; no post-IO allocation.
    D cursorDiagnostic;std::size_t after=0;
    receipt.afterKnown=cursor12F520(after,cursorDiagnostic);
    if(receipt.afterKnown){receipt.after=after;logicalCursor=after;}
    // On a failed130560, copied destination/device progress can coexist with
    // unchanged membercursor10. Never synthesize cursor from done or replay it.
    receipt.exact=receipt.returned&&receipt.done==count&&receipt.afterKnown&&after==before+count;
    readTerminal_=!receipt.exact; // Preserve one last partial receipt; no implicit retry.
    if(!receipt.returned){d=readDiagnostic;return false;}
    if(!receipt.afterKnown){d=cursorDiagnostic;return false;}
    if(!receipt.exact)return fail(d,E::source,0x12FB30,receipt.done);
    d=readDiagnostic;return true;
}

bool OriginalStandardTextureResolver50LA::sourcePsgFrameCurrentNative(const OriginalSourcePsgFile50LA& p)const noexcept {
    if(!sourcePsgFoundPredecessorCurrentNative(p))return false;
    if(p.foundPredecessor_&&p.nativeClosed_)
        return p.resolver_.get()==this&&p.root_&&p.thread_==std::this_thread::get_id()&&
            p.searchReached_&&p.modelId_.resolver==this&&p.expected_.identity&&p.expected_.data&&
            p.handle_==-1&&(!p.associationStored_||p.associationWithdrawn_)&&!p.warmLookupBorrowers_&&
            (!p.returnedWarm_||(p.warmLookupWithdrawn_&&!p.warmCreatorFile_&&!p.warmResourceBorrower_));
    //An exact native-closed warm tombstone owns no source bank/creator borrow.
    //It remains cleanup-valid after those owners have legitimately retired.
    if(p.returnedWarm_&&p.nativeClosed_)
        return p.resolver_.get()==this&&p.root_&&p.thread_==std::this_thread::get_id()&&
            p.searchReached_&&p.modelId_.resolver==this&&!p.returnedCold_&&p.handle_==-1&&
            !p.associationStored_&&!p.sourceCloseReturned_&&p.warmLookupWithdrawn_&&
            !p.warmCreatorFile_&&!p.warmResourceBorrower_&&!p.warmLookupBorrowers_;
    if(p.resolver_.get()!=this||!p.root_||p.thread_!=std::this_thread::get_id()||!p.searchReached_||
       !p.expected_.identity||!p.expected_.data||!freshFileCapacity_||!storage_||
       p.modelId_.resolver!=this||p.modelId_.row>=storage_->files.size())return false;
    const auto& s=*freshFileCapacity_;const auto& t=s.track;
    if(t.root!=p.root_.get()||!t.joined||!s.members150||t.mount!=p.mount_||t.generation!=p.generation_||
       p.row_< -1||p.row_>=s.memberCount154||(p.returnedCold_&&p.row_<0))return false;
    if(p.returnedWarm_){
        if(p.returnedCold_||p.handle_!=-1||p.associationStored_||p.sourceCloseReturned_)return false;
        if(p.nativeClosed_)return p.warmLookupWithdrawn_&&!p.warmCreatorFile_&&!p.warmResourceBorrower_;
        return sourcePsgWarmCurrent(p,p.warmGeometry_,p.warmCreator_);
    }
    if(p.associationStored_&&!p.associationWithdrawn_&&
       !sourcePsgGeometryCurrent(p,p.associatedGeometry_,p.associatedModel_))return false;
    if(p.associationWithdrawn_){
        const auto& file=*storage_->files[p.modelId_.row];
        if(!p.associationStored_||p.row_<0||s.members150[p.row_].associatedObject1C||
           file.geometryIdentity||!file.geometry.expired()||!file.payloadGeometry.expired())return false;
    }
    if(p.nativeClosed_)return p.handle_==-1&&(!p.associationStored_||p.associationWithdrawn_);
    if(p.sourceCloseReturned_)return p.handle_==-1&&p.sourceCloseResult_==1&&p.row_>=0&&
        !(s.members150[p.row_].flags18&4)&&!s.members150[p.row_].courseConfigOwner;
    if(p.handle_!=-1)return p.handle_==p.row_&&sourceBankMemberCurrent(p.root_.get(),&p,p.handle_);
    // Failed source search/warm refusal has no open interval. Do not confuse
    // a missing returnedCold flag with loss of a partially committed131380.
    for(std::int32_t n=0;n<s.memberCount154;++n)if(s.members150[n].courseConfigOwner==&p)return false;
    return !p.returnedCold_;
}
bool OriginalSourcePsgFile50LA::preflightNative()const noexcept {
    return resolver_&&resolver_->sourcePsgFrameCurrentNative(*this);
}

bool OriginalStandardTextureResolver50LA::sourcePsgCursorNative(const OriginalSourcePsgFile50LA& p,
    std::size_t& out,D& d)const noexcept {
    d={};if(!sourcePsgFileCurrent(p))return fail(d,E::phase,0);
    out=freshFileCapacity_->members150[p.handle_].cursor10;
    return true; // Native owner observation, no added source12F520 call/guards.
}

bool OriginalSourcePsgFile50LA::associateGeometry1326A0(
    const std::shared_ptr<OriginalStandardModelGeometry50LA>& geometry,
    const rr::game::OriginalRacerModelSceneOwner50LA* model,D& d)noexcept {
    d={};return resolver_?resolver_->associateSourcePsgGeometry1326A0(*this,geometry,model,d):fail(d,E::phase,0x1326A0);
}
bool OriginalStandardTextureResolver50LA::associateSourcePsgGeometry1326A0(
    OriginalSourcePsgFile50LA& p,const std::shared_ptr<OriginalStandardModelGeometry50LA>& geometry,
    const rr::game::OriginalRacerModelSceneOwner50LA* model,D& d)noexcept {
    d={};if(!sourcePsgFileCurrent(p)||p.associationStored_||!geometry||!model||
        !geometry->sourcePrototypeCurrent(model,&p))return fail(d,E::phase,0x1326A0);
    auto& s=*freshFileCapacity_;auto& guards=fileIoAssertions();
    if(!guards.cache827C_3E1BA8.reach(p.handle_>=0,d,0x1326F8)||
       !guards.cache8280_3E1BB0.reach(p.handle_<s.memberCount154,d,0x132764))return false;
    auto& row=s.members150[p.handle_];auto& file=*storage_->files[p.modelId_.row];
    // Initial cold frame proves these empty. No generic native publisher is
    // opened for raw source Geometry; this is the reached source store itself.
    if(row.associatedObject1C||file.geometryIdentity||!file.geometry.expired()||
       !file.payloadGeometry.expired())return fail(d,E::phase,0x1327B0);
    row.associatedObject1C=geometry.get();p.associatedGeometry_=geometry.get();
    p.associatedModel_=model;p.associationStored_=true;d.sourceEffectsCommitted=true;
    // weak_ptr assignments/reset cannot allocate or retain a strong owner.
    // No fallible crosswalk remains after the actual row1C publication.
    file.finalGeometryRelease={};file.geometry=geometry;file.payloadGeometry=geometry;
    file.geometryIdentity=geometry.get();d.sourcePC=0x1327B0;return true;
}
bool OriginalStandardTextureResolver50LA::sourcePsgGeometryCurrent(const OriginalSourcePsgFile50LA& p,
    const OriginalStandardModelGeometry50LA* geometry,const rr::game::OriginalRacerModelSceneOwner50LA* model)const noexcept {
    if(p.resolver_.get()!=this||p.thread_!=std::this_thread::get_id()||!p.root_||
       !p.associationStored_||p.associationWithdrawn_||!geometry||!model||
       p.associatedGeometry_!=geometry||p.associatedModel_!=model||!freshFileCapacity_||!storage_||
       p.modelId_.resolver!=this||p.modelId_.row>=storage_->files.size())return false;
    const auto& s=*freshFileCapacity_;const auto& t=s.track;
    if(t.root!=p.root_.get()||!t.joined||t.mount!=p.mount_||t.generation!=p.generation_||
       !s.members150||p.row_<0||p.row_>=s.memberCount154)return false;
    const auto& row=s.members150[p.row_];const auto& file=*storage_->files[p.modelId_.row];
    return row.originalMember==p.expected_.identity&&row.associatedObject1C==geometry&&
        file.geometryIdentity==geometry&&file.geometry.lock().get()==geometry&&
        file.payloadGeometry.lock().get()==geometry&&geometry->sourcePrototypeIdentityCurrent(model,&p);
}
bool OriginalSourcePsgFile50LA::associatedGeometryCurrent(const OriginalStandardModelGeometry50LA* geometry,
    const rr::game::OriginalRacerModelSceneOwner50LA* model)const noexcept {
    return resolver_&&resolver_->sourcePsgGeometryCurrent(*this,geometry,model);
}
bool OriginalStandardTextureResolver50LA::withdrawSourcePsgGeometryNative(OriginalSourcePsgFile50LA& p,
    const OriginalStandardModelGeometry50LA* geometry,const rr::game::OriginalRacerModelSceneOwner50LA* model,D& d)noexcept {
    d={};if(p.resolver_.get()!=this||p.thread_!=std::this_thread::get_id()||!geometry||!model||
       p.associatedGeometry_!=geometry||p.associatedModel_!=model||!p.associationStored_)
        return fail(d,E::phase,0);
    if(p.associationWithdrawn_)return true; // Identity history only, never dereference freed owner.
    if(p.warmLookupBorrowers_||!sourcePsgGeometryCurrent(p,geometry,model)||!p.sourceGeometryWithdrawalAllowedNative(model))return fail(d,E::phase,0);
    auto& file=*storage_->files[p.modelId_.row];
    freshFileCapacity_->members150[p.row_].associatedObject1C=nullptr;
    file.geometry.reset();file.payloadGeometry.reset();file.geometryIdentity=nullptr;
    p.associationWithdrawn_=true;return true; // Native cancellation, not a1327D0 call receipt.
}
bool OriginalSourcePsgFile50LA::withdrawGeometryNative(const OriginalStandardModelGeometry50LA* geometry,
    const rr::game::OriginalRacerModelSceneOwner50LA* model,D& d)noexcept {
    d={};return resolver_?resolver_->withdrawSourcePsgGeometryNative(*this,geometry,model,d):fail(d,E::phase,0);
}
bool OriginalSourcePsgFile50LA::closeSource131040(D& d)noexcept {
    d={};if(sourceCloseReturned_)return fail(d,E::phase,0x131040); // No replay.
    if(!current()||!associationStored_||associationWithdrawn_||readTerminal_)
        return fail(d,E::phase,0x131040);
    // This authenticated bank domain executes the actual open-row archive
    // branch, whose source return is1. No12F290 flush and no loose retirement.
    if(!resolver_->closeSourceBankMember131040(root_.get(),this,handle_,false,d))return false;
    handle_=-1;sourceCloseResult_=1;sourceCloseReturned_=true;d.sourcePC=0x131228;return true;
}

bool OriginalSourcePsgFile50LA::captureWarm132820(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalRaTrackSourceFile50LA> root,const std::string& name,
    OriginalStandardModelFileId50LA id,std::shared_ptr<OriginalSourcePsgFile50LA> creatorFile,
    const OriginalStandardModelGeometry50LA* geometry,const rr::game::OriginalRacerModelSceneOwner50LA* creator,
    std::shared_ptr<OriginalSourcePsgFile50LA>& pending,D& d)noexcept {
    d={};if(!resolver||!root||pending||!creatorFile||!geometry||!creator)return fail(d,E::phase,0x132820);
    return resolver->captureSourcePsg132820(std::move(root),name,id,pending,d,std::move(creatorFile),geometry,creator);
}
bool OriginalStandardTextureResolver50LA::sourcePsgWarmCurrent(const OriginalSourcePsgFile50LA& p,
    const OriginalStandardModelGeometry50LA* geometry,const rr::game::OriginalRacerModelSceneOwner50LA* creator)const noexcept {
    if(!sourcePsgFoundPredecessorCurrentNative(p))return false;
    if(p.resolver_.get()!=this||p.thread_!=std::this_thread::get_id()||!p.returnedWarm_||p.returnedCold_||
       p.nativeClosed_||p.warmLookupWithdrawn_||p.handle_!=-1||p.associationStored_||p.sourceCloseReturned_||
       !geometry||!creator||p.warmGeometry_!=geometry||p.warmCreator_!=creator||!p.warmCreatorFile_||
       p.warmCreatorFile_.get()==&p||!freshFileCapacity_||!storage_)return false;
    const auto& original=*p.warmCreatorFile_;
    if(!original.warmLookupBorrowers_||original.nativeClosed_||!original.sourceCloseReturned_||
       original.root_!=p.root_||original.row_!=p.row_||original.modelId_.resolver!=p.modelId_.resolver||
       original.modelId_.row!=p.modelId_.row||original.expected_.identity!=p.expected_.identity||
       original.mount_!=p.mount_||original.generation_!=p.generation_)
        return false;
    // Same actual Fresh row and native weak crosswalk, no native cache-ready
    // promotion and no source reference increment by lookup/currentness.
    return sourcePsgGeometryCurrent(original,geometry,creator);
}
bool OriginalSourcePsgFile50LA::warmGeometryCurrent(const OriginalStandardModelGeometry50LA* geometry,
    const rr::game::OriginalRacerModelSceneOwner50LA* creator)const noexcept {
    return resolver_&&resolver_->sourcePsgWarmCurrent(*this,geometry,creator);
}

bool OriginalSourcePsgFile50LA::continueFound132988(
    std::shared_ptr<OriginalSourcePsgMissingLookup50LA> previous,OriginalStandardModelFileId50LA id,
    std::shared_ptr<OriginalSourcePsgFile50LA>& pending,D& d,
    std::shared_ptr<OriginalSourcePsgFile50LA> creatorFile,const OriginalStandardModelGeometry50LA* geometry,
    const rr::game::OriginalRacerModelSceneOwner50LA* creator)noexcept {
    d={};if(!previous||!previous->resolver_||pending)return fail(d,E::phase,0x132988);
    auto resolver=previous->resolver_;
    return resolver->continueSourcePsgFound132988(std::move(previous),id,pending,d,
        std::move(creatorFile),geometry,creator);
}
bool OriginalStandardTextureResolver50LA::continueSourcePsgFound132988(
    std::shared_ptr<OriginalSourcePsgMissingLookup50LA> previous,OriginalStandardModelFileId50LA id,
    std::shared_ptr<OriginalSourcePsgFile50LA>& pending,D& d,
    std::shared_ptr<OriginalSourcePsgFile50LA> creatorFile,const OriginalStandardModelGeometry50LA* geometry,
    const rr::game::OriginalRacerModelSceneOwner50LA* creator)noexcept try {
    d={};if(!previous||pending||!sourcePsgMissingCurrentNative(*previous)||previous->nativeClosed_||
       !previous->found_||previous->sourcePC_!=0x132988||previous->returnV0_||previous->observed170_||
       previous->continuationAttempted_||previous->fileBorrower_||previous->collisionSearchBorrower_||!freshFileCapacity_||!storage_||
       id.resolver!=this||id.row>=storage_->files.size())return fail(d,E::phase,0x132988);
    auto& q=*previous;auto& s=*freshFileCapacity_;const auto& t=s.track;
    if(!s.members150||s.memberCount154!=q.memberCount_||s.cursor160!=q.after160_||s.cursor168!=q.after168_||
       s.cursor160<0||s.cursor160>=s.memberCount154||!q.root_->provider_||!q.root_->archive_)
        return fail(d,E::phase,0x132988);
    auto& row=s.members150[static_cast<std::int32_t>(s.cursor160)];auto& file=*storage_->files[id.row];
    if(row.originalMember!=q.foundMemberIdentity_||row.filename08!=q.foundFilenameIdentity_||
       !row.filename08||!courseSourceName308F00(q.name_,row.filename08)||file.retired||
       file.kind!=Storage::FileKind::psg||!courseSourceName308F00(q.name_,file.member.c_str()))
        return fail(d,E::source,0x132988);
    OriginalStandardArchiveProvider50LA::MemberView winner;
    if(q.root_->provider_->lookup132820(q.name_,winner)!=OriginalStandardArchiveProvider50LA::Lookup::present||
       !winner.member||!winner.data||winner.mount!=t.mount||winner.bankGeneration!=t.generation||
       winner.revision!=q.root_->provider_->revision()||winner.size!=file.bytes.size()||
       std::memcmp(winner.data,file.bytes.data(),winner.size))return fail(d,E::source,0x132988);
    const auto expected=q.root_->archive_->member(winner.ordinal);
    if(!expected.identity||!expected.data||expected.size!=winner.size||row.originalMember!=expected.identity||
       row.deviceID00!=t.device||row.length14!=expected.size||!row.canonicalData||
       !courseSourceName308F00(q.name_,expected.identity->name)||
       std::memcmp(expected.data,winner.data,winner.size)||std::memcmp(row.canonicalData,winner.data,winner.size))
        return fail(d,E::source,0x132988);
    //Fallible native admission is before one-shot continuation publication.
    //No new132820 search, shared160/168 write, or source allocation is issued.
    std::string retainedName=q.name_;
    auto attempt=std::shared_ptr<OriginalSourcePsgFile50LA>(new OriginalSourcePsgFile50LA);
    auto& p=*attempt;p.resolver_=q.resolver_;p.root_=q.root_;p.modelId_=id;
    p.expected_=expected;p.name_=std::move(retainedName);p.mount_=winner.mount;p.ordinal_=winner.ordinal;
    p.generation_=winner.bankGeneration;p.revision_=winner.revision;p.row_=static_cast<std::int32_t>(q.after160_);
    p.searchReached_=true;p.foundPredecessor_=std::move(previous);
    q.continuationAttempted_=true;q.fileBorrower_=&p;pending=std::move(attempt);
    //From this point every refusal/partialopen retains exact file token until
    //native file withdrawal; closing the old found attempt is now fenced.
    if(row.associatedObject1C){
        if(!creatorFile||creatorFile.get()==&p||!geometry||!creator||
           creatorFile->resolver_.get()!=this||creatorFile->root_!=p.root_||creatorFile->row_!=p.row_||
           creatorFile->modelId_.resolver!=id.resolver||creatorFile->modelId_.row!=id.row||
           row.associatedObject1C!=geometry||!creatorFile->sourceCloseReturned_||creatorFile->nativeClosed_||
           !sourcePsgGeometryCurrent(*creatorFile,geometry,creator)||
           creatorFile->warmLookupBorrowers_==std::numeric_limits<std::size_t>::max())
            return fail(d,E::unsupported,0x1329a0);
        p.warmCreatorFile_=std::move(creatorFile);p.warmGeometry_=geometry;p.warmCreator_=creator;
        ++p.warmCreatorFile_->warmLookupBorrowers_; //Native custody; no104960 retain here.
        p.returnedWarm_=true;d.sourcePC=0x1329e8;return true;
    }
    if(creatorFile||geometry||creator||file.geometryIdentity||!file.geometry.expired()||!file.payloadGeometry.expired())
        return fail(d,E::unsupported,0x1329a0);
    if(!openSourceBankMember131380(p.root_.get(),&p,p.row_,p.handle_,d))return false;
    p.returnedCold_=true;d.sourcePC=0x1329e8;return true;
}catch(...){return fail(d,E::allocation,0x132988);}
bool OriginalStandardTextureResolver50LA::sourcePsgFoundPredecessorCurrentNative(
    const OriginalSourcePsgFile50LA& p)const noexcept {
    if(!p.foundPredecessor_)return !p.foundPredecessorWithdrawn_;
    const auto& q=*p.foundPredecessor_;
    if(q.resolver_.get()!=this||q.root_!=p.root_||q.name_!=p.name_||!q.found_||
       !q.continuationAttempted_||q.collisionSearchBorrower_||q.sourcePC_!=0x132988||q.returnV0_||q.observed170_||
       q.after160_!=p.row_||q.foundMemberIdentity_!=p.expected_.identity||
       q.mount_!=p.mount_||q.generation_!=p.generation_)return false;
    if(p.nativeClosed_)return p.foundPredecessorWithdrawn_&&!q.fileBorrower_;
    return !p.foundPredecessorWithdrawn_&&q.fileBorrower_==&p&&!q.nativeClosed_&&
        sourcePsgMissingCurrentNative(q);
}
bool OriginalStandardTextureResolver50LA::withdrawSourcePsgFoundNative(OriginalSourcePsgFile50LA& p,D& d)noexcept {
    if(!p.foundPredecessor_)return true;
    auto& q=*p.foundPredecessor_;
    //closeNative already validated all dependencies before releasing its
    //handle/warm borrow. These exact-token stores cannot fail after that.
    if(p.foundPredecessorWithdrawn_||q.fileBorrower_!=&p||!q.continuationAttempted_||q.nativeClosed_||q.collisionSearchBorrower_)
        return fail(d,E::phase,0);
    q.fileBorrower_=nullptr;p.foundPredecessorWithdrawn_=true;return true;
}
