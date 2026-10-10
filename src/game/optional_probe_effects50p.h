#pragma once
#include "impact_effects50b.h"
namespace rr::game {
struct OptionalProbeOwners50P {
 std::uint32_t optional7FC,manager51C,effect808[2];
};
enum class OptionalProbeStatus50P { Complete,UnsupportedActive,Binding,Arithmetic };
struct OptionalProbeResult50P {
 OptionalProbeStatus50P status=OptionalProbeStatus50P::Complete;
 std::uint32_t sourcePC=0,count=0;
 ImpactEffectEvent50B events[2]{};
};
// Original2179D0 early-return or stop-only route. Presence/effect identities are
// explicit current owners. Probe fields and main local velocity are canonical.
// Active geometry/B9C-writing branches stop before217A40, not a fabricated no-op.
// Backend calls remain ordered outputs; duplicates are not removed. No mutation
// or allocation; unconsumed velocity words are not classified or used.
OptionalProbeResult50P inspectOptionalProbeEffects50P(const OptionalProbeOwners50P&,
 std::uint32_t probe104,std::uint32_t probe118,const float& localVelocityZ88) noexcept;
}
