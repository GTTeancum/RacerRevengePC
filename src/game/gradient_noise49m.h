#pragma once
#include <cstdint>

namespace rr::game {
// Projected state of original 14EEC0 and the 1-D dependencies of 14EFD0.
// No 2-D/3-D gradients, duplicated tails, process-global storage or seed policy.
// Cold state ignores all table/cell fields. Caller supplies the shared raw32 RNG
// separately; no implicit seed is chosen. Live disjoint typed objects required.
struct GradientNoiseState49M {
    std::uint32_t permutation[256]{};
    float gradient1D[256]{};
    std::int32_t scratchCell = 0; // original 4332A8, written on every evaluation
    bool initialized = false;   // inverse of original nonzero flag382B40
    //Native admission only, NOT a guest flag or a lock. A direct projected
    //initializer sets this before clearing382B40 and leaves it nonzero on
    //ANY partial stop. All readers/copybacks must reject it; no live reset,
    //reseed, replay or table repair may clear it. Zero only after genuine
    //projected completion, or on an independently fresh owner. Serial use
    //remains caller-owned; this byte does not make concurrent access safe.
    std::uint8_t initializationIncomplete49M = 0;
};

// Exact source-ordered scalar binary32 reference, not universal EE silicon proof.
// Inherits the caller's FE_TONEAREST, FE_DOWNWARD, FE_UPWARD or FE_TOWARDZERO;
// the standalone suite proves all four. Wrapper49M separately admits nearest only.
// Cold call creates projected tables and advances sharedRawRandomState1791 times;
// ready call advances it zero times. Shared scheduling/seeding is caller-owned.
// Requires finite coordinate and rounded(4096+coordinate) in[-2^31,2^31).
// Reached ready permutation entries must be<256; reached gradients finite within
// [-1,255/256]. Readiness/table provenance beyond this admission is caller-owned.
// Native incomplete-initialization admission also rejects nonzero
// initializationIncomplete49M without reading tables, drawing RNG or retrying.
// Native invalid-input failure leaves state, RNG and output unchanged, a stronger
// failure contract than the original unchecked code. No allocation or callbacks.
bool evaluateGradientNoise49M(GradientNoiseState49M& state,
    std::uint32_t& sharedRawRandomState,float coordinate,float& output) noexcept;
}
