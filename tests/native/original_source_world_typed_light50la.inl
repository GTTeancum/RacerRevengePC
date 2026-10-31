#pragma once
#include <new>
#include "../../src/game/original_parameter_packet_history50la.h"
#include "../../platform/linux/native/original_ui_garage_screen_text50kg.h"
#include "../../src/game/original_world_light_assertion273abc.h"
#include "../../src/game/source_acos49t.h"
#include "../../src/game/native_directional_attenuation147268_proposal.h"
namespace ra_pod_scene50LA {
bool Owner::preflightSourceTypedLightsNative()const noexcept {
    if(!preflightSourceLightSiblingLinksNative())return false;
    for(std::size_t i=0;i<sourceTypedLights_.size();++i){
        if(!sourceTypedLights_[i])return false;const auto& f=*sourceTypedLights_[i];
        if(f.ordinal!=i+2||f.ordinal>=185||!preflightSourceFlareObserverNative(f))return false;
        if(f.nativeClosed){if(f.registryManager||f.light||f.wrapper||f.descriptor||f.lensStack00||f.argumentsBorrowed||f.counted||f.enrolled||
            f.sourceFlare||f.flareParameters54||f.flarePrimitiveParameters10||f.flareCounted||f.flareEnrolled||f.flarePositionBorrow||f.flareTextureBorrow||f.flareArgumentsBorrowed)return false;continue;}
        if(!worldApplication_||!nativeLightList3A8_||!nativeLightList3A8_->sourceMode||!nativeLightList3A8_->wrappers38||
           f.wrapper!=&nativeLightList3A8_->wrappers38[f.ordinal]||
           (!f.descriptor&&!f.descriptorDisposed)||f.descriptor!=f.wrapper->sourceDescriptor04.get()||f.guard>1||f.enabled>1||(f.guardMask&~3u))return false;
        if(f.descriptorDisposed){
            if(!f.retirementEntered||!f.retiredDescriptor||f.descriptor||f.wrapper->sourceDescriptor04||f.argumentsBorrowed||
               f.lensStack00||f.flareArgumentsBorrowed||f.flarePositionBorrow||f.flareTextureBorrow||!f.wrapperV0||*f.wrapperV0!=1||!f.wrapperReturned||
               (f.retiredDescriptor->type!=1&&f.retiredDescriptor->type!=3&&f.retiredDescriptor->type!=4)||f.retiredDescriptor->knownFlags!=0x7f||
               (f.retiredDescriptor->flags&0x28)||!f.retiredDescriptor->loaded||
               f.descriptorFreeMask!=(f.retiredDescriptor->type==4?8u:15u))return false;
            if(f.retiredDescriptor->type==1&&(!f.flareObserverCallReturned||!f.flare146AE0Returned||!f.flareC4Published||!(f.retiredDescriptor->flags&16u)))return false;
            if(f.retiredDescriptor->type!=1&&(f.retiredDescriptor->flags&16u))return false;
        }else if(f.retiredDescriptor||f.retirementEntered||f.descriptorFreeMask||f.wrapperV0||f.wrapperReturned||f.attachLock||f.attachCallReturned||f.nativeDetached)return false;
        if(f.attachCallReturned&&!f.attachLock)return false;
        if(f.zone&&*f.zone!=f.descriptorZone())return false;
        if(f.assertionPassed&&(!f.zone||f.guardMask!=3||!f.enabled))return false;
        if(f.ordinalStack08&&(*f.ordinalStack08!=static_cast<std::int32_t>(f.ordinal)||!f.published||
           f.lensStack00!=((!f.descriptorDisposed&&(f.descriptorFlags()&8u))?f.descriptor->lensTexture50.data():nullptr)))return false;
        if(f.argumentsBorrowed&&!f.ordinalStack08)return false;
        if(!f.light){if(f.counted||f.enrolled||f.defaults||f.returned||f.vtable08||f.luminance||f.configured||
            f.positionWords||f.attenuationWords||f.attenuationRange||(f.published&&f.wrapper->live00))return false;continue;}
        const auto& l=*f.light;const auto& e=l.entity;
        if(l.attached!=bool(f.attachCallReturned&&f.attachLock&&!*f.attachLock&&!f.nativeDetached)||
           (f.nativeDetached&&(!f.attachCallReturned||!f.attachLock||*f.attachLock)))return false;
        const auto& authority=worldApplication_->entityAuthority_;
        if(f.counted){if(e.reference00!=1||e.kind!=g::OriginalWreckComponentKind50LA::light146720||e.objectKey!=key(&e)||
            e.sessionGeneration!=worldEntity_.sessionGeneration||e.category28!=0xff||(e.flags29&15u)!=3u||e.nativeFlags29||
            e.parent1C!=(l.attached?&nativeLightList3A8_->entity:nullptr)||e.firstChild18!=(f.sourceFlare&&f.sourceFlare->lightAttached?&f.sourceFlare->entity:nullptr)||e.mask20!=((l.attached||f.nativeDetached)?2u:0u)||
            e.descendants24!=(f.sourceFlare&&f.sourceFlare->lightAttached?f.sourceFlare->entity.descendants24:0u)||!worldApplication_->objectCount423F60())return false;}
        else if(e.reference00)return false;
        if(f.enrolled){if(!f.counted||!authority.canRetire(e))return false;}
        else for(std::size_t n=0;n<authority.count_;++n)if(authority.entries_[n].node==&e)return false;
        const auto xyz=[](const std::array<std::uint32_t,4>& v){return std::array<float,3>{{
            g::directional_native_proposal::detail::fromWord(v[0]),g::directional_native_proposal::detail::fromWord(v[1]),
            g::directional_native_proposal::detail::fromWord(v[2])}};};
        if(f.positionWords&&(!f.descriptorPosition()||*f.positionWords!=*f.descriptorPosition()))return false;
        if(f.attenuationWords&&(!f.descriptorAttenuation()||*f.attenuationWords!=*f.descriptorAttenuation()))return false;
        if(l.rangeWritten&&!f.attenuationRange)return false;
        if(f.defaults){if(!f.counted||!f.enrolled||!f.vtable08||*f.vtable08!=0x3cd600||l.query30||l.flags33||l.knownFlags33!=7||
            l.type34!=((l.colorsWritten||f.spotlightConfigEntered||f.laterAmbientConfigEntered)?f.descriptorType():0u)||l.mode38!=((f.configured&&f.descriptorType()==1)?1u:3u)||
            l.diffuse3C!=(l.colorsWritten?(f.laterAmbientConfigEntered?std::array<float,4>{}:(f.spotlightHalfColor?*f.spotlightHalfColor:std::array<float,4>{{127.5f,127.5f,127.5f,127.5f}})):std::array<float,4>{{128,128,128,0}})||
            l.specular4C!=l.diffuse3C||l.ambient5C!=(l.colorsWritten?(f.laterAmbientConfigEntered?std::array<float,4>{{12.5f,12.5f,12.5f,127.5f}}:std::array<float,4>{}):l.diffuse3C)||
            l.position6C!=(f.positionWords?xyz(*f.positionWords):std::array<float,3>{})||l.look78!=(f.spotlightLookWords?xyz(*f.spotlightLookWords):std::array<float,3>{{1,0,0}})||
            l.positionA0!=(f.positionWords?*f.positionWords:worldApplication_->lightVector426F10_)||
            l.lookB0!=(f.spotlightLookWords?*f.spotlightLookWords:std::array<std::uint32_t,4>{{0,0,0,0x3f800000}})||
            l.attenuation8C!=(f.attenuationWords?xyz(*f.attenuationWords):std::array<float,3>{{1,0,0}})||
            l.range84!=(l.rangeWritten?*f.attenuationRange:g::SourceUiLightMath50KG::range146898())||l.field88!=0x3f800000||
            l.cone98!=f.spotlightCone98.value_or(0x3f490fdb)||l.cone9C!=f.spotlightCone9C.value_or(0x3fc90fdb)||l.fieldC0||
            l.fieldC4!=(f.flareC4Published&&!f.flareC4Withdrawn&&f.sourceFlare?&f.sourceFlare->entity:nullptr)||
            l.ordinalC8!=((l.colorsWritten||f.spotlightConfigEntered||f.laterAmbientConfigEntered)?static_cast<std::int32_t>(f.ordinal):-1)||
            l.luminanceCC!=(f.luminance?*f.luminance:0xbf800000u))return false;}
        if(l.fieldD8||l.registered!=(f.registryCountStored&&!f.registryWithdrawn)||l.retiring||l.lookWritten!=bool(f.spotlightLookWords)||
           (l.lookRevision&&!l.lookWritten))return false;
        if(f.spotlightConfigEntered){
            if(f.descriptorType()!=3||!f.descriptorLoaded()||(!f.argumentsBorrowed&&!f.descriptorDisposed)||
               (f.spotlightHalfColor&&!l.colorsWritten)||(f.spotlightLookWords&&!f.lookCompared)||
               (f.attenuationWords&&l.lookWritten&&!l.lookRevision)||
               (f.spotlightCone98&&!f.configured)||(f.spotlightCone9C&&!f.spotlightCone98))return false;
        }else if(f.spotlightHalfColor||f.spotlightLookWords||f.spotlightCone98||f.spotlightCone9C)return false;
        if(f.laterAmbientConfigEntered){
            if(f.spotlightConfigEntered||f.descriptorType()!=4||!f.descriptorLoaded()||(!f.argumentsBorrowed&&!f.descriptorDisposed)||
               f.descriptorPosition()||f.descriptorLook()||f.descriptorAttenuation()||
               f.positionCompared||f.lookCompared||f.positionWords||f.attenuationWords||f.attenuationRange||
               f.observerEntered||f.registryColorMask||f.registryA0Stored||
               (f.laterAmbientStoresReturned&&!f.registrationEntered))return false;
        }else if(f.laterAmbientStoresReturned)return false;
        if(!g::OriginalGarageObserverShared50KG::sourceAssertionHistoryCurrent(f.observerReturnAssertion)||
           (f.observerCallReturned&&(!f.observerReturnAssertion.passed||!f.secondaryReturned))||
           (f.registrationEntered&&!f.observerCallReturned&&!f.laterAmbientConfigEntered)||
           (f.registryColorMask!=0&&f.registryColorMask!=1&&f.registryColorMask!=3&&f.registryColorMask!=7&&f.registryColorMask!=15))return false;
        for(unsigned k=0;k<4;++k)if(bool(f.registryColorWords[k])!=bool(f.registryColorMask&(1u<<k)))return false;
        if((f.spotlightConfigEntered&&f.registryColorMask)||f.registrationEntered!=bool(f.registryIndex)||f.registrationEntered!=bool(f.registryManager)||
           (f.registryGrowthEntered&&!f.registrationEntered)||(f.registryGrowthReturned&&!f.registryGrowthEntered))return false;
        if((f.registryColorMask&&!f.registrationEntered)||(f.registryA0Stored&&f.registryColorMask!=15)||
           (f.registrySlotPublished&&(!f.registryIndex||(f.descriptorType()==1&&!f.registryA0Stored)||f.registryOwner!=&e))||
           (f.registryCountStored&&!f.registrySlotPublished)||(f.registryWithdrawn&&!f.registryCountStored)||
           (f.registrationReturned&&!f.registryCountStored)||(f.flareFlagRead&&!f.registrationReturned)||
           (f.flareSelected&&(!f.flareFlagRead||!(f.descriptorFlags()&16u))))return false;
        bool conversionGap=false;for(const auto& word:f.flareConversions){if(!word)conversionGap=true;
            else if(conversionGap||*word!=255||!f.flareSelected)return false;}
        if(f.flarePackedStack6C&&(!f.flareConversions[3]||*f.flarePackedStack6C!=0xffffffffu))return false;
        if((f.flareAllocationReached&&!f.flarePackedStack6C)||(f.flareAllocationReturned&&!f.flareAllocationReached)||
           (f.flareArgumentsBorrowed&&!f.flareAllocationReturned))return false;
        if(f.flareArgumentsBorrowed){if(f.flarePositionBorrow!=f.descriptor->position14.get()||
            f.flareTextureBorrow!=f.descriptor->flareTextureA4.data()||f.flareSizeArgument!=f.descriptor->flareSizeA0||
            f.flareTurningArgument!=static_cast<std::uint32_t>(f.descriptor->flareTurningE4))return false;}
        else if(f.flarePositionBorrow||f.flareTextureBorrow||(!f.flareNativeWithdrawn&&!f.descriptorDisposed&&(f.flareSizeArgument||f.flareTurningArgument)))return false;
        if((f.flareSetupEntered&&!f.flareMaterialAdmitted)||(f.flareGroupsPublished&&!f.flareGroupsAttempted)||
            (f.flareGroupWordStored&&!f.flareGroupsPublished)||(f.flarePrimitiveAttempted&&!f.flareGroupWordStored)||
            (f.flarePrimitivePublished&&!f.flarePrimitiveAttempted)||(f.flareMaterialEntered&&!f.flarePrimitivePublished)||
            (f.flareMaterialReturned&&!f.flareMaterialEntered)||(f.flareSetupReturned&&!f.flareMaterialReturned))return false;
        if(f.flareVectorCalls>8||(f.flareVectorCalls&&f.flareUVCalls!=4)||
           (f.flareReturned14AB30&&f.flareVectorCalls!=8))return false;
        for(unsigned i=0;i<8;++i){const auto mask=f.flareVectorStoreMask[i];
            if(mask!=0&&mask!=1&&mask!=3&&mask!=7)return false;
            if(i<f.flareVectorCalls&&mask!=7)return false;
            for(unsigned k=0;k<3;++k)if((mask&(1u<<k))&&!f.flareVectorReadWords[i][k])return false;
            for(const auto& receipt:f.flareVectorAssertions[i])if(!g::OriginalStripStreamHistory23CCA0::current(receipt))return false;}
        if(f.flareUVCalls>4||(f.flareStripAttachReturned&&(!f.flareStripAttachLock||!f.flareAttributesReturned)))return false;
        for(unsigned i=0;i<4;++i){if(f.flareUVStoreMask[i]!=0&&f.flareUVStoreMask[i]!=1&&f.flareUVStoreMask[i]!=3)return false;
            if(i<f.flareUVCalls&&f.flareUVStoreMask[i]!=3)return false;
            for(const auto& receipt:f.flareUVAssertions[i])if(!g::OriginalStripStreamHistory23CCA0::current(receipt))return false;}
        if(!g::OriginalStripStreamHistory23CCA0::current(f.flareBatchAssertion))return false;
        for(const auto& receipt:f.flareAliasAssertions)if(!g::OriginalStripStreamHistory23CCA0::current(receipt))return false;
        if((f.flareHeaderEntered&&!f.flareStreamReturned)||f.flareHeaderPaddingWords>3||
           (f.flareHeaderStoreMask!=0&&f.flareHeaderStoreMask!=1&&f.flareHeaderStoreMask!=3&&
            f.flareHeaderStoreMask!=7&&f.flareHeaderStoreMask!=15&&f.flareHeaderStoreMask!=31&&f.flareHeaderStoreMask!=63)||
           (f.flareHeaderReturned&&f.flareHeaderStoreMask!=63)||
           (f.flarePrimitiveStoreMask!=0&&f.flarePrimitiveStoreMask!=1&&f.flarePrimitiveStoreMask!=3&&
            f.flarePrimitiveStoreMask!=7&&f.flarePrimitiveStoreMask!=15)||
           (f.flarePrimitiveStoreMask&&!f.flareHeaderReturned)||
           (f.flareAttributesReturned&&f.flarePrimitiveStoreMask!=15))return false;
        if(f.flarePrimitiveParameters10){if(!(f.flarePrimitiveStoreMask&1)||f.flarePrimitiveParametersWithdrawn||
            f.flarePrimitiveParameters10!=f.flareParameters54.get()||!f.sourceFlare||!f.sourceFlare->strip90||
            !f.sourceFlare->strip90->primitive30_)return false;}
        else if((f.flarePrimitiveStoreMask&1)&&!f.flarePrimitiveParametersWithdrawn)return false;
        if((f.flarePayloadEntered&&(!f.flareStackTagKnown||!f.flareDescriptorParameterBegin))||
           (f.flarePayloadNativeBacking&&!f.flarePayloadEntered)||f.flarePayloadCount>128||f.flarePayloadFinished>15||
           (f.flareDescriptorParameterSpan&&(!f.flareDescriptorParameterEnd||!f.flareDescriptorParameterBegin||
             *f.flareDescriptorParameterSpan!=*f.flareDescriptorParameterEnd-*f.flareDescriptorParameterBegin))||
           (f.flareStreamReturned&&(!f.flareDescriptorParameterSpan||f.flarePayloadFinished!=15)))return false;
        if(f.flarePayloadEntered){for(unsigned stream=0;stream<4;++stream){
            if(f.flarePayloadCopies[stream]>(stream?4:1)||
               (f.flarePayloadOffsets[stream]&&*f.flarePayloadOffsets[stream]>f.flarePayloadCount)||
               (f.flarePayloadCopies[stream]&&!f.flarePayloadOffsets[stream]))return false;}}
        if((f.flareStreamEntered&&!f.flareParameterReturned134000)||
           (f.flareStreamArraysPublished&~f.flareStreamArraysAttempted)||
           (f.flareAliasPublished&&!f.flareAliasAllocationAttempted)||(f.flareBatchCountStored&&!f.flareAliasPublished))return false;
        if((f.flareAttributesEntered&&!f.flareSetupReturned)||(f.flareAttributeGroupPublished&&!f.flareAttributeGroupAttempted)||
           (f.flareCommonPublished&&!f.flareCommonAttempted)||(f.flareCommonZeroed&&!f.flareCommonPublished)||
           (f.flareAttributeCount5C&&(!f.flareCommonReturned||*f.flareAttributeCount5C!=1))||
           (f.flareAttributeCountsPublished&&!f.flareAttributeCountsAttempted)||
           (f.flareAttributeCountStored&&!f.flareAttributeCountsPublished)||(f.flareParameterPublished&&!f.flareParameterAttempted))return false;
        if(f.flareParameters54){const auto& p=*f.flareParameters54;
            if(!f.flareParameterAttempted||!p.sourceMode||p.sourceKnownMask!=15||
               p.nativePayload04!=((f.flareHeaderStoreMask&8)?p.buffer20:nullptr)||p.packetComplete!=bool(f.flareHeaderStoreMask&32)||
               p.packetKind!=NativeSunFlareParameters::PacketKind::none)return false;
            if(p.sourcePacketAttempted){if(p.sourcePacketHistorySerial!=f.flareParameterHistorySerial||p.field2C!=1||
                p.sourceHeaderKnownMask!=((f.flareHeaderStoreMask&8)?253u:255u)||p.count28!=f.flarePayloadCount||p.count24!=128||!p.sourceBufferPublished||!p.sourceBuffer20||
                *p.sourceBuffer20!=p.buffer20||p.buffer20!=(p.packet?p.packet->words.data():nullptr))return false;
                const std::uint32_t header0=((f.flareHeaderStoreMask&1)?((p.count28+f.flareHeaderPaddingWords)/4u)&0xffffu:0u)|
                    ((f.flareHeaderStoreMask&4)?0x30000000u:0u);
                if(p.unknown00To1C[0]!=header0)return false;
                if(f.flareHeaderStoreMask&8){if(p.unknown00To1C[1])return false;}
                else if(p.unknown00To1C[1]!=0)return false;
                for(unsigned i=2;i<8;++i)if(p.unknown00To1C[i]!=0)return false;
                for(unsigned i=0;i<128;++i){const auto& word=f.flarePayloadWords[i];
                    if(p.sourcePacketWordKnown[i]!=bool(word)||(word&&(!p.packet||i>=p.count28+f.flareHeaderPaddingWords||p.packet->words[i]!=*word)))return false;}
                if(p.count28>128)return false;
            }else if(p.field2C!=-1||p.count24||p.count28||p.buffer20||p.packet)return false;
        }else if(f.flareParameterReturned134000&&!f.flareAttributesWithdrawn)return false;
        if(f.sourceFlare){if(f.flareNativeWithdrawn)return false;const auto& flare=*f.sourceFlare;const auto& fe=flare.entity;
            if(!f.flareAllocationReached||(!f.flareMaterialAdmitted&&flare.table)||
               flare.setupStarted||flare.setupComplete||flare.inputsWritten!=(f.flareInputMask==7))return false;
            if((f.flareCountStack7C&&!f.flareArgumentsBorrowed&&!f.flareNativeWithdrawn&&!f.descriptorDisposed)||
               (f.flare141C20Returned&&!f.flareCountStack7C)||
               (f.flareInputMask!=0&&f.flareInputMask!=1&&f.flareInputMask!=3&&f.flareInputMask!=7)||
               (f.flareInputMask&&!f.flare141C20Returned)||
               (f.flareStripAllocationReached&&f.flareInputMask!=7))return false;
            if(f.flareInputMask&1){if(!flare.size98)return false;std::uint32_t word=0;std::memcpy(&word,&*flare.size98,4);
                if(!f.flareSizeArgument||word!=*f.flareSizeArgument)return false;}
            if((f.flareInputMask&2)&&(!(f.descriptorDisposed?f.descriptorPosition():f.flarePositionBorrow)||flare.vectors40To80[0]!=*(f.descriptorDisposed?f.descriptorPosition():f.flarePositionBorrow)))return false;
            if((f.flareInputMask&4)&&(!f.flareTurningArgument||flare.turning9C!=*f.flareTurningArgument))return false;
            if(flare.strip90){const auto& strip=*flare.strip90;const auto& se=strip.entity_;
                if(!f.flareStripAllocationReached||f.flareStripNativeWithdrawn||
                    (!f.flareGroupsPublished&&strip.groups38_)||(!f.flarePrimitiveAttempted&&strip.primitive30_)||
                    bool(strip.field40_)!=bool(f.flareSetupMask&4)||bool(strip.color70_)!=bool(f.flareSetupMask&16)||
                    (bool(strip.attributes_.vertices_)!=(f.flarePayloadNativeBacking&&!f.flareAttributesWithdrawn))||(!f.flareCommonPublished&&strip.attributes_.common_)||
                    (!f.flareAttributeGroupPublished&&strip.attributes_.group_)||
                    (!f.flareAttributeCountsPublished&&strip.attributes_.groupCounts_))return false;
                const auto& attrs=strip.attributes_;
                if(attrs.vertices_){if(!attrs.group_)return false;const auto& group=*attrs.group_;
                    if(f.flarePayloadOffsets[1]&&(!group.positions||group.positions[0].values!=attrs.vertices_->positions.data()||group.positions[0].count!=4))return false;
                    if(f.flarePayloadOffsets[2]&&(!group.triples||group.triples[0].values!=attrs.vertices_->triples.data()||group.triples[0].count!=4))return false;
                    if(f.flarePayloadOffsets[3]&&(!group.texcoords||group.texcoords[0].values!=attrs.vertices_->texcoords.data()||group.texcoords[0].count!=4))return false;
                    for(unsigned i=0;i<4;++i){const auto& v=attrs.vertices_->positions[i];
                        if(v.stripControl!=(i<f.flarePayloadCopies[1]?(i<2?-14:14):0))return false;
                        for(unsigned kind=0;kind<2;++kind){const unsigned call=2*i+kind;
                            for(unsigned k=0;k<3;++k){const auto& lane=kind?v.xyz[k]:attrs.vertices_->triples[i].components[k];
                                if(f.flareVectorStoreMask[call]&(1u<<k)){if(!lane.value||lane.unknownScratchLane!=0xff)return false;
                                    std::uint32_t bits=0;std::memcpy(&bits,&*lane.value,4);if(f.flareVectorReadWords[call][k]!=bits)return false;}
                                else if(lane.value)return false;}}
                        for(unsigned k=0;k<2;++k){const auto& lane=attrs.vertices_->texcoords[i].uv[k];
                            if(f.flareUVStoreMask[i]&(1u<<k)){const float expected=(k==0?i>=2:(i&1))?1.f:0.f;
                                if(lane.value!=expected||lane.unknownScratchLane!=0xff)return false;}
                            else if(lane.value)return false;}}}

                if(attrs.group_){const auto& group=*attrs.group_;if(f.flareAttributeDescriptorMask!=8191||
                    !f.flareAttributeDescriptor10||*f.flareAttributeDescriptor10!=((f.flareStreamDescriptorMask&4)?1:(f.flareStreamDescriptorMask&2)?0:-1)||
                    group.vertexCount!=((f.flareStreamDescriptorMask&1)?4u:0u)||group.batchCount!=((f.flareStreamDescriptorMask&4)?1u:0u)||
                    (!(f.flareStreamArraysPublished&1)&&group.counts)||(!(f.flareStreamArraysPublished&2)&&group.batches)||
                    (!(f.flareStreamArraysPublished&4)&&group.positions)||(!(f.flareStreamArraysPublished&8)&&group.triples)||
                    (!(f.flareStreamArraysPublished&16)&&group.extra)||(!(f.flareStreamArraysPublished&32)&&group.colors)||
                    (!(f.flareStreamArraysPublished&64)&&group.texcoords)||(!f.flareAliasPublished&&group.aliases))return false;
                    if(f.flareBatchCountStored&&(!group.counts||group.counts[0]!=4))return false;
                    if(group.aliases){if(f.flareAliasConstructed!=4)return false;
                        for(unsigned i=0;i<4;++i){const auto& alias=group.aliases[i];const auto mask=f.flareAliasStoreMask[i];
                            if(mask!=0&&mask!=1&&mask!=3&&mask!=7)return false;
                            if(alias.count!=((mask&4)?1u:0u)||((mask&1)&&alias.aliases[0].batch!=0)||
                               ((mask&2)&&alias.aliases[0].ordinal!=i)||alias.aliases[1].batch||alias.aliases[1].ordinal)return false;}}
                }
                if(attrs.common_&&f.flareCommonZeroed){const auto& c=*attrs.common_;if(c.layout)return false;
                    for(unsigned i=0;i<16;++i){const auto row=i/4,col=i%4;const float diagonal=row==col?1.f:0.f;
                        if(c.matrix40[row][col]!=((f.flareCommon40Mask&(1u<<i))?diagonal:0.f)||
                           c.matrix90[row][col]!=0.f||c.matrixE0[row][col]!=((f.flareCommonE0Mask&(1u<<i))?diagonal:0.f))return false;}
                    for(unsigned i=0;i<4;++i)if(c.vector130[i]!=((f.flareCommon130Mask&(1u<<i))?1.f:0.f))return false;}
                if(f.flareCommonReturned&&(f.flareCommon40Mask!=65535||f.flareCommon90Mask!=15||
                    f.flareCommonE0Mask!=65535||f.flareCommon130Mask!=15))return false;
                if(f.flareAttributeCountStored&&!f.flareAttributesWithdrawn&&(!attrs.groupCounts_||!f.flareCountStack7C||
                    attrs.groupCounts_[0]!=*f.flareCountStack7C))return false;
                if(f.flareStripCounted){if(se.reference00!=1||se.kind!=g::OriginalWreckComponentKind50LA::strip23BCA0||
                    se.objectKey!=key(&se)||se.sessionGeneration!=worldEntity_.sessionGeneration||se.parent1C!=(flare.stripAttached?&fe:nullptr)||se.firstChild18||
                    se.previous10||se.next14||se.mask20!=(flare.stripAttached?8u:0u)||se.descendants24||se.category28!=255||(se.flags29&15)!=3)return false;}
                else if(se.reference00)return false;
                if(f.flareStripEnrolled){if(!f.flareStripCounted||!authority.canRetire(se))return false;}
                else for(std::size_t n=0;n<authority.count_;++n)if(authority.entries_[n].node==&se)return false;
                if(f.flareStripCtorReturned&&(!f.flareStripVtable08||*f.flareStripVtable08!=0x3d1ea0||
                    f.flareStripCtorMask!=31||strip.count34_!=((f.flareSetupMask&2)?1u:0u)||
                    strip.format3C_!=((f.flareSetupMask&1)?7u:0u)||strip.color44_!=((f.flareSetupMask&8)?*f.flarePackedStack6C:0x007f7f7fu)||strip.flags48_!=1))return false;
                if(strip.field40_&&*strip.field40_!=1)return false;
                if(strip.color70_&&*strip.color70_!=std::array<float,4>{{127.f,127.f,127.f,64.f}})return false;
                if(f.flareGroupWordStored&&!f.flareResourcesWithdrawn&&(!strip.groups38_||!f.flareCountStack7C||strip.groups38_[0]!=*f.flareCountStack7C))return false;
                if(strip.primitive30_){const auto& p=*strip.primitive30_;
                    if(!f.flarePrimitiveCtorReturned||!f.flarePrimitiveVtable08||*f.flarePrimitiveVtable08!=0x3d20c0||
                       p.byte0C!=(f.flareMaterialReturned?7u:0u)||p.byte0D||p.byte0E!=255||p.flags0F||p.attributes10||
                       p.scale18!=1.f||p.scale1C!=1.f||p.vertices20!=((f.flarePrimitiveStoreMask&8)?4u:0u)||
                       p.groups22!=((f.flarePrimitiveStoreMask&4)?1u:0u)||!p.reference.canRelease299AC0())return false;
                    if(f.flareMaterialReturned&&!p.reference.retained())return false;
                }else if(!f.flareResourcesWithdrawn&&(f.flareMaterialEntered||f.flareMaterialReturned))return false;
            }else if(f.flareStripCounted||f.flareStripEnrolled||(!f.flareStripNativeWithdrawn&&f.flareStripCtorReturned))return false;
            if(flare.stripAttached!=bool(f.flareStripAttachReturned&&f.flareStripAttachLock&&!*f.flareStripAttachLock&&!f.flareStripAttachmentWithdrawn))return false;
            if(flare.stripAttached&&!flare.strip90)return false;
            if(flare.lightAttached!=bool(f.flareLightAttachReturned&&f.flareLightAttachLock&&!*f.flareLightAttachLock&&!f.flareLightAttachmentWithdrawn)||
               flare.lightPublished!=(f.flareC4Published&&!f.flareC4Withdrawn))return false;
            if(f.flareC4Published&&(!f.flare146AE0Entered||!f.flareObserverCallReturned))return false;
            if(f.flareLightAttachReturned&&(!f.flareC4Published||!f.flareLightAttachLock))return false;
            if(f.flare146AE0Returned&&(!f.flare146AE0Entered||!f.flareLightAttachReturned))return false;
            if(f.flareCounted){if(fe.reference00!=1||fe.kind!=g::OriginalWreckComponentKind50LA::lightFlare14AA10||
                fe.objectKey!=key(&fe)||fe.sessionGeneration!=worldEntity_.sessionGeneration||fe.parent1C!=(flare.lightAttached?&l.entity:nullptr)||
                fe.firstChild18!=(flare.stripAttached&&flare.strip90?&flare.strip90->entity_:nullptr)||
                fe.previous10||fe.next14||fe.mask20||fe.descendants24!=(flare.stripAttached?8u:0u)||fe.category28!=255||(fe.flags29&15)!=3)return false;}
            else if(fe.reference00)return false;
            if(f.flareEnrolled){if(!f.flareCounted||!authority.canRetire(fe))return false;}
            else for(std::size_t n=0;n<authority.count_;++n)if(authority.entries_[n].node==&fe)return false;
            if(f.flareBaseReturned&&(!f.flareEnrolled||!flare.typed||flare.query32==255))return false;
            if(f.flareVtable08&&(!f.flareBaseReturned||*f.flareVtable08!=0x3cd680))return false;
            if(f.flareVectorMask!=0&&f.flareVectorMask!=31)return false;
            if(f.flareVectorMask){for(unsigned k=0;k<5;++k){if(k==0&&(f.flareInputMask&2))continue;
                if(flare.vectors40To80[k]!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}})return false;}}
            if(f.flareCtorReturned&&(!f.flareVtable08||f.flareVectorMask!=31||!flare.constructed||flare.flags33!=3||
                flare.knownFlags33!=7||flare.query30||(!(f.flareInputMask&4)&&flare.turning9C!=1)))return false;
        }else if(f.flareCounted||f.flareEnrolled||(!f.flareNativeWithdrawn&&(f.flareBaseReturned||f.flareCtorReturned||f.flareVtable08||f.flareVectorMask)))return false;
        if(f.observerEntered&&!f.observerWithdrawn){
            if(!f.observerManager||!f.observerManager->preflightSourceTypedObserverNative(*this,f.ordinal))return false;
        }else if(l.observerD0||l.fieldD4||l.observerOwner||f.observerManager||f.primaryNode||f.primaryList||f.observerLease||f.primarySelectionPending)return false;
        if(l.typed!=f.returned||l.constructed!=f.returned||(!l.typed&&l.query32!=0xff)||(l.typed&&l.query32==0xff)||
           (f.published&&(!f.returned||f.wrapper->live00!=&e))||l.positionWritten!=bool(f.positionWords)||
           (l.colorsWritten&&(!f.published||(!f.argumentsBorrowed&&!f.descriptorDisposed)))||(l.revisionWritten&&!l.colorsWritten)||
           (f.luminance&&!l.revisionWritten)||(f.positionCompared&&!f.luminance)||
           (l.positionWritten&&!f.positionCompared)||(l.positionRevision&&!l.positionWritten)||
           (f.lookCompared&&(!f.positionCompared||(l.positionWritten&&!l.positionRevision)))||
           (f.attenuationWords&&!f.lookCompared)||(f.attenuationRange&&!f.attenuationWords)||
           (l.rangeWritten&&!f.attenuationRange)||(l.rangeRevision&&!l.rangeWritten)||l.configured!=f.configured||
           (f.configured&&(!f.laterAmbientConfigEntered&&!f.attenuationRange||(l.rangeWritten&&!l.rangeRevision))))return false;
    }
    return true;
}
bool Owner::sourceTypedLuminance146E60(const SourceTypedLightFrame& f,std::uint32_t& out)const noexcept {
    if(!preflightSourceTypedLightsNative()||!f.light||!f.light->colorsWritten||!f.light->revisionWritten||f.luminance)return false;
    if(f.laterAmbientConfigEntered)return g::SourceUiLightMath50KG::luminance146E60(0,0,0,out);
    if(f.spotlightConfigEntered){const auto& c=f.light->diffuse3C;
        return g::SourceUiLightMath50KG::luminanceRaRemaining146E60(
            g::directional_native_proposal::detail::word(c[0]),g::directional_native_proposal::detail::word(c[1]),
            g::directional_native_proposal::detail::word(c[2]),out);}
    return g::SourceUiLightMath50KG::luminance146E60(0x42ff0000u,0x42ff0000u,0x42ff0000u,out);
}
bool Owner::sourceTypedAttenuation147268(const SourceTypedLightFrame& f,std::uint32_t& out)const noexcept {
    if(!preflightSourceTypedLightsNative()||!f.light||!f.attenuationWords||f.attenuationRange)return false;
    return g::directional_native_proposal::range147268(*f.attenuationWords,g::SourceUiLightMath50KG::range146898(),out)==
        g::directional_native_proposal::Status::complete;
}
bool Owner::abortSourceTypedLightsNative()noexcept {
    if(!preflightSourceTypedLightsNative())return false;
    for(auto i=sourceTypedLights_.rbegin();i!=sourceTypedLights_.rend();++i){auto& f=**i;if(f.nativeClosed)continue;
        if(f.light&&f.light->attached){const auto detached=worldApplication_->entityAuthority_.detach(f.light->entity,worldApplication_->attachmentLock423F58());
            if(detached.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
            f.light->attached=false;f.nativeDetached=true;}
        if(!withdrawSourceFlareObserverNative(f))return false;
        if(f.sourceFlare&&f.sourceFlare->lightAttached){
            //Native withdrawal of the exact flare child, before its resources/owner die.
            if(worldApplication_->attachmentLock423F58()||!f.light)return false;
            auto& flare=*f.sourceFlare;auto& light=*f.light;
            if(light.entity.firstChild18!=&flare.entity||flare.entity.parent1C!=&light.entity||flare.entity.previous10||flare.entity.next14)return false;
            light.entity.firstChild18=nullptr;light.entity.descendants24=0;flare.entity.parent1C=nullptr;
            flare.lightAttached=false;f.flareLightAttachmentWithdrawn=true;
        }
        if(f.flareC4Published&&!f.flareC4Withdrawn){
            if(!f.light||!f.sourceFlare||f.light->fieldC4!=&f.sourceFlare->entity)return false;
            f.light->fieldC4=nullptr;f.sourceFlare->lightPublished=false;f.flareC4Withdrawn=true;
        } //Native borrow withdrawal, not a fabricated source146980 store.
        if(f.sourceFlare&&f.sourceFlare->stripAttached){
            //Native abort of this exact isolated relationship, not source141A70 replay.
            if(worldApplication_->attachmentLock423F58()||!f.sourceFlare->strip90)return false;
            auto& flare=*f.sourceFlare;auto& child=flare.strip90->entity_;
            flare.entity.firstChild18=nullptr;flare.entity.descendants24=0;
            child.parent1C=nullptr;child.mask20=0;flare.stripAttached=false;f.flareStripAttachmentWithdrawn=true;
        }
        if(f.flareParameterHistorySerial&&!f.flareParameterHistoryWithdrawn){
            if(!g::OriginalParameterPacketHistory50LA::withdrawNative(f.flareParameterHistorySerial))return false;
            f.flareParameterHistoryWithdrawn=true;}
        //Withdraw actual source Primitive+10 borrower before destroying its parameter owner.
        //Legacy Primitive.attributes10 was never used as this parameter pointer.
        if(f.flarePrimitiveParameters10){f.flarePrimitiveParameters10=nullptr;f.flarePrimitiveParametersWithdrawn=true;}
        f.flareParameters54.reset();
        if(f.sourceFlare&&f.sourceFlare->strip90){auto& attrs=f.sourceFlare->strip90->attributes_;
            attrs.group_.reset();attrs.vertices_.reset();attrs.common_.reset();attrs.groupCounts_.reset();f.flareAttributesWithdrawn=true;}
        //Unattached flare has no List/registry borrowers yet; retire before its source light.
        if(f.sourceFlare){
            if(f.sourceFlare->strip90){auto& strip=*f.sourceFlare->strip90;
                if(strip.primitive30_){if(!strip.primitive30_->reference.canRelease299AC0())return false;
                    strip.primitive30_->reference.release299AC0();strip.primitive30_.reset();}
                strip.groups38_.reset();f.flareResourcesWithdrawn=true;auto& se=strip.entity_;
                if(f.flareStripEnrolled&&!worldApplication_->entityAuthority_.retire(se))return false;f.flareStripEnrolled=false;
                if(f.flareStripCounted){--se.reference00;--worldApplication_->objectCount423F60();f.flareStripCounted=false;}
                f.sourceFlare->strip90.reset();f.flareStripNativeWithdrawn=true;}
            auto& e=f.sourceFlare->entity;
            if(f.flareEnrolled&&!worldApplication_->entityAuthority_.retire(e))return false;f.flareEnrolled=false;
            if(f.flareCounted){--e.reference00;--worldApplication_->objectCount423F60();f.flareCounted=false;}
            f.sourceFlare.reset();f.flareNativeWithdrawn=true;}
        f.flarePositionBorrow=nullptr;f.flareTextureBorrow=nullptr;f.flareArgumentsBorrowed=false;f.flareNativeWithdrawn=true;
        if(f.light){rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic md;
            if(f.registryCountStored&&!f.registryWithdrawn&&
               (!f.registryManager||!f.registryManager->withdrawSourceTypedRegistryNative(*this,f.ordinal,md)))return false;
            if(f.observerEntered&&!f.observerWithdrawn&&
               (!f.observerManager||!f.observerManager->withdrawSourceTypedObserverNative(*this,f.ordinal,md)))return false;
            auto& e=f.light->entity;
            if(f.enrolled&&!worldApplication_->entityAuthority_.retire(e))return false;f.enrolled=false;
            if(f.counted){--e.reference00;--worldApplication_->objectCount423F60();f.counted=false;}
        }
        if(f.published)f.wrapper->live00=nullptr;
        f.registryManager=nullptr;f.light.reset();f.wrapper=nullptr;f.descriptor=nullptr;f.lensStack00=nullptr;f.argumentsBorrowed=false;f.nativeClosed=true;
    }
    return true; //No source destructor, descriptor disposal or revision rollback claim.
}
}
namespace ra_cold_aggregate50LA {
bool Owner::continueSourceTypedLightIteration273BA8(std::size_t ordinal,std::uint32_t& pc)noexcept {
    //One actual source wrapper, indexed by the original loop counter.
    if(ordinal<2||ordinal>=185||sourceTypedLights_.size()!=ordinal-2||nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       !sourceLightPopulationS0_||*sourceLightPopulationS0_!=static_cast<std::int32_t>(ordinal)||
       !sourceSecondWrapperReturned_||!sourceSecondDescriptorDisposed_||!sourceSecondLight_||!sourceSecondLight_->attached||
       !sourceSecondLight_->registered||!sourceDefaultObserverLease_||!preflightSourceWorldTopologyCountNative()||
       !preflightSourceLightPopulationNative()||!sourceTopologyLightManager_)return false;
    auto* list=*sourceLight3A8_;if(!list||list!=nativeLightList3A8_.get()||!list->wrappers38||
       list->count30!=185||list->loaded!=185||list->sourceWrappersConstructed!=185||list->sourceWrappersConstructed<=ordinal)return false;
    auto& w=list->wrappers38[ordinal];if(!w.sourceConstructed||!w.sourceDescriptorPublished||!w.sourceDescriptor04||w.live00)return false;
    if(ordinal>2){const auto& prior=sourceTypedLights_.back();
        if(!prior||prior->ordinal+1!=ordinal||!prior->populationIncremented||!prior->wrapperReturned||
           !prior->descriptorDisposed||!prior->attachCallReturned)return false;}
    try{sourceTypedLights_.reserve(sourceTypedLights_.size()+1);auto f=std::make_unique<SourceTypedLightFrame>();
        f->ordinal=ordinal;f->wrapper=&w;f->descriptor=w.sourceDescriptor04.get();sourceTypedLights_.push_back(std::move(f));
    }catch(...){return false;} //Native frame admission only; source loop not advanced.
    auto& f=*sourceTypedLights_.back();auto& d=*f.descriptor;
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};
    const auto stop=[&](std::uint32_t p){at(p);return false;};
    at(0x273ba8);at(0x273bac);const auto count=static_cast<std::int32_t>(list->count30);
    at(0x273bb0);if(*sourceLightPopulationS0_>=count)return stop(0x273bbc);
    using namespace g::world_light_assertion273ABC;
    at(0x273abc);if(!read(1,f.guard))return stop(pc);f.guardMask|=1;
    if(f.guard!=1)return stop(pc); //Same process first-row initialization, never reset.
    at(0x273adc);if(!read(0,f.enabled))return stop(pc);f.guardMask|=2;if(f.enabled!=1)return stop(pc);
    at(0x273b14);f.zone=d.zone20;
    at(0x273b1c);if(!worldZones34C_||static_cast<std::int32_t>(*worldZones34C_)<*f.zone)return stop(pc);f.assertionPassed=true;
    at(0x273b74);at(0x277da0);
    if(!d.loaded||d.knownFlags28!=0x7f||!(d.flags28&4u)||(d.type00!=1&&d.type00!=3&&d.type00!=4))
        return stop(pc); //Unsupported source type/disabled branch is not manufactured.
    if(d.type00==1){
        if((d.zone20!=1&&d.zone20!=7)||(d.flags28&0x7f)!=0x17||!d.position14||!d.look18||!d.attenuation1C||d.range24||
           d.color04!=std::array<std::uint32_t,4>{{0x437f0000,0x437f0000,0x437f0000,0x437f0000}})return stop(pc);
        for(auto word:*d.position14)if(!g::directional_native_proposal::detail::normalOrZeroWord(word))return stop(pc);
        for(auto word:*d.look18)if(!g::directional_native_proposal::detail::normalOrZeroWord(word))return stop(pc);
    } //Spotlight/Ambient leaves validate their actual live descriptor after publication.
    if(!reserveSourceAmbientListNative())return stop(0); //Native Entity ledger reserve before guest allocation.
    auto& authority=worldApplication_->entityAuthority_;
    at(0x277e0c);f.outerReached=true;f.light.reset(new(std::nothrow) NativeRaLight146720);
    if(f.light){auto& l=*f.light;auto& e=l.entity;
        at(0x14672c);if(!authority.canEnroll(e,key(&e)))return stop(pc);
        const auto made=g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::light146720,
            worldEntity_.sessionGeneration,key(&e),true,worldApplication_->objectCount423F60());f.counted=e.reference00!=0;
        if(made.status!=g::OriginalWreckComponentStatus50LA::complete)return stop(made.sourcePC);
        if(!authority.enroll(e))return stop(pc);f.enrolled=true;
        at(0x1467a8);f.vtable08=0x3cd600;at(0x1468c0);l.range84=g::SourceUiLightMath50KG::range146898();
        at(0x146924);l.positionA0=worldApplication_->lightVector426F10_;at(0x146934);f.defaults=true;
        at(0x146938);const auto query=worldApplication_->types383408().lightEmitter146938();if(!query.okay())return stop(query.sourcePC);
        at(0x146964);l.query32=query.type;l.typed=true;at(0x146970);l.constructed=true;f.returned=true;
    }
    at(0x277e28);w.live00=f.light?&f.light->entity:nullptr;f.published=true;
    at(0x277e50);f.lensStack00=(d.flags28&8u)?d.lensTexture50.data():nullptr;
    at(0x277e54);f.ordinalStack08=*sourceLightPopulationS0_;at(0x277e84);f.argumentsBorrowed=true;
    if(!f.light)return stop(0x146fc0); //Original NULL receiver fault, no fake return.
    if(d.type00==3){
        if(!configureSourceSpotlight146F60(f,pc)||!registerSourceSpotlight148F30(f,pc))return stop(pc);
        return retireAndAttachSourceTypedWrapper2780AC(f,pc);
    }
    if(d.type00==4){
        if(!continueSourceLaterAmbient146F60(f,pc))return stop(pc);
        return retireAndAttachSourceTypedWrapper2780AC(f,pc);
    }
    auto& l=*f.light;
    at(0x146fc0);l.type34=1;at(0x146fd8);l.ordinalC8=*f.ordinalStack08;
    at(0x146d04);l.diffuse3C=l.specular4C={{127.5f,127.5f,127.5f,127.5f}};l.ambient5C={};l.colorsWritten=true;
    rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic md;
    at(0x146e30);if(!sourceTopologyLightManager_->incrementSourceTyped149300(*this,ordinal,0,md))return stop(md.sourcePC);l.revisionWritten=true;
    at(0x146e3c);std::uint32_t luminance=0;if(!sourceTypedLuminance146E60(f,luminance))return stop(0x146e60);
    at(0x146e44);f.luminance=luminance;l.luminanceCC=luminance;
    at(0x147028); //Zero source Range skips explicit range setter/revision.
    const auto sameXYZ=[](const std::array<std::uint32_t,4>& a,const std::array<std::uint32_t,4>& b){
        for(unsigned n=0;n<3;++n)if(g::directional_native_proposal::detail::fromWord(a[n])!=g::directional_native_proposal::detail::fromWord(b[n]))return false;
        return true;
    };
    at(0x1470bc);f.positionCompared=true;
    if(!sameXYZ(*d.position14,l.positionA0)){
        at(0x147130);l.positionA0=*d.position14;f.positionWords=*d.position14;
        at(0x147600); //Fresh observerD0NULL; no callback or membership access.
        for(unsigned n=0;n<3;++n)l.position6C[n]=g::directional_native_proposal::detail::fromWord(l.positionA0[n]);l.positionWritten=true;
        at(0x147184);if(!sourceTopologyLightManager_->incrementSourceTyped149300(*this,ordinal,1,md))return stop(md.sourcePC);l.positionRevision=true;
    }
    at(0x147194);f.lookCompared=true;
    //Actual absent look uses source origin; equality, not a type1 skip shortcut.
    //Any differing look remains unsupported here; never normalize a zero vector.
    if(!sameXYZ(*d.look18,l.lookB0))return stop(0x1471a8);
    if(*d.attenuation1C!=std::array<std::uint32_t,4>{{0x3cf5c28f,0x3b03126f,0x3a83126f,0x3f800000}})return stop(0x147238);
    at(0x147248);l.attenuation8C[0]=g::directional_native_proposal::detail::fromWord((*d.attenuation1C)[0]);
    at(0x147250);l.attenuation8C[1]=g::directional_native_proposal::detail::fromWord((*d.attenuation1C)[1]);
    at(0x147264);l.attenuation8C[2]=g::directional_native_proposal::detail::fromWord((*d.attenuation1C)[2]);f.attenuationWords=*d.attenuation1C;
    at(0x147268);std::uint32_t range=0;if(!sourceTypedAttenuation147268(f,range))return stop(pc);
    at(0x14732c);f.attenuationRange=range;
    if(g::directional_native_proposal::detail::fromWord(range)!=g::directional_native_proposal::detail::fromWord(l.range84)){
        at(0x147340);l.range84=range;l.rangeWritten=true;
        at(0x147354);if(!sourceTopologyLightManager_->incrementSourceTyped149300(*this,ordinal,2,md))return stop(md.sourcePC);l.rangeRevision=true;
    }
    at(0x147470);l.mode38=1; //147444 loads1; type1 branch147448 selects147470.
    at(0x147494);l.configured=true;f.configured=true;
    at(0x277fcc); //Same manager registration caller; no legacy registry adoption.
    //Keep both previous siblings/registry slots intact and descriptor live.
    //Join the genuine primary, secondary and postcondition returns in source order.
    const bool primary=sourceTopologyLightManager_->bindSourceTypedPrimary1474A0(*this,ordinal,md);
    if(!primary&&(md.sourcePC!=0x280bf0||f.primaryStoreMask!=31))return stop(md.sourcePC);
    if(!sourceTopologyLightManager_->continueSourceTypedSecondary280BF0(*this,ordinal,md))return stop(md.sourcePC);
    if(!sourceTopologyLightManager_->finishSourceTypedRegistration149088(*this,ordinal,md))return stop(md.sourcePC);
    at(0x277fd4);if(f.descriptor!=w.sourceDescriptor04.get())return stop(pc);
    at(0x277fd8);f.flareFlagRead=true;f.flareSelected=(f.descriptor->flags28&16u)!=0;
    if(!f.flareSelected)return stop(0x2780ac);
    //Actual CVT.W.S inputs are integral255. No rounding-mode or exceptional
    //conversion policy is invented; each refused lane keeps prior conversions.
    constexpr unsigned channels[]{3,0,1,2};
    constexpr std::uint32_t conversions[]{0x278000,0x278008,0x278014,0x278024};
    for(unsigned i=0;i<4;++i){at(conversions[i]);if(d.flareColor90[channels[i]]!=0x437f0000u)return stop(pc);
        f.flareConversions[i]=255;}
    at(0x278038);const auto packed=(*f.flareConversions[0]<<24)|(*f.flareConversions[1]<<16)|
        (*f.flareConversions[2]<<8)|*f.flareConversions[3];
    at(0x278040);f.flarePackedStack6C=packed; //JAL delay store, before allocator effects.
    if(!reserveSourceAmbientListNative())return stop(0x27803c); //Native ledger admission, no guest effect.
    at(0x27803c);f.flareAllocationReached=true;f.sourceFlare.reset(new(std::nothrow) NativeLocalFlare);
    if(f.sourceFlare){auto& flare=*f.sourceFlare;auto& e=flare.entity;
        at(0x27e13c);if(!authority.canEnroll(e,key(&e)))return stop(pc);
        const auto made=g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::lightFlare14AA10,
            worldEntity_.sessionGeneration,key(&e),true,worldApplication_->objectCount423F60());f.flareCounted=e.reference00!=0;
        if(made.status!=g::OriginalWreckComponentStatus50LA::complete)return stop(made.sourcePC);
        if(!authority.enroll(e))return stop(pc);f.flareEnrolled=true;flare.entityConstructed=true;
        at(0x27e154);flare.query30=0;at(0x27e168);flare.query32=255;flare.flags33=0;flare.knownFlags33=7;
        at(0x27e1bc);const auto query=worldApplication_->types383408().searchableGameObject27E1BC();
        if(!query.okay())return stop(query.sourcePC);
        at(0x27e1f8);flare.query32=query.type;flare.typed=true;at(0x27e218);flare.flags33=1;
        at(0x27e24c);f.flareBaseReturned=true;
        at(0x14aa2c);f.flareVtable08=0x3cd680;
        constexpr std::uint32_t vectorStores[]{0x14aa34,0x14aa3c,0x14aa44,0x14aa4c,0x14aa54};
        for(unsigned i=0;i<5;++i){at(vectorStores[i]);flare.vectors40To80[i]={{0,0,0,0x3f800000u}};f.flareVectorMask|=1u<<i;}
        at(0x14aa70);flare.flags33|=1;at(0x14aa8c);flare.flags33|=2;
        at(0x14aa90);flare.strip90.reset();at(0x14aa94);flare.observer94=nullptr;at(0x14aa98);flare.turning9C=1;
        at(0x14aaa4);flare.constructed=true;f.flareCtorReturned=true;
    }
    at(0x27805c);f.flareAllocationReturned=true; //Includes genuine NULL receiver branch.
    at(0x278068);f.flarePositionBorrow=d.position14.get();at(0x27806c);f.flareSizeArgument=d.flareSizeA0;
    at(0x278070);f.flareTurningArgument=static_cast<std::uint32_t>(d.flareTurningE4);
    at(0x278078);f.flareTextureBorrow=d.flareTextureA4.data();f.flareArgumentsBorrowed=true;
    //Continue through the same loader-backed process382B30 lane, retaining
    //actual source arguments and nullable allocation history.
    at(0x14ab5c);continueSourceTypedFlare14AB30(f,pc);
    if(pc!=0x14abd0||!f.flareStripCtorReturned||!f.flareStripPublished)return false;
    //Only the actual retained same-App source namespace and DefaultMT owner.
    const auto& domain=sourceLoadApplication_->standardMaterialDomainOwner_;
    if(!domain||domain!=sourceTrackMountDomain_||!domain->prepared()||!sourceLoadApplication_->materialApplication)return false;
    const auto table=domain->materials382B38();
    if(!table||!table->sourceNames17CAD4Current()||!table->screenApplicationMatches(*sourceLoadApplication_->materialApplication))return false;
    if(!continueSourceTypedFlare150450(f,table,sourceLoadApplication_->materialDirectory36B970(),pc))return false;
    return retireAndAttachSourceTypedWrapper2780AC(f,pc);
}
}

