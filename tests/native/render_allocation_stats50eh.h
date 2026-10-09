#pragma once
#include <cstddef>
#include <cstdint>
#include <limits>

namespace rr::test {
// Observation only: supplied counter deltas do not identify allocation owners,
// bytes, C allocation calls, other threads, or GPU completion. No GL calls,
// counter instrumentation, logging, containers or heap allocation live here.
constexpr std::size_t renderAllocationFrameCapacity50EH = 160;

struct RenderAllocationFrame50EH {
    std::uint64_t allocations = 0;
    // False means the counter decreased; allocations is then unknown, not zero.
    bool valid = false;
};

struct RenderAllocationTotals50EH {
    std::uint32_t frames = 0, validFrames = 0, nonzeroFrames = 0;
    // Includes valid samples only. Saturation is sticky and explicitly flagged.
    std::uint64_t sum = 0, maximum = 0;
    bool sumOverflow = false;
};

struct RenderAllocationSummary50EH {
    RenderAllocationFrame50EH firstFrame{};
    RenderAllocationTotals50EH all{}, afterFirst{};
    // Capacity rejection does not overwrite samples or extend either total.
    // Rejected-frame count also saturates rather than silently wrapping.
    std::uint64_t rejectedFrames = 0;
    bool capacityOverflow = false, rejectedFramesOverflow = false;
};

enum class RenderAllocationObservation50EH : std::uint8_t {
    recorded, counterDecreased, capacityExhausted
};

// Single-threaded caller-owned collector for one bounded run. Construct before
// the frame loop and snapshot the same monotonic uint64 counter immediately
// before/after each submission. Record failures too, before logging/asserting.
// A detected reset/wrap consumes its chronological slot as an invalid sample;
// it never becomes a huge wrapped delta or a zero-allocation success. An
// undetected complete counter cycle is outside the supplied-counter contract.
// afterFirst excludes chronological slot 0 only, even when that slot is invalid;
// it is not evidence of warm-up completion or a steady-state guarantee.
class RenderAllocationStats50EH final {
public:
    RenderAllocationObservation50EH observe(std::uint64_t before,
        std::uint64_t after) noexcept {
        const std::size_t ordinal = summary_.all.frames;
        if (ordinal == renderAllocationFrameCapacity50EH) {
            summary_.capacityOverflow = true;
            addSaturated(summary_.rejectedFrames, 1, summary_.rejectedFramesOverflow);
            return RenderAllocationObservation50EH::capacityExhausted;
        }
        const RenderAllocationFrame50EH sample{after >= before ? after - before : 0,
            after >= before};
        frames_[ordinal] = sample;
        if (ordinal == 0) summary_.firstFrame = sample;
        add(summary_.all, sample);
        if (ordinal != 0) add(summary_.afterFirst, sample);
        return sample.valid ? RenderAllocationObservation50EH::recorded :
            RenderAllocationObservation50EH::counterDecreased;
    }

    const RenderAllocationSummary50EH& summary() const noexcept { return summary_; }
    // Zero-based chronological index. No dangling sentinel/reference for an
    // absent sample; a stored invalid sample is distinguished by its valid flag.
    const RenderAllocationFrame50EH* frame(std::size_t ordinal) const noexcept {
        return ordinal < summary_.all.frames ? &frames_[ordinal] : nullptr;
    }

private:
    static void addSaturated(std::uint64_t& total, std::uint64_t amount,
        bool& overflow) noexcept {
        constexpr auto limit = (std::numeric_limits<std::uint64_t>::max)();
        if (amount > limit - total) { total = limit; overflow = true; }
        else total += amount;
    }
    static void add(RenderAllocationTotals50EH& total,
        const RenderAllocationFrame50EH& sample) noexcept {
        ++total.frames; // Both frame counts are bounded by the fixed capacity.
        if (!sample.valid) return;
        ++total.validFrames;
        if (sample.allocations != 0) ++total.nonzeroFrames;
        if (sample.allocations > total.maximum) total.maximum = sample.allocations;
        addSaturated(total.sum, sample.allocations, total.sumOverflow);
    }

    RenderAllocationFrame50EH frames_[renderAllocationFrameCapacity50EH]{};
    RenderAllocationSummary50EH summary_{};
};
}
