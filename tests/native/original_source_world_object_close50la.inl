#pragma once
namespace ra_cold_aggregate50LA {
bool Owner::closeSourceObjectConfig272800(std::uint32_t& pc)noexcept {
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceObjectConfigCloseAttempted_||sourceTrackStep_!=SourceTrackStep::objectsReturned||
       sourceTrackPC_!=0x1aa0d4||!sourceTobA4Returned_||!sourceTobA4V0_||*sourceTobA4V0_!=1||
       sourceStaticModelParamPhase_!=SourceStaticModelParamPhase::returned||
       !preflightSourceWorldStaticModelParamsNative()||!preflightSourceWorldBuoysNative()||
       !preflightSourceWorldParticleCountNative()||!preflightSourceWorldSunFlareNative()||
       !preflightSourceWorldCsCamerasNative()||!preflightSourceWorldTriggerNative()||
       !preflightSourceWorldTerrainNative()||!preflightSourceWorldToyNative()||!sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||
       !sourceWorldObjectConfig_||!sourceWorldObjectConfig_->source_||!sourceTrackFile_||
       sourceWorldObjectConfig_->file_!=sourceTrackFile_||!sourceObjectConfig34Published_||
       !worldParser34_||*worldParser34_!=sourceWorldObjectConfig_->source_.get()||
       !sourceWorldObjectConfig_->sourceStaticModelParamConfigCurrent()||
       sourceWorldObjectConfig_->sourceStaticModelParamNextSelection_!=674||
       !sourceWorldObjectConfig_->sourceStaticModelParamSelected_||
       *sourceWorldObjectConfig_->sourceStaticModelParamSelected_!=673||
       !sourceTrackFile_->current(*sourceWorldObjectConfig_,sourceWorldObjectConfig_->source_->file00))return false;
    sourceObjectConfigCloseAttempted_=true;pc=sourceTrackPC_=0x27281c;
    a::OriginalStandardTextureDiagnostic50LA d;
    if(!sourceWorldObjectConfig_->close1573B0(d)){
        pc=sourceTrackPC_=d.sourcePC?d.sourcePC:sourceWorldObjectConfig_->sourcePC_;return false;
    }
    //Only the actual returned +1 destructor reaches this source NULL store.
    pc=sourceTrackPC_=0x272824;worldParser34_=nullptr;sourceObjectConfigCloseReturned_=true;
    //Retain the SAME wrapper as the historical Terrain/Trigger/Particle Config
    //argument. Withdraw only its now-closed native File occurrence; no copied
    //authority or destructor receipt for the next, distinct AUB allocation.
    pc=sourceTrackPC_=0x1aa0e8;
    if(!sourceWorldObjectConfig_->abortNative(d)){
        if(d.sourcePC)pc=sourceTrackPC_=d.sourcePC;return false;
    }
    sourceObjectConfigNativeWithdrawn_=true;sourceTrackStep_=SourceTrackStep::objectConfigClosed;
    return true; //Actual vE0 return1AA0E8, before vE4 dispatch/AUB producer.
}
}
