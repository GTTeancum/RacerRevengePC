#pragma once
#include "original_ui_navigation50kg.h"
#include <array>
#include <memory>
namespace rr::game {
enum class OriginalUiCourseWidgetPhase50KG : std::uint8_t {requestedPage90,previousPage94,entry74,openingAC,openedB0,closingB4,closedB8};
enum class OriginalUiCourseWidgetControllerStatus50KG :std::uint8_t {ok,notBound,alreadyBound,archive,parse,page,unsupportedProfile,invalidNavigation,capacity,allocation};
struct OriginalUiCourseWidgetProfile50KG {
    std::uint32_t mode34=0,course700=0,subtype70C=0;
    std::uint8_t flags100BC=0,flags11018=0,flags1125C=0;
    std::array<std::uint8_t,13> courseRecords100C4{};
};
enum class OriginalUiCourseWidgetStepKind50KG :std::uint8_t {widget,audioStartLoop11,audioFade1,promptMask42C};
struct OriginalUiCourseWidgetStep50KG {
    OriginalUiCourseWidgetStepKind50KG kind=OriginalUiCourseWidgetStepKind50KG::widget;
    std::uint16_t widget=0;
    OriginalUiControlOperation50KG operation=OriginalUiControlOperation50KG::hide;
    bool value=false;std::uint32_t sourcePC=0,argument=0;
};
struct OriginalUiCourseWidgetPlan50KG {
    OriginalUiNavigation50KG navigation{};
    std::array<OriginalUiCourseWidgetStep50KG,512> steps{};std::size_t count=0;
    bool writesPromptMask42C=false;std::uint32_t promptMask42C=0;
};
class OriginalUiCourseWidgetController50KG final {
public:
    OriginalUiCourseWidgetController50KG()noexcept;~OriginalUiCourseWidgetController50KG();
    OriginalUiCourseWidgetController50KG(const OriginalUiCourseWidgetController50KG&)=delete;
    OriginalUiCourseWidgetController50KG& operator=(const OriginalUiCourseWidgetController50KG&)=delete;
    OriginalUiCourseWidgetControllerStatus50KG bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,assets::OriginalUiPageDiagnostic50KG&)noexcept;
    const assets::OriginalUiPage50KG* page()const noexcept;
    const assets::OriginalUiDat50KG* widgetsDat()const noexcept;
    bool initialNavigation(OriginalUiNavigation50KG&)const noexcept;
    OriginalUiCourseWidgetControllerStatus50KG moveCourse(const OriginalUiNavigation50KG&,bool forward,
        const OriginalUiCourseWidgetProfile50KG&,OriginalUiCourseWidgetPlan50KG&)const noexcept;
    OriginalUiCourseWidgetControllerStatus50KG phase(const OriginalUiNavigation50KG&,OriginalUiCourseWidgetPhase50KG,
        const OriginalUiCourseWidgetProfile50KG&,OriginalUiCourseWidgetPlan50KG&)const noexcept;
private:struct Storage;std::unique_ptr<Storage> storage_;
};
}
