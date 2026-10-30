#pragma once
namespace ra_cold_aggregate50LA {
bool Owner::registerSourceSpotlight148F30(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    //Conditional only. No source loop admission or index17 manufacture.
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       !sourceTopologyLightManager_||!preflightSourceTypedLightsNative()||f.ordinal<2||
       f.ordinal-2>=sourceTypedLights_.size()||sourceTypedLights_[f.ordinal-2].get()!=&f||
       !sourceLightPopulationS0_||*sourceLightPopulationS0_!=static_cast<std::int32_t>(f.ordinal)||
       !f.spotlightConfigEntered||!f.spotlightCone9C||!f.configured||!f.light||!f.descriptor||
       f.descriptor->type00!=3||f.pc!=0x277fcc||f.observerEntered||f.registrationEntered)return false;
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic d;
    const bool primary=sourceTopologyLightManager_->bindSourceTypedPrimary1474A0(*this,f.ordinal,d);
    if(!primary&&(d.sourcePC!=0x280bf0||f.primaryStoreMask!=31)){pc=d.sourcePC;return false;}
    if(!sourceTopologyLightManager_->continueSourceTypedSecondary280BF0(*this,f.ordinal,d)||
       !sourceTopologyLightManager_->finishSourceTypedRegistration149088(*this,f.ordinal,d)){pc=d.sourcePC;return false;}
    pc=f.pc=0x277fd4;return true; //Wrapper flare/flasher flags/descriptor retirement still belong to caller.
}
}
