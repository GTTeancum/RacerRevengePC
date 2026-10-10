#pragma once
#include "import_decode48c.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rr::assets {
struct OriginalUiAtmLimits50KG {
    std::size_t inputBytes = 8u * 1024u * 1024u;
    std::uint32_t frames = 64;
};
struct OriginalUiAtmFrame50KG {
    // Exact member-relative consecutive PST extent, including its own header
    // and palette. Feed these ranges into OriginalUiImageSource50KG.
    std::size_t pstOffset = 0, pstBytes = 0;
    std::uint32_t kind = 0, mipCount = 0;
    std::uint32_t width = 0, height = 0; // Base (last stored mip) dimensions.
};
struct OriginalUiAtm50KG {
    std::uint32_t revision = 1, storage = 0;
    // Count and order are read from the ATM, never inferred from member size.
    std::vector<OriginalUiAtmFrame50KG> frames;
};
// Original 102230 storage=0: u32 revision, u32 count, u32 selector, then count
// consecutive PST streams. The nonzero selector is a different TGA path,
// not reserved padding. Only the proven 48E/50KG indexed PST subset is admitted.
// Reads no files and creates no pixels. Authenticate the enclosing archive
// member upstream. Input must remain immutable during this synchronous call.
// Outputs are transactional. Limits may only reduce the fixed ceilings above.
bool splitOriginalUiAtm50KG(const std::uint8_t* input, std::size_t bytes,
    OriginalUiAtm50KG& output, DecodeError48C& error,
    const DecodeHooks48C* hooks = nullptr,
    const OriginalUiAtmLimits50KG& limits = OriginalUiAtmLimits50KG{});
}
