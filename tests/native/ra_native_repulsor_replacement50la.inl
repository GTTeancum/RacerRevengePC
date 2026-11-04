//Included only inside OwnershipBridge. Native full-publication route, not a
//fresh source348 constructor or a query/2146 loader return capability.
    using NativeTerrainCut=storage::Storage::SharedRosterObject::NativeTerrainCut;
    const g::Type2Ray49X* sharedTerrainPair(const storage::Storage::SharedRosterObject& row)const noexcept {
        if(!row.terrainResource||!row.terrainResult90)return nullptr;
        const auto& live=*row.terrainResource;
        if(row.nativeTerrainCut==NativeTerrainCut::none){
            if(row.nativeTerrainPayload||!live.initialized||live.nativePayload||live.legacyWithdrawn||
               live.sourceShape8C||live.sourceWithdrawn||live.resource.rays!=&live.pair||
               live.resource.rayCount!=1u||live.resource.nodeCount||live.resource.root!=g::type2Leaf49X)
                return nullptr;
            return &live.pair;
        }
        //Cross-slot readers may validate the UNBANKED row and retained payload;
        //they must not inspect its bankView_ while a different slot is active.
        const auto* payload=row.nativeTerrainPayload.get();
        if(row.nativeTerrainCut!=NativeTerrainCut::published||!payload||!payload->bodyCurrent()||
           payload->body_!=row.nativeTerrainBody128||payload->generation_!=row.generation||
           payload->rowView_!=&live||payload->resultView_!=row.terrainResult90.get()||
           !payload->liveCurrent(live)||!payload->resultCurrent())return nullptr;
        return live.resource.rays;
    }
    bool sharedTerrainMatches(const storage::Storage::SharedRosterObject& row,
        const g::TerrainPrepareState48M& prepared)const noexcept {
        const auto* pair=sharedTerrainPair(row);
        return pair&&row.type2Resource==&row.terrainResource->resource&&
            !std::memcmp(pair->first.v,prepared.segment.first.v,12)&&
            !std::memcmp(pair->second.v,prepared.segment.second.v,12);
    }
    bool sharedTerrainBankViewMatches(const storage::Storage::SharedRosterObject& row,
        const live_terrain_probe50fr::Owner& view)const noexcept {
        if(!actual_)return false;
        const auto* pair=sharedTerrainPair(row);if(!pair)return false;
        if(row.nativeTerrainPayload)return row.nativeTerrainPayload->liveCurrent(view);
        //A copied Bank view still borrows the one canonical arena pair, not
        //the address of the temporary inspection copy's embedded pair.
        return view.initialized&&!view.sourceShape8C&&!view.sourceWithdrawn&&
            !view.nativePayload&&!view.legacyWithdrawn&&
            view.resource.rays==&actual_->terrainLive.pair&&view.resource.rayCount==1u&&
            !view.resource.nodes&&!view.resource.nodeCount&&view.resource.root==g::type2Leaf49X&&
            !std::memcmp(&view.pair,pair,sizeof(*pair));
    }
    static bool legacyTerrainViewCurrent(const live_terrain_probe50fr::Owner& view)noexcept {
        return view.initialized&&!view.sourceShape8C&&!view.sourceWithdrawn&&
            !view.nativePayload&&!view.legacyWithdrawn&&view.resource.rays==&view.pair&&
            view.resource.rayCount==1u&&!view.resource.nodeCount&&view.resource.root==g::type2Leaf49X;
    }
    static void withdrawNativeLegacyTerrain(live_terrain_probe50fr::Owner& view)noexcept {
        //Fixed backing remains allocated, but no consumer may reach its old
        //pair again. This native withdrawal does not impersonate11B600.
        view.initialized=false;view.resource={};view.nativePayload=nullptr;view.legacyWithdrawn=true;
    }
    bool nativeTerrainRetirementCurrent(const storage::Storage::SharedRosterObject& row)const noexcept {
        if(!actual_||!bankIdentity_||bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=row.slot||
           !row.terrainResource||!row.terrainResult90||!row.nativeTerrainBody128||
           row.nativeTerrainCut==NativeTerrainCut::none||row.nativeTerrainCut==NativeTerrainCut::retired)return false;
        const auto* payload=row.nativeTerrainPayload.get();
        if(row.nativeTerrainCut==NativeTerrainCut::admitted||row.nativeTerrainCut==NativeTerrainCut::removed){
            //A setter/budget/allocation failure before actual11C424 retains
            //the old views. They need not match newly-written130/140 lanes.
            if(!legacyTerrainViewCurrent(*row.terrainResource)||!legacyTerrainViewCurrent(actual_->terrainLive)||
               !row.terrainResult90->constructed_||row.terrainResult90->revoked_||
               row.terrainResult90->sourceResult90_||row.terrainResult90->sourceWithdrawn_||
               row.terrainResult90->nativePayload_||row.terrainResult90->nativeResult90_||
               row.type2Resource!=&row.terrainResource->resource)return false;
            if(!payload)return row.nativeTerrainCut==NativeTerrainCut::admitted;
            return payload->bodyCurrent()&&payload->body_==row.nativeTerrainBody128&&
                payload->generation_==row.generation&&payload->rowView_==row.terrainResource.get()&&
                payload->bankView_==&actual_->terrainLive&&payload->resultView_==row.terrainResult90.get()&&
                !payload->payload_.attempted()&&!payload->projected_&&!payload->withdrawn_;
        }
        if(!payload||!payload->current()||payload->body_!=row.nativeTerrainBody128||
           payload->generation_!=row.generation||payload->rowView_!=row.terrainResource.get()||
           payload->bankView_!=&actual_->terrainLive||payload->resultView_!=row.terrainResult90.get())return false;
        return row.type2Resource==(payload->projected_?&row.terrainResource->resource:nullptr);
    }
    bool nativeRepulsorRow(std::size_t slot,const g::OriginalRacerEntityCold50LA& racer,
        const native_racer_model50LA::Owner& model,storage::Storage::SharedRosterObject*& out)const noexcept {
        const auto pin=bankLifetime_.lock();auto* s=storage_.get();
        if(source348Arena_||terminal_||closed_||sharedEffectsStopped_||sharedOwnerLease_||sharedRoleViews_||
           nativeFifoPins_||triggerQueryActive_||(endCandidateProducer_&&endCandidateProducer_->inCall)||
           !pin||!bankIdentity_||pin->identity!=bankIdentity_||bankIdentity_->raWorldLifetime_.get()!=pin.get()||
           !bankIdentity_->sealed_||bankIdentity_->retired_||bankIdentity_->visitDepth_!=1u||
           bankIdentity_->currentSlot_!=slot||(!bankIdentity_->visiting_&&!bankIdentity_->readBlocked_)||
           bankIdentity_->sourceQueryPending()||!s||!s->sharedRoster_||s->sharedRoster_->retired||s->queue.count||
           slot>=s->sharedRoster_->count||slot>=source348CountedOwners_.size()||source348CountedOwners_[slot]||
           !actual_||!gridArena_||!gridInputsRegistered(*gridArena_)||!camera_||!camera_->session||
           camera_->session->closed_||camera_->session->abandoned_||
           camera_->session->generation_!=racer.racer.sessionGeneration||
           &camera_->session->actual_.bank!=bankIdentity_||&camera_->session->racerEntities_[slot]!=&racer||
           !camera_->session->actual_.actualChariotModels||
           (*camera_->session->actual_.actualChariotModels)[slot].get()!=&model||
           model.bank_!=bankIdentity_||model.slot_!=slot||model.racer_!=&racer||
           !model.application_||model.application_!=camera_->session->actual_.applicationEntities||
           model.application_->nativeOwnerThread_!=std::this_thread::get_id()||
           model.handlingRoute_!=native_racer_model50LA::Owner::HandlingRoute::legacy||
           !model.nativePhysicalComplete_||!model.nativeRepulsorAttempted_||model.nativeRepulsorComplete_||
           model.generation_!=racer.racer.sessionGeneration||model.actualRig()!=gridRigIdentity_[slot]||
           !racer.prepared()||racer.nativeWreckWithdrawn_||!racer.body340||!racer.scene190||
           racer.body340!=model.body340_.get()||racer.body340->scene288()!=racer.scene190||
           racer.body340->canonicalNumericAddress()!=&actual_->frame.bodies.main||
           racer.body340->entity().reference00!=1u||racer.body340->entity().parent1C!=&racer.racer||
           racer.body340->entity().sessionGeneration!=racer.racer.sessionGeneration||
           racer.scene190->released||!racer.scene190->constructed||!racer.scene190->entity().reference00||
           racer.scene190->entity().sessionGeneration!=racer.racer.sessionGeneration||
           !gridCompletion_[slot].complete||!freshComplete_[slot]||
           !gridRetirementRequired_[slot]||!freshRetirementRequired_[slot])return false;
        auto& q=*actual_;auto& n=q.terrain;
        if(!registeredRange(&n.spring1F4,sizeof(n.spring1F4))||!registeredRange(&n.damping1F8,sizeof(n.damping1F8))||
           !registeredRange(&n.gravity1FC,sizeof(n.gravity1FC))||!registeredRange(&n.sideForce200,sizeof(n.sideForce200))||
           !registeredRange(&n.lift1F0,sizeof(n.lift1F0))||!registeredRange(&n.follow,sizeof(n.follow))||
           !registeredRange(&q.frame.terrainProbe,sizeof(q.frame.terrainProbe)))return false;
        std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles> rows{};
        if(!sharedColdRowsCurrent(slot,rows))return false;
        auto& row=*rows[2];auto& member=row.membership.state;
        if(row.nativeTerrainCut!=NativeTerrainCut::none||row.nativeTerrainPayload||
           row.nativeTerrainBody128!=racer.body340||row.generation!=racer.racer.sessionGeneration||
           row.collision!=bodyIdentities_[slot].collision||q.main.resource!=&row.collision->main()||
           !row.terrainResult90||!row.terrainResult90->constructed_||row.terrainResult90->revoked_||
           row.terrainResult90->sourceResult90_||row.terrainResult90->nativePayload_||
           !legacyTerrainViewCurrent(q.terrainLive)||!n.prepared||
           !n.metadata.normalMode54Bound||n.metadata.normalMode54!=1u||!n.metadata.terrainCallbackEnabled||
           !q.frame.terrainProbe.enabled||q.frame.bodies.main.domain.probe100!=1u||
           !member.attached||member.object!=row.reservedRef||member.lastType2Resource!=row.type2Resource||
           member.lastShapeTag!=2u||!member.storage.links||!member.storage.capacity||
           !member.storage.links[0].zone||member.storage.links[0].zone>s->lifecycle.zoneCount||
           s->objects[row.reservedRef-1u].identity!=row.identity||
           !sameSharedFilterView(s->objects[row.reservedRef-1u].filter,*row.coldFilter)||
           member.revision!=gridCompletion_[slot].terrainRevision||
           row.effectRevision>std::numeric_limits<std::uint64_t>::max()-3u||
           member.revision>std::numeric_limits<std::uint64_t>::max()-3u)return false;
        out=&row;return true;
    }
    bool writeNative348ScalarBefore21494C(std::size_t slot,const g::OriginalRacerEntityCold50LA& racer,
        const native_racer_model50LA::Owner& model,float value)noexcept {
        storage::Storage::SharedRosterObject* row=nullptr;
        if(!std::isfinite(value)||!nativeRepulsorRow(slot,racer,model,row))return false;
        if(racer.nativePostEmberPC_==0x2148f0u){actual_->terrain.sideForce200=value;return true;}
        if(racer.nativePostEmberPC_==0x214918u){actual_->terrain.gravity1FC=value;return true;}
        return false;
    }
    bool readNativeRepulsorBound324(std::size_t slot,const g::OriginalRacerEntityCold50LA& racer,
        const native_racer_model50LA::Owner& model,float& value)const noexcept {
        storage::Storage::SharedRosterObject* row=nullptr;
        if(racer.nativePostEmberPC_!=0x21491cu||!nativeRepulsorRow(slot,racer,model,row))return false;
        //Explicit NATIVE operand from the same owned COL, not publication of
        //unknown counted sourceCollisionHalf320_ or adoption of a receipt.
        g::Type2BoundsInput50BY input{};input.resource=&row->collision->main();
        for(unsigned i=0;i<4;++i)input.preparedWorld.v[5*i]=1.f;
        g::CollectionBoundsResult48S bounds{};
        if(g::prepareType2Bounds50BY(input,bounds)!=g::CollectionBoundsStatus48S::Complete||
           std::bit_cast<std::uint32_t>(bounds.half.v[1])!=
               std::bit_cast<std::uint32_t>(actual_->terrain.receipt.vehicle324))return false;
        value=bounds.half.v[1];return true;
    }
    bool applyNative348Setup21494C(std::size_t slot,g::OriginalRacerEntityCold50LA& racer,
        const native_racer_model50LA::Owner& model,float native324,float lift,float springInput,
        float dampingInput,float orientation,std::uint32_t& pc)noexcept {
        storage::Storage::SharedRosterObject* row=nullptr;
        if(racer.nativePostEmberPC_!=0x21494cu||pc!=0x21494cu||
           !nativeRepulsorRow(slot,racer,model,row))return false;
        auto& s=*storage_.get();auto& q=*actual_;auto& n=q.terrain;auto& a=*gridArena_;
        auto& member=row->membership.state;
        if(std::fegetround()!=FE_TONEAREST||!std::isfinite(native324)||!std::isfinite(lift)||
           !std::isfinite(springInput)||!std::isfinite(dampingInput)||!std::isfinite(orientation)||
           std::bit_cast<std::uint32_t>(native324)!=std::bit_cast<std::uint32_t>(n.receipt.vehicle324)||
           !n.gravity1FC||!racer.sourceGravity378_||
           std::bit_cast<std::uint32_t>(*n.gravity1FC)!=std::bit_cast<std::uint32_t>(*racer.sourceGravity378_))return false;
        const auto main=static_cast<std::size_t>(racer.scene190->sceneIndex);
        if(main>=q.frame.sceneNodeCount||racer.scene190->sceneIndex!=a.bindings.lifecycle.main190||
           !g::validVehicleSceneCount49W(q.frame,a.bindings))return false;
        const auto zone=member.storage.links[0].zone;const auto method=member.method;
        const auto oldObject=s.objects[row->reservedRef-1u];const auto oldState=s.source[row->reservedRef-1u];
        row->nativeTerrainCut=NativeTerrainCut::admitted;
        const auto at=[&](std::uint32_t address){pc=row->nativeTerrainPC=racer.nativePostEmberPC_=address;};
        at(0x21494cu);
        //Unchanged native capacity. Charge actual retained payload plus all
        //new arrays and conservative temporary peak before any allocation.
        const auto bytes=native_repulsor_payload50LA::Owner::allocationBudget();
        if(bytes>s.sharedRoster_->coldPayloadRemaining)return false;
        s.sharedRoster_->coldPayloadRemaining-=bytes;
        try {row->nativeTerrainPayload.reset(new native_repulsor_payload50LA::Owner(
            *racer.body340,*row->terrainResource,q.terrainLive,*row->terrainResult90));}
        catch(...){return false;}
        n.prepared=false; //Native consumer fence, not a source enabled100 store.
        namespace arithmetic=terrain_probe_authoring50fq_fixture::detail;
        const float conversion=std::bit_cast<float>(0x4200cccdu);
        at(0x1b1270u);n.lift1F0=lift;
        const float mass=a.parameters.bodies.main.body.mass;
        at(0x1b1298u);const float spring=std::fabs(arithmetic::mul(conversion,arithmetic::mul(springInput,mass)));
        if(!std::isfinite(spring))return false;
        at(0x1b12a4u);n.spring1F4=spring;
        at(0x1b12c0u);const float torque=std::fabs(arithmetic::mul(conversion,arithmetic::mul(orientation,mass)));
        if(!std::isfinite(torque))return false;
        at(0x1b12ccu);n.follow.airborneTorque204=torque;q.frame.terrainProbe.response.airborneTorque204=torque;
        float damping=0.f;
        if(mass>0.f&&spring>0.f){
            at(0x1b12fcu);const float product=arithmetic::mul(mass,spring);
            const float twice=arithmetic::mul(2.f,dampingInput);std::uint32_t word=0;
            if(!g::SourceTerrainSetupMath50LA::damping1B12FC(std::bit_cast<std::uint32_t>(product),
                std::bit_cast<std::uint32_t>(twice),word))return false;
            damping=std::bit_cast<float>(word);
        }
        at(mass>0.f&&spring>0.f?0x1b1338u:0x1b133cu);n.damping1F8=damping;
        const float offset[3]={0,native324,0};
        g::SpringVector4_48N first{},second{},local{};
        at(0x1b1390u);
        for(unsigned i=0;i<3;++i){first.v[i]=arithmetic::add(a.parameters.bodies.main.centerOffset.v[i],offset[i]);
            if(!std::isfinite(first.v[i]))return false;}
        first.v[3]=1;n.receipt.probe130=first;
        //Read the same process up434280 owner, not an equal literal stand-in.
        float up[3];std::memcpy(up,model.application_->terrainUp434280_.data(),sizeof(up));
        at(0x1b145cu);const float fiveLift=arithmetic::mul(5.f,lift);
        for(unsigned i=0;i<3;++i){const float base=arithmetic::sub(first.v[i],arithmetic::mul(2.f,offset[i]));
            second.v[i]=arithmetic::sub(base,arithmetic::mul(fiveLift,up[i]));
            if(!std::isfinite(second.v[i]))return false;}
        second.v[3]=1;n.receipt.probe140=second;
        for(unsigned i=0;i<3;++i)n.lowerEndpoint140.v[i]=second.v[i];
        std::memcpy(a.parameters.bodies.main.probeLocal140,second.v,sizeof(second.v));
        //Only NOW reach11C3F0. The old terrain query alone is detached; main,
        //secondary, POD, movement, filters and their revisions stay untouched.
        at(0x11c424u);
        if(g::removeLiveZoneMembership50EN(s.lifecycle,s.world,member,s.membershipWork).status!=
            g::LiveZoneStatus50EN::Complete)return false;
        row->nativeTerrainCut=NativeTerrainCut::removed;
        s.objects[row->reservedRef-1u]={};s.source[row->reservedRef-1u]={};++row->effectRevision;
        row->terrainResult90->withdrawLegacy();
        withdrawNativeLegacyTerrain(*row->terrainResource);withdrawNativeLegacyTerrain(q.terrainLive);
        row->type2Resource=nullptr;row->nativeTerrainCut=NativeTerrainCut::withdrawn;
        if(!row->nativeTerrainPayload->construct11C3F0(first,second,pc)){
            row->nativeTerrainPC=racer.nativePostEmberPC_=pc;return false;
        }
        row->type2Resource=&row->terrainResource->resource;row->nativeTerrainCut=NativeTerrainCut::returned;
        //The existing native typed callback remains this exact row's binding.
        //No counted E4/9C stores or Racer694 token are invented here.
        at(0x1b1478u);
        n.initial.segment.first=arithmetic::identityPoint(first);
        n.initial.segment.second=arithmetic::identityPoint(second);
        at(0x1b1540u);
        for(unsigned i=0;i<3;++i){const float base=arithmetic::sub(first.v[i],arithmetic::mul(2.f,offset[i]));
            local.v[i]=arithmetic::sub(base,arithmetic::mul(lift,up[i]));
            if(!std::isfinite(local.v[i]))return false;}
        local.v[3]=1;
        for(unsigned i=0;i<3;++i){n.follow.localOffset1c0.v[i]=local.v[i];
            q.frame.terrainProbe.response.localOffset1c0.v[i]=local.v[i];}
        n.receipt.bodyMass=mass;n.receipt.authoredLift=lift;n.receipt.authoredOrientationSpring=orientation;
        n.receipt.localOffset1C0=local;
        //Pure native geometric cache refresh after the setter return. This
        //does not invoke1B1800, assume auxiliary120/124 NULL, or reset history.
        at(0x214954u);g::QuerySpherePrepareInput48O input{};
        input.preparedScene=q.frame.nodes[main].world100;
        if(!q.frame.bodies.main.components.probe348.field104){
            input.preparedScene={};for(unsigned i=0;i<4;++i)input.preparedScene.v[5*i]=1.f;
            for(unsigned i=0;i<3;++i)input.preparedScene.v[12+i]=q.frame.nodes[main].world100.v[12+i];
        }
        const auto* pair=row->terrainResource->resource.rays;
        input.first=pair->first;input.second=pair->second;
        g::QuerySpherePrepareOutput48O prepared{};
        if(g::prepareQuerySphere48O(input,prepared)!=g::QuerySpherePrepareStatus48O::Ready)return false;
        n.initial.segment.worldMatrix=prepared.world;
        std::memcpy(n.initial.sphere.center.v,prepared.center.v,sizeof(n.initial.sphere.center.v));
        n.initial.sphere.radius=prepared.radius;
        const auto bounds=live_terrain_probe50fr::bounds(*row->terrainResource,n.initial);
        if(g::attachLiveZoneMembership50EN(s.lifecycle,s.world,member,row->membership.storage(),
            row->reservedRef,zone,method,0.f,bounds,s.membershipWork).status!=g::LiveZoneStatus50EN::Complete)return false;
        row->nativeTerrainCut=NativeTerrainCut::attached;
        if(!row->nativeTerrainPayload->current()||!member.attached||member.object!=row->reservedRef||
           member.lastType2Resource!=row->type2Resource||member.lastShapeTag!=2u)return false;
        //Republish the same identity/filter/epoch/member metadata. Only the
        //cache and actual filter flags changed; this is not a fresh object.
        s.objects[row->reservedRef-1u]=oldObject;
        s.source[row->reservedRef-1u]=oldState;
        s.source[row->reservedRef-1u].cachedCenter=member.bounds.cachedCenter;
        s.source[row->reservedRef-1u].cachedRadius=member.bounds.cachedRadius;
        s.source[row->reservedRef-1u].flagsF8=row->canonicalFilter->flagsF8;
        ++row->effectRevision;gridCompletion_[slot].terrainRevision=member.revision;
        row->nativeTerrainCut=NativeTerrainCut::published;n.prepared=true;
        return true; //Native bounded214954 only, no original2146 return.
    }
    bool retireNativeRepulsorPayload(std::size_t slot,native_racer50HN::Bank& bank)noexcept {
        auto* s=storage_.get();
        if(!s||!s->sharedRoster_||slot>=s->sharedRoster_->count)return true;
        auto& row=*s->sharedRoster_->objects[slot*storage::Storage::rosterRoles+2u];
        if(row.nativeTerrainCut==NativeTerrainCut::none||row.nativeTerrainCut==NativeTerrainCut::retired)return true;
        if(&bank!=bankIdentity_||!cameraBankCurrent()||!actual_||sharedOwnerLease_||sharedRoleViews_||nativeFifoPins_||triggerQueryActive_||
           (endCandidateProducer_&&endCandidateProducer_->inCall)||gridRetirementRequired_[slot]||
           freshRetirementRequired_[slot]||row.membership.state.attached||row.membership.state.object||
           s->objects[row.reservedRef-1u].identity)return false;
        bool done=false;
        try {bank.visit(slot,[&](){
            if(!nativeTerrainRetirementCurrent(row))return;
            if(row.nativeTerrainCut==NativeTerrainCut::admitted||row.nativeTerrainCut==NativeTerrainCut::removed){
                row.terrainResult90->withdrawLegacy();
                withdrawNativeLegacyTerrain(*row.terrainResource);withdrawNativeLegacyTerrain(actual_->terrainLive);
                row.type2Resource=nullptr;
            }
            if(row.nativeTerrainPayload){
                if(!row.nativeTerrainPayload->current())return;
                row.nativeTerrainPayload->withdrawViews();
            }else row.terrainResult90->revoke();
            actual_->terrain.prepared=false;row.type2Resource=nullptr;
            //Slot0 visit does NOT save automatically. Save this exact selected
            //slot now, so no saved raw pointer survives the following release.
            bank.save(slot);
            if(row.nativeTerrainPayload)row.nativeTerrainPayload->releaseWithdrawn();
            row.nativeTerrainCut=NativeTerrainCut::retired;done=true;
        });return done;}catch(...){return false;}
    }
