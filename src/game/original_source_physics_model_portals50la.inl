    //Stable per-edge addresses are occurrence tokens; no callback survives this call.
    struct SourceModelPortalEdge {
        const void* edge=nullptr;const void* next=nullptr;
        const C* neighbor=nullptr;const C* portal=nullptr;
        OriginalNativeListMethodCustody50LA* previousMethod=nullptr;
        OriginalNativeListMethodCustody50LA* method=nullptr;
        std::optional<std::uint32_t> slot,distance;
        source_portal_overlap231150::History overlap;
        unsigned edgeIndex=0;std::uint8_t distanceStoreMask=0,linkStoreMask=0;
        bool entered=false,overlapReturned=false,hit=false,methodReturned=false,selectionPending=false;
        bool methodPinned=false,methodPinWithdrawn=false,selectionAttempted=false,selectionReturned=false,selectionCanceled=false;
        bool counterAdvanced=false,inserted=false,slotWithdrawn=false,allocationAttempted=false,allocationRecorded=false,allocationReturned=false;
        bool headPublished=false,linkReleased=false,complete=false;
        std::uint32_t freeHeadBefore=0,lastUsedBefore=0,freeHeadAfter=0,lastUsedAfter=0;
        ModelObserver::SecondaryLink* link=nullptr;const void* oldHead=nullptr;
    };
    struct SourceModelPortals280BF0 {
        bool entered=false,helperEntered=false,rootRead=false,returned=false,nativeWithdrawn=false;
        std::uint32_t pc=0,observerReturnPC=0;std::size_t count=0;
        bool observerReturned=false;
        const void* root=nullptr;const void* cursor=nullptr;
        OriginalNativeListMethodCustody50LA* cursorMethod=nullptr;
        std::shared_ptr<const void> graphPin;
        std::array<SourceModelPortalEdge,20> edges{};
    } modelPortals_;
    template<class ActualPortal>
    Result continueSourceModelPortals280BF0(ActualPortal&& actualPortal)noexcept {
        auto& h=modelObserver_;auto& p=modelPortals_;
        if(!h.primaryPublished||h.returned||h.nativeWithdrawn||p.entered||!modelObserverStorage_||
           h.pc!=0x280bf0u||!sourceModelPrimaryHistoryNative()||!preflightNative())
            return {Status::owner,0x280bf0u};
        auto& observer=*modelObserverStorage_;
        const auto stop=[&](std::uint32_t pc){h.pc=p.pc=pc;return Result{Status::bounded,pc};};
        p.entered=true;p.pc=0x280bf0u;
        if(!observer.beginSecondary280BF0(&h))return stop(p.pc);
        p.helperEntered=true;p.cursorMethod=h.method;
        SourceModelPortalEdge initial;
        if(!actualPortal(0u,&h,h.ownerIdentity,h.zone,h.center,nullptr,initial,p.graphPin,p.pc))return stop(p.pc);
        p.rootRead=true;p.root=p.cursor=initial.next;
        while(p.cursor){
            if(p.count==p.edges.size())return stop(0x280c40u);
            auto& s=p.edges[p.count++];s.previousMethod=p.cursorMethod;
            const bool edgeOkay=actualPortal(1u,&h,h.ownerIdentity,h.zone,h.center,p.cursor,s,p.graphPin,p.pc);
            if(s.methodReturned)p.cursorMethod=s.method;
            if(!edgeOkay)return stop(p.pc);
            if(s.hit){
                p.cursorMethod=s.method;
                if(!s.methodReturned||!s.method)return stop(0x280c78u);
                auto& c=*s.method;
                if(c.application_!=base_->application_.get()||!c.sourceMode_||c.usage10_!=2u||c.ordinal_/10u!=2u||
                   c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative())return stop(0x280c90u);
                try{c.sourceCollisionRegistrationBorrowers_.push_back(&s);}catch(...){return stop(0x280c90u);}
                s.methodPinned=true;s.selectionAttempted=true;
                if(!actualPortal(2u,&h,h.ownerIdentity,h.zone,h.center,p.cursor,s,p.graphPin,p.pc))return stop(p.pc);
                s.selectionReturned=true;
                if(!s.slot)return stop(0x280c9cu);
                auto& e=c.sourceEntries_;
                if(!*s.slot){
                    if(e.pending!=&s||e.pendingIdentity!=h.ownerIdentity||e.pendingSlot)return stop(0x280c9cu);
                    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;s.selectionPending=false;s.selectionCanceled=true;
                }else{
                    const auto slot=*s.slot;
                    if(slot>1024||!s.selectionPending||e.pending!=&s||e.pendingIdentity!=h.ownerIdentity||
                       e.pendingSlot!=slot||!c.source_.field40||!c.source_.lastUsed38)return stop(0x280cbcu);
                    s.freeHeadBefore=*c.source_.field40;s.lastUsedBefore=*c.source_.lastUsed38;
                    if(*c.source_.field40==slot){if(!e.words||e.members[slot-1])return stop(0x27e594u);
                        c.source_.field40=e.nativeFreeNext[slot-1];}
                    else c.source_.lastUsed38=*c.source_.lastUsed38+1u;
                    s.freeHeadAfter=*c.source_.field40;s.lastUsedAfter=*c.source_.lastUsed38;
                    e.pendingConsumed=true;s.counterAdvanced=true;
                    if(!e.words)return stop(0x27e5a8u);
                    e.members[slot-1]=h.ownerIdentity;e.borrowers[slot-1]=&s;e.wordKnown[slot-1]=false;
                    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;
                    s.selectionPending=false;s.inserted=true;s.allocationAttempted=true;
                    const auto allocated=observer.allocateSecondary105720(&s);
                    s.allocationRecorded=observer.secondaryAllocationRecordedNative(&s);
                    if(allocated.status==ModelObserver::SecondaryLinkStatus::nativeRefused)return stop(allocated.sourcePC);
                    s.allocationReturned=true;s.link=allocated.record;
                    if(!s.link)return stop(0x280cd0u); //Inserted membership survives NULL pool result.
                    s.link->slot14=slot;s.linkStoreMask|=1u;
                    s.link->child04=s.neighbor;s.linkStoreMask|=2u;
                    s.link->collisionMethod08=s.method;s.link->method08=nullptr;s.linkStoreMask|=4u;
                    s.link->portal0C=s.portal;s.linkStoreMask|=8u;
                    if(!s.distance)return stop(0x280ce8u);
                    s.link->distance10=*s.distance;s.linkStoreMask|=16u;
                    s.oldHead=observer.secondaryHeadNative();
                    s.link->next00=static_cast<ModelObserver::SecondaryLink*>(const_cast<void*>(s.oldHead));s.linkStoreMask|=32u;
                    if(!observer.publishSecondary280CF4(&s,s.link,s.oldHead))return stop(0x280cf4u);
                    s.linkStoreMask|=64u;s.headPublished=true;
                }
            }
            if(!actualPortal(4u,&h,h.ownerIdentity,h.zone,h.center,p.cursor,s,p.graphPin,p.pc))return stop(p.pc);
            s.complete=true;p.cursor=s.next;
        }
        if(!observer.finishSecondary280D30(&h))return stop(0x280d30u);
        p.returned=true;p.pc=0x280d38u; //Actual280D30 JR/delay280D34 completed.
        p.observerReturned=true;p.observerReturnPC=0x280becu; //280BE4 JR/delay280BE8.
        h.pc=0x280becu;
        return {Status::bounded,h.pc}; //Late28E8A0 zoneguard and28EA14 remain unissued.
    }
    bool sourceModelPortalsHistoryNative()const noexcept {
        const auto& p=modelPortals_;const auto& h=modelObserver_;
        if(!p.entered)return !p.helperEntered&&!p.rootRead&&!p.returned&&!p.nativeWithdrawn&&!p.count&&!p.pc&&!p.graphPin&&!p.root&&!p.cursor&&!p.cursorMethod&&
            !p.observerReturned&&!p.observerReturnPC;
        if(!h.primaryPublished||p.count>20||!p.pc||p.returned!=p.observerReturned||
           (p.observerReturned?p.observerReturnPC!=0x280becu:p.observerReturnPC!=0u)||
           (!h.returnEntered&&h.pc!=(p.observerReturned?p.observerReturnPC:p.pc))||(p.returned&&(!p.rootRead||p.cursor||p.pc!=0x280d38u))||
           (p.rootRead&&!p.nativeWithdrawn&&!p.graphPin))return false;
        const auto* observer=modelObserverStorage_.get();if(!observer)return false;
        if(observer->secondary_.entered!=p.helperEntered||observer->secondary_.returned!=p.returned||
           (p.helperEntered&&observer->secondary_.nativeWithdrawn!=p.nativeWithdrawn))return false;
        if(!p.helperEntered)return !p.rootRead&&!p.graphPin&&!p.count&&!p.returned;
        const void* cursor=p.root;auto* method=h.method;
        for(std::size_t i=0;i<p.count;++i){const auto& s=p.edges[i];
            if(s.previousMethod!=method||!source_portal_overlap231150::historyCurrent(s.overlap)||
               s.overlapReturned!=s.overlap.returned||s.hit!=s.overlap.hit||s.distanceStoreMask>3u||
               (s.entered&&(s.edge!=cursor||(s.edgeIndex&1u)||s.edgeIndex>=40))||
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
                if(!c.sourceCollisionEntriesCurrentNative()||c.usage10_!=2u||c.ordinal_/10u!=2u)return false;
                unsigned pins=0;for(auto pin:c.sourceCollisionRegistrationBorrowers_)if(pin==&s)++pins;
                if(pins!=1||s.selectionPending!=(e.pending==&s))return false;
                if(s.selectionPending&&(e.pendingIdentity!=h.ownerIdentity||e.pendingConsumed!=s.counterAdvanced||
                    (s.slot&&e.pendingSlot!=*s.slot)))return false;
                if(s.inserted&&!s.slotWithdrawn&&(!s.slot||!*s.slot||*s.slot>1024||
                    e.members[*s.slot-1]!=h.ownerIdentity||e.borrowers[*s.slot-1]!=&s||e.wordKnown[*s.slot-1]))return false;
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
    bool withdrawSourceModelPortalsNative()noexcept {
        auto& p=modelPortals_;auto& h=modelObserver_;
        if(!p.entered||p.nativeWithdrawn)return true;
        if(!sourceModelPortalsHistoryNative())return false;
        auto& observer=*modelObserverStorage_;
        for(std::size_t i=p.count;i>0;--i){auto& s=p.edges[i-1];
            if(s.methodPinned&&!s.methodPinWithdrawn){auto& c=*s.method;auto& e=c.sourceEntries_;
                if(s.selectionPending){if(e.pending!=&s)return false;
                    if(e.pendingConsumed){e.abandonedConsumed=e.pendingSlot;e.terminal=true;e.pendingConsumed=false;}
                    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;s.selectionPending=false;s.selectionCanceled=true;}
                if(s.inserted&&!s.slotWithdrawn){if(!s.slot||!e.words||e.pending||!c.source_.field40||
                    e.members[*s.slot-1]!=h.ownerIdentity||e.borrowers[*s.slot-1]!=&s)return false;
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
