#pragma once
#include "original_ui_text_widget50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rr::game {
// This is a static-text container owner, NOT a complete screen controller.
// It does not own page transitions, backgrounds, popups, font/material loads,
// complete draw scheduling, depth, blending, routes, host input mapping or GPU submission.
enum class OriginalUiStaticTextPageStatus50KG : std::uint8_t {
    ok, noChange, hidden, lockedOut, unavailable,
    notBound, alreadyBound, invalidArchive, parseFailure, allocation,
    unsupportedWidget, unsupportedField, unsupportedSection,
    unsupportedTextSource, unsupportedPageFeature, unsupportedAnimation,
    invalidCommand, invalidWidget, invalidLine, invalidInput, invalidMetrics,
    navigationFailure, textFailure
};
enum class OriginalUiStaticTextInput50KG : std::uint8_t { none, page, widgets };
constexpr std::size_t originalUiStaticTextNoIndex50KG=static_cast<std::size_t>(-1);
struct OriginalUiStaticTextPageDiagnostic50KG {
    OriginalUiStaticTextPageStatus50KG status=OriginalUiStaticTextPageStatus50KG::ok;
    OriginalUiStaticTextInput50KG input=OriginalUiStaticTextInput50KG::none;
    // Exact parsed spelling, including an unknown key/section on failed bind.
    // A missing required key uses its canonical reader spelling instead.
    // DAT admission already limits each to 127 bytes. No borrowed strings.
    std::array<char,128> section{},key{};
    assets::OriginalUiPageDiagnostic50KG parse{};
    OriginalUiNavigationStatus50KG navigation=OriginalUiNavigationStatus50KG::ok;
    OriginalUiTextWidgetStatus50KG text=OriginalUiTextWidgetStatus50KG::ok;
    std::uint16_t widgetId=0,controlId=0;
    std::size_t commandIndex=originalUiStaticTextNoIndex50KG;
    std::size_t membershipIndex=originalUiStaticTextNoIndex50KG;
    std::size_t lineIndex=originalUiStaticTextNoIndex50KG;
    std::size_t commandCount=0,callbackCount=0;
    // For admission failures this is an original leaf anchor when available;
    // zero marks a new native restriction, never an invented original PC.
    std::uint32_t sourcePC=0;
};
struct OriginalUiStaticTextMetrics50KG {
    float fontLineHeight=0;
    // Caller supplies original ScreenText GetWidth/GetHeight integer results,
    // e.g. using roundOriginalUiTextMetric50KG. No host metric substitution.
    std::int32_t measuredWidth=0,measuredHeight=0;
};
struct OriginalUiStaticTextProjection50KG {
    const assets::OriginalUiOverlayText50KG* descriptor=nullptr;
    std::uint16_t widgetId=0;
    std::size_t lineIndex=0;
    OriginalUiTextPosition50KG position{};
    OriginalUiTextLineProjection50KG line{};
};

// Native immutable projection of PodUIManager child attachment order for the
// admitted all-Overlay_Text companion Widgets DAT. Source 1CD7A0 attaches
// ordinary widgets immediately; 1CDA70 appends the deferred head list after
// original setup/page children, hence descending OnTop IDs. Both vectors use
// one-based widget IDs and together contain every declared widget once.
// Control membership/callback duplication does not duplicate tree children.
struct OriginalUiStaticTextManagerOrder50KG {
    std::vector<std::uint16_t> ordinaryWidgets;
    std::vector<std::uint16_t> onTopWidgets;
};

class OriginalUiStaticTextPageOwner50KG final {
public:
    OriginalUiStaticTextPageOwner50KG() noexcept;
    ~OriginalUiStaticTextPageOwner50KG();
    OriginalUiStaticTextPageOwner50KG(const OriginalUiStaticTextPageOwner50KG&)=delete;
    OriginalUiStaticTextPageOwner50KG& operator=(const OriginalUiStaticTextPageOwner50KG&)=delete;
    OriginalUiStaticTextPageOwner50KG(OriginalUiStaticTextPageOwner50KG&&)=delete;
    OriginalUiStaticTextPageOwner50KG& operator=(OriginalUiStaticTextPageOwner50KG&&)=delete;

    // Cold, load-once, all-or-nothing. Parses authenticated members itself and
    // pins their immutable archive owners. No fabricated typed descriptor input.
    // Every declared widget must be admitted Overlay_Text, including widgets
    // unused by this page. Unknown fields/sections and dynamic text fail closed.
    // Popup and video requirements are unsupported. NumberOfStaticWidgets is
    // retained metadata only; no static membership is inferred from its value.
    // Authored OnTop supplies the explicit manager order exposed below.
    // English is borrowed only during parsing; resolved text is copied here.
    // Archive shared_ptrs must genuinely own their pointees, not no-op aliases.
    // On failure nothing is published and bind remains retryable.
    OriginalUiStaticTextPageStatus50KG bind(
        std::shared_ptr<const assets::OriginalUiArchive50KG> pageArchive,
        const std::string& pageMember,
        std::shared_ptr<const assets::OriginalUiArchive50KG> widgetsArchive,
        const std::string& widgetsMember,const assets::OriginalUiEnglish50KG* english,
        OriginalUiStaticTextPageDiagnostic50KG&) noexcept;
    bool bound() const noexcept;
    const assets::OriginalUiDat50KG* pageDat() const noexcept;
    const assets::OriginalUiDat50KG* widgetsDat() const noexcept;
    const assets::OriginalUiPage50KG* page() const noexcept;
    const OriginalUiNavigation50KG* navigation() const noexcept;
    // Traversal evidence only: ordinaryWidgets, then actual intervening manager
    // children, then onTopWidgets. A page's controls are reference/callback
    // lists, not widget ancestors. Caller must establish the full manager tree
    // and lifecycle before dispatching any update or render. No sweep occurs.
    const OriginalUiStaticTextManagerOrder50KG* managerWidgetOrder() const noexcept;
    const assets::OriginalUiOverlayText50KG* descriptor(std::uint16_t widgetId) const noexcept;
    const OriginalUiTextWidget50KG* textWidget(std::uint16_t widgetId) const noexcept;

