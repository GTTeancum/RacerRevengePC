#pragma once
#include <stdint.h>

namespace rr {
// Little-endian register aliases used by the borrowed PCSX2 scalar handlers.
// No SIMD type, allocation, host pointer, global CPU, or platform API is needed.
union alignas(16) MmiRegister {
    uint64_t UD[2]; int64_t SD[2];
    uint32_t UL[4]; int32_t SL[4];
    uint16_t US[8]; int16_t SS[8];
    uint8_t UC[16]; int8_t SC[16];
};
struct alignas(16) MmiState {
    struct { MmiRegister r[32]; } GPR;
    MmiRegister HI, LO;
    uint32_t code, sa; // PCSX2 stores SA in bytes (0..15), not bits.
};
static_assert(sizeof(MmiRegister) == 16, "Guest register width");
static_assert(sizeof(MmiState) == 560, "State layout must match on x86 and x64");

// Caller maintains architectural GPR zero and SA in [0,15]. Invalid major or
// reserved sub-opcodes return false without modifying any state. This executes
// an instruction only: PC, cycles, memory and event scheduling belong to caller.
bool executeMmi(MmiState& state, uint32_t instruction) noexcept;
} // namespace rr
