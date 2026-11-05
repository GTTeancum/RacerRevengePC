//Included inside ra_cold_aggregate50LA, after the immutable material prefix.
//No source/helper is executed as verification of this staged implementation.
namespace {
struct WaterMaterialReaderCase50LA {
    std::uint32_t target,setPC,branchPC,delayPC,storePC,word;bool fallthrough;
};
//Exact immutable words at3C11B0..3C11D8, with actual case control flow.
static constexpr std::array<WaterMaterialReaderCase50LA,11> waterMaterialReaderCases50LA{{
    {0x2bf95c,0,0x2bf95c,0x2bf960,0x2bf960,0,false},
    {0x2bf964,0x2bf964,0x2bf968,0x2bf96c,0x2bf96c,1,false},
    {0x2bf988,0x2bf988,0x2bf98c,0x2bf990,0x2bf990,1,false},
    {0x2bf970,0x2bf970,0x2bf974,0x2bf978,0x2bf978,1,false},
    {0x2bf97c,0x2bf97c,0x2bf980,0x2bf984,0x2bf984,1,false},
    {0x2bf994,0x2bf994,0x2bf998,0x2bf99c,0x2bf99c,1,false},
    {0x2bf9a0,0,0x2bf9a0,0x2bf9a4,0x2bf9a4,0,false},
    {0x2bf9bc,0x2bf9bc,0,0,0x2bf9c0,1,true},
    {0x2bf9a8,0x2bf9a8,0x2bf9ac,0x2bf9b0,0x2bf9b0,1,false},
    {0x2bf9c4,0,0,0,0,0,false}, //Selector9: no store, NOT a zero store.
    {0x2bf9b4,0,0x2bf9b4,0x2bf9b8,0x2bf9b8,0,false}
}};
}

