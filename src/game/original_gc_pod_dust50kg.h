#pragma once
#include "effect_dispatch50be.h"
#include "original_gc_spray50kg.h"
#include <array>
#include <cstdint>
#include <vector>

namespace rr::game {
// Existing racer member order is retained; wreckDust adds the proved piece168 member.
enum class OriginalGcPodDustMember50KG : std::uint8_t {
    leftDust,rightDust,leftSmoke,rightSmoke,wreckDust,wreckSmoke
};
// Immutable source15AEA0/19B060 authoring. Constructor defaults are replaced
// by reader defaults where retail does so (notably +230 and +23C).
struct OriginalGcPodDustDefinition50KG {
    OriginalGcPodDustMember50KG member=OriginalGcPodDustMember50KG::leftDust;
    OriginalGcCommonEmitterTiming50KG timing{};
    std::array<float,3> velocity60{},minimumVelocityNoise240{},maximumVelocityNoise250{};
    std::int32_t particleGroup1B8=-1;
    float particleLife17C=1000,growth180=0,initialSize184=1,emitRate188=500,ageAtBirth1A0=0;
    float gravityMultiplier=3,groundLevel=0,restitution=.55f;
    float framesPerSet=-1,inverseFramesPerSet=-1;
    std::uint32_t particleFlags198=0;
    bool randomMirrorX=false,randomMirrorY=false;
    float minimumInitialSize208=0,maximumInitialSize20C=5,initialSizeToSpeedRatio210=.01f;
    float minimumLife214=500,maximumLife218=3500,minimumPercentLife21C=.5f;
    float minimumPercentLifeConvergance220=1,lifeToSpeedRatio224=5,initialPercentAge228=0;
    float minimumEmitRate22C=0,maximumEmitRate230=10,particlesToDistanceRatio234=.01f;
    float particlesToSpeedRatio238=0,minimumEmissionSpeed23C=0;
    bool parsed=false;
};
OriginalGcSprayStatus50KG readOriginalGcPodDustDefinition50KG(
    const std::vector<std::uint8_t>& originalEmt,OriginalGcPodDustMember50KG,
    OriginalGcPodDustDefinition50KG&,assets::OriginalUiPageDiagnostic50KG&) noexcept;

// POD native extension of the EXISTING EffectOwner50BE. The real controls,
// delay and duration are not copied here. Keep one runtime per actual dust or
// smoke owner in the same racer bank; keep immutable authoring outside it.
struct OriginalGcPodDustRuntime50KG {
    float remainder200=0;
    std::int32_t currentFrame190=0,firstFrame1B0=0,lastFrame1B4=0;
    std::uint32_t managerOwnerId=0,canonicalOwnerId=0;
    OriginalGcPodDustMember50KG member=OriginalGcPodDustMember50KG::leftDust;
    const OriginalGcParticleManager50KG* managerIdentity=nullptr;
    bool bound=false,failed=false;
    OriginalGcSprayReceipt50KG stoppedAt{};
};
// Call after genuine buildEffectControlOwner50BJ and immediate source Stop84,
// before its first component visit. Authored velocity is checked in the real
// owner; this call neither replaces that owner nor republishes its controls.
OriginalGcSprayReceipt50KG bindOriginalGcPodDust50KG(
    OriginalGcPodDustRuntime50KG&,const OriginalGcPodDustDefinition50KG&,
    OriginalGcParticleManager50KG&,const EffectOwner50BE&) noexcept;
// Only call after exactly one successful15A7A0 common prefix returned zero.
// Caller supplies the SAME canonical50BE owner and SAME scene fixed5000 pool
// on every visit. A failed receipt retains all prior source writes/draws and
// publications and latches failure. Never replay common to retry this prefix.
OriginalGcSprayReceipt50KG advanceOriginalGcPodDustAfterCommon50KG(
    OriginalGcPodDustRuntime50KG&,const OriginalGcPodDustDefinition50KG&,
    OriginalGcParticleManager50KG&,EffectOwner50BE&,float elapsedSeconds,
    std::uint32_t& sharedRawRandom,SharedRandomOwner50ID&,HostMonotonicClock50ID&) noexcept;
} // namespace rr::game
