//Included after the accepted material/reader chain. The two caller entries
//share only the genuine topology prefix. Earlier source PCs are immutable.
bool Owner::preflightNativeWaterPoolTopologyHistory(std::array<std::uint8_t,4>& bytes)const noexcept {
    using N=SourceWaterConfig::NativeWaterPoolTopologyCount;using S=WaterPoolTopologyStop;
    using K=WaterPoolTopologyEntry;using M=N::Save::Meaning;
    using F=a::OriginalWaterPoolSourceFile50LA;
    bytes={};const auto& n=sourceWaterConfig_.nativeWaterPoolTopologyCount;
    const auto& d=sourceWaterConfig_.nativeWaterPoolData;const auto& p=sourceWaterConfig_.nativeWaterPoolMaterial;
    const auto& m=sourceWaterConfig_.nativeWaterPoolMaterialReader;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;const auto& name=sourceWaterConfig_.nativeWaterPoolName;
    const auto virginLoad=[](const SourceWaterConfig::NativeWaterPoolResource::ServiceLoad& l)noexcept {
        return !l.attempted&&!l.resolved&&!l.pc&&!l.applicationIdentity&&!l.domainIdentity&&!l.resolverIdentity&&
            !l.providerIdentity&&!l.trackIdentity&&!l.trackMountIdentity&&
            l.trackMount==static_cast<std::size_t>(-1)&&!l.trackGeneration;
    };
    const auto noMath=[&]()noexcept {
        return !n.topologyLoaded&&!n.shifted&&!n.added&&!n.topologyWord&&!n.signedTopology&&!n.topologyRegister&&
            !n.shiftWord&&!n.shiftRegister&&!n.sumWord&&!n.sumRegister&&!n.prospectiveRequestWord&&
            !n.topologyLoadPC&&!n.shiftPC&&!n.sumPC;
    };
    const auto noRead=[&]()noexcept {
        return !n.argumentsPrepared&&!n.prefixSynchronized&&!n.readResultAdmitted&&!n.destinationOffset&&
            !n.itemSize&&!n.itemCount&&!n.copied&&n.topologyBytes==std::array<std::uint8_t,4>{}&&noMath();
    };
    const auto noBody=[&]()noexcept {
        for(const auto& s:n.saves)if(s.pc||s.offset||s.meaning!=M::none||s.roleIdentity||s.low64)return false;
        return !n.callIssued&&!n.callDelay&&!n.frame&&!n.wholeSelected&&!n.callPC&&!n.delayPC&&!n.entryPC&&
            !n.callerReturnPC&&!n.frameBytes&&!n.flag&&!n.serviceLuiPC&&virginLoad(n.load2BAB30)&&noRead();
    };
    const auto noLoop=[&]()noexcept {
        return !n.ordinalIncremented&&!n.ordinalNop&&!n.countLoaded&&!n.compared&&!n.loopBranch&&!n.loopDelay&&
            !n.ordinal&&!n.countWord&&!n.countLoadOffset&&!n.countLoadPC&&!n.signedCount&&!n.countRegister&&!n.loopTaken&&
            n.currentCountBytes==std::array<std::uint8_t,4>{}&&n.currentCountMask==std::array<std::uint8_t,4>{};
    };
    if(!n.attempted)return n.entry==K::none&&n.stop==S::none&&!n.pc&&!n.poolRoleIdentity&&!n.dataRoleIdentity&&
        !n.fileIdentity&&!n.loaderFrameIdentity&&!n.poolEpoch&&!n.dataEpoch&&!n.exitBranch&&!n.aliasLoaded&&
        !n.aliasLoadPC&&!n.handleReloaded&&!n.argumentData&&!n.argumentHandle&&!n.pendingCallPC&&!n.pendingDelayPC&&
        noLoop()&&noBody()&&(!f.file||!f.file->topologyRead_.entered);
    //This is deliberately a leaf: no live role/payload dereference and no
    //Data, material-count, Pool, provider or aggregate-floor callback. The
    //outer floor separately validates all predecessor descendants.
    if(!d.nativeAllocated||!d.dataRoleIdentity||!d.dataEpoch||!d.poolEpoch||!d.readResultAdmitted||
       !d.countWord||!d.signedCount||!d.nonpositive||d.derivedStores!=8u||!d.aliasStored||!d.aliasReloaded||
       !f.file||n.fileIdentity!=f.file.get()||n.fileIdentity!=d.fileIdentity||
       n.dataRoleIdentity!=d.dataRoleIdentity||n.poolRoleIdentity!=d.poolRoleIdentity||
       n.dataEpoch!=*d.dataEpoch||n.poolEpoch!=*d.poolEpoch||!n.dataEpoch||!n.poolEpoch||
       n.loaderFrameIdentity!=&name||f.loaderFrameIdentity!=&name||!name.flagInS4||
       name.argumentA3!=1u||name.argumentA2!=1u||!name.loaderFrame||name.loaderReturned||name.loaderStackReleased||
       !f.sourceHandle1E0||*f.sourceHandle1E0<0||d.savedHandleS3!=f.sourceHandle1E0||
       !f.file->historyNative(f.wholeRoleIdentity))return false;
    const auto sign=[](std::uint32_t w)noexcept {
        return w&0x80000000u?(0xffffffff00000000ull|std::uint64_t(w)):std::uint64_t(w);
    };
    if(n.entry==K::returnedFirstMaterial){
        if(d.stop!=WaterPoolDataStop::childAllocation2BB1B0||d.pc!=0x2bb1b0u||*d.nonpositive||*d.signedCount<=0||
           d.childPointerZeroRestored||d.topologyAliasReloaded||d.topologyHandleReloaded||
           !p.attempted||!p.rawAllocated||!p.childAllocated||!p.slotStored||p.ordinal!=0u||
           p.originalCount!=d.countWord||p.dataRoleIdentity!=n.dataRoleIdentity||p.dataEpoch!=n.dataEpoch||
           p.readerFlag!=1u||!m.attempted||!m.filePrepared||!m.returned||!m.frameReleased||
           m.stop!=WaterPoolMaterialReaderStop::callerOrdinal2BB220||m.pc!=0x2bb220u||m.callerPC!=0x2bb220u||
           !m.word80Loaded||m.word80!=0u||m.zero80!=true||!m.branch918||!m.delay91C||
           m.branch920||m.delay924||m.zeroFlag||!m.returnIssued||!m.returnDelay||
           m.restorePCs!=std::array<std::uint32_t,4>{{0x2bf9c4u,0x2bf9c8u,0x2bf9ccu,0x2bf9d0u}}||
           m.returnPC!=0x2bf9d4u||m.returnDelayPC!=0x2bf9d8u||m.releasedBytes!=64u||
           m.dataRoleIdentity!=n.dataRoleIdentity||m.dataEpoch!=n.dataEpoch||
           m.arrayRoleIdentity!=p.arrayRoleIdentity||m.materialRoleIdentity!=p.childRoleIdentity||
           m.returnV0!=f.file->materialReader_.reads[17].sourceItems)return false;
        for(std::size_t i=0;i<18u;++i)if(!m.reads[i].reached||!m.reads[i].prefixSynchronized||
            !f.file->materialReader_.reads[i].resultAdmitted)return false;
        if(!n.ordinalIncremented||!n.ordinalNop||n.ordinal!=1u||!n.countLoaded||n.countLoadOffset!=0x14u||n.countLoadPC!=0x2bb228u||
           !n.compared||!n.loopBranch||!n.loopDelay||n.currentCountBytes!=d.countBytes||
           n.currentCountMask!=std::array<std::uint8_t,4>{{255u,255u,255u,255u}})return false;
        std::uint32_t word=0;for(std::size_t i=0;i<4u;++i)word|=std::uint32_t(n.currentCountBytes[i])<<(8u*i);
        const auto signedWord=word&0x80000000u?std::int64_t(word)-0x100000000ll:std::int64_t(word);
        if(n.countWord!=word||n.countWord!=d.countWord||n.signedCount!=signedWord||n.countRegister!=sign(word)||
           signedWord<=0||n.loopTaken!=(1<signedWord))return false;
        if(signedWord>1)return n.stop==S::secondMaterialCall2BB1C4&&n.pc==0x2bb1c4u&&
            n.pendingCallPC==0x2bb1c4u&&n.pendingDelayPC==0x2bb1c8u&&!n.exitBranch&&!n.aliasLoaded&&
            !n.aliasLoadPC&&!n.handleReloaded&&!n.argumentData&&!n.argumentHandle&&noBody()&&
            !f.file->topologyRead_.entered; //Not even preparation for a topology file read.
        if(!n.exitBranch||!n.aliasLoaded||n.aliasLoadPC!=0x2bb23cu||!n.handleReloaded)return false;
    }else if(n.entry==K::nonpositiveCount){
        if(!noLoop()||n.exitBranch||n.aliasLoaded||n.aliasLoadPC||n.handleReloaded||
           d.stop!=WaterPoolDataStop::topologyCall2BB24C||d.pc!=0x2bb24cu||!*d.nonpositive||*d.signedCount>0||
           !d.childPointerZeroRestored||!d.topologyAliasReloaded||!d.topologyHandleReloaded||
           p.attempted||p.rawAllocated||p.rawRetired||p.arrayAliasStored||p.arrayRoleIdentity||
           p.childAllocated||p.childRoleIdentity||p.countAcquired||m.attempted||f.file->materialReader_.entered)return false;
    }else return false;
    if(n.argumentData!=n.dataRoleIdentity||n.argumentHandle!=f.sourceHandle1E0||
       !n.callIssued||!n.callDelay||n.callPC!=0x2bb24cu||n.delayPC!=0x2bb250u||n.entryPC!=0x2bab10u||
       n.callerReturnPC!=0x2bb254u||n.flag!=1u||!n.frame||n.frameBytes!=96u||!n.wholeSelected||
       n.serviceLuiPC!=0x2bab2cu||!n.load2BAB30.attempted||n.load2BAB30.pc!=0x2bab30u)return false;
    const std::array<std::uint32_t,5> savePC{{0x2bab14u,0x2bab18u,0x2bab1cu,0x2bab20u,0x2bab28u}};
    const std::array<std::uint32_t,5> saveOffset{{0x40u,0x30u,0x20u,0x10u,0u}};
    const std::array<M,5> meaning{{M::returnAddress,M::handle,M::dataRole,
        n.entry==K::returnedFirstMaterial?M::ordinal:M::dataRole,M::poolRole}};
    for(std::size_t i=0;i<5u;++i){const auto& s=n.saves[i];
        if(s.pc!=savePC[i]||s.offset!=saveOffset[i]||s.meaning!=meaning[i])return false;
        const void* role=i==2u||(i==3u&&n.entry==K::nonpositiveCount)?static_cast<const void*>(n.dataRoleIdentity):
            i==4u?static_cast<const void*>(n.poolRoleIdentity):nullptr;
        if(s.roleIdentity!=role)return false;
        if(role){if(s.low64)return false;}
        else if(s.low64!=(i==0u?std::uint64_t(0x2bb254u):i==1u?std::uint64_t(*n.argumentHandle):1ull))return false;
    }
    const auto& l=n.load2BAB30;const auto& first=f.load2BB044;
    if(!l.resolved)return n.stop==S::service2BAB30&&n.pc==0x2bab30u&&!n.pendingCallPC&&!n.pendingDelayPC&&
        !l.applicationIdentity&&!l.domainIdentity&&!l.resolverIdentity&&!l.providerIdentity&&!l.trackIdentity&&
        !l.trackMountIdentity&&l.trackMount==static_cast<std::size_t>(-1)&&!l.trackGeneration&&noRead()&&
        !f.file->topologyRead_.entered;
    if(!first.resolved||l.applicationIdentity!=first.applicationIdentity||l.domainIdentity!=first.domainIdentity||
       l.resolverIdentity!=first.resolverIdentity||l.providerIdentity!=first.providerIdentity||
       l.trackIdentity!=first.trackIdentity||l.trackMountIdentity!=first.trackMountIdentity||
       l.trackMount!=first.trackMount||l.trackGeneration!=first.trackGeneration||
       !n.argumentsPrepared||n.destinationOffset!=0x1cu||n.itemSize!=4u||n.itemCount!=1u||
       !n.prefixSynchronized)return false;
    const auto& r=f.file->topologyRead_;
    if(r.entered&&(r.dataIdentity!=n.dataRoleIdentity||r.dataEpoch!=n.dataEpoch||r.fileIdentity!=n.fileIdentity||
       r.entryKind!=(n.entry==K::returnedFirstMaterial?F::TopologyEntryKind::returnedFirstMaterial:
           F::TopologyEntryKind::nonpositiveCount)))return false;
    if(n.copied!=r.copied||n.copied>4u||n.readResultAdmitted!=r.resultAdmitted)return false;
    for(std::size_t i=0;i<4u;++i){bytes[i]=i<r.copied?r.bytes[i]:0u;if(n.topologyBytes[i]!=bytes[i])return false;}
    if(!n.readResultAdmitted)return !n.pendingCallPC&&!n.pendingDelayPC&&noMath()&&
        n.stop==(!r.attempted?S::readPreparation:!r.nativeReturned?S::readHelper:S::readAdmission)&&
        n.pc==(r.pc?r.pc:0x2bab40u);
    if(!r.entered||!r.attempted||!r.resultAdmitted||!r.sourceItems||!n.topologyLoaded||!n.shifted||!n.added||
       n.topologyLoadPC!=0x2bab48u||n.shiftPC!=0x2bab4cu||n.sumPC!=0x2bab50u)return false;
    std::uint32_t word=0;for(std::size_t i=0;i<4u;++i)word|=std::uint32_t(bytes[i])<<(8u*i);
    const auto shifted=std::uint32_t(word<<2u),sum=std::uint32_t(shifted+word);
    return n.topologyWord==word&&n.signedTopology==(word&0x80000000u?std::int64_t(word)-0x100000000ll:std::int64_t(word))&&
        n.topologyRegister==sign(word)&&n.shiftWord==shifted&&n.shiftRegister==sign(shifted)&&
        n.sumWord==sum&&n.sumRegister==sign(sum)&&n.prospectiveRequestWord==std::uint32_t(sum<<2u)&&
        n.stop==S::mallocCall2BAB54&&n.pc==0x2bab54u&&n.pendingCallPC==0x2bab54u&&n.pendingDelayPC==0x2bab58u;
}

