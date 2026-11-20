#pragma once
// Included only after both Operation and its privately allocated arena are complete.
namespace ra_cold_roster50LA {
inline bool Operation::constructorArenaCurrent()const noexcept {
    const auto owner=constructorArena_.lock();
    return constructorOnly_&&owner&&owner->resources_==resources_&&
        bank_==&owner->bank_&&&arena_.live==&owner->live_&&
        owner->matches(*bank_,arena_.live.state,arena_.live.bindings,arena_.ai,arena_.history,resources_->rigs_)&&
        (!owner->operationIdentity_||owner->operationIdentity_==this);
}
inline std::shared_ptr<Operation> Operation::bindConstructorOnly(
    std::shared_ptr<ra_source_camera50LA::OwnershipBridge> runtime,
    const original_main_entry50KG::Application& app,
    const std::shared_ptr<FreshExecutionArena>& fresh,std::uint64_t generation,Diagnostic& d,
    std::shared_ptr<RuntimeOwners> owners)noexcept {
    d={};d.stage="distinct source constructor arena";
    if(!fresh||!fresh->currentNative()||fresh->operationIdentity_||!runtime||
       runtime->rosterResources_!=fresh->resources_||fresh->borrowedBridge_.lock()!=runtime||
       !app.state||fresh->live_.state.borrowedNoise50KG!=&app.state->sharedNoise308030){
        d.failure="source construction requires its actual independently allocated arena";return {};}
    auto out=bindImpl(std::move(runtime),app,fresh->bank_,fresh->arena(),{},d,std::move(owners),fresh,generation);
    if(!out)return {};
    if(!fresh->bindOperationNative(out)){
        // Native-only admission has made no model effects, but the real Bank
        // pin must keep the Operation until its normal close path withdraws it.
        d.failure="fresh arena operation lifetime admission refused";return {};
    }
    return out;
}
inline bool Operation::constructorSlotsCurrent(const FreshExecutionArena& fresh)const noexcept {
    if(!constructorArenaCurrent()||constructorArena_.lock().get()!=&fresh||terminal_||closed_||
       nextSlot_!=resources_->racerCount_)return false;
    for(std::size_t slot=0;slot<nextSlot_;++slot)
        if(!constructorSlotComplete_[slot]||completed_[slot]||runtime().rosterSlotComplete_[slot])return false;
    return true;
}
inline bool Operation::constructConstructorSlot(native_racer50HN::Bank& actualBank,
    std::size_t slot,Diagnostic& d)noexcept {
    d={};d.slot=slot;d.stage="source constructor native reservation";
    if(!constructorArenaCurrent()||terminal_||closed_||&actualBank!=bank_||
       slot>=resources_->racerCount_||slot!=nextSlot_||attempted_[slot]||!registered()){
        d.failure="source constructor arena/slot admission refused";return false;}
    auto& r=runtime();
    if(r.terminal_||r.cameraAttempted_||r.gridAttempted_[slot]||actualBank.completedSteps(slot)||
       !r.source348Arena_||!r.hasSharedRosterAllocation()){
        d.failure="source constructor cannot adopt a later query or camera route";return false;}
    attempted_[slot]=true;r.rosterRetirementRequired_=true;
    try {
        auto& x=arena_.live;auto& s=x.state;auto& q=*r.actual_;
        const auto& rig=*resources_->rigs_[slot];const auto& collision=*resources_->collisions_[slot];
        actualBank.visit(slot,[&](){
            need(q.recovery&&!q.recovery->sourcePrepared&&!q.recovery->selectedSource&&
                !q.terrain.prepared&&!q.terrainLive.initialized&&!s.shape.registrationE0,
                "source constructor cannot adopt prepared recovery/terrain backing");
            d.stage="actual retained model constructors";
            const auto model=[&](std::unique_ptr<Model>& out,unsigned role){
                out=std::make_unique<Model>();
                out->prepareEarly(rig,slot,generation_,actualBank,x,entities_,domain_,motions_,
                    application_->modelDirectory36B860(),role);
            };
            model(right_[slot],native_racer50HW::Rig::right);model(left_[slot],native_racer50HW::Rig::left);
            if(rig.modelCount==5u){model(bottomRight_[slot],native_racer50HW::Rig::bottomRight);
                model(bottomLeft_[slot],native_racer50HW::Rig::bottomLeft);}
            model(chariot_[slot],native_racer50HW::Rig::chariot);
            chariot_[slot]->nativeSecondaryContactsRequested_=true;
            right_[slot]->bindSibling(*left_[slot]);
            chariot_[slot]->bindChariotSiblings(*right_[slot],*left_[slot],bottomRight_[slot].get(),bottomLeft_[slot].get());
            rig.install(s,x.bindings,x.handRecords,right_[slot].get(),left_[slot].get(),chariot_[slot].get(),
                bottomRight_[slot].get(),bottomLeft_[slot].get(),true); //Shared motion is process-wide.
            d.stage="immutable source handling and native metadata reservation";
            chariot_[slot]->reserveSourceHandlingBefore2146(x);
            // No main authored handling, AI, grid, terrain setup, recovery,
            // visual/cable constructors, or full query/coldBound publication.
            const auto& ids=r.bodyIdentities_[slot];
            need(ids.collision==&collision&&ids.main&&ids.secondary,"source immutable collision reservation differs");
            q.main.resource=&collision.main();q.secondary.resource=&collision.secondary();
            q.mainFilter.identity=ids.main;q.secondaryFilter.identity=ids.secondary;
            need(r.assignSharedColdIdentities(slot),"source native identity reservation differs");
            auto binding=arena_.terrainBindings;binding.identity=ids.terrain;std::string error;
            need(r.reserveSource348InActualSlot(slot,x.parameters,rig.phy,binding,error),
                error.empty()?"source348 immutable reservation refused":error.c_str());
            need(chariot_[slot]->body340_&&r.stageConstructorPod694(slot,*chariot_[slot]->body340_,x.parameters),
                "constructor-only694 main owner differs");
            r.stagedSecondaryContacts_[slot]=chariot_[slot]->nativeSecondaryContacts_;
            r.stagedRacerTerrain_[slot]=chariot_[slot]->nativeRacerTerrain_;
            r.stagedAero_[slot]=chariot_[slot]->nativeAero_;
        });
        constructorSlotComplete_[slot]=true;++nextSlot_;return true;
    }catch(const std::exception& e){terminal_=true;r.terminal_=true;
        d.failure="source constructor stopped with actual partial owners retained";
        try{d.detail=e.what();}catch(...){}return false;
    }catch(...){terminal_=true;r.terminal_=true;d.failure="source constructor exception; partial owners retained";return false;}
}
} // namespace ra_cold_roster50LA
namespace ra_cold_roster50LA {
inline bool Operation::releaseConstructorArenaNative(FreshExecutionArena& owner)noexcept {
    return !owner.operationIdentity_||owner.releaseOperationNative(*this);
}
}
namespace ra_source_camera50LA {
inline bool OwnershipBridge::bindConstructorActual(
    const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>& fresh,Diagnostic& d)noexcept try {
    d={};
    if(!fresh||fresh->thread_!=std::this_thread::get_id()||fresh->nativeClosed_||fresh->lanesRegistered_||
       bindAttempted_||actual_||bankIdentity_||!bankLifetime_.expired()||closed_||terminal_||
       !constructorArena_.expired()||!source348Arena_||!storage_.get()||!storage_.get()->sharedRoster_||
       !rosterResources_||rosterResources_!=fresh->resources_||inputs_.recovery!=&fresh->recovery_||
       fresh->terrain_.prepared||fresh->terrainLive_.initialized||fresh->recovery_.sourcePrepared||
       !fresh->recovery_.storagePrepared||queriesConstructed_||follow_)
        return fail(d,"distinct constructor query backing admission refused");
    bindAttempted_=true;constructorArena_=fresh;
    const auto* collision=inputs_.collisionRoster.empty()?nullptr:inputs_.collisionRoster[0];
    if(!collision||collision!=fresh->resources_->collisions_[0].get()||!collision->prepared())
        return fail(d,"constructor query source collision differs");
    // Native pointer custody only. No source bounds, Ready phase, terrain,
    // recovery, Follow object or shared query coldBound is inferred.
    fresh->main_.filter=&fresh->mainFilter_;fresh->secondary_.filter=&fresh->secondaryFilter_;
    actual_=std::make_unique<ActualQueryOwners>(ActualQueryOwners{fresh->live_.state,fresh->normalMode_,
        fresh->main_,fresh->mainFilter_,fresh->secondary_,fresh->secondaryFilter_,*collision,
        fresh->terrain_,fresh->terrainLive_,&fresh->recovery_,&fresh->movement_});
    return true;
}catch(...){return fail(d,"constructor query admission failed; native prefix retained");}
inline bool OwnershipBridge::stageConstructorPod694(std::size_t slot,
    const g::OriginalRacerMainBodyOwner50LA& body,const g::VehicleFrameParameters49W& parameters)noexcept {
    const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
    if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
       bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
       bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
       terminal_||closed_||!actual_||&actual_->frame!=&fresh->live_.state||
       &parameters!=&fresh->live_.parameters||slot>=inputs_.collisionRoster.size()||
       slot>=stagedPods694_.size()||stagedPods694_[slot].bodyA0||
       actual_->recovery!=&fresh->recovery_||actual_->recovery->sourcePrepared||
       actual_->recovery->selectedSource||actual_->terrain.prepared||queriesConstructed_||follow_)return false;
    const auto* collision=inputs_.collisionRoster[slot];const auto& entity=body.entity();
    if(!collision||bodyIdentities_[slot].collision!=collision||!collision->prepared()||
       actual_->main.resource!=&collision->main()||actual_->secondary.resource!=&collision->secondary()||
       actual_->mainFilter.identity!=bodyIdentities_[slot].main||
       actual_->secondaryFilter.identity!=bodyIdentities_[slot].secondary||
       body.canonicalNumericAddress()!=&actual_->frame.bodies.main||!body.scene288()||
       !body.scene288()->constructed||body.scene288()->released||entity.reference00!=1u||
       !entity.objectKey||!entity.sessionGeneration||entity.kind!=g::OriginalWreckComponentKind50LA::body167920||
       body.scene288()->entity().sessionGeneration!=entity.sessionGeneration||
       !registeredRange(&parameters,sizeof(parameters)))return false;
    // Exact native immutable shape/main-body reservation only. Source E8
    // remains unpublished: actual counted694/11B350 has not run at21204C.
    // No query setup, membership, Entity identity or coldBound is issued.
    stagedPods694_[slot]={collision,&collision->mainTarget().source,&body,
        entity.sessionGeneration,slot,NativePodResponseE8::unpublished};
    return true;
}
}
namespace ra_cold_roster50LA {
inline bool Operation::completeConstructorModelResources(Diagnostic& d)noexcept {
    d={};d.stage="actual constructor model resources";
    const auto fresh=constructorArena_.lock();
    // Common rewrite (area C): the RA static-world material namespace gate is the bridge's bound
    // Track (its static world owns the one material namespace, single_pass_world50LM).
    if(!fresh||!constructorSlotsCurrent(*fresh)||constructorResourcesAttempted_||
       runtime().cameraAttempted_||!runtime().track_||!runtime().track_->roles.staticWorld)return false;
    constructorResourcesAttempted_=true;
    try {
        const auto& provider=domain_->archiveProvider3E1A70();
        const auto& directory=application_->materialDirectory36B970();
        need(directory=="textures\\"&&provider&&provider->sealed()&&
            !domain_->materials382B38()->coldCollectorCurrent(domain_->coldCollector14D440()),
            "constructor source material namespace differs");
        using Rig=native_racer50HW::Rig;
        for(std::size_t slot=0;slot<resources_->racerCount_;++slot){d.slot=slot;
            bank_->visit(slot,[&](){
                const auto* rig=resources_->rigs_[slot].get();
                need(rig&&(rig->modelCount==3||rig->modelCount==5),"source model topology differs");
                for(const auto role:{Rig::right,Rig::left,Rig::bottomRight,Rig::bottomLeft,Rig::chariot}){
                    if(role>=rig->modelCount)continue;
                    auto* model=(role==Rig::right?right_:role==Rig::left?left_:role==Rig::bottomRight?
                        bottomRight_:role==Rig::bottomLeft?bottomLeft_:chariot_)[slot].get();
                    need(model&&model->actualRig()==rig&&model->modelRole()==role&&
                        model->materialDomain()==domain_.get(),"constructor model owner differs");
                    model->completeResources1538B0(directory);
                    const auto* geometry=model->modelGeometry();const auto* refs=model->materialReferences();
                    need(geometry&&geometry->payload()&&refs&&refs->table382B38()==domain_->materials382B38().get(),
                        "constructor resource prefix incomplete");
                    original_racer_material_sources50LA::Diagnostic sourceDiagnostic;
                    need(original_racer_material_sources50LA::appendModelSources(*geometry->payload(),*refs,
                        directory,*provider,modelMaterials_,modelTextures_,sourceDiagnostic),
                        "constructor immutable material source closure refused");
                }
            });
        }
        rr::assets::OriginalStandardMaterialDiagnostic50LA materialDiagnostic;
        need(provider->sealed()&&
            domain_->appendOriginalSources(modelMaterials_,modelTextures_,materialDiagnostic),
            "constructor material source corpus append refused");
        constructorResourcesComplete_=true;return true;
    }catch(const std::exception& e){terminal_=true;d.failure="source model resource prefix retained after refusal";
        try{d.detail=e.what();}catch(...){}return false;
    }catch(...){terminal_=true;d.failure="source model resource exception; prefix retained";return false;}
}
}


