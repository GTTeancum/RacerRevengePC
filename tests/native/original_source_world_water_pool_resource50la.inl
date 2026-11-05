//Included inside ra_cold_aggregate50LA. A separate one-shot, same-Pool
//continuation from before2BB044; every predecessor receipt stays immutable.
//Fresh service resolutions below are native mappings for the SAME Application,
//not source13FB6C publication and never host addresses cast to guest pointers.
bool Owner::waterPoolFileServiceCurrentNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||!sourceLoadApplication_||
       sourceTrackMountApplication_!=sourceLoadApplication_||!sourceTrackMountComplete_||
       sourceTrackMountClosed_||!sourceTrackMountDomain_||!sourceTrackFile_||
       sourceLoadApplication_->standardMaterialDomain()!=sourceTrackMountDomain_||
       sourceLoadApplication_->entities()!=worldApplication_)return false;
    const auto& domain=*sourceTrackMountDomain_;const auto& track=*sourceTrackFile_;
    return domain.resolver3E1A70()&&domain.archiveProvider3E1A70()&&
        sourceTrackMountLease_.provider_==domain.archiveProvider3E1A70()&&
        !sourceTrackMountLease_.closed_&&track.attempted_&&track.acquired_&&track.joined_&&
        !track.nativeClosed_&&track.thread_==std::this_thread::get_id()&&
        track.resolver_==domain.resolver3E1A70()&&track.provider_==domain.archiveProvider3E1A70();
    //No Water Config-open, World400, search index, row association or provider
    //revision requirement. Exact table/pin checks belong to the file preflight.
}

