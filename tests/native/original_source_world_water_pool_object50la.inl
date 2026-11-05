//Native-success custody for the distinct first scalar Pool occurrence. This
//leaf never returns the original allocator or publishes an array element.
//Its descendant scene/Pool and name leaves keep separate history. Count validation is LOCAL.
bool Owner::sourceWaterPoolCountedNative(std::size_t& count)const noexcept {
    count=0;const auto& b=sourceWaterConfig_.nativeWaterPoolBacking;
    const auto& c=sourceWaterConfig_.nativeWaterPoolEntity;
    const auto& s=sourceWaterConfig_.nativeWaterPoolScene;
    const auto& name=sourceWaterConfig_.nativeWaterPoolName;
    using B=WaterPoolBackingRefusal;using C=WaterPoolEntityRefusal;
    using G=a::OriginalConfigProcessGlobals50LA;
    const auto noEntityEffects=[&]()noexcept {
        return !c.admitted&&!c.poolFrame&&!c.sceneCallIssued&&!c.sceneFrame&&!c.wholeSaved&&
            !c.entityCallIssued&&!c.entityFrame&&!c.referenceCallIssued&&!c.poolSaveMask&&!c.sceneSaveMask&&
            !c.entitySaveMask&&!c.entityRestoreMask&&!c.referenceDispatchStored&&!c.referenceAcquired&&
            !c.referenceReturned&&!c.entityDispatchStored&&!c.countAcquired&&!c.enrolled&&!c.entityReturned&&
            !c.entityStackReleased&&!c.authorityRetired&&!c.referenceRetired&&!c.countRetired&&
            !c.countBefore&&!c.session&&!c.key&&!c.header00&&!c.header02&&!c.category30&&
            !c.header03Mask&&!c.header03Bits&&!c.flags31Mask&&!c.flags31Bits&&!c.linksWritten&&
            !c.mask28Stored&&!c.descendants2CStored;
    };
    if(!c.attempted){if(c.pc||c.refusal!=C::none||!noEntityEffects())return false;}
    else {
        if(!b.nativeAllocated)return false;
        if(c.refusal!=C::none){
            if((c.refusal!=C::countCapacity&&c.refusal!=C::authorityCapacity&&c.refusal!=C::authorityAdmission)||
               c.pc!=0x2bae30u||!noEntityEffects())return false;
        }else if(!c.admitted||c.pc!=0x289420u||!c.poolFrame||!c.sceneCallIssued||!c.sceneFrame||!c.wholeSaved||
            !c.entityCallIssued||!c.entityFrame||!c.referenceCallIssued||c.poolSaveMask!=3u||c.sceneSaveMask!=1u||
            c.entitySaveMask!=7u||c.entityRestoreMask!=7u||!c.referenceDispatchStored||!c.referenceAcquired||
            !c.referenceReturned||!c.entityDispatchStored||!c.countAcquired||!c.enrolled||!c.entityReturned||
            !c.entityStackReleased||!c.countBefore||*c.countBefore==std::numeric_limits<std::uint32_t>::max()||
            !c.session||!*c.session||!c.key||!*c.key||c.header00!=0u||c.header02!=0xffu||c.category30!=0xffu||
            c.header03Mask!=7u||c.header03Bits||c.flags31Mask!=15u||c.flags31Bits!=3u||c.linksWritten!=4u||
            !c.mask28Stored||!c.descendants2CStored||c.authorityRetired!=b.nativeRetired||
            c.referenceRetired!=b.nativeRetired||c.countRetired!=b.nativeRetired)return false;
    }
    if(!preflightNativeWaterPoolSceneHistory()||!preflightNativeWaterPoolNameHistory()||
       !preflightNativeWaterPoolResourceHistory()||!preflightNativeWaterPoolDataHistory())return false;
    const auto noOwner=[&]()noexcept{return !sourceWaterProvider_||!sourceWaterProvider_->nativePool;};
    const auto noCaller=[&]()noexcept {
        return !b.pc&&!b.callerEntered&&!b.constructorCallIssued&&!b.wholeArgumentPassed&&
            !b.nullBranchTaken&&!b.argumentA1&&!b.argumentA2&&!b.callerReturnPC;
    };
    const auto noPayload=[&]()noexcept {
        return !b.payloadAttempted&&!b.nativeAllocated&&!b.nativeRetired&&!b.viewsRetired&&!b.epoch&&noCaller()&&noOwner();
    };
    if(!b.attempted)return !b.ownerAttempted&&!b.ownerPrepared&&!b.temporaryOwnerRetired&&noPayload()&&
        b.refusal==B::none&&!b.nativeAdmissionReent&&!b.aggregateIdentity&&!b.providerIdentity&&!b.arrayEpoch&&
        !b.ordinal&&!b.requestedBytes&&!b.effectiveBytes&&!c.attempted;
    const auto& scalar=sourceWaterConfig_.nativeWaterPoolAllocationEntry;
    const auto& format=sourceWaterConfig_.nativeWaterPoolPathFormat;
    if(b.aggregateIdentity!=this||!b.providerIdentity||b.arrayEpoch!=1u||b.ordinal!=0u||
       b.requestedBytes!=0x1d0u||b.effectiveBytes!=0x1d0u||!scalar.attempted||!scalar.callIssued||
       scalar.pc!=0x3059ecu||!scalar.lockReturned||scalar.requestedBytes!=b.requestedBytes||
       scalar.effectiveBytes!=b.effectiveBytes||scalar.callerReturnPC!=0x2b9d14u||
       !format.attempted||format.pc!=0x2b9d0cu||!format.terminated||!format.wrapperReleased||
       format.phase!=SourceWaterConfig::NativeWaterPoolPathFormat::Phase::callerBoundary)return false;
    if(b.refusal==B::reentrancyUnknown)return !b.nativeAdmissionReent&&!b.ownerAttempted&&!b.ownerPrepared&&
        !b.temporaryOwnerRetired&&noPayload()&&!c.attempted;
    if(b.refusal==B::reentrancyUnsupported)return b.nativeAdmissionReent&&
        *b.nativeAdmissionReent!=G::StdioIdentity::reent385160&&!b.ownerAttempted&&!b.ownerPrepared&&
        !b.temporaryOwnerRetired&&noPayload()&&!c.attempted;
    if(b.nativeAdmissionReent!=G::StdioIdentity::reent385160||!b.ownerAttempted)return false;
    if(b.refusal==B::ownerAllocation)return !b.ownerPrepared&&!b.temporaryOwnerRetired&&noPayload()&&!c.attempted;
    if(!b.ownerPrepared||!b.payloadAttempted)return false;
    if(b.refusal==B::payloadAllocation)return b.temporaryOwnerRetired&&!b.nativeAllocated&&!b.nativeRetired&&
        !b.viewsRetired&&!b.epoch&&noCaller()&&noOwner()&&!c.attempted;
    if(b.refusal!=B::none||b.temporaryOwnerRetired||!b.nativeAllocated||b.epoch!=1u||b.pc!=0x2bae30u||
       !b.callerEntered||b.nullBranchTaken!=false||b.argumentA1!=1u||b.argumentA2!=1u||
       !b.constructorCallIssued||!b.wholeArgumentPassed||b.callerReturnPC!=0x2b9d28u||
       b.viewsRetired!=b.nativeRetired)return false;
    //Retired custody is a valid intermediate state if the later provider
    //retirement refuses. Historical flags/PCs do not dereference dead roles.
    if(b.nativeRetired)return sourceWaterConfig_.nativeWithdrawn&&noOwner();
    if(sourceWaterProvider274720_.nativeWithdrawn||!sourceWaterProvider_||!sourceWaterProvider_->nativePool||
       b.providerIdentity!=sourceWaterProvider_.get()||!worldApplication_||
       nativeScbThread_!=std::this_thread::get_id()||worldApplication_->attachmentLock423F58())return false;
    const auto& p=*sourceWaterProvider_->nativePool;const auto& e=p.entity;
    if(s.attempted&&(s.applicationIdentity!=worldApplication_.get()||
       s.registryIdentity!=&worldApplication_->types383408_||s.wholeRoleIdentity!=&p.role))return false;
    if(name.attempted&&(name.wholeRoleIdentity!=&p.role||name.canonicalEntityIdentity!=&e))return false;
    const auto& resource=sourceWaterConfig_.nativeWaterPoolResource;
    if(resource.attempted&&(resource.wholeRoleIdentity!=&p.role||resource.canonicalEntityIdentity!=&e))return false;
    //Identity checks only. Historical shared-search observations never demand
    //that the CURRENT cache cursors, association or provider revision match.
    if(resource.load2BB044.resolved&&(!resource.file||!resource.file->nativeClosed())){
        if(!waterPoolFileServiceCurrentNative())return false;
        const auto& first=resource.load2BB044;
        if(first.applicationIdentity!=sourceLoadApplication_||first.domainIdentity!=sourceTrackMountDomain_.get()||
           first.resolverIdentity!=sourceTrackMountDomain_->resolver3E1A70().get()||
           first.providerIdentity!=sourceTrackMountDomain_->archiveProvider3E1A70().get()||
           first.trackIdentity!=sourceTrackFile_.get())return false;
    }
    const auto& array=sourceWaterProvider_->nativeArrayBacking;const auto& view=sourceWaterProvider_->nativeArray68;
    if(!p.payload||p.payload.get()==array.payload.get()||p.role.kind!=NativeWaterArrayElementRole::Kind::pool||
       p.role.wholeOwner!=&p||p.role.payloadIdentity!=p.payload.get()||p.role.entityFacet!=&e||
       p.role.epoch!=*b.epoch||p.role.arrayEpoch!=*b.arrayEpoch||p.role.ordinal!=*b.ordinal||
       !p.wholeArgument||p.wholeArgument->role!=&p.role||p.wholeArgument->offset||
       bool(p.entityArgument)!=c.admitted||(p.entityArgument&&(p.entityArgument->role!=&p.role||p.entityArgument->offset!=8u))||
       !array.payload||!array.slots||array.role!=NativeWaterArrayRole::poolPointers||array.epoch!=*b.arrayEpoch||
       *b.ordinal>=array.originalCount||array.slotCapacity!=array.originalCount+1u||
       !view||view->role!=NativeWaterArrayRole::poolPointers||view->epoch!=array.epoch||view->offset||
       sourceWaterProvider_->numPools64!=array.originalCount)return false;
    for(std::uint32_t i=0;i<array.slotCapacity;++i)
        if(array.slots[i].known||array.slots[i].role||array.slots[i].offset)return false;
    if(e.previous10||e.next14||e.firstChild18||e.parent1C||e.mask20||e.descendants24||
       e.nativeFlags29||e.nativeEffectControlId)return false;
    //Only full-known bytes are read from raw payload. Partially written
    //flags retain unknown upper bits in the sidecar, never via an indeterminate read.
    const auto* bytes=static_cast<const unsigned char*>(p.payload.get());
    for(std::size_t i=0;i<p.knownMask.size();++i){
        std::uint8_t mask=0,bits=0;
        if(c.admitted){
            if(i<2u)mask=0xffu;
            else if(i==2u){mask=0xffu;bits=s.registryReturned?*s.returnedType:0xffu;}
            else if(i==3u){mask=7u;bits=s.poolReturned?3u:0u;}
            else if(i>=8u&&i<16u){mask=0xffu;bits=i==8u?1u:0u;}
            else if(i>=16u&&i<20u){mask=0xffu;
                const auto dispatch=s.poolReturned?0x3d47f8u:s.attempted?0x3d3198u:0x3cd240u;
                bits=std::uint8_t(dispatch>>((i-16u)*8u));}
            else if(i>=24u&&i<48u)mask=0xffu;
            else if(i==48u){mask=0xffu;bits=s.registryReturned?*s.returnedType:0xffu;}
            else if(i==49u){mask=15u;bits=3u;}
        }
        //A registry refusal follows every fixed scene store but none of the
        //type/suffix stores. Only the successful descendant returns the Pool.
        if(s.attempted){
            if(i==0x38u)mask=0xffu;
            else if(i>=0xc0u&&i<0x100u){
                mask=0xffu;
                const auto word=(i-0xc0u)/4u;
                bits=std::uint8_t((word%5u==0u?0x3f800000u:0u)>>((i%4u)*8u));
            }else if(i>=0x100u&&i<0x140u)mask=0xffu;
            else if(i>=0x140u&&i<0x180u){
                mask=0xffu;bits=std::uint8_t((i%16u>=12u?0x3f800000u:0u)>>((i%4u)*8u));
            }else if(i>=0x180u&&i<0x190u)mask=0xffu;
            else if(i==0x190u){mask=0xffu;bits=0x80u;}
            else if(i==0x191u&&s.poolReturned){mask=3u;bits=0u;}
            else if(i>=0x19cu&&i<0x1a4u)mask=0xffu;
            else if(i>=0x1a4u&&i<0x1a8u&&s.poolReturned)mask=0xffu;
            else if(i>=0x1a8u&&i<0x1acu){mask=0xffu;
                bits=std::uint8_t((s.poolReturned?0x3d47f0u:0x3d3190u)>>((i-0x1a8u)*8u));}
            else if(s.poolReturned&&((i>=0x1b0u&&i<0x1b8u)||(i>=0x1bcu&&i<0x1c0u)))mask=0xffu;
        }
        //Only the admitted nonoverlap name aperture is extended. A refused
        //name copy leaves the constructor's single zero at38 and all other
        //payload bytes exactly as they were. Never clear its untouched tail.
        if(name.nameCopy.returned&&i>=0x38u&&i-0x38u<name.nameCopy.stored){
            mask=0xffu;bits=name.scratch60[i-0x38u];
        }
        //The descendant's reached semantic Resource pointer replaces the
        //constructor zero; native identity supplies no guest pointer bits.
        //Its historical store remains even after native-only alias revocation.
        if(sourceWaterConfig_.nativeWaterPoolData.aliasStored&&i>=0x1b4u&&i<0x1b8u){mask=0u;bits=0u;}
        if(p.knownMask[i]!=mask||p.knownBits[i]!=bits||(mask==0xffu&&bytes[i]!=bits))return false;
    }
    const auto& authority=worldApplication_->entityAuthority_;std::size_t memberships=0;
    for(std::size_t i=0;i<authority.count_;++i){
        const auto& member=authority.entries_[i];const auto* node=member.node;
        if(!node)return false;
        if(node==&e){
            ++memberships;
            if(member.generation!=e.sessionGeneration||member.key!=e.objectKey)return false;
        }else if(member.key==key(&e)||node->previous10==&e||node->next14==&e||
            node->firstChild18==&e||node->parent1C==&e)return false;
    }
    if(!c.admitted)return !memberships&&!e.reference00&&!e.sessionGeneration&&!e.objectKey&&
        e.kind==g::OriginalWreckComponentKind50LA::unbound&&!e.category28&&!e.flags29;
    if(memberships!=1u||e.reference00!=1u||e.kind!=g::OriginalWreckComponentKind50LA::entity141300||
       e.sessionGeneration!=worldEntity_.sessionGeneration||c.session!=e.sessionGeneration||
       e.objectKey!=key(&e)||c.key!=e.objectKey||e.category28!=(s.registryReturned?*s.returnedType:0xffu)||
       (e.flags29&15u)!=3u||
       !worldApplication_->objectCount423F60()||!authority.canRetire(e))return false;
    count=1;return true;
}

