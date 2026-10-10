#pragma once
#include <cstddef>
#include <cstdint>
namespace rr::assets {
struct TrackCollisionMasks50AN {std::uint32_t staticObject=0,floor=0,water=0,tube=0;};
// Original track-specific creator1B5638..1B56F4: lowercase copied filename,
// unanchored floor/water/tube substring matches, OR into STATICOBJECT category.
// Masks are caller-supplied registry values, not guessed loaded bit positions.
// Ordinary printable ASCII names, length excluding NUL,1..127. Cold bounded
// scratch only; no heap. Invalid input preserves output. Does not register a
// world object or imply that every static collider belongs to FLOOR.
bool classifyTrackCollision50AN(const char*name,std::size_t length,
 TrackCollisionMasks50AN,std::uint32_t& category) noexcept;
}
