#pragma once
// Common rewrite (W0 freeze): the ONE lap tracker, for every course. PLAN.md 2.5, B-C12 lap part.
//
// Wraps the single existing algorithm of src\game\lap_progress50gf.cpp
// (prepareLapProgressTrack50GFShared50MB / applyPlayerLapCrossing50GFShared50MB):
// ProgressNode markers 2A1980/2A273C/2A26E0, FF8 marker, CC8++ at 1A0F18,
// 1A0F24 -> 1A2B80 callback, 1A1108 scan. Retires the five typed views
// LapProgressTrackView50{MB,TC,TB,GB,MC}, the five lap_drive50GJ slots and their
// ternary, the five race_session50HM course sessions and the five-way wrong-way
// split. One view type (common_track50CR::NavigationView), one session row type,
// one wrong-way rule. Event 6 (1A55F0..1A5914) reads lapCC8 from here.
//
// Timing stays in src\game\lap_timing50id.h: 216690 resets CB0/CB8/CC0 and keeps
// the best lap CB4 when 11258==3 (initializeLapTimingFields50ID), which is how the
// Time Trial in-place restart keeps the run's best lap with no special code.
//
// Owner: R4 (common_lap50cr.cpp). Per-racer storage is the race's field
// (maxHostRaceRacers50HT = 8, the original 1AD520 limit); every slot index is
// bounds-checked and refused past the field size.
#include "common_track50cr.h"
#include "../../src/game/host_race_limits50ht.h"
#include "../../src/game/lap_progress50gf.h"
#include "../../src/game/lap_timing50id.h"
#include "../../src/game/wrong_way_progress50go.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace race_session50HM { struct Config; class CourseSession; }

namespace common_lap50CR {

struct Diagnostic final {
    std::uint32_t pc=0;
    bool refused=false;
    std::size_t slot=0;
    const char* reason=nullptr;
    rr::game::LapProgressDiagnostic50GF progress{};
};

// Per racer slot: the 1A0F18/1A0F24/1A1108 state and its pending 1A2B80 callback.
struct Racer final {
    rr::game::LapProgressState50GF state{};             // vehicle FF8, CC8, CD8
    rr::game::LapProgressContinuation50GF pending{};
    rr::game::WrongWayProgressState50GO wrongWay{};     // vehicle FF8, 10CC
    rr::game::WrongWayProgressContinuation50GO wrongWayPending{};
    bool bound=false;
};

class Tracker final {
    common_track50CR::NavigationView map_{},ai_{};
    rr::game::LapProgressTrack50GF markers_{};           // the original's 10 ProgressNode slots (+48/+4C)
    std::array<Racer,rr::game::maxHostRaceRacers50HT> racers_{};
    std::size_t field_=0;
    bool prepared_=false;
public:
    // R4, 2A1980/2A273C/2A26E0: ProgressNode markers from the map/AI views (the
    // one shared prepare). lap_markers is counted at step 08 (readNavigation2A1260) and printed by Track::logTrackData (W0 round 2). `field` is the
    // race's racer count (1..8); slots >= field are refused.
    bool prepare(const common_track50CR::NavigationView& map,const common_track50CR::NavigationView& ai,
        std::size_t field,Diagnostic& d)noexcept;
    bool prepared()const noexcept {return prepared_;}
    std::size_t field()const noexcept {return field_;}
    const rr::game::LapProgressTrack50GF& markers()const noexcept {return markers_;}
    const common_track50CR::NavigationView& map()const noexcept {return map_;}
    const common_track50CR::NavigationView& ai()const noexcept {return ai_;}
    // R4: initializePlayerLapRaceFields50GF / initializePlayerLapCursor50GF for one slot
    // (fresh race and 1AEA70 per-racer restart reset). Refused past the field.
    bool resetRacer(std::size_t slot,Diagnostic& d)noexcept;
    // R4, 1A0F18 CC8++, 1A0F24 -> 1A2B80 request, 1A1108 scan. Logs common_lap on a
    // completed lap (racer, lapCC8, time/best from `timing`).
    bool applyCrossing(std::size_t slot,const rr::game::LapProgressCrossing50GF& crossing,
        const rr::game::LapTimingFields50ID& timing,rr::game::LapProgressReceipt50GF& receipt,Diagnostic& d)noexcept;
    bool resumeAfterCallback(std::size_t slot,Diagnostic& d)noexcept;      // R4, resumePlayerLapAfterCallback50GF
    // R4: the one wrong-way rule (beginWrongWayProgress50GO over the map view + markers).
    bool beginWrongWay(std::size_t slot,const rr::game::WrongWayProgressInput50GO& input,
        rr::game::WrongWayProgressReceipt50GO& receipt,rr::game::WrongWayProgressDiagnostic50GO& d)noexcept;
    bool resumeWrongWay(std::size_t slot,rr::game::WrongWayProgressDiagnostic50GO& d)noexcept;   // R4
    // Bounds-checked reads (nullptr / 0 past the field).
    const Racer* racer(std::size_t slot)const noexcept {return slot<field_?&racers_[slot]:nullptr;}
    std::uint32_t lapCC8(std::size_t slot)const noexcept {return slot<field_?racers_[slot].state.vehicleCC8:0u;}
    std::uint32_t markerFF8(std::size_t slot)const noexcept {return slot<field_?racers_[slot].state.vehicleFF8:0u;}
    void release()noexcept;                                                 // R4: drop views (teardown step 18)
};

} // namespace common_lap50CR

namespace race_session50HM {
// R4 (race_session50hm_fixture.h): the ONE course race-session row type replacing
// MbSession/TcSession/TbSession/GbSession/McSession, node storage sized from the
// view's count (not a fixed 293/256 template). Declared here so the frozen
// initializer below can name it; R4 defines the class in its own header.
// R4: initialize the one session from the one map view and the lap tracker.
bool initialize(CourseSession& session,const common_track50CR::NavigationView& map,
    const common_lap50CR::Tracker& laps,std::uint32_t racers,const Config& config)noexcept;
}
