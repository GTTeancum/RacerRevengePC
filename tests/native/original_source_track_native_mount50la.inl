// Include after complete Application and aggregate definitions. Private native
// namespace admission only; no original FileManager/source load call occurs.
namespace ra_cold_aggregate50LA {
inline bool Owner::prepareSourceTrackNativeProvider(const original_main_entry50KG::Application& app,
    a::OriginalStandardMaterialDiagnostic50LA& d)noexcept {
    using S=a::OriginalStandardMaterialStatus50LA;
    d={};const auto reject=[&](S status,const char* why)noexcept {d.status=status;d.field=why;d.sourcePC=0;return false;};
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||sourceTrackMountClosed_||
       sourceTrackMountAttempted_||
       !worldColdConstructed_||!worldApplication_||app.entities()!=worldApplication_||
       !source_||!prefix_.sources||!source_->sourcesDecoded()||!source_->sourceTrackInput_||
       !source_->sourceTrackInput_->loaded()||
       source_->sourceTrackInput_->identity()!=a::originalUiArchiveIdentity50KG(a::OriginalUiArchiveId50KG::trackRa)||
       worldAttempted_||prefix_.staticWorld||worldImagesAttempted_||trackParticleAttempted_||modelPrefixesAttempted_||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||rosterMountAttempted_)
        return reject(S::phase,"native Track namespace requires the same opt-in cold source before resource consumers");
    // Parent may preadmit/bind these native wrappers before namespace staging.
    // They must not yet own an actual Config allocation, member, or mount.
    if(sourceTrackFile_&&(sourceTrackFile_->thread_!=std::this_thread::get_id()||
       sourceTrackFile_->attempted_||sourceTrackFile_->acquired_||sourceTrackFile_->joined_||
       sourceTrackFile_->nativeClosed_||sourceTrackFile_->occurrence_||sourceTrackFile_->handle_!=-1))
        return reject(S::phase,"Track wrapper already entered source transport");
    if(sourceWorldConfig_&&(sourceWorldConfig_->thread_!=std::this_thread::get_id()||
       sourceWorldConfig_->file_!=sourceTrackFile_||sourceWorldConfig_->allocated_||sourceWorldConfig_->source_||
       sourceWorldConfig_->names_||sourceWorldConfig_->index_||sourceWorldConfig_->attemptRetained_||
       sourceWorldConfig_->opens_||sourceWorldConfig_->sourceClosed_||sourceWorldConfig_->nativeClosed_))
        return reject(S::phase,"World Config wrapper already entered source construction");
    const auto* track=source_->sourcesDecoded();const auto& archive=source_->sourceTrackInput_;
    if(archive->memberCount()!=1010u||track->archive.entries.size()!=1010u||track->archive.decoded.size()!=29687808u)
        return reject(S::source,"actual complete RA directory differs");
    // Same authenticated opt-in input and the same decoded source corpus. No
    // supplied rows, standalone UI catalog identity, or selected-member subset.
    for(std::size_t i=0;i<track->archive.entries.size();++i){
        const auto& row=track->archive.entries[i];const auto member=archive->member(i);
        if(!member.identity||row.name!=member.identity->name||row.logical_offset!=member.identity->logicalOffset||
           row.bytes!=member.size||row.logical_offset>track->archive.decoded.size()||
           row.bytes>track->archive.decoded.size()-row.logical_offset||
           std::memcmp(track->archive.decoded.data()+row.logical_offset,member.data,member.size))
            return reject(S::source,"retained Track input and decoded archive are not the same full inventory");
    }
    sourceTrackMountAttempted_=true;
    sourceTrackMountApplication_=&app;sourceTrackMountSource_=source_;
    try {
        sourceTrackMountDomain_=app.standardMaterialDomain();
        const auto& domain=sourceTrackMountDomain_;
        if(!domain||!domain->prepared()||!domain->materials382B38()||!domain->resolver3E1A70()||!domain->archiveProvider3E1A70()||
           !domain->archiveProvider3E1A70()->sealed()||
           domain->materials382B38()->coldCollectorCurrent(domain->coldCollector14D440()))
            return reject(S::phase,"actual Application material domain/provider is not a native mount admission point");
        const auto& provider=domain->archiveProvider3E1A70();
        // Same native lease initialization used by the existing retained
        // roster mount path, but this capability contains ONLY this Track bank.
        sourceTrackMountLease_.provider_=provider;sourceTrackMountLease_.thread_=std::this_thread::get_id();
        a::OriginalStandardArchiveProvider50LA::Diagnostic mounted;
        if(!provider->mountAuthenticatedRes("TRACKS/RA.RES",ra_cold_sources50LA::archiveSha,4604561,
            track->archive,mounted,&sourceTrackMountLease_)){
            d.slot=mounted.mount;d.storedSlots=mounted.ordinal;
            return reject(mounted.status==a::OriginalStandardArchiveProvider50LA::Status::allocation?S::allocation:S::archive,
                "actual native Track mount rejected; exact partial lease retained");
        }
        if(!provider->sealNativeMounts(mounted))return reject(S::archive,"native Track namespace seal refused; acquired mount retained");
        sourceTrackMountComplete_=true;return true;
    }catch(...){return reject(S::allocation,"native Track namespace admission exception; acquired custody retained");}
}
inline bool Owner::closeSourceTrackNativeProvider(a::OriginalStandardMaterialDiagnostic50LA& d)noexcept {
    using S=a::OriginalStandardMaterialStatus50LA;
    d={};const auto reject=[&](const char* why)noexcept {d.status=S::phase;d.field=why;d.sourcePC=0;return false;};
    if(nativeScbThread_!=std::this_thread::get_id())return reject("native Track mount owner-thread mismatch");
    // Exact owned transport/Config must finish cleanup first. Merely closed
    // numeric handles or a caller-ready boolean cannot withdraw this custody.
    if(sourceWorldConfig_||sourceTrackFile_)return reject("actual World Config/Track transport still retained");
    if(sourceTrackMountClosed_)return true;
    if(sourceTrackMountLease_.provider_&&!sourceTrackMountLease_.closed_){
        if(!sourceTrackMountDomain_||!sourceTrackMountDomain_->resolver3E1A70()||
           sourceTrackMountDomain_->archiveProvider3E1A70()!=sourceTrackMountLease_.provider_)
            return reject("native Track mount domain/provider identity changed");
        // Resolver performs whole acquired-set resource/reference/provenance
        // preflight before removing any mount. Active foreign exact-repeat
        // mounts were not acquired and are never removed by this lease.
        if(!sourceTrackMountDomain_->resolver3E1A70()->retireNativeRaceMounts1316A0(sourceTrackMountLease_,d.texture))
            return reject("native Track mount still borrowed or exact retirement custody changed");
        d.texture.sourceEffectsCommitted=false; //This call is native cleanup, not original1316A0 execution.
    }else if(sourceTrackMountLease_.count_)return reject("native Track mount retained entries lost provider custody");
    sourceTrackMountDomain_.reset();sourceTrackMountSource_.reset();sourceTrackMountApplication_=nullptr;
    sourceTrackMountComplete_=false;sourceTrackMountClosed_=true;return true;
}
} // namespace ra_cold_aggregate50LA