namespace ra_pod_scene50LA {
bool Owner::continueSourceTypedFlare14AB30(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(f.pc!=0x14ab5c||!preflightSourceTypedLightsNative()||!f.flareArgumentsBorrowed||f.flareCountStack7C||!worldApplication_)return false;
    std::uint32_t raw=0;at(0x14ab5c);if(!worldApplication_->readFlareCount382B30(raw))return stop(pc);
    at(0x14ab7c);f.flareCountStack7C=raw; //JAL141C20 delay slot, exact raw bits.
    at(0x141c20);f.flare141C20Returned=true; //Actual identity leaf141C20/24, including NULL.
    at(0x14ab80);if(!f.sourceFlare)return stop(pc); //First actual receiver dereference.
    auto& flare=*f.sourceFlare;
    if(!f.flareSizeArgument)return stop(pc);float size;std::memcpy(&size,&*f.flareSizeArgument,4);flare.size98=size;f.flareInputMask|=1;
    at(0x14ab84);if(!f.flarePositionBorrow)return stop(pc);
    at(0x14ab8c);flare.vectors40To80[0]=*f.flarePositionBorrow;f.flareInputMask|=2;
    at(0x14ab94);if(!f.flareTurningArgument)return stop(pc);flare.turning9C=*f.flareTurningArgument;f.flareInputMask|=4;flare.inputsWritten=true;
    if(!reserveSourceAmbientListNative())return stop(0x14ab90);
    at(0x14ab90);f.flareStripAllocationReached=true;flare.strip90.reset(new(std::nothrow) g::OriginalPodCableStripCold50LA);
    if(flare.strip90){auto& strip=*flare.strip90;auto& e=strip.entity_;auto& authority=worldApplication_->entityAuthority_;
        at(0x15032c);if(!authority.canEnroll(e,key(&e)))return stop(pc);
        const auto made=g::initializeOriginalWreckComponent50LA(e,g::OriginalWreckComponentKind50LA::strip23BCA0,
            worldEntity_.sessionGeneration,key(&e),true,worldApplication_->objectCount423F60());f.flareStripCounted=e.reference00!=0;
        if(made.status!=g::OriginalWreckComponentStatus50LA::complete)return stop(made.sourcePC);
        if(!authority.enroll(e))return stop(pc);f.flareStripEnrolled=true;
        at(0x15033c);f.flareStripVtable08=0x3cd7d0;at(0x150340);strip.count34_=0;at(0x150344);strip.groups38_.reset();
        at(0x15034c);strip.format3C_=0;at(0x150354);strip.color44_=0x007f7f7f;at(0x150358);strip.primitive30_.reset();
        at(0x150378);strip.flags48_=1;at(0x150390);strip.flags48_&=static_cast<std::uint8_t>(~2u);
        at(0x23bcbc);f.flareStripVtable08=0x3d1ea0;
        //23BCC0..D0 initialize source attribute pointers50..60, no allocation.
        constexpr std::uint32_t stores[]{0x23bcc0,0x23bcc4,0x23bcc8,0x23bccc,0x23bcd0};
        for(unsigned k=0;k<5;++k){at(stores[k]);f.flareStripCtorMask|=1u<<k;}
        at(0x23bce0);f.flareStripCtorReturned=true;flare.stripConstructed=true;
    }
    at(0x14abac);f.flareStripPublished=true;flare.stripPublished=true; //Includes sourceNULL publication.
    at(0x14abc4);if(!flare.strip90)return stop(pc); //Actual strip vtable load, before virtual call.
    return stop(0x14abd0); //23BEB0 setup/material resources and later attachments remain unjoined.
}
}

