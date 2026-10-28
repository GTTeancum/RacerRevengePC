//Private actual219420 owner access, no prologue replay or scene-body snapshots.
    bool readSharedContactScale(const SharedOwnerLease& lease,
        const g::OriginalApplicationEntities50LA& app,std::uint32_t& word)const noexcept {
        if(!sharedLeaseCurrent(lease)||!lease.producer_->activeSession||
           lease.producer_->activeSession->actual_.applicationEntities.get()!=&app||
           app.nativeOwnerThread_!=std::this_thread::get_id()||
           !app.nativeVehicleContactStatics_.initialized[0])return false;
        word=app.nativeVehicleContactStatics_.words[0];return true;
    }
    bool prepareSharedContactLocalPoint(const SharedOwnerLease& lease,
        const g::SpringVector4_48N& point,g::SpringVector4_48N& output)const noexcept {
        if(!sharedLeaseCurrent(lease)||!actual_||!gridArena_)return false;
        const auto& session=*lease.producer_->activeSession;
        const auto& racer=session.racerEntities_[lease.activeSlot_];const auto* scene=racer.scene190;
        if(!racer.body340||!scene||racer.body340->scene288()!=scene||!scene->constructed||scene->released||
           !scene->entity().reference00||scene->entity().sessionGeneration!=session.generation_||
           scene->sceneIndex!=gridArena_->bindings.lifecycle.main190||
           racer.body340->canonicalNumericAddress()!=&actual_->frame.bodies.main||
           !g::validVehicleSceneCount49W(actual_->frame,gridArena_->bindings))return false;
        const auto index=static_cast<int>(scene->sceneIndex);
        if(index<0||static_cast<std::size_t>(index)>=actual_->frame.sceneNodeCount)return false;
        const auto& node=actual_->frame.nodes[index];
        //Actual28BD50 calls its190 scene getter. Only the represented clean
        //cache leaf is admitted; no dirty parent/child update is manufactured.
        if(!(node.sceneFlags190&1u)||!registeredRange(&node,sizeof(node))||
           std::memcmp(&node.world100,&actual_->frame.bodies.main.components.root.scene,sizeof(node.world100)))return false;
        return g::prepareVehicleContactLocalPoint48R(node.world100,point,output);
    }
