namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceStandardObserversNative()const noexcept {
    using Shared=g::OriginalGarageObserverShared50KG;
    for(const auto& owned:sourceModelObservers_){
        if(!owned)return false;const auto& f=*owned;
        if(!f.entered||!f.occurrence||!f.wrapper||!f.scene||!f.member||f.zone>=10||f.scalarMask>63||f.primaryMask>31)return false;
        for(const auto* h:{&f.allocationAssertion,&f.childAssertion,&f.methodAssertion,&f.slotAssertion,&f.returnAssertion})
            if(!Shared::sourceAssertionHistoryCurrent(*h))return false;
        if(f.returned&&(!f.secondaryReturned||!f.returnAssertion.passed))return false;
        if(f.constructorReturned&&(!f.lease||!f.vectorKnown||f.scalarMask!=63))return false;
        if(f.published&&(!f.outerReturned||(f.owner&&!f.constructorReturned)))return false;
        if(f.boundsEntered&&(!f.published||!f.allocationAssertion.passed))return false;
        if(f.inserted&&(!f.boundsReturned||!f.slotAssertion.passed))return false;
        if(f.withdrawn){if(f.owner||f.node||f.method||f.selectionPending)return false;continue;}
        if(!sourceStaticOwner_||!worldApplication_)return false;
        const ra_static_model_cold50LA::Owner::SourceModelFrame* model=nullptr;
        for(const auto& row:sourceStaticOwner_->sourceWrapperLedger_)for(const auto& w:row)
            if(w&&w.get()==f.wrapper&&w->sourceModel.get()==f.occurrence)model=w->sourceModel.get();
        if(!model||model->nativeRetired||&model->scene!=f.scene||f.member!=&model->scene.model_||
           !model->packetCallReturned||!model->sourcePackets.sourcePackets_.returned||
           model->sourcePackets.sourcePackets_.nativeWithdrawn||!model->sourcePackets.preflightSourcePacketsNative()||
           model->loaderReturnedEntity!=&model->scene.model_.entity8||
           !model->placement.orientationReturned||!model->scene.preflightSourceRenderableReturn26E820Native()||
           model->sourceObserverOccurrence!=&f||model->scene.sourceObserverBorrower_!=&f||model->sourceObserver19C!=(f.published?f.owner.get():nullptr)||
           model->scene.model_.observer19C!=model->sourceObserver19C)return false;
        if(!sourceVisibility4C0_||!*sourceVisibility4C0_||f.node!=(*sourceVisibility4C0_)[f.zone]||!f.node||
           !sourcePortalWorldLinksCurrent(*f.node,false,f.zone))return false;
        if(!preflightSourceStandardObserverSecondaryNative(f))return false;
        if(f.boundsReturned&&(!f.boundsEntered||!f.scene->sourceBounds_.returned))return false;
        if(f.boundsEntered&&!f.scene->preflightSourceBoundsHistoryNative())return false;
        if(f.primaryMask&&!f.inserted)return false;
        const auto* o=f.owner.get();
        if(o){if(!f.outerReturned)return false;auto& shared=worldApplication_->garageObserverShared_;
            if(f.lease&&(f.constructorReturned?!shared.ownsReturnedSourceObserver(o):!shared.sourceObserverCurrent280890(o)))return false;
            if(o->child04!=((f.primaryMask&1)?&f.node->entity:nullptr)||
               o->method08!=((f.primaryMask&2)?f.method:nullptr)||
               o->slot14!=((f.primaryMask&4)&&f.slot?*f.slot:0u)||o->field0C||o->field10)return false;
            for(const auto& word:o->unknown18To1C)if(word)return false;
            if(f.vectorKnown&&o->center20!=((f.primaryMask&16)?f.centerWords:std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}}))return false;
        }else if(f.lease||f.constructorReturned||f.primaryMask)return false;
        if(f.method){const auto& c=*f.method;
            if(f.node->nativeList34!=&c||!sourceListMethodArrayCurrent(0,f.zone,&c)||!sourceListEntriesCurrent(c))return false;
            const auto& e=c.sourceEntries_;if(f.selectionPending!=(e.pending==&f))return false;
            if(f.selectionPending&&e.pendingIdentity!=f.member)return false;
            if(f.inserted&&!f.primaryWithdrawn){if(!f.slot||!*f.slot||*f.slot>1024||
                e.members[*f.slot-1]!=f.member||e.borrowers[*f.slot-1]!=&f)return false;}
        }else if(f.selectionPending||f.inserted)return false;
    }
    return true;
}
bool Owner::sourceStandardObserverReturnCurrent(const ra_static_model_cold50LA::Owner::StandardWrapper& w)const noexcept {
    if(!w.sourceModel||!preflightSourceStandardObserversNative())return false;
    for(const auto& p:sourceModelObservers_)if(p&&p->wrapper==&w&&p->occurrence==w.sourceModel.get())
        return p->returned&&!p->withdrawn&&p->scene==&w.sourceModel->scene;
    return false;
}
bool Owner::retireSourceStandardObserversNative(std::uint32_t& pc)noexcept {
    if(!preflightSourceStandardObserversNative())return false;
    for(std::size_t i=sourceModelObservers_.size();i>0;--i){auto& f=*sourceModelObservers_[i-1];if(f.withdrawn)continue;
        pc=f.pc;
        if(!withdrawSourceStandardObserverSecondaryNative(f))return false;
        if(f.method){auto& c=*f.method;auto& e=c.sourceEntries_;
            if(f.selectionPending){if(e.pending!=&f)return false;
                if(e.pendingConsumed){e.abandonedConsumed=e.pendingSlot;e.terminal=true;e.pendingConsumed=false;}
                e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;f.selectionPending=false;}
            if(f.inserted&&!f.primaryWithdrawn){if(!f.slot||!e.words||e.pending)return false;const auto slot=*f.slot;
                e.nativeFreeNext[slot-1]=*c.source_.field40;e.wordKnown[slot-1]=false;
                e.members[slot-1]=e.borrowers[slot-1]=nullptr;c.source_.field40=slot;f.primaryWithdrawn=true;}
        }
        if(f.owner&&f.lease){auto& shared=worldApplication_->garageObserverShared_;
            if(f.constructorReturned?!shared.retireReturnedSourceObserverNative(f.owner.get()):!shared.abortSourceObserver280890(f.owner.get()))return false;}
        //Native reciprocal-borrow withdrawal, not source replacement280920.
        auto* model=static_cast<ra_static_model_cold50LA::Owner::SourceModelFrame*>(const_cast<void*>(f.occurrence));
        model->scene.model_.observer19C=nullptr;model->sourceObserver19C=nullptr;model->sourceObserverOccurrence=nullptr;
        model->scene.sourceObserverBorrower_=nullptr;
        f.owner.reset();f.node=nullptr;f.method=nullptr;f.withdrawn=true;
    }
    return true;
}
}

namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceStandardObserverSecondaryNative(const SourceModelObserverFrame& f)const noexcept {
    const auto* o=f.owner.get();
    if(f.secondaryCount>20||(f.secondaryReturned&&!f.secondaryEntered))return false;
    if(f.secondaryWithdrawn){
        if(o&&(o->next00||o->portalCursor||o->cursorMethod))return false;
        for(const auto& s:f.secondary)if(s.edge||s.neighbor||s.portal||s.method||s.link||s.oldHead||s.selectionPending)return false;
        return true;
    }
    if(!f.secondaryEntered){if(f.secondaryCount)return false;
        return !o||(!o->next00&&!o->portalCursor&&!o->cursorMethod);}
    if(!o||!f.constructorReturned||f.primaryMask!=31||!f.node||!worldApplication_)return false;
    if(f.zone>=10||
       !sourcePortalWorldLinksCurrent(*f.node,false,f.zone))return false;
    const auto* expected=f.node->portalLinks30.get();
    const NativeListMethodCustody* method=f.method;const void* head=nullptr;
    const auto& pool=worldApplication_->garageObserverShared_;
    for(std::size_t i=0;i<f.secondary.size();++i){const auto& s=f.secondary[i];
        if(i>=f.secondaryCount){if(s.entered||s.complete||s.edge||s.method||s.selectionPending||s.inserted||s.link||s.headPublished)return false;continue;}
        if(!s.entered||!expected||s.edge!=expected||s.edgeIndex>=40||(s.edgeIndex&1u)||
           s.neighbor!=expected->neighbor04||s.portal!=expected->portal08||s.edgeDistanceMask>3||s.linkStoreMask>127||
           !g::source_portal_overlap231150::historyCurrent(s.overlap))return false;
        const auto& r=sourcePortalWorldLinks_[s.edgeIndex/4][s.edgeIndex%4];
        if(r.withdrawn||r.published!=expected||!r.to||&r.to->entity!=s.neighbor||r.knownMask!=31)return false;
        if(s.edgeDistanceMask&&(!s.overlap.returned||!s.overlap.hit||!s.overlap.outputDistance))return false;
        //The graph owns latest stores; older occurrence receipts remain historical if later callers overwrite them.
        if(s.methodReturned){if(s.edgeDistanceMask!=3||!method)return false;
            const auto usage=method->usage10_;const auto* wanted=(usage&1u)?r.to->nativeList34:(usage&2u)?r.to->nativeList38:nullptr;
            if(s.method!=wanted)return false;method=s.method;
        }else if(s.method)return false;
        if(s.selectionAttempted&&(!s.methodReturned||!s.method))return false;
        if(s.slot&&!s.selectionAttempted)return false;
        if(s.inserted&&(!s.slot||!*s.slot||s.selectionPending))return false;
        if(s.nativeWithdrawn&&!s.inserted)return false;
        if(s.method){const auto& e=s.method->sourceEntries_;
            std::size_t zone=0;while(zone<10&&(!sourceVisibilitySlots4C0_||sourceVisibilitySlots4C0_[zone]!=r.to))++zone;
            if(zone==10||!sourceListMethodArrayCurrent(0,zone,s.method))return false;
            if(!sourceListEntriesCurrent(*s.method)||s.selectionPending!=(e.pending==&s))return false;
            if(s.selectionPending&&e.pendingIdentity!=f.member)return false;
            if(s.inserted&&!s.nativeWithdrawn){if(*s.slot>1024||e.members[*s.slot-1]!=f.member||e.borrowers[*s.slot-1]!=&s)return false;}
        }
        if(s.allocationAttempted&&!s.inserted)return false;
        if(s.allocationReturned&&!s.allocationAttempted)return false;
        if(s.link){if(!s.allocationReturned||s.linkReleased||!pool.ownsSourceLink(sourceDefaultObserver_.get(),o,s.link))return false;
            const auto& link=*s.link;
            if((s.linkStoreMask&1)&&link.slot14!=s.slot.value_or(0))return false;
            if((s.linkStoreMask&2)&&link.child04!=s.neighbor)return false;
            if((s.linkStoreMask&4)&&(link.collisionMethod08!=s.method||link.method08))return false;
            if((s.linkStoreMask&8)&&link.portal0C!=s.portal)return false;
            if((s.linkStoreMask&16)&&(!s.overlap.outputDistance||link.distance10!=*s.overlap.outputDistance))return false;
            if((s.linkStoreMask&32)&&link.next00!=s.oldHead)return false;
        }else if(s.linkStoreMask&&!s.linkReleased)return false;
        if(s.headPublished){if(s.linkStoreMask!=127)return false;
            if(!s.linkReleased){if(s.oldHead!=head)return false;head=s.link;}}
        if(s.complete){if(!s.overlap.returned||(s.overlap.hit&&(!s.slot||(*s.slot&&!s.headPublished))))return false;
            expected=expected->next00.get();
        }else if(i+1!=f.secondaryCount)return false;
    }
    if(o->next00!=head||o->portalCursor!=expected||o->cursorMethod!=method)return false;
    return !f.secondaryReturned||!expected;
}
bool Owner::withdrawSourceStandardObserverSecondaryNative(SourceModelObserverFrame& f)noexcept {
    if(!preflightSourceStandardObserverSecondaryNative(f))return false;
    if(!f.secondaryEntered||f.secondaryWithdrawn)return true;
    auto& o=*f.owner;auto& pool=worldApplication_->garageObserverShared_;
    for(std::size_t i=f.secondaryCount;i>0;--i){auto& s=f.secondary[i-1];
        if(s.selectionPending){auto& e=s.method->sourceEntries_;if(e.pending!=&s)return false;
            if(e.pendingConsumed){e.abandonedConsumed=e.pendingSlot;e.terminal=true;e.pendingConsumed=false;}
            e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;s.selectionPending=false;}
        if(s.inserted&&!s.nativeWithdrawn){auto& c=*s.method;auto& e=c.sourceEntries_;
            if(!s.slot||!*s.slot||*s.slot>1024||e.pending||!e.words||e.members[*s.slot-1]!=f.member||e.borrowers[*s.slot-1]!=&s)return false;
            const auto slot=*s.slot;e.nativeFreeNext[slot-1]=*c.source_.field40;e.wordKnown[slot-1]=false;
            e.members[slot-1]=e.borrowers[slot-1]=nullptr;c.source_.field40=slot;s.nativeWithdrawn=true;}
        if(s.link&&!s.linkReleased){if(s.headPublished&&o.next00!=s.link)return false;
            const void* next=s.headPublished?s.link->next00:o.next00;std::uint32_t pc=0;
            if(!pool.releaseSource105830(sourceDefaultObserver_.get(),&o,s.link,pc))return false;
            o.next00=next;s.linkReleased=true;s.link=nullptr;}
    }
    for(auto& s:f.secondary){s.edge=nullptr;s.neighbor=nullptr;s.portal=nullptr;s.method=nullptr;s.oldHead=nullptr;}
    o.portalCursor=nullptr;o.cursorMethod=nullptr;f.secondaryWithdrawn=true;return true;
}
}

