#pragma once
#include "../../../src/game/original_application_animation_clock50kg.h"
#include <memory>
#include <cstdint>
namespace ra_cold_aggregate50LA {class Owner;}
namespace original_main_entry50KG {struct Application;struct Video;class RaceDisplayLifetime50LA;}
namespace rr::platform {
class OriginalUiGarageManagerFog50KG;
class OriginalUiGarageSceneLighting50KG;
class OriginalUiGaragePodScene50KG;
// Private scalar-only authority for the exact13E8A8 store. It pins the actual
// Application-issued display/Video, but cannot service queues, initialize GL,
// draw, or certify a renderer. The existing244D60 lease is unchanged.
class OriginalUiGarageFogColorLease50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class ::ra_cold_aggregate50LA::Owner; //Actual conditional World13E8A8 scalar store.
    friend class OriginalUiGarageManagerFog50KG;
    friend class OriginalUiGarageSceneLighting50KG;
    friend class OriginalUiGaragePodScene50KG;
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> display_;
    std::shared_ptr<::original_main_entry50KG::Video> video_;
    const ::original_main_entry50KG::Application* issuer_=nullptr;
    game::OriginalApplicationAnimationClockView50KG applicationLifetime_;
    std::weak_ptr<OriginalUiGarageFogColorLease50KG> issuedOwner_; // Native control-block provenance, no cycle.
    std::uint64_t generation_=0;
    OriginalUiGarageFogColorLease50KG(const ::original_main_entry50KG::Application&,
        std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA>)noexcept;
    static std::shared_ptr<OriginalUiGarageFogColorLease50KG> retain(
        const ::original_main_entry50KG::Application&,
        const std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA>&)noexcept;
    bool currentFor(const ::original_main_entry50KG::Application*)const noexcept;
    bool currentOwnedBy(const std::shared_ptr<OriginalUiGarageFogColorLease50KG>&,
        const ::original_main_entry50KG::Application*)const noexcept;
    bool store13E8A8(const OriginalUiGarageManagerFog50KG&)noexcept;
    bool storeScene13E8A8(const OriginalUiGarageSceneLighting50KG&)noexcept;
public:
    ~OriginalUiGarageFogColorLease50KG()noexcept;
    OriginalUiGarageFogColorLease50KG(const OriginalUiGarageFogColorLease50KG&)=delete;
    OriginalUiGarageFogColorLease50KG& operator=(const OriginalUiGarageFogColorLease50KG&)=delete;
};
}
