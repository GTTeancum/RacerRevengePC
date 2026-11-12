#pragma once
// One pod load (2129C0) for every racer slot, player and AI, on every course.
// D:\Programming\GitHub\RacerRevengePC\work\scratch\pod-load-all\PLAN.md (3.1, 3.2).
//
// Original order (SLUS_202.68): 1A72C4 1AA510 racer manager -> 1AD520 ->
// 1AE280: humans 1ADF80 -> 1A0570/1A0848 -> 210770 -> 2129C0, then AIs
// 1AE3CC -> 16E450 -> 16E5F4 210770 -> 2129C0. The camera binding 1750A0
// (1750F8 286D20 death target) follows the whole manager.
//
// This header defines the ONLY track-side input interface the pod load reads.
// Courses differ only in the data behind it, never in which code runs:
//  - zones: the track's 4C0[0] visibility and 4C4[0] collision search nodes,
//    borrowed once for all racers (1AE0C8/1AE488 pass the same rows, 1AE4B0);
//  - water: the track's own Race70 water object (22C21C/210804), or none;
//  - particles: the counted 4A0 owner the racer's 210890 already used.
// No course name, track id or per-pod key appears in this interface.
#include <cstddef>
#include <cstdint>

namespace ra_source_camera50LA { class OwnershipBridge; }
namespace rr::game {
struct OriginalWreckSetupZones50LA;
class OriginalTrackParticleCold50LA;
}

namespace original_pod_load50PL {
using ZoneRef50PL=std::uint32_t; //Native SearchNode registry ref (ordinal48+1), not a guest pointer.

class TrackDataProvider50PL {
public:
    virtual ~TrackDataProvider50PL()=default;
    // 4C0[0]/4C4[0] for the whole roster. One borrower at a time; the owner
    // refuses to close its track while the borrow is held.
    virtual bool retainWreckZones(ra_source_camera50LA::OwnershipBridge& borrower,std::uint64_t generation,
        rr::game::OriginalWreckSetupZones50LA& zones,ZoneRef50PL& visibility,ZoneRef50PL& collision)noexcept=0;
    virtual bool releaseWreckZones(ra_source_camera50LA::OwnershipBridge& borrower)noexcept=0;
    // 1AA57C World4A0: the same counted particle owner the racer already used.
    virtual bool retainWreckParticles(ra_source_camera50LA::OwnershipBridge& borrower,
        rr::game::OriginalTrackParticleCold50LA* actual4A0)noexcept=0;
    // 210804/22C21C: this track's Race70 water object, or nullptr for NONE.
    virtual const void* water3FC()const noexcept=0;
};

// 1AE280 manager order: every human slot (1ADF80) before every AI (1AE3CC).
// The port seats humans in the lowest slots; the order is recomputed from
// the actual counts on every call and refused if they are inconsistent.
inline bool managerOrderSlot50PL(std::size_t ordinal,std::size_t humans,std::size_t racers,
    std::size_t& slot)noexcept {
    if(racers>8u||humans>racers||ordinal>=racers)return false;
    slot=ordinal;return true;
}
}
