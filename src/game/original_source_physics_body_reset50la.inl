//Included inside OriginalSourcePhysicsBody50LA. Distinct post-membership
//167A50 -> exact dispatch+A8/167AD0 occurrence, never constructor replay.
    struct SourceBodyReset167A50 {
        struct Image {
            std::array<std::array<std::uint32_t,4>,8> vectors{};
            std::uint32_t speed34=0;
            std::array<std::array<std::uint32_t,3>,6> histories{};
            std::array<std::array<std::uint32_t,4>,2> quaternions{};
        } before,after;
        const void* updateOccurrence=nullptr;
        const void* bodyIdentity=nullptr;
        const void* modelOwnerHistory=nullptr;
        const void* modelIdentity=nullptr;
        const void* modelRegistrationHistory=nullptr;
        const void* vectorGlobalIdentity=nullptr;
        const void* quaternionGlobalIdentity=nullptr;
        std::uint32_t modelReturnPC=0,dispatch08=0,slotAC=0,slotA8=0,pc=0;
        std::array<std::array<std::uint32_t,4>,8> vectorReads{};
        std::array<std::array<std::uint32_t,4>,2> quaternionReads{};
        std::array<std::uint32_t,6> zeroCallPC{},zeroReturnPC{};
        std::uint8_t vectorReadMask=0,vectorStoreMask=0,zeroHistoryMask=0,quaternionReadMask=0,quaternionStoreMask=0;
        bool entered=false,speedCleared=false,tailEntered=false,returned=false,nativeWithdrawn=false;
    } bodyReset_;
    SourceBodyReset167A50::Image bodyResetImageNative()const noexcept {
        SourceBodyReset167A50::Image image;
        const auto& p=body_->physical;const auto& b=p.body;const auto& r=b.root;
        const Vector3_48C* xyz[]={&r.loads.force,&r.loads.torque,&r.body.linearAcceleration,&r.body.angularAcceleration,
            &r.body.velocity,&r.localVelocity80,&r.previousVelocity90,&r.body.angularVelocity};
        const float* w[]={&b.force180W,&b.torque190W,&p.supplement.linearAccelerationC0W,&p.supplement.angularAccelerationD0W,
            &b.velocity70W,&p.supplement.localVelocity80W,&p.supplement.previousVelocity90W,&b.angularVelocityB0W};
        for(unsigned i=0;i<8;++i){std::memcpy(image.vectors[i].data(),xyz[i]->v,12);std::memcpy(&image.vectors[i][3],w[i],4);}
        std::memcpy(&image.speed34,&b.cachedSpeed34,4);
        const Vector3_48C* history[]={&p.supplement.positionDerivative2B8,&p.supplement.linearAcceleration2D0,
            &p.supplement.angularAcceleration308,&r.body.previousPositionDerivative,&r.body.previousLinearAcceleration,
            &r.body.previousAngularAcceleration};
        for(unsigned i=0;i<6;++i)std::memcpy(image.histories[i].data(),history[i]->v,12);
        std::memcpy(image.quaternions[0].data(),r.body.orientationDerivative.v,16);
        std::memcpy(image.quaternions[1].data(),r.body.previousOrientationDerivative.v,16);return image;
    }
    static bool bodyResetImagesEqualNative(const SourceBodyReset167A50::Image& a,const SourceBodyReset167A50::Image& b)noexcept {
        return a.vectors==b.vectors&&a.speed34==b.speed34&&a.histories==b.histories&&a.quaternions==b.quaternions;
    }
    bool bodyResetHistoryNative()const noexcept {
        const auto& h=bodyReset_;
        if(!h.entered)return !h.updateOccurrence&&!h.bodyIdentity&&!h.modelOwnerHistory&&!h.modelIdentity&&
            !h.modelRegistrationHistory&&!h.vectorGlobalIdentity&&!h.quaternionGlobalIdentity&&!h.modelReturnPC&&
            !h.dispatch08&&!h.slotAC&&!h.slotA8&&!h.pc&&!h.vectorReadMask&&!h.vectorStoreMask&&!h.zeroHistoryMask&&
            !h.quaternionReadMask&&!h.quaternionStoreMask&&!h.speedCleared&&!h.tailEntered&&!h.returned&&!h.nativeWithdrawn;
        if(!h.updateOccurrence||!h.bodyIdentity||!h.modelOwnerHistory||!h.modelIdentity||!h.modelRegistrationHistory||
           !h.vectorGlobalIdentity||!h.quaternionGlobalIdentity||h.modelReturnPC!=0x28ea14u||h.dispatch08!=0x3ce000u||
           h.slotAC!=0x167a50u||h.slotA8!=0x167ad0u||h.pc!=0x167b7cu||!h.returned||!h.speedCleared||!h.tailEntered||
           h.vectorReadMask!=255u||h.vectorStoreMask!=255u||h.zeroHistoryMask!=63u||
           h.quaternionReadMask!=255u||h.quaternionStoreMask!=255u||h.after.speed34)return false;
        if(!bodyResetImagesEqualNative(h.before,h.after))return false; //Admitted constructor/reset lanes were unchanged up to this call.
        for(unsigned i=0;i<6;++i)if(h.zeroCallPC[i]!=(i%3==0?0x167af8u:i%3==1?0x167b38u:0x167b48u)||h.zeroReturnPC[i]!=0x306538u)return false;
        for(unsigned i=0;i<8;++i)if(h.after.vectors[i]!=h.vectorReads[i]||h.vectorReads[i]!=h.vectorReads[0])return false;
        for(const auto& v:h.after.histories)for(auto word:v)if(word)return false;
        if(h.after.quaternions!=h.quaternionReads||h.quaternionReads[0]!=h.quaternionReads[1])return false;
        if(h.nativeWithdrawn)return nativeWithdrawn_; //Retained identities only, no released owner/global reads.
        if(nativeWithdrawn_||!body_||!base_||!base_->application_||!modelBorrow_||
           h.bodyIdentity!=&body_->entity||h.modelOwnerHistory!=modelBorrow_||
           !body_->source.model288||h.modelIdentity!=*body_->source.model288||h.modelRegistrationHistory!=&modelBorrow_->modelObserver_||
           h.vectorGlobalIdentity!=globals_.reset.vector433B80||h.quaternionGlobalIdentity!=globals_.reset.quaternion382B60||
           !bodyResetImagesEqualNative(h.after,bodyResetImageNative()))return false;
        return !std::memcmp(h.vectorReads[0].data(),globals_.reset.vector433B80,16)&&
            !std::memcmp(h.quaternionReads[0].data(),globals_.reset.quaternion382B60,16);
    }
    Result resetSourceBody167A50(const void* updateOccurrence,OriginalSourcePhysicsModel50LA& actualModel)noexcept {
        auto& h=bodyReset_;
        //Model first-registration prefix/published19C is insufficient. Require
        //its genuine28EA14 return for this same166680 configuration occurrence.
        if(!updateOccurrence||h.entered||nativeWithdrawn_||nativeRetired_||!body_||!base_||!base_->object_||
           !attachReturned_||detachedNative_||!constructorReturned_||!published_||!disableReturned_||
           modelBorrow_!=&actualModel||!modelBorrowCurrentNative()||!preflightNative()||
           base_->object_->bodyD0!=std::optional<C*>(&body_->entity)||body_->dispatch08!=0x3ce000u||
           !actualModel.modelObserver_.returned||actualModel.modelObserver_.pc!=0x28ea14u||
           actualModel.modelObserver_.updateOccurrence!=updateOccurrence||!actualModel.sourceModelObserverHistoryNative())
            return {Status::owner,0x167a50u};
        h.entered=true;h.updateOccurrence=updateOccurrence;h.bodyIdentity=&body_->entity;
        h.modelOwnerHistory=&actualModel;h.modelIdentity=*body_->source.model288;
        h.modelRegistrationHistory=&actualModel.modelObserver_;h.modelReturnPC=actualModel.modelObserver_.pc;
        h.vectorGlobalIdentity=globals_.reset.vector433B80;h.quaternionGlobalIdentity=globals_.reset.quaternion382B60;
        h.dispatch08=body_->dispatch08;h.slotAC=0x167a50u;h.before=bodyResetImageNative();
        auto& p=body_->physical;auto& b=p.body;auto& r=b.root;
        Vector3_48C* xyz[]={&r.loads.force,&r.loads.torque,&r.body.linearAcceleration,&r.body.angularAcceleration,
            &r.body.velocity,&r.localVelocity80,&r.previousVelocity90,&r.body.angularVelocity};
        float* w[]={&b.force180W,&b.torque190W,&p.supplement.linearAccelerationC0W,&p.supplement.angularAccelerationD0W,
            &b.velocity70W,&p.supplement.localVelocity80W,&p.supplement.previousVelocity90W,&b.angularVelocityB0W};
        //Eight distinct LQC2/SQC2 pairs. Every full lane is copied bitwise;
        //no finite/rounding admission, arithmetic, cached-vector reuse or V0.
        for(unsigned i=0;i<8;++i){h.pc=0x167a54u+12u*i;
            std::memcpy(h.vectorReads[i].data(),globals_.reset.vector433B80,16);h.vectorReadMask|=std::uint8_t(1u<<i);
            h.pc=0x167a58u+12u*i;std::memcpy(xyz[i]->v,h.vectorReads[i].data(),12);std::memcpy(w[i],&h.vectorReads[i][3],4);
            h.vectorStoreMask|=std::uint8_t(1u<<i);}
        h.pc=0x167ab0u;std::memset(&b.cachedSpeed34,0,4);h.speedCleared=true;
        h.pc=0x167abcu;h.slotA8=0x167ad0u;h.tailEntered=true;
        Vector3_48C* history[]={&p.supplement.positionDerivative2B8,&p.supplement.linearAcceleration2D0,
            &p.supplement.angularAcceleration308,&r.body.previousPositionDerivative,&r.body.previousLinearAcceleration,
            &r.body.previousAngularAcceleration};
        float* quaternions[]={r.body.orientationDerivative.v,r.body.previousOrientationDerivative.v};
        for(unsigned i=0;i<2;++i){
            //Exact167AD0 two-iteration layout.306478 fills only12 bytes,
            //not the complete16-byte quaternion or a contiguous native blob.
            //Recovered306478 aligned/unaligned size12 paths both write exactly
            //these12 zero bytes and return306538; native spans are admitted above.
            h.pc=h.zeroCallPC[3*i]=0x167af8u;std::memset(history[3*i]->v,0,12);h.zeroReturnPC[3*i]=0x306538u;h.zeroHistoryMask|=std::uint8_t(1u<<(3*i));
            for(unsigned lane=0;lane<4;++lane){
                constexpr std::uint32_t loadPC[4]={0x167b04u,0x167b1cu,0x167b28u,0x167b34u};
                constexpr std::uint32_t storePC[4]={0x167b14u,0x167b20u,0x167b2cu,0x167b3cu};
                h.pc=loadPC[lane];std::memcpy(&h.quaternionReads[i][lane],static_cast<const unsigned char*>(globals_.reset.quaternion382B60)+4*lane,4);
                h.quaternionReadMask|=std::uint8_t(1u<<(4*i+lane));
                h.pc=storePC[lane];std::memcpy(quaternions[i]+lane,&h.quaternionReads[i][lane],4);h.quaternionStoreMask|=std::uint8_t(1u<<(4*i+lane));}
            h.zeroCallPC[3*i+1]=0x167b38u;h.pc=0x306478u;std::memset(history[3*i+1]->v,0,12);h.zeroReturnPC[3*i+1]=0x306538u;h.zeroHistoryMask|=std::uint8_t(1u<<(3*i+1));
            h.pc=h.zeroCallPC[3*i+2]=0x167b48u;std::memset(history[3*i+2]->v,0,12);h.zeroReturnPC[3*i+2]=0x306538u;h.zeroHistoryMask|=std::uint8_t(1u<<(3*i+2));
        }
        h.after=bodyResetImageNative();h.returned=true;h.pc=0x167b7cu;
        return {Status::reached,h.pc}; //Tailcallee JR167B74/delay167B78; original caller16676C.
    }
