#pragma once
// Common rewrite (W0 freeze): the ONE race roster, any pod. PLAN.md 2.9, B-C18, B-P03, B-P04, DLC seams.
//
// The field is built once from rows. Each row is a stock pod (Pods\PodNN\, A5)
// or an add-on pod (dlc:/<pkg>/..., registry row >= 32), marked human or AI.
// Every slot goes through the original 1AA510 -> 1AD520 -> 1AE280 load: humans
// 1ADF80 -> 1AE0A4 -> 2129C0, AI 1AE3CC -> 2129C0, then the wreck chain 165E90.
// Humans load before every AI (1AE280 order). Pod differences come only from the
// pod's own files (piece counts 7 or 9, .phy +50 explosive-wreck speed, materials,
// sounds.ini, announcer INI, UIACCEPT event row). No slot, pod or pilot is named
// in code; the only racer-keyed rules are the original's own (A2 AI steering
// 1730D0, A3 announcer racer 16, A8 pod id 0 / >=100 refused at 210824).
//
// Field kinds (PLAN 2.9): tournament field 188470 over the class tables
// 3A8940/3A8960/3A8980; Free Play the 8 Course-screen entries; Time Trial solo
// (1AD520 with one racer); ALL8 spectator (humans34 == 0). Add-on pods join an
// AI field only through dlc50DL::addonRiderMayBeAi (registry aiOnAddonCourses,
// add-on courses only): this replaces the hard stockPodId refusal at
// terrain_interactive_roster_cold50du.cpp:153-157.
//
// Owner: R10 (common_roster50cr.cpp). Callers: I2 (cold), I3 (Results/Pause binding).
#include "common_race_log50cr.h"
#include "../../src/game/host_race_limits50ht.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace native_racer50HW { class Rig; }
namespace common_track50CR { class Track; }

namespace common_roster50CR {
using Source=common_race_log50CR::Source;
inline constexpr std::size_t maxSlots=rr::game::maxHostRaceRacers50HT;   // 1AD520 field limit (8)

// One field row. Built from data (Course-screen entries, 188470, registry rows).
struct Row final {
    std::uint32_t racer60=0;        // racer id (A5 table / registry number >= 32)
    std::string podId;              // "POD07", or the registry pod id
    Source source=Source::stock;
    bool human=false;
    std::string archive;            // Pods\PodNN\PodNN.res, or the dlc:/ package member
    std::string camera;             // pod CAM member
    std::string archiveSha256;      // listed digest: stock = receipt, registry = overlay check
    std::uint64_t archiveBytes=0;
};
enum class FieldKind : std::uint8_t { tournament188470, freePlay, timeTrialSolo, practice, all8Spectator };
struct Field final {
    FieldKind kind=FieldKind::freePlay;
    std::array<Row,maxSlots> rows{};
    std::size_t count=0;            // 1..8
    std::uint32_t humans34=0;       // manager human count
    // Bounds-checked append; refused (logged common_storage_refused) past maxSlots.
    bool append(const Row& row,std::uint32_t pc)noexcept;                    // R10
};

struct Diagnostic final {
    std::size_t slot=0;
    std::uint32_t pc=0;             // 1AD520 / 1AE280 / 1ADF80 / 1AE3CC / 2129C0 / 165E90
    bool refused=false;
    std::string reason;
};

// ---- field builders (R10). Each fills a Field from data only.
// R10, 188470 deterministic opponents over the class tables 3A8940/3A8960/3A8980
// (rr::game::original_opponent_selection50KG::deterministic188470), human first.
bool buildTournamentField188470(std::uint32_t klass,std::uint32_t humanRacer60,Field& out,Diagnostic& d)noexcept;
// R10: Free Play / practice field from the Course-screen entries (racer60 per
// entry, humans first as 1AD520 commits them); `addonCourse` feeds addonRiderMayBeAi.
bool buildCourseField(const std::uint32_t* racer60,std::size_t entries,std::size_t humans,
    bool addonCourse,FieldKind kind,Field& out,Diagnostic& d)noexcept;
// R10: Time Trial solo field (1AD520 with one human racer).
bool buildTimeTrialField(std::uint32_t humanRacer60,Field& out,Diagnostic& d)noexcept;
// R10: ALL8 spectator field (humans34 == 0) on whatever course the launch names.
bool buildSpectatorField(const std::uint32_t* racer60,std::size_t entries,bool addonCourse,Field& out,Diagnostic& d)noexcept;
// R10: one row from a racer id: stock A5 table (original_racer_selection50IJ /
// original_player_identity50IK) or registry pod row (original_racer_selection50IJ::findAnyByRacer60 / rr::assets::dlc50DL::addonPod). Refuses
// pod id 0 / >=100 (A8, 210824) and an add-on pod in an AI slot unless
// dlc50DL::addonRiderMayBeAi(racer, addonCourse).
bool rowForRacer(std::uint32_t racer60,bool human,bool addonCourse,Row& out,Diagnostic& d)noexcept;

// ---- the roster owner (R10)
class Owner final {
    Field field_{};
    bool built_=false;
public:
    // R10: adopt a built field; logs one common_roster line per slot when each slot loads.
    bool build(const Field& field,Diagnostic& d)noexcept;
    bool built()const noexcept {return built_;}
    const Field& field()const noexcept {return field_;}
    std::size_t count()const noexcept {return built_?field_.count:0u;}
    std::uint32_t humans34()const noexcept {return built_?field_.humans34:0u;}
    const Row* row(std::size_t slot)const noexcept {return built_&&slot<field_.count?&field_.rows[slot]:nullptr;}
    // R10, 1AA510 -> 1AD520 -> 1AE280: load every slot's pod from its own files into
    // `rigs` in the original order (every human 1ADF80 -> 1AE0A4 -> 2129C0 before
    // every AI 1AE3CC -> 2129C0), then the wreck chain 165E90 per slot. The
    // human pod loads through the same 2129C0 PodLoad as every slot (OwnershipBridge,
    // 1A0848 bind for slot < humans34). PORT STAND-IN (W0 round 2, r10 #2): the race+B0
    // manager's own 1ADF80 -> 1AE0A4 source chain is not joined (it needs sourceWorldRows_
    // from 1AA580, and its Pod%02d id cannot name an add-on human). The query bootstrap collision uses each slot's own
    // pod (C2). Logs common_roster and common_pod_receipt lines. Stops at the first
    // refused slot.
    bool loadAll(const common_track50CR::Track& track,
        std::array<std::unique_ptr<native_racer50HW::Rig>,maxSlots>& rigs,Diagnostic& d)noexcept;
    // R10: receipts for one slot's pod files (UIACCEPT event row 1E7C74, sounds.ini
    // [Indices], announcer INI): logs common_pod_receipt; never refuses on a differing receipt.
    void logPodReceipts(std::size_t slot)const;
    void release()noexcept;                                                   // R10 (teardown step 18)
};

} // namespace common_roster50CR
