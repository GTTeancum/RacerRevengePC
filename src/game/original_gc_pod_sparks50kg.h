#pragma once
#include "original_gc_spray50kg.h"
#include "effect_dispatch50be.h"
#include <array>
#include <cstdint>
#include <vector>
namespace rr::game {
enum class OriginalGcPodSparksMember50KG : std::uint8_t {podSparks,wreckSparks};
struct OriginalGcPodSparksDefinition50KG {
    OriginalGcPodSparksMember50KG member=OriginalGcPodSparksMember50KG::podSparks;
    std::array<float,3> minRange{},maxRange{},velocity{};
    OriginalGcCommonEmitterTiming50KG timing{};
    std::int32_t group=-1;
    float life=1000,size=1,growth=0,rate=500,birthAge=0;
    float minStreamLength=3,maxStreamLength=5,separation=1.5f,tailLife=.6f;
    float tailThickness=1,headThickness=2,framesPerSet=-1,inverseFramesPerSet=-1;
    float gravity=3,groundLevel=0,restitution=.55f;
    std::uint32_t flags=0;
    bool superbrightHead=true,moving=false,mirrorX=false,mirrorY=false,parsed=false;
};
// Actual process BSS84D0/433980. Shared by ALL ordinary emitters; neither
// reconstructed from a frame nor privately initialized in a racer instance.
struct OriginalGcPodSparksGlobals50KG {bool lazy84D0=false;float speedScale433980=0;};
struct OriginalGcPodSparksRuntime50KG {
    OriginalGcPodSparksMember50KG member=OriginalGcPodSparksMember50KG::podSparks;
    std::uint32_t ownerId=0,canonicalOwnerId=0;
    const OriginalGcParticleManager50KG* manager=nullptr;
    std::int32_t firstFrame=0,lastFrame=0,currentFrame=0;
    float carriedSize18C=0;
    bool bound=false,failed=false;
    OriginalGcSprayReceipt50KG stoppedAt{};
};
struct OriginalGcPodSparksAttachmentGate50KG {
    bool sourceBound=false,attachment74Present=false,attachment19CPresent=false;
    bool attachment19CChild04Bound=false;
    std::uint16_t attachment19CChild04TagA4=0,currentTag8BE0=0;
};
OriginalGcSprayStatus50KG readOriginalGcPodSparksDefinition50KG(
    const std::vector<std::uint8_t>& actualEmt,OriginalGcPodSparksMember50KG,
    OriginalGcPodSparksDefinition50KG&,assets::OriginalUiPageDiagnostic50KG&) noexcept;
OriginalGcSprayStatus50KG readOriginalGcPodSparksDefinition50KG(
    const std::vector<std::uint8_t>& actualPodSparksEmt,
    OriginalGcPodSparksDefinition50KG&,assets::OriginalUiPageDiagnostic50KG&) noexcept;
OriginalGcSprayReceipt50KG bindOriginalGcPodSparks50KG(
    OriginalGcPodSparksRuntime50KG&,const OriginalGcPodSparksDefinition50KG&,
    OriginalGcParticleManager50KG&,const EffectOwner50BE&) noexcept;
// Exact15F3C0 onward, only AFTER one genuine15A7A0 returned zero. Actual
// vehicle sparks have source-null74; wreck sparks require their real attachment gate. Controls remain canonical50BE state;
// derived204 is authored maxStreamLength, NOT adapter intensity204 storage.
// Failure retains and latches the completed source/RNG/particle prefix.
OriginalGcSprayReceipt50KG advanceOriginalGcPodSparksAfterCommon50KG(
    OriginalGcPodSparksRuntime50KG&,const OriginalGcPodSparksDefinition50KG&,
    OriginalGcParticleManager50KG&,EffectOwner50BE&,float elapsedSeconds,
    std::uint32_t& sharedRawRandom,OriginalGcPodSparksGlobals50KG&,
    const OriginalGcPodSparksAttachmentGate50KG* actualAttachment=nullptr) noexcept;
} // namespace rr::game
