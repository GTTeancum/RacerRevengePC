// New reconstruction after workspace rollback; source-only and unverified.
#pragma once
#include "original_ui_route_decisions50kg.h"
#include <array>
#include <memory>
namespace rr::game {
enum class OriginalUiCourseSelectionStatus50KG : std::uint8_t {
    ok,alreadyLoaded,invalidArchive,invalidPage,parseFailure,allocation,missingProfile,unsupportedMode,invalidSelection
};
struct OriginalUiCourseSelectionDiagnostic50KG {
    OriginalUiCourseSelectionStatus50KG status=OriginalUiCourseSelectionStatus50KG::ok;
    assets::OriginalUiPageDiagnostic50KG parse{};std::uint32_t sourcePC=0;
};
struct OriginalUiCourseControl50KG {std::uint16_t controlId=0;std::uint32_t planet40=0,course44=0;};
class OriginalUiCourseSelectionData50KG final {
public:
    OriginalUiCourseSelectionData50KG()=default;
    OriginalUiCourseSelectionData50KG(const OriginalUiCourseSelectionData50KG&)=delete;
    OriginalUiCourseSelectionData50KG& operator=(const OriginalUiCourseSelectionData50KG&)=delete;
    bool load(std::shared_ptr<const assets::OriginalUiArchive50KG>,OriginalUiCourseSelectionDiagnostic50KG&)noexcept;
    const assets::OriginalUiPage50KG* page()const noexcept{return archive_?&page_:nullptr;}
    bool control(std::uint16_t oneBasedSlot,OriginalUiCourseControl50KG&)const noexcept;
    bool authoredCourseIndex(std::uint16_t oneBasedSlot,std::uint32_t&)const noexcept;
private:
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive_;assets::OriginalUiPage50KG page_{};
    std::array<std::int32_t,assets::originalUiControlLimit50KG> courseIndices_{};
    std::array<std::int8_t,assets::originalUiControlLimit50KG> planets_{};
};
struct OriginalUiCourseRememberedProfile50KG {
    bool ownerReady=false;std::uint32_t mode34=0;
    std::uint32_t course700=0,laps704=0,entrants708=0,subtype70C=0;
    std::uint32_t course710=0,entrants714=0,laps718=0;
    std::uint8_t packed1125E=0;bool cardFlag100BC=false;
    std::uint8_t availability11018=0;
    std::uint32_t planet0Primary79C=0,planet0Secondary7A0=0;
    std::array<std::uint8_t,13> courseOrder100C4{};
};
struct OriginalUiCourseEntryPrefix50KG {
    std::uint32_t promptMask42C=0;
    std::uint16_t courseSlot14E4=0,subtypeSlot14F4=0;
    std::uint32_t laps14EC=0,entrants14F0=0;
    OriginalUiCourseControl50KG selectedControl{};
    OriginalUiProfileChange50KG planetProfileChange{};
    bool showPopup438=false;std::uint32_t sourcePC=0x1DA4A0;
};
OriginalUiCourseSelectionStatus50KG planOriginalUiCourseEntryPrefix50KG(
    const OriginalUiCourseSelectionData50KG&,const OriginalUiCourseRememberedProfile50KG&,OriginalUiCourseEntryPrefix50KG&)noexcept;
}
