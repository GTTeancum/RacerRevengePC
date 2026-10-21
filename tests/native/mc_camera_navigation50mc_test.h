#pragma once
#include "../../src/game/original_position_stats_radar_route50la.h"
#include "../../src/game/navigation_consumer_bridge50mc.h"
#include <string>
namespace mc_camera_navigation50MC_test {
// Test hook for the parent's actual collision-aggregate issuer. This is NOT a
// runtime admission function; it returns observations from real typed views.
inline bool check(const rr::game::TrackNavigationView50MC& map,
 const rr::game::TrackNavigationView50MC& ai,std::string& error){
 namespace n=rr::game::navigation_consumer_detail50MB;namespace r=rr::game::position_stats_detail50LA;
 if(!map.runtimeValid()||!ai.runtimeValid()||!map.sameOwner(ai)||map.count()!=173||ai.count()!=173||
    !map.sourceIdentity()||map.sourceIdentity()!=ai.sourceIdentity()){error="actual MC map/AI issuer required";return false;}
 if(!n::mapReady(map)||n::mapReady(ai)||!n::aiReady(ai)||!n::pairReady(map,ai)){error="MC typed role joins differ";return false;}
 std::size_t emits=0,finishes=0;
 // Alpha0 is a valid numerical visibility fixture: visits all authored nodes
 // without claiming a completed strip or renderer geometry submission.
 auto emit=[&](std::size_t,std::size_t,float,float,std::uint32_t& alpha)noexcept{++emits;alpha=0;return true;};
 auto finish=[&](std::size_t,std::size_t)noexcept{++finishes;return true;};
 r::RadarRouteProgress progress;
 if(!r::traverseRadar1924C0(map,emit,finish,progress)||progress.visitedNodes!=173||progress.completedElements||finishes||!emits){error="MC genuine173-node radar traversal differs";return false;}
 emits=0;
 if(r::traverseRadar1924C0(ai,emit,finish,progress)||emits||progress.sourcePC!=0x192734){error="AI-role map masquerade admitted";return false;}
 rr::game::TrackNavigationView50MC empty;
 if(r::traverseRadar1924C0(empty,emit,finish,progress)||emits){error="empty MC map admitted";return false;}
 error.clear();return true;
}
}
