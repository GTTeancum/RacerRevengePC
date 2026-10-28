// Same counted native identities; no constructor replay or manager registration.
namespace ra_pod_scene50LA {
bool Owner::nativeEmitterManager274370Current(g::OriginalGcParticleManager50KG*& manager,
    Entity*& entity,std::uint32_t& pc)const noexcept {
    manager=nullptr;entity=nullptr;pc=0x274370;
    if(!sceneApplication_||sceneApplication_->attachmentLock423F58()||
       !nativeParticleManagerComplete_||!trackParticles_||nativeParticleManager4A0_!=trackParticles_.get()||
       !trackParticles_->matchesNativeResources160E00(sceneApplication_.get(),
           trackParticles_->table_.get(),sceneGeneration_,trackParticles_->particles()))return false;
    manager=trackParticles_->particles();entity=trackParticles_->component();
    return manager&&entity&&entity->reference00==1&&entity->sessionGeneration==sceneGeneration_&&
        entity->objectKey==key(entity)&&entity->kind==g::OriginalWreckComponentKind50LA::particleManager160B10;
}
bool Owner::constructNativeParticleList27DC9C(Entity& entity,std::uint32_t& pc)noexcept {
    pc=0x27dc9c;
    if(!sceneApplication_||sceneApplication_->attachmentLock423F58())return false;
    auto& authority=sceneApplication_->entityAuthority_;auto& count=sceneApplication_->objectCount423F60();
    if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
       !authority.canEnroll(entity,key(&entity)))return false;
    if(g::initializeOriginalWreckComponent50LA(entity,g::OriginalWreckComponentKind50LA::entity141300,
       sceneGeneration_,key(&entity),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
       !authority.enroll(entity))std::terminate();
    return true;
}
bool Owner::enrollNativeParticleEntity274370(Entity& entity,std::uint32_t& pc)noexcept {
    pc=0x27ca6c;
    if(!sceneApplication_||sceneApplication_->attachmentLock423F58()||
       entity.reference00!=1||entity.sessionGeneration!=sceneGeneration_||entity.objectKey!=key(&entity)||
       entity.kind!=g::OriginalWreckComponentKind50LA::effect15DE00||entity.parent1C||
       entity.previous10||entity.next14||entity.firstChild18||entity.mask20||entity.descendants24)return false;
    auto& authority=sceneApplication_->entityAuthority_;
    // Native authority enrollment of the already-counted identity. NOT15DE00.
    return authority.reserveAdditional(1)&&authority.canEnroll(entity,key(&entity))&&authority.enroll(entity);
}
bool Owner::attachNativeParticleEntity27CE6C(Entity& list,Entity& emitter,std::uint32_t& pc)noexcept {
    pc=0x27ce6c;
    if(!canRetireNativeParticleEntity274370(list,false)||!canRetireNativeParticleEntity274370(emitter,true)||
       list.parent1C||list.previous10||list.next14||list.mask20||emitter.parent1C||emitter.previous10||
       emitter.next14||emitter.firstChild18||emitter.mask20||emitter.descendants24)return false;
    const auto r=sceneApplication_->entityAuthority_.attachChildren(list,emitter,0x402u,
        sceneApplication_->attachmentLock423F58());
    pc=r.sourcePC;return r.status==g::OriginalWreckComponentStatus50LA::complete;
}
bool Owner::canRetireNativeParticleEntity274370(const Entity& entity,bool emitter)const noexcept {
    return sceneApplication_&&!sceneApplication_->attachmentLock423F58()&&
        entity.reference00==1&&entity.sessionGeneration==sceneGeneration_&&entity.objectKey==key(&entity)&&
        entity.kind==(emitter?g::OriginalWreckComponentKind50LA::effect15DE00:
            g::OriginalWreckComponentKind50LA::entity141300)&&
        sceneApplication_->objectCount423F60()&&sceneApplication_->entityAuthority_.canRetire(entity);
}
bool Owner::releaseNativeParticleEntity142590(Entity& entity,bool emitter,Entity* parent,Entity* previous)noexcept {
    if(!canRetireNativeParticleEntity274370(entity,emitter)||entity.firstChild18||entity.next14||
       entity.parent1C!=parent||entity.previous10!=previous||
       (parent&&(!emitter||parent->kind!=g::OriginalWreckComponentKind50LA::entity141300))||
       (previous&&(previous->parent1C!=parent||previous->next14!=&entity))||
       (parent&&!previous&&parent->firstChild18!=&entity))return false;
    // Exact last-sibling/childless1424E0->142590 link stores. No141A70
    // mask recomputation: the dying List's descendants24 survives until its
    // own final release. No intervening forest traversal is enabled.
    if(previous)previous->next14=nullptr;
    if(parent&&!previous)parent->firstChild18=nullptr;
    if(!sceneApplication_->entityAuthority_.retire(entity))std::terminate();
    --entity.reference00;--sceneApplication_->objectCount423F60();
    // Native dead-object metadata withdrawal, not additional retail stores.
    entity.parent1C=entity.previous10=entity.next14=nullptr;
    return true;
}
}