namespace ra_pod_scene50LA {
bool Owner::continueSourceTypedFlare150450(SourceTypedLightFrame& f,
    const std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA>& table,const std::string& directory,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(f.pc!=0x14abd0||!preflightSourceTypedLightsNative()||!f.sourceFlare||!f.sourceFlare->strip90||
       f.flareSetupEntered||!f.flarePackedStack6C||!f.flareCountStack7C||*f.flareCountStack7C!=4||!f.descriptor||!table)return false;
    const auto& name=f.descriptor->flareTextureA4;std::size_t length=0;
    while(length<f.descriptor->flareTextureKnown&&length<name.size()&&name[length])++length;
    if(length>=f.descriptor->flareTextureKnown||length==name.size()||directory.find('\0')!=std::string::npos||
       directory.size()+length+(directory.empty()||directory.back()=='\\'?0u:1u)>255u)return false;
    auto& flare=*f.sourceFlare;auto& strip=*flare.strip90;
    try{f.flareMaterialName.assign(name.data(),length);flare.directory=directory;}catch(...){return false;}
    flare.table=table;f.flareMaterialAdmitted=true;
    at(0x150474);f.flareSetupEntered=true;strip.format3C_=7;f.flareSetupMask|=1;
    at(0x150480);strip.count34_=1;f.flareSetupMask|=2;
    at(0x150498);f.flareGroupsAttempted=true;strip.groups38_.reset(new(std::nothrow) std::uint32_t[1]);
    at(0x1504a0);f.flareGroupsPublished=true;
    at(0x1504a4);strip.field40_=1;f.flareSetupMask|=4;
    //Actual3D1EA0 slot9C=23D750->150560 then each byte>>1/CVT.S.W.
    at(0x150560);strip.color44_=*f.flarePackedStack6C;f.flareSetupMask|=8;
    at(0x15057c);strip.flags48_|=1;
    if(strip.color44_!=0xffffffffu)return stop(0x23d76c); //Exact admitted integral conversion domain.
    at(0x23d798);strip.color70_=std::array<float,4>{{127.f,127.f,127.f,64.f}};f.flareSetupMask|=16;
    //No fallible native operation between these exact source color stores.
    at(0x23d7b4);at(0x23d7b8);at(0x23d7bc);
    at(0x1504ec);if(!strip.groups38_)return stop(pc);strip.groups38_[0]=*f.flareCountStack7C;f.flareGroupWordStored=true;
    at(0x1504f0);f.flarePrimitiveAttempted=true;strip.primitive30_.reset(new(std::nothrow) g::OriginalPodCableStripCold50LA::Primitive);
    if(strip.primitive30_){auto& p=*strip.primitive30_;
        //Existing concrete reference owns299AA0 material00=-1/optional04NULL.
        at(0x299aa8);f.flarePrimitiveVtable08=0x3d36b0;
        at(0x2461fc);f.flarePrimitiveVtable08=0x3d20c0;at(0x246200);p.byte0C=0;at(0x246208);p.byte0D=0;
        at(0x24620c);p.byte0E=255;at(0x246210);p.flags0F=0;at(0x246218);p.scale18=1.f;at(0x24621c);p.scale1C=1.f;
        at(0x246220);p.attributes10=nullptr;at(0x246244);p.vertices20=0;at(0x246248);p.groups22=0;f.flarePrimitiveCtorReturned=true;
    }
    at(0x150508);f.flarePrimitivePublished=true;
    at(0x15051c);if(!strip.primitive30_)return stop(pc);
    auto& p=*strip.primitive30_;rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
    auto beforeRetain=[&]()noexcept {using Shared=rr::platform::OriginalUiGarageScreenText50KG;
        at(0x299cb8);if(!Shared::guard8CD8().load(std::memory_order_relaxed)){
            at(0x299ccc);Shared::assertion443640().store(1,std::memory_order_relaxed);
            at(0x299cd0);Shared::guard8CD8().store(1,std::memory_order_relaxed);}
        at(0x299d38); //Existing nonnegative row: source assertion returns regardless mode.
    };
    at(0x299be0);f.flareMaterialEntered=true;
    if(!p.reference.acquireNamed299BE0Impl(table,flare.directory,f.flareMaterialName,diagnostic,beforeRetain,true))
        return stop(diagnostic.sourcePC?diagnostic.sourcePC:pc);
    at(0x2465cc);p.byte0C=7;f.flareMaterialReturned=true;
    at(0x150550);f.flareSetupReturned=true;
    at(0x23bed8);return continueSourceTypedFlareAttributes23BED8(f,pc);
}
}

