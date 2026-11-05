bool Owner::preflightNativeWaterPoolRowInput()const noexcept {
    using S=WaterPoolRowInputStop;
    const auto& n=sourceWaterConfig_.nativeWaterPoolRowInput;
    const auto& b=sourceWaterConfig_.nativeWaterPoolRowBacking;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    if(!n.attempted)return n.stop==S::none&&!n.pc&&!n.prepared&&!n.verticesLoaded&&!n.verticesCommitted&&
        !n.ordinal&&!n.normalsLoaded&&!n.normalsCommitted&&!n.rowIdentity&&(!f.file||!f.file->rowInputs_.entered);
    if(b.stop!=WaterPoolRowBackingStop::bulkRead2999CC||b.stage!=2u||!b.count||
       !n.rowIdentity||n.rowIdentity!=b.ownerIdentity||!f.file||!f.file->rowInputsHistoryNative())return false;
    const auto& inputs=f.file->rowInputs_;const auto& v=inputs.vertices;
    if(inputs.entered&&(inputs.rowIdentity!=n.rowIdentity||inputs.dataIdentity!=t.dataRole||inputs.dataEpoch!=t.dataEpoch||
       inputs.rowsIdentity!=b.rowsIdentity||inputs.objectIdentity!=b.objectIdentity||inputs.count!=*b.count))return false;
    if(n.prepared!=inputs.prepared||n.ordinal>*b.count||n.normalsLoaded>*b.count||n.normalsCommitted>*b.count||
       n.verticesCommitted!=v.attempted)return false;
    std::uint32_t committed=0;for(const auto& r:inputs.normals)if(r.attempted)++committed;
    if(committed!=n.normalsCommitted)return false;
    const auto* topology=sourceWaterProvider_&&sourceWaterProvider_->nativePool&&sourceWaterProvider_->nativePool->nativeData?
        sourceWaterProvider_->nativePool->nativeData->nativeTopology.get():nullptr;
    const auto* object=topology&&topology->firstRecord?topology->firstRecord->firstInner.get():nullptr;
    const auto* rows=object?object->rows.get():nullptr;
    const auto* row=rows?rows->firstRow.get():nullptr;
    if(b.retired){if(row||!f.nativeWithdrawn||!f.file->nativeClosed())return false;}
    else {
        if(!row||row!=n.rowIdentity||rows!=b.rowsIdentity||object!=b.objectIdentity)return false;
        const auto& storage=row->storage;
        for(std::size_t i=0;i<std::size_t(*b.count)*32u;++i){
            const auto expected=n.verticesCommitted&&i<v.copied?std::optional<std::uint8_t>(v.bytes[i]):std::nullopt;
            if(storage.vertexByte(i)!=expected)return false;
        }
        for(std::uint32_t i=0;i<*b.count;++i)for(unsigned j=0;j<16u;++j){
            std::optional<std::uint8_t> expected;
            if(i<inputs.normals.size()&&inputs.normals[i].attempted&&j<inputs.normals[i].copied)expected=inputs.normals[i].bytes[j];
            if(storage.normalByte(i,j)!=expected)return false;
        }
        if(storage.outputIdentity()||storage.outputXYZ(0))return false;
    }
    const auto noNormals=[&]()noexcept {return !n.ordinal&&!n.normalsLoaded&&!n.normalsCommitted;};
    if(n.stop==S::preparation)return n.pc==0x2999ccu&&!n.prepared&&!n.verticesLoaded&&!v.entered&&noNormals();
    if(!n.prepared)return false;
    if(n.stop==S::bulkService)return n.pc==0x2999d0u&&!n.verticesLoaded&&!v.entered&&noNormals();
    if(!n.verticesLoaded)return false;
    if(n.stop==S::bulkRead)return n.pc==0x2999e0u&&!v.resultAdmitted&&noNormals();
    if(!v.resultAdmitted||!n.verticesCommitted)return false;
    for(std::uint32_t i=0;i<n.ordinal;++i)if(!inputs.normals[i].resultAdmitted)return false;
    for(std::uint32_t i=n.ordinal+1u;i<*b.count;++i)if(inputs.normals[i].entered)return false;
    if(n.stop==S::calculation299A38)return n.pc==0x299a38u&&n.ordinal==*b.count&&
        n.normalsLoaded==*b.count&&n.normalsCommitted==*b.count;
    if(n.ordinal>=*b.count)return false;
    const auto& r=inputs.normals[n.ordinal];
    if(n.stop==S::normalService)return n.pc==0x299a04u&&!r.entered&&n.normalsLoaded==n.ordinal&&n.normalsCommitted==n.ordinal;
    return n.stop==S::normalRead&&n.pc==0x299a0cu&&!r.resultAdmitted&&n.normalsLoaded==n.ordinal+1u&&
        n.normalsCommitted==n.ordinal+(r.attempted?1u:0u);
}

