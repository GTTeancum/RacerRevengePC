// Private native outer-loader join, included inside native_racer50HW only.
// The Bridge authenticates the actual Session racer60, retained Cables pin,
// Application and already-active Bank.visit(0). No nested visit is permitted.
inline bool Cables::continueNativeOuter212DF0(g::OriginalRacerEntityCold50LA& racer,
    const native_racer_model50LA::Owner& chariot,
    std::shared_ptr<g::OriginalApplicationEntities50LA> app,std::uint32_t actualRacer60,
    std::uint32_t& pc)noexcept {
    using S=g::OriginalPodCableAllocationStatus50LA;
    if(nativeOuterAttempted_){pc=nativeOuterResult_.sourcePC;return false;}
    nativeOuterAttempted_=true;
    const auto stop=[&](S status,std::uint32_t address)noexcept {
        nativeOuterResult_={status,address};pc=address;return false;};
    const auto at=[&](std::uint32_t address)noexcept {
        pc=address;nativeOuterResult_={S::owner,address};};
    if(pc!=0x216034u||racer.nativePostEmberPC_!=0x216034u||!chariot.nativeHandlingReturned_||
       chariot.failurePC_!=0x216034u||!sourceRig_||!app||!app->application10()||
       cameraPins_!=1u||nativeFifoPins_||lateStrips_||racer.lateStrips_||
       selectionState_.allocation_||selectionState_.membership50_||racer.cables254||
       !racer.prepared()||racer.nativeWreckWithdrawn_||!racer.color260_||
       racer.mode3C_!=0u||racer.cableMaterialMode3C!=0||
       !chariot.finishedFor(racer)||chariot.modelRole_!=Rig::chariot||
       chariot.rig_!=sourceRig_||chariot.slot_>=8u||!chariot.bank_||!chariot.arena_||
       chariot.application_!=app||chariot.generation_!=racer.racer.sessionGeneration||
       chariot.handlingRoute_!=native_racer_model50LA::Owner::HandlingRoute::legacy||
       racer.actualCounter423F60()!=&app->objectCount423F60()||
       racer.actualLock423F58()!=&app->attachmentLock423F58()||app->attachmentLock423F58())
        return stop(S::owner,0x212df0u);
    const bool bottom=sourceRig_->modelCount==5u;
    if((sourceRig_->modelCount!=3u&&!bottom)||!racer.leftModel1BC||!racer.rightModel1B8||
       bool(racer.bottomLeftModel1C4)!=bottom||bool(racer.bottomRightModel1C0)!=bottom)
        return stop(S::owner,0x213a40u);
    const auto graph=chariot.graph();
    if(!graph.nodes||graph.nodes!=chariot.arena_->nodes||graph.count!=sourceRig_->nodeCount||
       !g::validateSceneGraph49P(graph))return stop(S::owner,0x213a40u);
    // Same retained actual model owners, not just matching model names. Their
    // current rows/numeric graph are the only operands admitted to28C590.
    const auto ownerCurrent=[&](const native_racer_model50LA::Owner* model,unsigned role,
        const g::OriginalRacerModelSceneOwner50LA* scene)noexcept {
        return model&&scene&&model->modelRole_==role&&model->rig_==sourceRig_&&
            model->bank_==chariot.bank_&&model->arena_==chariot.arena_&&model->slot_==chariot.slot_&&
            model->generation_==chariot.generation_&&model->application_==app&&
            model->domain_==chariot.domain_&&model->finishedFor(racer)&&model->modelScene()==scene&&
            scene->model()&&scene->model()->sceneIndex==sourceRig_->roots[role]&&
            scene->sourceRowCount()!=0u&&model->materialReferences()&&
            model->materialReferences()->table382B38()==racer.selectionMaterialTable_.get();
    };
    if(!ownerCurrent(&chariot,Rig::chariot,racer.chariotModel1B4)||
       !ownerCurrent(chariot.peers_[Rig::left],Rig::left,racer.leftModel1BC)||
       !ownerCurrent(chariot.peers_[Rig::right],Rig::right,racer.rightModel1B8)||
       (bottom&&(!ownerCurrent(chariot.peers_[Rig::bottomLeft],Rig::bottomLeft,racer.bottomLeftModel1C4)||
                 !ownerCurrent(chariot.peers_[Rig::bottomRight],Rig::bottomRight,racer.bottomRightModel1C0))))
        return stop(S::owner,0x213a40u);
    at(0x212df0u);
    try {
        const auto& binding=racer.binding_;
        if(!binding.actualAIDamageMaterial||!binding.actualMaterialDirectory36B970||
           !binding.materialDirectoryCapacity)return stop(S::material,0x299be0u);
        auto materials=binding.actualAIDamageMaterial->table382B38();
        if(!materials||!materials->prepared()||materials!=racer.selectionMaterialTable_)
            return stop(S::material,0x299be0u);
        std::size_t length=0;
        while(length<binding.materialDirectoryCapacity&&binding.actualMaterialDirectory36B970[length])++length;
        if(length==binding.materialDirectoryCapacity||length>248u)return stop(S::material,0x299be0u);
        const std::string directory(binding.actualMaterialDirectory36B970,length);
        // Native custody allocation, not a guest allocator result. On a host
        //exception no source NULL store is invented, and this call stays spent.
        lateStrips_.reset(new g::OriginalRacerStripOwners50LA);
        auto result=lateStrips_->constructBinder213A40(racer,app,graph,directory,true);
        nativeOuterResult_=result;pc=result.sourcePC;
        if(result.status!=S::selectionAttachmentComplete||result.sourcePC!=0x213e84u)return false;
        at(0x212df8u);
        result=lateStrips_->constructRing213E90(actualRacer60,directory);
        nativeOuterResult_=result;pc=result.sourcePC;
        if(result.status!=S::selectionAttachmentComplete||result.sourcePC!=0x214100u)return false;
        at(0x212e00u); //Actual214110 jr ra /214114 nop has no state effects.
        at(0x212e08u);
        // Admission only, not retroactive211C44/211C4C or211CA0/211CA8
        //execution. Canonical258 still holds its real20F94C value1. With both
        //bottoms, the original primary-secondary branches are skipped. Without
        //bottoms, inspect BOTH same current model lookups before consuming258.
        //Prewarmed aliases authenticate indices only, never source-call history.
        if(racer.cableMultiplicity258!=1u)
            return stop(S::multiplicityProducerRequired,0x214154u);
        if(!bottom){
            const g::OriginalRacerSceneObject50LA *left=nullptr,*right=nullptr;
            if(!lateStrips_->find28C590(racer.leftModel1BC,graph,"enginecableleft2",left)||
               !lateStrips_->find28C590(racer.rightModel1B8,graph,"enginecableright2",right))
                return stop(S::owner,0x214154u);
            if(left||right||selectionSecond_[2]!=Index::none||selectionSecond_[3]!=Index::none)
                return stop(S::multiplicityProducerRequired,0x214154u);
        }
        //214154/168 consume the untouched same field, then21417C/18C add
        //bottoms. This bounded valid-count check does not claim the omitted
        //process gp8730/4346A0 lazy assertion history at214190..2141F0.
        std::uint32_t count=0;
        if(racer.leftModel1BC)count+=racer.cableMultiplicity258;
        if(racer.rightModel1B8)count+=racer.cableMultiplicity258;
        if(racer.bottomLeftModel1C4)++count;
        if(racer.bottomRightModel1C0)++count;
        if(!count||count>4u)return stop(S::owner,0x2141c0u);
        at(0x214200u);nativeOuterCablesAllocationAttempted_=true;
        selectionState_.allocation_.reset(new g::OriginalPodCableAllocation50LA);
        std::array<g::OriginalWreckComponent50LA*,12> objects{};
        const auto domain=lateStrips_->detachedDomain(objects);
        result=selectionState_.allocation_->constructWithConstructorLanes(racer,app,count,materials,
            directory,domain,true,[&]()noexcept {
                // Fresh173CC0 reinitializes the existing canonical lanes even
                //after an earlier camera lifetime changed position40.
                selectionState_.position40_={{0.f,0.f,0.f,1.f}}; //173CE8 vf0
                selectionState_.membership50_=0u; //173D1C
            });
        nativeOuterResult_=result;pc=result.sourcePC;
        if(result.status!=S::selectionAttachmentComplete||result.sourcePC!=0x214254u)return false;
        // Native resources returned through214250/214254. No212ED0/SPIN,
        //outer210770, fullPlayer, selection224720, renderer or Restart claim.
        nativeOuterReturned_=true;nativeOuterResult_={S::selectionAttachmentComplete,0x212e10u};
        pc=0x212e10u;return true;
    }catch(...){return stop(S::hostAllocation,pc);}
}
