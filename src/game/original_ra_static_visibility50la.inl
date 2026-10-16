// Private implementation beside the one lighting/visibility Storage definition.
namespace rr::game::original_gc_gameplay_lighting_candidate {
bool Owner::bindRaStaticOwner(const ::ra_static_model_cold50LA::Owner* owner,
    OriginalApplicationEntities50LA* app,const assets::OriginalStandardMaterialTable50LA* materials,
    const assets::World48F& world) noexcept {
    if(!raStaticCurrent()||!owner||!app||!materials)return false;
    auto& s=*storage_;
    if(s.staticOwner||s.staticAccess||s.cameraGeneration||s.drawGeneration||s.staticModels.size()!=world.instances.size()||
       s.world.zones.size()!=world.zones.size()||s.visibility.lifecycle.objectCount!=s.identityCount())return false;
    for(std::size_t i=0;i<world.instances.size();++i){
        const auto& a=world.instances[i];const auto& b=s.world.instances[i];
        if(a.zone!=b.zone||a.ordinal!=b.ordinal||a.parameter_block!=b.parameter_block||
           a.source_block_offset!=b.source_block_offset||a.geometry_member!=b.geometry_member||
           a.position!=b.position||a.look!=b.look||a.up!=b.up)return false;
    }
    // Native construction join for230450's process-shared quad. The retained
    // actual portal geometry already owns the authored four vertices; repeat
    // the source's literal quad copy/reorder once for each original portal.
    // No camera/host bounds are accepted. This is not visibility admission.
    for(std::size_t i=0;i<s.visibility.portals.portalCount;++i){
        if(!constructPortalViewport230450_50EO(app->portalViewport434940_))return false;
        app->portalViewportConstructed_=true;
    }
    // Concrete method facets for these SAME registry/method identities. The
    // native late construction join keeps actual shared registry IDs, not
    // retail integer constants or a copied selection registry.
    try {
        for(std::size_t i=0;i<s.world.zones.size()*2u;++i){auto& m=s.selectedMethods[i];
            m.selected18.resize(30,nullptr);m.capacity0C=30; //27ED40(30,30)
            const auto a=app->types383408_.findOrAppend2A31E0("SoultreeObject");
            if(!a.okay())return false;m.type2C=a.type;
            const auto b=app->types383408_.findOrAppend2A31E0("SearchableGameObject");
            if(!b.okay())return false;m.type2D=b.type;m.constructed=true;
        }
    }catch(...){return false;}
    s.staticOwner=owner;s.staticApplication=app;s.staticMaterials=materials;
    return true;
}
bool Owner::raStaticOwnerIdle(const ::ra_static_model_cold50LA::Owner* owner)const noexcept {
    return storage_&&owner&&storage_->staticOwner==owner&&!storage_->staticAccess&&
        storage_->staticApplication&&storage_->visibility.lifecycle.objectCount==storage_->identityCount()&&
        storage_->visibility.lifecycle.storage.registrations==storage_->registrations.data()&&
        !storage_->activeLease&&!storage_->activeCamera&&!storage_->activeReset&&!storage_->resetPending();
}
bool Owner::beginRaStaticAccess(const ::ra_static_model_cold50LA::Owner* owner,bool retiring)noexcept {
    if(!raStaticOwnerIdle(owner)||(!retiring&&invalidated_))return false;
    storage_->staticAccess=true;return true;
}
void Owner::endRaStaticAccess(const ::ra_static_model_cold50LA::Owner* owner)noexcept {
    if(storage_&&owner&&storage_->staticOwner==owner)storage_->staticAccess=false;
}
ResetVisibilityWorld50EY* Owner::raStaticVisibility(const ::ra_static_model_cold50LA::Owner* owner)noexcept {
    return storage_&&owner&&storage_->staticOwner==owner&&storage_->staticAccess?&storage_->visibility:nullptr;
}
LiveZoneWorkspace50EN* Owner::raStaticWorkspace(const ::ra_static_model_cold50LA::Owner* owner)noexcept {
    return raStaticVisibility(owner)?&storage_->workspace:nullptr;
}
CollectionRef50BX Owner::claimRaStaticModel(const ::ra_static_model_cold50LA::Owner* owner,
    std::size_t instance,const OriginalRacerModelSceneOwner50LA* model)noexcept {
    if(!raStaticVisibility(owner)||!model||instance>=storage_->staticModels.size())return 0;
    auto& s=*storage_;
    for(const auto* existing:s.staticModels)if(existing==model)return 0;
    if(s.staticModels[instance])return 0;
    const auto index=s.racerCount+1u+instance;const auto& r=s.registrations[index];
    if(r.owner||r.slot||r.encodedObject)return 0;
    s.staticModels[instance]=model;
    return static_cast<CollectionRef50BX>(index+1u);
}
bool Owner::releaseRaStaticModel(const ::ra_static_model_cold50LA::Owner* owner,
    std::size_t instance,const OriginalRacerModelSceneOwner50LA* model)noexcept {
    if(!raStaticVisibility(owner)||instance>=storage_->staticModels.size()||
       storage_->staticModels[instance]!=model)return false;
    const auto& r=storage_->registrations[storage_->racerCount+1u+instance];
    if(r.owner||r.slot||r.encodedObject)return false;
    for(auto& method:storage_->selectedMethods){
        method.complete=false;
        for(auto& entity:method.selected18)if(entity==model->modelEntity8())entity=nullptr;
    }
    storage_->staticSelection[instance]={};storage_->staticModels[instance]=nullptr;return true;
}
bool Owner::releaseRaStaticOwner(const ::ra_static_model_cold50LA::Owner* owner)noexcept {
    if(!raStaticVisibility(owner))return false;
    for(const auto* model:storage_->staticModels)if(model)return false;
    storage_->staticAccess=false;storage_->staticRegistrationComplete=false;
    storage_->staticOwner=nullptr;storage_->staticApplication=nullptr;storage_->staticMaterials=nullptr;
    for(auto& method:storage_->selectedMethods)method={};return true;
}
bool Owner::commitRaStaticOwner(const ::ra_static_model_cold50LA::Owner* owner)noexcept {
    if(!raStaticVisibility(owner))return false;
    auto& s=*storage_;bool found=false;
    for(std::size_t i=0;i<s.staticModels.size();++i)if(s.staticModels[i]){
        const auto zone=s.world.instances[i].zone;
        const auto& r=s.registrations[s.racerCount+1u+i];
        if(s.staticSelection[i].model!=s.staticModels[i]||!s.staticSelection[i].membership||
           !zone||zone>s.world.zones.size()||r.owner!=&s.registries[(zone-1u)*2u]||
           !r.slot||r.encodedObject!=((s.racerCount+2u+i)<<2u))return false;
        found=true;
    }
    if(!found)return false;s.staticRegistrationComplete=true;return true;
}
}
