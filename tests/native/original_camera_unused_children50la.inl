// Included privately inside the one real Session. These source camera-behavior
// objects are UNCOUNTED:115AE0 writes only vtable00, not Entity141300. Their
// owning controller is counted; no Entity parent/count or live registry entry
// is invented for these previously omitted70/78 children.
struct NativeCinematographer70 final {
    std::uint32_t vtable00=0;
    std::array<std::uint32_t,4> vector10{},vector20{},vector30{},vector40{};
    std::uint32_t field50=0,field54=0,field58=0,field5C=0,field60=0,field64=0;
    std::uint32_t field68=0,field6C=0,field70=0,field74=0,field78=0,field7C=0;
    std::uint32_t field80=0,field84=0,field88=0,fieldA4=0,fieldA8=0,fieldAC=0;
    std::array<const void*,6> allocations8C{};
    // Unwritten04..0F/B0..DF are absent, not certified zero backing.
};
struct NativeTv78 final {
    std::uint32_t vtable00=0,field04=0;
    std::array<std::uint32_t,4> vector10{},vector30{},vector60{},vector70{};
    const OriginalRacerMainBodyOwner50LA* body20=nullptr;
    std::optional<std::uint32_t> field08,field0C; //Unwritten by the constructor.
    std::uint32_t field24=0,field40=0,field44=0,field4C=0,field50=0,field80=0;
    const void* allocation48=nullptr;
    std::array<std::uint32_t,4> vectorA0{},vectorB0{};
    std::uint32_t fieldC0=0,fieldC4=0,fieldC8=0,fieldCC=0,fieldD0=0;
    const void* allocationD4=nullptr;
    const void* allocationD8=nullptr;
    const void* allocationE0=nullptr;
    // Constructor-unwritten filter/hold90 etc. remain unrepresented. No
    // selector reset, authored TV load, world query, or virtual update exists.
};
std::unique_ptr<NativeCinematographer70> nativeCinematographer70_;
std::unique_ptr<NativeTv78> nativeTv78_;
bool nativeCinematographer70Attempted_=false,nativeTv78Attempted_=false;
bool nativeCinematographer70Retired_=false,nativeTv78Retired_=false;
bool nativeCinematographer70Published_=false;
bool nativeTvBody20Attempted_=false,nativeTvBody20Revoked_=false;

// Cold constructor/lifetime complement of the EXISTING start_ payload, never
// a second camera identity. Only the bounded native-owned preparation (the
// 174DF0 load operands published on this controller) may create this prefix,
// on any course; the existing live numeric preparation is not changed.
// Embedded native storage replaces allocation175060 explicitly.
// PORT STAND-IN (flagged): no route publishes those 174DF0 operands today, so
// Start7C (and TvBody below) stay unreached on every course alike.
struct NativeStart7CPrefix final {
    bool constructed=false,retired=false;
    std::uint32_t vtable00=0;
    std::array<const void*,7> allocations244{}; //244..25C, freed by2D4CD0.
    const void* field260=nullptr; //Constructor clears it; destructor DOES NOT free it.
} nativeStart7C_;