// Append after both FreshExecutionArena and Operation definitions are complete.
namespace ra_source_camera50LA {
inline bool OwnershipBridge::registerConstructorWorldOwners(
    const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>& fresh,
    const std::shared_ptr<OwnershipBridge>& self,Diagnostic& d)noexcept {
    d={};
    if(!fresh||self.get()!=this||fresh->thread_!=std::this_thread::get_id()||fresh->nativeClosed_||
       fresh->bankRetired_||!fresh->lanesRegistrationComplete_||fresh->sealed_||fresh->operationIdentity_||
       fresh->borrowedBridge_.lock().get()!=this||constructorArena_.lock()!=fresh||
       terminal_||closed_||registrationAttempted_||!bindAttempted_||!actual_||queriesConstructed_||follow_||
       bankIdentity_||!bankLifetime_.expired()||rosterResources_!=fresh->resources_||
       &actual_->frame!=&fresh->live_.state||&actual_->main!=&fresh->main_||
       &actual_->secondary!=&fresh->secondary_||&actual_->mainFilter!=&fresh->mainFilter_||
       &actual_->secondaryFilter!=&fresh->secondaryFilter_||&actual_->terrain!=&fresh->terrain_||
       &actual_->terrainLive!=&fresh->terrainLive_||actual_->recovery!=&fresh->recovery_||
       actual_->movement!=&fresh->movement_||staticBindings().empty())
        return fail(d,"constructor-only shared Bank owner identity refused");
    auto& bank=fresh->bank_;auto* s=storage_.get();
    if(!s||!s->sharedRoster_||s->sharedRoster_->retired||
       s->sharedRoster_->count!=fresh->resources_->racerCount_||
       inputs_.collisionRoster.size()!=fresh->resources_->racerCount_||
       !s->sharedRosterCanRetire()||fresh->terrain_.prepared||fresh->terrainLive_.initialized||
       fresh->recovery_.sourcePrepared||bank.raWorldLifetime_||bank.retired_||bank.sealed_||
       bank.visiting_||bank.readBlocked_)
        return fail(d,"constructor-only shared Bank requires untouched native query topology");
    // Half-open overlap checks avoid pointer-end overflow. No shared topology,
    // member, static candidate or shared method-state address is snapshotted.
    const auto absent=[&](const void* pointer,std::size_t bytes)noexcept {
        if(!bytes)return true;if(!pointer)return false;
        const auto first=reinterpret_cast<std::uintptr_t>(pointer);
        for(const auto& span:bank.spans_){
            const auto other=reinterpret_cast<std::uintptr_t>(span.address);
            if(!span.bytes)continue;
            if(first<=other ? other-first<bytes : first-other<span.bytes)return false;
        }
        return true;
    };
    const auto sharedAbsent=[&]()noexcept {
        if(!s->sharedSpansAbsent(absent))return false;
        for(const auto& binding:staticBindings()){
            if(!binding.candidate)return false;
            const auto& c=binding.candidate->candidate.collision;
            if(!absent(&c.filter.flagsF8,sizeof(c.filter.flagsF8))||
               !absent(&c.copied40,sizeof(c.copied40))||!absent(&c.copied50,sizeof(c.copied50)))return false;
        }
        return true;
    };
    if(!sharedAbsent())return fail(d,"shared query state overlaps per-racer Bank spans");
    registrationAttempted_=true;
    try {
        // Pin FIRST. If any subsequent enrollment refuses, the actual partial
        // spans and Bridge remain retained until explicit native Bank close.
        bank.raWorldLifetime_=std::make_shared<BankPin>(bank,self);
        bankLifetime_=bank.raWorldLifetime_;bankIdentity_=&bank;
        s->registerMutableOwners(bank); // Existing shared-specific output subset only.
        if(!sharedAbsent())return fail(d,"shared Bank output registration crossed topology");
        // Numeric/identity lanes belong to Fresh's prior enrollment. Do not
        // replay registerSharedAuxiliaryIdentitySpans or generic candidate lanes.
        worldOwnersRegistered_=true;return true;
    }catch(...){return fail(d,"constructor shared Bank enrollment failed; actual pin/spans retained");}
}
}
// Common rewrite (area C): the RA aggregate composition members that lived here
// (prepareConstructorRosterStorage, bindConstructorQueryOwners, constructRetainedRuntimeSourceCamera,
// mintConstructorRacerWorldBorrow, registerConstructorQueryWorldOwners) are retired with the RA world.
