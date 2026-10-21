#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
namespace rr::game {
// Source-only ordered effect plan. Successful planning is NOT page admission,
// popup construction, camera activation, audio playback or a renderer receipt.
enum class OriginalUiTournamentCleanPhase50KG : std::uint8_t {entry74,openingAC,openedB0,closingB4,closedB8};
enum class OriginalUiTournamentCleanEffect50KG : std::uint8_t {
 promptMask,groupEnabled,widgetEnabled,hidePopup438,resetExit4060,
 resetTimer4064,showCamera4BC,activateCamera4BC,resetSharedPopupResult3D8,
 applyBackground430,startSong11,fadeMusic1
};
struct OriginalUiTournamentCleanStep50KG {
 OriginalUiTournamentCleanEffect50KG kind{};
 std::uint32_t argument=0,value=0,sourcePC=0;
};
struct OriginalUiTournamentCleanPlan50KG {
 std::array<OriginalUiTournamentCleanStep50KG,16> steps{};
 std::size_t count=0;
};
// Caller supplies observations from the SAME retained source profile and page.
// Dirty entry is deliberately unsupported: never convert it to clean/absent.
// Other phases have no original dirty test; clean-only session admission owns
// that invariant. No caller-provided readiness value is accepted here.
bool planOriginalUiTournamentCleanPhase50KG(OriginalUiTournamentCleanPhase50KG,
 bool actualCardFlag100BC,bool actualCamera4BC,OriginalUiTournamentCleanPlan50KG&) noexcept;
// Source1F2470 subtracts only while timer>0, and does NOT clamp at zero.
// These numeric results do not execute inherited2A6BB0 child update.
bool updateOriginalUiTournamentTimer50KG(float timer4064,float delta298,
 float& nextTimer4064) noexcept;
}