bool Owner::preflightNativeWaterPoolResourceHistory()const noexcept {
    using N=SourceWaterConfig::NativeWaterPoolResource;using S=WaterPoolResourceStop;
    const auto& n=sourceWaterConfig_.nativeWaterPoolResource;
    const auto& name=sourceWaterConfig_.nativeWaterPoolName;
    const auto& b=sourceWaterConfig_.nativeWaterPoolBacking;
    const auto& c=sourceWaterConfig_.nativeWaterPoolEntity;
    const auto virginLoad=[](const N::ServiceLoad& x)noexcept {
        return !x.attempted&&!x.resolved&&!x.pc&&!x.applicationIdentity&&!x.domainIdentity&&
            !x.resolverIdentity&&!x.providerIdentity&&!x.trackIdentity&&!x.trackMountIdentity&&
            x.trackMount==static_cast<std::size_t>(-1)&&!x.trackGeneration;
    };
    const auto loadAt=[&](const N::ServiceLoad& x,std::uint32_t pc)noexcept {
        if(!x.attempted)return virginLoad(x);
        if(x.pc!=pc)return false;
        if(!x.resolved)return !x.applicationIdentity&&!x.domainIdentity&&!x.resolverIdentity&&
            !x.providerIdentity&&!x.trackIdentity&&!x.trackMountIdentity&&
            x.trackMount==static_cast<std::size_t>(-1)&&!x.trackGeneration;
        return x.applicationIdentity&&x.domainIdentity&&x.resolverIdentity&&x.providerIdentity&&x.trackIdentity&&
            x.trackMountIdentity&&x.trackMount!=static_cast<std::size_t>(-1)&&x.trackGeneration;
    };
    const auto sameLoad=[&](const N::ServiceLoad& x)noexcept {
        const auto& first=n.load2BB044;
        return !x.resolved||(first.resolved&&x.applicationIdentity==first.applicationIdentity&&
            x.domainIdentity==first.domainIdentity&&x.resolverIdentity==first.resolverIdentity&&
            x.providerIdentity==first.providerIdentity&&x.trackIdentity==first.trackIdentity&&
            x.trackMountIdentity==first.trackMountIdentity&&x.trackMount==first.trackMount&&
            x.trackGeneration==first.trackGeneration);
    };
    const auto noClose=[&]()noexcept {
        return virginLoad(n.load2BB10C)&&!n.closeHandleIsMinusOne&&!n.closeCallIssued&&!n.closeSourceReturned;
    };
    const auto noVersion=[&]()noexcept {
        return !n.versionLoaded&&!n.versionBranchExecuted&&!n.versionWord&&!n.allocationArgument&&!n.versionTwo&&noClose();
    };
    const auto noRead=[&]()noexcept {
        return virginLoad(n.load2BB0CC)&&!n.readCallIssued&&!n.readNativeReturned&&!n.readResultAdmitted&&!n.readHandle&&
            !n.versionOffset&&!n.itemSize&&!n.itemCount&&noVersion();
    };
    const auto noLookup=[&]()noexcept {
        return !n.handleInitialized&&!n.associationInitialized&&!n.lookupCallIssued&&!n.lookupReturned&&
            !n.nameOffset&&!n.modeLiteral&&!n.handleOffset&&!n.associationOffset&&!n.lookupT1&&
            !n.sourceHandle1E0&&!n.sourceAssociated1E8&&!n.lookupV0&&!n.handleOutputStored&&
            !n.associationOutputStored&&!n.coldBranch&&!n.file&&noRead();
    };
    if(!n.attempted)return n.stop==S::none&&!n.pc&&!n.wholeRoleIdentity&&!n.canonicalEntityIdentity&&
        !n.loaderFrameIdentity&&!n.session&&!n.key&&!n.namePrepared&&n.exactName.empty()&&
        virginLoad(n.load2BB044)&&noLookup()&&!n.nativeWithdrawalAttempted&&!n.nativeWithdrawn;
    //Opaque identity comparisons only: retired Pool roles and historical
    //association pointers are never dereferenced by this local validator.
    if(!name.attempted||name.refusal!=WaterPoolNameRefusal::none||name.pc!=0x2bb044u||
       !name.nameCopy.returned||!name.normalizedLength||!name.loaderFrame||name.loaderFrameBytes!=496u||
       name.loaderReturned||name.loaderStackReleased||n.loaderFrameIdentity!=&name||
       n.wholeRoleIdentity!=name.wholeRoleIdentity||n.canonicalEntityIdentity!=name.canonicalEntityIdentity||
       !n.wholeRoleIdentity||!n.canonicalEntityIdentity||n.session!=c.session||n.key!=c.key||
       !c.admitted||!c.enrolled||!c.countAcquired||!c.referenceAcquired||
       !loadAt(n.load2BB044,0x2bb044u)||!loadAt(n.load2BB0CC,0x2bb0ccu)||
       !loadAt(n.load2BB10C,0x2bb10cu)||!sameLoad(n.load2BB0CC)||!sameLoad(n.load2BB10C))return false;
    if(n.nativeWithdrawn&&(!n.nativeWithdrawalAttempted||!n.file||!n.file->nativeClosed()))return false;
    if(n.file){
        if(!n.file->historyNative(n.wholeRoleIdentity)||!n.load2BB044.resolved||
           n.file->resolver_.get()!=n.load2BB044.resolverIdentity||
           n.file->provider_.get()!=n.load2BB044.providerIdentity||
           n.file->track_.get()!=n.load2BB044.trackIdentity||
           n.file->serviceMountIdentity_!=n.load2BB044.trackMountIdentity||
           n.file->serviceTrackMount_!=n.load2BB044.trackMount||
           n.file->serviceTrackGeneration_!=n.load2BB044.trackGeneration||
           n.nativeWithdrawn!=n.file->nativeClosed()||
           (b.nativeRetired&&!n.file->nativeClosed()))return false;
    }else if(n.nativeWithdrawalAttempted||n.nativeWithdrawn)return false;
    if(!n.namePrepared)return n.stop==S::namePreparation&&n.pc==0x2bb044u&&
        virginLoad(n.load2BB044)&&noLookup();
    if(n.exactName.size()!=*name.normalizedLength||n.exactName.size()+1u>name.scratchKnown||
       name.scratch60[n.exactName.size()])return false;
    for(std::size_t i=0;i<n.exactName.size();++i)
        if(!name.scratch60[i]||static_cast<std::uint8_t>(n.exactName[i])!=name.scratch60[i])return false;
    if(!n.load2BB044.attempted)return false;
    if(!n.load2BB044.resolved)return n.stop==S::service2BB044&&n.pc==0x2bb044u&&noLookup();
    if(!n.handleInitialized||!n.associationInitialized||!n.lookupCallIssued||
       n.nameOffset!=0x60u||n.modeLiteral!=0x3c0e88u||n.handleOffset!=0x1e0u||
       n.associationOffset!=0x1e8u||n.lookupT1!=0u||!n.sourceHandle1E0||!n.sourceAssociated1E8||
       *n.sourceAssociated1E8!=nullptr||n.lookupReturned!=bool(n.lookupV0))return false;
    if(n.file){
        const auto& file=*n.file;
        if((file.searchReached_&&file.name_!=n.exactName)||n.lookupV0!=file.sourceReturnV0_||
           n.handleOutputStored!=file.outputHandleStored_||
           n.associationOutputStored!=file.outputAssociationStored_||
           n.sourceHandle1E0!=(file.sourceHandle1E0_?file.sourceHandle1E0_:std::optional<std::int64_t>(-1))||
           (file.sourceAssociated1E8_&&n.sourceAssociated1E8!=file.sourceAssociated1E8_))return false;
    }else if(n.lookupReturned||n.handleOutputStored||n.associationOutputStored||n.sourceHandle1E0!=-1)return false;
    if(!n.lookupReturned)return n.stop==S::lookup132820&&!n.coldBranch&&noRead();
    if(!n.file)return false;
    if(n.lookupV0==0)return n.stop==S::missingDiagnostic2BB07C&&n.pc==0x2bb07cu&&
        !n.handleOutputStored&&!n.associationOutputStored&&n.sourceHandle1E0==-1&&!n.coldBranch&&noRead();
    if(n.lookupV0!=1||!n.handleOutputStored||!n.associationOutputStored||*n.sourceHandle1E0<0||
       !n.coldBranch||!n.load2BB0CC.attempted)return false;
    if(!n.load2BB0CC.resolved)return n.stop==S::service2BB0CC&&n.pc==0x2bb0ccu&&
        !n.readCallIssued&&!n.readNativeReturned&&!n.readResultAdmitted&&!n.readHandle&&
        !n.versionOffset&&!n.itemSize&&!n.itemCount&&noVersion();
    if(!n.readCallIssued||n.readHandle!=n.sourceHandle1E0||n.versionOffset!=0x1ecu||
       n.itemSize!=4u||n.itemCount!=1u)return false;
    if(n.readNativeReturned!=n.file->read_.nativeReturned||n.readResultAdmitted!=n.file->read_.resultAdmitted)return false;
    if(!n.readResultAdmitted)return n.stop==S::read12FB30&&noVersion();
    if(!n.readNativeReturned||!n.file->read_.sourceItems)return false;
    const auto word=n.file->versionWordNative();
    if(!word)return n.stop==S::unknownVersion2BB0E4&&n.pc==0x2bb0e4u&&noVersion();
    if(!n.versionLoaded||!n.versionBranchExecuted||n.versionWord!=word||n.versionTwo!=(*word==2u)||
       n.allocationArgument!=0x30u)return false;
    if(*word==2u)return n.stop==S::resourceAllocation2BB12C&&n.pc==0x2bb12cu&&noClose()&&
        !n.file->sourceCloseAttempted_&&!n.file->sourceCloseReturned_;
    if(n.closeHandleIsMinusOne!=false||!n.closeCallIssued||!n.load2BB10C.attempted)return false;
    if(!n.load2BB10C.resolved)return n.stop==S::service2BB10C&&n.pc==0x2bb10cu&&
        !n.closeSourceReturned&&!n.file->sourceCloseAttempted_;
    if(n.closeSourceReturned!=n.file->sourceCloseReturned_)return false;
    if(!n.closeSourceReturned)return n.stop==S::close131040;
    return n.stop==S::entityDispatch2BB110&&n.pc==0x2bb110u&&n.file->sourceCloseV0_==1;
}

