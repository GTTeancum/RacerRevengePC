#pragma once
// Optional development-host observation only. No native simulation clocks,
// allocation, logging, synchronization, or background workers are introduced.
#include <cstddef>
#include <cstdint>
#include <limits>
#if defined(__linux__)
#include <time.h>
#else
#include <chrono>
#endif

namespace phase_timing50GK {
enum class Phase : std::uint8_t {
    Input, Physics, RenderPrepareSubmit, CompletionWait, Capture, HostSleep, Count
};
constexpr std::size_t phaseCount = static_cast<std::size_t>(Phase::Count);

inline const char* phaseName(Phase phase) noexcept {
    switch (phase) {
    case Phase::Input: return "input";
    case Phase::Physics: return "physics";
    case Phase::RenderPrepareSubmit: return "render_prepare_submit";
    case Phase::CompletionWait: return "completion_wait";
    case Phase::Capture: return "capture";
    case Phase::HostSleep: return "host_sleep";
    default: return "invalid";
    }
}

struct Sample {
    std::uint64_t wallNs = 0, cpuNs = 0;
    bool wallValid = false, cpuValid = false;
};

namespace detail {
inline void add(std::uint64_t& value, std::uint64_t increment,
    bool& overflow) noexcept {
    constexpr auto limit = (std::numeric_limits<std::uint64_t>::max)();
    if (increment > limit - value) { value = limit; overflow = true; }
    else value += increment;
}
#if defined(__linux__)
inline bool readClock(clockid_t clock, std::uint64_t& ns) noexcept {
    timespec value{};
    if (::clock_gettime(clock, &value) != 0 || value.tv_sec < 0 ||
        value.tv_nsec < 0 || value.tv_nsec >= 1000000000L) return false;
    const auto seconds = static_cast<std::uint64_t>(value.tv_sec);
    const auto fraction = static_cast<std::uint64_t>(value.tv_nsec);
    constexpr auto limit = (std::numeric_limits<std::uint64_t>::max)();
    if (seconds > (limit - fraction) / 1000000000ull) return false;
    ns = seconds * 1000000000ull + fraction;
    return true;
}
#endif
}

inline Sample readClocks() noexcept {
    Sample sample{};
#if defined(__linux__)
    sample.wallValid = detail::readClock(CLOCK_MONOTONIC, sample.wallNs);
    sample.cpuValid = detail::readClock(CLOCK_THREAD_CPUTIME_ID, sample.cpuNs);
#else
    // A non-Linux build retains monotonic wall observations, but explicitly
    // reports missing thread-CPU samples instead of substituting process CPU.
    const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    if (ns >= 0) { sample.wallNs = static_cast<std::uint64_t>(ns); sample.wallValid = true; }
#endif
    return sample;
}

struct Totals {
    // calls includes failed clock observations; each time total includes only
    // its own valid monotonic pairs. Missing CPU is never evidence of idle CPU.
    std::uint64_t calls = 0, wallCalls = 0, cpuCalls = 0;
    std::uint64_t wallNs = 0, cpuNs = 0, maxWallNs = 0, cpuAtMaxWallNs = 0;
    bool cpuAtMaxWallValid = false, clockFailure = false, overflow = false;

    // Public supplied-sample seam for focused checks without sleeps or clock
    // assumptions. Ties retain the first maximum's associated CPU observation.
    void record(const Sample& before, const Sample& after) noexcept {
        detail::add(calls, 1, overflow);
        const bool wallValid = before.wallValid && after.wallValid &&
            after.wallNs >= before.wallNs;
        const bool cpuValid = before.cpuValid && after.cpuValid &&
            after.cpuNs >= before.cpuNs;
        if (!wallValid || !cpuValid) clockFailure = true;
        const auto cpuDelta = cpuValid ? after.cpuNs - before.cpuNs : 0;
        if (wallValid) {
            const auto wallDelta = after.wallNs - before.wallNs;
            if (wallCalls == 0 || wallDelta > maxWallNs) {
                maxWallNs = wallDelta;
                cpuAtMaxWallNs = cpuDelta;
                cpuAtMaxWallValid = cpuValid;
            }
            detail::add(wallCalls, 1, overflow);
            detail::add(wallNs, wallDelta, overflow);
        }
        if (cpuValid) {
            detail::add(cpuCalls, 1, overflow);
            detail::add(cpuNs, cpuDelta, overflow);
        }
    }
};

struct Interval {
    // Set only during cold setup, before any scopes begin. One owning thread;
    // the collector deliberately has no atomics, lock, global state, or heap.
    explicit Interval(bool enable = false) noexcept : enabled(enable) {}
    bool enabled = false;
    Totals totals[phaseCount]{};

    // Only between scopes, ordinarily after five-second telemetry is emitted.
    // Preserves the cold enable choice and performs fixed-size assignments.
    void resetInterval() noexcept {
        for (auto& total : totals) total = Totals{};
    }
};

class Scope final {
public:
    Scope(Interval& interval, Phase phase) noexcept {
        const auto index = static_cast<std::size_t>(phase);
        if (interval.enabled && index < phaseCount) {
            total_ = &interval.totals[index];
            before_ = readClocks();
        }
    }
    ~Scope() noexcept { finish(); }
    Scope(const Scope&) = delete;
    Scope& operator=(const Scope&) = delete;
    Scope(Scope&&) = delete;
    Scope& operator=(Scope&&) = delete;

    // End an observation without changing the lifetime of existing client
    // variables. Idempotent; the destructor also closes early-exit paths.
    void finish() noexcept {
        if (!total_) return;
        const Sample after = readClocks();
        total_->record(before_, after);
        total_ = nullptr;
    }

private:
    Totals* total_ = nullptr;
    Sample before_{};
};
}
