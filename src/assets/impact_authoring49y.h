#pragma once
#include "pod_physics49n.h"
#include "../game/static_impact49y.h"
#include "../game/dynamic_impact50iw.h"
namespace rr::assets {
struct ImpactAuthoringToken49Y {
    std::uint32_t sectionOffset, sectionBytes, keyOffset, keyBytes, valueOffset, valueBytes;
};
struct EnvironmentImpactProvenance49Y { ImpactAuthoringToken49Y fields[10]; };
struct AIEnvironmentImpactProvenance49Y { ImpactAuthoringToken49Y fields[11]; };
struct DynamicImpactProvenance50IW { ImpactAuthoringToken49Y fields[13]; };
struct FinalImpactAuthoring49Y { float threshold64, increment68; };
struct FinalImpactProvenance49Y { ImpactAuthoringToken49Y fields[2]; };
struct DefenseValue49Y { float value; bool present; };
struct DefenseAuthoring49Y {
    DefenseValue49Y baseDefense, maxDefense, minDefense, maxDefenseBound;
};
struct DefenseProvenance49Y { ImpactAuthoringToken49Y fields[4]; };
// Exact 49N decimal subset plus optional signed leading-dot fractions. A
// leading zero is inserted before invoking the integer-only 49N conversion;
// original302E10/310440 equivalence is independently verified. Atomic failure.
bool parseImpactNumber49Y(const char*,std::size_t,float&,
                          std::uint64_t* binary64Bits=nullptr) noexcept;
// Bounded load-time original decoded text readers; no heap allocation. Same
// byte/line/section/key bounds and ASCII/case/duplicate rules as49N. Unknown
// values remain unconsumed; duplicate unknown keys/sections still reject.
// EnvDamage requires all10keys, in source order hitXL..XS,pointsXL..XS.
// The ordinary reader leaves tuning.aiEnvironmentMultiplierBound=false; its
// zero-initialized+60 is deliberately not a usable AI tuning value.
// Defense fields are independently optional: missing remains absent, never
// supplies a default, resolves upgrades or selects a live/profile CF0.
// Output and optional provenance are unchanged on failure. Input/output/audit
// must not overlap. Diagnostic is always written. Native admission is stricter
// than the original first-match reader; inline comments and exponents reject.
bool readEnvironmentImpact49Y(const char*,std::size_t,
    rr::game::StaticImpactTuning49Y&,PodPhysicsDiagnostic49N&,
    EnvironmentImpactProvenance49Y* = nullptr) noexcept;
// AI reader requires the same10 EnvDamage keys and the separate PodDamage
// AIEnvPointsMultiplier key. Original1AA8E8 stores this value to tuning+60.
// All11 values and provenance commit together, with+60 explicitly bound.
// Missing or malformed AI authoring fails, never substituting a multiplier1.
bool readAIEnvironmentImpact49Y(const char*,std::size_t,
    rr::game::StaticImpactTuning49Y&,PodPhysicsDiagnostic49N&,
    AIEnvironmentImpactProvenance49Y* = nullptr) noexcept;
// Requires both PodDamage.FinalDamageThreshold/FinalDamageIncrement keys.
// Same bounded/atomic contract as environment authoring; no default values.
bool readFinalImpactAuthoring49Y(const char*,std::size_t,FinalImpactAuthoring49Y&,
    PodPhysicsDiagnostic49N&,FinalImpactProvenance49Y* = nullptr) noexcept;
bool readDynamicImpactAuthoring50IW(const char*,std::size_t,
    rr::game::DynamicImpactTuning50IW&,PodPhysicsDiagnostic49N&,
    DynamicImpactProvenance50IW* = nullptr) noexcept;
bool readDefenseAuthoring49Y(const char*,std::size_t,DefenseAuthoring49Y&,
    PodPhysicsDiagnostic49N&,DefenseProvenance49Y* = nullptr) noexcept;
}
