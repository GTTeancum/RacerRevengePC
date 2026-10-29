//UNCALLED proposal: retained slot74 prefix only. No slot78/7C or frame receipt.
    void advanceSharedSecondaryJointPrefix(SharedTerrainInvocation& call)noexcept {
        auto& lease=call.lease_;auto& out=call.result_;
        if(call.secondaryJointPrefixEntered_||!call.secondaryForcesEntered_||
           call.cut_!=SharedTerrainInvocation::Cut::returned||out.stoppedPC!=0x2190a0||
           !sharedLeaseCurrent(lease)||!lease.producer_->activeSession||
           !actual_||!gridArena_||!gridInputsRegistered(*gridArena_))return;
        call.secondaryJointPrefixEntered_=true;
        const auto& session=*lease.producer_->activeSession;
        const auto slot=lease.activeSlot_;
        if(slot>=session.racerEntities_.size()||slot>=stagedSecondaryContacts_.size()||
           slot>=stagedPods694_.size()||slot>=rosterSlotComplete_.size()||
           slot>=gridArena_->rigs.size()||slot>=gridRigIdentity_.size()||
           !session.actual_.actualChariotModels||
           slot>=session.actual_.actualChariotModels->size())return;
        auto& frame=actual_->frame;auto& joint=frame.bodies.joint;
        const auto& racer=session.racerEntities_[slot];
        const auto* retained=racer.joint360;
        const auto a=stagedSecondaryContacts_[slot].lock();
        if(!retained||!a||!racer.prepared()||!racer.racer.reference00||
           !racer.racer.objectKey||racer.racer.sessionGeneration!=session.generation_||
           racer.racer.kind!=g::OriginalWreckComponentKind50LA::racer27E130||
           !registeredRange(&joint,sizeof(joint))||std::fegetround()!=FE_TONEAREST)return;
        const auto& bindings=retained->nativeBodies_;
        const auto& entity=retained->entity();
        if(!bindings.published||bindings.revoked||bindings.bank!=lease.bank_||
           bindings.frame!=&frame||bindings.slot!=slot||bindings.generation!=session.generation_||
           bindings.racer!=&racer||bindings.racerKey!=racer.racer.objectKey||
           bindings.a!=a.get()||!bindings.b||bindings.b!=racer.body340||
           retained->canonicalNumericAddress()!=&joint||!entity.reference00||
           entity.sessionGeneration!=session.generation_||entity.parent1C!=&racer.racer||
           retained->stage_!=g::OriginalPodSphereOwner50LA::Stage::appended||
           a->revoked_||a->bank_!=lease.bank_||a->frame_!=&frame||a->slot_!=slot||
           a->generation_!=session.generation_||!rosterSlotComplete_[slot]||
           !gridArena_->rigs[slot]||gridArena_->rigs[slot].get()!=gridRigIdentity_[slot]||
           bindings.model!=(*session.actual_.actualChariotModels)[slot].get())return;
        //The actual cold producer binds positive A and positive B. Neither
        //presence Boolean is authority to substitute another owner or NULL.
        out.stoppedPC=0x1b66e8;
        if(!joint.enabled38){out.stoppedPC=0x2190b4;return;}
        out.stoppedPC=0x1b66f4;
        if(!joint.bodyAPresent44||!a->parameters_||
           a->parameters_!=&gridArena_->parameters.bodies.secondary||
           !frame.bodies.main.domain.secondBodyPresent||!frame.bodies.secondary.scenePresent||
           a->physical19C_!=gridArena_->bindings.lifecycle.physical19C||
           a->physical19C_!=gridArena_->rigs[slot]->physical||
           !g::validVehicleSceneCount49W(frame,gridArena_->bindings)||
           !registeredRange(frame.nodes,frame.sceneNodeCount*sizeof(frame.nodes[0])))return;
        const auto physical=static_cast<std::size_t>(a->physical19C_);
        if(physical>=frame.sceneNodeCount)return;
        auto& physicalNode=frame.nodes[physical];
        //28BAB0 getter cache prefix on the authenticated actual node. Ready
        //nodes keep their cached world even if parent/local differ. Only a
        //DIRTY parentless node is joined; hierarchy arithmetic remains unknown.
        const auto pointWorld=[&](g::SceneNode49P& node,g::BodyMatrix48G& numericScene,
                                  std::uint32_t& numericFlags)->bool {
            out.stoppedPC=0x28bad8;
            if(node.sceneFlags190&1u)return true;
            out.stoppedPC=0x28bae8;
            if(node.parent180!=g::SceneNodeIndex49P::none){
                out.stoppedPC=0x28baf4;return false; //Before parent2897B0/product289940.
            }
            out.stoppedPC=0x28bb18;
            if(!registeredRange(&numericScene,sizeof(numericScene))||
               !registeredRange(&numericFlags,sizeof(numericFlags)))return false;
            //Eight original two-word iterations, NOT one atomic matrix copy.
            //Read both source words before the first store, preserving payload
            //bits (including NaN/signzero) without numerical admission.
            for(unsigned pair=0;pair<8u;++pair){
                const unsigned lane=pair*2u;std::uint32_t firstWord,secondWord;
                out.stoppedPC=0x28bb24;
                std::memcpy(&firstWord,node.localC0.v+lane,sizeof(firstWord));
                out.stoppedPC=0x28bb28;
                std::memcpy(&secondWord,node.localC0.v+lane+1u,sizeof(secondWord));
                out.stoppedPC=0x28bb30;
                if(!markSharedDirectQueryEffect(lease))return false;
                std::memcpy(node.world100.v+lane,&firstWord,sizeof(firstWord));
                std::memcpy(numericScene.v+lane,&firstWord,sizeof(firstWord));
                out.stoppedPC=0x28bb34;
                if(!markSharedDirectQueryEffect(lease))return false;
                std::memcpy(node.world100.v+lane+1u,&secondWord,sizeof(secondWord));
                std::memcpy(numericScene.v+lane+1u,&secondWord,sizeof(secondWord));
            }
            out.stoppedPC=0x28bb44;
            const auto flags=static_cast<std::uint8_t>((node.sceneFlags190&0xfeu)|1u);
            out.stoppedPC=0x28bb58;
            if(!markSharedDirectQueryEffect(lease))return false;
            node.sceneFlags190=flags;
            //Native body mirrors are the same logical source writes, not extra
            //guest stores. Preserve their unrelated upper flag bytes.
            numericFlags=(numericFlags&0xffffff00u)|flags;
            return true;
        };
        g::detail::SceneVectorContext49U scratch{};
        g::VisualVector49R anchor{},point{};
        out.stoppedPC=0x1b6700;
        if(!pointWorld(physicalNode,frame.bodies.secondary.root.scene,
            frame.bodies.secondary.sceneFlags190))return;
        //28BB74 reads point input only AFTER any actual cache refresh stores.
        std::memcpy(anchor.v,joint.localAnchor60.v,sizeof(anchor.v));
        out.stoppedPC=0x28bb64; //Numerical point leaf after cache prefix.
        if(!g::detail::cleanMatrixPointPrefix49U(physicalNode.world100,anchor,scratch,point))return;
        out.stoppedPC=0x1b670c;
        if(!markSharedDirectQueryEffect(lease))return;
        std::memcpy(joint.worldAnchor80.v,point.v,sizeof(point.v));

        //Only after actual A80 commit inspect source B48 and its live scene.
        //1B6738/3C would copy fixedAnchorA0 verbatim if a genuine producer
        //had established NULL B. Current retained cold bindings cannot do so.
        out.stoppedPC=0x1b6710;
        if(!joint.bodyBPresent48)return;
        const auto* b=bindings.b;
        if(b->canonicalNumericAddress()!=&frame.bodies.main||b!=stagedPods694_[slot].bodyA0||
           !racer.scene190||racer.scene190!=b->scene288()||
           !racer.scene190->constructed||racer.scene190->released||
           !racer.scene190->entity().reference00||
           racer.scene190->entity().sessionGeneration!=session.generation_||
           racer.scene190->sceneIndex!=gridArena_->bindings.lifecycle.main190)return;
        const auto mainIndex=static_cast<std::size_t>(racer.scene190->sceneIndex);
        if(mainIndex>=frame.sceneNodeCount||mainIndex==physical)return;
        auto& mainNode=frame.nodes[mainIndex];
        out.stoppedPC=0x1b6724;
        if(!pointWorld(mainNode,frame.bodies.main.components.root.scene,
            frame.bodies.mainSphere.sceneFlags190))return;
        std::memcpy(anchor.v,joint.localAnchor70.v,sizeof(anchor.v));
        out.stoppedPC=0x28bb64; //Numerical point leaf after cache prefix.
        if(!g::detail::cleanMatrixPointPrefix49U(mainNode.world100,anchor,scratch,point))return;
        //Copy-only actual surviving VF6.W from this B getter. Do not admit
        //finiteness until1B67C0, after the earlier80/90/D0 source stores.
        float lastPointVf6W;
        std::memcpy(&lastPointVf6W,mainNode.world100.v+11,sizeof(lastPointVf6W));
        out.stoppedPC=0x1b6734;
        if(!markSharedDirectQueryEffect(lease))return;
        std::memcpy(joint.worldAnchor90.v,point.v,sizeof(point.v));
        //Source computes all three differences in stack scratch, W=1, before
        //the one quadword D0 store. Earlier80/90 survive arithmetic rejection.
        g::PodSphereVector49L separation{{0.f,0.f,0.f,1.f}};
        constexpr std::uint32_t subtractPCs[]={0x1b675cu,0x1b6764u,0x1b676cu};
        for(unsigned k=0;k<3;++k){
            out.stoppedPC=subtractPCs[k];
            const float av=joint.worldAnchor80.v[k],bv=joint.worldAnchor90.v[k];
            if(!std::isfinite(av)||!std::isfinite(bv))return;
            volatile float value=av-bv;if(!std::isfinite(value))return;
            separation.v[k]=value;
        }
        out.stoppedPC=0x1b6780;
        if(!markSharedDirectQueryEffect(lease))return;
        joint.separationD0=separation;
        g::PodSphereVector49L tangential{},centripetal{};float squared;
        //W is not read by source cross XYZ. The cross W comes exclusively
        //from the separately captured/validated getter scratch above.
        if(!g::preparePodSphereCentripetalPrefix49L(joint.separationD0,
            frame.bodies.secondary.root.body.angularVelocity.v,
            joint.localAnchor60,lastPointVf6W,squared,tangential,centripetal,out.stoppedPC))return;
        if(!markSharedDirectQueryEffect(lease))return;
        joint.centripetalC0=centripetal; //Actual1B67F0 quadword store.
        out.stoppedPC=0x1b67f4; //Later transpose needs unjoined VF30 provenance.
    }