Owner::WaterPoolRowInputResult Owner::readFirstWaterPoolRowNativeOnly(WaterArrayBackingMode mode)noexcept {
    using R=WaterPoolRowInputResult;using S=WaterPoolRowInputStop;
    auto& n=sourceWaterConfig_.nativeWaterPoolRowInput;
    const auto& b=sourceWaterConfig_.nativeWaterPoolRowBacking;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    R result;const auto finish=[&](R::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.pc=n.pc;result.ordinal=n.ordinal;
        result.verticesCommitted=n.verticesCommitted;result.normalsCommitted=n.normalsCommitted;
        result.diagnostic=n.diagnostic;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !preflightNativeWaterPoolDataHistory())return result;
    if(n.attempted)return finish(R::Status::alreadyAttempted);
    std::size_t pools=0,materials=0;
    if(!sourceWaterPoolCountedNative(pools)||!sourceWaterPoolMaterialCountedNative(materials)||pools!=1u||
       !waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       b.stop!=WaterPoolRowBackingStop::bulkRead2999CC||b.retired||!f.file||f.file->nativeClosed()||
       f.file->nativeCloseAttempted_||f.file->sourceCloseAttempted_||!f.file->preflightNative(f.wholeRoleIdentity))return result;
    auto& row=*sourceWaterProvider_->nativePool->nativeData->nativeTopology->firstRecord->firstInner->rows->firstRow;
    n.attempted=true;n.rowIdentity=&row;n.stop=S::preparation;n.pc=0x2999ccu;
    if(!f.file->prepareRowInputsNative(t.dataRole,b.objectIdentity,b.rowsIdentity,&row,t.dataEpoch,*b.count,n.diagnostic))
        return finish(R::Status::nativeStopped);
    n.prepared=true;n.stop=S::bulkService;n.pc=0x2999d0u;
    if(!waterPoolFileServiceCurrentNative())return finish(R::Status::nativeStopped);
    n.verticesLoaded=true;n.stop=S::bulkRead;n.pc=0x2999e0u;
    const bool bulk=f.file->readRowInput12FB30(false,0,n.diagnostic);
    const auto& v=f.file->rowInputs_.vertices;
    if(v.attempted){
        if(!row.storage.copyVertexPrefix(std::span(v.bytes).first(v.copied)))std::terminate();
        n.verticesCommitted=true;
    }
    if(!bulk)return finish(R::Status::nativeStopped);
    for(n.ordinal=0;n.ordinal<*b.count;++n.ordinal){
        n.stop=S::normalService;n.pc=0x299a04u;
        if(!waterPoolFileServiceCurrentNative())return finish(R::Status::nativeStopped);
        ++n.normalsLoaded;n.stop=S::normalRead;n.pc=0x299a0cu;
        const bool normal=f.file->readRowInput12FB30(true,n.ordinal,n.diagnostic);
        const auto& r=f.file->rowInputs_.normals[n.ordinal];
        if(r.attempted){
            if(!row.storage.copyNormalPrefix(n.ordinal,std::span(r.bytes).first(r.copied)))std::terminate();
            ++n.normalsCommitted;
        }
        if(!normal)return finish(R::Status::nativeStopped);
    }
    n.pc=0x299a38u;n.stop=S::calculation299A38;
    return finish(R::Status::sourceBoundary); //Before299420 call; output allocation and calculation pending.
}
