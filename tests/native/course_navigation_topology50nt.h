#pragma once
// ONE ordered topology check for every navigation owner of the 50MB spline
// family (MB, GB, MC, TB, TC). The live collision world is built from the
// course SCB's own portals; navigation binds only to a world whose heads,
// links and portal geometry equal what those same authored portals produce.
// Counts are the SCB's (live), bounded by the shared navigation capacity
// (track_navigation_view50mb.h); no stock zone/portal count is checked here.
#include "../../src/game/track_navigation_view50mb.h"
#include <cstring>
#include <vector>

namespace course_navigation_topology50NT {
namespace g=rr::game;
// Worst-case ephemeral scratch for one check (heap; charged by callers).
inline constexpr std::size_t scratchBytes() noexcept {
    return 256u*sizeof(g::CollectionRef50BX)+
        2u*g::courseNavigationPortalCapacity50MB*sizeof(g::LiveZonePortalLink50EN)+
        g::courseNavigationPortalCapacity50MB*sizeof(g::PortalGeometry50EO)+sizeof(g::LiveZoneWorld50EN);
}
inline bool matches(const g::LiveZonePortalInput50EN* authored,std::size_t portals,std::size_t zones,
    const g::LiveZoneWorld50EN& actual) noexcept {
    if(!authored||!zones||zones>256||portals>g::courseNavigationPortalCapacity50MB||
       actual.zoneCount!=zones||actual.portalCount!=portals||!actual.portalHeads||
       (actual.portalLinkCount&&!actual.portalLinks)||(portals&&!actual.portals))return false;
    try {
        // Ephemeral ordered verification only; no alternate live world escapes.
        std::vector<g::CollectionRef50BX> heads(zones);
        std::vector<g::LiveZonePortalLink50EN> links(2*portals+1);
        std::vector<g::PortalGeometry50EO> geometry(portals+1);
        g::LiveZoneWorld50EN expected{};
        const g::LiveZoneWorldStorage50EN backing{heads.data(),zones,links.data(),2*portals,geometry.data(),portals};
        if(g::prepareLiveZoneWorld50EN(authored,portals,zones,backing,expected)!=g::LiveZoneStatus50EN::Complete||
           expected.portalLinkCount!=actual.portalLinkCount)return false;
        for(std::size_t i=0;i<zones;++i)if(actual.portalHeads[i]!=heads[i])return false;
        for(std::size_t i=0;i<actual.portalLinkCount;++i){const auto& p=actual.portalLinks[i];
            if(p.next!=links[i].next||p.zone!=links[i].zone||p.portal!=links[i].portal)return false;}
        // Distance caches are runtime-mutated, not topology. Prepared geometry
        // retains the source binary32 vertices, normal and all edge normals.
        for(std::size_t i=0;i<portals;++i){
            if(std::memcmp(actual.portals[i].vertices,geometry[i].vertices,sizeof geometry[i].vertices)||
               std::memcmp(&actual.portals[i].normal,&geometry[i].normal,sizeof geometry[i].normal)||
               std::memcmp(actual.portals[i].edgeNormals,geometry[i].edgeNormals,sizeof geometry[i].edgeNormals))return false;
        }
        return true;
    }catch(...){return false;}
}
}
