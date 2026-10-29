//Private synchronous1B1800 custody. No factory or public resume entry exists.
    class SharedTerrainInvocation {
        friend class OwnershipBridge;
        friend struct ::interactive_setup50DU::Context; //Actual synchronous terrain callback only.
        enum class Cut { beforeAuxiliary120,afterAuxiliary124,limited,returned };
        SharedOwnerLease& lease_;
        EndCandidateResult& result_;
        storage::Storage::SharedRosterObject& row_;
        const g::BodyMatrix48G& scene100_;
        const void* sceneOwner_;
        Cut cut_=Cut::beforeAuxiliary120;
        bool entered_=false,limitedEntered_=false,forcesEntered_=false,engineAppliedEntered_=false,aeroEntered_=false,springEntered_=false,terrainResponseEntered_=false,secondaryForcesEntered_=false;
        std::optional<std::uint32_t> secondaryFactorF20Bits_;
        bool secondaryJointPrefixEntered_=false;
        //Private same-invocation F0 after real ready14EEC0 returns. No public
        //resume token; secondaryForcesEntered_ still prevents replay.
        std::optional<std::uint32_t> secondaryNoiseF0Bits_;
        std::size_t ordinal_=0;
        SharedTerrainInvocation(SharedOwnerLease& lease,EndCandidateResult& result,
            storage::Storage::SharedRosterObject& row,const g::BodyMatrix48G& scene,
            const void* owner)noexcept
            :lease_(lease),result_(result),row_(row),scene100_(scene),sceneOwner_(owner){}
    public:
        SharedTerrainInvocation(const SharedTerrainInvocation&)=delete;
        SharedTerrainInvocation& operator=(const SharedTerrainInvocation&)=delete;
    };
    //UNCALLED: no code currently advances the lexical cut past120/124. Future
    //actual auxiliary bodies must return in THIS invocation, not manufacture
    //a new lease/token or resume after Bank restore. All source effects below
    //remain unreachable from the existing unknown120 barrier.
    void continueNativeTerrainAfterAuxiliary(SharedTerrainInvocation& call)noexcept {
        auto& lease=call.lease_;auto& out=call.result_;auto& row=call.row_;
        if(call.cut_!=SharedTerrainInvocation::Cut::afterAuxiliary124||call.entered_)return;
        call.entered_=true;
        //Prior contact/reset/auxiliary effects cannot be retried even if this
        //entry's owner preflight rejects without a new write.
        struct Stop {OwnershipBridge& owner;~Stop(){owner.sharedEffectsStopped_=true;}} stop{*this};
        out.stoppedPC=0x1b1904;
        if(!sharedLeaseCurrent(lease)||!out.nativeTerrainSeedPublished||!actual_||!gridArena_||
           lease.producer_->activeRevision!=out.projectionRevision||!out.secondReturned||!out.merged||
           !out.sharedSecondFlagsPublished||out.secondCount<0)return;
        auto& frame=actual_->frame;auto& s=*storage_.get();
        const auto& racer=lease.producer_->activeSession->racerEntities_[lease.activeSlot_];
        const auto index=static_cast<int>(gridArena_->bindings.lifecycle.main190);
        if(!g::validVehicleSceneCount49W(frame,gridArena_->bindings)||index<0||
           static_cast<std::size_t>(index)>=frame.sceneNodeCount||
           &call.scene100_!=&frame.nodes[index].world100||call.sceneOwner_!=racer.scene190||
           !racer.body340||racer.body340->canonicalNumericAddress()!=&frame.bodies.main||
           racer.body340->scene288()!=racer.scene190||
           !racer.scene190||!racer.scene190->constructed||racer.scene190->released||
           !racer.scene190->entity().reference00||racer.scene190->entity().sessionGeneration!=row.generation||
           s.sharedRoster_->objects[lease.activeSlot_*storage::Storage::rosterRoles+
               static_cast<std::size_t>(storage::Storage::RosterRole::terrain348)].get()!=&row||
           !row.coldBound||row.slot!=lease.activeSlot_||row.role!=storage::Storage::RosterRole::terrain348||
           row.generation!=lease.producer_->activeSession->generation_||row.nativeTerrainBody128!=racer.body340||
           !row.terrainResource||!row.terrainResource->initialized||!row.terrainResult90||
           !row.terrainResult90->constructed_||row.terrainResult90->revoked_||
           row.type2Resource!=&row.terrainResource->resource||
           row.canonicalFilter!=&actual_->terrain.metadata.filter||!row.coldFilter||
           !row.coldFilter->matches(*row.canonicalFilter)||!row.reservedRef||row.reservedRef>s.count.objects||
           s.objects[row.reservedRef-1u].identity!=row.identity||
           row.identity!=row.canonicalFilter->identity||
           !sameSharedFilterView(s.objects[row.reservedRef-1u].filter,*row.coldFilter)||
           !registeredRange(&frame.terrainProbe,sizeof(frame.terrainProbe))||
           !registeredRange(&frame.bodies.main.domain,sizeof(frame.bodies.main.domain))||
           !registeredRange(&actual_->terrain.initial,sizeof(actual_->terrain.initial)))return;
        //Actual enableVehicleTerrain50FP/cold-roster native100 and enabled
        //policy lanes from THIS Bank slot, not a fabricated guest348 Entity.
        //Other domains do NOT take the original disabled return.
        //No phase/stage read is a substitute for the preceding source calls.
        if(!frame.terrainProbe.enabled||frame.bodies.main.domain.probe100!=1u)return;
        out.stoppedPC=0x1b1910;
        auto& preparedOwner=actual_->terrain.initial;
        if(!live_terrain_probe50fr::matches(*row.terrainResource,preparedOwner)||
           row.effectRevision==std::numeric_limits<std::uint64_t>::max())return;
        g::QuerySpherePrepareInput48O input{};
        input.preparedScene=call.scene100_;
        if(!frame.bodies.main.components.probe348.field104){
            input.preparedScene={};for(unsigned i=0;i<4;++i)input.preparedScene.v[5*i]=1.f;
            //1B1968..1980 copies XYZ only; identity W remains1.
            for(unsigned i=0;i<3;++i)input.preparedScene.v[12+i]=call.scene100_.v[12+i];
        }
        input.first=row.terrainResource->pair.first;input.second=row.terrainResource->pair.second;
        g::QuerySpherePrepareOutput48O prepared{};
        out.stoppedPC=0x11f680;
        if(g::prepareQuerySphere48O(input,prepared)!=g::QuerySpherePrepareStatus48O::Ready||
           !markSharedDirectQueryEffect(lease))return;
        preparedOwner.segment.worldMatrix=prepared.world;
        for(unsigned i=0;i<3;++i)preparedOwner.sphere.center.v[i]=prepared.center.v[i];
        preparedOwner.sphere.radius=prepared.radius;
        auto& mirror=s.source[row.reservedRef-1u];
        mirror.cachedCenter=prepared.center;mirror.cachedRadius=prepared.radius;++row.effectRevision;
        //Actual cache publication precedes120230 even when membership rejects.
        out.stoppedPC=0x120230;
        auto& membership=row.membership.state;
        if(!membership.attached||membership.object!=row.reservedRef||membership.lastShapeTag!=2||
           membership.lastType2Resource!=row.type2Resource)return;
        const auto bounds=live_terrain_probe50fr::bounds(*row.terrainResource,preparedOwner);
        const auto before=membership.revision;
        const auto updated=g::updateLiveZoneMembership50EN(s.lifecycle,s.world,membership,bounds,s.membershipWork);
        if(updated.status!=g::LiveZoneStatus50EN::Complete||before==std::numeric_limits<std::uint64_t>::max()||
           membership.revision!=before+1u||!membership.attached||membership.object!=row.reservedRef||
           membership.lastType2Resource!=row.type2Resource||
           std::memcmp(&membership.lastShapeWorld,&prepared.world,sizeof(prepared.world)))return;
        out.stoppedPC=0x1b1984;
        //212090/212094 bind a NONNULL inline SECOND list even for count zero.
        //Do not branch on count to generic121920, recollect or use merged IDs.
        const auto& selected=sharedProbeSelection_;
        const auto count=static_cast<std::size_t>(out.secondCount);
        if(selected.projection!=lease.projection_||selected.count!=count||count>selected.entries.size()||
           count>s.secondIds.size()||row.effectRevision==std::numeric_limits<std::uint64_t>::max()||
           !registeredRange(&row.canonicalFilter->flagsF8,sizeof(row.canonicalFilter->flagsF8)))return;
        for(std::size_t i=0;i<count;++i)if(selected.entries[i].identity!=s.secondIds[i])return;
        out.stoppedPC=0x1226a4;
        row.canonicalFilter->flagsF8=static_cast<std::uint8_t>(row.canonicalFilter->flagsF8|0x10u);
        mirror.flagsF8=row.canonicalFilter->flagsF8;++row.effectRevision;
        out.stoppedPC=0x11dd60;
        row.canonicalFilter->flagsF8=static_cast<std::uint8_t>(row.canonicalFilter->flagsF8&~0x20u);
        mirror.flagsF8=row.canonicalFilter->flagsF8;
        actual_->normalMode.acceptReachedLimitedQueryCounts11DD00();
        //124F70's process pointer is represented by this SAME lexical typed
        //owner borrow, not global guest3DF5B8 or a spring/MB transaction.
        auto& result=*row.terrainResult90;
        const auto& sentinel=original_follow_query_bridge::QueryBridge::sentinel3E02C0;
        std::memcpy(&result.selected_.first,&sentinel,sizeof(sentinel));
        std::memcpy(&result.selected_.second,&sentinel,sizeof(sentinel));
        result.selected_.fraction=1.f;result.count60_=0;result.resetKnown_=true;
        //All previous selected suffix/per-contact bytes survive11DD00.
        call.cut_=SharedTerrainInvocation::Cut::limited;out.stoppedPC=0x1226b0;
        if(!lease.producer_->terrainLimited)return;
        lease.producer_->terrainLimited(call); //Same current lease; never fresh projection.
        //The callback owns exact-count exhaustion and1B19B4, not219810 return.
    }
