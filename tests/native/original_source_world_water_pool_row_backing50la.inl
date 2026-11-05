bool Owner::preflightNativeWaterPoolRowBacking()const noexcept {
    if(!preflightNativeWaterPoolRowInput())return false;
    using S=WaterPoolRowBackingStop;using G=a::OriginalConfigProcessGlobals50LA;
    using A=SourceWaterConfig::NativeWaterPoolMaterial::RawAllocation;
    const auto& n=sourceWaterConfig_.nativeWaterPoolRowBacking;
    const auto& parent=sourceWaterConfig_.nativeWaterPoolInnerLoad;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    const auto* topology=sourceWaterProvider_&&sourceWaterProvider_->nativePool&&sourceWaterProvider_->nativePool->nativeData?
        sourceWaterProvider_->nativePool->nativeData->nativeTopology.get():nullptr;
    const auto* object=topology&&topology->firstRecord?topology->firstRecord->firstInner.get():nullptr;
    const auto* rows=object?object->rows.get():nullptr;
    const auto* owner=rows?rows->firstRow.get():nullptr;
    const auto noAllocation=[&]()noexcept {
        return !n.ownerAllocated&&!n.ownerIdentity&&!n.countStored&&!n.retired&&!owner&&!n.stage&&
            n.raw==std::array<A,2>{}&&n.nativeReent==decltype(n.nativeReent){}&&
            n.callEntered==std::array<bool,2>{}&&n.allocationAttempted==std::array<bool,2>{}&&
            n.aliasStored==std::array<bool,2>{};
    };
    for(unsigned i=0;i<4u;++i){
        const auto expected=f.file&&f.file->innerRowCount_.read.entered&&i<f.file->innerRowCount_.read.copied?
            std::optional<std::uint8_t>(f.file->innerRowCount_.read.bytes[i]):std::nullopt;
        if(n.scratch.byte(i)!=expected)return false;
    }
    if(!n.attempted)return n.stop==S::none&&!n.pc&&!n.serviceLoaded&&!n.count&&!n.objectIdentity&&
        !n.rowsIdentity&&noAllocation()&&(!f.file||!f.file->innerRowCount_.read.entered);
    if(parent.stop!=WaterPoolInnerLoadStop::rowLoop299A60||!parent.zeroed||!parent.count||
       n.objectIdentity!=parent.objectIdentity||n.rowsIdentity!=parent.rowsIdentity||!n.objectIdentity||!n.rowsIdentity||
       !f.file||!f.file->innerRowCountHistoryNative())return false;
    const auto& entry=f.file->innerRowCount_;const auto& r=entry.read;
    if(n.stop==S::empty299A70)return !*parent.count&&n.pc==0x299a70u&&!r.entered&&!n.serviceLoaded&&!n.count&&noAllocation();
    if(!*parent.count)return false;
    if(r.entered&&(entry.dataIdentity!=t.dataRole||entry.dataEpoch!=t.dataEpoch||
        entry.objectIdentity!=n.objectIdentity||entry.rowsIdentity!=n.rowsIdentity))return false;
    if(n.stop==S::serviceLoad)return n.pc==0x299954u&&!n.serviceLoaded&&!r.entered&&!n.count&&noAllocation();
    if(!n.serviceLoaded)return false;
    if(n.stop==S::read)return n.pc==0x299964u&&!r.resultAdmitted&&!n.count&&noAllocation();
    if(!r.resultAdmitted)return false;
    if(n.stop==S::unknownCount)return n.pc==0x29996cu&&!n.scratch.word()&&!n.count&&noAllocation();
    if(!n.count||n.count!=n.scratch.word())return false;
    if(n.stop==S::capacity)return n.pc==0x299970u&&*n.count>nativeWaterArrayMaxCount&&noAllocation();
    if(*n.count>nativeWaterArrayMaxCount)return false;
    if(n.stop==S::ownerAllocation)return n.pc==0x299970u&&noAllocation();
    if(!n.ownerAllocated||!n.ownerIdentity||n.stage>2u||(parent.retired&&!n.retired))return false;
    if(n.countStored!=n.aliasStored[0])return false;
    constexpr std::uint32_t pcs[]{0x299970u,0x2999b0u};
    constexpr std::uint32_t strides[]{16u,32u};
    for(unsigned i=0;i<2u;++i){
        if(i>n.stage){
            if(n.callEntered[i]||n.raw[i]!=A{}||n.nativeReent[i]||n.allocationAttempted[i]||n.aliasStored[i])return false;
            continue;
        }
        if(!n.callEntered[i]||n.raw[i]!=topologyRawAllocationPrefix(*n.count*strides[i],n.raw[i].reent,pcs[i]))return false;
        if(i<n.stage){
            if(n.raw[i].reentStop!=A::ReentStop::none||n.nativeReent[i]!=G::StdioIdentity::reent385160||
               !n.allocationAttempted[i]||!n.aliasStored[i])return false;
        }else {
            if(n.pc!=pcs[i]||n.aliasStored[i])return false;
            if(n.stop==S::sourceReentrancy){
                if(n.raw[i].reentStop==A::ReentStop::none||n.nativeReent[i]||n.allocationAttempted[i])return false;
            }else if(n.stop==S::nativeReentrancy){
                if(n.raw[i].reentStop!=A::ReentStop::none||n.nativeReent[i]==G::StdioIdentity::reent385160||n.allocationAttempted[i])return false;
            }else if(n.stop==S::payloadAllocation){
                if(n.raw[i].reentStop!=A::ReentStop::none||n.nativeReent[i]!=G::StdioIdentity::reent385160||!n.allocationAttempted[i])return false;
            }else return false;
        }
    }
    if(n.stage==2u&&(n.stop!=S::bulkRead2999CC||n.pc!=0x2999ccu))return false;
    if(n.retired)return !owner&&f.nativeWithdrawn&&f.file->nativeClosed();
    if(!object||object!=n.objectIdentity||!rows||rows!=n.rowsIdentity||!owner||owner!=n.ownerIdentity||
       owner->rows!=rows||owner->count!=*n.count)return false;
    const auto& storage=owner->storage;
    if(bool(storage.normalsIdentity())!=n.aliasStored[0]||bool(storage.verticesIdentity())!=n.aliasStored[1]||
       owner->normals0C!=storage.normalsIdentity()||owner->vertices08!=storage.verticesIdentity()||storage.outputIdentity())return false;
    if(!sourceWaterConfig_.nativeWaterPoolRowInput.attempted&&
       (storage.normalByte(0,0)||storage.vertexByte(0)||storage.outputXYZ(0)))return false;
    return true;
}

