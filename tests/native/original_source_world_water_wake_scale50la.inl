bool Owner::preflightNativeWakeScale()const noexcept {
    if(!preflightNativeWaterArrayPrefix())return false;
    const auto& n=sourceWaterConfig_.nativeWakeScale;const auto& p=sourceWaterConfig_.nativeWakePacked;
    const auto noProduct=[&]()noexcept {
        return !n.numericAdmitted&&!n.product&&!n.localClearMask&&!n.stored&&!n.storePublication&&
            !n.signatureWaves&&!n.skippedSignatureWaves;
    };
    if(!n.attempted)return n.pc==0x2b9888u&&!n.input&&!n.inputPublication&&!n.inputWasAdapted&&!n.f1Bits&&noProduct();
    if(!p.stored||p.pc!=0x2b9888u||p.f1Bits!=0x41800000u||!sourceWaterConfigOwner_||
       sourceWaterConfigOwner_->pc_!=0x30492cu||sourceWaterConfigOwner_->wakeSelection_.returned||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    if(!n.input)return !n.inputPublication&&!n.inputWasAdapted&&!n.f1Bits&&noProduct()&&n.pc==0x2b988cu;
    if(!n.inputPublication||!*n.inputPublication||!n.inputWasAdapted||n.f1Bits!=p.f1Bits)return false;
    const auto exponent=(*n.input>>23u)&0xffu;
    const bool admitted=!*n.input||(exponent>=1u&&exponent<=250u);
    if(!n.numericAdmitted){if(admitted||!noProduct()||n.pc!=0x2b9890u)return false;}
    else{
        const auto product=*n.input?((*n.input&0x807fffffu)|((exponent+4u)<<23u)):0u;
        if(!admitted||n.product!=product||n.localClearMask!=0x8000u)return false;
        if(!n.stored){
            if(n.storePublication||n.signatureWaves||n.skippedSignatureWaves||n.pc!=0x2b9898u)return false;
        }else{
            if(!n.storePublication||*n.inputPublication==std::numeric_limits<std::uint64_t>::max()||
               *n.storePublication!=*n.inputPublication+1u)return false;
            if(!n.signatureWaves){if(n.skippedSignatureWaves||n.pc!=0x2b989cu)return false;}
            else if(!n.skippedSignatureWaves||*n.skippedSignatureWaves!=(*n.signatureWaves==0u)||
                n.pc!=(*n.skippedSignatureWaves?0x2b9c40u:0x2b98a8u))return false;
        }
    }
    if(!sourceWaterProvider274720_.nativeWithdrawn){
        const auto& current=worldApplication_->sourceWaterProcessScalars_;
        const auto version=n.stored?*n.storePublication:*n.inputPublication;
        if(current.publications[4]<version)return false;
        if(current.publications[4]==version&&!current.unknown){
            if(current.words[4]!=(n.stored?n.product:n.input)||
               current.nativeAdapted[4]!=(n.stored?true:*n.inputWasAdapted))return false;
        }
        if(n.signatureWaves&&sourceWaterProvider_->signatureWaves6C!=n.signatureWaves)return false;
    }
    return true;
}
Owner::WaterWakeScaleResult Owner::scaleWakeRangeNativeOnly(WaterWakeLoadMode mode)noexcept {
    WaterWakeScaleResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterWakeLoadMode::nativeRetiredBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& n=sourceWaterConfig_.nativeWakeScale;const auto& p=sourceWaterConfig_.nativeWakePacked;
    result.originalSourceStop=sourceWaterConfigOwner_->pc_;
    if(!p.stored||p.pc!=0x2b9888u||p.f1Bits!=0x41800000u)return result;
    const auto finish=[&](WaterWakeScaleResult::Status status)noexcept {
        result.status=status;result.adaptedPC=n.pc;result.stored=n.stored;
        result.skippedSignatureWaves=n.skippedSignatureWaves;return result;
    };
    if(n.attempted)return finish(WaterWakeScaleResult::Status::alreadyAttempted);
    n.attempted=true;n.pc=0x2b9888;n.pc=0x2b988c;
    std::uint32_t input=0;std::uint64_t publication=0;
    if(!worldApplication_->waterAdaptedScalarReadNative(4u,input,publication))
        return finish(WaterWakeScaleResult::Status::nativeAdmissionStopped);
    auto& current=worldApplication_->sourceWaterProcessScalars_;
    n.input=input;n.inputPublication=publication;n.inputWasAdapted=current.nativeAdapted[4];
    n.pc=0x2b9890;n.f1Bits=p.f1Bits;
    std::uint32_t product=0,clearMask=0;
    if(!g::detail::sourceWaterScale16Native50LA(input,*n.f1Bits,product,clearMask))
        return finish(WaterWakeScaleResult::Status::nativeAdmissionStopped);
    n.numericAdmitted=true;n.product=product;n.localClearMask=clearMask;
    n.pc=0x2b9894;n.pc=0x2b9898;
    if(!worldApplication_->waterAdaptedLoadScalarAdmittedNative(4u))
        return finish(WaterWakeScaleResult::Status::nativeAdmissionStopped);
    current.words[4]=n.product;current.nativeAdapted[4]=true;++current.publications[4];
    n.storePublication=current.publications[4];n.stored=true;
    n.pc=0x2b989c;
    if(!sourceWaterProvider_->signatureWaves6C)return finish(WaterWakeScaleResult::Status::nativeAdmissionStopped);
    n.signatureWaves=sourceWaterProvider_->signatureWaves6C; //CURRENT same Water+6C, not saved getter result.
    n.pc=0x2b98a0;n.skippedSignatureWaves=*n.signatureWaves==0u;
    n.pc=0x2b98a4; //Actual NOP delay on both arms, including negative/high-bit nonzero.
    n.pc=*n.skippedSignatureWaves?0x2b9c40u:0x2b98a8u;
    return finish(WaterWakeScaleResult::Status::nativeContinued); //Before either target body.
}

#include "original_source_world_water_array_prefix50la.inl"
