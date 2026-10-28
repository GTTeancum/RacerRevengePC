#pragma once
// Authenticated SLUS_202.68 deterministic selection leaf only. No RNG adapter.
// Production caller admits the actual UI mode34==2 branch of1AE620.
#include <array>
#include <cstdint>
#include <span>
namespace rr::game::original_opponent_selection50KG {
using RacerId=std::int32_t;
std::uint32_t courseIndex187B10(std::span<const std::uint8_t,13> courseOrder100C4,
    std::uint8_t course1125F) noexcept;
// Exact valid-class1..3 188470 result. Invalid-class false is a native guard,
// not the original assertion/debug path. firstHuman is source racer60, not slot.
bool deterministic188470(std::uint32_t klass,RacerId firstHuman,
    std::array<RacerId,7>& out) noexcept;
}