namespace ra_pod_scene50LA {
bool Owner::continueSourceTypedFlareAttributes23BED8(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(f.pc!=0x23bed8||!preflightSourceTypedLightsNative()||!f.sourceFlare||!f.sourceFlare->strip90||
       !f.flareSetupReturned||f.flareAttributesEntered||!f.flareCountStack7C||*f.flareCountStack7C!=4)return false;
    auto& strip=*f.sourceFlare->strip90;auto& attrs=strip.attributes_;
    if(strip.count34_!=1||!strip.groups38_||strip.groups38_[0]!=4)return false;
    using Attributes=g::OriginalCableStripAttributes50LA;
    at(0x23bef0);f.flareAttributesEntered=true;f.flareAttributeGroupAttempted=true;
    attrs.group_.reset(new(std::nothrow) Attributes::Group);
    if(attrs.group_){ //2E8990 header/allocation adapter; one real23C050 descriptor.
        constexpr std::uint32_t stores[]{0x23c050,0x23c054,0x23c058,0x23c060,0x23c064,0x23c068,0x23c06c,
            0x23c070,0x23c074,0x23c078,0x23c07c,0x23c080,0x23c084};
        for(unsigned i=0;i<13;++i){at(stores[i]);if(i==4)f.flareAttributeDescriptor10=-1;f.flareAttributeDescriptorMask|=1u<<i;}
    }
    at(0x23bf18);f.flareAttributeGroupPublished=true; //NULL58 does not stop23CB20.
    at(0x23cb34);f.flareCommonAttempted=true;attrs.common_.reset(new(std::nothrow) Attributes::CommonData);
    at(0x23cb3c);f.flareCommonPublished=true;
    at(0x306478);if(!attrs.common_)return stop(pc); //NULL fails in actual memset, not publication.
    auto& common=*attrs.common_;common.layout.reset();common.matrix40={};common.matrix90={};common.matrixE0={};common.vector130={};
    f.flareCommonZeroed=true;
    //Only native numeric semantic fields are represented. Source command headers,
    //self-address and DMA/VIF words have no native consumer or readiness authority.
    for(unsigned i=0;i<16;++i){at(0x23cbc0+4*i);common.matrix40[i/4][i%4]=i/4==i%4?1.f:0.f;f.flareCommon40Mask|=1u<<i;}
    for(unsigned i=0;i<4;++i){at(0x23cc08+4*i);common.matrix90[i]={};f.flareCommon90Mask|=1u<<i;}
    for(unsigned i=0;i<16;++i){at(0x23cc20+4*i);common.matrixE0[i/4][i%4]=i/4==i%4?1.f:0.f;f.flareCommonE0Mask|=1u<<i;}
    constexpr std::uint32_t vectorPC[]{0x23cc64,0x23cc68,0x23cc6c,0x23cc74};
    for(unsigned i=0;i<4;++i){at(vectorPC[i]);common.vector130[i]=1.f;f.flareCommon130Mask|=1u<<i;}
    at(0x23cc98);f.flareCommonReturned=true;
    at(0x23bf28);f.flareAttributeCount5C=strip.count34_;
    at(0x23bf30);f.flareAttributeCountsAttempted=true;attrs.groupCounts_.reset(new(std::nothrow) std::uint32_t[1]);
    at(0x23bf38);f.flareAttributeCountsPublished=true;
    at(0x23bf64);if(!attrs.groupCounts_)return stop(pc);attrs.groupCounts_[0]=strip.groups38_[0];f.flareAttributeCountStored=true;
    at(0x23bf78);f.flareParameterAttempted=true;f.flareParameters54.reset(new(std::nothrow) NativeSunFlareParameters);
    if(f.flareParameters54){auto& p=*f.flareParameters54;p.sourceMode=true;
        at(0x133f84);p.field2C=-1;p.sourceKnownMask|=1;at(0x133f88);p.count24=0;p.sourceKnownMask|=2;
        at(0x133f8c);p.count28=0;p.sourceKnownMask|=4;at(0x133f90);p.buffer20=nullptr;p.sourceKnownMask|=8;}
    at(0x23bf90);f.flareParameterPublished=true;
    using History=g::OriginalParameterPacketHistory50LA;
    if(!History::enter())return stop(0x134014);
    struct Exit{void(*leave)();~Exit(){leave();}} exit{+[](){History::leave();}};
    f.flareParameterHistorySerial=History::state().serial;
    if(!History::guard(0,0x134014,0x134030,0x134034,pc))return stop(pc);
    //128 is divisible by4; enabled assertion's real positive condition holds.
    at(0x1340ac);if(!f.flareParameters54)return stop(pc);
    auto& p=*f.flareParameters54;p.sourcePacketAttempted=true;p.sourcePacketHistorySerial=f.flareParameterHistorySerial;
    History::state().scratchSerial=p.sourcePacketHistorySerial;History::state().scratchAccessible=true;
    p.field2C=1;at(0x306478);for(auto& word:p.unknown00To1C)word=0;p.sourceHeaderKnownMask=255;
    p.packetStarted=true;at(0x1340bc);p.count24=128;
    at(0x1340c4);p.packet.reset(new(std::nothrow) NativeSunFlarePacket);
    at(0x1340cc);p.buffer20=p.packet?p.packet->words.data():nullptr;p.sourceBuffer20=p.buffer20;p.sourceBufferPublished=true;
    if(!History::guard(1,0x1340d0,0x1340e4,0x1340e8,pc))return stop(pc);
    if(History::state().enabled[1]&&!p.buffer20)return stop(0x13410c);
    at(0x1341d4);p.count28=0;at(0x1341e4);History::state().target30=0;
    at(0x1342a0); //Actual empty1342A0 leaf, not1342B0 header finalization.
    at(0x1341f8);f.flareParameterReturned134000=true;
    at(0x23bfb4);return continueSourceTypedFlareStreams23CCA0(f,pc);
}
}

