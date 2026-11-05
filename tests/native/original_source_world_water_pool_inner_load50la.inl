bool Owner::preflightNativeWaterPoolInnerLoad()const noexcept {
    if(!preflightNativeWaterPoolRowBacking())return false;
    using S=WaterPoolInnerLoadStop;using G=a::OriginalConfigProcessGlobals50LA;
    using A=SourceWaterConfig::NativeWaterPoolMaterial::RawAllocation;
    const auto& n=sourceWaterConfig_.nativeWaterPoolInnerLoad;
    const auto& parent=sourceWaterConfig_.nativeWaterPoolInnerObject;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    const auto* topology=sourceWaterProvider_&&sourceWaterProvider_->nativePool&&sourceWaterProvider_->nativePool->nativeData?
        sourceWaterProvider_->nativePool->nativeData->nativeTopology.get():nullptr;
    const auto* object=topology&&topology->firstRecord?topology->firstRecord->firstInner.get():nullptr;
    const auto absent=[&]()noexcept{return !object||(!object->rows&&!object->rows00);};
    const auto noOwner=[&]()noexcept{return !n.ownerAllocated&&!n.rowsIdentity&&!n.payloadAttempted&&
        !n.allocated&&!n.aliasStored&&!n.zeroed&&!n.retired&&absent();};
    if(!n.attempted)return !n.serviceLoaded&&n.stop==S::none&&!n.pc&&!n.count&&!n.objectIdentity&&
        n.raw==A{}&&!n.nativeReent&&noOwner()&&(!f.file||!f.file->innerCount_.read.entered);
    if(parent.stop!=WaterPoolInnerObjectStop::loader2998D0||!parent.constructed||!n.objectIdentity||
       n.objectIdentity!=parent.objectIdentity||!f.file||!f.file->innerCountHistoryNative())return false;
    const auto& entry=f.file->innerCount_;const auto& r=entry.read;
    if(r.entered&&(entry.dataIdentity!=t.dataRole||entry.objectIdentity!=n.objectIdentity||entry.dataEpoch!=t.dataEpoch))return false;
    if(n.stop==S::serviceLoad)return n.pc==0x2998f8u&&!n.serviceLoaded&&!r.entered&&!n.count&&n.raw==A{}&&!n.nativeReent&&noOwner();
    if(!n.serviceLoaded)return false;
    if(n.stop==S::read)return n.pc==0x299908u&&!r.resultAdmitted&&!n.count&&n.raw==A{}&&!n.nativeReent&&noOwner();
    if(!r.resultAdmitted||!n.count)return false;
    std::uint32_t count=0;for(unsigned i=0;i<4;++i)count|=std::uint32_t(r.bytes[i])<<(8u*i);
    if(*n.count!=count)return false;
    if(n.stop==S::capacity)return count>nativeWaterArrayMaxCount&&n.pc==0x29991cu&&n.raw==A{}&&!n.nativeReent&&noOwner();
    if(count>nativeWaterArrayMaxCount||n.raw!=topologyRawAllocationPrefix(count*24u,n.raw.reent,0x29991cu))return false;
    if(n.raw.reentStop!=A::ReentStop::none)return n.stop==S::sourceReentrancy&&n.pc==0x29991cu&&!n.nativeReent&&noOwner();
    if(n.stop==S::nativeReentrancy)return n.pc==0x29991cu&&n.nativeReent!=G::StdioIdentity::reent385160&&noOwner();
    if(n.nativeReent!=G::StdioIdentity::reent385160)return false;
    if(n.stop==S::ownerAllocation)return n.pc==0x29991cu&&noOwner();
    if(!n.ownerAllocated||!n.rowsIdentity||!n.payloadAttempted||(parent.retired&&!n.retired))return false;
    if(n.stop==S::payloadAllocation){
        if(n.pc!=0x29991cu||n.allocated||n.aliasStored||n.zeroed)return false;
    }else if(n.stop!=S::rowLoop299A60||n.pc!=0x299a60u||!n.allocated||!n.aliasStored||!n.zeroed)return false;
    if(n.retired)return absent()&&f.nativeWithdrawn&&f.file->nativeClosed();
    if(!object||object!=n.objectIdentity||!object->rows||object->rows.get()!=n.rowsIdentity)return false;
    const auto& rows=*object->rows;
    if(rows.object!=object||rows.count!=count||rows.bytes!=count*24u||bool(rows.payload)!=n.allocated||
       object->rows00!=(n.aliasStored?&rows:nullptr))return false;
    if(n.zeroed){
        if(rows.payload.get()==object->payload.get()||rows.payload.get()==topology->payload.get())return false;
        const auto* bytes=static_cast<const std::uint8_t*>(rows.payload.get());
        const auto& row=sourceWaterConfig_.nativeWaterPoolRowBacking;
        for(std::size_t i=0;i<rows.bytes;++i){
            //Native aliases do not serialize host addresses into guest pointer lanes.
            if((row.aliasStored[0]&&i>=12u&&i<16u)||(row.aliasStored[1]&&i>=8u&&i<12u))continue;
            const auto expected=row.countStored&&i>=20u&&i<24u?std::uint8_t(*row.count>>(8u*(i-20u))):0u;
            if(bytes[i]!=expected)return false;
        }
    }
    return true;
}

