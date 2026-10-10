#pragma once
#include "effect_dispatch50be.h"
namespace rr::game {
struct ResetEffectBindings50BK {std::uint32_t owners808[2],owners81C[2],owners804[4];bool table81CPresent;};
struct ResetEffectState50BK {std::uint32_t timers820[4];};
struct ResetEffectTrace50BK {std::uint32_t sourcePC,target;};
struct ResetEffectWorkspace50BK {EffectOwner50BE owners[maxEffectOwners50BE];ResetEffectState50BK state;EffectDispatchWorkspace50BE control;ResetEffectTrace50BK trace[8];};
struct ResetEffectResult50BK {EffectDispatchStatus50BE status;std::uint32_t sourcePC;std::size_t calls,deferredStops;};
// Source217260..217378, ending after final delay-slot timer clear. Arrays808
// and804 exist; optional81C presence and genuine null entries are explicit.
// Performs native Stop84 calls in source order, interleaved timer clears. Does
// not stop800/80C or run subsequent228110/217940/2179B0/registration stages.
// Distinct caller owners/state/bindings/scratch, no external callbacks or heap.
// All caller changes atomic; scratch may change on unsupported input.
ResetEffectResult50BK resetEffectStops50BK(const ResetEffectBindings50BK&,
 EffectOwner50BE*,std::size_t,ResetEffectState50BK&,ResetEffectWorkspace50BK&) noexcept;
}
