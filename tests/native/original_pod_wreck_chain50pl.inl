// Common rewrite (area C): the OwnershipBridge native wreck lifecycle and the fresh 165E90
// wreck-chain members (2129A4 .. 2CB628, 22C0A0 .. 22C25C, 19A7D0/19DDC8/15E024), moved
// unchanged from the retired RA world TU original_race_world_load_prefix50la.cpp. The only
// changes: the 4C0[0]/4C4[0] zone owner is the pod-load track provider (TrackDataProvider50PL
// over the race's one common Track, original_pod_load50pl.cpp) instead of RA's scene owner, and
// the World74 check reads the common World's Track. Included once, by original_pod_load50pl.cpp.
#pragma once
// Separate native cold inventory/query lifecycle. These hooks do not execute
//2129C0's source allocations or publish racer374, collection38, or any pieceD0.
namespace original_camera_session50KX {
bool Session::stageNativeRetainedWreckCold()noexcept {
    return !nativeWreckOwner_||nativeWreckOwner_->stageNativeRetainedWreckCold(*this);
}
bool Session::retireNativePlayerSpeechCold()noexcept {
    return !nativeWreckOwner_||nativeWreckOwner_->retireNativePlayerSpeechCold(*this);
}
bool Session::retireNativeRetainedWreckCold()noexcept {
    return !nativeWreckOwner_||nativeWreckOwner_->retireNativeRetainedWreckCold(*this);
}
}
namespace ra_source_camera50LA {
bool OwnershipBridge::stageNativeRetainedWreckCold(original_camera_session50KX::Session& session)noexcept {
    try {
    if(sourceIncomingZonesAttempted_||sourceIncomingZonesWithdrawing_||sourceWreckConstructorAttempted_)return false; //Distinct fresh chronology cannot enter legacy Follow adapter.
    if(!session.onOwnerThread()||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent()||closed_||terminal_||
       cameraPrepareTerminal_||nativeWreckRetired_||!storage_.get()||!follow_||!queriesConstructed_||
       session.actual_.followQueries.get()!=follow_.get()||!rosterResources_||!rosterResources_->wreckBacking_||
       !session.readsBound_||!session.identitiesCurrent()||session.closed_||session.abandoned_||
       !camera_->generation||session.generation_!=camera_->generation)return false;
    if(nativeWreckAttempted_)return nativeWreckPrepared_||inputs_.wreck.empty();
    nativeWreckAttempted_=true;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !backing.modelSourcesBound()||follow_->attempted_||follow_->ready_||follow_->closed_||follow_->synced_)
            return false;
        std::array<const g::OriginalWreckComponent50LA*,8> racers{};
        for(std::size_t i=0;i<session.actual_.bank.count();++i){
            const auto& racer=session.racerEntities_[i];
            if(!racer.prepared()||racer.racer.objectKey!=session.racerKey(i))return false;
            racers[i]=&racer.racer;
        }
        // No wreck rows in this world (the pod load does not run, as on every
        // other course today): nothing to bind before Follow; the backing is
        // released unbound at retirement. Follow then runs with zero rows.
        if(inputs_.wreck.empty()){
            std::fprintf(stderr,"wreck_rows_reserved rows=0 pod_load=0");std::fputc(10,stderr);
            return true;
        }
        std::string error;
        if(!backing.bindActualRacers(racers,session.actual_.bank.count(),session.generation_,error))return false;
        const auto* rows=backing.reservations();
        if(!rows||rows->empty()||rows->size()!=inputs_.wreck.size())return false;
        return session.actual_.bank.visit(0,[&](){
        // Real current native owner field, corroborated by its named registry;
        // not a copied guest-global/category value or a synthetic field.
        const auto& category=follow_->main_.dynamicMask;
        if(category!=g::OriginalNativeCollisionCategories::physicsBody||
           follow_->mainFilter_.categoryA4!=category)return false;
        std::vector<g::OriginalWreckPieceQuerySlots50LA> slots;slots.reserve(rows->size());
        g::OriginalWreckQueryWorld50LA common{};
        for(std::size_t i=0;i<rows->size();++i){
            const auto& row=(*rows)[i];const auto& admitted=inputs_.wreck[i];
            if(row.queries!=admitted.queries||row.assets!=admitted.assets||row.identities!=admitted.identities)return false;
            g::OriginalWreckQueryWorld50LA world{};
            if(!follow_->bindWreckQueryWorldCold(i,*row.queries,*row.assets,row.identities,category,world))return false;
            // The same bridge supplies every common lane. Only its actual
            // reserved refs/identities vary per occurrence.
            if(world.world!=&storage_.get()->world||world.lifecycle!=&storage_.get()->lifecycle)return false;
            slots.push_back({world.refs,world.identities});
            if(!i){common=world;common.refs={};common.identities={};}
        }
        if(!backing.prepareOrderedQueries(common,slots,error))return false;
        nativeWreckPrepared_=true;return true;
        });
    }catch(...){return false;} // Exact owners and any successful prefix retained.
}
bool OwnershipBridge::retireNativeRetainedWreckCold(original_camera_session50KX::Session& session)noexcept {
    if(!session.onOwnerThread()||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent())return false;
    if(!nativeWreckAttempted_||nativeWreckRetired_)return true;
    if(!storage_.get()||!follow_||!rosterResources_||!rosterResources_->wreckBacking_||
       session.actual_.followQueries.get()!=follow_.get())return false;
    auto& backing=*rosterResources_->wreckBacking_;
    // Close the exact Follow while its retained wreck aliases still live.
    // Never bypass real source memberships; close rejects any attached tail.
    try {if(!session.actual_.bank.visit(0,[&](){return follow_->close();}))return false;}
    catch(...){return false;}
    if(!nativeWreckPrepared_&&inputs_.wreck.empty()){
        // Zero-row world: nothing was bound or published; release the
        // never-bound reservations (refused if anything was adopted).
        if(!backing.releaseUnboundColdReservations())return false;
        podExclusions_.reset();nativeWreckRetired_=true;return true;
    }
    if(!backing.retireNativeSourcePrefixAll(storage_.get()->world))return false;
    if(!backing.retireNativeColdQueries(&storage_.get()->world))return false;
    // Any actual query publications have already passed their concrete source
    // destructor. The untouched later pieces remain cold-only cancellation.
    // Revoke all Follow aliases before destroying even the first piece owner.
    for(std::size_t i=0;i<follow_->wreck_.size();++i)follow_->wreck_[i]={};
    if(!backing.releaseNativeColdBacking())return false;
    inputs_.wreck.clear();
    podExclusions_.reset(); //The pieces whose E4 the lanes named have retired.
    if(podZonesHeld_){
        if(!podTrack_||!podTrack_->releaseWreckZones(*this))return false;
        podZonesHeld_=false;podTrack_=nullptr;
    }else if(nativeWreckZoneOwner_){
        if(!nativeWreckZoneOwner_->releaseWreckZones(*this))return false; //Clears nativeWreckZoneOwner_.
    }
    nativeWreckRetired_=true;return true;
}
}

