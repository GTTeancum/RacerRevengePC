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
// then, in the original order (dis_all.txt 1AC8E0..1ACE48, R9 notes; W0 round 2):
//   1AC9A0 1AEA70 per racer (16E830 AI scalars, 2A2880 grid, 216690 incl. its 217420 leaf);
//   1AC9A8 1AED30 start lock; race1B0 bit4 clear: race94 = 4.0, bit2 clear;
//   1ACB18 (11258==3, race78, bit4 clear): 287BE0(race78), raceA0 = 0, B0+40 vtable+18 record actor;
//   1ACB74 267BE0; 1ACB7C tail (race1B0 bit3); 1ACBA0 per human 1AF330 / 1750A0 camera racer binding;
//   a1 != 0: 175430(race48), 1A9750(race,1); a1 == 0: 1AFD80(B4), race78: 287B60 + 287C80(1.0)
//   (11258==3) or 287B60 + race78+B0 bit1 (otherwise); 1ACDF0 1983A0(raceD4), 1ACE04 194CC0(raceD0).
// PLAN 2.6 named "record actor / ghost backup 288630": 288630 is not on this path (it is the
// 1ABA68 end backup); the restart runs 287BE0 at 1ACB18 (W0 round 2 correction).
// Fresh (W0 round 2 ruling): the cold load already placed every racer (I2: the collision
// world's placeGrid after step 08, roster step 10), so the fresh tail is 1AED30 start lock,
// 197040 start overlay and race94 only; begin(fresh) never calls placeOnGrid2A2880.
//
// Who performs each step (W0 round 2 ruling):
//   race-internal, R9 through the race/Application friends (no hook): 2DCC70/161F60/2702E0
//     (restartWorld, existing), App11C, 103210, 1AC960, race90, race94/bit2, 1ACB18's 287BE0 and
//     raceA0, 267BE0, bit3, the 1ACD68 VCR tail (1AFD80(B4), 287B60, 287C80 / race78+B0 bit1;
//     OriginalSimpleVCR50LA befriends only the race, so R1's race gains the private leaf R9 calls),
//     1A9750(race,1) (Application member), 194CC0;
//   Hooks (I3, port-side owners in the runSession locals): 216690 per racer (with 217420),
//     2A2880 per racer (restart only), 1AED30, 197040/1983A0, the 1ACB18 record actor
//     (B0+40 vtable+18, Time Trial only), the 1ACBA0 camera racer binding (+175430(race48) when a1 != 0).
//
// Callers (Request::caller): 1AC8E0 jal sites 1A8958 (race input restart, a1=0), 1ABAD8
// (1AB520 Results-waiting replay, a1=0), 1ABC0C (Time Trial, a1=0), 1ABD28 (Practice 1ABC1C,
// a1=0), 1BD990 (Pause RESTART 1BD934, a1=1), 1C8DF0 (Results summary restart row, a1=0),
// 1C9774 (Results tournament restart row, a1=0). Retired: SessionExit50DU::Restart,
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
inline constexpr std::uint32_t callerResultsReplay=0x1C8DF0u;                 // Results summary restart row (1AC8E0 A1=0); 1CA6A4 was wrong (R9)
inline constexpr std::uint32_t callerResultsTournamentReplay1C9774=0x1C9774u; // Results tournament restart row (A1=0)
inline constexpr std::uint32_t callerResultsWaiting1ABAD8=0x1ABAD8u;          // 1AB520 Results-waiting replay (A1=0)
inline constexpr std::uint32_t callerRaceInput1A8958=0x1A8958u;               // 1A87A0 race input restart key (A1=0)
inline constexpr std::uint32_t callerPauseRestart1BD934=0x1BD934u;
inline constexpr std::uint32_t callerTimeTrial1ABC0C=0x1ABC0Cu;
inline constexpr std::uint32_t callerPractice1ABC1C=0x1ABC1Cu;                 // branch 1ABC1C; jal site 1ABD28

