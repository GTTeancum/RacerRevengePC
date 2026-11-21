#pragma once
// DLC career: an add-on pod as the PLAYER in the stock tournament and the
// Garage. PORT EXTENSION, NOT ORIGINAL BEHAVIOUR.
// Design: work\scratch\dlc\dlc-career-design.txt (user decision 2026-11-16:
// "DLC racers can run tournament, they just don't appear as NPC racers. They
// get upgrades too.").
//
// - The original keeps 27 careers in the profile (P+1920 + k*0x894), indexed by
//   the selected career P+100C0 (0..26). An add-on number (32..99) does not fit
//   that table and is never written into it: the profile's 100C0 / 1125D keep
//   the ANCHOR, the stock rider whose original Rider Confirm ran (exactly as the
//   free-play add-on Confirm does, dlc_rider_entries50dl.h), and the add-on
//   career lives HERE, host-side, while it is latched.
// - The add-on career has the stock career shape: the scalars (2190 earned,
//   2194 unspent, 2198 kills, 219C progress), 27 stock pod rows (the AI
//   opponents' abilities, medals and prizes, as off-diagonal rows of a stock
//   career) and the add-on pod's OWN row (the original's diagonal row).
// - Results run the same 1875C0/187D80 body as a stock career
//   (applyAddonTournamentResult50DL, original_tournament_result50kg.cpp). The
//   187D80 availability step is never written into the stock byte 11018; the
//   availability an add-on career earns by that same rule is merged at READ
//   time (addonCareerAvailability; user decision 2026-11-21 "Pods should all
//   be treated the same").
// - Persistence: the DLC side save (dlc_progress50dl.h, rrpc-dlc-progress/3)
//   keeps the whole career: scalars, own row, the 27 AI rows, the 219D name
//   and the 102CC flag. A /2 entry (no AI rows) is seeded from base0180 at the
//   next latch, as 1800E0 seeds a fresh career.
// - Every write is bounds-checked first and refused whole: nothing is changed
//   when any value does not fit (abilities finite, base <= value <= maximum for
//   the own row, medals 0..8, progress 0..13, name <= 20 printable characters).
// - With no add-on latched every query answers "none" and the stock path runs
//   exactly as before.
#include "original_tournament_result50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace rr::game {
// The one 1875C0 + 187D80 body for an add-on career (defined beside the stock
// applyOriginalTournamentResult50KG). ownPod = the add-on number (32..99).
// Writes career/own/availability/boundary/sharedRandom only on a complete receipt.
OriginalTournamentResultReceipt50KG applyAddonTournamentResult50DL(
    OriginalTournamentCareer50KG& career,OriginalTournamentPodCareer50KG& own,std::uint32_t ownPod,
    const std::array<std::array<float,6>,27>& base0180,const std::array<std::array<float,6>,27>& maximum0408,
    std::uint8_t selectedTrack,const OriginalTournamentResultRacer50KG* rows,std::size_t count,
    std::uint32_t humans,std::uint8_t& availability,std::uint32_t& boundary102CC,
    std::uint32_t& sharedRandom) noexcept;
}

