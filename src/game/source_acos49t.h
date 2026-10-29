namespace ra_pod_scene50LA {class Owner;}
namespace terrain_probe_authoring50fq_fixture {class SourceStage348;}
#pragma once
#include <cstdint>

namespace rr::platform {class OriginalUiGarageUiLights50KG;class OriginalUiGarageSceneLighting50KG;class OriginalUiCourseOwnedLifecycle50KG;}
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

// Original311038 ->302580 ->310440 sine for finite nonnegative inputs whose
// widened magnitude lies within the recovered bound below3*pi/4.
// Preserves source software-double operations and exact constants, including
// the special pi/2 reduction tail. No host libm or global state is involved.
// Legacy original MOT rotation conversion consumes this bounded domain.
// False preserves output. This does not admit arbitrary sine arguments.
bool sourceSinePositiveReduced50IG(std::uint32_t inputBits,std::uint32_t& outputBits) noexcept;

// Concrete reached146898/146E60 use of the SAME source software-double
// operations. Private finite nonnegative RGB envelope, not a parser policy.
class SourceUiLightMath50KG final {
    friend class ::ra_pod_scene50LA::Owner; //Reached RA146720/146E60 only.
    friend class ::rr::platform::OriginalUiGarageUiLights50KG;
    friend class ::rr::platform::OriginalUiCourseOwnedLifecycle50KG; // Same reached Course146720/146E60.
    friend class ::rr::platform::OriginalUiGarageSceneLighting50KG;
    static std::uint32_t range146898()noexcept;
    static bool luminance146E60(std::uint32_t r,std::uint32_t g,std::uint32_t b,
        std::uint32_t& output)noexcept;
    //Actual RA remainder signed diffuse cohort only; UI envelope unchanged.
    static bool luminanceRaRemaining146E60(std::uint32_t,std::uint32_t,std::uint32_t,
        std::uint32_t&)noexcept;
};

// Reached1B12FC..1B1338 only: binary32 products are supplied after the
// actual mul.s steps; software-double sqrt/multiply/narrow preserve source
// order. Finite positive product, finite signed damping multiplier only.
class SourceTerrainSetupMath50LA final {
    friend class ::terrain_probe_authoring50fq_fixture::SourceStage348;
    static bool damping1B12FC(std::uint32_t massSpringProduct,
        std::uint32_t twiceDamping,std::uint32_t& output)noexcept;
};

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