bool Owner::waterPoolInvocationCurrentNative()const noexcept {
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    return !closed_&&!terminal_&&!nativeScbRevoked_&&nativeScbThread_==std::this_thread::get_id()&&
        !runtimeCameraBorrows_&&!runtimeWorldBorrows_&&!constructorRacerWorldBorrows_&&!sourceParticleInvocationBorrowers_&&
        h.wakeSelectionEntered&&!h.nativeWithdrawn&&!sourceWaterProvider274720_.nativeWithdrawn&&
        sourceWaterConfigOwner_&&sourceWaterProvider_&&worldApplication_&&sourceLoadApplication_&&sourceLoadRace_&&
        sourceLoadApplication_->entities()==worldApplication_&&sourceLoadRace_->world74_.get()==this&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&worldEntity_.sessionGeneration&&
        !worldApplication_->attachmentLock423F58()&&sourcePostParticleModelsPhase_==SourcePostParticleModelsPhase::entered&&
        r.pc==0x30492cu&&sourcePostParticleModelsPC_==r.pc&&sourceWaterProvider274720_.pc==r.pc&&
        sourceWaterProviderNumeric_.pc==r.pc&&sourceWaterConfigOwner_->pc_==r.pc&&
        !sourceWaterConfigOwner_->wakeSelection_.returned;
}

