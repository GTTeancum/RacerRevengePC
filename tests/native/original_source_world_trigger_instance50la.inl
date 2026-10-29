#pragma once
namespace ra_cold_aggregate50LA {
inline bool Owner::continueSourceTriggerInstance2DBFE0(std::uint32_t& pc)noexcept {
    pc=0;std::array<std::uint32_t,4> origin,sentinel;std::uint32_t category=0;
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||
       sourceTriggerPhase_!=SourceTriggerPhase::radiusPrefix||sourceTriggerPC_!=0x2dbfe0||sourceTrackPC_!=0x2dbfe0||
       !sourceLoadRace_||!sourceLoadApplication_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldParser34_||
       !sourceWorldObjectConfig_||!sourceWorldObjectConfig_->source_||
       *worldParser34_!=sourceWorldObjectConfig_->source_.get()||
       !sourceTriggerEulerAttempted_||!sourceTriggerEulerReturned_||!sourceTriggerRadiusReturned_||
       !sourceWorldObjectConfig_||sourceTriggerConfigArg_!=sourceWorldObjectConfig_.get()||
       !sourceWorldObjectConfig_->sourceTriggerConfigCurrent()||!sourceWorldObjectConfig_->sourceTriggerSelected_||
       *sourceWorldObjectConfig_->sourceTriggerSelected_!=1+2*sourceTriggerRow_||sourceTriggerRow_>=2||
       !nativeTriggerList3D0_||!sourceTrigger3D0_||*sourceTrigger3D0_!=nativeTriggerList3D0_.get()||
       !nativeTriggerList3D0_->sourceMode||nativeTriggerList3D0_->descriptorSlotsPublished!=sourceTriggerRow_+1||
       !nativeTriggerList3D0_->descriptorOwners[sourceTriggerRow_]||
       !copySourceTriggerInstanceOrigin(origin)||!copySourceTriggerCollisionDefaults(sentinel,category)||
       !preflightSourceWorldTriggerNative()||!reserveSourceTriggerInstanceNative())return false;
    auto& d=*nativeTriggerList3D0_->descriptorOwners[sourceTriggerRow_];
    if(!d.sourceFields||!d.sourceFields->geometry64||!d.sourceFields->sourceGeometry64||
       *d.sourceFields->sourceGeometry64!=d.sourceFields->geometry64.get()||d.actionCount04!=1||
       d.sourceInstanceAttempted)return false;
    sourceTriggerPhase_=SourceTriggerPhase::instanceEntered;
    const auto stop=[&]()noexcept{sourceTriggerPC_=pc;sourceTriggerPhase_=SourceTriggerPhase::bounded;return false;};
    if(!constructSourceTriggerInstance2DBFE0(d,origin,pc))return stop();
    if(!setupSourceTriggerInstance2DC01C(d,sentinel,category,pc))return stop();
    sourceTriggerPC_=pc=0x2dc024;sourceTriggerPhase_=SourceTriggerPhase::instancePrefix;return true;
}
} //namespace ra_cold_aggregate50LA