struct Request final {
    Reason reason=Reason::fresh;
    std::uint32_t a1=0;             // 1AC8E0 a1 (Pause RESTART passes 1)
    std::uint32_t caller=callerFresh1A6AA0;
    std::uint32_t mode11258=1;      // launch subtype (216690 keeps CB4 when 3)
};

// Ordered steps; Receipt::reached names the last step entered.
// W0 round 2: the original 1AC8E0 order (r9 #2, checked against dis_all.txt).
enum class Step : std::uint8_t {
    triggerReset2DCC70, app11CClear, aural103210, music1AC960, particles161F60, world2702E0, race90,
    racers1AEA70, grid2A2880, startLock1AED30, countdown94, record1ACB18, flush267BE0, tail1ACB7C,
    cameraReset, vcr1ACD68, startOverlay197040, done
};
inline std::uint32_t stepPc(Step s)noexcept {
    static constexpr std::uint32_t pcs[]={0x2DCC70u,0x1AC920u,0x103210u,0x1AC960u,0x161F60u,0x2702E0u,0x1AC994u,
        0x1AEA70u,0x2A2880u,0x1AED30u,0x1ACA98u,0x1ACB18u,0x267BE0u,0x1ACB7Cu,0x1750A0u,0x1ACD68u,0x197040u,0u};
    static_assert(sizeof(pcs)/sizeof(pcs[0])==static_cast<std::size_t>(Step::done)+1u);
    return pcs[static_cast<std::size_t>(s)];
}

// Port-side per-step actions supplied by the integration writer (I3) for the
// owners that live in the runSession locals. Each is called once per begin() in
// the order above (per racer where it takes a slot); returning false refuses the
// start at that step (logged). An empty optional on a step this begin() reaches
// is logged as refused, never skipped silently. Race-internal steps have no hook
// (see the ruling above). W0 round 2: resetRacerDamage217420 removed (217420 is a
// leaf of 216690, jal 216758), resetVcrChannels and backupRecordActor288630
// removed (race78 is race-internal), restartRecordActor and readBestLapCB4 added,
// resetCamera takes a1.
template<class Sig> using Hook=std::optional<interactive_frame50DU::BorrowedCall<Sig>>;
struct Hooks final {
    // 1AEA70 -> 216690 for one racer: CB0/CB8/CC0 (CB4 kept when 11258==3, lap_timing50id.h),
    // CC8/CCC/CD8 and its 217420 leaf (racer +7D0..+7DC, +D38/+D3C zeroed). Restart only.
    Hook<bool(std::size_t slot,std::uint32_t mode11258)noexcept> resetRacerTiming216690;
    // 1AEA70 per racer (restart only; fresh = cold load): 16E830 (AI, all first), 2A2880 rank pose,
    // 7F4/C84 = 0, 2169C0, virtual78(a1 = !race1B0 bit4, a2 = 1), 844 = (1AC8E0 a1 != 0), B0+3C[slot] = 0.
    Hook<bool(std::size_t slot,std::uint32_t a1,bool replay1B0bit4)noexcept> placeOnGrid2A2880;
    Hook<bool()noexcept> lockStart1AED30;                    // 1AED30 start lock (fresh and restart)
    Hook<bool()noexcept> showStartOverlay197040;             // 197040 fresh / 1983A0(raceD4) restart: the race's start overlay
    Hook<bool()noexcept> restartRecordActor;                 // 1ACB18 B0+40 vtable+18 record actor (11258==3 and bit4 clear only)
    // 1ACBA0 per human 1AF330/1750A0; unless race1B0 bit4 (1ACC24): 1AFC70(B4), then 1ACD44
    // a1 != 0: 175430(race48) + 1A9750(race,1); a1 == 0: 1AFD80(B4) countdown re-arm.
    Hook<bool(std::uint32_t a1,bool replay1B0bit4)noexcept> resetCamera;
    // Read only, for common_race_start best_lap_cb4= (PLAN 5.5 run 3): the human slot's CB4
    // after the 216690 reset. Empty = the field is logged as 0; never a refusal.
    Hook<bool(std::size_t slot,float& bestLapCB4)noexcept> readBestLapCB4;
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