Owner::WaterPoolTopologyResult Owner::readWaterPoolTopologyCountNativeOnly(WaterArrayBackingMode mode)noexcept {
    using N=SourceWaterConfig::NativeWaterPoolTopologyCount;using S=WaterPoolTopologyStop;
    using K=WaterPoolTopologyEntry;using M=N::Save::Meaning;using Result=WaterPoolTopologyResult;
    using F=a::OriginalWaterPoolSourceFile50LA;
    auto& n=sourceWaterConfig_.nativeWaterPoolTopologyCount;const auto& d=sourceWaterConfig_.nativeWaterPoolData;
    const auto& p=sourceWaterConfig_.nativeWaterPoolMaterial;const auto& m=sourceWaterConfig_.nativeWaterPoolMaterialReader;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;const auto& name=sourceWaterConfig_.nativeWaterPoolName;
    Result result;
    const auto finish=[&](Result::Status status)noexcept {
        result.status=status;result.entry=n.entry;result.stop=n.stop;result.originalSourceStop=0x30492cu;
        result.poolAllocatorStop=sourceWaterConfig_.nativeWaterPoolAllocationEntry.pc;
        result.fileStop=f.pc;result.dataStop=d.pc;result.materialStop=p.pc;result.readerStop=m.pc;result.adaptedPC=n.pc;
        result.pendingCallPC=n.pendingCallPC;result.pendingDelayPC=n.pendingDelayPC;result.ordinal=n.ordinal;
        result.childCount=n.countWord;result.signedChildCount=n.signedCount;result.childCountRegister=n.countRegister;
        result.freshChildCountLoaded=n.countLoaded;
        if(n.entry==K::nonpositiveCount){result.inheritedChildCount=d.countWord;result.inheritedSignedChildCount=d.signedCount;}
        result.topologyWord=n.topologyWord;result.signedTopology=n.signedTopology;result.topologyRegister=n.topologyRegister;
        result.shiftWord=n.shiftWord;result.shiftRegister=n.shiftRegister;result.sumWord=n.sumWord;result.sumRegister=n.sumRegister;
        result.prospectiveRequestWord=n.prospectiveRequestWord;result.callIssued=n.callIssued;result.diagnostic=n.diagnostic;
        if(n.callIssued)for(std::size_t i=0;i<4u;++i)result.topologyBytes[i]=n.topologyBytes[i];
        if(f.file){const auto& r=f.file->topologyRead_;result.readAttempted=r.attempted;result.nativeReturned=r.nativeReturned;
            result.afterObserved=r.afterObserved;result.resultAdmitted=r.resultAdmitted;result.copied=r.copied;
            result.clampedAmount=r.clampedAmount;result.sourceItems=r.sourceItems;
            result.preparationDiagnostic=r.preparationDiagnostic;result.helperDiagnostic=r.helperDiagnostic;
            result.admissionDiagnostic=r.admissionDiagnostic;
            if(r.beforeObserved)result.beforeCursor=r.before.memberCursor10;if(r.afterObserved)result.afterCursor=r.after.memberCursor10;}
        return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_)return result;
    std::size_t poolCount=0,materialCount=0;
    if(!sourceWaterPoolCountedNative(poolCount)||!sourceWaterPoolMaterialCountedNative(materialCount))return result;
    if(n.attempted)return finish(Result::Status::alreadyAttempted);
    if(!waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       poolCount!=1u||!d.nativeAllocated||d.nativeRetired||!d.readResultAdmitted||!d.countWord||!d.signedCount||
       !d.nonpositive||!name.flagInS4||name.argumentA3!=1u||name.argumentA2!=1u||
       !f.file||f.file->nativeClosed()||f.file->nativeCloseAttempted_||f.file->sourceCloseAttempted_||
       f.file->nativeHandle_<0||f.file->topologyRead_.entered||!f.file->preflightNative(f.wholeRoleIdentity)||
       !f.sourceHandle1E0||*f.sourceHandle1E0<0||!sourceWaterProvider_||!sourceWaterProvider_->nativePool)return result;
    auto& pool=*sourceWaterProvider_->nativePool;
    if(!pool.nativeData||!pool.data1B4||pool.data1B4->offset||pool.data1B4->role!=&pool.nativeData->role||
       &pool.role!=d.poolRoleIdentity||pool.role.epoch!=d.poolEpoch)return result;
    auto& data=*pool.nativeData;
    if(!data.payload||&data.role!=d.dataRoleIdentity||data.role.epoch!=d.dataEpoch)return result;
    auto* payload=static_cast<std::uint8_t*>(data.payload.get());
    const bool positive=*d.signedCount>0;
    if(positive){
        if(*d.nonpositive||d.stop!=WaterPoolDataStop::childAllocation2BB1B0||d.pc!=0x2bb1b0u||
           materialCount!=1u||p.rawRetired||p.childRetired||p.ordinal!=0u||p.readerFlag!=1u||
           !data.nativeMaterials||!data.materials10||!data.nativeMaterials->firstChild||
           data.nativeMaterials->firstChild->role.ordinal!=0u||!m.returned||!m.frameReleased||
           m.stop!=WaterPoolMaterialReaderStop::callerOrdinal2BB220||m.pc!=0x2bb220u||m.callerPC!=0x2bb220u||
           m.word80!=0u||m.zero80!=true||!f.file->materialReader_.reads[17].resultAdmitted)return result;
    }else if(!*d.nonpositive||d.stop!=WaterPoolDataStop::topologyCall2BB24C||d.pc!=0x2bb24cu||
       !d.childPointerZeroRestored||!d.topologyAliasReloaded||!d.topologyHandleReloaded||materialCount||
       p.attempted||m.attempted||data.nativeMaterials||data.materials10||f.file->materialReader_.entered)return result;
    //The current-byte validator above authenticated all lanes; inspect the
    //actual known payload again before committing this new source load.
    for(std::size_t i=0;i<4u;++i)if(data.knownMask[0x1cu+i]!=255u||data.knownBits[0x1cu+i]||payload[0x1cu+i])return result;
    if(positive)for(std::size_t i=0;i<4u;++i)if(data.knownMask[0x14u+i]!=255u||
        payload[0x14u+i]!=d.countBytes[i]||data.knownBits[0x14u+i]!=d.countBytes[i])return result;
    n.attempted=true;n.entry=positive?K::returnedFirstMaterial:K::nonpositiveCount;
    n.poolRoleIdentity=&pool.role;n.poolEpoch=pool.role.epoch;n.dataRoleIdentity=&data.role;n.dataEpoch=data.role.epoch;
    n.fileIdentity=f.file.get();n.loaderFrameIdentity=&name;
    const auto sign=[](std::uint32_t w)noexcept {
        return w&0x80000000u?(0xffffffff00000000ull|std::uint64_t(w)):std::uint64_t(w);
    };
    if(positive){
        n.pc=0x2bb220u;n.ordinal=1u;n.ordinalIncremented=true;n.pc=0x2bb224u;n.ordinalNop=true;
        n.countLoadPC=n.pc=0x2bb228u;n.countLoadOffset=0x14u;std::uint32_t word=0;
        for(std::size_t i=0;i<4u;++i){n.currentCountMask[i]=data.knownMask[0x14u+i];
            n.currentCountBytes[i]=payload[0x14u+i];word|=std::uint32_t(n.currentCountBytes[i])<<(8u*i);}
        n.countLoaded=true;n.countWord=word;n.signedCount=word&0x80000000u?std::int64_t(word)-0x100000000ll:std::int64_t(word);
        n.countRegister=sign(word);n.pc=0x2bb22cu;n.compared=true;n.loopTaken=std::int64_t(*n.ordinal)<*n.signedCount;
        n.pc=0x2bb230u;n.loopBranch=true;n.pc=0x2bb234u;n.loopDelay=true;
        if(*n.loopTaken){n.stop=S::secondMaterialCall2BB1C4;n.pc=n.pendingCallPC=0x2bb1c4u;n.pendingDelayPC=0x2bb1c8u;
            return finish(Result::Status::sourceBoundary);} //No next allocation occurrence or topology receipt.
        n.pc=0x2bb238u;n.exitBranch=true;n.pc=0x2bb23cu;n.aliasLoadPC=n.pc;
        n.argumentData=pool.data1B4->role;n.aliasLoaded=true;
        n.pc=0x2bb248u;n.argumentHandle=f.sourceHandle1E0;n.handleReloaded=true;
    }else {
        //The accepted240/244/248 effects are inherited, NEVER replayed. In
        //particular s1 is still the construction Data role, not ordinal zero.
        n.pc=0x2bb24cu;n.argumentData=d.dataRoleIdentity;n.argumentHandle=f.sourceHandle1E0;
    }
    n.callPC=n.pc=0x2bb24cu;n.callIssued=true;n.callerReturnPC=0x2bb254u;
    n.delayPC=n.pc=0x2bb250u;n.callDelay=true;n.flag=name.argumentA3;
    n.entryPC=n.pc=0x2bab10u;n.frame=true;n.frameBytes=96u;
    n.pc=0x2bab14u;n.saves[0]={n.pc,0x40u,M::returnAddress,nullptr,std::uint64_t(n.callerReturnPC)};
    n.pc=0x2bab18u;n.saves[1]={n.pc,0x30u,M::handle,nullptr,std::uint64_t(*d.savedHandleS3)};
    n.pc=0x2bab1cu;n.saves[2]={n.pc,0x20u,M::dataRole,n.dataRoleIdentity,std::nullopt};
    n.pc=0x2bab20u;
    if(positive)n.saves[3]={n.pc,0x10u,M::ordinal,nullptr,std::uint64_t(*n.ordinal)};
    else n.saves[3]={n.pc,0x10u,M::dataRole,n.dataRoleIdentity,std::nullopt};
    n.pc=0x2bab24u;n.wholeSelected=true;
    n.pc=0x2bab28u;n.saves[4]={n.pc,0u,M::poolRole,n.poolRoleIdentity,std::nullopt};
    n.serviceLuiPC=n.pc=0x2bab2cu;auto& l=n.load2BAB30;
    l.attempted=true;l.pc=n.pc=0x2bab30u;n.stop=S::service2BAB30;
    if(!waterPoolFileServiceCurrentNative())return finish(Result::Status::nativeStopped);
    const auto& domain=sourceTrackMountDomain_;const auto& first=f.load2BB044;
    if(first.applicationIdentity!=sourceLoadApplication_||first.domainIdentity!=domain.get()||
       first.resolverIdentity!=domain->resolver3E1A70().get()||first.providerIdentity!=domain->archiveProvider3E1A70().get()||
       first.trackIdentity!=sourceTrackFile_.get())return finish(Result::Status::nativeStopped);
    const void* mountIdentity=nullptr;std::size_t mount=static_cast<std::size_t>(-1);std::uint64_t generation=0;
    if(!domain->resolver3E1A70()->waterPoolTrackRelationNative(sourceTrackFile_.get(),domain->archiveProvider3E1A70().get(),
       mountIdentity,mount,generation)||mountIdentity!=first.trackMountIdentity||mount!=first.trackMount||
       generation!=first.trackGeneration)return finish(Result::Status::nativeStopped);
    l.applicationIdentity=sourceLoadApplication_;l.domainIdentity=domain.get();l.resolverIdentity=domain->resolver3E1A70().get();
    l.providerIdentity=domain->archiveProvider3E1A70().get();l.trackIdentity=sourceTrackFile_.get();
    l.trackMountIdentity=mountIdentity;l.trackMount=mount;l.trackGeneration=generation;l.resolved=true;
    n.pc=0x2bab34u;n.destinationOffset=0x1cu;n.pc=0x2bab38u;n.itemSize=4u;
    n.pc=0x2bab3cu;n.itemCount=1u;n.argumentsPrepared=true;n.pc=0x2bab40u;n.stop=S::readPreparation;
    const bool admitted=f.file->readTopologyCount12FB30(n.dataRoleIdentity,n.dataEpoch,
        positive?F::TopologyEntryKind::returnedFirstMaterial:F::TopologyEntryKind::nonpositiveCount,n.diagnostic);
    const auto& r=f.file->topologyRead_;
    //Synchronize the genuine copied prefix, including native refusal, before
    //any later aggregate validation. The untouched known suffix stays zero.
    for(std::size_t i=0;i<std::min(r.copied,std::size_t(4u));++i){n.topologyBytes[i]=r.bytes[i];
        payload[0x1cu+i]=r.bytes[i];data.knownMask[0x1cu+i]=255u;data.knownBits[0x1cu+i]=r.bytes[i];}
    n.copied=r.copied;n.prefixSynchronized=true;n.readResultAdmitted=admitted;n.pc=r.pc?r.pc:0x2bab40u;
    if(!admitted){n.stop=!r.attempted?S::readPreparation:!r.nativeReturned?S::readHelper:S::readAdmission;
        return finish(Result::Status::nativeStopped);}
    //The return is admitted before this actual current payload LW. No count
    //guard or host signed shift/multiply is inserted into the source path.
    n.topologyLoadPC=n.pc=0x2bab48u;std::uint32_t word=0;
    for(std::size_t i=0;i<4u;++i)word|=std::uint32_t(payload[0x1cu+i])<<(8u*i);
    n.topologyWord=word;n.topologyLoaded=true;n.signedTopology=word&0x80000000u?
        std::int64_t(word)-0x100000000ll:std::int64_t(word);n.topologyRegister=sign(word);
    n.shiftPC=n.pc=0x2bab4cu;n.shiftWord=std::uint32_t(word<<2u);n.shiftRegister=sign(*n.shiftWord);n.shifted=true;
    n.sumPC=n.pc=0x2bab50u;n.sumWord=std::uint32_t(*n.shiftWord+word);n.sumRegister=sign(*n.sumWord);n.added=true;
    n.prospectiveRequestWord=std::uint32_t(*n.sumWord<<2u); //Mathematics only; delay58 is still pending.
    n.pc=n.pendingCallPC=0x2bab54u;n.pendingDelayPC=0x2bab58u;n.stop=S::mallocCall2BAB54;
    return finish(Result::Status::sourceBoundary);
    //No malloc call/delay, return, backing, R+28 store, guard or loader return.
}
