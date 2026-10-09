#include "compact_registry.h"

namespace rr {
bool CompactRegistry::configure(uint32_t base, uint32_t slots, const RegistryPage* pages,
        uint32_t page_count, const uint8_t* patterns, uint32_t pattern_count,
        const uint16_t* owners, uint32_t owner_count, uint32_t function_count) noexcept {
    *this = CompactRegistry{};
    if (!slots || (base & 3u) || uint64_t(base)+uint64_t(slots)*4u > 0x100000000ull ||
        !pages || !patterns || !owners || !pattern_count || pattern_count > 65536u ||
        page_count != ((slots-1u)/64u+1u) || function_count > 65535u) return false;
    for (uint32_t n=0; n<page_count; ++n) {
        const RegistryPage& page = pages[n];
        if (page.pattern >= pattern_count || !page.owner_count || page.owner_count > 65u ||
            page.owner_base > owner_count || page.owner_count > owner_count-page.owner_base ||
            owners[page.owner_base] != 0) return false;
        for (uint32_t j=0; j<page.owner_count; ++j)
            if (owners[page.owner_base+j] > function_count) return false;
        for (uint32_t j=0; j<64u; ++j)
            if (patterns[uint32_t(page.pattern)*64u+j] >= page.owner_count) return false;
    }
    base_=base; slots_=slots; pages_=pages; patterns_=patterns; owners_=owners;
    return true;
}
} // namespace rr
