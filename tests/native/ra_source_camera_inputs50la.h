#pragma once
#include "original_camera_session50kx.h"
#include "native_racer_cables50hw_fixture.h"

namespace ra_source_camera50LA {
// Actual stable runtime borrows. The aggregate supplies light/world/Follow,
// height, Application material/Entity owners and its single track-particle
// manager privately. No alternate-world fields or fake readiness input.
struct CameraCallerOwners {
    native_racer50HN::Bank& bank;
    rr::game::VehicleFrameState49W& arena;
    rr::game::VehicleFrameBindings49W& bindings;
    rr::game::BaseTailState50X& baseTail;
    rr::game::AiRuntimeState50IV& ai;
    rr::game::LapTimingFields50ID& racerTiming;
    rr::game::LapTimingManager50ID& all8Manager;
    race_session50HM::Session& hostRace;
    rr::game::SharedRandomOwner50ID& sharedRandom;
    rr::game::OriginalAttachedGlobals& attachedGlobals;
    rr::game::HostMonotonicClock50ID& applicationClock;
    rr::game::ControlPresets49D& preferences;
    const std::uint32_t& launchKind11258;
    const std::uint32_t& configuration34;
    const std::int32_t& application350;
    rr::game::GameplayBlurPending& blur;
    rr::game::BasePoseHistoryState50Y& poseHistory;
    const rr::game::BoostState50FK& boost;
    original_camera_cycle_audio50KX::Owner& cycleAudio;
    const std::array<std::uint32_t,8>& racer60;
    const std::uint32_t& humanCount34;
    float& race94;
    const rr::game::ManagerHeldSources49A& humanHeld;
    const std::array<std::unique_ptr<native_racer50HW::Rig>,8>& actualRigs;
    const std::array<std::unique_ptr<native_racer50HW::Cables>,8>& actualCables;
    const std::array<std::unique_ptr<native_racer_model50LA::Owner>,8>& actualRightModels;
    const std::array<std::unique_ptr<native_racer_model50LA::Owner>,8>& actualLeftModels;
    const std::array<std::unique_ptr<native_racer_model50LA::Owner>,8>& actualChariotModels;
    const std::array<std::unique_ptr<native_racer_model50LA::Owner>,8>* actualBottomRightModels=nullptr;
    const std::array<std::unique_ptr<native_racer_model50LA::Owner>,8>* actualBottomLeftModels=nullptr;
    rr::game::OriginalRaceHud50KG* raceHud=nullptr;
    const rr::game::OriginalDeathCameraDebrisView50KG* deathDebris=nullptr;
};
// Same runtime services, without caller-supplied roster arrays. The retained
// roster handoff obtains model/rig/cable ownership from the actual Bank pin.
struct CameraRuntimeOwners {
    native_racer50HN::Bank& bank;
    rr::game::VehicleFrameState49W& arena;
    rr::game::VehicleFrameBindings49W& bindings;
    rr::game::BaseTailState50X& baseTail;
    rr::game::AiRuntimeState50IV& ai;
    rr::game::LapTimingFields50ID& racerTiming;
    rr::game::LapTimingManager50ID& all8Manager;
    race_session50HM::Session& hostRace;
    rr::game::SharedRandomOwner50ID& sharedRandom;
    rr::game::OriginalAttachedGlobals& attachedGlobals;
    rr::game::HostMonotonicClock50ID& applicationClock;
    rr::game::ControlPresets49D& preferences;
    const std::uint32_t& launchKind11258;
    const std::uint32_t& configuration34;
    const std::int32_t& application350;
    rr::game::GameplayBlurPending& blur;
    rr::game::BasePoseHistoryState50Y& poseHistory;
    const rr::game::BoostState50FK& boost;
    original_camera_cycle_audio50KX::Owner& cycleAudio;
    const std::array<std::uint32_t,8>& racer60;
    const std::uint32_t& humanCount34;
    float& race94;
    const rr::game::ManagerHeldSources49A& humanHeld;
    rr::game::OriginalRaceHud50KG* raceHud=nullptr;
    const rr::game::OriginalDeathCameraDebrisView50KG* deathDebris=nullptr;
};
} // namespace ra_source_camera50LA
