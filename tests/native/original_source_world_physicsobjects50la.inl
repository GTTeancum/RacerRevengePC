#pragma once
namespace ra_cold_aggregate50LA {
inline bool Owner::continueSourceWorldPhysicsObjects2718C0(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||sourcePhysicsObjectsAttempted_||
       sourceTrackPC_!=0x271d4c||sourceTriggerPhase_!=SourceTriggerPhase::listReturned||
       sourceTriggerPC_!=0x271d4c||!sourceTriggerListReturn_||*sourceTriggerListReturn_!=1||
       !sourceLoadRace_||!sourceLoadApplication_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldParser34_||!sourceWorldObjectConfig_||
       !sourceWorldObjectConfig_->source_||*worldParser34_!=sourceWorldObjectConfig_->source_.get()||
       sourceTriggerConfigArg_!=sourceWorldObjectConfig_.get()||!sourceWorldObjectConfig_->sourceTriggerConfigCurrent()||
       !sourceWorldObjectConfig_->sourceTriggerSelected_||*sourceWorldObjectConfig_->sourceTriggerSelected_!=4||
       !preflightSourceWorldTriggerNative())return false;
    sourcePhysicsObjectsAttempted_=true;
    auto& config=*sourceWorldObjectConfig_;a::OriginalStandardTextureDiagnostic50LA d;bool returned=false;
    pc=0x2718e0;
    if(!config.selectSourcePhysicsObjects157D00(returned,d)){pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}
    //Actual selector return is ignored at2718E8. This bounded same-source path
    //proves zero without discarding the retained Trigger2Action1 cache.
    if(returned){pc=0x2718e8;return false;}
    pc=0x2718f8;
    if(!config.readSourcePhysicsObjectCount158B30(sourcePhysicsObjectStack14C_,returned,d)){
        pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;
    }
    pc=0x271900;
    if(returned||!sourcePhysicsObjectStack14C_||*sourcePhysicsObjectStack14C_!=0){pc=0x271904;return false;}
    //Actual BLEZ at271904 takes2719E8. It never reads/writes World4B0/4B4,
    //allocates a PhysicsObject or reaches their constructors/loaders.
    //v0 is the actual getter0 and is ignored by the containing vA4 caller.
    sourcePhysicsObjectsReturned_=true;pc=0x271d5c;return true;
}
} //namespace ra_cold_aggregate50LA
