    bool attachEntered_=false,attachReturned_=false,attached_=false,detachedNative_=false;
    std::optional<std::uint32_t> attachLock_;
    std::uint32_t attachPC_=0;
    template<class ActualAttach> Result attachSourceDetector141960(ActualAttach&& attach)noexcept {
        if(attachEntered_||nativeWithdrawn_||!bindingReturned_||bindingNativeWithdrawn_||!enabled64Stored_||
           !preflightNative()||!storage_||!base_||!base_->object_||base_->detectorChild141960_)
            return {Status::owner,0x1661d0u};
        auto& child=storage_->entity;auto& parent=base_->object_->entity;
        if(child.parent1C||child.previous10||child.next14||child.firstChild18||child.mask20||child.descendants24)
            return {Status::owner,0x1661d0u};
        attachEntered_=true;attachLock_=base_->application_->attachmentLock423F58();attachPC_=0x141970u;
        if(!*attachLock_){
            const auto result=attach(parent,child,0u,*attachLock_);
            if(result.status!=OriginalWreckComponentStatus50LA::complete){
                attachPC_=result.sourcePC;return {Status::bounded,attachPC_};
            }
            base_->detectorChild141960_=&child;attached_=true;
        }
        attachReturned_=true;attachPC_=0x1661d8u;return {Status::reached,attachPC_};
    }
    bool attachmentHistoryNative()const noexcept {
        if(!attachEntered_)return !attachReturned_&&!attached_&&!detachedNative_&&!attachLock_&&!attachPC_&&
            (!base_||!base_->detectorChild141960_);
        if(!base_||!base_->object_||!storage_||!bindingReturned_||!enabled64Stored_||!attachLock_||
           (detachedNative_&&!attached_)||(*attachLock_&&attached_))return false;
        if(attachReturned_&&((*attachLock_?attached_:!attached_)||attachPC_!=0x1661d8u))return false;
        const auto& child=storage_->entity;const bool live=attached_&&!detachedNative_;
        if(base_->detectorChild141960_!=(live?&child:nullptr)||!base_->sourceChildChainCurrentNative()||
           child.parent1C!=(live?&base_->object_->entity:nullptr)||
           (!live&&(child.previous10||child.next14))||child.firstChild18||child.descendants24||child.mask20)return false;
        return true;
    }
    template<class ActualDetach> bool detachSourceDetectorNative(ActualDetach&& detach)noexcept {
        if(directCollisionBorrower_)return false;
        if(!attempted_||nativeWithdrawn_||!attached_||detachedNative_)return true;
        if(!base_||!base_->application_||base_->application_->attachmentLock423F58()||!preflightNative())return false;
        const auto result=detach(storage_->entity,0u);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        base_->detectorChild141960_=nullptr;detachedNative_=true;return true;
    }
