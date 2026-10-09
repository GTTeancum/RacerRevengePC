#pragma once
#include "body_scene48g.h"
#include <cstdint>

namespace rr::game {
// Owned ordinary223030 state. Index order is original D08,D0C,...,D24.
// BA0.W is copy-only and may retain any bit pattern.
struct StaticImpactState49Y { float directionalD08[8]; float impactBA0[4]; };
struct StaticImpactTuning49Y { float thresholds00[5]; float severities14[5]; };
struct StaticImpactInput49Y {
    std::uint32_t vehicle40, vehicle58, bodyContactPartnerCount20C;
    std::uint8_t flagsCD8, sceneFlags190;
    BodyMatrix48G readyMainScene190;
    float accumulatedImpact1C0[4];
    float reductionCF0;
    StaticImpactTuning49Y tuning;
    float priorVf9W;
};
enum class StaticImpactStatus49Y { applied, skipped, unsupported, arithmetic };
// Complete reached223030 ->28BBB0 ->222770 numerical projection for an ordinary
// vehicle58=0, body20C=0 and already-ready scene190. Input1C0 is not cleared.
// The caller owns real CF0, tuning and empty-partner-list provenance; these are
// never defaulted from static candidate category. Source gate CD8&1 or40=3/4
// returns skipped before consuming any scene/body/tuning data.
// Consumed operands/intermediates must be normal binary32 or signed zero;
// subnormal/nonfinite arithmetic is rejected, including a subnormal squared
// length before scalar SQRT or VU RSQRT. This avoids claiming host gradual
// underflow equals EE/VU flush behavior. Exact-zero normalization uses original
// positive maximum-finite Q and produces zero XYZ, not host division by zero.
// Requires FE_TONEAREST, no fast math/contraction; IEEE ordinary rounding only,
// not a proof of physical EE extended-float accuracy or status flags.
// Ready getter scratch operands used by self-subtraction must be finite.
// No allocations. Output and scratchW are unchanged on unsupported/arithmetic;
// skipped copies state and priorVf9W without interpreting copy-only words.
// Output may alias state; otherwise descriptors/storage must not overlap.
// This is a caller precondition, not a runtime pointer-overlap check. In
// particular nextVf9W must not alias input, state, or output storage.
// Later2228D0/222B20 consumers, dynamic partners, special vehicle58 routes and
// scene refresh are separate dependencies, not silently executed or omitted.
StaticImpactStatus49Y recordStaticImpact49Y(const StaticImpactInput49Y&,
    const StaticImpactState49Y&, StaticImpactState49Y& output,
    float& nextVf9W) noexcept;
}
