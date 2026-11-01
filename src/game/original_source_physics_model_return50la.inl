    //Original28E98C process guard GP8C44/443408: same shared index4 as Standard.
    Result continueSourceModelReturn28E98C()noexcept {
        auto& h=modelObserver_;const auto& p=modelPortals_;
        if(!h.entered||h.returnEntered||h.returned||h.nativeWithdrawn||nativeWithdrawn_||
           !h.primaryPublished||!p.returned||p.nativeWithdrawn||p.pc!=0x280d38u||
           !p.observerReturned||p.observerReturnPC!=0x280becu||h.pc!=0x280becu||
           !model_||!modelObserverStorage_||!sourceModelObserverHistoryNative()||!preflightNative())
            return {Status::owner,0x28e98cu};
        h.returnEntered=true;h.pc=0x28e98cu;
        if(!ObserverShared::beginSourceAssertion(4,h.returnAssertion))return {Status::bounded,h.pc};
        if(h.returnAssertion.enabled){
            h.pc=0x28e9b8u;h.returnObserver19C=model_->model_.observer19C;h.returnObserverRead=true;
            h.pc=0x28e9c8u;
            if(!h.returnObserver19C||h.returnObserver19C!=modelObserverStorage_->identity())
                return {Status::bounded,h.pc};
            h.returnZone04=modelObserverStorage_->record_.zone04;h.returnZoneRead=true;
        }
        h.pc=h.returnAssertion.enabled?0x28e9ccu:0x28e9fcu; //Disabled guard skips the predicate call.
        if(!ObserverShared::finishSourceAssertion(h.returnAssertion,h.returnZone04!=nullptr))
            return {Status::bounded,h.pc};
        //Actual28E9FC..28EA10 restores registers and JR/delay. No defined V0.
        h.returned=true;h.pc=0x28ea14u;return {Status::reached,h.pc};
    }
    bool sourceModelReturnHistoryNative()const noexcept {
        const auto& h=modelObserver_;const auto& p=modelPortals_;
        if(!ObserverShared::sourceAssertionHistoryCurrent(h.returnAssertion))return false;
        if(!h.returnEntered)return !h.returned&&!h.returnObserverRead&&!h.returnZoneRead&&
            !h.returnObserver19C&&!h.returnZone04&&!h.returnAssertion.entered;
        if(!h.primaryPublished||!p.returned||p.pc!=0x280d38u||!p.observerReturned||
           p.observerReturnPC!=0x280becu||!modelObserverStorage_||!h.returnAssertion.entered||
           h.returned!=h.returnAssertion.passed)return false;
        if(!h.returnAssertion.ready)return !h.returnObserverRead&&!h.returnZoneRead&&
            !h.returnObserver19C&&!h.returnZone04&&!h.returned&&h.pc==0x28e98cu;
        if(h.returnAssertion.enabled){
            if(!h.returnObserverRead||h.returnObserver19C!=h.published)return false;
            if(!h.returnZoneRead)return !h.returnZone04&&!h.returned&&h.pc==0x28e9c8u;
            if(h.returnZone04!=modelObserverStorage_->record_.zone04||h.returnZone04!=h.zone)return false;
        }else if(h.returnObserverRead||h.returnZoneRead||h.returnObserver19C||h.returnZone04)return false;
        return h.returned?h.pc==0x28ea14u:(!h.returnAssertion.passed&&h.pc==0x28e9ccu);
    }
