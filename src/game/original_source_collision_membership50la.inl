    using MembershipObserver=OriginalSourceCollisionObserver50LA;
    using MembershipGuards=OriginalCollisionMembershipGuards50LA;
    MembershipObserver membershipStack_;
    std::unique_ptr<MembershipObserver> membershipHeap_;
    struct SourceMembership120070 {
        const void* updateOccurrence=nullptr;
        const void* outerIdentity=nullptr;
        const void* resultOwnerHistory=nullptr;
        const void* resultOuterHistory=nullptr;
        //Role and preceding full registration are immutable occurrence evidence.
        bool resultBacked=false;
        const void* precedingRegistrationHistory=nullptr;
        std::uint32_t precedingRegistrationPC=0;
        const void* zone=nullptr;
        std::uint32_t mode=0,pc=0;
        const void* before=nullptr;
        const void* published=nullptr;
        const void* selected=nullptr;
        std::uint8_t flagsBefore=0;
        bool entered=false,returned=false,stackCalled=false,stackReturned=false,flagsRead=false,stackSelected=false;
        bool existingRead=false,allocationCalled=false,allocationReturned=false,heapCalled=false,heapReturned=false,pointerStored=false;
        bool scratchInitialized=false,boundsEntered=false,boundsCompleted=false,boundsReturned=false;
        bool nativePointerCleared=false,nativeWithdrawn=false;
        bool primaryCalled=false,methodResolved=false,methodPinned=false,methodPinWithdrawn=false;
        bool selectionAttempted=false,selectionReturned=false,selectionPending=false,selectionCanceled=false;
        bool insertEntered=false,counterAdvanced=false,inserted=false,slotWithdrawn=false;
        bool primaryPublished=false,primaryNativeWithdrawn=false;
        OriginalNativeListMethodCustody50LA* method=nullptr;
        std::optional<std::uint32_t> slot;
        std::uint32_t freeHeadBefore=0,lastUsedBefore=0,freeHeadAfter=0,lastUsedAfter=0;
        std::uint32_t selectionReturnPC=0,insertionReturnPC=0;
        MembershipGuards::History allocationGuard;
        std::array<std::uint32_t,4> center{{0,0,0,0x3f800000u}},half{{0,0,0,0x3f800000u}};
        std::optional<Result> boundsResult;
    } membershipRegistration_;
    struct SourceMembershipReturn120190 {
        const void* selectedIdentity=nullptr;
        const void* zone04=nullptr;
        bool entered=false,observerReturned=false,zoneRead=false;
        bool stackDestroyCalled=false,stackDestroyReturned=false,returned=false;
        std::int32_t stackArgument=0;
        std::uint32_t observerReturnPC=0,stackReturnPC=0,pc=0;
        MembershipGuards::History zoneGuard;
    } membershipReturn_;
    #include "original_source_collision_membership_portals50la.inl"
    template<class ActualMethod,class ActualPortal>
    Result registerSourceDirectMembership120070(const void* updateOccurrence,
        const OriginalSourcePhysicsCollision50LA& actualE0,const void* zone,std::uint32_t mode,
        ActualMethod&& actualMethod,ActualPortal&& actualPortal)noexcept {
        return registerSourceMembership120070(updateOccurrence,actualE0,zone,mode,false,actualMethod,actualPortal);
    }
    template<class ActualMethod,class ActualPortal>
    Result registerSourceResultMembership120070(const void* updateOccurrence,
        const OriginalSourcePhysicsCollision50LA& actualE4,const void* zone,std::uint32_t mode,
        ActualMethod&& actualMethod,ActualPortal&& actualPortal)noexcept {
        return registerSourceMembership120070(updateOccurrence,actualE4,zone,mode,true,actualMethod,actualPortal);
    }
    template<class ActualMethod,class ActualPortal>
    Result registerSourceMembership120070(const void* updateOccurrence,
        const OriginalSourcePhysicsCollision50LA& predecessor,const void* actualZone,std::uint32_t actualMode,
        bool resultBacked,ActualMethod&& actualMethod,ActualPortal&& actualPortal)noexcept {
        auto& h=membershipRegistration_;
        if(!updateOccurrence||h.entered||nativeWithdrawn_||nativeRetired_||
           (resultBacked?occurrence_!=Occurrence::resultE0:!isDirectE4())||
           !base_||!base_->object_||!collision_||!postUpdate_.returned||postUpdate_.leaseWithdrawn||
           postUpdate_.updateOccurrence!=updateOccurrence||!nullMembership_.returned||
           nullMembership_.updateOccurrence!=updateOccurrence||!base_->object_->collisionB8||
           *base_->object_->collisionB8!=actualZone||actualMode!=2u||
           (resultBacked?base_->object_->collisionE0:base_->object_->collisionE4)!=std::optional<B*>(collision_.get())||
           !preflightNative()||&predecessor==this||predecessor.base_!=base_||predecessor.bodyBorrow_!=bodyBorrow_||
           (resultBacked?!predecessor.isDirectE4():predecessor.occurrence_!=Occurrence::resultE0)||
           predecessor.nativeWithdrawn_||predecessor.nativeRetired_||!predecessor.postUpdate_.returned||
           predecessor.postUpdate_.pc!=0x11ff5cu||predecessor.postUpdate_.leaseWithdrawn||
           predecessor.postUpdate_.updateOccurrence!=updateOccurrence||!predecessor.postUpdateHistoryNative()||
           !predecessor.collision_||
           (resultBacked?base_->object_->collisionE4:base_->object_->collisionE0)!=std::optional<B*>(predecessor.collision_.get()))
            return {Status::owner,0x120070u};
        if(resultBacked){const auto& prior=predecessor.membershipRegistration_;
            if(!prior.returned||prior.pc!=0x120224u||prior.nativeWithdrawn||prior.resultBacked||
               prior.updateOccurrence!=updateOccurrence||prior.zone!=actualZone||prior.mode!=actualMode||
               prior.resultOwnerHistory!=this||prior.resultOuterHistory!=collision_.get()||
               postUpdate_.e4OwnerHistory!=&predecessor||postUpdate_.e4OuterHistory!=predecessor.collision_.get()||
               !predecessor.membershipRegistrationHistoryNative())return {Status::owner,0x120070u};}
        h.entered=true;h.updateOccurrence=updateOccurrence;h.outerIdentity=collision_.get();h.resultBacked=resultBacked;
        //These earlier sibling identities remain opaque after native retirement.
        //No reverse lease on E4 is introduced into the existing Contact/Body chain.
        h.resultOwnerHistory=&predecessor;h.resultOuterHistory=predecessor.collision_.get();h.zone=actualZone;h.mode=actualMode;
        if(resultBacked){h.precedingRegistrationHistory=&predecessor.membershipRegistration_;h.precedingRegistrationPC=0x120224u;}
        //Native before-image only. Actual E0 load is separately recorded at1200B8.
        h.before=*collision_->membershipE0_;h.flagsBefore=collision_->flagsF8_;
        h.stackCalled=true;h.pc=0x280890u;
        if(!membershipStack_.construct280890(base_->application_->garageObserverShared_))
            return {Status::bounded,h.pc=membershipStack_.pc()};
        h.stackReturned=true;h.pc=0x12009cu;h.flagsRead=true;
        h.stackSelected=(collision_->flagsF8_&8u)!=0;
        if(h.stackSelected)h.selected=membershipStack_.identity();
        else {
            h.pc=0x1200b8u;h.existingRead=true;
            //Initial exact NULL removal is retained. No foreign observer is
            //adopted or native-freed as an invented280920 source destructor.
            if(*collision_->membershipE0_)return {Status::bounded,h.pc=0x1200c4u};
            h.pc=0x1200d0u;h.allocationCalled=true;
            membershipHeap_.reset(new(std::nothrow) MembershipObserver);
            h.allocationReturned=true;
            if(membershipHeap_){h.heapCalled=true;h.pc=0x280890u;
                if(!membershipHeap_->construct280890(base_->application_->garageObserverShared_))
                    return {Status::bounded,h.pc=membershipHeap_->pc()};
                h.heapReturned=true;}
            h.pc=0x1200e8u;h.published=membershipHeap_?membershipHeap_->identity():nullptr;
            collision_->membershipE0_=h.published;h.pointerStored=true;h.selected=h.published;
        }
        h.pc=0x1200f0u;
        if(!MembershipGuards::beginAllocation(h.allocationGuard))return {Status::bounded,h.pc=h.allocationGuard.pc};
        if(!MembershipGuards::finishAllocation(h.allocationGuard,h.selected!=nullptr))
            return {Status::bounded,h.pc=h.allocationGuard.pc};
        //Both caller quads initialized from VF0 before the actual bounds call.
        h.center=h.half={{0,0,0,0x3f800000u}};h.scratchInitialized=true;
        h.boundsEntered=true;h.pc=0x1202e0u;
        const auto bounds=boundsSourceType0_1202E0(updateOccurrence,h.center,h.half);
        h.boundsCompleted=true;h.boundsResult=bounds;h.pc=bounds.sourcePC;
        if(bounds.status!=Status::reached||bounds.sourcePC!=0x120e68u||!membershipBounds_.returned)
            return {Status::bounded,h.pc};
        h.boundsReturned=true;
        //120170 supplies zero overlap radius. Only this exact observer's
        //primary registration is continued; portal280BF0 remains separate.
        h.pc=0x2809c0u;h.primaryCalled=true;
        auto& observer=h.stackSelected?membershipStack_:*membershipHeap_;
        if(!observer.beginRegistration2809C0(&h,h.zone,h.center))
            return {Status::bounded,h.pc=observer.registrationPC()?observer.registrationPC():0x2809c0u};
        OriginalNativeListMethodCustody50LA* resolved=nullptr;
        h.pc=0x27f5b4u;
        if(!actualMethod(h.zone,&h,h.outerIdentity,resolved,h.slot,h.selectionPending,h.pc,false))
            return {Status::bounded,h.pc};
        h.methodResolved=true;h.method=resolved;
        //Owner-aware native pin is independent of source refcount/usage. It
        //fences method destruction even before selection acquires a pending slot.
        h.pc=0x280a80u;
        if(h.method){
            auto& c=*h.method;
            if(c.application_!=base_->application_.get()||!c.sourceMode_||c.usage10_!=2u||
               c.ordinal_/10u!=3u||c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())
                return {Status::bounded,h.pc};
            for(const auto* pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&h)return {Status::bounded,h.pc};
            try{c.sourceCollisionRegistrationBorrowers_.push_back(&h);}catch(...){return {Status::bounded,h.pc};}
            h.methodPinned=true;
        }
        if(!observer.recordMethod280A80(h.method))return {Status::bounded,h.pc=observer.registrationPC()};
        h.selectionAttempted=true;h.pc=0x280afcu;
        if(!actualMethod(h.zone,&h,h.outerIdentity,h.method,h.slot,h.selectionPending,h.pc,true))
            return {Status::bounded,h.pc};
        h.selectionReturned=true;h.selectionReturnPC=0x27e568u;
        if(!h.slot||!observer.recordSelection280B08(*h.slot))
            return {Status::bounded,h.pc=h.slot?observer.registrationPC():0x280b08u};
        //Authenticated table3D2D30+24=27E570; exact selected slot is not a
        //host-selected index or a source DWORD made from a native address.
        h.insertEntered=true;h.pc=0x280b88u;
        auto& c=*h.method;auto& e=c.sourceEntries_;auto& state=c.source_;
        const auto slot=*h.slot;
        if(!slot||slot>1024||!state.field40||!state.lastUsed38||e.pending!=&h||
           e.pendingIdentity!=h.outerIdentity||e.pendingSlot!=slot||!h.selectionPending)
            return {Status::bounded,h.pc};
        h.freeHeadBefore=*state.field40;h.lastUsedBefore=*state.lastUsed38;
        if(*state.field40==slot){h.pc=0x27e594u;
            if(!e.words||e.members[slot-1])return {Status::bounded,h.pc};
            state.field40=e.nativeFreeNext[slot-1];}
        else {h.pc=0x27e5a0u;state.lastUsed38=*state.lastUsed38+1u;}
        h.freeHeadAfter=*state.field40;h.lastUsedAfter=*state.lastUsed38;
        e.pendingConsumed=true;h.counterAdvanced=true;h.pc=0x27e5a8u;
        if(!e.words)return {Status::bounded,h.pc};
        e.members[slot-1]=h.outerIdentity;e.borrowers[slot-1]=&h;e.wordKnown[slot-1]=false;
        e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;
        h.selectionPending=false;h.inserted=true;h.insertionReturnPC=0x27e5acu;
        if(!observer.publishPrimary280B90())return {Status::bounded,h.pc=observer.registrationPC()};
        h.primaryPublished=true;h.pc=0x280bf0u;
        const auto portals=continueSourceMembershipPortals280BF0(actualPortal);
        if(!membershipPortals_.returned)return portals;
        return continueSourceMembershipReturn120190();
    }
    Result continueSourceMembershipReturn120190()noexcept {
        auto& h=membershipRegistration_;auto& f=membershipReturn_;
        if(f.entered||!h.entered||h.returned||h.nativeWithdrawn||!h.primaryPublished||
           !membershipPortals_.returned||membershipPortals_.pc!=0x280d38u||
           membershipPortals_.nativeWithdrawn||h.pc!=0x280d38u||!membershipRegistrationHistoryNative())
            return {Status::owner,0x120190u};
        auto* observer=h.stackSelected?&membershipStack_:membershipHeap_.get();
        if(!observer||observer->identity()!=h.selected||!observer->secondary_.returned||
           observer->secondary_.returnPC!=0x280d38u)return {Status::owner,0x120190u};
        f.entered=true;f.selectedIdentity=h.selected;
        //Actual2809C0 epilogue after its returned280BF0, JR280BE4/delay280BE8.
        f.observerReturned=true;f.observerReturnPC=0x280becu;
        const auto stop=[&](std::uint32_t pc){f.pc=h.pc=pc;return Result{Status::bounded,pc};};
        f.pc=h.pc=0x120190u;
        if(!MembershipGuards::beginZone(f.zoneGuard))return stop(f.zoneGuard.pc);
        //Known-enabled guard route reaches the actual selected observer+04
        //load; B8 and COL membership pointers are not substitute predicates.
        f.pc=h.pc=0x1201bcu;f.zone04=observer->record_.zone04;f.zoneRead=true;
        if(!MembershipGuards::finishZone(f.zoneGuard,f.zone04!=nullptr))return stop(f.zoneGuard.pc);
        f.pc=h.pc=0x120200u;f.stackDestroyCalled=true;f.stackArgument=-1;
        //Always the separate stack observer, even on heap-selected flags4.
        //Registered stack-selected teardown remains outside this helper's
        //qualified domain; its refusal cannot be translated to native cleanup.
        if(!membershipStack_.destroy280920(f.stackArgument))return stop(0x280920u);
        f.stackDestroyReturned=true;f.stackReturnPC=membershipStack_.pc();
        if(f.stackReturnPC!=0x2809b8u)return stop(f.stackReturnPC);
        //120208..120220 restores caller registers and returns; no defined V0.
        f.returned=h.returned=true;f.pc=h.pc=0x120224u;return {Status::reached,h.pc};
    }
    bool membershipReturnHistoryNative()const noexcept {
        const auto& h=membershipRegistration_;const auto& f=membershipReturn_;
        if(!f.entered)return !f.selectedIdentity&&!f.zone04&&!f.observerReturned&&!f.zoneRead&&
            !f.stackDestroyCalled&&!f.stackDestroyReturned&&!f.returned&&!f.stackArgument&&
            !f.observerReturnPC&&!f.stackReturnPC&&!f.pc&&!h.returned&&MembershipGuards::zoneHistory(f.zoneGuard);
        const auto* observer=h.stackSelected?&membershipStack_:membershipHeap_.get();
        if(!h.entered||!h.primaryPublished||!membershipPortals_.returned||membershipPortals_.pc!=0x280d38u||
           !observer||!f.selectedIdentity||f.selectedIdentity!=h.selected||observer->identity()!=f.selectedIdentity||
           !f.observerReturned||f.observerReturnPC!=0x280becu||!observer->secondary_.returned||
           observer->secondary_.returnPC!=0x280d38u||!MembershipGuards::zoneHistory(f.zoneGuard)||
           f.pc!=h.pc||f.returned!=h.returned)return false;
        if(!f.zoneGuard.returned){
            if(f.stackDestroyCalled||f.stackDestroyReturned||f.stackArgument||f.stackReturnPC||f.returned||
               f.pc!=f.zoneGuard.pc)return false;
            return f.zoneRead?(f.zoneGuard.finishAttempted&&f.zone04==observer->record_.zone04):
                (!f.zone04&&!f.zoneGuard.finishAttempted);
        }
        if(!f.zoneRead||!f.zone04||f.zone04!=observer->record_.zone04||
           !f.zoneGuard.nonnull||!f.stackDestroyCalled||f.stackArgument!=-1)return false;
        if(!f.stackDestroyReturned)return !f.returned&&!f.stackReturnPC&&f.pc==0x280920u&&
            !membershipStack_.destroyEntered_;
        return !h.stackSelected&&f.returned&&f.pc==0x120224u&&f.stackReturnPC==0x2809b8u&&
            membershipStack_.destroyEntered_&&membershipStack_.destroyReturned_&&membershipStack_.countDecremented_&&
            membershipStack_.destructionArgument_==-1&&!membershipStack_.sourceDeleted_&&
            membershipStack_.destroyPC_==f.stackReturnPC&&!membershipStack_.lease_;
    }
    bool membershipPointerCurrentNative()const noexcept {
        if(!collision_||!collision_->membershipE0_)return false;
        const auto& h=membershipRegistration_;
        const void* expected=h.entered?(h.nativePointerCleared?nullptr:h.pointerStored?h.published:h.before):nullptr;
        return *collision_->membershipE0_==expected;
    }
    bool membershipRegistrationHistoryNative()const noexcept {
        const auto& h=membershipRegistration_;
        if(!membershipReturnHistoryNative())return false;
        if(!h.entered)return !h.resultBacked&&!h.precedingRegistrationHistory&&!h.precedingRegistrationPC&&!h.updateOccurrence&&!h.outerIdentity&&!h.resultOwnerHistory&&!h.resultOuterHistory&&!h.zone&&!h.mode&&!h.pc&&
            !h.returned&&!h.stackCalled&&!h.stackReturned&&!h.flagsRead&&!h.stackSelected&&!h.existingRead&&!h.allocationCalled&&
            !h.allocationReturned&&!h.heapCalled&&!h.heapReturned&&!h.pointerStored&&!h.selected&&!h.published&&!h.before&&
            !h.scratchInitialized&&!h.boundsEntered&&!h.boundsCompleted&&!h.boundsReturned&&!h.boundsResult&&
            !h.nativePointerCleared&&!h.nativeWithdrawn&&!h.primaryCalled&&!h.methodResolved&&!h.methodPinned&&!h.methodPinWithdrawn&&!h.method&&!h.slot&&
            !h.selectionAttempted&&!h.selectionReturned&&!h.selectionPending&&!h.insertEntered&&!h.counterAdvanced&&!h.inserted&&!h.slotWithdrawn&&
            !h.primaryPublished&&!h.primaryNativeWithdrawn&&!membershipHeap_&&membershipStack_.preflightNative()&&
            MembershipGuards::allocationHistory(h.allocationGuard);
        if((h.resultBacked?occurrence_!=Occurrence::resultE0:!isDirectE4())||
           (h.resultBacked?(!h.precedingRegistrationHistory||h.precedingRegistrationHistory==&h||h.precedingRegistrationPC!=0x120224u||
                            h.resultOwnerHistory!=postUpdate_.e4OwnerHistory||h.resultOuterHistory!=postUpdate_.e4OuterHistory):
                            bool(h.precedingRegistrationHistory||h.precedingRegistrationPC))||!h.updateOccurrence||h.updateOccurrence!=postUpdate_.updateOccurrence||!postUpdate_.returned||
           !h.outerIdentity||h.outerIdentity!=nullMembership_.ownerIdentity||!nullMembership_.returned||
           !h.resultOwnerHistory||h.resultOwnerHistory==this||!h.resultOuterHistory||h.resultOuterHistory==h.outerIdentity||
           h.mode!=2u||h.before||!h.stackCalled||!membershipStack_.preflightNative()||
           (membershipHeap_&&!membershipHeap_->preflightNative())||!MembershipGuards::allocationHistory(h.allocationGuard))return false;
        if(h.stackReturned!=membershipStack_.returned()||h.heapReturned!=bool(membershipHeap_&&membershipHeap_->returned()))return false;
        if(h.nativePointerCleared&&!h.pointerStored)return false;
        if(h.nativeWithdrawn&&(!membershipStack_.nativeWithdrawn_||
           (membershipHeap_&&!membershipHeap_->nativeWithdrawn_)||(h.pointerStored&&!h.nativePointerCleared)))return false;
        if(nativeWithdrawn_){if(!h.nativeWithdrawn)return false;}
        else if(!collision_||collision_.get()!=h.outerIdentity||collision_->flagsF8_!=h.flagsBefore||!membershipPointerCurrentNative())return false;
        if(!h.stackReturned)return !h.flagsRead&&!h.heapCalled&&!h.allocationCalled&&!h.pointerStored&&!h.selected&&
            !h.allocationGuard.entered&&!h.boundsEntered&&h.pc==membershipStack_.pc();
        if(!h.flagsRead||h.stackSelected!=bool(h.flagsBefore&8u))return false;
        if(h.stackSelected){
            if(h.existingRead||h.allocationCalled||h.allocationReturned||membershipHeap_||h.heapCalled||h.heapReturned||
               h.pointerStored||h.published||h.selected!=membershipStack_.identity())return false;
        }else{
            if(!h.existingRead||!h.allocationCalled||!h.allocationReturned||h.heapCalled!=bool(membershipHeap_))return false;
            if(membershipHeap_&&!h.heapReturned)return !h.pointerStored&&!h.selected&&!h.allocationGuard.entered&&
                !h.boundsEntered&&h.pc==membershipHeap_->pc();
            if(!h.pointerStored||h.published!=(membershipHeap_?membershipHeap_->identity():nullptr)||h.selected!=h.published)return false;
        }
        if(!h.allocationGuard.entered)return false;
        if(!h.allocationGuard.returned)return !h.scratchInitialized&&!h.boundsEntered&&!h.boundsResult&&h.pc==h.allocationGuard.pc;
        if(!h.selected||!h.scratchInitialized||!h.boundsEntered)return false;
        if(!h.boundsCompleted)return !h.boundsResult&&!h.boundsReturned&&h.pc==0x1202e0u;
        if(!h.boundsResult)return false;
        const auto& b=membershipBounds_;
        if(!b.attempted)return !h.boundsReturned&&h.boundsResult->status==Status::owner&&h.boundsResult->sourcePC==0x1202e0u&&h.pc==0x1202e0u;
        if(b.updateOccurrence!=h.updateOccurrence||h.boundsResult->sourcePC!=b.pc||h.center!=b.centerAfter||h.half!=b.halfAfter||
           h.boundsResult->status!=(b.returned?Status::reached:Status::bounded))return false;
        return h.boundsReturned?(b.returned&&b.pc==0x120e68u&&membershipPrimaryHistoryNative()):(!h.primaryCalled&&!b.returned&&h.pc==b.pc);
    }
    bool membershipPrimaryHistoryNative()const noexcept {
        const auto& h=membershipRegistration_;
        if(!h.primaryCalled)return !h.methodResolved&&!h.methodPinned&&!h.methodPinWithdrawn&&!h.method&&!h.slot&&
            !h.selectionAttempted&&!h.selectionReturned&&!h.selectionPending&&!h.selectionCanceled&&!h.selectionReturnPC&&
            !h.insertEntered&&!h.counterAdvanced&&!h.inserted&&!h.insertionReturnPC&&!h.slotWithdrawn&&
            !h.primaryPublished&&!h.primaryNativeWithdrawn&&h.pc==0x2809c0u;
        const auto* observer=h.stackSelected?&membershipStack_:membershipHeap_.get();
        if(!h.boundsReturned||!observer||!observer->preflightNative())return false;
        const auto& o=observer->primary_;
        if(!o.entered)return !h.methodResolved&&!h.methodPinned&&!h.selectionAttempted&&!h.insertEntered&&h.pc==0x2809c0u;
        if(o.occurrence!=&h||o.zone!=h.zone||o.center!=h.center||o.nativeWithdrawn!=h.primaryNativeWithdrawn||
           (h.methodPinned&&(!h.methodResolved||!h.method))||(h.methodPinWithdrawn&&!h.methodPinned)||
           (h.selectionAttempted&&(!h.methodPinned||!o.methodAssertion.passed))||
           (h.selectionReturned&&(!h.selectionAttempted||!h.slot||h.selectionReturnPC!=0x27e568u))||
           (!h.selectionReturned&&h.selectionReturnPC)||
           (h.insertEntered&&(!h.selectionReturned||!o.slotAssertion.passed))||
           (h.counterAdvanced&&!h.insertEntered)||(h.inserted&&(!h.counterAdvanced||h.insertionReturnPC!=0x27e5acu))||
           (!h.inserted&&h.insertionReturnPC)||(h.slotWithdrawn&&!h.inserted)||
           (h.primaryPublished&&(!h.inserted||o.stores!=31u||(!membershipPortals_.entered&&h.pc!=0x280bf0u)))||
           (!h.primaryPublished&&o.stores)||(h.selectionCanceled&&h.selectionPending))return false;
        if(o.methodRead&&(!h.methodResolved||o.method!=h.method))return false;
        if(o.slotRead&&(!h.selectionReturned||o.slot!=*h.slot))return false;
        if(h.counterAdvanced){
            if(!h.slot||!*h.slot||*h.slot>1024)return false;
            if(h.freeHeadBefore==*h.slot){if(h.lastUsedAfter!=h.lastUsedBefore||h.freeHeadAfter>1024)return false;}
            else if(h.freeHeadBefore||*h.slot!=h.lastUsedBefore+1u||h.lastUsedAfter!=h.lastUsedBefore+1u||h.freeHeadAfter)return false;
        }
        if(h.methodPinned&&!h.methodPinWithdrawn){
            const auto& c=*h.method;const auto& e=c.sourceEntries_;
            if(!base_||c.application_!=base_->application_.get()||!c.sourceMode_||c.usage10_!=2u||
               c.ordinal_/10u!=3u||c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())return false;
            unsigned pins=0;for(const auto* pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&h)++pins;
            if(pins!=1||h.selectionPending!=(e.pending==&h))return false;
            if(h.selectionPending&&(e.pendingIdentity!=h.outerIdentity||
                (h.selectionReturned&&e.pendingSlot!=*h.slot)||e.pendingConsumed!=h.counterAdvanced))return false;
            if(h.inserted&&!h.slotWithdrawn&&(!h.slot||!*h.slot||*h.slot>1024||
                e.members[*h.slot-1]!=h.outerIdentity||e.borrowers[*h.slot-1]!=&h||e.wordKnown[*h.slot-1]))return false;
        }else if(h.methodPinWithdrawn&&(!h.primaryNativeWithdrawn||h.selectionPending||(h.inserted&&!h.slotWithdrawn)))return false;
        if(h.nativeWithdrawn&&!h.primaryNativeWithdrawn)return false;
        return membershipPortalsHistoryNative(); //No dereference after exact native unpin.
    }
    bool withdrawSourceMembershipPrimaryNative()noexcept {
        auto& h=membershipRegistration_;if(!h.primaryCalled||h.primaryNativeWithdrawn)return true;
        if(!membershipPrimaryHistoryNative()||!withdrawSourceMembershipPortalsNative())return false;
        auto* observer=h.stackSelected?&membershipStack_:membershipHeap_.get();
        if(!observer)return false;
        if(h.methodPinned&&!h.methodPinWithdrawn){
            auto& c=*h.method;auto& e=c.sourceEntries_;
            if(h.selectionPending){
                if(e.pending!=&h)return false;
                if(e.pendingConsumed){e.abandonedConsumed=e.pendingSlot;e.terminal=true;e.pendingConsumed=false;}
                e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;
                h.selectionPending=false;h.selectionCanceled=true;
            }
            if(h.inserted&&!h.slotWithdrawn){
                if(!h.slot||!*h.slot||*h.slot>1024||!e.words||e.pending||!c.source_.field40||
                   e.members[*h.slot-1]!=h.outerIdentity||e.borrowers[*h.slot-1]!=&h)return false;
                const auto slot=*h.slot;e.nativeFreeNext[slot-1]=*c.source_.field40;
                e.wordKnown[slot-1]=false;e.members[slot-1]=e.borrowers[slot-1]=nullptr;
                c.source_.field40=slot;h.slotWithdrawn=true;
            }
        }
        if(observer->primary_.entered&&!observer->withdrawRegistrationNative(&h))return false;
        h.primaryNativeWithdrawn=true;
        if(h.methodPinned&&!h.methodPinWithdrawn){
            auto& pins=h.method->sourceCollisionRegistrationBorrowers_;
            auto i=pins.begin();while(i!=pins.end()&&*i!=&h)++i;
            if(i==pins.end())return false;
            pins.erase(i);h.methodPinWithdrawn=true;
        }
        return true; //Native removal/free-list repair, not source280920/observer return.
    }
    bool withdrawSourceMembershipNative()noexcept {
        auto& h=membershipRegistration_;if(!h.entered||h.nativeWithdrawn)return true;
        if(!membershipRegistrationHistoryNative()||!boundsHistoryNative()||!withdrawSourceMembershipPrimaryNative())return false;
        if(h.pointerStored&&!h.nativePointerCleared){
            if(!collision_||!membershipPointerCurrentNative())return false;
            collision_->membershipE0_=nullptr;h.nativePointerCleared=true;
        }
        //Native cancellation is distinct from any already completed source
        //120200 stack destruction. Heap leaves first; a source-destroyed stack
        //has no count lease left, so its native withdrawal cannot decrement twice.
        if(membershipHeap_&&!membershipHeap_->withdrawNative())return false;
        if(!membershipStack_.withdrawNative())return false;
        h.nativeWithdrawn=true;return true;
    }
