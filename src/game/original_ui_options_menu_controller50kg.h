#pragma once
#include "original_ui_route_decisions50kg.h"
#include "original_ui_movement_widget50kg.h"
#include <array>
#include <memory>

namespace rr::game {
// Options menu is screen12/page1/menu.dat, NOT page4/options.dat.
// This owner supplies native controller proposals, never rendered readiness.
enum class OriginalUiOptionsMenuStatus50KG : std::uint8_t {
    ok,noChange,unavailable,notBound,alreadyBound,invalidArchive,parseFailure,
    allocation,invalidPage,invalidInput,navigationFailure,capacity
};
enum class OriginalUiOptionsMenuPhase50KG : std::uint8_t {
    requestedPage90,previousPage94,entry74,openingAC,openedB0,closingB4
};
struct OriginalUiOptionsMenuCallback50KG {
    std::uint16_t widgetId=0;
    OriginalUiControlOperation50KG operation=OriginalUiControlOperation50KG::hide;
    bool value=false;
    std::uint32_t sourcePC=0;
};
struct OriginalUiOptionsMenuPlan50KG {
    // Apply callbacks in order, including duplicates. Preflight the ENTIRE
    // callback batch before applying any callback or publishing navigation.
    // Keep the caller's source snapshot current throughout the transaction.
    OriginalUiNavigation50KG navigation{};
    std::array<OriginalUiOptionsMenuCallback50KG,128> callbacks{};
    std::size_t callbackCount=0;
    bool writesPageInputFlags42C=false;
    std::uint32_t pageInputFlags42C=0;
};
struct OriginalUiOptionsMenuInput50KG {
    // Supply the actual container delivery and selection snapshot, not guessed
    // UI visibility. The page always returns1 if invoked, including ignored input.
    bool containerReached=false,pageInputEnabled268=false;
    bool movementSelectedAtSnapshot=false;
};
struct OriginalUiOptionsMenuPageResult50KG {
    bool pageAccepted=false;
    OriginalUiRouteStatus50KG routeStatus=OriginalUiRouteStatus50KG::noChange;
    OriginalUiRouteDecision50KG route{};
    // Source audio request only; requires real sound owner and flush owner.
    bool soundRequested=false,flushRequested=false;
    std::uint8_t soundId=0;
    std::uint32_t soundPC=0,flushPC=0;
};
class OriginalUiOptionsMenuController50KG final {
public:
    OriginalUiOptionsMenuController50KG() noexcept;
    ~OriginalUiOptionsMenuController50KG();
    OriginalUiOptionsMenuController50KG(const OriginalUiOptionsMenuController50KG&)=delete;
    OriginalUiOptionsMenuController50KG& operator=(const OriginalUiOptionsMenuController50KG&)=delete;
    // Pins authenticated original archive; reads page/widget DAT itself.
    // Only actual menu membership and Movement_Control5 are admitted here.
    OriginalUiOptionsMenuStatus50KG bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        assets::OriginalUiPageDiagnostic50KG&) noexcept;
    const assets::OriginalUiPage50KG* page() const noexcept;
    const assets::OriginalUiDat50KG* widgetsDat() const noexcept;
    bool initialNavigation(OriginalUiNavigation50KG&) const noexcept;
    OriginalUiOptionsMenuStatus50KG phase(const OriginalUiNavigation50KG&,
        OriginalUiOptionsMenuPhase50KG,bool originalProfileKnown,std::uint16_t savedSlot6E4,
        OriginalUiOptionsMenuPlan50KG&) const noexcept;
    // Page callback only; directions play audio but NEVER move selection.
    // Uses existing route leaf, with caller-supplied real destination admission.
    OriginalUiOptionsMenuStatus50KG pageInput(const OriginalUiNavigation50KG&,
        std::uint8_t controller,assets::OriginalUiMovementInput50KG,
        const OriginalUiOptionsMenuInput50KG&,const OriginalUiOptionsContext50KG&,
        OriginalUiOptionsMenuPageResult50KG&) const noexcept;
    // Invoke AFTER pageInput and snapshot widget5 selected+38 after that page
    // callback. Source popup/controller early exits still return pageAccepted.
    OriginalUiOptionsMenuStatus50KG movementInput(const OriginalUiNavigation50KG&,
        std::uint8_t controller,assets::OriginalUiMovementInput50KG,
        const OriginalUiOptionsMenuInput50KG&,bool pageAccepted,
        OriginalUiOptionsMenuPlan50KG&,OriginalUiMovementNotification50KG&) const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
