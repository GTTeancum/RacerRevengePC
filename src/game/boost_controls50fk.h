#pragma once
#include <cstdint>

namespace rr::game {
// Native player-subtype projection of2176E0..217934. These fields were outside
// the49W frame owner; retain this supplement across ticks, never reconstruct it
// from the held key or selected engine flags.
struct BoostState50FK {
    std::uint32_t vehicle64=0,vehicle68=0;
    float duration6C=0.f,speedRatio70=0.f,heat7C=0.f;
};
struct BoostParameters50FK {
    float baseCooling80,speedCooling84,heatRate88,engineDifference5C;
};
struct BoostInput50FK {
    float frameFactor,engine70,engine74,cachedSpeed34;
    std::uint8_t flagsB9C;
    std::uint32_t vehicle58,profile34,profile10FC0;
    // Genuine absence in the supplied development-player topology, not an
    // assertion that the original player constructor always lacks this owner.
    bool d40Present;
};
enum class BoostError50FK : std::uint32_t { None, Input, AudioOwner, Arithmetic };
struct BoostDiagnostic50FK { BoostError50FK error; std::uint32_t sourceStage; };

// Source228788: after217DE0 binding, before21A000 graphics. Engine70/74 and
// cachedSpeed34 are their CURRENT retained values before this frame's physics.
// Source player table3CED50.A8=1A2B70 is genuinely a return; D40-present audio
// is outside this component and rejects rather than silently skipping it.
// profile34==2 makes actual188F10(profile,1) return zero; otherwise10FC0 is
// its result. Does not require a speed threshold or force an always-on boost.
// Caller feeds result64 to BOTH49K engine selection and49Z periodic damage.
// Fixed storage, no allocation, FE_TONEAREST/finite binary32/no contraction.
// Failure leaves output unchanged. Input/parameters/diagnostic must not alias
// state/output; previous and output may alias.
bool advancePlayerBoost50FK(const BoostInput50FK&,const BoostParameters50FK&,
    const BoostState50FK& previous,BoostState50FK& output,BoostDiagnostic50FK&) noexcept;
// Numeric value projection ONLY, for a real D40-aware source owner that must
// perform217774/2177D0 between the prefix68/64 writes and the remaining heat,
// ratio/duration writes. It does not assert D40 absence or execute callbacks.
// Never substitute this projection alone for a completed2176E0 invocation.
// Existing advancePlayerBoost50FK retains its strict absent-D40 admission.
bool projectBoostThermalNumbers50FK(const BoostInput50FK&,const BoostParameters50FK&,
    const BoostState50FK& previous,BoostState50FK& output,BoostDiagnostic50FK&) noexcept;
}
