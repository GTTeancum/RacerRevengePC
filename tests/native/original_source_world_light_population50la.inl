#pragma once
#include "../../src/game/original_world_light_assertion273abc.h"
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceLightPopulationNative()const noexcept {
    if(!preflightSourceFirstLightNative()||!preflightSourceSecondLightFrameNative())return false;
    if(sourceLightPopulationPhase_==SourceLightPopulationPhase::unreached)
        return !sourceTopologyRoutineReturned_&&!sourceLightPopulationS0_&&!sourceLightPopulationWrapper_&&!sourceLightAssertionMask_;
    if(sourceLightPopulationPhase_==SourceLightPopulationPhase::nativeClosed)return !sourceLightPopulationWrapper_&&!sourceLightPopulationDescriptor_;
    if(!sourceTopologyRoutineReturned_||!sourceTopologyReturnV0_||*sourceTopologyReturnV0_!=1||
       !sourceDefaultObserverLease_||sourceDefaultObserverPhase_!=SourceDefaultObserverPhase::returned||
       !worldApplication_||!sourceLight3A8_||!nativeLightList3A8_||*sourceLight3A8_!=nativeLightList3A8_.get()||
       !nativeLightList3A8_->sourceMode||sourceLightsPhase_!=SourceLightsPhase::returnedOne||
       !preflightSourceLightListNative()||sourceLightAssertionGuard_>1||sourceLightAssertionEnabled_>1||
       (sourceLightAssertionMask_&~15u))return false;
    if(sourceLightPopulationS0_&&*sourceLightPopulationS0_!=0){
        if(*sourceLightPopulationS0_!=1||!sourceFirstWrapperReturned_||!sourceFirstLight_||
           (!sourceFirstLight_->attached&&!sourceFirstLightNativeDetached_))return false;
    }
    if(sourceFirstDescriptorDisposed_){
        if(sourceLightPopulationDescriptor_||!sourceLightPopulationWrapper_||sourceLightPopulationWrapper_->sourceDescriptor04||
           !sourceFirstWrapperV0_||*sourceFirstWrapperV0_!=1)return false;
    }else if(sourceFirstWrapperV0_||sourceFirstWrapperReturned_)return false;
    if(sourceFirstWrapperReturned_&&(!sourceFirstDescriptorDisposed_||!sourceFirstLightPublished_||!sourceFirstLight_))return false;
    if(sourceLightPopulationCount_&&*sourceLightPopulationCount_!=static_cast<std::int32_t>(nativeLightList3A8_->count30))return false;
    if(sourceLightPopulationWrapper_){
        if(!nativeLightList3A8_->wrappers38||sourceLightPopulationWrapper_!=&nativeLightList3A8_->wrappers38[0]||
           !sourceLightPopulationWrapper_->sourceConstructed)return false;
        if(sourceLightPopulationDescriptor_&&sourceLightPopulationDescriptor_!=sourceLightPopulationWrapper_->sourceDescriptor04.get())return false;
    }
    if(sourceLightPopulationZone_){
        if(sourceFirstDescriptorDisposed_){if(*sourceLightPopulationZone_!=0)return false;} //Retained actual first-zone receipt.
        else if(!sourceLightPopulationDescriptor_||*sourceLightPopulationZone_!=sourceLightPopulationDescriptor_->zone20)return false;
    }
    if((sourceLightAssertionMask_&2u)&&(!(sourceLightAssertionMask_&1u)||sourceLightAssertionGuard_))return false;
    if((sourceLightAssertionMask_&4u)&&!(sourceLightAssertionMask_&2u))return false;
    if((sourceLightAssertionMask_&8u)&&(!(sourceLightAssertionMask_&1u)||
        (!sourceLightAssertionGuard_&&!(sourceLightAssertionMask_&4u))))return false;
    if(sourceLightAssertionPassed_&&(!(sourceLightAssertionMask_&8u)||!sourceLightAssertionEnabled_||!sourceLightPopulationZone_))return false;
    return sourceLightPopulationPhase_!=SourceLightPopulationPhase::wrapperPending||
        (sourceLightPopulationWrapper_&&sourceLightPopulationDescriptor_&&sourceLightPopulationPC_==0x273b74&&
         (!sourceLightAssertionEnabled_||sourceLightAssertionPassed_));
}
bool Owner::abortSourceLightPopulationNative(std::uint32_t& pc)noexcept {
    if(!preflightSourceLightPopulationNative())return false;
    if(sourceLightPopulationPhase_==SourceLightPopulationPhase::unreached||sourceLightPopulationPhase_==SourceLightPopulationPhase::nativeClosed)return true;
    if(!abortSourceSecondLightFrameNative())return false;
    if(!detachSourceFirstLightNative(pc))return false;
    if(sourceTopologyLightManager_){rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic d;
        if(!sourceTopologyLightManager_->withdrawSourceAmbientNative(*this,d))return false;}
    if(!abortSourceFirstLightNative())return false;
    sourceLightPopulationWrapper_=nullptr;sourceLightPopulationDescriptor_=nullptr;
    sourceLightPopulationPhase_=SourceLightPopulationPhase::nativeClosed;
    return true; //Native partial light withdrawal; no process assertion history reset.
}
bool Owner::continueSourceLightPopulation1AA144(std::uint32_t& pc)noexcept {
    pc=sourceLightPopulationPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceLightPopulationPhase_!=SourceLightPopulationPhase::unreached||sourceTopologyRoutineReturned_||
       sourceTopologyPhase_!=SourceTopologyPhase::observerReturned||sourceTopologyPC_!=0x272f94||sourceTrackPC_!=0x272f94||
       sourceDefaultObserverPhase_!=SourceDefaultObserverPhase::returned||!sourceDefaultObserverLease_||
       !sourceTopologyReturnV0_||*sourceTopologyReturnV0_!=1||!sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||sourceLoadApplication_->entities()!=worldApplication_||
       !sourceLight3A8_||*sourceLight3A8_!=nativeLightList3A8_.get()||!nativeLightList3A8_||
       !nativeLightList3A8_->sourceMode||sourceLightsPhase_!=SourceLightsPhase::returnedOne||
       !preflightSourceWorldTopologyCountNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourceLightPopulationPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourceLightPopulationPhase_=SourceLightPopulationPhase::bounded;return false;};
    at(0x272fac);sourceTopologyRoutineReturned_=true; //Only authentic vFC return1, not World loaded.
    sourceLightPopulationPhase_=SourceLightPopulationPhase::entered;
    at(0x1aa144);if(sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_)return stop(pc);
    at(0x1aa14c); //Most-derived3CF110+10C is273AA0, verified in retained original table.
    at(0x1aa150); //Same World74 actual virtual call; no legacy wrapper adoption.
    at(0x273ab8);sourceLightPopulationS0_=0;
    at(0x273ba8);auto* list=*sourceLight3A8_;if(!list)return stop(pc);
    at(0x273bac);sourceLightPopulationCount_=static_cast<std::int32_t>(list->count30);
    at(0x273bb0);if(*sourceLightPopulationS0_>=*sourceLightPopulationCount_)return stop(0x273bbc);
    using namespace g::world_light_assertion273ABC;
    at(0x273abc);if(!read(1,sourceLightAssertionGuard_))return stop(pc);sourceLightAssertionMask_|=1u;
    if(!sourceLightAssertionGuard_){
        at(0x273ad0);if(!storeOne(0))return stop(pc);sourceLightAssertionMask_|=2u;
        at(0x273ad4);if(!storeOne(1))return stop(pc);sourceLightAssertionMask_|=4u;
    }
    at(0x273adc);if(!read(0,sourceLightAssertionEnabled_))return stop(pc);sourceLightAssertionMask_|=8u;
    if(sourceLightAssertionEnabled_){
        at(0x273ae8);list=*sourceLight3A8_;if(!list)return stop(pc);
        at(0x273af4);if(!worldZones34C_)return stop(pc);const auto zones=static_cast<std::int32_t>(*worldZones34C_);
        at(0x273b00);if(!list->wrappers38||list->sourceWrappersConstructed<1)return stop(pc);
        auto& wrapper=list->wrappers38[0];
        at(0x273b10);if(!wrapper.sourceDescriptorPublished)return stop(pc);
        sourceLightPopulationWrapper_=&wrapper;sourceLightPopulationDescriptor_=wrapper.sourceDescriptor04.get();
        at(0x273b14);if(!sourceLightPopulationDescriptor_||!sourceLightPopulationDescriptor_->loaded)return stop(pc);
        sourceLightPopulationZone_=sourceLightPopulationDescriptor_->zone20;
        at(0x273b1c);
        if(zones<*sourceLightPopulationZone_)return stop(pc); //133660 failure/log/trap branch not replaced by a pass.
        sourceLightAssertionPassed_=true; //133660(1) returns0; signed source comparison has no lower-bound test.
    }
    at(0x273b50);list=*sourceLight3A8_;if(!list)return stop(pc);
    at(0x273b64);if(!list->wrappers38||list->sourceWrappersConstructed<1)return stop(pc);
    auto& actual=list->wrappers38[0];sourceLightPopulationWrapper_=&actual;
    at(0x273b6c);if(!actual.sourceConstructed||!actual.sourceDescriptorPublished||!actual.sourceDescriptor04)return stop(pc);
    sourceLightPopulationDescriptor_=actual.sourceDescriptor04.get();
    at(0x273b70); //Actual wrapper3D2B30+0C=277D80, not descriptor reader277D10.
    sourceLightPopulationPhase_=SourceLightPopulationPhase::wrapperPending;at(0x273b74);
    //This bounded source branch is the actual first Ambient descriptor. No
    //metadata substitute, local primitive vector rewrite, or manager adoption.
    auto& d=*sourceLightPopulationDescriptor_;
    at(0x277da0);if(!d.loaded||d.type00!=4||d.knownFlags28!=0x7f||
        (d.flags28&0x7f)!=4||d.position14||d.look18||d.attenuation1C)return stop(pc);
    at(0x277dd4); //Known bit2 selects the genuine counted light constructor.
    if(!constructSourceFirstLight146720(pc))return stop(pc);
    at(0x277e2c);sourceLightPopulationPhase_=SourceLightPopulationPhase::bounded;
    if(!sourceFirstLight_)return stop(0x146fc0); //Original NULL receiver would fault; no fabricated failure return.
    if(d.color04!=std::array<std::uint32_t,4>{{0x42c80000u,0x42c80000u,0x42c80000u,0x437f0000u}}||d.range24||
       d.position14||d.look18||d.attenuation1C||!sourceTopologyLightManager_||
       sourceLoadApplication_->sourceLightManager426F60Owner_!=sourceTopologyLightManager_)return stop(0x146f80);
    auto& light=*sourceFirstLight_;
    at(0x146fc0);light.type34=4;at(0x146fd8);light.ordinalC8=0;
    //Exact half of100,100,100,255; no arbitrary FCSR/normalization inference.
    at(0x146c84);light.ambient5C={{50,50,50,127.5f}};
    at(0x146cc0);light.diffuse3C={};light.specular4C={};light.colorsWritten=true;
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic md;
    at(0x146e30);if(!sourceTopologyLightManager_->incrementSourceAmbient149300(*this,md))return stop(md.sourcePC);
    light.revisionWritten=true;
    at(0x146e60);std::uint32_t luminance=0;
    if(!sourceFirstLightLuminance146E60(luminance)||luminance)return stop(pc);
    //Source147028 range0 skips range writes/revision. NULL position/look skip
    //their branches; NULL attenuation writes1,0,0 at147424..2C. Ambient leaves mode3.
    at(0x146e44);light.luminanceCC=luminance;
    at(0x14742c);light.attenuation8C={{1,0,0}};light.configured=true;
    at(0x147494); //146F60 returns this, distinct from wrapper return.
    at(0x277fcc);if(!sourceTopologyLightManager_->appendSourceAmbient148F30(*this,md))return stop(md.sourcePC);
    at(0x277fd4); //Same descriptor, known bits4/5 false: no flare owner or flasher call.
    if(sourceLightPopulationDescriptor_!=actual.sourceDescriptor04.get()||!actual.sourceDescriptor04||
       (d.knownFlags28&0x30)!=0x30||(d.flags28&0x30)||d.position14||d.look18||d.attenuation1C)return stop(pc);
    at(0x2780ec);d.vtableF4=0x3d2b40; //Actual descriptor destructor vtable restore.
    //The three actual vector branches areNULL for this Ambient; no frees or
    //newNULL stores are invented for branches which skip them.
    at(0x27813c);sourceLightPopulationDescriptor_=nullptr; //Withdraw native borrower BEFORE outerfree.
    actual.sourceDescriptor04.reset();
    at(0x278144);sourceFirstWrapperV0_=1;
    at(0x278148);sourceFirstDescriptorDisposed_=true; //Actual wrapper04=NULL publication.
    at(0x27815c);sourceFirstWrapperReturned_=true;
    at(0x273b7c);if(*sourceLight3A8_!=nativeLightList3A8_.get())return stop(pc);
    at(0x273b94);if(actual.live00!=&light.entity)return stop(pc);
    if(!attachSourceFirstLight141960(pc))return stop(pc);
    at(0x273ba0);++*sourceLightPopulationS0_; //Actual increment,273BA4 is its followingNOP.
    at(0x273ba8);return continueSourceSecondLight273BA8(pc);



}
}
