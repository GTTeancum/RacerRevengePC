#pragma once
#include <cstdint>
#include <cstddef>
namespace rr::game {
struct SearchListBlock50BR{const std::uint32_t* words=nullptr;std::size_t capacity=0;};
struct SearchListInput50BR{const SearchListBlock50BR* blocks=nullptr;std::size_t blockCapacity=0;std::int32_t blockCount=0,perBlock30=0,lastUsed38=0;};
struct SearchListCursor50BR{std::int32_t block44=0,slot48=0;std::uint8_t flags16=0;};
enum class SearchListStatus50BR{Complete,InvalidInput,InsufficientCapacity};
// Original27E660/27E820 open/rewind and27E850 close. Upper flags retained.
void resetSearchList50BR(SearchListCursor50BR&,bool open)noexcept;
// Original27E6C0; skip only bit1-marked deleted entries, preserve raw word/order,
// including a raw zero entry and repeats. No object stamps/geometry/filtering.
// Nonnull pointer table must include a backed terminal descriptor at blockCount,
// mirroring the original terminal pointer read. Null table returns zero without
// consuming descriptor counts or changing cursor. Rejection is transactional.
SearchListStatus50BR nextSearchList50BR(const SearchListInput50BR&,
 SearchListCursor50BR&,std::uint32_t&)noexcept;
}
