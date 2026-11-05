bool Owner::preflightNativeWaterAllocationEntry()const noexcept {
    if(!preflightNativeWaterArrayBacking())return false;
    const auto& n=sourceWaterConfig_.nativeWaterAllocationEntry;
    const auto& p=sourceWaterConfig_.nativeWaterArrayPrefix;
    using G=a::OriginalConfigProcessGlobals50LA;using Stop=SourceWaterConfig::NativeWaterAllocationEntry::ReentStop;
    const auto sx=[](std::uint32_t word)noexcept {
        return std::uint64_t(word)|((word&0x80000000u)?0xffffffff00000000ull:0ull);
    };
    const auto noZones=[&]()noexcept {
        return !n.zonesCallIssued&&!n.zoneCount&&!n.zoneCountRegister&&!n.zoneTimes8&&!n.zoneTimes9&&!n.zoneTimes36;
    };
    const auto noArenaPrefix=[&]()noexcept {
        return !n.allocatorFrame&&!n.allocatorSaveMask&&!n.reentCopied&&!n.lockCalled&&!n.lockReturned&&
            !n.sumWord&&!n.sumRegister&&!n.normalizedChunk&&!n.normalizedRegister&&
            !n.minimumChunk&&!n.smallBin&&!n.binIndex&&!n.arenaReadOffset&&!n.pendingBranchTarget;
    };
    const auto noWrapper=[&]()noexcept {
        return !n.requestedBytes&&!n.callerReturnPC&&!n.argumentA0&&!n.effectiveBytes&&!n.effectiveArgument&&
            !n.arrayFrame&&!n.arraySaveMask&&!n.operatorFrame&&!n.operatorSaveMask&&!n.operatorFramePointer&&
            !n.mallocFrame&&!n.mallocSaveMask&&!n.zeroCoerced&&!n.reent&&n.reentStop==Stop::none&&noArenaPrefix();
    };
    if(!n.attempted)return !n.zones&&n.pc==0u&&noZones()&&noWrapper();
    if(!p.attempted||!sourceWaterConfigOwner_||sourceWaterConfigOwner_->pc_!=0x30492cu||
       sourceWaterConfigOwner_->wakeSelection_.returned||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    if(n.zones){
        if(p.callIssued||p.pc!=0x2b9d98u||p.skippedPools!=true)return false;
        if(!n.zoneCount)return n.pc==0x2b9d98u&&noZones()&&noWrapper();
        const auto t8=std::uint32_t(*n.zoneCount<<3u);
        const auto t9=std::uint32_t(t8+*n.zoneCount);
        const auto t36=std::uint32_t(t9<<2u);
        if(!n.zonesCallIssued||n.zoneCountRegister!=sx(*n.zoneCount)||n.zoneTimes8!=t8||
           n.zoneTimes9!=t9||n.zoneTimes36!=t36||n.requestedBytes!=std::uint32_t(t36+16u)||
           n.callerReturnPC!=0x2b9db0u)return false;
        if(!sourceWaterProvider274720_.nativeWithdrawn&&sourceWaterProvider_->numZones4A8!=n.zoneCount)return false;
    }else{
        if(!noZones()||!p.callIssued||p.pc!=0x2e8ca0u||!p.signatureArm||!p.requestedBytes||!p.argumentA0||
           n.requestedBytes!=p.requestedBytes||n.argumentA0!=p.argumentA0||
           n.callerReturnPC!=(*p.signatureArm?0x2b98dcu:0x2b9c58u))return false;
    }
    if(!n.requestedBytes||n.argumentA0!=sx(*n.requestedBytes)||!n.arrayFrame||n.arraySaveMask!=1u||
       !n.operatorFrame||n.operatorSaveMask!=7u||!n.operatorFramePointer||!n.mallocFrame||
       n.zeroCoerced!=(*n.requestedBytes==0u)||n.effectiveBytes!=(*n.requestedBytes?*n.requestedBytes:1u)||
       !n.effectiveBytes||n.effectiveArgument!=sx(*n.effectiveBytes))return false;
    if(n.reentStop!=Stop::none){
        if(n.pc!=0x30570cu||n.mallocSaveMask||!noArenaPrefix())return false;
        if(n.reentStop==Stop::unknown)return !n.reent;
        return n.reentStop==Stop::unsupportedIdentity&&n.reent&&*n.reent!=G::StdioIdentity::reent385160;
    }
    if(n.reent!=G::StdioIdentity::reent385160||n.mallocSaveMask!=1u||!n.allocatorFrame||
       n.allocatorSaveMask!=63u||!n.reentCopied||!n.lockCalled||!n.lockReturned)return false;
    const auto sum=std::uint32_t(*n.effectiveBytes+19u);
    const bool minimum=sx(sum)<31u;
    const auto chunk=minimum?16u:(sum&0xfffffff0u);
    const bool small=sx(chunk)<504u;
    if(n.sumWord!=sum||n.sumRegister!=sx(sum)||n.minimumChunk!=minimum||
       n.normalizedChunk!=chunk||n.normalizedRegister!=sx(chunk)||n.smallBin!=small)return false;
    if(small)return !n.binIndex&&!n.pendingBranchTarget&&n.arenaReadOffset==chunk+12u&&n.pc==0x3059ecu;
    const auto q=chunk>>9u;
    const auto index=q==0u?(chunk>>3u):q<5u?(chunk>>6u)+56u:q<21u?q+91u:
        q<85u?(chunk>>12u)+110u:q<341u?(chunk>>15u)+119u:q<1365u?(chunk>>18u)+124u:126u;
    return n.binIndex==index&&n.arenaReadOffset==(index<<3u)+12u&&
        n.pendingBranchTarget==0x305ad4u&&n.pc==0x305ac4u;
}
Owner::WaterAllocationEntryResult Owner::enterWaterAllocationNativeOnly(WaterWakeLoadMode mode)noexcept {
    WaterAllocationEntryResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterWakeLoadMode::nativeRetiredBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& n=sourceWaterConfig_.nativeWaterAllocationEntry;const auto& p=sourceWaterConfig_.nativeWaterArrayPrefix;
    result.originalSourceStop=sourceWaterConfigOwner_->pc_;
    const bool request=p.callIssued&&p.pc==0x2e8ca0u&&p.requestedBytes&&p.argumentA0&&p.signatureArm;
    const bool zones=!p.callIssued&&p.pc==0x2b9d98u&&p.skippedPools==true;
    if(!p.attempted||(!request&&!zones))return result;
    const auto finish=[&](WaterAllocationEntryResult::Status status)noexcept {
        result.status=status;result.adaptedPC=n.pc;result.requestedBytes=n.requestedBytes;
        result.effectiveBytes=n.effectiveBytes;result.arenaReadOffset=n.arenaReadOffset;
        result.pendingBranchTarget=n.pendingBranchTarget;result.zones=n.zones;return result;
    };
    if(n.attempted)return finish(WaterAllocationEntryResult::Status::alreadyAttempted);
    const auto sx=[](std::uint32_t word)noexcept {
        return std::uint64_t(word)|((word&0x80000000u)?0xffffffff00000000ull:0ull);
    };
    n.attempted=true;n.zones=zones;n.pc=p.pc;
    if(zones){
        if(!sourceWaterProvider_->numZones4A8)return finish(WaterAllocationEntryResult::Status::nativeAdmissionStopped);
        n.zoneCount=sourceWaterProvider_->numZones4A8;n.zoneCountRegister=sx(*n.zoneCount); //Fresh LW at2B9D98.
        n.pc=0x2b9d9c;n.zoneTimes8=std::uint32_t(*n.zoneCount<<3u);
        n.pc=0x2b9da0;n.zoneTimes9=std::uint32_t(*n.zoneTimes8+*n.zoneCount);
        n.pc=0x2b9da4;n.zoneTimes36=std::uint32_t(*n.zoneTimes9<<2u);
        n.pc=0x2b9da8;n.zonesCallIssued=true;n.callerReturnPC=0x2b9db0;
        n.pc=0x2b9dac;n.requestedBytes=std::uint32_t(*n.zoneTimes36+16u);n.argumentA0=sx(*n.requestedBytes);
    }else{
        n.requestedBytes=p.requestedBytes;n.argumentA0=p.argumentA0;
        n.callerReturnPC=*p.signatureArm?0x2b98dcu:0x2b9c58u;
    }
    //Symbolic caller-stack frame/save roles only. Saved unknown register bits
    //are not initialized; SQ128 and SD64 remain distinct from payload stores.
    n.pc=0x2e8ca0;n.arrayFrame=true; //sp-=16.
    n.pc=0x2e8ca4;n.arraySaveMask=1u; //SQ ra at+0.
    n.pc=0x2e8ca8;n.pc=0x2e8cac;n.pc=0x2e8d90;n.operatorFrame=true; //sp-=64.
    n.pc=0x2e8d94;n.operatorSaveMask|=1u; //SQ ra+20.
    n.pc=0x2e8d98;n.operatorSaveMask|=2u; //SQ fp+10.
    n.pc=0x2e8d9c;n.operatorSaveMask|=4u; //SQ s0+0.
    n.pc=0x2e8da0;n.effectiveBytes=n.requestedBytes;n.effectiveArgument=n.argumentA0;
    n.pc=0x2e8da4;n.zeroCoerced=*n.effectiveArgument==0u;
    n.pc=0x2e8da8;n.operatorFramePointer=true; //ADDU fp,sp,zero in branch delay.
    if(n.zeroCoerced){n.pc=0x2e8dac;n.pc=0x2e8db0;n.effectiveBytes=1u;n.effectiveArgument=1u;}
    n.pc=0x2e8e1c; //POR a0,s0,zero: full effective argument preserved.
    n.pc=0x2e8e20;n.pc=0x2e8e24;n.pc=0x305700; //DADDU a1,a0,zero.
    n.pc=0x305704;n.pc=0x305708;n.mallocFrame=true; //sp-=16 before current reent read.
    using G=a::OriginalConfigProcessGlobals50LA;using Stop=SourceWaterConfig::NativeWaterAllocationEntry::ReentStop;
    n.pc=0x30570c;
    if(!G::reentrancyCurrentNative()){n.reentStop=Stop::unknown;return finish(WaterAllocationEntryResult::Status::nativeAdmissionStopped);}
    n.reent=G::state().reent38544C; //Fresh current owner, not previous Config/free read.
    if(*n.reent!=G::StdioIdentity::reent385160){n.reentStop=Stop::unsupportedIdentity;return finish(WaterAllocationEntryResult::Status::nativeAdmissionStopped);}
    n.pc=0x305710;n.mallocSaveMask=1u; //SD ra+0,64-bit.
    n.pc=0x305714;n.pc=0x305718;n.pc=0x305990;n.allocatorFrame=true; //sp-=96.
    n.pc=0x305994;n.sumWord=std::uint32_t(*n.effectiveBytes+19u);n.sumRegister=sx(*n.sumWord);
    n.pc=0x305998;n.allocatorSaveMask|=1u; //SD s3+30.
    n.pc=0x30599c;n.minimumChunk=*n.sumRegister<31u; //Unsigned64 SLTIU after ADDIU sign extension.
    n.pc=0x3059a0;n.allocatorSaveMask|=2u; //SD ra+50.
    n.pc=0x3059a4;n.reentCopied=true; //DADDU s3,a0,zero.
    n.pc=0x3059a8;n.allocatorSaveMask|=4u; //SD s4+40.
    n.pc=0x3059ac;n.allocatorSaveMask|=8u; //SD s2+20.
    n.pc=0x3059b0;n.allocatorSaveMask|=16u; //SD s1+10.
    n.pc=0x3059b4;n.pc=0x3059b8;n.allocatorSaveMask|=32u; //SD s0+0 in branch delay.
    if(*n.minimumChunk){n.pc=0x3059cc;n.normalizedChunk=16u;n.normalizedRegister=16u;}
    else{
        n.pc=0x3059bc;n.pc=0x3059c0;n.pc=0x3059c4;n.pc=0x3059c8;
        n.normalizedChunk=*n.sumWord&0xfffffff0u;n.normalizedRegister=sx(*n.normalizedChunk);
    }
    n.pc=0x3059d0;n.lockCalled=true;n.pc=0x3059d4; //Argument is retained symbolic reent.
    n.pc=0x306538;n.pc=0x30653c; //Verified JR ra / NOP, no real native mutex or arena custody.
    n.lockReturned=true;n.pc=0x3059d8;n.smallBin=*n.normalizedRegister<504u;
    n.pc=0x3059dc;n.pc=0x3059e0; //LUI arena base in branch delay, not a heap read.
    const auto chunk=*n.normalizedChunk;
    if(*n.smallBin){
        n.pc=0x3059e4;n.pc=0x3059e8;n.arenaReadOffset=chunk+12u;
        n.pc=0x3059ec; //BEFORE current arena backward-link read.
    }else{
        n.pc=0x305a38;const auto q=chunk>>9u;
        n.pc=0x305a3c;n.pc=0x305a40;
        if(q==0u){n.pc=0x305a44;n.pc=0x305a48;n.binIndex=chunk>>3u;}
        else{
            n.pc=0x305a4c;n.pc=0x305a50;
            if(q<5u){n.pc=0x305a54;n.pc=0x305a58;n.binIndex=(chunk>>6u)+56u;}
            else{
                n.pc=0x305a5c;n.pc=0x305a60;n.pc=0x305a64;n.binIndex=q+91u;
                if(q>=21u){
                    n.pc=0x305a68;n.pc=0x305a6c;n.pc=0x305a70;
                    if(q<85u){n.pc=0x305a74;n.pc=0x305a78;n.binIndex=(chunk>>12u)+110u;}
                    else{
                        n.pc=0x305a7c;n.pc=0x305a80;n.pc=0x305a84;
                        if(q<341u){n.pc=0x305a88;n.pc=0x305a8c;n.binIndex=(chunk>>15u)+119u;}
                        else{
                            n.pc=0x305a90;n.pc=0x305a94;n.pc=0x305a98;
                            if(q<1365u){n.pc=0x305a9c;n.pc=0x305aa0;n.binIndex=(chunk>>18u)+124u;}
                            else{n.pc=0x305aa4;n.binIndex=126u;}
                        }
                    }
                }
            }
        }
        n.pc=0x305aa8;n.pc=0x305aac;n.pc=0x305ab0;n.pc=0x305ab4;n.pc=0x305ab8;
        n.arenaReadOffset=(*n.binIndex<<3u)+12u;
        n.pc=0x305abc;n.pc=0x305ac0;n.pendingBranchTarget=0x305ad4u;
        n.pc=0x305ac4; //Unexecuted arena read is the pending branch delay, not completed transfer.
    }
    return finish(WaterAllocationEntryResult::Status::arenaBoundary);
}

#include "original_source_world_water_array_backing50la.inl"
