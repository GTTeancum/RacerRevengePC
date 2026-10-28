//Private shared native cold continuation; actual visibility owner returns
//before these collision constructors. Never an undertrack/reset phase alias.
    undertrack_owners50ew_fixture::BorrowedQueries sharedFreshBorrows(
        const std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles>& rows)noexcept {
        auto& q=*actual_;auto& a=*freshRecovery_;
        undertrack_owners50ew_fixture::BorrowedQueries out{};
        out.main690=&q.main;out.mainMembership=&rows[0]->membership.state;
        out.mainStorage=rows[0]->membership.storage();out.mainRef=rows[0]->reservedRef;
        out.movement7A4=q.movement;out.movementEndpoints=a.endpoints;
        out.movementMetadata=a.metadata;out.movementSource=&a.source;
        out.terrain348=&q.terrain.initial;out.terrainResource=rows[2]->type2Resource;
        out.terrainMetadata=&q.terrain.metadata;out.terrainMembership=&rows[2]->membership.state;
        out.terrainStorage=rows[2]->membership.storage();out.terrainRef=rows[2]->reservedRef;
        return out;
    }
    bool publishSharedFreshRole(storage::Storage::SharedRosterObject& row)noexcept {
        auto& s=*storage_.get();auto& q=*actual_;auto& recovery=*q.recovery;
        const bool pod=row.role==storage::Storage::RosterRole::pod694;
        if(!pod&&row.role!=storage::Storage::RosterRole::movement7A4)return false;
        const auto& member=row.membership.state;
        if(!row.coldBound||!row.coldFilter||!row.canonicalFilter||
           !row.coldFilter->matches(*row.canonicalFilter)||!row.reservedRef||row.reservedRef>s.count.objects||
           !member.attached||member.object!=row.reservedRef||member.lastShapeTag!=(pod?0u:3u)||
           member.lastType2Resource||s.objects[row.reservedRef-1u].identity||
           row.effectRevision==std::numeric_limits<std::uint64_t>::max())return false;
        if(pod&&(q.frame.shape.registrationE0!=row.reservedRef||q.frame.shape.type88||
           row.type0Resource!=&recovery.selectedSource->mainTarget().source))return false;
        if(!pod&&(recovery.movementTag!=3u||q.movement->pairCount!=1u))return false;
        g::CollisionSphere48J sphere{};std::uint32_t w=0;
        if(pod){std::memcpy(sphere.center.v,q.frame.shape.center60.v,12);sphere.radius=q.frame.shape.radius80;
            w=std::bit_cast<std::uint32_t>(q.frame.shape.center60.v[3]);}
        else {sphere=recovery.movementSphere;w=std::bit_cast<std::uint32_t>(member.bounds.cachedCenter.v[3]);}
        if(std::memcmp(sphere.center.v,member.bounds.cachedCenter.v,12)||
           std::memcmp(&sphere.radius,&member.bounds.cachedRadius,sizeof(float)))return false;
        auto projected=g::projectCollectionObject50CF(*row.canonicalFilter,sphere,q.main.collisionTypeTag,pod?0u:3u,w);
        if(!projected.valid||projected.object.identity!=row.identity)return false;
        projected.object.filter=row.coldFilter->view;
        s.objects[row.reservedRef-1u]=projected.object;s.source[row.reservedRef-1u]=projected.state;++row.effectRevision;
        //shape0Ref/dynamic E4 consumers are NOT inferred from this native
        //constructor. Collector extent remains withheld until those real joins.
        return true;
    }
    bool attachSharedFreshQueries(
        const std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles>& rows,
        std::size_t slot,Diagnostic& d)noexcept {
        auto& q=*actual_;auto& a=*gridArena_;auto& s=*storage_.get();auto& r=*q.recovery;
        const auto borrowed=sharedFreshBorrows(rows);
        const auto queries=r.bindingSharedMembers(borrowed,{rows[3]->reservedRef,rows[4]->reservedRef},
            {&rows[3]->membership.state,&rows[4]->membership.state},
            {rows[3]->membership.storage(),rows[4]->membership.storage()});
        for(const auto& query:queries.query)if(!query.membership||!query.object)
            return fail(d,"actual shared fresh query owner binding rejected");
        const bool attached=undertrack_owners50ew_fixture::attachFreshQueriesBound(r,q.frame,a.bindings,a.parameters,
            borrowed,queries,s.lifecycle,s.world,s.membershipWork,1,
            slot<a.humanCount?undertrack_owners50ew_fixture::ColdQueryRole50LH::humanPlayer:
                undertrack_owners50ew_fixture::ColdQueryRole50LH::originalAi);
        //The real source body commits7A4 before694. Publish its actual partial
        //owner effects even if the later POD constructor/attachment rejects.
        if(rows[4]->membership.state.attached&&!publishSharedFreshRole(*rows[4]))
            return fail(d,"actual shared7A4 constructor publication incomplete");
        if(rows[3]->membership.state.attached&&!publishSharedFreshRole(*rows[3]))
            return fail(d,"actual shared694 constructor publication incomplete");
        if(!attached)return fail(d,"actual shared fresh collision prefix incomplete; retain successful owners");
        return true;
    }
    bool retireSharedFresh(native_racer50HN::Bank& bank,std::size_t slot,Diagnostic& d)noexcept {
        d={};auto* s=storage_.get();
        if(!s||!s->sharedRoster_||closed_||sharedOwnerLease_||sharedRoleViews_||sharedEffectsStopped_||
           nativeFifoPins_||triggerQueryActive_||(endCandidateProducer_&&endCandidateProducer_->inCall)||
           !cameraBankCurrent()||&bank!=bankIdentity_||slot>=s->sharedRoster_->count||
           (camera_&&camera_->session&&!camera_->session->closed_)||
           (follow_&&follow_->attempted_&&!follow_->closed_)){
            d.message="shared fresh retirement requires actual quiescent camera/Follow/Bank owners";return false;}
        terminal_=true;for(auto& record:gridCompletion_)record.complete=false;freshComplete_.fill(false);
        if(!freshRetirementRequired_[slot])return true;
        bool complete=false;
        try {bank.visit(slot,[&](){
            std::array<storage::Storage::SharedRosterObject*,storage::Storage::rosterRoles> rows{};
            if(!sharedColdRowsCurrent(slot,rows)){d.message="actual shared fresh retirement ownership differs";return;}
            for(unsigned i:{3u,4u}){auto& row=*rows[i];auto& object=s->objects[row.reservedRef-1u];
                if((object.identity&&object.identity!=row.identity)||
                   (row.membership.state.attached&&row.membership.state.object!=row.reservedRef)||
                   (i==3u&&actual_->frame.shape.registrationE0&&actual_->frame.shape.registrationE0!=row.reservedRef)||
                   row.effectRevision==std::numeric_limits<std::uint64_t>::max()){
                    d.message="shared fresh removal identity/revision differs";return;}
                if(g::removeLiveZoneMembership50EN(s->lifecycle,s->world,row.membership.state,
                    s->membershipWork).status!=g::LiveZoneStatus50EN::Complete){
                    d.message="actual shared fresh collision removal incomplete";return;}
                if(i==3u)actual_->frame.shape.registrationE0=0; //Only after real removal.
                if(object.identity){object={};s->source[row.reservedRef-1u]={};++row.effectRevision;}
            }
            g::original_gc_gameplay_lighting_candidate::Diagnostic ld{};
            if(!freshLighting_||!freshLighting_->retireRaFreshVehicle(static_cast<std::uint32_t>(slot),ld)){
                d.message="actual shared fresh visibility removal incomplete";return;}
            complete=true;
        });
        if(complete)freshRetirementRequired_[slot]=false;
        return complete;}catch(...){d.message="shared fresh retirement exception; actual partial effects retained";return false;}
    }
