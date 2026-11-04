// Included inside rr::assets after the complete resolver and FileManager helpers.
OriginalStreamLibraryFile50LA::OriginalStreamLibraryFile50LA(
    const original_main_entry50KG::Application* issuer,const rr::platform::OriginalStreamLibrarySource50LA* loader)noexcept
    :issuer_(issuer),loader_(loader){}
OriginalStreamLibraryFile50LA::~OriginalStreamLibraryFile50LA()noexcept {
    if(acquired_&&!nativeClosed_)std::terminate();
}
bool OriginalStandardTextureResolver50LA::streamLibraryRootCurrent(const void* root)const noexcept {
    if(!root||!freshFileCapacity_||!scratch_)return false;
    const auto& s=*freshFileCapacity_;const auto* o=s.streamLibraryOwner;
    if(o!=root||!o->issuer_||!o->loader_||!o->acquired_||o->nativeClosed_||
       o->thread_!=std::this_thread::get_id()||o->resolver_.get()!=this||
       o->provider_!=s.provider.get()||!s.provider||o->archive_!=s.appStart||!s.appStart||
       s.registration!=FreshFileCapacity::Registration::complete||!s.members150||s.memberCount154<6||
       !s.devices14||s.deviceCount18<1||o->mount_>=s.provider->mounts_.size())return false;
    const auto& bank=*s.provider->mounts_[o->mount_];const auto& device=s.devices14[0];
    if(!bank.active||bank.generation!=o->generation_||bank.ui!=s.appStart||
       device.id00!=0||device.flags0C!=0x342||!device.name04||device.length14!=58977||
       device.payload18!=283||device.tableOffset1C!=20||device.tableBytes20!=64)return false;
    for(std::int32_t n=3;n<=5;++n){const auto& row=s.members150[n];const auto actual=s.appStart->member(n);
        if(!actual.identity||row.originalMember!=actual.identity||row.deviceID00||!row.filename08||
           !row.canonicalName||!row.canonicalData||row.length14!=actual.size||
           row.logicalStart0C!=(n==3?122880u:n==4?147456u:221184u)||
           (n==3&&row.length14!=7542u)||row.flags18&1)return false;
        // Authenticate canonical ownership only; never use it as read output.
        if(bank.rows.size()<=std::size_t(n)||&bank.rows[n].name!=row.canonicalName||
           bank.rows[n].data!=row.canonicalData||bank.rows[n].size!=row.length14)return false;
    }
    return true;
}
bool OriginalStreamLibraryFile50LA::begin131230(std::shared_ptr<R> resolver,const void* occurrence,
    Kind kind,std::int64_t& returned,D& d)noexcept try {
    d={};using C=R::FreshFileCapacity;
    const bool audio=kind==Kind::audioDirectory,parameters=kind==Kind::streamParameters;
    if(!audio&&!parameters&&kind!=Kind::streamNames)return fail(d,E::phase,0x131230);
    const std::int32_t expected=audio?3:parameters?5:4;
    //298640..66C supplies the lowercased source path to131230.
    const char* name=audio?"appstart\\audiostr.txt":parameters?"audio\\streams.juk":"audio\\streams.txt";
    if(!issuer_||!loader_||!occurrence||!resolver||thread_!=std::this_thread::get_id()||nativeClosed_||blocked_||
       (parameters?stage_!=Stage::txtClosed:stage_!=Stage::fresh)||!resolver->freshFileCapacity_||!resolver->scratch_)
        return fail(d,E::phase,0x131230);
    auto& s=*resolver->freshFileCapacity_;
    if(!s.provider||!s.appStart||!s.members150||s.memberCount154<6||!s.devices14||s.deviceCount18<1||
       s.registration!=C::Registration::complete||!s.namesReadAttempted||
       s.cursor168< -1||s.cursor168>=s.memberCount154||s.failedCourseDevices||s.failedCourseMembers||
       s.musicOwner||s.loadingRaOwner||s.courseRootOwner||s.track.root||s.pod.root)
        return fail(d,E::phase,0x131230);
    if(!acquired_){
        if(s.streamLibraryOwner)return fail(d,E::phase,0x131230);
        // Resolve metadata only to authenticate the already-registered bank.
        OriginalStandardArchiveProvider50LA::MemberView view;
        if(s.provider->lookup132820(name,view)!=OriginalStandardArchiveProvider50LA::Lookup::present||
           view.member!=s.members150[expected].canonicalName||view.data!=s.members150[expected].canonicalData)
            return fail(d,E::source,0x131230);
        resolver_=std::move(resolver);archive_=s.appStart;provider_=s.provider.get();
        mount_=view.mount;generation_=view.bankGeneration;acquired_=true;s.streamLibraryOwner=this;
    }else if(resolver_.get()!=resolver.get()||s.streamLibraryOwner!=this)return fail(d,E::phase,0x131230);
    if(!resolver_->streamLibraryRootCurrent(this)){blocked_=true;return fail(d,E::phase,0x131230);}
    occurrence_=occurrence;stage_=audio?Stage::audioAttempted:parameters?Stage::jukAttempted:Stage::txtAttempted;winner_=-1;
    ++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0;
    auto n=static_cast<std::int32_t>(s.cursor168);const auto start=n;
    do {if(s.members150[n].filename08&&courseSourceName308F00(name,s.members150[n].filename08)){
            s.cursor168=n;winner_=n;break;}
        if(++n==s.memberCount154)n=0;
    }while(n!=start);
    d.sourceEffectsCommitted=true;
    // Never skip a foreign winner or use a loose-device fallback. A missing
    // expected registered member is a native custody bound, not source -1.
    if(winner_!=expected){blocked_=true;return fail(d,E::unsupported,winner_==-1?0x131318:0x131354);}
    if(!resolver_->openExactSourceMember131380(occurrence_,winner_,handle_,d)){blocked_=true;return false;}
    stage_=audio?Stage::audioOpen:parameters?Stage::jukOpen:Stage::txtOpen;returned=handle_;return true;
}catch(...){if(acquired_)blocked_=true;return fail(d,E::allocation,0);}
bool OriginalStreamLibraryFile50LA::current(const void* occurrence,std::int64_t handle)const noexcept {
    return acquired_&&!nativeClosed_&&thread_==std::this_thread::get_id()&&resolver_&&
        occurrence&&occurrence==occurrence_&&handle==handle_&&handle_>=0&&
        resolver_->sourceBankMemberCurrent(this,occurrence_,handle_);
}
bool OriginalStreamLibraryFile50LA::status12EC90(const void* occurrence,std::int64_t handle,
    std::uint64_t& modified,std::uint64_t& size,D& d)noexcept {
    d={};if(blocked_||flushAttempted_||!current(occurrence,handle))return fail(d,E::phase,0x12EC90);
    auto& g=fileIoAssertions();d.sourceEffectsCommitted=true;
    if(!g.status8218_3E1AE0.reach(handle_>=0,d,0x12ECF0)||
       !g.status821C_3E1AE8.reach(handle_<resolver_->freshFileCapacity_->memberCount154,d,0x12ED5C)){
        blocked_=true;return false;
    }
    using G=OriginalConfigProcessGlobals50LA;
    const G::StreamSprintfAccess key{};std::uint32_t pc=0x1336c0;
    if(!G::initializeStreamStatusWarning1336C0(key,pc)){
        blocked_=true;return fail(d,E::phase,pc);
    }
    // Exact389670 fixed literal:1336C0 forwards va_list to30D920(vprintf).
    // Native stdout is diagnostic transport only, not emulated PS2 stdio.
    // Original caller ignores the logging return, including output errors.
    (void)std::fputs("WARNING: GetStatus 'last modify time' not implemented!\n",stdout);
    G::streamNativeDiagnosticBoundary(key);
    modified=0; //12EDBC, only after the diagnostic call returns.
    size=resolver_->freshFileCapacity_->members150[handle_].length14; //12EDDC zero extension.
    return true;
}
bool OriginalStreamLibraryFile50LA::read12FB30(const void* occurrence,std::int64_t handle,void* output,
    std::size_t requested,std::size_t& copied,D& d)noexcept {
    d={};copied=0;if(blocked_||flushAttempted_||!current(occurrence,handle))return fail(d,E::phase,0x12FB30);
    const bool returned=resolver_->readSourceBankMember12FB30(this,occurrence_,handle_,output,requested,copied,d);
    d.sourceEffectsCommitted=true;if(!returned)blocked_=true;return returned;
}
bool OriginalStreamLibraryFile50LA::seekSet12F640(const void* occurrence,std::int64_t handle,std::size_t position,D& d)noexcept {
    d={};if(blocked_||flushAttempted_||!current(occurrence,handle))return fail(d,E::phase,0x12F640);
    const bool returned=resolver_->seekSourceBankMember12F640(this,occurrence_,handle_,position,d);
    d.sourceEffectsCommitted=true;if(!returned)blocked_=true;return returned;
}
bool OriginalStreamLibraryFile50LA::flush12F290(const void* occurrence,std::int64_t handle,
    std::int32_t& returned,D& d)noexcept {
    d={};if(blocked_||flushAttempted_||stage_!=Stage::jukOpen||!current(occurrence,handle))
        return fail(d,E::phase,0x12f290);
    flushAttempted_=true;auto& s=*resolver_->freshFileCapacity_;
    d.sourceEffectsCommitted=true;
    if(!fileAssertions12F290(handle_,s.memberCount154,d)){blocked_=true;return false;}
    const auto deviceID=s.members150[handle_].deviceID00;
    if(!s.devices14||deviceID<0||deviceID>=s.deviceCount18){blocked_=true;return fail(d,E::phase,0x12f3ac);}
    //12F3AC obtains the real retained device handle. A nonnull handle calls
    //13D9E0, exactly JR RA with a zero-return delay slot; null also returns0.
    //No host flush, callback, codec/window change or device retirement occurs.
    flushHandleCall_=s.devices14[deviceID].handle10!=nullptr;
    flushReturned_=true;returned=0;return true;
}
bool OriginalStreamLibraryFile50LA::close131040(const void* occurrence,std::int64_t handle,bool flush,D& d)noexcept {
    d={};if(blocked_||!current(occurrence,handle)||(stage_!=Stage::audioOpen&&stage_!=Stage::txtOpen&&stage_!=Stage::jukOpen)||(flush&&stage_!=Stage::jukOpen))return fail(d,E::phase,0x131040);
    // Audio/TXT298714 and JUK157970/1579B4 failure closes never flush.
    // Config destruction separately returns12F290, then closes with false.
    // The combined private option remains available without replaying flush.
    if(flush){std::int32_t returned=0;if(!flush12F290(occurrence,handle,returned,d))return false;}
    //A separately returned12F290 must not be replayed inside close.
    if(!resolver_->closeSourceBankMember131040(this,occurrence_,handle_,false,d)){blocked_=true;return false;}
    const bool audio=stage_==Stage::audioOpen;
    stage_=audio?Stage::audioClosed:stage_==Stage::txtOpen?Stage::txtClosed:Stage::jukClosed;
    handle_=-1;occurrence_=nullptr;
    if(audio){
        // Only AFTER actual131040 returned: end host pin custody, not source
        // catalog lifetime or26D8C0 return. This File owner cannot be reused.
        resolver_->freshFileCapacity_->streamLibraryOwner=nullptr;nativeClosed_=true;
    }
    return true; // Stream TXT retains SAME root pin for subsequent JUK.
}
bool OriginalStreamLibraryFile50LA::closeNative(D& d)noexcept {
    d={};if(nativeClosed_)return true;
    if(thread_!=std::this_thread::get_id())return fail(d,E::phase,0);
    if(!acquired_){nativeClosed_=true;return true;}
    if(!resolver_||!resolver_->freshFileCapacity_||!resolver_->scratch_)return fail(d,E::phase,0);
    auto& s=*resolver_->freshFileCapacity_;if(s.streamLibraryOwner!=this)return fail(d,E::phase,0);
    if(handle_>=0){
        if(!s.members150||handle_>=s.memberCount154)return fail(d,E::phase,0);
        auto& row=s.members150[handle_];if(row.courseConfigOwner!=occurrence_||row.deviceID00!=0)return fail(d,E::phase,0);
        // Native abort only. No131040/12F290 return receipt, no registration
        // removal, no cache/device/window retirement and no foreign winner edit.
        row.courseConfigOwner=nullptr;row.cursor10=0;row.flags18&=~4u;
        if(row.auxiliarySlot04>=0&&std::size_t(row.auxiliarySlot04)<resolver_->scratch_->slots.size()&&
           resolver_->scratch_->slots[row.auxiliarySlot04].status08==handle_){
            auto& slot=resolver_->scratch_->slots[row.auxiliarySlot04];
            // A bounded codec can leave its original pre-codec offset stamp
            // and partial scratch bytes. Native disposal must not let a later
            // occurrence reuse that stamp as a successfully decoded block.
            if(blocked_){slot.status08=-1;slot.offset10=-0x6000;}
            resolver_->scratch_->selected148=row.auxiliarySlot04;
        }
    }
    handle_=-1;s.streamLibraryOwner=nullptr;nativeClosed_=true;return true;
}
