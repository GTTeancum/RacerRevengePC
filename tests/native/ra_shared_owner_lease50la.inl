// Included inside OwnershipBridge's private class body. Native custody only;
// no public mode switch, original return, scene callback or Bank visit.
    bool registerSharedWorldOwners(native_racer50HN::Bank& bank,
        const std::shared_ptr<OwnershipBridge>& self,Diagnostic& d)noexcept {
        d={};auto* s=storage_.get();
        if(!s||!s->sharedRoster_||s->sharedRoster_->retired||terminal_||closed_||
           !actual_||!bindAttempted_||follow_||queriesConstructed_||registrationAttempted_||
           self.get()!=this||bankIdentity_||!bankLifetime_.expired()||bank.raWorldLifetime_||
           bank.retired_||bank.sealed_||bank.visiting_||bank.readBlocked_||
           !actual_->recovery||!actual_->movement)
            return fail(d,"shared registration requires actual fresh typed arena and idle unsealed Bank");
        const auto absent=[&](const void* address,std::size_t bytes){
            if(!bytes)return true;
            const auto first=reinterpret_cast<std::uintptr_t>(address);
            if(!address||bytes>std::numeric_limits<std::uintptr_t>::max()-first)return false;
            for(const auto& span:bank.spans_){const auto begin=reinterpret_cast<std::uintptr_t>(span.address);
                if(first>=begin?first-begin<span.bytes:begin-first<bytes)return false;}
            return true;
        };
        const auto present=[&](const void* address,std::size_t bytes){
            const auto first=reinterpret_cast<std::uintptr_t>(address);
            for(const auto& span:bank.spans_){const auto begin=reinterpret_cast<std::uintptr_t>(span.address);
                if(first>=begin&&first-begin<=span.bytes&&bytes<=span.bytes-(first-begin))return true;}
            return false;
        };
        //Reject mixed legacy snapshots BEFORE installing a pin or any new span.
        if(!s->sharedSpansAbsent(absent))return fail(d,"shared World topology already occurs in racer Bank");
        for(const auto& binding:*candidates_->staticBindings()){
            const auto& c=binding.candidate->candidate.collision;
            if(!absent(&c.filter.flagsF8,sizeof(c.filter.flagsF8))||
               !absent(&c.copied40,sizeof(c.copied40))||!absent(&c.copied50,sizeof(c.copied50)))
                return fail(d,"shared actual candidate effects were registered as racer snapshots");
        }
        auto& a=*actual_;auto& r=*a.recovery;
        for(unsigned i=0;i<2;++i)
            if(!absent(&r.memberships[i],sizeof(r.memberships[i]))||
               !absent(r.links[i].data(),r.links[i].size()*sizeof(r.links[i][0]))||
               !absent(r.entries[i].data(),r.entries[i].size()*sizeof(r.entries[i][0])))
                return fail(d,"legacy recovery memberships cannot enter shared World Bank");
        if(!present(&a.main,sizeof(a.main))||!present(&a.secondary,sizeof(a.secondary))||
           !present(&a.mainFilter.identity,sizeof(a.mainFilter.identity))||
           !present(&a.secondaryFilter.identity,sizeof(a.secondaryFilter.identity))||
           !present(&a.mainFilter.flagsF8,sizeof(a.mainFilter.flagsF8))||
           !present(&a.secondaryFilter.flagsF8,sizeof(a.secondaryFilter.flagsF8))||
           !present(&a.terrain.initial,sizeof(a.terrain.initial))||!present(&a.terrainLive,sizeof(a.terrainLive))||
           !present(&a.terrain.metadata.filter.flagsF8,sizeof(a.terrain.metadata.filter.flagsF8))||
           !present(&a.movement->filter.flagsF8,sizeof(a.movement->filter.flagsF8))||
           !present(&a.movement->copied40,sizeof(a.movement->copied40))||
           !present(&a.movement->copied50,sizeof(a.movement->copied50))||
           !present(&a.frame.bodies.main,sizeof(a.frame.bodies.main))||
           !present(a.frame.mainContactPartners210,sizeof(a.frame.mainContactPartners210)))
            return fail(d,"actual canonical shared-query numeric owners are not Bank-registered");
        //Actual fresh policy constructor/registration cut. Bind immutable
        //per-row member/ref owners BEFORE any new policy/Bank spans escape.
        //No current-slot pointer rewrite or legacy admission is used later.
        auto* policy=s->nativeDetectorPolicy_.get();
        if(!policy||!policy->live_||policy->registered_||policy->sharedWorld_||policy->sharedRetired_||
           policy->frame_!=&a.frame||s->sharedRoster_->count>policy->sharedMemberships_.size()||
           policy->entries_[0].query!=&a.main||policy->entries_[1].query!=&a.secondary||
           policy->entries_[0].membership!=&s->mainMembership.state||
           policy->entries_[1].membership!=&s->secondaryMembership.state)
            return fail(d,"shared detector policy is not at its actual fresh registration cut");
        decltype(policy->sharedMemberships_) selected{};
        for(std::size_t slot=0;slot<s->sharedRoster_->count;++slot)for(unsigned role=0;role<2;++role){
            const auto& row=s->sharedRoster_->objects[slot*storage::Storage::rosterRoles+role];
            if(!row||row->slot!=slot||static_cast<unsigned>(row->role)!=role||
               !row->reservedRef||row->reservedRef>s->count.objects||row->coldBound||
               row->membership.state.attached||row->membership.state.object||
               !absent(&row->membership.state,sizeof(row->membership.state)))
                return fail(d,"shared detector row is not an actual fresh unbanked membership");
            selected[slot][role]={&row->membership.state,row->reservedRef};
        }
        registrationAttempted_=true;
        try {
            //Pin precedes ALL escaping new spans, including partial failure.
            bank.raWorldLifetime_=std::make_shared<BankPin>(bank,self);
            bankLifetime_=bank.raWorldLifetime_;bankIdentity_=&bank;
            if(!registerSharedAuxiliaryIdentitySpans(bank,a))
                return fail(d,"shared auxiliary identity registration incomplete; pin retained");
            r.registerSharedNumericOwners(bank);
            bank.owner(a.terrain.metadata.copied40);bank.owner(a.terrain.metadata.copied50);
            policy->sharedMemberships_=selected;policy->sharedCount_=s->sharedRoster_->count;
            policy->sharedWorld_=s;
            s->registerMutableOwners(bank); //Only lanes/numeric partition; immutable member views are NOT banked.
            worldOwnersRegistered_=true;return true;
        }catch(...){return fail(d,"shared numeric registration failed; actual pin and spans retained");}
    }

    class SharedOwnerLease {
        friend class OwnershipBridge;
        OwnershipBridge* issuer_=nullptr;
        native_racer50HN::Bank* bank_=nullptr;
        EndCandidateProducer* producer_=nullptr;
        std::uint64_t serial_=0,projection_=0;
        std::size_t activeSlot_=0;
        bool wrote_=false,completed_=false;
    public:
        SharedOwnerLease()=default;
        SharedOwnerLease(const SharedOwnerLease&)=delete;
        SharedOwnerLease& operator=(const SharedOwnerLease&)=delete;
        ~SharedOwnerLease(){if(issuer_)issuer_->endSharedOwnerLease(*this);}
    };
    SharedOwnerLease* sharedOwnerLease_=nullptr;
    std::uint64_t sharedOwnerLeaseSerial_=0;
    bool sharedEffectsStopped_=false;
    bool beginSharedOwnerLease(SharedOwnerLease& lease,const EndCandidateResult& out)noexcept {
        auto* s=storage_.get();auto* p=endCandidateProducer_;const auto pin=bankLifetime_.lock();
        if(lease.issuer_||sharedOwnerLease_||sharedEffectsStopped_||terminal_||closed_||
           !s||!s->sharedRoster_||s->sharedRoster_->retired||!actual_||!worldOwnersRegistered_||
           !p||!p->inCall||!p->activeSession||!p->activeRevision||
           p->activeRevision!=out.projectionRevision||p->storage!=s||p->owner.lock().get()!=this||
           !pin||!bankIdentity_||pin->identity!=bankIdentity_||
           sharedOwnerLeaseSerial_==std::numeric_limits<std::uint64_t>::max())return false;
        auto& bank=p->activeSession->actual_.bank;
        if(&bank!=bankIdentity_||bank.raWorldLifetime_.get()!=pin.get()||!bank.sealed_||bank.retired_||
           bank.visitDepth_!=1u||bank.currentSlot_!=p->activeSlot||(!bank.visiting_&&!bank.readBlocked_)||
           p->activeSlot>=bank.count_||bank.count_!=s->sharedRoster_->count||
           !camera_||camera_->session.get()!=p->activeSession||p->activeSession->closed_||
           p->activeSession->abandoned_||!p->activeSession->coldBound_||!p->activeSession->readsBound_||
           p->activeSession->generation_!=camera_->generation)return false;
        lease.issuer_=this;lease.bank_=&bank;lease.producer_=p;lease.serial_=++sharedOwnerLeaseSerial_;
        lease.projection_=p->activeRevision;lease.activeSlot_=p->activeSlot;
        sharedOwnerLease_=&lease;return true;
    }
    bool sharedLeaseCurrent(const SharedOwnerLease& lease)const noexcept {
        const auto pin=bankLifetime_.lock();
        return sharedOwnerLease_==&lease&&lease.issuer_==this&&lease.serial_==sharedOwnerLeaseSerial_&&
            !sharedEffectsStopped_&&!terminal_&&!closed_&&pin&&lease.bank_&&bankIdentity_==lease.bank_&&
            pin->identity==lease.bank_&&lease.bank_->raWorldLifetime_.get()==pin.get()&&
            !lease.bank_->retired_&&lease.bank_->visitDepth_==1u&&lease.bank_->currentSlot_==lease.activeSlot_&&
            lease.producer_==endCandidateProducer_&&lease.producer_&&lease.producer_->inCall&&
            lease.producer_->activeRevision==lease.projection_;
    }
    void endSharedOwnerLease(SharedOwnerLease& lease)noexcept {
        if(sharedOwnerLease_!=&lease||lease.issuer_!=this)std::terminate();
        if(lease.wrote_&&!lease.completed_)sharedEffectsStopped_=true;
        sharedOwnerLease_=nullptr;lease.issuer_=nullptr;lease.bank_=nullptr;lease.producer_=nullptr;
    }
    unsigned char* sharedSlotSpan(const SharedOwnerLease& lease,std::size_t slot,
        void* canonical,std::size_t bytes)const noexcept {
        if(!sharedLeaseCurrent(lease)||slot>=lease.bank_->count_||!lease.bank_->slots_[slot].active)return nullptr;
        const auto first=reinterpret_cast<std::uintptr_t>(canonical);
        for(const auto& span:lease.bank_->spans_){const auto begin=reinterpret_cast<std::uintptr_t>(span.address);
            if(first<begin||first-begin>span.bytes||bytes>span.bytes-(first-begin))continue;
            if(slot==lease.activeSlot_)return static_cast<unsigned char*>(canonical);
            const auto offset=span.offset+(first-begin);auto& saved=lease.bank_->slots_[slot].bytes;
            if(offset>saved.size()||bytes>saved.size()-offset)return nullptr;
            //No typed dereference: Bank offsets need not provide typed alignment.
            return saved.data()+offset;
        }
        return nullptr;
    }
    //Private UNCALLED collection-result publisher. The only accepted buffers
    //are the real same-producer first/second outputs; never arbitrary flags or
    //a latest-frame snapshot. Other static/camera/tree effects remain caller-
    //owned; this method claims roster F8 and its actual compact-state mirror only.
    bool publishSharedRosterFlags(bool second,EndCandidateResult& out)noexcept {
        if(!out.firstReturned||(second?!out.secondReturned:out.sharedFirstFlagsPublished)||
           (second&&(!out.sharedFirstFlagsPublished||out.sharedSecondFlagsPublished)))return false;
        SharedOwnerLease lease;if(!beginSharedOwnerLease(lease,out))return false;
        auto& s=*storage_.get();auto& session=*lease.producer_->activeSession;
        auto& accepted=second?s.second:s.first;
        struct Target {storage::Storage::SharedRosterObject* row;unsigned char* flags;std::size_t index;};
        std::array<Target,storage::Storage::maximumRoster*storage::Storage::rosterRoles> targets{};
        std::size_t count=0;
        const auto allocated=s.sharedRoster_->count*storage::Storage::rosterRoles;
        for(std::size_t ordinal=0;ordinal<allocated;++ordinal){
            const auto& owned=s.sharedRoster_->objects[ordinal];if(!owned)return false;
            auto& row=*owned;const auto slot=row.slot;
            if(slot!=ordinal/storage::Storage::rosterRoles||
               static_cast<std::size_t>(row.role)!=ordinal%storage::Storage::rosterRoles)return false;
            if(!row.coldBound||!row.canonicalFilter||slot>=session.racerEntities_.size()||
               !rosterSlotComplete_[slot]||row.generation!=session.generation_||
               row.effectRevision==std::numeric_limits<std::uint64_t>::max()||
               !row.reservedRef||row.reservedRef>s.collectionInput.objectCount||
               row.reservedRef>s.count.objects)return false;
            const auto& racer=session.racerEntities_[slot];const auto& pod=stagedPods694_[slot];
            if(!racer.prepared()||!pod.bodyA0||racer.body340!=pod.bodyA0||
               racer.racer.sessionGeneration!=row.generation||!pod.bodyA0->entity().reference00||
               pod.bodyA0->entity().sessionGeneration!=row.generation)return false;
            const auto index=row.reservedRef-1u;
            if(s.objects[index].identity!=row.identity||!row.identity||
               (row.membership.state.attached&&row.membership.state.object!=row.reservedRef))return false;
            auto* id=sharedSlotSpan(lease,slot,&row.canonicalFilter->identity,sizeof(row.identity));
            auto* flags=sharedSlotSpan(lease,slot,&row.canonicalFilter->flagsF8,sizeof(std::uint8_t));
            std::uint32_t actualIdentity=0;if(!id||!flags)return false;
            std::memcpy(&actualIdentity,id,sizeof(actualIdentity));if(actualIdentity!=row.identity)return false;
            targets[count++]={&row,flags,index};
        }
        //All owners/addresses preflight before writes. No allocation, source
        //callback or foreign visit can occur while the lexical lease is held.
        for(std::size_t i=0;i<count;++i){auto& target=targets[i];
            *target.flags=accepted[target.index].flagsF8;
            std::memcpy(&s.source[target.index],&accepted[target.index],sizeof(s.source[target.index]));
            ++target.row->effectRevision;lease.wrote_=true;
        }
        if(second)out.sharedSecondFlagsPublished=true;else out.sharedFirstFlagsPublished=true;
        lease.completed_=true;return true;
    }
    void finishSharedCollectionInvocation(const EndCandidateResult& out)noexcept {
        //A first returned query cannot be rerun even if publication preflight
        //rejects before its native mirrors are written. Published effects survive.
        //Do not refresh/replay them against another World/producer publication.
        if(storage_.get()&&storage_.get()->sharedRoster_&&out.firstReturned&&
           (!out.sharedFirstFlagsPublished||!out.sharedSecondFlagsPublished||!out.merged))sharedEffectsStopped_=true;
    }
