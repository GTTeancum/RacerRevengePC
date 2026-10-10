#pragma once
#include <stdint.h>

namespace rr {
// Single executor, 2 MiB IOP RAM. Every actual RAM mutation must pass changed().
// Saturation permanently disables reuse; reset is itself a full-range mutation.
template<uint32_t SerialLimit = UINT32_MAX>
class IopRamVersions {
public:
    static constexpr uint32_t Bytes = 2u * 1024u * 1024u;
    static constexpr uint32_t PageShift = 12;
    static constexpr uint32_t Pages = Bytes >> PageShift;
    bool changed(uint32_t address, uint32_t bytes) noexcept {
        if (!valid_) return false;
        if (address > Bytes || bytes > Bytes - address) { valid_ = false; return false; }
        if (!bytes) return true;
        if (serial_ == SerialLimit) { valid_ = false; return false; }
        ++serial_;
        const uint32_t last = (address + bytes - 1) >> PageShift;
        for (uint32_t p = address >> PageShift; p <= last; ++p) versions_[p] = serial_;
        return true;
    }
    bool trustworthy() const noexcept { return valid_; }
    uint32_t pageVersion(uint32_t page) const noexcept { return page < Pages ? versions_[page] : 0; }
private:
    uint32_t versions_[Pages]{};
    uint32_t serial_ = 0;
    bool valid_ = true;
};

// A snapshot belongs to the same memory owner that created it. The 64 KiB
// backwards import search plus two instruction words can span eighteen pages.
struct IopImportRangeVersion {
    static constexpr uint32_t MaxPages = 18;
    uint32_t first = 0, count = 0;
    uint32_t versions[MaxPages]{};
    template<class Watch>
    bool capture(const Watch& watch, uint32_t begin, uint32_t end) noexcept {
        count = 0;
        if (!watch.trustworthy() || begin >= end || end > Watch::Bytes) return false;
        first = begin >> Watch::PageShift;
        const uint32_t n = ((end - 1) >> Watch::PageShift) - first + 1;
        if (n > MaxPages) return false;
        for (uint32_t p = 0; p < n; ++p) versions[p] = watch.pageVersion(first + p);
        count = n;
        return true;
    }
    template<class Watch>
    bool matches(const Watch& watch) const noexcept {
        if (!count || !watch.trustworthy()) return false;
        for (uint32_t p = 0; p < count; ++p)
            if (versions[p] != watch.pageVersion(first + p)) return false;
        return true;
    }
};
static_assert(sizeof(IopRamVersions<>) <= 2060, "IOP write tracking must stay bounded");
static_assert(sizeof(IopImportRangeVersion) == 80, "Import range snapshot size");
} // namespace rr
