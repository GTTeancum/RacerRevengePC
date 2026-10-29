#pragma once
#include "original_source_world_light_read50la.inl"
#include <cstdio>
namespace ra_cold_aggregate50LA {
inline bool Owner::continueSourceWorldTriggerAction2DC024(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||terminal_||closed_||
       sourceTriggerPhase_!=SourceTriggerPhase::instancePrefix||sourceTriggerPC_!=0x2dc024||sourceTrackPC_!=0x2dc024||
       !sourceLoadRace_||!sourceLoadApplication_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldParser34_||
       !sourceWorldObjectConfig_||!sourceWorldObjectConfig_->source_||
       *worldParser34_!=sourceWorldObjectConfig_->source_.get()||
       sourceTriggerConfigArg_!=sourceWorldObjectConfig_.get()||!sourceWorldObjectConfig_->sourceTriggerConfigCurrent()||
       !sourceWorldObjectConfig_->sourceTriggerSelected_||*sourceWorldObjectConfig_->sourceTriggerSelected_!=1+2*sourceTriggerRow_||
       !sourceTriggerActionSuccessCount_||*sourceTriggerActionSuccessCount_!=0||sourceTriggerRow_>=2||
       !nativeTriggerList3D0_||!nativeTriggerList3D0_->descriptorOwners[sourceTriggerRow_]||!preflightSourceWorldTriggerNative())return false;
    auto& desc=*nativeTriggerList3D0_->descriptorOwners[sourceTriggerRow_];
    if(!desc.sourceFields||!desc.trigger00||!desc.sourceInstancePublished||desc.actionCount04!=1||
       desc.sourceActionAttempted||!sourceTriggerParentKnown_||!sourceTriggerActionType_||*sourceTriggerActionType_!=0||!sourceTriggerFactoryVtable_||*sourceTriggerFactoryVtable_!=0x3cf090)return false;
    desc.sourceActionAttempted=true;sourceTriggerPhase_=SourceTriggerPhase::actionEntered;
    const auto stop=[&](std::uint32_t at)noexcept{pc=sourceTriggerPC_=at;sourceTriggerPhase_=SourceTriggerPhase::bounded;return false;};
    auto& config=*sourceWorldObjectConfig_;a::OriginalStandardTextureDiagnostic50LA d;bool found=false,returned=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& present,std::uint32_t& copy)noexcept {
        if(!config.readSourceTriggerField158240(key,value,present,copy,d)){pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    //Actual158B30's one-character case-insensitive F comparison accepts FALSE.
    //Narrow authored lexeme only; do not widen the common integer parser.
    std::string_view value;std::uint32_t copy=0;std::int32_t oneTime=0;
    pc=0x2dc034;if(!fetch("onetimeonly",value,found,copy))return stop(pc);
    if(found){pc=0x158990;if(value!="FALSE")return stop(0x158978);oneTime=0;}
    else{pc=0x158b60;oneTime=0;}
    pc=0x2dc060;if(!sourceTriggerOneTime2DC060(desc,oneTime))return stop(pc);
    std::size_t parentLength=0;while(parentLength<sourceTriggerParentName_.size()&&sourceTriggerParentName_[parentLength])++parentLength;
    pc=0x2dc068;if(parentLength==sourceTriggerParentName_.size()||
        !sourceTriggerParent2DC068(desc,sourceTriggerParentName_.data(),parentLength))return stop(pc);
    pc=0x2dc074;if(!desc.sourceFields->enabled0C)return stop(0x2dc088); //No unjoined virtual hide.
    pc=0x2dc0a8;try{desc.actions68.reset(new NativeActionDescriptor*[1]);}catch(...){desc.actions68.reset();}
    desc.sourceActionsAllocated=true;pc=0x2dc0b0; //Actual nullable68 publication, elements stillunknown.
    pc=0x2dc0b8;sourceTriggerActionLoopIndex_=0;
    //Actual retained row0/1 and action index0 prove Trigger%dAction%d =>Trigger1Action1/Trigger2Action1.
    static constexpr char firstSection[]="Trigger1Action1",secondSection[]="Trigger2Action1";
    const char* actualSection=sourceTriggerRow_?secondSection:firstSection;
    std::memcpy(sourceTriggerActionSection_.data(),actualSection,sizeof(firstSection));sourceTriggerActionSectionKnown_=true; //Native fixed-array stack name retention, not a Config admission.
    pc=0x2dc0d8;if(!config.selectSourceTriggerSection157D00(static_cast<std::uint32_t>(2+2*sourceTriggerRow_),returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    if(!returned)return stop(0x2dc0e8); //Missing section logging/fatal branch not claimed.
    pc=0x2dc120;if(!read.text("actiontype","none",sourceTriggerActionName_.data(),sourceTriggerActionName_.size(),found))return stop(pc);
    //1B5E70 executes actual case-insensitive Shortcut/CameraShake/LostAI comparisons.
    //Only this exact authenticated type is represented, not fallback factory authority.
    if(std::string_view(sourceTriggerActionName_.data())!="LostAI")return stop(0x1b5e90);
    pc=0x2dc13c;sourceTriggerActionType_=22; //Actual resolver v0 copied to retained stack398.
    pc=0x1b5db8;try{desc.action.reset(new NativeActionDescriptor);}catch(...){desc.action.reset();}
    desc.sourceActionAllocated=true;
    //This existing typed allocation's constructor IS the exact nonfallible
    //2DA090/1B5DDC/E0/E4 store sequence; no ready Actions owner is adopted.
    pc=0x1b5dec;if(!desc.actions68)return stop(pc); //Keep actual allocated unpublishable action.
    desc.actions68[0]=desc.action.get();desc.actionSlotsPublished=1; //NULL included.
    pc=0x2dc1ec;if(!desc.action)return stop(pc);
    auto& action=*desc.action;action.type00=22;desc.sourceActionTypeWritten=true;
    //Each source getter searches the actual currently selected two-field block.
    //The typed LostAI target is uint32 bits; defaults/parser stores occur here,
    //not by copying native authoring/navigation data.
    auto integerWord=[&](const char* key,std::uint32_t& output)noexcept {
        std::string_view v;std::uint32_t cp=0;bool present=false;if(!fetch(key,v,present,cp))return false;
        std::int32_t n=0;
        if(present){pc=0x158a34;if(!source_world_light50LA::Reader<decltype(fetch)>::integerToken(v,n))return false;pc=0x158a44;}
        else pc=0x158b60;
        output=static_cast<std::uint32_t>(n);return true;
    };
    pc=0x1b5a88;if(!integerWord("targetnodeindex",action.payload10C.targetNodeIndex))return stop(pc);
    //Reset is an actual missing integer getter, so the existing semantic bool
    //represents precisely source word0. No host bool parser or positive path.
    pc=0x1b5aa0;if(!fetch("reset",value,found,copy))return stop(pc);
    if(found)return stop(0x158a34);pc=0x158b60;action.payload10C.reset=false;
    pc=0x2da0d0;if(!fetch("specificobject",value,found,copy))return stop(pc);
    if(!found){value={};copy=0x158b08;}
    pc=copy;if(value.size()>259)return stop(pc);
    for(std::size_t i=0;i<value.size();++i)action.text04[i]=value[i];action.text04[value.size()]='\0';
    //Actual common return1 then derived return. No callback installation or
    //navigation reference has occurred; exact authored defaults qualify it.
    if(action.payload10C.targetNodeIndex!=(sourceTriggerRow_?104u:29u)||action.payload10C.reset||!value.empty())return stop(0x2da0e0);
    desc.sourceActionReaderComplete=true;pc=0x2dc20c;++*sourceTriggerActionSuccessCount_;
    pc=0x2dc210;++*sourceTriggerActionLoopIndex_; //Now1, actual count04 is1: loop exits.
    //Explicit native1336C0 diagnostic adaptation, not guest stdio equivalence.
    //Exact retained actual arguments (success1, Trigger1/2,total1), one latched call.
    static constexpr char firstDiagnostic[]="Read 1 Action(s) for Trigger1 out of a total of 1.\n";
    static constexpr char secondDiagnostic[]="Read 1 Action(s) for Trigger2 out of a total of 1.\n";
    const char* diagnostic=sourceTriggerRow_?secondDiagnostic:firstDiagnostic;
    pc=0x2dc234;if(sourceTriggerActionLogAttempted_||desc.sourceActionLogAttempted)return stop(pc);
    sourceTriggerActionLogAttempted_=true;desc.sourceActionLogAttempted=true;
    sourceTriggerActionLogBytes_=std::fwrite(diagnostic,1,sizeof(firstDiagnostic)-1,stdout);
    sourceTriggerActionLogError_=std::ferror(stdout)!=0; //Source ignores return/short write.
    desc.sourceActionLogBytes=sourceTriggerActionLogBytes_;desc.sourceActionLogError=sourceTriggerActionLogError_;
    pc=0x2dc23c;sourceTriggerDescriptorReturn_=1;desc.sourceDescriptorReturned=true;
    sourceTriggerPhase_=SourceTriggerPhase::firstRowPrefix;pc=sourceTriggerPC_=0x2dc5d0;return true;
}
} //namespace ra_cold_aggregate50LA
