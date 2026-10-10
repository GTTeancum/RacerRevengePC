#pragma once
#include "impact_effects50b.h"
namespace rr::game {
struct PlainEffectSlot50Q { std::uint32_t id; std::uint8_t flags1D4; };
struct PlainEffectOwners50Q {
 std::uint32_t attachment334;
 PlainEffectSlot50Q slots[12]; // 800[0..3],804[0..3],80C[0..3], in source order.
};
enum class PlainEffectStatus50Q { Complete,UnsupportedAttached,AliasedOwners };
struct PlainEffectResult50Q {
 PlainEffectStatus50Q status=PlainEffectStatus50Q::Complete;
 std::uint32_t count=0;
 ImpactEffectEvent50B events[12]{};
};
// Explicit ordered backend-call boundary for228390. Flags are current AFTER
// preceding effects have been consumed. Backend Stop84 must not mutate another
// slot's flags or the arrays; unique nonzero owners enforce no self-alias reread.
// Attached22A620 is not substituted. No allocation or caller mutation.
PlainEffectResult50Q inspectPlainEffectTail50Q(const PlainEffectOwners50Q&) noexcept;
}
