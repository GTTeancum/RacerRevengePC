#pragma once
#include "vehicle_base_tail50x.h"

namespace rr::game {
// A render-time projection of the two original per-engine material callbacks.
// Existing Pod01 geometry, material indices, color bytes and texture stay owned
// by the ordinary scene/material path. Nothing here emits replacement geometry.
struct EngineFlareOutput50GV {
    assets::FlameUniformRows50W uniforms[2]{}; // private left/right copies
    float uv[2][6]{}; // u'=u*m0+v*m1+m2; v'=u*m3+v*m4+m5
    std::uint32_t flameAC4[2]{}, sharedRandomState=0, frameStamp[2]{};
    float thrustMagnitude[2]{};
    std::uint8_t callbacks=0, randomDraws=0;
};
enum class EngineFlareError50GV : std::uint8_t {
    none, phase, binding, unsupportedState, roundingMode, input, arithmetic
};
struct EngineFlareDiagnostic50GV {
    EngineFlareError50GV error=EngineFlareError50GV::none;
    std::uint32_t side=0, sourceStage=0;
};

// Source20F110(left)/20EE90(right), dispatch guard29AC90 and RNG308030.
// Uses canonical local thrust, lowSpeedDC, rawB5C, application58 and BaseTail
// ACC/AD0, AC4/AC8. The source callback runs once per MATERIAL per rendering
// frame230, not per triangle, LOD slot, physics substep, or repeated submission.
// Use one monotonically advancing renderFrameStamp for the accepted frame.
// Repeating a stamp already in records[2/3].words[2] does not consume RNG or
// advance smoothing/scroll. Initial FFFFFFFF is reserved and rejects as a stamp.
//
// Output only: no physics, frame, baseTail, material, or RNG is mutated. After
// renderer success the caller may commit the exact fields with the helper.
// This extends the existing source-shared random stream by exactly one draw
// per invoked callback; do not replace its input with a visual-only seed.
// Ordinary ready/postgraphics path only; appActive11C pause handling is outside
// admission. Only the actual affine Pod01 TexGen rows are supported.
// Strict finite binary32, FE_TONEAREST, bounded stack, no allocation or I/O.
// Failure preserves output; diagnostic may change. Inputs/output are distinct.
bool advanceEngineFlare50GV(const VehicleFrameState49W&,
    const BaseTailState50X&, const assets::PodFlameOwners50W&,
    std::uint32_t renderFrameStamp, EngineFlareOutput50GV&,
    EngineFlareDiagnostic50GV&) noexcept;

// Commit only immediately after accepting the frame built from the same input.
// No rebase/retry against a later physics frame: output owns a serial RNG result.
void commitEngineFlare50GV(const EngineFlareOutput50GV&, VehicleFrameState49W&,
    BaseTailState50X&, assets::PodFlameOwners50W&) noexcept;
}
