bool Owner::preflightNativeWaterPoolIndices()const noexcept {
    using S=WaterPoolIndexStop;using A=SourceWaterConfig::NativeWaterPoolMaterial::RawAllocation;
    using Allocation=g::OriginalWaterIndexStorage50LA::Allocation;using G=a::OriginalConfigProcessGlobals50LA;
    const auto& n=sourceWaterConfig_.nativeWaterPoolIndices;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    const auto absent=[&]()noexcept {
        return !sourceWaterProvider_||!sourceWaterProvider_->nativePool||!sourceWaterProvider_->nativePool->nativeData||
            (!sourceWaterProvider_->nativePool->nativeData->nativeIndices&&!sourceWaterProvider_->nativePool->nativeData->indices20);
    };
    const auto noArray=[&]()noexcept {
        return !n.allocated&&!n.aliasStored&&!n.retired&&!n.arrayIdentity&&!n.ordinal&&!n.loopTests&&!n.serviceLoads&&
            (!f.file||!f.file->indexReader_.entered)&&absent();
    };
    if(!n.attempted)return n.stop==S::none&&!n.pc&&!n.dataRole&&!n.dataEpoch&&!n.count&&!n.nativeReent&&
        !n.ownerAttempted&&n.storageResult==Allocation::none&&n.allocator==A{}&&noArray();
    if(!t.initializationAttempted||t.initialization.status!=g::OriginalWaterTopologyInitialization50LA::Status::initialized||
       !t.count||n.count!=*t.count||n.count>nativeWaterArrayMaxCount||n.dataRole!=t.dataRole||
       n.dataEpoch!=t.dataEpoch||!n.dataRole||!n.dataEpoch||!f.file)return false;
    const auto expected=topologyRawAllocationPrefix(std::uint32_t(n.count<<2u),n.allocator.reent,0x2babe8u);
    if(n.allocator!=expected)return false;
    if(n.allocator.reentStop!=A::ReentStop::none)return n.stop==S::sourceReentrancy&&n.pc==0x2babe8u&&
        !n.nativeReent&&!n.ownerAttempted&&n.storageResult==Allocation::none&&noArray();
    if(n.stop==S::nativeReentrancy)return n.pc==0x2babe8u&&
        (!n.nativeReent||*n.nativeReent!=G::StdioIdentity::reent385160)&&
        !n.ownerAttempted&&n.storageResult==Allocation::none&&noArray();
    if(n.nativeReent!=G::StdioIdentity::reent385160||!n.ownerAttempted)return false;
    if(n.stop==S::ownerAllocation)return n.pc==0x2babe8u&&n.storageResult==Allocation::none&&noArray();
    if(n.stop==S::storageAllocation)return n.pc==0x2babe8u&&
        (n.storageResult==Allocation::metadata||n.storageResult==Allocation::payload)&&noArray();
    if(!n.allocated||!n.aliasStored||!n.arrayIdentity||n.storageResult!=Allocation::ready||(t.retired&&!n.retired))return false;
    const auto& reader=f.file->indexReader_;
    if(!f.file->indexReadHistoryNative())return false;
    if(reader.entered&&(reader.dataIdentity!=n.dataRole||reader.arrayIdentity!=n.arrayIdentity||
       reader.dataEpoch!=n.dataEpoch||reader.count!=n.count))return false;
    std::uint32_t admitted=0;
    for(const auto& r:reader.reads)if(r.resultAdmitted)++admitted;
    if(n.ordinal!=admitted)return false;
    if(n.stop==S::readerPreparation){
        if(n.pc!=0x2babf4u||reader.prepared||n.ordinal||n.loopTests||n.serviceLoads)return false;
    }else {
        if(!reader.prepared||n.loopTests!=n.ordinal+1u)return false;
        if(n.stop==S::outerLoop2BAC3C){
            if(n.pc!=0x2bac3cu||n.ordinal!=(n.count?n.count-1u:0u)||n.serviceLoads!=n.ordinal)return false;
        }else if(n.stop==S::countReload||n.stop==S::serviceLoad){
            if(n.pc!=(n.stop==S::countReload?0x2bac28u:0x2bac04u)||n.serviceLoads!=n.ordinal)return false;
        }else if(n.stop==S::read){
            if(n.pc!=0x2bac18u||n.serviceLoads!=n.ordinal+1u||n.ordinal>=reader.reads.size())return false;
        }else return false;
    }
    if(n.retired)return f.nativeWithdrawn&&f.file->nativeClosed()&&absent();
    if(!sourceWaterProvider_||!sourceWaterProvider_->nativePool||!sourceWaterProvider_->nativePool->nativeData)return false;
    const auto& data=*sourceWaterProvider_->nativePool->nativeData;const auto* array=data.nativeIndices.get();
    if(!array||array!=n.arrayIdentity||data.indices20!=array||array->dataOwner!=&data||
       array->dataEpoch!=n.dataEpoch||&data.role!=n.dataRole||!array->storage.ready()||array->storage.count()!=n.count||
       array->storage.identity()==data.payload.get()||
       (data.nativeTopology&&array->storage.identity()==data.nativeTopology->payload.get()))return false;
    for(std::uint32_t i=0;i<n.count;++i){
        const auto* r=i<reader.reads.size()?&reader.reads[i]:nullptr;
        if(!array->storage.matches(i,r?r->bytes:std::array<std::uint8_t,4>{},r?r->copied:0u))return false;
    }
    return true;
}

