#include <cmath>
#include <limits>
#include <cstring>
#include "live_audio50hf_fixture.h"
// Included once after the actual Operation/Session definitions in the retained
// roster TU. These are synchronous native-owner joins, not counted350/354
// adoption, a Player+A0 receipt, or full source210770/Config chronology.
namespace ra_source_camera50LA {
ra_cold_roster50LA::Operation* OwnershipBridge::nativeRacerOperationCurrent(
    original_camera_session50KX::Session& session,const native_racer_model50LA::Owner& model)const noexcept {
    if(!nativeRacerInputOwnerCurrent(session,model))return nullptr;
    const auto pin=std::static_pointer_cast<BankPin>(bankLifetime_.lock());
    auto* op=pin&&pin->owner.get()==this&&pin->rosterOperation?pin->rosterOperation.get():nullptr;
    if(!op||op->terminal_||op->closed_||op->constructorOnly_||!op->aggregate_||
       op->aggregate_->terminal_||op->aggregate_->closed_||op->aggregate_->runtimeQueries_.get()!=this||
       op->bank_!=bankIdentity_||op->generation_!=model.generation_||op->entities_!=model.application_||
       op->resources_!=rosterResources_||!op->resources_||!op->owners_||
       &op->arena_.live!=model.fixture_||&op->arena_.live.state!=&actual_->frame||
       model.slot_>=op->chariot_.size()||op->chariot_[model.slot_].get()!=&model||
       op->resources_->rigs_[model.slot_].get()!=model.actualRig())return nullptr;
    return op;
}
ra_cold_roster50LA::Operation* OwnershipBridge::nativeHandlingOperationCurrent(
    original_camera_session50KX::Session& session,const native_racer_model50LA::Owner& model)const noexcept {
    return nativeHandlingInputOwnerCurrent(session,model)?nativeRacerOperationCurrent(session,model):nullptr;
}
rr::assets::NativePhyLoad50LA* OwnershipBridge::nativePhyOwnerCurrent(
    original_camera_session50KX::Session& session,const native_racer_model50LA::Owner& model)const noexcept {
    auto* op=nativeRacerOperationCurrent(session,model);
    if(!op||!model.finishedFor(session.racerEntities_[model.slot_]))return nullptr;
    auto& owner=op->nativePhyLoads_[model.slot_];
    const rr::assets::NativePhyLoad50LA::Context context{op,bankIdentity_,model.generation_,model.slot_};
    const auto& bound=model.nativePhyContext_;
    if(model.nativePhyLoad_!=&owner||bound.operation!=context.operation||bound.bank!=context.bank||
       bound.generation!=context.generation||bound.slot!=context.slot||!model.rig_||
       !owner.current(context,model.rig_->phy))return nullptr;
    return &owner;
}
ra_cold_roster50LA::Operation* OwnershipBridge::nativePlayerOperationCurrent(
    original_camera_session50KX::Session& session,const native_racer_model50LA::Owner& model)const noexcept {
    auto* op=nativeRacerOperationCurrent(session,model);
    if(!op||model.slot_!=0u||op->arena_.humanCount!=1u||session.actual_.humanCount34!=1u)return nullptr;
    const auto& c=op->arena_.nativePlayerCaller;const auto& h=op->nativePlayerLoad_;
    const auto lane=[&](const auto* p){return p&&registeredRange(p,sizeof(*p));};
    if(!h.armed||h.retired||c.bank_!=bankIdentity_||!c.ingress_||!c.actions_||
       !c.racer60_||c.racer60_!=&session.actual_.racer60||
       !lane(c.queue_)||!lane(c.application_)||!lane(c.child_)||!lane(c.player_)||!lane(c.environment_)||
       !op->nativePlayerCameraOwnersCurrent(session.actual_.nativeHumanPlayer0,session.actual_.humanHeld)||
       !c.authored_||c.authored_!=op->arena_.selectedAuthored||!c.steering10A4_||
       c.steering10A4_!=op->arena_.selectedSteering||c.child_->player!=c.player_||
       c.child_->environment!=c.environment_||c.child_->bindings!=h.bindings||c.child_->childIndex!=0u||
       c.player_->source5C!=h.source5C||c.player_->indexFFC!=h.indexFFC||h.source5C>=8u||
       c.actions_->deviceIndex()!=h.source5C||
       c.environment_->held.devices[h.source5C]!=&c.ingress_->buttons()||
       actual_->frame.fields.vehicle58!=0u||actual_->frame.fields.nativeAiOwnersBound50IV)return nullptr;
    // The original one-child router was authenticated before cold consumers.
    // A later camera publication may legitimately add its second child. This
    // retained input identity is not permission to dispatch a changed router.
    return op;
}
bool OwnershipBridge::bindNativePlayerLoad1A0848(original_camera_session50KX::Session& session,
    native_racer_model50LA::Owner& model)noexcept {
    auto* op=nativePlayerOperationCurrent(session,model);if(!op)return false;
    auto& h=op->nativePlayerLoad_;auto& racer=session.racerEntities_[0];
    if(h.session||h.model||h.racer||h.nativeReturn||h.returnAttempted||h.returned||
       h.pc!=0x1a0848u||!model.finishedFor(racer))return false;
    // Forward join before this actual racer's first Wreck/handling effects.
    h.session=&session;h.model=&model;h.racer=&racer;h.racerKey=racer.racer.objectKey;return true;
}
bool OwnershipBridge::continueNativePlayerReturn1A0850(original_camera_session50KX::Session& session,
    native_racer_model50LA::Owner& model,std::uint32_t& pc)noexcept {
    auto* op=nativePlayerOperationCurrent(session,model);
    if(!op||nativeHandlingOperationCurrent(session,model)!=op)return false;
    auto& h=op->nativePlayerLoad_;auto& racer=session.racerEntities_[0];
    if(h.returnAttempted){pc=h.pc;return false;} // No replay of reached stores.
    const auto& outer=racer.nativeOuterStorage_;auto& phy=op->nativePhyLoads_[0];
    const auto& context=model.nativePhyContext_;
    if(h.session!=&session||h.model!=&model||h.racer!=&racer||h.racerKey!=racer.racer.objectKey||
       h.pc!=0x1a0848u||pc!=0x212eb4u||model.failurePC_!=pc||racer.nativePostEmberPC_!=pc||
       !outer.attempted||outer.pc!=pc||outer.bank!=bankIdentity_||outer.generation!=session.generation_||
       outer.slot!=0u||!outer.nativePhyClosed||outer.nativeReturnRacer!=&racer||
       model.nativePhyLoad_!=&phy||racer.nativePhyLoad_!=&phy||context.operation!=op||
       context.bank!=bankIdentity_||context.generation!=session.generation_||context.slot!=0u||
       !phy.closed()||phy.aborted()||phy.activeReaders()!=0u)return false;
    // The owner is successfully CLOSED. A live-reader predicate would reject
    // it; never reopen/borrow/reparse merely to authenticate this continuation.
    h.returnAttempted=true;
    const auto at=[&](std::uint32_t address)noexcept {pc=h.pc=address;};
    at(0x1a0850u);
    const auto* aero=model.nativeAero_.get();
    if(!aero||!aero->published_||aero->revoked_||aero->sourceFresh_||aero->bank_!=bankIdentity_||
       aero->frame_!=&actual_->frame||aero->parameters_!=&op->arena_.live.parameters.bodies.main||
       aero->model_!=&model||aero->body_!=model.body340_.get()||aero->application_!=model.application_.get()||
       aero->slot_!=0u||aero->generation_!=session.generation_||aero->racer_!=&racer||
       aero->racerKey_!=racer.racer.objectKey)return false;
    at(0x1a0854u);h.nativeReturn=&racer; // Typed SAME native receiver, not a guest v0 word.
    at(0x1a0858u);
    static_assert(sizeof(float)==sizeof(std::uint32_t),"native Player steering requires binary32 storage");
    if(!registeredRange(&aero->parameters_->aero.steering100,sizeof(float)))return false;
    std::memcpy(&h.steeringWord,&aero->parameters_->aero.steering100,sizeof(h.steeringWord));
    h.steeringLoaded=true;
    at(0x1a085cu);
    // Source1A0858/5C is a raw load/store. No arithmetic, finite-value guard,
    // profile replay or copy from authored metadata may replace these bits.
    std::memcpy(op->arena_.nativePlayerCaller.steering10A4_,&h.steeringWord,sizeof(h.steeringWord));
    h.steeringStored=true;at(0x1a0890u);h.returned=true;return true;
}
bool OwnershipBridge::continueNativePlayerResources1AE104(original_camera_session50KX::Session& session,
    native_racer_model50LA::Owner& model,const g::OriginalRaceConstructor50LA& race,std::uint32_t& pc)noexcept {
    auto* op=nativePlayerOperationCurrent(session,model);
    if(!op||nativeHandlingOperationCurrent(session,model)!=op)return false;
    auto& h=op->nativePlayerLoad_;auto& racer=session.racerEntities_[0];
    if(h.managerAttempted){pc=h.managerPC;return false;} // This call is never replayed.
    const auto& outer=racer.nativeOuterStorage_;const auto& phy=op->nativePhyLoads_[0];
    if(!h.returned||!h.steeringLoaded||!h.steeringStored||h.pc!=0x1a0890u||pc!=h.pc||
       h.session!=&session||h.model!=&model||h.racer!=&racer||h.nativeReturn!=&racer||
       h.racerKey!=racer.racer.objectKey||h.managerReceiver||h.resourceRace||h.resourceDomain||h.resourceContext||
       !outer.nativePhyClosed||outer.nativeReturnRacer!=&racer||outer.pc!=0x212eb4u||
       model.failurePC_!=outer.pc||racer.nativePostEmberPC_!=outer.pc||
       model.nativePhyLoad_!=&phy||racer.nativePhyLoad_!=&phy||!phy.closed()||phy.aborted()||phy.activeReaders()||
       race.cameraNativeAdapter34_!=&session||!race.nativeCameraAdapter34LifetimeCurrent(session)||
       !race.world74_||race.world74_->runtimeQueries_.get()!=this||race.world74_->terminal_||race.world74_->closed_||
       race.application_!=op->entities_||!race.resourcesPrepared_||!op->domain_||
       race.resourceDomain_!=op->domain_||op->application_->standardMaterialDomain()!=op->domain_||
       session.actual_.applicationMaterials!=op->domain_->materials382B38()||!race.resourceContext_)return false;
    // Native manager custody consumes the actual SAME returned receiver.
    // No guest s1 word, source manager array44 publication or derived Player
    // dispatch follows merely from this typed native pointer.
    h.managerAttempted=true;
    const auto at=[&](std::uint32_t address)noexcept {pc=h.managerPC=address;};
    at(0x1ae104u);h.managerReceiver=h.nativeReturn;
    at(0x1ae108u);h.resourceRace=&race;
    h.resourceDomain=race.resourceDomain_.get();h.resourceContext=race.resourceContext_.get();
    // Retain both diagnostics, including any material/texture partial effects.
    // A failed pair stays at the original callsite with its callee diagnostic;
    // neither it nor the earlier successful Race population is replayed.
    if(!race.populateNativeReturnedPlayerResources1AA5A0(h.resourceDiagnostic))return false;
    h.resourcePairReturned=true;at(0x1ae110u);
    // Next original operation reads the actual receiver's dispatch. Player
    // and base Racer share+98=22A1D0 but have DIFFERENT+9C targets. The SAME
    // native base object does not acquire Player dispatch or D40 ownership.
    return nativePlayerOperationCurrent(session,model)==op&&nativeHandlingOperationCurrent(session,model)==op;
}
bool OwnershipBridge::bindNativePlayerSpeechForward22A210(ra_cold_roster50LA::Operation& op)noexcept {
    auto& h=op.nativePlayerLoad_;const auto& c=op.arena_.nativePlayerCaller;
    const auto pin=bankLifetime_.lock();
    // Already inside the actual human visit: visit(0) uses readBlocked_,
    // unlike batch visits. The idle-only cameraBankCurrent cannot apply here.
    if(op.constructorOnly_||op.closed_||op.terminal_||op.bank_!=bankIdentity_||
       !pin||!bankIdentity_||pin->identity!=bankIdentity_||bankIdentity_->raWorldLifetime_.get()!=pin.get()||
       !bankIdentity_->sealed_||bankIdentity_->retired_||bankIdentity_->count_!=inputs_.collisionRoster.size()||
       bankIdentity_->visitDepth_!=1u||(!bankIdentity_->visiting_&&!bankIdentity_->readBlocked_)||
       bankIdentity_->currentSlot_!=0u||
       !actual_||&actual_->frame!=&op.arena_.live.state||op.arena_.humanCount!=1u||
       !h.armAttempted||h.armed||h.speechForwardAttempted||h.retired||
       !c.audio_||!c.speechCounts_||!c.speechState_||!c.racer60_||
       !registeredRange(c.speechState_,sizeof(*c.speechState_))||
       !op.application_||op.application_->entities()!=op.entities_||!op.application_->state||
       op.arena_.live.state.borrowedNoise50KG!=&op.application_->state->sharedNoise308030)return false;
    h.speechForwardAttempted=true;
    h.speechForwardBound=c.audio_->bindNativeHumanSpeechForward22A210(*op.application_,*op.bank_,
        op.entities_,op.generation_,(*c.racer60_)[0],*c.speechCounts_,*c.speechState_,
        op.application_->state->rawRandom308030);
    return h.speechForwardBound;
}
bool OwnershipBridge::withdrawNativePlayerSpeechForward22A210(ra_cold_roster50LA::Operation& op)noexcept {
    auto& h=op.nativePlayerLoad_;const auto& c=op.arena_.nativePlayerCaller;
    if(!h.speechForwardBound)return !h.speechEntity;
    // No Session/constructor is required for cancellation of an unused
    // forward borrow. A real D48 must retire through its actual owner first.
    if(h.speechEntity||(h.speechPublished&&!h.speechRetired)||!c.audio_||!op.application_||!op.bank_||
       !c.audio_->canWithdrawNativeHumanSpeechForward22A210(*op.application_,*op.bank_,op.generation_))return false;
    if(!c.audio_->withdrawNativeHumanSpeechForward22A210(*op.application_,*op.bank_,op.generation_))std::terminate();
    h.speechForwardBound=false;h.speechRetired=true;return true;
}
bool OwnershipBridge::continueNativeBaseRacerSpeech22A1D0(original_camera_session50KX::Session& session,
    native_racer_model50LA::Owner& model,const g::OriginalRaceConstructor50LA& race,std::uint32_t& pc)noexcept {
    auto* op=nativePlayerOperationCurrent(session,model);
    if(!op||nativeHandlingOperationCurrent(session,model)!=op)return false;
    auto& h=op->nativePlayerLoad_;const auto& c=op->arena_.nativePlayerCaller;
    auto& racer=session.racerEntities_[0];
    if(h.speechCallAttempted){pc=h.speechPC;return false;}
    if(!h.resourcePairReturned||h.managerPC!=0x1ae110u||pc!=h.managerPC||
       h.managerReceiver!=&racer||h.nativeReturn!=&racer||h.resourceRace!=&race||
       h.session!=&session||h.model!=&model||h.racer!=&racer||h.racerKey!=racer.racer.objectKey||
       racer.actualCounter423F60()!=&op->entities_->objectCount423F60()||
       racer.actualLock423F58()!=&op->entities_->attachmentLock423F58()||
       !racer.nativeDispatch20F390Known_||!racer.sourceAudioD40_||*racer.sourceAudioD40_||
       !racer.nativeSpeechD48_||*racer.nativeSpeechD48_||h.speechEntity||h.speechRetired||
       !op->application_||op->application_->race28C()!=&race||
       race.cameraNativeAdapter34_!=&session||!race.nativeCameraAdapter34LifetimeCurrent(session)||
       !race.world74_||race.world74_->runtimeQueries_.get()!=this||race.application_!=op->entities_)return false;
    h.speechCallAttempted=true;
    const auto at=[&](std::uint32_t address)noexcept {pc=h.speechPC=address;};
    at(0x1ae110u);
    // Actual20F3D4 history identifies this SAME native base receiver. The
    // shared+98 target22A1D0 is NOT evidence of Player3CED50/+9C1A1E60.
    at(0x1ae118u);at(0x22a1e0u);at(0x173c90u);
    // Base3D1790+9C is exactly JR RA/NOP. No generic callback/no-op fallback.
    at(0x173c94u);h.base9CReturned=true;at(0x22a1f0u);at(0x22a220u);
    // Race stores six groups in source call order, not the seven-group enum order.
    constexpr std::size_t characterSlot=3u;
    static_assert(g::OriginalRaceConstructor50LA::groupOrder_[characterSlot]==g::RaceAudioGroup50IC::Character11C);
    if(!h.speechForwardBound||!c.audio_||!c.speechCounts_||!c.speechState_||!c.racer60_||
       c.racer60_!=&session.actual_.racer60||!registeredRange(c.speechState_,sizeof(*c.speechState_))||
       !op->application_||!op->application_->state||!race.raceAudio_||
       !race.raceGroupsAttempted_||race.raceGroupCount_!=race.raceGroups_.size()||
       race.raceGroupsAttached_!=race.raceGroups_.size()||
       race.raceAudio_!=&c.audio_->groups_||
       !race.raceGroups_[characterSlot]||
       session.actual_.arena.borrowedNoise50KG!=&op->application_->state->sharedNoise308030)return false;
    const auto made=c.audio_->constructNativeHumanSpeech2C1000(*op->application_,*op->bank_,op->entities_,
        op->generation_,session.actual_.racer60[0],race.entity_,
        *race.raceGroups_[characterSlot],racer.racer,
        op->application_->state->rawRandom308030);
    at(made.sourcePC);h.speechEntity=made.entity;
    if(made.status!=rr::platform::NativePlayerSpeechConstructorStatus50LA::constructorComplete)return false;
    if(!made.entity||made.sourcePC!=0x2c10a0u||
       made.entity->kind!=g::OriginalWreckComponentKind50LA::speech2C1000||made.entity->reference00!=1u||
       made.entity->sessionGeneration!=session.generation_||made.entity->parent1C||
       made.entity->previous10||made.entity->next14||made.entity->firstChild18||
       !op->entities_->entityAuthority_.canRetire(*made.entity))return false;
    h.speechConstructorComplete=true;
    at(0x22a23cu);racer.nativeSpeechD48_=made.entity;h.speechPublished=true;
    // Native allocation refusal never manufactures guest NULL. A real
    // successful constructor publishes its exact owner before the load call.
    at(0x22a240u);at(0x22a24cu);at(0x2c1170u);
    // Native input custody at the reached load boundary. Use the SAME mounted
    // namespace and Resources-owned selected Rig, not loose extraction or the
    // earlier independently prepared native catalog as source-loader success.
    if(!op->domain_||op->application_->standardMaterialDomain()!=op->domain_||
       !op->aggregate_->rosterMountComplete_||op->aggregate_->rosterMountDomain_!=op->domain_||
       op->aggregate_->rosterMountApplication_!=op->application_||!op->resources_||
       op->resources_->rigs_[0].get()!=model.actualRig())return false;
    const auto& provider=op->domain_->archiveProvider3E1A70();
    const std::shared_ptr<const native_racer50HW::Rig> rig(op->resources_,op->resources_->rigs_[0].get());
    if(!c.audio_->retainNativeHumanSpeechInputs2C1170(*op->application_,*op->bank_,op->generation_,
        provider,rig,racer.racer,made.entity))return false;
    at(0x2c1190u);
    // Use the actual21081C-published source+60 field on this SAME racer.
    // Its210824-produced name1E0 and the host selected catalog are not this LW.
    if(!racer.sourcePod60_||*racer.sourcePod60_!=session.actual_.racer60[0])return false;
    auto prefixPC=pc;
    const bool filenameReturned=c.audio_->beginNativeHumanSpeechConfig2C1190(*op->application_,*op->bank_,
        op->generation_,op->application_->nativeAudioDirectory36AFE0(),racer.racer,made.entity,
        *racer.sourcePod60_,prefixPC);
    at(prefixPC);if(!filenameReturned)return false;
    return false; //2C11C8: original Config open/file manager, RNG, B4 and attachment remain pending.
}
bool OwnershipBridge::retireNativePlayerSpeechCold(original_camera_session50KX::Session& session)noexcept {
    if(!session.onOwnerThread()||!camera_||camera_->session.get()!=&session||
       session.nativeWreckOwner_!=this||!cameraBankCurrent()||&session.actual_.bank!=bankIdentity_)return false;
    const auto pin=std::static_pointer_cast<BankPin>(bankLifetime_.lock());
    auto* op=pin&&pin->owner.get()==this&&pin->rosterOperation?pin->rosterOperation.get():nullptr;
    if(!op)return !session.racerEntities_[0].nativeSpeechD48_||
                  !*session.racerEntities_[0].nativeSpeechD48_; // No orphaned owner may bypass retirement.
    auto& h=op->nativePlayerLoad_;const auto& c=op->arena_.nativePlayerCaller;
    if(!h.speechForwardBound)return !h.speechEntity;
    if(op->bank_!=bankIdentity_||op->generation_!=session.generation_||!op->application_||!c.audio_||
       (h.session&&h.session!=&session)||!c.racer60_||c.racer60_!=&session.actual_.racer60)return false;
    try{return session.actual_.bank.visit(0,[&](){
        if(!h.speechEntity)return withdrawNativePlayerSpeechForward22A210(*op);
        auto& racer=session.racerEntities_[0];auto* const historical=h.speechEntity;
        if(h.racer!=&racer||h.racerKey!=racer.racer.objectKey||!h.speechConstructorComplete||
           !h.speechPublished||!racer.nativeSpeechD48_||*racer.nativeSpeechD48_!=historical||
           !c.audio_->canRetireNativeHumanSpeech2C10B0(*op->application_,*op->bank_,op->generation_,racer.racer,historical))return false;
        // Same-thread preflight has no callbacks or allocation. Withdraw the
        // native alias, never invent a source NULL write, before actual free.
        racer.nativeSpeechD48_.reset();h.speechEntity=nullptr;
        if(!c.audio_->retireNativeHumanSpeech2C10B0(*op->application_,*op->bank_,op->generation_,racer.racer,historical))std::terminate();
        h.speechForwardBound=false;h.speechRetired=true;return true;
    });}catch(...){return false;}
}
bool OwnershipBridge::readNativeHandlingContact215900(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model,g::SpringGeneratedParameters48K*& spring,
    g::SpringResponseParameters48L*& response)const noexcept {
    spring=nullptr;response=nullptr;
    auto* op=nativeHandlingOperationCurrent(session,model);if(!op)return false;
    auto& t=op->arena_.tuning;
    if(!registeredRange(&t.mainSpring,sizeof(t.mainSpring))||
       !registeredRange(&t.mainResponse,sizeof(t.mainResponse))||
       op->contacts_[model.slot_].resources!=op->resources_->collisions_[model.slot_].get()||
       !op->contacts_[model.slot_].resources)return false;
    spring=&t.mainSpring;response=&t.mainResponse;return true;
}
bool OwnershipBridge::publishNativeHandlingContact215A70(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model)const noexcept {
    g::SpringGeneratedParameters48K* spring=nullptr;g::SpringResponseParameters48L* response=nullptr;
    if(!readNativeHandlingContact215900(session,model,spring,response))return false;
    auto* op=nativeHandlingOperationCurrent(session,model);if(!op)return false;
    // Existing per-racer immutable native contact descriptors are projections
    // of these same Bank lanes. This does not rebuild an already-prepared peer
    // contact coordinator or confer its activation/registration authority.
    op->contacts_[model.slot_].settings=*spring;op->contacts_[model.slot_].response=*response;
    return true;
}
bool OwnershipBridge::readNativeHandlingCooling215CC8(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model,g::BoostParameters50FK*& boost,float*& cooling)const noexcept {
    boost=nullptr;cooling=nullptr;
    auto* op=nativeHandlingOperationCurrent(session,model);if(!op)return false;
    auto& t=op->arena_.tuning;
    if(!registeredRange(&t.boost,sizeof(t.boost))||!registeredRange(&t.coolingCF8,sizeof(t.coolingCF8)))return false;
    boost=&t.boost;cooling=&t.coolingCF8;return true;
}
bool OwnershipBridge::readNativeHandlingProfile216040(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model,const g::OriginalRaceConstructor50LA& race,
    bool& profileRequired,std::array<float,6>& abilities)const noexcept {
    if(!nativeHandlingOperationCurrent(session,model)||race.cameraNativeAdapter34_!=&session||
       !race.nativeCameraAdapter34LifetimeCurrent(session)||!race.world74_||
       race.world74_->runtimeQueries_.get()!=this||race.world74_->terminal_||race.world74_->closed_)return false;
    // The actual Application350 is a borrowed native process field. Nonzero
    // skips216040 without requiring or inventing a profile ability row.
    if(session.actual_.application350!=0){profileRequired=false;return true;}
    if(!race.profile_||!race.profile_->initialized()||model.slot_>=session.actual_.racer60.size())return false;
    const auto before=race.profile_->stamp();
    std::uint8_t unusedPacked1125D=0;std::uint32_t career100C0=0;
    g::OriginalTournamentPodCareer50KG row;
    // Existing read-only getter exposes the exact retained100C0 value. It
    // imposes neither mode2 nor a diagonal career==pod/Garage assumption.
    if(!race.profile_->riderConfirmSnapshot(unusedPacked1125D,career100C0)||
       !race.profile_->tournamentPodCareerSnapshot(career100C0,session.actual_.racer60[model.slot_],row))return false;
    const auto after=race.profile_->stamp();
    if(after.generation!=before.generation||after.mode34!=before.mode34)return false;
    abilities=row.abilities1920;profileRequired=true;return true;
}
bool OwnershipBridge::publishNativeHandlingDifference21600C(original_camera_session50KX::Session& session,
    const native_racer_model50LA::Owner& model,float value)const noexcept {
    auto* op=nativeHandlingOperationCurrent(session,model);if(!op||!std::isfinite(value))return false;
    auto& a=op->arena_;auto& s=a.live.state;
    if(!registeredRange(&a.tuning.boost,sizeof(a.tuning.boost)))return false;
    const bool human=model.slot_==0u&&a.humanCount==1u;
    const bool ai=s.fields.nativeAiOwnersBound50IV;
    if(human){if(ai||!a.selectedAuthored||!a.selectedSteering||s.fields.vehicle58!=0u)return false;}
    else if(!ai||!a.ai.initialized||!registeredRange(&a.ai,sizeof(a.ai)))return false;
    // All native ownership checks precede the actual engine5C write. Its
    // source-authoritative native consumer is the same Bank-owned boost lane.
    a.tuning.boost.engineDifference5C=value;
    if(human){
        auto& authored=*a.selectedAuthored;
        authored.main=a.live.parameters.bodies.main;
        authored.controls=a.live.parameters.control;authored.rates=a.live.parameters.rates;
        authored.engineDifference5C=value;
        *a.selectedSteering=a.live.parameters.bodies.main.aero.steering100;
    }else a.ai.engineDifference5C=value;
    return true;
}
bool OwnershipBridge::continueNativeOuterStorage212E18(original_camera_session50KX::Session& session,
    native_racer_model50LA::Owner& model,std::uint32_t& pc)noexcept {
    auto* op=nativeHandlingOperationCurrent(session,model);
    if(!op||model.slot_!=0u||!model.finishedFor(session.racerEntities_[0])||
       !model.nativeHandlingReturned_||!model.nativeSpinReturned_||
       model.nativeSpinPC_!=0x212e18u||!model.nativeHandlingStack_||
       !camera_||session.actual_.actualCables!=&camera_->cables||
       !camera_->cablePins[0]||!camera_->cables[0]||
       !camera_->cables[0]->nativeOuterReturned_||
       camera_->cables[0]->nativeOuterResult_.sourcePC!=0x212e10u||
       &session.actual_.racerTiming!=&op->arena_.lap.timing||
       &session.actual_.bindings!=&op->arena_.live.bindings)return false;
    auto& racer=session.racerEntities_[0];auto& h=racer.nativeOuterStorage_;
    auto* phy=nativePhyOwnerCurrent(session,model);
    if(!phy||racer.nativePhyLoad_!=phy)return false;
    if(h.attempted){pc=h.pc;return false;} //Completed effects are never replayed.
    if(pc!=0x212e18u||model.failurePC_!=pc||racer.nativePostEmberPC_!=pc||
       !racer.sourceWater334_||*racer.sourceWater334_||racer.nativePreviousBE0_||
       h.bank||h.lapTimesCC0||h.impact.partnerId||h.impactStores||
       !registeredRange(session.actual_.racerTiming.lapTimesCC0,
           sizeof(session.actual_.racerTiming.lapTimesCC0)))return false;
    h.attempted=true;h.bank=bankIdentity_;h.generation=session.generation_;h.slot=model.slot_;
    const auto at=[&](std::uint32_t address)noexcept {
        pc=h.pc=model.failurePC_=racer.nativePostEmberPC_=address;
    };
    at(0x212e1cu);
    // Native fixed-backing policy: the original100x4 calloc's zeroed result
    // uses the EXISTING banked100-float owner. No allocator was invoked and
    // no guest NULL/success result is synthesized. Only CC0 contents change;
    // later216690's CB0/CB4/CB8/CBC/CD0/CD4 initializer is not called.
    std::memset(session.actual_.racerTiming.lapTimesCC0,0,
        sizeof(session.actual_.racerTiming.lapTimesCC0));
    at(0x212e24u);h.lapTimesCC0=session.actual_.racerTiming.lapTimesCC0;

    at(0x212e28u);
    const auto* scene=racer.chariotModel1B4;
    const auto* object=scene?scene->model():nullptr;
    const auto graph=model.graph();
    if(!scene||scene!=model.modelScene()||!scene->constructed2403A0()||scene->released()||
       !object||!object->constructed||object->released||!object->entity().reference00||
       object->entity().sessionGeneration!=session.generation_||
       object->sceneIndex!=model.rig_->roots[native_racer_model50LA::Rig::chariot]||
       object->sceneIndex!=session.actual_.bindings.lifecycle.model1B4||
       graph.nodes!=actual_->frame.nodes||graph.count!=actual_->frame.sceneNodeCount||
       !registeredRange(graph.nodes,graph.count*sizeof(g::SceneNode49P))||
       !g::validateSceneGraph49P(graph))return false;
    const auto index=static_cast<std::int32_t>(object->sceneIndex);
    if(index<0||static_cast<std::uint32_t>(index)>=graph.count)return false;
    at(0x28b300u);
    // Actual getter stack40 starts from VF0. It supplies W=1 even when the
    // current world matrix's fourth translation lane has different bits.
    std::array<std::uint32_t,4> position{{0u,0u,0u,0x3f800000u}};
    auto& target=graph.nodes[static_cast<std::uint32_t>(index)];
    at(0x28b304u);
    if(!(target.sceneFlags190&1u)){
        if(target.parent180!=g::SceneNodeIndex49P::none){
            at(0x28b320u);
            // 2897B0 updates only its dirty ancestor chain. Refresh oldest
            // first so a later native arithmetic refusal preserves earlier
            // matrices/ready flags. No sibling/child or bounds refresh.
            std::array<g::SceneNodeIndex49P,g::maxSceneNodes49P> chain{};
            std::size_t size=0;auto cursor=target.parent180;
            while(cursor!=g::SceneNodeIndex49P::none){
                const auto n=static_cast<std::uint32_t>(cursor);
                if(graph.nodes[n].sceneFlags190&1u)break;
                chain[size++]=cursor;cursor=graph.nodes[n].parent180;
            }
            while(size){const auto item=chain[--size];
                auto& node=graph.nodes[static_cast<std::uint32_t>(item)];
                if(node.parent180==g::SceneNodeIndex49P::none){
                    // The root's source branch is an integer-word copy,
                    // independent of floating-point rounding/finiteness.
                    std::memcpy(&node.world100,&node.localC0,sizeof(node.world100));
                    node.sceneFlags190=static_cast<std::uint8_t>(node.sceneFlags190|1u);
                }else if(!g::refreshSceneWorld49P(graph,item))return false;
                ++h.refreshedNodes;
            }
            at(0x28b334u);
            // The now-ready parent makes this one-node composition. Existing
            // finite binary32/FE_TONEAREST admission remains explicit; a
            // rejection is not an exceptional EE/VU execution receipt.
            if(!g::refreshSceneWorld49P(graph,object->sceneIndex))return false;
            ++h.refreshedNodes;
        }else{
            //28B350..36C copies eight pairs without evaluating the floats.
            for(unsigned pair=0;pair<8u;++pair){
                at(0x28b35cu);std::memcpy(target.world100.v+2u*pair,target.localC0.v+2u*pair,4);
                at(0x28b360u);std::memcpy(target.world100.v+2u*pair+1u,target.localC0.v+2u*pair+1u,4);
            }
            at(0x28b384u);target.sceneFlags190=static_cast<std::uint8_t>(target.sceneFlags190|1u);
            ++h.refreshedNodes;
        }
    }
    //relative=NULL skips28BD50. These are raw copies of the current cache,
    //not re-read authored transforms, a pose snapshot or normalized values.
    constexpr std::uint32_t outputPC[3]={0x28b38cu,0x28b394u,0x28b3a0u};
    for(unsigned i=0;i<3u;++i){at(outputPC[i]);std::memcpy(&position[i],target.world100.v+12u+i,4);}
    at(0x28b3d8u); //Actual getter return, including its delay slot.
    at(0x212e44u);racer.nativePreviousBE0_=position; //Delay slot BEFORE allocator effects.

    at(0x212e40u);
    auto& supplement=actual_->frame.impactSupplement;auto& fields=actual_->frame.fields;
    if(!registeredRange(&supplement.partnerId,sizeof(supplement.partnerId))||
       !registeredRange(&supplement.partnerState,sizeof(supplement.partnerState))||
       !registeredRange(&supplement.partnerTimer08,sizeof(supplement.partnerTimer08))||
       !registeredRange(&supplement.partnerAmount0C,sizeof(supplement.partnerAmount0C))||
       !registeredRange(&fields.objectD34_10,sizeof(fields.objectD34_10)))return false;
    // D34's five canonical consumers already exist, split across two Bank
    // records. Publish a native descriptor rather than allocate a dead mirror
    // or pretend these unrelated addresses are one contiguous20-byte block.
    at(0x212e48u);h.impact={&supplement.partnerId,&supplement.partnerState,
        &supplement.partnerTimer08,&supplement.partnerAmount0C,&fields.objectD34_10};
    const std::uint32_t zero=0u;
    at(0x212e58u);*h.impact.partnerId=0xffffffffu;++h.impactStores;
    at(0x212e60u);*h.impact.partnerState=0u;++h.impactStores;
    at(0x212e68u);std::memcpy(h.impact.timer08,&zero,4);++h.impactStores;
    at(0x212e70u);std::memcpy(h.impact.amount0C,&zero,4);++h.impactStores;
    // The store belongs to212E78's delay slot and precedes1573B0 entry.
    at(0x212e7cu);std::memcpy(h.impact.duration10,&zero,4);++h.impactStores;
    h.sourcePhyClose1573B0Unrepresented=true;at(0x1573b0u);
    //The forward native owner actually existed before cold bulk consumers
    //and supplied every shared handling selection/read. Close only its real
    //cache/cursor lifetime; immutable Rig bytes and all live outputs survive.
    //No original file00/child24/text1C/cache438 receipt is synthesized.
    if(nativePhyOwnerCurrent(session,model)!=phy||racer.nativePhyLoad_!=phy||
       !phy->close(model.nativePhyContext_,model.rig_->phy))return false;
    h.nativePhyClosed=true;
    //Typed native ABI return of the SAME actual racer, not a guest v0 word or
    //proof of source1573B0 execution. Player1A0850/camera joins remain gated.
    at(0x212e80u);h.nativeReturnRacer=&racer;
    at(0x212eb4u);return true;
}
} // namespace ra_source_camera50LA
