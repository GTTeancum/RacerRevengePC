#pragma once
#include "impact_authoring49y.h"
namespace rr::assets {
enum class RepairAuthoringKey50A : std::uint32_t {
    BaseAbilityRepair, MaxAbilityRepair, MinRepair, MaxRepair,
    RepairA, RepairB, RepairC, Count
};
constexpr std::size_t repairAuthoringKeyCount50A=
    static_cast<std::size_t>(RepairAuthoringKey50A::Count);
struct RepairAuthoring50A {
    rr::game::AuthoredValue49N values[repairAuthoringKeyCount50A];
};
struct RepairProvenance50A {
    ImpactAuthoringToken49Y fields[repairAuthoringKeyCount50A];
};
// Converter-facing raw repair authoring, not live CF4 selection. Reads
// BaseAbilities/MaxAbilities.BaseRepairAmount and DamageRepair.Min/Max-
// BaseRepairAmount/BaseRepairAmountA/B/C. Each field is independently optional;
// absent remains present=false, value=0 and empty provenance. This zero is an
// absent-record representation, never an authored default or a runtime binding.
// Tuning selection metadata stays with AuthoredParameters49N; current profile,
// inferred class, override route and interpolation remain separate owners.
// Uses exact parseImpactNumber49Y. Same bounded ASCII text grammar and limits
// as49N/49Y:32768bytes,255-byte lines,32sections,512keys, case-insensitive names,
// LF/CRLF/external whitespace/whole-line semicolon comments. Unknown values are
// unconsumed. Reject duplicate sections/keys (including unknown), malformed
// text, unsupported consumed numeric forms. These are native admission limits,
// not the original first-match or missing-key write behavior.
// Allocation-free, serial load-time call. Output/provenance unchanged on false;
// diagnostic always written. Input/output/audit/diagnostic must be disjoint.
bool readRepairAuthoring50A(const char* text,std::size_t bytes,
    RepairAuthoring50A& output,PodPhysicsDiagnostic49N& diagnostic,
    RepairProvenance50A* provenance=nullptr) noexcept;
}
