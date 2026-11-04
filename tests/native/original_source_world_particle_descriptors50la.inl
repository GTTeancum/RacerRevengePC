#pragma once
#include "original_source_world_light_read50la.inl"
#include <cstdio>
#include <cstring>
#include <new>
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
                    //Subtype1 is recovered only through loader return27D8D0.
                    //Its distinct15CFF0 wrapper has not positioned/activated
                    //or registered this emitter, even on the attached branch.
                    if(service.emitterType==1u&&(e->positionCopyCount||e->sourceStartOn||
                       e->particleActivationReturned||e->entityActivityStored||e->entityActivityReturned||
                       e->rowConfigReturned||e->sourceRegistered))return false;
                    if(e->positionCopyCount>2||(e->positionCopyCount&&(!(service.emitterType==0u?e->type0LoaderReturned:e->type8LoaderReturned)||e->modelStored)))return false;
                    if(e->sourceStartOn&&(!(service.emitterType==0u?e->type0LoaderReturned:e->type8LoaderReturned)||!r->unknown10||
                       *e->sourceStartOn!=(*r->unknown10!=0)||(!e->modelStored&&e->positionCopyCount!=2)))return false;
                    if(e->particleActivationReturned&&(!e->sourceStartOn||e->known[0x19c/4]!=0xffffffffu||e->words[0x19c/4]))return false;
                    if(e->entityActivityStored&&!e->particleActivationReturned)return false;
                    if(e->entityActivityReturned&&!e->entityActivityStored)return false;
                    if(e->rowConfigReturned&&!e->entityActivityReturned)return false;
                    if(e->sourceRegistered&&!e->rowConfigReturned)return false;
                    if(e->positionCopyCount){
                        if(!proofOffset)return false;
                        for(std::size_t lane=0;lane<4;++lane){
                            if(e->known[0x30/4+lane]!=0xffffffffu||e->words[0x30/4+lane]!=(*proofOffset)[lane])return false;
                            if(e->positionCopyCount==2&&(e->known[0x40/4+lane]!=0xffffffffu||e->words[0x40/4+lane]!=e->words[0x30/4+lane]))return false;
                        }
                    }
                    if((e->managerStored||e->modelStored||e->offsetStored||e->directoryCopied||e->filenameAppended)&&
                       (i>=4||j!=0||!e->constructorReturned||
                        !((service.emitterType==8u&&service.type8LoadEntered)||
                          (service.emitterType==0u&&service.type0LoadEntered)||
                          (service.emitterType==1u&&service.type1LoadEntered))))return false;
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
                       (service.emitterType!=0u&&service.emitterType!=1u&&service.emitterType!=8u)))return false;
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
                           (service.emitterType==0u?0x15c330u:(service.emitterType==1u?0x15d560u:0x15e6d0u)))return false;
                        constexpr std::uint32_t type0Offsets[]={0x21c,0x220,0x208,0x20c,0x210,0x214,0x218,0x224,0x22c,0x230,0x234,0x238,0x228,0x23c};
                        for(std::size_t n=0;n<config.type0ScalarCount_;++n)
                            if(e->known[type0Offsets[n]/4]!=0xffffffffu)return false;
                        for(std::size_t vector=0;vector<2;++vector)for(std::size_t lane=0;lane<3;++lane)
                            if((config.type0NoiseWritten_[vector]&(1u<<lane))&&
                               e->known[(vector?0x250:0x240)/4+lane]!=0xffffffffu)return false;
                        for(std::size_t vector=0;vector<2;++vector)for(std::size_t lane=0;lane<3;++lane)
                            if((config.emitterVectorWritten_[vector]&(1u<<lane))&&
                               e->known[(vector?0x210:0x200)/4+lane]!=0xffffffffu)return false;
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
                        const bool subtype0=service.emitterType==0u,subtype1=service.emitterType==1u;
                        const auto& groupsNonNull=subtype0?e->type0GroupsNonNull:(subtype1?e->type1GroupsNonNull:e->type8GroupsNonNull);
                        const auto& groupIndex=subtype0?e->type0GroupIndex:(subtype1?e->type1GroupIndex:e->type8GroupIndex);
                        const auto groupCopyMask=subtype0?e->type0GroupCopyMask:(subtype1?e->type1GroupCopyMask:e->type8GroupCopyMask);
                        const bool propertiesReturned=subtype0?e->type0PropertiesReturned:(subtype1?e->type1PropertiesReturned:e->type8PropertiesReturned);
                        if(!subtype0&&(e->type0GroupsNonNull||e->type0GroupIndex||e->type0GroupCopyMask||e->type0PropertiesReturned||e->type0LoaderReturned))return false;
                        if(!subtype1&&(e->type1GroupsNonNull||e->type1GroupIndex||e->type1GroupCopyMask||e->type1PropertiesReturned||e->type1LoaderReturned))return false;
                        if((subtype0||subtype1)&&(e->type8GroupsNonNull||e->type8GroupIndex||e->type8GroupCopyMask||e->type8PropertiesReturned||e->type8LoaderReturned))return false;
                        if(groupsNonNull&&(!config.commonReturned_||
                           (subtype0?config.type0NoiseReturned_!=2:(!subtype1&&!e->type8VelocityCopied))))return false;
                        if(groupIndex&&(groupsNonNull!=true||e->known[0x1b8/4]!=0xffffffffu||e->words[0x1b8/4]!=*groupIndex))return false;
                        if(groupCopyMask!=0&&groupCopyMask!=1&&groupCopyMask!=3&&groupCopyMask!=7)return false;
                        if(groupCopyMask&&(!groupsNonNull||(*groupsNonNull&&!groupIndex)))return false;
                        if(propertiesReturned!=(subtype0?config.emitterType0Returned_:(subtype1?config.emitterType1Returned_:config.emitterType8Returned_))||
                           (propertiesReturned&&groupCopyMask!=7))return false;
                        if(e->emitterFileLoadReturned&&(!propertiesReturned||!config.sourceClosed_||!e->configNativeWithdrawn))return false;
                        if((subtype0?e->type0LoaderReturned:(subtype1?e->type1LoaderReturned:e->type8LoaderReturned))&&!e->emitterFileLoadReturned)return false;
                        if((groupCopyMask&1)&&e->known[0x1b4/4]!=0xffffffffu)return false;
                        if((groupCopyMask&2)&&e->known[0x1b0/4]!=0xffffffffu)return false;
                        if((groupCopyMask&4)&&(e->known[0x190/4]!=0xffffffffu||e->words[0x190/4]!=e->words[0x1b0/4]))return false;
                        if(groupsNonNull==false){
                            if((groupCopyMask&1)&&e->words[0x1b4/4]!=((subtype0||subtype1)?0x1fu:0x40u))return false;
                            if((groupCopyMask&2)&&e->words[0x1b0/4]!=((subtype0||subtype1)?0x10u:0x54u))return false;
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
                                    found=true;
                                }
                            if(!found)return false;
                            std::size_t expected=0;
                            for(const auto& other:l.rows)if(other)for(const auto& item:other->sourceEmitters)
                                if(item&&!item->borrowsWithdrawn&&item->modelStored&&item->modelOwner==e->modelOwner)++expected;
                            if(e->modelOwner->sourceParticleEmitterBorrowers_!=expected)return false;
                        }else if(e->modelOwner||e->model74)return false;
                    }
                    if(i<4&&j==0&&(e->managerStored!=(service.emitterType==0u?service.type0ManagerStored:(service.emitterType==1u?service.type1ManagerStored:service.type8ManagerStored))||e->modelStored!=(service.emitterType==0u?service.type0ModelStored:(service.emitterType==1u?service.type1ModelStored:service.type8ModelStored))||
                       e->offsetStored!=(service.emitterType==0u?service.type0OffsetStored:(service.emitterType==1u?service.type1OffsetStored:service.type8OffsetStored))||e->directoryCopied!=(service.emitterType==0u?service.type0DirectoryCopied:(service.emitterType==1u?service.type1DirectoryCopied:service.type8DirectoryCopied))||
                       e->filenameAppended!=(service.emitterType==0u?service.type0FilenameAppended:(service.emitterType==1u?service.type1FilenameAppended:service.type8FilenameAppended))||
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
