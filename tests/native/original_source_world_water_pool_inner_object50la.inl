bool Owner::preflightNativeWaterPoolInnerObject()const noexcept {
    if(!preflightNativeWaterPoolInnerLoad())return false;
    using S=WaterPoolInnerObjectStop;using G=a::OriginalConfigProcessGlobals50LA;
    const auto& n=sourceWaterConfig_.nativeWaterPoolInnerObject;
    const auto& element=sourceWaterConfig_.nativeWaterPoolFirstElement;
    const auto& b=sourceWaterConfig_.nativeWaterPoolRecordBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    const auto* topology=sourceWaterProvider_&&sourceWaterProvider_->nativePool&&sourceWaterProvider_->nativePool->nativeData?
        sourceWaterProvider_->nativePool->nativeData->nativeTopology.get():nullptr;
    const auto* record=topology?topology->firstRecord.get():nullptr;
    const auto absent=[&]()noexcept{return !record||(!record->firstInner&&!record->firstObjectSlot);};
    const auto noOwner=[&]()noexcept{return !n.ownerAllocated&&!n.objectIdentity&&!n.payloadAttempted&&
        !n.allocated&&!n.constructed&&!n.aliasStored&&!n.retired&&absent();};
    if(!n.attempted)return n.stop==S::none&&!n.pc&&!n.nativeReent&&!n.recordIdentity&&noOwner();
    if(element.stop!=WaterPoolElementStop::newEntry2E8D90||!element.halfwordStored||
       !n.recordIdentity||n.recordIdentity!=element.recordIdentity||!b.count)return false;
    if(n.stop==S::nativeReentrancy)return n.pc==0x2e8d90u&&
        n.nativeReent!=G::StdioIdentity::reent385160&&noOwner();
    if(n.nativeReent!=G::StdioIdentity::reent385160)return false;
    if(n.stop==S::ownerAllocation)return n.pc==0x2e8d90u&&noOwner();
    if(!n.ownerAllocated||!n.objectIdentity||!n.payloadAttempted||(b.retired&&!n.retired))return false;
    if(n.stop==S::payloadAllocation){
        if(n.pc!=0x2e8d90u||n.allocated||n.constructed||n.aliasStored)return false;
    }else if(n.stop!=S::loader2998D0||n.pc!=0x2998d0u||!n.allocated||!n.constructed||!n.aliasStored)return false;
    if(n.retired)return absent()&&f.nativeWithdrawn&&f.file&&f.file->nativeClosed();
    if(!record||record!=n.recordIdentity||!record->firstInner||record->firstInner.get()!=n.objectIdentity)return false;
    const auto& object=*record->firstInner;
    if(object.record!=record||bool(object.payload)!=n.allocated||
       record->firstObjectSlot!=(n.aliasStored?&object:nullptr))return false;
    if(n.constructed){
        const auto* bytes=static_cast<const std::uint8_t*>(object.payload.get());
        const auto& load=sourceWaterConfig_.nativeWaterPoolInnerLoad;
        const auto& read=f.file->innerCount_.read;
        for(unsigned i=0;i<12u;++i){
            if(load.aliasStored&&i<4u)continue; //Semantic rows pointer: unknown guest bits.
            const auto expected=i>=4u&&i<4u+read.copied?read.bytes[i-4u]:std::uint8_t{0};
            if(bytes[i]!=expected)return false;
        }
        if(object.payload.get()==record->objectWords.get()||object.payload.get()==record->halfwords.get()||
           object.payload.get()==topology->payload.get())return false;
    }
    return true;
}

bool Owner::withdrawNativeWaterPoolInnerObject()noexcept {
    if(!preflightNativeWaterPoolInnerObject())return false;
    auto& n=sourceWaterConfig_.nativeWaterPoolInnerObject;
    if(!n.ownerAllocated||n.retired)return true;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    if(nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||!sourceWaterConfig_.nativeWithdrawn||
       !sourceWaterProviderInitializer_.world400NativeWithdrawn||!f.nativeWithdrawn||!f.file->nativeClosed())return false;
    if(!withdrawNativeWaterPoolInnerLoad())return false;
    auto& record=*sourceWaterProvider_->nativePool->nativeData->nativeTopology->firstRecord;
    record.firstObjectSlot=nullptr;record.firstInner.reset();n.retired=true;
    return true; //No original scalar-delete or owner-count event is inferred.
}

Owner::WaterPoolInnerObjectResult Owner::constructFirstWaterPoolInnerNativeOnly(WaterArrayBackingMode mode)noexcept {
    using R=WaterPoolInnerObjectResult;using S=WaterPoolInnerObjectStop;using G=a::OriginalConfigProcessGlobals50LA;
    auto& n=sourceWaterConfig_.nativeWaterPoolInnerObject;
    const auto& element=sourceWaterConfig_.nativeWaterPoolFirstElement;
    const auto& b=sourceWaterConfig_.nativeWaterPoolRecordBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    R result;const auto finish=[&](R::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.pc=n.pc;result.originalSourceStop=element.pc;
        result.allocated=n.allocated;result.constructed=n.constructed;result.aliasStored=n.aliasStored;result.retired=n.retired;
        return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !preflightNativeWaterPoolDataHistory())return result;
    if(n.attempted)return finish(R::Status::alreadyAttempted);
    std::size_t pools=0,materials=0;
    if(!sourceWaterPoolCountedNative(pools)||!sourceWaterPoolMaterialCountedNative(materials)||pools!=1u||
       !waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       element.stop!=WaterPoolElementStop::newEntry2E8D90||!element.halfwordStored||b.retired||
       !f.file||f.file->nativeClosed()||f.file->nativeCloseAttempted_||f.file->sourceCloseAttempted_||
       !f.file->preflightNative(f.wholeRoleIdentity))return result;
    auto& record=*sourceWaterProvider_->nativePool->nativeData->nativeTopology->firstRecord;
    if(record.firstInner||record.firstObjectSlot)return result;
    n.attempted=true;n.recordIdentity=&record;n.pc=0x2e8d90u;n.stop=S::nativeReentrancy;
    //The original scalar-new prefix remains at its authenticated entry.
    //Native success backing is distinct; no original allocation return is claimed.
    if(G::reentrancyCurrentNative())n.nativeReent=G::state().reent38544C;
    if(n.nativeReent!=G::StdioIdentity::reent385160)return finish(R::Status::nativeStopped);
    n.stop=S::ownerAllocation;
    record.firstInner.reset(new(std::nothrow) SourceWaterPoolInnerOwner);
    if(!record.firstInner)return finish(R::Status::nativeStopped);
    auto& object=*record.firstInner;object.record=&record;n.objectIdentity=&object;n.ownerAllocated=true;
    n.stop=S::payloadAllocation;n.payloadAttempted=true;object.payload.reset(std::malloc(12u));
    if(!object.payload)return finish(R::Status::nativeStopped);
    n.allocated=true;
    //299790/94/98 initialize exactly the three constructor words.
    auto* bytes=static_cast<std::uint8_t*>(object.payload.get());
    for(unsigned word=0;word<3;++word)for(unsigned byte=0;byte<4;++byte)bytes[word*4u+byte]=0;
    n.constructed=true;
    //2BADB0 publishes slot0 in the separate4*N pointer array semantically.
    //The unknown guest pointer word is not replaced by truncated host bits.
    record.firstObjectSlot=&object;n.aliasStored=true;
    n.pc=0x2998d0u;n.stop=S::loader2998D0;
    return finish(R::Status::sourceBoundary); //2BADC8/CC reached the same-file loader; body pending.
}
