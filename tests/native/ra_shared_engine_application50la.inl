//Same held post-terrain invocation. No stored flags byte or inferred scene null.
    void advanceSharedEngineApplication(SharedTerrainInvocation& call)noexcept {
        if(call.cut_!=SharedTerrainInvocation::Cut::returned||!call.forcesEntered_||
           call.engineAppliedEntered_||call.result_.stoppedPC!=0x219c98)return;
        call.engineAppliedEntered_=true;
        auto& lease=call.lease_;auto& out=call.result_;
        if(!sharedLeaseCurrent(lease)||!actual_||!gridArena_)return;
        const auto* reader=lease.producer_->engineEnable;
        if(!reader||!reader->context||!reader->read||
           reader->session!=lease.producer_->activeSession)return;
        bool enabled=false;
        if(!reader->read(reader->context,enabled))return; //Actual B4 bit0x02, NOW.
        if(!enabled){out.stoppedPC=0x219cc8;advanceSharedAero(call);return;} //No scene read on clear branch.
        out.stoppedPC=0x2e7c20;
        const auto& session=*lease.producer_->activeSession;
        const auto& racer=session.racerEntities_[lease.activeSlot_];
        auto& frame=actual_->frame;auto& main=frame.bodies.main;
        if(!racer.body340||racer.body340!=stagedPods694_[lease.activeSlot_].bodyA0||
           racer.body340->canonicalNumericAddress()!=&main||
           racer.body340->entity().reference00!=1||
           racer.body340->entity().parent1C!=&racer.racer||
           racer.body340->entity().sessionGeneration!=session.generation_||
           !registeredRange(&main,sizeof(main))||std::fegetround()!=FE_TONEAREST)return;
        //Explicit existing native engine->main-body linkage, not guest34C+30.
        //Each source getter is performed separately before its own force adds.
        g::detail::SceneVectorContext49U scratch{0.f}; //Finite scratch domain; W is dead.
        const auto direction=[&](unsigned side,g::VisualVector49R& output){
            const auto* scene=racer.body340->scene288();
            if(!scene||scene!=racer.scene190||!scene->constructed||scene->released||
               !scene->entity().reference00||scene->entity().sessionGeneration!=session.generation_||
               scene->sceneIndex!=gridArena_->bindings.lifecycle.main190||
               !g::validVehicleSceneCount49W(frame,gridArena_->bindings))return false;
            const auto index=static_cast<int>(scene->sceneIndex);
            if(index<0||static_cast<std::size_t>(index)>=frame.sceneNodeCount)return false;
            const auto& node=frame.nodes[index];
            if(!(node.sceneFlags190&1u)||!registeredRange(&node,sizeof(node))||
               std::memcmp(&node.world100,&main.components.root.scene,sizeof(node.world100)))return false;
            g::VisualVector49R input{};
            std::memcpy(input.v,main.components.engine.localThrust[side].v,3*sizeof(float));
            //Clean getter only. No dirty graph traversal/owner publication is
            //authorized; input/scratch W cannot affect the represented XYZ.
            return g::detail::direction49U({frame.nodes,frame.sceneNodeCount},scene->sceneIndex,
                input,scratch,output);
        };
        const auto add=[&](float& lane,float value,std::uint32_t pc){
            out.stoppedPC=pc;
            if(!std::isfinite(lane)||!std::isfinite(value))return false;
            volatile float sum=lane+value;if(!std::isfinite(sum))return false;
            lane=sum;return true;
        };
        if(!markSharedDirectQueryEffect(lease))return;
        constexpr std::uint32_t getters[]={0x2e7c3cu,0x2e7c98u};
        constexpr std::uint32_t forceStores[][3]={{0x2e7c58u,0x2e7c68u,0x2e7c78u},
            {0x2e7cb4u,0x2e7cc4u,0x2e7cd4u}};
        for(unsigned side=0;side<2;++side){
            out.stoppedPC=getters[side];g::VisualVector49R world;
            if(!direction(side,world))return; //Earlier getter/force stores survive.
            for(unsigned k=0;k<3;++k)
                if(!add(main.components.root.loads.force.v[k],world.v[k],forceStores[side][k]))return;
        }
        constexpr std::uint32_t torqueStores[][3]={{0x2e7cfcu,0x2e7d0cu,0x2e7d1cu},
            {0x2e7d38u,0x2e7d48u,0x2e7d58u}};
        for(unsigned side=0;side<2;++side)for(unsigned k=0;k<3;++k)
            if(!add(main.components.root.loads.torque.v[k],main.components.engine.torque[side].v[k],
                torqueStores[side][k]))return;
        out.stoppedPC=0x219cc8; //Actual350 native owner service, still same invocation.
        advanceSharedAero(call);
    }

    //Private prerequisite called by the source-ordered native16CD90 body.
    //The outer factory/auxiliary completion route remains uncalled.
    //It confers no Aero return and cannot skip any prior auxiliary effects.
    bool borrowSharedNativeAero(const SharedTerrainInvocation& call,
        std::shared_ptr<g::NativeRacerAeroOwner50LA>& owner,std::uint32_t& factorBits)const noexcept {
        const auto& lease=call.lease_;
        if(call.cut_!=SharedTerrainInvocation::Cut::returned||!call.forcesEntered_||
           !call.engineAppliedEntered_||call.result_.stoppedPC!=0x219cc8||
           !sharedLeaseCurrent(lease)||!actual_||!gridArena_||
           !gridInputsRegistered(*gridArena_)||!lease.producer_->activeSession||
           lease.activeSlot_>=stagedAero_.size())return false;
        const auto* invocation=lease.producer_->engineEnable;
        const auto& session=*lease.producer_->activeSession;const auto slot=lease.activeSlot_;
        if(!invocation||invocation->session!=&session||!invocation->context||!invocation->read||
           !session.actual_.actualChariotModels||slot>=session.racerEntities_.size())return false;
        const auto retained=stagedAero_[slot].lock();
        const auto& racer=session.racerEntities_[slot];
        if(!retained||!retained->published_||retained->revoked_||!retained->sourceConstructorComplete_||
           retained->bank_!=lease.bank_||retained->frame_!=&actual_->frame||
           retained->parameters_!=&gridArena_->parameters.bodies.main||
           retained->application_!=session.actual_.applicationEntities.get()||
           retained->slot_!=slot||retained->generation_!=session.generation_||
           retained->model_!=(*session.actual_.actualChariotModels)[slot].get()||
           retained->racer_!=&racer||!racer.prepared()||!racer.racer.reference00||
           racer.racer.kind!=g::OriginalWreckComponentKind50LA::racer27E130||
           racer.racer.sessionGeneration!=retained->generation_||
           racer.racer.objectKey!=retained->racerKey_||!retained->racerKey_||
           !racer.body340||retained->body_!=racer.body340||
           racer.body340!=stagedPods694_[slot].bodyA0||
           racer.body340->canonicalNumericAddress()!=&actual_->frame.bodies.main||
           racer.body340->scene288()!=racer.scene190)return false;
        if(!invocation->sourceFactorF20Bits)return false;
        const float factor=std::bit_cast<float>(*invocation->sourceFactorF20Bits);
        if(!std::isfinite(factor)||factor<0.f)return false;
        owner=retained;factorBits=*invocation->sourceFactorF20Bits;return true;
    }