bool Owner::preflightNativeWaterPoolFileWithdrawal()const noexcept {
    const auto& n=sourceWaterConfig_.nativeWaterPoolResource;
    if(!n.attempted)return true; //No broadening of the unentered predecessor.
    if(!preflightNativeWaterPoolResourceHistory())return false;
    if(!n.file||n.nativeWithdrawn)return true;
    if(!waterPoolFileServiceCurrentNative()||!sourceWaterProvider_||!sourceWaterProvider_->nativePool)return false;
    const auto& p=*sourceWaterProvider_->nativePool;
    const auto& first=n.load2BB044;
    if(n.wholeRoleIdentity!=&p.role||n.canonicalEntityIdentity!=&p.entity||
       first.applicationIdentity!=sourceLoadApplication_||first.domainIdentity!=sourceTrackMountDomain_.get()||
       first.resolverIdentity!=sourceTrackMountDomain_->resolver3E1A70().get()||
       first.providerIdentity!=sourceTrackMountDomain_->archiveProvider3E1A70().get()||
       first.trackIdentity!=sourceTrackFile_.get())return false;
    return n.file->preflightNative(n.wholeRoleIdentity)&&n.file->canCloseNative(n.wholeRoleIdentity);
}

Owner::WaterPoolResourceResult Owner::acquireWaterPoolResourceNativeOnly(WaterArrayBackingMode mode)noexcept {
    using N=SourceWaterConfig::NativeWaterPoolResource;using S=WaterPoolResourceStop;
    WaterPoolResourceResult result;auto& n=sourceWaterConfig_.nativeWaterPoolResource;
    const auto& name=sourceWaterConfig_.nativeWaterPoolName;
    const auto& backing=sourceWaterConfig_.nativeWaterPoolBacking;
    const auto& entity=sourceWaterConfig_.nativeWaterPoolEntity;
    const auto finish=[&](WaterPoolResourceResult::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.originalSourceStop=0x30492cu;
        result.allocatorStop=sourceWaterConfig_.nativeWaterPoolAllocationEntry.pc;
        result.formatterStop=sourceWaterConfig_.nativeWaterPoolPathFormat.pc;
        result.entityPrefixStop=entity.pc;result.sceneStop=sourceWaterConfig_.nativeWaterPoolScene.pc;
        result.nameStop=name.pc;result.adaptedPC=n.pc;result.lookupV0=n.lookupV0;
        result.sourceHandle1E0=n.sourceHandle1E0;result.lookupReturned=n.lookupReturned;
        result.readNativeReturned=n.readNativeReturned;result.readResultAdmitted=n.readResultAdmitted;result.versionWord=n.versionWord;
        result.sourceCloseReturned=n.closeSourceReturned;result.nativeFileWithdrawn=n.nativeWithdrawn;
        result.nativePoolRetired=backing.nativeRetired;result.diagnostic=n.diagnostic;
        if(n.file){const auto& f=*n.file;result.readSourceItems=f.read_.sourceItems;
            result.sourceCloseV0=f.sourceCloseV0_;result.nativeHandlePresent=f.nativeHandle_!=-1;
            result.copiedVersionBytes=f.read_.copied;
            //Only known lanes are copied. The source destination's unknown
            //suffix is never read, zero-filled, returned or compared.
            for(std::size_t i=0;i<4u;++i)if(f.read_.known[i])result.versionBytes[i]=f.read_.bytes[i];}
        return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_)return result;
    std::size_t counted=0;if(!sourceWaterPoolCountedNative(counted))return result;
    if(n.attempted)return finish(WaterPoolResourceResult::Status::alreadyAttempted);
    if(!waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||
       !preflightSourceWorldParticleCountNative()||counted!=1u||!backing.nativeAllocated||backing.nativeRetired||
       !sourceWaterProvider_->nativePool||!name.attempted||name.refusal!=WaterPoolNameRefusal::none||
       name.pc!=0x2bb044u||!name.normalizedLength||!name.nameCopy.returned)return result;
    auto& p=*sourceWaterProvider_->nativePool;
    if(!p.wholeArgument||p.wholeArgument->role!=&p.role||p.wholeArgument->offset||
       !p.entityArgument||p.entityArgument->role!=&p.role||p.entityArgument->offset!=8u||
       name.wholeRoleIdentity!=&p.role||name.canonicalEntityIdentity!=&p.entity)return result;
    n.attempted=true;n.wholeRoleIdentity=&p.role;n.canonicalEntityIdentity=&p.entity;
    n.loaderFrameIdentity=&name;n.session=entity.session;n.key=entity.key;n.pc=0x2bb044u;
    n.stop=S::namePreparation;
    try {n.exactName.assign(reinterpret_cast<const char*>(name.scratch60.data()),*name.normalizedLength);}
    catch(...){return finish(WaterPoolResourceResult::Status::nativeStopped);}
    n.namePrepared=true;
    const auto load=[&](N::ServiceLoad& x,std::uint32_t pc,S stop)noexcept {
        n.pc=pc;n.stop=stop;x.attempted=true;x.pc=pc;
        if(!waterPoolFileServiceCurrentNative())return false;
        const auto& domain=sourceTrackMountDomain_;
        const auto& first=n.load2BB044;
        if(&x!=&first&&(!first.resolved||first.applicationIdentity!=sourceLoadApplication_||
           first.domainIdentity!=domain.get()||first.resolverIdentity!=domain->resolver3E1A70().get()||
           first.providerIdentity!=domain->archiveProvider3E1A70().get()||first.trackIdentity!=sourceTrackFile_.get()))return false;
        const void* mountIdentity=nullptr;std::size_t mount=static_cast<std::size_t>(-1);std::uint64_t generation=0;
        if(!domain->resolver3E1A70()->waterPoolTrackRelationNative(sourceTrackFile_.get(),
           domain->archiveProvider3E1A70().get(),mountIdentity,mount,generation)||
           (&x!=&first&&(mountIdentity!=first.trackMountIdentity||mount!=first.trackMount||
                        generation!=first.trackGeneration)))return false;
        x.applicationIdentity=sourceLoadApplication_;x.domainIdentity=domain.get();
        x.resolverIdentity=domain->resolver3E1A70().get();x.providerIdentity=domain->archiveProvider3E1A70().get();
        x.trackIdentity=sourceTrackFile_.get();x.trackMountIdentity=mountIdentity;x.trackMount=mount;
        x.trackGeneration=generation;x.resolved=true;return true;
    };
    if(!load(n.load2BB044,0x2bb044u,S::service2BB044))return finish(WaterPoolResourceResult::Status::nativeStopped);
    n.pc=0x2bb048;n.sourceHandle1E0=-1;n.handleInitialized=true;
    n.pc=0x2bb050;n.associationOffset=0x1e8u;
    n.pc=0x2bb054;n.nameOffset=0x60u;n.pc=0x2bb058;n.modeLiteral=0x3c0e88u;
    n.pc=0x2bb05c;n.handleOffset=0x1e0u;n.pc=0x2bb060;n.lookupT1=0u;
    n.pc=0x2bb064;n.lookupCallIssued=true;
    n.pc=0x2bb068;n.sourceAssociated1E8=nullptr;n.associationInitialized=true;
    n.pc=0x132820;n.stop=S::lookup132820;
    n.lookupReturned=a::OriginalWaterPoolSourceFile50LA::begin132820(
        sourceTrackMountDomain_->resolver3E1A70(),sourceTrackMountDomain_->archiveProvider3E1A70(),
        sourceTrackFile_,n.wholeRoleIdentity,n.exactName,n.file,n.diagnostic);
    if(n.file){
        const auto& f=*n.file;n.handleOutputStored=f.outputHandleStored_;n.associationOutputStored=f.outputAssociationStored_;
        if(f.sourceHandle1E0_)n.sourceHandle1E0=f.sourceHandle1E0_;
        if(f.sourceAssociated1E8_)n.sourceAssociated1E8=f.sourceAssociated1E8_;
        n.lookupV0=f.sourceReturnV0_;
    }
    if(!n.lookupReturned){n.pc=n.diagnostic.sourcePC?n.diagnostic.sourcePC:0x132820u;
        return finish(WaterPoolResourceResult::Status::nativeStopped);}
    n.pc=0x2bb06c;n.pc=0x2bb070;
    if(n.lookupV0==0){n.pc=0x2bb074;n.pc=0x2bb078;n.pc=0x2bb07c;n.stop=S::missingDiagnostic2BB07C;
        return finish(WaterPoolResourceResult::Status::sourceBoundary);}
    //Only the genuine cold132820=1 path is implemented. The warm no-custodian
    //path cannot reach here: it stops before either source output store.
    n.pc=0x2bb09c;n.pc=0x2bb0a0;n.pc=0x2bb0a4;n.coldBranch=true;n.pc=0x2bb0c8;
    if(!load(n.load2BB0CC,0x2bb0ccu,S::service2BB0CC))return finish(WaterPoolResourceResult::Status::nativeStopped);
    n.pc=0x2bb0d0;n.readHandle=n.sourceHandle1E0;n.pc=0x2bb0d4;n.versionOffset=0x1ecu;
    n.pc=0x2bb0d8;n.itemSize=4u;n.pc=0x2bb0dc;n.readCallIssued=true;n.pc=0x2bb0e0;n.itemCount=1u;
    n.pc=0x12fb30;n.stop=S::read12FB30;
    n.readResultAdmitted=n.file->readVersion12FB30(n.diagnostic);n.readNativeReturned=n.file->read_.nativeReturned;
    if(!n.readResultAdmitted){n.pc=n.diagnostic.sourcePC?n.diagnostic.sourcePC:0x12fb30u;
        return finish(WaterPoolResourceResult::Status::nativeStopped);}
    n.pc=0x2bb0e4;const auto word=n.file->versionWordNative();
    if(!word){n.stop=S::unknownVersion2BB0E4;return finish(WaterPoolResourceResult::Status::sourceBoundary);}
    n.versionWord=word;n.versionLoaded=true;n.pc=0x2bb0e8;n.pc=0x2bb0ec;
    n.versionTwo=*word==2u;n.versionBranchExecuted=true;
    n.pc=0x2bb0f0;n.allocationArgument=0x30u; //Delay executes on BOTH paths.
    if(*n.versionTwo){n.pc=0x2bb12c;n.stop=S::resourceAllocation2BB12C;
        return finish(WaterPoolResourceResult::Status::sourceBoundary);}
    n.pc=0x2bb0f4;n.pc=0x2bb0f8;n.pc=0x2bb0fc;n.closeHandleIsMinusOne=*n.sourceHandle1E0==-1;
    n.pc=0x2bb100; //Cold success carries a real numeric handle, not -1.
    n.pc=0x2bb104;n.pc=0x2bb108;n.closeCallIssued=true;
    if(!load(n.load2BB10C,0x2bb10cu,S::service2BB10C))return finish(WaterPoolResourceResult::Status::nativeStopped);
    n.pc=0x131040;n.stop=S::close131040;
    n.closeSourceReturned=n.file->closeSource131040(n.diagnostic);
    if(!n.closeSourceReturned){n.pc=n.diagnostic.sourcePC?n.diagnostic.sourcePC:0x131040u;
        return finish(WaterPoolResourceResult::Status::nativeStopped);}
    //Historical P+1E0 remains unchanged. No Entity dispatch/release, loader
    //return, Resource allocation/retain/association, W+1B4 or slot publication.
    n.pc=0x2bb110;n.stop=S::entityDispatch2BB110;
    return finish(WaterPoolResourceResult::Status::sourceBoundary);
}
