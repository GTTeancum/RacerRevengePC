#pragma once
#include "../game/grid_center50ac.h"
#include <cstddef>
#include <cstdint>
namespace rr::assets {
constexpr std::size_t maxTrackNodes50AD=256,maxTrackBytes50AD=65536;
struct TrackNodeScratch50AD {std::uint32_t position[4],next0;std::uint8_t fields;bool seen;};
struct TrackStartScratch50AD {TrackNodeScratch50AD nodes[maxTrackNodes50AD];};
struct TrackStart50AD {rr::game::GridCenterInput50AC grid;std::uint32_t nodeCount,startNode,previousNode;};
enum class TrackStartError50AD {None,Input,Limit,Syntax,Duplicate,Number,Missing,UnsupportedStart,NoPredecessor};
struct TrackStartDiagnostic50AD {TrackStartError50AD error;std::size_t offset;};
// Exact bounded decimal route, <=11digits and <=6fractional digits; normal float32.
// Native integer-only conversion, original soft-double then float narrowing.
bool parseTrackNumber50AD(const char*,std::size_t,float&,std::uint64_t* binary64Bits=nullptr) noexcept;
// Caller-owned bounded cold reader, no allocation. Admits selected StartNode0
// (absent,0,or authored1) and center ranks1/4; other starts explicitly reject.
// All nodes require position,positive bounded NumNextNodes,and first-next link.
// Other keys are unconsumed; this is a start projection, not a full AI graph.
// Scratch may change on failure; output is transactional. Nonoverlapping storage.
bool readTrackStart50AD(const char*,std::size_t,std::uint32_t rank,TrackStartScratch50AD&,
 TrackStart50AD&,TrackStartDiagnostic50AD&) noexcept;
}