bool Owner::withdrawNativeWaterPoolRowBacking()noexcept {
    if(!preflightNativeWaterPoolRowBacking())return false;
    auto& n=sourceWaterConfig_.nativeWaterPoolRowBacking;
    if(!n.ownerAllocated||n.retired)return true;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    if(nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||!sourceWaterConfig_.nativeWithdrawn||
       !sourceWaterProviderInitializer_.world400NativeWithdrawn||!f.nativeWithdrawn||!f.file->nativeClosed())return false;
    auto& rows=*sourceWaterProvider_->nativePool->nativeData->nativeTopology->firstRecord->firstInner->rows;
    rows.firstRow.reset();n.retired=true;return true;
}

Owner::WaterPoolRowBackingResult Owner::prepareFirstWaterPoolRowNativeOnly(WaterArrayBackingMode mode)noexcept {
    using R=WaterPoolRowBackingResult;using S=WaterPoolRowBackingStop;using G=a::OriginalConfigProcessGlobals50LA;
    auto& n=sourceWaterConfig_.nativeWaterPoolRowBacking;
    const auto& parent=sourceWaterConfig_.nativeWaterPoolInnerLoad;
    const auto& t=sourceWaterConfig_.nativeWaterPoolTopologyBacking;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    R result;const auto finish=[&](R::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.pc=n.pc;result.count=n.count;result.stage=n.stage;
        result.retired=n.retired;result.diagnostic=n.diagnostic;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !preflightNativeWaterPoolDataHistory())return result;
    if(n.attempted)return finish(R::Status::alreadyAttempted);
    std::size_t pools=0,materials=0;
    if(!sourceWaterPoolCountedNative(pools)||!sourceWaterPoolMaterialCountedNative(materials)||pools!=1u||
       !waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       parent.stop!=WaterPoolInnerLoadStop::rowLoop299A60||parent.retired||!f.file||f.file->nativeClosed()||
       f.file->nativeCloseAttempted_||f.file->sourceCloseAttempted_||!f.file->preflightNative(f.wholeRoleIdentity))return result;
    auto& object=*sourceWaterProvider_->nativePool->nativeData->nativeTopology->firstRecord->firstInner;
    auto& rows=*object.rows;
    n.attempted=true;n.objectIdentity=&object;n.rowsIdentity=&rows;
    if(!rows.count){n.pc=0x299a70u;n.stop=S::empty299A70;return finish(R::Status::sourceBoundary);}
    n.pc=0x299954u;n.stop=S::serviceLoad;
    if(!waterPoolFileServiceCurrentNative())return finish(R::Status::nativeStopped);
    n.serviceLoaded=true;n.pc=0x299964u;n.stop=S::read;
    const bool admitted=f.file->readInnerRowCount12FB30(t.dataRole,&object,&rows,t.dataEpoch,n.diagnostic);
    const auto& read=f.file->innerRowCount_.read;
    if(!n.scratch.copyPrefix(std::span(read.bytes).first(read.copied)))std::terminate();
    if(!admitted)return finish(R::Status::nativeStopped);
    n.pc=0x29996cu;n.stop=S::unknownCount;
    n.count=n.scratch.word();if(!n.count)return finish(R::Status::nativeStopped);
    n.pc=0x299970u;n.stop=S::capacity;
    if(*n.count>nativeWaterArrayMaxCount)return finish(R::Status::nativeStopped);
    n.stop=S::ownerAllocation;rows.firstRow.reset(new(std::nothrow) SourceWaterPoolFirstRowOwner);
    if(!rows.firstRow)return finish(R::Status::nativeStopped);
    auto& owner=*rows.firstRow;owner.rows=&rows;owner.count=*n.count;n.ownerAllocated=true;n.ownerIdentity=&owner;
    constexpr std::uint32_t pcs[]{0x299970u,0x2999b0u};
    constexpr std::uint32_t strides[]{16u,32u};
    for(unsigned i=0;i<2u;++i){
        n.stage=i;n.pc=pcs[i];n.stop=S::sourceReentrancy;n.callEntered[i]=true;
        std::optional<G::StdioIdentity> captured;
        if(G::reentrancyCurrentNative())captured=G::state().reent38544C;
        n.raw[i]=topologyRawAllocationPrefix(*n.count*strides[i],captured,pcs[i]);
        if(n.raw[i].reentStop!=SourceWaterConfig::NativeWaterPoolMaterial::RawAllocation::ReentStop::none)return finish(R::Status::nativeStopped);
        n.stop=S::nativeReentrancy;
        if(G::reentrancyCurrentNative())n.nativeReent[i]=G::state().reent38544C;
        if(n.nativeReent[i]!=G::StdioIdentity::reent385160)return finish(R::Status::nativeStopped);
        n.stop=S::payloadAllocation;n.allocationAttempted[i]=true;
        if(i==0u){
            if(!owner.storage.allocateNormals(*n.count))return finish(R::Status::nativeStopped);
            owner.normals0C=owner.storage.normalsIdentity();n.aliasStored[0]=true;
            auto* bytes=static_cast<std::uint8_t*>(rows.payload.get());
            //29998C/299998 clear output and auxiliary fields;2999A8 stores N.
            for(unsigned j=0;j<4u;++j){bytes[16u+j]=0;bytes[4u+j]=0;bytes[20u+j]=std::uint8_t(*n.count>>(8u*j));}
            n.countStored=true;
        }else {
            if(!owner.storage.allocateVertices())return finish(R::Status::nativeStopped);
            owner.vertices08=owner.storage.verticesIdentity();n.aliasStored[1]=true;
        }
    }
    n.stage=2u;n.pc=0x2999ccu;n.stop=S::bulkRead2999CC;
    return finish(R::Status::sourceBoundary); //Before fresh service load and32*N input read.
}
