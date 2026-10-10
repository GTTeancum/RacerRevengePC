#pragma once
#include "model_geometry49o.h"
#include "../game/scene_graph49p.h"
namespace rr::assets {
struct ModelRootName50BL {char text[128];bool known;};
// Source289648 writes only the first name byte. Cold fresh construction only;
// never call this to coerce an unknown/renamed live model into admission.
void initializeFreshModelRootName50BL(ModelRootName50BL&) noexcept;
struct EffectNameModel50BL {rr::game::SceneNodeIndex49P root;const ModelRootName50BL*rootName;PsgHierarchyView49O hierarchy;const rr::game::SceneNodeIndex49P*sourceNodes;std::size_t nodeCount;};
struct EffectNames50BL {
    std::uint8_t damage00[rr::game::maxSceneNodes49P];
    // Low words retain the existing <=64-node API; high words cover the native
    // roster forest without any shift by64 or truncation of source membership.
    std::uint64_t knownNodes,modelRoots,knownNodesHigh,modelRootsHigh;
};
inline bool effectModelRoot50BL(const EffectNames50BL& names,
    rr::game::SceneNodeIndex49P node) noexcept {
    const auto index=static_cast<std::uint32_t>(node);
    return index<rr::game::maxSceneNodes49P&&
        ((index<64?names.modelRoots:names.modelRootsHigh)&(std::uint64_t(1)<<(index%64)));
}
// Up to4 parentless model trees. Complete unique source-node mapping required;
// PSG names are current immutable names under the fresh, unrenamed admission.
// Root names must explicitly be known. Source308F00 ASCII comparison; unrelated
// roots are unbound and MUST NOT be searched using this table. Caller checks
// modelRoots for every effect model; changed topology/names require rebinding.
// Borrowed admitted PSG views; disjoint output; false preserves it. No heap.
bool buildEffectNames50BL(const rr::game::SceneGraph49P&,const EffectNameModel50BL*,
 std::size_t,EffectNames50BL&) noexcept;
}
