//Native successor of the authenticated topology-count prefix. Original malloc
//and its arena load do not return; native backing has a separate lifetime.
Owner::SourceWaterConfig::NativeWaterPoolMaterial::RawAllocation Owner::topologyRawAllocationPrefix(
    std::uint32_t request,std::optional<a::OriginalConfigProcessGlobals50LA::StdioIdentity> captured,std::uint32_t callPC)noexcept {
    using G=a::OriginalConfigProcessGlobals50LA;
    using A=SourceWaterConfig::NativeWaterPoolMaterial::RawAllocation;using R=A::ReentStop;
    A x;
    const auto signWord=[](std::uint32_t v)noexcept {
        return v&0x80000000u?(0xffffffff00000000ull|std::uint64_t(v)):std::uint64_t(v);
    };
    x.attempted=true;x.callIssued=true;x.pc=callPC;x.callerReturnPC=callPC+8u;
    x.pc=callPC+4u;x.requestedBytes=request;
    x.argumentA0=signWord(*x.requestedBytes);x.shiftDelay=true;
    x.pc=0x305700;x.argumentCopied=true;x.pc=0x305704;x.pc=0x305708;x.mallocFrame=true;x.pc=0x30570c;
    if(!captured){x.reentStop=R::unknown;return x;}
    x.reent=captured;
    if(*captured!=G::StdioIdentity::reent385160){x.reentStop=R::unsupportedIdentity;return x;}
    x.pc=0x305710;x.mallocSaveMask=1u;x.pc=0x305714;x.pc=0x305718;
    x.pc=0x305990;x.allocatorFrame=true;x.pc=0x305994;x.sumWord=std::uint32_t(*x.requestedBytes+19u);
    x.sumRegister=signWord(*x.sumWord);x.pc=0x305998;x.allocatorSaveMask|=1u;
    x.pc=0x30599c;x.minimumChunk=*x.sumRegister<31ull;x.pc=0x3059a0;x.allocatorSaveMask|=2u;
    x.pc=0x3059a4;x.reentCopied=true;x.pc=0x3059a8;x.allocatorSaveMask|=4u;
    x.pc=0x3059ac;x.allocatorSaveMask|=8u;x.pc=0x3059b0;x.allocatorSaveMask|=16u;
    x.pc=0x3059b4;x.pc=0x3059b8;x.allocatorSaveMask|=32u;
    if(*x.minimumChunk){x.pc=0x3059cc;x.normalizedChunk=16u;}
    else {x.pc=0x3059bc;x.pc=0x3059c0;x.pc=0x3059c4;x.pc=0x3059c8;x.normalizedChunk=*x.sumWord&0xfffffff0u;}
    x.normalizedRegister=signWord(*x.normalizedChunk);
    x.pc=0x3059d0;x.lockCalled=true;x.pc=0x3059d4;x.pc=0x306538;x.pc=0x30653c;x.lockReturned=true;
    x.pc=0x3059d8;x.smallBin=*x.normalizedRegister<504ull;x.pc=0x3059dc;x.pc=0x3059e0;
    if(*x.smallBin){
        x.pc=0x3059e4;x.arenaBase=0x385468u;x.pc=0x3059e8;
        x.arenaReadOffset=*x.normalizedChunk+12u;x.arenaReadAddress=*x.arenaBase+*x.arenaReadOffset;
        x.pc=0x3059ec; //BEFORE arena LW, never executed.
    }else {
        x.pc=0x305a38;const auto chunk=*x.normalizedChunk;const auto t=chunk>>9u;x.binScale=t;
        x.pc=0x305a3c;x.pc=0x305a40;
        if(!t){x.pc=0x305a44;x.pc=0x305a48;x.binIndex=chunk>>3u;x.binSelectionPC=x.pc;}
        else {
            x.pc=0x305a4c;x.pc=0x305a50;
            if(t<5u){x.pc=0x305a54;x.pc=0x305a58;x.binIndex=(chunk>>6u)+56u;x.binSelectionPC=x.pc;}
            else {
                x.pc=0x305a5c;x.pc=0x305a60;x.pc=0x305a64;
                if(t<21u){x.binIndex=t+91u;x.binSelectionPC=x.pc;}
                else {
                    x.pc=0x305a68;x.pc=0x305a6c;x.pc=0x305a70;
                    if(t<85u){x.pc=0x305a74;x.pc=0x305a78;x.binIndex=(chunk>>12u)+110u;x.binSelectionPC=x.pc;}
                    else {
                        x.pc=0x305a7c;x.pc=0x305a80;x.pc=0x305a84;
                        if(t<341u){x.pc=0x305a88;x.pc=0x305a8c;x.binIndex=(chunk>>15u)+119u;x.binSelectionPC=x.pc;}
                        else {
                            x.pc=0x305a90;x.pc=0x305a94;x.pc=0x305a98;
                            if(t<1365u){x.pc=0x305a9c;x.pc=0x305aa0;x.binIndex=(chunk>>18u)+124u;x.binSelectionPC=x.pc;}
                            else {x.pc=0x305aa4;x.binIndex=126u;x.binSelectionPC=x.pc;}
                        }
                    }
                }
            }
        }
        x.pc=0x305aa8;x.pc=0x305aac;x.pc=0x305ab0;x.arenaBase=0x385468u;
        x.pc=0x305ab4;x.pc=0x305ab8;x.arenaReadOffset=8u * *x.binIndex+12u;
        x.arenaReadAddress=*x.arenaBase+*x.arenaReadOffset;x.pc=0x305abc;
        x.pc=0x305ac0;x.largeBranchIssued=true;x.pendingDelayPC=0x305ac4u;x.pendingTargetPC=0x305ad4u;
        x.pc=0x305ac4; //BEFORE unexecuted LW delay; branch transfer is still pending.
    }
    return x;
}

