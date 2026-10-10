#pragma once
#include <stdint.h>

namespace rr {
// One bit per guest byte. Storage is supplied by the caller; no allocation,
// atomics, vector instructions, or host-width guest addresses are required.
class ByteOwnership {
public:
    bool configure(uint32_t* words, uint32_t wordCount, uint32_t bytes) noexcept {
        words_ = nullptr; count_ = bytes_ = 0;
        const uint32_t required = (bytes >> 5) + ((bytes & 31u) != 0u);
        if (!words || !bytes || wordCount < required) return false;
        words_ = words; count_ = required; bytes_ = bytes;
        clear();
        return true;
    }
    void clear() noexcept {
        for (uint32_t i = 0; i < count_; ++i) words_[i] = 0;
    }
    bool mark(uint32_t address, uint32_t bytes) noexcept {
        if (!validRange(address, bytes)) return false;
        if (!bytes) return true;
        const uint32_t first = address >> 5, last = (address + bytes - 1) >> 5;
        const uint32_t firstMask = UINT32_MAX << (address & 31u);
        const uint32_t lastMask = UINT32_MAX >> (31u - ((address + bytes - 1) & 31u));
        if (first == last) words_[first] |= firstMask & lastMask;
        else {
            words_[first] |= firstMask;
            for (uint32_t i = first + 1; i < last; ++i) words_[i] = UINT32_MAX;
            words_[last] |= lastMask;
        }
        return true;
    }
    bool release(uint32_t address, uint32_t bytes) noexcept {
        if (!validRange(address, bytes)) return false;
        if (!bytes) return true;
        const uint32_t first = address >> 5, last = (address + bytes - 1) >> 5;
        const uint32_t firstMask = UINT32_MAX << (address & 31u);
        const uint32_t lastMask = UINT32_MAX >> (31u - ((address + bytes - 1) & 31u));
        if (first == last) words_[first] &= ~(firstMask & lastMask);
        else {
            words_[first] &= ~firstMask;
            for (uint32_t i = first + 1; i < last; ++i) words_[i] = 0;
            words_[last] &= ~lastMask;
        }
        return true;
    }
    bool owns(uint32_t address, uint32_t bytes) const noexcept {
        if (!validRange(address, bytes)) return false;
        if (!bytes) return true;
        const uint32_t first = address >> 5, last = (address + bytes - 1) >> 5;
        const uint32_t firstMask = UINT32_MAX << (address & 31u);
        const uint32_t lastMask = UINT32_MAX >> (31u - ((address + bytes - 1) & 31u));
        if (first == last) {
            const uint32_t mask = firstMask & lastMask;
            return (words_[first] & mask) == mask;
        }
        if ((words_[first] & firstMask) != firstMask || (words_[last] & lastMask) != lastMask) return false;
        for (uint32_t i = first + 1; i < last; ++i) if (words_[i] != UINT32_MAX) return false;
        return true;
    }
    uint32_t storageBytes() const noexcept { return count_ * 4u; }
private:
    bool validRange(uint32_t address, uint32_t bytes) const noexcept {
        return words_ && address <= bytes_ && bytes <= bytes_ - address;
    }
    uint32_t* words_ = nullptr;
    uint32_t count_ = 0, bytes_ = 0;
};
} // namespace rr
