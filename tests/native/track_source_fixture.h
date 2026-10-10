#pragma once
#include <cstdint>

// Explicit source identity. TA remains the compatibility default; unknown
// values are rejected by the cold adapters and never fall back to TA.
namespace track_source_fixture {
enum class Track : std::uint8_t { ta, gc };
}
