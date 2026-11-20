#pragma once
// Common rewrite (W0 freeze): the ONE race start, used by a fresh race and by
// every restart (original 1AC8E0). PLAN.md 2.6, F01, F02.
//
// begin(race, Request) runs, for Reason::fresh (after the 1A6AA0 load, 2.1
// steps 1-13) only the shared tail, and for Reason::restart the 1AC8E0 body
// first. The 1AC8E0 body builds on the partial restart that already exists for
// the Results replay (src\game\original_race_end_dispatch50la.cpp:365-485,
// EndWaitingPlaceScan::Cut worldReset..racerRestart):
//   World74+3D0 trigger reset 2DCC70; App11C clear; Aural 103210; music 1AC960;
//   particles 161F60; world 2702E0; race90;
// then the steps that are missing today:
//   1AEA70 per racer (216690 timing with the 11258 rule, 217420 damage reset);
//   VCR channel reset; record actor / ghost backup 288630; camera reset;
//   1ACB7C tail (race1B0 bit3).
// Shared tail of both reasons: 2A2880 grid placement, 1AED30 start lock, 197040
// start overlay, race94 countdown.
//
// Callers (Request::caller): Results replay 1AC8E0(A1=0) (1CA... rows), Pause
// RESTART 1BD934 -> 1AC8E0(race,1), Time Trial finish 1ABC0C -> 1AC8E0(race,0),
// Practice 1ABC1C -> 1AC8E0. Retired: SessionExit50DU::Restart,
// timeTrialRestartPending148, the consumed-launch renewal (B-C07) and the
// restart=port_rebuild path. No session reload happens here.
//
// Owner: R9 (common_race_start50cr.cpp). The race-internal steps reach
// rr::game::OriginalRaceConstructor50LA through `friend class ::common_race_start50CR::Owner;`
// (R1 adds that friend line). Port-side owners that live in the runSession locals
// (racer bank, timing fields, damage, grid, camera) are reached through Hooks,
// supplied by I3 at race publication; each hook performs exactly one original
// step for every racer and reports refusal. Nothing writes guest memory.
#include "common_race_log50cr.h"
#include "native_borrowed_call50du.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>

namespace rr::game { class OriginalRaceConstructor50LA; }
namespace original_main_entry50KG { struct Application; }
namespace common_roster50CR { class Owner; }
namespace common_lap50CR { class Tracker; }

namespace common_race_start50CR {

enum class Reason : std::uint8_t { fresh, restart };
// Original caller addresses (logged as caller=<pc>).
inline constexpr std::uint32_t callerFresh1A6AA0=0x1A6AA0u;
inline constexpr std::uint32_t callerResultsReplay=0x1CA6A4u;   // Results replay row (1AC8E0 A1=0); R9 confirms the exact 1CA... site
inline constexpr std::uint32_t callerPauseRestart1BD934=0x1BD934u;
inline constexpr std::uint32_t callerTimeTrial1ABC0C=0x1ABC0Cu;
inline constexpr std::uint32_t callerPractice1ABC1C=0x1ABC1Cu;

struct Request final {
    Reason reason=Reason::fresh;
    std::uint32_t a1=0;             // 1AC8E0 a1 (Pause RESTART passes 1)
    std::uint32_t caller=callerFresh1A6AA0;
    std::uint32_t mode11258=1;      // launch subtype (216690 keeps CB4 when 3)
};

// Ordered steps; Receipt::reached names the last step entered.
enum class Step : std::uint8_t {
    triggerReset2DCC70, app11CClear, aural103210, music1AC960, particles161F60, world2702E0, race90,
    racers1AEA70, vcrReset, recordBackup288630, cameraReset, tail1ACB7C,
    grid2A2880, startLock1AED30, startOverlay197040, countdown94, done
};
inline std::uint32_t stepPc(Step s)noexcept {
    static constexpr std::uint32_t pcs[]={0x2DCC70u,0x1AC920u,0x103210u,0x1AC960u,0x161F60u,0x2702E0u,0x1AC8E0u,
        0x1AEA70u,0x287430u,0x288630u,0x1750A0u,0x1ACB7Cu,0x2A2880u,0x1AED30u,0x197040u,0x1AC8E0u,0u};
    return pcs[static_cast<std::size_t>(s)];
}

// Port-side per-step actions supplied by the integration writer (I3). Each is
// called once per begin() in the order above, for every racer of the field;
// returning false refuses the start at that step (logged). Empty optional = the
// step's owner is not present in this race (logged as refused, never skipped
// silently).
template<class Sig> using Hook=std::optional<interactive_frame50DU::BorrowedCall<Sig>>;
struct Hooks final {
    Hook<bool(std::size_t slot,std::uint32_t mode11258)noexcept> resetRacerTiming216690;   // 1AEA70 -> 216690 (lap_timing50id.h rule)
    Hook<bool(std::size_t slot)noexcept> resetRacerDamage217420;                           // 1AEA70 -> 217420
    Hook<bool(std::size_t slot)noexcept> placeOnGrid2A2880;                                // 2A2880 grid placement
    Hook<bool()noexcept> lockStart1AED30;                                                  // 1AED30 start lock
    Hook<bool()noexcept> showStartOverlay197040;                                           // 197040 start overlay
    Hook<bool()noexcept> resetVcrChannels;                                                 // 287430/287710/2878F0 channel reset
    Hook<bool()noexcept> backupRecordActor288630;                                          // 288630 record actor / ghost backup
    Hook<bool()noexcept> resetCamera;                                                      // camera session reset (1750A0 rebinding)
};

struct Receipt final {
    bool ok=false;
    Step reached=Step::triggerReset2DCC70;
    std::uint32_t refusedPc=0;
    const char* reason=nullptr;
    float bestLapCB4=0.f;           // slot 0 after the 216690 reset (kept when 11258==3)
    std::size_t racers=0;
};

class Owner final {
    const original_main_entry50KG::Application& application_;
    std::weak_ptr<rr::game::OriginalRaceConstructor50LA> race_;
    common_roster50CR::Owner* roster_=nullptr;
    common_lap50CR::Tracker* laps_=nullptr;
    bool busy_=false;               // one begin at a time; a re-entrant call is refused
public:
    Owner(const original_main_entry50KG::Application& app)noexcept:application_(app){}
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    // R9: bind the counted race and its roster/lap owners (I3 at publication).
    bool bind(const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>& race,
        common_roster50CR::Owner& roster,common_lap50CR::Tracker& laps)noexcept;
    bool bound()const noexcept {return !race_.expired()&&roster_&&laps_;}
    // R9: 1AC8E0 (restart) or the fresh tail. Logs common_race_start (PLAN 4.1).
    bool begin(const Request& request,const Hooks& hooks,Receipt& out)noexcept;
    void unbind()noexcept;                                                   // R9
};

} // namespace common_race_start50CR
