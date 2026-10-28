//Positive native35C adaptation only, not guest Entity/presence publication.
    void advanceSharedSecondaryForces(SharedTerrainInvocation& call)noexcept {
        auto& lease=call.lease_;auto& out=call.result_;
        if(call.secondaryForcesEntered_||!call.terrainResponseEntered_||
           call.cut_!=SharedTerrainInvocation::Cut::returned||out.stoppedPC!=0x219cfc||
           !sharedLeaseCurrent(lease)||!actual_||!gridArena_)return;
        call.secondaryForcesEntered_=true;
        const auto& session=*lease.producer_->activeSession;const auto slot=lease.activeSlot_;
        auto& frame=actual_->frame;auto& secondary=frame.bodies.secondary;
        const auto owner=stagedSecondaryContacts_[slot].lock();
        const auto* registry=lease.producer_->bodyRegistry;
        if(!owner||owner->revoked_||owner->slot_!=slot||owner->bank_!=lease.bank_||
           owner->frame_!=&frame||owner->generation_!=session.generation_||!owner->parameters_||
           !rosterSlotComplete_[slot]||!registry||!registry->prepared()||
           registry->count()!=lease.bank_->count_||!registry->nativeSecondaryIdentity(slot,*owner).value||
           !gridArena_->rigs[slot]||gridArena_->rigs[slot].get()!=gridRigIdentity_[slot]||
           !frame.bodies.main.domain.secondBodyPresent||
           owner->physical19C_!=gridArena_->bindings.lifecycle.physical19C||
           owner->physical19C_!=gridArena_->rigs[slot]->physical||
           !bodyIdentities_[slot].collision||
           actual_->secondaryFilter.identity!=bodyIdentities_[slot].secondary||
           actual_->secondary.resource!=&bodyIdentities_[slot].collision->secondary()||
           !registeredRange(&secondary,sizeof(secondary))||
           !registeredRange(owner->parameters_,sizeof(*owner->parameters_))||
           !registeredRange(&frame.bodies.vehicle838,sizeof(frame.bodies.vehicle838)))return;
        out.stoppedPC=0x219d08;
        if(!frame.bodies.vehicle838){out.stoppedPC=0x219d1c;return;}
        const auto* invocation=lease.producer_->engineEnable;
        if(!invocation||!invocation->context||invocation->session!=&session||
           !invocation->sourceFactorF20Bits)return;
        //Same Race dispatch-capturedF20, not secondary/main schedule. Kept
        //without consuming it early; original use is in later2188B0 phases.
        call.secondaryFactorF20Bits_=*invocation->sourceFactorF20Bits;
        out.stoppedPC=0x2188c8;
        const auto app=session.actual_.applicationEntities;
        if(!app||app->nativeOwnerThread_!=std::this_thread::get_id()||std::fegetround()!=FE_TONEAREST)return;
        constexpr std::uint32_t values[]={0x42820000u,0x43af0000u,0x44bb8000u,0x43960000u,
            0x457a0000u,0x42480000u,0x42f00000u,0u,0x40a00000u};
        constexpr std::uint32_t stores[]={0x2188e0u,0x218900u,0x218924u,0x218944u,
            0x218964u,0x218984u,0x2189a4u,0x2189c4u,0x2189e0u};
        for(unsigned i=0;i<9;++i){
            out.stoppedPC=stores[i];if(!markSharedDirectQueryEffect(lease))return;
            auto& word=app->nativeSecondaryForceLazyWords_[i];
            if(!word)word=values[i];if(*word!=values[i])return;
        }
        //Only after all nine actual lazy stores read actual post-response104.
        out.stoppedPC=0x218a3c;
        const bool attached=frame.bodies.main.components.probe348.field104!=0;
        const auto* gravity=attached?&frame.fields.gravity378:&frame.fields.gravity37C;
        if(!registeredRange(gravity,sizeof(*gravity)))return;
        const float mass=owner->parameters_->body.mass;
        if(!std::isfinite(mass)||!std::isfinite(*gravity))return;
        volatile float gravityY=(-mass)*(*gravity);if(!std::isfinite(gravityY))return;
        const float force[]={0.f,gravityY,0.f};
        constexpr std::uint32_t attachedStores[]={0x218a94u,0x218aa4u,0x218ab8u};
        constexpr std::uint32_t detachedStores[]={0x218b04u,0x218b14u,0x218b24u};
        const auto add=[&](float& lane,float value,std::uint32_t pc){
            out.stoppedPC=pc;if(!std::isfinite(lane)||!std::isfinite(value))return false;
            volatile float sum=lane+value;
            if(!std::isfinite(sum)||!markSharedDirectQueryEffect(lease))return false;
            lane=sum;return true;
        };
        for(unsigned k=0;k<3;++k)
            if(!add(secondary.root.loads.force.v[k],force[k],attached?attachedStores[k]:detachedStores[k]))return;
        out.stoppedPC=0x218b28;float low,high;
        std::memcpy(&low,&*app->nativeSecondaryForceLazyWords_[1],sizeof(low));
        std::memcpy(&high,&*app->nativeSecondaryForceLazyWords_[2],sizeof(high));
        g::Vector3_48C torque;
        if(!g::prepareSecondaryDampingPrefix49M(secondary.cachedSpeed34,low,high,
            secondary.root.body.angularVelocity,torque))return;
        constexpr std::uint32_t torqueStores[]={0x218c34u,0x218c44u,0x218c54u};
        for(unsigned k=0;k<3;++k)
            if(!add(secondary.root.loads.torque.v[k],torque.v[k],torqueStores[k]))return;
        //Actual next28B1A0 relative physical19C/main190 getter not yet joined.
        //No fake ready caches, joint/368 completion or secondary phase change.
        out.stoppedPC=0x218c58;
    }