bool Owner::preflightNativeWaterPoolTopologyBacking()const noexcept {
    if(!preflightNativeWaterPoolRecordCount())return false;
    using S=WaterPoolTopologyBackingStop;using G=a::OriginalConfigProcessGlobals50LA;
    using A=SourceWaterConfig::NativeWaterPoolMaterial::RawAllocation;
    const auto& n=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyCount;
    const auto& d=sourceWaterConfig_.nativeWaterPoolData;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    using Init=g::OriginalWaterTopologyInitialization50LA;
    const auto& init=n.initialization;
    if(!n.initializationAttempted){if(init!=Init{})return false;}
    else {
        if(!n.allocated||!n.aliasStored||!n.count)return false;
        if(init.status==Init::Status::invalidGlobals){
            auto expected=Init{};expected.status=Init::Status::invalidGlobals;expected.pc=0x2bab60u;
            if(init!=expected)return false;
        }else if(init.status==Init::Status::initialized){
            if(init.pc!=0x2babe4u||!init.guardBefore||*init.guardBefore>1u||
               init.assertionStored!=(*init.guardBefore==0u)||init.guardStored!=init.assertionStored||
               init.assertionRead!=1u||!init.assertionCalled||init.assertionReturn!=0u||
               !init.zeroFillReturned||init.zeroedBytes!=std::uint64_t(*n.count)*20ull)return false;
        }else return false;
    }
    const auto absent=[&]()noexcept {
        return !sourceWaterProvider_||!sourceWaterProvider_->nativePool||
            !sourceWaterProvider_->nativePool->nativeData||
            (!sourceWaterProvider_->nativePool->nativeData->nativeTopology&&
             !sourceWaterProvider_->nativePool->nativeData->topology28);
    };
    const auto noBacking=[&]()noexcept {
        return !n.allocated&&!n.aliasStored&&!n.aliasRevoked&&!n.retired&&!n.backingIdentity&&absent();
    };
    const auto noNative=[&]()noexcept {
        return !n.ownerAttempted&&!n.ownerPrepared&&!n.payloadAttempted&&!n.temporaryRetired&&noBacking();
    };
    if(!n.attempted)return n.stop==S::none&&!n.pc&&!n.dataRole&&!n.fileIdentity&&!n.dataEpoch&&
        !n.count&&!n.nativeReent&&n.allocator==A{}&&noNative();
    //Leaf history: do not call Data/count/provider preflights recursively.
    if(!t.attempted||!t.readResultAdmitted||t.stop!=WaterPoolTopologyStop::mallocCall2BAB54||
       t.pc!=0x2bab54u||t.pendingCallPC!=0x2bab54u||t.pendingDelayPC!=0x2bab58u||
       !t.topologyWord||n.count!=t.topologyWord||n.dataRole!=t.dataRoleIdentity||
       n.dataEpoch!=t.dataEpoch||!n.dataRole||!n.dataEpoch||n.fileIdentity!=t.fileIdentity||
       n.fileIdentity!=f.file.get()||!f.file||!d.nativeAllocated||d.dataRoleIdentity!=n.dataRole||
       d.dataEpoch!=n.dataEpoch)return false;
    const auto request=std::uint32_t(std::uint32_t(*n.count*5u)<<2u);
    if(t.prospectiveRequestWord!=request||n.allocator!=topologyRawAllocationPrefix(request,n.allocator.reent))return false;
    if(n.allocator.reentStop!=A::ReentStop::none)return n.stop==S::sourceReentrancy&&
        n.pc==0x2bab54u&&!n.nativeReent&&noNative();
    if(n.stop==S::nativeReentrancy)return n.pc==0x2bab54u&&
        (!n.nativeReent||*n.nativeReent!=G::StdioIdentity::reent385160)&&noNative();
    if(n.nativeReent!=G::StdioIdentity::reent385160)return false;
    const auto plan=g::originalWaterTopologyRequest50LA(*n.count,nativeWaterArrayMaxCount,nativeWaterArrayMaxBytes);
    using Admission=g::OriginalWaterTopologyRequest50LA::Admission;
    if(n.stop==S::countCapacity||n.stop==S::byteCapacity||n.stop==S::requestMismatch){
        const bool exact=n.stop==S::countCapacity?plan.admission==Admission::countCapacity:
            n.stop==S::byteCapacity?plan.admission==Admission::byteCapacity:plan.admission==Admission::requestMismatch;
        return exact&&n.pc==0x2bab54u&&noNative();
    }
    if(plan.admission!=Admission::accepted||!n.ownerAttempted)return false;
    if(n.stop==S::ownerAllocation)return n.pc==0x2bab54u&&!n.ownerPrepared&&!n.payloadAttempted&&
        !n.temporaryRetired&&noBacking();
    if(!n.ownerPrepared||!n.payloadAttempted)return false;
    if(n.stop==S::payloadAllocation)return n.pc==0x2bab54u&&n.temporaryRetired&&noBacking();
    if(n.stop!=S::assertionGuard2BAB60||n.pc!=0x2bab60u||n.temporaryRetired||!n.allocated||
       !n.aliasStored||!n.backingIdentity||n.aliasRevoked!=n.retired||(d.nativeRetired&&!n.retired))return false;
    if(n.retired)return f.nativeWithdrawn&&f.file->nativeClosed()&&absent();
    if(d.nativeRetired||!sourceWaterProvider_||!sourceWaterProvider_->nativePool||
       !sourceWaterProvider_->nativePool->nativeData)return false;
    const auto& data=*sourceWaterProvider_->nativePool->nativeData;
    const auto* backing=data.nativeTopology.get();
    if(init.zeroFillReturned){
        if(!backing||!backing->payload||backing->requestedBytes!=init.zeroedBytes)return false;
        const auto* bytes=static_cast<const std::uint8_t*>(backing->payload.get());
        const auto& record=f.file->firstTopologyRecord_.read;
        for(std::size_t i=0;i<init.zeroedBytes;++i){
            const auto expected=i<record.copied?record.bytes[i]:std::uint8_t{0};
            if(bytes[i]!=expected)return false;
        }
    }
    return backing&&backing==n.backingIdentity&&data.topology28==backing&&backing->payload&&
        backing->dataOwner==&data&&backing->dataEpoch==n.dataEpoch&& &data.role==n.dataRole&&
        backing->count==*n.count&&backing->requestedBytes==request&&
        backing->payload.get()!=data.payload.get();
}

