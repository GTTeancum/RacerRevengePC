    //Actual28E984 call after fresh28E230, with ModelBASE and visibility mode2.
    //This primary owner stops before280BF0; its separately gated continuation cannot admit28EA14.
    template<class ActualMethod>
    Result continueSourceModelPrimary2809C0(ActualMethod&& actualMethod)noexcept {
        auto& h=modelObserver_;
        if(!h.entered||h.returned||h.nativeWithdrawn||h.primaryCalled||!h.boundsReturned||
           h.pc!=0x28e96cu||!h.constructorReturned||!h.pointerStored||!h.published||
           !modelObserverStorage_||nativeWithdrawn_||!base_||!base_->object_||!model_||
           h.ownerIdentity!=&model_->model_||base_->object_->model14C!=h.ownerIdentity||
           !base_->object_->visibilityB4||*base_->object_->visibilityB4!=h.zone||h.mode!=2u||
           model_->model_.observer19C!=h.published||model_->sourceObserverBorrower_!=&h||
           !preflightNative())return {Status::owner,0x2809c0u};
        h.pc=0x2809c0u;h.primaryCalled=true;
        auto& observer=*modelObserverStorage_;
        if(!observer.beginRegistration2809C0(&h,h.zone,h.center))
            return {Status::bounded,h.pc=observer.registrationPC()?observer.registrationPC():0x2809c0u};
        OriginalNativeListMethodCustody50LA* resolved=nullptr;
        h.pc=0x27f5b4u;
        if(!actualMethod(h.zone,&h,h.ownerIdentity,resolved,h.slot,h.selectionPending,h.pc,false))
            return {Status::bounded,h.pc};
        h.methodResolved=true;h.method=resolved;
        //Owner-aware native pin is independent of source refcount/usage. It
        //fences method destruction even before selection acquires a pending slot.
        h.pc=0x280a80u;
        if(h.method){
            auto& c=*h.method;
            if(c.application_!=base_->application_.get()||!c.sourceMode_||c.usage10_!=2u||
               c.ordinal_/10u!=2u||c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())
                return {Status::bounded,h.pc};
            for(const auto* pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&h)return {Status::bounded,h.pc};
            try{c.sourceCollisionRegistrationBorrowers_.push_back(&h);}catch(...){return {Status::bounded,h.pc};}
            h.methodPinned=true;
        }
        if(!observer.recordMethod280A80(h.method))return {Status::bounded,h.pc=observer.registrationPC()};
        h.selectionAttempted=true;h.pc=0x280afcu;
        if(!actualMethod(h.zone,&h,h.ownerIdentity,h.method,h.slot,h.selectionPending,h.pc,true))
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
           e.pendingIdentity!=h.ownerIdentity||e.pendingSlot!=slot||!h.selectionPending)
            return {Status::bounded,h.pc};
        h.freeHeadBefore=*state.field40;h.lastUsedBefore=*state.lastUsed38;
        if(*state.field40==slot){h.pc=0x27e594u;
            if(!e.words||e.members[slot-1])return {Status::bounded,h.pc};
            state.field40=e.nativeFreeNext[slot-1];}
        else {h.pc=0x27e5a0u;state.lastUsed38=*state.lastUsed38+1u;}
        h.freeHeadAfter=*state.field40;h.lastUsedAfter=*state.lastUsed38;
        e.pendingConsumed=true;h.counterAdvanced=true;h.pc=0x27e5a8u;
        if(!e.words)return {Status::bounded,h.pc};
        e.members[slot-1]=h.ownerIdentity;e.borrowers[slot-1]=&h;e.wordKnown[slot-1]=false;
        e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;
        h.selectionPending=false;h.inserted=true;h.insertionReturnPC=0x27e5acu;
        if(!observer.publishPrimary280B90())return {Status::bounded,h.pc=observer.registrationPC()};
        h.primaryPublished=true;h.pc=0x280bf0u;
        return {Status::bounded,h.pc}; //280BF0 remains unissued.
    }
    bool sourceModelPrimaryHistoryNative()const noexcept {
        const auto& h=modelObserver_;
        if(!h.primaryCalled)return !h.methodResolved&&!h.methodPinned&&!h.methodPinWithdrawn&&!h.method&&!h.slot&&
            !h.selectionAttempted&&!h.selectionReturned&&!h.selectionPending&&!h.selectionCanceled&&!h.selectionReturnPC&&
            !h.insertEntered&&!h.counterAdvanced&&!h.inserted&&!h.insertionReturnPC&&!h.slotWithdrawn&&
            !h.primaryPublished&&!h.primaryNativeWithdrawn&&(!h.boundsReturned||h.pc==0x28e96cu)&&
            sourceModelPortalsHistoryNative();
        const auto* observer=modelObserverStorage_.get();
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
           (h.primaryPublished&&(!h.inserted||o.stores!=31u||(!modelPortals_.entered&&h.pc!=0x280bf0u)))||
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
               c.ordinal_/10u!=2u||c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())return false;
            unsigned pins=0;for(const auto* pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&h)++pins;
            if(pins!=1||h.selectionPending!=(e.pending==&h))return false;
            if(h.selectionPending&&(e.pendingIdentity!=h.ownerIdentity||
                (h.selectionReturned&&e.pendingSlot!=*h.slot)||e.pendingConsumed!=h.counterAdvanced))return false;
            if(h.inserted&&!h.slotWithdrawn&&(!h.slot||!*h.slot||*h.slot>1024||
                e.members[*h.slot-1]!=h.ownerIdentity||e.borrowers[*h.slot-1]!=&h||e.wordKnown[*h.slot-1]))return false;
        }else if(h.methodPinWithdrawn&&(!h.primaryNativeWithdrawn||h.selectionPending||(h.inserted&&!h.slotWithdrawn)))return false;
        if(h.nativeWithdrawn&&!h.primaryNativeWithdrawn)return false;
        return sourceModelPortalsHistoryNative(); //No method dereference after exact native unpin.
    }
    bool withdrawSourceModelPrimaryNative()noexcept {
        auto& h=modelObserver_;if(!h.primaryCalled||h.primaryNativeWithdrawn)return true;
        if(!sourceModelPrimaryHistoryNative()||!withdrawSourceModelPortalsNative())return false;
        auto* observer=modelObserverStorage_.get();
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
                   e.members[*h.slot-1]!=h.ownerIdentity||e.borrowers[*h.slot-1]!=&h)return false;
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
