//Second selection lookup/seek and genuine first free's supported prefix only.
bool Owner::preflightSourceWaterWakeNative()const noexcept {
    if(!preflightNativeWakeGetters())return false;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(!h.wakeSelectionEntered)return !sourceWaterConfigOwner_||!sourceWaterConfigOwner_->wakeSelection_.attempted;
    if(!h.alphaTailEntered||!h.wakeFrothCallIssued||!h.waterVectorCopied[1]||!sourceWaterConfigOwner_||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    const auto& config=*sourceWaterConfigOwner_;const auto& w=config.wakeSelection_;
    if(!w.attempted)return false;
    if(w.returned)return *w.returned==false&&r.pc==0x2b97bcu; //Caller ignores actual missing return.
    return r.pc==config.pc_;
}
bool Owner::continueSourceWaterWake157D00(std::uint32_t& pc)noexcept {
    auto& h=sourceWaterConfig_;auto& r=sourceWaterProviderInitializer_;
    if(pc!=0x157d00u||r.pc!=pc||sourcePostParticleModelsPC_!=pc||h.wakeSelectionEntered||h.nativeWithdrawn||
       !h.wakeFrothCallIssued||!sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||nativeScbThread_!=std::this_thread::get_id()||
       !preflightSourceWaterConfigNative())return false;
    const auto at=[&](std::uint32_t value)noexcept {
        pc=sourcePostParticleModelsPC_=sourceWaterProvider274720_.pc=sourceWaterProviderNumeric_.pc=r.pc=value;
    };
    h.wakeSelectionEntered=true;a::OriginalStandardTextureDiagnostic50LA d;
    if(!sourceWaterConfigOwner_->selectWakeFroth157D00(d)){at(sourceWaterConfigOwner_->pc_);return false;}
    at(0x2b97bc);return false; //Actual missing selection returned; MinForce has not run.
}

Owner::WaterBackingRetirementResult Owner::retireWaterBackingNativeOnly(WaterBackingRetirementMode mode)noexcept {
    WaterBackingRetirementResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterBackingRetirementMode::nativeSuccessBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& config=*sourceWaterConfigOwner_;
    //One-shot adapter, not a retry/replay of original malloc/free or loading.
    //A repeated request reports retained native effects without touching them.
    if(config.nativeWakeRetirement_.attempted){
        result.status=WaterBackingRetirementResult::Status::alreadyAttempted;
        result.dataRetired=config.nativeWakeRetirement_.dataRetired;
        result.tableRetired=config.nativeWakeRetirement_.tableRetired;
        result.originalSourceStop=config.pc_;
        result.adaptedResumePC=config.nativeWakeRetirement_.adaptedResumePC;
        return result;
    }
    const bool retired=config.retireWakeBackingNativeOnly();
    const auto& n=config.nativeWakeRetirement_;
    result.status=retired?WaterBackingRetirementResult::Status::nativeRetired:
        (n.attempted?WaterBackingRetirementResult::Status::nativeAdmissionStopped:WaterBackingRetirementResult::Status::refused);
    result.dataRetired=n.dataRetired;result.tableRetired=n.tableRetired;
    result.originalSourceStop=config.pc_;result.adaptedResumePC=n.adaptedResumePC;
    //Neither Config nor any aggregate source-PC cursor advances across30492C.
    //The separate adaptedResumePC is NOT source selection/Water/GPU readiness.
    return result;
}

#include "original_source_world_water_wake_getters50la.inl"

Owner::WaterWakeLoadResult Owner::loadWakeFrothNativeOnly(WaterWakeLoadMode mode)noexcept {
    WaterWakeLoadResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterWakeLoadMode::nativeRetiredBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& config=*sourceWaterConfigOwner_;
    result.originalSourceStop=config.pc_;
    if(config.nativeWakeLoad_.attempted){
        result.status=WaterWakeLoadResult::Status::alreadyAttempted;
        result.adaptedPC=config.nativeWakeLoad_.pc;result.nativeLoaded=config.nativeWakeLoad_.nativeLoaded;
        return result;
    }
    a::OriginalStandardTextureDiagnostic50LA diagnostic;
    const bool loaded=config.loadWakeFrothNativeOnly(diagnostic);const auto& n=config.nativeWakeLoad_;
    result.status=loaded?WaterWakeLoadResult::Status::nativeLoaded:
        (n.attempted?WaterWakeLoadResult::Status::nativeAdmissionStopped:WaterWakeLoadResult::Status::refused);
    if(n.attempted)result.adaptedPC=n.pc;result.nativeLoaded=n.nativeLoaded;
    //No source-PC updates, original return, MinForce getter or new borrower.
    return result;
}
