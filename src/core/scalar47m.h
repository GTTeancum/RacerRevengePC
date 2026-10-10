#pragma once
#include "mmi.h"
namespace rr {
// Non-trapping integer/shift/multiply/divide instructions from PCSX2.
// Branches, memory, syscall, traps and overflow-producing variants are left to
// the execution engine. Unsupported words leave all state, including code, intact.
bool executeScalar47m(MmiState& state,uint32_t instruction) noexcept;
}
