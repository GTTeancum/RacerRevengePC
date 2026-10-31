    using ModelObserver=OriginalSourceCollisionObserver50LA;
    using ObserverShared=OriginalGarageObserverShared50KG;
    std::unique_ptr<ModelObserver> modelObserverStorage_;
    Scene::SourceObserverBounds28E230 modelObserverBounds_;
    struct SourceModelObserver28E8A0 {
        const void* updateOccurrence=nullptr;
        const void* ownerIdentity=nullptr;
        const void* zone=nullptr;
        const void* before=nullptr;
        const void* published=nullptr;
        std::uint32_t mode=0,pc=0;
        bool entered=false,returned=false,allocationCalled=false,allocationReturned=false;
        bool constructorCalled=false,constructorReturned=false,pointerStored=false;
        bool scratchInitialized=false,boundsCallReached=false,boundsCompleted=false,boundsReturned=false;
        bool nativePointerCleared=false,nativeWithdrawn=false;
        ObserverShared::SourceAssertionHistory allocationAssertion;
        std::array<std::uint32_t,4> center{},half{};
    } modelObserver_;
    const void* sourceModelObserverCurrentNative()const noexcept {
        const auto& h=modelObserver_;
        return h.entered&&h.pointerStored&&!h.nativePointerCleared?h.published:nullptr;
    }
    Result beginSourceModelObserver28E8A0(const void* actualUpdateOccurrence,
        const void* actualVisibilityB4,std::uint32_t actualMode)noexcept {
        auto& h=modelObserver_;
        if(h.entered||!actualUpdateOccurrence||actualMode!=2u||nativeWithdrawn_||modelRetired_||ledgerWithdrawn_||
           sourceSceneReleased_||sourceOuterDeleted_||detachedNative_||!base_||!base_->object_||!model_||
           !bodyModelBorrower_||!base_->object_->visibilityB4||*base_->object_->visibilityB4!=actualVisibilityB4||
           !postE4WorldCallEntered_||!postE4World_.returned||postE4WorldCallPC_!=0x28b67cu||
           postE4WorldUpdateOccurrence_!=actualUpdateOccurrence||!nullObserver_.returned||
           nullObserver_.updateOccurrence!=actualUpdateOccurrence||model_->model_.observer19C||
           model_->sourceObserverBorrower_||!packetsReturned_||sourcePackets_.sourcePackets_.nativeWithdrawn||
           !preflightNative()||base_->object_->model14C!=static_cast<const void*>(&model_->model_)||
           !model_->preflightSourceRenderableReturn26E820Native())return {Status::owner,0x28e8a0u};
        //Caller authenticates E4 then E0 full120224 returns before166750.
        //This is actual BaseB4, not collisionB8 or a prepared visibility node.
        h.entered=true;h.updateOccurrence=actualUpdateOccurrence;h.ownerIdentity=&model_->model_;
        h.zone=actualVisibilityB4;h.mode=actualMode;model_->sourceObserverBorrower_=&h;
        h.pc=0x28e8b8u;h.before=model_->model_.observer19C; //Fresh qualified NULL branch only.
        h.pc=0x28e8d0u;h.allocationCalled=true;
        modelObserverStorage_.reset(new(std::nothrow) ModelObserver);h.allocationReturned=true;
        if(modelObserverStorage_){
            h.constructorCalled=true;h.pc=0x280890u;
            if(!modelObserverStorage_->construct280890(base_->application_->garageObserverShared_))
                return {Status::bounded,h.pc=modelObserverStorage_->pc()};
            h.constructorReturned=true;
        }
        //Nullable malloc30 return is published evenNULL; prior completed ctor
        //effects survive a later assertion or fresh-bounds admission refusal.
        h.pc=0x28e8e8u;h.published=modelObserverStorage_?modelObserverStorage_->identity():nullptr;
        model_->model_.observer19C=h.published;h.pointerStored=true;
        h.pc=0x28e8ecu;if(!ObserverShared::beginSourceAssertion(0,h.allocationAssertion))return {Status::bounded,h.pc};
        h.pc=0x28e924u;if(!ObserverShared::finishSourceAssertion(h.allocationAssertion,h.published!=nullptr))
            return {Status::bounded,h.pc};
        h.pc=0x28e958u;h.center={{0,0,0,0x3f800000u}};
        h.pc=0x28e960u;h.half={{0,0,0,0x3f800000u}};h.scratchInitialized=true;
        h.boundsCallReached=true;h.pc=0x28e964u;
        const auto bounds=model_->produceSourceObserverBounds28E230(graph(),h.center,h.half,modelObserverBounds_);
        h.boundsCompleted=true;h.pc=bounds.sourcePC;
        if(!bounds.okay())return {Status::bounded,h.pc};
        h.boundsReturned=true;h.pc=0x28e96cu;
        return {Status::bounded,h.pc}; //Fresh bounds returned; registration2809C0 remains unissued.
    }
    bool sourceModelObserverHistoryNative()const noexcept {
        const auto& h=modelObserver_;
        if(!h.entered)return !modelObserverStorage_&&!h.updateOccurrence&&!h.ownerIdentity&&!h.zone&&!h.before&&
            !h.published&&!h.mode&&!h.pc&&!h.returned&&!h.allocationCalled&&!h.allocationReturned&&
            !h.constructorCalled&&!h.constructorReturned&&!h.pointerStored&&!h.scratchInitialized&&!h.boundsCallReached&&
            !h.boundsCompleted&&!h.boundsReturned&&!modelObserverBounds_.entered&&
            !h.nativePointerCleared&&!h.nativeWithdrawn&&ObserverShared::sourceAssertionHistoryCurrent(h.allocationAssertion);
        if(!h.updateOccurrence||!h.ownerIdentity||h.mode!=2u||h.before||h.returned||
           !postE4World_.returned||h.updateOccurrence!=postE4WorldUpdateOccurrence_||
           !nullObserver_.returned||h.updateOccurrence!=nullObserver_.updateOccurrence||
           !h.allocationCalled||!h.allocationReturned||h.constructorCalled!=bool(modelObserverStorage_)||
           h.constructorReturned!=bool(modelObserverStorage_&&modelObserverStorage_->returned())||
           (modelObserverStorage_&&!modelObserverStorage_->preflightNative())||
           !ObserverShared::sourceAssertionHistoryCurrent(h.allocationAssertion)||
           (h.nativePointerCleared&&!h.pointerStored))return false;
        if(h.nativeWithdrawn&&((h.pointerStored&&!h.nativePointerCleared)||
           (modelObserverStorage_&&!modelObserverStorage_->nativeWithdrawn_)))return false;
        if(nativeWithdrawn_){if(!h.nativeWithdrawn)return false;}
        else if(!base_||!base_->object_||!model_||h.ownerIdentity!=&model_->model_||
                !base_->object_->visibilityB4||h.zone!=*base_->object_->visibilityB4||
                model_->model_.observer19C!=sourceModelObserverCurrentNative()||
                model_->sourceObserverBorrower_!=(h.nativeWithdrawn?nullptr:&h))return false;
        if(modelObserverStorage_&&!h.constructorReturned)return !h.pointerStored&&!h.published&&
            !h.allocationAssertion.entered&&!h.scratchInitialized&&!h.boundsCallReached&&h.pc==modelObserverStorage_->pc();
        if(!h.pointerStored||h.published!=(modelObserverStorage_?modelObserverStorage_->identity():nullptr)||
           !h.allocationAssertion.entered)return false;
        if(!h.allocationAssertion.passed)return !h.scratchInitialized&&!h.boundsCallReached&&
            (h.pc==0x28e8ecu||h.pc==0x28e924u);
        if(!h.scratchInitialized||!h.boundsCallReached||!h.boundsCompleted)return false;
        const auto& b=modelObserverBounds_;
        const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
        if(!b.entered)return !h.boundsReturned&&h.pc==0x28e230u&&h.center==vf0&&h.half==vf0;
        if(b.centerBefore!=vf0||b.halfBefore!=vf0||h.center!=b.centerAfter||h.half!=b.halfAfter||
           h.boundsReturned!=b.returned||h.pc!=(b.returned?0x28e96cu:b.pc))return false;
        //After Model native storage is erased, only retained opaque occurrence
        //and output history remain. Live/partial cleanup still pins every byte.
        return nativeWithdrawn_?h.nativeWithdrawn:model_->sourceObserverBoundsHistoryNative(graph(),b);
    }
    bool withdrawSourceModelObserverNative()noexcept {
        auto& h=modelObserver_;if(!h.entered||h.nativeWithdrawn)return true;
        if(!sourceModelObserverHistoryNative()||!base_||!model_||!preflightNative())return false;
        //This newest shared-cohort child must retire before collision E0/E4
        //observers. No guest280920/19C=NULL or registration return is claimed.
        if(modelObserverStorage_&&!modelObserverStorage_->withdrawNative())return false;
        if(h.pointerStored){model_->model_.observer19C=nullptr;h.nativePointerCleared=true;}
        model_->sourceObserverBorrower_=nullptr;h.nativeWithdrawn=true;return true;
    }
