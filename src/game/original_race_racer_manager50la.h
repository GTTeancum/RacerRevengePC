#pragma once
#include <cstdint>
#include <memory>
namespace native_racer50HN { class Bank; }
namespace original_race_hud_scope50LA { class Registration; }
namespace rr::game {
class OriginalApplicationEntities50LA;
class OriginalRaceConstructor50LA;
struct VehicleFrameState49W;
struct AiRuntimeState50IV;
}
namespace common_track50CR { class NavigationView; }
namespace rr::game {
// Counted1AD440 owner for the race's one native roster (every course). The native
// Bank replaces source44/count/indexed reads explicitly; it does not certify
//1AD520, Player construction, guest arrays, source40, or full Race load/attach.
// The independent subtype3 record manager must not be installed on this route.
class OriginalRaceRacerManager50LA final {
    friend class OriginalRaceConstructor50LA;
    friend class ::original_race_hud_scope50LA::Registration;
    struct Impl;
    std::unique_ptr<Impl> impl_;
    OriginalRaceRacerManager50LA(std::shared_ptr<OriginalApplicationEntities50LA>,
        std::uint64_t,native_racer50HN::Bank&,VehicleFrameState49W&,
        AiRuntimeState50IV&,const std::uint32_t&);
    bool constructed()const noexcept;
    bool associateNativeRoster(OriginalRaceConstructor50LA&,native_racer50HN::Bank&,
        VehicleFrameState49W&,AiRuntimeState50IV&,const std::uint32_t&,
        std::uint32_t&)noexcept;
    // One source invocation only. False includes the intentional placement
    // boundary AFTER real AI scalar writes and owned grid output. Never
    // reports1AEA70 returned or physical racer reset completed.
    bool continueRestart1AEA70(OriginalRaceConstructor50LA&,native_racer50HN::Bank&,
        VehicleFrameState49W&,AiRuntimeState50IV&,const std::uint32_t&,
        const ::common_track50CR::NavigationView& actualMap,std::uint32_t actualA2,std::uint32_t&)noexcept;
    bool retireNativeDetached(std::uint32_t&)noexcept;
public:
    OriginalRaceRacerManager50LA(const OriginalRaceRacerManager50LA&)=delete;
    OriginalRaceRacerManager50LA& operator=(const OriginalRaceRacerManager50LA&)=delete;
    ~OriginalRaceRacerManager50LA()noexcept;
};
}
