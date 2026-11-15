#pragma once
#include <cstdint>

namespace rr::game {
// One record of the current camera's region list as source 275990 walks it:
// currentCamera+34 -> +4 primary zone node; that node's +30 list holds
// {+0 next, +4 neighbour zone node, +8 portal}. 275A5C calls 231150 for the
// node's portal against camera+180 with the function-static radius at 442FE8
// (set once to 200.0 at 2759DC..2759EC). Only an overlapping record weighs in.
// Kept in its own tiny header so the camera Session header does not depend on
// the fog reader: editing the reader must not rebuild every Session includer.
struct OriginalZoneFogOverlap50CR {
    std::uint32_t zone48=0;        // neighbour zone, zero-based (node+4 -> +48)
    bool overlaps=false;           // 231150 return value (275A64 beqz v0)
    float planeDistance7C=0.f;     // 231150 output at stack+7C
};
inline constexpr float originalZoneFogRadius275990_50CR=200.0f; // 0x43480000 at 2759DC
}
