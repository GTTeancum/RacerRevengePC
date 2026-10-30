    //Source-only continuation after genuine296190 return. No point readiness
    //is inferred from a native record array or a prepared Wreck owner.
    bool suffixEntered_=false,suffixReturned_=false;
    std::uint16_t suffixStoreMask_=0;
    std::uint32_t suffixPC_=0;
    static constexpr std::array<unsigned,12> suffixOffsets_{{0x34,0x54,0x4c,0x64,0x50,0xac,0xb0,0xb4,0xc4,0xcc,0xd0,0x98}};
    static constexpr std::array<std::uint32_t,12> suffixValues_{{1,1,0,1,1,0x3f800000,0x3f800000,0x3f800000,0x3f800000,0x3ba3d70a,0,0x3f800000}};
    static constexpr std::array<std::uint32_t,12> suffixStorePCs_{{0x166100,0x16610c,0x166114,0x16611c,0x166124,0x16612c,0x166134,0x16613c,0x166144,0x16614c,0x166154,0x16615c}};
    Result continueSourceScalars1660EC()noexcept {
        if(suffixEntered_||nativeWithdrawn_||!sourcePoints_.returned||sourcePoints_.nativeWithdrawn||
           !bodyStored_||!preflightNative()||!storage_||!storage_->raw140||!countedEntityNative())
            return {Status::owner,0x1660ecu};
        suffixEntered_=true;
        for(unsigned i=0;i<suffixOffsets_.size();++i){
            suffixPC_=suffixStorePCs_[i];
            storage_->word(suffixOffsets_[i],suffixValues_[i]);
            suffixStoreMask_|=static_cast<std::uint16_t>(1u<<i);
        }
        //Existing copy-only represented field adapter; no allocations/math or
        //semantic prepared payload admission. Raw stores above retain order.
        applyOriginalWreckContact1660F0_50LA(storage_->physical);
        suffixReturned_=true;suffixPC_=0x166160u;
        return {Status::reached,suffixPC_};
    }
    bool sourceScalarSuffixHistoryNative()const noexcept {
        if(!suffixEntered_)return !suffixReturned_&&!suffixStoreMask_&&!suffixPC_;
        if(!sourcePoints_.returned||!suffixReturned_||suffixStoreMask_!=0xfffu||suffixPC_!=0x166160u)return false;
        if(nativeWithdrawn_)return true;
        if(!storage_||!storage_->raw140)return false;
        for(unsigned i=0;i<suffixOffsets_.size();++i){const auto lane=suffixOffsets_[i]/4;
            if(storage_->known[lane]!=0xffffffffu||storage_->words[lane]!=suffixValues_[i])return false;
        }
        const auto& physical=storage_->physical;
        const auto bits=[](float value)noexcept {std::uint32_t raw;std::memcpy(&raw,&value,4);return raw;};
        if(physical.contactMode34!=1||!physical.generated.impact54||physical.generated.springForce4C||
           !physical.generated.impact.correctPenetration64||!physical.generated.friction50||
           bits(physical.normalVelocityScaleAC)!=0x3f800000u||
           bits(physical.generated.impact.angularImpulseScaleB0)!=0x3f800000u||
           bits(physical.generated.impact.forceTorqueScaleB4)!=0x3f800000u||
           bits(physical.generated.frictionTorqueScaleC4)!=0x3f800000u||
           bits(physical.generated.impact.positionCorrectionScaleCC)!=0x3ba3d70au||
           bits(physical.generated.impact.angularCorrectionScaleD0)!=0u||
           bits(physical.generated.impact.normalForceScale98)!=0x3f800000u)return false;
        return true;
    }
    bool attachEntered_=false,attachReturned_=false,attached_=false,detachedNative_=false;
    std::optional<std::uint32_t> attachLock_;
    std::uint32_t attachPC_=0;
    template<class ActualAttach> Result attachSourceContact141960(ActualAttach&& attach)noexcept {
        if(attachEntered_||nativeWithdrawn_||!suffixReturned_||sourcePoints_.nativeWithdrawn||
           !preflightNative()||!storage_||!base_||!base_->object_||base_->contactChild141960_)
            return {Status::owner,0x166164u};
        auto& child=storage_->entity;auto& parent=base_->object_->entity;
        if(child.parent1C||child.previous10||child.next14||child.firstChild18||child.mask20||child.descendants24)
            return {Status::owner,0x166164u};
        attachEntered_=true;attachLock_=base_->application_->attachmentLock423F58();attachPC_=0x141970u;
        if(!*attachLock_){
            const auto result=attach(parent,child,0u,*attachLock_);
            if(result.status!=OriginalWreckComponentStatus50LA::complete){
                attachPC_=result.sourcePC;return {Status::bounded,attachPC_};
            }
            base_->contactChild141960_=&child;attached_=true;
        }
        attachReturned_=true;attachPC_=0x16616cu;return {Status::reached,attachPC_};
    }
    bool attachmentHistoryNative()const noexcept {
        if(!attachEntered_)return !attachReturned_&&!attached_&&!detachedNative_&&!attachLock_&&!attachPC_&&
            (!base_||!base_->contactChild141960_);
        if(!base_||!base_->object_||!storage_||!suffixReturned_||!attachLock_||
           (detachedNative_&&!attached_)||(*attachLock_&&attached_))return false;
        if(attachReturned_&&((*attachLock_?attached_:!attached_)||attachPC_!=0x16616cu))return false;
        const auto& child=storage_->entity;const bool live=attached_&&!detachedNative_;
        if(base_->contactChild141960_!=(live?&child:nullptr)||!base_->sourceChildChainCurrentNative()||
           child.parent1C!=(live?&base_->object_->entity:nullptr)||
           (!live&&(child.previous10||child.next14))||child.firstChild18||child.descendants24||child.mask20)return false;
        return true;
    }
    template<class ActualDetach> bool detachSourceContactNative(ActualDetach&& detach)noexcept {
        if(detectorBorrower_)return false;
        if(!attempted_||nativeWithdrawn_||!attached_||detachedNative_)return true;
        if(!base_||!base_->application_||base_->application_->attachmentLock423F58()||!preflightNative())return false;
        const auto result=detach(storage_->entity,0u);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
        base_->contactChild141960_=nullptr;detachedNative_=true;return true;
    }
