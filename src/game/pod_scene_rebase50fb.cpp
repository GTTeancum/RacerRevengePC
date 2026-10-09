#include "pod_scene_rebase50fb.h"
#include <cfenv>
#include <cstring>

namespace rr::game {
namespace {
bool valid(SceneNodeIndex49P node, std::uint32_t count) noexcept {
    const auto index = static_cast<std::int32_t>(node);
    return index >= 0 && static_cast<std::uint32_t>(index) < count;
}
bool nullcg(const char* name) noexcept {
    constexpr char expected[] = "nullcg";
    for (unsigned i = 0; i < 6; ++i) {
        const char c = name[i] >= 'A' && name[i] <= 'Z' ?
            static_cast<char>(name[i] + ('a' - 'A')) : name[i];
        if (c != expected[i]) return false;
    }
    return name[6] == 0;
}
}
bool rebaseColdPodScene50FB(const SceneGraph49P& graph,
    const PodSceneRebaseModel50FB (&models)[3],
    PodSceneRebaseWorkspace50FB& workspace,
    ModelBoundsResult49O (&bounds)[3],
    PodSceneRebaseDiagnostic50FB& diagnostic) noexcept {
    diagnostic = {};
    auto reject = [&](PodSceneRebaseError50FB error) noexcept {
        diagnostic.error = error;
        return false;
    };
    if (std::fegetround() != FE_TONEAREST)
        return reject(PodSceneRebaseError50FB::context);
    if (!graph.count || !validateSceneGraph49P(graph))
        return reject(PodSceneRebaseError50FB::graph);
    bool claimed[maxSceneNodes49P]{};
    // Admit all descriptors before making even staged source changes.
    for (unsigned model = 0; model < 3; ++model) {
        diagnostic.model = model;
        const auto& input = models[model];
        if (!valid(input.root, graph.count) || !input.sourceNodes ||
            !input.sourceNodeCount || input.sourceNodeCount >= maxSceneNodes49P ||
            input.sourceNodeCount != input.hierarchy.nodeCount)
            return reject(PodSceneRebaseError50FB::binding);
        const auto root = static_cast<std::uint32_t>(input.root);
        if (claimed[root] || graph.nodes[root].parent180 != SceneNodeIndex49P::none ||
            (graph.nodes[root].sceneFlags190 & 0x14u))
            return reject(PodSceneRebaseError50FB::binding);
        claimed[root] = true;
        for (std::uint32_t n = 0; n < input.sourceNodeCount; ++n) {
            diagnostic.sourceNode = n;
            assets::PsgNode49O authored{};
            if (!assets::readPsgNode49O(input.hierarchy, n, authored) ||
                (n == 0 ? authored.parent != -1 || !nullcg(authored.name) :
                          authored.parent < 0 || static_cast<std::uint32_t>(authored.parent) >= n))
                return reject(PodSceneRebaseError50FB::authoring);
            const auto index = input.sourceNodes[n];
            if (!valid(index, graph.count)) return reject(PodSceneRebaseError50FB::binding);
            const auto ni = static_cast<std::uint32_t>(index);
            if (claimed[ni] || graph.nodes[ni].parent180 !=
                (n == 0 ? input.root : input.sourceNodes[authored.parent]) ||
                !(graph.nodes[ni].sceneFlags190 & 4u))
                return reject(PodSceneRebaseError50FB::binding);
            claimed[ni] = true;
        }
    }
    std::memcpy(workspace.nodes, graph.nodes, graph.count * sizeof(SceneNode49P));
    const SceneGraph49P staged{workspace.nodes, graph.count};
    for (unsigned model = 0; model < 3; ++model) {
        diagnostic.model = model;
        diagnostic.sourceNode = 0;
        const auto& input = models[model];
        const auto cg = input.sourceNodes[0];
        auto matrix = workspace.nodes[static_cast<std::uint32_t>(cg)].localC0;
        // 217478 / 2174F4 / 217570 ->28AB90 copies only zero434630 XYZ.
        // setLocalMatrix preserves the other13 words byte-for-byte and has
        // the identical28C4F0/ready-gated289A20 represented flag effects.
        matrix.v[12] = matrix.v[13] = matrix.v[14] = 0.0f;
        if (!setLocalMatrix49P(staged, cg, matrix))
            return reject(PodSceneRebaseError50FB::graph);
        workspace.nodes[static_cast<std::uint32_t>(cg)].sceneFlags190 &= ~0x20u;
        workspace.nodes[static_cast<std::uint32_t>(input.root)].sceneFlags190 &= ~0x40u;
        for (std::uint32_t n = 0; n < input.sourceNodeCount; ++n) {
            diagnostic.sourceNode = n;
            assets::PsgNode49O authored{};
            if (!assets::readPsgNode49O(input.hierarchy, n, authored))
                return reject(PodSceneRebaseError50FB::authoring);
            auto& target = workspace.boundsNodes[n];
            target.node = input.sourceNodes[n];
            // The current graph supplies animated locals; PSG supplies only
            // immutable box data. Never restore authored translations here.
            for (unsigned axis = 0; axis < 3; ++axis) {
                target.center140.v[axis] = authored.bounds[axis];
                target.halfExtent150.v[axis] = authored.bounds[3 + axis];
            }
            // Do not repair an inconsistent dirty-parent/ready-child input.
            if (workspace.nodes[static_cast<std::uint32_t>(target.node)].sceneFlags190 & 1u)
                return reject(PodSceneRebaseError50FB::cache);
        }
        // 2174C8 / 217544 / 2175C0 ->28E110 sees dirty80 and cleared40,
        // selecting model virtual+90 (26F4B0), not the generic world-only path.
        if (!refreshResetModelBounds50BA(staged, input.root, workspace.boundsNodes,
            input.sourceNodeCount, workspace.bounds, workspace.result[model]))
            return reject(PodSceneRebaseError50FB::bounds);
    }
    std::memcpy(graph.nodes, workspace.nodes, graph.count * sizeof(SceneNode49P));
    std::memcpy(bounds, workspace.result, sizeof workspace.result);
    diagnostic = {};
    return true;
}
}
