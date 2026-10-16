#pragma once
#include "effect_dispatch50be.h"
#include "original_gc_spray50kg.h"
#include <array>
#include <cstdint>
#include <vector>

namespace rr::game {
// Native reconstruction of19DBE0/19DCD0/19E9F0, specifically the actual
// piece+16C owner created by2CB5A0.19CC50 podSpray is a different class.
// Values below are reader defaults; only the authenticated member parser may
// publish a parsed definition. Common mutable controls remain with50BE.
struct OriginalWreckSplashDefinition50LA {
    OriginalGcCommonEmitterTiming50KG timing{};
    std::array<float,3> minRange220{},maxRange230{},positionOffset240{},velocity60{};
    float minStreamHeight208=1,maxStreamHeight20C=3,sizeModifier200=.5f;
    float fillerLifeModifier210=.4f,fillerSizeModifier214=.8f;
    float fillerVelocityModifier218=.5f,sideVelocityModifier21C=2.5f;
    std::int32_t particleGroup1B8=-1;
    float particleLife17C=1000,initialSize184=1,growth180=0,emitRate188=500,ageAtBirth1A0=0;
    float groundLevel=0,gravityMultiplier=3,restitution=.55f;
    std::uint32_t particleFlags198=0;
    bool randomMirrorX=false,randomMirrorY=false;
    float framesPerSet=-1,inverseFramesPerSet=-1;
    bool parsed=false;
};
OriginalGcSprayStatus50KG readOriginalWreckSplashDefinition50LA(
    const std::vector<std::uint8_t>& originalEmt,OriginalWreckSplashDefinition50LA&,
    assets::OriginalUiPageDiagnostic50KG&) noexcept;

// Native extension of the real canonical50BE owner, not another control bank.
// +204 fractional demand aliases canonicalOwner.state.intensity204. Actual
//2CB898 args a3/t0/t1 are null: optional side250, attachment74 and offset are
// absent by source construction. This bounded owner is not an attached variant.
struct OriginalWreckSplashRuntime50LA {
    const OriginalGcParticleManager50KG* managerIdentity=nullptr;
    std::uint32_t managerOwnerId=0,canonicalOwnerId=0;
    std::int32_t firstFrame1B0=0,lastFrame1B4=0,currentFrame190=0;
    float carriedSize18C=0;
    bool bound=false,failed=false;
    OriginalGcSprayReceipt50KG stoppedAt{};
};
// Call after genuine Splash19DBE0 construction/load and immediate Stop84,
// before source parent141960 registration with child mask0402. The existing
// scene+4A0 manager must be the same fixed5000 pool used by all race effects.
OriginalGcSprayReceipt50KG bindOriginalWreckSplash50LA(
    OriginalWreckSplashRuntime50LA&,const OriginalWreckSplashDefinition50LA&,
    OriginalGcParticleManager50KG&,const EffectOwner50BE&) noexcept;
// Real virtual88=15AD50 from vtable3CEAA8. For this authenticated mode1
// owner, restore canonical delay1DC then duration1E4 from authoring only.
// Leaves flags, positions, timer19C and fractional204 untouched; no RNG draw.
OriginalGcSprayReceipt50KG resetOriginalWreckSplash88_50LA(
    const OriginalWreckSplashRuntime50LA&,const OriginalWreckSplashDefinition50LA&,
    EffectOwner50BE&,float& canonicalDelay1DC) noexcept;
// Caller has run exactly one successful common15A7A0 prefix returning zero
// on this SAME canonical owner. Common delay/duration and callbacks are not
// repeated here. Failed receipts retain source writes, random draws and live
// particles and latch failure; never replay this partial prefix to retry.
OriginalGcSprayReceipt50KG advanceOriginalWreckSplashAfterCommon50LA(
    OriginalWreckSplashRuntime50LA&,const OriginalWreckSplashDefinition50LA&,
    OriginalGcParticleManager50KG&,EffectOwner50BE&,float elapsedSeconds,
    std::uint32_t& sharedRawRandom,SharedRandomOwner50ID&,HostMonotonicClock50ID&) noexcept;
} // namespace rr::game
