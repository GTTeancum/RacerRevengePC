// Included inside rr::assets after the actual Fresh FileManager definition.
bool OriginalSourceWaterExists131A40::preflightHistory()const noexcept {
    if(!entered_)return !resolverIdentity_&&!trackIdentity_&&!trackGeneration_&&name_.empty()&&
        !searchReturned_&&!found_&&!rowsCompared_&&!mode170_&&!returnV0_&&pc_==0x131a40u;
    if(!resolverIdentity_||!trackIdentity_||name_.find('\0')!=std::string::npos||
       !searchReturned_||memberCount_<=0||before168_<-1||before168_>=memberCount_||
       after168_<0||after168_>=memberCount_||!rowsCompared_||rowsCompared_>std::uint32_t(memberCount_)||
       before160_!=after160_)return false;
    const auto start=before168_+1==memberCount_?0:before168_+1;
    const auto matched=(start+std::int64_t(rowsCompared_)-1)%memberCount_;
    if(found_)return !mode170_&&returnV0_==1u&&pc_==0x131b60u&&after168_==matched;
    if(rowsCompared_!=std::uint32_t(memberCount_)||after168_!=start||!mode170_)return false;
    if(*mode170_)return !returnV0_&&pc_==0x131b10u; //Before real loose fopen13C980.
    return returnV0_==0u&&pc_==0x131b60u;
}
bool OriginalStandardTextureResolver50LA::captureSourceWaterExists131A40(
    const std::shared_ptr<OriginalRaTrackSourceFile50LA>& root,const std::string& name,
    OriginalSourceWaterExists131A40& p,D& d)noexcept {
    d={};if(p.entered_||!p.preflightHistory()||p.thread_!=std::this_thread::get_id()||!root||
       root->resolver_.get()!=this||root->nativeClosed_||!root->joined_||
       root->thread_!=std::this_thread::get_id()||!freshFileCapacity_||name.find('\0')!=std::string::npos)
        return fail(d,E::phase,0x131a40);
    auto& s=*freshFileCapacity_;const auto& track=s.track;
    if(track.root!=root.get()||!track.joined||!s.members150||s.memberCount154<=0||
       s.cursor168< -1||s.cursor168>=s.memberCount154)
        return fail(d,E::phase,0x131a40);
    //Synchronous native custody of the existing Track registration, mirroring
    //the manager's ordinary Track-member admission. This does not search the
    //provider namespace or retain a membership grant after the source return.
    if(!s.provider||s.provider!=root->provider_||!s.provider->sealed()||
       track.mount>=s.provider->mounts_.size()||!s.provider->mounts_[track.mount])
        return fail(d,E::phase,0x131a40);
    const auto& bank=*s.provider->mounts_[track.mount];
    if(!bank.active||bank.generation!=track.generation)return fail(d,E::phase,0x131a40);
    //The shared comparison adapter proves308F00 only for ASCII. Admit the
    //entire possible scan before the first cursor effect. High-byte names
    //remain unsupported, never an authentic not-found result. This includes
    //rows a hit might otherwise skip; it is a conservative native subset.
    const auto ascii=[](std::string_view text)noexcept {
        for(const unsigned char c:text)if(c>=0x80u)return false;return true;
    };
    if(!ascii(name))return fail(d,E::unsupported,0x131a40);
    for(std::int32_t i=0;i<s.memberCount154;++i)
        if(s.members150[i].filename08&&!ascii(s.members150[i].filename08))
            return fail(d,E::unsupported,0x131a40);
    //Native bookkeeping admission before ANY source write. Original131A40
    //has no empty-table guard; rejected unsafe storage is NOT return0.
    try{p.name_=name;}catch(...){return fail(d,E::allocation,0x131a40);}
    p.resolverIdentity_=this;p.trackIdentity_=root.get();p.trackGeneration_=track.generation;
    p.memberCount_=s.memberCount154;p.before160_=s.cursor160;p.before168_=s.cursor168;
    p.entered_=true;p.pc_=0x131a54;
    ++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0; //131A60/74.
    auto index=static_cast<std::int32_t>(s.cursor168);const auto start=index;
    do {const auto& row=s.members150[index];++p.rowsCompared_;
        if(row.filename08&&courseSourceName308F00(p.name_,row.filename08)){
            p.found_=true;s.cursor168=index;break;} //131AB4, matched signed index.
        if(++index==s.memberCount154)index=0;
    }while(index!=start);
    p.after168_=s.cursor168;p.after160_=s.cursor160;p.searchReturned_=true;
    d.sourceEffectsCommitted=true;
    if(p.found_){p.returnV0_=1;p.pc_=0x131b60;d.sourcePC=p.pc_;return true;}
    //Miss preserves the initial advanced168 and leaves160 untouched. No
    //provider.lookup132820, fast160 check, open, association or read occurs.
    p.pc_=0x131afc;p.mode170_=s.sourceFileMode170;
    if(*p.mode170_){p.pc_=0x131b10;return fail(d,E::unsupported,p.pc_);}
    p.returnV0_=0;p.pc_=0x131b60;d.sourcePC=p.pc_;return true;
}
