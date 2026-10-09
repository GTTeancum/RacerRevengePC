#pragma once
#include "search_storage50bs.h"
#include "search_list50br.h"
namespace rr::game{
// Validated non-owning projection over the CURRENT prepared storage. Rebuild
// after storage mutation; state and descriptor lifetimes cover iterator use.
// No fake ordering: descriptors point to the actual native physical block slots.
// Unused descriptor tails remain untouched. Rejection preserves all outputs.
SearchStorageStatus50BS prepareSearchView50BT(const SearchStorage50BS&,
 SearchListBlock50BR* descriptors,std::size_t descriptorCapacity,
 SearchListInput50BR& output)noexcept;
}
