#pragma once
#include "../../src/assets/native_asset48b.h"
#include "../../src/assets/original_ui_screen_assets50kg.h"
#include "../../src/assets/original_ui_material50kg.h"
#include <array>
#include <memory>
#include <string_view>
namespace rr::game { struct OriginalRaceF8Diagnostic50LA; }
namespace rr::platform {class VehicleWindowGl50DS;struct VehicleWindowFrame50DS;}
namespace original_main_entry50KG {
struct Application;
// A synchronous native F8 bars/icon/text adapter, NOT199500/140230 or a source FIFO.
// Only Application can construct it from its actual consumed RA/F8 owners.
// The exact supplied Frame must outlive this entire batch scope, including
// its destruction and synchronous submit, and must remain unmoved throughout.
// Access/destruction stay on the immutable creator thread. Boolean observations
// reject other threads; reference-returning vertices() and destruction terminate.
// Releasing this scope releases CPU pins, never acknowledges a source callback.
class NativeF8BarPrefix50LA final {
    friend struct Application;
    friend class rr::platform::VehicleWindowGl50DS;
    struct Impl;
    std::unique_ptr<Impl> impl_;
    NativeF8BarPrefix50LA();
    bool initializePolicies(rr::game::OriginalRaceF8Diagnostic50LA&)noexcept;
    bool servicePolicy(std::size_t,rr::game::OriginalRaceF8Diagnostic50LA&)noexcept;
    bool policiesCurrent()const noexcept;
    bool initializeSuffixImages(rr::game::OriginalRaceF8Diagnostic50LA&)noexcept;
    bool appendSuffixMaterial(std::size_t,std::uint32_t,std::uint32_t,std::uint32_t,
        rr::game::OriginalRaceF8Diagnostic50LA&)noexcept;
    bool prepareIcon(std::size_t,rr::game::OriginalRaceF8Diagnostic50LA&)noexcept;
    bool setCrossText(std::string_view,rr::game::OriginalRaceF8Diagnostic50LA&)noexcept;
    bool prepareText(std::size_t,rr::game::OriginalRaceF8Diagnostic50LA&)noexcept;
public:
    // Native triangles expand the actual source strips/sprite pairs. These
    // typed bytes never stand in for a source FIFO record or callback return.
    static constexpr std::size_t maximumSuffixQuads=82,maximumSuffixPackets=6;
    struct SuffixPacket {
        std::uint32_t first=0,count=0,image=0;
        rr::assets::OriginalUiNativeMaterial50KG policy;
    };
    struct Suffix {
        std::array<rr::assets::Vertex48B,maximumSuffixQuads*4> vertices{};
        std::array<SuffixPacket,maximumSuffixPackets> packets{};
        std::shared_ptr<const rr::assets::OriginalUiScreenAssets50KG> images;
        std::uint32_t quads=0,packetCount=0;
    };
    const Suffix& suffix()const noexcept;
    NativeF8BarPrefix50LA(const NativeF8BarPrefix50LA&)=delete;
    NativeF8BarPrefix50LA& operator=(const NativeF8BarPrefix50LA&)=delete;
    ~NativeF8BarPrefix50LA()noexcept;
    bool current(const rr::platform::VehicleWindowGl50DS&,
        const rr::platform::VehicleWindowFrame50DS&)const noexcept;
    const std::array<rr::assets::Vertex48B,16>& vertices()const noexcept;
    bool enabled()const noexcept;
    // A published batch has lowered every reached icon/text primitive.
    // True is reserved for an incomplete suffix and is never published.
    // False still does not certify original gates, child traversal or return.
    bool suffixPending()const noexcept;
};
}
