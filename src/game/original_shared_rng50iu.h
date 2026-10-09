#pragma once
#include "vehicle_frame49w.h"
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace rr::game {
// Original runtime pointer38544C ->385160, raw state+58 starts at1 in ELF.
// Bind once at fresh native APPLICATION initialization, before any consumer.
// This is not permission to reseed a continuing race/application chronology.
// The complete noise owner is process-wide:14EEF0's tables/readiness and
//4332A8 scratch cell are shared too. All represented308030 consumers use this
// one stable arena address; it is deliberately EXCLUDED from racer snapshots.
inline void initializeOriginalSharedNoise50IU(CoupledNoiseContext49M& shared) noexcept {
    shared={};shared.sharedRandomState=1u;
}
inline void seedOriginalSharedRng50IU(CoupledNoiseContext49M& shared,
    std::uint32_t seed) noexcept {shared.sharedRandomState=seed;}
inline std::uint32_t drawOriginalSharedRng50IU(CoupledNoiseContext49M& shared) noexcept {
    auto& raw=shared.sharedRandomState;raw=raw*0x41c64e6du+0x3039u;
    return raw&0x7fffffffu;
}
// Replace, NEVER supplement, racerBank.owner(frame). The Bank elements API
// records raw POD spans. It already rejects overlapping ranges. Excluding the
// whole shared noise owner preserves one RNG/table chronology across every
// save/restore, including inspect-only visits and failures. Existing consumers
// referencing frame.noise need no copy-in/out or shadow random state.
// Call before seal(), exactly once for the frame. Other owner registrations
// must not include this frame or this shared subobject again.
template<class RacerBank>
inline void registerRacerFrameExceptSharedNoise50IU(RacerBank& bank,
    VehicleFrameState49W& frame) {
    static_assert(std::is_standard_layout_v<VehicleFrameState49W>);
    static_assert(std::is_trivially_copyable_v<VehicleFrameState49W>);
    constexpr auto offset=offsetof(VehicleFrameState49W,noise);
    constexpr auto end=offset+sizeof(CoupledNoiseContext49M);
    static_assert(end<=sizeof(VehicleFrameState49W));
    auto* bytes=reinterpret_cast<unsigned char*>(&frame);
    bank.elements(bytes,offset);
    bank.elements(bytes+end,sizeof(VehicleFrameState49W)-end);
}
} // namespace rr::game
