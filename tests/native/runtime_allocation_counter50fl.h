#pragma once
#include <cstdint>
// One inline TLS counter shared by source-separated synchronous client passes.
// The replacement allocation operators remain in the main translation unit.
inline thread_local std::uint64_t allocations=0;