bool Owner::preflightNativeWaterPoolMaterialReaderHistory(
    std::array<std::uint8_t,0xd8>& masks,std::array<std::uint8_t,0xd8>& bits)const noexcept {
    using S=WaterPoolMaterialReaderStop;
    const auto& n=sourceWaterConfig_.nativeWaterPoolMaterialReader;
    const auto& p=sourceWaterConfig_.nativeWaterPoolMaterial;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    const auto virginLoad=[](const SourceWaterConfig::NativeWaterPoolResource::ServiceLoad& l)noexcept {
        return !l.attempted&&!l.resolved&&!l.pc&&!l.applicationIdentity&&!l.domainIdentity&&!l.resolverIdentity&&
            !l.providerIdentity&&!l.trackIdentity&&!l.trackMountIdentity&&
            l.trackMount==static_cast<std::size_t>(-1)&&!l.trackGeneration;
    };
    const auto noReturn=[&]()noexcept {
        return n.restorePCs==std::array<std::uint32_t,4>{}&&!n.returnIssued&&!n.returnDelay&&!n.frameReleased&&
            !n.returned&&!n.returnPC&&!n.returnDelayPC&&!n.releasedBytes&&!n.callerPC&&!n.returnV0;
    };
    const auto noSwitch=[&]()noexcept {
        return !n.selectorLoaded&&!n.rangeCompared&&!n.rangeBranch&&!n.rangeDelay&&!n.selector&&!n.inRange&&
            !n.tableBaseFormed&&!n.selectorShifted&&!n.tableAddressFormed&&!n.tableLoaded&&!n.dispatchIssued&&
            !n.dispatchDelay&&!n.tableBase&&!n.tableAddress&&!n.tableTarget&&!n.caseSetPC&&!n.caseBranchPC&&
            !n.caseDelayPC&&!n.caseBranchTarget&&!n.caseFallthrough&&!n.b4Store&&noReturn();
    };
    const auto noGates=[&]()noexcept {
        return !n.word80Loaded&&!n.branch918&&!n.delay91C&&!n.branch920&&!n.delay924&&!n.word80&&
            !n.signedWord80&&!n.zero80&&!n.zeroFlag&&noSwitch();
    };
    if(!n.attempted){
        if(n.stop!=S::none||n.filePrepared||n.pc||n.fileIdentity||n.dataRoleIdentity||n.arrayRoleIdentity||
           n.materialRoleIdentity||n.entityIdentity||n.dataEpoch||n.arrayEpoch||n.materialEpoch||!noGates()||
           (f.file&&f.file->materialReader_.entered))return false;
        for(const auto& r:n.reads)if(r.reached||r.firstLoadRevalidated||r.argumentsInherited||r.argumentsPrepared||
            r.prefixSynchronized||!virginLoad(r.freshLoad)||r.luiPC||r.destinationPC||r.sizePC||r.countPC||
            r.destinationOffset||r.itemSize||r.itemCount||r.copied)return false;
        return true;
    }
    //Only recorded identities/epochs and byte values below. No material role,
    //payload, Entity, Pool, provider or full-floor dereference/callback here.
    if(!p.attempted||p.stop!=WaterPoolMaterialStop::readCall2BF730||p.pc!=0x2bf730u||
       !p.constructorReturned||!p.setupReturned||!p.load2BF71C.resolved||p.readerFlag!=1u||
       sourceWaterConfig_.nativeWaterPoolName.argumentA2!=1u||sourceWaterConfig_.nativeWaterPoolName.argumentA3!=1u||
       !f.file||n.fileIdentity!=f.file.get()||n.fileIdentity!=p.fileIdentity||
       n.dataRoleIdentity!=p.dataRoleIdentity||n.arrayRoleIdentity!=p.arrayRoleIdentity||
       n.materialRoleIdentity!=p.childRoleIdentity||n.entityIdentity!=p.entityIdentity||
       !n.dataRoleIdentity||!n.arrayRoleIdentity||!n.materialRoleIdentity||!n.entityIdentity||
       n.dataEpoch!=1u||n.arrayEpoch!=1u||n.materialEpoch!=1u||p.dataEpoch!=n.dataEpoch||
       p.arrayEpoch!=n.arrayEpoch||p.childEpoch!=n.materialEpoch||
       !f.file->historyNative(f.wholeRoleIdentity))return false;
    const auto& m=f.file->materialReader_;
    if(n.filePrepared!=m.prepared)return false;
    if(m.entered&&(m.materialIdentity!=n.materialRoleIdentity||m.arrayIdentity!=n.arrayRoleIdentity||
       m.dataIdentity!=n.dataRoleIdentity||m.materialEpoch!=n.materialEpoch||m.arrayEpoch!=n.arrayEpoch||
       m.dataEpoch!=n.dataEpoch))return false;
    const auto& schedule=a::OriginalWaterPoolSourceFile50LA::materialReadScheduleNative();
    std::size_t reached=0,admitted=0;bool terminal=false,refused=false;
    for(std::size_t i=0;i<n.reads.size();++i){const auto& r=n.reads[i];const auto& c=m.reads[i];const auto& s=schedule[i];
        if(!r.reached){
            if(r.firstLoadRevalidated||r.argumentsInherited||r.argumentsPrepared||r.prefixSynchronized||
               !virginLoad(r.freshLoad)||r.luiPC||r.destinationPC||r.sizePC||r.countPC||r.destinationOffset||
               r.itemSize||r.itemCount||r.copied||c.entered)return false;
            terminal=true;continue;
        }
        if(terminal||!n.filePrepared||admitted!=i)return false;++reached;
        bool loadReady=false;
        if(!i){
            if(!virginLoad(r.freshLoad)||r.luiPC)return false;
            loadReady=r.firstLoadRevalidated;
        }else {
            const auto& l=r.freshLoad;const auto& first=p.load2BF71C;
            if(r.firstLoadRevalidated||!l.attempted||l.pc!=s.serviceLoadPC||r.luiPC!=s.serviceLuiPC)return false;
            if(l.resolved){
                if(l.applicationIdentity!=first.applicationIdentity||l.domainIdentity!=first.domainIdentity||
                   l.resolverIdentity!=first.resolverIdentity||l.providerIdentity!=first.providerIdentity||
                   l.trackIdentity!=first.trackIdentity||l.trackMountIdentity!=first.trackMountIdentity||
                   l.trackMount!=first.trackMount||l.trackGeneration!=first.trackGeneration)return false;
            }else if(l.applicationIdentity||l.domainIdentity||l.resolverIdentity||l.providerIdentity||l.trackIdentity||
                l.trackMountIdentity||l.trackMount!=static_cast<std::size_t>(-1)||l.trackGeneration)return false;
            loadReady=l.resolved;
        }
        if(!loadReady){
            if(r.argumentsInherited||r.argumentsPrepared||r.prefixSynchronized||r.destinationPC||r.sizePC||r.countPC||
               r.destinationOffset||r.itemSize||r.itemCount||r.copied||c.entered||n.stop!=S::serviceLoad||
               n.pc!=(i?s.serviceLoadPC:s.callPC))return false;
            terminal=true;refused=true;continue;
        }
        if(!r.argumentsPrepared||r.argumentsInherited!=(i==0u)||r.destinationPC!=s.destinationPC||
           r.sizePC!=s.sizePC||r.countPC!=s.countPC||r.destinationOffset!=s.offset||r.itemSize!=s.size||
           r.itemCount!=1u||!c.entered||!r.prefixSynchronized||r.copied!=c.copied)return false;
        //Authenticate the mixed constructor preimage before overlay. Unknown
        //source lanes are never read, even when their bookkeeping bytes are0.
        for(std::size_t j=0;j<s.size;++j){const auto at=s.offset+j;
            if(masks[at]!=c.preimageMask[j]||bits[at]!=c.preimageBytes[j])return false;
            if(j<c.copied){masks[at]=255u;bits[at]=c.bytes[j];}
        }
        if(c.resultAdmitted)++admitted;
        else {
            const auto stop=!c.attempted?S::readPreparation:!c.nativeReturned?S::readHelper:S::readAdmission;
            if(n.stop!=stop||n.pc!=c.pc)return false;terminal=true;refused=true;
        }
    }
    if(!n.filePrepared)return !reached&&n.stop==S::receiptPreparation&&n.pc==0x2bf730u&&noGates();
    if(admitted!=18u)return reached&&refused&&noGates();
    if(reached!=18u||!n.word80Loaded||!n.branch918||!n.delay91C)return false;
    std::uint32_t word=0;for(std::size_t i=0;i<4u;++i){if(masks[0x80u+i]!=255u)return false;
        word|=std::uint32_t(bits[0x80u+i])<<(8u*i);}
    const auto sign=word&0x80000000u?(0xffffffff00000000ull|std::uint64_t(word)):std::uint64_t(word);
    if(n.word80!=word||n.signedWord80!=sign||n.zero80!=(word==0u))return false;
    if(word){
        return n.branch920&&n.delay924&&n.zeroFlag==false&&n.stop==S::textureCall2BF928&&
            n.pc==0x2bf928u&&noSwitch(); //Neither pending JAL nor its delay issued.
    }
    if(n.branch920||n.delay924||n.zeroFlag||!n.selectorLoaded||!n.rangeCompared||!n.rangeBranch||!n.rangeDelay||
       masks[0xacu]!=255u||masks[0xadu]!=255u)return false;
    const auto selector=std::uint16_t(std::uint16_t(bits[0xacu])|(std::uint16_t(bits[0xadu])<<8u));
    if(n.selector!=selector||n.inRange!=(selector<11u))return false;
    if(selector<11u){const auto& c=waterMaterialReaderCases50LA[selector];
        if(!n.tableBaseFormed||!n.selectorShifted||!n.tableAddressFormed||!n.tableLoaded||!n.dispatchIssued||
           !n.dispatchDelay||n.tableBase!=0x3c11b0u||n.tableAddress!=0x3c11b0u+4u*selector||n.tableTarget!=c.target||
           n.caseSetPC!=c.setPC||n.caseBranchPC!=c.branchPC||n.caseDelayPC!=c.delayPC||
           n.caseBranchTarget!=(c.branchPC?0x2bf9c4u:0u)||
           n.caseFallthrough!=c.fallthrough||bool(n.b4Store)!=(c.storePC!=0u))return false;
        if(n.b4Store){const auto& store=*n.b4Store;
            if(store.pc!=c.storePC||store.offset!=0xb4u||store.width!=4u||store.word!=c.word||store.mask!=255u)return false;
            for(std::size_t i=0;i<4u;++i){masks[0xb4u+i]=255u;bits[0xb4u+i]=std::uint8_t(c.word>>(8u*i));}
        }
    }else if(n.tableBaseFormed||n.selectorShifted||n.tableAddressFormed||n.tableLoaded||n.dispatchIssued||
        n.dispatchDelay||n.tableBase||n.tableAddress||n.tableTarget||n.caseSetPC||n.caseBranchPC||n.caseDelayPC||
        n.caseBranchTarget||n.caseFallthrough||n.b4Store)return false;
    return n.restorePCs==std::array<std::uint32_t,4>{{0x2bf9c4u,0x2bf9c8u,0x2bf9ccu,0x2bf9d0u}}&&
        n.returnIssued&&n.returnDelay&&n.frameReleased&&n.returned&&n.returnPC==0x2bf9d4u&&
        n.returnDelayPC==0x2bf9d8u&&n.releasedBytes==64u&&n.callerPC==0x2bb220u&&
        n.returnV0==m.reads[17].sourceItems&&n.stop==S::callerOrdinal2BB220&&n.pc==0x2bb220u;
}