bool Owner::withdrawNativeWaterPoolInnerLoad()noexcept {
    if(!preflightNativeWaterPoolInnerLoad())return false;
    auto& n=sourceWaterConfig_.nativeWaterPoolInnerLoad;
    if(!n.ownerAllocated||n.retired)return true;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    if(nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||!sourceWaterConfig_.nativeWithdrawn||
       !sourceWaterProviderInitializer_.world400NativeWithdrawn||!f.nativeWithdrawn||!f.file->nativeClosed())return false;
    auto& object=*sourceWaterProvider_->nativePool->nativeData->nativeTopology->firstRecord->firstInner;
    if(!withdrawNativeWaterPoolRowBacking())return false;
    object.rows00=nullptr;object.rows.reset();n.retired=true;return true;
}

Owner::WaterPoolInnerLoadResult Owner::loadFirstWaterPoolInnerNativeOnly(WaterArrayBackingMode mode)noexcept {
    using R=WaterPoolInnerLoadResult;using S=WaterPoolInnerLoadStop;using G=a::OriginalConfigProcessGlobals50LA;
    auto& n=sourceWaterConfig_.nativeWaterPoolInnerLoad;
    const auto& parent=sourceWaterConfig_.nativeWaterPoolInnerObject;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    R result;const auto finish=[&](R::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.pc=n.pc;result.count=n.count;
        result.allocated=n.allocated;result.zeroed=n.zeroed;result.retired=n.retired;result.diagnostic=n.diagnostic;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !preflightNativeWaterPoolDataHistory())return result;
    if(n.attempted)return finish(R::Status::alreadyAttempted);
    std::size_t pools=0,materials=0;
    if(!sourceWaterPoolCountedNative(pools)||!sourceWaterPoolMaterialCountedNative(materials)||pools!=1u||
       !waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       parent.stop!=WaterPoolInnerObjectStop::loader2998D0||parent.retired||!f.file||f.file->nativeClosed()||
       f.file->nativeCloseAttempted_||f.file->sourceCloseAttempted_||!f.file->preflightNative(f.wholeRoleIdentity))return result;
    auto& object=*sourceWaterProvider_->nativePool->nativeData->nativeTopology->firstRecord->firstInner;
    n.attempted=true;n.objectIdentity=&object;n.pc=0x2998f8u;n.stop=S::serviceLoad;
    if(!waterPoolFileServiceCurrentNative())return finish(R::Status::nativeStopped);
    n.serviceLoaded=true;n.pc=0x299908u;n.stop=S::read;
    const bool admitted=f.file->readInnerCount12FB30(t.dataRole,&object,t.dataEpoch,n.diagnostic);
    const auto& read=f.file->innerCount_.read;auto* bytes=static_cast<std::uint8_t*>(object.payload.get());
    for(std::size_t i=0;i<read.copied;++i)bytes[4u+i]=read.bytes[i];
    if(!admitted)return finish(R::Status::nativeStopped);
    std::uint32_t count=0;for(unsigned i=0;i<4;++i)count|=std::uint32_t(bytes[4u+i])<<(8u*i);
    n.count=count;n.pc=0x29991cu;n.stop=S::capacity;
    if(count>nativeWaterArrayMaxCount)return finish(R::Status::nativeStopped);
    n.stop=S::sourceReentrancy;std::optional<G::StdioIdentity> captured;
    if(G::reentrancyCurrentNative())captured=G::state().reent38544C;
    n.raw=topologyRawAllocationPrefix(count*24u,captured,0x29991cu);
    if(n.raw.reentStop!=SourceWaterConfig::NativeWaterPoolMaterial::RawAllocation::ReentStop::none)return finish(R::Status::nativeStopped);
    n.stop=S::nativeReentrancy;
    if(G::reentrancyCurrentNative())n.nativeReent=G::state().reent38544C;
    if(n.nativeReent!=G::StdioIdentity::reent385160)return finish(R::Status::nativeStopped);
    n.stop=S::ownerAllocation;object.rows.reset(new(std::nothrow) SourceWaterPoolInnerRowsOwner);
    if(!object.rows)return finish(R::Status::nativeStopped);
    auto& rows=*object.rows;rows.object=&object;rows.count=count;rows.bytes=count*24u;
    n.ownerAllocated=true;n.rowsIdentity=&rows;n.stop=S::payloadAllocation;n.payloadAttempted=true;
    rows.payload.reset(std::malloc(rows.bytes)); //Preserve actual native malloc(0) behavior.
    if(!rows.payload)return finish(R::Status::nativeStopped);
    n.allocated=true;object.rows00=&rows;n.aliasStored=true; //299924 semantic pointer store.
    //299928/2C reload this same count and alias before native306478 semantics.
    std::uint32_t current=0;for(unsigned i=0;i<4;++i)current|=std::uint32_t(bytes[4u+i])<<(8u*i);
    if(current!=count||object.rows00!=&rows)std::terminate();
    std::memset(rows.payload.get(),0,rows.bytes);n.zeroed=true;
    n.pc=0x299a60u;n.stop=S::rowLoop299A60;
    return finish(R::Status::sourceBoundary); //Ordinal/stride zero; first row test/read pending.
}
