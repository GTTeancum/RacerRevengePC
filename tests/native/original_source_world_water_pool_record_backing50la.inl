bool Owner::preflightNativeWaterPoolRecordBacking()const noexcept {
    if(!preflightNativeWaterPoolFirstElement())return false;
    using S=WaterPoolRecordBackingStop;using A=SourceWaterConfig::NativeWaterPoolMaterial::RawAllocation;
    using G=a::OriginalConfigProcessGlobals50LA;using E=g::OriginalWaterTopologyElements50LA;
    const auto& n=sourceWaterConfig_.nativeWaterPoolRecordBacking;
    const auto& c=sourceWaterConfig_.nativeWaterPoolRecordCount;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    const auto* topology=sourceWaterProvider_&&sourceWaterProvider_->nativePool&&sourceWaterProvider_->nativePool->nativeData?
        sourceWaterProvider_->nativePool->nativeData->nativeTopology.get():nullptr;
    const auto* owner=topology?topology->firstRecord.get():nullptr;
    const auto noCalls=[&]()noexcept {
        return n.callEntered==std::array<bool,3>{}&&n.payloadAttempted==std::array<bool,3>{}&&
            n.aliasStored==std::array<bool,3>{}&&!n.nativeReent[0]&&!n.nativeReent[1]&&!n.nativeReent[2]&&
            n.raw[0]==A{}&&n.raw[1]==A{}&&!n.elementsAllocation&&!n.arrayRequest&&!n.arraySourcePC;
    };
    if(!n.attempted)return n.stop==S::none&&!n.pc&&!n.stage&&!n.count&&!n.ownerAllocated&&
        !n.ownerIdentity&&!n.retired&&!owner&&noCalls();
    if(c.stop!=WaterPoolRecordCountStop::malloc2BAC88||!c.count||n.count!=*c.count||n.stage>3u)return false;
    if(n.stop==S::capacity)return n.count>nativeWaterArrayMaxCount&&n.pc==0x2bac88u&&!n.stage&&
        !n.ownerAllocated&&!n.ownerIdentity&&!n.retired&&!owner&&noCalls();
    if(n.count>nativeWaterArrayMaxCount)return false;
    if(n.stop==S::ownerAllocation)return n.pc==0x2bac88u&&!n.stage&&!n.ownerAllocated&&
        !n.ownerIdentity&&!n.retired&&!owner&&noCalls();
    if(!n.ownerAllocated||!n.ownerIdentity||(t.retired&&!n.retired))return false;
    constexpr std::array<std::uint32_t,3> calls{0x2bac88u,0x2bacb4u,0x2bacecu};
    for(unsigned i=0;i<3;++i){
        if(i>n.stage){
            if(n.callEntered[i]||n.payloadAttempted[i]||n.aliasStored[i]||n.nativeReent[i])return false;
            if(i<2&&n.raw[i]!=A{})return false;
            continue;
        }
        if(!n.callEntered[i])return false;
        if(i<2){
            if(n.raw[i]!=topologyRawAllocationPrefix(n.count<<(i+1u),n.raw[i].reent,calls[i]))return false;
            if(n.raw[i].reentStop!=A::ReentStop::none){
                if(i!=n.stage||n.stop!=S::sourceReentrancy||n.pc!=calls[i]||n.nativeReent[i]||
                   n.payloadAttempted[i]||n.aliasStored[i])return false;
                continue;
            }
        }
        if(n.nativeReent[i]!=G::StdioIdentity::reent385160){
            if(i!=n.stage||n.stop!=S::nativeReentrancy||n.pc!=calls[i]||n.payloadAttempted[i]||n.aliasStored[i])return false;
            continue;
        }
        if(!n.payloadAttempted[i])return false;
        if(!n.aliasStored[i]){
            if(i!=n.stage)return false;
            if(n.stop==S::payloadAllocation){if(n.pc!=calls[i])return false;}
            else if(i!=2||n.stop!=S::elementConstruction||n.pc!=0x2bad0cu)return false;
        }else if(i>=n.stage)return false;
    }
    if(n.stage==3&&(n.stop!=S::innerLoop2BADD8||n.pc!=0x2badd8u))return false;
    if(n.callEntered[2]){
        if(n.arraySourcePC!=0x2e8ca0u||n.arrayRequest!=n.count*48u+16u)return false;
    }else if(n.arraySourcePC||n.arrayRequest)return false;
    if(!n.payloadAttempted[2]){if(n.elementsAllocation)return false;}
    else if(!n.elementsAllocation||
        *n.elementsAllocation!=(n.aliasStored[2]||n.stop==S::elementConstruction?E::Allocation::ready:E::Allocation::payload))return false;
    if(n.retired)return !owner&&f.nativeWithdrawn&&f.file&&f.file->nativeClosed();
    if(!owner||owner!=n.ownerIdentity||owner->topology!=topology||owner->count!=n.count)return false;
    if(bool(owner->halfwords)!=n.aliasStored[0]||bool(owner->objectWords)!=n.aliasStored[1]||
       owner->halfwords04!=owner->halfwords.get()||owner->objects08!=owner->objectWords.get())return false;
    if(owner->elements10!=(n.aliasStored[2]?owner->elements.elementsIdentity():nullptr))return false;
    if(n.elementsAllocation==E::Allocation::ready){
        if(!owner->elements.allocationIdentity()||owner->elements.count()!=n.count)return false;
    }else if(owner->elements.allocationIdentity())return false;
    if(n.aliasStored[2]){
        if(!owner->elements10)return false;
        for(std::size_t at=0;at<owner->elements.extent();++at){
            const auto byte=owner->elements.knownByte(at);
            if(at<8u){const auto word=at<4u?48u:n.count;if(byte!=std::uint8_t(word>>(8u*(at%4u))))return false;}
            else if(at<16u||(at-16u)%48u<32u){if(byte)return false;}
            else if(byte!=std::uint8_t((at-16u)%48u<44u?0u:255u))return false;
        }
    }
    return true;
}

