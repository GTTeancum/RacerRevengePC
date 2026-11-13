#pragma once
// Host-side swap counters kept by WindowHost50DQ::swap (window_host50dq.cpp).
// Measurement only: read by the race frame recorder in
// tests/native/terrain_interactive_loop50du.cpp to count swaps made by the
// camera fade path, which the gameplay window_swaps counter does not see.
// Owner thread only (swap() rejects any other thread). No game memory.
#include <cstdint>

namespace rr::platform {
// Every successful WindowHost50DQ::swap since process start.
std::uint64_t windowSwapTotal50AU() noexcept;
// Largest swap-to-swap gap (ms) among swaps made since the previous call,
// then reset to 0. Returns 0 when no swap happened since the previous call.
// The gap of a swap is measured from the swap before it, whatever made it.
double windowSwapTakeMaxGapMs50AU() noexcept;
} // namespace rr::platform
