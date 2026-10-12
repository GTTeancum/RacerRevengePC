#pragma once
#include <cstdint>

namespace rr::game {
// Typed projections retained for the existing lap/Dax and contact APIs.
// These are the SAME source statics used by other selectors, not private
// per-consumer initialization slots. Earlier raw RNG use alone does not
// initialize them; a cached zero timestamp deliberately permits reseeding.
struct LapEventRandomOwner50ID {
    bool lazy80A8=false;
    std::uint32_t seed3D6B30=0;
};
struct DynamicContactRangeOwner50IW {
    bool lazy80AC=false;
    std::uint32_t rangeCheck3D6B38=0;
};
// One native owner for the original application-static guard/cache pairs.
// Consumers borrow these projections plus the SAME actual raw RNG and clock.
// Keep this outside per-racer snapshots and serialize all consumers. Direct
// raw RNG seeding/drawing must not implicitly initialize these lazy fields.
//
// Lifetime belongs to the caller: the current native race host resets this
// whole owner with its fresh-session raw RNG. That policy does not claim the
// original application's cross-race lifetime. An eventual menu/race join must
// share the owner and raw RNG at one consistent boundary, never copy caches.
struct SharedRandomOwner50ID {
    LapEventRandomOwner50ID seed{};
    DynamicContactRangeOwner50IW range{};
};
} // namespace rr::game
