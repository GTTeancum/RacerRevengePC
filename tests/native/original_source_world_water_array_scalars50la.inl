bool Owner::preflightNativeWaterArrayScalars()const noexcept {
    if(!preflightNativeWaterPoolPathFormat())return false;
    const auto& words=sourceWaterConfig_.nativeWaterArrayScalars;
    const auto noNewWords=[&]()noexcept {
        for(const auto& word:words.stackA48ToA70)if(word)return false;
        for(const auto& word:words.stackA74ToA7C)if(word)return false;return true;
    };
    if(!sourceWaterConfigOwner_)return noNewWords();
    const auto& g=sourceWaterConfigOwner_->nativeArrayScalars_;
    if(!sourceWaterConfigOwner_->preflightArrayScalarsNative())return false;
    if(!g.entered)return noNewWords();
    const auto& format=sourceWaterConfig_.nativeWaterArrayFormat;
    const auto& prefix=sourceWaterConfig_.nativeWaterArrayPrefix;
    if(!format.role||*format.role==NativeWaterArrayRole::zoneRaw||
       g.signature!=(*format.role==NativeWaterArrayRole::signaturePointers)||
       prefix.signatureArm!=g.signature||!sourceWaterConfigOwner_->nativeArrayFilename_.value.returned)return false;
    static constexpr std::array<std::size_t,11> temporaryIndex{{0u,1u,7u,4u,5u,6u,2u,3u,10u,8u,9u}};
    for(std::size_t ordinal=0;ordinal<(g.signature?14u:3u);++ordinal){
        const auto* destination=g.signature?(ordinal<3u?&prefix.stack820[ordinal]:
            &words.stackA48ToA70[temporaryIndex[ordinal-3u]]):&words.stackA74ToA7C[ordinal];
        const auto index=a::OriginalWaterSourceConfig50LA::arrayScalarIndexNative(ordinal);
        const bool integer=a::OriginalWaterSourceConfig50LA::arrayScalarIntegerNative(ordinal);
        const auto& value=integer?g.integers[index].result:g.floats[index].result;
        if(g.destinations[ordinal]&&g.destinations[ordinal]!=destination)return false;
        const auto expected=value?value:g.signature&&ordinal<3u?prefix.initialized820[ordinal]:std::optional<std::uint32_t>{};
        if(*destination!=expected)return false;
    }
    if(g.signature){for(const auto& word:words.stackA74ToA7C)if(word)return false;}
    else for(const auto& word:words.stackA48ToA70)if(word)return false;
    return true;
}
Owner::WaterArrayScalarsResult Owner::readWaterArrayScalarsNativeOnly(WaterArrayBackingMode mode)noexcept {
    WaterArrayScalarsResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterArrayBackingMode::nativeSuccessBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& config=*sourceWaterConfigOwner_;const auto& g=config.nativeArrayScalars_;
    const auto& format=h.nativeWaterArrayFormat;
    if(!format.role||*format.role==NativeWaterArrayRole::zoneRaw||!config.nativeArrayFilename_.value.returned)return result;
    const bool signature=*format.role==NativeWaterArrayRole::signaturePointers;
    result.originalSourceStop=config.pc_;result.selectionStop=config.nativeArraySelection_.pc;
    result.allocatorStop=h.nativeWaterAllocationEntry.pc;
    const auto finish=[&](WaterArrayScalarsResult::Status status)noexcept {
        result.status=status;result.completed=g.completed;result.adaptedPC=g.pc;
        if(g.entered)result.signature=g.signature;return result;
    };
    if(g.entered)return finish(WaterArrayScalarsResult::Status::alreadyAttempted);
    auto& words=sourceWaterConfig_.nativeWaterArrayScalars;
    auto& prefix=sourceWaterConfig_.nativeWaterArrayPrefix;
    static constexpr std::array<std::size_t,11> temporaryIndex{{0u,1u,7u,4u,5u,6u,2u,3u,10u,8u,9u}};
    for(std::size_t ordinal=0;ordinal<(signature?14u:3u);++ordinal){
        auto* destination=signature?(ordinal<3u?&prefix.stack820[ordinal]:
            &words.stackA48ToA70[temporaryIndex[ordinal-3u]]):&words.stackA74ToA7C[ordinal];
        //Existing stack820 lanes are overwritten in place. New temporaries are
        //unknown until THEIR getter store; no detached duplicate or blanket zero.
        a::OriginalStandardTextureDiagnostic50LA diagnostic;
        if(!config.readArrayScalarNative(ordinal,signature,*destination,diagnostic))
            return finish(WaterArrayScalarsResult::Status::nativeAdmissionStopped);
    }
    return finish(WaterArrayScalarsResult::Status::nativeCompleted);
}

#include "original_source_world_water_pool_format50la.inl"
