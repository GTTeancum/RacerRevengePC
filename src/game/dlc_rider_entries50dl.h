#pragma once
// Add-on riders for the free-play Rider screen (DLC phase 2): rows and the
// port-owned selection. PORT EXTENSION, NOT ORIGINAL BEHAVIOUR.
// Design: work\scratch\dlc\DESIGN.md 4.3 / 4.4, user decisions D4-D7.
//
// - Rows come only from the frozen DLC registry (src\assets\dlc_registry50dl.h),
//   in number order, after the stock riders (D4), unlocked at once (D5).
// - The selection of an add-on rider lives HERE, never in an original field:
//   the profile's packed 1125D rider bits, the remembered-rider words and the
//   stock save keep the last STOCK selection (an add-on number >= 32 would not
//   fit the original 5-bit field and is refused by its encoding anyway).
// - AI (D6): an add-on pod may be an opponent only on an add-on course and only
//   when its package allows it; never on a stock course, so a stock race is the
//   same with or without DLC installed.
// - HOOK (DLC save side file, helper 146s): records set with an add-on rider go
//   to the side file keyed by riderKey() (D7 b); nothing here writes them.
// The free-play Rider screen (original_ui_rider_session50kg.cpp) calls select /
// clear / setHighlighted. With no add-on pod every query answers "none", clear
// stores the 0 it already holds, and nothing is logged.
#include <cstddef>
#include <string>

namespace rr::assets::dlc50DL {struct AddonPod;}
namespace rr::game::dlc50DL {

std::size_t addonRiderCount() noexcept;                       // admitted add-on pods
const rr::assets::dlc50DL::AddonPod* addonRiderAt(std::size_t index) noexcept; // menu order
const rr::assets::dlc50DL::AddonPod* addonRider(unsigned racer) noexcept;      // 32..99

// Port-owned free-play selection, one per human player: player 0 = Rider page
// 5/1 (P1), player 1 = Rider page 5/2 (P2, two-player VS). Both players may pick
// add-on pods, also the same one. select refuses anything that is not an
// admitted add-on number, and any player other than 0 or 1; clear returns that
// player to the stock selection the profile keeps (P1: 1125D, P2: 1125E).
// The player-less forms are player 0 (P1): the race helper reads selectedAddonRider().
inline constexpr unsigned addonRiderPlayers=2;
bool selectAddonRider(unsigned player,unsigned racer) noexcept;
void clearAddonRider(unsigned player) noexcept;
unsigned selectedAddonRider(unsigned player) noexcept;        // 0 = a stock rider (or no such player)
bool selectAddonRider(unsigned racer) noexcept;               // player 0
void clearAddonRider() noexcept;                              // player 0
unsigned selectedAddonRider() noexcept;                       // player 0; 0 = a stock rider is selected
std::string riderKey(unsigned racer);                         // "stock:NN" or the item key; "" if unknown

// Rider-screen highlight: the add-on number a free-play Rider page shows right
// now, per player as above, 0 = a stock rider (or no Rider page). Written only
// by the Rider session (platform\linux\native\original_ui_rider_session50kg.cpp);
// read by the test navigation and logs. Not a selection: Confirm selects.
// set refuses anything that is neither 0 nor an admitted add-on number.
bool setHighlightedAddonRider(unsigned player,unsigned racer) noexcept;
unsigned highlightedAddonRider(unsigned player) noexcept;
bool setHighlightedAddonRider(unsigned racer) noexcept;       // player 0
unsigned highlightedAddonRider() noexcept;                    // player 0

// Port-owned "remembered" add-on of a Rider page, per player: what Back records
// while an add-on is highlighted (the original Back word keeps its stock rider,
// the anchor). The next entry of that page re-highlights the add-on when it opens
// on the same anchor. A stock Back forgets it. Process memory only, never saved.
struct RememberedAddonRider50DL {unsigned racer=0,anchorRider=0;bool anchorLast=true;};
bool rememberAddonRider(unsigned player,unsigned racer,unsigned anchorRider,bool anchorLast) noexcept;
void forgetAddonRider(unsigned player) noexcept;
bool rememberedAddonRider(unsigned player,RememberedAddonRider50DL&) noexcept; // false = none (or no longer admitted)

// D6: may this racer be an AI opponent on this course?
bool addonRiderMayBeAi(unsigned racer,bool addonCourse) noexcept;

// Test route only: RR_TEST_ADDON_RIDER=<package-id>/<item-id> selects that
// add-on rider once at the first call (one log line either way). Unset = no-op.
void applyTestAddonRider() noexcept;

// Race handoff (free play only). Called once per consumed Course launch, with
// the stock rider the profile holds. Returns the racer that races: the
// port-owned add-on selection when one is selected and the launch is free
// play (mode34 0), otherwise profileRider unchanged. Latches the pair for the
// session and logs one "dlc_race_player" line when an add-on races; with no
// add-on nothing is logged and the result is profileRider.
unsigned latchRaceRider(unsigned profileRider,bool freePlay,const char* track) noexcept;
// Clears the latch (a session without a consumed Course launch).
void clearRaceRider() noexcept;
// The profile's stock rider for a raced racer number: the latched profile
// rider when `raced` is the latched add-on, otherwise `raced` itself. Every
// check of the raced racer against the profile/save goes through this, so
// an add-on number never meets an original profile field.
unsigned profileRider(unsigned raced) noexcept;
// The reverse: the racer that races for the profile's stock rider (the latched
// add-on when the profile rider is the latched one), otherwise profileRider.
unsigned racedRider(unsigned profileRider) noexcept;

} // namespace rr::game::dlc50DL