#pragma once
namespace ra_cold_aggregate50LA {
bool Owner::continueSourceStandardObserver28E8A0(ra_static_model_cold50LA::Owner::StandardWrapper& w,std::uint32_t& pc)noexcept {
    if(sourcePostParticleModelsPC_!=0x27adf4u||sourceStandardWrapperA0_!=&w||!sourceStandardCallbackEntered_||
       !sourceStaticOwner_||!w.sourceModel||w.model8!=&w.sourceModel->scene||!worldApplication_||
       !sourceDefaultObserver_||!sourceDefaultObserverCtorReturned_||!sourceDefaultObserverLease_)return false;
    auto& model=*w.sourceModel;auto& scene=model.scene;
    if(!model.packetCallReturned||!model.sourcePackets.sourcePackets_.returned||
       model.sourcePackets.sourcePackets_.nativeWithdrawn||!model.sourcePackets.preflightSourcePacketsNative()||
       model.loaderReturnedEntity!=&scene.model_.entity8||
       !model.placement.orientationReturned||model.nativeRetired||model.nativeAliasWithdrawn||
       !scene.preflightSourceRenderableReturn26E820Native()||scene.sourceObserverBorrower_||scene.model_.observer19C||
       !preflightSourceStandardObserversNative())return false; //Fresh-only;280920 replacement is not issued.
    for(const auto& old:sourceModelObservers_)if(old&&old->occurrence==&model)return false;
    if(!sourceStandardZoneStored_||!nativeModelZone354_||!sourceStandardZoneS0_||
       *nativeModelZone354_!=*sourceStandardZoneS0_||*nativeModelZone354_>=10||
       !sourceVisibility4C0_||!*sourceVisibility4C0_)return false;
    const auto zone=*nativeModelZone354_;auto* node=(*sourceVisibility4C0_)[zone];
    if(!node||zone>=sourceVisibilityPublished_||!sourcePortalWorldLinksCurrent(*node,false,zone))return false;
    //Native frame/vector custody is reserved before any represented call effects.
    auto fresh=std::unique_ptr<SourceModelObserverFrame>(new(std::nothrow) SourceModelObserverFrame);
    if(!fresh)return false;fresh->occurrence=&model;fresh->wrapper=&w;fresh->scene=&scene;
    fresh->member=&scene.model_;fresh->zone=zone;fresh->node=node;
    try{sourceModelObservers_.push_back(std::move(fresh));}catch(...){return false;}
    auto& f=*sourceModelObservers_.back();f.entered=true;model.sourceObserverOccurrence=&f;scene.sourceObserverBorrower_=&f;
    const auto at=[&](std::uint32_t p){pc=f.pc=sourceTrackPC_=sourcePostParticleModelsPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);return false;};
    auto& shared=worldApplication_->garageObserverShared_;using Shared=g::OriginalGarageObserverShared50KG;
    at(0x27ae08);at(0x28e8b8); //Actual NULL19C branch; no replacement destructor.
    at(0x28e8d0);f.owner.reset(new(std::nothrow) SourceModelObserverRecord);f.outerReturned=true;
    if(f.owner){auto& o=*f.owner;
        if(!shared.beginSourceObserver280890(&o,sourceDefaultObserver_.get()))return stop(0x280890);
        f.lease=true;at(0x2808a4);o.center20={{0,0,0,0x3f800000u}};f.vectorKnown=true;
        if(!shared.sourceObserverAllocator2808AC(&o,pc))return stop(pc);
        at(0x2808e0);o.next00=nullptr;f.scalarMask|=1;
        at(0x2808e4);o.child04=nullptr;f.scalarMask|=2;
        at(0x2808e8);o.method08=nullptr;f.scalarMask|=4;
        at(0x2808ec);o.field0C=0;f.scalarMask|=8;
        at(0x2808f0);o.field10=0;f.scalarMask|=16;
        at(0x2808f4);o.slot14=0;f.scalarMask|=32;
        if(!shared.sourceObserverIncrement28090C(&o,pc))return stop(pc);
        at(0x280918);if(!shared.finishSourceObserver280918(&o))return stop(pc);f.constructorReturned=true;
    }
    at(0x28e8e8);scene.model_.observer19C=f.owner.get();model.sourceObserver19C=f.owner.get();f.published=true;
    at(0x28e8ec);if(!Shared::beginSourceAssertion(0,f.allocationAssertion))return stop(pc);
    at(0x28e924);if(!Shared::finishSourceAssertion(f.allocationAssertion,bool(f.owner)))return stop(pc);
    at(0x28e958);f.center={{0,0,0,1}};at(0x28e960);f.half={{0,0,0,1}};
    at(0x28e964);f.boundsEntered=true;const auto bounds=scene.produceSourceDirectBounds28E230(model.graph(),f.center,f.half);
    std::memcpy(f.centerWords.data(),f.center.data(),16);std::memcpy(f.halfWords.data(),f.half.data(),16);
    if(!bounds.okay())return stop(bounds.sourcePC);f.boundsReturned=true;
    at(0x2809e8);if(!Shared::beginSourceAssertion(1,f.childAssertion))return stop(pc);
    if(f.childAssertion.enabled&&!f.owner)return stop(0x280a2c);
    at(0x280a40);if(!Shared::finishSourceAssertion(f.childAssertion,f.owner&&f.owner->child04==nullptr))return stop(pc);
    at(0x27f5a0);f.method=f.node->nativeList34;
    at(0x280a80);if(!Shared::beginSourceAssertion(2,f.methodAssertion))return stop(pc);
    at(0x280ab8);if(!Shared::finishSourceAssertion(f.methodAssertion,f.method!=nullptr))return stop(pc);
    at(0x280ae8);if(!f.method)return stop(pc);
    if(!sourceListMethodArrayCurrent(0,zone,f.method))return stop(0x280afc);
    at(0x280afc);if(!selectSourceObserverSlot27E3C0(*f.method,&f,f.member,f.slot,f.selectionPending,pc))return stop(pc);
    at(0x280b08);if(!Shared::beginSourceAssertion(3,f.slotAssertion))return stop(pc);
    at(0x280b40);if(!Shared::finishSourceAssertion(f.slotAssertion,f.slot&&*f.slot))return stop(pc);
    at(0x280b88);auto& c=*f.method;auto& e=c.sourceEntries_;const auto slot=f.slot.value_or(0);
    if(!slot||slot>1024||slot!=e.pendingSlot||e.pending!=&f||e.pendingIdentity!=f.member)return stop(pc);
    if(*c.source_.field40==slot){at(0x27e594);if(!e.words||e.members[slot-1])return stop(pc);c.source_.field40=e.nativeFreeNext[slot-1];}
    else{at(0x27e5a0);c.source_.lastUsed38=*c.source_.lastUsed38+1;}
    e.pendingConsumed=true;at(0x27e5a8);if(!e.words)return stop(pc);
    e.members[slot-1]=f.member;e.borrowers[slot-1]=&f;e.wordKnown[slot-1]=false;
    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;f.selectionPending=false;f.inserted=true;
    at(0x280b90);if(!f.owner)return stop(pc);auto& o=*f.owner;
    o.child04=&f.node->entity;f.primaryMask|=1;
    at(0x280b94);o.method08=f.method;f.primaryMask|=2;
    at(0x280b98);o.slot14=slot;f.primaryMask|=4;
    at(0x280b9c);o.next00=nullptr;f.primaryMask|=8;
    at(0x280bbc);o.center20=f.centerWords;f.primaryMask|=16;
    at(0x280bf0);return continueSourceStandardObserverSecondary280BF0(f,pc);
}
}

