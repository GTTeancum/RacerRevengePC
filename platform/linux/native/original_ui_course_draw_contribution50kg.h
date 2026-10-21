#pragma once
#include "../../../src/game/original_application_animation_clock50kg.h"
#include "original_ui_course_draw_resources50kg.h"
#include "original_ui_course_hologram_draw50kg.h"
#include "original_ui_native_draw50kg.h"
#include "../../../src/game/original_ui_course_render_context50kg.h"
#include "../../../src/game/original_ui_course_lighting50kg.h"
#include "../../../src/game/original_ui_course_page_draw50kg.h"
#include "../../../src/game/original_ui_aperture_owner50kg.h"
namespace rr::platform {
struct OriginalUiCourseDrawClock50KG {bool known=false,advanceAnimations=false;std::uint32_t milliseconds=0;
    game::OriginalApplicationAnimationClockView50KG applicationAnimation{};};
enum class OriginalUiCourseContributionError50KG : std::uint8_t {none,input,alreadyBound,resource,bank,publication,phase,clock,widget,model,geometry,material,schedule,capacity,allocation,unfinished};
struct OriginalUiCourseContributionDiagnostic50KG {
 OriginalUiCourseContributionError50KG error=OriginalUiCourseContributionError50KG::none;
 std::uint16_t widget=0;std::uint32_t sourcePC=0;
 assets::OriginalUiMaterialDiagnostic50KG materialDetail{};
 game::OriginalUiCourseWidgetDiagnostic50KG widgetDetail{};
 OriginalUiCourseHologramDrawDiagnostic50KG hologram{};
 game::OriginalUiTextDiagnostic50KG text{};
};
// Original content and layout with explicit native RGB material token/pass/FIFO
// ordering, global Course-light adaptation, and raw background clamp-to-edge.
// This does not mint the Rider-only source-RGBA seal or emulate guest GS/VU.
class OriginalUiCourseDrawContribution50KG final {
public:
 OriginalUiCourseDrawContribution50KG()noexcept;~OriginalUiCourseDrawContribution50KG();
 bool bind(std::shared_ptr<OriginalUiCourseDrawResources50KG>,std::shared_ptr<OriginalUiCourseHologramDraw50KG>,OriginalUiCourseContributionDiagnostic50KG&,
    const game::OriginalApplicationAnimationClockView50KG& tournamentClock={})noexcept;
 bool prepareBank(OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 std::shared_ptr<const assets::OriginalUiImageBank50KG> bank()const noexcept;
 const std::vector<MaterialTextureSampler50EB>* samplers()const noexcept;
 bool bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB>,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool begin(const OriginalUiCourseDrawClock50KG&,const game::OriginalUiCourseRenderContext50KG&,const game::OriginalUiCourseLighting50KG&,OriginalUiCourseContributionDiagnostic50KG&,std::uint32_t planet=0)noexcept;
 bool projectOrdinaryWidgets(bool,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool projectPage(game::OriginalUiCoursePageDraw50KG&,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool projectExitText(const game::OriginalUiApertureHook50KG&,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool projectAperture(std::uint8_t,const game::OriginalUiApertureRectangle50KG&,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool projectPrompts(std::uint32_t,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool projectOnTopWidgets(bool,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool finish(OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 const OriginalUiNativeDraw50KG* draw()const noexcept;
 void drain()noexcept;
private:
 bool widgets(bool,bool,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool model(std::uint16_t,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool append(const game::OriginalUiOverlayQuad50KG&,std::uint32_t,float,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool appendVertices(const std::vector<assets::Vertex48B>&,std::uint32_t,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 struct Storage;std::unique_ptr<Storage> storage_;
};
}
