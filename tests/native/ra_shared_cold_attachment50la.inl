//Private fresh shared-mode producer/retirement adaptation, never a live reset.
    bool sharedPodContextOwned(const g::PodLiveQueryContext50IZ* context)const noexcept {
        const auto* s=storage_.get();if(!context||!s||!s->sharedRoster_||!podParameters_)return false;
        for(std::size_t slot=0;slot<s->sharedRoster_->count;++slot){
            //Stage S-2: rows only through the one storage accessor.
            const auto* row=const_cast<storage::Storage*>(s)->row(slot,storage::Storage::RosterRole::pod694).row;
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
        for(std::size_t i=0;i<rows.size();++i){
            auto* row=const_cast<storage::Storage*>(s)->row(slot,static_cast<storage::Storage::RosterRole>(i)).row;
            if(!row||row->slot!=slot||static_cast<std::size_t>(row->role)!=i||!row->coldBound||
               row->collision!=ids.collision||row->generation!=staged.generation||!row->coldFilter||
               row->canonicalFilter!=filters[i]||row->identity!=filters[i]->identity||
               !row->coldFilter->matches(*filters[i])||!row->reservedRef||row->reservedRef>s->count.objects)return false;
            rows[i]=row;
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
        (void)bytes;(void)after;(void)noise;
        if(!frameLanesRegistered(actual_->frame)||
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
            //Stage S-2: the row producer lives in Storage (one layout owner).
            const auto* context=s->bindRowPodContext(slot,parameters.shape);
            if(!context){fail(d,"shared POD row context producer refused");return;}
            podBindings_=&bindings;podParameters_=&parameters;podHumanCount_=humanCount;
            bindings.podLiveQuery50IZ=context;podBindingInstalled_[slot]=true;complete=true;
        });return complete;}catch(...){return fail(d,"shared POD context binding exception; real saved prefix retained");}
    }
    bool publishSharedColdGridRole(storage::Storage::SharedRosterObject& row,
        const g::LiveZoneBounds50EN& bounds)noexcept {
        //Stage S-2: the grid publication producer lives in Storage. No extent,
        //head or query activation: 694/7A4 and the other roster roles still
        //require their actual producers before collector admission.
        auto* s=storage_.get();
        return s&&actual_&&s->publishRowGridRole(row.slot,row.role,bounds,actual_->main.collisionTypeTag);
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
            //Stage S-2: the row retirement producer lives in Storage. Only each
            //actually removed row's descriptor is retired; earlier successful
            //removals stay committed on a later failure.
            if(const char* refused=s->retireRowGridRoles(slot)){d.message=refused;return;}
            complete=true;
        });
        if(complete)gridRetirementRequired_[slot]=false;
        return complete;}catch(...){d.message="shared grid retirement exception; real prefix retained";return false;}
    }
