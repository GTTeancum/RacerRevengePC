#pragma once
//Shared source wrapper retirement and caller141960 suffix. No loop issuer.
namespace ra_pod_scene50LA {
bool Owner::preflightSourceLightSiblingLinksNative()const noexcept {
    //Native-adopted lists have their own topology authority. Source-unreached
    //cleanup must not interpret that existing native chain as an empty source list.
    if(nativeLightList3A8_&&!nativeLightList3A8_->sourceMode)
        return !sourceFirstLight_&&!sourceSecondLight_&&!sourceFirstLightOuterReached_&&
            !sourceSecondLightOuterReached_&&sourceTypedLights_.empty();
    using C=g::OriginalWreckComponent50LA;
    const C* previous=nullptr;const C* head=nullptr;std::uint32_t descendants=0;
    const auto visit=[&](const NativeRaLight146720* light)->bool {
        if(!light)return true;const auto& e=light->entity;
        if(!light->attached)return !e.parent1C&&!e.previous10&&!e.next14;
        if(!nativeLightList3A8_||e.parent1C!=&nativeLightList3A8_->entity||e.previous10!=previous||e.mask20!=2u||
           (previous&&previous->next14!=&e))return false;
        if(!head)head=&e;previous=&e;descendants|=e.mask20|e.descendants24;return true;
    };
    if(!visit(sourceFirstLight_.get())||!visit(sourceSecondLight_.get()))return false;
    for(const auto& f:sourceTypedLights_)if(!f||!visit(f->light.get()))return false;
    if(previous&&previous->next14)return false;
    if(!nativeLightList3A8_)return !head;
    return nativeLightList3A8_->entity.firstChild18==head&&nativeLightList3A8_->entity.descendants24==descendants;
}
}
namespace ra_cold_aggregate50LA {
bool Owner::retireAndAttachSourceTypedWrapper2780AC(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||!worldApplication_||
       !preflightSourceTypedLightsNative()||!sourceTopologyLightManager_||!nativeLightList3A8_||
       !nativeLightList3A8_->sourceAuthorityEnrolled||!sourceLight3A8_||*sourceLight3A8_!=nativeLightList3A8_.get()||
       f.ordinal<2||f.ordinal-2>=sourceTypedLights_.size()||sourceTypedLights_[f.ordinal-2].get()!=&f||
       !sourceLightPopulationS0_||*sourceLightPopulationS0_!=static_cast<std::int32_t>(f.ordinal)||
       !f.light||!f.descriptor||!f.wrapper||f.wrapper->sourceDescriptor04.get()!=f.descriptor||
       f.wrapper->live00!=&f.light->entity||!f.light->registered||f.light->attached||!f.registrationReturned||
       f.retirementEntered||!f.argumentsBorrowed||!f.ordinalStack08||worldApplication_->attachmentLock423F58())return false;
    auto& d=*f.descriptor;auto& light=*f.light;
    if(!d.loaded||d.knownFlags28!=0x7f||(d.flags28&0x28))return false; //Lens/flasher not joined here.
    if(d.type00==1){if(f.pc!=0x2780ac||!(d.flags28&16u)||!f.flareSelected||!f.flareObserverCallReturned||
        !f.flare146AE0Returned||!f.flareC4Published||f.flareC4Withdrawn||!f.sourceFlare||!f.flareArgumentsBorrowed)return false;}
    else if(d.type00==3||d.type00==4){if(f.pc!=0x277fd4||(d.flags28&16u))return false;}
    else return false;
    if(d.type00==4){if(d.position14||d.look18||d.attenuation1C)return false;}
    else if(!d.position14||!d.look18||!d.attenuation1C)return false;
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};
    const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(d.type00!=1){at(0x277fd8);f.flareFlagRead=true;f.flareSelected=false;at(0x277fe4);}
    at(0x2780ac);at(0x2780b0);at(0x2780bc); //Actual known-false bit5; no146B10.
    SourceTypedLightFrame::DescriptorHistory history;
    history.type=d.type00;history.zone=d.zone20;history.flags=d.flags28;history.knownFlags=d.knownFlags28;history.loaded=d.loaded;
    if(d.position14)history.position=*d.position14;if(d.look18)history.look=*d.look18;if(d.attenuation1C)history.attenuation=*d.attenuation1C;
    f.retiredDescriptor=history;f.retirementEntered=true;
    //Native borrowers end before the pointed allocations die. These are proof
    //values only; no synthetic guest ownership or source reference decrement.
    f.flarePositionBorrow=nullptr;f.flareTextureBorrow=nullptr;f.flareArgumentsBorrowed=false;
    at(0x2780ec);d.vtableF4=0x3d2b40;
    if(d.position14){at(0x2780fc);d.position14.reset();at(0x278104);f.descriptorFreeMask|=1u;}
    if(d.look18){at(0x278114);d.look18.reset();at(0x27811c);f.descriptorFreeMask|=2u;}
    if(d.attenuation1C){at(0x27812c);d.attenuation1C.reset();at(0x278134);f.descriptorFreeMask|=4u;}
    f.argumentsBorrowed=false;f.lensStack00=nullptr;f.descriptor=nullptr;
    at(0x27813c);f.wrapper->sourceDescriptor04.reset();f.descriptorFreeMask|=8u;
    at(0x278144);f.wrapperV0=1;at(0x278148);f.descriptorDisposed=true;at(0x27815c);f.wrapperReturned=true;
    at(0x273b7c);if(*sourceLight3A8_!=nativeLightList3A8_.get())return stop(pc);
    at(0x273b94);if(f.wrapper->live00!=&light.entity)return stop(pc);
    at(0x273b98);at(0x141970);f.attachLock=worldApplication_->attachmentLock423F58();
    if(!*f.attachLock){const auto r=worldApplication_->entityAuthority_.attachChildren(nativeLightList3A8_->entity,light.entity,2u,*f.attachLock);
        if(r.status!=g::OriginalWreckComponentStatus50LA::complete)return stop(r.sourcePC);
        light.attached=true;
    } //Original locked NULL return is ignored by273B98, not a new failure.
    f.attachCallReturned=true;at(0x273ba0);return true; //Caller owns increment; never issue it here.
}
}