bool Owner::withdrawNativeWaterPoolRecordBacking()noexcept {
    if(!preflightNativeWaterPoolRecordBacking())return false;
    auto& n=sourceWaterConfig_.nativeWaterPoolRecordBacking;
    if(!n.ownerAllocated||n.retired)return true;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    if(nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||!sourceWaterConfig_.nativeWithdrawn||
       !sourceWaterProviderInitializer_.world400NativeWithdrawn||!f.nativeWithdrawn||!f.file->nativeClosed())return false;
    auto& record=sourceWaterProvider_->nativePool->nativeData->nativeTopology->firstRecord;
    record->elements10=nullptr;record->objects08=nullptr;record->halfwords04=nullptr;
    record.reset();n.retired=true;return true; //Native withdrawal; no guest destructor receipt.
}

Owner::WaterPoolRecordBackingResult Owner::allocateFirstWaterPoolRecordNativeOnly(WaterArrayBackingMode mode)noexcept {
    using R=WaterPoolRecordBackingResult;using S=WaterPoolRecordBackingStop;
    using G=a::OriginalConfigProcessGlobals50LA;using E=g::OriginalWaterTopologyElements50LA;
    auto& n=sourceWaterConfig_.nativeWaterPoolRecordBacking;
    const auto& c=sourceWaterConfig_.nativeWaterPoolRecordCount;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    R result;const auto finish=[&](R::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.pc=n.pc;result.stage=n.stage;result.count=n.count;
        result.aliases=n.aliasStored;result.retired=n.retired;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !preflightNativeWaterPoolDataHistory())return result;
    if(n.attempted)return finish(R::Status::alreadyAttempted);
    std::size_t pools=0,materials=0;
    if(!sourceWaterPoolCountedNative(pools)||!sourceWaterPoolMaterialCountedNative(materials)||pools!=1u||
       !waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       c.stop!=WaterPoolRecordCountStop::malloc2BAC88||!c.count||t.retired||!f.file||f.file->nativeClosed()||
       f.file->nativeCloseAttempted_||f.file->sourceCloseAttempted_||!f.file->preflightNative(f.wholeRoleIdentity))return result;
    auto& topology=*sourceWaterProvider_->nativePool->nativeData->nativeTopology;
    const auto countNow=[&]()noexcept {
        const auto* bytes=static_cast<const std::uint8_t*>(topology.payload.get());std::uint32_t v=0;
        for(unsigned i=0;i<4;++i)v|=std::uint32_t(bytes[i])<<(8u*i);return v;
    };
    if(countNow()!=*c.count||topology.firstRecord)return result;
    n.attempted=true;n.count=*c.count;n.pc=0x2bac88u;n.stop=S::capacity;
    if(n.count>nativeWaterArrayMaxCount)return finish(R::Status::nativeStopped);
    n.stop=S::ownerAllocation;
    topology.firstRecord.reset(new(std::nothrow) SourceWaterPoolRecordOwner);
    if(!topology.firstRecord)return finish(R::Status::nativeStopped);
    auto& record=*topology.firstRecord;record.topology=&topology;record.count=n.count;
    n.ownerAllocated=true;n.ownerIdentity=&record;
    constexpr std::array<std::uint32_t,3> calls{0x2bac88u,0x2bacb4u,0x2bacecu};
    for(;n.stage<3u;++n.stage){const auto i=n.stage;
        //No callbacks between admissions and these fresh source count reloads.
        if(countNow()!=n.count)std::terminate();
        n.pc=calls[i];n.callEntered[i]=true;
        if(i==2u){n.arrayRequest=n.count*48u+16u;n.arraySourcePC=0x2e8ca0u;}
        if(i<2u){
            n.stop=S::sourceReentrancy;std::optional<G::StdioIdentity> captured;
            if(G::reentrancyCurrentNative())captured=G::state().reent38544C;
            n.raw[i]=topologyRawAllocationPrefix(n.count<<(i+1u),captured,calls[i]);
            if(n.raw[i].reentStop!=SourceWaterConfig::NativeWaterPoolMaterial::RawAllocation::ReentStop::none)
                return finish(R::Status::nativeStopped);
        }
        //For new[] the source prefix stops at2E8CA0 entry; no raw-malloc
        //history is substituted for the distinct array-new wrapper.
        n.stop=S::nativeReentrancy;
        if(G::reentrancyCurrentNative())n.nativeReent[i]=G::state().reent38544C;
        if(n.nativeReent[i]!=G::StdioIdentity::reent385160)return finish(R::Status::nativeStopped);
        n.stop=S::payloadAllocation;n.payloadAttempted[i]=true;
        if(i<2u){
            auto& payload=i==0u?record.halfwords:record.objectWords;
            payload.reset(std::malloc(std::size_t(n.count)<<(i+1u)));
            if(!payload)return finish(R::Status::nativeStopped);
            if(i==0u)record.halfwords04=payload.get();else record.objects08=payload.get();
        }else {
            n.elementsAllocation=record.elements.allocate(n.count);
            if(*n.elementsAllocation!=E::Allocation::ready)return finish(R::Status::nativeStopped);
            n.pc=0x2bad0cu;n.stop=S::elementConstruction;
            if(!record.elements.construct())return finish(R::Status::nativeStopped);
            record.elements10=record.elements.elementsIdentity();
        }
        n.aliasStored[i]=true; //2BACA4,2BACD0,2BAD30 semantic stores only.
    }
    n.pc=0x2badd8u;n.stop=S::innerLoop2BADD8;
    return finish(R::Status::sourceBoundary); //No inner element reads or loader return.
}
