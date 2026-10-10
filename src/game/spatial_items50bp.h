#pragma once
#include <cstdint>
namespace rr::game {
struct SpatialItemBlock50BP{std::uint32_t next=0,identities[4]{};};
struct SpatialItemNode50BP{std::uint32_t firstBlock=0,children[4]{};};
struct SpatialItemState50BP{
 std::uint32_t nodeCount=0,blockCount=0,freeHead=0,bytesUsed=0;
 SpatialItemNode50BP nodes[32]{};SpatialItemBlock50BP blocks[128]{};
};
struct SpatialItemWorkspace50BP{SpatialItemState50BP staged{};std::uint8_t membership[128]{};};
struct SpatialItemResult50BP{bool hasItemsOrChildren=false;};
enum class SpatialItemStatus50BP{Complete,InvalidInput,InvalidTopology,AllocatorBoundary};
// Original264E30/264F00 with105720/105830 and20-byte zero fill. Explicit pool
// domain: active chunk exhausted, no unused chunks, all declared blocks already
// belong to this pool and are either in declared nodes or its free list. Live
// blocks must have at least one nonzero identity (no pre-existing empty blocks). Growth
// returns AllocatorBoundary, never a successful fake allocation. First-null-slot
// insertion; removal preserves source first-matching-block and hole/free order.
// References are1-based native indices, identities/children opaque nonzero words.
// A whole-state transaction; failure preserves state/result; scratch may change.
SpatialItemStatus50BP editSpatialItems50BP(SpatialItemState50BP&,
 std::uint32_t nodeIndex,std::uint32_t identity,bool add,
 SpatialItemWorkspace50BP&,SpatialItemResult50BP&)noexcept;
}
