#pragma once
#include "vehicle_frame49w.h"
#include "model_palette49s.h"
#include "rigid_vertices49s.h"
#include "../assets/import_scene48d.h"
#include "../assets/native_asset48b.h"
#include "../assets/world_normals50hk.h"
#include "original_racer_material50km.h"
#include "original_racer_model_view50la.h"
#include <array>
#include <cmath>
#include <cstdint>

namespace rr::game {
class NativeGcSelectedLightsCandidate;
class OriginalRacerModelPackets50LA;
constexpr std::uint32_t maxVehicleDrawSpans50CM = 64;
constexpr std::uint32_t defaultVehicleDrawComponents50CM = 3;
constexpr std::uint32_t maxVehicleDrawComponents50CM = 5;
enum class VehicleDrawComponent50CM : std::uint8_t {
    chariot, leftEngine, rightEngine, bottomLeftEngine, bottomRightEngine
};
constexpr bool validVehicleDrawComponentCount50CM(std::uint32_t count) noexcept {
    return count >= defaultVehicleDrawComponents50CM && count <= maxVehicleDrawComponents50CM;
}

// Cold exact normal decode result, in the prepared mesh's flattened order.
// Invalid entries are retained: only a selected source program rejects them.
struct VehicleDrawCompressedNormal50LA {
    original_racer_lighting50KM::CompressedNormal value{};
    bool valid = false;
};
namespace vehicle_draw_detail50LA {
// Exact per-call memoization of successful native lighting/UV results. Clear
// before consuming a new palette or camera; collisions only replace entries.
// No cross-frame readiness, source ownership or arithmetic result is inferred.
inline std::uint32_t normalKey(const original_racer_lighting50KM::CompressedNormal& n,
    std::uint8_t bone = 0) noexcept {
    return std::uint32_t(static_cast<std::uint8_t>(n[0])) |
        (std::uint32_t(static_cast<std::uint8_t>(n[1])) << 8) |
        (std::uint32_t(static_cast<std::uint8_t>(n[2])) << 16) |
        (std::uint32_t(bone) << 24);
}
template<class Value> struct NormalResultCache {
    struct Entry { std::uint32_t key = 0; Value value{}; bool valid = false; };
    std::array<Entry, 256> entries{};
    void clear() noexcept { for (auto& entry : entries) entry.valid = false; }
    static std::uint32_t index(std::uint32_t key) noexcept {
        return (key * 0x9e3779b9u) >> 24;
    }
    bool find(std::uint32_t key, Value& value) const noexcept {
        const auto& entry = entries[index(key)];
        if (!entry.valid || entry.key != key) return false;
        value = entry.value; return true;
    }
    void insert(std::uint32_t key, const Value& value) noexcept {
        auto& entry = entries[index(key)];
        entry.value = value; entry.key = key; entry.valid = true;
    }
};
// The caller admits authoredQ and FE_TONEAREST first. Keep the same original
// double division, integer/range check and exact re-expansion comparison used
// by the frame loops; this is neither normal quantization nor normalization.
inline bool decodeCompressedNormal(const std::array<double, 3>& expandedNormal,
    double authoredQ, original_racer_lighting50KM::CompressedNormal& normal) noexcept {
    for (unsigned axis = 0; axis < 3; ++axis) {
        const double expanded = expandedNormal[axis];
        if (!std::isfinite(expanded)) return false;
        const double raw = expanded / authoredQ;
        if (!std::isfinite(raw) || raw < -128.0 || raw > 127.0 ||
            std::trunc(raw) != raw || raw * authoredQ != expanded) return false;
        normal[axis] = static_cast<std::int8_t>(raw);
    }
    return true;
}
}

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
    // Immutable semantic normals in the same flattened order, when prepared
    // from Scene48D. Keeps authored normals/splits outside Vertex48B's ABI.
    const assets::Scene48D* semanticScene = nullptr;
    // Optional immutable exact decode owned beside semanticScene. The owner
    // never moves/reloads either array; copying this descriptor only borrows it.
    // A missing/mismatched view uses the original live decode instead. Entries
    // must remain unchanged and have compressedNormalCount50LA live elements.
    const VehicleDrawCompressedNormal50LA* compressedNormals50LA = nullptr;
    const assets::Scene48D* compressedNormalScene50LA = nullptr;
    std::uint32_t compressedNormalCount50LA = 0;
    double compressedNormalQ50LA = 0;
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
    assets::WorldNormal50HK* sourceNormals = nullptr;
    std::uint32_t normalCapacity = 0;
};
struct VehicleDrawSourceColors50LA {
    // Cold immutable semantic-material table: 0 retains legacy template
    // colors; 1 requests original compressed diffuse RGBA for a body slot.
    // The actual material owner must establish that slot's source program.
    const std::uint8_t* materialModes = nullptr;
    std::uint32_t materialCount = 0;
    const NativeGcSelectedLightsCandidate* selectedLights = nullptr;
    // Opt-in source geometry for this actual engine's multi-node dual-map route.
    // Retains model/root inputs only; presentation supplies the current camera.
    bool prepareSpecular50LA = false;
    // Optional actual240740 owner, borrowed only during this preparation.
    // Its241280 matrix slots feed positions/normals/colors immediately; no
    // mutable packet pointer is retained by the completed draw descriptor.
    OriginalRacerModelPackets50LA* modelPackets50LA = nullptr;
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
    // Optional source-derived world normals, exact vertex order. Zero normals
    // request geometric fallback; neither positions nor RGB/alpha are altered.
    const assets::WorldNormal50HK* sourceNormals = nullptr;
    std::uint32_t sourceNormalCount = 0;
    // Exact immutable cold mask and selected publication used to generate
    // these vertex colors. Renderer must join all identities to its retained
    // material table and selected lights; stopped geometry keeps this receipt.
    const std::uint8_t* sourceColorMaterialModes50LA = nullptr;
    std::uint32_t sourceColorMaterialCount50LA = 0;
    std::uintptr_t sourceColorOwner50LA = 0;
    std::uint64_t sourceColorPublication50LA = 0, sourceColorGraphicsSequence50LA = 0;
    std::uint32_t sourceColorRacerSlot50LA = 0;
    // Exact source model/root world, aggregate160/170/198 and localC0/F0
    // translation. These are not the display AABB or its estimated origin.
    // Retaining them permits current-camera UV/distance refresh while stopped.
    bool sourceSpecularGeometry50LA = false;
    BodyMatrix48G sourceRootWorld50LA{};
    OriginalRacerAggregateBounds50LA sourceAggregateBounds50LA{};
    std::array<float, 3> sourcePosition50LA{};
};
struct VehicleDrawWorkspace50CM {
    SceneNode49P nodes[vehicleFrameNodeCount49W];
    ModelPalette49S palette;
    ModelPaletteWorkspace49S paletteWorkspace;
    RigidPosition49S positions[maxRigidVertices49S];
    assets::WorldNormal50HK normals[maxRigidVertices49S];
    original_racer_lighting50KM::DiffuseRgbaProgramInputs sourceColorPalette50LA[maxRigidPalette49S];
    original_racer_lighting50KM::DiffuseVertexRgba sourceColors50LA[maxRigidVertices49S];
    vehicle_draw_detail50LA::NormalResultCache<original_racer_lighting50KM::DiffuseVertexRgba>
        sourceColorCache50LA;
};
enum class VehicleDrawError50CM : std::uint8_t {
    none, storage, scene, geometry, vertexTemplate, phase, binding, palette, vertices, normals, sourceColors,
    sourceSpecular
};
struct VehicleDrawDiagnostic50CM {
    VehicleDrawError50CM error = VehicleDrawError50CM::none;
    std::uint32_t item = 0;
    ModelPaletteDiagnostic49S palette{};
    RigidVertexDiagnostic49S vertices{};
};

