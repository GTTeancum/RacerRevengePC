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
        //Actual28B1A0: physical19C refresh/copy precedes main190 inverse
        //position. Current native scene owners, never a temporary graph.
        out.stoppedPC=0x218c58;
        if(!g::validVehicleSceneCount49W(frame,gridArena_->bindings)||
           !registeredRange(frame.nodes,frame.sceneNodeCount*sizeof(frame.nodes[0])))return;
        const auto physical=static_cast<std::size_t>(owner->physical19C_);
        const auto& racer=session.racerEntities_[slot];
        if(physical>=frame.sceneNodeCount)return;
        g::SceneGraph49P graph{frame.nodes,frame.sceneNodeCount};auto& physicalNode=frame.nodes[physical];
        if(!g::validateSceneGraph49P(graph)||physicalNode.parent180!=g::SceneNodeIndex49P::none)return;
        out.stoppedPC=0x28b1e0;if(!markSharedDirectQueryEffect(lease))return;
        if(!g::refreshSceneWorld49P(graph,owner->physical19C_))return;
        secondary.root.scene=physicalNode.world100;
        secondary.sceneFlags190=(secondary.sceneFlags190&0xffffff00u)|physicalNode.sceneFlags190;
        g::VisualVector49R position{};std::memcpy(position.v,physicalNode.world100.v+12,3*sizeof(float));
        //Source outputW is authored by the later inverse-point helper, not
        //inferred from this XYZ-only translation copy.
        out.stoppedPC=0x28b288;
        if(!racer.prepared()||!racer.body340||racer.body340!=stagedPods694_[slot].bodyA0||
           racer.body340->canonicalNumericAddress()!=&frame.bodies.main||
           !racer.scene190||racer.scene190!=racer.body340->scene288()||
           !racer.scene190->constructed||racer.scene190->released||
           !racer.scene190->entity().reference00||
           racer.scene190->entity().sessionGeneration!=session.generation_||
           racer.scene190->sceneIndex!=gridArena_->bindings.lifecycle.main190)return;
        const auto mainIndex=static_cast<std::size_t>(racer.scene190->sceneIndex);
        if(mainIndex>=frame.sceneNodeCount||mainIndex==physical)return;
        auto& mainNode=frame.nodes[mainIndex];auto& mainRoot=frame.bodies.main.components.root;
        if(mainNode.parent180!=g::SceneNodeIndex49P::none||
           !registeredRange(&mainRoot.scene,sizeof(mainRoot.scene))||
           !registeredRange(&frame.bodies.mainSphere.sceneFlags190,sizeof(frame.bodies.mainSphere.sceneFlags190)))return;
        const auto refreshMain=[&]()->bool {
            if(!sharedLeaseCurrent(lease)||!g::validateSceneGraph49P(graph)||
               mainNode.parent180!=g::SceneNodeIndex49P::none||!markSharedDirectQueryEffect(lease))return false;
            if(!g::refreshSceneWorld49P(graph,racer.scene190->sceneIndex))return false;
            mainRoot.scene=mainNode.world100;
            auto& flags=frame.bodies.mainSphere.sceneFlags190;flags=(flags&0xffffff00u)|mainNode.sceneFlags190;
            return true;
        };
        if(!refreshMain())return;
        g::detail::SceneVectorContext49U scratch{};g::VisualVector49R relative{};
        if(!g::detail::cleanMatrixInversePointPrefix49U(mainNode.world100,position,scratch,relative))return;
        //Only NOW read the actual target2F0. Its immutable address was bound
        //at actual2119C4/212538 producer; value stays current in this Bank slot.
        out.stoppedPC=0x218c68;
        if(owner->revoked_||!owner->targetRelative2F0_||
           !registeredRange(owner->targetRelative2F0_,sizeof(*owner->targetRelative2F0_)))return;
        g::Vector3_48C relativeXYZ,error;float length;
        std::memcpy(relativeXYZ.v,relative.v,sizeof(relativeXYZ.v));
        if(!g::prepareSecondaryRestorationError49M(*owner->targetRelative2F0_,relativeXYZ,error,length))return;
        out.stoppedPC=0x218ce0;
        if(!refreshMain())return; //Distinct actual28B9B0 even though usually ready now.
        g::VisualVector49R localError{},worldError{};
        std::memcpy(localError.v,error.v,sizeof(error.v));localError.v[3]=1.f;
        if(!g::detail::cleanMatrixDirectionPrefix49U(mainNode.world100,localError,scratch,worldError))return;
        out.stoppedPC=0x218cf8;g::Vector3_48C worldXYZ,direction;
        std::memcpy(worldXYZ.v,worldError.v,sizeof(worldXYZ.v));
        if(!g::prepareSecondaryRestorationDirection49M(worldXYZ,direction))return;
        out.stoppedPC=0x218d20;
        const auto& coefficient=app->nativeSecondaryForceLazyWords_[3];if(!coefficient)return;
        float stiffness;std::memcpy(&stiffness,&*coefficient,sizeof(stiffness));
        if(!std::isfinite(stiffness)||!std::isfinite(length))return;
        volatile float strength=stiffness*length;if(!std::isfinite(strength))return;
        g::Vector3_48C restoration;
        for(unsigned k=0;k<3;++k){volatile float value=direction.v[k]*strength;
            if(!std::isfinite(value))return;restoration.v[k]=value;}
        constexpr std::uint32_t restorationStores[]={0x218d6cu,0x218d7cu,0x218d8cu};
        for(unsigned k=0;k<3;++k)
            if(!add(secondary.root.loads.force.v[k],restoration.v[k],restorationStores[k]))return;
        out.stoppedPC=0x218d90;
        if(!registeredRange(&frame.bodies.main.vehicleState40,sizeof(frame.bodies.main.vehicleState40)))return;
        if(frame.bodies.main.vehicleState40){
            out.stoppedPC=0x218efc;
            //Actual same-Application private producer custody, not merely a
            //nonnull frame borrow. Never fall back to embedded/per-racer noise.
            if(!sharedSecondaryNoiseOwner_||
               sharedSecondaryNoiseEntities_.lock()!=app||
               frame.borrowedNoise50KG!=sharedSecondaryNoiseOwner_.get())return;
            const auto& frequencyWord=app->nativeSecondaryForceLazyWords_[8];
            if(!frequencyWord)return;
            float frequency;std::memcpy(&frequency,&*frequencyWord,sizeof(frequency));
            out.stoppedPC=0x218f04;
            const float time=secondary.time864;
            out.stoppedPC=0x218f0c;
            if(!std::isfinite(frequency)||!std::isfinite(time))return;
            volatile float coordinate=frequency*time;
            if(!std::isfinite(coordinate))return;
            auto& noise=sharedSecondaryNoiseOwner_->noise;
            out.stoppedPC=0x14eecc;
            if(noise.initializationIncomplete49M)return; //No retry of a real partial prefix.
            if(!noise.initialized){
                out.stoppedPC=0x14eee0;
                if(!markSharedDirectQueryEffect(lease))return;
                //Native poison is distinct from the guest lazy flag. Publish
                //the source flag clear NOW, not after a staged table build.
                noise.initializationIncomplete49M=1;
                noise.initialized=true;
                auto& raw=sharedSecondaryNoiseOwner_->sharedRandomState;
                const auto draw=[&](std::uint32_t& result)->bool {
                    //308030's return-delay raw store. No local stream, seed,
                    //batched jump or prepared context; every draw is visible.
                    const std::uint32_t next=raw*0x41c64e6du+0x3039u;
                    out.stoppedPC=0x30805c;
                    if(!markSharedDirectQueryEffect(lease))return false;
                    raw=next;result=next&0x7fffffffu;return true;
                };
                for(std::uint32_t i=0;i<256u;++i){
                    //14F018 call-delay permutation store precedes its draw.
                    out.stoppedPC=0x14f01c;
                    if(!markSharedDirectQueryEffect(lease))return;
                    noise.permutation[i]=i;
                    std::uint32_t firstDraw;
                    if(!draw(firstDraw))return;
                    const auto centered=static_cast<std::int32_t>(firstDraw&511u)-256;
                    out.stoppedPC=0x14f040;
                    volatile float converted=static_cast<float>(centered);
                    out.stoppedPC=0x14f054;
                    volatile float gradient=converted/256.f;
                    out.stoppedPC=0x14f058;
                    if(!markSharedDirectQueryEffect(lease))return;
                    noise.gradient1D[i]=gradient;
                    //Exact established1-D projection: nonretained2-D and3-D
                    //gradient arithmetic/normalization have no dependency
                    //back to p/g1/raw. Keep ALL two then three308030 effects;
                    //do not claim their omitted arrays/helpers were executed.
                    for(unsigned component=0;component<2u;++component){
                        out.stoppedPC=0x14f060;std::uint32_t unused;
                        if(!draw(unused))return;
                    }
                    for(unsigned component=0;component<3u;++component){
                        out.stoppedPC=0x14f0f8;std::uint32_t unused;
                        if(!draw(unused))return;
                    }
                }
                for(std::uint32_t i=255u;i!=0u;--i){
                    //14F1DC saves p[i] in the RNG-call delay slot.
                    out.stoppedPC=0x14f1dc;
                    const auto saved=noise.permutation[i];std::uint32_t drawn;
                    if(!draw(drawn))return;
                    const auto j=drawn&255u;
                    out.stoppedPC=0x14f204;const auto other=noise.permutation[j];
                    out.stoppedPC=0x14f214;
                    if(!markSharedDirectQueryEffect(lease))return;
                    noise.permutation[i]=other;
                    //Separate source branch-delay store, even when i==j.
                    out.stoppedPC=0x14f224;
                    if(!markSharedDirectQueryEffect(lease))return;
                    noise.permutation[j]=saved;
                }
                //14F228..14F2C8 mirrors are outside this established1-D
                //projection: both14EEC0 indices mask255, p remains0..255.
                //Completion is the projected return, not full2-D/3-D state.
                out.stoppedPC=0x14f2e8;
                if(!sharedLeaseCurrent(lease))return;
                noise.initializationIncomplete49M=0;
            }
            out.stoppedPC=0x14eef4;
            volatile float shifted=4096.f+coordinate;
            out.stoppedPC=0x14eef8;
            if(!std::isfinite(shifted)||shifted< -0x1p31f||shifted>=0x1p31f)return;
            const auto cell=static_cast<std::int32_t>(shifted);
            out.stoppedPC=0x14ef04;
            if(!markSharedDirectQueryEffect(lease))return;
            noise.scratchCell=cell; //Actual shared4332A8, before ANY table admission.
            const auto first=static_cast<std::uint32_t>(cell)&255u;
            out.stoppedPC=0x14ef3c;
            volatile float cellAsFloat=static_cast<float>(cell);
            const auto second=(first+1u)&255u;
            out.stoppedPC=0x14ef50;
            volatile float fraction=shifted-cellAsFloat;
            out.stoppedPC=0x14ef58;
            const auto p0=noise.permutation[first];
            out.stoppedPC=0x14ef5c;
            volatile float previous=fraction-1.f;
            out.stoppedPC=0x14ef64;
            volatile float twice=2.f*fraction;
            out.stoppedPC=0x14ef78;
            volatile float curveFactor=3.f-twice;
            out.stoppedPC=0x14ef7c;
            if(p0>=256u)return;
            const float g0=noise.gradient1D[p0];
            if(!std::isfinite(g0)||g0< -1.f||g0>0x1.fep-1f)return;
            out.stoppedPC=0x14ef88;
            volatile float left=fraction*g0;
            out.stoppedPC=0x14ef90;
            const auto p1=noise.permutation[second];
            out.stoppedPC=0x14ef94;
            volatile float square=fraction*fraction;
            out.stoppedPC=0x14ef9c;
            volatile float curve=curveFactor*square;
            out.stoppedPC=0x14efac;
            if(p1>=256u)return;
            const float g1=noise.gradient1D[p1];
            if(!std::isfinite(g1)||g1< -1.f||g1>0x1.fep-1f)return;
            out.stoppedPC=0x14efb0;
            volatile float right=previous*g1;
            out.stoppedPC=0x14efb4;
            volatile float delta=right-left;
            out.stoppedPC=0x14efb8;
            volatile float weighted=curve*delta;
            out.stoppedPC=0x14efc0;
            volatile float value=left+weighted; //Return delay slot, no FMA.
            if(!std::isfinite(value))return; //Scratch remains committed on failure.
            const float returned=value;std::uint32_t returnedBits;
            std::memcpy(&returnedBits,&returned,sizeof(returnedBits));
            call.secondaryNoiseF0Bits_=returnedBits;
            out.stoppedPC=0x218f10;
            //Same lexical invocation's real14EEC0 return, not an injected value
            //or a new entry that could replay its shared scratch prefix.
            if(!call.secondaryNoiseF0Bits_)return;
            float noiseValue;
            std::memcpy(&noiseValue,&*call.secondaryNoiseF0Bits_,sizeof(noiseValue));
            out.stoppedPC=0x218f1c;
            volatile float halfNoise=.5f*noiseValue;
            out.stoppedPC=0x218f2c;
            const auto& torqueWord=app->nativeSecondaryForceLazyWords_[6];
            if(!torqueWord)return;
            float noiseTorqueScale;std::memcpy(&noiseTorqueScale,&*torqueWord,sizeof(noiseTorqueScale));
            out.stoppedPC=0x218f30;
            volatile float h=.5f+halfNoise;
            out.stoppedPC=0x218f3c;
            const float torqueSpeed=frame.bodies.main.cachedSpeed34;
            out.stoppedPC=0x218f40;
            if(!std::isfinite(h)||!std::isfinite(noiseTorqueScale))return;
            volatile float torqueFactor=h*noiseTorqueScale;
            out.stoppedPC=0x218f50;
            if(!std::isfinite(torqueSpeed)||!std::isfinite(torqueFactor))return;
            volatile float torqueZ=torqueSpeed*torqueFactor;
            if(!std::isfinite(torqueZ))return;
            //Source218F18/20 authored +zero. Their real add/stores must not be
            //optimized into skipped lanes, including signed-zero effects.
            if(!add(secondary.root.loads.torque.v[0],0.f,0x218f70))return;
            if(!add(secondary.root.loads.torque.v[1],0.f,0x218f80))return;
            if(!add(secondary.root.loads.torque.v[2],torqueZ,0x218f90))return;
            out.stoppedPC=0x218f98;
            const auto& forceWord=app->nativeSecondaryForceLazyWords_[7];
            if(!forceWord)return;
            float noiseForceScale;std::memcpy(&noiseForceScale,&*forceWord,sizeof(noiseForceScale));
            out.stoppedPC=0x218f9c;
            const float forceSpeed=frame.bodies.main.cachedSpeed34; //Actual distinct source reload.
            out.stoppedPC=0x218fa0;
            if(!std::isfinite(noiseForceScale))return;
            volatile float forceFactor=h*noiseForceScale;
            out.stoppedPC=0x218fa4;
            if(!std::isfinite(forceSpeed)||!std::isfinite(forceFactor))return;
            volatile float forceX=forceSpeed*forceFactor;
            if(!std::isfinite(forceX))return;
            g::VisualVector49R localNoiseForce{},worldNoiseForce{};
            localNoiseForce.v[0]=forceX;
            //Only XYZ are original authored/read lanes here. Direction helper
            //does not consume inputW; zero initialization is not a guestW claim.
            out.stoppedPC=0x218fb4;
            const float currentTime=secondary.time864;
            out.stoppedPC=0x218fb8;
            if(!call.secondaryFactorF20Bits_)return;
            float dispatchFactor;
            std::memcpy(&dispatchFactor,&*call.secondaryFactorF20Bits_,sizeof(dispatchFactor));
            if(!std::isfinite(currentTime)||!std::isfinite(dispatchFactor))return;
            volatile float nextTime=currentTime+dispatchFactor;
            out.stoppedPC=0x218fbc;
            if(!std::isfinite(nextTime)||!markSharedDirectQueryEffect(lease))return;
            secondary.time864=nextTime; //Before body288 read and any physical getter.
            out.stoppedPC=0x218fcc;
            //Retain the same admitted positive native physical linkage. A false
            //native scene flag is NOT evidence for the source null288 branch.
            if(!secondary.scenePresent||owner->revoked_||
               owner->physical19C_!=gridArena_->bindings.lifecycle.physical19C||
               owner->physical19C_!=gridArena_->rigs[slot]->physical)return;
            out.stoppedPC=0x218fd8;
            if(!g::validateSceneGraph49P(graph)||physicalNode.parent180!=g::SceneNodeIndex49P::none||
               !markSharedDirectQueryEffect(lease))return;
            if(!g::refreshSceneWorld49P(graph,owner->physical19C_))return;
            secondary.root.scene=physicalNode.world100;
            secondary.sceneFlags190=(secondary.sceneFlags190&0xffffff00u)|physicalNode.sceneFlags190;
            if(!g::detail::cleanMatrixDirectionPrefix49U(physicalNode.world100,localNoiseForce,scratch,worldNoiseForce))return;
            constexpr std::uint32_t noiseForceStores[]={0x218ff4u,0x219004u,0x219014u};
            for(unsigned k=0;k<3;++k)
                if(!add(secondary.root.loads.force.v[k],worldNoiseForce.v[k],noiseForceStores[k]))return;
            out.stoppedPC=0x21901c;
            const auto& noiseDragWord=app->nativeSecondaryForceLazyWords_[5];
            if(!noiseDragWord)return;
            float noiseDrag;std::memcpy(&noiseDrag,&*noiseDragWord,sizeof(noiseDrag));
            out.stoppedPC=0x219030;
            if(!std::isfinite(noiseDrag))return;
            const float negativeNoiseDrag=-noiseDrag;
            //Source loads all velocity XYZ before the three products, then
            //computes all products before any damping force add/store.
            const auto noiseVelocity=secondary.root.body.velocity;
            g::Vector3_48C noiseDamping;
            constexpr std::uint32_t noiseDragProducts[]={0x219040u,0x219044u,0x21904cu};
            for(unsigned k=0;k<3;++k){
                out.stoppedPC=noiseDragProducts[k];
                if(!std::isfinite(noiseVelocity.v[k]))return;
                volatile float product=negativeNoiseDrag*noiseVelocity.v[k];
                if(!std::isfinite(product))return;noiseDamping.v[k]=product;
            }
            constexpr std::uint32_t noiseDragStores[]={0x21907cu,0x21908cu,0x21909cu};
            for(unsigned k=0;k<3;++k)
                if(!add(secondary.root.loads.force.v[k],noiseDamping.v[k],noiseDragStores[k]))return;
            //One-shot invocation/terminal shared custody remains unchanged.
            //No joint360, response368, integration or frame success is claimed.
            out.stoppedPC=0x2190a0;return;
        }
        out.stoppedPC=0x218d9c;
        const auto& drag=app->nativeSecondaryForceLazyWords_[0];if(!drag)return;
        float dragValue;std::memcpy(&dragValue,&*drag,sizeof(dragValue));
        if(!std::isfinite(dragValue))return;const float negativeDrag=-dragValue;
        g::Vector3_48C damping;
        for(unsigned k=0;k<3;++k){
            if(!std::isfinite(secondary.root.body.velocity.v[k]))return;
            volatile float value=negativeDrag*secondary.root.body.velocity.v[k];
            if(!std::isfinite(value))return;damping.v[k]=value;
        }
        constexpr std::uint32_t dampingStores[]={0x218e08u,0x218e18u,0x218e28u};
        for(unsigned k=0;k<3;++k)
            if(!add(secondary.root.loads.force.v[k],damping.v[k],dampingStores[k]))return;
        //Distinct actual28A700 physical then main, literal relative=null and
        //forward-output=null. No scene getter was moved before force stores.
        out.stoppedPC=0x218e30;
        if(!g::validateSceneGraph49P(graph)||physicalNode.parent180!=g::SceneNodeIndex49P::none||
           !markSharedDirectQueryEffect(lease))return;
        if(!g::refreshSceneWorld49P(graph,owner->physical19C_))return;
        secondary.root.scene=physicalNode.world100;
        secondary.sceneFlags190=(secondary.sceneFlags190&0xffffff00u)|physicalNode.sceneFlags190;
        g::Vector3_48C physicalUp,mainUp;
        std::memcpy(physicalUp.v,physicalNode.world100.v+4,sizeof(physicalUp.v));
        out.stoppedPC=0x218e44;if(!refreshMain())return;
        std::memcpy(mainUp.v,mainNode.world100.v+4,sizeof(mainUp.v));
        out.stoppedPC=0x218e58;g::Vector3_48C cross;
        if(!g::prepareSecondaryAlignmentCross49M(physicalUp,mainUp,cross))return;
        out.stoppedPC=0x218e70;
        const auto& alignmentWord=app->nativeSecondaryForceLazyWords_[4];if(!alignmentWord)return;
        float alignmentScale;std::memcpy(&alignmentScale,&*alignmentWord,sizeof(alignmentScale));
        if(!std::isfinite(alignmentScale))return;
        g::VisualVector49R worldAlignment{},localAlignment{};
        for(unsigned k=0;k<3;++k){volatile float value=cross.v[k]*alignmentScale;
            if(!std::isfinite(value))return;worldAlignment.v[k]=value;}
        //Source body288 null branch is NOT inferred from native flag false.
        //Only actual positive native same-owner physical linkage is supported.
        out.stoppedPC=0x218eac;if(!secondary.scenePresent)return;
        out.stoppedPC=0x218eb8;
        if(!g::validateSceneGraph49P(graph)||physicalNode.parent180!=g::SceneNodeIndex49P::none||
           !markSharedDirectQueryEffect(lease))return;
        if(!g::refreshSceneWorld49P(graph,owner->physical19C_))return;
        secondary.root.scene=physicalNode.world100;
        secondary.sceneFlags190=(secondary.sceneFlags190&0xffffff00u)|physicalNode.sceneFlags190;
        if(!g::detail::cleanMatrixInverseDirectionPrefix49U(physicalNode.world100,worldAlignment,scratch,localAlignment))return;
        constexpr std::uint32_t alignmentStores[]={0x218ed4u,0x218ee4u,0x218ef8u};
        for(unsigned k=0;k<3;++k)
            if(!add(secondary.root.loads.torque.v[k],localAlignment.v[k],alignmentStores[k]))return;
        out.stoppedPC=0x2190a0; //Actual joint360/368/integration calls still unjoined.
    }
