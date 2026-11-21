#pragma once
// Race fog far distance: the one value the race load (1AA158..1AA178) and the
// camera start restore (1A98DC) pass to 13E920 as far50. The original passes
// the constant 4000 (1AA15C lui 0x457A) with near 0x44A6A99A (1333.3) and
// base 64, so Visibility 1 puts the fog end and the camera far plane (13EA30,
// far = end38) at 4000 ft on every stock course.
//
// N64-canonical DLC key (inert on stock): a converted N64 course may author
// "FarDistance" in its SCB Fog block (278570 block; read by the one track fog
// reader, original_track_fog50cr.h). N64 fog ends at the 2.5-ft track scale
// reach 5,000-15,000 ft. No stock SCB authors the key (13 stock Fog blocks
// carry only ColorRGB / Visibility / Haziness), so on stock the far stays
// 4000 and every fog value is bit-identical.
//
// One owner writes it: OriginalTrackFogRuntime50CR::prepare, on every race
// load (stock: 4000), before 273C64 / 1A98DC read it. Host memory only, never
// guest memory. A value that does not fit is refused, never clamped.
#include <cmath>

namespace rr::game {
inline constexpr float originalRaceFogFar50NF=4000.f;          // 1AA15C/1AA160 0x457A0000
inline constexpr float originalRaceFogNear50NF=1333.300048828125f; // 0x44A6A99A (1AA164..1AA170)
inline constexpr float raceFogFarMaximum50NF=100000.f;          // 114F20 far-plane clamp
inline bool raceFogFarAdmissible50NF(float farDistance) noexcept {
    return std::isfinite(farDistance)&&farDistance>originalRaceFogNear50NF&&farDistance<=raceFogFarMaximum50NF;
}
namespace race_fog_far50nf_detail {
inline float& storage() noexcept {static float farDistance=originalRaceFogFar50NF;return farDistance;}
}
inline float raceFogFarDistance50NF() noexcept {return race_fog_far50nf_detail::storage();}
inline bool setRaceFogFarDistance50NF(float farDistance) noexcept {
    if(!raceFogFarAdmissible50NF(farDistance))return false;
    race_fog_far50nf_detail::storage()=farDistance;return true;
}
} // namespace rr::game
