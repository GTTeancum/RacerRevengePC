#pragma once
#include "body_scene48g.h"
#include <cstdint>

namespace rr::game {
// Owned ordinary223030 state. Index order is original D08,D0C,...,D24.
// BA0.W is copy-only and may retain any bit pattern.
struct StaticImpactState49Y { float directionalD08[8]; float impactBA0[4]; };
struct StaticImpactTuning49Y {
    float thresholds00[5]; float severities14[5];
    // Actual track app28C->C0 +60, authored as PodDamage.AIEnvPointsMultiplier.
    // Kept distinct from PodDamage.AIPointsMultiplier at+58.
    float aiEnvironmentMultiplier60;
    bool aiEnvironmentMultiplierBound;
};
struct StaticImpactInput49Y {
    std::uint32_t vehicle40, vehicle58, bodyContactPartnerCount20C;
    std::uint8_t flagsCD8, sceneFlags190;
    BodyMatrix48G readyMainScene190;
    float accumulatedImpact1C0[4];
    float reductionCF0;
    StaticImpactTuning49Y tuning;
    float priorVf9W;
    // Per-call observations of existing canonical owners, not persistent
    // replicas. partnerState is vehicleD34->+4, never body20C or vehicleCFC.
    std::uint32_t vehicleCFC, partnerState;
    float damage7D0, damage7D4, damage7DC; //2234E0 does not consume damage7D8.
    bool aiOwnersBound;
};
enum class StaticImpactStatus49Y { applied, skipped, unsupported, arithmetic };
// Complete reached223030 ->28BBB0 ->222770 numerical projection for bounded
// vehicle58=0 or1, body20C=0 and already-ready scene190. Input1C0 is not cleared.
// The caller owns real CF0, tuning and empty-partner-list provenance; these are
// never defaulted from static candidate category. Source gate CD8&1 or40=3/4
// returns skipped before consuming any scene/body/tuning data.
// AI58=1 requires bound canonical CFC/partner/damage observations. Only an XS
// threshold match enters222820's partner gate: if partnerState!=0 and source
// max(damage7D0,damage7D4,damage7DC)>0.9, chosen severity becomes zero. Earlier
// matches and no match bypass that gate. CFC=0 additionally requires and
// applies actual authored+60 before CF0 reduction; no default1 is supplied.
// Rear D18 raising uses normalizedZ<-0.85 when AI and CFC!=0, otherwise<-0.75.
// Inputs/all other intermediates remain normal binary32/signed zero. Only the six static
// world/local XYZ square results map subnormal results to signed zero (bounded native
// PCSX2v2.4.0 value policy; no hardware/flag parity or global FTZ). Dynamic split stays strict.
// Nonfinite results still fail. Exact-zero normalization uses original
// positive maximum-finite Q and produces zero XYZ, not host division by zero.
// Requires FE_TONEAREST, no fast math/contraction; IEEE ordinary rounding only,
// not a proof of physical EE extended-float accuracy or status flags.
// Ready getter scratch operands used by self-subtraction must be finite.
// No allocations. Output and scratchW are unchanged on unsupported/arithmetic;
// skipped copies state and priorVf9W without interpreting copy-only words.
// Output may alias state; otherwise descriptors/storage must not overlap.
// This is a caller precondition, not a runtime pointer-overlap check. In
// particular nextVf9W must not alias input, state, or output storage.
// Later2228D0/222B20 consumers, dynamic body20C partners, other vehicle58 routes and
// scene refresh are separate dependencies, not silently executed or omitted.
StaticImpactStatus49Y recordStaticImpact49Y(const StaticImpactInput49Y&,
    const StaticImpactState49Y&, StaticImpactState49Y& output,
    float& nextVf9W) noexcept;
// Original223030 split around its222F00 dynamic call. These transient values
// are calculated once from the canonical scene/1C0, never supplied as guessed
// contact direction or severity. The dynamic source continuation owns the
// scalar severity between these two calls. Existing static entry is unchanged.
struct ImpactGeometry50IW {
    float magnitude=0,local[3]{},normal[3]{},nextScratchW=0;
};
StaticImpactStatus49Y prepareImpactGeometry50IW(const StaticImpactInput49Y&,
    ImpactGeometry50IW&) noexcept;
StaticImpactStatus49Y applyImpactSeverity50IW(const StaticImpactInput49Y&,
    const ImpactGeometry50IW&,float sourceSeverity,
    const StaticImpactState49Y&,StaticImpactState49Y&) noexcept;
}
