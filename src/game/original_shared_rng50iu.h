#pragma once
#include "vehicle_frame49w.h"
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace rr::game {
// Original runtime pointer38544C ->385160, raw state+58 starts at1 in ELF.
// Bind once at fresh native APPLICATION initialization, before any consumer.
// This is not permission to reseed a continuing race/application chronology.
// The complete noise owner is process-wide:14EFD0's tables/readiness and
//4332A8 scratch cell are shared too. All represented308030 consumers use this
// one stable arena address; it is deliberately EXCLUDED from racer snapshots.
inline bool initializeOriginalSharedNoise50IU(CoupledNoiseContext49M& shared) noexcept {
    //Fresh-only reset must never erase a reached partial initializer. Raw-only
    //source reseeding below does not clear this native admission guard.
    if(shared.noise.initializationIncomplete49M)return false;
    shared={};shared.sharedRandomState=1u;return true;
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
// use frame.noiseOwner(). Optional application borrowing preserves the same
// owner address across frontend/native-frame consumers. Transactional frame
// leaves stage noise in their existing scratch and publish only on success.
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
// GB's four shared world animations use the SAME motion owner as racer
// animation. Its lazy source fields are process chronology, not racer state.
// Keep all other animationShared fields banked; do not replace the owner with
// a detached world-only surrogate or weaken the Binding's disjointness proof.
struct RacerSharedFrameLayout50GB final {
    using Frame=VehicleFrameState49W;
    static constexpr std::size_t motionBegin=offsetof(Frame,animationShared)+offsetof(VehicleAnimationShared49T,motion);
    static constexpr std::size_t motionEnd=motionBegin+sizeof(ModelMotionSharedState49T);
    static constexpr std::size_t noiseBegin=offsetof(Frame,noise);
    static constexpr std::size_t noiseEnd=noiseBegin+sizeof(CoupledNoiseContext49M);
    static_assert(std::is_standard_layout_v<Frame>&&std::is_trivially_copyable_v<Frame>);
    static_assert(std::is_standard_layout_v<VehicleAnimationShared49T>);
    static_assert(motionBegin<motionEnd&&motionEnd<noiseBegin&&noiseEnd<sizeof(Frame));
};
template<class RacerBank>
inline void registerRacerFrameExceptSharedMotionAndNoise50GB(RacerBank& bank,VehicleFrameState49W& frame) {
    using L=RacerSharedFrameLayout50GB;
    auto* bytes=reinterpret_cast<unsigned char*>(&frame);
    bank.elements(bytes,L::motionBegin);
    bank.elements(bytes+L::motionEnd,L::noiseBegin-L::motionEnd);
    bank.elements(bytes+L::noiseEnd,sizeof(frame)-L::noiseEnd);
}
// Pure layout predicate shared by the concrete GB visibility bridge and its
// regression. This is not Bank/query authority: the bridge supplies private
// real span tests and separately authenticates its selected Bank/rig/cables.
template<class ExactSpan,class DisjointSpan>
inline bool racerFrameSharedExclusion50GB(const VehicleFrameState49W& frame,
    ExactSpan&& exact,DisjointSpan&& disjoint) noexcept {
    using L=RacerSharedFrameLayout50GB;
    const auto* bytes=reinterpret_cast<const unsigned char*>(&frame);
    return exact(bytes,L::motionBegin)&&
        exact(bytes+L::motionEnd,L::noiseBegin-L::motionEnd)&&
        exact(bytes+L::noiseEnd,sizeof(frame)-L::noiseEnd)&&
        disjoint(bytes+L::motionBegin,sizeof(ModelMotionSharedState49T))&&
        disjoint(bytes+L::noiseBegin,sizeof(CoupledNoiseContext49M));
}
} // namespace rr::game
