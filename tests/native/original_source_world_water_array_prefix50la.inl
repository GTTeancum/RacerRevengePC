bool Owner::preflightNativeWaterArrayPrefix()const noexcept {
    if(!preflightNativeWaterAllocationEntry())return false;
    const auto& n=sourceWaterConfig_.nativeWaterArrayPrefix;const auto& s=sourceWaterConfig_.nativeWakeScale;
    const auto noStack=[&]()noexcept {
        if(n.stackWrites)return false;
        for(const auto& v:n.stack820)if(v)return false;
        for(const auto& v:n.stack830)if(v)return false;
        for(const auto& v:n.initialized820)if(v)return false;
        for(const auto& v:n.initialized830)if(v)return false;
        return true;
    };
    const auto noRequest=[&]()noexcept {
        return !n.callIssued&&!n.countPlusOne&&!n.requestedBytes&&!n.argumentA0;
    };
    if(!n.attempted)return n.pc==0u&&!n.signatureArm&&!n.skippedPools&&!n.countWord&&noStack()&&noRequest();
    if(!s.stored||!s.skippedSignatureWaves||!n.signatureArm||
       *n.signatureArm==*s.skippedSignatureWaves||
       s.pc!=(*n.signatureArm?0x2b98a8u:0x2b9c40u)||
       !sourceWaterConfigOwner_||sourceWaterConfigOwner_->pc_!=0x30492cu||
       sourceWaterConfigOwner_->wakeSelection_.returned||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    //These provider words have no admitted intervening writer yet. Keep their
    //custody check without replacing the distinct original CURRENT reads.
    if(n.countWord&&!sourceWaterProvider274720_.nativeWithdrawn&&
       (*n.signatureArm?sourceWaterProvider_->signatureWaves6C:sourceWaterProvider_->numPools64)!=n.countWord)return false;
    if(*n.signatureArm){
        if(n.stackWrites!=8u||n.skippedPools)return false;
        for(std::size_t i=0;i<4u;++i){
            if(n.initialized820[i]!=(i==3u?0x3f800000u:0u)||
               n.initialized830[i]!=(i>=2u?0x3f800000u:0u)||n.stack830[i]!=n.initialized830[i])return false;
            const auto& values=sourceWaterConfigOwner_->nativeArrayScalars_;
            const auto expected=i<3u&&values.entered&&values.signature&&values.floats[i].result?
                values.floats[i].result:n.initialized820[i];
            if(n.stack820[i]!=expected)return false;
        }
        if(!n.countWord)return n.pc==0x2b98ccu&&noRequest();
    }else{
        if(!noStack())return false;
        if(!n.countWord)return !n.skippedPools&&n.pc==0x2b9c40u&&noRequest();
        if(!n.skippedPools||*n.skippedPools!=(*n.countWord==0u))return false;
        if(*n.skippedPools)return n.pc==0x2b9d98u&&noRequest();
    }
    const auto plus=std::uint32_t(*n.countWord+1u);
    const auto bytes=std::uint32_t(plus<<2u);
    const auto argument=std::uint64_t(bytes)|((bytes&0x80000000u)?0xffffffff00000000ull:0ull);
    if(!n.callIssued||n.countPlusOne!=plus||n.requestedBytes!=bytes||n.argumentA0!=argument||n.pc!=0x2e8ca0u)return false;
    return true;
}
Owner::WaterArrayPrefixResult Owner::prepareWaterArraysNativeOnly(WaterWakeLoadMode mode)noexcept {
    WaterArrayPrefixResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterWakeLoadMode::nativeRetiredBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& n=sourceWaterConfig_.nativeWaterArrayPrefix;const auto& s=sourceWaterConfig_.nativeWakeScale;
    result.originalSourceStop=sourceWaterConfigOwner_->pc_;
    if(!s.stored||!s.skippedSignatureWaves||s.pc!=(*s.skippedSignatureWaves?0x2b9c40u:0x2b98a8u))return result;
    const auto finish=[&](WaterArrayPrefixResult::Status status)noexcept {
        result.status=status;result.adaptedPC=n.pc;result.requestedBytes=n.requestedBytes;
        result.signatureArm=n.signatureArm;result.skippedPools=n.skippedPools;
        result.callIssued=n.callIssued;return result;
    };
    if(n.attempted)return finish(WaterArrayPrefixResult::Status::alreadyAttempted);
    n.attempted=true;n.signatureArm=!*s.skippedSignatureWaves;n.pc=s.pc;
    if(*n.signatureArm){
        //Actual caller-stack stores in source order. Adjacent stack is unknown.
        n.stack820[0]=0u;n.initialized820[0]=n.stack820[0];++n.stackWrites;n.pc=0x2b98ac;
        n.stack830[0]=0u;n.initialized830[0]=n.stack830[0];++n.stackWrites;n.pc=0x2b98b0;
        n.stack820[1]=0u;n.initialized820[1]=n.stack820[1];++n.stackWrites;n.pc=0x2b98b4;
        n.stack830[1]=0u;n.initialized830[1]=n.stack830[1];++n.stackWrites;n.pc=0x2b98b8;
        const std::uint32_t one=0x3f800000u;n.pc=0x2b98bc;
        n.stack820[2]=0u;n.initialized820[2]=n.stack820[2];++n.stackWrites;n.pc=0x2b98c0;
        n.stack830[2]=one;n.initialized830[2]=n.stack830[2];++n.stackWrites;n.pc=0x2b98c4;
        n.stack820[3]=one;n.initialized820[3]=n.stack820[3];++n.stackWrites;n.pc=0x2b98c8;
        n.stack830[3]=one;n.initialized830[3]=n.stack830[3];++n.stackWrites;n.pc=0x2b98cc;
        if(!sourceWaterProvider_->signatureWaves6C)return finish(WaterArrayPrefixResult::Status::nativeAdmissionStopped);
        n.countWord=sourceWaterProvider_->signatureWaves6C; //Fresh +6C read, not prior branch/getter snapshot.
        n.pc=0x2b98d0;n.countPlusOne=std::uint32_t(*n.countWord+1u);
        n.pc=0x2b98d4;n.callIssued=true;
        n.pc=0x2b98d8; //SLL delay slot, including low-word wrap and sign extension.
    }else{
        n.pc=0x2b9c40;
        if(!sourceWaterProvider_->numPools64)return finish(WaterArrayPrefixResult::Status::nativeAdmissionStopped);
        n.countWord=sourceWaterProvider_->numPools64; //CURRENT +64, no extra reread before this request.
        n.pc=0x2b9c44;n.skippedPools=*n.countWord==0u;
        n.pc=0x2b9c48; //Actual NOP on both arms.
        if(*n.skippedPools){n.pc=0x2b9d98;return finish(WaterArrayPrefixResult::Status::nativeContinued);}
        n.pc=0x2b9c4c;n.countPlusOne=std::uint32_t(*n.countWord+1u);
        n.pc=0x2b9c50;n.callIssued=true;
        n.pc=0x2b9c54;
    }
    const auto bytes=std::uint32_t(*n.countPlusOne<<2u);
    n.requestedBytes=bytes;
    n.argumentA0=std::uint64_t(bytes)|((bytes&0x80000000u)?0xffffffff00000000ull:0ull);
    n.pc=0x2e8ca0; //Callee entry only: no new[] body, malloc, result, or pointer publication.
    return finish(WaterArrayPrefixResult::Status::nativeContinued);
}

#include "original_source_world_water_allocation_entry50la.inl"
