bool Owner::preflightNativeWaterArrayLoad()const noexcept {
    if(!preflightNativeWaterArrayFilename())return false;
    if(!sourceWaterConfigOwner_)return true;
    const auto& n=sourceWaterConfigOwner_->nativeArrayLoad_;
    if(!sourceWaterConfigOwner_->preflightArrayLoadNative())return false;
    if(!n.attempted)return true;
    const auto& f=sourceWaterConfig_.nativeWaterArrayFormat;
    return f.attempted&&f.selectionCallIssued&&f.configIdentity==sourceWaterConfigOwner_.get()&&
        f.selectionReturnPC&&(f.role==NativeWaterArrayRole::signaturePointers||f.role==NativeWaterArrayRole::poolPointers);
}
Owner::WaterArrayLoadResult Owner::loadWaterArraySectionNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterArrayLoadResult result;
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
    const auto& retired=config.nativeArrayRetirement_;const auto& n=config.nativeArrayLoad_;
    if(!retired.tableRetired||retired.adaptedResumePC!=0x157fe8u||s.returned||!s.heapHeaderBoundary)return result;
    result.originalSourceStop=config.pc_;result.selectionStop=s.pc;result.allocatorStop=h.nativeWaterAllocationEntry.pc;
    const auto finish=[&](WaterArrayLoadResult::Status status)noexcept {
        result.status=status;result.nameLength=n.nameLength;result.counted=n.counted;result.stored=n.stored;
        result.adaptedPC=n.nativeLoaded?h.nativeWaterArrayFormat.selectionReturnPC:std::optional<std::uint32_t>(n.pc);
        if(n.nativeLoaded)result.nativeSelectionValue=true;return result;
    };
    if(n.attempted)return finish(WaterArrayLoadResult::Status::alreadyAttempted);
    a::OriginalStandardTextureDiagnostic50LA diagnostic;
    const bool loaded=config.loadArraySectionNativeOnly(diagnostic);
    return finish(loaded?WaterArrayLoadResult::Status::nativeCompleted:WaterArrayLoadResult::Status::nativeAdmissionStopped);
}

#include "original_source_world_water_array_filename50la.inl"
