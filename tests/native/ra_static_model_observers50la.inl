// Included inside the private static model Owner. All inputs originate from
// retained source placements and the actual Application-bound canonical model.
bool parentTopology(const Parent& p)const noexcept {
    const C* previous=nullptr;const C* cursor=p.entity.firstChild18;
    for(const auto& slot:p.entries34){
        const auto* m=slot.model;if(!m||!m->parentAttached||m->referenceReleased)continue;
        const auto& e=m->scene.model_.entity8;
        if(cursor!=&e||e.parent1C!=&p.entity||e.previous10!=previous||e.firstChild18||
           e.reference00!=1||e.mask20||e.descendants24)return false;
        previous=&e;cursor=e.next14;
    }
    return cursor==nullptr;
}
bool staticBounds(Model& m,g::LiveZoneBounds50EN& out)noexcept {
    auto& root=m.scene.model_;auto graph=m.graph();
    if(m.scene.sourceRowCount()!=1||m.scene.sourceRow(0)!=&root||
       !m.scene.fields().sourceRows1B8||m.scene.fields().sourceRows1B8[0]!=&root||
       (m.node.sceneFlags190&0x10u))return false;
    //28E260..278 invokes actual model slot90=26F4B0 on this exact gate.
    const bool recalculate=(m.node.sceneFlags190&0x80u)&&
        ((m.node.sceneFlags190&8u)||!(m.node.sceneFlags190&0x40u));
    if(!g::refreshSceneWorld49P(graph,static_cast<I>(0)))return false;
    if(recalculate){
        g::ModelBoundsNode49O row{};row.local=m.node.localC0;row.parent=-1;
        row.sceneFlags190=m.node.sceneFlags190;
        g::ModelBoundsContext49O context{};context.rootWorld100=m.node.world100;
        context.rootSceneFlags190=m.node.sceneFlags190;
        for(unsigned i=0;i<3;++i){
            row.center140.v[i]=context.rootCenter140.v[i]=root.bounds140[i];
            row.halfExtent150.v[i]=context.rootHalfExtent150.v[i]=root.bounds150[i];
        }
        g::ModelBoundsWorkspace49O scratch{};g::ModelBoundsResult49O result{};
        if(!g::computeDirectRootModelBounds50KG(&row,1,context,scratch,result))return false;
        std::memcpy(root.bounds160.data(),result.center160,sizeof result.center160);
        std::memcpy(root.bounds170.data(),result.halfExtent170,sizeof result.halfExtent170);
        root.radius198=result.radius198;m.node.sceneFlags190=result.rootSceneFlags190;
    }
    if(!root.radius198)return false;
    //28E230 uses the common affine box arithmetic. Tag1 selects that numeric
    //leaf only; no collision object is constructed or registered here.
    out.bounds.shapeTag=1;out.bounds.shapeWorld=m.node.world100;
    std::memcpy(out.bounds.localCenter.v,root.bounds160.data(),sizeof out.bounds.localCenter.v);
    std::memcpy(out.bounds.localHalf.v,root.bounds170.data(),sizeof out.bounds.localHalf.v);
    out.bounds.cachedRadius=*root.radius198;
    g::CollectionBoundsResult48S actual{};
    if(g::prepareCollectionBounds48S(out.bounds,actual)!=g::CollectionBoundsStatus48S::Complete)return false;
    out.bounds.cachedCenter=actual.center;return true;
}
bool registerObserversAndParents(std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner> visibility,
    Diagnostic& d)noexcept {
    d={};if(observersAttempted_||!resourcesComplete_||closed_||!visibility||!canClose())
        return reject(d,"retained static resource prefix and idle visibility owner required");
    observersAttempted_=true;
    if(!visibility->bindRaStaticOwner(this,application_.get(),materials_.get(),source_->sourcesDecoded()->world))
        return reject(d,"actual static world/visibility identity binding rejected");
    visibility_=std::move(visibility);
    if(!visibility_->beginRaStaticAccess(this,false))return reject(d,"static visibility access unavailable");
    const auto finish=[&](bool value){visibility_->endRaStaticAccess(this);return value;};
    try {
        auto* world=visibility_->raStaticVisibility(this);auto* work=visibility_->raStaticWorkspace(this);
        if(!world||!work)return finish(reject(d,"canonical visibility borrow missing"));
        for(std::size_t i=0;i<models_.size();++i){auto& m=*models_[i];d.placement=i;
            auto& scene=m.scene;auto& root=scene.model_;auto& p=*parents_[m.parent];
            // Retained Application identities, never a caller count or ready bit.
            if(!m.placed||!scene.constructed2403A0()||m.observer||root.observer19C||
               scene.binding_.actualObjectCount423F60!=&application_->objectCount423F60()||
               scene.binding_.actualAttachmentLock423F58!=&application_->attachmentLock423F58()||
               scene.binding_.actualTypes383408!=&application_->types383408()||
               root.entity8.sessionGeneration!=generation_||!parentTopology(p))
                return finish(reject(d,"actual canonical static model/Application changed"));
            auto observer=std::make_unique<Observer>();
            observer->links.resize(world->portals.portalCount+1u);
            observer->entries.resize(observer->links.size());
            observer->membership.storage={observer->links.data(),observer->entries.data(),observer->links.size()};
            observer->object=visibility_->claimRaStaticModel(this,m.source.worldInstance,&scene);
            if(!observer->object)return finish(reject(d,"authored static identity already claimed"));
            m.observer=std::move(observer);root.observer19C=m.observer.get(); //28E8E8.
            g::LiveZoneBounds50EN bounds{};
            if(!staticBounds(m,bounds))return finish(reject(d,"actual28E230/26F4B0 bounds rejected"));
            auto& o=*m.observer;
            const auto registered=g::attachLiveZoneMembership50EN(world->lifecycle,world->portals,
                o.membership,o.membership.storage,o.object,p.zone,g::ZoneMethod50CL::First,0.f,bounds,*work);
            d.visibility=registered;
            if(registered.status!=g::LiveZoneStatus50EN::Complete)
                return finish(reject(d,"actual static2809C0 registration rejected"));
            if(!visibility_->bindRaStaticSelection(this,m.source.worldInstance,&scene,m.graph(),
                    m.observer.get(),&o.membership))
                return finish(reject(d,"actual selection model/observer binding rejected"));
            //27AE38 ->141960 with mask0, only AFTER actual observer success.
            // All children are genuine private model Entities. Walk the retained
            // authored slots rather than widening the unrelated wreck64 domain.
            auto& child=root.entity8;C* tail=nullptr;
            for(auto* e=p.entity.firstChild18;e;e=e->next14)tail=e;
            if(child.parent1C||child.previous10||child.next14||child.firstChild18||
               child.mask20||child.descendants24||application_->attachmentLock423F58())
                return finish(reject(d,"actual static Entity append preflight rejected"));
            if(tail)tail->next14=&child;else p.entity.firstChild18=&child;
            child.previous10=tail;child.parent1C=&p.entity;child.mask20=0;
            // Parent has no outside parent and all descendant masks are zero;
            // the source141A00 propagation consequently preserves zero24.
            p.entity.descendants24|=child.descendants24|child.mask20;
            m.parentAttached=true;++d.models;
            // Descriptor remains: smoker/later tail is not implemented here.
        }
        if(!visibility_->commitRaStaticOwner(this))return finish(reject(d,"actual static registrations incomplete"));
        observersComplete_=true;return finish(true);
    }catch(...){return finish(reject(d,"observer allocation failed; actual prefix retained"));}
}
bool retireObserver(Model& m)noexcept {
    if(!m.observer)return m.scene.model_.observer19C==nullptr;
    if(!visibility_||m.scene.model_.observer19C!=m.observer.get())return false;
    auto* world=visibility_->raStaticVisibility(this);auto* work=visibility_->raStaticWorkspace(this);
    if(!world||!work)return false;
    auto& o=*m.observer;
    if(o.membership.attached){
        const auto r=g::removeLiveZoneMembership50EN(world->lifecycle,world->portals,o.membership,*work);
        if(r.status!=g::LiveZoneStatus50EN::Complete)return false;
    }
    if(!visibility_->releaseRaStaticModel(this,m.source.worldInstance,&m.scene))return false;
    m.scene.model_.observer19C=nullptr;m.observer.reset();return true;
}
