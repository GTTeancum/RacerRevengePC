#pragma once
#include <stdint.h>

namespace rr {
struct RegistryPage {
    uint32_t owner_base;
    uint16_t pattern;
    uint16_t owner_count;
};
static_assert(sizeof(RegistryPage) == 8, "Registry page layout must be portable");

// Immutable original-PC lookup. This stores IDs, never host pointers. The
// platform binds IDs to native functions; per-PC replacements require a
// separate overlay and must not mutate a shared symbol binding.
class CompactRegistry {
public:
    bool configure(uint32_t base, uint32_t slots, const RegistryPage* pages,
                   uint32_t page_count, const uint8_t* patterns,
                   uint32_t pattern_count, const uint16_t* owners,
                   uint32_t owner_count, uint32_t function_count) noexcept;
    uint16_t lookup(uint32_t address) const noexcept {
        if ((address & 3u) || address < base_) return 0;
        const uint32_t slot = (address-base_) >> 2;
        if (slot >= slots_) return 0;
        const RegistryPage& page = pages_[slot >> 6];
        const uint8_t local = patterns_[uint32_t(page.pattern)*64u+(slot & 63u)];
        return owners_[page.owner_base+local];
    }
private:
    uint32_t base_ = 0, slots_ = 0;
    const RegistryPage* pages_ = nullptr;
    const uint8_t* patterns_ = nullptr;
    const uint16_t* owners_ = nullptr;
};
} // namespace rr
