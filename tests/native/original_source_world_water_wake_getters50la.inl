bool Owner::preflightNativeWakeGetters()const noexcept {
    if(!preflightNativeWakePacked())return false;
    const auto& h=sourceWaterConfig_;
    if(!sourceWaterConfigOwner_){for(const auto& p:h.nativeWakeGetterPublications)if(p)return false;return true;}
    const auto& g=sourceWaterConfigOwner_->nativeWakeGetters_;
    if(!g.entered){for(const auto& p:h.nativeWakeGetterPublications)if(p)return false;return true;}
    if(!h.wakeSelectionEntered||!sourceWaterConfigOwner_->nativeWakeLoad_.nativeLoaded||
       (!sourceWaterProvider274720_.nativeWithdrawn&&(!sourceWaterProvider_||!worldApplication_)))return false;
    for(std::size_t ordinal=0;ordinal<5u;++ordinal){
        const auto& receipt=h.nativeWakeGetterPublications[ordinal];
        if(ordinal>=g.completed){if(receipt)return false;continue;}
        const auto& result=ordinal==1u?g.alpha.result:g.floats[ordinal?ordinal-1u:0u].result;
        if(!receipt||!*receipt||!result)return false;
        if(!sourceWaterProvider274720_.nativeWithdrawn){
            const auto lane=30u+ordinal;const auto& p=worldApplication_->sourceWaterProcessScalars_;
            if(p.publications[lane]<*receipt)return false;
            if(p.publications[lane]==*receipt&&!p.unknown&&
               (!p.nativeAdapted[lane]||p.words[lane]!=result))return false;
        }
    }
    return true;
}
Owner::WaterWakeSettingsResult Owner::readWakeSettingsNativeOnly(WaterWakeLoadMode mode)noexcept {
    WaterWakeSettingsResult result;
    const auto& h=sourceWaterConfig_;const auto& r=sourceWaterProviderInitializer_;
    if(mode!=WaterWakeLoadMode::nativeRetiredBackingOnly||closed_||
       nativeScbThread_!=std::this_thread::get_id()||runtimeCameraBorrows_||runtimeWorldBorrows_||
       constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
       !h.wakeSelectionEntered||h.nativeWithdrawn||sourceWaterProvider274720_.nativeWithdrawn||
       !sourceWaterConfigOwner_||!sourceWaterProvider_||!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       r.pc!=0x30492cu||sourcePostParticleModelsPC_!=r.pc||sourceWaterProvider274720_.pc!=r.pc||
       sourceWaterProviderNumeric_.pc!=r.pc||!preflightSourceWaterConfigNative())return result;
    auto& config=*sourceWaterConfigOwner_;auto& g=config.nativeWakeGetters_;
    result.originalSourceStop=config.pc_;
    if(!config.nativeWakeLoad_.nativeLoaded)return result;
    if(g.entered){
        result.status=WaterWakeSettingsResult::Status::alreadyAttempted;
        result.adaptedPC=g.pc;result.completed=g.completed;return result;
    }
    g.entered=true;
    static constexpr std::array<std::uint32_t,5> delays{{0x2b97dcu,0x2b97f8u,0x2b981cu,0x2b9840u,0x2b9860u}};
    for(std::size_t ordinal=0;ordinal<5u;++ordinal){
        const auto lane=30u+ordinal;g.pc=delays[ordinal];
        if(!worldApplication_->waterAdaptedLoadScalarAdmittedNative(lane))break;
        auto& p=worldApplication_->sourceWaterProcessScalars_;
        a::OriginalStandardTextureDiagnostic50LA diagnostic;
        if(!config.readWakeSettingNative(ordinal,p.words[lane],diagnostic))break;
        //No fallible work follows the getter's actual adapted output store.
        //Same-App globals persist beyond this provider. Provenance forbids
        //source-exact scalar/vector consumers from laundering this write.
        p.nativeAdapted[lane]=true;++p.publications[lane];
        sourceWaterConfig_.nativeWakeGetterPublications[ordinal]=p.publications[lane];
    }
    result.status=g.completed==5u?WaterWakeSettingsResult::Status::nativeCompleted:
        WaterWakeSettingsResult::Status::nativeAdmissionStopped;
    result.adaptedPC=g.pc;result.completed=g.completed;return result;
}

#include "original_source_world_water_wake_packed50la.inl"
