#pragma once
#include "original_gc_particle_manager50kg.h"
#include "original_ui_main_particle_geometry50kg.h"
#include "original_camera_start_effects.h"
#include "original_gc_gameplay_lighting_candidate.h"
#include <memory>

namespace rr::game {
struct OriginalGcParticleRun50KG {
    std::uint32_t firstQuad=0,quadCount=0;
    std::uint8_t stateIndex=0;
};
// Native source-canvas geometry; Z is the retained source integer packet Z.
// The gameplay backend reverses source screen depth into its SAME core49F
// world depth attachment. No UI frame/scene/material identity is borrowed.
struct OriginalGcParticleDraw50KG {
    const assets::Vertex48B* vertices=nullptr;
    const float* fogFactors=nullptr;
    const OriginalGcParticleRun50KG* runs=nullptr;
    std::uint32_t quadCount=0,runCount=0,sourceParticleCount=0;
    std::array<float,3> fogColor{};
    std::array<float,16> sourceCombined{};
    std::uint64_t cameraSceneKey=0,cameraSessionGeneration=0,generation=0;
};
enum class OriginalGcParticleRenderStatus50KG : std::uint8_t {
    complete,unprepared,alreadyPrepared,manager,camera,fog,lighting,arithmetic,
    frame,unsupportedBrightness,storage,allocation
};
bool originalParticleCornerColors50KG(const CameraViewOutput49F&,
    const original_gc_gameplay_lighting_candidate::Owner&,const NativeGcSelectedLightsCandidate&,
    std::array<std::array<std::uint8_t,3>,4>&) noexcept;
class OriginalGcParticleRenderOwner50KG final {
public:
    OriginalGcParticleRenderOwner50KG() noexcept;
    ~OriginalGcParticleRenderOwner50KG();
    OriginalGcParticleRenderOwner50KG(const OriginalGcParticleRenderOwner50KG&)=delete;
    OriginalGcParticleRenderOwner50KG& operator=(const OriginalGcParticleRenderOwner50KG&)=delete;
    // Allocate once; manager remains at this address for the owner lifetime.
    OriginalGcParticleRenderStatus50KG prepare(const OriginalGcParticleManager50KG&) noexcept;
    // Actual single current camera/viewport/fog publication. Same GC registered
    // ambient owner and permitted native selected-directional publication as the
    // real scene. Native camera-facing light basis is explicit; no claim of VU
    // reciprocal-square-root/cache parity. No RNG, time advance or allocation.
    OriginalGcParticleRenderStatus50KG build(const CameraViewOutput49F&,
        const OriginalCameraViewportFields&,const OriginalRaceFogFields&,
        const original_gc_gameplay_lighting_candidate::Owner&,
        const NativeGcSelectedLightsCandidate&) noexcept;
    const OriginalGcParticleDraw50KG* draw() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
