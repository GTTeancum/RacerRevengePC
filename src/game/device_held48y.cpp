#include "device_held48y.h"
#include <cstring>

namespace rr::game {
static_assert(sizeof(DeviceButtonRecord48Y)==24,"Original record must remain24 bytes");
static_assert(sizeof(DeviceButtonTransition48Y)==20,"Original event must remain20 bytes");

DeviceButtonDescriptors48Y initialDualShockDescriptors48Y() noexcept {
    DeviceButtonDescriptors48Y result{};
    constexpr DeviceButtonDescriptor48Y configured[16]={
        {3,12,0x0010},{3,13,0x0020},{3,14,0x0040},{3,15,0x0080},
        {3,18,0x0001},{3,19,0x0002},{3,16,0x0004},{3,17,0x0008},
        {1,0,0x0100},{1,0,0x0800},{1,0,0x0400},{1,0,0x0200},
        {3,10,0x1000},{3,8,0x2000},{3,11,0x4000},{3,9,0x8000}};
    for(unsigned i=0;i<16;++i)result.records[i]=configured[i];
    return result;
}

bool permitsDeviceQuery48Y(DeviceQueryContext48Y context,std::uint32_t mask) noexcept {
    if(!context.present)return mask==0u||mask==0x80000000u||mask==0x3fu;
    const bool rootBit=(context.rootFlags228&4u)!=0;
    if(!rootBit&&(mask&0x80u))return false;
    if(mask==0x3fu)return true;
    if(rootBit&&mask==0x80u&&context.context518==1u)return true;
    if(mask==0x80000000u)return context.context1898==0u;
    if(mask&0x40u)return (context.context1898|0x40u)==mask;
    return (context.context1898&mask)!=0u;
}

bool queryDeviceHeld48Y(const DeviceButtonBank48Y& bank,std::int32_t code,std::uint32_t mask,
                       DeviceQueryContext48Y context,bool& matched,DeviceButtonRecord48Y* copyOut) noexcept {
    if(!permitsDeviceQuery48Y(context,mask)){matched=false;return true;}
    if(code<0||code>=32)return false;
    DeviceButtonRecord48Y record;
    std::memcpy(&record,&bank.records[static_cast<unsigned>(code)],sizeof record);
    const bool hit=record.held==1u;
    if(hit&&copyOut)std::memcpy(copyOut,&record,sizeof record);
    matched=hit;return true;
}

PlayerActionResult48X queryHeldPlayerAction48Y(void* opaque,std::int32_t code,std::uint32_t mask) noexcept {
    if(!opaque)return PlayerActionResult48X::Unavailable;
    const auto& provider=*static_cast<const DeviceHeldProvider48Y*>(opaque);
    if(!provider.buttons)return PlayerActionResult48X::Unavailable;
    bool matched=false;
    if(!queryDeviceHeld48Y(*provider.buttons,code,mask,provider.queryContext,matched))return PlayerActionResult48X::Unavailable;
    return matched?PlayerActionResult48X::Match:PlayerActionResult48X::NoMatch;
}

bool sampleDeviceButtons48Y(bool ready,bool readSucceeded,const DevicePacket48Y& packet,
                            const DeviceButtonDescriptors48Y& descriptors,std::uint32_t count,
                            std::uint32_t deviceId,const DeviceButtonBank48Y& previous,
                            DeviceButtonSample48Y& out) noexcept {
    if(count>32)return false;
    DeviceButtonSample48Y result{};std::memcpy(&result.buttons,&previous,sizeof previous);
    result.status=DevicePacketStatus48Y::NotReady;
    if(!ready){std::memcpy(&out,&result,sizeof result);return true;}
    for(unsigned i=0;i<count;++i){result.buttons.records[i].held=0;result.buttons.records[i].value=0.0f;}
    result.status=DevicePacketStatus48Y::ReadFailed;
    if(!readSucceeded||packet.bytes[0]!=0){std::memcpy(&out,&result,sizeof result);return true;}
    const std::uint16_t active=static_cast<std::uint16_t>(((std::uint32_t(packet.bytes[2])<<8)|packet.bytes[3])^0xffffu);
    for(unsigned i=0;i<count;++i) {
        const auto& descriptor=descriptors.records[i];auto& record=result.buttons.records[i];
        if(descriptor.flags&2u) {
            if(descriptor.packetOffset>=32)return false;
            const unsigned value=packet.bytes[descriptor.packetOffset];
            if(value>=26u) {
                volatile float converted=static_cast<float>(value+1u);
                volatile float pressure=(1.0f/256.0f)*converted;
                record.value=pressure;record.held=1;
            }
        } else if(active&descriptor.digitalMask) {record.held=1;record.value=1.0f;}
        else record.value=0.0f;
    }
    for(unsigned i=0;i<count;++i) {
        auto& record=result.buttons.records[i];
        if(record.held==previous.records[i].held)continue;
        if(record.held==0u){record.auxiliary10=0;record.auxiliary08=0;}
        else {record.auxiliary14=0;record.auxiliary0C=0;}
        result.transitions[result.transitionCount++]={i,2u,deviceId,record.held,0x3fu};
    }
    result.status=DevicePacketStatus48Y::Accepted;
    std::memcpy(&out,&result,sizeof result);return true;
}

bool stepReadyHeldPlayerControls48Y(const DevicePacket48Y& packet,const DeviceButtonDescriptors48Y& descriptors,
                                   std::uint32_t count,std::uint32_t deviceId,const DeviceButtonBank48Y& previousButtons,
                                   DeviceQueryContext48Y queryContext,const PlayerReadContext48X& playerContext,
                                   const PlayerActionCodes48X& codes,float previousD28,const ControlChannels48B& previousFiltered,
                                   ControlRateLimits48B rates,float elapsedFactor,const ControlBindingInput48W& binding,
                                   const ControlBindingParameters48W& parameters,const ControlModelBindings48W& previousBindings,
                                   HeldPlayerStep48Y& out) noexcept {
    if(packet.bytes[0]!=0)return false;
    HeldPlayerStep48Y result{};
    if(!sampleDeviceButtons48Y(true,true,packet,descriptors,count,deviceId,previousButtons,result.device))return false;
    DeviceHeldProvider48Y provider{&result.device.buttons,queryContext};
    const DualShockAxisBytes48X axes{packet.bytes[4],packet.bytes[5],packet.bytes[6],packet.bytes[7]};
    if(!stepDualShockPlayerControls48X(axes,playerContext,codes,{&provider,queryHeldPlayerAction48Y},previousD28,
                                     previousFiltered,rates,elapsedFactor,binding,parameters,previousBindings,result.player))return false;
    out=result;return true;
}
}
