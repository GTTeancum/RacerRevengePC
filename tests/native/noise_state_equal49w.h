#pragma once
#include "../../src/game/gradient_noise49m.h"
#include <cstring>

namespace rr::test {
// Compare every represented source field bit-for-bit. C++ tail padding is not
// source state and memberwise assignment may leave it indeterminate on MSVC.
// Also compare native incomplete-initialization admission, not a guest field.
inline bool equalNoiseState49W(const game::GradientNoiseState49M& a,
                              const game::GradientNoiseState49M& b) noexcept {
    return std::memcmp(a.permutation, b.permutation, sizeof a.permutation) == 0 &&
           std::memcmp(a.gradient1D, b.gradient1D, sizeof a.gradient1D) == 0 &&
           a.scratchCell == b.scratchCell && a.initialized == b.initialized &&
           a.initializationIncomplete49M == b.initializationIncomplete49M;
}
}
