#pragma once
#include <new>
#include "../../src/game/source_acos49t.h"
namespace ra_pod_scene50LA {
bool Owner::preflightSourceFirstLightNative()const noexcept {
    if(!sourceFirstLight_){
        return !sourceFirstLightCounted_&&!sourceFirstLightEnrolled_&&!sourceFirstLightDefaults_&&
            !sourceFirstLightReturned_&&!sourceFirstLightVtable08_&&!sourceFirstLightNativeDetached_&&
            (!sourceFirstLightPublished_||(sourceFirstLightOuterReached_&&nativeLightList3A8_&&
             nativeLightList3A8_->wrappers38&&!nativeLightList3A8_->wrappers38[0].live00));
    }
    if(!sourceFirstLightOuterReached_||!worldApplication_||worldApplication_->attachmentLock423F58()||
       !nativeLightList3A8_||!nativeLightList3A8_->sourceMode||!nativeLightList3A8_->wrappers38)return false;
    const auto& l=*sourceFirstLight_;const auto& e=l.entity;const auto& authority=worldApplication_->entityAuthority_;
    if(sourceFirstLightCounted_){
        if(e.reference00!=1||e.kind!=g::OriginalWreckComponentKind50LA::light146720||e.objectKey!=key(&e)||
           e.sessionGeneration!=worldEntity_.sessionGeneration||e.category28!=0xff||(e.flags29&15u)!=3u||
           e.nativeFlags29||e.parent1C!=(l.attached?&nativeLightList3A8_->entity:nullptr)||e.firstChild18||e.previous10||e.next14||
           e.mask20!=((l.attached||sourceFirstLightNativeDetached_)?2u:0u)||e.descendants24||
           !worldApplication_->objectCount423F60())return false;
    }else if(e.reference00)return false;
    if(sourceFirstLightEnrolled_){if(!sourceFirstLightCounted_||!authority.canRetire(e))return false;}
    else for(std::size_t i=0;i<authority.count_;++i)if(authority.entries_[i].node==&e)return false;
    if(sourceFirstLightDefaults_){
        if(!sourceFirstLightCounted_||!sourceFirstLightEnrolled_||!sourceFirstLightVtable08_||*sourceFirstLightVtable08_!=0x3cd600||
           l.query30||l.flags33||l.knownFlags33!=7||l.type34!=(l.colorsWritten?4u:0u)||l.mode38!=3||
           l.diffuse3C!=(l.colorsWritten?std::array<float,4>{}:std::array<float,4>{{128,128,128,0}})||
           l.specular4C!=l.diffuse3C||l.ambient5C!=(l.colorsWritten?std::array<float,4>{{50,50,50,127.5f}}:l.diffuse3C)||
           l.position6C!=std::array<float,3>{}||l.look78!=std::array<float,3>{{1,0,0}}||
           l.range84!=g::SourceUiLightMath50KG::range146898()||l.field88!=0x3f800000||
           l.attenuation8C!=std::array<float,3>{{1,0,0}}||l.cone98!=0x3f490fdb||l.cone9C!=0x3fc90fdb||
           l.positionA0!=worldApplication_->lightVector426F10_||l.lookB0!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000}}||
           l.fieldC0||l.fieldC4||l.ordinalC8!=(l.colorsWritten?0:-1)||l.luminanceCC!=(l.configured?0u:0xbf800000u)||l.observerD0||l.fieldD4||l.fieldD8)return false;
    }else if(sourceFirstLightVtable08_||sourceFirstLightReturned_)return false;
    if((l.colorsWritten&&!sourceFirstLightPublished_)||(l.revisionWritten&&!l.colorsWritten)||
       (l.configured&&!l.revisionWritten)||(l.registered&&!l.configured)||
       (l.attached&&(!l.registered||sourceFirstLightNativeDetached_))||l.retiring||l.observerOwner)return false;
    const auto& parent=nativeLightList3A8_->entity;
    if(parent.firstChild18!=(l.attached?&e:nullptr)||parent.descendants24!=(l.attached?2u:0u))return false;
    if(sourceFirstLightReturned_!=l.typed||l.constructed!=sourceFirstLightReturned_||
       (!l.typed&&l.query32!=0xff)||(l.typed&&l.query32==0xff))return false;
    if(sourceFirstLightPublished_&&(!sourceFirstLightReturned_||nativeLightList3A8_->wrappers38[0].live00!=&e))return false;
    return true;
}
bool Owner::constructSourceFirstLight146720(std::uint32_t& pc)noexcept {
    //Native admission and ledger reserve precede nullable guest allocation.
    //An admission failure is NOT a source allocationNULL or constructor return.
    if(!preflightSourceFirstLightNative()||sourceFirstLightOuterReached_||!worldApplication_||
       !nativeLightList3A8_||!nativeLightList3A8_->sourceMode||!nativeLightList3A8_->wrappers38||
       nativeLightList3A8_->wrappers38[0].live00||!reserveSourceAmbientListNative())return false;
    auto& authority=worldApplication_->entityAuthority_;
    pc=0x277e0c;sourceFirstLightOuterReached_=true;
    sourceFirstLight_.reset(new(std::nothrow) NativeRaLight146720);
    if(sourceFirstLight_){auto& l=*sourceFirstLight_;auto& e=l.entity;
        pc=0x14672c;if(!authority.canEnroll(e,key(&e)))return false;
        const auto made=g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::light146720,
            worldEntity_.sessionGeneration,key(&e),true,worldApplication_->objectCount423F60());
        sourceFirstLightCounted_=e.reference00!=0;
        if(made.status!=g::OriginalWreckComponentStatus50LA::complete){pc=made.sourcePC;return false;}
        if(!authority.enroll(e))return false;sourceFirstLightEnrolled_=true;
        //146744..146934 exact constructor stores. Typed storage carries these
        //known defaults, but they become source knowledge only after this block.
        //Flags33 upper five bits remain unknown (knownFlags33==7), never zero-claimed.
        pc=0x1467a8;sourceFirstLightVtable08_=0x3cd600;
        pc=0x1468c0;l.range84=g::SourceUiLightMath50KG::range146898();
        pc=0x146924;l.positionA0=worldApplication_->lightVector426F10_; //Actual3C8A0C producer.
        pc=0x146934;sourceFirstLightDefaults_=true;
        pc=0x146938;const auto query=worldApplication_->types383408().lightEmitter146938();
        if(!query.okay()){pc=query.sourcePC;return false;}
        pc=0x146964;l.query32=query.type;l.typed=true;
        pc=0x146970;l.constructed=true;sourceFirstLightReturned_=true;
    }
    pc=0x277e28;nativeLightList3A8_->wrappers38[0].live00=sourceFirstLight_?&sourceFirstLight_->entity:nullptr;
    sourceFirstLightPublished_=true;return true;
}
bool Owner::sourceFirstLightLuminance146E60(std::uint32_t& out)const noexcept {
    if(!preflightSourceFirstLightNative()||!sourceFirstLight_||!sourceFirstLight_->colorsWritten||
       !sourceFirstLight_->revisionWritten||sourceFirstLight_->configured)return false;
    //Existing pod Owner friendship is the exact source-math authority; no
    //aggregate access or widened arbitrary numeric input permission.
    return g::SourceUiLightMath50KG::luminance146E60(0,0,0,out);
}
bool Owner::attachSourceFirstLight141960(std::uint32_t& pc)noexcept {
    pc=0x273b98;
    if(!preflightSourceFirstLightNative()||!sourceFirstLight_||!sourceFirstLightPublished_||
       !sourceFirstLight_->registered||sourceFirstLight_->attached||sourceFirstLightNativeDetached_||
       !nativeLightList3A8_||!nativeLightList3A8_->sourceMode||!nativeLightList3A8_->sourceAuthorityEnrolled||
       nativeLightList3A8_->wrappers38[0].sourceDescriptor04)return false;
    auto& light=*sourceFirstLight_;auto& parent=nativeLightList3A8_->entity;
    const auto attached=worldApplication_->entityAuthority_.attachChildren(parent,light.entity,2u,
        worldApplication_->attachmentLock423F58());
    if(attached.status!=g::OriginalWreckComponentStatus50LA::complete){pc=attached.sourcePC;return false;}
    light.attached=true;pc=0x273ba0;return true;
}
bool Owner::detachSourceFirstLightNative(std::uint32_t& pc)noexcept {
    if(!preflightSourceFirstLightNative())return false;
    if(!sourceFirstLight_||!sourceFirstLight_->attached)return true;
    const auto detached=worldApplication_->entityAuthority_.detach(sourceFirstLight_->entity,
        worldApplication_->attachmentLock423F58());
    if(detached.status!=g::OriginalWreckComponentStatus50LA::complete){pc=detached.sourcePC;return false;}
    //Same forest splice; native unwind is NOT a guest141A70 call receipt.
    //141A70 semantics retain child mask2, while parent descendant mask becomes0.
    sourceFirstLight_->attached=false;sourceFirstLightNativeDetached_=true;return true;
}
bool Owner::abortSourceFirstLightNative()noexcept {
    if(!preflightSourceFirstLightNative()||(sourceFirstLight_&&(sourceFirstLight_->registered||sourceFirstLight_->attached)))return false;
    if(sourceFirstLight_){auto& e=sourceFirstLight_->entity;
        if(sourceFirstLightEnrolled_&&!worldApplication_->entityAuthority_.retire(e))return false;
        sourceFirstLightEnrolled_=false;
        if(sourceFirstLightCounted_){--e.reference00;--worldApplication_->objectCount423F60();sourceFirstLightCounted_=false;}
    }
    if(sourceFirstLightPublished_)nativeLightList3A8_->wrappers38[0].live00=nullptr;
    sourceFirstLight_.reset();sourceFirstLightOuterReached_=false;sourceFirstLightDefaults_=false;
    sourceFirstLightReturned_=false;sourceFirstLightPublished_=false;sourceFirstLightVtable08_.reset();sourceFirstLightNativeDetached_=false;
    return true; //Native withdrawal only; no146980/manager-removal guest call invented.
}
}
