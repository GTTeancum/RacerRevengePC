// Callback-owned genuine mode1 registration. No persistent Physics E0 owner.
    template<class ActualPortal>
    static bool continueSourceMembershipPortals280BF0(SourceCallback& callback,ActualPortal&& actualPortal)noexcept {
        auto& h=callback.membership;auto& p=h.portals;
        auto& observer=*h.stack;
        const auto stop=[&](std::uint32_t pc){h.pc=p.pc=pc;return false;};
        if(!h.primaryPublished||p.entered)return stop(0x280bf0u);
        p.entered=true;p.pc=0x280bf0u;
        if(!observer.beginSecondary280BF0(&h))return stop(p.pc);
        p.helperEntered=true;p.cursorMethod=h.method;
        SourceMembershipPortalEdge initial;
        if(!actualPortal(0u,&h,h.outerIdentity,h.zoneIdentity,h.center,nullptr,initial,p.graphPin,p.pc))return stop(p.pc);
        p.rootRead=true;p.root=p.cursor=initial.next;
        while(p.cursor){
            if(p.count==p.edges.size())return stop(0x280c40u);
            auto& s=p.edges[p.count++];s.previousMethod=p.cursorMethod;
            const bool edgeOkay=actualPortal(1u,&h,h.outerIdentity,h.zoneIdentity,h.center,p.cursor,s,p.graphPin,p.pc);
            if(s.methodReturned)p.cursorMethod=s.method;
            if(!edgeOkay)return stop(p.pc);
            if(s.hit){
                p.cursorMethod=s.method;
                if(!s.methodReturned||!s.method)return stop(0x280c78u);
                auto& c=*s.method;
                if(c.application_!=callback.application||c.world_!=h.world||!c.sourceMode_||c.usage10_!=1u||c.ordinal_/10u!=1u||
                   c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())return stop(0x280c90u);
                try{c.sourceCollisionRegistrationBorrowers_.push_back(&s);}catch(...){return stop(0x280c90u);}
                s.methodPinned=true;s.selectionAttempted=true;
                if(!actualPortal(2u,&h,h.outerIdentity,h.zoneIdentity,h.center,p.cursor,s,p.graphPin,p.pc))return stop(p.pc);
                s.selectionReturned=true;
                if(!s.slot)return stop(0x280c9cu);
                auto& e=c.sourceEntries_;
                if(!*s.slot){
                    if(e.pending!=&s||e.pendingIdentity!=h.outerIdentity||e.pendingSlot)return stop(0x280c9cu);
                    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;s.selectionPending=false;s.selectionCanceled=true;
                }else{
                    const auto slot=*s.slot;
                    if(slot>1024||!s.selectionPending||e.pending!=&s||e.pendingIdentity!=h.outerIdentity||
                       e.pendingSlot!=slot||!c.source_.field40||!c.source_.lastUsed38)return stop(0x280cbcu);
                    s.freeHeadBefore=*c.source_.field40;s.lastUsedBefore=*c.source_.lastUsed38;
                    if(*c.source_.field40==slot){if(!e.words||e.members[slot-1])return stop(0x27e594u);
                        c.source_.field40=e.nativeFreeNext[slot-1];}
                    else c.source_.lastUsed38=*c.source_.lastUsed38+1u;
                    s.freeHeadAfter=*c.source_.field40;s.lastUsedAfter=*c.source_.lastUsed38;
                    e.pendingConsumed=true;s.counterAdvanced=true;
                    if(!e.words)return stop(0x27e5a8u);
                    e.members[slot-1]=h.outerIdentity;e.borrowers[slot-1]=&s;e.wordKnown[slot-1]=false;
                    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;
                    s.selectionPending=false;s.inserted=true;s.allocationAttempted=true;
                    const auto allocated=observer.allocateSecondary105720(&s);
                    s.allocationRecorded=observer.secondaryAllocationRecordedNative(&s);
                    if(allocated.status==MembershipObserver::SecondaryLinkStatus::nativeRefused)return stop(allocated.sourcePC);
                    s.allocationReturned=true;s.link=allocated.record;
                    if(!s.link)return stop(0x280cd0u); //Inserted membership survives NULL pool result.
                    s.link->slot14=slot;s.linkStoreMask|=1u;
                    s.link->child04=s.neighbor;s.linkStoreMask|=2u;
                    s.link->collisionMethod08=s.method;s.link->method08=nullptr;s.linkStoreMask|=4u;
                    s.link->portal0C=s.portal;s.linkStoreMask|=8u;
                    if(!s.distance)return stop(0x280ce8u);
                    s.link->distance10=*s.distance;s.linkStoreMask|=16u;
                    s.oldHead=observer.secondaryHeadNative();
                    s.link->next00=static_cast<MembershipObserver::SecondaryLink*>(const_cast<void*>(s.oldHead));s.linkStoreMask|=32u;
                    if(!observer.publishSecondary280CF4(&s,s.link,s.oldHead))return stop(0x280cf4u);
                    s.linkStoreMask|=64u;s.headPublished=true;
                }
            }
            if(!actualPortal(4u,&h,h.outerIdentity,h.zoneIdentity,h.center,p.cursor,s,p.graphPin,p.pc))return stop(p.pc);
            s.complete=true;p.cursor=s.next;
        }
        if(!observer.finishSecondary280D30(&h))return stop(0x280d30u);
        p.returned=true;return stop(0x280d38u); //Genuine portal return only;120070 suffix not claimed.
    }
    static bool membershipPortalsHistoryNative(const SourceCallback& callback)noexcept {
        const auto& h=callback.membership;const auto& p=h.portals;
        if(!p.entered)return !p.helperEntered&&!p.rootRead&&!p.returned&&!p.nativeWithdrawn&&!p.count&&!p.pc&&!p.graphPin;
        if(!h.primaryPublished||p.count>20||!p.pc||(p.returned&&(!p.rootRead||p.cursor||p.pc!=0x280d38u))||
           (p.rootRead&&!p.nativeWithdrawn&&!p.graphPin))return false;
        const auto* observer=h.stack.get();if(!observer)return false;
        if(observer->secondary_.entered!=p.helperEntered||observer->secondary_.returned!=p.returned||
           (p.helperEntered&&observer->secondary_.nativeWithdrawn!=p.nativeWithdrawn))return false;
        if(!p.helperEntered)return !p.rootRead&&!p.graphPin&&!p.count&&!p.returned;
        const void* cursor=p.root;auto* method=h.method;
        for(std::size_t i=0;i<p.count;++i){const auto& s=p.edges[i];
            if(s.previousMethod!=method||!source_portal_overlap231150::historyCurrent(s.overlap)||
               s.overlapReturned!=s.overlap.returned||s.hit!=s.overlap.hit||s.distanceStoreMask>3u||
               (s.entered&&(s.edge!=cursor||!(s.edgeIndex&1u)||s.edgeIndex>=40))||
               (s.distanceStoreMask&&(!s.hit||!s.distance||s.distance!=s.overlap.outputDistance))||
               (s.methodReturned&&(!s.hit||s.distanceStoreMask!=3u))||
               (s.methodPinned&&(!s.methodReturned||!s.method))||(s.methodPinWithdrawn&&!s.methodPinned)||
               (s.selectionAttempted&&!s.methodPinned)||(s.selectionReturned&&(!s.selectionAttempted||!s.slot))||
               (s.counterAdvanced&&!s.selectionReturned)||(s.inserted&&!s.counterAdvanced)||
               (s.allocationAttempted&&!s.inserted)||(s.allocationRecorded&&!s.allocationAttempted)||(s.allocationReturned&&!s.allocationRecorded)||
               (s.headPublished&&(!s.link||s.linkStoreMask!=127u))||(s.slotWithdrawn&&!s.inserted)||
               (s.linkReleased&&!s.allocationAttempted)||(s.selectionCanceled&&s.selectionPending))return false;
            if(s.counterAdvanced){
                if(!s.slot||!*s.slot||*s.slot>1024)return false;
                if(s.freeHeadBefore==*s.slot){if(s.lastUsedAfter!=s.lastUsedBefore||s.freeHeadAfter>1024)return false;}
                else if(s.freeHeadBefore||*s.slot!=s.lastUsedBefore+1u||s.lastUsedAfter!=s.lastUsedBefore+1u||s.freeHeadAfter)return false;
            }
            if(s.methodReturned)method=s.method;
            if(s.methodPinned&&!s.methodPinWithdrawn){const auto& c=*s.method;const auto& e=c.sourceEntries_;
                if(c.application_!=callback.application||c.world_!=h.world||!c.sourceMode_||!c.sourceCollisionEntriesCurrentNative()||c.usage10_!=1u||c.ordinal_/10u!=1u)return false;
                unsigned pins=0;for(auto pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&s)++pins;
                if(pins!=1||s.selectionPending!=(e.pending==&s))return false;
                if(s.selectionPending&&(e.pendingIdentity!=h.outerIdentity||e.pendingConsumed!=s.counterAdvanced||
                    (s.slot&&e.pendingSlot!=*s.slot)))return false;
                if(s.inserted&&!s.slotWithdrawn&&(!s.slot||!*s.slot||*s.slot>1024||
                    e.members[*s.slot-1]!=h.outerIdentity||e.borrowers[*s.slot-1]!=&s||e.wordKnown[*s.slot-1]))return false;
            }
            if(s.link&&!s.linkReleased){if(!observer->ownsSecondaryLinkNative(s.link))return false;
                const auto& l=*s.link;
                if(((s.linkStoreMask&1u)&&(!s.slot||l.slot14!=*s.slot))||((s.linkStoreMask&2u)&&l.child04!=s.neighbor)||
                   ((s.linkStoreMask&4u)&&(l.collisionMethod08!=s.method||l.method08))||
                   ((s.linkStoreMask&8u)&&l.portal0C!=s.portal)||((s.linkStoreMask&16u)&&(!s.distance||l.distance10!=*s.distance))||
                   ((s.linkStoreMask&32u)&&l.next00!=s.oldHead))return false;
            }
            if(s.complete){if(!s.overlapReturned||(s.hit&&(!s.selectionReturned||(*s.slot&&!s.headPublished))))return false;cursor=s.next;}
            else if(i+1!=p.count)return false;
            if(p.nativeWithdrawn&&(s.selectionPending||(s.methodPinned&&!s.methodPinWithdrawn)||
               (s.inserted&&!s.slotWithdrawn)||(s.allocationRecorded&&!s.linkReleased)))return false;
        }
        return cursor==p.cursor&&method==p.cursorMethod&&(!p.nativeWithdrawn||!p.graphPin);
    }
    static bool withdrawSourceMembershipPortalsNative(SourceCallback& callback)noexcept {
        auto& h=callback.membership;auto& p=h.portals;
        if(!p.entered||p.nativeWithdrawn)return true;
        if(!membershipPortalsHistoryNative(callback))return false;
        auto& observer=*h.stack;
        for(std::size_t i=p.count;i>0;--i){auto& s=p.edges[i-1];
            if(s.methodPinned&&!s.methodPinWithdrawn){auto& c=*s.method;auto& e=c.sourceEntries_;
                if(s.selectionPending){if(e.pending!=&s)return false;
                    if(e.pendingConsumed){e.abandonedConsumed=e.pendingSlot;e.terminal=true;e.pendingConsumed=false;}
                    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;s.selectionPending=false;s.selectionCanceled=true;}
                if(s.inserted&&!s.slotWithdrawn){if(!s.slot||!e.words||e.pending||!c.source_.field40||
                    e.members[*s.slot-1]!=h.outerIdentity||e.borrowers[*s.slot-1]!=&s)return false;
                    e.nativeFreeNext[*s.slot-1]=*c.source_.field40;e.members[*s.slot-1]=e.borrowers[*s.slot-1]=nullptr;
                    e.wordKnown[*s.slot-1]=false;c.source_.field40=*s.slot;s.slotWithdrawn=true;}
            }
            if(s.allocationRecorded&&!s.linkReleased){if(!observer.releaseSecondaryNative(&s))return false;s.linkReleased=true;}
            if(s.methodPinned&&!s.methodPinWithdrawn){auto& pins=s.method->sourceCollisionRegistrationBorrowers_;
                auto j=pins.begin();while(j!=pins.end()&&*j!=&s)++j;if(j==pins.end())return false;
                pins.erase(j);s.methodPinWithdrawn=true;}
        }
        if(p.helperEntered&&!observer.withdrawSecondaryNative(&h))return false;
        //All graph-derived addresses now historical opaque tokens; never dereferenced after this reset.
        p.graphPin.reset();p.nativeWithdrawn=true;return true;
    }
    static bool membershipReturnHistoryNative(const SourceCallback& callback)noexcept {
        const auto& h=callback.membership;const auto& f=h.finish;
        if(!MembershipGuards::zoneHistory(f.zoneGuard))return false;
        if(!f.entered)return !h.returned&&!f.returned&&!f.selectedIdentity&&!f.zoneRead&&
            !f.stackDestroyCalled&&!f.stackDestroyReturned&&!f.pc;
        if(!h.portals.returned||!h.stack||!f.observerReturned||f.observerReturnPC!=0x280becu||
           f.selectedIdentity!=h.stack->identity()||f.returned!=h.returned||f.pc!=h.pc)return false;
        if(!f.zoneGuard.returned)return !f.stackDestroyCalled&&!f.stackDestroyReturned&&!f.returned&&
            f.pc==f.zoneGuard.pc&&(!f.zoneRead||f.zone04==h.stack->record_.zone04);
        if(!f.zoneRead||!f.zone04||f.zone04!=h.stack->record_.zone04||!f.stackDestroyCalled||f.stackArgument!=-1)return false;
        if(!f.stackDestroyReturned)return !f.returned&&!f.stackReturnPC&&f.pc==0x280920u&&!h.stack->destroyEntered_;
        return f.returned&&f.pc==0x120224u&&f.stackReturnPC==0x2809b8u&&h.stack->detached_.entered&&
            h.stack->destroyEntered_&&h.stack->destroyReturned_&&h.stack->countDecremented_&&
            h.stack->destroyPC_==f.stackReturnPC&&h.stack->destructionArgument_==-1&&!h.stack->sourceDeleted_&&!h.stack->lease_;
    }
    static bool membershipPrimaryHistoryNative(const SourceCallback& callback)noexcept {
        const auto& h=callback.membership;
        if(!membershipReturnHistoryNative(callback))return false;
        if(!h.primaryCalled)return !h.methodResolved&&!h.methodPinned&&!h.methodPinWithdrawn&&!h.method&&!h.slot&&
            !h.selectionAttempted&&!h.selectionReturned&&!h.selectionPending&&!h.selectionCanceled&&!h.selectionReturnPC&&
            !h.insertEntered&&!h.counterAdvanced&&!h.inserted&&!h.insertionReturnPC&&!h.slotWithdrawn&&
            !h.primaryPublished&&!h.primaryNativeWithdrawn&&h.pc==0x2809c0u;
        const auto* observer=h.stack.get();
        if(!h.bounds.returned||!observer||!observer->preflightNative())return false;
        const auto& o=observer->primary_;
        if(!o.entered)return !h.methodResolved&&!h.methodPinned&&!h.selectionAttempted&&!h.insertEntered&&h.pc==0x2809c0u;
        if(o.occurrence!=&h||o.zone!=h.zoneIdentity||o.center!=h.center||o.nativeWithdrawn!=h.primaryNativeWithdrawn||
           (h.methodPinned&&(!h.methodResolved||!h.method))||(h.methodPinWithdrawn&&!h.methodPinned)||
           (h.selectionAttempted&&(!h.methodPinned||!o.methodAssertion.passed))||
           (h.selectionReturned&&(!h.selectionAttempted||!h.slot||h.selectionReturnPC!=0x27e568u))||
           (!h.selectionReturned&&h.selectionReturnPC)||
           (h.insertEntered&&(!h.selectionReturned||!o.slotAssertion.passed))||
           (h.counterAdvanced&&!h.insertEntered)||(h.inserted&&(!h.counterAdvanced||h.insertionReturnPC!=0x27e5acu))||
           (!h.inserted&&h.insertionReturnPC)||(h.slotWithdrawn&&!h.inserted)||
           (h.primaryPublished&&(!h.inserted||o.stores!=31u||(!h.portals.entered&&h.pc!=0x280bf0u)))||
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
            if(c.application_!=callback.application||c.world_!=h.world||!c.sourceMode_||c.usage10_!=1u||
               c.ordinal_/10u!=1u||c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())return false;
            unsigned pins=0;for(const auto* pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&h)++pins;
            if(pins!=1||h.selectionPending!=(e.pending==&h))return false;
            if(h.selectionPending&&(e.pendingIdentity!=h.outerIdentity||
                (h.selectionReturned&&e.pendingSlot!=*h.slot)||e.pendingConsumed!=h.counterAdvanced))return false;
            if(h.inserted&&!h.slotWithdrawn&&(!h.slot||!*h.slot||*h.slot>1024||
                e.members[*h.slot-1]!=h.outerIdentity||e.borrowers[*h.slot-1]!=&h||e.wordKnown[*h.slot-1]))return false;
        }else if(h.methodPinWithdrawn&&(!h.primaryNativeWithdrawn||h.selectionPending||(h.inserted&&!h.slotWithdrawn)))return false;
        if(h.nativeWithdrawn&&!h.primaryNativeWithdrawn)return false;
        return membershipPortalsHistoryNative(callback); //No dereference after exact native unpin.
    }
    static bool withdrawSourceMembershipPrimaryNative(SourceCallback& callback)noexcept {
        auto& h=callback.membership;if(!h.primaryCalled||h.primaryNativeWithdrawn)return true;
        if(!membershipPrimaryHistoryNative(callback)||!withdrawSourceMembershipPortalsNative(callback))return false;
        auto* observer=h.stack.get();
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
    template<class ActualMethod,class ActualPortal>
    static bool continueSourceStaticMembershipNative(SourceCallback& callback,
        const ::ra_pod_scene50LA::Owner* world,ActualMethod&& actualMethod,ActualPortal&& actualPortal,
        std::uint32_t& pc)noexcept {
        auto& h=callback.membership;
        const auto stop=[&](std::uint32_t p){pc=callback.pc=h.pc=p;return h.returned;};
        if(!world||!h.bounds.returned||h.pc!=0x2809c0u||h.primaryCalled||!h.stack||
           !membershipCurrentNative(callback))return false;
        h.outerIdentity=callback.outer.get();h.world=world;
        h.pc=0x2809c0u;h.primaryCalled=true;
        auto& observer=*h.stack;
        if(!observer.beginRegistration2809C0(&h,h.zoneIdentity,h.center))
            return stop(observer.registrationPC()?observer.registrationPC():0x2809c0u);
        OriginalNativeListMethodCustody50LA* resolved=nullptr;
        h.pc=0x27f5a0u;
        if(!actualMethod(h.zoneIdentity,&h,h.outerIdentity,resolved,h.slot,h.selectionPending,h.pc,false))
            return stop(h.pc);
        h.methodResolved=true;h.method=resolved;
        //Owner-aware native pin is independent of source refcount/usage. It
        //fences method destruction even before selection acquires a pending slot.
        h.pc=0x280a80u;
        if(h.method){
            auto& c=*h.method;
            if(c.application_!=callback.application||c.world_!=h.world||!c.sourceMode_||c.usage10_!=1u||
               c.ordinal_/10u!=1u||c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())
                return stop(h.pc);
            for(const auto* pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&h)return stop(h.pc);
            try{c.sourceCollisionRegistrationBorrowers_.push_back(&h);}catch(...){return stop(h.pc);}
            h.methodPinned=true;
        }
        if(!observer.recordMethod280A80(h.method))return stop(observer.registrationPC());
        h.selectionAttempted=true;h.pc=0x280afcu;
        if(!actualMethod(h.zoneIdentity,&h,h.outerIdentity,h.method,h.slot,h.selectionPending,h.pc,true))
            return stop(h.pc);
        h.selectionReturned=true;h.selectionReturnPC=0x27e568u;
        if(!h.slot||!observer.recordSelection280B08(*h.slot))
            return stop(h.slot?observer.registrationPC():0x280b08u);
        //Authenticated table3D2D30+24=27E570; exact selected slot is not a
        //host-selected index or a source DWORD made from a native address.
        h.insertEntered=true;h.pc=0x280b88u;
        auto& c=*h.method;auto& e=c.sourceEntries_;auto& state=c.source_;
        const auto slot=*h.slot;
        if(!slot||slot>1024||!state.field40||!state.lastUsed38||e.pending!=&h||
           e.pendingIdentity!=h.outerIdentity||e.pendingSlot!=slot||!h.selectionPending)
            return stop(h.pc);
        h.freeHeadBefore=*state.field40;h.lastUsedBefore=*state.lastUsed38;
        if(*state.field40==slot){h.pc=0x27e594u;
            if(!e.words||e.members[slot-1])return stop(h.pc);
            state.field40=e.nativeFreeNext[slot-1];}
        else {h.pc=0x27e5a0u;state.lastUsed38=*state.lastUsed38+1u;}
        h.freeHeadAfter=*state.field40;h.lastUsedAfter=*state.lastUsed38;
        e.pendingConsumed=true;h.counterAdvanced=true;h.pc=0x27e5a8u;
        if(!e.words)return stop(h.pc);
        e.members[slot-1]=h.outerIdentity;e.borrowers[slot-1]=&h;e.wordKnown[slot-1]=false;
        e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;
        h.selectionPending=false;h.inserted=true;h.insertionReturnPC=0x27e5acu;
        if(!observer.publishPrimary280B90())return stop(observer.registrationPC());
        h.primaryPublished=true;h.pc=0x280bf0u;
        continueSourceMembershipPortals280BF0(callback,actualPortal);
        if(!h.portals.returned)return stop(h.pc);
        auto& f=h.finish;f.entered=true;f.selectedIdentity=h.stack->identity();
        f.observerReturned=true;f.observerReturnPC=0x280becu;
        f.pc=h.pc=0x120190u;
        if(!MembershipGuards::beginZone(f.zoneGuard))return stop(f.pc=f.zoneGuard.pc);
        f.pc=h.pc=0x1201bcu;f.zone04=h.stack->record_.zone04;f.zoneRead=true;
        if(!MembershipGuards::finishZone(f.zoneGuard,f.zone04!=nullptr))return stop(f.pc=f.zoneGuard.pc);
        f.pc=h.pc=0x120200u;f.stackDestroyCalled=true;f.stackArgument=-1;
        //Zero-slot portal paths have no secondary record; their native pins
        //need a separately authenticated transfer. Keep those paths bounded.
        for(std::size_t i=0;i<h.portals.count;++i){const auto& e=h.portals.edges[i];
            if(e.methodPinned&&!e.inserted)return stop(f.pc=0x280920u);}
        if(!h.stack->destroyRegisteredMode1Temporary280920(&h,h.outerIdentity,h.zone354,world,callback.application))
            return stop(f.pc=0x280920u);
        f.stackDestroyReturned=true;f.stackReturnPC=h.stack->destroyPC_;
        f.returned=h.returned=true;f.pc=0x120224u;return stop(0x120224u);
    }