bool Owner::withdrawNativeWaterPoolTopologyBacking()noexcept {
    if(!preflightNativeWaterPoolTopologyBacking())return false;
    auto& n=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    if(!n.allocated||n.retired)return true;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    if(nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||!sourceWaterConfig_.nativeWithdrawn||
       !sourceWaterProviderInitializer_.world400NativeWithdrawn||!f.nativeWithdrawn||!f.file->nativeClosed())return false;
    auto& data=*sourceWaterProvider_->nativePool->nativeData;
    data.topology28=nullptr;n.aliasRevoked=true;
    data.nativeTopology.reset();n.retired=true;
    //Retained source history and unknown +28 guest bytes are not restored to zero.
    return true;
}

Owner::WaterPoolTopologyBackingResult Owner::allocateWaterPoolTopologyNativeOnly(WaterArrayBackingMode mode)noexcept {
    using S=WaterPoolTopologyBackingStop;using G=a::OriginalConfigProcessGlobals50LA;
    using Result=WaterPoolTopologyBackingResult;
    auto& n=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyCount;
    const auto& d=sourceWaterConfig_.nativeWaterPoolData;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    Result result;
    const auto finish=[&](Result::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.countPrefixPC=t.pc;
        result.allocatorPC=n.allocator.pc;result.adaptedPC=n.pc;
        result.requestedBytes=n.allocator.requestedBytes;result.normalizedChunk=n.allocator.normalizedChunk;
        result.allocated=n.allocated;result.aliasStored=n.aliasStored;result.retired=n.retired;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_)return result;
    std::size_t poolCount=0,materialCount=0;
    if(!sourceWaterPoolCountedNative(poolCount)||!sourceWaterPoolMaterialCountedNative(materialCount)||
       !preflightNativeWaterPoolDataHistory())return result;
    if(n.attempted)return finish(Result::Status::alreadyAttempted);
    if(!waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       poolCount!=1u||!d.nativeAllocated||d.nativeRetired||!t.readResultAdmitted||
       t.stop!=WaterPoolTopologyStop::mallocCall2BAB54||!t.topologyWord||!t.prospectiveRequestWord||
       !f.file||f.file->nativeClosed()||f.file->nativeCloseAttempted_||f.file->sourceCloseAttempted_||
       !f.file->preflightNative(f.wholeRoleIdentity)||!sourceWaterProvider_||!sourceWaterProvider_->nativePool||
       !sourceWaterProvider_->nativePool->nativeData)return result;
    auto& data=*sourceWaterProvider_->nativePool->nativeData;
    if(data.nativeTopology||data.topology28||&data.role!=t.dataRoleIdentity||data.role.epoch!=t.dataEpoch)return result;
    n.attempted=true;n.pc=0x2bab54u;n.dataRole=&data.role;n.dataEpoch=data.role.epoch;
    n.fileIdentity=f.file.get();n.count=t.topologyWord;n.stop=S::sourceReentrancy;
    //A fresh source reentrancy read, independent of the preceding allocations.
    std::optional<G::StdioIdentity> captured;
    if(G::reentrancyCurrentNative())captured=G::state().reent38544C;
    n.allocator=topologyRawAllocationPrefix(*t.prospectiveRequestWord,captured);
    if(n.allocator.reentStop!=SourceWaterConfig::NativeWaterPoolMaterial::RawAllocation::ReentStop::none)
        return finish(Result::Status::nativeStopped);
    n.stop=S::nativeReentrancy;
    if(!G::reentrancyCurrentNative())return finish(Result::Status::nativeStopped);
    n.nativeReent=G::state().reent38544C;
    if(*n.nativeReent!=G::StdioIdentity::reent385160)return finish(Result::Status::nativeStopped);
    const auto plan=g::originalWaterTopologyRequest50LA(*n.count,nativeWaterArrayMaxCount,nativeWaterArrayMaxBytes);
    using Admission=g::OriginalWaterTopologyRequest50LA::Admission;
    if(plan.admission!=Admission::accepted){
        n.stop=plan.admission==Admission::countCapacity?S::countCapacity:
            plan.admission==Admission::byteCapacity?S::byteCapacity:S::requestMismatch;
        return finish(Result::Status::nativeStopped);
    }
    const auto full=plan.fullBytes;const auto request=plan.sourceRequest;
    n.stop=S::ownerAllocation;n.ownerAttempted=true;
    std::unique_ptr<SourceWaterPoolTopologyOwner> pending(new(std::nothrow) SourceWaterPoolTopologyOwner);
    if(!pending)return finish(Result::Status::nativeStopped);
    n.ownerPrepared=true;n.payloadAttempted=true;n.stop=S::payloadAllocation;
    //malloc(0) stays malloc(0). A native null refuses this adapter; it never
    //becomes an original failure return or a fabricated one-byte allocation.
    pending->payload.reset(std::malloc(static_cast<std::size_t>(full)));
    if(!pending->payload){n.temporaryRetired=true;return finish(Result::Status::nativeStopped);}
    pending->dataOwner=&data;pending->dataEpoch=data.role.epoch;pending->count=*n.count;pending->requestedBytes=request;
    data.nativeTopology=std::move(pending);n.backingIdentity=data.nativeTopology.get();n.allocated=true;
    n.pc=0x2bab5cu;data.topology28=n.backingIdentity;n.aliasStored=true;
    for(std::size_t i=0x28u;i<0x2cu;++i){data.knownMask[i]=0u;data.knownBits[i]=0u;}
    n.pc=0x2bab60u;n.stop=S::assertionGuard2BAB60;
    return finish(Result::Status::sourceBoundary);
    //No assertion global read, zero fill, index allocation, file read or return.
}


