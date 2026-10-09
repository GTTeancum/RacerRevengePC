#pragma once
#include "search_storage50bs.h"
namespace rr::game{
// Storage projection of original SearchList constructor27E27C..27E2B8. Caller
// asserts this is a fresh owner. Retains all unused backing words verbatim.
SearchStorageStatus50BS initializeColdSearchStorage50BU(SearchStorage50BS&,
 std::uint32_t perBlock)noexcept;
// Original post-allocation publication27E494..27E568 for the first block only.
// Native caller-owned words[] provide the backing; no guest malloc/realloc runs.
// Requires untouched empty constructor state and leaves backing words opaque.
// Returns source-selected first slot without registering/reserving it.
SearchStorageStatus50BS adoptFirstSearchBlock50BU(SearchStorage50BS&,
 std::uint32_t& selectedSlot)noexcept;
}
