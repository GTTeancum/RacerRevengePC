//Positive-count descendant only. Original raw-malloc and scalar-new stops
//remain immutable. This file never consumes the material's64-byte name or calls
//a source allocator, destructor, release, topology, texture or registry helper.
bool Owner::sourceWaterPoolMaterialCountedNative(std::size_t& count)const noexcept {
    using N=SourceWaterConfig::NativeWaterPoolMaterial;using S=WaterPoolMaterialStop;
    using G=a::OriginalConfigProcessGlobals50LA;using R=N::RawAllocation::ReentStop;
    using C=SourceWaterConfig::NativeWaterPoolData::Allocation::ReentStop;
    count=0;const auto& n=sourceWaterConfig_.nativeWaterPoolMaterial;const auto& x=n.raw;const auto& c=n.child;
    const auto& d=sourceWaterConfig_.nativeWaterPoolData;const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    const auto& name=sourceWaterConfig_.nativeWaterPoolName;
    //Reject a forged descendant even on one of the prefix's early refusal
    //returns. The full overlay check below uses the exact39-store preimage.
    if(!sourceWaterConfig_.nativeWaterPoolMaterialReader.attempted){
        std::array<std::uint8_t,0xd8> masks{},bits{};
        if(!preflightNativeWaterPoolMaterialReaderHistory(masks,bits))return false;
    }else if(!n.attempted||n.stop!=S::readCall2BF730||n.pc!=0x2bf730u||!n.constructorAdmitted)return false;
    const auto virginLoad=[](const SourceWaterConfig::NativeWaterPoolResource::ServiceLoad& l)noexcept {
        return !l.attempted&&!l.resolved&&!l.pc&&!l.applicationIdentity&&!l.domainIdentity&&!l.resolverIdentity&&
            !l.providerIdentity&&!l.trackIdentity&&!l.trackMountIdentity&&
            l.trackMount==static_cast<std::size_t>(-1)&&!l.trackGeneration;
    };
    const auto virginCopy=[](const N::EmptyCopy& v)noexcept {
        return !v.callIssued&&!v.allAlignmentsProved&&!v.returned&&!v.callPC&&!v.delayPC&&!v.helperEntry&&
            !v.literal&&!v.destinationOffset&&!v.returnPC&&!v.storedBytes;
    };
    const auto noScalarArena=[&]()noexcept {
        return !c.allocatorFrame&&!c.allocatorSaveMask&&!c.reentCopied&&!c.lockCalled&&!c.lockReturned&&
            !c.sumWord&&!c.normalizedChunk&&!c.arenaBase&&!c.arenaReadOffset&&!c.arenaReadAddress&&
            !c.sumRegister&&!c.normalizedRegister&&!c.minimumChunk&&!c.smallBin;
    };
    const auto virginScalar=[&]()noexcept {
        return !c.attempted&&!c.callIssued&&!c.pc&&!c.requestedBytes&&!c.effectiveBytes&&!c.callerReturnPC&&
            !c.argumentA0&&!c.effectiveArgument&&!c.operatorFrame&&!c.operatorFramePointer&&!c.nonzeroBranch&&
            !c.mallocFrame&&!c.operatorSaveMask&&!c.mallocSaveMask&&c.reentStop==C::none&&!c.reent&&noScalarArena();
    };
    const auto noBody=[&]()noexcept {
        if(n.storesWritten)return false;
        for(const auto& v:n.stores)if(v.pc||v.offset||v.width||v.word||v.mask)return false;
        return !n.constructorAdmitted&&!n.materialFrame&&!n.materialWholeSaved&&!n.entityCallIssued&&!n.entityDelay&&
            !n.entityFrame&&!n.referenceCallIssued&&!n.referenceReturned&&!n.referenceAcquired&&!n.countAcquired&&
            !n.enrolled&&!n.entityReturned&&!n.constructorReturned&&!n.constructorStackReleased&&!n.materialSaveMask&&
            !n.materialRestoreMask&&!n.entitySaveMask&&!n.entityRestoreMask&&!n.entityOffset&&!n.countBefore&&
            !n.session&&!n.key&&!n.slotStored&&!n.slotRevoked&&!n.authorityRetired&&!n.referenceRetired&&!n.countRetired&&
            virginCopy(n.constructorCopy)&&virginCopy(n.setupCopy)&&!n.setupSlotReloaded&&!n.setupCallIssued&&
            !n.setupDelay&&!n.setupFrame&&!n.setupIdentityCalled&&!n.setupIdentityReturned&&!n.setupReturned&&
            !n.setupStackReleased&&!n.setupSaveMask&&!n.setupRestoreMask&&!n.setupArgument&&!n.setupReturnPC&&
            !n.readerSlotReloaded&&!n.readerCallIssued&&!n.readerDelay&&!n.readerFrame&&!n.readerWholeSelected&&
            !n.readerFlagPreserved&&!n.readerSaveMask&&!n.readerHandle&&!n.readerFlag&&!n.readerReturnPC&&
            !n.destinationOffset&&!n.itemSize&&!n.itemCount&&virginLoad(n.load2BF71C);
    };
    const auto noChildBacking=[&]()noexcept {
        return !n.childAllocated&&!n.childRetired&&!n.childRoleIdentity&&!n.entityIdentity&&!n.childEpoch&&
            !n.callerNonnull&&!n.constructorCallIssued&&!n.constructorDelay&&!n.constructorA1&&!n.constructorReturnPC&&
            !n.viewsRetired&&noBody();
    };
    const auto noChildNative=[&]()noexcept {
        return !n.childAdmissionReent&&!n.childOwnerAttempted&&!n.childOwnerPrepared&&!n.childPayloadAttempted&&
            !n.childTemporaryRetired&&noChildBacking();
    };
    const auto noArrayBacking=[&]()noexcept {
        return !n.rawAllocated&&!n.rawRetired&&!n.arrayAliasStored&&!n.arrayAliasRevoked&&!n.arrayRoleIdentity&&
            !n.arrayEpoch&&!n.loopBranchIssued&&!n.ordinalZeroed&&!n.countReloaded&&!n.loopCompared&&!n.loopTaken&&
            !n.loopDelay&&!n.ordinal&&virginScalar()&&noChildNative();
    };
    const auto noRawNative=[&]()noexcept {
        return !n.rawAdmissionReent&&!n.rawOwnerAttempted&&!n.rawOwnerPrepared&&!n.slotsAttempted&&!n.slotsPrepared&&
            !n.rawPayloadAttempted&&!n.rawTemporaryRetired&&noArrayBacking();
    };
    const auto noRawArena=[&]()noexcept {
        return !x.allocatorFrame&&!x.allocatorSaveMask&&!x.reentCopied&&!x.lockCalled&&!x.lockReturned&&
            !x.sumWord&&!x.normalizedChunk&&!x.arenaBase&&!x.arenaReadOffset&&!x.arenaReadAddress&&!x.sumRegister&&
            !x.normalizedRegister&&!x.minimumChunk&&!x.smallBin&&!x.binScale&&!x.binIndex&&!x.binSelectionPC&&
            !x.largeBranchIssued&&!x.pendingDelayPC&&!x.pendingTargetPC;
    };
    const auto absentArray=[&]()noexcept {
        return !sourceWaterProvider_||!sourceWaterProvider_->nativePool||!sourceWaterProvider_->nativePool->nativeData||
            (!sourceWaterProvider_->nativePool->nativeData->nativeMaterials&&!sourceWaterProvider_->nativePool->nativeData->materials10);
    };
    if(!n.attempted)return n.stop==S::none&&!n.pc&&!n.dataRoleIdentity&&!n.fileIdentity&&!n.applicationIdentity&&
        !n.originalCount&&!n.dataEpoch&&!x.attempted&&!x.callIssued&&!x.shiftDelay&&!x.mallocFrame&&!x.pc&&
        !x.mallocSaveMask&&!x.requestedBytes&&!x.callerReturnPC&&!x.argumentA0&&!x.argumentCopied&&
        x.reentStop==R::none&&!x.reent&&noRawArena()&&noRawNative()&&absentArray();
    //This predecessor stays BEFORE2BB1B0 even after our adapted continuation.
    //No call back to the provider/full World count floor from this local helper.
    if(!preflightNativeWaterPoolDataHistory()||!d.attempted||d.stop!=WaterPoolDataStop::childAllocation2BB1B0||
       d.pc!=0x2bb1b0u||!d.readResultAdmitted||!d.countWord||!d.signedCount||*d.signedCount<=0||
       n.originalCount!=d.countWord||n.dataRoleIdentity!=d.dataRoleIdentity||n.dataEpoch!=d.dataEpoch||
       n.fileIdentity!=d.fileIdentity||!n.fileIdentity||!n.dataRoleIdentity||
       n.applicationIdentity!=f.load2BB044.applicationIdentity||!n.applicationIdentity||
       !x.attempted||!x.callIssued||!x.shiftDelay||!x.argumentCopied||!x.mallocFrame||x.callerReturnPC!=0x2bb1b8u)return false;
    const auto signWord=[](std::uint32_t v)noexcept {
        return v&0x80000000u?(0xffffffff00000000ull|std::uint64_t(v)):std::uint64_t(v);
    };
    const auto request=std::uint32_t(*n.originalCount<<2u);
    if(x.requestedBytes!=request||x.argumentA0!=signWord(request))return false;
    if(x.reentStop!=R::none){
        if(n.stop!=S::rawSourceReentrancy||n.pc!=0x2bb1b0u||x.pc!=0x30570cu||x.mallocSaveMask||
           !noRawArena()||!noRawNative()||!absentArray())return false;
        return x.reentStop==R::unknown?!x.reent:
            x.reentStop==R::unsupportedIdentity&&x.reent&&*x.reent!=G::StdioIdentity::reent385160;
    }
    const auto sum=std::uint32_t(request+19u);const bool minimum=signWord(sum)<31ull;
    const auto normalized=minimum?16u:(sum&0xfffffff0u);const bool small=signWord(normalized)<504ull;
    if(x.reent!=G::StdioIdentity::reent385160||x.mallocSaveMask!=1u||!x.allocatorFrame||x.allocatorSaveMask!=63u||
       !x.reentCopied||!x.lockCalled||!x.lockReturned||x.sumWord!=sum||x.sumRegister!=signWord(sum)||
       x.minimumChunk!=minimum||x.normalizedChunk!=normalized||x.normalizedRegister!=signWord(normalized)||
       x.smallBin!=small||x.arenaBase!=0x385468u)return false;
    if(small){
        if(x.pc!=0x3059ecu||x.arenaReadOffset!=normalized+12u||x.arenaReadAddress!=0x385468u+normalized+12u||
           x.binScale||x.binIndex||x.binSelectionPC||x.largeBranchIssued||x.pendingDelayPC||x.pendingTargetPC)return false;
    }else{
        const auto t=normalized>>9u;
        const auto bin=t==0u?(normalized>>3u):t<5u?(normalized>>6u)+56u:t<21u?t+91u:
            t<85u?(normalized>>12u)+110u:t<341u?(normalized>>15u)+119u:t<1365u?(normalized>>18u)+124u:126u;
        const auto selected=t==0u?0x305a48u:t<5u?0x305a58u:t<21u?0x305a64u:t<85u?0x305a78u:
            t<341u?0x305a8cu:t<1365u?0x305aa0u:0x305aa4u;
        if(x.pc!=0x305ac4u||x.binScale!=t||x.binIndex!=bin||x.binSelectionPC!=selected||
           x.arenaReadOffset!=8u*bin+12u||x.arenaReadAddress!=0x385468u+8u*bin+12u||
           !x.largeBranchIssued||x.pendingDelayPC!=0x305ac4u||x.pendingTargetPC!=0x305ad4u)return false;
    }
    if(n.stop==S::rawNativeReentrancy)return n.pc==0x2bb1b0u&&
        (!n.rawAdmissionReent||*n.rawAdmissionReent!=G::StdioIdentity::reent385160)&&
        !n.rawOwnerAttempted&&!n.rawOwnerPrepared&&!n.slotsAttempted&&!n.slotsPrepared&&!n.rawPayloadAttempted&&
        !n.rawTemporaryRetired&&noArrayBacking()&&absentArray();
    if(n.rawAdmissionReent!=G::StdioIdentity::reent385160)return false;
    const auto full=std::uint64_t(*n.originalCount)*4ull;
    const bool countSafe=*n.originalCount<=nativeWaterArrayMaxCount;
    const bool bytesSafe=full<=nativeWaterArrayMaxBytes;
    const bool requestSafe=full==request&&request&&!(request&0x80000000u)&&
        !(sum&0x80000000u)&&!(normalized&0x80000000u)&&normalized>=request;
    if(n.stop==S::rawCountCapacity||n.stop==S::rawByteCapacity||n.stop==S::rawRequestMismatch){
        const bool exact=n.stop==S::rawCountCapacity?!countSafe:
            n.stop==S::rawByteCapacity?countSafe&&!bytesSafe:countSafe&&bytesSafe&&!requestSafe;
        return exact&&n.pc==0x2bb1b0u&&!n.rawOwnerAttempted&&!n.rawOwnerPrepared&&!n.slotsAttempted&&
            !n.slotsPrepared&&!n.rawPayloadAttempted&&!n.rawTemporaryRetired&&noArrayBacking()&&absentArray();
    }
    if(!countSafe||!bytesSafe||!requestSafe||!n.rawOwnerAttempted)return false;
    if(n.stop==S::rawOwnerAllocation)return n.pc==0x2bb1b0u&&!n.rawOwnerPrepared&&!n.slotsAttempted&&
        !n.slotsPrepared&&!n.rawPayloadAttempted&&!n.rawTemporaryRetired&&noArrayBacking()&&absentArray();
    if(!n.rawOwnerPrepared||!n.slotsAttempted)return false;
    if(n.stop==S::rawSlotAllocation)return n.pc==0x2bb1b0u&&!n.slotsPrepared&&!n.rawPayloadAttempted&&
        n.rawTemporaryRetired&&noArrayBacking()&&absentArray();
    if(!n.slotsPrepared||!n.rawPayloadAttempted)return false;
    if(n.stop==S::rawPayloadAllocation)return n.pc==0x2bb1b0u&&n.rawTemporaryRetired&&noArrayBacking()&&absentArray();
    if(n.rawTemporaryRetired||!n.rawAllocated||!n.arrayRoleIdentity||n.arrayEpoch!=1u||!n.arrayAliasStored||
       n.arrayAliasRevoked!=n.rawRetired||!n.loopBranchIssued||!n.ordinalZeroed||n.ordinal!=0u||
       !n.countReloaded||!n.loopCompared||!n.loopTaken||!n.loopDelay||
       (d.nativeRetired&&!n.rawRetired)||(n.rawRetired&&n.childAllocated&&!n.childRetired))return false;
    const SourceWaterPoolMaterialArrayOwner* array=nullptr;
    if(n.rawRetired){if(!f.nativeWithdrawn||!f.file||!f.file->nativeClosed()||!absentArray())return false;}
    else {
        if(d.nativeRetired||!sourceWaterProvider_||!sourceWaterProvider_->nativePool||!sourceWaterProvider_->nativePool->nativeData)return false;
        const auto& data=*sourceWaterProvider_->nativePool->nativeData;
        if(!data.nativeMaterials||!data.materials10)return false;array=data.nativeMaterials.get();
        if(&array->role!=n.arrayRoleIdentity||array->role.nativeOwner!=array||array->role.dataOwner!=&data||
           array->role.payloadIdentity!=array->payload.get()||array->role.epoch!=1u||!array->payload||!array->slots||
           array->payload.get()==data.payload.get()||array->payload.get()==sourceWaterProvider_->nativePool->payload.get()||
           array->originalCount!=*n.originalCount||array->slotCapacity!=*n.originalCount||array->requestedBytes!=request||
           data.materials10->role!=&array->role||data.materials10->offset)return false;
        for(std::uint32_t i=0;i<array->slotCapacity;++i){
            const auto& slot=array->slots[i];const bool published=i==0u&&n.slotStored&&!n.slotRevoked;
            if(slot.known!=published||slot.offset||slot.role!=(published?n.childRoleIdentity:nullptr))return false;
        }
    }
    if(!c.attempted||!c.callIssued||c.requestedBytes!=0xd8u||c.argumentA0!=0xd8ull||
       c.effectiveBytes!=0xd8u||c.effectiveArgument!=0xd8ull||c.callerReturnPC!=0x2bb1ccu||
       !c.operatorFrame||c.operatorSaveMask!=7u||!c.operatorFramePointer||!c.nonzeroBranch||!c.mallocFrame)return false;
    if(c.reentStop!=C::none){
        if(n.stop!=S::childSourceReentrancy||n.pc!=0x2bb1c4u||c.pc!=0x30570cu||c.mallocSaveMask||
           !noScalarArena()||!noChildNative()||(array&&array->firstChild))return false;
        return c.reentStop==C::unknown?!c.reent:
            c.reentStop==C::unsupportedIdentity&&c.reent&&*c.reent!=G::StdioIdentity::reent385160;
    }
    if(c.pc!=0x3059ecu||c.reent!=G::StdioIdentity::reent385160||c.mallocSaveMask!=1u||
       !c.allocatorFrame||c.allocatorSaveMask!=63u||!c.reentCopied||c.sumWord!=0xebu||c.sumRegister!=0xebull||
       c.minimumChunk!=false||c.normalizedChunk!=0xe0u||c.normalizedRegister!=0xe0ull||!c.lockCalled||
       !c.lockReturned||c.smallBin!=true||c.arenaBase!=0x385468u||c.arenaReadOffset!=0xecu||c.arenaReadAddress!=0x385554u)return false;
    if(n.stop==S::childNativeReentrancy)return n.pc==0x2bb1c4u&&
        (!n.childAdmissionReent||*n.childAdmissionReent!=G::StdioIdentity::reent385160)&&!n.childOwnerAttempted&&
        !n.childOwnerPrepared&&!n.childPayloadAttempted&&!n.childTemporaryRetired&&noChildBacking()&&(!array||!array->firstChild);
    if(n.childAdmissionReent!=G::StdioIdentity::reent385160||!n.childOwnerAttempted)return false;
    if(n.stop==S::childOwnerAllocation)return n.pc==0x2bb1c4u&&!n.childOwnerPrepared&&!n.childPayloadAttempted&&
        !n.childTemporaryRetired&&noChildBacking()&&(!array||!array->firstChild);
    if(!n.childOwnerPrepared||!n.childPayloadAttempted)return false;
    if(n.stop==S::childPayloadAllocation)return n.pc==0x2bb1c4u&&n.childTemporaryRetired&&noChildBacking()&&(!array||!array->firstChild);
    if(n.childTemporaryRetired||!n.childAllocated||!n.childRoleIdentity||!n.entityIdentity||n.childEpoch!=1u||
       !n.callerNonnull||!n.constructorCallIssued||!n.constructorDelay||n.constructorA1!=1u||
       n.constructorReturnPC!=0x2bb1e0u||n.viewsRetired!=n.childRetired)return false;
    const SourceWaterPoolMaterialOwner* material=nullptr;
    if(n.childRetired){
        if(!f.nativeWithdrawn||!f.file||!f.file->nativeClosed()||(array&&array->firstChild))return false;
    }else {
        if(!array||!array->firstChild)return false;material=array->firstChild.get();
        if(&material->role!=n.childRoleIdentity||&material->entity!=n.entityIdentity||!material->payload||
           material->payload.get()==array->payload.get()||material->role.nativeOwner!=material||
           material->role.arrayOwner!=array||material->role.payloadIdentity!=material->payload.get()||
           material->role.entityFacet!=&material->entity||material->role.epoch!=1u||material->role.arrayEpoch!=1u||
           material->role.ordinal||!material->wholeArgument||material->wholeArgument->role!=&material->role||
           material->wholeArgument->offset||bool(material->entityArgument)!=n.constructorAdmitted||
           (material->entityArgument&&(material->entityArgument->role!=&material->role||material->entityArgument->offset)))return false;
    }
    if(!n.constructorAdmitted){
        if((n.stop!=S::countCapacity&&n.stop!=S::authorityCapacity&&n.stop!=S::authorityAdmission)||
           n.pc!=0x2bf510u||!noBody())return false;
    }else {
        if(!n.materialFrame||n.materialSaveMask!=1u||n.materialRestoreMask!=1u||!n.materialWholeSaved||
           !n.entityCallIssued||!n.entityDelay||!n.entityFrame||n.entitySaveMask!=7u||n.entityRestoreMask!=7u||
           !n.referenceCallIssued||!n.referenceReturned||!n.referenceAcquired||!n.countAcquired||!n.enrolled||
           !n.entityReturned||!n.constructorReturned||!n.constructorStackReleased||n.entityOffset!=0u||
           !n.countBefore||*n.countBefore==std::numeric_limits<std::uint32_t>::max()||!n.session||!*n.session||!n.key||!*n.key||
           n.authorityRetired!=n.childRetired||n.referenceRetired!=n.childRetired||n.countRetired!=n.childRetired||
           !n.slotStored||n.slotRevoked!=n.childRetired||!n.setupSlotReloaded||!n.setupCallIssued||!n.setupDelay||
           !n.setupFrame||n.setupSaveMask!=3u||n.setupRestoreMask!=3u||!n.setupIdentityCalled||!n.setupIdentityReturned||
           !n.setupReturned||!n.setupStackReleased||n.setupArgument!=555u||n.setupReturnPC!=0x2bb204u||
           !n.readerSlotReloaded||!n.readerCallIssued||!n.readerDelay||!n.readerFrame||n.readerSaveMask!=15u||
           !n.readerWholeSelected||n.readerHandle!=d.savedHandleS3||n.readerHandle!=f.sourceHandle1E0||
           n.readerFlag!=name.argumentA3||!n.readerFlag||n.readerReturnPC!=0x2bb220u)return false;
        const auto copyAt=[](const N::EmptyCopy& v,std::uint32_t call,std::uint32_t delay,std::uint32_t ret)noexcept {
            return v.callIssued&&v.allAlignmentsProved&&v.returned&&v.callPC==call&&v.delayPC==delay&&
                v.helperEntry==0x309230u&&v.literal==0x3c11a0u&&v.destinationOffset==0x30u&&v.returnPC==ret&&v.storedBytes==1u;
        };
        if(!copyAt(n.constructorCopy,0x2bf540u,0x2bf544u,0x2bf548u)||
           !copyAt(n.setupCopy,0x2bf62cu,0x2bf630u,0x2bf634u))return false;
        const auto& l=n.load2BF71C;const auto& first=f.load2BB044;
        if(!l.attempted||l.pc!=0x2bf71cu)return false;
        if(!l.resolved){
            if(n.stop!=S::service2BF71C||n.pc!=0x2bf71cu||l.applicationIdentity||l.domainIdentity||l.resolverIdentity||
               l.providerIdentity||l.trackIdentity||l.trackMountIdentity||l.trackMount!=static_cast<std::size_t>(-1)||
               l.trackGeneration||n.readerFlagPreserved||n.destinationOffset||n.itemSize||n.itemCount)return false;
        }else if(n.stop!=S::readCall2BF730||n.pc!=0x2bf730u||!n.readerFlagPreserved||n.destinationOffset!=0x30u||
            n.itemSize!=64u||n.itemCount!=1u||l.applicationIdentity!=first.applicationIdentity||
            l.domainIdentity!=first.domainIdentity||l.resolverIdentity!=first.resolverIdentity||
            l.providerIdentity!=first.providerIdentity||l.trackIdentity!=first.trackIdentity||
            l.trackMountIdentity!=first.trackMountIdentity||l.trackMount!=first.trackMount||l.trackGeneration!=first.trackGeneration)return false;
    }
    //Actual writes in order, including constructor strcpy, later explicit SB,
    //and setup strcpy. AC is only u16. Unknown bytes never enter a raw read.
    static constexpr std::array<N::Store,39> expected{{
        {0x1048f8,8,4,0x3cc980,255},{0x104900,0,8,1,255},{0x141324,8,4,0x3cd240,255},
        {0x14132c,0x28,1,255,255},{0x141330,0x10,4,0,255},{0x141334,0x14,4,0,255},
        {0x141338,0x18,4,0,255},{0x14133c,0x1c,4,0,255},
        {0x14135c,0x29,1,1,1},{0x14137c,0x29,1,2,2},{0x141398,0x29,1,0,4},{0x1413a8,0x29,1,0,8},
        {0x1413ac,0x20,4,0,255},{0x1413b0,0x24,4,0,255},{0x2bf538,8,4,0x3d48c0,255},
        {0x309328,0x30,1,0,255},{0x2bf55c,0xd0,4,0,255},{0x2bf564,0xd4,4,0,255},{0x2bf56c,0x30,1,0,255},
        {0x2bf574,0x80,4,0,255},{0x2bf57c,0x8c,4,0,255},{0x2bf584,0x88,4,1,255},
        {0x2bf58c,0x90,4,0,255},{0x2bf594,0x94,4,0,255},{0x2bf59c,0xd0,4,0,255},
        {0x2bf5a4,0xac,2,0,255},{0x2bf5ac,0xa0,4,0,255},{0x2bf5b4,0xa4,4,0x3f800000,255},
        {0x2bf5bc,0xb0,4,0,255},{0x2bf5c4,0xa8,4,0,255},{0x2bf5cc,0x84,4,0,255},
        {0x2bf5d4,0x98,4,0,255},{0x2bf5dc,0x9c,4,1,255},{0x2bf5e4,0xb4,4,0,255},
        {0x2bf5ec,0x74,4,0x00ff00ff,255},{0x2bf5f4,0xb8,4,0,255},{0x2bf5fc,0xc4,4,0,255},
        {0x309328,0x30,1,0,255},{0x2bf634,0xd0,4,0,255}
    }};
    if(n.storesWritten!=(n.constructorAdmitted?expected.size():0u))return false;
    std::array<std::uint8_t,0xd8> masks{},bits{};
    for(std::size_t i=0;i<n.storesWritten;++i){
        const auto& v=n.stores[i];const auto& want=expected[i];
        if(v.pc!=want.pc||v.offset!=want.offset||v.width!=want.width||v.word!=want.word||v.mask!=want.mask)return false;
        for(std::size_t j=0;j<v.width;++j){const auto at=v.offset+j;const auto b=std::uint8_t(v.word>>(8u*j));
            bits[at]=std::uint8_t((bits[at]&~v.mask)|(b&v.mask));masks[at]=std::uint8_t(masks[at]|v.mask);}
    }
    //Retain constructor history verbatim, then overlay only authenticated
    //read prefixes (including a terminal failure) and the optional B4 store.
    //This value-only descendant validation also runs AFTER native retirement.
    if(!preflightNativeWaterPoolMaterialReaderHistory(masks,bits))return false;
    if(!material)return true; //Authenticated retired identity; never dereference its role or Entity.
    const auto& e=material->entity;
    if(!worldApplication_||nativeScbThread_!=std::this_thread::get_id()||worldApplication_->attachmentLock423F58()||
       n.applicationIdentity!=sourceLoadApplication_||sourceLoadApplication_->entities()!=worldApplication_||
       e.previous10||e.next14||e.firstChild18||e.parent1C||e.mask20||e.descendants24||e.nativeFlags29||e.nativeEffectControlId||
       material->authorityLive!=n.constructorAdmitted||material->countLive!=n.constructorAdmitted)return false;
    const auto* bytes=static_cast<const std::uint8_t*>(material->payload.get());
    for(std::size_t i=0;i<masks.size();++i)
        if(material->knownMask[i]!=masks[i]||material->knownBits[i]!=bits[i]||(masks[i]==255u&&bytes[i]!=bits[i]))return false;
    const auto& authority=worldApplication_->entityAuthority_;std::size_t memberships=0;
    for(std::size_t i=0;i<authority.count_;++i){const auto& entry=authority.entries_[i];const auto* node=entry.node;
        if(!node)return false;
        if(node==&e){++memberships;if(entry.generation!=e.sessionGeneration||entry.key!=e.objectKey)return false;}
        else if(entry.key==key(&e)||node->previous10==&e||node->next14==&e||node->firstChild18==&e||node->parent1C==&e)return false;
    }
    if(!n.constructorAdmitted)return !memberships&&!e.reference00&&!e.sessionGeneration&&!e.objectKey&&!e.category28&&
        !e.flags29&&e.kind==g::OriginalWreckComponentKind50LA::unbound;
    if(memberships!=1u||e.reference00!=1u||e.kind!=g::OriginalWreckComponentKind50LA::entity141300||
       e.sessionGeneration!=worldEntity_.sessionGeneration||n.session!=e.sessionGeneration||
       e.objectKey!=key(&e)||n.key!=e.objectKey||e.category28!=255u||(e.flags29&15u)!=3u||
       !worldApplication_->objectCount423F60()||!authority.canRetire(e))return false;
    count=1u;return true;
}

