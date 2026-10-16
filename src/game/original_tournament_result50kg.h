#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Native typed storage for17FFB0/1875C0. No guest memory, arbitrary unlock
// override, file IO, or result-screen/save dispatch is represented here.
struct OriginalTournamentPodCareer50KG {
    std::array<float,6> abilities1920{};
    std::array<std::uint8_t,13> medals1938{};
    std::array<std::int16_t,13> remainingPrizes1946{};
};
struct OriginalTournamentCareer50KG {
    std::array<OriginalTournamentPodCareer50KG,27> pods{};
    std::uint32_t earned2190=0,unspent2194=0,kills2198=0;
    std::uint8_t progress219C=0;
};
struct OriginalTournamentProfile50KG {
    std::array<OriginalTournamentCareer50KG,27> careers{};
    std::array<std::array<float,6>,27> base0180{},maximum0408{};
    std::uint32_t tournamentBoundary102CC=0;
};
// Exact original constructor17EEE8..17F6EC, in course ORDER, not track ID.
inline constexpr std::array<std::uint8_t,13> originalTournamentCourseOrder50KG{{0,11,4,8,7,1,5,9,12,6,10,3,2}};
inline constexpr std::array<std::int16_t,13> originalTournamentPrizes50KG{{2200,2400,2700,3700,3200,3400,3700,4700,4400,4700,4900,5200,6000}};
// Actual1AF330(managerB0,i) observations, in ORIGINAL manager order.
struct OriginalTournamentResultRacer50KG {
    std::uint32_t pod60=0,class58=0,retirementCD4=0;
    std::int32_t placeCCC=0,killsCD0=0;
};
enum class OriginalTournamentResultStatus50KG:std::uint8_t {complete,owner,numeric};
struct OriginalTournamentResultReceipt50KG {
    OriginalTournamentResultStatus50KG status=OriginalTournamentResultStatus50KG::owner;
    std::uint32_t sourcePC=0,courseOrder=0,prize=0,killBonus=0,randomDraws=0;
    bool retired=false,priorMedalSkipped=false,careerAdvanced=false,availabilityAdvanced=false;
};
// Source1875C0 then187D80, INCLUDING187900/187A20 and native1AF4B0 stat
// growth with the application's SAME308030 raw RNG. Unsupported input/numeric
// admission leaves profile/availability/RNG unchanged. Receipt covers only these
// owners; it never claims1AB520 routing, save IO, or a retail Results page.
// The application profile's private commit must validate real mode2 launch,
// retained profile generation and genuine ended manager before reaching here.
OriginalTournamentResultReceipt50KG applyOriginalTournamentResult50KG(
    OriginalTournamentProfile50KG&,std::uint32_t career100C0,
    std::uint8_t selected1125F,const OriginalTournamentResultRacer50KG*,
    std::size_t managerCount30,std::uint32_t humanCount34,
    std::uint8_t& availability11018,std::uint32_t& sharedRawRandom308030) noexcept;
}
