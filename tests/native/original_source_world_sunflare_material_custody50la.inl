#pragma once
#include "../../src/game/original_parameter_packet_history50la.h"
namespace ra_pod_scene50LA {
inline bool Owner::canRetireSourceSunFlareMaterialNative(const NativeSunFlareMaterial& m)noexcept {
    if(!m.sourceMode)return !m.primitive04&&!m.parameters08;
    if(m.sourceNativeClosed)return !m.primitive04&&!m.parameters08&&!m.sourcePrimitive04&&!m.sourceParameters08&&!m.sourceTable&&!m.sourceApplication&&!m.sourceCallback04&&!m.sourceCallbackOwner;
    if(!m.sourceTable||!m.sourceApplication)return false;
    if(m.sourceParametersPublished&&(!m.sourceParametersAttempted||!m.sourceParameters08||*m.sourceParameters08!=m.parameters08.get()))return false;
    if(m.sourcePrimitivePublished&&(!m.sourcePrimitiveAttempted||!m.sourcePrimitive04||*m.sourcePrimitive04!=m.primitive04.get()))return false;
    if(m.parameters08){const auto& p=*m.parameters08;
        if(!p.sourceMode||p.sourceKnownMask!=15)return false;
        if(!p.sourcePacketAttempted){
            if(p.packet||p.nativePayload04||p.buffer20||p.packetStarted||p.packetComplete||
               p.packetKind!=NativeSunFlareParameters::PacketKind::none||p.sourceBufferPublished||p.sourceBuffer20||p.sourceHeaderKnownMask)return false;
            for(const auto& word:p.unknown00To1C)if(word)return false;
        }else {
            if(!rr::game::OriginalParameterPacketHistory50LA::canWithdrawNative(p.sourcePacketHistorySerial)||!p.packetStarted||(p.packetKind!=NativeSunFlareParameters::PacketKind::sprite&&p.packetKind!=NativeSunFlareParameters::PacketKind::retrieve)||p.field2C!=1||
               (p.sourceHeaderKnownMask&~255u)||p.count24!=128||p.count28>128)return false;
            if(p.sourceBufferPublished){
                if(!p.sourceBuffer20||*p.sourceBuffer20!=p.buffer20||
                   p.buffer20!=(p.packet?p.packet->words.data():nullptr))return false;
            }else if(p.packet||p.buffer20||p.sourceBuffer20)return false;
            if(p.nativePayload04&&(!p.packetComplete||p.nativePayload04!=p.buffer20))return false;
            if(p.packetComplete&&(!p.packet||p.count28!=(p.packetKind==NativeSunFlareParameters::PacketKind::sprite?41u:1u)||p.nativePayload04!=p.buffer20))return false;
            for(unsigned i=0;i<8;++i)if(bool(p.unknown00To1C[i])!=bool(p.sourceHeaderKnownMask&(1u<<i)))return false;
            if(!p.packet){for(bool written:p.sourcePacketWordKnown)if(written)return false;}
            else if(p.packetComplete){for(unsigned i=0;i<(p.packetKind==NativeSunFlareParameters::PacketKind::sprite?44u:4u);++i)if(!p.sourcePacketWordKnown[i])return false;}
        }
    }
    if(m.primitive04){const auto& p=*m.primitive04;
        if(!p.sourceMode||p.sourceKnownMask!=0x7fffu||!p.sourceVtable08||*p.sourceVtable08!=0x3d20c0||p.optional14)return false;
        if(m.sourceCallbackAttempted){
            if(!m.sourceCallbackOwner||!m.sourceCallbackPublished||!m.sourceCallback04||*m.sourceCallback04!=p.optional04.get())return false;
            if(p.optional04){const auto& callback=*p.optional04;
                if(!callback.sourceMode||(callback.sourceKnownMask&~3u)||
                   ((callback.sourceKnownMask&1u)&&callback.sourceFunction!=0x2c5610)||
                   ((callback.sourceKnownMask&2u)&&callback.self!=m.sourceCallbackOwner))return false;
            }
        }else if(p.optional04||m.sourceCallback04||m.sourceCallbackOwner||m.sourceCallbackPublished)return false;
        if(!p.sourceParameters10||*p.sourceParameters10!=p.parameters10||
           (p.parameters10&&p.parameters10!=m.parameters08.get()))return false;
        if(p.stored){rr::assets::OriginalStandardMaterialRowView50LA row;
            if(p.table!=m.sourceTable||p.material00<0||!p.table->row(p.material00,row)||!row.name00||row.name00!=p.sourceRowName||!row.references08)return false;
            if(p.retained&&(*row.references08<=0||!p.table->canRelease14D500(p.material00)))return false;
        }else if(p.retained||p.table||p.sourceRowName)return false;
    }
    return true;
}
inline bool Owner::retireSourceSunFlareMaterialNative(NativeSunFlareMaterial& m)noexcept {
    if(!canRetireSourceSunFlareMaterialNative(m))return false;
    if(!m.sourceMode||m.sourceNativeClosed)return true;
    if(m.parameters08&&m.parameters08->sourcePacketAttempted&&!m.parameters08->sourcePacketHistoryWithdrawn){
        if(!rr::game::OriginalParameterPacketHistory50LA::withdrawNative(m.parameters08->sourcePacketHistorySerial))return false;
        m.parameters08->sourcePacketHistoryWithdrawn=true;
    }
    if(m.primitive04&&m.primitive04->retained){auto& p=*m.primitive04;
        if(!p.table->release14D500(p.material00))return false;
        p.retained=false;p.sourceReferenceReleased=true;
    }
    //No source245D10 claim: withdraw native aliases before disposing only
    //this unsubmitted primitive/parameter backing. Release is retry-ledgered
    //before any enclosing Entity authority or count can be withdrawn.
    if(m.parameters08&&m.parameters08->sourcePacketAttempted){
        auto& p=*m.parameters08;m.sourcePacketRetiredSerial=p.sourcePacketHistorySerial;m.sourcePacketRetiredCount=p.count28;
        m.sourcePacketRetiredCapacity=p.count24;m.sourcePacketRetiredHeaderMask=p.sourceHeaderKnownMask;
        p.sourceBuffer20.reset();p.nativePayload04=nullptr;p.buffer20=nullptr;
        p.packet.reset();m.sourcePacketNativeWithdrawn=true;
    }
    m.sourcePrimitive04.reset();m.sourceParameters08.reset();
    m.sourceCallback04.reset();m.sourceCallbackOwner=nullptr;
    m.primitive04.reset();m.parameters08.reset();
    m.sourceTable.reset();m.sourceApplication.reset();m.sourceNativeClosed=true;
    return true;
}
}
