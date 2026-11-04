//Actual shape2->shape2/3 no-hit service. No geometry/result/callback owner is
//invented:121410's original target table388AF0 returns0 at1218F0 for2/3/4.
//There is no represented shape4 roster producer here, so none is admitted.
    bool processSharedTerrainNoHitRole(SharedTerrainInvocation& call,
        const SharedSelectedOwner& selected)noexcept {
        auto& out=call.result_;auto& lease=call.lease_;auto* s=storage_.get();
        out.stoppedPC=0x1226b0;
        if(!sharedTerrainLimitedCurrent(call)||selected.kind!=SharedSelectedKind::otherRoster||
           !actual_||!actual_->recovery||!actual_->movement||!s||!s->sharedRoster_||
           selected.index>=s->sharedRoster_->count*storage::Storage::rosterRoles||
           call.ordinal_>=sharedProbeSelection_.count||
           &selected!=&sharedProbeSelection_.entries[call.ordinal_])return false;
        const auto slot=selected.index/storage::Storage::rosterRoles;
        const auto role=selected.index%storage::Storage::rosterRoles;
        if(role==3u||role>=storage::Storage::rosterRoles)return false;
        const auto& owned=s->sharedRoster_->objects[selected.index];if(!owned)return false;
        auto& row=*owned;const auto& member=row.membership.state;
        const auto& session=*lease.producer_->activeSession;
        if(slot>=session.racerEntities_.size()||slot>=stagedPods694_.size())return false;
        const auto& racer=session.racerEntities_[slot];
        const auto& body=stagedPods694_[slot];
        g::CollisionFilter48J* expected[]={&actual_->mainFilter,&actual_->secondaryFilter,
            &actual_->terrain.metadata.filter,&actual_->recovery->podFilter,&actual_->movement->filter};
        const auto shape=role<3u?2u:3u;
        if(row.slot!=slot||static_cast<std::size_t>(row.role)!=role||!row.coldBound||!row.coldFilter||
           row.identity!=selected.identity||row.canonicalFilter!=expected[role]||
           row.generation!=session.generation_||!rosterSlotComplete_[slot]||!freshComplete_[slot]||
           !racer.prepared()||!body.bodyA0||racer.body340!=body.bodyA0||
           body.generation!=row.generation||row.collision!=body.collision||!row.collision||
           !body.bodyA0->entity().reference00||body.bodyA0->entity().sessionGeneration!=row.generation||
           !member.attached||member.object!=row.reservedRef||member.lastShapeTag!=shape||
           member.lastType2Resource!=(role<3u?row.type2Resource:nullptr)||
           row.reservedRef!=s->count.terrain+selected.index+1u||row.reservedRef>s->count.objects||
           row.effectRevision==std::numeric_limits<std::uint64_t>::max())return false;
        const auto index=row.reservedRef-1u;const auto& object=s->objects[index];
        if(object.identity!=row.identity||object.group||!object.hasShape||object.shapeTag!=shape||
           object.typeTag!=actual_->main.collisionTypeTag||
           !sameSharedFilterView(object.filter,*row.coldFilter))return false;
        const auto read=[&](void* canonical,void* value,std::size_t bytes){
            auto* address=sharedSlotSpan(lease,slot,canonical,bytes);
            if(!address)return false;std::memcpy(value,address,bytes);return true;
        };
        //Only real saved/live owner lanes are read; the active racer's mutable
        //filter/cache is not used as another slot's state.
        std::uint32_t identity=0;
        auto* flags=sharedSlotSpan(lease,slot,&row.canonicalFilter->flagsF8,1);
        if(!flags||!read(&row.canonicalFilter->identity,&identity,sizeof(identity))||identity!=row.identity||
           *flags!=s->source[index].flagsF8)return false;
        g::CollisionSphere48J sphere{};
        if(role<2u){
            auto& query=role==0u?actual_->main:actual_->secondary;
            const g::Type2ResourceView49X* resource=nullptr;
            if(!read(&query.resource,&resource,sizeof(resource))||resource!=row.type2Resource||
               resource!=(role==0u?&row.collision->main():&row.collision->secondary())||
               !read(&query.sphere,&sphere,sizeof(sphere)))return false;
        }else if(role==2u){
            g::TerrainPrepareState48M state;live_terrain_probe50fr::Owner live;
            if(!read(&actual_->terrainLive,&live,sizeof(live))||!sharedTerrainBankViewMatches(row,live)||
               !row.terrainResource||!row.terrainResource->initialized||
               row.type2Resource!=&row.terrainResource->resource||
               !read(&actual_->terrain.initial,&state,sizeof(state))||
               !sharedTerrainMatches(row,state))return false;
            sphere=state.sphere;
        }else{
            std::uint32_t actualTag=0,pairs=0;
            if(!read(&actual_->recovery->movementTag,&actualTag,sizeof(actualTag))||actualTag!=3u||
               !read(&actual_->movement->pairCount,&pairs,sizeof(pairs))||pairs!=1u||
               !read(&actual_->recovery->movementSphere,&sphere,sizeof(sphere)))return false;
        }
        if(std::memcmp(sphere.center.v,member.bounds.cachedCenter.v,sizeof(sphere.center.v))||
           std::memcmp(&sphere.radius,&member.bounds.cachedRadius,sizeof(float))||
           std::memcmp(&member.bounds.cachedCenter,&s->source[index].cachedCenter,sizeof(member.bounds.cachedCenter))||
           std::memcmp(&sphere.radius,&s->source[index].cachedRadius,sizeof(float)))return false;
        auto& query=actual_->terrain.metadata;
        if(object.typeTag!=query.collisionTypeTag){
            if(!(query.filter.flagsF8&1u)&&object.typeTag==query.groupTypeTag){out.stoppedPC=0x1228b4;return false;}
            return true;
        }
        if(!markSharedDirectQueryEffect(lease))return false;
        out.stoppedPC=0x1226cc;actual_->frame.terrainProbe.currentCandidateIdentity=row.identity;
        out.stoppedPC=0x1226e8;*flags=static_cast<std::uint8_t>(*flags|0x10u);
        s->source[index].flagsF8=*flags;++row.effectRevision;
        out.stoppedPC=0x126eb0;bool overlaps=false;
        if(!g::evaluateType2CandidateSphere50CC(actual_->terrain.initial.sphere,sphere,overlaps))return false;
        if(!overlaps)return true;
        out.stoppedPC=0x120e70;
        const g::Type2BorrowedTargetFilter50CC filter{&row.coldFilter->view,row.identity,*flags};
        bool passes=false;if(!g::passType2BorrowedCandidateFilter50CC(query.filter,filter,passes))return false;
        if(!passes)return true;
        out.stoppedPC=0x121434;
        if(!actual_->normalMode.acceptReachedSourceDispatch50MB(query.normalMode54,query.normalMode54Bound))return false;
        //Actual typed target2/3 dispatch, NOT a blanket unsupported-as-miss.
        //Source does not read geometry/result90/copies/E4/E8/owner9C here.
        out.stoppedPC=0x1218f0;return true;
    }
