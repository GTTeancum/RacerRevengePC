#pragma once
// ============================================================================
// PORT EXTENSION, NOT ORIGINAL REVENGE: N64-canonical airborne gravity, keyed
// on track data, inert on stock.
//
// Revenge: each pod pulls itself down in the air with its own PHY
// AirborneGravity x 32.2 (authored_parameters49n.cpp:127; racer +37C, e.g.
// 322 ft/s^2), chosen whenever probe 348+104 says "not near the ground"
// (219810 main body, 218A3C secondary body).
//
// N64 (racer-n64\notes\n64-gravity.md): one gravity for every pod, 32.0 x 0.25
// = 8.0 (0x800A3080, 0x80065E18); the fall speed grows by dt per second in the
// air and each frame drops the pod dt x 30 x 8 x fall speed (0x80067558), i.e.
// a constant 240 N64 units/s^2 = 240 / track scale ft/s^2 (96 at scale 2.5).
//
// Track data: SCB [Environment] AirborneGravity = <ft/s^2>. No stock SCB has
// it, so on stock every racer keeps its own PHY value bit-identically. When
// present it replaces the airborne value (+37C) for every racer, player and AI
// alike, at the three points that select it; the near-ground value (+378 and
// its blend) stays the pod's own. Not reproduced (flagged): the N64 doubles
// the fall growth while reversing, and scales the step by (1 + 0.9 x pitch)
// for a negative pitch (0x80067590..0x800675D4).
//
// One owner writes it: the race world setup (terrain_interactive_world50du.cpp)
// on every race load (absent = cleared). Host memory only. A value outside
// (0, 3220] ft/s^2 is refused, never clamped. Light header (std only).
// ============================================================================
#include <cmath>
#include <cstdint>
#include <iostream>

namespace rr::game {
inline constexpr float n64AirborneGravityMaximum50AG=3220.f; // 100 x the 32.2 PHY unit
struct N64AirborneGravity50AG {
    bool authored=false;
    float value=0.f;            // ft/s^2, used only when authored
    std::uint64_t uses=0;       // selections that took the track value
};
inline N64AirborneGravity50AG n64AirborneGravity50AG;

inline bool n64AirborneGravityAdmissible50AG(float v) noexcept {
    return std::isfinite(v)&&v>0.f&&v<=n64AirborneGravityMaximum50AG;
}
inline void clearN64AirborneGravity50AG() noexcept {n64AirborneGravity50AG=N64AirborneGravity50AG{};}
inline bool setN64AirborneGravity50AG(float v) noexcept {
    if(!n64AirborneGravityAdmissible50AG(v))return false;
    n64AirborneGravity50AG=N64AirborneGravity50AG{};
    n64AirborneGravity50AG.authored=true;n64AirborneGravity50AG.value=v;return true;
}
// The airborne gravity to apply: the pod's own +37C unless the course authors
// the N64 value. Stock: returns podAirborne unchanged.
inline float n64AirborneGravityOr50AG(float podAirborne) noexcept {
    auto& g=n64AirborneGravity50AG;
    if(!g.authored)return podAirborne;
    if(g.uses++==0)std::cerr<<"airborne_gravity_first_use pod_value="<<podAirborne
        <<" track_value="<<g.value<<'\n';
    return g.value;
}
} // namespace rr::game