namespace ra_pod_scene50LA {
bool Owner::continueSourceTypedFlareStreams23CCA0(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(f.pc!=0x23bfb4||!preflightSourceTypedLightsNative()||!f.sourceFlare||!f.sourceFlare->strip90||
       !f.flareParameterReturned134000||!f.flareParameters54||f.flareStreamEntered)return false;
    auto& strip=*f.sourceFlare->strip90;auto& attrs=strip.attributes_;
    if(!attrs.groupCounts_||!strip.groups38_||!f.flareAttributeCount5C||*f.flareAttributeCount5C!=1||attrs.groupCounts_[0]!=4)return false;
    using Attributes=g::OriginalCableStripAttributes50LA;using History=g::OriginalStripStreamHistory23CCA0;
    at(0x23cca0);f.flareStreamEntered=true;
    //Original stackD0 is zeroed then bitfields written; no guest pointer is present.
    at(0x23ccd0);f.flareStackTagD0={};
    at(0x23ccf4);f.flareStackTagD0[0]=0x8000;
    at(0x23cd24);f.flareStackTagD0[1]=0x4000;
    at(0x23cd54);f.flareStackTagD0[1]|=0x00360000; //0x6C shifted left47 in the low64 lane.
    at(0x23cd70);f.flareStackTagD0[1]&=~0x0c000000u;
    at(0x23cd8c);f.flareStackTagD0[1]=(f.flareStackTagD0[1]&~0xf0000000u)|0x30000000u;
    at(0x23cd9c);f.flareStackTagD0[2]=2;
    at(0x23cdac);f.flareStackTagD0[2]|=0x10;
    at(0x23cdc0);f.flareStackTagD0[2]|=0x400;f.flareStackTagKnown=true;
    //Actual count loop reaches its descriptor before any stream pointer use.
    at(0x23ce08);strip.groups38_[0]=attrs.groupCounts_[0];f.flareStreamCountStored=true;
    at(0x23ce0c);if(!attrs.group_)return stop(pc);auto& group=*attrs.group_;
    group.vertexCount=4;f.flareStreamDescriptorMask|=1;
    at(0x23ce24);f.flareAttributeDescriptor10=0;f.flareStreamDescriptorMask|=2;
    at(0x23ce30);f.flareAttributeDescriptor10=1;group.batchCount=1;f.flareStreamDescriptorMask|=4;
    at(0x23ce34);if(!History::begin(0,f.flareBatchAssertion))return stop(pc);
    at(0x23ce70);if(!History::finish(f.flareBatchAssertion,group.batchCount>0))return stop(pc);
    //Seven independent nullable arrays. Native views are not source pointers;
    //their elements remain unknown until corresponding source stores occur.
    at(0x23cea4);f.flareStreamArraysAttempted|=1;group.counts.reset(new(std::nothrow) std::uint32_t[1]);
    at(0x23ceac);f.flareStreamArraysPublished|=1;
    at(0x23ceb4);f.flareStreamArraysAttempted|=2;group.batches.reset(new(std::nothrow) Attributes::Batch[1]);
    at(0x23cebc);f.flareStreamArraysPublished|=2;
    at(0x23cec4);f.flareStreamArraysAttempted|=4;group.positions.reset(new(std::nothrow) Attributes::View<Attributes::Position>[1]);
    at(0x23cecc);f.flareStreamArraysPublished|=4;
    at(0x23ced4);f.flareStreamArraysAttempted|=8;group.triples.reset(new(std::nothrow) Attributes::View<Attributes::Triple>[1]);
    at(0x23cedc);f.flareStreamArraysPublished|=8;
    at(0x23cee4);f.flareStreamArraysAttempted|=16;group.extra.reset(new(std::nothrow) std::optional<Attributes::View<Attributes::Triple>>[1]);
    at(0x23ceec);f.flareStreamArraysPublished|=16;
    at(0x23cef4);f.flareStreamArraysAttempted|=32;group.colors.reset(new(std::nothrow) std::optional<Attributes::View<Attributes::Color>>[1]);
    at(0x23cefc);f.flareStreamArraysPublished|=32;
    at(0x23cf04);f.flareStreamArraysAttempted|=64;group.texcoords.reset(new(std::nothrow) Attributes::View<Attributes::Texcoord>[1]);
    at(0x23cf0c);f.flareStreamArraysPublished|=64;
    at(0x23cf20);f.flareAliasAllocationAttempted=true;group.aliases.reset(new(std::nothrow) Attributes::VertexCopies[4]);
    if(group.aliases){for(unsigned i=0;i<4;++i){at(0x23d420);group.aliases[i].count=0;
        at(0x23d424);group.aliases[i].aliases[1].batch=0;at(0x23d428);group.aliases[i].aliases[0].batch=0;
        at(0x23d42c);group.aliases[i].aliases[1].ordinal=0;at(0x23d430);group.aliases[i].aliases[0].ordinal=0;++f.flareAliasConstructed;}}
    at(0x23cf44);f.flareAliasPublished=true;
    at(0x23cf48); //Descriptor08=0 remains represented native no emitted-word span.
    at(0x23cf58);f.flareDescriptorParameterBegin=f.flareParameters54->count28;
    at(0x23cfb4);if(!group.counts)return stop(pc);group.counts[0]=4;f.flareBatchCountStored=true;
    for(unsigned i=0;i<4;++i){at(0x23cfc8);auto& receipt=f.flareAliasAssertions[i];
        if(!History::begin(1,receipt))return stop(pc);
        //Enabled assertion loads actual alias.count before evaluating<2.
        if(receipt.enabled&&!group.aliases)return stop(0x23cff8);
        if(!History::finish(receipt,!group.aliases||group.aliases[i].count<2))return stop(0x23d008);
        at(0x23d038);if(!group.aliases)return stop(pc);auto& alias=group.aliases[i];
        if(alias.count>=alias.aliases.size())return stop(pc); //Native bound after source assertion, no invented write.
        at(0x23d048);alias.aliases[alias.count].batch=0;f.flareAliasStoreMask[i]|=1;
        at(0x23d058);alias.aliases[alias.count].ordinal=i;f.flareAliasStoreMask[i]|=2;
        at(0x23d068);++alias.count;f.flareAliasStoreMask[i]|=4;
    }
    at(0x23d088);return continueSourceTypedFlarePayload23D088(f,pc);
}
}

namespace ra_pod_scene50LA {
bool Owner::continueSourceTypedFlarePayload23D088(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(f.pc!=0x23d088||!preflightSourceTypedLightsNative()||f.flarePayloadEntered||!f.flareStackTagKnown||
       !f.sourceFlare||!f.sourceFlare->strip90||!f.flareParameters54)return false;
    using History=g::OriginalParameterPacketHistory50LA;using Attributes=g::OriginalCableStripAttributes50LA;
    auto& history=History::state();auto& p=*f.flareParameters54;auto& strip=*f.sourceFlare->strip90;
    auto& attrs=strip.attributes_;
    if(!history.busy||history.thread!=std::this_thread::get_id()||!history.scratchAccessible||
       history.serial!=f.flareParameterHistorySerial||history.scratchSerial!=f.flareParameterHistorySerial||
       p.sourcePacketHistorySerial!=f.flareParameterHistorySerial||!attrs.group_||strip.format3C_!=7)return false;
    f.flarePayloadEntered=true;
    //Native semantic backing only, admitted before further source effects. These
    //host arrays never become source addresses or command data.
    attrs.vertices_.reset(new(std::nothrow) Attributes::NativeVertices);
    if(!attrs.vertices_)return stop(pc);f.flarePayloadNativeBacking=true;
    auto& group=*attrs.group_;
    auto guard=[&](unsigned i,std::uint32_t read,std::uint32_t word,std::uint32_t byte){
        const bool okay=History::guard(i,read,word,byte,pc);f.pc=pc;return okay;};
    auto capacity=[&](unsigned count,bool scalar){
        if(!guard(6,scalar?0x1349a0:0x134860,scalar?0x1349b8:0x134878,scalar?0x1349bc:0x13487c))return false;
        if(history.enabled[6]&&p.count28>p.count24)return stop(scalar?0x1349e8:0x1348a8);
        //This exact first-use cohort fits128. Reallocation is not fabricated.
        if(p.count28>p.count24||count>p.count24-p.count28)return stop(scalar?0x134a30:0x1348f0);
        return true;
    };
    auto write=[&](unsigned index,std::optional<std::uint32_t> word){
        if(!p.packet||!p.buffer20||index>=128)return false;
        if(word)p.packet->words[index]=*word;
        p.sourcePacketWordKnown[index]=bool(word);f.flarePayloadWords[index]=word;return true;
    };
    auto begin=[&](unsigned format,bool first){
        at(0x1345e0);history.format40=format;at(0x1345e8);history.words38=0;
        at(0x134600);history.placeholder28=p.count28;
        at(0x134614);if(first)history.target30=0;
        if(!capacity(1,false))return false;
        at(0x13462c);++p.count28;f.flarePayloadCount=p.count28;return true;
    };
    auto copy=[&](const std::array<std::optional<std::uint32_t>,4>& words,unsigned count,unsigned kind){
        if(!capacity(count,false))return false;
        at(kind==0?0x134518:kind==1?0x1344a8:0x134590);history.words38+=4;
        //134550 increments scratch by FOUR even when copying only2/3 words.
        at(0x3062c0);if(!p.packet||!p.buffer20)return false;
        for(unsigned i=0;i<count;++i)if(!write(p.count28+i,words[i]))return false;
        at(kind==0?0x134538:kind==1?0x1344c8:0x1345b0);p.count28+=count;f.flarePayloadCount=p.count28;return true;
    };
    auto finish=[&](unsigned stream){
        if(!guard(4,0x134660,0x134674,0x134678))return false;
        if(history.enabled[4]&&!history.words38)return stop(0x1346a0);
        const auto qwords=(history.words38+3u)/4u;
        if(!guard(5,0x1346ec,0x134700,0x134704))return false;
        if(history.enabled[5]&&qwords>256)return stop(0x134724);
        if(p.field2C!=1)return stop(0x1347bc);
        at(0x134818);if(!write(history.placeholder28,((history.format40|0x60u)<<24)|
            ((qwords==256?0:qwords)<<16)|(history.target30|0x8000u)))return false;
        at(0x13482c);history.target30+=qwords;f.flarePayloadFinished|=1u<<stream;return true;
    };
    if(!begin(12,true))return stop(pc);
    at(0x23d0a8);if(!group.batches)return stop(pc);f.flarePayloadOffsets[0]=p.count28;
    at(0x23d0c4);f.flareStackTagD0[0]=(f.flareStackTagD0[0]&0xffff8000u)|4u;
    std::array<std::optional<std::uint32_t>,4> tag;
    for(unsigned i=0;i<4;++i)tag[i]=f.flareStackTagD0[i];
    if(!copy(tag,4,0))return stop(pc);f.flarePayloadCopies[0]=1;
    if(!finish(0))return stop(pc);
    at(0x23d100);if(!begin(12,false))return stop(pc);
    at(0x23d120);if(!group.positions)return stop(pc);f.flarePayloadOffsets[1]=p.count28;
    group.positions[0]={attrs.vertices_->positions.data(),4};
    for(unsigned i=0;i<4;++i){const std::int32_t control=i<2?-14:14;
        at(0x23d144);std::array<std::optional<std::uint32_t>,4> words{};words[3]=static_cast<std::uint32_t>(control);
        //StackC0/C4/C8 are never initialized by this routine. Do not read or zero them.
        at(0x23d148);if(!copy(words,4,1))return stop(pc);
        attrs.vertices_->positions[i].stripControl=control;++f.flarePayloadCopies[1];}
    if(!finish(1))return stop(pc);
    at(0x23d1a0);if(!begin(8,false))return stop(pc);
    at(0x23d1c0);if(!group.triples)return stop(pc);f.flarePayloadOffsets[2]=p.count28;
    group.triples[0]={attrs.vertices_->triples.data(),4};
    for(unsigned i=0;i<4;++i){at(0x23d1cc);if(!copy({},3,2))return stop(pc);++f.flarePayloadCopies[2];}
    if(!finish(2))return stop(pc);
    //Format7 excludes extra bit32; source array24 elements remain unknown.
    at(0x23d2b0);if(!begin(4,false))return stop(pc);
    at(0x23d2d0);if(!group.texcoords)return stop(pc);f.flarePayloadOffsets[3]=p.count28;
    group.texcoords[0]={attrs.vertices_->texcoords.data(),4};
    for(unsigned i=0;i<4;++i){at(0x23d2dc);if(!copy({},2,2))return stop(pc);++f.flarePayloadCopies[3];}
    if(!finish(3))return stop(pc);
    //Format7 excludes color bit8; source array28 elements remain unknown.
    at(0x23d394);if(!capacity(1,true))return stop(pc);
    at(0x134ac8);if(!write(p.count28,0x17000000u))return stop(pc);
    at(0x134ad4);++p.count28;f.flarePayloadCount=p.count28;
    at(0x23d3bc);f.flareDescriptorParameterEnd=p.count28;
    at(0x23d3d0);f.flareDescriptorParameterSpan=p.count28-*f.flareDescriptorParameterBegin;
    at(0x23d414);f.flareStreamReturned=true;
    at(0x23bfbc);return continueSourceTypedFlareHeader23BFBC(f,pc);
}
}

