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
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

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

// 212ABC: the original keeps one pair of exclusion lists per racer (Q1):
// each piece's E0 list takes that racer's own 694 identity, and that racer's
// 690 list takes each piece's E4 identity. This is the ONE layout of the
// port's per-racer 690 lane. Each lane is a fixed array sized at load, once,
// from the racer's live 690 list (read inside that racer's own Bank visit)
// plus that racer's authored piece count; it never grows afterwards. An
// append that would not fit is refused before the write. Lanes are host
// memory owned here, never guest memory and never another owner's list.
class RacerExclusionLanes50PL {
public:
    static constexpr std::size_t racers=8u;
    struct Lane {
        bool sized=false;
        std::uint32_t own694=0;        //This racer's 694 identity, as read in its own visit.
        std::uint32_t own690=0;        //This racer's 690 identity, as read in its own visit.
        std::size_t base=0,capacity=0,count=0,pieces=0;
        std::unique_ptr<std::uint32_t[]> values; //[capacity], first `base` = the live 690 list.
    };
    // Called once per slot, inside that slot's Bank visit, before its 212ABC.
    bool size(std::size_t slot,std::uint32_t own690,std::uint32_t own694,
        const std::uint32_t* base,std::size_t baseCount,std::size_t pieces)noexcept {
        if(slot>=racers||lanes_[slot].sized||!own690||!own694||own690==own694||
           (baseCount&&!base)||baseCount>64u||!pieces||pieces>64u)return false;
        auto& l=lanes_[slot];
        try{l.values=std::make_unique<std::uint32_t[]>(baseCount+pieces);}catch(...){return false;}
        for(std::size_t i=0;i<baseCount;++i)l.values[i]=base[i];
        l.own690=own690;l.own694=own694;l.base=l.count=baseCount;l.pieces=pieces;
        l.capacity=baseCount+pieces;l.sized=true;return true;
    }
    // 123EE4: an identity already present is the original duplicate branch
    // (no store). Otherwise the write must fit the lane's fixed capacity.
    bool append690(std::size_t slot,std::uint32_t pieceE4)noexcept {
        if(slot>=racers||!pieceE4)return false;
        auto& l=lanes_[slot];if(!l.sized||l.count>l.capacity)return false;
        if(pieceE4==l.own690||pieceE4==l.own694)return false;
        for(std::size_t i=0;i<l.count;++i)if(l.values[i]==pieceE4)return true;
        if(l.count==l.capacity)return false; //Refused: would not fit.
        l.values[l.count++]=pieceE4;return true;
    }
    const Lane* lane(std::size_t slot)const noexcept{return slot<racers&&lanes_[slot].sized?&lanes_[slot]:nullptr;}
    // Number of other sized lanes that report the same 694 identity as this one.
    std::size_t shared694(std::size_t slot)const noexcept {
        const auto* l=lane(slot);if(!l)return 0;std::size_t n=0;
        for(std::size_t i=0;i<racers;++i)if(i!=slot&&lanes_[i].sized&&lanes_[i].own694==l->own694)++n;
        return n;
    }
    void reset()noexcept{for(auto& l:lanes_)l=Lane{};}
private:
    std::array<Lane,racers> lanes_{};
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
