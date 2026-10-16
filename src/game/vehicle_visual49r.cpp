#include "vehicle_visual49r_internal.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
using Index = SceneNodeIndex49P;
bool reject(VehicleVisualDiagnostic49R& d, VehicleVisualError49R e,
            std::uint32_t stage) noexcept {
    d = {e, stage}; return false;
}
bool validIndex(Index i, std::uint32_t count) noexcept {
    const auto value = static_cast<std::int32_t>(i);
    return value >= 0 && static_cast<std::uint32_t>(value) < count;
}
bool overlap(const void* a, std::size_t n, const void* b, std::size_t m) noexcept {
    if (n == 0 || m == 0) return false;
    const auto x = reinterpret_cast<std::uintptr_t>(a);
    const auto y = reinterpret_cast<std::uintptr_t>(b);
    return x <= y ? y - x < n : x - y < m;
}
bool admittedRecords(const VehicleVisualInput49R& input,
    const VehicleVisualParameters49R& parameters, const VisualSceneBindings49R& bindings,
    const SceneGraph49P& graph, const VehicleVisualState49R& state,
    const VisualShared49R& shared, const CoupledNoiseContext49M& noise,
    const VehicleVisualWorkspace49R& workspace) noexcept {
    const void* records[] = {graph.nodes, &state, &shared, &noise, &workspace};
    const std::size_t sizes[] = {graph.count * sizeof(SceneNode49P), sizeof state,
        sizeof shared, sizeof noise, sizeof workspace};
    for (unsigned i = 0; i < 5; ++i)
        for (unsigned j = i + 1; j < 5; ++j)
            if (overlap(records[i], sizes[i], records[j], sizes[j])) return false;
    return !overlap(&workspace, sizeof workspace, &input, sizeof input) &&
        !overlap(&workspace, sizeof workspace, &parameters, sizeof parameters) &&
        !overlap(&workspace, sizeof workspace, &bindings, sizeof bindings) &&
        !overlap(&workspace, sizeof workspace, &graph, sizeof graph);
}
}

bool updateVehicleVisual49R(const VehicleVisualInput49R& input,
    const VehicleVisualParameters49R& parameters, const VisualSceneBindings49R& bindings,
    const SceneGraph49P& graph, VehicleVisualState49R& state, VisualShared49R& shared,
    CoupledNoiseContext49M& noise, VehicleVisualWorkspace49R& workspace,
    VehicleVisualDiagnostic49R& diagnostic) noexcept {
    diagnostic = {};
    const bool bottomRight = bindings.bottomRight1C0 != Index::none;
    const bool bottomLeft = bindings.bottomLeft1C4 != Index::none;
    if (input.phase != VehicleVisualPhase49R::afterAnimationBeforeChariotAndPhysics ||
        input.vehicle834 != 0 || bottomRight != bottomLeft ||
        bindings.extra1C0Present != bottomRight || bindings.extra1C4Present != bottomLeft ||
        bindings.rotatingNodeCountB54 != 0)
        return reject(diagnostic, VehicleVisualError49R::context, 0x21c120);
    if (std::fegetround() != FE_TONEAREST)
        return reject(diagnostic, VehicleVisualError49R::rounding_mode, 0x21c120);
    if (!validateSceneGraph49P(graph))
        return reject(diagnostic, VehicleVisualError49R::graph, 0x21c120);
    const Index indices[] = {bindings.main190, bindings.container194,
                             bindings.right1B8, bindings.left1BC,
                             bindings.bottomRight1C0, bindings.bottomLeft1C4};
    const unsigned indexCount = bottomRight ? 6u : 4u;
    for (unsigned i = 0; i < indexCount; ++i) {
        if (!validIndex(indices[i], graph.count))
            return reject(diagnostic, VehicleVisualError49R::graph, 0x21c120);
        for (unsigned j = 0; j < i; ++j)
            if (indices[i] == indices[j])
                return reject(diagnostic, VehicleVisualError49R::graph, 0x21c120);
    }
    const auto& main = graph.nodes[static_cast<std::uint32_t>(bindings.main190)];
    const auto& container = graph.nodes[static_cast<std::uint32_t>(bindings.container194)];
    if (main.parent180 != Index::none || container.parent180 != bindings.main190 ||
        graph.nodes[static_cast<std::uint32_t>(bindings.right1B8)].parent180 != Index::none ||
        graph.nodes[static_cast<std::uint32_t>(bindings.left1BC)].parent180 != Index::none)
        return reject(diagnostic, VehicleVisualError49R::graph, 0x21c120);
    if (bottomRight &&
        (graph.nodes[static_cast<std::uint32_t>(bindings.bottomRight1C0)].parent180 != Index::none ||
         graph.nodes[static_cast<std::uint32_t>(bindings.bottomLeft1C4)].parent180 != Index::none))
        return reject(diagnostic, VehicleVisualError49R::graph, 0x21c120);
    if (!admittedRecords(input, parameters, bindings, graph, state, shared, noise, workspace))
        return reject(diagnostic, VehicleVisualError49R::records, 0x21c120);
    // No blanket finite scan: unused coefficients, copy-only W words and
    // store-only warm globals must remain unconsumed on their source paths.
    if (!std::isfinite(input.frameFactor) || input.frameFactor < 0.0f)
        return reject(diagnostic, VehicleVisualError49R::input, 0x21c120);
    std::memcpy(workspace.nodes, graph.nodes, graph.count * sizeof(SceneNode49P));
    workspace.state = state;
    workspace.shared = shared;
    workspace.noise = noise;
    const SceneGraph49P stagedGraph{workspace.nodes, graph.count};
    detail::VisualFrame49R frame{};
    if (!detail::prepareVisualContainer49R(stagedGraph, bindings, input.probe108,
            workspace.state, workspace.shared, frame, diagnostic)) return false;
    if (!detail::updateVisualResponses49R(input, parameters, bindings, frame, stagedGraph,
            workspace.state, workspace.shared, workspace.noise, diagnostic)) return false;
    std::memcpy(graph.nodes, workspace.nodes, graph.count * sizeof(SceneNode49P));
    state = workspace.state;
    shared = workspace.shared;
    noise = workspace.noise;
    diagnostic = {};
    return true;
}
}