namespace ra_cold_aggregate50LA {
bool Owner::continueSourceStandardObserverSecondary280BF0(SourceModelObserverFrame& f,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=f.pc=sourceTrackPC_=sourcePostParticleModelsPC_=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(f.pc!=0x280bf0||!preflightSourceStandardObserversNative()||f.secondaryEntered||f.withdrawn||
       !f.owner||f.primaryMask!=31||!f.node)return false;
    auto& o=*f.owner;auto& pool=worldApplication_->garageObserverShared_;
    f.secondaryEntered=true;
    at(0x280c1c);if(o.child04!=&f.node->entity)return stop(pc);
    at(0x280c20);o.cursorMethod=o.method08;at(0x280c34);o.portalCursor=f.node->portalLinks30.get();
    while(o.portalCursor){
        std::size_t index=0;while(index<40&&sourcePortalWorldLinks_[index/4][index%4].published!=o.portalCursor)++index;
        if(index==40||(index&1u)||f.secondaryCount==20)return stop(0x280c40);
        auto& r=sourcePortalWorldLinks_[index/4][index%4];
        if(r.withdrawn||r.knownMask!=31||!r.to||!r.from)return stop(0x280c40);
        auto& edge=*r.published;auto& s=f.secondary[f.secondaryCount++];
        s.entered=true;s.edgeIndex=index;s.edge=&edge;s.neighbor=edge.neighbor04;s.portal=edge.portal08;
        const auto& portal=nativePortals_[index/4];at(0x280c40);
        if(!portal||!portal->sourceMode||&portal->entity!=s.portal||!portal->source.loadReturned||
           portal->source.normalKnown!=7||!portal->source.normalNormalized||portal->source.copiedVertices!=4)return stop(0x231150);
        for(unsigned e=0;e<4;++e)if(portal->source.edgeKnown[e]!=7||!portal->source.edgeNormalized[e])return stop(0x231150);
        //28E970 supplies radius +0. Actual bounds center XYZ; half is passed through
        //to List services, while source231150 consumes center and radius only.
        at(0x231150);if(!g::source_portal_overlap231150::run(portal->source.vertices30,portal->source.normal70,
            portal->source.edges80,f.centerWords,0u,s.overlap))return stop(s.overlap.pc);
        at(0x280c54);if(!s.overlap.hit){s.complete=true;at(0x280cf8);o.portalCursor=edge.next00.get();continue;}
        if(!s.overlap.outputDistance)return stop(0x280c5c);
        at(0x280c60);edge.field10=*s.overlap.outputDistance;r.distance10=s.overlap.outputDistance;s.edgeDistanceMask|=1;
        at(0x280c64);edge.field0C=*s.overlap.outputDistance;r.distance0C=s.overlap.outputDistance;s.edgeDistanceMask|=2;
        at(0x280c6c);if(!o.cursorMethod)return stop(pc);const auto usage=o.cursorMethod->usage10_;
        if(&r.to->entity!=s.neighbor)return stop(0x27f5a0);
        s.method=(usage&1u)?r.to->nativeList34:(usage&2u)?r.to->nativeList38:nullptr;s.methodReturned=true;
        at(0x280c7c);o.cursorMethod=s.method;if(!s.method)return stop(0x280c78);
        std::size_t zone=0;while(zone<10&&(!sourceVisibilitySlots4C0_||sourceVisibilitySlots4C0_[zone]!=r.to))++zone;
        if(zone==10||!sourceListMethodArrayCurrent(0,zone,s.method))return stop(0x280c90);
        s.selectionAttempted=true;at(0x280c90);
        if(!selectSourceObserverSlot27E3C0(*s.method,&s,f.member,s.slot,s.selectionPending,pc))return stop(pc);
        at(0x280c9c);if(!*s.slot){auto& e=s.method->sourceEntries_;e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;
            s.selectionPending=false;s.complete=true;at(0x280cf8);o.portalCursor=edge.next00.get();continue;}
        auto& c=*s.method;auto& e=c.sourceEntries_;const auto slot=*s.slot;at(0x280cbc);
        if(slot>1024||slot!=e.pendingSlot||e.pending!=&s||e.pendingIdentity!=f.member)return stop(pc);
        if(*c.source_.field40==slot){at(0x27e594);if(!e.words||e.members[slot-1])return stop(pc);c.source_.field40=e.nativeFreeNext[slot-1];}
        else {at(0x27e5a0);c.source_.lastUsed38=*c.source_.lastUsed38+1;}
        e.pendingConsumed=true;at(0x27e5a8);if(!e.words)return stop(pc);
        e.members[slot-1]=f.member;e.borrowers[slot-1]=&s;e.wordKnown[slot-1]=false;
        e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;s.selectionPending=false;s.inserted=true;
        at(0x280cc8);s.allocationAttempted=true;const auto allocated=pool.allocateSource105720(sourceDefaultObserver_.get(),&o);
        if(allocated.status==g::OriginalGarageObserverShared50KG::SourceLinkStatus::nativeRefused)return stop(allocated.sourcePC);
        s.allocationReturned=true;s.link=allocated.record;
        at(0x280cd0);if(!s.link)return stop(pc); //List admission survives the original NULL link store.
        s.link->slot14=slot;s.linkStoreMask|=1;at(0x280cd4);s.link->child04=s.neighbor;s.linkStoreMask|=2;
        at(0x280cd8);s.link->collisionMethod08=s.method;s.link->method08=nullptr;s.linkStoreMask|=4;
        at(0x280ce0);s.link->portal0C=s.portal;s.linkStoreMask|=8;
        at(0x280ce8);s.link->distance10=*s.overlap.outputDistance;s.linkStoreMask|=16;
        at(0x280cec);s.oldHead=o.next00;
        at(0x280cf0);s.link->next00=static_cast<g::OriginalGarageObserverShared50KG::Link105720*>(const_cast<void*>(s.oldHead));s.linkStoreMask|=32;
        at(0x280cf4);o.next00=s.link;s.linkStoreMask|=64;s.headPublished=true;
        s.complete=true;at(0x280cf8);o.portalCursor=edge.next00.get();
    }
    at(0x280d30);f.secondaryReturned=true;
    at(0x280be4);at(0x28e98c);using Shared=g::OriginalGarageObserverShared50KG;
    if(!Shared::beginSourceAssertion(4,f.returnAssertion))return stop(pc);
    if(f.returnAssertion.enabled&&!f.owner)return stop(0x28e9c8);
    at(0x28e9cc);if(!Shared::finishSourceAssertion(f.returnAssertion,f.owner&&f.owner->child04))return stop(pc);
    at(0x28ea10);f.returned=true;at(0x27ae14);return true; //No Entity attachment or wrapper return.
}
}

