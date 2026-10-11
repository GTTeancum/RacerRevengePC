#pragma once
#include <cstdint>
namespace rr::game {
struct ImpactTransitionInput50C {
    float damage7D0[4]; // Ascending7D0,7D4,7D8,7DC, actual canonical damage.
    float impactBA8, threshold50; // BA0.Z and actual vehicle50, not defaults.
    // Only source B9D bit0 is consumed. A canonical bit-only owner may supply
    // 0/1 here as its representative; this projection does not own a raw byte.
    std::uint32_t flagsB9D;
};
enum class ImpactTransitionStatus50C : std::uint32_t { noTransition, unsupportedTransition, arithmetic };
struct ImpactTransitionResult50C {
    ImpactTransitionStatus50C status;
    std::uint32_t sourcePC, vtableSlot, originalTarget;
};
// Native original225C50 decision order. noTransition covers the authentic full
// no-call return225EC8->226038->22618C. No gameplay writes occur on this route.
// unsupportedTransition stops at the FIRST actual virtual call, before genuine
// destructive gameplay handlers2261B0/226750/227370/227A40. Diagnostics are NOT
// deferred executable events. Caller must reject/stop, never clear/force gates.
// The caller owns the original outer manager58&2 and vehicle40/3C dispatch.
// Finite normal/signed-zero consumed scalars, FE_TONEAREST. Unconsumed words
// remain unread, including exceptional BA8/50 behind actual low-damage gates.
// No mutation, allocation, pointers to borrowed state, or output aliasing.
ImpactTransitionResult50C inspectImpactTransition50C(const ImpactTransitionInput50C&) noexcept;
}
