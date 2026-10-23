#pragma once
#include <cstdint>
#include <array>
#include <memory>
#include "../../../src/game/original_application_animation_clock50kg.h"
namespace original_main_entry50KG {struct Application;class RaceDisplayLifetime50LA;}
namespace rr::game {struct OriginalCameraStartResult;}
namespace rr::platform {
struct VehicleWindowDiagnostic50DS;
class OriginalUiSourceOwner50KM;
class OriginalUiGarageActors50GN;
class OriginalUiGarageWattoOwner50KG;
class OriginalUiGarageWattoSoundGroup50KG;
// Actual root-owned display borrow for Entity141F3C's renderer244D60(0).
// No public factory, caller-supplied dispatch, ready flag or callback exists.
// Absence is an unavailable required stage, never an inert renderer service.
class OriginalUiGarageRendererLease50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiGarageActors50GN;
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    friend class OriginalUiGarageCameraFrame50KG;
    friend class OriginalUiGarageWidget1Owner50KG;
    friend class OriginalUiGarageTextOverlay50KG;
    friend class OriginalUiGarageWattoOwner50KG;
    friend class OriginalUiGarageWattoSoundGroup50KG;
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> display_;
    const ::original_main_entry50KG::Application* issuer_=nullptr;
    std::uint64_t generation_=0;
    game::OriginalApplicationAnimationClockView50KG applicationLifetime_;
    OriginalUiGarageRendererLease50KG(
        std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA>,
        const ::original_main_entry50KG::Application*) noexcept;
    static std::shared_ptr<OriginalUiGarageRendererLease50KG> retain(
        const ::original_main_entry50KG::Application*,
        const std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA>&) noexcept;
    bool currentFor(const ::original_main_entry50KG::Application*)const noexcept;
    bool poll245030(bool&,VehicleWindowDiagnostic50DS&)noexcept;
    bool drain1404BC(VehicleWindowDiagnostic50DS&,game::OriginalCameraStartResult&)noexcept;
    bool complete245090(VehicleWindowDiagnostic50DS&,game::OriginalCameraStartResult&)noexcept;
    bool applyFogColor244A40(const OriginalUiSourceOwner50KM&,std::uint64_t,std::uint32_t,VehicleWindowDiagnostic50DS&)noexcept;
    bool clearViewport2458C0(const OriginalUiSourceOwner50KM&,std::uint64_t,bool,std::uint32_t,VehicleWindowDiagnostic50DS&)noexcept;
    bool releaseRenderPrelude(const OriginalUiSourceOwner50KM&,std::uint64_t,VehicleWindowDiagnostic50DS&)noexcept;
    bool ownsRenderPrelude(const OriginalUiSourceOwner50KM&,std::uint64_t)const noexcept;
    bool serviceEntity141F3C(VehicleWindowDiagnostic50DS&,game::OriginalCameraStartResult&)noexcept;
    // Read the SAME selected live Camera and Video, after source29CDA0 has
    // cleared its dirty words. No camera defaults, cold diagnostic projection,
    // render submission, resize or selection mutation is manufactured.
    bool textViewport1159D0(std::array<std::int32_t,4>&,
        std::array<std::int32_t,2>&,std::uint32_t&)const noexcept;
public:
    ~OriginalUiGarageRendererLease50KG()noexcept;
    OriginalUiGarageRendererLease50KG(const OriginalUiGarageRendererLease50KG&)=delete;
    OriginalUiGarageRendererLease50KG& operator=(const OriginalUiGarageRendererLease50KG&)=delete;
};
}
