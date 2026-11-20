#pragma once
// Common rewrite (W0 freeze): the ONE race World at race+74 (original 1B2870,
// 0x550 bytes), for every course. Plan PLAN.md 2.1 step 2, B-C05.
//
// Replaces the 13 per-course world slots of rr::game::OriginalRaceConstructor50LA
// (world74_ for RA, taWorld74_ .. tcWorld74_, commonRouteWorld74_, gaWorld74_),
// their 12 per-course world-prefix headers (original_{ga,..,tc}_world_cold_prefix50la.h)
// and the per-course exclusion lists of original_race_world_join50la.h. The race
// holds exactly one `std::shared_ptr<common_race_world50CR::World> world74_` (R1).
//
// Prefix (1A6BC0): new 0x550, 1B2870 constructor, 141300 entity enrolment,
// 2350D0 (World4D8 sound group), 1B2918; the 1A9EE0 track load then fills the
// World's Track (common_track50cr.h). Retirement: 1B2870 destructor, 235150,
// 1B5F64. All fields are port-owned host state; nothing writes guest memory.
// The list at +500 is host storage sized from its own growth rule, every write
// bounds-checked (refused when it would not fit).
#include "common_track50cr.h"
#include "../../src/game/original_wreck_components50la.h"
#include <cstdint>
#include <memory>

namespace original_main_entry50KG { struct Application; }
namespace rr::game { class OriginalRaceConstructor50LA; }

namespace common_race_world50CR {

class World final {
public:
    World()noexcept=default;
    World(const World&)=delete;World& operator=(const World&)=delete;
    ~World();                                                       // R1 (refuses to die while counted: std::terminate as today)

    // ---- 1B2870 fields the port represents (offsets are the original's)
    rr::game::OriginalWreckComponent50LA entity;                    // 141300 enrolled entity
    bool counted=false,prefixComplete=false;
    std::uint32_t mode30=0,useEnvironment35C=0,mode524=0;
    std::unique_ptr<rr::game::OriginalWreckComponent50LA*[]> list500;
    std::uint32_t listCapacity504=0,listGrowth508=0,listCount50C=0;

    // ---- the track the 1A9EE0 load fills (World+A4.. +3E4 are its data)
    std::shared_ptr<common_track50CR::Track> track;
    const common_track50CR::Track* trackData()const noexcept {return track&&track->loaded()?track.get():nullptr;}
    // World+3E4 cut count as 174E58 reads it ([CSCameras].NumberOfCameras).
    std::uint32_t cutCount3E4()const noexcept {return track?track->cutCount3E4():0u;}
    // World+70: water present (2745D4 factory ran) or absent (274570). A9 data branch.
    bool water70()const noexcept {return track&&track->hasWater();}

    // R1: bounds-checked append to the +500 list (refused when the list is full
    // and growth would exceed its own rule; logs common_storage_refused).
    bool appendList500(rr::game::OriginalWreckComponent50LA& child,std::uint32_t pc)noexcept;
};

// R1, 1A6BC0 -> 1B2870 -> 141300 -> 2350D0(World4D8) -> 1B2918. Constructs the
// race's one World with an (unloaded) Track that the 1A9EE0 load fills next.
// Refuses when the race already holds a world. Logs common_load step=02.
bool construct1A6BC0(rr::game::OriginalRaceConstructor50LA& race,
    const original_main_entry50KG::Application& application,
    std::shared_ptr<common_track50CR::Track> track,
    std::shared_ptr<World>& out,std::uint32_t& pc)noexcept;
// R1, retirement 1B2870 destructor -> 235150 -> 1B5F64 (one retire for every course).
bool retire1B2870(rr::game::OriginalRaceConstructor50LA& race,World& world,std::uint32_t& pc)noexcept;
// R1: the intro/world entity identity checks of original_race_world_join50la.h:18-66,
// one rule (no per-course exclusion list): the race's one World, counted, prefix
// complete, reference00==1, same session generation, retirable.
const rr::game::OriginalWreckComponent50LA* introWorldEntity(const rr::game::OriginalRaceConstructor50LA& race)noexcept;

} // namespace common_race_world50CR
