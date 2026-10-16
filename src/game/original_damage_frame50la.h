#pragma once
#include "original_damage_proximity50la.h"
#include "original_ui_text50kg.h"
namespace rr::game {
struct OriginalCameraViewportFields;
struct OriginalDamageNativeFrame50LA {
    const OriginalCameraViewportFields* viewport=nullptr;
    std::uint64_t sessionGeneration=0,cameraPublication=0;
    std::uint32_t viewIndex=0,viewVehicleState40=0;
    std::int32_t raceMode8C=0;
    std::uint8_t raceFlags1B0=0,raceEntityFlags29=0,uiFlags1E4=0,flagsB9C=0;
    float damage7D4=0,damage7DC=0,damage7D0=0,damage7D8=0;
    OriginalDamageProximityObservation50LA proximity;
};
struct OriginalDamageNativeQuad50LA {
    std::array<OriginalUiTextVertex50KG,4> vertices{};
    std::array<std::array<std::uint8_t,4>,4> packetColors{};
    float depth=0;
};
struct OriginalDamageNativePacket50LA {
    std::uint32_t first=0,count=0,material=0;
    std::int32_t leaf=-1;
};
struct OriginalDamageNativeDraw50LA {
    static constexpr std::size_t maximumQuads=32,maximumPackets=32;
    std::array<OriginalDamageNativeQuad50LA,maximumQuads> quads{};
    std::array<OriginalDamageNativePacket50LA,maximumPackets> packets{};
    std::uint32_t quadCount=0,packetCount=0;
    std::uint64_t serial=0,cameraPublication=0;
    const OriginalCameraViewportFields* viewport=nullptr;
    bool prepared=false;
};
}
