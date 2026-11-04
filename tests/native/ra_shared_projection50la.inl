//Synchronous native representation work, not the source collector. No callback,
//registration drain, scene update, Bank visit, readiness token or new World.
    bool projectSharedRegistered(EndCandidateResult& out)noexcept {
        auto* s=storage_.get();auto* p=endCandidateProducer_;const auto pin=bankLifetime_.lock();
        if(!s||!s->sharedRoster_||s->sharedRoster_->retired||!p||!p->inCall||p->activeRevision||
           !p->activeSession||p->storage!=s||p->owner.lock().get()!=this||
           p->serial==std::numeric_limits<std::uint64_t>::max()||
           !pin||!bankIdentity_||pin->identity!=bankIdentity_||
           terminal_||closed_||sharedOwnerLease_||sharedRoleViews_||sharedEffectsStopped_||
           nativeFifoPins_||triggerQueryActive_||s->queue.count||!actual_||!actual_->recovery||!actual_->movement)return false;
        const auto& session=*p->activeSession;auto& bank=session.actual_.bank;
        if(&bank!=bankIdentity_||bank.raWorldLifetime_.get()!=pin.get()||!bank.sealed_||bank.retired_||
           bank.visitDepth_!=1u||bank.currentSlot_!=p->activeSlot||(!bank.visiting_&&!bank.readBlocked_)||
           bank.count_!=s->sharedRoster_->count||p->activeSlot>=bank.count_||
           !camera_||camera_->session.get()!=&session||session.closed_||session.abandoned_||
           !session.coldBound_||!session.readsBound_||session.generation_!=camera_->generation||
           bank.sourceQueryPending())return false;
        auto* policy=s->nativeDetectorPolicy_.get();
        if(!policy||!policy->live_||!policy->registered_||policy->sharedWorld_!=s||policy->sharedRetired_||
           policy->sharedCount_!=bank.count_)return false;
        const auto* bindings=candidates_->staticBindings();const auto rows=bank.count_*storage::Storage::rosterRoles;
        if(!bindings||bindings->size()!=s->count.terrain||s->candidates.size()!=s->count.terrain||
           s->count.terrain+rows>s->count.objects||s->count.objects>s->objects.size()||
           s->count.objects>s->source.size()||s->collectionInput.objects!=s->objects.data()||
           rows>storage::Storage::maximumRoster*storage::Storage::rosterRoles)return false;
        //No projection revision is issued yet. These addresses are scoped to
        //the actual active producer/Bank above, not a claimed completed query.
        const auto read=[&](std::size_t slot,const void* canonical,void* destination,std::size_t bytes){
            if(slot>=bank.count_||!bank.slots_[slot].active)return false;
            const auto first=reinterpret_cast<std::uintptr_t>(canonical);
            for(const auto& span:bank.spans_){const auto begin=reinterpret_cast<std::uintptr_t>(span.address);
                if(first<begin||first-begin>span.bytes||bytes>span.bytes-(first-begin))continue;
                if(slot==p->activeSlot){std::memcpy(destination,canonical,bytes);return true;}
                const auto offset=span.offset+(first-begin);const auto& saved=bank.slots_[slot].bytes;
                if(offset>saved.size()||bytes>saved.size()-offset)return false;
                std::memcpy(destination,saved.data()+offset,bytes);return true;
            }
            return false;
        };
        std::array<g::CollectionObjectState50BX,storage::Storage::maximumRoster*storage::Storage::rosterRoles> prepared{};
        for(std::size_t ordinal=0;ordinal<rows;++ordinal){
            const auto& owned=s->sharedRoster_->objects[ordinal];if(!owned)return false;
            const auto& row=*owned;const auto slot=ordinal/storage::Storage::rosterRoles;
            const auto role=ordinal%storage::Storage::rosterRoles;const auto& pod=stagedPods694_[slot];
            if(row.slot!=slot||static_cast<std::size_t>(row.role)!=role||!row.coldBound||!row.coldFilter||
               !row.canonicalFilter||!row.identity||!rosterSlotComplete_[slot]||!freshComplete_[slot]||
               row.generation!=session.generation_||pod.generation!=row.generation||
               !pod.bodyA0||!pod.bodyA0->entity().reference00||!session.racerEntities_[slot].prepared()||
               session.racerEntities_[slot].body340!=pod.bodyA0||row.collision!=pod.collision||
               row.reservedRef!=s->count.terrain+ordinal+1u||
               row.effectRevision==std::numeric_limits<std::uint64_t>::max())return false;
            const auto& member=row.membership.state;const auto& object=s->objects[row.reservedRef-1u];
            const auto tag=role<3?2u:role==3?0u:3u;
            if(!member.attached||member.object!=row.reservedRef||member.lastShapeTag!=tag||
               member.lastType2Resource!=(role<3?row.type2Resource:nullptr)||
               object.identity!=row.identity||object.group||!object.hasShape||object.shapeTag!=tag||
               object.typeTag!=actual_->main.collisionTypeTag||!sameSharedFilterView(object.filter,*row.coldFilter))return false;
            std::uint32_t identity=0;std::uint8_t flags=0;
            if(!read(slot,&row.canonicalFilter->identity,&identity,sizeof(identity))||identity!=row.identity||
               !read(slot,&row.canonicalFilter->flagsF8,&flags,sizeof(flags)))return false;
            if(role<2){
                const auto& query=role==0?actual_->main:actual_->secondary;const g::Type2ResourceView49X* resource=nullptr;
                if(!read(slot,&query.resource,&resource,sizeof(resource))||resource!=row.type2Resource||
                   resource!=(role==0?&row.collision->main():&row.collision->secondary())||
                   policy->sharedMemberships_[slot][role].owner!=&row.membership.state||
                   policy->sharedMemberships_[slot][role].ref!=row.reservedRef)return false;
            }else if(role==2){
                g::TerrainPrepareState48M state;live_terrain_probe50fr::Owner live;
                if(!read(slot,&actual_->terrainLive,&live,sizeof(live))||!sharedTerrainBankViewMatches(row,live)||
                   !row.terrainResource||row.type2Resource!=&row.terrainResource->resource||
                   !read(slot,&actual_->terrain.initial,&state,sizeof(state))||
                   !sharedTerrainMatches(row,state))return false;
            }else if(role==3){
                g::PodShapeState49W shape;
                if(row.bodyA0!=pod.bodyA0||row.type0Resource!=pod.shape||
                   !read(slot,&actual_->frame.shape,&shape,sizeof(shape))||shape.type88||
                   shape.registrationE0!=row.reservedRef||
                   std::memcmp(&shape.composed00,&member.lastShapeWorld,sizeof(shape.composed00)))return false;
            }
            //Accepted membership cache is the real outer219AF0/219B14 input.
            //Retain query-written epoch, member translation and scale fields.
            prepared[ordinal]=s->source[row.reservedRef-1u];prepared[ordinal].flagsF8=flags;
            prepared[ordinal].cachedCenter=member.bounds.cachedCenter;
            prepared[ordinal].cachedRadius=member.bounds.cachedRadius;
            for(std::size_t previous=0;previous<row.reservedRef-1u;++previous)
                if(s->objects[previous].identity==row.identity)return false;
        }
        for(std::size_t i=0;i<s->count.terrain;++i){const auto* candidate=(*bindings)[i].candidate;
            if(!candidate||s->candidates[i]!=candidate||
               s->objects[i].identity!=candidate->candidate.collision.filter.identity)return false;
            const auto& c=candidate->candidate.collision;
            if(!g::projectCollectionObject50CF(c.filter,c.sphere,c.typeTag,candidate->shapeType,0).valid)return false;
            for(std::size_t j=0;j<i;++j)if(s->objects[j].identity==s->objects[i].identity)return false;
        }
        auto& main=*s->sharedRoster_->objects[p->activeSlot*storage::Storage::rosterRoles];
        if(main.type2Resource!=actual_->main.resource||!main.membership.storage().links||
           !main.membership.storage().capacity)return false;
        //All live/saved owner preflight is complete. No gameplay callback or
        //allocation occurs between these writes and the real publication.
        for(std::size_t i=0;i<s->count.terrain;++i){const auto& c=s->candidates[i]->candidate.collision;
            const auto projection=g::projectCollectionObject50CF(c.filter,c.sphere,c.typeTag,s->candidates[i]->shapeType,0);
            //Keep actual existing group/reference/position/scale metadata;
            //projection is not a constructor or a group-table reset.
            auto& object=s->objects[i];object.identity=projection.object.identity;
            object.filter=projection.object.filter;object.typeTag=projection.object.typeTag;
            object.shapeTag=projection.object.shapeTag;object.hasShape=projection.object.hasShape;
            s->source[i].flagsF8=projection.state.flagsF8;s->source[i].cachedCenter=projection.state.cachedCenter;
            s->source[i].cachedRadius=projection.state.cachedRadius;
        }
        for(std::size_t ordinal=0;ordinal<rows;++ordinal){auto& row=*s->sharedRoster_->objects[ordinal];
            s->source[row.reservedRef-1u]=prepared[ordinal];++row.effectRevision;
        }
        auto& input=s->collectionInput;
        //Publish ONLY the genuinely owned contiguous static+roster prefix.
        //Trailing camera/wreck reservations remain outside the extent until
        //their own real constructors/consumers join; never fill fake holes.
        input.objectCount=s->count.terrain+rows;
        input.query=main.reservedRef;input.zoneLinks=main.membership.storage().links;
        input.zoneLinkCount=main.membership.storage().capacity;input.zoneHead=1;
        input.bounds.shapeTag=2;input.bounds.shapeWorld=main.membership.state.lastShapeWorld;
        input.bounds.cachedCenter=main.membership.state.bounds.cachedCenter;
        input.bounds.cachedRadius=main.membership.state.bounds.cachedRadius;input.type2Resource=main.type2Resource;
        input.directTypeTag=actual_->main.collisionTypeTag;input.groupTypeTag=actual_->main.groupTypeTag;
        input.initialCurrentIdentity=actual_->main.initialQueryCurrentIdentity;
        if(!p->projected()){sharedEffectsStopped_=true;return false;}
        out.projectionRevision=p->activeRevision;return true;
    }
