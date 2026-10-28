// Same private model forest, scoped per source zone. No global enrollment,
// Entity recount, new heap budget, persistent View, or arbitrary extra suffix.
struct NativeCollisionForestView {
    static constexpr std::size_t capacity=646u; //1 parent +467 slots +178 queries.
    std::array<C*,capacity> nodes{};
    C* const* objects=nodes.data();
    std::size_t count=0;
    std::uint64_t generation=0;
    mutable std::array<unsigned char,capacity> seen{};
    bool add(C& e)noexcept {
        if(count==capacity)return false;
        for(std::size_t i=0;i<count;++i)if(nodes[i]==&e)return false;
        nodes[count++]=&e;return true;
    }
    friend bool domain(const NativeCollisionForestView& v)noexcept {
        if(!v.generation||!v.count||v.count>v.capacity||v.objects!=v.nodes.data())return false;
        for(std::size_t i=0;i<v.count;++i){const auto* e=v.objects[i];
            if(!e||e->sessionGeneration!=v.generation||e->reference00!=1u||
               e->objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(e))||
               e->kind==K::unbound)return false;
            for(std::size_t j=0;j<i;++j)if(v.objects[j]==e)return false;
        }
        return true;
    }
    friend bool contains(const NativeCollisionForestView& v,const C* e)noexcept {
        for(std::size_t i=0;i<v.count;++i)if(v.objects[i]==e)return true;
        return false;
    }
    friend unsigned char* entityVisitScratch(const NativeCollisionForestView& v)noexcept {
        return v.seen.data();
    }
};
bool nativeCollisionOwnerCurrent27B170()const noexcept {
    if(!nativeCollisionForest_||!source_||!application_||closed_||parents_.size()!=10u||
       nativeCollisionForest_->source_!=source_||!source_->sourcesDecoded())return false;
    const auto& decoded=*source_->sourcesDecoded();
    if(decoded.world.zones.size()!=10u||decoded.world.instances.size()!=1261u||decoded.collisions.size()!=178u)return false;
    for(std::size_t z=0;z<10u;++z){const auto& p=parents_[z];const auto& zone=nativeCollisionForest_->zones_[z];
        if(!p||p->zone!=z+1u||p->entries34.size()>467u||zone.entries34.size()!=p->entries34.size())return false;
        for(std::size_t row=0;row<p->entries34.size();++row){const auto& w=zone.entries34[row];const auto& slot=p->entries34[row];
            if(slot.worldInstance>=decoded.world.instances.size()||
               w.source!=&decoded.world.instances[slot.worldInstance]||w.source->zone!=z+1u||w.source->ordinal!=row+1u||
               w.type00!=1u||(w.descriptor08&&w.descriptor08->source!=w.source)||
               (w.descriptorRetired&&w.descriptor08))return false;
            if(!w.nativeQuery){if(w.collision04)return false;continue;}
            const auto& q=*w.nativeQuery;
            if(q.retired){if(q.counted||w.collision04||q.entity.reference00||q.appended||q.registered)return false;continue;}
            if(!q.counted){if(q.appended||q.registered||w.collision04||q.entity.reference00)return false;continue;}
            if(!nativeCollisionForest_->nativeQueryCurrent27B040(w)||q.application!=application_.get()||
               q.entity.sessionGeneration!=generation_||q.source!=w.source||q.member!=w.source->collision_member)return false;
            if(q.candidate){
                if(q.resourceIndex>=decoded.collisions.size()||w.decodedCollision!=&decoded.collisions[q.resourceIndex]||
                   w.decodedCollision->zone!=z+1u||w.decodedCollision->ordinal!=row+1u||
                   w.decodedCollision->member!=w.source->collision_member||
                   q.candidate->candidate.collision.geometry.mesh!=&w.decodedCollision->mesh)return false;
            }
            if(q.appended&&(!q.loaded||!q.resourceStarted||q.resourceWithdrawn||q.terminal||
               !q.typed||!q.flagsApplied||!q.candidate||w.collision04!=&q.entity))return false;
            if(!q.appended&&(q.entity.parent1C||q.entity.previous10||q.entity.next14))return false;
        }
    }
    return true;
}
bool nativeCollisionSuffixPresent27B170()const noexcept {
    if(!nativeCollisionForest_)return false;
    for(const auto& zone:nativeCollisionForest_->zones_)for(const auto& w:zone.entries34)
        if(w.nativeQuery&&(w.nativeQuery->appended||w.nativeQuery->entity.parent1C||
            w.nativeQuery->entity.previous10||w.nativeQuery->entity.next14))return true;
    return false;
}
bool nativeCollisionSuffixTopology27B170(const Parent& p,const C*& previous,const C*& cursor)const noexcept {
    if(!nativeCollisionForest_)return cursor==nullptr;
    if(!nativeCollisionOwnerCurrent27B170()||!p.zone||p.zone>10u||parents_[p.zone-1u].get()!=&p)return false;
    const auto& zone=nativeCollisionForest_->zones_[p.zone-1u];
    for(const auto& w:zone.entries34){if(!w.nativeQuery||!w.nativeQuery->appended)continue;
        const auto& e=w.nativeQuery->entity;
        //Compare ownership-derived address BEFORE dereferencing any cursor.
        if(cursor!=&e||e.parent1C!=&p.entity||e.previous10!=previous||e.firstChild18||
           e.reference00!=1u||e.mask20||e.descendants24)return false;
        previous=&e;cursor=e.next14;
    }
    return cursor==nullptr;
}
bool nativeCollisionView27B170(std::size_t zone,NativeCollisionForestView& view)const noexcept {
    if(zone>=parents_.size()||!nativeCollisionOwnerCurrent27B170())return false;
    const auto& p=*parents_[zone];
    if(!p.constructed||p.released||p.entity.kind!=K::staticModels27B610||
       p.entity.parent1C||p.entity.previous10||p.entity.next14||p.entity.mask20||p.entity.descendants24||
       !parentTopology(p)||!view.add(const_cast<C&>(p.entity)))return false;
    view.generation=generation_;
    for(const auto& slot:p.entries34){const auto* m=slot.model;
        if(!m||m->referenceReleased)continue;
        if(m->parent!=zone||m->source.worldInstance!=slot.worldInstance||
           !m->parentAttached||!m->scene.model()||m->scene.released()||
           m->scene.model_.entity8.kind!=K::model2403A0||
           !view.add(const_cast<C&>(m->scene.model_.entity8)))return false;
    }
    std::size_t queries=0;
    for(const auto& w:nativeCollisionForest_->zones_[zone].entries34){
        if(!w.nativeQuery||!w.nativeQuery->counted||w.nativeQuery->retired)continue;
        if(++queries>178u||!view.add(w.nativeQuery->entity))return false;
    }
    return domain(view)&&g::original_entity_chain_detail50LA::forest(view);
}
bool nativeCollisionForestCurrent27B170()const noexcept {
    if(!nativeCollisionForest_)return true;
    if(!nativeCollisionOwnerCurrent27B170()||application_->attachmentLock423F58()||
       (visibility_&&!visibility_->raStaticOwnerIdle(this)))return false;
    for(std::size_t z=0;z<10u;++z){NativeCollisionForestView view;
        if(!nativeCollisionView27B170(z,view))return false;
    }
    return true;
}
bool bindNativeCollisionForest27B170(g::OriginalWorldCollisionObjects50LA& owner,std::uint32_t& pc)noexcept {
    pc=0x27b170;
    if(nativeCollisionForest_)return nativeCollisionForest_==&owner&&nativeCollisionForestCurrent27B170();
    if(nativeCollisionForestRetiring_||!constructed_||!resourcesComplete_||!observersComplete_||
       models_.size()!=1085u||parents_.size()!=10u||!canClose()||owner.source_!=source_)return false;
    nativeCollisionForest_=&owner;
    if(!nativeCollisionForestCurrent27B170()){nativeCollisionForest_=nullptr;return false;}
    return true;
}
bool appendNativeCollision27B170(std::size_t zone,std::size_t row,std::uint32_t& pc)noexcept {
    pc=0x27b170;
    if(nativeCollisionForestRetiring_||!nativeCollisionForestCurrent27B170()||!nativeCollisionForest_||
       zone>=10u||row>=nativeCollisionForest_->zones_[zone].entries34.size())return false;
    auto& w=nativeCollisionForest_->zones_[zone].entries34[row];
    if(!w.nativeQuery)return false;auto& q=*w.nativeQuery;
    if(q.appended){pc=0x141a58;return true;}
    if(q.retired||q.terminal||!q.loaded||!q.resourceStarted||q.resourceWithdrawn||q.registered||
       !q.typed||!q.flagsApplied||!q.candidate||w.collision04!=&q.entity)return false;
    //Source-order successful suffix, not sorted candidate order. Earlier rows
    //must have genuinely completed their positive or negative wrapper path.
    for(std::size_t z=0;z<=zone;++z){const auto& entries=nativeCollisionForest_->zones_[z].entries34;
        const auto end=z==zone?row:entries.size();
        for(std::size_t i=0;i<end;++i){const auto& prior=entries[i];
            if(!prior.nativeQuery||!prior.nativeAttempted||!prior.descriptorRetired||prior.descriptor08)return false;
            const auto& p=*prior.nativeQuery;
            if(prior.decodedCollision){if(!p.loaded||!p.appended||!p.registered||p.retired)return false;}
            else if(!p.negativeLoadReturned||!p.retired||p.counted||prior.collision04)return false;
        }
    }
    NativeCollisionForestView view;if(!nativeCollisionView27B170(zone,view))return false;
    const auto result=g::original_entity_chain_detail50LA::attachOriginalWreckChildChain14196050LA(
        view,parents_[zone]->entity,q.entity,0u,application_->attachmentLock423F58());
    pc=result.sourcePC;if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
    q.appended=true;return true; //Publish ONLY after actual source splice.
}
bool detachNativeCollisionSuffix27B170(std::uint32_t& pc)noexcept {
    pc=0x141a70;
    if(!nativeCollisionForest_)return true;
    if(!nativeCollisionForestCurrent27B170())return false;
    //Caller first withdraws real runtime registration/observer borrowers.
    //This helper has no authority to clear those flags or replay their service.
    for(const auto& zone:nativeCollisionForest_->zones_)for(const auto& w:zone.entries34)
        if(w.nativeQuery&&w.nativeQuery->registered)return false;
    nativeCollisionForestRetiring_=true;
    for(std::size_t z=10u;z;){--z;auto& entries=nativeCollisionForest_->zones_[z].entries34;
        for(std::size_t row=entries.size();row;){auto& w=entries[--row];
            if(!w.nativeQuery||!w.nativeQuery->appended)continue;
            NativeCollisionForestView view;if(!nativeCollisionView27B170(z,view))return false;
            auto& q=*w.nativeQuery;
            const auto result=g::original_entity_chain_detail50LA::detachOriginalWreckComponent141A7050LA(
                view,q.entity,application_->attachmentLock423F58());
            pc=result.sourcePC;if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
            q.appended=false; //Actual detached owner survives subsequent resource failure.
        }
    }
    return !nativeCollisionSuffixPresent27B170();
}
bool releaseNativeCollisionForest27B170(std::uint32_t& pc)noexcept {
    pc=0x141b88;
    if(!nativeCollisionForest_)return true;
    if(!nativeCollisionForestCurrent27B170()||nativeCollisionSuffixPresent27B170())return false;
    for(const auto& zone:nativeCollisionForest_->zones_)for(const auto& w:zone.entries34)
        if(w.nativeQuery&&w.nativeQuery->registered)return false;
    nativeCollisionForest_=nullptr;nativeCollisionForestRetiring_=true;return true;
}

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
    return nativeCollisionSuffixTopology27B170(p,previous,cursor);
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
        const Parent* enteredParent=nullptr;
        for(std::size_t i=0;i<models_.size();++i){auto& m=*models_[i];d.placement=i;
            auto& scene=m.scene;auto& root=scene.model_;auto& p=*parents_[m.parent];
            // Retained Application identities, never a caller count or ready bit.
            if(!m.placed||!scene.constructed2403A0()||m.observer||root.observer19C||
               scene.binding_.actualObjectCount423F60!=&application_->objectCount423F60()||
               scene.binding_.actualAttachmentLock423F58!=&application_->attachmentLock423F58()||
               scene.binding_.actualTypes383408!=&application_->types383408()||
               root.entity8.sessionGeneration!=generation_||!parentTopology(p))
                return finish(reject(d,"actual canonical static model/Application changed"));
            if(enteredParent!=&p){
                //2741C0 is before this zone's27B930 ->27ABC0 ->28E8A0.
                //Publish once in the existing authored model order. A cold
                //parent with no represented callback remains unproduced.
                if(!visibility_->publishRaStaticZone2741C0(this,p.zone))
                    return finish(reject(d,"actual zone44 publication predecessor rejected"));
                enteredParent=&p;
            }
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
            // The attach transaction mutates membership and reads the storage
            // descriptor. Keep that descriptor outside the mutated record;
            // its backing arrays remain the same stable observer allocations.
            const auto registrationStorage=o.membership.storage;
            const auto registered=g::attachLiveZoneMembership50EN(world->lifecycle,world->portals,
                o.membership,registrationStorage,o.object,p.zone,g::ZoneMethod50CL::First,0.f,bounds,*work);
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
