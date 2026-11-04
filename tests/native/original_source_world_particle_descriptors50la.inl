#pragma once
#include "original_source_world_light_read50la.inl"
#include <cstdio>
#include <cstring>
#include <new>
#include "../../src/game/original_source_particle_random50la.h"
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldParticleDescriptorsNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceParticleDescriptorPhase_==SourceParticleDescriptorPhase::unreached)return !sourceParticleDescriptorAttempted_;
    if(sourceParticleDescriptorPhase_==SourceParticleDescriptorPhase::nativeClosed)return !nativeParticleList3D8_&&!sourceParticleList3D8_;
    if(!sourceParticleDescriptorAttempted_||!worldApplication_||worldEntity_.reference00!=1||
       runtimeWorldBorrows_||runtimeCameraBorrows_||constructorRacerWorldBorrows_||
       (sourceParticleDescriptorConfigArg_&&sourceParticleDescriptorConfigArg_!=sourceWorldObjectConfig_.get()))return false;
    if(sourceParticleDescriptorPublished_){if(!sourceParticleList3D8_||*sourceParticleList3D8_!=nativeParticleList3D8_.get())return false;}
    else if(!sourceParticleList3D8_||*sourceParticleList3D8_)return false; //Still actual resetNULL.
    if(!nativeParticleList3D8_)return true;
    const auto& l=*nativeParticleList3D8_;
    if(!l.sourceMode||l.actualOwner||l.complete||l.published||l.loaded||l.servicesCompleted||l.serviceEntered||
       l.serviceTerminal||l.serviceComplete||(l.sourceConstructorMask&~7u)||
       (l.sourceConstructorReturned&&l.sourceConstructorMask!=7u)||
       !preflightSourceParticleEntityIdentityNative(l.entity,l.counted,l.sourceEnrolled,3)||
       l.entity.parent1C||l.entity.previous10||l.entity.next14||l.entity.mask20)return false;
    //Only genuine registered emitter links may inhabit the source3D8 list.
    //Compare before dereferencing: foreign/cyclic tails never gain authority.
    const Entity* previous=nullptr;const Entity* cursor=l.entity.firstChild18;std::uint32_t descendants=0;
    for(const auto& row:l.rows)if(row)for(const auto& emitter:row->sourceEmitters)if(emitter){
        const auto& e=*emitter;
        if(e.nativeTopologyWithdrawn&&!e.sourceRegistered)return false;
        if(e.sourceRegistered&&!e.nativeTopologyWithdrawn){
            if(cursor!=&e.entity||e.entity.parent1C!=&l.entity||e.entity.previous10!=previous||e.entity.mask20!=0x402)return false;
            descendants|=e.entity.mask20|e.entity.descendants24;previous=&e.entity;cursor=e.entity.next14;
        }else if(e.entity.parent1C||e.entity.previous10||e.entity.next14)return false;
        if(e.entity.firstChild18||e.entity.descendants24||e.entity.mask20!=(e.sourceRegistered?0x402u:0u))return false;
    }
    if(cursor||l.entity.descendants24!=descendants)return false;
    if(l.sourceArrayAttempted){
        if(l.count30!=4||!l.sourceArrayPublished||!l.sourceArray34||*l.sourceArray34!=l.pointers34.get())return false;
    }else if(l.pointers34||l.sourceArrayPublished||l.sourceArray34||((l.sourceConstructorMask&2u)&&l.count30))return false;
    std::size_t liveEmitterManagerBorrows=0;
    for(std::size_t i=0;i<4;++i){const auto& r=l.rows[i];
        if(l.sourceSlotPublished[i]){if(!l.sourceRowAttempted[i]||!l.pointers34||l.pointers34[i]!=r.get())return false;}
        if(!r){if(l.sourceRowReturned[i])return false;continue;}
        if(!l.sourceRowAttempted[i]||!r->sourceMode||r->sourceConstructorMask!=7u||r->emitterPointers00||
           r->emitterPublished||r->emitterEnrolled||r->configured||r->started||r->enabled||r->attached||
           r->descriptorRetired||r->attachment74)return false;
        //Each authored row has its own genuine27C81C history. Neither a
        //previous row nor a prepared emitter can authorize this row's slots.
        const auto& service=i?sourceParticleLaterRows_[i-1]:sourceParticleService124_;
        const auto* live=r->descriptor04.get();
        const auto& history=r->sourceDescriptorHistory;
        if(r->sourceDescriptorFreed){
            if(i>=4||live||!history||!service.descriptorFreed||!service.descriptorDeleteAdmitted||
               !r->sourceDescriptor04||*r->sourceDescriptor04||!service.instances330Read||
               history->count!=*service.instances330Read||!service.attachmentOffsetE0||
               history->offset!=*service.attachmentOffsetE0||r->sourceEmitters.empty()||
               !r->sourceEmitters[0]||!r->sourceEmitters[0]->sourceRegistered)return false;
        }else if(history)return false;
        if(r->sourceRowServiceReturned!=service.rowServiceReturned&&i<4)return false;
        if(r->sourceRowServiceReturned&&!r->sourceDescriptorFreed)return false;
        const bool proof=live?(live->sourceLoaded&&(live->sourceKnownMask&128u)&&
            live->instances330&&live->startOn334&&live->sourceOffsetKnown==15):bool(history);
        const std::uint32_t proofType=live?static_cast<std::uint32_t>(live->type04):(history?history->type:0);
        const std::int32_t proofCount=live&&live->instances330?*live->instances330:(history?history->count:0);
        const std::int32_t proofStart=live&&live->startOn334?*live->startOn334:(history?history->start:0);
        const auto* proofOffset=live&&live->sourceOffsetKnown==15?&live->sourceOffset320:(history?&history->offset:nullptr);
        const bool nonFogAfterFog=bool(sourceNonFogPrecedingFogRowNative(i));
        if(nonFogAfterFog){
            const bool progressed=service.matchCountAdvanced||service.completionGuardRead||
                service.descriptorDeleteAdmitted||service.descriptorFreed||service.rowServiceReturned||
                r->sourceDescriptorFreed||r->sourceDescriptorHistory||r->sourceRowServiceReturned;
            if(progressed){
                if(service.configEmitterIndex!=0u||r->sourceEmitters.empty()||!r->sourceEmitters[0])return false;
                const auto& configured=*r->sourceEmitters[0];
                const bool loaderReturned=service.emitterType==0u?configured.type0LoaderReturned:
                    (service.emitterType==1u?configured.type1LoaderReturned:(service.emitterType==2u?configured.type2LoaderReturned:
                     (service.emitterType==9u?configured.type9LoaderReturned:(service.emitterType==11u?
                        configured.type11LoaderReturned:configured.type8LoaderReturned))));
                //Own non-Fog completion, not the preceding Fog's receipts.
                //Historical validation remains safe after Track withdrawal.
                if(!loaderReturned||!configured.emitterFileLoadReturned||!configured.configNativeWithdrawn||
                   !configured.sourceRegistered||!configured.rowConfigReturned)return false;
            }
            if(service.matchCountAdvanced&&(service.attachmentIsNone!=false||service.attachmentMatches!=1))return false;
            if(service.descriptorDeleteAdmitted&&service.attachmentIsNone==false&&
               !service.completionAssertionReturned)return false;
        }
        //Authenticate only the configured emitter0. Other allocated emitters
        //may still be constructor-only, including after an unattached return.
        if(service.emitterType==12u){
            const bool progressed=service.matchCountAdvanced||service.descriptorDeleteAdmitted||
                service.descriptorFreed||service.rowServiceReturned||r->sourceDescriptorFreed||
                r->sourceDescriptorHistory||r->sourceRowServiceReturned;
            if(progressed){
                if(service.configEmitterIndex!=0u||r->sourceEmitters.empty()||!r->sourceEmitters[0])return false;
                const auto& configured=*r->sourceEmitters[0];
                if(!configured.sourceRegistered||!configured.rowConfigReturned||
                   !configured.type12LoaderReturned||!configured.configNativeWithdrawn)return false;
                if(i>0&&(i==1?sourceParticleService124_:sourceParticleLaterRows_[i-2]).emitterType==12u){
                    const auto& population=configured.type12Population;
                    //This row owns the distinct1->2 event, never a copied
                    //prior completion. No live Track dereference after disposal.
                    if(!population.returned||population.countStep!=4||!population.countStore.stored||
                       !g::OriginalTrackParticleCold50LA::sourceFogSuccessorCountHistoryNative(population.countStore)||
                       population.countStore.emitter!=&configured||population.countStore.occurrence!=&population)return false;
                }
            }
            if(service.matchCountAdvanced&&(service.attachmentIsNone!=false||service.attachmentMatches!=1))return false;
            if(service.descriptorDeleteAdmitted&&service.attachmentIsNone==false&&
               !service.completionAssertionReturned)return false;
        }

        if(r->unknown08){
            if(i>=4||!service.rowType08Stored||!service.descriptorType04||
               service.emitterIsNone!=false||!service.descriptorRead||
               r->unknown08!=service.descriptorType04||!proof||*r->unknown08!=proofType)return false;
        }else if(i<4&&service.rowType08Stored)return false;
        if(r->sourceEmitterArrayAttempted){
            if(i>=4||!service.countPositive||!service.emitterArrayAttempted||!service.emitterType||
               !proof||proofCount<=0||
               r->sourceEmitters.size()!=static_cast<std::size_t>(proofCount)||
               r->sourceEmitterAttempts>r->sourceEmitters.size()||
               r->sourceEmitterPublishedSlots>r->sourceEmitterAttempts||
               r->sourceEmitterAttempts!=service.emitterAttempts||
               r->sourceEmitterPublishedSlots!=service.emitterPublishedSlots||
               r->sourceEmitterArrayPublished!=service.emitterArrayPublished)return false;
            for(std::size_t j=0;j<r->sourceEmitters.size();++j){const auto& e=r->sourceEmitters[j];
                if(e&&(j>=r->sourceEmitterAttempts||!preflightSourceParticleEntityIdentityNative(e->entity,e->counted,e->enrolled,
                    e->entityActivityStored&&e->sourceStartOn==false?0:3)))return false;
                if(e&&e->constructorReturned){
                    std::uint32_t table=0;
                    switch(*service.emitterType){case 0:table=0x3cdcf0;break;case 1:table=0x3cdc50;break;
                    case 2:table=0x3cdbb0;break;case 8:table=0x3cdb10;break;case 9:table=0x3cda70;break;
                    case 11:table=0x3cd9d0;break;case 12:table=0x3cd930;break;default:return false;}
                    if(!e->counted||!e->enrolled||e->dispatch08!=table||
                       e->known[0x70/4]!=(e->managerStored?0u:0xffffffffu)||e->words[0x70/4]||
                       e->known[0x74/4]!=(e->modelStored?0u:0xffffffffu)||e->words[0x74/4]||
                       e->known[0x1c4/4]!=0xffffffffu||e->words[0x1c4/4]||
                       e->known[0x1d4/4]!=7u||e->words[0x1d4/4]!=(e->particleActivationReturned&&e->sourceStartOn==true?3u:1u))return false;
                }
                if(e){
                    const bool subtype1=service.emitterType==1u;
                    const bool loaderReturned=service.emitterType==0u?e->type0LoaderReturned:
                        (subtype1?e->type1LoaderReturned:(service.emitterType==2u?e->type2LoaderReturned:(service.emitterType==9u?e->type9LoaderReturned:(service.emitterType==11u?e->type11LoaderReturned:(service.emitterType==12u?e->type12LoaderReturned:e->type8LoaderReturned)))));
                    std::uint8_t type1PositionCount=0;
                    const SourceEmitter27C890::Type1Position15CFF0* lastPrevious=nullptr;
                    const SourceEmitter27C890::Type1Position15CFF0* lastCurrent=nullptr;
                    for(std::size_t call=0;call<e->type1PositionCalls.size();++call){
                        const auto& position=e->type1PositionCalls[call];const auto mask=position.copyMask;
                        if(mask!=0&&mask!=1&&mask!=3&&mask!=7&&mask!=15)return false;
                        if(position.argumentF12&&(!subtype1||!e->type1LoaderReturned||e->modelStored||
                           *position.argumentF12!=0x3f000000u||(call&&e->type1PositionCalls[call-1].copyMask!=15)))return false;
                        if(mask&&!position.argumentF12)return false;
                        if(mask&1){if(!proofOffset||position.temporary00!=*proofOffset)return false;}
                        if(mask&2){
                            if(call&&position.previous30!=e->type1PositionCalls[call-1].temporary00)return false;
                            lastPrevious=&position;
                        }
                        if(mask&4)lastCurrent=&position;
                        if(mask==15)++type1PositionCount;
                    }
                    for(std::size_t lane=0;lane<4;++lane){
                        if(lastPrevious&&(e->known[0x40/4+lane]!=0xffffffffu||
                           e->words[0x40/4+lane]!=lastPrevious->previous30[lane]))return false;
                        if(lastCurrent&&(e->known[0x30/4+lane]!=0xffffffffu||
                           e->words[0x30/4+lane]!=lastCurrent->temporary00[lane]))return false;
                    }
                    //15CFF0's last store is the exact caller f12 bits, not a
                    //host float conversion. Attached subtype1 retains ctor200=0.
                    if(subtype1&&e->constructorReturned&&(e->positionCopyCount||
                       e->known[0x200/4]!=0xffffffffu||
                       e->words[0x200/4]!=(type1PositionCount?0x3f000000u:0u)))return false;
                    const auto positionCount=subtype1?type1PositionCount:e->positionCopyCount;
                    if(positionCount>2||(positionCount&&(!loaderReturned||e->modelStored)))return false;
                    const auto fogPosition=e->type12PositionMask;
                    if(fogPosition!=0&&fogPosition!=1&&fogPosition!=3&&fogPosition!=7)return false;
                    if(fogPosition&&(service.emitterType!=12u||!service.type12ManagerStored||e->modelStored||
                       !proofOffset||e->type12PositionInput!=*proofOffset))return false;
                    if(!g::OriginalTrackParticleCold50LA::sourceFogRetirementHistoryNative(e->type12Population.retirementScan)||
                       (e->type12Population.retirementScan.step&&!e->type12Population.entered))return false;
                    const bool successorFog=service.emitterType==12u&&i>0&&
                        (i==1?sourceParticleService124_:sourceParticleLaterRows_[i-2]).emitterType==12u;
                    if(successorFog){
                        const auto& p=e->type12Population;
                        if(!g::OriginalTrackParticleCold50LA::sourceFogSuccessorPositionHistoryNative(p.retirementScan,p.positionStore)||
                           !g::OriginalTrackParticleCold50LA::sourceFogSuccessorVelocityHistoryNative(p.retirementScan,p.positionStore,p.velocityStore)||
                           !g::OriginalTrackParticleCold50LA::sourceFogSuccessorFieldsHistoryNative(p.retirementScan,p.positionStore,p.fieldStores,e.get())||
                           !g::OriginalTrackParticleCold50LA::sourceFogSuccessorTailHistoryNative(p.retirementScan,
                               p.positionStore,p.fieldStores,p.tailStores,e.get())||
                           !g::OriginalTrackParticleCold50LA::sourceFogSuccessorCountHistoryNative(p.countStore)||
                           p.fieldTailStep>18||p.postDivideStep>11||p.countStep>4||p.frameSelection.step>28||
                           p.returned!=(p.frameSelection.step==28)||
                           (p.returned&&(p.pc!=0x160b00u||p.retirementScan.step!=10||e->type12PopulationPending))||
                           (p.frameSelection.step&&(!p.countStore.stored||p.countStep!=4))||
                           p.countStore.stored!=(p.countStep==4)||p.countStore.pc!=(p.countStep>=3?0x160994u:0u)||
                           ((p.oscillationStep||p.oscillationRandomStep||p.velocityStep||p.fieldTailStep||
                               p.postDivideStep||p.countStep||p.frameSelection.step)&&!p.entered))return false;
                    }
                    if(successorFog&&(e->type12PropertiesReturned||e->type12LoaderReturned||e->emitterFileLoadReturned)&&
                       (!e->config15AE00||!e->type12Population.returned||e->type12Population.frameSelection.step!=28||
                        e->type12Population.countStep!=4||!e->type12Population.countStore.stored))return false;
                    if(successorFog&&(e->sourceStartOn||e->type12StopMode1D8||e->type12StopActiveBit||
                       e->type12StopWriteMask||e->type12StopReturned||e->particleActivationReturned||
                       e->entityActivityStored||e->entityActivityReturned||e->rowConfigReturned||e->sourceRegistered)&&
                       (!e->type12LoaderReturned||!e->emitterFileLoadReturned||!e->configNativeWithdrawn||
                        !e->type12Population.returned||e->type12Population.countStep!=4||
                        !e->type12Population.countStore.stored))return false;
                    const auto& fogPositionCall=e->type12AttachmentPosition;
                    const auto& fogDirectionCall=e->type12AttachmentDirection;
                    if(!g::OriginalRacerModelSceneOwner50LA::sourceFogTransformHistoryNative(fogPositionCall,fogDirectionCall))return false;
                    const bool fogAttachedEntered=e->type12AttachmentPositionPending||fogPositionCall.entered||
                        fogDirectionCall.entered||e->type12AttachmentAddMask||e->type12AttachmentPreviousStored;
                    if(fogAttachedEntered&&(service.emitterType!=12u||!service.type12ManagerStored||!e->modelStored||
                       !e->offsetStored||fogPosition||(!e->type12AttachmentPositionPending&&!e->type12AttachmentPreviousStored)))return false;
                    if(e->type12AttachmentPositionPending&&(e->type12AttachmentPreviousStored||e->directoryCopied||e->configConstructAttempted))return false;
                    if(fogPositionCall.entered&&fogPositionCall.output!=e->type12AttachmentPositionXYZ)return false;
                    if(fogDirectionCall.entered&&(!fogPositionCall.returned||fogDirectionCall.owner!=fogPositionCall.owner||
                       fogDirectionCall.canonicalNodes!=fogPositionCall.canonicalNodes||
                       fogDirectionCall.graphCount!=fogPositionCall.graphCount||fogDirectionCall.sceneIndex!=fogPositionCall.sceneIndex||
                       fogDirectionCall.output!=e->type12AttachmentDirectionXYZ))return false;
                    if(fogDirectionCall.entered)for(std::size_t lane=0;lane<4;++lane)
                        if(e->known[0x50/4+lane]!=0xffffffffu||fogDirectionCall.input[lane]!=e->words[0x50/4+lane])return false;
                    const auto fogAdd=e->type12AttachmentAddMask;
                    if(fogAdd!=0&&fogAdd!=1&&fogAdd!=3&&fogAdd!=7)return false;
                    if(fogAdd&&!fogDirectionCall.returned)return false;
                    if(e->type12AttachmentPreviousStored&&(fogAdd!=7||!fogDirectionCall.returned))return false;
                    if(fogAttachedEntered){
                        if(e->known[0x3c/4]!=0xffffffffu||e->words[0x3c/4]!=0x3f800000u)return false;
                        for(std::size_t lane=0;lane<3;++lane){
                            const auto bit=1u<<lane;
                            const auto expected=(fogAdd&bit)?e->type12AttachmentAddResult[lane]:
                                ((fogPositionCall.outputMask&bit)?e->type12AttachmentPositionXYZ[lane]:0u);
                            if(e->known[0x30/4+lane]!=0xffffffffu||e->words[0x30/4+lane]!=expected)return false;
                        }
                        for(std::size_t lane=0;lane<4;++lane){
                            const auto expected=e->type12AttachmentPreviousStored?e->words[0x30/4+lane]:(lane==3?0x3f800000u:0u);
                            if(e->known[0x40/4+lane]!=0xffffffffu||e->words[0x40/4+lane]!=expected)return false;
                        }
                    }
                    for(std::size_t lane=0;lane<4;++lane){
                        if((fogPosition&2)&&(e->known[0x30/4+lane]!=0xffffffffu||e->words[0x30/4+lane]!=e->type12PositionInput[lane]))return false;
                        if((fogPosition&4)&&(e->known[0x40/4+lane]!=0xffffffffu||e->words[0x40/4+lane]!=e->type12PositionInput[lane]))return false;
                    }
                    if((e->type12GroupsNonNull||e->type12GroupIndex||e->type12GroupCopyMask||e->type12PopulationPending||e->type12Population.entered||e->type12PropertiesReturned||e->type12LoaderReturned)&&
                       (service.emitterType!=12u||!e->config15AE00))return false;
                    if(service.emitterType==12u&&(positionCount||
                       (e->directoryCopied&&(e->modelStored?!e->type12AttachmentPreviousStored:fogPosition!=7))))return false;
                    if(e->sourceStartOn&&(!loaderReturned||!r->unknown10||
                       *e->sourceStartOn!=(*r->unknown10!=0)||
                       (!e->modelStored&&(service.emitterType==12u?fogPosition!=7:positionCount!=2))))return false;
                    if(e->particleActivationReturned&&(!e->sourceStartOn||e->known[0x19c/4]!=0xffffffffu||e->words[0x19c/4]))return false;
                    if(e->type9StopMode1D8&&(service.emitterType!=9u||!e->type9LoaderReturned||
                       e->sourceStartOn!=false||e->known[0x1d8/4]!=0xffffffffu||
                       *e->type9StopMode1D8!=e->words[0x1d8/4]))return false;
                    if(e->type9StopActiveBit&&(!e->type9StopMode1D8||*e->type9StopMode1D8!=1||
                       !(e->known[0x1d4/4]&2u)||*e->type9StopActiveBit!=((e->words[0x1d4/4]&2u)!=0)))return false;
                    const auto stopMask=e->type9StopWriteMask;
                    if(stopMask!=0&&stopMask!=1&&stopMask!=3&&stopMask!=7&&stopMask!=15)return false;
                    if(stopMask&&(!e->type9StopMode1D8||
                       (*e->type9StopMode1D8==1&&e->type9StopActiveBit!=false)))return false;
                    if((stopMask&1)&&(!(e->known[0x1d4/4]&2u)||(e->words[0x1d4/4]&2u)))return false;
                    if((stopMask&2)&&(!(e->known[0x1d4/4]&4u)||(e->words[0x1d4/4]&4u)))return false;
                    if(stopMask&4)for(std::size_t lane=0;lane<4;++lane)
                        if(e->known[0x30/4+lane]!=0xffffffffu||e->known[0x40/4+lane]!=0xffffffffu||
                           e->words[0x40/4+lane]!=e->words[0x30/4+lane])return false;
                    if((stopMask&8)&&(e->known[0x19c/4]!=0xffffffffu||e->words[0x19c/4]))return false;
                    if(e->type9StopReturned&&(stopMask!=15||!e->particleActivationReturned))return false;
                    if(service.emitterType==9u&&e->sourceStartOn==false&&
                       e->particleActivationReturned!=e->type9StopReturned)return false;
                    if(e->type12StopMode1D8&&(service.emitterType!=12u||!e->type12LoaderReturned||
                       e->sourceStartOn!=false||e->known[0x1d8/4]!=0xffffffffu||
                       *e->type12StopMode1D8!=e->words[0x1d8/4]))return false;
                    if(e->type12StopActiveBit&&(!e->type12StopMode1D8||*e->type12StopMode1D8!=1||
                       !(e->known[0x1d4/4]&2u)||*e->type12StopActiveBit!=((e->words[0x1d4/4]&2u)!=0)))return false;
                    const auto fogStopMask=e->type12StopWriteMask;
                    if(fogStopMask!=0&&fogStopMask!=1&&fogStopMask!=3&&fogStopMask!=7&&fogStopMask!=15)return false;
                    if(fogStopMask&&(!e->type12StopMode1D8||
                       (*e->type12StopMode1D8==1&&e->type12StopActiveBit!=false)))return false;
                    if((fogStopMask&1)&&(!(e->known[0x1d4/4]&2u)||(e->words[0x1d4/4]&2u)))return false;
                    if((fogStopMask&2)&&(!(e->known[0x1d4/4]&4u)||(e->words[0x1d4/4]&4u)))return false;
                    if(fogStopMask&4)for(std::size_t lane=0;lane<4;++lane)
                        if(e->known[0x30/4+lane]!=0xffffffffu||e->known[0x40/4+lane]!=0xffffffffu||
                           e->words[0x40/4+lane]!=e->words[0x30/4+lane])return false;
                    if((fogStopMask&8)&&(e->known[0x19c/4]!=0xffffffffu||e->words[0x19c/4]))return false;
                    if(e->type12StopReturned&&(fogStopMask!=15||!e->particleActivationReturned))return false;
                    if(service.emitterType==12u&&e->sourceStartOn==false&&
                       e->particleActivationReturned!=e->type12StopReturned)return false;

                    if(e->entityActivityStored&&!e->particleActivationReturned)return false;
                    if(e->entityActivityReturned&&!e->entityActivityStored)return false;
                    if(e->rowConfigReturned&&!e->entityActivityReturned)return false;
                    if(e->sourceRegistered&&!e->rowConfigReturned)return false;
                    if(positionCount){
                        if(!proofOffset)return false;
                        for(std::size_t lane=0;lane<4;++lane){
                            if(e->known[0x30/4+lane]!=0xffffffffu||e->words[0x30/4+lane]!=(*proofOffset)[lane])return false;
                            if(positionCount==2&&(e->known[0x40/4+lane]!=0xffffffffu||e->words[0x40/4+lane]!=e->words[0x30/4+lane]))return false;
                        }
                    }
                    if((e->managerStored||e->modelStored||e->offsetStored||e->directoryCopied||e->filenameAppended)&&
                       (i>=4||j!=0||!e->constructorReturned||
                        !((service.emitterType==8u&&service.type8LoadEntered)||
                          (service.emitterType==0u&&service.type0LoadEntered)||
                          (service.emitterType==1u&&service.type1LoadEntered)||
                          (service.emitterType==2u&&service.type2LoadEntered)||
                          (service.emitterType==9u&&service.type9LoadEntered)||
                          (service.emitterType==11u&&service.type11LoadEntered)||
                          (service.emitterType==12u&&service.type12LoadEntered))))return false;
                    if(e->modelStored&&!e->managerStored)return false;
                    if(e->offsetStored&&!e->modelStored)return false;
                    if(e->directoryCopied&&!e->managerStored)return false;
                    if(e->filenameAppended&&!e->directoryCopied)return false;
                    if(e->filenameBytes){if(!e->directoryCopied||*e->filenameBytes>=e->filename78.size()||
                        e->filename78[*e->filenameBytes])return false;}
                    else if(e->directoryCopied)return false;
                    if(e->offsetStored){if(!proofOffset)return false;
                        for(std::size_t n=0;n<4;++n)if(e->known[0x50/4+n]!=0xffffffffu||
                            e->words[0x50/4+n]!=(*proofOffset)[n])return false;}
                    if(e->configConstructAttempted&&(!e->filenameAppended||i>=4||j!=0||
                       (service.emitterType!=0u&&service.emitterType!=1u&&service.emitterType!=2u&&service.emitterType!=8u&&service.emitterType!=9u&&service.emitterType!=11u&&service.emitterType!=12u)))return false;
                    if(e->configConstructReturned&&(!e->configConstructAttempted||!e->config15AE00))return false;
                    if(bool(e->config15AE00)!=e->configConstructReturned)return false;
                    if(e->configOpenAttempted&&!e->configConstructReturned)return false;
                    if(e->configOpenReturned&&!e->configOpenAttempted)return false;
                    if(e->configNativeWithdrawn&&!e->config15AE00)return false;
                    if(e->config15AE00){
                        const auto& config=*e->config15AE00;
                        if(config.nativeClosed_!=e->configNativeWithdrawn||
                           config.sourceOpenReturned_!=e->configOpenReturned||
                           (!e->configNativeWithdrawn&&(!sourceTrackMountDomain_||sourceTrackMountClosed_))||
                           !config.emitterOccurrenceCurrentNative(&e->entity,
                               e->configNativeWithdrawn?nullptr:sourceTrackMountDomain_->resolver3E1A70().get(),
                               e->configNativeWithdrawn?nullptr:sourceTrackMountDomain_->archiveProvider3E1A70().get()))return false;
                        if(config.attempted_&&std::string_view(config.filename_)!=std::string_view(e->filename78.data(),*e->filenameBytes))return false;
                        if(config.emitterPropertyTarget_&&*config.emitterPropertyTarget_!=
                           (service.emitterType==0u?0x15c330u:(service.emitterType==1u?0x15d560u:(service.emitterType==2u?0x15dd00u:(service.emitterType==9u?0x15ef70u:(service.emitterType==11u?0x15fc10u:(service.emitterType==12u?0x1602b0u:0x15e6d0u)))))))return false;
                        constexpr std::uint32_t type0Offsets[]={0x21c,0x220,0x208,0x20c,0x210,0x214,0x218,0x224,0x22c,0x230,0x234,0x238,0x228,0x23c};
                        for(std::size_t n=0;n<config.type0ScalarCount_;++n)
                            if(e->known[type0Offsets[n]/4]!=0xffffffffu)return false;
                        for(std::size_t vector=0;vector<2;++vector)for(std::size_t lane=0;lane<3;++lane)
                            if((config.type0NoiseWritten_[vector]&(1u<<lane))&&
                               e->known[(vector?0x250:0x240)/4+lane]!=0xffffffffu)return false;
                        for(std::size_t vector=0;vector<2;++vector)for(std::size_t lane=0;lane<3;++lane)
                            if((config.emitterVectorWritten_[vector]&(1u<<lane))&&
                               e->known[(vector?0x210:0x200)/4+lane]!=0xffffffffu)return false;
                        for(std::size_t vector=0;vector<2;++vector)for(std::size_t lane=0;lane<3;++lane)
                            if((config.type2RangeWritten_[vector]&(1u<<lane))&&
                               e->known[(vector?0x220:0x210)/4+lane]!=0xffffffffu)return false;
                        if(config.type2MovingReturned_&&e->known[0x200/4]!=0xffffffffu)return false;
                        for(std::size_t vector=0;vector<2;++vector)for(std::size_t lane=0;lane<3;++lane)
                            if((config.type9RangeWritten_[vector]&(1u<<lane))&&
                               e->known[(vector?0x210:0x200)/4+lane]!=0xffffffffu)return false;
                        if(config.type9SpreadReturned_&&e->known[0x224/4]!=0xffffffffu)return false;
                        constexpr std::uint32_t type11Offsets[]={0x200,0x204,0x210,0x214,0x208,0x20c};
                        for(std::size_t n=0;n<config.type11ScalarCount_;++n)
                            if(e->known[type11Offsets[n]/4]!=0xffffffffu)return false;
                        if(config.type11HeadReturned_&&e->known[0x218/4]!=0xffffffffu)return false;
                        for(std::size_t vector=0;vector<2;++vector)for(std::size_t lane=0;lane<3;++lane)
                            if((config.type11RangeWritten_[vector]&(1u<<lane))&&
                               e->known[(vector?0x230:0x220)/4+lane]!=0xffffffffu)return false;
                        if(config.type11MovingReturned_&&e->known[0x21c/4]!=0xffffffffu)return false;
                        for(std::size_t vector=0;vector<2;++vector)for(std::size_t lane=0;lane<3;++lane)
                            if((config.type12RangeWritten_[vector]&(1u<<lane))&&
                               e->known[(vector?0x210:0x200)/4+lane]!=0xffffffffu)return false;
                        if(config.type12DistanceInput_&&(e->known[0x220/4]!=0xffffffffu||
                           e->words[0x220/4]!=(config.type12DistanceSquared_?*config.type12SquaredBits_:*config.type12DistanceInput_)))return false;
                        if(config.type12ReciprocalWritten_&&(e->known[0x224/4]!=0xffffffffu||
                           e->words[0x224/4]!=*config.type12ReciprocalBits_))return false;
                        constexpr std::uint32_t type12Offsets[]={0x228,0x22c,0x234};
                        for(std::size_t n=0;n<config.type12TailScalarCount_;++n)
                            if(e->known[type12Offsets[n]/4]!=0xffffffffu)return false;
                        if(config.type12OscillationEnabled_&&*config.type12OscillationEnabled_!=(e->words[0x22c/4]!=0xbf800000u))return false;
                        if((config.type12OscillationMask_&1)&&(e->known[0x230/4]!=0xffffffffu||e->words[0x230/4]))return false;
                        if((config.type12OscillationMask_&2)&&(e->known[0x238/4]!=0xffffffffu||e->words[0x238/4]))return false;
                        if((config.type12OscillationMask_&4)&&(!(e->known[0x23c/4]&1u)||!(e->words[0x23c/4]&1u)))return false;
                        if((config.type12FlagMask_&1)&&(e->known[0x198/4]!=0xffffffffu||!(e->words[0x198/4]&0x80u)))return false;
                        if((config.type12FlagMask_&2)&&(e->words[0x198/4]&8u))return false;
                        if(config.emitterSpreadReturned_&&e->known[0x228/4]!=0xffffffffu)return false;
                        if(config.commonGroupWritten_&&e->known[0x1b8/4]!=0xffffffffu)return false;
                        constexpr std::uint32_t scalarOffsets[]={0x17c,0x184,0x180,0x188,0x1a0};
                        for(std::uint32_t n=0;n<config.commonScalarCount_;++n)
                            if(e->known[scalarOffsets[n]/4]!=0xffffffffu)return false;
                        for(std::size_t lane=0;lane<3;++lane)if((config.commonVelocityWritten_&(1u<<lane))&&
                            e->known[0x60/4+lane]!=0xffffffffu)return false;
                        if(config.commonModeStored_&&e->known[0x1d8/4]!=0xffffffffu)return false;
                        constexpr std::uint32_t tailOffsets[]={0x1e0,0x1a4,0x1a8,0x1ac};
                        for(std::uint32_t n=0;n<config.commonTailScalarCount_;++n)
                            if(e->known[tailOffsets[n]/4]!=0xffffffffu)return false;
                        if(config.commonDelayCopied_&&(e->known[0x1dc/4]!=0xffffffffu||e->words[0x1dc/4]!=e->words[0x1e0/4]))return false;
                        if(config.commonModeTimeCopied_&&(e->known[0x1e4/4]!=0xffffffffu||e->words[0x1e4/4]!=e->words[0x1ec/4]))return false;
                        if(config.commonFlagCount_&&e->known[0x198/4]!=0xffffffffu)return false;
                        if(config.commonFramesWritten_&&e->known[0x1bc/4]!=0xffffffffu)return false;
                        if(config.commonReciprocalWritten_&&e->known[0x1c0/4]!=0xffffffffu)return false;
                        if(config.commonForcedCountWritten_&&e->known[0x1c8/4]!=0xffffffffu)return false;
                        if(config.commonReturned_&&(e->words[0x1c8/4]&0x80000000u)==0&&e->words[0x1c8/4]!=0)return false;
                        if(e->type8VelocityCopied&&(service.emitterType!=8u||!config.commonReturned_||e->known[0x220/4]!=0xffffffffu||
                           e->words[0x220/4]!=e->words[0x64/4]))return false;
                        if(e->type9VelocityCopied&&(service.emitterType!=9u||!config.commonReturned_||e->known[0x220/4]!=0xffffffffu||
                           e->words[0x220/4]!=e->words[0x64/4]))return false;
                        const bool subtype0=service.emitterType==0u,subtype1=service.emitterType==1u,subtype2=service.emitterType==2u,subtype9=service.emitterType==9u,subtype11=service.emitterType==11u,subtype12=service.emitterType==12u;
                        const auto& groupsNonNull=subtype0?e->type0GroupsNonNull:(subtype1?e->type1GroupsNonNull:(subtype2?e->type2GroupsNonNull:(subtype9?e->type9GroupsNonNull:(subtype11?e->type11GroupsNonNull:(subtype12?e->type12GroupsNonNull:e->type8GroupsNonNull)))));
                        const auto& groupIndex=subtype0?e->type0GroupIndex:(subtype1?e->type1GroupIndex:(subtype2?e->type2GroupIndex:(subtype9?e->type9GroupIndex:(subtype11?e->type11GroupIndex:(subtype12?e->type12GroupIndex:e->type8GroupIndex)))));
                        const auto groupCopyMask=subtype0?e->type0GroupCopyMask:(subtype1?e->type1GroupCopyMask:(subtype2?e->type2GroupCopyMask:(subtype9?e->type9GroupCopyMask:(subtype11?e->type11GroupCopyMask:(subtype12?e->type12GroupCopyMask:e->type8GroupCopyMask)))));
                        const bool propertiesReturned=subtype0?e->type0PropertiesReturned:(subtype1?e->type1PropertiesReturned:(subtype2?e->type2PropertiesReturned:(subtype9?e->type9PropertiesReturned:(subtype11?e->type11PropertiesReturned:(subtype12?e->type12PropertiesReturned:e->type8PropertiesReturned)))));
                        if(!subtype0&&(e->type0GroupsNonNull||e->type0GroupIndex||e->type0GroupCopyMask||e->type0PropertiesReturned||e->type0LoaderReturned))return false;
                        if(!subtype1&&(e->type1GroupsNonNull||e->type1GroupIndex||e->type1GroupCopyMask||e->type1PropertiesReturned||e->type1LoaderReturned))return false;
                        if(!subtype2&&(e->type2GroupsNonNull||e->type2GroupIndex||e->type2GroupCopyMask||e->type2PropertiesReturned||e->type2LoaderReturned))return false;
                        if(!subtype9&&(e->type9GroupsNonNull||e->type9GroupIndex||e->type9GroupCopyMask||e->type9PropertiesReturned||e->type9LoaderReturned))return false;
                        if(!subtype11&&(e->type11GroupsNonNull||e->type11GroupIndex||e->type11GroupCopyMask||e->type11PropertiesReturned||e->type11LoaderReturned))return false;
                        if(!subtype12&&(e->type12GroupsNonNull||e->type12GroupIndex||e->type12GroupCopyMask||e->type12PopulationPending||e->type12Population.entered||e->type12PropertiesReturned||e->type12LoaderReturned))return false;
                        const auto& fogPopulation=e->type12Population;
                        if((e->type12PopulationPending||fogPopulation.entered)&&
                           (config.type12FlagMask_!=3||e->type12GroupCopyMask!=7||
                            (config.sourceDestructorAttempted_&&!e->type12PropertiesReturned)))return false;
                        if(e->type12PropertiesReturned&&(!fogPopulation.entered||!fogPopulation.returned||e->type12PopulationPending||
                           fogPopulation.pc!=0x160b00u||config.type12FlagMask_!=3||e->type12GroupCopyMask!=7))return false;
                        const bool fogRandomRecorded=fogPopulation.randomOwner||fogPopulation.randomApplication||
                            fogPopulation.randomDrawCount||fogPopulation.randomConvertedMask||fogPopulation.randomUnitMask||
                            fogPopulation.randomStack90Mask||fogPopulation.randomStack70Copied;
                        if(fogRandomRecorded&&(!fogPopulation.entered||fogPopulation.capacityAvailable!=true||
                           e->type12PopulationPending==fogPopulation.returned||!fogPopulation.randomOwner||!fogPopulation.randomApplication))return false;
                        if(fogPopulation.randomDrawCount>3)return false;
                        const auto convertedMask=fogPopulation.randomConvertedMask,unitMask=fogPopulation.randomUnitMask;
                        if((convertedMask!=0&&convertedMask!=1&&convertedMask!=3&&convertedMask!=7)||
                           (unitMask!=0&&unitMask!=1&&unitMask!=3&&unitMask!=7))return false;
                        const auto drawMask=(1u<<fogPopulation.randomDrawCount)-1u;
                        if((convertedMask&~drawMask)||(unitMask&~convertedMask))return false;
                        for(std::size_t draw=0;draw<3;++draw){
                            const auto bit=1u<<draw;
                            if(draw<fogPopulation.randomDrawCount){
                                const auto after=fogPopulation.randomBefore[draw]*0x41c64e6du+0x3039u;
                                if(fogPopulation.randomAfter[draw]!=after||fogPopulation.randomValue[draw]!=(after&0x7fffffffu)||
                                   (draw&&fogPopulation.randomBefore[draw]!=fogPopulation.randomAfter[draw-1])||
                                   (draw&&!(unitMask&(1u<<(draw-1)))))return false;
                            }else if(fogPopulation.randomBefore[draw]||fogPopulation.randomAfter[draw]||fogPopulation.randomValue[draw])return false;
                            if(convertedMask&bit){std::uint32_t bits=0;
                                if(!g::detail::sourceParticleTruncateInteger50LA(fogPopulation.randomValue[draw],bits)||
                                   fogPopulation.randomConverted[draw]!=bits)return false;
                            }else if(fogPopulation.randomConverted[draw])return false;
                            if(unitMask&bit){std::uint32_t bits=0;
                                if(!g::detail::sourceParticleScaleRandom50LA(fogPopulation.randomConverted[draw],bits)||
                                   fogPopulation.randomUnit[draw]!=bits)return false;
                            }else if(fogPopulation.randomUnit[draw])return false;
                        }
                        const auto randomStackMask=fogPopulation.randomStack90Mask;
                        if(randomStackMask!=0&&randomStackMask!=1&&randomStackMask!=3&&randomStackMask!=7&&randomStackMask!=15)return false;
                        if(randomStackMask&&(fogPopulation.randomDrawCount!=3||convertedMask!=7||!(unitMask&1u)))return false;
                        for(std::size_t lane=0;lane<4;++lane){
                            if(randomStackMask&(1u<<lane)){
                                if(lane<3){if(!(unitMask&(1u<<lane))||fogPopulation.randomStack90[lane]!=fogPopulation.randomUnit[lane])return false;}
                                else if(fogPopulation.randomStack90[lane]!=0x3f800000u)return false;
                            }else if(fogPopulation.randomStack90[lane])return false;
                            if(fogPopulation.randomStack70Copied){
                                if(randomStackMask!=15||fogPopulation.randomStack70Initial[lane]!=fogPopulation.randomStack90[lane])return false;
                                const auto expected=(lane<3&&fogPopulation.rangeAxes[lane].step==7)
                                    ?fogPopulation.rangeAxes[lane].sum:fogPopulation.randomStack70Initial[lane];
                                if(fogPopulation.randomStack70[lane]!=expected)return false;
                            }else if(fogPopulation.randomStack70[lane]||fogPopulation.randomStack70Initial[lane])return false;
                        }
                        //Authenticate the exact reached range/position prefix,
                        //including partial reads and stores. No broad PC-range
                        //allowlist or fabricated later result replaces a receipt.
                        bool rangeComplete=true;
                        std::uint32_t rangePendingPC=0x160658u;
                        for(std::size_t lane=0;lane<3;++lane){
                            const auto& range=fogPopulation.rangeAxes[lane];
                            if(range.step>7||(!rangeComplete&&range.step))return false;
                            if(range.step&&!fogPopulation.randomStack70Copied)return false;
                            if(range.step>=1){if(e->known[0x210/4+lane]!=0xffffffffu||range.maximum!=e->words[0x210/4+lane])return false;}
                            else if(range.maximum)return false;
                            if(range.step>=2){if(e->known[0x200/4+lane]!=0xffffffffu||range.minimum!=e->words[0x200/4+lane])return false;}
                            else if(range.minimum)return false;
                            if(range.step>=3){if(range.unit!=fogPopulation.randomStack90[lane])return false;}
                            else if(range.unit)return false;
                            std::uint32_t checked=0;
                            if(range.step>=4){if(!g::detail::sourceParticleZeroRangeDifference50LA(range.maximum,range.minimum,checked)||range.difference!=checked)return false;}
                            else if(range.difference)return false;
                            if(range.step>=5){if(!g::detail::sourceParticleUnitTimesPositiveZero50LA(range.unit,range.difference,checked)||range.product!=checked)return false;}
                            else if(range.product)return false;
                            if(range.step>=6){if(!g::detail::sourceParticleBothZeroAdd50LA(range.minimum,range.product,checked)||range.sum!=checked)return false;}
                            else if(range.sum)return false;
                            if(rangeComplete&&range.step<7){
                                rangePendingPC=0x160658u+static_cast<std::uint32_t>(lane)*0x1cu+std::uint32_t(range.step)*4u;
                                rangeComplete=false;
                            }
                        }
                        const auto positionStep=fogPopulation.positionStep;
                        const auto& positionStore=fogPopulation.positionStore;
                        if(positionStep>21||(positionStep&&!rangeComplete)||
                           !g::OriginalTrackParticleCold50LA::sourceFogPositionHistoryNative(positionStore))return false;
                        if(!rangeComplete&&(positionStore.pc||positionStore.slotRead))return false;
                        if(fogPopulation.randomStack70Copied){
                            const auto expectedPC=rangeComplete?0x1606acu+std::uint32_t(positionStep)*4u:rangePendingPC;
                            if((positionStep!=21&&fogPopulation.pc!=expectedPC)||fogPopulation.randomPending)return false;
                        }else if(positionStep||positionStore.pc||positionStore.slotRead)return false;
                        //Source position reads occur Z,X,Y, while additions
                        //occur Z,Y,X. Each threshold means the operation before
                        //the current pending PC completed, not merely attempted.
                        constexpr std::uint8_t inputReached[]={4,5,2},localReached[]={9,6,3},addReached[]={14,12,8};
                        for(std::size_t lane=0;lane<3;++lane){
                            if(positionStep>=inputReached[lane]){
                                if(e->known[0x30/4+lane]!=0xffffffffu||fogPopulation.positionInput[lane]!=e->words[0x30/4+lane])return false;
                            }else if(fogPopulation.positionInput[lane])return false;
                            if(positionStep>=localReached[lane]){
                                if(fogPopulation.positionLocal[lane]!=fogPopulation.randomStack70[lane])return false;
                            }else if(fogPopulation.positionLocal[lane])return false;
                            if(positionStep>=addReached[lane]){std::uint32_t checked=0;
                                if(!g::detail::sourceParticleBothZeroAdd50LA(fogPopulation.positionInput[lane],fogPopulation.positionLocal[lane],checked)||
                                   fogPopulation.positionResult[lane]!=checked)return false;
                            }else if(fogPopulation.positionResult[lane])return false;
                            if(positionStep>=16u+lane){if(fogPopulation.positionStackA0[lane]!=fogPopulation.positionResult[lane])return false;}
                            else if(fogPopulation.positionStackA0[lane])return false;
                        }
                        if(fogPopulation.positionIndex128.has_value()!=(positionStep>=7)||
                           fogPopulation.positionPublication138.has_value()!=(positionStep>=10))return false;
                        if(positionStep>=19){if(fogPopulation.positionStackA0[3]!=0x3f800000u)return false;}
                        else if(fogPopulation.positionStackA0[3])return false;
                        if(positionStore.slotRead!=(positionStep>=15)||positionStore.stored!=(positionStep==21))return false;
                        if(positionStep<14){if(positionStore.pc)return false;}
                        else if(positionStep==14){if(positionStore.pc!=0x1606e4u)return false;}
                        else if(positionStep<20){if(positionStore.pc!=0x1606e4u)return false;}
                        else if(positionStore.pc!=0x1606fcu)return false;
                        if(positionStore.slotRead&&(!fogPopulation.positionIndex128||!fogPopulation.positionPublication138||
                           positionStore.index128!=fogPopulation.positionIndex128||
                           positionStore.publication138!=*fogPopulation.positionPublication138))return false;
                        if(fogPopulation.positionIndex128&&*fogPopulation.positionIndex128!=(successorFog?1:0))return false;
                        if(positionStore.stored&&positionStore.after!=fogPopulation.positionStackA0)return false;
                        if(!e->borrowsWithdrawn&&positionStep){
                            const auto* current=e->manager70;
                            if(!current||current!=fogPopulation.manager||
                               (fogPopulation.positionIndex128&&!(successorFog?
                                   current->sourceFogSuccessorCountObservationCurrentNative(*fogPopulation.positionIndex128,
                                       fogPopulation.retirementScan,e.get(),&fogPopulation):
                                   current->sourceFogCountObservationCurrentNative(*fogPopulation.positionIndex128,&fogPopulation)))||
                               (fogPopulation.positionPublication138&&(!current->sourceParticlePointersPublished_||
                                  current->sourceParticlePointers138_!=*fogPopulation.positionPublication138))||
                               !(successorFog?current->sourceFogSuccessorPositionCurrentNative(fogPopulation.retirementScan,positionStore):
                                   current->sourceFogPositionCurrentNative(positionStore)))return false;
                        }
                        //The committed position's next160700 prefix is
                        //independent of the earlier three-draw/range history.
                        const auto oscillationStep=fogPopulation.oscillationStep;
                        const auto oscillationRandomStep=fogPopulation.oscillationRandomStep;
                        const auto fieldTailStep=fogPopulation.fieldTailStep;
                        const auto& fields=fogPopulation.fieldStores;
                        if(oscillationStep>2||oscillationRandomStep>5||fieldTailStep>18||
                           (oscillationStep&&positionStep!=21)||
                           fogPopulation.oscillation22C.has_value()!=(oscillationStep>=1)||
                           fogPopulation.oscillationDisabled.has_value()!=(oscillationStep==2)||
                           !g::OriginalTrackParticleCold50LA::sourceFogFieldsHistoryNative(fields))return false;
                        if(oscillationStep>=1){
                            if(e->known[0x22c/4]!=0xffffffffu||*fogPopulation.oscillation22C!=e->words[0x22c/4])return false;
                        }
                        if(oscillationStep==2){
                            const auto value=*fogPopulation.oscillation22C;
                            if((value&0x7f800000u)==0x7f800000u||
                               (!(value&0x7f800000u)&&(value&0x007fffffu))||
                               *fogPopulation.oscillationDisabled!=(value==0xbf800000u))return false;
                        }
                        const bool disabled=fogPopulation.oscillationDisabled==true;
                        const bool enabled=fogPopulation.oscillationDisabled==false;
                        const auto velocityStep=fogPopulation.velocityStep;
                        const auto& velocityStore=fogPopulation.velocityStore;
                        if(velocityStep>24||(velocityStep&&(!enabled||oscillationRandomStep!=5))||
                           !g::OriginalTrackParticleCold50LA::sourceFogVelocityHistoryNative(velocityStore))return false;
                        constexpr std::uint8_t velocityRawAt[]={2,7,12,16,20};
                        constexpr std::uint8_t velocityConvertedAt[]={3,8,13,17,21};
                        constexpr std::uint8_t velocityUnitAt[]={5,10,14,18,22};
                        for(std::size_t draw=0;draw<5;++draw){
                            if(velocityStep>=velocityRawAt[draw]){
                                const auto previous=draw?fogPopulation.velocityRandomAfter[draw-1]:fogPopulation.oscillationRandomAfter;
                                const auto after=fogPopulation.velocityRandomBefore[draw]*0x41c64e6du+0x3039u;
                                if(fogPopulation.velocityRandomBefore[draw]!=previous||fogPopulation.velocityRandomAfter[draw]!=after||
                                   fogPopulation.velocityRandomValue[draw]!=(after&0x7fffffffu))return false;
                            }else if(fogPopulation.velocityRandomBefore[draw]||fogPopulation.velocityRandomAfter[draw]||fogPopulation.velocityRandomValue[draw])return false;
                            if(velocityStep>=velocityConvertedAt[draw]){std::uint32_t bits=0;
                                if(!g::detail::sourceParticleTruncateInteger50LA(fogPopulation.velocityRandomValue[draw],bits)||
                                   fogPopulation.velocityRandomConverted[draw]!=bits)return false;
                            }else if(fogPopulation.velocityRandomConverted[draw])return false;
                            if(velocityStep>=velocityUnitAt[draw]){std::uint32_t bits=0;
                                if(!g::detail::sourceParticleScaleRandom50LA(fogPopulation.velocityRandomConverted[draw],bits)||
                                   fogPopulation.velocityRandomUnit[draw]!=bits)return false;
                            }else if(fogPopulation.velocityRandomUnit[draw])return false;
                        }
                        constexpr std::uint8_t magnitudeReadAt[]={4,9},productAt[]={1,6,11};
                        for(std::size_t input=0;input<2;++input){
                            const auto& magnitude=fogPopulation.velocityMagnitude234[input];
                            if(magnitude.has_value()!=(velocityStep>=magnitudeReadAt[input])||
                               (magnitude&&(e->known[0x234/4]!=0xffffffffu||*magnitude!=e->words[0x234/4])))return false;
                        }
                        for(std::size_t product=0;product<3;++product){
                            if(velocityStep>=productAt[product]){
                                const auto& magnitude=product?fogPopulation.velocityMagnitude234[product-1]:fogPopulation.oscillationMagnitude234;
                                const auto unit=product?fogPopulation.velocityRandomUnit[product-1]:fogPopulation.oscillationRandomUnit;
                                std::uint32_t bits=0;
                                if(!magnitude||!g::detail::sourceParticleUnitTimesPositiveZero50LA(unit,*magnitude,bits)||
                                   fogPopulation.velocityProducts[product]!=bits)return false;
                            }else if(fogPopulation.velocityProducts[product])return false;
                        }
                        if(fogPopulation.velocityStackB0Mask!=(velocityStep>=11?15u:0u)||
                           fogPopulation.velocityStackB0Loaded!=(velocityStep>=11)||
                           fogPopulation.velocityStack80Copied!=(velocityStep>=11))return false;
                        const std::array<std::uint32_t,4> emptyVelocity{};
                        if(velocityStep>=11){
                            const std::array<std::uint32_t,4> expectedVelocity={{fogPopulation.velocityProducts[2],
                                fogPopulation.velocityProducts[1],fogPopulation.velocityProducts[0],0x3f800000u}};
                            if(fogPopulation.velocityStackB0!=expectedVelocity||fogPopulation.velocityStack80!=fogPopulation.velocityStackB0)return false;
                        }else if(fogPopulation.velocityStackB0!=emptyVelocity||fogPopulation.velocityStack80!=emptyVelocity)return false;
                        constexpr std::uint8_t signAt[]={14,18,22};
                        for(std::size_t lane=0;lane<3;++lane){
                            const auto& decision=fogPopulation.velocitySignLeHalf[lane];
                            const auto& negation=fogPopulation.velocityNegationInput[lane];
                            if(decision.has_value()!=(velocityStep>=signAt[lane])||
                               negation.has_value()!=(velocityStep==signAt[lane]))return false;
                            if(decision&&(*decision!=(fogPopulation.velocityRandomUnit[lane+2]<=0x3f000000u)||
                               *decision!=(velocityStep>signAt[lane])))return false;
                            if(negation&&*negation!=fogPopulation.velocityStack80[lane])return false;
                        }
                        if(velocityStore.stored!=(velocityStep==24)||
                           velocityStore.pc!=(velocityStep>=23?0x16088cu:0u)||
                           (velocityStore.stored&&velocityStore.after!=fogPopulation.velocityStack80))return false;
                        if(!e->borrowsWithdrawn&&(velocityStore.pc||(successorFog&&positionStore.stored))){
                            const auto* current=e->manager70;
                            if(!current||current!=fogPopulation.manager||
                               !(successorFog?current->sourceFogSuccessorVelocityCurrentNative(fogPopulation.retirementScan,
                                   positionStore,velocityStore):current->sourceFogVelocityCurrentNative(positionStore,velocityStore)))return false;
                        }
                        const bool fieldTailEntered=disabled||(enabled&&velocityStep==24);
                        if((fieldTailStep&&!fieldTailEntered)||(oscillationRandomStep&&!enabled)||
                           fogPopulation.oscillationMagnitude234.has_value()!=(oscillationRandomStep>=4))return false;
                        if(oscillationRandomStep>=2){
                            const auto after=fogPopulation.oscillationRandomBefore*0x41c64e6du+0x3039u;
                            if(fogPopulation.oscillationRandomBefore!=fogPopulation.randomAfter[2]||
                               fogPopulation.oscillationRandomAfter!=after||fogPopulation.oscillationRandomValue!=(after&0x7fffffffu))return false;
                        }else if(fogPopulation.oscillationRandomBefore||fogPopulation.oscillationRandomAfter||fogPopulation.oscillationRandomValue)return false;
                        if(oscillationRandomStep>=3){std::uint32_t bits=0;
                            if(!g::detail::sourceParticleTruncateInteger50LA(fogPopulation.oscillationRandomValue,bits)||
                               fogPopulation.oscillationRandomConverted!=bits)return false;
                        }else if(fogPopulation.oscillationRandomConverted)return false;
                        if(oscillationRandomStep>=4&&(e->known[0x234/4]!=0xffffffffu||
                           *fogPopulation.oscillationMagnitude234!=e->words[0x234/4]))return false;
                        if(oscillationRandomStep>=5){std::uint32_t bits=0;
                            if(!g::detail::sourceParticleScaleRandom50LA(fogPopulation.oscillationRandomConverted,bits)||
                               fogPopulation.oscillationRandomUnit!=bits)return false;
                        }else if(fogPopulation.oscillationRandomUnit)return false;
                        if(fogPopulation.sizeInput184.has_value()!=(fieldTailStep>=1)||
                           fogPopulation.initialSizeInput184.has_value()!=(fieldTailStep>=8)||
                           fogPopulation.lifeInput17C.has_value()!=(fieldTailStep>=12)||
                           fogPopulation.ageInput1A0.has_value()!=(fieldTailStep>=14)||
                           fogPopulation.frameInput190.has_value()!=(fieldTailStep>=16)||
                           fogPopulation.initialMirrorXLeHalf.has_value()!=(fieldTailStep>=6))return false;
                        if((fogPopulation.sizeInput184&&(e->known[0x184/4]!=0xffffffffu||*fogPopulation.sizeInput184!=e->words[0x184/4]))||
                           (fogPopulation.initialSizeInput184&&(e->known[0x184/4]!=0xffffffffu||*fogPopulation.initialSizeInput184!=e->words[0x184/4]))||
                           (fogPopulation.lifeInput17C&&(e->known[0x17c/4]!=0xffffffffu||*fogPopulation.lifeInput17C!=e->words[0x17c/4]))||
                           (fogPopulation.ageInput1A0&&(e->known[0x1a0/4]!=0xffffffffu||*fogPopulation.ageInput1A0!=e->words[0x1a0/4]))||
                           (fogPopulation.frameInput190&&(e->known[0x190/4]!=0xffffffffu||
                               (fogPopulation.frameSelection.frameStored190?
                                   (e->known[0x1b0/4]!=0xffffffffu||*fogPopulation.frameInput190!=e->words[0x1b0/4]):
                                   *fogPopulation.frameInput190!=e->words[0x190/4])))||
                           (fogPopulation.initialMirrorXLeHalf&&*fogPopulation.initialMirrorXLeHalf!=(fogPopulation.randomUnit[0]<=0x3f000000u)))return false;
                        const auto expectedFieldCount=fieldTailStep>=17?6u:(fieldTailStep>=15?5u:
                            (fieldTailStep>=13?4u:(fieldTailStep>=11?3u:(fieldTailStep>=10?2u:(fieldTailStep>=7?1u:0u)))));
                        if(fields.storedCount!=expectedFieldCount||fields.lifeRead30.has_value()!=(fieldTailStep==18))return false;
                        const auto expectedFieldPC=fieldTailStep<6?0u:(fieldTailStep<9?0x1608acu:
                            (fieldTailStep==9?0x1608b8u:(fieldTailStep<12?0x1608bcu:
                            (fieldTailStep<14?0x1608c4u:(fieldTailStep<16?0x1608ccu:
                            (fieldTailStep==16?0x1608d4u:0x1608d8u))))));
                        if(fields.pc!=expectedFieldPC||(fields.storedCount&&fields.emitter!=e.get()))return false;
                        if((fields.storedCount>=1&&fields.values[0]!=*fogPopulation.sizeInput184)||
                           (fields.storedCount>=2&&fields.values[1]!=*fogPopulation.initialSizeInput184)||
                           (fields.storedCount>=4&&fields.values[2]!=*fogPopulation.lifeInput17C)||
                           (fields.storedCount>=5&&fields.values[3]!=*fogPopulation.ageInput1A0)||
                           (fields.storedCount>=6&&fields.values[4]!=*fogPopulation.frameInput190))return false;
                        if(!e->borrowsWithdrawn&&(fields.pc||fields.storedCount||(successorFog&&positionStore.stored))){
                            const auto* current=e->manager70;
                            if(!current||current!=fogPopulation.manager||
                               !(successorFog?current->sourceFogSuccessorFieldsCurrentNative(fogPopulation.retirementScan,
                                   positionStore,fields,e.get()):current->sourceFogFieldsCurrentNative(positionStore,fields,e.get())))return false;
                        }
                        const auto postDivideStep=fogPopulation.postDivideStep;
                        const auto& tail=fogPopulation.tailStores;
                        if(postDivideStep>11||(postDivideStep&&fieldTailStep!=18)||
                           fogPopulation.reciprocalBits.has_value()!=(postDivideStep>=1)||
                           fogPopulation.reciprocalDAndSD!=(postDivideStep>=1)||
                           fogPopulation.initialFlags198.has_value()!=(postDivideStep>=4)||
                           fogPopulation.mirrorYLeHalf.has_value()!=(postDivideStep>=5)||
                           fogPopulation.mirrorXSet.has_value()!=(postDivideStep>=6)||
                           fogPopulation.firstFlagsResult.has_value()!=(postDivideStep>=7)||
                           fogPopulation.mirrorYSet.has_value()!=(postDivideStep>=9)||
                           fogPopulation.secondFlagsResult.has_value()!=(postDivideStep>=10)||
                           !g::OriginalTrackParticleCold50LA::sourceFogTailHistoryNative(tail))return false;
                        if(postDivideStep>=1){std::uint32_t bits=0;
                            if(!fields.lifeRead30||!g::detail::sourceParticleDivideOneBySignedZero50LA(*fields.lifeRead30,bits)||
                               *fogPopulation.reciprocalBits!=bits)return false;
                        }
                        if(fogPopulation.initialFlags198&&(e->known[0x198/4]!=0xffffffffu||
                           *fogPopulation.initialFlags198!=e->words[0x198/4]))return false;
                        if(fogPopulation.mirrorYLeHalf&&*fogPopulation.mirrorYLeHalf!=(fogPopulation.randomUnit[1]<=0x3f000000u))return false;
                        const bool firstChoice=fogPopulation.initialMirrorXLeHalf==false;
                        const bool secondChoice=fogPopulation.mirrorYLeHalf==false;
                        if((postDivideStep==5&&!firstChoice)||(postDivideStep==8&&!secondChoice)||
                           fogPopulation.mirrorEnable1CC.has_value()!=(postDivideStep>=6&&firstChoice)||
                           fogPopulation.mirrorEnable1D0.has_value()!=(postDivideStep>=9&&secondChoice))return false;
                        if((fogPopulation.mirrorEnable1CC&&(e->known[0x1cc/4]!=0xffffffffu||*fogPopulation.mirrorEnable1CC!=e->words[0x1cc/4]))||
                           (fogPopulation.mirrorEnable1D0&&(e->known[0x1d0/4]!=0xffffffffu||*fogPopulation.mirrorEnable1D0!=e->words[0x1d0/4])))return false;
                        if((fogPopulation.mirrorXSet&&*fogPopulation.mirrorXSet!=(firstChoice&&*fogPopulation.mirrorEnable1CC!=0))||
                           (fogPopulation.mirrorYSet&&*fogPopulation.mirrorYSet!=(secondChoice&&*fogPopulation.mirrorEnable1D0!=0)))return false;
                        const bool firstSet=fogPopulation.mirrorXSet==true,secondSet=fogPopulation.mirrorYSet==true;
                        const auto firstReadPC=firstSet?0x160930u:0x160940u,firstWritePC=firstSet?0x16093cu:0x16094cu;
                        const auto secondReadPC=secondSet?0x160964u:0x160974u,secondWritePC=secondSet?0x160970u:0x160984u;
                        constexpr std::uint8_t tailCompletedAt[]={0,0,1,2,2,3,3,4,5,5,6,7};
                        if(tail.completedOps!=tailCompletedAt[postDivideStep])return false;
                        std::uint32_t expectedTailPC=0,postDividePC=0x1608dcu;
                        switch(postDivideStep){
                            case 0:break;
                            case 1:expectedTailPC=postDividePC=0x1608e0u;break;
                            case 2:expectedTailPC=postDividePC=0x1608e4u;break;
                            case 3:expectedTailPC=0x1608e4u;postDividePC=0x1608e8u;break;
                            case 4:expectedTailPC=postDividePC=0x1608f0u;break;
                            case 5:expectedTailPC=0x1608f0u;postDividePC=0x160924u;break;
                            case 6:expectedTailPC=postDividePC=firstReadPC;break;
                            case 7:expectedTailPC=postDividePC=firstWritePC;break;
                            case 8:expectedTailPC=firstWritePC;postDividePC=0x160958u;break;
                            case 9:expectedTailPC=postDividePC=secondReadPC;break;
                            case 10:expectedTailPC=postDividePC=secondWritePC;break;
                            case 11:expectedTailPC=secondWritePC;postDividePC=0x160988u;break;
                        }
                        if(tail.pc!=expectedTailPC)return false;
                        if((tail.completedOps>=1&&(tail.pcs[0]!=0x1608e0u||tail.values[0]!=*fogPopulation.reciprocalBits))||
                           (tail.completedOps>=2&&(tail.pcs[1]!=0x1608e4u||tail.values[1]))||
                           (tail.completedOps>=3&&(tail.pcs[2]!=0x1608f0u||tail.values[2]!=*fogPopulation.initialFlags198))||
                           (tail.completedOps>=4&&(tail.pcs[3]!=firstReadPC||tail.values[3]!=tail.values[2]))||
                           (tail.completedOps>=5&&(tail.pcs[4]!=firstWritePC||tail.values[4]!=*fogPopulation.firstFlagsResult))||
                           (tail.completedOps>=6&&(tail.pcs[5]!=secondReadPC||tail.values[5]!=tail.values[4]))||
                           (tail.completedOps>=7&&(tail.pcs[6]!=secondWritePC||tail.values[6]!=*fogPopulation.secondFlagsResult)))return false;
                        if((fogPopulation.firstFlagsResult&&*fogPopulation.firstFlagsResult!=(firstSet?(tail.values[3]|0x4000u):(tail.values[3]&0xffffbfffu)))||
                           (fogPopulation.secondFlagsResult&&*fogPopulation.secondFlagsResult!=(secondSet?(tail.values[5]|0x8000u):(tail.values[5]&0xffff7fffu))))return false;
                        //Even an ordinary-life pendingDIV must agree with an
                        //authoritatively empty post-DIV pool history.
                        if(!e->borrowsWithdrawn&&fieldTailStep==18){
                            const auto* current=e->manager70;
                            if(!current||current!=fogPopulation.manager||
                               !(successorFog?current->sourceFogSuccessorTailCurrentNative(fogPopulation.retirementScan,
                                   positionStore,fields,tail,e.get()):current->sourceFogTailCurrentNative(positionStore,fields,tail,e.get())))return false;
                        }
                        const auto countStep=fogPopulation.countStep;
                        const auto& countStore=fogPopulation.countStore;
                        if(countStep>4||(countStep&&postDivideStep!=11)||
                           bool(fogPopulation.countManager70)!=(countStep>=1)||
                           fogPopulation.countBefore128.has_value()!=(countStep>=2)||
                           fogPopulation.countAfter128.has_value()!=(countStep>=3)||
                           countStore.stored!=(countStep==4)||
                           countStore.pc!=(countStep>=3?0x160994u:0u)||
                           !(successorFog?g::OriginalTrackParticleCold50LA::sourceFogSuccessorCountHistoryNative(countStore):
                               g::OriginalTrackParticleCold50LA::sourceFogCountHistoryNative(countStore)))return false;
                        if((fogPopulation.countManager70&&fogPopulation.countManager70!=fogPopulation.manager)||
                           (fogPopulation.countBefore128&&*fogPopulation.countBefore128!=(successorFog?1:0))||
                           (fogPopulation.countAfter128&&*fogPopulation.countAfter128!=(successorFog?2:1)))return false;
                        if(countStore.stored&&(!positionStore.index128||!positionStore.stored||
                           countStore.manager!=fogPopulation.countManager70||
                           countStore.pool!=positionStore.pool||countStore.record!=positionStore.record||
                           countStore.publication138!=positionStore.publication138||countStore.index128!=*positionStore.index128||
                           countStore.emitter!=e.get()||countStore.occurrence!=&fogPopulation||
                           countStore.generation!=e->entity.sessionGeneration||
                           countStore.before128!=*fogPopulation.countBefore128||countStore.after128!=*fogPopulation.countAfter128))return false;
                        //After withdrawal, Track may already be destroyed.
                        //Only the copied structural receipt is consulted then.
                        if(!e->borrowsWithdrawn&&postDivideStep==11){
                            const auto* current=e->manager70;
                            if(!current||current!=fogPopulation.manager||
                               !(successorFog?current->sourceFogSuccessorCountReceiptCurrentNative(countStore,e.get(),&fogPopulation):
                                   current->sourceFogCountReceiptCurrentNative(countStore,e.get(),&fogPopulation)))return false;
                        }
                        if(postDivideStep==11)postDividePC=0x160988u+std::uint32_t(countStep)*4u;
                        const auto& frame=fogPopulation.frameSelection;
                        const auto frameStep=frame.step;
                        if(frameStep>28||(frameStep&&countStep!=4)||
                           frame.seedGuard80A8.has_value()!=(frameStep>=1)||
                           frame.first1B0.has_value()!=(frameStep>=2)||
                           frame.last1B4.has_value()!=(frameStep>=3)||
                           frame.seedCache3D6B30.has_value()!=(frameStep>=6)||
                           frame.differenceWord.has_value()!=(frameStep>=7)||
                           frame.spanWord.has_value()!=(frameStep>=8)||
                           frame.spanConverted.has_value()!=(frameStep>=9)||
                           frame.randomDrawn!=(frameStep>=10)||
                           frame.randomConverted.has_value()!=(frameStep>=11)||
                           frame.rangeGuard80AC.has_value()!=(frameStep>=12)||
                           frame.randomUnit.has_value()!=(frameStep>=13)||
                           frame.productBits.has_value()!=(frameStep>=14)||
                           frame.offsetWord.has_value()!=(frameStep>=15)||
                           frame.selectedWord.has_value()!=(frameStep>=16)||
                           frame.rangeCheck3D6B38.has_value()!=(frameStep>=19)||
                           frame.frameStored190!=(frameStep>=23)||
                           frame.creationIndexAfter.has_value()!=(frameStep>=24)||
                           frame.loopVolume228.has_value()!=(frameStep>=25)||
                           frame.indexConverted.has_value()!=(frameStep>=26)||
                           frame.loopAgain.has_value()!=(frameStep>=27))return false;
                        const bool coldSeed=frame.seedGuard80A8==false;
                        if(((frameStep==3||frameStep==4)&&!coldSeed)||
                           frame.seedCacheCleared!=(coldSeed&&frameStep>=4)||
                           frame.seedGuardStored!=(coldSeed&&frameStep>=5)||
                           (coldSeed&&frame.seedCache3D6B30&&*frame.seedCache3D6B30!=0)||
                           (frameStep>=8&&(!frame.seedCache3D6B30||!*frame.seedCache3D6B30)))return false;
                        if((frame.first1B0&&(e->known[0x1b0/4]!=0xffffffffu||*frame.first1B0!=e->words[0x1b0/4]))||
                           (frame.last1B4&&(e->known[0x1b4/4]!=0xffffffffu||*frame.last1B4!=e->words[0x1b4/4]))||
                           (frame.differenceWord&&*frame.differenceWord!=*frame.last1B4-*frame.first1B0)||
                           (frame.spanWord&&*frame.spanWord!=*frame.differenceWord+1u))return false;
                        if(frame.spanConverted){std::uint32_t bits=0;
                            if(!g::detail::sourceParticleExactSignedSpan50LA(*frame.spanWord,bits)||*frame.spanConverted!=bits)return false;
                        }
                        if(frame.randomDrawn){
                            const auto predecessor=fogPopulation.oscillationDisabled==true?
                                fogPopulation.randomAfter[2]:fogPopulation.velocityRandomAfter[4];
                            const auto after=frame.randomBefore*0x41c64e6du+0x3039u;
                            if(frame.randomBefore!=predecessor||frame.randomAfter!=after||frame.randomValue!=(after&0x7fffffffu))return false;
                        }else if(frame.randomBefore||frame.randomAfter||frame.randomValue)return false;
                        if(frame.randomConverted){std::uint32_t bits=0;
                            if(!g::detail::sourceParticleTruncateInteger50LA(frame.randomValue,bits)||*frame.randomConverted!=bits)return false;
                        }
                        if(frame.randomUnit){std::uint32_t bits=0;
                            if(!g::detail::sourceParticleScaleRandom50LA(*frame.randomConverted,bits)||*frame.randomUnit!=bits)return false;
                        }
                        if(frame.productBits){std::uint32_t bits=0;
                            if(!g::detail::sourceParticleBothZeroMultiply50LA(*frame.spanConverted,*frame.randomUnit,bits)||
                               *frame.productBits!=bits)return false;
                        }
                        if(frame.offsetWord){std::uint32_t word=0;
                            if(!g::detail::sourceParticleZeroToWord50LA(*frame.productBits,word)||*frame.offsetWord!=word)return false;
                        }
                        if(frame.selectedWord&&*frame.selectedWord!=*frame.first1B0+*frame.offsetWord)return false;
                        const bool coldRange=frame.rangeGuard80AC==false;
                        if(((frameStep==16||frameStep==17)&&!coldRange)||
                           frame.rangeCacheStored!=(coldRange&&frameStep>=17)||
                           frame.rangeGuardStored!=(coldRange&&frameStep>=18)||
                           (coldRange&&frame.rangeCheck3D6B38&&*frame.rangeCheck3D6B38!=1))return false;
                        const bool checkedRange=frameStep>=20&&frame.rangeCheck3D6B38&&*frame.rangeCheck3D6B38!=0;
                        if(frame.selectedBelowFirst.has_value()!=checkedRange||frame.rangeCondition.has_value()!=checkedRange||
                           frame.lastBelowSelected.has_value()!=(checkedRange&&frame.selectedBelowFirst==false))return false;
                        if(checkedRange){
                            const bool below=(*frame.selectedWord^0x80000000u)<(*frame.first1B0^0x80000000u);
                            if(*frame.selectedBelowFirst!=below)return false;
                            bool condition=false;
                            if(!below){
                                const bool above=(*frame.last1B4^0x80000000u)<(*frame.selectedWord^0x80000000u);
                                if(*frame.lastBelowSelected!=above)return false;condition=!above;
                            }
                            if(*frame.rangeCondition!=condition)return false;
                        }
                        if(frame.assertionArgument.has_value()!=(checkedRange&&frameStep>=21)||
                           frame.assertionReturnV0.has_value()!=(checkedRange&&frameStep>=22)||
                           (frameStep==21&&!checkedRange))return false;
                        if(frame.assertionArgument&&*frame.assertionArgument!=(frame.rangeCondition==true?1u:0u))return false;
                        if(frame.assertionReturnV0&&(*frame.assertionReturnV0!=0||frame.rangeCondition!=true))return false;
                        if(frame.frameStored190&&(e->known[0x190/4]!=0xffffffffu||
                           e->words[0x190/4]!=*frame.selectedWord||!fogPopulation.frameInput190||
                           *fogPopulation.frameInput190!=*frame.first1B0))return false;
                        if(frame.creationIndexAfter&&(!fogPopulation.creationIndexInitial||
                           *fogPopulation.creationIndexInitial!=0||*frame.creationIndexAfter!=1))return false;
                        if(frame.loopVolume228&&(e->known[0x228/4]!=0xffffffffu||*frame.loopVolume228!=e->words[0x228/4]))return false;
                        if(frame.indexConverted&&*frame.indexConverted!=0x3f800000u)return false;
                        if(frame.loopAgain){const auto volume=*frame.loopVolume228;
                            if((volume&0x7f800000u)==0x7f800000u||((volume&0x7f800000u)==0&&(volume&0x7fffffu))||
                               *frame.loopAgain!=(!(volume&0x80000000u)&&volume>0x3f800000u))return false;
                        }
                        if(frameStep==28&&(frame.loopAgain!=false||!fogPopulation.returned))return false;
                        if(fogRandomRecorded&&fogPopulation.returned!=(frameStep==28))return false;
                        //The shared lazy/cache/RNG values may legitimately be
                        //changed by later process consumers. Validate retained
                        //observations and owner identity, never freeze globals.
                        if(countStep==4){
                            constexpr std::uint32_t framePCs[]={0x160998,0x16099c,0x1609a4,0x1609b0,0x1609b4,
                                0x1609bc,0x1609c4,0,0x1609f8,0x308038,0x160a04,0x160a08,0x160a14,0x160a18,
                                0x160a1c,0x160a28,0x160a34,0x160a38,0x160a44,0x160a48,0,
                                0x13366c,0x160ab0,0x160ab4,0x160abc,0x160ac0,0x160ac4,0,0x160b00};
                            if(frameStep==7)postDividePC=*frame.seedCache3D6B30?0x1609ecu:0x1609c8u;
                            else if(frameStep==20)postDividePC=*frame.rangeCheck3D6B38?0x160a7cu:0x160ab0u;
                            else if(frameStep==27)postDividePC=*frame.loopAgain?0x1605c4u:0x160ad4u;
                            else postDividePC=framePCs[frameStep];
                        }
                        if(positionStep==21){
                            std::uint32_t expectedPC=0x160700u;
                            if(oscillationStep==1)expectedPC=0x160704u;
                            else if(fieldTailEntered)expectedPC=fieldTailStep==18?postDividePC:
                                0x160894u+std::uint32_t(fieldTailStep)*4u;
                            else if(enabled){
                                if(oscillationRandomStep<5){
                                    constexpr std::uint32_t randomPCs[]={0x160714,0x308038,0x160724,0x160730,0x160734};
                                    expectedPC=randomPCs[oscillationRandomStep];
                                }else{
                                    constexpr std::uint32_t velocityPCs[]={
                                        0x16073c,0x308038,0x160748,0x160754,0x160758,0x160760,
                                        0x308038,0x16076c,0x160778,0x16077c,0x160780,
                                        0x308038,0x1607a8,0x1607bc,0x1607e0,
                                        0x308038,0x1607f8,0x16080c,0x160830,
                                        0x308038,0x160848,0x16085c,0x160880,0x16088c};
                                    if(velocityStep>=24)return false;
                                    expectedPC=velocityPCs[velocityStep];
                                }
                            }
                            if(fogPopulation.pc!=expectedPC)return false;
                        }
                        //These are retained draw receipts, not a freeze of the
                        //global raw word. Other legitimate process consumers may
                        //advance it later; only consecutive draws here are joined.
                        if(fogRandomRecorded&&!e->borrowsWithdrawn){
                            if(!sourceLoadApplication_||fogPopulation.randomApplication!=sourceLoadApplication_||
                               !sourceLoadApplication_->enabled||sourceLoadApplication_->state!=fogPopulation.randomOwner||
                               !fogPopulation.randomOwner->known||sourceLoadApplication_->entities()!=worldApplication_||
                               &fogPopulation.randomOwner->rawRandom308030!=&fogPopulation.randomOwner->sharedNoise308030.sharedRandomState)return false;
                        }
                        const auto& scan=fogPopulation.retirementScan;
                        if(!g::OriginalTrackParticleCold50LA::sourceFogRetirementHistoryNative(scan))return false;
                        if(scan.step){
                            if(!successorFog||!fogPopulation.entered||fogPopulation.retirementEmpty||
                               fieldTailStep>18||frameStep>28||scan.readerEmitter!=e.get()||
                               scan.readerOccurrence!=&fogPopulation||scan.publication.manager!=fogPopulation.manager||
                               scan.publication.generation!=e->entity.sessionGeneration||
                               fogPopulation.retirementCount128!=scan.first128)return false;
                            constexpr std::uint32_t pending[]={0,0x1605ac,0x16055c,0x160564,0x16056c,0x160570,
                                0x160578,0x1605a8,0x1605ac,0x1605b8,0x1605bc};
                            if(scan.step<10){
                                if(fogPopulation.pc!=pending[scan.step]||fogPopulation.creationIndexInitial||fogPopulation.volume228||
                                   fogPopulation.creationCount128||fogPopulation.capacity130||fogPopulation.randomPending||
                                   fogPopulation.randomDrawCount)return false;
                            }else{
                                //Exact initial-creation and pre-stack RNG refusal frontiers.
                                //Later range/position PCs are checked by their existing receipts above.
                                std::uint32_t expected=0;
                                const bool randomStarted=fogRandomRecorded||fogPopulation.randomPending;
                                if(!fogPopulation.creationIndexInitial){
                                    if(fogPopulation.volume228||fogPopulation.volumePositive||fogPopulation.creationCount128||
                                       fogPopulation.capacity130||fogPopulation.capacityAvailable||randomStarted)return false;
                                    expected=0x1605bc;
                                }else if(!fogPopulation.volume228){
                                    if(fogPopulation.volumePositive||fogPopulation.creationCount128||fogPopulation.capacity130||
                                       fogPopulation.capacityAvailable||randomStarted)return false;
                                    expected=0x160abc;
                                }else if(!fogPopulation.volumePositive){
                                    if(fogPopulation.creationCount128||fogPopulation.capacity130||fogPopulation.capacityAvailable||randomStarted)return false;
                                    expected=0x160ac4;
                                }else if(!*fogPopulation.volumePositive){
                                    if(fogPopulation.creationCount128||fogPopulation.capacity130||fogPopulation.capacityAvailable||randomStarted)return false;
                                    expected=0x160ad4;
                                }else if(!fogPopulation.creationCount128){
                                    if(fogPopulation.capacity130||fogPopulation.capacityAvailable||randomStarted||
                                       (fogPopulation.pc!=0x1605c4u&&fogPopulation.pc!=0x1605c8u))return false;
                                }else if(!fogPopulation.capacity130){
                                    if(fogPopulation.capacityAvailable||randomStarted)return false;expected=0x1605cc;
                                }else if(!fogPopulation.capacityAvailable){
                                    if(randomStarted)return false;expected=0x1605d0;
                                }else{
                                    if(*fogPopulation.creationCount128!=1||*fogPopulation.capacity130!=5000||
                                       !*fogPopulation.capacityAvailable)return false;
                                    if(fogPopulation.randomPending){if(fogRandomRecorded)return false;expected=0x1605dc;}
                                    else if(!fogRandomRecorded)return false;
                                    else if(!fogPopulation.randomStack70Copied){
                                        const auto draws=fogPopulation.randomDrawCount;
                                        if(draws==0&&convertedMask==0&&unitMask==0&&randomStackMask==0)expected=0x308038;
                                        else if(draws==1&&randomStackMask==0){
                                            if(convertedMask==0&&unitMask==0)expected=0x1605ec;
                                            else if(convertedMask==1&&unitMask==0)expected=0x1605fc;
                                            else if(convertedMask==1&&unitMask==1)expected=0x308038;
                                        }else if(draws==2&&randomStackMask==0){
                                            if(convertedMask==1&&unitMask==1)expected=0x160608;
                                            else if(convertedMask==3&&unitMask==1)expected=0x160618;
                                            else if(convertedMask==3&&unitMask==3)expected=0x308038;
                                        }else if(draws==3){
                                            if(convertedMask==3&&unitMask==3&&randomStackMask==0)expected=0x160624;
                                            else if(convertedMask==7&&unitMask==3&&randomStackMask==3)expected=0x160644;
                                        }
                                        if(!expected)return false;
                                    }
                                }
                                if(expected&&fogPopulation.pc!=expected)return false;
                            }
                            if(!e->borrowsWithdrawn){
                                const auto& priorRow=l.rows[i-1];
                                if(!priorRow||priorRow->sourceEmitters.empty()||!priorRow->sourceEmitters[0])return false;
                                const auto& prior=*priorRow->sourceEmitters[0];const auto& priorPopulation=prior.type12Population;
                                if(&prior==e.get()||!prior.constructorReturned||prior.dispatch08!=0x3cd930u||
                                   !prior.counted||!prior.enrolled||prior.borrowsWithdrawn||!prior.sourceRegistered||!prior.rowConfigReturned||
                                   !prior.type12LoaderReturned||!priorPopulation.returned||prior.manager70!=e->manager70||
                                   scan.publication.emitter!=&prior||scan.publication.occurrence!=&priorPopulation||
                                   !g::OriginalTrackParticleCold50LA::sameSourceFogCountNative(scan.publication,priorPopulation.countStore)||
                                   !e->manager70||!e->manager70->sourceFogRetirementCurrentNative(scan,e.get(),&fogPopulation))return false;
                            }
                        }
                        //First occurrence's epoch0 observations remain unchanged.
                        //The successor uses its separate Track-issued count1 reads.
                        if((fogPopulation.retirementCount128&&*fogPopulation.retirementCount128!=(scan.step?1:0))||
                           (fogPopulation.creationCount128&&*fogPopulation.creationCount128!=(successorFog?1:0)))return false;
                        if(successorFog&&fogPopulation.entered&&!scan.step&&
                           (fogPopulation.pc!=0x1605a8u||fogPopulation.retirementCount128||fogPopulation.retirementEmpty||
                            fogPopulation.creationIndexInitial||fogPopulation.volume228||fogPopulation.returned))return false;
                        if((!successorFog&&fogPopulation.creationIndexInitial.has_value()!=fogPopulation.retirementEmpty)||
                           (successorFog&&fogPopulation.creationIndexInitial&&scan.step!=10)||
                           (fogPopulation.creationIndexInitial&&*fogPopulation.creationIndexInitial!=0))return false;
                        if(!fogPopulation.entered){
                            if(fogPopulation.manager||fogPopulation.retirementEmpty||fogPopulation.randomPending||fogPopulation.returned||
                               fogPopulation.retirementCount128||fogPopulation.creationCount128||fogPopulation.capacity130||
                               fogPopulation.volume228||fogPopulation.volumePositive||fogPopulation.capacityAvailable||fogPopulation.pc)return false;
                        }else{
                            if(!subtype12||!fogPopulation.manager||!fogPopulation.pc||
                               e->type12PopulationPending==fogPopulation.returned)return false;
                            if(fogPopulation.retirementEmpty&&(!fogPopulation.retirementCount128||*fogPopulation.retirementCount128>0))return false;
                            if(fogPopulation.volume228&&(!(successorFog?(scan.step==10&&fogPopulation.creationIndexInitial.has_value()):
                                   fogPopulation.retirementEmpty)||e->known[0x228/4]!=0xffffffffu||
                               *fogPopulation.volume228!=e->words[0x228/4]))return false;
                            if(fogPopulation.volumePositive){
                                if(!fogPopulation.volume228||(*fogPopulation.volume228&0x7f800000u)==0x7f800000u||
                                   ((*fogPopulation.volume228&0x7f800000u)==0&&(*fogPopulation.volume228&0x7fffffu)))return false;
                                const auto volume=*fogPopulation.volume228;
                                if(*fogPopulation.volumePositive!=((volume&0x80000000u)==0&&(volume&0x7fffffffu)!=0))return false;
                            }
                            if(fogPopulation.creationCount128&&fogPopulation.volumePositive!=true)return false;
                            if(fogPopulation.capacity130&&!fogPopulation.creationCount128)return false;
                            if(fogPopulation.capacityAvailable&&(!fogPopulation.capacity130||!fogPopulation.creationCount128||
                               *fogPopulation.capacityAvailable!=(*fogPopulation.creationCount128<*fogPopulation.capacity130)))return false;
                            if(fogPopulation.randomPending&&(fogPopulation.capacityAvailable!=true||fogPopulation.pc!=0x1605dcu||
                               fogPopulation.returned||fogPopulation.randomDrawCount))return false;
                            if(fogPopulation.randomDrawCount&&fogPopulation.randomPending)return false;
                            if(fogPopulation.returned&&(fogPopulation.pc!=0x160b00u||!fogPopulation.volumePositive||
                               (successorFog?(fogPopulation.retirementEmpty||scan.step!=10||frameStep!=28||
                                   countStep!=4||!countStore.stored||fogPopulation.volumePositive!=true):
                                   (!fogPopulation.retirementEmpty||(frameStep!=28&&*fogPopulation.volumePositive&&
                                       fogPopulation.capacityAvailable!=false)))))return false;
                            if(!e->borrowsWithdrawn){
                                const auto* manager=e->manager70;
                                if(manager!=fogPopulation.manager||!manager||!manager->sourceActiveCount128Known_)return false;
                                if((fogPopulation.retirementCount128&&!scan.step&&!manager->sourceFogCountObservationCurrentNative(
                                       *fogPopulation.retirementCount128,&fogPopulation))||
                                   (fogPopulation.creationCount128&&!(successorFog?
                                       manager->sourceFogSuccessorCountObservationCurrentNative(*fogPopulation.creationCount128,
                                           scan,e.get(),&fogPopulation):manager->sourceFogCountObservationCurrentNative(
                                               *fogPopulation.creationCount128,&fogPopulation)))||
                                   (fogPopulation.capacity130&&*fogPopulation.capacity130!=manager->capacity130_))return false;
                            }
                        }
                        if((subtype0||subtype1||subtype2||subtype9||subtype11||subtype12)&&(e->type8GroupsNonNull||e->type8GroupIndex||e->type8GroupCopyMask||e->type8PropertiesReturned||e->type8LoaderReturned))return false;
                        if(groupsNonNull&&(!config.commonReturned_||
                           (subtype0?config.type0NoiseReturned_!=2:(subtype2?config.type2RangeReturned_!=2:(subtype11?!config.type11MovingReturned_:(subtype12?config.type12FlagMask_!=3:(!subtype1&&!(subtype9?e->type9VelocityCopied:e->type8VelocityCopied))))))))return false;
                        if(groupIndex&&(groupsNonNull!=true||e->known[0x1b8/4]!=0xffffffffu||e->words[0x1b8/4]!=*groupIndex))return false;
                        if(groupCopyMask!=0&&groupCopyMask!=1&&groupCopyMask!=3&&groupCopyMask!=7)return false;
                        if(groupCopyMask&&(!groupsNonNull||(*groupsNonNull&&!groupIndex)))return false;
                        if(propertiesReturned!=(subtype0?config.emitterType0Returned_:(subtype1?config.emitterType1Returned_:(subtype2?config.emitterType2Returned_:(subtype9?config.emitterType9Returned_:(subtype11?config.emitterType11Returned_:(subtype12?config.emitterType12Returned_:config.emitterType8Returned_))))))||
                           (propertiesReturned&&groupCopyMask!=7))return false;
                        if(e->emitterFileLoadReturned&&(!propertiesReturned||!config.sourceClosed_||!e->configNativeWithdrawn))return false;
                        if((subtype0?e->type0LoaderReturned:(subtype1?e->type1LoaderReturned:(service.emitterType==2u?e->type2LoaderReturned:(service.emitterType==9u?e->type9LoaderReturned:(service.emitterType==11u?e->type11LoaderReturned:(service.emitterType==12u?e->type12LoaderReturned:e->type8LoaderReturned))))))&&!e->emitterFileLoadReturned)return false;
                        if((groupCopyMask&1)&&e->known[0x1b4/4]!=0xffffffffu)return false;
                        if((groupCopyMask&2)&&e->known[0x1b0/4]!=0xffffffffu)return false;
                        if(groupCopyMask&4){
                            if(e->known[0x190/4]!=0xffffffffu)return false;
                            if(subtype12&&frame.frameStored190){
                                //The group copy and record28 retain the first
                                //frame; the later emitter190 store has its own receipt.
                                if(!fogPopulation.frameInput190||*fogPopulation.frameInput190!=e->words[0x1b0/4]||
                                   !frame.selectedWord||e->words[0x190/4]!=*frame.selectedWord)return false;
                            }else if(e->words[0x190/4]!=e->words[0x1b0/4])return false;
                        }
                        if(groupsNonNull==false){
                            if((groupCopyMask&1)&&e->words[0x1b4/4]!=((subtype0||subtype1)?0x1fu:(subtype2?0x2fu:(subtype9?0x55u:(subtype11?0x2fu:(subtype12?0x55u:0x40u))))))return false;
                            if((groupCopyMask&2)&&e->words[0x1b0/4]!=((subtype0||subtype1)?0x10u:(subtype2?0x20u:(subtype9?0x64u:(subtype11?0x20u:(subtype12?0x64u:0x54u))))))return false;
                        }
                        if(!e->borrowsWithdrawn&&groupsNonNull){
                            if(!e->manager70||(*groupsNonNull!=(e->manager70->sourceGroups13C_!=nullptr)))return false;
                            if(groupIndex){
                                const auto& manager=*e->manager70;const auto index=*groupIndex;
                                if(!manager.sourceGroupRows_||manager.sourceGroups13C_!=manager.sourceGroupStorage13C_.get()||
                                   index>=manager.sourceGroupCapacity_)return false;
                                const auto& lanes=manager.sourceGroupRows_[index].lanes;
                                if((groupCopyMask&1)&&(!lanes[2]||e->words[0x1b4/4]!=static_cast<std::uint32_t>(*lanes[2])))return false;
                                if((groupCopyMask&2)&&(!lanes[1]||e->words[0x1b0/4]!=static_cast<std::uint32_t>(*lanes[1])))return false;
                            }
                        }
                        if(e->borrowsWithdrawn&&!e->configNativeWithdrawn)return false;
                    }
                    if(e->borrowsWithdrawn){if(e->manager70||e->modelOwner||e->model74)return false;}
                    else{
                        if(e->managerStored){
                            if(!trackParticles_||e->manager70!=trackParticles_.get()||!sourceParticle4A0_||
                               *sourceParticle4A0_!=trackParticles_.get()||sourceParticleStageNativeClosed_||sourceParticle4A0NativeWithdrawn_||
                               liveEmitterManagerBorrows==std::numeric_limits<std::size_t>::max())return false;
                            ++liveEmitterManagerBorrows;
                        }else if(e->manager70)return false;
                        if(e->modelStored){
                            if(!sourceStaticOwner_||!e->modelOwner||!e->model74)return false;
                            bool found=false;
                            for(const auto& models:sourceStaticOwner_->sourceWrapperLedger_)for(const auto& wrapper:models)
                                if(wrapper&&wrapper->sourceModel&& &wrapper->sourceModel->scene==e->modelOwner){
                                    if(wrapper->sourceModel->nativeRetired||wrapper->model8!=e->modelOwner||
                                       !sourceStaticOwner_->sourceModelCurrentNative(*wrapper)||e->modelOwner->model()!=e->model74)return false;
                                    if(fogAttachedEntered&&(!e->modelOwner->sourceFogTransformGraphCurrentNative(wrapper->sourceModel->graph(),
                                        fogPositionCall,fogDirectionCall)))return false;
                                    found=true;
                                }
                            if(!found)return false;
                            std::size_t expected=0;
                            for(const auto& other:l.rows)if(other)for(const auto& item:other->sourceEmitters)
                                if(item&&!item->borrowsWithdrawn&&item->modelStored&&item->modelOwner==e->modelOwner)++expected;
                            if(e->modelOwner->sourceParticleEmitterBorrowers_!=expected)return false;
                        }else if(e->modelOwner||e->model74)return false;
                    }
                    if(i<4&&j==0&&(e->managerStored!=(service.emitterType==0u?service.type0ManagerStored:(service.emitterType==1u?service.type1ManagerStored:(service.emitterType==2u?service.type2ManagerStored:(service.emitterType==9u?service.type9ManagerStored:(service.emitterType==11u?service.type11ManagerStored:(service.emitterType==12u?service.type12ManagerStored:service.type8ManagerStored))))))||e->modelStored!=(service.emitterType==0u?service.type0ModelStored:(service.emitterType==1u?service.type1ModelStored:(service.emitterType==2u?service.type2ModelStored:(service.emitterType==9u?service.type9ModelStored:(service.emitterType==11u?service.type11ModelStored:(service.emitterType==12u?service.type12ModelStored:service.type8ModelStored))))))||
                       e->offsetStored!=(service.emitterType==0u?service.type0OffsetStored:(service.emitterType==1u?service.type1OffsetStored:(service.emitterType==2u?service.type2OffsetStored:(service.emitterType==9u?service.type9OffsetStored:(service.emitterType==11u?service.type11OffsetStored:(service.emitterType==12u?service.type12OffsetStored:service.type8OffsetStored))))))||e->directoryCopied!=(service.emitterType==0u?service.type0DirectoryCopied:(service.emitterType==1u?service.type1DirectoryCopied:(service.emitterType==2u?service.type2DirectoryCopied:(service.emitterType==9u?service.type9DirectoryCopied:(service.emitterType==11u?service.type11DirectoryCopied:(service.emitterType==12u?service.type12DirectoryCopied:service.type8DirectoryCopied))))))||
                       e->filenameAppended!=(service.emitterType==0u?service.type0FilenameAppended:(service.emitterType==1u?service.type1FilenameAppended:(service.emitterType==2u?service.type2FilenameAppended:(service.emitterType==9u?service.type9FilenameAppended:(service.emitterType==11u?service.type11FilenameAppended:(service.emitterType==12u?service.type12FilenameAppended:service.type8FilenameAppended))))))||
                       e->configConstructAttempted!=service.emitterConfigConstructAttempted||
                       e->configConstructReturned!=service.emitterConfigConstructReturned||
                       e->configOpenAttempted!=service.emitterConfigOpenAttempted||
                       e->configOpenReturned!=service.emitterConfigOpenReturned))return false;
                }
                if(j<r->sourceEmitterPublishedSlots&&(!r->sourceEmitterPointers||
                   r->sourceEmitterPointers[j]!=(e?&e->entity:nullptr)||(e&&!e->constructorReturned)))return false;
            }
            if(r->sourceEmitterArrayPublished){
                if(r->sourceEmitterPublishedSlots!=r->sourceEmitters.size()||!r->sourceEmitterArray00||
                   *r->sourceEmitterArray00!=r->sourceEmitterPointers.get())return false;
            }else if(r->sourceEmitterArray00)return false;
        }else if(r->sourceEmitterPointers||!r->sourceEmitters.empty()||r->sourceEmitterAttempts||
                 r->sourceEmitterPublishedSlots||r->sourceEmitterArrayPublished||r->sourceEmitterArray00)return false;
        if(r->unknown0C){if(i>=4||!service.rowCount0CStored||!r->sourceEmitterArrayPublished||
            !proof||*r->unknown0C!=static_cast<std::uint32_t>(proofCount))return false;}
        else if(i<4&&service.rowCount0CStored)return false;
        if(r->unknown10){if(i>=4||!service.rowStart10Stored||!r->unknown0C||!proof||*r->unknown10!=static_cast<std::uint32_t>(proofStart))return false;}
        else if(i<4&&service.rowStart10Stored)return false;
        if(r->sourceDescriptorPublished){
            if(!r->sourceDescriptorAttempted||!r->sourceDescriptor04||*r->sourceDescriptor04!=r->descriptor04.get())return false;
        }else if(r->descriptor04||r->sourceDescriptor04||r->sourceDescriptorAttempted)return false;
        if(!r->descriptor04){if(l.sourceRowReturned[i]&&!r->sourceDescriptorFreed)return false;continue;}
        const auto& d=*r->descriptor04;
        if(!d.sourceMode||d.actualAttachment||d.loaded||d.child10C||d.offset320||d.offsetW32C||
           (d.sourceKnownMask&~0x7ffu)||(d.sourceOffsetKnown&~15u))return false;
        const auto textCurrent=[](const std::array<char,260>& text,const std::optional<std::size_t>& n){return !n||(*n<text.size()&&text[*n]==0);};
        if(!textCurrent(d.sourceAttach08,d.sourceAttachBytes)||!textCurrent(d.sourceChild10C,d.sourceChildBytes)||
           !textCurrent(d.sourceEmitter210,d.sourceEmitterBytes)||
           (bool(d.instances330)!=bool(d.sourceKnownMask&512u))||
           (bool(d.startOn334)!=bool(d.sourceKnownMask&1024u))||
           (l.sourceRowReturned[i]&&!d.sourceLoaded))return false;
    }
    if((trackParticles_?trackParticles_->sourceParticleEmitterBorrowers_:0u)!=liveEmitterManagerBorrows)return false;
    return preflightSourceParticleNextRowNative(); //No unknown raw slot or unproduced descriptor byte was read.
}
bool Owner::abortSourceWorldParticleDescriptorsNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceWorldParticleCountNative())return false;
    //Withdraw only native next-call borrows before their row/descriptor owners.
    //Do not fabricate source NULL stores or a row callback return.
    for(auto& next:sourceParticleNextRows_)if(next.entered){
        next.row=nullptr;next.descriptor=nullptr;next.nativeWithdrawn=true;
    }
    if(sourceParticleDescriptorPhase_==SourceParticleDescriptorPhase::unreached||
       sourceParticleDescriptorPhase_==SourceParticleDescriptorPhase::nativeClosed)return true;
    if(!withdrawSourceParticleEmitterBorrowsNative())return false;
    if(nativeParticleList3D8_){auto& l=*nativeParticleList3D8_;
        //Preflight above validates every counted partial constructor before any release.
        for(auto& row:l.rows)if(row)for(auto& emitter:row->sourceEmitters)if(emitter){
            if(!releaseSourceParticleEmitterCountNative(emitter->entity,emitter->counted,emitter->enrolled,
                emitter->entityActivityStored&&emitter->sourceStartOn==false?0:3,emitter->sourceRegistered?0x402u:0u))return false;
        }
        //Potential authority refusal precedes every plain backing withdrawal.
        if(!releaseSourceParticleListCountNative(l.entity,l.counted,l.sourceEnrolled))return false;
        sourceParticleList3D8_.reset();
        for(std::size_t i=4;i-->0;)if(l.rows[i]){
            l.rows[i]->sourceDescriptor04.reset();l.rows[i]->descriptor04.reset();l.rows[i].reset();}
        l.sourceArray34.reset();l.pointers34.reset();nativeParticleList3D8_.reset();
    }else sourceParticleList3D8_.reset();
    sourceParticleDescriptorConfigArg_=nullptr;sourceParticleDescriptorPhase_=SourceParticleDescriptorPhase::nativeClosed;
    return true; //Native disposal only, not source NULL/destructor/loader history.
}
bool Owner::loadSourceWorldParticleDescriptors2722F0(std::uint32_t& pc)noexcept {
    pc=0;std::array<std::uint32_t,4> origin,forward;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceParticleDescriptorPhase_!=SourceParticleDescriptorPhase::unreached||
       !sourceWorldParticleDescriptorsInvocationCurrent()||nativeParticleList3D8_||
       !sourceParticleList3D8_||*sourceParticleList3D8_||!sourceWorldObjectConfig_||
       !copySourceWorldStartDefaults(origin,forward)||!reserveSourceParticleListNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourceParticleDescriptorPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourceParticleDescriptorPhase_=SourceParticleDescriptorPhase::bounded;return false;};
    sourceParticleDescriptorAttempted_=true;sourceParticleDescriptorPhase_=SourceParticleDescriptorPhase::entered;
    at(0x272308);nativeParticleList3D8_.reset(new(std::nothrow) NativeParticleList3D8);
    if(nativeParticleList3D8_){auto& l=*nativeParticleList3D8_;l.sourceMode=true;
        if(!constructSourceParticleList27DC90(l.entity,l.counted,l.sourceEnrolled,pc))return stop(pc);
        at(0x27dcac);l.sourceConstructorMask|=1; //vtable08=3D24F0.
        at(0x27dcb0);l.count30=0;l.sourceConstructorMask|=2;
        at(0x27dcb4);l.pointers34.reset();l.sourceConstructorMask|=4;l.sourceConstructorReturned=true;
    }
    at(0x272324);sourceParticleList3D8_=nativeParticleList3D8_.get();sourceParticleDescriptorPublished_=true;
    at(0x27232c);if(!nativeParticleList3D8_)return stop(pc);
    auto& l=*nativeParticleList3D8_;auto& config=*sourceWorldObjectConfig_;
    at(0x272338);sourceParticleDescriptorConfigArg_=&config;
    a::OriginalStandardTextureDiagnostic50LA diagnostic;bool returned=false,found=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& present,std::uint32_t& copy)noexcept {
        if(!config.readSourceParticleDescriptorField158240(key,value,present,copy,diagnostic)){
            pc=diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    at(0x27de34);sourceParticleDescriptorSuccessS1_=0; //Selector call delay slot before effects.
    at(0x27de30);if(!config.selectSourceParticleDescriptorSection157D00(0,returned,diagnostic))return stop(diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_);
    if(!returned)return stop(0x27de40); //Unjoined missing-root diagnostic/false return, not success.
    std::int32_t count;at(0x27de64);if(!read.integer("numberofemitters",0,&count,found))return stop(pc);
    sourceParticleDescriptorCountStack16C_=count;
    at(0x27de70);if(count!=4)return stop(pc); //Exact authenticated cohort, no policy clamp.
    at(0x27deb0);l.count30=count;
    at(0x27deb8);l.sourceArrayAttempted=true;l.pointers34.reset(new(std::nothrow) NativeParticleRow27DE00*[4]);
    at(0x27dec0);l.sourceArray34=l.pointers34.get();l.sourceArrayPublished=true;
    at(0x27dec4);sourceParticleDescriptorRowS0_=0;
    at(0x27decc);sourceParticleDescriptorOffsetS2_=0;
    for(std::size_t i=0;i<4;++i){
        if(!sourceParticleDescriptorRowS0_||*sourceParticleDescriptorRowS0_!=static_cast<std::int32_t>(i)||
           !sourceParticleDescriptorOffsetS2_||*sourceParticleDescriptorOffsetS2_!=i*4)return stop(0x27ded0);
        at(0x27dedc);constexpr char prefix[]="ParticleEmitter";
        std::memcpy(sourceParticleDescriptorName60_.data(),prefix,sizeof(prefix)-1);
        sourceParticleDescriptorName60_[sizeof(prefix)-1]=static_cast<char>('1'+i);
        sourceParticleDescriptorName60_[sizeof(prefix)]=0;sourceParticleDescriptorNameKnown_=true;
        at(0x27dee8);if(!config.selectSourceParticleDescriptorSection157D00(static_cast<std::uint32_t>(i+1),returned,diagnostic))return stop(diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_);
        if(!returned)return stop(0x27def8); //Do not invent a NULL slot for missing section.
        at(0x27df14);l.sourceRowAttempted[i]=true;l.rows[i].reset(new(std::nothrow) NativeParticleRow27DE00);
        if(l.rows[i]){auto& r=*l.rows[i];r.sourceMode=true;
            at(0x27df2c);r.sourceConstructorMask|=1;
            at(0x27df30);r.emitterPointers00.reset();r.sourceConstructorMask|=2;
            at(0x27df34);r.descriptor04.reset();r.sourceConstructorMask|=4;}
        at(0x27df40);if(!l.pointers34)return stop(pc); //Allocated row stays owned if destination is NULL.
        l.pointers34[i]=l.rows[i].get();l.sourceSlotPublished[i]=true;
        at(0x27df50);if(!l.rows[i])return stop(pc);
        auto& r=*l.rows[i];at(0x27c768);r.sourceDescriptorAttempted=true;
        r.descriptor04.reset(new(std::nothrow) NativeParticleDescriptor27C430(NativeParticleDescriptor27C430::SourceTag{}));
        if(r.descriptor04){auto& d=*r.descriptor04;
            at(0x27c44c);d.sourceKnownMask|=1; //vtable338=3D2588.
            at(0x27c454);d.sourceOffset320={{0,0,0,0x3f800000u}};d.sourceOffsetKnown=15;
            //Both 8BB0/8BB4 guard blocks are genuinely skipped:260!=4.
            at(0x27c500);std::memcpy(d.sourceAttach08.data(),"none",4);
            at(0x27c50c);d.sourceAttach08[4]=0;d.sourceAttachBytes=4;d.sourceKnownMask|=2;
            at(0x27c5b8);std::memcpy(d.sourceEmitter210.data(),"none",4);
            at(0x27c5c4);d.sourceEmitter210[4]=0;d.sourceEmitterBytes=4;d.sourceKnownMask|=4;
            at(0x27c5c8);d.startingZone00=0;d.sourceKnownMask|=8;
            at(0x27c5cc);d.type04=0;d.sourceKnownMask|=16;
            at(0x27c5dc);d.sourceOffset320=origin;d.sourceOffsetKnown=15;d.sourceKnownMask|=32;
        }
        at(0x27c780);r.sourceDescriptor04=r.descriptor04.get();r.sourceDescriptorPublished=true;
        at(0x27c788);if(!r.descriptor04)return stop(pc);
        auto& d=*r.descriptor04;
        at(0x27c628);if(!read.integer("startingzone",0,&d.startingZone00,found))return stop(pc);d.sourceKnownMask|=64;
        at(0x27c640);if(!read.integer("typeid",0,&d.type04,found))return stop(pc);d.sourceKnownMask|=128;
        at(0x27c660);if(!read.text("attachtosoultree","NONE",d.sourceAttach08.data(),d.sourceAttach08.size(),found))return stop(pc);
        d.sourceAttachBytes=std::strlen(d.sourceAttach08.data());
        at(0x27c678);if(*d.sourceAttachBytes==4&&std::memcmp(d.sourceAttach08.data(),"NONE",4)==0)return stop(pc); //Different actual position branch unjoined.
        at(0x27c6c0);if(!read.text("childobject","NONE",d.sourceChild10C.data(),d.sourceChild10C.size(),found))return stop(pc);
        d.sourceChildBytes=std::strlen(d.sourceChild10C.data());d.sourceKnownMask|=256;
        at(0x27c6dc);if(!read.vector("offsetfromsoultree",origin,&d.sourceOffset320,found))return stop(pc);
        std::int32_t n;at(0x27c6f4);if(!read.integer("numberofinstances",1,&n,found))return stop(pc);d.instances330=n;d.sourceKnownMask|=512;
        at(0x27c714);if(!read.text("emitterfilename","NONE",d.sourceEmitter210.data(),d.sourceEmitter210.size(),found))return stop(pc);
        d.sourceEmitterBytes=std::strlen(d.sourceEmitter210.data());
        at(0x27c72c);if(!read.integer("starton",1,&n,found))return stop(pc);d.startOn334=n;d.sourceKnownMask|=1024;
        at(0x27c740);d.sourceLoaded=true;l.sourceRowReturned[i]=true;
        at(0x27df60);++*sourceParticleDescriptorSuccessS1_;
        at(0x27df68);*sourceParticleDescriptorOffsetS2_+=4;
        at(0x27df6c);++*sourceParticleDescriptorRowS0_;
    }
    at(0x27df8c);if(sourceParticleDescriptorLogAttempted_)return stop(pc);
    sourceParticleDescriptorLogAttempted_=true;
    //Explicit native diagnostic adapter, not original stdio emulation. Exact
    //source format and actual completed arguments4/4; source ignores result.
    constexpr char message[]="Read 4 particle emitters out of a total of 4.\n";
    sourceParticleDescriptorLogWritten_=std::fwrite(message,1,sizeof(message)-1,stdout);
    sourceParticleDescriptorLogError_=std::ferror(stdout)!=0;
    at(0x27df94); //Actual loader v0=1;27233C skips source deletion.
    at(0x271d90);sourceParticleDescriptorPhase_=SourceParticleDescriptorPhase::returned;return true;
}
}
