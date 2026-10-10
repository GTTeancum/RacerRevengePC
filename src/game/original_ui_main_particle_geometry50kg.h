#pragma once
#include "original_ui_main_fire50kg.h"
#include "original_ui_main_particle_camera50kg.h"
#include "../assets/native_asset48b.h"
#include <array>

namespace rr::game {
struct OriginalUiMainParticleRenderConstants50KG {
    // Actual162010 lighting packet order TL,BL,TR,BR. Source149310 lighting
    // owner produces raw RGB bytes; ParticleCode supplies brightness alpha.
    std::array<std::array<std::uint8_t,3>,4> sourceLighting{};
    // Actual current camera previewport matrix and VU constant33 fog lanes.
    std::array<float,16> combinedClip100{};
    float fogStart33Z=0,fogScale33W=0;
    // Actual GS XYOFFSET in pixels, subtracted AFTER source fixed4 projection.
    std::array<float,2> sourceCanvasOffset{};
};
struct OriginalUiMainParticleQuad50KG {
    // Cyclic BL,TL,TR,BR for the native012/023 indices. This preserves the
    // source TL,BL,TR,BR strip's BL--TR diagonal and per-corner attributes.
    std::array<assets::Vertex48B,4> vertices{};
    std::array<float,4> fogFactors{};
    std::uint16_t sourceParticleIndex=0,sourceFrame=0,sourceDepthBucket=0;
    std::uint8_t stateIndex=0; //0=normal/source-alpha,1=negative-run/additive.
    bool visible=false;
};
enum class OriginalUiMainParticleGeometryStatus50KG : std::uint8_t {
    complete,input,arithmetic
};
// Direct native billboard equations statically recovered from ParticleCode.
// No program bytes, guest registers, interpreter, scheduler or RNG is used.
// The caller expands the real packets in unchanged source order and resolves
// their signed run headers before supplying stateIndex. Invisible source-center
// clip results succeed with visible=false. Failure leaves output untouched.
OriginalUiMainParticleGeometryStatus50KG projectOriginalUiMainParticleQuad50KG(
    const OriginalUiMainParticlePacket50KG&,std::uint8_t stateIndex,
    const OriginalUiMainParticleCamera50KG&,
    const OriginalUiMainParticleRenderConstants50KG&,
    OriginalUiMainParticleQuad50KG&) noexcept;
}
