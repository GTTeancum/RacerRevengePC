#include "cycle_ledger.h"

namespace rr {
bool CycleLedger::reset(uint64_t ee_cycle) noexcept {
    now_ = serviced_ = ee_cycle;
    deadline_ = 0;
    armed_ = false;
    return true;
}

bool CycleLedger::arm(uint64_t absolute_ee_deadline) noexcept {
    if (absolute_ee_deadline < now_) return false;
    deadline_ = absolute_ee_deadline;
    armed_ = true;
    return true;
}

uint32_t CycleLedger::budget(uint32_t requested) const noexcept {
    uint64_t remaining = UINT64_MAX - now_;
    if (armed_ && deadline_ - now_ < remaining) remaining = deadline_ - now_;
    return remaining < requested ? static_cast<uint32_t>(remaining) : requested;
}

bool CycleLedger::account(uint32_t elapsed) noexcept {
    if (elapsed > budget(elapsed)) return false;
    now_ += elapsed;
    return true;
}

DeviceAdvance CycleLedger::synchronize() noexcept {
    const DeviceAdvance result{now_ - serviced_, (now_ >> 3) - (serviced_ >> 3)};
    serviced_ = now_;
    return result;
}
} // namespace rr
