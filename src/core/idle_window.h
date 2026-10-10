#pragma once
#include <stdint.h>

namespace rr {
// Device-owned proof: no observable work before deadline. External mutations
// must invalidate the proof, even if they appear not to wake a guest thread.
class IdleWindow {
public:
    void invalidate() noexcept { deadline_ = 0; }
    void remember(uint64_t now, uint64_t deadline) noexcept {
        deadline_ = deadline > now ? deadline : 0;
    }
    bool advance(uint64_t target, uint64_t& now) const noexcept {
        if (target <= now || now >= deadline_) return false;
        now = target < deadline_ ? target : deadline_;
        return true;
    }
private:
    uint64_t deadline_ = 0;
};
} // namespace rr
