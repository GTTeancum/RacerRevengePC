#pragma once
#include "vehicle_frame49w.h"
#include "../assets/native_asset48b.h"

namespace rr::game {
constexpr std::uint32_t podCableCount50FD = 2;
constexpr std::uint32_t podCableSampleCount50FD = 9;
constexpr std::uint32_t podCableVertexCount50FD = 36;
constexpr std::uint32_t podCableIndexCount50FD = 96;
constexpr std::uint32_t maxPodCableCount50FD = 4;
constexpr std::uint32_t podCableVerticesPerCable50FD = 18;
constexpr std::uint32_t podCableIndicesPerCable50FD = 48;
constexpr std::uint32_t maxPodCableVertexCount50FD = 72;
constexpr std::uint32_t maxPodCableIndexCount50FD = 192;

// Primary left/right, then optional bottom left/right (source slots0..3).
// Resolve the original named drawcable attachment, falling
// back to chariotcable only when it is absent. Engine endpoints are the named
// enginecable nodes, NOT the animated engine model roots or the hand targets.
struct PodCableBindings50FD {
    SceneNodeIndex49P chariot[4]{SceneNodeIndex49P::none, SceneNodeIndex49P::none,
        SceneNodeIndex49P::none, SceneNodeIndex49P::none};
    SceneNodeIndex49P engine[4]{SceneNodeIndex49P::none, SceneNodeIndex49P::none,
        SceneNodeIndex49P::none, SceneNodeIndex49P::none};
    std::uint32_t cableCount = podCableCount50FD;
};
struct PodCableMesh50FD {
    assets::Vertex48B vertices[maxPodCableVertexCount50FD]{};
    std::uint32_t indices[maxPodCableIndexCount50FD]{};
    assets::Bounds48B bounds{};
    std::uint32_t cableCount = podCableCount50FD;
};
struct PodCableWorkspace50FD {
    SceneNode49P nodes[vehicleFrameNodeCount49W];
    PodCableMesh50FD next;
    Vector3_48C samples[maxPodCableCount50FD][podCableSampleCount50FD];
};
enum class PodCableError50FD : std::uint8_t {
    none, phase, unsupportedState, roundingMode, graph, binding, input, arithmetic
};
struct PodCableDiagnostic50FD {
    PodCableError50FD error = PodCableError50FD::none;
    std::uint32_t cable = 0, sample = 0;
};

// Native source-derived ordinary connected two-cable or four-engine path:
// 214120 selects nine samples;173EB0 Hermite basis/tangent6.5/UV repeat10;
// 224720 obtains current authored attachments and speed-dependent tangents;
// 1743E0 emits view-facing strips with source one-sided width0.25.
// cameraForward is the WORLD-space view axis (the camera view matrix column2),
// finite and nonzero. Its sign selects the same side as the original ribbon.
// Each sample's even vertex is on the source centerline; odd is center+width.
// U=0/1 and V=10*t require repeat sampling of the real textures/cable.pst.
// Vertex colors are explicit neutral modulation, not recovered shader colors.
// Positions are already world-space. Use opaque, two-sided, textured drawing.
// cableCount is2 for primary engines, or4 with both optional bottom engines.
// Only cableCount*18 vertices and cableCount*48 indices are active. Inactive
// output storage is zeroed; bounds cover active vertices only. Bottom cable
// chariot endpoints may share the primary ordinary anchors, as at211D0C/211D50.
//
// Only vehicleState40==0 is represented: detached/wreckage visual state and
// ghost materials are deliberately not substituted with attached cables.
// Ready/PostGraphics scenes accepted, matching current50CM graphics roots.
// This never substitutes the later physical body matrices or modifies scene,
// caches, body state, noise, RNG, or source assets. Copies/refreshes scene nodes in
// caller workspace. Finite strict binary32/FE_TONEAREST; no hot allocation.
// True-zero camera/segment cross product produces zero width as in the source;
// no arbitrary fallback axis. Positive subnormal squared norms reject.
// False preserves output; workspace/diagnostic may change. All arguments must
// have distinct storage, including the workspace's staging mesh and output.
bool buildPodCables50FD(const VehicleFrameState49W&,
    const VehicleFrameBindings49W&, const PodCableBindings50FD&,
    const Vector3_48C& cameraForward, PodCableMesh50FD&,
    PodCableWorkspace50FD&, PodCableDiagnostic50FD&) noexcept;
}
