bool Owner::preflightNativeWaterArrayFilename()const noexcept {
    if(!preflightNativeWaterArrayScalars())return false;
    const auto& output=sourceWaterConfig_.arrayFilename790;
    if(!sourceWaterConfigOwner_){for(const auto known:output.known)if(known)return false;return true;}
    const auto& g=sourceWaterConfigOwner_->nativeArrayFilename_;const auto& f=g.value;
    if(!sourceWaterConfigOwner_->preflightArrayFilenameNative())return false;
    for(std::size_t i=0;i<output.value.size();++i){
        const auto* receipt=f.defaultCopy.stores.known[i]?&f.defaultCopy.stores:
            f.lookupCopy.stores.known[i]?&f.lookupCopy.stores:nullptr;
        if(output.known[i]!=bool(receipt)||(receipt&&output.value[i]!=receipt->value[i])||
           (i>=128u&&output.known[i]))return false;
    }
    if(!g.entered)return true;
    const auto& formatter=sourceWaterConfig_.nativeWaterArrayFormat;
    if(!formatter.role||*formatter.role==NativeWaterArrayRole::zoneRaw||!formatter.selectionCallIssued||
       formatter.configIdentity!=sourceWaterConfigOwner_.get()||g.destinationIdentity!=&output)return false;
    return g.callerReturnPC==(*formatter.role==NativeWaterArrayRole::signaturePointers?0x2b9928u:0x2b9ca4u);
}
Owner::WaterArrayFilenameResult Owner::readWaterArrayFilenameNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterArrayFilenameResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& config=*sourceWaterConfigOwner_;const auto& g=config.nativeArrayFilename_;
    const auto& f=h.nativeWaterArrayFormat;
    if(!f.role||*f.role==NativeWaterArrayRole::zoneRaw||!f.selectionCallIssued||
       f.configIdentity!=sourceWaterConfigOwner_.get()||
       (!config.nativeArrayLoad_.nativeLoaded&&config.nativeArraySelection_.returned!=false))return result;
    result.originalSourceStop=config.pc_;result.selectionStop=config.nativeArraySelection_.pc;
    result.allocatorStop=h.nativeWaterAllocationEntry.pc;
    const auto finish=[&](WaterArrayFilenameResult::Status status)noexcept {
        result.status=status;result.nativeGetterValue=g.value.returned;
        result.adaptedPC=g.value.returned?g.callerReturnPC:std::optional<std::uint32_t>(g.pc);
        for(const auto known:h.arrayFilename790.known)if(known)++result.knownOutputBytes;
        return result;
    };
    if(g.entered)return finish(WaterArrayFilenameResult::Status::alreadyAttempted);
    const auto returnPC=*f.role==NativeWaterArrayRole::signaturePointers?0x2b9928u:0x2b9ca4u;
    a::OriginalStandardTextureDiagnostic50LA diagnostic;
    //Both original caller setups use sameConfig+240, key3C0B08, default3C0B18,
    //destination+790 and t0=-1. Neither tests the preceding selection value.
    const bool completed=config.readArrayFilenameNativeOnly(sourceWaterConfig_.arrayFilename790,returnPC,diagnostic);
    return finish(completed?WaterArrayFilenameResult::Status::nativeCompleted:
        WaterArrayFilenameResult::Status::nativeAdmissionStopped);
}

#include "original_source_world_water_array_scalars50la.inl"
