//A distinct WaterObjectData resource. Original allocation remains stopped;
//the explicit native-success adapter never returns the original heap call.
bool Owner::preflightNativeWaterPoolDataHistory()const noexcept {
    using N=SourceWaterConfig::NativeWaterPoolData;using S=WaterPoolDataStop;
    using G=a::OriginalConfigProcessGlobals50LA;using R=N::Allocation::ReentStop;
    const auto& n=sourceWaterConfig_.nativeWaterPoolData;const auto& x=n.allocator;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    const auto& b=sourceWaterConfig_.nativeWaterPoolBacking;
    //Validate the descendant even on an early refusal/retired path. This
    //leaf reads retained values/opaque identities only, never this validator
    //or a material/Pool/provider/World count callback.
    std::array<std::uint8_t,4> topologyBytes{};
    if(!preflightNativeWaterPoolTopologyHistory(topologyBytes)||!preflightNativeWaterPoolTopologyBacking()||!preflightNativeWaterPoolIndices())return false;
    const auto virginLoad=[](const SourceWaterConfig::NativeWaterPoolResource::ServiceLoad& l)noexcept {
        return !l.attempted&&!l.resolved&&!l.pc&&!l.applicationIdentity&&!l.domainIdentity&&
            !l.resolverIdentity&&!l.providerIdentity&&!l.trackIdentity&&!l.trackMountIdentity&&
            l.trackMount==static_cast<std::size_t>(-1)&&!l.trackGeneration;
    };
    const auto noArena=[&]()noexcept {
        return !x.allocatorFrame&&!x.allocatorSaveMask&&!x.reentCopied&&!x.lockCalled&&!x.lockReturned&&
            !x.sumWord&&!x.normalizedChunk&&!x.arenaBase&&!x.arenaReadOffset&&!x.arenaReadAddress&&
            !x.sumRegister&&!x.normalizedRegister&&!x.minimumChunk&&!x.smallBin;
    };
    const auto noBranch=[&]()noexcept {
        return !n.countLoaded&&!n.countBranchExecuted&&!n.countBranchDelay&&!n.countWord&&!n.signedCount&&
            !n.nonpositive&&!n.childPointerZeroRestored&&!n.topologyAliasReloaded&&!n.topologyHandleReloaded;
    };
    const auto noRead=[&]()noexcept {
        return !n.callerReadIssued&&!n.callerReadDelay&&!n.readResultAdmitted&&!n.destinationOffset&&
            !n.itemSize&&!n.itemCount&&!n.copiedCountBytes&&
            n.countBytes==std::array<std::uint8_t,4>{}&&noBranch();
    };
    const auto noConstruct=[&]()noexcept {
        return !n.nativeAllocated&&!n.nativeRetired&&!n.aliasRevoked&&!n.referenceRetired&&
            !n.dataRoleIdentity&&!n.dataEpoch&&!n.callerEntered&&!n.nonnullBranch&&!n.baseCallIssued&&
            !n.baseDispatchStored&&!n.referenceAcquired&&!n.baseReturned&&!n.wholeSelected&&
            !n.aliasStored&&!n.aliasReloaded&&!n.derivedStores&&!n.baseReturnPC&&
            !n.resourceArgumentOffset&&!n.aliasOffset&&!n.savedHandleS3&&virginLoad(n.load2BB18C)&&noRead();
    };
    const auto noNative=[&]()noexcept {
        return !n.nativeAdmissionReent&&!n.ownerAttempted&&!n.ownerPrepared&&!n.payloadAttempted&&
            !n.temporaryOwnerRetired&&noConstruct();
    };
    const auto absentData=[&]()noexcept {
        return !sourceWaterProvider_||!sourceWaterProvider_->nativePool||
            (!sourceWaterProvider_->nativePool->nativeData&&!sourceWaterProvider_->nativePool->data1B4);
    };
    if(!n.attempted)return n.stop==S::none&&!n.pc&&!n.poolRoleIdentity&&!n.poolEntityIdentity&&
        !n.fileIdentity&&!n.poolEpoch&&!x.attempted&&!x.callIssued&&!x.pc&&!x.requestedBytes&&
        !x.effectiveBytes&&!x.callerReturnPC&&!x.argumentA0&&!x.effectiveArgument&&!x.operatorFrame&&
        !x.operatorFramePointer&&!x.nonzeroBranch&&!x.mallocFrame&&!x.operatorSaveMask&&
        !x.mallocSaveMask&&x.reentStop==R::none&&!x.reent&&noArena()&&noNative()&&absentData();
    if(!f.attempted||f.stop!=WaterPoolResourceStop::resourceAllocation2BB12C||f.pc!=0x2bb12cu||
       !f.file||f.versionWord!=2u||f.versionTwo!=true||f.allocationArgument!=0x30u||
       !f.readResultAdmitted||!f.sourceHandle1E0||*f.sourceHandle1E0<0||
       n.poolRoleIdentity!=f.wholeRoleIdentity||n.poolEntityIdentity!=f.canonicalEntityIdentity||
       n.fileIdentity!=f.file.get()||!n.poolRoleIdentity||!n.poolEntityIdentity||n.poolEpoch!=b.epoch||
       !b.nativeAllocated||!x.attempted||!x.callIssued||x.requestedBytes!=0x30u||x.argumentA0!=0x30ull||
       x.effectiveBytes!=x.requestedBytes||x.effectiveArgument!=x.argumentA0||x.callerReturnPC!=0x2bb134u||
       !x.operatorFrame||x.operatorSaveMask!=7u||!x.operatorFramePointer||!x.nonzeroBranch||!x.mallocFrame)return false;
    if(x.reentStop!=R::none){
        if(n.stop!=S::sourceReentrancy||n.pc!=0x2bb12cu||x.pc!=0x30570cu||x.mallocSaveMask||
           !noArena()||!noNative()||!absentData())return false;
        return x.reentStop==R::unknown?!x.reent:
            x.reentStop==R::unsupportedIdentity&&x.reent&&*x.reent!=G::StdioIdentity::reent385160;
    }
    if(x.pc!=0x3059ecu||x.reent!=G::StdioIdentity::reent385160||x.mallocSaveMask!=1u||
       !x.allocatorFrame||x.allocatorSaveMask!=63u||!x.reentCopied||x.sumWord!=0x43u||x.sumRegister!=0x43ull||
       x.minimumChunk!=false||x.normalizedChunk!=0x40u||x.normalizedRegister!=0x40ull||
       !x.lockCalled||!x.lockReturned||x.smallBin!=true||x.arenaBase!=0x385468u||
       x.arenaReadOffset!=0x4cu||x.arenaReadAddress!=0x3854b4u)return false;
    if(n.stop==S::nativeReentrancy)return n.pc==0x2bb12cu&&
        (!n.nativeAdmissionReent||*n.nativeAdmissionReent!=G::StdioIdentity::reent385160)&&
        !n.ownerAttempted&&!n.ownerPrepared&&!n.payloadAttempted&&!n.temporaryOwnerRetired&&noConstruct()&&absentData();
    if(n.nativeAdmissionReent!=G::StdioIdentity::reent385160||!n.ownerAttempted)return false;
    if(n.stop==S::ownerAllocation)return n.pc==0x2bb12cu&&!n.ownerPrepared&&!n.payloadAttempted&&
        !n.temporaryOwnerRetired&&noConstruct()&&absentData();
    if(!n.ownerPrepared||!n.payloadAttempted)return false;
    if(n.stop==S::payloadAllocation)return n.pc==0x2bb12cu&&n.temporaryOwnerRetired&&noConstruct()&&absentData();
    if(n.temporaryOwnerRetired||!n.nativeAllocated||!n.dataRoleIdentity||n.dataEpoch!=1u||
       !n.callerEntered||!n.nonnullBranch||!n.baseCallIssued||!n.baseDispatchStored||!n.referenceAcquired||
       !n.baseReturned||n.baseReturnPC!=0x2bb148u||n.resourceArgumentOffset!=0u||n.derivedStores!=8u||
       !n.wholeSelected||!n.aliasStored||n.aliasOffset!=0x1b4u||!n.aliasReloaded||
       n.savedHandleS3!=f.sourceHandle1E0||!n.load2BB18C.attempted||n.load2BB18C.pc!=0x2bb18cu||
       n.referenceRetired!=n.nativeRetired||n.aliasRevoked!=n.nativeRetired||
       (b.nativeRetired&&!n.nativeRetired)||n.copiedCountBytes>4u)return false;
    //Retired identities are never dereferenced. The source reference and
    //constructor writes stay historical; native retirement is separate.
    const auto& material=sourceWaterConfig_.nativeWaterPoolMaterial;
    if(n.nativeRetired){if(!f.nativeWithdrawn||!f.file->nativeClosed()||!absentData()||
        (material.rawAllocated&&!material.rawRetired))return false;}
    else {
        if(!sourceWaterProvider_||!sourceWaterProvider_->nativePool)return false;
        const auto& p=*sourceWaterProvider_->nativePool;
        if(n.poolRoleIdentity!=&p.role||n.poolEntityIdentity!=&p.entity||!p.nativeData||!p.data1B4)return false;
        const auto& d=*p.nativeData;
        if(!d.payload||d.payload.get()==p.payload.get()||&d.role!=n.dataRoleIdentity||
           d.role.nativeOwner!=&d||d.role.poolOwner!=&p||d.role.payloadIdentity!=d.payload.get()||
           d.role.epoch!=*n.dataEpoch||d.reference00!=1u||p.data1B4->role!=&d.role||p.data1B4->offset)return false;
        //The actual2BB1B8 semantic alias supersedes the constructor's known
        //zero without exposing host pointer bits. Revocation stays unknown.
        if(material.arrayAliasStored){
            if(!material.attempted||!material.rawAllocated||material.dataRoleIdentity!=&d.role||
               material.dataEpoch!=d.role.epoch||material.originalCount!=n.countWord||
               !material.arrayRoleIdentity||material.arrayEpoch!=1u||material.arrayAliasRevoked!=material.rawRetired)return false;
            if(material.rawRetired){if(d.nativeMaterials||d.materials10)return false;}
            else if(!d.nativeMaterials||!d.materials10||d.materials10->role!=&d.nativeMaterials->role||
                d.materials10->offset||material.arrayRoleIdentity!=&d.nativeMaterials->role||
                d.nativeMaterials->role.nativeOwner!=d.nativeMaterials.get()||
                d.nativeMaterials->role.dataOwner!=&d||d.nativeMaterials->role.epoch!=1u)return false;
        }else if(d.nativeMaterials||d.materials10||material.rawAllocated||material.rawRetired||material.arrayAliasRevoked)return false;
        const auto* bytes=static_cast<const std::uint8_t*>(d.payload.get());
        for(std::size_t i=0;i<0x30u;++i){
            const bool known=i<12u||(i>=0x10u&&i<0x2cu&&!(material.arrayAliasStored&&i<0x14u)&&
                !(sourceWaterConfig_.nativeWaterPoolTopologyBacking.aliasStored&&i>=0x28u)&&
                !(sourceWaterConfig_.nativeWaterPoolIndices.aliasStored&&i>=0x20u&&i<0x24u));std::uint8_t bits=0;
            if(i<8u)bits=i==0u?1u:0u;
            else if(i<12u)bits=std::uint8_t(0x3d48a0u>>((i-8u)*8u));
            else if(i>=0x14u&&i<0x18u)bits=n.countBytes[i-0x14u];
            else if(i>=0x18u&&i<0x1cu)bits=std::uint8_t(0x22bu>>((i-0x18u)*8u));
            else if(i>=0x1cu&&i<0x20u)bits=topologyBytes[i-0x1cu];
            else if(i>=0x24u&&i<0x28u)bits=std::uint8_t(0x3f800000u>>((i-0x24u)*8u));
            if(d.knownMask[i]!=(known?0xffu:0u)||d.knownBits[i]!=bits||(known&&bytes[i]!=bits))return false;
        }
    }
    const auto& l=n.load2BB18C;const auto& first=f.load2BB044;
    if(!l.resolved){
        return n.stop==S::service2BB18C&&n.pc==0x2bb18cu&&!l.applicationIdentity&&!l.domainIdentity&&
            !l.resolverIdentity&&!l.providerIdentity&&!l.trackIdentity&&!l.trackMountIdentity&&
            l.trackMount==static_cast<std::size_t>(-1)&&!l.trackGeneration&&noRead()&&!f.file->countRead_.entered;
    }
    if(l.applicationIdentity!=first.applicationIdentity||l.domainIdentity!=first.domainIdentity||
       l.resolverIdentity!=first.resolverIdentity||l.providerIdentity!=first.providerIdentity||
       l.trackIdentity!=first.trackIdentity||l.trackMountIdentity!=first.trackMountIdentity||
       l.trackMount!=first.trackMount||l.trackGeneration!=first.trackGeneration||
       !n.callerReadIssued||!n.callerReadDelay||n.destinationOffset!=0x14u||n.itemSize!=4u||n.itemCount!=1u)return false;
    const auto& read=f.file->countRead_;
    if(read.entered&&read.resourceIdentity!=n.dataRoleIdentity)return false;
    if(n.readResultAdmitted!=read.resultAdmitted||n.copiedCountBytes!=read.copied)return false;
    for(std::size_t i=0;i<4u;++i)if(n.countBytes[i]!=(i<read.copied?read.bytes[i]:0u))return false;
    if(!n.readResultAdmitted)return n.stop==S::childCountRead&&noBranch();
    if(!read.entered||!read.attempted||!read.nativeReturned||!read.sourceItems||
       !n.countLoaded||!n.countBranchExecuted||!n.countBranchDelay||!n.countWord||!n.signedCount||!n.nonpositive)return false;
    const auto word=std::uint32_t(n.countBytes[0])|(std::uint32_t(n.countBytes[1])<<8u)|
        (std::uint32_t(n.countBytes[2])<<16u)|(std::uint32_t(n.countBytes[3])<<24u);
    const auto signedWord=word&0x80000000u?std::int64_t(word)-0x100000000ll:std::int64_t(word);
    if(n.countWord!=word||n.signedCount!=signedWord||n.nonpositive!=(signedWord<=0))return false;
    if(signedWord>0)return n.stop==S::childAllocation2BB1B0&&n.pc==0x2bb1b0u&&
        !n.childPointerZeroRestored&&!n.topologyAliasReloaded&&!n.topologyHandleReloaded;
    return n.stop==S::topologyCall2BB24C&&n.pc==0x2bb24cu&&n.childPointerZeroRestored&&
        n.topologyAliasReloaded&&n.topologyHandleReloaded;
}

