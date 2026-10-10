#pragma once
#include "original_ui_main_particle_geometry50kg.h"
#include "original_ui_main_particle_lighting50kg.h"
#include <array>
#include <cstdint>

namespace rr::game {
struct OriginalUiOptionsParticleRenderState50KG {
    // The manager camera is borrowed by OptionsMenu. Its pose/view stay unchanged,
    // but the scene fog render owner sets near/far to1/4000 before particles.
    OriginalUiOptionsCamera50KG sceneCamera{};
    OriginalUiMainParticleCamera50KG particleCamera{};
    OriginalUiMainParticleRenderConstants50KG constants{};
    // Actual GS FOGCOL: R in low byte, G in middle, B in high byte.
    std::uint32_t packedFogColorGs=0;
    std::array<float,3> fogColor{};
    float sourceFogStart=0,sourceFogEnd=0;
};
enum class OriginalUiOptionsParticleRenderStateStatus50KG : std::uint8_t {
    complete,input,arithmetic
};
// Cold closure of the actual Options manager/scene fog sequence. Manager ui.dat
// fog is installed first; scene+3EC fog/reset/+3F0 proxy attaches later at
//1B4918..1B49AC, before particle owner+4A0 at1B4AD0. The scene proxy restores
// its own fog immediately before the scene render consumers. Thus opt.scn wins
// over ui.dat for the actual Options particles; no caller-selected fog is used.
//
// Input camera is the cold authenticated manager preparation (near/far1/100000).
// Rebuilds only projection/combined/screen fields for the source fog phase;
// normalized pose is never fed through a second normalization. Actual fixed
//640x448 viewport, zoom1, no debug/pose mutations, and original Options lighting
// remain required. This does not schedule or execute guest instructions.
// Strict nearest-even binary32/no contraction. Failure leaves output unchanged.
OriginalUiOptionsParticleRenderStateStatus50KG prepareOriginalUiOptionsParticleRenderState50KG(
    const OriginalUiMainSceneOwner50KG&,
    const OriginalUiOptionsCamera50KG& coldManagerCamera,
    const OriginalUiMainParticleLighting50KG&,
    OriginalUiOptionsParticleRenderState50KG&) noexcept;
}
