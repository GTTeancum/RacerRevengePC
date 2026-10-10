#pragma once
#include "type3_state48p.h"

namespace rr::game {
struct Type3PrepareInput48P {
    std::uint32_t pairCount=0;
    const SpringVector4_48N* endpoints=nullptr;
    std::size_t endpointCount=0; // Vec4 entries, two per used pair.
    const std::uint32_t* endpointScenes=nullptr;
    std::size_t endpointSceneCount=0; // One metadata word per endpoint.
    SpringVector4_48N sentinel{}; // Explicit caller-supplied value at original 3E02C0.
};
enum class Type3PrepareStatus48P {
    Complete,InvalidInput,UnsupportedScope,CapacityExceeded,AliasedBuffers,NonFinite
};
// Original11DD00 type3 preparation, bounded to already-world-space endpoints:
// all 2*pairCount consumed scene metadata words must be zero. Positive signed
// pair counts only; no invented gameplay cap or scene resolver. Require finite
// consumed XYZ and finite intermediates; endpoint W and sentinel lanes are raw
// payloads. Preserve world-start W, local arrays, per-pair result arrays, all
// unused tails and unaffected state. Writes world starts/directions/lengths,
// world/query center and radius, result00/10, selectedFraction=1 and hitCount=0.
// Full declared spans must be aligned, representable and backed by the caller.
// worldStarts/worldDirections/lengths are required; untouched buffer arrays may
// be null, otherwise their full capacity is validated. All supplied buffer
// spans must be mutually disjoint and disjoint from source spans/descriptors/
// state. Read-only spans may overlap one another. State may not overlap any
// span or descriptor. No error changes any array, descriptor or state byte.
// Two arithmetic passes, constant stack, no allocations/threads/guest memory.
// Ordinary rounded binary32 domain, not all physical EE/VU exceptional modes.
Type3PrepareStatus48P prepareType3Rays48P(const Type3PrepareInput48P&,
    Type3RayBuffers48P&,Type3QueryState48P&) noexcept;
}
