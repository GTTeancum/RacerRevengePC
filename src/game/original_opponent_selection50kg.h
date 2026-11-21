#pragma once
// Authenticated SLUS_202.68 deterministic selection leaf only. No RNG adapter.
// Production caller admits the actual UI mode34==2 branch of1AE620.
#include <array>
#include <cstdint>
#include <span>
namespace rr::game {
struct CoupledNoiseContext49M;
struct SharedRandomOwner50ID;
struct HostMonotonicClock50ID;
}
namespace rr::game::original_opponent_selection50KG {
using RacerId=std::int32_t;
std::uint32_t courseIndex187B10(std::span<const std::uint8_t,13> courseOrder100C4,
    std::uint8_t course1125F) noexcept;
// Exact valid-class1..3 188470 result. Invalid-class false is a native guard,
// not the original assertion/debug path. firstHuman is source racer60, not slot.
bool deterministic188470(std::uint32_t klass,RacerId firstHuman,
    std::array<RacerId,7>& out) noexcept;

// 1AE620 non-tournament branch (app+340 "NoRandomAI" clear, mode34 != 2):
// opponent 0 is the course rival (signed byte P+11280+12*(1125F&15)) unless a
// human rides it; every other opponent is drawn by 1AEF70 from the shared
// 308030 stream (lazy seed 3D6B30 from 101A50 at the first draw, guard 80A8;
// range check 3D6B38, guard 80AC). Order of the result = manager order after
// the humans (1AE370 stores racers[humans+ordinal]).
struct FreePlayInputs1AE620 {
    std::span<const RacerId> humans;       // pod60 of racers[0..humans34-1]
    std::uint32_t opponents=0;             // manager38
    RacerId rival11280=-1;                 // <=0: no rival (PORT EXTENSION, add-on course "none")
    std::array<std::uint32_t,27> unlock10FAC{}; // P+10FAC+4r
    // PORT EXTENSION (DLC rule R4): add-on racers appended after the 26 stock
    // candidates, add-on courses only. Empty = exactly the original draw.
    std::span<const RacerId> addonCandidates;
};
struct FreePlayResult1AE620 {
    std::array<RacerId,7> opponents{};
    std::uint32_t count=0;
    std::uint32_t candidates=0;            // 26 + add-on candidates
    std::uint32_t draws=0;                 // 308030 calls made
    bool seeded=false;                     // 101A50 -> 308020 happened here
    bool rivalTaken=false;
    std::uint32_t failPC=0;                // 0 = success
};
bool freePlay1AE620(const FreePlayInputs1AE620&,CoupledNoiseContext49M&,SharedRandomOwner50ID&,
    HostMonotonicClock50ID&,FreePlayResult1AE620&) noexcept;
}
