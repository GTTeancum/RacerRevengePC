// Reached native +128 prefix. Same candidates/mesh/registries; no source
// placement replay, new primary membership, or invented temporary unregister.
namespace ra_pod_scene50LA {
bool Owner::beginNativeCollisionPrefix2743B0(ra_static_model_cold50LA::Owner& models,
    ra_static_candidates50LA::Owner& candidates,std::uint32_t& pc)noexcept {
    pc=0x2743b0;
    if(nativeCollisionServiceTerminal_){pc=nativeCollisionPrefixPC_;return false;}
    if(nativeCollisionPrefixEntered_&&!nativeCollisionServiceComplete_&&nativeCollisionPrefixPC_!=0x27b174){
        pc=nativeCollisionPrefixPC_;return false;}

    if(!worldApplication_||!worldCollisionObjects3CC_||worldCollisionObjects3CC_->sourceMode_||!worldCollisionResources_||!runtimeQueries_||
       !runtimeQueries_->actual_||!runtimeQueries_->actual_->recovery||
       !runtimeQueries_->storage_.get()||!models.canClose()||
       candidates.source_!=worldCollisionObjects3CC_->source_||
       worldCollisionResources_->candidates_.get()!=&candidates||
       !worldCollisionResources_->resolver_||!candidates.prepared_||
       candidates.candidates_.size()<178u||candidates.bindings_.size()<178u)return false;
    auto& bridge=*runtimeQueries_;auto& storage=*bridge.storage_.get();
    const auto& actual=*bridge.actual_;
    g::OriginalRaceFogFields* fog=nullptr;
    if(!bridge.readNativeColdFog273C30(*worldApplication_,*candidates.source_,
            worldEntity_.sessionGeneration,fog)||
       actual.recovery!=bridge.inputs_.recovery||!actual.recovery->storagePrepared||
       actual.recovery->globals.movementCategory3E0720!=actual.main.materialMask||
       actual.main.collisionTypeTag!=candidates.names_.collisionTypeTag||
       &storage.globalValid3E0730!=&worldApplication_->collisionGlobalValid3E0730_||
       !worldApplication_->collisionDirectory36B0F0_.empty()||
       worldApplication_->attachmentLock423F58())return false;
    auto& objects=*worldCollisionObjects3CC_;
    const auto* data=objects.source_->sourcesDecoded();
    if(!data||data->collisions.size()!=178u||data->world.instances.size()!=1261u||
       !models.bindNativeCollisionForest27B170(objects,pc))return false;
    nativeCollisionModelBorrow_=&models;
    if(nativeCollisionServiceComplete_){
        if(nativeCollisionNegativeLoads_!=1083u||nativeCollisionPositiveLoads_!=178u||
           !canRetireNativeCollisionQueries27B040())return false;
        pc=0x274458;return true;
    }
    nativeCollisionPrefixEntered_=true;nativeCollisionServiceTerminal_=true;
    struct Cursor {std::uint32_t& saved;std::uint32_t& current;~Cursor(){saved=current;}} cursor{nativeCollisionPrefixPC_,pc};
    //Native mounted namespace is deliberately separate from36B0F0, whose
    //frontend producer is unjoined. Never copy modelDirectory36B860 into it.
    //Known decoded members/cached file rows supply existing native ownership.
    //The source query category is borrowed from the actual recovery producer;
    //the existing candidate retains its later track-specific category/filter.
    const auto sourceCategory=actual.recovery->globals.movementCategory3E0720;
    try {
        for(std::size_t z=0;z<objects.zones_.size();++z){
            auto& zone=objects.zones_[z];pc=0x2743e4;
            if(!zone.counted||zone.count30!=zone.entries34.size()||
               zone.count30!=data->world.zones[z].models)return false;
            nativeModelZone354_=static_cast<std::uint32_t>(z);
            for(std::size_t row=0;row<zone.entries34.size();++row){
                auto& wrapper=zone.entries34[row];
                if(wrapper.nativeQuery){const auto& prior=*wrapper.nativeQuery;
                    if(prior.negativeLoadReturned&&prior.retired&&wrapper.descriptorRetired&&!wrapper.descriptor08)continue;
                    if(prior.registered&&wrapper.descriptorRetired&&!wrapper.descriptor08&&
                       nativeCollisionPrimaryCurrent27B174(prior))continue;
                }
                const g::Type2StaticCandidate49X* candidate=nullptr;
                std::size_t index=std::numeric_limits<std::size_t>::max();
                if(wrapper.decodedCollision){
                    for(std::size_t i=0;i<178u;++i)if(wrapper.decodedCollision==&data->collisions[i]){index=i;break;}
                    if(index==std::numeric_limits<std::size_t>::max())return false;
                    candidate=&candidates.candidates_[index];
                    const auto& binding=candidates.bindings_[index];const auto& terrain=candidates.terrainDescriptors_[index];
                    if(binding.candidate!=candidate||binding.originalOwner!=index||binding.animated||binding.trigger||
                       terrain.source!=wrapper.source||!wrapper.descriptor08||terrain.name00!=wrapper.descriptor08->name00||
                       candidate->terrainDescriptor180!=&terrain.field180||
                       candidate->candidate.collision.typeTag!=candidates.names_.collisionTypeTag||
                       storage.candidates[index]!=candidate)return false;
                }
                const bool resumed=bool(wrapper.nativeQuery);
                if(!resumed&&!objects.beginNativeQuery27B040(z,row,*worldApplication_,worldEntity_.sessionGeneration,
                    candidate,index,sourceCategory,pc))return false;
                auto& q=*wrapper.nativeQuery;
                if(resumed){
                    //Only the exact previously reached first-positive prefix
                    //may resume: no constructor/resource/global replay.
                    if(z||row!=361u||nativeCollisionNegativeLoads_!=361u||nativeCollisionPositiveLoads_||
                       !candidate||q.candidate!=candidate||!q.loaded||!q.appended||q.registered||
                       q.observerAttempted||q.terminal||q.sourceCategoryA4!=sourceCategory||
                       !objects.nativeQueryCurrent27B040(wrapper)||
                       !worldCollisionResources_->loadedBy11B920(index,q.entity))return false;
                }else {
                //Actual11B570 final byte store, preserving the rest of this
                //real process-name buffer and every existing alias to byte0.
                worldApplication_->collisionGlobalValid3E0730_=0;
                if(!objects.beginNativeResource11B920(z,row,pc))return false;
                pc=0x11b9fc;
                if(q.name.size()+1u>worldApplication_->collisionName3E0730_.size())return false;
                std::memcpy(worldApplication_->collisionName3E0730_.data(),q.name.c_str(),q.name.size()+1u);
                rr::assets::OriginalStandardModelFileView50LA file;
                rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
                const auto lookup=worldCollisionResources_->resolver_->lookupCollision132820(q.member,file,diagnostic);
                if(!candidate){
                    if(lookup!=rr::assets::OriginalStandardCollisionLookup50LA::absent)return false;
                    //Actual native mounted provider+cache negative result. Guest
                    //+170 fallback/cursor/diagnostic history remains unknown.
                    if(!objects.completeNativeNegativeLoad27B12C(z,row,q.member,pc))return false;
                    ++nativeCollisionNegativeLoads_;continue;
                }
                if(lookup!=rr::assets::OriginalStandardCollisionLookup50LA::present)return false;
                const bool loaded=worldCollisionResources_->load11B920(index,q.entity,diagnostic);
                if(!objects.finishNativeResource11B920(z,row,loaded,diagnostic.sourcePC,pc))return false;
                if(!models.appendNativeCollision27B170(z,row,pc))return false;
                }
                if(!completeNativeCollisionPlacement27B174(z,row,pc)||
                   !objects.freeNativeDescriptor27B2A4(z,row,pc))return false;
                ++nativeCollisionPositiveLoads_;
            }
        }
        if(nativeCollisionNegativeLoads_!=1083u||nativeCollisionPositiveLoads_!=178u||
           !canRetireNativeCollisionQueries27B040())return false;
        //Actual represented native+128 loop/epilogue only. Placement and
        //early-primary adoption qualifications remain; no full World claim.
        nativeCollisionServiceComplete_=true;nativeCollisionServiceTerminal_=false;
        nativeCollisionPrefixPC_=pc=0x274458;return true;
    }catch(...){nativeCollisionPrefixPC_=pc;return false;}
}
bool Owner::canRetireNativeCollisionQueries27B040()const noexcept {
    if(!worldCollisionObjects3CC_)return !nativeCollisionModelBorrow_;
    const auto& objects=*worldCollisionObjects3CC_;
    if(objects.sourceMode_)return !nativeCollisionModelBorrow_&&!worldCollisionResources_;
    if(nativeCollisionModelBorrow_&&!nativeCollisionModelBorrow_->nativeCollisionForestCurrent27B170())return false;
    for(const auto& zone:objects.zones_)for(const auto& w:zone.entries34){
        if(!w.nativeQuery){if(w.collision04)return false;continue;}
        const auto& q=*w.nativeQuery;
        if(!canRetireNativeCollisionObserver27B174(q))return false;
        if(q.registered&&!nativeCollisionPrimaryCurrent27B174(q))return false;
        if(q.retired){if(!objects.canRetireNativeQuery11B4A0(w))return false;continue;}
        if(q.counted&&(!q.application||q.application!=worldApplication_.get()||
           q.entity.reference00!=1||q.entity.sessionGeneration!=worldEntity_.sessionGeneration))return false;
        if(q.resourceStarted&&!q.resourceWithdrawn&&q.candidate){
            if(!worldCollisionResources_)return false;
            if(worldCollisionResources_->attemptedBy11B920(q.resourceIndex,q.entity)&&
               !worldCollisionResources_->canRelease11B600(q.resourceIndex,q.entity))return false;
        }
        if(!q.appended&&(q.entity.parent1C||q.entity.previous10||q.entity.next14))return false;
    }
    return true;
}
bool Owner::detachNativeCollisionQueries27B170(std::uint32_t& pc)noexcept {
    if(!canRetireNativeCollisionQueries27B040())return false;
    if(!nativeCollisionModelBorrow_)return true;
    auto& models=*nativeCollisionModelBorrow_;
    //Withdraw only the native source-owner association. Existing primary
    //registrations/candidate backing still belong to actual live Storage.
    for(auto& zone:worldCollisionObjects3CC_->zones_)for(auto& w:zone.entries34)
        if(w.nativeQuery&&!retireNativeCollisionObserver27B174(*w.nativeQuery))return false;
    if(!models.detachNativeCollisionSuffix27B170(pc)||!models.releaseNativeCollisionForest27B170(pc))return false;
    nativeCollisionModelBorrow_=nullptr;return true;
}
bool Owner::retireNativeCollisionQueries11B4A0(std::uint32_t& pc)noexcept {
    //Registry/query lifetime has actually ended, not a readiness observation.
    if(runtimeQueries_||nativeCollisionModelBorrow_||!canRetireNativeCollisionQueries27B040())return false;
    if(!worldCollisionObjects3CC_)return true;
    auto& objects=*worldCollisionObjects3CC_;
    for(std::size_t z=objects.zones_.size();z-->0;)for(std::size_t row=objects.zones_[z].entries34.size();row-->0;){
        auto& w=objects.zones_[z].entries34[row];if(!w.nativeQuery)continue;auto& q=*w.nativeQuery;
        if(q.retired)continue;
        if(q.resourceStarted&&!q.resourceWithdrawn){
            if(q.candidate&&worldCollisionResources_->attemptedBy11B920(q.resourceIndex,q.entity)&&
               !worldCollisionResources_->release11B600(q.resourceIndex,q.entity))return false;
            q.resourceWithdrawn=true; //No attempted resource or actual withdrawal above.
        }
        if(!objects.retireNativeQuery11B4A0(z,row,pc))return false;
    }
    return true;
}
}
namespace ra_cold_aggregate50LA {
bool Owner::continueNativeCollisionObjects2743B0(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa244;
    if(!nativeStandardServiceComplete_||!nativeFinishLine540_||!nativeFinishLine540_->complete||
       !world_||!world_->modelPrefixes_||!world_->domain_||!world_->rendererPublication_||
       !world_->imagePublication_||!runtimeQueries_||!candidates_||
       !nativeSceneInfo270900Current(app,pc))return false;
    const auto display=runtimeQueries_->nativeColdModelDisplay274160(*worldApplication_,*source_,worldEntity_.sessionGeneration);
    std::array<std::int32_t,4> rectangle{};std::uint64_t revision=0;
    if(!display||!runtimeQueries_->camera_||!runtimeQueries_->camera_->session||
       world_->rendererPublication_!=&display->renderer_||
       !display->readColdGameplayViewport115990(*runtimeQueries_->camera_->session,rectangle,revision)||
       world_->imagePublication_->raStaticWorldOwner()!=world_.get())return false;
    if(!beginNativeCollisionPrefix2743B0(*world_->modelPrefixes_,*candidates_,pc))return false;
    //1AA258 optional progress uses the established native policy, not a
    //fabricated original330/+30 observation or1CC900 call.
    //Source3AADA8 bytes:61 6e 69 6d 5c 00. This is the SAME actual
    //model directory36B860 already owned by the earlier1AA1DC producer;
    //never substitute it for the separate collision directory36B0F0.
    if(!nativeCollisionAnimationDirectory1AA2A0_){
        pc=0x1aa294;try{app.modelDirectory36B860()="anim\\";}catch(...){return false;}
        nativeCollisionAnimationDirectory1AA2A0_=true;
    }else if(app.modelDirectory36B860()!="anim\\")return false;
    pc=0x1aa2b0;
    //1B40B0->2742F0 reads the SAME3C8 cleared by the genuine prior
    //271454 empty-Animations lifetime. Not a constructor/default inference.
    if(!nativeEmptyAnimations2713E0Complete_||nativeEmptyToyList3C8_||
       nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !worldColdConstructed_||!worldApplication_||
       worldApplication_->attachmentLock423F58())return false;
    //274308 returns1;1B40E0 rechecks3C8 and jumps1B4304, preserving that
    //return through1B4328. No49C,500/50C,510/514 or animated child access.
    //Their unproduced state stays unknown; do not manufacture null stores.
    nativeEmptyToyService1B4328Complete_=true;
    return continueNativeParticleServices274370(app,pc); //Actual +124; no fullWorld.



}
}