namespace ra_pod_scene50LA {
bool Owner::continueSourceTypedFlareHeader23BFBC(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(f.pc!=0x23bfbc||!preflightSourceTypedLightsNative()||f.flareHeaderEntered||!f.flareStreamReturned||
       !f.sourceFlare||!f.sourceFlare->strip90||!f.flareParameters54)return false;
    using History=g::OriginalParameterPacketHistory50LA;auto& history=History::state();
    auto& p=*f.flareParameters54;auto& strip=*f.sourceFlare->strip90;
    if(!history.busy||history.thread!=std::this_thread::get_id()||!history.scratchAccessible||
       history.serial!=f.flareParameterHistorySerial||history.scratchSerial!=f.flareParameterHistorySerial)return false;
    f.flareHeaderEntered=true;
    if(!History::guard(3,0x13420c,0x134220,0x134224,pc)){f.pc=pc;return false;}f.pc=pc;
    if(history.enabled[3]&&p.count28>p.count24)return stop(0x134250);
    //305700/305990 allocation alignment and the align64 native owner both admit
    //a16-aligned payload. No host address is encoded as a guest command pointer.
    if(p.count28>128)return stop(0x1342b0);
    for(unsigned i=p.count28;i%4u;++i){at(0x1342c4);
        if(!p.packet||!p.buffer20||i>=128)return stop(pc);
        p.packet->words[i]=0;p.sourcePacketWordKnown[i]=true;f.flarePayloadWords[i]=0;++f.flareHeaderPaddingWords;}
    //Padding advances only the local pointer: source count28 remains unchanged.
    at(0x134300);p.unknown00To1C[0]=(*p.unknown00To1C[0]&0xffff0000u)|
        (((p.count28+f.flareHeaderPaddingWords)/4u)&0xffffu);f.flareHeaderStoreMask|=1;
    at(0x134308);p.unknown00To1C[0]=*p.unknown00To1C[0]&0xff00ffffu;f.flareHeaderStoreMask|=2;
    at(0x13430c);p.unknown00To1C[0]=(*p.unknown00To1C[0]&0x00ffffffu)|0x30000000u;f.flareHeaderStoreMask|=4;
    at(0x134310);p.nativePayload04=p.buffer20;p.unknown00To1C[1].reset();p.sourceHeaderKnownMask&=~2u;f.flareHeaderStoreMask|=8;
    at(0x134314);p.unknown00To1C[2]=0;f.flareHeaderStoreMask|=16;
    at(0x13431c);p.unknown00To1C[3]=0;f.flareHeaderStoreMask|=32;p.packetComplete=true;
    at(0x134294);f.flareHeaderReturned=true;
    at(0x23bfd8);if(!strip.primitive30_)return stop(pc);
    f.flarePrimitiveParameters10=&p;f.flarePrimitiveStoreMask|=1;
    //Separate typed source receipt: Primitive.attributes10 is a legacy native
    //attributes projection, never the parameter54 address stored by23BFD8.
    auto& primitive=*strip.primitive30_;
    at(0x23bfec);primitive.flags0F&=~1u;f.flarePrimitiveStoreMask|=2;
    at(0x23bff8);primitive.groups22=1;f.flarePrimitiveStoreMask|=4;
    at(0x23c000);if(!strip.attributes_.groupCounts_)return stop(pc);
    const auto vertices=static_cast<std::uint16_t>(strip.attributes_.groupCounts_[0]);
    at(0x23c014);primitive.vertices20=static_cast<std::uint16_t>(primitive.vertices20+vertices);f.flarePrimitiveStoreMask|=8;
    at(0x23c040);f.flareAttributesReturned=true;
    at(0x14abd8);return continueSourceTypedFlareUV14ABD8(f,pc);
}
}

namespace ra_pod_scene50LA {
bool Owner::continueSourceTypedFlareUV14ABD8(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(f.pc!=0x14abd8||!preflightSourceTypedLightsNative()||!f.flareAttributesReturned||f.flareStripAttachLock||
       !f.sourceFlare||!f.sourceFlare->strip90||!f.flareParameters54)return false;
    auto& flare=*f.sourceFlare;auto& strip=*flare.strip90;auto& attrs=strip.attributes_;auto& p=*f.flareParameters54;
    using History=g::OriginalStripStreamHistory23CCA0;
    at(0x141970);f.flareStripAttachLock=worldApplication_->attachmentLock423F58();
    if(!*f.flareStripAttachLock){at(0x14abe0);
        const auto result=worldApplication_->entityAuthority_.attachChildren(flare.entity,strip.entity_,8u,*f.flareStripAttachLock);
        if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return stop(result.sourcePC);
        flare.stripAttached=true;
    } //Actual locked141960 returnsNULL; caller ignores return and still calls setters.
    at(0x141a5c);f.flareStripAttachReturned=true;
    for(unsigned vertex=0;vertex<4;++vertex){
        at(vertex==0?0x14ac00:vertex==1?0x14ac24:vertex==2?0x14ac48:0x14ac6c);
        auto& receipts=f.flareUVAssertions[vertex];
        //The first lazy word initializes to-1, not an enabled assertion read.
        at(0x23c820);if(!History::begin(2,receipts[0]))return stop(pc);
        at(0x23c864);if(!History::begin(3,receipts[1]))return stop(pc);
        if(receipts[1].enabled&&!attrs.group_)return stop(0x23c894);
        at(0x23c8a4);if(!History::finish(receipts[1],attrs.group_&&bool(attrs.group_->aliases)))return stop(pc);
        at(0x23c8f4);if(!attrs.group_)return stop(pc);auto& group=*attrs.group_;
        at(0x23c8e8);if(!History::begin(4,receipts[2]))return stop(pc);
        if(receipts[2].enabled&&!group.aliases)return stop(0x23c92c);
        at(0x23c93c);if(!History::finish(receipts[2],group.aliases&&group.aliases[vertex].count<3))return stop(pc);
        at(0x23c974);if(!group.aliases)return stop(pc);const auto& copies=group.aliases[vertex];
        if(copies.count>2)return stop(pc); //Native fixed alias capacity after actual source assertion.
        for(unsigned copy=0;copy<copies.count;++copy){const auto alias=copies.aliases[copy];
            at(0x23c9a0);if(!group.texcoords||alias.batch!=0||!f.flarePayloadOffsets[3])return stop(pc);
            if(alias.ordinal>=4||!attrs.vertices_)return stop(pc); //Native stable view bound, no guest address conversion.
            const unsigned offset=*f.flarePayloadOffsets[3]+2u*alias.ordinal;
            at(0x23c9a4);if(f.flarePrimitiveParameters10!=&p||!p.packet||!p.buffer20)return stop(pc);
            for(unsigned lane=0;lane<2;++lane){const std::uint32_t word=(lane==0?vertex>=2:(vertex&1))?0x3f800000u:0u;
                at(lane==0?0x23c9b4:0x23c9b8);if(offset+lane>=p.count28||offset+lane>=128)return stop(pc);
                p.packet->words[offset+lane]=word;p.sourcePacketWordKnown[offset+lane]=true;f.flarePayloadWords[offset+lane]=word;
                float value;std::memcpy(&value,&word,sizeof value);
                attrs.vertices_->texcoords[alias.ordinal].uv[lane]={value,0xffu};f.flareUVStoreMask[vertex]|=1u<<lane;
            }
        }
        at(0x23c9ec);++f.flareUVCalls;
    }
    at(0x14ac74);return continueSourceTypedFlareVectors14AC74(f,pc);
}
}

namespace ra_pod_scene50LA {
bool Owner::continueSourceTypedFlareVectors14AC74(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(f.pc!=0x14ac74||!preflightSourceTypedLightsNative()||f.flareUVCalls!=4||f.flareVectorCalls||
       !f.sourceFlare||!f.sourceFlare->strip90||!f.flareParameters54)return false;
    auto& strip=*f.sourceFlare->strip90;auto& attrs=strip.attributes_;auto& p=*f.flareParameters54;
    using History=g::OriginalStripStreamHistory23CCA0;
    for(unsigned vertex=0;vertex<4;++vertex)for(unsigned kind=0;kind<2;++kind){
        //Actual14AC90 triple first, then14ACB0 position, for each vertex.
        const bool triple=kind==0;const unsigned call=2*vertex+kind,first=triple?5:8;
        auto& receipts=f.flareVectorAssertions[call];at(triple?0x14ac90:0x14acb0);
        at(triple?0x23c278:0x23c0a8);if(!History::begin(first,receipts[0]))return stop(pc);
        at(triple?0x23c2b4:0x23c0e4);if(!History::begin(first+1,receipts[1]))return stop(pc);
        if(receipts[1].enabled&&!attrs.group_)return stop(triple?0x23c2e4:0x23c114);
        at(triple?0x23c2f4:0x23c124);if(!History::finish(receipts[1],attrs.group_&&bool(attrs.group_->aliases)))return stop(pc);
        at(triple?0x23c32c:0x23c15c);if(!attrs.group_)return stop(pc);auto& group=*attrs.group_;
        at(triple?0x23c334:0x23c164);if(!History::begin(first+2,receipts[2]))return stop(pc);
        if(receipts[2].enabled&&!group.aliases)return stop(triple?0x23c360:0x23c190);
        at(triple?0x23c370:0x23c1a0);if(!History::finish(receipts[2],group.aliases&&group.aliases[vertex].count<3))return stop(pc);
        at(triple?0x23c3a8:0x23c1d8);if(!group.aliases)return stop(pc);const auto& copies=group.aliases[vertex];
        if(copies.count>2)return stop(pc); //Native fixed alias bound, after source assertion.
        for(unsigned copy=0;copy<copies.count;++copy){const auto alias=copies.aliases[copy];
            at(triple?0x23c3b4:0x23c1e4);
            if(alias.batch!=0)return stop(pc); //Native single-batch bound; nullable array is dereferenced later.
            const unsigned stream=triple?2:1;
            at(triple?0x23c3c0:0x23c1f0);
            std::uint32_t word=0;if(!worldApplication_->readFlareVectorWord(triple,0,word))return stop(pc);
            f.flareVectorReadWords[call][0]=word;
            at(triple?0x23c3d4:0x23c208);
            if((triple?!group.triples:!group.positions)||!f.flarePayloadOffsets[stream]||alias.ordinal>=4||!attrs.vertices_)return stop(pc);
            const unsigned offset=*f.flarePayloadOffsets[stream]+(triple?3u:4u)*alias.ordinal;
            at(triple?0x23c3e0:0x23c20c);if(f.flarePrimitiveParameters10!=&p)return stop(pc);
            for(unsigned lane=0;lane<3;++lane){
                if(lane){at(triple?(lane==1?0x23c3f4:0x23c3fc):(lane==1?0x23c220:0x23c228));
                    if(!worldApplication_->readFlareVectorWord(triple,lane,word))return stop(pc);f.flareVectorReadWords[call][lane]=word;}
                at(triple?(lane==0?0x23c3f0:lane==1?0x23c3f8:0x23c400):(lane==0?0x23c21c:lane==1?0x23c224:0x23c22c));
                if(!p.packet||!p.buffer20||offset+lane>=p.count28||offset+lane>=128)return stop(pc);
                p.packet->words[offset+lane]=word;p.sourcePacketWordKnown[offset+lane]=true;f.flarePayloadWords[offset+lane]=word;
                float value;std::memcpy(&value,&word,4);
                auto& scalar=triple?attrs.vertices_->triples[alias.ordinal].components[lane]:attrs.vertices_->positions[alias.ordinal].xyz[lane];
                scalar={value,0xffu};f.flareVectorStoreMask[call]|=1u<<lane;
            } //Position fourth/control word is deliberately untouched.
        }
        at(triple?0x23c42c:0x23c254);++f.flareVectorCalls;
    }
    at(0x14ace4);f.flareReturned14AB30=true;
    at(0x27807c);return continueSourceFlareObserver14BA00(f,pc);
}
}

