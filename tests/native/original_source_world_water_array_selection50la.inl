bool Owner::preflightNativeWaterArraySelection()const noexcept {
    if(!preflightNativeWaterArrayRetirement())return false;
    if(!sourceWaterConfigOwner_)return true;
    const auto& n=sourceWaterConfigOwner_->nativeArraySelection_;
    if(!sourceWaterConfigOwner_->preflightArraySelectionNative())return false;
    if(!n.attempted)return true;
    const auto& f=sourceWaterConfig_.nativeWaterArrayFormat;
    if(!f.attempted||f.phase!=SourceWaterConfig::NativeWaterArrayFormat::Phase::selectionEntry||
       f.pc!=0x157d00u||!f.selectionCallIssued||!f.role||*f.role==NativeWaterArrayRole::zoneRaw||
       f.configIdentity!=sourceWaterConfigOwner_.get()||n.queryIdentity!=f.output.data()||
       n.queryKnown!=f.known||n.queryKnown!=(*f.role==NativeWaterArrayRole::signaturePointers?17u:7u))return false;
    for(std::size_t i=0;i<n.queryKnown;++i)
        if(!n.query.known[i]||n.query.value[i]!=static_cast<unsigned char>(f.output[i]))return false;
    return true;
}
Owner::WaterArraySelectionResult Owner::selectWaterArraySectionNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterArraySelectionResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    const auto& f=h.nativeWaterArrayFormat;
    if(!f.attempted||f.phase!=SourceWaterConfig::NativeWaterArrayFormat::Phase::selectionEntry||
       f.pc!=0x157d00u||!f.selectionCallIssued||f.configIdentity!=sourceWaterConfigOwner_.get())return result;
    auto& config=*sourceWaterConfigOwner_;const auto& n=config.nativeArraySelection_;
    result.originalSourceStop=config.pc_;result.allocatorStop=h.nativeWaterAllocationEntry.pc;
    const auto finish=[&](WaterArraySelectionResult::Status status)noexcept {
        result.status=status;result.index=n.index;result.selectionReturn=n.returned;
        result.adaptedPC=n.returned?f.selectionReturnPC:std::optional<std::uint32_t>(n.pc);
        result.currentWakeRetained=config.nativeWakeLoad_.nativeLoaded&&config.nativeWakeLoad_.table&&
            config.nativeWakeLoad_.data&&
            config.source_.child24==&config.wakeTableRole_&&config.source_.selected20==config.nativeWakeLoad_.counted;
        return result;
    };
    if(n.attempted)return finish(WaterArraySelectionResult::Status::alreadyAttempted);
    a::OriginalStandardTextureDiagnostic50LA d;
    const bool completed=config.selectArraySectionNativeOnly(f.output.data(),f.known,d);
    if(completed&&n.returned==false)return finish(WaterArraySelectionResult::Status::nativeMissingReturn);
    return finish(n.heapHeaderBoundary?WaterArraySelectionResult::Status::nativeHeapMetadataBoundary:
        WaterArraySelectionResult::Status::nativeAdmissionStopped);
}

#include "original_source_world_water_array_retirement50la.inl"
