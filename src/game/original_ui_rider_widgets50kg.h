#pragma once
#include "original_ui_rider_selection50kg.h"
#include "original_ui_icon_layout50kg.h"
#include "original_ui_text_widget50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include "../assets/original_ui_movement_control50kg.h"
#include <memory>
#include <vector>
namespace rr::game {
enum class OriginalUiRiderWidgetKind50KG : std::uint8_t { absent,empty,text,icon,movement,pod,character };
enum class OriginalUiRiderWidgetError50KG : std::uint8_t {
    none,notBound,alreadyBound,archive,parse,population,input,callback,missingDynamicOwner,
    poisoned,text,icon,missingResource,hidden,allocation
};
struct OriginalUiRiderWidgetDiagnostic50KG {
    OriginalUiRiderWidgetError50KG error=OriginalUiRiderWidgetError50KG::none;
    assets::OriginalUiPageDiagnostic50KG parse{};
    OriginalUiRiderSelectionDiagnostic50KG selection{};
    std::uint16_t widget=0;std::size_t callback=0;
};
struct OriginalUiRiderWidgetState50KG {
    OriginalUiRiderWidgetKind50KG kind=OriginalUiRiderWidgetKind50KG::absent;
    bool selected=false,shown=false,onTop=false;
};
struct OriginalUiRiderWidgetCallback50KG {
    std::uint16_t widget=0;OriginalUiControlOperation50KG operation{};
    bool value=false;std::uint32_t sourcePC=0;
};
// Synchronous owner adapter, NOT an acknowledgement queue. Preflight must
// establish actual scene/assets/traversal capacity without publishing effects.
// apply must complete the actual dynamic callback and return observed state.
// Scene load/attachment/unload/selection cannot be replaced with true/flags.
struct OriginalUiRiderDynamicHooks50KG {
    void* context=nullptr;
    bool (*preflight)(void*,const OriginalUiRiderWidgetCallback50KG*,std::size_t) noexcept=nullptr;
    bool (*apply)(void*,const OriginalUiRiderWidgetCallback50KG&,
        OriginalUiRiderWidgetState50KG& actualState) noexcept=nullptr;
};
struct OriginalUiRiderMovementPlan50KG {
    OriginalUiNavigation50KG nextNavigation{};OriginalUiCommands50KG commands{};
    bool notifyManager=false;std::int32_t mapping=0;
    // Source manager virtual+84 request at2AB490. Caller resolves actual owner.
};
struct OriginalUiRiderTextProjection50KG {
    const assets::OriginalUiOverlayText50KG* descriptor=nullptr;
    OriginalUiTextPosition50KG position{};OriginalUiTextLineProjection50KG line{};
};
struct OriginalUiRiderIconProjection50KG {
    const assets::OriginalUiIcon50KG* descriptor=nullptr;
    const assets::OriginalUiScreenAssets50KG* images=nullptr;
    std::uint32_t imageIndex=0;OriginalUiIconLayout50KG layout{};
};
class OriginalUiRiderWidgets50KG final {
public:
    OriginalUiRiderWidgets50KG() noexcept;~OriginalUiRiderWidgets50KG();
    OriginalUiRiderWidgets50KG(const OriginalUiRiderWidgets50KG&)=delete;
    OriginalUiRiderWidgets50KG& operator=(const OriginalUiRiderWidgets50KG&)=delete;
    bool bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const assets::OriginalUiEnglish50KG*,OriginalUiRiderWidgetDiagnostic50KG&) noexcept;
    const OriginalUiRiderSelectionData50KG* selectionData()const noexcept;
    const OriginalUiNavigation50KG* navigation(std::uint8_t player)const noexcept;
    const std::vector<std::uint16_t>* order(bool onTop)const noexcept;
    OriginalUiRiderWidgetState50KG state(std::uint16_t)const noexcept;
    const assets::OriginalUiOverlayText50KG* textDescriptor(std::uint16_t)const noexcept;
    const assets::OriginalUiIcon50KG* iconDescriptor(std::uint16_t)const noexcept;
    bool poisoned()const noexcept;
    // Read-only measurement input; never project a stateful text line twice.
    // Borrowed strings remain valid until setDynamicLines or owner destruction.
    const std::vector<std::string>* textLines(std::uint16_t)const noexcept;
    const std::array<std::int32_t,2>* textPosition(std::uint16_t)const noexcept;
    // Expands actual control memberships, preserves duplicates, applies real
    // dynamic callbacks in order; only then publishes next navigation. It does
    // NOT execute page appearance/lighting/popup/sound/transform side effects.
    bool commitNavigation(std::uint8_t player,const OriginalUiNavigation50KG& next,const OriginalUiCommands50KG&,
        const OriginalUiRiderDynamicHooks50KG&,OriginalUiRiderWidgetDiagnostic50KG&) noexcept;
    // For source direct widget calls, e.g. appearance2A7A00. Caller owns phase order.
    bool apply(const OriginalUiRiderWidgetCallback50KG*,std::size_t,
        const OriginalUiRiderDynamicHooks50KG&,OriginalUiRiderWidgetDiagnostic50KG&) noexcept;
    bool planMovement(std::uint8_t player,std::uint8_t controller,assets::OriginalUiMovementInput50KG,
        bool containerReached,bool pageEnabled,bool pageAccepted,OriginalUiRiderMovementPlan50KG&)const noexcept;
    // Dynamic contents must come from the original caller/localization/profile
    // formatting. Drain borrowed projections before replacing live strings.
    bool setDynamicLines(std::uint16_t,const std::vector<std::string>&,OriginalUiRiderWidgetDiagnostic50KG&) noexcept;
    bool setTextPosition(std::uint16_t,std::int32_t,std::int32_t,OriginalUiRiderWidgetDiagnostic50KG&) noexcept;
    bool setColor(std::uint16_t,std::uint32_t,OriginalUiRiderWidgetDiagnostic50KG&) noexcept;
    bool updateText(std::uint16_t,bool ancestorsShown,OriginalUiRiderWidgetDiagnostic50KG&) noexcept;
    bool projectText(std::uint16_t,std::size_t,float fontHeight,std::int32_t width,std::int32_t height,
        bool ancestorsShown,OriginalUiRiderTextProjection50KG&,OriginalUiRiderWidgetDiagnostic50KG&) noexcept;
    bool prepareIcon(std::uint16_t,std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,
        std::uint32_t image,std::int32_t depthMaximum,OriginalUiRiderWidgetDiagnostic50KG&) noexcept;
    bool projectIcon(std::uint16_t,bool ancestorsShown,OriginalUiRiderIconProjection50KG&,
        OriginalUiRiderWidgetDiagnostic50KG&)const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
