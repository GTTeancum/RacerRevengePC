#include "controller_ingress50cx.h"
#include <cmath>
#include <cstring>

namespace rr::game {
ControllerIngress50CX::ControllerIngress50CX(const DeviceButtonBank48Y& initialButtons) noexcept {
    std::memcpy(&sample_.buttons,&initialButtons,sizeof initialButtons);
    sample_.status=DevicePacketStatus48Y::NotReady;
    provider_.buttons=&sample_.buttons;
}

bool ControllerIngress50CX::samplePacket(bool ready,bool readSucceeded,
    const DevicePacket48Y& packet,const DeviceButtonDescriptors48Y& descriptors,
    std::uint32_t activeCount,std::uint32_t deviceId) noexcept {
    //48Y explicitly supports its previous bank aliasing its output sample.
    if(!sampleDeviceButtons48Y(ready,readSucceeded,packet,descriptors,activeCount,
                              deviceId,sample_.buttons,sample_))return false;
    if(sample_.status==DevicePacketStatus48Y::Accepted)
        packet_={packet.bytes[4],packet.bytes[5],packet.bytes[6],packet.bytes[7]};
    return true;
}

bool ControllerIngress50CX::prepareFrameReader(DeviceQueryContext48Y queryContext,
    const PlayerReadContext48X& context,const PlayerActionCodes48X& codes,
    float preparedFactor,FrameReader49K& out) noexcept {
    if(sample_.status!=DevicePacketStatus48Y::Accepted||
       !std::isfinite(preparedFactor)||preparedFactor<0.0f||preparedFactor>0.06f)return false;
    // The callback points into this nonmovable owner, never this local result.
    const FrameReader49K reader{packet_,context,codes,
        {&provider_,queryHeldPlayerAction48Y},preparedFactor};
    provider_.queryContext=queryContext;
    out=reader;
    return true;
}
}
