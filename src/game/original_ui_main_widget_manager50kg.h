#pragma once
#include "original_ui_icon_layout50kg.h"
#include "original_ui_movement_widget50kg.h"
#include "original_ui_text_widget50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace rr::game {
constexpr std::size_t originalUiMainWidgetCount50KG=49;
constexpr std::size_t originalUiMainPageCount50KG=5;
enum class OriginalUiMainWidgetKind50KG : std::uint8_t { absent,text,icon,empty,movement };
enum class OriginalUiMainWidgetStatus50KG : std::uint8_t {
    ok,noChange,hidden,lockedOut,unavailable,notBound,alreadyBound,invalidArchive,
    parseFailure,allocation,unsupportedWidget,unsupportedText,invalidWidget,
    invalidPage,invalidCommand,invalidLine,invalidInput,invalidMetrics,
    navigationFailure,textFailure,movementFailure,missingResource,alreadyPrepared,layoutFailure
};
struct OriginalUiMainWidgetDiagnostic50KG {
    OriginalUiMainWidgetStatus50KG status=OriginalUiMainWidgetStatus50KG::ok;
    assets::OriginalUiPageDiagnostic50KG parse{};
    OriginalUiNavigationStatus50KG navigation=OriginalUiNavigationStatus50KG::ok;
    OriginalUiTextWidgetStatus50KG text=OriginalUiTextWidgetStatus50KG::ok;
    OriginalUiMovementDiagnostic50KG movement{};
    OriginalUiIconLayoutStatus50KG icon=OriginalUiIconLayoutStatus50KG::complete;
    std::uint16_t pageId=0,widgetId=0,controlId=0;
    std::size_t commandIndex=static_cast<std::size_t>(-1),membershipIndex=static_cast<std::size_t>(-1);
    std::size_t lineIndex=static_cast<std::size_t>(-1),commandCount=0,callbackCount=0;
    std::uint32_t sourcePC=0;
};
struct OriginalUiMainWidgetState50KG {
    OriginalUiMainWidgetKind50KG kind=OriginalUiMainWidgetKind50KG::absent;
    bool selected=false,shown=false,onTop=false;
    std::array<bool,2> wantsJoystickInput{{false,false}};
};
struct OriginalUiMainWidgetOrder50KG {
    // Ordinary widgets, actual manager setup/page children, then OnTop.
    // These are child segments, never two rendering passes. Page controls
    // reference these same objects; they are not widget ancestors. Empty4/10
    // retain null ID slots and appear in neither attachment segment.
    std::vector<std::uint16_t> ordinaryWidgets,onTopWidgets;
};
struct OriginalUiMainInputRegistrations50KG {
    // Source control-construction order, then each authored member order.
    // Duplicates remain registrations; they never duplicate entity children.
    std::array<std::vector<std::uint16_t>,2> joystick;
};
struct OriginalUiMainTextMetrics50KG {
    float fontLineHeight=0;
    std::int32_t measuredWidth=0,measuredHeight=0;
};
struct OriginalUiMainTextProjection50KG {
    const assets::OriginalUiOverlayText50KG* descriptor=nullptr;
    std::uint16_t widgetId=0;
    std::size_t lineIndex=0;
    OriginalUiTextPosition50KG position{};
    OriginalUiTextLineProjection50KG line{};
};
struct OriginalUiMainIconProjection50KG {
    const assets::OriginalUiIcon50KG* descriptor=nullptr;
    const assets::OriginalUiScreenAssets50KG* images=nullptr;
    std::uint32_t initialImageIndex=0;
    OriginalUiIconLayout50KG layout{};
};
// Only the widget/control portions of these real source callbacks. The owner
// does not claim their audio, profile, camera, effect or global-state effects.
enum class OriginalUiMainWidgetPhase50KG : std::uint8_t {
    requestedPage90,previousPage94,entry74,openingAC,openedB0,closingB4
};
struct OriginalUiMainWidgetPhaseInput50KG {
    // For AC/B0, caller reads the actual original profile: +6E0 selected slot,
    // and packed byte +11018. AC tests (profile11018 & 0x60), not route support.
    bool originalProfileKnown=false;
    std::uint16_t savedSlot6E0=0;
    std::uint8_t profile11018=0;
};
struct OriginalUiMainMovementDelivery50KG {
    // Main1E5E00 returns1 whenever actually invoked, even when its own
    // popup/input/controller gates suppress navigation. pageAccepted is not
    // inferred from a noChange/unavailable navigation result.
    bool containerReached=false,pageInputEnabled268=false,pageAccepted=false;
};

