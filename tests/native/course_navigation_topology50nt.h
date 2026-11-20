#pragma once
// ONE ordered topology check for the one navigation store (common rewrite R4,
// common_navigation50cr.cpp): the 1ACF70 zone index binds only to a live
// collision world whose heads, links and portal geometry equal what the
// course SCB's own authored portals produce. Counts are the SCB's (live); the
// zone limit is the live-world rule (<=256, nodeIndex78 is one byte). No stock
// zone/portal count and no fixed portal capacity is checked here.
#include "../../src/game/live_zone_membership50en.h"
#include <cstring>
#include <vector>

namespace course_navigation_topology50NT {
namespace g=rr::game;
// Ephemeral heap scratch for one check of `zones`/`portals` (the SCB's counts).
inline std::size_t scratchBytes(std::size_t zones,std::size_t portals) noexcept {
    return zones*sizeof(g::CollectionRef50BX)+(2u*portals+1u)*sizeof(g::LiveZonePortalLink50EN)+
        (portals+1u)*sizeof(g::PortalGeometry50EO)+sizeof(g::LiveZoneWorld50EN);
}
inline bool matches(const g::LiveZonePortalInput50EN* authored,std::size_t portals,std::size_t zones,
    const g::LiveZoneWorld50EN& actual) noexcept {
    if((portals&&!authored)||!zones||zones>256||
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
