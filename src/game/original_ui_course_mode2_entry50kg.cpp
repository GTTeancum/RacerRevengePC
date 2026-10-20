#include "original_ui_course_mode2_entry50kg.h"
namespace rr::game {
OriginalUiCourseSelectionStatus50KG planOriginalUiCourseMode2Entry50KG(
    const OriginalUiCourseSelectionData50KG& data,
    const OriginalUiCourseMode2EntryInput50KG& input,
    OriginalUiCourseMode2EntryPlan50KG& out) noexcept {
    using S=OriginalUiCourseSelectionStatus50KG;
    if(input.mode34!=2)return S::unsupportedMode;
    if(input.career100C0>=27)return S::invalidSelection;
    const auto* page=data.page();
    if(!page||page->groups.size()<2)return S::invalidPage;
    OriginalUiCourseMode2EntryPlan50KG next;
    auto slot=std::uint32_t(input.progress219C&31u)+1u; //1DA570..578
    if(slot>=14u)slot=13u; //1DA580..594
    next.courseSlot14E4=static_cast<std::uint16_t>(slot);
    next.subtypeSlot14F4=1; //1DA57C
    next.tier14E8=slot<5u?1u:(slot<9u?2u:3u); //1DA598..5C8
    if(!data.control(next.courseSlot14E4,next.selectedControl))return S::invalidSelection;
    //1DA5F4 masks the ACTUAL selected control's planet before the bit write.
    next.planetProfileChange={OriginalUiProfileField50KG::rider2Bits1125E,
        OriginalUiProfileWrite50KG::maskedByte,(next.selectedControl.planet40&7u)<<5,
        0xe0u,0x1da618u};
    out=next;return S::ok;
}
}