// Shared native widget population for the authenticated UIMAIN manager.
// This replaces neither the strict all-text owner nor any page controller.
// One bank serves all five original pages. Parsing their descriptors does not
// admit their movie/popup/controller/route behavior or make those routes ready.
class OriginalUiMainWidgetManager50KG final {
public:
    OriginalUiMainWidgetManager50KG() noexcept;
    ~OriginalUiMainWidgetManager50KG();
    OriginalUiMainWidgetManager50KG(const OriginalUiMainWidgetManager50KG&)=delete;
    OriginalUiMainWidgetManager50KG& operator=(const OriginalUiMainWidgetManager50KG&)=delete;
    OriginalUiMainWidgetManager50KG(OriginalUiMainWidgetManager50KG&&)=delete;
    OriginalUiMainWidgetManager50KG& operator=(OriginalUiMainWidgetManager50KG&&)=delete;

    // Atomic cold load, fixed addresses after publication. The real mainMenu
    // archive is pinned; English is borrowed only while resolving copied text.
    // Reads actual ui.dat PageFile entries and Widgets.Dat, never a fabricated
    // typed descriptor. No texture decode, GPU call, clock or guest execution.
    OriginalUiMainWidgetStatus50KG bindMain(
        std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const assets::OriginalUiEnglish50KG*,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    bool bound() const noexcept;
    const assets::OriginalUiDat50KG* uiDat() const noexcept;
    const assets::OriginalUiDat50KG* widgetsDat() const noexcept;
    const assets::OriginalUiDat50KG* pageDat(std::uint16_t pageId) const noexcept;
    const assets::OriginalUiPage50KG* page(std::uint16_t pageId) const noexcept;
    const OriginalUiNavigation50KG* navigation(std::uint16_t pageId) const noexcept;
    const OriginalUiMainWidgetOrder50KG* managerWidgetOrder() const noexcept;
    const OriginalUiMainInputRegistrations50KG* inputRegistrations(std::uint16_t pageId) const noexcept;
    OriginalUiMainWidgetState50KG widgetState(std::uint16_t widgetId) const noexcept;
    const assets::OriginalUiOverlayText50KG* textDescriptor(std::uint16_t widgetId) const noexcept;
    const assets::OriginalUiIcon50KG* iconDescriptor(std::uint16_t widgetId) const noexcept;
    const OriginalUiTextWidget50KG* textWidget(std::uint16_t widgetId) const noexcept;
    const OriginalUiMovementWidget50KG* movementWidget(std::uint16_t widgetId) const noexcept;

    // Every callback is performed in command/member order, including repeats.
    // A complete batch is checked before any runtime write. Control enabled
    // invokes source nonvirtual2A7A00, not text enable/disable virtual80/84.
    OriginalUiMainWidgetStatus50KG applyCommands(std::uint16_t pageId,
        const OriginalUiCommands50KG&,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    OriginalUiMainWidgetStatus50KG applyWidget(std::uint16_t widgetId,
        OriginalUiControlOperation50KG,bool,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    OriginalUiMainWidgetStatus50KG enterGroup(std::uint16_t pageId,std::uint16_t groupId,
        std::uint16_t slot,std::uint8_t controller,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    OriginalUiMainWidgetStatus50KG moveControl(std::uint16_t pageId,std::uint16_t groupId,
        bool forward,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    OriginalUiMainWidgetStatus50KG enableControl(std::uint16_t pageId,std::uint16_t controlId,
        bool enabled,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    OriginalUiMainWidgetStatus50KG dispatchMain(std::uint8_t controller,OriginalUiMainInput50KG,
        const OriginalUiMainContext50KG&,OriginalUiMainDecision50KG&,
        OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    // Call after the real page callback. Snapshots source selected+38 for the
    // registered widgets before dispatch, never replaces it with visibility.
    // Actual bank registers only widget3 once on Main/controller0. Ordinary
    // Main Group1 has zero associations, so nothing here selects widget3.
    OriginalUiMainWidgetStatus50KG dispatchMainMovement(std::uint8_t controller,
        assets::OriginalUiMovementInput50KG,const OriginalUiMainMovementDelivery50KG&,
        OriginalUiMovementNotification50KG&,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    // Serial source callback points only. All widget/nav changes are atomic
    // after preflight. Completion certifies these widget effects alone.
    OriginalUiMainWidgetStatus50KG applyMainWidgetPhase(OriginalUiMainWidgetPhase50KG,
        const OriginalUiMainWidgetPhaseInput50KG&,OriginalUiMainWidgetDiagnostic50KG&) noexcept;

    // Each Icon gets its initial ScreenTexture atlas only once. Caller must
    // resolve the real original material/pass to this actual decoded image;
    // an arbitrary atlas with matching dimensions is not sufficient. Owner
    // pins the immutable screen-assets lifetime. No synthesized image/material,
    // alpha policy, animation state, GPU readiness or draw completion implied.
    // Unresolved icons do not prevent CPU widget/navigation initialization.
    OriginalUiMainWidgetStatus50KG prepareIcon(std::uint16_t widgetId,
        std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,std::uint32_t imageIndex,
        std::int32_t sourceVideoDepthMaximum,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    OriginalUiMainWidgetStatus50KG projectIcon(std::uint16_t widgetId,bool ancestorsShown,
        OriginalUiMainIconProjection50KG&,OriginalUiMainWidgetDiagnostic50KG&) const noexcept;
    OriginalUiMainWidgetStatus50KG updateText(std::uint16_t widgetId,bool ancestorsShown,
        OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    OriginalUiMainWidgetStatus50KG projectText(std::uint16_t widgetId,std::size_t line,
        const OriginalUiMainTextMetrics50KG&,bool ancestorsShown,
        OriginalUiMainTextProjection50KG&,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    OriginalUiMainWidgetStatus50KG setupLine(std::uint16_t widgetId,std::size_t line,
        const OriginalUiTextLineSetup50KG&,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    OriginalUiMainWidgetStatus50KG setLineColor(std::uint16_t widgetId,std::size_t line,
        std::uint32_t,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    OriginalUiMainWidgetStatus50KG setLineAlpha(std::uint16_t widgetId,std::size_t line,
        std::int32_t,float duration,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    OriginalUiMainWidgetStatus50KG stopLine(std::uint16_t widgetId,std::size_t line,
        OriginalUiMainWidgetDiagnostic50KG&) noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
    OriginalUiMainWidgetStatus50KG preflight(std::uint16_t,const OriginalUiCommands50KG&,
        OriginalUiMainWidgetDiagnostic50KG&) const noexcept;
    void commitWidget(std::uint16_t,OriginalUiControlOperation50KG,bool) noexcept;
    void commitCommands(std::uint16_t,const OriginalUiCommands50KG&) noexcept;
    OriginalUiMainWidgetStatus50KG finishNavigation(std::uint16_t,OriginalUiNavigation50KG&,
        const OriginalUiCommands50KG&,OriginalUiNavigationStatus50KG,
        OriginalUiMainWidgetDiagnostic50KG&) noexcept;
    OriginalUiTextWidget50KG* mutableText(std::uint16_t,OriginalUiMainWidgetDiagnostic50KG&) noexcept;
};
}
