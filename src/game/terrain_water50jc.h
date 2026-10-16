#pragma once
#include "water_height50jc.h"

namespace rr::game {
// Actual native projection of 210800/210804 -> 212074/212078 -> 1B1200/1B120C.
// The original vehicle334 and probe348+124 borrow the SAME stable provider.
// Caller supplies this binding only from its authenticated fresh-track join.
// Neither a presence bit nor a stand-alone height callback admits the route.
// Provider120 (original vehicle330) is outside this bounded implementation.
struct TerrainWaterBinding50JC {
    const WaterHeightProvider50JC* vehicle334=nullptr;
    const WaterHeightProvider50JC* probe124=nullptr;
    const WaterHeightGlobals50JC* globals=nullptr;
    bool sourceTerrain330Absent=false;
};
inline bool coherentTerrainWater50JC(const TerrainWaterBinding50JC& b) noexcept {
    return b.sourceTerrain330Absent&&b.vehicle334&&b.probe124==b.vehicle334&&
        b.globals&&b.probe124->sourceStateBound;
}
// Exact 1B18C0..1B1900 after the full-scene lower140 -> point150 seed and
// before queryEnabled100. The caller owns the prefix's zero118 write.
// On success this proposes all four point160 lanes and the actual above118.
// It never changes point150/normal170, geometry selection, or repulsor support.
struct TerrainWaterProposal50JC {
    float point160[4]{0.f,0.f,0.f,1.f};
    std::uint32_t above118=0;
};
inline WaterHeightResult50JC projectTerrainWater50JC(
    const TerrainWaterBinding50JC& binding,const float (&point150)[3],
    TerrainWaterProposal50JC& out) noexcept {
    using namespace water_height_detail50jc;
    if(!coherentTerrainWater50JC(binding))return fail(Status::Owner,0x1b18b4u);
    float point[3]{point150[0],point150[1],point150[2]};
    const auto queried=queryWaterHeight50JC(*binding.probe124,*binding.globals,point,nullptr);
    if(queried.status!=Status::Complete)return queried;
    Math math;
    // 1B18DC loads10;1B18E8 adds10+point150.y (in that operand order).
    const float threshold=math.add(10.f,point150[1],0x1b18e8u);
    const float waterY=math.read(point[1],0x1b18ecu);
    if(math.badPC)return arithmetic(math);
    TerrainWaterProposal50JC next;
    for(unsigned axis=0;axis<3;++axis)next.point160[axis]=point[axis];
    // C.OLE.S waterY,threshold; BC1T skips the store. Strict greater-than.
    next.above118=waterY>threshold?1u:0u;
    out=next;return fail(Status::Complete,0x1b1904u);
}
} // namespace rr::game
