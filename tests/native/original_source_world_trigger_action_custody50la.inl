#pragma once
namespace ra_pod_scene50LA {
inline bool Owner::sourceTriggerOneTime2DC060(NativeTriggerDescriptor& d,std::int32_t value)noexcept {
    if(!d.sourceInstance||!d.sourceInstancePublished||d.trigger00!=&d.sourceInstance->entity_||
       !d.sourceInstance->setupReturned_||d.sourceInstance->oneTimeWritten_)return false;
    auto& t=*d.sourceInstance;t.flags1B8_=static_cast<std::uint8_t>((t.flags1B8_&~4u)|((static_cast<std::uint32_t>(value)&1u)<<2));
    t.flagsKnown_|=4;t.oneTimeWritten_=true;return true;
}
inline bool Owner::sourceTriggerParent2DC068(NativeTriggerDescriptor& d,const char* text,std::size_t length)noexcept {
    if(!d.sourceInstance||!d.sourceInstance->oneTimeWritten_||d.sourceInstance->parentWritten_||!text||length>=260)return false;
    auto& t=*d.sourceInstance;
    for(std::size_t i=0;i<length;++i)t.parentA4_[i]=text[i];t.parentA4_[length]='\0';
    t.parentWritten_=true;return true;
}
inline bool Owner::preflightSourceTriggerActionNative(const NativeTriggerDescriptor& d)const noexcept {
    if(d.sourceActionNativeWithdrawn)return d.sourceActionAttempted&&!d.actions68&&!d.action&&!d.actionSlotsPublished;
    if(!d.sourceActionAttempted)return !d.actions68&&!d.action&&!d.actionSlotsPublished&&
        !d.sourceActionsAllocated&&!d.sourceActionAllocated&&!d.sourceActionTypeWritten&&!d.sourceActionReaderComplete;
    std::size_t row=2;if(nativeTriggerList3D0_&&nativeTriggerList3D0_->sourceMode)
        for(std::size_t i=0;i<2;++i)if(nativeTriggerList3D0_->descriptorOwners[i].get()==&d)row=i;
    if(row>=2)return false;
    if(d.actionCount04!=1||!d.sourceInstance||!d.sourceInstancePublished||d.actionSlotsPublished>1||
       (d.actions68&&!d.sourceActionsAllocated)||(d.action&&!d.sourceActionAllocated)||
       (d.sourceActionAllocated&&!d.sourceActionsAllocated))return false;
    if(d.actionSlotsPublished&&(!d.actions68||d.actions68[0]!=d.action.get()))return false;
    if(d.sourceActionTypeWritten&&(!d.action||d.action->type00!=22||d.actionSlotsPublished!=1))return false;
    if(d.action&&(d.action->vtable108!=0x3cf0b0||(!d.sourceActionTypeWritten&&d.action->type00!=0)))return false;
    if(d.sourceInstance->serviceAction_.entered&&(!d.action||
       d.sourceInstance->serviceAction_.payload!=&d.action->payload10C||
       !d.sourceInstance->serviceActionCurrentNative()))return false;
    if(d.sourceActionReaderComplete&&(!d.sourceActionTypeWritten||d.action->payload10C.targetNodeIndex!=(row?104u:29u)||
       d.action->payload10C.reset||!d.action->text04[0]||*d.action->text04[0]!='\0'))return false;
    return true;
}
inline bool Owner::retireSourceTriggerActionNative(NativeTriggerDescriptor& d)noexcept {
    if(!preflightSourceTriggerActionNative(d))return false;
    if(d.sourceActionNativeWithdrawn||!d.sourceActionAttempted)return true;
    //Withdraw the exact installed native payload alias BEFORE freeing action storage.
    if(d.sourceInstance&&d.action&&!d.sourceInstance->withdrawServiceActionNative(&d.action->payload10C))return false;
    //Source count98/code/data receipts remain historical; this is not a source clear.
    //Native knowledge withdrawal only. Never inspect unwritten raw array slots.
    d.actionSlotsPublished=0;d.action.reset();d.actions68.reset();d.sourceActionNativeWithdrawn=true;
    //Actual allocation/reader ledgers remain historical on a later count-close
    //refusal; they do not imply still-live source pointer knowledge.
    return true;
}
} //namespace ra_pod_scene50LA
