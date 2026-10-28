//Private native per-roster views; included inside OwnershipBridge. These are
//lexical active-slot borrows, never new guest entities or saved arena pointers.
    class SharedRoleViews {
        friend class OwnershipBridge;
        OwnershipBridge* issuer_=nullptr;
        native_racer50HN::Bank* bank_=nullptr;
        std::size_t slot_=0;
        std::uint64_t serial_=0;
        std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles> rows_{};
        g::UndertrackQueryOwners50EW undertrack_{};
        std::optional<g::PodLiveQueryContext50IZ> pod_;
        bool wrote_=false,completed_=false;
    public:
        SharedRoleViews()=default;
        SharedRoleViews(const SharedRoleViews&)=delete;
        SharedRoleViews& operator=(const SharedRoleViews&)=delete;
        ~SharedRoleViews(){if(issuer_)issuer_->endSharedRoleViews(*this);}
    };
    SharedRoleViews* sharedRoleViews_=nullptr;
    std::uint64_t sharedRoleSerial_=0;
    bool beginSharedRoleViews(SharedRoleViews& view,native_racer50HN::Bank& bank,
        std::size_t slot)noexcept {
        const auto pin=bankLifetime_.lock();auto* s=storage_.get();
        if(view.issuer_||sharedRoleViews_||sharedOwnerLease_||sharedEffectsStopped_||terminal_||closed_||
           nativeFifoPins_||triggerQueryActive_||(endCandidateProducer_&&endCandidateProducer_->inCall)||
           !pin||pin->identity!=&bank||bankIdentity_!=&bank||bank.raWorldLifetime_.get()!=pin.get()||
           !bank.sealed_||bank.retired_||bank.visitDepth_!=1u||bank.currentSlot_!=slot||
           (!bank.visiting_&&!bank.readBlocked_)||bank.sourceQueryPending()||
           !s||!s->sharedRoster_||s->sharedRoster_->retired||s->queue.count||
           slot>=s->sharedRoster_->count||bank.count_!=s->sharedRoster_->count||
           !actual_||!actual_->recovery||!actual_->movement||!gridArena_||!freshRecovery_||!worldOwnersRegistered_||
           !gridInputsRegistered(*gridArena_)||!rosterSlotComplete_[slot]||
           sharedRoleSerial_==std::numeric_limits<std::uint64_t>::max())return false;
        auto& q=*actual_;auto& r=*q.recovery;auto& a=*freshRecovery_;
        const auto& ids=bodyIdentities_[slot];const auto& staged=stagedPods694_[slot];
        if(!ids.collision||!ids.collision->prepared()||!staged.bodyA0||staged.collision!=ids.collision||
           !staged.generation||staged.bodyA0->canonicalNumericAddress()!=&q.frame.bodies.main||
           !staged.bodyA0->entity().reference00||staged.bodyA0->entity().sessionGeneration!=staged.generation||
           q.main.resource!=&ids.collision->main()||q.secondary.resource!=&ids.collision->secondary()||
           !r.sourcePrepared||r.selectedSource!=ids.collision||
           a.source.endpoints!=a.endpoints||a.source.endpointCount!=2||
           a.source.sceneMetadata!=a.metadata||a.source.sceneMetadataCount!=2||
           !registeredRange(a.endpoints,sizeof(a.endpoints))||!registeredRange(a.metadata,sizeof(a.metadata))||
           !registeredRange(&r.movementTag,sizeof(r.movementTag))||
           !registeredRange(&r.movementSphere,sizeof(r.movementSphere)))return false;
        g::CollisionFilter48J* filters[]={&q.mainFilter,&q.secondaryFilter,&q.terrain.metadata.filter,
            &r.podFilter,&q.movement->filter};
        for(std::size_t i=0;i<view.rows_.size();++i){
            const auto& owned=s->sharedRoster_->objects[slot*view.rows_.size()+i];
            if(!owned||!owned->coldBound||owned->slot!=slot||static_cast<std::size_t>(owned->role)!=i||
               owned->collision!=ids.collision||owned->generation!=staged.generation||
               owned->canonicalFilter!=filters[i]||!owned->coldFilter||
               !owned->coldFilter->matches(*filters[i])||owned->identity!=filters[i]->identity||
               !owned->reservedRef||owned->reservedRef>s->count.objects)return false;
            view.rows_[i]=owned.get();
        }
        auto& main=*view.rows_[0];auto& secondary=*view.rows_[1];auto& terrain=*view.rows_[2];
        auto& pod=*view.rows_[3];auto& movement=*view.rows_[4];
        const auto& authored=ids.collision->mainTarget().source;const auto& shape=gridArena_->parameters.shape;
        if(main.type2Resource!=q.main.resource||secondary.type2Resource!=q.secondary.resource||
           !terrain.terrainResource||!terrain.terrainResource->initialized||
           terrain.type2Resource!=&terrain.terrainResource->resource||
           !live_terrain_probe50fr::matches(*terrain.terrainResource,q.terrain.initial)||
           pod.bodyA0!=staged.bodyA0||pod.type0Resource!=&authored||
           std::memcmp(shape.local40.v,authored.local40.v,sizeof(shape.local40.v))||
           std::memcmp(shape.rootCenter.v,authored.rootCenter.v,sizeof(shape.rootCenter.v))||
           std::memcmp(shape.rootHalf.v,authored.rootHalf.v,sizeof(shape.rootHalf.v)))return false;
        undertrack_owners50ew_fixture::BorrowedQueries borrowed{};
        borrowed.main690=&q.main;borrowed.mainMembership=&main.membership.state;
        borrowed.mainStorage=main.membership.storage();borrowed.mainRef=main.reservedRef;
        borrowed.movement7A4=q.movement;borrowed.movementEndpoints=a.endpoints;
        borrowed.movementMetadata=a.metadata;borrowed.movementSource=&a.source;
        borrowed.terrain348=&q.terrain.initial;borrowed.terrainResource=terrain.type2Resource;
        borrowed.terrainMetadata=&q.terrain.metadata;borrowed.terrainMembership=&terrain.membership.state;
        borrowed.terrainStorage=terrain.membership.storage();borrowed.terrainRef=terrain.reservedRef;
        view.undertrack_=r.bindingSharedMembers(borrowed,{pod.reservedRef,movement.reservedRef},
            {&pod.membership.state,&movement.membership.state},
            {pod.membership.storage(),movement.membership.storage()});
        for(const auto& query:view.undertrack_.query)if(!query.membership||!query.object)return false;
        //The real source contract remains canonical parameters.shape. Type0
        //bounds consume it synchronously; shared membership retains no type0
        //resource pointer. Cross-racer shape tests use pod.type0Resource instead.
        view.pod_.emplace(g::PodLiveQueryContext50IZ{{s->lifecycle,s->world,pod.membership.state,
            s->membershipWork,pod.reservedRef},&gridArena_->parameters.shape});
        view.issuer_=this;view.bank_=&bank;view.slot_=slot;view.serial_=++sharedRoleSerial_;
        sharedRoleViews_=&view;return true;
    }
    void endSharedRoleViews(SharedRoleViews& view)noexcept {
        if(sharedRoleViews_!=&view||view.issuer_!=this||view.serial_!=sharedRoleSerial_)std::terminate();
        if(view.wrote_&&!view.completed_)sharedEffectsStopped_=true;
        view.pod_.reset();view.undertrack_={};view.rows_={};
        sharedRoleViews_=nullptr;view.issuer_=nullptr;view.bank_=nullptr;
    }
    static bool sameSharedFilterView(const g::CollectionFilter50BX& actual,
        const storage::Storage::SharedColdFilter& owner)noexcept {
        const auto& expected=owner.view;
        if(actual.categoryA4!=expected.categoryA4||actual.acceptMaskB8!=expected.acceptMaskB8||
           actual.flags31!=expected.flags31)return false;
        const g::CollectionWords50BX* a[]={&actual.identities[0],&actual.identities[1],&actual.categories};
        const g::CollectionWords50BX* b[]={&expected.identities[0],&expected.identities[1],&expected.categories};
        for(unsigned i=0;i<3;++i)if(a[i]->values!=b[i]->values||a[i]->capacity!=b[i]->capacity||
            a[i]->count!=b[i]->count||a[i]->mode!=b[i]->mode)return false;
        return true;
    }
    //UNCALLED exact registered-POD update leaf. No phase assignment, model
    //update, cold setup, visibility callback or source-null substitution.
    g::PodLiveQueryResult50IZ updateSharedRegisteredPod(native_racer50HN::Bank& bank,
        std::size_t slot)noexcept {
        g::PodLiveQueryResult50IZ result{};SharedRoleViews view;
        if(!beginSharedRoleViews(view,bank,slot))return result;
        auto& row=*view.rows_[3];auto& s=*storage_.get();auto& frame=actual_->frame;
        const auto main=static_cast<unsigned>(gridArena_->bindings.lifecycle.main190);
        if(!g::validVehicleSceneCount49W(frame,gridArena_->bindings)||main>=frame.sceneNodeCount||
           !(frame.nodes[main].sceneFlags190&1u)||
           std::memcmp(&frame.nodes[main].world100,&frame.bodies.main.components.root.scene,sizeof(g::BodyMatrix48G))||
           !registeredRange(&frame.nodes[main],sizeof(frame.nodes[main]))||
           s.objects[row.reservedRef-1u].identity!=row.identity||
           !s.objects[row.reservedRef-1u].hasShape||s.objects[row.reservedRef-1u].shapeTag!=0u||
           s.objects[row.reservedRef-1u].group||s.objects[row.reservedRef-1u].typeTag!=actual_->main.collisionTypeTag||
           !sameSharedFilterView(s.objects[row.reservedRef-1u].filter,*row.coldFilter)||
           row.effectRevision==std::numeric_limits<std::uint64_t>::max())return result;
        result=g::updateRegisteredPodShape50IZ(frame.shape,frame.nodes[main].world100,*view.pod_);
        if(result.preparationCommitted){
            //Prepare-before-registry effects remain real even when120230 fails.
            //Mirror only the actual prepared cache; preserve epoch/F8 and all
            //other owners. Canonical frame writes are saved by the ACTIVE visit.
            auto& state=s.source[row.reservedRef-1u];state.cachedCenter=frame.shape.center60;
            state.cachedRadius=frame.shape.radius80;++row.effectRevision;view.wrote_=true;
        }
        view.completed_=result.status==g::PodLiveQueryStatus50IZ::Complete;
        return result;
    }
