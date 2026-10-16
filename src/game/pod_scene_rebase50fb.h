#pragma once
#include "reset_model_bounds50ba.h"
#include "../assets/model_geometry49o.h"

namespace rr::game {
// Caller order is source 217440 order: Chariot, right engine, left engine.
// Optional fourth/fifth models follow: bottom right, bottom left.
struct PodSceneRebaseModel50FB {
    SceneNodeIndex49P root = SceneNodeIndex49P::none;
    assets::PsgHierarchyView49O hierarchy{};
    const SceneNodeIndex49P* sourceNodes = nullptr;
    std::uint32_t sourceNodeCount = 0;
};
enum class PodSceneRebaseError50FB : std::uint8_t {
    none, context, graph, binding, authoring, cache, bounds
};
struct PodSceneRebaseDiagnostic50FB {
    PodSceneRebaseError50FB error = PodSceneRebaseError50FB::none;
    std::uint32_t model = 0, sourceNode = 0;
};
struct PodSceneRebaseWorkspace50FB {
    SceneNode49P nodes[maxSceneNodes49P];
    ResetBoundNode50BA boundsNodes[maxSceneNodes49P];
    ResetBoundsWorkspace50BA bounds;
    ModelBoundsResult49O result[5];
};
// Cold setup of the source first-base 217440 represented scene effects.
// Call AFTER capture of pre-animation geometry/target/inertia and any reset
// bound inputs, and BEFORE declaring the first-base B9C bit80 precondition.
// Requires three or five present fresh-model box roots (own04/sphere10 clear), exact
// complete PSG source-node mappings and coherent caches.
// Preserves all bodies, authored parameters, model-root matrices, animation
// locals other than nullcg XYZ, and unrelated scene flags. Source28AB90's
// ready-gated invalidation and 28E110's reached model-box/world recomputation
// use the existing49P/50BA/49O operations. No frame49W admission is widened.
// Graph/output remain unchanged on rejection. Scratch must be disjoint.
bool rebaseColdPodScene50FB(const SceneGraph49P& graph,
    const PodSceneRebaseModel50FB* models, std::uint32_t modelCount,
    PodSceneRebaseWorkspace50FB& workspace, ModelBoundsResult49O* bounds,
    PodSceneRebaseDiagnostic50FB& diagnostic) noexcept;
// Historical three-model client remains source-compatible.
bool rebaseColdPodScene50FB(const SceneGraph49P& graph,
    const PodSceneRebaseModel50FB (&models)[3],
    PodSceneRebaseWorkspace50FB& workspace,
    ModelBoundsResult49O (&bounds)[3],
    PodSceneRebaseDiagnostic50FB& diagnostic) noexcept;
}
