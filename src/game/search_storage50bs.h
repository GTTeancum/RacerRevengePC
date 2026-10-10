#pragma once
#include <cstdint>
namespace rr::game {
// Native flattened physical blocks. Slot references are index+1. Live identities
// are nonzero/aligned4. Free words are (nextSlotRef<<2)|2; zero next is end.
struct SearchStorage50BS{
 std::uint32_t perBlock=0,blockCount=0,lastUsed=0,freeHead=0;
 std::uint32_t words[1024]{};
};
enum class SearchStorageStatus50BS{Complete,InvalidInput,InvalidTopology,GrowthBoundary};
// 27E3C0 existing previous handle, free head, then current block tail. Selection
// does not reserve/pop. Realloc/new-block path is an explicit boundary.
SearchStorageStatus50BS selectSearchSlot50BS(const SearchStorage50BS&,
 std::uint32_t previousHandle,std::uint32_t& selected)noexcept;
// 27E570/27E5B0; only the selected free-head or fresh-tail insertion is admitted.
// Previous live-handle updates use original27E9C0 separately, not insertion.
SearchStorageStatus50BS insertSearchSlot50BS(SearchStorage50BS&,
 std::uint32_t slot,std::uint32_t identity)noexcept;
SearchStorageStatus50BS removeSearchSlot50BS(SearchStorage50BS&,
 std::uint32_t slot,std::uint32_t identity)noexcept;
}
