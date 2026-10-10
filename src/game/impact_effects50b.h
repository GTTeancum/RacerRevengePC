#pragma once
#include "scene_graph49p.h"
#include <cstddef>
#include <cstdint>
namespace rr::game {
struct ImpactEffectBinding50B { std::uint32_t id, flags1D4; }; // id0 means genuinely absent.
struct ImpactEffectModel50B {
    SceneNodeIndex49P node;
    std::uint32_t flags03;
    ImpactEffectBinding50B effect800, effect804, effect80C;
};
struct ImpactEffectsInput50B {
    // Source order1BC,1B8,1C4,1C0. Metadata must come from actual model/effect owners.
    ImpactEffectModel50B models[4];
    float damage7D0[4], timers820[5], bodyVelocity70[4];
    std::uint32_t effectD40;
    // Per-node original308F00(name38,"damage00")==0; ASCII case-insensitive
    // under the retained original character table. Immutable authored metadata.
    const std::uint8_t* damage00Matches;
    std::size_t nameCount;
};
enum class ImpactEffectKind50B : std::uint32_t { Start80, Stop84, Position15ACB0, Intensity19A810, Velocity15FBF0, ChannelA8 };
struct ImpactEffectEvent50B {
    ImpactEffectKind50B kind;
    std::uint32_t sourcePC, target, argument;
    float value[4];
};
constexpr std::size_t maxImpactEffectEvents50B=41;
struct ImpactEffectsWorkspace50B {
    SceneNode49P nextNodes[maxSceneNodes49P];
    ImpactEffectEvent50B events[maxImpactEffectEvents50B];
};
enum class ImpactEffectsStatus50B { applied, storage, graph, binding, arithmetic, capacity };
// Native223540 and224170/250/2D0: actual lookup and graph cache refresh, original
// shared308030 RNG transitions, ordered backend-owned effects. No gameplay
// transition is replaced by an event.225C50 remains a subsequent dependency.
// Backend executes start/stop/position/intensity/velocity/channel events in order
// before next frame and owns actual effect flags/presence. Backend must not mutate
// this model graph, damage/timers/RNG, or other effect objects. Every nonzero
// effect id must be distinct; unsupported aliasing rejects. Names are source-comparator
// authored matches, never guessed anchors. Dirty graph nodes refresh natively.
// All outputs including graph/RNG/events/count are atomic on rejection; workspace
// is scratch and can change. No allocation. Disjoint ranges required; no pointer
// overlap detection. FE_TONEAREST/strict binary32. Copy-only vector bits preserve.
ImpactEffectsStatus50B updateImpactEffects50B(const SceneGraph49P&,
    const ImpactEffectsInput50B&, std::uint32_t& sharedRandomState,
    ImpactEffectsWorkspace50B&, ImpactEffectEvent50B* events,
    std::size_t capacity, std::size_t& count) noexcept;
}