namespace ra_pod_scene50LA {
bool Owner::sourceListEntriesCurrent(const NativeListMethodCustody& c)const noexcept {
    const auto& s=c.source_;const auto& e=c.sourceEntries_;
    if(!s.pointer3C||!s.blockCount34||!s.lastUsed38||!s.field40)return false;
    if(!g::search_block_guards27E3C0::valid(e.tableGuard)||!g::search_block_guards27E3C0::valid(e.blockGuard)||
       bool(e.pending)!=bool(e.pendingIdentity)||(e.pendingSlot&&!e.pending)||(e.pendingConsumed&&!e.pendingSlot)||
       (e.abandonedConsumed&&!e.terminal))return false;
    if(!e.entered){if(e.tableAttempted||e.blockAttempted||e.pending||e.table||e.words||*s.pointer3C||*s.blockCount34||
        *s.lastUsed38!=1024||*s.field40)return false;}
    if(e.tablePublished&&(!e.tableAttempted||!e.tableGuard.returned||*s.pointer3C!=e.table.get()))return false;
    if(!e.tablePublished&&*s.pointer3C)return false;
    if(e.blockAttempted&&!e.tablePublished)return false;
    if(e.blockPublished&&(!e.blockAttempted||!e.table||e.table[0]!=e.words.get()))return false;
    if(e.ready&&(!e.blockPublished||!e.blockGuard.returned))return false;
    if(!e.ready){if(*s.blockCount34||*s.lastUsed38!=1024||*s.field40||e.pendingSlot)return false;
        for(unsigned i=0;i<1024;++i)if(e.wordKnown[i]||e.members[i]||e.borrowers[i])return false;return true;}
    if(*s.blockCount34!=1||*s.lastUsed38>1024||*s.field40>1024||
       (e.abandonedConsumed&&(!*e.abandonedConsumed||*e.abandonedConsumed>*s.lastUsed38)))return false;
    std::array<bool,1024> freed{};auto next=*s.field40;
    while(next){if(next>*s.lastUsed38||freed[next-1]||e.wordKnown[next-1]||e.members[next-1]||e.nativeFreeNext[next-1]>1024)return false;freed[next-1]=true;next=e.nativeFreeNext[next-1];}
    for(unsigned i=0;i<1024;++i){if(i>=*s.lastUsed38){if(e.wordKnown[i]||e.members[i]||e.borrowers[i])return false;continue;}
        if(e.members[i]){if(!e.borrowers[i]||e.wordKnown[i]||freed[i])return false;}
        else if(e.borrowers[i]||(!freed[i]&&!(e.pendingConsumed&&e.pendingSlot==i+1)&&e.abandonedConsumed!=i+1))return false;}
    return !e.pendingSlot||(e.pendingConsumed?e.pendingSlot<=*s.lastUsed38:e.pendingSlot==(*s.field40?*s.field40:*s.lastUsed38+1));
}
bool Owner::preflightSourceFlareObserverNative(const SourceTypedLightFrame& f)const noexcept {
    using Shared=g::OriginalGarageObserverShared50KG;
    for(const auto* r:{&f.flareObserverEntryAssertion,&f.flareObserverChildAssertion,&f.flareObserverMethodAssertion,&f.flareObserverSlotAssertion,&f.flareObserverReturnAssertion})
        if(!Shared::sourceAssertionHistoryCurrent(*r))return false;
    if(!preflightSourceFlareSecondaryNative(f))return false;
    if(f.flareObserverCallReturned&&(!f.flareSecondaryReturned||!f.flareObserverReturnAssertion.passed))return false;
    if(f.flare146AE0Entered&&!f.flareObserverCallReturned)return false;
    if(f.flareObserverWithdrawn)return !f.flareVisibilityNode&&!f.flareVisibilityMethod&&!f.flareSelectionPending&&
        (!f.sourceFlare||(!f.sourceFlare->observer94&&!f.sourceFlare->observerOwner));
    if(!f.flareObserverEntered)return !f.flareVisibilityNode&&!f.flareVisibilityMethod&&!f.flareObserverOuter&&!f.flareSelectionPending&&
        (!f.sourceFlare||(!f.sourceFlare->observer94&&!f.sourceFlare->observerOwner));
    if(!worldApplication_||!f.flareReturned14AB30||!f.sourceFlare||!f.zone)return false;
    const auto zone=*f.zone;
    if(zone<0||zone>=10||!sourceVisibility4C0_||!*sourceVisibility4C0_||
       f.flareVisibilityNode!=(*sourceVisibility4C0_)[zone]||!f.flareVisibilityNode)return false;
    const auto& flare=*f.sourceFlare;const auto* o=flare.observerOwner.get();
    if(flare.observer94!=(f.flareObserverPublished?o:nullptr)||f.flareObserverScalarMask>63||f.flarePrimaryStoreMask>31)return false;
    if(o){if(!f.flareObserverOuter)return false;
        auto& shared=worldApplication_->garageObserverShared_;
        if(f.flareObserverLease&&(f.flareObserverReturned?!shared.ownsReturnedSourceObserver(o):!shared.sourceObserverCurrent280890(o)))return false;
        if(o->sharedConstructed!=f.flareObserverReturned||o->primaryRegistered!=(f.flarePrimaryStoreMask==31))return false;
        if(o->child04!=((f.flarePrimaryStoreMask&1)?&f.flareVisibilityNode->entity:nullptr)||
           o->method08!=((f.flarePrimaryStoreMask&2)?f.flareVisibilityMethod:nullptr)||
           o->slot14!=((f.flarePrimaryStoreMask&4)&&f.flarePrimarySlot?*f.flarePrimarySlot:0u)||o->field0C||o->field10)return false;
        for(const auto& word:o->unknown18To1C)if(word)return false;
        if(f.flareObserverVectorKnown&&o->center20!=((f.flarePrimaryStoreMask&16)?flare.vectors40To80[0]:std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}}))return false;
    }else if(f.flareObserverLease||f.flareObserverReturned||f.flarePrimaryStoreMask)return false;
    if(f.flareVisibilityMethod){const auto& c=*f.flareVisibilityMethod;
        if(f.flareVisibilityNode->nativeList34!=&c||!sourceListMethodArrayCurrent(0,zone,&c)||!sourceListEntriesCurrent(c))return false;
        const auto& e=c.sourceEntries_;
        if(f.flareSelectionPending!=(e.pending==&f))return false;
        if(f.flarePrimaryInserted&&!f.flarePrimaryWithdrawn){if(!f.flarePrimarySlot||*f.flarePrimarySlot==0||*f.flarePrimarySlot>1024||
            e.members[*f.flarePrimarySlot-1]!=&flare.entity||e.borrowers[*f.flarePrimarySlot-1]!=&f)return false;}
    }else if(f.flareSelectionPending||f.flarePrimaryInserted)return false;
    return true;
}
bool Owner::withdrawSourceFlareObserverNative(SourceTypedLightFrame& f)noexcept {
    if(!preflightSourceFlareObserverNative(f))return false;
    if(f.flareObserverWithdrawn||!f.flareObserverEntered)return true;
    if(!withdrawSourceFlareSecondaryNative(f))return false;
    if(f.flareVisibilityMethod){auto& c=*f.flareVisibilityMethod;auto& e=c.sourceEntries_;auto& s=c.source_;
        if(f.flareSelectionPending){if(e.pending!=&f)return false;
            if(e.pendingConsumed){e.abandonedConsumed=e.pendingSlot;e.terminal=true;e.pendingConsumed=false;}
            e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;f.flareSelectionPending=false;}
        if(f.flarePrimaryInserted&&!f.flarePrimaryWithdrawn){if(!f.flarePrimarySlot||!e.words||e.pending)return false;const auto slot=*f.flarePrimarySlot;
            //Native free-chain handles retain custody without fabricating guest pointer bits.
            e.nativeFreeNext[slot-1]=*s.field40;e.wordKnown[slot-1]=false;e.members[slot-1]=e.borrowers[slot-1]=nullptr;s.field40=slot;
            f.flarePrimaryWithdrawn=true;}
    }
    if(f.sourceFlare&&f.sourceFlare->observerOwner){auto* o=f.sourceFlare->observerOwner.get();
        if(f.flareObserverLease){auto& shared=worldApplication_->garageObserverShared_;
            if(f.flareObserverReturned?!shared.retireReturnedSourceObserverNative(o):!shared.abortSourceObserver280890(o))return false;}
        f.sourceFlare->observer94=nullptr;f.sourceFlare->observerOwner.reset();}
    f.flareVisibilityNode=nullptr;f.flareVisibilityMethod=nullptr;f.flareObserverWithdrawn=true;return true;
}
bool Owner::continueSourceFlareObserver14BA00(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(f.pc!=0x27807c||!preflightSourceTypedLightsNative()||f.flareObserverEntered||!f.flareReturned14AB30||
       !f.sourceFlare||!f.descriptor||!sourceDefaultObserver_||!sourceDefaultObserverCtorReturned_||!sourceDefaultObserverLease_)return false;
    auto& flare=*f.sourceFlare;auto& shared=worldApplication_->garageObserverShared_;using Shared=g::OriginalGarageObserverShared50KG;
    at(0x278094);const auto zone=f.descriptor->zone20;
    if(zone<0||zone>=10||!sourceVisibility4C0_||!*sourceVisibility4C0_||static_cast<std::size_t>(zone)>=sourceVisibilityPublished_)return stop(pc);
    f.flareVisibilityNode=(*sourceVisibility4C0_)[zone];if(!f.flareVisibilityNode)return stop(pc);f.flareObserverEntered=true;
    at(0x14ba14);if(!Shared::beginSourceAssertion(8,f.flareObserverEntryAssertion))return stop(pc);
    at(0x14ba5c);if(!Shared::finishSourceAssertion(f.flareObserverEntryAssertion,flare.observer94==nullptr))return stop(pc);
    at(0x14ba90);f.flareObserverOuter=true;flare.observerOwner.reset(new(std::nothrow) NativeLocalFlareObserver);
    if(flare.observerOwner){auto& o=*flare.observerOwner;
        if(!shared.beginSourceObserver280890(&o,sourceDefaultObserver_.get()))return stop(0);
        f.flareObserverLease=true;at(0x2808a4);o.center20={{0,0,0,0x3f800000u}};f.flareObserverVectorKnown=true;
        if(!shared.sourceObserverAllocator2808AC(&o,pc))return stop(pc);
        at(0x2808e0);o.next00=nullptr;f.flareObserverScalarMask|=1;
        at(0x2808e4);o.child04=nullptr;f.flareObserverScalarMask|=2;
        at(0x2808e8);o.method08=nullptr;f.flareObserverScalarMask|=4;
        at(0x2808ec);o.field0C=0;f.flareObserverScalarMask|=8;
        at(0x2808f0);o.field10=0;f.flareObserverScalarMask|=16;
        at(0x2808f4);o.slot14=0;f.flareObserverScalarMask|=32;
        if(!shared.sourceObserverIncrement28090C(&o,pc))return stop(pc);
        at(0x280918);if(!shared.finishSourceObserver280918(&o))return stop(pc);
        o.sharedConstructed=true;f.flareObserverReturned=true;
    }
    at(0x14bab0);flare.observer94=flare.observerOwner.get();f.flareObserverPublished=true;
    at(0x2809e8);if(!Shared::beginSourceAssertion(1,f.flareObserverChildAssertion))return stop(pc);
    if(f.flareObserverChildAssertion.enabled&&!flare.observerOwner)return stop(0x280a2c);
    at(0x280a40);if(!Shared::finishSourceAssertion(f.flareObserverChildAssertion,flare.observerOwner&&flare.observerOwner->child04==nullptr))return stop(pc);
    at(0x27f5a0);f.flareVisibilityMethod=f.flareVisibilityNode->nativeList34;
    at(0x280a80);if(!Shared::beginSourceAssertion(2,f.flareObserverMethodAssertion))return stop(pc);
    at(0x280ab8);if(!Shared::finishSourceAssertion(f.flareObserverMethodAssertion,f.flareVisibilityMethod!=nullptr))return stop(pc);
    at(0x280ae8);if(!f.flareVisibilityMethod)return stop(pc);
    auto& c=*f.flareVisibilityMethod;auto& s=c.source_;auto& e=c.sourceEntries_;
    if(!sourceListMethodArrayCurrent(0,zone,&c)||!sourceListEntriesCurrent(c)||e.pending||e.terminal||!c.sourceMode_||
       c.application_!=worldApplication_.get()||c.world_!=this||c.usage10_!=1||c.reference00_!=2)return stop(0x280afc);
    //ActualList virtual20=27E3B0 passes a3=0 to27E3C0. Position/vector
    //arguments are not guest slot words and are not writable aliases here.
    at(0x280afc);e.entered=true;e.pending=&f;e.pendingIdentity=&flare.entity;f.flareSelectionPending=true;
    if(!*s.blockCount34){
        at(0x27e410);e.tableAttempted=true;e.table.reset(new(std::nothrow) std::uint32_t*[1]);
        if(!g::search_block_guards27E3C0::run(0,bool(e.table),e.tableGuard,pc))return stop(pc);
        at(0x27e484);s.pointer3C=e.table.get();e.tablePublished=true;
        at(0x27e48c);e.blockAttempted=true;e.words.reset(new(std::nothrow) std::uint32_t[1024]);
        at(0x27e4a4);if(!e.table)return stop(pc);e.table[0]=e.words.get();e.blockPublished=true;
        if(!g::search_block_guards27E3C0::run(1,bool(e.words),e.blockGuard,pc))return stop(pc);
        at(0x27e52c);s.blockCount34=1;at(0x27e530);s.lastUsed38=0;e.ready=true;
    }
    if(*s.field40)e.pendingSlot=*s.field40;
    else {if(*s.lastUsed38==1024)return stop(0x27e410);e.pendingSlot=*s.lastUsed38+1;}
    at(0x27e564);f.flarePrimarySlot=e.pendingSlot;
    at(0x280b08);if(!Shared::beginSourceAssertion(3,f.flareObserverSlotAssertion))return stop(pc);
    at(0x280b40);if(!Shared::finishSourceAssertion(f.flareObserverSlotAssertion,*f.flarePrimarySlot!=0))return stop(pc);
    const auto slot=*f.flarePrimarySlot;at(0x280b88);
    if(!slot||slot>1024||slot!=e.pendingSlot||e.pending!=&f||e.pendingIdentity!=&flare.entity)return stop(pc);
    if(*s.field40==slot){at(0x27e594);if(!e.words||e.members[slot-1])return stop(pc);s.field40=e.nativeFreeNext[slot-1];}
    else {at(0x27e5a0);s.lastUsed38=*s.lastUsed38+1;}
    e.pendingConsumed=true;
    at(0x27e5a8);if(!e.words)return stop(pc);
    e.members[slot-1]=&flare.entity;e.borrowers[slot-1]=&f;e.wordKnown[slot-1]=false;
    e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;f.flareSelectionPending=false;f.flarePrimaryInserted=true;
    at(0x280b90);if(!flare.observerOwner)return stop(pc);auto& o=*flare.observerOwner;
    o.child04=&f.flareVisibilityNode->entity;f.flarePrimaryStoreMask|=1;
    at(0x280b94);o.method08=&c;f.flarePrimaryStoreMask|=2;
    at(0x280b98);o.slot14=slot;f.flarePrimaryStoreMask|=4;
    at(0x280b9c);o.next00=nullptr;f.flarePrimaryStoreMask|=8;
    at(0x280bbc);o.center20=flare.vectors40To80[0];f.flarePrimaryStoreMask|=16;o.primaryRegistered=true;
    at(0x280bf0);return continueSourceFlareSecondary280BF0(f,pc);
}
}