namespace rr::game::dlc50DL {

inline constexpr std::size_t addonCareerNameChars=20; // 219D: 21 bytes with NUL

struct AddonCareer50DL {
    unsigned number=0;                 // add-on registry number 32..99
    unsigned anchor=0;                 // stock rider held in 100C0 / 1125D (1..26)
    std::string key;                   // "<package-id>/<item-id>", the side-save key
    OriginalTournamentCareer50KG career{}; // scalars + 27 stock (AI) pod rows
    OriginalTournamentPodCareer50KG own{};  // the add-on pod's own row
    std::array<float,6> base{},maximum{};   // the add-on PHY [BaseAbilities] / [MaxAbilities]
    std::uint32_t boundary102CC=0;     // circuit-boundary flag of this career's last result
    std::string name;                  // the 219D career name (side save)
};

// Latch the add-on career for the tournament Rider Confirm that ran the
// original Confirm for `anchorRider`. Loads the stored career from the DLC side
// save (or a fresh seeded one) and seeds the 27 AI rows from the stock
// constructor table base0180 with the stock remaining prizes (the rows
// 1800E0 gives a fresh stock career). Refused (and nothing latched) for a
// number that is not an admitted add-on pod or an anchor outside 1..26.
bool latchTournamentCareer(unsigned anchorRider,unsigned addonNumber,
    const std::array<std::array<float,6>,27>& base0180,
    const std::array<std::int16_t,13>& constructorPrizes) noexcept;
// The same latch with the stock tables bound at startup (bindStockCareerTables):
// the Rider session calls this at the add-on tournament Confirm, before the
// Tournament destination is prepared, so every snapshot taken for that
// destination already reads the add-on career. Refused when none are bound.
bool latchTournamentCareer(unsigned anchorRider,unsigned addonNumber) noexcept;
void clearTournamentCareer() noexcept;
// The latched career when the profile's selected career (100C0) is its
// anchor; nullptr otherwise (stock career, or nothing latched). The profile
// owner calls this only in mode 2.
const AddonCareer50DL* activeAddonCareer(unsigned comparison100C0) noexcept;

// The race helper (dlc_rider_entries50dl latchRaceRider): the add-on that
// races for `profileRider` in a tournament launch, 0 = none. Only after a
// tournament Course launch (noteCourseLaunch(true)) of the latched anchor.
void noteCourseLaunch(bool tournament) noexcept;
unsigned tournamentRaceAddon(unsigned profileRider) noexcept;

// ---- commits (the profile owner calls these in place of its stock-table
// writes while an add-on career is active). Each is all or nothing, writes
// the DLC side save state through DlcProgress50DL::commitCareer (saved at the
// next stock save moment by saveIfChanged), and refuses when it cannot.

// Garage 1FEB90: the six own-row abilities and the unspent credits 2194.
bool commitAddonGarage(unsigned comparison100C0,const std::array<float,6>& abilities,
    std::uint32_t unspent2194) noexcept;
// 1875C0/187D80 for the active add-on career. base0180 / maximum0408: the
// stock constructor/PHY tables (read-only). availability11018: the merged
// availability the profile reads (read only); the rule's advanced copy is
// reported in the receipt and not stored (addonCareerAvailability derives it).
bool commitAddonResult(unsigned comparison100C0,std::uint8_t selectedTrack,
    const OriginalTournamentResultRacer50KG* rows,std::size_t count,std::uint32_t humans,
    const std::array<std::array<float,6>,27>& base0180,const std::array<std::array<float,6>,27>& maximum0408,
    std::uint8_t availability11018,std::uint32_t& sharedRandom,
    OriginalTournamentResultReceipt50KG& receipt) noexcept;
// 1CA528 mode 3 in a tournament: the career name (<= 20 printable characters).
bool storeAddonCareerName(unsigned comparison100C0,std::string_view name) noexcept;
// The stock constructor tables a fresh career is seeded from (base0180 and the
// prize halves 100C8), bound once by the profile owner at startup (read-only copy).
void bindStockCareerTables(const std::array<std::array<float,6>,27>& base0180,
    const std::array<std::int16_t,13>& constructorPrizes) noexcept;
// 1800E0 for an add-on career: a fresh seeded career (side save replaced).
// canResetAddonCareer: the original 1EAFB0 prompt rule (the career's own
// medal 0 is nonzero), on the add-on career.
bool canResetAddonCareer(unsigned addonNumber) noexcept;
bool resetAddonCareer(unsigned addonNumber) noexcept;

// Read-only view of an add-on pod's career (the latched one, else the side
// save's stored or seeded one): Rider-page stats and gates.
struct AddonCareerView50DL {
    OriginalTournamentPodCareer50KG own{};
    std::uint32_t earned2190=0,unspent2194=0,kills2198=0;
    std::uint8_t progress219C=0;
};
bool addonCareerView(unsigned addonNumber,AddonCareerView50DL& out) noexcept;

// Course availability (11018 bits 0..4) earned by add-on careers, by the
// original 187D80 rule. 187D80 raises availability by one whenever the
// career's progress equals it, so for a career whose availability starts at
// or above its progress the rule's result is min(13, progress + 1); over every
// stored add-on career the value is the largest. 0 = no add-on career. The
// profile's availability READS take max(stock, this) in bits 0..4; the stock
// byte itself is never written by an add-on career (stock save untouched).
std::uint8_t addonCareerAvailability() noexcept;

} // namespace rr::game::dlc50DL