Owner::WaterPoolBackingResult Owner::allocateWaterPoolBackingNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterPoolBackingResult result;auto& n=sourceWaterConfig_.nativeWaterPoolBacking;
    const auto finish=[&](WaterPoolBackingResult::Status status)noexcept {
        result.status=status;result.refusal=n.refusal;result.originalSourceStop=0x30492cu;
        result.allocatorStop=sourceWaterConfig_.nativeWaterPoolAllocationEntry.pc;
        result.formatterStop=sourceWaterConfig_.nativeWaterPoolPathFormat.pc;result.adaptedPC=n.pc;
        result.requestedBytes=n.requestedBytes;result.effectiveBytes=n.effectiveBytes;
        result.nativeAllocated=n.nativeAllocated;result.nativeRetired=n.nativeRetired;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_)return result;
    std::size_t counted=0;
    if(!sourceWaterPoolCountedNative(counted))return result;
    if(n.attempted)return finish(WaterPoolBackingResult::Status::alreadyAttempted); //History only, even after withdrawal.
    if(!waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||
       !preflightSourceWorldParticleCountNative())return result;
    const auto& scalar=sourceWaterConfig_.nativeWaterPoolAllocationEntry;
    const auto& caller=sourceWaterConfig_.nativeWaterArrayCaller;
    const auto& array=sourceWaterProvider_->nativeArrayBacking;const auto& view=sourceWaterProvider_->nativeArray68;
    if(!scalar.attempted||scalar.pc!=0x3059ecu||!scalar.callIssued||!scalar.lockReturned||
       scalar.requestedBytes!=0x1d0u||scalar.effectiveBytes!=0x1d0u||scalar.callerReturnPC!=0x2b9d14u||
       scalar.reentStop!=SourceWaterConfig::NativeWaterPoolAllocationEntry::ReentStop::none||
       !array.payload||!array.slots||array.role!=NativeWaterArrayRole::poolPointers||array.epoch!=1u||
       caller.role!=NativeWaterArrayRole::poolPointers||caller.ordinal!=0u||!caller.loopTaken||!*caller.loopTaken||
       *caller.ordinal>=array.originalCount||!view||view->role!=NativeWaterArrayRole::poolPointers||
       view->epoch!=array.epoch||view->offset||sourceWaterProvider_->nativePool)return result;
    n.attempted=true;n.aggregateIdentity=this;n.providerIdentity=sourceWaterProvider_.get();
    n.arrayEpoch=array.epoch;n.ordinal=caller.ordinal;n.requestedBytes=scalar.requestedBytes;n.effectiveBytes=scalar.effectiveBytes;
    const auto refuse=[&](WaterPoolBackingRefusal reason)noexcept {
        n.refusal=reason;return finish(WaterPoolBackingResult::Status::nativeAdmissionStopped);
    };
    using G=a::OriginalConfigProcessGlobals50LA;
    //Fresh native-only reentrancy admission, separate from source30570C and
    //the formatter. None of those historical captures/PCs is rewritten.
    if(!G::reentrancyCurrentNative())return refuse(WaterPoolBackingRefusal::reentrancyUnknown);
    n.nativeAdmissionReent=G::state().reent38544C;
    if(*n.nativeAdmissionReent!=G::StdioIdentity::reent385160)return refuse(WaterPoolBackingRefusal::reentrancyUnsupported);
    n.ownerAttempted=true;std::unique_ptr<SourceWaterPoolOwner> pool(new(std::nothrow) SourceWaterPoolOwner);
    if(!pool)return refuse(WaterPoolBackingRefusal::ownerAllocation);
    n.ownerPrepared=true;n.payloadAttempted=true;
    pool->payload.reset(std::malloc(0x1d0u)); //Uninitialized exact scalar request, NOT normalized chunk1E0.
    if(!pool->payload){pool.reset();n.temporaryOwnerRetired=true;return refuse(WaterPoolBackingRefusal::payloadAllocation);}
    //No fallible preparation remains before transfer. No raw bytes are filled.
    pool->role.wholeOwner=pool.get();pool->role.payloadIdentity=pool->payload.get();pool->role.entityFacet=&pool->entity;
    pool->role.epoch=1u;pool->role.arrayEpoch=*n.arrayEpoch;pool->role.ordinal=*n.ordinal;
    sourceWaterProvider_->nativePool=std::move(pool);n.epoch=1u;n.nativeAllocated=true;
    auto& p=*sourceWaterProvider_->nativePool;
    n.pc=0x2b9d14;n.callerEntered=true;n.nullBranchTaken=false; //Private nonnull role, not malloc/new return.
    n.pc=0x2b9d18;n.argumentA2=1u; //BEQ delay always executes.
    n.pc=0x2b9d1c;n.argumentA1=1u;
    n.pc=0x2b9d20;n.constructorCallIssued=true;n.callerReturnPC=0x2b9d28u;
    n.pc=0x2b9d24;p.wholeArgument=SourceWaterPoolOwner::View{&p.role,0u};n.wholeArgumentPassed=true;
    n.pc=0x2bae30; //Before Pool prologue. No reference, count, enrollment or source bytes yet.
    return finish(WaterPoolBackingResult::Status::nativeConstructorBoundary);
}

