//New scalar operator-new occurrence for the first Pool object. This is NOT
//the earlier pool-pointer new[] occurrence or a native backing allocation.
bool Owner::preflightNativeWaterPoolAllocationEntry()const noexcept {
    std::size_t poolCount=0;if(!sourceWaterPoolCountedNative(poolCount))return false;
    const auto& n=sourceWaterConfig_.nativeWaterPoolAllocationEntry;
    const auto& f=sourceWaterConfig_.nativeWaterPoolPathFormat;
    using G=a::OriginalConfigProcessGlobals50LA;using Stop=SourceWaterConfig::NativeWaterPoolAllocationEntry::ReentStop;
    const auto noArena=[&]()noexcept {return !n.allocatorFrame&&!n.allocatorSaveMask&&!n.reentCopied&&!n.lockCalled&&!n.lockReturned&&
        !n.sumWord&&!n.sumRegister&&!n.normalizedChunk&&!n.normalizedRegister&&!n.minimumChunk&&!n.smallBin&&
        !n.arenaBase&&!n.arenaReadOffset&&!n.arenaReadAddress;};
    if(!n.attempted)return !n.pc&&!n.callIssued&&!n.requestedBytes&&!n.effectiveBytes&&!n.callerReturnPC&&
        !n.argumentA0&&!n.effectiveArgument&&!n.operatorFrame&&!n.operatorFramePointer&&!n.nonzeroBranch&&
        !n.mallocFrame&&!n.operatorSaveMask&&!n.mallocSaveMask&&n.reentStop==Stop::none&&!n.reent&&noArena();
    if(!sourceWaterConfigOwner_||sourceWaterConfigOwner_->pc_!=0x30492cu||sourceWaterConfigOwner_->wakeSelection_.returned||
       !f.attempted||f.phase!=SourceWaterConfig::NativeWaterPoolPathFormat::Phase::callerBoundary||f.pc!=0x2b9d0cu||
       !f.terminated||!f.wrapperReleased||!f.wrapperReturn||f.wrapperReturn!=f.formatterReturn||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    if(!n.callIssued||n.requestedBytes!=0x1d0u||n.argumentA0!=0x1d0ull||n.callerReturnPC!=0x2b9d14u||
       n.effectiveBytes!=n.requestedBytes||n.effectiveArgument!=n.argumentA0||!n.operatorFrame||
       n.operatorSaveMask!=7u||!n.operatorFramePointer||!n.nonzeroBranch||!n.mallocFrame)return false;
    if(n.reentStop!=Stop::none){
        if(n.pc!=0x30570cu||n.mallocSaveMask||!noArena())return false;
        if(n.reentStop==Stop::unknown)return !n.reent;
        return n.reentStop==Stop::unsupportedIdentity&&n.reent&&*n.reent!=G::StdioIdentity::reent385160;
    }
    //Fixed small request: the original code has not reached3059F4's bin-index
    //delay instruction, any arena data, or ANY allocator/wrapper return.
    return n.reent==G::StdioIdentity::reent385160&&n.mallocSaveMask==1u&&n.allocatorFrame&&n.allocatorSaveMask==63u&&
        n.reentCopied&&n.sumWord==0x1e3u&&n.sumRegister==0x1e3ull&&n.minimumChunk==false&&
        n.normalizedChunk==0x1e0u&&n.normalizedRegister==0x1e0ull&&n.lockCalled&&n.lockReturned&&n.smallBin==true&&
        n.arenaBase==0x385468u&&n.arenaReadOffset==0x1ecu&&n.arenaReadAddress==0x385654u&&n.pc==0x3059ecu;
}

#include "original_source_world_water_pool_object50la.inl"
Owner::WaterPoolAllocationEntryResult Owner::enterWaterPoolAllocationNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterPoolAllocationEntryResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    const auto& f=h.nativeWaterPoolPathFormat;
    if(!f.attempted||f.phase!=SourceWaterConfig::NativeWaterPoolPathFormat::Phase::callerBoundary||f.pc!=0x2b9d0cu||
       !f.terminated||!f.wrapperReleased||!f.wrapperReturn||f.wrapperReturn!=f.formatterReturn)return result;
    auto& n=sourceWaterConfig_.nativeWaterPoolAllocationEntry;
    result.originalSourceStop=sourceWaterConfigOwner_->pc_;result.selectionStop=sourceWaterConfigOwner_->nativeArraySelection_.pc;
    result.priorArrayAllocatorStop=h.nativeWaterAllocationEntry.pc;result.formatterStop=f.pc;
    const auto finish=[&](WaterPoolAllocationEntryResult::Status status)noexcept {
        result.status=status;result.adaptedPC=n.pc;result.requestedBytes=n.requestedBytes;
        result.normalizedChunk=n.normalizedChunk;result.pendingArenaReadAddress=n.arenaReadAddress;return result;
    };
    if(n.attempted)return finish(WaterPoolAllocationEntryResult::Status::alreadyAttempted);
    n.attempted=true;n.pc=0x2b9d0c;n.callIssued=true;n.callerReturnPC=0x2b9d14;
    n.pc=0x2b9d10;n.requestedBytes=0x1d0u;n.argumentA0=0x1d0ull; //ADDIU delay, signed32->64 positive464.
    n.pc=0x2e8d90;n.operatorFrame=true; //Direct scalar entry, sp-=64. NO new[] frame or cookie.
    n.pc=0x2e8d94;n.operatorSaveMask|=1u; //SQ128 ra+20; unknown saved bits remain unknown.
    n.pc=0x2e8d98;n.operatorSaveMask|=2u; //SQ fp+10.
    n.pc=0x2e8d9c;n.operatorSaveMask|=4u; //SQ s0+0.
    n.pc=0x2e8da0;n.effectiveBytes=n.requestedBytes;n.effectiveArgument=n.argumentA0;
    n.pc=0x2e8da4;n.nonzeroBranch=*n.effectiveArgument!=0u;
    n.pc=0x2e8da8;n.operatorFramePointer=true; //Executed branch delay. Zero coercion not reached.
    n.pc=0x2e8e1c;n.pc=0x2e8e20;n.pc=0x2e8e24; //POR a0,s0; malloc call; NOP delay.
    n.pc=0x305700;n.pc=0x305704;n.pc=0x305708;n.mallocFrame=true; //16-byte frame before current reent read.
    using G=a::OriginalConfigProcessGlobals50LA;using Stop=SourceWaterConfig::NativeWaterPoolAllocationEntry::ReentStop;
    n.pc=0x30570c;
    if(!G::reentrancyCurrentNative()){n.reentStop=Stop::unknown;return finish(WaterPoolAllocationEntryResult::Status::nativeAdmissionStopped);}
    n.reent=G::state().reent38544C; //Fresh current shared owner, NOT formatter/old-array capture.
    if(*n.reent!=G::StdioIdentity::reent385160){n.reentStop=Stop::unsupportedIdentity;return finish(WaterPoolAllocationEntryResult::Status::nativeAdmissionStopped);}
    n.pc=0x305710;n.mallocSaveMask=1u; //SD64 ra+0 occurs only AFTER the admitted read.
    n.pc=0x305714;n.pc=0x305718;n.pc=0x305990;n.allocatorFrame=true; //96-byte frame, not object storage.
    n.pc=0x305994;n.sumWord=std::uint32_t(*n.effectiveBytes+19u);n.sumRegister=*n.sumWord; //Fixed positive1E3.
    n.pc=0x305998;n.allocatorSaveMask|=1u;
    n.pc=0x30599c;n.minimumChunk=*n.sumRegister<31u; //SLTIU false for this exact request.
    n.pc=0x3059a0;n.allocatorSaveMask|=2u;n.pc=0x3059a4;n.reentCopied=true;
    n.pc=0x3059a8;n.allocatorSaveMask|=4u;n.pc=0x3059ac;n.allocatorSaveMask|=8u;
    n.pc=0x3059b0;n.allocatorSaveMask|=16u;n.pc=0x3059b4;
    n.pc=0x3059b8;n.allocatorSaveMask|=32u; //SD s0+0 in branch delay; minimum arm is not reached.
    n.pc=0x3059bc;n.pc=0x3059c0;n.pc=0x3059c4;n.pc=0x3059c8;
    n.normalizedChunk=*n.sumWord&0xfffffff0u;n.normalizedRegister=*n.normalizedChunk; //AND delay ->positive1E0.
    n.pc=0x3059d0;n.lockCalled=true;n.pc=0x3059d4;
    n.pc=0x306538;n.pc=0x30653c;n.lockReturned=true; //Exact JR/NOP, not a native mutex or arena receipt.
    n.pc=0x3059d8;n.smallBin=*n.normalizedRegister<504u; //True; no large-bin branch.
    n.pc=0x3059dc;n.pc=0x3059e0; //LUI in non-taken branch delay.
    n.pc=0x3059e4;n.arenaBase=0x385468u;
    n.pc=0x3059e8;n.arenaReadOffset=*n.normalizedChunk+12u;n.arenaReadAddress=*n.arenaBase+*n.arenaReadOffset;
    n.pc=0x3059ec; //BEFORE LW. Address arithmetic is known; current arena contents are not.
    return finish(WaterPoolAllocationEntryResult::Status::arenaBoundary);
}
