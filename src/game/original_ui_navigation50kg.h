#pragma once
#include "../assets/original_ui_pages50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
enum class OriginalUiControlOperation50KG : std::uint8_t {
    show, hide, select, deselect, clearSelection, setEnabled, reset
};
struct OriginalUiControlCommand50KG {
    std::uint16_t controlId=0;
    OriginalUiControlOperation50KG operation=OriginalUiControlOperation50KG::deselect;
    bool value=false;
    std::uint32_t sourcePC=0;
};
constexpr std::size_t originalUiCommandLimit50KG=assets::originalUiMembershipLimit50KG*2+8;
struct OriginalUiCommands50KG {
    std::array<OriginalUiControlCommand50KG,originalUiCommandLimit50KG> values{};
    std::size_t count=0;
};
struct OriginalUiControlState50KG { bool enabled=false, blocked=false, selected=false, shown=false; };
struct OriginalUiGroupState50KG { bool enabled=true; std::uint16_t selectedSlot=0; };
struct OriginalUiNavigation50KG {
    // Immutable owner must outlive this state. No guest addresses are retained.
    const assets::OriginalUiPage50KG* page=nullptr;
    std::array<OriginalUiControlState50KG,assets::originalUiControlLimit50KG> controls{};
    std::array<OriginalUiGroupState50KG,assets::originalUiGroupLimit50KG> groups{};
    std::array<std::int16_t,2> controllerGroup{{-1,-1}};
};
enum class OriginalUiNavigationStatus50KG : std::uint8_t {
    ok, noChange, invalidPage, invalidGroup, invalidSlot, invalidController,
    noSelectableControl, commandCapacity, unsupportedInput, unavailable
};
// Cold bind validates the typed model again; allocation-free and atomic.
bool bindOriginalUiNavigation50KG(const assets::OriginalUiPage50KG&,
    OriginalUiNavigation50KG&) noexcept;
// Source 0x2A6BE0 entry selection: deselect unowned groups 1..N, select target
// associations, select the requested one-based slot, attach controller.
OriginalUiNavigationStatus50KG enterOriginalUiGroup50KG(OriginalUiNavigation50KG&,
    std::uint16_t groupId,std::uint16_t slot,std::uint8_t controller,
    OriginalUiCommands50KG&) noexcept;
// Source 0x2A50F0 / 0x2A51B0: wrap in group order, skipping -1 holes,
// disabled or blocked controls. Native fail-closed bound replaces the
// original infinite scan when every control is ineligible; failure is atomic.
OriginalUiNavigationStatus50KG moveOriginalUiControl50KG(OriginalUiNavigation50KG&,
    std::uint16_t groupId,bool forward,OriginalUiCommands50KG&) noexcept;
// Source 2A6BC0: select one slot without attaching a controller or changing
// associations. Source 2A68E0/2A69B0: move the attached group, retaining its slot.
OriginalUiNavigationStatus50KG selectOriginalUiGroupSlot50KG(OriginalUiNavigation50KG&,
    std::uint16_t groupId,std::uint16_t slot,OriginalUiCommands50KG&) noexcept;
OriginalUiNavigationStatus50KG moveOriginalUiGroup50KG(OriginalUiNavigation50KG&,
    std::uint8_t controller,bool forward,OriginalUiCommands50KG&) noexcept;
OriginalUiNavigationStatus50KG enableOriginalUiControl50KG(OriginalUiNavigation50KG&,
    std::uint16_t controlId,bool enabled,OriginalUiCommands50KG&) noexcept;
// Consumers expand every command through page.controls[id].widgets in listed
// order, preserving duplicates. Commands are callback requests, not proof
// that a renderer/animation/sound effect has completed. The UIWidget callbacks
// (selection textures, motions and visibility flags) remain renderer-owned.

// Event indices returned by 0x2A7410; PC key mapping belongs to the host.
enum class OriginalUiMainInput50KG : std::uint8_t { previous=2,next=3,activate=4 };
enum class OriginalUiMainAction50KG : std::uint8_t {
    none, changedSelection, changeScreen, changePage, secondControllerRequired,
    unavailableMode, unavailableScreen
};
struct OriginalUiMainContext50KG {
    bool popupActive=false,sourceInputBlocked=false,secondControllerPresent=false;
    // Explicit ORIGINAL-menu admission only. This mask is NOT RR_RACE_MODE,
    // which belongs to a separate native host timing/audio domain. Unsupported
    // original menu items keep
    // their authentic labels and selection positions, returning unavailable.
    std::uint8_t supportedGameModes=0; // bits 0,1,2 correspond to original modes.
    bool supportsScreen12=false,supportsPage5=false;
};
struct OriginalUiMainDecision50KG {
    OriginalUiMainAction50KG action=OriginalUiMainAction50KG::none;
    std::uint16_t selectedSlot=0;
    std::int8_t originalGameMode=-1; // never remap mode0 to mode2
    std::uint8_t originalPlayerBits=0; // source options byte bits 0x20/0x40
    std::int16_t routeArgument=-1,targetScreen=-1;
    std::uint32_t sourcePC=0;
};
// Native translation of 0x1E5E00's bounded main-menu dispatch. Navigation
// mutates only native selection state; accepted activations return an exact
// original route request. They do not start gameplay, modify options/save
// files or mark a backend operation complete. Second-controller error popup
// rendering and original sound IDs are outside this leaf.
OriginalUiNavigationStatus50KG dispatchOriginalUiMain50KG(OriginalUiNavigation50KG&,
    std::uint8_t controller,OriginalUiMainInput50KG,const OriginalUiMainContext50KG&,
    OriginalUiCommands50KG&,OriginalUiMainDecision50KG&) noexcept;
}
