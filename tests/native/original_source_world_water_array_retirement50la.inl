bool Owner::preflightNativeWaterArrayRetirement()const noexcept {
    if(!preflightNativeWaterArrayLoad())return false;
    return !sourceWaterConfigOwner_||sourceWaterConfigOwner_->preflightArrayRetirementNative();
}
Owner::WaterArrayRetirementResult Owner::retireWaterArraySectionNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterArrayRetirementResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& config=*sourceWaterConfigOwner_;const auto& s=config.nativeArraySelection_;
    const auto& n=config.nativeArrayRetirement_;
    if(!s.attempted||!s.heapHeaderBoundary||s.returned||s.pc!=0x30492cu)return result;
    result.originalSourceStop=config.pc_;result.selectionStop=s.pc;result.allocatorStop=h.nativeWaterAllocationEntry.pc;
    const auto finish=[&](WaterArrayRetirementResult::Status status)noexcept {
        result.status=status;result.adaptedResumePC=n.adaptedResumePC;
        result.dataRetired=n.dataRetired;result.tableRetired=n.tableRetired;
        result.countCleared=n.countCleared;result.childCleared=n.childCleared;return result;
    };
    if(n.attempted)return finish(WaterArrayRetirementResult::Status::alreadyAttempted);
    const bool retired=config.retireArraySectionBackingNativeOnly();
    return finish(retired?WaterArrayRetirementResult::Status::nativeRetired:
        WaterArrayRetirementResult::Status::nativeAdmissionStopped);
}

#include "original_source_world_water_array_load50la.inl"
