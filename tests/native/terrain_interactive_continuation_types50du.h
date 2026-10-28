#pragma once
#include "../../src/game/race_event_queue50id.h"
#include "../../src/game/host_race_limits50ht.h"
#include "../../src/game/player_events49a.h"
#include <array>
namespace original_camera_session50KX { class Session; }
#include "terrain_interactive_session50du.h"
namespace interactive_continuation50DU {
using namespace rr::game;
inline constexpr std::uint32_t raceRacerCapacity=rr::game::defaultHostRaceRacers50HT;
// Original layouts only. Both actual objects remain in the caller scope.
struct DynamicActors50IW {
  std::array<RaceActorSnapshot50ID,raceRacerCapacity> values{};
  std::array<bool,raceRacerCapacity> bound{};
 };
struct HumanCameraInputRoute {
  PlayerEventChildBinding49A* player=nullptr;
  original_camera_session50KX::Session* camera=nullptr;
  const ManagerHeldSources49A* held=nullptr;
 };
} // namespace interactive_continuation50DU