// Common rewrite (R3): removed - the RA-only 1A9EE0 entry. The one track load is common_track50CR::Load (common_track_load50cr.cpp), run before the camera session for every course.
namespace ra_source_camera50LA {
bool OwnershipBridge::nativeFirstWreckApplicationCurrent(
    const original_camera_session50KX::Session& session,
    const original_main_entry50KG::Application& app)const noexcept {
    const auto state=app.state;
    return state&&app.entities()==session.actual_.applicationEntities&&
        session.actual_.arena.borrowedNoise50KG==&state->sharedNoise308030;
}
//Synchronous adapter to the same native frame/control owners. This does not
//upgrade the cold Entity's base dispatch to Player/AI or claim their loaders.
bool OwnershipBridge::nativeRacerInputOwnerCurrent(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model)const noexcept {
    const auto pin=bankLifetime_.lock();
    if(!session.onOwnerThread()||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||session.closed_||session.abandoned_||terminal_||closed_||
       cameraPrepareTerminal_||source348Arena_||!pin||!bankIdentity_||pin->identity!=bankIdentity_||
       bankIdentity_->raWorldLifetime_.get()!=pin.get()||!bankIdentity_->sealed_||bankIdentity_->retired_||
       bankIdentity_->visitDepth_!=1u||(!bankIdentity_->visiting_&&!bankIdentity_->readBlocked_)||
       bankIdentity_->sourceQueryPending()||model.slot_>=session.racerEntities_.size()||
       bankIdentity_->currentSlot_!=model.slot_||&session.actual_.bank!=bankIdentity_||
       !actual_||!gridArena_||!gridInputsRegistered(*gridArena_)||sharedOwnerLease_||sharedRoleViews_||
       nativeFifoPins_||triggerQueryActive_||sharedEffectsStopped_||!nativeWreckPrepared_||nativeWreckRetired_||
       !session.actual_.actualChariotModels||(*session.actual_.actualChariotModels)[model.slot_].get()!=&model||
       model.bank_!=bankIdentity_||!model.fixture_||
       &model.fixture_->parameters!=&gridArena_->parameters||model.arena_!=&actual_->frame||
       &session.actual_.arena!=&actual_->frame||!model.application_||
       model.application_!=session.actual_.applicationEntities||
       model.handlingRoute_!=native_racer_model50LA::Owner::HandlingRoute::legacy||
       model.generation_!=session.generation_)return false;
    const auto& racer=session.racerEntities_[model.slot_];
    return model.racer_==&racer&&model.attachedFor(racer)&&racer.prepared()&&!racer.nativeWreckWithdrawn_&&
        racer.racer.sessionGeneration==session.generation_&&racer.body340==model.body340_.get()&&
        racer.body340&&racer.body340->canonicalNumericAddress()==&actual_->frame.bodies.main&&
        registeredRange(&actual_->frame.fields.vehicle58,sizeof(actual_->frame.fields.vehicle58))&&
        registeredRange(&gridArena_->parameters.bodies.main,sizeof(gridArena_->parameters.bodies.main));
}
bool OwnershipBridge::nativeHandlingInputOwnerCurrent(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model)const noexcept {
    return model.nativePhysicalComplete_&&model.nativeRepulsorComplete_&&
        nativeRacerInputOwnerCurrent(session,model);
}
bool OwnershipBridge::readNativeHandlingTrack187330(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model,const g::OriginalRaceConstructor50LA& race,
    std::array<char,3>& out)const noexcept {
    //The ordinary ownsNativeCameraAdapter34 gate requires an idle Bank.
    //This getter instead runs inside the already-authenticated mutable visit.
    return nativeHandlingInputOwnerCurrent(session,model)&&race.cameraNativeAdapter34_==&session&&
        race.nativeCameraAdapter34LifetimeCurrent(session)&&race.world74_&&
        track_&&race.world74_->trackData()==track_&&
        camera_->display&&race.readSourceTrackName187330(out);
}
bool OwnershipBridge::readNativeHandlingConfiguration215434(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model,std::uint32_t& out)const noexcept {
    if(!nativeHandlingInputOwnerCurrent(session,model)||model.slot_!=0u||
       session.actual_.humanCount34!=1u||actual_->frame.fields.vehicle58!=0u||
       actual_->frame.fields.nativeAiOwnersBound50IV||!session.actual_.nativeHumanPlayer0)return false;
    //Native PlayerEventState is the SAME owner used by48Z/49A dispatch, not
    //a copied cold selection. It is explicitly not a2107E8 source receipt.
    const auto index=session.actual_.nativeHumanPlayer0->source5C;
    if(index>=g::controlPresetSlots49D)return false;
    out=session.actual_.preferences.slots[index].configuration11040;return true;
}
// 2129C0 per-racer load moved to original_pod_load50pl.cpp (PodLoad, every slot).
}

