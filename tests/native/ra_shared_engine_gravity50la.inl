//Private genuine post1B1800 caller prefix. Existing native engine-field
//representation only; no invented guest34C Entity, scheduler or phase.
    void advanceSharedEngineGravity(SharedTerrainInvocation& call)noexcept {
        if(call.cut_!=SharedTerrainInvocation::Cut::returned||!call.entered_||
           !call.limitedEntered_||call.forcesEntered_)return;
        call.forcesEntered_=true;
        auto& lease=call.lease_;auto& out=call.result_;out.stoppedPC=0x219c80;
        //Called before the enclosing terrain callback's terminal guard fires.
        //Every failure retains earlier terrain effects and cannot be retried.
        if(!sharedLeaseCurrent(lease)||!actual_||!gridArena_||
           !gridInputsRegistered(*gridArena_)||!lease.producer_->activeSession||
           lease.activeSlot_>=lease.producer_->activeSession->racerEntities_.size()||
           call.ordinal_!=sharedProbeSelection_.count||out.secondCount<0||
           static_cast<std::size_t>(out.secondCount)!=call.ordinal_)return;
        const auto& session=*lease.producer_->activeSession;
        const auto& racer=session.racerEntities_[lease.activeSlot_];
        auto& main=actual_->frame.bodies.main;auto& components=main.components;
        auto& parameters=gridArena_->parameters.bodies.main;
        if(!racer.prepared()||!racer.body340||
           racer.body340!=stagedPods694_[lease.activeSlot_].bodyA0||
           racer.body340->canonicalNumericAddress()!=&main||
           racer.body340->entity().reference00!=1||
           racer.body340->entity().parent1C!=&racer.racer||
           racer.body340->entity().sessionGeneration!=session.generation_||
           !registeredRange(&main,sizeof(main))||
           !registeredRange(&parameters,sizeof(parameters))||
           std::fegetround()!=FE_TONEAREST)return;
        //Same actual lanes consumed by49K/49W: binding68/6C, speed34,
        //native engine34/38 and feedback3C. No unrelated Aero input validation
        //or rerun of the earlier control-binding producer is permitted here.
        g::EngineInput48D input{};
        input.drive[0]=components.bindings.engineDrive[0];
        input.drive[1]=components.bindings.engineDrive[1];
        input.bodySpeed=main.cachedSpeed34;
        input.boost=main.selectedEngineFlags.engine34!=0;
        input.repair=main.selectedEngineFlags.engine38!=0;
        input.fullTorque=components.feedback.engine3C!=0;
        g::EngineState48D produced;
        out.stoppedPC=0x2e7720;
        //Bounded pure numeric leaf: unsupported arithmetic rejects BEFORE a
        //claimed engine return. No source partial-engine failure is simulated.
        if(!g::updateEngine48D(components.engine,parameters.engine,input,produced))return;
        if(!markSharedDirectQueryEffect(lease))return;
        components.engine=produced; //Actual registered native engine backing.
        //Only now is gravity344 read. A rejected gravity owner must preserve
        //the already-returned engine producer; do not fold both into an atomic
        //engine/gravity helper or rollback the earlier source call.
        out.stoppedPC=0x219c8c;
        const auto app=session.actual_.applicationEntities;
        if(!app||!racer.gravity344||
           app->nativeOwnerThread_!=std::this_thread::get_id()||
           racer.gravity344->canonicalAccelerationAddress()!=&main.selectedGravity||
           racer.gravity344->target50()!=racer.body340)return;
        if(!racer.gravity344->accumulateTarget2827D0(*app,racer.racer,*racer.body340,
            main,parameters,out.stoppedPC))return;
        //The reader belongs to this actual Race invocation and is not called
        //until both preceding source producer calls have returned.
        out.stoppedPC=0x219c98;
        advanceSharedEngineApplication(call);
    }
