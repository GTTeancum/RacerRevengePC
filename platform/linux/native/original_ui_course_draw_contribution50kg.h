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
// ---- PORT EXTENSION, NOT ORIGINAL BEHAVIOUR (DLC add-on courses, 146r-U) ----------
// Loose package images of the add-on Course-screen entries: each listed preview
// (same PST shape as the template's ui/textures/holo_<template>.pst, checked at
// startup) and each package course_background (640x448 one-level layout). Read
// once at Course bind from the admitted dlc:/ paths; one port-owned identity per
// file; joined to the Course bank as its third owner (loose members). Stock: none.
struct OriginalUiCourseAddonImages50DL {
 struct File {std::string path;std::vector<std::uint8_t> bytes;std::string name;
  assets::OriginalUiMemberIdentity50KG identity{};bool background=false;std::uint16_t templateModel=0;};
 std::vector<std::unique_ptr<File>> files;                  // stable identity/byte addresses
 std::vector<assets::OriginalUiMemberView50KG> views;       // files order
 std::shared_ptr<assets::OriginalUiScreenAssets50KG> images; // one image per file, files order
 int image(const std::string& path,bool background)const noexcept; // -1 = not loaded
};
struct OriginalUiCourseAddonImageRequest50DL {std::string path;bool background=false;std::uint16_t templateModel=0;};
// Reads and decodes every requested file (duplicates once). False with why:
// nothing usable (the entries then draw the template's own images, logged).
bool loadOriginalUiCourseAddonImages50DL(const std::vector<OriginalUiCourseAddonImageRequest50DL>&,
    std::shared_ptr<OriginalUiCourseAddonImages50DL>&,std::string& why)noexcept;
// One collector frame while an add-on entry is highlighted. Null = stock frame.
struct OriginalUiCourseAddonFrame50DL {
 std::uint16_t anchorModel=0,templateModel=0; // draw templateModel where anchorModel's hologram is drawn
 std::int32_t previewImage=-1;                // add-on image replacing the template's pass-0 holo texture
 std::int32_t backgroundImage=-1;             // add-on image of a package background (else begin's planet)
 std::uint16_t anchorName=0;const std::string* name=nullptr; // course-name text widget whose line 0 shows *name
 unsigned nameDrawn=0,nameSkipped=0;          // filled by the frame
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
 // PORT EXTENSION (DLC add-on courses): before prepareBank only. When the bank
 // cannot take them, prepareBank builds the stock bank and addonImagesAdmitted() is false.
 bool setAddonImages(std::shared_ptr<const OriginalUiCourseAddonImages50DL>)noexcept;
 bool addonImagesAdmitted()const noexcept;
 // Per frame, set before begin and cleared after finish (or a failed frame).
 void setAddonFrame(OriginalUiCourseAddonFrame50DL*)noexcept;
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
 bool model(std::uint16_t,OriginalUiCourseContributionDiagnostic50KG&,std::int32_t pass0AddonImage=-1)noexcept;
 bool addonName(std::uint16_t,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool prepareBankOnce(bool withAddon,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool append(const game::OriginalUiOverlayQuad50KG&,std::uint32_t,float,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 bool appendVertices(const std::vector<assets::Vertex48B>&,std::uint32_t,OriginalUiCourseContributionDiagnostic50KG&)noexcept;
 struct Storage;std::unique_ptr<Storage> storage_;
};
}