bool constructNativeRaStart7C()noexcept {
    if(!onOwnerThread())return false;
    if(!nativeRaOwnedPreparationAttempted_)return true; //No change to legacy live routes.
    auto& p=nativeStart7C_;
    if(p.constructed||p.retired||start_.prepared||start_.active||start_.sessionGeneration||
       coldController_.children64[6]||!nativeTv78_||nativeTv78Retired_||
       coldController_.children64[5]!=nativeTv78_.get()||
       !coldController_.nativeLoadOperandsPublished||coldController_.nativeLoadOperandsRevoked||
       !coldController_.nativeScene3COwner||!coldController_.race88||
       !nativeTaIssuance_||
       nativeTaIssuance_->generation!=generation_||coldController_.issuerGeneration!=generation_||
       !actual_.applicationEntities||!actual_.applicationEntities->application10()||
       actual_.applicationEntities->attachmentLock423F58()||closed_||abandoned_||coldBound_||
       (!nativeIntro177750_.childrenAttempted&&(coldPhase_!=2u||!taCuts_||!taCuts_->loaded())))
        return taReject(0x175060);
    if(nativeIntro177750_.childrenAttempted&&(!nativeRaOwnedPreparationAttempted_||
       !nativeAttached6C_.completed||nativeIntro177750_.childrenComplete))return taReject(0x175060);
    p.constructed=true;
    p.vtable00=0x3cca90u; //115AE8: uncounted base, no Entity enrollment.
    p.vtable00=0x3d5210u; //2D4BB8.
    const RaceStartVector vf0{0.f,0.f,0.f,1.f};
    //2E8860 invokes2D4CC0 twelve times at30+i*20. Only the first
    //vector is written; native neighbor defaults are NOT constructor facts.
    for(auto& record:start_.grid)record.position=vf0;
    start_.initialTarget1B0=vf0;start_.unfilteredPosition1C0=vf0;
    start_.look1D0=vf0;start_.step1E0=vf0; //2D4BDC..2D4C00.
    start_.maxFov04=0.f;start_.initialFov08=0.f;start_.reachRadius0C=8.f;
    start_.nextRoute1F8=0;
    start_.flag230=false;start_.zoomIn234=false;start_.zoomOut238=false;
    start_.handoff23C=false;start_.complete240=false;
    start_.routeCount1FC=0;start_.target1F0=-1;
    for(auto& allocation:p.allocations244)allocation=nullptr; //2D4C68..94.
    p.field260=nullptr; //2D4CA0. No assignment to source-unwritten1F4/filters.
    coldController_.children64[6]=&start_; //175078, AFTER constructor return.
    return true; //No2D4DC0, RNG, registry, selection, resources or readiness.
}
bool retireNativeRaStart7C()noexcept {
    if(!onOwnerThread())return false;
    auto& p=nativeStart7C_;
    if(!p.constructed)return true; //Existing nonbounded/live route unchanged.
    if(p.retired)return !coldController_.children64[6];
    if(coldController_.children64[6]!=&start_||start_.prepared||start_.active)return false;
    //No producer can allocate into this private cold-only prefix. Reject an
    //unsupported allocation rather than pretending to execute source free.
    for(const auto* allocation:p.allocations244)if(allocation)return false;
    p.vtable00=0x3d5210u; //2D4CF4, seven constructor-null branches skip.
    p.vtable00=0x3cca90u; //2D4D88 ->115B00. field260 deliberately untouched.
    coldController_.children64[6]=nullptr; //Native borrow withdrawal, not source store.
    p.retired=true;return true; //Embedded storage remains inert until Session destruction.
}