// Cold, allocation-free flattening of 1..8192 semantic LOD0 local vertices.
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
// the active scene forest, then runs the real49S palette and exact49S diagnostic
// rigid arithmetic once into disposable private scratch before publication.
// Canonical graph, frame caches, physics and RNG are never mutated. In
// particular, later physical poses never replace the earlier graphics roots.
//
// On success output is directly compatible with main48b.cpp: Vertex48B stride,
// R32_UINT indices, triangle-list SceneSpan48D ranges and baseVertex=0. Positions
// are already world-space; do not apply rootWorld or another model transform.
// UV/index/span/material bytes remain those of the prepared source. Color
// bytes also remain unchanged unless sourceColors explicitly selects a slot.
// An optional separate sourceNormals destination receives authored normals
// transformed by the same per-node world palette. Singular transforms produce
// zero, requesting geometric lighting fallback; no normals are smoothed.
// Optional sourceColors selects only actual engine body slots. Its
// full semantic material table must contain only 0/1 and at least one 1; the
// real ready selected packet must contain this component. The current node
// palette and exactly reversible signed-byte source normals generate all RGBA
// lanes before any output mutation. This route never consumes sourceNormals,
// inverse-transpose/normalized normals, or imported RGB. Unpaired presentation
// keeps opaque alpha; only an admitted engine pair consumes generated alpha.
// prepareSpecular50LA admits engine components only and requires a multi-node source
// model whose extra owner has no authored box or sphere route. It recomputes
// original aggregate bounds from this same current graph's locals/flags,
// semantic boxes and source-index parent map before publishing model inputs,
// including the radius consumed by per-presentation model distance/fog flags.
// No device commands, camera, material selection, visibility, LOD scheduling,
// packet execution or original VU-shader parity is supplied.
//
// Single-threaded, fixed caller storage, no allocation/I/O/callback. Inherits
// strict FP/FE_TONEAREST requirements of49S. Inputs, their backing arrays,
// destination vertices, output, workspace and diagnostic are distinct live
// storage with stable lifetimes. Descriptor/backing-array validity and overlap
// are caller preconditions; prepare once and do not mutate the prepared mesh.
// False preserves destination vertices and output; workspace/diagnostic and
// optional model packet preparation slots may change. Submit only on true.
// The packet path owns matrix payloads only, not flags/LOD/FIFO acceptance.
// For a multi-model vehicle, prepare all active outputs
// successfully before submitting any of them to avoid a mixed-frame draw.
bool buildVehicleDraw50CM(const VehicleFrameState49W&, const VehicleFrameBindings49W&,
    const VehicleDrawSelection50CM&, const VehicleDrawMesh50CM&,
    VehicleDrawBuffer50CM, VehicleDrawSubmission50CM&,
    VehicleDrawWorkspace50CM&, VehicleDrawDiagnostic50CM&,
    const VehicleDrawSourceColors50LA* sourceColors = nullptr) noexcept;
}
