#pragma once
#include "original_ui_course_widget_controller50kg.h"
#include "original_ui_icon_layout50kg.h"
#include "original_ui_text_widget50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include "../assets/original_ui_movement_control50kg.h"
#include <memory>
#include <vector>
namespace rr::game {
enum class OriginalUiCourseWidgetKind50KG : std::uint8_t {absent,empty,text,icon,selectableIcon,movement,character};
enum class OriginalUiCourseWidgetError50KG : std::uint8_t {none,notBound,alreadyBound,archive,parse,population,input,callback,missingOwner,poisoned,text,icon,missingResource,hidden,allocation,controller};
struct OriginalUiCourseWidgetDiagnostic50KG {
    OriginalUiCourseWidgetError50KG error=OriginalUiCourseWidgetError50KG::none;
    assets::OriginalUiPageDiagnostic50KG parse{};
    OriginalUiCourseWidgetControllerStatus50KG controller=OriginalUiCourseWidgetControllerStatus50KG::ok;
    std::uint16_t widget=0;std::size_t callback=0;
};
struct OriginalUiCourseWidgetState50KG {OriginalUiCourseWidgetKind50KG kind=OriginalUiCourseWidgetKind50KG::absent;bool selected=false,shown=false,onTop=false;};
// Synchronous real callbacks. Audio may be suppressed only by genuine muted application mode.
struct OriginalUiCourseWidgetHooks50KG {
    void* context=nullptr;
    bool (*preflight)(void*,const OriginalUiCourseWidgetStep50KG*,std::size_t)noexcept=nullptr;
    bool (*character)(void*,const OriginalUiCourseWidgetStep50KG&,OriginalUiCourseWidgetState50KG&)noexcept=nullptr;
    bool (*audio)(void*,const OriginalUiCourseWidgetStep50KG&)noexcept=nullptr;
};
struct OriginalUiCourseTextProjection50KG {const assets::OriginalUiOverlayText50KG* descriptor=nullptr;OriginalUiTextPosition50KG position{};OriginalUiTextLineProjection50KG line{};};
struct OriginalUiCourseIconProjection50KG {const assets::OriginalUiIcon50KG* descriptor=nullptr;const assets::OriginalUiScreenAssets50KG* images=nullptr;std::uint32_t imageIndex=0;OriginalUiIconLayout50KG layout{};};
struct OriginalUiCoursePopupTextClone50KG {
    std::uint16_t sourceWidget=0;
    const assets::OriginalUiMemberIdentity50KG* materialIdentity=nullptr;
    const assets::OriginalUiFont50KG* font=nullptr;
    const assets::Texture48B* atlas=nullptr;
    std::string text;
    float x10=0,y14=0;
    assets::OriginalUiTextJustification50KG justification50=assets::OriginalUiTextJustification50KG::left;
    std::uint32_t color54=0;
    std::uint8_t capacity5B=0,characterCount5C=0,allocatedPackets5D=0,activePackets5E=0;
};
class OriginalUiCourseWidgets50KG final {
public:
    OriginalUiCourseWidgets50KG()noexcept;~OriginalUiCourseWidgets50KG();
    OriginalUiCourseWidgets50KG(const OriginalUiCourseWidgets50KG&)=delete;
    OriginalUiCourseWidgets50KG& operator=(const OriginalUiCourseWidgets50KG&)=delete;
    bool bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,const assets::OriginalUiEnglish50KG*,OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    const OriginalUiCourseWidgetController50KG* controller()const noexcept;
    const OriginalUiNavigation50KG* navigation()const noexcept;
    const std::vector<std::uint16_t>* order(bool onTop)const noexcept;
    OriginalUiCourseWidgetState50KG state(std::uint16_t)const noexcept;
    const assets::OriginalUiOverlayText50KG* textDescriptor(std::uint16_t)const noexcept;
    const assets::OriginalUiIcon50KG* iconDescriptor(std::uint16_t,bool selected=false)const noexcept;
    bool poisoned()const noexcept;std::uint32_t promptMask42C()const noexcept;
    //1FF020/2037D0 COPY nine template ScreenTexts. They are not reparented.
    bool preparePopupClones438(std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,
        const std::array<std::uint32_t,9>& fontImages,OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    bool disableFreshPopup438(OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    bool popupEnabled438()const noexcept;
    bool popupTemplatesHidden438()const noexcept;
    const std::array<std::vector<OriginalUiCoursePopupTextClone50KG>,9>* popupClones438()const noexcept;
    bool moveCourse(bool forward,const OriginalUiCourseWidgetProfile50KG&,const OriginalUiCourseWidgetHooks50KG&,OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    bool moveRow(bool down,const OriginalUiCourseWidgetProfile50KG&,const OriginalUiCourseWidgetHooks50KG&,OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    bool phase(OriginalUiCourseWidgetPhase50KG,const OriginalUiCourseWidgetProfile50KG&,const OriginalUiCourseWidgetHooks50KG&,OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    bool apply(const OriginalUiCourseWidgetStep50KG*,std::size_t,const OriginalUiCourseWidgetHooks50KG&,OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    const std::vector<std::string>* textLines(std::uint16_t)const noexcept;
    const std::array<std::int32_t,2>* textPosition(std::uint16_t)const noexcept;
    bool setDynamicLines(std::uint16_t,const std::vector<std::string>&,OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    bool setTextPosition(std::uint16_t,std::int32_t,std::int32_t,OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    bool setColor(std::uint16_t,std::uint32_t,OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    bool updateText(std::uint16_t,bool,OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    bool projectText(std::uint16_t,std::size_t,float,std::int32_t,std::int32_t,bool,OriginalUiCourseTextProjection50KG&,OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    bool prepareIcon(std::uint16_t,bool,std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,std::uint32_t,std::int32_t,OriginalUiCourseWidgetDiagnostic50KG&)noexcept;
    bool projectIcon(std::uint16_t,bool,OriginalUiCourseIconProjection50KG&,OriginalUiCourseWidgetDiagnostic50KG&)const noexcept;
private:struct Storage;std::unique_ptr<Storage> storage_;
};
}
