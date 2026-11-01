    struct SourceParentAttachment27B5C8 {
        const void* updateOccurrence=nullptr;const void* parentOwnerIdentity=nullptr;
        C* parent=nullptr;const C* childIdentity=nullptr;
        std::uint32_t zone=0,mode=0,updateReturnPC=0,pc=0;
        std::optional<std::uint32_t> lock;
        std::optional<OriginalWreckComponentResult50LA> call;
        bool entered=false,returned=false,attached=false,nativeDetached=false;
    } parentAttachment_;
    template<class ActualAttach>
    Result attachSourcePhysicsParent27B5C8(const void* updateOccurrence,std::uint32_t updateReturnPC,
        const void* parentOwner,C& parent,std::uint32_t zone,ActualAttach&& attach)noexcept {
        auto& h=parentAttachment_;
        if(h.entered||!updateOccurrence||updateReturnPC!=0x16677cu||!parentOwner||zone>=10||
           !sourceConfigurationTail_.returned||sourceConfigurationTail_.occurrence!=updateOccurrence||
           nativeWithdrawn_||nativeRetired_||!object_||!constructorReturned_||!wrapperPublished_||
           !preflightNative()||parent.kind!=K::staticModels27B610||parent.reference00!=1||
           parent.sessionGeneration!=generation_||parent.objectKey!=key(&parent)||
           parent.parent1C||parent.previous10||parent.next14||parent.mask20||parent.nativeFlags29||
           parent.category28!=0xffu||parent.flags29!=3u||
           &parent==&object_->entity)return {Status::owner,0x27b5c8u};
        h.entered=true;h.updateOccurrence=updateOccurrence;h.updateReturnPC=updateReturnPC;
        h.parentOwnerIdentity=parentOwner;h.parent=&parent;h.childIdentity=&object_->entity;
        h.zone=zone;h.mode=2u;h.pc=0x141970u;h.lock=application_->attachmentLock423F58();
        const auto result=attach(parent,object_->entity,h.mode,*h.lock);h.call=result;h.pc=result.sourcePC;
        if(result.status==OriginalWreckComponentStatus50LA::complete){
            h.attached=true;
        }else if(result.status!=OriginalWreckComponentStatus50LA::attachmentLocked)return {Status::bounded,h.pc};
        //Actual wrapper ignores141960 V0, including the lockedNULL branch.
        h.returned=true;h.pc=0x27b5d0u;return {Status::reached,h.pc};
    }
    bool sourceParentAttachmentHistoryNative()const noexcept {
        const auto& h=parentAttachment_;
        if(!h.entered){
            if(h.updateOccurrence||h.parentOwnerIdentity||h.parent||h.childIdentity||h.zone||h.mode||
               h.updateReturnPC||h.pc||h.lock||h.call||h.returned||h.attached||h.nativeDetached)return false;
            return !object_||(!object_->entity.parent1C&&!object_->entity.previous10&&
                !object_->entity.next14&&!object_->entity.mask20);
        }
        if(!h.updateOccurrence||h.updateOccurrence!=sourceConfigurationTail_.occurrence||
           !sourceConfigurationTail_.returned||!h.parentOwnerIdentity||!h.parent||!h.childIdentity||
           h.zone>=10||h.mode!=2u||h.updateReturnPC!=0x16677cu||!h.lock||!h.call||
           h.attached!=(h.call->status==OriginalWreckComponentStatus50LA::complete)||
           h.returned!=(h.attached||h.call->status==OriginalWreckComponentStatus50LA::attachmentLocked)||
           (h.nativeDetached&&!h.attached)||
           (h.returned?h.pc!=0x27b5d0u:h.pc!=h.call->sourcePC))return false;
        if(h.attached&&(*h.lock||h.call->sourcePC!=0x141a58u))return false;
        if(h.call->status==OriginalWreckComponentStatus50LA::attachmentLocked&&(!*h.lock||h.call->sourcePC!=0x14197cu))return false;
        if(nativeWithdrawn_)return !object_&&(!h.attached||h.nativeDetached);
        if(!object_||h.childIdentity!=&object_->entity)return false;
        const auto& child=object_->entity;
        if(child.mask20!=(h.attached?2u:0u))return false; //Native detach preserves source mask.
        if(!h.attached||h.nativeDetached)return !child.parent1C&&!child.previous10&&!child.next14;
        if(child.parent1C!=h.parent||h.parent->kind!=K::staticModels27B610||
           h.parent->sessionGeneration!=generation_||h.parent->reference00!=1)return false;
        if(child.previous10){if(child.previous10->parent1C!=h.parent||child.previous10->next14!=&child)return false;}
        else if(h.parent->firstChild18!=&child)return false;
        if(child.next14&&(child.next14->parent1C!=h.parent||child.next14->previous10!=&child))return false;
        return (h.parent->descendants24&(child.mask20|child.descendants24))==(child.mask20|child.descendants24);
    }
    template<class ActualDetach>
    bool detachSourcePhysicsParentNative(ActualDetach&& detach)noexcept {
        auto& h=parentAttachment_;
        if(!h.attached||h.nativeDetached)return true;
        if(physicsIndexBorrower_||nativeWithdrawn_||!object_||!application_||application_->attachmentLock423F58()||!preflightNative())return false;
        const auto result=detach(object_->entity,0u);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        h.nativeDetached=true;return true; //Native removal, not source141A70 or wrapper progress.
    }
