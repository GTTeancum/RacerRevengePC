#pragma once
// Synchronous borrowed view; existing world locals retain ownership and order.
// Common rewrite R10 (PLAN 2.9, B-C18/B-P04): the cold roster of EVERY race.
// It builds the one field from the launch data (188470 tournament, the Course
// screen free-play entries, Time Trial solo), hands it to the one roster
// (common_roster50CR::Owner: every slot's own pod files, humans before AI,
// wreck chain 165E90), then prepares each slot's own collision owner.
// No course, pod or pilot is named; the track is data (stock or registry row).
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
namespace common_track50CR { class Track; }
namespace common_roster50CR { class Owner; }
namespace interactive_roster_cold50DU {
// The race's per-slot rig/collision and runtime (visual, cable, model) arrays.
// Every race uses them (the type names are R3's ra_cold_roster_storage50la.h).
std::shared_ptr<ra_cold_roster50LA::SharedOwners> makeSharedOwners();
std::shared_ptr<ra_cold_roster50LA::RuntimeOwners> makeRuntimeOwners();
struct Context {
    const rr::game::Type2ResourceView49X& mainQueryResource;
    const bool& directOriginalInputs;
    const std::uint32_t& playerRacer60;
    const bool& rootedCourseRace;            // launched through the rooted original Course entry (4.2 map)
    bool sourceUiHumanCourse;
    const rr::platform::OriginalUiRiderProfileOwner50KG* courseProfile;
    const std::uint32_t& raceRacerCount;
    const common_track50CR::Track& track;    // the race's one track (data: code, stock or registry)
    common_roster50CR::Owner& roster;        // the race's one roster (built and loaded here, step 10)
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
