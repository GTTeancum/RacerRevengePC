bool Owner::preflightNativeWaterPoolFirstElement()const noexcept {
    using S=WaterPoolElementStop;
    const auto& n=sourceWaterConfig_.nativeWaterPoolFirstElement;
    const auto& b=sourceWaterConfig_.nativeWaterPoolRecordBacking;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    if(!n.attempted){
        for(unsigned i=0;i<4;++i)if(n.scratch.byte(i))return false;
        return !n.serviceLoaded&&!n.halfwordStored&&!n.halfword&&!n.recordIdentity&&!n.pc&&n.stop==S::none&&
            (!f.file||!f.file->firstTopologyElement_.read.entered);
    }
    if(b.stop!=WaterPoolRecordBackingStop::innerLoop2BADD8||!n.recordIdentity||n.recordIdentity!=b.ownerIdentity||
       !f.file||!f.file->firstTopologyElementHistoryNative())return false;
    const auto& entry=f.file->firstTopologyElement_;const auto& r=entry.read;
    for(unsigned i=0;i<4;++i){
        const auto byte=n.scratch.byte(i);
        if(i<r.copied){if(byte!=r.bytes[i])return false;}else if(byte)return false;
    }
    if(n.stop==S::empty2BADFC)return !b.count&&n.pc==0x2badfcu&&!r.entered&&
        !n.serviceLoaded&&!n.halfword&&!n.halfwordStored;
    if(!b.count)return false;
    if(r.entered&&(entry.dataIdentity!=t.dataRole||entry.topologyIdentity!=t.backingIdentity||entry.dataEpoch!=t.dataEpoch))return false;
    if(n.stop==S::serviceLoad)return n.pc==0x2bad38u&&!n.serviceLoaded&&!r.entered&&!n.halfword&&!n.halfwordStored;
    if(!n.serviceLoaded)return false;
    if(n.stop==S::read)return n.pc==0x2bad48u&&!r.resultAdmitted&&!n.halfword&&!n.halfwordStored;
    if(!r.resultAdmitted)return false;
    if(n.stop==S::unknownHalfword)return n.pc==0x2bad5cu&&!n.scratch.signedHalfword()&&!n.halfword&&!n.halfwordStored;
    if(n.stop!=S::newEntry2E8D90||n.pc!=0x2e8d90u||!n.halfwordStored||!n.halfword||n.halfword!=n.scratch.signedHalfword())return false;
    if(b.retired)return f.nativeWithdrawn&&f.file->nativeClosed();
    if(!sourceWaterProvider_||!sourceWaterProvider_->nativePool||!sourceWaterProvider_->nativePool->nativeData)return false;
    const auto* topology=sourceWaterProvider_->nativePool->nativeData->nativeTopology.get();
    const auto* owner=topology?topology->firstRecord.get():nullptr;
    if(!owner||owner!=n.recordIdentity||!owner->halfwords)return false;
    const auto* bytes=static_cast<const std::uint8_t*>(owner->halfwords.get());
    const auto bits=std::uint32_t(*n.halfword)&65535u;
    return bytes[0]==std::uint8_t(bits)&&bytes[1]==std::uint8_t(bits>>8u);
}

Owner::WaterPoolElementResult Owner::readFirstWaterPoolElementNativeOnly(WaterArrayBackingMode mode)noexcept {
    using R=WaterPoolElementResult;using S=WaterPoolElementStop;
    auto& n=sourceWaterConfig_.nativeWaterPoolFirstElement;
    const auto& b=sourceWaterConfig_.nativeWaterPoolRecordBacking;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    R result;const auto finish=[&](R::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.pc=n.pc;result.halfword=n.halfword;
        result.halfwordStored=n.halfwordStored;result.diagnostic=n.diagnostic;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !preflightNativeWaterPoolDataHistory())return result;
    if(n.attempted)return finish(R::Status::alreadyAttempted);
    std::size_t pools=0,materials=0;
    if(!sourceWaterPoolCountedNative(pools)||!sourceWaterPoolMaterialCountedNative(materials)||pools!=1u||
       !waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       b.stop!=WaterPoolRecordBackingStop::innerLoop2BADD8||b.retired||!f.file||f.file->nativeClosed()||
       f.file->nativeCloseAttempted_||f.file->sourceCloseAttempted_||!f.file->preflightNative(f.wholeRoleIdentity))return result;
    auto& topology=*sourceWaterProvider_->nativePool->nativeData->nativeTopology;
    const auto* raw=static_cast<const std::uint8_t*>(topology.payload.get());std::uint32_t count=0;
    for(unsigned i=0;i<4;++i)count|=std::uint32_t(raw[i])<<(8u*i);
    if(count!=b.count)return result;
    n.attempted=true;n.recordIdentity=topology.firstRecord.get();
    if(!count){n.pc=0x2badfcu;n.stop=S::empty2BADFC;return finish(R::Status::sourceBoundary);}
    n.pc=0x2bad38u;n.stop=S::serviceLoad;
    if(!waterPoolFileServiceCurrentNative())return finish(R::Status::nativeStopped);
    n.serviceLoaded=true;n.pc=0x2bad48u;n.stop=S::read;
    const bool admitted=f.file->readFirstTopologyElement12FB30(t.dataRole,t.backingIdentity,t.dataEpoch,n.diagnostic);
    const auto& read=f.file->firstTopologyElement_.read;
    if(!n.scratch.copyPrefix(std::span<const std::uint8_t>(read.bytes).first(read.copied)))std::terminate();
    if(!admitted)return finish(R::Status::nativeStopped);
    n.pc=0x2bad5cu;n.stop=S::unknownHalfword;n.halfword=n.scratch.signedHalfword();
    if(!n.halfword)return finish(R::Status::nativeStopped);
    //JAL2BAD78 with actual a0=12; its delay2BAD7C stores only the low half.
    auto* destination=static_cast<std::uint8_t*>(topology.firstRecord->halfwords.get());
    const auto bits=std::uint32_t(*n.halfword)&65535u;
    destination[0]=std::uint8_t(bits);destination[1]=std::uint8_t(bits>>8u);
    n.halfwordStored=true;n.pc=0x2e8d90u;n.stop=S::newEntry2E8D90;
    return finish(R::Status::sourceBoundary); //Scalar-new body/construction remain unissued.
}
