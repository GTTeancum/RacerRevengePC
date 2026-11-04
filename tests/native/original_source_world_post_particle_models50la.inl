#pragma once
#include "../../src/game/original_shared_rng50iu.h"
#include "../../src/game/original_source_particle_random50la.h"
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourcePostParticleModelsNative()const noexcept {
    if(!preflightSourceFinishLineNative())return false;
    const auto& anchors=sourceFinishLineAnchors_;
    if(anchors.entered&&!sourceFinishLineMaterial544_)return false;
    if(anchors.initialized&&!anchors.entered)return false;
    if(anchors.scanReturned&&(!anchors.initialized||!anchors.liveCount||
       anchors.ordinal<*anchors.liveCount))return false;
    for(const auto& slot:anchors.lastMatchingSlot)
        if(slot&&(!anchors.initialized||*slot>=anchors.ordinal))return false;
    const auto& comparison=sourceFinishLineComparison_;
    if(comparison.entered&&!anchors.scanReturned)return false;
    for(unsigned i=0;i<2;++i){
        const auto mask=comparison.readMask[i];
        if(mask!=0&&mask!=1&&mask!=3&&mask!=7)return false;
        if(mask&&!comparison.entered)return false;
        if(comparison.equalsOrigin[i]&&(!mask||(*comparison.equalsOrigin[i]&&mask!=7)))return false;
    }
    if(comparison.readMask[1]&&(!comparison.equalsOrigin[0]||*comparison.equalsOrigin[0]))return false;
    if(comparison.returned){
        if(!comparison.entered||!comparison.equalsOrigin[0]||!comparison.missingAnchor)return false;
        if(*comparison.equalsOrigin[0]){
            if(comparison.readMask[1]||comparison.equalsOrigin[1]||!*comparison.missingAnchor)return false;
        }else if(!comparison.equalsOrigin[1]||*comparison.missingAnchor!=*comparison.equalsOrigin[1])return false;
    }else if(comparison.missingAnchor)return false;
    if(sourceFinishLineGeometry_.entered&&(!comparison.returned||!comparison.missingAnchor||
       *comparison.missingAnchor))return false;
    if(sourceFinishLineMissingCleanup_.entered&&
       (!comparison.returned||comparison.missingAnchor!=true))return false;
    const auto& finishReturn=sourceFinishLineCallerReturn_;
    if(finishReturn.restoreMask!=0&&finishReturn.restoreMask!=1&&finishReturn.restoreMask!=3&&
       finishReturn.restoreMask!=7&&finishReturn.restoreMask!=15)return false;
    if(finishReturn.restoreMask){
        if(!comparison.returned||!comparison.missingAnchor)return false;
        if(*comparison.missingAnchor?!sourceFinishLineMissingCleanup_.returned:
           !sourceFinishLineGeometry_.returned)return false;
    }
    if(finishReturn.stackReleased&&(finishReturn.restoreMask!=15||!finishReturn.v0Preserved))return false;
    if(finishReturn.returned!=finishReturn.stackReleased)return false;
    if(finishReturn.v0Preserved&&!finishReturn.restoreMask)return false;
    if(finishReturn.loadingMode30&&!finishReturn.returned)return false;
    if(finishReturn.progressSkipped&&(!finishReturn.loadingMode30||*finishReturn.loadingMode30))return false;
    const auto& afterCollision=sourcePostCollisionCaller_;
    if(afterCollision.loadingMode30&&!sourceCollisionDispatch128_.worldReturned)return false;
    if(afterCollision.progressSkipped&&(!afterCollision.loadingMode30||*afterCollision.loadingMode30))return false;
    if(afterCollision.animationDirectoryStoreMask!=0&&afterCollision.animationDirectoryStoreMask!=1&&
       afterCollision.animationDirectoryStoreMask!=3)return false;
    if(afterCollision.animationDirectoryStoreMask&&!afterCollision.progressSkipped)return false;
    if(afterCollision.worldTable08&&(afterCollision.animationDirectoryStoreMask!=3||
       *afterCollision.worldTable08!=0x3cf110u))return false;
    if(afterCollision.target120&&(!afterCollision.worldTable08||*afterCollision.target120!=0x1b40b0u))return false;
    if(afterCollision.baseToyNullRead&&!afterCollision.target120)return false;
    if(afterCollision.baseToyReturnV0&&(!afterCollision.baseToyNullRead||*afterCollision.baseToyReturnV0!=1))return false;
    if(afterCollision.derivedToyNullRead&&!afterCollision.baseToyReturnV0)return false;
    if(afterCollision.toyReturnV0&&(!afterCollision.derivedToyNullRead||
       afterCollision.toyReturnV0!=afterCollision.baseToyReturnV0))return false;
    if(afterCollision.toyReturned!=bool(afterCollision.toyReturnV0))return false;
    if(afterCollision.target124&&(!afterCollision.toyReturned||*afterCollision.target124!=0x274370u))return false;
    if(afterCollision.particleDispatched&&!afterCollision.target124)return false;
    if(afterCollision.particleReturnV0&&(!afterCollision.particleDispatched||*afterCollision.particleReturnV0!=1||
       !sourceParticleNextRows_[3].listReturned))return false;
    const auto particleReturnMask=afterCollision.particleWrapperReturnMask;
    if(particleReturnMask!=0&&particleReturnMask!=1&&particleReturnMask!=3&&
       particleReturnMask!=7&&particleReturnMask!=15)return false;
    //Enforce the bounded proof even at mask0 and after nativeClosed skips
    //live descriptor preflight. Generic non-Fog returns keep their old path.
    const bool boundedParticleReturn=sourceParticleLaterRows_[2].emitterType==12u||
        bool(sourceNonFogPrecedingFogRowNative(3));
    if(boundedParticleReturn){
        if(bool(afterCollision.particleReturnV0)!=bool(particleReturnMask)||
           afterCollision.particleReturned!=(particleReturnMask==15))return false;
    }else if(particleReturnMask||afterCollision.particleReturned!=bool(afterCollision.particleReturnV0))return false;
    if(particleReturnMask){
        //History only: no live owner/count dereference after native withdrawal.
        if(afterCollision.particleReturnV0!=1u)return false;
        for(std::size_t i=0;i<4;++i){
            const auto& rowService=i?sourceParticleLaterRows_[i-1]:sourceParticleService124_;
            if(!rowService.rowServiceReturned||!rowService.descriptorFreed||rowService.configEmitterIndex!=0u||
               (i&&(!sourceParticleNextRows_[i-1].dispatched||sourceParticleNextRows_[i-1].rowS0!=i)))return false;
        }
    }
    if(afterCollision.target144&&(!afterCollision.particleReturned||*afterCollision.target144!=0x274e90u))return false;
    if(!preflightSourcePostParticleCallerNative()||!preflightSourceTriggerServiceNative())return false;
    const auto& postTrigger=sourcePostTriggerCaller_;
    if((postTrigger.entered&&(!sourceTriggerService274E90_.wrapperReturned||
         sourceTriggerService274E90_.wrapperReturnV0!=1u))||
       (postTrigger.applicationRead&&!postTrigger.entered)||(postTrigger.profileRead&&!postTrigger.applicationRead)||
       (postTrigger.profile1125C&&!postTrigger.profileRead)||
       (postTrigger.mode&&(!postTrigger.profile1125C||*postTrigger.mode!=((*postTrigger.profile1125C>>5u)&3u)))||
       (postTrigger.worldRead&&!postTrigger.mode)||
       (postTrigger.worldTable08&&(!postTrigger.worldRead||*postTrigger.worldTable08!=0x3cf110u))||
       (postTrigger.target13C&&(!postTrigger.worldTable08||*postTrigger.target13C!=0x274ae0u))||
       (postTrigger.argumentA1&&(!postTrigger.target13C||!postTrigger.mode||
         *postTrigger.argumentA1!=(*postTrigger.mode<2u?1u:0u)))||
       (postTrigger.dispatched&&!postTrigger.argumentA1))return false;
    const auto& sky=sourceSkySetup274AE0_;
    if((sky.entered&&!postTrigger.dispatched)||(sky.environmentRead&&!sky.entered)||
       (sky.environmentNonNull&&!sky.environmentRead)||
       (sky.argumentS1&&(!sky.environmentRead||sky.argumentS1!=postTrigger.argumentA1))||
       (sky.count00&&(!sky.argumentS1||!sky.environmentNonNull))||
       (sky.arrayBytes&&(!sky.count00||*sky.arrayBytes!=(static_cast<std::uint32_t>(*sky.count00)<<2u)))||
       (sky.returnV0&&(!sky.argumentS1||*sky.returnV0!=1u||
         (sky.environmentNonNull&&(!sky.count00||*sky.count00!=0)))))return false;
    if(sky.restoreMask!=0&&sky.restoreMask!=1&&sky.restoreMask!=3&&sky.restoreMask!=7&&sky.restoreMask!=15)return false;
    if((sky.restoreMask&&!sky.returnV0)||(sky.stackReleased&&sky.restoreMask!=15)||
       sky.returned!=sky.stackReleased||
       (sky.callerResumePC&&(!sky.returned||!sky.argumentS1||
          *sky.callerResumePC!=(*sky.argumentS1?0x1aa33cu:0x1aa320u)))||
       (sky.callerSkipExecuted&&sky.callerResumePC!=0x1aa320u))return false;
    if(sky.environmentRead&&sourceEnvironmentPhase_!=SourceEnvironmentPhase::nativeClosed){
        if(!sourceEnvironmentPublished_||!nativeEnvironment3A4_||
           *nativeEnvironment3A4_!=sourceEnvironment3A4_.get()||sky.environmentNonNull!=bool(sourceEnvironment3A4_))return false;
        if(sky.environmentNonNull){
            if(sourceEnvironmentPhase_!=SourceEnvironmentPhase::returnedOne||!sourceWorldConfig_||
               !sourceEnvironment3A4_->preflightSourceNative())return false;
            if(sky.count00){std::int32_t current=0;
                if(!sourceEnvironment3A4_->readSourceSkyCount274B0C(worldEntity_,*sourceWorldConfig_,current)||
                   current!=*sky.count00)return false;}
        }else if(sourceEnvironmentPhase_!=SourceEnvironmentPhase::returnedZero)return false;
    }
    if(!preflightSourceSkyArrayNative())return false;
    const auto& particle=sourceParticleService124_;
    if(particle.entered&&!afterCollision.particleDispatched)return false;
    if(particle.listRead&&!particle.entered)return false;
    if(particle.listNonNull&&!particle.listRead)return false;
    if(particle.listTable08&&(!particle.listNonNull||*particle.listTable08!=0x3d24f0u))return false;
    if(particle.listTarget78&&(!particle.listTable08||*particle.listTarget78!=0x27dfd0u))return false;
    if(particle.listEntered&&!particle.listTarget78)return false;
    if(particle.rowS0&&(!particle.listEntered||*particle.rowS0!=0))return false;
    if(particle.offsetS1&&(!particle.rowS0||*particle.offsetS1!=0))return false;
    if(particle.count30&&(!particle.offsetS1||*particle.count30!=4))return false;
    if(particle.rowRead&&!particle.count30)return false;
    if(particle.rowTable14&&(!particle.rowRead||*particle.rowTable14!=0x3d2570u))return false;
    if(particle.rowTarget0C&&(!particle.rowTable14||*particle.rowTarget0C!=0x27c7b0u))return false;
    if(particle.rowEntered&&!particle.rowTarget0C)return false;
    if(particle.descriptorRead&&!particle.rowEntered)return false;
    if(particle.emitterIsNone&&!particle.descriptorRead)return false;
    if(particle.descriptorType04&&particle.emitterIsNone!=false)return false;
    if(particle.rowType08Stored&&!particle.descriptorType04)return false;
    if(particle.guard3D6528Read&&(!particle.rowType08Stored||*particle.guard3D6528Read>1))return false;
    if(particle.assertionLazyStored&&(!particle.guard3D6528Read||*particle.guard3D6528Read))return false;
    if(particle.assertion443068Read&&(!particle.guard3D6528Read||
       *particle.assertion443068Read!=1||(!*particle.guard3D6528Read&&!particle.assertionLazyStored)))return false;
    if(particle.manager4A0Read&&!particle.assertion443068Read)return false;
    if(particle.manager4A0NonNull&&!particle.manager4A0Read)return false;
    if(particle.assertionReturnV0&&(!particle.manager4A0NonNull||*particle.assertionReturnV0))return false;
    if(particle.instances330Read&&!particle.assertionReturnV0)return false;
    if(particle.countPositive&&(!particle.instances330Read||*particle.instances330Read<=0))return false;
    if(particle.emitterType&&(!particle.countPositive||particle.emitterType!=particle.descriptorType04))return false;
    if(bool(particle.emitterType)!=bool(particle.emitterAllocationBytes)||
       bool(particle.emitterType)!=bool(particle.emitterConstructor))return false;
    if(particle.emitterArrayAttempted&&!particle.emitterType)return false;
    if(particle.emitterAttempts&&(!particle.emitterArrayAttempted||
       particle.emitterAttempts>static_cast<std::size_t>(*particle.instances330Read)))return false;
    if(particle.emitterPublishedSlots>particle.emitterAttempts)return false;
    if(particle.emitterArrayPublished&&(!particle.emitterArrayAttempted||
       particle.emitterPublishedSlots!=static_cast<std::size_t>(*particle.instances330Read)))return false;
    if(particle.rowCount0CStored&&!particle.emitterArrayPublished)return false;
    if(particle.rowStart10Stored&&!particle.rowCount0CStored)return false;
    if(particle.attachmentOffsetE0&&!particle.rowStart10Stored)return false;
    if(particle.attachmentIsNone&&!particle.attachmentOffsetE0)return false;
    if(particle.attachmentZone&&(particle.attachmentIsNone!=false||*particle.attachmentZone>10))return false;
    if(particle.attachmentSlot&&!particle.attachmentZone)return false;
    if(particle.attachmentLiveZones&&(!particle.attachmentZone||*particle.attachmentLiveZones!=10))return false;
    if(particle.attachmentLiveCount&&!particle.attachmentSlot)return false;
    if(particle.attachmentMatches!=(particle.matchCountAdvanced?1u:0u))return false;
    if(particle.matchCountAdvanced&&(particle.attachmentIsNone!=false||!(particle.emitterType==0u?particle.type0LoadEntered:(particle.emitterType==1u?particle.type1LoadEntered:(particle.emitterType==2u?particle.type2LoadEntered:(particle.emitterType==9u?particle.type9LoadEntered:(particle.emitterType==11u?particle.type11LoadEntered:(particle.emitterType==12u?particle.type12LoadEntered:particle.type8LoadEntered))))))))return false;
    if(particle.completionGuardRead&&(!particle.matchCountAdvanced||*particle.completionGuardRead>1))return false;
    if(particle.completionLazyStored&&(!particle.completionGuardRead||*particle.completionGuardRead))return false;
    if(particle.completionAssertionRead&&(!particle.completionGuardRead||*particle.completionAssertionRead!=1))return false;
    if(particle.completionAssertionReturned&&!particle.completionAssertionRead)return false;
    if(particle.descriptorDeleteAdmitted&&(particle.attachmentIsNone==false&&!particle.completionAssertionReturned))return false;
    if(particle.descriptorFreed&&!particle.descriptorDeleteAdmitted)return false;
    if(particle.rowServiceReturned&&!particle.descriptorFreed)return false;
    if(particle.attachmentMatched&&(!particle.attachmentSlot||!particle.attachmentLiveCount||
       *particle.attachmentSlot>=*particle.attachmentLiveCount))return false;
    if(particle.attachmentPhysics&&!particle.attachmentMatched)return false;
    if(particle.childIsNone&&(!particle.attachmentMatched||particle.attachmentPhysics))return false;
    if(particle.childLookupPending&&particle.childIsNone!=false)return false;
    if(particle.configEntered&&(!particle.configTarget10||*particle.configTarget10!=0x27d7a0u||
       !particle.configEmitterIndex||*particle.configEmitterIndex!=0||!particle.attachmentIsNone||
       (particle.attachmentIsNone==false&&(!particle.attachmentMatched||particle.childIsNone!=true))))return false;
    if(particle.configModelNonNull&&(!particle.configEntered||!particle.attachmentMatched))return false;
    if(particle.configEmitterNonNull&&!particle.configEntered)return false;
    if(particle.configManagerRead&&!particle.configEntered)return false;
    if(particle.configLoaderTarget&&!particle.configManagerRead)return false;
    if(particle.type0LoadEntered&&(particle.configLoaderTarget!=0x15b9d0u||particle.emitterType!=0u))return false;
    if(particle.type0NoOpReturned&&!particle.type0LoadEntered)return false;
    if(particle.type0ManagerStored&&(!particle.type0NoOpReturned||!particle.configEmitterNonNull))return false;
    if(particle.type0ModelStored&&(!particle.type0ManagerStored||!particle.configModelNonNull))return false;
    if(particle.type0OffsetStored&&!particle.type0ModelStored)return false;
    if(particle.type0DirectoryCopied&&!particle.type0ManagerStored)return false;
    if(particle.type0FilenameAppended&&!particle.type0DirectoryCopied)return false;
    if(particle.type1LoadEntered&&(particle.configLoaderTarget!=0x15ce90u||particle.emitterType!=1u))return false;
    if(particle.type1NoOpReturned&&!particle.type1LoadEntered)return false;
    if(particle.type1ManagerStored&&(!particle.type1NoOpReturned||!particle.configEmitterNonNull))return false;
    if(particle.type1ModelStored&&(!particle.type1ManagerStored||!particle.configModelNonNull))return false;
    if(particle.type1OffsetStored&&!particle.type1ModelStored)return false;
    if(particle.type1DirectoryCopied&&!particle.type1ManagerStored)return false;
    if(particle.type1FilenameAppended&&!particle.type1DirectoryCopied)return false;
    if(particle.type2LoadEntered&&(particle.configLoaderTarget!=0x15d6d0u||particle.emitterType!=2u))return false;
    if(particle.type2NoOpReturned&&!particle.type2LoadEntered)return false;
    if(particle.type2ManagerStored&&(!particle.type2NoOpReturned||!particle.configEmitterNonNull))return false;
    if(particle.type2ModelStored&&(!particle.type2ManagerStored||!particle.configModelNonNull))return false;
    if(particle.type2OffsetStored&&!particle.type2ModelStored)return false;
    if(particle.type2DirectoryCopied&&!particle.type2ManagerStored)return false;
    if(particle.type2FilenameAppended&&!particle.type2DirectoryCopied)return false;
    if(particle.type8LoadEntered&&(particle.configLoaderTarget!=0x15def0u||particle.emitterType!=8u))return false;
    if(particle.type8NoOpReturned&&!particle.type8LoadEntered)return false;
    if(particle.type8ManagerStored&&(!particle.type8NoOpReturned||!particle.configEmitterNonNull))return false;
    if(particle.type8ModelStored&&(!particle.type8ManagerStored||!particle.configModelNonNull))return false;
    if(particle.type8OffsetStored&&!particle.type8ModelStored)return false;
    if(particle.type8DirectoryCopied&&!particle.type8ManagerStored)return false;
    if(particle.type8FilenameAppended&&!particle.type8DirectoryCopied)return false;
    if(particle.type9LoadEntered&&(particle.configLoaderTarget!=0x15e8f0u||particle.emitterType!=9u))return false;
    if(particle.type9NoOpReturned&&!particle.type9LoadEntered)return false;
    if(particle.type9ManagerStored&&(!particle.type9NoOpReturned||!particle.configEmitterNonNull))return false;
    if(particle.type9ModelStored&&(!particle.type9ManagerStored||!particle.configModelNonNull))return false;
    if(particle.type9OffsetStored&&!particle.type9ModelStored)return false;
    if(particle.type9DirectoryCopied&&!particle.type9ManagerStored)return false;
    if(particle.type9FilenameAppended&&!particle.type9DirectoryCopied)return false;
    if(particle.type11LoadEntered&&(particle.configLoaderTarget!=0x15f1e0u||particle.emitterType!=11u))return false;
    if(particle.type11NoOpReturned&&!particle.type11LoadEntered)return false;
    if(particle.type11ManagerStored&&(!particle.type11NoOpReturned||!particle.configEmitterNonNull))return false;
    if(particle.type11ModelStored&&(!particle.type11ManagerStored||!particle.configModelNonNull))return false;
    if(particle.type11OffsetStored&&!particle.type11ModelStored)return false;
    if(particle.type11DirectoryCopied&&!particle.type11ManagerStored)return false;
    if(particle.type11FilenameAppended&&!particle.type11DirectoryCopied)return false;
    if(particle.type12LoadEntered&&(particle.configLoaderTarget!=0x15ff10u||particle.emitterType!=12u))return false;
    if(particle.type12NoOpReturned&&!particle.type12LoadEntered)return false;
    if(particle.type12ManagerStored&&(!particle.type12NoOpReturned||!particle.configEmitterNonNull))return false;
    if(particle.type12ModelStored&&(!particle.type12ManagerStored||!particle.configModelNonNull))return false;
    if(particle.type12OffsetStored&&!particle.type12ModelStored)return false;
    if(particle.type12DirectoryCopied&&!particle.type12ManagerStored)return false;
    if(particle.type12FilenameAppended&&!particle.type12DirectoryCopied)return false;
    //A registered Fog may retire this descriptor; next-row entry remains separately bounded.
    if(particle.emitterConfigConstructAttempted&&!(particle.emitterType==0u?particle.type0FilenameAppended:(particle.emitterType==1u?particle.type1FilenameAppended:(particle.emitterType==2u?particle.type2FilenameAppended:(particle.emitterType==9u?particle.type9FilenameAppended:(particle.emitterType==11u?particle.type11FilenameAppended:(particle.emitterType==12u?particle.type12FilenameAppended:particle.type8FilenameAppended)))))))return false;
    if(particle.emitterConfigConstructReturned&&!particle.emitterConfigConstructAttempted)return false;
    if(particle.emitterConfigOpenAttempted&&!particle.emitterConfigConstructReturned)return false;
    if(particle.emitterConfigOpenReturned&&!particle.emitterConfigOpenAttempted)return false;
    if(particle.emitterConfigTarget94&&(particle.emitterConfigOpenReturned!=true||*particle.emitterConfigTarget94!=(particle.emitterType==0u?0x15c330u:(particle.emitterType==1u?0x15d560u:(particle.emitterType==2u?0x15dd00u:(particle.emitterType==9u?0x15ef70u:(particle.emitterType==11u?0x15fc10u:(particle.emitterType==12u?0x1602b0u:0x15e6d0u))))))))return false;
    if(particle.toyScanNullRead&&(!particle.attachmentZone||!particle.attachmentLiveZones||
       *particle.attachmentZone!=*particle.attachmentLiveZones||particle.attachmentMatched))return false;
    const auto& collision=sourceCollisionDispatch128_;
    if(collision.stage>10||collision.zoneS0>10||collision.zoneOffsetS1!=4u*collision.zoneS0||
       collision.slotOffsetS1!=4u*collision.slotS0)return false;
    if(collision.callbackReturned!=bool(collision.callbackReturnV0)||
       (collision.callbackReturnV0&&*collision.callbackReturnV0>1u)||
       (collision.loopCountReloaded&&!collision.callbackReturned))return false;
    if(collision.stage&&!finishReturn.progressSkipped)return false;
    if(collision.stage>=1&&collision.worldTable08!=0x3cf110u)return false;
    if(collision.stage>=2&&collision.target128!=0x2743b0u)return false;
    if(collision.zone354Stored!=bool(collision.storedZone354)||
       (collision.storedZone354&&*collision.storedZone354>=10))return false;
    if(collision.wrapperTable0C&&*collision.wrapperTable0C!=0x3cf0f0u)return false;
    if(collision.target0C&&*collision.target0C!=0x1b5590u)return false;
    if(collision.stage>=7&&!collision.wrapperTable0C)return false;
    if(collision.stage>=8&&!collision.target0C)return false;
    if(collision.stage>=5&&!collision.groupCount30)return false;
    if(collision.worldReturned&&(!collision.liveZoneCount34C||
       static_cast<std::int32_t>(collision.zoneS0)<static_cast<std::int32_t>(*collision.liveZoneCount34C)))return false;
    if(collision.stage>=6&&collision.zoneS0<10&&
       collision.callbacksReturned[collision.zoneS0]!=collision.slotS0)return false;
    for(std::size_t z=0;z<10;++z){
        if((collision.groupsReturned[z]||collision.emptyGroupsSkipped[z])!=(z<collision.zoneS0)||
           ((collision.groupsReturned[z]||collision.emptyGroupsSkipped[z])&&!collision.groupLogAttempted[z])||
           (collision.groupsReturned[z]&&collision.emptyGroupsSkipped[z]))return false;
        if(collision.groupLogWritten[z]&&!collision.groupLogAttempted[z])return false;
        if(z>collision.zoneS0&&(collision.callbacksReturned[z]||collision.groupLogAttempted[z]))return false;
        if(sourcePostParticleModelsPhase_==SourcePostParticleModelsPhase::nativeClosed)continue;
        if(!worldCollisionObjects3CC_){if(collision.callbacksReturned[z])return false;continue;}
        const auto& wrappers=worldCollisionObjects3CC_->sourceWrappers_[z];
        if(collision.emptyGroupsSkipped[z]&&(!wrappers.empty()||collision.callbacksReturned[z]))return false;
        if(collision.callbacksReturned[z]>wrappers.size()||
           (collision.groupsReturned[z]&&collision.callbacksReturned[z]!=wrappers.size()))return false;
        for(std::size_t n=0;n<wrappers.size();++n){
            if(!wrappers[n]){if(n<collision.callbacksReturned[z])return false;continue;}
            const auto& c=wrappers[n]->sourceCallback;
            if(n<collision.callbacksReturned[z]){
                if(!c.returned||!c.returnV0||*c.returnV0>1u||c.pc!=0x27bee8u)return false;
            }else if(c.returned|| (c.entered&&(z!=collision.zoneS0||n!=collision.slotS0)))return false;
        }
    }
    if(collision.zone354Stored&&sourcePostParticleModelsPhase_!=SourcePostParticleModelsPhase::nativeClosed&&
       (!nativeModelZone354_||nativeModelZone354_!=collision.storedZone354))return false;
    if(sourceFinishLineStripOwner_&&sourceFinishLineSetupEntered_){
    if(!sourceLoadApplication_||!sourceLoadApplication_->standardMaterialDomainOwner_||
       sourceLoadApplication_->standardMaterialDomainOwner_!=sourceTrackMountDomain_||
       sourceLoadApplication_->standardMaterialDomainOwner_->materials382B38()!=sourceFinishLineMaterialTable_||
       &sourceLoadApplication_->materialDirectory36B970()!=sourceFinishLineDirectory_||
       !sourceLoadApplication_->materialApplication||!sourceFinishLineMaterialTable_->sourceNames17CAD4Current()||
       !sourceFinishLineMaterialTable_->screenApplicationMatches(*sourceLoadApplication_->materialApplication))return false;
    }

    if(sourceFinishLineConstructorEntered_&&!sourceBuoyAttachNoEffect1B3AE0_)return false;
    if(sourcePostParticleModelsPhase_==SourcePostParticleModelsPhase::unreached)
        return !sourcePostParticleLoadingMode30_&&!sourceGeometryDirectoryStoreMask_&&!sourceEmptyInstancesReturnV0_&&
            !sourceStandardParentS3_&&!sourceStandardWrapperA0_&&!sourceBuoyAttachNoEffect1B3AE0_;
    if(sourcePostParticleModelsPhase_==SourcePostParticleModelsPhase::nativeClosed)
        return !sourceStandardParentS3_&&!sourceStandardWrapperA0_&&!sourceStandardTypeParentRead_&&!sourceFinishLineStripOwner_;
    if(!sourceLoadApplication_||!sourceLoadRace_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||sourceGeometryDirectoryStoreMask_>3)return false;
    if(sourceGeometryDirectoryStoreMask_&&(!sourcePostParticleLoadingMode30_||*sourcePostParticleLoadingMode30_))return false;
    //The later1AA29C/2A0 writes the SAME process lane. Earlier geometry
    //publication is historical after that write, not a second live directory.
    if(sourceGeometryDirectoryStoreMask_==3){
        const auto& directory=sourceLoadApplication_->modelDirectory36B860_;
        if(afterCollision.animationDirectoryStoreMask==0&&directory!="geometry\\")return false;
        if(afterCollision.animationDirectoryStoreMask==1&&directory!="animetry\\")return false;
        if(afterCollision.animationDirectoryStoreMask==3&&directory!="anim\\")return false;
    }
    if((afterCollision.baseToyNullRead||afterCollision.derivedToyNullRead)&&
       (sourceToyPhase_!=SourceToyPhase::returned||!sourceToyLoaderReturnedZero_||
        !sourceToy3C8_||*sourceToy3C8_||nativeEmptyToyList3C8_))return false;
    if(particle.listRead){
        if(sourceParticleDescriptorPhase_!=SourceParticleDescriptorPhase::returned||
           !preflightSourceWorldParticleDescriptorsNative()||!sourceParticleList3D8_||
           *sourceParticleList3D8_!=nativeParticleList3D8_.get()||
           particle.listNonNull!=bool(nativeParticleList3D8_))return false;
        if(particle.rowRead&&(!nativeParticleList3D8_||!nativeParticleList3D8_->sourceSlotPublished[0]||
           !nativeParticleList3D8_->sourceRowReturned[0]||!nativeParticleList3D8_->pointers34||
           nativeParticleList3D8_->pointers34[0]!=nativeParticleList3D8_->rows[0].get()))return false;
        if(particle.rowType08Stored){
            const auto& row=*nativeParticleList3D8_->rows[0];
            if(row.unknown08!=particle.descriptorType04)return false;
            if(row.sourceDescriptorFreed){
                if(!row.sourceDescriptorHistory||row.sourceDescriptorHistory->type!=*particle.descriptorType04)return false;
            }else if(!row.descriptor04||!row.descriptor04->sourceLoaded||!(row.descriptor04->sourceKnownMask&128u)||
                     *particle.descriptorType04!=static_cast<std::uint32_t>(row.descriptor04->type04))return false;
        }
    }
    if(particle.manager4A0Read){
        std::size_t count=0;
        if(!sourceParticleStageCountedNative(count)||sourceParticleStageNativeClosed_||
           sourceParticle4A0NativeWithdrawn_||!sourceParticlePublished4A0_||
           !sourceParticleLoadReturned_||!sourceParticle4A0_||
           *sourceParticle4A0_!=trackParticles_.get()||nativeParticleManager4A0_!=*sourceParticle4A0_||
           particle.manager4A0NonNull!=bool(*sourceParticle4A0_))return false;
    }
    if(particle.instances330Read){
        if(!nativeParticleList3D8_||!nativeParticleList3D8_->rows[0])return false;
        const auto& row=*nativeParticleList3D8_->rows[0];
        if(row.sourceDescriptorFreed){
            if(!row.sourceDescriptorHistory||row.sourceDescriptorHistory->count!=*particle.instances330Read)return false;
        }else if(!row.descriptor04||!(row.descriptor04->sourceKnownMask&512u)||
                 row.descriptor04->instances330!=particle.instances330Read)return false;
    }
    if(particle.attachmentOffsetE0){
        if(!nativeParticleList3D8_||!nativeParticleList3D8_->rows[0])return false;
        const auto& row=*nativeParticleList3D8_->rows[0];
        if(row.sourceDescriptorFreed){
            if(!row.sourceDescriptorHistory||row.sourceDescriptorHistory->offset!=*particle.attachmentOffsetE0)return false;
        }else if(!row.descriptor04||row.descriptor04->sourceOffsetKnown!=15||
                 row.descriptor04->sourceOffset320!=*particle.attachmentOffsetE0)return false;
        if(particle.configEntered){
            if(!particle.configEmitterIndex||*particle.configEmitterIndex>=row.sourceEmitterPublishedSlots||
               !row.sourceEmitterPointers||particle.configEmitterNonNull!=bool(row.sourceEmitters[*particle.configEmitterIndex])||
               particle.configModelNonNull!=(particle.attachmentIsNone==false))return false;
        }
        if(particle.attachmentMatched){
            const auto z=*particle.attachmentZone,slot=*particle.attachmentSlot;
            if(!sourceStaticOwner_||sourceStaticOwner_->closed_||sourceModelOwner3C4_!=sourceStaticOwner_.get()||
               sourceStaticOwner_->sourceWorld3C4Borrower_!=this||z>=sourceStaticOwner_->parents_.size()||
               !sourceStaticOwner_->parents_[z]||!sourceModelParents3C4_||!sourceModels3C4_||
               *sourceModels3C4_!=sourceModelParents3C4_.get())return false;
            const auto& parent=*sourceStaticOwner_->parents_[z];
            if(sourceModelParents3C4_[z]!=&parent.entity||slot>=parent.sourceEntriesKnown||!parent.sourceEntries34||
               z>=sourceStaticOwner_->sourceWrapperLedger_.size()||slot>=sourceStaticOwner_->sourceWrapperLedger_[z].size()||
               !parent.sourceEntries34[slot]||parent.sourceEntries34[slot]!=sourceStaticOwner_->sourceWrapperLedger_[z][slot].get()||
               !sourceStaticOwner_->sourceModelCurrentNative(*parent.sourceEntries34[slot]))return false;
        }
        if((particle.toyScanNullRead||particle.completionGuardRead)&&
           (!sourceToy3C8_||*sourceToy3C8_||!preflightSourceWorldToyNative()))return false;
    }
    if(sourceEmptyInstancesReturnV0_&&(*sourceEmptyInstancesReturnV0_!=1||!sourceEmptyInstancesCounterS1_||
       *sourceEmptyInstancesCounterS1_||!sourceEmptyInstancesCountRead_||*sourceEmptyInstancesCountRead_||
       !sourceEmptyInstancesPointerRead_||*sourceEmptyInstancesPointerRead_))return false;
    if(collision.previousModelZone354&&(!sourceStandardWorldReturnV0_||
       *sourceStandardWorldReturnV0_!=1||*collision.previousModelZone354!=9))return false;
    if(collision.zone354Stored&&!collision.previousModelZone354)return false;
    const auto& modelReceiptZone354=collision.zone354Stored?collision.previousModelZone354:nativeModelZone354_;
    if(sourceStandardZoneStored_){
        if(!sourceStandardZoneS0_||*sourceStandardZoneS0_>10||!modelReceiptZone354||*modelReceiptZone354>=10||
           !sourceStandardZoneByteOffsetS1_||*sourceStandardZoneByteOffsetS1_!=0x160u*(*sourceStandardZoneS0_)||
           !sourceStandardZonePointerOffsetS2_||*sourceStandardZonePointerOffsetS2_!=4u*(*sourceStandardZoneS0_)||
           (*sourceStandardZoneS0_!=*modelReceiptZone354&&*sourceStandardZoneS0_!=*modelReceiptZone354+1u))return false;
    }else if(sourceStandardVisibilityMask_||sourceStandardVisibilityStored_)return false;
    if(sourceStandardVisibilityMask_>0x3ffu||
       (sourceStandardVisibilityMask_&(sourceStandardVisibilityMask_+1u)))return false;
    for(std::size_t z=0;z<10;++z){
        for(std::size_t collision=0;collision<2;++collision){const auto& n=sourceSearchNodes_[collision][z];
            if(!collision&&(sourceStandardVisibilityMask_&(1u<<z))){
                if(!sourceStandardZoneStored_||!sourceZones350_||sourceZonesLoaded_!=10||
                   !(sourceZones350_[z].knownFlags148&2u)||!n.zoneVisibility44||
                   *n.zoneVisibility44!=((sourceZones350_[z].flags148>>1)&1u))return false;
            }else if(n.zoneVisibility44)return false;
        }
        if(sourceStandardZoneS0_&&z<*sourceStandardZoneS0_&&!sourceStandardTypeReturned_[z])return false;
        if(modelReceiptZone354&&z>*modelReceiptZone354&&(sourceStandardCompletedCallbacks_[z]||sourceStandardTypeScanned_[z]||
           sourceStandardParentReturned_[z]||sourceStandardTypeReturned_[z]||sourceStandardDiagnosticAttempted_[z]))return false;
        const auto completed=sourceStandardCompletedCallbacks_[z],scanned=sourceStandardTypeScanned_[z];
        if(completed||scanned||sourceStandardParentReturned_[z]||sourceStandardTypeReturned_[z]||sourceStandardDiagnosticAttempted_[z]){
            if(!sourceStaticOwner_||z>=sourceStaticOwner_->parents_.size()||!sourceStaticOwner_->parents_[z])return false;
            const auto& parent=*sourceStaticOwner_->parents_[z];
            if(completed>parent.count30||scanned>parent.count30||completed>sourceStaticOwner_->sourceWrapperLedger_[z].size()||
               scanned>sourceStaticOwner_->sourceWrapperLedger_[z].size())return false;
            for(std::uint32_t i=0;i<completed;++i){const auto& w=sourceStaticOwner_->sourceWrapperLedger_[z][i];
                if(!w||!w->sourceTypeKnown)return false;
                if(w->type00==2){
                    //Physics has its own callback receipt, never Standard flags.
                    const auto& h=w->sourcePhysics;const auto& config=h.configure166400;
                    if(!sourceStaticOwner_->sourcePhysicsWrapperCurrentNative(*w)||!h.callbackEntered||
                       !config.wrapperReturnV0||*config.wrapperReturnV0!=1u||
                       config.wrapperReturnPC!=0x27b5f8u||!config.managerReturned)return false;
                }else if(w->type00!=1||!w->sourceModelCallbackEntered||
                    !w->sourceCallbackReturnV0||*w->sourceCallbackReturnV0>1)return false;
            }
            for(std::uint32_t i=0;i<scanned;++i){const auto& w=sourceStaticOwner_->sourceWrapperLedger_[z][i];
                if(!w||!w->sourceTypeKnown)return false;
                if(w->type00==2&&(!sourceStaticOwner_->sourcePhysicsWrapperCurrentNative(*w)||
                   !w->sourcePhysics.secondCallback27B600Returned))return false;
            }
            if(sourceStandardDiagnosticAttempted_[z]&&(!(sourceStandardVisibilityMask_&(1u<<z))||completed!=parent.count30))return false;
            if(sourceStandardParentReturned_[z]&&(!parent.count30||completed!=parent.count30||!sourceStandardDiagnosticAttempted_[z]))return false;
            if(sourceStandardTypeReturned_[z]&&(scanned!=parent.count30||
               (parent.count30?!sourceStandardParentReturned_[z]:!sourceStandardDiagnosticAttempted_[z])))return false;
        }
    }
    if(sourceStandardParentS3_){
        if(!sourceStaticOwner_||!sourceStaticOwner_->sourceMode_||!modelReceiptZone354||
           *modelReceiptZone354>=sourceStaticOwner_->parents_.size()||
           sourceStandardParentS3_!=sourceStaticOwner_->parents_[*modelReceiptZone354].get()||!sourceStandardVisibilityStored_)return false;
    }
    if(sourceStandardWrapperA0_){
        if(!sourceStandardParentS3_||!sourceStandardWrapperSlot_||!sourceStandardSlotS0_||!sourceStandardSlotByteOffsetS1_||
           *sourceStandardSlotByteOffsetS1_!=4u*(*sourceStandardSlotS0_)||
           !modelReceiptZone354||!sourceStaticOwner_||*modelReceiptZone354>=10)return false;
        const auto z=*modelReceiptZone354,slot=*sourceStandardWrapperSlot_;
        if(slot>=sourceStaticOwner_->sourceWrapperLedger_[z].size()||
           sourceStandardWrapperA0_!=sourceStaticOwner_->sourceWrapperLedger_[z][slot].get()||
           (*sourceStandardSlotS0_!=slot&&*sourceStandardSlotS0_!=slot+1u)||
           sourceStandardCompletedCallbacks_[z]!=*sourceStandardSlotS0_)return false;
    }
    if(sourceStandardTypeSlotS0_){
        if(!modelReceiptZone354||!sourceStandardTypeByteOffsetS1_||
           *sourceStandardTypeByteOffsetS1_!=4u*(*sourceStandardTypeSlotS0_)||
           sourceStandardTypeScanned_[*modelReceiptZone354]!=*sourceStandardTypeSlotS0_)return false;
    }
    if(sourceStandardTypeParentRead_||sourceStandardTypeCountRead_||sourceStandardTypeZoneRead_){
        if(!sourceStandardTypeSlotS0_||!sourceStandardTypeZoneRead_||!modelReceiptZone354||
           *sourceStandardTypeZoneRead_!=*modelReceiptZone354||!sourceStaticOwner_||
           *modelReceiptZone354>=sourceStaticOwner_->parents_.size()||!sourceStaticOwner_->parents_[*modelReceiptZone354])return false;
        const auto& p=*sourceStaticOwner_->parents_[*modelReceiptZone354];
        if(sourceStandardTypeParentRead_!=&p.entity||!sourceStandardTypeCountRead_||*sourceStandardTypeCountRead_!=p.count30)return false;
    }
    if(sourceStandardWorldReturnV0_){
        if(*sourceStandardWorldReturnV0_!=1||!sourceStandardZoneS0_||*sourceStandardZoneS0_!=10||sourceStandardVisibilityMask_!=0x3ff)return false;
        for(bool returned:sourceStandardTypeReturned_)if(!returned)return false;
    }
    if(sourceBuoyAttachNoEffect1B3AE0_&&
       (!sourceStandardWorldReturnV0_||sourceBuoysPhase_!=SourceBuoysPhase::returned||
        !preflightSourceWorldBuoysNative()||!nativeBuoyCount538_||*nativeBuoyCount538_!=0))return false;
    return !sourceStandardCallbackEntered_||sourceStandardWrapperA0_;
}
bool Owner::abortSourcePostParticleModelsNative(std::uint32_t& pc)noexcept {
    if(!preflightSourcePostParticleModelsNative())return false;
    if(sourcePostParticleModelsPhase_==SourcePostParticleModelsPhase::unreached||
       sourcePostParticleModelsPhase_==SourcePostParticleModelsPhase::nativeClosed)return true;
    //Later3F8 unknown-slot array leaves before the Environment owner/config.
    if(!withdrawSourceSkyArrayNative())return false;
    //Latest trigger stack/heap280890 leases leave before older observer cohorts.
    if(!withdrawSourceTriggerServiceMembershipNative())return false;
    //Later emitter74/70 raw borrows leave before older model and manager owners.
    if(!withdrawSourceParticleEmitterBorrowsNative())return false;
    //Pending trigger-service operands are native reads, not new ownership.
    //Withdraw their currentness dependence before trigger/topology retirement.
    if(sourceTriggerService274E90_.entered)sourceTriggerService274E90_.nativeWithdrawn=true;
    if(!retireSourceFinishLineNative())return false;
    //Collision callbacks were appended after Standard children; withdraw their
    //native tail custody before retiring those children or their Parent owner.
    if(worldCollisionObjects3CC_&&worldCollisionObjects3CC_->sourceMode_){
        //Source callbacks append in zone/slot order. Withdraw the newest membership
        //cohort and parent tail first, including an unfinished final callback.
        auto& rows=worldCollisionObjects3CC_->sourceWrappers_;
        for(std::size_t z=rows.size();z-->0;)for(std::size_t n=rows[z].size();n-->0;){auto& w=rows[z][n];
            if(w&&(!g::OriginalWorldCollisionObjects50LA::withdrawSourceCallbackMembershipNative(
                *w,*worldApplication_,worldEntity_.sessionGeneration)||
                !g::OriginalWorldCollisionObjects50LA::withdrawSourceCallbackParentNative(
                *w,*worldApplication_,worldEntity_.sessionGeneration)))return false;
        }
    }
    //The later callback's temporary source cohort owner must leave before
    //older Standard/default observers. Loader/outer and any unretired descriptor survive.
    if(!retireSourceStandardModelsNative(pc))return false;
    //Borrowed stack identities retire before source parent/descriptor and node
    //owners. Do not restore the process directory or erase reached44/354 stores.
    sourceStandardWrapperA0_=nullptr;sourceStandardParentS3_=nullptr;sourceStandardTypeParentRead_=nullptr;
    sourcePostParticleModelsPhase_=SourcePostParticleModelsPhase::nativeClosed;pc=sourcePostParticleModelsPC_;return true;
}
bool Owner::continueSourcePostParticleModels1AA194(std::uint32_t& pc)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourcePostParticleModelsPhase_!=SourcePostParticleModelsPhase::unreached||
       sourcePostLightPhase_!=SourcePostLightPhase::particleReturned||sourcePostLightPC_!=0x1aa194||
       !sourceParticleStageReturnV0_||!sourceFogCallReturnV0_||*sourceFogCallReturnV0_!=1||
       !sourceLoadApplication_||!sourceLoadRace_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!preflightSourceWorldTopologyCountNative()||
       !preflightSourceWorldInstanceMasterNative()||!preflightSourceWorldStaticModelsNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourceTrackPC_=sourcePostParticleModelsPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourcePostParticleModelsPhase_=SourcePostParticleModelsPhase::bounded;return false;};
    sourcePostParticleModelsPhase_=SourcePostParticleModelsPhase::entered;
    at(0x1aa198);const auto& app=*sourceLoadApplication_;
    at(0x1aa19c);const auto& loading=app.sourceLoading330Owner_;
    if(!loading)return stop(0x1aa1a0);
    //Same constructor/initializer-produced Application330, not a native screen
    //or a readiness flag. The source30 read is retained even on nonzero branch.
    if(!loading->current()||loading->issuer_!=&app||!loading->managerInitialized_||
       !loading->slot330Published_||loading->retiring_||loading->retired_)return stop(0x1aa1a0);
    at(0x1aa1a0);sourcePostParticleLoadingMode30_=loading->mode30_;
    if(*sourcePostParticleLoadingMode30_)return stop(0x1aa1b8); //Real optional1CC900 remains unjoined.
    auto& directory=app.modelDirectory36B860_;
    //Native string backing preadmission only; source copies10 bytes to a raw
    //process lane. Existing storage must hold9 characters plus its terminator.
    //No allocation failure is passed off as an original branch or partial copy.
    if(directory.size()<9)return stop(0x1aa1d8);
    at(0x1aa1d8);std::memcpy(directory.data(),"geometry",8);sourceGeometryDirectoryStoreMask_|=1;
    at(0x1aa1dc);directory[8]='\\';directory.resize(9);sourceGeometryDirectoryStoreMask_|=2;
    at(0x1aa1ec);at(0x274080);
    if(sourceInstancePhase_!=SourceInstancePhase::returned||!nativeEmptyInstances3DC_||
       !nativeEmptyInstances3DC_->sourceMode||!nativeEmptyInstances3DC_->complete||
       !nativeEmptyInstances3DC_->sourceCallerPublished||!nativeInstanceCount3E0_||!nativeInstanceDescriptors3DC_)return stop(pc);
    at(0x27409c);sourceEmptyInstancesCounterS1_=0;
    at(0x274110);sourceEmptyInstancesCountRead_=*nativeInstanceCount3E0_;
    if(*sourceEmptyInstancesCountRead_!=0)return stop(0x2740a8); //No synthetic descriptor loop/adopted manager.
    at(0x274120);sourceEmptyInstancesPointerRead_=*nativeInstanceDescriptors3DC_;
    if(*sourceEmptyInstancesPointerRead_)return stop(0x274134); //No invented array allocation/deletion.
    at(0x274154);sourceEmptyInstancesReturnV0_=1;at(0x274158);
    at(0x1aa200);at(0x274178);
    if(!sourceModels3C4_||!*sourceModels3C4_||!sourceStaticOwner_||!sourceStaticOwner_->sourceAllDescriptorsReturned_||
       !sourceModelParents3C4_||*sourceModels3C4_!=sourceModelParents3C4_.get()||sourceModelParentsPublished3C4_!=10||
       !worldZones34C_||*worldZones34C_!=10||!sourceZones350_||sourceZonesLoaded_!=10)return stop(pc);
    at(0x27418c);sourceStandardZoneS0_=0;at(0x274190);sourceStandardZoneByteOffsetS1_=0;
    at(0x274198);sourceStandardZonePointerOffsetS2_=0;
    while(*sourceStandardZoneS0_<*worldZones34C_){
        const auto z=*sourceStandardZoneS0_;if(z>=10)return stop(0x274220);
        at(0x27419c);nativeModelZone354_=z;sourceStandardZoneStored_=true;
        //Saved native argument receipts move to the new actual selected parent;
        //per-wrapper completion history remains in its distinct owned ledger.
        sourceStandardParentS3_=nullptr;sourceStandardWrapperA0_=nullptr;sourceStandardWrapperSlot_.reset();sourceStandardCallbackEntered_=false;
        sourceStandardSlotS0_.reset();sourceStandardSlotByteOffsetS1_.reset();
        sourceStandardTypeSlotS0_.reset();sourceStandardTypeByteOffsetS1_.reset();
        sourceStandardTypeZoneRead_.reset();sourceStandardTypeCountRead_.reset();sourceStandardTypeParentRead_=nullptr;
        at(0x2741ac);const auto& zone=sourceZones350_[z];if(!(zone.knownFlags148&2u))return stop(pc);
        at(0x2741b4);if(!sourceVisibility4C0_||!*sourceVisibility4C0_||!sourceSearchNodes_[0][z].owner||
           (*sourceVisibility4C0_)[z]!=sourceSearchNodes_[0][z].owner.get())return stop(pc);
        at(0x2741c0);sourceSearchNodes_[0][z].zoneVisibility44=(zone.flags148>>1)&1u;
        sourceStandardVisibilityStored_=true;sourceStandardVisibilityMask_|=static_cast<std::uint16_t>(1u<<z);
        at(0x2741cc);if(z>=sourceStaticOwner_->parents_.size())return stop(pc);const auto& parent=sourceStaticOwner_->parents_[z];
        if(!parent||sourceModelParents3C4_[z]!=&parent->entity||!parent->sourceMode||!parent->sourceCountKnown)return stop(pc);
        sourceStandardParentS3_=parent.get();at(0x2741d0);
        if(parent->count30>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return stop(pc);
        if(!parent->count30){
            at(0x2741e4);sourceStandardDiagnosticAttempted_[z]=true;
            sourceStandardDiagnosticWritten_[z]=std::fprintf(stdout,"Zone%d does not contain any static models.\n",static_cast<int>(z));
        }else{
            at(0x2741f8);at(0x27b950);sourceStandardSlotS0_=0;at(0x27b958);sourceStandardSlotByteOffsetS1_=0;
            //27B954 jumps to the live signed count test before the first
            //callback. Both Standard and Physics returns come back here;
            //never reuse an entry count captured before a callback.
            for(;;){
                at(0x27b980);const auto currentCount=parent->count30;
                if(currentCount>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return stop(pc);
                at(0x27b984);const bool another=*sourceStandardSlotS0_<currentCount;
                at(0x27b988);if(!another)break;
                const auto slot=*sourceStandardSlotS0_;at(0x27b964);
                if(!parent->sourceEntries34||slot>=parent->sourceEntriesKnown||
                   slot>=sourceStaticOwner_->sourceWrapperLedger_[z].size())return stop(pc);
                auto* wrapper=parent->sourceEntries34[slot];
                if(!wrapper||wrapper!=sourceStaticOwner_->sourceWrapperLedger_[z][slot].get())return stop(pc);
                sourceStandardWrapperA0_=wrapper;sourceStandardWrapperSlot_=slot;sourceStandardCallbackEntered_=false;
                at(0x27b968);if(!wrapper->sourceMode||!wrapper->sourceTypeKnown)return stop(pc);
                if(wrapper->type00==2){
                    if(!sourceStaticOwner_->sourcePhysicsWrapperCurrentNative(*wrapper))return stop(pc);
                    at(0x27b970);sourceStandardCallbackEntered_=true;
                    auto& h=wrapper->sourcePhysics;
                    if(h.callbackEntered||!h.descriptor0C||!h.descriptorReadReturned||
                       worldEntity_.sessionGeneration!=sourceStaticOwner_->generation_||
                       sourceStaticOwner_->application_!=worldApplication_)return stop(0x27b3d0);
                    h.callbackEntered=true;at(0x27b3e0);const auto& descriptor=h.descriptor0C->common;
                    if(!descriptor.name180Known||descriptor.name180Known>descriptor.name180.size()||
                       descriptor.name180[descriptor.name180Known-1])return stop(0x27b3f8);
                    std::size_t length=0;while(length<descriptor.name180Known&&descriptor.name180[length])++length;
                    if(length==descriptor.name180Known||length>h.setupName.size()-5)return stop(0x27b3f8);
                    //Actual%s.slt stack formatting uses name180, NOT name00.
                    at(0x27b3f8);std::memcpy(h.setupName.data(),descriptor.name180.data(),length);
                    std::memcpy(h.setupName.data()+length,".slt",5);h.setupNameLength=length;h.nameFormatted=true;
                    auto& authority=worldApplication_->entityAuthority_;
                    if(authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                       !authority.reserveAdditional(1))return stop(0x27b400);
                    //Native control owner exists before the original160 allocation.
                    h.bodyCallEntered=true;h.bodyOwner.reset(new(std::nothrow)g::OriginalSourcePhysicsObject50LA);
                    if(!h.bodyOwner)return stop(0x27b400);
                    const auto made=h.bodyOwner->allocateSource27B400(worldApplication_,worldEntity_.sessionGeneration,h.body8);
                    if(auto* entity=h.bodyOwner->countedEntityNative()){
                        if(!authority.enroll(*entity))return stop(made.sourcePC);h.bodyEnrolled=true;
                    }
                    if(made.status!=g::OriginalSourcePhysicsObject50LA::Status::reached||made.sourcePC!=0x27b420u)
                        return stop(made.sourcePC);
                    h.bodyCallReturned=true;
                    //Source reads both actual selected topology arrays AFTER
                    //wrapper8 publication, even if the receiver is nowNULL.
                    at(0x27b420);if(!nativeModelZone354_)return stop(pc);h.collisionZone=*nativeModelZone354_;
                    at(0x27b424);if(!sourceCollision4C4_||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get())return stop(pc);
                    at(0x27b42c);h.terrainZero27B42C=true; //Actual caller t0=zero, distinct from selected collision.
                    at(0x27b43c);if(!*sourceCollision4C4_||*h.collisionZone>=sourceCollisionPublished_)return stop(pc);
                    h.collisionNode=(*sourceCollision4C4_)[*h.collisionZone];h.collisionRead=true;
                    at(0x27b444);if(!nativeModelZone354_)return stop(pc);h.visibilityZone=*nativeModelZone354_;
                    at(0x27b448);if(!sourceVisibility4C0_||*sourceVisibility4C0_!=sourceVisibilitySlots4C0_.get())return stop(pc);
                    at(0x27b45c);if(!*sourceVisibility4C0_||*h.visibilityZone>=sourceVisibilityPublished_)return stop(pc);
                    h.visibilityNode=(*sourceVisibility4C0_)[*h.visibilityZone];h.visibilityRead=true;
                    //27B460/464/46C pass the SAME descriptor110/120/130;
                    //t0/t1 are zero, t2 is setupName, t3 visibility, stack collision.
                    at(0x27b468);h.setupEntered=true;
                    //Native admission precedes the genuine165C30 prefix;
                    //the source nullable body still reaches stack vector copies.
                    if(authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                       !authority.reserveAdditional(1))return stop(0x165c30);
                    h.nestedCallEntered=true;h.nestedModel.reset(new(std::nothrow)g::OriginalSourcePhysicsModel50LA);
                    if(!h.nestedModel)return stop(0x165c30);
                    const auto nested=h.nestedModel->beginSource165C30(*h.bodyOwner,descriptor.position110,
                        descriptor.look120,descriptor.up130,h.setupName.data());
                    if(auto* entity=h.nestedModel->countedModelEntityNative()){
                        if(!authority.enroll(*entity))return stop(nested.sourcePC);h.nestedModelEnrolled=true;
                    }
                    if(nested.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||nested.sourcePC!=0x165cc0u)
                        return stop(nested.sourcePC);
                    h.nestedCallReturned=true;
                    if(!h.terrainZero27B42C)return stop(0x165c50);
                    h.terrainSaved165C50=true; //Actual incomingt0→s3 save reached inside165C30.
                    if(!sourceLoadApplication_)return stop(0x165cc0);
                    h.nestedNameEntered=true;
                    const auto named=h.nestedModel->prepareSourceModelName26E848(
                        sourceLoadApplication_->modelDirectory36B860_,std::string_view(h.setupName.data(),h.setupNameLength+4));
                    if(named.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||named.sourcePC!=0x26e9acu)
                        return stop(named.sourcePC);
                    h.nestedNameReturned=true;at(0x26e9ac);
                    if(!sourceTrackMountDomain_||sourceTrackMountLease_.closed_||!sourceTrackMountDomain_->prepared()||
                       !sourceTrackFile_)return stop(pc);
                    const auto resolver=sourceTrackMountDomain_->resolver3E1A70();if(!resolver)return stop(pc);
                    h.nestedLookupEntered=true;a::OriginalStandardTextureDiagnostic50LA lookupDiagnostic;
                    const bool returned=a::OriginalSourcePsgMissingLookup50LA::capture132820(resolver,sourceTrackFile_,
                        h.nestedModel->requestedPsg_,h.missingLookup,lookupDiagnostic);
                    if(!returned&&h.missingLookup&&h.missingLookup->found_&&
                       h.missingLookup->sourcePC_==0x132988u&&h.missingLookup->preflightNative()&&
                       !h.missingLookup->nativeClosed()){
                        //Authenticate native file crosswalk AFTER the actual
                        //source search; never replay132820 or treat provider
                        //absence as an original zero result.
                        const auto foundProvider=sourceTrackMountDomain_->archiveProvider3E1A70();
                        if(!foundProvider||foundProvider!=sourceTrackMountLease_.provider_)return stop(0x132988);
                        h.foundEntered=true;h.foundProvider=foundProvider;h.foundResolver=resolver;
                        const auto present=h.foundProvider->lookup132820(h.nestedModel->requestedPsg_,h.foundMember);
                        if(present!=a::OriginalStandardArchiveProvider50LA::Lookup::present||
                           !h.foundMember.member||!h.foundMember.data||!h.foundMember.size)return stop(0x132988);
                        h.foundProviderResolved=true;a::OriginalStandardModelFileId50LA nativeId;
                        a::OriginalStandardTextureDiagnostic50LA foundDiagnostic;
                        try{std::vector<std::uint8_t> bytes(h.foundMember.data,h.foundMember.data+h.foundMember.size);
                            if(!resolver->bindModelMember(*h.foundMember.member,a::OriginalStandardModelFileKind50LA::psg,
                               bytes,nativeId,foundDiagnostic))return stop(0x132988);
                        }catch(...){return stop(0x132988);}
                        const bool nativeResolved=resolver->lookupModel132820(h.nestedModel->requestedPsg_,a::OriginalStandardModelFileKind50LA::psg,
                            h.foundNativeFile,foundDiagnostic);
                        h.foundGeometryHistory=h.foundNativeFile.geometry240110.get();
                        h.foundNativeFile.geometry240110.reset(); //No native strong resource adoption/retain, even on refusal.
                        if(!nativeResolved||h.foundNativeFile.id.resolver!=resolver.get()||
                           h.foundNativeFile.id.row!=nativeId.row||!h.foundNativeFile.bytes||
                           h.foundNativeFile.bytes->size()!=h.foundMember.size||
                           std::memcmp(h.foundNativeFile.bytes->data(),h.foundMember.data,h.foundMember.size))return stop(0x132988);
                        h.foundNativeResolved=true;h.foundCaptureEntered=true;
                        const bool opened=a::OriginalSourcePsgFile50LA::continueFound132988(h.missingLookup,
                            h.foundNativeFile.id,h.foundFile,foundDiagnostic);
                        if(!opened)return stop(foundDiagnostic.sourcePC?foundDiagnostic.sourcePC:0x132988u);
                        //Only genuinely cold output is currently issued here.
                        //AssociatedObject1C without exact warm creator remains
                        //the helper's bounded1329A0 path, pendingfile retained.
                        if(!h.foundFile||!h.foundFile->returnedCold()||!h.foundFile->current())return stop(0x1329e8);
                        h.foundHandle678=h.foundFile->sourceHandle();h.foundResource684=nullptr;
                        h.foundCaptureReturned=true;
                        at(0x26ebcc);
                        //Native capacity admission only: all actual source
                        //row constructors/read effects remain in28CAC0.
                        if(authority.enrollmentSerial_>std::numeric_limits<std::uint64_t>::max()-(a::maxModelNodes49O-1)||
                           !authority.reserveAdditional(a::maxModelNodes49O-1))return stop(pc);
                        h.nestedRowsEntered=true;
                        const auto imported=h.nestedModel->importSourceRows28CAC0(h.foundResolver,h.foundNativeFile,h.foundFile);
                        //A constructor may count before source publication or
                        //type-registry refusal. Enrol every real child BEFORE
                        //inspecting returned/bounded status; Model0 is separate.
                        for(std::uint32_t row=1;row<a::maxModelNodes49O;++row){
                            auto* entity=h.nestedModel->countedChildEntityNative(row);if(!entity)continue;
                            if(h.nestedRowEnrolled[row]||!authority.enroll(*entity))return stop(imported.sourcePC);
                            h.nestedRowEnrolled[row]=entity;
                        }
                        if(imported.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||imported.sourcePC!=0x28cd5cu||
                           !h.nestedModel->rowsReturned_||!h.nestedModel->rowsReturnV0_)return stop(imported.sourcePC);
                        h.nestedRowsReturned=true;h.nestedRowsReturnV0=h.nestedModel->rowsReturnV0_;
                        at(0x26ed4c);at(0x26ed50);
                        if(!h.nestedModel->model_||(h.nestedModel->model_->fields_.flags1B3&4u))return stop(pc);
                        at(0x26ee18);
                        const auto& materialDomain=sourceLoadApplication_->standardMaterialDomainOwner_;
                        if(!materialDomain||materialDomain!=sourceTrackMountDomain_||!materialDomain->prepared()||
                           !sourceLoadApplication_->materialApplication)return stop(pc);
                        const auto materialTable=materialDomain->materials382B38();
                        if(!materialTable||!materialTable->sourceNames17CAD4Current()||
                           !materialTable->screenApplicationMatches(*sourceLoadApplication_->materialApplication))return stop(pc);
                        const auto* materialPrefix=&sourceLoadApplication_->materialDirectory36B970();
                        a::OriginalStandardMaterialDiagnostic50LA materialDiagnostic;
                        h.nestedMaterialEntered=true;at(0x26ee1c);
                        const auto materials=h.nestedModel->loadSourceMaterials26EE18(materialTable,materialPrefix,materialDiagnostic);
                        if(materials.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||materials.sourcePC!=0x26e810u||
                           !h.nestedModel->materialReturned_)return stop(materials.sourcePC);
                        h.nestedMaterialReturned=true;at(0x26ee24);
                        h.nestedQuantizationEntered=true;at(0x26ee30);
                        const auto quantized=h.nestedModel->loadSourceQuantization26EE30();
                        if(quantized.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||quantized.sourcePC!=0x2406dcu||
                           !h.nestedModel->quantizationReturned_)return stop(quantized.sourcePC);
                        //Original caller ignores V0; actual reached-return is
                        //separate from any native helper success convention.
                        h.nestedQuantizationReturned=true;
                        at(0x26ee38);h.nestedGeometryEntered=true;
                        const auto geometry=h.nestedModel->allocateSourceGeometry26EE38();
                        if(geometry.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||geometry.sourcePC!=0x26ee64u||
                           !h.nestedModel->geometryAllocated_)return stop(geometry.sourcePC);
                        h.nestedGeometryAllocated=true;at(0x26ee64);h.nestedGeometryLoadEntered=true;
                        const auto lods=h.nestedModel->loadSourceGeometry26E170();
                        if(lods.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||lods.sourcePC!=0x24024cu||
                           !h.nestedModel->geometryLodsReturned_)return stop(lods.sourcePC);
                        h.nestedGeometryLodsReturned=true;at(0x24024c);h.nestedGeometryPaletteEntered=true;
                        const auto palette=h.nestedModel->finishSourceGeometry240210();
                        if(palette.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||palette.sourcePC!=0x240394u||
                           !h.nestedModel->geometryReturned_)return stop(palette.sourcePC);
                        h.nestedGeometryReturned=true;
                        at(0x26ee6c);h.nestedPrototypeEntered=true;
                        const auto prototype=h.nestedModel->publishSourceGeometryPrototype26EE74();
                        if(prototype.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||prototype.sourcePC!=0x26ee78u||
                           !h.nestedModel->prototypeReturned_)return stop(prototype.sourcePC);
                        h.nestedPrototypeReturned=true;
                        const auto& geometryOwner=h.nestedModel->model_->sourceGeometry_.owner;
                        a::OriginalStandardTextureDiagnostic50LA cacheDiagnostic;
                        at(0x26ee80);h.nestedCacheEntered=true;
                        if(!h.foundFile->associateGeometry1326A0(geometryOwner,h.nestedModel->model_.get(),cacheDiagnostic))
                            return stop(cacheDiagnostic.sourcePC?cacheDiagnostic.sourcePC:pc);
                        if(!h.foundFile->sourceAssociationStored()||!h.foundFile->nativeCacheAssociated()||
                           !h.foundFile->associatedGeometryCurrent(geometryOwner.get(),h.nestedModel->model_.get()))return stop(pc);
                        h.nestedCacheReturned=true;at(0x26ee88);
                        at(0x26ee90);h.nestedCloseEntered=true;
                        if(!h.foundFile->closeSource131040(cacheDiagnostic))
                            return stop(cacheDiagnostic.sourcePC?cacheDiagnostic.sourcePC:pc);
                        if(!h.foundFile->sourceCloseReturned())return stop(pc);
                        //Original archive131040 returns1 (delay1311C0), but
                        //26E820 ignores V0; native cancellation never mints it.
                        h.nestedCloseReturned=true;
                        at(0x26ee98);h.nestedRowTableEntered=true;
                        const auto rowTable=h.nestedModel->allocateClosedSourceRowTable26EE9C();
                        if(rowTable.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||rowTable.sourcePC!=0x26eee8u||
                           !h.nestedModel->rowTableReturned_||!h.nestedModel->model_||
                           !h.nestedModel->model_->preflightSourceRenderableReturn26E820Native())return stop(rowTable.sourcePC);
                        const auto* returnedEntity=h.nestedModel->model_->sourceRenderableEntity26E820Native();
                        if(!returnedEntity||returnedEntity!=h.nestedModel->renderableReturnedEntity_||
                           returnedEntity!=h.nestedModel->model_->wreckModelStorage())return stop(rowTable.sourcePC);
                        h.nestedRowTableReturned=true;h.nestedRenderableEntity=returnedEntity;
                        //Actual24075C sees nonNULL26E820 Entity+8; packet/flags
                        //suffix and final240740 return are distinct dependencies.
                        at(0x24076c);h.nestedPacketsEntered=true;
                        //Actual165CD8 delay slot passes raw a3=0 to240740;
                        //neither constructor a1=1 nor Standard dynamic144 applies.
                        const auto packets=h.nestedModel->constructSourcePackets240740();
                        if(packets.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||packets.sourcePC!=0x2407d8u||
                           !h.nestedModel->packetsReturned_||!h.nestedModel->loaderReturnedEntity_||
                           h.nestedModel->loaderReturnedEntity_!=h.nestedRenderableEntity)return stop(packets.sourcePC);
                        h.nestedPacketsReturned=true;h.nestedLoaderEntity=h.nestedModel->loaderReturnedEntity_;
                        h.nestedLoaderReturned=true;
                        //Actual165C30 ignores typed240740 V0. Only this
                        //genuinely live Model path may enter original141960;
                        //source-missing dead14C remains separately bounded.
                        at(0x165cdc);
                        if(!h.bodyOwner||!h.bodyEnrolled||!h.nestedModelEnrolled||
                           !h.bodyOwner->countedEntityNative()||!h.nestedModel->countedModelEntityNative()||
                           !authority.canRetire(*h.bodyOwner->countedEntityNative())||
                           !authority.canRetire(*h.nestedModel->countedModelEntityNative()))return stop(pc);
                        const auto attach=[&](g::OriginalWreckComponent50LA& parent,g::OriginalWreckComponent50LA& child,
                            std::uint32_t mask,std::uint32_t lock)noexcept {
                            return authority.attachChildren(parent,child,mask,lock);
                        };
                        h.nestedAttachEntered=true;at(0x165cf0);
                        const auto attached=h.nestedModel->attachSourceModel141960(attach);
                        if(attached.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||attached.sourcePC!=0x165cf8u||
                           !h.nestedModel->attachReturned_)return stop(attached.sourcePC);
                        h.nestedAttachReturned=true;
                        at(0x165cf8);h.nestedPlacementEntered=true;
                        const auto placed=h.nestedModel->placeSourceModel165CF8();
                        if(placed.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||placed.sourcePC!=0x165d94u||
                           !h.nestedModel->placementReturned_)return stop(placed.sourcePC);
                        h.nestedPlacementReturned=true;
                        at(0x165d94);
                        //Retained original165C30 arguments feed165E90:
                        //t0=actual selected visibility, t1=selected collision.
                        if(!h.visibilityRead||!h.collisionRead||!h.bodyOwner||!h.bodyOwner->object_||
                           !h.nestedModel->placementReturned_||h.sourceBodyD0||
                           authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                           !authority.reserveAdditional(1))return stop(pc);
                        h.setup165E90Entered=true;
                        try{h.sourceBodyD0=std::make_unique<g::OriginalSourcePhysicsBody50LA>();}
                        catch(...){return stop(0x165e90);}
                        if(!h.terrainSaved165C50)return stop(0x165d98);
                        at(0x165d98);h.terrainForwarded165D98=true; //s3→a1, not B4/B8.
                        at(0x165eac);h.bodyOwner->sourceTerrain165E90_=nullptr;h.terrainSaved165EAC=true; //a1→s1.
                        at(0x165ebc);h.setupNoOpReturned=true; //Actual141C20 no-op, no retain.
                        at(0x165ec8);h.bodyOwner->object_->visibilityB4=h.visibilityNode;h.visibilityB4Stored=true;
                        at(0x165ed0);h.bodyOwner->object_->collisionB8=h.collisionNode;h.collisionB8Stored=true;
                        //B8 is the allocation-call delay slot, before nullable330.
                        h.sourceBodyCallEntered=true;
                        const auto body=h.sourceBodyD0->allocateSource165ECC(*h.bodyOwner);
                        if(auto* entity=h.sourceBodyD0->countedEntityNative()){
                            if(!authority.enroll(*entity))return stop(body.sourcePC);h.sourceBodyEnrolled=true;
                        }
                        if(body.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||body.sourcePC!=0x165eecu||
                           !h.sourceBodyD0->published_)return stop(body.sourcePC);
                        h.sourceBodyCallReturned=true;h.sourceBodyConfigEntered=true;at(0x165ef0);
                        const auto configured=h.sourceBodyD0->configureSource165EF8(*h.nestedModel);
                        if(configured.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||configured.sourcePC!=0x165f34u||
                           !h.sourceBodyD0->disableReturned_)return stop(configured.sourcePC);
                        h.sourceBodyConfigReturned=true;
                        at(0x165f34);h.sourceBodyAttachEntered=true;
                        const auto bodyAttached=h.sourceBodyD0->attachSourceBody141960(attach);
                        if(bodyAttached.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||bodyAttached.sourcePC!=0x165f44u||
                           !h.sourceBodyD0->attachReturned_)return stop(bodyAttached.sourcePC);
                        h.sourceBodyAttachReturned=true;at(0x165f44);
                        if(h.sourceGravityD4||authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                           !authority.reserveAdditional(1))return stop(pc);
                        try{h.sourceGravityD4=std::make_unique<g::OriginalSourcePhysicsGravity50LA>();}catch(...){return stop(pc);}
                        h.sourceGravityCallEntered=true;
                        const auto gravity=h.sourceGravityD4->allocateSource165F44(*h.bodyOwner,*h.sourceBodyD0);
                        if(auto* entity=h.sourceGravityD4->countedEntityNative()){
                            if(!authority.enroll(*entity))return stop(gravity.sourcePC);h.sourceGravityEnrolled=true;
                        }
                        if(gravity.status!=g::OriginalSourcePhysicsGravity50LA::Status::reached||gravity.sourcePC!=0x165f64u||
                           !h.sourceGravityD4->published_)return stop(gravity.sourcePC);
                        h.sourceGravityCallReturned=true;at(0x165f64);h.sourceGravityTargetEntered=true;
                        const auto target=h.sourceGravityD4->configureTargetSource165F70(*h.sourceBodyD0);
                        if(target.status!=g::OriginalSourcePhysicsGravity50LA::Status::reached||target.sourcePC!=0x165f8cu||
                           !h.sourceGravityD4->targetReturned_)return stop(target.sourcePC);
                        h.sourceGravityTargetReturned=true;at(0x165f8c);h.sourceGravityAttachEntered=true;
                        const auto gravityAttached=h.sourceGravityD4->attachSourceGravity141960(attach);
                        if(gravityAttached.status!=g::OriginalSourcePhysicsGravity50LA::Status::reached||gravityAttached.sourcePC!=0x165f9cu||
                           !h.sourceGravityD4->attachReturned_)return stop(gravityAttached.sourcePC);
                        h.sourceGravityAttachReturned=true;at(0x165f9c);h.collisionNameEntered=true;
                        //Actual distinct stack+60 scratch. Native256 capacity
                        //is an adapter bound; original strcpy is unbounded.
                        const auto setupLength=h.setupNameLength+4u;
                        if(setupLength>=h.collisionSetupName.size()||h.setupName[setupLength])return stop(0x165fa0);
                        std::memcpy(h.collisionSetupName.data(),h.setupName.data(),setupLength+1);
                        h.collisionSetupNameLength=setupLength;h.collisionNameCopied=true;
                        for(std::size_t i=setupLength;i>0;--i)if(h.collisionSetupName[i-1]=='.'){
                            h.collisionSetupDot=i-1;break;}
                        h.collisionDotSearched=true;at(0x166010);
                        if(!h.collisionSetupDot)return stop(pc); //Original NULLstrcpy destination, not append fallback.
                        if(*h.collisionSetupDot+sizeof("m.col")>h.collisionSetupName.size())return stop(pc);
                        std::memcpy(h.collisionSetupName.data()+*h.collisionSetupDot,"m.col",sizeof("m.col"));
                        h.collisionSetupNameLength=*h.collisionSetupDot+5u;h.collisionNameReturned=true;at(0x166018);
                        if(h.sourceCollisionE0||authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                           !authority.reserveAdditional(1))return stop(pc);
                        try{h.sourceCollisionE0=std::make_unique<g::OriginalSourcePhysicsCollision50LA>();}catch(...){return stop(pc);}
                        h.sourceCollisionCallEntered=true;
                        const auto collision=h.sourceCollisionE0->allocateSource166018(*h.bodyOwner,*h.sourceGravityD4);
                        if(auto* entity=h.sourceCollisionE0->countedEntityNative()){
                            if(!authority.enroll(*entity))return stop(collision.sourcePC);h.sourceCollisionEnrolled=true;
                        }
                        if(collision.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||collision.sourcePC!=0x166038u||
                           !h.sourceCollisionE0->published_)return stop(collision.sourcePC);
                        h.sourceCollisionCallReturned=true;at(0x16603c);h.sourceCollisionConfigEntered=true;
                        const auto collisionConfig=h.sourceCollisionE0->configureSource16603C(*h.sourceBodyD0);
                        if(collisionConfig.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||collisionConfig.sourcePC!=0x166074u||
                           !h.sourceCollisionE0->modeReturned_)return stop(collisionConfig.sourcePC);
                        h.sourceCollisionConfigReturned=true;at(0x166074);h.sourceCollisionNameEntered=true;
                        const auto collisionName=h.sourceCollisionE0->prepareSourcePath11B920(
                            std::string_view(h.collisionSetupName.data(),h.collisionSetupNameLength));
                        if(collisionName.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||collisionName.sourcePC!=0x11ba2cu||
                           !h.sourceCollisionE0->nameReturned_)return stop(collisionName.sourcePC);
                        h.sourceCollisionNameReturned=true;at(0x11ba2c);
                        if(!sourceTrackMountDomain_||sourceTrackMountLease_.closed_||!sourceTrackFile_||
                           sourceTrackMountDomain_!=sourceLoadApplication_->standardMaterialDomainOwner_||
                           sourceTrackMountDomain_->resolver3E1A70()!=resolver)return stop(pc);
                        h.sourceCollisionSearchEntered=true;
                        a::OriginalStandardTextureDiagnostic50LA collisionDiagnostic;
                        const bool collisionMissing=a::OriginalSourcePsgMissingLookup50LA::capture132820(resolver,sourceTrackFile_,
                            h.sourceCollisionE0->lookupName_,h.collisionSearch,collisionDiagnostic);
                        if(!h.collisionSearch)return stop(collisionDiagnostic.sourcePC?collisionDiagnostic.sourcePC:pc);
                        //The retained generic search has no PSG file/open
                        //authority. COL pins it exclusively even on found/refusal.
                        const auto accepted=h.sourceCollisionE0->acceptSourceSearch11BA2C(h.collisionSearch,resolver.get(),sourceTrackFile_.get());
                        h.sourceCollisionSearchAccepted=h.sourceCollisionE0->searchAccepted_;
                        if(accepted.sourcePC==0x132988u&&h.sourceCollisionSearchAccepted&&h.collisionSearch->found_){
                            //Consume this SAME retained found occurrence; no
                            //replayed lookup or PSG/native-cache substitution.
                            h.sourceCollisionFoundEntered=true;
                            const auto found=h.sourceCollisionE0->continueSourceFound11BA34();
                            h.sourceCollisionFoundHeaderReturned=h.sourceCollisionE0->foundB0Stored_&&
                                h.sourceCollisionE0->headerValid_&&h.sourceCollisionE0->foundOutputReturned_&&found.sourcePC==0x124f80u;
                            if(!h.sourceCollisionFoundHeaderReturned)return stop(found.sourcePC);
                            at(0x124f80);h.sourceCollisionType0Entered=true;
                            const auto type0=h.sourceCollisionE0->continueSourceType0Prefix124F80();
                            if(type0.sourcePC!=0x125ab4u&&type0.sourcePC!=0x125ba0u)return stop(type0.sourcePC);
                            at(type0.sourcePC);h.sourceCollisionArraysEntered=true;
                            const auto arrays=h.sourceCollisionE0->continueSourceType0Arrays125AB4();
                            if(arrays.sourcePC!=0x12602cu&&arrays.sourcePC!=0x126030u)return stop(arrays.sourcePC);
                            at(arrays.sourcePC);h.sourceCollisionNodesEntered=true;
                            const auto nodes=h.sourceCollisionE0->continueSourceType0NodesFaces12602C();
                            if(nodes.status!=rr::game::OriginalSourcePhysicsCollision50LA::Status::reached||
                               nodes.sourcePC!=0x126410u)return stop(nodes.sourcePC);
                            h.sourceCollisionImportReturned=true;
                            at(0x125014);h.sourceCollisionResourceEntered=true;
                            const auto resource=h.sourceCollisionE0->continueSourceType0Resource125014();
                            if(resource.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               resource.sourcePC!=0x125488u||!h.sourceCollisionE0->loader124F80Returned_||
                               h.sourceCollisionE0->loader124F80ReturnV0_!=1)return stop(resource.sourcePC);
                            h.sourceCollisionPayloadReturned=true;at(0x11bb9c);h.sourceCollisionCloseEntered=true;
                            const auto close=h.sourceCollisionE0->closeSourceFound11BB9C();
                            if(close.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               close.sourcePC!=0x11bbbcu||!h.sourceCollisionE0->foundCloseReturned_||
                               !h.sourceCollisionE0->loader11B920Returned_||h.sourceCollisionE0->loader11B920ReturnV0_!=1)
                                return stop(close.sourcePC);
                            h.sourceCollisionLoaderReturned=true;
                            at(0x11bbe0);h.sourceCollisionResultEntered=true;
                            //Actual166078 supplies rawA3zero;11BBC0 ignores loaderV0.
                            const auto collisionResult=h.sourceCollisionE0->continueSourceResult11BBE0(0u);
                            if(collisionResult.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               collisionResult.sourcePC!=0x11bd30u)return stop(collisionResult.sourcePC);
                            h.sourceCollisionResultReturned=true;
                            at(0x16607c);h.sourceCollisionTransformEntered=true;
                            const auto transformed=h.sourceCollisionE0->continueSourceTransform11F570();
                            if(transformed.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               transformed.sourcePC!=0x11f678u||!h.sourceCollisionE0->transformReturned_)
                                return stop(transformed.sourcePC);
                            h.sourceCollisionTransformReturned=true;
                            at(0x166084);
                            if(!h.sourceCollisionEnrolled||!h.sourceCollisionE0->countedEntityNative()||
                               !authority.canRetire(*h.sourceCollisionE0->countedEntityNative())||
                               !h.bodyOwner->object_||!authority.canRetire(h.bodyOwner->object_->entity))return stop(pc);
                            h.sourceCollisionAttachEntered=true;at(0x166098);
                            const auto collisionAttached=h.sourceCollisionE0->attachSourceCollision141960(attach);
                            if(collisionAttached.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               collisionAttached.sourcePC!=0x1660a0u||!h.sourceCollisionE0->attachReturned_)
                                return stop(collisionAttached.sourcePC);
                            h.sourceCollisionAttachReturned=true;
                            at(0x1660a0);
                            if(h.sourceContactDC||authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                               !authority.reserveAdditional(1))return stop(pc);
                            try{h.sourceContactDC=std::make_unique<g::OriginalSourcePhysicsContact50LA>();}catch(...){return stop(pc);}
                            h.sourceContactCallEntered=true;
                            const auto contact=h.sourceContactDC->allocateSource1660A0(*h.bodyOwner,*h.sourceBodyD0,*h.sourceCollisionE0);
                            //Enroll every actual counted prefix before inspecting native/source outcome.
                            if(auto* entity=h.sourceContactDC->countedEntityNative()){
                                if(!authority.enroll(*entity))return stop(contact.sourcePC);h.sourceContactEnrolled=true;
                            }
                            if(contact.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                               contact.sourcePC!=0x1660c0u||!h.sourceContactDC->published_)return stop(contact.sourcePC);
                            h.sourceContactCallReturned=true;at(0x1660c4);h.sourceContactCapacityEntered=true;
                            const auto capacity=h.sourceContactDC->configureCapacitySource295C20();
                            if(capacity.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                               capacity.sourcePC!=0x1660ccu||!h.sourceContactDC->capacityReturned_)return stop(capacity.sourcePC);
                            h.sourceContactCapacityReturned=true;at(0x1660d0);h.sourceContactBodyEntered=true;
                            const auto body30=h.sourceContactDC->bindBodySource295C80(*h.sourceBodyD0);
                            if(body30.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                               body30.sourcePC!=0x1660d8u||!h.sourceContactDC->bodyStored_)return stop(body30.sourcePC);
                            h.sourceContactBodyReturned=true;
                            at(0x1660d8);
                            if(!h.bodyOwner->object_||!h.bodyOwner->object_->modelKnown||
                               !h.bodyOwner->object_->model14C||!h.sourceContactEnrolled||
                               !h.sourceContactDC->countedEntityNative()||
                               !authority.canRetire(*h.sourceContactDC->countedEntityNative()))return stop(pc);
                            //Actual14C ModelBASE and E0 outer remain distinct from embedded Entity aliases.
                            const auto actualModel=h.bodyOwner->object_->model14C;
                            h.sourceContactPointsEntered=true;at(0x1660e4);
                            const auto points=h.sourceContactDC->selectSourcePoints296190(*h.sourceCollisionE0,1u,actualModel);
                            if(points.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                               points.sourcePC!=0x2963d4u||!h.sourceContactDC->sourcePoints_.returned)return stop(points.sourcePC);
                            h.sourceContactPointsReturned=true;
                            at(0x1660ec);h.sourceContactScalarsEntered=true;
                            const auto scalars=h.sourceContactDC->continueSourceScalars1660EC();
                            if(scalars.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                               scalars.sourcePC!=0x166160u||!h.sourceContactDC->suffixReturned_)return stop(scalars.sourcePC);
                            h.sourceContactScalarsReturned=true;at(0x166160);
                            if(!h.sourceContactEnrolled||!h.sourceContactDC->countedEntityNative()||
                               !authority.canRetire(*h.sourceContactDC->countedEntityNative())||
                               !h.bodyOwner->object_||!authority.canRetire(h.bodyOwner->object_->entity))return stop(pc);
                            h.sourceContactAttachEntered=true;at(0x166164);
                            const auto contactAttached=h.sourceContactDC->attachSourceContact141960(attach);
                            if(contactAttached.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                               contactAttached.sourcePC!=0x16616cu||!h.sourceContactDC->attachReturned_)return stop(contactAttached.sourcePC);
                            h.sourceContactAttachReturned=true;
                            at(0x16616c);
                            if(!h.terrainSaved165EAC||h.sourceDetectorD8||
                               authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                               !authority.reserveAdditional(1))return stop(pc);
                            try{h.sourceDetectorD8=std::make_unique<g::OriginalSourcePhysicsDetector50LA>();}catch(...){return stop(pc);}
                            h.sourceDetectorCallEntered=true;
                            const auto detector=h.sourceDetectorD8->allocateSource16616C(*h.bodyOwner,*h.sourceContactDC);
                            if(auto* entity=h.sourceDetectorD8->countedEntityNative()){
                                if(!authority.enroll(*entity))return stop(detector.sourcePC);h.sourceDetectorEnrolled=true;
                            }
                            if(detector.status!=g::OriginalSourcePhysicsDetector50LA::Status::reached||
                               detector.sourcePC!=0x16618cu||!h.sourceDetectorD8->published_)return stop(detector.sourcePC);
                            h.sourceDetectorCallReturned=true;at(0x1661a8);h.sourceDetectorConfigEntered=true;
                            //Original s1→a2 is the retained knownNULL above; a3/t0/t2 are actualzero selectors.
                            const auto detectorConfig=h.sourceDetectorD8->configureSource2953A0();
                            if(detectorConfig.status!=g::OriginalSourcePhysicsDetector50LA::Status::reached||
                               detectorConfig.sourcePC!=0x1661b0u||!h.sourceDetectorD8->configureReturned_)return stop(detectorConfig.sourcePC);
                            h.sourceDetectorConfigReturned=true;
                            at(0x1661b8);h.sourceDetectorEnableEntered=true;
                            const auto enabled=h.sourceDetectorD8->enableSource1661B8();
                            if(enabled.status!=g::OriginalSourcePhysicsDetector50LA::Status::reached||
                               enabled.sourcePC!=0x1661bcu||!h.sourceDetectorD8->enabled64Stored_)return stop(enabled.sourcePC);
                            h.sourceDetectorEnableReturned=true;at(0x1661c0);h.sourceDetectorBindingEntered=true;
                            const auto binding=h.sourceDetectorD8->bindCollisionSource2955F0(*h.sourceCollisionE0);
                            if(binding.status!=g::OriginalSourcePhysicsDetector50LA::Status::reached||
                               binding.sourcePC!=0x1661c8u||!h.sourceDetectorD8->bindingReturned_)return stop(binding.sourcePC);
                            h.sourceDetectorBindingReturned=true;at(0x1661c8);
                            if(!h.sourceDetectorEnrolled||!h.sourceDetectorD8->countedEntityNative()||
                               !authority.canRetire(*h.sourceDetectorD8->countedEntityNative())||
                               !h.bodyOwner->object_||!authority.canRetire(h.bodyOwner->object_->entity))return stop(pc);
                            h.sourceDetectorAttachEntered=true;at(0x1661d0);
                            const auto detectorAttached=h.sourceDetectorD8->attachSourceDetector141960(attach);
                            if(detectorAttached.status!=g::OriginalSourcePhysicsDetector50LA::Status::reached||
                               detectorAttached.sourcePC!=0x1661d8u||!h.sourceDetectorD8->attachReturned_)return stop(detectorAttached.sourcePC);
                            h.sourceDetectorAttachReturned=true;
                            at(0x1661d8);h.directCollisionNameEntered=true;
                            //New originalSP+170 occurrence snapshots the saved setupName,
                            //not E0'sm.col scratch or the nested rewrittenPSG name.
                            const auto directSetupLength=h.setupNameLength+4u;
                            if(directSetupLength>=h.directCollisionSetupName.size()||h.setupName[directSetupLength])return stop(0x1661dc);
                            std::memcpy(h.directCollisionSetupName.data(),h.setupName.data(),directSetupLength+1);
                            h.directCollisionSetupNameLength=directSetupLength;h.directCollisionNameCopied=true;
                            for(std::size_t i=directSetupLength;i>0;--i)if(h.directCollisionSetupName[i-1]=='.'){
                                h.directCollisionSetupDot=i-1;break;}
                            h.directCollisionDotSearched=true;at(0x166248);
                            if(!h.directCollisionSetupDot)return stop(pc); //Actual NULL strcpy destination, no append fallback.
                            if(*h.directCollisionSetupDot+sizeof("f.col")>h.directCollisionSetupName.size())return stop(pc);
                            std::memcpy(h.directCollisionSetupName.data()+*h.directCollisionSetupDot,"f.col",sizeof("f.col"));
                            h.directCollisionSetupNameLength=*h.directCollisionSetupDot+5u;h.directCollisionNameReturned=true;at(0x166250);
                            if(h.sourceCollisionE4||authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                               !authority.reserveAdditional(1))return stop(pc);
                            try{h.sourceCollisionE4=std::make_unique<g::OriginalSourcePhysicsCollision50LA>();}catch(...){return stop(pc);}
                            h.sourceDirectCollisionCallEntered=true;
                            const auto direct=h.sourceCollisionE4->allocateSource166250(*h.bodyOwner,*h.sourceDetectorD8);
                            if(auto* entity=h.sourceCollisionE4->countedEntityNative()){
                                if(!authority.enroll(*entity))return stop(direct.sourcePC);h.sourceDirectCollisionEnrolled=true;
                            }
                            if(direct.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               direct.sourcePC!=0x166270u||!h.sourceCollisionE4->published_)return stop(direct.sourcePC);
                            h.sourceDirectCollisionCallReturned=true;at(0x166274);h.sourceDirectCollisionConfigEntered=true;
                            const auto directConfig=h.sourceCollisionE4->configureSource166274();
                            if(directConfig.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               directConfig.sourcePC!=0x16628cu||!h.sourceCollisionE4->modeReturned_)return stop(directConfig.sourcePC);
                            h.sourceDirectCollisionConfigReturned=true;at(0x16628c);
                            if(!h.bodyOwner->object_||!h.bodyOwner->object_->modelKnown||!h.bodyOwner->object_->model14C)return stop(pc);
                            h.sourceDirectCollisionPathEntered=true;at(0x166294);
                            const auto directPath=h.sourceCollisionE4->prepareSourceDirectPath11B920(*h.sourceBodyD0,
                                h.bodyOwner->object_->model14C,std::string_view(h.directCollisionSetupName.data(),h.directCollisionSetupNameLength));
                            if(directPath.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               directPath.sourcePC!=0x11ba2cu||!h.sourceCollisionE4->nameReturned_)return stop(directPath.sourcePC);
                            h.sourceDirectCollisionPathReturned=true;at(0x11ba2c);
                            if(!sourceTrackMountDomain_||sourceTrackMountLease_.closed_||!sourceTrackFile_||
                               sourceTrackMountDomain_!=sourceLoadApplication_->standardMaterialDomainOwner_||
                               sourceTrackMountDomain_->resolver3E1A70()!=resolver)return stop(pc);
                            h.sourceDirectCollisionSearchEntered=true;
                            a::OriginalStandardTextureDiagnostic50LA directDiagnostic;
                            const bool directMissing=a::OriginalSourcePsgMissingLookup50LA::capture132820(resolver,sourceTrackFile_,
                                h.sourceCollisionE4->lookupName_,h.directCollisionSearch,directDiagnostic);
                            if(!h.directCollisionSearch)return stop(directDiagnostic.sourcePC?directDiagnostic.sourcePC:pc);
                            const auto directAccepted=h.sourceCollisionE4->acceptSourceSearch11BA2C(h.directCollisionSearch,resolver.get(),sourceTrackFile_.get());
                            h.sourceDirectCollisionSearchAccepted=h.sourceCollisionE4->searchAccepted_;
                            if(directAccepted.sourcePC==0x132988u&&h.sourceDirectCollisionSearchAccepted&&h.directCollisionSearch->found_){
                                //Consume the SAME retained E4 found occurrence, never E0's file or cache.
                                h.sourceDirectCollisionFoundEntered=true;
                                const auto directFound=h.sourceCollisionE4->continueSourceFound11BA34();
                                h.sourceDirectCollisionHeaderReturned=h.sourceCollisionE4->foundB0Stored_&&
                                    h.sourceCollisionE4->headerValid_&&h.sourceCollisionE4->foundOutputReturned_&&directFound.sourcePC==0x124f80u;
                                if(!h.sourceDirectCollisionHeaderReturned)return stop(directFound.sourcePC);
                                at(0x124f80);h.sourceDirectCollisionType0Entered=true;
                                const auto directType0=h.sourceCollisionE4->continueSourceType0Prefix124F80();
                                if(directType0.sourcePC!=0x125ab4u&&directType0.sourcePC!=0x125ba0u)return stop(directType0.sourcePC);
                                at(directType0.sourcePC);h.sourceDirectCollisionArraysEntered=true;
                                const auto directArrays=h.sourceCollisionE4->continueSourceType0Arrays125AB4();
                                if(directArrays.sourcePC!=0x12602cu&&directArrays.sourcePC!=0x126030u)return stop(directArrays.sourcePC);
                                at(directArrays.sourcePC);h.sourceDirectCollisionNodesEntered=true;
                                const auto directNodes=h.sourceCollisionE4->continueSourceType0NodesFaces12602C();
                                if(directNodes.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||directNodes.sourcePC!=0x126410u)
                                    return stop(directNodes.sourcePC);
                                h.sourceDirectCollisionImportReturned=true;at(0x125014);h.sourceDirectCollisionResourceEntered=true;
                                const auto directResource=h.sourceCollisionE4->continueSourceType0Resource125014();
                                if(directResource.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||directResource.sourcePC!=0x125488u||
                                   !h.sourceCollisionE4->loader124F80Returned_||h.sourceCollisionE4->loader124F80ReturnV0_!=1)
                                    return stop(directResource.sourcePC);
                                h.sourceDirectCollisionPayloadReturned=true;at(0x11bb9c);h.sourceDirectCollisionCloseEntered=true;
                                const auto directClose=h.sourceCollisionE4->closeSourceFound11BB9C();
                                if(directClose.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||directClose.sourcePC!=0x11bbbcu||
                                   !h.sourceCollisionE4->foundCloseReturned_||!h.sourceCollisionE4->loader11B920Returned_||
                                   h.sourceCollisionE4->loader11B920ReturnV0_!=1)return stop(directClose.sourcePC);
                                h.sourceDirectCollisionCloseReturned=true;
                            }else {
                                if(directAccepted.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   directAccepted.sourcePC!=0x11ba34u)return stop(directAccepted.sourcePC);
                                if(!directMissing||!h.directCollisionSearch->returnedMissing132820())return stop(directAccepted.sourcePC);
                                h.sourceDirectCollisionMissingEntered=true;
                                const auto directReturn=h.sourceCollisionE4->continueSourceMissing11BA34();
                                if(directReturn.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   directReturn.sourcePC!=0x11bbbcu||!h.sourceCollisionE4->loader11B920Returned_)return stop(directReturn.sourcePC);
                            }
                            //Original E4 caller ignores genuine11B920V0, includingzero.
                            h.sourceDirectCollisionLoaderReturned=true;
                            at(0x16629c);h.sourceDirectCollisionLateEntered=true;
                            const auto directLate=h.sourceCollisionE4->publishSourceDirectCaller16629C();
                            if(directLate.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               directLate.sourcePC!=0x1662c0u||!h.sourceCollisionE4->directLateReturned_)return stop(directLate.sourcePC);
                            h.sourceDirectCollisionLateReturned=true;at(0x1662c0);
                            if(h.sourceDirectCollisionCloseReturned){
                                //Native dispatch to the exact qualified nonNULL source branch, not an original V0 condition.
                                h.sourceDirectCollisionFoundTransformEntered=true;
                                const auto directFoundTransform=h.sourceCollisionE4->beginSourceDirectFoundTransform11F570();
                                h.sourceDirectCollisionWorldCallReached=directFoundTransform.status==g::OriginalSourcePhysicsCollision50LA::Status::bounded&&
                                    directFoundTransform.sourcePC==0x28b550u&&h.sourceCollisionE4->directWorldCallReached_;
                                if(!h.sourceDirectCollisionWorldCallReached)return stop(directFoundTransform.sourcePC);
                                at(0x28b550);h.sourceDirectCollisionMatrixEntered=true;
                                const auto directMatrix=h.sourceCollisionE4->continueSourceDirectMatrix11F5B4();
                                if(directMatrix.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   directMatrix.sourcePC!=0x11f678u||!h.sourceCollisionE4->directMatrix_.returned)return stop(directMatrix.sourcePC);
                                h.sourceDirectCollisionMatrixReturned=true;
                                at(0x1662c8);
                                if(!h.sourceDirectCollisionEnrolled||!h.sourceCollisionE4->countedEntityNative()||
                                   !authority.canRetire(*h.sourceCollisionE4->countedEntityNative())||
                                   !h.bodyOwner->object_||!authority.canRetire(h.bodyOwner->object_->entity))return stop(pc);
                                h.sourceDirectCollisionAttachEntered=true;at(0x1662dc);
                                const auto directAttached=h.sourceCollisionE4->attachSourceCollision141960(attach);
                                if(directAttached.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   directAttached.sourcePC!=0x1662e4u||!h.sourceCollisionE4->attachReturned_)return stop(directAttached.sourcePC);
                                h.sourceDirectCollisionAttachReturned=true;
                                at(0x1662e4);
                                if(h.sourceCollisionE8||authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                                   !authority.reserveAdditional(1))return stop(pc);
                                try{h.sourceCollisionE8=std::make_unique<g::OriginalSourcePhysicsCollision50LA>();}catch(...){return stop(pc);}
                                h.sourceProceduralCollisionCallEntered=true;
                                const auto procedural=h.sourceCollisionE8->allocateSource1662E4(*h.bodyOwner,*h.sourceCollisionE4);
                                //Enroll every actual counted prefix before inspecting any source/native result.
                                if(auto* entity=h.sourceCollisionE8->countedEntityNative()){
                                    if(!authority.enroll(*entity))return stop(procedural.sourcePC);h.sourceProceduralCollisionEnrolled=true;
                                }
                                if(procedural.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   procedural.sourcePC!=0x166304u||!h.sourceCollisionE8->published_)return stop(procedural.sourcePC);
                                h.sourceProceduralCollisionCallReturned=true;
                                at(0x166308);h.sourceProceduralCollisionConfigEntered=true;
                                const auto proceduralMode=h.sourceCollisionE8->configureSource166308();
                                if(proceduralMode.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   proceduralMode.sourcePC!=0x166310u||!h.sourceCollisionE8->modeReturned_)return stop(proceduralMode.sourcePC);
                                h.sourceProceduralCollisionConfigReturned=true;
                                at(0x166310);
                                if(!h.sourceProceduralCollisionEnrolled||!h.sourceCollisionE8->countedEntityNative()||
                                   !authority.canRetire(*h.sourceCollisionE8->countedEntityNative())||
                                   !h.bodyOwner->object_||!authority.canRetire(h.bodyOwner->object_->entity))return stop(pc);
                                h.sourceProceduralCollisionAttachEntered=true;at(0x166324);
                                const auto proceduralAttached=h.sourceCollisionE8->attachSourceCollision141960(attach);
                                if(proceduralAttached.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   proceduralAttached.sourcePC!=0x16632cu||!h.sourceCollisionE8->attachReturned_)return stop(proceduralAttached.sourcePC);
                                h.sourceProceduralCollisionAttachReturned=true;
                                at(0x16632c);h.sourceProceduralCollisionLateEntered=true;
                                const auto proceduralLate=h.sourceCollisionE8->publishSourceProceduralCaller16632C();
                                if(proceduralLate.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   proceduralLate.sourcePC!=0x166350u||!h.sourceCollisionE8->proceduralLateReturned_)return stop(proceduralLate.sourcePC);
                                h.sourceProceduralCollisionLateReturned=true;
                                at(0x166350);h.sourceProceduralCollisionShapeEntered=true;
                                const auto proceduralShape=h.sourceCollisionE8->beginSourceProceduralShape11C710();
                                h.sourceProceduralCollisionShapeResult=proceduralShape;
                                if(proceduralShape.status!=g::OriginalSourcePhysicsCollision50LA::Status::bounded||
                                   proceduralShape.sourcePC!=0x11c934u)return stop(proceduralShape.sourcePC);
                                at(0x11c934);h.sourceProceduralCollisionResultEntered=true;
                                const auto proceduralResult=h.sourceCollisionE8->continueSourceProceduralResult11C934();
                                h.sourceProceduralCollisionResultCall=proceduralResult;
                                if(proceduralResult.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   proceduralResult.sourcePC!=0x11ca80u||!h.sourceCollisionE8->proceduralResultReturned_)
                                    return stop(proceduralResult.sourcePC);
                                h.sourceProceduralCollisionResultReturned=true;
                                at(0x166358);
                                const auto filterCall=[&](unsigned index,std::uint32_t expectedPC,auto&& issue)noexcept {
                                    h.sourceLocalFilterEntered|=std::uint16_t(1u<<index);
                                    const auto result=issue();h.sourceLocalFilterCalls[index]=result;at(result.sourcePC);
                                    return result.status==g::OriginalSourcePhysicsCollision50LA::Status::reached&&result.sourcePC==expectedPC;
                                };
                                if(!filterCall(0,0x166364u,[&]()noexcept{return h.sourceCollisionE8->configureSourceCategoryMode124160();}))return stop(pc);
                                if(!filterCall(1,0x166374u,[&]()noexcept{return h.sourceCollisionE8->appendSourceCategory124170();}))return stop(pc);
                                if(!filterCall(2,0x166390u,[&]()noexcept{return h.sourceCollisionE8->enableSourceProceduralCollision16638C();}))return stop(pc);
                                if(!filterCall(3,0x16639cu,[&]()noexcept{return h.sourceCollisionE0->setSourceFilterMode123EA0(*h.sourceCollisionE8,*h.sourceCollisionE0);}))return stop(pc);
                                if(!filterCall(4,0x1663a8u,[&]()noexcept{return h.sourceCollisionE4->setSourceFilterMode123EA0(*h.sourceCollisionE8,*h.sourceCollisionE0);}))return stop(pc);
                                if(!filterCall(5,0x1663b4u,[&]()noexcept{return h.sourceCollisionE0->appendSourceExclusion123EB0(*h.sourceCollisionE4,*h.sourceCollisionE8);}))return stop(pc);
                                if(!filterCall(6,0x1663c0u,[&]()noexcept{return h.sourceCollisionE0->appendSourceExclusion123EB0(*h.sourceCollisionE8,*h.sourceCollisionE8);}))return stop(pc);
                                if(!filterCall(7,0x1663ccu,[&]()noexcept{return h.sourceCollisionE4->appendSourceExclusion123EB0(*h.sourceCollisionE0,*h.sourceCollisionE8);}))return stop(pc);
                                if(!filterCall(8,0x1663d8u,[&]()noexcept{return h.sourceCollisionE4->appendSourceExclusion123EB0(*h.sourceCollisionE8,*h.sourceCollisionE8);}))return stop(pc);
                                //Only original control epilogues remain after all genuine calls.
                                //1663D8..3F4 returns165E90;165DB0 explicitly returns same BASE.
                                h.sourceSetup165E90Returned=true;at(0x165db0);
                                if(!h.bodyOwner->preflightNative()||!h.bodyOwner->object_||!h.body8||
                                   *h.body8!=&h.bodyOwner->object_->entity)return stop(pc);
                                h.sourceSetupReturnedBase=&h.bodyOwner->object_->entity;
                                h.sourceSetup165C30Returned=true;at(0x27b470);
                                if(!h.sourceSetupReturnedBase)return stop(pc);
                                h.sourceSetupPositive27B470=true;
                                at(0x27b534);auto& config=h.configure166400;
                                if(config.entered)return stop(config.pc); //No replay of original argument/scratch prefix.
                                //Native exact-owner/knownness admission, before original caller stores.
                                if(!h.descriptor0C||h.descriptor0C->scalarKnown!=0x7ffu||
                                   !h.bodyOwner->object_||h.bodyOwner->nativeWithdrawn_||
                                   h.sourceSetupReturnedBase!=&h.bodyOwner->object_->entity||
                                   !h.body8||*h.body8!=h.sourceSetupReturnedBase||!h.nestedModel||
                                   h.nestedModel->nativeWithdrawn_||!h.nestedModel->model_||
                                   h.bodyOwner->object_->model14C!=&h.nestedModel->model_->model_||
                                   !h.sourceCollisionE0->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE4->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE8->sourceFilterLiveOwnerNative())return stop(pc);
                                const auto& descriptor=*h.descriptor0C;config.entered=true;config.descriptor=&descriptor;
                                config.receiver=*h.body8;config.nestedOwner=h.nestedModel.get();
                                at(0x27b538);const auto stackArgument=descriptor.scalar284[7];
                                at(0x27b53c);config.callerStack[0]=stackArgument;config.callerStackKnown|=1u;
                                for(unsigned i=1;i<6;++i){at(0x27b53cu+4u*i);config.callerStack[2*i]=0;
                                    config.callerStackKnown|=std::uint16_t(1u<<(2*i));}
                                //High32 halves of these six caller stack slots remain source-unknown.
                                at(0x27b55c);config.dispatch08=h.bodyOwner->object_->dispatch08;
                                for(unsigned i=1;i<8;++i){at(0x27b55cu+4u*i);
                                    config.argumentFpu[i]=descriptor.scalar284[i==7?8:i];config.argumentFpuKnown|=std::uint8_t(1u<<i);}
                                at(0x27b57c);config.target74=0x166400u; //Actual3CDF80+74 entry, not an alternate prepared method.
                                at(0x27b584);config.argumentFpu[0]=descriptor.scalar284[0];config.argumentFpuKnown|=1u;
                                config.argumentsPrepared=true;config.calleeEntered=true;at(0x166400);
                                //Original saves f27..f20 to stack1C..0 before replacing them; old values are unavailable, not zero.
                                config.savedOldFpuStores=0xffu;config.savedOldFpuKnown=0;
                                for(unsigned i=0;i<8;++i){at(0x166430u+4u*i);config.calleeFpu[7-i]=config.argumentFpu[i];
                                    config.calleeFpuKnown|=std::uint8_t(1u<<(7-i));}
                                const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
                                at(0x166454);config.center40=vf0;config.centerKnown=0x0fu;
                                at(0x16645c);config.half50=vf0;config.halfKnown=0x0fu;
                                at(0x166460); //Actual14D7F0 is an all-ZERO matrix constructor, not14D840 identity.
                                for(auto& word:config.scratchA0)word=0;
                                config.scratchKnown=0xffffu;config.scratchReturned=true;
                                at(0x166484);config.working60=config.scratchA0;config.workingKnown=0xffffu;config.scratchCopied=true;
                                at(0x166494);config.model14C=h.bodyOwner->object_->model14C;
                                config.boundsCallReached=true;config.pc=0x28d670u;
                                //Actual same-owner callee consumes the real stack40/50 temporaries.
                                if(h.nestedModel->nativeWithdrawn_||!h.nestedModel->model_||
                                   config.model14C!=&h.nestedModel->model_->model_||
                                   !h.sourceCollisionE0->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE4->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE8->sourceFilterLiveOwnerNative())return stop(config.pc);
                                config.boundsEntered=true;
                                const auto bounds=h.nestedModel->produceSourceLocalBounds28D670(config.center40,config.half50);
                                config.boundsResult=bounds;
                                if(bounds.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||
                                   bounds.sourcePC!=0x28d93cu||!h.nestedModel->localBounds_.returned)return stop(bounds.sourcePC);
                                config.boundsReturned=true;
                                at(0x1664a0);
                                if(!h.sourceBodyD0||!h.nestedModel||h.nestedModel->nativeWithdrawn_||
                                   !h.sourceCollisionE0->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE4->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE8->sourceFilterLiveOwnerNative())return stop(pc);
                                config.dimensionsEntered=true;
                                const auto dimensions=h.sourceBodyD0->configureSourceDimensionsMass1664A0(
                                    *h.nestedModel,config.half50,config.argumentFpu[0]); //Actual f27=descriptor284 raw bits.
                                config.dimensionsResult=dimensions;
                                if(dimensions.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||
                                   dimensions.sourcePC!=0x166558u||!h.sourceBodyD0->dimensionsMass_.returned||
                                   !h.sourceBodyD0->dimensionsMass_.massCallReturned)return stop(dimensions.sourcePC);
                                config.dimensionsReturned=true;
                                at(0x166558);
                                if(!h.sourceCollisionE0->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE4->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE8->sourceFilterLiveOwnerNative())return stop(pc);
                                config.inertiaEntered=true;
                                const auto inertia=h.sourceBodyD0->configureSourceInertia166558(
                                    config.half50,config.working60,config.argumentFpu[1]); //Actual descriptor288/f26 raw bits.
                                config.inertiaResult=inertia;
                                if(inertia.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||
                                   inertia.sourcePC!=0x166618u||!h.sourceBodyD0->inertia_.returned)return stop(inertia.sourcePC);
                                config.inertiaReturned=true;
                                at(0x166618);
                                if(!h.sourceContactDC||!h.bodyOwner||
                                   !h.sourceCollisionE0->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE4->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE8->sourceFilterLiveOwnerNative())return stop(pc);
                                for(unsigned i=0;i<5;++i)config.contactArguments[i]=config.argumentFpu[i+2];
                                for(unsigned i=5;i<8;++i)config.contactArguments[i]=config.callerStack[0];
                                config.contactEntered=true;
                                const auto contact=h.sourceContactDC->configureSource295C90(config.contactArguments);
                                config.contactResult=contact;
                                if(contact.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                                   contact.sourcePC!=0x295dc8u||!h.sourceContactDC->tuning_.returned)return stop(contact.sourcePC);
                                config.contactReturned=true;at(0x166640);
                                config.frictionEntered=true;
                                const auto friction=h.sourceContactDC->frictionSource295DD0(config.argumentFpu[7]); //Actual signed index0.
                                config.frictionResult=friction;
                                if(friction.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                                   friction.sourcePC!=0x295df4u||!h.sourceContactDC->tuning_.frictionReturned)return stop(friction.sourcePC);
                                config.frictionReturned=true;at(0x166650);
                                config.tailEntered=true;
                                const auto tail=h.bodyOwner->publishSourceConfigurationTail166650(
                                    *h.sourceContactDC,&config,config.callerStack,config.callerStackKnown);
                                config.tailResult=tail;
                                if(tail.status!=g::OriginalSourcePhysicsObject50LA::Status::reached||
                                   tail.sourcePC!=0x1666b8u||!h.bodyOwner->sourceConfigurationTail_.returned)return stop(tail.sourcePC);
                                config.returned74=true;
                                at(0x27b588);
                                if(!h.descriptor0C||config.descriptor!=h.descriptor0C.get()||!h.descriptor0C->vectorKnown||
                                   !h.bodyOwner||!h.sourceBodyD0||!h.sourceContactDC||
                                   !h.bodyOwner->sourceConfigurationTailReturnCurrentNative(*h.sourceContactDC,&config)||
                                   !h.sourceCollisionE0->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE4->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE8->sourceFilterLiveOwnerNative())return stop(pc);
                                config.updateEntered=true;
                                at(0x27b590);config.descriptorQuad=h.descriptor0C->vector2B0;config.descriptorQuadLoaded=true;
                                at(0x27b594);
                                if(!h.bodyOwner->object_->bodyD0)return stop(pc);
                                config.quadBodyD0=*h.bodyOwner->object_->bodyD0;
                                if(!config.quadBodyD0)return stop(0x27b598u);
                                if(config.quadBodyD0!=h.sourceBodyD0->publishedIdentity_)return stop(pc);
                                at(0x27b598);config.bodyQuadEntered=true;
                                const auto quad=h.sourceBodyD0->copySourceCenterOffset27B598(*h.sourceContactDC,&config,config.descriptorQuad);
                                config.bodyQuadResult=quad;
                                if(quad.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||quad.sourcePC!=0x27b59cu||
                                   !h.sourceBodyD0->sourceCenterOffset_.returned)return stop(quad.sourcePC);
                                config.bodyQuadReturned=true;at(0x27b59c);
                                if(!h.body8||*h.body8!=config.receiver)return stop(pc);
                                config.updateDispatch08=h.bodyOwner->object_->dispatch08;
                                if(config.updateDispatch08!=0x3cdf80u)return stop(0x27b5a0u);
                                config.updateTarget78=0x1666c0u; //Actual original3CDFF8 dispatch word.
                                config.updateCallReached=true;config.updatePC=0x120290u;
                                at(config.updatePC);
                                g::OriginalSourcePhysicsCollision50LA* const unlinkOwners[2]={h.sourceCollisionE4.get(),h.sourceCollisionE0.get()};
                                for(unsigned index=0;index<2;++index){
                                    config.nullMembershipEntered|=std::uint8_t(1u<<index);
                                    const auto unlinked=unlinkOwners[index]->removeSourceNullMembership120290(&config);
                                    config.nullMembershipResults[index]=unlinked;
                                    if(unlinked.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                       unlinked.sourcePC!=0x1202d4u||!unlinkOwners[index]->nullMembership_.returned)return stop(unlinked.sourcePC);
                                    config.nullMembershipReturned|=std::uint8_t(1u<<index);
                                    at(index?0x1666e0u:0x1666d8u);
                                }
                                config.nullObserverEntered=true;
                                const auto observerRemoved=h.nestedModel->removeSourceNullObserver28EA80(&config);
                                config.nullObserverResult=observerRemoved;
                                if(observerRemoved.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||
                                   observerRemoved.sourcePC!=0x28eac4u||!h.nestedModel->nullObserver_.returned)return stop(observerRemoved.sourcePC);
                                config.nullObserverReturned=true;
                                at(0x1666e8);
                                //Actual a0=same Model14C,a1=NULL,a2=&same Base50. The callee
                                //owns the read after its process guard/parent branch; no eager raw input read here.
                                config.positionEntered=true;
                                const auto position=h.nestedModel->translateSourceNullRelative28ADD0(&config);
                                config.positionResult=position;
                                if(position.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||
                                   position.sourcePC!=0x28af90u||!h.nestedModel->translation_.returned)return stop(position.sourcePC);
                                config.positionReturned=true;
                                at(0x1666f8);
                                //Actual1666F8..166714: a0=same Model14C,a1=NULL,
                                //a2=&same Base60,a3=&same Base70,t0=0,t1=1,t2=0.
                                //The narrow callee owns raw Base reads after its independent
                                //process guard and saved-translation snapshot, not this caller.
                                at(0x166710);config.orientationEntered=true;
                                const auto orientation=h.nestedModel->orientSourceNullRelative28A090(&config);
                                config.orientationResult=orientation;
                                if(orientation.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||
                                   orientation.sourcePC!=0x28a528u||!h.nestedModel->orientation_.returned)return stop(orientation.sourcePC);
                                config.orientationReturned=true;
                                at(0x166718);
                                //Actual16671C delay-slot argument is this BaseE4, not E0/E8.
                                //11FE50 owns a fresh28B550 getter and one11F680 pass;
                                //neither initialization getter nor either initialization pass is replayed.
                                config.directUpdateEntered=true;
                                const auto directUpdate=h.sourceCollisionE4->updateSourceDirectType0_11FE50(&config,
                                    [&](const void* updateOccurrence,const void* collisionOccurrence,
                                        std::array<std::uint32_t,16>& incoming,std::uint16_t& known)noexcept
                                        ->g::OriginalSourcePhysicsBody50LA::Result {
                                        const auto world=h.nestedModel->copySourcePostPoseWorld28B550(
                                            updateOccurrence,collisionOccurrence,incoming,known);
                                        using ModelStatus=g::OriginalSourcePhysicsModel50LA::Status;
                                        using BodyStatus=g::OriginalSourcePhysicsBody50LA::Status;
                                        return {world.status==ModelStatus::reached?BodyStatus::reached:
                                            world.status==ModelStatus::owner?BodyStatus::owner:BodyStatus::bounded,world.sourcePC};
                                    });
                                config.directUpdateResult=directUpdate;
                                if(directUpdate.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   directUpdate.sourcePC!=0x11ff5cu||!h.sourceCollisionE4->postUpdate_.returned)return stop(directUpdate.sourcePC);
                                config.directUpdateReturned=true;
                                at(0x166720);
                                //Actual166724 delay-slot argument is this BaseE0. This distinct
                                //result-backed occurrence follows the genuine E4 control return;
                                //its new28B550 getter may copy the proven clean root, not replay E4.
                                config.resultUpdateEntered=true;
                                const auto resultUpdate=h.sourceCollisionE0->updateSourceResultType0_11FE50(
                                    &config,*h.sourceCollisionE4,
                                    [&](const void* updateOccurrence,const void* collisionOccurrence,
                                        std::array<std::uint32_t,16>& incoming,std::uint16_t& known)noexcept
                                        ->g::OriginalSourcePhysicsBody50LA::Result {
                                        const auto world=h.nestedModel->copySourcePostE4World28B550(
                                            updateOccurrence,collisionOccurrence,incoming,known);
                                        using ModelStatus=g::OriginalSourcePhysicsModel50LA::Status;
                                        using BodyStatus=g::OriginalSourcePhysicsBody50LA::Status;
                                        return {world.status==ModelStatus::reached?BodyStatus::reached:
                                            world.status==ModelStatus::owner?BodyStatus::owner:BodyStatus::bounded,world.sourcePC};
                                    });
                                config.resultUpdateResult=resultUpdate;
                                if(resultUpdate.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   resultUpdate.sourcePC!=0x11ff5cu||!h.sourceCollisionE0->postUpdate_.returned)return stop(resultUpdate.sourcePC);
                                config.resultUpdateReturned=true;
                                at(0x166728u);
                                //Actual E4 pointer reload, then typed BaseB8 at16672C.
                                //An absent optional is unknown, not sourceNULL; raw Fields
                                //storage cannot manufacture a guest pointer or this load.
                                if(!h.bodyOwner->object_||!h.sourceCollisionE4||!h.sourceCollisionE0||
                                   h.bodyOwner->object_->collisionE4!=std::optional<g::OriginalCollisionBaseOwner50LA*>(
                                       h.sourceCollisionE4->collision_.get()))return stop(0x166728u);
                                at(0x16672cu);
                                if(!h.bodyOwner->object_->collisionB8)return stop(0x16672cu);
                                config.directMembershipZone=*h.bodyOwner->object_->collisionB8;
                                //166734 delay slot supplies mode2; the later E0 registration also uses mode2.
                                config.directMembershipMode=2u;at(0x166730u);
                                config.directMembershipEntered=true;
                                const auto makeCollisionMethod=[&](bool resultBacked)noexcept {
                                    //Only these caller-owned occurrences can authorize callbacks;
                                    //capture the selected owner by value, never a mutable routing slot.
                                    auto* const target=resultBacked?h.sourceCollisionE0.get():h.sourceCollisionE4.get();
                                    return [&,target](const void* actualZone,const void* registrationOccurrence,
                                    const void* actualCOL,NativeListMethodCustody*& method,std::optional<std::uint32_t>& slot,
                                    bool& selectionPending,std::uint32_t& methodPC,bool select)noexcept {
                                    //Native same-world custody proof for the actual27B43C ->165ED0
                                    //collisionB8 argument. Never resolve it through visibility4C0/B4.
                                    methodPC=select?0x280afcu:0x27f5b4u;
                                    if(!h.collisionRead||!h.collisionB8Stored||!h.collisionZone||*h.collisionZone>=10||
                                       !h.bodyOwner||!h.bodyOwner->object_||!h.bodyOwner->object_->collisionB8||
                                       actualZone!=*h.bodyOwner->object_->collisionB8||actualZone!=h.collisionNode||
                                       !target||actualCOL!=target->collision_.get()||
                                       registrationOccurrence!=&target->membershipRegistration_||
                                       !sourceTopologyMode_||sourceTopologyWithdrawn_||!sourceCollision4C4_||
                                       *sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||!sourceCollisionSlots4C4_||
                                       *h.collisionZone>=sourceCollisionPublished_)return false;
                                    const auto zone=*h.collisionZone;auto& node=sourceSearchNodes_[1][zone];
                                    if(node.withdrawn||!node.published||!node.owner||!node.ordinalKnown||
                                       sourceCollisionSlots4C4_[zone]!=node.owner.get()||actualZone!=node.owner.get()||
                                       !preflightSourceSearchNodeNative(node,zone,true))return false;
                                    //Mode2 follows27F5B4, selecting collision dynamic method kind3
                                    //(World4D4), NOT collision static kind1/World4CC.
                                    if(!select)method=node.owner->nativeList38;
                                    if(method!=node.owner->nativeList38||!method||!nativeMethods4D4_||
                                       nativeMethods4D4_[zone]!=method||!sourceListMethodArrayCurrent(3,zone,method))return false;
                                    const auto& publication=sourceListMethods_[3][zone];
                                    if(!publication.arrayPublished||!publication.aliasPublished||!publication.aliasRetained||
                                       publication.withdrawn||publication.aliasNode!=node.owner.get()||
                                       !method->sourceMode_||method->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
                                       method->application_!=worldApplication_.get()||method->ordinal_!=30u+zone||
                                       method->usage10_!=2||method->reference00_!=2||
                                       !method->source_.constructorReturned||method->source_.vtable08!=0x3d2d30u||
                                       !sourceListEntriesCurrent(*method))return false;
                                    if(!select)return true;
                                    //Actual3D2D30+20=27E3B0 tailcalls27E3C0 with a3(NULL)
                                    //as requested slot. +24=27E570 is owned by COL insertion.
                                    return selectSourceObserverSlot27E3C0(*method,registrationOccurrence,actualCOL,
                                        slot,selectionPending,methodPC,2u);
                                };
                                };
                                const auto makeCollisionPortal=[&](bool resultBacked)noexcept {
                                    //Only these caller-owned occurrences can authorize callbacks;
                                    //capture the selected owner by value, never a mutable routing slot.
                                    auto* const target=resultBacked?h.sourceCollisionE0.get():h.sourceCollisionE4.get();
                                    return [&,target](unsigned phase,const void* registration,const void* actualCOL,
                                    const void* actualZone,const std::array<std::uint32_t,4>& center,const void* cursor,
                                    auto& edge,std::shared_ptr<const void>& graphPin,std::uint32_t& portalPC)noexcept {
                                    portalPC=phase==0?0x280c34u:phase==2?0x280c90u:phase==4?0x280cf8u:0x280c40u;
                                    if(phase>4||!target||registration!=&target->membershipRegistration_||
                                       actualCOL!=target->collision_.get()||!h.collisionRead||!h.collisionB8Stored||
                                       !h.collisionZone||*h.collisionZone>=10||!h.bodyOwner||!h.bodyOwner->object_||
                                       !h.bodyOwner->object_->collisionB8||actualZone!=*h.bodyOwner->object_->collisionB8||
                                       actualZone!=h.collisionNode||center!=target->membershipRegistration_.center||
                                       !sourceTopologyMode_||sourceTopologyWithdrawn_||!sourceCollision4C4_||
                                       *sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||!sourceCollisionSlots4C4_||
                                       *h.collisionZone>=sourceCollisionPublished_)return false;
                                    const auto zone=*h.collisionZone;auto& root=sourceSearchNodes_[1][zone];
                                    if(root.withdrawn||!root.published||!root.owner||actualZone!=root.owner.get()||
                                       sourceCollisionSlots4C4_[zone]!=root.owner.get()||
                                       !preflightSourceSearchNodeNative(root,zone,true)||
                                       !sourcePortalWorldLinksCurrent(*root.owner,true,zone))return false;
                                    if(phase==0){
                                        if(graphPin||cursor)return false;
                                        for(const auto& old:sourceCollisionPortalBorrows_)
                                            if(old.registration==registration&&!old.token.expired())return false;
                                        //Allocate native custody before exposing the source head pointer.
                                        //Weak root records never keep a completed COL borrower alive.
                                        try{std::shared_ptr<const void> token=std::make_shared<std::uint8_t>(0);
                                            sourceCollisionPortalBorrows_.push_back({registration,token});graphPin=std::move(token);
                                        }catch(...){return false;}
                                        edge.next=root.owner->portalLinks30.get();return true;
                                    }
                                    bool pinned=false;
                                    for(const auto& borrow:sourceCollisionPortalBorrows_)if(borrow.registration==registration){
                                        const auto live=borrow.token.lock();if(live&&live==graphPin){pinned=true;break;}}
                                    if(!pinned||!graphPin||!cursor)return false;
                                    std::size_t index=0;while(index<40&&sourcePortalWorldLinks_[index/4][index%4].published!=cursor)++index;
                                    //Odd links are collision namespace (stages1/3), never visibility0/2.
                                    if(index==40||!(index&1u))return false;
                                    auto& link=sourcePortalWorldLinks_[index/4][index%4];
                                    if(link.withdrawn||link.knownMask!=31||!link.published||link.from!=root.owner.get()||!link.to)return false;
                                    auto& raw=*link.published;const auto& portal=nativePortals_[index/4];
                                    if(!portal||!portal->sourceMode||raw.neighbor04!=&link.to->entity||
                                       raw.portal08!=&portal->entity||!portal->source.loadReturned||
                                       portal->source.normalKnown!=7||!portal->source.normalNormalized||portal->source.copiedVertices!=4)return false;
                                    for(unsigned i=0;i<4;++i)if(portal->source.edgeKnown[i]!=7||!portal->source.edgeNormalized[i])return false;
                                    if(phase==1){
                                        edge.entered=true;edge.edge=cursor;edge.edgeIndex=index;
                                        edge.neighbor=raw.neighbor04;edge.portal=raw.portal08;
                                        //Actual231150 uses center and f12=+0, not half/radius80.
                                        portalPC=0x231150u;
                                        const bool overlapOkay=g::source_portal_overlap231150::run(portal->source.vertices30,portal->source.normal70,
                                            portal->source.edges80,center,0u,edge.overlap);
                                        edge.overlapReturned=edge.overlap.returned;edge.hit=edge.overlap.hit;edge.distance=edge.overlap.outputDistance;
                                        if(!overlapOkay){portalPC=edge.overlap.pc;return false;}
                                        portalPC=0x280c54u;if(!edge.hit)return true;
                                        if(!edge.distance)return false;
                                        portalPC=0x280c60u;raw.field10=*edge.distance;link.distance10=edge.distance;edge.distanceStoreMask|=1u;
                                        portalPC=0x280c64u;raw.field0C=*edge.distance;link.distance0C=edge.distance;edge.distanceStoreMask|=2u;
                                    }else if(edge.edge!=cursor||edge.edgeIndex!=index||edge.neighbor!=raw.neighbor04||edge.portal!=raw.portal08||
                                             !g::source_portal_overlap231150::historyCurrent(edge.overlap))return false;
                                    if(phase==4){edge.next=raw.next00.get();portalPC=0x280cf8u;return true;}
                                    if(phase==3&&!edge.hit)return true;
                                    //The source keeps a method cursor: each hit reads its usage10,
                                    //then resolves the neighbor method using that same mode2.
                                    portalPC=phase==1?0x280c6cu:portalPC;
                                    const auto* prior=edge.previousMethod;
                                    if(!prior||!prior->sourceMode_||prior->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
                                       prior->application_!=worldApplication_.get()||prior->usage10_!=2||prior->ordinal_<30||prior->ordinal_>=40||
                                       !sourceListMethodArrayCurrent(3,prior->ordinal_-30,prior))return false;
                                    std::size_t neighborZone=0;while(neighborZone<10&&sourceCollisionSlots4C4_[neighborZone]!=link.to)++neighborZone;
                                    if(neighborZone==10||neighborZone>=sourceCollisionPublished_)return false;
                                    auto& neighbor=sourceSearchNodes_[1][neighborZone];
                                    if(neighbor.withdrawn||!neighbor.published||neighbor.owner.get()!=link.to||
                                       !preflightSourceSearchNodeNative(neighbor,neighborZone,true))return false;
                                    if(phase==1){portalPC=0x27f5b4u;edge.method=link.to->nativeList38;edge.methodReturned=true;}
                                    auto* method=edge.method;
                                    if(!method||method!=link.to->nativeList38||!nativeMethods4D4_||nativeMethods4D4_[neighborZone]!=method||
                                       !sourceListMethodArrayCurrent(3,neighborZone,method)||method->usage10_!=2||method->reference00_!=2||
                                       !method->sourceMode_||method->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
                                       method->application_!=worldApplication_.get()||method->ordinal_!=30u+neighborZone||
                                       !method->source_.constructorReturned||method->source_.vtable08!=0x3d2d30u||
                                       !sourceListEntriesCurrent(*method))return false;
                                    const auto& publication=sourceListMethods_[3][neighborZone];
                                    if(!publication.arrayPublished||!publication.aliasPublished||!publication.aliasRetained||
                                       publication.withdrawn||publication.aliasNode!=link.to)return false;
                                    if(phase==2){portalPC=0x280c90u;
                                        if(!edge.overlap.returned||!edge.overlap.hit||edge.distanceStoreMask!=3u||!edge.methodReturned)return false;
                                        return selectSourceObserverSlot27E3C0(*method,&edge,actualCOL,
                                            edge.slot,edge.selectionPending,portalPC,2u);}
                                    return phase==1||phase==3;
                                };
                                };
                                const auto membership=h.sourceCollisionE4->registerSourceDirectMembership120070(
                                    &config,*h.sourceCollisionE0,*config.directMembershipZone,config.directMembershipMode,makeCollisionMethod(false),makeCollisionPortal(false));
                                config.directMembershipResult=membership;
                                if(membership.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   membership.sourcePC!=0x120224u||!h.sourceCollisionE4->membershipRegistration_.returned)
                                    return stop(membership.sourcePC);
                                config.directMembershipReturned=true;
                                at(0x166738u);
                                //Actual E0 reload only after the E4 call genuinely returned120224.
                                //The result-backed COL has a distinct observer/list/pool occurrence.
                                if(!h.bodyOwner->object_||!h.sourceCollisionE0||
                                   h.bodyOwner->object_->collisionE0!=std::optional<g::OriginalCollisionBaseOwner50LA*>(
                                       h.sourceCollisionE0->collision_.get()))return stop(0x166738u);
                                at(0x16673cu);
                                if(!h.bodyOwner->object_->collisionB8)return stop(0x16673cu);
                                config.resultMembershipZone=*h.bodyOwner->object_->collisionB8;
                                config.resultMembershipMode=2u;at(0x166740u); //Mode2 in166744 delay slot.
                                config.resultMembershipEntered=true;
                                const auto resultMembership=h.sourceCollisionE0->registerSourceResultMembership120070(
                                    &config,*h.sourceCollisionE4,*config.resultMembershipZone,config.resultMembershipMode,
                                    makeCollisionMethod(true),makeCollisionPortal(true));
                                config.resultMembershipResult=resultMembership;
                                if(resultMembership.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   resultMembership.sourcePC!=0x120224u||!h.sourceCollisionE0->membershipRegistration_.returned)
                                    return stop(resultMembership.sourcePC);
                                config.resultMembershipReturned=true;
                                at(0x166748u);
                                //Actual ModelBASE14C is distinct from embedded Entity+8.
                                //Both preceding COL registrations genuinely returned before this call.
                                if(!config.directMembershipReturned||!h.bodyOwner->object_||!h.nestedModel||
                                   h.nestedModel->nativeWithdrawn_||!h.nestedModel->model_||
                                   h.bodyOwner->object_->model14C!=&h.nestedModel->model_->model_)return stop(pc);
                                at(0x16674cu);
                                if(!h.bodyOwner->object_->visibilityB4)return stop(pc);
                                config.modelObserverZone=*h.bodyOwner->object_->visibilityB4;
                                config.modelObserverMode=2u;at(0x166750u); //166754 delay-slot mode2.
                                config.modelObserverEntered=true;
                                auto modelObserver=h.nestedModel->beginSourceModelObserver28E8A0(
                                    &config,*config.modelObserverZone,config.modelObserverMode);
                                if(h.nestedModel->modelObserver_.boundsReturned&&
                                   modelObserver.sourcePC==0x28e96cu){
                                    const auto primary=withSourcePhysicsModelPrimary2809C0(h,[&](auto& method,auto& portal)noexcept {
                                        const auto primary=h.nestedModel->continueSourceModelPrimary2809C0(method);
                                        if(primary.status!=g::OriginalSourcePhysicsModel50LA::Status::bounded||
                                           primary.sourcePC!=0x280bf0u||!h.nestedModel->modelObserver_.primaryPublished)
                                            return primary;
                                        const auto secondary=h.nestedModel->continueSourceModelPortals280BF0(portal);
                                        return secondary.status==g::OriginalSourcePhysicsModel50LA::Status::owner?primary:secondary;
                                    });
                                    if(primary.status!=g::OriginalSourcePhysicsModel50LA::Status::owner)
                                        modelObserver=primary;
                                }
                                if(modelObserver.status==g::OriginalSourcePhysicsModel50LA::Status::bounded&&
                                   modelObserver.sourcePC==0x280becu&&h.nestedModel->modelPortals_.observerReturned){
                                    const auto returned=h.nestedModel->continueSourceModelReturn28E98C();
                                    if(returned.status!=g::OriginalSourcePhysicsModel50LA::Status::owner)modelObserver=returned;
                                }
                                config.modelObserverResult=modelObserver;
                                if(modelObserver.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||
                                   modelObserver.sourcePC!=0x28ea14u||!h.nestedModel->modelObserver_.returned)
                                    return stop(modelObserver.sourcePC);
                                config.modelObserverReturned=true;at(0x166758u);
                                if(!h.bodyOwner->object_->bodyD0||!h.sourceBodyD0)return stop(pc);
                                config.resetBodyD0=*h.bodyOwner->object_->bodyD0;
                                if(!config.resetBodyD0||!h.sourceBodyD0->body_||
                                   config.resetBodyD0!=&h.sourceBodyD0->body_->entity)return stop(0x16675cu);
                                at(0x16675cu);config.resetDispatch08=h.sourceBodyD0->body_->dispatch08;
                                if(config.resetDispatch08!=0x3ce000u)return stop(pc);
                                at(0x166760u);config.resetTargetAC=0x167a50u; //Actual3CE0AC dispatch word.
                                at(0x166764u);config.bodyResetEntered=true;
                                const auto reset=h.sourceBodyD0->resetSourceBody167A50(&config,*h.nestedModel);
                                config.bodyResetResult=reset;
                                if(reset.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||
                                   reset.sourcePC!=0x167b7cu||!h.sourceBodyD0->bodyReset_.returned)return stop(reset.sourcePC);
                                config.bodyResetReturned=true;at(0x16676cu);
                                //16676C/70 restores,166774 JR with166778 delay. No V0 inferred.
                                config.updateReturned=true;config.updateReturnPC=0x16677cu;
                                at(0x27b5b0u);
                                if(!nativeModelZone354_||*nativeModelZone354_>=10)return stop(pc);
                                config.parentAttachZone=*nativeModelZone354_;
                                at(0x27b5b4u);
                                if(!sourceStaticOwner_||!sourceModelParents3C4_||!sourceModels3C4_||
                                   *sourceModels3C4_!=sourceModelParents3C4_.get()||
                                   sourceModelParentsPublished3C4_!=10||*config.parentAttachZone>=sourceStaticOwner_->parents_.size())return stop(pc);
                                at(0x27b5b8u);
                                if(!h.body8||*h.body8!=h.bodyOwner->publishedIdentity_||!h.bodyEnrolled||
                                   !h.bodyOwner->countedEntityNative())return stop(pc);
                                auto& selectedParent=sourceStaticOwner_->parents_[*config.parentAttachZone];
                                at(0x27b5c4u);config.parentAttachIdentity=sourceModelParents3C4_[*config.parentAttachZone];
                                if(!selectedParent||selectedParent->released||!selectedParent->sourceMode||
                                   !selectedParent->constructed||selectedParent->zone!=*config.parentAttachZone+1u||
                                   !selectedParent->sourceCountKnown||!selectedParent->sourceArrayReturned||
                                   selectedParent->sourceVtable08!=0x3d2640u||
                                   config.parentAttachIdentity!=&selectedParent->entity||
                                   sourceStaticOwner_->application_!=worldApplication_||
                                   sourceStaticOwner_->generation_!=worldEntity_.sessionGeneration||
                                   sourceStandardParentS3_!=selectedParent.get()||!sourceStandardSlotS0_||
                                   *sourceStandardSlotS0_>=selectedParent->sourceEntriesKnown||!selectedParent->sourceEntries34||
                                   selectedParent->sourceEntries34[*sourceStandardSlotS0_]!=wrapper||
                                   *config.parentAttachZone>=sourceStaticOwner_->sourceWrapperLedger_.size()||
                                   *sourceStandardSlotS0_>=sourceStaticOwner_->sourceWrapperLedger_[*config.parentAttachZone].size()||
                                   sourceStaticOwner_->sourceWrapperLedger_[*config.parentAttachZone][*sourceStandardSlotS0_].get()!=wrapper||
                                   !sourceStaticOwner_->preflightSourceModelParentLinksNative()||
                                   !authority.canRetire(*h.bodyOwner->countedEntityNative()))return stop(pc);
                                //Same lazy native Parent authority enrollment used by Standard attachment.
                                //The source Parent is already counted; this adds no guest ref/count.
                                if(!selectedParent->sourceAuthorityEnrolled){
                                    if(!authority.reserveAdditional(1)||
                                       !authority.canEnroll(selectedParent->entity,selectedParent->entity.objectKey)||
                                       !authority.enroll(selectedParent->entity))return stop(pc);
                                    selectedParent->sourceAuthorityEnrolled=true;
                                }else if(!authority.canRetire(selectedParent->entity))return stop(pc);
                                at(0x27b5c8u);config.parentAttachEntered=true;
                                const auto parentAttached=h.bodyOwner->attachSourcePhysicsParent27B5C8(
                                    &config,config.updateReturnPC,selectedParent.get(),selectedParent->entity,
                                    *config.parentAttachZone,attach); //Mode2 in27B5CC delay slot.
                                config.parentAttachResult=parentAttached;
                                if(parentAttached.status!=g::OriginalSourcePhysicsObject50LA::Status::reached||
                                   parentAttached.sourcePC!=0x27b5d0u||!h.bodyOwner->parentAttachment_.returned)
                                    return stop(parentAttached.sourcePC);
                                config.parentAttachReturned=true;
                                at(0x27b5d0u);
                                if(!sourcePhysics3D4_)return stop(pc);
                                config.manager3D4=*sourcePhysics3D4_;
                                at(0x27b5d4u);
                                if(!nativeModelZone354_)return stop(pc);
                                std::int32_t managerZone=0;const std::uint32_t managerZoneBits=*nativeModelZone354_;
                                std::memcpy(&managerZone,&managerZoneBits,4);config.managerZone=managerZone;
                                at(0x27b5d8u); //Wrapper08 load in27B5DC delay.
                                if(!h.body8||!*h.body8||*h.body8!=h.bodyOwner->publishedIdentity_)return stop(pc);
                                config.managerBase=*h.body8;
                                if(!*config.manager3D4||!worldPhysics3D4_||*config.manager3D4!=worldPhysics3D4_.get()||
                                   !worldPhysics3D4_->sourceCurrentNative())return stop(pc);
                                config.managerCallEntered=true;config.managerPC=0x27c290u;
                                bool managerReturned=false;std::uint32_t managerV0=0;
                                const bool managerOkay=worldPhysics3D4_->registerSource27C290(worldEntity_,&config,
                                    *h.bodyOwner,*config.managerZone,config.managerRegistrationIndex,
                                    managerReturned,managerV0,config.managerPC);
                                if(!managerOkay||!managerReturned)return stop(config.managerPC);
                                config.managerReturned=true;config.managerReturnV0=managerV0;
                                //Wrapper ignores actual managerV0, including0. No registration-success inference.
                                at(0x27b5e0u);config.wrapperReturnV0=1u;
                                //27B5E4..EC restores registers;JR27B5F0/delay27B5F4 returns.
                                config.wrapperReturnPC=0x27b5f8u;
                                //27B930 ignores the genuine callback V0, even when
                                //27C290 returned0. Advance only after the real wrapper
                                //return; Physics descriptor0C remains owned.
                                if(!sourceStandardSlotS0_||*sourceStandardSlotS0_!=slot||
                                   !sourceStandardSlotByteOffsetS1_||*sourceStandardSlotByteOffsetS1_!=4u*slot||
                                   sourceStandardCompletedCallbacks_[z]!=slot)return stop(config.wrapperReturnPC);
                                at(0x27b978u);*sourceStandardSlotByteOffsetS1_+=4u;
                                at(0x27b97cu);++*sourceStandardSlotS0_;
                                sourceStandardCompletedCallbacks_[z]=*sourceStandardSlotS0_;
                                //Return to this same parent's live count test.
                                //The next actual entry chooses its own callback;
                                //do not replay this Physics wrapper or its receipt.
                                continue;
                            }
                            h.sourceDirectCollisionTransformEntered=true;
                            const auto directTransform=h.sourceCollisionE4->continueSourceDirectMissingTransform11F570();
                            h.sourceDirectCollisionNullModelBound=directTransform.status==g::OriginalSourcePhysicsCollision50LA::Status::bounded&&
                                directTransform.sourcePC==0x11f5b0u&&h.sourceCollisionE4->directNullModelBound_;
                            return stop(directTransform.sourcePC); //Actual NULLshape110 load; no28B550 or transform return.
                        }
                        if(accepted.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||accepted.sourcePC!=0x11ba34u)
                            return stop(accepted.sourcePC);
                        if(!collisionMissing||!h.collisionSearch->returnedMissing132820())return stop(accepted.sourcePC);
                        h.sourceCollisionMissingEntered=true;
                        const auto missing=h.sourceCollisionE0->continueSourceMissing11BA34();
                        return stop(missing.sourcePC); //Actual missing11BBEC NULLdelay-store or honest refusal; no165E90 return.
                    }
                    //Nonzero-mode miss retains12DCB0 fallback, never source0.
                    if(!returned)return stop(lookupDiagnostic.sourcePC?lookupDiagnostic.sourcePC:0x132820u);
                    if(!h.missingLookup||!h.missingLookup->liveMissingNative())return stop(0x132a0c);
                    h.sourceLookupReturnV0=h.missingLookup->sourceReturnV0();h.nestedLookupReturned=true;
                    at(0x26e9b8);h.nestedReleaseEntered=true;
                    const auto retireMissing=[&](g::OriginalWreckComponent50LA& entity)noexcept {
                        if(!h.nestedModelEnrolled||&entity!=h.nestedModel->countedModelEntityNative()||
                           !authority.canRetire(entity)||!authority.retire(entity))return false;
                        h.nestedModelEnrolled=false;return true;
                    };
                    const auto released=h.nestedModel->releaseSourceMissing26E9D4(h.missingLookup,
                        resolver.get(),sourceTrackFile_.get(),retireMissing);
                    if(released.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||released.sourcePC!=0x165cdcu)
                        return stop(released.sourcePC);
                    if(!h.nestedModel->sourceReleaseReturned_||!h.nestedModel->sourceLoaderReturnV0_||
                       *h.nestedModel->sourceLoaderReturnV0_)return stop(released.sourcePC);
                    h.nestedLoaderReturnV0=h.nestedModel->sourceLoaderReturnV0_;h.nestedLoaderReturned=true;
                    //165C30 ignores this real240740 zero. BASE14C remains the
                    //original published ModelBASE, including source-dead history.
                    //No safeNULL or live141960 attachment is fabricated.
                    return stop(0x165cdc);
                }
                if(wrapper->type00!=1||wrapper->sourceVtable04!=0x3d26f0||
                   wrapper->dispatch!=ra_static_model_cold50LA::Owner::StandardWrapper::Dispatch::standard3D26F0)return stop(pc);
                at(0x27b970);sourceStandardCallbackEntered_=true;
                if(!continueSourceStandardModel27ABC0(pc))return stop(pc);
                if(!wrapper->sourceCallbackReturnV0||*wrapper->sourceCallbackReturnV0>1)return stop(pc);
                //Original ignores actual callback V0, including genuine filename-none0.
                //Native refusal never reaches these source increments.
                at(0x27b978);*sourceStandardSlotByteOffsetS1_+=4;
                at(0x27b97c);++*sourceStandardSlotS0_;sourceStandardCompletedCallbacks_[z]=*sourceStandardSlotS0_;
                at(0x27b980);
            }
            at(0x27b9a0);sourceStandardDiagnosticAttempted_[z]=true;
            sourceStandardDiagnosticWritten_[z]=std::fprintf(stdout,"\nLoaded %d static models from Zone%d.\n\n",
                static_cast<int>(*sourceStandardSlotS0_),static_cast<int>(*nativeModelZone354_+1u));
            at(0x27b9bc);sourceStandardParentReturned_[z]=true;at(0x27b9c4);
        }
        //Actual World3CF110+98 is274250: second pass invokes +14 only for
        //type2 PhysicsObject; known other types skip, not fabricated callbacks.
        at(0x274208);at(0x274268);sourceStandardTypeSlotS0_=0;at(0x274270);sourceStandardTypeByteOffsetS1_=0;
        for(;;){
            //Original2742A8 reloads World354/3C4 and parent30 on EACH test.
            at(0x2742a8);if(!nativeModelZone354_||!sourceModelParents3C4_||!sourceModels3C4_||
               *sourceModels3C4_!=sourceModelParents3C4_.get())return stop(pc);
            sourceStandardTypeZoneRead_=*nativeModelZone354_;
            if(*sourceStandardTypeZoneRead_!=z)return stop(pc);
            at(0x2742b8);sourceStandardTypeParentRead_=sourceModelParents3C4_[z];
            if(sourceStandardTypeParentRead_!=&parent->entity)return stop(pc);
            at(0x2742bc);sourceStandardTypeCountRead_=parent->count30;
            if(*sourceStandardTypeCountRead_>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return stop(pc);
            at(0x2742c0);if(*sourceStandardTypeSlotS0_>=*sourceStandardTypeCountRead_)break;
            const auto slot=*sourceStandardTypeSlotS0_;at(0x274274);
            if(!parent->sourceEntries34||slot>=parent->sourceEntriesKnown||slot>=sourceStaticOwner_->sourceWrapperLedger_[z].size())return stop(pc);
            auto* wrapper=parent->sourceEntries34[slot];
            if(!wrapper||wrapper!=sourceStaticOwner_->sourceWrapperLedger_[z][slot].get()||!wrapper->sourceTypeKnown)return stop(0x274288);
            if(wrapper->type00==2){
                //Actual3D26C0+14 is27B600, not Standard's +10 callback.
                auto& h=wrapper->sourcePhysics;const auto& config=h.configure166400;
                at(0x274290);
                if(!sourceStandardParentReturned_[z]||sourceStandardCompletedCallbacks_[z]!=parent->count30||
                   !sourceStaticOwner_->sourcePhysicsWrapperCurrentNative(*wrapper)||
                   !h.callbackEntered||!config.wrapperReturnV0||*config.wrapperReturnV0!=1u||
                   config.wrapperReturnPC!=0x27b5f8u||!config.managerReturned||
                   h.secondCallback27B600Returned)return stop(pc);
                at(0x274294);at(0x274298);at(0x27429c);
                //27B600 JR RA /27B604 NOP returns without producing V0,
                //mutating the body, or retiring positive descriptor0C.
                at(0x27b600);at(0x27b604);h.secondCallback27B600Returned=true;
                at(0x2742a0);*sourceStandardTypeByteOffsetS1_+=4;
            }else{
                //BNE-likely skip executes this delay slot only for non-type2.
                at(0x27428c);*sourceStandardTypeByteOffsetS1_+=4;
            }
            at(0x2742a4);++*sourceStandardTypeSlotS0_;sourceStandardTypeScanned_[z]=*sourceStandardTypeSlotS0_;
        }
        at(0x2742e0);sourceStandardTypeReturned_[z]=true;
        at(0x274210);*sourceStandardZoneByteOffsetS1_+=0x160;
        at(0x274214);*sourceStandardZonePointerOffsetS2_+=4;
        at(0x274218);++*sourceStandardZoneS0_;at(0x274220);
    }
    at(0x274230);sourceStandardWorldReturnV0_=1;at(0x27424c);
    at(0x1aa208);
    //Actual1B3AE0 consumes World538/53C, not the legacy native model owner.
    //The genuine source1B50E0 absent NumberOfBuoys producer established538=0.
    //53C remains UNKNOWN: either null returns at1B3AF8, or nonnull reaches
    //the signed0<538 test1B3B50..58 before any element read/call2D3E60.
    //Record only the common no-effect return, never a fabricated pointer,
    //chosen branch, V0 result, buoy allocation, or attachment publication.
    if(sourceBuoysPhase_!=SourceBuoysPhase::returned||!preflightSourceWorldBuoysNative()||
       !nativeBuoyCount538_||*nativeBuoyCount538_!=0||sourceBuoyAttachNoEffect1B3AE0_)
        return stop(pc);
    at(0x1aa20c);at(0x1b3ae0);at(0x1b3b70);at(0x1b3b74);
    sourceBuoyAttachNoEffect1B3AE0_=true;
    at(0x1aa210);at(0x1b3b80);
    if(!constructSourceFinishLine1B3B80(pc))return stop(pc);
    //Loader authority belongs to this derived Owner, not the base strip owner.
    const auto& finishDomain=sourceLoadApplication_->standardMaterialDomainOwner_;
    if(!finishDomain||finishDomain!=sourceTrackMountDomain_||!finishDomain->prepared()||
       !sourceLoadApplication_->materialApplication)return stop(pc);
    const auto finishTable=finishDomain->materials382B38();
    if(!finishTable||!finishTable->sourceNames17CAD4Current()||
       !finishTable->screenApplicationMatches(*sourceLoadApplication_->materialApplication))return stop(pc);
    if(!setupSourceFinishLine150450(finishTable,sourceLoadApplication_->materialDirectory36B970(),pc))return stop(pc);
    if(!continueSourceFinishLineAttributes23BED8(pc))return stop(pc);
    if(!continueSourceFinishLineParameters23BF78(pc))return stop(pc);
    //Genuine134200 finalization and23BEB0 return completed in the same packet lease.
    if(!continueSourceFinishLineIndex1B3BFC(pc))return stop(pc);
    if(!continueSourceFinishLineAnchors1B3C1C(pc))return stop(pc);
    if(!continueSourceFinishLineComparison1B3CD8(pc))return stop(pc);
    if(!sourceFinishLineComparison_.missingAnchor||!preflightSourcePostParticleModelsNative())return stop(pc);
    if(*sourceFinishLineComparison_.missingAnchor){
        if(!destroySourceMissingFinishLine1B3DC0(pc))return stop(pc);
    }else{
        //Only the actual nonmissing branch supplies these authenticated snapshots.
        if(!continueSourceFinishLineGeometry1B3DDC(sourceFinishLineAnchors_.stackWords[0],
            sourceFinishLineAnchors_.stackWords[1],pc))return stop(pc);
    }
    if(pc!=0x1b4090||sourceFinishLineCallerReturn_.restoreMask||
       !preflightSourcePostParticleModelsNative())return stop(pc);
    auto& finishReturn=sourceFinishLineCallerReturn_;
    //Actual LQ RA,30(SP); S2,20(SP); S1,10(SP); S0,00(SP).
    //Native lexical caller state already retains these identities; only the
    //executed restore sequence is recorded, never invented guest register bits.
    at(0x1b4090);finishReturn.restoreMask|=1;finishReturn.v0Preserved=true;
    at(0x1b4094);finishReturn.restoreMask|=2;
    at(0x1b4098);finishReturn.restoreMask|=4;
    at(0x1b409c);finishReturn.restoreMask|=8;
    at(0x1b40a0); //JR RA: actual1AA210 JAL produced return address1AA218.
    at(0x1b40a4);finishReturn.stackReleased=true; //Delay slot ADDIU SP,SP,0xD0.
    finishReturn.returned=true;
    at(0x1aa218); //No instruction in this epilogue assigns V0.
    //1AA21C replaces V0 with the actual Application before its next use;
    //the branch-dependent, otherwise unknown finishline V0 is not consumed.
    at(0x1aa21c);
    if(!sourceLoadApplication_||sourceLoadApplication_->entities()!=worldApplication_)
        return stop(pc);
    at(0x1aa220);const auto& finishLoading=sourceLoadApplication_->sourceLoading330Owner_;
    if(!finishLoading)return stop(0x1aa224); //Unknown/missing producer, not an invented null skip.
    if(!finishLoading->current()||finishLoading->issuer_!=sourceLoadApplication_||
       !finishLoading->managerInitialized_||!finishLoading->slot330Published_||
       finishLoading->retiring_||finishLoading->retired_)return stop(0x1aa224);
    at(0x1aa224);finishReturn.loadingMode30=finishLoading->mode30_;
    at(0x1aa228);at(0x1aa22c); //BEQ V0,zero,1AA244 and NOP delay slot.
    if(*finishReturn.loadingMode30){
        //1AA230/234 materialize3DCCCCCD;1AA238 moves it toF12.
        //1AA23C JAL1CC900 (+40 delay-slot receiver) is NOT joined. Do
        //not borrow the legacy optional-native-progress callback policy.
        return stop(0x1aa230);
    }
    finishReturn.progressSkipped=true;
    at(0x1aa244);
    if(!continueSourceCollisionDispatch1AA244(pc))return stop(pc);
    if(!continueSourcePostCollisionCaller1AA258(pc))return stop(pc);
    return stop(pc); //No World-load completion is represented by this prefix.
}
bool Owner::continueSourcePostCollisionCaller1AA258(std::uint32_t& pc)noexcept {
    auto& h=sourcePostCollisionCaller_;
    if(pc!=0x1aa258||h.loadingMode30||!sourceCollisionDispatch128_.worldReturned||
       !preflightSourcePostParticleModelsNative()||nativeScbThread_!=std::this_thread::get_id()||
       closed_||terminal_||nativeScbRevoked_||!sourceLoadApplication_||!sourceLoadRace_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||!worldColdConstructed_||
       sourceLoadApplication_->entities()!=worldApplication_||sceneApplication_!=worldApplication_||
       worldEntity_.reference00!=1)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x1aa258);at(0x1aa25c); //Original replaces collision V0 with actual Application.
    at(0x1aa260);const auto& loading=sourceLoadApplication_->sourceLoading330Owner_;
    if(!loading||!loading->current()||loading->issuer_!=sourceLoadApplication_||
       !loading->managerInitialized_||!loading->slot330Published_||
       loading->retiring_||loading->retired_){at(0x1aa264);return false;}
    at(0x1aa264);h.loadingMode30=loading->mode30_;
    at(0x1aa268);at(0x1aa26c);
    if(*h.loadingMode30){at(0x1aa270);return false;} //Actual1CC900 branch remains unjoined.
    h.progressSkipped=true;
    at(0x1aa284);at(0x1aa288);at(0x1aa28c);at(0x1aa290);
    //Sign-extended addresses3AADA8 and36B860, not3BADA8/37B860.
    //Original LWC1/LH then SWC1/SH copies six bytes including the NUL.
    //Existing string backing is native storage; no source allocation invented.
    auto& directory=sourceLoadApplication_->modelDirectory36B860_;
    if(directory!="geometry\\"||directory.size()<5){at(0x1aa294);return false;}
    at(0x1aa294);at(0x1aa298);
    at(0x1aa29c);std::memcpy(directory.data(),"anim",4);h.animationDirectoryStoreMask=1;
    at(0x1aa2a0);directory[4]='\\';directory.resize(5);h.animationDirectoryStoreMask=3;
    at(0x1aa2a4);at(0x1aa2a8);h.worldTable08=0x3cf110u;
    at(0x1aa2ac);h.target120=0x1b40b0u; //Original3CF230.
    at(0x1aa2b0);at(0x1aa2b4);at(0x1b40b0);
    //Actual base2742F0 receives the SAME World, not a legacy toy-ready flag.
    at(0x1b40d4);at(0x1b40d8);at(0x2742f0);at(0x2742fc);
    if(sourceToyPhase_!=SourceToyPhase::returned||!sourceToyLoaderReturnedZero_||
       !preflightSourceWorldToyNative()||!sourceToy3C8_)return false;
    if(*sourceToy3C8_||nativeEmptyToyList3C8_)return false; //Nonnull constructor/service remains bounded.
    h.baseToyNullRead=true;
    at(0x274300);at(0x274304);at(0x274308);at(0x27430c);h.baseToyReturnV0=1;
    at(0x274354);at(0x274358);at(0x27435c);at(0x274360);at(0x1b40dc);
    at(0x1b40e0);if(!sourceToy3C8_||*sourceToy3C8_)return false;
    h.derivedToyNullRead=true;at(0x1b40e4);at(0x1b40e8); //Delay copies retained S1 to V0.
    for(std::uint32_t p=0x1b4304;p<=0x1b4328;p+=4)at(p);
    h.toyReturnV0=h.baseToyReturnV0;h.toyReturned=true;at(0x1aa2b8);
    at(0x1aa2bc);at(0x1aa2c0);h.target124=0x274370u; //Original3CF234.
    at(0x1aa2c4);at(0x1aa2c8);h.particleDispatched=true;at(0x274370);
    return continueSourceParticleService274370(pc);
}
bool Owner::continueSourceParticleService274370(std::uint32_t& pc)noexcept {
    auto& h=sourceParticleService124_;
    if(pc!=0x274370||h.entered||!sourcePostCollisionCaller_.particleDispatched||
       !preflightSourcePostParticleModelsNative()||nativeScbThread_!=std::this_thread::get_id()||
       closed_||terminal_||nativeScbRevoked_||!sourceLoadRace_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||!worldColdConstructed_||
       worldEntity_.reference00!=1)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    h.entered=true;at(0x274370);at(0x274374);at(0x274378); //A1 is the SAME World.
    at(0x27437c);
    if(sourceParticleDescriptorPhase_!=SourceParticleDescriptorPhase::returned||
       !preflightSourceWorldParticleDescriptorsNative()||!sourceParticleList3D8_||
       *sourceParticleList3D8_!=nativeParticleList3D8_.get())return false;
    h.listRead=true;h.listNonNull=bool(*sourceParticleList3D8_);
    at(0x274380);at(0x274384);
    //The admitted source2722F0 returned cohort is nonNULL. Do not invent a
    //NULL-list return from an allocation or native loader refusal.
    if(!h.listNonNull){at(0x274388);return false;}
    auto& list=*nativeParticleList3D8_;
    at(0x274390);h.listTable08=list.sourceVtable08;
    if(*h.listTable08!=0x3d24f0u)return false;
    at(0x274394);h.listTarget78=0x27dfd0u; //Original table word3D2568.
    at(0x274398);at(0x27439c);at(0x27dfd0);h.listEntered=true;
    //27DFE8 saves list inS3;27DFEC saves actual World argument inS2.
    at(0x27dff0);h.rowS0=0;at(0x27dff4);at(0x27dff8);h.offsetS1=0;
    at(0x27e020);h.count30=list.count30;
    if(*h.count30!=4)return false; //Existing exact source descriptor cohort only.
    at(0x27e024);at(0x27e028);at(0x27e02c);at(0x27dffc);
    if(!list.sourceArrayPublished||!list.sourceArray34||
       *list.sourceArray34!=list.pointers34.get()||!list.pointers34)return false;
    at(0x27e000);at(0x27e004);
    if(!list.sourceSlotPublished[0]||!list.sourceRowReturned[0]||!list.rows[0]||
       list.pointers34[0]!=list.rows[0].get())return false;
    h.rowRead=true;auto& row=*list.rows[0];
    at(0x27e008);h.rowTable14=row.sourceVtable14;
    if(*h.rowTable14!=0x3d2570u)return false;
    at(0x27e00c);h.rowTarget0C=0x27c7b0u; //Original3D257C.
    at(0x27e010);at(0x27e014);at(0x27c7b0);h.rowEntered=true;
    at(0x27c7dc);
    if(!row.sourceDescriptorPublished||!row.sourceDescriptor04||!row.descriptor04||
       *row.sourceDescriptor04!=row.descriptor04.get()||!row.descriptor04->sourceLoaded)return false;
    h.descriptorRead=true;const auto& d=*row.descriptor04;
    at(0x27c7e0);at(0x27c7e4);at(0x27c7e8);at(0x27c7ec);at(0x27c7f0);at(0x27c7f4);
    if(!d.sourceEmitterBytes||*d.sourceEmitterBytes>=d.sourceEmitter210.size()||
       d.sourceEmitter210[*d.sourceEmitterBytes]!=0)return false;
    //308F00 ASCII case fold, original literal3BB488="NONE". No locale/native
    //prepared emitter name or legacy decoded attachment is substituted.
    bool none=*d.sourceEmitterBytes==4;constexpr char target[]="none";
    for(std::size_t i=0;i<*d.sourceEmitterBytes;++i){
        auto c=static_cast<unsigned char>(d.sourceEmitter210[i]);
        if(!c||c>=128)return false;
        if(c>='A'&&c<='Z')c=static_cast<unsigned char>(c+32);
        if(i>=4||c!=static_cast<unsigned char>(target[i]))none=false;
    }
    h.emitterIsNone=none;at(0x27c7f8);at(0x27c7fc);
    if(none){at(0x27c800);return false;} //Diagnostic remains unjoined.
    at(0x27c814); //Reload the SAME source descriptor04, not a prepared emitter.
    if(!row.sourceMode||!d.sourceMode||!(d.sourceKnownMask&128u)||
       !row.sourceDescriptor04||*row.sourceDescriptor04!=row.descriptor04.get())return false;
    at(0x27c818);h.descriptorType04=static_cast<std::uint32_t>(d.type04);
    at(0x27c81c);if(!continueSourceParticleRowCreation27C81C(0,h,pc))return false;
    return continueSourceParticleRowAttachment27CC08(0,h,pc);
}
bool Owner::continueSourceParticleRowAttachment27CC08(std::size_t rowIndex,SourceParticleService124& h,std::uint32_t& pc)noexcept {
    if(pc!=0x27cc08||rowIndex>=4||!h.rowStart10Stored||h.attachmentOffsetE0||
       !nativeParticleList3D8_||!nativeParticleList3D8_->rows[rowIndex]||
       !preflightSourceWorldParticleDescriptorsNative())return false;
    if((rowIndex==0&& &h!=&sourceParticleService124_)||(rowIndex>0&& &h!=&sourceParticleLaterRows_[rowIndex-1]))return false;
    auto& row=*nativeParticleList3D8_->rows[rowIndex];
    if(!row.descriptor04)return false;const auto& d=*row.descriptor04;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x27cc08);
    if(!row.sourceDescriptor04||*row.sourceDescriptor04!=row.descriptor04.get()||d.sourceOffsetKnown!=15)return false;
    at(0x27cc14);at(0x27cc18);h.attachmentOffsetE0=d.sourceOffset320;at(0x27cc1c);
    const auto equalAscii=[](std::string_view left,std::string_view right)noexcept {
        if(left.size()!=right.size())return false;
        for(std::size_t i=0;i<left.size();++i){
            auto a=static_cast<unsigned char>(left[i]),b=static_cast<unsigned char>(right[i]);
            if(a>='A'&&a<='Z')a=static_cast<unsigned char>(a+32);
            if(b>='A'&&b<='Z')b=static_cast<unsigned char>(b+32);
            if(a!=b)return false;
        }return true;
    };
    if(!d.sourceAttachBytes||*d.sourceAttachBytes>=d.sourceAttach08.size()||d.sourceAttach08[*d.sourceAttachBytes])return false;
    const std::string_view attachName(d.sourceAttach08.data(),*d.sourceAttachBytes);
    at(0x27cc20);at(0x27cc24);at(0x27cc28);
    h.attachmentIsNone=equalAscii(attachName,"NONE");at(0x27cc2c);at(0x27cc30);
    //27D7A0 only reads the original call operands here. No emitter manager/
    //model pointer write, borrowed model lease, load or return is invented.
    const auto enterConfig=[&](bool modelNonNull,std::uint32_t callPC)noexcept {
        at(callPC);
        if(row.sourceVtable14!=0x3d2570u||!row.sourceEmitterArrayPublished||!row.sourceEmitterArray00||
           *row.sourceEmitterArray00!=row.sourceEmitterPointers.get()||!row.sourceEmitterPointers||
           h.attachmentMatches>=row.sourceEmitterPublishedSlots)return false;
        h.configEmitterIndex=h.attachmentMatches;
        const auto& emitter=row.sourceEmitters[*h.configEmitterIndex];
        if(row.sourceEmitterPointers[*h.configEmitterIndex]!=(emitter?&emitter->entity:nullptr))return false;
        h.configEmitterNonNull=bool(emitter);h.configModelNonNull=modelNonNull;
        h.configTarget10=0x27d7a0u;h.configEntered=true;at(0x27d7a0);at(0x27d7b4);
        if(row.unknown08!=h.emitterType)return false;
        struct Load {std::uint32_t type,readPC,callPC,target;};
        constexpr Load loads[]={ {0,0x27d81c,0x27d828,0x15b9d0},{1,0x27d8bc,0x27d8c8,0x15ce90},
            {2,0x27d96c,0x27d978,0x15d6d0},{8,0x27da0c,0x27da18,0x15def0},
            {9,0x27daac,0x27dab8,0x15e8f0},{11,0x27db4c,0x27db58,0x15f1e0},
            {12,0x27dbec,0x27dbf8,0x15ff10}};
        for(const auto& load:loads)if(load.type==*row.unknown08){
            at(load.readPC);std::size_t managerCount=0;
            if(!sourceParticleStageCountedNative(managerCount)||!sourceParticle4A0_||
               *sourceParticle4A0_!=trackParticles_.get()||!sourceParticleLoadReturned_||
               sourceParticleStageNativeClosed_||sourceParticle4A0NativeWithdrawn_)return false;
            h.configManagerRead=true;
            if(!row.sourceDescriptor04||*row.sourceDescriptor04!=row.descriptor04.get()||
               !d.sourceEmitterBytes||d.sourceOffsetKnown!=15)return false;
            at(load.callPC);h.configLoaderTarget=load.target;
            //Same source implementation, distinct exact row/emitter occurrence.
            if(load.type==0||load.type==1||load.type==2||load.type==8||load.type==9||load.type==11||load.type==12)return continueSourceEmitterLoaderPrefixNative(rowIndex,h,pc);
            return false; //No non-original subtype is admitted.
        }
        at(0x27dc5c);return false;
    };
    if(*h.attachmentIsNone){at(0x27d700);return enterConfig(false,0x27d718);}
    at(0x27cc34);h.attachmentMatches=0;at(0x27cc38);at(0x27cc3c);h.attachmentZone=0;
    if(!sourceStaticOwner_||sourceStaticOwner_->closed_||!sourceStaticOwner_->sourceMode_||
       sourceModelOwner3C4_!=sourceStaticOwner_.get()||sourceStaticOwner_->sourceWorld3C4Borrower_!=this||
       sourceStaticOwner_->application_!=worldApplication_||sourceStaticOwner_->generation_!=worldEntity_.sessionGeneration)return false;
    for(;;){
        at(0x27cfb0);if(!worldZones34C_)return false;h.attachmentLiveZones=*worldZones34C_;
        if(*h.attachmentLiveZones>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return false;
        at(0x27cfb4);at(0x27cfb8);at(0x27cfbc);at(0x27cfc0);
        if(*h.attachmentZone>=*h.attachmentLiveZones)break;
        const auto z=*h.attachmentZone;at(0x27cc40);at(0x27cc44);h.attachmentSlot=0;
        at(0x27cc4c);
        if(!sourceModels3C4_||!sourceModelParents3C4_||*sourceModels3C4_!=sourceModelParents3C4_.get()||
           z>=sourceModelParentsPublished3C4_||z>=sourceStaticOwner_->parents_.size()||
           !sourceStaticOwner_->parents_[z])return false;
        auto& parent=*sourceStaticOwner_->parents_[z];at(0x27cc54);
        if(sourceModelParents3C4_[z]!=&parent.entity||parent.released||!parent.sourceMode||!parent.sourceCountKnown)return false;
        for(;;){
            at(0x27cf78);at(0x27cf7c);h.attachmentLiveCount=parent.count30;
            if(*h.attachmentLiveCount>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return false;
            at(0x27cf80);at(0x27cf84);at(0x27cf88);
            if(*h.attachmentSlot>=*h.attachmentLiveCount)break;
            const auto slot=*h.attachmentSlot;at(0x27cc60);at(0x27cc68);
            if(!parent.sourceEntries34||slot>=parent.sourceEntriesKnown||
               z>=sourceStaticOwner_->sourceWrapperLedger_.size()||slot>=sourceStaticOwner_->sourceWrapperLedger_[z].size())return false;
            at(0x27cc70);auto* wrapper=parent.sourceEntries34[slot];
            if(wrapper!=sourceStaticOwner_->sourceWrapperLedger_[z][slot].get())return false;
            if(!wrapper){at(0x27cc78);++*h.attachmentSlot;continue;}
            at(0x27cc7c);if(!sourceStaticOwner_->sourceModelCurrentNative(*wrapper))return false;
            bool match=false;
            if(wrapper->dispatch==ra_static_model_cold50LA::Owner::StandardWrapper::Dispatch::physics3D26C0){
                const auto& physics=wrapper->sourcePhysics;if(!physics.body8)return false;
                if(*physics.body8){std::uint8_t byte=0;at(0x27cc90);
                    if(!physics.bodyOwner||!physics.bodyOwner->readSourceInlineName38FirstByteNative(
                        worldApplication_.get(),worldEntity_.sessionGeneration,*physics.body8,byte)||byte)return false;
                    //Genuine inline+38 constructor NUL; never the unrelated
                    //native Standard model8 alias or nested Physics Model14C.
                    if(attachName.empty()){h.attachmentMatched=true;h.attachmentPhysics=true;at(0x27cca8);return false;}
                }
            }else if(wrapper->model8){
                auto* frame=wrapper->sourceModel.get();
                if(!frame||wrapper->model8!=&frame->scene||frame->nativeRetired||frame->scene.released())return false;
                const auto* model=frame->scene.model();if(!model)return false;
                at(0x27cc88);at(0x27cc90);match=equalAscii(model->name38,attachName);at(0x27cc98);
            }
            if(match){
                h.attachmentMatched=true;at(0x27cca0);at(0x27cca8);
                if(!d.sourceChildBytes||*d.sourceChildBytes>=d.sourceChild10C.size()||d.sourceChild10C[*d.sourceChildBytes])return false;
                at(0x27ccb0);h.childIsNone=equalAscii(std::string_view(d.sourceChild10C.data(),*d.sourceChildBytes),"NONE");
                at(0x27ccb8);at(0x27ccbc);
                if(!*h.childIsNone){at(0x27ccc8);h.childLookupPending=true;return false;} //28C590 graph lookup still unjoined.
                at(0x27ccd8);
                std::uint32_t call=0;switch(*h.emitterType){case 0:call=0x27cd54;break;case 1:call=0x27cda8;break;
                case 2:call=0x27cdfc;break;case 8:call=0x27ce4c;break;case 9:call=0x27ce98;break;
                case 11:call=0x27cee4;break;case 12:call=0x27cf30;break;default:return false;}
                return enterConfig(true,call);
            }
            at(0x27cf70);++*h.attachmentSlot;at(0x27cf74);
        }
        at(0x27cf90);if(d.instances330!=h.instances330Read)return false;
        at(0x27cfa4);at(0x27cfa8);++*h.attachmentZone;at(0x27cfac);
    }
    at(0x27cfc8);if(!sourceToy3C8_||!preflightSourceWorldToyNative())return false;
    at(0x27cfcc);at(0x27cfd0);at(0x27cfd4);
    if(*sourceToy3C8_){at(0x27cfdc);return false;}
    h.toyScanNullRead=true;at(0x27d680);return false; //Distinct second guard remains unjoined.
}
bool Owner::continueSourceParticleRowCreation27C81C(std::size_t rowIndex,SourceParticleService124& h,std::uint32_t& pc)noexcept {
    if(pc!=0x27c81c||rowIndex>=4||h.rowType08Stored||!h.rowEntered||!h.descriptorRead||
       h.emitterIsNone!=false||!h.descriptorType04||!nativeParticleList3D8_||
       !nativeParticleList3D8_->rows[rowIndex]||!preflightSourceWorldParticleDescriptorsNative())return false;
    if((rowIndex==0&& &h!=&sourceParticleService124_)||(rowIndex>0&& &h!=&sourceParticleLaterRows_[rowIndex-1]))return false;
    auto& row=*nativeParticleList3D8_->rows[rowIndex];
    if(!row.descriptor04||!row.sourceDescriptor04||*row.sourceDescriptor04!=row.descriptor04.get())return false;
    const auto& d=*row.descriptor04;
    if(!d.sourceLoaded||!(d.sourceKnownMask&128u)||*h.descriptorType04!=static_cast<std::uint32_t>(d.type04))return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x27c81c);row.unknown08=h.descriptorType04;h.rowType08Stored=true;
    at(0x27c820);
    if(!worldApplication_||!sourceLoadApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||sceneApplication_!=worldApplication_||
       !worldApplication_->constructed_||worldApplication_->released_)return false;
    auto& shared=g::OriginalApplicationEntities50LA::processParticleAssertion27C820();
    if(!shared.known||shared.thread!=std::this_thread::get_id()||
       shared.guard3D6528>1||shared.assertion443068!=shared.guard3D6528)return false;
    h.guard3D6528Read=shared.guard3D6528;at(0x27c824);at(0x27c828);
    if(!*h.guard3D6528Read){
        at(0x27c82c);at(0x27c830);at(0x27c834);shared.assertion443068=1;
        at(0x27c838);shared.guard3D6528=1;h.assertionLazyStored=true;
    }
    at(0x27c83c);at(0x27c840);h.assertion443068Read=shared.assertion443068;
    at(0x27c844);at(0x27c848);at(0x27c84c);
    //Read actual World4A0, whose1B4E54 publication and160E00 same-self
    //return are retained. No legacy render-query/native manager adoption.
    std::size_t managerCount=0;
    if(!sourceParticleStageCountedNative(managerCount)||sourceParticleStageNativeClosed_||
       sourceParticle4A0NativeWithdrawn_||!sourceParticlePublished4A0_||
       !sourceParticleLoadReturned_||!sourceParticle4A0_||
       *sourceParticle4A0_!=trackParticles_.get()||nativeParticleManager4A0_!=*sourceParticle4A0_)return false;
    h.manager4A0Read=true;h.manager4A0NonNull=bool(*sourceParticle4A0_);
    if(!h.manager4A0NonNull)return false; //NULL assertion/diagnostic path remains bounded.
    at(0x27c850);at(0x27c854);at(0x27c858);at(0x27c85c);
    //Actual133660 returns0 for a nonNULL argument on BOTH debug36C100
    //branches:13367C or133684. No debug-global default or logging is invented.
    h.assertionReturnV0=0;at(0x27c860);at(0x27c864);at(0x27c888);
    if(!row.sourceDescriptor04||*row.sourceDescriptor04!=row.descriptor04.get()||
       !d.sourceLoaded||!(d.sourceKnownMask&512u)||!d.instances330)return false;
    at(0x27c88c);h.instances330Read=d.instances330;at(0x27c890);
    at(0x27c894);
    if(*h.instances330Read<=0){at(0x27cbf4);return false;} //Unjoined diagnostic, not success.
    h.countPositive=true;at(0x27c898);
    if(!row.unknown08||row.unknown08!=h.descriptorType04)return false;
    const auto type=*row.unknown08;
    struct Branch {std::uint32_t type,bytes,ctor,table,arrayPC,loopPC,allocationPC,storePC,publishPC;};
    constexpr Branch branches[]={
        {0,0x260,0x15b860,0x3cdcf0,0x27c8f8,0x27c940,0x27c90c,0x27c938,0x27c95c},
        {1,0x210,0x15cdc0,0x3cdc50,0x27c964,0x27c9b0,0x27c978,0x27c9a8,0x27c9cc},
        {2,0x230,0x15d5e0,0x3cdbb0,0x27c9d4,0x27ca20,0x27c9e8,0x27ca18,0x27ca3c},
        {8,0x230,0x15de00,0x3cdb10,0x27ca44,0x27ca88,0x27ca58,0x27ca84,0x27caa0},
        {9,0x230,0x15e800,0x3cda70,0x27caa8,0x27cae8,0x27cabc,0x27cae4,0x27cb00},
        {11,0x240,0x15f100,0x3cd9d0,0x27cb08,0x27cb48,0x27cb1c,0x27cb44,0x27cb60},
        {12,0x240,0x15fe30,0x3cd930,0x27cb68,0x27cba8,0x27cb7c,0x27cba4,0x27cbc0}};
    const Branch* branch=nullptr;for(const auto& b:branches)if(b.type==type){branch=&b;break;}
    if(!branch){at(0x27cbc4);return false;} //Actual unsupported-type diagnostic remains unjoined.
    h.emitterType=type;h.emitterAllocationBytes=branch->bytes;h.emitterConstructor=branch->ctor;
    //Host pointer width differs from EE count*4. Refuse wrapping guest byte
    //counts before allocation; this is a bounded native safety adaptation.
    const auto count=static_cast<std::uint32_t>(*h.instances330Read);
    if(count>0x3fffffffu)return false;
    at(branch->arrayPC);
    //Owner metadata is allocated before guest effects; failures create no
    //fabricated guest NULL publication. Actual raw-pointer failure is distinct.
    try{row.sourceEmitters.resize(count);}catch(...){return false;}
    row.sourceEmitterArrayAttempted=true;h.emitterArrayAttempted=true;
    row.sourceEmitterPointers.reset(new(std::nothrow) Entity*[count]);
    for(std::size_t index=0;;++index){
        at(branch->loopPC);
        if(!row.sourceDescriptor04||*row.sourceDescriptor04!=row.descriptor04.get()||
           !d.instances330||d.instances330!=h.instances330Read)return false;
        if(index>=static_cast<std::size_t>(*d.instances330))break;
        at(branch->allocationPC);
        auto& owner=row.sourceEmitters[index];
        owner.reset(new(std::nothrow) SourceEmitter27C890);
        ++row.sourceEmitterAttempts;h.emitterAttempts=row.sourceEmitterAttempts;
        if(owner){auto& e=*owner;at(branch->ctor);
            if(!constructSourceEmitterEntity15A5B0(e.entity,e.counted,e.enrolled,pc)){
                sourcePostParticleModelsPC_=pc;return false;
            }
            //Actual15A5B0 constructor stores only. Unknown lanes/upper flag
            //bits remain unknown; manager70/attachment74/aux1C4 are knownNULL.
            e.dispatch08=0x3cdd90;
            for(auto off:{0x30u,0x40u,0x50u,0x60u})e.vector(off);
            e.words[0x1d4/4]=1;e.known[0x1d4/4]=7;
            e.word(0x70,0);e.word(0x74,0);e.word(0x17c,0x457a0000);e.word(0x180,0x40c00000);
            e.word(0x184,0x3f800000);e.word(0x188,0x41200000);e.word(0x18c,0);
            for(auto off:{0x1ccu,0x1d0u,0x190u,0x198u,0x19cu,0x1a0u,0x1b0u,0x1b4u})e.word(off,0);
            e.word(0x194,0x00ffffff);e.word(0x1b8,0xffffffff);
            for(std::uint32_t off=0x1d8;off<=0x1f4;off+=4)e.word(off,0);
            at(0x15a70c);if(!e.forcedSets1C4.constructorNull15A70C(&e.entity))return false;
            e.word(0x1c4,0); // Actual source NULL, with separate future typed-array custody.
            e.dispatch08=branch->table;
            if(type==0){
                constexpr std::uint32_t values[]={0,0x3f800000,0,0x40a00000,0x3c23d70a,0x43fa0000,
                    0x455ac000,0x3f000000,0x3f800000,0x40a00000,0,0,0x3f800000,0x3c23d70a,0,0x41c80000};
                for(std::uint32_t n=0;n<16;++n)e.word(0x200+n*4,values[n]);
                e.vector(0x240);e.vector(0x250);
            }else if(type==1){e.word(0x200,0);e.word(0x204,0);}
            else if(type==2){e.vector(0x210);e.vector(0x220);e.vector(0x40);e.word(0x19c,0);e.word(0x200,0);}
            else if(type==11){e.vector(0x220);e.vector(0x230);e.vector(0x40);e.word(0x19c,0);}
            else {e.vector(0x200);e.vector(0x210);e.vector(0x40);e.word(0x19c,0);
                if(type==8){e.word(0x228,1);e.word(0x224,0);}
                if(type==9){e.word(0x224,1);e.word(0x1c4,0);}}
            e.constructorReturned=true;
        }
        at(branch->storePC);
        if(!row.sourceEmitterPointers)return false; //Retain even this unpublishable counted owner.
        row.sourceEmitterPointers[index]=owner?&owner->entity:nullptr;
        ++row.sourceEmitterPublishedSlots;h.emitterPublishedSlots=row.sourceEmitterPublishedSlots;
    }
    at(branch->publishPC);row.sourceEmitterArray00=row.sourceEmitterPointers.get();
    row.sourceEmitterArrayPublished=true;h.emitterArrayPublished=true;
    at(0x27cbd8);at(0x27cbdc);
    if(!d.instances330||d.instances330!=h.instances330Read)return false;
    at(0x27cbe0);row.unknown0C=static_cast<std::uint32_t>(*d.instances330);h.rowCount0CStored=true;
    at(0x27cbe4);at(0x27cbe8);
    if(!(d.sourceKnownMask&1024u)||!d.startOn334)return false;
    at(0x27cbec);at(0x27cbf0);row.unknown10=static_cast<std::uint32_t>(*d.startOn334);h.rowStart10Stored=true;
    at(0x27cc08);return true; //Only source creation/count/start stores; not configuration/row success.
}
bool Owner::continueSourceEmitterLoaderPrefixNative(std::size_t rowIndex,SourceParticleService124& h,std::uint32_t& pc)noexcept {
    if(rowIndex>=4||(rowIndex==0&& &h!=&sourceParticleService124_)||
       (rowIndex>0&& &h!=&sourceParticleLaterRows_[rowIndex-1]))return false;
    const bool subtype0=h.emitterType==0u,subtype1=h.emitterType==1u,subtype2=h.emitterType==2u,subtype9=h.emitterType==9u,subtype11=h.emitterType==11u,subtype12=h.emitterType==12u;
    if((!subtype0&&!subtype1&&!subtype2&&!subtype9&&!subtype11&&!subtype12&&h.emitterType!=8u)||pc!=(subtype0?0x27d828u:(subtype1?0x27d8c8u:(subtype2?0x27d978u:(subtype9?0x27dab8u:(subtype11?0x27db58u:(subtype12?0x27dbf8u:0x27da18u))))))||
       h.configLoaderTarget!=(subtype0?0x15b9d0u:(subtype1?0x15ce90u:(subtype2?0x15d6d0u:(subtype9?0x15e8f0u:(subtype11?0x15f1e0u:(subtype12?0x15ff10u:0x15def0u))))))||
       (subtype0?h.type0LoadEntered:(subtype1?h.type1LoadEntered:(subtype2?h.type2LoadEntered:(subtype9?h.type9LoadEntered:(subtype11?h.type11LoadEntered:(subtype12?h.type12LoadEntered:h.type8LoadEntered))))))||!h.configEntered||!h.configManagerRead||!h.configEmitterIndex||
       !nativeParticleList3D8_||!nativeParticleList3D8_->rows[rowIndex])return false;
    auto& row=*nativeParticleList3D8_->rows[rowIndex];
    if(!row.descriptor04||!row.sourceDescriptor04||*row.sourceDescriptor04!=row.descriptor04.get()||
       *h.configEmitterIndex>=row.sourceEmitterPublishedSlots||!row.sourceEmitterPointers)return false;
    const auto& d=*row.descriptor04;
    auto& owner=row.sourceEmitters[*h.configEmitterIndex];
    if(row.sourceEmitterPointers[*h.configEmitterIndex]!=(owner?&owner->entity:nullptr))return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(subtype0?0x15b9d0u:(subtype1?0x15ce90u:(subtype2?0x15d6d0u:(subtype9?0x15e8f0u:(subtype11?0x15f1e0u:(subtype12?0x15ff10u:0x15def0u))))));(subtype0?h.type0LoadEntered:(subtype1?h.type1LoadEntered:(subtype2?h.type2LoadEntered:(subtype9?h.type9LoadEntered:(subtype11?h.type11LoadEntered:(subtype12?h.type12LoadEntered:h.type8LoadEntered))))))=true;
    at(subtype0?0x15b9fcu:(subtype1?0x15cebcu:(subtype2?0x15d6fcu:(subtype9?0x15e91cu:(subtype11?0x15f20cu:(subtype12?0x15ff3cu:0x15df1cu)))))); //141C20 JR RA, delay copies A0→V0. No retain/disable effects.
    at(subtype0?0x15ba04u:(subtype1?0x15cec4u:(subtype2?0x15d704u:(subtype9?0x15e924u:(subtype11?0x15f214u:(subtype12?0x15ff44u:0x15df24u))))));(subtype0?h.type0NoOpReturned:(subtype1?h.type1NoOpReturned:(subtype2?h.type2NoOpReturned:(subtype9?h.type9NoOpReturned:(subtype11?h.type11NoOpReturned:(subtype12?h.type12NoOpReturned:h.type8NoOpReturned))))))=true;
    at(subtype0?0x15ba08u:(subtype1?0x15cec8u:(subtype2?0x15d708u:(subtype9?0x15e928u:(subtype11?0x15f218u:(subtype12?0x15ff48u:0x15df28u))))));if(!owner)return false; //Actual first NULL receiver store remains bounded.
    auto& e=*owner;
    if(!e.constructorReturned||e.dispatch08!=(subtype0?0x3cdcf0u:(subtype1?0x3cdc50u:(subtype2?0x3cdbb0u:(subtype9?0x3cda70u:(subtype11?0x3cd9d0u:(subtype12?0x3cd930u:0x3cdb10u))))))||e.managerStored||e.modelStored||e.borrowsWithdrawn||
       !trackParticles_||!sourceParticle4A0_||*sourceParticle4A0_!=trackParticles_.get()||
       sourceParticleStageNativeClosed_||sourceParticle4A0NativeWithdrawn_||
       trackParticles_->sourceParticleEmitterBorrowers_==std::numeric_limits<std::size_t>::max())return false;
    //A native lease protects the same actual source4A0 while emitter70 exists.
    //It is NOT an original retain, manager registration, or readiness token.
    ++trackParticles_->sourceParticleEmitterBorrowers_;e.manager70=trackParticles_.get();
    e.managerStored=true;e.known[0x70/4]=0;(subtype0?h.type0ManagerStored:(subtype1?h.type1ManagerStored:(subtype2?h.type2ManagerStored:(subtype9?h.type9ManagerStored:(subtype11?h.type11ManagerStored:(subtype12?h.type12ManagerStored:h.type8ManagerStored))))))=true;
    if(h.configModelNonNull){
        at(subtype0?0x15ba0cu:(subtype1?0x15ceccu:(subtype2?0x15d70cu:(subtype9?0x15e92cu:(subtype11?0x15f21cu:(subtype12?0x15ff4cu:0x15df2cu))))));
        if(!h.attachmentZone||!h.attachmentSlot||!sourceStaticOwner_||h.attachmentPhysics||
           *h.attachmentZone>=sourceStaticOwner_->sourceWrapperLedger_.size()||
           *h.attachmentSlot>=sourceStaticOwner_->sourceWrapperLedger_[*h.attachmentZone].size())return false;
        auto& wrapper=sourceStaticOwner_->sourceWrapperLedger_[*h.attachmentZone][*h.attachmentSlot];
        if(!wrapper||!sourceStaticOwner_->sourceModelCurrentNative(*wrapper)||!wrapper->sourceModel||
           wrapper->model8!=&wrapper->sourceModel->scene||wrapper->sourceModel->nativeRetired)return false;
        auto& model=wrapper->sourceModel->scene;
        if(!model.model()||model.sourceParticleEmitterBorrowers_==std::numeric_limits<std::size_t>::max())return false;
        at(subtype0?0x15ba10u:(subtype1?0x15ced0u:(subtype2?0x15d710u:(subtype9?0x15e930u:(subtype11?0x15f220u:(subtype12?0x15ff50u:0x15df30u))))));++model.sourceParticleEmitterBorrowers_;
        e.modelOwner=&model;e.model74=model.model();e.modelStored=true;e.known[0x74/4]=0;(subtype0?h.type0ModelStored:(subtype1?h.type1ModelStored:(subtype2?h.type2ModelStored:(subtype9?h.type9ModelStored:(subtype11?h.type11ModelStored:(subtype12?h.type12ModelStored:h.type8ModelStored))))))=true;
        at(subtype0?0x15ba14u:(subtype1?0x15ced4u:(subtype2?0x15d714u:(subtype9?0x15e934u:(subtype11?0x15f224u:(subtype12?0x15ff54u:0x15df34u))))));if(d.sourceOffsetKnown!=15)return false;
        at(subtype0?0x15ba18u:(subtype1?0x15ced8u:(subtype2?0x15d718u:(subtype9?0x15e938u:(subtype11?0x15f228u:(subtype12?0x15ff58u:0x15df38u))))));for(std::size_t i=0;i<4;++i)e.word(0x50+static_cast<std::uint32_t>(i)*4,d.sourceOffset320[i]);
        e.offsetStored=true;(subtype0?h.type0OffsetStored:(subtype1?h.type1OffsetStored:(subtype2?h.type2OffsetStored:(subtype9?h.type9OffsetStored:(subtype11?h.type11OffsetStored:(subtype12?h.type12OffsetStored:h.type8OffsetStored))))))=true;
    }
    if(subtype12){
        at(0x15ff5c);at(0x15ff60);
        if(h.configModelNonNull){
            at(0x15ff64);at(0x15ff68);at(0x15ff6c);
            e.type12AttachmentPositionPending=true;
            //Reauthenticate this selected Standard wrapper, not a prepared
            //matrix or an earlier finish-line/28B550 getter occurrence.
            const auto attachedFrame=[&]()noexcept -> ra_static_model_cold50LA::Owner::SourceModelFrame* {
                if(!h.attachmentZone||!h.attachmentSlot||h.attachmentPhysics||!sourceStaticOwner_||
                   sourceStaticOwner_->closed_||!sourceStaticOwner_->sourceMode_||
                   sourceModelOwner3C4_!=sourceStaticOwner_.get()||sourceStaticOwner_->sourceWorld3C4Borrower_!=this||
                   sourceStaticOwner_->application_!=worldApplication_||sourceStaticOwner_->generation_!=worldEntity_.sessionGeneration||
                   !sourceModels3C4_||!sourceModelParents3C4_||*sourceModels3C4_!=sourceModelParents3C4_.get())return nullptr;
                const auto zone=*h.attachmentZone,slot=*h.attachmentSlot;
                if(zone>=sourceModelParentsPublished3C4_||zone>=sourceStaticOwner_->parents_.size()||
                   !sourceStaticOwner_->parents_[zone]||zone>=sourceStaticOwner_->sourceWrapperLedger_.size()||
                   slot>=sourceStaticOwner_->sourceWrapperLedger_[zone].size())return nullptr;
                const auto& parent=*sourceStaticOwner_->parents_[zone];
                if(sourceModelParents3C4_[zone]!=&parent.entity||parent.released||!parent.sourceMode||!parent.sourceCountKnown||
                   slot>=parent.count30||slot>=parent.sourceEntriesKnown||!parent.sourceEntries34)return nullptr;
                auto& wrapper=sourceStaticOwner_->sourceWrapperLedger_[zone][slot];
                if(!wrapper||parent.sourceEntries34[slot]!=wrapper.get()||!wrapper->sourceMode||!wrapper->sourceTypeKnown||
                   wrapper->type00!=1||wrapper->sourceVtable04!=0x3d26f0u||
                   wrapper->dispatch!=ra_static_model_cold50LA::Owner::StandardWrapper::Dispatch::standard3D26F0||
                   !sourceStaticOwner_->sourceModelCurrentNative(*wrapper)||!wrapper->sourceModel||
                   wrapper->model8!=e.modelOwner||&wrapper->sourceModel->scene!=e.modelOwner||
                   wrapper->sourceModel->nativeRetired||!wrapper->sourceModel->parentAttached||
                   !wrapper->sourceModel->parentAttachReturned||e.modelOwner->model()!=e.model74)return nullptr;
                return wrapper->sourceModel.get();
            };
            auto* frame=attachedFrame();if(!frame)return false;
            at(0x15ff70); //a1=NULL in the actual28B1A0 call delay.
            const auto position=e.modelOwner->copySourcePosition28B1A0(frame->graph(),
                e.type12AttachmentPositionXYZ,e.type12AttachmentPosition);
            //Mirror each actual getter destination store, including a bounded
            //partial getter; W is never overwritten by28B1A0.
            constexpr std::uint32_t storeXYZ[]={0x28b268,0x28b270,0x28b27c};
            for(std::size_t lane=0;lane<3;++lane)if(e.type12AttachmentPosition.outputMask&(1u<<lane)){
                at(storeXYZ[lane]);e.word(0x30+static_cast<std::uint32_t>(lane)*4,e.type12AttachmentPositionXYZ[lane]);
            }
            at(position.sourcePC);if(!position.okay())return false;
            at(0x15ff74);frame=attachedFrame();if(!frame)return false;
            at(0x15ff78);at(0x15ff7c);at(0x15ff80);
            std::array<std::uint32_t,4> offset;
            for(std::size_t lane=0;lane<4;++lane){if(e.known[0x50/4+lane]!=0xffffffffu)return false;offset[lane]=e.words[0x50/4+lane];}
            const auto direction=e.modelOwner->transformSourceDirection28B9B0(frame->graph(),offset,
                e.type12AttachmentDirectionXYZ,e.type12AttachmentDirection);
            at(direction.sourcePC);if(!direction.okay())return false;
            //Bounded native scalar ADD.S sequence: finite operands/results,
            //nearest rounding, no contraction. Prior XYZ stores survive a
            //later refusal; no guest exceptional arithmetic is substituted.
            constexpr std::uint32_t addPC[]={0x15ff8c,0x15ff9c,0x15ffac};
            constexpr std::uint32_t resultPC[]={0x15ff90,0x15ffa0,0x15ffb0};
            for(std::size_t lane=0;lane<3;++lane){
                at(addPC[lane]-8);if(e.known[0x30/4+lane]!=0xffffffffu)return false;
                at(addPC[lane]-4);float current=0,delta=0;
                std::memcpy(&current,&e.words[0x30/4+lane],4);std::memcpy(&delta,&e.type12AttachmentDirectionXYZ[lane],4);
                at(addPC[lane]);if(std::fegetround()!=FE_TONEAREST||!std::isfinite(current)||!std::isfinite(delta))return false;
                volatile float sum=current+delta;const float value=sum;if(!std::isfinite(value))return false;
                std::uint32_t bits=0;std::memcpy(&bits,&value,4);
                at(resultPC[lane]);e.word(0x30+static_cast<std::uint32_t>(lane)*4,bits);
                e.type12AttachmentAddResult[lane]=bits;e.type12AttachmentAddMask|=static_cast<std::uint8_t>(1u<<lane);
            }
            at(0x15ffb4);std::array<std::uint32_t,4> current;
            for(std::size_t lane=0;lane<4;++lane){if(e.known[0x30/4+lane]!=0xffffffffu)return false;current[lane]=e.words[0x30/4+lane];}
            at(0x15ffb8);at(0x15ffbc);
            for(std::size_t lane=0;lane<4;++lane)e.word(0x40+static_cast<std::uint32_t>(lane)*4,current[lane]);
            e.type12AttachmentPreviousStored=true;e.type12AttachmentPositionPending=false;
        }else{
            //Unlike the other loaders, Fog positions BEFORE opening Config.
            //This is one LQ and two SQs, never two15ACB0 calls or wrapper success.
            at(0x15ffc0);if(d.sourceOffsetKnown!=15||e.type12PositionMask)return false;
            e.type12PositionInput=d.sourceOffset320;e.type12PositionMask=1;
            at(0x15ffc4);for(std::size_t lane=0;lane<4;++lane)
                e.word(0x30+static_cast<std::uint32_t>(lane)*4,e.type12PositionInput[lane]);
            e.type12PositionMask=3;
            at(0x15ffc8);for(std::size_t lane=0;lane<4;++lane)
                e.word(0x40+static_cast<std::uint32_t>(lane)*4,e.type12PositionInput[lane]);
            e.type12PositionMask=7;
        }
    }
    at(subtype0?0x15ba1cu:(subtype1?0x15cedcu:(subtype2?0x15d71cu:(subtype9?0x15e93cu:(subtype11?0x15f22cu:(subtype12?0x15ffccu:0x15df3cu))))));at(subtype0?0x15ba20u:(subtype1?0x15cee0u:(subtype2?0x15d720u:(subtype9?0x15e940u:(subtype11?0x15f230u:(subtype12?0x15ffd0u:0x15df40u)))))); //Genuine source4A0 is nonNULL, no default-only branch.
    at(subtype0?0x15ba50u:(subtype1?0x15cf30u:(subtype2?0x15d798u:(subtype9?0x15e9d4u:(subtype11?0x15f2a0u:(subtype12?0x160060u:0x15dfd4u))))));const auto* directory=sourceParticleDirectory36B530(worldApplication_.get());
    if(!directory||directory->find('\0')!=std::string::npos||directory->size()>=e.filename78.size())return false;
    at(subtype0?0x15ba5cu:(subtype1?0x15cf3cu:(subtype2?0x15d7a4u:(subtype9?0x15e9e0u:(subtype11?0x15f2acu:(subtype12?0x16006cu:0x15dfe0u)))))); //Actual309230 strcpy, including terminating NUL.
    std::memcpy(e.filename78.data(),directory->data(),directory->size());e.filename78[directory->size()]=0;
    e.filenameBytes=directory->size();e.directoryCopied=true;(subtype0?h.type0DirectoryCopied:(subtype1?h.type1DirectoryCopied:(subtype2?h.type2DirectoryCopied:(subtype9?h.type9DirectoryCopied:(subtype11?h.type11DirectoryCopied:(subtype12?h.type12DirectoryCopied:h.type8DirectoryCopied))))))=true;
    at(subtype0?0x15ba60u:(subtype1?0x15cf40u:(subtype2?0x15d7a8u:(subtype9?0x15e9e4u:(subtype11?0x15f2b0u:(subtype12?0x160070u:0x15dfe4u)))))); //Repeated SAME manager70 store in the call delay slot, not a second lease.
    at(subtype0?0x15ba64u:(subtype1?0x15cf44u:(subtype2?0x15d7acu:(subtype9?0x15e9e8u:(subtype11?0x15f2b4u:(subtype12?0x160074u:0x15dfe8u))))));at(subtype0?0x15ba68u:(subtype1?0x15cf48u:(subtype2?0x15d7b0u:(subtype9?0x15e9ecu:(subtype11?0x15f2b8u:(subtype12?0x160078u:0x15dfecu))))));
    if(!d.sourceEmitterBytes||*d.sourceEmitterBytes>=d.sourceEmitter210.size()||d.sourceEmitter210[*d.sourceEmitterBytes])return false;
    for(std::size_t i=0;i<*d.sourceEmitterBytes;++i)if(!d.sourceEmitter210[i])return false;
    at(subtype0?0x15ba94u:(subtype1?0x15cf74u:(subtype2?0x15d7dcu:(subtype9?0x15ea18u:(subtype11?0x15f2e4u:(subtype12?0x1600a4u:0x15e018u))))));
    if(*d.sourceEmitterBytes>=e.filename78.size()-*e.filenameBytes)return false; //Native overflow guard, no truncation.
    std::memcpy(e.filename78.data()+*e.filenameBytes,d.sourceEmitter210.data(),*d.sourceEmitterBytes+1);
    *e.filenameBytes+=*d.sourceEmitterBytes;e.filenameAppended=true;(subtype0?h.type0FilenameAppended:(subtype1?h.type1FilenameAppended:(subtype2?h.type2FilenameAppended:(subtype9?h.type9FilenameAppended:(subtype11?h.type11FilenameAppended:(subtype12?h.type12FilenameAppended:h.type8FilenameAppended))))))=true;
    at(subtype0?0x15ba9cu:(subtype1?0x15cf7cu:(subtype2?0x15d7e4u:(subtype9?0x15ea20u:(subtype11?0x15f2ecu:(subtype12?0x1600acu:0x15e020u))))));at(subtype0?0x15baa0u:(subtype1?0x15cf80u:(subtype2?0x15d7e8u:(subtype9?0x15ea24u:(subtype11?0x15f2f0u:(subtype12?0x1600b0u:0x15e024u))))));
    return continueSourceEmitterConfig15AE00(rowIndex,h,pc);
}
std::optional<std::size_t> Owner::sourceNonFogPrecedingFogRowNative(std::size_t rowIndex)const noexcept {
    if(rowIndex<1||rowIndex>=4)return std::nullopt;
    const auto supported=[](const SourceParticleService124& service)noexcept {
        return service.emitterType==0u||service.emitterType==1u||service.emitterType==2u||
            service.emitterType==8u||service.emitterType==9u||service.emitterType==11u;
    };
    if(!supported(sourceParticleLaterRows_[rowIndex-1]))return std::nullopt;
    //Structural classification only. Never dereference Track, descriptors or
    //borrowed emitters here: historical preflight also runs after withdrawal.
    for(std::size_t prior=rowIndex;prior>0;){
        --prior;const auto& service=prior?sourceParticleLaterRows_[prior-1]:sourceParticleService124_;
        if(service.emitterType==12u)return prior;
        if(!supported(service))return std::nullopt;
    }
    return std::nullopt;
}
bool Owner::sourceNonFogAfterFogCustodyCurrentNative(std::size_t rowIndex)const noexcept {
    const auto fogRowIndex=sourceNonFogPrecedingFogRowNative(rowIndex);
    if(!fogRowIndex||nativeScbThread_!=std::this_thread::get_id()||!nativeParticleList3D8_)return false;
    const auto& currentService=sourceParticleLaterRows_[rowIndex-1];
    const auto& currentRow=nativeParticleList3D8_->rows[rowIndex];
    if(!currentRow||currentService.configEmitterIndex!=0u||
       currentRow->sourceEmitters.empty()||!currentRow->sourceEmitters[0])return false;
    const auto& e=*currentRow->sourceEmitters[0];
    if(!e.constructorReturned||!e.counted||!e.enrolled||e.nativeTopologyWithdrawn||
       !e.managerStored||e.borrowsWithdrawn||!e.manager70||e.manager70!=trackParticles_.get())return false;
    //Resolve every completed occurrence through its actual retained row. An
    //intervening non-Fog row cannot replace the Fog's count receipt or owners.
    for(std::size_t i=*fogRowIndex;i<rowIndex;++i){
        const auto& service=i?sourceParticleLaterRows_[i-1]:sourceParticleService124_;
        const auto& row=nativeParticleList3D8_->rows[i];
        if(!row||!row->sourceRowServiceReturned||!service.rowServiceReturned||
           !row->sourceDescriptorFreed||!row->sourceDescriptorHistory||!service.descriptorFreed||
           service.configEmitterIndex!=0u||row->sourceEmitters.empty()||!row->sourceEmitters[0])return false;
        const auto& completed=*row->sourceEmitters[0];
        const bool loaderReturned=service.emitterType==12u?completed.type12LoaderReturned:
            (service.emitterType==0u?completed.type0LoaderReturned:(service.emitterType==1u?
             completed.type1LoaderReturned:(service.emitterType==2u?completed.type2LoaderReturned:
             (service.emitterType==9u?completed.type9LoaderReturned:(service.emitterType==11u?
              completed.type11LoaderReturned:completed.type8LoaderReturned)))));
        if(&completed==&e||!completed.constructorReturned||!completed.counted||!completed.enrolled||
           !completed.sourceRegistered||completed.nativeTopologyWithdrawn||!completed.rowConfigReturned||
           !loaderReturned||!completed.emitterFileLoadReturned||!completed.configNativeWithdrawn||
           completed.borrowsWithdrawn||!completed.managerStored||completed.manager70!=e.manager70||
           completed.entity.sessionGeneration!=e.entity.sessionGeneration)return false;
    }
    const auto& fog=*nativeParticleList3D8_->rows[*fogRowIndex]->sourceEmitters[0];
    const auto& population=fog.type12Population;const auto& manager=*e.manager70;
    if(fog.dispatch08!=0x3cd930u||population.manager!=e.manager70||!population.returned||
       population.pc!=0x160b00u||fog.type12PopulationPending||!manager.sourceActiveCount128Known_)return false;
    //Producer-only native epoch check. Never called by historical preflight;
    //this records no new original128 read and never dereferences a receipt identity.
    const bool fogSuccessor=*fogRowIndex>0&&(*fogRowIndex==1?sourceParticleService124_:
        sourceParticleLaterRows_[*fogRowIndex-2]).emitterType==12u;
    if(fogSuccessor)return population.countStep==4&&population.countStore.stored&&
        manager.sourceActiveCount128_==2&&manager.sourceFogSuccessorCountReceiptCurrentNative(
            population.countStore,&fog,&population);
    if(population.countStore.stored)return population.countStep==4&&manager.sourceActiveCount128_==1&&
        manager.sourceFogCountReceiptCurrentNative(population.countStore,&fog,&population);
    return population.retirementEmpty&&population.volumePositive==false&&!population.countStep&&
        !population.countStore.pc&&manager.sourceActiveCount128_==0&&
        manager.sourceFogCountReceiptCurrentNative(population.countStore,&fog,&population);
}
bool Owner::drawSourceParticleMode2Native(std::size_t rowIndex,std::size_t emitterIndex,
    a::OriginalParticleSourceConfig50LA& config,std::uint32_t& value)noexcept {
    auto& history=config.commonMode2_;
    if(config.sourcePC_!=0x15b148u||history.pc!=0x15b148u||!config.commonMode2DrawAdapter_||
       history.drawEntered||history.drawReturned||history.applicationPin||history.application||
       history.rawBefore||history.rawAfter||history.rawValue||history.minimum||history.converted||
       history.maximum||history.unit||history.difference||history.product||history.sum||history.stored)return false;
    const auto at=[&](std::uint32_t p){config.sourcePC_=history.pc=p;};
    at(0x308030);at(0x308038);
    //Only leaf admission while this exact Config's scoped adapter is live.
    //The full descriptor preflight requires the adapter already withdrawn.
    if(nativeScbThread_!=std::this_thread::get_id()||rowIndex>=4||!nativeParticleList3D8_||
       !nativeParticleList3D8_->rows[rowIndex]||!worldApplication_||
       worldApplication_->nativeOwnerThread_!=std::this_thread::get_id()||
       !worldApplication_->constructed_||worldApplication_->released_)return false;
    auto& row=*nativeParticleList3D8_->rows[rowIndex];
    auto& service=rowIndex?sourceParticleLaterRows_[rowIndex-1]:sourceParticleService124_;
    if(!row.sourceEmitterArrayPublished||!row.sourceEmitterArray00||
       *row.sourceEmitterArray00!=row.sourceEmitterPointers.get()||!row.sourceEmitterPointers||
       emitterIndex>=row.sourceEmitterPublishedSlots||emitterIndex>=row.sourceEmitters.size()||
       !row.sourceEmitters[emitterIndex]||service.configEmitterIndex!=emitterIndex||
       !service.configEntered||service.configTarget10!=0x27d7a0u||
       service.emitterConfigTarget94!=config.emitterPropertyTarget_)return false;
    auto& emitter=*row.sourceEmitters[emitterIndex];
    if(row.sourceEmitterPointers[emitterIndex]!=&emitter.entity||
       emitter.config15AE00.get()!=&config||config.emitterOwner_!=&emitter.entity||
       !emitter.constructorReturned||!emitter.counted||!emitter.enrolled||
       emitter.nativeTopologyWithdrawn||emitter.borrowsWithdrawn||emitter.configNativeWithdrawn||
       !emitter.managerStored||!emitter.manager70||emitter.manager70!=trackParticles_.get()||
       !sourceParticle4A0_||*sourceParticle4A0_!=emitter.manager70||!sourceParticleLoadReturned_||
       sourceParticleStageNativeClosed_||sourceParticle4A0NativeWithdrawn_||
       emitter.configOpenReturned!=true||config.sourceOpenReturned_!=true||
       config.sourceClosed_||config.nativeClosed_||!config.current()||
       !config.emitterPropertiesAttempted_||!config.commonTailAttempted_||
       config.commonEmitMode5C_!=2||!config.commonModeStored_||config.commonTailScalarCount_!=4||
       config.commonModeScalarCount_!=4||config.commonMode2TimeStored_||
       config.commonModeTimeCopied_||config.commonPropertiesSelectionAttempted_||config.commonReturned_)return false;
    if(!sourceLoadApplication_||!sourceLoadApplication_->enabled||!sourceLoadApplication_->state||
       !sourceLoadApplication_->state->known||sourceLoadApplication_->entities()!=worldApplication_||
       !sourceLoadRace_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->sourceRace28COwner_.get()!=sourceLoadRace_||
       &sourceLoadApplication_->state->rawRandom308030!=
           &sourceLoadApplication_->state->sharedNoise308030.sharedRandomState)return false;
    //Pin this existing process state; never copy its noise object, install a
    //seed, sample a clock, or manufacture a per-emitter random stream.
    history.applicationPin=sourceLoadApplication_->state;
    history.application=sourceLoadApplication_;
    at(0x308044);history.drawEntered=true;
    history.rawBefore=history.applicationPin->rawRandom308030;
    at(0x30804c);at(0x308050);at(0x308054);at(0x308058);at(0x30805c);
    history.rawValue=g::drawOriginalSharedRng50IU(history.applicationPin->sharedNoise308030);
    history.rawAfter=history.applicationPin->rawRandom308030;
    history.drawReturned=true;value=history.rawValue;return true;
}
bool Owner::continueSourceEmitterConfig15AE00(std::size_t rowIndex,SourceParticleService124& h,std::uint32_t& pc)noexcept {
    if(rowIndex>=4||(rowIndex==0&& &h!=&sourceParticleService124_)||
       (rowIndex>0&& &h!=&sourceParticleLaterRows_[rowIndex-1]))return false;
    const bool subtype0=h.emitterType==0u,subtype1=h.emitterType==1u,subtype2=h.emitterType==2u,subtype9=h.emitterType==9u,subtype11=h.emitterType==11u,subtype12=h.emitterType==12u;
    const bool successorFog=subtype12&&rowIndex>0&&(rowIndex==1?sourceParticleService124_:
        sourceParticleLaterRows_[rowIndex-2]).emitterType==12u;
    const bool nonFogAfterFog=bool(sourceNonFogPrecedingFogRowNative(rowIndex));
    if((!subtype0&&!subtype1&&!subtype2&&!subtype9&&!subtype11&&!subtype12&&h.emitterType!=8u)||pc!=(subtype0?0x15baa0u:(subtype1?0x15cf80u:(subtype2?0x15d7e8u:(subtype9?0x15ea24u:(subtype11?0x15f2f0u:(subtype12?0x1600b0u:0x15e024u))))))||
       !(subtype0?h.type0FilenameAppended:(subtype1?h.type1FilenameAppended:(subtype2?h.type2FilenameAppended:(subtype9?h.type9FilenameAppended:(subtype11?h.type11FilenameAppended:(subtype12?h.type12FilenameAppended:h.type8FilenameAppended))))))||h.emitterConfigConstructAttempted||
       !h.configEmitterIndex||!nativeParticleList3D8_||!nativeParticleList3D8_->rows[rowIndex])return false;
    auto& row=*nativeParticleList3D8_->rows[rowIndex];
    if(*h.configEmitterIndex>=row.sourceEmitterPublishedSlots||!row.sourceEmitters[*h.configEmitterIndex])return false;
    auto& e=*row.sourceEmitters[*h.configEmitterIndex];
    if(!e.filenameAppended||!e.filenameBytes||e.borrowsWithdrawn||e.config15AE00||
       !e.managerStored||e.manager70!=trackParticles_.get()||!sourceTrackMountDomain_||
       !preflightSourceWorldParticleDescriptorsNative())return false;
    const auto precedingFogCurrent=[&]()noexcept {
        return !nonFogAfterFog||sourceNonFogAfterFogCustodyCurrentNative(rowIndex);
    };
    if(!precedingFogCurrent())return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x15ae00);at(0x15ae14); //A1 is actual inline filename78, neverNULL.
    at(0x15ae1c);h.emitterConfigConstructAttempted=true;e.configConstructAttempted=true;
    try{
        //Native heap custody for the original stack object; failure is a
        //host allocation bound, not an invented source nullable new branch.
        e.config15AE00.reset(new a::OriginalParticleSourceConfig50LA(
            a::OriginalParticleSourceConfig50LA::Emitter15AE00Tag{},&e.entity,e.forcedSets1C4));
    }catch(...){return false;}
    at(0x1573a4);e.configConstructReturned=true;h.emitterConfigConstructReturned=true;at(0x15ae24);
    std::string filename;
    try{filename.assign(e.filename78.data(),*e.filenameBytes);}catch(...){return false;}
    at(0x15ae2c);e.configOpenAttempted=true;h.emitterConfigOpenAttempted=true;
    a::OriginalStandardTextureDiagnostic50LA diagnostic;
    const bool returned=e.config15AE00->beginEmitter15AE00(sourceTrackMountDomain_->resolver3E1A70(),
        sourceTrackMountDomain_->archiveProvider3E1A70(),filename,&e.entity,diagnostic);
    at(!returned&&diagnostic.sourcePC?diagnostic.sourcePC:e.config15AE00->sourcePC_);
    if(!returned||!e.config15AE00->sourceOpenReturned_)return false;
    e.configOpenReturned=e.config15AE00->sourceOpenReturned_;h.emitterConfigOpenReturned=e.configOpenReturned;
    at(0x15ae34);at(0x15ae38);
    if(!*e.configOpenReturned){at(0x15ae64);return false;} //Original diagnostic/close remain unjoined.
    if(!e.config15AE00->current()||!e.config15AE00->textReturned_||
       e.dispatch08!=(subtype0?0x3cdcf0u:(subtype1?0x3cdc50u:(subtype2?0x3cdbb0u:(subtype9?0x3cda70u:(subtype11?0x3cd9d0u:(subtype12?0x3cd930u:0x3cdb10u)))))))return false;
    at(0x15ae3c);at(0x15ae40);at(0x15ae44);h.emitterConfigTarget94=subtype0?0x15c330u:(subtype1?0x15d560u:(subtype2?0x15dd00u:(subtype9?0x15ef70u:(subtype11?0x15fc10u:(subtype12?0x1602b0u:0x15e6d0u)))));
    at(0x15ae48);
    auto& config=*e.config15AE00;
    {
    //The adapter can only serve this synchronous, genuine property call.
    //Its RAII scope ends before external currentness, even on partial failure.
    struct Mode2Draw final : a::OriginalParticleSourceConfig50LA::Mode2DrawAdapter {
        Owner& owner;std::size_t row,index;a::OriginalParticleSourceConfig50LA* expected;
        Mode2Draw(Owner& o,std::size_t r,std::size_t i,a::OriginalParticleSourceConfig50LA& c)noexcept
            :owner(o),row(r),index(i),expected(&c){}
        bool draw15B148(a::OriginalParticleSourceConfig50LA& c,std::uint32_t& result)noexcept override {
            return &c==expected&&owner.drawSourceParticleMode2Native(row,index,c,result);
        }
    } adapter(*this,rowIndex,*h.configEmitterIndex,config);
    a::OriginalParticleSourceConfig50LA::Mode2DrawScope drawScope(config,adapter);
    if(!drawScope.bound())return false;
    if(subtype1){
        const bool returned=config.beginType1Properties15D560(&e.entity,e.words,e.known,diagnostic);
        at(config.sourcePC_);(void)returned;
        if(pc!=0x15d574||!config.commonReturned_)return false;
    }else if(subtype2){
        const bool returned=config.beginType2Properties15DD00(&e.entity,e.words,e.known,diagnostic);
        at(config.sourcePC_);(void)returned;
        if(pc!=0x15dd88||config.type2RangeReturned_!=2||!config.commonReturned_)return false;
    }else if(subtype9){
        const bool returned=config.beginType9Properties15EF70(&e.entity,e.words,e.known,diagnostic);
        at(config.sourcePC_);(void)returned;
        if(pc!=0x15eff8||!config.type9SpreadReturned_||!config.commonReturned_)return false;
    }else if(subtype11){
        const bool returned=config.beginType11Properties15FC10(&e.entity,e.words,e.known,diagnostic);
        at(config.sourcePC_);(void)returned;
        if(pc!=0x15fd5c||!config.type11MovingReturned_||!config.commonReturned_)return false;
    }else if(subtype12){
        const bool returned=config.beginType12Properties1602B0(&e.entity,e.words,e.known,diagnostic);
        at(config.sourcePC_);(void)returned;
        if(pc!=0x160414||config.type12FlagMask_!=3||!config.commonReturned_)return false;
    }else if(subtype0){
        const bool returned=config.beginType0Properties15C330(&e.entity,e.words,e.known,diagnostic);
        at(config.sourcePC_);(void)returned;
        if(pc!=0x15cca8||config.type0NoiseReturned_!=2)return false;
    }else{
        const bool returned=config.beginType8Properties15E6D0(&e.entity,e.words,e.known,diagnostic);
        at(config.sourcePC_);(void)returned;
        if(pc!=0x15e758)return false;
    }
    } //No Config keeps a pointer to the stack adapter beyond this call.
    if(!config.commonReturned_||!config.current()||!preflightSourceWorldParticleDescriptorsNative())return false;
    if(!subtype0&&!subtype1&&!subtype2&&!subtype11&&!subtype12){
        at(subtype9?0x15eff8u:0x15e758u);if(e.known[0x64/4]!=0xffffffffu)return false;
        at(subtype9?0x15effcu:0x15e75cu);e.word(0x220,e.words[0x64/4]);
        (subtype9?e.type9VelocityCopied:e.type8VelocityCopied)=true;
    }
    auto& groupsNonNull=subtype0?e.type0GroupsNonNull:(subtype1?e.type1GroupsNonNull:(subtype2?e.type2GroupsNonNull:(subtype9?e.type9GroupsNonNull:(subtype11?e.type11GroupsNonNull:(subtype12?e.type12GroupsNonNull:e.type8GroupsNonNull)))));
    auto& groupIndex=subtype0?e.type0GroupIndex:(subtype1?e.type1GroupIndex:(subtype2?e.type2GroupIndex:(subtype9?e.type9GroupIndex:(subtype11?e.type11GroupIndex:(subtype12?e.type12GroupIndex:e.type8GroupIndex)))));
    auto& groupCopyMask=subtype0?e.type0GroupCopyMask:(subtype1?e.type1GroupCopyMask:(subtype2?e.type2GroupCopyMask:(subtype9?e.type9GroupCopyMask:(subtype11?e.type11GroupCopyMask:(subtype12?e.type12GroupCopyMask:e.type8GroupCopyMask)))));
    at(subtype0?0x15cca8u:(subtype1?0x15d574u:(subtype2?0x15dd88u:(subtype9?0x15f000u:(subtype11?0x15fd5cu:(subtype12?0x160414u:0x15e760u))))));auto& manager=*e.manager70;
    if(manager.closed_||!manager.constructorReturned_||!manager.sourceEntityEnrolled_||
       !manager.sourceParticleEmitterBorrowers_)return false;
    at(subtype0?0x15ccacu:(subtype1?0x15d578u:(subtype2?0x15dd8cu:(subtype9?0x15f004u:(subtype11?0x15fd60u:(subtype12?0x160418u:0x15e764u))))));groupsNonNull=manager.sourceGroups13C_!=nullptr;
    if(subtype12){at(0x16041c);at(0x160420);}
    if(subtype11){at(0x15fd64);at(0x15fd68);}
    if(subtype9){at(0x15f008);at(0x15f00c);}
    if(subtype2){at(0x15dd90);at(0x15dd94);}
    if(subtype1){at(0x15d57c);at(0x15d580);} //Delay initializes the source lookup result toNULL.
    if(*groupsNonNull){
        if(subtype12)at(0x160424);
        if(subtype11)at(0x15fd6c);
        if(subtype9)at(0x15f010);
        if(subtype2)at(0x15dd98);
        if(subtype1)at(0x15d584);
        at(subtype0?0x15ccbcu:(subtype1?0x15d588u:(subtype2?0x15dd9cu:(subtype9?0x15f014u:(subtype11?0x15fd70u:(subtype12?0x160428u:0x15e770u))))));if(e.known[0x1b8/4]!=0xffffffffu)return false;
        const auto bits=e.words[0x1b8/4];
        const auto group=bits<=0x7fffffffu?static_cast<std::int32_t>(bits):static_cast<std::int32_t>(std::int64_t(bits)-0x100000000LL);
        at(0x161eb0);
        if(!config.checkEmitterGroupLookup161EA0(group,manager.sourceNumGroups140_)){at(config.sourcePC_);return false;}
        at(0x161f2c);
        //Actual assertion does not reject negative indexes. Native pointer
        //arithmetic is bounded separately; never manufacture a guest pointer.
        if(group<0){at(0x161f30);return false;}
        const auto index=static_cast<std::uint32_t>(group);
        if(!manager.sourceGroupAllocationPublished_||!manager.sourceGroupRows_||
           manager.sourceGroups13C_!=manager.sourceGroupStorage13C_.get()||index>=manager.sourceGroupCapacity_)return false;
        groupIndex=index;at(0x161f58);
        if(subtype12)at(0x16042c);
        if(subtype11)at(0x15fd74);
        if(subtype9)at(0x15f018);
        if(subtype2)at(0x15dda0);
        if(subtype1)at(0x15d58c); //Nonnull result annuls the BEQL delay15D590.
        at(subtype0?0x15ccc8u:(subtype1?0x15d594u:(subtype2?0x15dda8u:(subtype9?0x15f020u:(subtype11?0x15fd7cu:(subtype12?0x160434u:0x15e780u))))));
        const auto& record=manager.sourceGroupRows_[index].lanes;
        if(!record[2])return false;
        at(subtype0?0x15ccccu:(subtype1?0x15d598u:(subtype2?0x15ddacu:(subtype9?0x15f024u:(subtype11?0x15fd80u:(subtype12?0x160438u:0x15e784u))))));e.word(0x1b4,static_cast<std::uint32_t>(*record[2]));groupCopyMask=1;
        at(subtype0?0x15ccd0u:(subtype1?0x15d59cu:(subtype2?0x15ddb0u:(subtype9?0x15f028u:(subtype11?0x15fd84u:(subtype12?0x16043cu:0x15e788u))))));if(!record[1])return false;
        at(subtype0?0x15ccd4u:(subtype1?0x15d5a0u:(subtype2?0x15ddb4u:(subtype9?0x15f02cu:(subtype11?0x15fd88u:(subtype12?0x160440u:0x15e78cu))))));e.word(0x1b0,static_cast<std::uint32_t>(*record[1]));groupCopyMask=3;
        if(subtype12)at(0x160444);
        if(subtype11)at(0x15fd8c);
        if(subtype9)at(0x15f030);
        if(subtype2)at(0x15ddb8);
        if(subtype1)at(0x15d5a4);
        at(subtype0?0x15ccdcu:(subtype1?0x15d5a8u:(subtype2?0x15ddbcu:(subtype9?0x15f034u:(subtype11?0x15fd90u:(subtype12?0x160448u:0x15e794u))))));e.word(0x190,e.words[0x1b0/4]);groupCopyMask=7;
    }else{
        if(subtype12){at(0x16042c);at(0x160430);} //BEQL supplies85;16044C is not executed.
        if(subtype11){at(0x15fd74);at(0x15fd78);} //BEQL delay supplies47;15FD94 is not executed.
        if(subtype9){at(0x15f018);at(0x15f01c);} //BEQL delay supplies85;15F038 is not executed.
        if(subtype2){at(0x15dda0);at(0x15dda4);} //BEQL delay supplies47;15DDC0 is not executed.
        if(subtype1){at(0x15d58c);at(0x15d590);} //Taken BEQL supplies31;15D5AC is not executed.
        at(subtype0?0x15cce4u:(subtype1?0x15d5b0u:(subtype2?0x15ddc4u:(subtype9?0x15f03cu:(subtype11?0x15fd98u:(subtype12?0x160450u:0x15e79cu))))));e.word(0x1b4,(subtype0||subtype1)?0x1fu:(subtype2?0x2fu:(subtype9?0x55u:(subtype11?0x2fu:(subtype12?0x55u:0x40u)))));groupCopyMask=1;
        if(subtype12)at(0x160454);
        if(subtype11)at(0x15fd9c);
        if(subtype9)at(0x15f040);
        if(subtype2)at(0x15ddc8);
        if(subtype1)at(0x15d5b4);
        at(subtype0?0x15ccecu:(subtype1?0x15d5b8u:(subtype2?0x15ddccu:(subtype9?0x15f044u:(subtype11?0x15fda0u:(subtype12?0x160458u:0x15e7a4u))))));e.word(0x1b0,(subtype0||subtype1)?0x10u:(subtype2?0x20u:(subtype9?0x64u:(subtype11?0x20u:(subtype12?0x64u:0x54u)))));groupCopyMask=3;
        at(subtype0?0x15ccf0u:(subtype1?0x15d5bcu:(subtype2?0x15ddd0u:(subtype9?0x15f048u:(subtype11?0x15fda4u:(subtype12?0x16045cu:0x15e7a8u))))));e.word(0x190,(subtype0||subtype1)?0x10u:(subtype2?0x20u:(subtype9?0x64u:(subtype11?0x20u:(subtype12?0x64u:0x54u)))));groupCopyMask=7;
    }
    if(subtype12){
        at(0x160460);e.type12PopulationPending=true;
        auto& population=e.type12Population;
        const SourceEmitter27C890* previousFog=nullptr;
        if(successorFog){
            //The old emitter owner is still retained even though its descriptor
            //retired. No historical void pointer becomes a dereference capability.
            const auto& priorRow=nativeParticleList3D8_->rows[rowIndex-1];
            if(!priorRow||priorRow->sourceEmitters.empty()||!priorRow->sourceEmitters[0])return false;
            previousFog=priorRow->sourceEmitters[0].get();const auto& prior=previousFog->type12Population;
            if(previousFog==&e||!previousFog->constructorReturned||previousFog->dispatch08!=0x3cd930u||
               !previousFog->counted||!previousFog->enrolled||previousFog->borrowsWithdrawn||!previousFog->sourceRegistered||
               !previousFog->rowConfigReturned||!previousFog->type12LoaderReturned||!prior.returned||
               !previousFog->managerStored||previousFog->manager70!=e.manager70||
               previousFog->entity.sessionGeneration!=e.entity.sessionGeneration||
               !prior.countStore.stored||!manager.sourceFogCountReceiptCurrentNative(
                   prior.countStore,previousFog,&prior))return false;
            //Earlier no-emission/count0 remains at this actual call boundary.
            //It cannot authorize a later0->1 event or invalidate its old epoch0.
        }
        if(population.entered||!e.managerStored||e.manager70!=trackParticles_.get()||e.borrowsWithdrawn||
           !sourceParticle4A0_||*sourceParticle4A0_!=e.manager70||!sourceParticleLoadReturned_||
           sourceParticleStageNativeClosed_||sourceParticle4A0NativeWithdrawn_)return false;
        at(0x160464); //Same emitter receiver in the actual160520 call delay.
        const auto populationAt=[&](std::uint32_t p){at(p);population.pc=p;};
        populationAt(0x160520);population.entered=true;population.manager=e.manager70;
        populationAt(0x160554);populationAt(0x160558); //Actual first retirement index0.
        populationAt(0x1605a8);
        if(e.manager70!=population.manager)return false;
        if(successorFog){
            auto& scan=population.retirementScan;
            const auto step=[&](std::uint32_t sourcePC)noexcept {
                populationAt(sourcePC);
                return manager.stepSourceFogRetirement160570(sourcePC,e.manager70,&e,&population,
                    e.entity.sessionGeneration,scan);
            };
            if(!previousFog||!step(0x1605a8)||!step(0x1605ac))return false;
            population.retirementCount128=scan.first128;
            populationAt(0x1605b0);populationAt(0x1605b4);populationAt(0x1605b8);
            if(!step(0x16055c))return false;
            populationAt(0x160560);if(!step(0x160564))return false;
            populationAt(0x160568);if(!step(0x16056c)||!step(0x160570))return false;
            populationAt(0x160574);if(!step(0x160578))return false; //BNEL mismatch executes its increment delay.
            populationAt(0x1605a4);if(!step(0x1605a8)||!step(0x1605ac))return false;
            populationAt(0x1605b0);populationAt(0x1605b4);if(!step(0x1605b8))return false;
            populationAt(0x1605bc); //Scanned, not empty; actual creation jump/delay follow below.
        }else{
        populationAt(0x1605ac);
        //The original first-occurrence count0 route remains unchanged.
        if(!manager.sourceActiveCount128Known_)return false;
        population.retirementCount128=manager.sourceActiveCount128_;
        populationAt(0x1605b0);populationAt(0x1605b4);populationAt(0x1605b8);
        if(*population.retirementCount128>0){
            populationAt(0x16055c);return false; //No admitted source record+48 owner/knownness yet.
        }
        population.retirementEmpty=true;
        }
        populationAt(0x1605bc);populationAt(0x1605c0);population.creationIndexInitial=0;
        //Actual branch delay initializes creation-loop s0, not manager128.
        populationAt(0x160ab8);populationAt(0x160abc);
        if(e.known[0x228/4]!=0xffffffffu)return false;
        population.volume228=e.words[0x228/4];
        populationAt(0x160ac0); //CVT.S.W(0) is exact +0.
        populationAt(0x160ac4);
        const auto volume=*population.volume228;
        //Current Config permits finite values only. Keep exceptional values
        //bounded rather than replacing the source C.OLT.S semantics.
        if((volume&0x7f800000u)==0x7f800000u||
           ((volume&0x7f800000u)==0&&(volume&0x7fffffu)))return false;
        population.volumePositive=(volume&0x80000000u)==0&&(volume&0x7fffffffu)!=0;
        populationAt(0x160ac8);populationAt(0x160acc);populationAt(0x160ad0);
        if(*population.volumePositive){
            populationAt(0x1605c4);if(e.manager70!=population.manager)return false;
            populationAt(0x1605c8);if(!manager.sourceActiveCount128Known_||
                (successorFog&&!manager.sourceFogSuccessorCountObservationCurrentNative(manager.sourceActiveCount128_,
                    population.retirementScan,&e,&population)))return false;
            population.creationCount128=manager.sourceActiveCount128_; //Fresh source read, not a scan-count copy.
            populationAt(0x1605cc);population.capacity130=manager.capacity130_;
            populationAt(0x1605d0);population.capacityAvailable=*population.creationCount128<*population.capacity130;
            populationAt(0x1605d4);populationAt(0x1605d8);
            if(*population.capacityAvailable){
                populationAt(0x1605dc);population.randomPending=true;
                //This actual source Application already owns the native
                //process RNG used by Main/Rider/race. Reuse its live chronology;
                //never install ELF seed1, sample a clock, or reset lazy caches.
                const auto randomApplicationCurrent=[&]()noexcept {
                    return nativeScbThread_==std::this_thread::get_id()&&sourceLoadApplication_&&
                        sourceLoadApplication_->enabled&&sourceLoadApplication_->state&&sourceLoadApplication_->state->known&&
                        worldApplication_&&worldApplication_->nativeOwnerThread_==std::this_thread::get_id()&&
                        worldApplication_->constructed_&&!worldApplication_->released_&&
                        sourceLoadApplication_->entities()==worldApplication_&&sourceLoadRace_&&
                        sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
                        sourceLoadApplication_->sourceRace28COwner_.get()==sourceLoadRace_&&
                        &sourceLoadApplication_->state->rawRandom308030==
                            &sourceLoadApplication_->state->sharedNoise308030.sharedRandomState;
                };
                if(!randomApplicationCurrent())return false;
                population.randomOwner=sourceLoadApplication_->state;
                population.randomApplication=sourceLoadApplication_;
                const auto draw=[&](std::size_t n)noexcept {
                    populationAt(0x308030);population.randomPending=false;
                    populationAt(0x308038);
                    if(n>=3||population.randomDrawCount!=n||!randomApplicationCurrent()||
                       population.randomApplication!=sourceLoadApplication_||
                       population.randomOwner!=sourceLoadApplication_->state||
                       (n&&population.randomOwner->rawRandom308030!=population.randomAfter[n-1]))return false;
                    //308044 reads the real raw word; the30805C return delay
                    //publishes its new value. No whole-noise copy or rollback.
                    populationAt(0x308044);
                    population.randomBefore[n]=population.randomOwner->rawRandom308030;
                    populationAt(0x30804c);populationAt(0x308050);populationAt(0x308054);populationAt(0x308058);
                    populationAt(0x30805c);
                    population.randomValue[n]=g::drawOriginalSharedRng50IU(population.randomOwner->sharedNoise308030);
                    population.randomAfter[n]=population.randomOwner->rawRandom308030;
                    ++population.randomDrawCount;population.randomPending=false;return true;
                };
                const auto convert=[&](std::size_t n,std::uint32_t sourcePC)noexcept {
                    populationAt(sourcePC);std::uint32_t bits=0;
                    if(n>=population.randomDrawCount||!g::detail::sourceParticleTruncateInteger50LA(population.randomValue[n],bits))return false;
                    population.randomConverted[n]=bits;population.randomConvertedMask|=static_cast<std::uint8_t>(1u<<n);return true;
                };
                const auto scale=[&](std::size_t n,std::uint32_t sourcePC)noexcept {
                    populationAt(sourcePC);std::uint32_t bits=0;
                    if(!(population.randomConvertedMask&(1u<<n))||!g::detail::sourceParticleFogScale50LA(population.randomConverted[n],bits))return false;
                    population.randomUnit[n]=bits;population.randomUnitMask|=static_cast<std::uint8_t>(1u<<n);return true;
                };
                populationAt(0x1605dc);populationAt(0x1605e0);if(!draw(0))return false;
                populationAt(0x1605e4);populationAt(0x1605e8);
                if(!convert(0,0x1605ec))return false;
                populationAt(0x1605f0);populationAt(0x1605f4);
                //The first scaling is the NEXT JAL's delay, before draw2.
                populationAt(0x1605f8);if(!scale(0,0x1605fc))return false;
                if(!draw(1))return false;
                populationAt(0x160600);populationAt(0x160604);
                if(!convert(1,0x160608))return false;
                populationAt(0x16060c);populationAt(0x160610);
                populationAt(0x160614);if(!scale(1,0x160618))return false;
                if(!draw(2))return false;
                populationAt(0x16061c);populationAt(0x160620);
                if(!convert(2,0x160624))return false;
                populationAt(0x160628);population.randomStack90[0]=population.randomUnit[0];population.randomStack90Mask=1;
                populationAt(0x16062c);populationAt(0x160630);
                population.randomStack90[1]=population.randomUnit[1];population.randomStack90Mask=3;
                populationAt(0x160634);populationAt(0x160638);populationAt(0x16063c);populationAt(0x160640);
                if(!scale(2,0x160644))return false;
                populationAt(0x160648);population.randomStack90[2]=population.randomUnit[2];population.randomStack90Mask=7;
                populationAt(0x16064c);population.randomStack90[3]=0x3f800000u;population.randomStack90Mask=15;
                populationAt(0x160650);populationAt(0x160654);
                population.randomStack70Initial=population.randomStack90;population.randomStack70=population.randomStack70Initial;population.randomStack70Copied=true;
                //Signed finite endpoints use the bounded scalar SUB/ADD.
                //The actual difference remains visible before MUL admits only
                //+0 or bounded positive widths, including non-powers. Initial scales
                //use constantLEFT; range products use unitLEFT,widthRIGHT.
                //Their different pinned-model corrections are never exchanged.
                //Every reached read/operation/store retains its own receipt;
                //a later refusal cannot undo earlier RNG or completed lanes.
                for(std::size_t lane=0;lane<3;++lane){
                    auto& range=population.rangeAxes[lane];
                    const auto base=0x160658u+static_cast<std::uint32_t>(lane)*0x1cu;
                    populationAt(base);if(e.known[0x210/4+lane]!=0xffffffffu)return false;
                    range.maximum=e.words[0x210/4+lane];range.step=1;
                    populationAt(base+4);if(e.known[0x200/4+lane]!=0xffffffffu)return false;
                    range.minimum=e.words[0x200/4+lane];range.step=2;
                    populationAt(base+8);range.unit=population.randomStack70[lane];range.step=3;
                    populationAt(base+12);
                    if(!g::detail::sourceParticleFogRangeDifference50LA(range.maximum,range.minimum,range.difference))return false;
                    range.step=4;populationAt(base+16);
                    if(!g::detail::sourceParticleFogRangeProduct50LA(range.unit,range.difference,range.product))return false;
                    range.step=5;populationAt(base+20);
                    if(!g::detail::sourceParticleFogRangeSum50LA(range.minimum,range.product,range.sum))return false;
                    range.step=6;populationAt(base+24);
                    population.randomStack70[lane]=range.sum;range.step=7;
                }
                const auto positionAt=[&](std::uint8_t step){
                    population.positionStep=step;populationAt(0x1606acu+std::uint32_t(step)*4u);
                };
                positionAt(0);if(e.manager70!=population.manager)return false;
                positionAt(1);if(e.known[0x38/4]!=0xffffffffu)return false;
                population.positionInput[2]=e.words[0x38/4];
                positionAt(2);population.positionLocal[2]=population.randomStack70[2];
                positionAt(3);if(e.known[0x30/4]!=0xffffffffu)return false;
                population.positionInput[0]=e.words[0x30/4];
                positionAt(4);if(e.known[0x34/4]!=0xffffffffu)return false;
                population.positionInput[1]=e.words[0x34/4];
                positionAt(5);population.positionLocal[1]=population.randomStack70[1];
                positionAt(6);if(!manager.sourceActiveCount128Known_||
                    (successorFog&&!manager.sourceFogSuccessorCountObservationCurrentNative(manager.sourceActiveCount128_,
                        population.retirementScan,&e,&population)))return false;
                population.positionIndex128=manager.sourceActiveCount128_; //Distinct fresh1606C4 observation.
                //Each genuine emitter/local pair uses the bounded pinned
                //scalar ADD, including nonzero translated boxes. Refusal at a
                //later axis preserves earlier results and owner observations.
                positionAt(7);
                if(!g::detail::sourceParticleFogPositionAdd50LA(population.positionInput[2],population.positionLocal[2],population.positionResult[2]))return false;
                positionAt(8);population.positionLocal[0]=population.randomStack70[0];
                positionAt(9);if(!manager.sourceParticlePointersPublished_)return false;
                population.positionPublication138=manager.sourceParticlePointers138_;
                //1606D4's index*4 and1606DC's address construction remain a
                //symbolic source address. The private slot owner performs safe
                //host-width indexing only at1606E4; no guest pointer is forged.
                positionAt(10);positionAt(11);
                if(!g::detail::sourceParticleFogPositionAdd50LA(population.positionInput[1],population.positionLocal[1],population.positionResult[1]))return false;
                positionAt(12);positionAt(13);
                if(!g::detail::sourceParticleFogPositionAdd50LA(population.positionInput[0],population.positionLocal[0],population.positionResult[0]))return false;
                positionAt(14);
                if(successorFog){
                    if(!manager.readSourceFogSuccessorPositionSlot1606E4(population.retirementScan,*population.positionIndex128,
                        *population.positionPublication138,population.positionStore))return false;
                }else if(!manager.readSourceFogPositionSlot1606E4(*population.positionIndex128,
                    *population.positionPublication138,population.positionStore))return false;
                positionAt(15);population.positionStackA0[0]=population.positionResult[0];
                positionAt(16);population.positionStackA0[1]=population.positionResult[1];
                positionAt(17);population.positionStackA0[2]=population.positionResult[2];
                positionAt(18);population.positionStackA0[3]=0x3f800000u;
                positionAt(19); //Actual full LQ of the four recorded stack words.
                positionAt(20);
                if(successorFog){
                    if(!manager.storeSourceFogSuccessorPosition1606FC(population.retirementScan,
                        population.positionStackA0,population.positionStore))return false;
                }else if(!manager.storeSourceFogPosition1606FC(population.positionStackA0,population.positionStore))return false;
                positionAt(21); //Committed position precedes every new effect.
                populationAt(0x160700);if(e.known[0x22c/4]!=0xffffffffu)return false;
                population.oscillation22C=e.words[0x22c/4];population.oscillationStep=1;
                populationAt(0x160704);
                const auto oscillation=*population.oscillation22C;
                //Only finite normal or signed-zero comparison operands. The
                //actual other operand is f0=-1 from16063C, not a host boolean.
                if((oscillation&0x7f800000u)==0x7f800000u||
                   (!(oscillation&0x7f800000u)&&(oscillation&0x007fffffu)))return false;
                population.oscillationDisabled=oscillation==0xbf800000u;
                populationAt(0x160708);populationAt(0x16070c);populationAt(0x160710);
                population.oscillationStep=2; //Taken and untaken delays both load1.0 bits.
                if(!*population.oscillationDisabled){
                    populationAt(0x160714);populationAt(0x160718);
                    populationAt(0x308030);population.oscillationRandomStep=1;
                    populationAt(0x308038);
                    if(!randomApplicationCurrent()||population.randomApplication!=sourceLoadApplication_||
                       population.randomOwner!=sourceLoadApplication_->state||
                       population.randomOwner->rawRandom308030!=population.randomAfter[2])return false;
                    populationAt(0x308044);population.oscillationRandomBefore=population.randomOwner->rawRandom308030;
                    populationAt(0x30804c);populationAt(0x308050);populationAt(0x308054);populationAt(0x308058);
                    populationAt(0x30805c);
                    population.oscillationRandomValue=g::drawOriginalSharedRng50IU(population.randomOwner->sharedNoise308030);
                    population.oscillationRandomAfter=population.randomOwner->rawRandom308030;
                    population.oscillationRandomStep=2;
                    populationAt(0x16071c);populationAt(0x160720);populationAt(0x160724);
                    if(!g::detail::sourceParticleTruncateInteger50LA(population.oscillationRandomValue,population.oscillationRandomConverted))return false;
                    population.oscillationRandomStep=3;
                    populationAt(0x160728);populationAt(0x16072c);populationAt(0x160730);
                    if(e.known[0x234/4]!=0xffffffffu)return false;
                    population.oscillationMagnitude234=e.words[0x234/4];population.oscillationRandomStep=4;
                    populationAt(0x160734);
                    if(!g::detail::sourceParticleFogScale50LA(population.oscillationRandomConverted,population.oscillationRandomUnit))return false;
                    population.oscillationRandomStep=5;
                    populationAt(0x160738);populationAt(0x16073c);
                    //JAL160738's delay must complete BEFORE its fifth draw.
                    //Actual magnitude is LEFT and unit RIGHT. Admit+0 or
                    //bounded positive magnitudes using that exact operand order;
                    //each authentic draw and partial product stays irreversible.
                    if(!g::detail::sourceParticleFogVelocityProduct50LA(population.oscillationRandomUnit,
                       *population.oscillationMagnitude234,population.velocityProducts[0]))return false;
                    const auto velocityAt=[&](std::uint8_t step,std::uint32_t pc){
                        population.velocityStep=step;populationAt(pc);
                    };
                    const auto velocityDraw=[&](std::size_t n,std::uint8_t step)noexcept {
                        velocityAt(step,0x308030);populationAt(0x308038);
                        if(n>=5)return false;
                        const auto previous=n?population.velocityRandomAfter[n-1]:population.oscillationRandomAfter;
                        if(!randomApplicationCurrent()||population.randomApplication!=sourceLoadApplication_||
                           population.randomOwner!=sourceLoadApplication_->state||
                           population.randomOwner->rawRandom308030!=previous)return false;
                        populationAt(0x308044);population.velocityRandomBefore[n]=population.randomOwner->rawRandom308030;
                        populationAt(0x30804c);populationAt(0x308050);populationAt(0x308054);populationAt(0x308058);
                        populationAt(0x30805c);
                        population.velocityRandomValue[n]=g::drawOriginalSharedRng50IU(population.randomOwner->sharedNoise308030);
                        population.velocityRandomAfter[n]=population.randomOwner->rawRandom308030;
                        return true;
                    };
                    if(!velocityDraw(0,1))return false;
                    populationAt(0x160740);populationAt(0x160744);velocityAt(2,0x160748);
                    if(!g::detail::sourceParticleTruncateInteger50LA(population.velocityRandomValue[0],population.velocityRandomConverted[0]))return false;
                    populationAt(0x16074c);populationAt(0x160750);velocityAt(3,0x160754);
                    if(e.known[0x234/4]!=0xffffffffu)return false;
                    population.velocityMagnitude234[0]=e.words[0x234/4];velocityAt(4,0x160758);
                    if(!g::detail::sourceParticleFogScale50LA(population.velocityRandomConverted[0],population.velocityRandomUnit[0]))return false;
                    populationAt(0x16075c);velocityAt(5,0x160760);
                    if(!g::detail::sourceParticleFogVelocityProduct50LA(population.velocityRandomUnit[0],
                       *population.velocityMagnitude234[0],population.velocityProducts[1]))return false;
                    //The sixth call follows its actual160760 delay product.
                    if(!velocityDraw(1,6))return false;
                    populationAt(0x160764);populationAt(0x160768);velocityAt(7,0x16076c);
                    if(!g::detail::sourceParticleTruncateInteger50LA(population.velocityRandomValue[1],population.velocityRandomConverted[1]))return false;
                    populationAt(0x160770);populationAt(0x160774);velocityAt(8,0x160778);
                    if(e.known[0x234/4]!=0xffffffffu)return false;
                    population.velocityMagnitude234[1]=e.words[0x234/4];velocityAt(9,0x16077c);
                    if(!g::detail::sourceParticleFogScale50LA(population.velocityRandomConverted[1],population.velocityRandomUnit[1]))return false;
                    velocityAt(10,0x160780);
                    if(!g::detail::sourceParticleFogVelocityProduct50LA(population.velocityRandomUnit[1],
                       *population.velocityMagnitude234[1],population.velocityProducts[2]))return false;
                    populationAt(0x160784);population.velocityStackB0[0]=population.velocityProducts[2];population.velocityStackB0Mask=1;
                    populationAt(0x160788);population.velocityStackB0[1]=population.velocityProducts[1];population.velocityStackB0Mask=3;
                    populationAt(0x16078c);population.velocityStackB0[2]=population.velocityProducts[0];population.velocityStackB0Mask=7;
                    populationAt(0x160790);population.velocityStackB0[3]=0x3f800000u;population.velocityStackB0Mask=15;
                    populationAt(0x160794);population.velocityStackB0Loaded=true;
                    populationAt(0x160798);populationAt(0x16079c);
                    population.velocityStack80=population.velocityStackB0;population.velocityStack80Copied=true;
                    //The stack copy is the seventh call's delay. Every sign
                    //decision uses its own real draw; no branch is forced.
                    if(!velocityDraw(2,11))return false;
                    populationAt(0x1607a0);populationAt(0x1607a4);velocityAt(12,0x1607a8);
                    if(!g::detail::sourceParticleTruncateInteger50LA(population.velocityRandomValue[2],population.velocityRandomConverted[2]))return false;
                    populationAt(0x1607ac);populationAt(0x1607b0);populationAt(0x1607b4);populationAt(0x1607b8);velocityAt(13,0x1607bc);
                    if(!g::detail::sourceParticleFogScale50LA(population.velocityRandomConverted[2],population.velocityRandomUnit[2]))return false;
                    populationAt(0x1607c0);population.velocitySignLeHalf[0]=population.velocityRandomUnit[2]<=0x3f000000u;
                    populationAt(0x1607c4);populationAt(0x1607c8);populationAt(0x1607cc);
                    if(!*population.velocitySignLeHalf[0]){
                        populationAt(0x1607d0);population.velocityNegationInput[0]=population.velocityStack80[0];
                        populationAt(0x1607d4);populationAt(0x1607d8);populationAt(0x1607dc);velocityAt(14,0x1607e0);
                        std::uint32_t result=0;
                        if(!g::detail::sourceParticleFogVelocityNegate50LA(*population.velocityNegationInput[0],result))return false;
                        population.velocityNegationResult[0]=result;
                        populationAt(0x1607e4);population.velocityStack80[0]=result;
                        population.velocityNegationStoredMask|=static_cast<std::uint8_t>(1u<<0);
                    }
                    populationAt(0x1607e8);populationAt(0x1607ec);
                    if(!velocityDraw(3,15))return false;
                    populationAt(0x1607f0);populationAt(0x1607f4);velocityAt(16,0x1607f8);
                    if(!g::detail::sourceParticleTruncateInteger50LA(population.velocityRandomValue[3],population.velocityRandomConverted[3]))return false;
                    populationAt(0x1607fc);populationAt(0x160800);populationAt(0x160804);populationAt(0x160808);velocityAt(17,0x16080c);
                    if(!g::detail::sourceParticleFogScale50LA(population.velocityRandomConverted[3],population.velocityRandomUnit[3]))return false;
                    populationAt(0x160810);population.velocitySignLeHalf[1]=population.velocityRandomUnit[3]<=0x3f000000u;
                    populationAt(0x160814);populationAt(0x160818);populationAt(0x16081c);
                    if(!*population.velocitySignLeHalf[1]){
                        populationAt(0x160820);population.velocityNegationInput[1]=population.velocityStack80[1];
                        populationAt(0x160824);populationAt(0x160828);populationAt(0x16082c);velocityAt(18,0x160830);
                        std::uint32_t result=0;
                        if(!g::detail::sourceParticleFogVelocityNegate50LA(*population.velocityNegationInput[1],result))return false;
                        population.velocityNegationResult[1]=result;
                        populationAt(0x160834);population.velocityStack80[1]=result;
                        population.velocityNegationStoredMask|=static_cast<std::uint8_t>(1u<<1);
                    }
                    populationAt(0x160838);populationAt(0x16083c);
                    if(!velocityDraw(4,19))return false;
                    populationAt(0x160840);populationAt(0x160844);velocityAt(20,0x160848);
                    if(!g::detail::sourceParticleTruncateInteger50LA(population.velocityRandomValue[4],population.velocityRandomConverted[4]))return false;
                    populationAt(0x16084c);populationAt(0x160850);populationAt(0x160854);populationAt(0x160858);velocityAt(21,0x16085c);
                    if(!g::detail::sourceParticleFogScale50LA(population.velocityRandomConverted[4],population.velocityRandomUnit[4]))return false;
                    populationAt(0x160860);population.velocitySignLeHalf[2]=population.velocityRandomUnit[4]<=0x3f000000u;
                    populationAt(0x160864);populationAt(0x160868);populationAt(0x16086c);
                    if(!*population.velocitySignLeHalf[2]){
                        populationAt(0x160870);population.velocityNegationInput[2]=population.velocityStack80[2];
                        populationAt(0x160874);populationAt(0x160878);populationAt(0x16087c);velocityAt(22,0x160880);
                        std::uint32_t result=0;
                        if(!g::detail::sourceParticleFogVelocityNegate50LA(*population.velocityNegationInput[2],result))return false;
                        population.velocityNegationResult[2]=result;
                        populationAt(0x160884);population.velocityStack80[2]=result;
                        population.velocityNegationStoredMask|=static_cast<std::uint8_t>(1u<<2);
                    }
                    populationAt(0x160888);velocityAt(23,0x16088c);
                    if(successorFog){
                        if(!manager.storeSourceFogSuccessorVelocity16088C(population.retirementScan,population.positionStore,
                            population.velocityStack80,population.velocityStore))return false;
                    }else if(!manager.storeSourceFogVelocity16088C(population.positionStore,population.velocityStack80,population.velocityStore))return false;
                    velocityAt(24,0x160890); //Actual LUI supplies1.0 before common tail.
                }
                //Actual22C=-1 skips velocity generation. The enabled route
                //joins here only after its genuine16088C SQ. Keep every scalar
                //read/store independently, including both separate184 reads.
                const auto fieldTailAt=[&](std::uint8_t step){
                    population.fieldTailStep=step;populationAt(0x160894u+std::uint32_t(step)*4u);
                };
                const auto storeField=[&](std::uint32_t sourcePC,std::uint32_t bits)noexcept {
                    return successorFog?manager.storeSourceFogSuccessorField1608AC(population.retirementScan,
                        population.positionStore,&e,sourcePC,bits,population.fieldStores):
                        manager.storeSourceFogField1608AC(population.positionStore,&e,sourcePC,bits,population.fieldStores);
                };
                fieldTailAt(0);if(e.known[0x184/4]!=0xffffffffu)return false;
                population.sizeInput184=e.words[0x184/4];
                fieldTailAt(1);fieldTailAt(2);fieldTailAt(3);fieldTailAt(4);fieldTailAt(5);
                //Authenticated source unitX is nonnegative normal or+0;
                //unsigned binary32 order is exactly its comparison to0.5.
                population.initialMirrorXLeHalf=population.randomUnit[0]<=0x3f000000u;
                fieldTailAt(6);
                if(!storeField(0x1608acu,*population.sizeInput184))return false;
                fieldTailAt(7);if(e.known[0x184/4]!=0xffffffffu)return false;
                population.initialSizeInput184=e.words[0x184/4];fieldTailAt(8);fieldTailAt(9);
                if(!storeField(0x1608b8u,*population.initialSizeInput184))return false;
                fieldTailAt(10);
                if(!storeField(0x1608bcu,0))return false;
                fieldTailAt(11);if(e.known[0x17c/4]!=0xffffffffu)return false;
                population.lifeInput17C=e.words[0x17c/4];fieldTailAt(12);
                if(!storeField(0x1608c4u,*population.lifeInput17C))return false;
                fieldTailAt(13);if(e.known[0x1a0/4]!=0xffffffffu)return false;
                population.ageInput1A0=e.words[0x1a0/4];fieldTailAt(14);
                if(!storeField(0x1608ccu,*population.ageInput1A0))return false;
                fieldTailAt(15);if(e.known[0x190/4]!=0xffffffffu)return false;
                population.frameInput190=e.words[0x190/4];fieldTailAt(16);
                if(!storeField(0x1608d4u,*population.frameInput190))return false;
                fieldTailAt(17);
                if(successorFog){
                    if(!manager.readSourceFogSuccessorLife1608D8(population.retirementScan,
                        population.positionStore,&e,population.fieldStores))return false;
                }else if(!manager.readSourceFogLife1608D8(population.positionStore,&e,population.fieldStores))return false;
                fieldTailAt(18); //Actual record30 readback precedes DIV1608DC.
                std::uint32_t inverse=0;bool assertedDivideByZero=false;
                if(!g::detail::sourceParticleFogReciprocal50LA(*population.fieldStores.lifeRead30,inverse,assertedDivideByZero))return false;
                population.reciprocalBits=inverse;population.reciprocalDAndSD=assertedDivideByZero;
                //Record only a new divide-by-zero assertion, never a clearing
                //of unknown prior D/SD. C remains the real1608A8 result; the
                //positive-normal path is the pinned model, not host division.
                const auto postDivideAt=[&](std::uint8_t step,std::uint32_t pc){
                    population.postDivideStep=step;populationAt(pc);
                };
                const auto storeTail=[&](std::uint32_t pc,std::uint32_t bits)noexcept {
                    return successorFog?manager.storeSourceFogSuccessorTail1608E0(population.retirementScan,
                        population.positionStore,population.fieldStores,&e,pc,bits,population.tailStores):
                        manager.storeSourceFogTail1608E0(population.positionStore,population.fieldStores,&e,pc,bits,population.tailStores);
                };
                const auto readFlags=[&](std::uint32_t pc)noexcept {
                    return successorFog?manager.readSourceFogSuccessorFlags160930(population.retirementScan,
                        population.positionStore,population.fieldStores,&e,pc,population.tailStores):
                        manager.readSourceFogFlags160930(population.positionStore,population.fieldStores,&e,pc,population.tailStores);
                };
                postDivideAt(1,0x1608e0);
                if(!storeTail(0x1608e0u,*population.reciprocalBits))return false;
                postDivideAt(2,0x1608e4);
                if(!storeTail(0x1608e4u,0))return false;
                postDivideAt(3,0x1608e8);if(e.known[0x198/4]!=0xffffffffu)return false;
                population.initialFlags198=e.words[0x198/4];
                populationAt(0x1608ec);postDivideAt(4,0x1608f0);
                //Ordinary BC1F delay: the initial44 store occurs on both edges.
                if(!storeTail(0x1608f0u,*population.initialFlags198))return false;
                if(*population.initialMirrorXLeHalf)populationAt(0x1608f4);
                populationAt(0x1608f8);populationAt(0x1608fc);populationAt(0x160900);populationAt(0x160904);
                populationAt(0x160908);population.mirrorYLeHalf=population.randomUnit[1]<=0x3f000000u;
                populationAt(0x16090c);populationAt(0x160910);populationAt(0x160914);
                if(*population.mirrorYLeHalf)populationAt(0x160918);
                populationAt(0x16091c);populationAt(0x160920);
                if(!*population.initialMirrorXLeHalf){
                    postDivideAt(5,0x160924);if(e.known[0x1cc/4]!=0xffffffffu)return false;
                    population.mirrorEnable1CC=e.words[0x1cc/4];populationAt(0x160928);populationAt(0x16092c);
                    population.mirrorXSet=*population.mirrorEnable1CC!=0;
                }else population.mirrorXSet=false; //No eager1CC read on the short-circuit edge.
                const auto firstReadPC=*population.mirrorXSet?0x160930u:0x160940u;
                postDivideAt(6,firstReadPC);
                if(!readFlags(firstReadPC))return false;
                if(*population.mirrorXSet){
                    populationAt(0x160934);population.firstFlagsResult=population.tailStores.values[3]|0x4000u;
                    populationAt(0x160938);postDivideAt(7,0x16093c);
                }else{
                    populationAt(0x160944);populationAt(0x160948);
                    population.firstFlagsResult=population.tailStores.values[3]&0xffffbfffu;
                    postDivideAt(7,0x16094c);
                }
                if(!storeTail(population.pc,*population.firstFlagsResult))return false;
                populationAt(0x160950);populationAt(0x160954);
                if(!*population.mirrorYLeHalf){
                    postDivideAt(8,0x160958);if(e.known[0x1d0/4]!=0xffffffffu)return false;
                    population.mirrorEnable1D0=e.words[0x1d0/4];populationAt(0x16095c);populationAt(0x160960);
                    population.mirrorYSet=*population.mirrorEnable1D0!=0;
                }else population.mirrorYSet=false;
                const auto secondReadPC=*population.mirrorYSet?0x160964u:0x160974u;
                postDivideAt(9,secondReadPC);
                if(!readFlags(secondReadPC))return false;
                if(*population.mirrorYSet){
                    populationAt(0x160968);population.secondFlagsResult=population.tailStores.values[5]|0x8000u;
                    populationAt(0x16096c);postDivideAt(10,0x160970);
                }else{
                    populationAt(0x160978);populationAt(0x16097c);populationAt(0x160980);
                    population.secondFlagsResult=population.tailStores.values[5]&0xffff7fffu;
                    postDivideAt(10,0x160984);
                }
                if(!storeTail(population.pc,*population.secondFlagsResult))return false;
                postDivideAt(11,0x160988);
                if(e.borrowsWithdrawn||!e.managerStored||e.manager70!=population.manager||
                   e.manager70!=trackParticles_.get())return false;
                auto* const reachedManager70=e.manager70; //Actual fresh emitter70 LW.
                population.countManager70=reachedManager70;
                const auto countAt=[&](std::uint8_t step,std::uint32_t pc){
                    population.countStep=step;populationAt(pc);
                };
                countAt(1,0x16098c);
                auto& countManager=*reachedManager70; //Retain that LW operand, not a second emitter70 read.
                if(!countManager.sourceActiveCount128Known_||
                   !countManager.sourceActiveCountAuthorityNative())return false;
                population.countBefore128=countManager.sourceActiveCount128_; //Actual fresh128 LW.
                countAt(2,0x160990);
                //The original ADDIU is exact for the independently bounded
                //epoch0 or epoch1 occurrence; neither case can overflow.
                if(*population.countBefore128!=(successorFog?1:0))return false;
                population.countAfter128=*population.countBefore128+1;
                countAt(3,0x160994);
                if(successorFog){
                    if(!countManager.storeSourceFogSuccessorCount160994(population.retirementScan,
                        population.positionStore,population.fieldStores,population.tailStores,&e,&population,
                        population.countManager70,*population.countBefore128,*population.countAfter128,population.countStore))return false;
                }else if(!countManager.storeSourceFogCount160994(population.positionStore,population.fieldStores,
                    population.tailStores,&e,&population,population.countManager70,
                    *population.countBefore128,*population.countAfter128,population.countStore))return false;
                countAt(4,0x160998);
                auto& frame=population.frameSelection;
                const auto frameAt=[&](std::uint8_t step,std::uint32_t pc){
                    frame.step=step;populationAt(pc);
                };
                const auto frameOwnerCurrent=[&]()noexcept {
                    return randomApplicationCurrent()&&population.randomApplication==sourceLoadApplication_&&
                        population.randomOwner==sourceLoadApplication_->state&&
                        (!successorFog||(!e.borrowsWithdrawn&&e.managerStored&&e.manager70==population.manager&&
                            e.manager70==trackParticles_.get()&&population.countStore.stored&&
                            e.manager70->sourceFogSuccessorCountReceiptCurrentNative(population.countStore,&e,&population)));
                    //Native custody only, not a fresh source128 read. The
                    //successor owns its count2 event and its own draw chain.
                };
                if(!frameOwnerCurrent())return false;
                //Same process statics80A8/3D6B30, not a Fog-local cache.
                auto& selectors=population.randomOwner->sharedRandomSelectors;
                frame.seedGuard80A8=selectors.seed.lazy80A8;
                frameAt(1,0x16099c);if(e.known[0x1b0/4]!=0xffffffffu)return false;
                frame.first1B0=e.words[0x1b0/4];populationAt(0x1609a0);
                frameAt(2,0x1609a4);if(e.known[0x1b4/4]!=0xffffffffu)return false;
                frame.last1B4=e.words[0x1b4/4]; //Ordinary BNE delay executes on BOTH edges.
                if(!*frame.seedGuard80A8){
                    populationAt(0x1609a8);populationAt(0x1609ac);frameAt(3,0x1609b0);
                    if(!frameOwnerCurrent())return false;
                    selectors.seed.seed3D6B30=0;frame.seedCacheCleared=true;
                    frameAt(4,0x1609b4);if(!frameOwnerCurrent())return false;
                    selectors.seed.lazy80A8=true;frame.seedGuardStored=true;
                }
                populationAt(0x1609b8);frameAt(5,0x1609bc);
                if(!frameOwnerCurrent())return false;
                frame.seedCache3D6B30=selectors.seed.seed3D6B30;populationAt(0x1609c0);
                frameAt(6,0x1609c4);
                //SUBU and subsequent ADDIU preserve low32 register bits.
                //Do not repair the original fallback first100,last85 range.
                frame.differenceWord=*frame.last1B4-*frame.first1B0;
                if(*frame.seedCache3D6B30==0){
                    frameAt(7,0x1609c8);return false;
                    //The real shared host clock exists, but its elapsed-time
                    //backend is not EE timer/MMIO101A50 evidence. Do not enter
                    //the call, sample a clock, seed RNG or undo prior effects.
                }
                frameAt(7,0x1609ec);frame.spanWord=*frame.differenceWord+1u;
                populationAt(0x1609f0);populationAt(0x1609f4);frameAt(8,0x1609f8);
                std::uint32_t spanBits=0;
                //The CVT.S.W is JAL's delay, BEFORE the next308030 draw.
                if(!g::detail::sourceParticleExactSignedSpan50LA(*frame.spanWord,spanBits))return false;
                frame.spanConverted=spanBits;
                populationAt(0x308030);frameAt(9,0x308038);
                const auto predecessor=population.oscillationDisabled==true?
                    population.randomAfter[2]:population.velocityRandomAfter[4];
                if(!frameOwnerCurrent()||population.randomOwner->rawRandom308030!=predecessor)return false;
                populationAt(0x308044);frame.randomBefore=population.randomOwner->rawRandom308030;
                populationAt(0x30804c);populationAt(0x308050);populationAt(0x308054);populationAt(0x308058);
                populationAt(0x30805c);
                frame.randomValue=g::drawOriginalSharedRng50IU(population.randomOwner->sharedNoise308030);
                frame.randomAfter=population.randomOwner->rawRandom308030;frame.randomDrawn=true;
                populationAt(0x1609fc);populationAt(0x160a00);frameAt(10,0x160a04);
                std::uint32_t frameBits=0;
                if(!g::detail::sourceParticleTruncateInteger50LA(frame.randomValue,frameBits))return false;
                frame.randomConverted=frameBits;frameAt(11,0x160a08);
                if(!frameOwnerCurrent())return false;
                frame.rangeGuard80AC=selectors.range.lazy80AC; //Actual reached LB, before either multiply.
                populationAt(0x160a0c);populationAt(0x160a10);frameAt(12,0x160a14);
                if(!g::detail::sourceParticleFogScale50LA(*frame.randomConverted,frameBits))return false;
                frame.randomUnit=frameBits;frameAt(13,0x160a18);
                //Actual span is LEFT, unit RIGHT. Admit zero or positive
                //integer spans through2^24; the genuine fallback span-14
                //still refuses here, after its real random draw and scale.
                if(!g::detail::sourceParticleFogFrameProduct50LA(*frame.spanConverted,*frame.randomUnit,frameBits))return false;
                frame.productBits=frameBits;frameAt(14,0x160a1c);
                if(!g::detail::sourceParticleFogFrameOffset50LA(*frame.productBits,frameBits))return false;
                frame.offsetWord=frameBits;populationAt(0x160a20);populationAt(0x160a24);
                frameAt(15,0x160a28);
                frame.selectedWord=*frame.first1B0+*frame.offsetWord; //ADDU in the ordinary BNE delay, both edges.
                //Use the saved160A08 guard; do not replace it with a fresh read.
                if(!*frame.rangeGuard80AC){
                    populationAt(0x160a2c);populationAt(0x160a30);frameAt(16,0x160a34);
                    if(!frameOwnerCurrent())return false;
                    selectors.range.rangeCheck3D6B38=1;frame.rangeCacheStored=true;
                    frameAt(17,0x160a38);if(!frameOwnerCurrent())return false;
                    selectors.range.lazy80AC=true;frame.rangeGuardStored=true;
                    populationAt(0x160a3c);
                }
                populationAt(0x160a40);frameAt(18,0x160a44);
                if(!frameOwnerCurrent())return false;
                frame.rangeCheck3D6B38=selectors.range.rangeCheck3D6B38;
                frameAt(19,0x160a48);populationAt(0x160a4c);
                if(*frame.rangeCheck3D6B38==0){
                    frameAt(20,0x160ab0); //Actual cache bypass: no assertion call or invented return.
                }else{
                    //LW/ADDU sign-extend their32-bit results. XOR bit31 gives
                    //the same signed ordering without a host narrowing cast.
                    const auto signedLess=[](std::uint32_t left,std::uint32_t right)noexcept {
                        return (left^0x80000000u)<(right^0x80000000u);
                    };
                    populationAt(0x160a50);frame.selectedBelowFirst=signedLess(*frame.selectedWord,*frame.first1B0);
                    populationAt(0x160a54);populationAt(0x160a58);frame.rangeCondition=false;
                    if(!*frame.selectedBelowFirst){
                        populationAt(0x160a5c);frame.lastBelowSelected=signedLess(*frame.last1B4,*frame.selectedWord);
                        populationAt(0x160a60);populationAt(0x160a64);
                        if(!*frame.lastBelowSelected){populationAt(0x160a68);frame.rangeCondition=true;populationAt(0x160a6c);}
                    }
                    populationAt(0x160a70);populationAt(0x160a74);populationAt(0x160a78);
                    frameAt(20,0x160a7c);populationAt(0x160a80);
                    frame.assertionArgument=*frame.rangeCondition?1u:0u; //Actual ANDI a0,v1,0xff delay.
                    populationAt(0x133660);populationAt(0x133664);populationAt(0x133668);frameAt(21,0x13366c);
                    if(!*frame.assertionArgument)return false; //False needs the real unknown36C100 debug owner.
                    //For a0!=0, BOTH debug branches return0 without output or
                    //callbacks. Collapse only that proven common outcome; no
                    //debug value, selected branch or global read is invented.
                    populationAt(0x1336a8);populationAt(0x1336ac);populationAt(0x1336b0);
                    frame.assertionReturnV0=0;
                    populationAt(0x160a84);populationAt(0x160a88);
                }
                frameAt(22,0x160ab0);
                if(e.borrowsWithdrawn||!e.managerStored||e.manager70!=population.manager||
                   e.manager70!=trackParticles_.get()||(successorFog&&!frameOwnerCurrent())||
                   !preflightSourceWorldParticleDescriptorsNative())return false;
                e.word(0x190,*frame.selectedWord);frame.frameStored190=true;
                //The completed particle's28 stays its old1608D4 value. This
                //source store selects the emitter frame for a NEXT particle.
                frameAt(23,0x160ab4);
                if(successorFog&&!frameOwnerCurrent())return false; //Native count2/owner guard, not a source count read.
                if(!population.creationIndexInitial||*population.creationIndexInitial!=0)return false;
                frame.creationIndexAfter=*population.creationIndexInitial+1u;
                populationAt(0x160ab8);frameAt(24,0x160abc);
                if(e.known[0x228/4]!=0xffffffffu)return false;
                frame.loopVolume228=e.words[0x228/4]; //Fresh LWC1; never substitute the initial loop read.
                frameAt(25,0x160ac0);frame.indexConverted=0x3f800000u; //Exact CVT.S.W of the actual index1.
                frameAt(26,0x160ac4);const auto nextVolume=*frame.loopVolume228;
                if((nextVolume&0x7f800000u)==0x7f800000u||
                   ((nextVolume&0x7f800000u)==0&&(nextVolume&0x7fffffu)))return false;
                frame.loopAgain=!(nextVolume&0x80000000u)&&nextVolume>0x3f800000u;
                populationAt(0x160ac8);populationAt(0x160acc);populationAt(0x160ad0);
                if(*frame.loopAgain){frameAt(27,0x1605c4);return false;} //Before any second-iteration manager/count access.
                frameAt(27,0x160ad4);
                if(successorFog&&(!frameOwnerCurrent()||!preflightSourceWorldParticleDescriptorsNative()))return false;
                for(std::uint32_t p=0x160ad4;p<=0x160b00;p+=4)populationAt(p);
                frame.step=28;population.returned=true;e.type12PopulationPending=false;
                at(0x160468); //Only160520 returned; no duplicate helper epilogue.
            }
        }
        //The real no-emission branch reads no138 record and (for228<=0)
        //no capacity130. Do not replay this epilogue after the one-particle return.
        if(!population.returned){
            if(successorFog){populationAt(0x160ad4);return false;} //Actual false-volume edge, before epilogue/return.
            for(std::uint32_t p=0x160ad4;p<=0x160b00;p+=4)populationAt(p);
            population.returned=true;e.type12PopulationPending=false;
        }
        at(0x160468);
    }
    //These are LastFrame/FirstFrame/current frame, not renderer dimensions.
    if(subtype12){
        //Only the actual mandatory helper return admits the callback epilogue.
        //The one-particle loop-reentry and every earlier pending path returned
        //false above; none may close Config or acquire a loader receipt.
        if(!e.type12Population.returned||e.type12Population.pc!=0x160b00u||
           e.type12PopulationPending||e.type12PropertiesReturned||config.emitterType12Returned_||
           e.borrowsWithdrawn||!e.managerStored||e.manager70!=trackParticles_.get()||
           e.manager70!=e.type12Population.manager||!config.current()||
           !preflightSourceWorldParticleDescriptorsNative())return false;
        if(successorFog&&(!e.type12Population.countStore.stored||
           !manager.sourceFogSuccessorCountReceiptCurrentNative(e.type12Population.countStore,&e,&e.type12Population)))return false;
        at(0x160468);at(0x16046c); //Unconditional branch; delay supplies the actual V0=1.
        //160470 is the separate zero-result path and is not executed here.
        for(std::uint32_t p=0x160474;p<=0x160484;p+=4)at(p);
        e.type12PropertiesReturned=true;config.emitterType12Returned_=true;
    }else if(subtype0){at(0x15ccf8);at(0x15cd10);e.type0PropertiesReturned=true;config.emitterType0Returned_=true;}
    else if(subtype1){
        at(0x15d5c0);at(0x15d5c4);at(0x15d5c8);at(0x15d5cc);at(0x15d5d0);
        e.type1PropertiesReturned=true;config.emitterType1Returned_=true;
    }else if(subtype2){
        at(0x15ddd4);at(0x15ddd8);
        for(std::uint32_t p=0x15dde0;p<=0x15ddf0;p+=4)at(p);
        e.type2PropertiesReturned=true;config.emitterType2Returned_=true;
    }else if(subtype9){
        at(0x15f04c);at(0x15f050);
        for(std::uint32_t p=0x15f058;p<=0x15f068;p+=4)at(p);
        e.type9PropertiesReturned=true;config.emitterType9Returned_=true;
    }else if(subtype11){
        at(0x15fda8);at(0x15fdac);
        for(std::uint32_t p=0x15fdb4;p<=0x15fdc4;p+=4)at(p);
        e.type11PropertiesReturned=true;config.emitterType11Returned_=true;
    }else{at(0x15e7b0);at(0x15e7c8);e.type8PropertiesReturned=true;config.emitterType8Returned_=true;}
    at(0x15ae50);at(0x15ae54);
    if(subtype12)at(0x15ae58); //Actual A1=-1 in this same stack Config destructor call delay.
    if(!config.closeEmitterStack1573B0(&e.entity,diagnostic)){
        at(diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_);return false;
    }
    at(config.sourcePC_); //Actual destructor return1574F4 precedes caller continuation.
    if(subtype12)at(0x15ae5c);
    at(0x15ae60);
    //Source stack Config has finished destruction. Withdraw its remaining
    //native singleton custody now, without a second source close, so another
    //emitter can construct its own fresh Config. Keep all source history.
    if(!config.abortNative(diagnostic))return false;
    e.configNativeWithdrawn=true;
    if(subtype12)for(std::uint32_t p=0x15ae8c;p<0x15ae9c;p+=4)at(p);
    at(0x15ae9c);e.emitterFileLoadReturned=true; //Actual15AE00 returns1.
    if(subtype0){
        //Actual nonzero15AE00 return branches over the fallback15BAB0.
        //15BAAC delay slot sets V0 to this same emitter; no new retain.
        at(0x15baa8);at(0x15baac);at(0x15bac8);at(0x15bae4);e.type0LoaderReturned=true;
        at(0x27d830);
    }else if(subtype1){
        //15CF88 skips the fallback after the actual nonzero15AE00 return.
        //Its delay15CF8C preserves this exact emitter as the loader result.
        at(0x15cf88);at(0x15cf8c);
        for(std::uint32_t p=0x15cfc8;p<=0x15cfe4;p+=4)at(p);
        e.type1LoaderReturned=true;
        at(0x27d8d0);
    }else if(subtype2){
        at(0x15d7f0);at(0x15d7f4); //Success skips fallback, preserving this same emitter.
        for(std::uint32_t p=0x15d858;p<=0x15d874;p+=4)at(p);
        e.type2LoaderReturned=true;at(0x27d980);
    }else if(subtype9){
        at(0x15ea2c);at(0x15ea30); //Success preserves this same emitter and skips fallback.
        for(std::uint32_t p=0x15eab0;p<=0x15eacc;p+=4)at(p);
        e.type9LoaderReturned=true;at(0x27dac0);
    }else if(subtype11){
        at(0x15f2f8);at(0x15f2fc); //Actual nonzero15AE00 result skips all fallback stores.
        for(std::uint32_t p=0x15f35c;p<=0x15f378;p+=4)at(p);
        e.type11LoaderReturned=true;at(0x27db60);
    }else if(subtype12){
        at(0x1600b8);at(0x1600bc); //Nonzero15AE00 result preserves this same emitter and skips defaults.
        for(std::uint32_t p=0x160138;p<=0x160154;p+=4)at(p);
        e.type12LoaderReturned=true;
        at(0x27dc00); //Fog already positioned inside15FF10; no wrapper position calls.
    }else{
        at(0x15e02c);at(0x15e030);at(0x15e0b0);at(0x15e0cc);e.type8LoaderReturned=true;
        at(0x27da20);
    }
    //3CDCF0,3CDC50,3CDBB0,3CDB10 and3CD9D0 share +80/+84/+20/+18:
    //15ACE0,15AD00,141560,141520. Type9 instead overrides+84 with15F070.
    //Fog3CD930 also shares+80/+20/+18 but its own+84 is160490.
    //Loader, position and custom stop histories stay distinct.
    if(e.dispatch08!=(subtype0?0x3cdcf0u:(subtype1?0x3cdc50u:(subtype2?0x3cdbb0u:(subtype9?0x3cda70u:(subtype11?0x3cd9d0u:(subtype12?0x3cd930u:0x3cdb10u))))))||
       !(subtype0?e.type0LoaderReturned:(subtype1?e.type1LoaderReturned:(subtype2?e.type2LoaderReturned:(subtype9?e.type9LoaderReturned:(subtype11?e.type11LoaderReturned:(subtype12?e.type12LoaderReturned:e.type8LoaderReturned)))))))return false;
    const auto successorWrapperCurrent=[&]()noexcept {
        if(!successorFog)return true;
        const auto& population=e.type12Population;
        //Native custody only: no additional original count read or write.
        return !e.borrowsWithdrawn&&e.managerStored&&e.manager70&&e.manager70==trackParticles_.get()&&
            e.manager70==population.manager&&e.type12LoaderReturned&&e.emitterFileLoadReturned&&
            e.configNativeWithdrawn&&population.returned&&population.countStep==4&&
            population.countStore.stored&&e.manager70->sourceFogSuccessorCountReceiptCurrentNative(
                population.countStore,&e,&population);
    };
    if(!precedingFogCurrent()||!successorWrapperCurrent()||!preflightSourceWorldParticleDescriptorsNative()||!row.descriptor04||
       !row.sourceDescriptor04||*row.sourceDescriptor04!=row.descriptor04.get())return false;
    if(subtype11){at(0x27db60);at(0x27db64);}
    if(subtype9){at(0x27dac0);at(0x27dac4);}
    if(subtype2){at(0x27d980);at(0x27d984);}
    if(subtype1){at(0x27d8d0);at(0x27d8d4);}
    if(subtype1&&!h.configModelNonNull){
        //Both15CFF0 calls take the same live descriptor320, but each copies
        //all four source lanes into its own original stack temporary first.
        //Do not replace this subtype's return-delay200 write with15ACB0.
        for(std::size_t call=0;call<e.type1PositionCalls.size();++call){
            auto& position=e.type1PositionCalls[call];
            if(position.argumentF12||position.copyMask||(call&&e.type1PositionCalls[call-1].copyMask!=15))return false;
            at(call?0x27d8f0u:0x27d8d8u);
            at(call?0x27d8f4u:0x27d8dcu);position.argumentF12=0x3f000000u;
            at(call?0x27d8f8u:0x27d8e0u);at(call?0x27d8fcu:0x27d8e4u);
            if(!row.descriptor04||!row.sourceDescriptor04||*row.sourceDescriptor04!=row.descriptor04.get())return false;
            const auto& descriptor=*row.descriptor04;
            at(call?0x27d900u:0x27d8e8u);at(call?0x27d904u:0x27d8ecu);
            at(0x15cff0);if(descriptor.sourceOffsetKnown!=15)return false;
            at(0x15cff4);at(0x15cff8);at(0x15cffc);
            position.temporary00=descriptor.sourceOffset320;position.copyMask=1;
            at(0x15d000);for(std::size_t lane=0;lane<4;++lane){
                if(e.known[0x30/4+lane]!=0xffffffffu)return false;
                position.previous30[lane]=e.words[0x30/4+lane];
            }
            at(0x15d004);for(std::size_t lane=0;lane<4;++lane)
                e.word(0x40+static_cast<std::uint32_t>(lane)*4,position.previous30[lane]);
            position.copyMask=3;
            at(0x15d008);at(0x15d00c);for(std::size_t lane=0;lane<4;++lane)
                e.word(0x30+static_cast<std::uint32_t>(lane)*4,position.temporary00[lane]);
            position.copyMask=7;
            at(0x15d010);at(0x15d014);at(0x15d018);
            e.word(0x200,*position.argumentF12);position.copyMask=15;
        }
    }else if(!subtype12&&!h.configModelNonNull){
        //Original invokes15ACB0 twice. The second copy deliberately makes
        //previous40 equal current30; do not collapse away either occurrence.
        for(std::uint32_t call:{subtype0?0x27d840u:(subtype2?0x27d990u:(subtype9?0x27dad0u:(subtype11?0x27db70u:0x27da30u))),subtype0?0x27d850u:(subtype2?0x27d9a0u:(subtype9?0x27dae0u:(subtype11?0x27db80u:0x27da40u)))}){
            at(call);const auto& descriptor=*row.descriptor04;
            at(0x15acb0);if(descriptor.sourceOffsetKnown!=15)return false;
            at(0x15acc0);for(std::size_t n=0;n<4;++n)if(e.known[0x30/4+n]!=0xffffffffu)return false;
            at(0x15acc4);for(std::size_t n=0;n<4;++n)e.word(0x40+static_cast<std::uint32_t>(n)*4,e.words[0x30/4+n]);
            at(0x15acd4);for(std::size_t n=0;n<4;++n)e.word(0x30+static_cast<std::uint32_t>(n)*4,descriptor.sourceOffset320[n]);
            ++e.positionCopyCount;
        }
    }
    at(subtype0?0x27d858u:(subtype1?0x27d908u:(subtype2?0x27d9a8u:(subtype9?0x27dae8u:(subtype11?0x27db88u:(subtype12?0x27dc00u:0x27da48u))))));if(!row.unknown10||!h.rowStart10Stored)return false;
    e.sourceStartOn=*row.unknown10!=0;
    if(subtype12){at(0x27dc04);at(0x27dc08);} //The actual row10 branch and NOP delay.
    //Fresh source emitter has actual child18=NULL. These childless base
    //notifications cannot activate descendants or invoke a prepared owner.
    if(e.entity.firstChild18||e.entity.nativeFlags29)return false;
    if(*e.sourceStartOn){
        if(subtype12){at(0x27dc0c);at(0x27dc10);}
        at(subtype0?0x27d86cu:(subtype1?0x27d91cu:(subtype2?0x27d9bcu:(subtype9?0x27dafcu:(subtype11?0x27db9cu:(subtype12?0x27dc14u:0x27da5cu))))));
        if(subtype12){at(0x27dc18);for(std::uint32_t p=0x15ace0;p<0x15acf4;p+=4)at(p);}
        at(0x15acf4);
        e.words[0x1d4/4]|=2u;e.known[0x1d4/4]|=2u; //Only known lowbits; unknown upper bytes preserved.
        if(subtype12)at(0x15acf8);
        at(0x15acfc);e.word(0x19c,0);e.particleActivationReturned=true;
        if(subtype12){at(0x27dc1c);at(0x27dc20);}
        at(subtype0?0x27d87cu:(subtype1?0x27d92cu:(subtype2?0x27d9ccu:(subtype9?0x27db0cu:(subtype11?0x27dbacu:(subtype12?0x27dc24u:0x27da6cu))))));
        if(subtype12){at(0x27dc28);at(0x141560);}
        at(0x141578);e.entity.flags29=static_cast<std::uint8_t>(e.entity.flags29|1u);
        at(0x14158c);e.entity.flags29=static_cast<std::uint8_t>(e.entity.flags29|2u);e.entityActivityStored=true;
        at(0x1414cc);at(0x14151c);e.entityActivityReturned=true;
    }else{
        if(subtype12){at(0x27dc34);at(0x27dc38);}
        at(subtype0?0x27d894u:(subtype1?0x27d944u:(subtype2?0x27d9e4u:(subtype9?0x27db24u:(subtype11?0x27dbc4u:(subtype12?0x27dc3cu:0x27da84u))))));
        if(subtype12){
            at(0x27dc40);at(0x160490);
            if(e.type12StopMode1D8||e.type12StopActiveBit||e.type12StopWriteMask||e.type12StopReturned||
               e.known[0x1d8/4]!=0xffffffffu)return false;
            e.type12StopMode1D8=e.words[0x1d8/4];at(0x160494);at(0x160498);at(0x16049c);
            if(*e.type12StopMode1D8==1){
                at(0x1604a0);if(!(e.known[0x1d4/4]&2u))return false;
                e.type12StopActiveBit=(e.words[0x1d4/4]&2u)!=0;
                at(0x1604a4);at(0x1604a8);at(0x1604ac);at(0x1604b0);
                if(*e.type12StopActiveBit){at(0x1604b4);return false;}
            }
            //Fresh constructor bit1 is clear. Both admitted branches skip the
            //active-only1E4 comparison; no clock value or timer result is invented.
            for(std::uint32_t p=0x1604d0;p<0x1604ec;p+=4)at(p);
            at(0x1604ec);e.words[0x1d4/4]&=~2u;e.known[0x1d4/4]|=2u;e.type12StopWriteMask=1;
            for(std::uint32_t p=0x1604f0;p<0x160500;p+=4)at(p);
            at(0x160500);e.words[0x1d4/4]&=~4u;e.known[0x1d4/4]|=4u;e.type12StopWriteMask=3;
            at(0x160504);for(std::size_t n=0;n<4;++n)if(e.known[0x30/4+n]!=0xffffffffu)return false;
            at(0x160508);for(std::size_t n=0;n<4;++n)e.word(0x40+static_cast<std::uint32_t>(n)*4,e.words[0x30/4+n]);
            e.type12StopWriteMask=7;
            at(0x16050c);e.word(0x19c,0);e.type12StopWriteMask=15;
            at(0x160510);at(0x160514); //Fog's19C store is BEFORE JR; this delay is NOP.
            e.type12StopReturned=true;e.particleActivationReturned=true;
        }else if(subtype9){
            //Actual ExplosionParticleEmitter+84 is15F070. Only this fresh
            //cold invocation is joined; no active mode1/time predicate is invented.
            at(0x27db28);at(0x15f070);
            if(e.type9StopMode1D8||e.type9StopActiveBit||e.type9StopWriteMask||e.type9StopReturned||
               e.known[0x1d8/4]!=0xffffffffu)return false;
            e.type9StopMode1D8=e.words[0x1d8/4];at(0x15f074);at(0x15f078);at(0x15f07c);
            if(*e.type9StopMode1D8==1){
                at(0x15f080);if(!(e.known[0x1d4/4]&2u))return false;
                e.type9StopActiveBit=(e.words[0x1d4/4]&2u)!=0;
                at(0x15f084);at(0x15f088);at(0x15f08c);at(0x15f090);
                if(*e.type9StopActiveBit){at(0x15f094);return false;}
            }
            //Both admitted branches reach15F0B0. All4 previous/current
            //lanes retain genuine source values; unknown upper flag bits remain unknown.
            for(std::uint32_t p=0x15f0b0;p<0x15f0cc;p+=4)at(p);
            at(0x15f0cc);e.words[0x1d4/4]&=~2u;e.known[0x1d4/4]|=2u;e.type9StopWriteMask=1;
            for(std::uint32_t p=0x15f0d0;p<0x15f0e0;p+=4)at(p);
            at(0x15f0e0);e.words[0x1d4/4]&=~4u;e.known[0x1d4/4]|=4u;e.type9StopWriteMask=3;
            at(0x15f0e4);for(std::size_t n=0;n<4;++n)if(e.known[0x30/4+n]!=0xffffffffu)return false;
            at(0x15f0e8);for(std::size_t n=0;n<4;++n)e.word(0x40+static_cast<std::uint32_t>(n)*4,e.words[0x30/4+n]);
            e.type9StopWriteMask=7;
            at(0x15f0ec);e.word(0x19c,0);e.type9StopWriteMask=15;at(0x15f0f0);at(0x15f0f4);
            e.type9StopReturned=true;e.particleActivationReturned=true;
        }else{
        at(0x15ad1c);e.words[0x1d4/4]&=~2u;e.known[0x1d4/4]|=2u;
        at(0x15ad30);e.words[0x1d4/4]&=~4u;e.known[0x1d4/4]|=4u;
        at(0x15ad34);for(std::size_t n=0;n<4;++n)if(e.known[0x30/4+n]!=0xffffffffu)return false;
        at(0x15ad38);for(std::size_t n=0;n<4;++n)e.word(0x40+static_cast<std::uint32_t>(n)*4,e.words[0x30/4+n]);
        at(0x15ad40);e.word(0x19c,0);e.particleActivationReturned=true;
        }
        if(subtype12){at(0x27dc44);at(0x27dc48);}
        at(subtype0?0x27d8a4u:(subtype1?0x27d954u:(subtype2?0x27d9f4u:(subtype9?0x27db34u:(subtype11?0x27dbd4u:(subtype12?0x27dc4cu:0x27da94u))))));
        if(subtype12){at(0x27dc50);at(0x141520);}
        at(0x141538);e.entity.flags29=static_cast<std::uint8_t>(e.entity.flags29&~1u);
        at(0x14154c);e.entity.flags29=static_cast<std::uint8_t>(e.entity.flags29&~2u);e.entityActivityStored=true;
        at(0x14146c);at(0x1414bc);e.entityActivityReturned=true;
    }
    if(subtype12){at(*e.sourceStartOn?0x27dc2cu:0x27dc54u);at(*e.sourceStartOn?0x27dc30u:0x27dc58u);}
    if(subtype11){at(*e.sourceStartOn?0x27dbb4u:0x27dbdcu);at(*e.sourceStartOn?0x27dbb8u:0x27dbe0u);}
    if(subtype9){at(*e.sourceStartOn?0x27db14u:0x27db3cu);at(*e.sourceStartOn?0x27db18u:0x27db40u);}
    if(subtype2){at(*e.sourceStartOn?0x27d9d4u:0x27d9fcu);at(*e.sourceStartOn?0x27d9d8u:0x27da00u);}
    if(subtype1){at(*e.sourceStartOn?0x27d934u:0x27d95cu);at(*e.sourceStartOn?0x27d938u:0x27d960u);}
    if(subtype0&&*e.sourceStartOn){at(0x27d888);at(0x27dc74);}
    else at(0x27dc70);
    if(subtype1||subtype2||subtype9||subtype11||subtype12)for(std::uint32_t p=0x27dc74;p<0x27dc88;p+=4)at(p);
    at(0x27dc88);e.rowConfigReturned=true;
    const bool attached=h.configModelNonNull;
    at(attached?(subtype0?0x27cd5cu:(subtype1?0x27cdb0u:(subtype2?0x27ce04u:(subtype9?0x27cea0u:(subtype11?0x27ceecu:(subtype12?0x27cf38u:0x27ce54u)))))):0x27d720u);
    if(!sourceParticleList3D8_||*sourceParticleList3D8_!=nativeParticleList3D8_.get()||
       row.sourceEmitterPointers[*h.configEmitterIndex]!=&e.entity||!precedingFogCurrent()||!successorWrapperCurrent()||
       !preflightSourceWorldParticleDescriptorsNative())return false;
    if(subtype12){
        if(attached)for(std::uint32_t p=0x27cf3c;p<0x27cf50;p+=4)at(p);
        else for(std::uint32_t p=0x27d724;p<0x27d734;p+=4)at(p);
    }
    at(attached?(subtype0?0x27cd78u:(subtype1?0x27cdccu:(subtype2?0x27ce20u:(subtype9?0x27ceb8u:(subtype11?0x27cf04u:(subtype12?0x27cf50u:0x27ce6cu)))))):0x27d734u);
    if(subtype12)at(attached?0x27cf54u:0x27d738u); //Actual registration mask402 in JAL delay.
    auto& list=*nativeParticleList3D8_;
    const auto registered=worldApplication_->entityAuthority_.attachChildren(list.entity,e.entity,0x402,
        worldApplication_->attachmentLock423F58());
    at(registered.sourcePC);
    if(registered.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
    e.sourceRegistered=true;
    if(nonFogAfterFog){
        const auto resume=attached?(subtype0?0x27cd80u:(subtype1?0x27cdd4u:(subtype2?0x27ce28u:
            (subtype9?0x27cec0u:(subtype11?0x27cf0cu:0x27ce74u))))):0x27d73cu;
        at(resume);
        if(!precedingFogCurrent()||!preflightSourceWorldParticleDescriptorsNative())return false;
        at(resume+4u); //Actual branch delay: NOP attached, descriptor04 LW unattached.
    }
    if(successorFog){
        //Continue only this registered occurrence, with both count events
        //and its own still-live descriptor/manager/model custody intact.
        at(attached?0x27cf58u:0x27d73cu);
        if(!successorWrapperCurrent()||!preflightSourceWorldParticleDescriptorsNative())return false;
    }
    if(subtype12&&!attached){
        at(0x27d73c);at(0x27d740); //Taken branch loads this row's descriptor in its delay.
    }
    if(subtype11&&!nonFogAfterFog){
        if(attached){at(0x27cf0c);at(0x27cf10);}
        else{at(0x27d73c);at(0x27d740);}
    }
    if(subtype9&&!nonFogAfterFog){
        if(attached){at(0x27cec0);at(0x27cec4);}
        else{at(0x27d73c);at(0x27d740);}
    }
    if(subtype2&&!nonFogAfterFog){
        if(attached){at(0x27ce28);at(0x27ce2c);}
        else{at(0x27d73c);at(0x27d740);}
    }
    if(subtype1&&!nonFogAfterFog){
        if(attached){at(0x27cdd4);at(0x27cdd8);}
        else{at(0x27d73c);at(0x27d740);}
    }
    const bool detailedRowTail=subtype12||nonFogAfterFog;
    at(attached?0x27cf58:0x27d754);
    if(attached){
        //Actual S0++ occurs after this configuration/registration, not when
        //the model name matched. Remaining emitter iterations are not reused.
        if(!row.descriptor04||!row.descriptor04->instances330||h.attachmentMatches)return false;
        at(0x27cf5c);++h.attachmentMatches;h.matchCountAdvanced=true;
        at(0x27cf60);const auto requested=*row.descriptor04->instances330;
        if(detailedRowTail)at(0x27cf64);
        at(0x27cf68);if(detailedRowTail)at(0x27cf6c);
        if(static_cast<std::int32_t>(h.attachmentMatches)<requested){at(0x27cf70);return false;}
        at(0x27cf90);at(0x27cf94);at(0x27cf98);
        if(detailedRowTail){at(0x27cf9c);at(0x27cfa0);}
        at(0x27cfc8);
        //World3C8 is read before the sufficient-count branch. A typed known
        //NULL receipt already exists in this source world; no toy adoption.
        if(!sourceToy3C8_||*sourceToy3C8_||!preflightSourceWorldToyNative())return false;
        if(detailedRowTail){at(0x27cfcc);at(0x27cfd0);at(0x27cfd4);at(0x27cfd8);}
        at(0x27d680);
        auto& shared=g::OriginalApplicationEntities50LA::processParticleAssertion27C820();
        if(!shared.known||shared.thread!=std::this_thread::get_id()||shared.completionGuard3D652C>1||
           shared.completionAssertion443070!=shared.completionGuard3D652C)return false;
        h.completionGuardRead=shared.completionGuard3D652C;
        if(detailedRowTail){at(0x27d684);at(0x27d688);}
        if(!*h.completionGuardRead){
            if(detailedRowTail){at(0x27d68c);at(0x27d690);}
            at(0x27d694);shared.completionAssertion443070=1;
            at(0x27d698);shared.completionGuard3D652C=1;h.completionLazyStored=true;}
        if(detailedRowTail)at(0x27d69c);
        at(0x27d6a0);h.completionAssertionRead=shared.completionAssertion443070;
        if(detailedRowTail)for(std::uint32_t p=0x27d6a4;p<0x27d6c4;p+=4)at(p);
        at(0x27d6c4); //True133660 predicate returns0 regardless of debug mode.
        if(detailedRowTail)at(0x27d6c8); //XORI makes the reached >=required predicate true in the call delay.
        if(static_cast<std::int32_t>(h.attachmentMatches)<*row.descriptor04->instances330){at(0x13366c);return false;}
        at(0x1336a8);if(detailedRowTail){at(0x1336ac);at(0x1336b0);}
        h.completionAssertionReturned=true;
        if(detailedRowTail){at(0x27d6cc);at(0x27d6d0);}
        at(0x27d750);
    }
    at(0x27d754);
    if(!precedingFogCurrent()||!successorWrapperCurrent()||!preflightSourceWorldParticleDescriptorsNative()||!row.descriptor04||
       !row.sourceDescriptor04||*row.sourceDescriptor04!=row.descriptor04.get()||
       !a::OriginalConfigProcessGlobals50LA::descriptorDelete2E8B00AdmittedNative())return false;
    const auto& retiring=*row.descriptor04;
    if(!retiring.sourceLoaded||!retiring.instances330||!retiring.startOn334||retiring.sourceOffsetKnown!=15)return false;
    //No live pointer is retained in this proof snapshot. Inline strings and
    //offsets were copied into emitter storage; manager/model pins survive.
    NativeParticleRow27DE00::DescriptorHistory proof;
    proof.type=static_cast<std::uint32_t>(retiring.type04);proof.count=*retiring.instances330;
    proof.start=*retiring.startOn334;proof.offset=retiring.sourceOffset320;
    h.descriptorDeleteAdmitted=true;row.sourceDescriptorHistory=proof;
    if(detailedRowTail)at(0x27d758); //Actual delete call's NOP delay; native allocator adaptation is admitted above.
    //The pending row descriptor read is a source borrow, not ownership.
    //Retire it with its allocation; native cancellation is a distinct event.
    if(rowIndex>0){sourceParticleNextRows_[rowIndex-1].descriptor=nullptr;
        sourceParticleNextRows_[rowIndex-1].descriptorSourceRetired=true;}
    row.descriptor04.reset();row.sourceDescriptorFreed=true;h.descriptorFreed=true;
    at(0x27d75c);at(0x27d760);row.sourceDescriptor04=nullptr;
    if(detailedRowTail)for(std::uint32_t p=0x27d764;p<0x27d790;p+=4)at(p);
    at(0x27d790);row.sourceRowServiceReturned=true;h.rowServiceReturned=true;
    at(0x27e018);
    return continueSourceParticleNextRow27E018(rowIndex+1,pc);
}
bool Owner::preflightSourceParticleNextRowNative()const noexcept {
    for(std::size_t row=1;row<=4;++row)if(!preflightSourceParticleNextRowNative(row))return false;
    return true;
}
bool Owner::preflightSourceParticleNextRowNative(std::size_t rowIndex)const noexcept {
    if(rowIndex<1||rowIndex>4)return false;
    const auto& n=sourceParticleNextRows_[rowIndex-1];
    if(!n.entered)return !n.offsetAdvanced&&!n.rowAdvanced&&!n.countRead&&!n.rowRead&&!n.dispatched&&
        !n.descriptorRead&&!n.row&&!n.descriptor&&!n.table14&&!n.target0C&&!n.type04&&!n.emitterIsNone&&
        !n.descriptorSourceRetired&&!n.nativeWithdrawn&&!n.listReturned;
    const auto& previous=rowIndex==1?sourceParticleService124_:sourceParticleLaterRows_[rowIndex-2];
    const bool afterFog=previous.emitterType==12u;
    const bool afterSuccessorFog=afterFog&&rowIndex>1&&(rowIndex==2?sourceParticleService124_:
        sourceParticleLaterRows_[rowIndex-3]).emitterType==12u;
    const bool afterNonFogFollowingFog=bool(sourceNonFogPrecedingFogRowNative(rowIndex-1));
    const bool boundedParent=afterFog||afterNonFogFollowingFog;
    if(!previous.rowServiceReturned||!nativeParticleList3D8_||
       (rowIndex>1&&(!sourceParticleNextRows_[rowIndex-2].dispatched||
                    sourceParticleNextRows_[rowIndex-2].rowS0!=rowIndex-1))||
       n.offsetS1!=4u*(n.offsetAdvanced?rowIndex:rowIndex-1)||n.rowS0!=(n.rowAdvanced?rowIndex:rowIndex-1)||
       (n.rowAdvanced&&!n.offsetAdvanced)||(n.countRead&&!n.rowAdvanced)||
       n.countRead!=bool(n.count30)||(n.count30&&*n.count30!=nativeParticleList3D8_->count30)||
       (n.rowRead&&!n.countRead)||(n.table14&&!n.rowRead)||(n.target0C&&n.table14!=0x3d2570u)||
       (n.dispatched&&n.target0C!=0x27c7b0u)||(n.descriptorRead&&!n.dispatched)||
       (n.emitterIsNone&&!n.descriptorRead)||(n.type04&&n.emitterIsNone!=false))return false;
    if(rowIndex==4){
        //Own World return and caller dispatch have distinct histories.
        //The new dispatch still cannot authorize trigger body effects.
        if(boundedParent){
            const auto& caller=sourcePostCollisionCaller_;const auto mask=caller.particleWrapperReturnMask;
            if((mask!=0&&mask!=1&&mask!=3&&mask!=7&&mask!=15)||
               (mask&&!n.listReturned)||bool(caller.particleReturnV0)!=bool(mask)||
               (caller.particleReturnV0&&caller.particleReturnV0!=1u)||
               caller.particleReturned!=(mask==15)||!preflightSourcePostParticleCallerNative())return false;
        }
        //This is the real terminating parent iteration, not a fifth row.
        if(n.rowRead||n.dispatched||n.descriptorRead||n.row||n.descriptor||n.table14||n.target0C||n.type04||
           n.emitterIsNone||n.descriptorSourceRetired)return false;
        return !n.listReturned||(n.countRead&&n.rowAdvanced&&n.offsetAdvanced&&n.count30&&
            static_cast<std::int32_t>(n.rowS0)>=*n.count30);
    }
    if(n.listReturned)return false;
    const auto& h=sourceParticleLaterRows_[rowIndex-1];
    const bool supportedNonFog=n.type04==0u||n.type04==1u||n.type04==2u||n.type04==8u||n.type04==9u||n.type04==11u;
    //Only supported non-Fog rows may extend this Fog-derived chain.
    //Another Fog or unsupported type retains its inspection-only boundary.
    if(boundedParent&&(h.entered||h.listRead||h.listNonNull||h.listEntered||h.listTable08||h.listTarget78||
       h.rowTable14||h.rowTarget0C||h.rowS0||h.count30||h.offsetS1||h.rowRead))return false;
    if((afterNonFogFollowingFog||afterSuccessorFog)&&!supportedNonFog&&
       (h.rowEntered||h.descriptorRead||h.emitterIsNone||h.descriptorType04||
       h.attachmentOffsetE0||h.attachmentMatched||h.attachmentPhysics||h.childLookupPending||h.toyScanNullRead||
       h.configTarget10||h.configEmitterIndex||h.matchCountAdvanced||h.completionGuardRead||
       h.descriptorDeleteAdmitted||h.descriptorFreed||h.rowServiceReturned))return false;
    if(afterFog&&n.type04!=12u&&!supportedNonFog&&(h.attachmentOffsetE0||h.attachmentMatched||h.attachmentPhysics||
       h.childLookupPending||h.toyScanNullRead||h.configTarget10||h.configEmitterIndex||
       h.matchCountAdvanced||h.completionGuardRead||h.descriptorDeleteAdmitted||h.descriptorFreed||h.rowServiceReturned))return false;
    //The caller seeds these observations together from this exact dispatched
    //descriptor. An isolated optional/flag cannot authorize a creation prefix.
    if(boundedParent&&(h.descriptorRead!=h.rowEntered||bool(h.emitterIsNone)!=h.rowEntered||
       bool(h.descriptorType04)!=h.rowEntered))return false;
    //Remaining service receipts depend on these same creation seeds.
    //This validator never calls descriptor preflight or dereferences Track.
    if(h.entered||h.listRead||h.listEntered||h.rowRead)return false;
    if(h.rowEntered&&(!n.type04||h.descriptorType04!=n.type04||h.emitterIsNone!=false||!h.descriptorRead))return false;
    if(h.rowType08Stored&&!h.rowEntered)return false;
    if(h.guard3D6528Read&&(!h.rowType08Stored||*h.guard3D6528Read>1))return false;
    if(h.assertionLazyStored&&(!h.guard3D6528Read||*h.guard3D6528Read))return false;
    if(h.assertion443068Read&&(!h.guard3D6528Read||*h.assertion443068Read!=1||
       (!*h.guard3D6528Read&&!h.assertionLazyStored)))return false;
    if(h.manager4A0Read&&!h.assertion443068Read)return false;
    if(h.manager4A0NonNull&&!h.manager4A0Read)return false;
    if(h.assertionReturnV0&&(!h.manager4A0NonNull||*h.assertionReturnV0))return false;
    if(h.instances330Read&&!h.assertionReturnV0)return false;
    if(h.countPositive&&(!h.instances330Read||*h.instances330Read<=0))return false;
    if(h.emitterType&&(!h.countPositive||h.emitterType!=h.descriptorType04))return false;
    if(bool(h.emitterType)!=bool(h.emitterAllocationBytes)||bool(h.emitterType)!=bool(h.emitterConstructor))return false;
    if(h.emitterArrayAttempted&&!h.emitterType)return false;
    if(h.emitterAttempts&&(!h.emitterArrayAttempted||h.emitterAttempts>static_cast<std::size_t>(*h.instances330Read)))return false;
    if(h.emitterPublishedSlots>h.emitterAttempts)return false;
    if(h.emitterArrayPublished&&(!h.emitterArrayAttempted||h.emitterPublishedSlots!=static_cast<std::size_t>(*h.instances330Read)))return false;
    if(h.rowCount0CStored&&!h.emitterArrayPublished)return false;
    if(h.rowStart10Stored&&!h.rowCount0CStored)return false;
    if(h.attachmentOffsetE0&&!h.rowStart10Stored)return false;
    if(h.attachmentIsNone&&!h.attachmentOffsetE0)return false;
    if(h.attachmentZone&&(h.attachmentIsNone!=false||*h.attachmentZone>10))return false;
    if(h.attachmentSlot&&!h.attachmentZone)return false;
    if(h.attachmentLiveZones&&(!h.attachmentZone||*h.attachmentLiveZones!=10))return false;
    if(h.attachmentLiveCount&&!h.attachmentSlot)return false;
    if(h.attachmentMatches!=(h.matchCountAdvanced?1u:0u))return false;
    if(h.matchCountAdvanced&&(h.attachmentIsNone!=false||!(h.emitterType==0u?h.type0LoadEntered:(h.emitterType==1u?h.type1LoadEntered:(h.emitterType==2u?h.type2LoadEntered:(h.emitterType==9u?h.type9LoadEntered:(h.emitterType==11u?h.type11LoadEntered:(h.emitterType==12u?h.type12LoadEntered:h.type8LoadEntered))))))))return false;
    if(h.completionGuardRead&&(!h.matchCountAdvanced||*h.completionGuardRead>1))return false;
    if(h.completionLazyStored&&(!h.completionGuardRead||*h.completionGuardRead))return false;
    if(h.completionAssertionRead&&(!h.completionGuardRead||*h.completionAssertionRead!=1))return false;
    if(h.completionAssertionReturned&&!h.completionAssertionRead)return false;
    if(h.descriptorDeleteAdmitted&&(h.attachmentIsNone==false&&!h.completionAssertionReturned))return false;
    if(h.descriptorFreed&&!h.descriptorDeleteAdmitted)return false;
    if(h.rowServiceReturned&&!h.descriptorFreed)return false;
    if(n.descriptorSourceRetired!=h.descriptorFreed)return false;
    if(h.attachmentMatched&&(!h.attachmentSlot||!h.attachmentLiveCount||*h.attachmentSlot>=*h.attachmentLiveCount))return false;
    if(h.attachmentPhysics&&!h.attachmentMatched)return false;
    if(h.childIsNone&&(!h.attachmentMatched||h.attachmentPhysics))return false;
    if(h.childLookupPending&&h.childIsNone!=false)return false;
    if(h.configEntered&&(!h.configTarget10||*h.configTarget10!=0x27d7a0u||!h.configEmitterIndex||
       *h.configEmitterIndex!=0||!h.attachmentIsNone||
       (h.attachmentIsNone==false&&(!h.attachmentMatched||h.childIsNone!=true))))return false;
    if(h.configModelNonNull&&(!h.configEntered||!h.attachmentMatched))return false;
    if(h.configEmitterNonNull&&!h.configEntered)return false;
    if(h.configManagerRead&&!h.configEntered)return false;
    if(h.configLoaderTarget&&!h.configManagerRead)return false;
    if(h.type0LoadEntered&&(h.configLoaderTarget!=0x15b9d0u||h.emitterType!=0u))return false;
    if(h.type0NoOpReturned&&!h.type0LoadEntered)return false;
    if(h.type0ManagerStored&&(!h.type0NoOpReturned||!h.configEmitterNonNull))return false;
    if(h.type0ModelStored&&(!h.type0ManagerStored||!h.configModelNonNull))return false;
    if(h.type0OffsetStored&&!h.type0ModelStored)return false;
    if(h.type0DirectoryCopied&&!h.type0ManagerStored)return false;
    if(h.type0FilenameAppended&&!h.type0DirectoryCopied)return false;
    if(h.type1LoadEntered&&(h.configLoaderTarget!=0x15ce90u||h.emitterType!=1u))return false;
    if(h.type1NoOpReturned&&!h.type1LoadEntered)return false;
    if(h.type1ManagerStored&&(!h.type1NoOpReturned||!h.configEmitterNonNull))return false;
    if(h.type1ModelStored&&(!h.type1ManagerStored||!h.configModelNonNull))return false;
    if(h.type1OffsetStored&&!h.type1ModelStored)return false;
    if(h.type1DirectoryCopied&&!h.type1ManagerStored)return false;
    if(h.type1FilenameAppended&&!h.type1DirectoryCopied)return false;
    if(h.type2LoadEntered&&(h.configLoaderTarget!=0x15d6d0u||h.emitterType!=2u))return false;
    if(h.type2NoOpReturned&&!h.type2LoadEntered)return false;
    if(h.type2ManagerStored&&(!h.type2NoOpReturned||!h.configEmitterNonNull))return false;
    if(h.type2ModelStored&&(!h.type2ManagerStored||!h.configModelNonNull))return false;
    if(h.type2OffsetStored&&!h.type2ModelStored)return false;
    if(h.type2DirectoryCopied&&!h.type2ManagerStored)return false;
    if(h.type2FilenameAppended&&!h.type2DirectoryCopied)return false;
    if(h.type8LoadEntered&&(h.configLoaderTarget!=0x15def0u||h.emitterType!=8u))return false;
    if(h.type8NoOpReturned&&!h.type8LoadEntered)return false;
    if(h.type8ManagerStored&&(!h.type8NoOpReturned||!h.configEmitterNonNull))return false;
    if(h.type8ModelStored&&(!h.type8ManagerStored||!h.configModelNonNull))return false;
    if(h.type8OffsetStored&&!h.type8ModelStored)return false;
    if(h.type8DirectoryCopied&&!h.type8ManagerStored)return false;
    if(h.type8FilenameAppended&&!h.type8DirectoryCopied)return false;
    if(h.type9LoadEntered&&(h.configLoaderTarget!=0x15e8f0u||h.emitterType!=9u))return false;
    if(h.type9NoOpReturned&&!h.type9LoadEntered)return false;
    if(h.type9ManagerStored&&(!h.type9NoOpReturned||!h.configEmitterNonNull))return false;
    if(h.type9ModelStored&&(!h.type9ManagerStored||!h.configModelNonNull))return false;
    if(h.type9OffsetStored&&!h.type9ModelStored)return false;
    if(h.type9DirectoryCopied&&!h.type9ManagerStored)return false;
    if(h.type9FilenameAppended&&!h.type9DirectoryCopied)return false;
    if(h.type11LoadEntered&&(h.configLoaderTarget!=0x15f1e0u||h.emitterType!=11u))return false;
    if(h.type11NoOpReturned&&!h.type11LoadEntered)return false;
    if(h.type11ManagerStored&&(!h.type11NoOpReturned||!h.configEmitterNonNull))return false;
    if(h.type11ModelStored&&(!h.type11ManagerStored||!h.configModelNonNull))return false;
    if(h.type11OffsetStored&&!h.type11ModelStored)return false;
    if(h.type11DirectoryCopied&&!h.type11ManagerStored)return false;
    if(h.type11FilenameAppended&&!h.type11DirectoryCopied)return false;
    if(h.type12LoadEntered&&(h.configLoaderTarget!=0x15ff10u||h.emitterType!=12u))return false;
    if(h.type12NoOpReturned&&!h.type12LoadEntered)return false;
    if(h.type12ManagerStored&&(!h.type12NoOpReturned||!h.configEmitterNonNull))return false;
    if(h.type12ModelStored&&(!h.type12ManagerStored||!h.configModelNonNull))return false;
    if(h.type12OffsetStored&&!h.type12ModelStored)return false;
    if(h.type12DirectoryCopied&&!h.type12ManagerStored)return false;
    if(h.type12FilenameAppended&&!h.type12DirectoryCopied)return false;
    //A registered Fog may retire this descriptor; next-row entry remains separately bounded.
    if(h.emitterConfigConstructAttempted&&!(h.emitterType==0u?h.type0FilenameAppended:(h.emitterType==1u?h.type1FilenameAppended:(h.emitterType==2u?h.type2FilenameAppended:(h.emitterType==9u?h.type9FilenameAppended:(h.emitterType==11u?h.type11FilenameAppended:(h.emitterType==12u?h.type12FilenameAppended:h.type8FilenameAppended)))))))return false;
    if(h.emitterConfigConstructReturned&&!h.emitterConfigConstructAttempted)return false;
    if(h.emitterConfigOpenAttempted&&!h.emitterConfigConstructReturned)return false;
    if(h.emitterConfigOpenReturned&&!h.emitterConfigOpenAttempted)return false;
    if(h.emitterConfigTarget94&&(h.emitterConfigOpenReturned!=true||*h.emitterConfigTarget94!=(h.emitterType==0u?0x15c330u:(h.emitterType==1u?0x15d560u:(h.emitterType==2u?0x15dd00u:(h.emitterType==9u?0x15ef70u:(h.emitterType==11u?0x15fc10u:(h.emitterType==12u?0x1602b0u:0x15e6d0u))))))))return false;
    if(h.toyScanNullRead&&(!h.attachmentZone||!h.attachmentLiveZones||
       *h.attachmentZone!=*h.attachmentLiveZones||h.attachmentMatched))return false;
    if(n.nativeWithdrawn)return !n.row&&!n.descriptor;
    const auto& list=*nativeParticleList3D8_;
    if(n.rowRead){
        if(n.rowS0>=list.rows.size()||!list.sourceSlotPublished[n.rowS0]||!list.sourceRowReturned[n.rowS0]||
           !list.pointers34||!list.sourceArray34||*list.sourceArray34!=list.pointers34.get()||
           !list.rows[n.rowS0]||n.row!=list.rows[n.rowS0].get()||list.pointers34[n.rowS0]!=n.row)return false;
        if(n.table14&&*n.table14!=n.row->sourceVtable14)return false;
    }else if(n.row)return false;
    if(n.descriptorRead){
        if(!n.row||!n.row->sourceDescriptorPublished||!n.row->sourceDescriptor04)return false;
        if(n.descriptorSourceRetired){
            if(n.descriptor||n.row->descriptor04||*n.row->sourceDescriptor04||!n.row->sourceDescriptorFreed||
               !n.row->sourceDescriptorHistory||!n.type04||n.row->sourceDescriptorHistory->type!=*n.type04||
               !h.instances330Read||n.row->sourceDescriptorHistory->count!=*h.instances330Read||
               !h.attachmentOffsetE0||n.row->sourceDescriptorHistory->offset!=*h.attachmentOffsetE0)return false;
        }else{
            if(!n.row->descriptor04||n.descriptor!=n.row->descriptor04.get()||*n.row->sourceDescriptor04!=n.descriptor||
               !n.descriptor->sourceMode||!n.descriptor->sourceLoaded)return false;
            if(n.type04&&(!(n.descriptor->sourceKnownMask&128u)||*n.type04!=static_cast<std::uint32_t>(n.descriptor->type04)))return false;
            if(h.instances330Read&&n.descriptor->instances330!=h.instances330Read)return false;
            if(h.attachmentOffsetE0&&(n.descriptor->sourceOffsetKnown!=15||n.descriptor->sourceOffset320!=*h.attachmentOffsetE0))return false;
        }
        if(h.configEntered){
            if(!h.configEmitterIndex||*h.configEmitterIndex>=n.row->sourceEmitterPublishedSlots||
               !n.row->sourceEmitterPointers||h.configEmitterNonNull!=bool(n.row->sourceEmitters[*h.configEmitterIndex])||
               n.row->sourceEmitterPointers[*h.configEmitterIndex]!=(n.row->sourceEmitters[*h.configEmitterIndex]?
                   &n.row->sourceEmitters[*h.configEmitterIndex]->entity:nullptr)||
               h.configModelNonNull!=(h.attachmentIsNone==false))return false;
        }
        if(h.configManagerRead&&(!sourceParticle4A0_||*sourceParticle4A0_!=trackParticles_.get()||
           !sourceParticleLoadReturned_||sourceParticleStageNativeClosed_||sourceParticle4A0NativeWithdrawn_))return false;
        if(h.attachmentMatched){
            const auto z=*h.attachmentZone,slot=*h.attachmentSlot;
            if(!sourceStaticOwner_||sourceStaticOwner_->closed_||sourceModelOwner3C4_!=sourceStaticOwner_.get()||
               sourceStaticOwner_->sourceWorld3C4Borrower_!=this||z>=sourceStaticOwner_->parents_.size()||
               !sourceStaticOwner_->parents_[z]||!sourceModelParents3C4_||!sourceModels3C4_||
               *sourceModels3C4_!=sourceModelParents3C4_.get())return false;
            const auto& parent=*sourceStaticOwner_->parents_[z];
            if(sourceModelParents3C4_[z]!=&parent.entity||slot>=parent.sourceEntriesKnown||!parent.sourceEntries34||
               z>=sourceStaticOwner_->sourceWrapperLedger_.size()||slot>=sourceStaticOwner_->sourceWrapperLedger_[z].size()||
               !parent.sourceEntries34[slot]||parent.sourceEntries34[slot]!=sourceStaticOwner_->sourceWrapperLedger_[z][slot].get()||
               !sourceStaticOwner_->sourceModelCurrentNative(*parent.sourceEntries34[slot]))return false;
        }
        if((h.toyScanNullRead||h.completionGuardRead)&&(!sourceToy3C8_||*sourceToy3C8_||!preflightSourceWorldToyNative()))return false;
    }else if(n.descriptor)return false;
    return true;
}
bool Owner::continueSourceParticleNextRow27E018(std::size_t rowIndex,std::uint32_t& pc)noexcept {
    if(rowIndex<1||rowIndex>4)return false;
    auto& n=sourceParticleNextRows_[rowIndex-1];
    const auto& previous=rowIndex==1?sourceParticleService124_:sourceParticleLaterRows_[rowIndex-2];
    const bool afterFog=previous.emitterType==12u;
    const bool afterSuccessorFog=afterFog&&rowIndex>1&&(rowIndex==2?sourceParticleService124_:
        sourceParticleLaterRows_[rowIndex-3]).emitterType==12u;
    const bool afterNonFogFollowingFog=bool(sourceNonFogPrecedingFogRowNative(rowIndex-1));
    const bool boundedParent=afterFog||afterNonFogFollowingFog;
    if(pc!=0x27e018||n.entered||!previous.rowServiceReturned||
       !preflightSourcePostParticleModelsNative()||!nativeParticleList3D8_)return false;
    auto& list=*nativeParticleList3D8_;
    if(afterNonFogFollowingFog&&!sourceNonFogAfterFogCustodyCurrentNative(rowIndex-1))return false;
    if(afterSuccessorFog){
        const auto& completed=list.rows[rowIndex-1];
        if(!completed||previous.configEmitterIndex!=0u||completed->sourceEmitters.empty()||
           !completed->sourceEmitters[0])return false;
        const auto& emitter=*completed->sourceEmitters[0];const auto& population=emitter.type12Population;
        //Producer-only native custody check. Historical preflight remains
        //structural after withdrawal, and source count128 is not read again.
        if(!emitter.sourceRegistered||emitter.nativeTopologyWithdrawn||!emitter.rowConfigReturned||
           !emitter.type12LoaderReturned||!emitter.configNativeWithdrawn||emitter.borrowsWithdrawn||
           !emitter.managerStored||!emitter.manager70||emitter.manager70!=trackParticles_.get()||
           emitter.manager70!=population.manager||!population.returned||population.countStep!=4||
           !population.countStore.stored||!emitter.manager70->sourceFogSuccessorCountReceiptCurrentNative(
               population.countStore,&emitter,&population))return false;
    }
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    //Copy actual retained parent registers, not a new inferred loop index.
    if(rowIndex==1){
        if(!sourceParticleService124_.rowS0||!sourceParticleService124_.offsetS1)return false;
        n.rowS0=static_cast<std::uint32_t>(*sourceParticleService124_.rowS0);
        n.offsetS1=*sourceParticleService124_.offsetS1;
    }else{
        const auto& prior=sourceParticleNextRows_[rowIndex-2];
        if(!prior.dispatched||prior.rowS0!=rowIndex-1||prior.offsetS1!=4u*(rowIndex-1))return false;
        n.rowS0=prior.rowS0;n.offsetS1=prior.offsetS1;
    }
    n.entered=true;
    //Parent ignores row V0; descriptor-load receipts never stand in for it.
    at(0x27e018);n.offsetS1+=4;n.offsetAdvanced=true;
    at(0x27e01c);++n.rowS0;n.rowAdvanced=true;
    at(0x27e020);n.count30=list.count30;n.countRead=true;
    at(0x27e024);at(0x27e028);if(boundedParent)at(0x27e02c); //NOP delay on both parent-loop edges.
    if(static_cast<std::int32_t>(n.rowS0)>=*n.count30){
        if(rowIndex!=4||*n.count30!=4)return false;
        //Reached only through every preceding real row service return.
        if(boundedParent){
            for(std::uint32_t p=0x27e030;p<=0x27e04c;p+=4)at(p);
            n.listReturned=true;at(0x2743a0);
            return continueSourceParticleWorldReturn2743A0(pc);
        }
        at(0x27e044);n.listReturned=true;at(0x27e04c);
        at(0x2743a0);sourcePostCollisionCaller_.particleReturnV0=1;
        at(0x2743ac);sourcePostCollisionCaller_.particleReturned=true;
        at(0x1aa2cc);
        if(!sourceLoadRace_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
           worldEntity_.reference00!=1||sourcePostCollisionCaller_.worldTable08!=0x3cf110u)return false;
        at(0x1aa2d0);at(0x1aa2d4);sourcePostCollisionCaller_.target144=0x274e90u;
        at(0x1aa2d8);at(0x1aa2dc);at(0x274e90);
        return continueSourceTriggerService274E90(pc);
    }
    if(rowIndex>=4)return false;
    at(0x27dffc);
    if(!list.sourceArrayPublished||!list.sourceArray34||*list.sourceArray34!=list.pointers34.get()||
       !list.pointers34||n.rowS0>=list.rows.size()||!list.sourceSlotPublished[n.rowS0]||
       !list.sourceRowReturned[n.rowS0]||!list.rows[n.rowS0]||list.pointers34[n.rowS0]!=list.rows[n.rowS0].get())return false;
    if(boundedParent)at(0x27e000); //Actual table+retained offset address, after the same array ownership guard.
    at(0x27e004);n.row=list.rows[n.rowS0].get();n.rowRead=true;
    at(0x27e008);n.table14=n.row->sourceVtable14;if(*n.table14!=0x3d2570u)return false;
    at(0x27e00c);n.target0C=0x27c7b0u;
    at(0x27e010);at(0x27e014);at(0x27c7b0);n.dispatched=true;
    if(boundedParent)for(std::uint32_t p=0x27c7b4;p<0x27c7dc;p+=4)at(p);
    at(0x27c7dc);
    if(!n.row->sourceDescriptorPublished||!n.row->sourceDescriptor04||!n.row->descriptor04||
       *n.row->sourceDescriptor04!=n.row->descriptor04.get()||!n.row->descriptor04->sourceLoaded)return false;
    n.descriptor=n.row->descriptor04.get();n.descriptorRead=true;const auto& d=*n.descriptor;
    if(boundedParent)for(std::uint32_t p=0x27c7e0;p<0x27c7f0;p+=4)at(p);
    at(0x27c7f0);if(boundedParent)at(0x27c7f4); //Same descriptor's inline210 string in the comparison call delay.
    if(!d.sourceEmitterBytes||*d.sourceEmitterBytes>=d.sourceEmitter210.size()||d.sourceEmitter210[*d.sourceEmitterBytes])return false;
    bool none=*d.sourceEmitterBytes==4;constexpr char target[]="none";
    for(std::size_t i=0;i<*d.sourceEmitterBytes;++i){
        auto c=static_cast<unsigned char>(d.sourceEmitter210[i]);if(!c||c>=128)return false;
        if(c>='A'&&c<='Z')c=static_cast<unsigned char>(c+32);
        if(i>=4||c!=static_cast<unsigned char>(target[i]))none=false;
    }
    n.emitterIsNone=none;at(0x27c7f8);if(boundedParent)at(0x27c7fc);
    if(none){at(0x27c800);return false;} //Authentic diagnostic remains bounded.
    at(0x27c814);if(!(d.sourceKnownMask&128u)||*n.row->sourceDescriptor04!=&d)return false;
    at(0x27c818);n.type04=static_cast<std::uint32_t>(d.type04);
    const bool supportedNonFog=n.type04==0u||n.type04==1u||n.type04==2u||n.type04==8u||n.type04==9u||n.type04==11u;
    if((afterNonFogFollowingFog||afterSuccessorFog)&&!supportedNonFog){
        at(0x27c81c);return false; //Before next creation seeds; no further population.
    }
    auto& creation=sourceParticleLaterRows_[rowIndex-1];
    if(creation.rowEntered)return false;
    creation.rowEntered=true;creation.descriptorRead=true;creation.emitterIsNone=n.emitterIsNone;
    creation.descriptorType04=n.type04;
    at(0x27c81c);if(!continueSourceParticleRowCreation27C81C(n.rowS0,creation,pc))return false;
    if(afterFog&&creation.emitterType!=12u&&!supportedNonFog)return false; //Unsupported successors retain their construction bound.
    return continueSourceParticleRowAttachment27CC08(n.rowS0,creation,pc);
}
bool Owner::sourceCompletedParticleWorldCustodyCurrentNative()const noexcept {
    const auto& h=sourcePostCollisionCaller_;const auto& parent=sourceParticleNextRows_[3];
    const auto& service=sourceParticleService124_;const auto& finalService=sourceParticleLaterRows_[2];
    const bool finalFog=finalService.emitterType==12u;
    const bool finalNonFog=bool(sourceNonFogPrecedingFogRowNative(3));
    if((!finalFog&&!finalNonFog)||!h.particleDispatched||h.target124!=0x274370u||
       h.worldTable08!=0x3cf110u||!parent.listReturned||parent.rowS0!=4||parent.offsetS1!=16||
       parent.count30!=4||!service.entered||!service.listRead||!service.listNonNull||!service.listEntered||
       service.listTable08!=0x3d24f0u||service.listTarget78!=0x27dfd0u||
       nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       !worldColdConstructed_||!sourceLoadRace_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       !sourceLoadApplication_||sourceLoadApplication_->entities()!=worldApplication_||
       !worldApplication_||!worldApplication_->constructed_||worldApplication_->released_||
       sceneApplication_!=worldApplication_||worldEntity_.reference00!=1||
       !preflightSourcePostParticleModelsNative()||!preflightSourceWorldParticleDescriptorsNative()||
       !nativeParticleList3D8_||!sourceParticleList3D8_||
       *sourceParticleList3D8_!=nativeParticleList3D8_.get()||!trackParticles_)return false;
    const auto& list=*nativeParticleList3D8_;
    //All four callbacks must have actually returned, each with its own
    //retired descriptor and still-live configured emitter/Config/registry.
    for(std::size_t i=0;i<4;++i){
        const auto& row=list.rows[i];const auto& rowService=i?sourceParticleLaterRows_[i-1]:service;
        if(!row||!row->sourceRowServiceReturned||!rowService.rowServiceReturned||
           !row->sourceDescriptorFreed||!row->sourceDescriptorHistory||!rowService.descriptorFreed||
           rowService.configEmitterIndex!=0u||row->sourceEmitters.empty()||!row->sourceEmitters[0])return false;
        const auto& emitter=*row->sourceEmitters[0];
        if(!emitter.constructorReturned||!emitter.counted||!emitter.enrolled||!emitter.sourceRegistered||
           emitter.nativeTopologyWithdrawn||!emitter.rowConfigReturned||!emitter.emitterFileLoadReturned||
           !emitter.configNativeWithdrawn||emitter.borrowsWithdrawn||!emitter.managerStored||
           emitter.manager70!=trackParticles_.get()||emitter.entity.sessionGeneration!=worldEntity_.sessionGeneration)return false;
    }
    if(finalNonFog){
        if(!sourceNonFogAfterFogCustodyCurrentNative(3))return false;
    }else{
        const auto& fog=*list.rows[3]->sourceEmitters[0];const auto& population=fog.type12Population;
        const auto& manager=*trackParticles_;
        if(fog.dispatch08!=0x3cd930u||!fog.type12LoaderReturned||population.manager!=&manager||
           !population.returned||population.pc!=0x160b00u||fog.type12PopulationPending||
           !manager.sourceActiveCount128Known_)return false;
        //Current epoch, not historical first-publication validity at count2.
        //This producer guard adds no original128 read or new source event.
        if(sourceParticleLaterRows_[1].emitterType==12u){
            if(population.countStep!=4||!population.countStore.stored||manager.sourceActiveCount128_!=2||
               !manager.sourceFogSuccessorCountReceiptCurrentNative(population.countStore,&fog,&population))return false;
        }else if(population.countStore.stored){
            if(population.countStep!=4||manager.sourceActiveCount128_!=1||
               !manager.sourceFogCountReceiptCurrentNative(population.countStore,&fog,&population))return false;
        }else if(!population.retirementEmpty||population.volumePositive!=false||population.countStep||
            population.countStore.pc||manager.sourceActiveCount128_!=0||
            !manager.sourceFogCountReceiptCurrentNative(population.countStore,&fog,&population))return false;
    }
    return true;
}
bool Owner::continueSourceParticleWorldReturn2743A0(std::uint32_t& pc)noexcept {
    auto& h=sourcePostCollisionCaller_;
    if(pc!=0x2743a0||h.particleWrapperReturnMask||h.particleReturnV0||h.particleReturned||h.target144||
       !sourceCompletedParticleWorldCustodyCurrentNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    //Exact original epilogue. The mask records native continuation history,
    //not fabricated guest RA/stack bytes or the list's V0 reused as World V0.
    at(0x2743a0);h.particleReturnV0=1;h.particleWrapperReturnMask=1;
    at(0x2743a4);h.particleWrapperReturnMask=3;
    at(0x2743a8);h.particleWrapperReturnMask=7;
    at(0x2743ac);h.particleWrapperReturnMask=15;h.particleReturned=true;
    at(0x1aa2cc);return continueSourcePostParticleCaller1AA2CC(pc);
}
bool Owner::preflightSourcePostParticleCallerNative()const noexcept {
    const auto& h=sourcePostParticleCaller_;const auto& prior=sourcePostCollisionCaller_;
    const bool bounded=sourceParticleLaterRows_[2].emitterType==12u||
        bool(sourceNonFogPrecedingFogRowNative(3));
    //Old generic non-Fog dispatch remains separate. Historical validation
    //uses scalar receipts only, including after World/Track withdrawal.
    if(!bounded)return !h.step&&!h.worldTable08;
    if(h.step>5||bool(h.worldTable08)!=(h.step>=2)||
       (h.worldTable08&&h.worldTable08!=0x3cf110u)||
       bool(prior.target144)!=(h.step>=3)||(prior.target144&&prior.target144!=0x274e90u))return false;
    if(h.step&&(prior.particleWrapperReturnMask!=15||prior.particleReturnV0!=1u||!prior.particleReturned||
       !prior.particleDispatched||prior.target124!=0x274370u||!sourceParticleNextRows_[3].listReturned))return false;
    return !sourceTriggerService274E90_.entered||h.step==5; //Only the own completed JALR/delay may enter.
}
bool Owner::continueSourcePostParticleCaller1AA2CC(std::uint32_t& pc)noexcept {
    auto& h=sourcePostParticleCaller_;auto& prior=sourcePostCollisionCaller_;
    if(pc!=0x1aa2cc||h.step||h.worldTable08||prior.target144||prior.particleWrapperReturnMask!=15||
       prior.particleReturnV0!=1u||!prior.particleReturned||sourceTriggerService274E90_.entered||
       !sourceCompletedParticleWorldCustodyCurrentNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x1aa2cc);
    //Actual fresh Race74 owner lookup. Do not reuse the earlier A0/table
    //receipt as this instruction's result or expose a raw pointer history.
    if(sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_)return false;
    h.step=1;
    at(0x1aa2d0);
    //Typed source facet of this authenticated concrete RA World: original
    //1B2898 stores3CF110. Native Entity has no arbitrary guest vtable lane.
    h.worldTable08=0x3cf110u;h.step=2;
    at(0x1aa2d4);prior.target144=0x274e90u;h.step=3; //Actual table word3CF254.
    at(0x1aa2d8);h.step=4; //JALR supplies the own caller continuation1AA2E0.
    at(0x1aa2dc);h.step=5; //NOP delay executes before transfer.
    at(0x274e90);return continueSourceTriggerService274E90(pc);
}
bool Owner::sourceTriggerFogPortalGraphLiveNative(std::size_t z)const noexcept {
    if(z>=10||sourcePortalGraphPhase_!=SourcePortalGraphPhase::returned||sourcePortalGraphPC_!=0x272c90u||
       !sourcePortalCohortAdmitted_||sourcePortalGraphS0_!=10u||!sourcePortalParameterFreeReached_||
       !sourcePortalParameterNullStored_||!preflightSourceWorldPortalGraphNative())return false;
    for(std::size_t i=0;i<10;++i){const auto& p=nativePortals_[i];
        if(sourcePortalFrames_[i].withdrawn||sourcePortalCallerMask_[i]!=63u||!p||!p->sourceMode||
           !p->attached||!p->source.loadReturned||p->source.worldLinksProduced!=4u)return false;
        for(const auto& link:sourcePortalWorldLinks_[i])if(link.withdrawn||!link.attempted||
           !link.published||link.pending||link.knownMask!=31u)return false;
    }
    const auto& node=sourceSearchNodes_[1][z];
    return sourceTopologyMode_&&!sourceTopologyWithdrawn_&&sourceSearchNodesPhase_==SourceSearchNodesPhase::returned&&
        sourceCollision4C4_&&sourceCollisionSlots4C4_&&*sourceCollision4C4_==sourceCollisionSlots4C4_.get()&&
        z<sourceCollisionPublished_&&node.owner&&!node.withdrawn&&node.published&&node.counted&&node.enrolled&&
        node.ordinalKnown&&node.constructorMask==511&&node.owner->constructed&&node.owner->ordinal48==z&&
        sourceCollisionSlots4C4_[z]==node.owner.get()&&preflightSourceSearchNodeNative(node,z,true)&&
        sourcePortalWorldLinksCurrent(*node.owner,true,z);
}
bool Owner::preflightSourceTriggerServiceNative()const noexcept {
    if(!preflightSourcePostParticleCallerNative())return false;
    const auto& s=sourceTriggerService274E90_;const bool fogCaller=sourcePostParticleCaller_.step!=0;
    if(!s.fogGeometry.historyCurrent()||(s.fogGeometry.step&&(!fogCaller||s.fogBodyStep!=16))||
       !s.fogMembership.historyCurrent()||(s.fogMembership.step&&(!fogCaller||s.fogBodyStep!=16||s.fogGeometry.step!=22))||
       !s.fogBounds.historyCurrent()||(s.fogBounds.step&&(!fogCaller||s.fogBodyStep!=16||s.fogGeometry.step!=22||s.fogMembership.step!=11)))return false;
    if(!s.fogPrimary.historyCurrent()||(s.fogPrimary.step&&(!fogCaller||s.fogBounds.tailStep!=100)))return false;
    if(s.fogPrimary.nativeCenter){
        if(s.fogPrimary.nativeCenter!=s.fogBounds.commonResult||(*s.fogPrimary.nativeHalf)[3]!=0x3f800000u)return false;
        for(unsigned k=0;k<3;++k)if(!s.fogBounds.scalarOuter[k]||(*s.fogPrimary.nativeHalf)[k]!=*s.fogBounds.scalarOuter[k])return false;
    }
    if(s.fogBounds.selectedNonNull&&s.fogBounds.selectedNonNull!=s.fogMembership.heapNonNull)return false;
    if(s.fogBounds.centerXYZ){if(!s.fogGeometry.center00)return false;
        for(unsigned k=0;k<3;++k)if((*s.fogBounds.centerXYZ)[k]!=(*s.fogGeometry.center00)[k])return false;}
    if(s.fogBounds.radius50&&s.fogBounds.radius50!=s.fogGeometry.radius50)return false;
    if(s.fogBounds.commonResult&&s.fogBounds.commonResult!=s.fogGeometry.transformedCenter)return false;
    if(s.fogBounds.matrixWords){if(!s.fogGeometry.matrix50)return false;
        for(unsigned k=0;k<s.fogBounds.matrixWords;++k)if(s.fogBounds.matrix10[k]!=(*s.fogGeometry.matrix50)[k])return false;}
    if(!fogCaller&&(s.fogPrefixStep||s.fogBodyStep||s.fogDescriptorStep||s.fogAssertionPredicate||s.fogAssertionReturnV0))return false;
    if(fogCaller){
        const auto step=s.fogPrefixStep;const auto& first=s.rows[0];
        //Pure scalar history, retained after native withdrawal. No live
        //loaded-owner or Fog-count predicate may be called from this check.
        if(step>12||s.entered!=(step>=1)||s.listRead!=(step>=2)||s.listNonNull!=(step>=2)||
           bool(s.listTable08)!=(step>=3)||bool(s.listTarget7C)!=(step>=4)||s.listEntered!=(step>=5)||
           bool(s.currentRow)!=(step>=7)||(s.currentRow&&*s.currentRow!=(first.parentAdvanced?1u:0u))||
           bool(s.freshCount30)!=(step>=8)||(s.freshCount30&&s.freshCount30!=2u)||
           bool(first.rowIndex)!=(step>=10)||(first.rowIndex&&first.rowIndex!=0u)||
           bool(first.count30)!=(step>=10)||(first.count30&&first.count30!=2u)||
           first.rowRead!=(step>=11)||first.rowNonNull!=(step>=11)||
           s.listReturned||s.wrapperReturned||s.listReturnV0||s.wrapperReturnV0)return false;
        const auto body=s.fogBodyStep;
        if(first.geometryReturned!=(s.fogGeometry.step==22))return false;
        if(body>16||(body&&step!=12)||
           bool(first.guard3D686CRead)!=(body>=1)||
           first.assertionLazyStored!=(body>=2&&first.guard3D686CRead==0u)||
           bool(first.assertion444618Read)!=(body>=3)||bool(first.worldZones34CRead)!=(body>=4)||
           bool(first.zone08)!=(body>=5)||bool(s.fogAssertionPredicate)!=(body>=6)||
           bool(s.fogAssertionReturnV0)!=(body>=7)||first.assertionReturned!=(body>=7)||
           bool(first.descriptorTable6C)!=(body>=9)||bool(first.descriptorTarget0C)!=(body>=10)||
           first.descriptorEntered!=(body>=11)||bool(first.callbackZone08)!=(body>=13)||
           first.collisionArrayRead!=(body>=14)||first.collisionNodeRead!=(body>=15)||
           first.trigger00Read!=(body>=16))return false;
        if((first.worldZones34CRead&&*first.worldZones34CRead!=10u)||
           (first.callbackZone08&&first.callbackZone08!=first.zone08)||
           (body>=15&&(*first.callbackZone08<0||*first.callbackZone08>=10)))return false;
        if(s.fogAssertionPredicate&&*s.fogAssertionPredicate!=
           (static_cast<std::int32_t>(*first.worldZones34CRead)<*first.zone08?0u:1u))return false;
        if(s.fogAssertionReturnV0&&(s.fogAssertionPredicate!=1u||*s.fogAssertionReturnV0))return false;
        for(std::size_t i=0;i<s.rows.size();++i){const auto& row=s.rows[i];
            if(i&&(row.rowIndex||row.count30||row.rowRead||row.rowNonNull||
               row.guard3D686CRead||row.assertionLazyStored||row.assertion444618Read||row.worldZones34CRead||
               row.zone08||row.assertionReturned||row.descriptorTable6C||row.descriptorTarget0C||row.descriptorEntered||
               row.callbackZone08||row.collisionArrayRead||row.collisionNodeRead||row.trigger00Read))return false;
            if(i&&(row.geometryReturned||row.triggerCallbackReturned||row.actionLoopEntered||row.actionIndex||row.actionOffset||row.actionCount||
               row.actionRead||row.actionTable108||row.actionTarget0C||row.actionReturned||row.actionReturnV0||
               row.geometrySourceFreed||row.descriptorReturned||row.descriptorReturnV0||row.parentOperandsRead||
               row.registrationReturned||row.parentAdvanced||row.nextRow))return false;
            if(!i&&row.triggerCallbackReturned!=s.fogPrimary.returnFlow.triggerReturned())return false;
        }
        const auto tail=s.fogDescriptorStep;
        if(tail>18||(tail&&!first.triggerCallbackReturned)||first.actionLoopEntered!=(tail>=1)||
           bool(first.actionIndex)!=(tail>=1)||(first.actionIndex&&*first.actionIndex!=(tail>=9?1u:0u))||
           bool(first.actionOffset)!=(tail>=2)||(first.actionOffset&&*first.actionOffset!=(tail>=8?4u:0u))||
           bool(first.actionCount)!=(tail>=3)||(first.actionCount&&*first.actionCount!=1u)||
           first.actionRead!=(tail>=4)||bool(first.actionTable108)!=(tail>=5)||
           (first.actionTable108&&*first.actionTable108!=0x3cf0b0u)||bool(first.actionTarget0C)!=(tail>=6)||
           (first.actionTarget0C&&*first.actionTarget0C!=0x1b5ad0u)||first.actionReturned!=(tail>=7)||
           bool(first.actionReturnV0)!=(tail>=7)||(first.actionReturnV0&&*first.actionReturnV0!=1u)||
           first.geometrySourceFreed!=(tail>=12)||bool(first.descriptorReturnV0)!=(tail>=13)||
           (first.descriptorReturnV0&&*first.descriptorReturnV0!=1u)||first.descriptorReturned!=(tail>=14)||
           first.parentOperandsRead!=(tail>=15)||first.registrationReturned!=(tail>=16)||
           first.parentAdvanced!=(tail>=17)||bool(first.nextRow)!=(tail>=17)||(first.nextRow&&*first.nextRow!=1u))return false;
    }
    if(!s.entered&&(s.listRead||s.listEntered||s.nativeWithdrawn||s.currentRow||s.freshCount30))return false;
    if((s.entered&&(sourcePostCollisionCaller_.target144!=0x274e90u||!sourcePostCollisionCaller_.particleReturned))||
       (s.listNonNull&&!s.listRead)||(s.listTable08&&(!s.listNonNull||*s.listTable08!=0x3d5230u))||
       (s.listTarget7C&&s.listTable08!=0x3d5230u)||(s.listEntered&&s.listTarget7C!=0x2dcb30u)||
       (s.currentRow&&(!s.listEntered||*s.currentRow>2u))||(s.freshCount30&&!s.currentRow)||
       (s.listReturned&&(!s.currentRow||!s.freshCount30||
           static_cast<std::int32_t>(*s.currentRow)<static_cast<std::int32_t>(*s.freshCount30)))||
       (s.listReturnV0&&(!s.listReturned||*s.listReturnV0!=1u))||
       (s.wrapperReturned&&(!s.listReturned||s.listReturnV0!=1u))||
       (s.wrapperReturnV0&&(!s.wrapperReturned||*s.wrapperReturnV0!=1u)))return false;
    for(std::size_t i=0;i<s.rows.size();++i){const auto& row=s.rows[i];
        if(!preflightSourceTriggerServiceRowNative(row,i))return false;
        if(row.rowIndex&&(!s.currentRow||*s.currentRow<i||(i&&!s.rows[i-1].parentAdvanced)))return false;
        if(row.parentAdvanced&&(!s.currentRow||*s.currentRow<*row.nextRow))return false;
        if(s.currentRow&&i<*s.currentRow&&!row.parentAdvanced)return false;
    }
    if(!s.nativeWithdrawn&&s.listRead){
        if(sourceTriggerPhase_!=SourceTriggerPhase::listReturned||!preflightSourceWorldTriggerNative()||
           !sourceTrigger3D0_||*sourceTrigger3D0_!=nativeTriggerList3D0_.get()||s.listNonNull!=bool(nativeTriggerList3D0_))return false;
        if(s.listNonNull){const auto& list=*nativeTriggerList3D0_;
            if(s.listTable08&&(!list.sourceVtableKnown||*s.listTable08!=list.sourceVtable08))return false;
            if(s.freshCount30&&(!list.sourceCountKnown||*s.freshCount30!=list.count30))return false;}
    }
    return true;
}
bool Owner::preflightSourceTriggerServiceRowNative(const SourceTriggerServiceRow& h,std::size_t rowIndex)const noexcept {
    const auto& s=sourceTriggerService274E90_;
    if((h.rowIndex&&(!s.listEntered||*h.rowIndex!=rowIndex))||(h.count30&&!h.rowIndex)||(h.rowRead&&!h.count30)||
       (h.rowNonNull&&!h.rowRead)||(h.guard3D686CRead&&(!h.rowNonNull||*h.guard3D686CRead>1))||
       (h.assertionLazyStored&&(!h.guard3D686CRead||*h.guard3D686CRead))||
       (h.assertion444618Read&&(!h.guard3D686CRead||*h.assertion444618Read!=1||
           (!*h.guard3D686CRead&&!h.assertionLazyStored)))||
       (h.worldZones34CRead&&!h.assertion444618Read)||(h.zone08&&!h.worldZones34CRead)||
       (h.assertionReturned&&(!h.zone08||!h.worldZones34CRead||
           static_cast<std::int32_t>(*h.worldZones34CRead)<*h.zone08))||
       (h.descriptorTable6C&&!h.assertionReturned)||(h.descriptorTarget0C&&h.descriptorTable6C!=0x3d52c0u)||
       (h.descriptorEntered&&h.descriptorTarget0C!=0x2dc270u)||(h.callbackZone08&&!h.descriptorEntered)||
       (h.collisionArrayRead&&!h.callbackZone08)||(h.collisionNodeRead&&!h.collisionArrayRead)||
       (h.trigger00Read&&!h.collisionNodeRead)||(h.geometryReturned&&!h.trigger00Read)||
       (h.triggerCallbackReturned&&!h.geometryReturned)||(h.actionLoopEntered&&!h.triggerCallbackReturned)||
       (h.actionIndex&&!h.actionLoopEntered)||(h.actionOffset&&!h.actionIndex)||(h.actionCount&&!h.actionOffset)||
       (h.actionRead&&!h.actionCount)||(h.actionTable108&&!h.actionRead)||
       (h.actionTarget0C&&h.actionTable108!=0x3cf0b0u)||(h.actionReturned&&h.actionTarget0C!=0x1b5ad0u)||
       (h.actionReturnV0&&(!h.actionReturned||*h.actionReturnV0!=1u))||
       (h.geometrySourceFreed&&(!h.actionLoopEntered||!h.actionIndex||!h.actionCount||
           static_cast<std::int32_t>(*h.actionIndex)<static_cast<std::int32_t>(*h.actionCount)))||
       (h.descriptorReturned&&!h.geometrySourceFreed)||
       (h.descriptorReturnV0&&(!h.descriptorReturned||*h.descriptorReturnV0!=1u))||
       (h.parentOperandsRead&&!h.descriptorReturned)||(h.registrationReturned&&!h.parentOperandsRead)||
       (h.parentAdvanced&&(!h.rowIndex||!h.rowRead||(h.rowNonNull&&!h.registrationReturned)))||
       (h.nextRow.has_value()!=h.parentAdvanced)||(h.nextRow&&*h.nextRow!=rowIndex+1u))return false;
    if(s.nativeWithdrawn||!h.rowIndex)return true;
    if(s.listRead){
        if(sourceTriggerPhase_!=SourceTriggerPhase::listReturned||!preflightSourceWorldTriggerNative()||
           !sourceTrigger3D0_||*sourceTrigger3D0_!=nativeTriggerList3D0_.get()||
           s.listNonNull!=bool(nativeTriggerList3D0_))return false;
        if(s.listNonNull){const auto& list=*nativeTriggerList3D0_;
            if(s.listTable08&&(!list.sourceVtableKnown||*s.listTable08!=list.sourceVtable08))return false;
            if(h.count30&&(!list.sourceCountKnown||*h.count30!=list.count30))return false;
            if(h.rowRead){
                if(!h.rowIndex||*h.rowIndex>=list.descriptorSlotsPublished||*h.rowIndex>=list.descriptorOwners.size()||
                   !list.sourceArrayKnown||!list.descriptors34||list.descriptors34[*h.rowIndex]!=list.descriptorOwners[*h.rowIndex].get()||
                   h.rowNonNull!=bool(list.descriptorOwners[*h.rowIndex]))return false;
                if(h.rowNonNull){const auto& row=*list.descriptorOwners[*h.rowIndex];
                    if(!row.sourceFields)return false;const auto& f=*row.sourceFields;
                    if(h.zone08&&(!(f.known&64u)||*h.zone08!=f.zone08))return false;
                    if(h.callbackZone08&&(!(f.known&64u)||*h.callbackZone08!=f.zone08))return false;
                    if(h.descriptorTable6C&&(!(f.known&1u)||*h.descriptorTable6C!=f.vtable6C))return false;
                    if(h.trigger00Read&&!sourceTriggerGeometryCurrentNative(row,h.geometryReturned))return false;
                    if(rowIndex==0&&sourcePostParticleCaller_.step&&h.trigger00Read&&
                       (!sourceTriggerFogGeometryCurrentNative(row,s.fogGeometry)||
                        !sourceTriggerFogMembershipCurrentNative(row,s.fogMembership)||
                        !sourceTriggerFogBoundsCurrentNative(row,s.fogBounds)||
                        !sourceTriggerFogPrimaryCurrentNative(row,s.fogPrimary)))return false;
                    if(sourceTriggerFogPortalHeadPinnedNative(row)&&(!h.callbackZone08||*h.callbackZone08<0||
                       !sourceTriggerFogPortalGraphLiveNative(static_cast<std::size_t>(*h.callbackZone08))))return false;
                    if(h.triggerCallbackReturned&&!sourceTriggerCallbackReturnedNative(row))return false;
                    if(h.actionCount&&*h.actionCount!=static_cast<std::uint32_t>(row.actionCount04))return false;
                    if(h.actionRead&&(!row.action||!row.actions68||row.actionSlotsPublished!=1||row.actions68[0]!=row.action.get()))return false;
                    if(h.actionTable108&&*h.actionTable108!=row.action->vtable108)return false;
                    if(h.actionReturned&&!sourceTriggerActionInstalledNative(row))return false;
                    if(h.registrationReturned&&!sourceTriggerRegistrationCurrentNative(row))return false;
                    if(h.geometrySourceFreed&&(!f.geometrySourceFreed||!f.geometrySourceNullStored||
                       !f.geometrySourceHistory||f.geometry64||!f.sourceGeometry64||*f.sourceGeometry64))return false;
                }
            }
        }
    }
    if(h.worldZones34CRead&&(!worldZones34C_||*worldZones34C_!=*h.worldZones34CRead))return false;
    if(h.collisionArrayRead&&(!sourceTopologyMode_||sourceTopologyWithdrawn_||!sourceCollision4C4_||
       !sourceCollisionSlots4C4_||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get()))return false;
    if(h.collisionNodeRead){
        if(!h.callbackZone08||*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)>=sourceCollisionPublished_||
           static_cast<std::uint32_t>(*h.callbackZone08)>=sourceSearchNodes_[1].size())return false;
        const auto z=static_cast<std::uint32_t>(*h.callbackZone08);const auto& node=sourceSearchNodes_[1][z];
        if(!node.owner||node.withdrawn||!node.published||!node.ordinalKnown||
           sourceCollisionSlots4C4_[z]!=node.owner.get()||!preflightSourceSearchNodeNative(node,z,true))return false;
        if(rowIndex==0&&sourcePostParticleCaller_.step&&s.fogMembership.step>=4){
            const auto& descriptor=nativeTriggerList3D0_->descriptorOwners[0];
            if(!descriptor||!sourceTriggerFogPrimaryNodeCurrentNative(*descriptor,node.owner.get()))return false;
        }
    }
    return true;
}
bool Owner::continueSourceTriggerFogBody2DCB6C(std::uint32_t& pc)noexcept {
    auto& s=sourceTriggerService274E90_;auto& h=s.rows[0];
    if(pc!=0x2dcb6c||sourcePostParticleCaller_.step!=5||s.fogPrefixStep!=12||s.fogBodyStep||
       s.nativeWithdrawn||s.currentRow!=0u||s.freshCount30!=2u||!h.rowRead||!h.rowNonNull||
       !sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
       !nativeTriggerList3D0_)return false;
    auto& list=*nativeTriggerList3D0_;
    //Fresh source slot reads must keep the genuine loaded descriptor, instance
    //and action. Cleanup-acceptable absent/withdrawn owners are not admission.
    const auto rowCurrent=[&]()noexcept {
        return sourceTriggerPhase_==SourceTriggerPhase::listReturned&&sourceTriggerPublished_&&
            sourceTrigger3D0_&&*sourceTrigger3D0_==&list&&list.sourceMode&&list.counted&&list.sourceEnrolled&&
            list.sourceCountKnown&&list.count30==2&&list.sourceArrayAllocationReturned&&list.sourceArrayKnown&&
            list.descriptors34&&list.descriptorSlotsPublished==2&&!list.sourceDescriptorNativeRetired[0]&&
            list.descriptorOwners[0]&&list.descriptors34[0]==list.descriptorOwners[0].get()&&
            sourceTriggerLoadedDescriptorCurrentNative(*list.descriptorOwners[0]);
    };
    if(!rowCurrent()||!list.descriptorOwners[0]->sourceFields)return false;
    auto& row=*list.descriptorOwners[0];const auto& f=*row.sourceFields;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x2dcb6c);auto& shared=g::OriginalApplicationEntities50LA::processParticleAssertion27C820();
    if(!worldApplication_||!worldApplication_->constructed_||worldApplication_->released_||
       !shared.known||shared.thread!=std::this_thread::get_id()||shared.triggerGuard3D686C>1||
       shared.triggerAssertion444618!=shared.triggerGuard3D686C)return false;
    h.guard3D686CRead=shared.triggerGuard3D686C;s.fogBodyStep=1;
    at(0x2dcb70);at(0x2dcb74); //BNE/NOP; guard1 skips the four cold setup instructions.
    if(!*h.guard3D686CRead){
        at(0x2dcb78);at(0x2dcb7c);at(0x2dcb80);shared.triggerAssertion444618=1;
        at(0x2dcb84);shared.triggerGuard3D686C=1;h.assertionLazyStored=true;
    }
    s.fogBodyStep=2;at(0x2dcb88);at(0x2dcb8c);h.assertion444618Read=shared.triggerAssertion444618;s.fogBodyStep=3;
    at(0x2dcb90);at(0x2dcb94); //Known closed-domain assertion1 does not skip the check.
    at(0x2dcb98);if(!rowCurrent())return false;
    at(0x2dcb9c);at(0x2dcba0);at(0x2dcba4);
    if(!worldZones34C_||*worldZones34C_!=10||sourceZonesPhase_!=SourceZonesPhase::returned||
       !sourceZones350_||!worldZoneInfo350_||*worldZoneInfo350_!=sourceZones350_.get()||
       sourceZonesLoaded_!=10||!preflightSourceWorldZonesNative())return false;
    h.worldZones34CRead=*worldZones34C_;s.fogBodyStep=4;
    at(0x2dcba8);at(0x2dcbac);at(0x2dcbb0);at(0x2dcbb4);if(!rowCurrent())return false;
    at(0x2dcbb8);if(!(f.known&64u))return false;h.zone08=f.zone08;s.fogBodyStep=5;
    at(0x2dcbbc);const bool less=static_cast<std::int32_t>(*h.worldZones34CRead)<*h.zone08;
    at(0x2dcbc0);at(0x2dcbc4);s.fogAssertionPredicate=less?0u:1u;s.fogBodyStep=6; //XORI in call delay.
    at(0x133660);at(0x133664);at(0x133668);
    if(less){at(0x13366c);return false;} //Actual debug36C100 is unknown; false assertion is not a success.
    //For true A0 both original debug branches produce V0=0. Summarize that
    //equivalent outcome without inventing a debug read or branch receipt.
    at(0x1336a8);at(0x1336ac);at(0x1336b0);s.fogAssertionReturnV0=0;h.assertionReturned=true;s.fogBodyStep=7;
    at(0x2dcbc8);at(0x2dcbcc); //BEQ V0,zero to2DCBF8, NOP delay; diagnostics/BREAK skipped.
    at(0x2dcbf8);if(!rowCurrent())return false;
    at(0x2dcbfc);at(0x2dcc00);at(0x2dcc04);if(!rowCurrent())return false;s.fogBodyStep=8;
    at(0x2dcc08);if(!(f.known&1u)||f.vtable6C!=0x3d52c0u)return false;
    h.descriptorTable6C=f.vtable6C;s.fogBodyStep=9;
    at(0x2dcc0c);h.descriptorTarget0C=0x2dc270u;s.fogBodyStep=10; //Original3D52CC.
    at(0x2dcc10);at(0x2dcc14); //JALR; delay copies the same World into A1, own return2DCC18.
    at(0x2dc270);h.descriptorEntered=true;s.fogBodyStep=11;
    for(std::uint32_t p=0x2dc274;p<=0x2dc284;p+=4)at(p);s.fogBodyStep=12;
    at(0x2dc288);if(!rowCurrent()||!(f.known&64u))return false;
    h.callbackZone08=f.zone08;s.fogBodyStep=13;
    at(0x2dc28c);
    if(!sourceTopologyMode_||sourceTopologyWithdrawn_||sourceSearchNodesPhase_!=SourceSearchNodesPhase::returned||
       !sourceCollision4C4_||!sourceCollisionSlots4C4_||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||
       !preflightSourceWorldSearchNodesNative())return false;
    h.collisionArrayRead=true;s.fogBodyStep=14;
    at(0x2dc290);at(0x2dc294);at(0x2dc298);at(0x2dc29c); //Retain descriptor/World; calculate actual slot.
    at(0x2dc2a0);
    //The source assertion is inclusive and permits negative zones. It does
    //NOT authorize a native pointer read; enforce the distinct real extent.
    if(*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)>=sourceCollisionPublished_||
       static_cast<std::uint32_t>(*h.callbackZone08)>=sourceSearchNodes_[1].size())return false;
    const auto z=static_cast<std::uint32_t>(*h.callbackZone08);const auto& node=sourceSearchNodes_[1][z];
    if(!node.owner||node.withdrawn||!node.published||!node.ordinalKnown||!node.counted||!node.enrolled||
       node.constructorMask!=511||!node.owner->constructed||node.owner->ordinal48!=z||
       sourceCollisionSlots4C4_[z]!=node.owner.get()||!preflightSourceSearchNodeNative(node,z,true))return false;
    h.collisionNodeRead=true;s.fogBodyStep=15;
    at(0x2dc2a4);at(0x2dc2a8);if(!rowCurrent()||!row.trigger00)return false;
    h.trigger00Read=true;s.fogBodyStep=16; //Actual call delay, exact live instance entity, not an observer.
    at(0x2a0a80);const bool geometry=refreshSourceTriggerFogGeometry2A0A80(row,s.fogGeometry,pc);
    sourcePostParticleModelsPC_=pc;h.geometryReturned=geometry;
    if(!geometry)return false; //Unsupported VU inputs retain the original partial effects/model bound.
    //Positive producer custody again, not merely a cleanup-valid old node receipt.
    //No new source slot read is claimed: this authenticates retainedS0 from2DC2A0.
    if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
       sourceTopologyWithdrawn_||sourceSearchNodesPhase_!=SourceSearchNodesPhase::returned||
       !sourceCollision4C4_||!sourceCollisionSlots4C4_||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||
       z>=sourceCollisionPublished_||!node.owner||node.withdrawn||!node.published||!node.ordinalKnown||
       !node.counted||!node.enrolled||node.constructorMask!=511||!node.owner->constructed||node.owner->ordinal48!=z||
       sourceCollisionSlots4C4_[z]!=node.owner.get()||!preflightSourceSearchNodeNative(node,z,true))return false;
    const bool published=beginSourceTriggerFogMembership120070(row,node.owner.get(),s.fogGeometry,s.fogMembership,pc);
    sourcePostParticleModelsPC_=pc;
    if(!published)return false;
    if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
       sourceTopologyWithdrawn_||!node.owner||node.withdrawn||!node.counted||!node.enrolled||
       !node.published||!node.ordinalKnown||node.constructorMask!=511||!node.owner->constructed||node.owner->ordinal48!=z||
       !sourceCollisionSlots4C4_||sourceCollisionSlots4C4_[z]!=node.owner.get()||!preflightSourceSearchNodeNative(node,z,true))return false;
    const bool bounds=continueSourceTriggerFogBounds1200F0(row,s.fogGeometry,s.fogMembership,s.fogBounds,pc);
    sourcePostParticleModelsPC_=pc;if(!bounds||pc!=0x120170u)return false;
    //Fresh native admission for the same retained node. No new source4C4 read.
    if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
       sourceTopologyWithdrawn_||!node.owner||node.withdrawn||!node.published||!node.ordinalKnown||
       !node.counted||!node.enrolled||node.constructorMask!=511||!node.owner->constructed||node.owner->ordinal48!=z||
       !sourceCollisionSlots4C4_||sourceCollisionSlots4C4_[z]!=node.owner.get()||!preflightSourceSearchNodeNative(node,z,true))return false;
    const auto actualMethod=[&](const void* occurrence,const void* actualCollider,
        NativeListMethodCustody*& method,std::uint32_t& methodPC)noexcept {
        methodPC=0x27f5a0u;
        //Leaf checks only: the outer Fog history is copied after this call.
        if(!sourceTriggerPrimaryInputsNative(row,occurrence,actualCollider)||!h.callbackZone08||*h.callbackZone08<0||
           static_cast<std::uint32_t>(*h.callbackZone08)!=z||!node.owner||node.withdrawn||!node.published||
           !node.counted||!node.enrolled||!node.ordinalKnown||node.constructorMask!=511||
           !node.owner->constructed||node.owner->ordinal48!=z||!sourceCollisionSlots4C4_||
           sourceCollisionSlots4C4_[z]!=node.owner.get()||!preflightSourceSearchNodeNative(node,z,true))return false;
        auto* actual=node.owner->nativeList34;
        if(!actual||!nativeMethods4CC_||nativeMethods4CC_[z]!=actual||!sourceListMethodArrayCurrent(1,z,actual))return false;
        const auto& publication=sourceListMethods_[1][z];
        if(!publication.arrayPublished||!publication.aliasPublished||!publication.aliasRetained||publication.withdrawn||
           publication.aliasNode!=node.owner.get()||!actual->sourceMode_||
           actual->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||actual->application_!=worldApplication_.get()||
           actual->ordinal_!=10u+z||actual->usage10_!=1||actual->reference00_!=2||
           !actual->source_.constructorReturned||actual->source_.vtable08!=0x3d2d30u||!sourceListEntriesCurrent(*actual))return false;
        method=actual;return true; //Actual node34 read, never node38/World4D4 or an ordinal-as-pointer.
    };
    (void)continueSourceTriggerFogPrimary120170(row,node.owner.get(),s.fogBounds,s.fogPrimary,actualMethod,pc);
    sourcePostParticleModelsPC_=pc;
    if(pc!=0x280bf0u)return false;
    //The preceding wrapper copied every publication effect. Full native root
    //admission is now safe, before a distinct portal prefix begins.
    if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
       !sourceTriggerFogPortalGraphLiveNative(z))return false;
    const auto finishFogDescriptor=[&]()noexcept {
        if(pc!=0x2dc2acu||s.fogDescriptorStep||!h.triggerCallbackReturned||!h.rowIndex||*h.rowIndex||
           !sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z)||!worldApplication_||worldApplication_->released_||
           sourceTopologyWithdrawn_||row.sourceActionNativeWithdrawn||!row.sourceInstance||
           row.sourceInstance->nativeClosed_||row.sourceInstance->membershipNativeWithdrawn_)return false;
        const auto liveRow=[&]()noexcept {
            return sourceTriggerPhase_==SourceTriggerPhase::listReturned&&sourceTriggerPublished_&&
                sourceTrigger3D0_&&*sourceTrigger3D0_==&list&&list.sourceMode&&list.counted&&list.sourceEnrolled&&
                list.sourceCountKnown&&list.count30==2&&list.sourceArrayKnown&&list.descriptors34&&
                list.descriptorSlotsPublished==2&&!list.sourceDescriptorNativeRetired[0]&&
                list.descriptorOwners[0].get()==&row&&list.descriptors34[0]==&row&&
                row.sourceDescriptorReturned&&row.sourceActionReaderComplete&&!row.sourceActionNativeWithdrawn&&
                row.sourceInstance&&row.sourceInstancePublished&&row.trigger00==&row.sourceInstance->entity_&&
                !row.sourceInstance->nativeClosed_&&!row.sourceInstance->membershipNativeWithdrawn_&&
                preflightSourceTriggerInstanceNative(row)&&preflightSourceTriggerActionNative(row);
        };
        if(!liveRow())return false;
        at(0x2dc2ac);h.actionLoopEntered=true;h.actionIndex=0;s.fogDescriptorStep=1;
        at(0x2dc2b0);at(0x2dc2b4);h.actionOffset=0;s.fogDescriptorStep=2; //Unconditional branch and offset-zero delay.
        at(0x2dc2e0);if(!liveRow()||row.actionCount04!=1)return false;
        h.actionCount=static_cast<std::uint32_t>(row.actionCount04);s.fogDescriptorStep=3;
        at(0x2dc2e4);at(0x2dc2e8);at(0x2dc2ec); //Actual signed0<1 branch, ordinary NOP delay.
        at(0x2dc2b8);if(!liveRow()||!row.actions68||!row.action||row.actionSlotsPublished!=1)return false;
        at(0x2dc2bc);if(row.trigger00!=&row.sourceInstance->entity_)return false;
        at(0x2dc2c0);at(0x2dc2c4);if(row.actions68[0]!=row.action.get())return false;
        h.actionRead=true;s.fogDescriptorStep=4;
        at(0x2dc2c8);h.actionTable108=row.action->vtable108;s.fogDescriptorStep=5;
        if(*h.actionTable108!=0x3cf0b0u)return false;
        at(0x2dc2cc);h.actionTarget0C=0x1b5ad0u;s.fogDescriptorStep=6;
        at(0x2dc2d0);at(0x2dc2d4); //Actual JALR and World->A1 delay; own return2DC2D8.
        if(!installSourceTriggerLostAi1B5AD0(row,pc)){sourcePostParticleModelsPC_=pc;return false;}
        sourcePostParticleModelsPC_=pc;if(pc!=0x2dc2d8u)return false;
        h.actionReturned=true;h.actionReturnV0=1;s.fogDescriptorStep=7;
        at(0x2dc2d8);*h.actionOffset+=4u;s.fogDescriptorStep=8;
        at(0x2dc2dc);++*h.actionIndex;s.fogDescriptorStep=9;
        at(0x2dc2e0);if(!liveRow()||row.actionCount04!=1)return false;
        h.actionCount=static_cast<std::uint32_t>(row.actionCount04);s.fogDescriptorStep=10; //Own fresh reload, not a reused loop test.
        at(0x2dc2e4);at(0x2dc2e8);at(0x2dc2ec); //1<1 is false; NOP delay still executes.
        at(0x2dc2f0);at(0x2dc2f4); //JAL2E8B00; delay reads this descriptor's source64.
        if(!liveRow()||!row.sourceFields)return false;auto& geometry=*row.sourceFields;
        if(!geometry.sourceGeometry64||!geometry.geometry64||*geometry.sourceGeometry64!=geometry.geometry64.get()||
           !geometry.geometryAllocationReturned||!geometry.geometryPublished||!geometry.geometryConstructed||
           geometry.geometrySourceFreeEntered||!preflightSourceWorldTriggerNative())return false;
        geometry.geometrySourceHistory=*geometry.geometry64;geometry.geometrySourceFreeEntered=true;s.fogDescriptorStep=11;
        at(0x2e8b00);if(!a::OriginalConfigProcessGlobals50LA::descriptorDelete2E8B00AdmittedNative())return false;
        geometry.geometry64.reset();geometry.geometrySourceFreed=true; //Only plain descriptor80B; action/instance/shape remain owned.
        at(0x2dc2f8);geometry.sourceGeometry64=nullptr;geometry.geometrySourceNullStored=true;
        h.geometrySourceFreed=true;s.fogDescriptorStep=12; //No fallible gap between native free and source-knownNULL publication.
        for(std::uint32_t p=0x2dc2fc;p<=0x2dc30c;p+=4)at(p);
        at(0x2dc310);h.descriptorReturnV0=1;s.fogDescriptorStep=13;
        at(0x2dc314);at(0x2dc318);h.descriptorReturned=true;s.fogDescriptorStep=14;
        at(0x2dcc18); //Own caller continuation;2DC31C padding is not executed.
        if(!liveRow()||!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative())return false;
        at(0x2dcc1c);at(0x2dcc20);at(0x2dcc24); //Original index offset,402 argument and slot address.
        at(0x2dcc28);if(list.descriptors34[*h.rowIndex]!=&row)return false;
        at(0x2dcc2c);if(row.trigger00!=&row.sourceInstance->entity_)return false;
        h.parentOperandsRead=true;s.fogDescriptorStep=15;
        at(0x2dcc30);at(0x2dcc34); //JAL141960 with actual list parent in delay.
        if(!registerSourceTrigger141960(row,pc)){sourcePostParticleModelsPC_=pc;return false;}
        sourcePostParticleModelsPC_=pc;if(pc!=0x2dcc38u)return false;
        h.registrationReturned=true;s.fogDescriptorStep=16; //Actual returned trigger identity, not scalar V0=1.
        at(0x2dcc38);++*s.currentRow;h.nextRow=*s.currentRow;h.parentAdvanced=true;s.fogDescriptorStep=17;
        at(0x2dcc3c);s.fogDescriptorStep=18;at(0x2dcc40);
        return false; //Descriptor0 complete; the next fresh count load and descriptor1 service remain unexecuted.
    };
    const auto finishFogMembership=[&]()noexcept {
        if(pc!=0x280bc0u||!sourceCompletedParticleWorldCustodyCurrentNative()||
           !preflightSourceTriggerServiceNative()||!sourceTriggerFogPortalGraphLiveNative(z))return false;
        const bool returned=finishSourceTriggerFogMembership280BC0(row,s.fogPrimary,pc);
        h.triggerCallbackReturned=returned;sourcePostParticleModelsPC_=pc;
        if(returned&&pc==0x2dc2acu)return finishFogDescriptor();
        return false; //Assertion/destructor refusal retains its own exact partial state.
    };
    const auto actualHead=[&](const void* registration,const void* actualCollider,const void* actualZone,
        const NativeListMethodCustody* actualMethod,const void*& head,std::shared_ptr<const void>& graphPin,
        std::uint32_t& portalPC)noexcept {
        portalPC=0x280c34u;
        //Only leaf checks here: this prefix's outer history copy is stale.
        if(head||graphPin||!sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||
           !h.callbackZone08||*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)!=z||
           !sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get()||!actualMethod||
           actualMethod!=node.owner->nativeList34||!nativeMethods4CC_||nativeMethods4CC_[z]!=actualMethod||
           !sourceListMethodArrayCurrent(1,z,actualMethod)||actualMethod->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
           actualMethod->application_!=worldApplication_.get()||actualMethod->ordinal_!=10u+z||
           actualMethod->usage10_!=1u||actualMethod->reference00_!=2u||!sourceListEntriesCurrent(*actualMethod))return false;
        for(const auto& old:sourceCollisionPortalBorrows_)if(old.registration==registration&&!old.token.expired())return false;
        try{std::shared_ptr<const void> token=std::make_shared<std::uint8_t>(0);
            sourceCollisionPortalBorrows_.push_back({registration,token});graphPin=std::move(token);
        }catch(...){return false;} //No head read is adopted after native allocation refusal.
        head=node.owner->portalLinks30.get();return true; //Actual source C34 load, under the live completed graph pin.
    };
    (void)continueSourceTriggerFogPortalHead280BF0(row,s.fogPrimary,actualHead,pc);
    sourcePostParticleModelsPC_=pc;
    if(pc==0x280bc0u)return finishFogMembership();
    if(pc!=0x280c40u)return false;
    if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
       !sourceTriggerFogPortalGraphLiveNative(z))return false;
    const auto actualPortal=[&](const void* registration,const void* actualCollider,const void* actualZone,
        const void* cursor,const std::shared_ptr<const void>& graphPin,auto& portal,auto& vertices,
        std::uint32_t& portalPC)noexcept {
        portalPC=0x280c40u;
        //Leaf ownership checks only while the outer history copy is stale.
        if(portal||vertices||!graphPin||!sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||
           !h.callbackZone08||*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)!=z||
           !sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get())return false;
        return sourceTriggerFogPortalInputsOwnerNative(row,cursor,portal,vertices);
    };
    (void)continueSourceTriggerFogPortalInputs280C40(row,s.fogPrimary,actualPortal,pc);
    sourcePostParticleModelsPC_=pc;
    if(pc!=0x2311a0u)return false;
    if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
       !sourceTriggerFogPortalGraphLiveNative(z))return false;
    const auto continueMissTail=[&]()noexcept {
        if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z))return false;
        const auto admitted=[&](const void* registration,const void* collider,const void* zone,
            const std::shared_ptr<const void>& pin)noexcept {
            return bool(pin)&&sourceTriggerPrimaryInputsNative(row,registration,collider)&&h.callbackZone08&&
                *h.callbackZone08>=0&&static_cast<std::uint32_t>(*h.callbackZone08)==z&&
                sourceTriggerFogPortalGraphLiveNative(z)&&zone==node.owner.get();
        };
        const auto portal=[&](const void* registration,const void* collider,const void* zone,const void* cursor,
            const std::shared_ptr<const void>& pin,auto& p,auto& vertices,std::uint32_t& reached)noexcept {
            reached=0x280c40u;
            return admitted(registration,collider,zone,pin)&&sourceTriggerFogTailPortalNative(row,cursor,p,vertices);
        };
        const auto next=[&](const void* registration,const void* collider,const void* zone,const void* cursor,
            const std::shared_ptr<const void>& pin,const void*& result,std::uint32_t& reached)noexcept {
            reached=0x280cf8u;
            return admitted(registration,collider,zone,pin)&&sourceTriggerFogTailNextNative(row,cursor,result);
        };
        const auto operand=[&](const void* registration,const void* collider,const void* zone,const void* cursor,
            const std::shared_ptr<const void>& pin,unsigned index,unsigned stage,auto& p,auto& vertices,auto& word,
            std::uint32_t& reached)noexcept {
            reached=stage==0u?0x2311a0u:stage==3u?0x231214u:stage==1u?0x23125cu:stage==8u?0x231278u:0x2312a4u;
            return admitted(registration,collider,zone,pin)&&sourceTriggerFogTailOperandNative(row,cursor,index,stage,p,vertices,word);
        };
        (void)continueSourceTriggerFogPortalMissTailNative(row,s.fogPrimary,portal,next,operand,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc==0x280bc0u)return finishFogMembership();
        return false; //A new hit remains pending280C5C; no old hit publication is borrowed.
    };
    const auto continuePortalResult=[&]()noexcept {
        if((pc!=0x280c5cu&&pc!=0x280cf8u)||!sourceCompletedParticleWorldCustodyCurrentNative()||
           !preflightSourceTriggerServiceNative()||!sourceTriggerFogPortalGraphLiveNative(z))return false;
        const auto actualStore=[&](const void* registration,const void* actualCollider,const void* actualZone,
            const void* head,const std::shared_ptr<const void>& graphPin,unsigned stage,std::uint32_t distance,
            std::uint32_t& portalPC)noexcept {
            portalPC=stage==1u?0x280c60u:0x280c64u;
            if(!graphPin||!sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||
               !h.callbackZone08||*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)!=z||
               !sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get())return false;
            return storeSourceTriggerFogPortalDistanceNative(row,head,stage,distance);
        };
        const auto actualNext=[&](const void* registration,const void* actualCollider,const void* actualZone,
            const void* head,const std::shared_ptr<const void>& graphPin,const void*& next,std::uint32_t& portalPC)noexcept {
            portalPC=0x280cf8u;
            if(next||!graphPin||!sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||
               !h.callbackZone08||*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)!=z||
               !sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get())return false;
            return readSourceTriggerFogPortalNextNative(row,head,next);
        };
        (void)continueSourceTriggerFogPortalAfterNative(row,s.fogPrimary,actualStore,actualNext,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc==0x280c40u)return continueMissTail();
        if(pc==0x280bc0u)return finishFogMembership();
        if(pc!=0x280c68u)return false; //First hit keeps its original publication path.
        if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z))return false;
        const auto actualNeighbor=[&](const void* registration,const void* actualCollider,const void* actualZone,
            const void* head,const std::shared_ptr<const void>& graphPin,unsigned stage,const void*& neighborNode,
            const g::OriginalWreckComponent50LA*& neighbor,NativeListMethodCustody*& method,std::uint32_t& portalPC)noexcept {
            portalPC=stage==1u?0x280c68u:0x27f5a0u;
            if((stage!=1u&&stage!=9u)||neighborNode||neighbor||method||!graphPin||
               !sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||!h.callbackZone08||*h.callbackZone08<0||
               static_cast<std::uint32_t>(*h.callbackZone08)!=z||!sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get())return false;
            return sourceTriggerFogPortalNeighborOwnerNative(row,head,stage==9u,neighborNode,neighbor,method);
        };
        (void)continueSourceTriggerFogPortalNeighbor280C68(row,s.fogPrimary,actualNeighbor,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc!=0x27e3c0u)return false;
        if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z))return false;
        (void)continueSourceTriggerFogPortalSelector27E3C0(row,s.fogPrimary,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc!=0x280ca4u)return false;
        if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z))return false;
        (void)continueSourceTriggerFogPortalInsertion280CA4(row,s.fogPrimary,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc!=0x280cc4u)return false;
        if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z))return false;
        (void)continueSourceTriggerFogPortalPool280CC4(row,s.fogPrimary,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc!=0x105740u&&pc!=0x10576cu)return false; //Cold growth remains pending1057D8.
        if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z))return false;
        (void)continueSourceTriggerFogPortalPoolMutationNative(row,s.fogPrimary,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc!=0x105804u)return false;
        if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z))return false;
        (void)continueSourceTriggerFogPortalPoolReturn105804(row,s.fogPrimary,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc!=0x280cd0u)return false;
        if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z))return false;
        (void)continueSourceTriggerFogPortalSlot14280CD0(row,s.fogPrimary,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc!=0x280cd4u)return false;
        if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z))return false;
        const auto actualPayloadPortal=[&](const void* registration,const void* actualCollider,const void* actualZone,
            const void* cursor,const std::shared_ptr<const void>& graphPin,const g::OriginalWreckComponent50LA*& portal,
            std::uint32_t& portalPC)noexcept {
            portalPC=0x280cdcu;
            if(portal||!graphPin||!sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||
               !h.callbackZone08||*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)!=z||
               !sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get())return false;
            const std::array<std::array<std::uint32_t,4>,4>* vertices=nullptr;
            return sourceTriggerFogPortalInputsOwnerNative(row,cursor,portal,vertices); //Fresh actual link08 read under the original graph owner.
        };
        (void)continueSourceTriggerFogPortalPayload280CD4(row,s.fogPrimary,actualPayloadPortal,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc!=0x280cf8u)return false;
        if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z))return false;
        const auto actualHitNext=[&](const void* registration,const void* actualCollider,const void* actualZone,
            const void* head,const std::shared_ptr<const void>& graphPin,const void*& next,std::uint32_t& portalPC)noexcept {
            portalPC=0x280cf8u;
            if(next||!graphPin||!sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||
               !h.callbackZone08||*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)!=z||
               !sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get())return false;
            return readSourceTriggerFogPortalHitNextNative(row,head,next);
        };
        const auto actualNextPortal=[&](const void* registration,const void* actualCollider,const void* actualZone,
            const void* cursor,const std::shared_ptr<const void>& graphPin,const g::OriginalWreckComponent50LA*& portal,
            std::uint32_t& portalPC)noexcept {
            portalPC=0x280c40u;
            if(portal||!graphPin||!sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||
               !h.callbackZone08||*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)!=z||
               !sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get())return false;
            return sourceTriggerFogNextPortalOwnerNative(row,cursor,portal);
        };
        (void)continueSourceTriggerFogPortalHitNext280CF8(row,s.fogPrimary,actualHitNext,actualNextPortal,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc==0x280bc0u)return finishFogMembership();
        if(pc!=0x280c44u)return false;
        if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z))return false;
        const auto actualNextInputs=[&](const void* registration,const void* actualCollider,const void* actualZone,
            const void* cursor,const std::shared_ptr<const void>& graphPin,const g::OriginalWreckComponent50LA* portal,
            const std::array<std::array<std::uint32_t,4>,4>*& vertices,std::uint32_t& portalPC)noexcept {
            portalPC=0x280c44u;
            if(vertices||!portal||!graphPin||!sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||
               !h.callbackZone08||*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)!=z||
               !sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get())return false;
            return sourceTriggerFogNextInputsOwnerNative(row,cursor,portal,vertices);
        };
        (void)continueSourceTriggerFogPortalNextInputs280C44(row,s.fogPrimary,actualNextInputs,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc!=0x23116cu)return false;
        if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
           !sourceTriggerFogPortalGraphLiveNative(z))return false;
        const auto actualQueryOperand=[&](const void* registration,const void* actualCollider,const void* actualZone,
            const void* cursor,const std::shared_ptr<const void>& graphPin,unsigned index,unsigned stage,
            const g::OriginalWreckComponent50LA*& portal,const std::array<std::array<std::uint32_t,4>,4>*& vertices,
            const std::array<std::uint32_t,4>*& operand,std::uint32_t& portalPC)noexcept {
            portalPC=stage==0u?0x2311a0u:stage==3u?0x231214u:stage==1u?0x23125cu:stage==8u?0x231278u:0x2312a4u;
            if(portal||vertices||operand||!graphPin||!sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||
               !h.callbackZone08||*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)!=z||
               !sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get())return false;
            return sourceTriggerFogNextQueryOperandNative(row,cursor,index,stage,portal,vertices,operand);
        };
        (void)continueSourceTriggerFogPortalNextQuery23116C(row,s.fogPrimary,actualQueryOperand,pc);
        sourcePostParticleModelsPC_=pc;
        if(pc==0x280cf8u&&s.fogPrimary.portalNextQuery.returned())return continueMissTail();
        return false; //A second hit or unsupported arithmetic still stops without borrowing first-hit publication.
    };
    const auto actualNormal=[&](const void* registration,const void* actualCollider,const void* actualZone,
        const void* cursor,const std::shared_ptr<const void>& graphPin,const std::array<std::uint32_t,4>*& normal,
        std::uint32_t& portalPC)noexcept {
        portalPC=0x2311a0u;
        if(normal||!graphPin||!sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||
           !h.callbackZone08||*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)!=z||
           !sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get())return false;
        const g::OriginalWreckComponent50LA* portal=nullptr;
        const std::array<std::array<std::uint32_t,4>,4>* vertices=nullptr;
        return sourceTriggerFogPortalInputsOwnerNative(row,cursor,portal,vertices,&normal);
    };
    (void)continueSourceTriggerFogPortalDot2311A0(row,s.fogPrimary,actualNormal,pc);
    sourcePostParticleModelsPC_=pc;
    if(pc!=0x23120cu)return continuePortalResult(); //Only an authentic helper miss can advance the link.
    if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
       !sourceTriggerFogPortalGraphLiveNative(z))return false;
    const auto actualEdge=[&](const void* registration,const void* actualCollider,const void* actualZone,
        const void* cursor,const std::shared_ptr<const void>& graphPin,const std::array<std::uint32_t,4>*& edge0,
        std::uint32_t& portalPC)noexcept {
        portalPC=0x231214u;
        if(edge0||!graphPin||!sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||
           !h.callbackZone08||*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)!=z||
           !sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get())return false;
        const g::OriginalWreckComponent50LA* portal=nullptr;
        const std::array<std::array<std::uint32_t,4>,4>* vertices=nullptr;
        return sourceTriggerFogPortalInputsOwnerNative(row,cursor,portal,vertices,nullptr,&edge0);
    };
    (void)continueSourceTriggerFogPortalEdge023120C(row,s.fogPrimary,actualEdge,pc);
    sourcePostParticleModelsPC_=pc;
    if(pc!=0x23125cu)return continuePortalResult();
    if(!sourceCompletedParticleWorldCustodyCurrentNative()||!preflightSourceTriggerServiceNative()||
       !sourceTriggerFogPortalGraphLiveNative(z))return false;
    const auto actualLoopOperand=[&](const void* registration,const void* actualCollider,const void* actualZone,
        const void* cursor,const std::shared_ptr<const void>& graphPin,unsigned index,unsigned stage,
        const g::OriginalWreckComponent50LA*& portal,const std::array<std::array<std::uint32_t,4>,4>*& vertices,
        const std::array<std::uint32_t,4>*& edge,std::uint32_t& portalPC)noexcept {
        portalPC=stage==1u?0x23125cu:(stage==8u?0x231278u:0x2312a4u);
        if(index<1u||index>3u||(stage!=1u&&stage!=8u&&stage!=19u)||portal||vertices||edge||!graphPin||
           !sourceTriggerPrimaryInputsNative(row,registration,actualCollider)||!h.callbackZone08||*h.callbackZone08<0||
           static_cast<std::uint32_t>(*h.callbackZone08)!=z||!sourceTriggerFogPortalGraphLiveNative(z)||actualZone!=node.owner.get())return false;
        //Address formation needs the authentic owner. Edge contents become required only at the actual LQC2.
        return sourceTriggerFogPortalInputsOwnerNative(row,cursor,portal,vertices,nullptr,stage>=8u?&edge:nullptr,index,stage>=19u);
    };
    (void)continueSourceTriggerFogPortalLoop23125C(row,s.fogPrimary,actualLoopOperand,pc);
    sourcePostParticleModelsPC_=pc;
    return continuePortalResult(); //Hit stops before neighbor setup; miss stops before a fresh portal occurrence or primary epilogue.
}
bool Owner::continueSourceTriggerService274E90(std::uint32_t& pc)noexcept {
    const bool fogCaller=sourcePostParticleCaller_.step!=0;
    auto& s=sourceTriggerService274E90_;
    if(fogCaller&&(sourcePostParticleCaller_.step!=5||!sourceCompletedParticleWorldCustodyCurrentNative()))return false;
    if(pc!=0x274e90||s.entered||sourcePostCollisionCaller_.target144!=0x274e90u||
       !preflightSourcePostParticleModelsNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x274e90);s.entered=true;
    if(fogCaller){at(0x274e94);at(0x274e98);s.fogPrefixStep=1;} //Save RA and copy this World into A1.
    at(0x274e9c);
    if(sourceTriggerPhase_!=SourceTriggerPhase::listReturned||!preflightSourceWorldTriggerNative()||
       !sourceTrigger3D0_||*sourceTrigger3D0_!=nativeTriggerList3D0_.get())return false;
    if(fogCaller){
        //The genuine returned loader is the positive two-descriptor cohort,
        //not constructor count0, a nullable failure, or native-retired storage.
        if(!sourceTriggerPublished_||!nativeTriggerList3D0_||!sourceTriggerCohortAttempted_||
           sourceTriggerRow_!=2||sourceTriggerRawOffset_!=8u||sourceTriggerListSuccessCount_!=2u||
           sourceTriggerListStartIndex_!=0u||sourceTriggerListLimit_!=2u||sourceTriggerListReturn_!=1u)return false;
        const auto& loaded=*nativeTriggerList3D0_;
        if(!loaded.sourceMode||!loaded.counted||!loaded.sourceEnrolled||!loaded.sourceVtableKnown||
           loaded.sourceVtable08!=0x3d5230u||!loaded.sourceCountKnown||loaded.count30!=2||
           !loaded.sourceArrayAllocationReturned||!loaded.sourceArrayKnown||!loaded.descriptors34||
           loaded.descriptorSlotsPublished!=2)return false;
        for(std::size_t i=0;i<2;++i){const auto& descriptor=loaded.descriptorOwners[i];
            if(loaded.sourceDescriptorNativeRetired[i]||!descriptor||loaded.descriptors34[i]!=descriptor.get()||
               !sourceTriggerLoadedDescriptorCurrentNative(*descriptor))return false;
        }
    }
    s.listRead=true;s.listNonNull=bool(*sourceTrigger3D0_);
    if(fogCaller){s.fogPrefixStep=2;at(0x274ea0);at(0x274ea4);}
    if(!s.listNonNull){at(0x274ea8);return false;} //No substituted NULL owner/return.
    auto& list=*nativeTriggerList3D0_;
    at(0x274eb0);if(!list.sourceVtableKnown)return false;s.listTable08=list.sourceVtable08;
    if(*s.listTable08!=0x3d5230u)return false;if(fogCaller)s.fogPrefixStep=3;
    at(0x274eb4);s.listTarget7C=0x2dcb30u;if(fogCaller)s.fogPrefixStep=4;
    at(0x274eb8);if(fogCaller)at(0x274ebc); //Actual JALR/NOP, own return274EC0.
    at(0x2dcb30);s.listEntered=true;if(fogCaller)s.fogPrefixStep=5;
    if(fogCaller){for(std::uint32_t p=0x2dcb34;p<=0x2dcb48;p+=4)at(p);
        s.fogPrefixStep=6;at(0x2dcb4c);} //S2=list, S1=World; branch to fresh count.
    at(0x2dcb50);s.currentRow=0;if(fogCaller)s.fogPrefixStep=7; //Branch delay initializes index0.
    for(;;){
        at(0x2dcc40);if(!list.sourceCountKnown)return false;
        if(fogCaller&&(s.currentRow!=0u||list.count30!=2))return false;
        s.freshCount30=list.count30;if(fogCaller)s.fogPrefixStep=8;
        at(0x2dcc44);at(0x2dcc48);at(0x2dcc4c);if(fogCaller)s.fogPrefixStep=9;
        if(static_cast<std::int32_t>(*s.currentRow)>=static_cast<std::int32_t>(*s.freshCount30)){
            //Actual list return ends at68;6C is padding, not its delay.
            for(std::uint32_t p=0x2dcc50;p<=0x2dcc5c;p+=4)at(p);
            at(0x2dcc60);s.listReturnV0=1;at(0x2dcc64);at(0x2dcc68);s.listReturned=true;
            //The World wrapper ends atECC. ED0 begins a different function.
            at(0x274ec0);s.wrapperReturnV0=1;at(0x274ec4);at(0x274ec8);at(0x274ecc);s.wrapperReturned=true;
            at(0x1aa2e0);return continueSourcePostTriggerCaller1AA2E0(pc);
        }
        at(0x2dcb54);const auto i=*s.currentRow;
        if(i>=s.rows.size())return false;
        auto& h=s.rows[i];if(h.rowIndex)return false;
        if(!fogCaller){h.rowIndex=i;h.count30=*s.freshCount30;}
        if(!list.sourceArrayKnown||!list.descriptors34||i>=list.descriptorSlotsPublished||i>=list.descriptorOwners.size()||
           list.descriptors34[i]!=list.descriptorOwners[i].get())return false;
        if(fogCaller){
            if(i!=0||list.sourceDescriptorNativeRetired[i]||!list.descriptorOwners[i]||
               !sourceTriggerLoadedDescriptorCurrentNative(*list.descriptorOwners[i]))return false;
            h.rowIndex=i;h.count30=*s.freshCount30;at(0x2dcb58);at(0x2dcb5c);s.fogPrefixStep=10;
        }
        at(0x2dcb60);h.rowRead=true;h.rowNonNull=bool(list.descriptorOwners[i]);
        if(fogCaller){s.fogPrefixStep=11;at(0x2dcb64);s.fogPrefixStep=12;
            at(0x2dcb6c);return continueSourceTriggerFogBody2DCB6C(pc);} //Nonnull BEQL annuls increment2DCB68.
        if(!h.rowNonNull){at(0x2dcb68);++*s.currentRow;h.nextRow=*s.currentRow;h.parentAdvanced=true;continue;}
        auto& row=*list.descriptorOwners[i];if(!row.sourceFields)return false;const auto& f=*row.sourceFields;
        at(0x2dcb6c);auto& shared=g::OriginalApplicationEntities50LA::processParticleAssertion27C820();
        if(!shared.known||shared.thread!=std::this_thread::get_id()||shared.triggerGuard3D686C>1||
           shared.triggerAssertion444618!=shared.triggerGuard3D686C)return false;
        h.guard3D686CRead=shared.triggerGuard3D686C;
        if(!*h.guard3D686CRead){at(0x2dcb80);shared.triggerAssertion444618=1;
            at(0x2dcb84);shared.triggerGuard3D686C=1;h.assertionLazyStored=true;}
        at(0x2dcb8c);h.assertion444618Read=shared.triggerAssertion444618;
        at(0x2dcba4);if(!worldZones34C_)return false;h.worldZones34CRead=*worldZones34C_;
        at(0x2dcbb8);if(!(f.known&64u))return false;h.zone08=f.zone08;
        at(0x2dcbc0);if(static_cast<std::int32_t>(*h.worldZones34CRead)<*h.zone08){at(0x13366c);return false;}
        at(0x1336a8);h.assertionReturned=true;at(0x2dcbf8);
        at(0x2dcc08);if(!(f.known&1u))return false;h.descriptorTable6C=f.vtable6C;
        if(*h.descriptorTable6C!=0x3d52c0u)return false;
        at(0x2dcc0c);h.descriptorTarget0C=0x2dc270u;at(0x2dcc10);at(0x2dc270);h.descriptorEntered=true;
        at(0x2dc288);h.callbackZone08=f.zone08;
        at(0x2dc28c);if(!sourceTopologyMode_||sourceTopologyWithdrawn_||!sourceCollision4C4_||
           !sourceCollisionSlots4C4_||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get())return false;
        h.collisionArrayRead=true;at(0x2dc298);at(0x2dc2a0);
        if(*h.callbackZone08<0||static_cast<std::uint32_t>(*h.callbackZone08)>=sourceCollisionPublished_||
           static_cast<std::uint32_t>(*h.callbackZone08)>=sourceSearchNodes_[1].size())return false;
        const auto z=static_cast<std::uint32_t>(*h.callbackZone08);const auto& node=sourceSearchNodes_[1][z];
        if(!node.owner||node.withdrawn||!node.published||!node.ordinalKnown||
           sourceCollisionSlots4C4_[z]!=node.owner.get()||!preflightSourceSearchNodeNative(node,z,true))return false;
        h.collisionNodeRead=true;at(0x2dc2a4);
        at(0x2dc2a8); //Actual delay-slot descriptor.trigger00 read, not a prepared observer.
        if(!row.trigger00||!row.sourceInstance||!row.sourceInstancePublished||
           !preflightSourceTriggerInstanceNative(row))return false;
        h.trigger00Read=true;
        const bool geometry=refreshSourceTriggerGeometry2A0A80(row,pc);
        sourcePostParticleModelsPC_=pc;if(!geometry)return false;
        h.geometryReturned=true;
        at(0x2a0b14);
        beginSourceTriggerMembership120070(row,pc);sourcePostParticleModelsPC_=pc;
        if(pc==0x120188u){
            const auto actualMethod=[&](const void* occurrence,const void* actualCollider,
                NativeListMethodCustody*& method,std::optional<std::uint32_t>& slot,
                bool& pending,std::uint32_t& methodPC,bool select)noexcept {
                methodPC=select?0x280afcu:0x27f5a0u;
                if(!sourceTriggerPrimaryInputsNative(row,occurrence,actualCollider)||!h.callbackZone08||*h.callbackZone08<0||
                   static_cast<std::uint32_t>(*h.callbackZone08)!=z||!preflightSourceSearchNodeNative(node,z,true)||
                   sourceCollisionSlots4C4_[z]!=node.owner.get())return false;
                //Fresh actual mode1 node34 / World4CC, not Model354 or mode2 node38.
                if(!select)method=node.owner->nativeList34;
                if(method!=node.owner->nativeList34||!method||!nativeMethods4CC_||
                   nativeMethods4CC_[z]!=method||!sourceListMethodArrayCurrent(1,z,method))return false;
                const auto& publication=sourceListMethods_[1][z];
                if(!publication.arrayPublished||!publication.aliasPublished||!publication.aliasRetained||
                   publication.withdrawn||publication.aliasNode!=node.owner.get()||!method->sourceMode_||
                   method->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
                   method->application_!=worldApplication_.get()||method->ordinal_!=10u+z||
                   method->usage10_!=1||method->reference00_!=2||!method->source_.constructorReturned||
                   method->source_.vtable08!=0x3d2d30u||!sourceListEntriesCurrent(*method))return false;
                if(!select)return true;
                return selectSourceObserverSlot27E3C0(*method,occurrence,actualCollider,slot,pending,methodPC,1u);
            };
            continueSourceTriggerPrimary2809C0(row,node.owner.get(),actualMethod,pc);
            sourcePostParticleModelsPC_=pc;
        }
        if(pc==0x280bf0u){
        const auto actualPortal=[&](unsigned phase,const void* registration,const void* actualCOL,
            const void* actualZone,const std::array<std::uint32_t,4>& center,const void* cursor,
            auto& edge,std::shared_ptr<const void>& graphPin,std::uint32_t& portalPC)noexcept {
            portalPC=phase==0?0x280c34u:phase==2?0x280c90u:phase==4?0x280cf8u:0x280c40u;
            if(phase>4||!sourceTriggerPrimaryInputsNative(row,registration,actualCOL)||
               !sourceTriggerPortalCenterNative(row,center)||
               actualZone!=node.owner.get()||!h.callbackZone08||*h.callbackZone08<0||
               static_cast<std::uint32_t>(*h.callbackZone08)!=z||!sourceTopologyMode_||sourceTopologyWithdrawn_||
               !sourceCollision4C4_||!sourceCollisionSlots4C4_||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||
               z>=sourceCollisionPublished_)return false;
            const auto zone=z;auto& root=sourceSearchNodes_[1][zone];
            if(root.withdrawn||!root.published||!root.owner||actualZone!=root.owner.get()||
               sourceCollisionSlots4C4_[zone]!=root.owner.get()||
               !preflightSourceSearchNodeNative(root,zone,true)||
               !sourcePortalWorldLinksCurrent(*root.owner,true,zone))return false;
            if(phase==0){
                if(graphPin||cursor)return false;
                for(const auto& old:sourceCollisionPortalBorrows_)
                    if(old.registration==registration&&!old.token.expired())return false;
                //Allocate native custody before exposing the source head pointer.
                //Weak root records never keep a completed COL borrower alive.
                try{std::shared_ptr<const void> token=std::make_shared<std::uint8_t>(0);
                    sourceCollisionPortalBorrows_.push_back({registration,token});graphPin=std::move(token);
                }catch(...){return false;}
                edge.next=root.owner->portalLinks30.get();return true;
            }
            bool pinned=false;
            for(const auto& borrow:sourceCollisionPortalBorrows_)if(borrow.registration==registration){
                const auto live=borrow.token.lock();if(live&&live==graphPin){pinned=true;break;}}
            if(!pinned||!graphPin||!cursor)return false;
            std::size_t index=0;while(index<40&&sourcePortalWorldLinks_[index/4][index%4].published!=cursor)++index;
            //Odd links are collision namespace (stages1/3), never visibility0/2.
            if(index==40||!(index&1u))return false;
            auto& link=sourcePortalWorldLinks_[index/4][index%4];
            if(link.withdrawn||link.knownMask!=31||!link.published||link.from!=root.owner.get()||!link.to)return false;
            auto& raw=*link.published;const auto& portal=nativePortals_[index/4];
            if(!portal||!portal->sourceMode||raw.neighbor04!=&link.to->entity||
               raw.portal08!=&portal->entity||!portal->source.loadReturned||
               portal->source.normalKnown!=7||!portal->source.normalNormalized||portal->source.copiedVertices!=4)return false;
            for(unsigned i=0;i<4;++i)if(portal->source.edgeKnown[i]!=7||!portal->source.edgeNormalized[i])return false;
            if(phase==1){
                edge.entered=true;edge.edge=cursor;edge.edgeIndex=index;
                edge.neighbor=raw.neighbor04;edge.portal=raw.portal08;
                //Actual231150 uses center and f12=+0, not half/radius80.
                portalPC=0x231150u;
                const bool overlapOkay=g::source_portal_overlap231150::run(portal->source.vertices30,portal->source.normal70,
                    portal->source.edges80,center,0u,edge.overlap);
                edge.overlapReturned=edge.overlap.returned;edge.hit=edge.overlap.hit;edge.distance=edge.overlap.outputDistance;
                if(!overlapOkay){portalPC=edge.overlap.pc;return false;}
                portalPC=0x280c54u;if(!edge.hit)return true;
                if(!edge.distance)return false;
                portalPC=0x280c60u;raw.field10=*edge.distance;link.distance10=edge.distance;edge.distanceStoreMask|=1u;
                portalPC=0x280c64u;raw.field0C=*edge.distance;link.distance0C=edge.distance;edge.distanceStoreMask|=2u;
            }else if(edge.edge!=cursor||edge.edgeIndex!=index||edge.neighbor!=raw.neighbor04||edge.portal!=raw.portal08||
                     !g::source_portal_overlap231150::historyCurrent(edge.overlap))return false;
            if(phase==4){edge.next=raw.next00.get();portalPC=0x280cf8u;return true;}
            if(phase==3&&!edge.hit)return true;
            //The source keeps a method cursor: each hit reads its usage10,
            //then resolves the neighbor method using that same mode1.
            portalPC=phase==1?0x280c6cu:portalPC;
            const auto* prior=edge.previousMethod;
            if(!prior||!prior->sourceMode_||prior->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
               prior->application_!=worldApplication_.get()||prior->usage10_!=1||prior->ordinal_<10||prior->ordinal_>=20||
               !sourceListMethodArrayCurrent(1,prior->ordinal_-10,prior))return false;
            std::size_t neighborZone=0;while(neighborZone<10&&sourceCollisionSlots4C4_[neighborZone]!=link.to)++neighborZone;
            if(neighborZone==10||neighborZone>=sourceCollisionPublished_)return false;
            auto& neighbor=sourceSearchNodes_[1][neighborZone];
            if(neighbor.withdrawn||!neighbor.published||neighbor.owner.get()!=link.to||
               !preflightSourceSearchNodeNative(neighbor,neighborZone,true))return false;
            if(phase==1){portalPC=0x27f5a0u;edge.method=link.to->nativeList34;edge.methodReturned=true;}
            auto* method=edge.method;
            if(!method||method!=link.to->nativeList34||!nativeMethods4CC_||nativeMethods4CC_[neighborZone]!=method||
               !sourceListMethodArrayCurrent(1,neighborZone,method)||method->usage10_!=1||method->reference00_!=2||
               !method->sourceMode_||method->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
               method->application_!=worldApplication_.get()||method->ordinal_!=10u+neighborZone||
               !method->source_.constructorReturned||method->source_.vtable08!=0x3d2d30u||
               !sourceListEntriesCurrent(*method))return false;
            const auto& publication=sourceListMethods_[1][neighborZone];
            if(!publication.arrayPublished||!publication.aliasPublished||!publication.aliasRetained||
               publication.withdrawn||publication.aliasNode!=link.to)return false;
            if(phase==2){portalPC=0x280c90u;
                if(!edge.overlap.returned||!edge.overlap.hit||edge.distanceStoreMask!=3u||!edge.methodReturned)return false;
                return selectSourceObserverSlot27E3C0(*method,&edge,actualCOL,
                    edge.slot,edge.selectionPending,portalPC,1u);}
            return phase==1||phase==3;
        };
            continueSourceTriggerPortals280BF0(row,actualPortal,pc);sourcePostParticleModelsPC_=pc;
        }
        if(pc==0x280d38u){
            h.triggerCallbackReturned=finishSourceTriggerMembership120190(row,pc);sourcePostParticleModelsPC_=pc;
        }
        if(pc==0x2dc2acu&&h.triggerCallbackReturned){
            h.actionLoopEntered=true;at(0x2dc2ac);h.actionIndex=0;at(0x2dc2b4);h.actionOffset=0;
            at(0x2dc2e0);h.actionCount=static_cast<std::uint32_t>(row.actionCount04);
            at(0x2dc2e4);if(static_cast<std::int32_t>(*h.actionIndex)<row.actionCount04){
                at(0x2dc2b8);if(!row.actions68||!row.action||row.actionSlotsPublished!=1||*h.actionIndex!=0||*h.actionOffset!=0)return false;
                at(0x2dc2bc);if(!row.trigger00||!row.sourceInstancePublished)return false;
                at(0x2dc2c4);if(row.actions68[0]!=row.action.get())return false;h.actionRead=true;
                at(0x2dc2c8);h.actionTable108=row.action->vtable108;
                if(*h.actionTable108!=0x3cf0b0u)return false;
                at(0x2dc2cc);h.actionTarget0C=0x1b5ad0u;at(0x2dc2d0);
                if(!installSourceTriggerLostAi1B5AD0(row,pc)){sourcePostParticleModelsPC_=pc;return false;}
                h.actionReturned=true;h.actionReturnV0=1;
                at(0x2dc2d8);*h.actionOffset+=4u;at(0x2dc2dc);++*h.actionIndex;
                at(0x2dc2e0);h.actionCount=static_cast<std::uint32_t>(row.actionCount04);
                at(0x2dc2e4);if(static_cast<std::int32_t>(*h.actionIndex)<row.actionCount04){at(0x2dc2b8);return false;}
            }
            at(0x2dc2f0);
            auto& geometry=*row.sourceFields;at(0x2dc2f4);
            if(!geometry.sourceGeometry64||!geometry.geometry64||*geometry.sourceGeometry64!=geometry.geometry64.get()||
               !geometry.geometryPublished||!geometry.geometryConstructed||geometry.geometrySourceFreeEntered||
               !preflightSourceWorldTriggerNative())return false;
            //Descriptor plain80B owner only. Instance/shape copied values and
            //installed action payload are distinct live allocations.
            geometry.geometrySourceHistory=*geometry.geometry64;geometry.geometrySourceFreeEntered=true;
            at(0x2e8b00);
            if(!a::OriginalConfigProcessGlobals50LA::descriptorDelete2E8B00AdmittedNative())return false;
            //Native allocator adaptation of original free, not guest heap emulation.
            geometry.geometry64.reset();geometry.geometrySourceFreed=true;
            at(0x2dc2f8);geometry.sourceGeometry64=nullptr;geometry.geometrySourceNullStored=true;
            h.geometrySourceFreed=true;at(0x2dc310);h.descriptorReturnV0=1;
            at(0x2dc31c);h.descriptorReturned=true;at(0x2dcc18);
            //Fresh caller34 slot/trigger00 loads after descriptor return.
            at(0x2dcc18);if(!list.sourceArrayKnown||!list.descriptors34||!h.rowIndex||
               *h.rowIndex>=list.descriptorSlotsPublished||list.descriptors34[*h.rowIndex]!=&row)return false;
            at(0x2dcc28);at(0x2dcc2c);if(!row.trigger00||!row.sourceInstancePublished)return false;
            h.parentOperandsRead=true;at(0x2dcc30);
            if(!registerSourceTrigger141960(row,pc)){sourcePostParticleModelsPC_=pc;return false;}
            h.registrationReturned=true;at(0x2dcc38);
            //Next descriptor requires a fresh occurrence, not overwriting this row's receipts.

        }
        if(h.registrationReturned){
            at(0x2dcc38);++*s.currentRow;h.nextRow=*s.currentRow;h.parentAdvanced=true;
            continue; //Fresh count/slot and independent next descriptor occurrence.
        }
        return false;

    }
}
bool Owner::continueSourcePostTriggerCaller1AA2E0(std::uint32_t& pc)noexcept {
    auto& h=sourcePostTriggerCaller_;
    if(pc!=0x1aa2e0u||h.entered||nativeScbThread_!=std::this_thread::get_id()||
       nativeScbRevoked_||closed_||terminal_||!preflightSourcePostParticleModelsNative()||
       !sourceTriggerService274E90_.wrapperReturned)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x1aa2e0);h.entered=true;at(0x1aa2e4);
    //Original initialized382B80 points at static Application4334A0. The
    //native source-load binding represents that SAME process owner, not a
    //guest-global dereference or a claim about arbitrary unrepresented writes.
    if(!sourceLoadApplication_||!sourceLoadRace_||!worldApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||
       sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_)return false;
    h.applicationRead=true;at(0x1aa2e8);
    const auto& app=*sourceLoadApplication_;
    if(!app.riderProfile||!app.profile||sourceLoadRace_->profile_!=app.riderProfile)return false;
    h.profileRead=true;at(0x1aa2ec);at(0x1aa2f0);at(0x1aa2f4);
    std::uint8_t value=0;
    if(!app.riderProfile->readWorldProfileByte1AA2F4(*app.profile,value))return false;
    h.profile1125C=value;at(0x1aa2f8);at(0x1aa2fc);h.mode=(value>>5u)&3u;
    at(0x1aa300);at(0x1aa304);at(0x1aa308);
    const bool lessThanTwo=*h.mode<2u;
    at(lessThanTwo?0x1aa328u:0x1aa30cu);
    if(sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||worldEntity_.reference00!=1||
       sourcePostCollisionCaller_.worldTable08!=0x3cf110u)return false;
    h.worldRead=true;at(lessThanTwo?0x1aa32cu:0x1aa310u);h.worldTable08=0x3cf110u;
    at(lessThanTwo?0x1aa330u:0x1aa314u);h.target13C=0x274ae0u;
    at(lessThanTwo?0x1aa334u:0x1aa318u);
    at(lessThanTwo?0x1aa338u:0x1aa31cu);h.argumentA1=lessThanTwo?1u:0u;
    at(0x274ae0);h.dispatched=true;
    at(0x274af4);return continueSourceSkySetup274AF4(pc);
}
bool Owner::continueSourceSkySetup274AF4(std::uint32_t& pc)noexcept {
    auto& h=sourceSkySetup274AE0_;
    if(pc!=0x274af4u||h.entered||!sourcePostTriggerCaller_.dispatched||
       !preflightSourcePostParticleModelsNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x274af4);h.entered=true;
    if(!sourceEnvironmentPublished_||!nativeEnvironment3A4_||
       *nativeEnvironment3A4_!=sourceEnvironment3A4_.get())return false;
    const bool present=bool(sourceEnvironment3A4_);
    if(present){if(sourceEnvironmentPhase_!=SourceEnvironmentPhase::returnedOne||!sourceWorldConfig_||
       !sourceEnvironment3A4_->preflightSourceNative())return false;}
    else if(sourceEnvironmentPhase_!=SourceEnvironmentPhase::returnedZero)return false;
    h.environmentRead=true;h.environmentNonNull=present;
    at(0x274af8);h.argumentS1=sourcePostTriggerCaller_.argumentA1;
    at(0x274afc);at(0x274b00);
    if(!present){at(0x274b04);at(0x274b08);h.returnV0=1;}
    else{
        at(0x274b0c);std::int32_t count=0;
        if(!sourceEnvironment3A4_->readSourceSkyCount274B0C(worldEntity_,*sourceWorldConfig_,count))return false;
        h.count00=count;at(0x274b10);at(0x274b14);h.arrayBytes=static_cast<std::uint32_t>(count)<<2u;
        if(count!=0){at(0x274b24);return continueSourceSkyArray274B24(pc);}
        at(0x274b18);at(0x274b1c);h.returnV0=1;
    }
    //Both actual no-sky branches reach the same real epilogue. No heap/3F8 store occurs.
    at(0x274cb8);h.restoreMask|=1;at(0x274cbc);h.restoreMask|=2;
    at(0x274cc0);h.restoreMask|=4;at(0x274cc4);h.restoreMask|=8;
    at(0x274cc8);at(0x274ccc);h.stackReleased=true;h.returned=true;
    h.callerResumePC=*h.argumentS1?0x1aa33cu:0x1aa320u;
    at(*h.callerResumePC);
    if(!*h.argumentS1){at(0x1aa324);h.callerSkipExecuted=true;}
    at(0x1aa33c);return false; //Next fresh profile/course read is still unjoined.
}
bool Owner::preflightSourceSkyArrayNative()const noexcept {
    const auto& h=sourceSkyArray274B24_;const auto& sky=sourceSkySetup274AE0_;
    if((h.entered&&(!sky.count00||*sky.count00==0||!sky.arrayBytes))||
       (h.allocationAttempted&&(!h.entered||h.allocatedCount!=1u))||
       (h.allocatedCount&&(!h.entered||!sky.count00||*h.allocatedCount!=static_cast<std::uint32_t>(*sky.count00)))||
       (h.allocationReturned&&!h.allocationAttempted)||(h.allocationNonNull&&!h.allocationReturned)||
       (h.published&&!h.allocationReturned)||(h.indexS0&&(!h.published||*h.indexS0>1u))||
       (h.freshCount&&!h.indexS0)||(h.fileIndex&&(!h.freshCount||*h.fileIndex!=0u))||
       (h.fileRead&&!h.fileIndex)||(h.fileLength&&(!h.fileRead||*h.fileLength>=h.fileName.size()))||
       (h.equalsNone&&!h.fileLength)||(h.noneSkip&&(h.equalsNone!=true||h.indexS0!=1u)))return false;
    if(h.fileLength){
        if(h.fileName[*h.fileLength]!=0)return false;
        for(std::size_t i=0;i<*h.fileLength;++i)if(h.fileName[i]==0)return false;
        if(h.equalsNone&&*h.equalsNone!=(*h.fileLength==4&&std::memcmp(h.fileName.data(),"none",4)==0))return false;
    }
    const auto& c=sourceSkyConstructor288A50_;
    if((c.allocationAttempted&&(!h.fileRead||h.equalsNone!=false))||
       (c.allocationReturned&&!c.allocationAttempted)||(c.allocationNonNull&&!c.allocationReturned)||
       (c.entered&&!c.allocationNonNull)||(c.countAcquired&&(!c.entered||!c.countBefore))||
       (c.countBefore&&!c.entered)||(c.countRetiredNative&&(!c.countAcquired||!h.nativeWithdrawn))||
       (c.query30&&(!c.countAcquired||*c.query30!=0))||(c.type32&&!c.query30)||
       (c.knownFlags33!=0&&c.knownFlags33!=1&&c.knownFlags33!=3&&c.knownFlags33!=7)||
       (c.flags33&~c.knownFlags33)||(c.knownFlags33&&!c.type32)||
       (c.searchableReturned&&(!c.type32||c.knownFlags33!=7))||
       (c.tableZeroed&&!c.searchableReturned)||(c.triplesWritten&&(!c.tableZeroed||c.triplesWritten>96))||
       (c.baseReturned&&(!c.tableZeroed||c.triplesWritten!=96))||
       (c.returned&&(!c.baseReturned||c.vtable08!=(sourceSkyError129960_.baseDestructorEntered?0x3cce60u:0x3d2ff0u)))||
       (c.slotPublished&&(!c.allocationReturned||(c.allocationNonNull&&!c.returned))))return false;
    bool missingNull=false;
    for(const auto& lane:c.nullFields){if(!lane)missingNull=true;
        else if(missingNull||!c.searchableReturned||*lane!=0)return false;}
    if(c.tableZeroed&&missingNull)return false;
    const auto& load=sourceSkyLoad288A90_;
    if((load.entered&&!c.slotPublished)||(load.policy3E0990Stored&&!load.entered)||
       (load.argumentA1&&(!load.policy3E0990Stored||load.argumentA1!=sky.argumentS1))||
       (load.policy36C0F0Stored&&!load.argumentA1)||
       (load.argumentF12&&(!load.policy36C0F0Stored||*load.argumentF12!=0x44800000u))||
       (load.environmentRead&&!load.argumentF12)||(load.fileRead&&!load.environmentRead)||
       (load.fileLength&&(!load.fileRead||*load.fileLength>=load.fileName.size()))||
       (load.receiverRead&&!load.fileRead)||(load.receiverNonNull&&!load.receiverRead)||
       (load.wrapperEntered&&!load.receiverRead)||(load.directoryRead&&!load.wrapperEntered)||
       (load.formatted&&(!load.directoryRead||!load.pathLength))||
       (load.pathLength&&(!load.formatted||*load.pathLength>=load.path30.size()))||
       (load.loaderEntered&&!load.formatted)||(load.noOp141C20Returned&&!load.loaderEntered))return false;
    if(load.fileLength){if(load.fileName[*load.fileLength]!=0)return false;
        for(std::size_t i=0;i<*load.fileLength;++i)if(!load.fileName[i])return false;}
    if(load.pathLength){
        if(!load.fileLength||*load.pathLength!=6u+*load.fileLength||
           std::memcmp(load.path30.data(),"skies\\",6)!=0||
           std::memcmp(load.path30.data()+6,load.fileName.data(),*load.fileLength+1)!=0)return false;
    }
    const auto& file=sourceSkyFileCaller1295E0_;
    if((file.entered&&!load.noOp141C20Returned)||(file.resolverRead&&!file.entered)||
       (file.openAttempted&&!file.resolverRead)||(file.openReturned&&!file.openAttempted)||
       (file.handleS3.has_value()!=file.openReturned)||
       (file.readAttempted&&(!file.handleS3||*file.handleS3<0))||
       (file.readReturned&&!file.readAttempted)||(file.firstReadItems&&(!file.readReturned||*file.firstReadItems>1)))return false;
    if(sourceSkyFile_&&!sourceSkyFile_->preflightNative())return false;
    if(!h.nativeWithdrawn){
        if(file.openAttempted&&!sourceSkyFile_)return false;
        if(sourceSkyFile_&&(sourceSkyFile_->nativeClosed_||sourceSkyFile_->receiver_!=sourceSkyOwner_.get()))return false;
        if(sourceSkyFile_&&sourceSkyFile_->attempted_){
            if(!sourceTrackMountDomain_||!load.pathLength||
               sourceSkyFile_->resolver_!=sourceTrackMountDomain_->resolver3E1A70()||
               sourceSkyFile_->provider_!=sourceTrackMountDomain_->archiveProvider3E1A70()||
               sourceSkyFile_->provider_!=sourceTrackMountLease_.provider_||sourceSkyFile_->root_!=sourceTrackFile_||
               sourceSkyFile_->name_.size()!=*load.pathLength||
               std::memcmp(sourceSkyFile_->name_.data(),load.path30.data(),*load.pathLength)!=0)return false;
        }
        if(file.openReturned&&(!sourceSkyFile_||sourceSkyFile_->sourceHandle_!=file.handleS3))return false;
        if(file.firstReadItems&&(!sourceSkyFile_||sourceSkyFile_->read_.items!=file.firstReadItems))return false;
        if(file.resolverRead&&(!sourceTrackMountDomain_||sourceTrackMountLease_.closed_||!sourceTrackFile_||
           !sourceTrackMountDomain_->prepared()||!sourceLoadApplication_||
           sourceLoadApplication_->standardMaterialDomainOwner_!=sourceTrackMountDomain_))return false;
    }else if(sourceSkyFile_&&!sourceSkyFile_->nativeClosed_)return false;
    const auto& header=sourceSkyHeader129644_;
    if((header.entered&&file.firstReadItems!=1u)||(header.resolverRead&&!header.entered)||
       (header.countReadAttempted&&!header.resolverRead)||(header.countReadReturned&&!header.countReadAttempted)||
       (header.countReadItems.has_value()!=header.countReadReturned)||header.receiverBytesWritten>4||
       (header.count7D8.has_value()!=header.countReadAttempted)||
       (header.indexS2&&(!header.countReadReturned||header.countReadItems!=1u||*header.indexS2!=0))||
       (header.paletteCount.has_value()!=header.indexS2.has_value())||
       (header.loopEntered&&(!header.paletteCount||*header.paletteCount==0||*header.paletteCount>=0x80000000u))||
       (header.offsetReadAttempted&&!header.loopEntered)||(header.offsetReadReturned&&!header.offsetReadAttempted)||
       (header.tellReturned&&(!header.paletteOffset||!header.resourceStart))||
       (header.savedS4&&(!header.tellReturned||*header.savedS4!=(*header.resourceStart<=0x7fffffffu?
           static_cast<std::int64_t>(*header.resourceStart):static_cast<std::int64_t>(*header.resourceStart)-0x100000000ll)))||
       (header.seekReturned&&(!header.tellReturned||!header.savedS4))||
       (header.field40Cleared&&(!header.paletteCount||(*header.paletteCount>0&&*header.paletteCount<0x80000000u))))return false;
    const auto headerWord=[](const std::array<std::uint8_t,4>& bytes){return std::uint32_t(bytes[0])|
        (std::uint32_t(bytes[1])<<8)|(std::uint32_t(bytes[2])<<16)|(std::uint32_t(bytes[3])<<24);};
    if(header.paletteCount&&(!sourceSkyFile_||sourceSkyFile_->read_.items!=1u||
       *header.paletteCount!=headerWord(sourceSkyFile_->read_.bytes)))return false;
    if(header.paletteOffset&&(!header.offsetReadReturned||!sourceSkyFile_||sourceSkyFile_->paletteOffsetRead_.items!=1u||
       *header.paletteOffset!=headerWord(sourceSkyFile_->paletteOffsetRead_.bytes)))return false;
    if(header.resourceStart.has_value()!=header.tellReturned)return false;
    if(header.countReadAttempted){
        if(!sourceSkyFile_||sourceSkyFile_->countRead_.copied!=header.receiverBytesWritten||
           sourceSkyFile_->countRead_.returned!=header.countReadReturned||sourceSkyFile_->countRead_.items!=header.countReadItems)return false;
        std::uint32_t value=0;for(std::size_t i=0;i<header.receiverBytesWritten;++i)
            value|=std::uint32_t(sourceSkyFile_->countRead_.bytes[i])<<static_cast<unsigned>(i*8);
        if(header.count7D8!=value||(!h.nativeWithdrawn&&(!sourceSkyOwner_||sourceSkyOwner_->count7D8!=value)))return false;
    }
    if(header.offsetReadAttempted&&(!sourceSkyFile_||sourceSkyFile_->paletteOffsetRead_.attempted!=true||
       sourceSkyFile_->paletteOffsetRead_.returned!=header.offsetReadReturned))return false;
    if(header.tellReturned&&(!sourceSkyFile_||sourceSkyFile_->resourceStart_!=header.resourceStart))return false;
    if(header.seekReturned&&(!sourceSkyFile_||!sourceSkyFile_->seekReturned_||
       sourceSkyFile_->resourceSeekTarget_!=header.paletteOffset))return false;
    if(!preflightSourceSkyPaletteNative()||!preflightSourceSkyItemsNative())return false;
    if(h.nativeWithdrawn)return h.entered&&!sourceSkySlots3F8_&&!sourceSkyPublished3F8_&&!sourceSkyOwner_&&
        (!c.countAcquired||c.countRetiredNative);
    if((h.entered&&nativeSky3F8_)||c.countRetiredNative||c.allocationNonNull!=bool(sourceSkyOwner_))return false;
    if(c.slotPublished&&(!sourceSkySlots3F8_||sourceSkySlots3F8_[0]!=sourceSkyOwner_.get()))return false;
    if(c.countAcquired){
        if(!worldApplication_||!sourceSkyOwner_||worldApplication_->attachmentLock423F58())return false;
        const auto& e=sourceSkyOwner_->entity;
        if(e.reference00!=(sourceSkyError129960_.referenceStored?0u:1u)||e.sessionGeneration!=worldEntity_.sessionGeneration||e.objectKey!=key(&e)||
           e.parent1C||e.previous10||e.next14||e.firstChild18||e.mask20||e.descendants24||
           (sourceSkyError129960_.authorityRetired?contains(worldApplication_->entityAuthority_.view(),&e):!worldApplication_->entityAuthority_.canRetire(e))||
           worldApplication_->objectCount423F60()<=*c.countBefore)return false;
    }
    const auto& error=sourceSkyError129960_;
    if((error.lockRead&&!error.entered)||(error.unlinkedLeafRead&&!error.lockRead)||
       (error.authorityRetired&&!error.unlinkedLeafRead)||(error.referenceStored!=error.authorityRetired)||
       (error.derivedDestructorEntered&&!error.referenceStored)||(error.baseDestructorEntered&&!error.derivedDestructorEntered))return false;
    if(error.entered){if(error.failedItem>sourceSkyLaterItems_.size())return false;
        const auto& failed=sourceItemHistory(error.failedItem);
        if(!failed.failureNullReturned||!failed.slotStored)return false;
        if(!h.nativeWithdrawn&&c.vtable08!=(error.baseDestructorEntered?0x3cce60u:0x3d2ff0u))return false;}
    if(load.policy3E0990Stored&&(!worldApplication_||worldApplication_->skyPolicy3E0990_!=1u))return false;
    if(load.policy36C0F0Stored&&(!worldApplication_||worldApplication_->skyPolicy36C0F0_!=load.argumentA1))return false;
    if(load.receiverRead&&load.receiverNonNull!=bool(sourceSkyOwner_))return false;
    if(load.fileRead){const char* name=nullptr;std::size_t bytes=0;
        if(!sourceEnvironment3A4_||!sourceWorldConfig_||!h.fileIndex||!load.fileLength||
           !sourceEnvironment3A4_->readSourceSkyFile274B54(worldEntity_,*sourceWorldConfig_,*h.fileIndex,name,bytes)||
           bytes!=*load.fileLength||std::memcmp(name,load.fileName.data(),bytes+1)!=0)return false;}
    if(c.tableZeroed){
        if(!worldApplication_||!worldApplication_->skyTable3E09A0_)return false;
        const auto& table=*worldApplication_->skyTable3E09A0_;
        for(std::size_t i=0;i<table.size();++i)
            if(table[i]!=std::array<std::uint32_t,3>{{i<c.triplesWritten?0x3f800000u:0u,0u,0u}})return false;
    }
    if(h.allocationReturned&&h.allocationNonNull!=bool(sourceSkySlots3F8_))return false;
    if(h.published!=bool(sourceSkyPublished3F8_))return false;
    if(sourceSkyPublished3F8_&&*sourceSkyPublished3F8_!=sourceSkySlots3F8_.get())return false;
    if(!h.entered)return !sourceSkySlots3F8_&&!sourceSkyPublished3F8_;
    if(!sourceEnvironment3A4_||!sourceWorldConfig_||!nativeEnvironment3A4_||
       *nativeEnvironment3A4_!=sourceEnvironment3A4_.get())return false;
    if(h.freshCount){std::int32_t current=0;
        if(!sourceEnvironment3A4_->readSourceSkyCount274B0C(worldEntity_,*sourceWorldConfig_,current)||
           current!=*h.freshCount)return false;}
    if(h.fileRead){const char* name=nullptr;std::size_t bytes=0;
        if(!h.fileLength||!sourceEnvironment3A4_->readSourceSkyFile274B54(worldEntity_,*sourceWorldConfig_,
           *h.fileIndex,name,bytes)||bytes!=*h.fileLength||std::memcmp(name,h.fileName.data(),bytes+1)!=0)return false;}
    return true;
}
bool Owner::withdrawSourceSkyArrayNative()noexcept {
    auto& h=sourceSkyArray274B24_;
    if(!preflightSourceSkyArrayNative())return false;
    if(!h.entered||h.nativeWithdrawn)return true;
    //Partial palette owns its own backing and file destination borrow, not a sky
    //Entity count. Retire it before numeric file/sky receiver custody.
    if(!withdrawSourceSkyItemsNative())return false;
    if(!withdrawSourceSkyPaletteNative())return false;
    //Fresh numeric handle/manager pin leaves before receiver count or array backing.
    if(sourceSkyFile_){a::OriginalStandardTextureDiagnostic50LA d;
        if(!sourceSkyFile_->closeNative(d))return false;} //Retain closed scalar search/read history.
    //Withdraw published aliases first. Unknown slots are never inspected.
    sourceSkyPublished3F8_.reset();sourceSkySlots3F8_.reset();
    auto& c=sourceSkyConstructor288A50_;
    if(c.countAcquired){
        if(!sourceSkyError129960_.authorityRetired){
            if(!worldApplication_->entityAuthority_.retire(sourceSkyOwner_->entity))std::terminate();
            --sourceSkyOwner_->entity.reference00;
        }
        //Source142590 already released the reference on this error prefix,
        //but the unreached base destructor has not retired the object count.
        --worldApplication_->objectCount423F60();c.countRetiredNative=true;
    }
    sourceSkyOwner_.reset();h.nativeWithdrawn=true;
    //Process table writes persist; never restore an unproved prior value.
    return true; //Native cancellation only, no World/source destructor or NULL-store receipt.
}
bool Owner::continueSourceSkyArray274B24(std::uint32_t& pc)noexcept {
    auto& h=sourceSkyArray274B24_;const auto& sky=sourceSkySetup274AE0_;
    if(pc!=0x274b24u||h.entered||!preflightSourcePostParticleModelsNative()||
       !sky.count00||*sky.count00!=1||sky.arrayBytes!=4u||nativeSky3F8_)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x274b24);h.entered=true;h.allocatedCount=static_cast<std::uint32_t>(*sky.count00);
    at(0x274b28);at(0x2e8ca0);h.allocationAttempted=true;
    //Native allocator adapter to nullable source allocation; preserve NULL publication.
    //Host pointer width is not a claim that the guest count*4 allocation changed.
    try{sourceSkySlots3F8_.reset(new SourceSkyOwner*[*h.allocatedCount]);}
    catch(...){sourceSkySlots3F8_.reset();}
    h.allocationReturned=true;h.allocationNonNull=bool(sourceSkySlots3F8_);
    at(0x274b2c);sourceSkyPublished3F8_=sourceSkySlots3F8_.get();h.published=true;
    at(0x274b30);at(0x274b34);h.indexS0=0;
    for(;;){
        at(0x274ca0);
        if(!nativeEnvironment3A4_||*nativeEnvironment3A4_!=sourceEnvironment3A4_.get()||
           !sourceEnvironment3A4_||!sourceWorldConfig_)return false;
        at(0x274ca4);std::int32_t count=0;
        if(!sourceEnvironment3A4_->readSourceSkyCount274B0C(worldEntity_,*sourceWorldConfig_,count))return false;
        h.freshCount=count;at(0x274ca8);at(0x274cac);at(0x274cb0);
        if(static_cast<std::int32_t>(*h.indexS0)>=count){at(0x274cb4);return false;} //Common return join not yet claimed.
        at(0x274b38);if(h.fileRead||*h.indexS0!=0)return false;
        at(0x274b3c);at(0x274b40);at(0x274b44);h.fileIndex=*h.indexS0;
        at(0x274b48);at(0x274b4c);at(0x274b50);at(0x274b54);
        const char* name=nullptr;std::size_t bytes=0;
        if(!sourceEnvironment3A4_->readSourceSkyFile274B54(worldEntity_,*sourceWorldConfig_,*h.fileIndex,name,bytes))return false;
        std::memcpy(h.fileName.data(),name,bytes+1);h.fileLength=bytes;h.fileRead=true;
        at(0x3090e8);h.equalsNone=(bytes==4&&std::memcmp(name,"none",4)==0);
        at(0x274b58);
        if(*h.equalsNone){at(0x274b5c);++*h.indexS0;h.noneSkip=true;at(0x274c9c);continue;}
        at(0x274b60);return continueSourceSkyConstructor274B60(pc);
    }
}
bool Owner::continueSourceSkyConstructor274B60(std::uint32_t& pc)noexcept {
    auto& c=sourceSkyConstructor288A50_;
    if(pc!=0x274b60u||c.allocationAttempted||!preflightSourcePostParticleModelsNative()||
       !sourceLoadApplication_||sourceLoadApplication_->entities()!=worldApplication_||!worldApplication_||
       nativeScbThread_!=std::this_thread::get_id()||worldApplication_->attachmentLock423F58())return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x274b60);at(0x274b64);at(0x2e8d90);c.allocationAttempted=true;
    //Nullable native allocator adaptation of source0xAF0, not a prepared owner.
    try{sourceSkyOwner_.reset(new SourceSkyOwner);}catch(...){sourceSkyOwner_.reset();}
    c.allocationReturned=true;c.allocationNonNull=bool(sourceSkyOwner_);
    at(0x274b68);at(0x274b6c);
    if(sourceSkyOwner_){
        at(0x274b70);at(0x274b74);at(0x274b78);at(0x288a50);c.entered=true;
        at(0x288a5c);at(0x288a60);at(0x129280);at(0x12928c);at(0x129290);at(0x27e130);
        at(0x27e13c);at(0x27e140);at(0x141300);
        auto& e=sourceSkyOwner_->entity;auto& authority=worldApplication_->entityAuthority_;
        auto& count=worldApplication_->objectCount423F60();
        if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
           !authority.canEnroll(e,key(&e)))return false;
        c.countBefore=count;
        if(g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::entity141300,
           worldEntity_.sessionGeneration,key(&e),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
           !authority.enroll(e))std::terminate();
        at(0x1413c0);c.countAcquired=true;c.vtable08=0x3cd240u;at(0x1413d4);
        at(0x27e154);c.query30=0;at(0x27e168);c.type32=0xff;
        at(0x27e188);c.knownFlags33=1;at(0x27e19c);c.knownFlags33=3;
        at(0x27e1b0);c.knownFlags33=7;at(0x27e1b8);c.vtable08=0x3ccee0u;
        at(0x27e1bc);const auto type=worldApplication_->types383408_.searchableGameObject27E1BC();
        if(!type.okay()){at(type.sourcePC);return false;}
        at(0x27e1f8);c.type32=type.type;at(0x27e218);c.flags33=1;
        at(0x27e22c);at(0x27e240);at(0x27e250);c.searchableReturned=true;
        at(0x1292ac);c.vtable08=0x3cce60u;at(0x1292d4);c.flags33=1;
        at(0x1292e8);c.flags33=3;
        constexpr std::uint32_t stores[]={0x1292f0,0x1292f8,0x129300,0x129308,0x129310,0x129318,0x129320,0x129328};
        for(std::size_t i=0;i<8;++i){at(stores[i]);c.nullFields[i]=0;}
        at(0x129330);at(0x129334);c.nullFields[8]=0;at(0x306478);
        //SAME process lane as other represented sky callers. New source writes,
        //not proof about all possible guest aliases or a renderer reset.
        worldApplication_->skyTable3E09A0_.emplace();
        for(auto& triple:*worldApplication_->skyTable3E09A0_)triple={{0u,0u,0u}};
        c.tableZeroed=true;
        for(std::size_t i=0;i<96;i+=2){
            at(0x129348);at(0x12934c);(*worldApplication_->skyTable3E09A0_)[i][0]=0x3f800000u;
            ++c.triplesWritten;at(0x129350);(*worldApplication_->skyTable3E09A0_)[i+1][0]=0x3f800000u;
            ++c.triplesWritten;at(0x129354);at(0x129358);at(0x12935c);at(0x129360);
        }
        at(0x129370);c.baseReturned=true;at(0x288a6c);c.vtable08=0x3d2ff0u;
        at(0x288a80);c.returned=true;at(0x274b7c);
    }
    at(0x274b80);at(0x274b84);at(0x274b88);at(0x274b8c);at(0x274b90);at(0x274b94);at(0x274b98);
    at(0x274b9c);
    if(!sourceSkyPublished3F8_||!*sourceSkyPublished3F8_||
       *sourceSkyPublished3F8_!=sourceSkySlots3F8_.get())return false; //ActualNULL array store cannot be performed.
    sourceSkySlots3F8_[0]=sourceSkyOwner_.get();c.slotPublished=true;
    at(0x12a100);return continueSourceSkyLoad12A100(pc);
}
bool Owner::continueSourceSkyLoad12A100(std::uint32_t& pc)noexcept {
    auto& h=sourceSkyLoad288A90_;const auto& array=sourceSkyArray274B24_;
    if(pc!=0x12a100u||h.entered||!preflightSourcePostParticleModelsNative()||
       nativeScbThread_!=std::this_thread::get_id()||!sourceLoadApplication_||!worldApplication_||
       sourceLoadApplication_->entities()!=worldApplication_||!sourceSkyConstructor288A50_.slotPublished)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x12a100);h.entered=true;at(0x12a104);at(0x12a108);
    worldApplication_->skyPolicy3E0990_=1u;h.policy3E0990Stored=true;
    at(0x274ba0);at(0x274ba4);at(0x274ba8);at(0x274bac);h.argumentA1=sourceSkySetup274AE0_.argumentS1;
    at(0x12a110);at(0x12a114);at(0x12a118);
    worldApplication_->skyPolicy36C0F0_=*h.argumentA1;h.policy36C0F0Stored=true;
    at(0x274bb0);at(0x274bb4);h.argumentF12=0x44800000u;
    at(0x274bb8);
    if(!sourceSkyPublished3F8_||!*sourceSkyPublished3F8_||
       *sourceSkyPublished3F8_!=sourceSkySlots3F8_.get())return false;
    at(0x274bbc);
    if(!nativeEnvironment3A4_||*nativeEnvironment3A4_!=sourceEnvironment3A4_.get()||
       !sourceEnvironment3A4_||!sourceWorldConfig_)return false;
    h.environmentRead=true;at(0x274bc0);at(0x274bc4);at(0x274bc8);at(0x274bcc);at(0x274bd0);
    const char* name=nullptr;std::size_t bytes=0;
    if(!array.indexS0||!sourceEnvironment3A4_->readSourceSkyFile274B54(worldEntity_,*sourceWorldConfig_,
       *array.indexS0,name,bytes))return false;
    std::memcpy(h.fileName.data(),name,bytes+1);h.fileLength=bytes;h.fileRead=true;
    at(0x274bd4);at(0x274bd8);
    if(sourceSkySlots3F8_[0]!=sourceSkyOwner_.get())return false;
    h.receiverRead=true;h.receiverNonNull=bool(sourceSkySlots3F8_[0]);
    at(0x288a90);h.wrapperEntered=true;
    //NULL receiver is not dereferenced by this wrapper prefix or141C20.
    //Preserve it rather than inventing an early loader failure/destructor.
    at(0x288a9c);at(0x288aa0);at(0x288aa4);at(0x288aa8);at(0x288aac);at(0x288ab0);
    at(0x288ab4);at(0x288ab8);at(0x288abc);at(0x288ac0);
    const auto& directory=sourceLoadApplication_->skyDirectory36B750();
    if(directory!="skies\\"||directory.size()+bytes+1>h.path30.size())return false;
    h.directoryRead=true;at(0x308c90);
    std::memcpy(h.path30.data(),directory.data(),directory.size());
    std::memcpy(h.path30.data()+directory.size(),name,bytes+1);
    h.pathLength=directory.size()+bytes;h.formatted=true;
    at(0x288ac4);at(0x288ac8);at(0x288acc);at(0x1295b0);h.loaderEntered=true;
    at(0x1295d0);at(0x1295d4);at(0x1295d8);at(0x141c20);at(0x141c24);
    h.noOp141C20Returned=true; //Identity only: no reference or count acquisition.
    at(0x1295dc);at(0x1295e0);return continueSourceSkyFile1295E0(pc);
}
bool Owner::continueSourceSkyFile1295E0(std::uint32_t& pc)noexcept {
    auto& h=sourceSkyFileCaller1295E0_;const auto& load=sourceSkyLoad288A90_;
    if(pc!=0x1295e0u||h.entered||!preflightSourcePostParticleModelsNative()||
       !sourceTrackMountDomain_||sourceTrackMountLease_.closed_||!sourceTrackMountDomain_->prepared()||
       !sourceTrackFile_||!sourceLoadApplication_||
       sourceLoadApplication_->standardMaterialDomainOwner_!=sourceTrackMountDomain_)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x1295e0);h.entered=true;
    const auto resolver=sourceTrackMountDomain_->resolver3E1A70();
    const auto provider=sourceTrackMountDomain_->archiveProvider3E1A70();
    if(!resolver||!provider||provider!=sourceTrackMountLease_.provider_)return false;
    h.resolverRead=true;at(0x1295e4);at(0x1295e8); //Actual mode388E60 is rb.
    try{sourceSkyFile_.reset(new a::OriginalSourceSkyFile50LA(sourceSkyOwner_.get()));}
    catch(...){return false;} //Native custody failure, not a source open return.
    at(0x1295ec);at(0x1295f0);h.openAttempted=true;
    a::OriginalStandardTextureDiagnostic50LA d;
    std::string name;
    try{name.assign(load.path30.data(),*load.pathLength);}catch(...){return false;}
    if(!sourceSkyFile_->begin131230(resolver,provider,sourceTrackFile_,name,d)){
        at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    h.handleS3=sourceSkyFile_->sourceHandle_;h.openReturned=true;at(0x1295f4);at(0x1295f8);at(0x1295fc);at(0x129600);
    if(*h.handleS3==-1){at(0x129604);return false;} //Real error receiver deref/destruction not joined.
    at(0x12961c);at(0x129620);
    if(sourceTrackMountDomain_->resolver3E1A70()!=resolver)return false;
    at(0x129624);at(0x129628);at(0x12962c);at(0x129630);at(0x129634);h.readAttempted=true;
    if(!sourceSkyFile_->readFirst4(d)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    h.readReturned=true;h.firstReadItems=sourceSkyFile_->read_.items;
    at(0x129638);at(0x12963c);at(0x129640);
    if(*h.firstReadItems!=1){at(0x129960);return false;} //Short-read error branch, no invented success.
    at(0x129644);return continueSourceSkyHeader129644(pc);
}
bool Owner::continueSourceSkyHeader129644(std::uint32_t& pc)noexcept {
    auto& h=sourceSkyHeader129644_;
    if(pc!=0x129644u||h.entered||!preflightSourcePostParticleModelsNative()||
       !sourceSkyFile_||!sourceSkyFile_->current()||sourceSkyFile_->read_.items!=1u)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    const auto resolverCurrent=[&](){return sourceTrackMountDomain_&&
        sourceTrackMountDomain_->resolver3E1A70()==sourceSkyFile_->resolver_;};
    const auto word=[](const std::array<std::uint8_t,4>& b){return std::uint32_t(b[0])|
        (std::uint32_t(b[1])<<8)|(std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);};
    h.entered=true;at(0x129644);at(0x129648);if(!resolverCurrent())return false;h.resolverRead=true;
    at(0x12964c);at(0x129650);at(0x129654);at(0x129658);
    //The first header went to stack. This second read targets actual receiver7D8;
    //a NULL receiver cannot supply native storage and is bounded at the real call.
    if(!sourceSkyOwner_){at(0x129654);return false;}
    h.countReadAttempted=true;a::OriginalStandardTextureDiagnostic50LA d;
    const bool returned=sourceSkyFile_->readCount4(d);
    const auto& count=sourceSkyFile_->countRead_;
    for(std::size_t i=0;i<count.copied;++i){const auto shift=static_cast<unsigned>(i*8);
        sourceSkyOwner_->count7D8=(sourceSkyOwner_->count7D8&~(0xffu<<shift))|(std::uint32_t(count.bytes[i])<<shift);}
    h.receiverBytesWritten=count.copied;h.count7D8=sourceSkyOwner_->count7D8;
    if(!returned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    h.countReadReturned=true;h.countReadItems=count.items;at(0x12965c);at(0x129660);at(0x129664);
    if(*h.countReadItems!=1){at(0x129960);return false;}
    at(0x129668);at(0x12966c);h.indexS2=0;at(0x129740);h.paletteCount=word(sourceSkyFile_->read_.bytes);
    at(0x129744);at(0x129748);at(0x12974c);
    //Signed SLT against stack7C: this is a palette count, not a validated format version.
    if(*h.paletteCount==0||*h.paletteCount>=0x80000000u){
        at(0x129750);h.field40Cleared=true;at(0x129754);
        at(0x129758);return continueSourceSkyItems129758(pc);
    }
    h.loopEntered=true;at(0x129670);at(0x129674);if(!resolverCurrent())return false;
    at(0x129678);at(0x12967c);at(0x129680);at(0x129684);at(0x129688);h.offsetReadAttempted=true;
    if(!sourceSkyFile_->readPaletteOffset4(d)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    h.offsetReadReturned=true;at(0x12968c);at(0x129690);at(0x129694);
    if(sourceSkyFile_->paletteOffsetRead_.items!=1u){at(0x129960);return false;}
    h.paletteOffset=word(sourceSkyFile_->paletteOffsetRead_.bytes);
    at(0x129698);at(0x12969c);if(!resolverCurrent())return false;at(0x1296a0);at(0x1296a4);
    if(!sourceSkyFile_->tellResourceStart(d)){at(d.sourcePC?d.sourcePC:0x12f520u);return false;}
    h.tellReturned=true;h.resourceStart=sourceSkyFile_->resourceStart_;at(0x1296a8);at(0x1296ac);at(0x1296b0);
    if(!resolverCurrent())return false;at(0x1296b4);
    h.savedS4=*h.resourceStart<=0x7fffffffu?static_cast<std::int64_t>(*h.resourceStart):
        static_cast<std::int64_t>(*h.resourceStart)-0x100000000ll;
    at(0x1296b8);at(0x1296bc);at(0x1296c0);at(0x1296c4);
    if(!sourceSkyFile_->seekResourceAbsolute(*h.paletteOffset,d)){at(d.sourcePC?d.sourcePC:0x12f640u);return false;}
    h.seekReturned=true;at(0x1296c8);at(0x1296cc);at(0x1296d0);
    //Fresh constructor7D0 is known NULL; no prior resource destructor is invented.
    if(sourceSkyConstructor288A50_.nullFields[7]!=0u)return false;
    at(0x1296d4);at(0x1296d8);at(0x1296f4);return continueSourceSkyPalette1296F4(pc);
}
bool Owner::preflightSourceSkyPaletteNative()const noexcept {
    const auto& h=sourceSkyPalette2E38C0_;
    if((h.entered&&!sourceSkyHeader129644_.seekReturned)||(h.allocationAttempted&&!h.entered)||
       (h.allocationReturned&&!h.allocationAttempted)||(h.allocationNonNull&&!h.allocationReturned)||
       (h.constructorEntered&&!h.allocationNonNull)||(h.baseReturned&&(!h.constructorEntered||h.reference00!=1u))||
       (h.derivedVtableStored&&(!h.baseReturned||h.vtable08!=0x3d5650u))||
       (h.constructorReturned&&!h.constructorEntered)||
       (h.slotPublished&&(!h.allocationReturned||(h.allocationNonNull&&!h.constructorReturned)))||
       (h.nativeReferenceWithdrawn&&(!h.baseReturned||!h.nativeWithdrawn||h.sourceReferenceDecremented))||
       (h.nativeWithdrawn&&!h.entered))return false;
    for(std::size_t i=0;i<3;++i){
        if((h.readAttempted[i]&&(!h.derivedVtableStored||(i&&!h.readReturned[i-1])))||
           (h.readReturned[i]&&!h.readAttempted[i]))return false;
        if(!h.readAttempted[i])continue;
        if(!sourceSkyFile_)return false;const auto& row=sourceSkyFile_->paletteHeaderReads_[i];
        if(!row.attempted||row.returned!=h.readReturned[i])return false;
        if(i<2){if(h.fieldBytes[i]!=row.copied)return false;
            if(!h.nativeWithdrawn&&!h.sourceBackingFreed&&(!sourceSkyPaletteOwner_||!sourceSkyPaletteOwner_->storage||
               std::memcmp(sourceSkyPaletteOwner_->storage.get()+0x10+i*4,row.bytes.data(),row.copied)!=0))return false;}
    }
    if(h.compressedFlag){
        if(!h.readReturned[2]||!sourceSkyFile_||sourceSkyFile_->paletteHeaderReads_[2].items!=1u)return false;
        const auto& b=sourceSkyFile_->paletteHeaderReads_[2].bytes;
        if(*h.compressedFlag!=(std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|(std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24)))return false;
    }
    constexpr std::size_t payloadSizes[3]={0x300,0x8000,0x10000};
    if((h.rawEntered&&h.compressedFlag!=0u)||(h.rawReturned&&!h.rawEntered)||
       (h.count14Read&&!h.rawReturned)||(h.count14.has_value()!=h.count14Read)||
       h.convertedColors>256||(h.convertedColors&&h.count14!=256u))return false;
    for(std::size_t i=0;i<3;++i){
        if(h.payloadBytes[i]>payloadSizes[i]||
           (h.payloadAttempted[i]&&(!h.rawEntered||(i&&!h.payloadReturned[i-1])))||
           (h.payloadReturned[i]&&!h.payloadAttempted[i]))return false;
        if(!h.payloadAttempted[i]){if(h.payloadBytes[i])return false;continue;}
        if(!sourceSkyFile_)return false;const auto& row=sourceSkyFile_->rawPaletteReads_[i];
        if(!row.attempted||h.payloadReturned[i]!=row.returned||h.payloadBytes[i]!=row.copied||
           (h.rawReturned&&row.items!=1u))return false;
    }
    if(h.rawReturned&&(!h.payloadReturned[2]||h.payloadBytes[2]!=0x10000u))return false;
    if(h.count14Read){
        if(!sourceSkyFile_||sourceSkyFile_->paletteHeaderReads_[1].items!=1u)return false;
        const auto& bytes=sourceSkyFile_->paletteHeaderReads_[1].bytes;
        if(*h.count14!=(std::uint32_t(bytes[0])|(std::uint32_t(bytes[1])<<8)|
            (std::uint32_t(bytes[2])<<16)|(std::uint32_t(bytes[3])<<24)))return false;
    }
    if(!h.nativeWithdrawn&&!h.sourceBackingFreed&&h.convertedColors){
        if(!sourceSkyPaletteOwner_||!sourceSkyPaletteOwner_->storage)return false;
        const auto* b=sourceSkyPaletteOwner_->storage.get();
        for(std::uint32_t i=0;i<h.convertedColors;++i){const auto r=b[0x18+i*3],g=b[0x19+i*3],blue=b[0x1a+i*3];
            const auto value555=((std::uint32_t(r)>>3)<<10)|((std::uint32_t(g)>>3)<<5)|(std::uint32_t(blue)>>3);
            const auto value565=((std::uint32_t(r)>>3)<<11)|((std::uint32_t(g)>>2)<<5)|(std::uint32_t(blue)>>3);
            if(b[0x318+i*2]!=(value555&255u)||b[0x319+i*2]!=(value555>>8)||
               b[0x518+i*2]!=(value565&255u)||b[0x519+i*2]!=(value565>>8))return false;
        }
    }
    const auto& ret=sourceSkyPaletteReturn129710_;
    if((ret.entered&&!h.slotPublished)||(ret.restoreAttempted&&!ret.entered)||
       (ret.restoreReturned&&(!ret.restoreAttempted||!sourceSkyFile_||!sourceSkyFile_->restoreReturned_))||
       (ret.indexS2&&(!ret.restoreReturned||*ret.indexS2!=1))||
       (ret.nextEntered&&(!ret.indexS2||!sourceSkyHeader129644_.paletteCount||*sourceSkyHeader129644_.paletteCount<=1u))||
       (ret.nextOffset&&(!ret.nextEntered||!sourceSkyFile_||sourceSkyFile_->followingPalettes_.empty()))||
       (ret.nextSavedS4&&!ret.nextOffset)||
       (ret.field40Cleared&&(!ret.indexS2||sourceSkyHeader129644_.paletteCount!=1u)))return false;
    if(ret.nextOffset){const auto& step=*sourceSkyFile_->followingPalettes_[0];const auto& bytes=step.offsetRead.bytes;
        if((step.seekAttempted&&step.target!=ret.nextOffset)||step.offsetRead.items!=1u||*ret.nextOffset!=(std::uint32_t(bytes[0])|(std::uint32_t(bytes[1])<<8)|
           (std::uint32_t(bytes[2])<<16)|(std::uint32_t(bytes[3])<<24)))return false;
        if(ret.nextSavedS4&&(!step.tellReturned||!step.start||*ret.nextSavedS4!=(*step.start<=0x7fffffffu?
           static_cast<std::int64_t>(*step.start):static_cast<std::int64_t>(*step.start)-0x100000000ll)))return false;
    }
    if((h.sourceReleaseEntered&&(!ret.nextEntered||!h.slotPublished))||
       (h.sourceReferenceDecremented&&(!h.sourceReleaseEntered||h.reference00!=1u))||
       (h.sourceAssociationScanReturned&&(!h.sourceReferenceDecremented||!sourceSkyFile_||!sourceSkyFile_->paletteScanReturned_))||
       (h.sourceBaseDestructed&&!h.sourceAssociationScanReturned)||
       (h.sourceFreeAdmitted&&!h.sourceBaseDestructed)||
       (h.sourceBackingFreed&&(!h.sourceFreeAdmitted||!sourceSkyFile_||!sourceSkyFile_->paletteReleasedSource_))||
       (h.sourceReleaseReturned&&!h.sourceBackingFreed))return false;
    const auto& next=sourceSkyNextPaletteHistory_;
    if(!preflightSourceSkyLaterPalettesNative())return false;
    if(bool(sourceSkyPalette7D0_)!=bool(sourceSkyPublishedPaletteOccurrence_))return false;
    if(sourceSkyPublishedPaletteOccurrence_&&(*sourceSkyPublishedPaletteOccurrence_>=sourceSkyLaterPalettes_.size()+2||
       *sourceSkyPalette7D0_!=sourcePaletteOwner(*sourceSkyPublishedPaletteOccurrence_).get()))return false;
    if(h.nativeWithdrawn)return (!h.baseReturned||h.sourceReferenceDecremented||h.nativeReferenceWithdrawn)&&!sourceSkyPaletteOwner_&&!sourceSkyPalette7D0_&&
        (!h.constructorEntered||(sourceSkyFile_&&(sourceSkyFile_->paletteWithdrawn_||sourceSkyFile_->paletteReleasedSource_)))&&
        !sourceSkyNextPaletteOwner_&&(!next.entered||next.nativeWithdrawn)&&(!next.baseReturned||next.sourceReferenceDecremented||next.nativeReferenceWithdrawn);
    if(h.allocationNonNull!=bool(sourceSkyPaletteOwner_)||h.slotPublished!=bool(sourceSkyPalette7D0_))return false;
    
    if(h.constructorEntered&&(!sourceSkyFile_||!sourceSkyFile_->paletteEntered_||sourceSkyFile_->paletteWithdrawn_||
       (h.sourceBackingFreed?!sourceSkyFile_->paletteReleasedSource_:
        sourceSkyFile_->paletteReceiver_!=sourceSkyPaletteOwner_->storage.get())))return false;
    if(h.baseReturned&&!h.sourceBackingFreed){
        if(!sourceSkyPaletteOwner_||!sourceSkyPaletteOwner_->storage)return false;
        const auto* b=sourceSkyPaletteOwner_->storage.get();
        if(b[0]!=(h.sourceReferenceDecremented?0:1))return false;for(std::size_t i=1;i<8;++i)if(b[i])return false;
        if(!h.vtable08)return false;const auto table=h.sourceBaseDestructed?0x3cc980u:*h.vtable08;
        for(std::size_t i=0;i<4;++i)if(b[8+i]!=((table>>(i*8))&0xffu))return false;
    }
    return true;
}
bool Owner::withdrawSourceSkyPaletteNative()noexcept {
    auto& first=sourceSkyPalette2E38C0_;if(!preflightSourceSkyPaletteNative())return false;
    if(!first.entered||first.nativeWithdrawn)return true;
    //Preflight all association refusals before any alias or owner mutation.
    if(sourceSkyFile_&&!sourceSkyFile_->withdrawPaletteNative())return false;
    sourceSkyPalette7D0_.reset();sourceSkyPublishedPaletteOccurrence_.reset();
    for(std::size_t n=sourceSkyLaterPalettes_.size()+2;n>0;--n){const auto i=n-1;
        auto& h=sourcePaletteHistory(i);auto& owner=sourcePaletteOwner(i);
        if(h.baseReturned&&!h.sourceReferenceDecremented){for(std::size_t k=0;k<8;++k)owner->storage[k]=0;h.nativeReferenceWithdrawn=true;}
        owner.reset();if(h.entered)h.nativeWithdrawn=true;
    }
    return true; //Native inverse; source destructor/ref/deallocation receipts remain untouched.
}
bool Owner::continueSourceSkyPalette1296F4(std::uint32_t& pc)noexcept {
    auto& h=sourceSkyPalette2E38C0_;
    if(pc!=0x1296f4u||h.entered||!preflightSourcePostParticleModelsNative()||!sourceSkyFile_||
       !sourceSkyFile_->current()||!sourceSkyOwner_||sourceSkyConstructor288A50_.nullFields[7]!=0u)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    h.entered=true;at(0x1296f4);at(0x1296f8);h.allocationAttempted=true;
    //Native bookkeeping allocation is separate from the nullable source0x18B18 backing.
    try{sourceSkyPaletteOwner_.reset(new SourceSkyPaletteOwner);}catch(...){return false;}
    try{sourceSkyPaletteOwner_->storage.reset(new std::uint8_t[0x18b18]);}
    catch(...){sourceSkyPaletteOwner_.reset();}
    h.allocationReturned=true;h.allocationNonNull=bool(sourceSkyPaletteOwner_);at(0x1296fc);at(0x129700);
    if(!h.allocationNonNull){at(0x129710);return continueSourceSkyPaletteReturn129710(pc);}
    at(0x129704);at(0x129708);at(0x12970c);
    if(!sourceSkyFile_->beginPalette2E38C0(sourceSkyPaletteOwner_->storage.get()))return false;
    if(!initializeSourceSkyPaletteBase2E38C0(*sourceSkyPaletteOwner_,h,pc))return false;
    return continueSourceSkyPaletteHeaders2E38FC(pc,0);
}
bool Owner::continueSourceSkyPaletteHeaders2E38FC(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(occurrence>=sourceSkyLaterPalettes_.size()+2||pc!=0x2e38fcu||!preflightSourcePostParticleModelsNative()||!sourceSkyFile_||!sourceSkyFile_->current())return false;
    auto& h=sourcePaletteHistory(occurrence);
    auto& owner=sourcePaletteOwner(occurrence);
    if(!owner||!owner->storage||!h.derivedVtableStored||h.readAttempted[0])return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    auto* b=owner->storage.get();
    constexpr std::uint32_t loads[3]={0x2e3900,0x2e3924,0x2e3948};
    constexpr std::uint32_t calls[3]={0x2e3908,0x2e3930,0x2e3954};
    constexpr std::uint32_t returns[3]={0x2e3910,0x2e3938,0x2e395c};
    for(std::size_t i=0;i<3;++i){
        at(loads[i]);if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
        at(calls[i]);at(calls[i]+4);h.readAttempted[i]=true;a::OriginalStandardTextureDiagnostic50LA d;
        const bool returned=sourceSkyFile_->readPaletteHeader4(i,d,occurrence);const auto& row=sourceSkyFile_->paletteHeader(occurrence,i);
        if(i<2){if(row.copied)std::memcpy(b+0x10+i*4,row.bytes.data(),row.copied);h.fieldBytes[i]=row.copied;}
        if(!returned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
        h.readReturned[i]=true;at(returns[i]);at(returns[i]+4);at(returns[i]+8);
        if(row.items!=1u){at(0x2e3d34);return continueSourceSkyPaletteReturnForOccurrence(pc,occurrence);}
    }
    const auto& flag=sourceSkyFile_->paletteHeader(occurrence,2).bytes;
    at(0x2e3968);h.compressedFlag=std::uint32_t(flag[0])|(std::uint32_t(flag[1])<<8)|
        (std::uint32_t(flag[2])<<16)|(std::uint32_t(flag[3])<<24);
    at(0x2e396c);at(0x2e3970);at(*h.compressedFlag?0x2e3974u:0x2e3b68u);
    if(*h.compressedFlag)return false;return continueSourceSkyRawPalette2E3B68(pc,occurrence);
}
bool Owner::continueSourceSkyRawPalette2E3B68(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(occurrence>=sourceSkyLaterPalettes_.size()+2)return false;
    auto& h=sourcePaletteHistory(occurrence);
    auto& owner=sourcePaletteOwner(occurrence);
    if(pc!=0x2e3b68u||h.rawEntered||h.compressedFlag!=0u||!preflightSourcePostParticleModelsNative()||
       !sourceSkyFile_||!sourceSkyFile_->current()||!owner||!owner->storage)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    h.rawEntered=true;
    constexpr std::uint32_t loads[3]={0x2e3b70,0x2e3b98,0x2e3bbc};
    constexpr std::uint32_t calls[3]={0x2e3b7c,0x2e3ba4,0x2e3bd0};
    constexpr std::uint32_t resumes[3]={0x2e3b84,0x2e3bac,0x2e3bd8};
    for(std::size_t i=0;i<3;++i){
        at(loads[i]);if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
        at(calls[i]);at(calls[i]+4);h.payloadAttempted[i]=true;a::OriginalStandardTextureDiagnostic50LA d;
        const bool returned=sourceSkyFile_->readRawPaletteBlock(i,d,occurrence);const auto& row=sourceSkyFile_->palettePayload(occurrence,i);
        h.payloadBytes[i]=row.copied;
        if(!returned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
        h.payloadReturned[i]=true;at(resumes[i]);at(resumes[i]+4);at(resumes[i]+8);
        if(row.items!=1u){at(0x2e3d34);return continueSourceSkyPaletteReturnForOccurrence(pc,occurrence);}
    }
    h.rawReturned=true;at(0x2e3be4);at(0x2e3be8);at(0x2e3bec);
    auto* b=owner->storage.get();
    h.count14=std::uint32_t(b[0x14])|(std::uint32_t(b[0x15])<<8)|(std::uint32_t(b[0x16])<<16)|(std::uint32_t(b[0x17])<<24);
    h.count14Read=true;at(0x2e3bf0);at(0x2e3bf4);
    if(*h.count14!=256u){at(0x2e3bf8);return false;} //Authored insertion/index/wrap effects must be joined separately.
    at(0x2e3ca0);at(0x2e3ca4);
    for(std::uint32_t i=0;i<256;++i){
        at(0x2e3ca8);const auto r=std::uint32_t(b[0x18+i*3]),g=std::uint32_t(b[0x19+i*3]),blue=std::uint32_t(b[0x1a+i*3]);
        const auto value555=((r>>3)<<10)|((g>>3)<<5)|(blue>>3);
        at(0x2e3ce4);b[0x318+i*2]=static_cast<std::uint8_t>(value555);b[0x319+i*2]=static_cast<std::uint8_t>(value555>>8);
        const auto value565=((r>>3)<<11)|((g>>2)<<5)|(blue>>3);
        at(0x2e3d24);at(0x2e3d28);b[0x518+i*2]=static_cast<std::uint8_t>(value565);b[0x519+i*2]=static_cast<std::uint8_t>(value565>>8);
        ++h.convertedColors;
    }
    at(0x2e3d2c);return continueSourceSkyPaletteReturnForOccurrence(pc,occurrence);
}
bool Owner::continueSourceSkyPaletteReturn129710(std::uint32_t& pc)noexcept {
    auto& ret=sourceSkyPaletteReturn129710_;
    if(ret.entered||!finishSourceSkyPaletteConstructor(pc,0))return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    ret.entered=true;
    at(0x129714);at(0x129718);
    if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
    at(0x12971c);at(0x129720);at(0x129724);at(0x129728);at(0x12972c);ret.restoreAttempted=true;
    a::OriginalStandardTextureDiagnostic50LA d;
    if(!sourceSkyFile_->restorePaletteStart(d)){at(d.sourcePC?d.sourcePC:0x12f640u);return false;}
    ret.restoreReturned=true;at(0x129730);at(0x129734);at(0x129738);ret.indexS2=1;at(0x12973c);
    at(0x129740);at(0x129744);at(0x129748);at(0x12974c);
    if(!sourceSkyHeader129644_.paletteCount)return false;
    if(*sourceSkyHeader129644_.paletteCount<=1u){
        at(0x129750);ret.field40Cleared=true;at(0x129754);at(0x129758);return continueSourceSkyItems129758(pc);
    }
    at(0x129670);return continueSourceSkyFollowingPaletteDirectory(pc,ret);
}
bool Owner::continueSourceSkyFollowingPaletteDirectory(std::uint32_t& pc,SourceSkyPaletteReturn129710& ret)noexcept {
    if(pc!=0x129670u||ret.nextEntered||!ret.restoreReturned||!ret.indexS2||*ret.indexS2<1||
       !preflightSourcePostParticleModelsNative()||!sourceSkyFile_||!sourceSkyFile_->current()||
       sourceSkyFile_->followingPalettes_.size()+1!=static_cast<std::size_t>(*ret.indexS2))return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    a::OriginalStandardTextureDiagnostic50LA d;
    ret.nextEntered=true;at(0x129670);at(0x129674);
    if(sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
    at(0x129678);at(0x12967c);at(0x129680);at(0x129684);at(0x129688);
    if(!sourceSkyFile_->readFollowingPaletteOffset(d)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    const auto& step=*sourceSkyFile_->followingPalettes_.back();at(0x12968c);at(0x129690);at(0x129694);
    if(step.offsetRead.items!=1u){at(0x129960);return false;}
    const auto& b=step.offsetRead.bytes;ret.nextOffset=std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|
        (std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);
    at(0x129698);at(0x12969c);if(sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
    at(0x1296a0);at(0x1296a4);if(!sourceSkyFile_->tellFollowingPaletteStart(d)){at(d.sourcePC?d.sourcePC:0x12f520u);return false;}
    at(0x1296a8);at(0x1296ac);at(0x1296b0);if(sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
    at(0x1296b4);ret.nextSavedS4=*step.start<=0x7fffffffu?static_cast<std::int64_t>(*step.start):static_cast<std::int64_t>(*step.start)-0x100000000ll;
    at(0x1296b8);at(0x1296bc);at(0x1296c0);at(0x1296c4);
    if(!sourceSkyFile_->seekFollowingPaletteAbsolute(*ret.nextOffset,d)){at(d.sourcePC?d.sourcePC:0x12f640u);return false;}
    at(0x1296c8);at(0x1296cc);at(0x1296d0);
    if(*ret.indexS2==1)return continueSourceSkyOldPalette1296D0(pc);
    if(sourceSkyLaterDispatcherActive_)return false; //Return one actual directory transition to iterative driver.
    return continueSourceSkyLaterPalettes(pc,static_cast<std::size_t>(*ret.indexS2));
}
bool Owner::initializeSourceSkyPaletteBase2E38C0(SourceSkyPaletteOwner& owner,SourceSkyPalette2E38C0& h,std::uint32_t& pc)noexcept {
    if(!owner.storage||!h.allocationNonNull||h.constructorEntered)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    h.constructorEntered=true;at(0x2e38c0);at(0x2e38d8);at(0x2e38dc);at(0x1048f0);
    auto* b=owner.storage.get();
    const auto store32=[&](std::size_t offset,std::uint32_t value){for(std::size_t i=0;i<4;++i)b[offset+i]=static_cast<std::uint8_t>(value>>(i*8));};
    at(0x1048f8);h.vtable08=0x3cc980u;store32(8,*h.vtable08);at(0x1048fc);at(0x104900);
    b[0]=1;for(std::size_t i=1;i<8;++i)b[i]=0;h.reference00=1;
    at(0x104904);at(0x104908);h.baseReturned=true;
    at(0x2e38e0);at(0x2e38e4);at(0x2e38e8);at(0x2e38ec);at(0x2e38f0);at(0x2e38f4);
    at(0x2e38f8);h.vtable08=0x3d5650u;store32(8,*h.vtable08);h.derivedVtableStored=true;
    at(0x2e38fc);return true;
}
bool Owner::continueSourceSkyOldPalette1296D0(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(!occurrence||occurrence>=sourceSkyLaterPalettes_.size()+2)return false;
    auto& old=sourcePaletteHistory(occurrence-1);auto& next=sourcePaletteHistory(occurrence);
    auto& oldOwner=sourcePaletteOwner(occurrence-1);auto& nextOwner=sourcePaletteOwner(occurrence);
    if(pc!=0x1296d0u||next.entered||!preflightSourcePostParticleModelsNative()||
       !sourceSkyFile_||!sourceSkyFile_->current()||!sourceSkyPalette7D0_||sourceSkyPublishedPaletteOccurrence_!=occurrence-1)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x1296d0);at(0x1296d4);at(0x1296d8);
    if(*sourceSkyPalette7D0_){
        if(*sourceSkyPalette7D0_!=oldOwner.get()||!old.constructorReturned||
           old.sourceReleaseEntered||old.reference00!=1u||!oldOwner->storage)return false;
        auto* bytes=oldOwner->storage.get();at(0x1296dc);at(0x1296e0);at(0x1296e4);at(0x1296e8);
        at(0x104970);old.sourceReleaseEntered=true;at(0x104978);at(0x10497c);at(0x104980);
        for(std::size_t i=0;i<8;++i)bytes[i]=0;old.sourceReferenceDecremented=true;
        at(0x104984);at(0x104988);at(0x10498c);at(0x104990);at(0x104994);at(0x104998);at(0x10499c);at(0x1049a0);at(0x1049a4);
        at(0x2e3d50);at(0x2e3d74); //Derived table rewrite is same known3D5650.
        at(0x2e3d78);at(0x2e3d7c);
        if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
        at(0x2e3d80);at(0x2e3d84);a::OriginalStandardTextureDiagnostic50LA d;
        if(!sourceSkyFile_->scanFreshPalette1327D0(d,occurrence-1)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
        old.sourceAssociationScanReturned=true;at(0x2e3d88);at(0x2e3d8c);at(0x2e3d90);at(0x104910);
        at(0x104938);at(0x10493c);for(std::size_t i=0;i<4;++i)bytes[8+i]=static_cast<std::uint8_t>(0x3cc980u>>(i*8));
        old.sourceBaseDestructed=true;at(0x104948);at(0x10494c);at(0x104950);at(0x104954);at(0x104958);
        at(0x2e3d94);at(0x2e3d98);at(0x2e3d9c);at(0x2e3da0);at(0x2e3da4);at(0x2e3da8);
        at(0x2e8b00);
        if(!a::OriginalConfigProcessGlobals50LA::descriptorDelete2E8B00AdmittedNative())return false;
        old.sourceFreeAdmitted=true;
        if(!sourceSkyFile_->retirePaletteDestinationSource(occurrence-1))return false;
        oldOwner->storage.reset();old.sourceBackingFreed=true;
        //Keep source7D0's stale identity represented by metadata until a later
        //source publication overwrites it; never dereference freed backing.
        at(0x2e3dac);at(0x2e3db0);at(0x2e3db4);at(0x2e3db8);at(0x2e3dbc);at(0x2e3dc0);
        at(0x1049a8);at(0x1049ac);at(0x1049b0);at(0x1049b4);old.sourceReleaseReturned=true;
        at(0x1296ec);at(0x1296f0);
    }
    at(0x1296f4);at(0x1296f8);next.entered=true;next.allocationAttempted=true;
    try{nextOwner.reset(new SourceSkyPaletteOwner);}catch(...){return false;}
    try{nextOwner->storage.reset(new std::uint8_t[0x18b18]);}catch(...){nextOwner.reset();}
    next.allocationReturned=true;next.allocationNonNull=bool(nextOwner);at(0x1296fc);at(0x129700);
    if(!next.allocationNonNull){at(0x129710);return continueSourceSkyPaletteReturnForOccurrence(pc,occurrence);}
    at(0x129704);at(0x129708);at(0x12970c);
    if(!initializeSourceSkyPaletteBase2E38C0(*nextOwner,next,pc))return false;
    if(!sourceSkyFile_->beginPalette2E38C0(nextOwner->storage.get(),occurrence))return false;
    return continueSourceSkyPaletteHeaders2E38FC(pc,occurrence);
}
bool Owner::continueSourceSkyNextPaletteReturn129710(std::uint32_t& pc)noexcept {
    if(!finishSourceSkyPaletteConstructor(pc,1))return false;
    pc=sourcePostParticleModelsPC_=0x129714;return continueSourceSkySecondPaletteRestore129714(pc,1);
}
bool Owner::continueSourceSkySecondPaletteRestore129714(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(!occurrence||occurrence>=sourceSkyLaterPalettes_.size()+2||occurrence>=0x7fffffffu)return false;
    auto& ret=sourcePaletteReturn(occurrence);
    if(pc!=0x129714u||ret.entered||!sourcePaletteHistory(occurrence).slotPublished||
       !preflightSourcePostParticleModelsNative()||!sourceSkyFile_||!sourceSkyFile_->current()||
       sourceSkyFile_->followingPalettes_.size()!=occurrence)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    ret.entered=true;at(0x129714);at(0x129718);
    if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
    at(0x12971c);at(0x129720);at(0x129724);at(0x129728);at(0x12972c);ret.restoreAttempted=true;
    a::OriginalStandardTextureDiagnostic50LA d;
    if(!sourceSkyFile_->restoreFollowingPaletteStart(occurrence-1,d)){at(d.sourcePC?d.sourcePC:0x12f640u);return false;}
    ret.restoreReturned=true;at(0x129730);at(0x129734);at(0x129738);ret.indexS2=static_cast<std::int32_t>(occurrence+1);at(0x12973c);
    at(0x129740);at(0x129744);at(0x129748);at(0x12974c);
    if(!sourceSkyHeader129644_.paletteCount)return false;
    //The admitted original header is positive signed32; both palette returns
    //were actually reached before this occurrence-qualified signed comparison.
    if(*sourceSkyHeader129644_.paletteCount<=static_cast<std::uint32_t>(*ret.indexS2)){
        at(0x129750);ret.field40Cleared=true;at(0x129754);at(0x129758);return continueSourceSkyItems129758(pc);
    }
    at(0x129670);return continueSourceSkyFollowingPaletteDirectory(pc,ret);
}
bool Owner::preflightSourceSkyLaterPalettesNative()const noexcept {
    const bool retired=sourceSkyPalette2E38C0_.nativeWithdrawn;
    const auto retAt=[&](std::size_t i)->const SourceSkyPaletteReturn129710& {
        return i==0?sourceSkyPaletteReturn129710_:i==1?sourceSkyNextPaletteReturn129710_:sourceSkyLaterPalettes_[i-2]->returned;
    };
    const auto word=[](const std::array<std::uint8_t,4>& b){return std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|
        (std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);};
    constexpr std::size_t sizes[3]={0x300,0x8000,0x10000};
    for(const auto& row:sourceSkyLaterPalettes_)if(!row)return false;
    for(std::size_t i=1;i<sourceSkyLaterPalettes_.size()+2;++i){
        const auto& h=sourcePaletteHistory(i);const auto& owner=sourcePaletteOwner(i);
        const auto& prev=sourcePaletteHistory(i-1);const auto& previousReturn=retAt(i-1);const auto& ret=retAt(i);
        if((h.entered&&(!previousReturn.nextEntered||!previousReturn.indexS2||
              static_cast<std::size_t>(*previousReturn.indexS2)!=i||(prev.allocationNonNull&&!prev.sourceReleaseReturned)))||
           (h.allocationAttempted&&!h.entered)||(h.allocationReturned&&!h.allocationAttempted)||
           (h.allocationNonNull&&!h.allocationReturned)||(h.constructorEntered&&!h.allocationNonNull)||
           (h.baseReturned&&(!h.constructorEntered||h.reference00!=1u))||
           (h.derivedVtableStored&&(!h.baseReturned||h.vtable08!=0x3d5650u))||
           (h.constructorReturned&&!h.constructorEntered)||(h.slotPublished&&(!h.allocationReturned||(h.allocationNonNull&&!h.constructorReturned)))||
           (h.nativeReferenceWithdrawn&&(!h.baseReturned||!h.nativeWithdrawn||h.sourceReferenceDecremented))||
           (h.nativeWithdrawn&&(!retired||!h.entered))||
           (h.sourceReleaseEntered&&(!ret.nextEntered||!h.slotPublished))||
           (h.sourceReferenceDecremented&&(!h.sourceReleaseEntered||h.reference00!=1u))||
           (h.sourceAssociationScanReturned&&!h.sourceReferenceDecremented)||
           (h.sourceBaseDestructed&&!h.sourceAssociationScanReturned)||(h.sourceFreeAdmitted&&!h.sourceBaseDestructed)||
           (h.sourceBackingFreed&&!h.sourceFreeAdmitted)||(h.sourceReleaseReturned&&!h.sourceBackingFreed))return false;
        if(retired){if(owner||(h.entered&&!h.nativeWithdrawn)||(h.baseReturned&&!h.sourceReferenceDecremented&&!h.nativeReferenceWithdrawn))return false;}
        else if(h.allocationNonNull!=bool(owner))return false;
        const a::OriginalSourceSkyFile50LA::PaletteIteration* step=nullptr;
        if(sourceSkyFile_&&i<=sourceSkyFile_->followingPalettes_.size())step=sourceSkyFile_->followingPalettes_[i-1].get();
        if(h.entered&&(!step||!step->seekReturned||!sourceSkyHeader129644_.paletteCount||i>=*sourceSkyHeader129644_.paletteCount))return false;
        if(step){
            if((step->parserEntered&&!h.derivedVtableStored)||
               (h.sourceAssociationScanReturned&&!step->scanReturned)||(h.sourceBackingFreed&&!step->parserReleasedSource))return false;
            if(step->parserEntered){
                if(retired){if(!step->parserWithdrawn&&!step->parserReleasedSource)return false;}
                else if(h.sourceBackingFreed){if(!step->parserReleasedSource)return false;}
                else if(step->parserWithdrawn||step->parserReleasedSource||!owner||!owner->storage||step->parserReceiver!=owner->storage.get())return false;
            }
            for(std::size_t j=0;j<3;++j){const auto& read=step->headers[j];const auto& payload=step->payloads[j];
                if(h.readAttempted[j]!=read.attempted||h.readReturned[j]!=read.returned||
                   (read.attempted&&(!h.derivedVtableStored||(j&&step->headers[j-1].items!=1u)))||
                   h.payloadAttempted[j]!=payload.attempted||h.payloadReturned[j]!=payload.returned||h.payloadBytes[j]!=payload.copied||
                   (payload.attempted&&(!h.rawEntered||(j&&step->payloads[j-1].items!=1u)))||h.payloadBytes[j]>sizes[j])return false;
                if(j<2&&h.fieldBytes[j]!=read.copied)return false;
                if(!retired&&!h.sourceBackingFreed&&read.copied&&j<2&&(!owner||!owner->storage||
                   std::memcmp(owner->storage.get()+0x10+j*4,read.bytes.data(),read.copied)!=0))return false;
            }
            if(h.compressedFlag&&(step->headers[2].items!=1u||*h.compressedFlag!=word(step->headers[2].bytes)))return false;
            if((h.rawEntered&&h.compressedFlag!=0u)||(h.rawReturned&&(!h.rawEntered||step->payloads[0].items!=1u||
               step->payloads[1].items!=1u||step->payloads[2].items!=1u))||
               (h.count14Read&&(!h.rawReturned||step->headers[1].items!=1u||h.count14!=word(step->headers[1].bytes)))||
               (h.count14.has_value()!=h.count14Read)||h.convertedColors>256||(h.convertedColors&&h.count14!=256u))return false;
        }else if(h.readAttempted[0]||h.sourceReleaseEntered)return false;
        if(!retired&&h.baseReturned&&!h.sourceBackingFreed){
            if(!owner||!owner->storage||!h.vtable08)return false;const auto* b=owner->storage.get();
            if(b[0]!=(h.sourceReferenceDecremented?0:1))return false;for(std::size_t k=1;k<8;++k)if(b[k])return false;
            const auto table=h.sourceBaseDestructed?0x3cc980u:*h.vtable08;
            for(std::size_t k=0;k<4;++k)if(b[8+k]!=((table>>(k*8))&255u))return false;
            for(std::uint32_t c=0;c<h.convertedColors;++c){const auto red=std::uint32_t(b[0x18+c*3]),green=std::uint32_t(b[0x19+c*3]),blue=std::uint32_t(b[0x1a+c*3]);
                const auto a=((red>>3)<<10)|((green>>3)<<5)|(blue>>3),v=((red>>3)<<11)|((green>>2)<<5)|(blue>>3);
                if(b[0x318+c*2]!=(a&255u)||b[0x319+c*2]!=(a>>8)||b[0x518+c*2]!=(v&255u)||b[0x519+c*2]!=(v>>8))return false;
            }
        }
        if((ret.entered&&!h.slotPublished)||(ret.restoreAttempted&&!ret.entered)||
           (ret.restoreReturned&&(!ret.restoreAttempted||!step||!step->restoreReturned))||
           (ret.indexS2&&(!ret.restoreReturned||*ret.indexS2<1||static_cast<std::size_t>(*ret.indexS2)!=i+1))||
           (ret.nextEntered&&(!ret.indexS2||!sourceSkyHeader129644_.paletteCount||static_cast<std::uint32_t>(*ret.indexS2)>=*sourceSkyHeader129644_.paletteCount))||
           (ret.field40Cleared&&(!ret.indexS2||sourceSkyHeader129644_.paletteCount!=static_cast<std::uint32_t>(*ret.indexS2)))||
           (ret.nextSavedS4&&!ret.nextOffset))return false;
        if(ret.nextOffset){
            if(!ret.nextEntered||!sourceSkyFile_||sourceSkyFile_->followingPalettes_.size()<=i)return false;
            const auto& directory=*sourceSkyFile_->followingPalettes_[i];
            if(directory.offsetRead.items!=1u||*ret.nextOffset!=word(directory.offsetRead.bytes)||
               (directory.seekAttempted&&directory.target!=ret.nextOffset))return false;
            if(ret.nextSavedS4&&(!directory.tellReturned||!directory.start||*ret.nextSavedS4!=(*directory.start<=0x7fffffffu?
               static_cast<std::int64_t>(*directory.start):static_cast<std::int64_t>(*directory.start)-0x100000000ll)))return false;
        }
    }
    return true;
}
bool Owner::finishSourceSkyPaletteConstructor(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(occurrence>=sourceSkyLaterPalettes_.size()+2)return false;
    auto& h=sourcePaletteHistory(occurrence);auto& owner=sourcePaletteOwner(occurrence);
    if((pc!=0x2e3d2cu&&pc!=0x2e3d34u&&pc!=0x129710u)||h.slotPublished||
       !preflightSourcePostParticleModelsNative()||!sourceSkyFile_||!sourceSkyFile_->current())return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    if(pc!=0x129710u){
        if(!h.constructorEntered||h.constructorReturned||!owner)return false;
        if(pc==0x2e3d2cu){if(h.convertedColors!=256u)return false;at(0x2e3d30);}else at(0x2e3d38);
        at(0x2e3d3c);at(0x2e3d40);at(0x2e3d44);at(0x2e3d48);at(0x2e3d4c);h.constructorReturned=true;
        at(0x129710);
    }else if(h.allocationNonNull)return false;
    sourceSkyPalette7D0_=owner.get();sourceSkyPublishedPaletteOccurrence_=occurrence;h.slotPublished=true;
    return true; //Actual store/control return only, not resource readiness.
}
bool Owner::continueSourceSkyPaletteReturnForOccurrence(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(!occurrence)return continueSourceSkyPaletteReturn129710(pc);
    if(!finishSourceSkyPaletteConstructor(pc,occurrence))return false;
    pc=sourcePostParticleModelsPC_=0x129714;return continueSourceSkySecondPaletteRestore129714(pc,occurrence);
}
bool Owner::continueSourceSkyLaterPalettes(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(sourceSkyLaterDispatcherActive_||occurrence<2||pc!=0x1296d0u)return false;
    struct ActiveScope {bool& active;explicit ActiveScope(bool& b):active(b){active=true;}~ActiveScope(){active=false;}} scope(sourceSkyLaterDispatcherActive_);
    for(;;){
        if(!sourceSkyHeader129644_.paletteCount||occurrence>=*sourceSkyHeader129644_.paletteCount||
           occurrence!=sourceSkyLaterPalettes_.size()+2||!sourceSkyFile_||sourceSkyFile_->followingPalettes_.size()!=occurrence||
           !sourceSkyFile_->followingPalettes_[occurrence-1]->seekReturned||!preflightSourcePostParticleModelsNative())return false;
        //Native ledger allocation precedes source release/allocation. Refusal
        //keeps the prior published owner untouched; never a guest NULL result.
        try{auto row=std::unique_ptr<SourceSkyLaterPalette>(new SourceSkyLaterPalette);sourceSkyLaterPalettes_.push_back(std::move(row));}
        catch(...){return false;}
        (void)continueSourceSkyOldPalette1296D0(pc,occurrence);
        const auto& done=sourceSkyLaterPalettes_.back()->returned;
        if(pc!=0x1296d0u||!done.restoreReturned||!done.nextEntered||!done.indexS2||
           static_cast<std::size_t>(*done.indexS2)!=occurrence+1||sourceSkyFile_->followingPalettes_.size()!=occurrence+1||
           !sourceSkyFile_->followingPalettes_[occurrence]->seekReturned)return false;
        ++occurrence; //Iterative source loop; native call stack is independent of authored count.
    }
}
bool Owner::preflightSourceSkyItemsNative()const noexcept {
    const auto& h=sourceSkyItems129758_;
    if((h.allocationAttempted&&(!h.entered||!h.count7D8||*h.count7D8>0x3fffffffu))||
       (h.arrayBytes&&(!h.count7D8||*h.arrayBytes!=*h.count7D8*4u))||
       (h.allocationReturned&&!h.allocationAttempted)||(h.allocationNonNull&&!h.allocationReturned)||
       (h.published&&!h.allocationReturned)||(h.indexS4&&(!h.published||*h.indexS4!=0))||
       (h.offsetS2&&(!h.indexS4||*h.offsetS2!=0))||(h.directoryEntered&&(!h.offsetS2||!h.count7D8||!*h.count7D8))||
       (h.itemAllocationAttempted&&(!h.directoryEntered||!h.absoluteOffset))||
       (h.itemAllocationReturned&&!h.itemAllocationAttempted)||(h.itemAllocationNonNull&&!h.itemAllocationReturned)||
       (h.constructorEntered&&!h.itemAllocationNonNull)||(h.baseReturned&&(!h.constructorEntered||h.reference00!=(h.failureReferenceStored?0u:1u)))||
       h.callbacks>6||h.defaultGroups>6||(h.constructorReturned&&(!h.baseReturned||h.callbacks!=6||h.defaultGroups!=6||!h.origin3E0900))||
       (h.parserEntered&&!h.itemAllocationReturned)||(h.handleStored&&(!h.constructorReturned||!h.parserEntered||!h.handle40))||
       (h.headerAttempted&&!h.handleStored)||(h.headerReturned&&!h.headerAttempted)||h.headerCopied>4||
       (h.referenceWithdrawnNative&&(!h.nativeWithdrawn||!h.baseReturned))||
       (h.parserModeA2&&*h.parserModeA2!=0u)||(h.parserEntered&&!h.parserModeA2)||
       (h.dataPosition650&&(!h.headerReturned||!sourceSkyFile_||!sourceSkyFile_->firstItem_||
        !sourceSkyFile_->firstItem_->finalTellReturned||!sourceSkyFile_->firstItem_->finalPosition||
        *h.dataPosition650!=static_cast<std::uint32_t>(*sourceSkyFile_->firstItem_->finalPosition)))||
       (h.parserReturned&&((!h.dataPosition650&&!h.failureNullReturned)||h.parserModeA2!=0u))||
       (h.slotStored&&(!h.parserReturned||!h.allocationNonNull))||
       (h.restoreAttempted&&!h.slotStored)||(h.restoreReturned&&!h.restoreAttempted)||
       (h.advancedIndexS4&&(!h.restoreReturned||*h.advancedIndexS4!=1u||h.advancedOffsetS2!=4u))||
       (h.loopCount7D8&&(!h.advancedIndexS4||h.loopCount7D8!=h.count7D8))||
       (h.nextDirectoryEntered&&(!h.loopCount7D8||*h.loopCount7D8<=1u)))return false;
    if(h.entered){
        if(!sourceSkyHeader129644_.count7D8||h.count7D8!=sourceSkyHeader129644_.count7D8)return false;
        if(!h.nativeWithdrawn&&(!sourceSkyOwner_||sourceSkyOwner_->count7D8!=*h.count7D8))return false;
    }
    if(h.directoryEntered){if(!sourceSkyFile_)return false;
        if(sourceSkyFile_->firstItem_){const auto& item=*sourceSkyFile_->firstItem_;
            const auto word=[](const std::array<std::uint8_t,4>& b){return std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|
                (std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);};
            if(h.absoluteOffset&&(item.offsetRead.items!=1u||*h.absoluteOffset!=word(item.offsetRead.bytes)||
               (item.seekAttempted&&item.target!=h.absoluteOffset)))return false;
            if(h.savedS5&&(!item.tellReturned||!item.start||*h.savedS5!=(*item.start<=0x7fffffffu?
               static_cast<std::int64_t>(*item.start):static_cast<std::int64_t>(*item.start)-0x100000000ll)))return false;
            if(item.header48.attempted!=h.headerAttempted||item.header48.returned!=h.headerReturned||
               item.header48.copied!=h.headerCopied||item.header48.items!=h.headerItems)return false;
            if(item.parserEntered&&(!h.handleStored||(!h.nativeWithdrawn&&(!sourceSkyFirstItem_||
               item.receiver!=sourceSkyFirstItem_->storage.get()||item.nativeWithdrawn))))return false;
            if(h.nativeWithdrawn&&!item.nativeWithdrawn)return false;
        }else if(h.absoluteOffset||h.itemAllocationAttempted)return false;
    }
    if(!preflightSourceSkyItemOccurrence(0))return false;
    if(!sourceSkyLaterItems_.empty()&&(!sourceSkyFile_||sourceSkyLaterItems_.size()>sourceSkyFile_->followingItems_.size()))return false;
    for(std::size_t i=1;i<=sourceSkyLaterItems_.size();++i)if(!sourceSkyLaterItems_[i-1]||!preflightSourceSkyItemOccurrence(i))return false;
    for(std::size_t i=0;i<=sourceSkyLaterItems_.size();++i){const auto& loop=sourceItemLoop(i);const auto& state=sourceItemHistory(i);
        const auto* item=sourceSkyFile_?sourceSkyFile_->itemOccurrence(i):nullptr;
        if((loop.restoreAttempted&&!state.slotStored)||(loop.restoreReturned&&!loop.restoreAttempted)||
           (loop.advancedIndexS4&&(!loop.restoreReturned||*loop.advancedIndexS4!=i+1u||loop.advancedOffsetS2!=(i+1u)*4u))||
           (loop.loopCount7D8&&(!loop.advancedIndexS4||loop.loopCount7D8!=h.count7D8))||
           (loop.nextDirectoryEntered&&(!loop.loopCount7D8||*loop.loopCount7D8<=i+1u)))return false;
        if(item){if(item->restoreAttempted!=loop.restoreAttempted||item->restoreReturned!=loop.restoreReturned)return false;}
        else if(loop.restoreAttempted||loop.nextDirectoryEntered)return false;
        const auto* next=sourceSkyFile_?sourceSkyFile_->itemOccurrence(i+1):nullptr;
        if(loop.nextDirectoryEntered){
            if(next){const auto& b=next->offsetRead.bytes;
                if(loop.nextAbsoluteOffset&&(next->offsetRead.items!=1u||*loop.nextAbsoluteOffset!=(std::uint32_t(b[0])|
                    (std::uint32_t(b[1])<<8)|(std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24))||
                    (next->seekAttempted&&next->target!=loop.nextAbsoluteOffset)))return false;
                if(loop.nextSavedS5&&(!next->tellReturned||!next->start||*loop.nextSavedS5!=(*next->start<=0x7fffffffu?
                    static_cast<std::int64_t>(*next->start):static_cast<std::int64_t>(*next->start)-0x100000000ll)))return false;
                if((next->parserEntered&&i+1>sourceSkyLaterItems_.size())||(h.nativeWithdrawn&&!next->nativeWithdrawn))return false;
            }else if(loop.nextAbsoluteOffset||loop.nextSavedS5)return false;
        }else if(next)return false;
    }
    if(bool(sourceSkySelected38_)!=bool(sourceSkySelectedItemOccurrence_))return false;
    if(sourceSkyPostItems_.entered){
        if(!h.count7D8||sourceSkyPostItems_.indexS4>*h.count7D8||sourceSkyPostItems_.offsetS2!=sourceSkyPostItems_.indexS4*4u)return false;
        if(sourceSkyPostItems_.nativeWithdrawn){if(sourceSkySelected38_||sourceSkySelectedItemOccurrence_)return false;}
        else if(sourceSkySelectedItemOccurrence_){const auto i=*sourceSkySelectedItemOccurrence_;
            if(i>sourceSkyLaterItems_.size()||!sourceItemHistory(i).registration.field38Stored||*sourceSkySelected38_!=sourceItemOwner(i).get())return false;}
    }else if(sourceSkySelected38_||sourceSkySelectedItemOccurrence_)return false;
    const auto& post=sourceSkyPostItems_;
    if(post.tailEntered){if(!post.entered||!h.count7D8||post.indexS4!=*h.count7D8)return false;
        for(std::size_t i=0;i<post.indexS4;++i)if(!sourceItemHistory(i).registration.returned)return false;}
    if((post.index40Stored&&!post.tailEntered)||(post.firstSlotRead&&!post.index40Stored)||
       (post.firstSlotSelected&&!post.firstSlotRead)||(post.packetCallEntered&&!post.firstSlotSelected)||
       (post.packetAllocationAttempted&&!post.packetCallEntered)||(post.packetAllocationReturned&&!post.packetAllocationAttempted)||
       (post.packetZeroed&&!post.packetAllocationReturned)||(post.packetHeaderStored&&!post.packetZeroed))return false;
    if(post.packetItemLanesStored>3u||(post.packetItemLanesStored&&!post.packetHeaderStored)||
       (post.packetMatrixStored&&post.packetItemLanesStored!=3u)||(post.packetTagsStored&&!post.packetMatrixStored))return false;
    if((post.packetPointerStored&&!post.packetTagsStored)||(post.packetReturned&&!post.packetPointerStored)||
       (post.packetPublished7E0&&!post.packetReturned))return false;
    if(post.nativeWithdrawn){if(post.packetBacking||post.packet340||post.packetData04||post.packet7E0||!post.packetNativeWithdrawn)return false;}
    else {
        if(post.firstSlotSelected&&(!sourceSkySelectedItemOccurrence_||*sourceSkySelectedItemOccurrence_!=0||!sourceSkySelected38_||*sourceSkySelected38_!=sourceItemOwner(0).get()))return false;
        if(bool(post.packetBacking)!=bool(post.packet340))return false;
        if(post.packetPointerStored){if(!post.packet340||post.packetData04!=post.packet340+0x10)return false;}else if(post.packetData04)return false;
        if(post.packetPublished7E0){if(post.packet7E0!=post.packet340||!sourceSkyOwner_)return false;}else if(post.packet7E0)return false;
        if(post.packet340){const auto base=reinterpret_cast<std::uintptr_t>(post.packetBacking.get()),address=reinterpret_cast<std::uintptr_t>(post.packet340);
            if(!post.packetAllocationReturned||address%64||address<base||address-base>63)return false;}
        if(post.packetZeroed){if(!post.packet340)return false;std::array<std::uint8_t,0x340> expected{};
            if(post.packetHeaderStored){const auto word=[&](std::size_t off,std::uint32_t value){for(std::size_t k=0;k<4;++k)expected[off+k]=static_cast<std::uint8_t>(value>>(8*k));};
                word(0x10,0x11000000);word(0x14,0x03000000);word(0x18,0x02000000);word(0x1c,0x6c028320);
                expected[0x20]=1;expected[0x21]=0x80;expected[0x27]=0x10;expected[0x28]=0x0e;
                word(0x30,0x0005000c);word(0x38,0x47);word(0x40,0x14000000);word(0x4c,0x6c2a8000);
                word(0x50,0x3f800000);word(0x64,0x3f800000);word(0x78,0x3f800000);}
            for(std::size_t lane=0;lane<post.packetItemLanesStored;++lane){
                const auto* selected=sourceItemOwner(0).get();
                if(!selected||!selected->storage)return false;
                for(std::size_t k=0;k<4;++k){const auto value=static_cast<std::uint8_t>(post.packetItemWords[lane]>>(8*k));
                    if(!selected->known[0x10+4*lane+k]||selected->storage[0x10+4*lane+k]!=value)return false;
                    expected[0x80+4*lane+k]=value;}
            }
            if(post.packetMatrixStored){const auto word=[&](std::size_t off,std::uint32_t value){for(std::size_t k=0;k<4;++k)expected[off+k]=static_cast<std::uint8_t>(value>>(8*k));};
                for(const auto off:{0x8cu,0x190u,0x1a4u,0x1b8u,0x1ccu})word(off,0x3f800000u);
                for(const auto off:{0x1e8u,0x1e4u,0x1e0u,0x1f8u,0x1f4u,0x1f0u,0x208u,0x204u,0x200u,0x218u,0x214u,0x210u})word(off,0x42fe0000u);
            }
            if(post.packetTagsStored){const auto word=[&](std::size_t off,std::uint32_t value){for(std::size_t k=0;k<4;++k)expected[off+k]=static_cast<std::uint8_t>(value>>(8*k));};
                expected[0x270]=5;expected[0x271]=0x80;expected[0x275]=0xc0;expected[0x276]=0x0e;expected[0x277]=0x30;expected[0x278]=0x12;expected[0x279]=4;
                for(const auto off:{0x280u,0x2a0u}){expected[off]=1;expected[off+1]=0x80;expected[off+5]=0x40;expected[off+7]=0x10;expected[off+8]=0x0f;}
                word(0x29c,0x7f);word(0x2bc,0x7f);word(0x2e0,6);word(0x2fc,0x6c02802b);
                word(0x320,0x14000010);word(0x324,0x10000000);word(0x328,0x030000b5);word(0x32c,0x02000190);
                expected[0]=0x33;expected[3]=0x30;
            }
            for(std::size_t k=0;k<expected.size();++k){
                if(post.packetPointerStored&&k>=4&&k<8)continue; //EE address bytes are unknown; typed alias checked above.
                if(post.packet340[k]!=expected[k])return false;}}
    }
    if(post.rowsEntered&&!post.packetPublished7E0)return false;
    if(post.rowIndexS4>=96u||post.rowOffsetS2!=post.rowIndexS4*4u)return false;
    for(std::size_t i=0;i<post.rows.size();++i){const auto* row=post.rows[i].get();
        if(!row){if(post.rows7E8[i])return false;continue;}
        if(!post.rowsEntered||i>post.rowIndexS4||
           (row->allocationReturned&&!row->allocationAttempted)||
           (row->constructorReturned&&!row->allocationReturned)||
           (row->slot7E8Stored&&!row->allocationReturned)||
           (row->helper12A510Entered&&(!row->slot7E8Stored||!row->constructorReturned)))return false;
        if(post.nativeWithdrawn){if(!row->nativeWithdrawn||row->object30||post.rows7E8[i]||row->packetSkyThis||row->packetBacking||row->packetC0)return false;continue;}
        if(row->nativeWithdrawn)return false;
        if((row->packetSkyThis&&(!row->helper12A510Entered||row->packetSkyThis!=sourceSkyOwner_.get()))||
           (row->packetAllocationAttempted&&!row->packetSkyThis)||
           (row->packetAllocationReturned&&!row->packetAllocationAttempted)||
           (row->packetZeroed&&!row->packetAllocationReturned)||(row->packetPrefixStored&&!row->packetZeroed))return false;
        if(bool(row->packetBacking)!=bool(row->packetC0))return false;
        if(row->packetC0){const auto base=reinterpret_cast<std::uintptr_t>(row->packetBacking.get()),address=reinterpret_cast<std::uintptr_t>(row->packetC0);
            if(!row->packetAllocationReturned||address%64||address<base||address-base>63)return false;}
        if(row->packetZeroed){if(!row->packetC0)return false;std::array<std::uint8_t,0xc0> expected{};
            if(row->packetPrefixStored){const auto word=[&](std::size_t off,std::uint32_t value){for(std::size_t k=0;k<4;++k)expected[off+k]=static_cast<std::uint8_t>(value>>(8*k));};
                word(0x10,0x11000000);word(0x14,0x03000000);word(0x18,0x02000000);word(0x1c,0x6c038320);
                expected[0x20]=1;expected[0x21]=0x80;expected[0x27]=0x20;expected[0x28]=0xee;word(0x38,6);}
            for(std::size_t k=0;k<expected.size();++k)if(row->packetC0[k]!=expected[k])return false;}

        if(row->object30&&!row->allocationReturned)return false;
        if(row->slot7E8Stored){if(post.rows7E8[i]!=row->object30.get())return false;
            if(row->object30&&!row->constructorReturned)return false;
        }else if(post.rows7E8[i])return false;
        for(std::size_t k=0;k<0x30;++k){const bool expected=row->constructorReturned&&k>=0x20;
            if(row->known[k]!=expected)return false;
            if(expected&&(!row->object30||row->object30[k]!=(k>=0x2c?0xff:0)))return false;}
    }
    if(h.nativeWithdrawn)return h.entered&&!sourceSkyItemSlots3C_&&!sourceSkyPublishedItems3C_;
    if(h.allocationNonNull!=bool(sourceSkyItemSlots3C_)||h.published!=bool(sourceSkyPublishedItems3C_)||
       (sourceSkyPublishedItems3C_&&*sourceSkyPublishedItems3C_!=sourceSkyItemSlots3C_.get()))return false;
    return true;
}
bool Owner::preflightSourceSkyItemOccurrence(std::size_t occurrence)const noexcept {
    const auto& h=sourceItemHistory(occurrence);const auto& owner=sourceItemOwner(occurrence);
    const auto* item=sourceSkyFile_?sourceSkyFile_->itemOccurrence(occurrence):nullptr;
    if((h.itemAllocationAttempted&&(!item||!item->seekReturned))||
       (h.itemAllocationReturned&&!h.itemAllocationAttempted)||(h.itemAllocationNonNull&&!h.itemAllocationReturned)||
       (h.constructorEntered&&!h.itemAllocationNonNull)||(h.baseReturned&&(!h.constructorEntered||h.reference00!=(h.failureReferenceStored?0u:1u)))||
       h.callbacks>6||h.defaultGroups>6||(h.constructorReturned&&(!h.baseReturned||h.callbacks!=6||h.defaultGroups!=6||!h.origin3E0900))||
       (h.parserEntered&&(!h.itemAllocationReturned||h.parserModeA2!=0u))||
       (h.handleStored&&(!h.constructorReturned||!h.parserEntered||!h.handle40))||
       (h.headerAttempted&&!h.handleStored)||(h.headerReturned&&!h.headerAttempted)||h.headerCopied>4||
       (h.referenceWithdrawnNative&&(!h.nativeWithdrawn||!h.baseReturned))||
       (h.dataPosition650&&(!item||!item->finalTellReturned||!item->finalPosition||
        *h.dataPosition650!=static_cast<std::uint32_t>(*item->finalPosition)))||
       (h.parserReturned&&((!h.dataPosition650&&!h.failureNullReturned)||h.parserModeA2!=0u))||
       (h.slotStored&&(!h.parserReturned||!sourceSkyItems129758_.allocationNonNull)))return false;
    if((h.failureReleaseEntered&&(!h.parserEntered||(h.parserReturned&&!h.failureNullReturned)||(h.slotStored&&!h.failureNullReturned)||h.registration.entered))||
       (h.failureReferenceStored&&!h.failureReleaseEntered)||(h.failureDestructorEntered&&!h.failureReferenceStored)||
       h.failureNullSlotsRead>96u||(h.failureNullSlotsRead&&!h.failureDestructorEntered)||
       (h.failureBaseReturned&&h.failureNullSlotsRead!=96u)||
       (h.failureReferenceStored&&h.referenceWithdrawnNative)||
       (h.failureSourceDeleted&&(!h.failureBaseReturned||h.failureNullSlotsRead!=96u||h.vtable08!=0x3cc980u||h.reference00!=0u))||
       (h.failureNullReturned&&(!h.failureSourceDeleted||!h.parserReturned)))return false;
    if(h.failureReleaseEntered){bool sourceFailure=item&&item->header48.returned&&item->header48.items!=1u;
        if(item)for(const auto& field:item->fields)if(field.returned&&field.items!=1u)sourceFailure=true;
        if(!sourceFailure)return false;}
    if(item){if(item->header48.attempted!=h.headerAttempted||item->header48.returned!=h.headerReturned||
       item->header48.copied!=h.headerCopied||item->header48.items!=h.headerItems)return false;
       if(item->parserEntered&&(!h.handleStored||(!h.nativeWithdrawn&&!h.failureSourceDeleted&&(!owner||item->receiver!=owner->storage.get()||item->nativeWithdrawn||item->parserReleasedSource))))return false;
       if(h.nativeWithdrawn&&!item->nativeWithdrawn)return false;
       if(item->parserReleasedSource!=h.failureSourceDeleted)return false;}
    const auto& reg=h.registration;
    if(reg.completedEntries>96||(reg.entered&&(!h.slotStored||!sourceSkyPostItems_.entered))||
       (reg.field38Stored&&!reg.entered)||(reg.paletteRead&&!reg.field38Stored)||
       (reg.seekAttempted&&!reg.paletteRead)||(reg.seekReturned&&!reg.seekAttempted)||
       (reg.returned&&(!reg.seekReturned||reg.completedEntries!=96||reg.sourceResult!=1u))||
       (reg.sourceResult&&(!reg.returned||*reg.sourceResult!=1u)))return false;
    if(reg.paletteRead&&!reg.nativeWithdrawn){
        if(reg.paletteOccurrence){if(sourceSkyPublishedPaletteOccurrence_!=reg.paletteOccurrence||!sourceSkyPalette7D0_||
           reg.paletteBorrow!=*sourceSkyPalette7D0_||reg.paletteBorrow!=sourcePaletteOwner(*reg.paletteOccurrence).get())return false;}
        else if(reg.paletteBorrow||sourceSkyPalette7D0_||sourceSkyConstructor288A50_.nullFields[7]!=0u)return false;
    }
    if(reg.nativeWithdrawn&&reg.paletteBorrow)return false;
    if(reg.seekAttempted){if(!sourceSkyFile_||occurrence>=sourceSkyFile_->itemResourceSeeks_.size()||
        !sourceSkyFile_->itemResourceSeeks_[occurrence]->attempted||sourceSkyFile_->itemResourceSeeks_[occurrence]->returned!=reg.seekReturned)return false;}
    if(!preflightSourceSkyResourcePrefix(occurrence))return false;
    if(h.nativeWithdrawn)return !owner&&(!h.baseReturned||h.referenceWithdrawnNative||h.failureReferenceStored)&&(!reg.entered||reg.nativeWithdrawn);
    if(reg.entered){if(!owner||!owner->storage)return false;
        for(std::size_t i=0;i<96;++i){if(i<reg.completedEntries&&!reg.masks[i])return false;
            if(reg.masks[i]){const auto off=0x54+(i/16)*0x100;
                if(!reg.seekReturned||!owner->known[off]||!owner->known[off+1]||*reg.masks[i]!=(std::uint16_t(owner->storage[off])|
                    (std::uint16_t(owner->storage[off+1])<<8)))return false;
                if(i<reg.completedEntries&&(*reg.masks[i]&(1u<<(i%16)))){
                    const SourceSkyResourcePrefix* completed=nullptr;
                    for(const auto& resource:reg.completedResources)if(resource&&resource->group*16+resource->entry==i){if(completed)return false;completed=resource.get();}
                    if(!completed||!completed->resourceSourceReturned||completed->resourceSourceResult!=1u)return false;
                }
            }}
    }
    if(h.itemAllocationNonNull!=bool(owner))return false;
    if(h.slotStored&&(!sourceSkyItemSlots3C_||!sourceSkyItems129758_.count7D8||occurrence>=*sourceSkyItems129758_.count7D8||
       sourceSkyItemSlots3C_[occurrence]!=(h.failureNullReturned?nullptr:owner.get())))return false;
    if(h.failureSourceDeleted)return owner&&!owner->storage&&item&&item->parserReleasedSource&&!item->receiver;
    if(h.baseReturned){if(!owner||!owner->storage||!h.vtable08)return false;
        const auto* b=owner->storage.get();const auto& known=owner->known;
        for(std::size_t k=0;k<8;++k)if(!known[k]||b[k]!=(k?0:(h.failureReferenceStored?0:1)))return false;
        for(std::size_t k=0;k<4;++k)if(!known[8+k]||b[8+k]!=((*h.vtable08>>(k*8))&255u))return false;
        if(h.handleStored){for(std::size_t k=0;k<8;++k)if(!known[0x40+k]||b[0x40+k]!=((*h.handle40>>(k*8))&255u))return false;}
        if(h.headerCopied){const auto& read=item->header48;
            for(std::size_t k=0;k<h.headerCopied;++k)if(!known[0x48+k]||b[0x48+k]!=read.bytes[k])return false;}
        if(h.origin3E0900){if(!worldApplication_||*h.origin3E0900!=worldApplication_->skyOrigin3E0900_)return false;}
        if(h.constructorReturned){
            const auto expect=[&](std::size_t off,std::uint64_t value,std::size_t size){
                for(std::size_t k=0;k<size;++k){auto byte=static_cast<std::uint8_t>(value>>(k*8));
                    //Later source reads replace only their copied prefix. Every
                    //untouched constructor byte (especially vector W) persists.
                    if(sourceSkyFile_&&item){const auto& fields=item->fields;
                        for(std::size_t i=1;i<fields.size();++i){const auto spec=rr::assets::OriginalSourceSkyFile50LA::itemFieldSpec(i);
                            if(off+k>=spec.destination&&off+k-spec.destination<fields[i].copied)byte=fields[i].bytes[off+k-spec.destination];}}
                    if(!known[off+k]||b[off+k]!=byte)return false;
                }return true;};
            if(!expect(0x650,h.dataPosition650.value_or(0),4)||!expect(0x4c,0,2)||!expect(0x4e,0xffff,2))return false;
            if(!h.handleStored&&!expect(0x40,~std::uint64_t(0),8))return false;
            for(std::size_t k=h.headerCopied;k<4;++k)if(!known[0x48+k]||b[0x48+k])return false;
            for(std::size_t j=0;j<3;++j)for(std::size_t k=0;k<4;++k)if(!expect(0x10+j*16+k*4,(*h.origin3E0900)[k],4))return false;
            for(std::size_t j=0;j<6;++j){const auto off=j*0x100;
                if(!expect(off+0x50,0,4)||!expect(off+0x54,0,2)||!expect(off+0x56,0,2)||
                   !expect(off+0x140,0,4)||!expect(off+0x144,0,4)||!expect(off+0x148,0,4)||!expect(off+0x14c,0x3f800000u,4))return false;
                for(std::size_t k=0;k<16;++k){
                    const auto* resource=h.registration.pendingResource.get();
                    if(resource&&(resource->group!=j||resource->entry!=k))resource=nullptr;
                    for(const auto& completed:h.registration.completedResources)if(completed&&completed->group==j&&completed->entry==k){if(resource)return false;resource=completed.get();}
                    const bool published=resource&&resource->slot58Stored&&resource->group==j&&resource->entry==k;
                    if(published&&resource->allocationNonNull){
                        if(owner->resources58[j*16+k]!=resource->texture.get())return false;
                        for(std::size_t b=0;b<4;++b)if(known[off+0x58+k*4+b])return false;
                    }else if(owner->resources58[j*16+k]||!expect(off+0x58+k*4,0,4))return false;
                    if(!expect(off+0x98+k*4,0,4)||!expect(off+0xd8+k*2,0xffff,2)||!expect(off+0xf8+k*4,0xffffffffu,4))return false;
                }
            }
        }
    }
    return true;
}
bool Owner::withdrawSourceSkyItemsNative()noexcept {
    auto& root=sourceSkyItems129758_;if(!preflightSourceSkyItemsNative())return false;
    if(!root.entered||root.nativeWithdrawn)return true;
    if(sourceSkyFile_&&!sourceSkyFile_->withdrawFirstItemNative())return false;
    // Later rows retire in reverse order before their earlier packet owner.
    // Source133FA0 would free field20 then optionally object30. This admitted
    // constructor prefix has field20 knownNULL and has created no child buffer;
    // native disposal does not pretend the guest destructor was called.
    for(std::size_t n=sourceSkyPostItems_.rows.size();n;){--n;
        auto* row=sourceSkyPostItems_.rows[n].get();if(!row)continue;
        //12A510's later independent packet retires before the row30 owner.
        row->packetSkyThis=nullptr;row->packetC0=nullptr;row->packetBacking.reset();
        sourceSkyPostItems_.rows7E8[n]=nullptr;row->object30.reset();row->nativeWithdrawn=true;}
    //12A120 packet custody ends before selected item owners.
    // Withdraw published base and interior address before freeing their single owner.
    sourceSkyPostItems_.packet7E0=nullptr;sourceSkyPostItems_.packetData04=nullptr;
    sourceSkyPostItems_.packet340=nullptr;sourceSkyPostItems_.packetBacking.reset();sourceSkyPostItems_.packetNativeWithdrawn=true;
    sourceSkySelected38_.reset();sourceSkySelectedItemOccurrence_.reset();if(sourceSkyPostItems_.entered)sourceSkyPostItems_.nativeWithdrawn=true;
    sourceSkyPublishedItems3C_.reset();sourceSkyItemSlots3C_.reset();
    for(std::size_t n=sourceSkyLaterItems_.size()+1;n;){--n;auto& h=sourceItemHistory(n);auto& owner=sourceItemOwner(n);
        for(std::size_t ri=h.registration.completedResources.size()+1;ri;){--ri;
            auto* resourcePointer=ri==h.registration.completedResources.size()?h.registration.pendingResource.get():h.registration.completedResources[ri].get();
            if(!resourcePointer)continue;auto& resource=*resourcePointer;
            // Shared buffer borrows end before manager pin/texture detach; backing
            // stays with443870 for the next actual texture call.
            resource.payloadVirtualBorrow=nullptr;
            if(resource.loaderBufferManager){for(std::size_t slot=2;slot;){--slot;
                if(!resource.loaderBufferManager->withdrawSourceBufferNative(slot,&resource,resource.loaderBuffers[slot]))return false;}
                resource.loaderBufferManager.reset();}
            // Withdraw item alias and in-flight call borrows before manager detach/backing disposal.
            if(resource.slot58Stored){owner->resources58[resource.group*16+resource.entry]=nullptr;resource.slot58WithdrawnNative=true;}
            resource.loaderReceiver=nullptr;resource.postLoaderReceiver=nullptr;resource.loaderPaletteBorrow=nullptr;resource.payloadVirtualBorrow=nullptr;resource.resourcePaletteB4=nullptr;
            // Native custody ends without inventing a source counter decrement:
            //435940 is cumulative; source destructor has no such subtraction.
            for(std::size_t i=resource.followingCompressedLevels.size();i;){--i;auto& next=*resource.followingCompressedLevels[i];
                next.liveOffset118=0;next.backing.aligned=nullptr;next.backing.allocation.reset();next.backing.nativeWithdrawn=true;}
            resource.compressedLiveOffset118=0;resource.compressedBacking.aligned=nullptr;resource.compressedBacking.allocation.reset();
            resource.compressedBacking.nativeWithdrawn=true;
            resource.embeddedFCPublished=nullptr;
            if(resource.embedded){auto& embedded=*resource.embedded;embedded.cursor04=nullptr;embedded.data00=nullptr;
                embedded.alignedBacking.reset();embedded.object20.reset();embedded.nativeWithdrawn=true;}
            if(resource.texture&&!resource.texture->detachNative())return false;
            if(resource.baseReferenceAcquired){for(std::size_t k=0;k<8;++k)resource.texture->storage_[k]=0;resource.nativeReferenceRetired=true;}
            if(resource.texture)resource.texture->storage_.reset();resource.nativeWithdrawn=true;
        }
        if(h.registration.entered){h.registration.paletteBorrow=nullptr;h.registration.nativeWithdrawn=true;}
        if(h.baseReturned&&!h.failureReferenceStored){for(std::size_t k=0;k<8;++k)owner->storage[k]=0;h.referenceWithdrawnNative=true;}
        owner.reset();h.nativeWithdrawn=true;
    }
    return true; //Native inverse only; no source destructor/return receipt invented.
}
bool Owner::continueSourceSkyItems129758(std::uint32_t& pc)noexcept {
    auto& h=sourceSkyItems129758_;
    if(pc!=0x129758u||h.entered||!preflightSourcePostParticleModelsNative()||!sourceSkyOwner_||!sourceSkyFile_||!sourceSkyFile_->current()||
       sourceSkyHeader129644_.countReadItems!=1u||!sourceSkyHeader129644_.paletteCount)return false;
    const auto palettes=*sourceSkyHeader129644_.paletteCount;
    if(palettes==0||palettes>=0x80000000u){if(!sourceSkyHeader129644_.field40Cleared)return false;}
    else {if(palettes>sourceSkyLaterPalettes_.size()+2||!sourcePaletteReturn(palettes-1).field40Cleared)return false;}
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    h.entered=true;h.count7D8=sourceSkyOwner_->count7D8;
    //Native admission, not source validation: negative/wrapped32-bit counts
    //cannot be used as safe host pointer-array lengths.
    if(*h.count7D8>0x3fffffffu)return false;
    h.arrayBytes=*h.count7D8*4u;at(0x129758);at(0x12975c);h.allocationAttempted=true;
    try{sourceSkyItemSlots3C_.reset(new SourceSkyItemOwner*[*h.count7D8]);}catch(...){sourceSkyItemSlots3C_.reset();}
    h.allocationReturned=true;h.allocationNonNull=bool(sourceSkyItemSlots3C_);at(0x129760);
    sourceSkyPublishedItems3C_=sourceSkyItemSlots3C_.get();h.published=true;
    at(0x129764);h.indexS4=0;at(0x129768);at(0x12976c);h.offsetS2=0;
    at(0x129848);at(0x12984c);at(0x129850);at(0x129854);
    if(!*h.count7D8){at(0x129858);return continueSourceSkyPostItems(pc);}
    h.directoryEntered=true;at(0x129770);at(0x129774);
    const auto resolverCurrent=[&](){return sourceTrackMountDomain_&&sourceTrackMountDomain_->resolver3E1A70()==sourceSkyFile_->resolver_;};
    if(!resolverCurrent())return false;at(0x129778);at(0x12977c);at(0x129780);at(0x129784);at(0x129788);
    a::OriginalStandardTextureDiagnostic50LA d;
    if(!sourceSkyFile_->readItemOffset(d)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    const auto& item=*sourceSkyFile_->firstItem_;at(0x12978c);at(0x129790);at(0x129794);
    if(item.offsetRead.items!=1u){at(0x129960);return false;}
    const auto& b=item.offsetRead.bytes;h.absoluteOffset=std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|(std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);
    at(0x129798);at(0x12979c);if(!resolverCurrent())return false;at(0x1297a0);at(0x1297a4);
    if(!sourceSkyFile_->tellItemStart(d)){at(d.sourcePC?d.sourcePC:0x12f520u);return false;}
    at(0x1297a8);at(0x1297ac);at(0x1297b0);if(!resolverCurrent())return false;at(0x1297b4);
    h.savedS5=*item.start<=0x7fffffffu?static_cast<std::int64_t>(*item.start):static_cast<std::int64_t>(*item.start)-0x100000000ll;
    at(0x1297b8);at(0x1297bc);at(0x1297c0);at(0x1297c4);
    if(!sourceSkyFile_->seekItemAbsolute(*h.absoluteOffset,d)){at(d.sourcePC?d.sourcePC:0x12f640u);return false;}
    at(0x1297c8);return continueSourceSkyItemLoop(pc,0);
}
bool Owner::continueSourceSkyError129960(std::uint32_t& pc,std::size_t failedItem)noexcept {
    if(pc!=0x129960u||sourceSkyError129960_.entered||failedItem>sourceSkyLaterItems_.size()||
       !preflightSourcePostParticleModelsNative()||!sourceSkyOwner_||!worldApplication_)return false;
    const auto& failed=sourceItemHistory(failedItem);
    if(!failed.failureNullReturned||!failed.slotStored||!sourceSkyItemSlots3C_||sourceSkyItemSlots3C_[failedItem])return false;
    auto& error=sourceSkyError129960_;auto& c=sourceSkyConstructor288A50_;auto& entity=sourceSkyOwner_->entity;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x129960);if(c.vtable08!=0x3d2ff0u)return false;
    error.entered=true;error.failedItem=failedItem;at(0x129964);at(0x129968);at(0x12996c);at(0x142590);
    at(0x1425a4);error.lockRead=true;if(worldApplication_->attachmentLock423F58())return false;
    //Fresh source sky is a genuinely unlinked leaf. Do not substitute a tree
    //cleanup for unsupported child/sibling/parent dispatches.
    at(0x1425b8);if(entity.firstChild18)return false;at(0x1425bc);at(0x1425c0);
    at(0x142604);if(entity.previous10)return false;at(0x142608);at(0x14260c);
    at(0x142618);if(entity.next14)return false;at(0x14261c);at(0x142620);
    at(0x14262c);if(entity.parent1C)return false;at(0x142630);at(0x142634);error.unlinkedLeafRead=true;
    at(0x142654);at(0x142658);at(0x104970);at(0x104978);if(entity.reference00!=1u)return false;
    //Withdraw native enrollment immediately before the genuine last-reference
    //store, not by pretending that source destruction or count retirement ended.
    if(!worldApplication_->entityAuthority_.retire(entity))return false;error.authorityRetired=true;
    at(0x10497c);at(0x104980);entity.reference00=0;error.referenceStored=true;
    at(0x104984);at(0x104988);at(0x10498c);at(0x104990);at(0x104994);at(0x104998);at(0x10499c);
    at(0x1049a0);at(0x1049a4);at(0x288bd0);error.derivedDestructorEntered=true;
    at(0x288be4);at(0x288be8);at(0x288bf4);c.vtable08=0x3d2ff0u;
    at(0x288bf8);at(0x288bfc);at(0x1293e0);at(0x1293fc);at(0x129400);
    at(0x12940c);c.vtable08=0x3cce60u;error.baseDestructorEntered=true;
    at(0x129410);at(0x129414);at(0x129418);
    //Constructor129280 never initializes7E8/968. This early item-parser failure
    //has not executed the later96-row producers. Native zero-initialized array
    //metadata is NOT evidence of sourceNULL, so stop before that unknown read.
    return false;
}
bool Owner::continueSourceSkyItemFailure128C8C(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(pc!=0x128c8cu||occurrence>sourceSkyLaterItems_.size()||!preflightSourcePostParticleModelsNative()||
       !sourceSkyFile_||!sourceSkyFile_->current())return false;
    auto& h=sourceItemHistory(occurrence);auto& owner=sourceItemOwner(occurrence);
    const auto* file=sourceSkyFile_->itemOccurrence(occurrence);
    if(!owner||!owner->storage||!file||file->receiver!=owner->storage.get()||file->nativeWithdrawn||
       !h.parserEntered||h.parserReturned||h.slotStored||h.registration.entered||h.failureReleaseEntered)return false;
    bool failed=file->header48.returned&&file->header48.items!=1u;
    for(const auto& field:file->fields)if(field.returned&&field.items!=1u)failed=true;
    if(!failed)return false; //A copied prefix or pending return is not a genuine source failure result.
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x128c8c);if(h.vtable08!=0x3cce40u||h.reference00!=1u)return false;h.failureReleaseEntered=true;
    at(0x128c90);at(0x128c94);at(0x128c98);at(0x104970);at(0x104978);at(0x10497c);
    //104970 is the actual item virtual+10, not a palette release receipt.
    at(0x104980);for(std::size_t k=0;k<8;++k)owner->storage[k]=0;h.reference00=0;h.failureReferenceStored=true;
    at(0x104984);at(0x104988);at(0x10498c);at(0x104990);at(0x104994);at(0x104998);at(0x10499c);
    at(0x1049a0);at(0x1049a4); //Actual derived virtual+8=1291B0, deleteflag1.
    at(0x1291b0);h.failureDestructorEntered=true;at(0x1291d4);at(0x1291d8);at(0x1291ec);
    //1291EC rewrites the same derived table. Fresh parser has not constructed
    //resource slots, but authenticate each actual reverse-order load anyway.
    for(std::size_t group=6;group;){--group;
        at(0x1291f0);at(0x1291f4);
        for(std::size_t entry=16;entry;){--entry;const auto index=group*16+entry,off=0x58+group*0x100+entry*4;
            at(0x1291f8);if(owner->resources58[index])return false;
            for(std::size_t k=0;k<4;++k)if(!owner->known[off+k]||owner->storage[off+k])return false;
            ++h.failureNullSlotsRead;at(0x1291fc);at(0x129200);at(0x12921c);at(0x129220);
        }
        at(0x129224);at(0x129228);at(0x12922c);
    }
    at(0x129230);at(0x129234);at(0x129238);at(0x104910);at(0x104920);at(0x104924);at(0x104938);
    at(0x10493c);h.vtable08=0x3cc980u;for(std::size_t k=0;k<4;++k)owner->storage[8+k]=static_cast<std::uint8_t>(*h.vtable08>>(8*k));
    at(0x104948);at(0x10494c);at(0x104954);at(0x104958);h.failureBaseReturned=true;
    at(0x12923c);at(0x129240);at(0x129244);at(0x129248);at(0x12924c);at(0x129250);
    at(0x2e8b00);
    if(!a::OriginalConfigProcessGlobals50LA::descriptorDelete2E8B00AdmittedNative()||
       !sourceSkyFile_->releaseItemParserSource(owner->storage.get(),occurrence))return false;
    //Actual source delete after completed derived/base destruction. Keep native
    //history wrapper, but withdraw its file alias BEFORE releasing the backing.
    owner->storage.reset();h.failureSourceDeleted=true;
    at(0x129254);for(std::uint32_t p=0x129258;p<=0x129278;p+=4)at(p);
    at(0x1049a8);at(0x1049ac);at(0x1049b0);at(0x1049b4);
    at(0x128c9c);for(std::uint32_t p=0x128ca0;p<=0x128cc0;p+=4)at(p);
    h.failureNullReturned=true;h.parserReturned=true;
    at(0x1297f8);at(0x1297fc);at(0x129800);
    if(!sourceSkyPublishedItems3C_||!*sourceSkyPublishedItems3C_||!preflightSourceSkyItemsNative())return false;
    sourceSkyItemSlots3C_[occurrence]=nullptr;h.slotStored=true;
    at(0x129804);at(0x129808);at(0x12980c);at(0x129810);at(0x129814);
    at(0x129960);return continueSourceSkyError129960(pc,occurrence); //Actual caller error path, never item-loop advance.
}
bool Owner::continueSourceSkyItemBody(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(pc!=0x1297c8u||occurrence>sourceSkyLaterItems_.size()||!sourceSkyFile_||
       occurrence!=sourceSkyFile_->followingItems_.size()||!sourceSkyFile_->itemOccurrence(occurrence)||
       !sourceSkyFile_->itemOccurrence(occurrence)->seekReturned||!sourceSkyItems129758_.count7D8||
       occurrence>=*sourceSkyItems129758_.count7D8||!preflightSourcePostParticleModelsNative())return false;
    auto& h=sourceItemHistory(occurrence);auto& owner=sourceItemOwner(occurrence);if(h.itemAllocationAttempted)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    const auto resolverCurrent=[&](){return sourceTrackMountDomain_&&sourceTrackMountDomain_->resolver3E1A70()==sourceSkyFile_->resolver_;};
    a::OriginalStandardTextureDiagnostic50LA d;
    at(0x1297c8);at(0x1297cc);at(0x1297d0);at(0x1297d4);h.itemAllocationAttempted=true;
    try{owner.reset(new SourceSkyItemOwner);}catch(...){return false;}
    try{owner->storage.reset(new std::uint8_t[0x660]);}catch(...){owner.reset();}
    h.itemAllocationReturned=true;h.itemAllocationNonNull=bool(owner);at(0x1297d8);at(0x1297dc);
    if(h.itemAllocationNonNull){
        at(0x1297e0);at(0x1297e4);at(0x128930);h.constructorEntered=true;
        auto* bytes=owner->storage.get();auto& known=owner->known;
        const auto store=[&](std::size_t off,std::uint64_t value,std::size_t size){for(std::size_t k=0;k<size;++k){bytes[off+k]=static_cast<std::uint8_t>(value>>(k*8));known[off+k]=true;}};
        const auto vf0=[&](std::size_t off){store(off,0,8);store(off+8,0,4);store(off+12,0x3f800000u,4);};
        at(0x12893c);at(0x128940);at(0x1048f8);store(8,0x3cc980u,4);at(0x104900);store(0,1,8);
        h.reference00=1;h.vtable08=0x3cc980u;h.baseReturned=true;at(0x104904);at(0x104908);
        at(0x128950);store(8,0x3cce40u,4);h.vtable08=0x3cce40u;
        at(0x12895c);vf0(0x10);at(0x128968);vf0(0x20);at(0x128974);vf0(0x30);
        at(0x128990);at(0x128994);at(0x2e8860);
        for(std::size_t j=0;j<6;++j){at(0x2e88cc);at(0x2e88d0);at(0x1289b0);at(0x1289b4);vf0(0x140+j*0x100);at(0x1289b8);at(0x1289bc);++h.callbacks;}
        at(0x128998);at(0x12899c);at(0x128880);at(0x128884);store(0x650,0,4);at(0x128888);store(0x40,~std::uint64_t(0),8);
        at(0x12888c);store(0x48,0,4);at(0x128890);store(0x4c,0,2);at(0x128898);store(0x4e,0xffff,2);
        if(!worldApplication_){at(0x1288a0);return false;}h.origin3E0900=worldApplication_->skyOrigin3E0900_;
        for(std::size_t j=0;j<3;++j){at(0x1288a8+static_cast<std::uint32_t>(j)*12u);for(std::size_t k=0;k<4;++k)store(0x10+j*16+k*4,(*h.origin3E0900)[k],4);}
        for(std::size_t j=0;j<6;++j){const auto off=j*0x100;at(0x1288c4);store(off+0x50,0,4);at(0x1288c8);store(off+0x54,0,2);at(0x1288cc);store(off+0x56,0,2);
            for(std::size_t k=0;k<16;++k){at(0x1288e0);store(off+0x58+k*4,0,4);at(0x1288e4);store(off+0x98+k*4,0,4);at(0x1288e8);store(off+0xd8+k*2,0xffff,2);at(0x1288f0);store(off+0xf8+k*4,0xffffffffu,4);}
            at(0x128904);store(off+0x140,0,4);at(0x12890c);store(off+0x144,0,4);at(0x128910);store(off+0x148,0,4);++h.defaultGroups;
        }
        at(0x128920);at(0x128924);at(0x1289a0);at(0x1289a4);at(0x1289a8);at(0x1289ac);h.constructorReturned=true;at(0x1297e8);
    }
    at(0x1297ec);at(0x1297f0);at(0x1297f4);h.parserModeA2=0;at(0x1289c0);h.parserEntered=true;at(0x1289e0);
    if(!owner)return false; //Original NULL receiver reaches this first SD, not a fabricated loader result.
    if(!sourceSkyFile_->beginItemParser(owner->storage.get(),occurrence))return false;
    h.handle40=static_cast<std::uint64_t>(*sourceSkyFile_->sourceHandle_);
    for(std::size_t k=0;k<8;++k){owner->storage[0x40+k]=static_cast<std::uint8_t>(*h.handle40>>(k*8));owner->known[0x40+k]=true;}h.handleStored=true;
    at(0x1289e4);at(0x1289e8);at(0x1289ec);if(!resolverCurrent())return false;at(0x1289f0);at(0x1289f4);at(0x1289f8);at(0x1289fc);at(0x128a00);at(0x128a04);h.headerAttempted=true;
    const bool returned=sourceSkyFile_->readItemHeader48(d,occurrence);const auto& read=sourceSkyFile_->itemOccurrence(occurrence)->header48;
    if(read.copied)std::memcpy(owner->storage.get()+0x48,read.bytes.data(),read.copied);h.headerCopied=read.copied;
    if(!returned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    h.headerReturned=true;h.headerItems=read.items;at(0x128a08);at(0x128a0c);at(0x128a10);
    if(read.items!=1u){at(0x128c8c);return continueSourceSkyItemFailure128C8C(pc,occurrence);}
    at(0x128a14);
    //Actual stack flags, receiver4E, six halfwords, six words, then three
    //XYZ reads. No flags-to4C store and no replacement of vector W.
    for(std::size_t i=0;i<17;++i){const auto spec=rr::assets::OriginalSourceSkyFile50LA::itemFieldSpec(i);
        if(!resolverCurrent()||!preflightSourceSkyItemsNative())return false;
        at(spec.call);const bool fieldReturned=sourceSkyFile_->readItemField(i,d,occurrence);
        const auto& field=sourceSkyFile_->itemOccurrence(occurrence)->fields[i];
        if(i&&field.copied){std::memcpy(owner->storage.get()+spec.destination,field.bytes.data(),field.copied);
            for(std::size_t k=0;k<field.copied;++k)owner->known[spec.destination+k]=true;}
        if(!fieldReturned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
        at(spec.resume);if(field.items!=1u){at(0x128c8c);return continueSourceSkyItemFailure128C8C(pc,occurrence);}
        if(i>=2&&i<8){at(0x128a8c);at(0x128a90);at(0x128a94);at(0x128a98);}
        if(i>=8&&i<14){at(0x128acc);at(0x128ad0);at(0x128ad4);at(0x128ad8);}
    }
    at(0x128b48);
    const auto& flagsRead=sourceSkyFile_->itemOccurrence(occurrence)->fields[0];
    if(flagsRead.items!=1u||flagsRead.copied!=2u)return false;
    const auto flags=std::uint32_t(flagsRead.bytes[0])|(std::uint32_t(flagsRead.bytes[1])<<8);
    at(0x128b4c);at(0x128b50);at(0x128b54);
    const auto optionalRead=[&](std::size_t index){const auto spec=rr::assets::OriginalSourceSkyFile50LA::itemFieldSpec(index);
        if(!resolverCurrent()||!preflightSourceSkyItemsNative())return false;
        at(spec.call);const bool returned=sourceSkyFile_->readItemField(index,d,occurrence);
        const auto& field=sourceSkyFile_->itemOccurrence(occurrence)->fields[index];
        if(field.copied){std::memcpy(owner->storage.get()+spec.destination,field.bytes.data(),field.copied);
            for(std::size_t k=0;k<field.copied;++k)owner->known[spec.destination+k]=true;}
        if(!returned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
        at(spec.resume);if(field.items!=1u){at(0x128c8c);return continueSourceSkyItemFailure128C8C(pc,occurrence);}return true;
    };
    if(flags&4u){at(0x128b58);at(0x128b5c);
        for(std::size_t group=0;group<6;++group){if(!optionalRead(17+group))return false;
            at(0x128b88);at(0x128b8c);at(0x128b90);at(0x128b94);}
    }
    at(0x128b98);at(0x128b9c);at(0x128ba0);at(0x128ba4);
    if(flags&1u){at(0x128ba8);at(0x128bac);
        for(std::size_t group=0;group<6;++group){at(0x128bb0);at(0x128bb4);
            for(std::size_t entry=0;entry<16;++entry){if(!optionalRead(23+group*16+entry))return false;
                at(0x128be0);at(0x128be4);at(0x128be8);at(0x128bec);}
            at(0x128bf0);at(0x128bf4);at(0x128bf8);at(0x128bfc);
        }
    }
    at(0x128c00);
    if(!resolverCurrent()||!preflightSourceSkyItemsNative())return false;
    if(!sourceSkyFile_->finishItemReads(d,occurrence)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    const auto& tail=*sourceSkyFile_->itemOccurrence(occurrence);at(0x128c74);at(0x128c78);at(0x128c7c);
    if(!tail.finalPosition||*tail.finalPosition>0xffffffffu||h.parserModeA2!=0u)return false;
    at(0x128c80);h.dataPosition650=static_cast<std::uint32_t>(*tail.finalPosition);
    for(std::size_t k=0;k<4;++k){owner->storage[0x650+k]=static_cast<std::uint8_t>(*h.dataPosition650>>(k*8));owner->known[0x650+k]=true;}
    at(0x128c84);at(0x128c88);at(0x128ca0);at(0x128cbc);at(0x128cc0);h.parserReturned=true;
    //Return is this fresh item, never a prepared resource. NULL array is
    //bounded at the actual store after parser return, not treated as success.
    at(0x1297f8);at(0x1297fc);at(0x129800);
    if(!sourceSkyPublishedItems3C_||!*sourceSkyPublishedItems3C_||!preflightSourceSkyItemsNative())return false;
    sourceSkyItemSlots3C_[occurrence]=owner.get();h.slotStored=true;
    at(0x129804);at(0x129808);at(0x12980c);at(0x129810);at(0x129814);
    at(0x129818);at(0x12982c);return false; //The iterative parent driver owns the next source restore.
}
bool Owner::continueSourceSkyItemRestore(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(occurrence>sourceSkyLaterItems_.size())return false;
    auto& h=sourceItemLoop(occurrence);const auto& state=sourceItemHistory(occurrence);
    if(pc!=0x12982cu||!state.slotStored||h.restoreAttempted||!preflightSourcePostParticleModelsNative()||!sourceSkyFile_||!sourceSkyOwner_)return false;
    const auto at=[&](std::uint32_t v){pc=sourcePostParticleModelsPC_=v;};
    const auto resolverCurrent=[&](){return sourceTrackMountDomain_&&sourceTrackMountDomain_->resolver3E1A70()==sourceSkyFile_->resolver_;};
    rr::assets::OriginalStandardTextureDiagnostic50LA d;at(0x12981c);if(!resolverCurrent())return false;
    at(0x12982c);at(0x129830);h.restoreAttempted=true;
    const bool returned=sourceSkyFile_->restoreItemStart(d,occurrence);
    h.restoreAttempted=sourceSkyFile_->itemOccurrence(occurrence)->restoreAttempted;h.restoreReturned=sourceSkyFile_->itemOccurrence(occurrence)->restoreReturned;
    if(!returned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    at(0x129834);at(0x129838);if(sourceSkyFile_->itemOccurrence(occurrence)->restoreResult!=0){at(0x129960);return false;}
    at(0x12983c);h.advancedOffsetS2=static_cast<std::uint32_t>((occurrence+1u)*4u);at(0x129840);h.advancedIndexS4=static_cast<std::uint32_t>(occurrence+1u);at(0x129844);at(0x129848);
    if(!preflightSourcePostParticleModelsNative())return false;h.loopCount7D8=sourceSkyOwner_->count7D8;
    at(0x12984c);at(0x129850);at(0x129854);
    if(*h.loopCount7D8<=occurrence+1u){at(0x129858);return continueSourceSkyPostItems(pc);}
    at(0x129770);h.nextDirectoryEntered=true;at(0x129774);if(!resolverCurrent())return false;
    if(!sourceSkyFile_->readItemOffset(d,occurrence+1)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    const auto& next=*sourceSkyFile_->itemOccurrence(occurrence+1);at(0x12978c);at(0x129790);
    if(next.offsetRead.items!=1u){at(0x129960);return false;}
    const auto& b=next.offsetRead.bytes;h.nextAbsoluteOffset=std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|(std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);
    at(0x12979c);if(!resolverCurrent())return false;at(0x1297a0);if(!sourceSkyFile_->tellItemStart(d,occurrence+1)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    at(0x1297b0);if(!resolverCurrent())return false;at(0x1297b4);
    h.nextSavedS5=*next.start<=0x7fffffffu?static_cast<std::int64_t>(*next.start):static_cast<std::int64_t>(*next.start)-0x100000000ll;
    at(0x1297c0);if(!sourceSkyFile_->seekItemAbsolute(*h.nextAbsoluteOffset,d,occurrence+1)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    at(0x1297c8);
    return false; //Next own allocation is performed by the iterative driver.
}
bool Owner::sourceSkyFormat29E510(std::uint32_t format)noexcept {
    return (format>=5&&format<=12)||(format>=20&&format<=27)||(format>=31&&format<=36);
}
std::uint32_t Owner::sourceSkyFormat29E480(std::uint32_t format)noexcept {
    //Original29E480 jump table3BE230; unsigned out-of-range returns zero.
    static constexpr std::array<std::uint32_t,37> bytes{{0,3,2,2,1,2,1,2,2,3,2,2,4,2,2,4,3,2,2,1,2,1,2,2,3,2,2,4,2,2,4,3,3,2,2,4,4}};
    return format<bytes.size()?bytes[format]:0u;
}
std::uint32_t Owner::sourceSkyFormat29E640(std::uint32_t format)noexcept {
    static constexpr std::uint32_t formats[37]={0,0x378,0x22b,0x235,8,0x22b,8,0x22b,0x235,0x378,0x613,0x115c,0x22b8,
            0x613,0x115c,0x22b8,0x378,0x22b,0x235,8,0x22b,8,0x22b,0x235,0x378,0x613,0x115c,0x22b8,
            0x613,0x115c,0x22b8,0x378,0x378,0x613,0x613,0x22b8,0x22b8};
    return format<37?formats[format]:0;
}
std::uint32_t Owner::sourceSkyFormatBits29E910(std::uint32_t format)noexcept {
    switch(format){case 8:return 8;case 0x115c:case 0x613:case 0x22b:case 0x235:return 16;
        case 0x378:return 24;case 0x22b8:return 32;default:return 0;}
}
std::uint32_t Owner::sourceSkyFileBits29E5B0(std::uint32_t format)noexcept {
    static constexpr std::uint32_t bits[37]={0,24,16,16,8,16,8,16,16,24,16,16,32,16,16,32,24,16,16,8,16,8,16,16,24,16,16,32,16,16,32,24,24,16,16,32,32};
    return format<37?bits[format]:0;
}
bool Owner::preflightSourceSkyResourcePrefix(std::size_t occurrence)const noexcept {
    const auto& reg=sourceItemHistory(occurrence).registration;
    if(reg.completedResources.size()>96||reg.resourceCalls.size()>95)return false;
    for(std::size_t ordinal=0;ordinal<reg.completedResources.size();++ordinal){const auto& completed=reg.completedResources[ordinal];
        if(!completed||completed->fileOrdinal!=ordinal||!preflightSourceSkyResourceOccurrence(occurrence,*completed,true))return false;}
    for(std::size_t n=0;n<reg.resourceCalls.size();++n){const auto& pointer=reg.resourceCalls[n];if(!pointer)return false;const auto& call=*pointer;const auto ordinal=n+1;
        if(!call.entered||call.index>=96||!reg.masks[call.index]||!(*reg.masks[call.index]&(1u<<(call.index%16)))||
           (n&&reg.resourceCalls[n-1]->index>=call.index))return false;
        const SourceSkyResourcePrefix* history=ordinal<reg.completedResources.size()?reg.completedResources[ordinal].get():ordinal==reg.completedResources.size()?reg.pendingResource.get():nullptr;
        if(history&&(!call.bodyEntered||history->group*16+history->entry!=call.index||history->fileOrdinal!=ordinal))return false;
        if(call.bodyEntered){const auto* file=sourceSkyFile_?sourceSkyFile_->resourceFileOccurrence(occurrence,ordinal):nullptr;
            if(!file||file->entryCursor!=call.cursor)return false;}
        else {if(n+1!=reg.resourceCalls.size()||history||reg.completedResources.size()!=ordinal||reg.completedEntries!=call.index)return false;
            if(!reg.nativeWithdrawn){const auto& owner=sourceItemOwner(occurrence);
                if(!owner||owner->resources58[call.index]||!sourceSkyFile_||sourceSkyFile_->expectedCursor_!=call.cursor)return false;}}
    }
    if(reg.pendingResource&&reg.pendingResource->fileOrdinal&&reg.resourceCalls.size()!=reg.pendingResource->fileOrdinal)return false;
    if(reg.resourceScanExhausted&&(reg.completedEntries!=96||reg.pendingResource))return false;
    return !reg.pendingResource||preflightSourceSkyResourceOccurrence(occurrence,*reg.pendingResource,false);
}
bool Owner::preflightSourceSkyResourceOccurrence(std::size_t occurrence,const SourceSkyResourcePrefix& h,bool completed)const noexcept {
    const auto& state=sourceItemHistory(occurrence);const auto& reg=state.registration;
    if((completed?(h.fileOrdinal>=reg.completedResources.size()||reg.completedResources[h.fileOrdinal].get()!=&h):h.fileOrdinal!=reg.completedResources.size())||!sourceSkyFile_||!sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal))return false;
    if(!completed&&sourceSkyFile_->activeResourceFile(occurrence)!=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal))return false;
    if(!h.entered||!reg.seekReturned||h.group>=6||h.entry>=16||(completed?(!h.resourceSourceReturned||h.resourceSourceResult!=1u||reg.completedEntries<=h.group*16+h.entry):reg.completedEntries!=h.group*16+h.entry)||
       !reg.masks[h.group*16+h.entry]||!(*reg.masks[h.group*16+h.entry]&(1u<<h.entry))||
       (h.side&&(!h.sizeCode||*h.side!=(*h.sizeCode==0?512u:*h.sizeCode==1?64u:*h.sizeCode==2?128u:256u)))||
       (h.format48&&h.compressed!=sourceSkyFormat29E510(*h.format48))||
       (h.bytesPerPixel&&(!h.format48||h.compressed||*h.bytesPerPixel!=sourceSkyFormat29E480(*h.format48)))||
       (h.payloadBytes&&!h.compressed&&(!h.side||!h.bytesPerPixel||*h.payloadBytes!=(*h.side)*(*h.side)*(*h.bytesPerPixel)))||
       (h.sizeReadAttempted&&!h.compressed)||(h.sizeReadReturned&&!h.sizeReadAttempted)||h.sizeCopied>4||
       (h.allocationAttempted&&(!h.side||!h.format48||(h.compressed&&!h.sizeReadReturned)||(!h.compressed&&!h.payloadBytes)))||
       (h.allocationReturned&&!h.allocationAttempted)||(h.allocationNonNull&&!h.allocationReturned)||
       (h.constructorEntered&&!h.allocationNonNull)||(h.baseReferenceAcquired&&(!h.constructorEntered||h.reference00!=1u))||
       (h.nativeReferenceRetired&&(!h.nativeWithdrawn||!h.baseReferenceAcquired)))return false;
    if(h.sizeReadAttempted){if(!sourceSkyFile_||occurrence>=sourceSkyFile_->itemResourceSeeks_.size())return false;
        const auto& read=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->resourceSizeA8;
        if(read.attempted!=h.sizeReadAttempted||read.returned!=h.sizeReadReturned||read.copied!=h.sizeCopied||read.items!=h.sizeReadItems)return false;
        if(h.payloadBytes){if(read.copied!=4||*h.payloadBytes!=(std::uint32_t(read.bytes[0])|(std::uint32_t(read.bytes[1])<<8)|
           (std::uint32_t(read.bytes[2])<<16)|(std::uint32_t(read.bytes[3])<<24)))return false;}
    }
    if(h.compressedStackInitialized){
        if(!h.compressed||!h.loaderClassified||!sourceSkyFile_||occurrence>=sourceSkyFile_->itemResourceSeeks_.size())return false;
        const auto& reads=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->compressedHeaders;
        std::array<std::optional<std::uint32_t>,5> words;for(std::size_t i=1;i<5;++i)words[i]=0;
        for(std::size_t i=0;i<5;++i){const auto& read=reads[i];
            if(read.attempted!=h.compressedHeaderAttempted[i]||read.returned!=h.compressedHeaderReturned[i]||
               read.copied!=h.compressedHeaderCopied[i]||read.items!=h.compressedHeaderItems[i])return false;
            if(i||read.copied==4){std::uint32_t value=0;
                for(std::size_t k=0;k<read.copied;++k)value|=std::uint32_t(read.bytes[k])<<(k*8);words[i]=value;}
        }
        if(h.compressedHeaderConverted){if(!words[0]||words[3]==0x1000u)return false;
            if(*words[0]==31u||*words[0]==32u)words[0]=31u;
            else if(*words[0]==33u||*words[0]==34u)words[0]=32u;
        }
        if(h.compressedB8Replaced){for(bool returned:h.compressedHeaderReturned)if(!returned)return false;words[3]=words[2];}
        if(words!=h.compressedHeaderWords)return false;
    }
    if(!h.nativeWithdrawn&&h.compressedLoaderEntered&&(!h.compressedB8Replaced||!h.payloadBytes||h.rawLoaderEntered||
        h.loaderReceiver!=h.texture.get()||h.loaderPaletteBorrow!=reg.paletteBorrow||h.loaderHandle40!=state.handle40||
        h.compressedPendingReturnPC!=0x128fd0u||h.compressedInputFormat!=32u||h.compressedInputLayout!=0x613u||h.rawStackScalars!=std::array<std::uint64_t,6>{{0,0,2,1,128,0x00ff00ffu}}))return false;
    if(h.nativeWithdrawn){if(h.loaderBufferManager)return false;
        for(const auto& call:h.loaderBuffers)if(call.entered&&(!call.withdrawn||call.borrowed))return false;}
    if(h.nativeWithdrawn&&(h.compressedLiveOffset118||!h.compressedBacking.nativeWithdrawn||h.compressedBacking.allocation||h.compressedBacking.aligned))return false;
    if(h.nativeWithdrawn)for(const auto& next:h.followingCompressedLevels)
        if(!next||next->liveOffset118||!next->backing.nativeWithdrawn||next->backing.allocation||next->backing.aligned)return false;
    if(h.nativeWithdrawn&&h.embedded&&(!h.embedded->nativeWithdrawn||h.embedded->object20||h.embedded->alignedBacking||h.embedded->data00||h.embedded->cursor04))return false;
    if(h.nativeWithdrawn)return !h.postLoaderReceiver&&!h.embeddedFCPublished&&!h.resourcePaletteB4&&!h.payloadVirtualBorrow&&!h.loaderReceiver&&!h.loaderPaletteBorrow&&(!h.slot58Stored||h.slot58WithdrawnNative)&&(!h.texture||(!h.texture->storage_&&!h.texture->manager_&&!h.texture->registered_&&
        h.texture->nativeDetached_&&h.texture->topologyCurrentNative()))&&(!h.baseReferenceAcquired||h.nativeReferenceRetired);
    if(h.texture&&!h.texture->topologyCurrentNative())return false;
    if(h.managerRead){if(!h.texture||!sourceLoadApplication_||sourceLoadApplication_!=sourceTrackMountApplication_||
        sourceLoadApplication_->entities()!=worldApplication_||h.texture->manager_!=sourceLoadApplication_->textureLifetimeOwner_||
        h.texture->issuer_!=sourceLoadApplication_||!h.texture->topologyCurrentNative())return false;}
    if(h.registrationReturned&&(!h.managerRead||!h.texture||!h.texture->registered_))return false;
    if(h.registrationReturned)for(std::size_t k=0;k<8;++k)if(h.texture->known_[0xa0+k])return false;
    if(h.base29C6E0Returned&&!h.registrationReturned)return false;
    if((h.loaderGuardReturned&&!h.rawLoaderEntered&&!h.compressedLoaderEntered)||(h.loaderFrameInitialized&&!h.loaderGuardReturned)||
       (h.loaderFormat3C&&(!h.loaderFrameInitialized||!h.loaderFormat48||*h.loaderFormat3C!=sourceSkyFormat29E640(h.compressedLoaderEntered?h.compressedInputFormat.value_or(0):*h.loaderFormat48)))||
       (h.loaderFlagsStored&&(!h.loaderFormat3C||
        h.loaderF8KnownZero!=(h.compressedLoaderEntered?14:11)||h.loaderF8KnownOne!=(h.compressedLoaderEntered?1:4)||
        h.loaderEffectiveFormat!=(h.compressedLoaderEntered?34u:h.loaderFormat48.value_or(0))||
        h.loaderEffectiveLayout!=(h.compressedLoaderEntered?0x613u:h.rawFormat29E640.value_or(0))))||
       (h.loaderWorkspaceCleared&&!h.loaderFlagsStored)||
       (h.loaderE8&&(!h.loaderWorkspaceCleared||!h.loaderEffectiveLayout||*h.loaderE8!=sourceSkyFormatBits29E910(*h.loaderEffectiveLayout)))||
       (h.loaderMaximumBits&&(!h.loaderE8||*h.loaderMaximumBits!=*h.loaderE8)))return false;
    if(h.compressedDimensionsStored&&(!h.compressedLoaderEntered||!h.loaderE8||*h.loaderE8!=16||h.loaderBufferManager||h.payloadVirtualEntered||h.payloadBranchSelected))return false;
    if(h.compressedLoopInitialized){
        if(!h.compressedDimensionsStored||!h.compressedLoopScale||!h.compressedLoopIndex||!sourceSkyFile_||
           occurrence>=sourceSkyFile_->itemResourceSeeks_.size())return false;
        const auto& file=*sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal);
        std::array<std::uint8_t,0x30> workspace{};std::uint32_t count=0,scale=16;bool terminalReturned=false,shortExit=false;
        for(std::size_t i=0;i<12;++i){const auto& step=file.compressedOffsets[i];const auto& read=step.read;
            if(step.tell.attempted&&(!h.side||step.terminal!=(*h.side<scale)))return false;
            for(std::size_t k=0;k<read.copied;++k)workspace[i*4+k]=read.bytes[k];
            if(read.returned){if(read.items==1u){for(std::size_t k=0;k<4;++k)workspace[i*4+k]=0;
                    if(step.terminal)terminalReturned=true;else {++count;scale*=2;}}
                else shortExit=true;}
        }
        if(h.loaderWorkspace170!=workspace||h.compressedLoopIndex!=count||h.compressedLoopScale!=scale||
           h.compressedOffsetsReturned!=terminalReturned||h.compressedOffsetShortExit!=shortExit)return false;
    }
    if(h.compressedCountAdjusted&&(!h.compressedOffsetsReturned||h.rawStackScalars[0]!=0))return false;
    if(h.compressedLevelSetup){
        if(!h.compressedCountAdjusted||!h.compressedLoopIndex||!*h.compressedLoopIndex||
           h.compressedLevelIndex!=*h.compressedLoopIndex-1||!h.compressedPayloadSize16C||!sourceSkyFile_)return false;
        const auto& read=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->compressedPayloadSize;
        std::uint32_t size=0;for(std::size_t k=0;k<read.copied;++k)size|=std::uint32_t(read.bytes[k])<<(k*8);
        if(*h.compressedPayloadSize16C!=size)return false;
        if(h.compressedLevelSizeStored&&(!read.returned||*h.compressedLevelIndex>=5))return false;
    }
    const auto backingCurrent=[](const SourceSkyCompressedBacking& backing,bool sizeStored,std::optional<std::uint32_t> size){
        if((backing.attempted&&!sizeStored)||(backing.returned&&!backing.attempted)||
           (backing.published&&!backing.returned)||backing.nativeWithdrawn)return false;
        if(backing.attempted){if(!size)return false;
            const auto rounded=(std::uint64_t(*size)+19u)&~std::uint64_t(15);
            if(backing.capacity!=(rounded<16?16:rounded))return false;}
        if(backing.aligned){if(!backing.allocation||!backing.returned)return false;
            const auto first=reinterpret_cast<std::uintptr_t>(backing.allocation.get()),aligned=reinterpret_cast<std::uintptr_t>(backing.aligned);
            if(aligned<first||aligned-first>=64||aligned%64)return false;
        }else if(backing.allocation)return false;
        return true;
    };
    const auto& backing=h.compressedBacking;
    if(!backingCurrent(backing,h.compressedLevelSizeStored,h.compressedPayloadSize16C))return false;
    if(h.compressedCounterRead&&!backing.published)return false;
    if(h.compressedCounterRead&&!h.compressedCounterStored){std::uint32_t value=0;
        if(!a::OriginalConfigProcessGlobals50LA::readCompressedBytes24F7F0(value)||value!=h.compressedCounterBefore)return false;}
    if(h.compressedCounterStored&&(!h.compressedCounterRead||!backing.published||!h.compressedPayloadSize16C||
       !a::OriginalConfigProcessGlobals50LA::compressedBytesReceiptCurrentNative(*h.compressedPayloadSize16C,
        h.compressedCounterBefore,h.compressedCounterAfter,h.compressedCounterSequence)))return false;
    if(h.compressedCounterStored){const auto& read=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->compressedPayload;
        if(read.attempted&&(read.requested!=h.compressedPayloadSize16C||read.destination!=backing.aligned))return false;
        if(h.compressedKnownPrefix!=read.copied||h.compressedKnownPrefix>backing.capacity||
           h.compressedTransferShort!=(read.returned&&read.items!=h.compressedPayloadSize16C))return false;
        if(h.compressedLevelDimensionsStored&&(!read.returned||read.items!=h.compressedPayloadSize16C))return false;
    }else if(h.compressedKnownPrefix||h.compressedTransferShort||h.compressedLevelDimensionsStored)return false;
    if((h.compressedLogReturned[0]&&!h.compressedLevelDimensionsStored)||
       (h.compressedLogReturned[1]&&!h.compressedLogReturned[0])||
       (h.compressedMirrorDimensionsStored&&!h.compressedLogReturned[1])||
       (h.compressedHeaderSizeAdjusted&&!h.compressedMirrorDimensionsStored)||
       (h.compressedBaseAliasStored&&!h.compressedHeaderSizeAdjusted)||
       (h.compressedOffsetAliasStored&&!h.compressedBaseAliasStored)||
       h.compressedLiveOffset118!=(h.compressedOffsetAliasStored?16u:0u)||
       (h.compressedOffsetAliasStored&&backing.aligned&&backing.capacity<16))return false;
    if(h.compressedLoopAdvanced&&(!h.compressedOffsetAliasStored||h.compressedNextDimension!=32u||
       h.compressedNextIteration!=1u||!h.side||h.compressedNextLoopTaken!=(*h.side>=32u)))return false;
    if(h.followingCompressedLevels.size()>4||(!h.followingCompressedLevels.empty()&&!h.compressedNextLoopTaken))return false;
    if(sourceSkyFile_&&occurrence<sourceSkyFile_->itemResourceSeeks_.size()&&
       sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->followingCompressedSizes.size()>h.followingCompressedLevels.size())return false;
    for(std::size_t n=0;n<h.followingCompressedLevels.size();++n){
        if(!h.followingCompressedLevels[n])return false;
        const auto& next=*h.followingCompressedLevels[n];
        if(!next.selected||next.ordinal!=n+1||next.dimension!=(32u<<n)||!h.compressedLevelIndex||*h.compressedLevelIndex<next.ordinal||next.index!=*h.compressedLevelIndex-next.ordinal)return false;
        if(n){const auto& prior=*h.followingCompressedLevels[n-1];if(!prior.loopAdvanced||!prior.nextLoopTaken)return false;}
        if(next.loopAdvanced&&(!next.offsetAliasStored||next.nextDimension!=next.dimension*2u||next.nextOrdinal!=next.ordinal+1||!h.side||next.nextLoopTaken!=(*h.side>=next.nextDimension)))return false;
        const auto& reads=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->followingCompressedSizes;
        if(reads.size()<=n){if(next.sizeReadAttempted||next.sizeReadReturned||next.size16C)return false;}
        else {const auto& read=*reads[n];std::uint32_t value=0;
            for(std::size_t k=0;k<read.copied;++k)value|=std::uint32_t(read.bytes[k])<<(k*8);
            if(next.sizeReadAttempted!=read.attempted||next.sizeReadReturned!=read.returned||next.size16C!=value)return false;}
        if(next.sizeStored&&(!next.sizeReadReturned||next.index>=5))return false;
        if(!backingCurrent(next.backing,next.sizeStored,next.size16C))return false;
        if(next.counterRead&&!next.backing.published)return false;
        if(next.counterRead&&!next.counterStored){std::uint32_t value=0;
            if(!a::OriginalConfigProcessGlobals50LA::readCompressedBytes24F7F0(value)||value!=next.counterBefore)return false;}
        if(next.counterStored&&(!next.counterRead||!a::OriginalConfigProcessGlobals50LA::compressedBytesReceiptCurrentNative(next.size16C,next.counterBefore,next.counterAfter,next.counterSequence)))return false;
        const auto& payloads=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->followingCompressedPayloads;
        if(payloads.size()<=n){if(next.knownPrefix||next.transferShort||next.transferAccepted)return false;}
        else {const auto& read=*payloads[n];
            if(!next.counterStored||(read.attempted&&(read.requested!=next.size16C||read.destination!=next.backing.aligned))||
               next.knownPrefix!=read.copied||next.knownPrefix>next.backing.capacity||
               next.transferShort!=(read.returned&&read.items!=next.size16C)||
               next.transferAccepted!=(read.returned&&read.items==next.size16C))return false;}
        if((next.dimensionsStored&&!next.transferAccepted)||(next.logReturned[0]&&!next.dimensionsStored)||
           (next.logReturned[1]&&!next.logReturned[0])||(next.mirrorDimensionsStored&&!next.logReturned[1])||
           (next.headerSizeAdjusted&&!next.mirrorDimensionsStored)||(next.baseAliasStored&&!next.headerSizeAdjusted)||
           (next.offsetAliasStored&&!next.baseAliasStored)||next.liveOffset118!=(next.offsetAliasStored?16u:0u)||
           (next.offsetAliasStored&&next.backing.aligned&&next.backing.capacity<16))return false;
    }
    if(h.loaderStack140&&(*h.loaderStack140!=0u||!h.loaderFrameInitialized))return false;
    if(h.compressedTailEntered){
        if(h.loaderStack140!=0u||!h.compressedLoopAdvanced||!h.compressedOffsetAliasStored)return false;
        if(h.followingCompressedLevels.empty()){if(h.compressedNextLoopTaken)return false;}
        else {const auto& last=*h.followingCompressedLevels.back();if(!last.loopAdvanced||last.nextLoopTaken)return false;}
    }
    if((h.compressedTailCountStored&&!h.compressedTailEntered)||(h.compressedTailFormatStored&&(!h.compressedTailCountStored||h.compressedTailFormat!=0x613u))||
       (h.compressedFinalTellReturned&&!h.compressedTailFormatStored)||(h.compressedLoaderResult&&(!h.embeddedCallReturned||*h.compressedLoaderResult!=1u))||
       (h.compressedLoaderReturned&&(!h.compressedLoaderResult||!h.compressedFinalTellReturned)))return false;
    if(h.compressedTailEntered){const auto& tell=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->compressedFinalTell;
        if(tell.returned!=h.compressedFinalTellReturned||(tell.attempted&&!h.compressedTailFormatStored))return false;}
    if(h.loaderWorkspaceCleared&&!h.compressedLoopInitialized)for(auto byte:h.loaderWorkspace170)if(byte)return false;
    if(h.loaderBufferBytes&&(!h.loaderMaximumBits||!h.side||*h.loaderBufferBytes!=(*h.side)*(*h.side)*(*h.loaderMaximumBits)/8u))return false;
    if(h.loaderBufferManager){if(!h.texture||h.loaderBufferManager!=h.texture->manager_||
        !sourceLoadApplication_||h.loaderBufferManager!=sourceLoadApplication_->textureLifetimeOwner_)return false;
        for(std::size_t slot=0;slot<2;++slot){const auto& call=h.loaderBuffers[slot];
            if(!h.loaderBufferManager->sourceBufferCurrentNative(slot,&h,call))return false;
            if(call.entered&&(!h.loaderBufferBytes||call.requested!=*h.loaderBufferBytes+(slot?4u:0u)))return false;}
    }else for(const auto& call:h.loaderBuffers)if(call.entered)return false;
    if(h.loaderBuffers[1].entered&&(!h.loaderBuffers[0].returned||!h.loaderBuffers[0].borrowed))return false;
    if((h.payloadBranchSelected&&(!h.loaderBuffers[1].returned||!h.loaderBuffers[1].borrowed||!h.loaderFormat48||
        h.payloadExtendedFormat!=(*h.loaderFormat48>=16&&*h.loaderFormat48<=30)))||
       (h.payloadFileBits&&(!h.payloadBranchSelected||h.payloadExtendedFormat||*h.payloadFileBits!=sourceSkyFileBits29E5B0(*h.loaderFormat48)))||
       (h.payloadReadBytes&&(!h.payloadFileBits||!h.side||*h.payloadReadBytes!=(*h.side)*(*h.side)*(*h.payloadFileBits)/8u)))return false;
    if(h.payloadReadAttempted){
        if(!sourceSkyFile_||occurrence>=sourceSkyFile_->itemResourceSeeks_.size()||!h.loaderBufferManager||!h.payloadReadBytes)return false;
        const auto& read=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->rawPayload;const auto& buffer=h.loaderBufferManager->sourceBuffers_[1];
        if(!read.attempted||read.returned!=h.payloadReadReturned||read.copied!=h.payloadCopied||read.items!=h.payloadReadItems||
           read.requested!=*h.payloadReadBytes||read.destination!=h.loaderBuffers[1].borrowed||
           buffer.knownPrefix!=(h.payloadKnownPrefixBefore>h.payloadCopied?h.payloadKnownPrefixBefore:h.payloadCopied))return false;
    }
    if((h.payloadShortExit&&(!h.payloadReadReturned||h.payloadReadItems==h.payloadReadBytes))||
       (h.payloadVirtualEntered&&(!h.payloadBranchSelected||h.payloadPendingReturnPC!=(h.payloadExtendedFormat?0x24f550u:0x25000cu)||
        (h.payloadExtendedFormat?(h.payloadReadAttempted||h.payloadVirtualBorrow!=nullptr):
         (!h.payloadReadReturned||h.payloadReadItems!=h.payloadReadBytes||h.payloadVirtualBorrow!=h.loaderBuffers[1].borrowed)))))return false;
    if((h.payloadFieldsStored&&!h.payloadVirtualEntered)||(h.paletteB4Stored&&!h.payloadFieldsStored)||
       (h.paletteB4Stored&&h.resourcePaletteB4!=reg.paletteBorrow)||
       (h.formatNormalized&&(!h.paletteB4Stored||!h.rawFormat29E640||!h.normalizedFormatS1||
         (*h.rawFormat29E640!=0x22b8u&&*h.rawFormat29E640!=0x115cu&&*h.rawFormat29E640!=0x613u&&*h.rawFormat29E640!=0x235u&&*h.rawFormat29E640!=0x22bu)||
         *h.normalizedFormatS1!=((*h.rawFormat29E640==0x22b8u||*h.rawFormat29E640==0x115cu)?0x22b8u:0x613u)))||
       (h.embeddedFCGuardReturned&&(!(h.formatNormalized||h.compressedFinalTellReturned)||h.embeddedFCCapacityArgument!=10)))return false;
    if(h.embedded){const auto& e=*h.embedded;
        if(!h.embeddedFCGuardReturned||e.nativeWithdrawn||(e.allocationReturned&&!e.allocationAttempted)||
           (e.constructorEntered&&(!e.allocationReturned||!e.object20))||
           (e.alignedAttempted&&(!e.constructorEntered||e.roundedCount!=16||e.bytes!=256))||
           (e.alignedReturned&&!e.alignedAttempted)||(e.uncachedAliasSelected&&!e.alignedReturned)||
           (e.zeroed&&(!e.uncachedAliasSelected||!e.alignedBacking||!e.data00))||
           (e.constructorReturned&&(!e.zeroed||e.cursor04!=e.data00))||
           (e.published&&(!e.allocationReturned||(e.object20&&!e.constructorReturned))))return false;
        if(e.data00){if(!e.alignedBacking)return false;const auto address=reinterpret_cast<std::uintptr_t>(e.data00);
            const auto base=reinterpret_cast<std::uintptr_t>(e.alignedBacking.get());
            if(address%64||address<base||address-base>63)return false;}
        if(e.zeroed)for(std::size_t k=0;k<e.bytes;++k)if(e.data00[k])return false;
        if(e.constructorReturned){for(std::size_t k=0;k<8;++k)if(e.known[k])return false;
            for(std::size_t k=8;k<0x20;++k)if(!e.known[k]||e.object20[k]!=(k==8?16:0))return false;}
        if(e.published&&h.embeddedFCPublished!=(e.object20?h.embedded.get():nullptr))return false;
    }
    if((h.embeddedCallReturned&&(!h.embedded||!h.embedded->published))||
       (h.resourceFormatFieldsStored&&(!h.embeddedCallReturned||!h.normalizedFormatS1||
        h.resourceCurrentE8!=sourceSkyFormatBits29E910(*h.normalizedFormatS1)))||
       (h.levelAllocationEntered&&!h.resourceFormatFieldsStored))return false;
    if(h.loaderNullBufferExit&&!((h.loaderBuffers[0].returned&&!h.loaderBuffers[0].borrowed)||
       (h.loaderBuffers[1].returned&&!h.loaderBuffers[1].borrowed)))return false;
    if((h.rawFormat29E640&&(!h.loaderFormat48||*h.rawFormat29E640!=sourceSkyFormat29E640(*h.loaderFormat48)))||
       (h.slot58Stored&&(!h.allocationReturned||(h.allocationNonNull&&!h.derivedConstructorReturned)))||
       (h.loaderClassified&&(!h.slot58Stored||!h.allocationNonNull||h.loaderFormat48!=h.format48))||
       (h.rawLoaderEntered&&(!h.loaderClassified||h.compressed||!h.rawFormat29E640||
        h.loaderReceiver!=h.texture.get()||h.loaderPaletteBorrow!=reg.paletteBorrow||
        h.loaderHandle40!=state.handle40||h.rawStackScalars!=std::array<std::uint64_t,6>{{0,0,2,1,128,0x00ff00ffu}})))return false;
    if((h.derivedTableStored&&(!h.base29C6E0Returned||h.vtable08!=0x3d21c0u))||
       (h.embeddedFCConstructed&&!h.derivedTableStored)||h.derivedLevels>5||
       (h.derivedLevels&&!h.embeddedFCConstructed)||
       (h.derivedConstructorReturned&&(h.derivedLevels!=5||h.flagsF8KnownZero!=15)))return false;
    const auto& owner=sourceItemOwner(occurrence);if(!owner)return false;
    if(h.postLoaderSlotReloaded&&(!h.compressedLoaderReturned||!h.slot58Stored||h.slot58WithdrawnNative||
       !owner||owner->resources58[h.group*16+h.entry]!=h.postLoaderReceiver||h.postLoaderReceiver!=h.texture.get()||!h.postLoaderReceiver))return false;
    if((h.postLoaderReceiver&&!h.postLoaderSlotReloaded)||
       (h.postLoaderVirtualEntered&&(!h.postLoaderSlotReloaded||h.vtable08!=0x3d21c0u||h.postLoaderTarget!=0x2516f0u||h.postLoaderArguments!=std::array<std::uint32_t,3>{{1,0,0}}))||
       (h.postLoaderVirtualReturned&&(!h.postLoaderVirtualEntered||h.postLoaderResult!=1u))||
       (h.postLoaderResult&&!h.postLoaderVirtualReturned))return false;

    if(h.resourceTailEntered){
        if(!h.slot58Stored||(h.allocationNonNull?!h.postLoaderVirtualReturned:!h.allocationReturned)||!h.resourceTailFlags4C)return false;
        if(!owner->known[0x4c]||!owner->known[0x4d]||*h.resourceTailFlags4C!=(std::uint16_t(owner->storage[0x4c])|(std::uint16_t(owner->storage[0x4d])<<8)))return false;
        const auto& tail=*sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal);
        if(tail.resourceTailEntered&&tail.resourceAlignmentSelected!=bool(*h.resourceTailFlags4C&2u))return false;
        if(h.resourceAlignmentReturned!=tail.resourceAlignmentReturned)return false;
    }
    if((h.resourceAlignmentReturned&&!h.resourceTailEntered)||(h.resourceSourceReturned&&(!h.resourceAlignmentReturned||h.resourceSourceResult!=1u))||
       (h.resourceSourceResult&&!h.resourceSourceReturned))return false;
    const auto word=[&](std::size_t off,std::uint32_t value){for(std::size_t k=0;k<4;++k)
        if(!owner->known[off+k]||owner->storage[off+k]!=((value>>(k*8))&255u))return false;return true;};
    if(h.groupWord50&&!word(0x50+h.group*0x100,*h.groupWord50))return false;
    if(h.format48&&!word(0x48,*h.format48))return false;
    if(h.slot58Stored){if(owner->resources58[h.group*16+h.entry]!=(h.allocationNonNull?h.texture.get():nullptr)||h.slot58WithdrawnNative)return false;
        const auto off=0x58+h.group*0x100+h.entry*4;
        for(std::size_t k=0;k<4;++k)if(h.allocationNonNull?owner->known[off+k]:(!owner->known[off+k]||owner->storage[off+k]))return false;
    }
    if(h.sizeCode&&(!h.groupWord50||*h.sizeCode!=((*h.groupWord50>>(2*h.entry))&3u)))return false;
    if(h.allocationNonNull!=bool(h.texture&&h.texture->storage_))return false;
    if(h.baseReferenceAcquired){if(!h.vtable08||!h.texture)return false;
        const auto expect=[&](std::size_t off,std::uint64_t value,std::size_t size){for(std::size_t k=0;k<size;++k)
            if(!h.texture->known_[off+k]||h.texture->storage_[off+k]!=((value>>(k*8))&255u))return false;return true;};
        if(!expect(0,1,8)||!expect(8,*h.vtable08,4)||!expect(0x10,0,4)||!expect(0x14,(h.payloadFieldsStored||h.compressedDimensionsStored)?h.side.value_or(0):0,4)||!expect(0x18,(h.payloadFieldsStored||h.compressedDimensionsStored)?h.side.value_or(0):0,4)||
           (!h.registrationReturned&&(!expect(0xa0,0,4)||!expect(0xa4,0,4)))||!expect(0xa8,h.compressedDimensionsStored?16u:1u,4)||!expect(0xac,h.compressedTailFormatStored?h.compressedTailFormat.value_or(0):h.resourceFormatFieldsStored?h.normalizedFormatS1.value_or(0):0xffffffffu,4)||!expect(0x20,0,1))return false;
        for(std::size_t off=0xb0;off<=0xc0;off+=4){
            if(off==0xb4&&h.paletteB4Stored&&h.resourcePaletteB4){for(std::size_t k=0;k<4;++k)if(h.texture->known_[off+k])return false;}
            else if(!expect(off,off==0xb0?((h.resourceFormatFieldsStored||h.compressedCountAdjusted)?1u:h.compressedLoopIndex.value_or(0)):0u,4))return false;
        }
        if(h.embeddedFCConstructed){if(h.embeddedFCPublished){for(std::size_t k=0;k<4;++k)if(h.texture->known_[0xfc+k])return false;}
            else if(!expect(0xfc,0,4))return false;}
        for(std::size_t j=0;j<5;++j){const auto delta=j*0x34;
            const SourceSkyFollowingCompressedLevel* later=nullptr;
            for(const auto& level:h.followingCompressedLevels)if(level->index==j){if(later)return false;later=level.get();}
            if(j<h.derivedLevels){
                if(h.level130KnownZero[j]!=3||h.texture->known_[0x130+delta]||
                   (!(h.compressedBaseAliasStored&&h.compressedLevelIndex==j&&h.compressedBacking.aligned)&&!(later&&later->baseAliasStored&&later->backing.aligned)&&!expect(0x100+delta,0,4))||
                   (!(h.compressedBacking.published&&h.compressedLevelIndex==j)&&!(later&&later->backing.published)&&!expect(0x118+delta,0,4))||!expect(0x124+delta,later&&later->dimensionsStored?later->dimension:h.compressedLevelDimensionsStored&&h.compressedLevelIndex==j?16u:0xffffu,2))return false;
                if(h.compressedBaseAliasStored&&h.compressedLevelIndex==j&&h.compressedBacking.aligned)
                    for(std::size_t k=0;k<4;++k)if(h.texture->known_[0x100+delta+k])return false;
                if(h.compressedBacking.published&&h.compressedLevelIndex==j){
                    if(h.compressedBacking.aligned){for(std::size_t k=0;k<4;++k)if(h.texture->known_[0x118+delta+k])return false;}
                    else if(!expect(0x118+delta,h.compressedOffsetAliasStored?16u:0u,4))return false;
                }
                if(later&&later->baseAliasStored&&later->backing.aligned)
                    for(std::size_t k=0;k<4;++k)if(h.texture->known_[0x100+delta+k])return false;
                if(later&&later->backing.published){
                    if(later->backing.aligned){for(std::size_t k=0;k<4;++k)if(h.texture->known_[0x118+delta+k])return false;}
                    else if(!expect(0x118+delta,later->offsetAliasStored?16u:0u,4))return false;
                }
                for(const auto off:{0x104u,0x110u,0x108u,0x10cu,0x114u,0x11cu,0x120u,0x12cu,0x128u})
                    if(!expect(off+delta,later&&later->dimensionsStored&&(off==0x104u||off==0x110u)?later->dimension:
                        later&&later->dimensionsStored&&off==0x108u?(later->dimension/64u?later->dimension/64u:1u):
                        later&&((later->logReturned[0]&&off==0x10cu)||(later->logReturned[1]&&off==0x114u))?static_cast<std::uint32_t>(4u+later->ordinal):
                        later&&later->mirrorDimensionsStored&&(off==0x128u||off==0x12cu)?later->dimension:
                        later&&later->sizeStored&&off==0x11cu?later->size16C-(later->headerSizeAdjusted?16u:0u):
                        h.compressedLevelIndex==j&&h.compressedLevelDimensionsStored&&(off==0x104u||off==0x110u)?16u:
                        h.compressedLevelIndex==j&&h.compressedLevelDimensionsStored&&off==0x108u?1u:
                        h.compressedLevelIndex==j&&((h.compressedLogReturned[0]&&off==0x10cu)||(h.compressedLogReturned[1]&&off==0x114u))?4u:
                        h.compressedLevelIndex==j&&h.compressedMirrorDimensionsStored&&(off==0x128u||off==0x12cu)?16u:
                        off==0x11cu&&h.compressedLevelSizeStored&&h.compressedLevelIndex==j?
                            h.compressedPayloadSize16C.value_or(0)-(h.compressedHeaderSizeAdjusted?16u:0u):0xffffffffu,4))return false;
            }else if(h.level130KnownZero[j])return false;
        }
        if(h.derivedConstructorReturned){
            if(!expect(0xe8,h.resourceCurrentE8.value_or(h.loaderE8.value_or(0xffffffffu)),4)||!expect(0x204,0,4)||!expect(0x208,0,4)||h.texture->known_[0xf8])return false;
            for(const auto off:{0xf4u,0xecu,0xf0u})if(!expect(off,0xffffffffu,4))return false;
        }
    }
    return true;
}
bool Owner::continueSourceSkyResourcePrefix(std::uint32_t& pc,std::size_t occurrence,std::size_t group,std::size_t entry)noexcept {
    if(pc!=0x128d50u||occurrence>sourceSkyLaterItems_.size()||group>=6||entry>=16||!preflightSourcePostParticleModelsNative())return false;
    auto& state=sourceItemHistory(occurrence);auto& reg=state.registration;auto& owner=sourceItemOwner(occurrence);
    if(!reg.seekReturned||reg.pendingResource||reg.completedResources.size()>=96||reg.completedEntries!=group*16+entry||!owner)return false;
    const auto fileOrdinal=reg.completedResources.size();
    if(fileOrdinal){
        if(reg.resourceCalls.size()!=fileOrdinal||!reg.resourceCalls.back()||!sourceSkyFile_)return false;auto& call=*reg.resourceCalls.back();
        if(!call.entered||call.bodyEntered||call.index!=group*16+entry)return false;
        a::OriginalStandardTextureDiagnostic50LA d;
        if(!sourceSkyFile_->beginNextResource(occurrence,fileOrdinal,call.cursor,d)){pc=sourcePostParticleModelsPC_=d.sourcePC?d.sourcePC:sourceSkyFile_->pc_;return false;}
        call.bodyEntered=true;
    }else if(!reg.resourceCalls.empty())return false;
    try{reg.pendingResource.reset(new SourceSkyResourcePrefix);}catch(...){return false;}
    auto& h=*reg.pendingResource;h.entered=true;h.fileOrdinal=fileOrdinal;h.group=group;h.entry=entry;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    const auto word=[&](std::size_t off,std::optional<std::uint32_t>& out){for(std::size_t k=0;k<4;++k)if(!owner->known[off+k])return false;
        out=std::uint32_t(owner->storage[off])|(std::uint32_t(owner->storage[off+1])<<8)|(std::uint32_t(owner->storage[off+2])<<16)|(std::uint32_t(owner->storage[off+3])<<24);return true;};
    // One source callee prefix, qualified by the actual caller arguments.
    // Original item format is not substituted for compressed input32.
    const auto initializeLoader=[&](std::uint32_t input,std::uint32_t layout,bool indexed){
        std::uint32_t guardPC=0x24f2ec;
        if(!a::OriginalConfigProcessGlobals50LA::checkTextureLoad24F2C0(input,guardPC)){at(guardPC);return false;}
        at(guardPC);h.loaderGuardReturned=true;
        at(0x24f378);at(0x24f37c);at(0x24f380);h.loaderFrameInitialized=true;
        at(0x29e640);at(0x24f384);h.loaderFormat3C=sourceSkyFormat29E640(input);
        at(0x24f388);at(0x24f38c);at(0x24f390);h.loaderStack140=0;at(0x29ea50);
        if(input>=0x80000000u&&input<0x8000001fu)return false;
        if(indexed!=(input>=31u&&input<=36u))return false;
        at(0x24f394);at(0x24f3a8);h.loaderF8KnownZero=indexed?14:15;h.loaderF8KnownOne=indexed?1:0;
        at(0x24f3ac);at(0x24f3b4);at(0x24f3b8);
        if(indexed){at(0x24f3bc);at(0x24f3c0);at(0x24f3c4);layout=0x613;
            if(input==32u){at(0x24f3c8);at(0x24f3cc);input=34;}else {at(0x24f3d0);at(0x24f3d4);input=33;}
        }else {at(0x24f3d8);at(0x24f3e4);h.loaderF8KnownZero=11;h.loaderF8KnownOne=4;}
        h.loaderEffectiveFormat=input;h.loaderEffectiveLayout=layout;h.loaderFlagsStored=true;
        // Preserve unknown upper nibble; no whole-byte host read or invented bits.
        at(0x24f3e8);at(0x24f3f0);at(0x306478);h.loaderWorkspace170.fill(0);h.loaderWorkspaceCleared=true;
        at(0x24f3f8);at(0x29e910);h.loaderE8=sourceSkyFormatBits29E910(layout);at(0x24f400);
        for(std::size_t k=0;k<4;++k){h.texture->storage_[0xe8+k]=static_cast<std::uint8_t>(*h.loaderE8>>(k*8));h.texture->known_[0xe8+k]=true;}
        return true;
    };
    const auto publishAndEnterLoader=[&](){
        at(0x128e94);at(0x128e98);at(0x128e9c);
        const auto index=group*16+entry,off=0x58+group*0x100+entry*4;
        owner->resources58[index]=h.allocationNonNull?h.texture.get():nullptr;
        for(std::size_t k=0;k<4;++k){if(h.allocationNonNull)owner->known[off+k]=false;
            else {owner->storage[off+k]=0;owner->known[off+k]=true;}}
        h.slot58Stored=true;at(0x128ea0);at(0x128ea4);at(0x128ea8);
        if(!h.allocationNonNull){at(0x129054);return continueSourceSkyResourceTail129054(pc,occurrence);} //ActualNULL bypasses loader/virtualcall.
        at(0x128eac);at(0x128eb0);if(!word(0x48,h.loaderFormat48))return false;
        at(0x29e510);const bool compressed=sourceSkyFormat29E510(*h.loaderFormat48);h.loaderClassified=true;
        at(0x128eb4);at(0x128eb8);
        if(compressed){at(0x128ebc);at(0x128ec0);at(0x128ec4);at(0x128ec8);at(0x128ecc);
            h.compressedStackInitialized=true;for(std::size_t i=1;i<5;++i)h.compressedHeaderWords[i]=0;
            static constexpr std::uint32_t globals[5]={0x128ed0,0x128eec,0x128f08,0x128f24,0x128f40};
            static constexpr std::uint32_t calls[5]={0x128ee0,0x128efc,0x128f18,0x128f34,0x128f50};
            for(std::size_t i=0;i<5;++i){at(globals[i]);
                if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
                at(calls[i]);a::OriginalStandardTextureDiagnostic50LA d;
                const bool returned=sourceSkyFile_->readCompressedResourceHeader(occurrence,i,d);
                const auto& read=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->compressedHeaders[i];
                h.compressedHeaderAttempted[i]=read.attempted;h.compressedHeaderReturned[i]=read.returned;
                h.compressedHeaderCopied[i]=read.copied;h.compressedHeaderItems[i]=read.items;
                // AC was uninitialized; the other four stackwords startzero.
                // Short reads preserve those zero suffixes; they are not errors.
                if(i||read.copied==4){std::uint32_t value=0;for(std::size_t k=0;k<read.copied;++k)value|=std::uint32_t(read.bytes[k])<<(k*8);
                    h.compressedHeaderWords[i]=value;}
                if(!returned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
            }
            at(0x128f58);at(0x128f5c);at(0x128f60);at(0x128f64);
            if(h.compressedHeaderWords[3]!=0x1000u){at(0x128f68);at(0x128f6c);at(0x29e9f0);
                if(!h.compressedHeaderWords[0])return false; //Cannot read uninitialized AC suffix.
                auto& value=*h.compressedHeaderWords[0];if(value==31u||value==32u)value=31u;else if(value==33u||value==34u)value=32u;
                h.compressedHeaderConverted=true;at(0x29ea48);at(0x29ea4c);
            }
            at(0x128f70);at(0x128f80);h.compressedHeaderWords[3]=h.compressedHeaderWords[2];h.compressedB8Replaced=true;
            at(0x128f84);h.loaderPaletteBorrow=reg.paletteBorrow;h.rawStackScalars={{0,0,2,1,128,0x00ff00ffu}};
            at(0x128fa8);h.loaderReceiver=owner->resources58[index];at(0x128fac);h.loaderHandle40=state.handle40;
            at(0x128fb0);if(!h.payloadBytes)return false; //Actual stackA8 load, not an earlier artificial shortread error.
            at(0x128fb4);h.compressedInputFormat=32;at(0x128fb8);h.compressedInputLayout=0x613;at(0x128fbc);if(h.vtable08!=0x3d21c0u||h.loaderReceiver!=h.texture.get()||!h.loaderHandle40)return false;
            at(0x128fc0);at(0x128fc4);at(0x128fc8);at(0x128fcc);h.compressedPendingReturnPC=0x128fd0u;h.compressedLoaderEntered=true;
            at(0x24f2c0);if(!initializeLoader(*h.compressedInputFormat,*h.compressedInputLayout,true))return false;
            at(0x24f404);at(0x24f408);at(0x24f40c);at(0x24f410);
            // F8bit0 branches around both Application reusable-buffer calls.
            at(0x24f4c4);at(0x29e560);at(0x24f4cc);at(0x24f4d0);
            at(0x24f4d4);at(0x24f4d8);at(0x24f4dc);at(0x24f4e0);
            // Effective format34 takes extended path, bit0 skips virtual+24.
            const auto store=[&](std::size_t off,std::uint32_t value){for(std::size_t k=0;k<4;++k){
                h.texture->storage_[off+k]=static_cast<std::uint8_t>(value>>(k*8));h.texture->known_[off+k]=true;}};
            const auto allocateCompressed=[&](SourceSkyCompressedBacking& backing,std::uint32_t requested,std::uint32_t index,
                bool& counterRead,bool& counterStored,std::uint32_t& counterBefore,std::uint32_t& counterAfter,std::uint64_t& counterSequence){
            at(0x24f7b4);at(0x24f7b8);at(0x305500);
            if(!a::OriginalConfigProcessGlobals50LA::descriptorDelete2E8B00AdmittedNative())return false;
            //305530 rounds size+13hex, then requests rounded+50hex from
            //305990, which itself adds13hex. Bound wrapping/signed-large
            //allocator forms before claiming any source allocation result.
            const auto rounded=(std::uint64_t(requested)+19u)&~std::uint64_t(15);
            const auto capacity=rounded<16?16:rounded;
            if(capacity+80u+19u>0x7fffffffu)return false;
            backing.capacity=static_cast<std::size_t>(capacity);backing.attempted=true;
            try{backing.allocation.reset(new std::uint8_t[backing.capacity+63]);}catch(...){backing.allocation.reset();}
            backing.returned=true;
            if(backing.allocation){const auto address=reinterpret_cast<std::uintptr_t>(backing.allocation.get());
                backing.aligned=backing.allocation.get()+((64-address%64)%64);}
            // Size0 may return nonNULL; no prepared buffer or fake guest pointer.
            at(0x24f7c0);at(0x24f7c4);at(0x24f7c8);at(0x24f7cc);at(0x24f7d0);
            at(0x24f7d4);at(0x24f7d8);at(0x24f7dc);at(0x24f7e0);at(0x24f7e4);at(0x24f7e8);at(0x24f7ec);
            const auto pointerOffset=0x118u+0x34u*(index);
            for(std::size_t k=0;k<4;++k){if(backing.aligned)h.texture->known_[pointerOffset+k]=false;
                else {h.texture->storage_[pointerOffset+k]=0;h.texture->known_[pointerOffset+k]=true;}}
            backing.published=true;at(0x24f7f0);
            if(!a::OriginalConfigProcessGlobals50LA::readCompressedBytes24F7F0(counterBefore))return false;
            counterRead=true;at(0x24f7f4);at(0x24f7f8);at(0x24f7fc);
            if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
            at(0x24f800);at(0x24f804);at(0x24f808);
            if(!a::OriginalConfigProcessGlobals50LA::addCompressedBytes24F808(requested,
               counterBefore,counterAfter,counterSequence))return false;
            //The scalar addition is unconditional, including a NULL allocation.
            counterStored=true;
                return true;
            };
            const auto finishCompressedLevel=[&](std::uint32_t index,std::uint32_t dimension,std::uint32_t originalSize,SourceSkyCompressedBacking& backing,
                bool& dimensionsStored,std::array<bool,2>& logs,bool& mirrorStored,bool& headerAdjusted,bool& baseAliasStored,bool& offsetAliasStored,std::size_t& liveOffset){
            at(0x24f82c);at(0x24f830);at(0x24f834);at(0x24f838);at(0x24f83c);at(0x24f840);at(0x24f844);at(0x24f848);
            const auto level=0x100u+0x34u*(index);
            at(0x24f84c);store(level+4,dimension);at(0x24f850);store(level+0x10,dimension);
            at(0x24f854);at(0x24f858);at(0x24f85c);h.texture->storage_[level+0x24]=static_cast<std::uint8_t>(dimension);h.texture->storage_[level+0x25]=static_cast<std::uint8_t>(dimension>>8);
            h.texture->known_[level+0x24]=h.texture->known_[level+0x25]=true;
            at(0x24f860);at(0x24f864);at(0x24f868);at(0x24f874);at(0x24f878);
            //Admitted positive dimensions take BGEZ; division delay shift is
            //signed/64. BNEL annuls24F87C when quotient>=2, then24F884 stores.
            const auto tiles=dimension/64u;
            if(tiles<2u){at(0x24f87c);at(0x24f888);store(level+8,1);}
            else {at(0x24f880);at(0x24f884);store(level+8,tiles);}
            dimensionsStored=true;
            const auto readDimension=[&](std::size_t offset,std::uint32_t& value){value=0;
                for(std::size_t k=0;k<4;++k){if(!h.texture->known_[offset+k])return false;
                    value|=std::uint32_t(h.texture->storage_[offset+k])<<(k*8);}return true;};
            std::uint32_t integerLog=0;
            const auto dimensionLog=[&](std::uint32_t operand){
                at(0x3101e8);if(operand!=16u&&operand!=32u&&operand!=64u&&operand!=128u&&operand!=256u)return false;
                //Source-specific proof, NOT host log():3101E8 converts16
                //exactly. 302A58/2FF788 normalizes16 and2 to1 (f=0),
                //then rounds k*ln2_hi+k*ln2_lo with k=4/1. Constants:
                //3C5D88=3FE62E42FEE00000,3C5D90=3DEA39EF35793C76.
                //Scaling both normal finite terms by4 commutes with final
                //binary rounding. 30FF18 therefore divides equal significands
                //with exponentdifference2, exactly4;3102A0 truncates to4.
                //For positive finite operands every302A58 libmode exits before
                //errno/error callbacks; no current mode value is assumed.
                //Independent operand32 proof: original power-of-two branch
                //gives integer significandsD=162E42FEFA39EF and
                //N=1BB9D3BEB8C86B, withlog2=D*2^-53 and
                //log32=N*2^-51. For those integers,4*N-5*D=1;
                //quotient5+1/D is inside5's halfULP2^-51 sinceD>2^51.
                //Original nearest-even softpack returns exact5, truncate5.
                //Independent k6/7/8 proofs: log(64/128/256)=N*2^-50,
                //N=10A2B23F3BAB73/13687A9F1AF2B1/162E42FEFA39EF.
                //With D above,8*N-k*D=-2/-1/0. SinceD>2*2^51,
                //the quotients6-2/D and7-1/D round exactly6/7;8 is exact.
                //k*HI exact; k*LO nearest-even integer significands at2^-82:
                //13AB73681AED58 (tie-even),16F2B14ECA14E7,1A39EF35793C76.
                //Final-sum residuals681AED58/4ECA14E7/35793C76 over2^32
                //are each below1/2. These proofs use original softpack rules.
                integerLog=operand==16u?4u:operand==32u?5u:operand==64u?6u:operand==128u?7u:8u;
                at(0x30f7a0);at(0x310298);return true;
            };
            std::uint32_t operand=0;at(0x24f88c);if(!readDimension(level+4,operand)||!dimensionLog(operand))return false;
            at(0x24f894);at(0x302a58);at(0x2ff788);at(0x24f89c);at(0x24f8a0);
            at(0x24f8a4);at(0x302a58);at(0x2ff788);at(0x24f8ac);at(0x24f8b0);at(0x30ff18);
            at(0x24f8b8);at(0x3102a0);at(0x24f8c0);store(level+0xc,integerLog);logs[0]=true;
            at(0x24f8c4);if(!readDimension(level+0x10,operand)||!dimensionLog(operand))return false;
            at(0x24f8cc);at(0x302a58);at(0x2ff788);at(0x24f8d4);at(0x24f8d8);
            at(0x24f8dc);at(0x302a58);at(0x2ff788);at(0x24f8e4);at(0x24f8e8);at(0x30ff18);
            at(0x24f8f0);at(0x3102a0);at(0x24f8f8);store(level+0x14,integerLog);logs[1]=true;
            at(0x24f8fc);at(0x24f900);store(level+0x2c,dimension);at(0x24f904);at(0x24f908);store(level+0x28,dimension);mirrorStored=true;
            at(0x24f90c);at(0x24f910);at(0x24f914);store(level+0x1c,originalSize-16u);headerAdjusted=true;
            at(0x24f918);at(0x24f91c);
            for(std::size_t k=0;k<4;++k){if(backing.aligned)h.texture->known_[level+k]=false;
                else {h.texture->storage_[level+k]=0;h.texture->known_[level+k]=true;}}
            baseAliasStored=true;
            at(0x24f920);at(0x24f924);at(0x24f928);at(0x24f92c);
            //Keep the owned allocation base and its known prefix unchanged.
            //A NULL source base plus16 is known EE word16, NOT host address16.
            //Nonnull base+16 is represented as a typed offset, even if it is
            //one-past; no payload/header bytes are read by this adjustment.
            if(backing.aligned&&backing.capacity<16)return false;
            liveOffset=16;offsetAliasStored=true;
            if(!backing.aligned)store(level+0x18,16);
                at(0x24f968);return true;
            };
            const auto finishCompressedTail=[&](){
                at(0x24faac);at(0x24fab0);at(0x24fab4);at(0x24fab8);h.compressedTailEntered=true;
                if(!(h.loaderF8KnownOne&1u))return false;
                at(0x24fbc8);std::uint32_t count=0;for(std::size_t k=0;k<4;++k){if(!h.texture->known_[0xb0+k])return false;count|=std::uint32_t(h.texture->storage_[0xb0+k])<<(k*8);}
                at(0x24fbcc);at(0x24fbd0);if(static_cast<std::int32_t>(count)>=5){at(0x24fbd4);count=5;}
                at(0x24fbd8);store(0xb0,count);h.compressedTailCountStored=true;
                at(0x24fbdc);at(0x24fbe0);at(0x24fbe4);at(0x24fbe8);at(0x24fbec);at(0x29e640);
                if(h.loaderEffectiveFormat!=34u)return false;
                h.compressedTailFormat=sourceSkyFormat29E640(*h.loaderEffectiveFormat);
                at(0x24fbf4);store(0xac,*h.compressedTailFormat);h.compressedTailFormatStored=true;
                at(0x24fbf8);at(0x24fbfc);at(0x24fc00);at(0x24fc04);
                if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
                a::OriginalStandardTextureDiagnostic50LA tailDiagnostic;
                if(!sourceSkyFile_->tellCompressedFinal(occurrence,tailDiagnostic)){at(tailDiagnostic.sourcePC?tailDiagnostic.sourcePC:sourceSkyFile_->pc_);return false;}
                h.compressedFinalTellReturned=true;at(0x24fc08);
                //Actual stack140 was zeroed at24F390, not a caller argument.
                if(h.loaderStack140!=0u)return false;
                at(0x24fc0c);at(0x24fc10);at(0x24fc14);at(0x24fc18);at(0x24fc1c);
                h.embeddedFCCapacityArgument=10;at(0x257750);
                if(!continueSourceSkyEmbedded257750(pc,occurrence))return false;
                at(0x24fc20);h.compressedLoaderResult=1;at(0x24fc24);at(0x24fc28);
                at(0x25000c);at(0x250010);at(0x250014);at(0x250018);at(0x25001c);at(0x250020);at(0x250024);at(0x250028);at(0x25002c);at(0x250030);at(0x250034);at(0x250038);
                h.compressedLoaderReturned=true;at(0x128fd0);at(0x128fd4);
                //Branch delay reloads the actual item slot, not the earlier
                //loader return or a prepared resource. Revalidate live owners.
                if(!preflightSourceSkyResourcePrefix(occurrence))return false;
                h.postLoaderReceiver=owner->resources58[index];h.postLoaderSlotReloaded=true;
                if(!h.postLoaderReceiver||h.postLoaderReceiver!=h.texture.get())return false;
                at(0x12903c);h.postLoaderArguments[0]=1;at(0x129040);h.postLoaderArguments[1]=0;
                at(0x129044);std::uint32_t table=0;for(std::size_t k=0;k<4;++k){if(!h.texture->known_[8+k])return false;table|=std::uint32_t(h.texture->storage_[8+k])<<(k*8);}
                if(table!=0x3d21c0u||h.vtable08!=table)return false;
                //Immutable original3D21C0+30 entry is2516F0. This callee is
                //JR RA with V0=1 in its delay slot; no allocation/upload/retain.
                at(0x129048);h.postLoaderTarget=0x2516f0u;at(0x12904c);at(0x129050);h.postLoaderArguments[2]=0;h.postLoaderVirtualEntered=true;
                at(0x2516f0);at(0x2516f4);h.postLoaderResult=1;h.postLoaderVirtualReturned=true;
                at(0x129054);return continueSourceSkyResourceTail129054(pc,occurrence);
            };
            if(!h.side)return false;
            at(0x24f568);store(0x14,*h.side);at(0x24f56c);at(0x24f570);store(0x18,*h.side);
            at(0x24f574);store(0xa8,16);h.compressedDimensionsStored=true;
            at(0x24f578);store(0xb0,0);at(0x24f57c);at(0x24f580);h.compressedLoopScale=1;
            at(0x24f584);at(0x24f588);at(0x24f58c);h.compressedLoopIndex=0;
            at(0x24f590);h.compressedLoopScale=16;h.compressedLoopInitialized=true;
            at(0x24f594);at(0x24f598);
            if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
            at(0x24f59c);a::OriginalStandardTextureDiagnostic50LA d;
            if(!sourceSkyFile_->tellCompressedOffset(occurrence,0,false,d)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
            at(0x24f5a4);
            for(;;){const auto index=static_cast<std::size_t>(*h.compressedLoopIndex);const bool terminal=*h.side<*h.compressedLoopScale;
                // Native workspace capacity bound, not a source decoder success.
                if(index>=12)return false;
                at(terminal?0x24f644:0x24f5a4);at(terminal?0x24f648:0x24f5a8);
                if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
                at(terminal?0x24f64c:0x24f5ac);
                if(!sourceSkyFile_->tellCompressedOffset(occurrence,index+1,terminal,d)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
                at(terminal?0x24f654:0x24f5b4);at(terminal?0x24f658:0x24f5b8);
                // The signed-low32 tell conversion is dead on this indexed path;
                // actual tell still occurred, and no offset addition is invented.
                at(terminal?0x24f664:0x24f5c4);
                if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
                at(terminal?0x24f678:0x24f5d8);
                if(!h.loaderWorkspaceCleared||!h.compressedLoaderEntered||h.loaderF8KnownOne!=1u||
                   (h.loaderF8KnownZero&14u)!=14u)return false;
                const bool returned=sourceSkyFile_->readCompressedOffset(occurrence,index,d);
                const auto& read=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->compressedOffsets[index].read;
                for(std::size_t k=0;k<read.copied;++k)h.loaderWorkspace170[index*4+k]=read.bytes[k];
                if(!returned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
                at(terminal?0x24f680:0x24f5e0);at(terminal?0x24f684:0x24f5e4);
                if(read.items!=1u){h.compressedOffsetShortExit=true;at(terminal?0x24f68c:0x24f5ec);at(0x25000c);return false;}
                at(terminal?0x24f694:0x24f5f4);at(terminal?0x24f69c:0x24f5fc);
                at(terminal?0x24f6b0:0x24f610);for(std::size_t k=0;k<4;++k)h.loaderWorkspace170[index*4+k]=0;
                if(terminal){h.compressedOffsetsReturned=true;break;}
                at(0x24f628);at(0x24f62c);*h.compressedLoopScale*=2;at(0x24f630);++*h.compressedLoopIndex;
                at(0x24f634);at(0x24f638);at(0x24f63c);at(0x24f640);store(0xb0,*h.compressedLoopIndex);
            }
            at(0x24f6c4);at(0x24f6c8);at(0x24f6cc);at(0x24f6d0);
            // Actual stack1A8 is caller stack08, presently0; no guessed flags.
            if(h.rawStackScalars[0]!=0)return false;
            at(0x24f6ec);store(0xb0,1);h.compressedCountAdjusted=true;
            at(0x24f6f0);at(0x24f6f4);at(0x24f6f8);at(0x24f6fc);
            h.compressedLevelIndex=*h.compressedLoopIndex-1;
            at(0x24f700);at(0x24f704);at(0x24f708);at(0x24f70c);at(0x24f710);at(0x24f714);
            at(0x24f718);at(0x24f71c);at(0x24f720);at(0x24f724);at(0x24f728);at(0x24f72c);
            at(0x24f730);at(0x24f734);at(0x24f738);at(0x24f73c);at(0x24f740);at(0x24f744);at(0x24f748);
            // Every successful indexed offset word was genuinely overwritten tozero,
            //including a positive EOF clamp reporting the original requested1.
            // New local loopS5=0; the preserved offset-loop index is historical.
            h.compressedPayloadSize16C=0;h.compressedLevelSetup=true;
            at(0x24f760);at(0x24f764);at(0x24f768);at(0x24f76c);at(0x24f770);at(0x24f774);
            if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
            at(0x24f778);at(0x24f77c);at(0x24f780);at(0x24f784);
            if(!h.compressedOffsetsReturned||!h.loaderWorkspaceCleared||
               h.compressedPayloadSize16C!=0u)return false;
            const bool returned=sourceSkyFile_->readCompressedPayloadSize(occurrence,d);
            const auto& sizeRead=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->compressedPayloadSize;
            for(std::size_t k=0;k<sizeRead.copied;++k)*h.compressedPayloadSize16C|=std::uint32_t(sizeRead.bytes[k])<<(k*8);
            if(!returned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
            at(0x24f78c);at(0x24f790);at(0x24f794);at(0x24f798);at(0x24f79c);at(0x24f7a0);
            at(0x24f7a4);at(0x24f7a8);at(0x24f7ac);at(0x24f7b0);
            const auto destination=0x11cu+0x34u*(*h.compressedLevelIndex);
            // Side512 reaches level5 and destination220 outside fresh210.
            // Do not perform an out-of-owner native write or claim guest success.
            if(destination>0x210u-4u)return false;
            store(destination,*h.compressedPayloadSize16C);h.compressedLevelSizeStored=true;
            if(!allocateCompressed(h.compressedBacking,*h.compressedPayloadSize16C,*h.compressedLevelIndex,
                h.compressedCounterRead,h.compressedCounterStored,h.compressedCounterBefore,h.compressedCounterAfter,h.compressedCounterSequence))return false;
            auto& backing=h.compressedBacking;
            at(0x24f80c);at(0x24f810);at(0x24f814);
            const bool transferReturned=sourceSkyFile_->readCompressedPayload(occurrence,backing.aligned,*h.compressedPayloadSize16C,d);
            const auto& transfer=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->compressedPayload;
            h.compressedKnownPrefix=transfer.copied;
            if(!transferReturned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
            at(0x24f81c);at(0x24f820);
            if(transfer.items!=h.compressedPayloadSize16C){h.compressedTransferShort=true;at(0x24f824);at(0x25000c);return false;}
            if(!finishCompressedLevel(*h.compressedLevelIndex,16,*h.compressedPayloadSize16C,h.compressedBacking,
                h.compressedLevelDimensionsStored,h.compressedLogReturned,h.compressedMirrorDimensionsStored,h.compressedHeaderSizeAdjusted,
                h.compressedBaseAliasStored,h.compressedOffsetAliasStored,h.compressedLiveOffset118))return false;
            at(0x24f968);at(0x24f96c);at(0x24f970);at(0x24f974);h.compressedNextDimension=32;
            at(0x24fa9c);h.compressedNextIteration=1;at(0x24faa0);at(0x24faa4);
            h.compressedNextLoopTaken=*h.side>=*h.compressedNextDimension;
            at(0x24faa8);h.compressedLoopAdvanced=true; //Height doubles in branch delay, including exit.
            if(!h.compressedNextLoopTaken)return finishCompressedTail();
            std::uint32_t nextDimension=*h.compressedNextDimension;std::size_t nextOrdinal=*h.compressedNextIteration;
            for(;;){
            at(0x24f718);
            try{auto next=std::unique_ptr<SourceSkyFollowingCompressedLevel>(new SourceSkyFollowingCompressedLevel);h.followingCompressedLevels.push_back(std::move(next));}catch(...){return false;}
            auto& next=*h.followingCompressedLevels.back();next.ordinal=nextOrdinal;
            next.dimension=nextDimension;
            at(0x24f71c);at(0x24f720);at(0x24f724);at(0x24f728);at(0x24f72c);
            if(!h.compressedLevelIndex||*h.compressedLevelIndex<next.ordinal)return false;
            next.index=*h.compressedLevelIndex-static_cast<std::uint32_t>(next.ordinal);
            at(0x24f730);at(0x24f734);at(0x24f738);at(0x24f73c);at(0x24f740);at(0x24f744);at(0x24f748);
            //Same source workspace, all indexed offsets actually zeroed. New
            //stack16C/read occurrence, not reused first-level size or buffer.
            next.size16C=0;next.selected=true;
            at(0x24f760);at(0x24f764);at(0x24f768);at(0x24f76c);at(0x24f770);at(0x24f774);
            if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
            at(0x24f778);at(0x24f77c);at(0x24f780);at(0x24f784);
            if(!h.compressedOffsetsReturned||!h.loaderWorkspaceCleared||next.size16C!=0u)return false;
            const bool nextReturned=sourceSkyFile_->readFollowingCompressedSize(occurrence,next.ordinal,d);
            const auto& following=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->followingCompressedSizes;
            if(following.size()==next.ordinal){const auto& read=*following.back();next.sizeReadAttempted=read.attempted;next.sizeReadReturned=read.returned;
                for(std::size_t k=0;k<read.copied;++k)next.size16C|=std::uint32_t(read.bytes[k])<<(k*8);}
            if(!nextReturned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
            at(0x24f78c);at(0x24f790);at(0x24f794);at(0x24f798);at(0x24f79c);at(0x24f7a0);
            at(0x24f7a4);at(0x24f7a8);at(0x24f7ac);at(0x24f7b0);
            const auto nextDestination=0x11cu+0x34u*next.index;if(nextDestination>0x210u-4u)return false;
            store(nextDestination,next.size16C);next.sizeStored=true;
            if(!allocateCompressed(next.backing,next.size16C,next.index,next.counterRead,next.counterStored,
                next.counterBefore,next.counterAfter,next.counterSequence))return false;
            at(0x24f80c);at(0x24f810);at(0x24f814);
            const bool nextTransferReturned=sourceSkyFile_->readFollowingCompressedPayload(occurrence,next.ordinal,next.backing.aligned,next.size16C,d);
            const auto& payloads=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->followingCompressedPayloads;
            if(payloads.size()==next.ordinal)next.knownPrefix=payloads.back()->copied;
            if(!nextTransferReturned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
            const auto& nextTransfer=*payloads.back();at(0x24f81c);at(0x24f820);
            if(nextTransfer.items!=next.size16C){next.transferShort=true;at(0x24f824);at(0x25000c);return false;}
            next.transferAccepted=true;at(0x24f82c);
            if(!finishCompressedLevel(next.index,next.dimension,next.size16C,next.backing,next.dimensionsStored,next.logReturned,
                next.mirrorDimensionsStored,next.headerSizeAdjusted,next.baseAliasStored,next.offsetAliasStored,next.liveOffset118))return false;
            at(0x24f968);at(0x24f96c);at(0x24f970);at(0x24f974);next.nextDimension=next.dimension*2u;
            at(0x24fa9c);next.nextOrdinal=next.ordinal+1;at(0x24faa0);at(0x24faa4);
            next.nextLoopTaken=*h.side>=next.nextDimension;
            at(0x24faa8);next.loopAdvanced=true; //S4 delay-slot update also occurs on exit.
            if(!next.nextLoopTaken)return finishCompressedTail();
            nextDimension=next.nextDimension;nextOrdinal=next.nextOrdinal;
            if(nextOrdinal>4)return false; //Native five-record bound, not source success.
            }
        }
        at(0x128fd8);at(0x128fdc);at(0x29e640);
        // Original format switch, including default0 for unsigned values>=37.
        h.rawFormat29E640=sourceSkyFormat29E640(*h.loaderFormat48);
        at(0x128fe0);h.loaderPaletteBorrow=reg.paletteBorrow;
        h.rawStackScalars={{0,0,2,1,128,0x00ff00ffu}};
        at(0x129014);h.loaderReceiver=owner->resources58[index];at(0x129018);
        if(!state.handle40||!h.payloadBytes||!h.side||h.loaderReceiver!=h.texture.get())return false;
        h.loaderHandle40=state.handle40;at(0x12901c);at(0x129020);at(0x129024);at(0x129028);
        if(h.vtable08!=0x3d21c0u)return false;
        at(0x12902c);at(0x129030);at(0x129034);h.rawLoaderEntered=true;
        at(0x24f2c0);
        if(!initializeLoader(*h.loaderFormat48,*h.rawFormat29E640,false))return false;
        at(0x24f404);at(0x24f408);at(0x24f40c);at(0x24f410);
        at(0x24f414);at(0x29e910);const auto firstBits=sourceSkyFormatBits29E910(*h.rawFormat29E640);
        at(0x24f41c);at(0x24f420);at(0x29e910);const auto secondBits=sourceSkyFormatBits29E910(*h.loaderFormat3C);
        at(0x24f428);at(0x24f42c);
        h.loaderMaximumBits=firstBits>secondBits?firstBits:secondBits;
        at(0x24f430);at(0x24f434);
        if(!sourceLoadApplication_||sourceLoadApplication_!=sourceTrackMountApplication_||
           sourceLoadApplication_->entities()!=worldApplication_||h.texture->manager_!=sourceLoadApplication_->textureLifetimeOwner_)return false;
        h.loaderBufferManager=sourceLoadApplication_->textureLifetimeOwner_;
        at(0x24f438);at(0x24f43c);at(0x24f440);at(0x24f444);at(0x24f448);at(0x24f44c);
        h.loaderBufferBytes=(*h.side)*(*h.side)*(*h.loaderMaximumBits)/8u;
        std::uint32_t bufferPC=0x29dfd0;
        if(!h.loaderBufferManager->sourceBufferCallNative(0,*h.loaderBufferBytes,&h,h.loaderBuffers[0],bufferPC)){at(bufferPC);return false;}
        at(0x24f450);at(0x24f454);at(0x24f458);at(0x24f45c);
        if(!h.loaderBuffers[0].borrowed){at(0x24f460);at(0x24f464);h.loaderNullBufferExit=true;at(0x25000c);return false;}
        at(0x24f468);at(0x29e910);at(0x24f470);at(0x24f474);at(0x29e910);at(0x24f47c);at(0x24f480);
        at(0x24f484);at(0x24f488);
        if(h.loaderBufferManager!=sourceLoadApplication_->textureLifetimeOwner_||h.texture->issuer_!=sourceLoadApplication_)return false;
        at(0x24f48c);at(0x24f490);at(0x24f494);at(0x24f498);at(0x24f49c);at(0x24f4a0);at(0x24f4a4);
        bufferPC=0x29e050;
        if(!h.loaderBufferManager->sourceBufferCallNative(1,*h.loaderBufferBytes+4u,&h,h.loaderBuffers[1],bufferPC)){at(bufferPC);return false;}
        at(0x24f4a8);at(0x24f4ac);at(0x24f4b0);at(0x24f4b4);
        if(!h.loaderBuffers[1].borrowed){at(0x24f4b8);at(0x24f4bc);h.loaderNullBufferExit=true;at(0x250010);return false;}
        at(0x24f4c0);at(0x24f4c4);at(0x29e560);
        // Full switch true set is16..30,32,34,36. Raw caller excludes31..36.
        h.payloadExtendedFormat=*h.loaderFormat48>=16&&*h.loaderFormat48<=30;h.payloadBranchSelected=true;
        at(0x24f4cc);at(0x24f4d0);
        if(h.payloadExtendedFormat){
            at(0x24f4d4);at(0x24f4d8);at(0x24f4dc);at(0x24f4e0);
            // Admitted raw loader proves F8bit0=0. This is the original
            // virtual+24 allocation/configuration call with A1=NULL, not a
            // successful file read or a substitute buffer38 data pointer.
            if(!(h.loaderF8KnownZero&1u))return false;
            at(0x24f4e4);at(0x24f4ec);at(0x24f4f0);at(0x24f4fc);
            if(h.loaderPaletteBorrow!=reg.paletteBorrow)return false;
            at(0x24f504);at(0x24f508);h.payloadVirtualBorrow=nullptr;
            at(0x24f50c);at(0x24f510);at(0x24f518);at(0x24f51c);at(0x24f520);
            at(0x24f528);at(0x24f530);at(0x24f538);at(0x24f53c);at(0x24f540);
            if(h.vtable08!=0x3d21c0u)return false;
            at(0x24f544);at(0x24f548);at(0x24f54c);h.payloadPendingReturnPC=0x24f550u;h.payloadVirtualEntered=true;
            at(0x250ee0);return continueSourceSkyPayload250EE0(pc,occurrence);
        }
        at(0x24fc2c);at(0x24fc30);at(0x24fc34);at(0x24fc38);
        at(0x24ff58);at(0x29e5b0);h.payloadFileBits=sourceSkyFileBits29E5B0(*h.loaderFormat48);
        at(0x24ff60);at(0x24ff64);at(0x24ff68);at(0x24ff6c);
        if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
        at(0x24ff70);at(0x24ff74);at(0x24ff78);at(0x24ff7c);at(0x24ff80);
        h.payloadReadBytes=(*h.side)*(*h.side)*(*h.payloadFileBits)/8u;
        auto& buffer=h.loaderBufferManager->sourceBuffers_[1];
        if(!h.loaderBufferManager->sourceBufferCurrentNative(1,&h,h.loaderBuffers[1])||*h.payloadReadBytes>buffer.capacity)return false;
        h.payloadKnownPrefixBefore=buffer.knownPrefix;at(0x24ff84);at(0x24ff88);at(0x24ff8c);a::OriginalStandardTextureDiagnostic50LA d;
        const bool returned=sourceSkyFile_->readItemResourcePayload(occurrence,h.loaderBuffers[1].borrowed,*h.payloadReadBytes,d);
        const auto& read=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->rawPayload;
        h.payloadReadAttempted=read.attempted;h.payloadReadReturned=read.returned;h.payloadCopied=read.copied;h.payloadReadItems=read.items;
        if(read.copied>buffer.knownPrefix)buffer.knownPrefix=read.copied;
        if(!returned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
        at(0x24ff90);at(0x24ff94);
        if(h.payloadReadItems!=h.payloadReadBytes){at(0x24ff98);at(0x24ff9c);h.payloadShortExit=true;at(0x25000c);return false;}
        // Genuine virtual+24 receives buffer38, same side/format/palette borrow,
        // and original scalar arguments. No retain occurs before its body.
        at(0x24ffa0);at(0x24ffa8);h.payloadVirtualBorrow=h.loaderBuffers[1].borrowed;
        at(0x24fffc);if(h.vtable08!=0x3d21c0u)return false;
        at(0x250000);at(0x250004);at(0x250008);h.payloadPendingReturnPC=0x25000cu;h.payloadVirtualEntered=true;
        at(0x250ee0);return continueSourceSkyPayload250EE0(pc,occurrence);
    };
    at(0x128d68);at(0x128d6c);at(0x128d74);if(!word(0x50+group*0x100,h.groupWord50))return false;
    //For actual entry0..15 the pair of original arithmetic shifts followed
    //by AND3 is exactly this unsigned two-bit extraction, including bit31.
    at(0x128db8);h.sizeCode=(*h.groupWord50>>(2*entry))&3u;
    h.side=*h.sizeCode==0?512u:*h.sizeCode==1?64u:*h.sizeCode==2?128u:256u;
    at(0x128e24);if(!word(0x48,h.format48))return false;at(0x128e28);at(0x29e510);
    const auto format=*h.format48;h.compressed=sourceSkyFormat29E510(format);
    at(0x128e30);at(0x128e34);
    if(h.compressed){at(0x128e38);at(0x128e3c);
        if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
        at(0x128e40);at(0x128e4c);a::OriginalStandardTextureDiagnostic50LA d;
        const bool returned=sourceSkyFile_->readItemResourceSize(occurrence,d);const auto& read=sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->resourceSizeA8;
        h.sizeReadAttempted=read.attempted;h.sizeReadReturned=read.returned;h.sizeCopied=read.copied;h.sizeReadItems=read.items;
        if(read.copied==4)h.payloadBytes=std::uint32_t(read.bytes[0])|(std::uint32_t(read.bytes[1])<<8)|(std::uint32_t(read.bytes[2])<<16)|(std::uint32_t(read.bytes[3])<<24);
        if(!returned){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
        at(0x128e54);at(0x128e58); //Original ignores short source read; unknown stackA8 suffix stays unknown.
    }else {at(0x128e5c);at(0x29e480);
        h.bytesPerPixel=sourceSkyFormat29E480(format);at(0x128e64);at(0x128e68);
        h.payloadBytes=(*h.side)*(*h.side)*(*h.bytesPerPixel);at(0x128e6c);at(0x128e70);
    }
    //Native metadata owns its link before the actual guest allocation. A
    //metadata refusal is not a guest allocator NULL result.
    try{h.texture.reset(new a::OriginalSourceTextureLifetimeFacet50LA);}catch(...){return false;}
    at(0x128e74);at(0x128e78);h.allocationAttempted=true;
    try{h.texture->storage_.reset(new std::uint8_t[0x210]);}catch(...){h.texture->storage_.reset();}
    h.allocationReturned=true;h.allocationNonNull=bool(h.texture->storage_);at(0x128e7c);at(0x128e80);
    if(!h.texture->storage_)return publishAndEnterLoader();
    at(0x128e84);at(0x128e88);at(0x128e8c);at(0x24e1b0);h.constructorEntered=true;
    const auto store=[&](std::size_t off,std::uint64_t value,std::size_t size){for(std::size_t k=0;k<size;++k){h.texture->storage_[off+k]=static_cast<std::uint8_t>(value>>(k*8));h.texture->known_[off+k]=true;}};
    at(0x24e1bc);at(0x29c6e0);at(0x29c6f4);at(0x1049c0);at(0x1049cc);at(0x1048f0);
    at(0x1048f8);store(8,0x3cc980u,4);at(0x104900);store(0,1,8);h.reference00=1;h.baseReferenceAcquired=true;
    at(0x1049dc);store(8,0x3cc770u,4);at(0x1049e0);store(0x10,0,4);at(0x1049e4);store(0x14,0,4);at(0x1049e8);store(0x18,0,4);
    at(0x29c710);store(8,0x3d3790u,4);h.vtable08=0x3d3790u;
    at(0x29c718);store(0xa0,0,4);at(0x29c720);store(0xa4,0,4);at(0x29c728);store(0xa8,1,4);at(0x29c730);store(0xac,0xffffffffu,4);
    for(std::size_t off=0xb0;off<=0xc0;off+=4){at(0x29c738+static_cast<std::uint32_t>((off-0xb0)*2));store(off,0,4);}
    at(0x29c760);at(0x29c764);store(0x20,0,1);at(0x29c768);at(0x29c76c);
    if(!sourceLoadApplication_||sourceLoadApplication_!=sourceTrackMountApplication_||sourceLoadApplication_->entities()!=worldApplication_||
       !sourceLoadApplication_->textureLifetimeOwner_)return false;
    if(!h.texture->capture443870(sourceLoadApplication_->textureLifetimeOwner_,sourceLoadApplication_))return false;h.managerRead=true;
    at(0x29c770);at(0x29c774);at(0x29c778);at(0x29c77c);at(0x29c780);at(0x29c784);
    std::uint32_t registrationPC=0x29c780;
    if(!h.texture->register29C780(registrationPC)){at(registrationPC);return false;}h.registrationReturned=true;
    at(0x29c788);at(0x29c78c);at(0x29c790);at(0x29c794);at(0x29c798);h.base29C6E0Returned=true;
    at(0x24e1c4);at(0x24e1c8);at(0x24e1cc);at(0x24e1d0);
    store(8,0x3d21c0u,4);h.vtable08=0x3d21c0u;h.derivedTableStored=true;
    at(0x24e1d4);at(0x24e1d8);at(0x24e1dc);at(0x257740);store(0xfc,0,4);
    at(0x257744);at(0x257748);h.embeddedFCConstructed=true;
    at(0x24e1e0);at(0x24e1f0);store(0xe8,0xffffffffu,4);
    // Five genuine level records, stride0x34. Unknown initial130 is not
    // read as a host byte: source clears only bits1 then0; all other bits
    // remain unknown and the byte-level known flag deliberately stays false.
    for(std::size_t j=0;j<5;++j){const auto d=j*0x34;
        at(0x24e218);store(0x100+d,0,4);
        at(0x24e224);store(0x104+d,0xffffffffu,4);at(0x24e230);store(0x110+d,0xffffffffu,4);
        at(0x24e23c);store(0x124+d,0xffffu,2);
        at(0x24e248);store(0x108+d,0xffffffffu,4);at(0x24e254);store(0x10c+d,0xffffffffu,4);
        at(0x24e260);store(0x114+d,0xffffffffu,4);at(0x24e26c);store(0x11c+d,0xffffffffu,4);
        at(0x24e278);store(0x118+d,0,4);at(0x24e284);store(0x120+d,0xffffffffu,4);
        at(0x24e290);at(0x24e29c);h.level130KnownZero[j]=2;
        at(0x24e2a8);at(0x24e2b4);h.level130KnownZero[j]|=1;
        at(0x24e2c0);store(0x12c+d,0xffffffffu,4);at(0x24e2d0);at(0x24e2d4);store(0x128+d,0xffffffffu,4);
        ++h.derivedLevels;
    }
    at(0x24e2ec);store(0x204,0,4);at(0x24e2f4);store(0x208,0,4);
    at(0x24e2fc);store(0xf4,0xffffffffu,4);at(0x24e304);store(0xec,0xffffffffu,4);at(0x24e30c);store(0xf0,0xffffffffu,4);
    at(0x24e314);at(0x24e320);h.flagsF8KnownZero=1;
    at(0x24e328);at(0x24e334);h.flagsF8KnownZero|=2;
    at(0x24e33c);at(0x24e348);h.flagsF8KnownZero|=4;
    at(0x24e350);at(0x24e35c);h.flagsF8KnownZero|=8;
    at(0x24e360);at(0x24e364);at(0x24e368);at(0x24e36c);h.derivedConstructorReturned=true;
    return publishAndEnterLoader();
}
bool Owner::continueSourceSkyResourceDriver(std::uint32_t& pc,std::size_t occurrence,std::size_t group,std::size_t entry)noexcept {
    if(sourceSkyResourceDriverActive_)return false;
    struct ActiveScope{bool& active;explicit ActiveScope(bool& value):active(value){active=true;}~ActiveScope(){active=false;}} scope(sourceSkyResourceDriverActive_);
    for(;;){
        if(pc!=0x128d50u||occurrence>sourceSkyLaterItems_.size())return false;
        (void)continueSourceSkyResourcePrefix(pc,occurrence,group,entry);
        auto& reg=sourceItemHistory(occurrence).registration;
        if(pc!=0x129168u||!reg.pendingResource||!reg.pendingResource->resourceSourceReturned)return false;
        const bool exhausted=continueSourceSkyRegistration129168(pc,occurrence);
        if(exhausted)return pc==0x129188u&&reg.resourceScanExhausted&&reg.completedEntries==96&&!reg.pendingResource;
        if(pc!=0x128d50u||reg.pendingResource||reg.resourceCalls.empty()||!reg.resourceCalls.back()||reg.resourceCalls.back()->bodyEntered)return false;
        const auto index=reg.resourceCalls.back()->index;group=index/16;entry=index%16;
        //Constant native stack depth, independent of selected-mask population.
    }
}
bool Owner::continueSourceSkyRegistration129168(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(pc!=0x129168u||occurrence>sourceSkyLaterItems_.size()||!preflightSourcePostParticleModelsNative())return false;
    auto& reg=sourceItemHistory(occurrence).registration;auto& owner=sourceItemOwner(occurrence);
    if(!reg.pendingResource||!owner||!reg.pendingResource->resourceSourceReturned||reg.pendingResource->resourceSourceResult!=1u||
       reg.completedResources.size()>=96||reg.pendingResource->fileOrdinal!=reg.completedResources.size())return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    //Move only custody, never reset source history or replay originalitemseek.
    //Vector allocation failure preserves the pending owner and return receipt.
    try{reg.completedResources.push_back(std::move(reg.pendingResource));}catch(...){return false;}
    at(0x129168);++reg.completedEntries;
    for(;;){at(0x12916c);at(0x129170);at(0x129174);
        if(reg.completedEntries%16==0){at(0x129178);at(0x12917c);at(0x129180);at(0x129184);
            if(reg.completedEntries==96){reg.resourceScanExhausted=true;at(0x129188);return true;} //Only actually exhausted masks authorize the epilogue.
            at(0x129128);at(0x12912c);}
        const auto index=reg.completedEntries,group=index/16,entry=index%16,off=0x54+group*0x100;
        at(0x129130);if(!owner->known[off]||!owner->known[off+1])return false;
        const auto mask=std::uint16_t(owner->storage[off])|(std::uint16_t(owner->storage[off+1])<<8);reg.masks[index]=static_cast<std::uint16_t>(mask);
        at(0x129134);at(0x129138);at(0x12913c);at(0x129140);
        if(!(mask&(1u<<entry))){at(0x129144);++reg.completedEntries;continue;}
        at(0x129148);const auto slot=0x58+group*0x100+entry*4;
        //All slots were actually constructorNULL; a new nonNULL alias without
        //retained occurrence proof is a currentness refusal, not a skipped bit.
        for(std::size_t k=0;k<4;++k)if(!owner->known[slot+k]||owner->storage[slot+k])return false;
        if(owner->resources58[index])return false;
        at(0x12914c);at(0x129150);at(0x129154);at(0x129158);at(0x12915c);at(0x129160);at(0x129164);
        if(!sourceSkyFile_||!sourceSkyFile_->current()||!sourceSkyFile_->expectedCursor_)return false;
        if(reg.resourceCalls.size()+1!=reg.completedResources.size())return false;
        try{auto call=std::unique_ptr<SourceSkyResourceCall>(new SourceSkyResourceCall);call->index=index;call->cursor=*sourceSkyFile_->expectedCursor_;call->entered=true;reg.resourceCalls.push_back(std::move(call));}catch(...){return false;}
        at(0x128d50);return false; //Iterative driver opens the distinct next file/body occurrence.
    }
}
bool Owner::continueSourceSkyResourceTail129054(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(pc!=0x129054u||occurrence>sourceSkyLaterItems_.size()||!preflightSourcePostParticleModelsNative())return false;
    auto& state=sourceItemHistory(occurrence);auto& reg=state.registration;auto& owner=sourceItemOwner(occurrence);
    if(!reg.pendingResource||!owner||!sourceSkyFile_)return false;auto& h=*reg.pendingResource;
    if(h.resourceTailEntered||!h.slot58Stored||(h.allocationNonNull?!h.postLoaderVirtualReturned:!h.allocationReturned))return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x129054);if(!owner->known[0x4c]||!owner->known[0x4d])return false;
    h.resourceTailFlags4C=std::uint16_t(owner->storage[0x4c])|(std::uint16_t(owner->storage[0x4d])<<8);h.resourceTailEntered=true;
    at(0x129058);at(0x12905c);at(0x129060);
    if(*h.resourceTailFlags4C&2u){at(0x129064);if(!state.handle40||*state.handle40!=sourceSkyFile_->handle_||(h.loaderHandle40&&h.loaderHandle40!=state.handle40))return false;
        at(0x129068);at(0x12906c);if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;}
    a::OriginalStandardTextureDiagnostic50LA d;
    if(!sourceSkyFile_->alignResource129054(occurrence,*h.resourceTailFlags4C,d)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    if(!sourceSkyFile_->resourceFileOccurrence(occurrence,h.fileOrdinal)->resourceAlignmentReturned){
        at(0x12909c);at(0x1290a0);if(!sourceTrackMountDomain_||sourceTrackMountDomain_->resolver3E1A70()!=sourceSkyFile_->resolver_)return false;
        at(0x1290a4);at(0x1290a8);at(0x1290ac);at(0x1290b0);at(0x1290b4);
        if(!sourceSkyFile_->seekResourceAlignment1290B0(occurrence,d)){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;}
    }
    h.resourceAlignmentReturned=true;at(0x1290b8);at(0x1290bc);at(0x1290c0);at(0x1290c4);at(0x1290c8);at(0x1290cc);
    at(0x1290d0);h.resourceSourceResult=1;at(0x1290d4);at(0x1290d8);h.resourceSourceReturned=true;
    at(0x129168);return false; //Iterative caller owns registration advancement.
}
bool Owner::continueSourceSkyEmbedded257750(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(pc!=0x257750u||occurrence>sourceSkyLaterItems_.size()||!preflightSourcePostParticleModelsNative())return false;
    auto& reg=sourceItemHistory(occurrence).registration;if(!reg.pendingResource)return false;auto& h=*reg.pendingResource;
    if(!h.texture||!h.texture->storage_||h.embedded||h.embeddedFCGuardReturned||h.embeddedFCCapacityArgument!=10||
       !(h.formatNormalized||h.compressedFinalTellReturned))return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    std::optional<bool> fcNull=true;for(std::size_t k=0;k<4;++k){if(!h.texture->known_[0xfc+k]){fcNull.reset();break;}if(h.texture->storage_[0xfc+k])fcNull=false;}
    std::uint32_t fcPC=0x257760;
    if(!a::OriginalConfigProcessGlobals50LA::checkEmbeddedFC257750(fcNull,fcPC)){at(fcPC);return false;}
    h.embeddedFCGuardReturned=true;at(fcPC);
    // Native metadata is distinct from actual nullable0x20 guest allocation.
    try{h.embedded.reset(new SourceSkyEmbeddedFC);}catch(...){return false;}auto& e=*h.embedded;
    if(!a::OriginalConfigProcessGlobals50LA::descriptorDelete2E8B00AdmittedNative())return false;
    at(0x2577d8);at(0x2577dc);e.allocationAttempted=true;
    try{e.object20.reset(new std::uint8_t[0x20]);}catch(...){e.object20.reset();}e.allocationReturned=true;
    at(0x2577e0);at(0x2577e4);
    if(e.object20){at(0x2577e8);at(0x2577ec);at(0x2577f0);at(0x2577f4);at(0x2592d0);e.constructorEntered=true;
        // Actual fixed argument10: signed rounded(10+7)/8=2, bytes2<<7,
        // count2<<3. Native aligned custody represents uncached alias mode1
        // without OR-ing host pointers with an EE address-region bit.
        at(0x2592e8);at(0x259308);e.bytes=256;at(0x259310);e.roundedCount=16;
        at(0x259314);at(0x259318);
        if(!a::OriginalConfigProcessGlobals50LA::descriptorDelete2E8B00AdmittedNative())return false;
        e.alignedAttempted=true;
        try{e.alignedBacking.reset(new std::uint8_t[256+63]);}catch(...){e.alignedBacking.reset();}e.alignedReturned=true;
        if(e.alignedBacking){const auto base=reinterpret_cast<std::uintptr_t>(e.alignedBacking.get());
            const auto offset=static_cast<std::size_t>((64-(base%64))%64);e.data00=e.alignedBacking.get()+offset;}
        at(0x25931c);at(0x259324);at(0x259328);at(0x25932c);at(0x259330);at(0x259338);e.uncachedAliasSelected=true;
        at(0x259340);at(0x259348);at(0x25934c);
        if(!e.data00)return false; //Source NULL allocation becomes EE0x20000000 alias; no safe native memset/forged ctor return.
        for(std::size_t k=0;k<256;++k)e.data00[k]=0;e.zeroed=true;
        at(0x259358);for(std::size_t k=0;k<4;++k){e.object20[8+k]=static_cast<std::uint8_t>(16u>>(k*8));e.known[8+k]=true;}
        at(0x259354);at(0x2593f0);at(0x2593f4);e.cursor04=e.data00;
        for(std::size_t off=0xc;off<=0x1c;off+=4){at(off==0x1c?0x25940cu:0x2593f8u+static_cast<std::uint32_t>(off-0xc));
            for(std::size_t k=0;k<4;++k){e.object20[off+k]=0;e.known[off+k]=true;}}
        at(0x259408);at(0x25940c);at(0x25935c);at(0x259374);at(0x259378);e.constructorReturned=true;
    }
    at(0x2577f8);h.embeddedFCPublished=e.object20?h.embedded.get():nullptr;e.published=true;
    for(std::size_t k=0;k<4;++k){if(h.embeddedFCPublished)h.texture->known_[0xfc+k]=false;
        else {h.texture->storage_[0xfc+k]=0;h.texture->known_[0xfc+k]=true;}}
    at(0x2577fc);at(0x25780c);at(0x257810);h.embeddedCallReturned=true;
    return true;
}
bool Owner::continueSourceSkyPayload250EE0(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(pc!=0x250ee0u||occurrence>sourceSkyLaterItems_.size()||!preflightSourcePostParticleModelsNative())return false;
    auto& reg=sourceItemHistory(occurrence).registration;
    if(!reg.pendingResource)return false;auto& h=*reg.pendingResource;
    if(!h.payloadVirtualEntered||h.payloadFieldsStored||!h.side||!h.rawFormat29E640||!h.texture||!h.texture->storage_)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x250ee0);
    const auto put=[&](std::size_t off,std::uint32_t value){for(std::size_t k=0;k<4;++k){h.texture->storage_[off+k]=static_cast<std::uint8_t>(value>>(k*8));h.texture->known_[off+k]=true;}};
    // Caller24FFAC reloads T1 from loader stack168, whose actual incoming
    // T0 was1 at128FF8. This is min-width A8, not format/side substitution.
    at(0x250f10);put(0x14,*h.side);at(0x250f14);put(0x18,*h.side);at(0x250f18);put(0xa8,1);h.payloadFieldsStored=true;
    at(0x250f1c);at(0x250f3c);at(0x250f40);h.resourcePaletteB4=h.loaderPaletteBorrow;
    for(std::size_t k=0;k<4;++k){if(h.resourcePaletteB4)h.texture->known_[0xb4+k]=false;
        else {h.texture->storage_[0xb4+k]=0;h.texture->known_[0xb4+k]=true;}}
    h.paletteB4Stored=true; // Actual raw pointer store; no104960/AddRef in this prefix.
    const auto format=*h.rawFormat29E640;
    if(format==0x22b8u||format==0x115cu){at(0x250f9c);at(0x250fa8);h.normalizedFormatS1=0x22b8u;at(0x250fb8);}
    else if(format==0x613u||format==0x235u||format==0x22bu){at(0x250f7c);at(0x250f88);h.normalizedFormatS1=0x613u;at(0x250f98);}
    else {at(0x250f78);return false;} //Actual unsupported-format process guard/false assertion, not invented normalization.
    h.loaderF8KnownZero&=static_cast<std::uint8_t>(~4u);h.loaderF8KnownOne|=4u;h.formatNormalized=true;
    at(0x251024);at(0x251028);at(0x25102c);h.embeddedFCCapacityArgument=10;at(0x257750);
    if(!continueSourceSkyEmbedded257750(pc,occurrence))return false;
    at(0x251030);at(0x251034);at(0x29e910);h.resourceCurrentE8=sourceSkyFormatBits29E910(*h.normalizedFormatS1);
    at(0x251038);put(0xe8,*h.resourceCurrentE8);at(0x251040);put(0xac,*h.normalizedFormatS1);at(0x251044);put(0xb0,1);h.resourceFormatFieldsStored=true;
    at(0x251048);at(0x25104c);at(0x251050);at(0x251054);
    // Payload stackB8 is original stack08=0; bit2 clear, no mip-count loop.
    if(h.rawStackScalars[0]!=0)return false;
    at(0x251098);at(0x25109c);at(0x2510a0);
    for(std::size_t k=0;k<4;++k)if(!h.texture->known_[0x100+k]||h.texture->storage_[0x100+k])return false;
    at(0x2510a4);at(0x2510a8);h.levelAllocationEntered=true;at(0x250040);at(0x250060);at(0x250064);
    return false; //Next actual process442F70 backend owner; no prepared renderer/context or GPU readiness.
}
bool Owner::continueSourceSkyPostRegistration129890(std::uint32_t& pc)noexcept {
    if(pc!=0x129890u||!preflightSourcePostParticleModelsNative()||!sourceSkyOwner_)return false;
    auto& post=sourceSkyPostItems_;const auto& items=sourceSkyItems129758_;
    if(post.tailEntered||!items.count7D8||post.indexS4!=*items.count7D8)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    at(0x129890);post.tailEntered=true;post.index40Stored=true; //Actual sky index40=0, not filehandle40 of an item.
    at(0x129894);at(0x129898);at(0x12989c);at(0x1298a0);at(0x1298a4);at(0x1298a8);
    //Original unconditionally dereferences array[0], even count0. Keep that
    //NULL/out-of-bounds source point explicit, never invent an empty success.
    if(!*items.count7D8||!sourceSkyPublishedItems3C_||!sourceSkyItemSlots3C_)return false;
    auto* selected=sourceSkyItemSlots3C_[0];post.firstSlotRead=true;
    if(selected!=sourceItemOwner(0).get())return false;
    at(0x1298ac);at(0x1298b0);sourceSkySelected38_=selected;sourceSkySelectedItemOccurrence_=0;post.firstSlotSelected=true;
    at(0x12a120);post.packetCallEntered=true;at(0x12a148);at(0x12a14c);at(0x12a150);at(0x12a154);at(0x12a158);at(0x305500);
    if(!a::OriginalConfigProcessGlobals50LA::descriptorDelete2E8B00AdmittedNative())return false;
    post.packetAllocationAttempted=true;
    try{post.packetBacking.reset(new std::uint8_t[0x340+63]);}catch(...){post.packetBacking.reset();}
    if(post.packetBacking){const auto base=reinterpret_cast<std::uintptr_t>(post.packetBacking.get());post.packet340=post.packetBacking.get()+(64-base%64)%64;}
    post.packetAllocationReturned=true;at(0x12a15c);at(0x12a160);at(0x12a164);at(0x12a168);at(0x12a16c);at(0x306478);
    if(!post.packet340)return false; //ActualNULL memset is not native-safe; no invented source failure return.
    for(std::size_t k=0;k<0x340;++k)post.packet340[k]=0;post.packetZeroed=true;
    const auto word=[&](std::size_t off,std::uint32_t value){for(std::size_t k=0;k<4;++k)post.packet340[off+k]=static_cast<std::uint8_t>(value>>(k*8));};
    at(0x12a170);at(0x12a174);word(0x10,0x11000000);at(0x12a178);at(0x12a17c);word(0x14,0x03000000);
    at(0x12a180);at(0x12a184);at(0x12a188);word(0x18,0x02000000);at(0x12a18c);at(0x12a190);at(0x12a194);at(0x12a198);word(0x1c,0x6c028320);
    at(0x12a19c);for(std::size_t k=0x20;k<0x30;++k)post.packet340[k]=0;
    at(0x12a1a0);at(0x12a1a4);at(0x12a1a8);at(0x12a1ac);at(0x12a1b0);at(0x12a1b4);at(0x12a1b8);at(0x12a1bc);post.packet340[0x27]&=0xf3;
    at(0x12a1c0);at(0x12a1c4);at(0x12a1c8);at(0x12a1cc);at(0x12a1d0);at(0x12a1d4);at(0x12a1d8);post.packet340[0x21]=static_cast<std::uint8_t>((post.packet340[0x21]&0x7f)|0x80);
    at(0x12a1dc);at(0x12a1e0);at(0x12a1e4);at(0x12a1e8);at(0x12a1ec);at(0x12a1f0);at(0x12a1f4);at(0x12a1f8);post.packet340[0x27]=static_cast<std::uint8_t>((post.packet340[0x27]&0x0f)|0x10);
    at(0x12a1fc);at(0x12a200);at(0x12a204);at(0x12a208);at(0x12a20c);at(0x12a210);at(0x12a214);at(0x12a218);post.packet340[0x20]=1;post.packet340[0x21]&=0x80;
    at(0x12a21c);at(0x12a220);at(0x12a224);at(0x12a228);at(0x12a22c);at(0x12a230);at(0x12a234);at(0x12a238);at(0x12a23c);post.packet340[0x28]=static_cast<std::uint8_t>((post.packet340[0x28]&0xf0)|0x0e);
    at(0x12a240);word(0x30,0x0005000c);word(0x34,0);at(0x12a244);word(0x38,0x47);word(0x3c,0);
    at(0x12a248);word(0x40,0x14000000);at(0x12a24c);word(0x4c,0x6c2a8000);
    at(0x12a250);word(0x50,0x3f800000);at(0x12a254);word(0x64,0x3f800000);at(0x12a258);word(0x78,0x3f800000);post.packetHeaderStored=true;
    // Each original load reloads sky38. Do not borrow a prepared sky item or
    // consume an unknown suffix merely because the original read reported one item.
    constexpr std::uint32_t receiverPC[3]{0x12a25cu,0x12a28cu,0x12a2b0u};
    constexpr std::uint32_t loadPC[3]{0x12a274u,0x12a2a4u,0x12a2b4u};
    constexpr std::uint32_t storePC[3]{0x12a288u,0x12a2acu,0x12a2bcu};
    for(std::size_t lane=0;lane<3;++lane){
        at(receiverPC[lane]);
        if(!sourceSkySelected38_||!sourceSkySelectedItemOccurrence_||*sourceSkySelectedItemOccurrence_!=0||
           *sourceSkySelected38_!=sourceItemOwner(0).get())return false;
        const auto* actual=*sourceSkySelected38_;
        at(loadPC[lane]);if(!actual||!actual->storage)return false;
        std::uint32_t bits=0;
        for(std::size_t k=0;k<4;++k){if(!actual->known[0x10+4*lane+k])return false;
            bits|=std::uint32_t(actual->storage[0x10+4*lane+k])<<(8*k);}
        // Intervening instructions only establish integer masks/constants and
        // local packet aliases; LWC1/SWC1 performs no floating-point arithmetic.
        at(storePC[lane]);word(0x80+4*lane,bits);post.packetItemWords[lane]=bits;
        ++post.packetItemLanesStored;
    }
    // t7=3F800000 from12A22C; t6=42FE0000 from12A230.
    constexpr std::size_t oneOffsets[]{0x8c,0x190,0x1a4,0x1b8,0x1cc};
    for(std::size_t k=0;k<5;++k){at(0x12a2c0u+static_cast<std::uint32_t>(4*k));word(oneOffsets[k],0x3f800000u);}
    constexpr std::size_t colorOffsets[]{0x1e8,0x1e4,0x1e0,0x1f8,0x1f4,0x1f0,0x208,0x204,0x200,0x218,0x214,0x210};
    for(std::size_t k=0;k<12;++k){at(0x12a2d4u+static_cast<std::uint32_t>(4*k));word(colorOffsets[k],0x42fe0000u);}
    post.packetMatrixStored=true;
    // Original private packet tag construction. The source integer masks are
    // applied to freshly zeroed quadwords, never to prepared renderer data.
    at(0x12a304);for(std::size_t k=0x270;k<0x280;++k)post.packet340[k]=0;
    at(0x12a318);post.packet340[0x271]=0x80;
    at(0x12a328);post.packet340[0x275]=0x40;
    // DSLL/DSLL32 at12A26C/278/27C establish FC007FFFFFFFFFFF and29<<47.
    at(0x12a338);std::uint64_t tag=0;
    for(std::size_t k=0;k<8;++k)tag|=std::uint64_t(post.packet340[0x270+k])<<(8*k);
    tag=(tag&UINT64_C(0xfc007fffffffffff))|(UINT64_C(29)<<47);
    for(std::size_t k=0;k<8;++k)post.packet340[0x270+k]=static_cast<std::uint8_t>(tag>>(8*k));
    at(0x12a348);post.packet340[0x277]&=0xf3;
    at(0x12a358);post.packet340[0x277]=static_cast<std::uint8_t>((post.packet340[0x277]&0x0f)|0x30);
    at(0x12a368);post.packet340[0x278]=static_cast<std::uint8_t>((post.packet340[0x278]&0xf0)|2);
    at(0x12a378);post.packet340[0x278]=static_cast<std::uint8_t>((post.packet340[0x278]&0x0f)|0x10);
    at(0x12a38c);post.packet340[0x279]=static_cast<std::uint8_t>((post.packet340[0x279]&0xf0)|4);
    at(0x12a3a0);post.packet340[0x270]=5;post.packet340[0x271]&=0x80;
    // Two independently executed zero/tag blocks at280 and2A0.
    constexpr std::uint32_t starts[]{0x12a3a4u,0x12a40cu};
    constexpr std::size_t offsets[]{0x280,0x2a0};
    for(std::size_t i=0;i<2;++i){const auto off=offsets[i];const auto start=starts[i];
        at(start);for(std::size_t k=0;k<16;++k)post.packet340[off+k]=0;
        at(start+0x10);post.packet340[off]=1;
        at(start+0x20);post.packet340[off+1]=0x80;
        at(start+0x30);post.packet340[off+5]=0x40;
        at(start+0x40);post.packet340[off+7]&=0xf3;
        at(start+0x50);post.packet340[off+7]=static_cast<std::uint8_t>((post.packet340[off+7]&0x0f)|0x10);
        at(start+0x60);post.packet340[off+8]=static_cast<std::uint8_t>((post.packet340[off+8]&0xf0)|0x0f);
        at(start+0x64);word(off+0x1c,0x7f);
    }
    constexpr std::size_t zeroOffsets[]{0x2cc,0x2c8,0x2c4,0x2c0,0x2dc,0x2d8,0x2d4};
    for(std::size_t k=0;k<7;++k){at(0x12a474u+static_cast<std::uint32_t>(4*k));word(zeroOffsets[k],0);}
    at(0x12a494);word(0x2d0,0);at(0x12a498);word(0x2e0,6);at(0x12a49c);word(0x2ec,0);
    at(0x12a4a0);word(0x2fc,0x6c02802b);at(0x12a4a4);word(0x320,0x14000010);
    at(0x12a4ac);word(0x324,0x10000000);at(0x12a4b0);word(0x328,0x030000b5);at(0x12a4b4);word(0x32c,0x02000190);
    at(0x12a4bc);post.packet340[0]=0x33;post.packet340[1]=0;
    at(0x12a4c4);post.packet340[2]=0;at(0x12a4c8);post.packet340[3]=0x30;post.packetTagsStored=true;
    // S1 was packet+10 at12A2B8. No address encoding or extra ownership is invented.
    at(0x12a4cc);post.packetData04=post.packet340+0x10;post.packetPointerStored=true;
    at(0x12a4d0);word(8,0);at(0x12a4d4);word(0x0c,0);
    // POR V0,S0,zero at12A30C retained the allocation base; the register-only
    // epilogue does not replace it or dispatch/execute the packet.
    for(std::uint32_t p=0x12a4d8;p<=0x12a504;p+=4)at(p);
    post.packetReturned=true;
    at(0x1298b4);post.packet7E0=post.packet340;post.packetPublished7E0=true;
    at(0x1298b8);post.rowsEntered=true;post.rowIndexS4=0;at(0x1298bc);post.rowOffsetS2=0;
    // Original loop interleaves each30 constructor with12A510. Do not allocate
    // all96 up front or advance the count before that helper genuinely returns.
    at(0x1298c0);
    if(!a::OriginalConfigProcessGlobals50LA::descriptorDelete2E8B00AdmittedNative())return false;
    try{post.rows[post.rowIndexS4].reset(new SourceSkyPacketRow);}catch(...){return false;}
    auto& row=*post.rows[post.rowIndexS4];row.allocationAttempted=true;
    at(0x1298c4);at(0x2e8d90);
    try{row.object30.reset(new std::uint8_t[0x30]);}catch(...){row.object30.reset();}
    row.allocationReturned=true;at(0x1298c8);at(0x1298cc);
    if(row.object30){
        at(0x1298d0);at(0x1298d4);at(0x133f80);
        const auto store=[&](std::size_t off,std::uint32_t value){for(std::size_t k=0;k<4;++k){row.object30[off+k]=static_cast<std::uint8_t>(value>>(8*k));row.known[off+k]=true;}};
        at(0x133f84);store(0x2c,0xffffffffu);at(0x133f88);store(0x24,0);
        at(0x133f8c);store(0x28,0);at(0x133f90);store(0x20,0);
        at(0x133f94);at(0x133f98);row.constructorReturned=true; //POR returnsTHIS; no refcount/vtable writes.
    }
    at(0x1298d8);post.rows7E8[post.rowIndexS4]=row.object30.get();row.slot7E8Stored=true;
    at(0x1298dc);at(0x1298e0);at(0x1298e4);
    if(!post.rows7E8[post.rowIndexS4]){at(0x129960);return false;} //Real error destructor path remains bounded, not fake success.
    at(0x1298e8);at(0x1298ec);row.helper12A510Entered=true;
    at(0x12a510);at(0x12a520);row.packetSkyThis=sourceSkyOwner_.get();
    at(0x12a524);at(0x12a52c);at(0x12a530);at(0x12a534);at(0x305500);
    if(!row.packetSkyThis||!a::OriginalConfigProcessGlobals50LA::descriptorDelete2E8B00AdmittedNative())return false;
    row.packetAllocationAttempted=true;
    try{row.packetBacking.reset(new std::uint8_t[0xc0+63]);}catch(...){row.packetBacking.reset();}
    if(row.packetBacking){const auto base=reinterpret_cast<std::uintptr_t>(row.packetBacking.get());row.packetC0=row.packetBacking.get()+(64-base%64)%64;}
    row.packetAllocationReturned=true;
    at(0x12a538);at(0x12a53c);at(0x12a540);at(0x12a544);at(0x12a548);at(0x306478);
    if(!row.packetC0)return false; //Actual NULL memset is unsafe, not a sourceNULL return.
    for(std::size_t k=0;k<0xc0;++k)row.packetC0[k]=0;row.packetZeroed=true;
    const auto packetWord=[&](std::size_t off,std::uint32_t value){for(std::size_t k=0;k<4;++k)row.packetC0[off+k]=static_cast<std::uint8_t>(value>>(8*k));};
    at(0x12a550);packetWord(0x10,0x11000000);at(0x12a558);packetWord(0x14,0x03000000);
    at(0x12a560);packetWord(0x18,0x02000000);at(0x12a56c);packetWord(0x1c,0x6c038320);
    at(0x12a570);for(std::size_t k=0x20;k<0x30;++k)row.packetC0[k]=0;
    at(0x12a590);row.packetC0[0x27]&=0xf3;
    at(0x12a5ac);row.packetC0[0x21]=static_cast<std::uint8_t>((row.packetC0[0x21]&0x7f)|0x80);
    at(0x12a5c8);row.packetC0[0x27]=static_cast<std::uint8_t>((row.packetC0[0x27]&0x0f)|0x20);
    at(0x12a5e8);row.packetC0[0x20]=1;row.packetC0[0x21]&=0x80;
    at(0x12a5f8);row.packetC0[0x28]=static_cast<std::uint8_t>((row.packetC0[0x28]&0xf0)|0x0e);
    at(0x12a608);row.packetC0[0x28]=static_cast<std::uint8_t>((row.packetC0[0x28]&0x0f)|0xe0);
    at(0x12a60c);packetWord(0x38,6);packetWord(0x3c,0);row.packetPrefixStored=true;
    at(0x12a610);return false; //Process443A60 Video owner/getters are not yet joined; no prepared renderer substitute.
}
bool Owner::continueSourceSkyPostItems(std::uint32_t& pc)noexcept {
    auto& post=sourceSkyPostItems_;const auto& root=sourceSkyItems129758_;
    if(pc!=0x129858u||post.entered||!root.count7D8||!preflightSourcePostParticleModelsNative()||!sourceSkyOwner_||!sourceSkyFile_)return false;
    const auto count=*root.count7D8;
    if(count){if(sourceSkyLaterItems_.size()+1!=count)return false;
        for(std::size_t i=0;i<count;++i)if(!sourceItemHistory(i).slotStored||!sourceItemLoop(i).restoreReturned||sourceItemLoop(i).advancedIndexS4!=i+1u)return false;}
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    const auto resolverCurrent=[&](){return sourceTrackMountDomain_&&sourceTrackMountDomain_->resolver3E1A70()==sourceSkyFile_->resolver_;};
    at(0x129858);post.entered=true;post.indexS4=0;at(0x12985c);at(0x129860);post.offsetS2=0;
    for(;;){at(0x129880);if(!preflightSourcePostParticleModelsNative())return false;
        const auto observed=sourceSkyOwner_->count7D8;at(0x129884);at(0x129888);at(0x12988c);
        if(post.indexS4>=observed){at(0x129890);return continueSourceSkyPostRegistration129890(pc);}
        const auto occurrence=static_cast<std::size_t>(post.indexS4);auto& state=sourceItemHistory(occurrence);auto& owner=sourceItemOwner(occurrence);auto& reg=state.registration;
        at(0x129864);at(0x129868);at(0x12986c);
        if(!sourceSkyItemSlots3C_||sourceSkyItemSlots3C_[occurrence]!=owner.get()||!state.slotStored||reg.entered)return false;
        at(0x129870);at(0x129874);at(0x12b750);reg.entered=true;at(0x12b758);
        sourceSkySelected38_=owner.get();sourceSkySelectedItemOccurrence_=occurrence;reg.field38Stored=true;
        at(0x12b75c);
        if(sourceSkyPalette7D0_){reg.paletteBorrow=*sourceSkyPalette7D0_;reg.paletteOccurrence=sourceSkyPublishedPaletteOccurrence_;}
        else {if(sourceSkyConstructor288A50_.nullFields[7]!=0u)return false;reg.paletteBorrow=nullptr;}
        reg.paletteRead=true;at(0x12b760);at(0x12b764);at(0x1290e0);at(0x129100);
        if(!state.dataPosition650)return false;at(0x129104);at(0x129108);at(0x12910c);at(0x129110);at(0x129114);if(!resolverCurrent())return false;
        at(0x129118);at(0x12911c);a::OriginalStandardTextureDiagnostic50LA d;
        const bool sought=sourceSkyFile_->seekItemResource(occurrence,d);
        if(occurrence<sourceSkyFile_->itemResourceSeeks_.size()){reg.seekAttempted=sourceSkyFile_->itemResourceSeeks_[occurrence]->attempted;reg.seekReturned=sourceSkyFile_->itemResourceSeeks_[occurrence]->returned;}
        if(!sought){at(d.sourcePC?d.sourcePC:sourceSkyFile_->pc_);return false;} //Native admission only; source ignores successfulseekV0.
        at(0x129120);at(0x129124);
        for(std::size_t group=0;group<6;++group){at(0x129128);at(0x12912c);
            for(std::size_t entry=0;entry<16;++entry){const auto index=group*16+entry,off=0x54+group*0x100;
                at(0x129130);if(!owner->known[off]||!owner->known[off+1])return false;
                const auto mask=std::uint16_t(owner->storage[off])|(std::uint16_t(owner->storage[off+1])<<8);reg.masks[index]=static_cast<std::uint16_t>(mask);
                at(0x129134);at(0x129138);at(0x12913c);at(0x129140);
                if(mask&(1u<<entry)){at(0x129148);const auto pointerOff=0x58+group*0x100+entry*4;
                    for(std::size_t k=0;k<4;++k)if(!owner->known[pointerOff+k]||owner->storage[pointerOff+k])return false;
                    at(0x12914c);at(0x129150);at(0x129154);at(0x129158);at(0x12915c);at(0x129160);at(0x129164);at(0x128d50);
                    if(!continueSourceSkyResourceDriver(pc,occurrence,group,entry))return false;
                    goto sourceSkyResourceScanFinished;
                }
                at(0x129144);++reg.completedEntries;at(0x12916c);at(0x129170);at(0x129174);
            }
            at(0x129178);at(0x12917c);at(0x129180);at(0x129184);
        }
        reg.resourceScanExhausted=true;
sourceSkyResourceScanFinished:
        if(reg.completedEntries!=96||reg.pendingResource||!reg.resourceScanExhausted)return false;
        at(0x129188);at(0x12918c);at(0x129190);at(0x129194);at(0x129198);at(0x12919c);at(0x1291a0);at(0x1291a4);at(0x1291a8);at(0x1291ac);at(0x12b768);at(0x12b76c);reg.returned=true;reg.sourceResult=1;
        at(0x12b770);at(0x12b774);at(0x129878);post.offsetS2+=4;at(0x12987c);++post.indexS4;
    }
}
bool Owner::continueSourceSkyItemLoop(std::uint32_t& pc,std::size_t occurrence)noexcept {
    if(sourceSkyItemDriverActive_)return false;
    struct ActiveScope {bool& active;explicit ActiveScope(bool& value):active(value){active=true;}~ActiveScope(){active=false;}} scope(sourceSkyItemDriverActive_);
    for(;;){
        if(pc!=0x1297c8u||!sourceSkyFile_||!sourceSkyItems129758_.count7D8||occurrence>=*sourceSkyItems129758_.count7D8||
           occurrence!=sourceSkyFile_->followingItems_.size()||!sourceSkyFile_->itemOccurrence(occurrence)||
           !sourceSkyFile_->itemOccurrence(occurrence)->seekReturned||!preflightSourcePostParticleModelsNative())return false;
        if(occurrence){
            if(occurrence!=sourceSkyLaterItems_.size()+1)return false;
            //Ledger refusal precedes guest allocation, preserving every earlier
            //slot/parser and this directory occurrence without a fake NULL.
            try{sourceSkyLaterItems_.push_back(std::unique_ptr<SourceSkyLaterItem>(new SourceSkyLaterItem));}catch(...){return false;}
        }else if(!sourceSkyLaterItems_.empty())return false;
        (void)continueSourceSkyItemBody(pc,occurrence);
        const auto& state=sourceItemHistory(occurrence);
        if(pc!=0x12982cu||!state.parserReturned||!state.slotStored)return false;
        (void)continueSourceSkyItemRestore(pc,occurrence);
        const auto& loop=sourceItemLoop(occurrence);
        if(pc!=0x1297c8u||!loop.restoreReturned||!loop.nextDirectoryEntered||
           loop.advancedIndexS4!=occurrence+1u||sourceSkyFile_->followingItems_.size()!=occurrence+1u||
           !sourceSkyFile_->itemOccurrence(occurrence+1)->seekReturned)return false;
        ++occurrence; //Native stack depth is independent of authored item count.
    }
}

bool Owner::withdrawSourceParticleEmitterBorrowsNative()noexcept {
    if(!preflightSourceWorldParticleDescriptorsNative())return false;
    if(!nativeParticleList3D8_)return true;
    //Validate all live pins before any mutation. This operation is native
    //disposal only; no source NULL stores/destructor/return receipts change.
    for(const auto& row:nativeParticleList3D8_->rows)if(row)for(const auto& e:row->sourceEmitters)if(e&&!e->borrowsWithdrawn){
        if(e->managerStored&&(!e->manager70||e->manager70!=trackParticles_.get()||!e->manager70->sourceParticleEmitterBorrowers_))return false;
        if(e->modelStored&&(!e->modelOwner||!e->model74||!e->modelOwner->sourceParticleEmitterBorrowers_||
           e->modelOwner->model()!=e->model74))return false;
    }
    //Close each real partial Config occurrence before model/manager pins.
    //abortNative owns the exact shared numeric handle; not source1573B0.
    for(auto& row:nativeParticleList3D8_->rows)if(row)for(auto& e:row->sourceEmitters)if(e&&e->config15AE00&&!e->configNativeWithdrawn){
        a::OriginalStandardTextureDiagnostic50LA d;
        if(!e->config15AE00->abortNative(d))return false;e->configNativeWithdrawn=true;
    }
    //Native cancellation removes registered links before count-owner release;
    //retain sourceRegistered/mask history and never claim a guest detach call.
    for(auto& row:nativeParticleList3D8_->rows)if(row)for(auto& e:row->sourceEmitters)
        if(e&&e->sourceRegistered&&!e->nativeTopologyWithdrawn){
            const auto detached=worldApplication_->entityAuthority_.detach(e->entity,worldApplication_->attachmentLock423F58());
            if(detached.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
            e->nativeTopologyWithdrawn=true;
        }
    //The pending row call also borrows matched model/manager operands by
    //identity. Withdraw that native call frame before those owners retire.
    for(auto& next:sourceParticleNextRows_)if(next.entered){
        next.row=nullptr;next.descriptor=nullptr;next.nativeWithdrawn=true;
    }
    for(auto& row:nativeParticleList3D8_->rows)if(row)for(auto& e:row->sourceEmitters)if(e&&!e->borrowsWithdrawn){
        if(e->modelStored)--e->modelOwner->sourceParticleEmitterBorrowers_;
        if(e->managerStored)--e->manager70->sourceParticleEmitterBorrowers_;
        e->modelOwner=nullptr;e->model74=nullptr;e->manager70=nullptr;e->borrowsWithdrawn=true;
    }
    return true;
}
bool Owner::continueSourceCollisionDispatch1AA244(std::uint32_t& pc)noexcept {
    auto& h=sourceCollisionDispatch128_;
    if(pc!=0x1aa244||h.stage||!sourceFinishLineCallerReturn_.progressSkipped||
       !preflightSourcePostParticleModelsNative()||!worldColdConstructed_||
       !sourceLoadRace_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       !sourceLoadApplication_||sourceLoadApplication_->entities()!=worldApplication_||
       sceneApplication_!=worldApplication_||worldEntity_.reference00!=1)return false;
    const auto at=[&](std::uint32_t p){pc=sourcePostParticleModelsPC_=p;};
    //Actual1B2870 constructor calls26FC90 then stores3CF110 at+08 at1B2898.
    //This same most-derived cold World is retained in Race74; native owner
    //identity carries the immutable dispatch table, not a copied guest pointer.
    at(0x1aa244);at(0x1aa248);h.worldTable08=0x3cf110u;h.stage=1;
    at(0x1aa24c);h.target128=0x2743b0u;h.stage=2; //Original word3CF238.
    at(0x1aa250);at(0x1aa254);at(0x2743b0); //JALR/NOP; reserve40 and save caller frame.
    at(0x2743c4);
    //Genuine earlier1B4EC0/27BCA0 source owners only. No legacy decoded
    //candidates, modelPrefixes, runtimeQueries, or gameplay viewport admission.
    if(sourceCollisionPhase_!=SourceCollisionPhase::returned||!preflightSourceWorldCollisionNative()||
       !worldCollisionObjects3CC_||!worldCollisionObjects3CC_->sourceMode_||
       !worldCollisionObjects3CC_->sourceArrayReturned_||
       !sourceCollisionObjects3CC_||*sourceCollisionObjects3CC_!=worldCollisionObjects3CC_->sourceArray3CC_.get()||
       !*sourceCollisionObjects3CC_||sourceCollisionSucceeded_!=10)return false;
    auto& objects=*worldCollisionObjects3CC_;h.stage=3;
    at(0x2743c8);at(0x2743cc); //Nonnull branch; S2 is this same World.
    at(0x2743d8);h.zoneS0=0;at(0x2743dc);at(0x2743e0);h.zoneOffsetS1=0;
    if(!sourceStandardWorldReturnV0_||*sourceStandardWorldReturnV0_!=1||
       !nativeModelZone354_||*nativeModelZone354_!=9)return false;
    h.previousModelZone354=nativeModelZone354_;
    for(;;){
    at(0x274430);if(!worldZones34C_)return false;h.liveZoneCount34C=*worldZones34C_;
    at(0x274434);const bool moreZones=static_cast<std::int32_t>(h.zoneS0)<static_cast<std::int32_t>(*h.liveZoneCount34C);
    at(0x274438);at(0x27443c);
    if(!moreZones){
        at(0x274440);at(0x274444);at(0x274448);at(0x27444c);at(0x274450);
        at(0x274454);at(0x274458);h.worldReturned=true;at(0x1aa258);return true;
    }
    at(0x2743e4);if(h.zoneS0>=10)return false; //Existing ten-group owner capacity, not a substituted loop bound.
    nativeModelZone354_=h.zoneS0;h.storedZone354=h.zoneS0;h.zone354Stored=true;h.stage=4;
    at(0x2743e8);at(0x2743ec);at(0x2743f0);
    const auto z=h.zoneS0;
    if(objects.sourceArrayKnown_<=z||!objects.sourceZones_[z]||
       (*sourceCollisionObjects3CC_)[z]!=&objects.sourceZones_[z]->entity)return false;
    auto& group=*objects.sourceZones_[z];at(0x2743f4);
    if(!group.sourceMode)return false;
    h.groupCount30=group.count30;h.stage=5;
    at(0x2743f8);at(0x2743fc);
    if(static_cast<std::int32_t>(group.count30)<=0){
        at(0x274400);at(0x274404);at(0x274408);at(0x27440c);
        if(h.groupLogAttempted[z])return false;
        //Original argument is S0 (zero-based), unlike positive-group +1 log.
        char message[]="Zone0 does not contain any collision objects\n";message[4]=char('0'+z);
        h.groupLogAttempted[z]=true;h.groupLogWritten[z]=std::fwrite(message,1,sizeof(message)-1,stdout);
        h.emptyGroupsSkipped[z]=true; //No27BEA0 call/return and no callbacks.
        at(0x274410);at(0x274414);h.zoneOffsetS1+=4u;
        at(0x27442c);++h.zoneS0;continue;
    }
    at(0x274418);at(0x27441c);at(0x274420);at(0x27bea0);
    at(0x27beb8);at(0x27bebc);at(0x27bec0);h.slotS0=0;
    at(0x27bec4);at(0x27bec8);h.slotOffsetS1=0;h.stage=6;
    for(;;){
    at(0x27bef0);h.groupCount30=group.count30; //Reload live count on every edge.
    at(0x27bef4);const bool more=static_cast<std::int32_t>(h.slotS0)<static_cast<std::int32_t>(*h.groupCount30);
    at(0x27bef8);at(0x27befc);if(!more)break;
    h.callbackReturned=false;h.callbackReturnV0.reset();h.loopCountReloaded=false;
    h.wrapperTable0C.reset();h.target0C.reset();h.stage=6;
    at(0x27becc);if(!group.sourceEntries34)return false;
    at(0x27bed0);at(0x27bed4);
    if(group.sourceEntriesKnown<=h.slotS0||objects.sourceWrappers_[z].size()<=h.slotS0)return false;
    auto* wrapper=group.sourceEntries34[h.slotS0];
    if(!wrapper||wrapper!=objects.sourceWrappers_[z][h.slotS0].get()||!wrapper->sourceMode||
       !wrapper->sourceTypeKnown||(wrapper->type00!=1&&wrapper->type00!=2)||!wrapper->sourceDescriptorPublished||
       !wrapper->descriptor08||!wrapper->descriptor08->sourceFields||
       !wrapper->descriptor08->sourceFields->loaded)return false;
    at(0x27bed8);if(wrapper->sourceVtable0C!=0x3cf0f0u)return false;
    h.wrapperTable0C=wrapper->sourceVtable0C;h.stage=7;
    at(0x27bedc);h.target0C=0x1b5590u;h.stage=8; //Original word3CF0FC.
    at(0x27bee0);at(0x27bee4); //JALR; delay A1=World, A0=actual wrapper.
    at(0x1b5590);
    if(g::OriginalWorldCollisionObjects50LA::returnSourceSkippedCallback1B5590(
        *wrapper,*worldApplication_,worldEntity_.sessionGeneration,pc)){
        h.callbackReturnV0=wrapper->sourceCallback.returnV0;h.callbackReturned=true;h.stage=9;
        at(0x27bee8u);h.slotOffsetS1+=4u;at(0x27beecu);++h.slotS0;
        ++h.callbacksReturned[z];h.loopCountReloaded=true;h.stage=10;continue;
    }
    //Completed A may own detached links. Never admit B if a constructor is
    //already suspended: old-link currentness deliberately rejects that cohort.
    //With this retained allocator,280890 reserves before entry and has no
    //fallible post-entry allocation. No arbitrary suspended-cohort unwind is claimed.
    const auto& shared=worldApplication_->garageObserverShared_;
    const auto* root=shared.sourceObserverGroup_;
    if(wrapper->sourceCallback.entered||!root||shared.sourceObserver280890_||
       !shared.sourceGroupCurrent(root)||!shared.ownsReturnedSourceObserver(root)||
       !shared.allocator443138_||shared.allocator443138_.get()!=shared.sourceGroupRetainedAllocator_||
       shared.observers443140_==std::numeric_limits<std::uint32_t>::max())return false;
    //Same process Application's immutable3C81A0-produced sentinel. No Physics
    //BASE alias, cached query/candidate, or decoded model prefix substitutes.
    const auto& sentinel=worldApplication_->collisionStartup3C81A0_.vectors3E02C0[0];
    g::OriginalWorldCollisionObjects50LA::beginSourceCallback1B5590(*wrapper,*worldApplication_,
        worldEntity_.sessionGeneration,sentinel,pc);
    if(pc==0x11ba2cu&&sourceLoadApplication_&&sourceLoadApplication_->entities()==worldApplication_&&
       sceneApplication_==worldApplication_&&sourceTrackMountDomain_&&
       sourceLoadApplication_->standardMaterialDomainOwner_==sourceTrackMountDomain_&&
       !sourceTrackMountLease_.closed_&&sourceTrackMountDomain_->prepared()&&sourceTrackFile_){
        const auto resolver=sourceTrackMountDomain_->resolver3E1A70();
        if(resolver)g::OriginalWorldCollisionObjects50LA::continueSourceCallbackSearchNative(
            *wrapper,*worldApplication_,worldEntity_.sessionGeneration,resolver,sourceTrackFile_,pc);
    }
    if(pc==0x11bbb8u&&g::OriginalWorldCollisionObjects50LA::observeSourceLoaderReturn1B5730(
        *wrapper,*worldApplication_,worldEntity_.sessionGeneration,pc)){
        //Actual World354 and World3C4 producers, never a model/query facade.
        if(nativeModelZone354_&&*nativeModelZone354_<10&&sourceStaticOwner_&&
           sourceModelParents3C4_&&sourceModels3C4_&&*sourceModels3C4_==sourceModelParents3C4_.get()&&
           sourceModelParentsPublished3C4_==10&&sourceStaticOwner_->application_==worldApplication_&&
           sourceStaticOwner_->generation_==worldEntity_.sessionGeneration&&
           sourceStaticOwner_->preflightSourceModelParentLinksNative()){
            const auto z=*nativeModelZone354_;
            if(z<sourceStaticOwner_->parents_.size())if(auto& parent=sourceStaticOwner_->parents_[z]){
                if(parent->sourceMode&&parent->constructed&&!parent->released&&parent->zone==z+1&&
                   parent->sourceAuthorityEnrolled&&parent->sourceVtable08==0x3d2640u&&
                   sourceModelParents3C4_[z]==&parent->entity)
                    g::OriginalWorldCollisionObjects50LA::attachSourceCallback1B577C(*wrapper,*worldApplication_,
                        worldEntity_.sessionGeneration,parent->entity,sourceStaticOwner_->sourceCollisionTailNative_[z],z,pc);
            }
        }
    }
    if(pc==0x1b5784u)g::OriginalWorldCollisionObjects50LA::transformSourceCallback1B5784(
        *wrapper,*worldApplication_,worldEntity_.sessionGeneration,pc);
    if(pc==0x11f680u)g::OriginalWorldCollisionObjects50LA::firstSourceSetter11F680(
        *wrapper,*worldApplication_,worldEntity_.sessionGeneration,pc);
    if(pc==0x1b5884u)g::OriginalWorldCollisionObjects50LA::secondSourceSetter1B5888(
        *wrapper,*worldApplication_,worldEntity_.sessionGeneration,pc);
    if(pc==0x1b5890u){
        //Original1B5890/94/98/9C/A0/A4 use this live World, current354,
        //actual4C4 array, and the same wrapper04. Never BodyB8 or visibility4C0.
        at(0x1b5890u);
        if(!nativeModelZone354_||*nativeModelZone354_>=10)return false;
        const auto z=*nativeModelZone354_;at(0x1b5894u);
        if(!sourceTopologyMode_||sourceTopologyWithdrawn_||!sourceCollision4C4_||
           !sourceCollisionSlots4C4_||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||
           z>=sourceCollisionPublished_)return false;
        at(0x1b5898u);
        if(!wrapper->sourceCallback.outer||wrapper->sourceCallback.source04!=wrapper->sourceCallback.outer.get())return false;
        at(0x1b589cu);at(0x1b58a0u);at(0x1b58a4u);
        const auto& node=sourceSearchNodes_[1][z];
        if(!node.owner||node.withdrawn||!node.published||!node.ordinalKnown||
           sourceCollisionSlots4C4_[z]!=node.owner.get()||!preflightSourceSearchNodeNative(node,z,true))return false;
        g::OriginalWorldCollisionObjects50LA::beginSourceMembership1B58A8(*wrapper,*worldApplication_,
            worldEntity_.sessionGeneration,node.owner.get(),z,pc);
    }
    if(pc==0x2809c0u){
        auto& m=wrapper->sourceCallback.membership;
        const auto actualMethod=[&](const void* actualZone,const void* registrationOccurrence,
            const void* actualCOL,NativeListMethodCustody*& method,std::optional<std::uint32_t>& slot,
            bool& selectionPending,std::uint32_t& methodPC,bool select)noexcept {
            methodPC=select?0x280afcu:0x27f5a0u;
            if(registrationOccurrence!=&m||actualCOL!=wrapper->sourceCallback.outer.get()||
               actualZone!=m.zoneIdentity||m.mode!=1||m.zone354>=10||!nativeModelZone354_||*nativeModelZone354_!=m.zone354||
               !sourceTopologyMode_||sourceTopologyWithdrawn_||!sourceCollision4C4_||!sourceCollisionSlots4C4_||
               *sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||m.zone354>=sourceCollisionPublished_)return false;
            const auto zone=m.zone354;auto& node=sourceSearchNodes_[1][zone];
            if(node.withdrawn||!node.published||!node.owner||!node.ordinalKnown||
               sourceCollisionSlots4C4_[zone]!=node.owner.get()||actualZone!=node.owner.get()||
               !preflightSourceSearchNodeNative(node,zone,true))return false;
            //Actual mode1 27F5A0 selects static collision +34 / World4CC.
            if(!select)method=node.owner->nativeList34;
            if(method!=node.owner->nativeList34||!method||!nativeMethods4CC_||
               nativeMethods4CC_[zone]!=method||!sourceListMethodArrayCurrent(1,zone,method))return false;
            const auto& publication=sourceListMethods_[1][zone];
            if(!publication.arrayPublished||!publication.aliasPublished||!publication.aliasRetained||
               publication.withdrawn||publication.aliasNode!=node.owner.get()||
               !method->sourceMode_||method->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
               method->application_!=worldApplication_.get()||method->ordinal_!=10u+zone||
               method->usage10_!=1||method->reference00_!=2||
               !method->source_.constructorReturned||method->source_.vtable08!=0x3d2d30u||
               !sourceListEntriesCurrent(*method))return false;
            if(!select)return true;
            //Actual3D2D30+20=27E3B0 tailcalls27E3C0 with a3(NULL)
            //as requested slot. +24=27E570 is owned by COL insertion.
            return selectSourceObserverSlot27E3C0(*method,registrationOccurrence,actualCOL,
                slot,selectionPending,methodPC,1u);
        };
        const auto actualPortal=[&](unsigned phase,const void* registration,const void* actualCOL,
            const void* actualZone,const std::array<std::uint32_t,4>& center,const void* cursor,
            auto& edge,std::shared_ptr<const void>& graphPin,std::uint32_t& portalPC)noexcept {
            portalPC=phase==0?0x280c34u:phase==2?0x280c90u:phase==4?0x280cf8u:0x280c40u;
            if(phase>4||registration!=&m||actualCOL!=wrapper->sourceCallback.outer.get()||
               actualZone!=m.zoneIdentity||center!=m.center||m.mode!=1||m.zone354>=10||
               !nativeModelZone354_||*nativeModelZone354_!=m.zone354||!sourceTopologyMode_||sourceTopologyWithdrawn_||
               !sourceCollision4C4_||!sourceCollisionSlots4C4_||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||
               m.zone354>=sourceCollisionPublished_)return false;
            const auto zone=m.zone354;auto& root=sourceSearchNodes_[1][zone];
            if(root.withdrawn||!root.published||!root.owner||actualZone!=root.owner.get()||
               sourceCollisionSlots4C4_[zone]!=root.owner.get()||
               !preflightSourceSearchNodeNative(root,zone,true)||
               !sourcePortalWorldLinksCurrent(*root.owner,true,zone))return false;
            if(phase==0){
                if(graphPin||cursor)return false;
                for(const auto& old:sourceCollisionPortalBorrows_)
                    if(old.registration==registration&&!old.token.expired())return false;
                //Allocate native custody before exposing the source head pointer.
                //Weak root records never keep a completed COL borrower alive.
                try{std::shared_ptr<const void> token=std::make_shared<std::uint8_t>(0);
                    sourceCollisionPortalBorrows_.push_back({registration,token});graphPin=std::move(token);
                }catch(...){return false;}
                edge.next=root.owner->portalLinks30.get();return true;
            }
            bool pinned=false;
            for(const auto& borrow:sourceCollisionPortalBorrows_)if(borrow.registration==registration){
                const auto live=borrow.token.lock();if(live&&live==graphPin){pinned=true;break;}}
            if(!pinned||!graphPin||!cursor)return false;
            std::size_t index=0;while(index<40&&sourcePortalWorldLinks_[index/4][index%4].published!=cursor)++index;
            //Odd links are collision namespace (stages1/3), never visibility0/2.
            if(index==40||!(index&1u))return false;
            auto& link=sourcePortalWorldLinks_[index/4][index%4];
            if(link.withdrawn||link.knownMask!=31||!link.published||link.from!=root.owner.get()||!link.to)return false;
            auto& raw=*link.published;const auto& portal=nativePortals_[index/4];
            if(!portal||!portal->sourceMode||raw.neighbor04!=&link.to->entity||
               raw.portal08!=&portal->entity||!portal->source.loadReturned||
               portal->source.normalKnown!=7||!portal->source.normalNormalized||portal->source.copiedVertices!=4)return false;
            for(unsigned i=0;i<4;++i)if(portal->source.edgeKnown[i]!=7||!portal->source.edgeNormalized[i])return false;
            if(phase==1){
                edge.entered=true;edge.edge=cursor;edge.edgeIndex=index;
                edge.neighbor=raw.neighbor04;edge.portal=raw.portal08;
                //Actual231150 uses center and f12=+0, not half/radius80.
                portalPC=0x231150u;
                const bool overlapOkay=g::source_portal_overlap231150::run(portal->source.vertices30,portal->source.normal70,
                    portal->source.edges80,center,0u,edge.overlap);
                edge.overlapReturned=edge.overlap.returned;edge.hit=edge.overlap.hit;edge.distance=edge.overlap.outputDistance;
                if(!overlapOkay){portalPC=edge.overlap.pc;return false;}
                portalPC=0x280c54u;if(!edge.hit)return true;
                if(!edge.distance)return false;
                portalPC=0x280c60u;raw.field10=*edge.distance;link.distance10=edge.distance;edge.distanceStoreMask|=1u;
                portalPC=0x280c64u;raw.field0C=*edge.distance;link.distance0C=edge.distance;edge.distanceStoreMask|=2u;
            }else if(edge.edge!=cursor||edge.edgeIndex!=index||edge.neighbor!=raw.neighbor04||edge.portal!=raw.portal08||
                     !g::source_portal_overlap231150::historyCurrent(edge.overlap))return false;
            if(phase==4){edge.next=raw.next00.get();portalPC=0x280cf8u;return true;}
            if(phase==3&&!edge.hit)return true;
            //The source keeps a method cursor: each hit reads its usage10,
            //then resolves the neighbor method using that same mode1.
            portalPC=phase==1?0x280c6cu:portalPC;
            const auto* prior=edge.previousMethod;
            if(!prior||!prior->sourceMode_||prior->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
               prior->application_!=worldApplication_.get()||prior->usage10_!=1||prior->ordinal_<10||prior->ordinal_>=20||
               !sourceListMethodArrayCurrent(1,prior->ordinal_-10,prior))return false;
            std::size_t neighborZone=0;while(neighborZone<10&&sourceCollisionSlots4C4_[neighborZone]!=link.to)++neighborZone;
            if(neighborZone==10||neighborZone>=sourceCollisionPublished_)return false;
            auto& neighbor=sourceSearchNodes_[1][neighborZone];
            if(neighbor.withdrawn||!neighbor.published||neighbor.owner.get()!=link.to||
               !preflightSourceSearchNodeNative(neighbor,neighborZone,true))return false;
            if(phase==1){portalPC=0x27f5a0u;edge.method=link.to->nativeList34;edge.methodReturned=true;}
            auto* method=edge.method;
            if(!method||method!=link.to->nativeList34||!nativeMethods4CC_||nativeMethods4CC_[neighborZone]!=method||
               !sourceListMethodArrayCurrent(1,neighborZone,method)||method->usage10_!=1||method->reference00_!=2||
               !method->sourceMode_||method->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
               method->application_!=worldApplication_.get()||method->ordinal_!=10u+neighborZone||
               !method->source_.constructorReturned||method->source_.vtable08!=0x3d2d30u||
               !sourceListEntriesCurrent(*method))return false;
            const auto& publication=sourceListMethods_[1][neighborZone];
            if(!publication.arrayPublished||!publication.aliasPublished||!publication.aliasRetained||
               publication.withdrawn||publication.aliasNode!=link.to)return false;
            if(phase==2){portalPC=0x280c90u;
                if(!edge.overlap.returned||!edge.overlap.hit||edge.distanceStoreMask!=3u||!edge.methodReturned)return false;
                return selectSourceObserverSlot27E3C0(*method,&edge,actualCOL,
                    edge.slot,edge.selectionPending,portalPC,1u);}
            return phase==1||phase==3;
        };
        g::OriginalWorldCollisionObjects50LA::continueSourceStaticMembershipNative(
            wrapper->sourceCallback,static_cast<const ra_pod_scene50LA::Owner*>(this),actualMethod,actualPortal,pc);
    }
    if(pc==0x120224u&&g::OriginalWorldCollisionObjects50LA::finishSourceCallback1B58B0(
        *wrapper,*worldApplication_,worldEntity_.sessionGeneration,pc)){
        h.callbackReturnV0=wrapper->sourceCallback.returnV0;h.callbackReturned=true;h.stage=9;
        //27BEA0 ignores callback V0; advance only after its genuine return.
        at(0x27bee8u);h.slotOffsetS1+=4u;
        at(0x27beecu);++h.slotS0;
        ++h.callbacksReturned[z];h.loopCountReloaded=true;h.stage=10;
        continue; //Fresh entries34 lookup and fresh callback occurrence; never replay A.
    }
    sourcePostParticleModelsPC_=pc;return false; //Partial B remains owned, not counted as returned.
    }
    //27BF00 observes live World354 for the original diagnostic arguments.
    at(0x27bf00);if(!nativeModelZone354_||*nativeModelZone354_!=z)return false;
    at(0x27bf04);at(0x27bf08);at(0x27bf0c);at(0x27bf10);at(0x27bf14);
    if(h.groupLogAttempted[z])return false;
    std::array<char,96> message{};std::size_t length=0;
    const auto append=[&](const char* text)noexcept{while(*text)message[length++]=*text++;};
    const auto number=[&](std::uint32_t v)noexcept{char digits[10];std::size_t n=0;
        do{digits[n++]=char('0'+v%10);v/=10;}while(v);while(n)message[length++]=digits[--n];};
    append("\nLoaded ");number(h.slotS0);append(" collision objects from Zone");
    number(*nativeModelZone354_+1u);append(".\n\n");
    //Same bounded native diagnostic adapter as source group loading. Short
    //writes are recorded and ignored, never retried or treated as group failure.
    h.groupLogAttempted[z]=true;h.groupLogWritten[z]=std::fwrite(message.data(),1,length,stdout);
    at(0x27bf18);at(0x27bf1c);at(0x27bf20);at(0x27bf24);at(0x27bf28);
    at(0x27bf2c);at(0x27bf30);at(0x27bf34);h.groupsReturned[z]=true;
    at(0x274424);at(0x274428);h.zoneOffsetS1+=4u;
    at(0x27442c);++h.zoneS0;
    }
}

bool Owner::continueSourceFinishLineAnchors1B3C1C(std::uint32_t& pc)noexcept {
    auto& h=sourceFinishLineAnchors_;
    if(pc!=0x1b3c1c||h.entered||!sourceFinishLineMaterial544_||
       !sourceFinishLineHeader_.setupReturned||sourceFinishLineHeader_.borrowersWithdrawn||
       !sourceLoadApplication_||!worldApplication_||sourceLoadApplication_->entities()!=worldApplication_||
       sceneApplication_!=worldApplication_||!sourceStaticOwner_||
       sourceStaticOwner_->application_!=worldApplication_||
       sourceStaticOwner_->generation_!=worldEntity_.sessionGeneration||
       !sourceStaticOwner_->sourceLedgerPrepared_||
       !sourceStaticOwner_->canCloseSourceNative()||!preflightSourceFinishLineNative())return false;
    h.entered=true; //Once-only admission, including a bounded partial scan.
    if(!worldApplication_->readWorldOrigin4342D0(h.originWords))return false;
    pc=0x1b3c24;h.stackWords[0]=h.originWords;
    pc=0x1b3c2c;h.stackWords[1]=h.originWords;h.initialized=true;
    const auto equal=[](const std::string& left,const char* right)noexcept {
        std::size_t i=0;
        for(;;++i){
            unsigned char a=i<left.size()?static_cast<unsigned char>(left[i]):0;
            unsigned char b=static_cast<unsigned char>(right[i]);
            if(a>='A'&&a<='Z')a=static_cast<unsigned char>(a+'a'-'A');
            if(b>='A'&&b<='Z')b=static_cast<unsigned char>(b+'a'-'A');
            if(a!=b)return false;if(!a)return true;
        }
    };
    for(;;){
        //Original1B3CC0..D0 reloads World3C4[0]/parent30 on every test.
        pc=0x1b3cc0;
        if(!sourceModels3C4_||!sourceModelParents3C4_||
           *sourceModels3C4_!=sourceModelParents3C4_.get()||
           sourceStaticOwner_->parents_.empty()||!sourceStaticOwner_->parents_[0])return false;
        auto& parent=*sourceStaticOwner_->parents_[0];
        pc=0x1b3cc4;if(sourceModelParents3C4_[0]!=&parent.entity||parent.released||
           !parent.sourceMode||sourceStaticOwner_->closed_)return false;
        pc=0x1b3cc8;h.liveCount=parent.count30;
        //Native storage admits nonnegative signed counts only; never reinterpret
        //an unsupported negative count as a huge unsigned traversal.
        if(*h.liveCount>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return false;
        pc=0x1b3ccc;if(h.ordinal>=*h.liveCount)break;
        pc=0x1b3c30;
        if(!parent.sourceEntries34||h.ordinal>=parent.sourceEntriesKnown||
           h.ordinal>=sourceStaticOwner_->sourceWrapperLedger_[0].size())return false;
        pc=0x1b3c44;auto* wrapper=parent.sourceEntries34[h.ordinal];
        if(wrapper!=sourceStaticOwner_->sourceWrapperLedger_[0][h.ordinal].get())return false;
        if(!wrapper){pc=0x1b3c4c;++h.ordinal;continue;} //BEQL delay slot.
        pc=0x1b3c50;
        if(!sourceStaticOwner_->sourceModelCurrentNative(*wrapper))return false;
        //The tagged Physics+8 is a BODY, not Standard model8. Never turn the
        //native null Standard alias into an original null+8 observation.
        if(wrapper->dispatch==ra_static_model_cold50LA::Owner::StandardWrapper::Dispatch::physics3D26C0){
            const auto& physics=wrapper->sourcePhysics;
            if(!physics.body8)return false;
            if(*physics.body8){
                pc=0x1b3c5c;std::uint8_t firstByte=0;
                if(!physics.bodyOwner||!physics.bodyOwner->readSourceInlineName38FirstByteNative(
                    worldApplication_.get(),worldEntity_.sessionGeneration,*physics.body8,firstByte))return false;
                //Actual308F00 compares the inline first byte. Constructor165AF4
                //produced NUL, hence both nonempty finish_droid names mismatch.
                //Do not read unknown trailing bytes or invoke a Model getter on
                //this PhysicsObject. A future nonzero producer needs real string custody.
                if(firstByte)return false;
                pc=0x1b3c64; //Left comparison: nonzero return, no anchor getter.
                pc=0x1b3c98; //Right comparison: nonzero return, no anchor getter.
            }
        }else if(wrapper->model8){
            auto* model=wrapper->sourceModel.get();
            if(!model||wrapper->model8!=&model->scene||model->nativeRetired||model->scene.released())return false;
            const auto* scene=model->scene.model();if(!scene)return false;
            pc=0x1b3c64;int which=equal(scene->name38,"finish_droidleft.slt")?0:-1;
            if(which<0){pc=0x1b3c98;if(equal(scene->name38,"finish_droidright.slt"))which=1;}
            if(which>=0){
                pc=which?0x1b3cb0:0x1b3c7c;
                const auto index=scene->sceneIndex;
                const auto root=static_cast<std::size_t>(index);
                if(root>=model->graphCount||!model->parentAttached||!model->parentAttachReturned)return false;
                //Actual28B1A0(a1=NULL) uses this graph's real cache, then only
                //stores XYZ (28B268/270/27C). W remains the startup stack copy.
                if(!g::refreshSceneWorld49P(model->graph(),index))return false;
                for(std::size_t k=0;k<3;++k)
                    std::memcpy(&h.stackWords[which][k],&model->nodes[root].world100.v[12+k],sizeof(std::uint32_t));
                h.lastMatchingSlot[which]=h.ordinal; //Original later match overwrites earlier XYZ.
            }
        }
        pc=0x1b3cb8;++h.ordinal;
    }
    pc=0x1b3cd8;h.scanReturned=true;return true;
}

bool Owner::continueSourceFinishLineComparison1B3CD8(std::uint32_t& pc)noexcept {
    auto& c=sourceFinishLineComparison_;const auto& anchors=sourceFinishLineAnchors_;
    if(pc!=0x1b3cd8||c.entered||!anchors.scanReturned||!sourceLoadApplication_||
       !worldApplication_||sourceLoadApplication_->entities()!=worldApplication_||
       sceneApplication_!=worldApplication_||!sourceFinishLineHeader_.setupReturned||
       sourceFinishLineHeader_.borrowersWithdrawn||!sourceFinishLine540_||
       !preflightSourcePostParticleModelsNative())return false;
    c.entered=true;
    constexpr std::uint32_t loadPC[2][3]{{0x1b3ce0,0x1b3d00,0x1b3d28},{0x1b3d54,0x1b3d74,0x1b3d9c}};
    constexpr std::uint32_t comparePC[2][3]{{0x1b3ce8,0x1b3d04,0x1b3d2c},{0x1b3d5c,0x1b3d78,0x1b3da0}};
    for(unsigned side=0;side<2;++side){
        bool equal=true;
        for(unsigned lane=0;lane<3;++lane){
            pc=loadPC[side][lane];std::array<std::uint32_t,4> current;
            //Reader reauthenticates the SAME live Application publication on
            //each executed scalar load; other lanes of this copy are not receipts.
            if(!worldApplication_->readWorldOrigin4342D0(current))return false;
            c.originWords[side][lane]=current[lane];c.readMask[side]|=1u<<lane;
            float stackValue,originValue;
            std::memcpy(&stackValue,&anchors.stackWords[side][lane],sizeof(float));
            std::memcpy(&originValue,&current[lane],sizeof(float));
            pc=comparePC[side][lane];
            //C.EQ.S, not integer word equality: signed zeros equal, NaN unordered
            //and therefore false. No finite-only filter or W comparison. Host
            //FP exception flags/traps are not an emulation of original EE FCSR.
            if(!(stackValue==originValue)){equal=false;break;}
        }
        c.equalsOrigin[side]=equal;
        pc=side?0x1b3db8:0x1b3d44;
        if(equal){c.missingAnchor=true;c.returned=true;pc=0x1b3dc0;return true;}
        //Left mismatch reaches right scalar comparisons. Left all-equal skips
        //them entirely through1B3D44; no guessed anchor-count condition.
    }
    c.missingAnchor=false;c.returned=true;pc=0x1b3ddc;return true;
}

}
