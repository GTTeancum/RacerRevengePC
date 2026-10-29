// Included inside rr::assets AFTER Resolver::Storage/FreshFileCapacity and
// real shared FileManager helpers. No namespace reopening or provider-copy IO.
bool OriginalStandardTextureResolver50LA::beginLoadingFontPstPopulation(
    std::shared_ptr<OriginalLoadingFontSource50LA> root,const void* entry,
    OriginalUiMemberView50KG expected,D& d)noexcept try {
    d={};if(loadingPstFontEntry_||!root||root->resolver_.get()!=this||!root->currentCel(entry)||
       !expected.identity||!expected.data||!expected.size||!root->archive_||!root->provider_)
        return fail(d,E::phase,0x281F14);
    const auto& name=expected.identity->name;
    if(!name||(!courseSourceName308F00(name,"font\\denmrk16.pst")&&!courseSourceName308F00(name,"font\\venus16.pst")))
        return fail(d,E::source,0x281F14);
    const bool denmrk=courseSourceName308F00(name,"font\\denmrk16.pst");
    if(!root->cel_.identity||!courseSourceName308F00(root->cel_.identity->name,
        denmrk?"font\\denmrk16.cel":"font\\venus16.cel"))return fail(d,E::source,0x281F14);
    const auto actual=root->archive_->find(name);OriginalStandardArchiveProvider50LA::MemberView winner;
    if(actual.identity!=expected.identity||actual.data!=expected.data||actual.size!=expected.size||
       root->provider_->lookup132820(name,winner)!=OriginalStandardArchiveProvider50LA::Lookup::present||
       !winner.data||winner.size!=expected.size||std::memcmp(winner.data,expected.data,expected.size))
        return fail(d,E::source,0x281F14);
    loadingPstRoot_=root;loadingPstFontEntry_=entry;loadingPstExpected_=expected;return true;
}catch(...){return fail(d,E::allocation,0x281F14);}
bool OriginalStandardTextureResolver50LA::endLoadingFontPstPopulation(
    const OriginalLoadingFontSource50LA& root,const void* entry,D& d)noexcept {
    d={};if(loadingPstRoot_.lock().get()!=&root||!entry||loadingPstFontEntry_!=entry)
        return fail(d,E::phase,0);
    // Native lexical authority only: does not close any retained source handle.
    loadingPstFontEntry_=nullptr;loadingPstRoot_.reset();loadingPstExpected_={};return true;
}
bool OriginalStandardTextureResolver50LA::captureLoadingPst132820(const std::string& name,
    std::size_t source,std::shared_ptr<OriginalLoadingPstFile50LA>& pending,D& d)noexcept try {
    d={};const auto root=loadingPstRoot_.lock();
    if(pending||!root||!root->currentCel(loadingPstFontEntry_)||!storage_||source>=storage_->entries.size()||
       !freshFileCapacity_||!scratch_||!loadingPstExpected_.identity)
        return fail(d,E::phase,0x132820);
    auto& s=*freshFileCapacity_;auto& file=*storage_->entries[source].file;
    if(!sourceBankFamilyCurrent(root.get(),SourceBankFamily::loadingFonts)||s.memberCount154<=0||
       s.cursor160< -1||s.cursor160>=s.memberCount154||s.cursor168< -1||s.cursor168>=s.memberCount154||
       file.retired||!courseSourceName308F00(name,file.member.c_str())||file.kind!=Storage::FileKind::pst||file.bank!=s.courseMount||file.bankGeneration!=s.courseGeneration||
       !courseSourceName308F00(name,loadingPstExpected_.identity->name)||
       file.bytes.size()!=loadingPstExpected_.size||std::memcmp(file.bytes.data(),loadingPstExpected_.data,file.bytes.size()))
        return fail(d,E::source,0x132820);
    // Native attempt allocation/pending-slot publication precede original IO.
    // This is NOT source Resource allocation24DEE8 and has no mallocNULL claim.
    pending=std::shared_ptr<OriginalLoadingPstFile50LA>(new OriginalLoadingPstFile50LA);
    auto& p=*pending;p.root_=root;p.fontEntry_=loadingPstFontEntry_;p.expected_=loadingPstExpected_;
    p.sourceIndex_=source;p.generation_=s.courseGeneration;p.searchReached_=true;
    bool found=false;
    if(s.cursor160!=-1){const auto& row=s.members150[s.cursor160];
        found=row.filename08&&courseSourceName308F00(name,row.filename08);}
    if(!found){
        ++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0;
        auto index=static_cast<std::int32_t>(s.cursor168);const auto start=index;
        do {const auto& row=s.members150[index];if(row.filename08&&courseSourceName308F00(name,row.filename08)){
                s.cursor168=index;found=true;break;}
            if(++index==s.memberCount154)index=0;
        }while(index!=start);
        // 132938 writes cursor160=-1 on failed circular search.
        s.cursor160=found?s.cursor168:-1;
    }
    d.sourceEffectsCommitted=true;
    if(!found)return fail(d,E::unresolved,0x13294C); // Loose-file fallback remains unsupported.
    p.row_=static_cast<std::int32_t>(s.cursor160);auto& row=s.members150[p.row_];
    if(row.originalMember!=p.expected_.identity||row.length14!=p.expected_.size||row.deviceID00!=s.courseDevice||
       !row.canonicalData||std::memcmp(row.canonicalData,p.expected_.data,p.expected_.size))return fail(d,E::source,0x1329A0);
    auto cached=file.cached.lock();
    if(row.associatedObject1C){
        if(!cached||cached.get()!=row.associatedObject1C||file.cacheIdentity!=cached.get()||!cached->constructed24DA30())
            return fail(d,E::phase,0x1329A0);
        p.hit_=cached;p.hitReturned_=true;p.returned132820_=true;return true; // Actual out handle=-1.
    }
    // An old native weakcache is not an actual SourceFile1C producer.
    if(cached||file.cacheIdentity)return fail(d,E::phase,0x1329D4);
    if(!openSourceBankMember131380(root.get(),&p,p.row_,p.handle_,d))return false;
    p.returned132820_=true;return true;
}catch(...){return fail(d,E::allocation,0x132820);}
bool OriginalStandardTextureResolver50LA::loadingPstFileCurrent(const OriginalLoadingPstFile50LA& p,bool open)const noexcept {
    if(p.raRoot_)return loadingRaFileCurrent(p,open);
    if(!p.root_||p.root_->resolver_.get()!=this||p.root_->thread_!=std::this_thread::get_id()||p.nativeAborted_||
       !p.returned132820_||!freshFileCapacity_||!storage_||p.sourceIndex_>=storage_->entries.size()||
       !sourceBankFamilyCurrent(p.root_.get(),SourceBankFamily::loadingFonts))return false;
    const auto& s=*freshFileCapacity_;
    if(s.courseGeneration!=p.generation_||p.row_<0||p.row_>=s.memberCount154)return false;
    const auto& row=s.members150[p.row_];
    if(row.originalMember!=p.expected_.identity||row.deviceID00!=s.courseDevice||row.length14!=p.expected_.size)return false;
    return !open||(!p.hitReturned_&&!p.closed_&&sourceBankMemberCurrent(p.root_.get(),&p,p.handle_));
}
bool OriginalStandardTextureResolver50LA::bindLoadingPstResource(OriginalLoadingPstFile50LA& p,
    OriginalStandardTextureResource50LA& resource,D& d)noexcept {
    d={};if(!loadingPstFileCurrent(p,true)||p.boundResource_||p.associated_||p.hitReturned_)
        return fail(d,E::phase,0x24DEE8);
    // Caller supplies actual allocated object before its source constructor.
    p.boundResource_=&resource;return true;
}
bool OriginalStandardTextureResolver50LA::readLoadingPst12FB30(OriginalLoadingPstFile50LA& p,
    void* output,std::size_t count,std::size_t& done,D& d)noexcept {
    done=0;d={};if(!loadingPstFileCurrent(p,true)||!p.boundResource_)return fail(d,E::phase,0x12FB30);
    if(p.raRoot_){
        if(!readAssertions12FB30(p.handle_,freshFileCapacity_->memberCount154,freshFileCapacity_->members150[p.handle_].flags18,d))return false;
        return readLoadingRa130560(p,output,count,done,d);
    }
    return readSourceBankMember12FB30(p.root_.get(),&p,p.handle_,output,count,done,d);
}
bool OriginalStandardTextureResolver50LA::readLoadingPst12FF70(OriginalLoadingPstFile50LA& p,
    void* output,std::size_t count,std::size_t& done,D& d)noexcept {
    done=0;d={};if(!loadingPstFileCurrent(p,true)||!p.boundResource_)return fail(d,E::phase,0x12FF70);
    auto& s=*freshFileCapacity_;auto& g=fileIoAssertions();
    if(!g.read825C_3E1B68.reach(p.handle_>=0,d,0x12FFD8)||
       !g.read8260_3E1B70.reach(p.handle_<s.memberCount154,d,0x130040)||
       !g.read8264_3E1B78.reach(s.members150[p.handle_].flags18&4,d,0x1300BC))return false;
    if(p.raRoot_)return readLoadingRa130560(p,output,count,done,d);
    return readSourceBankMember130560(p.root_.get(),&p,p.handle_,output,count,done,true,d);
}
bool OriginalStandardTextureResolver50LA::associateLoadingPst1326A0(OriginalLoadingPstFile50LA& p,
    const std::shared_ptr<OriginalStandardTextureResource50LA>& resource,D& d)noexcept {
    d={};if(!loadingPstFileCurrent(p,true)||p.associated_||!resource||resource.get()!=p.boundResource_||
       !resource->constructed24DA30()||resource->resolver_.get()!=this||resource->sourceIndex_!=p.sourceIndex_)
        return fail(d,E::phase,0x1326A0);
    auto& s=*freshFileCapacity_;auto& g=fileIoAssertions();
    if(!g.cache827C_3E1BA8.reach(p.handle_>=0,d,0x1326F8)||
       !g.cache8280_3E1BB0.reach(p.handle_<s.memberCount154,d,0x132764))return false;
    auto& row=s.members150[p.handle_];auto& file=*storage_->entries[p.sourceIndex_].file;
    if(row.associatedObject1C||file.cacheIdentity||!file.cached.expired())return fail(d,E::phase,0x1327B0);
    row.associatedObject1C=resource.get();p.associated_=true;d.sourceEffectsCommitted=true;
    // Same actual pointer is now recorded in native weak-cache backing; no ref increment.
    file.cached=resource;file.cacheIdentity=resource.get();return true;
}
bool OriginalStandardTextureResolver50LA::closeLoadingPst131040(OriginalLoadingPstFile50LA& p,bool nativeAbort,D& d)noexcept {
    if(p.raRoot_)return closeLoadingRa131040(p,nativeAbort,d);
    d={};if(!p.root_||p.root_->resolver_.get()!=this||p.root_->thread_!=std::this_thread::get_id())return fail(d,E::phase,0x131040);
    if(p.closed_||p.nativeAborted_)return true;
    if(p.handle_!=-1){
        if(!closeSourceBankMember131040(p.root_.get(),&p,p.handle_,false,d))return false;
        p.handle_=-1;
    }else if(!nativeAbort&&!p.hitReturned_)return fail(d,E::phase,0x131040);
    // Native abort may close a retained partial-open interval. It does not
    // certify that original24E880 returned or24E0A4 was reached.
    if(nativeAbort)p.nativeAborted_=true;else p.closed_=true;return true;
}
bool OriginalStandardTextureResolver50LA::canClearLoadingPst1327D0(const OriginalLoadingPstFile50LA& p,
    const OriginalStandardTextureResource50LA& resource)const noexcept {
    if(p.raRoot_){
        const auto& owner=p.raRoot_;
        if(p.root_||owner->resolver_.get()!=this||p.boundResource_!=&resource||!p.associated_||
           owner->thread_!=std::this_thread::get_id())return false;
        if(p.associationCleared_)return true;
        if(owner->nativeClosed_||!freshFileCapacity_||freshFileCapacity_->loadingRaOwner!=owner.get()||
           !freshFileCapacity_->members150)return false;
        const auto& state=*freshFileCapacity_;
        return p.row_==owner->member_&&p.row_>=0&&p.row_<state.memberCount154&&
            state.members150[p.row_].loadingRaOwner==owner.get()&&state.members150[p.row_].associatedObject1C==&resource;
    }

    if(!p.root_||p.root_->resolver_.get()!=this||p.boundResource_!=&resource||
       p.root_->thread_!=std::this_thread::get_id()||!p.associated_)return false;
    if(p.associationCleared_)return true;
    if(p.root_->sourceRetired_)return p.sourceBankRetiredAssociation_;
    if(!freshFileCapacity_||!sourceBankFamilyCurrent(p.root_.get(),SourceBankFamily::loadingFonts)||
       freshFileCapacity_->courseGeneration!=p.generation_||!freshFileCapacity_->members150)return false;
    const auto& s=*freshFileCapacity_;
    for(std::int32_t i=0;i<s.memberCount154;++i)if(s.members150[i].associatedObject1C==&resource)return true;
    return false;
}
bool OriginalStandardTextureResolver50LA::clearLoadingPst1327D0(OriginalLoadingPstFile50LA& p,
    const OriginalStandardTextureResource50LA& resource,D& d)noexcept {
    if(p.raRoot_){
        d={};if(!canClearLoadingPst1327D0(p,resource))return fail(d,E::phase,0x1327D0);
        if(!p.associationCleared_){freshFileCapacity_->members150[p.row_].associatedObject1C=nullptr;
            p.associationCleared_=true;d.sourceEffectsCommitted=true;}
        return true; // Source cache clear alone does NOT retire closed loose registration.
    }

    d={};if(!p.root_||p.root_->resolver_.get()!=this||p.boundResource_!=&resource||
       p.root_->thread_!=std::this_thread::get_id()||!p.associated_)return fail(d,E::phase,0x1327D0);
    if(p.associationCleared_)return true;
    // A genuine forced12EAA0 retirement already cleared this old bank's1C.
    // Never search a replacement bank as if it were this resource's old owner.
    if(p.root_->sourceRetired_){
        if(!p.sourceBankRetiredAssociation_)return fail(d,E::phase,0x1327D0);
        return true; // Exact earlier force1 tombstone, no new1327D0 search/store.
    }
    if(!freshFileCapacity_||!sourceBankFamilyCurrent(p.root_.get(),SourceBankFamily::loadingFonts)||
       freshFileCapacity_->courseGeneration!=p.generation_)return fail(d,E::phase,0x1327D0);
    auto& s=*freshFileCapacity_;
    for(std::int32_t i=0;i<s.memberCount154;++i)if(s.members150[i].associatedObject1C==&resource){
        s.members150[i].associatedObject1C=nullptr;p.associationCleared_=true;d.sourceEffectsCommitted=true;return true;}
    return fail(d,E::phase,0x1327D0);
}
