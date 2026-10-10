#pragma once
#include "candidate_collection50bx.h"
#include "search_view50bt.h"
namespace rr::game {
struct ZoneSearchBinding50CG {
    const SearchStorage50BS* first=nullptr;
    const SearchStorage50BS* second=nullptr;
};
struct ZoneSearchViews50CG {
    CollectionTree50BX* methods=nullptr;std::size_t methodCapacity=0;
    CollectionZone50BX* zones=nullptr;std::size_t zoneCapacity=0;
    SearchListBlock50BR* blocks=nullptr;std::size_t blockCapacity=0;
};
// Builds two real SearchList method views per explicitly supplied zone, including
// empty methods. No candidate flattening, fallback substitution or zone sorting.
// Zone refs follow input order; query's linked zone chain remains caller-owned.
// Rebuild after registry mutation. These views borrow source registry words.
// Output arrays are disposable staging; publish only on Complete. No allocation.
SearchStorageStatus50BS prepareZoneSearchViews50CG(const ZoneSearchBinding50CG*,
    std::size_t zoneCount,const ZoneSearchViews50CG&)noexcept;
}
