#pragma once
#include "authored_runtime_ground50cp_fixture.h"
#include "live_zone_membership50en.h"
#include <stdexcept>
#include <string>
#include <vector>

// Cold bridge only. Retain the authored order and parse original field text
// through the existing source binary32 parser rather than recasting doubles.
inline std::vector<rr::game::LiveZonePortalInput50EN> loadLivePortalInputs50EP(
    const rr::assets::World48F& world) {
    std::vector<rr::game::LiveZonePortalInput50EN> result;
    result.reserve(world.portals.size());
    for(const auto& portal:world.portals) {
        if(portal.parameter_block>=world.parameters.blocks.size() ||
           !portal.zones[0] || !portal.zones[1] ||
           portal.zones[0]>world.zones.size() || portal.zones[1]>world.zones.size() ||
           portal.zones[0]==portal.zones[1])
            throw std::runtime_error("invalid retained portal ownership");
        rr::game::LiveZonePortalInput50EN input{};
        input.zoneA=portal.zones[0];input.zoneB=portal.zones[1];
        const auto& block=world.parameters.blocks[portal.parameter_block];
        for(unsigned vertex=0;vertex<4;++vertex) {
            const auto name="vertex"+std::to_string(vertex+1);
            const auto parsed=authored_runtime_ground50cp_fixture::vec(block,name.c_str());
            for(unsigned lane=0;lane<3;++lane)input.vertices[vertex].v[lane]=parsed[lane];
        }
        result.push_back(input);
    }
    return result;
}
