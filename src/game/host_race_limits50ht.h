#pragma once
#include <cstdint>
namespace rr::game {
// Shared storage limits for the HOST native race client and renderer. They do
// not establish the recovered original constructor, AI or grid layout.
inline constexpr std::uint32_t maxHostRaceRacers50HT=8;
inline constexpr std::uint32_t maxHostRaceOpponents50HT=maxHostRaceRacers50HT-1;
// Normal original field size is eight: retained SLUS 1DED10..1DED48 selects
// AI=8-human; PodRacers 1AD618..1AD628 sums human+AI. Only this field size is
// recovered here; host driver and host lateral-grid policy remain explicit.
inline constexpr std::uint32_t defaultHostRaceRacers50HT=maxHostRaceRacers50HT;
static_assert(defaultHostRaceRacers50HT>=2&&defaultHostRaceRacers50HT<=maxHostRaceRacers50HT);
}