g::OriginalWaterTopologyInitialization50LA Owner::initializeWaterPoolTopologyNativeOnly(WaterArrayBackingMode mode)noexcept {
    using R=g::OriginalWaterTopologyInitialization50LA;
    auto& n=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !preflightNativeWaterPoolDataHistory())return {};
    if(n.initializationAttempted){auto result=n.initialization;result.status=R::Status::alreadyAttempted;return result;}
    std::size_t poolCount=0,materialCount=0;
    if(!sourceWaterPoolCountedNative(poolCount)||!sourceWaterPoolMaterialCountedNative(materialCount)||
       !waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       poolCount!=1u||!n.allocated||n.retired||!f.file||f.file->nativeClosed()||
       f.file->nativeCloseAttempted_||f.file->sourceCloseAttempted_||!f.file->preflightNative(f.wholeRoleIdentity)||
       !worldApplication_)return {};
    auto& data=*sourceWaterProvider_->nativePool->nativeData;
    auto& backing=*data.nativeTopology;
    //Fresh current Data count LW2BABC8 and alias LW2BABCC must agree with
    //the exact allocation validated above; read actual known bytes again.
    const auto* bytes=static_cast<const std::uint8_t*>(data.payload.get());
    std::uint32_t currentCount=0;
    for(std::size_t i=0;i<4u;++i){
        if(data.knownMask[0x1cu+i]!=255u)return {};
        currentCount|=std::uint32_t(bytes[0x1cu+i])<<(8u*i);
    }
    if(currentCount!=backing.count||data.topology28!=&backing)return {};
    n.initializationAttempted=true;
    n.initialization=g::initializeOriginalWaterTopology50LA(worldApplication_->sourceWaterTopologyGlobals_,
        backing.payload.get(),backing.requestedBytes,currentCount);
    return n.initialization;
    //Count reload/index malloc2BABE4/2BABE8 and delay2BABEC remain pending.
}
