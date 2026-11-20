#pragma once
#include <cstddef>
namespace rr::game {
// Common rewrite (R5, PLAN B-C13 / 2.2 storage rule): the ONE trigger store of
// the common race path (NativeTriggerGroup50MC, course_trigger_sources50CT) is
// sized once from the TOB's own [Triggers] NumberOfTriggers and each trigger's
// NumberOfActions (274E90 / 2A00A0 read whatever the course archive holds).
// Nothing here sizes that store any more.
//
// RECEIPTS ONLY (stock_course_receipts50CR rows "ALL triggers.capacity" and
// "ALL triggers.action_slots"): the largest stock trigger set (TC, 22) and the
// most actions any stock trigger authors (GB trigger 18, 5).
inline constexpr std::size_t stockLargestTriggerSetReceipt50TG=22;
inline constexpr std::size_t stockLargestActionSetReceipt50TG=5;
// PORT STAND-IN (flagged, retiring): the four legacy per-course trigger groups
// native_trigger_group50{gb,tb,tc}.h still size fixed arrays from these two
// names. They stay only because the shared query kernels (vehicle_live_query50eq.cpp,
// indexed_type2_contact50cc.cpp, static_type2_producer49x.*, source_indexed_type2_query50mb.*)
// still carry their pointer arms; the common path never instantiates them.
// Delete both names with those groups.
inline constexpr std::size_t nativeTriggerCapacity50TG=stockLargestTriggerSetReceipt50TG;
inline constexpr std::size_t nativeTriggerActionSlots50TG=stockLargestActionSetReceipt50TG;
} // namespace rr::game
