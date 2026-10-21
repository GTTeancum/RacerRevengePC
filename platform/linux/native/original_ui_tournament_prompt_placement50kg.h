#pragma once
#include <cstdint>

namespace rr::platform {
struct OriginalUiTournamentPromptPlacement50KG {
    float x=0,y=0;
    std::uint32_t mask=0,drawPC=0,positionXPC=0;
};
// Numeric placement only, not resource, source-frame or route admission.
// Base1D63B0 sets cross484/triangle397/square285/circle160, all atY378.
// Tournament1EDAA0 calls that base at1EDAB8, then overrides square to265
// (43848000 -> +370 at1EDB34) and triangle to377 (43BC8000 -> +310
// at1EDBD4). Shared1D7A10 still draws cross,triangle,square,circle in order.
// Authored label positions and original texture extents remain unchanged.
inline constexpr bool originalUiTournamentPromptPlacement50KG(
    std::uint32_t ordinal,OriginalUiTournamentPromptPlacement50KG& output) noexcept {
    constexpr OriginalUiTournamentPromptPlacement50KG placements[]{
        {484,378,1,0x1D7A2C,0x1D6878},
        {377,378,2,0x1D7A44,0x1EDBD4},
        {265,378,4,0x1D7A5C,0x1EDB34},
        {160,378,8,0x1D7A74,0x1D6A98}
    };
    if(ordinal>=4)return false;
    output=placements[ordinal];return true;
}
}
