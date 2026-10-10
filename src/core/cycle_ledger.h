#pragma once
#include <stdint.h>

namespace rr {

struct DeviceAdvance {
    uint64_t ee_cycles;
    uint64_t iop_cycles;
};

// Single executor only. The adapter must arm its earliest externally visible
// deadline and synchronize before changing or observing another device's state.
class CycleLedger {
public:
    bool reset(uint64_t ee_cycle = 0) noexcept;
    bool arm(uint64_t absolute_ee_deadline) noexcept;
    void disarm() noexcept { armed_ = false; }
    uint32_t budget(uint32_t requested) const noexcept;
    bool account(uint32_t elapsed) noexcept;
    DeviceAdvance synchronize() noexcept;
    uint64_t now() const noexcept { return now_; }
    uint64_t serviced() const noexcept { return serviced_; }
    bool due() const noexcept { return armed_ && now_ == deadline_; }
private:
    uint64_t now_ = 0;
    uint64_t serviced_ = 0;
    uint64_t deadline_ = 0;
    bool armed_ = false;
};

} // namespace rr
