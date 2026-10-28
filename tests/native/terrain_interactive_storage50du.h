#pragma once
#include "../../src/game/vehicle_static_response50g.h"
#include "../../src/game/type3_query48p.h"
// Shared value storage, not resource owners. These declarations were moved
// unchanged from the root's dependency umbrella for the physics suffix.
using namespace rr::game;
struct ResponseStorage50K {
 SpringGeneratedRecord48K projected[60]{};SpringRecord48N staged[60]{};
 std::uint8_t flags[60]{},nextFlags[60]{};SpringModelBody48K scratch{},prepared{};
 VehicleStaticResponseWorkspace50G view(VehicleFrameWorkspace49W&w){return {{projected,60,{&scratch,1,flags,60},{&prepared,1,nextFlags,60}},staged,60,&w};}
};
struct MovementRays50N {
 Vector3_48C localStart[1]{},localDirection[1]{};SpringVector4_48N start[1]{},direction[1]{},normal[1]{},point[1]{};
 std::uint32_t hit[1]{};float fraction[1]{},length[1]{};
 Type3RayBuffers48P view(){return {localStart,localDirection,start,direction,normal,point,hit,fraction,1,length,1};}
 Type3RaySource48P source()const{return {localStart,localDirection,start,direction,normal,point,hit,fraction,1,length,1};}
};
