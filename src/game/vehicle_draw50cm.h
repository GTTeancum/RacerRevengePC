#pragma once
#include "vehicle_frame49w.h"
#include "model_palette49s.h"
#include "rigid_vertices49s.h"
#include "../assets/import_scene48d.h"
#include "../assets/native_asset48b.h"

namespace rr::game {
constexpr std::uint32_t maxVehicleDrawSpans50CM = 64;
enum class VehicleDrawComponent50CM : std::uint8_t { chariot, leftEngine, rightEngine };

// Prepared once from the existing semantic LOD0 and its native compatibility
// mesh. All pointers borrow immutable caller-owned storage; no ownership moves.
// Keep the scene, mesh and rigid array alive and unchanged through submission.
struct VehicleDrawMesh50CM {
    const RigidVertex49S* localVertices = nullptr;
    const assets::Vertex48B* vertexTemplate = nullptr;
    std::uint32_t vertexCount = 0, sourceNodeCount = 0;
    const std::uint32_t* indices = nullptr;
    std::uint32_t indexCount = 0;
    const assets::SceneSpan48D* spans = nullptr;
    std::uint32_t spanCount = 0;
};
struct VehicleDrawSelection50CM {
    VehicleDrawComponent50CM component = VehicleDrawComponent50CM::chariot;
    ModelPalettePhase49S phase = ModelPalettePhase49S::unspecified;
    ModelPaletteSceneState49S suppliedState = ModelPaletteSceneState49S::unavailable;
    // Authentic model+1B8 source-index mapping and selected PSG palette. This
    // bridge admits the identity palettes represented by Scene48D only.
    const SceneNodeIndex49P* sourceNodes = nullptr;
    std::uint32_t sourceNodeCount = 0;
    const std::uint8_t* paletteIds = nullptr;
    std::uint32_t paletteCount = 0;
};
struct VehicleDrawBuffer50CM {
    assets::Vertex48B* vertices = nullptr;
    std::uint32_t capacity = 0;
};
struct VehicleDrawSubmission50CM {
    const assets::Vertex48B* vertices = nullptr;
    std::uint32_t vertexCount = 0;
    const std::uint32_t* indices = nullptr;
    std::uint32_t indexCount = 0;
    const assets::SceneSpan48D* spans = nullptr;
    std::uint32_t spanCount = 0;
    // Computed display bounds, not source model/collision bounds.
    assets::Bounds48B bounds{};
};
struct VehicleDrawWorkspace50CM {
    SceneNode49P nodes[vehicleFrameNodeCount49W];
    ModelPalette49S palette;
    ModelPaletteWorkspace49S paletteWorkspace;
    RigidPosition49S positions[maxRigidVertices49S];
};
enum class VehicleDrawError50CM : std::uint8_t {
    none, storage, scene, geometry, vertexTemplate, phase, binding, palette, vertices
};
struct VehicleDrawDiagnostic50CM {
    VehicleDrawError50CM error = VehicleDrawError50CM::none;
    std::uint32_t item = 0;
    ModelPaletteDiagnostic49S palette{};
    RigidVertexDiagnostic49S vertices{};
};

// Cold, allocation-free flattening of 1..4096 semantic LOD0 local vertices.
// Requires a parsed object-space RRSCENE1 with identity_matrix_palette and
// its matching Mesh48B compatibility export. Verifies ordered spans, indices,
// UV and existing color bytes, including the export's explicit white fallback.
// No bind matrix is applied and no new material/color policy is selected.
// Checks all input before writing the rigid array or mesh descriptor. False
// preserves both; item identifies the rejected surface (zero for global errors).
bool prepareVehicleDrawMesh50CM(const assets::Scene48D&, const assets::Mesh48B&,
    RigidVertex49S* localVertices, std::uint32_t capacity,
    VehicleDrawMesh50CM&, VehicleDrawDiagnostic50CM&) noexcept;

// Observe a complete current graphics scene at PostGraphics or completed Ready.
// The explicit 49S tags attest the caller's selected render-preparation cut;
// Ready alone does not prove that an initialized vehicle has run graphics.
// Derives the model root and flag191 from canonical bindings/fields, snapshots
// the active scene forest, then runs the real49S palette and diagnostic rigid projection.
// Canonical graph, frame caches, physics and RNG are never mutated. In
// particular, later physical poses never replace the earlier graphics roots.
//
// On success output is directly compatible with main48b.cpp: Vertex48B stride,
// R32_UINT indices, triangle-list SceneSpan48D ranges and baseVertex=0. Positions
// are already world-space; do not apply rootWorld or another model transform.
// UV/color/index/span/material bytes remain those of the prepared source.
// No device commands, camera, material selection, visibility, LOD scheduling,
// packet execution or original VU-shader parity is supplied.
//
// Single-threaded, fixed caller storage, no allocation/I/O/callback. Inherits
// strict FP/FE_TONEAREST requirements of49S. Inputs, their backing arrays,
// destination vertices, output, workspace and diagnostic are distinct live
// storage with stable lifetimes. Descriptor/backing-array validity and overlap
// are caller preconditions; prepare once and do not mutate the prepared mesh.
// False preserves destination vertices and output; workspace/diagnostic may
// change. Submit only on true. For a three-model vehicle, prepare all outputs
// successfully before submitting any of them to avoid a mixed-frame draw.
bool buildVehicleDraw50CM(const VehicleFrameState49W&, const VehicleFrameBindings49W&,
    const VehicleDrawSelection50CM&, const VehicleDrawMesh50CM&,
    VehicleDrawBuffer50CM, VehicleDrawSubmission50CM&,
    VehicleDrawWorkspace50CM&, VehicleDrawDiagnostic50CM&) noexcept;
}
