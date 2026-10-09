#pragma once
#include "../assets/model_geometry49o.h"
#include "../assets/model_bindings49o.h"
#include "scene_graph49p.h"
#include <cstdint>

namespace rr::game {
// Explicit original model+18 word. The admitted 1559F0 operation clears it.
struct ModelMotionState49Q { std::uint32_t model18; };
// Caller-owned transaction scratch, disjoint from the graph and asset bytes.
// Scratch contents are unspecified after either success or rejection.
struct ModelMotionWorkspace49Q { SceneNode49P nodes[maxSceneNodes49P]; };
enum class ModelMotionError49Q : std::uint32_t {
    none, rounding_mode, input, topology, binding, sample_time, arithmetic
};
struct ModelMotionDiagnostic49Q {
    ModelMotionError49Q error;
    std::uint32_t track;
    std::uint32_t channel; // 0=rotation, 1=position; zero outside a track.
};
// Bounded original1559F0 single-motion route, mirror=0 and mask=0 as at21A000.
// Successfully admitted, unchanged49O views/bytes must remain alive. Bindings
// explicitly map original rows to caller graph indices; no hierarchy-base
// arithmetic, guessed defaults, or use of binding+38/+3C is performed.
// Duration*fraction is sampled directly: no paired49O blend/endpoint shortcut.
// All rotations execute before all positions, including repeated target nodes.
// Setter28A580 writes basis XYZ, zeros basis fourth lanes, preserves translation;
// setter28AC10 writes translation XYZ only. Each track performs original80
// ancestor marking and ready01-gated descendant invalidation through equivalent
// 49P setters. Every world cache and topology link remains unchanged, and a
// dirty node with ready descendants is intentionally not normalized.
// Requires FE_TONEAREST,1..64 coherent graph nodes with all local/world lanes
// finite, bindings1..64, finite fraction[0,1], >=2 keys per consumed track,
// inclusive covered sample times, and finite consumed/intermediate arithmetic.
// Retains49O original-software trig tables/cold acos padding and separate
// nearest/even binary32 operations; no blanket physical-console FCR claim.
// Graph, state, workspace and immutable inputs must be disjoint. Serial use.
// Fixed-record admission rejects workspace overlap with the graph, state,
// graph/view/bindings records or asset byte span, and state/graph overlap.
// This is not object ownership or allocation-failure/capacity validation.
// False leaves graph and model18 untouched; diagnostic is always assigned.
// Zero allocation; no mirror/mask filtering, IK, scale restoration, world-cache
// refresh, model ownership, playback clock, or full-animation service.
bool sampleModelMotion49Q(const assets::MotionView49O& motion,
    const assets::MotionNodeBindings49O& bindings, float frameFraction,
    const SceneGraph49P& graph, ModelMotionState49Q& state,
    ModelMotionWorkspace49Q& workspace, ModelMotionDiagnostic49Q& diagnostic) noexcept;
}