#include "original_source_world_water_pool_material_reader50la.inl"

bool Owner::withdrawNativeWaterPoolMaterial()noexcept {
    std::size_t count=0;if(!sourceWaterPoolMaterialCountedNative(count))return false;
    auto& n=sourceWaterConfig_.nativeWaterPoolMaterial;
    if(!n.rawAllocated||n.rawRetired)return true;
    const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    if(nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||!sourceWaterConfig_.nativeWithdrawn||
       !sourceWaterProviderInitializer_.world400NativeWithdrawn||!f.nativeWithdrawn||!f.file||!f.file->nativeClosed()||
       !sourceWaterProvider_||!sourceWaterProvider_->nativePool||!sourceWaterProvider_->nativePool->nativeData)return false;
    auto& data=*sourceWaterProvider_->nativePool->nativeData;auto& array=*data.nativeMaterials;
    //Exact file close/pin discharge has already succeeded. A refusal below
    //restores ONLY native views/slot; it never reopens or replays source calls.
    if(n.childAllocated&&!n.childRetired){
        auto& m=*array.firstChild;
        const auto whole=m.wholeArgument,entity=m.entityArgument;const auto slot=array.slots[0];
        m.wholeArgument.reset();m.entityArgument.reset();n.viewsRetired=true;
        if(n.slotStored){array.slots[0]={};n.slotRevoked=true;}
        if(count&&!worldApplication_->entityAuthority_.retire(m.entity)){
            m.wholeArgument=whole;m.entityArgument=entity;array.slots[0]=slot;
            n.viewsRetired=false;n.slotRevoked=false;return false;
        }
        if(count){
            n.authorityRetired=true;m.authorityLive=false;
            --m.entity.reference00;n.referenceRetired=true;
            --worldApplication_->objectCount423F60();n.countRetired=true;m.countLive=false;
        }
        //Backing-only constructor refusal owns no source reference or count.
        array.firstChild.reset();n.childRetired=true;
    }
    data.materials10.reset();n.arrayAliasRevoked=true;
    data.nativeMaterials.reset();n.rawRetired=true;
    //Historical2BB1B8/2BB1EC and unknown guest pointer bytes remain unchanged.
    //No2BA970/142590/104970/2BF650/1413E0/305728/free source receipt is created.
    return true;
}

