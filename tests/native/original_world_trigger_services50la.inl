// Actual World+144, same two trigger/collider/action/membership identities.
namespace ra_pod_scene50LA {
bool Owner::completeNativeTriggerServices2DCB30(std::uint32_t& pc)noexcept {
    pc=0x2dcb30;
    if(!nativeTriggerList3D0_||!worldApplication_||worldApplication_->attachmentLock423F58()||
       !canRetireNativeTriggerList3D0())return false;
    auto& list=*nativeTriggerList3D0_;
    if(!list.complete||list.count30!=2u||list.joined!=2u||list.serviceTerminal||
       !list.authoring->sources()||list.actions->source_!=list.authoring.get()||
       !list.actions->ready_||list.actions->lease_||list.actions->terminal_!=ra_trigger_actions50LA::Status::Complete)
        return false;
    for(std::size_t i=0;i<2u;++i){const auto& source=(*list.authoring->sources())[i];
        const auto& pair=list.actions->nativeEntities_[i];
        if(list.descriptors34[i]!=&source||source.zone!=4u||!source.startEnabled||source.oneTimeOnly||
           !source.ignorePlayer||source.randomTriggerPercent90!=100||source.action.reset||
           source.action.targetNodeIndex!=(i?104u:29u)||!pair.triggerCounted||!pair.colliderCounted||
           !pair.attached||pair.trigger.nativeFlags29!=&list.actions->group_.triggers[i].flags29)return false;
    }
    if(list.serviceComplete){
        for(std::size_t i=0;i<2u;++i)if(!list.parented[i]||!list.geometryRoleConsumed[i]||
            !list.actions->nativeLostAiCallbackCurrent(i)||!nativeTriggerObserverCurrent(i))return false;
        pc=0x274ecc;return true;
    }
    if(list.serviceEntered)return false;
    list.serviceEntered=true;list.serviceTerminal=true;
    for(;list.servicesCompleted<2u;++list.servicesCompleted){const auto i=list.servicesCompleted;
        auto& pair=list.actions->nativeEntities_[i];
        //Descriptor08 stores authored zone minus1. World4C4[3] is the SAME
        //existing native zone4 First membership, never a relocation.
        pc=0x2dc270;if(!configureNativeTriggerObserver2A0A80(i,pc))return false;
        pc=0x1b5ad0;if(!list.actions->adoptNativeLostAiCallback1B5AD0(i,pc))return false;
        //Native immutable reader representation has no separately allocated
        //retail50-byte geometry64. Consume only its private reader role; never
        //free Source geometry or pretend omitted2DBEA4 allocation executed.
        pc=0x2dc2f0;list.geometryRoleConsumed[i]=true;
        pc=0x2dcc30;
        const auto result=worldApplication_->entityAuthority_.attachChildren(list.entity,pair.trigger,
            0x402u,worldApplication_->attachmentLock423F58());
        pc=result.sourcePC;if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
        list.parented[i]=true; //Actual existing collider child/mask2 stays intact.
    }
    list.serviceComplete=true;list.serviceTerminal=false;
    pc=0x274ecc;return true; //2DCC60->274EC0 both return1; no fullWorld.
}
}
namespace ra_cold_aggregate50LA {
bool Owner::continueNativeTriggerServices274E90(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa2d8;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeParticleList3D8_||!nativeParticleList3D8_->serviceComplete||
       nativeParticleList3D8_->serviceTerminal||!nativeSceneInfo270900Current(app,pc)||
       !world_||!world_->modelPrefixes_||!world_->domain_||!world_->imagePublication_||
       !runtimeQueries_||runtimeCameraBorrows_||runtimeWorldBorrows_||!worldApplication_||
       worldApplication_->attachmentLock423F58())return false;
    const auto display=runtimeQueries_->nativeColdModelDisplay274160(*worldApplication_,*source_,worldEntity_.sessionGeneration);
    std::array<std::int32_t,4> rectangle{};std::uint64_t revision=0;
    if(!display||!runtimeQueries_->camera_||!runtimeQueries_->camera_->session||
       world_->rendererPublication_!=&display->renderer_||
       world_->imagePublication_->raStaticWorldOwner()!=world_.get()||
       !display->readColdGameplayViewport115990(*runtimeQueries_->camera_->session,rectangle,revision))return false;
    if(!completeNativeTriggerServices2DCB30(pc))return false;
    return continueNativeSky274AE0(app,pc); //Actual next Profile/sky prefix; no fullWorld.
}
}
