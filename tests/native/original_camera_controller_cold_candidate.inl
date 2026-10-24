// Included privately INSIDE the actual Session class. No independent owner,
// callable dispatch, source-clock input, parent attachment or readiness grant.
bool constructColdController174A30(std::uint64_t issuerGeneration)noexcept {
    auto& p=coldController_;const auto& app=actual_.applicationEntities;
    if(nativeTaCutChildPhase_!=NativeTaCutChildPhase::empty||
       !issuerGeneration||p.started||p.entity.reference00||!app||!app->application10()||
       app->nativeOwnerThread_!=std::this_thread::get_id()||app->attachmentLock423F58()||
       app->objectCount423F60()==std::numeric_limits<std::uint32_t>::max()||
       generation_||displayInputs_||coldPhase_||authoredPhase_||coldBound_||readsBound_||
       closed_||abandoned_||frameStage_!=FrameStage::idle||controller_.bound||
       collection_.bound||collection_.registered38||collection_.count30||collection_.selected34||
       controllerEntity_.controllerKey||objectChildren_.controllerKey||
       !original_entity_chain_detail50LA::forest(app->entityAuthority_.view()))return false;
    for(const auto* displayed:display_.controllers48)if(displayed)return false;
    // The existing nonmoving numeric payload supplies this controller's ONE
    // identity. The Entity prefix is separate solely because numeric leaves
    // copy controller_; those copies must never copy links/reference ownership.
    const auto identity=key(controller_);
    if(!app->entityAuthority_.reserveAdditional(1)||
       !app->entityAuthority_.canEnroll(p.entity,identity))return false;
    p.started=true;p.issuerGeneration=issuerGeneration;
    //1A95D0 a1=1 survives174A30->115B60->141300. Native authority enrollment
    // is reserved beforehand; constructor itself adds exactly one real count.
    const auto made=initializeOriginalWreckComponent50LA(p.entity,
        OriginalWreckComponentKind50LA::cameraController174A30,
        issuerGeneration,identity,true,app->objectCount423F60());
    if(made.status!=OriginalWreckComponentStatus50LA::complete||
       !app->entityAuthority_.enroll(p.entity))std::terminate();
    p.vtable08=0x3ccab0u; //115B7C: source identity, never an executable pointer.
    collection_.count30=0;collection_.selected34=0;collection_.registered38=nullptr;
    p.scene3C=nullptr;
    p.vtable08=0x3ce310u; //174A4C.
    p.mode84=0;p.racer40=nullptr;
    const auto bits60=[&](std::uint32_t mask,std::uint32_t value)noexcept {
        p.word60Value=(p.word60Value&~mask)|(value&mask);p.word60Known|=mask;
    };
    const auto bitsA8=[&](std::uint16_t mask,std::uint16_t value)noexcept {
        p.halfA8Value=static_cast<std::uint16_t>((p.halfA8Value&~mask)|(value&mask));
        p.halfA8Known=static_cast<std::uint16_t>(p.halfA8Known|mask);
    };
    bitsA8(0x000fu,0); //174A74, only low cursor nibble.
    for(auto& child:p.children64)child=nullptr; //174A78..94, source order64..80.
    p.script5C=nullptr;
    bits60(0x001e0000u,0);controller_.flags62&=static_cast<std::uint8_t>(~0x1eu);
    bitsA8(0x01f0u,0); //174AD4, signed5-bit cut count.
    controller_.mode44=4;
    bits60(0x00200000u,0);controller_.flags62&=static_cast<std::uint8_t>(~0x20u);
    bits60(0x00400000u,0);controller_.flags62&=static_cast<std::uint8_t>(~0x40u);
    bits60(0x00800000u,0);controller_.flags62&=static_cast<std::uint8_t>(~0x80u);
    bits60(0x0001f000u,0);controller_.flags62&=static_cast<std::uint8_t>(~1u); //174B58 includes62 bit0.
    controller_.cooldown8C=0.f;controller_.cutRemaining90=0.f;
    bits60(0x01000000u,0);controller_.flags63Low5&=static_cast<std::uint8_t>(~1u);
    bits60(0x02000000u,0);controller_.flags63Low5&=static_cast<std::uint8_t>(~2u);
    controller_.specialDelayA4=1.f;
    bits60(0x04000000u,0x04000000u);controller_.flags63Low5|=4u;
    controller_.shakeDuration98=0.f;controller_.shakeRemaining94=-1.f;
    controller_.shakeMagnitude9C=0.f;
    bitsA8(0x0200u,0);controller_.shakeDecayingA9=false; //174BC0.
    controller_.shakeDelayA0=0.f;
    bitsA8(0x0400u,0);
    bits60(0x08000000u,0);controller_.flags63Low5&=static_cast<std::uint8_t>(~8u);
    bits60(0x10000000u,0);controller_.flags63Low5&=static_cast<std::uint8_t>(~16u);
    for(auto& query:p.queries48)query=nullptr; //174BF8..C04, source order48..54.
    p.word58=0; //174C08; no destructor/ownership claim for this word.
    // Do not bind the legacy display/object-child projections or race48.
    // No174DF0, collection registration, parent edge, or scene is constructed.
    return true;
}
bool coldControllerIdentity174A30()const noexcept {
    const auto& p=coldController_;const auto& e=p.entity;const auto& app=actual_.applicationEntities;
    if(!p.started||p.released||!p.issuerGeneration||!app||!app->application10()||
       app->nativeOwnerThread_!=std::this_thread::get_id()||app->attachmentLock423F58()||
       e.kind!=OriginalWreckComponentKind50LA::cameraController174A30||e.reference00!=1||
       e.sessionGeneration!=p.issuerGeneration||e.objectKey!=key(controller_)||
       e.previous10||e.next14||e.firstChild18||e.parent1C||e.mask20||e.descendants24||
       e.category28!=0xffu||e.flags29!=3u||e.nativeEffectControlId||
       !app->entityAuthority_.canRetire(e)||
       !original_entity_chain_detail50LA::forest(app->entityAuthority_.view())||
       p.vtable08!=0x3ce310u||p.scene3C||p.racer40||p.script5C||p.mode84||p.word58||
       p.word60Known!=0x1ffff000u||p.word60Value!=0x04000000u||
       p.halfA8Known!=0x07ffu||p.halfA8Value||controller_.bound||
       controller_.mode44!=4||controller_.flags62||controller_.flags63Low5!=4||
       !same(controller_.cooldown8C,0.f)||!same(controller_.cutRemaining90,0.f)||
       !same(controller_.shakeRemaining94,-1.f)||!same(controller_.shakeDuration98,0.f)||
       !same(controller_.shakeMagnitude9C,0.f)||!same(controller_.shakeDelayA0,0.f)||
       !same(controller_.specialDelayA4,1.f)||controller_.shakeDecayingA9||
       collection_.bound||collection_.registered38||collection_.count30||collection_.selected34||
       controllerEntity_.controllerKey||objectChildren_.controllerKey||
       generation_||displayInputs_||coldPhase_||authoredPhase_||coldBound_||readsBound_)return false;
    for(const auto* query:p.queries48)if(query)return false;
    for(const auto* child:p.children64)if(child)return false;
    for(const auto* displayed:display_.controllers48)if(displayed)return false;
    return true;
}
bool releaseColdController174C20()noexcept {
    auto& p=coldController_;
    if(!p.started||p.released)return !p.entity.reference00;
    if(!coldControllerIdentity174A30()||!actual_.applicationEntities->objectCount423F60())return false;
    auto& authority=actual_.applicationEntities->entityAuthority_;
    if(!authority.retire(p.entity))std::terminate();
    --p.entity.reference00; //142590->104970 last reference, before subclass destructor.
    // Generic child18 is authentically empty.174C20's64/68/6C/70,5C,74,
    //78,7C and48..54 branches are also genuinely empty, not skipped callbacks.
    p.vtable08=0x3ce310u;
    p.vtable08=0x3ccab0u; //115BB0; actual registered38 is null, so no list free.
    p.vtable08=0x3cd240u; //141404, Entity base destructor.
    --actual_.applicationEntities->objectCount423F60(); //141420 delay slot.
    p.vtable08=0x3cc980u; //10493C, reference base destructor.
    p.released=true; // Native history, never reset for reuse/admission.
    return true;
}
