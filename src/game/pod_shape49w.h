#pragma once
#include "body_scene48g.h"
#include "spring_records48n.h"
#include <cstdint>

namespace rr::game {
// Immutable, load-time projection of the authored type0 shape: local matrix
// at shape+40 and root node *(shape+114), center+0C and half+18. The original
// POD01 podboxf.col has identity local40, but the numerical leaf never folds it.
struct PodShapeSource49W {
    BodyMatrix48G local40{};
    Vector3_48C rootCenter{}, rootHalf{};
};
// Caller-owned persistent projection of vehicle694. registrationE0 represents
// the CURRENT object+E0 identity, not its constructor initialization. Default
// state deliberately requires an explicit current-null admission before use.
struct PodShapeState49W {
    BodyMatrix48G composed00{}, world80{}, previousC0{};
    SpringVector4_48N center60{};
    float radius80 = 0;
    std::uint32_t type88 = 0;
    std::uint32_t registrationE0 = 1;
};
// Exact11F680 type0 numeric preparation, before120230. Does not inspect or
// clear registrationE0 and never claims the live membership update completed.
// Numeric failure preserves state; success commits histories/matrices/sphere
// while retaining the caller's actual E0. The registered adapter must perform
// the real120230 next, preserving this completed prefix on a later boundary.
bool preparePodShape50IZ(const PodShapeSource49W& source,
    const BodyMatrix48G& mainCachedWorld100,PodShapeState49W& state) noexcept;

// Exact bounded11FF90 ->11F680 type0 ->120230 E0-null projection. Input is
// actual main190.cachedWorld100 AFTER the source tail getters; this function
// performs no graph refresh. All16 old world80 words become previousC0, then
// incoming world replaces world80. Four-term ordered composition produces00;
// the unscaled root-half length produces radius, and incoming world (not00)
// transforms root center with W forced to1. No identity or affine shortcut.
//
// Only type0 and current registrationE0==0 are admitted. Consumed arithmetic
// and intermediates must be finite, nearest/even separate binary32. The scalar
// SQRT.S radius step flushes a subnormal squared sum to signed zero, matching
// the retained EE interface; normal sums use sqrt(double) rounded once. Old raw
// histories/center/radius are overwritten/copied without numeric inspection.
// False preserves every state byte. Input may alias state.world80. No I/O,
// callbacks, allocation, traversal, spatial registration or heap ownership.
bool transformPodShape49W(const PodShapeSource49W& source,
    const BodyMatrix48G& mainCachedWorld100, PodShapeState49W& state) noexcept;
}
