#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include "native_query_identities50kx.h"

// One allocator for every racer's own query identities, on every course.
// 2129C0 builds each racer's own 690 (main), 364 (secondary spring), 348
// (terrain probe), 694 (pod) and 7A4 (movement) objects; 212984/212990/
// 21299C then append that racer's own 690, 7A4 and 348 to its 694 list. The
// port's identities are opaque HOST labels (not guest addresses): this file is
// the one place that lays them out, five per racer, starting at the racer
// namespace already used by the trigger tables. Every identity is checked by
// the caller's predicate (the course's own static and trigger owners) and
// against the fixed native constants; a conflict or exhaustion is refused.
namespace native_racer_identities50PL {
inline constexpr std::size_t maxRacers=8;
inline constexpr unsigned rolesPerRacer=5;
inline constexpr std::uint32_t firstRacerIdentity=0x30000000u;

struct RacerIdentities {
    std::uint32_t main=0,secondary=0,terrain=0,pod=0,movement=0;
};

inline bool fixedNativeIdentity(std::uint32_t id)noexcept {
    namespace k=native_query_identities50KX;
    return id==k::main690||id==k::movement7A4||id==k::secondary364||id==k::terrain348||
        id==k::followSphere24||id==k::followRay190||id==k::death120||id==k::pod694;
}

// Fills out[0..racers) in slot order (main, secondary, terrain, pod,
// movement). Nothing is written to out unless every identity was found and all
// racers*5 are distinct.
template<class Available>
bool allocate(std::size_t racers,Available&& available,RacerIdentities* out,std::size_t outCapacity)noexcept {
    if(!out||!racers||racers>maxRacers||racers>outCapacity)return false;
    std::array<RacerIdentities,maxRacers> staged{};
    std::uint32_t next=firstRacerIdentity;
    std::size_t issued=0;std::array<std::uint32_t,maxRacers*rolesPerRacer> all{};
    for(std::size_t slot=0;slot<racers;++slot){
        auto& row=staged[slot];
        std::uint32_t* targets[rolesPerRacer]={&row.main,&row.secondary,&row.terrain,&row.pod,&row.movement};
        for(auto* target:targets){
            while(next&&(fixedNativeIdentity(next)||!available(next))){
                if(next==std::numeric_limits<std::uint32_t>::max())next=0;else ++next;
            }
            if(!next)return false;
            for(std::size_t i=0;i<issued;++i)if(all[i]==next)return false;
            *target=next;all[issued++]=next;
            if(next==std::numeric_limits<std::uint32_t>::max())next=0;else ++next;
        }
    }
    for(std::size_t slot=0;slot<racers;++slot)out[slot]=staged[slot];
    return true;
}
}
