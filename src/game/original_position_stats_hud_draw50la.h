#pragma once
#include "original_ui_text50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include "../assets/original_ui_material50kg.h"
namespace rr::game {
struct OriginalCameraViewportFields;
struct OriginalPositionStatsNativeQuad50LA {
    std::array<OriginalUiTextVertex50KG,4> vertices{};
    std::array<std::array<std::uint8_t,4>,4> packetColors{};
    float depth=0;
};
struct OriginalPositionStatsNativePacket50LA {
    std::uint32_t first=0,count=0,material=0;
    std::int32_t leaf=-1;
};
struct OriginalPositionStatsNativeDraw50LA {
    static constexpr std::size_t maximumQuads=2304,maximumPackets=128;
    std::array<OriginalPositionStatsNativeQuad50LA,maximumQuads> quads{};
    std::array<OriginalPositionStatsNativePacket50LA,maximumPackets> packets{};
    std::uint32_t quadCount=0,packetCount=0;
    std::uint64_t serial=0,cameraPublication=0;
    const OriginalCameraViewportFields* viewport=nullptr;
    bool prepared=false; //CPU preparation only; never a GPU completion claim.
};
// Renderer retains only this adapter. Its canonical PositionStats resource
// borrow is weak, avoiding a cycle through the real display retirement lease.
class OriginalPositionStatsNativeOwner50LA {
public:
    virtual ~OriginalPositionStatsNativeOwner50LA()=default;
    virtual bool nativeCpuResourcesPrepared()const noexcept=0;
    virtual const assets::OriginalUiScreenAssets50KG* nativePixels()const noexcept=0;
    virtual const assets::OriginalUiMaterial50KG* nativeMaterial(std::size_t)const noexcept=0;
    virtual std::uint32_t nativeImage(std::size_t)const noexcept=0;
    virtual const assets::OriginalUiNativeUntexturedMaterial50KG* nativeRadarPolicy()const noexcept=0;
    virtual const OriginalPositionStatsNativeDraw50LA& nativeDraw()const noexcept=0;
};
}
