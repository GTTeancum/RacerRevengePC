#pragma once
#include "original_ui_tournament_widget_resources50kg.h"
#include "../../../src/game/original_ui_text_widget50kg.h"
#include "../../../src/game/original_ui_options_menu_controller50kg.h"
namespace rr::platform {
struct OriginalUiTournamentWidgetState50KG {
 OriginalUiTournamentWidgetKind50KG kind=OriginalUiTournamentWidgetKind50KG::absent;
 bool selected=false,shown=false,onTop=false;
};
struct OriginalUiTournamentTextProjection50KG {const assets::OriginalUiOverlayText50KG* descriptor=nullptr;game::OriginalUiTextPosition50KG position{};game::OriginalUiTextLineProjection50KG line{};};
struct OriginalUiTournamentIconProjection50KG {const assets::OriginalUiIcon50KG* descriptor=nullptr;const assets::OriginalUiScreenAssets50KG* images=nullptr;std::uint32_t imageIndex=0;game::OriginalUiIconLayout50KG layout{};};
struct OriginalUiTournamentWidgetHooks50KG {
 void* context=nullptr;
 bool (*preflight)(void*,const game::OriginalUiOptionsMenuCallback50KG*,std::size_t) noexcept=nullptr;
 bool (*pod)(void*,const game::OriginalUiOptionsMenuCallback50KG&,OriginalUiTournamentWidgetState50KG&) noexcept=nullptr;
};
enum class OriginalUiTournamentWidgetsError50KG:std::uint8_t {none,owner,alreadyBound,resource,input,phase,text,callback,missingOwner,poisoned,allocation};
struct OriginalUiTournamentWidgetsDiagnostic50KG {
 OriginalUiTournamentWidgetsError50KG error=OriginalUiTournamentWidgetsError50KG::none;
 std::uint16_t widget=0;std::size_t callback=0;
 OriginalUiTournamentWidgetResourceDiagnostic50KG resource{};
 game::OriginalUiTextWidgetStatus50KG text=game::OriginalUiTextWidgetStatus50KG::ok;
};
// Concrete native widget state, not a page/session or live model issuer.
// Immutable authentic resources outlive callbacks and every draw borrow.
// Dynamic text is populated by source career entry BEFORE that text widget's first callback; the selected pod callback legitimately precedes it.
// A later update/reentry requires a newly constructed fresh-page owner.
class OriginalUiTournamentWidgets50KG final {
public:
 OriginalUiTournamentWidgets50KG() noexcept;~OriginalUiTournamentWidgets50KG();
 OriginalUiTournamentWidgets50KG(const OriginalUiTournamentWidgets50KG&)=delete;
 OriginalUiTournamentWidgets50KG& operator=(const OriginalUiTournamentWidgets50KG&)=delete;
 bool bind(std::shared_ptr<const OriginalUiTournamentWidgetResources50KG>,OriginalUiTournamentWidgetsDiagnostic50KG&) noexcept;
 // Immutable preparation may precede source effects; publish is allocation-free
 // at the real SetText boundary after the initial selected-pod callback.
 bool prepareDynamic(std::uint16_t,const std::vector<std::string>&,OriginalUiTournamentWidgetsDiagnostic50KG&) noexcept;
 bool publishDynamic(std::uint16_t,OriginalUiTournamentWidgetsDiagnostic50KG&) noexcept;
 bool populateDynamic(std::uint16_t,const std::vector<std::string>&,OriginalUiTournamentWidgetsDiagnostic50KG&) noexcept;
 bool apply(const game::OriginalUiOptionsMenuCallback50KG*,std::size_t,const OriginalUiTournamentWidgetHooks50KG&,OriginalUiTournamentWidgetsDiagnostic50KG&) noexcept;
 OriginalUiTournamentWidgetState50KG state(std::uint16_t) const noexcept;
 const game::OriginalUiTextWidget50KG* text(std::uint16_t) const noexcept;
 const game::OriginalUiTextGeometry50KG* textGeometry(std::uint16_t) const noexcept;
 const std::vector<std::uint16_t>* order(bool onTop) const noexcept;
 const assets::OriginalUiOverlayText50KG* textDescriptor(std::uint16_t) const noexcept;
 bool updateText(std::uint16_t,bool ancestorsShown,OriginalUiTournamentWidgetsDiagnostic50KG&) noexcept;
 bool projectText(std::uint16_t,std::size_t,float fontHeight,std::int32_t width,std::int32_t height,bool ancestorsShown,OriginalUiTournamentTextProjection50KG&,OriginalUiTournamentWidgetsDiagnostic50KG&) noexcept;
 bool projectIcon(std::uint16_t,bool ancestorsShown,OriginalUiTournamentIconProjection50KG&,OriginalUiTournamentWidgetsDiagnostic50KG&) const noexcept;
 bool setColor(std::uint16_t,std::uint32_t,OriginalUiTournamentWidgetsDiagnostic50KG&) noexcept;
 bool poisoned() const noexcept;
 bool dynamicComplete() const noexcept;
private:struct Storage;std::unique_ptr<Storage> storage_;
};
}
