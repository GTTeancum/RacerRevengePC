//Private fresh shared-mode producer/retirement adaptation, never a live reset.
    bool sharedPodContextOwned(const g::PodLiveQueryContext50IZ* context)const noexcept {
        const auto* s=storage_.get();if(!context||!s||!s->sharedRoster_||!podParameters_)return false;
        for(std::size_t slot=0;slot<s->sharedRoster_->count;++slot){
            const auto& row=s->sharedRoster_->objects[slot*storage::Storage::rosterRoles+
                static_cast<std::size_t>(storage::Storage::RosterRole::pod694)];
            if(row&&row->podContext&&context==&*row->podContext)
                return context->source==&podParameters_->shape&&
                    &context->live.lifecycle==&s->lifecycle&& &context->live.world==&s->world&&
                    &context->live.registrationE0==&row->membership.state&&
                    context->live.expectedObjectRef==row->reservedRef;
        }
        return false;
    }
    bool sharedColdRowsCurrent(std::size_t slot,
        std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles>& rows,
        bool retirementOnly=false)const noexcept {
        const auto pin=bankLifetime_.lock();const auto* s=storage_.get();
        if(!pin||!bankIdentity_||pin->identity!=bankIdentity_||
           bankIdentity_->raWorldLifetime_.get()!=pin.get()||!bankIdentity_->sealed_||bankIdentity_->retired_||
           bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
           !s||!s->sharedRoster_||s->sharedRoster_->retired||slot>=s->sharedRoster_->count||
           bankIdentity_->count_!=s->sharedRoster_->count||!actual_||!actual_->recovery||!actual_->movement||
           !rosterSlotComplete_[slot])return false;
        const auto& q=*actual_;const auto& ids=bodyIdentities_[slot];const auto& staged=stagedPods694_[slot];
        if(!ids.collision||!ids.collision->prepared()||!staged.bodyA0||!staged.generation||
           staged.collision!=ids.collision||staged.bodyA0->canonicalNumericAddress()!=&q.frame.bodies.main||
           !staged.bodyA0->entity().reference00||staged.bodyA0->entity().sessionGeneration!=staged.generation||
           q.main.resource!=&ids.collision->main()||q.secondary.resource!=&ids.collision->secondary()||
           !q.recovery->sourcePrepared||q.recovery->selectedSource!=ids.collision)return false;
        const g::CollisionFilter48J* filters[]={&q.mainFilter,&q.secondaryFilter,&q.terrain.metadata.filter,
            &q.recovery->podFilter,&q.movement->filter};
        for(std::size_t i=0;i<rows.size();++i){const auto& row=s->sharedRoster_->objects[slot*rows.size()+i];
            if(!row||row->slot!=slot||static_cast<std::size_t>(row->role)!=i||!row->coldBound||
               row->collision!=ids.collision||row->generation!=staged.generation||!row->coldFilter||
               row->canonicalFilter!=filters[i]||row->identity!=filters[i]->identity||
               !row->coldFilter->matches(*filters[i])||!row->reservedRef||row->reservedRef>s->count.objects)return false;
            rows[i]=row.get();
        }
        return rows[0]->type2Resource==q.main.resource&&rows[1]->type2Resource==q.secondary.resource&&
            ((retirementOnly&&rows[2]->nativeTerrainCut!=storage::Storage::SharedRosterObject::NativeTerrainCut::none)?
                nativeTerrainRetirementCurrent(*rows[2]):
                (sharedTerrainMatches(*rows[2],q.terrain.initial)&&sharedTerrainBankViewMatches(*rows[2],q.terrainLive)))&&
            rows[3]->bodyA0==staged.bodyA0&&rows[3]->type0Resource==&ids.collision->mainTarget().source;
    }
    bool bindSharedPodLiveQuery(native_racer50HN::Bank& bank,std::size_t slot,
        g::VehicleFrameBindings49W& bindings,g::VehicleFrameParameters49W& parameters,Diagnostic& d,
        std::uint32_t humanCount=0)noexcept {
        d={};auto* s=storage_.get();
        if(!s||!s->sharedRoster_||terminal_||closed_||cameraAttempted_||sharedEffectsStopped_||
           sharedOwnerLease_||sharedRoleViews_||nativeFifoPins_||triggerQueryActive_||
           (endCandidateProducer_&&endCandidateProducer_->inCall)||!worldOwnersRegistered_||
           !actual_||!actual_->recovery||!cameraBankCurrent()||&bank!=bankIdentity_||
           slot>=s->sharedRoster_->count||humanCount>1u||humanCount!=inputs_.humanCount||
           gridAttempted_[slot]||podBindingInstalled_[slot]||
           !registeredRange(&bindings,sizeof(bindings))||!registeredRange(&parameters,sizeof(parameters))||
           (podBindings_&&podBindings_!=&bindings)||(podParameters_&&podParameters_!=&parameters))
            return fail(d,"shared POD binding requires actual cold Bank/arena and no query borrowers");
        constexpr auto noise=offsetof(g::VehicleFrameState49W,noise);
        constexpr auto after=noise+sizeof(g::CoupledNoiseContext49M);
        const auto* bytes=reinterpret_cast<const unsigned char*>(&actual_->frame);
        if(!registeredRange(bytes,noise)||!registeredRange(bytes+after,sizeof(actual_->frame)-after)||
           (gridArena_&&(&gridArena_->bindings!=&bindings||&gridArena_->parameters!=&parameters)))
            return fail(d,"actual shared POD frame/parameter backing differs");
        bool complete=false;
        try {bank.visit(slot,[&](){
            std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles> rows{};
            if(!sharedColdRowsCurrent(slot,rows)){fail(d,"actual shared cold role owners differ");return;}
            auto& q=*actual_;auto& pod=*rows[3];const auto& source=*pod.type0Resource;
            if(q.frame.phase!=g::VehicleFramePhase49W::Ready||q.frame.bodies.main.frameFactor!=0.f||
               (slot<humanCount?(q.frame.fields.vehicle58!=0u||q.frame.fields.nativeAiOwnersBound50IV):
                    (q.frame.fields.vehicle58!=1u||!q.frame.fields.nativeAiOwnersBound50IV))||
               q.frame.shape.registrationE0||pod.membership.state.attached||rows[4]->membership.state.attached||
               pod.podContext||bindings.podLiveQuery50IZ||
               std::memcmp(source.local40.v,parameters.shape.local40.v,sizeof source.local40.v)||
               std::memcmp(source.rootCenter.v,parameters.shape.rootCenter.v,sizeof source.rootCenter.v)||
               std::memcmp(source.rootHalf.v,parameters.shape.rootHalf.v,sizeof source.rootHalf.v)||
               !g::validVehicleSceneCount49W(q.frame,bindings)){
                fail(d,"shared POD context requires actual fresh selected shape and empty source binding");return;}
            //Actual row storage was budgeted by sizeof(SharedRosterObject).
            //It outlives this pointer in EVERY saved slot until Bank retirement.
            pod.podContext.emplace(g::PodLiveQueryContext50IZ{{s->lifecycle,s->world,pod.membership.state,
                s->membershipWork,pod.reservedRef},&parameters.shape});
            podBindings_=&bindings;podParameters_=&parameters;podHumanCount_=humanCount;
            bindings.podLiveQuery50IZ=&*pod.podContext;podBindingInstalled_[slot]=true;complete=true;
        });return complete;}catch(...){return fail(d,"shared POD context binding exception; real saved prefix retained");}
    }
    bool publishSharedColdGridRole(storage::Storage::SharedRosterObject& row,
        const g::LiveZoneBounds50EN& bounds)noexcept {
        auto& s=*storage_.get();const auto& member=row.membership.state;
        if(!row.coldBound||!row.coldFilter||!row.canonicalFilter||
           !row.coldFilter->matches(*row.canonicalFilter)||!row.reservedRef||row.reservedRef>s.count.objects||
           row.role>storage::Storage::RosterRole::terrain348||!member.attached||member.object!=row.reservedRef||
           member.lastShapeTag!=2u||member.lastType2Resource!=row.type2Resource||
           bounds.type2Resource!=row.type2Resource||s.objects[row.reservedRef-1u].identity||
           row.effectRevision==std::numeric_limits<std::uint64_t>::max())return false;
        g::CollisionSphere48J sphere{};std::memcpy(sphere.center.v,bounds.bounds.cachedCenter.v,12);
        sphere.radius=bounds.bounds.cachedRadius;
        auto projected=g::projectCollectionObject50CF(*row.canonicalFilter,sphere,actual_->main.collisionTypeTag,2u,
            std::bit_cast<std::uint32_t>(bounds.bounds.cachedCenter.v[3]));
        if(!projected.valid||projected.object.identity!=row.identity)return false;
        projected.object.filter=row.coldFilter->view; //Own genuine cold list backing, never Bank-restored vectors.
        s.objects[row.reservedRef-1u]=projected.object;s.source[row.reservedRef-1u]=projected.state;
        ++row.effectRevision;
        //No extent/head/query activation:694/7A4 and all other roster roles
        //still require their actual producers before collector admission.
        return true;
    }
    bool retireSharedGrid(native_racer50HN::Bank& bank,std::size_t slot,Diagnostic& d)noexcept {
        d={};auto* s=storage_.get();
        if(!s||!s->sharedRoster_||closed_||sharedOwnerLease_||sharedRoleViews_||sharedEffectsStopped_||
           nativeFifoPins_||triggerQueryActive_||(endCandidateProducer_&&endCandidateProducer_->inCall)||
           !cameraBankCurrent()||&bank!=bankIdentity_||slot>=s->sharedRoster_->count||
           (camera_&&camera_->session&&!camera_->session->closed_)||
           (follow_&&follow_->attempted_&&!follow_->closed_)){
            d.message="shared grid retirement requires actual quiescent same Bank/World";return false;}
        gridCompletion_[slot].complete=false;freshComplete_[slot]=false;
        if(freshRetirementRequired_[slot]){
            d.message="actual shared fresh visibility/recovery must retire first";return false;}
        if(!gridRetirementRequired_[slot])return true;
        terminal_=true;
        bool complete=false;
        try {bank.visit(slot,[&](){
            std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles> rows{};
            if(!sharedColdRowsCurrent(slot,rows,true)){d.message="actual shared grid retirement owner differs";return;}
            for(unsigned i=0;i<3;++i){auto& row=*rows[i];auto& object=s->objects[row.reservedRef-1u];
                if((object.identity&&object.identity!=row.identity)||
                   (row.membership.state.attached&&row.membership.state.object!=row.reservedRef)||
                   row.effectRevision==std::numeric_limits<std::uint64_t>::max()){
                    d.message="shared grid removal identity/revision differs";return;}
                if(g::removeLiveZoneMembership50EN(s->lifecycle,s->world,row.membership.state,
                    s->membershipWork).status!=g::LiveZoneStatus50EN::Complete){
                    d.message="actual shared grid removal incomplete; prefix retained";return;}
                //Only this actual removed row's native descriptor is retired.
                //Prior successful removals remain committed on later failure.
                if(object.identity){object={};s->source[row.reservedRef-1u]={};++row.effectRevision;}
            }
            complete=true;
        });
        if(complete)gridRetirementRequired_[slot]=false;
        return complete;}catch(...){d.message="shared grid retirement exception; real prefix retained";return false;}
    }
