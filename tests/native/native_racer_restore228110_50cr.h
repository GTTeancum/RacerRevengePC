#pragma once
// Common rewrite (race-flow audit F7): the ONE native 228110 restore of a racer,
// used by both original callers, every course and every pod:
//   * 1AEA70 -> virtual78(a1 = !race1B0 bit4) -> 216A30 a1 != 0 tail 21737C 228110
//     (the in-place 1AC8E0 restart, terrain_interactive_continuation50du.cpp);
//   * 229F80 at a Results replay start (1AB520) and each replay loop (1A8158)
//     (terrain_interactive_loop50du.cpp, which adds 229F80's CD4 keep test).
// 228110: vehicleState40 = 0, damage 7D0..7DC = 0, C00..C06 = 0, and the racer's
// wreck pieces leave the world. Writes only the racer's own banked frame state and
// the host-side destroyed record of that slot (bounds-checked).
#include "native_wreck_debris_sim50pd.h"
#include "../../src/game/native_destroyed_racers50pd.h"
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace native_racer_restore50CR {
// Call inside the racer bank visit of `slot`. Returns false (nothing stored) for a
// slot outside the destroyed-record table. wasOut: the racer was destroyed/limping.
template<class Frame>
inline bool restore228110(std::size_t slot,Frame& s,bool& wasOut,std::uint32_t& state40)noexcept {
    wasOut=false;state40=0;
    if(slot>=rr::game::nativeDestroyedRacers50PD.size())return false;
    auto& record=rr::game::nativeDestroyedRacers50PD[slot];
    state40=s.bodies.main.vehicleState40;
    wasOut=state40!=0u||record.destroyed||record.limping;
    s.bodies.main.vehicleState40=0u;record={};
    s.fields.damage7D0=s.fields.damage7D4=s.fields.damage7D8=s.fields.damage7DC=0.f;
    std::memset(s.packedC00,0,sizeof s.packedC00);
    return true;
}
// After the visits: the restored racers' wreck pieces leave the world (one call per mask).
inline std::size_t removeRestoredPieces228110(std::uint32_t mask)noexcept {
    return mask?native_wreck_debris50PD::removeRacers(mask):0u;
}
} // namespace native_racer_restore50CR
