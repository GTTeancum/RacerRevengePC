#pragma once
#include "effect_stop50bd.h"
#include "impact_effects50b.h"
namespace rr::game {
enum class EffectProfile50BE : std::uint32_t {Particle19A5E0,Ordinary15F100,Deferred15E800,Fire15DE00};
struct EffectOwner50BE {std::uint32_t id;EffectProfile50BE profile;EffectControlState50BC state;std::uint32_t mode1D8,duration1E4;};
constexpr std::size_t maxEffectOwners50BE=16,maxEffectCommands50BE=64;
struct EffectDispatchWorkspace50BE {EffectOwner50BE next[maxEffectOwners50BE];};
enum class EffectDispatchStatus50BE {Complete,Storage,Binding,UnsupportedCommand,UnsupportedDuration};
struct EffectDispatchResult50BE {EffectDispatchStatus50BE status;std::size_t index,commands,deferredStops;};
// Concrete native owner-state consumption of ordered50B events. Profiles must
// come from proved actual owner construction, never guessed from the event.
// Does not create owners, run particles, draw effects, or handle D40 ChannelA8.
// Disjoint caller arrays/scratch, unique nonzero IDs, serial access. All owner
// state commits atomically; a failed batch leaves owners unchanged. No hot heap.
EffectDispatchResult50BE consumeEffectControls50BE(const ImpactEffectEvent50B*,std::size_t,
 EffectOwner50BE*,std::size_t,EffectDispatchWorkspace50BE&) noexcept;
}
