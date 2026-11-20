#pragma once
#include <cstdint>

// Explicit source identity. TA remains the compatibility default; unknown
// values are rejected by the cold adapters and never fall back to TA.
//
// `course` (146r): a course on the ONE common race route whose code is not a
// stock code (an add-on course from the DLC registry). It names no stock
// profile: every consumer takes that course's data by its own code and has no
// stock pins to compare (absent stock row = no receipt, never a refusal).
// PORT EXTENSION, NOT ORIGINAL (the original has 13 courses only).
namespace track_source_fixture {
enum class Track : std::uint8_t { ta, gc, ga, ma, sa, ra, mb, sc, tc, rc, tb, gb, mc, course };
}