Owner::WaterPoolMaterialResult Owner::constructWaterPoolMaterialNativeOnly(WaterArrayBackingMode mode)noexcept {
    using N=SourceWaterConfig::NativeWaterPoolMaterial;using S=WaterPoolMaterialStop;
    using G=a::OriginalConfigProcessGlobals50LA;using R=N::RawAllocation::ReentStop;
    using C=SourceWaterConfig::NativeWaterPoolData::Allocation::ReentStop;
    auto& n=sourceWaterConfig_.nativeWaterPoolMaterial;auto& x=n.raw;auto& c=n.child;
    const auto& d=sourceWaterConfig_.nativeWaterPoolData;const auto& f=sourceWaterConfig_.nativeWaterPoolResource;
    WaterPoolMaterialResult result;
    const auto finish=[&](WaterPoolMaterialResult::Status status)noexcept {
        result.status=status;result.stop=n.stop;result.originalSourceStop=0x30492cu;result.dataStop=d.pc;
        result.rawAllocatorStop=x.pc;result.childAllocatorStop=c.pc;result.adaptedPC=n.pc;
        result.countWord=n.originalCount;result.rawRequestedBytes=x.requestedBytes;result.rawArgumentA0=x.argumentA0;
        result.rawNormalizedChunk=x.normalizedChunk;result.rawPendingArenaAddress=x.arenaReadAddress;
        result.rawPendingDelayPC=x.pendingDelayPC;result.rawPendingTargetPC=x.pendingTargetPC;
        result.childPendingArenaAddress=c.arenaReadAddress;result.rawAllocated=n.rawAllocated;
        result.arrayAliasStored=n.arrayAliasStored;result.childAllocated=n.childAllocated;
        result.referenceAcquired=n.referenceAcquired;result.countAcquired=n.countAcquired;
        result.constructorReturned=n.constructorReturned;result.slotStored=n.slotStored;result.setupReturned=n.setupReturned;
        result.childRetired=n.childRetired;result.rawRetired=n.rawRetired;return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_)return result;
    std::size_t materialCount=0,poolCount=0;
    if(!sourceWaterPoolMaterialCountedNative(materialCount)||!sourceWaterPoolCountedNative(poolCount))return result;
    if(n.attempted)return finish(WaterPoolMaterialResult::Status::alreadyAttempted);
    if(!waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||!preflightSourceWorldParticleCountNative()||
       materialCount||poolCount!=1u||!sourceWaterProvider_||!sourceWaterProvider_->nativePool||
       !sourceWaterProvider_->nativePool->nativeData||d.stop!=WaterPoolDataStop::childAllocation2BB1B0||
       d.pc!=0x2bb1b0u||d.nativeRetired||!d.countWord||!d.signedCount||*d.signedCount<=0||!d.readResultAdmitted||
       !f.file||f.file->nativeClosed()||!f.file->preflightNative(f.wholeRoleIdentity)||
       f.file->nativeHandle_==-1||!f.file->countRead_.resultAdmitted)return result;
    auto& data=*sourceWaterProvider_->nativePool->nativeData;
    if(data.nativeMaterials||data.materials10||&data.role!=d.dataRoleIdentity)return result;
    n.attempted=true;n.pc=0x2bb1b0u;n.dataRoleIdentity=&data.role;n.dataEpoch=data.role.epoch;
    n.fileIdentity=f.file.get();n.applicationIdentity=sourceLoadApplication_;n.originalCount=d.countWord;
    const auto signWord=[](std::uint32_t v)noexcept {
        return v&0x80000000u?(0xffffffff00000000ull|std::uint64_t(v)):std::uint64_t(v);
    };
    //RAW malloc occurrence. No scalar/new[] frame, zero coercion, cookie or
    //constructor wrapper. SLL delay forms the true wrapping signed32 register.
    x.attempted=true;x.callIssued=true;x.pc=0x2bb1b0;x.callerReturnPC=0x2bb1b8u;
    x.pc=0x2bb1b4;x.requestedBytes=std::uint32_t(*d.countWord<<2u);
    x.argumentA0=signWord(*x.requestedBytes);x.shiftDelay=true;
    x.pc=0x305700;x.argumentCopied=true;x.pc=0x305704;x.pc=0x305708;x.mallocFrame=true;x.pc=0x30570c;
    n.stop=S::rawSourceReentrancy;
    if(!G::reentrancyCurrentNative()){x.reentStop=R::unknown;return finish(WaterPoolMaterialResult::Status::nativeStopped);}
    x.reent=G::state().reent38544C;
    if(*x.reent!=G::StdioIdentity::reent385160){x.reentStop=R::unsupportedIdentity;
        return finish(WaterPoolMaterialResult::Status::nativeStopped);}
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
    //Native admission is independent of the immutable allocator frontier.
    n.stop=S::rawNativeReentrancy;
    if(!G::reentrancyCurrentNative())return finish(WaterPoolMaterialResult::Status::nativeStopped);
    n.rawAdmissionReent=G::state().reent38544C;
    if(*n.rawAdmissionReent!=G::StdioIdentity::reent385160)return finish(WaterPoolMaterialResult::Status::nativeStopped);
    n.stop=S::rawCountCapacity;
    if(*n.originalCount>nativeWaterArrayMaxCount)return finish(WaterPoolMaterialResult::Status::nativeStopped);
    const auto full=std::uint64_t(*n.originalCount)*4ull;n.stop=S::rawByteCapacity;
    if(full>nativeWaterArrayMaxBytes)return finish(WaterPoolMaterialResult::Status::nativeStopped);
    n.stop=S::rawRequestMismatch;
    if(full!=*x.requestedBytes||!*x.requestedBytes||(*x.requestedBytes&0x80000000u)||
       (*x.sumWord&0x80000000u)||(*x.normalizedChunk&0x80000000u)||*x.normalizedChunk<*x.requestedBytes)
        return finish(WaterPoolMaterialResult::Status::nativeStopped);
    n.stop=S::rawOwnerAllocation;n.rawOwnerAttempted=true;
    std::unique_ptr<SourceWaterPoolMaterialArrayOwner> pendingArray(new(std::nothrow) SourceWaterPoolMaterialArrayOwner);
    if(!pendingArray)return finish(WaterPoolMaterialResult::Status::nativeStopped);
    n.rawOwnerPrepared=true;n.stop=S::rawSlotAllocation;n.slotsAttempted=true;
    pendingArray->slots.reset(new(std::nothrow) NativeWaterPoolMaterialSlot[*n.originalCount]);
    if(!pendingArray->slots){pendingArray.reset();n.rawTemporaryRetired=true;return finish(WaterPoolMaterialResult::Status::nativeStopped);}
    n.slotsPrepared=true;n.stop=S::rawPayloadAllocation;n.rawPayloadAttempted=true;
    pendingArray->payload.reset(std::malloc(static_cast<std::size_t>(full))); //Raw payload is NOT initialized.
    if(!pendingArray->payload){pendingArray.reset();n.rawTemporaryRetired=true;return finish(WaterPoolMaterialResult::Status::nativeStopped);}
    pendingArray->role.nativeOwner=pendingArray.get();pendingArray->role.dataOwner=&data;
    pendingArray->role.payloadIdentity=pendingArray->payload.get();pendingArray->role.epoch=1u;
    pendingArray->originalCount=*n.originalCount;pendingArray->slotCapacity=*n.originalCount;
    pendingArray->requestedBytes=*x.requestedBytes;data.nativeMaterials=std::move(pendingArray);
    auto& array=*data.nativeMaterials;n.rawAllocated=true;n.arrayEpoch=1u;n.arrayRoleIdentity=&array.role;
    n.pc=0x2bb1b8;data.materials10=SourceWaterPoolDataOwner::MaterialArrayView{&array.role,0u};n.arrayAliasStored=true;
    //Supersede the constructor's historic2BB154 zero, never fabricate pointer bits.
    for(std::size_t i=0x10u;i<0x14u;++i){data.knownMask[i]=0u;data.knownBits[i]=0u;}
    n.pc=0x2bb1bc;n.loopBranchIssued=true;n.pc=0x2bb1c0;n.ordinal=0u;n.ordinalZeroed=true;
    n.pc=0x2bb228;n.countReloaded=true;n.pc=0x2bb22c;n.loopCompared=true;
    n.pc=0x2bb230;n.loopTaken=true;n.pc=0x2bb234;n.loopDelay=true;n.pc=0x2bb1c4;
    //Separate first-child scalar new. Fresh30570C capture; source allocation
    //failure/new-handler/NULL paths are not native admission failures.
    c.attempted=true;c.callIssued=true;c.pc=0x2bb1c4;c.callerReturnPC=0x2bb1ccu;
    c.pc=0x2bb1c8;c.requestedBytes=0xd8u;c.argumentA0=0xd8ull;
    c.pc=0x2e8d90;c.operatorFrame=true;c.pc=0x2e8d94;c.operatorSaveMask|=1u;
    c.pc=0x2e8d98;c.operatorSaveMask|=2u;c.pc=0x2e8d9c;c.operatorSaveMask|=4u;
    c.pc=0x2e8da0;c.effectiveBytes=0xd8u;c.effectiveArgument=0xd8ull;
    c.pc=0x2e8da4;c.nonzeroBranch=true;c.pc=0x2e8da8;c.operatorFramePointer=true;
    c.pc=0x2e8e1c;c.pc=0x2e8e20;c.pc=0x2e8e24;
    c.pc=0x305700;c.pc=0x305704;c.pc=0x305708;c.mallocFrame=true;c.pc=0x30570c;
    n.stop=S::childSourceReentrancy;
    if(!G::reentrancyCurrentNative()){c.reentStop=C::unknown;return finish(WaterPoolMaterialResult::Status::nativeStopped);}
    c.reent=G::state().reent38544C;
    if(*c.reent!=G::StdioIdentity::reent385160){c.reentStop=C::unsupportedIdentity;
        return finish(WaterPoolMaterialResult::Status::nativeStopped);}
    c.pc=0x305710;c.mallocSaveMask=1u;c.pc=0x305714;c.pc=0x305718;
    c.pc=0x305990;c.allocatorFrame=true;c.pc=0x305994;c.sumWord=0xebu;c.sumRegister=0xebull;
    c.pc=0x305998;c.allocatorSaveMask|=1u;c.pc=0x30599c;c.minimumChunk=false;
    c.pc=0x3059a0;c.allocatorSaveMask|=2u;c.pc=0x3059a4;c.reentCopied=true;
    c.pc=0x3059a8;c.allocatorSaveMask|=4u;c.pc=0x3059ac;c.allocatorSaveMask|=8u;
    c.pc=0x3059b0;c.allocatorSaveMask|=16u;c.pc=0x3059b4;c.pc=0x3059b8;c.allocatorSaveMask|=32u;
    c.pc=0x3059bc;c.pc=0x3059c0;c.pc=0x3059c4;c.pc=0x3059c8;c.normalizedChunk=0xe0u;c.normalizedRegister=0xe0ull;
    c.pc=0x3059d0;c.lockCalled=true;c.pc=0x3059d4;c.pc=0x306538;c.pc=0x30653c;c.lockReturned=true;
    c.pc=0x3059d8;c.smallBin=true;c.pc=0x3059dc;c.pc=0x3059e0;c.pc=0x3059e4;c.arenaBase=0x385468u;
    c.pc=0x3059e8;c.arenaReadOffset=0xecu;c.arenaReadAddress=0x385554u;c.pc=0x3059ec;
    n.stop=S::childNativeReentrancy;
    if(!G::reentrancyCurrentNative())return finish(WaterPoolMaterialResult::Status::nativeStopped);
    n.childAdmissionReent=G::state().reent38544C;
    if(*n.childAdmissionReent!=G::StdioIdentity::reent385160)return finish(WaterPoolMaterialResult::Status::nativeStopped);
    n.stop=S::childOwnerAllocation;n.childOwnerAttempted=true;
    std::unique_ptr<SourceWaterPoolMaterialOwner> pendingChild(new(std::nothrow) SourceWaterPoolMaterialOwner);
    if(!pendingChild)return finish(WaterPoolMaterialResult::Status::nativeStopped);
    n.childOwnerPrepared=true;n.stop=S::childPayloadAllocation;n.childPayloadAttempted=true;
    pendingChild->payload.reset(std::malloc(0xd8u)); //Exact216 bytes, not normalized224; no blanket zero.
    if(!pendingChild->payload){pendingChild.reset();n.childTemporaryRetired=true;return finish(WaterPoolMaterialResult::Status::nativeStopped);}
    pendingChild->role.nativeOwner=pendingChild.get();pendingChild->role.arrayOwner=&array;
    pendingChild->role.payloadIdentity=pendingChild->payload.get();pendingChild->role.entityFacet=&pendingChild->entity;
    pendingChild->role.epoch=1u;pendingChild->role.arrayEpoch=1u;pendingChild->role.ordinal=0u;
    array.firstChild=std::move(pendingChild);auto& m=*array.firstChild;auto& e=m.entity;
    n.childAllocated=true;n.childEpoch=1u;n.childRoleIdentity=&m.role;n.entityIdentity=&e;
    n.pc=0x2bb1cc;n.callerNonnull=true;n.pc=0x2bb1d0;n.pc=0x2bb1d4;n.constructorA1=1u;
    n.pc=0x2bb1d8;n.constructorCallIssued=true;n.constructorReturnPC=0x2bb1e0u;
    n.pc=0x2bb1dc;n.constructorDelay=true;m.wholeArgument=SourceWaterPoolMaterialOwner::View{&m.role,0u};
    n.pc=0x2bf510;
    auto& authority=worldApplication_->entityAuthority_;auto& actualCount=worldApplication_->objectCount423F60();
    //All fallible capacity and exact-facet admission precede the FIRST source
    //constructor write/reference. Native refusal retains a backing-only child.
    n.stop=S::countCapacity;
    if(actualCount==std::numeric_limits<std::uint32_t>::max())return finish(WaterPoolMaterialResult::Status::nativeStopped);
    n.stop=S::authorityCapacity;
    if(!authority.reserveAdditional(1))return finish(WaterPoolMaterialResult::Status::nativeStopped);
    n.stop=S::authorityAdmission;
    if(!worldEntity_.sessionGeneration||!key(&e)||!authority.canEnroll(e,key(&e))||e.reference00||e.objectKey||
       e.sessionGeneration||e.kind!=g::OriginalWreckComponentKind50LA::unbound||e.previous10||e.next14||
       e.firstChild18||e.parent1C||e.mask20||e.descendants24||e.category28||e.flags29||e.nativeFlags29||e.nativeEffectControlId||
       m.authorityLive||m.countLive||&m!=array.firstChild.get()||m.role.nativeOwner!=&m||m.role.entityFacet!=&e)
        return finish(WaterPoolMaterialResult::Status::nativeStopped);
    n.constructorAdmitted=true;n.session=worldEntity_.sessionGeneration;n.key=key(&e);
    auto* bytes=static_cast<std::uint8_t*>(m.payload.get());
    const auto store=[&](std::uint32_t pc,std::uint32_t offset,std::uint32_t width,std::uint64_t word,std::uint8_t mask=255u)noexcept {
        n.pc=pc;n.stores[n.storesWritten++]={pc,offset,width,word,mask};
        for(std::size_t i=0;i<width;++i){const auto at=offset+i;const auto value=std::uint8_t(word>>(8u*i));
            m.knownBits[at]=std::uint8_t((m.knownBits[at]&~mask)|(value&mask));
            m.knownMask[at]=std::uint8_t(m.knownMask[at]|mask);
            if(mask==255u)bytes[at]=value; //Never read/write unknown upper flag bits.
        }
    };
    const auto emptyCopy=[&](N::EmptyCopy& copy,std::uint32_t call,std::uint32_t delay,std::uint32_t ret)noexcept {
        n.pc=call;copy.callIssued=true;copy.callPC=call;n.pc=delay;copy.delayPC=delay;
        copy.helperEntry=0x309230u;copy.literal=0x3c11a0u;copy.destinationOffset=0x30u;
        //The16-byte-aligned immutable literal has16 known zero bytes. For
        //unaligned M+30,30923C takes the byte loop. For8-not16 alignment,
        //30927C LD sees0 and its detector branches3092F0 BEFORE SD3092F8.
        //For16 alignment, the likely LD is annulled; LQ309284 sees16 zero
        //bytes, PSUBB/PNOR/PAND plus both-half OR branch3092A4 BEFORE SQ3092AC.
        //Every alignment therefore reaches LBU/SB309320/328 once and exits
        //on the first NUL. We claim no alignment or selected wide-read path.
        copy.allAlignmentsProved=true;store(0x309328u,0x30u,1u,0u);copy.storedBytes=1u;
        n.pc=0x309334;n.pc=0x309338;n.pc=0x30933c;n.pc=0x309340;
        copy.returned=true;copy.returnPC=ret;n.pc=ret;
    };
    n.pc=0x2bf510;n.materialFrame=true;n.pc=0x2bf514;n.materialSaveMask=1u;
    n.pc=0x2bf518;n.materialWholeSaved=true;n.pc=0x2bf51c;n.entityCallIssued=true;
    n.pc=0x2bf520;n.entityDelay=true;n.entityOffset=0u;m.entityArgument=SourceWaterPoolMaterialOwner::View{&m.role,0u};
    n.pc=0x141300;n.entityFrame=true;n.pc=0x141304;n.entitySaveMask|=1u;
    n.pc=0x141308;n.entitySaveMask|=2u;n.pc=0x14130c;n.entitySaveMask|=4u;n.pc=0x141310;
    n.pc=0x141314;n.referenceCallIssued=true;n.pc=0x141318;n.pc=0x1048f0;n.pc=0x1048f4;
    store(0x1048f8,8,4,0x3cc980);n.pc=0x1048fc;store(0x104900,0,8,1);e.reference00=1u;n.referenceAcquired=true;
    e.sessionGeneration=*n.session;e.objectKey=*n.key;e.kind=g::OriginalWreckComponentKind50LA::entity141300;
    n.pc=0x104904;n.pc=0x104908;n.referenceReturned=true;
    n.pc=0x14131c;n.pc=0x141320;store(0x141324,8,4,0x3cd240);n.pc=0x141328;
    store(0x14132c,0x28,1,255);e.category28=255u;
    store(0x141330,0x10,4,0);e.previous10=nullptr;store(0x141334,0x14,4,0);e.next14=nullptr;
    store(0x141338,0x18,4,0);e.firstChild18=nullptr;store(0x14133c,0x1c,4,0);e.parent1C=nullptr;
    store(0x14135c,0x29,1,1,1);e.flags29=std::uint8_t((e.flags29&~1u)|1u);
    store(0x14137c,0x29,1,2,2);e.flags29=std::uint8_t((e.flags29&~2u)|2u);
    store(0x141398,0x29,1,0,4);e.flags29=std::uint8_t(e.flags29&~4u);
    store(0x1413a8,0x29,1,0,8);e.flags29=std::uint8_t(e.flags29&~8u);
    store(0x1413ac,0x20,4,0);e.mask20=0;store(0x1413b0,0x24,4,0);e.descendants24=0;
    n.pc=0x1413b4;n.countBefore=actualCount;n.pc=0x1413b8;const auto incremented=std::uint32_t(actualCount+1u);
    n.pc=0x1413bc;n.pc=0x1413c0;actualCount=incremented;n.countAcquired=true;m.countLive=true;
    //The exact admitted canonical facet is enrolled once. No copied Entity,
    //extra retain, type555 registration or generic dispatch is introduced.
    if(!authority.enroll(e))std::terminate();n.enrolled=true;m.authorityLive=true;
    n.pc=0x1413c4;n.entityRestoreMask|=1u;n.pc=0x1413c8;n.entityRestoreMask|=2u;
    n.pc=0x1413cc;n.entityRestoreMask|=4u;n.pc=0x1413d0;n.pc=0x1413d4;n.entityReturned=true;
    n.pc=0x2bf524;n.pc=0x2bf528;n.pc=0x2bf52c;n.pc=0x2bf530;n.pc=0x2bf534;
    store(0x2bf538,8,4,0x3d48c0);n.pc=0x2bf53c;
    emptyCopy(n.constructorCopy,0x2bf540,0x2bf544,0x2bf548);
    n.pc=0x2bf54c;n.pc=0x2bf550;n.pc=0x2bf554;n.pc=0x2bf558;
    store(0x2bf55c,0xd0,4,0);n.pc=0x2bf560;store(0x2bf564,0xd4,4,0);n.pc=0x2bf568;
    store(0x2bf56c,0x30,1,0);n.pc=0x2bf570;store(0x2bf574,0x80,4,0);n.pc=0x2bf578;
    store(0x2bf57c,0x8c,4,0);n.pc=0x2bf580;store(0x2bf584,0x88,4,1);n.pc=0x2bf588;
    store(0x2bf58c,0x90,4,0);n.pc=0x2bf590;store(0x2bf594,0x94,4,0);n.pc=0x2bf598;
    store(0x2bf59c,0xd0,4,0);n.pc=0x2bf5a0;store(0x2bf5a4,0xac,2,0);n.pc=0x2bf5a8;
    store(0x2bf5ac,0xa0,4,0);n.pc=0x2bf5b0;store(0x2bf5b4,0xa4,4,0x3f800000);n.pc=0x2bf5b8;
    store(0x2bf5bc,0xb0,4,0);n.pc=0x2bf5c0;store(0x2bf5c4,0xa8,4,0);n.pc=0x2bf5c8;
    store(0x2bf5cc,0x84,4,0);n.pc=0x2bf5d0;store(0x2bf5d4,0x98,4,0);n.pc=0x2bf5d8;
    store(0x2bf5dc,0x9c,4,1);n.pc=0x2bf5e0;store(0x2bf5e4,0xb4,4,0);n.pc=0x2bf5e8;
    store(0x2bf5ec,0x74,4,0x00ff00ff);n.pc=0x2bf5f0;store(0x2bf5f4,0xb8,4,0);n.pc=0x2bf5f8;
    store(0x2bf5fc,0xc4,4,0);n.pc=0x2bf600;n.pc=0x2bf604;n.materialRestoreMask=1u;
    n.pc=0x2bf608;n.pc=0x2bf60c;n.constructorStackReleased=true;n.constructorReturned=true;
    n.pc=0x2bb1e0;n.pc=0x2bb1e4;n.pc=0x2bb1e8;n.pc=0x2bb1ec;
    array.slots[0]={&m.role,0u,true};n.slotStored=true; //ONLY ordinal0, after actual constructor return.
    n.pc=0x2bb1f0;n.pc=0x2bb1f4;n.pc=0x2bb1f8;n.setupSlotReloaded=true;
    n.pc=0x2bb1fc;n.setupCallIssued=true;n.setupReturnPC=0x2bb204u;
    n.pc=0x2bb200;n.setupDelay=true;n.setupArgument=555u; //Actual R+18 reload, not a type lookup.
    n.pc=0x2bf610;n.setupFrame=true;n.pc=0x2bf614;n.setupSaveMask|=1u;n.pc=0x2bf618;n.setupSaveMask|=2u;
    n.pc=0x2bf61c;n.setupIdentityCalled=true;n.pc=0x2bf620;n.pc=0x141c20;n.pc=0x141c24;
    n.setupIdentityReturned=true; //JR/POR returns SAME M; no mutation/retain/count.
    n.pc=0x2bf624;n.pc=0x2bf628;emptyCopy(n.setupCopy,0x2bf62c,0x2bf630,0x2bf634);
    store(0x2bf634,0xd0,4,0);n.pc=0x2bf638;n.pc=0x2bf63c;n.setupRestoreMask|=1u;
    n.pc=0x2bf640;n.setupRestoreMask|=2u;n.pc=0x2bf644;n.pc=0x2bf648;n.setupStackReleased=true;n.setupReturned=true;
    n.pc=0x2bb204;n.pc=0x2bb208;n.pc=0x2bb20c;n.readerHandle=d.savedHandleS3;
    n.pc=0x2bb210;n.pc=0x2bb214;n.readerSlotReloaded=true;
    n.pc=0x2bb218;n.readerCallIssued=true;n.readerReturnPC=0x2bb220u;
    n.pc=0x2bb21c;n.readerDelay=true;n.readerFlag=sourceWaterConfig_.nativeWaterPoolName.argumentA3;
    n.pc=0x2bf700;n.readerFrame=true;n.pc=0x2bf704;n.readerSaveMask|=1u;
    n.pc=0x2bf708;n.readerSaveMask|=2u;n.pc=0x2bf70c;n.readerSaveMask|=4u;n.pc=0x2bf710;n.readerSaveMask|=8u;
    n.pc=0x2bf714;n.readerWholeSelected=true;n.pc=0x2bf718;n.pc=0x2bf71c;n.stop=S::service2BF71C;
    auto& l=n.load2BF71C;l.attempted=true;l.pc=0x2bf71cu;
    if(!waterPoolFileServiceCurrentNative())return finish(WaterPoolMaterialResult::Status::nativeStopped);
    const auto& domain=sourceTrackMountDomain_;const auto& first=f.load2BB044;
    if(first.applicationIdentity!=sourceLoadApplication_||first.domainIdentity!=domain.get()||
       first.resolverIdentity!=domain->resolver3E1A70().get()||first.providerIdentity!=domain->archiveProvider3E1A70().get()||
       first.trackIdentity!=sourceTrackFile_.get())return finish(WaterPoolMaterialResult::Status::nativeStopped);
    const void* mountIdentity=nullptr;std::size_t mount=static_cast<std::size_t>(-1);std::uint64_t generation=0;
    if(!domain->resolver3E1A70()->waterPoolTrackRelationNative(sourceTrackFile_.get(),domain->archiveProvider3E1A70().get(),
       mountIdentity,mount,generation)||mountIdentity!=first.trackMountIdentity||mount!=first.trackMount||
       generation!=first.trackGeneration)return finish(WaterPoolMaterialResult::Status::nativeStopped);
    l.applicationIdentity=sourceLoadApplication_;l.domainIdentity=domain.get();l.resolverIdentity=domain->resolver3E1A70().get();
    l.providerIdentity=domain->archiveProvider3E1A70().get();l.trackIdentity=sourceTrackFile_.get();
    l.trackMountIdentity=mountIdentity;l.trackMount=mount;l.trackGeneration=generation;l.resolved=true;
    n.pc=0x2bf720;n.readerFlagPreserved=true;n.pc=0x2bf724;n.destinationOffset=0x30u;
    n.pc=0x2bf728;n.itemSize=64u;n.pc=0x2bf72c;n.itemCount=1u;n.pc=0x2bf730;n.stop=S::readCall2BF730;
    return finish(WaterPoolMaterialResult::Status::sourceBoundary);
    //JAL2BF730 and delay2BF734 are BOTH pending. No s2 handle save, read,
    //cursor movement, return2BF738/2BB220, ordinal increment or second child.
}
