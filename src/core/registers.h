#pragma once
#include <stdint.h>

namespace rr {
struct alignas(16) Register128 { uint32_t lane[4]; };
static_assert(sizeof(Register128) == 16, "EE registers must remain 128 bits");

class Registers {
public:
    uint32_t low32(unsigned reg) const noexcept {
        return reg == 0 ? 0 : values_[reg].lane[0];
    }
    uint64_t low64(unsigned reg) const noexcept {
        return reg == 0 ? 0 : uint64_t(values_[reg].lane[0]) |
            (uint64_t(values_[reg].lane[1]) << 32);
    }
    Register128 full(unsigned reg) const noexcept {
        return reg == 0 ? Register128{{0, 0, 0, 0}} : values_[reg];
    }
    void write32(unsigned reg, uint32_t value) noexcept {
        if (reg == 0) return;
        values_[reg].lane[0] = value;
        values_[reg].lane[1] = (value & 0x80000000u) ? UINT32_MAX : 0;
    }
    void writeZeroExtended32(unsigned reg, uint32_t value) noexcept {
        if (reg == 0) return;
        values_[reg].lane[0] = value;
        values_[reg].lane[1] = 0;
    }
    void write64(unsigned reg, uint64_t value) noexcept {
        if (reg == 0) return;
        values_[reg].lane[0] = static_cast<uint32_t>(value);
        values_[reg].lane[1] = static_cast<uint32_t>(value >> 32);
    }
    void write128(unsigned reg, const Register128& value) noexcept {
        if (reg != 0) values_[reg] = value;
    }
private:
    Register128 values_[32]{};
};
static_assert(sizeof(Registers) == 512, "Do not embed host pointers in guest state");
} // namespace rr
