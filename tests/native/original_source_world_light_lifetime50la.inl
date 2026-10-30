#pragma once
namespace ra_pod_scene50LA {
inline bool Owner::allocateSourceLightList270AA8(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeLightList3A8_||!sourceLight3A8_.has_value()||*sourceLight3A8_||
       !worldColdConstructed_||!worldApplication_||worldEntity_.reference00!=1||
       !worldEntity_.sessionGeneration||worldApplication_->attachmentLock423F58()||
       nativeAmbientLight_||nativeDirectionalLight_||!nativePointOwnersEmpty())return false;
    auto& authority=worldApplication_->entityAuthority_;auto& count=worldApplication_->objectCount423F60();
    if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1))return false;
    pc=0x270aa8;
    try{nativeLightList3A8_.reset(new NativeLightList3A8);}catch(...){nativeLightList3A8_.reset();}
    if(!nativeLightList3A8_){pc=0x270ac4;return true;}
    auto& l=*nativeLightList3A8_;l.sourceMode=true;l.sourcePC=0x27817c;
    if(!authority.canEnroll(l.entity,key(&l.entity))){pc=0x141300;return false;}
    const auto made=g::initializeOriginalWreckComponent50LA(l.entity,
        g::OriginalWreckComponentKind50LA::entity141300,worldEntity_.sessionGeneration,key(&l.entity),true,count);
    if(made.status!=g::OriginalWreckComponentStatus50LA::complete){pc=made.sourcePC;return false;}
    l.counted=true;
    if(!authority.enroll(l.entity)){pc=0;return false;} //Nativeauthority refusal retains counted unpublished allocation.
    l.sourceAuthorityEnrolled=true;
    l.sourcePC=0x27818c; //Actualderived vtable is semantic2781B0/278250/278290 dispatch.
    l.sourcePC=0x278190;l.count30=0;
    l.sourcePC=0x278194;l.wrappers38.reset();
    l.sourcePC=0x278198;l.colors3C.reset();
    l.zoneCount34.reset(); //Constructor leaves34 unknown.
    l.sourceConstructorReturned=true;l.sourcePC=0x2781ac;pc=0x270ac4;return true;
}
inline bool Owner::preflightSourceLightListNative()const noexcept {
    if(!nativeLightList3A8_)return !sourceLight3A8_||*sourceLight3A8_==nullptr;
    const auto& l=*nativeLightList3A8_;const auto& e=l.entity;
    if(!l.sourceMode||l.sourceThread!=std::this_thread::get_id()||!worldApplication_||
       worldApplication_->attachmentLock423F58()||l.canonical||
       nativeAmbientLight_||nativeDirectionalLight_||!nativePointOwnersEmpty()||
       runtimeCameraBorrows_||runtimeWorldBorrows_||constructorRacerWorldBorrows_)return false;
    if(l.sourcePublished&&(!sourceLight3A8_||*sourceLight3A8_!=&l))return false;
    if(!l.counted)return !l.sourcePublished&&!l.sourceAuthorityEnrolled&&!e.reference00&&!l.wrappers38&&!l.colors3C;
    if(e.reference00!=(l.sourceReferenceReleased?0u:1u)||e.kind!=g::OriginalWreckComponentKind50LA::entity141300||
       e.sessionGeneration!=worldEntity_.sessionGeneration||e.objectKey!=key(&e)||
       e.parent1C||e.previous10||e.next14||
       !preflightSourceLightSiblingLinksNative()||e.mask20||
       !worldApplication_->objectCount423F60())return false;
    if(l.sourceAuthorityEnrolled){if(!worldApplication_->entityAuthority_.canRetire(e))return false;}
    else if(l.sourcePublished||l.sourceConstructorReturned)return false;
    if(l.sourceWrappersConstructed>185||l.loaded>185||l.colorsLoaded>10||
       (l.loaded&&!l.wrappers38)||(l.colorsLoaded&&!l.colors3C))return false;
    if(l.wrappers38){for(std::size_t i=0;i<l.sourceWrappersConstructed;++i){const auto& w=l.wrappers38[i];
        const auto* sourceLive=(i==0&&sourceFirstLightPublished_&&sourceFirstLight_)?&sourceFirstLight_->entity:
            (i==1&&sourceSecondLightPublished_&&sourceSecondLight_)?&sourceSecondLight_->entity:nullptr;
        if(i>=2&&i-2<sourceTypedLights_.size()){
            const auto& typed=sourceTypedLights_[i-2];
            if(!typed||typed->ordinal!=i)return false;
            if(!typed->nativeClosed&&typed->published&&typed->light)sourceLive=&typed->light->entity;
        }
        if(!w.sourceConstructed||w.live00!=sourceLive||w.descriptor04||
           (w.sourceDescriptor04&&!w.sourceDescriptorPublished))return false;
    }}
    return true;
}
inline bool Owner::abortSourceLightListNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceLightListNative()||sourceFirstLight_||sourceFirstLightPublished_||sourceSecondLight_||sourceSecondLightPublished_)return false;
    for(const auto& typed:sourceTypedLights_)if(!typed||!typed->nativeClosed||typed->light||typed->wrapper||typed->descriptor)return false;
    // Withdraw actual pointer knowledge BEFORE freeing actual backing; this is
    //native cleanup, not270B0C or a completed source142590/destructor receipt.
    sourceLight3A8_.reset();
    if(!nativeLightList3A8_)return true;auto& l=*nativeLightList3A8_;
    if(l.wrappers38){for(std::size_t i=l.sourceWrappersConstructed;i>0;--i){auto& w=l.wrappers38[i-1];
        if(w.sourceDescriptor04){auto& d=*w.sourceDescriptor04;
            d.position14.reset();d.look18.reset();d.attenuation1C.reset();w.sourceDescriptor04.reset();}
    }}
    l.wrappers38.reset();l.colors3C.reset();l.sourcePayloadReleased=true;
    if(l.counted){
        if(l.sourceAuthorityEnrolled&&!worldApplication_->entityAuthority_.retire(l.entity))std::terminate();
        if(!l.sourceReferenceReleased)--l.entity.reference00;
        --worldApplication_->objectCount423F60();l.counted=false;l.sourceEntityRetired=true;
    }
    nativeLightList3A8_.reset();return true;
}
inline bool Owner::retireSourceLightList142590(std::uint32_t& pc)noexcept {
    pc=0x142590;if(!nativeLightList3A8_||!preflightSourceLightListNative())return false;
    auto& l=*nativeLightList3A8_;
    //This actual caller branch is only loaderfalse before descriptor allocation.
    //Do not reuse it as a blanket source destructor for bounded partial loads.
    if(!l.sourceConstructorReturned||!l.sourceAuthorityEnrolled||l.wrappers38||l.loaded||
       l.sourceArrayAllocationReturned)return false;
    // Native authority withdrawal precedes the genuine final-reference release.
    // All refusal gates above are read-only; no fallible operation follows.
    if(!worldApplication_->entityAuthority_.retire(l.entity))return false;
    l.sourceAuthorityEnrolled=false;
    pc=0x104970;--l.entity.reference00;l.sourceReferenceReleased=true;
    pc=0x2781d4; //Derived vtable restore.
    if(l.colors3C){pc=0x278200;l.colors3C.reset();pc=0x278208;}
    pc=0x278210;
    --worldApplication_->objectCount423F60();l.counted=false;l.sourceEntityRetired=true;
    pc=0x278228;nativeLightList3A8_.reset();return true;
    //Caller stores genuineNULL3A8 at270B0C after this actual+1 destruction.
}
} //namespace ra_pod_scene50LA
