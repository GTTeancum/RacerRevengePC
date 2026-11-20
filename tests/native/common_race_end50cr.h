#pragma once
// Common rewrite (W0 freeze): the ONE end-of-race dispatch and the one
// departure, for every race. PLAN.md 2.7, 2.8 rows 9/10, 2.1 step 18, F02-F05.
//
// 1A7DD0 for every race (branches only on launch data, A10):
//   11258 == 1, ordinary:  199770, Results104 vtable+20, 1AD230, 1AB520 -> 1AB5BC rows
//                          (tournament 1AB7F8 Unlock, 1AB818 Garage, ...; Free Play
//                          1AB828 -> Hall 1AB884 / Unlock 1AB8E8 / Course 1AB908).
//   11258 == 3, Time Trial: skip 199770 and Results at the finish -> 1AB520 -> 1ABBC0
//                          -> 1ABBE8 199770 -> 1ABC0C restart (common_race_start50CR).
//   11258 == 2, Practice:   1ABC1C bit5, 199770, VCR, 1AC8E0, replay.
//   ALL8 spectator:         the same dispatch with humans34 == 0, on the launch's course.
// RA Free Play and RA Time Trial get this with no course code. HostRaceResults50IL
// is deleted. A rejected Results root logs `common_race_end ... route=refused` and
// departs to the Course screen through step 18 (PORT failure handling, flagged;
// it does not stand in for an original screen).
//
// Departure (step 18), in the original order: 26D070, 1A4AB0(0.5), 1A4B90(2),
// 267BE0, 2AED60 (500 ms fade), 1BE830, 17DE10(a,b,c). Used by Pause rows 9/10,
// the Results rows and the Time Trial record confirm (1CA574..1CA6F0).
//
// Owner: R9 (common_race_end50cr.cpp). Race internals through
// `friend class ::common_race_end50CR::Owner;` (R1 adds it). Callers: I4 (per frame).
#include "common_race_log50cr.h"
#include <cstdint>
#include <memory>

namespace rr::game { class OriginalRaceConstructor50LA; struct OriginalRaceEndEntryDiagnostic50LA; }
namespace original_main_entry50KG { struct Application; }
namespace original_position_stats_adapter50LA { class Owner; }
namespace common_race_start50CR { class Owner; struct Hooks; }

namespace common_race_end50CR {

// Launch data the dispatch reads (A10). Never a course code.
struct Launch final {
    std::uint32_t subtype11258=1;   // 1 race, 2 practice, 3 Time Trial
    std::uint32_t p34=0;            // profile +34 (2 = tournament)
    std::uint32_t humans34=0;       // 0 = ALL8 spectator
    std::uint32_t mode34=0;         // game34
    std::uint32_t app2E0=0;         // App+2E0 (Pause rows 9/10 skip rule)
};
using Route=common_race_log50CR::EndRoute;
struct Decision final {
    Route route=Route::results;
    std::uint32_t branch=0x1A7E18u; // 1A7E18 / 1ABBE8 / 1ABC1C
    std::uint32_t next=0x1AB5BCu;   // next row pc
};
// R9: the 1A7DD0 branch table above, from launch data only.
Decision decide1A7DD0(const Launch& launch)noexcept;

// Step-18 destinations (17DE10 arguments).
enum class Destination : std::uint8_t {
    hall,           // 17DE10(6,11,1) after 1AC440 lap rows / record confirm
    course,         // 17DE10(6,6,1)
    mainMenu,       // Pause row 9 (QUIT TO MAIN)
    tournament      // tournament return rows (1AB7F8 / 1AB818 ...)
};
struct Departure final {
    Destination destination=Destination::course;
    bool ok=false;
    std::uint32_t reachedPc=0;      // last step pc reached (26D070..17DE10)
    const char* reason=nullptr;
};

class Owner final {
    const original_main_entry50KG::Application& application_;
    std::weak_ptr<rr::game::OriginalRaceConstructor50LA> race_;
    common_race_start50CR::Owner* start_=nullptr;
    Launch launch_{};
    bool departing_=false;
public:
    explicit Owner(const original_main_entry50KG::Application& app)noexcept:application_(app){}
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    // R9: bind the counted race, its launch data and its race-start owner (I3).
    bool bind(const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>& race,const Launch& launch,
        common_race_start50CR::Owner& start)noexcept;
    const Launch& launch()const noexcept {return launch_;}
    // R9, 1A7DD0/1A7E18 -> 1AB520 -> 1ABBC0 / 1AB5BC: one call per frame once the
    // race reports its end (replaces Application::continueConsumedRaceEndEntry1A7DD0's
    // 0x1abc0c stop and the host text panel). Time Trial reaches 1ABC0C and calls the
    // race start in place with `hooks`. Logs common_race_end once per dispatch.
    bool continue1A7DD0(const ::original_position_stats_adapter50LA::Owner& observer,
        const common_race_start50CR::Hooks& hooks,rr::game::OriginalRaceEndEntryDiagnostic50LA& d)noexcept;
    // R9, step 18 in the original order. Logs common_teardown lines.
    bool depart(Destination destination,Departure& out)noexcept;
    bool departing()const noexcept {return departing_;}
    void unbind()noexcept;                                                   // R9
};

} // namespace common_race_end50CR
