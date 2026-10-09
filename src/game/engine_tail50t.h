#pragma once
#include <cstdint>
namespace rr::game {
struct EngineTailState50T {
 std::uint32_t flagsACC,flagsAD0,countAD4[2],timerADC[2],timerAE4,activeAE8,field844;
 std::uint32_t thrustA0[2][4],sharedRandomState;
};
struct EngineTailInput50T {
 std::uint8_t flagsB9C,application58;
 std::uint32_t state40,frameFactorBits;
 bool probe128Present;
};
enum class EngineTailStatus50T { Complete,NeedsFlameInitialization,NeedsProbeReset,Arithmetic };
struct EngineTailResult50T {
 EngineTailStatus50T status=EngineTailStatus50T::Complete;
 EngineTailState50T state{};
 std::uint8_t thrustWriteMask=0,randomDraws=0;
 bool nullProbeResetExecuted=false;
};
// Original2208A0 already-installed flame callback path. Raw source words retain
// untouched lanes. Nonnull probe128 reset and missing flame installation reject
// before any caller mutation; no stubbed gameplay helper or guest execution.
EngineTailResult50T advanceEngineTail50T(const EngineTailState50T&,const EngineTailInput50T&) noexcept;
}
