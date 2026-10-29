#pragma once
#include <cstdio>
namespace ra_cold_aggregate50LA {
inline bool Owner::continueSourceWorldTriggerList2DC5D0(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceTriggerPhase_!=SourceTriggerPhase::firstRowPrefix||sourceTriggerPC_!=0x2dc5d0||sourceTrackPC_!=0x2dc5d0||
       sourceTriggerCohortAttempted_||sourceTriggerRow_!=0||!sourceTriggerListSuccessCount_||*sourceTriggerListSuccessCount_!=0||
       !sourceTriggerListStartIndex_||*sourceTriggerListStartIndex_!=0||!sourceTriggerListLimit_||*sourceTriggerListLimit_!=2||
       !sourceTriggerRawOffset_||*sourceTriggerRawOffset_!=0||!sourceTriggerDescriptorReturn_||*sourceTriggerDescriptorReturn_!=1||
       !sourceLoadRace_||!sourceLoadApplication_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldParser34_||
       !sourceWorldObjectConfig_||!sourceWorldObjectConfig_->source_||*worldParser34_!=sourceWorldObjectConfig_->source_.get()||
       sourceTriggerConfigArg_!=sourceWorldObjectConfig_.get()||!sourceWorldObjectConfig_->sourceTriggerConfigCurrent()||
       !sourceWorldObjectConfig_->sourceTriggerSelected_||*sourceWorldObjectConfig_->sourceTriggerSelected_!=2||
       !nativeTriggerList3D0_||nativeTriggerList3D0_->descriptorSlotsPublished!=1||
       !preflightSourceWorldTriggerNative())return false;
    auto& l=*nativeTriggerList3D0_;const auto& first=l.descriptorOwners[0];
    if(!first||!first->sourceDescriptorReturned||!first->sourceActionLogAttempted||!first->sourceActionReaderComplete)return false;
    sourceTriggerCohortAttempted_=true;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceTriggerPC_=at;sourceTrackPC_=at;
        sourceTriggerPhase_=SourceTriggerPhase::bounded;return false;};
    for(;;){
        //Actual descriptor return is ignored by the list: its s1 increment is
        //unconditional. This bounded cohort reaches each real return before it.
        pc=0x2dc5d0;++*sourceTriggerListSuccessCount_;
        pc=0x2dc5d8;++sourceTriggerRow_;
        const bool another=sourceTriggerRow_<*sourceTriggerListLimit_; //2DC5DC/5E0
        pc=0x2dc5e4;*sourceTriggerRawOffset_+=4; //Branch delay runs on BOTH outcomes.
        if(!another)break;
        sourceTriggerPhase_=SourceTriggerPhase::rowEntered;
        if(!loadSourceWorldTriggerDescriptor2DC53C(pc))return stop(pc);
        sourceTrackPC_=pc;
        if(!continueSourceWorldTrigger2DBF98(pc))return stop(pc);
        sourceTrackPC_=pc;
        if(!continueSourceTriggerInstance2DBFE0(pc))return stop(pc);
        sourceTrackPC_=pc;
        if(!continueSourceWorldTriggerAction2DC024(pc))return stop(pc);
        sourceTrackPC_=pc;
        const auto& completed=l.descriptorOwners[sourceTriggerRow_];
        if(pc!=0x2dc5d0||!sourceTriggerDescriptorReturn_||*sourceTriggerDescriptorReturn_!=1||
           !completed||!completed->sourceDescriptorReturned||!completed->sourceActionLogAttempted||
           !completed->sourceActionReaderComplete)return stop(0x2dc5d0);
    }
    if(sourceTriggerRow_!=2||*sourceTriggerRawOffset_!=8||*sourceTriggerListSuccessCount_!=2||
       l.count30!=2||l.descriptorSlotsPublished!=2)return stop(0x2dc5e8);
    //Actual list1336C0 at2DC5F4, exact live args success2/count2. Separate
    //native adapter receipt, not reusing either per-descriptor diagnostic.
    static constexpr char diagnostic[]="Read 2 triggers out of a total of 2.\n";
    pc=0x2dc5f4;if(sourceTriggerListLogAttempted_)return stop(pc);
    sourceTriggerListLogAttempted_=true;
    sourceTriggerListLogBytes_=std::fwrite(diagnostic,1,sizeof(diagnostic)-1,stdout);
    sourceTriggerListLogError_=std::ferror(stdout)!=0;
    pc=0x2dc5fc;sourceTriggerListReturn_=1; //Actual v74 returns to1B5054.
    //The real positive branch skips release/null3D0;1B507C returns to271D4C.
    //No2DC630 service, callback install, observer or navigation dispatch here.
    sourceTriggerPhase_=SourceTriggerPhase::listReturned;pc=sourceTriggerPC_=0x271d4c;return true;
}
} //namespace ra_cold_aggregate50LA