    // Generated command order and each control's membership order are exact.
    // The complete batch is preflighted before any callback or navigation write.
    // A repeated widget or command produces another callback, never deduplication.
    // applyCommands also mirrors the control-state changes used by navigation's
    // command helper; it does not select group slots or attach controllers.
    OriginalUiStaticTextPageStatus50KG applyCommands(const OriginalUiCommands50KG&,
        OriginalUiStaticTextPageDiagnostic50KG&) noexcept;
    OriginalUiStaticTextPageStatus50KG enterGroup(std::uint16_t groupId,
        std::uint16_t slot,std::uint8_t controller,OriginalUiStaticTextPageDiagnostic50KG&) noexcept;
    OriginalUiStaticTextPageStatus50KG moveControl(std::uint16_t groupId,bool forward,
        OriginalUiStaticTextPageDiagnostic50KG&) noexcept;
    OriginalUiStaticTextPageStatus50KG enableControl(std::uint16_t controlId,bool enabled,
        OriginalUiStaticTextPageDiagnostic50KG&) noexcept;
    // Calls only the original MAIN dispatch leaf. Caller must establish that
    // this is its original main page. It never infers routes from a filename,
    // executes a decision, or enables a backend capability on the caller's behalf.
    // The exact decision is published for ok/noChange/unavailable only.
    OriginalUiStaticTextPageStatus50KG dispatchMain(std::uint8_t controller,
        OriginalUiMainInput50KG,const OriginalUiMainContext50KG&,
        OriginalUiMainDecision50KG&,OriginalUiStaticTextPageDiagnostic50KG&) noexcept;

    // Serial source entry points; no mutable runtime/descriptor access escapes.
    // Forward lock gates and unsupportedAnimation exactly as the text leaf does.
    OriginalUiStaticTextPageStatus50KG setupLine(std::uint16_t widgetId,std::size_t line,
        const OriginalUiTextLineSetup50KG&,OriginalUiStaticTextPageDiagnostic50KG&) noexcept;
    OriginalUiStaticTextPageStatus50KG setLineColor(std::uint16_t widgetId,std::size_t line,
        std::uint32_t color,OriginalUiStaticTextPageDiagnostic50KG&) noexcept;
    OriginalUiStaticTextPageStatus50KG setLineAlpha(std::uint16_t widgetId,std::size_t line,
        std::int32_t alpha,float duration,OriginalUiStaticTextPageDiagnostic50KG&) noexcept;
    OriginalUiStaticTextPageStatus50KG stopLine(std::uint16_t widgetId,std::size_t line,
        OriginalUiStaticTextPageDiagnostic50KG&) noexcept;

    // Caller supplies proven manager/entity ancestry and source update/draw
    // point. Widget visibility is not gated by control/page "shown" metadata:
    // original widgets are manager siblings of pages. Use managerWidgetOrder
    // only with the established intervening tree. No implicit sweep or clock.
    OriginalUiStaticTextPageStatus50KG updateStatic(std::uint16_t widgetId,
        bool ancestorsShown,OriginalUiStaticTextPageDiagnostic50KG&) noexcept;
    OriginalUiStaticTextPageStatus50KG positionLine(std::uint16_t widgetId,std::size_t line,
        const OriginalUiStaticTextMetrics50KG&,OriginalUiTextPosition50KG&,
        OriginalUiStaticTextPageDiagnostic50KG&) const noexcept;
    // Computes position before preparing the packet. Failure/hidden leaves
    // output and last-rendered state untouched. NO implicit updateStatic call.
    // A successful projection records source packet preparation, not GPU draw.
    // Borrowed descriptor/text pointers remain valid only while this owner lives.
    OriginalUiStaticTextPageStatus50KG projectLine(std::uint16_t widgetId,std::size_t line,
        const OriginalUiStaticTextMetrics50KG&,bool ancestorsShown,
        OriginalUiStaticTextProjection50KG&,OriginalUiStaticTextPageDiagnostic50KG&) noexcept;

private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
    OriginalUiStaticTextPageStatus50KG preflight(const OriginalUiCommands50KG&,
        OriginalUiStaticTextPageDiagnostic50KG&) const noexcept;
    void commitCommands(const OriginalUiCommands50KG&) noexcept;
    OriginalUiStaticTextPageStatus50KG finishNavigation(OriginalUiNavigation50KG&,
        const OriginalUiCommands50KG&,OriginalUiNavigationStatus50KG,
        OriginalUiStaticTextPageDiagnostic50KG&) noexcept;
    OriginalUiTextWidget50KG* mutableWidget(std::uint16_t,
        OriginalUiStaticTextPageDiagnostic50KG&) noexcept;
};
}
