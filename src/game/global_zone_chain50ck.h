#pragma once
#include "candidate_collection50bx.h"
namespace rr::game {
struct GlobalZoneLink50CK {
    CollectionZoneLink50BX collection{};
    std::uint32_t searchIdentity8=0;
};
// Constructor2808E0 clears next; loader272F80 publishes world4C4[0] as zone,
//272F90 stores the explicitly supplied visibility search identity. This is the
// consumed global443148 head projection, not a chain across all authored zones.
// Caller owns storage/allocation/lifetime and supplies the actual first-zone ref.
// No allocator, global counter or unused link payload behavior is invented.
bool initializeGlobalZoneLink50CK(CollectionRef50BX firstZone,std::size_t zoneCount,
    std::uint32_t visibilitySearchIdentity,GlobalZoneLink50CK&)noexcept;
}
