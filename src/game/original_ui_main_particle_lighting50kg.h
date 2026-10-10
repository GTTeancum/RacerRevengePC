#pragma once
#include "original_ui_main_scene50kg.h"
#include <array>
#include <cstdint>

namespace rr::game {
struct OriginalUiMainParticleLighting50KG {
    // Post-162CD8 alpha clear, before 162010 packet upload; RGB byte order
    // is R at bits16..23, G at8..15, B at0..7.
    std::array<std::uint32_t,4> sourceRgbPacked{};
    // Original ParticleCode input rows46..49. RGB are byte-scale floats,
    // not normalized colors. W is stored as raw integer bits0x40 at162158,
    // NOT floating64. ParticleCode later replaces output alpha from brightness.
    std::array<std::array<float,3>,4> sourceLightingRgb46To49{};
    std::array<std::uint32_t,4> sourceLightingWBits46To49{};
};
enum class OriginalUiMainParticleLightingStatus50KG : std::uint8_t {
    complete,unprepared,unsupportedScene,unsupportedArithmetic
};
// Cold authenticated Main-only lighting preparation. Main ambient is50,50,50
// and its directional RGB is0,0,0. All four source corner normals therefore
// yield the same original149310 result; no substitute normal/white tint is used.
// Does not register lights or claim scene update/render scheduling is complete.
// Output remains unchanged on failure.
OriginalUiMainParticleLightingStatus50KG prepareOriginalUiMainParticleLighting50KG(
    const OriginalUiMainSceneOwner50KG&,
    OriginalUiMainParticleLighting50KG&) noexcept;
}
