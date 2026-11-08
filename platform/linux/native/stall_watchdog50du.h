#pragma once
// Stall diagnostic: the game thread names the work it is about to do
// (mark), and a background thread reports that name when the game thread
// stops marking for more than 1.5 s. No game state is read or written.
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <thread>

namespace stall_watchdog50DU {
inline std::atomic<const char*> stage{"process_start"};
inline std::atomic<std::uint64_t> beats{0};
inline void mark(const char* name) noexcept {
    stage.store(name, std::memory_order_relaxed);
    beats.fetch_add(1, std::memory_order_relaxed);
}
inline void start() noexcept {
    static std::atomic<bool> started{false};
    if (started.exchange(true)) return;
    try {
        std::thread([] {
            using Clock = std::chrono::steady_clock;
            auto last = beats.load(std::memory_order_relaxed);
            auto since = Clock::now();
            const char* reported = nullptr;
            for (;;) {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                const auto now = Clock::now();
                const auto b = beats.load(std::memory_order_relaxed);
                if (b != last) {
                    if (reported) {
                        std::fprintf(stderr, "stall_watchdog resumed after_ms=%lld%c",
                            static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(now - since).count()), 10);
                    }
                    last = b; since = now; reported = nullptr; continue;
                }
                const auto stalled = std::chrono::duration_cast<std::chrono::milliseconds>(now - since).count();
                const char* current = stage.load(std::memory_order_relaxed);
                if (stalled > 1500 && current != reported) {
                    std::fprintf(stderr, "stall_watchdog stage=%s stalled_ms=%lld%c", current,
                        static_cast<long long>(stalled), 10);
                    std::fflush(stderr);
                    reported = current;
                }
            }
        }).detach();
    } catch (...) {}
}
}
