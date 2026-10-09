#include "global_zone_chain50ck.h"
namespace rr::game {
bool initializeGlobalZoneLink50CK(CollectionRef50BX firstZone,std::size_t zoneCount,
    std::uint32_t visibilitySearchIdentity,GlobalZoneLink50CK&destination)noexcept{
    if(!firstZone||static_cast<std::size_t>(firstZone)>zoneCount)return false;
    destination={{0,firstZone},visibilitySearchIdentity};return true;
}
}
