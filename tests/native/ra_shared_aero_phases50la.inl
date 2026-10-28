//Source-ordered native Aero350 adaptation. All callbacks/owners belong to
//this same held invocation. Neither pure numerical phase means16CD90 returned.
    void advanceSharedAero(SharedTerrainInvocation& call)noexcept {
        if(call.aeroEntered_)return;
        std::shared_ptr<g::NativeRacerAeroOwner50LA> owner;std::uint32_t factorBits;
        if(!borrowSharedNativeAero(call,owner,factorBits))return;
        call.aeroEntered_=true;
        auto& lease=call.lease_;auto& out=call.result_;
        const auto& session=*lease.producer_->activeSession;
        const auto& racer=session.racerEntities_[lease.activeSlot_];
        auto& frame=actual_->frame;auto& main=frame.bodies.main;auto& c=main.components;
        auto& parameters=*owner->parameters_;const auto app=session.actual_.applicationEntities;
        out.stoppedPC=0x16cdf0;
        if(!app||app.get()!=owner->application_||app->nativeOwnerThread_!=std::this_thread::get_id()||
           !registeredRange(&main,sizeof(main))||!registeredRange(&parameters,sizeof(parameters))||
           std::fegetround()!=FE_TONEAREST)return;
        //Only native live state34/40; neither old PostWorld{} nor ctor-null
        //guest350 history supplies this policy. Existing input lanes stay Banked.
        g::AeroInput48E input{};
        input.bodyLocalVelocity=c.root.localVelocity80;input.angularVelocity=c.root.body.angularVelocity;
        input.bodyAngleE0=main.cachedAnglesE0.v[0];input.subtractWorldFlow=owner->flowEnabled34_!=0;
        std::memcpy(input.worldFlow40.v,owner->flow40Words_.data(),3*sizeof(float));
        input.triggerD0=c.feedback.aeroD0!=0;input.triggerD8=c.bindings.aeroTriggerD8!=0;
        input.fullSteeringE0=c.feedback.aeroE0!=0;
        input.driveE4=c.bindings.aeroDrive[0];input.driveE8=c.bindings.aeroDrive[1];
        input.brakeEc=c.bindings.aeroBrakeEC;
        const auto cleanScene=[&](g::ReadyBodyBasis48D& basis){
            const auto* scene=owner->body_->scene288();
            if(!scene||scene!=racer.scene190||!scene->constructed||scene->released||
               !scene->entity().reference00||scene->entity().sessionGeneration!=session.generation_||
               scene->sceneIndex!=gridArena_->bindings.lifecycle.main190||
               !g::validVehicleSceneCount49W(frame,gridArena_->bindings))return false;
            const auto index=static_cast<int>(scene->sceneIndex);
            if(index<0||static_cast<std::size_t>(index)>=frame.sceneNodeCount)return false;
            const auto& node=frame.nodes[index];
            if(!(node.sceneFlags190&1u)||!registeredRange(&node,sizeof(node))||
               std::memcmp(&node.world100,&c.root.scene,sizeof(node.world100)))return false;
            for(unsigned column=0;column<3;++column)
                std::memcpy(basis.columns[column].v,node.world100.v+4*column,3*sizeof(float));
            return true; //Actual clean getter; no dirty traversal or null branch.
        };
        g::ReadyBodyBasis48D basis{};
        if(input.subtractWorldFlow){out.stoppedPC=0x16ce10;if(!cleanScene(basis))return;}
        g::AeroMotionPrefix48E motion;
        out.stoppedPC=0x16ce48;
        if(!g::prepareAeroMotionPrefix48E(input,basis,motion))return;
        out.stoppedPC=0x16cf58;
        if(!cleanScene(basis))return; //Actual28A700(relative=null) clean axes copy.
        if(!markSharedDirectQueryEffect(lease))return;
        auto& words=app->nativeAeroLazyWords_;
        out.stoppedPC=0x16cf74;if(!words[0])words[0]=0x3f000000u;
        out.stoppedPC=0x16cf88;
        const float up=basis.columns[1].v[1];
        if(*words[0]!=0x3f000000u||!std::isfinite(up))return;
        const bool tilted=up<=std::bit_cast<float>(*words[0]);
        out.stoppedPC=0x16cfb0;if(!words[1])words[1]=0x3e800000u;
        out.stoppedPC=0x16cfcc;if(!words[2])words[2]=0x40800000u;
        //Native current process constants, not silently replaced global data.
        if(*words[1]!=0x3e800000u||*words[2]!=0x40800000u)return;
        const float dt=std::bit_cast<float>(factorBits);
        auto& state=c.aero;
        if(input.triggerD0){
            out.stoppedPC=0x16cfe8;state.timer140=std::bit_cast<float>(*words[1]);
            out.stoppedPC=0x16cfec;state.timer144=0.f;
        }
        out.stoppedPC=0x16cff0;if(!std::isfinite(state.timer140))return;
        const bool firstTimer=!(state.timer140<=0.f);
        if(firstTimer){
            volatile float next=state.timer140-dt;out.stoppedPC=0x16d014;
            if(!std::isfinite(next))return;state.timer140=next;
        }
        if(input.triggerD8&&!firstTimer){out.stoppedPC=0x16d030;state.timer144=std::bit_cast<float>(*words[2]);}
        out.stoppedPC=0x16d034;if(!std::isfinite(state.timer144))return;
        const bool secondTimer=!(state.timer144<=0.f);
        if(secondTimer){
            volatile float next=state.timer144-dt;out.stoppedPC=0x16d058;
            if(!std::isfinite(next))return;state.timer144=next;
        }
        out.stoppedPC=motion.speed<50.f?0x16d07c:0x16d080;state.lowSpeedDc=motion.speed<50.f;
        g::AeroForcePrefix48E forces;out.stoppedPC=0x16d084;
        if(!g::calculateAeroForcePrefix48E(motion,parameters.aero,input,state,up,
            firstTimer,secondTimer,tilted,forces))return; //Timer/DC effects retained.
        out.stoppedPC=0x16d7c0;
        if(!cleanScene(basis))return; //Separate source direction getter, after state stores.
        const auto index=racer.scene190->sceneIndex;
        g::VisualVector49R local{},world;std::memcpy(local.v,forces.localForce.v,3*sizeof(float));
        g::detail::SceneVectorContext49U scratch{0.f}; //Finite surviving-VU domain, dead W.
        if(!g::detail::direction49U({frame.nodes,frame.sceneNodeCount},index,local,scratch,world))return;
        const auto add=[&](float& destination,float value,std::uint32_t pc){
            out.stoppedPC=pc;
            if(!std::isfinite(destination)||!std::isfinite(value))return false;
            volatile float sum=destination+value;if(!std::isfinite(sum))return false;
            destination=sum;return true;
        };
        constexpr std::uint32_t forcePC[]={0x16d7dcu,0x16d7ecu,0x16d7fcu};
        for(unsigned k=0;k<3;++k)if(!add(c.root.loads.force.v[k],world.v[k],forcePC[k]))return;
        constexpr std::uint32_t torquePC[]={0x16d820u,0x16d830u,0x16d840u};
        for(unsigned k=0;k<3;++k)if(!add(c.root.loads.torque.v[k],forces.directTorque.v[k],torquePC[k]))return;
        out.stoppedPC=0x219cdc; //Actual next354 virtual3C, not whole219810 return.
        if(lease.producer_->springResponse)lease.producer_->springResponse(call);
    }
