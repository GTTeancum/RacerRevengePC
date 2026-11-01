#pragma once
#include <limits>
namespace ra_pod_scene50LA {
inline bool Owner::reserveSourceTriggerAuthorityNative()noexcept {
    return !sourceTriggerAttempted_&&!nativeTriggerList3D0_&&worldApplication_&&worldApplication_->application10()&&
        !worldApplication_->attachmentLock423F58()&&worldEntity_.reference00==1&&worldEntity_.sessionGeneration&&
        worldApplication_->objectCount423F60()!=std::numeric_limits<std::uint32_t>::max()&&
        worldApplication_->entityAuthority_.reserveAdditional(1);
}
inline bool Owner::constructSourceTriggerList2DC320(std::uint32_t& pc)noexcept {
    pc=0x2dc32c;if(!sourceTriggerAttempted_||!nativeTriggerList3D0_||!nativeTriggerList3D0_->sourceMode||!worldApplication_)return false;
    auto& l=*nativeTriggerList3D0_;auto& a=worldApplication_->entityAuthority_;
    if(l.counted||l.sourceEnrolled||!a.canEnroll(l.entity,key(&l.entity)))return false;
    const auto r=g::initializeOriginalWreckComponent50LA(l.entity,g::OriginalWreckComponentKind50LA::triggerList2DC320,
        worldEntity_.sessionGeneration,key(&l.entity),true,worldApplication_->objectCount423F60());
    if(r.status!=g::OriginalWreckComponentStatus50LA::complete){pc=r.sourcePC;return false;}
    l.counted=true;if(!a.enroll(l.entity)){pc=0x2dc334;return false;}l.sourceEnrolled=true;
    pc=0x2dc33c;l.sourceVtable08=0x3d5230;l.sourceVtableKnown=true;
    pc=0x2dc340;l.count30=0;l.sourceCountKnown=true;
    pc=0x2dc344;l.descriptors34.reset();l.sourceArrayKnown=true;return true;
}
inline bool Owner::canRetireSourceTriggerNative()const noexcept {
    if(!nativeTriggerList3D0_)return true;
    const auto& l=*nativeTriggerList3D0_;const auto& e=l.entity;
    if(!sourceTriggerAttempted_||!l.sourceMode||!worldApplication_||worldApplication_->attachmentLock423F58()||
       l.candidates||l.authoring||l.actions||l.complete||l.joined||l.serviceEntered||l.serviceComplete||l.servicesCompleted||
       l.descriptorsRetired||l.descriptorSlotsPublished>2||(!l.descriptors34&&l.descriptorSlotsPublished)||
       (l.sourceArrayAllocationReturned&&(!l.sourceCountKnown||l.count30!=2))||
       (l.descriptors34&&!l.sourceArrayAllocationReturned))return false;
    for(std::size_t i=0;i<2;++i){if(l.observers[i]||l.parented[i]||l.geometryRoleConsumed[i])return false;
        const auto& d=l.descriptorOwners[i];
        if(l.sourceDescriptorNativeRetired[i]){if(d)return false;continue;}
        if(i<l.descriptorSlotsPublished&&l.descriptors34[i]!=d.get())return false;
        if(d&&(!d->sourceFields||!preflightSourceTriggerInstanceNative(*d)||d->authoring||!preflightSourceTriggerActionNative(*d)||d->complete))return false;
        if(d){const auto& f=*d->sourceFields;
            if(f.geometrySourceFreeEntered!=f.geometrySourceHistory.has_value()||
               (f.geometrySourceFreeEntered&&(!f.geometryAllocationReturned||!f.geometryPublished||!f.geometryConstructed))||
               (f.geometrySourceFreed&&!f.geometrySourceFreeEntered)||(f.geometrySourceNullStored&&!f.geometrySourceFreed))return false;
            if(f.geometrySourceFreed){if(f.geometry64||!f.geometrySourceNullStored||!f.sourceGeometry64||*f.sourceGeometry64)return false;}
            else if(f.geometrySourceHistory&&(!f.geometry64||
                std::memcmp(&*f.geometrySourceHistory,f.geometry64.get(),sizeof(SourceTriggerGeometry2DBEB4))))return false;
            if((f.known&4)&&!f.sourceGeometry64)return false;
            if(f.geometryPublished&&(!f.sourceGeometry64||*f.sourceGeometry64!=f.geometry64.get()))return false;
            if(!f.geometryPublished&&f.sourceGeometry64&&*f.sourceGeometry64)return false;
            if(f.geometry64&&(!f.geometryAllocationReturned||!f.geometryPublished||!f.geometryConstructed))return false;}
    }
    //Only genuinely source-registered trigger children inhabit the source list.
    //Compare exact owned addresses before following any sibling pointer.
    const Entity* cursor=e.firstChild18;const Entity* previous=nullptr;std::uint32_t descendants=0;
    for(const auto& d:l.descriptorOwners)if(d&&d->sourceInstance){const auto& t=*d->sourceInstance;
        if(t.serviceRegistered_&&!t.serviceTopologyWithdrawn_){
            if(t.serviceParent_!=&e||cursor!=&t.entity_||t.entity_.parent1C!=&e||
               t.entity_.previous10!=previous||t.entity_.mask20!=0x402u)return false;
            descendants|=t.entity_.mask20|t.entity_.descendants24;previous=&t.entity_;cursor=t.entity_.next14;
        }
    }
    if(cursor||e.descendants24!=descendants)return false;
    if(worldApplication_->objectCount423F60()<sourceTriggerInstanceCountNative()+std::size_t(l.counted))return false;
    if(!l.counted)return !l.sourceEnrolled&&!e.reference00;
    if(e.reference00!=1||e.previous10||e.next14||e.parent1C||e.mask20||e.nativeFlags29||
       e.objectKey!=key(&e)||e.sessionGeneration!=worldEntity_.sessionGeneration||!worldApplication_->objectCount423F60())return false;
    const auto& a=worldApplication_->entityAuthority_;
    if(l.sourceEnrolled)return a.canRetire(e);
    for(std::size_t i=0;i<a.count_;++i)if(a.entries_[i].node==&e)return false;
    return true;
}
inline bool Owner::retireSourceTriggerCountNative(std::uint32_t& pc)noexcept {
    pc=0;if(!canRetireSourceTriggerNative())return false;
    if(!nativeTriggerList3D0_)return true;
    auto& l=*nativeTriggerList3D0_;
    //Detach while root AND children still belong to the authority forest.
    if(!withdrawSourceTriggerServiceMembershipNative())return false;
    if(l.sourceEnrolled&&!worldApplication_->entityAuthority_.retire(l.entity))return false;
    l.sourceEnrolled=false;
    //No source destructor calls claimed. After native authority withdrawal no
    //fallible operation remains: plain payloads first, then actual count/ref.
    for(std::size_t i=2;i>0;--i){auto& d=l.descriptorOwners[i-1];
        if(d&&!retireSourceTriggerActionNative(*d))return false;
        if(d&&!retireSourceTriggerInstanceNative(*d))return false;
        if(d&&d->sourceFields)d->sourceFields->sourceGeometry64.reset();
        l.sourceDescriptorNativeRetired[i-1]=true;d.reset();}
    l.sourceArrayKnown=false;l.descriptors34.reset();l.descriptorsRetired=true;
    if(l.counted){--l.entity.reference00;--worldApplication_->objectCount423F60();l.counted=false;}return true;
}
} //namespace ra_pod_scene50LA
