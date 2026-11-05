//Included inside ra_cold_aggregate50LA by the Pool object leaf. This is a
//one-shot native continuation of the SAME detached Pool/Entity at289420.
//Only complete registry success reaches before2B9D28. A native refusal keeps
//every preceding scene byte and the previously acquired reference/count.
bool Owner::preflightNativeWaterPoolSceneHistory()const noexcept {
    const auto& n=sourceWaterConfig_.nativeWaterPoolScene;
    const auto& c=sourceWaterConfig_.nativeWaterPoolEntity;
    using S=g::OriginalEntityTypeRegistry50LA::Status;
    const auto noSuffix=[&]()noexcept {
        return !n.type02Stored&&!n.category30Stored&&!n.zero1A4Stored&&
            !n.sceneFlags191Mask&&!n.sceneFlags191Bits&&!n.sceneRestoreMask&&
            !n.sceneWholeSelected&&!n.sceneReturned&&!n.sceneStackReleased&&
            !n.sceneReturnPC&&!n.sceneReturnOffset&&!n.poolDispatchMask&&!n.poolHeader03Mask&&
            !n.poolHeader03Bits&&!n.poolZeroWordsWritten&&!n.poolFlags191Mask&&!n.poolFlags191Bits&&
            !n.poolRestoreMask&&!n.poolWholeSelected&&!n.poolReturned&&!n.poolStackReleased&&
            !n.poolReturnPC&&!n.poolReturnOffset;
    };
    if(!n.attempted)return !n.pc&&!n.applicationIdentity&&!n.registryIdentity&&!n.wholeRoleIdentity&&
        !n.sceneDispatchMask&&!n.vectorsWritten&&!n.zeroWordsWritten&&!n.fillCalls&&!n.fillReturns&&
        !n.identityWordsWritten&&!n.xyzWordsWritten&&!n.flags190Mask&&!n.flags190Bits&&
        !n.zero19CStored&&!n.header03Bit2Cleared&&!n.name38Stored&&!n.zero1A0Stored&&
        !n.registryCallIssued&&!n.applicationRead&&!n.registryReturned&&!n.registryStatus&&
        !n.registrySourcePC&&!n.registryCountBefore&&!n.registryCountAfter&&!n.returnedType&&noSuffix();
    if(!c.admitted||c.refusal!=WaterPoolEntityRefusal::none||c.pc!=0x289420u||
       !n.applicationIdentity||!n.registryIdentity||!n.wholeRoleIdentity||
       n.sceneDispatchMask!=3u||n.vectorsWritten!=4u||n.zeroWordsWritten!=4u||
       n.fillCalls!=2u||n.fillReturns!=2u||n.identityWordsWritten!=4u||n.xyzWordsWritten!=12u||
       n.flags190Mask!=0xffu||n.flags190Bits!=0x80u||!n.zero19CStored||!n.header03Bit2Cleared||
       !n.name38Stored||!n.zero1A0Stored||!n.registryCallIssued||!n.applicationRead||
       !n.registryStatus||!n.registryCountBefore||!n.registryCountAfter||
       *n.registryCountBefore>255u||*n.registryCountAfter>255u)return false;
    //These are immutable call receipts, NOT current registry invariants.
    //Do not query count00/name here: later appends or a terminal registry
    //must not strand this still-detached Pool's safe native retirement.
    const auto before=*n.registryCountBefore,after=*n.registryCountAfter;
    if(*n.registryStatus!=S::complete){
        if(n.pc!=0x289658u||n.registryReturned||n.returnedType||!noSuffix())return false;
        switch(*n.registryStatus){
        case S::capacity:return n.registrySourcePC==0x2a327cu&&before==255u&&after==before;
        case S::allocation:return n.registrySourcePC==0x2a32c4u&&before<255u&&after==before+1u;
        case S::terminal:case S::input:return n.registrySourcePC==0x2a31e0u&&after==before;
        default:return false;
        }
    }
    if(!n.registryReturned||!n.returnedType||*n.returnedType==0xffu)return false;
    if(n.registrySourcePC==0x2a3228u){if(after!=before||*n.returnedType>=before)return false;}
    else if(n.registrySourcePC==0x2a3308u){
        if(before>=255u||after!=before+1u||*n.returnedType!=before)return false;
    }else return false;
    return n.pc==0x2b9d28u&&n.type02Stored&&n.category30Stored&&n.zero1A4Stored&&
        n.sceneFlags191Mask==3u&&n.sceneFlags191Bits==1u&&n.sceneRestoreMask==1u&&
        n.sceneWholeSelected&&n.sceneReturned&&n.sceneStackReleased&&n.sceneReturnPC==0x2bae44u&&
        n.sceneReturnOffset==0u&&n.poolDispatchMask==3u&&n.poolHeader03Mask==7u&&n.poolHeader03Bits==3u&&
        n.poolZeroWordsWritten==3u&&n.poolFlags191Mask==3u&&!n.poolFlags191Bits&&n.poolRestoreMask==3u&&
        n.poolWholeSelected&&n.poolReturned&&n.poolStackReleased&&n.poolReturnPC==0x2b9d28u&&n.poolReturnOffset==0u;
}