bool constructNativeRaCinematographer70()noexcept {
    // Every Application-issued course session (one rule, no course gate).
    if(!nativeTaIssuance_)return true;
    if(nativeCinematographer70Attempted_||nativeCinematographer70Retired_||nativeCinematographer70_||
       !coldControllerIdentity174A30())return taReject(0x174f6c);
    nativeCinematographer70Attempted_=true;
    try {nativeCinematographer70_.reset(new NativeCinematographer70);}
    catch(...){return taReject(0x174f6c);} //Native allocation rejection, never unchecked null call.
    auto& c=*nativeCinematographer70_;
    c.vtable00=0x3cca90u; //115AE8, sole uncounted base constructor effect.
    c.vtable00=0x3ccc20u; //118A2C.
    const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
    c.vector10=vf0;c.vector20=vf0;c.vector30=vf0;c.vector40=vf0; //118A34..4C.
    c.field50=0x3fc90fdbu;c.field54=0;c.field58=0;c.fieldAC=0x42a00000u;
    const std::array<std::uint32_t,4> position{{0x44bb8000u,0x42c80000u,0x44bb8000u,0x3f800000u}};
    c.vector10=position;c.vector20=position; //118AA0/A4.
    c.field78=0;c.field7C=0;c.field84=1;c.field80=0;
    c.field68=0x42b40000u;c.field6C=0x41c80000u;c.fieldA4=1;c.fieldA8=0;
    for(auto& allocation:c.allocations8C)allocation=nullptr; //118AC8..DC.
    c.field70=0;c.field74=0;c.field88=0;c.field5C=0;c.field60=0;c.field64=0;
    //Prewarmed backing only. Actual alias adoption follows6C at174F84;
    //the unadopted standalone route retains its explicitly native staging.
    return true;
}
bool constructNativeRaTv78()noexcept {
    // Every Application-issued course session (one rule, no course gate).
    if(!nativeTaIssuance_)return true;
    const auto& app=actual_.applicationEntities;
    if(nativeTv78Retired_||!app||app->nativeOwnerThread_!=std::this_thread::get_id()||
       !app->application10()||app->attachmentLock423F58()||closed_||abandoned_||
       !coldController_.started||coldController_.released||
       coldController_.issuerGeneration!=generation_||generation_!=nativeTaIssuance_->generation||
       !app->entityAuthority_.canRetire(coldController_.entity)||
       !nativeCinematographer70_||coldController_.children64[3]!=nativeCinematographer70_.get()||
       (!nativeIntro177750_.childrenAttempted&&(!taCuts_||!taCuts_->loaded()||
          coldController_.children64[4]!=taCuts_.get()||!actual_.taIntroAudio||
          !actual_.taIntroAudio->taIntroReady()||
          coldController_.children64[7]!=actual_.taIntroAudio->taIntroEntity80())))return taReject(0x175044);
    if(nativeIntro177750_.childrenAttempted&&(!nativeRaOwnedPreparationAttempted_||
       !nativeAttached6C_.completed||nativeIntro177750_.childrenComplete||
       (nativeAttached6C_.pc==0x175038&&!nativeIntroSourceCurrent())))return taReject(0x175044);
    if(nativeTv78Attempted_)
        return nativeTv78_&&coldController_.children64[5]==nativeTv78_.get(); //Same completed owner, no replay.
    if(nativeTv78_||coldController_.children64[5])return taReject(0x175064);
    nativeTv78Attempted_=true;
    try {nativeTv78_.reset(new NativeTv78);}
    catch(...){return taReject(0x175044);}
    auto& c=*nativeTv78_;
    c.vtable00=0x3cca90u; //115AE8.
    c.vtable00=0x3d5130u; //2D1F9C.
    const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
    c.vector10=vf0;c.vector30=vf0;c.vector60=vf0;c.vector70=vf0;
    c.field04=0x42700000u;c.body20=nullptr;c.field24=0;
    c.vector30=app->tvCameraOrigin444400_; //2D1FD4 actual process read,2D1FE0 store.
    c.field40=0xffffffffu;c.field50=0;c.field44=0;c.allocation48=nullptr;c.field80=0;
    c.vtable00=0x3d5160u; //2D288C.
    c.vectorA0=vf0;c.vectorB0=vf0;
    c.allocationD4=nullptr;c.allocationD8=nullptr;c.field4C=0;c.allocationE0=nullptr;
    c.vectorB0={{0,0,0,0x3f800000u}}; //2D28B0..D0, source stack vector.
    c.fieldC0=0;c.fieldC4=0;c.fieldCC=0;c.fieldC8=0;c.fieldD0=0xffffffffu;
    coldController_.children64[5]=&c; //175064: AFTER2D2870 returns.
    return true;
}
bool bindNativeRaTvBody1752A0()noexcept {
    if(!onOwnerThread())return false;
    if(!nativeRaOwnedPreparationAttempted_||nativeTvBody20Attempted_||nativeTvBody20Revoked_||
       !nativeTv78_||nativeTv78Retired_||coldController_.children64[5]!=nativeTv78_.get()||
       !coldController_.nativeLoadOperandsPublished||coldController_.nativeLoadOperandsRevoked||
       !coldController_.nativeScene3COwner||!coldController_.race88||
       coldController_.scene3C!=&scene_||coldController_.mode84!=0||
       !nativeTaIssuance_||
       nativeTaIssuance_->generation!=generation_||!nativeTaIssuance_->issuer||
       coldController_.issuerGeneration!=generation_||!coldController_.started||coldController_.released||
       !actual_.applicationEntities||!actual_.applicationEntities->application10()||
       actual_.applicationEntities->nativeOwnerThread_!=std::this_thread::get_id()||
       actual_.applicationEntities->attachmentLock423F58()||closed_||abandoned_||coldBound_||
       coldPhase_!=4u||!readsBound_||!identitiesCurrent()||!actual_.bank.active(0)||
       !actual_.actualChariotModels||actual_.actualChariotModels->empty()||
       !(*actual_.actualChariotModels)[0])return taReject(0x175280);
    const auto& racer=racerEntities_[0];const auto* body=racer.body340;
    if(!racer.prepared()||!(*actual_.actualChariotModels)[0]->finishedFor(racer)||
       racer.racer.kind!=OriginalWreckComponentKind50LA::racer27E130||
       racer.racer.reference00!=1||racer.racer.sessionGeneration!=generation_||!body||
       body->entity().kind!=OriginalWreckComponentKind50LA::body167920||
       body->entity().reference00!=1||body->entity().sessionGeneration!=generation_||
       body->entity().parent1C!=&racer.racer||body->entity().mask20!=2u||
       body->canonicalNumericAddress()!=&actual_.arena.bodies.main||
       !racer.scene190||body->scene288()!=racer.scene190||
       !racer.scene190->constructed||racer.scene190->released||
       racer.scene190->entity().sessionGeneration!=generation_)return taReject(0x1752a4);
    auto& tv=*nativeTv78_;
    if(tv.body20||tv.field08||tv.field0C)return taReject(0x2d209c);
    nativeTvBody20Attempted_=true;
    tv.body20=body; //2D209C: actual retained340, BEFORE arithmetic admission.
    //175290/294 supply3.f;175298/29C supply70.f.2D2090/94 loads
    //literal3FBBBB84. Preserve the source store20 if this reached native
    //rounding/arithmetic boundary rejects; never replay it on another prepare.
    if(std::fegetround()!=FE_TONEAREST)return taReject(0x2d20a0);
    const std::uint32_t factorBits=0x3fbbbb84u;float factor;
    std::memcpy(&factor,&factorBits,sizeof factor);
    const float product=original_race_camera_detail::mul(factor,70.f);
    if(!std::isfinite(product))return taReject(0x2d20a0);
    tv.field24=0; //2D20A4, BEFORE field08 store.
    std::uint32_t productBits;std::memcpy(&productBits,&product,sizeof productBits);
    tv.field08=productBits; //2D20A8.
    tv.field0C=0x40400000u; //2D20B0 delay store of supplied3.f.
    return true; //Closed binding leaf only; no TV load/update or1752D0.
}
bool revokeNativeRaTvBody20()noexcept {
    if(!onOwnerThread())return false;
    if(!nativeTv78_||!nativeTv78_->body20)return true;
    if(nativeTvBody20Revoked_||!nativeTvBody20Attempted_||
       coldController_.children64[5]!=nativeTv78_.get()||
       nativeTv78_->body20!=racerEntities_[0].body340)return false;
    // Compare actual identity before retirement; do not dereference a body
    // during teardown or require live admission after camera revocation.
    nativeTv78_->body20=nullptr;nativeTvBody20Revoked_=true;
    return true; //Native borrow withdrawal, NOT a source2D209C null write.
}
//Private same-owner adaptation only. The genuine Race30/1A95B0 caller
//remains unjoined; no constructor, child allocation or load return is replayed.
bool beginSameControllerLoad174DF0(const OriginalRaceConstructor50LA* race,
    const OriginalFrontendCameraCold50LA* camera,std::uint32_t mode,std::uint32_t& pc)noexcept {
    auto& c=coldController_;pc=c.nativeLoadPrefixPC;
    if(!onOwnerThread()||c.nativeLoadPrefixEntered||!c.started||c.released||
       c.nativeLoadOperandsPublished||c.nativeLoadOperandsRevoked||!race||!camera||
       c.entity.reference00!=1||!actual_.applicationEntities||
       !actual_.applicationEntities->entityAuthority_.canRetire(c.entity))return false;
    c.nativeLoadPrefixEntered=true;
    //115C44 calls literal identity-return141C20 on THIS counted controller.
    //The nonnull owner branch115C5C then publishes actual scene3C at115C68.
    pc=c.nativeLoadPrefixPC=0x115c44;
    pc=c.nativeLoadPrefixPC=0x115c68;c.nativeScene3COwner=camera;c.scene3C=&scene_;
    pc=c.nativeLoadPrefixPC=0x174e24;c.mode84=mode;
    pc=c.nativeLoadPrefixPC=0x174e28;c.race88=race;c.nativeLoadOperandsPublished=true;
    pc=c.nativeLoadPrefixPC=0x174e48;return true;
}
bool publishSameControllerWorldCount174E58(std::uint32_t actualWorld3E4,std::uint32_t& pc)noexcept {
    auto& c=coldController_;pc=c.nativeLoadPrefixPC;
    if(!onOwnerThread()||!c.nativeLoadPrefixEntered||c.nativeLoadPrefixComplete||
       !c.nativeLoadOperandsPublished||c.nativeLoadOperandsRevoked||
       !startPrelude_.bindControllerA8(c.halfA8Value,c.halfA8Known))return false;
    //Actual LH consumes only low16; sign extension precedes original ANDI31.
    const auto low16=static_cast<std::uint16_t>(actualWorld3E4);
    const auto signed16=std::bit_cast<std::int16_t>(low16);
    pc=c.nativeLoadPrefixPC=0x174e58;
    return startPrelude_.publishSceneCutCount174E58(signed16);
}
bool finishSameControllerProfile174EC0(std::uint32_t subtype,std::uint32_t mode,
    std::int32_t process350,std::uint32_t& pc)noexcept {
    auto& c=coldController_;
    if(!onOwnerThread()||c.nativeLoadPrefixComplete||!c.nativeLoadOperandsPublished||
       c.nativeLoadOperandsRevoked||!startPrelude_.currentControllerA8(c.halfA8Value,c.halfA8Known))return false;
    pc=c.nativeLoadPrefixPC=0x174ec0;
    if(!startPrelude_.finishControllerCutCount174EC0(subtype,mode,process350))return false;
    c.nativeLoadPrefixComplete=true;
    pc=c.nativeLoadPrefixPC=0x174ec8;return true; //BEFORE child64 allocation, not174DF0 return.
}
bool withdrawNativeRaLoadingOperands()noexcept {
    // Public close/abandon reach this BEFORE the old adapter-revoke gate.
    // Reject foreign-thread entry before reading or clearing retained context.
    if(!onOwnerThread())return false;
    auto& c=coldController_;
    if(!c.nativeLoadOperandsPublished)return !c.nativeScene3COwner&&!c.race88;
    if(c.nativeLoadOperandsRevoked)return !c.nativeScene3COwner&&!c.race88;
    if(!c.nativeScene3COwner||!c.race88)return false;
    // Before fallible Camera34 adapter retirement. Reached mode84 survives;
    // native pointer withdrawal is not an original scene3C/Race88 clear.
    startPrelude_.withdrawControllerA8(); //Revoke consumer before owner lifetime ends; keep bits.
    c.nativeScene3COwner=nullptr;c.race88=nullptr;c.nativeLoadOperandsRevoked=true;
    return true;
}
bool retireNativeCinematographer70()noexcept {
    if(!nativeCinematographer70_)return !coldController_.children64[3];
    auto& c=*nativeCinematographer70_;
    if(nativeCinematographer70Published_?coldController_.children64[3]!=&c:
       coldController_.children64[3]!=nullptr)return false;
    //119D98..E08: all six actual constructor-null allocation branches skip.
    //No loader or live dispatcher can mutate this private cold-only owner.
    for(const auto* allocation:c.allocations8C)if(allocation)return false;
    c.vtable00=0x3ccc20u;c.vtable00=0x3cca90u; //119D94,115B2C.
    coldController_.children64[3]=nullptr; //Native borrow withdrawal, not a source70 clear.
    nativeCinematographer70Published_=false;
    nativeCinematographer70_.reset();nativeCinematographer70Retired_=true;return true;
}
bool retireNativeTv78()noexcept {
    if(!nativeTv78_)return !coldController_.children64[5];
    auto& c=*nativeTv78_;
    if(coldController_.children64[5]!=&c||c.allocationD4||c.allocationD8||c.allocationE0||
       c.allocation48||c.field4C||c.body20)return false;
    c.vtable00=0x3d5160u; //2D292C; genuine D4/D8/E0 null branches.
    c.vtable00=0x3d5130u; //2D2034; genuine48-null base branch.
    c.vtable00=0x3cca90u; //115B2C. Uncounted delete, no Entity release.
    coldController_.children64[5]=nullptr; //Native borrow withdrawal only.
    nativeTv78_.reset();nativeTv78Retired_=true;return true;
}