Owner::WaterPoolEntityResult Owner::constructWaterPoolEntityNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterPoolEntityResult result;auto& n=sourceWaterConfig_.nativeWaterPoolEntity;
    const auto& backing=sourceWaterConfig_.nativeWaterPoolBacking;
    const auto finish=[&](WaterPoolEntityResult::Status status)noexcept {
        result.status=status;result.refusal=n.refusal;result.originalSourceStop=0x30492cu;
        result.allocatorStop=sourceWaterConfig_.nativeWaterPoolAllocationEntry.pc;
        result.callerStop=backing.pc;result.adaptedPC=n.pc;result.referenceAcquired=n.referenceAcquired;
        result.countAcquired=n.countAcquired;result.enrolled=n.enrolled;result.entityReturned=n.entityReturned;
        result.nativeRetired=backing.nativeRetired;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_)return result;
    std::size_t counted=0;if(!sourceWaterPoolCountedNative(counted))return result;
    if(n.attempted)return finish(WaterPoolEntityResult::Status::alreadyAttempted);
    if(!waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||
       !preflightSourceWorldParticleCountNative()||!backing.nativeAllocated||backing.nativeRetired||
       backing.pc!=0x2bae30u||!sourceWaterProvider_->nativePool)return result;
    auto& p=*sourceWaterProvider_->nativePool;auto& e=p.entity;
    auto& authority=worldApplication_->entityAuthority_;auto& actualCount=worldApplication_->objectCount423F60();
    n.attempted=true;n.pc=0x2bae30u;
    const auto refuse=[&](WaterPoolEntityRefusal reason)noexcept {
        n.refusal=reason;return finish(WaterPoolEntityResult::Status::nativeAdmissionStopped);
    };
    //All fallible admission BEFORE any constructor store/reference. A refusal
    //retains backing-only custody; it is not an original constructor branch.
    if(actualCount==std::numeric_limits<std::uint32_t>::max())return refuse(WaterPoolEntityRefusal::countCapacity);
    if(!authority.reserveAdditional(1))return refuse(WaterPoolEntityRefusal::authorityCapacity);
    if(!authority.canEnroll(e,key(&e)))return refuse(WaterPoolEntityRefusal::authorityAdmission);
    n.admitted=true;n.session=worldEntity_.sessionGeneration;n.key=key(&e);
    auto* bytes=static_cast<unsigned char*>(p.payload.get());
    const auto store=[&](std::size_t offset,std::size_t width,std::uint64_t value)noexcept {
        for(std::size_t i=0;i<width;++i){const auto v=std::uint8_t(value>>(i*8u));
            bytes[offset+i]=v;p.knownMask[offset+i]=0xffu;p.knownBits[offset+i]=v;}
    };
    const auto bits=[&](std::size_t offset,std::uint8_t mask,std::uint8_t value)noexcept {
        //No read/write of the indeterminate raw byte: only the actual known
        //source bits are represented. Upper preserved bits stay unknown.
        p.knownBits[offset]=std::uint8_t((p.knownBits[offset]&~mask)|(value&mask));
        p.knownMask[offset]=std::uint8_t(p.knownMask[offset]|mask);
    };
    n.pc=0x2bae30;n.poolFrame=true; //32 bytes; source saves remain opaque.
    n.pc=0x2bae34;n.poolSaveMask|=1u;n.pc=0x2bae38;n.poolSaveMask|=2u;
    n.pc=0x2bae3c;n.sceneCallIssued=true;n.pc=0x2bae40; //Same whole view retained in s0.
    n.pc=0x2893a0;n.sceneFrame=true;n.pc=0x2893a4;n.sceneSaveMask=1u;
    n.pc=0x2893a8;n.wholeSaved=true;
    n.pc=0x2893bc;store(0u,2u,0u);n.header00=0u;
    n.pc=0x2893d4;store(2u,1u,0xffu);n.header02=0xffu;
    n.pc=0x2893e8;bits(3u,1u,0u);n.header03Mask=1u;
    n.pc=0x2893fc;bits(3u,2u,0u);n.header03Mask=3u;
    n.pc=0x289410;bits(3u,4u,0u);n.header03Mask=7u;
    n.pc=0x289418;n.entityCallIssued=true;
    n.pc=0x28941c;p.entityArgument=SourceWaterPoolOwner::View{&p.role,8u};
    n.pc=0x141300;n.entityFrame=true; //48 bytes, original a1 remains1.
    n.pc=0x141304;n.entitySaveMask|=1u;n.pc=0x141308;n.entitySaveMask|=2u;
    n.pc=0x14130c;n.entitySaveMask|=4u;n.pc=0x141310;
    n.pc=0x141314;n.referenceCallIssued=true;n.pc=0x141318;n.pc=0x1048f0;
    n.pc=0x1048f8;store(0x10u,4u,0x3cc980u);n.referenceDispatchStored=true;
    n.pc=0x104900;store(8u,8u,1u);e.reference00=1u;n.referenceAcquired=true;
    //Session/key/kind are native authority bookkeeping, never guest writes.
    e.sessionGeneration=*n.session;e.objectKey=*n.key;e.kind=g::OriginalWreckComponentKind50LA::entity141300;
    n.pc=0x104904;n.pc=0x104908;n.referenceReturned=true;
    n.pc=0x141324;store(0x10u,4u,0x3cd240u);n.entityDispatchStored=true;
    n.pc=0x14132c;store(0x30u,1u,0xffu);e.category28=0xffu;n.category30=0xffu;
    n.pc=0x141330;store(0x18u,4u,0u);e.previous10=nullptr;++n.linksWritten;
    n.pc=0x141334;store(0x1cu,4u,0u);e.next14=nullptr;++n.linksWritten;
    n.pc=0x141338;store(0x20u,4u,0u);e.firstChild18=nullptr;++n.linksWritten;
    n.pc=0x14133c;store(0x24u,4u,0u);e.parent1C=nullptr;++n.linksWritten;
    n.pc=0x14135c;bits(0x31u,1u,1u);e.flags29=std::uint8_t((e.flags29&~1u)|1u);n.flags31Mask=1u;n.flags31Bits=1u;
    n.pc=0x14137c;bits(0x31u,2u,2u);e.flags29=std::uint8_t((e.flags29&~2u)|2u);n.flags31Mask=3u;n.flags31Bits=3u;
    n.pc=0x141398;bits(0x31u,4u,0u);e.flags29=std::uint8_t(e.flags29&~4u);n.flags31Mask=7u;
    n.pc=0x1413a8;bits(0x31u,8u,0u);e.flags29=std::uint8_t(e.flags29&~8u);n.flags31Mask=15u;
    n.pc=0x1413ac;store(0x28u,4u,0u);e.mask20=0u;n.mask28Stored=true;
    n.pc=0x1413b0;store(0x2cu,4u,0u);e.descendants24=0u;n.descendants2CStored=true;
    n.pc=0x1413b4;n.countBefore=actualCount;n.pc=0x1413b8;
    const auto incremented=std::uint32_t(*n.countBefore+1u);n.pc=0x1413bc;
    n.pc=0x1413c0;actualCount=incremented;n.countAcquired=true;
    //No fallible step since104900. Exactly one same-facet enrollment; this
    //native bookkeeping is not another reference or an original call.
    if(!authority.enroll(e))std::terminate();n.enrolled=true;
    n.pc=0x1413c4;n.entityRestoreMask|=1u;n.pc=0x1413c8;n.entityRestoreMask|=2u;
    n.pc=0x1413cc;n.entityRestoreMask|=4u;n.pc=0x1413d0;
    n.pc=0x1413d4;n.entityStackReleased=true;n.entityReturned=true;
    n.pc=0x289420; //BEFORE LW. Scene and Pool frames are still active.
    return finish(WaterPoolEntityResult::Status::nativeEntityBoundary);
}

