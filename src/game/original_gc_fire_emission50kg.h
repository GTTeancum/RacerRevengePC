#pragma once
#include "original_gc_spray50kg.h"
#include <array>
#include <cstdint>

namespace rr::game {
// Source15E1D0..15E6A8, shared below the actual attached-generator and pod-fire
// owners. This is neither a UI/Main identity nor an emitter control owner.
// Caller has authenticated the EMT and proved NumberOfForcedSets=0.
struct OriginalGcFireEmissionDefinition50KG {
    std::int32_t particleGroup=-1,firstFrame=0,lastFrame=0;
    float particleLife=0,initialSize=0,growth=0,emitRate=0,framesPerSet=0;
    std::array<float,3> minRange{},maxRange{};
    float authoredVelocityY220=0;
    std::uint32_t particleFlags=0;
    bool randomMirrorX=false,randomMirrorY=false,applySpreadToVelocity=true;
};
struct OriginalGcFireEmissionReceipt50KG : OriginalGcSprayReceipt50KG {
    bool nativeUndefinedCarryPolicy=false;
};
// Runs only AFTER common15A7A0 returned0, velocity60 was written, and any real
// attachment render gate passed. Previous-position W is an opaque source word.
// The accumulator is the real derived +224 field, distinct from controls204.
// No lazy reseed occurs in this fire path. raw308030 and initialized433970 are
// the application-owned words, shared with generator/Main/other consumers.
// A failed call retains its source mutation prefix, including unpublished pool
// writes. Do not retry it. Pool capacity failure is a successful source exit.
OriginalGcFireEmissionReceipt50KG emitOriginalGcFireParticles50KG(
    const OriginalGcFireEmissionDefinition50KG&,OriginalGcParticleManager50KG&,
    std::uint32_t managerOwnerId,float& fractional224,
    const std::array<float,3>& previousPosition,std::uint32_t previousPositionW,
    const std::array<float,3>& emitterVelocity,float elapsedSeconds,
    std::uint32_t& sharedRawRandom,std::uint32_t& sharedFireSerial433970,
    bool allowNativeUndefinedCarryPolicy=false) noexcept;
} // namespace rr::game
