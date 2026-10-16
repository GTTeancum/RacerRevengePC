#pragma once
#include "vehicle_base_tail50x.h"

namespace rr::assets { class OriginalStandardMaterialReferences50LA; }
namespace rr::game {
class OriginalRacerModelSceneOwner50LA;
// Per-model dispatch of the original left/right engine material callbacks.
// Existing source geometry, material indices, color bytes and texture stay owned
// by the ordinary scene/material path. Nothing here emits replacement geometry.
struct EngineFlareOutput50GV {
    assets::FlameUniformRows50W uniforms[assets::podFlameModelCapacity50W]{}; // distinct per-model copies
    float uv[assets::podFlameModelCapacity50W][6]{}; // u'=u*m0+v*m1+m2; v'=u*m3+v*m4+m5
    std::uint32_t flameAC4[2]{}, sharedRandomState=0, frameStamp[assets::podFlameModelCapacity50W]{};
    float thrustMagnitude[assets::podFlameModelCapacity50W]{};
    std::uint8_t callbacks=0, randomDraws=0;
    std::uint32_t modelCount=2;
    // Exact borrowed owners sampled by this candidate, with native-only
    // stale-result guards. These are not source fields or application epochs.
    assets::OriginalStandardMaterialAnimationInstance50LA* canonicalInstances50LA[assets::podFlameModelCapacity50W]{};
    std::uint64_t canonicalRevisions50LA[assets::podFlameModelCapacity50W]{};
    const assets::OriginalStandardMaterialParameters50LA* canonicalParameters50LA[assets::podFlameModelCapacity50W]{};
};
enum class EngineFlareError50GV : std::uint8_t {
    none, phase, binding, unsupportedState, roundingMode, input, arithmetic
};
struct EngineFlareDiagnostic50GV {
    EngineFlareError50GV error=EngineFlareError50GV::none;
    std::uint32_t side=0, sourceStage=0;
};

// Cold join after actual material population, before the first rendered flame
// frame. Walks the selected engine model's SAME actual retained references/
// table; the matching flame references must select one real animation pass.
// Their multiplicity comes from the actual model slots. modelOrdinal selects
// the existing L/R/BL/BR bank: models[0..3], private records[2..5], immutable
// templates[0/1]. Source220978/2209BC binds bottom-left1C4 to20F110;
//2209D4/220A18 binds bottom-right1C0 to20EE90, sharing the upper side's
// callback and prototype, never its mutable clone. Registers29B050 and binds
//29BD80 from that genuine callback/owner, then publishes the stable clone
// borrow in banked50W state.
// Before2208A0, an actual null/null callback pair leaves the clone unbound;
// the accepted native base-engine binder installs it later at its real call.
// A rendered legacy owner cannot be silently reset/relabelled as this clone.
bool bindCanonicalEngineFlare50LA(assets::PodFlameOwners50W&,
    const OriginalRacerModelSceneOwner50LA& actualModel,
    const assets::OriginalStandardMaterialReferences50LA& actualReferences,
    unsigned modelOrdinal,EngineFlareDiagnostic50GV&) noexcept;
bool bindCanonicalLeftEngineFlare50LA(assets::PodFlameOwners50W&,
    const OriginalRacerModelSceneOwner50LA& actualLeftModel,
    const assets::OriginalStandardMaterialReferences50LA& actualLeftReferences,
    EngineFlareDiagnostic50GV&) noexcept;
bool bindCanonicalRightEngineFlare50LA(assets::PodFlameOwners50W&,
    const OriginalRacerModelSceneOwner50LA& actualRightModel,
    const assets::OriginalStandardMaterialReferences50LA& actualRightReferences,
    EngineFlareDiagnostic50GV&) noexcept;
// Call inside that racer's actual bank visit BEFORE releasing its retained
// material references/model. This removes borrows only; pass destruction owns
// its clones. No frame candidate may remain outstanding during teardown.
void clearCanonicalEngineFlareBorrows50LA(assets::PodFlameOwners50W&,
    const OriginalRacerModelSceneOwner50LA& actualModel) noexcept;

// Source20F110(left)/20EE90(right), dispatch guard29AC90 and RNG308030.
// Uses canonical local thrust, lowSpeedDC, rawB5C, application58 and BaseTail
// ACC/AD0, AC4/AC8. Native model dispatch retains left/right then
// bottom-left/bottom-right order; the second same-side call observes the first
// call's canonical AC4/AC8 update. This extends the existing native dispatch
// order; original visibility/render scheduling remains outside admission.
// Each unique material instance runs once per rendering
// frame230, not per triangle, LOD slot, physics substep, or repeated submission.
// Use one monotonically advancing renderFrameStamp for the accepted frame.
// Repeating a stamp already in the actual joined clone (or the legacy
// model's record[2+model].words[2]) consumes no RNG or
// advance smoothing/scroll. Initial FFFFFFFF is reserved and rejects as a stamp.
//
// Output only: no physics, frame, baseTail, material, or RNG is mutated. After
// renderer success the caller may commit the exact fields with the helper.
// This extends the existing source-shared random stream by exactly one draw
// per invoked callback; do not replace its input with a visual-only seed.
// Ordinary ready/postgraphics path only; appActive11C pause handling is outside
// admission. Only the admitted source affine TexGen rows are supported.
// Strict finite binary32, FE_TONEAREST, bounded stack, no allocation or I/O.
// Failure preserves output; diagnostic may change. Inputs/output are distinct.
bool advanceEngineFlare50GV(const VehicleFrameState49W&,
    const BaseTailState50X&, const assets::PodFlameOwners50W&,
    std::uint32_t renderFrameStamp, EngineFlareOutput50GV&,
    EngineFlareDiagnostic50GV&) noexcept;

// Actual native UV consumer of the originating pass's249420 seed payload,
// followed by THIS candidate's already-produced vertex animation rows:
//24B3D0 parameters, then24B408 vertex overlay. Only the cold-bound three
// TexGen destinations28/29/30 are admitted. All three seeds are overwritten
// by animated rows in this domain, preserving the source application order.
// The representative collector pass is never consulted. No callback, RNG,
// allocation or table lookup; the origin pass/clone must remain live and
// unchanged until acceptance. Unjoined legacy models retain their existing
// UV output. Failure leaves uv unchanged. Use before commitEngineFlare50GV.
bool exportEngineFlareUv50LA(const EngineFlareOutput50GV&,
    unsigned modelOrdinal,float (&uv)[6]) noexcept;

// Commit only immediately after accepting the frame built from the same input.
// No rebase/retry against a later physics frame: output owns a serial RNG result.
// Joined clones are preflighted together before any state mutation. A stale
// or retargeted joined candidate violates this serial contract and terminates;
// canonical and legacy row state are never silently merged or partially saved.
void commitEngineFlare50GV(const EngineFlareOutput50GV&, VehicleFrameState49W&,
    BaseTailState50X&, assets::PodFlameOwners50W&) noexcept;
}
