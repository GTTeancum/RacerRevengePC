#pragma once
#include <cstdint>
#include "body_scene48g.h"
namespace rr::game {
struct GridCenterInput50AC {std::uint32_t current[4],previous[4],rank,nextNode;};
struct GridCenterResult50AC {std::uint32_t position[4],heading[4],nextNode;};
// Original2A2880 center-column ranks1/4 only, after source predecessor selection.
// Current W is copy-only. Heading W is source-written1. No lateral-column or
// vehicle reset/terrain-height behavior is fabricated. Finite nondegenerate XYZ.
// Failure preserves output; no allocation, guest execution or retained pointers.
bool prepareGridCenter50AC(const GridCenterInput50AC&,GridCenterResult50AC&) noexcept;
// Source-derived dry/unattached diagnostic pose:2169C0 stored lift offset and
// 216AEC/28A090 basis construction. Does not execute full vehicle reset,
// scene mutation or optional terrain/water adjustment. Caller supplies shared up.
bool prepareCenterPose50AC(const GridCenterInput50AC&,float maxLiftHeight,
 const Vector3_48C& sharedUp,BodyMatrix48G&) noexcept;
}
