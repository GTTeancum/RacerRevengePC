bool Owner::preflightNativeWaterPoolRecordCount()const noexcept {
    using S=WaterPoolRecordCountStop;
    const auto& n=sourceWaterConfig_.nativeWaterPoolRecordCount;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& indices=sourceWaterConfig_.nativeWaterPoolIndices;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    if(!n.attempted)return !n.serviceLoaded&&n.stop==S::none&&!n.pc&&!n.dataRole&&
        !n.topologyIdentity&&!n.dataEpoch&&!n.count&&(!f.file||!f.file->firstTopologyRecord_.read.entered);
    if(!t.initialization.zeroFillReturned||!t.count||indices.stop!=WaterPoolIndexStop::outerLoop2BAC3C||
       !n.dataRole||n.dataRole!=t.dataRole||n.dataEpoch!=t.dataEpoch||!n.topologyIdentity||
       n.topologyIdentity!=t.backingIdentity||!f.file||!f.file->firstTopologyRecordHistoryNative())return false;
    const auto& entry=f.file->firstTopologyRecord_;const auto& r=entry.read;
    if(n.stop==S::empty2BAE10)return !*t.count&&n.pc==0x2bae10u&&!n.count&&!n.serviceLoaded&&!r.entered;
    if(!*t.count)return false;
    if(r.entered&&(entry.dataIdentity!=n.dataRole||entry.topologyIdentity!=n.topologyIdentity||entry.dataEpoch!=n.dataEpoch))return false;
    if(n.stop==S::serviceLoad)return n.pc==0x2bac54u&&!n.count&&!n.serviceLoaded&&!r.entered;
    if(!n.serviceLoaded)return false;
    if(n.stop==S::read)return n.pc==0x2bac68u&&!n.count&&!r.resultAdmitted;
    if(n.stop!=S::malloc2BAC88||n.pc!=0x2bac88u||!r.resultAdmitted||!n.count)return false;
    std::uint32_t word=0;for(unsigned i=0;i<4;++i)word|=std::uint32_t(r.bytes[i])<<(8u*i);
    return *n.count==word; //Historical opaque identities only; no freed-owner reads.
}

Owner::WaterPoolRecordCountResult Owner::readFirstWaterPoolRecordNativeOnly(WaterArrayBackingMode mode)noexcept {
    using R=WaterPoolRecordCountResult;using S=WaterPoolRecordCountStop;
    auto& n=sourceWaterConfig_.nativeWaterPoolRecordCount;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& indices=sourceWaterConfig_.nativeWaterPoolIndices;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    R result;
    const auto finish=[&](R::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.pc=n.pc;result.count=n.count;result.diagnostic=n.diagnostic;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !preflightNativeWaterPoolDataHistory())return result;
    if(n.attempted)return finish(R::Status::alreadyAttempted);
    std::size_t pools=0,materials=0;
    if(!sourceWaterPoolCountedNative(pools)||!sourceWaterPoolMaterialCountedNative(materials)||pools!=1u||
       !waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       indices.stop!=WaterPoolIndexStop::outerLoop2BAC3C||indices.retired||!t.count||t.retired||
       !t.initialization.zeroFillReturned||!f.file||f.file->nativeClosed()||f.file->nativeCloseAttempted_||
       f.file->sourceCloseAttempted_||!f.file->preflightNative(f.wholeRoleIdentity))return result;
    auto& data=*sourceWaterProvider_->nativePool->nativeData;
    //The actual count reload at2BAE00 follows branch2BAC3C/delay40 (ordinal0).
    std::uint32_t topologyCount=0;const auto* raw=static_cast<const std::uint8_t*>(data.payload.get());
    for(unsigned i=0;i<4;++i){if(data.knownMask[0x1cu+i]!=255u)return result;topologyCount|=std::uint32_t(raw[0x1cu+i])<<(8u*i);}
    if(topologyCount!=*t.count)return result;
    n.attempted=true;n.dataRole=&data.role;n.dataEpoch=data.role.epoch;n.topologyIdentity=data.nativeTopology.get();
    if(!topologyCount){
        n.pc=0x2bae10u;n.stop=S::empty2BAE10;
        return finish(R::Status::sourceBoundary); //Saved-register restores/return remain unissued.
    }
    n.pc=0x2bac54u;n.stop=S::serviceLoad;
    if(!waterPoolFileServiceCurrentNative())return finish(R::Status::nativeStopped);
    n.serviceLoaded=true;n.pc=0x2bac68u;n.stop=S::read;
    const bool admitted=f.file->readFirstTopologyRecord12FB30(n.dataRole,n.topologyIdentity,n.dataEpoch,n.diagnostic);
    const auto& read=f.file->firstTopologyRecord_.read;
    auto* destination=static_cast<std::uint8_t*>(data.nativeTopology->payload.get());
    //Preserve actual partial helper effects even when no source return is admitted.
    for(std::size_t i=0;i<read.copied;++i)destination[i]=read.bytes[i];
    if(!admitted)return finish(R::Status::nativeStopped);
    std::uint32_t count=0;for(unsigned i=0;i<4;++i)count|=std::uint32_t(destination[i])<<(8u*i);
    n.count=count;n.pc=0x2bac88u;n.stop=S::malloc2BAC88;
    return finish(R::Status::sourceBoundary); //Before malloc JAL and2*N delay-slot computation.
}