bool Owner::withdrawSourceWaterPoolNative()noexcept {
    std::size_t count=0;if(!sourceWaterPoolCountedNative(count))return false;
    auto& b=sourceWaterConfig_.nativeWaterPoolBacking;auto& c=sourceWaterConfig_.nativeWaterPoolEntity;
    if(!b.nativeAllocated||b.nativeRetired)return true;
    //Called only after provider local/full-floor preflight, successful Config
    //abort, and World400/light-pin withdrawal. Revalidate before mutation.
    if(nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||!sourceWaterConfig_.nativeWithdrawn||
       !sourceWaterProviderInitializer_.world400NativeWithdrawn||!preflightSourceWorldParticleCountNative())return false;
    auto& p=*sourceWaterProvider_->nativePool;
    auto& resource=sourceWaterConfig_.nativeWaterPoolResource;
    //The provider preflights this before Config/World withdrawal. Recheck now,
    //then discharge the exact file before revoking any Pool view or count.
    //A refused close retains this still-counted Pool and independent bank pin;
    //retry only native cleanup, never the one-shot forward source calls.
    if(!preflightNativeWaterPoolFileWithdrawal())return false;
    if(resource.file&&!resource.nativeWithdrawn){
        resource.nativeWithdrawalAttempted=true;
        if(!resource.file->closeNative(resource.nativeWithdrawalDiagnostic))return false;
        resource.nativeWithdrawn=true;
    }
    //The unpublished partial Resource has its own reference/storage. Retire
    //it after exact file cancellation but before Pool Entity/views/storage.
    //If later authority retirement refuses, do not reacquire or reread it.
    if(!withdrawNativeWaterPoolData())return false;
    //Revoke the private arguments before authority retirement. A refused
    //retire does not change authority; restore only these native views so
    //the caller retains the exact same complete custody for a later retry.
    const auto whole=p.wholeArgument,entity=p.entityArgument;
    p.entityArgument.reset();p.wholeArgument.reset();b.viewsRetired=true;
    if(count&&!worldApplication_->entityAuthority_.retire(p.entity)){
        p.wholeArgument=whole;p.entityArgument=entity;b.viewsRetired=false;return false;
    }
    if(count){c.authorityRetired=true;--p.entity.reference00;c.referenceRetired=true;
        --worldApplication_->objectCount423F60();c.countRetired=true;}
    sourceWaterProvider_->nativePool.reset();b.nativeRetired=true;
    //No source destructor/free/NULL receipt, no rollback of authority serial.
    return true;
}

#include "original_source_world_water_pool_scene50la.inl"
#include "original_source_world_water_pool_name50la.inl"
#include "original_source_world_water_pool_resource50la.inl"
#include "original_source_world_water_pool_data50la.inl"