bool Owner::withdrawNativeWaterPoolIndices()noexcept {
    if(!preflightNativeWaterPoolIndices())return false;
    auto& n=sourceWaterConfig_.nativeWaterPoolIndices;
    if(!n.allocated||n.retired)return true;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    if(nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||!sourceWaterConfig_.nativeWithdrawn||
       !sourceWaterProviderInitializer_.world400NativeWithdrawn||!f.nativeWithdrawn||!f.file->nativeClosed())return false;
    auto& data=*sourceWaterProvider_->nativePool->nativeData;
    data.indices20=nullptr;data.nativeIndices.reset();n.retired=true;
    return true; //No guest free or restoration of constructor zero at+20.
}

Owner::WaterPoolIndexResult Owner::readWaterPoolIndicesNativeOnly(WaterArrayBackingMode mode)noexcept {
    using R=WaterPoolIndexResult;using S=WaterPoolIndexStop;using G=a::OriginalConfigProcessGlobals50LA;
    auto& n=sourceWaterConfig_.nativeWaterPoolIndices;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    R result;
    const auto finish=[&](R::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.pc=n.pc;result.allocatorPC=n.allocator.pc;
        result.count=n.count;result.admittedReads=n.ordinal;result.allocated=n.allocated;result.retired=n.retired;
        result.diagnostic=n.diagnostic;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !preflightNativeWaterPoolDataHistory())return result;
    if(n.attempted)return finish(R::Status::alreadyAttempted);
    std::size_t poolCount=0,materialCount=0;
    if(!sourceWaterPoolCountedNative(poolCount)||!sourceWaterPoolMaterialCountedNative(materialCount)||poolCount!=1u||
       !waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       !t.allocated||t.retired||t.initialization.status!=g::OriginalWaterTopologyInitialization50LA::Status::initialized||
       !f.file||f.file->nativeClosed()||f.file->nativeCloseAttempted_||f.file->sourceCloseAttempted_||
       !f.file->preflightNative(f.wholeRoleIdentity))return result;
    auto& data=*sourceWaterProvider_->nativePool->nativeData;
    const auto currentCount=[&](std::uint32_t& out)noexcept {
        const auto* bytes=static_cast<const std::uint8_t*>(data.payload.get());out=0;
        for(std::size_t i=0;i<4u;++i){if(data.knownMask[0x1cu+i]!=255u)return false;out|=std::uint32_t(bytes[0x1cu+i])<<(8u*i);}
        return out==t.count;
    };
    std::uint32_t count=0;if(!currentCount(count)||data.nativeIndices||data.indices20)return result;
    n.attempted=true;n.count=count;n.dataRole=&data.role;n.dataEpoch=data.role.epoch;
    //Actual Data count reload2BABE4 precedes this new raw malloc occurrence.
    n.pc=0x2babe8u;n.stop=S::sourceReentrancy;
    std::optional<G::StdioIdentity> captured;
    if(G::reentrancyCurrentNative())captured=G::state().reent38544C;
    n.allocator=topologyRawAllocationPrefix(std::uint32_t(count<<2u),captured,0x2babe8u);
    if(n.allocator.reentStop!=SourceWaterConfig::NativeWaterPoolMaterial::RawAllocation::ReentStop::none)
        return finish(R::Status::nativeStopped);
    n.stop=S::nativeReentrancy;
    if(!G::reentrancyCurrentNative())return finish(R::Status::nativeStopped);
    n.nativeReent=G::state().reent38544C;
    if(*n.nativeReent!=G::StdioIdentity::reent385160)return finish(R::Status::nativeStopped);
    n.stop=S::ownerAllocation;n.ownerAttempted=true;
    std::unique_ptr<SourceWaterPoolIndexOwner> pending(new(std::nothrow) SourceWaterPoolIndexOwner);
    if(!pending)return finish(R::Status::nativeStopped);
    n.stop=S::storageAllocation;n.storageResult=pending->storage.prepare(count);
    if(n.storageResult!=g::OriginalWaterIndexStorage50LA::Allocation::ready)return finish(R::Status::nativeStopped);
    pending->dataOwner=&data;pending->dataEpoch=data.role.epoch;data.nativeIndices=std::move(pending);
    n.arrayIdentity=data.nativeIndices.get();n.allocated=true;n.pc=0x2babf0u;
    data.indices20=n.arrayIdentity;n.aliasStored=true;
    for(std::size_t i=0x20u;i<0x24u;++i){data.knownMask[i]=0u;data.knownBits[i]=0u;}
    n.pc=0x2babf4u;n.stop=S::readerPreparation;
    if(!f.file->beginIndexReaderNative(n.dataRole,n.arrayIdentity,n.dataEpoch,count,n.diagnostic))
        return finish(R::Status::nativeStopped);
    //2BABF4/1F8 branches to2BAC28 with actual ordinal zero. Each source loop
    //test reloads count; no signed overflow or host negative shift is used.
    for(;;){
        n.pc=0x2bac28u;n.stop=S::countReload;++n.loopTests;
        std::uint32_t current=0;if(!currentCount(current)||current!=n.count)return finish(R::Status::nativeStopped);
        const auto word=std::uint32_t(current-1u);
        const auto limit=word&0x80000000u?std::int64_t(word)-0x100000000ll:std::int64_t(word);
        if(std::int64_t(n.ordinal)>=limit)break;
        n.pc=0x2bac04u;n.stop=S::serviceLoad;
        if(!waterPoolFileServiceCurrentNative())return finish(R::Status::nativeStopped);
        ++n.serviceLoads;n.pc=0x2bac18u;n.stop=S::read;
        const bool admitted=f.file->readIndex12FB30(n.ordinal,n.diagnostic);
        const auto& read=f.file->indexReader_.reads[n.ordinal];
        if(!data.nativeIndices->storage.copyPrefix(n.ordinal,read.bytes,read.copied))return finish(R::Status::nativeStopped);
        if(!admitted)return finish(R::Status::nativeStopped);
        ++n.ordinal; //2BAC20, then NOP24 and next genuine count test.
    }
    n.pc=0x2bac3cu;n.stop=S::outerLoop2BAC3C;
    return finish(R::Status::sourceBoundary);
    //Branch2BAC3C/delay40, per-record construction and topology return pending.
}
