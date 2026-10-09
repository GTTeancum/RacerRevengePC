#include "chariot_projection49p.h"

namespace rr::game {
bool projectChariotPhysicalBranch49P(const SceneGraph49P& graph,
                                    const ChariotProjectionInput49P& input) noexcept {
    if (input.phase != ChariotProjectionPhase49P::beforePhysicalCopy21B9A4 ||
        input.suppliedState != ChariotSuppliedState49P::completeSceneStateThroughChariotPrefix ||
        input.vehicle838 == 0 || input.vehicle83C != 0 ||
        !validateSceneGraph49P(graph)) return false;
    const auto source = static_cast<std::int32_t>(input.secondaryPhysical19C);
    const auto destination = static_cast<std::int32_t>(input.chariotModel1B4);
    if (source < 0 || destination < 0 || source == destination ||
        static_cast<std::uint32_t>(source) >= graph.count ||
        static_cast<std::uint32_t>(destination) >= graph.count) return false;
    if (graph.nodes[source].parent180 != SceneNodeIndex49P::none ||
        graph.nodes[destination].parent180 != SceneNodeIndex49P::none) return false;
    //21B9BC loads19C,21B9C0 loads1B4,21B9C8 addsC0 to the source pointer in
    // the call delay slot.28B490 owns all copying and subsequent cache effects.
    return setLocalMatrix49P(graph, input.chariotModel1B4, graph.nodes[source].localC0);
}
}