bool Owner::withdrawNativeWaterPoolData()noexcept {
    auto& n=sourceWaterConfig_.nativeWaterPoolData;
    if(!preflightNativeWaterPoolDataHistory()||!withdrawNativeWaterPoolIndices()||!withdrawNativeWaterPoolTopologyBacking()||
       !withdrawNativeWaterPoolMaterial())return false;
    if(!n.nativeAllocated||n.nativeRetired)return true;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    if(!f.nativeWithdrawn||!f.file||!f.file->nativeClosed()||!sourceWaterProvider_||
       !sourceWaterProvider_->nativePool)return false;
    auto& p=*sourceWaterProvider_->nativePool;auto& d=*p.nativeData;
    //Every actually owned child and raw-array alias was retired above. No
    //source release/destructor: unknown slots make original2BA970 unsafe.
    p.data1B4.reset();n.aliasRevoked=true;
    --d.reference00;n.referenceRetired=true;
    p.nativeData.reset();n.nativeRetired=true;
    return true;
}

Owner::WaterPoolDataResult Owner::constructWaterPoolDataNativeOnly(WaterArrayBackingMode mode)noexcept {
    using N=SourceWaterConfig::NativeWaterPoolData;using S=WaterPoolDataStop;
    using G=a::OriginalConfigProcessGlobals50LA;using R=N::Allocation::ReentStop;
    auto& n=sourceWaterConfig_.nativeWaterPoolData;auto& x=n.allocator;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;WaterPoolDataResult result;
    const auto finish=[&](WaterPoolDataResult::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.originalSourceStop=0x30492cu;
        result.priorPoolAllocatorStop=sourceWaterConfig_.nativeWaterPoolAllocationEntry.pc;
        result.fileStop=f.pc;result.allocatorStop=x.pc;result.adaptedPC=n.pc;
        result.requestedBytes=x.requestedBytes;result.normalizedChunk=x.normalizedChunk;
        result.pendingArenaAddress=x.arenaReadAddress;result.countWord=n.countWord;result.signedCount=n.signedCount;
        result.nativeAllocated=n.nativeAllocated;result.referenceAcquired=n.referenceAcquired;
        result.aliasStored=n.aliasStored;result.readResultAdmitted=n.readResultAdmitted;
        result.copiedCountBytes=n.copiedCountBytes;result.nativeRetired=n.nativeRetired;result.aliasRevoked=n.aliasRevoked;
        result.diagnostic=n.diagnostic;
        if(n.referenceAcquired)for(std::size_t i=0;i<4u;++i)result.countBytes[i]=n.countBytes[i];
        if(f.file&&f.file->countRead_.entered){const auto& read=f.file->countRead_;
            result.readAttempted=read.attempted;result.readNativeReturned=read.nativeReturned;result.readSourceItems=read.sourceItems;}
        return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_)return result;
    std::size_t count=0;if(!sourceWaterPoolCountedNative(count))return result;
    if(n.attempted)return finish(WaterPoolDataResult::Status::alreadyAttempted);
    if(!waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||
       !preflightSourceWorldParticleCountNative()||count!=1u||!sourceWaterProvider_||
       !sourceWaterProvider_->nativePool||f.stop!=WaterPoolResourceStop::resourceAllocation2BB12C||
       f.pc!=0x2bb12cu||f.versionWord!=2u||f.versionTwo!=true||f.allocationArgument!=0x30u||
       !f.readResultAdmitted||!f.file||!f.file->preflightNative(f.wholeRoleIdentity)||f.file->nativeClosed()||
       f.file->nativeHandle_==-1||f.file->countRead_.entered)return result;
    auto& p=*sourceWaterProvider_->nativePool;
    if(p.nativeData||p.data1B4||f.wholeRoleIdentity!=&p.role||f.canonicalEntityIdentity!=&p.entity)return result;
    n.attempted=true;n.pc=0x2bb12cu;n.poolRoleIdentity=&p.role;n.poolEntityIdentity=&p.entity;
    n.fileIdentity=f.file.get();n.poolEpoch=p.role.epoch;
    x.attempted=true;x.callIssued=true;x.pc=0x2bb12c;x.requestedBytes=0x30u;x.argumentA0=0x30ull;
    x.callerReturnPC=0x2bb134u;x.pc=0x2bb130; //NOP delay. Taken version path never executes2BB128.
    x.pc=0x2e8d90;x.operatorFrame=true;x.pc=0x2e8d94;x.operatorSaveMask|=1u;
    x.pc=0x2e8d98;x.operatorSaveMask|=2u;x.pc=0x2e8d9c;x.operatorSaveMask|=4u;
    x.pc=0x2e8da0;x.effectiveBytes=x.requestedBytes;x.effectiveArgument=x.argumentA0;
    x.pc=0x2e8da4;x.nonzeroBranch=true;x.pc=0x2e8da8;x.operatorFramePointer=true;
    x.pc=0x2e8e1c;x.pc=0x2e8e20;x.pc=0x2e8e24;
    x.pc=0x305700;x.pc=0x305704;x.pc=0x305708;x.mallocFrame=true;x.pc=0x30570c;
    n.stop=S::sourceReentrancy;
    if(!G::reentrancyCurrentNative()){x.reentStop=R::unknown;return finish(WaterPoolDataResult::Status::nativeStopped);}
    x.reent=G::state().reent38544C;
    if(*x.reent!=G::StdioIdentity::reent385160){x.reentStop=R::unsupportedIdentity;
        return finish(WaterPoolDataResult::Status::nativeStopped);}
    x.pc=0x305710;x.mallocSaveMask=1u;x.pc=0x305714;x.pc=0x305718;
    x.pc=0x305990;x.allocatorFrame=true;x.pc=0x305994;x.sumWord=0x43u;x.sumRegister=0x43ull;
    x.pc=0x305998;x.allocatorSaveMask|=1u;x.pc=0x30599c;x.minimumChunk=false;
    x.pc=0x3059a0;x.allocatorSaveMask|=2u;x.pc=0x3059a4;x.reentCopied=true;
    x.pc=0x3059a8;x.allocatorSaveMask|=4u;x.pc=0x3059ac;x.allocatorSaveMask|=8u;
    x.pc=0x3059b0;x.allocatorSaveMask|=16u;x.pc=0x3059b4;x.pc=0x3059b8;x.allocatorSaveMask|=32u;
    x.pc=0x3059bc;x.pc=0x3059c0;x.pc=0x3059c4;x.pc=0x3059c8;
    x.normalizedChunk=0x40u;x.normalizedRegister=0x40ull;
    x.pc=0x3059d0;x.lockCalled=true;x.pc=0x3059d4;x.pc=0x306538;x.pc=0x30653c;x.lockReturned=true;
    x.pc=0x3059d8;x.smallBin=true;x.pc=0x3059dc;x.pc=0x3059e0;x.pc=0x3059e4;x.arenaBase=0x385468u;
    x.pc=0x3059e8;x.arenaReadOffset=0x4cu;x.arenaReadAddress=0x3854b4u;
    x.pc=0x3059ec; //Immutable original frontier, BEFORE arena LW.
    n.stop=S::nativeReentrancy;
    if(!G::reentrancyCurrentNative())return finish(WaterPoolDataResult::Status::nativeStopped);
    n.nativeAdmissionReent=G::state().reent38544C;
    if(*n.nativeAdmissionReent!=G::StdioIdentity::reent385160)return finish(WaterPoolDataResult::Status::nativeStopped);
    n.stop=S::ownerAllocation;n.ownerAttempted=true;
    std::unique_ptr<SourceWaterPoolDataOwner> data(new(std::nothrow) SourceWaterPoolDataOwner);
    if(!data)return finish(WaterPoolDataResult::Status::nativeStopped);
    n.ownerPrepared=true;n.stop=S::payloadAllocation;n.payloadAttempted=true;
    data->payload.reset(std::malloc(0x30u)); //Exact scalar bytes; neither normalized chunk nor blanket zero.
    if(!data->payload){data.reset();n.temporaryOwnerRetired=true;return finish(WaterPoolDataResult::Status::nativeStopped);}
    data->role.nativeOwner=data.get();data->role.poolOwner=&p;data->role.payloadIdentity=data->payload.get();data->role.epoch=1u;
    p.nativeData=std::move(data);auto& d=*p.nativeData;
    n.nativeAllocated=true;n.dataEpoch=1u;n.dataRoleIdentity=&d.role;
    auto* bytes=static_cast<std::uint8_t*>(d.payload.get());
    const auto store=[&](std::size_t offset,std::size_t width,std::uint64_t word)noexcept {
        for(std::size_t i=0;i<width;++i){const auto byte=std::uint8_t(word>>(i*8u));
            bytes[offset+i]=byte;d.knownMask[offset+i]=0xffu;d.knownBits[offset+i]=byte;}
    };
    //Private nonnull backing selects this adapted caller. No source heap
    //return/NULL branch/allocator frame return is reported by these stores.
    n.pc=0x2bb134;n.callerEntered=true;n.nonnullBranch=true;n.pc=0x2bb138;
    n.pc=0x2bb13c;n.pc=0x2bb140;n.baseCallIssued=true;n.baseReturnPC=0x2bb148u;
    n.pc=0x2bb144;n.resourceArgumentOffset=0u;n.pc=0x1048f0;n.pc=0x1048f4;
    n.pc=0x1048f8;store(8u,4u,0x3cc980u);n.baseDispatchStored=true;n.pc=0x1048fc;
    n.pc=0x104900;store(0u,8u,1u);d.reference00=1u;n.referenceAcquired=true;
    n.pc=0x104904;n.pc=0x104908;n.baseReturned=true;
    n.pc=0x2bb148;n.pc=0x2bb14c;n.pc=0x2bb150;store(8u,4u,0x3d48a0u);++n.derivedStores;
    n.pc=0x2bb154;store(0x10u,4u,0u);++n.derivedStores;n.pc=0x2bb158;
    n.pc=0x2bb15c;store(0x14u,4u,0u);++n.derivedStores;
    n.pc=0x2bb160;store(0x18u,4u,0x22bu);++n.derivedStores;
    n.pc=0x2bb164;store(0x1cu,4u,0u);++n.derivedStores;
    n.pc=0x2bb168;store(0x28u,4u,0u);++n.derivedStores;
    n.pc=0x2bb16c;store(0x20u,4u,0u);++n.derivedStores;n.pc=0x2bb170;
    n.pc=0x2bb174;store(0x24u,4u,0x3f800000u);++n.derivedStores;
    n.pc=0x2bb178;n.wholeSelected=true;
    n.pc=0x2bb17c;p.data1B4=SourceWaterPoolOwner::DataView{&d.role,0u};n.aliasStored=true;n.aliasOffset=0x1b4u;
    //The reached pointer supersedes historical zero, but native role identity
    //does not make the four guest pointer bytes known. Do not read/write them.
    for(std::size_t i=0x1b4u;i<0x1b8u;++i){p.knownMask[i]=0u;p.knownBits[i]=0u;}
    n.pc=0x2bb180;n.aliasReloaded=true;n.pc=0x2bb184;n.savedHandleS3=f.sourceHandle1E0;
    n.pc=0x2bb188;n.pc=0x2bb18c;n.stop=S::service2BB18C;
    auto& l=n.load2BB18C;l.attempted=true;l.pc=0x2bb18cu;
    if(!waterPoolFileServiceCurrentNative())return finish(WaterPoolDataResult::Status::nativeStopped);
    const auto& domain=sourceTrackMountDomain_;const auto& first=f.load2BB044;
    if(first.applicationIdentity!=sourceLoadApplication_||first.domainIdentity!=domain.get()||
       first.resolverIdentity!=domain->resolver3E1A70().get()||first.providerIdentity!=domain->archiveProvider3E1A70().get()||
       first.trackIdentity!=sourceTrackFile_.get())return finish(WaterPoolDataResult::Status::nativeStopped);
    const void* mountIdentity=nullptr;std::size_t mount=static_cast<std::size_t>(-1);std::uint64_t generation=0;
    if(!domain->resolver3E1A70()->waterPoolTrackRelationNative(sourceTrackFile_.get(),domain->archiveProvider3E1A70().get(),
       mountIdentity,mount,generation)||mountIdentity!=first.trackMountIdentity||mount!=first.trackMount||
       generation!=first.trackGeneration)return finish(WaterPoolDataResult::Status::nativeStopped);
    l.applicationIdentity=sourceLoadApplication_;l.domainIdentity=domain.get();l.resolverIdentity=domain->resolver3E1A70().get();
    l.providerIdentity=domain->archiveProvider3E1A70().get();l.trackIdentity=sourceTrackFile_.get();
    l.trackMountIdentity=mountIdentity;l.trackMount=mount;l.trackGeneration=generation;l.resolved=true;
    n.pc=0x2bb190;n.itemSize=4u;n.pc=0x2bb194;n.itemCount=1u;n.pc=0x2bb198;n.destinationOffset=0x14u;
    n.pc=0x2bb19c;n.callerReadIssued=true;n.pc=0x2bb1a0;n.callerReadDelay=true;
    n.pc=0x12fb30;n.stop=S::childCountRead;
    n.readResultAdmitted=f.file->readChildCount12FB30(&d.role,n.diagnostic);
    const auto& read=f.file->countRead_;n.copiedCountBytes=read.copied;
    //Only bytes really copied overlay the constructor's known zero suffix.
    //Even a refused native transfer can leave these destination effects.
    for(std::size_t i=0;i<read.copied;++i){n.countBytes[i]=read.bytes[i];store(0x14u+i,1u,read.bytes[i]);}
    if(!n.readResultAdmitted){n.pc=read.pc?read.pc:(n.diagnostic.sourcePC?n.diagnostic.sourcePC:0x12fb30u);
        return finish(WaterPoolDataResult::Status::nativeStopped);}
    n.pc=0x2bb1a4;n.countWord=std::uint32_t(n.countBytes[0])|(std::uint32_t(n.countBytes[1])<<8u)|
        (std::uint32_t(n.countBytes[2])<<16u)|(std::uint32_t(n.countBytes[3])<<24u);n.countLoaded=true;
    n.signedCount=*n.countWord&0x80000000u?std::int64_t(*n.countWord)-0x100000000ll:std::int64_t(*n.countWord);
    n.pc=0x2bb1a8;n.nonpositive=*n.signedCount<=0;n.countBranchExecuted=true;
    n.pc=0x2bb1ac;n.countBranchDelay=true;
    if(!*n.nonpositive){n.pc=0x2bb1b0;n.stop=S::childAllocation2BB1B0;
        return finish(WaterPoolDataResult::Status::sourceBoundary);} //SLL delay2BB1B4 is UNISSUED.
    n.pc=0x2bb240;store(0x10u,4u,0u);n.childPointerZeroRestored=true;
    n.pc=0x2bb244;n.topologyAliasReloaded=true;n.pc=0x2bb248;n.topologyHandleReloaded=true;
    n.pc=0x2bb24c;n.stop=S::topologyCall2BB24C;
    return finish(WaterPoolDataResult::Status::sourceBoundary); //JAL/delay/topology body all pending.
}

#include "original_source_world_water_pool_material50la.inl"
#include "original_source_world_water_pool_topology_count50la.inl"

#include "original_source_world_water_pool_topology_backing50la.inl"

#include "original_source_world_water_pool_indices50la.inl"
#include "original_source_world_water_pool_record_count50la.inl"
#include "original_source_world_water_pool_record_backing50la.inl"
#include "original_source_world_water_pool_first_element50la.inl"
#include "original_source_world_water_pool_inner_object50la.inl"
#include "original_source_world_water_pool_inner_load50la.inl"

#include "original_source_world_water_pool_row_backing50la.inl"

#include "original_source_world_water_pool_row_input50la.inl"