//Private source1B1800 prefix, still inside the SAME actual219810 Bank visit.
    void advanceNativeTerrainPreparePrefix(const original_camera_session50KX::Session& expected,
        std::size_t slot,EndCandidateResult& out)noexcept {
        out.stoppedPC=0x219c74;
        if(!out.nativeContactReturned||!storage_.get()||!storage_.get()->sharedRoster_)return;
        //Earlier query/contact effects already exist, even if THIS lease
        //cannot be acquired (including serial exhaustion) or preflight fails.
        //No supported1B1800 return exists past unknown120 in this packet.
        //Every reached shared entry is terminal, not merely entries that write.
        struct ReachedTerrainStop {
            OwnershipBridge& owner;
            ~ReachedTerrainStop(){owner.sharedEffectsStopped_=true;}
        } stopped{*this};
        if(out.nativeTerrainEntered||!actual_||!gridArena_)return;
        SharedOwnerLease lease;if(!beginSharedOwnerLease(lease,out))return;
        if(lease.producer_->activeSession!=&expected||lease.activeSlot_!=slot)return;
        auto& worldStorage=*storage_.get();
        auto& row=worldStorage.sharedRoster_->objects[slot*storage::Storage::rosterRoles+
            static_cast<std::size_t>(storage::Storage::RosterRole::terrain348)];
        const auto& racer=expected.racerEntities_[slot];
        const auto* body=row?row->nativeTerrainBody128:nullptr;
        auto& frame=actual_->frame;auto& probe=frame.bodies.main.components.probe348;
        auto& parameters=gridArena_->parameters.bodies.main;
        const auto app=expected.actual_.applicationEntities;
        out.stoppedPC=0x1b1810;
        if(!row||!row->coldBound||!row->coldFilter||row->slot!=slot||
           row->role!=storage::Storage::RosterRole::terrain348||
           row->generation!=expected.generation_||!row->terrainResource||
           !row->terrainResource->initialized||row->type2Resource!=&row->terrainResource->resource||
           !rosterTerrain_||!rosterTerrain_->prepared||
           row->canonicalFilter!=&rosterTerrain_->metadata.filter||
           row->identity!=row->canonicalFilter->identity||
           !body||body!=racer.body340||body!=stagedPods694_[slot].bodyA0||
           body->canonicalNumericAddress()!=&frame.bodies.main||
           !body->entity().reference00||body->entity().sessionGeneration!=expected.generation_||
           !app||app->nativeOwnerThread_!=std::this_thread::get_id()||
           !registeredRange(&probe,sizeof(probe))||
           !registeredRange(parameters.probeLocal140,sizeof(parameters.probeLocal140)))return;
        //Positive actual linked native body: absence was NOT interpreted as
        //the original NoBody/null128 return. There is no unbound-null shortcut.
        if(!markSharedDirectQueryEffect(lease))return;
        out.nativeTerrainEntered=true;
        out.stoppedPC=0x1b181c;probe.field108=0;
        probe.field110=0;probe.field114=0;probe.field118=0;
        probe.field1E8=0;probe.field12C=0; //Count only; slots/current identity survive.
        out.stoppedPC=0x1b1840;
        std::memcpy(probe.vector1D0,app->terrainClear434260_.data(),sizeof(probe.vector1D0));
        //Original1B1844 reads linked body.scene288 then RAW cached100, not a
        //scene getter. Do not update dirty scene/parents or require clean bits.
        out.stoppedPC=0x1b1848;
        const auto* scene=body->scene288();
        if(!scene||scene!=racer.scene190||!scene->constructed||scene->released||
           !scene->entity().reference00||scene->entity().sessionGeneration!=expected.generation_||
           scene->sceneIndex!=gridArena_->bindings.lifecycle.main190||
           !g::validVehicleSceneCount49W(frame,gridArena_->bindings))return;
        const auto index=static_cast<int>(scene->sceneIndex);
        if(index<0||static_cast<std::size_t>(index)>=frame.sceneNodeCount)return;
        const auto& node=frame.nodes[index];if(!registeredRange(&node,sizeof(node)))return;
        out.stoppedPC=0x1b1854;
        std::uint32_t localW;std::memcpy(&localW,parameters.probeLocal140+3,4);
        if(localW!=0x3f800000u)return; //Bounded genuine setter140.W domain.
        g::Vector3_48C local,point;std::memcpy(local.v,parameters.probeLocal140,12);
        if(!g::prepareTerrainSeedPoint48M(node.world100,local,point))return;
        std::memcpy(probe.point150,point.v,12);
        const std::uint32_t one=0x3f800000u;std::memcpy(probe.point150+3,&one,4);
        out.nativeTerrainSeedPublished=true;
        [[maybe_unused]] SharedTerrainInvocation continuation(lease,out,*row,node.world100,scene);
        //No cut transition or service call:120 and124 are not yet owned.
        //21207C overwrote ctor120/124 with racer330/334;2107FC/210804
        //copied actual World3FC/400. Those owners remain unpublished in the
        //current World, NOT known null. Native Fixture false flags do not
        //permit either source branch. Preserve all earlier writes at120.
        out.stoppedPC=0x1b188c;
        //No completeSharedOwnerLease: this actual partial invocation is
        //nonretry. Bank::visit saves the six resets/vector/point before exit.
    }
