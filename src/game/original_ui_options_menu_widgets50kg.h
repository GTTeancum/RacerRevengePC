#pragma once
#include "original_ui_options_menu_controller50kg.h"
#include "original_ui_icon_layout50kg.h"
#include "original_ui_text_widget50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include <array>
#include <memory>
#include <vector>

namespace rr::game {
// Closed menu population projection, not the whole357-widget Options manager.
// All other pages remain unavailable. Widget3 is directly referenced by phases.
inline constexpr std::array<std::uint16_t,20> originalUiOptionsMenuWidgetIds50KG{{
    1,2,3,5,6,7,8,9,10,12,97,98,99,100,101,102,103,104,105,106}};
enum class OriginalUiOptionsWidgetKind50KG : std::uint8_t { absent,text,icon,movement };
enum class OriginalUiOptionsWidgetStatus50KG : std::uint8_t {
    ok,noChange,hidden,unavailable,notBound,alreadyBound,parseFailure,allocation,
    invalidWidget,invalidInput,invalidCommand,controllerFailure,textFailure,missingResource,alreadyPrepared,layoutFailure
};
struct OriginalUiOptionsWidgetDiagnostic50KG {
    OriginalUiOptionsWidgetStatus50KG status=OriginalUiOptionsWidgetStatus50KG::ok;
    OriginalUiOptionsMenuStatus50KG controller=OriginalUiOptionsMenuStatus50KG::ok;
    assets::OriginalUiPageDiagnostic50KG parse{};
    OriginalUiTextWidgetStatus50KG text=OriginalUiTextWidgetStatus50KG::ok;
    OriginalUiIconLayoutStatus50KG icon=OriginalUiIconLayoutStatus50KG::complete;
    std::uint16_t widgetId=0;
    std::size_t callbackIndex=0;
    std::uint32_t sourcePC=0;
};
struct OriginalUiOptionsWidgetState50KG {
    OriginalUiOptionsWidgetKind50KG kind=OriginalUiOptionsWidgetKind50KG::absent;
    bool selected=false,shown=false,onTop=false;
};
struct OriginalUiOptionsWidgetOrder50KG {
    // Relative order of admitted siblings only. Omitted Options widgets stay
    // source-hidden in this closed menu-only session. Never navigate another page.
    std::vector<std::uint16_t> ordinaryWidgets,onTopWidgets;
};
struct OriginalUiOptionsTextProjection50KG {
    const assets::OriginalUiOverlayText50KG* descriptor=nullptr;
    std::uint16_t widgetId=0;std::size_t lineIndex=0;
    OriginalUiTextPosition50KG position{};OriginalUiTextLineProjection50KG line{};
};
struct OriginalUiOptionsIconProjection50KG {
    const assets::OriginalUiIcon50KG* descriptor=nullptr;
    const assets::OriginalUiScreenAssets50KG* images=nullptr;
    std::uint32_t imageIndex=0;OriginalUiIconLayout50KG layout{};
};
class OriginalUiOptionsMenuWidgets50KG final {
public:
    OriginalUiOptionsMenuWidgets50KG() noexcept;
    ~OriginalUiOptionsMenuWidgets50KG();
    OriginalUiOptionsMenuWidgets50KG(const OriginalUiOptionsMenuWidgets50KG&)=delete;
    OriginalUiOptionsMenuWidgets50KG& operator=(const OriginalUiOptionsMenuWidgets50KG&)=delete;
    OriginalUiOptionsWidgetStatus50KG bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const assets::OriginalUiEnglish50KG*,OriginalUiOptionsWidgetDiagnostic50KG&) noexcept;
    bool bound() const noexcept;
    const assets::OriginalUiPage50KG* page() const noexcept;
    const OriginalUiNavigation50KG* navigation() const noexcept;
    const OriginalUiOptionsWidgetOrder50KG* order() const noexcept;
    OriginalUiOptionsWidgetState50KG widgetState(std::uint16_t) const noexcept;
    const assets::OriginalUiOverlayText50KG* textDescriptor(std::uint16_t) const noexcept;
    const assets::OriginalUiIcon50KG* iconDescriptor(std::uint16_t) const noexcept;
    const OriginalUiTextWidget50KG* textWidget(std::uint16_t) const noexcept;
    std::uint32_t pageInputFlags42C() const noexcept;
    // All command validation precedes first callback. Phase success certifies
    // only native widget/navigation effects, NOT camera/popup/audio/GPU effects.
    OriginalUiOptionsWidgetStatus50KG phase(OriginalUiOptionsMenuPhase50KG,
        bool originalProfileKnown,std::uint16_t savedSlot6E4,OriginalUiOptionsWidgetDiagnostic50KG&) noexcept;
    // Page first, then the actual widget5 selection snapshot and one registered
    // movement callback. Route/profile/audio results are requests for caller.
    OriginalUiOptionsWidgetStatus50KG input(std::uint8_t,assets::OriginalUiMovementInput50KG,
        bool containerReached,bool pageInputEnabled268,const OriginalUiOptionsContext50KG&,
        OriginalUiOptionsMenuPageResult50KG&,OriginalUiMovementNotification50KG&,
        OriginalUiOptionsWidgetDiagnostic50KG&) noexcept;
    // Concrete session effect boundary: page handler requests are consumed
    // before the selected Movement_Control5 snapshot/delivery. False stops
    // traversal and must be treated as a source-owner failure, never retried.
    OriginalUiOptionsWidgetStatus50KG inputWithPageEffects(std::uint8_t,assets::OriginalUiMovementInput50KG,
        bool containerReached,bool pageInputEnabled268,const OriginalUiOptionsContext50KG&,
        void* effectsContext,bool (*pageEffects)(void*,const OriginalUiOptionsMenuPageResult50KG&) noexcept,
        OriginalUiOptionsMenuPageResult50KG&,OriginalUiMovementNotification50KG&,
        OriginalUiOptionsWidgetDiagnostic50KG&) noexcept;
    OriginalUiOptionsWidgetStatus50KG updateText(std::uint16_t,bool ancestorsShown,
        OriginalUiOptionsWidgetDiagnostic50KG&) noexcept;
    OriginalUiOptionsWidgetStatus50KG projectText(std::uint16_t,std::size_t,float fontLineHeight,
        std::int32_t measuredWidth,std::int32_t measuredHeight,bool ancestorsShown,
        OriginalUiOptionsTextProjection50KG&,OriginalUiOptionsWidgetDiagnostic50KG&) noexcept;
    // Actual material-to-image association is the cold resource owner's duty.
    OriginalUiOptionsWidgetStatus50KG prepareIcon(std::uint16_t,
        std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,std::uint32_t imageIndex,
        std::int32_t depthMaximum,OriginalUiOptionsWidgetDiagnostic50KG&) noexcept;
    OriginalUiOptionsWidgetStatus50KG projectIcon(std::uint16_t,bool ancestorsShown,
        OriginalUiOptionsIconProjection50KG&,OriginalUiOptionsWidgetDiagnostic50KG&) const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
    OriginalUiOptionsWidgetStatus50KG commitPlan(const OriginalUiOptionsMenuPlan50KG&,
        OriginalUiOptionsWidgetDiagnostic50KG&) noexcept;
};
}