namespace ra_source_camera50LA {
bool OwnershipBridge::bindFreshSourceIncomingZones(original_camera_session50KX::Session& session)noexcept {
    if(!session.onOwnerThread()||!session.constructorOnly_||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent()||terminal_||closed_||cameraPrepareTerminal_||
       session.closed_||session.abandoned_||sourceIncomingZonesAttempted_||sourceIncomingZonesWithdrawing_||nativeWreckAttempted_||
       nativeWreckZoneOwner_||follow_||queriesConstructed_||!session.constructorWorld_||
       !session.actual_.applicationEntities||session.generation_!=camera_->generation)return false;
    const auto fresh=constructorArena_.lock();
    if(!fresh||!fresh->currentNative()||bankIdentity_!=&fresh->bank_||!actual_||&actual_->frame!=&fresh->live_.state)return false;
    const void* v=nullptr;const void* c=nullptr;
    const auto& lease=*session.constructorWorld_;
    if(!lease.readIncoming210770(session.generation_,
        &session.actual_.applicationEntities->objectCount423F60(),v,c))return false;
    // The race's one Track supplies 4C0[0]/4C4[0] (bindTrackDataProvider), every course.
    auto* track=podTrackStorage_.get();if(!track||podZonesHeld_)return false;
    sourceIncomingZonesAttempted_=true;
    g::OriginalWreckSetupZones50LA zones;
    original_pod_load50PL::ZoneRef50PL visibilityRef=0,collisionRef=0;
    if(!track->retainWreckZones(*this,session.generation_,zones,visibilityRef,collisionRef))return false;
    //Retained (nativeWreckZoneOwner_) before any later refusal/210770 effects.
    sourceIncomingVisibilityOrdinal_=visibilityRef;sourceIncomingCollisionOrdinal_=collisionRef;
    if(zones.nativeVisibilityNode!=v||zones.nativeCollisionNode!=c||!zones.visibility4C0||
       !zones.collision4C4||!zones.collisionLifecycle)return false;
    sourceIncomingVisibility_=v;sourceIncomingCollision_=c;
    sourceIncomingVisibilityWorld_=zones.visibility4C0;
    sourceIncomingCollisionWorld_=zones.collision4C4;sourceIncomingCollisionLifecycle_=zones.collisionLifecycle;
    sourceIncomingZonesReady_=true;return true;
}
bool OwnershipBridge::withdrawFreshSourceIncomingZones(original_camera_session50KX::Session& session)noexcept {
    if(!session.onOwnerThread()||!session.constructorOnly_||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent())return false;
    sourceIncomingZonesWithdrawing_=true; //One-way admission revocation BEFORE fallible source cleanup.
    return true;
}
bool OwnershipBridge::retireFreshSourceIncomingZones(original_camera_session50KX::Session& session)noexcept {
    if(!session.onOwnerThread()||!session.constructorOnly_||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent())return false;
    if(!sourceIncomingZonesAttempted_||sourceIncomingZonesWithdrawn_)return true;
    if(sourceWreckConstructorAttempted_&&!sourceWreckConstructorRetired_)return false;
    //Every actual Racer/model prefix has already retired. No copied ready flag
    //stands in for its count/reference teardown; failed earlier cleanup retains this borrow.
    for(std::size_t i=0;i<session.actual_.bank.count();++i)
        if(!session.racerEntities_[i].released())return false;
    if(nativeWreckZoneOwner_){
        if(!nativeWreckZoneOwner_->releaseWreckZones(*this))return false; //Clears nativeWreckZoneOwner_.
    }
    sourceIncomingZonesWithdrawn_=true;sourceIncomingZonesReady_=false;return true;
}
bool OwnershipBridge::loadFreshSourceWreckConfig2129A4(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    const std::vector<std::uint8_t>& phy,std::uint32_t& pc)noexcept {
    pc=0x2129a4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||nativeWreckPrepared_||follow_||queriesConstructed_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_||
           !rosterResources_->rigs_[slot]||rosterResources_->rigs_[slot]->phy!=phy)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.constructorWorld_->current()||
           session.generation_!=camera_->generation||slot>=session.actual_.bank.count()||
           &session.racerEntities_[slot]!=&racer||racer.racer.objectKey!=session.racerKey(slot)||
           racer.racer.sessionGeneration!=session.generation_)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||!backing.modelSourcesBound())return false;
        if(!sourceWreckConstructorAttempted_){
            sourceWreckConstructorAttempted_=true; //Custody before any per-owner identity mutation.
            std::array<const g::OriginalWreckComponent50LA*,8> racers{};
            for(std::size_t i=0;i<session.actual_.bank.count();++i){
                const auto& actual=session.racerEntities_[i];
                if(!actual.prepared()||actual.racer.objectKey!=session.racerKey(i)||
                   actual.racer.sessionGeneration!=session.generation_)return false;
                racers[i]=&actual.racer;
            }
            std::string error;
            if(!backing.selectFreshSourceIdentities(racers,session.actual_.bank.count(),session.generation_,error))return false;
        }
        return backing.loadFreshSourceFirstCollection2129A4(slot,racer,phy,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckPiece212A50(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x212a50u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||!sourceWreckConstructorAttempted_||
           sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||nativeWreckPrepared_||follow_||queriesConstructed_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.constructorWorld_->current()||
           !session.actual_.actualMaterialDirectory36B970||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||!backing.modelSourcesBound())return false;
        return backing.loadFreshSourceFirstPiece212A50(slot,racer,session.constructorWorld_,
            *session.actual_.actualMaterialDirectory36B970,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckSecondBody165E90(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x165e90u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        g::OriginalWreckSetupZones50LA zones;
        zones.visibility4C0=sourceIncomingVisibilityWorld_;
        zones.visibilityZoneField=&sourceIncomingVisibilityOrdinal_;
        zones.collision4C4=sourceIncomingCollisionWorld_;zones.collisionLifecycle=sourceIncomingCollisionLifecycle_;
        zones.collisionZoneField=&sourceIncomingCollisionOrdinal_;
        zones.nativeVisibilityNode=v;zones.nativeCollisionNode=c;
        return backing.loadFreshSourceSecondBody165E90(slot,racer,zones,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckBody165E90(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x165e90u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        g::OriginalWreckSetupZones50LA zones;
        zones.visibility4C0=sourceIncomingVisibilityWorld_;
        zones.visibilityZoneField=&sourceIncomingVisibilityOrdinal_;
        zones.collision4C4=sourceIncomingCollisionWorld_;zones.collisionLifecycle=sourceIncomingCollisionLifecycle_;
        zones.collisionZoneField=&sourceIncomingCollisionOrdinal_;
        zones.nativeVisibilityNode=v;zones.nativeCollisionNode=c;
        return backing.loadFreshSourceFirstBody165E90(slot,racer,zones,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckSecondE0166018(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x166018u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        std::size_t ordinal=0;
        if(!backing.freshSecondE0Reservation166018(slot,racer,ordinal))return false;
        const auto* rows=backing.reservations();
        if(!rows||ordinal>=rows->size()||rows->size()!=inputs_.wreck.size())return false;
        const auto& row=(*rows)[ordinal];const auto& reserved=inputs_.wreck[ordinal];
        if(row.queries!=reserved.queries||row.assets!=reserved.assets||row.identities!=reserved.identities)return false;
        auto& actual=*storage_.get();auto& app=*session.actual_.applicationEntities;
        const auto& physics=app.nativeCollisionCategoryBindings_[1];
        if(std::string_view(physics.name)!="PHYSICSBODY"||!physics.bit||!actual.count.wreckFirstQuery||
           ordinal>=actual.count.wreckReferences/3u)return false;
        g::OriginalWreckQueryWorld50LA world;
        world.lifecycle=&actual.lifecycle;world.world=&actual.world;world.workspace=&actual.membershipWork;
        world.objects=actual.objects.data();world.states=actual.source.data();world.objectCount=actual.count.objects;
        world.identities=row.identities;
        for(unsigned q=0;q<3;++q)world.refs[q]=static_cast<g::CollectionRef50BX>(actual.count.wreckFirstQuery+ordinal*3u+q);
        //Constructor-only metadata borrows the same reserved registry. Future
        //Follow copies/geometry/workspaces do not exist and are not fabricated.
        world.physicsBodyCategory3E0718=&physics.bit;
        world.sentinel3E02C0=&original_follow_query_bridge::QueryBridge::sentinel3E02C0;
        world.globalValid3E0730=&app.collisionGlobalValid3E0730_;
        return backing.loadFreshSourceSecondE0166018(slot,racer,world,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckE0166018(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x166018u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        const auto& span=backing.spans_[slot];const auto* rows=backing.reservations();
        if(!rows||!span.count||span.begin>=rows->size()||rows->size()!=inputs_.wreck.size())return false;
        const auto& row=(*rows)[span.begin];const auto& reserved=inputs_.wreck[span.begin];
        if(row.queries!=reserved.queries||row.assets!=reserved.assets||row.identities!=reserved.identities)return false;
        auto& actual=*storage_.get();auto& app=*session.actual_.applicationEntities;
        const auto& physics=app.nativeCollisionCategoryBindings_[1];
        if(std::string_view(physics.name)!="PHYSICSBODY"||!physics.bit||!actual.count.wreckFirstQuery||
           span.begin>=actual.count.wreckReferences/3u)return false;
        g::OriginalWreckQueryWorld50LA world;
        world.lifecycle=&actual.lifecycle;world.world=&actual.world;world.workspace=&actual.membershipWork;
        world.objects=actual.objects.data();world.states=actual.source.data();world.objectCount=actual.count.objects;
        world.identities=row.identities;
        for(unsigned q=0;q<3;++q)world.refs[q]=static_cast<g::CollectionRef50BX>(actual.count.wreckFirstQuery+span.begin*3u+q);
        //Constructor-only metadata borrows the same reserved registry. Future
        //Follow copies/geometry/workspaces do not exist and are not fabricated.
        world.physicsBodyCategory3E0718=&physics.bit;
        world.sentinel3E02C0=&original_follow_query_bridge::QueryBridge::sentinel3E02C0;
        world.globalValid3E0730=&app.collisionGlobalValid3E0730_;
        return backing.loadFreshSourceFirstE0166018(slot,racer,world,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckSecondE0Shape166074(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x166074u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.loadFreshSourceSecondE0Shape166074(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::prepareAppendFreshSourceWreckSecondE016607C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x16607cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.prepareAppendFreshSourceSecondE016607C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckE0Shape166074(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x166074u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.loadFreshSourceFirstE0Shape166074(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::prepareAppendFreshSourceWreckE016607C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x16607cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.prepareAppendFreshSourceFirstE016607C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondDC1660A0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x1660a0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceSecondDC1660A0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondD816616C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x16616cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceSecondD816616C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckDC1660A0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x1660a0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceFirstDC1660A0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckD816616C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x16616cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceFirstD816616C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructLoadFreshSourceWreckSecondE41661D8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x1661d8u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructLoadFreshSourceSecondE41661D8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::prepareAppendFreshSourceWreckSecondE416629C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x16629cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.prepareAppendFreshSourceSecondE416629C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructLoadFreshSourceWreckE41661D8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x1661d8u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructLoadFreshSourceFirstE41661D8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::prepareAppendFreshSourceWreckE416629C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x16629cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.prepareAppendFreshSourceFirstE416629C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::bindFreshSourceWreckSecondEffectInputs2CB618(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb618u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        //The first piece already acquired this exact World-issued manager pin.
        //Do not replace it or construct an alternate context/domain for piece1.
        auto* manager=nativeWreckZoneOwner_->particles4A0();
        if(!manager||nativeWreckParticles_!=manager||!manager->prepared()||!manager->particles())return false;
        return backing.bindFreshSourceSecondEffectInputs2CB618(slot,racer,*manager,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondMutual166390(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x166390u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceSecondMutual166390(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondE81662E4(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x1662e4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceSecondE81662E4(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckE81662E4(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x1662e4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        return backing.constructFreshSourceFirstE81662E4(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::stopFreshSourceWreckSecondFirstPair2CB778(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb778u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.stopFreshSourceSecondFirstPair2CB778(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::stopFreshSourceWreckFirstPair2CB778(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb778u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.stopFreshSourceFirstPair2CB778(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::loadFreshSourceWreckSecondPiece22C0A0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c0a0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        if(!session.actual_.actualMaterialDirectory36B970)return false;
        return backing.loadFreshSourceSecondPiece22C0A0(slot,racer,*session.actual_.actualMaterialDirectory36B970,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::advanceFreshSourceWreckSecondPieceCursor22C25C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c25cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.advanceFreshSourceSecondPieceCursor22C25C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::setupFreshSourceWreckGroupSparks2CC0D4(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cc0d4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        const std::string* directory=nullptr;
        directory=nativeWreckZoneOwner_->particleDirectory36B530();
        return backing.setupFreshSourceGroupSparks2CC0D4(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::beginFreshSourceWreckGroupSparksConfig15F2F0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x15f2f0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.beginFreshSourceGroupSparksConfig15F2F0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckGroupSparks2CC0B4(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cc0b4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.constructFreshSourceGroupSparks2CC0B4(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::consumeFreshSourceWreckGroupContext2CC088(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cc088u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.consumeFreshSourceGroupContext2CC088(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::bindFreshSourceWreckGroupPrimary22C298(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c298u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.bindFreshSourceGroupPrimary22C298(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckGroup22C278(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c278u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.constructFreshSourceGroup22C278(slot,racer,session.actual_.applicationEntities->groupOrigin4441F0_,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::advanceFreshSourceWreckPieceCursor22C25C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c25cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.advanceFreshSourcePieceCursor22C25C(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::applyFreshSourceWreckSecondWater22C21C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c21cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        const auto& water=session.actual_.applicationEntities->nativeCollisionCategoryBindings_[5];
        if(std::string_view(water.name)!="WATER"||!water.bit)return false;
        //Same App HOST namespace used by existing track/queries; not guest4342A8 numeric bits.
        return backing.applyFreshSourceSecondWater22C21C(slot,racer,water.bit,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::applyFreshSourceWreckWater22C21C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x22c21cu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        const auto& water=session.actual_.applicationEntities->nativeCollisionCategoryBindings_[5];
        if(std::string_view(water.name)!="WATER"||!water.bit)return false;
        //Same App HOST namespace used by existing track/queries; not guest4342A8 numeric bits.
        return backing.applyFreshSourceWater22C21C(slot,racer,water.bit,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::appendFreshSourceWreckSecondPiece22C1B8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb8c4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.appendFreshSourceSecondPiece22C1B8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::appendFreshSourceWreckPiece22C1B8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb8c4u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.appendFreshSourcePiece22C1B8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readStopAppendFreshSourceWreckSplash19DDC8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19ddc8u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readStopAppendFreshSourceSplash19DDC8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readStopAppendFreshSourceWreckDust19A7D0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19a7d0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readStopAppendFreshSourceDust19A7D0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readAppendFreshSourceWreckFire15E024(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x15e024u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readAppendFreshSourceFire15E024(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::completeFreshSourceWreckSecondInitialChild19A7F8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19a7f8u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.completeFreshSourceSecondInitialChild19A7F8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::completeFreshSourceWreckSecondSuccessorChild(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    if(pc!=0x15e0b0u&&pc!=0x19de7cu)return false;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.completeFreshSourceSecondSuccessorChild(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readFreshSourceWreckSecondSuccessorChild(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    if(pc!=0x15e024u&&pc!=0x19ddc8u)return false;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readFreshSourceSecondSuccessorChild(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readStopAppendFreshSourceWreckSecondPositiveDust19A7D0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19a7d0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readStopAppendFreshSourceSecondPositiveDust19A7D0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readStopAppendFreshSourceWreckSecondPositiveSplash19DDC8(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19ddc8u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readStopAppendFreshSourceSecondPositiveSplash19DDC8(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readFreshSourceWreckSecondInitialChild19A7D0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19a7d0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readFreshSourceSecondInitialChild19A7D0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::readAppendFreshSourceWreckSmoke19A7D0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x19a7d0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.readAppendFreshSourceSmoke19A7D0(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::continueFreshSourceWreckSecondPairs2CB6CC(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb6ccu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed only by each actual smoke/fire setup after its reached manager stores.
        const std::string* directory=nullptr;
        directory=nativeWreckZoneOwner_->particleDirectory36B530();
        return backing.continueFreshSourceSecondPairs2CB6CC(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::continueFreshSourceWreckPairs2CB6CC(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb6ccu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed only by each actual smoke/fire setup after its reached manager stores.
        const std::string* directory=nullptr;
        directory=nativeWreckZoneOwner_->particleDirectory36B530();
        return backing.continueFreshSourcePairs2CB6CC(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckFire2CB720(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb720u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed by15DFDC AFTER the actual fire constructor/manager stores.
        const std::string* directory=nullptr;
        directory=nativeWreckZoneOwner_->particleDirectory36B530();
        return backing.constructFreshSourceFire2CB720(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondPositiveSplash(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb854u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed only by the positive Splash setup AFTER constructor/manager stores.
        const std::string* directory=nullptr;
        directory=nativeWreckZoneOwner_->particleDirectory36B530();
        return backing.constructFreshSourceSecondPositiveSplash(slot,racer,
            &session.actual_.applicationEntities->particleDefault433F50_,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondSuccessorChild(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    if(pc!=0x2cb720u&&pc!=0x2cb854u)return false;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed only by the selected Fire/Splash setup AFTER constructor/manager stores.
        const std::string* directory=nullptr;
        directory=nativeWreckZoneOwner_->particleDirectory36B530();
        return backing.constructFreshSourceSecondSuccessorChild(slot,racer,
            &session.actual_.applicationEntities->particleDefault433F50_,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSplash2CB854(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb854u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed by19DD80 AFTER the actual splash constructor/manager stores.
        const std::string* directory=nullptr;
        directory=nativeWreckZoneOwner_->particleDirectory36B530();
        return backing.constructFreshSourceSplash2CB854(slot,racer,
            &session.actual_.applicationEntities->particleDefault433F50_,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondPositiveDust(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb7e0u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed by19A788 AFTER the actual selected child constructor/manager stores.
        const std::string* directory=nullptr;
        directory=nativeWreckZoneOwner_->particleDirectory36B530();
        return backing.constructFreshSourceSecondPositiveDust(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondInitialChild(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    if(pc!=0x2cb6ccu&&pc!=0x2cb7e4u)return false;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed by19A788 AFTER the actual selected child constructor/manager stores.
        const std::string* directory=nullptr;
        directory=nativeWreckZoneOwner_->particleDirectory36B530();
        return backing.constructFreshSourceSecondInitialChild(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckDust2CB7E0(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    if(pc!=0x2cb7e0u&&pc!=0x2cb7e4u)return false;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed by19A788 AFTER the actual dust constructor/manager stores.
        const std::string* directory=nullptr;
        directory=nativeWreckZoneOwner_->particleDirectory36B530();
        return backing.constructFreshSourceDust2CB7E0(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSmoke2CB6CC(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb6ccu;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        //Only bind immutable directory authority here; missing authority is
        //consumed by19A788 AFTER the actual smoke constructor/manager stores.
        const std::string* directory=nullptr;
        directory=nativeWreckZoneOwner_->particleDirectory36B530();
        return backing.constructFreshSourceSmoke2CB6CC(slot,racer,directory,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckSecondArrays2CB628(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb628u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.constructFreshSourceSecondArrays2CB628(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckArrays2CB628(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x2cb628u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get()||
           !nativeWreckParticles_||nativeWreckParticles_!=nativeWreckZoneOwner_->particles4A0()||
           !nativeWreckParticles_->prepared()||racer.particles4A0_!=nativeWreckParticles_->particles())return false;
        return backing.constructFreshSourceArrays2CB628(slot,racer,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::constructFreshSourceWreckMutual166390(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
    std::uint32_t& pc)noexcept {
    pc=0x166390u;
    try {
        const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
        if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           terminal_||closed_||cameraPrepareTerminal_||!sourceIncomingZonesReady_||sourceIncomingZonesWithdrawing_||sourceIncomingZonesWithdrawn_||
           !sourceWreckConstructorAttempted_||sourceWreckConstructorWithdrawing_||sourceWreckConstructorRetired_||
           nativeWreckAttempted_||follow_||queriesConstructed_||!nativeWreckZoneOwner_||
           !camera_||!camera_->session||!actual_||&actual_->frame!=&fresh->live_.state||
           !rosterResources_||!rosterResources_->wreckBacking_||slot>=rosterResources_->racerCount_)return false;
        auto& session=*camera_->session;
        if(!session.onOwnerThread()||!session.constructorOnly_||session.nativeWreckOwner_!=this||
           session.closed_||session.abandoned_||session.actual_.followQueries.get()||session.actual_.actualCables||
           !session.constructorWorld_||!session.actual_.applicationEntities||session.generation_!=camera_->generation||
           slot>=session.actual_.bank.count()||&session.racerEntities_[slot]!=&racer||
           racer.racer.objectKey!=session.racerKey(slot)||racer.racer.sessionGeneration!=session.generation_||
           !nativeWreckZoneOwner_->zonesHeldBy(*this,sourceIncomingVisibility_,sourceIncomingCollision_)||
           !storage_.get()||
           sourceIncomingCollisionWorld_!=&storage_.get()->world||
           sourceIncomingCollisionLifecycle_!=&storage_.get()->lifecycle||!sourceIncomingVisibilityWorld_)return false;
        const void* v=nullptr;const void* c=nullptr;
        if(!session.constructorWorld_->readIncoming210770(session.generation_,
            &session.actual_.applicationEntities->objectCount423F60(),v,c)||
           v!=sourceIncomingVisibility_||c!=sourceIncomingCollision_||
           racer.binding_.actualIncomingVisibility4C0!=v||racer.binding_.actualIncomingCollision4C4!=c)return false;
        auto& backing=*rosterResources_->wreckBacking_;
        if(backing.applicationIdentity()!=session.actual_.applicationEntities.get())return false;
        if(!backing.constructFreshSourceMutual166390(slot,racer,pc))return false;
        //Only now at2CB618 consume the actual incoming Track4A0 context.
        //1AA57C -> wrapper chain ->210770 t2; same object as Racer210890.
        auto* manager=nativeWreckZoneOwner_->particles4A0();
        if(!manager||!manager->prepared()||!manager->particles())return false;
        if(nativeWreckParticles_){
            if(nativeWreckParticles_!=manager)return false; //Same private World-issued pin; release clears both sides.
        }else if(!nativeWreckZoneOwner_->retainWreckParticles(*this,manager))return false;
        return backing.bindFreshSourceEffectInputs2CB618(slot,racer,*manager,pc);
    }catch(...){return false;}
}
bool OwnershipBridge::retireFreshSourceWreckConstructors(original_camera_session50KX::Session& session)noexcept {
    if(!session.onOwnerThread()||!session.constructorOnly_||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent())return false;
    if(!sourceWreckConstructorAttempted_||sourceWreckConstructorRetired_)return true;
    if(closed_||follow_||queriesConstructed_||nativeWreckAttempted_||!rosterResources_||!rosterResources_->wreckBacking_)return false;
    sourceWreckConstructorWithdrawing_=true; //One-way; a rejected cleanup cannot reopen construction.
    auto& backing=*rosterResources_->wreckBacking_;
    if(!backing.retireFreshSourceConstructors())return false;
    //There are no prepared/active query aliases on this route. Still perform
    //the same two-phase native cancellation before their retained backing dies.
    for(const auto& row:inputs_.wreck)if(!row.queries||row.queries->prepared()||row.queries->constructed())return false;
    if(!backing.retireNativeColdQueries(nullptr))return false;
    inputs_.wreck.clear(); //Withdraw actual Bridge borrows BEFORE backing destruction.
    if(!backing.releaseNativeColdBacking())return false;
    sourceWreckConstructorRetired_=true;return true;
}
}
