#pragma once
#include <cstdint>

namespace rr::game {
struct SourceLibmState49T {
    std::int32_t mode = 1;
    std::int32_t errnoValue = 0;
    // Caller attestations for original3023A0 and cold44BEB0..44BEC8.
    bool defaultMatherr = true;
    bool coldNanRecord = true;
};
struct SourceAcosTrace49T {
    std::uint64_t widened, inner, wrapped;
};

// Exact bounded311038 ->302730/2FEA90 ->310440 numeric projection. The caller
// owns and serially shares the source libm state. Only mode1, source's default
// matherr handler and cold-zero NaN record are admitted. These flags attest the
// supplied state; this function does not discover or reset source globals.
//
// Every binary32 input word is accepted under that contract. Source exponent-
// zero inputs widen to signed zero. Source NaN payload packing is preserved.
// A non-NaN domain error returns +0 and writes errno33. Other calls preserve
// errno. No clamp, host libm, host floating-point arithmetic or rounding-mode
// dependency is introduced. Optional trace exposes three intermediate words.
//
// False preserves state, output and trace. Distinct initialized live records
// and serial access are caller preconditions. Fixed scalar storage, no heap,
// callbacks, guest memory, instruction interpretation, I/O or hidden globals.
bool sourceAcos49T(std::uint32_t inputBits, SourceLibmState49T& state,
    std::uint32_t& outputBits, SourceAcosTrace49T* trace = nullptr) noexcept;

#ifdef RR_SOURCE_ACOS49T_TEST_API
// Test-only access to the elementary source operations and conversions.
// These symbols are omitted from ordinary production builds.
namespace source_acos49t_test {
enum class Operation : std::uint8_t { add, subtract, multiply, divide };
std::uint64_t arithmetic(Operation, std::uint64_t, std::uint64_t) noexcept;
std::uint64_t widen(std::uint32_t) noexcept;
std::uint32_t narrow(std::uint64_t) noexcept;
std::uint64_t squareRoot(std::uint64_t) noexcept;
}
#endif
}
