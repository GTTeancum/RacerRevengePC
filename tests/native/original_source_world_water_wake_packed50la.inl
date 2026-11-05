bool Owner::preflightNativeWakePacked()const noexcept {
    if(!preflightNativeWakeScale())return false;
    const auto& p=sourceWaterConfig_.nativeWakePacked;
    const auto noStore=[&]()noexcept {
        return !p.stored&&!p.f1Bits&&!p.signExtended&&!p.mask&&!p.shifted&&!p.packed&&
            (!sourceWaterProvider_||!sourceWaterProvider_->nativePackedState2A0);
    };
    if(!p.attempted)return p.pc==0x2b9864u&&!p.alphaWord&&!p.alphaPublication&&noStore();
    if(!sourceWaterConfigOwner_||sourceWaterConfigOwner_->pc_!=0x30492cu||
       !sourceWaterConfig_.wakeSelectionEntered||!sourceWaterConfigOwner_->nativeWakeLoad_.nativeLoaded||
       sourceWaterConfigOwner_->nativeWakeGetters_.completed!=5u||
       sourceWaterConfigOwner_->nativeWakeGetters_.pc!=0x2b9864u||sourceWaterConfigOwner_->wakeSelection_.returned||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    if(!p.alphaWord)return !p.alphaPublication&&p.pc==0x2b9868u&&noStore();
    if(!p.alphaPublication||!*p.alphaPublication||!p.stored||p.pc!=0x2b9888u||
       p.f1Bits!=0x41800000u||p.mask!=0x5000dULL)return false;
    const auto extended=std::uint64_t(*p.alphaWord)|((*p.alphaWord&0x80000000u)?0xffffffff00000000ULL:0ULL);
    if(p.signExtended!=extended||p.shifted!=(extended<<4u)||p.packed!=((extended<<4u)|0x5000dULL))return false;
    if(!sourceWaterProvider274720_.nativeWithdrawn){
        if(sourceWaterProvider_->nativePackedState2A0!=p.packed)return false;
        const auto& current=worldApplication_->sourceWaterProcessScalars_;
        if(current.publications[31]<*p.alphaPublication)return false;
        if(current.publications[31]==*p.alphaPublication&&!current.unknown&&
           (!current.nativeAdapted[31]||current.words[31]!=p.alphaWord))return false;
    }
    return true;
}
Owner::WaterWakePackedResult Owner::packWakeStateNativeOnly(WaterWakeLoadMode mode)noexcept {
    WaterWakePackedResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterWakeLoadMode::nativeRetiredBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    const auto& config=*sourceWaterConfigOwner_;auto& p=sourceWaterConfig_.nativeWakePacked;
    result.originalSourceStop=config.pc_;
    if(config.nativeWakeGetters_.completed!=5u||config.nativeWakeGetters_.pc!=0x2b9864u)return result;
    if(p.attempted){
        result.status=WaterWakePackedResult::Status::alreadyAttempted;
        result.adaptedPC=p.pc;result.stored=p.stored;return result;
    }
    p.attempted=true;p.pc=0x2b9864;p.pc=0x2b9868;
    std::uint32_t alpha=0;std::uint64_t publication=0;
    if(!worldApplication_->waterAdaptedScalarReadNative(31u,alpha,publication)){
        result.status=WaterWakePackedResult::Status::nativeAdmissionStopped;result.adaptedPC=p.pc;return result;
    }
    //CURRENT383540, not the earlier AlphaReference getter's saved result.
    p.alphaWord=alpha;p.alphaPublication=publication;
    p.signExtended=std::uint64_t(alpha)|((alpha&0x80000000u)?0xffffffff00000000ULL:0ULL);
    p.pc=0x2b986c;p.pc=0x2b9870;p.mask=0x5000dULL;
    p.pc=0x2b9874;p.pc=0x2b9878;p.f1Bits=0x41800000u;
    //This is loader F1, not the earlier constructor's Members.rawF1 receipt.
    //Unsigned64 bit operations express sign-extended LW then modulo64 DSLL;
    //no signed-shift UB, clamp, 32-bit truncation or float conversion.
    p.pc=0x2b987c;p.shifted=*p.signExtended<<4u;
    p.pc=0x2b9880;p.packed=*p.shifted|*p.mask;
    p.pc=0x2b9884;sourceWaterProvider_->nativePackedState2A0=p.packed;p.stored=true;
    p.pc=0x2b9888;result.status=WaterWakePackedResult::Status::nativeStored;
    result.adaptedPC=p.pc;result.stored=true;return result;
}

#include "original_source_world_water_wake_scale50la.inl"
