#pragma once
#include "scene_graph49p.h"
#include <cstdint>

namespace rr::game {
constexpr std::uint32_t maxModelPaletteEntries49S = maxSceneNodes49P;
// Caller attestations, not original state fields or a renderer/frame scheduler.
enum class ModelPalettePhase49S : std::uint8_t {
    unspecified, selectedRenderPreparation
};
enum class ModelPaletteSceneState49S : std::uint8_t {
    unavailable, localMatricesOnly, completeCurrentScene
};
struct ModelPaletteInput49S {
    ModelPalettePhase49S phase = ModelPalettePhase49S::unspecified;
    ModelPaletteSceneState49S suppliedState = ModelPaletteSceneState49S::unavailable;
    // Original2412AC gate: only bit01-selected models reach the producer.
    // Other cases reject; they are not successful visibility no-ops.
    std::uint8_t modelFlags191 = 0;
    SceneNodeIndex49P modelRoot = SceneNodeIndex49P::none;
    // model+1B8: PSG source-index order, not motion/VUT enumeration order.
    const SceneNodeIndex49P* sourceNodes = nullptr;
    std::uint32_t sourceNodeCount = 0;
    // One selected original palette. Order, permutations and repeated IDs are
    // retained. All referenced nodes must be valid and nonnull; no null slot.
    const std::uint8_t* paletteIds = nullptr;
    std::uint32_t paletteCount = 0;
};
struct ModelPalette49S {
    BodyMatrix48G rootWorld;
    BodyMatrix48G nodeWorld[maxModelPaletteEntries49S];
    std::uint32_t entryCount;
};
struct ModelPaletteWorkspace49S {
    SceneNode49P nodes[maxSceneNodes49P];
    ModelPalette49S palette;
};
enum class ModelPaletteError49S : std::uint8_t {
    none, unsupportedContext, invalidGraph, invalidRoot,
    invalidSourceTable, invalidPalette, sceneArithmetic
};
struct ModelPaletteDiagnostic49S { ModelPaletteError49S error; };

// Selected numeric projection of2412E4..2412FC and2414F8..2415A4, with exact
//2897B0/289940 lazy worlds supplied by49P. This is NOT a complete241280 call.
// The caller supplies complete current locals, world caches, flags and links
// after its selected animation/root producers; the tags do not run or prove
// those producers, visibility, a frame phase, or intervening renderer work.
//
// Conditionally refresh the model root if dirty, copy all16 root-world words,
// then call the getter on EVERY palette entry in stored order and copy its16
// current world words. This includes repeated IDs and geometry-empty nodes.
// World already contains the full parent chain: do not append rootWorld again
// to a vertex transformed by nodeWorld. No inverse-bind or weighted skinning.
//
// Native bounds are1..64 source nodes and1..64 palette entries in a coherent
// forest of1..64 nodes; storage order need not be parent-first. The model can
// have a parent. No geometry, normal, material, packet/VU, LOD, visibility,
// observer, resource ownership or physical-body association is inferred.
// Uses49P's strict FP/FE_TONEAREST domain. Copy-only opaque matrix words are
// preserved; only arithmetic operands/intermediates require finite values.
//
// Caller contract: graph nodes, input descriptor and its two immutable arrays,
// output, workspace and diagnostic are distinct live initialized records with
// sufficient extents and serial access. This is not a general alias/lifetime
// validator. No allocation or runtime callback. Diagnostic is assigned first.
// Failure preserves graph and output; caller scratch may change. Success sets
// entryCount and the active matrices only; unused output entries are retained.
bool projectModelPalette49S(const SceneGraph49P& graph,
    const ModelPaletteInput49S& input, ModelPalette49S& output,
    ModelPaletteWorkspace49S& workspace, ModelPaletteDiagnostic49S& diagnostic) noexcept;
}
