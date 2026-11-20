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
    // Mode-2 observations from the SAME selected live profile; never mode-0 remembered values.
    std::uint32_t career100C0=0xffffffffu,courseSlot14E4=0,tier14E8=0;
    std::uint8_t progress219C=0;
    std::array<std::uint8_t,13> medals1938{};
    // Actual selected profile facts, not a blanket native capability flag.
    std::uint32_t selectedRider=0xffffffffu;
    std::uint8_t selectedUnlock10FAC=0;
    bool nativeBetaStorage=false;
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
// Page-local source writes only. Remembered profile fields are committed by
// the genuine Back/Confirm transaction, never by these Left/Right callbacks.
struct OriginalUiCourseLapMove50KG {
    std::uint32_t laps14EC=0,sourcePC=0,audioPC=0,effectId=0;
    float repeatRight14D4=0,repeatLeft14D8=0;
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
    OriginalUiCourseWidgetControllerStatus50KG moveEventType(const OriginalUiNavigation50KG&,bool forward,
        const OriginalUiCourseWidgetProfile50KG&,OriginalUiCourseWidgetPlan50KG&)const noexcept;
    // Exact mode0 event-dependent row cycle from 1DF674/1DF810. Group3 is an
    // authenticated empty-control group with real association callbacks.
    OriginalUiCourseWidgetControllerStatus50KG moveRow(const OriginalUiNavigation50KG&,bool down,
        const OriginalUiCourseWidgetProfile50KG&,OriginalUiCourseWidgetPlan50KG&)const noexcept;
    OriginalUiCourseWidgetControllerStatus50KG moveLaps(const OriginalUiNavigation50KG&,std::uint32_t laps,bool forward,
        const OriginalUiCourseWidgetProfile50KG&,OriginalUiCourseLapMove50KG&)const noexcept;
    OriginalUiCourseWidgetControllerStatus50KG phase(const OriginalUiNavigation50KG&,OriginalUiCourseWidgetPhase50KG,
        const OriginalUiCourseWidgetProfile50KG&,OriginalUiCourseWidgetPlan50KG&)const noexcept;
private:struct Storage;std::unique_ptr<Storage> storage_;
};
}
