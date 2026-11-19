#pragma once
#include <cstddef>
namespace rr::game {
// The ONE definition of the common trigger reader's capacity (the original's
// trigger reader walks [Triggers] NumberOfTriggers and TriggerN /
// TriggerNActionK of whatever TOB the course holds; the port's store is sized
// once for the largest stock set: TC 22 triggers; GB trigger 18 authors the
// most actions, 5). The common reader (tests/native/sc_trigger_authoring50la.h
// count / maxActions) and every capacity-sized native trigger group
// (native_trigger_group50{tb,tc,mc,gb}.h) reference these; nothing else
// defines a trigger capacity. Kept dependency-free so the ~152-TU group headers
// can use it. GA's own group (native_trigger_group50ga.h, 13) is unchanged.
inline constexpr std::size_t nativeTriggerCapacity50TG=22;
inline constexpr std::size_t nativeTriggerActionSlots50TG=5;
} // namespace rr::game