Owner::WaterPoolMaterialReaderResult Owner::readWaterPoolMaterialNativeOnly(WaterArrayBackingMode mode)noexcept {
    using S=WaterPoolMaterialReaderStop;using Result=WaterPoolMaterialReaderResult;
    auto& n=sourceWaterConfig_.nativeWaterPoolMaterialReader;
    const auto& p=sourceWaterConfig_.nativeWaterPoolMaterial;const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    Result result;
    const auto finish=[&](Result::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.originalSourceStop=0x30492cu;result.materialPrefixStop=p.pc;
        result.rawAllocatorStop=p.raw.pc;result.childAllocatorStop=p.child.pc;result.adaptedPC=n.pc;
        result.word80=n.word80;result.selector=n.selector;result.tableTarget=n.tableTarget;
        if(n.b4Store){result.b4StorePC=n.b4Store->pc;result.b4Word=std::uint32_t(n.b4Store->word);}
        result.returned=n.returned;result.frameReleased=n.frameReleased;result.returnV0=n.returnV0;result.diagnostic=n.diagnostic;
        if(f.file)for(std::size_t i=0;i<18u;++i){const auto& c=f.file->materialReader_.reads[i];auto& r=result.reads[i];
            r.attempted=c.attempted;r.nativeReturned=c.nativeReturned;r.afterObserved=c.afterObserved;
            r.resultAdmitted=c.resultAdmitted;r.copied=c.copied;r.clampedAmount=c.clampedAmount;r.sourceItems=c.sourceItems;
            r.helperDiagnostic=c.helperDiagnostic;r.admissionDiagnostic=c.admissionDiagnostic;
            if(c.beforeObserved)r.beforeCursor=c.before.memberCursor10;if(c.afterObserved)r.afterCursor=c.after.memberCursor10;
            if(c.attempted)result.requestedBytes+=c.source.size;result.copiedBytes+=c.copied;
            if(c.resultAdmitted)++result.admittedReturns;
        }
        return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_)return result;
    std::size_t materialCount=0,poolCount=0;
    if(!sourceWaterPoolMaterialCountedNative(materialCount)||!sourceWaterPoolCountedNative(poolCount))return result;
    if(n.attempted)return finish(Result::Status::alreadyAttempted);
    if(!waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       materialCount!=1u||poolCount!=1u||p.stop!=WaterPoolMaterialStop::readCall2BF730||p.pc!=0x2bf730u||
       p.childRetired||p.rawRetired||p.readerFlag!=1u||!f.file||f.file->nativeClosed()||
       !f.file->preflightNative(f.wholeRoleIdentity)||f.file->nativeHandle_<0||
       !sourceWaterProvider_||!sourceWaterProvider_->nativePool||!sourceWaterProvider_->nativePool->nativeData)return result;
    auto& data=*sourceWaterProvider_->nativePool->nativeData;
    if(!data.nativeMaterials||!data.nativeMaterials->firstChild)return result;
    auto& array=*data.nativeMaterials;auto& material=*array.firstChild;
    n.attempted=true;n.stop=S::receiptPreparation;n.pc=0x2bf730u;n.fileIdentity=f.file.get();
    n.dataRoleIdentity=&data.role;n.arrayRoleIdentity=&array.role;n.materialRoleIdentity=&material.role;
    n.entityIdentity=&material.entity;n.dataEpoch=data.role.epoch;n.arrayEpoch=array.role.epoch;n.materialEpoch=material.role.epoch;
    n.filePrepared=f.file->beginMaterialReaderNative(n.materialRoleIdentity,n.arrayRoleIdentity,n.dataRoleIdentity,
        n.materialEpoch,n.arrayEpoch,n.dataEpoch,n.diagnostic);
    if(!n.filePrepared)return finish(Result::Status::nativeStopped);
    const auto& schedule=a::OriginalWaterPoolSourceFile50LA::materialReadScheduleNative();
    auto* bytes=static_cast<std::uint8_t*>(material.payload.get());
    //Native bounded traversal of eighteen distinct straight-line call sites,
    //not an invented guest loop. Each later site freshly re-resolves3E1A70.
    for(std::size_t i=0;i<18u;++i){auto& r=n.reads[i];const auto& s=schedule[i];
        r.reached=true;n.stop=S::serviceLoad;
        if(i){r.luiPC=s.serviceLuiPC;n.pc=s.serviceLuiPC;r.freshLoad.attempted=true;
            r.freshLoad.pc=n.pc=s.serviceLoadPC;}
        else n.pc=s.callPC; //Native revalidation, NOT a second source2BF71C load.
        const auto& first=p.load2BF71C;
        if(!waterPoolFileServiceCurrentNative())return finish(Result::Status::nativeStopped);
        const auto& domain=sourceTrackMountDomain_;
        if(first.applicationIdentity!=sourceLoadApplication_||first.domainIdentity!=domain.get()||
           first.resolverIdentity!=domain->resolver3E1A70().get()||first.providerIdentity!=domain->archiveProvider3E1A70().get()||
           first.trackIdentity!=sourceTrackFile_.get())return finish(Result::Status::nativeStopped);
        const void* mountIdentity=nullptr;std::size_t mount=static_cast<std::size_t>(-1);std::uint64_t generation=0;
        if(!domain->resolver3E1A70()->waterPoolTrackRelationNative(sourceTrackFile_.get(),domain->archiveProvider3E1A70().get(),
           mountIdentity,mount,generation)||mountIdentity!=first.trackMountIdentity||mount!=first.trackMount||
           generation!=first.trackGeneration)return finish(Result::Status::nativeStopped);
        if(!i)r.firstLoadRevalidated=true;
        else {auto& l=r.freshLoad;l.applicationIdentity=sourceLoadApplication_;l.domainIdentity=domain.get();
            l.resolverIdentity=domain->resolver3E1A70().get();l.providerIdentity=domain->archiveProvider3E1A70().get();
            l.trackIdentity=sourceTrackFile_.get();l.trackMountIdentity=mountIdentity;l.trackMount=mount;
            l.trackGeneration=generation;l.resolved=true;}
        //For0 these arguments were already established at724/728/72C in the
        //predecessor. Later triples are fresh reached source instructions.
        r.argumentsInherited=i==0u;r.destinationPC=s.destinationPC;r.sizePC=s.sizePC;r.countPC=s.countPC;
        r.destinationOffset=s.offset;r.itemSize=s.size;r.itemCount=1u;r.argumentsPrepared=true;
        if(i){n.pc=s.destinationPC;n.pc=s.sizePC;n.pc=s.countPC;}
        n.pc=s.callPC;n.stop=S::readPreparation;
        const bool admitted=f.file->readMaterial12FB30(i,n.diagnostic);
        const auto& c=f.file->materialReader_.reads[i];
        //Complete both sides before any aggregate validation. Preserve every
        //actual copied byte even on a native failure; uncopied source lanes
        //are not read, cleared or restored. Metadata zero is never a write.
        for(std::size_t j=0;j<std::min(c.copied,std::size_t(s.size));++j){const auto at=s.offset+j;
            bytes[at]=c.bytes[j];material.knownMask[at]=255u;material.knownBits[at]=c.bytes[j];}
        r.copied=c.copied;r.prefixSynchronized=true;n.pc=c.pc;
        if(!admitted){n.stop=!c.attempted?S::readPreparation:!c.nativeReturned?S::readHelper:S::readAdmission;
            return finish(Result::Status::nativeStopped);}
    }
    //Initialized +80 and +AC remain wholly known after partial/EOF overlays.
    //No name, unknown scalar suffix, float conversion or texture data is read.
    n.pc=0x2bf914u;std::uint32_t word=0;
    for(std::size_t i=0;i<4u;++i)word|=std::uint32_t(bytes[0x80u+i])<<(8u*i);
    n.word80=word;n.signedWord80=word&0x80000000u?(0xffffffff00000000ull|std::uint64_t(word)):std::uint64_t(word);
    n.word80Loaded=true;n.pc=0x2bf918u;n.branch918=true;n.zero80=word==0u;n.pc=0x2bf91cu;n.delay91C=true;
    if(word){
        n.pc=0x2bf920u;n.branch920=true;n.zeroFlag=*p.readerFlag==0u;n.pc=0x2bf924u;n.delay924=true;
        n.pc=0x2bf928u;n.stop=S::textureCall2BF928;
        return finish(Result::Status::sourceBoundary); //JAL and delay2BF92C BOTH pending.
    }
    n.pc=0x2bf930u;n.selector=std::uint16_t(std::uint16_t(bytes[0xacu])|
        (std::uint16_t(bytes[0xadu])<<8u));n.selectorLoaded=true;
    n.pc=0x2bf934u;n.inRange=*n.selector<11u;n.rangeCompared=true;
    n.pc=0x2bf938u;n.rangeBranch=true;n.pc=0x2bf93cu;n.rangeDelay=true;
    if(*n.inRange){
        n.pc=0x2bf940u;n.pc=0x2bf944u;n.tableBase=0x3c11b0u;n.tableBaseFormed=true;
        n.pc=0x2bf948u;n.selectorShifted=true;n.pc=0x2bf94cu;n.tableAddress=0x3c11b0u+4u * *n.selector;
        n.tableAddressFormed=true;const auto& c=waterMaterialReaderCases50LA[*n.selector];
        n.pc=0x2bf950u;n.tableTarget=c.target;n.tableLoaded=true;n.pc=0x2bf954u;n.dispatchIssued=true;
        n.pc=0x2bf958u;n.dispatchDelay=true;n.pc=c.target;
        if(c.setPC){n.pc=c.setPC;n.caseSetPC=c.setPC;}
        if(c.branchPC){n.pc=c.branchPC;n.caseBranchPC=c.branchPC;n.caseBranchTarget=0x2bf9c4u;
            n.pc=c.delayPC;n.caseDelayPC=c.delayPC;}
        if(c.storePC){n.pc=c.storePC;n.b4Store=SourceWaterConfig::NativeWaterPoolMaterial::Store{c.storePC,0xb4u,4u,c.word,255u};
            for(std::size_t i=0;i<4u;++i){const auto value=std::uint8_t(c.word>>(8u*i));
                bytes[0xb4u+i]=value;material.knownMask[0xb4u+i]=255u;material.knownBits[0xb4u+i]=value;}}
        n.caseFallthrough=c.fallthrough;
    }
    n.pc=0x2bf9c4u;n.restorePCs[0]=n.pc;n.pc=0x2bf9c8u;n.restorePCs[1]=n.pc;
    n.pc=0x2bf9ccu;n.restorePCs[2]=n.pc;n.pc=0x2bf9d0u;n.restorePCs[3]=n.pc;
    n.pc=0x2bf9d4u;n.returnPC=n.pc;n.returnIssued=true;n.pc=0x2bf9d8u;n.returnDelayPC=n.pc;
    n.returnDelay=true;n.releasedBytes=64u;n.frameReleased=true;n.returned=true;
    n.returnV0=f.file->materialReader_.reads[17].sourceItems;
    n.pc=n.callerPC=0x2bb220u;n.stop=S::callerOrdinal2BB220;
    return finish(Result::Status::sourceBoundary);
    //ADDIU ordinal at2BB220 remains pending. No next child or loop control.
}
