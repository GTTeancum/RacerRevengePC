//Private separate native1750A0 adaptation, NOT fallthrough from174DF0 or
//proof of the unresolved Race30 caller. Same actual first native racer only.
struct NativeRacerCamera1750A0 {
    bool attempted=false,prefixComplete=false,revoked=false;
    std::uint32_t pc=0x1750a0;
    const OriginalRacerEntityCold50LA* follow158=nullptr;
    const OriginalRacerMainBodyOwner50LA* follow1C=nullptr;
    const OriginalRacerSceneObject50LA* follow20=nullptr;
    const OriginalRacerModelSceneOwner50LA* attached24=nullptr;
    const OriginalRacerMainBodyOwner50LA* attached20=nullptr;
} nativeRacerCamera1750A0_;
bool adoptNativeRacerCamera1750A0(std::uint32_t& pc)noexcept {
    auto& n=nativeRacerCamera1750A0_;auto& c=coldController_;pc=n.pc;
    if(!onOwnerThread()||n.attempted||!nativeRaOwnedPreparationAttempted_||
       !nativeIntro177750_.childrenComplete||nativeIntro177750_.pc!=0x175080||
       !nativeAttached6C_.completed||nativeAttached6C_.retired||
       !nativeDeathFiltersCurrent()||c.nativeLoadOperandsRevoked||
       !c.nativeScene3COwner||!c.race88||!readsBound_||!identitiesCurrent()||
       !actual_.bank.active(0)||follow_.prepared||follow_.active||
       attached_.prepared||attached_.active||c.children64[0]!=&follow_||
       c.children64[2]!=&attached_)return false;
    auto& racer=racerEntities_[0];const auto& id=identity_[0];
    if(!racer.prepared()||racer.racer.kind!=OriginalWreckComponentKind50LA::racer27E130||
       racer.racer.reference00!=1||racer.racer.sessionGeneration!=generation_||
       racer.racer.objectKey!=key(id.racerTag))return false;
    n.attempted=true;
    //179D90→17BEE0: each real source borrow is retained before the next read.
    pc=n.pc=0x179d9c;n.follow158=&racer;
    follow_.sessionGeneration=generation_;follow_.racerKey=racer.racer.objectKey;
    pc=n.pc=0x17bee4;const auto* body=racer.body340;
    if(!body||body->entity().kind!=OriginalWreckComponentKind50LA::body167920||
       body->entity().reference00!=1||body->entity().sessionGeneration!=generation_||
       body->entity().parent1C!=&racer.racer||
       body->canonicalNumericAddress()!=&actual_.arena.bodies.main)return false;
    pc=n.pc=0x17bee8;n.follow1C=body;
    //Numeric keys are the existing Session identity projections of these SAME
    //owners, not additional source handles or caller-supplied replacements.
    follow_.mainBodyKey=key(id.mainBodyTag);
    pc=n.pc=0x17bef0;const auto* container=racer.scene194;
    if(!container||!container->constructed||container->released||
       container->entity().sessionGeneration!=generation_||
       container->sceneIndex!=actual_.bank.read(0,reads_.container194))return false;
    pc=n.pc=0x17bef8;n.follow20=container;follow_.containerKey=key(id.containerTag);
    pc=n.pc=0x179db0;follow_.bodySpeedScale88=std::bit_cast<float>(std::uint32_t{0x42cd5518});
    pc=n.pc=0x179db8;follow_.bodyScale8C=3.f;
    //13DEF0:model24 precedes13DEE0:body20. Do not prevalidate a later
    //model/body admission before the preceding source stores have occurred.
    pc=n.pc=0x1750d8;const auto* model=racer.chariotModel1B4;
    if(!model||!actual_.actualChariotModels||!(*actual_.actualChariotModels)[0]||
       !(*actual_.actualChariotModels)[0]->finishedFor(racer)||
       (*actual_.actualChariotModels)[0]->modelScene()!=model||
       !model->model()||model->released()||
       model->model()->sceneIndex!=actual_.bank.read(0,reads_.model1B4))return false;
    pc=n.pc=0x13def4;n.attached24=model;nativeAttached6C_.model24=model;
    auto& binding=attached_.binding;binding.sessionGeneration=generation_;
    binding.racerKey=racer.racer.objectKey;binding.model1B4Key=key(id.modelTag);
    binding.model1B4=model->model()->sceneIndex;binding.pod60=static_cast<std::int32_t>(id.pod60);
    pc=n.pc=0x1750e4;
    if(racer.body340!=body||!body->scene288()||body->scene288()!=racer.scene190||
       body->scene288()->sceneIndex!=actual_.bank.read(0,reads_.body288))return false;
    pc=n.pc=0x13dee4;n.attached20=body;nativeAttached6C_.mainBody20=body;
    binding.mainBodyKey=key(id.mainBodyTag);binding.body288=body->scene288()->sceneIndex;
    //1750F8 reads actual racer374[0]→collection38[0]→pieceD0. The optional
    //deathDebris view has only models/flags and is NOT this physical owner.
    //No primary340 substitution, empty-list assumption or286D20 store.
    n.prefixComplete=true;pc=n.pc=0x1750f8;return true;
}
bool nativeRacerCameraBindingsCurrent()const noexcept {
    const auto& n=nativeRacerCamera1750A0_;
    if(!n.attempted)return true; //Never-adopted standalone consumers unchanged.
    if(!n.prefixComplete||n.revoked||n.follow158!=&racerEntities_[0]||
       n.follow1C!=racerEntities_[0].body340||n.follow20!=racerEntities_[0].scene194||
       n.attached24!=racerEntities_[0].chariotModel1B4||n.attached20!=n.follow1C||
       nativeAttached6C_.model24!=n.attached24||nativeAttached6C_.mainBody20!=n.attached20)return false;
    return follow_.racerKey==key(identity_[0].racerTag)&&follow_.mainBodyKey==key(identity_[0].mainBodyTag)&&
        follow_.containerKey==key(identity_[0].containerTag)&&attached_.binding.racerKey==follow_.racerKey&&
        attached_.binding.mainBodyKey==follow_.mainBodyKey&&attached_.binding.model1B4Key==key(identity_[0].modelTag);
}
bool revokeNativeRacerCameraBindings()noexcept {
    auto& n=nativeRacerCamera1750A0_;
    if(!n.attempted)return true;
    if(!onOwnerThread())return false;
    if(n.revoked)return !n.follow158&&!n.follow1C&&!n.follow20&&!n.attached24&&!n.attached20;
    if(follow_.prepared||follow_.active||attached_.prepared||attached_.active||
       followChild_.follow64||attachedChild_.attached6C)return false;
    //Identity-only preflight BEFORE any model/body/scene retirement. Reached
    //partial prefixes may contain fewer borrows; never demand later stores.
    if((n.follow158&&n.follow158!=&racerEntities_[0])||
       (n.follow1C&&n.follow1C!=racerEntities_[0].body340)||
       (n.follow20&&n.follow20!=racerEntities_[0].scene194)||
       (n.attached24&&(n.attached24!=racerEntities_[0].chariotModel1B4||nativeAttached6C_.model24!=n.attached24))||
       (n.attached20&&(n.attached20!=racerEntities_[0].body340||nativeAttached6C_.mainBody20!=n.attached20)))return false;
    n.revoked=true;
    n.follow158=nullptr;n.follow1C=nullptr;n.follow20=nullptr;
    if(n.attached24)nativeAttached6C_.model24=nullptr;
    if(n.attached20)nativeAttached6C_.mainBody20=nullptr;
    n.attached24=nullptr;n.attached20=nullptr;
    //Native identity withdrawal, not guest pointer stores or constructor reset.
    follow_.sessionGeneration=0;follow_.racerKey=0;follow_.mainBodyKey=0;follow_.containerKey=0;
    attached_.binding={};return true;
}
