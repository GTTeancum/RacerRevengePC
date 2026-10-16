#pragma once
#include "original_gc_fire_emission50kg.h"
#include "effect_dispatch50be.h"

namespace rr::game {
struct OriginalGcPodFireDefinition50KG {
    OriginalGcFireEmissionDefinition50KG emission{};
    std::array<float,3> authoredVelocity{};
    float gravityMultiplier=3,groundLevel=0,restitution=.55f;
    bool parsed=false;
};
// Only the source15DE00 derived state missing from the canonical50BE controls.
// Never copy/reinitialize controls30/40/60,19C,1D4,1D8 or1E4 here. Common delay
// and timing remain in the existing source-common owner. One runtime per actual
// fire child, kept with that racer across all source component visits.
struct OriginalGcPodFireRuntime50KG {
    float fractional224=0;
    std::uint32_t managerOwnerId=0,canonicalOwnerId=0;
    OriginalGcParticleManager50KG* manager=nullptr;
};
using OriginalGcPodFireReceipt50KG=OriginalGcFireEmissionReceipt50KG;
OriginalGcSprayStatus50KG readOriginalGcPodFireDefinition50KG(
    const std::vector<std::uint8_t>& originalPodFireEmt,OriginalGcPodFireDefinition50KG&,
    assets::OriginalUiPageDiagnostic50KG&) noexcept;
// Call alongside the actual freshly constructed/loaded/stopped Fire15DE00
// child, in source registration order. Does not start/stop or reset that child.
OriginalGcPodFireReceipt50KG bindOriginalGcPodFire50KG(
    OriginalGcPodFireRuntime50KG&,const OriginalGcPodFireDefinition50KG&,
    OriginalGcParticleManager50KG&,const EffectOwner50BE&) noexcept;
//15E10C continuation, only after the SAME child's common15A7A0 returned0.
// Source-null74 is proved by the actual vehicle construction route. There is
// no render-stamp input or forced-visible default. Position40 (including W)
// feeds new particles; velocity60 is overwritten in the canonical controls.
// Caller owns common Start/Stop, delay/duration, ordered visit and terminal
// failure policies. This function neither reruns common nor advances manager.
OriginalGcPodFireReceipt50KG advanceOriginalGcPodFireAfterCommon50KG(
    OriginalGcPodFireRuntime50KG&,const OriginalGcPodFireDefinition50KG&,
    OriginalGcParticleManager50KG&,EffectOwner50BE&,float elapsedSeconds,
    std::uint32_t& sharedRawRandom,std::uint32_t& sharedFireSerial433970,
    bool allowNativeUndefinedCarryPolicy=false) noexcept;
} // namespace rr::game
