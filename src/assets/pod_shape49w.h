#pragma once
#include "import_decode48c.h"
#include "../game/pod_shape49w.h"
#include <cstdint>
#include <vector>

namespace rr::assets {
// Cold, bounded COL1/type0 ingestion through the unchanged48G decoder.
// Validates the ENTIRE record, including all indices/tree reachability and
// trailing bytes, before projecting immutable local40 and root bounds. Limits
// are POD01's16 vertices/27 nodes/28 faces,4096 source bytes. No runtime state
// or history is fabricated. False preserves source; decoder allocations are
// load-time only. This is not a collision traversal or heap-ownership bridge.
bool decodePodShapeSource49W(const std::vector<std::uint8_t>& bytes,
    game::PodShapeSource49W& source, DecodeError48C& error);
}
