//Private source-query borrower. Included inside OwnershipBridge; no registry
//addition, projection, slot visit, native extent or shared factory activation.
    struct SharedDirectPodView {
        storage::Storage::SharedRosterObject* row=nullptr;
        const NativePod694Owner* owner=nullptr;
        g::Type2StaticCandidate49X geometry; //Empty vectors are NOT list owners.
        g::Type2BorrowedTargetFilter50CC filter{};
        g::TerrainCandidateEffects48J effect{};
        std::uint64_t revision=0;
        unsigned char* flags=nullptr;unsigned char* copied40=nullptr;unsigned char* copied50=nullptr;
    };
    bool borrowSharedDirectPod(SharedOwnerLease& lease,std::size_t targetSlot,
        SharedDirectPodView& out)noexcept {
        if(!sharedLeaseCurrent(lease)||out.row||!actual_||!actual_->recovery)return false;
        auto& s=*storage_.get();const auto& session=*lease.producer_->activeSession;
        if(targetSlot>=s.sharedRoster_->count||targetSlot>=session.racerEntities_.size())return false;
        auto& owned=s.sharedRoster_->objects[targetSlot*storage::Storage::rosterRoles+
            static_cast<std::size_t>(storage::Storage::RosterRole::pod694)];
        if(!owned)return false;
        auto& row=*owned;const auto& owner=stagedPods694_[targetSlot];
        const auto& racer=session.racerEntities_[targetSlot];const auto& member=row.membership.state;
        if(row.slot!=targetSlot||!row.coldBound||!row.coldFilter||!row.identity||
           row.canonicalFilter!=&actual_->recovery->podFilter||row.bodyA0!=owner.bodyA0||
           !rosterSlotComplete_[targetSlot]||!owner.bodyA0||!owner.collision||
           row.collision!=owner.collision||owner.collision!=inputs_.collisionRoster[targetSlot]||
           row.type0Resource!=owner.shape||owner.shape!=&owner.collision->mainTarget().source||
           owner.responseE8!=NativePodResponseE8::constructorNull11B350||
           row.generation!=session.generation_||owner.generation!=row.generation||
           !racer.prepared()||racer.body340!=owner.bodyA0||racer.scene190!=owner.bodyA0->scene288()||
           !racer.scene190||!racer.scene190->constructed||racer.scene190->released||
           !owner.bodyA0->entity().reference00||owner.bodyA0->entity().sessionGeneration!=row.generation||
           owner.bodyA0->canonicalNumericAddress()!=&actual_->frame.bodies.main||
           !member.attached||member.object!=row.reservedRef||member.lastShapeTag!=0||member.lastType2Resource||
           !row.reservedRef||row.reservedRef>s.collectionInput.objectCount||row.reservedRef>s.count.objects||
           row.effectRevision>std::numeric_limits<std::uint64_t>::max()-2u)return false;
        const auto index=row.reservedRef-1u;const auto& compact=s.objects[index];
        if(compact.identity!=row.identity||compact.group||!compact.hasShape||compact.shapeTag!=0||
           compact.typeTag!=actual_->main.collisionTypeTag||
           !sameSharedFilterView(compact.filter,*row.coldFilter))return false;
        auto& q=*actual_;auto& recovery=*q.recovery;
        auto* identity=sharedSlotSpan(lease,targetSlot,&recovery.podFilter.identity,sizeof(row.identity));
        auto* shape=sharedSlotSpan(lease,targetSlot,&q.frame.shape,sizeof(q.frame.shape));
        auto* flags=sharedSlotSpan(lease,targetSlot,&recovery.podFilter.flagsF8,1);
        auto* copy40=sharedSlotSpan(lease,targetSlot,&recovery.podCopied40,sizeof(recovery.podCopied40));
        auto* copy50=sharedSlotSpan(lease,targetSlot,&recovery.podCopied50,sizeof(recovery.podCopied50));
        if(!identity||!shape||!flags||!copy40||!copy50)return false;
        std::uint32_t currentIdentity=0;std::memcpy(&currentIdentity,identity,sizeof(currentIdentity));
        g::PodShapeState49W currentShape;std::memcpy(&currentShape,shape,sizeof(currentShape));
        if(currentIdentity!=row.identity||currentShape.registrationE0!=row.reservedRef||currentShape.type88||
           std::memcmp(&currentShape.composed00,&member.lastShapeWorld,sizeof(member.lastShapeWorld))||
           std::memcmp(&currentShape.center60,&member.bounds.cachedCenter,sizeof(currentShape.center60))||
           std::memcmp(&currentShape.radius80,&member.bounds.cachedRadius,sizeof(float))||
           std::memcmp(&currentShape.center60,&s.source[index].cachedCenter,sizeof(currentShape.center60))||
           std::memcmp(&currentShape.radius80,&s.source[index].cachedRadius,sizeof(float))||
           *flags!=s.source[index].flagsF8)return false;
        auto& carrier=out.geometry;auto& c=carrier.candidate.collision;
        const auto& target=owner.collision->mainTarget();
        c.geometry.mesh=&target.mesh;c.geometry.worldMatrix=member.lastShapeWorld;c.geometry.id=row.identity;
        carrier.nodeRadii=target.radii.data();carrier.nodeRadiusCount=target.radii.size();carrier.maxDepth=target.maxDepth;
        c.filter.identity=row.identity;c.filter.flagsF8=*flags;
        c.filter.categoryA4=row.coldFilter->view.categoryA4;
        c.filter.acceptMaskB8=row.coldFilter->view.acceptMaskB8;c.filter.flags31=row.coldFilter->view.flags31;
        std::memcpy(c.sphere.center.v,currentShape.center60.v,sizeof(c.sphere.center.v));c.sphere.radius=currentShape.radius80;
        c.typeTag=compact.typeTag;c.hasResponseCallbackE8=false; //Actual retained constructorNull above.
        std::memcpy(c.copied40.data(),copy40,sizeof(c.copied40));std::memcpy(c.copied50.data(),copy50,sizeof(c.copied50));
        out.effect.identity=row.identity;out.effect.flagsF8=*flags;
        out.effect.copied40=c.copied40;out.effect.copied50=c.copied50;
        out.filter={&row.coldFilter->view,row.identity,*flags};out.row=&row;out.owner=&owner;
        out.revision=row.effectRevision;out.flags=flags;out.copied40=copy40;out.copied50=copy50;
        return true;
    }
    bool publishSharedDirectPod(SharedOwnerLease& lease,SharedDirectPodView& view,
        const g::TerrainCandidateEffects48J& effect)noexcept {
        if(!sharedLeaseCurrent(lease)||!view.row||view.row->effectRevision!=view.revision||
           view.revision==std::numeric_limits<std::uint64_t>::max()||effect.identity!=view.row->identity)return false;
        std::memcpy(view.flags,&effect.flagsF8,1);
        std::memcpy(view.copied40,effect.copied40.data(),sizeof(effect.copied40));
        std::memcpy(view.copied50,effect.copied50.data(),sizeof(effect.copied50));
        storage_.get()->source[view.row->reservedRef-1u].flagsF8=effect.flagsF8;
        ++view.row->effectRevision;view.revision=view.row->effectRevision;lease.wrote_=true;
        view.effect=effect;view.filter.flagsF8=effect.flagsF8;
        auto& c=view.geometry.candidate.collision;c.filter.flagsF8=effect.flagsF8;
        c.copied40=effect.copied40;c.copied50=effect.copied50;return true;
    }
    bool markSharedDirectQueryEffect(SharedOwnerLease& lease)noexcept {
        if(!sharedLeaseCurrent(lease))return false;lease.wrote_=true;return true;
    }
    bool publishSharedActiveQueryFlags(SharedOwnerLease& lease,bool secondary)noexcept {
        if(!sharedLeaseCurrent(lease))return false;
        const auto lane=secondary?1u:0u;auto& s=*storage_.get();
        auto& row=s.sharedRoster_->objects[lease.activeSlot_*storage::Storage::rosterRoles+lane];
        const auto& filter=secondary?actual_->secondaryFilter:actual_->mainFilter;
        if(!row||!row->coldBound||row->canonicalFilter!=&filter||row->identity!=filter.identity||
           !row->reservedRef||row->reservedRef>s.collectionInput.objectCount||
           s.objects[row->reservedRef-1u].identity!=row->identity||
           row->effectRevision==std::numeric_limits<std::uint64_t>::max())return false;
        s.source[row->reservedRef-1u].flagsF8=filter.flagsF8;
        ++row->effectRevision;lease.wrote_=true;return true;
    }
    bool publishSharedActiveQueryFlags(bool secondary,const EndCandidateResult& out)noexcept {
        SharedOwnerLease lease;if(!beginSharedOwnerLease(lease,out))return false;
        if(!publishSharedActiveQueryFlags(lease,secondary))return false;
        lease.completed_=true;return true;
    }
    bool completeSharedOwnerLease(SharedOwnerLease& lease)noexcept {
        if(!sharedLeaseCurrent(lease))return false;lease.completed_=true;return true;
    }
