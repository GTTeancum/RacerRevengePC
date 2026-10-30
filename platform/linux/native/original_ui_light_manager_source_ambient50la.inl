#pragma once
#include <new>
#include <cstring>
namespace rr::platform {
bool OriginalUiLightManagerOwner50KG::appendSourceSecond148F30(::ra_cold_aggregate50LA::Owner& world,Diagnostic& d)noexcept {
    d={};
    if(!preflightSourceRaZonesNative(world)||!raZones_||!raZones_->sourceReturned||raZones_->sourceNativeClosed||
       !raZones_->sourceAmbientRegistered||raZones_->sourceAmbientWithdrawn||raZones_->sourceSecondRegistered||
       raZones_->sourceSecondWithdrawn||!world.preflightSourceSecondLightNative()||!world.sourceSecondLight_||
       !world.sourceSecondConfigReturned_||!world.sourceSecondLight_->configured||world.sourceSecondLight_->registered||
       !world.sourceSecondLightDescriptor_||world.sourceSecondLightDescriptor_->type00!=2||world.sourceSecondLightDescriptor_->zone20||
       world.sourceTopologyLightManager_.get()!=this||world.sourceLoadApplication_!=issuer_||
       world.sourceLoadApplication_->sourceLightManager426F60Owner_.get()!=this)
        return fail(d,Status::phase,0x148f30);
    //Exact existing source registry from first Ambient; never substitute legacy
    //registration or reinterpret a failed/warm allocation as fresh capacity10.
    if(count94_!=1||capacity98_!=10||!registry9C_||registryWritten_!=1||registryFailed_||raRegistryGrowthFailed_||
       registry9C_[0]!=raZones_->sourceAmbient||registryPublished_[0]!=raZones_->sourceAmbient||
       !directional44_||count90_!=10||(raZones_->sourceArrayWithdrawnMask&16u))return fail(d,Status::phase,0x149180);
    auto& r=*raZones_;auto& light=*world.sourceSecondLight_;
    //Native admission above covers every borrowed destination before first
    //source store; below has no allocation, callback, or failure boundary.
    //149048 type2 skips node attachment,1490A4..BC copies RGB/W0.
    for(unsigned lane=0;lane<3;++lane)std::memcpy(&vector30_[lane],&light.diffuse3C[lane],sizeof(std::uint32_t));
    vector30_[3]=0;
    field40_=&light.entity; //1490D0 same live counted source owner.
    for(std::size_t i=0;i<10;++i)directional44_[i]=vector30_; //1490F4..149140 zone0 propagation.
    //14915C type2 skips type1-only A0; count1/cap10 skips realloc149194.
    registry9C_[1]=&light.entity;registryPublished_[1]=&light.entity;registryWritten_=2; //1491C0.
    ++count94_; //1491CC, after slot1 publication; first Ambient slot0 untouched.
    r.sourceSecondRegistryReceipt=&light.entity;r.sourceSecondRegistered=true;light.registered=true;
    //Source-specific receipt only. Generic native directional/render-ready flags
    //remain false; no legacy lane is adopted or newly admitted.
    d={Status::complete,0x1491e0};return true;
}
bool OriginalUiLightManagerOwner50KG::withdrawSourceSecondNative(::ra_cold_aggregate50LA::Owner& world,Diagnostic& d)noexcept {
    d={};if(!preflightSourceRaZonesNative(world))return fail(d,Status::phase,0x148110);
    if(!raZones_||!raZones_->sourceMode||raZones_->sourceWorldIdentity!=&world)return true;
    auto& r=*raZones_;if(!r.sourceSecondRegistered||r.sourceSecondWithdrawn)return true;
    if(!world.sourceSecondLight_||r.sourceSecondRegistryReceipt!=&world.sourceSecondLight_->entity||
       !world.sourceSecondLight_->registered||count94_!=2||r.sourceAmbientWithdrawn)
        return fail(d,Status::identity,0x1491f0);
    --count94_;world.sourceSecondLight_->registered=false;r.sourceSecondWithdrawn=true;
    //Native lifetime withdrawal, NOT guest1491F0. Keep known dead slot1/field40,
    //vector30, row colors, capacity10, registryWritten2 and revision history.
    //The receipt is thereafter an identity-only address, never dereferenced.
    return true;
}

bool OriginalUiLightManagerOwner50KG::incrementSourceTyped149300(::ra_cold_aggregate50LA::Owner& world,
    std::size_t ordinal,unsigned stage,Diagnostic& d)noexcept {
    d={};if(stage>3||ordinal<2||ordinal-2>=world.sourceTypedLights_.size()||
       !preflightSourceRaZonesNative(world)||!raZones_||!raZones_->sourceReturned||raZones_->sourceNativeClosed||
       !raZones_->sourceSecondRegistered||raZones_->sourceSecondWithdrawn||
       !world.preflightSourceTypedLightsNative()||world.sourceTopologyLightManager_.get()!=this||
       world.sourceLoadApplication_!=issuer_||issuer_->sourceLightManager426F60Owner_.get()!=this)
        return fail(d,Status::phase,0x149300);
    const auto& f=*world.sourceTypedLights_[ordinal-2];
    if(f.ordinal!=ordinal||f.nativeClosed||!f.light||!f.descriptor||(f.descriptor->type00!=1&&f.descriptor->type00!=3&&f.descriptor->type00!=4))
        return fail(d,Status::identity,0x149300);
    const auto& l=*f.light;
    if(f.laterAmbientConfigEntered&&stage!=0)return fail(d,Status::phase,0x149300);
    if(stage==0){if(!l.colorsWritten||l.revisionWritten)return fail(d,Status::phase,0x149300);}
    else if(stage==1){if(!l.revisionWritten||!f.luminance||!f.positionCompared||!l.positionWritten||l.positionRevision)
        return fail(d,Status::phase,0x149300);}
    else if(stage==2&&f.spotlightConfigEntered){if(!f.lookCompared||!f.spotlightLookWords||!l.lookWritten||l.lookRevision)
        return fail(d,Status::phase,0x149300);}
    else if(stage!=(f.spotlightConfigEntered?3u:2u)||!f.lookCompared||!f.attenuationWords||!f.attenuationRange||!l.rangeWritten||l.rangeRevision)
        return fail(d,Status::phase,0x149300);
    for(const auto& receipt:raZones_->sourceTypedRevisions)
        if(receipt.ordinal>ordinal||(receipt.ordinal==ordinal&&receipt.stage>=stage))return fail(d,Status::phase,0x149300);
    try{raZones_->sourceTypedRevisions.reserve(raZones_->sourceTypedRevisions.size()+1);}catch(...){return fail(d,Status::allocation,0x149300);}
    raZones_->sourceTypedRevisions.push_back({ordinal,stage,field84_});
    ++field84_;d={Status::complete,0x149310};return true;
}
bool OriginalUiLightManagerOwner50KG::boundSourceTyped148F30(::ra_cold_aggregate50LA::Owner& world,
    std::size_t ordinal,Diagnostic& d)noexcept {
    d={};if(!preflightSourceRaZonesNative(world)||!raZones_||raZones_->sourceNativeClosed||
       !raZones_->sourceReturned||!raZones_->sourceSecondRegistered||raZones_->sourceSecondWithdrawn||
       !world.preflightSourceTypedLightsNative()||ordinal<2||ordinal-2>=world.sourceTypedLights_.size()||
       world.sourceTopologyLightManager_.get()!=this||world.sourceLoadApplication_!=issuer_||
       issuer_->sourceLightManager426F60Owner_.get()!=this)return fail(d,Status::phase,0x148f30);
    const auto& f=*world.sourceTypedLights_[ordinal-2];
    if(f.nativeClosed||!f.light||!f.configured||!f.descriptor||f.ordinal!=ordinal||(f.light->type34!=1&&f.light->type34!=3)||
       f.light->type34!=f.descriptor->type00||(f.light->type34==3&&(!f.spotlightCone9C||!f.spotlightConfigEntered))||
       f.wrapper->live00!=&f.light->entity||f.descriptor!=f.wrapper->sourceDescriptor04.get())
        return fail(d,Status::identity,0x148f30);
    //148F44/148F60: type1 and3 skip Ambient writes.149048 sees same actual nodes88.
    const auto zone=f.descriptor->zone20;
    if(!nodes88_||zone<0||static_cast<std::uint32_t>(zone)>=count90_)return fail(d,Status::phase,0x149078);
    const auto i=static_cast<std::size_t>(zone);
    if(!raZones_->zones[i]||nodes88_[i]!=raZones_->zones[i].get()||!raZones_->lists[i]||
       nodes88_[i]->first34!=raZones_->lists[i].get()||nodes88_[i]->second38!=raZones_->lists[i].get())
        return fail(d,Status::identity,0x149078);
    //149078 selects nodes88[zone20], NOT source ordinal or human zone number.
    //Return bounded admission only;1474A0 is uncalled and registry unchanged.
    d={Status::complete,0x149080};return true;
}

bool OriginalUiLightManagerOwner50KG::preflightSourceTypedObserverNative(
    const ::ra_pod_scene50LA::Owner& world,std::size_t ordinal)const noexcept {
    if(ordinal<2||ordinal-2>=world.sourceTypedLights_.size()||!world.sourceTypedLights_[ordinal-2])return false;
    const auto& f=*world.sourceTypedLights_[ordinal-2];if(!f.light)return !f.observerEntered;
    const auto& l=*f.light;using Shared=game::OriginalGarageObserverShared50KG;
    if(!Shared::sourceAssertionHistoryCurrent(f.observerD0Assertion)||
       !Shared::sourceAssertionHistoryCurrent(f.observerChildAssertion)||
       !Shared::sourceAssertionHistoryCurrent(f.observerMethodAssertion)||
       !Shared::sourceAssertionHistoryCurrent(f.observerSlotAssertion))return false;
    if(f.observerWithdrawn)return !l.observerOwner&&!l.observerD0&&!l.fieldD4&&!f.observerLease&&
        !f.primaryNode&&!f.primaryList&&!f.primarySelectionPending;
    if(!f.observerEntered)return !l.observerOwner&&!l.observerD0&&!l.fieldD4&&!f.observerOuterReached&&
        !f.observerLease&&!f.observerReturned&&!f.observerVectorKnown&&!f.observerScalarMask&&
        !f.observerD0Published&&!f.observerD4Published&&!f.primaryNode&&!f.primaryList&&
        !f.primaryMethodReturned&&!f.primarySelectionAttempted&&!f.primarySelectionPending&&!f.primaryInserted&&
        !f.primarySlot&&!f.primaryStoreMask;
    if(!raZones_||!raZones_->world||static_cast<const ::ra_pod_scene50LA::Owner*>(raZones_->world)!=&world||
       !preflightSourceRaZonesNative(*raZones_->world)||world.worldApplication_!=entities_||
       f.observerManager!=this||f.ordinal!=ordinal||!f.configured||!f.zone||*f.zone<0||static_cast<std::uint32_t>(*f.zone)>=count90_||!f.primaryNode||
       !raZones_->zones[static_cast<std::size_t>(*f.zone)]||f.primaryNode!=&raZones_->zones[static_cast<std::size_t>(*f.zone)]->entity||f.observerScalarMask>63||f.primaryStoreMask>31)
        return false;
    if(f.observerOuterReached&&!f.observerD0Assertion.passed)return false;
    if(f.observerReturned&&(!f.observerLease||!f.observerVectorKnown||f.observerScalarMask!=63))return false;
    if(f.observerD0Published&&(!f.observerOuterReached||l.observerD0!=l.observerOwner.get()))return false;
    if(!f.observerD0Published&&l.observerD0)return false;
    if(f.observerD4Published&&(!f.observerD0Published||l.fieldD4!=f.primaryNode))return false;
    if(!f.observerD4Published&&l.fieldD4)return false;
    if(f.observerLease){if(!l.observerOwner)return false;const auto& shared=entities_->garageObserverShared_;
        if(f.observerReturned){if(!shared.ownsReturnedSourceObserver(l.observerOwner.get()))return false;}
        else if(!shared.sourceObserverCurrent280890(l.observerOwner.get()))return false;}
    if(!l.observerOwner){if(f.observerLease||f.observerReturned||f.observerVectorKnown||f.observerScalarMask||
        f.primaryStoreMask||f.primaryInserted||f.primarySelectionAttempted)return false;}
    else {const auto& o=*l.observerOwner;
        if(!f.observerOuterReached||o.unknown18To1C[0]||o.unknown18To1C[1]||o.field0C||o.field10||
           o.pending||
           o.sharedConstructed!=f.observerReturned||o.primaryRegistered!=(f.primaryStoreMask==31))return false;
        if(o.child04!=((f.primaryStoreMask&1)?f.primaryNode:nullptr)||
           o.method08!=((f.primaryStoreMask&2)?f.primaryList:nullptr)||
           o.slot14!=((f.primaryStoreMask&4)?f.primarySlot.value_or(0):0))return false;
        if(f.observerVectorKnown&&o.center20!=((f.primaryStoreMask&16)?l.positionA0:Words{{0,0,0,0x3f800000u}}))return false;
    }
    if(!preflightSourceTypedSecondaryNative(world,ordinal))return false;
    if(f.primaryMethodReturned){if(!f.observerChildAssertion.passed||!f.observerD4Published||
        f.primaryList!=raZones_->zones[static_cast<std::size_t>(*f.zone)]->first34||f.primaryList!=raZones_->lists[static_cast<std::size_t>(*f.zone)].get())return false;}
    else if(f.primaryList)return false;
    if(f.primarySelectionAttempted&&(!f.primaryMethodReturned||!f.observerMethodAssertion.passed||!f.primaryList))return false;
    if(f.primarySlot&&(!f.primarySelectionAttempted||!*f.primarySlot))return false;
    if(f.primaryInserted&&(!f.primarySlot||!f.observerSlotAssertion.passed||f.primarySelectionPending))return false;
    if(f.primaryNativeWithdrawn&&!f.primaryInserted)return false;
    if(f.primaryStoreMask&&(!f.primaryInserted||!f.observerReturned))return false;
    if(f.primaryList){const auto& list=*f.primaryList;
        if(!list.sourceLightHistoryCurrent(this,issuer_,entities_))return false;
        if(f.primarySelectionPending!=(list.sourceLightOccurrence_==&f))return false;
        if(f.primarySelectionPending&&(list.sourcePendingLight_!=&l.entity||list.sourcePendingToken_!=4u*(ordinal+1)||
           (f.primarySlot&&list.sourcePendingSlot_!=*f.primarySlot)))return false;
        if(f.primaryInserted&&list.lightMemberAt(&l.entity,*f.primarySlot)==f.primaryNativeWithdrawn)return false;
    }
    return true;
}
bool OriginalUiLightManagerOwner50KG::withdrawSourceTypedObserverNative(
    ::ra_pod_scene50LA::Owner& world,std::size_t ordinal,Diagnostic& d)noexcept {
    d={};if(!preflightSourceTypedObserverNative(world,ordinal))return fail(d,Status::phase,0);
    auto& f=*world.sourceTypedLights_[ordinal-2];if(!f.observerEntered||f.observerWithdrawn)return true;
    auto& l=*f.light;L::D ld;
    if(!withdrawSourceTypedSecondaryNative(world,ordinal,d))return false;
    //Reverse borrower withdrawal before shared observer/count, then light Entity.
    //Keep source allocation/selection/store history; no source destructor replay.
    if(f.primarySelectionPending){if(!f.primaryList->cancelSourceLightSelectionNative(&f,ld))return fail(d,Status::list,ld.sourcePC);
        f.primarySelectionPending=false;}
    if(f.primaryInserted&&!f.primaryNativeWithdrawn){
        if(!f.primaryList->withdrawSourceLightNative(this,issuer_,entities_,l.entity,*f.primarySlot,ld))return fail(d,Status::list,ld.sourcePC);
        f.primaryNativeWithdrawn=true;
    }
    if(f.observerLease){auto& shared=entities_->garageObserverShared_;
        if(f.observerReturned){if(!shared.retireReturnedSourceObserverNative(l.observerOwner.get()))return fail(d,Status::phase,0);}
        else if(!shared.abortSourceObserver280890(l.observerOwner.get()))return fail(d,Status::phase,0);
        f.observerLease=false;
    }
    l.observerD0=nullptr;l.fieldD4=nullptr;l.observerOwner.reset();f.primaryList=nullptr;f.primaryNode=nullptr;
    f.observerManager=nullptr;f.observerWithdrawn=true;return true;
}
bool OriginalUiLightManagerOwner50KG::bindSourceTypedPrimary1474A0(
    ::ra_cold_aggregate50LA::Owner& world,std::size_t ordinal,Diagnostic& d)noexcept {
    if(!boundSourceTyped148F30(world,ordinal,d))return false;
    auto& f=*world.sourceTypedLights_[ordinal-2];auto& l=*f.light;
    if(f.observerEntered||f.observerWithdrawn||!world.sourceDefaultObserver_||!world.sourceDefaultObserverCtorReturned_||
       !world.sourceDefaultObserverLease_||!entities_->garageObserverShared_.ownsReturnedSourceObserver(world.sourceDefaultObserver_.get()))
        return fail(d,Status::phase,0x1474a0);
    f.observerEntered=true;f.observerManager=this;f.primaryNode=&raZones_->zones[static_cast<std::size_t>(f.descriptor->zone20)]->entity;
    const auto at=[&](std::uint32_t pc){f.pc=pc;d.sourcePC=pc;};
    const auto stop=[&](std::uint32_t pc){at(pc);return fail(d,Status::phase,pc);};
    using Shared=game::OriginalGarageObserverShared50KG;auto& shared=entities_->garageObserverShared_;
    at(0x1474b4);if(!Shared::beginSourceAssertion(6,f.observerD0Assertion))return stop(0x1474b4);
    at(0x1474fc);if(!Shared::finishSourceAssertion(f.observerD0Assertion,l.observerD0==nullptr))return stop(0x1474fc);
    at(0x147530);f.observerOuterReached=true;l.observerOwner.reset(new(std::nothrow) ::ra_pod_scene50LA::Owner::NativeLightObserver280890);
    if(l.observerOwner){auto& o=*l.observerOwner;
        //Native ledger refusal precedes represented constructor effects.
        if(!shared.beginSourceObserver280890(&o,world.sourceDefaultObserver_.get()))return stop(0);
        f.observerLease=true;at(0x2808a4);o.center20={{0,0,0,0x3f800000u}};f.observerVectorKnown=true;
        std::uint32_t pc=0;if(!shared.sourceObserverAllocator2808AC(&o,pc))return stop(pc);
        at(0x2808e0);o.next00=nullptr;f.observerScalarMask|=1;
        at(0x2808e4);o.child04=nullptr;f.observerScalarMask|=2;
        at(0x2808e8);o.method08=nullptr;f.observerScalarMask|=4;
        at(0x2808ec);o.field0C=0;f.observerScalarMask|=8;
        at(0x2808f0);o.field10=0;f.observerScalarMask|=16;
        at(0x2808f4);o.slot14=0;f.observerScalarMask|=32;
        if(!shared.sourceObserverIncrement28090C(&o,pc))return stop(pc);
        at(0x280918);if(!shared.finishSourceObserver280918(&o))return stop(0x280918);
        f.observerReturned=true;o.sharedConstructed=true;
    }
    at(0x147548);l.observerD0=l.observerOwner.get();f.observerD0Published=true;
    at(0x147554);l.fieldD4=f.primaryNode;f.observerD4Published=true; //Delay-slot store even observerNULL.
    //147574 supplies actual light+30 identity, node, method1, A0position,
    //same-App426F10 and current84range. No transformed/native local inputs.
    at(0x2809e8);if(!Shared::beginSourceAssertion(1,f.observerChildAssertion))return stop(0x2809e8);
    if(f.observerChildAssertion.enabled&&!l.observerOwner)return stop(0x280a2c);
    at(0x280a40);if(!Shared::finishSourceAssertion(f.observerChildAssertion,
        l.observerOwner&&l.observerOwner->child04==nullptr))return stop(0x280a40);
    at(0x27f5a0);f.primaryList=raZones_->zones[static_cast<std::size_t>(f.descriptor->zone20)]->first34;f.primaryMethodReturned=true;
    at(0x280a80);if(!Shared::beginSourceAssertion(2,f.observerMethodAssertion))return stop(0x280a80);
    at(0x280ab8);if(!Shared::finishSourceAssertion(f.observerMethodAssertion,f.primaryList!=nullptr))return stop(0x280ab8);
    if(!f.primaryList)return stop(0x280ae8);
    L::D ld;std::uint32_t slot=0;f.primarySelectionAttempted=true;at(0x280afc);
    const bool selected=f.primaryList->selectSourceLightSlot(this,issuer_,entities_,&f,l.entity,
        static_cast<std::uint32_t>(ordinal),l.positionA0,entities_->lightVector426F10_,slot,ld);
    f.primarySelectionPending=f.primaryList->sourceLightOccurrence_==&f;
    if(!selected)return stop(ld.sourcePC);f.primarySlot=slot;
    at(0x280b08);if(!Shared::beginSourceAssertion(3,f.observerSlotAssertion))return stop(0x280b08);
    at(0x280b40);if(!Shared::finishSourceAssertion(f.observerSlotAssertion,slot!=0))return stop(0x280b40);
    at(0x280b88);if(!f.primaryList->insertSourceLightSelected(this,issuer_,entities_,&f,l.entity,
        static_cast<std::uint32_t>(ordinal),l.positionA0,entities_->lightVector426F10_,slot,ld))return stop(ld.sourcePC);
    f.primarySelectionPending=false;f.primaryInserted=true;
    if(!l.observerOwner)return stop(0x280b90);auto& o=*l.observerOwner;
    at(0x280b90);o.child04=f.primaryNode;f.primaryStoreMask|=1;
    at(0x280b94);o.method08=f.primaryList;f.primaryStoreMask|=2;
    at(0x280b98);o.slot14=slot;f.primaryStoreMask|=4; //Actual returned List handle, NOT descriptor ordinal2.
    at(0x280b9c);o.next00=nullptr;f.primaryStoreMask|=8;
    at(0x280bbc);o.center20=l.positionA0;f.primaryStoreMask|=16;o.primaryRegistered=true;
    return stop(0x280bf0); //Delay-slot center store committed; secondary traversal/1474A0 return unclaimed.
}

bool OriginalUiLightManagerOwner50KG::preflightSourceTypedSecondaryNative(
    const ::ra_pod_scene50LA::Owner& world,std::size_t ordinal)const noexcept {
    if(ordinal<2||ordinal-2>=world.sourceTypedLights_.size()||!world.sourceTypedLights_[ordinal-2])return false;
    const auto& f=*world.sourceTypedLights_[ordinal-2];if(!f.light)return !f.secondaryEntered;
    const auto* observer=f.light->observerOwner.get();
    if(f.secondaryCount>20||f.secondaryReturned&&!f.secondaryEntered)return false;
    if(f.secondaryNativeWithdrawn){
        if(observer&&(observer->next00||observer->portalCursor||observer->cursorMethod))return false;
        for(const auto& s:f.secondary)if(s.edge||s.neighbor||s.portal||s.method||s.link||s.oldHead||s.selectionPending)return false;
        return true;
    }
    if(!f.secondaryEntered){if(f.secondaryCount)return false;
        return !observer||(!observer->next00&&!observer->portalCursor&&!observer->cursorMethod&&!observer->portalLoopStarted&&!observer->portalLoopComplete);}
    if(!observer||!f.observerReturned||f.primaryStoreMask!=31||!raZones_||!f.zone||
       *f.zone<0||static_cast<std::uint32_t>(*f.zone)>=count90_||
       !raZones_->zones[static_cast<std::size_t>(*f.zone)]||
       !observer->portalLoopStarted||observer->portalLoopComplete!=f.secondaryReturned)return false;
    auto* expected=static_cast<const RaPortalLink27F3B0*>(raZones_->zones[static_cast<std::size_t>(*f.zone)]->field30);
    const L* expectedMethod=f.primaryList;const game::OriginalGarageObserverShared50KG::Link105720* head=nullptr;
    const auto& pool=entities_->garageObserverShared_;
    for(std::size_t i=0;i<f.secondary.size();++i){const auto& s=f.secondary[i];
        if(i>=f.secondaryCount){if(s.entered||s.complete||s.edge||s.method||s.selectionPending||s.inserted||s.link||s.headPublished)return false;continue;}
        if(!s.entered||!expected||s.edge!=expected||s.edgeIndex>=20||raZones_->portalLinks[s.edgeIndex].get()!=expected||
           s.neighbor!=expected->neighbor04||s.portal!=expected->portal08||s.edgeDistanceMask>3||s.linkStoreMask>127||
           !game::source_portal_overlap231150::historyCurrent(s.overlap))return false;
        if(s.edgeDistanceMask&&(!s.overlap.returned||!s.overlap.hit||!s.overlap.outputDistance))return false;
        if(s.methodReturned){if(s.edgeDistanceMask!=3||!expectedMethod)return false;
            const Zone* node=nullptr;for(const auto& z:raZones_->zones)if(z&&&z->entity==s.neighbor)node=z.get();
            if(!node)return false;const auto usage=expectedMethod->usage10_;
            const auto* wanted=(usage&1u)?node->first34:(usage&2u)?node->second38:nullptr;
            if(s.method!=wanted)return false;expectedMethod=s.method;
        }else if(s.method)return false;
        if(s.selectionAttempted&&(!s.methodReturned||!s.method))return false;
        if(s.slot&&!s.selectionAttempted)return false;
        if(s.inserted&&(!s.slot||!*s.slot||s.selectionPending))return false;
        if(s.nativeWithdrawn&&!s.inserted)return false;
        if(s.method){if(!s.method->sourceLightHistoryCurrent(this,issuer_,entities_)||
            s.selectionPending!=(s.method->sourceLightOccurrence_==&s))return false;
            if(s.selectionPending&&(s.method->sourcePendingLight_!=&f.light->entity||
                s.method->sourcePendingToken_!=4u*(ordinal+1)))return false;
            if(s.inserted&&s.method->lightMemberAt(&f.light->entity,*s.slot)==s.nativeWithdrawn)return false;
        }
        if(s.allocationAttempted&&!s.inserted)return false;
        if(s.allocationReturned&&!s.allocationAttempted)return false;
        if(s.link){
            if(!s.allocationReturned||s.linkReleased||!pool.ownsSourceLink(raZones_->world->sourceDefaultObserver_.get(),observer,s.link))return false;
            const auto& link=*s.link;
            if((s.linkStoreMask&1)&&link.slot14!=s.slot.value_or(0))return false;
            if((s.linkStoreMask&2)&&link.child04!=s.neighbor)return false;
            if((s.linkStoreMask&4)&&link.method08!=s.method)return false;
            if((s.linkStoreMask&8)&&link.portal0C!=s.portal)return false;
            if((s.linkStoreMask&16)&&(!s.overlap.outputDistance||link.distance10!=*s.overlap.outputDistance))return false;
            if((s.linkStoreMask&32)&&link.next00!=s.oldHead)return false;
        }else if(s.linkStoreMask&&!s.linkReleased)return false;
        if(s.headPublished){if(s.linkStoreMask!=127)return false;
            if(!s.linkReleased){if(s.oldHead!=head)return false;head=s.link;}}
        if(s.complete){if(!s.overlap.returned||(s.overlap.hit&&(!s.slot||(*s.slot&&!s.headPublished))))return false;
            expected=expected->next00;
        }else if(i+1!=f.secondaryCount)return false;
    }
    if(observer->next00!=head||observer->portalCursor!=expected||observer->cursorMethod!=expectedMethod)return false;
    return !f.secondaryReturned||!expected;
}
bool OriginalUiLightManagerOwner50KG::withdrawSourceTypedSecondaryNative(
    ::ra_pod_scene50LA::Owner& world,std::size_t ordinal,Diagnostic& d)noexcept {
    d={};if(!preflightSourceTypedSecondaryNative(world,ordinal))return fail(d,Status::phase,0);
    auto& f=*world.sourceTypedLights_[ordinal-2];if(!f.secondaryEntered||f.secondaryNativeWithdrawn)return true;
    auto& o=*f.light->observerOwner;auto& pool=entities_->garageObserverShared_;
    for(std::size_t i=f.secondaryCount;i>0;--i){auto& s=f.secondary[i-1];L::D ld;
        if(s.selectionPending){if(!s.method->cancelSourceLightSelectionNative(&s,ld))return fail(d,Status::list,ld.sourcePC);s.selectionPending=false;}
        if(s.inserted&&!s.nativeWithdrawn){if(!s.method->withdrawSourceLightNative(this,issuer_,entities_,f.light->entity,*s.slot,ld))
            return fail(d,Status::list,ld.sourcePC);s.nativeWithdrawn=true;}
        if(s.link&&!s.linkReleased){
            if(s.headPublished&&o.next00!=s.link)return fail(d,Status::identity,0);
            auto* next=s.headPublished?s.link->next00:o.next00;std::uint32_t pc=0;
            if(!pool.releaseSource105830(raZones_->world->sourceDefaultObserver_.get(),&o,s.link,pc))return fail(d,Status::phase,pc);
            o.next00=next;s.linkReleased=true;s.link=nullptr;
        }
    }
    for(auto& s:f.secondary){s.edge=nullptr;s.neighbor=nullptr;s.portal=nullptr;s.method=nullptr;s.oldHead=nullptr;}
    o.portalCursor=nullptr;o.cursorMethod=nullptr;f.secondaryNativeWithdrawn=true;return true;
}
bool OriginalUiLightManagerOwner50KG::continueSourceTypedSecondary280BF0(
    ::ra_cold_aggregate50LA::Owner& world,std::size_t ordinal,Diagnostic& d)noexcept {
    d={};if(ordinal<2||ordinal>=185||ordinal-2>=world.sourceTypedLights_.size()||!world.preflightSourceTypedLightsNative()||
       !preflightSourceRaZonesNative(world)||world.sourceTopologyLightManager_.get()!=this)return fail(d,Status::phase,0x280bf0);
    auto& f=*world.sourceTypedLights_[ordinal-2];auto& l=*f.light;
    if(f.secondaryEntered||f.observerWithdrawn||!l.observerOwner||f.primaryStoreMask!=31||f.pc!=0x280bf0)
        return fail(d,Status::phase,0x280bf0);
    auto& o=*l.observerOwner;auto& r=*raZones_;auto& pool=entities_->garageObserverShared_;
    const auto at=[&](std::uint32_t pc){f.pc=pc;d.sourcePC=pc;};
    const auto stop=[&](std::uint32_t pc){at(pc);return fail(d,Status::phase,pc);};
    f.secondaryEntered=true;o.portalLoopStarted=true;
    at(0x280c1c);if(o.child04!=&r.zones[static_cast<std::size_t>(f.descriptor->zone20)]->entity)return stop(0x280c1c);
    at(0x280c20);o.cursorMethod=o.method08;
    at(0x280c34);o.portalCursor=r.zones[static_cast<std::size_t>(f.descriptor->zone20)]->field30;
    while(o.portalCursor){
        std::size_t index=0;while(index<20&&r.portalLinks[index].get()!=o.portalCursor)++index;
        if(index==20||f.secondaryCount==20)return stop(0x280c40);
        auto& edge=*r.portalLinks[index];auto& s=f.secondary[f.secondaryCount++];
        s.entered=true;s.edgeIndex=index;s.edge=&edge;s.neighbor=edge.neighbor04;s.portal=edge.portal08;
        at(0x280c40);const auto& portal=world.nativePortals_[index/2];
        if(!portal||!portal->sourceMode||&portal->entity!=s.portal||!portal->source.loadReturned||
           portal->source.normalKnown!=7||!portal->source.normalNormalized||portal->source.copiedVertices!=4)
            return stop(0x231150);
        for(unsigned e=0;e<4;++e)if(portal->source.edgeKnown[e]!=7||!portal->source.edgeNormalized[e])return stop(0x231150);
        //Borrow the exact prepared source XYZ words; W of normal/edges remains unknown.
        at(0x231150);if(!game::source_portal_overlap231150::run(portal->source.vertices30,portal->source.normal70,
            portal->source.edges80,l.positionA0,l.range84,s.overlap))return stop(s.overlap.pc);
        at(0x280c54);if(!s.overlap.hit){s.complete=true;at(0x280cf8);o.portalCursor=edge.next00;continue;}
        if(!s.overlap.outputDistance)return stop(0x280c5c);
        at(0x280c60);edge.field10=*s.overlap.outputDistance;s.edgeDistanceMask|=1;
        at(0x280c64);edge.field0C=*s.overlap.outputDistance;s.edgeDistanceMask|=2;edge.distanceWritten=true;
        at(0x280c68);Zone* neighbor=nullptr;for(const auto& z:r.zones)if(z&&&z->entity==s.neighbor)neighbor=z.get();
        at(0x280c6c);if(!o.cursorMethod)return stop(0x280c6c);const auto usage=o.cursorMethod->usage10_;
        if(!neighbor)return stop(0x27f5a0);
        s.method=(usage&1u)?neighbor->first34:(usage&2u)?neighbor->second38:nullptr;
        s.methodReturned=true;at(0x280c7c);o.cursorMethod=s.method;
        if(!s.method)return stop(0x280c78);
        L::D ld;std::uint32_t slot=0;s.selectionAttempted=true;at(0x280c90);
        const bool selected=s.method->selectSourceLightSlot(this,issuer_,entities_,&s,l.entity,
            static_cast<std::uint32_t>(ordinal),l.positionA0,entities_->lightVector426F10_,slot,ld);
        s.selectionPending=s.method->sourceLightOccurrence_==&s;
        if(!selected)return stop(ld.sourcePC);s.slot=slot;
        at(0x280c9c);if(!slot){ //Original branch skips insertion/allocator; selection does not reserve a slot.
            if(s.selectionPending&&!s.method->cancelSourceLightSelectionNative(&s,ld))return stop(ld.sourcePC);
            s.selectionPending=false;s.complete=true;at(0x280cf8);o.portalCursor=edge.next00;continue;
        }
        at(0x280cbc);if(!s.method->insertSourceLightSelected(this,issuer_,entities_,&s,l.entity,
            static_cast<std::uint32_t>(ordinal),l.positionA0,entities_->lightVector426F10_,slot,ld))return stop(ld.sourcePC);
        s.selectionPending=false;s.inserted=true;
        at(0x280cc8);s.allocationAttempted=true;
        const auto allocated=pool.allocateSource105720(world.sourceDefaultObserver_.get(),&o);
        if(allocated.status==game::OriginalGarageObserverShared50KG::SourceLinkStatus::nativeRefused)return stop(allocated.sourcePC);
        s.allocationReturned=true;s.link=allocated.record;
        at(0x280cd0);if(!s.link)return stop(0x280cd0); //Original NULL store AFTER List insertion; retain that membership.
        s.link->slot14=slot;s.linkStoreMask|=1;
        at(0x280cd4);s.link->child04=s.neighbor;s.linkStoreMask|=2;
        at(0x280cd8);s.link->method08=s.method;s.linkStoreMask|=4;
        at(0x280cdc);at(0x280ce0);s.link->portal0C=edge.portal08;s.linkStoreMask|=8;
        at(0x280ce4);at(0x280ce8);s.link->distance10=*s.overlap.outputDistance;s.linkStoreMask|=16;
        at(0x280cec);s.oldHead=o.next00;
        at(0x280cf0);s.link->next00=s.oldHead;s.linkStoreMask|=32;
        at(0x280cf4);o.next00=s.link;s.linkStoreMask|=64;s.headPublished=true;
        s.complete=true;at(0x280cf8);o.portalCursor=edge.next00;
    }
    at(0x280d30);f.secondaryReturned=true;o.portalLoopComplete=true;
    at(0x280be4); //2809C0 epilogue follows the represented280BF0 return.
    at(0x14757c);d={Status::complete,0x14757c};return true; //1474A0 postcondition assertion/return not yet admitted.
}

bool OriginalUiLightManagerOwner50KG::finishSourceTypedRegistration149088(
    ::ra_cold_aggregate50LA::Owner& world,std::size_t ordinal,Diagnostic& d)noexcept {
    d={};if(ordinal<2||ordinal>=185||ordinal-2>=world.sourceTypedLights_.size()||!world.preflightSourceTypedLightsNative()||
       !preflightSourceRaZonesNative(world)||world.sourceTopologyLightManager_.get()!=this||world.sourceLoadApplication_!=issuer_||
       issuer_->sourceLightManager426F60Owner_.get()!=this)return fail(d,Status::phase,0x14757c);
    auto& f=*world.sourceTypedLights_[ordinal-2];auto& l=*f.light;
    if(f.pc!=0x14757c||!f.secondaryReturned||f.observerWithdrawn||f.observerCallReturned||f.registrationEntered||
       !l.observerOwner||!l.observerOwner->portalLoopComplete||!f.descriptor||
       (f.descriptor->type00!=1&&f.descriptor->type00!=3)||l.type34!=f.descriptor->type00||
       (l.type34==3&&(!f.spotlightConfigEntered||!f.spotlightCone9C))||
       f.descriptor->zone20<0||static_cast<std::uint32_t>(f.descriptor->zone20)>=count90_||
       l.ordinalC8!=static_cast<std::int32_t>(ordinal)||l.registered||
       !capacity98_||capacity98_>190||capacity98_%10||registryWritten_!=count94_||f.registryIndex||
       !registry9C_||registryFailed_||raRegistryGrowthFailed_||!raZones_->sourceSecondRegistered||raZones_->sourceSecondWithdrawn)
        return fail(d,Status::phase,0x14757c); //Native exact-domain admission, before postcondition effects.
    const auto at=[&](std::uint32_t pc){f.pc=pc;d.sourcePC=pc;};
    const auto stop=[&](std::uint32_t pc){at(pc);return fail(d,Status::phase,pc);};
    using Shared=game::OriginalGarageObserverShared50KG;
    at(0x14757c);if(!Shared::beginSourceAssertion(7,f.observerReturnAssertion))return stop(0x14757c);
    //1475A8 actual D0 load,1475B4 actual child04 load. No boolean proxy.
    if(f.observerReturnAssertion.enabled&&(!l.observerD0||l.observerD0!=l.observerOwner.get()))return stop(0x1475b4);
    at(0x1475b8);if(!Shared::finishSourceAssertion(f.observerReturnAssertion,l.observerOwner->child04!=nullptr))return stop(0x1475b8);
    at(0x1475f8);f.observerCallReturned=true;
    at(0x149088);f.registrationEntered=true;f.registryIndex=count94_;f.registryManager=this;
    //149090 selects type2;14909C selects type1. Type3 skips ALL vector30 stores.
    if(l.type34==1){
        constexpr std::uint32_t rgbStores[]{0x1490a8,0x1490b0,0x1490b8};
        for(unsigned i=0;i<3;++i){at(rgbStores[i]);std::uint32_t word=0;std::memcpy(&word,&l.diffuse3C[i],sizeof(word));
            vector30_[i]=word;f.registryColorWords[i]=word;f.registryColorMask|=1u<<i;}
        at(0x1490bc);vector30_[3]=0;f.registryColorWords[3]=0;f.registryColorMask|=8;
    }else at(0x1490c0);
    //Type1/3 both leave field40/directional44 alone. Only type1 writesA0.
    if(l.type34==1){at(0x149178);fieldA0_=1;f.registryA0Stored=true;}
    else at(0x149170); //Type3 delay loads count; no A0 write.
    at(0x149188);
    if(capacity98_<count94_+1u){
        at(0x149194);f.registryGrowthEntered=true;
        //Existing native adapter preserves source capacity-before-call ordering;
        //native refusal retains old storage, terminally, NOT guest NULL return.
        if(!growRaRegistry149194(d))return stop(d.sourcePC);
        at(0x1491ac);f.registryGrowthReturned=true;
    }
    if(count94_!=*f.registryIndex||registryWritten_!=count94_||!registry9C_||capacity98_<=count94_)
        return stop(0x1491b4);
    at(0x1491c0);registry9C_[*f.registryIndex]=&l.entity;registryPublished_[*f.registryIndex]=&l.entity;
    registryWritten_=*f.registryIndex+1u;
    f.registryOwner=&l.entity;f.registrySlotPublished=true;
    at(0x1491cc);++count94_;f.registryCountStored=true;l.registered=true;
    at(0x1491e0);f.registrationReturned=true;d={Status::complete,0x1491e0};return true;
}
bool OriginalUiLightManagerOwner50KG::withdrawSourceTypedRegistryNative(
    ::ra_pod_scene50LA::Owner& world,std::size_t ordinal,Diagnostic& d)noexcept {
    d={};if(ordinal<2||ordinal>=185||ordinal-2>=world.sourceTypedLights_.size()||!raZones_||!raZones_->world||
       static_cast<const ::ra_pod_scene50LA::Owner*>(raZones_->world)!=&world||!preflightSourceRaZonesNative(*raZones_->world))
        return fail(d,Status::phase,0);
    auto& f=*world.sourceTypedLights_[ordinal-2];if(!f.registryCountStored||f.registryWithdrawn)return true;
    if(!f.light||!f.light->registered||f.registryOwner!=&f.light->entity||!f.registrySlotPublished||
       !f.registryIndex||count94_!=*f.registryIndex+1u||!registry9C_||
       registry9C_[*f.registryIndex]!=f.registryOwner||registryPublished_[*f.registryIndex]!=f.registryOwner)
        return fail(d,Status::identity,0);
    --count94_;f.light->registered=false;f.registryWithdrawn=true;
    //Native reverse-order withdrawal only. Retain the actual dead slot,
    //registryWritten/capacity/vector30/A0 history; no1491F0 replay or compaction.
    return true;
}

bool OriginalUiLightManagerOwner50KG::incrementSourceSecond149300(::ra_cold_aggregate50LA::Owner& world,unsigned stage,Diagnostic& d)noexcept {
    d={};
    if(stage>=4||!preflightSourceRaZonesNative(world)||!raZones_||!raZones_->sourceReturned||raZones_->sourceNativeClosed||
       !raZones_->sourceAmbientRegistered||raZones_->sourceAmbientWithdrawn||raZones_->sourceSecondRevisionBefore[stage]||
       !world.preflightSourceSecondLightNative()||!world.sourceSecondLight_||
       world.sourceTopologyLightManager_.get()!=this||world.sourceLoadApplication_!=issuer_)
        return fail(d,Status::phase,0x149300);
    auto& l=*world.sourceSecondLight_;
    if(stage==0){if(!l.colorsWritten||l.revisionWritten)return fail(d,Status::phase,0x149300);}
    else if(stage==1){if(!l.revisionWritten||!world.sourceSecondLuminance_||!l.positionWritten||l.positionRevision)
        return fail(d,Status::phase,0x149300);}
    else if(stage==2){if(!l.revisionWritten||!world.sourceSecondPositionCompared_||(l.positionWritten&&!l.positionRevision)||
            !l.lookWritten||l.lookRevision)return fail(d,Status::phase,0x149300);}
    else if(!world.sourceSecondLookCompared_||(l.lookWritten&&!l.lookRevision)||!world.sourceSecondAttenuationWords_||
            !world.sourceSecondAttenuationRange_||!l.rangeWritten||l.rangeRevision)return fail(d,Status::phase,0x149300);
    raZones_->sourceSecondRevisionBefore[stage]=field84_;
    ++field84_; //Same149304 unsigned32 revision; no new registry or readiness.
    d={Status::complete,0x149310};return true;
}

bool OriginalUiLightManagerOwner50KG::incrementSourceAmbient149300(::ra_cold_aggregate50LA::Owner& world,Diagnostic& d)noexcept {
    d={};if(!preflightSourceRaZonesNative(world)||!raZones_||!raZones_->sourceReturned||raZones_->sourceNativeClosed||
       raZones_->sourceAmbientRevisionBefore||!world.preflightSourceFirstLightNative()||!world.sourceFirstLight_||
       !world.sourceFirstLight_->colorsWritten||world.sourceFirstLight_->revisionWritten||
       world.sourceTopologyLightManager_.get()!=this||world.sourceLoadApplication_!=issuer_)
        return fail(d,Status::phase,0x149300);
    raZones_->sourceAmbientRevisionBefore=field84_;
    ++field84_; //Actual149304 unsigned32 wrap, not a reset or invented revision.
    d={Status::complete,0x149310};return true;
}
bool OriginalUiLightManagerOwner50KG::appendSourceAmbient148F30(::ra_cold_aggregate50LA::Owner& world,Diagnostic& d)noexcept {
    d={};if(!preflightSourceRaZonesNative(world)||!raZones_||!raZones_->sourceReturned||raZones_->sourceNativeClosed||
       raZones_->sourceAmbientAttempted||!raZones_->sourceAmbientRevisionBefore||!world.sourceFirstLight_||
       !world.preflightSourceFirstLightNative()||!world.sourceFirstLight_->configured||world.sourceFirstLight_->registered||
       !world.sourceLightPopulationDescriptor_||world.sourceLightPopulationDescriptor_->zone20||
       world.sourceTopologyLightManager_.get()!=this||world.sourceLoadApplication_!=issuer_)
        return fail(d,Status::phase,0x148f30);
    //Only constructor-derived zero registry admitted. Warm retained source
    //backing is not adopted, cleared or falsely described as capacity10.
    if(count94_||capacity98_||registry9C_||registryWritten_||registryFailed_||raRegistryGrowthFailed_)
        return fail(d,Status::phase,0x148f30); //Native admission boundary, before any callee effect.
    if(!ambient60_||!ambient80_||count90_!=10)return fail(d,Status::phase,0x148f7c);
    auto& r=*raZones_;auto& light=*world.sourceFirstLight_;
    r.sourceAmbientAttempted=true;
    constexpr Words ambient{{0x42480000u,0x42480000u,0x42480000u,0x42800000u}};
    //148F7C..149040 actual zone0 ambient writes and ten-row propagation.
    default50_=default70_=ambient;for(std::size_t i=0;i<10;++i)ambient60_[i]=ambient80_[i]=ambient;
    r.sourceAmbientColors=true;
    //149184 count+1;149188 signed capacity<count+1 is true for proven0/0. Actual149198
    //capacity=count+10 precedes3080C0(NULL,40);1491AC publishes even NULL.
    capacity98_=10;r.sourceAmbientAllocation=true;
    registry9C_.reset(new(std::nothrow) const C*[10]);r.sourceAmbientPublished=true;
    if(!registry9C_){registryFailed_=true;return fail(d,Status::allocation,0x1491c0);}
    registry9C_[0]=&light.entity;registryPublished_[0]=&light.entity;registryWritten_=1;
    ++count94_;r.sourceAmbient=&light.entity;r.sourceAmbientRegistered=true;light.registered=true;
    d={Status::complete,0x1491e0};return true;
}
bool OriginalUiLightManagerOwner50KG::withdrawSourceAmbientNative(::ra_cold_aggregate50LA::Owner& world,Diagnostic& d)noexcept {
    d={};if(!preflightSourceRaZonesNative(world))return fail(d,Status::phase,0x148110);
    if(!raZones_||!raZones_->sourceMode||raZones_->sourceWorldIdentity!=&world)return true;
    auto& r=*raZones_;if(r.sourceAmbientWithdrawn)return true;
    if(r.sourceAmbientRegistered){
        if(!world.sourceFirstLight_||r.sourceAmbient!=&world.sourceFirstLight_->entity||count94_!=1)
            return fail(d,Status::identity,0x1491f0);
        --count94_;world.sourceFirstLight_->registered=false;r.sourceAmbient=nullptr;
    }
    //Retain actual capacity, NULL failure, known dead slot and revision history.
    //This withdraws native lifetime only; no fake guest destructor/rollback.
    r.sourceAmbientWithdrawn=true;return true;
}
}
