#pragma once
#include <new>
namespace ra_pod_scene50LA {
bool Owner::constructSourceTopologyContainer141300(Entity& e,bool& counted,bool& enrolled,std::uint32_t& pc)noexcept {
    pc=0x141300;if(!worldApplication_||counted||enrolled)return false;
    auto& authority=worldApplication_->entityAuthority_;
    if(!authority.canEnroll(e,key(&e)))return false;
    const auto r=g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::entity141300,
        worldEntity_.sessionGeneration,key(&e),true,worldApplication_->objectCount423F60());
    counted=e.reference00!=0;
    if(r.status!=g::OriginalWreckComponentStatus50LA::complete){pc=r.sourcePC;return false;}
    if(!authority.enroll(e))return false;enrolled=true;return true;
}
}
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldTopologyNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||!preflightSourceWorldSearchNodesNative()||!preflightSourceWorldPortalGraphNative()||!preflightSourceWorldMethodArraysNative()||!preflightSourceDefaultObserverNative()||!preflightSourceLightPopulationNative()||!preflightSourceVisibilityEnvironmentNative()||!preflightSourcePostParticleModelsNative())return false;
    if(sourceTopologyPhase_==SourceTopologyPhase::unreached)
        return !sourceTopologyContainer_&&!sourceTopologyLightManager_&&!sourceTopologyMode_;
    if(sourceTopologyPhase_==SourceTopologyPhase::nativeClosed)
        return sourceTopologyMode_&&sourceTopologyWithdrawn_&&!sourceTopologyContainer_&&!sourceTopologyLightManager_&&
            (sourceTopologyContainerPublished_?!sourceContainer4BC_:(sourceContainer4BC_&&!*sourceContainer4BC_))&&
            (sourceTopologyVisibilityPublished_?!sourceVisibility4C0_:(sourceVisibility4C0_&&!*sourceVisibility4C0_))&&
            (sourceTopologyCollisionPublished_?!sourceCollision4C4_:(sourceCollision4C4_&&!*sourceCollision4C4_))&&
            !sourceVisibilitySlots4C0_&&!sourceCollisionSlots4C4_;
    if(!sourceTopologyMode_||sourceTopologyWithdrawn_||topologyAttempted_||topologyConstructed_||topologyClosed_||
       !worldApplication_||worldEntity_.reference00!=1||container4BC_.reference00||domainCount_||
       sourceVisibilityPublished_>10||sourceCollisionPublished_>sourceVisibilityPublished_||nativeWreckZoneBorrower_||constructorRacerWorldBorrows_)return false;
    if(sourceTopologyContainer_){
        if(!preflightSourcePortalContainerNative(*sourceTopologyContainer_,sourceTopologyContainerCounted_,sourceTopologyContainerEnrolled_))return false;
    }else if(sourceTopologyContainerCounted_||sourceTopologyContainerEnrolled_)return false;
    if(sourceTopologyContainerPublished_){if(!sourceContainer4BC_||*sourceContainer4BC_!=sourceTopologyContainer_.get())return false;}
    else if(!sourceContainer4BC_||*sourceContainer4BC_)return false;
    if(sourceTopologyVisibilityPublished_){if(!sourceVisibility4C0_||*sourceVisibility4C0_!=sourceVisibilitySlots4C0_.get())return false;}
    else if(!sourceVisibility4C0_||*sourceVisibility4C0_||sourceVisibilitySlots4C0_)return false;
    if(sourceTopologyCollisionPublished_){if(!sourceCollision4C4_||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get())return false;}
    else if(!sourceCollision4C4_||*sourceCollision4C4_||sourceCollisionSlots4C4_)return false;
    //Exact node-frame preflight above checks only source-produced slots and native withdrawal ledgers.
    return !sourceTopologyLightManager_||sourceTopologyLightManager_->preflightSourceRaZonesNative(*this);
}
bool Owner::preflightSourceWorldTopologyCountNative()const noexcept {
    return preflightSourceWorldParticleCountNative(); //Combined checked floor; local topology preflight is nonrecursive.
}
bool Owner::sourceWorldLightZonesInvocationCurrent(const rr::platform::OriginalUiLightManagerOwner50KG& manager)const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&!closed_&&!terminal_&&!nativeScbRevoked_&&
        sourceTopologyPhase_==SourceTopologyPhase::entered&&sourceTopologyPC_==0x272a48&&sourceTopologyMode_&&
        sourceTopologyContainerPublished_&&sourceTopologyVisibilityPublished_&&sourceTopologyCollisionPublished_&&
        (!sourceTopologyLightManager_||sourceTopologyLightManager_.get()==&manager)&&sourceAudioCloseReturned_&&sourceAudioCloseNativeWithdrawn_&&
        sourceTrackStep_==SourceTrackStep::audioConfigClosed&&sourceLoadRace_&&sourceLoadApplication_&&
        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&sourceLoadApplication_->entities()==worldApplication_&&
        sourceLoadApplication_->sourceLightManager426F60Owner_.get()==&manager&&
        worldZones34C_&&*worldZones34C_==10&&preflightSourceWorldTopologyCountNative();
}
bool Owner::abortSourceWorldTopologyNative(std::uint32_t& pc)noexcept {
    if(!preflightSourceWorldTopologyCountNative())return false;
    if(sourceTopologyPhase_==SourceTopologyPhase::unreached||sourceTopologyPhase_==SourceTopologyPhase::nativeClosed)return true;
    if(!retireSourceVisibilityEnvironmentNative(pc))return false;
    if(!abortSourceLightPopulationNative(pc))return false;
    if(!abortSourceDefaultObserverNative(pc))return false;
    if(!abortSourceWorldMethodArraysNative(pc))return false;
    if(!abortSourceWorldPortalGraphNative(pc))return false;
    if(!abortSourceWorldSearchNodesNative(pc))return false;
    if(sourceTopologyLightManager_){
        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic d;
        if(!sourceTopologyLightManager_->abortSourceRaZonesNative(*this,d)){pc=d.sourcePC;return false;}
    }
    if(sourceTopologyContainer_&&!releaseSourceParticleListCountNative(*sourceTopologyContainer_,
       sourceTopologyContainerCounted_,sourceTopologyContainerEnrolled_))return false;
    //No callbacks/late admission follow. Withdraw exact publication knowledge before backing disposal.
    if(sourceTopologyContainerPublished_)sourceContainer4BC_.reset();
    if(sourceTopologyVisibilityPublished_)sourceVisibility4C0_.reset();
    if(sourceTopologyCollisionPublished_)sourceCollision4C4_.reset();
    sourceCollisionSlots4C4_.reset();sourceVisibilitySlots4C0_.reset();sourceTopologyContainer_.reset();
    sourceTopologyLightManager_.reset();sourceTopologyWithdrawn_=true;sourceTopologyPhase_=SourceTopologyPhase::nativeClosed;
    return true; //Native-only disposal; no original NULL/count90 reset or topology return.
}
bool Owner::loadSourceWorldTopology2729E0(std::uint32_t& pc)noexcept {
    pc=sourceTrackPC_;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceTopologyPhase_!=SourceTopologyPhase::unreached||sourceTopologyMode_||topologyAttempted_||topologyClosed_||
       sourceTrackPC_!=0x1aa130||sourceTrackStep_!=SourceTrackStep::audioConfigClosed||
       !sourceAudioCloseReturned_||!sourceAudioCloseNativeWithdrawn_||!sourceWorldAudioConfig_||
       !sourceWorldAudioConfig_->sourceClosed_||!sourceWorldAudioConfig_->nativeClosed_||sourceWorldAudioConfig_->source_||
       !sourceLoadRace_||!sourceLoadApplication_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!worldZones34C_||*worldZones34C_!=10||
       !sourceContainer4BC_||*sourceContainer4BC_||!sourceVisibility4C0_||*sourceVisibility4C0_||
       !sourceCollision4C4_||*sourceCollision4C4_||sourceVisibilitySlots4C0_||sourceCollisionSlots4C4_||
       !preflightSourceWorldTopologyCountNative()||!reserveSourceParticleListNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourceTopologyPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourceTopologyPhase_=SourceTopologyPhase::bounded;return false;};
    sourceTopologyMode_=true;sourceTopologyPhase_=SourceTopologyPhase::entered;
    at(0x272a00);sourceTopologyContainer_.reset(new(std::nothrow) g::OriginalWreckComponent50LA);
    if(sourceTopologyContainer_&&!constructSourceTopologyContainer141300(*sourceTopologyContainer_,
       sourceTopologyContainerCounted_,sourceTopologyContainerEnrolled_,pc))return stop(pc);
    at(0x272a1c);sourceContainer4BC_=sourceTopologyContainer_.get();sourceTopologyContainerPublished_=true;
    at(0x272a24);sourceVisibilitySlots4C0_.reset(new(std::nothrow) SearchNode*[10]);
    at(0x272a2c);sourceVisibility4C0_=sourceVisibilitySlots4C0_.get();sourceTopologyVisibilityPublished_=true;
    at(0x272a34);sourceCollisionSlots4C4_.reset(new(std::nothrow) SearchNode*[10]);
    at(0x272a3c);sourceCollision4C4_=sourceCollisionSlots4C4_.get();sourceTopologyCollisionPublished_=true;
    //Neither NULL outer nor NULL pointer arrays skips the genuine148B60 call.
    at(0x272a48);rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic d;
    if(!rr::platform::OriginalUiLightManagerOwner50KG::bindSourceRaWorldZones272A48(
       *sourceLoadApplication_,*this,sourceTopologyLightManager_,d))return stop(d.sourcePC?d.sourcePC:pc);
    sourceTopologyPhase_=SourceTopologyPhase::lightZonesReturned;
    pc=sourceTrackPC_=sourceTopologyPC_=0x272a50;return true; //Before source loop initialization/dereferences272A58.
}
}