namespace ra_pod_scene50LA {
bool Owner::selectSourceObserverSlot27E3C0(NativeListMethodCustody& c,const void* occurrence,const void* identity,
    std::optional<std::uint32_t>& slot,bool& pending,std::uint32_t& pc,std::uint32_t expectedUsage)noexcept {
    auto& e=c.sourceEntries_;auto& s=c.source_;
    if(!occurrence||!identity||!sourceListEntriesCurrent(c)||e.pending||e.terminal||!c.sourceMode_||
       c.application_!=worldApplication_.get()||c.world_!=this||(expectedUsage!=1&&expectedUsage!=2)||
       c.usage10_!=expectedUsage||c.reference00_!=2)return false;
    //Existing visibility callers retain default usage1. Only the separately
    //authenticated actual collisionB8/+38 caller requests usage2.
    e.entered=true;e.pending=occurrence;e.pendingIdentity=identity;pending=true;
    if(!*s.blockCount34){
        pc=0x27e410;e.tableAttempted=true;e.table.reset(new(std::nothrow) std::uint32_t*[1]);
        if(!g::search_block_guards27E3C0::run(0,bool(e.table),e.tableGuard,pc))return false;
        pc=0x27e484;s.pointer3C=e.table.get();e.tablePublished=true;
        pc=0x27e48c;e.blockAttempted=true;e.words.reset(new(std::nothrow) std::uint32_t[1024]);
        pc=0x27e4a4;if(!e.table)return false;e.table[0]=e.words.get();e.blockPublished=true;
        if(!g::search_block_guards27E3C0::run(1,bool(e.words),e.blockGuard,pc))return false;
        pc=0x27e52c;s.blockCount34=1;pc=0x27e530;s.lastUsed38=0;e.ready=true;
    }
    if(*s.field40)e.pendingSlot=*s.field40;
    else {if(*s.lastUsed38==1024){pc=0x27e410;return false;}e.pendingSlot=*s.lastUsed38+1;}
    pc=0x27e564;slot=e.pendingSlot;return true;
}
bool Owner::preflightSourceFlareSecondaryNative(const SourceTypedLightFrame& f)const noexcept {
    const auto* o=f.sourceFlare?f.sourceFlare->observerOwner.get():nullptr;
    if(f.flareSecondaryCount>20||(f.flareSecondaryReturned&&!f.flareSecondaryEntered))return false;
    if(f.flareSecondaryWithdrawn){
        if(o&&(o->next00||o->portalCursor||o->cursorMethod))return false;
        for(const auto& s:f.flareSecondary)if(s.edge||s.neighbor||s.portal||s.method||s.link||s.oldHead||s.selectionPending)return false;
        return true;
    }
    if(!f.flareSecondaryEntered){if(f.flareSecondaryCount)return false;
        return !o||(!o->next00&&!o->portalCursor&&!o->cursorMethod&&!o->portalLoopStarted&&!o->portalLoopComplete);}
    if(!o||!f.flareObserverReturned||f.flarePrimaryStoreMask!=31||!f.flareVisibilityNode||!worldApplication_||
       !o->portalLoopStarted||o->portalLoopComplete!=f.flareSecondaryReturned)return false;
    if(!f.zone||*f.zone<0||*f.zone>=10||
       !sourcePortalWorldLinksCurrent(*f.flareVisibilityNode,false,static_cast<std::size_t>(*f.zone)))return false;
    const auto* expected=f.flareVisibilityNode->portalLinks30.get();
    const NativeListMethodCustody* method=f.flareVisibilityMethod;const void* head=nullptr;
    const auto& pool=worldApplication_->garageObserverShared_;
    for(std::size_t i=0;i<f.flareSecondary.size();++i){const auto& s=f.flareSecondary[i];
        if(i>=f.flareSecondaryCount){if(s.entered||s.complete||s.edge||s.method||s.selectionPending||s.inserted||s.link||s.headPublished)return false;continue;}
        if(!s.entered||!expected||s.edge!=expected||s.edgeIndex>=40||(s.edgeIndex&1u)||
           s.neighbor!=expected->neighbor04||s.portal!=expected->portal08||s.edgeDistanceMask>3||s.linkStoreMask>127||
           !g::source_portal_overlap231150::historyCurrent(s.overlap))return false;
        const auto& r=sourcePortalWorldLinks_[s.edgeIndex/4][s.edgeIndex%4];
        if(r.withdrawn||r.published!=expected||!r.to||&r.to->entity!=s.neighbor||r.knownMask!=31)return false;
        if(s.edgeDistanceMask&&(!s.overlap.returned||!s.overlap.hit||!s.overlap.outputDistance))return false;
        //The graph owns latest stores; older occurrence receipts remain historical if later callers overwrite them.
        if(s.methodReturned){if(s.edgeDistanceMask!=3||!method)return false;
            const auto usage=method->usage10_;const auto* wanted=(usage&1u)?r.to->nativeList34:(usage&2u)?r.to->nativeList38:nullptr;
            if(s.method!=wanted)return false;method=s.method;
        }else if(s.method)return false;
        if(s.selectionAttempted&&(!s.methodReturned||!s.method))return false;
        if(s.slot&&!s.selectionAttempted)return false;
        if(s.inserted&&(!s.slot||!*s.slot||s.selectionPending))return false;
        if(s.nativeWithdrawn&&!s.inserted)return false;
        if(s.method){const auto& e=s.method->sourceEntries_;
            std::size_t zone=0;while(zone<10&&(!sourceVisibilitySlots4C0_||sourceVisibilitySlots4C0_[zone]!=r.to))++zone;
            if(zone==10||!sourceListMethodArrayCurrent(0,zone,s.method))return false;
            if(!sourceListEntriesCurrent(*s.method)||s.selectionPending!=(e.pending==&s))return false;
            if(s.selectionPending&&e.pendingIdentity!=&f.sourceFlare->entity)return false;
            if(s.inserted&&!s.nativeWithdrawn){if(*s.slot>1024||e.members[*s.slot-1]!=&f.sourceFlare->entity||e.borrowers[*s.slot-1]!=&s)return false;}
        }
        if(s.allocationAttempted&&!s.inserted)return false;
        if(s.allocationReturned&&!s.allocationAttempted)return false;
        if(s.link){if(!s.allocationReturned||s.linkReleased||!pool.ownsSourceLink(sourceDefaultObserver_.get(),o,s.link))return false;
            const auto& link=*s.link;
            if((s.linkStoreMask&1)&&link.slot14!=s.slot.value_or(0))return false;
            if((s.linkStoreMask&2)&&link.child04!=s.neighbor)return false;
            if((s.linkStoreMask&4)&&(link.collisionMethod08!=s.method||link.method08))return false;
            if((s.linkStoreMask&8)&&link.portal0C!=s.portal)return false;
            if((s.linkStoreMask&16)&&(!s.overlap.outputDistance||link.distance10!=*s.overlap.outputDistance))return false;
            if((s.linkStoreMask&32)&&link.next00!=s.oldHead)return false;
        }else if(s.linkStoreMask&&!s.linkReleased)return false;
        if(s.headPublished){if(s.linkStoreMask!=127)return false;
            if(!s.linkReleased){if(s.oldHead!=head)return false;head=s.link;}}
        if(s.complete){if(!s.overlap.returned||(s.overlap.hit&&(!s.slot||(*s.slot&&!s.headPublished))))return false;
            expected=expected->next00.get();
        }else if(i+1!=f.flareSecondaryCount)return false;
    }
    if(o->next00!=head||o->portalCursor!=expected||o->cursorMethod!=method)return false;
    return !f.flareSecondaryReturned||!expected;
}
bool Owner::withdrawSourceFlareSecondaryNative(SourceTypedLightFrame& f)noexcept {
    if(!preflightSourceFlareSecondaryNative(f))return false;
    if(!f.flareSecondaryEntered||f.flareSecondaryWithdrawn)return true;
    auto& o=*f.sourceFlare->observerOwner;auto& pool=worldApplication_->garageObserverShared_;
    for(std::size_t i=f.flareSecondaryCount;i>0;--i){auto& s=f.flareSecondary[i-1];
        if(s.selectionPending){auto& e=s.method->sourceEntries_;if(e.pending!=&s)return false;
            if(e.pendingConsumed){e.abandonedConsumed=e.pendingSlot;e.terminal=true;e.pendingConsumed=false;}
            e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;s.selectionPending=false;}
        if(s.inserted&&!s.nativeWithdrawn){auto& c=*s.method;auto& e=c.sourceEntries_;
            if(!s.slot||!*s.slot||*s.slot>1024||e.pending||!e.words||e.members[*s.slot-1]!=&f.sourceFlare->entity||e.borrowers[*s.slot-1]!=&s)return false;
            const auto slot=*s.slot;e.nativeFreeNext[slot-1]=*c.source_.field40;e.wordKnown[slot-1]=false;
            e.members[slot-1]=e.borrowers[slot-1]=nullptr;c.source_.field40=slot;s.nativeWithdrawn=true;}
        if(s.link&&!s.linkReleased){if(s.headPublished&&o.next00!=s.link)return false;
            const void* next=s.headPublished?s.link->next00:o.next00;std::uint32_t pc=0;
            if(!pool.releaseSource105830(sourceDefaultObserver_.get(),&o,s.link,pc))return false;
            o.next00=next;s.linkReleased=true;s.link=nullptr;}
    }
    for(auto& s:f.flareSecondary){s.edge=nullptr;s.neighbor=nullptr;s.portal=nullptr;s.method=nullptr;s.oldHead=nullptr;}
    o.portalCursor=nullptr;o.cursorMethod=nullptr;f.flareSecondaryWithdrawn=true;return true;
}
bool Owner::continueSourceFlareSecondary280BF0(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(f.pc!=0x280bf0||!preflightSourceTypedLightsNative()||f.flareSecondaryEntered||f.flareObserverWithdrawn||
       !f.sourceFlare||!f.sourceFlare->observerOwner||f.flarePrimaryStoreMask!=31||!f.flareVisibilityNode)return false;
    auto& flare=*f.sourceFlare;auto& o=*flare.observerOwner;auto& pool=worldApplication_->garageObserverShared_;
    f.flareSecondaryEntered=true;o.portalLoopStarted=true;
    at(0x280c1c);if(o.child04!=&f.flareVisibilityNode->entity)return stop(pc);
    at(0x280c20);o.cursorMethod=o.method08;at(0x280c34);o.portalCursor=f.flareVisibilityNode->portalLinks30.get();
    while(o.portalCursor){
        std::size_t index=0;while(index<40&&sourcePortalWorldLinks_[index/4][index%4].published!=o.portalCursor)++index;
        if(index==40||(index&1u)||f.flareSecondaryCount==20)return stop(0x280c40);
        auto& r=sourcePortalWorldLinks_[index/4][index%4];
        if(r.withdrawn||r.knownMask!=31||!r.to||!r.from)return stop(0x280c40);
        auto& edge=*r.published;auto& s=f.flareSecondary[f.flareSecondaryCount++];
        s.entered=true;s.edgeIndex=index;s.edge=&edge;s.neighbor=edge.neighbor04;s.portal=edge.portal08;
        const auto& portal=nativePortals_[index/4];at(0x280c40);
        if(!portal||!portal->sourceMode||&portal->entity!=s.portal||!portal->source.loadReturned||
           portal->source.normalKnown!=7||!portal->source.normalNormalized||portal->source.copiedVertices!=4)return stop(0x231150);
        for(unsigned e=0;e<4;++e)if(portal->source.edgeKnown[e]!=7||!portal->source.edgeNormalized[e])return stop(0x231150);
        //14BAD0 supplies radius +0. Exact source XYZ lanes; unknown W never promoted.
        at(0x231150);if(!g::source_portal_overlap231150::run(portal->source.vertices30,portal->source.normal70,
            portal->source.edges80,flare.vectors40To80[0],0u,s.overlap))return stop(s.overlap.pc);
        at(0x280c54);if(!s.overlap.hit){s.complete=true;at(0x280cf8);o.portalCursor=edge.next00.get();continue;}
        if(!s.overlap.outputDistance)return stop(0x280c5c);
        at(0x280c60);edge.field10=*s.overlap.outputDistance;r.distance10=s.overlap.outputDistance;s.edgeDistanceMask|=1;
        at(0x280c64);edge.field0C=*s.overlap.outputDistance;r.distance0C=s.overlap.outputDistance;s.edgeDistanceMask|=2;
        at(0x280c6c);if(!o.cursorMethod)return stop(pc);const auto usage=o.cursorMethod->usage10_;
        if(&r.to->entity!=s.neighbor)return stop(0x27f5a0);
        s.method=(usage&1u)?r.to->nativeList34:(usage&2u)?r.to->nativeList38:nullptr;s.methodReturned=true;
        at(0x280c7c);o.cursorMethod=s.method;if(!s.method)return stop(0x280c78);
        std::size_t zone=0;while(zone<10&&(!sourceVisibilitySlots4C0_||sourceVisibilitySlots4C0_[zone]!=r.to))++zone;
        if(zone==10||!sourceListMethodArrayCurrent(0,zone,s.method))return stop(0x280c90);
        s.selectionAttempted=true;at(0x280c90);
        if(!selectSourceObserverSlot27E3C0(*s.method,&s,&flare.entity,s.slot,s.selectionPending,pc))return stop(pc);
        at(0x280c9c);if(!*s.slot){auto& e=s.method->sourceEntries_;e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;
            s.selectionPending=false;s.complete=true;at(0x280cf8);o.portalCursor=edge.next00.get();continue;}
        auto& c=*s.method;auto& e=c.sourceEntries_;const auto slot=*s.slot;at(0x280cbc);
        if(slot>1024||slot!=e.pendingSlot||e.pending!=&s||e.pendingIdentity!=&flare.entity)return stop(pc);
        if(*c.source_.field40==slot){at(0x27e594);if(!e.words||e.members[slot-1])return stop(pc);c.source_.field40=e.nativeFreeNext[slot-1];}
        else {at(0x27e5a0);c.source_.lastUsed38=*c.source_.lastUsed38+1;}
        e.pendingConsumed=true;at(0x27e5a8);if(!e.words)return stop(pc);
        e.members[slot-1]=&flare.entity;e.borrowers[slot-1]=&s;e.wordKnown[slot-1]=false;
        e.pending=e.pendingIdentity=nullptr;e.pendingSlot=0;e.pendingConsumed=false;s.selectionPending=false;s.inserted=true;
        at(0x280cc8);s.allocationAttempted=true;const auto allocated=pool.allocateSource105720(sourceDefaultObserver_.get(),&o);
        if(allocated.status==g::OriginalGarageObserverShared50KG::SourceLinkStatus::nativeRefused)return stop(allocated.sourcePC);
        s.allocationReturned=true;s.link=allocated.record;
        at(0x280cd0);if(!s.link)return stop(pc); //List admission survives the original NULL link store.
        s.link->slot14=slot;s.linkStoreMask|=1;at(0x280cd4);s.link->child04=s.neighbor;s.linkStoreMask|=2;
        at(0x280cd8);s.link->collisionMethod08=s.method;s.link->method08=nullptr;s.linkStoreMask|=4;
        at(0x280ce0);s.link->portal0C=s.portal;s.linkStoreMask|=8;
        at(0x280ce8);s.link->distance10=*s.overlap.outputDistance;s.linkStoreMask|=16;
        at(0x280cec);s.oldHead=o.next00;
        at(0x280cf0);s.link->next00=static_cast<g::OriginalGarageObserverShared50KG::Link105720*>(const_cast<void*>(s.oldHead));s.linkStoreMask|=32;
        at(0x280cf4);o.next00=s.link;s.linkStoreMask|=64;s.headPublished=true;
        s.complete=true;at(0x280cf8);o.portalCursor=edge.next00.get();
    }
    at(0x280d30);f.flareSecondaryReturned=true;o.portalLoopComplete=true;
    at(0x280be4);at(0x14bad8);return finishSourceFlare14BA00And146AE0(f,pc);
}
}

namespace ra_pod_scene50LA {
bool Owner::finishSourceFlare14BA00And146AE0(SourceTypedLightFrame& f,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};const auto stop=[&](std::uint32_t p){at(p);return false;};
    if(f.pc!=0x14bad8||!preflightSourceTypedLightsNative()||!f.flareSecondaryReturned||f.flareObserverCallReturned||
       f.flare146AE0Entered||!f.sourceFlare||!f.light)return false;
    auto& flare=*f.sourceFlare;auto& light=*f.light;using Shared=g::OriginalGarageObserverShared50KG;
    at(0x14bad8);if(!Shared::beginSourceAssertion(9,f.flareObserverReturnAssertion))return stop(pc);
    //The original guarded observer94 load precedes child04; do not turn NULL into an early readiness flag.
    at(0x14bb04);if(f.flareObserverReturnAssertion.enabled&&(!flare.observer94||flare.observer94!=flare.observerOwner.get()))return stop(0x14bb10);
    at(0x14bb14);if(!Shared::finishSourceAssertion(f.flareObserverReturnAssertion,
        flare.observerOwner&&flare.observerOwner->child04!=nullptr))return stop(pc);
    at(0x14bb54);f.flareObserverCallReturned=true;
    at(0x2780a4);f.flare146AE0Entered=true;
    at(0x146ae8);if(!light.fieldC4){
        at(0x146af4);light.fieldC4=&flare.entity;f.flareC4Published=true;flare.lightPublished=true;
        at(0x146afc);f.flareLightAttachLock=worldApplication_->attachmentLock423F58();
        if(!*f.flareLightAttachLock){
            const auto result=worldApplication_->entityAuthority_.attachChildren(light.entity,flare.entity,0u,*f.flareLightAttachLock);
            if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return stop(result.sourcePC);
            flare.lightAttached=true;
        } //Genuine locked141960 returnsNULL;146AE0 ignores it and keeps C4 publication.
        at(0x141a5c);f.flareLightAttachReturned=true;
    }
    at(0x146b08);f.flare146AE0Returned=true;
    at(0x2780ac);return true; //Actual flare call returned; caller owns wrapper suffix.
}
}

namespace ra_cold_aggregate50LA {
//Shared suffix is the reviewed canonical0248 implementation; no alternate ownership path.
//This driver does not fabricate any prerequisite; each return/attachment is checked from its real frame.
bool Owner::continueSourceTypedLight273BA8(std::size_t ordinal,std::uint32_t& pc)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       !sourceLightPopulationS0_||ordinal<2||ordinal>185||*sourceLightPopulationS0_!=static_cast<std::int32_t>(ordinal)||
       sourceLightPopulationItemsEnd_||sourceLightPopulationReturnV0_||!preflightSourceLightPopulationNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourceLightPopulationPC_=p;};
    while(true){
        at(0x273ba8);if(!sourceLight3A8_||*sourceLight3A8_!=nativeLightList3A8_.get())return false;
        auto* list=*sourceLight3A8_;if(!list)return false;
        at(0x273bac);sourceLightPopulationCount_=static_cast<std::int32_t>(list->count30);
        at(0x273bb0);if(*sourceLightPopulationS0_>=*sourceLightPopulationCount_)break;
        if(*sourceLightPopulationS0_<2||*sourceLightPopulationS0_>=185||list->count30!=185||
           sourceTypedLights_.size()!=static_cast<std::size_t>(*sourceLightPopulationS0_-2))return false;
        ordinal=static_cast<std::size_t>(*sourceLightPopulationS0_);
        if(!continueSourceTypedLightIteration273BA8(ordinal,pc)){sourceLightPopulationPC_=pc;return false;}
        auto& f=*sourceTypedLights_.back();
        if(f.ordinal!=ordinal||f.pc!=0x273ba0||!f.wrapperReturned||!f.wrapperV0||*f.wrapperV0!=1||
           !f.descriptorDisposed||!f.attachCallReturned||!f.attachLock||f.populationIncremented||!f.light||
           !f.wrapper||f.wrapper->sourceDescriptor04||f.wrapper->live00!=&f.light->entity)return false;
        at(0x273ba0);f.pc=pc;++*sourceLightPopulationS0_;f.populationIncremented=true;
    }
    //The authentic signed loop comparison fell through. No hardcoded completion substitution.
    if(*sourceLightPopulationS0_!=185||*sourceLightPopulationCount_!=185||sourceTypedLights_.size()!=183)return false;
    for(const auto& f:sourceTypedLights_)if(!f||!f->populationIncremented||!f->wrapperReturned||!f->descriptorDisposed||!f->attachCallReturned)return false;
    at(0x273bbc);sourceLightPopulationItemsEnd_=*sourceLightPopulationS0_;
    at(0x273bc0);*sourceLightPopulationS0_=0;
    while(true){
        at(0x273c08);if(!worldZones34C_)return false;
        at(0x273c0c);if(*sourceLightPopulationS0_>=static_cast<std::int32_t>(*worldZones34C_))break;
        const auto zone=static_cast<std::size_t>(*sourceLightPopulationS0_);
        at(0x273bc4);if(!sourceLight3A8_||*sourceLight3A8_!=nativeLightList3A8_.get()||zone>=10)return false;
        auto* list=*sourceLight3A8_;at(0x273bd4);
        if(!list||!list->colors3C||list->colorsLoaded<=zone||!list->zoneCount34||*list->zoneCount34<=zone)return false;
        at(0x273bdc);const auto& color=list->colors3C[zone];if(!color.lanes[0])return false;
        sourceLightZoneReadMask_|=std::uint16_t(1u<<zone);
        at(0x273be0); //273BE0 C.OLT.S (function0x34), not equality; unordered comparison is false.
        if(*color.lanes[0]<-255.f){
            sourceLightZoneSkippedMask_|=std::uint16_t(1u<<zone);
            at(0x273bec);++*sourceLightPopulationS0_;continue;
        }
        at(0x273bf8);if(!sourceTopologyLightManager_)return false;
        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic d;
        if(!sourceTopologyLightManager_->applySourceDirectionalColor1498A0(*this,zone,d)){
            pc=d.sourcePC;sourceLightPopulationPC_=pc;return false;}
        sourceLightZoneAppliedMask_|=std::uint16_t(1u<<zone);
        at(0x273c00);++*sourceLightPopulationS0_;
    }
    if(*sourceLightPopulationS0_!=10||*worldZones34C_!=10||sourceLightZoneReadMask_!=0x3ff||
       (sourceLightZoneSkippedMask_|sourceLightZoneAppliedMask_)!=0x3ff||
       (sourceLightZoneSkippedMask_&sourceLightZoneAppliedMask_))return false;
    at(0x273c24);sourceLightPopulationReturnV0_=1;
    at(0x273c28);sourceLightPopulationPhase_=SourceLightPopulationPhase::returned;
    return true; //Only this actual virtual273AA0 return; not World completion/renderer issuance.
}

}
