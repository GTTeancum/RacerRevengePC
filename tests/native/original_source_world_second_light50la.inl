#pragma once
#include <new>
#include "../../src/game/original_world_light_assertion273abc.h"
#include "../../src/game/source_acos49t.h"
#include "../../src/game/native_directional_normalize106ea0_proposal.h"
#include "../../src/game/native_directional_attenuation147268_proposal.h"
namespace ra_pod_scene50LA {
bool Owner::preflightSourceSecondLightNative()const noexcept {
    if(sourceSecondDescriptorFreeMask_!=0&&sourceSecondDescriptorFreeMask_!=1&&sourceSecondDescriptorFreeMask_!=3&&
       sourceSecondDescriptorFreeMask_!=7&&sourceSecondDescriptorFreeMask_!=15)return false;
    if(!sourceSecondLight_){
        return !sourceSecondLightCounted_&&!sourceSecondLightEnrolled_&&!sourceSecondLightDefaults_&&
            !sourceSecondLightReturned_&&!sourceSecondLightVtable08_&&!sourceSecondLuminance_&&
            !sourceSecondPositionWords_&&!sourceSecondLookInputWords_&&!sourceSecondNormalizedWords_&&
            !sourceSecondPositionCompared_&&!sourceSecondLookCompared_&&!sourceSecondAttenuationWords_&&
            !sourceSecondAttenuationRange_&&!sourceSecondConfigReturned_&&!sourceSecondLightNativeDetached_&&
            (sourceSecondDescriptorFreeMask_==0||sourceSecondDescriptorFreeMask_==15)&&
            (!sourceSecondLightPublished_||(sourceSecondLightOuterReached_&&nativeLightList3A8_&&
             nativeLightList3A8_->wrappers38&&!nativeLightList3A8_->wrappers38[1].live00));
    }
    if(!sourceFirstLight_||!sourceFirstLight_->attached||!sourceFirstLight_->registered||
       !sourceSecondLightOuterReached_||!worldApplication_||worldApplication_->attachmentLock423F58()||
       !nativeLightList3A8_||!nativeLightList3A8_->sourceMode||!nativeLightList3A8_->wrappers38)return false;
    const auto& l=*sourceSecondLight_;const auto& e=l.entity;const auto& authority=worldApplication_->entityAuthority_;
    if(sourceSecondLightCounted_){
        if(e.reference00!=1||e.kind!=g::OriginalWreckComponentKind50LA::light146720||e.objectKey!=key(&e)||
           e.sessionGeneration!=worldEntity_.sessionGeneration||e.category28!=0xff||(e.flags29&15u)!=3u||
           e.nativeFlags29||e.parent1C!=(l.attached?&nativeLightList3A8_->entity:nullptr)||e.firstChild18||
           !preflightSourceLightSiblingLinksNative()||
           e.mask20!=((l.attached||sourceSecondLightNativeDetached_)?2u:0u)||e.descendants24||
           !worldApplication_->objectCount423F60())return false;
    }else if(e.reference00)return false;
    if(sourceSecondLightEnrolled_){if(!sourceSecondLightCounted_||!authority.canRetire(e))return false;}
    else for(std::size_t i=0;i<authority.count_;++i)if(authority.entries_[i].node==&e)return false;
    const auto xyz=[](const std::array<std::uint32_t,4>& v){return std::array<float,3>{{
        g::directional_native_proposal::detail::fromWord(v[0]),g::directional_native_proposal::detail::fromWord(v[1]),
        g::directional_native_proposal::detail::fromWord(v[2])}};};
    const auto& descriptor=nativeLightList3A8_->wrappers38[1].sourceDescriptor04;
    if(sourceSecondDescriptorFreeMask_&8u){if(sourceSecondDescriptorFreeMask_!=15||descriptor)return false;}
    if(sourceSecondPositionWords_&&!(sourceSecondDescriptorFreeMask_&8u)){
        if(!descriptor)return false;
        if(sourceSecondDescriptorFreeMask_&1u){if(descriptor->position14)return false;}
        else if(!descriptor->position14||*descriptor->position14!=*sourceSecondPositionWords_)return false;
    }
    if(sourceSecondLookInputWords_&&!(sourceSecondDescriptorFreeMask_&8u)){
        if(!descriptor)return false;
        if(sourceSecondDescriptorFreeMask_&2u){if(descriptor->look18)return false;}
        else if(!descriptor->look18||*descriptor->look18!=*sourceSecondLookInputWords_)return false;
    }
    if(sourceSecondAttenuationWords_&&!(sourceSecondDescriptorFreeMask_&8u)){
        if(!descriptor)return false;
        if(sourceSecondDescriptorFreeMask_&4u){if(descriptor->attenuation1C)return false;}
        else if(!descriptor->attenuation1C||*descriptor->attenuation1C!=*sourceSecondAttenuationWords_)return false;
    }
    if(sourceSecondDescriptorFreeMask_&&(!sourceSecondConfigReturned_||!sourceSecondPositionWords_||
       !sourceSecondLookInputWords_||!sourceSecondAttenuationWords_))return false;
    if(l.positionWritten!=bool(sourceSecondPositionWords_)||l.lookWritten!=bool(sourceSecondNormalizedWords_)||
       bool(sourceSecondNormalizedWords_)!=bool(sourceSecondLookInputWords_))return false;
    if(l.rangeWritten&&!sourceSecondAttenuationRange_)return false;
    if(sourceSecondLightDefaults_){
        if(!sourceSecondLightCounted_||!sourceSecondLightEnrolled_||!sourceSecondLightVtable08_||*sourceSecondLightVtable08_!=0x3cd600||
           l.query30||l.flags33||l.knownFlags33!=7||l.type34!=(l.colorsWritten?2u:0u)||l.mode38!=3||
           l.diffuse3C!=(l.colorsWritten?std::array<float,4>{{103,103,92.5f,127.5f}}:std::array<float,4>{{128,128,128,0}})||
           l.specular4C!=l.diffuse3C||l.ambient5C!=(l.colorsWritten?std::array<float,4>{}:l.diffuse3C)||
           l.position6C!=(l.positionWritten?xyz(*sourceSecondPositionWords_):std::array<float,3>{})||
           l.look78!=(l.lookWritten?xyz(*sourceSecondNormalizedWords_):std::array<float,3>{{1,0,0}})||
           l.range84!=(l.rangeWritten?*sourceSecondAttenuationRange_:g::SourceUiLightMath50KG::range146898())||l.field88!=0x3f800000||
           l.attenuation8C!=(sourceSecondAttenuationWords_?xyz(*sourceSecondAttenuationWords_):std::array<float,3>{{1,0,0}})||l.cone98!=0x3f490fdb||l.cone9C!=0x3fc90fdb||
           l.positionA0!=(l.positionWritten?*sourceSecondPositionWords_:worldApplication_->lightVector426F10_)||
           l.lookB0!=(l.lookWritten?*sourceSecondNormalizedWords_:std::array<std::uint32_t,4>{{0,0,0,0x3f800000}})||
           l.fieldC0||l.fieldC4||l.ordinalC8!=(l.colorsWritten?1:-1)||
           l.luminanceCC!=(sourceSecondLuminance_?*sourceSecondLuminance_:0xbf800000u)||l.observerD0||l.fieldD4||l.fieldD8)return false;
    }else if(sourceSecondLightVtable08_||sourceSecondLightReturned_)return false;
    if((l.colorsWritten&&!sourceSecondLightPublished_)||(l.revisionWritten&&!l.colorsWritten)||
       (sourceSecondLuminance_&&!l.revisionWritten)||(sourceSecondPositionCompared_&&!sourceSecondLuminance_)||
       (l.positionWritten&&!sourceSecondPositionCompared_)||(l.positionRevision&&!l.positionWritten)||
       (sourceSecondLookCompared_&&(!sourceSecondPositionCompared_||(l.positionWritten&&!l.positionRevision)))||
       (l.lookWritten&&!sourceSecondLookCompared_)||(l.lookRevision&&!l.lookWritten)||
       (sourceSecondAttenuationWords_&&(!sourceSecondLookCompared_||(l.lookWritten&&!l.lookRevision)))||
       (sourceSecondAttenuationRange_&&!sourceSecondAttenuationWords_)||(l.rangeRevision&&!l.rangeWritten)||
       l.configured!=sourceSecondConfigReturned_||(l.configured&&(!sourceSecondAttenuationRange_||(l.rangeWritten&&!l.rangeRevision)))||
       (l.registered&&!l.configured)||(l.attached&&(!l.registered||sourceSecondLightNativeDetached_||sourceSecondDescriptorFreeMask_!=15))||
       l.retiring||l.observerOwner)return false;
    if(l.attached&&(sourceFirstLight_->entity.next14!=&e||nativeLightList3A8_->entity.firstChild18!=&sourceFirstLight_->entity||
       nativeLightList3A8_->entity.descendants24!=2u))return false;
    if(sourceSecondLightReturned_!=l.typed||l.constructed!=sourceSecondLightReturned_||
       (!l.typed&&l.query32!=0xff)||(l.typed&&l.query32==0xff))return false;
    if(sourceSecondLightPublished_&&(!sourceSecondLightReturned_||nativeLightList3A8_->wrappers38[1].live00!=&e))return false;
    return true;
}
bool Owner::constructSourceSecondLight146720(std::uint32_t& pc)noexcept {
    //Native admission and ledger reserve precede nullable guest allocation.
    //An admission failure is NOT a source allocationNULL or constructor return.
    if(!preflightSourceFirstLightNative()||!sourceFirstLight_||!sourceFirstLight_->attached||!sourceFirstLight_->registered||
       !preflightSourceSecondLightNative()||sourceSecondLightOuterReached_||sourceSecondDescriptorFreeMask_||!worldApplication_||
       !nativeLightList3A8_||!nativeLightList3A8_->sourceMode||!nativeLightList3A8_->wrappers38||
       nativeLightList3A8_->wrappers38[1].live00||!reserveSourceAmbientListNative())return false;
    auto& authority=worldApplication_->entityAuthority_;
    pc=0x277e0c;sourceSecondLightOuterReached_=true;
    sourceSecondLight_.reset(new(std::nothrow) NativeRaLight146720);
    if(sourceSecondLight_){auto& l=*sourceSecondLight_;auto& e=l.entity;
        pc=0x14672c;if(!authority.canEnroll(e,key(&e)))return false;
        const auto made=g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::light146720,
            worldEntity_.sessionGeneration,key(&e),true,worldApplication_->objectCount423F60());
        sourceSecondLightCounted_=e.reference00!=0;
        if(made.status!=g::OriginalWreckComponentStatus50LA::complete){pc=made.sourcePC;return false;}
        if(!authority.enroll(e))return false;sourceSecondLightEnrolled_=true;
        //146744..146934 exact constructor stores. Typed storage carries these
        //known defaults, but they become source knowledge only after this block.
        //Flags33 upper five bits remain unknown (knownFlags33==7), never zero-claimed.
        pc=0x1467a8;sourceSecondLightVtable08_=0x3cd600;
        pc=0x1468c0;l.range84=g::SourceUiLightMath50KG::range146898();
        pc=0x146924;l.positionA0=worldApplication_->lightVector426F10_; //Actual3C8A0C producer.
        pc=0x146934;sourceSecondLightDefaults_=true;
        pc=0x146938;const auto query=worldApplication_->types383408().lightEmitter146938();
        if(!query.okay()){pc=query.sourcePC;return false;}
        pc=0x146964;l.query32=query.type;l.typed=true;
        pc=0x146970;l.constructed=true;sourceSecondLightReturned_=true;
    }
    pc=0x277e28;nativeLightList3A8_->wrappers38[1].live00=sourceSecondLight_?&sourceSecondLight_->entity:nullptr;
    sourceSecondLightPublished_=true;return true;
}
bool Owner::sourceSecondLightLuminance146E60(std::uint32_t& out)const noexcept {
    if(!preflightSourceSecondLightNative()||!sourceSecondLight_||!sourceSecondLight_->colorsWritten||
       !sourceSecondLight_->revisionWritten||sourceSecondLuminance_)return false;
    //Same narrow pod Owner friendship; exact source software-double helper,
    //not the native normalization adapter and not a guessed output constant.
    return g::SourceUiLightMath50KG::luminance146E60(0x42ce0000u,0x42ce0000u,0x42b90000u,out);
}
bool Owner::sourceSecondAttenuation147268(std::uint32_t& out)const noexcept {
    if(!preflightSourceSecondLightNative()||!sourceSecondLight_||!sourceSecondAttenuationWords_||sourceSecondAttenuationRange_)
        return false;
    //Same reached source sqrt/narrow limit as1472E8/1472F0, via existing
    //private pod Owner math authority. Actual EE-FPU expression is explicit
    //strict-native adaptation; no original expected result word is asserted.
    return g::directional_native_proposal::range147268(*sourceSecondAttenuationWords_,
        g::SourceUiLightMath50KG::range146898(),out)==g::directional_native_proposal::Status::complete;
}
bool Owner::attachSourceSecondLight141960(std::uint32_t& pc)noexcept {
    pc=0x273b98;
    if(!preflightSourceFirstLightNative()||!preflightSourceSecondLightNative()||!sourceSecondLight_||
       !sourceSecondLightPublished_||!sourceSecondLight_->registered||sourceSecondLight_->attached||
       sourceSecondLightNativeDetached_||sourceSecondDescriptorFreeMask_!=15||
       !nativeLightList3A8_||!nativeLightList3A8_->sourceAuthorityEnrolled||nativeLightList3A8_->wrappers38[1].sourceDescriptor04)
        return false;
    const auto attached=worldApplication_->entityAuthority_.attachChildren(nativeLightList3A8_->entity,sourceSecondLight_->entity,2u,
        worldApplication_->attachmentLock423F58());
    if(attached.status!=g::OriginalWreckComponentStatus50LA::complete){pc=attached.sourcePC;return false;}
    sourceSecondLight_->attached=true;pc=0x273ba0;return true;
}
bool Owner::detachSourceSecondLightNative(std::uint32_t& pc)noexcept {
    if(!preflightSourceFirstLightNative()||!preflightSourceSecondLightNative())return false;
    if(!sourceSecondLight_||!sourceSecondLight_->attached)return true;
    const auto detached=worldApplication_->entityAuthority_.detach(sourceSecondLight_->entity,worldApplication_->attachmentLock423F58());
    if(detached.status!=g::OriginalWreckComponentStatus50LA::complete){pc=detached.sourcePC;return false;}
    sourceSecondLight_->attached=false;sourceSecondLightNativeDetached_=true;
    return true; //Native splice only, not a guest destructor or mask reset.
}
bool Owner::abortSourceSecondLightNative()noexcept {
    if(!preflightSourceSecondLightNative()||(sourceSecondLight_&&(sourceSecondLight_->registered||sourceSecondLight_->attached)))return false;
    if(sourceSecondLight_){auto& e=sourceSecondLight_->entity;
        if(sourceSecondLightEnrolled_&&!worldApplication_->entityAuthority_.retire(e))return false;
        sourceSecondLightEnrolled_=false;
        if(sourceSecondLightCounted_){--e.reference00;--worldApplication_->objectCount423F60();sourceSecondLightCounted_=false;}
    }
    if(sourceSecondLightPublished_)nativeLightList3A8_->wrappers38[1].live00=nullptr;
    sourceSecondLight_.reset();sourceSecondLightOuterReached_=false;sourceSecondLightDefaults_=false;
    sourceSecondLightReturned_=false;sourceSecondLightPublished_=false;sourceSecondLightVtable08_.reset();
    sourceSecondLuminance_.reset();sourceSecondPositionWords_.reset();sourceSecondLookInputWords_.reset();sourceSecondNormalizedWords_.reset();
    sourceSecondPositionCompared_=sourceSecondLookCompared_=false;
    sourceSecondAttenuationWords_.reset();sourceSecondAttenuationRange_.reset();sourceSecondConfigReturned_=false;
    sourceSecondLightNativeDetached_=false; //Keep descriptor free history; no fabricated restoration.
    return true; //Native withdrawal only; no146980/manager-removal guest call invented.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceSecondLightFrameNative()const noexcept {
    if(!preflightSourceSecondLightNative())return false;
    if(sourceSecondLightPhase_==SourceSecondLightPhase::unreached)
        return !sourceSecondLightWrapper_&&!sourceSecondLightDescriptor_&&!sourceSecondLightOuterReached_&&!sourceSecondGuardMask_&&
            !sourceSecondLensStack00_&&!sourceSecondOrdinalStack08_&&!sourceSecondArgumentsBorrowed_&&
            !sourceSecondDescriptorDisposed_&&!sourceSecondWrapperReturned_&&!sourceSecondWrapperV0_;
    if(sourceSecondLightPhase_==SourceSecondLightPhase::nativeClosed)
        return !sourceSecondLightWrapper_&&!sourceSecondLightDescriptor_&&!sourceSecondLight_&&!sourceSecondLightPublished_&&
            !sourceSecondLensStack00_&&!sourceSecondOrdinalStack08_&&!sourceSecondArgumentsBorrowed_;
    if(!sourceFirstWrapperReturned_||!sourceFirstDescriptorDisposed_||!sourceLightPopulationS0_||
       (sourceLightPopulationItemsEnd_.value_or(*sourceLightPopulationS0_)<1||sourceLightPopulationItemsEnd_.value_or(*sourceLightPopulationS0_)>185)||
       !sourceFirstLight_||!sourceFirstLight_->attached||!sourceFirstLight_->registered||!nativeLightList3A8_||
       !sourceLight3A8_||*sourceLight3A8_!=nativeLightList3A8_.get()||!nativeLightList3A8_->wrappers38||
       sourceSecondGuard_>1||sourceSecondEnabled_>1||(sourceSecondGuardMask_&~3u))return false;
    if(sourceSecondLightWrapper_&&sourceSecondLightWrapper_!=&nativeLightList3A8_->wrappers38[1])return false;
    if(sourceSecondLightDescriptor_&&(!sourceSecondLightWrapper_||sourceSecondLightDescriptor_!=sourceSecondLightWrapper_->sourceDescriptor04.get()))return false;
    if(sourceSecondLightZone_){
        if(sourceSecondDescriptorDisposed_){if(*sourceSecondLightZone_!=0)return false;}
        else if(!sourceSecondLightDescriptor_||*sourceSecondLightZone_!=sourceSecondLightDescriptor_->zone20)return false;
    }
    if(sourceSecondDescriptorDisposed_){
        if(sourceSecondDescriptorFreeMask_!=15||sourceSecondLightDescriptor_||!sourceSecondLightWrapper_||
           sourceSecondLightWrapper_->sourceDescriptor04||sourceSecondLensStack00_||sourceSecondOrdinalStack08_||
           sourceSecondArgumentsBorrowed_||!sourceSecondWrapperV0_||*sourceSecondWrapperV0_!=1)return false;
    }else if(sourceSecondWrapperReturned_||sourceSecondWrapperV0_)return false;
    if(sourceSecondWrapperReturned_&&!sourceSecondDescriptorDisposed_)return false;
    if(sourceLightPopulationItemsEnd_.value_or(*sourceLightPopulationS0_)>=2&&(!sourceSecondWrapperReturned_||!sourceSecondLight_||
       (!sourceSecondLight_->attached&&!sourceSecondLightNativeDetached_)))return false;
    if(sourceSecondOrdinalStack08_&&(!sourceSecondLightPublished_||!sourceSecondLightDescriptor_||
       *sourceSecondOrdinalStack08_!=1||sourceSecondLensStack00_!=((sourceSecondLightDescriptor_->flags28&8u)?
           sourceSecondLightDescriptor_->lensTexture50.data():nullptr)))return false;
    if(sourceSecondArgumentsBorrowed_&&(!sourceSecondOrdinalStack08_||!sourceSecondLightPublished_))return false;
    if(sourceSecondLight_&&sourceSecondLight_->colorsWritten&&!sourceSecondArgumentsBorrowed_&&!sourceSecondDescriptorDisposed_)return false;
    if((sourceSecondGuardMask_&2u)&&(!(sourceSecondGuardMask_&1u)||sourceSecondGuard_!=1))return false;
    if(sourceSecondAssertionPassed_&&(!(sourceSecondGuardMask_&2u)||!sourceSecondEnabled_||!sourceSecondLightZone_))return false;
    return sourceSecondLightPhase_!=SourceSecondLightPhase::published||
        (sourceSecondLightPublished_&&sourceSecondLightWrapper_&&sourceSecondLightDescriptor_&&sourceSecondLightPC_==0x277e2c);
}
bool Owner::abortSourceSecondLightFrameNative()noexcept {
    if(!preflightSourceSecondLightFrameNative())return false;
    if(sourceSecondLightPhase_==SourceSecondLightPhase::unreached||sourceSecondLightPhase_==SourceSecondLightPhase::nativeClosed)return true;
    std::uint32_t nativePC=0;
    if(!detachSourceSecondLightNative(nativePC))return false;
    if(sourceTopologyLightManager_){rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic d;
        if(!sourceTopologyLightManager_->withdrawSourceSecondNative(*this,d))return false;}
    if(!abortSourceSecondLightNative())return false;
    sourceSecondLensStack00_=nullptr;sourceSecondOrdinalStack08_.reset();sourceSecondArgumentsBorrowed_=false;
    sourceSecondLightWrapper_=nullptr;sourceSecondLightDescriptor_=nullptr;sourceSecondLightPhase_=SourceSecondLightPhase::nativeClosed;
    return true;
}
bool Owner::continueSourceSecondLight273BA8(std::uint32_t& pc)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceSecondLightPhase_!=SourceSecondLightPhase::unreached||sourceLightPopulationPC_!=0x273ba8||
       !sourceFirstWrapperReturned_||!sourceFirstDescriptorDisposed_||!sourceLightPopulationS0_||*sourceLightPopulationS0_!=1||
       !sourceFirstLight_||!sourceFirstLight_->attached||!sourceFirstLight_->registered||
       !sourceDefaultObserverLease_||!sourceTopologyRoutineReturned_||!preflightSourceWorldTopologyCountNative())return false;
    const auto at=[&](std::uint32_t value){pc=sourceSecondLightPC_=value;};
    const auto stop=[&](std::uint32_t value){at(value);sourceSecondLightPhase_=SourceSecondLightPhase::bounded;return false;};
    sourceSecondLightPhase_=SourceSecondLightPhase::entered;
    at(0x273ba8);auto* list=*sourceLight3A8_;if(list!=nativeLightList3A8_.get()||!list)return stop(pc);
    at(0x273bac);const auto count=static_cast<std::int32_t>(list->count30);
    at(0x273bb0);if(*sourceLightPopulationS0_>=count)return stop(0x273bbc);
    using namespace g::world_light_assertion273ABC;
    at(0x273abc);if(!read(1,sourceSecondGuard_))return stop(pc);sourceSecondGuardMask_|=1u;
    //Same process first-row guard store proved1. Unknown intervening native
    //history is not reset; a mismatching guard cannot mint fresh invocation.
    if(sourceSecondGuard_!=1)return stop(pc);
    at(0x273adc);if(!read(0,sourceSecondEnabled_))return stop(pc);sourceSecondGuardMask_|=2u;
    if(sourceSecondEnabled_!=1)return stop(pc); //First-row original store/read history proved enabled1.
    at(0x273b00);if(!list->wrappers38||list->sourceWrappersConstructed<2)return stop(pc);
    auto& actual=list->wrappers38[1];sourceSecondLightWrapper_=&actual;
    at(0x273b10);if(!actual.sourceConstructed||!actual.sourceDescriptorPublished||!actual.sourceDescriptor04)return stop(pc);
    sourceSecondLightDescriptor_=actual.sourceDescriptor04.get();auto& descriptor=*sourceSecondLightDescriptor_;
    if(sourceSecondEnabled_){
        at(0x273b14);sourceSecondLightZone_=descriptor.zone20;
        at(0x273b1c);if(!worldZones34C_||static_cast<std::int32_t>(*worldZones34C_)<*sourceSecondLightZone_)return stop(pc);
        sourceSecondAssertionPassed_=true;
    }
    at(0x273b70); //Actual same wrapper vtable3D2B30+0C dispatch277D80, ordinal1 argument.
    at(0x277da0);if(!descriptor.loaded||descriptor.type00!=2||descriptor.zone20||descriptor.knownFlags28!=0x7f||
       (descriptor.flags28&0x7f)!=0x0f||!descriptor.position14||!descriptor.look18||!descriptor.attenuation1C)return stop(pc);
    at(0x277dd4); //Emitslight bit2, not the bit4 attenuation rewrite path.
    if(!constructSourceSecondLight146720(pc))return stop(pc);
    at(0x277e2c);sourceSecondLightPhase_=SourceSecondLightPhase::published;
    //Preserve the actual bit3 argument even though146F60 does not consume it.
    //This is a borrow of the SAME live descriptor, not a copied UI texture.
    at(0x277e50);sourceSecondLensStack00_=(descriptor.flags28&8u)?descriptor.lensTexture50.data():nullptr;
    at(0x277e54);sourceSecondOrdinalStack08_=*sourceLightPopulationS0_;
    at(0x277e84);sourceSecondArgumentsBorrowed_=true;
    if(!sourceSecondLight_)return stop(0x146fc0); //Original NULL receiver fault, not a fake return.
    if(sourceSecondLightDescriptor_!=actual.sourceDescriptor04.get()||actual.live00!=&sourceSecondLight_->entity||
       *sourceSecondOrdinalStack08_!=1||!sourceTopologyLightManager_||
       sourceLoadApplication_->sourceLightManager426F60Owner_!=sourceTopologyLightManager_||
       descriptor.color04!=std::array<std::uint32_t,4>{{0x434e0000u,0x434e0000u,0x43390000u,0x437f0000u}}||
       descriptor.range24||!descriptor.position14||!descriptor.look18||!descriptor.attenuation1C)
        return stop(0x146f80); //Only actual zero-range second Directional cohort.
    auto& light=*sourceSecondLight_;
    const std::array<std::uint32_t,4>* admittedVectors[]={descriptor.position14.get(),descriptor.look18.get()};
    for(const auto* v:admittedVectors)
        for(auto word:*v)if(!g::directional_native_proposal::detail::normalOrZeroWord(word))return stop(0x146f80);
    at(0x146fb8); //141C20 identity return only (JR with original a0->v0 delay slot).
    at(0x146fc0);light.type34=descriptor.type00;at(0x146fd8);light.ordinalC8=*sourceSecondOrdinalStack08_;
    //Original four /2 operations on admitted206,206,185,255 are exact binary32.
    //146C40 type2 copies half RGBA to diffuse/specular and clears ambient.
    at(0x146d04);light.diffuse3C=light.specular4C={{103,103,92.5f,127.5f}};light.ambient5C={};light.colorsWritten=true;
    //Fresh C0/C4 are NULL, so both dependent setter branches skip.
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic md;
    at(0x146e30);if(!sourceTopologyLightManager_->incrementSourceSecond149300(*this,0,md))return stop(md.sourcePC);
    light.revisionWritten=true;
    at(0x146e3c);std::uint32_t luminance=0;
    if(!sourceSecondLightLuminance146E60(luminance))return stop(0x146e60);
    at(0x146e44);sourceSecondLuminance_=luminance;light.luminanceCC=luminance;
    at(0x147028); //Actual range0 equality skips range writes and their revision.
    const auto sameXYZ=[](const std::array<std::uint32_t,4>& a,const std::array<std::uint32_t,4>& b){
        //Admitted normal-or-zero operands; signed zeros compare equal just as
        //source c.eq.s. W deliberately does not participate in source equality.
        for(unsigned i=0;i<3;++i)if(g::directional_native_proposal::detail::fromWord(a[i])!=
            g::directional_native_proposal::detail::fromWord(b[i]))return false;
        return true;
    };
    at(0x1470bc);sourceSecondPositionCompared_=true;
    if(!sameXYZ(*descriptor.position14,light.positionA0)){
        at(0x147130);light.positionA0=*descriptor.position14;sourceSecondPositionWords_=*descriptor.position14;
        at(0x147600); //14760C/147610 observes constructor D0==NULL; returns147640, no callback.
        //D0,C0,C4 were checked by source constructor/preflight; no unrelated
        //observer traversal or owner acquisition is invented by this prefix.
        at(0x147138);light.position6C[0]=g::directional_native_proposal::detail::fromWord(light.positionA0[0]);
        at(0x147140);light.position6C[1]=g::directional_native_proposal::detail::fromWord(light.positionA0[1]);
        at(0x147148);light.position6C[2]=g::directional_native_proposal::detail::fromWord(light.positionA0[2]);light.positionWritten=true;
        at(0x147184);if(!sourceTopologyLightManager_->incrementSourceSecond149300(*this,1,md))return stop(md.sourcePC);
        light.positionRevision=true;
    }
    at(0x147194);sourceSecondLookCompared_=true;
    if(!sameXYZ(*descriptor.look18,light.lookB0)){
        //Explicit native numeric adaptation, NOT original PS2 output parity.
        //Use raw RA target-minus-position from277624, never UI pre-normalization.
        g::directional_native_proposal::Words normalized{};
        at(0x147200);
        if(g::directional_native_proposal::normalize106EA0(*descriptor.look18,normalized)!=
            g::directional_native_proposal::Status::complete)return stop(pc);
        sourceSecondLookInputWords_=*descriptor.look18;sourceSecondNormalizedWords_=normalized;light.lookB0=normalized;
        at(0x147210);light.look78[0]=g::directional_native_proposal::detail::fromWord(light.lookB0[0]);
        at(0x147218);light.look78[1]=g::directional_native_proposal::detail::fromWord(light.lookB0[1]);
        at(0x147220);light.look78[2]=g::directional_native_proposal::detail::fromWord(light.lookB0[2]);light.lookWritten=true;
        at(0x147230);if(!sourceTopologyLightManager_->incrementSourceSecond149300(*this,2,md))return stop(md.sourcePC);
        light.lookRevision=true;
    }
    at(0x147238);
    if(!descriptor.attenuation1C||*descriptor.attenuation1C!=std::array<std::uint32_t,4>{{0x3cf5c28fu,0x3b03126fu,0x3a83126fu,0x3f800000u}})
        return stop(pc); //Only actual second Directional default coefficients.
    at(0x147248);light.attenuation8C[0]=g::directional_native_proposal::detail::fromWord((*descriptor.attenuation1C)[0]);
    at(0x147250);light.attenuation8C[1]=g::directional_native_proposal::detail::fromWord((*descriptor.attenuation1C)[1]);
    at(0x147264);light.attenuation8C[2]=g::directional_native_proposal::detail::fromWord((*descriptor.attenuation1C)[2]);
    sourceSecondAttenuationWords_=*descriptor.attenuation1C;
    at(0x147268);std::uint32_t range=0;
    if(!sourceSecondAttenuation147268(range))return stop(pc); //Native refusal retains prior source stores/revisions.
    at(0x14732c);sourceSecondAttenuationRange_=range;
    if(g::directional_native_proposal::detail::fromWord(range)!=g::directional_native_proposal::detail::fromWord(light.range84)){
        at(0x147340);light.range84=range;light.rangeWritten=true;
        at(0x147354);if(!sourceTopologyLightManager_->incrementSourceSecond149300(*this,3,md))return stop(md.sourcePC);
        light.rangeRevision=true;
    }
    at(0x14746c);light.mode38=3; //Actual type2 branch, not t2/t3 or a generic mode policy.
    at(0x147494);light.configured=true;sourceSecondConfigReturned_=true; //146F60 returns the same owner.
    at(0x277f98); //NonNULL return; type2 skips Spotlight-only146F30 cone setter.
    if(sourceSecondLightDescriptor_!=actual.sourceDescriptor04.get()||actual.live00!=&light.entity)return stop(pc);
    at(0x277fcc);if(!sourceTopologyLightManager_->appendSourceSecond148F30(*this,md))return stop(md.sourcePC);
    at(0x277fd4);
    if(sourceSecondLightDescriptor_!=actual.sourceDescriptor04.get()||!actual.sourceDescriptor04||
       descriptor.knownFlags28!=0x7f||(descriptor.flags28&0x7f)!=0x0f||!descriptor.position14||!descriptor.look18||
       !descriptor.attenuation1C||!preflightSourceSecondLightNative())return stop(pc);
    at(0x277fe4); //Actual bit4 false: no flare owner allocation/attachment.
    at(0x2780bc); //Actual bit5 false: no flasher146B10 call.
    //All native admissions above are read-only. Below, typed array/POD frees
    //have no callback/failure boundary; exact source order/NULL stores retained.
    at(0x2780ec);descriptor.vtableF4=0x3d2b40;
    at(0x2780fc);descriptor.position14.reset();
    at(0x278104);sourceSecondDescriptorFreeMask_|=1u;
    at(0x278114);descriptor.look18.reset();
    at(0x27811c);sourceSecondDescriptorFreeMask_|=2u;
    at(0x27812c);descriptor.attenuation1C.reset();
    at(0x278134);sourceSecondDescriptorFreeMask_|=4u;
    //Withdraw native LIVE borrowers before outerfree. Source stack residue is
    //not dereferenced after its last use; scalar ordinal already copied to C8.
    sourceSecondLensStack00_=nullptr;sourceSecondOrdinalStack08_.reset();sourceSecondArgumentsBorrowed_=false;
    at(0x27813c);sourceSecondLightDescriptor_=nullptr;actual.sourceDescriptor04.reset();sourceSecondDescriptorFreeMask_|=8u;
    at(0x278144);sourceSecondWrapperV0_=1;
    at(0x278148);sourceSecondDescriptorDisposed_=true; //Original wrapper04=NULL publication.
    at(0x27815c);sourceSecondWrapperReturned_=true;
    at(0x273b7c);if(*sourceLight3A8_!=nativeLightList3A8_.get())return stop(pc);
    at(0x273b94);if(actual.live00!=&light.entity)return stop(pc);
    if(!attachSourceSecondLight141960(pc))return stop(pc);
    at(0x273ba0);++*sourceLightPopulationS0_;
    at(0x273ba8);sourceSecondLightPhase_=SourceSecondLightPhase::bounded;
    return continueSourceTypedLight273BA8(2,pc); //Same actual next ordinal; no loop/readiness shortcut.
}
}