namespace ra_cold_aggregate50LA {
bool Owner::sourceWorldSearchNodesInvocationCurrent()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceTopologyPhase_!=SourceTopologyPhase::lightZonesReturned||sourceTopologyPC_!=0x272a50||sourceTrackPC_!=0x272a50||
       !sourceTopologyMode_||sourceTopologyWithdrawn_||sourceSearchNodesPhase_!=SourceSearchNodesPhase::unreached||
       !sourceTopologyLightManager_||!sourceLoadRace_||!sourceLoadApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||sourceLoadApplication_->entities()!=worldApplication_||
       sourceLoadApplication_->sourceLightManager426F60Owner_!=sourceTopologyLightManager_||
       !worldZones34C_||*worldZones34C_!=10||sourceZonesPhase_!=SourceZonesPhase::returned||
       !sourceZones350_||!worldZoneInfo350_||*worldZoneInfo350_!=sourceZones350_.get()||
       sourceZonesConstructed_!=10||sourceZonesLoaded_!=10||!preflightSourceWorldZonesNative()||
       !preflightSourceWorldTopologyCountNative())return false;
    std::size_t count=0;
    if(!sourceTopologyLightManager_->sourceRaZonesCounted(*this,count)||count!=10)return false;
    for(std::size_t i=0;i<10;++i){const auto& z=sourceZones350_[i];
        if(!z.constructed||z.exclusionCount124<=0||!z.exclusions120||
           z.exclusionsProduced!=static_cast<std::size_t>(z.exclusionCount124))return false;
    }
    return true; //Actual retained source350 rows, no native metadata or Config reopening.
}
}
