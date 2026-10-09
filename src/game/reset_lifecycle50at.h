#pragma once
#include "chariot_lifecycle49v.h"
namespace rr::game {
struct ResetLifecycleGlobals50AT {std::uint32_t zero434630[4],forward434660[4],quaternion382B50[4];};
void initializedResetLifecycleGlobals50AT(ResetLifecycleGlobals50AT&) noexcept;
// Original216C4C..216CE4 numeric reset slice, after main rigid-body callback.
// Sets838=1,83C=0,84C=+0 and870/880/890/8A0 records. Preserves840,848,85C,
// other owners and phase. Forward XYZ must be finite; W is not consumed.
// Copy-only except source NEG.S sign flips. No scene/reset orchestration.
bool resetLifecycleRecords50AT(std::uint32_t& vehicle838,ChariotLifecycleState49V&,
 const ResetLifecycleGlobals50AT&) noexcept;
}