Owner::WaterPoolSceneResult Owner::constructWaterPoolSceneNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterPoolSceneResult result;auto& n=sourceWaterConfig_.nativeWaterPoolScene;
    const auto& c=sourceWaterConfig_.nativeWaterPoolEntity;
    const auto& b=sourceWaterConfig_.nativeWaterPoolBacking;
    const auto finish=[&](WaterPoolSceneResult::Status status)noexcept {
        result.status=status;result.originalSourceStop=0x30492cu;
        result.allocatorStop=sourceWaterConfig_.nativeWaterPoolAllocationEntry.pc;
        result.formatterStop=sourceWaterConfig_.nativeWaterPoolPathFormat.pc;result.entityPrefixStop=c.pc;
        result.adaptedPC=n.pc;result.registryCallIssued=n.registryCallIssued;result.registryReturned=n.registryReturned;
        result.registryStatus=n.registryStatus;result.registrySourcePC=n.registrySourcePC;
        result.registryCountBefore=n.registryCountBefore;result.registryCountAfter=n.registryCountAfter;
        result.returnedType=n.returnedType;result.sceneReturned=n.sceneReturned;result.poolReturned=n.poolReturned;
        result.sceneReturnPC=n.sceneReturnPC;result.poolReturnPC=n.poolReturnPC;result.nativeRetired=b.nativeRetired;
        return result;
    };
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||nativeScbThread_!=std::this_thread::get_id()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_)return result;
    std::size_t counted=0;if(!sourceWaterPoolCountedNative(counted))return result;
    //After either native refusal or success, return history only. This also
    //works after Pool retirement while a later provider withdrawal is pending.
    if(n.attempted)return finish(WaterPoolSceneResult::Status::alreadyAttempted);
    if(!waterPoolInvocationCurrentNative()||!preflightSourceWaterConfigNative()||
       !preflightSourceWorldParticleCountNative()||counted!=1u||!c.admitted||c.pc!=0x289420u||
       !b.nativeAllocated||b.nativeRetired||!sourceWaterProvider_->nativePool)return result;
    auto& p=*sourceWaterProvider_->nativePool;auto& e=p.entity;
    auto* bytes=static_cast<unsigned char*>(p.payload.get());
    const auto store=[&](std::size_t offset,std::size_t width,std::uint32_t value)noexcept {
        for(std::size_t i=0;i<width;++i){const auto v=std::uint8_t(value>>(i*8u));
            bytes[offset+i]=v;p.knownMask[offset+i]=0xffu;p.knownBits[offset+i]=v;}
    };
    const auto bits=[&](std::size_t offset,std::uint8_t mask,std::uint8_t value)noexcept {
        //Only sidecar bits are read. Never load an indeterminate raw flag.
        p.knownBits[offset]=std::uint8_t((p.knownBits[offset]&~mask)|(value&mask));
        p.knownMask[offset]=std::uint8_t(p.knownMask[offset]|mask);
        if(p.knownMask[offset]==0xffu)bytes[offset]=p.knownBits[offset];
    };
    const auto flags190=[&](std::uint32_t pc,std::uint8_t mask,std::uint8_t value)noexcept {
        n.pc=pc;bits(0x190u,mask,value);n.flags190Mask=p.knownMask[0x190u];n.flags190Bits=p.knownBits[0x190u];
    };
    //REPORT.md's complete306478 decode proves exactly64 zero bytes for
    //a1=0,a2=64 on BOTH alignment paths. This bounded projection does not
    //execute libc/the guest helper and establishes no other payload bytes.
    const auto zero64=[&](std::size_t offset)noexcept {
        for(std::size_t i=0;i<64u;++i)store(offset+i,1u,0u);
    };
    n.attempted=true;n.pc=0x289420u;n.wholeRoleIdentity=&p.role;
    n.pc=0x289434;store(0x1a8u,4u,0x3d3190u);n.sceneDispatchMask|=1u;
    n.pc=0x28943c;store(0x10u,4u,0x3d3198u);n.sceneDispatchMask|=2u;
    //SQC2 VF0: architectural read-only(+0,+0,+0,+1), not integer zero
    //or allocator initialization. Four exact128-bit source vector writes.
    for(std::uint32_t v=0;v<4u;++v){
        n.pc=0x289448u+12u*v;
        for(std::uint32_t lane=0;lane<4u;++lane)store(0x140u+16u*v+4u*lane,4u,lane==3u?0x3f800000u:0u);
        ++n.vectorsWritten;
    }
    n.pc=0x28947c;store(0x180u,4u,0u);++n.zeroWordsWritten;
    n.pc=0x289484;store(0x184u,4u,0u);++n.zeroWordsWritten;
    n.pc=0x28948c;store(0x18cu,4u,0u);++n.zeroWordsWritten;
    n.pc=0x289494;store(0x188u,4u,0u);++n.zeroWordsWritten;
    n.pc=0x28949c;++n.fillCalls;n.pc=0x2894a0;zero64(0xc0u);
    n.pc=0x2894a4;++n.fillReturns;
    n.pc=0x2894b4;store(0xfcu,4u,0x3f800000u);++n.identityWordsWritten;
    n.pc=0x2894bc;store(0xe8u,4u,0x3f800000u);++n.identityWordsWritten;
    n.pc=0x2894c4;store(0xd4u,4u,0x3f800000u);++n.identityWordsWritten;
    n.pc=0x2894cc;store(0xc0u,4u,0x3f800000u);++n.identityWordsWritten;
    n.pc=0x2894d4;++n.fillCalls;n.pc=0x2894d8;zero64(0x100u);
    n.pc=0x2894dc;++n.fillReturns; //The SECOND matrix remains all zero.
    flags190(0x289508u,1u,0u);flags190(0x28953cu,0x80u,0x80u);
    n.pc=0x289554;store(0x19cu,4u,0u);n.zero19CStored=true;
    n.pc=0x289568;bits(3u,4u,0u);n.header03Bit2Cleared=true;
    flags190(0x28957cu,2u,0u);flags190(0x289590u,4u,0u);flags190(0x2895a4u,8u,0u);
    flags190(0x2895b8u,16u,0u);flags190(0x2895ccu,32u,0u);
    n.pc=0x2895d4;store(0x140u,4u,0u);++n.xyzWordsWritten;
    n.pc=0x2895dc;store(0x144u,4u,0u);++n.xyzWordsWritten;
    n.pc=0x2895e4;store(0x148u,4u,0u);++n.xyzWordsWritten;
    n.pc=0x2895ec;store(0x150u,4u,0u);++n.xyzWordsWritten;
    n.pc=0x2895f4;store(0x154u,4u,0u);++n.xyzWordsWritten;
    n.pc=0x2895fc;store(0x158u,4u,0u);++n.xyzWordsWritten;
    flags190(0x289610u,64u,0u); //Now maskFF: materialize raw190=80.
    n.pc=0x289618;store(0x160u,4u,0u);++n.xyzWordsWritten;
    n.pc=0x289620;store(0x164u,4u,0u);++n.xyzWordsWritten;
    n.pc=0x289628;store(0x168u,4u,0u);++n.xyzWordsWritten;
    n.pc=0x289630;store(0x170u,4u,0u);++n.xyzWordsWritten;
    n.pc=0x289638;store(0x174u,4u,0u);++n.xyzWordsWritten;
    n.pc=0x289640;store(0x178u,4u,0u);++n.xyzWordsWritten; //W lanes untouched.
    n.pc=0x289648;store(0x38u,1u,0u);n.name38Stored=true; //One byte only.
    n.pc=0x289650;store(0x1a0u,4u,0u);n.zero1A0Stored=true;
    n.pc=0x289654;n.registryCallIssued=true;
    n.pc=0x289658;n.applicationIdentity=worldApplication_.get();n.applicationRead=true;
    auto& registry=worldApplication_->types383408_;n.registryIdentity=&registry;
    n.registryCountBefore=registry.count00();
    //Fresh383408 delay-slot load, exact3BCFA0 literal. Not the provider's
    //SearchableGameObject cache, a copied registry, or a fixed category ID.
    const auto lookup=registry.findOrAppend2A31E0("SoultreeObject");
    n.registryStatus=lookup.status;n.registrySourcePC=lookup.sourcePC;n.registryCountAfter=registry.count00();
    if(!lookup.okay())return finish(WaterPoolSceneResult::Status::nativeRegistryStopped);
    //No use of default Result.type on refusal. Shared lazy/count/name/terminal
    //effects are never undone, retried, or tied to this Pool's retirement.
    n.registryReturned=true;n.returnedType=lookup.type;n.pc=0x28965c;
    n.pc=0x289670;store(2u,1u,*n.returnedType);n.type02Stored=true;
    n.pc=0x28967c;const auto category=p.knownBits[2u];
    n.pc=0x289680;store(0x30u,1u,category);e.category28=category;n.category30Stored=true;
    n.pc=0x289694;bits(0x191u,1u,1u);n.sceneFlags191Mask=1u;n.sceneFlags191Bits=1u;
    n.pc=0x2896a8;bits(0x191u,2u,0u);n.sceneFlags191Mask=3u;
    n.pc=0x2896b0;store(0x1a4u,4u,0u);n.zero1A4Stored=true;
    n.pc=0x2896b4;n.sceneWholeSelected=true;n.sceneReturnOffset=0u;
    n.pc=0x2896b8;n.sceneRestoreMask=1u;
    n.pc=0x2896bc;n.sceneReturnPC=0x2bae44u;
    n.pc=0x2896c0;n.sceneStackReleased=true;n.sceneReturned=true;
    n.pc=0x2bae44;
    n.pc=0x2bae4c;store(0x1a8u,4u,0x3d47f0u);n.poolDispatchMask|=1u;
    n.pc=0x2bae58;store(0x10u,4u,0x3d47f8u);n.poolDispatchMask|=2u;
    n.pc=0x2bae74;bits(3u,1u,1u);n.poolHeader03Mask=p.knownMask[3u];n.poolHeader03Bits=p.knownBits[3u];
    n.pc=0x2bae84;n.poolWholeSelected=true;n.poolReturnOffset=0u;
    n.pc=0x2bae90;bits(3u,2u,2u);n.poolHeader03Mask=p.knownMask[3u];n.poolHeader03Bits=p.knownBits[3u];
    n.pc=0x2bae94;store(0x1b4u,4u,0u);++n.poolZeroWordsWritten;
    n.pc=0x2bae98;store(0x1b0u,4u,0u);++n.poolZeroWordsWritten;
    n.pc=0x2baea8;bits(0x191u,1u,0u);n.poolFlags191Mask=p.knownMask[0x191u];n.poolFlags191Bits=p.knownBits[0x191u];
    n.pc=0x2baeac;store(0x1bcu,4u,0u);++n.poolZeroWordsWritten;
    n.pc=0x2baeb0;n.poolRestoreMask|=1u;n.pc=0x2baeb4;n.poolRestoreMask|=2u;
    n.pc=0x2baeb8;n.poolReturnPC=0x2b9d28u;
    n.pc=0x2baebc;n.poolStackReleased=true;n.poolReturned=true;
    n.pc=0x2b9d28; //BEFORE caller ADDIU. No loader, transform, resource or array slot.
    return finish(WaterPoolSceneResult::Status::nativePoolReturnBoundary);
}
