#pragma once
#include "original_gc_spray50kg.h"
#include "effect_dispatch50be.h"
#include <array>
#include <cstdint>
#include <vector>

namespace rr::game {
// Real group2CC060 child+C4: 15E800 / 15E8F0 / reader15EF70.
// Defaults describe the source reader; only the authenticated original member
// can publish a parsed definition. Controls belong to the canonical50BE owner.
struct OriginalWreckExplosionDefinition50LA {
    std::array<float,3> minRange{},maxRange{},velocity{};
    OriginalGcCommonEmitterTiming50KG timing{};
    std::int32_t group=-1;
    float life=1000,size=1,growth=0,rate=500,birthAge=0;
    float framesPerSet=-1,inverseFramesPerSet=-1;
    float gravity=3,groundLevel=0,restitution=.55f,spreadVelocityScale220=0;
    std::uint32_t flags=0;
    bool applySpreadToVelocity=true,mirrorX=false,mirrorY=false,parsed=false;
};
// Actual process BSS84CC/433978. Share one record across EVERY explosion
// emitter and retain it across owner reuse. Increment occurs after capacity
// admission, before all three random draws; unsigned wrap preserves ADDIU.
struct OriginalWreckExplosionGlobals50LA {
    bool lazy84CC=false;
    std::uint32_t emissionCounter433978=0;
};
// Live native projection of the same primary body288 supplied to common74.
// This bounded C4 loader requires a genuine nonnull attachment. A null19C is
// distinct from a missing binding; if19C exists, bind its actual child04 tag.
// Names intentionally retain field identities without guessing class purpose.
struct OriginalWreckExplosionAttachmentGate50LA {
    bool sourceBound=false,attachment74Present=false,attachment19CPresent=false;
    bool attachment19CChild04Bound=false;
    std::uint16_t attachment19CChild04TagA4=0,currentTag8BE0=0;
};
struct OriginalWreckExplosionRuntime50LA {
    std::uint32_t ownerId=0,canonicalOwnerId=0;
    const OriginalGcParticleManager50KG* manager=nullptr;
    std::int32_t firstFrame=0,lastFrame=0;
    bool bound=false,failed=false;
    OriginalGcSprayReceipt50KG stoppedAt{};
};
OriginalGcSprayStatus50KG readOriginalWreckExplosionDefinition50LA(
    const std::vector<std::uint8_t>& actualPodExplosionEmt,
    OriginalWreckExplosionDefinition50LA&,assets::OriginalUiPageDiagnostic50KG&) noexcept;
OriginalGcSprayReceipt50KG bindOriginalWreckExplosion50LA(
    OriginalWreckExplosionRuntime50LA&,const OriginalWreckExplosionDefinition50LA&,
    OriginalGcParticleManager50KG&,const EffectOwner50BE&) noexcept;
// Exact15EB08 onward, ONLY after one genuine common15A7A0 returned zero.
// Common gets the live body transform. Its virtual80 is15ACE0; virtual84 is
// stopDeferredEffect50BD/15F070 using this canonical owner's mode/duration.
// This function never calls common or substitutes a callback for simulation.
// Failure latches and retains the completed source mutation/RNG/pool prefix.
OriginalGcSprayReceipt50KG advanceOriginalWreckExplosionAfterCommon50LA(
    OriginalWreckExplosionRuntime50LA&,const OriginalWreckExplosionDefinition50LA&,
    OriginalGcParticleManager50KG&,EffectOwner50BE&,float elapsedSeconds,
    std::uint32_t& sharedRawRandom,OriginalWreckExplosionGlobals50LA&,
    const OriginalWreckExplosionAttachmentGate50LA&) noexcept;
} // namespace rr::game
