#pragma once
#include "original_ui_options_menu_controller50kg.h"
#include "original_ui_icon_layout50kg.h"
#include "original_ui_text_widget50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include <memory>
#include <vector>
namespace rr::game {
inline constexpr std::size_t originalUiHallWidgetStorageCount50KG=31; // exact30 + sentinel
inline constexpr std::array<std::uint16_t,13> originalUiHallCourseWidgets50KG{{9,13,14,10,12,15,20,16,25,26,22,23,21}};
enum class OriginalUiHallWidgetKind50KG:std::uint8_t {absent,empty,text,icon,movement};
enum class OriginalUiHallWidgetError50KG:std::uint8_t {none,notBound,alreadyBound,archive,parse,population,input,callback,text,icon,missingResource,hidden,allocation};
struct OriginalUiHallWidgetDiagnostic50KG {OriginalUiHallWidgetError50KG error=OriginalUiHallWidgetError50KG::none;assets::OriginalUiPageDiagnostic50KG parse{};std::uint16_t widget=0;std::size_t callback=0;std::uint32_t sourcePC=0;};
struct OriginalUiHallWidgetState50KG {OriginalUiHallWidgetKind50KG kind=OriginalUiHallWidgetKind50KG::absent;bool selected=false,shown=false,onTop=false;};
struct OriginalUiHallTextProjection50KG {const assets::OriginalUiOverlayText50KG* descriptor=nullptr;OriginalUiTextPosition50KG position{};OriginalUiTextLineProjection50KG line{};};
struct OriginalUiHallIconProjection50KG {const assets::OriginalUiIcon50KG* descriptor=nullptr;const assets::OriginalUiScreenAssets50KG* images=nullptr;std::uint32_t imageIndex=0;OriginalUiIconLayout50KG layout{};};
// Authenticated UIHOF widget manager. The session/controller owns navigation;
// callback order, including duplicates, is preserved without inventing rows.
class OriginalUiHallWidgets50KG final {
public:
 OriginalUiHallWidgets50KG()noexcept;~OriginalUiHallWidgets50KG();
 OriginalUiHallWidgets50KG(const OriginalUiHallWidgets50KG&)=delete;OriginalUiHallWidgets50KG& operator=(const OriginalUiHallWidgets50KG&)=delete;
 bool bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,const assets::OriginalUiEnglish50KG*,OriginalUiHallWidgetDiagnostic50KG&)noexcept;
 const assets::OriginalUiPage50KG* page()const noexcept;
 const assets::OriginalUiDat50KG* widgetsDat()const noexcept;
 const assets::OriginalUiMovementControl50KG* movement()const noexcept;
 const std::vector<std::uint16_t>* order(bool onTop)const noexcept;
 OriginalUiHallWidgetState50KG state(std::uint16_t)const noexcept;
 const assets::OriginalUiOverlayText50KG* textDescriptor(std::uint16_t)const noexcept;
 const assets::OriginalUiIcon50KG* iconDescriptor(std::uint16_t)const noexcept;
 const OriginalUiTextWidget50KG* textWidget(std::uint16_t)const noexcept;
 bool preflight(const OriginalUiOptionsMenuCallback50KG*,std::size_t,OriginalUiHallWidgetDiagnostic50KG&)const noexcept;
 bool apply(const OriginalUiOptionsMenuCallback50KG*,std::size_t,OriginalUiHallWidgetDiagnostic50KG&)noexcept;
 bool setWidgetColor(std::uint16_t,std::uint32_t,OriginalUiHallWidgetDiagnostic50KG&)noexcept;
 bool updateText(std::uint16_t,bool,OriginalUiHallWidgetDiagnostic50KG&)noexcept;
 bool projectText(std::uint16_t,std::size_t,float,std::int32_t,std::int32_t,bool,OriginalUiHallTextProjection50KG&,OriginalUiHallWidgetDiagnostic50KG&)noexcept;
 bool prepareIcon(std::uint16_t,std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,std::uint32_t,std::int32_t,OriginalUiHallWidgetDiagnostic50KG&)noexcept;
 bool projectIcon(std::uint16_t,bool,OriginalUiHallIconProjection50KG&,OriginalUiHallWidgetDiagnostic50KG&)const noexcept;
private:struct Storage;std::unique_ptr<Storage> storage_;
};
}
