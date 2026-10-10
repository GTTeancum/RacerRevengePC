#pragma once
#include <stdint.h>

namespace rr {
// Per-address replacement storage. A page is allocated only when first changed;
// presence bits distinguish an explicit null value from an unchanged address.
// Value must be a trivial value (for example a function pointer or integer ID).
// The caller owns the zeroed directory and supplies platform allocation.
template<class Value> class RegistryReplacements {
public:
    struct Page { uint32_t present[2]; Value values[64]; };
    using Allocate = Page* (*)(void*);
    using Release = void (*)(void*,Page*);
    RegistryReplacements() = default;
    RegistryReplacements(const RegistryReplacements&) = delete;
    RegistryReplacements& operator=(const RegistryReplacements&) = delete;
    ~RegistryReplacements() { reset(); }
    void reset() noexcept {
        if (directory_) for (uint32_t i=0;i<page_count_;++i) {
            if (directory_[i]) release_(context_,directory_[i]);
            directory_[i]=nullptr;
        }
        directory_=nullptr; page_count_=slots_=allocated_pages_=0;
    }
    bool configure(uint32_t base, uint32_t slots, Page** directory, uint32_t page_count,
                   Allocate allocate, Release release, void* context) noexcept {
        reset();
        if (!slots || (base & 3u) || uint64_t(base)+uint64_t(slots)*4u > 0x100000000ull ||
            !directory || page_count != (slots-1u)/64u+1u || !allocate || !release) return false;
        for (uint32_t i=0;i<page_count;++i) if (directory[i]) return false;
        base_=base; slots_=slots; directory_=directory; page_count_=page_count;
        allocate_=allocate; release_=release; context_=context;
        return true;
    }
    bool get(uint32_t address, Value& value) const noexcept {
        uint32_t slot;
        if (!index(address,slot)) return false;
        const Page* page=directory_[slot>>6];
        if (!page || !(page->present[(slot>>5)&1u] & (uint32_t(1u)<<(slot&31u)))) return false;
        value=page->values[slot&63u];
        return true;
    }
    bool set(uint32_t address, Value value) noexcept {
        uint32_t slot;
        if (!index(address,slot)) return false;
        Page*& page=directory_[slot>>6];
        if (!page) {
            Page* fresh=allocate_(context_);
            if (!fresh) return false;
            fresh->present[0]=fresh->present[1]=0;
            page=fresh;
            ++allocated_pages_;
        }
        page->values[slot&63u]=value;
        page->present[(slot>>5)&1u] |= uint32_t(1u)<<(slot&31u);
        return true;
    }
    uint32_t allocatedPages() const noexcept { return allocated_pages_; }
private:
    bool index(uint32_t address, uint32_t& slot) const noexcept {
        if ((address&3u) || address<base_) return false;
        slot=(address-base_)>>2;
        return slot<slots_;
    }
    uint32_t base_=0, slots_=0, page_count_=0, allocated_pages_=0;
    Page** directory_=nullptr;
    Allocate allocate_=nullptr;
    Release release_=nullptr;
    void* context_=nullptr;
};
} // namespace rr
