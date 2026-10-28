#pragma once
// Synchronous borrowed view; existing world locals retain ownership and order.
#include "native_borrowed_call50du.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include "../../src/game/host_race_limits50ht.h"
#include <array>
#include <string>
namespace rr::game { struct Type2ResourceView49X; }
namespace rr::platform { class OriginalUiRiderProfileOwner50KG; }
namespace native_racer50HW { class Rig; }
namespace native_racer_collision50HW { class Owner; struct EndpointPolicy; }
namespace ra_cold_roster50LA { struct SharedOwners; struct RuntimeOwners; }
namespace interactive_roster_cold50DU {
std::shared_ptr<ra_cold_roster50LA::SharedOwners> makeSharedOwners();
std::shared_ptr<ra_cold_roster50LA::RuntimeOwners> makeRuntimeOwners();
struct Context {
    const rr::game::Type2ResourceView49X& mainQueryResource;
    const bool& directOriginalInputs;
    const std::uint32_t& playerRacer60;
    const bool& courseRaceCold;
    bool sourceUiHumanCourse;
    const rr::platform::OriginalUiRiderProfileOwner50KG* courseProfile;
    const std::string& playerPodId;
    const std::uint32_t& raceRacerCount;
    const bool& playerUsesRosterRig;
    std::array<std::unique_ptr<native_racer50HW::Rig>,8>& racerRigs;
    std::array<std::unique_ptr<native_racer_collision50HW::Owner>,8>& racerCollisions;
    const native_racer_collision50HW::EndpointPolicy& endpointPolicy;
    std::array<std::string,rr::game::defaultHostRaceRacers50HT>& racerNames;
    std::size_t& mainPairCapacity;
    std::size_t& secondaryPairCapacity;
    interactive_frame50DU::BorrowedCall<bool(const rr::game::Type2ResourceView49X&)> matchesEndpointPolicy;
    void run() const;
};
}
