#include "model_palette49s.h"
#include <cfenv>
#include <cstring>

namespace rr::game {
namespace {
bool validIndex(SceneNodeIndex49P index, std::uint32_t count) noexcept {
    const auto value = static_cast<std::int32_t>(index);
    return value >= 0 && static_cast<std::uint32_t>(value) < count;
}
bool reject(ModelPaletteDiagnostic49S& diagnostic, ModelPaletteError49S error) noexcept {
    diagnostic.error = error;
    return false;
}
bool getWorld(const SceneGraph49P& graph, SceneNodeIndex49P index) noexcept {
#ifdef RR_MODEL_PALETTE49S_GETTER
    // Test-only call-order evidence; absent from the ordinary production object.
    RR_MODEL_PALETTE49S_GETTER(index);
#endif
    return refreshSceneWorld49P(graph, index);
}
}
bool projectModelPalette49S(const SceneGraph49P& graph,
    const ModelPaletteInput49S& input, ModelPalette49S& output,
    ModelPaletteWorkspace49S& workspace, ModelPaletteDiagnostic49S& diagnostic) noexcept {
    diagnostic.error = ModelPaletteError49S::none;
    if (input.phase != ModelPalettePhase49S::selectedRenderPreparation ||
        input.suppliedState != ModelPaletteSceneState49S::completeCurrentScene ||
        (input.modelFlags191 & 1u) == 0 || std::fegetround() != FE_TONEAREST)
        return reject(diagnostic, ModelPaletteError49S::unsupportedContext);
    if (graph.count == 0 || !validateSceneGraph49P(graph))
        return reject(diagnostic, ModelPaletteError49S::invalidGraph);
    if (!validIndex(input.modelRoot, graph.count))
        return reject(diagnostic, ModelPaletteError49S::invalidRoot);
    if (input.sourceNodes == nullptr || input.sourceNodeCount == 0 ||
        input.sourceNodeCount > maxSceneNodes49P)
        return reject(diagnostic, ModelPaletteError49S::invalidSourceTable);
    for (std::uint32_t source = 0; source < input.sourceNodeCount; ++source)
        if (!validIndex(input.sourceNodes[source], graph.count))
            return reject(diagnostic, ModelPaletteError49S::invalidSourceTable);
    if (input.paletteIds == nullptr || input.paletteCount == 0 ||
        input.paletteCount > maxModelPaletteEntries49S)
        return reject(diagnostic, ModelPaletteError49S::invalidPalette);
    for (std::uint32_t entry = 0; entry < input.paletteCount; ++entry)
        if (input.paletteIds[entry] >= input.sourceNodeCount)
            return reject(diagnostic, ModelPaletteError49S::invalidPalette);

    std::memcpy(workspace.nodes, graph.nodes, graph.count * sizeof(SceneNode49P));
    const SceneGraph49P staged{workspace.nodes, graph.count};
    const auto root = static_cast<std::uint32_t>(input.modelRoot);
    // Original2412E4..2412FC; a ready model root is not called again here.
    if ((staged.nodes[root].sceneFlags190 & 1u) == 0 &&
        !getWorld(staged, input.modelRoot))
        return reject(diagnostic, ModelPaletteError49S::sceneArithmetic);
    // Original2414F8..241518, before any palette-node getter.
    std::memcpy(&workspace.palette.rootWorld, &staged.nodes[root].world100,
                sizeof(BodyMatrix48G));
    for (std::uint32_t entry = 0; entry < input.paletteCount; ++entry) {
        // Original241540..241550, using the source-index pointer table.
        const auto node = input.sourceNodes[input.paletteIds[entry]];
        // Original24155C executes for every admitted nonnull entry.
        if (!getWorld(staged, node))
            return reject(diagnostic, ModelPaletteError49S::sceneArithmetic);
        std::memcpy(&workspace.palette.nodeWorld[entry],
                    &staged.nodes[static_cast<std::uint32_t>(node)].world100,
                    sizeof(BodyMatrix48G));
    }
    workspace.palette.entryCount = input.paletteCount;
    std::memcpy(graph.nodes, workspace.nodes, graph.count * sizeof(SceneNode49P));
    std::memcpy(&output.rootWorld, &workspace.palette.rootWorld, sizeof(BodyMatrix48G));
    std::memcpy(output.nodeWorld, workspace.palette.nodeWorld,
                input.paletteCount * sizeof(BodyMatrix48G));
    output.entryCount = workspace.palette.entryCount;
    return true;
}
}
