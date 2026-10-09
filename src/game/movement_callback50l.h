#pragma once
#include "body_scene48g.h"
#include <cstdint>
namespace rr::game {
enum class MovementCallbackStatus50L { Complete,InvalidInput };
// Original20EE10. The static branch consumes no body pointers. The dynamic
// branch consumes XYZ velocities only; W is outside this view. Caller owns the
// sole vehicle7A0 word. Nearest/even ordinary finite binary32 products/sums only. A rejected
// input leaves it unchanged. No allocation, retained pointers, or guest state.
MovementCallbackStatus50L applyMovementCallback50L(std::uint32_t candidateCategory,
 std::uint32_t dynamicMask,const Vector3_48C* candidateVelocity,
 const Vector3_48C* vehicleVelocity,std::uint32_t& vehicle7A0) noexcept;
}
